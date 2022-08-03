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
    testing::AssertionResult areEqual
    (
        char const *,
        char const *,
        A1::ReservedToken const value1,
        A1::ReservedToken const value2
    )
    {
        if ( value1 == value2 ) { return testing::AssertionSuccess(); }

        return testing::AssertionFailure()
            << "Expected "                << A1::toStringView( value2 )
            << " reserved token but got " << A1::toStringView( value1 );
    }
} // namespace

TEST( TokenizerTest, tokenizationTest )
{
    {
        A1::TokenIterator it{ A1::PushBackStream{ "my_variable = 5" } };

        EXPECT_TRUE( std::holds_alternative< A1::Identifier >( it->value() ) );
        EXPECT_EQ( std::get< A1::Identifier >( it->value() ).name, "my_variable" );

        it++;

        EXPECT_TRUE( std::holds_alternative< A1::ReservedToken >( it->value() ) );
        EXPECT_PRED_FORMAT2( areEqual, std::get< A1::ReservedToken >( it->value() ), A1::ReservedToken::OpAssign );

        it++;

        EXPECT_TRUE( std::holds_alternative< double >( it->value() ) );
        EXPECT_EQ( std::get< double >( it->value() ), 5. );

        it++;

        EXPECT_TRUE( std::holds_alternative< A1::Eof >( it->value() ) );
    }
    {
        A1::TokenIterator it{ A1::PushBackStream{ "if my_variable == \"foo\"" } };

        EXPECT_TRUE( std::holds_alternative< A1::ReservedToken >( it->value() ) );
        EXPECT_PRED_FORMAT2( areEqual, std::get< A1::ReservedToken >( it->value() ), A1::ReservedToken::KwIf );

        it++;

        EXPECT_TRUE( std::holds_alternative< A1::Identifier >( it->value() ) );
        EXPECT_EQ( std::get< A1::Identifier >( it->value() ).name, "my_variable" );

        it++;

        EXPECT_TRUE( std::holds_alternative< A1::ReservedToken >( it->value() ) );
        EXPECT_PRED_FORMAT2( areEqual, std::get< A1::ReservedToken >( it->value() ), A1::ReservedToken::OpEqual );

        it++;

        EXPECT_TRUE( std::holds_alternative< std::string >( it->value() ) );
        EXPECT_EQ( std::get< std::string >( it->value() ), "foo" );

        it++;

        EXPECT_TRUE( std::holds_alternative< A1::Eof >( it->value() ) );
    }

    {
        A1::TokenIterator it{ A1::PushBackStream{ "if 5 < 2" } };

        EXPECT_TRUE( std::holds_alternative< A1::ReservedToken >( it->value() ) );
        EXPECT_PRED_FORMAT2( areEqual, std::get< A1::ReservedToken >( it->value() ), A1::ReservedToken::KwIf );

        it++;

        EXPECT_TRUE( std::holds_alternative< double >( it->value() ) );
        EXPECT_EQ( std::get< double >( it->value() ), 5 );

        it++;

        EXPECT_TRUE( std::holds_alternative< A1::ReservedToken >( it->value() ) );
        EXPECT_PRED_FORMAT2( areEqual, std::get< A1::ReservedToken >( it->value() ), A1::ReservedToken::OpLessThan );

        it++;

        EXPECT_TRUE( std::holds_alternative< double >( it->value() ) );
        EXPECT_EQ( std::get< double >( it->value() ), 2.0 );

        it++;

        EXPECT_TRUE( std::holds_alternative< A1::Eof >( it->value() ) );
    }
}