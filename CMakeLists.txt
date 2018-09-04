cmake_minimum_required (VERSION 3.0)

project(art)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_CXX_FLAGS "-Wall -Wextra")
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O2")

add_library(art INTERFACE)

target_include_directories(
    art
    INTERFACE
    "$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>"
    "$<INSTALL_INTERFACE:include>"
)

### dependencies ###

# doctest
set(DOCTEST_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/third_party/doctest/doctest)
add_library(doctest INTERFACE)
target_include_directories(doctest INTERFACE ${DOCTEST_INCLUDE_DIR})

# picobench
set(PICOBENCH_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/third_party/picobench/include)
add_library(picobench INTERFACE)
target_include_directories(picobench INTERFACE ${PICOBENCH_INCLUDE_DIR})

# zipf
set(ZIPF_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/third_party/zipf)
add_library(zipf ${PROJECT_SOURCE_DIR}/third_party/zipf/zipf.cpp)
target_include_directories(zipf PUBLIC ${ZIPF_INCLUDE_DIR})

### executables ###

# main executable
file(GLOB MAIN_SOURCES "${PROJECT_SOURCE_DIR}/src/*.cpp")
add_executable(main ${MAIN_SOURCES})
target_link_libraries(main art zipf)

# test executable
file(GLOB TEST_SOURCES "${PROJECT_SOURCE_DIR}/test/*.cpp")
add_executable(test ${TEST_SOURCES})
target_link_libraries(test art doctest)

# bench executable
file(GLOB BENCH_SOURCES "${PROJECT_SOURCE_DIR}/bench/*.cpp")
add_executable(bench ${BENCH_SOURCES})
target_link_libraries(bench art picobench zipf)
