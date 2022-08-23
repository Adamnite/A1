/**
 *
 * Copyright (c)2022 The Adamnite C++ Authors.
 *
 * This code is open-sourced under the MIT license.
 */


#include "CoreUtils/Ripemd160.hpp"
// #include "TestUtils.hpp"
#include <string>
using namespace std;

#include <gtest/gtest.h>
string localDirectory = "<put your relative term here>/A1/CoreUtilsTest/Source/testFiles/";
TEST(ripemd160Test, testFromFiles)
{
    string emptyFile = localDirectory + "empty.txt";
    char empty[emptyFile.length()+1];
    for (unsigned long i = 0; i < emptyFile.length()+1; i++)
    {
        empty[i] = emptyFile[i];
    }
    EXPECT_EQ(ripemd160_file(empty), "9c1185a5c5e9fc54612808977ee8f548b2258d31");
    EXPECT_EQ(ripemd160_str(""), "9c1185a5c5e9fc54612808977ee8f548b2258d31");
    
    string abcFile = localDirectory + "abc.txt";
    char abc[abcFile.length()+1];
    for (unsigned long i = 0; i < abcFile.length()+1; i++)
    {
        abc[i] = abcFile[i];
    }
    EXPECT_EQ(ripemd160_file(abc), "8eb208f7e05d987a9b044a8e98c6b087f15a0bfc");
}
TEST(ripemd160Test, testKnownPairs)
{   
    // testing against https://homes.esat.kuleuven.be/~bosselae/ripemd160.html
    cout << ripemd160_str("abc") << endl;
    EXPECT_EQ(ripemd160_str("abc"), "8eb208f7e05d987a9b044a8e98c6b087f15a0bfc");

}
TEST(ripemd160Test, againstSelf)
{   
    string emptyFile = localDirectory + "empty.txt";
    char empty[emptyFile.length()+1];
    for (unsigned long i = 0; i < emptyFile.length()+1; i++)
    {
        empty[i] = emptyFile[i];
    }
    string abcFile = localDirectory + "abc.txt";
    char abc[abcFile.length()+1];
    for (unsigned long i = 0; i < abcFile.length()+1; i++)
    {
        abc[i] = abcFile[i];
    }
    
    EXPECT_EQ(ripemd160_str(""), ripemd160_file(empty));
    EXPECT_EQ(ripemd160_str("abc"), ripemd160_file(abc));
}
TEST(ripemd160Test, againstLargeSelf){
    string romeoFile = localDirectory + "romeo_juliet.txt";
    char romeo[romeoFile.length()+1];
    for(unsigned long i = 0; i < romeoFile.length()+1; i++){
        romeo[i] = romeoFile[i];
    }

    std::ifstream t(romeo);
    std::stringstream buffer;
    buffer << t.rdbuf();

    EXPECT_EQ(ripemd160_str(buffer.str()), ripemd160_file(romeo));

    string hamletFile = localDirectory + "hamlet.txt";
    char hamlet[hamletFile.length()+1];
    for(unsigned long i = 0; i < hamletFile.length()+1; i++){
        hamlet[i] = hamletFile[i];
    }
    
    std::ifstream f(hamlet);
    std::stringstream buffer1;
    buffer1 << f.rdbuf();

    EXPECT_EQ(ripemd160_str(buffer1.str()), ripemd160_file(hamlet));
}