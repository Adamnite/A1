/**
 *
 * Copyright (c)2022 The Adamnite C++ Authors.
 *
 * This code is open-sourced under the MIT license.
 */

#pragma once

#include <cstdint>

namespace A1
{

enum class ReservedToken : std::uint8_t
{
    inc,
    dec,

    add,
    sub,
    concat,
    mul,
    div,
    idiv,
    mod,

    bitwise_not,
    bitwise_and,
    bitwise_or,
    bitwise_xor,
    shiftl,
    shiftr,

    assign,

    add_assign,
    sub_assign,
    concat_assign,
    mul_assign,
    div_assign,
    idiv_assign,
    mod_assign,

    and_assign,
    or_assign,
    xor_assign,
    shiftl_assign,
    shiftr_assign,

    logical_not,
    logical_and,
    logical_or,

    eq,
    ne,
    lt,
    gt,
    le,
    ge,

    question,
    colon,

    comma,

    semicolon,

    open_round,
    close_round,

    open_curly,
    close_curly,

    open_square,
    close_square,

    kw_if,
    kw_else,
    kw_elif,

    kw_switch,
    kw_case,
    kw_default,

    kw_for,
    kw_while,
    kw_do,

    kw_break,
    kw_continue,
    kw_return,

    kw_var,
    kw_fun,

    kw_void,
    kw_number,
    kw_string,

    count
};

} // namespace A1