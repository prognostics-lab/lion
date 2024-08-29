#!/bin/bash

# DISCLAIMER: Tools are meant to be run from the root directory of the project, not from the
#             `sys` directory.

params=""
example=""
platform=""
generator=""
force_generator=""
release_mode=0
examples=0
testing=0
verbose=0
python_install=0

while getopts "hcxvisetrdwlpG:R:P:" arg
do
    case "$arg" in
        c)
            sys/clean.sh
            ;;
        x)
            params+=" -DCMAKE_EXPORT_COMPILE_COMMANDS=1"
            ;;
        i)
            params+=" -DLION_INSTALL=ON"
            ;;
        s)
            params+=" -DBUILD_SHARED_LIBS=OFF"
            ;;
        h)
            params+=" -DBUILD_SHARED_LIBS=ON"
            ;;
        e)
            examples=1
            ;;
        t)
            testing=1
            ;;
        r)
            release_mode=1
            ;;
        d)
            release_mode=0
            ;;
        w)
            platform="WINDOWS"
            ;;
        l)
            platform="LINUX"
            ;;
        v)
            verbose=1
            ;;
        p)
            python_install=1
            ;;
        G)
            force_generator="${OPTARG}"
            ;;
        R)
            example="${OPTARG}"
            ;;
        P)
            platform="${OPTARG}"
            ;;
    esac
done

if [ $testing -eq 1 ]; then
    params+=" -DLION_BUILD_TESTS=ON"
else
    params+=" -DLION_BUILD_TESTS=OFF"
fi
if [ $examples -eq 1 ]; then
    params+=" -DLION_BUILD_EXAMPLES=ON"
else
    params+=" -DLION_BUILD_EXAMPLES=OFF"
fi

if [ $release_mode -eq "1" ]; then
    params+=" -DCMAKE_BUILD_TYPE=Release"
else
    params+=" -DCMAKE_BUILD_TYPE=Debug"
fi

if [[ "${platform}" == "WINDOWS" ]]; then
    generator="Visual Studio 17 2022"
elif [[ "${platform}" == "LINUX" ]]; then
    generator="Unix Makefiles"
fi

if [[ "${force_generator}" != "" ]]; then
    generator=$force_generator
fi

echo -e "\n\nBuilding with parameters \x1b[32;20m'${params}'\x1b[0m"
if [[ "${generator}" == "" ]]; then
    cmake -S . -B ./build/ $params
else
    cmake -S . -B ./build/ -G "${generator}" $params
fi

if [ $release_mode -eq 1 ]; then
    cmake --build ./build/ --config Release
    build_type="Release"
else
    cmake --build ./build/
    build_type="Debug"
fi

if [ $testing -eq 1 ]; then
    echo -e "\n\n\x1b[32;20mRunning tests\x1b[0m"
    if [ $verbose -eq 1 ]; then
        sys/test.sh -C $build_type -v
    else
        sys/test.sh -C $build_type
    fi
fi

if [[ "${example}" != "" ]]; then
    echo -e "\n\nRunning example '\x1b[32;20m${example}\x1b[0m'"
    if [ $release_mode -eq 1 ]; then
        ./bin/ex.${example}
    else
        ./bin/debug/ex.${example}
    fi
fi

if [ $python_install -eq 1 ]; then
    echo -e "\n\n\x1b[32;20mInstalling Python wrapper\x1b[0m"
    pip install -e .
fi
