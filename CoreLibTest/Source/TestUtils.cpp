/**
 *
 * Copyright (c)2022 The Adamnite C++ Authors.
 *
 * This code is open-sourced under the MIT license.
 */

#include "TestUtils.hpp"

namespace A1
{

testing::AssertionResult areEqual
(
    char const *,
    char const *,
    A1::ReservedToken const value1,
    A1::ReservedToken const value2
)
{
    if ( value1 == value2 ) { return testing::AssertionSuccess(); }

    return testing::AssertionFailure()
        << "Expected "                << A1::toStringView( value2 )
        << " reserved token but got " << A1::toStringView( value1 );
}

} // namespace A1