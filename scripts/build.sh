#!/bin/bash

set -e

source /etc/os-release

export WORKSPACE=$PWD
export VERSION=$(git describe --tags --always --dirty --first-parent)

# -----------------------------------------------------------------------------


# go tool dist list
function build_go() {
    echo "build backend on $1"
    cd $WORKSPACE/

    local pkg="github.com/saturn-xiv/pansy/env"    
    local ldflags="-a -extldflags '-static' -s -w -X '$pkg.build_time=$(date -u -R)' -X '$pkg.git_version=$VERSION'"
    local target=$WORKSPACE/tmp/pansy-$VERSION-$1

    if [ -d $target ]
    then
        rm -r $target
    fi
    mkdir -p $target
    CC=$2-linux-gnu-gcc CGO_ENABLED=0 GOOS=linux GOARCH=$1 go build -ldflags "$ldflags" -o $target/pansy
}

# -----------------------------------------------------------------------------

build_go amd64 x86_64
build_go arm64 aarch64
build_go riscv64 riscv64

echo "done."
exit 0
