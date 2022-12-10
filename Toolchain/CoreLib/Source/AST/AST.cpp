/**
 *
 * Copyright (c)2022 The Adamnite C++ Authors.
 *
 * This code is open-sourced under the MIT license.
 */

#include <CoreLib/AST/AST.hpp>
#include <CoreLib/AST/ASTNode.hpp>
#include <CoreLib/Utils/Macros.hpp>

#include "ASTHelper.hpp"

#include <fmt/format.h>

#include <stdexcept>
#include <cstdint>

namespace A1::AST
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
        if ( token.is< Indentation >() || token.is< Newline >() || token.is< Eof >() )
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

    struct NodeInfo
    {
        NodeType     type{ NodeType::Unknown };
        ErrorInfo    errorInfo{};
        std::size_t  operandsCount{ 0U };
    };

    [[ nodiscard ]] NodeInfo getNodeInfo
    (
        ReservedToken const token,
        ErrorInfo           errorInfo,
        bool          const isPrefix
    )
    {

#define MAP_TOKEN_TO_NODE( token, nodeType )                        \
    case ReservedToken::token:                                      \
        return                                                      \
        {                                                           \
            .type          = NodeType::nodeType,                    \
            .errorInfo     = std::move( errorInfo ),                \
            .operandsCount = getOperandsCount( NodeType::nodeType ) \
        }

        switch ( token )
        {
            MAP_TOKEN_TO_NODE( OpParenthesisOpen, Parentheses );
            MAP_TOKEN_TO_NODE( OpSubscriptOpen  , Index       );
            MAP_TOKEN_TO_NODE( OpDot            , MemberCall  );

            // Arithmetic operators
            MAP_TOKEN_TO_NODE( OpDiv     , Division       );
            MAP_TOKEN_TO_NODE( OpExp     , Exponent       );
            MAP_TOKEN_TO_NODE( OpFloorDiv, FloorDivision  );
            MAP_TOKEN_TO_NODE( OpMod     , Modulus        );
            MAP_TOKEN_TO_NODE( OpMul     , Multiplication );

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
            MAP_TOKEN_TO_NODE( OpBitwiseAnd       , BitwiseAnd        );
            MAP_TOKEN_TO_NODE( OpBitwiseLeftShift , BitwiseLeftShift  );
            MAP_TOKEN_TO_NODE( OpBitwiseNot       , BitwiseNot        );
            MAP_TOKEN_TO_NODE( OpBitwiseOr        , BitwiseOr         );
            MAP_TOKEN_TO_NODE( OpBitwiseRightShift, BitwiseRightShift );
            MAP_TOKEN_TO_NODE( OpBitwiseXor       , BitwiseXor        );

            // Logical operators
            MAP_TOKEN_TO_NODE( OpLogicalAnd, LogicalAnd );
            MAP_TOKEN_TO_NODE( OpLogicalNot, LogicalNot );
            MAP_TOKEN_TO_NODE( OpLogicalOr , LogicalOr  );

            // Assignment operators
            MAP_TOKEN_TO_NODE( OpAssign        , Assign               );
            MAP_TOKEN_TO_NODE( OpAssignAdd     , AssignAddition       );
            MAP_TOKEN_TO_NODE( OpAssignDiv     , AssignDivision       );
            MAP_TOKEN_TO_NODE( OpAssignExp     , AssignExponent       );
            MAP_TOKEN_TO_NODE( OpAssignFloorDiv, AssignFloorDivision  );
            MAP_TOKEN_TO_NODE( OpAssignMod     , AssignModulus        );
            MAP_TOKEN_TO_NODE( OpAssignMul     , AssignMultiplication );
            MAP_TOKEN_TO_NODE( OpAssignSub     , AssignSubtraction    );

            // Bitwise assignment operators
            MAP_TOKEN_TO_NODE( OpAssignBitwiseAnd       , AssignBitwiseAnd        );
            MAP_TOKEN_TO_NODE( OpAssignBitwiseLeftShift , AssignBitwiseLeftShift  );
            MAP_TOKEN_TO_NODE( OpAssignBitwiseOr        , AssignBitwiseOr         );
            MAP_TOKEN_TO_NODE( OpAssignBitwiseRightShift, AssignBitwiseRightShift );
            MAP_TOKEN_TO_NODE( OpAssignBitwiseXor       , AssignBitwiseXor        );

            // Comparison operators
            MAP_TOKEN_TO_NODE( OpEqual           , Equality         );
            MAP_TOKEN_TO_NODE( OpGreaterThan     , GreaterThan      );
            MAP_TOKEN_TO_NODE( OpGreaterThanEqual, GreaterThanEqual );
            MAP_TOKEN_TO_NODE( OpLessThan        , LessThan         );
            MAP_TOKEN_TO_NODE( OpLessThanEqual   , LessThanEqual    );
            MAP_TOKEN_TO_NODE( OpNotEqual        , Inequality       );

            // Statements
            MAP_TOKEN_TO_NODE( KwIf    , StatementIf     );
            MAP_TOKEN_TO_NODE( KwElif  , StatementElif   );
            MAP_TOKEN_TO_NODE( KwElse  , StatementElse   );
            MAP_TOKEN_TO_NODE( KwWhile , StatementWhile  );
            MAP_TOKEN_TO_NODE( KwPass  , StatementPass   );
            MAP_TOKEN_TO_NODE( KwReturn, StatementReturn );
            MAP_TOKEN_TO_NODE( KwImport, StatementImport );

            // Definitions
            MAP_TOKEN_TO_NODE( KwDef     , FunctionDefinition );
            MAP_TOKEN_TO_NODE( KwLet     , VariableDefinition );
            MAP_TOKEN_TO_NODE( KwContract, ContractDefinition );

#define IGNORE_TOKEN( token ) case ReservedToken::token: break;

            // Ignored tokens
            IGNORE_TOKEN( Unknown            );
            IGNORE_TOKEN( OpParenthesisClose );
            IGNORE_TOKEN( OpSubscriptClose   );
            IGNORE_TOKEN( OpColon            );
            IGNORE_TOKEN( OpComma            );
            IGNORE_TOKEN( KwNumber           );
            IGNORE_TOKEN( KwString           );

#undef IGNORE_TOKEN

            default:
                throw std::runtime_error( fmt::format( "Unknown token: {}", toStringView( token ) ) );
        }

        return {};

#undef MAP_TOKEN_TO_NODE
    }

    [[ nodiscard ]]
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

    void popOperator
    (
        std::stack< Node::Pointer > & operands,
        std::stack< NodeInfo      > & operators,
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
    void skipOneOf( TokenIterator & tokenIt )
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

    Node::Pointer parseImpl
    (
        TokenIterator       & tokenIt,
        std::size_t   const   indentationLevel = 0U,
        bool          const   alreadyInModule  = true
    );

    [[ nodiscard ]]
    std::vector< Node::Pointer > parseBody( TokenIterator & tokenIt, std::size_t const indentationLevel )
    {
        std::vector< Node::Pointer > operands;

        while ( tokenIt->is_not< Eof >() )
        {
            // parse body statement
            operands.push_back( parseImpl( tokenIt, indentationLevel ) );

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

    Node::Pointer parseImpl
    (
        TokenIterator       & tokenIt,
        std::size_t   const   indentationLevel,
        bool          const   alreadyInModule
    )
    {
        std::stack< Node::Pointer > operands;
        std::stack< NodeInfo      > operators;

        auto expectingOperand{ true };

        std::size_t currentIndentationLevel{ 0U };
        while ( currentIndentationLevel != indentationLevel )
        {
            if ( tokenIt->is< Indentation >() )
            {
                ++tokenIt;
                currentIndentationLevel++;
            }
            else
            {
                currentIndentationLevel = indentationLevel;
                break;
            }
        }

        if ( !alreadyInModule )
        {
            operators.push
            (
                NodeInfo
                {
                    .type      = NodeType::ModuleDefinition,
                    .errorInfo = tokenIt->errorInfo()
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
                auto nodeInfo
                {
                    getNodeInfo
                    (
                        tokenIt->get< ReservedToken >(),
                        tokenIt->errorInfo(),
                        expectingOperand
                    )
                };

                if ( nodeInfo.type == NodeType::Unknown )
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

                if ( !operators.empty() && operators.top().type != NodeType::ModuleDefinition && hasHigherPrecedence( operators.top().type, nodeInfo.type ) )
                {
                    popOperator( operands, operators, tokenIt->errorInfo() );

                    if ( !operators.empty() && operators.top().type == NodeType::ModuleDefinition )
                    {
                        operators.top().operandsCount--;
                    }
                }

                if ( nodeInfo.type == NodeType::Parentheses )
                {
                    skipOneOf< ReservedToken::OpParenthesisOpen >( tokenIt );

                    if ( tokenIt->is< ReservedToken >() && tokenIt->get< ReservedToken >() == ReservedToken::OpParenthesisClose )
                    {
                        throw std::runtime_error( "Expected expression" );
                    }

                    if ( tokenIt->is_not< ReservedToken >() || tokenIt->get< ReservedToken >() != ReservedToken::OpParenthesisClose )
                    {
                        while ( true )
                        {
                            operands.push( parseImpl( tokenIt ) );

                            ++nodeInfo.operandsCount;

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

                        --nodeInfo.operandsCount;
                    }
                }
                else if ( nodeInfo.type == NodeType::Index )
                {
                    skipOneOf< ReservedToken::OpSubscriptOpen >( tokenIt );
                    operands.push( parseImpl( tokenIt ) );
                    skipOneOf< ReservedToken::OpSubscriptClose >( tokenIt );
                }
                else if ( nodeInfo.type == NodeType::StatementIf || nodeInfo.type == NodeType::StatementElif )
                {
                    skipOneOf< ReservedToken::KwIf, ReservedToken::KwElif >( tokenIt );
                    operands.push( parseImpl( tokenIt ) ); // parse condition
                    skipOneOf< ReservedToken::OpColon >( tokenIt );
                    skipNewline( tokenIt );

                    auto bodyOperands{ parseBody( tokenIt, currentIndentationLevel + 1U ) };
                    nodeInfo.operandsCount += std::size( bodyOperands );
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
                        operands.push( parseImpl( tokenIt, currentIndentationLevel ) );
                        nodeInfo.operandsCount++;
                    }
                    else
                    {
                        tokenIt = prevTokenIt;
                    }
                }
                else if ( nodeInfo.type == NodeType::StatementElse )
                {
                    skipOneOf< ReservedToken::KwElse  >( tokenIt );
                    skipOneOf< ReservedToken::OpColon >( tokenIt );
                    skipNewline( tokenIt );

                    auto bodyOperands{ parseBody( tokenIt, currentIndentationLevel + 1U ) };
                    nodeInfo.operandsCount += std::size( bodyOperands );
                    fill( operands, std::move( bodyOperands ) );
                }
                else if ( nodeInfo.type == NodeType::StatementWhile )
                {
                    skipOneOf< ReservedToken::KwWhile >( tokenIt );
                    operands.push( parseImpl( tokenIt ) ); // parse condition
                    skipOneOf< ReservedToken::OpColon >( tokenIt );
                    skipNewline( tokenIt );

                    auto bodyOperands{ parseBody( tokenIt, currentIndentationLevel + 1U ) };
                    nodeInfo.operandsCount += std::size( bodyOperands );
                    fill( operands, std::move( bodyOperands ) );
                }
                else if ( nodeInfo.type == NodeType::FunctionDefinition )
                {
                    skipOneOf< ReservedToken::KwDef >( tokenIt );

                    operands.push( parseOperand( tokenIt ) ); // parse function name
                    tokenIt++;

                    {
                        // we are parsing function parameters

                        skipOneOf< ReservedToken::OpParenthesisOpen >( tokenIt );

                        if ( tokenIt->is< Identifier >() && tokenIt->get< Identifier >().name == "self" )
                        {
                            NodeInfo const parameterDefinition
                            {
                                .type          = NodeType::FunctionParameterDefinition,
                                .errorInfo     = tokenIt->errorInfo(),
                                .operandsCount = 1U
                            };

                            operands.push( parseImpl( tokenIt ) ); // parse self parameter name
                            operators.push( parameterDefinition );

                            while ( !operators.empty() && operators.top().type != NodeType::ModuleDefinition )
                            {
                                popOperator( operands, operators, tokenIt->errorInfo());
                            }

                            ++nodeInfo.operandsCount;

                            ++tokenIt;
                            if ( tokenIt->is< ReservedToken >() )
                            {
                                auto const reservedToken{ tokenIt->get< ReservedToken >() };
                                if ( reservedToken == ReservedToken::OpComma )
                                {
                                    ++tokenIt;
                                }
                                else if ( reservedToken != ReservedToken::OpParenthesisClose )
                                {
                                    throw std::runtime_error( "Syntax error - expecting ',' or ')'" );
                                }
                            }
                        }

                        if ( tokenIt->is_not< ReservedToken >() || tokenIt->get< ReservedToken >() != ReservedToken::OpParenthesisClose )
                        {
                            while ( true )
                            {
                                NodeInfo const parameterDefinition
                                {
                                    .type          = NodeType::FunctionParameterDefinition,
                                    .errorInfo     = tokenIt->errorInfo(),
                                    .operandsCount = getOperandsCount( NodeType::FunctionParameterDefinition )
                                };

                                operands.push( parseImpl( tokenIt ) ); // parse parameter name
                                skipOneOf< ReservedToken::OpColon >( tokenIt );
                                operands.push( parseImpl( tokenIt ) ); // parse parameter type

                                operators.push( parameterDefinition );

                                while ( !operators.empty() && operators.top().type != NodeType::ModuleDefinition )
                                {
                                    popOperator( operands, operators, tokenIt->errorInfo());
                                }

                                nodeInfo.operandsCount++;

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
                        skipOneOf< ReservedToken::OpParenthesisClose >( tokenIt );
                    }

                    if ( tokenIt->is< ReservedToken >() && tokenIt->get< ReservedToken >() == ReservedToken::OpArrow )
                    {
                        ++tokenIt;
                        operands.push( parseImpl( tokenIt ) ); // parse type
                        nodeInfo.operandsCount++;
                    }

                    skipOneOf< ReservedToken::OpColon >( tokenIt );
                    skipNewline( tokenIt );

                    auto bodyOperands{ parseBody( tokenIt, currentIndentationLevel + 1U ) };
                    nodeInfo.operandsCount += std::size( bodyOperands );
                    fill( operands, std::move( bodyOperands ) );
                }
                else if ( nodeInfo.type == NodeType::VariableDefinition )
                {
                    skipOneOf< ReservedToken::KwLet >( tokenIt );
                    operands.push( parseOperand( tokenIt ) ); // parse variable name
                    ++tokenIt;

                    if ( tokenIt->is< ReservedToken >() && tokenIt->get< ReservedToken >() == ReservedToken::OpColon )
                    {
                        // there is type declaration in this variable definition
                        ++tokenIt;
                        operands.push( parseOperand( tokenIt ) ); // parse type
                        nodeInfo.operandsCount++;
                        ++tokenIt;
                    }

                    if ( tokenIt->is< ReservedToken >() && tokenIt->get< ReservedToken >() == ReservedToken::OpAssign )
                    {
                        // there is initialization in this variable definition
                        ++tokenIt;
                        operands.push( parseImpl( tokenIt ) );
                        nodeInfo.operandsCount++;
                    }
                }
                else if ( nodeInfo.type == NodeType::ContractDefinition )
                {
                    skipOneOf< ReservedToken::KwContract >( tokenIt );
                    operands.push( parseImpl( tokenIt ) ); // parse contract name
                    skipOneOf< ReservedToken::OpColon >( tokenIt );
                    skipNewline( tokenIt );

                    auto bodyOperands{ parseBody( tokenIt, currentIndentationLevel + 1U ) };
                    nodeInfo.operandsCount += std::size( bodyOperands );
                    fill( operands, std::move( bodyOperands ) );
                }

                if ( !operators.empty() && operators.top().type == NodeType::ModuleDefinition )
                {
                    operators.top().operandsCount++;
                }

                operators.push( nodeInfo );

                expectingOperand = nodeInfo.operandsCount != 0;
            }
            else
            {
                if ( !expectingOperand )
                {
                    throw std::runtime_error( "Syntax error" );
                }

                operands.push( parseOperand( tokenIt ) );
                expectingOperand = false;

                if ( operands.top()->is< Identifier >() )
                {
                    auto oldToken{ tokenIt };
                    ++tokenIt;

                    if ( tokenIt->is< ReservedToken >() && tokenIt->get< ReservedToken >() == ReservedToken::OpParenthesisOpen )
                    {
                        NodeInfo nodeInfo
                        {
                            .type          = NodeType::Call,
                            .errorInfo     = tokenIt->errorInfo(),
                            .operandsCount = getOperandsCount( NodeType::Call )
                        };

                        skipOneOf< ReservedToken::OpParenthesisOpen >( tokenIt );

                        if ( tokenIt->is_not< ReservedToken >() || tokenIt->get< ReservedToken >() != ReservedToken::OpParenthesisClose )
                        {
                            while ( true )
                            {
                                operands.push( parseImpl( tokenIt ) );

                                ++nodeInfo.operandsCount;

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

                        if ( !operators.empty() && operators.top().type == NodeType::ModuleDefinition )
                        {
                            operators.top().operandsCount++;
                        }

                        operators.push( nodeInfo );

                        expectingOperand = nodeInfo.operandsCount != 0;
                    }
                    else
                    {
                        tokenIt = oldToken;
                    }
                }
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
            popOperator( operands, operators, tokenIt->errorInfo());
        }

        if ( !operators.empty() && operators.top().type == NodeType::ModuleDefinition )
        {
            while ( tokenIt->is_not< Eof >() )
            {
                operands.push( parseImpl( tokenIt ) );
                operators.top().operandsCount++;
            }

            while ( !operators.empty() )
            {
                popOperator( operands, operators, tokenIt->errorInfo());
            }
        }

        return operands.empty() ? nullptr : std::move( operands.top() );
    }
} // namespace

Node::Pointer parse( TokenIterator & tokenIt )
{
    return parseImpl( tokenIt, 0U, false );
}

} // namespace A1::AST