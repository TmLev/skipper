cmake_minimum_required(VERSION 3.16)
project(skipper)

set(CMAKE_CXX_STANDARD 17)

add_subdirectory(third-party)
add_subdirectory(tests)

add_executable(skipper main.cpp)

