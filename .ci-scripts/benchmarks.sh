#!/bin/sh

# Assume that project was built in directory `ROOT_DIR/build`.

run_benchmarks() {
  cd build/benchmarks
  ./benchmark_works
}

(run_benchmarks)
