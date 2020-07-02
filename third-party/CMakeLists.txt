if (SKIPPER_INCLUDE_CATCH2)
  # Hack to prevent building CTest targets.
  # https://github.com/catchorg/Catch2/issues/1669#issuecomment-611648068
  set_property(GLOBAL PROPERTY CTEST_TARGETS_ADDED 1)

  set(CATCH_BUILD_TESTING
      OFF
      CACHE BOOL "Build SelfTest project.")

  add_subdirectory(Catch2)
endif ()

if (SKIPPER_INCLUDE_BENCHMARK)
  set(BENCHMARK_ENABLE_TESTING
      OFF
      CACHE BOOL "Enable testing of the benchmark library.")
  set(BENCHMARK_ENABLE_INSTALL
      OFF
      CACHE BOOL "Enable installation of benchmark.")

  add_subdirectory(benchmark)
endif ()
