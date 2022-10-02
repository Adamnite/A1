/**
 *
 * Copyright (c)2022 The Adamnite C++ Authors.
 *
 * This code is open-sourced under the MIT license.
 */

#include <CoreLib/Compiler/LLVM/Codegen.hpp>
#include <CoreLib/Utils/Macros.hpp>
#include <CoreLib/Types.hpp>

#include "Utils/Utils.hpp"

#if defined (__clang__)
#   pragma clang diagnostic push
#   pragma clang diagnostic ignored "-Wunused-parameter"
#elif defined(__GNUC__) || defined(__GNUG__)
#   pragma GCC diagnostic push
#   pragma GCC diagnostic ignored "-Wunused-parameter"
#endif

#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Value.h>
#include <llvm/IR/Verifier.h>

#if defined(__clang__)
#   pragma clang diagnostic pop
#elif defined(__GNUC__) || defined(__GNUG__)
#   pragma GCC diagnostic pop
#endif

#include <vector>
#include <span>
#include <map>

namespace A1::LLVM
{

namespace
{
    using ScopeIdentifiers = std::map< std::string, llvm::Value * >;

    std::unique_ptr< llvm::LLVMContext > context;
    std::unique_ptr< llvm::IRBuilder<> > builder;
    std::unique_ptr< llvm::Module      > module_;

    std::map< std::string, llvm::FunctionCallee > stdFunctions;
    std::map< std::string, llvm::FunctionCallee > nonStdFunctions;

    std::map< std::string, llvm::Type * > contractTypes;

    [[ nodiscard ]]
    llvm::Value * getFromScope( ScopeIdentifiers const & scope, std::string const & name ) noexcept
    {
        if ( auto it{ scope.find( name ) }; it != std::end( scope ) )
        {
            return it->second;
        }
        return nullptr;
    }

    [[ nodiscard ]]
    llvm::Constant * getFormat( llvm::Type const * type )
    {
        if ( type->isDoubleTy() )
        {
            return builder->CreateGlobalStringPtr( "%f\n", "numFormat", 0, module_.get() );
        }
        else if
        (
            type->getNumContainedTypes() > 0 &&
            (
                type->getContainedType( 0 )->isArrayTy  () ||
                type->getContainedType( 0 )->isIntegerTy()
            )
        )
        {
            return builder->CreateGlobalStringPtr( "%s\n", "strFormat", 0, module_.get() );
        }
        return nullptr;
    }

    [[ nodiscard ]]
    llvm::Value * allocate( llvm::IRBuilder<> & builder, std::string_view const name, llvm::Type * type, llvm::Value * initialValue )
    {
        auto * value{ builder.CreateAlloca( type, 0, name.data() ) };
        if ( initialValue != nullptr )
        {
            builder.CreateStore( initialValue, value );
        }
        return value;
    }

    template< typename ... T >
    using IRBuilderUnaryClbk = llvm::Value * ( llvm::IRBuilderBase::* )( llvm::Value *, llvm::Twine const &, T ... );

    template< typename ... T >
    llvm::Value * codegenUnary
    (
        IRBuilderUnaryClbk< T ... >      const,
        std::span< Node::Pointer const > const,
        std::string_view                 const,
        ScopeIdentifiers                       &
    );

    template< typename ... T >
    using IRBuilderBinaryClbk = llvm::Value * ( llvm::IRBuilderBase::* )( llvm::Value *, llvm::Value *, llvm::Twine const &, T ... );

    template< typename ... T >
    llvm::Value * codegenBinary
    (
        IRBuilderBinaryClbk< T ... >     const,
        std::span< Node::Pointer const > const,
        std::string_view                 const,
        ScopeIdentifiers                       &
    );

    template< typename ... T>
    llvm::Value * codegenAssignerDefinition
    (
        IRBuilderBinaryClbk< T ... >     const,
        std::span< Node::Pointer const > const,
        std::string_view                 const,
        ScopeIdentifiers                       &
    );

