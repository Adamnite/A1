/**
 *
 * Copyright (c)2022 The Adamnite C++ Authors.
 *
 * This code is open-sourced under the MIT license.
 */

#include <CoreLib/Tokenizer/Tokenizer.hpp>

#include <string_view>

extern "C" int LLVMFuzzerTestOneInput( std::uint8_t const * data, std::size_t const size )
{
    std::string_view const expression{ reinterpret_cast< char const * >( data ), size };
    [[ maybe_unused ]] auto token{ A1::tokenize( A1::Stream{ expression } ) };
    return 0;
}