cmake_minimum_required(VERSION 3.24)
project(mora VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Library files
add_library(mora_lib STATIC
  src/lexer/token.cpp
  src/lexer/scanner.cpp
  src/parser/parser.cpp
  src/ast/ast.cpp
  src/interpreter/interpreter.cpp
  src/arena/arena.cpp
)

target_include_directories(mora_lib PUBLIC src)
target_compile_options(mora_lib PRIVATE -Wall -Wextra -Wpedantic)

# Main executables
add_executable(mora src/main.cpp)
target_link_libraries(mora PRIVATE mora_lib)

# Tests
find_package(Catch2 3 QUIET)
if(NOT Catch2_FOUND)
  include(FetchContent)
  FetchContent_Declare(
    Catch2
    GIT_REPOSITORY https://github.com/catchorg/Catch2.git
    GIT_TAG v3.7.1
  )
  FetchContent_MakeAvailable(Catch2)
endif()

file(GLOB_RECURSE TEST_SOURCES tests/*.cpp)
add_executable(mora_tests ${TEST_SOURCES})
target_link_libraries(mora_tests PRIVATE mora_lib Catch2::Catch2WithMain)
target_compile_options(mora_tests PRIVATE -Wall -Wextra -Wpedantic)

include(CTest)
add_test(NAME mora_tests COMMAND mora_tests)
