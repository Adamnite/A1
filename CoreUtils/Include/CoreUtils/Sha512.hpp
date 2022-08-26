/**
 *
 * Copyright (c)2022 The Adamnite C++ Authors.
 *
 * This code is open-sourced under the MIT license.
 */

#pragma once

#include <string_view>
#include <string>

namespace A1::Utils::Sha512
{

[[ nodiscard ]] std::string hash( std::string_view const data );

} // namespace A1::Utils::Sha512
