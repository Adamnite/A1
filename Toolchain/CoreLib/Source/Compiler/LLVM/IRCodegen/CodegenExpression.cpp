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
        if ( type->isIntegerTy( sizeof( Number ) * 8U ) )
        {
            static auto * format{ ctx.builder->CreateGlobalStringPtr( "%lu\n", "numFormat", 0U, ctx.module_.get() ) };
            return format;
        }
        else if
        (
            type->isPointerTy() &&
            type->getNumContainedTypes() > 0 &&
            (
                type->getContainedType( 0U )->isArrayTy  () ||
                type->getContainedType( 0U )->isIntegerTy()
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
        if ( typeID == Registry::getNumberHandle() )
        {
            return llvm::Type::getInt32Ty( *ctx.internalCtx );
        }
        else if ( typeID == Registry::getStringLiteralHandle() )
        {
            return llvm::Type::getInt8PtrTy( *ctx.internalCtx );
        }
        else
        {
            return nullptr;
        }
    }
} // namespace

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
                type->second,
                false, /* isConstant */
                llvm::GlobalVariable::ExternalLinkage,
                llvm::Constant::getNullValue( type->second )
            )
        };
        auto * initialContract{ ctx.builder->CreateCall( ctx.symbols.functions[ fmt::format( "{}_DefaultCTOR", name ) ], llvm::None, "" ) };
        ctx.builder->CreateStore( initialContract, globalContract );
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
            arguments.push_back( value );
        }

        auto const & builtInFunctions{ ctx.symbols.builtInFunctions() };

        /**
         * At the moment, the only standard function is 'print' function
         * TODO: Improve handling of standard functions uniformly.
         */
        if ( name == "print" )
        {
            // Standard print function currently supports one argument only
            if ( std::size( arguments ) > 1U ) { return nullptr; }

            arguments.insert( std::begin( arguments ), getPrintFormat( ctx, arguments[ 0U ]->getType() ) );

            return ctx.builder->CreateCall( builtInFunctions.at( name ), arguments, "" );
        }
        else if ( auto it{ builtInFunctions.find( name ) }; it != std::end( builtInFunctions ) )
        {
            return ctx.builder->CreateCall( builtInFunctions.at( name ), arguments, "" );
        }

        return arguments.empty()
            ? ctx.builder->CreateCall( ctx.symbols.functions[ name ], llvm::None, "" )
            : ctx.builder->CreateCall( ctx.symbols.functions[ name ], arguments , "" );
    }

    return nullptr;
}

