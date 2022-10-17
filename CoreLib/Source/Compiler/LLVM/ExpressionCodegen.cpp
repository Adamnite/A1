/**
 *
 * Copyright (c)2022 The Adamnite C++ Authors.
 *
 * This code is open-sourced under the MIT license.
 */

#include "ExpressionCodegen.hpp"
#include "Utils/Utils.hpp"

#include <CoreLib/Utils/Macros.hpp>
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

namespace A1::LLVM
{

namespace
{
    [[ nodiscard ]]
    llvm::Value * allocate( llvm::IRBuilder<> & builder, llvm::Type * type, std::string_view const name, llvm::Value * initialValue )
    {
        auto * value{ builder.CreateAlloca( type, 0U, name.data() ) };
        if ( initialValue != nullptr )
        {
            builder.CreateStore( initialValue, value );
        }
        return value;
    }

    [[ nodiscard ]]
    llvm::Value * createVariable( Context & ctx, llvm::IRBuilder<> & scopeBuilder, Node::Pointer const & node, std::string_view const name )
    {
        if ( node->is< TypeID >() )
        {
            auto const typeID{ node->get< TypeID >() };
            if ( typeID == Registry::getNumberHandle() )
            {
                return allocate( scopeBuilder, llvm::Type::getDoubleTy( *ctx.internalCtx ), name, llvm::ConstantFP::get( *ctx.internalCtx, llvm::APFloat( 0.0 ) ) );
            }
            else if ( typeID == Registry::getStringLiteralHandle() )
            {
                return ctx.builder->CreateGlobalStringPtr( "", "", 0U, ctx.module_.get() );
            }
            else
            {
                return nullptr;
            }
        }
        else if ( node->is< Number >() )
        {
            return allocate( scopeBuilder, llvm::Type::getDoubleTy( *ctx.internalCtx ), name, codegen( ctx, node ) );
        }
        else
        {
            return codegen( ctx, node );
        }
    }

