/**
 *
 * Copyright (c)2022 The Adamnite C++ Authors.
 *
 * This code is open-sourced under the MIT license.
 */

#pragma once

#include <string_view>
#include <optional>
#include <string>

#include <fmt/format.h>

namespace A1::CLI
{

/**
 * Represents an optional argument in the CLI.
 */
struct Option
{
    /**
     * A short option used to specify an optional argument
     * in the CLI. It is commonly prefixed with a single dash.
     * E.g. -o
     */
    std::string_view short_;

    /**
     * A long option used to specify an optional argument
     * in the CLI. It is commonly prefixed with double dashes.
     * E.g. --option
     */
    std::string_view long_;

    /**
     * A name that will appear in the CLI help text.
     */
    std::string_view name;

    /**
     * A description that will appear in the CLI help text.
     */
    std::string_view description;

    /**
     * A value should be omitted.
     */
    bool valueOmitted{ false };

    /**
     * An output variable to bind to.
     */
    std::optional< std::string > & output;
};

} // namespace A1::CLI

template<>
struct fmt::formatter< A1::CLI::Option >
{
    template< typename ParseContext >
    constexpr auto parse( ParseContext & ctx )
    {
        return ctx.begin();
    }

    template< typename FormatContext >
    auto format( A1::CLI::Option const & option, FormatContext & ctx ) const
    {
        if ( option.short_.empty() )
        {
            return fmt::format_to
            (
                ctx.out(),
                "\t{0} <{1}> {2}",
                option.long_,
                option.name,
                option.description
            );
        }
        else
        {
            return fmt::format_to
            (
                ctx.out(),
                "\t{0}, {1} <{2}> {3}",
                option.short_,
                option.long_,
                option.name,
                option.description
            );
        }
    }
};