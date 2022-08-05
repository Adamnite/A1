/**
 *
 * Copyright (c)2022 The Adamnite C++ Authors.
 *
 * This code is open-sourced under the MIT license.
 */

#include <CoreLib/Tokenizer/Token.hpp>

#include <CoreLib/Utils/Utils.hpp>

namespace A1
{

std::string Token::toString() const noexcept
{
    return std::visit
    (
        Overload
        {
            []( ReservedToken const token )
            {
                return std::string{ toStringView( token ) };
            },
            []( Identifier const & identifier )
            {
                return identifier.name;
            },
            []( double const number )
            {
                return std::to_string( number );
            },
            []( std::string const & str )
            {
                return str;
            },
            []( Eof )
            {
                return std::string{ "<EOF>" };
            }
        },
        value_
    );
}

} // namespace A1