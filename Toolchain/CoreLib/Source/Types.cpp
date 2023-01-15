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
    Type address;

    Type bool_;
    Type num;
    Type str;

    Type i8, i16, i32, i64;
    Type u8, u16, u32, u64;
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
                    case PrimitiveType::Address: return getAddressHandle();

                    case PrimitiveType::Bool: return getBoolHandle();
                    case PrimitiveType::Num : return getNumHandle ();
                    case PrimitiveType::Str : return getStrHandle ();

                    case PrimitiveType::I8 : return getI8Handle ();
                    case PrimitiveType::I16: return getI16Handle();
                    case PrimitiveType::I32: return getI32Handle();
                    case PrimitiveType::I64: return getI64Handle();

                    case PrimitiveType::U8 : return getU8Handle ();
                    case PrimitiveType::U16: return getU16Handle();
                    case PrimitiveType::U32: return getU32Handle();
                    case PrimitiveType::U64: return getU64Handle();

                    case PrimitiveType::Unknown:
                    case PrimitiveType::Count:
                        return nullptr;
                }

                return nullptr;
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

TypeID getAddressHandle() noexcept { return &address; }

TypeID getBoolHandle() noexcept { return &bool_; }
TypeID getNumHandle () noexcept { return &num;   }
TypeID getStrHandle () noexcept { return &str;   }

TypeID getI8Handle () noexcept { return &i8;  }
TypeID getI16Handle() noexcept { return &i16; }
TypeID getI32Handle() noexcept { return &i32; }
TypeID getI64Handle() noexcept { return &i64; }

TypeID getU8Handle () noexcept { return &u8;  }
TypeID getU16Handle() noexcept { return &u16; }
TypeID getU32Handle() noexcept { return &u32; }
TypeID getU64Handle() noexcept { return &u64; }

} // namespace A1::Registry