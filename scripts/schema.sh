#!/bin/bash

set -e

clang-format -i src/*.cpp begonia/src/*.cpp begonia/include/pansy/*.hpp tests/*.cpp

echo 'done.'
