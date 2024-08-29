#!/bin/bash

# This script compiles the documentation into a book using mdBook

cd docs

serve=0

while getopts "s" arg
do
    case "$arg" in
        s)
            serve=1
            ;;
    esac
done

if [ $serve -eq 1 ]; then
    mdbook serve --open
else
    mdbook build
fi
