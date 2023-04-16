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

namespace Detail
{
    [[ nodiscard  ]]
    inline llvm::Value * load( Context & ctx, llvm::Value * value ) noexcept
    {
        if
        (
            value->getType()->getNumContainedTypes() > 0U &&
            value->getType()->getContainedType( 0U )->isIntegerTy( sizeof( Number::Type ) * 8U )
        )
        {
            return ctx.builder->CreateLoad( llvm::Type::getInt64Ty( *ctx.internalCtx ), value );
        }
        else if
        (
            value->getType()->isPointerTy() &&
            value->getType()->getNumContainedTypes() > 0U &&
            value->getType()->getContainedType( 0U )->isPointerTy()
        )
        {
            return ctx.builder->CreateLoad( value->getType()->getContainedType( 0U ), value );
        }
        return value;
    }
} // namespace Detail

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
    return ( *( ctx.builder ).*clbk )( Detail::load( ctx, lhs ), opName, T{} ... );
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
    return ( *( ctx.builder ).*clbk )( Detail::load( ctx, lhs ), Detail::load( ctx, rhs ), opName, T{} ... );
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
    ASSERT( std::size( nodes ) == 2U );

    auto * lhs{ codegen( ctx, nodes[ 0U ] ) };
    auto * rhs{ codegenBinary( ctx, clbk, nodes, opName ) };

    if ( lhs == nullptr || rhs == nullptr ) { return nullptr; }

    ctx.builder->CreateStore( rhs, lhs );
    return lhs;
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