/**
 *
 * Copyright (c)2022 The Adamnite C++ Authors.
 *
 * This code is open-sourced under the MIT license.
 */

#include <CoreUtils/Crypto/Sha512.hpp>

#include "Utils.hpp"

#include <cstring>
#include <string>
#include <vector>
#include <array>

namespace A1::Utils::Sha512
{

namespace
{
    /**
     * Initial Vectors (IV) represent the first 64 bits of the
     * fractional parts of the square roots of the first 8 prime
     * numbers ( 8 * 64 = 512, the size of SHA512 result ).
     */
    static constexpr std::array< std::uint64_t, 8 > initialization
    {
        0x6a09e667f3bcc908ULL,
        0xbb67ae8584caa73bULL,
        0x3c6ef372fe94f82bULL,
        0xa54ff53a5f1d36f1ULL,
        0x510e527fade682d1ULL,
        0x9b05688c2b3e6c1fULL,
        0x1f83d9abfb41bd6bULL,
        0x5be0cd19137e2179ULL
    };

    /**
     * Round constants represent the first 64 bits from the fractional
     * part of the cube roots of the first 80 prime numbers (because
     * there are 80 rounds in hash process).
     */
    static constexpr std::array< std::uint64_t, 80 > roundConstants
    {
        0x428a2f98d728ae22ULL, 0x7137449123ef65cdULL, 0xb5c0fbcfec4d3b2fULL, 0xe9b5dba58189dbbcULL, 0x3956c25bf348b538ULL,
        0x59f111f1b605d019ULL, 0x923f82a4af194f9bULL, 0xab1c5ed5da6d8118ULL, 0xd807aa98a3030242ULL, 0x12835b0145706fbeULL,
        0x243185be4ee4b28cULL, 0x550c7dc3d5ffb4e2ULL, 0x72be5d74f27b896fULL, 0x80deb1fe3b1696b1ULL, 0x9bdc06a725c71235ULL,
        0xc19bf174cf692694ULL, 0xe49b69c19ef14ad2ULL, 0xefbe4786384f25e3ULL, 0x0fc19dc68b8cd5b5ULL, 0x240ca1cc77ac9c65ULL,
        0x2de92c6f592b0275ULL, 0x4a7484aa6ea6e483ULL, 0x5cb0a9dcbd41fbd4ULL, 0x76f988da831153b5ULL, 0x983e5152ee66dfabULL,
        0xa831c66d2db43210ULL, 0xb00327c898fb213fULL, 0xbf597fc7beef0ee4ULL, 0xc6e00bf33da88fc2ULL, 0xd5a79147930aa725ULL,
        0x06ca6351e003826fULL, 0x142929670a0e6e70ULL, 0x27b70a8546d22ffcULL, 0x2e1b21385c26c926ULL, 0x4d2c6dfc5ac42aedULL,
        0x53380d139d95b3dfULL, 0x650a73548baf63deULL, 0x766a0abb3c77b2a8ULL, 0x81c2c92e47edaee6ULL, 0x92722c851482353bULL,
        0xa2bfe8a14cf10364ULL, 0xa81a664bbc423001ULL, 0xc24b8b70d0f89791ULL, 0xc76c51a30654be30ULL, 0xd192e819d6ef5218ULL,
        0xd69906245565a910ULL, 0xf40e35855771202aULL, 0x106aa07032bbd1b8ULL, 0x19a4c116b8d2d0c8ULL, 0x1e376c085141ab53ULL,
        0x2748774cdf8eeb99ULL, 0x34b0bcb5e19b48a8ULL, 0x391c0cb3c5c95a63ULL, 0x4ed8aa4ae3418acbULL, 0x5b9cca4f7763e373ULL,
        0x682e6ff3d6b2b8a3ULL, 0x748f82ee5defb2fcULL, 0x78a5636f43172f60ULL, 0x84c87814a1f0ab72ULL, 0x8cc702081a6439ecULL,
        0x90befffa23631e28ULL, 0xa4506cebde82bde9ULL, 0xbef9a3f7b2c67915ULL, 0xc67178f2e372532bULL, 0xca273eceea26619cULL,
        0xd186b8c721c0c207ULL, 0xeada7dd6cde0eb1eULL, 0xf57d4f7fee6ed178ULL, 0x06f067aa72176fbaULL, 0x0a637dc5a2c898a6ULL,
        0x113f9804bef90daeULL, 0x1b710b35131c471bULL, 0x28db77f523047d84ULL, 0x32caab7b40c72493ULL, 0x3c9ebe0a15c9bebcULL,
        0x431d67c49c100d4cULL, 0x4cc5d4becb3e42b6ULL, 0x597f299cfc657e2aULL, 0x5fcb6fab3ad6faecULL, 0x6c44198c4a475817ULL
    };

