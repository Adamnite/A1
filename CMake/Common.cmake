include_guard()

macro( enable_sanitizers target )
    if( CMAKE_CXX_COMPILER_ID MATCHES "Clang" OR CMAKE_CXX_COMPILER_ID MATCHES "GNU" )
        # -fsanitize=undefined includes vptr sanitizer which requires RTTI to be enabled.
        # Therefore, disable vptr sanitizer as LLVM is built with RTTI disabled.
        target_compile_options( ${target} PRIVATE -fsanitize=undefined -fsanitize=address -fno-sanitize=vptr )
        target_link_libraries ( ${target} PUBLIC  -fsanitize=undefined -fsanitize=address -fno-sanitize=vptr )
    endif()
endmacro()

macro( strip target )
    if( CMAKE_STRIP )
        add_custom_command( TARGET ${target} POST_BUILD
            COMMAND $<$<CONFIG:Release>:${CMAKE_STRIP}> $<TARGET_FILE:${target}>
            COMMENT "Creating stripped binary for ${target}"
            VERBATIM
        )
    endif()
endmacro()