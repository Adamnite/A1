/**
 *
 * Copyright (c)2022 The Adamnite C++ Authors.
 *
 * This code is open-sourced under the MIT license.
 */


#include <CoreUtils/Sha512.hpp>

#include <gtest/gtest.h>

TEST( Sha512Test, hash )
{
    using namespace A1::Utils;

    EXPECT_EQ
    (
        Sha512::hash( "" ),
        "cf83e1357eefb8bdf1542850d66d8007d620e4050b5"
        "715dc83f4a921d36ce9ce47d0d13c5d85f2b0ff8318"
        "d2877eec2f63b931bd47417a81a538327af927da3e"
    );

    EXPECT_EQ
    (
        Sha512::hash( "test" ),
        "ee26b0dd4af7e749aa1a8ee3c10ae9923f618980772"
        "e473f8819a5d4940e0db27ac185f8a0e1d5f84f88bc"
        "887fd67b143732c304cc5fa9ad8e6f57f50028a8ff"
    );

    EXPECT_EQ
    (
        Sha512::hash( "testtesttest" ),
        "623c92d8c3e80a6963599e42aa37d43f8f4f4e84c74"
        "2bfe5cf26b33b6e5a281599dd9e948691b5f76566e5"
        "26375ef46cc5485af55bac2a198b69b40333ac92fb"
    );

    EXPECT_EQ
    (
        Sha512::hash
        (
            "testtesttesttesttesttesttesttesttesttest"
            "testtesttesttesttesttesttesttesttesttest"
        ),
        "4785d36bea718ced58e2d3e2c3c1340725b8d5bc9c0"
        "61abba6a47e3eade1e7701fe0ce2b3f2c640f58f6b9"
        "7ccb85f2b00c31b42a84112127d750c5e0e80b6d37"
    );

    // EXPECT_EQ
    // (
    //     Sha512::hash
    //     (
    //         "testtesttesttesttesttesttesttesttesttest"
    //         "testtesttesttesttesttesttesttesttesttest"
    //         "testtesttesttesttesttesttesttesttesttest"
    //         "testtesttesttesttesttesttesttesttesttest"
    //     ),
    //     "039f74360411cd1c028f1f761e2ced9e543457d8009"
    //     "0e0dca2579405805451312d09b5a77de4a2bb6b86a7"
    //     "31d20468fa48500c36e5a757c52e830bf2f5434998"
    // );
}
