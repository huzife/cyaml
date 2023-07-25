cmake_minimum_required(VERSION 3.10)
project(cyaml VERSION 0.1)
set(CMAKE_CXX_STANDARD 17)

add_compile_definitions(CYAML_TEST)
add_compile_definitions(CYAML_DEBUG)

find_package(GTest REQUIRED)
if (GTest_FOUND)
    message(${GTEST_LIBRARY})
    message(${GTEST_INCLUDE_DIR})
    message(${GTEST_MAIN_LIBRARY})
endif()

include_directories(include)

set(PARSER_SRC
    src/parser/token.cpp
    src/parser/scanner.cpp
)

set(CYAML_LIB_OUTPUT_PATH ${CMAKE_BINARY_DIR}/lib)
set(CYAML_TEST_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin/test)

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CYAML_LIB_OUTPUT_PATH})
add_library(cyaml SHARED
    ${PARSER_SRC}
)

# 测试部分
include_directories(${GTEST_INCLUDE_DIR})
link_directories(${GTEST_LIBRARY})

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CYAML_TEST_OUTPUT_PATH})
add_executable(scanner_test test/src/scanner_test.cpp)

target_link_libraries(scanner_test cyaml pthread libgtest.so)
