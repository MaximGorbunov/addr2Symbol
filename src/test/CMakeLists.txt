add_compile_options("-fno-omit-frame-pointer;-fsanitize=address;-fsanitize=undefined;-DDEBUG>")

add_link_options("-fsanitize=address;-fsanitize=undefined")

include(FetchContent)
FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/b796f7d44681514f58a683a3a71ff17c94edb0c1.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()

execute_process(COMMAND ${CMAKE_C_COMPILER} -print-file-name=${ASAN_NAME}
        OUTPUT_VARIABLE LIBASAN_PATH
        OUTPUT_STRIP_TRAILING_WHITESPACE)
include(GoogleTest)


include_directories(${SRC_DIR})
add_library(mockLibrary SHARED mockLibrary/mockLibrary.cpp)
link_libraries(mockLibrary)

add_executable(
        symbolsParsingTest
        symbolsParsingTest.cpp
)
target_link_libraries(
        symbolsParsingTest
        addr2Symbol
        GTest::gtest_main
)
gtest_discover_tests(symbolsParsingTest)