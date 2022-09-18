/**
 *
 * Copyright (c)2022 The Adamnite C++ Authors.
 *
 * This code is open-sourced under the MIT license.
 */

#pragma once

#include <CoreLib/Tokenizer/ReservedToken.hpp>
#include <CoreLib/Utils/Macros.hpp>

#include <variant>
#include <string>

namespace A1
{

struct Identifier
{
    std::string name;

    [[ nodiscard ]] bool operator==( Identifier const & ) const = default;
};

struct Newline
{
    [[ nodiscard ]] bool operator==( Newline const & ) const = default;
};

struct Eof
{
    [[ nodiscard ]] bool operator==( Eof const & ) const = default;
};

using Number = double;
using String = std::string;

class Token
{
public:
    using ValueType = std::variant< ReservedToken, Identifier, Number, String, Newline, Eof >;

    Token() noexcept = default;
    Token( ValueType value, std::size_t const lineNumber, std::size_t const charIndex )
    : value_     { std::move( value ) }
    , lineNumber_{ lineNumber         }
    , charIndex_ { charIndex          }
    {}

    template< typename T >
    [[ nodiscard ]] bool is() const noexcept
    {
        return std::holds_alternative< T >( value_ );
    }

    template< typename T >
    [[ nodiscard ]] T const & get() const noexcept
    {
        ASSERT( is< T >() );
        return std::get< T >( value_ );
    }

    [[ nodiscard ]] ValueType const & value() const noexcept { return value_; }

    [[ nodiscard ]] std::size_t lineNumber() const noexcept { return lineNumber_; }
    [[ nodiscard ]] std::size_t charIndex () const noexcept { return charIndex_;  }

    [[ nodiscard ]] std::string toString() const noexcept;

private:
    ValueType   value_;
    std::size_t lineNumber_;
    std::size_t charIndex_;
};

} // namespace A1