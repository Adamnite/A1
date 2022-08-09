include_guard()

include( ${CMAKE_CURRENT_LIST_DIR}/CoreUtils.srcs.cmake )

add_library( CoreUtils STATIC ${SOURCES} )

target_include_directories(
    CoreUtils
    PUBLIC  ${CMAKE_CURRENT_LIST_DIR}/Include
    PRIVATE ${CMAKE_CURRENT_LIST_DIR}/Source
)
