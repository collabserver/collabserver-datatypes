#!/bin/bash

set -e
set -x
set -v

mkdir build || true
cd build

cmake -DCMAKE_BUILD_TYPE=${BUILD_TYPE} \
      -Dcollab_examples=ON \
      -Dcollab_tests=ON \
      ..

make
make runAllTests


