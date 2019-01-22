# CollabServer - Conflict-free Replicated DataTypes (CRDTs)

[![license](https://img.shields.io/badge/license-LGPLv3.0-blue.svg)](https://github.com/CollabServer/collab-data-crdts/blob/master/LICENSE.txt)
[![build-status-master](https://travis-ci.org/CollabServer/collab-data-crdts.svg?branch=master)](https://travis-ci.org/CollabServer/collab-data-crdts)

| master | dev |
| :-----: | :----: |
| [![build-status-master](https://travis-ci.org/CollabServer/collab-data-crdts.svg?branch=master)](https://travis-ci.org/CollabServer/collab-data-crdts) | [![build-status-dev](https://travis-ci.org/CollabServer/collab-data-crdts.svg?branch=dev)](https://travis-ci.org/CollabServer/collab-data-crdts) |


# Overview
Defines a set of basic CRDTs that may be used to build your more complex data
(on top of these structures).
CRDTs stands for Conflict-free Replicated Data Structure.
The letter C in CRDT may either stands for "Commutative" or "Convergent".
Commutative is implemented as Operation based CRDT (CmRDT) whereas Convergent
is the State based (CvRDT).
To learn more about CRDTs, checkout the links at the end of this readme.

Custom folder contains several end-user interface to build high level
data on top of primitive CRDTs. SimpleGraph is a full example of
CRDT directed graph to end-user application.


# Quick Start
- CRDTs primitive are header only (LWWGraph, LWWMap...)
- Custom data (Optional) is a static lib (SimpleGraph, Timestamp...)

CRDTs primitives are header only (`CmRDT`, `CvRDT`), you only need to include
the headers you need in order to build your concurrent data.
For instance, to use CmRDT::LWWSet, add `#include "collabdata/CmRDT/LWWSet.h"`.
To use you data on a CollabServer, your data must implement `CollabData`
interface.
The custom data (`SimpleGraph`) is built as static lib, to use it in your
project, you must include the header and link the library.


# Features
- **CmRDT** (Operation-based)
    - *LWWGraph*: Last-Write-Wins Graph
    - *LWWMap*: Last-Write-Wins Map
    - *LWWRegister*: Last-Write-Wins Register
    - *LWWSet*: Last-Write-Wins Set
- **CvRDT** (State-based. See warning)
    - *2PSet*: Add / Remove set (Two-phases Set)
    - *GCounter*: Grow-only counter
    - *GGraph*: Grow-only graph
    - *GMap*: Grow-only map
    - *GSet*: Grow-only set
    - *LWWRegister*: Last-Write-Wins Register
    - *PNCounter*: Increment / Decrement counter
- **custom** (Assets classes to implements data for CollabServer)
    - *CollabData*: High level abstraction for data built on tope of CRDTs.
    - *Operation*: Represents a modification on a CollabData.
    - *OperationHandler*: Interface to handle operations received from observer.
    - *OperationObserver*: Interface for Operation observer.
    - *SimpleGraph*: Example of a CollabData: directed graph with attributes.
    - *Timestamp*: Example of custom timestamp. (Used by SimpleGraph)

> **Warning**: I wrote the CvRDTs as an example. They are not meant to be used.
> (At least, some changes may be required).


# Build on Linux (CMake)
**Build tests**
```bash
mkdir build
cd build
cmake -DCOLLAB_DEPENDENCIES_DOWNLOAD=ON -DCOLLAB_TESTS=ON ..
make
make runTests

# Or use build script
./build.sh
```

**Build examples**
```bash
mkdir build
cd build
cmake -DCOLLAB_DEPENDENCIES_DOWNLOAD=ON -DCOLLAB_EXAMPLES=ON ..
make
make runExamplesCmRDT
```

**CMake options**

| CMake option name | Description |
| --- | --- |
| COLLAB_DEPENDENCIES_DIR | (STRING) Path to a directory where to find all dependencies (By default, uses current cmake build). If this option is unset but the environment variable `COLLAB_DEPENDENCIES_DIR` exists, this one is used instead of the default value. |
| COLLAB_DEPENDENCIES_DOWNLOAD | (ON/OFF) Set ON to also download dependencies at cmake time. This is useful the first time you setup the project. Dependencies are placed in COLLAB_DEPENDENCIES_DIR. (By default: OFF).|
| COLLAB_TESTS | (ON/OFF) Set ON to build unit tests |
| COLLAB_EXAMPLES | (ON/OFF) Set ON to build examples |
| CMAKE_BUILD_TYPE | Debug, Release, RelWithDebInfo, MinSizeRel |


# Requirements
- C++11
- `pragma once` support
- Tested with gcc 4.8.4
- Tested with clang 5.0.0
- Tested only on Linux. No support certified for Mac and Windows


# Dependencies
- Automatically downloaded and built by CMake
    - [MessagePack](https://msgpack.org/) (Release 3.1.1)
    - [GoogleTest](https://github.com/google/googletest) (Release 1.8.1)

**Option: save dependencies for offline use**
> Dependencies downloaded by CMake are placed in the current CMake build folder
> (in `dependencies` folder).
> This is useful the firs time.
> To speedup the cmake process, you may keep these downloaded and built dependencies
> in a safe place and change the CMake dependencies path (See CMake options).

**Option: use environment variable for dependencies path**
> If `COLLAB_DEPENDENCIES_DIR` environment variable is set, CMake will use
> it as the current dependencies path.


# CRDTs theoretical description
- **State-based object (CvRDT)** \
CvRDT stands for Convergent Replicated Data Types.
Updates are applied locally, then the whole data state is sent to others.
A merge method integrate two states so that all replicates convergent.

```
    State CvRDT is defined by (S s0 q u m)
        S   -> Global State
        s0  -> State at beginning
        q   -> Query method
        u   -> Update method
        m   -> Merge method
```

- **Operation-based object (CmRDT)** \
CmRDT stands for Commutative Replicated Data Types.
Only delta of change (Operation) are broadcasted and applied to others.
All operations are commutative.

```
    Operation CmRDT is defined by (S s0 q t u P)
        S   -> Global State
        s0  -> State at beginning
        q   -> Query method
        t   -> Side-effect-free prepare update method
        u   -> Effect update method (downstream)
        P   -> Delivery relation P for communication protocol
```


# Papers and Resources
- Conflict-free Replicated Data Types \
  (https://pages.lip6.fr/Marc.Shapiro/papers/CRDTs_SSS-2011.pdf)
- A comprehensive study of Convergent and Commutative Replicated Data Types \
  (https://pages.lip6.fr/Marc.Shapiro/papers/Comprehensive-CRDTs-RR7506-2011-01.pdf)
- A Look at Conflict-Free Replicated Data Types (CRDT) \
  (https://medium.com/@istanbul_techie/a-look-at-conflict-free-replicated-data-types-crdt-221a5f629e7e)
- CRDT for collaborative editing by Irisate \
  (https://irisate.com/crdt-for-real-time-collaborative-apps/)


# Generate Documentation
1. Install [Doxygen](https://www.stack.nl/~dimitri/doxygen/)
1. `doxygen Doxyfile`
1. Files are placed in `doc` folder


# Contribution
Feel free to ask me any question, share your ideas or open an issue.
I created this project during my master thesis at University of Montreal.
I generally try to follow the [Google C++ Coding Style](https://google.github.io/styleguide/cppguide.html)
with some exceptions (For instance, I use 4 space indentation).


# Author
- Constantin Masson ([constantinmasson.com](http://constantinmasson.com/))


