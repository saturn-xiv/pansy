#!/bin/bash

set -e

clang-format -i src/*.cpp include/pansy/*.hpp

echo 'done.'
