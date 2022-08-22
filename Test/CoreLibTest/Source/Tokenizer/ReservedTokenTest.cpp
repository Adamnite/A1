/**
 *
 * Copyright (c)2022 The Adamnite C++ Authors.
 *
 * This code is open-sourced under the MIT license.
 */

#include <CoreLib/Tokenizer/ReservedToken.hpp>
#include <CoreLib/Utils/PushBackStream.hpp>

#include "TestUtils.hpp"

#include <gtest/gtest.h>

TEST( ReservedTokenTest, keywords )
{
    EXPECT_EQ( A1::getKeyword( "and"      ), A1::ReservedToken::KwAnd      );
    EXPECT_EQ( A1::getKeyword( "del"      ), A1::ReservedToken::KwDel      );
    EXPECT_EQ( A1::getKeyword( "yield"    ), A1::ReservedToken::KwYield    );
    EXPECT_EQ( A1::getKeyword( "contract" ), A1::ReservedToken::KwContract );

    EXPECT_EQ( A1::getKeyword( "=" ), A1::ReservedToken::Unknown );
    EXPECT_EQ( A1::getKeyword( "&" ), A1::ReservedToken::Unknown );
}

TEST( ReservedTokenTest, operators )
{
    {
        A1::PushBackStream stream{ "<<=" };
        EXPECT_PRED_FORMAT2( A1::areEqual, A1::getOperator( stream ), A1::ReservedToken::OpAssignBitwiseLeftShift );
    }
    {
        A1::PushBackStream stream{ "/" };
        EXPECT_PRED_FORMAT2( A1::areEqual, A1::getOperator( stream ), A1::ReservedToken::OpDiv );
    }
    {
        A1::PushBackStream stream{ "<" };
        EXPECT_PRED_FORMAT2( A1::areEqual, A1::getOperator( stream ), A1::ReservedToken::OpLessThan );
    }
    {
        A1::PushBackStream stream{ "/=" };
        EXPECT_PRED_FORMAT2( A1::areEqual, A1::getOperator( stream ), A1::ReservedToken::OpAssignDiv );
    }
    {
        A1::PushBackStream stream{ "&" };
        EXPECT_PRED_FORMAT2( A1::areEqual, A1::getOperator( stream ), A1::ReservedToken::OpBitwiseAnd );
    }
    {
        A1::PushBackStream stream{ "&&" };
        EXPECT_PRED_FORMAT2( A1::areEqual, A1::getOperator( stream ), A1::ReservedToken::OpLogicalAnd );
    }
    {
        A1::PushBackStream stream{ "random" };
        EXPECT_PRED_FORMAT2( A1::areEqual, A1::getOperator( stream ), A1::ReservedToken::Unknown );
    }
}