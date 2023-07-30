cmake_minimum_required(VERSION 3.24)
project(addr2symbol)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)

enable_testing()

set(SRC_DIR ${PROJECT_SOURCE_DIR}/src/main/)

add_subdirectory(src/main)
add_subdirectory(src/test)
