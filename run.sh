#!/usr/bin/env bash

SCRIPT_DIR="$(cd $(dirname -- $0) && pwd)"
# build 
build() {
    if premake5 --cc=clang --verbose gmake; then
        echo "premake succeeded"
        if make; then
            echo "make succeeded"
            # ./bin/debug/filefaff
        fi
    fi
}

# used for compile_commands
cc() {
    if premake5 export-compile-commands; then
        echo "compile-commands succeeded"
    fi
    echo $SCRIPT_DIR
    ln -sf $SCRIPT_DIR/compile_commands.json $SCRIPT_DIR/compile_commands/debug.json
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
