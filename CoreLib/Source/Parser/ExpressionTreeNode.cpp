/**
 *
 * Copyright (c)2022 The Adamnite C++ Authors.
 *
 * This code is open-sourced under the MIT license.
 */

#include <CoreLib/Parser/ExpressionTree.hpp>
#include <CoreLib/Utils/Utils.hpp>
#include <CoreLib/Types.hpp>

namespace A1
{

namespace
{
    TypeID getTypeID( Node::ValueType const & value ) noexcept
    {
        return std::visit
        (
            Overload
            {
                []( Identifier    const & ) noexcept -> TypeID { return nullptr; /* TODO */                 },
                []( OperatorType  const   ) noexcept -> TypeID { return nullptr; /* TODO */                 },
                []( std::string   const & ) noexcept -> TypeID { return Registry::getNumberHandle       (); },
                []( double        const   ) noexcept -> TypeID { return Registry::getStringLiteralHandle(); }
            },
            value
        );
    }
} // namespace

Node::Node
(
    ValueType                    value,
    std::vector< Pointer >       children,
    std::size_t            const lineNumber,
    std::size_t            const charIndex
) : value_     { std::move( value    ) }
  , children_  { std::move( children ) }
  , typeID_    { getTypeID( value_ )   }
  , lineNumber_{ lineNumber            }
  , charIndex_ { charIndex             }
{}

} // namespace A1