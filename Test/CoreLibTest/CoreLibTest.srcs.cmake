set( SOURCES
    ${CMAKE_CURRENT_LIST_DIR}/Source/Parser/ExpressionTreeTest.cpp

    ${CMAKE_CURRENT_LIST_DIR}/Source/Tokenizer/ReservedTokenTest.cpp
    ${CMAKE_CURRENT_LIST_DIR}/Source/Tokenizer/TokenizerTest.cpp

    ${CMAKE_CURRENT_LIST_DIR}/Source/TestUtils.cpp
)

if( ENABLE_LLVM )
    list( APPEND SOURCES
        ${CMAKE_CURRENT_LIST_DIR}/Source/Compiler/LLVM/CompilerTest.cpp
    )
endif()