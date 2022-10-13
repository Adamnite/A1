/**
 *
 * Copyright (c)2022 The Adamnite C++ Authors.
 *
 * This code is open-sourced under the MIT license.
 */

#include <CoreLib/Parser/ExpressionTree.hpp>
#include <CoreLib/Parser/ExpressionTreeNode.hpp>
#include <CoreLib/Utils/Macros.hpp>

#include "ExpressionTreeHelper.hpp"

#include <fmt/format.h>

#include <stdexcept>
#include <cstdint>

namespace A1
{

namespace
{
    void fill( std::stack< Node::Pointer > & dst, std::vector< Node::Pointer > src )
    {
        for ( std::size_t i{ 0U }; i < std::size( src ); i++ )
        {
            dst.push( std::move( src[ i ] ) );
        }
    }

    [[ nodiscard ]] bool isEndOfExpression( Token const & token ) noexcept
    {
        if ( token.is< Eof >() || token.is< Indentation >() || token.is< Newline >() )
        {
            return true;
        }

        if ( token.is< ReservedToken >() )
        {
            if
            (
                auto const reservedToken{ token.get< ReservedToken >() };
                reservedToken == ReservedToken::OpParenthesisClose ||
                reservedToken == ReservedToken::OpSubscriptClose   ||
                reservedToken == ReservedToken::OpColon            ||
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
        NodeType     type;
        ErrorInfo    errorInfo;
        std::size_t  operandsCount;
    };

    [[ nodiscard ]] OperatorInfo getOperatorInfo
    (
        ReservedToken const token,
        ErrorInfo           errorInfo,
        bool          const isPrefix
    )
    {

#define MAP_TOKEN_TO_OPERATOR( token, operator )                    \
    case ReservedToken::token:                                      \
        return                                                      \
        {                                                           \
            .type          = NodeType::operator,                    \
            .errorInfo     = std::move( errorInfo ),                \
            .operandsCount = getOperandsCount( NodeType::operator ) \
        }

#define IGNORE_TOKEN( token ) case ReservedToken::token: break;

        switch ( token )
        {
            MAP_TOKEN_TO_OPERATOR( OpParenthesisOpen, Call       );
            MAP_TOKEN_TO_OPERATOR( OpSubscriptOpen  , Index      );
            MAP_TOKEN_TO_OPERATOR( OpDot            , MemberCall );

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
                        .type          = NodeType::UnaryPlus,
                        .errorInfo     = std::move( errorInfo ),
                        .operandsCount = getOperandsCount( NodeType::UnaryPlus )
                    };
                }

                return
                {
                    .type          = NodeType::Addition,
                    .errorInfo     = std::move( errorInfo ),
                    .operandsCount = getOperandsCount( NodeType::Addition )
                };
            case ReservedToken::OpSub:
                if ( isPrefix )
                {
                    return
                    {
                        .type          = NodeType::UnaryMinus,
                        .errorInfo     = std::move( errorInfo ),
                        .operandsCount = getOperandsCount( NodeType::UnaryMinus )
                    };
                }

                return
                {
                    .type          = NodeType::Subtraction,
                    .errorInfo     = std::move( errorInfo ),
                    .operandsCount = getOperandsCount( NodeType::Subtraction )
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
            MAP_TOKEN_TO_OPERATOR( KwElif  , StatementElif   );
            MAP_TOKEN_TO_OPERATOR( KwElse  , StatementElse   );
            MAP_TOKEN_TO_OPERATOR( KwWhile , StatementWhile  );
            MAP_TOKEN_TO_OPERATOR( KwPass  , StatementPass   );
            MAP_TOKEN_TO_OPERATOR( KwReturn, StatementReturn );

            // Definitions
            MAP_TOKEN_TO_OPERATOR( KwDef     , FunctionDefinition );
            MAP_TOKEN_TO_OPERATOR( KwLet     , VariableDefinition );
            MAP_TOKEN_TO_OPERATOR( KwContract, ContractDefinition );

            // Ignored tokens
            IGNORE_TOKEN( Unknown            );
            IGNORE_TOKEN( OpParenthesisClose );
            IGNORE_TOKEN( OpSubscriptClose   );
            IGNORE_TOKEN( OpColon            );
            IGNORE_TOKEN( OpComma            );
            IGNORE_TOKEN( KwNumber           );
            IGNORE_TOKEN( KwString           );

            default:
                throw std::runtime_error( fmt::format( "Unknown token: {}", toStringView( token ) ) );
        }

        return {};

#undef IGNORE_TOKEN

#undef MAP_TOKEN_TO_OPERATOR

    }

    Node::Pointer parseOperand( TokenIterator const & tokenIt )
    {
        if ( tokenIt->is< Number >() )
        {
            return std::make_unique< Node >( tokenIt->get< Number >(), tokenIt->errorInfo() );
        }
        else if ( tokenIt->is< String >() )
        {
            return std::make_unique< Node >( tokenIt->get< String >(), tokenIt->errorInfo() );
        }
        else if ( tokenIt->is< Identifier >() )
        {
            return std::make_unique< Node >( tokenIt->get< Identifier >(), tokenIt->errorInfo() );
        }
        else if ( tokenIt->is< ReservedToken >() )
        {
            auto const reservedToken{ tokenIt->get< ReservedToken >() };
            if ( reservedToken == ReservedToken::KwNumber )
            {
                return std::make_unique< Node >( Registry::getNumberHandle(), tokenIt->errorInfo() );
            }
            else if ( reservedToken == ReservedToken::KwString )
            {
                return std::make_unique< Node >( Registry::getStringLiteralHandle(), tokenIt->errorInfo() );
            }
        }

        throw std::runtime_error( "Syntax error - unexpected operand" );
    }

    void popOneOperator
    (
        std::stack< Node::Pointer > & operands,
        std::stack< OperatorInfo  > & operators,
        ErrorInfo                     errorInfo
    )
    {
        auto const & lastOperator{ operators.top() };

        if ( operands.size() < lastOperator.operandsCount )
        {
            ( void ) errorInfo;
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
                lastOperator.errorInfo
            )
        );

        operators.pop();
    }

