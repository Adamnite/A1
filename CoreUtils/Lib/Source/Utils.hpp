/**
 *
 * Copyright (c)2022 The Adamnite C++ Authors.
 *
 * This code is open-sourced under the MIT license.
 */

#pragma once

#include <cstdint>
#include <string>
#include <span>

namespace A1::Utils
{

template< typename T >
concept Container = requires( T t )
{
    std::begin( t );
    std::end  ( t );
};

[[ nodiscard ]] std::string toHex( std::uint8_t const byte );

template< Container T >
[[ nodiscard ]] std::string toHex( T && bytes )
{
    using U = typename std::remove_cvref_t< T >::value_type;

    std::string result;
    result.reserve( std::size( bytes ) * sizeof( U ) * 2 );
    for ( auto i{ 0U }; i < std::size( bytes ); ++i )
    {
        for ( auto j{ 0U }; j < sizeof( U ); ++j )
        {
            auto const byte{ bytes[ i ] >> ( 8U * ( sizeof( U ) - 1 - j ) ) };
            result += toHex( static_cast< std::uint8_t >( byte ) );
        }
    }
    return result;
}

} // namespace A1::Utils
