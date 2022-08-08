/**
 *
 * Copyright (c)2022 The Adamnite C++ Authors.
 *
 * This code is open-sourced under the MIT license.
 */

#include <CoreLib/Parser/ExpressionTree.hpp>
#include <CoreLib/Tokenizer/Tokenizer.hpp>

#include <gtest/gtest.h>

TEST( ExpressionTreeTest, parsing )
{
    {
        auto tokenIt{ A1::tokenize( A1::PushBackStream{ "my_variable = 5" } ) };
        auto root   { A1::parse( tokenIt ) };

        ASSERT_TRUE( std::holds_alternative< A1::OperatorType >( root->value() ) );
        EXPECT_EQ( std::get< A1::OperatorType >( root->value() ), A1::OperatorType::Assign );

        auto const & children{ root->children() };
        ASSERT_EQ( children.size(), 2U );

        ASSERT_TRUE( std::holds_alternative< A1::Identifier >( children[ 0 ]->value() ) );
        EXPECT_EQ( std::get< A1::Identifier >( children[ 0 ]->value() ).name, "my_variable" );

        ASSERT_TRUE( std::holds_alternative< double >( children[ 1 ]->value() ) );
        EXPECT_EQ( std::get< double >( children[ 1 ]->value() ), 5. );
    }
    {
        auto tokenIt{ A1::tokenize( A1::PushBackStream{ "fun(1, \"my string\", 5)" } ) };
        auto root   { A1::parse( tokenIt ) };

        ASSERT_TRUE( std::holds_alternative< A1::OperatorType >( root->value() ) );
        EXPECT_EQ( std::get< A1::OperatorType >( root->value() ), A1::OperatorType::Call );

        auto const & children{ root->children() };
        ASSERT_EQ( children.size(), 4U );

        ASSERT_TRUE( std::holds_alternative< A1::Identifier >( children[ 0 ]->value() ) );
        EXPECT_EQ( std::get< A1::Identifier >( children[ 0 ]->value() ).name, "fun" );

        ASSERT_TRUE( std::holds_alternative< double >( children[ 1 ]->value() ) );
        EXPECT_EQ( std::get< double >( children[ 1 ]->value() ), 1. );

        ASSERT_TRUE( std::holds_alternative< std::string >( children[ 2 ]->value() ) );
        EXPECT_EQ( std::get< std::string >( children[ 2 ]->value() ), "my string" );

        ASSERT_TRUE( std::holds_alternative< double >( children[ 3 ]->value() ) );
        EXPECT_EQ( std::get< double >( children[ 3 ]->value() ), 5. );
    }
    {
        auto tokenIt{ A1::tokenize( A1::PushBackStream{ "arr[1]" } ) };
        auto root   { A1::parse( tokenIt ) };

        ASSERT_TRUE( std::holds_alternative< A1::OperatorType >( root->value() ) );
        EXPECT_EQ( std::get< A1::OperatorType >( root->value() ), A1::OperatorType::Index );

        auto const & children{ root->children() };
        ASSERT_EQ( children.size(), 2U );

        ASSERT_TRUE( std::holds_alternative< A1::Identifier >( children[ 0 ]->value() ) );
        EXPECT_EQ( std::get< A1::Identifier >( children[ 0 ]->value() ).name, "arr" );

        ASSERT_TRUE( std::holds_alternative< double >( children[ 1 ]->value() ) );
        EXPECT_EQ( std::get< double >( children[ 1 ]->value() ), 1. );
    }
}