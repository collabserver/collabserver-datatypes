#!/bin/bash

set -e
set -x
set -v

mkdir build || true
cd build

cmake -DCMAKE_BUILD_TYPE=${BUILD_TYPE} \
      -DCOLLAB_DEPENDENCIES_DOWNLOAD=ON \
      -DCOLLAB_TESTS=ON \
      -DCOLLAB_EXAMPLES=ON \
      ..

make
make runTests


