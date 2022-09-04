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

set( BUILD_GMOCK OFF CACHE BOOL "" FORCE )
set( BUILD_GTEST ON  CACHE BOOL "" FORCE )

FetchContent_MakeAvailable( gtest )

include( ${CMAKE_CURRENT_LIST_DIR}/../../CoreLib/CoreLib.cmake )
include( ${CMAKE_CURRENT_LIST_DIR}/CoreLibTest.srcs.cmake      )

add_executable( CoreLibTests ${SOURCES} )

target_link_libraries(
    CoreLibTests PRIVATE
    CoreLib
    gtest_main
)

target_compile_definitions(
    CoreLibTests PRIVATE
    -DENABLE_TESTING
)

target_include_directories(
    CoreLibTests
    PRIVATE ${CMAKE_CURRENT_LIST_DIR}/Source
)

# Include GoogleTest module for gtest_discover_tests function
include( GoogleTest )

enable_testing()

gtest_discover_tests( CoreLibTests )
