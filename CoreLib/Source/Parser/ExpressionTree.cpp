/**
 *
 * Copyright (c)2022 The Adamnite C++ Authors.
 *
 * This code is open-sourced under the MIT license.
 */

#include <CoreLib/Parser/ExpressionTree.hpp>
#include <CoreLib/Parser/ExpressionTreeNode.hpp>

#include "Utils/Macros.hpp"
#include "Operator.hpp"

#include <stdexcept>
#include <cstdint>

namespace A1
{

namespace
{
    [[ nodiscard ]] bool isEndOfExpression( Token const & token ) noexcept
    {
        if
        (
            std::holds_alternative< Eof     >( token.value() ) ||
            std::holds_alternative< Newline >( token.value() )
        )
        {
            return true;
        }

        if
        (
            auto const & value{ token.value() };
            std::holds_alternative< ReservedToken >( value )
        )
        {
            if
            (
                auto const reservedToken{ std::get< ReservedToken >( value ) };
                reservedToken == ReservedToken::OpCallClose  ||
                reservedToken == ReservedToken::OpIndexClose ||
                reservedToken == ReservedToken::OpColon      ||
                reservedToken == ReservedToken::OpComma
            )
            {
                return true;
            }
        }

        return false;
    }

    struct OperatorInfo
    {
        OperatorType type;
        std::size_t  lineNumber;
        std::size_t  charIndex;
        std::size_t  operandsCount;
    };

