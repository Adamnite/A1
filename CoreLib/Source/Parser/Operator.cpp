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

        /**
         * All other types should fall into this precedence group
         */
        case OperatorType::StatementIf:
        case OperatorType::StatementWhile:
        case OperatorType::StatementPass:
        case OperatorType::StatementReturn:
        case OperatorType::ContractDefinition:
        case OperatorType::FunctionDefinition:
        case OperatorType::FunctionParameterDefinition:
        case OperatorType::VariableDefinition:
            return OperatorPrecedence::Group16;

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
        case OperatorType::StatementPass:
            return 0U;

        /**
         * The number of arguments in a function call is variable. Thus,
         * it is detected by the parser itself. The only operand that's
         * certain to exist is the function name identifier.
         */
        case OperatorType::Call:
            return 1U;

        /**
         * The number of statements in a smart contract body is variable. Thus,
         * it is detected by the parser itself. The only operand that's
         * certain to exist is the smart contract name identifier.
         */
        case OperatorType::ContractDefinition:
            return 1U;

        /**
         * Variable definition consists of a name identifier and type identifier.
         */
        case OperatorType::VariableDefinition:
            return 2U;

        /**
         * The number of parameters and statements in a function body is variable.
         * Thus, those are detected by the parser itself. Return type is required
         * only if function returns a value. Therefore, the only operand that's
         * certain to exist is the function name identifier.
         */
        case OperatorType::FunctionDefinition:
            return 1U;

        /**
         * Function parameter definition consists of a name identifier and type identifier.
         */
        case OperatorType::FunctionParameterDefinition:
            return 2U;

        /**
         * Since the number of operands for if statement is variable,
         * detecting it is done by the parser itself.
         * It is certain that at least two operands should exist -
         * if condition and if body.
         */
        case OperatorType::StatementIf:
        case OperatorType::StatementWhile:
            return 2U;

        case OperatorType::StatementReturn:
            return 1U;

        case OperatorType::UnaryPlus:
        case OperatorType::UnaryMinus:
        case OperatorType::BitwiseNot:
        case OperatorType::LogicalNot:
            return 1U;

        case OperatorType::Index:
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