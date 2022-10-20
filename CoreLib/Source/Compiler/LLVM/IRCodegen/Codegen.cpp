/**
 *
 * Copyright (c)2022 The Adamnite C++ Authors.
 *
 * This code is open-sourced under the MIT license.
 */

#include "Codegen.hpp"
#include "CodegenVisitor.hpp"

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

#include <string>
#include <memory>
#include <map>

namespace A1::LLVM::IR
{

namespace
{
    [[ nodiscard ]]
    llvm::Value * createAddress( llvm::Module * module_, llvm::IRBuilder<> * builder, std::string_view const name )
    {
        // TODO: Make an ADVM API call to get the actual address
        return builder->CreateGlobalStringPtr( "", name, 0U, module_ );
    }

    [[ nodiscard ]]
    llvm::Value * createBlock( llvm::Module * module_, llvm::LLVMContext * ctx, std::string_view const name )
    {
        auto * blockType{ llvm::StructType::create( *ctx, { llvm::Type::getInt64Ty( *ctx ) }, "Block" ) };
        auto * block
        {
            new llvm::GlobalVariable
            (
                *module_,
                blockType,
                true, /* isConstant */
                llvm::GlobalVariable::ExternalLinkage,
                llvm::ConstantStruct::get
                (
                    blockType,
                    // Initial data members
                    {
                        // TODO: Make an ADVM API call to get the actual initial timestamp
                        llvm::ConstantInt::get( llvm::Type::getInt32Ty( *ctx ), 0U, false /* isSigned */ )
                    }
                ),
                name
            )
        };
        return block;
    }

    [[ nodiscard ]]
    llvm::Value * createMessage( llvm::Module * module_, llvm::IRBuilder<> * builder, llvm::LLVMContext * ctx, std::string_view const name )
    {
        auto * messageType{ llvm::StructType::create( *ctx, { llvm::Type::getInt8PtrTy( *ctx ) }, "Message" ) };
        auto * message
        {
            new llvm::GlobalVariable
            (
                *module_,
                messageType,
                true, /* isConstant */
                llvm::GlobalVariable::ExternalLinkage,
                llvm::ConstantStruct::get
                (
                    messageType,
                    // Initial data members
                    {
                        // TODO: Make an ADVM API call to get the actual sender
                        builder->CreateGlobalStringPtr( "", "msg.sender", 0U, module_ )
                    }
                ),
                name
            )
        };
        return message;
    }

    [[ nodiscard ]]
    std::map< std::string, llvm::FunctionCallee > createBuiltInFunctions( llvm::Module * module_, llvm::LLVMContext * ctx )
    {
        std::map< std::string, llvm::FunctionCallee > functions;

        functions[ "print" ] = module_->getOrInsertFunction
        (
            "printf",
            llvm::FunctionType::get
            (
                llvm::IntegerType::getInt32Ty( *ctx ),
                llvm::PointerType::get( llvm::Type::getInt8Ty( *ctx ), 0 ),
                true
            )
        );

        return functions;
    }
} // namespace

Context codegen
(
    Node::Pointer    const & moduleNode,
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

            Symbols symbols
            {
                // Built-in global variables
                {
                    { "address", createAddress( module_.get(), builder.get(), "address" ) },
                    { "block"  , createBlock  ( module_.get(), context.get(), "block"   ) },
                    { "msg"    , createMessage( module_.get(), builder.get(), context.get(), "msg" ) }
                },
                createBuiltInFunctions( module_.get(), context.get() )
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
        moduleNode->is< NodeType >() && moduleNode->get< NodeType >() == NodeType::ModuleDefinition,
        "Module definition is the root node of the AST"
    );

    auto * mainFunctionType{ llvm::FunctionType::get( llvm::Type::getVoidTy( *ctx.internalCtx ), false ) };
    auto * mainFunction    { llvm::Function::Create( mainFunctionType, llvm::Function::ExternalLinkage, "main", *ctx.module_ ) };

    auto * mainBlock{ llvm::BasicBlock::Create( *ctx.internalCtx, "entry", mainFunction ) };
    ctx.builder->SetInsertPoint( mainBlock );

    auto inMainBlock{ true };

    // Generating LLVM IR for all the statements within the module
    for ( auto const & node : moduleNode->children() )
    {
        if ( node == nullptr ) { continue; }
        if
        (
            node->is< NodeType >() &&
            (
                node->get< NodeType >() == NodeType::ContractDefinition ||
                node->get< NodeType >() == NodeType::FunctionDefinition
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

    ctx.builder->CreateRetVoid();
    return ctx;
}

} // namespace A1::LLVM::IR