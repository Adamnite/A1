include_guard()

include( ${CMAKE_CURRENT_LIST_DIR}/CoreLib.srcs.cmake )

add_library( CoreLib STATIC ${SOURCES} )

target_include_directories(
    CoreLib
    PUBLIC  ${CMAKE_CURRENT_LIST_DIR}/Include
    PRIVATE ${CMAKE_CURRENT_LIST_DIR}/Source
)
