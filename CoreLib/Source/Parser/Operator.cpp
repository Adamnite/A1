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

        case OperatorType::Index:
            return OperatorPrecedence::Group2;

        case OperatorType::Exponent:
            return OperatorPrecedence::Group3;

        case OperatorType::UnaryPlus:
        case OperatorType::UnaryMinus:
        case OperatorType::BitwiseNot:
            return OperatorPrecedence::Group4;

        case OperatorType::Multiplication:
        case OperatorType::Division:
        case OperatorType::FloorDivision:
        case OperatorType::Modulus:
            return OperatorPrecedence::Group5;

        case OperatorType::Addition:
        case OperatorType::Subtraction:
            return OperatorPrecedence::Group6;

        case OperatorType::BitwiseLeftShift:
        case OperatorType::BitwiseRightShift:
            return OperatorPrecedence::Group7;

        case OperatorType::BitwiseAnd:
            return OperatorPrecedence::Group8;

        case OperatorType::BitwiseXor:
            return OperatorPrecedence::Group9;

        case OperatorType::BitwiseOr:
            return OperatorPrecedence::Group10;

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
            return OperatorPrecedence::Group11;

        case OperatorType::LogicalNot:
            return OperatorPrecedence::Group12;

        case OperatorType::LogicalAnd:
            return OperatorPrecedence::Group13;

        case OperatorType::LogicalOr:
            return OperatorPrecedence::Group14;

        case OperatorType::Assign:
        case OperatorType::AssignExponent:
        case OperatorType::AssignAddition:
        case OperatorType::AssignSubtraction:
        case OperatorType::AssignMultiplication:
        case OperatorType::AssignDivision:
        case OperatorType::AssignFloorDivision:
        case OperatorType::AssignModulus:
        case OperatorType::AssignBitwiseLeftShift:
        case OperatorType::AssignBitwiseRightShift:
        case OperatorType::AssignBitwiseAnd:
        case OperatorType::AssignBitwiseOr:
        case OperatorType::AssignBitwiseXor:
            return OperatorPrecedence::Group15;

        case OperatorType::Unknown:
        case OperatorType::Count:
            break;
    }

    ASSERT( false );
    return OperatorPrecedence::Count;
}

OperatorAssociativity getOperatorAssociativity( OperatorType const type ) noexcept
{
    return type == OperatorType::Exponent
        ? OperatorAssociativity::RightToLeft
        : OperatorAssociativity::LeftToRight;
}

std::size_t getOperandsCount( OperatorType const type ) noexcept
{
    switch ( type )
    {
        /**
         * Since the number of arguments in a function call is variable,
         * detecting it is done by the parser itself.
         */
        case OperatorType::Call:
            return 0U;

        case OperatorType::Index:
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
        case OperatorType::Assign:
        case OperatorType::AssignExponent:
        case OperatorType::AssignAddition:
        case OperatorType::AssignSubtraction:
        case OperatorType::AssignMultiplication:
        case OperatorType::AssignDivision:
        case OperatorType::AssignFloorDivision:
        case OperatorType::AssignModulus:
        case OperatorType::AssignBitwiseLeftShift:
        case OperatorType::AssignBitwiseRightShift:
        case OperatorType::AssignBitwiseAnd:
        case OperatorType::AssignBitwiseOr:
        case OperatorType::AssignBitwiseXor:
            return 2U;

        case OperatorType::Unknown:
        case OperatorType::Count:
            break;
    }

    ASSERT( false );
    return 0U;
}

} // namespace A1