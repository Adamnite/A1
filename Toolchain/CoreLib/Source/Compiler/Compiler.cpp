/**
 *
 * Copyright (c)2022 The Adamnite C++ Authors.
 *
 * This code is open-sourced under the MIT license.
 */

#include <CoreLib/Compiler/Compiler.hpp>

#ifdef LLVM_ENABLED
#   include <CoreLib/Compiler/LLVM/Compiler.hpp>
#endif // LLVM_ENABLED

namespace A1
{

bool compile( [[ maybe_unused ]] Compiler::Settings settings, [[ maybe_unused ]] AST::Node::Pointer const & node )
{
#ifdef LLVM_ENABLED
    return LLVM::compile( std::move( settings ), node );
#else
    return false;
#endif // LLVM_ENABLED
}

} // namespace A1