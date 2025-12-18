#!/usr/bin/env bash

# build 
build() {
    if premake5 --cc=clang --verbose gmake; then
        echo "premake succeeded"
        if make; then
            echo "make succeeded"
            ./bin/debug/filefaff
        fi
    fi
}

# used for compile_commands
cc() {
    if premake5 --version export-compile-commands; then
        echo "compile-commands succeeded"
    fi
}

usage() {
    echo "./run.sh usage: build|cc"
}
if [[ $# -lt 1 ]]; then
    usage
fi
if [[ $1 =~ "build" ]]; then
    echo "./run.sh selected build"
    build
elif [[ $1 =~ "cc" ]]; then
    echo "./run.sh selected cc"
    cc
else
    usage
fi
