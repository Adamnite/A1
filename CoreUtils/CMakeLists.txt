cmake_minimum_required( VERSION 3.10.0 )

project( CoreUtils VERSION 1.0.0 )

set( CMAKE_CXX_STANDARD          20 )
set( CMAKE_CXX_STANDARD_REQUIRED ON )

set( CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib )
set( CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib )
set( CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin )

if( CMAKE_CXX_COMPILER_ID MATCHES "Clang" )
    set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Werror" )
endif()

include( ${CMAKE_CURRENT_LIST_DIR}/Lib/Lib.cmake )

option( ENABLE_TESTS "Enable tests" OFF )
message( STATUS "Tests enabled: ${ENABLE_TESTS}" )

if( ENABLE_TESTS )
    include( ${CMAKE_CURRENT_LIST_DIR}/LibTest/LibTest.cmake )
endif()
