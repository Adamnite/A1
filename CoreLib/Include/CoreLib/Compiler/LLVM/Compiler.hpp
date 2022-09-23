/**
 *
 * Copyright (c)2022 The Adamnite C++ Authors.
 *
 * This code is open-sourced under the MIT license.
 */

#pragma once

#include <CoreLib/Compiler/Settings.hpp>
#include <CoreLib/Parser/ExpressionTreeNode.hpp>

namespace A1::LLVM
{

[[ nodiscard ]] bool compile( Compiler::Settings const settings, Node::Pointer const & node );

} // namespace A1::LLVM