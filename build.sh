#!/bin/bash

set -e
set -x
set -v

mkdir build || true
cd build

cmake -DCMAKE_BUILD_TYPE=${ENV_BUILD_TYPE} \
      -DCOLLABSERVER_DATATYPES_TESTS=ON \
      -DCOLLABSERVER_DATATYPES_EXAMPLES=ON \
      ..

make
make runTests

