include_guard()

include( ${CMAKE_CURRENT_LIST_DIR}/AOC.srcs.cmake )

add_executable( aoc ${SOURCES} )

target_include_directories(
    aoc
    PRIVATE ${CMAKE_CURRENT_LIST_DIR}/Source
)

target_link_libraries(
    aoc PRIVATE
    CoreLib
    fmt::fmt
)
