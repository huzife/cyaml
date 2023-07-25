#! /bin/bash

if [ $(id -u) -ne 0 ]; then
    echo "Please run as root."
    exit 1
fi;

apt install libgtest-dev -y

cd /usr/src/gtest
mkdir build
cd build
cmake .. -DBUILD_SHARED_LIBS=ON
make

LIB_PATH=/usr/lib/$(uname -m)-linux-gnu
if [ ! -d $LIB_PATH ]; then
    LIB_PATH=/usr/lib
fi;

cp lib/*.so $LIB_PATH
ldconfig
