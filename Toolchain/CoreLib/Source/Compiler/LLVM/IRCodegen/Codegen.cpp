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
        auto * blockType{ llvm::StructType::create( *ctx, { llvm::Type::getInt32Ty( *ctx ) }, "Block" ) };
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

        functions[ "abort" ] = module_->getOrInsertFunction
        (
            "abort",
            llvm::FunctionType::get
            (
                llvm::Type::getVoidTy( *ctx ),
                true
            )
        );

        functions[ "is_utf8" ] = module_->getOrInsertFunction
        (
            "is_utf8",
            llvm::FunctionType::get
            (
                llvm::IntegerType::getInt32Ty( *ctx ),
                llvm::PointerType::get( llvm::Type::getInt8Ty( *ctx ), 0 ),
                false
            )
        );

        return functions;
    }
} // namespace

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
        else if ( node->is< AST::NodeType >() && node->get< AST::NodeType >() == AST::NodeType::VariableDefinition )
        {
            if ( !inMainBlock )
            {
                // Getting back to main block
                ctx.builder->SetInsertPoint( mainBlock );
                inMainBlock = true;
            }

            auto const & nodes{ node->children() };

            ASSERTM( std::size( nodes ) >= 2U, "Variable definition consists of an identifier and either type annotation or initialization or both" );

            ASSERTM( nodes[ 0U ]->is< Identifier >(), "Variable identifier is the first child node in the variable definition" );
            auto const & name{ nodes[ 0U ]->get< Identifier >().name };

            ctx.symbols.variables[ ctx.symbols.mangle( name ) ] = codegen( ctx, node );
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
     * In production, when source code is built to WASM, main function
     * should be left out.
     */
    mainFunction->eraseFromParent();
#endif // TESTS_ENABLED

    return ctx;
}

} // namespace A1::LLVM::IR