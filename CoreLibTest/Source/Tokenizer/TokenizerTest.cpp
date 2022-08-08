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
        auto tokenIt{ A1::tokenize( A1::PushBackStream{ "my_variable = 5" } ) };

        ASSERT_TRUE( std::holds_alternative< A1::Identifier >( tokenIt->value() ) );
        EXPECT_EQ( std::get< A1::Identifier >( tokenIt->value() ).name, "my_variable" );

        ++tokenIt;

        ASSERT_TRUE( std::holds_alternative< A1::ReservedToken >( tokenIt->value() ) );
        EXPECT_PRED_FORMAT2( A1::areEqual, std::get< A1::ReservedToken >( tokenIt->value() ), A1::ReservedToken::OpAssign );

        ++tokenIt;

        ASSERT_TRUE( std::holds_alternative< double >( tokenIt->value() ) );
        EXPECT_EQ( std::get< double >( tokenIt->value() ), 5. );

        ++tokenIt;

        ASSERT_TRUE( std::holds_alternative< A1::Eof >( tokenIt->value() ) );

        ++tokenIt;

        ASSERT_TRUE( std::holds_alternative< A1::Eof >( tokenIt->value() ) );
    }
    {
        auto tokenIt{ A1::tokenize( A1::PushBackStream{ "if my_variable == \"foo\"" } ) };

        ASSERT_TRUE( std::holds_alternative< A1::ReservedToken >( tokenIt->value() ) );
        EXPECT_PRED_FORMAT2( A1::areEqual, std::get< A1::ReservedToken >( tokenIt->value() ), A1::ReservedToken::KwIf );

        ++tokenIt;

        ASSERT_TRUE( std::holds_alternative< A1::Identifier >( tokenIt->value() ) );
        EXPECT_EQ( std::get< A1::Identifier >( tokenIt->value() ).name, "my_variable" );

        ++tokenIt;

        ASSERT_TRUE( std::holds_alternative< A1::ReservedToken >( tokenIt->value() ) );
        EXPECT_PRED_FORMAT2( A1::areEqual, std::get< A1::ReservedToken >( tokenIt->value() ), A1::ReservedToken::OpEqual );

        ++tokenIt;

        ASSERT_TRUE( std::holds_alternative< std::string >( tokenIt->value() ) );
        EXPECT_EQ( std::get< std::string >( tokenIt->value() ), "foo" );

        ++tokenIt;

        ASSERT_TRUE( std::holds_alternative< A1::Eof >( tokenIt->value() ) );

        ++tokenIt;

        ASSERT_TRUE( std::holds_alternative< A1::Eof >( tokenIt->value() ) );
    }
    {
        auto tokenIt{ A1::tokenize( A1::PushBackStream{ "return 5 < 2 # comment" } ) };

        ASSERT_TRUE( std::holds_alternative< A1::ReservedToken >( tokenIt->value() ) );
        EXPECT_PRED_FORMAT2( A1::areEqual, std::get< A1::ReservedToken >( tokenIt->value() ), A1::ReservedToken::KwReturn );

        ++tokenIt;

        ASSERT_TRUE( std::holds_alternative< double >( tokenIt->value() ) );
        EXPECT_EQ( std::get< double >( tokenIt->value() ), 5 );

        ++tokenIt;

        ASSERT_TRUE( std::holds_alternative< A1::ReservedToken >( tokenIt->value() ) );
        EXPECT_PRED_FORMAT2( A1::areEqual, std::get< A1::ReservedToken >( tokenIt->value() ), A1::ReservedToken::OpLessThan );

        ++tokenIt;

        ASSERT_TRUE( std::holds_alternative< double >( tokenIt->value() ) );
        EXPECT_EQ( std::get< double >( tokenIt->value() ), 2 );

        ++tokenIt;

        ASSERT_TRUE( std::holds_alternative< A1::Eof >( tokenIt->value() ) );

        ++tokenIt;

        ASSERT_TRUE( std::holds_alternative< A1::Eof >( tokenIt->value() ) );
    }
    {
        auto tokenIt{ A1::tokenize( A1::PushBackStream{ "fun(1, 2, 3)" } ) };

        ASSERT_TRUE( std::holds_alternative< A1::Identifier >( tokenIt->value() ) );
        EXPECT_EQ( std::get< A1::Identifier >( tokenIt->value() ).name, "fun" );

        ++tokenIt;

        ASSERT_TRUE( std::holds_alternative< A1::ReservedToken >( tokenIt->value() ) );
        EXPECT_PRED_FORMAT2( A1::areEqual, std::get< A1::ReservedToken >( tokenIt->value() ), A1::ReservedToken::OpCallOpen );

        ++tokenIt;

        ASSERT_TRUE( std::holds_alternative< double >( tokenIt->value() ) );
        EXPECT_EQ( std::get< double >( tokenIt->value() ), 1 );

        ++tokenIt;

        ASSERT_TRUE( std::holds_alternative< A1::ReservedToken >( tokenIt->value() ) );
        EXPECT_PRED_FORMAT2( A1::areEqual, std::get< A1::ReservedToken >( tokenIt->value() ), A1::ReservedToken::OpComma );

        ++tokenIt;

        ASSERT_TRUE( std::holds_alternative< double >( tokenIt->value() ) );
        EXPECT_EQ( std::get< double >( tokenIt->value() ), 2 );

        ++tokenIt;

        ASSERT_TRUE( std::holds_alternative< A1::ReservedToken >( tokenIt->value() ) );
        EXPECT_PRED_FORMAT2( A1::areEqual, std::get< A1::ReservedToken >( tokenIt->value() ), A1::ReservedToken::OpComma );

        ++tokenIt;

        ASSERT_TRUE( std::holds_alternative< double >( tokenIt->value() ) );
        EXPECT_EQ( std::get< double >( tokenIt->value() ), 3 );

        ++tokenIt;

        ASSERT_TRUE( std::holds_alternative< A1::ReservedToken >( tokenIt->value() ) );
        EXPECT_PRED_FORMAT2( A1::areEqual, std::get< A1::ReservedToken >( tokenIt->value() ), A1::ReservedToken::OpCallClose );

        ++tokenIt;

        ASSERT_TRUE( std::holds_alternative< A1::Eof >( tokenIt->value() ) );
    }
}