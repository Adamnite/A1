/**
 *
 * Copyright (c)2022 The Adamnite C++ Authors.
 *
 * This code is open-sourced under the MIT license.
 */

#include "ASTHelper.hpp"

#include <CoreLib/Utils/Macros.hpp>

namespace A1
{

NodePrecedence getPrecedence( NodeType const type ) noexcept
{
    switch ( type )
    {
        case NodeType::Call:
            return NodePrecedence::Group1;

        case NodeType::Index:
            return NodePrecedence::Group2;

        case NodeType::Exponent:
            return NodePrecedence::Group3;

        case NodeType::UnaryPlus:
        case NodeType::UnaryMinus:
        case NodeType::BitwiseNot:
            return NodePrecedence::Group4;

        case NodeType::Multiplication:
        case NodeType::Division:
        case NodeType::FloorDivision:
        case NodeType::Modulus:
            return NodePrecedence::Group5;

        case NodeType::Addition:
        case NodeType::Subtraction:
            return NodePrecedence::Group6;

        case NodeType::BitwiseLeftShift:
        case NodeType::BitwiseRightShift:
            return NodePrecedence::Group7;

        case NodeType::BitwiseAnd:
            return NodePrecedence::Group8;

        case NodeType::BitwiseXor:
            return NodePrecedence::Group9;

        case NodeType::BitwiseOr:
            return NodePrecedence::Group10;

        case NodeType::Equality:
        case NodeType::Inequality:
        case NodeType::GreaterThan:
        case NodeType::GreaterThanEqual:
        case NodeType::LessThan:
        case NodeType::LessThanEqual:
        case NodeType::IsIdentical:
        case NodeType::IsNotIdentical:
        case NodeType::IsMemberOf:
        case NodeType::IsNotMemberOf:
            return NodePrecedence::Group11;

        case NodeType::LogicalNot:
            return NodePrecedence::Group12;

        case NodeType::LogicalAnd:
            return NodePrecedence::Group13;

        case NodeType::LogicalOr:
            return NodePrecedence::Group14;

        case NodeType::Assign:
        case NodeType::AssignExponent:
        case NodeType::AssignAddition:
        case NodeType::AssignSubtraction:
        case NodeType::AssignMultiplication:
        case NodeType::AssignDivision:
        case NodeType::AssignFloorDivision:
        case NodeType::AssignModulus:
        case NodeType::AssignBitwiseLeftShift:
        case NodeType::AssignBitwiseRightShift:
        case NodeType::AssignBitwiseAnd:
        case NodeType::AssignBitwiseOr:
        case NodeType::AssignBitwiseXor:
            return NodePrecedence::Group15;

        /**
         * All other types should fall into this precedence group
         */
        case NodeType::MemberCall:
        case NodeType::StatementIf:
        case NodeType::StatementElif:
        case NodeType::StatementElse:
        case NodeType::StatementWhile:
        case NodeType::StatementPass:
        case NodeType::StatementReturn:
        case NodeType::ModuleDefinition:
        case NodeType::ContractDefinition:
        case NodeType::FunctionDefinition:
        case NodeType::FunctionParameterDefinition:
        case NodeType::VariableDefinition:
            return NodePrecedence::Group16;

        case NodeType::Unknown:
        case NodeType::Count:
            break;
    }

    ASSERT( false );
    return NodePrecedence::Count;
}

NodeAssociativity getAssociativity( NodeType const type ) noexcept
{
    return type == NodeType::Exponent
        ? NodeAssociativity::RightToLeft
        : NodeAssociativity::LeftToRight;
}

std::size_t getOperandsCount( NodeType const type ) noexcept
{
    switch ( type )
    {
        case NodeType::StatementPass:
            return 0U;

        case NodeType::UnaryPlus:
        case NodeType::UnaryMinus:
        case NodeType::BitwiseNot:
        case NodeType::LogicalNot:
        case NodeType::StatementReturn:
            return 1U;

        case NodeType::Index:
        case NodeType::Exponent:
        case NodeType::Multiplication:
        case NodeType::Division:
        case NodeType::FloorDivision:
        case NodeType::Modulus:
        case NodeType::Addition:
        case NodeType::Subtraction:
        case NodeType::BitwiseLeftShift:
        case NodeType::BitwiseRightShift:
        case NodeType::BitwiseAnd:
        case NodeType::BitwiseOr:
        case NodeType::BitwiseXor:
        case NodeType::Equality:
        case NodeType::Inequality:
        case NodeType::GreaterThan:
        case NodeType::GreaterThanEqual:
        case NodeType::LessThan:
        case NodeType::LessThanEqual:
        case NodeType::IsIdentical:
        case NodeType::IsNotIdentical:
        case NodeType::IsMemberOf:
        case NodeType::IsNotMemberOf:
        case NodeType::LogicalAnd:
        case NodeType::LogicalOr:
        case NodeType::Assign:
        case NodeType::AssignExponent:
        case NodeType::AssignAddition:
        case NodeType::AssignSubtraction:
        case NodeType::AssignMultiplication:
        case NodeType::AssignDivision:
        case NodeType::AssignFloorDivision:
        case NodeType::AssignModulus:
        case NodeType::AssignBitwiseLeftShift:
        case NodeType::AssignBitwiseRightShift:
        case NodeType::AssignBitwiseAnd:
        case NodeType::AssignBitwiseOr:
        case NodeType::AssignBitwiseXor:
        case NodeType::MemberCall:
        case NodeType::FunctionParameterDefinition:
            return 2U;

        /**
         * Since the number of statements in a body of the following
         * constructs is variable, it is detected by the parser itself.
         */
        case NodeType::StatementElse:
        case NodeType::ModuleDefinition:
            return 0U;

        /**
         * Since the number of operands of the following constructs is
         * variable, it is detected by the parser itself. Only one operand is
         * required to exist, e.g. branch condition and function, smart contract
         * or variable name identifier.
         */
        case NodeType::Call:
        case NodeType::StatementIf:
        case NodeType::StatementElif:
        case NodeType::StatementWhile:
        case NodeType::ContractDefinition:
        case NodeType::FunctionDefinition:
        case NodeType::VariableDefinition:
            return 1U;

        case NodeType::Unknown:
        case NodeType::Count:
            break;
    }

    ASSERT( false );
    return 0U;
}

bool hasHigherPrecedence( NodeType const lhs, NodeType const rhs ) noexcept
{
    return getAssociativity( lhs ) == NodeAssociativity::LeftToRight
        ? getPrecedence( lhs ) <= getPrecedence( rhs )
        : getPrecedence( lhs ) >  getPrecedence( rhs );
}

} // namespace A1