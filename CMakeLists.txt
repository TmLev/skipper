cmake_minimum_required(VERSION 3.12)

project(
  skipper
  VERSION 0.3.1
  DESCRIPTION "Various Skip List Implementations"
  LANGUAGES CXX)

include(cmake/ProjectSettings.cmake)
include(cmake/Ccache.cmake)
include(cmake/StaticAnalyzers.cmake)
include(cmake/Helpers.cmake)

# Public skipper options
option(SKIPPER_ENABLE_TESTS "Enable testing of the skipper library" ON)
option(SKIPPER_ENABLE_BENCHMARKS "Enable benchmarking of the skipper library" ON)

# Include skipper library
add_library(skipper INTERFACE)
add_library(skipper::skipper ALIAS skipper)
target_include_directories(skipper INTERFACE include)
include(cmake/CompilerWarnings.cmake)

set(SKIPPER_INCLUDE_CATCH2 ${SKIPPER_ENABLE_TESTS})
set(SKIPPER_INCLUDE_BENCHMARK ${SKIPPER_ENABLE_BENCHMARKS})
add_subdirectory(third-party)

if (SKIPPER_ENABLE_TESTS)
  add_subdirectory(tests)
endif ()

if (SKIPPER_ENABLE_BENCHMARKS)
  add_subdirectory(benchmarks)
endif ()