    [[ nodiscard ]]
    constexpr std::uint64_t ch( std::uint64_t const x, std::uint64_t const y, std::uint64_t const z ) noexcept
    {
        return ( x & y ) ^ ( ~x & z );
    }

    [[ nodiscard ]]
    constexpr std::uint64_t maj( std::uint64_t const x, std::uint64_t const y, std::uint64_t const z ) noexcept
    {
        return ( x & y ) ^ ( x & z ) ^ ( y & z );
    }

    [[ nodiscard ]]
    constexpr std::uint64_t rotateRight( std::uint64_t const x, std::size_t const n ) noexcept
    {
        return ( x >> n ) | ( x << ( ( sizeof( x ) << 3 ) - n ) );
    }

    [[ nodiscard ]]
    constexpr std::uint64_t sigma0( std::uint64_t const x ) noexcept
    {
        return rotateRight( x, 28 ) ^ rotateRight( x, 34 ) ^ rotateRight( x, 39 );
    }

    [[ nodiscard ]]
    constexpr std::uint64_t sigma1( std::uint64_t const x ) noexcept
    {
        return rotateRight( x, 14 ) ^ rotateRight( x, 18 ) ^ rotateRight( x, 41 );
    }

    [[ nodiscard ]]
    constexpr std::uint64_t sig0( std::uint64_t const x ) noexcept
    {
        return rotateRight( x, 1 ) ^ rotateRight( x, 8 ) ^ ( x >> 7 );
    }

    [[ nodiscard ]]
    constexpr std::uint64_t sig1( std::uint64_t const x ) noexcept
    {
        return rotateRight( x, 19 ) ^ rotateRight( x, 61 ) ^ ( x >> 6 );
    }

    static constexpr auto bitsInByte       {    8U };
    static constexpr auto hashBufferLength {    8U }; // 512 / ( sizeof( std::uint64_t ) * bitsInByte )
    static constexpr auto sequenceLength   {   16U };
    static constexpr auto singleBlockLength{ 1024U };

    [[ nodiscard ]]
    std::vector< std::uint64_t > preprocess( std::string_view const data )
    {
        /**
         * Result of preprocessing should be as follows:
         *
         * +--------------------------------+-----------------+----------------+
         * |              DATA              |     PADDING     |    DATA SIZE   |
         * |                                |                 |                |
         * |      Length < 2 ^ 129 - 2      |   e.g. 1000...  |  Length = 128  |
         * |                                |                 |                |
         * |                                |    Length > 1   |                |
         * +--------------------------------+-----------------+----------------+
         * |                      Total length = N * 1024                      |
         * +-------------------------------------------------------------------+
         */

        std::vector< std::uint64_t > result;

        static constexpr auto minPaddingLength{   1U }; // for a mandatory '1' bit in padding
        static constexpr auto dataLengthSize  { 128U };

        auto const dataLength     { std::size( data ) * bitsInByte };
        auto const minResultLength{ dataLength + minPaddingLength + dataLengthSize };

        // for the rest of '0' bits in padding
        auto const restOfPaddingLength
        {
            [ & ]() -> std::size_t
            {
                if ( auto const rem{ minResultLength % singleBlockLength }; rem != 0 )
                {
                    return singleBlockLength - rem;
                }
                return 0U;
            }()
        };

        auto const resultLength{ minResultLength + restOfPaddingLength };
        result.resize( resultLength / ( sizeof( std::uint64_t ) * bitsInByte ) );

        auto const blocksCount{ resultLength / singleBlockLength };

        for ( auto i{ 0U }, dataIndex{ 0U }; i < blocksCount; i++ )
        {
            for ( auto j{ 0U }; j < sequenceLength; j++ )
            {
                std::uint64_t input{ 0 };

                for ( auto k{ 0U }; k < bitsInByte; k++ )
                {
                    dataIndex = ( i * sequenceLength * bitsInByte ) + ( j * bitsInByte ) + k;

                    if ( dataIndex < std::size( data ) )
                    {
                        // write data byte per byte
                        input = input << bitsInByte | static_cast< std::uint64_t >( data[ dataIndex ] );
                    }
                    else if ( dataIndex == std::size( data ) )
                    {
                        // write mandatory '1' padding bit
                        input = input << bitsInByte | 0x80ULL;
                    }
                    else
                    {
                        // write '0' padding bits
                        input = input << bitsInByte;
                    }
                }
                result[ ( i * sequenceLength ) + j ] = input;
            }
        }

        // write the data length to the last 128 bits
        result[ ( ( blocksCount - 1 ) * sequenceLength ) + sequenceLength - 1 ] = dataLength;
        result[ ( ( blocksCount - 1 ) * sequenceLength ) + sequenceLength - 2 ] = 0;

        return result;
    }

