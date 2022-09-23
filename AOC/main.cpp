/**
 *
 * Copyright (c)2022 The Adamnite C++ Authors.
 *
 * This code is open-sourced under the MIT license.
 */

#include "CLI/App.hpp"
#include "CLI/Exception.hpp"

#include <CoreLib/Compiler/Settings.hpp>
#include <CoreLib/Module.hpp>

#include <cstdio>

int main( int argc, char * argv[] )
{
    A1::CLI::App app
    {
        {
            .title       = "aoc",
            .description = "A1 smart contract programming language compiler",
            .version     = AOC_VERSION
        }
    };

    std::optional< std::string > outputFile;
    app.addOption
    (
        {
            .short_      = "-o",
            .long_       = "--output",
            .name        = "file",
            .description = "Write output to specific file",
            .output      = outputFile
        }
    );

    std::string inputFile;
    app.addArgument
    (
        {
            .name        = "file",
            .description = "File to be compiled",
            .output      = inputFile
        }
    );

    try
    {
        app.parse( argc, argv );

        if ( app.writeHelp() )
        {
            std::printf( "%s\n", app.helpMessage().c_str() );
        }
        else if ( app.writeVersion() )
        {
            std::printf( "%s\n", app.versionMessage().c_str() );
        }
        else
        {
            A1::Compiler::Settings settings{ .executableFilename = outputFile.value_or( "main" ) };
            if ( auto const success{ A1::load( std::move( settings ), inputFile ) }; success )
            {
                std::printf( "Compilation successful!" );
            }
        }
    }
    catch ( A1::CLI::Exception const & ex )
    {
        std::printf( "%s\n\n", ex.what() );
        std::printf( "%s\n\n", ex.help() );
    }

    return 0;
}