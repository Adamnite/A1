/**
 *
 * Copyright (c)2022 The Adamnite C++ Authors.
 *
 * This code is open-sourced under the MIT license.
 */

#pragma once

#include <CoreLib/Tokenizer/ReservedToken.hpp>

#include <variant>
#include <string>

namespace A1
{

struct Identifier
{
    std::string name;
};

struct Eof
{};

class Token
{
public:
    using ValueType = std::variant< ReservedToken, Identifier, double, std::string, Eof >;


private:
};

} // namespace A1