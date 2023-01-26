/**
 *
 * Copyright (c)2022 The Adamnite C++ Authors.
 *
 * This code is open-sourced under the MIT license.
 */

#include "CodegenVisitor.hpp"
#include "CodegenExpression.hpp"
#include "Utils/Utils.hpp"

#if defined (__clang__)
#   pragma clang diagnostic push
#   pragma clang diagnostic ignored "-Wunused-parameter"
#elif defined(__GNUC__) || defined(__GNUG__)
#   pragma GCC diagnostic push
#   pragma GCC diagnostic ignored "-Wunused-parameter"
#endif

#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Value.h>

#if defined(__clang__)
#   pragma clang diagnostic pop
#elif defined(__GNUC__) || defined(__GNUG__)
#   pragma GCC diagnostic pop
#endif

namespace A1::LLVM::IR
{

llvm::Value * codegen( Context & ctx, AST::Node::Pointer const & node )
{
    if ( node == nullptr ) { return nullptr; }

    return std::visit
    (
        Overload
        {
            [ &ctx, &node ]( AST::NodeType const type ) -> llvm::Value *
            {
                switch ( type )
                {
#define CODEGEN( type, codegenFunc, builderFunc, opName ) \
    case AST::NodeType::type: return codegenFunc( ctx, &llvm::IRBuilder<>::builderFunc, node->children(), opName )

                    CODEGEN( UnaryMinus       , codegenUnary , CreateNeg       , "nottemp" );
                    CODEGEN( LogicalNot       , codegenUnary , CreateNot       , "lnottmp" );
                    CODEGEN( LogicalAnd       , codegenBinary, CreateLogicalAnd, "landtmp" );
                    CODEGEN( LogicalOr        , codegenBinary, CreateLogicalOr , "lortmp"  );
                    CODEGEN( Multiplication   , codegenBinary, CreateMul       , "multmp"  );
                    CODEGEN( Division         , codegenBinary, CreateSDiv      , "divtmp"  );
                    CODEGEN( FloorDivision    , codegenBinary, CreateSDiv      , "fdivtmp" );
                    CODEGEN( Modulus          , codegenBinary, CreateSRem      , "modtmp"  );
                    CODEGEN( Addition         , codegenBinary, CreateAdd       , "addtmp"  );
                    CODEGEN( Subtraction      , codegenBinary, CreateSub       , "subtmp"  );
                    CODEGEN( BitwiseLeftShift , codegenBinary, CreateShl       , "lshtmp"  );
                    CODEGEN( BitwiseRightShift, codegenBinary, CreateAShr      , "rshtmp"  );
                    CODEGEN( BitwiseAnd       , codegenBinary, CreateAnd       , "andtmp"  );
                    CODEGEN( BitwiseOr        , codegenBinary, CreateOr        , "ortmp "  );
                    CODEGEN( BitwiseXor       , codegenBinary, CreateXor       , "xortmp"  );
                    CODEGEN( BitwiseNot       , codegenUnary , CreateNot       , "nottmp"  );
                    CODEGEN( Equality         , codegenBinary, CreateICmpEQ    , "eqtmp"   );
                    CODEGEN( Inequality       , codegenBinary, CreateICmpNE    , "netmp"   );
                    CODEGEN( GreaterThan      , codegenBinary, CreateICmpUGT   , "gttmp"   );
                    CODEGEN( GreaterThanEqual , codegenBinary, CreateICmpUGE   , "getmp"   );
                    CODEGEN( LessThan         , codegenBinary, CreateICmpULT   , "ltmp"    );
                    CODEGEN( LessThanEqual    , codegenBinary, CreateICmpULE   , "letmp"   );
                    CODEGEN( IsIdentical      , codegenBinary, CreateICmpEQ    , "eqtmp"   );
                    CODEGEN( IsNotIdentical   , codegenBinary, CreateICmpNE    , "netmp"   );

                    CODEGEN( AssignAddition         , codegenAssign, CreateAdd , "addtmp"  );
                    CODEGEN( AssignSubtraction      , codegenAssign, CreateSub , "subtmp"  );
                    CODEGEN( AssignMultiplication   , codegenAssign, CreateMul , "multmp"  );
                    CODEGEN( AssignDivision         , codegenAssign, CreateUDiv, "divtmp"  );
                    CODEGEN( AssignFloorDivision    , codegenAssign, CreateUDiv, "fdivtmp" );
                    CODEGEN( AssignModulus          , codegenAssign, CreateURem, "modtmp"  );
                    CODEGEN( AssignBitwiseLeftShift , codegenAssign, CreateShl , "shltmp"  );
                    CODEGEN( AssignBitwiseRightShift, codegenAssign, CreateAShr, "shrtmp"  );
                    CODEGEN( AssignBitwiseAnd       , codegenAssign, CreateAnd , "andtmp"  );
                    CODEGEN( AssignBitwiseOr        , codegenAssign, CreateOr  , "ortmp"   );
                    CODEGEN( AssignBitwiseXor       , codegenAssign, CreateXor , "xortmp"  );

#undef CODEGEN
                    case AST::NodeType::Assign     : return codegenAssign    ( ctx, node->children() );
                    case AST::NodeType::Call       : return codegenCall      ( ctx, node->children() );
                    case AST::NodeType::MemberCall : return codegenMemberCall( ctx, node->children() );
                    case AST::NodeType::Parentheses:
                    {
                        ASSERT( std::size( node->children() ) == 1U );
                        return codegen( ctx, node->children()[ 0U ] );
                    }
                    case AST::NodeType::Index:
                    {
                        auto const & nodes{ node->children() };
                        ASSERT( std::size( nodes ) == 2U );
                        ASSERTM( nodes[ 0U ]->is< Identifier >(), "Identifier is the first child node in the subscript operator expression" );

                        auto const & internalBuiltInFunctions{ ctx.symbols.internalBuiltInFunctions() };

                        auto const & name{ nodes[ 0U ]->get< Identifier >().name };
                        if ( auto it{ internalBuiltInFunctions.find( name ) }; it != std::end( internalBuiltInFunctions ) )
                        {
                            return ctx.builder->CreateCall( internalBuiltInFunctions.at( name ), { codegen( ctx, nodes[ 1U ] ) } );
                        }

                        /**
                         * TODO: Implement subscript operator for user-defined maps.
                         */
                        return nullptr;
                    }
                    case AST::NodeType::StatementIf:
                    case AST::NodeType::StatementElif:
                    {
                        return codegenControlFlow( ctx, node->children() );
                    }
                    case AST::NodeType::StatementElse:
                    {
                        // Generate LLVM IR for all the statements in else body
                        llvm::Value * value{ nullptr };
                        for ( auto const & n : node->children() )
                        {
                            value = codegen( ctx, n );
                        }
                        return value;
                    }
                    case AST::NodeType::StatementWhile:
                    {
                        return codegenLoopFlow( ctx, node->children() );
                    }
                    case AST::NodeType::StatementReturn:
                    {
                        ASSERT( std::size( node->children() ) == 1U );
                        auto * value{ codegen( ctx, node->children()[ 0U ] ) };
                        return Detail::load( ctx, value );
                    }
                    case AST::NodeType::StatementImport:
                    {
                        auto const & nodes{ node->children() };
                        ASSERT( std::size( nodes ) == 1U );
                        ASSERTM( nodes[ 0U ]->is< Identifier >(), "Module identifier is the first and only child node in the import statement" );

                        ctx.importedModules.push_back( nodes[ 0U ]->get< Identifier >().name );
                        return nullptr;
                    }
                    case AST::NodeType::StatementAssert   : return codegenAssert            ( ctx, node->children() );
                    case AST::NodeType::ContractDefinition: return codegenContractDefinition( ctx, node->children() );
                    case AST::NodeType::FunctionDefinition: return codegenFunctionDefinition( ctx, node->children() );
                    case AST::NodeType::VariableDefinition: return codegenVariableDefinition( ctx, node->children() );

                    default:
                        return nullptr;
                }
            },
            [ &ctx ]( Identifier const & identifier ) -> llvm::Value *
            {
                return ctx.symbols.variable( identifier.name );
            },
            [ &ctx ]( Number const number ) -> llvm::Value *
            {
                return llvm::ConstantInt::get( *ctx.internalCtx, llvm::APInt( sizeof( Number ) * 8U /* numBits */, number, false /* isSigned */ ) );
            },
            [ &ctx ]( String const & str ) -> llvm::Value *
            {
                return ctx.builder->CreateGlobalStringPtr( str, "", 0U, ctx.module_.get() );
            },
            []( TypeID const ) -> llvm::Value *
            {
                return nullptr;
            }
        },
        node->value()
    );
}

} // namespace A1::LLVM::IR