/**
 *
 * Copyright (c)2022 The Adamnite C++ Authors.
 *
 * This code is open-sourced under the MIT license.
 */

#pragma once

#if defined (__clang__)
#   pragma clang diagnostic push
#   pragma clang diagnostic ignored "-Wunused-parameter"
#elif defined(__GNUC__) || defined(__GNUG__)
#   pragma GCC diagnostic push
#   pragma GCC diagnostic ignored "-Wunused-parameter"
#endif

#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/Type.h>
#include <llvm/IR/Value.h>

#if defined(__clang__)
#   pragma clang diagnostic pop
#elif defined(__GNUC__) || defined(__GNUG__)
#   pragma GCC diagnostic pop
#endif

#include <fmt/format.h>

#include <string_view>
#include <string>
#include <map>

namespace A1::LLVM
{

struct Symbols final
{
    template< typename T >
    using Table = std::map< std::string, T >;

    /**
     * Stores both local and global variables as well as function parameters.
     * Once variable has gone out of scope, it is removed from the map.
     */
    Table< llvm::Value * > variables;

    /** Stores all user-defined functions. */
    Table< llvm::FunctionCallee > functions;

    /** Stores all user-defined contract types. */
    Table< llvm::StructType * > contractTypes;

    /** Name of a function that is currently being processed (used for mangling). */
    std::string currentFunctionName;

    /** Name of a contract that is currently being processed (used for mangling). */
    std::string currentContractName;

    Symbols() = default;

    Symbols
    (
        Table< llvm::FunctionCallee > externalBuiltInFunctions,
        Table< llvm::Function *     > internalBuiltInFunctions
    )
    : externalBuiltInFunctions_{ std::move( externalBuiltInFunctions ) }
    , internalBuiltInFunctions_{ std::move( internalBuiltInFunctions ) }
    {}

    [[ nodiscard ]] Table< llvm::FunctionCallee > const & externalBuiltInFunctions() const noexcept { return externalBuiltInFunctions_; }
    [[ nodiscard ]] Table< llvm::Function *     > const & internalBuiltInFunctions() const noexcept { return internalBuiltInFunctions_; }

    [[ nodiscard ]]
    llvm::Value * variable( std::string_view const name ) noexcept
    {
        auto it{ variables.find( mangle( name ) ) };
        return it != std::end( variables ) ? it->second : nullptr;
    }

    void variable( std::string_view const name, llvm::Value * value ) noexcept
    {
        variables[ mangle( name ) ] = value;
    }

    [[ nodiscard ]]
    std::string mangle( std::string_view const name ) const
    {
        return fmt::format( "{}_{}_{}", currentContractName, currentFunctionName, name );
    }

private:
    Table< llvm::FunctionCallee > externalBuiltInFunctions_;
    Table< llvm::Function *     > internalBuiltInFunctions_;
};

} // namespace A1::LLVM