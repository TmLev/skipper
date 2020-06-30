cmake_minimum_required(VERSION 3.16)

# Catch 2
add_subdirectory(catch2)

# Google Benchmark
set(BENCHMARK_ENABLE_TESTING OFF CACHE BOOL "Enable testing of the benchmark library.")

if (CMAKE_BUILD_TYPE MATCHES "Release")
    add_subdirectory(benchmark)
else()
    message(STATUS "Skip benchmark library in '${CMAKE_BUILD_TYPE}' build type")
endif()
