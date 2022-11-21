/**
 *
 * Copyright (c)2022 The Adamnite C++ Authors.
 *
 * This code is open-sourced under the MIT license.
 */

#pragma once

#include <CoreLib/Compiler/Settings.hpp>

#include <filesystem>

namespace A1
{

[[ nodiscard ]] bool load( Compiler::Settings settings, std::filesystem::path const inputFile );

} // namespace A1