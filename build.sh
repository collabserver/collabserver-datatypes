#!/bin/bash

set -e
set -x
set -v

mkdir build || true
cd build

cmake -DCMAKE_BUILD_TYPE=${ENV_BUILD_TYPE} \
      -DCOLLAB_TESTS=ON \
      -DCOLLAB_EXAMPLES=ON \
      ..

make
make runTests


