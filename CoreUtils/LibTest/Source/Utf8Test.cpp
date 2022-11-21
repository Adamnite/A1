/**
 *
 * Copyright (c)2022 The Adamnite C++ Authors.
 *
 * This code is open-sourced under the MIT license.
 */

#include <CoreUtils/Utf8.hpp>

#include <gtest/gtest.h>

TEST( Utf8Test, isUtf8 )
{
    using namespace A1::Utils;

    EXPECT_TRUE( isUtf8( ""      ) );
    EXPECT_TRUE( isUtf8( "test"  ) );
    EXPECT_TRUE( isUtf8( "🙂🙂🙂" ) );

    EXPECT_FALSE( isUtf8( "a\xc5z"   ) );
    EXPECT_FALSE( isUtf8( "\xc3\x28" ) );
    EXPECT_FALSE( isUtf8( "\xa0\xa1" ) );
}