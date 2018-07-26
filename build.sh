#!/bin/bash

set -e
set -x
set -v

mkdir build || true
cd build

cmake -DCMAKE_BUILD_TYPE=${BUILD_TYPE} \
      -Dcollab_tests=ON \
      -Dcollab_examples=OFF \
      ..

make
make runAllTests


