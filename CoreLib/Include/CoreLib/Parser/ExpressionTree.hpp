/**
 *
 * Copyright (c)2022 The Adamnite C++ Authors.
 *
 * This code is open-sourced under the MIT license.
 */

#pragma once

#include <CoreLib/Parser/ExpressionTreeNode.hpp>
#include <CoreLib/Tokenizer/Tokenizer.hpp>

namespace A1
{

[[ nodiscard ]] Node::Pointer parse
(
    TokenIterator       & tokenIt,
    std::size_t   const   indentationLevelCount = 0U,
    bool          const   alreadyInModule       = true
);

} // namespace A1