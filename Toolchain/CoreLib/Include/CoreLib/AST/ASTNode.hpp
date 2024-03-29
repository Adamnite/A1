/**
 *
 * Copyright (c)2022 The Adamnite C++ Authors.
 *
 * This code is open-sourced under the MIT license.
 */

#pragma once

#include <CoreLib/Tokenizer/Token.hpp>
#include <CoreLib/Errors/ErrorInfo.hpp>
#include <CoreLib/Utils/Macros.hpp>
#include <CoreLib/Types.hpp>

#include <variant>
#include <cstdint>
#include <memory>
#include <vector>

namespace A1::AST
{

enum class NodeType : std::uint8_t
{
    Unknown,

    Call,                    // <operand>( <operand>, <operand>, ... )
    Parentheses,             // ( <operand>, <operand>, ... )
    Index,                   // [ <operand> ]
    MemberCall,              // <operand>.<operand>

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
    StatementElif,           // elif <expression>
    StatementElse,           // else
    StatementWhile,          // while <expression>
    StatementPass,           // pass
    StatementReturn,         // return <expression>
    StatementImport,         // import <expression>
    StatementAssert,         // assert <expression>

    ArrayDefinition,
    ClassDefinition,
    ContractDefinition,
    FunctionDefinition,
    FunctionParameterDefinition,
    MapDefinition,
    ModuleDefinition,
    VariableDefinition,

    // Number of possible node operations
    Count
};

using Boolean = bool;

class Node
{
public:
    using Pointer   = std::unique_ptr< Node >;
    using ValueType = std::variant
    <
        NodeType,   // Non-leaf nodes in expression tree
        Identifier, // Names of variables, functions, smart contracts etc.
        TypeID,     // Type declaration for variables, function parameters, return values etc.
        Boolean,
        Number,
        StringLiteral
    >;

    Node( ValueType value, ErrorInfo errorInfo = {} );
    Node( ValueType value, std::vector< Pointer > children, ErrorInfo errorInfo = {} );

    template< typename T >
    [[ nodiscard ]] bool is() const noexcept
    {
        return std::holds_alternative< T >( value_ );
    }

    template< typename T >
    [[ nodiscard ]] bool is_not() const noexcept
    {
        return !std::holds_alternative< T >( value_ );
    }

    template< typename T >
    [[ nodiscard ]] T const & get() const noexcept
    {
        ASSERT( is< T >() );
        return std::get< T >( value_ );
    }

    [[ nodiscard ]] ValueType              const & value   () const noexcept { return value_;    }
    [[ nodiscard ]] std::vector< Pointer > const & children() const noexcept { return children_; }

    [[ nodiscard ]] ErrorInfo errorInfo() const noexcept { return errorInfo_; }

private:
    ValueType              value_;
    std::vector< Pointer > children_;

    ErrorInfo errorInfo_;
};

} // namespace A1::AST