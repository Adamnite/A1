/**
 *
 * Copyright (c)2022 The Adamnite C++ Authors.
 *
 * This code is open-sourced under the MIT license.
 */

#pragma once

#include "../Context.hpp"
#include "CodegenVisitor.hpp"

#include <CoreLib/AST/ASTNode.hpp>
#include <CoreLib/Utils/Macros.hpp>

#if defined (__clang__)
#   pragma clang diagnostic push
#   pragma clang diagnostic ignored "-Wunused-parameter"
#elif defined(__GNUC__) || defined(__GNUG__)
#   pragma GCC diagnostic push
#   pragma GCC diagnostic ignored "-Wunused-parameter"
#endif

#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Value.h>

#if defined(__clang__)
#   pragma clang diagnostic pop
#elif defined(__GNUC__) || defined(__GNUG__)
#   pragma GCC diagnostic pop
#endif

#include <string_view>
#include <span>

namespace A1::LLVM::IR
{

template< typename ... T >
using IRBuilderUnaryClbk = llvm::Value * ( llvm::IRBuilderBase::* )( llvm::Value *, llvm::Twine const &, T ... );

template< typename ... T >
[[ nodiscard ]]
llvm::Value * codegenUnary
(
    Context                                     & ctx,
    IRBuilderUnaryClbk< T ... >           const   clbk,
    std::span< AST::Node::Pointer const > const   nodes,
    std::string_view                      const   opName
)
{
    ASSERT( std::size( nodes ) == 1U );
    auto * lhs{ codegen( ctx, nodes[ 0U ] ) };

    if ( lhs == nullptr ) { return nullptr; }

    if ( lhs->getType()->getNumContainedTypes() > 0U && lhs->getType()->getContainedType( 0U )->isIntegerTy( sizeof( Number ) * 8U ) )
    {
        lhs = ctx.builder->CreateLoad( llvm::Type::getInt32Ty( *ctx.internalCtx ), lhs );
    }

    return ( *( ctx.builder ).*clbk )( lhs, opName, T{} ... );
}

template< typename ... T >
using IRBuilderBinaryClbk = llvm::Value * ( llvm::IRBuilderBase::* )( llvm::Value *, llvm::Value *, llvm::Twine const &, T ... );

template< typename ... T >
[[ nodiscard ]]
llvm::Value * codegenBinary
(
    Context                                     & ctx,
    IRBuilderBinaryClbk< T ... >          const   clbk,
    std::span< AST::Node::Pointer const > const   nodes,
    std::string_view                      const   opName
)
{
    ASSERT( std::size( nodes ) == 2U );
    auto * lhs{ codegen( ctx, nodes[ 0U ] ) };
    auto * rhs{ codegen( ctx, nodes[ 1U ] ) };

    if ( lhs == nullptr || rhs == nullptr ) { return nullptr; }

    if ( lhs->getType()->getNumContainedTypes() > 0U && lhs->getType()->getContainedType( 0U )->isIntegerTy( sizeof( Number ) * 8U ) )
    {
        lhs = ctx.builder->CreateLoad( llvm::Type::getInt32Ty( *ctx.internalCtx ), lhs );
    }

    if ( rhs->getType()->getNumContainedTypes() > 0U && rhs->getType()->getContainedType( 0U )->isIntegerTy( sizeof( Number ) * 8U ) )
    {
        rhs = ctx.builder->CreateLoad( llvm::Type::getInt32Ty( *ctx.internalCtx ), rhs );
    }

    return ( *( ctx.builder ).*clbk )( lhs, rhs, opName, T{} ... );
}

template< typename ... T >
[[ nodiscard ]]
llvm::Value * codegenAssign
(
    Context                                     & ctx,
    IRBuilderBinaryClbk< T ... >          const   clbk,
    std::span< AST::Node::Pointer const > const   nodes,
    std::string_view                      const   opName
)
{
    ASSERTM( std::size( nodes ) == 2U, "Assign expression consists of an identifier and value to be assigned" );

    ASSERTM( nodes[ 0U ]->is< Identifier >(), "Variable identifier is the first child node in the assign expression" );
    auto const & name{ nodes[ 0U ]->get< Identifier >().name };

    auto * value{ ctx.symbols.variable( name ) };
    if ( value == nullptr ) { return nullptr; }

    ctx.builder->CreateStore( codegenBinary( ctx, clbk, nodes, opName ), value );
    return value;
}

[[ nodiscard ]] llvm::Value    * codegenAssign            ( Context & ctx, std::span< AST::Node::Pointer const > const nodes );
[[ nodiscard ]] llvm::Value    * codegenCall              ( Context & ctx, std::span< AST::Node::Pointer const > const nodes );
[[ nodiscard ]] llvm::Value    * codegenMemberCall        ( Context & ctx, std::span< AST::Node::Pointer const > const nodes );
[[ nodiscard ]] llvm::Value    * codegenContractDefinition( Context & ctx, std::span< AST::Node::Pointer const > const nodes );
[[ nodiscard ]] llvm::Function * codegenFunctionDefinition( Context & ctx, std::span< AST::Node::Pointer const > const nodes );
[[ nodiscard ]] llvm::Value    * codegenVariableDefinition( Context & ctx, std::span< AST::Node::Pointer const > const nodes );
[[ nodiscard ]] llvm::Value    * codegenControlFlow       ( Context & ctx, std::span< AST::Node::Pointer const > const nodes );
[[ nodiscard ]] llvm::Value    * codegenLoopFlow          ( Context & ctx, std::span< AST::Node::Pointer const > const nodes );
[[ nodiscard ]] llvm::Value    * codegenAssert            ( Context & ctx, std::span< AST::Node::Pointer const > const nodes );

} // namespace A1::LLVM::IR