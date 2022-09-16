/**
 *
 * Copyright (c)2022 The Adamnite C++ Authors.
 *
 * This code is open-sourced under the MIT license.
 */

#pragma once

#include "CLI/Option.hpp"
#include "CLI/Argument.hpp"

#include <string>
#include <vector>

namespace A1::CLI
{

struct Config
{
    std::string_view const title;
    std::string_view const description;
    std::string_view const version;
};

/**
 * Represents a CLI application.
 * Options -v/--version and -h/--help are added by default.
 */
class App
{
public:
    App( Config config );

    void addOption  ( Option   option   );
    void addArgument( Argument argument );

    void parse( int const argc, char * argv[] );

    [[ nodiscard ]] bool writeHelp   () const noexcept { return helpOutput_   .has_value(); }
    [[ nodiscard ]] bool writeVersion() const noexcept { return versionOutput_.has_value(); }

    [[ nodiscard ]] std::string helpMessage   () const;
    [[ nodiscard ]] std::string versionMessage() const;

private:
    std::vector< Option   > options_;
    std::vector< Argument > arguments_;

    std::vector< std::string_view > argumentNames_;

    std::optional< std::string > helpOutput_;
    std::optional< std::string > versionOutput_;

    Config config_;
};

} // namespace A1::CLI