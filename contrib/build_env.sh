#!/bin/bash

# Copyright (c) 2013 Alexander Ignatyev. All rights reserved.

if [ "$1" != "" ];
then
    export CXX=$1
fi

if [[ $CXX == clang++* ]];
then
    export CXXFLAGS="-stdlib=libc++"
    export LDFLAGS="-stdlib=libc++"
fi

if [ "$2" != "" ];
then
    INSTALL_PATH=$2/$CXX
elif [ "$OPTIMER_BUILD_ENV" == "" ];
then
    INSTALL_PATH=${PWD}/../env/$CXX
else
    INSTALL_PATH=${OPTIMER_BUILD_ENV}/$CXX
fi

function install_cppunit {
    VERSION=1.12.1

    echo $INSTALL_PATH
    echo cppunit-$VERSION.tar.gz

    tar xfz cppunit-$VERSION.tar.gz
    cd cppunit-$VERSION

    ./configure --prefix=$1 --disable-shared
    make
    make install

    cd ../
    rm -rf cppunit-$VERSION
}

install_cppunit $INSTALL_PATH
