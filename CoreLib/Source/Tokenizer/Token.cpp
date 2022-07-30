/**
 *
 * Copyright (c)2022 The Adamnite C++ Authors.
 *
 * This code is open-sourced under the MIT license.
 */

#include <CoreLib/Tokenizer/Token.hpp>

namespace A1
{

namespace
{
    template< typename ... Ts > struct Overload : Ts... { using Ts::operator()...; };
    template< typename ... Ts > Overload( Ts ... ) -> Overload< Ts ... >;
} // namespace

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