/**
 *
 * Copyright (c)2022 The Adamnite C++ Authors.
 *
 * This code is open-sourced under the MIT license.
 */

#pragma once

#include <string_view>

namespace A1::Utils
{

[[ nodiscard ]] bool isUtf8( std::string_view const data ) noexcept;

} // namespace A1::Utils
