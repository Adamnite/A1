/**
 *
 * Copyright (c)2022 The Adamnite C++ Authors.
 *
 * This code is open-sourced under the MIT license.
 */

#include <CoreLib/Compiler/LLVM/Codegen.hpp>

#include "Utils/Utils.hpp"

#if defined (__clang__)
#   pragma clang diagnostic push
#   pragma clang diagnostic ignored "-Wunused-parameter"
#elif defined(__GNUC__) || defined(__GNUG__)
#   pragma GCC diagnostic push
#   pragma GCC diagnostic ignored "-Wunused-parameter"
#endif

#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Value.h>

#if defined(__clang__)
#   pragma clang diagnostic pop
#elif defined(__GNUC__) || defined(__GNUG__)
#   pragma GCC diagnostic pop
#endif

namespace A1::LLVM
{

namespace
{
    std::unique_ptr< llvm::LLVMContext > context;
    std::unique_ptr< llvm::IRBuilder<> > builder;
    std::unique_ptr< llvm::Module      > module_;

    llvm::Value * codegenImpl( Node::Pointer const & node )
    {
        return std::visit
        (
            Overload
            {
                [ & ]( NodeType const ) -> llvm::Value *
                {
                    return codegenImpl( node );
                },
                []( Identifier const & ) -> llvm::Value *
                {
                    return nullptr;
                },
                []( Number const number ) -> llvm::Value *
                {
                    return llvm::ConstantFP::get( *context, llvm::APFloat( number ) );
                },
                []( String const & str ) -> llvm::Value *
                {
                    return llvm::ConstantDataArray::getString( *context, str.data() );
                },
                []( TypeID const ) -> llvm::Value *
                {
                    return nullptr;
                }
            },
            node->value()
        );
    }
} // namespace

std::unique_ptr< Module > codegen
(
    Node::Pointer    const & node,
    llvm::DataLayout const   dataLayout,
    std::string_view const   targetTriple
)
{
    context = std::make_unique< llvm::LLVMContext >();
    builder = std::make_unique< llvm::IRBuilder<> >( *context );
    module_ = std::make_unique< llvm::Module >( "Module", *context );

    /**
     * Optimizations benefit from knowing about the target and data layout.
     */
    module_->setDataLayout  ( dataLayout   );
    module_->setTargetTriple( targetTriple );

    codegenImpl( node );

    return std::move( module_ );
}

} // namespace A1::LLVM