    llvm::Value    * codegenFunctionCall      ( std::span< Node::Pointer const > const, ScopeIdentifiers & );
    llvm::Value    * codegenMemberCall        ( std::span< Node::Pointer const > const, ScopeIdentifiers & );
    llvm::Value    * codegenLoopFlow          ( std::span< Node::Pointer const > const, ScopeIdentifiers & );
    llvm::Value    * codegenControlFlow       ( std::span< Node::Pointer const > const, ScopeIdentifiers & );
    llvm::Value    * codegenContractDefinition( std::span< Node::Pointer const > const, ScopeIdentifiers & );
    llvm::Function * codegenFunctionDefinition( std::span< Node::Pointer const > const );
    llvm::Value    * codegenVariableDefinition( std::span< Node::Pointer const > const, ScopeIdentifiers & );

    llvm::Value * codegenImpl( Node::Pointer const & node, ScopeIdentifiers & scope )
    {
        if ( node == nullptr ) { return nullptr; }

        return std::visit
        (
            Overload
            {
                [ & ]( NodeType const type ) -> llvm::Value *
                {
                    switch ( type )
                    {
                        case NodeType::Call:
                            return codegenFunctionCall( node->children(), scope );
                        case NodeType::MemberCall:
                            return codegenMemberCall( node->children(), scope );

                        case NodeType::UnaryMinus:
                            return codegenUnary( &llvm::IRBuilder<>::CreateFNeg, node->children(), "nottmp", scope );

                        case NodeType::Multiplication:
                            return codegenBinary( &llvm::IRBuilder<>::CreateFMul, node->children(), "multmp", scope );
                        case NodeType::Division:
                            return codegenBinary( &llvm::IRBuilder<>::CreateFDiv, node->children(), "divtmp", scope );
                        case NodeType::FloorDivision:
                            return codegenBinary( &llvm::IRBuilder<>::CreateSDiv, node->children(), "fdivtmp", scope );
                        case NodeType::Modulus:
                            return codegenBinary( &llvm::IRBuilder<>::CreateFRem, node->children(), "modtmp", scope );
                        case NodeType::Addition:
                            return codegenBinary( &llvm::IRBuilder<>::CreateFAdd, node->children(), "addtmp", scope );
                        case NodeType::Subtraction:
                            return codegenBinary( &llvm::IRBuilder<>::CreateFSub, node->children(), "subtmp", scope );

                        case NodeType::BitwiseLeftShift:
                            return codegenBinary( &llvm::IRBuilder<>::CreateShl, node->children(), "lshtmp", scope );
                        case NodeType::BitwiseRightShift:
                            return codegenBinary( &llvm::IRBuilder<>::CreateAShr, node->children(), "rshtmp", scope );
                        case NodeType::BitwiseAnd:
                            return codegenBinary( &llvm::IRBuilder<>::CreateAnd, node->children(), "andtmp", scope );
                        case NodeType::BitwiseOr:
                            return codegenBinary( &llvm::IRBuilder<>::CreateOr, node->children(), "ortmp", scope );
                        case NodeType::BitwiseXor:
                            return codegenBinary( &llvm::IRBuilder<>::CreateXor, node->children(), "xortmp", scope );
                        case NodeType::BitwiseNot:
                            return codegenUnary( &llvm::IRBuilder<>::CreateNot, node->children(), "nottmp", scope );

                        case NodeType::Equality:
                            return codegenBinary( &llvm::IRBuilder<>::CreateFCmpOEQ, node->children(), "eqtmp", scope );
                        case NodeType::Inequality:
                            return codegenBinary( &llvm::IRBuilder<>::CreateFCmpONE, node->children(), "netmp", scope );
                        case NodeType::GreaterThan:
                            return codegenBinary( &llvm::IRBuilder<>::CreateFCmpOGT, node->children(), "gttmp", scope );
                        case NodeType::GreaterThanEqual:
                            return codegenBinary( &llvm::IRBuilder<>::CreateFCmpOGE, node->children(), "getmp", scope );
                        case NodeType::LessThan:
                            return codegenBinary( &llvm::IRBuilder<>::CreateFCmpOLT, node->children(), "ltmp", scope );
                        case NodeType::LessThanEqual:
                            return codegenBinary( &llvm::IRBuilder<>::CreateFCmpOLE, node->children(), "letmp", scope );

                        case NodeType::IsIdentical:
                            return codegenBinary( &llvm::IRBuilder<>::CreateFCmpOEQ, node->children(), "eqtmp", scope );
                        case NodeType::IsNotIdentical:
                            return codegenBinary( &llvm::IRBuilder<>::CreateFCmpONE, node->children(), "netmp", scope );

                        case NodeType::LogicalNot:
                            return codegenUnary( &llvm::IRBuilder<>::CreateNot, node->children(), "lnottmp", scope );
                        case NodeType::LogicalAnd:
                            return codegenBinary( &llvm::IRBuilder<>::CreateLogicalAnd, node->children(), "landtmp", scope );
                        case NodeType::LogicalOr:
                            return codegenBinary( &llvm::IRBuilder<>::CreateLogicalOr, node->children(), "lortmp", scope );

                        case NodeType::Assign:
                            return codegenVariableDefinition(node->children(), scope);
                        case NodeType::AssignAddition:
                            return codegenAssignerDefinition( &llvm::IRBuilder<>::CreateFAdd, node->children(), "addvar", scope );
                        case NodeType::AssignSubtraction:
                            return codegenAssignerDefinition( &llvm::IRBuilder<>::CreateFSub, node->children(), "subvar", scope );
                        case NodeType::AssignMultiplication:
                            return codegenAssignerDefinition( &llvm::IRBuilder<>::CreateFMul, node->children(), "mulvar", scope );
                        case NodeType::AssignDivision:
                            return codegenAssignerDefinition( &llvm::IRBuilder<>::CreateFDiv, node->children(), "divvar", scope );
                        case NodeType::AssignFloorDivision:
                            return codegenAssignerDefinition( &llvm::IRBuilder<>::CreateSDiv, node->children(), "fdivvar", scope );
                        case NodeType::AssignModulus:
                            return codegenAssignerDefinition( &llvm::IRBuilder<>::CreateFRem, node->children(), "modvar", scope );
                        case NodeType::AssignBitwiseLeftShift:
                            return codegenAssignerDefinition( &llvm::IRBuilder<>::CreateShl, node->children(), "shlvar", scope );
                        case NodeType::AssignBitwiseRightShift:
                            return codegenAssignerDefinition( &llvm::IRBuilder<>::CreateAShr, node->children(), "shrvar", scope );
                        case NodeType::AssignBitwiseAnd:
                            return codegenAssignerDefinition( &llvm::IRBuilder<>::CreateAnd, node->children(), "andvar", scope );
                        case NodeType::AssignBitwiseOr:
                            return codegenAssignerDefinition( &llvm::IRBuilder<>::CreateOr, node->children(), "orvar", scope );
                        case NodeType::AssignBitwiseXor:
                            return codegenAssignerDefinition( &llvm::IRBuilder<>::CreateXor, node->children(), "xorvar", scope );

                        case NodeType::StatementIf:
                            return codegenControlFlow( node->children(), scope );
                        case NodeType::StatementWhile:
                            return codegenLoopFlow( node->children(), scope );
                        case NodeType::StatementReturn:
                        {
                            ASSERT( std::size( node->children() ) == 1U );
                            return codegenImpl( node->children()[ 0 ], scope );
                        }

                        case NodeType::ContractDefinition:
                            return codegenContractDefinition( node->children(), scope );
                        case NodeType::FunctionDefinition:
                            return codegenFunctionDefinition( node->children() );
                        case NodeType::VariableDefinition:
                            return codegenVariableDefinition( node->children(), scope );

                        default:
                            return nullptr;
                    }
                },
                [ & ]( Identifier const & identifier ) -> llvm::Value *
                {
                    auto * valuePtr{ getFromScope( scope, identifier.name ) };
                    if ( valuePtr->getType()->getNumContainedTypes() > 0 && valuePtr->getType()->getContainedType( 0 )->isDoubleTy() )
                    {
                        return builder->CreateLoad( llvm::Type::getDoubleTy( *context ), valuePtr );
                    }
                    return valuePtr;
                },
                []( Number const number ) -> llvm::Value *
                {
                    return llvm::ConstantFP::get( *context, llvm::APFloat( number ) );
                },
                []( String const & str ) -> llvm::Value *
                {
                    return builder->CreateGlobalStringPtr( str, "", 0, module_.get() );
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
        IRBuilderUnaryClbk< T ... >      const   clbk,
        std::span< Node::Pointer const > const   nodes,
        std::string_view                 const   name,
        ScopeIdentifiers                       & scope
    )
    {
        ASSERT( std::size( nodes ) == 1U );
        auto * lhs{ codegenImpl( nodes[ 0U ], scope ) };

        if ( lhs == nullptr ) { return nullptr; }

        return ( *builder.*clbk )( lhs, name, T{} ... );
    }

    template< typename ... T >
    llvm::Value * codegenBinary
    (
        IRBuilderBinaryClbk< T ... >     const   clbk,
        std::span< Node::Pointer const > const   nodes,
        std::string_view                 const   name,
        ScopeIdentifiers                       & scope
    )
    {
        ASSERT( std::size( nodes ) == 2U );
        auto * lhs{ codegenImpl( nodes[ 0U ], scope ) };
        auto * rhs{ codegenImpl( nodes[ 1U ], scope ) };

        if ( lhs == nullptr || rhs == nullptr ) { return nullptr; }

        return ( *builder.*clbk )( lhs, rhs, name, T{} ... );
    }

    [[ nodiscard ]]
    llvm::Value * create( llvm::IRBuilder<> & inScopeBuilder, Node::Pointer const & node, std::string_view const name, ScopeIdentifiers & scope )
    {
        if ( node->is< TypeID >() )
        {
            if ( node->get< TypeID >() == Registry::getNumberHandle() )
            {
                return allocate( inScopeBuilder, name, llvm::Type::getDoubleTy( *context ), llvm::ConstantFP::get( *context, llvm::APFloat( 0.0 ) ) );
            }
            else if ( node->get< TypeID >() == Registry::getStringLiteralHandle() )
            {
                return builder->CreateGlobalStringPtr( "", "", 0, module_.get() );
            }
            else
            {
                return nullptr;
            }
        }
        else if ( node->is< Number >() )
        {
            return allocate( inScopeBuilder, name, llvm::Type::getDoubleTy( *context ), codegenImpl( node, scope ) );
        }
        else
        {
            return codegenImpl( node, scope );
        }
    }

    llvm::Value * codegenFunctionCall( std::span< Node::Pointer const > const nodes, ScopeIdentifiers & scope )
    {
        ASSERT( std::size( nodes ) >= 1U );

        auto const & functionName{ nodes[ 0U ]->get< Identifier >().name };
        if ( functionName == "print" )
        {
            if ( std::size( nodes ) > 2U ) { return nullptr; }

            auto * value{ codegenImpl( nodes[ 1U ], scope ) };
            if ( value == nullptr ) { return nullptr; }

            std::array< llvm::Value *, 2U > arguments{ getFormat( value->getType() ), value };
            return builder->CreateCall( stdFunctions[ "print" ], arguments, "print" );
        }
        else if ( contractTypes.find( functionName ) != std::end( contractTypes ) )
        {
            return new llvm::GlobalVariable( *module_, contractTypes[ functionName ], true, llvm::GlobalVariable::ExternalLinkage, llvm::Constant::getNullValue( contractTypes[ functionName ] ) );
        }
        else
        {
            std::vector< llvm::Value * > arguments;
            for ( auto i{ 1U }; i < std::size( nodes ); i++ )
            {
                arguments.push_back( codegenImpl( nodes[ i ], scope ) );
            }
            if ( arguments.empty() )
            {
                return builder->CreateCall( nonStdFunctions[ functionName ], llvm::None, "" );
            }
            else
            {
                return builder->CreateCall( nonStdFunctions[ functionName ], arguments, "" );
            }
        }

        return nullptr;
    }

    llvm::Value * codegenMemberCall( std::span< Node::Pointer const > const nodes, ScopeIdentifiers & scope )
    {
        auto const & member{ nodes[ 1U ] };
        if ( member->is< Identifier >() )
        {
            // accessing data member
            return nullptr;
        }
        else if ( member->is< NodeType >() && member->get< NodeType >() == NodeType::Call )
        {
            // accessing function member
            return codegenImpl( member, scope );
        }

        return nullptr;
    }

    llvm::Value * codegenLoopFlow( std::span< Node::Pointer const > const nodes, ScopeIdentifiers & scope )
    {
        ASSERT( std::size( nodes ) >= 1U );

        auto * parent        { builder->GetInsertBlock()->getParent() };
        auto * conditionBlock{ llvm::BasicBlock::Create( *context, "cond", parent ) };
        auto * loopBlock     { llvm::BasicBlock::Create( *context, "loop", parent ) };

        builder->CreateBr      ( conditionBlock );
        builder->SetInsertPoint( conditionBlock );

        auto * condition{ codegenImpl( nodes[ 0U ], scope ) };
        if ( condition == nullptr ) { return nullptr; }

        // convert condition to boolean by comparing non-equal to 0.0
        condition = builder->CreateUIToFP( condition, llvm::Type::getDoubleTy( *context ), "booltmp" );
        condition = builder->CreateFCmpONE( condition, llvm::ConstantFP::get( *context, llvm::APFloat( 0.0 ) ), "loopcond" );

        auto * afterLoopBlock{ llvm::BasicBlock::Create( *context, "afterloop", parent ) };
        builder->CreateCondBr( condition, loopBlock, afterLoopBlock );

        builder->SetInsertPoint( loopBlock );

        for ( std::size_t i{ 1U }; i < std::size( nodes ); i++ )
        {
            auto * then{ codegenImpl( nodes[ i ], scope ) };
            if ( then == nullptr ) { return nullptr; }
        }

        builder->CreateBr( conditionBlock );

        builder->SetInsertPoint( afterLoopBlock );

        return nullptr;
    }

    llvm::Value * codegenControlFlow( std::span< Node::Pointer const > const nodes, ScopeIdentifiers & scope )
    {
        ASSERT( std::size( nodes ) >= 2U );

        auto * condition{ codegenImpl( nodes[ 0U ], scope ) };
        if ( condition == nullptr ) { return nullptr; }

        // convert condition to boolean by comparing non-equal to 0.0
        condition = builder->CreateUIToFP( condition, llvm::Type::getDoubleTy( *context ), "booltmp" );
        condition = builder->CreateFCmpONE( condition, llvm::ConstantFP::get( *context, llvm::APFloat( 0.0 ) ), "ifcond" );

        auto * parent{ builder->GetInsertBlock()->getParent() };

        // create conditions and blocks for if/elif/else

        auto * thenBlock { llvm::BasicBlock::Create( *context, "then", parent ) };
        auto * elseBlock { llvm::BasicBlock::Create( *context, "else"   ) };
        auto * mergeBlock{ llvm::BasicBlock::Create( *context, "ifcont" ) };

        builder->CreateCondBr( condition, thenBlock, elseBlock );
        builder->SetInsertPoint( thenBlock );

        auto * then{ codegenImpl( nodes[ 1U ], scope ) };
        if ( then == nullptr ) { return nullptr; }

        builder->CreateBr( mergeBlock );

        // codegen of 'then' can change the current block, update 'then' block for the PHI
        thenBlock = builder->GetInsertBlock();

        // emit else block
        parent->getBasicBlockList().push_back( elseBlock );
        builder->SetInsertPoint( elseBlock );

        auto * else_{ codegenImpl( nodes[ 2 ], scope ) };
        if ( else_ == nullptr ) { return nullptr; }

        builder->CreateBr( mergeBlock );

        // codegen of 'else' can change the current block, update 'else' block for the PHI
        elseBlock = builder->GetInsertBlock();

        parent->getBasicBlockList().push_back( mergeBlock );
        builder->SetInsertPoint( mergeBlock );

        auto * phi{ builder->CreatePHI( llvm::IntegerType::getInt32Ty( *context ), 2, "iftmp" ) };

        phi->addIncoming(then, thenBlock);
        phi->addIncoming(else_, elseBlock);
        return phi;
    }

    llvm::Value * codegenContractDefinition( std::span< Node::Pointer const > const nodes, ScopeIdentifiers & scope )
    {
        auto const & contractName{ nodes[ 0 ]->get< Identifier >().name };

        auto * contractType{ llvm::StructType::create( *context, contractName ) };

        std::vector< llvm::Type * > dataTypes;
        for ( auto i{ 1U }; i < std::size( nodes ); i++ )
        {
            auto const & node{ nodes[ i ] };
            if ( node->is< NodeType >() && node->get< NodeType >() == NodeType::VariableDefinition )
            {
                dataTypes.push_back( llvm::Type::getDoubleTy( *context) );
            }
            else if ( node->is< NodeType >() && node->get< NodeType >() == NodeType::FunctionDefinition )
            {
                auto * function{ codegenImpl( node, scope ) };
                dataTypes.push_back( function->getType() );
            }
        }

        contractType->setBody( dataTypes );

        contractTypes[ contractName ] = contractType;

        return nullptr;
    }

    llvm::Function * codegenFunctionDefinition( std::span< Node::Pointer const > const nodes )
    {
        ASSERT( std::size( nodes ) >= 2U );

        auto functionBodyStartIdx{ 0U };
        auto returnTypeIdx{ 0U };

        auto const & functionName{ nodes[ 0 ]->get< Identifier >().name };

        std::vector< std::size_t > returnStatementsIndices;

        std::vector< std::string > paramNames;
        for ( auto i{ 0U }; i < std::size( nodes ); i++ )
        {
            auto const & node{ nodes[ i ] };
            if ( node->is< NodeType >() && node->get< NodeType >() == NodeType::FunctionParameterDefinition )
            {
                auto const & paramNodes{ node->children() };
                paramNames.push_back( paramNodes[ 0 ]->get< Identifier >().name );
            }
            else if ( node->is_not< Identifier >() && node->is_not< TypeID >() && functionBodyStartIdx == 0U )
            {
                if ( node->is< NodeType >() && node->get< NodeType >() == NodeType::StatementReturn )
                {
                    returnStatementsIndices.push_back( i );
                }
                functionBodyStartIdx = i;
            }
            else if ( node->is< TypeID >() )
            {
                returnTypeIdx = i;
            }
            else if ( node->is< NodeType >() && node->get< NodeType >() == NodeType::StatementReturn )
            {
                returnStatementsIndices.push_back( i );
            }
        }

        // create function definition
        std::vector< llvm::Type * > params{ std::size( paramNames ), llvm::Type::getDoubleTy( *context ) };

        llvm::Type * type{ nullptr };
        if ( returnTypeIdx != 0 && nodes[ returnTypeIdx ]->get< TypeID >() == Registry::getNumberHandle() )
        {
            type = llvm::Type::getDoubleTy( *context );
        }
        else if ( returnTypeIdx != 0 && nodes[ returnTypeIdx ]->get< TypeID >() == Registry::getStringLiteralHandle() )
        {
            type = llvm::Type::getInt8PtrTy( *context );
        }
        else
        {
            type = llvm::Type::getVoidTy(*context);
        }

        auto * functionType{ llvm::FunctionType::get( type, params, false ) };

        auto * function
        {
            llvm::Function::Create
            (
                functionType,
                llvm::Function::ExternalLinkage,
                functionName,
                module_.get()
            )
        };

        // set names for all the arguments
        auto idx{ 0U };
        for ( auto & arg : function->args() )
        {
            arg.setName( paramNames[ idx++ ] );
        }

        auto * block{ llvm::BasicBlock::Create( *context, "entry", function ) };
        builder->SetInsertPoint( block );

        ScopeIdentifiers functionScope;

        // record the function arguments
        for ( auto & arg : function->args() )
        {
            /**
             * Since we do not support mutable function arguments,
             * we don't need to create alloca for each argument.
             */
            functionScope[ std::string{ arg.getName() }] = &arg;
        }

        for ( std::size_t i{ functionBodyStartIdx }; i < std::size( nodes ); i++ )
        {
            if ( std::find( std::begin( returnStatementsIndices ), std::end( returnStatementsIndices ), i ) == std::end( returnStatementsIndices ) )
            {
                codegenImpl( nodes[ i ], functionScope );
            }
            else
            {
                if ( auto * return_{ codegenImpl( nodes[ i ], functionScope ) } )
                {
                    builder->CreateRet( return_ );
                }
            }
        }

        if ( returnTypeIdx == 0 )
        {
            builder->CreateRet( nullptr );
        }

        // validate the generated code, checking for consistency
        verifyFunction( *function );

        nonStdFunctions[ functionName ] = function;

        return function;
    }

    template< typename ... T >
    llvm::Value * codegenAssignerDefinition
    (
        IRBuilderBinaryClbk<T ...>       const   clbk,
        std::span< Node::Pointer const > const   nodes,
        std::string_view                 const   opName,
        ScopeIdentifiers                       & scope
    )
    {
        ASSERT( std::size( nodes ) == 2U || std::size( nodes ) == 3U);

        ASSERT( nodes[ 0 ]->is< Identifier >() );
        auto const name{ nodes[ 0 ]->get< Identifier >().name };

        auto * variable = getFromScope(scope, name);

        if (variable == nullptr){return nullptr;}
        auto * lhsVal = builder->CreateLoad(llvm::Type::getDoubleTy( *context ), variable, name.c_str());

        auto * rhs{ codegenImpl( nodes[ 1U ], scope ) };
        if ( nodes[1]->is< TypeID >() )
        {
            // TODO: if this is a type declaration, we should cast the result to that type
            rhs = codegenImpl( nodes[ 2U ], scope );
        }

        builder->CreateStore(( *builder.*clbk )( lhsVal, rhs, opName, T{} ... ), scope[name]);

        return scope[name];
    }

    llvm::Value * codegenVariableDefinition( std::span< Node::Pointer const > const nodes, ScopeIdentifiers & scope )
    {
        ASSERT( std::size( nodes ) >= 2U );

        ASSERT( nodes[ 0U ]->is< Identifier >() );
        auto const & name{ nodes[ 0U ]->get< Identifier >().name };

        auto * parent{ builder->GetInsertBlock()->getParent() };
        llvm::IRBuilder<> inScopeBuilder{ &parent->getEntryBlock(), parent->getEntryBlock().begin() };

        auto * value{ create( inScopeBuilder, std::size( nodes ) <= 2U ? nodes[ 1U ] : nodes[ 2U ], name, scope ) };
        scope[ name ] = value;
        return value;
    }
} // namespace

std::unique_ptr< Module > codegen
(
    Node::Pointer    const & node,
    llvm::DataLayout const   dataLayout,
    std::string_view const   targetTriple
)
{
    context = std::make_unique< llvm::LLVMContext >();
    builder = std::make_unique< llvm::IRBuilder<> >( *context );
    module_ = std::make_unique< llvm::Module >( "Module", *context );

    // optimizations benefit from knowing about the target and data layout
    module_->setDataLayout  ( dataLayout   );
    module_->setTargetTriple( targetTriple );

    stdFunctions[ "print" ] = module_->getOrInsertFunction
    (
        "printf",
        llvm::FunctionType::get
        (
            llvm::IntegerType::getInt32Ty( *context ),
            llvm::PointerType::get( llvm::Type::getInt8Ty( *context ), 0 ),
            true
        )
    );

    auto * mainFunctionType{ llvm::FunctionType::get( llvm::Type::getVoidTy( *context ), false ) };
    auto * mainFunction    { llvm::Function::Create( mainFunctionType, llvm::Function::ExternalLinkage, "main", *module_ ) };

    auto * mainBlock{ llvm::BasicBlock::Create( *context, "entry", mainFunction ) };
    builder->SetInsertPoint( mainBlock );

    ASSERT( node->is< NodeType >() && node->get< NodeType >() == NodeType::ModuleDefinition );

    ScopeIdentifiers moduleScope;

    for ( auto const & n : node->children() )
    {
        if ( n == nullptr ) { continue; }

        if
        (
            n->is< NodeType >() &&
            (
                n->get< NodeType >() == NodeType::ContractDefinition ||
                n->get< NodeType >() == NodeType::FunctionDefinition
            )
        )
        {
            if ( n != nullptr )
            {
                codegenImpl( n, moduleScope );
            }
        }
        else
        {
            builder->SetInsertPoint( mainBlock );
            codegenImpl( n, moduleScope );
        }
    }

    builder->CreateRetVoid();

    return std::move( module_ );
}

} // namespace A1::LLVM