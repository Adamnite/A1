include_guard()

macro( enable_sanitizers target )
    if( CMAKE_CXX_COMPILER_ID MATCHES "Clang" OR CMAKE_CXX_COMPILER_ID MATCHES "GNU" )
        target_compile_options( ${target} PRIVATE -fsanitize=undefined -fsanitize=address )
        target_link_libraries ( ${target} PUBLIC  -fsanitize=undefined -fsanitize=address )
    endif()
endmacro()