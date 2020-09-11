option(ENABLE_CCACHE "Enable ccache for builds" ON)
if (NOT ENABLE_CCACHE)
  message(STATUS "ccache is disabled")
  return()
endif ()

find_program(CCACHE_FOUND "ccache")
if (NOT CCACHE_FOUND)
  message(WARNING "ccache is not found")
  return()
endif ()

set(CMAKE_CXX_COMPILER_LAUNCHER "ccache")
message(STATUS "ccache is found and enabled")
