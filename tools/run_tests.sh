#!/bin/bash

# Copyright (c) 2013-2014 Alexander Ignatyev. All rights reserved.

# Run from directory that contains CMakeLists.txt
# Usage: run_tests.sh <COMPILER> <-c>
# use -c for clean build

if [ "$1" != "" ]; then
    CXX=$1
fi

NUMBER_OF_CORES=1
if [[ "$OSTYPE" == "darwin"* ]]; then
    NUMBER_OF_CORES=$(sysctl hw.ncpu | awk '{print $2}')
fi

BUILD_PATH=build_$CXX

if [[ ("$2" == "-c") && (-e $BUILD_PATH) ]]; then
    rm -rf $BUILD_PATH
fi

if [ ! -e $BUILD_PATH ]; then
    mkdir $BUILD_PATH
fi

rm -f $BUILD_PATH/optimer-tests
cd $BUILD_PATH
cmake -DCMAKE_BUILD_TYPE=Release ..
make -j $NUMBER_OF_CORES
cd ../
./$BUILD_PATH/optimer-tests
