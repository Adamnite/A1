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

#include <llvm/IR/Function.h>
#include <llvm/IR/Intrinsics.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>

#if defined(__clang__)
#   pragma clang diagnostic pop
#elif defined(__GNUC__) || defined(__GNUG__)
#   pragma GCC diagnostic pop
#endif

namespace A1::LLVM::IR
{

namespace
{
    template< typename ... ParameterTs > requires ( std::is_base_of_v< llvm::Type, std::remove_pointer_t< ParameterTs > > && ... )
    [[ maybe_unused ]]
    [[ nodiscard ]]
    llvm::Function * codegenIntrinsicWrapper
    (
        std::string_view const   intrinsicName,
        std::string_view const   wrapperName,
        llvm::LLVMContext      & ctx,
        llvm::Module           & module_,
        llvm::IRBuilder<>      & builder,
        llvm::Type             * returnType,
        ParameterTs ...          parameterTypes
    )
    {
        auto const   intrinsicID{ llvm::Function::lookupIntrinsicID( intrinsicName ) };
        auto       * intrinsic  { llvm::Intrinsic::getDeclaration( &module_, intrinsicID ) };

        auto * wrapperType{ llvm::FunctionType::get( returnType, { parameterTypes ... }, false ) };
        auto * wrapper    { llvm::Function::Create( wrapperType, llvm::Function::InternalLinkage, wrapperName, &module_ ) };
        auto * block      { llvm::BasicBlock::Create( ctx, "", wrapper ) };

        builder.SetInsertPoint( block );

        std::array< llvm::Value *, sizeof...( parameterTypes ) > arguments;
        for ( std::size_t i{ 0U }; i < std::size( arguments ); ++i )
        {
            arguments[ i ] = wrapper->getArg( i );
        }

        auto * returnValue
        {
            sizeof...( parameterTypes ) == 0
                ? builder.CreateCall( intrinsic, llvm::None )
                : builder.CreateCall( intrinsic, arguments )
        };

        if ( returnValue->getType() == returnType )
        {
            builder.CreateRet( returnValue );
        }
        else
        {
            builder.CreateRet( llvm::CastInst::Create( llvm::Instruction::BitCast, returnValue, returnType, "", block ) );
        }

        return wrapper;
    }

    [[ maybe_unused ]]
    [[ nodiscard ]]
    llvm::Function * codegenContractAddress( llvm::LLVMContext & ctx, llvm::Module & module_, llvm::IRBuilder<> & builder )
    {
        return codegenIntrinsicWrapper
        (
            "llvm.wasm.advm.contract.addr",
            "contract_address",
            ctx, module_, builder,
            llvm::Type::getInt8PtrTy( ctx )
        );
    }

    [[ maybe_unused ]]
    [[ nodiscard ]]
    llvm::Function * codegenCallerAddress( llvm::LLVMContext & ctx, llvm::Module & module_, llvm::IRBuilder<> & builder )
    {
        return codegenIntrinsicWrapper
        (
            "llvm.wasm.advm.caller.addr",
            "caller_address",
            ctx, module_, builder,
            llvm::Type::getInt8PtrTy( ctx )
        );
    }

    [[ maybe_unused ]]
    [[ nodiscard ]]
    llvm::Function * codegenBlockTimestamp( llvm::LLVMContext & ctx, llvm::Module & module_, llvm::IRBuilder<> & builder )
    {
        return codegenIntrinsicWrapper
        (
            "llvm.wasm.advm.block.ts",
            "block_timestamp",
            ctx, module_, builder,
            llvm::Type::getInt64Ty( ctx )
        );
    }

    [[ maybe_unused ]]
    [[ nodiscard ]]
    llvm::Function * codegenBalances( llvm::LLVMContext & ctx, llvm::Module & module_, llvm::IRBuilder<> & builder )
    {
        return codegenIntrinsicWrapper
        (
            "llvm.wasm.advm.balances",
            "balances",
            ctx, module_, builder,
            llvm::Type::getInt64PtrTy( ctx ),
            llvm::Type::getInt8PtrTy( ctx )
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
                    llvm::IntegerType::getInt64Ty( ctx ),
                    llvm::PointerType::get( llvm::Type::getInt8Ty( ctx ), 0 ),
                    false
                )
            )
        }
    };
}

Symbols::Table< llvm::Function * > internalBuiltinFunctions
(
    [[ maybe_unused ]] llvm::LLVMContext & ctx,
    [[ maybe_unused ]] llvm::Module      & module_,
    [[ maybe_unused ]] llvm::IRBuilder<> & builder
)
{
    return
    {
#ifndef TESTS_ENABLED
        { "contract_address", codegenContractAddress( ctx, module_, builder ) },
        { "caller_address"  , codegenCallerAddress  ( ctx, module_, builder ) },
        { "block_timestamp" , codegenBlockTimestamp ( ctx, module_, builder ) },
        { "balances"        , codegenBalances       ( ctx, module_, builder ) }
#endif // TESTS_ENABLED
    };
}

} // namespace A1::LLVM::IR
