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
add_executable(main 
  "${PROJECT_SOURCE_DIR}/src/example.cpp"
  )
target_link_libraries(main art zipf)

# test executable
add_executable(test
  "${PROJECT_SOURCE_DIR}/test/art.cpp"
  "${PROJECT_SOURCE_DIR}/test/main.cpp"
  "${PROJECT_SOURCE_DIR}/test/node.cpp"
  "${PROJECT_SOURCE_DIR}/test/inner_node.cpp"
  "${PROJECT_SOURCE_DIR}/test/node_4.cpp"
  "${PROJECT_SOURCE_DIR}/test/node_16.cpp"
  "${PROJECT_SOURCE_DIR}/test/node_48.cpp"
  "${PROJECT_SOURCE_DIR}/test/node_256.cpp"
  )
target_link_libraries(test art doctest)

# bench executable
add_executable(bench
  "${PROJECT_SOURCE_DIR}/bench/delete.cpp"
  "${PROJECT_SOURCE_DIR}/bench/insert.cpp"
  "${PROJECT_SOURCE_DIR}/bench/main.cpp"
  "${PROJECT_SOURCE_DIR}/bench/mixed.cpp"
  "${PROJECT_SOURCE_DIR}/bench/mixed_dense.cpp"
  # "${PROJECT_SOURCE_DIR}/bench/node_4.cpp"
  # "${PROJECT_SOURCE_DIR}/bench/node_16.cpp"
  # "${PROJECT_SOURCE_DIR}/bench/node_48.cpp"
  # "${PROJECT_SOURCE_DIR}/bench/node_256.cpp"
  "${PROJECT_SOURCE_DIR}/bench/query_sparse_uniform.cpp"
  "${PROJECT_SOURCE_DIR}/bench/query_sparse_uniform_64.cpp"
  "${PROJECT_SOURCE_DIR}/bench/query_sparse_zipf.cpp"
  )
target_link_libraries(bench art picobench zipf)
