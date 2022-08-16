/**
 *
 * Copyright (c)2022 The Adamnite C++ Authors.
 *
 * This code is open-sourced under the MIT license.
 */

#include <CoreLib/Tokenizer/Tokenizer.hpp>

#include "TestUtils.hpp"

#include <gtest/gtest.h>

namespace
{
    template< typename T >
    void match( T && token, std::size_t const tokenIndex, A1::TokenIterator const & tokenIt )
    {
        using CleanT = std::remove_cvref_t< T >;

        ASSERT_TRUE( std::holds_alternative< CleanT >( tokenIt->value() ) )
            << "Token at position " << ( tokenIndex + 1 ) << " is of an incorrect type";

        if constexpr ( std::same_as< CleanT, A1::ReservedToken > )
        {
            ASSERT_PRED_FORMAT2( A1::areEqual, std::get< A1::ReservedToken >( tokenIt->value() ), token );
        }
        else if constexpr ( std::same_as< CleanT, A1::Identifier > )
        {
            ASSERT_EQ( std::get< A1::Identifier >( tokenIt->value() ).name, token.name );
        }
        else if constexpr ( std::same_as< CleanT, A1::Number > )
        {
            ASSERT_EQ( std::get< A1::Number >( tokenIt->value() ), token );
        }
        else if constexpr ( std::same_as< CleanT, A1::String > )
        {
            ASSERT_EQ( std::get< A1::String >( tokenIt->value() ), token );
        }
    }

    template< typename ... Ts >
    void matchTokenization( std::string_view const expression, Ts && ... tokens )
    {
        std::size_t tokenIndex{ 0U };

        auto tokenIt{ A1::tokenize( A1::PushBackStream{ expression } ) };
        ( match( tokens, tokenIndex++, tokenIt++ ), ... );
    }
} // namespace

TEST( TokenizerTest, tokenization )
{
    {
        constexpr auto expression{ "var = 5" };
        EXPECT_NO_FATAL_FAILURE
        (
            matchTokenization
            (
                expression,
                A1::Identifier{ .name = "var" },
                A1::ReservedToken::OpAssign,
                A1::Number{ 5 },
                A1::Eof{}
            )
        ) << "Tokenization failure: '" << expression << "'";
    }
    {
        constexpr auto expression{ "if var == \"foo\"" };
        EXPECT_NO_FATAL_FAILURE
        (
            matchTokenization
            (
                expression,
                A1::ReservedToken::KwIf,
                A1::Identifier{ .name = "var" },
                A1::ReservedToken::OpEqual,
                A1::String{ "foo" },
                A1::Eof{}
            )
        ) << "Tokenization failure: '" << expression << "'";
    }
    {
        constexpr auto expression{ "return 5 < 2 # comment" };
        EXPECT_NO_FATAL_FAILURE
        (
            matchTokenization
            (
                expression,
                A1::ReservedToken::KwReturn,
                A1::Number{ 5 },
                A1::ReservedToken::OpLessThan,
                A1::Number{ 2 },
                A1::Eof{},
                A1::Eof{} // after first EOF, there should always be just EOF again
            )
        ) << "Tokenization failure: '" << expression << "'";
    }
    {
        constexpr auto expression{ "fun(1, 2, 3)" };
        EXPECT_NO_FATAL_FAILURE
        (
            matchTokenization
            (
                expression,
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
            )
        ) << "Tokenization failure: '" << expression << "'";
    }
    {
        constexpr auto expression{ "while var1 >= var2 && var3 == \"foo\"" };
        EXPECT_NO_FATAL_FAILURE
        (
            matchTokenization
            (
                expression,
                A1::ReservedToken::KwWhile,
                A1::Identifier{ .name = "var1" },
                A1::ReservedToken::OpGreaterThanEqual,
                A1::Identifier{ .name = "var2" },
                A1::ReservedToken::OpLogicalAnd,
                A1::Identifier{ .name = "var3" },
                A1::ReservedToken::OpEqual,
                A1::String{ "foo" },
                A1::Eof{}
            )
        ) << "Tokenization failure: '" << expression << "'";
    }
    {
        constexpr auto expression
        {
            "for key, value in items:\n"
            "    if key in foo_dict:\n"
            "        pass # avoid repeating keys\n"
            "    else:\n"
            "        foo_dict[key] = value"
        };
        EXPECT_NO_FATAL_FAILURE
        (
            matchTokenization
            (
                expression,

                // 1st line
                A1::ReservedToken::KwFor,
                A1::Identifier{ .name = "key" },
                A1::ReservedToken::OpComma,
                A1::Identifier{ .name = "value" },
                A1::ReservedToken::KwIn,
                A1::Identifier{ .name = "items" },
                A1::ReservedToken::OpColon,

                // 2nd line
                A1::ReservedToken::KwIf,
                A1::Identifier{ .name = "key" },
                A1::ReservedToken::KwIn,
                A1::Identifier{ .name = "foo_dict" },
                A1::ReservedToken::OpColon,

                // 3rd line
                A1::ReservedToken::KwPass,

                // 4th line
                A1::ReservedToken::KwElse,
                A1::ReservedToken::OpColon,

                // 5th line
                A1::Identifier{ .name = "foo_dict" },
                A1::ReservedToken::OpIndexOpen,
                A1::Identifier{ .name = "key" },
                A1::ReservedToken::OpIndexClose,
                A1::ReservedToken::OpAssign,
                A1::Identifier{ .name = "value" },

                A1::Eof{}
            )
        ) << "Tokenization failure: '" << expression << "'";
    }
}