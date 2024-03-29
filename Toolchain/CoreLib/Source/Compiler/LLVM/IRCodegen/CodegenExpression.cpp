/**
 *
 * Copyright (c)2022 The Adamnite C++ Authors.
 *
 * This code is open-sourced under the MIT license.
 */

#include "CodegenExpression.hpp"

#include <CoreLib/Types.hpp>

#if defined (__clang__)
#   pragma clang diagnostic push
#   pragma clang diagnostic ignored "-Wunused-parameter"
#elif defined(__GNUC__) || defined(__GNUG__)
#   pragma GCC diagnostic push
#   pragma GCC diagnostic ignored "-Wunused-parameter"
#endif

#include <llvm/IR/Verifier.h>

#if defined(__clang__)
#   pragma clang diagnostic pop
#elif defined(__GNUC__) || defined(__GNUG__)
#   pragma GCC diagnostic pop
#endif

#include <fmt/format.h>

namespace A1::LLVM::IR
{

namespace
{
    [[ nodiscard ]]
    llvm::Value * getPrintFormat( Context & ctx, llvm::Type const * type )
    {
        if ( type->isIntegerTy( sizeof( Number::Type ) * 8U ) )
        {
            static auto * format{ ctx.builder->CreateGlobalStringPtr( "%d\n", "numFormat", 0U, ctx.module_.get() ) };
            return format;
        }
        else if
        (
            type->isPointerTy() &&
            type->getNumContainedTypes() > 0 &&
            (
                type->getContainedType( 0U )->isArrayTy  () ||
                type->getContainedType( 0U )->isIntegerTy() ||
                type->getContainedType( 0U )->isPointerTy()
            )
        )
        {
            static auto * format{ ctx.builder->CreateGlobalStringPtr( "%s\n", "strFormat", 0U, ctx.module_.get() ) };
            return format;
        }
        return nullptr;
    }

