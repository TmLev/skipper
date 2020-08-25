#!/usr/bin/env bash

# Assume that script is running in the root directory.

python3 third-party/run-clang-format/run-clang-format.py \
  --clang-format-executable clang-format-10 \
  --extensions hpp,ipp,cpp \
  --recursive \
  include/skipper utils tests benchmarks
