/**
 *
 * Copyright (c)2022 The Adamnite C++ Authors.
 *
 * This code is open-sourced under the MIT license.
 */

#include <CoreLib/Compiler/LLVM/Codegen.hpp>
#include <CoreLib/Compiler/LLVM/Compiler.hpp>

#if defined (__clang__)
#   pragma clang diagnostic push
#   pragma clang diagnostic ignored "-Wunused-parameter"
#elif defined(__GNUC__) || defined(__GNUG__)
#   pragma GCC diagnostic push
#   pragma GCC diagnostic ignored "-Wunused-parameter"
#endif

#include <llvm/ADT/APInt.h>
#include <llvm/IR/LegacyPassManager.h>
#include <llvm/MC/TargetRegistry.h>
#include <llvm/Support/FileSystem.h>
#include <llvm/Support/Host.h>
#include <llvm/Support/raw_ostream.h>
#include <llvm/Support/TargetSelect.h>
#include <llvm/Target/TargetMachine.h>
#include <llvm/Target/TargetOptions.h>

#if defined(__clang__)
#   pragma clang diagnostic pop
#elif defined(__GNUC__) || defined(__GNUG__)
#   pragma GCC diagnostic pop
#endif

namespace A1::LLVM
{

bool compile( Node::Pointer const & node )
{
    /**
     * Initialize the target registry, ASM parser, ASM printers, etc.
     */
    llvm::InitializeAllTargetInfos();
    llvm::InitializeAllTargets    ();
    llvm::InitializeAllTargetMCs  ();
    llvm::InitializeAllAsmParsers ();
    llvm::InitializeAllAsmPrinters();

    /**
     * In order to specify the architecture we want to target, we need the
     * target triple which has the form: '<arch><sub>-<vendor>-<sys>-<abi>'.
     * An example of such target triple is: 'x86_64-unknown-linux-gnu'.
     * Even though LLVM supports cross-compilation, we target current machine here.
     */
    auto const targetTriple{ llvm::sys::getDefaultTargetTriple() };

    std::string error;
    auto const * target{ llvm::TargetRegistry::lookupTarget( targetTriple, error ) };

    if ( target == nullptr )
    {
        /**
         * The requested target could not be found.
         * This occurs if the TargetRegistry is not initialized or the
         * target triple is invalid.
         */
        return false;
    }

    /**
     * Generic CPU is used without any additional features.
     */
    auto const cpu{ "generic" };
    auto const features{ "" };

    llvm::TargetOptions options;
    llvm::Optional< llvm::Reloc::Model > relocationModel;

    auto targetMachine{ target->createTargetMachine( targetTriple, cpu, features, options, relocationModel ) };

    auto module_{ codegen( node, targetMachine->createDataLayout(), targetTriple ) };

    auto const outputFileName{ "output.o" };

    std::error_code errorCode;
    llvm::raw_fd_ostream dst{ outputFileName, errorCode, llvm::sys::fs::OF_None };

    if ( errorCode )
    {
        return false;
    }

    /**
     * @note: Uncomment following line if you want
     *        to print generated IR to the standard output.
     *
     * @todo: Support printing IR from the AOC CLI.
     */
    // module_->print( llvm::outs(), nullptr );

    // save LLVM IR to string
    std::string output;
    {
        llvm::raw_string_ostream os{ output };
        os << *module_;
        os.flush();
    }

    std::printf( "%s", output.c_str() );

    return true;
}

} // namespace A1::LLVM