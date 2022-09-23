/**
 *
 * Copyright (c)2022 The Adamnite C++ Authors.
 *
 * This code is open-sourced under the MIT license.
 */

#pragma once

#include <string_view>

namespace A1::Compiler
{

struct Settings
{
    /** Name of the compiled executable file. */
    std::string_view executableFilename;
};

} // namespace A1::Compiler