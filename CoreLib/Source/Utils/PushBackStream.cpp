/**
 *
 * Copyright (c)2022 The Adamnite C++ Authors.
 *
 * This code is open-sourced under the MIT license.
 */

#include <CoreLib/Utils/PushBackStream.hpp>

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
            // reading data from the character array
            result = data_[ dataIndex_++ ];
        }
        else if ( dataFile_ != nullptr )
        {
            // reading data from the file
            result = std::fgetc( dataFile_ );
            if ( std::feof( dataFile_ ) )
            {
                return {};
            }
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