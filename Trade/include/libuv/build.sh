#! /bin/sh

mkdir ./build
mkdir ./lib
rm -rf ./build/*
rm -rf ./lib/*
cd ./build
cmake ../ -DCMAKE_BUILD_TYPE=Release -DBUILD_TESTING=OFF -DCMAKE_INSTALL_PREFIX=../lib
make -j8

cp libuv* ../lib

cd ..
