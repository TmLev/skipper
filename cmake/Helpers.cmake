function (add_skipper_test NAME)
  if (NOT NAME)
    message(SEND_ERROR "`add_skipper_test` was called without name for test")
    return()
  endif ()

  add_executable(${NAME} test_catch2_main.cpp ${NAME}.cpp)

  target_link_libraries(${NAME} PRIVATE Catch2::Catch2)
  target_link_libraries(${NAME} PRIVATE skipper)
endfunction ()

function (add_skipper_benchmark NAME)
  if (NOT NAME)
    message(SEND_ERROR "`add_skipper_benchmark` was called without name for benchmark")
    return()
  endif ()

  add_executable(${NAME} ${NAME}.cpp)
  target_link_libraries(${NAME} PRIVATE benchmark::benchmark_main)
  target_link_libraries(${NAME} PRIVATE skipper)
endfunction ()
