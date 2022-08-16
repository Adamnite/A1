/**
 *
 * Copyright (c)2022 The Adamnite C++ Authors.
 *
 * This code is open-sourced under the MIT license.
 */

#pragma once

#include <CoreLib/Utils/PushBackStream.hpp>

#include <string_view>
#include <cstdint>

namespace A1
{

enum class ReservedToken : std::uint8_t
{
    Unknown,

    OpCallOpen,
    OpCallClose,
    OpIndexOpen,
    OpIndexClose,

    OpColon,
    OpComma,

    // Arithmetic operators
    OpAdd,
    OpDiv,
    OpExp,
    OpFloorDiv,
    OpMod,
    OpMul,
    OpSub,

    // Bitwise operators
    OpBitwiseAnd,
    OpBitwiseLeftShift,
    OpBitwiseNot,
    OpBitwiseOr,
    OpBitwiseRightShift,
    OpBitwiseXor,

    // Logical operators
    OpLogicalAnd,
    OpLogicalNot,
    OpLogicalOr,

    // Assignment operators
    OpAssign,
    OpAssignAdd,
    OpAssignDiv,
    OpAssignExp,
    OpAssignFloorDiv,
    OpAssignMod,
    OpAssignMul,
    OpAssignSub,

    // Bitwise assignment operators
    OpAssignBitwiseAnd,
    OpAssignBitwiseLeftShift,
    OpAssignBitwiseOr,
    OpAssignBitwiseRightShift,
    OpAssignBitwiseXor,

    // Comparison operators
    OpEqual,
    OpGreaterThan,
    OpGreaterThanEqual,
    OpLessThan,
    OpLessThanEqual,
    OpNotEqual,

    // Keywords
    KwAnd,
    KwAs,
    KwAssert,
    KwBreak,
    KwClass,
    KwContinue,
    KwDef,
    KwDel,
    KwElif,
    KwElse,
    KwExcept,
    KwFalse,
    KwFinally,
    KwFor,
    KwFrom,
    KwGlobal,
    KwIf,
    KwImport,
    KwIn,
    KwIs,
    KwLambda,
    KwNon,
    KwNone,
    KwNonLocal,
    KwNot,
    KwOr,
    KwPass,
    KwRaise,
    KwReturn,
    KwTrue,
    KwTry,
    KwWhile,
    KwWith,
    KwYield,

    // Number of possible reserved tokens
    Count
};

[[ nodiscard ]] std::string_view toStringView( ReservedToken const token ) noexcept;

[[ nodiscard ]] ReservedToken getKeyword ( std::string_view const   word   ) noexcept;
[[ nodiscard ]] ReservedToken getOperator( PushBackStream         & stream ) noexcept;

} // namespace A1