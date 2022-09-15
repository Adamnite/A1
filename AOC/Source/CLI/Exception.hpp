/**
 *
 * Copyright (c)2022 The Adamnite C++ Authors.
 *
 * This code is open-sourced under the MIT license.
 */

#pragma once

#include <stdexcept>

namespace A1::CLI
{

struct Exception : std::exception
{
    explicit Exception( std::string message, std::string help )
    : message_{ std::move( message ) }
    , help_   { std::move( help    ) }
    {}

    [[ nodiscard ]] char const * what() const noexcept
    {
        return message_.c_str();
    }

    [[ nodiscard ]] char const * help() const noexcept
    {
        return help_.c_str();
    }

private:
    std::string message_;
    std::string help_;
};

} // namespace A1::CLI