llvm::Value * codegenMemberCall( Context & ctx, std::span< AST::Node::Pointer const > const nodes )
{
    ASSERTM( std::size( nodes ) == 2U, "Member call expression consists of two identifiers: variable and either data member or function member identifier" );

    ASSERTM( nodes[ 0U ]->is< Identifier >(), "Variable identifier is the first child node in the member call expression" );
    auto const & variableName{ nodes[ 0U ]->get< Identifier >().name };

    auto const & member{ nodes[ 1U ] };
    if ( member->is< Identifier >() )
    {
        // Access data member
        auto * variable { ctx.symbols.variables[ variableName ] };
        auto * memberPtr{ ctx.builder->CreateStructGEP( variable->getType()->getContainedType( 0U ), variable, 0U ) };
        return ctx.builder->CreateLoad( llvm::Type::getInt32Ty( *ctx.internalCtx ), memberPtr );
    }
    else if ( member->is< AST::NodeType >() && member->get< AST::NodeType >() == AST::NodeType::Call )
    {
        // Access function member
        return codegen( ctx, member );
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

    for ( auto i{ 1U }; i < std::size( nodes ); i++ )
    {
        auto const & node{ nodes[ i ] };
        if ( node->is< AST::NodeType >() )
        {
            if ( node->get< AST::NodeType >() == AST::NodeType::VariableDefinition )
            {
                // TODO: Support other member types
                dataMemberTypes.push_back( llvm::Type::getInt32Ty( *ctx.internalCtx ) );
                dataMemberInitialValues.push_back( llvm::ConstantInt::get( llvm::Type::getInt32Ty( *ctx.internalCtx ), 0U, false /* isSigned */ ) );
            }
            else if ( node->get< AST::NodeType >() == AST::NodeType::FunctionDefinition )
            {
                // TODO: Call a function with contract type name prefix
                codegen( ctx, node );
            }
        }
    }

    contractType->setBody( dataMemberTypes );
    ctx.symbols.contractTypes[ contractName ] = contractType;

    {
        // Create default constructor
        auto   ctorName{ fmt::format( "{}_DefaultCTOR", contractName ) };
        auto * ctorType{ llvm::FunctionType::get( contractType, false ) };
        auto * ctor    { llvm::Function::Create( ctorType, llvm::Function::ExternalLinkage, ctorName, ctx.module_.get() ) };
        auto * block   { llvm::BasicBlock::Create( *ctx.internalCtx, "entry", ctor ) };
        ctx.builder->SetInsertPoint( block );

        auto * alloca{ ctx.builder->CreateAlloca( contractType, 0U ) };
        ctx.builder->CreateStore( llvm::ConstantStruct::get( contractType, dataMemberInitialValues ), alloca );
        ctx.builder->CreateRet( ctx.builder->CreateLoad( contractType, alloca ) );
        ctx.symbols.functions[ ctorName ] = ctor;
    }

    return nullptr;
}

llvm::Function * codegenFunctionDefinition( Context & ctx, std::span< AST::Node::Pointer const > const nodes )
{
    ASSERTM( std::size( nodes ) >= 2U, "Function definition consists of an identifier and at least one statement in the function body" );

    ASSERTM( nodes[ 0U ]->is< Identifier >(), "Function identifier is the first child node in the function definition" );
    auto const & name{ nodes[ 0U ]->get< Identifier >().name };

    std::vector< std::string > parameterNames;
    std::vector< llvm::Type * > parameters;

    llvm::Type * returnType{ llvm::Type::getVoidTy( *ctx.internalCtx ) };

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
                ASSERTM( std::size( parameterNodes ) == 2U, "Function parameter definition consists of an identifier and type annotation" );

                ASSERTM( parameterNodes[ 0U ]->is< Identifier >(), "Function parameter identifier is the first child node in the function parameter definition" );
                parameterNames.push_back( parameterNodes[ 0U ]->get< Identifier >().name );

                ASSERTM( parameterNodes[ 1U ]->is< TypeID >(), "Function parameter type annotation is the second child node in the function parameter definition" );
                parameters.push_back( getType( ctx, parameterNodes[ 1U ]->get< TypeID >() ) );
            }
        }
        else if ( node->is< TypeID >() )
        {
            returnType = getType( ctx, node->get< TypeID >() );
            break;
        }
    }

    auto * functionType{ llvm::FunctionType::get( returnType, parameters, false ) };
    auto * function    { llvm::Function::Create( functionType, llvm::Function::ExternalLinkage, name, ctx.module_.get() ) };

    // Set names for all the arguments
    auto idx{ 0U };
    for ( auto & arg : function->args() )
    {
        arg.setName( parameterNames[ idx++ ] );
    }

    auto * block{ llvm::BasicBlock::Create( *ctx.internalCtx, "entry", function ) };
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

    // TODO: Remove variables from the symbols table

    // Validate the generated code and check for consistency
    verifyFunction( *function );
    ctx.symbols.functions[ name ] = function;
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
        if ( typeID == Registry::getNumberHandle() )
        {
            value = inScopeBuilder.CreateAlloca( llvm::Type::getInt32Ty( *ctx.internalCtx ), 0U, name.data() );
            inScopeBuilder.CreateStore( llvm::ConstantInt::get( llvm::Type::getInt32Ty( *ctx.internalCtx ), 0U, false /* isSigned */ ), value );
        }
        else if ( typeID == Registry::getStringLiteralHandle() )
        {
            value = ctx.builder->CreateGlobalStringPtr( "", "", 0U, ctx.module_.get() );
        }
    }
    else if ( node->is< Number >() )
    {
        value = inScopeBuilder.CreateAlloca( llvm::Type::getInt32Ty( *ctx.internalCtx ), 0U, name.data() );
        inScopeBuilder.CreateStore( codegen( ctx, node ), value );
    }
    else
    {
        value = codegen( ctx, node );
    }
    ctx.symbols.variables[ name ] = value;
    return value;
}

