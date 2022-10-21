/**
 *
 * Copyright (c)2022 The Adamnite C++ Authors.
 *
 * This code is open-sourced under the MIT license.
 */

#include "Backend/ADTargetVirtualMachine.hpp"
#include "IRCodegen/Codegen.hpp"

#include <CoreLib/Compiler/LLVM/Compiler.hpp>

#if defined (__clang__)
#   pragma clang diagnostic push
#   pragma clang diagnostic ignored "-Wunused-parameter"
#   pragma clang diagnostic ignored "-Wdeprecated-enum-enum-conversion"
#elif defined(__GNUC__) || defined(__GNUG__)
#   pragma GCC diagnostic push
#   pragma GCC diagnostic ignored "-Wunused-parameter"
#endif

#include <llvm/ADT/APInt.h>
#include <llvm/ADT/IntrusiveRefCntPtr.h>
#include <llvm/IR/LegacyPassManager.h>
#include <llvm/MC/TargetRegistry.h>
#include <llvm/Support/FileSystem.h>
#include <llvm/Support/Host.h>
#include <llvm/Support/raw_ostream.h>
#include <llvm/Support/TargetSelect.h>
#include <llvm/Support/VirtualFileSystem.h>
#include <llvm/Target/TargetMachine.h>
#include <llvm/Target/TargetOptions.h>

#include <clang/Driver/Driver.h>
#include <clang/Driver/Compilation.h>
#include <clang/Frontend/TextDiagnosticPrinter.h>

#if defined(__clang__)
#   pragma clang diagnostic pop
#elif defined(__GNUC__) || defined(__GNUG__)
#   pragma GCC diagnostic pop
#endif

namespace A1::LLVM
{

bool compile( Compiler::Settings settings, AST::Node::Pointer const & node )
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

    /**
     * Generate LLVM IR code from the AST.
     */
    auto context{ IR::codegen( node, targetMachine->createDataLayout(), targetTriple ) };

    if ( settings.outputIR )
    {
        /**
         * Write generated LLVM IR code to standard output.
         */
        std::printf( "\nLLVM IR:\n" );
        context.module_->print( llvm::outs(), nullptr );
        std::printf( "\n" );
    }

    /**
     * Save generated LLVM IR to a file.
     */
    static constexpr auto IROutputFilename{ "tmp.ll" };
    {
        std::error_code errorCode;
        llvm::raw_fd_ostream os{ IROutputFilename, errorCode, llvm::sys::fs::OF_None };
        os << *context.module_;
        os.flush();
    }

    /**
     * Compile LLVM IR file to an executable.
     */

    llvm::IntrusiveRefCntPtr< clang::DiagnosticOptions > diagnosticOptions{ new clang::DiagnosticOptions() };
    clang::TextDiagnosticPrinter * diagnosticClient{ new clang::TextDiagnosticPrinter( llvm::errs(), &*diagnosticOptions ) };

    llvm::IntrusiveRefCntPtr< clang::DiagnosticIDs > diagnosticIDs{ new clang::DiagnosticIDs() };
    clang::DiagnosticsEngine diagnosticsEngine{ diagnosticIDs, &*diagnosticOptions, diagnosticClient };

    clang::driver::Driver driver{ CLANG_PATH, targetTriple, diagnosticsEngine };
    clang::ArrayRef< char const * > arguments{ "-g", IROutputFilename, "-o", settings.executableFilename.c_str() };

    std::unique_ptr< clang::driver::Compilation > compilation{ driver.BuildCompilation( arguments ) };

    if ( compilation == nullptr || compilation->containsError() )
    {
        std::remove( IROutputFilename );
        return false;
    }

    clang::SmallVector< std::pair< int, clang::driver::Command const * >, 4U > failingCommands;
    driver.ExecuteCompilation( *compilation, failingCommands );

    std::remove( IROutputFilename );
    return true;
}

} // namespace A1::LLVM