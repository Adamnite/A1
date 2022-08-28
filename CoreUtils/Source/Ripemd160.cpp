/**
 *
 * Copyright (c)2022 The Adamnite C++ Authors.
 *
 * This code is open-sourced under the MIT license.
 */

#include <CoreUtils/Ripemd160.hpp>

#include <sstream>

#include <array>

namespace A1::Utils::Ripemd160
{

namespace
{
    [[ nodiscard ]]
    constexpr std::uint32_t shiftLeft( std::uint32_t const x, std::size_t const n ) noexcept
    {
        return ( x << n ) | ( x >> ( 32 - n ) );
    }

    static constexpr auto roundsCount{ 80 };

    [[ nodiscard ]]
    constexpr std::size_t getWordIndex( std::size_t const roundIndex ) noexcept
    {
        constexpr std::array< std::size_t, roundsCount > wordIndicesPerRound
        {
            0U, 1U, 2U, 3U, 4U, 5U, 6U, 7U, 8U, 9U, 10U, 11U, 12U, 13U, 14U, 15U, // 1st group of rounds
            7U, 4U, 13U, 1U, 10U, 6U, 15U, 3U, 12U, 0U, 9U, 5U, 2U, 14U, 11U, 8U, // 2nd group of rounds
            3U, 10U, 14U, 4U, 9U, 15U, 8U, 1U, 2U, 7U, 0U, 6U, 13U, 11U, 5U, 12U, // 3rd group of rounds
            1U, 9U, 11U, 10U, 0U, 8U, 12U, 4U, 13U, 3U, 7U, 15U, 14U, 5U, 6U, 2U, // 4th group of rounds
            4U, 0U, 5U, 9U, 7U, 12U, 2U, 10U, 14U, 1U, 3U, 8U, 11U, 6U, 15U, 13U  // 5th group of rounds
        };

        return wordIndicesPerRound[ roundIndex % roundsCount ];
    }

    [[ nodiscard ]]
    constexpr std::size_t getWordIndexPrime( std::size_t const roundIndex ) noexcept
    {
        constexpr std::array< std::size_t, roundsCount > wordIndicesPerRound
        {
            5U, 14U, 7U, 0U, 9U, 2U, 11U, 4U, 13U, 6U, 15U, 8U, 1U, 10U, 3U, 12U, // 1st group of rounds
            6U, 11U, 3U, 7U, 0U, 13U, 5U, 10U, 14U, 15U, 8U, 12U, 4U, 9U, 1U, 2U, // 2nd group of rounds
            15U, 5U, 1U, 3U, 7U, 14U, 6U, 9U, 11U, 8U, 12U, 2U, 10U, 0U, 4U, 13U, // 3rd group of rounds
            8U, 6U, 4U, 1U, 3U, 11U, 15U, 0U, 5U, 12U, 2U, 13U, 9U, 7U, 10U, 14U, // 4th group of rounds
            12U, 15U, 10U, 4U, 1U, 5U, 8U, 7U, 6U, 2U, 13U, 14U, 0U, 3U, 9U, 11U  // 5th group of rounds
        };

        return wordIndicesPerRound[ roundIndex % roundsCount ];
    }

    [[ nodiscard ]]
    constexpr std::size_t getShiftCount( std::size_t const roundIndex ) noexcept
    {
        constexpr std::array< std::size_t, roundsCount > shiftCountsPerRound
        {
            11U, 14U, 15U, 12U, 5U, 8U, 7U, 9U, 11U, 13U, 14U, 15U, 6U, 7U, 9U, 8U, // 1st group of rounds
            7U, 6U, 8U, 13U, 11U, 9U, 7U, 15U, 7U, 12U, 15U, 9U, 11U, 7U, 13U, 12U, // 2nd group of rounds
            11U, 13U, 6U, 7U, 14U, 9U, 13U, 15U, 14U, 8U, 13U, 6U, 5U, 12U, 7U, 5U, // 3rd group of rounds
            11U, 12U, 14U, 15U, 14U, 15U, 9U, 8U, 9U, 14U, 5U, 6U, 8U, 6U, 5U, 12U, // 4th group of rounds
            9U, 15U, 5U, 11U, 6U, 8U, 13U, 12U, 5U, 12U, 13U, 14U, 11U, 8U, 5U, 6U  // 5th group of rounds
        };

        return shiftCountsPerRound[ roundIndex % roundsCount ];
    }

