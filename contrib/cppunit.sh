#!/bin/bash

INSTALL_PATH=${PWD}/../
VERSION=1.12.1

export CC=clang
#export CXX=clang++
#export CXXFLAGS=--std=c++11 --stdlib=libc++
export CXX=g++-mp-4.7

echo $INSTALL_PATH
echo cppunit-$VERSION.tar.gz

tar xfz cppunit-$VERSION.tar.gz
cd cppunit-$VERSION
./configure --prefix=$INSTALL_PATH
make
make install

cd ../
rm -rf cppunit-$VERSION
