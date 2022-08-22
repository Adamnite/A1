#include "CoreUtils/utf8.h"

#include <gtest/gtest.h>
TEST(utf8Test, testASCIIIsUTF8)
{
    std::string input = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
    size_t position;
    EXPECT_TRUE(CoreUtils::isUTFencoding(input, position));
}