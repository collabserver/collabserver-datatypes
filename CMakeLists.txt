cmake_minimum_required(VERSION 3.14)
project(collabserver-datatypes)

if("${PROJECT_SOURCE_DIR}" STREQUAL "${PROJECT_BINARY_DIR}")
    message(FATAL_ERROR "in-source build not allowed. Use another directory.")
endif()

# Dependencies and Misc
include("CMake/AddCompilerFlags.cmake")
include("CMake/AddDependencies.cmake")

# Examples
option(COLLABSERVER_EXAMPLES "Build examples" OFF)
if(COLLABSERVER_EXAMPLES)
    message(STATUS "Building examples")
    include("CMake/AddExamples.cmake")
endif()

# Tests
option(COLLABSERVER_TESTS "Build tests" OFF)
if(COLLABSERVER_TESTS)
    message(STATUS "Building tests")
    include("CMake/AddTests.cmake")
endif()

