#!/bin/sh

set -e

cmake -B build
cmake --build build

gdb build/cpptest
