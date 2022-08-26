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

include( ${CMAKE_CURRENT_LIST_DIR}/../CoreUtils/CoreUtils.cmake )
include( ${CMAKE_CURRENT_LIST_DIR}/CoreUtilsTest.srcs.cmake   )

add_executable( CoreUtilsTests ${SOURCES} )

target_link_libraries(
    CoreUtilsTests PRIVATE
    CoreUtils
    gtest_main
)

target_compile_definitions(
    CoreUtilsTests PRIVATE
    -DENABLED_TESTING
)

target_include_directories(
    CoreUtilsTests
    PRIVATE ${CMAKE_CURRENT_LIST_DIR}/Source
)

# Include GoogleTest module for gtest_discover_tests function
include( GoogleTest )

enable_testing()

gtest_discover_tests( CoreUtilsTests )
