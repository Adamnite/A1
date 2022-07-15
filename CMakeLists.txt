cmake_minimum_required( VERSION 3.10.0 )

project( A1 VERSION 1.0.0 )

set( CMAKE_CXX_STANDARD          20 )
set( CMAKE_CXX_STANDARD_REQUIRED ON )

if( CMAKE_CXX_COMPILER_ID MATCHES "Clang" OR CMAKE_CXX_COMPILER_ID MATCHES "GNU" )
    set( CMAKE_CXX_FLAGS "-Wall -Wextra" )
endif()

include( ${CMAKE_CURRENT_LIST_DIR}/CoreLib/CoreLib.cmake )

option( CoreLib_ENABLED_TESTING "Enable testing" OFF )
message( "Testing enabled: ${CoreLib_ENABLED_TESTING}" )

if( CoreLib_ENABLED_TESTING )
    include( ${CMAKE_CURRENT_LIST_DIR}/CoreLibTest/CoreLibTest.cmake )
endif()
