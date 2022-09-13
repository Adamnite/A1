cmake_minimum_required( VERSION 3.10.0 )

project( A1 VERSION 1.0.0 )

set( CMAKE_CXX_STANDARD          20 )
set( CMAKE_CXX_STANDARD_REQUIRED ON )

set( CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib )
set( CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib )
set( CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin )

if( CMAKE_CXX_COMPILER_ID MATCHES "Clang" OR CMAKE_CXX_COMPILER_ID MATCHES "GNU" )
    set( CMAKE_CXX_FLAGS "-Wall -Wextra -Werror" )
endif()

include( ${CMAKE_CURRENT_LIST_DIR}/CoreLib/CoreLib.cmake )
include( ${CMAKE_CURRENT_LIST_DIR}/CoreUtils/CoreUtils.cmake )

option( ENABLE_TESTING "Enable testing" OFF )
message( STATUS "Testing enabled: ${ENABLE_TESTING}" )

if( ENABLE_TESTING )
    include( ${CMAKE_CURRENT_LIST_DIR}/Test/Test.cmake )
endif()
