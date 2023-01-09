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

    Num,
    Str,

    I8,
    I16,
    I32,
    I64,

    U8,
    U16,
    U32,
    U64,

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

    TypeID                       returnTypeID{ nullptr };
    std::vector< ParameterType > parameterTypeIDs{};
};

namespace Registry
{

[[ nodiscard ]] TypeID getHandle( Type const & type ) noexcept;

[[ nodiscard ]] TypeID getNumHandle() noexcept;
[[ nodiscard ]] TypeID getStrHandle() noexcept;

[[ nodiscard ]] TypeID getI8Handle () noexcept;
[[ nodiscard ]] TypeID getI16Handle() noexcept;
[[ nodiscard ]] TypeID getI32Handle() noexcept;
[[ nodiscard ]] TypeID getI64Handle() noexcept;

[[ nodiscard ]] TypeID getU8Handle () noexcept;
[[ nodiscard ]] TypeID getU16Handle() noexcept;
[[ nodiscard ]] TypeID getU32Handle() noexcept;
[[ nodiscard ]] TypeID getU64Handle() noexcept;

} // namespace Registry

} // namespace A1