    template< ReservedToken tokenToSkip, ReservedToken ... moreTokensToSkip >
    void skipOneOfReservedTokens( TokenIterator & tokenIt )
    {
        if
        (
            tokenIt->is< ReservedToken >() &&
            (
                    tokenIt->get< ReservedToken >() == tokenToSkip        ||
                ( ( tokenIt->get< ReservedToken >() == moreTokensToSkip ) || ... )
            )
        )
        {
            ++tokenIt;
        }
        else
        {
            throw std::runtime_error
            (
                fmt::format
                (
                    "Syntax error - missing {}",
                    (
                        ( std::string{ "'"     } + std::string{ toStringView( tokenToSkip      ) } + std::string{ "'" } ) + ... +
                        ( std::string{ " or '" } + std::string{ toStringView( moreTokensToSkip ) } + std::string{ "'" } )
                    )
                )
            );
        }
    }

    void skipNewline( TokenIterator & tokenIt )
    {
        if ( tokenIt->is< Newline >() )
        {
            ++tokenIt;
        }
        else
        {
            throw std::runtime_error( "Syntax error - missing newline" );
        }
    }

    std::vector< Node::Pointer > parseBody( TokenIterator & tokenIt, std::size_t const indentationLevel )
    {
        std::vector< Node::Pointer > operands;

        while ( tokenIt->is_not< Eof >() )
        {
            // parse body statement
            operands.push_back( parse( tokenIt, indentationLevel ) );

            auto prevTokenIt{ tokenIt };

            auto currentIndentationLevel{ 0U };
            while ( currentIndentationLevel != indentationLevel )
            {
                if ( tokenIt->is< Indentation >() )
                {
                    ++tokenIt;
                    currentIndentationLevel++;
                }
                else if ( tokenIt->is< Newline >() )
                {
                    skipNewline( tokenIt );
                }
                else
                {
                    break;
                }
            }

            if ( currentIndentationLevel < indentationLevel )
            {
                tokenIt = prevTokenIt; // TODO: Implement this a bit better
                break;
            }
        }

        return operands;
    }
} // namespace

Node::Pointer parse
(
    TokenIterator       & tokenIt,
    std::size_t   const   indentationLevelCount,
    bool          const   alreadyInModule
)
{
    std::stack< Node::Pointer > operands;
    std::stack< OperatorInfo  > operators;

    auto expectingOperand{ true };

    std::size_t indentationIdx{ 0U };
    while ( indentationIdx != indentationLevelCount )
    {
        if ( tokenIt->is< Indentation >() )
        {
            ++tokenIt;
            indentationIdx++;
        }
        else
        {
            indentationIdx = indentationLevelCount;
            break;
        }
    }

    if ( indentationLevelCount == 0U && !alreadyInModule )
    {
        operators.push
        (
            OperatorInfo
            {
                .type = NodeType::ModuleDefinition,
                .errorInfo = tokenIt->errorInfo(),
                .operandsCount = 0U
            }
        );
    }

    while ( tokenIt->is< Newline >() || tokenIt->is< Eof >() )
    {
        // skip empty lines or comment lines
        ++tokenIt;
        if ( tokenIt->is< Eof >() )
        {
            expectingOperand = false;
            break;
        }
    }

    for ( ; !isEndOfExpression( *tokenIt ); ++tokenIt )
    {
        if ( tokenIt->is< ReservedToken >() )
        {
            auto const isTokenPrefix{ expectingOperand };
            auto       operatorInfo
            {
                getOperatorInfo
                (
                    tokenIt->get< ReservedToken >(),
                    tokenIt->errorInfo(),
                    isTokenPrefix
                )
            };

            if ( operatorInfo.type == NodeType::Unknown )
            {
                if
                (
                    auto const reservedToken{ tokenIt->get< ReservedToken >() };
                    reservedToken == ReservedToken::KwNumber ||
                    reservedToken == ReservedToken::KwString
                )
                {
                    if ( !expectingOperand )
                    {
                        throw std::runtime_error( "Syntax error" );
                    }

                    operands.push( parseOperand( tokenIt ) );
                    expectingOperand = false;

                    ++tokenIt;
                    break;
                }

                continue;
            }

            if ( !operators.empty() && operators.top().type != NodeType::ModuleDefinition && hasHigherPrecedence( operators.top().type, operatorInfo.type ) )
            {
                popOneOperator( operands, operators, tokenIt->errorInfo() );
            }

            if ( operatorInfo.type == NodeType::Call )
            {
                skipOneOfReservedTokens< ReservedToken::OpParenthesisOpen >( tokenIt );

                if ( tokenIt->is_not< ReservedToken >() || tokenIt->get< ReservedToken >() != ReservedToken::OpParenthesisClose )
                {
                    while ( true )
                    {
                        operands.push( parse( tokenIt ) );

                        ++operatorInfo.operandsCount;

                        if ( tokenIt->is< ReservedToken >() )
                        {
                            auto const reservedToken{ tokenIt->get< ReservedToken >() };
                            if ( reservedToken == ReservedToken::OpParenthesisClose )
                            {
                                break;
                            }
                            else if ( reservedToken == ReservedToken::OpComma )
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
            else if ( operatorInfo.type == NodeType::Index )
            {
                skipOneOfReservedTokens< ReservedToken::OpSubscriptOpen >( tokenIt );
                operands.push( parse( tokenIt ) );
                skipOneOfReservedTokens< ReservedToken::OpSubscriptClose >( tokenIt );
            }
            else if ( operatorInfo.type == NodeType::StatementIf || operatorInfo.type == NodeType::StatementElif )
            {
                skipOneOfReservedTokens< ReservedToken::KwIf, ReservedToken::KwElif >( tokenIt );
                operands.push( parse( tokenIt ) ); // parse condition
                skipOneOfReservedTokens< ReservedToken::OpColon >( tokenIt );
                skipNewline( tokenIt );

                auto bodyOperands{ parseBody( tokenIt, indentationIdx + 1U ) };
                operatorInfo.operandsCount += std::size( bodyOperands );
                fill( operands, std::move( bodyOperands ) );

                auto prevTokenIt{ tokenIt };
                if ( tokenIt->is< Newline >() ) { ++tokenIt; }

                if
                (
                    tokenIt->is< ReservedToken >() &&
                    (
                        tokenIt->get< ReservedToken >() == ReservedToken::KwElif ||
                        tokenIt->get< ReservedToken >() == ReservedToken::KwElse
                    )
                )
                {
                    // Parse elif / else expression
                    operands.push( parse( tokenIt, indentationIdx ) );
                    operatorInfo.operandsCount++;
                }
                else
                {
                    tokenIt = prevTokenIt;
                }
            }
            else if ( operatorInfo.type == NodeType::StatementElse )
            {
                skipOneOfReservedTokens< ReservedToken::KwElse  >( tokenIt );
                skipOneOfReservedTokens< ReservedToken::OpColon >( tokenIt );
                skipNewline( tokenIt );

                auto bodyOperands{ parseBody( tokenIt, indentationIdx + 1U ) };
                operatorInfo.operandsCount += std::size( bodyOperands );
                fill( operands, std::move( bodyOperands ) );
            }
            else if ( operatorInfo.type == NodeType::StatementWhile )
            {
                skipOneOfReservedTokens< ReservedToken::KwWhile >( tokenIt );
                operands.push( parse( tokenIt ) ); // parse condition
                skipOneOfReservedTokens< ReservedToken::OpColon >( tokenIt );
                skipNewline( tokenIt );

                auto bodyOperands{ parseBody( tokenIt, indentationIdx + 1U ) };
                operatorInfo.operandsCount += std::size( bodyOperands );
                fill( operands, std::move( bodyOperands ) );
            }
            else if ( operatorInfo.type == NodeType::FunctionDefinition )
            {
                skipOneOfReservedTokens< ReservedToken::KwDef >( tokenIt );

                operands.push( parseOperand( tokenIt ) ); // parse function name
                tokenIt++;

                {
                    // we are parsing function parameters

                    skipOneOfReservedTokens< ReservedToken::OpParenthesisOpen >( tokenIt );

                    if ( tokenIt->is_not< ReservedToken >() || tokenIt->get< ReservedToken >() != ReservedToken::OpParenthesisClose )
                    {
                        while ( true )
                        {
                            OperatorInfo const parameterDefinition
                            {
                                .type          = NodeType::FunctionParameterDefinition,
                                .errorInfo     = tokenIt->errorInfo(),
                                .operandsCount = getOperandsCount( NodeType::FunctionParameterDefinition )
                            };

                            operands.push( parse( tokenIt ) ); // parse parameter name
                            skipOneOfReservedTokens< ReservedToken::OpColon >( tokenIt );
                            operands.push( parse( tokenIt ) ); // parse parameter type

                            operators.push( parameterDefinition );

                            while ( !operators.empty() && operators.top().type != NodeType::ModuleDefinition )
                            {
                                popOneOperator( operands, operators, tokenIt->errorInfo());
                            }

                            operatorInfo.operandsCount++;

                            if ( tokenIt->is< ReservedToken >() )
                            {
                                auto const reservedToken{ tokenIt->get< ReservedToken >() };
                                if ( reservedToken == ReservedToken::OpParenthesisClose )
                                {
                                    break;
                                }
                                else if ( reservedToken == ReservedToken::OpComma )
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
                    skipOneOfReservedTokens< ReservedToken::OpParenthesisClose >( tokenIt );
                }

                if ( tokenIt->is< ReservedToken >() && tokenIt->get< ReservedToken >() == ReservedToken::OpArrow )
                {
                    ++tokenIt;
                    operands.push( parse( tokenIt ) ); // parse type
                    operatorInfo.operandsCount++;
                }

                skipOneOfReservedTokens< ReservedToken::OpColon >( tokenIt );
                skipNewline( tokenIt );

                auto bodyOperands{ parseBody( tokenIt, indentationIdx + 1U ) };
                operatorInfo.operandsCount += std::size( bodyOperands );
                fill( operands, std::move( bodyOperands ) );
            }
            else if ( operatorInfo.type == NodeType::VariableDefinition )
            {
                skipOneOfReservedTokens< ReservedToken::KwLet >( tokenIt );
                operands.push( parseOperand( tokenIt ) ); // parse variable name
                ++tokenIt;

                if ( tokenIt->is< ReservedToken >() && tokenIt->get< ReservedToken >() == ReservedToken::OpColon )
                {
                    // there is type declaration in this variable definition
                    ++tokenIt;
                    operands.push( parseOperand( tokenIt ) ); // parse type
                    operatorInfo.operandsCount++;
                    ++tokenIt;
                }

                if ( tokenIt->is< ReservedToken >() && tokenIt->get< ReservedToken >() == ReservedToken::OpAssign )
                {
                    // there is initialization in this variable definition
                    ++tokenIt;
                    operands.push( parse( tokenIt ) );
                    operatorInfo.operandsCount++;
                }
            }
            else if ( operatorInfo.type == NodeType::ContractDefinition )
            {
                skipOneOfReservedTokens< ReservedToken::KwContract >( tokenIt );
                operands.push( parse( tokenIt ) ); // parse contract name
                skipOneOfReservedTokens< ReservedToken::OpColon >( tokenIt );
                skipNewline( tokenIt );

                auto bodyOperands{ parseBody( tokenIt, indentationIdx + 1U ) };
                operatorInfo.operandsCount += std::size( bodyOperands );
                fill( operands, std::move( bodyOperands ) );
            }

            if ( !operators.empty() && operators.top().type == NodeType::ModuleDefinition )
            {
                operators.top().operandsCount++;
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

        if ( tokenIt->is< Newline >() )
        {
            break;
        }
    }

    if ( expectingOperand && operands.empty() )
    {
        throw std::runtime_error( "Syntax error - expecting an operand" );
    }

    while ( !operators.empty() && operators.top().type != NodeType::ModuleDefinition )
    {
        popOneOperator( operands, operators, tokenIt->errorInfo());
    }

    if ( !operators.empty() && operators.top().type == NodeType::ModuleDefinition )
    {
        while ( tokenIt->is_not< Eof >() )
        {
            operands.push( parse( tokenIt ) );
            operators.top().operandsCount++;
        }

        while ( !operators.empty() )
        {
            popOneOperator( operands, operators, tokenIt->errorInfo());
        }
    }

    return operands.empty() ? nullptr : std::move( operands.top() );
}

} // namespace A1