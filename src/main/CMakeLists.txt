add_compile_options(
        "-O3" "-Werror" "-Wall" "-Wpedantic" "-Wextra" "-Wshadow" "-fexceptions" "-Wnon-virtual-dtor" "-Wno-unused-parameter"
)

include_directories(./)

file(GLOB_RECURSE SOURCES_CPP *.cpp)

add_library(addr2Symbol SHARED ${SOURCES_CPP})