    [[ nodiscard ]] OperatorInfo getOperatorInfo
    (
        ReservedToken const token,
        std::size_t   const lineNumber,
        std::size_t   const charIndex,
        bool          const isPrefix
    )
    {

#define MAP_TOKEN_TO_OPERATOR( token, operator )                        \
    case ReservedToken::token:                                          \
        return                                                          \
        {                                                               \
            .type          = OperatorType::operator,                    \
            .lineNumber    = lineNumber,                                \
            .charIndex     = charIndex,                                 \
            .operandsCount = getOperandsCount( OperatorType::operator ) \
        }

#define IGNORE_TOKEN( token ) case ReservedToken::token: break;

        switch ( token )
        {
            MAP_TOKEN_TO_OPERATOR( OpCallOpen , Call  );
            MAP_TOKEN_TO_OPERATOR( OpIndexOpen, Index );

            // Arithmetic operators
            MAP_TOKEN_TO_OPERATOR( OpDiv     , Division       );
            MAP_TOKEN_TO_OPERATOR( OpExp     , Exponent       );
            MAP_TOKEN_TO_OPERATOR( OpFloorDiv, FloorDivision  );
            MAP_TOKEN_TO_OPERATOR( OpMod     , Modulus        );
            MAP_TOKEN_TO_OPERATOR( OpMul     , Multiplication );

            /**
             * Reserved tokens '+' and '-' can be used for two operations.
             * Thus, we need to make a check if reserved token is a prefix
             * or a suffix to the operand.
             */
            case ReservedToken::OpAdd:
                if ( isPrefix )
                {
                    return
                    {
                        .type          = OperatorType::UnaryPlus,
                        .lineNumber    = lineNumber,
                        .charIndex     = charIndex,
                        .operandsCount = getOperandsCount( OperatorType::UnaryPlus )
                    };
                }

                return
                {
                    .type          = OperatorType::Addition,
                    .lineNumber    = lineNumber,
                    .charIndex     = charIndex,
                    .operandsCount = getOperandsCount( OperatorType::Addition )
                };
            case ReservedToken::OpSub:
                if ( isPrefix )
                {
                    return
                    {
                        .type          = OperatorType::UnaryMinus,
                        .lineNumber    = lineNumber,
                        .charIndex     = charIndex,
                        .operandsCount = getOperandsCount( OperatorType::UnaryMinus )
                    };
                }

                return
                {
                    .type          = OperatorType::Subtraction,
                    .lineNumber    = lineNumber,
                    .charIndex     = charIndex,
                    .operandsCount = getOperandsCount( OperatorType::Subtraction )
                };

            // Bitwise operators
            MAP_TOKEN_TO_OPERATOR( OpBitwiseAnd       , BitwiseAnd        );
            MAP_TOKEN_TO_OPERATOR( OpBitwiseLeftShift , BitwiseLeftShift  );
            MAP_TOKEN_TO_OPERATOR( OpBitwiseNot       , BitwiseNot        );
            MAP_TOKEN_TO_OPERATOR( OpBitwiseOr        , BitwiseOr         );
            MAP_TOKEN_TO_OPERATOR( OpBitwiseRightShift, BitwiseRightShift );
            MAP_TOKEN_TO_OPERATOR( OpBitwiseXor       , BitwiseXor        );

            // Logical operators
            MAP_TOKEN_TO_OPERATOR( OpLogicalAnd, LogicalAnd );
            MAP_TOKEN_TO_OPERATOR( OpLogicalNot, LogicalNot );
            MAP_TOKEN_TO_OPERATOR( OpLogicalOr , LogicalOr  );

            // Assignment operators
            MAP_TOKEN_TO_OPERATOR( OpAssign        , Assign               );
            MAP_TOKEN_TO_OPERATOR( OpAssignAdd     , AssignAddition       );
            MAP_TOKEN_TO_OPERATOR( OpAssignDiv     , AssignDivision       );
            MAP_TOKEN_TO_OPERATOR( OpAssignExp     , AssignExponent       );
            MAP_TOKEN_TO_OPERATOR( OpAssignFloorDiv, AssignFloorDivision  );
            MAP_TOKEN_TO_OPERATOR( OpAssignMod     , AssignModulus        );
            MAP_TOKEN_TO_OPERATOR( OpAssignMul     , AssignMultiplication );
            MAP_TOKEN_TO_OPERATOR( OpAssignSub     , AssignSubtraction    );

            // Bitwise assignment operators
            MAP_TOKEN_TO_OPERATOR( OpAssignBitwiseAnd       , AssignBitwiseAnd        );
            MAP_TOKEN_TO_OPERATOR( OpAssignBitwiseLeftShift , AssignBitwiseLeftShift  );
            MAP_TOKEN_TO_OPERATOR( OpAssignBitwiseOr        , AssignBitwiseOr         );
            MAP_TOKEN_TO_OPERATOR( OpAssignBitwiseRightShift, AssignBitwiseRightShift );
            MAP_TOKEN_TO_OPERATOR( OpAssignBitwiseXor       , AssignBitwiseXor        );

            // Comparison operators
            MAP_TOKEN_TO_OPERATOR( OpEqual           , Equality         );
            MAP_TOKEN_TO_OPERATOR( OpGreaterThan     , GreaterThan      );
            MAP_TOKEN_TO_OPERATOR( OpGreaterThanEqual, GreaterThanEqual );
            MAP_TOKEN_TO_OPERATOR( OpLessThan        , LessThan         );
            MAP_TOKEN_TO_OPERATOR( OpLessThanEqual   , LessThanEqual    );
            MAP_TOKEN_TO_OPERATOR( OpNotEqual        , Inequality       );

            // Statements
            MAP_TOKEN_TO_OPERATOR( KwIf    , StatementIf     );
            MAP_TOKEN_TO_OPERATOR( KwElif  , StatementIf     );
            MAP_TOKEN_TO_OPERATOR( KwWhile , StatementWhile  );
            MAP_TOKEN_TO_OPERATOR( KwPass  , StatementPass   );
            MAP_TOKEN_TO_OPERATOR( KwReturn, StatementReturn );

            // Smart contract specific
            MAP_TOKEN_TO_OPERATOR( KwContract, ClassDefinition );

            // Ignored tokens
            IGNORE_TOKEN( OpCallClose  );
            IGNORE_TOKEN( OpIndexClose );
            IGNORE_TOKEN( OpColon      );
            IGNORE_TOKEN( OpComma      );
            IGNORE_TOKEN( KwElse       );

            default:
                throw std::runtime_error( toStringView( token ).data() );
        }

        return {};

#undef IGNORE_TOKEN

#undef MAP_TOKEN_TO_OPERATOR

    }

