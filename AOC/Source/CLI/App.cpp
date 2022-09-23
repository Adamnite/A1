/**
 *
 * Copyright (c)2022 The Adamnite C++ Authors.
 *
 * This code is open-sourced under the MIT license.
 */

#include "CLI/App.hpp"
#include "CLI/Exception.hpp"

#include <fmt/format.h>

#include <span>

namespace A1::CLI
{

namespace
{
    [[ nodiscard ]]
    bool isOption( std::string_view const value ) noexcept
    {
        return value.starts_with( "-" ) || value.starts_with( "--" );
    }

    [[ nodiscard ]]
    auto findOption( std::span< Option > const options, std::string_view const value ) noexcept
    {
        return std::find_if
        (
            std::begin( options ),
            std::end  ( options ),
            [ value ]( auto && option ) noexcept
            {
                return option.short_ == value || option.long_ == value;
            }
        );
    }

    [[ nodiscard ]]
    std::vector< std::string_view > getArgumentNames( std::span< Argument const > const arguments )
    {
        std::vector< std::string_view > argumentNames;
        std::transform
        (
            std::begin( arguments ),
            std::end  ( arguments ),
            std::back_inserter( argumentNames ),
            []( auto && argument ) noexcept
            {
                return argument.name;
            }
        );
        return argumentNames;
    }
} // namespace

App::App( Settings settings ) : settings_{ std::move( settings ) }
{
    // add default options
    options_.push_back
    (
        {
            .short_      = "-h",
            .long_       = "--help",
            .name        = "help",
            .description = "Print help",
            .output      = helpOutput_
        }
    );
    options_.push_back
    (
        {
            .short_      = "-v",
            .long_       = "--version",
            .name        = "version",
            .description = "Print version",
            .output      = versionOutput_
        }
    );
}

void App::addOption( Option argument )
{
    options_.push_back( std::move( argument ) );
}

void App::addArgument( Argument argument )
{
    arguments_.push_back( std::move( argument ) );
}

void App::parse( int const argc, char * argv[] )
{
    argumentNames_ = getArgumentNames( arguments_ );

    auto currentArgumentIdx{ 0U };

    for ( auto i{ 1 }; i < argc; ++i )
    {
        if ( isOption( argv[ i ] ) )
        {
            // parse option
            if ( auto optionIt{ findOption( options_, argv[ i ] ) }; optionIt != std::end( options_ ) )
            {
                if      ( optionIt->name == "help"    ) { helpOutput_    = ""; return; }
                else if ( optionIt->name == "version" ) { versionOutput_ = ""; return; }

                // parse value
                if ( i + 1 < argc )
                {
                    optionIt->output = argv[ ++i ];
                }
                else
                {
                    throw Exception
                    (
                        fmt::format( "Missing a value for the option: {}", argv[ i ] ),
                        helpMessage()
                    );
                }
            }
            else
            {
                throw Exception
                (
                    fmt::format( "Unknown option: {}", argv[ i ] ),
                    helpMessage()
                );
            }
        }
        else
        {
            // parse argument
            if ( currentArgumentIdx < std::size( arguments_ ) )
            {
                arguments_[ currentArgumentIdx++ ].output = argv[ i ];
            }
            else
            {
                throw Exception
                (
                    fmt::format( "Unknown argument: {}", argv[ i ] ),
                    helpMessage()
                );
            }
        }
    }

    if ( currentArgumentIdx != std::size( arguments_ ) )
    {
        throw Exception
        (
            fmt::format
            (
                "Missing arguments: {}",
                fmt::join( std::begin( argumentNames_ ) + currentArgumentIdx, std::end( argumentNames_ ), ", " )
            ),
            helpMessage()
        );
    }
}

std::string App::helpMessage() const
{
    return fmt::format
    (
        "{} - {}\n\n"
        "USAGE: {} [OPTIONS] {}\n\n"
        "OPTIONS:\n"
        "{}\n\n"
        "ARGS:\n"
        "{}",
        settings_.title,
        settings_.description,
        settings_.title,
        fmt::join( argumentNames_, " "  ),
        fmt::join( options_      , "\n" ),
        fmt::join( arguments_    , "\n" )
    );
}

std::string App::versionMessage() const
{
    return fmt::format
    (
        "{} version: {}\n",
        settings_.title,
        settings_.version
    );
}

} // namespace A1::CLI