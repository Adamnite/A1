/**
 *
 * Copyright (c)2022 The Adamnite C++ Authors.
 *
 * This code is open-sourced under the MIT license.
 */

#pragma once

#include <CoreLib/Parser/ExpressionTreeNode.hpp>

#include <cstdint>

namespace A1
{

/**
 * Node precedence guides the order in which operations in
 * an expression are carried out.
 *
 * Following list is in descending order, i.e. upper group has
 * higher precedence than the lower one.
 */
enum class NodePrecedence : std::uint8_t
{
    /**
     * Following groups are reserved for operators
     */
    Group1,
    Group2,
    Group3,
    Group4,
    Group5,
    Group6,
    Group7,
    Group8,
    Group9,
    Group10,
    Group11,
    Group12,
    Group13,
    Group14,
    Group15,

    /**
     * Following group is reserved for all other node types
     */
    Group16,

    /**
     * Number of possible precedences
     */
    Count
};

/**
 * Node associativity is the order in which an expression
 * that has multiple operators of the same precedence is evaluated.
 */
enum class NodeAssociativity : std::uint8_t
{
    LeftToRight,
    RightToLeft,

    /**
     * Number of possible associativities
     */
    Count
};

[[ nodiscard ]] NodePrecedence    getPrecedence   ( NodeType const type ) noexcept;
[[ nodiscard ]] NodeAssociativity getAssociativity( NodeType const type ) noexcept;

[[ nodiscard ]] std::size_t getOperandsCount( NodeType const type ) noexcept;

} // namespace A1