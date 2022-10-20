set( SOURCES
    ${CMAKE_CURRENT_LIST_DIR}/Source/Compiler/Compiler.cpp

    ${CMAKE_CURRENT_LIST_DIR}/Source/Parser/ExpressionTree.cpp
    ${CMAKE_CURRENT_LIST_DIR}/Source/Parser/ExpressionTreeHelper.cpp
    ${CMAKE_CURRENT_LIST_DIR}/Source/Parser/ExpressionTreeNode.cpp

    ${CMAKE_CURRENT_LIST_DIR}/Source/Tokenizer/ReservedToken.cpp
    ${CMAKE_CURRENT_LIST_DIR}/Source/Tokenizer/Token.cpp
    ${CMAKE_CURRENT_LIST_DIR}/Source/Tokenizer/Tokenizer.cpp

    ${CMAKE_CURRENT_LIST_DIR}/Source/Utils/PushBackStream.cpp

    ${CMAKE_CURRENT_LIST_DIR}/Source/Module.cpp
    ${CMAKE_CURRENT_LIST_DIR}/Source/Types.cpp
)

if( ENABLE_LLVM )
    list( APPEND SOURCES
        ${CMAKE_CURRENT_LIST_DIR}/Source/Compiler/LLVM/Compiler.cpp
        ${CMAKE_CURRENT_LIST_DIR}/Source/Compiler/LLVM/IRCodegen/Codegen.cpp
        ${CMAKE_CURRENT_LIST_DIR}/Source/Compiler/LLVM/IRCodegen/CodegenExpression.cpp
        ${CMAKE_CURRENT_LIST_DIR}/Source/Compiler/LLVM/IRCodegen/CodegenVisitor.cpp
    )
endif()
