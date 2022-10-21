/**
 *
 * Copyright (c)2022 The Adamnite C++ Authors.
 *
 * This code is open-sourced under the MIT license.
 */

#include <CoreLib/AST/AST.hpp>
#include <CoreLib/Types.hpp>

#include "Utils/Utils.hpp"

namespace A1::AST
{

Node::Node( ValueType value, ErrorInfo errorInfo )
: value_    { std::move( value     ) }
, errorInfo_{ std::move( errorInfo ) }
{}

Node::Node ( ValueType value, std::vector< Pointer > children, ErrorInfo errorInfo )
: value_    { std::move( value     ) }
, children_ { std::move( children  ) }
, errorInfo_{ std::move( errorInfo ) }
{}

} // namespace A1::AST