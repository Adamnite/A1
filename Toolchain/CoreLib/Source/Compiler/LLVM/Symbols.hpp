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
        Table< llvm::Value *        > builtInGlobalVariables,
        Table< llvm::FunctionCallee > builtInFunctions
    )
    : builtInGlobalVariables_{ std::move( builtInGlobalVariables ) }
    , builtInFunctions_      { std::move( builtInFunctions ) }
    {}

    [[ nodiscard ]] Table< llvm::Value *        > const & builtInGlobalVariables() const noexcept { return builtInGlobalVariables_; }
    [[ nodiscard ]] Table< llvm::FunctionCallee > const & builtInFunctions      () const noexcept { return builtInFunctions_;       }

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
    Table< llvm::Value *        > builtInGlobalVariables_;
    Table< llvm::FunctionCallee > builtInFunctions_;
};

} // namespace A1::LLVM