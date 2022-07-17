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

    Token( ValueType value, std::size_t const lineNumber, std::size_t const charIndex )
    : value_     { std::move( value ) }
    , lineNumber_{ lineNumber         }
    , charIndex_ { charIndex          }
    {}

    [[ nodiscard ]] ValueType const & value() const noexcept { return value_; }

    [[ nodiscard ]] std::size_t lineNumber() const noexcept { return lineNumber_; }
    [[ nodiscard ]] std::size_t charIndex () const noexcept { return charIndex_;  }

    [[ nodiscard ]] std::string toString() const noexcept
    {
        return {};
    }

private:
    ValueType   value_;
    std::size_t lineNumber_;
    std::size_t charIndex_;
};

} // namespace A1