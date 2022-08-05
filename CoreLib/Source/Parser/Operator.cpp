/**
 *
 * Copyright (c)2022 The Adamnite C++ Authors.
 *
 * This code is open-sourced under the MIT license.
 */

#include "Operator.hpp"

#include "Utils/Macros.hpp"

namespace A1
{

OperatorPrecedence getOperatorPrecedence( OperatorType const type ) noexcept
{
    switch ( type )
    {
        case OperatorType::Call:
            return OperatorPrecedence::Group1;

        case OperatorType::Exponent:
            return OperatorPrecedence::Group2;

        case OperatorType::UnaryPlus:
        case OperatorType::UnaryMinus:
        case OperatorType::BitwiseNot:
            return OperatorPrecedence::Group3;

        case OperatorType::Multiplication:
        case OperatorType::Division:
        case OperatorType::FloorDivision:
        case OperatorType::Modulus:
            return OperatorPrecedence::Group4;

        case OperatorType::Addition:
        case OperatorType::Subtraction:
            return OperatorPrecedence::Group5;

        case OperatorType::BitwiseLeftShift:
        case OperatorType::BitwiseRightShift:
            return OperatorPrecedence::Group6;

        case OperatorType::BitwiseAnd:
            return OperatorPrecedence::Group7;

        case OperatorType::BitwiseXor:
            return OperatorPrecedence::Group8;

        case OperatorType::BitwiseOr:
            return OperatorPrecedence::Group9;

        case OperatorType::Equality:
        case OperatorType::Inequality:
        case OperatorType::GreaterThan:
        case OperatorType::GreaterThanEqual:
        case OperatorType::LessThan:
        case OperatorType::LessThanEqual:
        case OperatorType::IsIdentical:
        case OperatorType::IsNotIdentical:
        case OperatorType::IsMemberOf:
        case OperatorType::IsNotMemberOf:
            return OperatorPrecedence::Group10;

        case OperatorType::LogicalNot:
            return OperatorPrecedence::Group11;

        case OperatorType::LogicalAnd:
            return OperatorPrecedence::Group12;

        case OperatorType::LogicalOr:
            return OperatorPrecedence::Group13;

        case OperatorType::Unknown:
        case OperatorType::Count:
            break;
    }

    ASSERT( false );
    return OperatorPrecedence::Count;
}

OperatorAssociativity getOperatorAssociativity( OperatorPrecedence const precedence ) noexcept
{
    return precedence == OperatorPrecedence::Group2
        ? OperatorAssociativity::RightToLeft
        : OperatorAssociativity::LeftToRight;
}

std::size_t getNumberOfOperands( OperatorType const type ) noexcept
{
    switch ( type )
    {
        case OperatorType::Call:
        case OperatorType::UnaryPlus:
        case OperatorType::UnaryMinus:
        case OperatorType::BitwiseNot:
        case OperatorType::LogicalNot:
            return 1U;

        case OperatorType::Exponent:
        case OperatorType::Multiplication:
        case OperatorType::Division:
        case OperatorType::FloorDivision:
        case OperatorType::Modulus:
        case OperatorType::Addition:
        case OperatorType::Subtraction:
        case OperatorType::BitwiseLeftShift:
        case OperatorType::BitwiseRightShift:
        case OperatorType::BitwiseAnd:
        case OperatorType::BitwiseOr:
        case OperatorType::BitwiseXor:
        case OperatorType::Equality:
        case OperatorType::Inequality:
        case OperatorType::GreaterThan:
        case OperatorType::GreaterThanEqual:
        case OperatorType::LessThan:
        case OperatorType::LessThanEqual:
        case OperatorType::IsIdentical:
        case OperatorType::IsNotIdentical:
        case OperatorType::IsMemberOf:
        case OperatorType::IsNotMemberOf:
        case OperatorType::LogicalAnd:
        case OperatorType::LogicalOr:
            return 2U;

        case OperatorType::Unknown:
        case OperatorType::Count:
            break;
    }

    ASSERT( false );
    return 0U;
}

} // namespace A1