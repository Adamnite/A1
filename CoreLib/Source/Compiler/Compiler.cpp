/**
 *
 * Copyright (c)2022 The Adamnite C++ Authors.
 *
 * This code is open-sourced under the MIT license.
 */

#include <CoreLib/Compiler/Compiler.hpp>
#include <CoreLib/Compiler/LLVM/Compiler.hpp>

namespace A1
{

bool compile( Compiler::Settings const settings, Node::Pointer const & node )
{
    return LLVM::compile( settings, node );
}

} // namespace A1