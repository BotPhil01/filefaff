#!/usr/bin/env bash

SCRIPT_DIR="$(cd "$(dirname -- "$0")" && pwd)"
PREMAKE_URL="https://github.com/premake/premake-core/releases/download/v5.0.0-beta7/premake-5.0.0-beta7-linux.tar.gz"

if [[ $(/usr/bin/id -u) -eq 0 ]]; then
    printf "Script must not run as root\n"
fi

if ! curl --version 1&>/dev/null; then
    printf "curl not installed\ninstalling curl from apt\n"
    sudo apt-get install -y curl 
fi

if ! tar --version 1&>/dev/null; then
    printf "tar not installed\ninstalling tar from apt\n"
    sudo apt-get install -y tar 
fi

if ! git --version 1&>/dev/null; then
    printf "git not installed\ninstalling git from apt\n"
    sudo apt-get install -y git 
fi

if ! premake5 --version 1&>/dev/null; then
    printf "premake5 not installed\nInstalling premake from premake.github.io\n"
    sudo curl -L $PREMAKE_URL | tar xkzvC /usr/bin
    sudo chmod +x /usr/bin/premake5
fi

if ! premake5 export-compile-commands 1&>/dev/null; then
    printf "premake5 modules not installed\ninstalling from "
    git clone https://github.com/tarruda/premake-export-compile-commands ~/.premake/export-compile-commands
    PREM_SYS_LUA=~/.premake/premake-system.lua
    if ! [[ -f $PREM_SYS_LUA ]]; then
        printf "creating premake system lua file\n"
        mkdir ~/.premake/ &>/dev/null
        touch $PREM_SYS_LUA
    fi
    oldfile="$(cat $PREM_SYS_LUA)"
    printf "require \"export-compile-commands\"\n$oldfile" > $PREM_SYS_LUA
fi
