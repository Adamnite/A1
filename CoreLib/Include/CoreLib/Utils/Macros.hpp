/**
 *
 * Copyright (c)2022 The Adamnite C++ Authors.
 *
 * This code is open-sourced under the MIT license.
 */

#pragma once

#ifdef TESTS_ENABLED
#   include <cassert>
#endif // TESTS_ENABLED

namespace A1
{

#ifdef TESTS_ENABLED
#   define ASSERT( expression ) assert( expression )
#else
#   define ASSERT( expression ) ( void ) 0
#endif // ENABLED_TESTING

} // namespace A1