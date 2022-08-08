/**
 *
 * Copyright (c)2022 The Adamnite C++ Authors.
 *
 * This code is open-sourced under the MIT license.
 */

#include <CoreLib/Types.hpp>

#include "Utils/Utils.hpp"

namespace A1::Registry
{

namespace
{
    Type numberType;
    Type stringLiteralType;
} // namespace

TypeID getHandle( Type const & type ) noexcept
{
    return std::visit
    (
        Overload
        {
            []( PrimitiveType const t ) noexcept -> TypeID
            {
                switch ( t )
                {
                    case PrimitiveType::Number:        return getNumberHandle       ();
                    case PrimitiveType::StringLiteral: return getStringLiteralHandle();

                    case PrimitiveType::Unknown:
                    case PrimitiveType::Count:
                        return nullptr;
                }
            },
            []( ArrayType const ) noexcept -> TypeID
            {
                return nullptr;
            },
            []( FunctionType const ) noexcept -> TypeID
            {
                return nullptr;
            }
        },
        type
    );
}

TypeID getNumberHandle       () noexcept { return &numberType;        }
TypeID getStringLiteralHandle() noexcept { return &stringLiteralType; }

} // namespace A1::Registry