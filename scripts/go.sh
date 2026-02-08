#!/bin/bash

set -e

export WORKSPACE=$PWD

# https://code.visualstudio.com/docs/languages/go
go install golang.org/x/tools/gopls@latest
go install github.com/go-delve/delve/cmd/dlv@latest
go install honnef.co/go/tools/cmd/staticcheck@latest

cd $WORKSPACE/
go get -u
go mod tidy

echo 'done.'
