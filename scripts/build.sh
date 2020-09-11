#!/usr/bin/env bash

# Assume that script is running in the root directory.

build() {
  rm -rf build
  mkdir build
  cd build || {
    echo "Failed to make build directory"
    exit 12
  }

  cmake ..
  cmake --build . --parallel 2
}

(build)
