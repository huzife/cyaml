cmake_minimum_required(VERSION 3.10)
project(cyaml VERSION 1.0)
set(CMAKE_CXX_STANDARD 17)

option(BUILD_TEST "build test or not" OFF)

include_directories(include)

set(PARSER_SRC
    src/parser/api.cpp
    src/parser/node_builder.cpp
    src/parser/scanner.cpp
    src/parser/scan_token.cpp
    src/parser/parser.cpp
    src/parser/serializer.cpp
    src/parser/stream.cpp
    src/parser/unicode.cpp
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
if (BUILD_TEST)
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
        add_executable(sax_test test/src/sax_test.cpp)

        set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CYAML_TEST_OUTPUT_PATH}/stdin)
        add_executable(stdin_test test/src/stdin/stdin_test.cpp)

        target_link_libraries(scanner_test cyaml pthread libgtest.so)
        target_link_libraries(parser_test cyaml pthread libgtest.so)
        target_link_libraries(serializer_test cyaml pthread libgtest.so)
        target_link_libraries(sax_test cyaml pthread libgtest.so)
        target_link_libraries(stdin_test cyaml pthread libgtest.so)
    else()
        message(WARNING "GTest not found, abort building test")
    endif()
endif()

# install
set(INSTALL_INCLUDE_DIR include)
set(INSTALL_LIBRARY_DIR lib)

install(TARGETS ${PROJECT_NAME}
    EXPORT ${PROJECT_NAME}Targets
    LIBRARY DESTINATION lib
)

install(DIRECTORY ${PROJECT_SOURCE_DIR}/include/cyaml
    DESTINATION include
)

include(CMakePackageConfigHelpers)
write_basic_package_version_file(
    ${PROJECT_NAME}-config-version.cmake
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY AnyNewerVersion
)

install(EXPORT ${PROJECT_NAME}Targets
    FILE ${PROJECT_NAME}Targets.cmake
    NAMESPACE ${PROJECT_NAME}::
    DESTINATION lib/cmake/${PROJECT_NAME}
)

configure_package_config_file(
    "${PROJECT_SOURCE_DIR}/${PROJECT_NAME}-config.cmake.in"
    "${PROJECT_BINARY_DIR}/${PROJECT_NAME}-config.cmake"
    INSTALL_DESTINATION "lib/cmake/${PROJECT_NAME}"
    PATH_VARS INSTALL_INCLUDE_DIR INSTALL_LIBRARY_DIR
)

configure_file(${PROJECT_NAME}-config.cmake.in ${PROJECT_NAME}.cmake @ONLY)
install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}-config.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}-config-version.cmake"
    DESTINATION lib/cmake/${PROJECT_NAME}
)
