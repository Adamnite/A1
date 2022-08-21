/**
 *
 * Copyright (c)2022 The Adamnite C++ Authors.
 *
 * This code is open-sourced under the MIT license.
 */

#include <CoreLib/Tokenizer/Tokenizer.hpp>

#include <gtest/gtest.h>

namespace
{
    struct TestParameter
    {
        std::string_view title;
        std::string_view expression;

        std::vector< A1::Token::ValueType > expectedTokens;

        friend std::ostream & operator<<( std::ostream & os, TestParameter const & param )
        {
            return os << param.expression;
        }
    };

    struct TokenizerTestFixture : ::testing::TestWithParam< TestParameter >
    {
        struct PrintTitle
        {
            template< typename ParamType >
            std::string operator()( testing::TestParamInfo< ParamType > const & info ) const
            {
                auto parameter{ static_cast< TestParameter >( info.param ) };
                return std::string{ parameter.title };
            }
        };
    };
} // namespace

TEST_P( TokenizerTestFixture, tokenization )
{
    auto const [ _, expression, expectedTokens ]{ GetParam() };

    std::size_t tokenIndex{ 0U };

    auto tokenIt{ A1::tokenize( A1::PushBackStream{ expression } ) };
    for ( auto const & token : expectedTokens )
    {
        EXPECT_EQ( token, tokenIt->value() )
            << "Token at position " << ( tokenIndex + 1 ) << " is of an incorrect type";

        ++tokenIndex;
        ++tokenIt;
    }
}

INSTANTIATE_TEST_SUITE_P
(
    TokenizerTest,
    TokenizerTestFixture,
    ::testing::Values
    (
        TestParameter
        {
            .title          = "VariableAssignment",
            .expression     = "var = 5",
            .expectedTokens =
            {
                A1::Identifier{ .name = "var" },
                A1::ReservedToken::OpAssign,
                A1::Number{ 5 },
                A1::Eof{}
            }
        },
        TestParameter
        {
            .title          = "IfCondition",
            .expression     = "if var == \"foo\":",
            .expectedTokens =
            {
                A1::ReservedToken::KwIf,
                A1::Identifier{ .name = "var" },
                A1::ReservedToken::OpEqual,
                A1::String{ "foo" },
                A1::ReservedToken::OpColon,
                A1::Eof{}
            }
        },
        TestParameter
        {
            .title          = "ReturnConditionWithComment",
            .expression     = "return 5 < 2 # comment",
            .expectedTokens =
            {
                A1::ReservedToken::KwReturn,
                A1::Number{ 5 },
                A1::ReservedToken::OpLessThan,
                A1::Number{ 2 },
                A1::Eof{},
                A1::Eof{} // after first EOF, there should always be just EOF again
            }
        },
        TestParameter
        {
            .title          = "FunctionCall",
            .expression     = "fun(1, 2, 3)",
            .expectedTokens =
            {
                A1::Identifier{ .name = "fun" },
                A1::ReservedToken::OpCallOpen,
                A1::Number{ 1 },
                A1::ReservedToken::OpComma,
                A1::Number{ 2 },
                A1::ReservedToken::OpComma,
                A1::Number{ 3 },
                A1::ReservedToken::OpCallClose,
                A1::Eof{},
                A1::Eof{} // after first EOF, there should always be just EOF again
            }
        },
        TestParameter
        {
            .title          = "WhileCondition",
            .expression     = "while var1 >= var2 && var3 == \"foo\":",
            .expectedTokens =
            {
                A1::ReservedToken::KwWhile,
                A1::Identifier{ .name = "var1" },
                A1::ReservedToken::OpGreaterThanEqual,
                A1::Identifier{ .name = "var2" },
                A1::ReservedToken::OpLogicalAnd,
                A1::Identifier{ .name = "var3" },
                A1::ReservedToken::OpEqual,
                A1::String{ "foo" },
                A1::ReservedToken::OpColon,
                A1::Eof{}
            }
        },
        TestParameter
        {
            .title      = "ForLoop",
            .expression =
                "for key, value in items:\n"
                "    if key in foo_dict:\n"
                "        pass # avoid repeating keys\n"
                "    else:\n"
                "        foo_dict[key] = value",
            .expectedTokens =
            {
                // 1st line
                A1::ReservedToken::KwFor,
                A1::Identifier{ .name = "key" },
                A1::ReservedToken::OpComma,
                A1::Identifier{ .name = "value" },
                A1::ReservedToken::KwIn,
                A1::Identifier{ .name = "items" },
                A1::ReservedToken::OpColon,
                A1::Newline{},

                // 2nd line
                A1::ReservedToken::KwIf,
                A1::Identifier{ .name = "key" },
                A1::ReservedToken::KwIn,
                A1::Identifier{ .name = "foo_dict" },
                A1::ReservedToken::OpColon,
                A1::Newline{},

                // 3rd line
                A1::ReservedToken::KwPass,
                A1::Newline{},

                // 4th line
                A1::ReservedToken::KwElse,
                A1::ReservedToken::OpColon,
                A1::Newline{},

                // 5th line
                A1::Identifier{ .name = "foo_dict" },
                A1::ReservedToken::OpIndexOpen,
                A1::Identifier{ .name = "key" },
                A1::ReservedToken::OpIndexClose,
                A1::ReservedToken::OpAssign,
                A1::Identifier{ .name = "value" },
                A1::Eof{}
            }
        }
    ),
    TokenizerTestFixture::PrintTitle()
);
