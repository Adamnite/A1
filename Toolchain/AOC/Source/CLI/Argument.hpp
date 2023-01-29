/**
 *
 * Copyright (c)2022 The Adamnite C++ Authors.
 *
 * This code is open-sourced under the MIT license.
 */

#pragma once

#include <string_view>

#include <fmt/format.h>

namespace A1::CLI
{

/**
 * Represents an argument in the CLI application which can be either optional or positional.
 * If either short or long version of an argument is specified, argument is considered to be
 * optional. Otherwise, it is positional.
 */
struct Argument
{
    /**
     * A short version of an optional argument in the CLI application.
     * NOTE: Short optional argument must be prefixed with a single dash, e.g. '-o'.
     */
    std::string_view short_{};

    /**
     * A long version of an argument in the CLI application.
     * NOTE: If long version of an argument is prefixed with double dashes, e.g. '--option',
     *       argument is considered to be optional. Otherwise, argument is positional.
     */
    std::string_view long_{};

    /**
     * A meta name of an argument in the CLI application, used only in the CLI help text.
     */
    std::string_view metaName{};

    /**
     * A description of an argument in the CLI application, used only in the CLI help text.
     */
    std::string_view description{};

    /**
     * If set to true, optional argument is considered to be a flag.
     * E.g. when '--help' is provided through CLI, it's value is set to true.
     */
    bool implicit{ false };

    /**
     * If set to true, arguments following the current argument are not parsed.
     * E.g. when '--help' is provided through CLI, other arguments are not parsed.
     */
    bool exit{ false };

    [[ nodiscard ]] bool isOptional() const noexcept
    {
        return ( short_.empty() || short_.starts_with( "-" ) ) && long_.starts_with( "--" );
    }
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
        if ( argument.isOptional() )
        {
            if ( argument.short_.empty() )
            {
                if ( argument.metaName.empty() )
                {
                    return fmt::format_to
                    (
                        ctx.out(), "\t{} {}",
                        argument.long_,
                        argument.description
                    );
                }

                return fmt::format_to
                (
                    ctx.out(), "\t{} {} {}",
                    argument.long_,
                    argument.metaName,
                    argument.description
                );
            }
            else
            {
                if ( argument.metaName.empty() )
                {
                    return fmt::format_to
                    (
                        ctx.out(), "\t{}, {} {}",
                        argument.short_,
                        argument.long_,
                        argument.description
                    );
                }

                return fmt::format_to
                (
                    ctx.out(), "\t{}, {} {} {}",
                    argument.short_,
                    argument.long_,
                    argument.metaName,
                    argument.description
                );
            }
        }
        else
        {
            return fmt::format_to
            (
                ctx.out(), "\t{0} {1}",
                argument.metaName.empty() ? argument.long_ : argument.metaName,
                argument.description
            );
        }
    }
};