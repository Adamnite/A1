/**
 *
 * Copyright (c)2022 The Adamnite C++ Authors.
 *
 * This code is open-sourced under the MIT license.
 */

#pragma once

#include "CLI/Argument.hpp"

#include <map>
#include <optional>
#include <string>
#include <type_traits>
#include <vector>

namespace A1::CLI
{

/**
 * Represents a CLI application.
 */
struct App final
{
#define STRONG_TYPE( X )                 \
    struct X                             \
    {                                    \
        explicit X( std::string value_ ) \
        : value{ std::move( value_ ) }   \
        {}                               \
                                         \
        std::string value;               \
    };

    STRONG_TYPE( Title       )
    STRONG_TYPE( Description )
    STRONG_TYPE( Version     )
#undef STRONG_TYPE

    App( Title title, Description description, Version version )
    : title_      { std::move( title       ) }
    , description_{ std::move( description ) }
    , version_    { std::move( version     ) }
    {}

    void addArgument( Argument argument )
    {
        if ( argument.isOptional() ) { optionalArguments_  .push_back( std::move( argument ) ); }
        else                         { positionalArguments_.push_back( std::move( argument ) ); }
    }

    void parse( int const argc, char * argv[] );

    template< typename T >
    typename std::enable_if_t< ! std::is_same_v< T, bool >, std::optional< T > >
    get( std::string_view const argument ) const noexcept
    {
        if ( auto it{ setArguments_.find( argument ) }; it != std::end( setArguments_ ) )
        {
            return it->second;
        }
        return std::nullopt;
    }

    template< typename T >
    typename std::enable_if_t< std::is_same_v< T, bool >, T >
    get( std::string_view const argument ) const noexcept
    {
        return std::find
        (
            std::begin( implicitlySetArguments_ ),
            std::end  ( implicitlySetArguments_ ),
            argument
        ) != std::end( implicitlySetArguments_ );
    }

    [[ nodiscard ]] std::string help   () const;
    [[ nodiscard ]] std::string version() const;

private:
    std::vector< Argument > optionalArguments_;
    std::vector< Argument > positionalArguments_;

    std::vector< std::string_view > implicitlySetArguments_;
    std::map< std::string_view, std::string > setArguments_;

    Title       title_;
    Description description_;
    Version     version_;
};

} // namespace A1::CLI