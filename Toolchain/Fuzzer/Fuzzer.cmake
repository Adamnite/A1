include_guard()

macro( add_fuzzer_target component )
    set( TARGET_NAME ${component}Fuzzer )

    add_executable( ${TARGET_NAME} ${CMAKE_CURRENT_LIST_DIR}/Source/${component}Fuzzer.cpp )
    target_link_libraries( ${TARGET_NAME} PRIVATE CoreLib )

    if( CMAKE_CXX_COMPILER_ID MATCHES "Clang" )
        target_compile_options( ${TARGET_NAME} PRIVATE -fsanitize=fuzzer,address )
        target_link_libraries ( ${TARGET_NAME} PRIVATE -fsanitize=fuzzer,address )
    else()
        message( FATAL_ERROR "libFuzzer available only with Clang compiler" )
    endif()
endmacro()

add_fuzzer_target( Tokenizer )
add_fuzzer_target( AST       )
