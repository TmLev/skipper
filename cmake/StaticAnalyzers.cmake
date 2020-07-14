option(ENABLE_CLANG_TIDY "Enable static analysis with clang-tidy" OFF)
if (NOT ENABLE_CLANG_TIDY)
  message(STATUS "clang-tidy is disabled")
  return()
endif ()

find_program(CLANG_TIDY_FOUND "clang-tidy-10")
if (NOT CLANG_TIDY_FOUND)
  message(SEND_ERROR "clang-tidy was not found")
  return()
endif ()

set(CMAKE_CXX_CLANG_TIDY "clang-tidy-10" -extra-arg=-Wno-unknown-warning-option)
message(STATUS "clang-tidy is found and enabled")
