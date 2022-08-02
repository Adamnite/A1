/**
 *
 * Copyright (c)2022 The Adamnite C++ Authors.
 *
 * This code is open-sourced under the MIT license.
 */

#pragma once

#include <CoreLib/Tokenizer/Token.hpp>
#include <CoreLib/Types.hpp>

#include <variant>
#include <cstdint>
#include <memory>
#include <vector>

namespace A1
{

enum class OperationType : std::uint8_t
{
    Unknown,

    Parameter,

    PreIncrement,
    PreDecrement,
    PostIncrement,
    PostDecrement,

    // Number of possible node operations
    Count
};

struct Node
{
    using Pointer   = std::unique_ptr< Node >;
    using ValueType = std::variant< Identifier, OperationType, std::string, double >;

    ValueType              value;
    std::vector< Pointer > children;

    TypeID typeID{ nullptr };

    std::size_t lineNumber{ 0U };
    std::size_t charIndex { 0U };

    Node
    (
        ValueType              value,
        std::vector< Pointer > children,
        std::size_t            lineNum
    );
};

} // namespace A1