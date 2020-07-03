#!/bin/sh

mkdir build
cd build
cmake ..
cmake --build . -- -j2
cd ..
