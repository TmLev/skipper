cmake_minimum_required(VERSION 3.12)

project(skipper)

set(CMAKE_CXX_STANDARD 17)

# Public skipper options
option(SKIPPER_ENABLE_TESTS "Enable testing of the skipper library" ON)
option(SKIPPER_ENABLE_BENCHMARKS "Enable benchmarking of the skipper library" ON)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Include skipper library
add_subdirectory(skipper)

set(SKIPPER_INCLUDE_CATCH2 "Include third-party library Catch2" ${SKIPPER_ENABLE_TESTS})
set(SKIPPER_INCLUDE_BENCHMARK "Include third-party library Google Benchmark" ${SKIPPER_ENABLE_BENCHMARKS})
add_subdirectory(third-party)

if (SKIPPER_ENABLE_TESTS)
  add_subdirectory(tests)
endif ()

if (SKIPPER_ENABLE_BENCHMARKS)
  add_subdirectory(benchmarks)
endif ()
