/**
 *
 * Copyright (c)2022 The Adamnite C++ Authors.
 *
 * This code is open-sourced under the MIT license.
 */

#include "CLI/App.hpp"

int main( int argc, char * argv[] )
{
    A1::CLI::App app
    {
        {
            .title       = "aoc",
            .description = "A1 smart contract programming language compiler",
            .version     = "0.1.0"
        }
    };

    app.addOption( { .short_ = "-o", .long_ = "--output", .name = "file", .description = "Write output to specific file" } );
    app.addArgument( { .name = "file", .description = "File to be compiled" } );

    app.parse( argc, argv );

    return 0;
}