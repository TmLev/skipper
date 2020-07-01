cmake_minimum_required(VERSION 3.16)

# Catch 2
add_subdirectory(Catch2)

# Google Benchmark
set(BENCHMARK_ENABLE_TESTING OFF CACHE BOOL "Enable testing of the benchmark library.")
add_subdirectory(benchmark)
