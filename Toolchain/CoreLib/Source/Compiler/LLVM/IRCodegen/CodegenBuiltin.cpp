/**
 *
 * Copyright (c)2022 The Adamnite C++ Authors.
 *
 * This code is open-sourced under the MIT license.
 */

#include "CodegenBuiltin.hpp"

#if defined (__clang__)
#   pragma clang diagnostic push
#   pragma clang diagnostic ignored "-Wunused-parameter"
#elif defined(__GNUC__) || defined(__GNUG__)
#   pragma GCC diagnostic push
#   pragma GCC diagnostic ignored "-Wunused-parameter"
#endif

#include <llvm/IR/Intrinsics.h>

#if defined(__clang__)
#   pragma clang diagnostic pop
#elif defined(__GNUC__) || defined(__GNUG__)
#   pragma GCC diagnostic pop
#endif

namespace A1::LLVM::IR
{

namespace
{
    [[ nodiscard ]]
    llvm::Function * createIntrinsicWrapper
    (
        llvm::LLVMContext      & ctx,
        llvm::Module           & module_,
        llvm::IRBuilder<>      & builder,
        llvm::Type             * returnType,
        std::string_view const   intrinsicName,
        std::string_view const   wrapperName
    )
    {
        auto const   intrinsicID{ llvm::Function::lookupIntrinsicID( intrinsicName ) };
        auto       * intrinsic  { llvm::Intrinsic::getDeclaration( &module_, intrinsicID ) };

        auto * wrapperType{ llvm::FunctionType::get( returnType, false ) };
        auto * wrapper    { llvm::Function::Create( wrapperType, llvm::Function::InternalLinkage, wrapperName, &module_ ) };
        auto * block      { llvm::BasicBlock::Create( ctx, "", wrapper ) };

        builder.SetInsertPoint( block );

        builder.CreateRet( builder.CreateCall( intrinsic, llvm::None, "" ) );
        return wrapper;
    }

    [[ nodiscard ]]
    llvm::Function * codegenContractAddress( llvm::LLVMContext & ctx, llvm::Module & module_, llvm::IRBuilder<> & builder )
    {
        return createIntrinsicWrapper
        (
            ctx, module_, builder,
            llvm::Type::getInt8PtrTy( ctx ),
            "llvm.wasm.advm.contract.addr",
            "contract_address"
        );
    }

    [[ nodiscard ]]
    llvm::Function * codegenCallerAddress( llvm::LLVMContext & ctx, llvm::Module & module_, llvm::IRBuilder<> & builder )
    {
        return createIntrinsicWrapper
        (
            ctx, module_, builder,
            llvm::Type::getInt8PtrTy( ctx ),
            "llvm.wasm.advm.caller.addr",
            "caller_address"
        );
    }

    [[ nodiscard ]]
    llvm::Function * codegenBlockTimestamp( llvm::LLVMContext & ctx, llvm::Module & module_, llvm::IRBuilder<> & builder )
    {
        return createIntrinsicWrapper
        (
            ctx, module_, builder,
            llvm::Type::getInt64Ty( ctx ),
            "llvm.wasm.advm.block.ts",
            "block_timestamp"
        );
    }
} // namespace

Symbols::Table< llvm::FunctionCallee > externalBuiltinFunctions( llvm::LLVMContext & ctx, llvm::Module & module_ )
{
    return
    {
        {
            "print",
            module_.getOrInsertFunction
            (
                "printf",
                llvm::FunctionType::get
                (
                    llvm::IntegerType::getInt32Ty( ctx ),
                    llvm::PointerType::get( llvm::Type::getInt8Ty( ctx ), 0 ),
                    true
                )
            )
        },
        {
            "abort",
            module_.getOrInsertFunction
            (
                "abort",
                llvm::FunctionType::get
                (
                    llvm::Type::getVoidTy( ctx ),
                    true
                )
            )
        },
        {
            "is_utf8",
            module_.getOrInsertFunction
            (
                "is_utf8",
                llvm::FunctionType::get
                (
                    llvm::IntegerType::getInt32Ty( ctx ),
                    llvm::PointerType::get( llvm::Type::getInt8Ty( ctx ), 0 ),
                    false
                )
            )
        }
    };
}

Symbols::Table< llvm::Function * > internalBuiltinFunctions
(
    llvm::LLVMContext & ctx,
    llvm::Module      & module_,
    llvm::IRBuilder<> & builder
)
{
    return
    {
        { "contract_address", codegenContractAddress( ctx, module_, builder ) },
        { "caller_address"  , codegenCallerAddress  ( ctx, module_, builder ) },
        { "block_timestamp" , codegenBlockTimestamp ( ctx, module_, builder ) }
    };
}

} // namespace A1::LLVM::IR
