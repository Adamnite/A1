/**
 *
 * Copyright (c)2022 The Adamnite C++ Authors.
 *
 * This code is open-sourced under the MIT license.
 */

#pragma once

namespace A1
{

#ifdef ENABLED_TESTING
#   define ASSERT( expression ) assert( expression )
#else
#   define ASSERT( expression ) ( void ) 0
#endif // ENABLED_TESTING

} // namespace A1