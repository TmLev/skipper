#!/bin/sh

# Assumes that project was built in directory `ROOT_DIR/build`.

python3 third-party/run-clang-format/run-clang-format.py \
  --clang-format-executable clang-format-10 \
  --extensions hpp,ipp,cpp \
  --recursive \
  skipper tests benchmarks
