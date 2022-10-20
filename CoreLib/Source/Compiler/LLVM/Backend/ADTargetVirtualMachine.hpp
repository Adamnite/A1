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
#   pragma clang diagnostic ignored "-Wdeprecated-enum-enum-conversion"
#elif defined(__GNUC__) || defined(__GNUG__)
#   pragma GCC diagnostic push
#   pragma GCC diagnostic ignored "-Wunused-parameter"
#endif

#include <llvm/Target/TargetMachine.h>
#include <llvm/CodeGen/TargetInstrInfo.h>

#if defined(__clang__)
#   pragma clang diagnostic pop
#elif defined(__GNUC__) || defined(__GNUG__)
#   pragma GCC diagnostic pop
#endif

namespace llvm
{
    class Module;
} // namespace llvm

namespace A1::LLVM
{

class ADTargetVirtualMachine : public llvm::TargetMachine
{
public:
    ADTargetVirtualMachine( llvm::Module const & module_, std::string const & FS );

    virtual llvm::DataLayout      const * getDataLayout() const { return &dataLayout_; }
    virtual llvm::TargetInstrInfo const * getInstrInfo () const { return &instrInfo_;  }

    static unsigned getModuleMatchQuality( llvm::Module const & module_ );

    // Pass pipeline configuration
    virtual bool addInstSelector( llvm::PassManagerBase & passManager, bool fast );
    virtual bool addPreEmitPass ( llvm::PassManagerBase & passManager, bool fast );

private:
    // calculates type size and alignment
    llvm::DataLayout const dataLayout_;

    llvm::TargetInstrInfo instrInfo_;
};

} // namespace A1::LLVM