    [[ nodiscard ]]
    llvm::Value * getPrintFormat( Context & ctx, llvm::Type const * type )
    {
        if ( type->isDoubleTy() )
        {
            static auto * format{ ctx.builder->CreateGlobalStringPtr( "%f\n", "numFormat", 0U, ctx.module_.get() ) };
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
            return llvm::Type::getDoubleTy( *ctx.internalCtx );
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

llvm::Value * codegen( Context & ctx, Node::Pointer const & node )
{
    if ( node == nullptr ) { return nullptr; }

    return std::visit
    (
        Overload
        {
            [ &ctx, &node ]( NodeType const type ) -> llvm::Value *
            {
                switch ( type )
                {
#define CODEGEN( type, codegenFunc, builderFunc, opName ) \
    case NodeType::type: return codegenFunc( ctx, &llvm::IRBuilder<>::builderFunc, node->children(), opName )

                    CODEGEN( UnaryMinus       , codegenUnary , CreateFNeg      , "nottemp" );
                    CODEGEN( LogicalNot       , codegenUnary , CreateNot       , "lnottmp" );
                    CODEGEN( LogicalAnd       , codegenBinary, CreateLogicalAnd, "landtmp" );
                    CODEGEN( LogicalOr        , codegenBinary, CreateLogicalOr , "lortmp"  );
                    CODEGEN( Multiplication   , codegenBinary, CreateFMul      , "multmp"  );
                    CODEGEN( Division         , codegenBinary, CreateFDiv      , "divtmp"  );
                    CODEGEN( FloorDivision    , codegenBinary, CreateSDiv      , "fdivtmp" );
                    CODEGEN( Modulus          , codegenBinary, CreateFRem      , "modtmp"  );
                    CODEGEN( Addition         , codegenBinary, CreateFAdd      , "addtmp"  );
                    CODEGEN( Subtraction      , codegenBinary, CreateFSub      , "subtmp"  );
                    CODEGEN( BitwiseLeftShift , codegenBinary, CreateShl       , "lshtmp"  );
                    CODEGEN( BitwiseRightShift, codegenBinary, CreateAShr      , "rshtmp"  );
                    CODEGEN( BitwiseAnd       , codegenBinary, CreateAnd       , "andtmp"  );
                    CODEGEN( BitwiseOr        , codegenBinary, CreateOr        , "ortmp "  );
                    CODEGEN( BitwiseXor       , codegenBinary, CreateXor       , "xortmp"  );
                    CODEGEN( BitwiseNot       , codegenUnary , CreateNot       , "nottmp"  );
                    CODEGEN( Equality         , codegenBinary, CreateFCmpOEQ   , "eqtmp"   );
                    CODEGEN( Inequality       , codegenBinary, CreateFCmpONE   , "netmp"   );
                    CODEGEN( GreaterThan      , codegenBinary, CreateFCmpOGT   , "gttmp"   );
                    CODEGEN( GreaterThanEqual , codegenBinary, CreateFCmpOGE   , "getmp"   );
                    CODEGEN( LessThan         , codegenBinary, CreateFCmpOLT   , "ltmp"    );
                    CODEGEN( LessThanEqual    , codegenBinary, CreateFCmpOLE   , "letmp"   );
                    CODEGEN( IsIdentical      , codegenBinary, CreateFCmpOEQ   , "eqtmp"   );
                    CODEGEN( IsNotIdentical   , codegenBinary, CreateFCmpONE   , "netmp"   );

                    CODEGEN( AssignAddition         , codegenAssign, CreateFAdd, "addtmp"  );
                    CODEGEN( AssignSubtraction      , codegenAssign, CreateFSub, "subtmp"  );
                    CODEGEN( AssignMultiplication   , codegenAssign, CreateFMul, "multmp"  );
                    CODEGEN( AssignDivision         , codegenAssign, CreateFDiv, "divtmp"  );
                    CODEGEN( AssignFloorDivision    , codegenAssign, CreateSDiv, "fdivtmp" );
                    CODEGEN( AssignModulus          , codegenAssign, CreateFRem, "modtmp"  );
                    CODEGEN( AssignBitwiseLeftShift , codegenAssign, CreateShl , "shltmp"  );
                    CODEGEN( AssignBitwiseRightShift, codegenAssign, CreateAShr, "shrtmp"  );
                    CODEGEN( AssignBitwiseAnd       , codegenAssign, CreateAnd , "andtmp"  );
                    CODEGEN( AssignBitwiseOr        , codegenAssign, CreateOr  , "ortmp"   );
                    CODEGEN( AssignBitwiseXor       , codegenAssign, CreateXor , "xortmp"  );

#undef CODEGEN
                    case NodeType::Assign    : return codegenVariableDefinition( ctx, node->children() );
                    case NodeType::Call      : return codegenCall              ( ctx, node->children() );
                    case NodeType::MemberCall: return codegenMemberCall        ( ctx, node->children() );

                    case NodeType::StatementIf:
                    case NodeType::StatementElif:
                        return codegenControlFlow( ctx, node->children() );

                    case NodeType::StatementElse:
                    {
                        // Generate LLVM IR for all the statements in else body
                        llvm::Value * value{ nullptr };
                        for ( auto const & n : node->children() )
                        {
                            value = codegen( ctx, n );
                        }
                        return value;
                    }

                    case NodeType::StatementWhile:
                        return codegenLoopFlow( ctx, node->children() );

                    case NodeType::StatementReturn:
                    {
                        ASSERT( std::size( node->children() ) == 1U );
                        return codegen( ctx, node->children()[ 0U ] );
                    }

                    case NodeType::ContractDefinition: return codegenContractDefinition( ctx, node->children() );
                    case NodeType::FunctionDefinition: return codegenFunctionDefinition( ctx, node->children() );
                    case NodeType::VariableDefinition: return codegenVariableDefinition( ctx, node->children() );

                    default:
                        return nullptr;
                }
            },
            [ &ctx ]( Identifier const & identifier ) -> llvm::Value *
            {
                auto * value{ ctx.symbols.getVariable( identifier.name ) };
                if ( value == nullptr ) { return nullptr; }

                if ( value->getType()->getNumContainedTypes() > 0 && value->getType()->getContainedType( 0U )->isDoubleTy() )
                {
                    return ctx.builder->CreateLoad( llvm::Type::getDoubleTy( *ctx.internalCtx ), value );
                }
                return value;
            },
            [ &ctx ]( Number const number ) -> llvm::Value *
            {
                return llvm::ConstantFP::get( *ctx.internalCtx, llvm::APFloat( number ) );
            },
            [ &ctx ]( String const & str ) -> llvm::Value *
            {
                return ctx.builder->CreateGlobalStringPtr( str, "", 0U, ctx.module_.get() );
            },
            []( TypeID const ) -> llvm::Value *
            {
                return nullptr;
            }
        },
        node->value()
    );
}

template< typename ... T >
llvm::Value * codegenUnary
(
    Context                                & ctx,
    IRBuilderUnaryClbk< T ... >      const   clbk,
    std::span< Node::Pointer const > const   nodes,
    std::string_view                 const   opName
)
{
    ASSERT( std::size( nodes ) == 1U );
    auto * lhs{ codegen( ctx, nodes[ 0U ] ) };

    if ( lhs == nullptr ) { return nullptr; }

    return ( *( ctx.builder ).*clbk )( lhs, opName, T{} ... );
}

template< typename ... T >
llvm::Value * codegenBinary
(
    Context                                & ctx,
    IRBuilderBinaryClbk< T ... >     const   clbk,
    std::span< Node::Pointer const > const   nodes,
    std::string_view                 const   opName
)
{
    ASSERT( std::size( nodes ) == 2U );
    auto * lhs{ codegen( ctx, nodes[ 0U ] ) };
    auto * rhs{ codegen( ctx, nodes[ 1U ] ) };

    if ( lhs == nullptr || rhs == nullptr ) { return nullptr; }

    return ( *( ctx.builder ).*clbk )( lhs, rhs, opName, T{} ... );
}

template< typename ... T >
llvm::Value * codegenAssign
(
    Context                                & ctx,
    IRBuilderBinaryClbk< T ... >     const   clbk,
    std::span< Node::Pointer const > const   nodes,
    std::string_view                 const   opName
)
{
    ASSERTM( std::size( nodes ) == 2U, "Assign expression consists of an identifier and value to be assigned" );

    ASSERTM( nodes[ 0U ]->is< Identifier >(), "Variable identifier is the first child node in the assign expression" );
    auto const & name{ nodes[ 0U ]->get< Identifier >().name };

    auto * value{ ctx.symbols.getVariable( name ) };
    if ( value == nullptr ) { return nullptr; }

    ctx.builder->CreateStore( codegenBinary( ctx, clbk, nodes, opName ), value );
    return value;
}

llvm::Value * codegenCall( Context & ctx, std::span< Node::Pointer const > const nodes )
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

        /**
         * At the moment, the only standard function is 'print' function
         * TODO: Improve handling of standard functions uniformly.
         */
        if ( name == "print" )
        {
            // Standard print function currently supports one argument only
            if ( std::size( arguments ) > 1U ) { return nullptr; }

            arguments.insert( std::begin( arguments ), getPrintFormat( ctx, arguments[ 0U ]->getType() ) );

            return ctx.builder->CreateCall( ctx.symbols.builtInFunctions().at( name ), arguments, "" );
        }

        return arguments.empty()
            ? ctx.builder->CreateCall( ctx.symbols.functions[ name ], llvm::None, "" )
            : ctx.builder->CreateCall( ctx.symbols.functions[ name ], arguments , "" );
    }

    return nullptr;
}

llvm::Value * codegenMemberCall( Context & ctx, std::span< Node::Pointer const > const nodes )
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
        return ctx.builder->CreateLoad( llvm::Type::getDoubleTy( *ctx.internalCtx ), memberPtr );
    }
    else if ( member->is< NodeType >() && member->get< NodeType >() == NodeType::Call )
    {
        // Access function member
        return codegen( ctx, member );
    }

