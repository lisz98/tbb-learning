#!/bin/sh

set -e

cmake -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build
build/main
