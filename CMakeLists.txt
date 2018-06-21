cmake_minimum_required(VERSION 2.8.2 FATAL_ERROR)
project(collab-data-crdts)

if("${PROJECT_SOURCE_DIR}" STREQUAL "${PROJECT_BINARY_DIR}")
    message(FATAL_ERROR "in-source build not allowed. Use another directory.")
endif()


# ------------------------------------------------------------------------------
# Various
# ------------------------------------------------------------------------------
set(DEPENDENCY_DIR "${CMAKE_SOURCE_DIR}/bin/")

include("CMake/AddCompilerFlags.cmake")


# ------------------------------------------------------------------------------
# Examples
# ------------------------------------------------------------------------------
option(BUILD_EXAMPLES "Build examples" ON)

if(BUILD_EXAMPLES)
    message(STATUS "Build examples")
    include("CMake/AddExamples.cmake")
else()
    message(STATUS "Doesn't build examples")
endif()


# ------------------------------------------------------------------------------
# Tests
# ------------------------------------------------------------------------------
option(BUILD_TESTS "Build Tests" OFF)

if(BUILD_TESTS)
    message(STATUS "Build tests")
    include("CMake/AddTests.cmake")
else()
    message(STATUS "Doesn't build Test")
endif()


