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
    PushBackStream( std::string_view const data ) : data_{ data } {}

    /**
     * Constructs the stream by reading the characters from the specific file.
     */
    PushBackStream( std::FILE * f )
    : dataFile_     { f }
    , dataFileIndex_{ std::ftell( f ) }
    {}

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
    std::size_t lineNumber_{ 0U };
    std::size_t charIndex_ { 0U };

    // in case stream is constructed from a string
    std::string_view data_;
    std::size_t      dataIndex_{ 0U };

    // in case stream is constructed from a file
    std::FILE * dataFile_     { nullptr };
    long        dataFileIndex_{ 0 };

    std::stack< int > stack_;
};

} // namespace A1