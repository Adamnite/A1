/**
 *
 * Copyright (c)2022 The Adamnite C++ Authors.
 *
 * This code is open-sourced under the MIT license.
 */

#pragma once

#include <string_view>
#include <optional>
#include <cstdio>
#include <stack>

namespace A1
{

/**
 * class PushBackStream
 *
 * Allows returning character back to the stream until it is
 * certain what is the type of the specific token.
 *
 * CAUTION: Class does not own the data in the stream.
 */
class PushBackStream
{
public:
    /**
     * Constructs the stream from the character array.
     */
    PushBackStream( std::string_view const data )
    {
        data_ = StringInfo
        {
            .value = data,
            .index = 0U
        };
    }

    /**
     * Constructs the stream by reading the characters from the specific file.
     */
    PushBackStream( std::FILE * f )
    {
        data_ = FileInfo
        {
            .file    = f,
            .filePos = std::ftell( f )
        };
    }

    /**
     * Pushes character back to the stream
     */
    void push( int const c ) noexcept;

    /**
     * Gets the next character from the stream
     */
    [[ nodiscard ]] std::optional< int > pop() noexcept;

    [[ nodiscard ]] std::size_t lineNumber() const noexcept { return lineNumber_; }
    [[ nodiscard ]] std::size_t charIndex () const noexcept { return charIndex_;  }

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
    std::stack< int > stack_;

    std::size_t lineNumber_{ 0U };
    std::size_t charIndex_ { 0U };
};

} // namespace A1