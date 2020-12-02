cmake_minimum_required(VERSION 3.14 FATAL_ERROR)
project(collabserver-data)

if("${PROJECT_SOURCE_DIR}" STREQUAL "${PROJECT_BINARY_DIR}")
    message(FATAL_ERROR "in-source build not allowed. Use another directory.")
endif()

# Dependencies and Misc
include("CMake/AddCompilerFlags.cmake")
include("CMake/AddDependencies.cmake")

# Examples
option(COLLAB_EXAMPLES "Build examples" OFF)
if(COLLAB_EXAMPLES)
    message(STATUS "Building examples")
    include("CMake/AddExamples.cmake")
endif()

# Tests
option(COLLAB_TESTS "Build tests" OFF)
if(COLLAB_TESTS)
    message(STATUS "Building tests")
    include("CMake/AddTests.cmake")
endif()


