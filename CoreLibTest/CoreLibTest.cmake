include_guard()

# Fetch Google Test
include( FetchContent )

FetchContent_Declare(
    gtest
    GIT_REPOSITORY https://github.com/google/googletest
    GIT_TAG release-1.11.0
)

if( NOT gtest_POPULATED )
    FetchContent_Populate( gtest )
    add_subdirectory( ${gtest_SOURCE_DIR} ${gtest_BINARY_DIR} )
endif()

include( ${CMAKE_CURRENT_LIST_DIR}/../CoreLib/CoreLib.cmake )
include( ${CMAKE_CURRENT_LIST_DIR}/CoreLibTest.srcs.cmake )

add_executable( CoreLibTests ${SOURCES} )

target_link_libraries(
    CoreLibTests PRIVATE
    CoreLib
    GTest::gtest_main
)

# Include GoogleTest module for gtest_discover_tests function
include( GoogleTest )

enable_testing()

gtest_discover_tests( CoreLibTests )
