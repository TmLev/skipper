#!/bin/sh

# shellcheck disable=SC2038

python3 third-party/run-clang-format/run-clang-format.py \
  --clang-format-executable clang-format-10 \
  --recursive \
  --extensions hpp,ipp,cpp \
  skipper tests benchmarks
