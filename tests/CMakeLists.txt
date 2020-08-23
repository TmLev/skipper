add_executable(test_sequential_set
  test_catch2_main.cpp
  test_sequential_set.cpp)
target_link_libraries(test_sequential_set PRIVATE Catch2::Catch2)
target_link_libraries(test_sequential_set PRIVATE skipper)
