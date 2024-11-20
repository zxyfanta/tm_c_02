#!/bin/bash

mkdir -p build

cmake . -Bbuild

cd build

make

echo "build completed"
echo "--------------------------------------------"
./tm_c_02
echo "--------------------------------------------"