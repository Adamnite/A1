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
    class Value;
} // namespace llvm

namespace A1::LLVM::IR
{

llvm::Value * codegen( Context & ctx, AST::Node::Pointer const & node );

} // namespace A1::LLVM::IR