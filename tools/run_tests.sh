#!/bin/bash

# Copyright (c) 2013 Alexander Ignatyev. All rights reserved.

# Run from directory that contains CMakeLists.txt
# Usage: run_tests.sh <COMPILER>

if [ "$1" != "" ];
then
    export CXX=$1
fi

export BUILD_PATH=build_$CXX

mkdir $BUILD_PATH
cd $BUILD_PATH
pwd
cmake -DCMAKE_BUILD_TYPE=Release ..
make
cd ../
./$BUILD_PATH/optimer-tests