    return nullptr;
}

llvm::Value * codegenContractDefinition( Context & ctx, std::span< Node::Pointer const > const nodes )
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
        if ( node->is< NodeType >() )
        {
            if ( node->get< NodeType >() == NodeType::VariableDefinition )
            {
                // TODO: Support other member types
                dataMemberTypes.push_back( llvm::Type::getDoubleTy( *ctx.internalCtx ) );
                dataMemberInitialValues.push_back( llvm::ConstantInt::get( llvm::Type::getDoubleTy( *ctx.internalCtx ), 3U, false /* isSigned */ ) );
            }
            else if ( node->get< NodeType >() == NodeType::FunctionDefinition )
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
        auto * ctor    { llvm::Function::Create( ctorType, llvm::Function::InternalLinkage, ctorName, ctx.module_.get() ) };
        auto * block   { llvm::BasicBlock::Create( *ctx.internalCtx, "entry", ctor ) };
        ctx.builder->SetInsertPoint( block );

        auto * alloca{ ctx.builder->CreateAlloca( contractType, 0U ) };
        ctx.builder->CreateStore( llvm::ConstantStruct::get( contractType, dataMemberInitialValues ), alloca );
        ctx.builder->CreateRet( ctx.builder->CreateLoad( contractType, alloca ) );
        ctx.symbols.functions[ ctorName ] = ctor;
    }

    return nullptr;
}