    Node::Pointer parseOperand( TokenIterator const & tokenIt )
    {
        auto const & token{ tokenIt->value() };

        if ( std::holds_alternative< Number >( token ) )
        {
            return std::make_unique< Node >( std::get< Number >( token ), tokenIt->lineNumber(), tokenIt->charIndex() );
        }
        else if ( std::holds_alternative< String >( token ) )
        {
            return std::make_unique< Node >( std::get< String >( token ), tokenIt->lineNumber(), tokenIt->charIndex() );
        }
        else if ( std::holds_alternative< Identifier >( token ) )
        {
            return std::make_unique< Node >( std::get< Identifier >( token ), tokenIt->lineNumber(), tokenIt->charIndex() );
        }

        throw std::runtime_error( "Syntax error - unexpected operand" );
    }

    bool hasHigherPrecedence( OperatorInfo const & lhs, OperatorInfo const & rhs ) noexcept
    {
        return getOperatorAssociativity( lhs.type ) == OperatorAssociativity::LeftToRight
            ? getOperatorPrecedence( lhs.type ) <= getOperatorPrecedence( rhs.type )
            : getOperatorPrecedence( lhs.type ) >  getOperatorPrecedence( rhs.type );
    }

    void popOneOperator
    (
        std::stack< Node::Pointer >       & operands,
        std::stack< OperatorInfo  >       & operators,
        std::size_t                 const   lineNumber,
        std::size_t                 const   charIndex
    )
    {
        auto const & lastOperator{ operators.top() };

        if ( operands.size() < lastOperator.operandsCount )
        {
            ( void ) lineNumber;
            ( void ) charIndex;
            throw std::runtime_error( "Compile error" );
        }

        std::vector< Node::Pointer > lastOperatorOperands;
        lastOperatorOperands.resize( lastOperator.operandsCount );

        for ( auto i{ lastOperator.operandsCount }; i > 0U; --i )
        {
            lastOperatorOperands[ i - 1 ] = std::move( operands.top() );
            operands.pop();
        }

        operands.push
        (
            std::make_unique< Node >
            (
                lastOperator.type,
                std::move( lastOperatorOperands ),
                lastOperator.lineNumber,
                lastOperator.charIndex
            )
        );

        operators.pop();
    }

    void skipReservedToken( TokenIterator & tokenIt, ReservedToken const tokenToSkip )
    {
        if
        (
            auto const & token{ tokenIt->value() };
            std::holds_alternative< ReservedToken >( token ) &&
            std::get              < ReservedToken >( token ) == tokenToSkip
        )
        {
            ++tokenIt;
        }
        else
        {
            throw std::runtime_error( std::string{ "Syntax error - missing '" } + toStringView( tokenToSkip ).data() + "'" );
        }
    }

