cmake_minimum_required(VERSION 2.8.2 FATAL_ERROR)
project(collab-data-crdts)

if("${PROJECT_SOURCE_DIR}" STREQUAL "${PROJECT_BINARY_DIR}")
    message(FATAL_ERROR "in-source build not allowed. Use another directory.")
endif()


# ------------------------------------------------------------------------------
# Dependencies and Misc
# ------------------------------------------------------------------------------
include("CMake/AddCompilerFlags.cmake")
include("CMake/AddDependencies.cmake")


# ------------------------------------------------------------------------------
# Build custom static lib (SimpleGraph)
# ------------------------------------------------------------------------------
message(STATUS "Building custom static lib")
include("CMake/AddCustom.cmake")


# ------------------------------------------------------------------------------
# Examples
# ------------------------------------------------------------------------------
option(COLLAB_EXAMPLES "Build examples" OFF)

if(COLLAB_EXAMPLES)
    message(STATUS "Building examples")
    include("CMake/AddExamples.cmake")
endif()


# ------------------------------------------------------------------------------
# Tests
# ------------------------------------------------------------------------------
option(COLLAB_TESTS "Build tests" OFF)

if(COLLAB_TESTS)
    message(STATUS "Building tests")
    include("CMake/AddTests.cmake")
endif()