llvm::Function * codegenFunctionDefinition( Context & ctx, std::span< Node::Pointer const > const nodes )
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
        if ( node->is< NodeType >() )
        {
            auto const nodeType{ node->get< NodeType >() };
            if ( nodeType == NodeType::FunctionParameterDefinition )
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
        }
    }

    auto * functionType{ llvm::FunctionType::get( returnType, parameters, false ) };
    auto * function    { llvm::Function::Create( functionType, llvm::Function::InternalLinkage, name, ctx.module_.get() ) };

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
        if ( node->is< NodeType >() )
        {
            auto const nodeType{ node->get< NodeType >() };
            if ( nodeType == NodeType::StatementReturn )
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

llvm::Value * codegenVariableDefinition( Context & ctx, std::span< Node::Pointer const > const nodes )
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

    auto * value{ createVariable( ctx, inScopeBuilder, node, name ) };
    ctx.symbols.variables[ name ] = value;
    return value;
}

llvm::Value * codegenControlFlow( Context & ctx, std::span< Node::Pointer const > const nodes )
{
    ASSERTM( std::size( nodes ) >= 2U, "Control flow consists of a condition and at least one statement in the if body" );

    auto * condition{ codegen( ctx, nodes[ 0U ] ) };
    if ( condition == nullptr ) { return nullptr; }

    // Convert condition to boolean by comparing it to 0.0
    condition = ctx.builder->CreateUIToFP( condition, llvm::Type::getDoubleTy( *ctx.internalCtx ), "booltmp" );
    condition = ctx.builder->CreateFCmpONE( condition, llvm::ConstantFP::get( *ctx.internalCtx, llvm::APFloat( 0.0 ) ), "ifcond" );

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
                node->is< NodeType >() &&
                (
                    node->get< NodeType >() == NodeType::StatementElif ||
                    node->get< NodeType >() == NodeType::StatementElse
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

    auto * phi{ ctx.builder->CreatePHI( llvm::IntegerType::getInt32Ty( *ctx.internalCtx ), 2, "iftmp" ) };
    phi->addIncoming( then, thenBlock );
    if ( hasElifElseBlock )
    {
        phi->addIncoming( elifOrElse_, elseBlock );
    }

    return phi;
}

llvm::Value * codegenLoopFlow( Context & ctx, std::span< Node::Pointer const > const nodes )
{
    ASSERTM( std::size( nodes ) >= 2U, "Loop flow consists of a condition and at least one statement in the loop body" );

    auto * parent        { ctx.builder->GetInsertBlock()->getParent() };
    auto * conditionBlock{ llvm::BasicBlock::Create( *ctx.internalCtx, "while.cond", parent ) };
    auto * loopBlock     { llvm::BasicBlock::Create( *ctx.internalCtx, "while.body", parent ) };

    ctx.builder->CreateBr      ( conditionBlock );
    ctx.builder->SetInsertPoint( conditionBlock );

    auto * condition{ codegen( ctx, nodes[ 0U ] ) };
    if ( condition == nullptr ) { return nullptr; }

    // Convert condition to boolean by comparing it to 0.0
    condition = ctx.builder->CreateUIToFP( condition, llvm::Type::getDoubleTy( *ctx.internalCtx ), "booltmp" );
    condition = ctx.builder->CreateFCmpONE( condition, llvm::ConstantFP::get( *ctx.internalCtx, llvm::APFloat( 0.0 ) ), "whilecond" );

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

} // namespace A1::LLVM