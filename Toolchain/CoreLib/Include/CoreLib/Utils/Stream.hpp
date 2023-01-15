/**
 *
 * Copyright (c)2022 The Adamnite C++ Authors.
 *
 * This code is open-sourced under the MIT license.
 */

#pragma once

#include <CoreLib/Errors/ErrorInfo.hpp>

#include <string_view>
#include <optional>
#include <variant>
#include <cstdio>
#include <stack>

namespace A1
{

/**
 * class Stream
 *
 * Allows returning character back to the stream until it is
 * certain what is the type of the specific token.
 *
 * CAUTION: Class does not own the data in the stream.
 */
class Stream
{
public:
    /**
     * Constructs the stream from the character array.
     */
    Stream( std::string_view const data )
    : data_{ StringInfo { .value = data, .index = 0U } }
    {}

    /**
     * Constructs the stream by reading the characters from the specific file.
     */
    Stream( std::FILE * f )
    : data_{ FileInfo { .file = f, .filePos = std::ftell( f ) } }
    {}

    /**
     * Pushes character back to the stream
     */
    void push( int const c ) noexcept;

    /**
     * Gets the next character from the stream
     */
    [[ nodiscard ]] std::optional< int > pop() noexcept;

    [[ nodiscard ]] ErrorInfo errorInfo() const noexcept { return errorInfo_; }

private:
    struct FileInfo
    {
        std::FILE * file{ nullptr };
        long        filePos{ 0 };
    };

    struct StringInfo
    {
        std::string_view value;
        std::size_t      index{ 0U };
    };

    std::variant< FileInfo, StringInfo > data_;
    std::stack< int >                    stack_;

    ErrorInfo errorInfo_;
};

} // namespace A1