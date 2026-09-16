#!/bin/bash

set -e

export VCPKG_DISABLE_METRICS=1
export VCPKG_DEFAULT_BINARY_CACHE=$PWD/.cache

mkdir -p $VCPKG_DEFAULT_BINARY_CACHE

function build_project() {
    cmake --preset=$1
    cmake --build build/$1
}

function build_on_linux() {
    local -a targets=("x86_64-linux" "aarch64-linux" "riscv64-linux" "x86_64-windows")
    for i in "${targets[@]}"
    do
        build_project $i
    done
}

case "$OSTYPE" in
    linux*)
        build_on_linux
        ;;
    darwin*)
        build_project "arm64-darwin"
        ;;
    *)
        echo "Unsupported operating system: $OSTYPE"
        exit 1
        ;;
esac

echo "done."
