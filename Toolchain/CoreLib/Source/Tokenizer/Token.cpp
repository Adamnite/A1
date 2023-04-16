/**
 *
 * Copyright (c)2022 The Adamnite C++ Authors.
 *
 * This code is open-sourced under the MIT license.
 */

#include <CoreLib/Tokenizer/Token.hpp>

#include "Utils/Utils.hpp"

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
            []( Number const number )
            {
                return std::to_string( number.value );
            },
            []( StringLiteral const & str )
            {
                return str.value;
            },
            []( auto const token ) -> std::string
            {
                return std::decay_t< decltype( token ) >::toString();
            }
        },
        value_
    );
}

} // namespace A1