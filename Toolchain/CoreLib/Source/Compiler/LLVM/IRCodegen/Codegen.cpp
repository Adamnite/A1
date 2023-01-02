/**
 *
 * Copyright (c)2022 The Adamnite C++ Authors.
 *
 * This code is open-sourced under the MIT license.
 */

#include "Codegen.hpp"
#include "CodegenVisitor.hpp"
#include "CodegenBuiltin.hpp"

#include <CoreLib/Utils/Macros.hpp>

#if defined (__clang__)
#   pragma clang diagnostic push
#   pragma clang diagnostic ignored "-Wunused-parameter"
#elif defined(__GNUC__) || defined(__GNUG__)
#   pragma GCC diagnostic push
#   pragma GCC diagnostic ignored "-Wunused-parameter"
#endif

#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Value.h>

#if defined(__clang__)
#   pragma clang diagnostic pop
#elif defined(__GNUC__) || defined(__GNUG__)
#   pragma GCC diagnostic pop
#endif

#include <string_view>
#include <memory>

namespace A1::LLVM::IR
{

Context codegen
(
    AST::Node::Pointer const & moduleNode,
    llvm::DataLayout   const   dataLayout,
    std::string_view   const   targetTriple
)
{
    auto ctx
    {
        [ & ]()
        {
            auto context{ std::make_unique< llvm::LLVMContext >()                };
            auto builder{ std::make_unique< llvm::IRBuilder<> >( *context )      };
            auto module_{ std::make_unique< llvm::Module >( "Module", *context ) };

            // Optimizations benefit from knowing about the target and data layout
            module_->setDataLayout  ( dataLayout   );
            module_->setTargetTriple( targetTriple );

            Symbols symbols
            {
                externalBuiltinFunctions( *context, *module_ ),
                internalBuiltinFunctions( *context, *module_, *builder )
            };

            return Context
            {
                .internalCtx = std::move( context ),
                .builder     = std::move( builder ),
                .module_     = std::move( module_ ),
                .symbols     = std::move( symbols )
            };
        }()
    };

    ASSERTM
    (
        moduleNode->is< AST::NodeType >() && moduleNode->get< AST::NodeType >() == AST::NodeType::ModuleDefinition,
        "Module definition is the root node of the AST"
    );

    auto * mainFunctionType
    {
        llvm::FunctionType::get
        (
            llvm::Type::getInt32Ty( *ctx.internalCtx ),
            std::array< llvm::Type *, 0U >{},
            false
        )
    };
    auto * mainFunction{ llvm::Function::Create( mainFunctionType, llvm::Function::ExternalLinkage, "main", *ctx.module_ ) };

    auto * mainBlock{ llvm::BasicBlock::Create( *ctx.internalCtx, "entry", mainFunction ) };
    ctx.builder->SetInsertPoint( mainBlock );

    auto inMainBlock{ true };

    // Generating LLVM IR for all the statements within the module
    for ( auto const & node : moduleNode->children() )
    {
        if ( node == nullptr ) { continue; }
        if
        (
            node->is< AST::NodeType >() &&
            (
                node->get< AST::NodeType >() == AST::NodeType::ContractDefinition ||
                node->get< AST::NodeType >() == AST::NodeType::FunctionDefinition
            )
        )
        {
            inMainBlock = false;
            codegen( ctx, node );
        }
        else
        {
            if ( !inMainBlock )
            {
                // Getting back to main block
                ctx.builder->SetInsertPoint( mainBlock );
                inMainBlock = true;
            }
            codegen( ctx, node );
        }
    }

    if ( !inMainBlock )
    {
        // Getting back to main block
        ctx.builder->SetInsertPoint( mainBlock );
    }

    ctx.builder->CreateRet( llvm::ConstantInt::get( *ctx.internalCtx, llvm::APInt( sizeof( Number ) * 8U /* numBits */, 0U, false /* isSigned */ ) ) );

#ifndef TESTS_ENABLED
    /**
     * @note: Main function exists only for the sake of testing.
     * In production, when source code is built to ADVM bytecode,
     * main function should be left out.
     */
    mainFunction->eraseFromParent();
#endif // TESTS_ENABLED

    return ctx;
}

} // namespace A1::LLVM::IR