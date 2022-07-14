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

    // Keywords
    And,
    As,
    Assert,
    Break,
    Class,
    Continue,
    Def,
    Del,
    Elif,
    Else,
    Except,
    False,
    Finally,
    For,
    From,
    Global,
    If,
    Import,
    In,
    Is,
    Lambda,
    Non,
    None,
    NonLocal,
    Not,
    Or,
    Pass,
    Raise,
    Return,
    True,
    Try,
    While,
    With,
    Yield,

    // Number of possible reserved tokens
    Count
};

[[ nodiscard ]] ReservedToken getKeyword( std::string_view const word ) noexcept;

[[ nodiscard ]] ReservedToken getOperator( PushBackStream & stream ) noexcept;
[[ nodiscard ]] ReservedToken getSymbol  ( PushBackStream & stream ) noexcept;

} // namespace A1