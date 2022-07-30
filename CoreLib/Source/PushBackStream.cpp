/**
 *
 * Copyright (c)2022 The Adamnite C++ Authors.
 *
 * This code is open-sourced under the MIT license.
 */

#include <CoreLib/PushBackStream.hpp>

namespace A1
{

void PushBackStream::push( int const c ) noexcept
{
    stack_.push( c );

    if ( c== '\n' ) { --lineNumber_; }

    --charIndex_;
}

std::optional< int > PushBackStream::pop() noexcept
{
    int result{ 0 };

    if ( stack_.empty() )
    {
        if ( dataIndex_ < std::size( data_ ) )
        {
            result = data_[ dataIndex_++ ];
        }
        else
        {
            return {};
        }
    }
    else
    {
        result = stack_.top();
        stack_.pop();
    }

    if ( result == '\n' ) { ++lineNumber_; }

    ++charIndex_;

    return result;
}

} // namespace A1