    [[ nodiscard ]]
    llvm::Type * getType( Context & ctx, TypeID const typeID )
    {
             if ( typeID == Registry::getAddressHandle() ) { return llvm::Type::getInt8PtrTy ( *ctx.internalCtx ); }
        else if ( typeID == Registry::getBoolHandle   () ) { return llvm::Type::getInt8Ty    ( *ctx.internalCtx ); }
        else if ( typeID == Registry::getNumHandle    () ) { return llvm::Type::getInt64Ty   ( *ctx.internalCtx ); }
        else if ( typeID == Registry::getStrHandle    () ) { return llvm::Type::getInt8PtrTy ( *ctx.internalCtx ); }
        else if ( typeID == Registry::getI8Handle     () || typeID == Registry::getU8Handle () ) { return llvm::Type::getInt8Ty ( *ctx.internalCtx ); }
        else if ( typeID == Registry::getI16Handle    () || typeID == Registry::getU16Handle() ) { return llvm::Type::getInt16Ty( *ctx.internalCtx ); }
        else if ( typeID == Registry::getI32Handle    () || typeID == Registry::getU32Handle() ) { return llvm::Type::getInt32Ty( *ctx.internalCtx ); }
        else if ( typeID == Registry::getI64Handle    () || typeID == Registry::getU64Handle() ) { return llvm::Type::getInt64Ty( *ctx.internalCtx ); }

        return nullptr;
    }
} // namespace

llvm::Value * codegenAssign( Context & ctx, std::span< AST::Node::Pointer const > const nodes )
{
    ASSERTM( std::size( nodes ) == 2U, "Assignment expression consists of two operands" );

    auto * lhs{ codegen( ctx, nodes[ 0U ] ) };

    llvm::Value * value{ nullptr };
    if ( nodes[ 1U ]->is< StringLiteral >() )
    {
        value = codegen( ctx, nodes[ 1U ] );
    }
    else
    {
        auto * rhs{ codegen( ctx, nodes[ 1U ] ) };
        ctx.builder->CreateStore( rhs, lhs );
        return lhs;
    }

    if ( nodes[ 0U ]->is< Identifier >() )
    {
        ctx.symbols.variable( nodes[ 0U ]->get< Identifier >().name, value );
    }
    else
    {
        ctx.builder->CreateStore( value, lhs );
    }
    return value;
}

llvm::Value * codegenCall( Context & ctx, std::span< AST::Node::Pointer const > const nodes )
{
    ASSERTM( std::size( nodes ) >= 1U, "Call expression consists of identifier and parameters, if any" );

    ASSERTM( nodes[ 0U ]->is< Identifier >(), "Identifier is the first child node in the call expression" );
    auto const & name{ nodes[ 0U ]->get< Identifier >().name };

    if ( auto const & type{ ctx.symbols.contractTypes.find( name ) }; type != std::end( ctx.symbols.contractTypes ) )
    {
        auto * globalContract
        {
            new llvm::GlobalVariable
            (
                *ctx.module_,
                type->second.internalType,
                false, /* isConstant */
                llvm::GlobalVariable::ExternalLinkage,
                llvm::Constant::getNullValue( type->second.internalType )
            )
        };

        auto * initialContract{ ctx.builder->CreateCall( ctx.symbols.functions[ fmt::format( "{}____default_init__", name ) ], llvm::None ) };
        ctx.builder->CreateStore( initialContract, globalContract );

        if ( auto const ctor{ ctx.symbols.functions.find( fmt::format( "{}____init__", name ) ) }; ctor != std::end( ctx.symbols.functions ) )
        {
            ctx.builder->CreateCall( ctor->second, { globalContract } );
        }
        return globalContract;
    }
    else
    {
        // Create a function call

        std::vector< llvm::Value * > arguments;
        for ( auto i{ 1U }; i < std::size( nodes ); i++ )
        {
            auto * value{ codegen( ctx, nodes[ i ] ) };
            if ( value == nullptr ) { return nullptr; }
            arguments.push_back( Detail::load( ctx, value ) );
        }

        auto const & externalBuiltInFunctions{ ctx.symbols.externalBuiltInFunctions() };
        auto const & internalBuiltInFunctions{ ctx.symbols.internalBuiltInFunctions() };

        /**
         * At the moment, the only standard function is 'print' function
         * TODO: Improve handling of standard functions uniformly.
         */
        if ( name == "print" )
        {
            // Standard print function currently supports one argument only
            if ( std::size( arguments ) > 1U ) { return nullptr; }

            arguments.insert( std::begin( arguments ), getPrintFormat( ctx, arguments[ 0U ]->getType() ) );

            return ctx.builder->CreateCall( externalBuiltInFunctions.at( name ), arguments );
        }
        else if ( auto it{ externalBuiltInFunctions.find( name ) }; it != std::end( externalBuiltInFunctions ) )
        {
            return ctx.builder->CreateCall( externalBuiltInFunctions.at( name ), arguments );
        }
        else if ( auto it{ internalBuiltInFunctions.find( name ) }; it != std::end( internalBuiltInFunctions ) )
        {
            return ctx.builder->CreateCall( internalBuiltInFunctions.at( name ), arguments );
        }

        return ctx.builder->CreateCall( ctx.symbols.functions[ ctx.symbols.mangle( name ) ], arguments );
    }

    return nullptr;
}

llvm::Value * codegenMemberCall( Context & ctx, std::span< AST::Node::Pointer const > const nodes )
{
    ASSERTM( std::size( nodes ) == 2U, "Member call expression consists of two identifiers: variable and either data member or function member identifier" );

    ASSERTM( nodes[ 0U ]->is< Identifier >(), "Variable identifier is the first child node in the member call expression" );
    auto const & variableName{ nodes[ 0U ]->get< Identifier >().name };

    auto * variable{ ctx.symbols.variable( variableName ) };

    auto const contractTypeName
    {
        [ &contracts = ctx.symbols.contractTypes, type = variable->getType() ]() -> std::string
        {
            for ( auto const & contract : contracts )
            {
                if ( contract.second.internalType == type->getContainedType( 0U ) ) { return contract.first; }
            }
            return {};
        }()
    };

    auto const & member{ nodes[ 1U ] };
    if ( member->is< Identifier >() )
    {
        auto const & dataMemberTypes{ ctx.symbols.contractTypes[ contractTypeName ].dataMemberTypes };

        // Access data member
        return ctx.builder->CreateStructGEP
        (
            variable->getType()->getContainedType( 0U ),
            variable,
            dataMemberTypes.at( member->get< Identifier >().name ).index
        );
    }
    else if ( member->is< AST::NodeType >() && member->get< AST::NodeType >() == AST::NodeType::Call )
    {
        // Access function member
        auto const & memberNodes{ member->children() };

        ASSERTM( std::size( memberNodes ) >= 1U, "Call expression consists of identifier and parameters, if any" );

        ASSERTM( memberNodes[ 0U ]->is< Identifier >(), "Identifier is the first child node in the call expression" );
        auto const & name{ memberNodes[ 0U ]->get< Identifier >().name };

        std::vector< llvm::Value * > arguments{ variable };
        for ( auto i{ 1U }; i < std::size( memberNodes ); i++ )
        {
            auto * value{ codegen( ctx, memberNodes[ i ] ) };
            if ( value == nullptr ) { return nullptr; }
            arguments.push_back( value );
        }

        return memberNodes.size() == 1U
            ? ctx.builder->CreateCall( ctx.symbols.functions[ fmt::format( "{}__{}", contractTypeName, name ) ], llvm::None )
            : ctx.builder->CreateCall( ctx.symbols.functions[ fmt::format( "{}__{}", contractTypeName, name ) ], arguments  );
    }

    return nullptr;
}

llvm::Value * codegenContractDefinition( Context & ctx, std::span< AST::Node::Pointer const > const nodes )
{
    ASSERTM( std::size( nodes ) >= 2U, "Contract definition consists of an identifier and one or more statements in the contract's body" );

    ASSERTM( nodes[ 0U ]->is< Identifier >(), "Contract identifier is the first child node in the contract definition" );
    auto const & contractName{ nodes[ 0U ]->get< Identifier >().name };

    auto * contractType{ llvm::StructType::create( *ctx.internalCtx, contractName ) };

    std::vector< llvm::Constant * > dataMemberInitialValues;
    std::vector< llvm::Type     * > dataMemberTypes;

    Symbols::Table< Symbols::DataMemberType > dataMembers;

    ctx.symbols.currentContractName = contractName;

    std::size_t dataMemberIndex{ 0U };

    for ( auto i{ 1U }; i < std::size( nodes ); i++ )
    {
        auto const & node{ nodes[ i ] };
        if ( node->is< AST::NodeType >() )
        {
            if ( node->get< AST::NodeType >() == AST::NodeType::VariableDefinition )
            {
                auto const & children{ node->children() };

                ASSERTM( std::size( children ) >= 2U, "Data member definition consists of an identifier, type annotation and, optionally, initialization" );
                ASSERTM( children[ 0U ]->is< Identifier >(), "Data member identifier is the first child node in the definition" );
                ASSERTM( children[ 1U ]->is< TypeID     >(), "Data member type is the second child node in the definition" );

                auto const typeID{ children[ 1U ]->get< TypeID >() };
                dataMemberTypes.push_back( getType( ctx, typeID ) );

                if ( std::size( children ) > 2U )
                {
                    auto const & initNode{ children[ 2U ] };
                    if ( initNode->is< Number >() )
                    {
                        dataMemberInitialValues.push_back( llvm::ConstantInt::get( llvm::Type::getInt64Ty( *ctx.internalCtx ), initNode->get< Number >().value, true /* isSigned */ ) );
                    }
                    else if ( initNode->is< StringLiteral >() )
                    {
                        dataMemberInitialValues.push_back( ctx.builder->CreateGlobalStringPtr( initNode->get< StringLiteral >().value, "", 0U, ctx.module_.get() ) );
                    }
                }
                else
                {
                    if ( typeID == Registry::getNumHandle() )
                    {
                        dataMemberInitialValues.push_back( llvm::ConstantInt::get( llvm::Type::getInt64Ty( *ctx.internalCtx ), 0U, true /* isSigned */ ) );
                    }
                    else if ( typeID == Registry::getStrHandle() )
                    {
                        dataMemberInitialValues.push_back( ctx.builder->CreateGlobalStringPtr( "", "", 0U, ctx.module_.get() ) );
                    }
                }

                dataMembers[ children[ 0U ]->get< Identifier >().name ] =
                {
                    .internalType = dataMemberTypes[ dataMemberIndex ],
                    .index        = dataMemberIndex++
                };

                // codegen( ctx, node );
            }
        }
    }

    contractType->setBody( dataMemberTypes );
    ctx.symbols.contractTypes[ contractName ] =
    {
        .internalType    = contractType,
        .dataMemberTypes = std::move( dataMembers )
    };

    for ( auto i{ 1U }; i < std::size( nodes ); i++ )
    {
        auto const & node{ nodes[ i ] };
        if ( node->is< AST::NodeType >() )
        {
            if ( node->get< AST::NodeType >() == AST::NodeType::FunctionDefinition )
            {
                // TODO: Call a function with contract type name prefix
                codegen( ctx, node );
            }
        }
    }

    {
        // Create default constructor if there is no user-defined constructor
        auto   ctorName{ fmt::format( "{}____default_init__", contractName ) };
        auto * ctorType{ llvm::FunctionType::get( contractType, false ) };
        auto * ctor    { llvm::Function::Create( ctorType, llvm::Function::ExternalLinkage, ctorName, ctx.module_.get() ) };
        auto * block   { llvm::BasicBlock::Create( *ctx.internalCtx, "", ctor ) };
        ctx.builder->SetInsertPoint( block );

        auto * alloca{ ctx.builder->CreateAlloca( contractType, 0U ) };
        ctx.builder->CreateStore( llvm::ConstantStruct::get( contractType, dataMemberInitialValues ), alloca );
        ctx.builder->CreateRet( ctx.builder->CreateLoad( contractType, alloca ) );

        ctx.symbols.functions[ ctorName ] = ctor;
    }

    /**
     * Remove contract member variables from scope.
     */
    for ( auto it{ std::begin( ctx.symbols.variables ) }; it != std::end( ctx.symbols.variables ); )
    {
        if ( it->first.starts_with( fmt::format( "{}_{}_", ctx.symbols.currentContractName, ctx.symbols.currentFunctionName ) ) )
        {
            ctx.symbols.variables.erase( it++ );
        }
        else { ++it; }
    }

    ctx.symbols.currentContractName.clear();
    return nullptr;
}

llvm::Function * codegenFunctionDefinition( Context & ctx, std::span< AST::Node::Pointer const > const nodes )
{
    ASSERTM( std::size( nodes ) >= 2U, "Function definition consists of an identifier and at least one statement in the function body" );

    ASSERTM( nodes[ 0U ]->is< Identifier >(), "Function identifier is the first child node in the function definition" );
    auto const & functionName{ nodes[ 0U ]->get< Identifier >().name };

    std::vector< std::string > parameterNames;
    std::vector< llvm::Type * > parameters;

    llvm::Type * returnType{ llvm::Type::getVoidTy( *ctx.internalCtx ) };

    ctx.symbols.currentFunctionName = functionName;

    /**
     * Gather all the info required to define the LLVM IR function.
     */
    for ( std::size_t i{ 0U }; i < std::size( nodes ); i++ )
    {
        auto const & node{ nodes[ i ] };
        if ( node->is< AST::NodeType >() )
        {
            if ( node->get< AST::NodeType >() == AST::NodeType::FunctionParameterDefinition )
            {
                auto const & parameterNodes{ node->children() };
                if ( std::size( parameterNodes ) == 2U )
                {
                    ASSERTM( parameterNodes[ 0U ]->is< Identifier >(), "Function parameter identifier is the first child node in the function parameter definition" );
                    parameterNames.push_back( ctx.symbols.mangle( parameterNodes[ 0U ]->get< Identifier >().name ) );

                    ASSERTM( parameterNodes[ 1U ]->is< TypeID >(), "Function parameter type annotation is the second child node in the function parameter definition" );
                    parameters.push_back( getType( ctx, parameterNodes[ 1U ]->get< TypeID >() ) );
                }
                else if ( std::size( parameterNodes ) == 1U )
                {
                    ASSERTM( parameterNodes[ 0U ]->is< Identifier >(), "Function parameter identifier is the first child node in the function parameter definition" );
                    parameterNames.push_back( ctx.symbols.mangle( parameterNodes[ 0U ]->get< Identifier >().name ) );

                    if ( parameterNames.back() != "self" || ctx.symbols.currentContractName.empty() )
                    {
                        // TODO: Throw an error here. Only 'self' parameter is allowed not to have the type specified.
                    }

                    parameters.push_back( llvm::PointerType::get( ctx.symbols.contractTypes[ ctx.symbols.currentContractName ].internalType, 0U ) );
                }
            }
        }
        else if ( node->is< TypeID >() )
        {
            returnType = getType( ctx, node->get< TypeID >() );
            break;
        }
    }

    auto * functionType{ llvm::FunctionType::get( returnType, parameters, false ) };
    auto * function    { llvm::Function::Create( functionType, llvm::Function::ExternalLinkage, functionName, ctx.module_.get() ) };

    // Set names for all the arguments
    auto idx{ 0U };
    for ( auto & arg : function->args() )
    {
        arg.setName( parameterNames[ idx++ ] );
    }

    auto * block{ llvm::BasicBlock::Create( *ctx.internalCtx, "", function ) };
    ctx.builder->SetInsertPoint( block );

    for ( auto & arg : function->args() )
    {
        /**
         * Since we do not support mutable function arguments,
         * we don't need to create alloca for each argument.
         */
        ctx.symbols.variables[ std::string{ arg.getName() }] = &arg;
    }

    /**
     * Generate LLVM IR for all the function's body statements.
     */
    for ( std::size_t i{ 0U }; i < std::size( nodes ); i++ )
    {
        auto const & node{ nodes[ i ] };
        if ( node->is< AST::NodeType >() )
        {
            if ( node->get< AST::NodeType >() == AST::NodeType::StatementReturn )
            {
                if ( auto * return_{ codegen( ctx, node ) } )
                {
                    ctx.builder->CreateRet( return_ );
                }
            }
            else if ( node->get< AST::NodeType >() == AST::NodeType::VariableDefinition )
            {
                ASSERTM( std::size( nodes ) >= 2U, "Variable definition consists of an identifier and either type annotation or initialization or both" );

                ASSERTM( nodes[ 0U ]->is< Identifier >(), "Variable identifier is the first child node in the variable definition" );
                auto const & name{ nodes[ 0U ]->get< Identifier >().name };
                ctx.symbols.variable( name, codegen( ctx, node ) );
            }
            else
            {
                codegen( ctx, node );
            }
        }
    }

    if ( returnType->isVoidTy() )
    {
        ctx.builder->CreateRet( nullptr );
    }

    /**
     * Remove local variables and function arguments from scope.
     */
    for ( auto it{ std::begin( ctx.symbols.variables ) }; it != std::end( ctx.symbols.variables ); )
    {
        if ( it->first.starts_with( fmt::format( "{}_{}_", ctx.symbols.currentContractName, ctx.symbols.currentFunctionName ) ) )
        {
            ctx.symbols.variables.erase( it++ );
        }
        else { ++it; }
    }

    ctx.symbols.currentFunctionName.clear();

    // Validate the generated code and check for consistency
    verifyFunction( *function );
    ctx.symbols.functions[ ctx.symbols.mangle( functionName ) ] = function;
    return function;
}

llvm::Value * codegenVariableDefinition( Context & ctx, std::span< AST::Node::Pointer const > const nodes )
{
    ASSERTM( std::size( nodes ) >= 2U, "Variable definition consists of an identifier and either type annotation or initialization or both" );

    ASSERTM( nodes[ 0U ]->is< Identifier >(), "Variable identifier is the first child node in the variable definition" );
    auto const & name{ nodes[ 0U ]->get< Identifier >().name };

    auto * parent{ ctx.builder->GetInsertBlock()->getParent() };
    llvm::IRBuilder<> inScopeBuilder{ &parent->getEntryBlock(), parent->getEntryBlock().begin() };

    /**
     * If there is only initialization node or if there are both type annotation and
     * initialization nodes, create new value from initialization node.
     * TODO: Check if type annotation and initialization match.
     *
     * If there is only type annotation node, create new value from the default value of
     * the specified type.
     */
    auto const & node{ std::size( nodes ) <= 2U ? nodes[ 1U ] : nodes[ 2U ] };

    llvm::Value * value{ nullptr };

    if ( node->is< TypeID >() )
    {
        auto const typeID{ node->get< TypeID >() };
        if ( typeID == Registry::getNumHandle() )
        {
            value = inScopeBuilder.CreateAlloca( llvm::Type::getInt64Ty( *ctx.internalCtx ), 0U, name.data() );
            inScopeBuilder.CreateStore( llvm::ConstantInt::get( llvm::Type::getInt64Ty( *ctx.internalCtx ), 0U, true /* isSigned */ ), value );
        }
        else if ( typeID == Registry::getStrHandle() )
        {
            value = ctx.builder->CreateGlobalStringPtr( "", "", 0U, ctx.module_.get() );
        }
    }
    else if ( node->is< Number >() )
    {
        value = inScopeBuilder.CreateAlloca( llvm::Type::getInt64Ty( *ctx.internalCtx ), 0U, name.data() );
        inScopeBuilder.CreateStore( codegen( ctx, node ), value );
    }
    else
    {
        value = codegen( ctx, node );
    }

    ctx.symbols.variable( name, value );
    return value;
}

llvm::Value * codegenControlFlow( Context & ctx, std::span< AST::Node::Pointer const > const nodes )
{
    ASSERTM( std::size( nodes ) >= 2U, "Control flow consists of a condition and at least one statement in the if body" );

    auto * condition{ codegen( ctx, nodes[ 0U ] ) };
    if ( condition == nullptr ) { return nullptr; }

    // Convert condition to boolean by comparing it to 0
    condition = ctx.builder->CreateIntCast( condition, llvm::Type::getInt64Ty( *ctx.internalCtx ), false /* isSigned */, "booltmp" );
    condition = ctx.builder->CreateICmpNE
    (
        condition,
        llvm::ConstantInt::get( *ctx.internalCtx, llvm::APInt( sizeof( Number::Type ) * 8U /* numBits */, 0U, false /* isSigned */ ) ),
        "cond"
    );

    auto * parent{ ctx.builder->GetInsertBlock()->getParent() };

    auto * thenBlock{ llvm::BasicBlock::Create( *ctx.internalCtx, "if.then", parent ) };
    auto * elseBlock{ llvm::BasicBlock::Create( *ctx.internalCtx, "if.else" ) };
    auto * endBlock { llvm::BasicBlock::Create( *ctx.internalCtx, "if.end"  ) };

    auto hasElifElseBlock{ false };

    std::size_t nodeIdx{ 1U };

    llvm::Value * then{ nullptr };
    {
        // Generate LLVM IR for all statements within the if body
        ctx.builder->CreateCondBr( condition, thenBlock, elseBlock );
        ctx.builder->SetInsertPoint( thenBlock );

        for ( auto idx{ 0U }; nodeIdx < std::size( nodes ); idx++, nodeIdx++ )
        {
            auto const & node{ nodes[ nodeIdx ] };
            if
            (
                node->is< AST::NodeType >() &&
                (
                    node->get< AST::NodeType >() == AST::NodeType::StatementElif ||
                    node->get< AST::NodeType >() == AST::NodeType::StatementElse
                )
            )
            {
                hasElifElseBlock = true;
                break;
            }

            auto * value{ codegen( ctx, node ) };
            if ( value == nullptr ) { return nullptr; }

            if ( idx == 0U )
            {
                // required for the PHI node below
                then = value;
            }
        }
        // Jump to end block
        ctx.builder->CreateBr( endBlock );
    }

    // Codegen of 'then' can change the current block, update 'then' block for the PHI
    thenBlock = ctx.builder->GetInsertBlock();

    llvm::Value * elifOrElse{ nullptr };
    {
        // Generate LLVM IR for all statements within else body
        parent->getBasicBlockList().push_back( elseBlock );
        ctx.builder->SetInsertPoint( elseBlock );

        for ( auto idx{ 0U }; nodeIdx < std::size( nodes ); idx++, nodeIdx++ )
        {
            auto * value{ codegen( ctx, nodes[ nodeIdx ] ) };
            if ( value == nullptr ) { return nullptr; }

            if ( idx == 0U )
            {
                // required for the PHI node below
                elifOrElse = value;
            }
        }
        // Jump to end block
        ctx.builder->CreateBr( endBlock );
    }

    // Codegen of 'else' can change the current block, update 'else' block for the PHI
    elseBlock = ctx.builder->GetInsertBlock();

    // Generate LLVM IR PHI instruction
    parent->getBasicBlockList().push_back( endBlock );
    ctx.builder->SetInsertPoint( endBlock );

    if ( hasElifElseBlock )
    {
        ASSERT( then->getType() == elifOrElse->getType() );
        auto * phi{ ctx.builder->CreatePHI( then->getType(), hasElifElseBlock ? 2U : 1U, "iftmp" ) };
        phi->addIncoming( then      , thenBlock );
        phi->addIncoming( elifOrElse, elseBlock );
        return phi;
    }
    return then;
}

llvm::Value * codegenLoopFlow( Context & ctx, std::span< AST::Node::Pointer const > const nodes )
{
    ASSERTM( std::size( nodes ) >= 2U, "Loop flow consists of a condition and at least one statement in the loop body" );

    auto * parent        { ctx.builder->GetInsertBlock()->getParent() };
    auto * conditionBlock{ llvm::BasicBlock::Create( *ctx.internalCtx, "while.cond", parent ) };
    auto * loopBlock     { llvm::BasicBlock::Create( *ctx.internalCtx, "while.body", parent ) };

    ctx.builder->CreateBr      ( conditionBlock );
    ctx.builder->SetInsertPoint( conditionBlock );

    auto * condition{ codegen( ctx, nodes[ 0U ] ) };
    if ( condition == nullptr ) { return nullptr; }

    // Convert condition to boolean by comparing it to 0
    condition = ctx.builder->CreateIntCast( condition, llvm::Type::getInt64Ty( *ctx.internalCtx ), false /* isSigned */, "booltmp" );
    condition = ctx.builder->CreateICmpNE
    (
        condition,
        llvm::ConstantInt::get( *ctx.internalCtx, llvm::APInt( sizeof( Number::Type ) * 8U /* numBits */, 0U, false /* isSigned */ ) ),
        "cond"
    );

    auto * afterLoopBlock{ llvm::BasicBlock::Create( *ctx.internalCtx, "while.after", parent ) };
    ctx.builder->CreateCondBr( condition, loopBlock, afterLoopBlock );
    ctx.builder->SetInsertPoint( loopBlock );

    llvm::Value * then{ nullptr };
    for ( std::size_t i{ 1U }; i < std::size( nodes ); i++ )
    {
        then = codegen( ctx, nodes[ i ] );
        if ( then == nullptr ) { return nullptr; }
    }

    ctx.builder->CreateBr( conditionBlock );
    ctx.builder->SetInsertPoint( afterLoopBlock );
    return then;
}

llvm::Value * codegenAssert( Context & ctx, std::span< AST::Node::Pointer const > const nodes )
{
    ASSERT( std::size( nodes ) == 1U );

    auto * condition{ codegen( ctx, nodes[ 0U ] ) };
    if ( condition == nullptr ) { return nullptr; }

    // Convert condition to boolean by comparing it to 0
    condition = ctx.builder->CreateIntCast( condition, llvm::Type::getInt64Ty( *ctx.internalCtx ), false /* isSigned */, "booltmp" );
    condition = ctx.builder->CreateICmpEQ
    (
        condition,
        llvm::ConstantInt::get( *ctx.internalCtx, llvm::APInt( sizeof( Number::Type ) * 8U /* numBits */, 0U, false /* isSigned */ ) ),
        "cond"
    );

    auto * parent{ ctx.builder->GetInsertBlock()->getParent() };

    auto * thenBlock{ llvm::BasicBlock::Create( *ctx.internalCtx, "if.then", parent ) };
    auto * elseBlock{ llvm::BasicBlock::Create( *ctx.internalCtx, "if.else" ) };
    auto * endBlock { llvm::BasicBlock::Create( *ctx.internalCtx, "if.end"  ) };

    llvm::Value * then{ nullptr };
    {
        // Generate LLVM IR for all statements within the if body
        ctx.builder->CreateCondBr( condition, thenBlock, elseBlock );
        ctx.builder->SetInsertPoint( thenBlock );

        then = ctx.builder->CreateCall( ctx.symbols.externalBuiltInFunctions().at( "abort" ), llvm::None );
    }
    // Jump to end block
    ctx.builder->CreateBr( endBlock );

    // Generate LLVM IR for all statements within else body
    parent->getBasicBlockList().push_back( elseBlock );
    ctx.builder->SetInsertPoint( elseBlock );

    // Jump to end block
    ctx.builder->CreateBr( endBlock );

    parent->getBasicBlockList().push_back( endBlock );
    ctx.builder->SetInsertPoint( endBlock );

    return then;
}

} // namespace A1::LLVM::IR