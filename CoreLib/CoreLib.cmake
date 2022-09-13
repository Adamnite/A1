include_guard()

option( ENABLE_LLVM "Enable LLVM" ON )
message( STATUS "LLVM enabled: ${ENABLE_LLVM}" )

include( ${CMAKE_CURRENT_LIST_DIR}/CoreLib.srcs.cmake )

add_library( CoreLib STATIC ${SOURCES} )

target_include_directories(
    CoreLib
    PUBLIC  ${CMAKE_CURRENT_LIST_DIR}/Include
    PRIVATE ${CMAKE_CURRENT_LIST_DIR}/Source
)

include( FetchContent )

# Use fmt library until C++20 std::format is supported by all compilers
FetchContent_Declare( fmt
    GIT_REPOSITORY https://github.com/fmtlib/fmt.git
    GIT_TAG 9.1.0
)
FetchContent_MakeAvailable( fmt )

target_link_libraries( CoreLib PRIVATE fmt::fmt )

if( ENABLE_LLVM )
    # Prerequisites:
    # - install Clang / LLVM using Homebrew
    #       $ brew install llvm
    #       $ export PATH="$(brew --prefix llvm)/bin:${PATH}"

    find_package( LLVM REQUIRED CONFIG )
    message( STATUS "LLVM version: ${LLVM_PACKAGE_VERSION}" )

    target_include_directories( CoreLib AFTER PRIVATE ${LLVM_INCLUDE_DIRS} )
    add_definitions( ${LLVM_DEFINITIONS} )

    llvm_map_components_to_libnames( LLVMLibs
        core
        irreader
        support
    )

    target_link_libraries( CoreLib PRIVATE ${LLVMLibs} )
endif()
