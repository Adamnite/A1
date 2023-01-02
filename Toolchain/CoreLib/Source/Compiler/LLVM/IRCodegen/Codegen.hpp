/**
 *
 * Copyright (c)2022 The Adamnite C++ Authors.
 *
 * This code is open-sourced under the MIT license.
 */

#pragma once

#include "Compiler/LLVM/Context.hpp"

#include <CoreLib/AST/ASTNode.hpp>

namespace llvm
{
    // fwd
    class DataLayout;
} // namespace llvm

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