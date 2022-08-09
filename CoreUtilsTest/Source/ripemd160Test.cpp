/**
 *
 * Copyright (c)2022 The Adamnite C++ Authors.
 *
 * This code is open-sourced under the MIT license.
 */

// #include <CoreUtils/Source/ripemd160.h>
#include "../../CoreUtils/Source/ripemd-160.h"
// #include "TestUtils.hpp"

#include <gtest/gtest.h>

TEST(ripemd160Test, testEmptyStrings)
{
    char empty[] = "/Users/jonahwilmsmeyer/PycharmProjects/A1/CoreUtilsTest/Source/empty.txt";
    EXPECT_EQ(ripemd160_file(empty), "9c1185a5c5e9fc54612808977ee8f548b2258d31");
    EXPECT_EQ(ripemd160_str(""), "9c1185a5c5e9fc54612808977ee8f548b2258d31");
}
TEST(ripemd160Test, testKnownPairs)
{   
    // testing against https://homes.esat.kuleuven.be/~bosselae/ripemd160.html
    cout << ripemd160_str("abc") << endl;
    EXPECT_EQ(ripemd160_str("abc"), "8eb208f7e05d987a9b044a8e98c6b087f15a0bfc");

}