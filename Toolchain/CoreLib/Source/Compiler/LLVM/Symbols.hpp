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

#include <string>
#include <map>

namespace A1::LLVM
{

struct Symbols
{
    /**
     * Stores both local and global variables as well as function parameters.
     * Once variable has gone out of scope, it is removed from the map.
     */
    std::map< std::string, llvm::Value * > variables;

    /** Stores all user-defined functions. */
    std::map< std::string, llvm::FunctionCallee > functions;

    /** Stores all user-defined contract types. */
    std::map< std::string, llvm::StructType * > contractTypes;

    Symbols() = default;

    Symbols
    (
        std::map< std::string, llvm::Value *        > builtInGlobalVariables,
        std::map< std::string, llvm::FunctionCallee > builtInFunctions
    )
    : builtInGlobalVariables_{ std::move( builtInGlobalVariables ) }
    , builtInFunctions_      { std::move( builtInFunctions ) }
    {}

    [[ nodiscard ]] std::map< std::string, llvm::Value *        > const & builtInGlobalVariables() const noexcept { return builtInGlobalVariables_; }
    [[ nodiscard ]] std::map< std::string, llvm::FunctionCallee > const & builtInFunctions      () const noexcept { return builtInFunctions_;       }

    [[ nodiscard ]]
    llvm::Value * getVariable( std::string const & name ) const noexcept
    {
        if ( auto it{ variables.find( name ) }; it != std::end( variables ) )
        {
            return it->second;
        }
        return nullptr;
    }

private:
    std::map< std::string, llvm::Value *        > builtInGlobalVariables_;
    std::map< std::string, llvm::FunctionCallee > builtInFunctions_;
};

} // namespace A1::LLVM