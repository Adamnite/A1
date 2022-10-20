/**
 *
 * Copyright (c)2022 The Adamnite C++ Authors.
 *
 * This code is open-sourced under the MIT license.
 */

#pragma once

#include "../Context.hpp"

#include <CoreLib/AST/ASTNode.hpp>

#if defined (__clang__)
#   pragma clang diagnostic push
#   pragma clang diagnostic ignored "-Wunused-parameter"
#elif defined(__GNUC__) || defined(__GNUG__)
#   pragma GCC diagnostic push
#   pragma GCC diagnostic ignored "-Wunused-parameter"
#endif

#include <llvm/IR/DataLayout.h>
#include <llvm/IR/Module.h>

#if defined(__clang__)
#   pragma clang diagnostic pop
#elif defined(__GNUC__) || defined(__GNUG__)
#   pragma GCC diagnostic pop
#endif

namespace A1::LLVM::IR
{

[[ nodiscard ]]
Context codegen
(
    AST::Node::Pointer const & moduleNode,
    llvm::DataLayout   const   dataLayout,
    std::string_view   const   targetTriple
);

} // namespace A1::LLVM::IR