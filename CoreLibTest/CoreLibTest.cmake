include_guard()

# Fetch Google Test
include( FetchContent )

FetchContent_Declare(
    gtest
    GIT_REPOSITORY https://github.com/google/googletest
    GIT_TAG release-1.11.0
)

# For Windows: Prevent overriding the parent project's compiler/linker settings
set( gtest_force_shared_crt ON CACHE BOOL "" FORCE )

FetchContent_MakeAvailable( gtest )

include( ${CMAKE_CURRENT_LIST_DIR}/../CoreLib/CoreLib.cmake )
include( ${CMAKE_CURRENT_LIST_DIR}/CoreLibTest.srcs.cmake   )

add_executable( CoreLibTests ${SOURCES} )

target_link_libraries(
    CoreLibTests PRIVATE
    CoreLib
    gtest_main
)

# Include GoogleTest module for gtest_discover_tests function
include( GoogleTest )

enable_testing()

gtest_discover_tests( CoreLibTests )
