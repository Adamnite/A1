/**
 *
 * Copyright (c)2022 The Adamnite C++ Authors.
 *
 * This code is open-sourced under the MIT license.
 */

#include "CLI/App.hpp"
#include "CLI/Exception.hpp"

#include <fmt/format.h>

#include <algorithm>

namespace A1::CLI
{

void App::parse( int const argc, char * argv[] )
{
    setArguments_          .clear();
    implicitlySetArguments_.clear();

    auto setPositionalArgumentsCount{ 0U };

    static constexpr auto isOptional
    {
        []( std::string_view const value ) noexcept
        {
            return value.starts_with( "-" ) || value.starts_with( "--" );
        }
    };

    for ( auto i{ 1 }; i < argc; ++i )
    {
        if ( isOptional( argv[ i ] ) )
        {
            auto const it
            {
                std::find_if
                (
                    std::begin( optionalArguments_ ),
                    std::end  ( optionalArguments_ ),
                    [ value = argv[ i ] ]( auto && arg ) noexcept
                    {
                        return arg.short_ == value || arg.long_ == value;
                    }
                )
            };

            if ( it == std::end( optionalArguments_ ) )
            {
                throw Exception( fmt::format( "Unknown argument: {}", argv[ i ] ), help() );
            }

            if ( it->implicit )
            {
                implicitlySetArguments_.push_back( it->long_ );
            }
            else if ( i + 1 < argc )
            {
                setArguments_[ it->long_ ] = argv[ ++i ];
            }
            else
            {
                throw Exception( fmt::format( "Missing a value for an optional argument: {}", argv[ i ] ), help() );
            }

            if ( it->exit ) { return; }
        }
        else
        {
            if ( setPositionalArgumentsCount < std::size( positionalArguments_ ) )
            {
                setArguments_[ positionalArguments_[ setPositionalArgumentsCount ].long_ ] = argv[ i++ ];
                setPositionalArgumentsCount++;
            }
            else
            {
                throw Exception( fmt::format( "Unknown argument: {}", argv[ i ] ), help() );
            }
        }
    }

    if ( setPositionalArgumentsCount != std::size( positionalArguments_ ) )
    {
        throw Exception
        (
            fmt::format
            (
                "Missing positional arguments:\n{}",
                fmt::join( std::begin( positionalArguments_ ) + setPositionalArgumentsCount, std::end( positionalArguments_ ), ", " )
            ),
            help()
        );
    }
}

std::string App::help() const
{
    std::vector< std::string_view > positionalArgumentNames;
    std::transform
    (
        std::begin( positionalArguments_ ),
        std::end  ( positionalArguments_ ),
        std::back_inserter( positionalArgumentNames ),
        []( auto && arg ) noexcept
        {
            return arg.long_;
        }
    );

    return fmt::format
    (
        "{} - {}\n\n"
        "USAGE: {} [OPTIONS] {}\n\n"
        "Positional arguments:\n"
        "{}\n\n"
        "Optional arguments:\n"
        "{}",
        title_      .value,
        description_.value,
        title_      .value,
        fmt::join( positionalArgumentNames, " "  ),
        fmt::join( positionalArguments_   , "\n" ),
        fmt::join( optionalArguments_     , "\n" )
    );
}

std::string App::version() const
{
    return fmt::format( "{} version: {}\n", title_.value, version_.value );
}

} // namespace A1::CLI