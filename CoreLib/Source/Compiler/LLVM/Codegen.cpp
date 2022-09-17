/**
 *
 * Copyright (c)2022 The Adamnite C++ Authors.
 *
 * This code is open-sourced under the MIT license.
 */

#include <CoreLib/Compiler/LLVM/Codegen.hpp>

#include "Utils/Macros.hpp"
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
    std::unique_ptr< llvm::LLVMContext > context;
    std::unique_ptr< llvm::IRBuilder<> > builder;
    std::unique_ptr< llvm::Module      > module_;

    template< typename ... T >
    using IRBuilderUnaryClbk = llvm::Value * ( llvm::IRBuilderBase::* )( llvm::Value *, llvm::Twine const &, T ... );

    template< typename ... T >
    llvm::Value * codegenUnaryExpression
    (
        IRBuilderUnaryClbk< T ... >      const,
        std::span< Node::Pointer const > const,
        std::string_view                 const
    );

    template< typename ... T >
    using IRBuilderBinaryClbk = llvm::Value * ( llvm::IRBuilderBase::* )( llvm::Value *, llvm::Value *, llvm::Twine const &, T ... );

    template< typename ... T >
    llvm::Value * codegenBinaryExpression
    (
        IRBuilderBinaryClbk< T ... >     const,
        std::span< Node::Pointer const > const,
        std::string_view                 const
    );

    llvm::Function * codegenFunctionDefinitionExpression( std::span< Node::Pointer const > const );

    llvm::Value * codegenImpl( Node::Pointer const & node )
    {
        if ( node == nullptr )
        {
            return nullptr;
        }

        return std::visit
        (
            Overload
            {
                [ & ]( NodeType const type ) -> llvm::Value *
                {
                    switch ( type ){
                        case NodeType::Multiplication:
                            return codegenBinaryExpression( &llvm::IRBuilder<>::CreateFMul, node->children(), "multmp" );
                        case NodeType::Division:
                            return codegenBinaryExpression( &llvm::IRBuilder<>::CreateFDiv, node->children(), "divtmp" );
                        case NodeType::FloorDivision:
                            return codegenBinaryExpression( &llvm::IRBuilder<>::CreateSDiv, node->children(), "fdivtmp" );
                        case NodeType::Modulus:
                            return codegenBinaryExpression( &llvm::IRBuilder<>::CreateFRem, node->children(), "modtmp" );
                        case NodeType::Addition:
                            return codegenBinaryExpression( &llvm::IRBuilder<>::CreateFAdd, node->children(), "addtmp" );
                        case NodeType::Subtraction:
                            return codegenBinaryExpression( &llvm::IRBuilder<>::CreateFSub, node->children(), "subtmp" );
                        case NodeType::BitwiseLeftShift:
                            return codegenBinaryExpression( &llvm::IRBuilder<>::CreateShl, node->children(), "lshtmp" );
                        case NodeType::BitwiseRightShift:
                            return codegenBinaryExpression( &llvm::IRBuilder<>::CreateAShr, node->children(), "rshtmp" );
                        case NodeType::BitwiseAnd:
                            return codegenBinaryExpression( &llvm::IRBuilder<>::CreateAnd, node->children(), "andtmp" );
                        case NodeType::BitwiseOr:
                            return codegenBinaryExpression( &llvm::IRBuilder<>::CreateOr, node->children(), "ortmp" );
                        case NodeType::BitwiseXor:
                            return codegenBinaryExpression( &llvm::IRBuilder<>::CreateXor, node->children(), "xortmp" );
                        case NodeType::BitwiseNot:
                            return codegenUnaryExpression( &llvm::IRBuilder<>::CreateNot, node->children(), "nottmp" );
                        case NodeType::Equality:
                            return codegenBinaryExpression( &llvm::IRBuilder<>::CreateFCmpOEQ, node->children(), "eqtmp" );
                        case NodeType::Inequality:
                            return codegenBinaryExpression( &llvm::IRBuilder<>::CreateFCmpONE, node->children(), "netmp" );
                        case NodeType::FunctionDefinition:
                            return codegenFunctionDefinitionExpression( node->children() );
                        default:
                            return nullptr;
                    }
                },
                []( Identifier const & ) -> llvm::Value *
                {
                    return nullptr;
                },
                []( Number const number ) -> llvm::Value *
                {
                    return llvm::ConstantFP::get( *context, llvm::APFloat( number ) );
                },
                []( String const & str ) -> llvm::Value *
                {
                    return llvm::ConstantDataArray::getString( *context, str.data() );
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
    llvm::Value * codegenUnaryExpression
    (
        IRBuilderUnaryClbk< T ... >      const clbk,
        std::span< Node::Pointer const > const nodes,
        std::string_view                 const name
    )
    {
        ASSERT( std::size( nodes ) == 1U );
        auto * lhs{ codegenImpl( nodes[ 0U ] ) };
        return ( *builder.*clbk )( lhs, name, T{} ... );
    }

    template< typename ... T >
    llvm::Value * codegenBinaryExpression
    (
        IRBuilderBinaryClbk< T ... >     const clbk,
        std::span< Node::Pointer const > const nodes,
        std::string_view                 const name
    )
    {
        ASSERT( std::size( nodes ) == 2U );
        auto * lhs{ codegenImpl( nodes[ 0U ] ) };
        auto * rhs{ codegenImpl( nodes[ 1U ] ) };
        return ( *builder.*clbk )( lhs, rhs, name, T{} ... );
    }

    llvm::Function * codegenFunctionDefinitionExpression( std::span< Node::Pointer const > const children )
    {
        auto functionBodyStartIdx{ 0 };

        std::vector< std::string > paramNames;
        for ( auto i{ 0U }; i < std::size( children ); i++ )
        {
            auto const & child{ children[ i ] };
            if
            (
                std::holds_alternative< NodeType >( child->value() ) &&
                std::get< NodeType >( child->value() ) == NodeType::FunctionParameterDefinition
            )
            {
                auto const & paramChildren{ child->children() };
                paramNames.push_back( std::get< Identifier >( paramChildren[ 0 ]->value() ).name );
            }
            else if
            (
                !std::holds_alternative< Identifier >( child->value() ) &&
                !std::holds_alternative< TypeID     >( child->value() )
            )
            {
                functionBodyStartIdx = i;
            }
        }

        // create function definition
        std::vector< llvm::Type * > params{ std::size( paramNames ), llvm::Type::getDoubleTy( *context ) };
        auto * functionType{ llvm::FunctionType::get( llvm::Type::getDoubleTy( *context ), params, false ) };

        auto * function
        {
            llvm::Function::Create
            (
                functionType,
                llvm::Function::ExternalLinkage,
                std::get< Identifier >( children[ 0 ]->value() ).name,
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

        std::map< std::string, llvm::Value * > identifiersInScope;

        // record the function arguments
        for ( auto & arg : function->args() )
        {
            identifiersInScope[ std::string{ arg.getName() }] = &arg;
        }

        // TODO: pass identifiers in scope to the body codegen
        if ( auto * return_{ codegenImpl( children[ functionBodyStartIdx ] ) } )
        {
            builder->CreateRet( return_ );

            // validate the generated code, checking for consistency
            verifyFunction( *function );

            return function;
        }

        // error while reading the body, remove function.
        function->eraseFromParent();
        return nullptr;
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

    /**
     * Optimizations benefit from knowing about the target and data layout.
     */
    module_->setDataLayout  ( dataLayout   );
    module_->setTargetTriple( targetTriple );

    codegenImpl( node );

    return std::move( module_ );
}

} // namespace A1::LLVM