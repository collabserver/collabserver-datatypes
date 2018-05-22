# Conflict-free Replicated Data Types (CRDTs)

[![release-version](https://img.shields.io/badge/release-beta--version-red.svg)]()
[![build-status-master](https://travis-ci.org/CollabServer/collab-data-crdts.svg?branch=master)](https://travis-ci.org/CollabServer/collab-data-crdts)
[![license](https://img.shields.io/badge/license-Apache2.0-blue.svg)](https://github.com/CollabServer/collab-data-crdts/blob/master/LICENSE.txt)


| master | dev
| :-----: | :----: |
| [![build-status-master](https://travis-ci.org/CollabServer/collab-data-crdts.svg?branch=master)](https://travis-ci.org/CollabServer/collab-data-crdts) | [![build-status-dev](https://travis-ci.org/CollabServer/collab-data-crdts.svg?branch=dev)](https://travis-ci.org/CollabServer/collab-data-crdts) |


# Description
Defines a set of basic CRDTs that may be used to build your more complex data (on top of these structures).
CRDTs stands for Conflict-free Replicated Data Structure.
Note that the C may either stands for "Commutative" or "Convergent".
Commutative is implemented as Operation based CRDT (CmRDT) whereas Convergent is the State based (CvRDT).
To learn more about CRDTs, checkout the links at the end of this readme.


# Features
- CmRDT (Operation-based)
    - LWWGraph: Last-Write-Wins Graph
    - LWWMap: Last-Write-Wins Map
    - LWWRegister: Last-Write-Wins Register
    - LWWSet: Last-Write-Wins Set
- CvRDT (State-based)
    - 2PSet: Add / Remove set (Two-phases Set)
    - GCounter: Grow-only counter
    - GGraph: Grow-only graph
    - GMap: Grow-only map
    - GSet: Grow-only set
    - LWWRegister: Last-Write-Wins Register
    - PNCounter: Increment / Decrement counter
- custom (Custom data built in top of CRDTs)
    - LWWViewMDE: custom data built for AToMPM software.

> Warning: I wrote the CvRDTs as an example. They are not meant to be used.
> (At least, some changes may be required).


# Dependencies
> All dependencies are automatically downloaded by CMake.

- [GoogleTest](https://github.com/google/googletest) (For unit tests).


# Build and run with CMake (Linux only)
> Requires C++11.

> Requires "pragma once" support.

```
mkdir build
cd build
cmake ..
make -j4
make ./testAll
```


# CRDTs operations / Academic description

## State-based object (CvRDT)
    Convergent Replicated Data Types (CvRDT)
    State CvRDT is defined by (S s0 q u m)
        S   -> Global State
        s0  -> State at beginning
        q   -> Query method
        u   -> Update method
        m   -> Merge method

## Operation-based object (CmRDT)
    Commutative Replicated Data Types (CmRDT)
    Operation CmRDT is defined by (S s0 q t u P)
        S   -> Global State
        s0  -> State at beginning
        q   -> Query method
        t   -> Side-effect-free prepare update method
        u   -> Effect update method (downstream)
        P   -> Delivery relation P for communication protocol


# Author
- Constantin Masson ([constantinmasson.com](http://constantinmasson.com/))


# Papers and resources
- Conflict-free Replicated Data Types (https://pages.lip6.fr/Marc.Shapiro/papers/CRDTs_SSS-2011.pdf)
- A comprehensive study of Convergent and Commutative Replicated Data Types (https://pages.lip6.fr/Marc.Shapiro/papers/Comprehensive-CRDTs-RR7506-2011-01.pdf)


