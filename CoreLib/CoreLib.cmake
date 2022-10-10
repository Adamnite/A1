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

target_link_libraries( CoreLib PRIVATE fmt::fmt )

if( ENABLE_TESTS )
    target_compile_definitions( CoreLib PRIVATE
        TESTS_ENABLED=1
    )
endif()

if( ENABLE_LLVM )
    find_package( LLVM  REQUIRED CONFIG )
    find_package( Clang REQUIRED CONFIG )

    message( STATUS "LLVM version: ${LLVM_PACKAGE_VERSION}" )

    target_include_directories( CoreLib AFTER PRIVATE ${LLVM_INCLUDE_DIRS} )
    add_definitions( ${LLVM_DEFINITIONS} )

    target_link_libraries( CoreLib PRIVATE
        LLVMSupport
        LLVMCore
        LLVMIRReader
        LLVMCodeGen
        LLVMTarget
        LLVMAArch64CodeGen
        LLVMAArch64AsmParser
        LLVMAArch64Disassembler
        LLVMAArch64Desc
        LLVMAArch64Info
        LLVMAArch64Utils
        LLVMARMCodeGen
        LLVMARMAsmParser
        LLVMARMDisassembler
        LLVMARMDesc
        LLVMARMInfo
        LLVMARMUtils
        LLVMX86CodeGen
        LLVMX86AsmParser
        LLVMX86Disassembler
        LLVMX86TargetMCA
        LLVMX86Desc
        LLVMX86Info
        LLVMAsmParser
        LLVMPasses
        LLVM
        clangTooling
    )

    # In order to compile .ao source files programmatically,
    # we need to use the Clang compiler coming with the LLVM package.
    target_compile_definitions( CoreLib PRIVATE
        CLANG_PATH=\"${LLVM_INSTALL_PREFIX}/bin/clang\"
        LLVM_ENABLED=1
    )
endif()
