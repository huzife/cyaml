#! /bin/bash

root_path=$(cd $(dirname $0) && pwd)

# check gtest
if [ $(ldconfig -p | grep gtest | wc -l) -eq 0 ]; then
    if [ $(id -u) -eq 0 ]; then
        $root_path/install_gtest.sh
    else
        sudo $root_path/install_gtest.sh
    fi;
fi;
    
# build
echo "building project..."
if [ ! -d $root_path/build ]; then
    mkdir $root_path/build
fi;

cd $root_path/build
cmake $root_path
make

# run test
echo -e "\nstart testing..."
for test in $root_path/build/bin/test/*
do
    $test
done
