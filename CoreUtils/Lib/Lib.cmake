include_guard()

include( ${CMAKE_CURRENT_LIST_DIR}/Lib.srcs.cmake )

add_library( CoreUtils STATIC ${SOURCES} )

target_include_directories(
    CoreUtils
    PUBLIC  ${CMAKE_CURRENT_LIST_DIR}/Include
    PRIVATE ${CMAKE_CURRENT_LIST_DIR}/Source
)

include( ${CMAKE_CURRENT_LIST_DIR}/../../CMake/Common.cmake )

if( CMAKE_BUILD_TYPE STREQUAL "Debug" )
    enable_sanitizers( CoreUtils )
endif()
