#!/usr/bin/env bash

# Assume that project was built in directory `ROOT_DIR/build`.

run_benchmarks() {
  cd build/benchmarks || echo "Benchmarks directory not found" && exit 1
  benchmark_names=$(find . -maxdepth 1 -name "benchmark_*")

  for benchmark in "${benchmark_names[@]}"; do
    chmod +x "$benchmark" && ./"$benchmark"
  done
}

(run_benchmarks)
