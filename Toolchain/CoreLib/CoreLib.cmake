include_guard()

option( ENABLE_LLVM "Enable LLVM" ON )
message( STATUS "LLVM enabled: ${ENABLE_LLVM}" )

# WASM specific options
option( WASM_SYSROOT_PATH         "Path to WASM sysroot (unused if tests are enabled)"                "" )
option( WASM_RUNTIME_LIBRARY_PATH "Path to static WASM runtime library (unused if tests are enabled)" "" )

# WASM external libraries
option( WASM_UTILS_LIBRARY_PATH "Path to static WASM utils library (unused if tests are enabled)" "" )

message( STATUS "WASM sysroot path:         ${WASM_SYSROOT_PATH}"         )
message( STATUS "WASM runtime library path: ${WASM_RUNTIME_LIBRARY_PATH}" )
message( STATUS "WASM utils library path:   ${WASM_UTILS_LIBRARY_PATH}"   )

include( ${CMAKE_CURRENT_LIST_DIR}/CoreLib.srcs.cmake )

add_library( CoreLib STATIC ${SOURCES} )

include( ${CMAKE_CURRENT_LIST_DIR}/../../CMake/Common.cmake )

if( CMAKE_BUILD_TYPE STREQUAL "Debug" )
    enable_sanitizers( CoreLib )
endif()

target_include_directories(
    CoreLib
    PUBLIC  ${CMAKE_CURRENT_LIST_DIR}/Include
    PRIVATE ${CMAKE_CURRENT_LIST_DIR}/Source
)

target_link_libraries( CoreLib PRIVATE fmt::fmt )

if( ENABLE_TESTS )
    target_compile_definitions( CoreLib PRIVATE TESTS_ENABLED=1 )
else()
    target_compile_definitions( CoreLib PRIVATE
        WASM_SYSROOT_PATH=\"${WASM_SYSROOT_PATH}\"
        WASM_RUNTIME_LIBRARY_PATH=\"${WASM_RUNTIME_LIBRARY_PATH}\"
        WASM_UTILS_LIBRARY_PATH=\"${WASM_UTILS_LIBRARY_PATH}\"
    )
endif()

if( ENABLE_LLVM )
    find_package( LLVM  REQUIRED CONFIG )
    find_package( Clang REQUIRED CONFIG )

    message( STATUS "LLVM version: ${LLVM_PACKAGE_VERSION}" )

    target_include_directories( CoreLib AFTER PRIVATE ${LLVM_INCLUDE_DIRS} )
    add_definitions( ${LLVM_DEFINITIONS} )

    target_link_libraries( CoreLib PRIVATE
        ${LLVM_AVAILABLE_LIBS}
        clangTooling
    )

    # In order to compile .ao source files programmatically,
    # we need to use the Clang compiler coming with the LLVM package.
    target_compile_definitions( CoreLib PRIVATE
        CLANG_PATH=\"${LLVM_INSTALL_PREFIX}/bin/clang\"
        LLVM_ENABLED=1
    )
endif()
