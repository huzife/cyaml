cmake_minimum_required(VERSION 3.10)
project(cyaml VERSION 0.1)
set(CMAKE_CXX_STANDARD 17)

add_compile_definitions(CYAML_TEST)
add_compile_definitions(CYAML_DEBUG)

include_directories(include)

set(PARSER_SRC
    src/parser/api.cpp
    src/parser/node_builder.cpp
    src/parser/scanner.cpp
    src/parser/scan_token.cpp
    src/parser/parser.cpp
    src/parser/serializer.cpp
    src/parser/stream.cpp
)

set(TYPE_SRC
    src/type/mark.cpp
    src/type/node.cpp
    src/type/node_data.cpp
    src/type/token.cpp
)

set(ERROR_SRC
    src/error/exceptions.cpp
)

set(CYAML_LIB_OUTPUT_PATH ${CMAKE_BINARY_DIR}/lib)
set(CYAML_TEST_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin/test)

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CYAML_LIB_OUTPUT_PATH})
add_library(cyaml SHARED
    ${PARSER_SRC}
    ${TYPE_SRC}
    ${ERROR_SRC}
)

# 测试部分
find_package(GTest)
if (GTest_FOUND)
    message(${GTEST_LIBRARY})
    message(${GTEST_INCLUDE_DIR})
    message(${GTEST_MAIN_LIBRARY})

    include_directories(${GTEST_INCLUDE_DIR})

    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CYAML_TEST_OUTPUT_PATH})
    add_executable(scanner_test test/src/scanner_test.cpp)
    add_executable(parser_test test/src/parser_test.cpp)
    add_executable(serializer_test test/src/serializer_test.cpp)

    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CYAML_TEST_OUTPUT_PATH}/stdin)
    add_executable(stdin_test test/src/stdin/stdin_test.cpp)

    target_link_libraries(scanner_test cyaml pthread libgtest.so)
    target_link_libraries(parser_test cyaml pthread libgtest.so)
    target_link_libraries(serializer_test cyaml pthread libgtest.so)
    target_link_libraries(stdin_test cyaml pthread libgtest.so)
else()
    message(WARNING "GTest not found, abort building test")
endif()
