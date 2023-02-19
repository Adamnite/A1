/**
 *
 * Copyright (c)2022 The Adamnite C++ Authors.
 *
 * This code is open-sourced under the MIT license.
 */

#include "IRCodegen/Codegen.hpp"
#include "Utils/Utils.hpp"

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
#include <llvm/Passes/OptimizationLevel.h>
#include <llvm/Passes/PassBuilder.h>
#include <llvm/Support/FileSystem.h>
#include <llvm/Support/Host.h>
#include <llvm/Support/Program.h>
#include <llvm/Support/raw_ostream.h>
#include <llvm/Support/TargetSelect.h>
#include <llvm/Support/VirtualFileSystem.h>
#include <llvm/Target/TargetMachine.h>
#include <llvm/Target/TargetOptions.h>

#if defined(__clang__)
#   pragma clang diagnostic pop
#elif defined(__GNUC__) || defined(__GNUG__)
#   pragma GCC diagnostic pop
#endif

#include <unordered_map>

namespace A1::LLVM
{

namespace
{
    std::unordered_map< std::string_view, char const * > externalModulePaths
    {
        {
#ifndef TESTS_ENABLED
            { "core", WASM_UTILS_LIBRARY_PATH }
#endif // TESTS_ENABLED
        }
    };
} // namespace

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
     */
#ifdef TESTS_ENABLED
    auto const targetTriple{ llvm::sys::getDefaultTargetTriple() };
#else
    auto const targetTriple{ "wasm32-unknown-wasi" };
#endif // TESTS_ENABLED

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

    /**
     * Optimize LLVM IR.
     *
     * @note: Optimizations are turned on only for the sake of testing.
     * In production, we do not want optimizations to remove all unused functions,
     * smart contracts, etc. as these are used later by the Adamnite VM.
     */
#ifdef TESTS_ENABLED
        llvm::ModuleAnalysisManager   moduleAnalysisManager;
        llvm::CGSCCAnalysisManager    cGSCCAnalysisManager;
        llvm::FunctionAnalysisManager functionAnalysisManager;
        llvm::LoopAnalysisManager     loopAnalysisManager;

        llvm::PassBuilder passBuilder{ targetMachine };
        passBuilder.registerModuleAnalyses  ( moduleAnalysisManager   );
        passBuilder.registerCGSCCAnalyses   ( cGSCCAnalysisManager    );
        passBuilder.registerFunctionAnalyses( functionAnalysisManager );
        passBuilder.registerLoopAnalyses    ( loopAnalysisManager     );

        passBuilder.crossRegisterProxies( loopAnalysisManager, functionAnalysisManager, cGSCCAnalysisManager, moduleAnalysisManager );

        /**
         * @note: Crash happens in llvm::AnalysisManager, LLVM 14.0.6.
         * Uncomment once we update LLVM to newer version.
         */
        // auto modulePassManager{ passBuilder.buildPerModuleDefaultPipeline( llvm::OptimizationLevel::O3 ) };
        // modulePassManager.run( *context.module_, moduleAnalysisManager );
#endif // TESTS_ENABLED

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
     * Compile to object code.
     */
    static constexpr auto objectCodeFilename{ "out.o" };
    {
        std::error_code errorCode;
        llvm::raw_fd_ostream dst{ objectCodeFilename, errorCode, llvm::sys::fs::OF_None };

        if ( errorCode )
        {
            llvm::errs() << "Could not open a file: " << errorCode.message();
            return false;
        }

        llvm::legacy::PassManager passManager;
        targetMachine->addPassesToEmitFile( passManager, dst, nullptr, llvm::CGFT_ObjectFile );
        passManager.run( *context.module_ );
        dst.flush();
    }

#ifdef TESTS_ENABLED
    static constexpr auto linker{ "ld" };
#else
    static constexpr auto linker{ "wasm-ld" };
#endif // TESTS_ENABLED

    if ( llvm::ErrorOr< std::string > path{ llvm::sys::findProgramByName( linker ) } )
    {
        ASSERT( llvm::sys::fs::can_execute( *path ) );

        std::vector< llvm::StringRef > arguments
        {
#ifdef TESTS_ENABLED
#   if __APPLE__
            "-L/usr/lib", "-lSystem",
            "-syslibroot", SYSROOT_PATH,
            "-arch", ARCHITECTURE,
#   elif __linux__
            "-lc", "--entry", "main",
#   endif
#else
            "--no-entry",
            "--allow-undefined",
            "--export-dynamic",
            "-L", WASM_RUNTIME_LIBRARY_PATH,
#endif // TESTS_ENABLED
            "-o", settings.executableFilename.data(), objectCodeFilename
        };

#ifndef TESTS_ENABLED
        for ( auto const & module : context.importedModules )
        {
            arguments.push_back( externalModulePaths[ module ] );
        }
#endif // TESTS_ENABLED

        auto const result{ llvm::sys::ExecuteAndWait( *path, arguments ) };
        if ( result < 0 )
        {
            std::printf( "%s linker failed!", linker );
            return false;
        }
    }
    else
    {
        std::printf( "%s linker not found in PATH!", linker );
        return false;
    }

    std::remove( objectCodeFilename );
    return true;
}

} // namespace A1::LLVM