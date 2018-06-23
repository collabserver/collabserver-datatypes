cmake_minimum_required(VERSION 2.8.2 FATAL_ERROR)
project(collab-data-crdts)

if("${PROJECT_SOURCE_DIR}" STREQUAL "${PROJECT_BINARY_DIR}")
    message(FATAL_ERROR "in-source build not allowed. Use another directory.")
endif()


# ------------------------------------------------------------------------------
# Dependencies and Misc
# ------------------------------------------------------------------------------
set(collab_dependency_dir "${CMAKE_SOURCE_DIR}/bin/")

include("CMake/AddCompilerFlags.cmake")


# ------------------------------------------------------------------------------
# Examples Executables
# ------------------------------------------------------------------------------
option(collab_examples "Build examples" ON)

if(collab_examples)
    message(STATUS "Build examples")
    include("CMake/AddExamples.cmake")
else()
    message(STATUS "Doesn't build examples")
endif()


# ------------------------------------------------------------------------------
# Tests Executables
# ------------------------------------------------------------------------------
option(collab_tests "Build Tests" OFF)

if(collab_tests)
    message(STATUS "Build tests")
    include("CMake/AddTests.cmake")
else()
    message(STATUS "Doesn't build Test")
endif()


