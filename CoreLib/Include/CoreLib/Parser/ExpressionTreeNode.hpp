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

enum class OperatorType : std::uint8_t
{
    Unknown,

    Call,                    // ( <operand>, <operand>, ... )
    Index,                   // [ <operand> ]

    Exponent,                // <operand> ** <operand>

    UnaryPlus,               // + <operand>
    UnaryMinus,              // - <operand>

    Multiplication,          // <operand>  * <operand>
    Division,                // <operand>  / <operand>
    FloorDivision,           // <operand> // <operand>
    Modulus,                 // <operand>  % <operand>

    Addition,                // <operand> + <operand>
    Subtraction,             // <operand> - <operand>

    BitwiseLeftShift,        // <operand> << <operand>
    BitwiseRightShift,       // <operand> >> <operand>
    BitwiseAnd,              // <operand>  & <operand>
    BitwiseOr,               // <operand>  | <operand>
    BitwiseXor,              // <operand>  ^ <operand>
    BitwiseNot,              // ~ <operand>

    Equality,                // <operand> == <operand>
    Inequality,              // <operand> != <operand>
    GreaterThan,             // <operand>  > <operand>
    GreaterThanEqual,        // <operand> >= <operand>
    LessThan,                // <operand>  < <operand>
    LessThanEqual,           // <operand> <= <operand>

    IsIdentical,             // <operand> is     <operand>
    IsNotIdentical,          // <operand> is not <operand>
    IsMemberOf,              // <operand> in     <operand>
    IsNotMemberOf,           // <operand> not in <operand>

    LogicalNot,              // ! <operand>
    LogicalAnd,              // <operand> && <operand>
    LogicalOr,               // <operand> || <operand>

    Assign,                  // <operand> = <operand>

    AssignExponent,          // <operand> **= <operand>

    AssignAddition,          // <operand> += <operand>
    AssignSubtraction,       // <operand> -= <operand>

    AssignMultiplication,    // <operand>  *= <operand>
    AssignDivision,          // <operand>  /= <operand>
    AssignFloorDivision,     // <operand> //= <operand>
    AssignModulus,           // <operand>  %= <operand>

    AssignBitwiseLeftShift,  // <operand> <<= <operand>
    AssignBitwiseRightShift, // <operand> >>= <operand>
    AssignBitwiseAnd,        // <operand>  &= <operand>
    AssignBitwiseOr,         // <operand>  |= <operand>
    AssignBitwiseXor,        // <operand>  ^= <operand>

    // Number of possible node operations
    Count
};

class Node
{
public:
    using Pointer   = std::unique_ptr< Node >;
    using ValueType = std::variant< Identifier, OperatorType, std::string, double >;

    Node
    (
        ValueType         value,
        std::size_t const lineNumber,
        std::size_t const charIndex
    );

    Node
    (
        ValueType                    value,
        std::vector< Pointer >       children,
        std::size_t            const lineNumber,
        std::size_t            const charIndex
    );

    [[ nodiscard ]] ValueType              const & value   () const noexcept { return value_;    }
    [[ nodiscard ]] std::vector< Pointer > const & children() const noexcept { return children_; }

private:
    ValueType              value_;
    std::vector< Pointer > children_;

    TypeID typeID_{ nullptr };

    [[ maybe_unused ]] std::size_t lineNumber_{ 0U };
    [[ maybe_unused ]] std::size_t charIndex_ { 0U };
};

} // namespace A1