/**
 *
 * Copyright (c)2022 The Adamnite C++ Authors.
 *
 * This code is open-sourced under the MIT license.
 */

#include "AST/ASTPrinter.hpp"

#include <CoreLib/AST/AST.hpp>
#include <CoreLib/Compiler/Compiler.hpp>
#include <CoreLib/Module.hpp>
#include <CoreLib/Tokenizer/Tokenizer.hpp>
#include <CoreLib/Utils/Stream.hpp>

#include <fmt/format.h>

#include <memory>

namespace A1
{

namespace
{
    using FilePtr = std::unique_ptr< std::FILE, decltype( &std::fclose ) >;

    /**
     * A1 programming language requires its source files
     * to have '.ao' extension.
     */
    constexpr auto requiredFileExtension{ ".ao" };
} // namespace

bool load( Compiler::Settings settings, std::filesystem::path const inputFile )
{
    if ( inputFile.extension() != requiredFileExtension )
    {
        throw std::runtime_error
        (
            fmt::format
            (
                "File '{}' has invalid extension. Required extension: '{}'",
                inputFile.filename().string(),
                requiredFileExtension
            )
        );
    }

    if ( FilePtr f{ std::fopen( inputFile.c_str(), "r" ), &std::fclose }; f != nullptr )
    {
        auto token   { tokenize( Stream{ f.get() } ) };
        auto rootNode{ AST::parse( token ) };

        if ( settings.outputAST )
        {
            std::printf( "\nAST:\n" );
            AST::print( rootNode );
            std::printf( "\n" );
        }

        return compile( std::move( settings ), rootNode );
    }
    else
    {
        throw std::runtime_error( fmt::format( "File '{}' not found", inputFile.filename().string() ) );
    }

    return false;
}

} // namespace A1