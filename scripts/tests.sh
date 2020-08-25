#!/usr/bin/env bash

# Assume that project was built in directory `ROOT_DIR/build`.

run_tests() {
  cd build/tests || echo "Tests directory not found" && exit 1
  test_names=$(find . -maxdepth 1 -name "test_*")

  for test in "${test_names[@]}"; do
    echo "Running \"$test\"..."
    chmod +x "$test" && ./"$test"
  done
}

(run_tests)
