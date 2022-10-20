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

#if defined(__clang__)
#   pragma clang diagnostic pop
#elif defined(__GNUC__) || defined(__GNUG__)
#   pragma GCC diagnostic pop
#endif

namespace A1::LLVM::IR
{

llvm::Value * codegen( Context & ctx, Node::Pointer const & node )
{
    if ( node == nullptr ) { return nullptr; }

    return std::visit
    (
        Overload
        {
            [ &ctx, &node ]( NodeType const type ) -> llvm::Value *
            {
                switch ( type )
                {
#define CODEGEN( type, codegenFunc, builderFunc, opName ) \
    case NodeType::type: return codegenFunc( ctx, &llvm::IRBuilder<>::builderFunc, node->children(), opName )

                    CODEGEN( UnaryMinus       , codegenUnary , CreateFNeg      , "nottemp" );
                    CODEGEN( LogicalNot       , codegenUnary , CreateNot       , "lnottmp" );
                    CODEGEN( LogicalAnd       , codegenBinary, CreateLogicalAnd, "landtmp" );
                    CODEGEN( LogicalOr        , codegenBinary, CreateLogicalOr , "lortmp"  );
                    CODEGEN( Multiplication   , codegenBinary, CreateFMul      , "multmp"  );
                    CODEGEN( Division         , codegenBinary, CreateFDiv      , "divtmp"  );
                    CODEGEN( FloorDivision    , codegenBinary, CreateSDiv      , "fdivtmp" );
                    CODEGEN( Modulus          , codegenBinary, CreateFRem      , "modtmp"  );
                    CODEGEN( Addition         , codegenBinary, CreateFAdd      , "addtmp"  );
                    CODEGEN( Subtraction      , codegenBinary, CreateFSub      , "subtmp"  );
                    CODEGEN( BitwiseLeftShift , codegenBinary, CreateShl       , "lshtmp"  );
                    CODEGEN( BitwiseRightShift, codegenBinary, CreateAShr      , "rshtmp"  );
                    CODEGEN( BitwiseAnd       , codegenBinary, CreateAnd       , "andtmp"  );
                    CODEGEN( BitwiseOr        , codegenBinary, CreateOr        , "ortmp "  );
                    CODEGEN( BitwiseXor       , codegenBinary, CreateXor       , "xortmp"  );
                    CODEGEN( BitwiseNot       , codegenUnary , CreateNot       , "nottmp"  );
                    CODEGEN( Equality         , codegenBinary, CreateFCmpOEQ   , "eqtmp"   );
                    CODEGEN( Inequality       , codegenBinary, CreateFCmpONE   , "netmp"   );
                    CODEGEN( GreaterThan      , codegenBinary, CreateFCmpOGT   , "gttmp"   );
                    CODEGEN( GreaterThanEqual , codegenBinary, CreateFCmpOGE   , "getmp"   );
                    CODEGEN( LessThan         , codegenBinary, CreateFCmpOLT   , "ltmp"    );
                    CODEGEN( LessThanEqual    , codegenBinary, CreateFCmpOLE   , "letmp"   );
                    CODEGEN( IsIdentical      , codegenBinary, CreateFCmpOEQ   , "eqtmp"   );
                    CODEGEN( IsNotIdentical   , codegenBinary, CreateFCmpONE   , "netmp"   );

                    CODEGEN( AssignAddition         , codegenAssign, CreateFAdd, "addtmp"  );
                    CODEGEN( AssignSubtraction      , codegenAssign, CreateFSub, "subtmp"  );
                    CODEGEN( AssignMultiplication   , codegenAssign, CreateFMul, "multmp"  );
                    CODEGEN( AssignDivision         , codegenAssign, CreateFDiv, "divtmp"  );
                    CODEGEN( AssignFloorDivision    , codegenAssign, CreateSDiv, "fdivtmp" );
                    CODEGEN( AssignModulus          , codegenAssign, CreateFRem, "modtmp"  );
                    CODEGEN( AssignBitwiseLeftShift , codegenAssign, CreateShl , "shltmp"  );
                    CODEGEN( AssignBitwiseRightShift, codegenAssign, CreateAShr, "shrtmp"  );
                    CODEGEN( AssignBitwiseAnd       , codegenAssign, CreateAnd , "andtmp"  );
                    CODEGEN( AssignBitwiseOr        , codegenAssign, CreateOr  , "ortmp"   );
                    CODEGEN( AssignBitwiseXor       , codegenAssign, CreateXor , "xortmp"  );

#undef CODEGEN
                    case NodeType::Assign    : return codegenVariableDefinition( ctx, node->children() );
                    case NodeType::Call      : return codegenCall              ( ctx, node->children() );
                    case NodeType::MemberCall: return codegenMemberCall        ( ctx, node->children() );

                    case NodeType::StatementIf:
                    case NodeType::StatementElif:
                        return codegenControlFlow( ctx, node->children() );

                    case NodeType::StatementElse:
                    {
                        // Generate LLVM IR for all the statements in else body
                        llvm::Value * value{ nullptr };
                        for ( auto const & n : node->children() )
                        {
                            value = codegen( ctx, n );
                        }
                        return value;
                    }

                    case NodeType::StatementWhile:
                        return codegenLoopFlow( ctx, node->children() );

                    case NodeType::StatementReturn:
                    {
                        ASSERT( std::size( node->children() ) == 1U );
                        return codegen( ctx, node->children()[ 0U ] );
                    }

                    case NodeType::ContractDefinition: return codegenContractDefinition( ctx, node->children() );
                    case NodeType::FunctionDefinition: return codegenFunctionDefinition( ctx, node->children() );
                    case NodeType::VariableDefinition: return codegenVariableDefinition( ctx, node->children() );

                    default:
                        return nullptr;
                }
            },
            [ &ctx ]( Identifier const & identifier ) -> llvm::Value *
            {
                auto * value{ ctx.symbols.getVariable( identifier.name ) };
                if ( value == nullptr ) { return nullptr; }

                if ( value->getType()->getNumContainedTypes() > 0 && value->getType()->getContainedType( 0U )->isDoubleTy() )
                {
                    return ctx.builder->CreateLoad( llvm::Type::getDoubleTy( *ctx.internalCtx ), value );
                }
                return value;
            },
            [ &ctx ]( Number const number ) -> llvm::Value *
            {
                return llvm::ConstantFP::get( *ctx.internalCtx, llvm::APFloat( number ) );
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