/**
 *
 * Copyright (c)2022 The Adamnite C++ Authors.
 *
 * This code is open-sourced under the MIT license.
 */

#include "ASTPrinter.hpp"
#include "Utils/Utils.hpp"

#include <fmt/format.h>

#include <string_view>

namespace A1::AST
{

namespace
{
    [[ nodiscard ]]
    std::string_view toString( TypeID const id ) noexcept
    {
             if ( id == Registry::getAddressHandle() ) { return "address"; }
        else if ( id == Registry::getBoolHandle   () ) { return "bool";    }
        else if ( id == Registry::getNumHandle    () ) { return "num";     }
        else if ( id == Registry::getStrHandle    () ) { return "str";     }
        else if ( id == Registry::getI8Handle     () ) { return "i8";      }
        else if ( id == Registry::getI16Handle    () ) { return "i16";     }
        else if ( id == Registry::getI32Handle    () ) { return "i32";     }
        else if ( id == Registry::getI64Handle    () ) { return "i64";     }
        else if ( id == Registry::getU8Handle     () ) { return "u8";      }
        else if ( id == Registry::getU16Handle    () ) { return "u16";     }
        else if ( id == Registry::getU32Handle    () ) { return "u32";     }
        else if ( id == Registry::getU64Handle    () ) { return "u64";     }

        return "null";
    }

    [[ nodiscard ]]
    constexpr std::string_view toString( NodeType const type ) noexcept
    {
        switch ( type )
        {
#define STR_CASE( x ) case NodeType::x: return #x
            STR_CASE( Unknown                     );
            STR_CASE( Call                        );
            STR_CASE( Parentheses                 );
            STR_CASE( Index                       );
            STR_CASE( MemberCall                  );
            STR_CASE( Exponent                    );
            STR_CASE( UnaryPlus                   );
            STR_CASE( UnaryMinus                  );
            STR_CASE( Multiplication              );
            STR_CASE( Division                    );
            STR_CASE( FloorDivision               );
            STR_CASE( Modulus                     );
            STR_CASE( Addition                    );
            STR_CASE( Subtraction                 );
            STR_CASE( BitwiseLeftShift            );
            STR_CASE( BitwiseRightShift           );
            STR_CASE( BitwiseAnd                  );
            STR_CASE( BitwiseOr                   );
            STR_CASE( BitwiseXor                  );
            STR_CASE( BitwiseNot                  );
            STR_CASE( Equality                    );
            STR_CASE( Inequality                  );
            STR_CASE( GreaterThan                 );
            STR_CASE( GreaterThanEqual            );
            STR_CASE( LessThan                    );
            STR_CASE( LessThanEqual               );
            STR_CASE( IsIdentical                 );
            STR_CASE( IsNotIdentical              );
            STR_CASE( IsMemberOf                  );
            STR_CASE( IsNotMemberOf               );
            STR_CASE( LogicalNot                  );
            STR_CASE( LogicalAnd                  );
            STR_CASE( LogicalOr                   );
            STR_CASE( Assign                      );
            STR_CASE( AssignExponent              );
            STR_CASE( AssignAddition              );
            STR_CASE( AssignSubtraction           );
            STR_CASE( AssignMultiplication        );
            STR_CASE( AssignDivision              );
            STR_CASE( AssignFloorDivision         );
            STR_CASE( AssignModulus               );
            STR_CASE( AssignBitwiseLeftShift      );
            STR_CASE( AssignBitwiseRightShift     );
            STR_CASE( AssignBitwiseAnd            );
            STR_CASE( AssignBitwiseOr             );
            STR_CASE( AssignBitwiseXor            );
            STR_CASE( StatementIf                 );
            STR_CASE( StatementElif               );
            STR_CASE( StatementElse               );
            STR_CASE( StatementWhile              );
            STR_CASE( StatementPass               );
            STR_CASE( StatementReturn             );
            STR_CASE( StatementImport             );
            STR_CASE( StatementAssert             );
            STR_CASE( ModuleDefinition            );
            STR_CASE( ContractDefinition          );
            STR_CASE( FunctionDefinition          );
            STR_CASE( FunctionParameterDefinition );
            STR_CASE( VariableDefinition          );
#undef STR_CASE

            case NodeType::Count:
                break;
        }

        ASSERT( false );
        return "";
    }
} // namespace

void print( Node::Pointer const & node, std::FILE * stream, std::size_t const indentationLevel )
{
    static auto printIndentation
    {
        []( std::FILE * stream, std::size_t const level )
        {
            fmt::print( stream, "{:{}}", "", level );
        }
    };

    if ( node == nullptr ) { return; }

    std::visit
    (
        Overload
        {
            [ =, &node ]( NodeType const type )
            {
                printIndentation( stream, indentationLevel );
                fmt::print( stream, "{}\n", toString( type ) );
                for ( auto const & n : node->children() )
                {
                    print( n, stream, indentationLevel + 2U );
                }
            },
            [ = ]( Identifier const & identifier )
            {
                printIndentation( stream, indentationLevel );
                fmt::print( stream, "Identifier = '{}'\n", identifier.name );
            },
            [ = ]( Boolean const boolean )
            {
                printIndentation( stream, indentationLevel );
                fmt::print( stream, "Boolean = '{}'\n", boolean ? "true" : "false" );
            },
            [ = ]( Number const number )
            {
                printIndentation( stream, indentationLevel );
                fmt::print( stream, "Number = '{}'\n", number );
            },
            [ = ]( String const & str )
            {
                printIndentation( stream, indentationLevel );
                fmt::print( stream, "String = '{}'\n", str );
            },
            [ = ]( TypeID const typeID )
            {
                printIndentation( stream, indentationLevel );
                fmt::print( stream, "TypeID = {}\n", toString( typeID ) );
            }
        },
        node->value()
    );
}

} // namespace A1::AST