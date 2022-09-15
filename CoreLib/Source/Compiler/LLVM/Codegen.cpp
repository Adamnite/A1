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
                [ & ]( NodeType const type ) -> llvm::Value *
                {
                    switch ( type ){
                        case NodeType::Multiplication:{
                            auto const & children{node -> children()};
                            auto * lhs{ codegenImpl( children[ 0U ] ) };
                            auto * rhs{ codegenImpl( children[ 1U ] ) };

                            return builder -> CreateFMul(lhs, rhs, "multmp");
                        }
                        case NodeType::Division:{
                            auto const & children{node -> children()};
                            auto * lhs{ codegenImpl( children[ 0U ] ) };
                            auto * rhs{ codegenImpl( children[ 1U ] ) };

                            return builder -> CreateFDiv(lhs, rhs, "divtmp");
                        }
                        case NodeType::FloorDivision:{
                            auto const & children{node -> children()};
                            auto * lhs{ codegenImpl( children[ 0U ] ) };
                            auto * rhs{ codegenImpl( children[ 1U ] ) };

                            return builder -> CreateSDiv(lhs, rhs, "fdivtmp");
                        }
                        case NodeType::Modulus:{
                            auto const & children{node -> children()};
                            auto * lhs{ codegenImpl( children[ 0U ] ) };
                            auto * rhs{ codegenImpl( children[ 1U ] ) };

                            return builder -> CreateFRem(lhs, rhs, "modtmp");
                        }

                        case NodeType::Addition:{
                            auto const & children{ node->children() };
                            // TODO: Add check if there are 2 childs, so that we don't get out of bounds exception here
                            auto * lhs{ codegenImpl( children[ 0U ] ) };
                            auto * rhs{ codegenImpl( children[ 1U ] ) };

                            return builder->CreateFAdd( lhs, rhs, "addtmp" );
                        }
                        case NodeType::Subtraction:{
                            auto const & children{node->children()};
                            // TODO: Add check if there are 2 childs, so that we don't get out of bounds exception here
                            auto * lhs{ codegenImpl( children[ 0U ] ) };
                            auto * rhs{ codegenImpl( children[ 1U ] ) };
                            return builder->CreateFSub(lhs, rhs, "subtmp");
                        }

                        case NodeType::BitwiseLeftShift:{
                            auto const & children{node->children()};
                            // TODO: Add check if there are 2 childs, so that we don't get out of bounds exception here
                            auto * lhs{ codegenImpl( children[ 0U ] ) };
                            auto * rhs{ codegenImpl( children[ 1U ] ) };
                            return builder->CreateShl(lhs, rhs, "lstmp");
                        }
                        case NodeType::BitwiseRightShift:{
                            auto const & children{node->children()};
                            // TODO: Add check if there are 2 childs, so that we don't get out of bounds exception here
                            auto * lhs{ codegenImpl( children[ 0U ] ) };
                            auto * rhs{ codegenImpl( children[ 1U ] ) };
                            return builder->CreateAShr(lhs, rhs, "rstmp");//TODO: CHECK THIS OUT! 
                        }
                        case NodeType::BitwiseAnd:{
                            auto const & children{node->children()};
                            auto * lhs{ codegenImpl( children[ 0U ] ) };
                            auto * rhs{ codegenImpl( children[ 1U ] ) };
                            return builder->CreateAnd(lhs, rhs, "andtmp");
                        }
                        case NodeType::BitwiseOr:{
                            auto const & children{node->children()};
                            // TODO: Add check if there are 2 childs, so that we don't get out of bounds exception here
                            auto * lhs{ codegenImpl( children[ 0U ] ) };
                            auto * rhs{ codegenImpl( children[ 1U ] ) };
                            return builder->CreateOr(lhs, rhs, "ortmp");
                        }
                        case NodeType::BitwiseXor:{
                            auto const & children{node->children()};
                            // TODO: Add check if there are 2 childs, so that we don't get out of bounds exception here
                            auto * lhs{ codegenImpl( children[ 0U ] ) };
                            auto * rhs{ codegenImpl( children[ 1U ] ) };
                            return builder->CreateXor(lhs, rhs, "xortmp");
                        }
                        case NodeType::BitwiseNot:{
                            auto const & children{node->children()};
                            auto * lhs{ codegenImpl( children[ 0U ] ) };
                            return builder->CreateNot(lhs, "nottmp");
                        }

                        case NodeType::Equality:{
                            auto const & children{node->children()};
                            auto * lhs{ codegenImpl( children[ 0U ] ) };
                            auto * rhs{ codegenImpl( children[ 1U ] ) };
                            return builder->CreateICmpEQ(lhs, rhs, "eqtmp");//TODO: check this is the correct EQ
                        }
                        case NodeType::Inequality:{
                            auto const & children{node->children()};
                            auto * lhs{ codegenImpl( children[ 0U ] ) };
                            auto * rhs{ codegenImpl( children[ 1U ] ) };
                            return builder->CreateNot(builder->CreateICmpEQ(lhs, rhs, "ieqtmp"));//TODO: check this is a valid statement
                        }
                        
                        default:
                            return nullptr;
                    }
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