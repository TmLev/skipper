#!/bin/bash

# Assume that script is running in the root directory.

build() {
  cmake -S . -B build
  cmake --build build
}

(build)
