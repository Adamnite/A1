/**
 *
 * Copyright (c)2022 The Adamnite C++ Authors.
 *
 * This code is open-sourced under the MIT license.
 */

#pragma once

#include <string>

namespace A1
{

struct ErrorInfo
{
    /** Line number where the error has happened. */
    std::size_t lineNumber{ 0U };

    /** Character index in the specific line pointing to the beginning of the error. */
    std::size_t charIndex{ 0U };
};

} // namespace A1::Compiler