/**
 *
 * Copyright (c)2022 The Adamnite C++ Authors.
 *
 * This code is open-sourced under the MIT license.
 */


#include <CoreUtils/Ripemd160.hpp>

#include <gtest/gtest.h>

TEST( Ripemd160Test, hash )
{
    using namespace A1::Utils;

    EXPECT_EQ( Ripemd160::hash( ""             ), "9c1185a5c5e9fc54612808977ee8f548b2258d31" );
    EXPECT_EQ( Ripemd160::hash( "test"         ), "5e52fee47e6b070565f74372468cdc699de89107" );
    EXPECT_EQ( Ripemd160::hash( "testtesttest" ), "82c874ba4012692fc1fa868aa4a3d217fae3f780" );

    EXPECT_EQ
    (
        Ripemd160::hash
        (
            "testtesttesttesttesttesttesttesttesttest"
            "testtesttesttesttesttesttesttesttesttest"
        ),
        "6ee547c5199889bb2f54bee6572e652c95c83183"
    );

    // EXPECT_EQ
    // (
    //     Ripemd160::hash
    //     (
    //         "testtesttesttesttesttesttesttesttesttest"
    //         "testtesttesttesttesttesttesttesttesttest"
    //         "testtesttesttesttesttesttesttesttesttest"
    //         "testtesttesttesttesttesttesttesttesttest"
    //     ),
    //     "6ee547c5199889bb2f54bee6572e652c95c83183"
    // );
}