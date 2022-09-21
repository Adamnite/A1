/**
 *
 * Copyright (c)2022 The Adamnite C++ Authors.
 *
 * This code is open-sourced under the MIT license.
 */

#pragma once

#include <CoreLib/Tokenizer/Token.hpp>
#include <CoreLib/Utils/Macros.hpp>
#include <CoreLib/Types.hpp>

#include <variant>
#include <cstdint>
#include <memory>
#include <vector>

namespace A1
{

enum class NodeType : std::uint8_t
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

    StatementIf,             // if <expression>
    StatementWhile,          // while <expression>
    StatementPass,           // pass
    StatementReturn,         // return <expression>

    ModuleDefinition,
    ContractDefinition,
    FunctionDefinition,
    FunctionParameterDefinition,
    VariableDefinition,

    // Number of possible node operations
    Count
};

class Node
{
public:
    using Pointer   = std::unique_ptr< Node >;
    using ValueType = std::variant
    <
        NodeType,   // Non-leaf nodes in expression tree
        Identifier, // Names of variables, functions, smart contracts etc.
        Number,     // Both integers and decimal numbers. AKA, i32, i64, f32, or f64
        String,     // String literals
        TypeID      // Type declaration for variables, function parameters, return values etc.
    >;

    Node
    (
        ValueType         value,
        std::size_t const lineNumber = 0U,
        std::size_t const charIndex  = 0U
    );

    Node
    (
        ValueType                    value,
        std::vector< Pointer >       children,
        std::size_t            const lineNumber = 0U,
        std::size_t            const charIndex  = 0U
    );

    template< typename T >
    [[ nodiscard ]] bool is() const noexcept
    {
        return std::holds_alternative< T >( value_ );
    }

    template< typename T >
    [[ nodiscard ]] T const & get() const noexcept
    {
        ASSERT( is< T >() );
        return std::get< T >( value_ );
    }

    [[ nodiscard ]] ValueType              const & value   () const noexcept { return value_;    }
    [[ nodiscard ]] std::vector< Pointer > const & children() const noexcept { return children_; }

    [[ nodiscard ]] std::size_t lineNumber() const noexcept { return lineNumber_; }
    [[ nodiscard ]] std::size_t charIndex () const noexcept { return charIndex_;  }

private:
    ValueType              value_;
    std::vector< Pointer > children_;

    std::size_t lineNumber_{ 0U };
    std::size_t charIndex_ { 0U };
};

} // namespace A1