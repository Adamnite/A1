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
        if ( std::holds_alternative< StringInfo >( data_ ) )
        {
            auto & info{ std::get< StringInfo >( data_ ) };
            if ( info.index < std::size( info.value ) )
            {
                // reading data from the character array
                result = info.value[ info.index++ ];
            }
            else
            {
                return {};
            }
        }
        else if ( std::holds_alternative< FileInfo >( data_ ) )
        {
            auto & info{ std::get< FileInfo >( data_ ) };
            if ( info.file != nullptr )
            {
                // reading data from the file
                std::fseek( info.file, info.filePos, SEEK_SET );
                result = std::fgetc( info.file );
                info.filePos = std::ftell( info.file );

                if ( std::feof( info.file ) ) { return {}; }
            }
            else
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