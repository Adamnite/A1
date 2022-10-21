/**
 *
 * Copyright (c)2022 The Adamnite C++ Authors.
 *
 * This code is open-sourced under the MIT license.
 */

#pragma once

#include <CoreLib/AST/ASTNode.hpp>

#include <cstdio>

namespace A1::AST
{

void print( Node::Pointer const & node, std::FILE * stream = stdout, std::size_t const indentationLevel = 0 );

} // namespace A1::AST