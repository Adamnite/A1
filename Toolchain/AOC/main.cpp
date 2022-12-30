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
    using A1::CLI::App;
    using A1::CLI::Exception;

    App app
    {
        App::Title      { "aoc" },
        App::Description{ "A1 compiler - Develop smart contracts for the Adamnite blockchain" },
        App::Version    { AOC_VERSION }
    };

    app.addArgument( { .short_ = "-h", .long_ = "--help"   , .description  = "Print help"   , .implicit = true, .exit = true } );
    app.addArgument( { .short_ = "-v", .long_ = "--version", .description  = "Print version", .implicit = true, .exit = true } );

    app.addArgument( {                 .long_ = "file"    , .metaName = "FILE", .description = "File to be compiled"           } );
    app.addArgument( { .short_ = "-o", .long_ = "--output", .metaName = "FILE", .description = "Write output to specific file" } );

    app.addArgument( { .long_ = "--ast"    , .description = "Write Abstract Syntax Tree (AST) to standard output", .implicit = true } );
    app.addArgument( { .long_ = "--llvm-ir", .description = "Write generated LLVM IR code to standard output"    , .implicit = true } );

    try
    {
        app.parse( argc, argv );

             if ( app.get< bool >( "--help"    ) ) { std::printf( "%s\n", app.help   ().c_str() ); }
        else if ( app.get< bool >( "--version" ) ) { std::printf( "%s\n", app.version().c_str() ); }
        else
        {
            A1::Compiler::Settings settings
            {
                .executableFilename = app.get< std::string >( "--output"  ).value_or( "main" ),
                .outputAST          = app.get< bool        >( "--ast"     ),
                .outputIR           = app.get< bool        >( "--llvm-ir" )
            };

            if ( auto const success{ A1::load( std::move( settings ), app.get< std::string >( "file" ).value_or( "" ) ) }; success )
            {
                std::printf( "Compilation successful!" );
            }
        }
    }
    catch ( Exception const & ex )
    {
        std::printf( "%s\n\n", ex.what() );
        std::printf( "%s\n\n", ex.help() );
    }

    return 0;
}