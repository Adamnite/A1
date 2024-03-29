/**
 *
 * Copyright (c)2022 The Adamnite C++ Authors.
 *
 * This code is open-sourced under the MIT license.
 */

#pragma once

#include "Symbols.hpp"

#if defined (__clang__)
#   pragma clang diagnostic push
#   pragma clang diagnostic ignored "-Wunused-parameter"
#elif defined(__GNUC__) || defined(__GNUG__)
#   pragma GCC diagnostic push
#   pragma GCC diagnostic ignored "-Wunused-parameter"
#endif

#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>

#if defined(__clang__)
#   pragma clang diagnostic pop
#elif defined(__GNUC__) || defined(__GNUG__)
#   pragma GCC diagnostic pop
#endif

#include <memory>

namespace A1::LLVM
{

struct Context final
{
    std::unique_ptr< llvm::LLVMContext > internalCtx;

    /** Provides an API for creating instructions and inserting them into basic blocks. */
    std::unique_ptr< llvm::IRBuilder<> > builder;

    /** Stores all the information related to LLVM module. */
    std::unique_ptr< llvm::Module > module_;

    /** Stores all the symbols within the module. */
    Symbols symbols;

    /** Stores all the external module names that are imported within the current module. */
    std::vector< std::string > importedModules{};
};

} // namespace A1::LLVM