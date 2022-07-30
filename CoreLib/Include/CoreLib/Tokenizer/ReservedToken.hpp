/**
 *
 * Copyright (c)2022 The Adamnite C++ Authors.
 *
 * This code is open-sourced under the MIT license.
 */

#pragma once

#include <CoreLib/PushBackStream.hpp>

#include <string_view>
#include <cstdint>

namespace A1
{

enum class ReservedToken : std::uint8_t
{
    Unknown,

    // Arithmetic operators
    OpAdd,
    OpDiv,
    OpExp,
    OpFloorDiv,
    OpMod,
    OpMul,
    OpSub,

    // Bitwise operators
    OpAnd,
    OpLeftShift,
    OpNot,
    OpOr,
    OpRightShift,
    OpXor,

    // Assignment operators
    OpAssign,
    OpAddAssign,
    OpDivAssign,
    OpExpAssign,
    OpFloorDivAssign,
    OpModAssign,
    OpMulAssign,
    OpSubAssign,

    // Bitwise assignment operators
    OpAndAssign,
    OpOrAssign,
    OpXorAssign,
    OpLeftShiftAssign,
    OpRightShiftAssign,

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