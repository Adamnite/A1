/**
 *
 * Copyright (c)2022 The Adamnite C++ Authors.
 *
 * This code is open-sourced under the MIT license.
 */

#pragma once

#include <CoreLib/Tokenizer/ReservedToken.hpp>

#include <gtest/gtest.h>

namespace A1
{

[[ nodiscard ]]
testing::AssertionResult areEqual
(
    char const *,
    char const *,
    A1::ReservedToken const value1,
    A1::ReservedToken const value2
);

} // namespace A1