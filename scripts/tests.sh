#!/bin/bash

# Assume that project was built in directory `ROOT_DIR/build`.

test_names=(
  "test_sequential_set"
)

run_tests() {
  cd build/tests
  for test in "${test_names[@]}"; do
    echo "Running \"$test\"..."
    chmod +x "$test" && ./"$test"
  done
}

(run_tests)
