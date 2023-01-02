/**
 *
 * Copyright (c)2022 The Adamnite C++ Authors.
 *
 * This code is open-sourced under the MIT license.
 */

#pragma once

#include "Compiler/LLVM/Symbols.hpp"

#if defined (__clang__)
#   pragma clang diagnostic push
#   pragma clang diagnostic ignored "-Wunused-parameter"
#elif defined(__GNUC__) || defined(__GNUG__)
#   pragma GCC diagnostic push
#   pragma GCC diagnostic ignored "-Wunused-parameter"
#endif

#include <llvm/IR/Function.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>

#if defined(__clang__)
#   pragma clang diagnostic pop
#elif defined(__GNUC__) || defined(__GNUG__)
#   pragma GCC diagnostic pop
#endif

namespace A1::LLVM::IR
{

/**
 * Creates builtin function declarations whose definitions are either
 * in the standard library or 3rd party module.
 */
[[ nodiscard ]]
Symbols::Table< llvm::FunctionCallee > externalBuiltinFunctions
(
    llvm::LLVMContext & ctx,
    llvm::Module      & module_
);

/**
 * Creates builtin functions that call Adamnite VM specific LLVM intrinsics.
 */
[[ nodiscard ]]
Symbols::Table< llvm::Function * > internalBuiltinFunctions
(
    llvm::LLVMContext & ctx,
    llvm::Module      & module_,
    llvm::IRBuilder<> & builder
);

} // namespace A1::LLVM::IR