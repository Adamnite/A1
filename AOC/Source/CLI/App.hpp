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

class App
{
public:
    App( Config config ) : config_{ config } {}

    void addOption  ( Option   option   );
    void addArgument( Argument argument );

    void parse( int const argc, char * argv[] );

    [[ nodiscard ]] std::string helpMessage   ();
    [[ nodiscard ]] std::string versionMessage();

private:
    std::vector< Option   > options_;
    std::vector< Argument > arguments_;

    Config config_;
};

} // namespace A1::CLI