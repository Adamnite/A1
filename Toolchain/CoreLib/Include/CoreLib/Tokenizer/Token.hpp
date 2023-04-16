/**
 *
 * Copyright (c)2022 The Adamnite C++ Authors.
 *
 * This code is open-sourced under the MIT license.
 */

#pragma once

#include <CoreLib/Tokenizer/ReservedToken.hpp>
#include <CoreLib/Errors/ErrorInfo.hpp>
#include <CoreLib/Utils/Macros.hpp>

#include <variant>
#include <string>

namespace A1
{

struct Identifier
{
    std::string name;

    [[ nodiscard ]] bool operator==( Identifier const & ) const = default;

    [[ nodiscard ]]
    static constexpr auto toString() noexcept { return "identifier"; }
};

struct Number
{
    using Type = std::int64_t;

    Type value{ 0 };

    [[ nodiscard ]] bool operator==( Number const & ) const = default;

    [[ nodiscard ]]
    static constexpr auto toString() noexcept { return "number"; }
};

struct StringLiteral
{
    std::string value;

    [[ nodiscard ]] bool operator==( StringLiteral const & ) const = default;

    [[ nodiscard ]]
    static constexpr auto toString() noexcept { return "string"; }
};

struct Indentation
{
    [[ nodiscard ]] bool operator==( Indentation const & ) const = default;

    [[ nodiscard ]]
    static constexpr auto toString() noexcept { return "indentation"; }
};

struct Newline
{
    [[ nodiscard ]] bool operator==( Newline const & ) const = default;

    [[ nodiscard ]]
    static constexpr auto toString() noexcept { return "newline"; }
};

struct Eof
{
    [[ nodiscard ]] bool operator==( Eof const & ) const = default;

    [[ nodiscard ]]
    static constexpr auto toString() noexcept { return "eof"; }
};

class Token
{
public:
    using ValueType = std::variant< ReservedToken, Identifier, Number, StringLiteral, Indentation, Newline, Eof >;

    Token() noexcept = default;
    Token( ValueType value, ErrorInfo errorInfo )
    : value_    { std::move( value     ) }
    , errorInfo_{ std::move( errorInfo ) }
    {}

    template< typename T >
    [[ nodiscard ]] bool is() const noexcept
    {
        return std::holds_alternative< T >( value_ );
    }

    template< typename T >
    [[ nodiscard ]] bool is_not() const noexcept
    {
        return !std::holds_alternative< T >( value_ );
    }

    template< typename T >
    [[ nodiscard ]] T const & get() const noexcept
    {
        ASSERT( is< T >() );
        return std::get< T >( value_ );
    }

    [[ nodiscard ]] ValueType const & value    () const noexcept { return value_;     }
    [[ nodiscard ]] ErrorInfo         errorInfo() const noexcept { return errorInfo_; }

    [[ nodiscard ]] std::string toString() const noexcept;

private:
    ValueType value_;
    ErrorInfo errorInfo_;
};

} // namespace A1