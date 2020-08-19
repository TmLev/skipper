#!/bin/bash

# Assume that script is running in the root directory.

build() {
  mkdir build && cd build
  cmake ..
  cmake --build . -- -j2
}

(build)