    void skipNewline( TokenIterator & tokenIt )
    {
        if ( std::holds_alternative< Newline >( tokenIt->value() ) )
        {
            ++tokenIt;
        }
        else
        {
            throw std::runtime_error( "Syntax error - missing newline" );
        }
    }
} // namespace

Node::Pointer parse( TokenIterator & tokenIt )
{
    std::stack< Node::Pointer > operands;
    std::stack< OperatorInfo  > operators;

    auto expectingOperand{ true };

    for ( ; !isEndOfExpression( *tokenIt ); ++tokenIt )
    {
        if ( std::holds_alternative< ReservedToken >( tokenIt->value() ) )
        {
            auto const isTokenPrefix{ expectingOperand };
            auto       operatorInfo
            {
                getOperatorInfo
                (
                    std::get< ReservedToken >( tokenIt->value() ),
                    tokenIt->lineNumber(),
                    tokenIt->charIndex (),
                    isTokenPrefix
                )
            };

            if ( operatorInfo.type == OperatorType::Unknown ) { continue; }

            if ( !operators.empty() && hasHigherPrecedence( operators.top(), operatorInfo ) )
            {
                popOneOperator( operands, operators, tokenIt->lineNumber(), tokenIt->charIndex() );
            }

            if ( operatorInfo.type == OperatorType::Call )
            {
                skipReservedToken( tokenIt, ReservedToken::OpCallOpen );

                if
                (
                    auto const & token{ tokenIt->value() };
                    !std::holds_alternative< ReservedToken >( token ) ||
                     std::get              < ReservedToken >( token ) != ReservedToken::OpCallClose
                )
                {
                    while ( true )
                    {
                        operands.push( parse( tokenIt ) );

                        ++operatorInfo.operandsCount;

                        if
                        (
                            auto const & current{ tokenIt->value() };
                            std::holds_alternative< ReservedToken >( current )
                        )
                        {
                            if ( std::get< ReservedToken >( current ) == ReservedToken::OpCallClose )
                            {
                                break;
                            }
                            else if ( std::get< ReservedToken >( current ) == ReservedToken::OpComma )
                            {
                                ++tokenIt;
                            }
                            else
                            {
                                throw std::runtime_error( "Syntax error - expecting ',' or ')'" );
                            }
                        }
                    }
                }
            }
            else if ( operatorInfo.type == OperatorType::Index )
            {
                skipReservedToken( tokenIt, ReservedToken::OpIndexOpen );
                operands.push( parse( tokenIt ) );

                if
                (
                    auto const & token{ tokenIt->value() };
                    !std::holds_alternative< ReservedToken >( token ) ||
                     std::get              < ReservedToken >( token ) != ReservedToken::OpIndexClose
                )
                {
                    throw std::runtime_error( "Syntax error - missing closing ']'" );
                }
            }
            else if ( operatorInfo.type == OperatorType::StatementIf )
            {
                ++tokenIt; // skip 'if' / 'elif' keyword

                operands.push( parse( tokenIt ) ); // parse condition

                skipReservedToken( tokenIt, ReservedToken::OpColon );
                skipNewline( tokenIt );

                operands.push( parse( tokenIt ) ); // parse body

                skipNewline( tokenIt );

                while ( true )
                {
                    if
                    (
                        auto const & token{ tokenIt->value() };
                        std::holds_alternative< ReservedToken >( token ) &&
                        std::get              < ReservedToken >( token ) == ReservedToken::KwElif
                    )
                    {
                        ++operatorInfo.operandsCount;
                        operands.push( parse( tokenIt ) ); // parse branch
                    }
                    else
                    {
                        break;
                    }
                }

                if
                (
                    auto const & token{ tokenIt->value() };
                    std::holds_alternative< ReservedToken >( token ) &&
                    std::get              < ReservedToken >( token ) == ReservedToken::KwElse
                )
                {
                    skipReservedToken( tokenIt, ReservedToken::KwElse  );
                    skipReservedToken( tokenIt, ReservedToken::OpColon );
                    skipNewline( tokenIt );

                    ++operatorInfo.operandsCount;

                    operands.push( parse( tokenIt ) ); // parse body
                }
            }
            else if ( operatorInfo.type == OperatorType::StatementWhile )
            {
                skipReservedToken( tokenIt, ReservedToken::KwWhile );

                operands.push( parse( tokenIt ) ); // parse condition

                skipReservedToken( tokenIt, ReservedToken::OpColon );
                skipNewline( tokenIt );

                operands.push( parse( tokenIt ) ); // parse while body
            }
            else if ( operatorInfo.type == OperatorType::ClassDefinition )
            {
                skipReservedToken( tokenIt, ReservedToken::KwContract );

                operands.push( parse( tokenIt ) ); // parse contract name

                skipReservedToken( tokenIt, ReservedToken::OpColon );
                skipNewline( tokenIt );

                operands.push( parse( tokenIt ) ); // parse contract definition
            }

            operators.push( operatorInfo );

            expectingOperand = operatorInfo.operandsCount != 0;
        }
        else
        {
            if ( !expectingOperand )
            {
                throw std::runtime_error( "Syntax error" );
            }

            operands.push( parseOperand( tokenIt ) );
            expectingOperand = false;
        }
    }

    if ( expectingOperand && operands.empty() )
    {
        throw std::runtime_error( "Syntax error - expecting an operand" );
    }

    while ( !operators.empty() )
    {
        popOneOperator( operands, operators, tokenIt->lineNumber(), tokenIt->charIndex());
    }

    return std::move( operands.top() );
}

} // namespace A1