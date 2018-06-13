cmake_minimum_required (VERSION 3.0)

project(art)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(art INTERFACE)

target_include_directories(
    art
    INTERFACE
    "$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>"
    "$<INSTALL_INTERFACE:include>"
)

add_executable(main "${PROJECT_SOURCE_DIR}/src/example.cpp")
target_link_libraries(main art)

# doctest
set(DOCTEST_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/third_party/doctest/doctest)
add_library(doctest INTERFACE)
target_include_directories(doctest INTERFACE ${DOCTEST_INCLUDE_DIR})

# test executable
file(GLOB TEST_SOURCES "${PROJECT_SOURCE_DIR}/test/*.cpp")
add_executable(test ${TEST_SOURCES})
target_link_libraries(test art doctest)

add_subdirectory(src)