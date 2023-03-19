cmake_minimum_required( VERSION 3.10.0 )

project( Toolchain VERSION 0.1.0 )

set( CMAKE_CXX_STANDARD          20 )
set( CMAKE_CXX_STANDARD_REQUIRED ON )

set( CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib )
set( CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib )
set( CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin )

if( CMAKE_CXX_COMPILER_ID MATCHES "Clang" OR CMAKE_CXX_COMPILER_ID MATCHES "GNU" )
    set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Werror" )
    if ( CMAKE_CXX_COMPILER_ID MATCHES "GNU" )
        set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fuse-ld=gold -pthread" )
    endif()
endif()

include( FetchContent )

# Use fmt library until C++20 std::format is supported by all the compilers
FetchContent_Declare( fmt
    GIT_REPOSITORY https://github.com/fmtlib/fmt.git
    GIT_TAG 9.1.0
)
FetchContent_MakeAvailable( fmt )

include( ${CMAKE_CURRENT_LIST_DIR}/CoreLib/CoreLib.cmake )

option( ENABLE_TESTS "Enable tests" OFF )
message( STATUS "Tests enabled: ${ENABLE_TESTS}" )

if( ENABLE_TESTS )
    include( ${CMAKE_CURRENT_LIST_DIR}/CoreLibTest/CoreLibTest.cmake )
endif()

option( ENABLE_FUZZERS "Enable fuzzers" OFF )
message( STATUS "Fuzzers enabled: ${ENABLE_FUZZERS}" )

if( ENABLE_FUZZERS )
    include( ${CMAKE_CURRENT_LIST_DIR}/Fuzzer/Fuzzer.cmake )
endif()

include( ${CMAKE_CURRENT_LIST_DIR}/AOC/AOC.cmake )
