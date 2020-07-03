#!/bin/bash

# Assume that script is running in root directory.

build() {
  mkdir build && cd build
  cmake ..
  cmake --build . -- -j2
}

(build)
