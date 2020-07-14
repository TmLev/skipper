option(ENABLE_CCACHE "Enable ccache for builds" OFF)
if (NOT ENABLE_CCACHE)
  message(STATUS "Ccache is disabled")
  return()
endif ()

find_program(CCACHE_FOUND "ccache")
if (NOT CCACHE_FOUND)
  message(STATUS "Ccache is not found")
  return()
endif ()

set(CMAKE_CXX_COMPILER_LAUNCHER "ccache")
message(STATUS "Ccache is found and enabled")
