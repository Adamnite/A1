/**
 *
 * Copyright (c)2022 The Adamnite C++ Authors.
 *
 * This code is open-sourced under the MIT license.
 */

#pragma once

#include <string_view>
#include <optional>
#include <stack>

namespace A1
{

/**
 * class PushBackStream
 *
 * Allows us to return character back to the stream
 * until we are sure about the type of the specific token.
 *
 * CAUTION: This class does not own the data in the stream.
 */
class PushBackStream
{
public:
    PushBackStream( std::string_view const strv )
    : lineNumber_{ 0U   }
    , charIndex_ { 0U   }
    , data_      { strv }
    {}

    /** Pushes character back to the stream */
    void push( int const c ) noexcept;

    /** Gets the next character from the stream */
    [[ nodiscard ]] std::optional< int > pop() noexcept;

    [[ nodiscard ]] std::size_t lineNumber() const noexcept { return lineNumber_; }
    [[ nodiscard ]] std::size_t charIndex () const noexcept { return charIndex_;  }

private:
    std::size_t lineNumber_{ 0U };
    std::size_t charIndex_ { 0U };

    std::string_view data_;
    std::size_t      dataIndex_{ 0U };

    std::stack< int > stack_;
};

} // namespace A1