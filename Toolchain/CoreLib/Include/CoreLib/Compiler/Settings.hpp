/**
 *
 * Copyright (c)2022 The Adamnite C++ Authors.
 *
 * This code is open-sourced under the MIT license.
 */

#pragma once

#include <string>

namespace A1::Compiler
{

struct Settings
{
    /** Name of the compiled executable file. */
    std::string executableFilename;

    /** Write AST to standard output. */
    bool outputAST{ false };

    /** Write generated LLVM IR code to standard output. */
    bool outputIR{ false };
};

} // namespace A1::Compiler