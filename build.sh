#!/usr/bin/env bash

# run cmake
cmake -B build

# if clean is passed as an argument, run `cmkae --build build --target clean`
if [ "$1" = "clean" ]; then
    cmake --build build --target clean
fi

cmake --build build