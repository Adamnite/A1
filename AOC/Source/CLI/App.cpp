/**
 *
 * Copyright (c)2022 The Adamnite C++ Authors.
 *
 * This code is open-sourced under the MIT license.
 */

#include "CLI/App.hpp"

#include <fmt/format.h>

namespace A1::CLI
{

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
    std::vector< std::string > tokens;

    for ( auto i{ 1 }; i < argc; ++i )
    {
        tokens.emplace_back( argv[ i ] );
    }
}

std::string App::helpMessage()
{
    std::vector< std::string_view > argumentNames{ std::size( arguments_ ) };
    std::transform
    (
        std::begin( arguments_ ),
        std::end  ( arguments_ ),
        std::back_inserter( argumentNames ),
        []( auto && argument )
        {
            return argument.name;
        }
    );

    return fmt::format
    (
        "{} - {}\n\n"
        "USAGE: {} [OPTIONS] {}\n\n"
        "OPTIONS:\n"
        "{}\n\n"
        "ARGS:\n"
        "{}\n\n",
        config_.title, config_.description,
        config_.title, fmt::join( argumentNames, " " ),
        fmt::join( options_  , "\n" ),
        fmt::join( arguments_, "\n" )
    );
}

std::string App::versionMessage()
{
    return fmt::format
    (
        "{} version: {}\n",
        config_.title, config_.version
    );
}

} // namespace A1::CLI