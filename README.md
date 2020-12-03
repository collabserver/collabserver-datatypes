# CollabServer - DataTypes (CRDTs)

[![license](https://img.shields.io/badge/license-LGPLv3.0-blue.svg)](https://github.com/CollabServer/collabserver-datatypes/blob/master/LICENSE.txt)
[![build-status-master](https://travis-ci.org/CollabServer/collabserver-datatypes.svg?branch=master)](https://travis-ci.org/CollabServer/collabserver-datatypes)

| master | dev |
| :-----: | :----: |
| [![build-status-master](https://travis-ci.org/CollabServer/collabserver-datatypes.svg?branch=master)](https://travis-ci.org/CollabServer/collabserver-datatypes) | [![build-status-dev](https://travis-ci.org/CollabServer/collabserver-datatypes.svg?branch=dev)](https://travis-ci.org/CollabServer/collabserver-datatypes) |

## Overview

---

> Conflict-free Replicated DataTypes (CRDTs)

Defines a set of CRDTs that may be used to build your more complex data
(on top of these structures) and used with the CollabServer framework.
CRDTs stands for Conflict-free Replicated Data Structure.
The letter C in CRDT may either stands for "Commutative" or "Convergent".
Commutative is implemented as Operation based CRDT (CmRDT) whereas Convergent
is the State based (CvRDT).
To learn more about CRDTs, checkout the links at the end of this readme.

Link to my thesis: <https://papyrus.bib.umontreal.ca/xmlui/handle/1866/22532>

## Quick Start

---

- This library is header only, you only have to include the headers in your project.
- To use your data with the CollabServer framework, your data have to implement `CollabData`.

## Features

---

- **CmRDT** (Operation-based CRDT)
  - *LWWGraph*: Last-Write-Wins Graph
  - *LWWMap*: Last-Write-Wins Map
  - *LWWRegister*: Last-Write-Wins Register
  - *LWWSet*: Last-Write-Wins Set
- **collabdata** (Interfaces to implements for CollabServer)
  - *CollabData*: High level abstraction for data built on tope of CRDTs.
  - *Operation*: Represents a modification on a CollabData.
  - *OperationHandler*: Interface to handle operations received from observer.
  - *OperationObserver*: Interface for Operation observer.

## Build (CMake)

---

- Requirements
  - [CMake](https://cmake.org/)
  - C++11
  - `pragma once` support
  - Tested with gcc 4.8.4
  - Tested with clang 5.0.0
  - Tested only on Linux. No support certified for Mac and Windows
- Dependencies (downloaded and placed in `extern` by CMake)
  - [GoogleTest](https://github.com/google/googletest)

```bash
# Build the tests and examples

mkdir build
cd build
cmake -DCOLLABSERVER_TESTS=ON -DCOLLABSERVER_EXAMPLES=ON ..
make
make runTests

# Or use the build script
./build.sh
```

| CMake option | Description |
| --- | --- |
| COLLABSERVER_TESTS | (ON / OFF) Set ON to build unit tests |
| COLLABSERVER_EXAMPLES | (ON / OFF) Set ON to build examples |
| CMAKE_BUILD_TYPE | Debug, Release, RelWithDebInfo, MinSizeRel |
| FETCHCONTENT_FULLY_DISCONNECTED=ON | To skip download of the `extern` depencencies |

## CRDTs theoretical description

---

- **State-based object (CvRDT)** \
CvRDT stands for Convergent Replicated Data Types.
Updates are applied locally, then the whole data state is sent to others.
A merge method integrate two states so that all replicates convergent.

```txt
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

```txt
    Operation CmRDT is defined by (S s0 q t u P)
        S   -> Global State
        s0  -> State at beginning
        q   -> Query method
        t   -> Side-effect-free prepare update method
        u   -> Effect update method (downstream)
        P   -> Delivery relation P for communication protocol
```

## Papers and Resources

---

- Conflict-free Replicated Data Types \
  (<https://pages.lip6.fr/Marc.Shapiro/papers/CRDTs_SSS-2011.pdf>)
- A comprehensive study of Convergent and Commutative Replicated Data Types \
  (<https://pages.lip6.fr/Marc.Shapiro/papers/Comprehensive-CRDTs-RR7506-2011-01.pdf>)
- A Look at Conflict-Free Replicated Data Types (CRDT) \
  (<https://medium.com/@istanbul_techie/a-look-at-conflict-free-replicated-data-types-crdt-221a5f629e7e>)
- CRDT for collaborative editing by Irisate \
  (<https://irisate.com/crdt-for-real-time-collaborative-apps/>)

## Generate Documentation

---

1. Install [Doxygen](https://www.stack.nl/~dimitri/doxygen/)
1. Run `doxygen Doxyfile`
1. Files are placed in `doc` folder

## Contribution

---

Feel free to ask me any question, share your ideas or open an issue.
I started this project during my master thesis at University of Montreal.
Format uses clang-format with the Google Coding style <https://google.github.io/styleguide/cppguide.html> (see `.clang-format` for further information).
Make sure you autoformat on save (see <https://clang.llvm.org/docs/ClangFormat.html>)
