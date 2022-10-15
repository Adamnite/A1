/**
 *
 * Copyright (c)2022 The Adamnite C++ Authors.
 *
 * This code is open-sourced under the MIT license.
 */

#include "Codegen.hpp"
#include "ExpressionCodegen.hpp"
#include "Utils/Utils.hpp"

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

#if defined(__clang__)
#   pragma clang diagnostic pop
#elif defined(__GNUC__) || defined(__GNUG__)
#   pragma GCC diagnostic pop
#endif

#include <string>
#include <memory>
#include <map>

namespace A1::LLVM
{

namespace
{
    [[ nodiscard ]]
    std::map< std::string, llvm::FunctionCallee > createStdFunctions( llvm::Module * module_, llvm::LLVMContext * context )
    {
        std::map< std::string, llvm::FunctionCallee > functions;

        functions[ "print" ] = module_->getOrInsertFunction
        (
            "printf",
            llvm::FunctionType::get
            (
                llvm::IntegerType::getInt32Ty( *context ),
                llvm::PointerType::get( llvm::Type::getInt8Ty( *context ), 0 ),
                true
            )
        );

        return functions;
    }
} // namespace

Context codegen
(
    Node::Pointer    const & node,
    llvm::DataLayout const   dataLayout,
    std::string_view const   targetTriple
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

            Symbols symbols{ createStdFunctions( module_.get(), context.get() ) };

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
        node->is< NodeType >() && node->get< NodeType >() == NodeType::ModuleDefinition,
        "Module definition is the root node of the AST"
    );

    auto * mainFunctionType{ llvm::FunctionType::get( llvm::Type::getVoidTy( *ctx.internalCtx ), false ) };
    auto * mainFunction    { llvm::Function::Create( mainFunctionType, llvm::Function::ExternalLinkage, "main", *ctx.module_ ) };

    auto * mainBlock{ llvm::BasicBlock::Create( *ctx.internalCtx, "entry", mainFunction ) };
    ctx.builder->SetInsertPoint( mainBlock );

    auto inMainBlock{ true };

    // Generating LLVM IR for all the statements within the module
    for ( auto const & n : node->children() )
    {
        if ( n == nullptr ) { continue; }

        if
        (
            n->is< NodeType >() &&
            (
                n->get< NodeType >() == NodeType::ContractDefinition ||
                n->get< NodeType >() == NodeType::FunctionDefinition
            )
        )
        {
            inMainBlock = false;
            codegen( ctx, n );
        }
        else
        {
            if ( !inMainBlock )
            {
                // Getting back to main block
                ctx.builder->SetInsertPoint( mainBlock );
                inMainBlock = true;
            }
            codegen( ctx, n );
        }
    }

    ctx.builder->CreateRetVoid();
    return ctx;
}

} // namespace A1::LLVM