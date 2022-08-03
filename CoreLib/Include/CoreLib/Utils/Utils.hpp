/**
 *
 * Copyright (c)2022 The Adamnite C++ Authors.
 *
 * This code is open-sourced under the MIT license.
 */

#pragma once

namespace A1
{

template< typename ... Ts > struct Overload : Ts... { using Ts::operator()...; };
template< typename ... Ts > Overload( Ts ... ) -> overload< Ts ... >;

} // namespace A1