    [[ nodiscard ]]
    std::vector< std::uint64_t > process( std::vector< std::uint64_t > buffer )
    {
        std::vector< std::uint64_t > result;
        result.resize( hashBufferLength );

        static constexpr auto roundsCount{ 80 };
        auto const blocksCount{ ( std::size( buffer ) * sizeof( std::uint64_t ) * bitsInByte ) / singleBlockLength };

        std::array< std::uint64_t, hashBufferLength > previousRoundOutput;
        std::array< std::uint64_t, roundsCount      > words;

        std::memcpy( result.data(), initialization.data(), hashBufferLength * sizeof( std::uint64_t ) );

        for ( auto i{ 0U }; i < blocksCount; i++ )
        {
            std::memcpy( words.data(), buffer.data() + ( sequenceLength * i ), sequenceLength * sizeof( std::uint64_t ) );

            // prepare the words
            for ( auto j{ sequenceLength }; j < roundsCount; j++ )
            {
                words[ j ] = words[ j - 16 ] + sig0( words[ j - 15 ] ) + words[ j - 7 ] + sig1( words[ j - 2 ] );
            }

            /**
             * Previous round output is the final output from the processing
             * phase for the previous block of 1024 bits. For the first round
             * of the first block of 1024 bits the Initializatio Vector is used.
             */
            std::memcpy( previousRoundOutput.data(), result.data(), hashBufferLength * sizeof( std::uint64_t ) );

            // compression
            for ( auto j{ 0U }; j < roundsCount; j++ )
            {
                auto const tmp1
                {
                    previousRoundOutput[ 7 ] +
                    sigma1( previousRoundOutput[ 4 ] ) +
                    ch( previousRoundOutput[ 4 ], previousRoundOutput[ 5 ], previousRoundOutput[ 6 ] ) +
                    roundConstants[ j ] +
                    words[ j ]
                };
                auto const tmp2
                {
                    sigma0( previousRoundOutput[ 0 ] ) +
                    maj( previousRoundOutput[ 0 ], previousRoundOutput[ 1 ], previousRoundOutput[ 2 ] )
                };

                previousRoundOutput[ 7U ] = previousRoundOutput[ 6U ];
                previousRoundOutput[ 6U ] = previousRoundOutput[ 5U ];
                previousRoundOutput[ 5U ] = previousRoundOutput[ 4U ];
                previousRoundOutput[ 4U ] = previousRoundOutput[ 3U ] + tmp1;
                previousRoundOutput[ 3U ] = previousRoundOutput[ 2U ];
                previousRoundOutput[ 2U ] = previousRoundOutput[ 1U ];
                previousRoundOutput[ 1U ] = previousRoundOutput[ 0U ];
                previousRoundOutput[ 0U ] = tmp1 + tmp2;
            }

            // perform addition to compute the intermediate hash values
            for ( auto j{ 0U }; j < hashBufferLength; j++ )
            {
                result[ j ] += previousRoundOutput[ j ];
            }
        }

        return result;
    }
} // namespace

std::string hash( std::string_view const data )
{
    auto buffer{ preprocess( data ) };
    auto result{ process( std::move( buffer ) ) };

    return toHex( result );
}

} // namespace A1::Utils::Sha512