/**
 *
 * Copyright (c)2022 The Adamnite C++ Authors.
 *
 * This code is open-sourced under the MIT license.
 */

#include "Utils.hpp"

namespace A1::Utils
{

std::string toHex( std::uint8_t const byte )
{
    constexpr auto hexChars{ "0123456789abcdef" };

    std::string result( 2U, '0' );
    result[ 0U ] = hexChars[ ( byte & 0xf0 ) >> 4 ];
    result[ 1U ] = hexChars[   byte & 0x0f        ];
    return result;
}

} // namespace A1::Utils
