/**
 *
 * Copyright (c)2022 The Adamnite C++ Authors.
 *
 * This code is open-sourced under the MIT license.
 */

#include <CoreUtils/Utf8.hpp>

extern "C"
{
    bool is_utf8( char const * data )
    {
        return A1::Utils::isUtf8( data );
    }
}

namespace A1::Utils
{

bool isUtf8( std::string_view const data ) noexcept
{
	if ( data.empty() ) { return true; }

	auto * bytes{ reinterpret_cast< std::uint8_t const * >( data.data() ) };

    std::uint32_t codePoint{ 0U };
    std::size_t   count    { 0U };

    while ( *bytes != 0x00 )
    {
        if ( ( *bytes & 0x80 ) == 0x00 )
        {
            // from U+0000 to U+007F
            codePoint = ( *bytes & 0x7F );
            count = 1;
        }
        else if ( ( *bytes & 0xE0 ) == 0xC0 )
        {
            // from U+0080 to U+07FF
            codePoint = ( *bytes & 0x1F );
            count = 2;
        }
        else if ( ( *bytes & 0xF0 ) == 0xE0 )
        {
            // from U+0800 to U+FFFF
            codePoint = ( *bytes & 0x0F );
            count = 3;
        }
        else if ( ( *bytes & 0xF8 ) == 0xF0 )
        {
            // from U+10000 to U+10FFFF
            codePoint = ( *bytes & 0x07 );
            count = 4;
        }
        else
		{
            return false;
		}

        bytes++;

        for ( auto i{ 1U }; i < count; ++i )
        {
            if ( ( *bytes & 0xC0 ) != 0x80 ) { return false; }

            codePoint = ( codePoint << 6U ) | ( *bytes & 0x3F );
            bytes += 1;
        }

        if
		(
			codePoint > 0x10FFFF ||
            ( codePoint >= 0xD800  && codePoint <= 0xDFFF ) ||
            ( codePoint <= 0x007F  && count     != 1      ) ||
            ( codePoint >= 0x0080  && codePoint <= 0x07FF   && count != 2 ) ||
            ( codePoint >= 0x0800  && codePoint <= 0xFFFF   && count != 3 ) ||
            ( codePoint >= 0x10000 && codePoint <= 0x1FFFFF && count != 4 )
		)
		{
            return false;
		}
    }

    return true;
}

} // namespace A1::Utils
