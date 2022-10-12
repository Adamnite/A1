/**
 *
 * Copyright (c)2022 The Adamnite C++ Authors.
 *
 * This code is open-sourced under the MIT license.
 */

#pragma once

#include "Context.hpp"

#include <CoreLib/Parser/ExpressionTreeNode.hpp>

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

namespace A1::LLVM
{

llvm::Value * codegen( Context & ctx, Node::Pointer const & node );

template< typename ... T >
using IRBuilderUnaryClbk = llvm::Value * ( llvm::IRBuilderBase::* )( llvm::Value *, llvm::Twine const &, T ... );

template< typename ... T >
[[ nodiscard ]]
llvm::Value * codegenUnary
(
    Context                                & ctx,
    IRBuilderUnaryClbk< T ... >      const   clbk,
    std::span< Node::Pointer const > const   nodes,
    std::string_view                 const   opName
);

template< typename ... T >
using IRBuilderBinaryClbk = llvm::Value * ( llvm::IRBuilderBase::* )( llvm::Value *, llvm::Value *, llvm::Twine const &, T ... );

template< typename ... T >
[[ nodiscard ]]
llvm::Value * codegenBinary
(
    Context                                & ctx,
    IRBuilderBinaryClbk< T ... >     const   clbk,
    std::span< Node::Pointer const > const   nodes,
    std::string_view                 const   opName
);

template< typename ... T >
[[ nodiscard ]]
llvm::Value * codegenAssign
(
    Context                                & ctx,
    IRBuilderBinaryClbk< T ... >     const   clbk,
    std::span< Node::Pointer const > const   nodes,
    std::string_view                 const   opName
);

[[ nodiscard ]] llvm::Value    * codegenCall              ( Context & ctx, std::span< Node::Pointer const > const nodes );
[[ nodiscard ]] llvm::Value    * codegenMemberCall        ( Context & ctx, std::span< Node::Pointer const > const nodes );
[[ nodiscard ]] llvm::Value    * codegenContractDefinition( Context & ctx, std::span< Node::Pointer const > const nodes );
[[ nodiscard ]] llvm::Function * codegenFunctionDefinition( Context & ctx, std::span< Node::Pointer const > const nodes );
[[ nodiscard ]] llvm::Value    * codegenVariableDefinition( Context & ctx, std::span< Node::Pointer const > const nodes );
[[ nodiscard ]] llvm::Value    * codegenControlFlow       ( Context & ctx, std::span< Node::Pointer const > const nodes );
[[ nodiscard ]] llvm::Value    * codegenLoopFlow          ( Context & ctx, std::span< Node::Pointer const > const nodes );

} // namespace A1::LLVM