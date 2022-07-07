/**
 *
 * Copyright (c)2022 The Adamnite C++ Authors.
 *
 * This code is open-sourced under the MIT license.
 */

#pragma once

#include <stack>

namespace A1
{

/**
 * class PushBackStream
 *
 * Allows us to return character back to the stream
 * until we are sure about the type of the specific token.
 */
template< typename GetCharClbk >
class PushBackStream
{
public:
    PushBackStream( GetCharClbk && clbk ) noexcept
    : lineNumber_ { 0 }
    , charIndex_  { 0 }
    , getCharClbk_{ std::forward< GetCharClbk >( clbk ) }
    {}

    /** Gets the next character from the stream */
    [[ nodiscard ]] int operator()();

    [[ nodiscard ]] std::size_t lineNumber() const noexcept { return lineNumber_; }
    [[ nodiscard ]] std::size_t charIndex () const noexcept { return charIndex_;  }

    /** Pushes character back to the stream */
    void pushBack( int const c );

private:
    std::size_t lineNumber_{ 0 };
    std::size_t charIndex_ { 0 };

    std::stack< int > stack_;

    GetCharClbk getCharClbk_;
};

template< typename GetCharClbk >
int PushBackStream< GetCharClbk >::operator()()
{
    int result{ -1 };
    if ( stack_.empty() ) {
        result = getCharClbk_();
    } else {
        result = _stack.top();
        _stack.pop();
    }

    if ( result == '\n' ) { ++lineNumber_; }

    ++charIndex_;

    return result;
}

template< typename GetCharClbk >
void PushBackStream< GetCharClbk >::pushBack( int const c )
{
    stack_.push( c );

    if ( c== '\n' ) { --lineNumber_; }

    --charIndex_;
}

} // namespace A1