    [[ nodiscard ]]
    constexpr std::size_t getShiftCountPrime( std::size_t const roundIndex ) noexcept
    {
        constexpr std::array< std::size_t, roundsCount > shiftCountsPerRound
        {
            8U, 9U, 9U, 11U, 13U, 15U, 15U, 5U, 7U, 7U, 8U, 11U, 14U, 14U, 12U, 6U, // 1st group of rounds
            9U, 13U, 15U, 7U, 12U, 8U, 9U, 11U, 7U, 7U, 12U, 7U, 6U, 15U, 13U, 11U, // 2nd group of rounds
            9U, 7U, 15U, 11U, 8U, 6U, 6U, 14U, 12U, 13U, 5U, 14U, 13U, 13U, 7U, 5U, // 3rd group of rounds
            15U, 5U, 8U, 11U, 14U, 14U, 6U, 14U, 6U, 9U, 12U, 9U, 12U, 5U, 15U, 8U, // 4th group of rounds
            8U, 5U, 12U, 9U, 12U, 5U, 14U, 6U, 8U, 13U, 6U, 5U, 15U, 13U, 11U, 11U  // 5th group of rounds
        };

        return shiftCountsPerRound[ roundIndex % roundsCount ];
    }

    [[ nodiscard ]]
    constexpr std::uint32_t func
    (
        std::size_t   const roundIndex,
        std::uint32_t const x,
        std::uint32_t const y,
        std::uint32_t const z
    )
    {
        if ( roundIndex >= 64U ) { return x ^ ( y | ~z );         } // 5th group of rounds
        if ( roundIndex >= 48U ) { return ( x & z ) | ( y & ~z ); } // 4th group of rounds
        if ( roundIndex >= 32U ) { return ( x | ~y ) ^ z;         } // 3rd group of rounds
        if ( roundIndex >= 16U ) { return ( x & y ) | ( ~x & z ); } // 2nd group of rounds
        if ( roundIndex >= 0U  ) { return x ^ y ^ z;              } // 1st group of rounds

        return 0U;
    }

    [[ nodiscard ]]
    constexpr std::uint32_t getConstant( std::size_t const roundIndex ) noexcept
    {
        if ( roundIndex >= 64U ) { return 0xa953fd4eUL; } // 5th group of rounds
        if ( roundIndex >= 48U ) { return 0x8f1bbcdcUL; } // 4th group of rounds
        if ( roundIndex >= 32U ) { return 0x6ed9eba1UL; } // 3rd group of rounds
        if ( roundIndex >= 16U ) { return 0x5a827999UL; } // 2nd group of rounds
        if ( roundIndex >= 0U  ) { return 0x00000000UL; } // 1st group of rounds

        return 0U;
    }

    [[ nodiscard ]]
    constexpr std::uint32_t getConstantPrime( std::size_t const roundIndex ) noexcept
    {
        if ( roundIndex >= 64U ) { return 0x00000000UL; } // 5th group of rounds
        if ( roundIndex >= 48U ) { return 0x7a6d76e9UL; } // 4th group of rounds
        if ( roundIndex >= 32U ) { return 0x6d703ef3UL; } // 3rd group of rounds
        if ( roundIndex >= 16U ) { return 0x5c4dd124UL; } // 2nd group of rounds
        if ( roundIndex >= 0U  ) { return 0x50a28be6UL; } // 1st group of rounds

        return 0U;
    }

    [[ nodiscard ]]
    constexpr std::uint32_t invert( std::uint32_t const x ) noexcept
    {
        std::uint32_t result{ 0 };

        result |= ( ( x >>  0 ) & 0xff ) << 24;
        result |= ( ( x >>  8 ) & 0xff ) << 16;
        result |= ( ( x >> 16 ) & 0xff ) << 8;
        result |= ( ( x >> 24 ) & 0xff );

        return result;
    }

