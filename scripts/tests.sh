#!/usr/bin/env bash

# Assume that project was built in directory `ROOT_DIR/build`.

run_tests() {
  cd build/tests || {
    echo "Tests directory not found"
    exit 11
  }
  test_names=$(find . -maxdepth 1 -name "test_*" | sed "s|^\./||")

  for test in "${test_names[@]}"; do
    echo "Running \"$test\"..."
    chmod +x "$test" && ./"$test"
  done
}

(run_tests)
