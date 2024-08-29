#!/bin/bash

# DISCLAIMER: Tools are meant to be run from the root directory of the project, not from the
#             `sys` directory.

params=""
config="Debug"

while getopts "vC:" arg
do
    case "$arg" in
        C)
            config=$OPTARG
            ;;
        v)
            params+=" --verbose"
            ;;
    esac
done

cd build
ctest -C $config --output-on-failure $params
