/**
 *
 * Copyright (c)2022 The Adamnite C++ Authors.
 *
 * This code is open-sourced under the MIT license.
 */

#pragma once

#include <CoreLib/Utils/Stream.hpp>

#include <string_view>
#include <cstdint>

namespace A1
{

enum class ReservedToken : std::uint8_t
{
    Unknown,

    OpParenthesisOpen,
    OpParenthesisClose,
    OpSubscriptOpen,
    OpSubscriptClose,

    OpArrow,
    OpColon,
    OpComma,
    OpDot,

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
    KwAddress,
    KwAnd,
    KwArray,
    KwAs,
    KwAssert,
    KwBool,
    KwBreak,
    KwClass,
    KwContinue,
    KwContract,
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
    KwI8,
    KwI16,
    KwI32,
    KwI64,
    KwLambda,
    KwLet,
    KwMap,
    KwNon,
    KwNone,
    KwNonLocal,
    KwNot,
    KwNum,
    KwOr,
    KwPass,
    KwRaise,
    KwReturn,
    KwStr,
    KwTrue,
    KwTry,
    KwU8,
    KwU16,
    KwU32,
    KwU64,
    KwWhile,
    KwWith,
    KwYield,

    // Number of possible reserved tokens
    Count
};

[[ nodiscard ]] bool isTypeSpecifier( ReservedToken const token ) noexcept;

[[ nodiscard ]] std::string_view toStringView( ReservedToken const token ) noexcept;

[[ nodiscard ]] ReservedToken getKeyword ( std::string_view const word ) noexcept;
[[ nodiscard ]] ReservedToken getOperator( Stream & stream ) noexcept;

} // namespace A1