/**
 *
 * Copyright (c)2022 The Adamnite C++ Authors.
 *
 * This code is open-sourced under the MIT license.
 */

#include <CoreLib/Parser/ExpressionTree.hpp>
#include <CoreLib/Types.hpp>

#include "Utils/Utils.hpp"

namespace A1
{

Node::Node
(
    ValueType         value,
    std::size_t const lineNumber,
    std::size_t const charIndex
)
: value_     { std::move( value  ) }
, lineNumber_{ lineNumber          }
, charIndex_ { charIndex           }
{}

Node::Node
(
    ValueType                    value,
    std::vector< Pointer >       children,
    std::size_t            const lineNumber,
    std::size_t            const charIndex
)
: value_     { std::move( value    ) }
, children_  { std::move( children ) }
, lineNumber_{ lineNumber            }
, charIndex_ { charIndex             }
{}

} // namespace A1