llvm::Value * codegenControlFlow( Context & ctx, std::span< AST::Node::Pointer const > const nodes )
{
    ASSERTM( std::size( nodes ) >= 2U, "Control flow consists of a condition and at least one statement in the if body" );

    auto * condition{ codegen( ctx, nodes[ 0U ] ) };
    if ( condition == nullptr ) { return nullptr; }

    // Convert condition to boolean by comparing it to 0
    condition = ctx.builder->CreateIntCast( condition, llvm::Type::getInt32Ty( *ctx.internalCtx ), false /* isSigned */, "booltmp" );
    condition = ctx.builder->CreateICmpNE
    (
        condition,
        llvm::ConstantInt::get( *ctx.internalCtx, llvm::APInt( sizeof( Number ) * 8U /* numBits */, 0U, false /* isSigned */ ) ),
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

        for ( ; nodeIdx < std::size( nodes ); nodeIdx++ )
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

            then = codegen( ctx, node );
            if ( then == nullptr ) { return nullptr; }
        }
        // Jump to end block
        ctx.builder->CreateBr( endBlock );
    }

    // Codegen of 'then' can change the current block, update 'then' block for the PHI
    thenBlock = ctx.builder->GetInsertBlock();

    llvm::Value * elifOrElse_{ nullptr };
    {
        // Generate LLVM IR for all statements within else body
        parent->getBasicBlockList().push_back( elseBlock );
        ctx.builder->SetInsertPoint( elseBlock );

        for ( ; nodeIdx < std::size( nodes ); nodeIdx++ )
        {
            elifOrElse_ = codegen( ctx, nodes[ nodeIdx ] );
            if ( elifOrElse_ == nullptr ) { return nullptr; }
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
        auto * phi{ ctx.builder->CreatePHI( llvm::IntegerType::getInt32Ty( *ctx.internalCtx ), hasElifElseBlock ? 2U : 1U, "iftmp" ) };
        phi->addIncoming( then, thenBlock );
        phi->addIncoming( elifOrElse_, elseBlock );
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
    condition = ctx.builder->CreateIntCast( condition, llvm::Type::getInt32Ty( *ctx.internalCtx ), false /* isSigned */, "booltmp" );
    condition = ctx.builder->CreateICmpNE
    (
        condition,
        llvm::ConstantInt::get( *ctx.internalCtx, llvm::APInt( sizeof( Number ) * 8U /* numBits */, 0U, false /* isSigned */ ) ),
        "cond"
    );

    auto * afterLoopBlock{ llvm::BasicBlock::Create( *ctx.internalCtx, "while.after", parent ) };
    ctx.builder->CreateCondBr( condition, loopBlock, afterLoopBlock );
    ctx.builder->SetInsertPoint( loopBlock );

    for ( std::size_t i{ 1U }; i < std::size( nodes ); i++ )
    {
        auto * then{ codegen( ctx, nodes[ i ] ) };
        if ( then == nullptr ) { return nullptr; }
    }

    ctx.builder->CreateBr( conditionBlock );
    ctx.builder->SetInsertPoint( afterLoopBlock );
    return nullptr;
}

} // namespace A1::LLVM::IR