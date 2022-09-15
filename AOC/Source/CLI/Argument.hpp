/**
 *
 * Copyright (c)2022 The Adamnite C++ Authors.
 *
 * This code is open-sourced under the MIT license.
 */

#pragma once

#include <string_view>
#include <cstdint>

#include <fmt/format.h>

namespace A1::CLI
{

/**
 * Represents a positional argument in the CLI.
 */
struct Argument
{
    /**
     * A name that will appear in the CLI help text.
     */
    std::string_view name;

    /**
     * A description that will appear in the CLI help text.
     */
    std::string_view description;
};

} // namespace A1::CLI

template<>
struct fmt::formatter< A1::CLI::Argument >
{
    template< typename ParseContext >
    constexpr auto parse( ParseContext & ctx )
    {
        return ctx.begin();
    }

    template< typename FormatContext >
    auto format( A1::CLI::Argument const & argument, FormatContext & ctx ) const
    {
        return fmt::format_to
        (
            ctx.out(),
            "\t{0} {1}",
            argument.name,
            argument.description
        );
    }
};