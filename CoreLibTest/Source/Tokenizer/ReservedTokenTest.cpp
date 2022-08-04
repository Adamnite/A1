/**
 *
 * Copyright (c)2022 The Adamnite C++ Authors.
 *
 * This code is open-sourced under the MIT license.
 */

#include <CoreLib/Tokenizer/ReservedToken.hpp>
#include <CoreLib/Utils/PushBackStream.hpp>

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

TEST( ReservedTokenTest, keywordTest )
{
    EXPECT_EQ( A1::getKeyword( "and"   ), A1::ReservedToken::KwAnd   );
    EXPECT_EQ( A1::getKeyword( "del"   ), A1::ReservedToken::KwDel   );
    EXPECT_EQ( A1::getKeyword( "yield" ), A1::ReservedToken::KwYield );

    EXPECT_EQ( A1::getKeyword( "=" ), A1::ReservedToken::Unknown );
    EXPECT_EQ( A1::getKeyword( "&" ), A1::ReservedToken::Unknown );
}

TEST( ReservedTokenTest, operatorTest )
{
    {
        A1::PushBackStream stream{ "<<=" };
        EXPECT_PRED_FORMAT2( areEqual, A1::getOperator( stream ), A1::ReservedToken::OpLeftShiftAssign );
    }
    {
        A1::PushBackStream stream{ "/" };
        EXPECT_PRED_FORMAT2( areEqual, A1::getOperator( stream ), A1::ReservedToken::OpDiv );
    }
    {
        A1::PushBackStream stream{ "<" };
        EXPECT_PRED_FORMAT2( areEqual, A1::getOperator( stream ), A1::ReservedToken::OpLessThan );
    }
    {
        A1::PushBackStream stream{ "/=" };
        EXPECT_PRED_FORMAT2( areEqual, A1::getOperator( stream ), A1::ReservedToken::OpDivAssign );
    }
    {
        A1::PushBackStream stream{ "random" };
        EXPECT_PRED_FORMAT2( areEqual, A1::getOperator( stream ), A1::ReservedToken::Unknown );
    }
}