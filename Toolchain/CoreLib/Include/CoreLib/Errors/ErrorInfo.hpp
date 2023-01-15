/**
 *
 * Copyright (c)2022 The Adamnite C++ Authors.
 *
 * This code is open-sourced under the MIT license.
 */

#pragma once

#include <cstddef>

namespace A1
{

struct ErrorInfo
{
    /** Line number where the error has happened. */
    std::size_t lineNumber{ 1U };

    /** Column number pointing to the beginning of the error. */
    std::size_t columnNumber{ 1U };

    /** Columns count of the line previous to the one being processed currently. */
    std::size_t prevLineColumnsCount{ 0U };
};

} // namespace A1