#!/bin/bash

set -e

source /etc/os-release

export WORKSPACE=$PWD
export VERSION=$(git describe --tags --always --dirty --first-parent)

# -----------------------------------------------------------------------------


# go tool dist list
function build_go() {
    echo "build backend on $1-$2"
    
    local pkg="github.com/saturn-xiv/pansy/env"    
    local ldflags="-a -extldflags '-static' -s -w -X '$pkg.build_time=$(date -u -R)' -X '$pkg.git_version=$VERSION'"
    local target=$WORKSPACE/tmp/pansy-$VERSION-$1-$2

    if [ -d $target ]
    then
        rm -r $target
    fi
    mkdir -p $target
    CGO_ENABLED=0 GOOS=$1 GOARCH=$2 go build -ldflags "$ldflags" -o $target/pansy
}

# -----------------------------------------------------------------------------
go mod tidy

build_go linux amd64
build_go linux arm64
build_go linux riscv64
build_go darwin arm64

echo "done."
exit 0
