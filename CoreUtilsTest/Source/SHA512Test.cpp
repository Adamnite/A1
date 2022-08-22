/**
 *
 * Copyright (c)2022 The Adamnite C++ Authors.
 *
 * This code is open-sourced under the MIT license.
 */


#include "CoreUtils/Sha512.hpp"
// #include "TestUtils.hpp"

#include <gtest/gtest.h>
SHA512 sha512 = SHA512();
TEST(SHA512Test, testAgainstLargeKnownValues)
{
    // https://emn178.github.io/online-tools/sha512.html
    // this sites used to get known goods
    EXPECT_EQ(sha512.hash("abckjndnsfgljadsfhgknskjdlah;sda gh ;sdhagjk leg bjhklafdhgl lalkjsgligera"), "08d24a15fc1bc9d0e79381942b085d7288252ad735815e888619be0cfaf0cf2936ad3286d8dc5da936f0a594116f07a4296d569682d3dc807af4f2dc7ca6cbe6");
}
TEST(SHA512Test, againstShortKnownValues){
    // https://emn178.github.io/online-tools/sha512.html
    // this sites used to get known goods

    // EXPECT_EQ(sha512.hash("A123456789101112"),"a2e2dfddc0dc16cb395ed13736efd351fe601a2097510238dc6a1a4b41d2c21f6ce049e03767637bb88f89aa1603778c493246b2746765a64ad8a5dfb1d54106");
    // EXPECT_EQ(sha512.hash("A12345678910111213"),"efe76722f2111344b98fbb3b7bd5d82dfc35aa4aa4f8d8a2e21fe22ff70dae330b0774ff17f4af33ed6527091d490ac40daf8cce3d1ab12b250e9971b1d97c38");
    // EXPECT_EQ(sha512.hash("A1234567891011121314"),"efe76722f2111344b98fbb3b7bd5d82dfc35aa4aa4f8d8a2e21fe22ff70dae330b0774ff17f4af33ed6527091d490ac40daf8cce3d1ab12b250e9971b1d97c38");
    // EXPECT_EQ(sha512.hash("A123456789101112131415"),"efe76722f2111344b98fbb3b7bd5d82dfc35aa4aa4f8d8a2e21fe22ff70dae330b0774ff17f4af33ed6527091d490ac40daf8cce3d1ab12b250e9971b1d97c38");
    // EXPECT_EQ(sha512.hash("abc"),"ddaf35a193617abacc417349ae20413112e6fa4e89a97ea20a9eeee64b55d39a2192992a274fc1a836ba3c23a3feebbd454d4423643ce80e2a9ac94fa54ca49f");
    // EXPECT_EQ(sha512.hash(""), "cf83e1357eefb8bdf1542850d66d8007d620e4050b5715dc83f4a921d36ce9ce47d0d13c5d85f2b0ff8318d2877eec2f63b931bd47417a81a538327af927da3e");
}