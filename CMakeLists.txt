cmake_minimum_required(VERSION 3.16)

project(skipper)

set(CMAKE_CXX_STANDARD 17)

option(SKIPPER_ENABLE_TESTS "Enable testing of the skipper library" ON)
option(SKIPPER_ENABLE_BENCHMARKS "Enable benchmarking of the skipper library" ON)

foreach (p
    SKIPPER_ENABLE_TESTS
    SKIPPER_ENABLE_BENCHMARKS)
    if (DEFINED p)
        set(SKIPPER_INCLUDE_THIRD_PARTY ON)
    endif ()
endforeach ()

if (SKIPPER_INCLUDE_THIRD_PARTY)
    add_subdirectory(third-party)
endif ()

if (SKIPPER_ENABLE_TESTS)
    add_subdirectory(tests)
endif ()

if (SKIPPER_ENABLE_BENCHMARKS)
    add_subdirectory(benchmarks)
endif ()

# Just to see if CMake actually works TODO: remove later
add_executable(skipper main.cpp)
