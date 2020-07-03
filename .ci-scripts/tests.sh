#!/bin/sh

# Assume that project was built in directory `ROOT_DIR/build`.

run_tests() {
  cd build/tests
  ./catch2_works
}

(run_tests)
