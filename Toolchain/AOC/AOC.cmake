include_guard()

set( AOC_VERSION_MAJOR 0 )
set( AOC_VERSION_MINOR 1 )
set( AOC_VERSION_PATCH 0 )
set( AOC_VERSION "${AOC_VERSION_MAJOR}.${AOC_VERSION_MINOR}.${AOC_VERSION_PATCH}")

include( ${CMAKE_CURRENT_LIST_DIR}/AOC.srcs.cmake )

add_executable( aoc ${SOURCES} )

include( ${CMAKE_CURRENT_LIST_DIR}/../../CMake/Common.cmake )
strip( aoc )

target_include_directories(
    aoc
    PRIVATE ${CMAKE_CURRENT_LIST_DIR}/Source
)

target_link_libraries( aoc PRIVATE
    CoreLib
    fmt::fmt
)

target_compile_definitions( aoc PRIVATE
    AOC_VERSION=\"${AOC_VERSION}\"
)

install( TARGETS aoc
    CONFIGURATIONS      Release
    RUNTIME DESTINATION bin
)

set( CPACK_PACKAGE_NAME   "aoc"      )
set( CPACK_PACKAGE_VENDOR "Adamnite" )

include( CPack )