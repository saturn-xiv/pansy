#!/bin/bash

set -e

export VCPKG_DISABLE_METRICS=1
export VCPKG_DEFAULT_BINARY_CACHE=$PWD/.cache

mkdir -p $VCPKG_DEFAULT_BINARY_CACHE

declare -a targets=("x86_64-linux" "aarch64-linux" "riscv64-linux" "x86_64-windows")
for i in "${targets[@]}"
do
    cmake --preset=$i
    cmake --build build/$i
done

echo "done."
