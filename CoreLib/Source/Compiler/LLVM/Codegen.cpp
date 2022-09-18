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
    using ScopeIdentifiers = std::map< std::string, llvm::Value * >;

    llvm::Value * getFromScope( ScopeIdentifiers const & scope, std::string const & name ) noexcept
    {
        if ( auto it{ scope.find( name ) }; it != std::end( scope ) )
        {
            return it->second;
        }
        return nullptr;
    }

    std::unique_ptr< llvm::LLVMContext > context;
    std::unique_ptr< llvm::IRBuilder<> > builder;
    std::unique_ptr< llvm::Module      > module_;

    template< typename ... T >
    using IRBuilderUnaryClbk = llvm::Value * ( llvm::IRBuilderBase::* )( llvm::Value *, llvm::Twine const &, T ... );

    template< typename ... T >
    llvm::Value * codegenUnary
    (
        IRBuilderUnaryClbk< T ... >      const,
        std::span< Node::Pointer const > const,
        std::string_view                 const,
        ScopeIdentifiers                 const &
    );

    template< typename ... T >
    using IRBuilderBinaryClbk = llvm::Value * ( llvm::IRBuilderBase::* )( llvm::Value *, llvm::Value *, llvm::Twine const &, T ... );

    template< typename ... T >
    llvm::Value * codegenBinary
    (
        IRBuilderBinaryClbk< T ... >     const,
        std::span< Node::Pointer const > const,
        std::string_view                 const,
        ScopeIdentifiers                 const &
    );

    llvm::Function * codegenFunctionDefinition( std::span< Node::Pointer const > const );

    llvm::Value * codegenImpl( Node::Pointer const & node, ScopeIdentifiers const & scope )
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
                        case NodeType::FunctionDefinition:
                            return codegenFunctionDefinition( node->children() );
                        case NodeType::StatementReturn:
                        {
                            ASSERT( std::size( node->children() ) == 1U );
                            return codegenImpl( node->children()[ 0 ], scope );
                        }
                        default:
                            return nullptr;
                    }
                },
                [ & ]( Identifier const & identifier ) -> llvm::Value *
                {
                    return getFromScope( scope, identifier.name );
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
    llvm::Value * codegenUnary
    (
        IRBuilderUnaryClbk< T ... >      const   clbk,
        std::span< Node::Pointer const > const   nodes,
        std::string_view                 const   name,
        ScopeIdentifiers                 const & scope
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
        ScopeIdentifiers                 const & scope
    )
    {
        ASSERT( std::size( nodes ) == 2U );
        auto * lhs{ codegenImpl( nodes[ 0U ], scope ) };
        auto * rhs{ codegenImpl( nodes[ 1U ], scope ) };

        if ( lhs == nullptr || rhs == nullptr ) { return nullptr; }

        return ( *builder.*clbk )( lhs, rhs, name, T{} ... );
    }

    llvm::Function * codegenFunctionDefinition( std::span< Node::Pointer const > const children )
    {
        ASSERT( std::size( children ) >= 2U );

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

        ScopeIdentifiers functionScope;

        // record the function arguments
        for ( auto & arg : function->args() )
        {
            functionScope[ std::string{ arg.getName() }] = &arg;
        }

        if ( auto * return_{ codegenImpl( children[ functionBodyStartIdx ], functionScope ) } )
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

    // optimizations benefit from knowing about the target and data layout
    module_->setDataLayout  ( dataLayout   );
    module_->setTargetTriple( targetTriple );

    ScopeIdentifiers moduleScope;

    codegenImpl( node, moduleScope );

    return std::move( module_ );
}

} // namespace A1::LLVM