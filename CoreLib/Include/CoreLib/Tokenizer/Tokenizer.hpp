/**
 *
 * Copyright (c)2022 The Adamnite C++ Authors.
 *
 * This code is open-sourced under the MIT license.
 */

#pragma once

#include <CoreLib/Tokenizer/Token.hpp>
#include <CoreLib/Utils/PushBackStream.hpp>

#include <iterator>

namespace A1
{

class TokenIterator
{
public:
    using iterator_category = std::forward_iterator_tag;
    using difference_type   = std::ptrdiff_t;
    using value_type        = Token;
    using pointer           = value_type *;
    using reference         = value_type &;
    using const_reference   = value_type const &;

    TokenIterator( PushBackStream stream )
    : current_{ Eof{}, 0U, 0U       }
    , stream_ { std::move( stream ) }
    {
        ++*this;
    }

    TokenIterator & operator++();
    TokenIterator   operator++( int );

    [[ nodiscard ]] const_reference operator* () const { return  current_; }
    [[ nodiscard ]] reference       operator* ()       { return  current_; }
    [[ nodiscard ]] pointer         operator->()       { return &current_; }

private:
    value_type     current_;
    PushBackStream stream_;
};

} // namespace A1