    void hashWords
    (
        std::uint32_t const * words,
        std::uint32_t & h0,
        std::uint32_t & h1,
        std::uint32_t & h2,
        std::uint32_t & h3,
        std::uint32_t & h4
    ) noexcept
    {
        std::uint32_t a{ h0 }; std::uint32_t aPrime{ h0 };
        std::uint32_t b{ h1 }; std::uint32_t bPrime{ h1 };
        std::uint32_t c{ h2 }; std::uint32_t cPrime{ h2 };
        std::uint32_t d{ h3 }; std::uint32_t dPrime{ h3 };
        std::uint32_t e{ h4 }; std::uint32_t ePrime{ h4 };

        std::uint32_t tmp{ 0 };

        for ( auto i{ 0U }; i < roundsCount; i++ )
        {
            tmp = shiftLeft
            (
                a + func( i, b, c, d ) + words[ getWordIndex( i ) ] + getConstant( i ),
                getShiftCount( i )
            ) + e;

            a = e;
            e = d;
            d = shiftLeft( c, 10U );
            c = b;
            b = tmp;

            tmp = shiftLeft
            (
                aPrime + func( roundsCount - i - 1, bPrime, cPrime, dPrime ) + words[ getWordIndexPrime( i ) ] + getConstantPrime( i ),
                getShiftCountPrime( i )
            ) + ePrime;

            aPrime = ePrime;
            ePrime = dPrime;
            dPrime = shiftLeft( cPrime, 10U );
            cPrime = bPrime;
            bPrime = tmp;
        }

        tmp = h1 + c + dPrime;
        h1  = h2 + d + ePrime;
        h2  = h3 + e + aPrime;
        h3  = h4 + a + bPrime;
        h4  = h0 + b + cPrime;
        h0  = tmp;
    }
} // namespace

std::string hash( std::string_view const data )
{
    static constexpr auto blockLength   { 64U }; // 512 bits
    static constexpr auto dataLengthSize{  8U }; // 64 bits
    static constexpr auto bitsInByte    {  8U };

    auto const lastBlockLength{ data.size() % blockLength };

    auto const paddingLength
    {
        [ & ]() -> std::size_t
        {
            if ( auto const rem{ ( std::size( data ) + dataLengthSize ) % blockLength }; rem != 0 )
            {
                return blockLength - rem;
            }
            return 0U;
        }()
    };

    auto const blocksCount{ ( std::size( data ) + paddingLength + dataLengthSize ) / blockLength };
    auto const modifiedBlocksCount{ lastBlockLength >= 56U ? 2U : 1U };

    std::uint32_t h0{ 0x67452301UL };
    std::uint32_t h1{ 0xefcdab89UL };
    std::uint32_t h2{ 0x98badcfeUL };
    std::uint32_t h3{ 0x10325476UL };
    std::uint32_t h4{ 0xc3d2e1f0UL };

    std::uint32_t words[ 16 ];

    std::size_t start{ 0U };

    for ( auto i{ 0U }; i < blocksCount - modifiedBlocksCount; i++, start += 64 )
    {
        for ( auto j{ 0U }; j < 16U; j++ )
        {
            words[ j ] = 256 * 256 * 256 * data[ 64 * i + 4 * j + 3 ] +
                               256 * 256 * data[ 64 * i + 4 * j + 2 ] +
                                     256 * data[ 64 * i + 4 * j + 1 ] +
                                           data[ 64 * i + 4 * j ];
        }

        hashWords( words, h0, h1, h2, h3, h4 );
    }

    if ( lastBlockLength < 56 )
    {
        std::uint8_t buffer[ 64 ];

        for ( auto i{ 0U }; i < lastBlockLength; i++ )
        {
            buffer[ i ] = data[ start + i ];
        }

        auto pos{ lastBlockLength };
        buffer[ pos++ ] = 0x80; // '1' bit in padding

        // rest of '0' bits in padding
        for ( auto i{ 0U }; i < paddingLength - 1; i++ ) { buffer[ pos++ ] = 0; }

        auto const dataLengthInBits{ std::size( data ) * bitsInByte };
        for ( auto i{ 0U }; i < bitsInByte; i++ )
        {
            buffer[ pos++ ] = dataLengthInBits >> i * bitsInByte;
        }

        for ( auto i{ 0U }; i < 16U; i++ )
        {
            words[ i ] = 256 * 256 * 256 * buffer[ 4 * i + 3 ] +
                               256 * 256 * buffer[ 4 * i + 2 ] +
                                     256 * buffer[ 4 * i + 1 ] +
                                           buffer[ 4 * i ];
        }

        hashWords(words, h0, h1, h2, h3, h4);
    }
    else
    {
        std::uint8_t buffer[ 128 ];

        for ( auto i{ 0U }; i < lastBlockLength; i++ )
        {
            buffer[ i ] = data[ start + i ];
        }

        auto pos{ lastBlockLength };
        buffer[ pos++ ] = 0x80; // '1' bit in padding

        // rest of '0' bits in padding
        for ( auto i{ 0U }; i < paddingLength - 1; i++ ) { buffer[ pos++ ] = 0; }

        auto const dataLengthInBits{ std::size( data ) * bitsInByte };
        for ( auto i{ 0U }; i < bitsInByte; i++ )
        {
            buffer[ pos++ ] = dataLengthInBits >> i * bitsInByte;
        }

        for ( auto i{ 0U }; i < 16U; i++ )
        {
            words[ i ] = 256 * 256 * 256 * buffer[ 4 * i + 3 ] +
                               256 * 256 * buffer[ 4 * i + 2 ] +
                                     256 * buffer[ 4 * i + 1 ] +
                                           buffer[ 4 * i ];
        }

        hashWords( words, h0, h1, h2, h3, h4 );

        for ( auto i{ 16U }; i < 32U; i++ )
        {
            words[ i - 16 ] = 256 * 256 * 256 * buffer[ 4 * i + 3 ] +
                                    256 * 256 * buffer[ 4 * i + 2 ] +
                                          256 * buffer[ 4 * i + 1 ] +
                                                buffer[ 4 * i ];
        }

        hashWords( words, h0, h1, h2, h3, h4 );
    }

    std::ostringstream ss;

    ss << std::hex
       << invert( h0 )
       << invert( h1 )
       << invert( h2 )
       << invert( h3 )
       << invert( h4 );

    return ss.str();
}

} // namespace A1::Utils::Ripemd160
