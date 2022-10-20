/**
 *
 * Copyright (c)2022 The Adamnite C++ Authors.
 *
 * This code is open-sourced under the MIT license.
 */

#pragma once

#include <CoreLib/AST/ASTNode.hpp>
#include <CoreLib/Tokenizer/Tokenizer.hpp>

namespace A1
{

[[ nodiscard ]] Node::Pointer parse( TokenIterator & tokenIt );

} // namespace A1