/**
 *
 * Copyright (c)2022 The Adamnite C++ Authors.
 *
 * This code is open-sourced under the MIT license.
 */

#include <CoreLib/Tokenizer/Tokenizer.hpp>

#include "TestUtils.hpp"

#include <gtest/gtest.h>

TEST( TokenizerTest, tokenization )
{
    {
        auto it{ A1::tokenize( A1::PushBackStream{ "my_variable = 5" } ) };

        ASSERT_TRUE( std::holds_alternative< A1::Identifier >( it->value() ) );
        EXPECT_EQ( std::get< A1::Identifier >( it->value() ).name, "my_variable" );

        ++it;

        ASSERT_TRUE( std::holds_alternative< A1::ReservedToken >( it->value() ) );
        EXPECT_PRED_FORMAT2( A1::areEqual, std::get< A1::ReservedToken >( it->value() ), A1::ReservedToken::OpAssign );

        ++it;

        ASSERT_TRUE( std::holds_alternative< double >( it->value() ) );
        EXPECT_EQ( std::get< double >( it->value() ), 5. );

        ++it;

        ASSERT_TRUE( std::holds_alternative< A1::Eof >( it->value() ) );

        ++it;

        ASSERT_TRUE( std::holds_alternative< A1::Eof >( it->value() ) );
    }
    {
        auto it{ A1::tokenize( A1::PushBackStream{ "if my_variable == \"foo\"" } ) };

        ASSERT_TRUE( std::holds_alternative< A1::ReservedToken >( it->value() ) );
        EXPECT_PRED_FORMAT2( A1::areEqual, std::get< A1::ReservedToken >( it->value() ), A1::ReservedToken::KwIf );

        ++it;

        ASSERT_TRUE( std::holds_alternative< A1::Identifier >( it->value() ) );
        EXPECT_EQ( std::get< A1::Identifier >( it->value() ).name, "my_variable" );

        ++it;

        ASSERT_TRUE( std::holds_alternative< A1::ReservedToken >( it->value() ) );
        EXPECT_PRED_FORMAT2( A1::areEqual, std::get< A1::ReservedToken >( it->value() ), A1::ReservedToken::OpEqual );

        ++it;

        ASSERT_TRUE( std::holds_alternative< std::string >( it->value() ) );
        EXPECT_EQ( std::get< std::string >( it->value() ), "foo" );

        ++it;

        ASSERT_TRUE( std::holds_alternative< A1::Eof >( it->value() ) );

        ++it;

        ASSERT_TRUE( std::holds_alternative< A1::Eof >( it->value() ) );
    }
    {
        auto it{ A1::tokenize( A1::PushBackStream{ "return 5 < 2 # comment" } ) };

        ASSERT_TRUE( std::holds_alternative< A1::ReservedToken >( it->value() ) );
        EXPECT_PRED_FORMAT2( A1::areEqual, std::get< A1::ReservedToken >( it->value() ), A1::ReservedToken::KwReturn );

        ++it;

        ASSERT_TRUE( std::holds_alternative< double >( it->value() ) );
        EXPECT_EQ( std::get< double >( it->value() ), 5 );

        ++it;

        ASSERT_TRUE( std::holds_alternative< A1::ReservedToken >( it->value() ) );
        EXPECT_PRED_FORMAT2( A1::areEqual, std::get< A1::ReservedToken >( it->value() ), A1::ReservedToken::OpLessThan );

        ++it;

        ASSERT_TRUE( std::holds_alternative< double >( it->value() ) );
        EXPECT_EQ( std::get< double >( it->value() ), 2 );

        ++it;

        ASSERT_TRUE( std::holds_alternative< A1::Eof >( it->value() ) );

        ++it;

        ASSERT_TRUE( std::holds_alternative< A1::Eof >( it->value() ) );
    }
}