/**
 *
 * Copyright (c)2022 The Adamnite C++ Authors.
 *
 * This code is open-sourced under the MIT license.
 */

#pragma once

#include <cstdint>
#include <variant>
#include <vector>

namespace A1
{

enum class PrimitiveType : std::uint8_t
{
    Unknown,

    Number,
    StringLiteral,

    // Number of possible primitive types
    Count
};

// Forward declarations

struct ArrayType;
struct FunctionType;

using Type   = std::variant< PrimitiveType, ArrayType, FunctionType >;
using TypeID = Type const *;

struct ArrayType
{
    TypeID innerTypeID{ nullptr };
};

struct FunctionType
{
    struct ParameterType
    {
        TypeID typeID{ nullptr };
    };

    TypeID                       returnTypeID    { nullptr };
    std::vector< ParameterType > parameterTypeIDs{};
};

} // namespace A1