cmake_minimum_required(VERSION 2.8.2 FATAL_ERROR)
project(collab-data-crdts)

if("${PROJECT_SOURCE_DIR}" STREQUAL "${PROJECT_BINARY_DIR}")
    message(FATAL_ERROR "in-source build not allowed. Use another directory.")
endif()


# ------------------------------------------------------------------------------
# Dependencies and Misc
# ------------------------------------------------------------------------------
set(collab_dependency_dir "${CMAKE_SOURCE_DIR}/dependencies/")

include("CMake/AddCompilerFlags.cmake")
include_directories("${CMAKE_SOURCE_DIR}/include/")


# ------------------------------------------------------------------------------
# Build static lib for Custom data (SimpleGraph)
# ------------------------------------------------------------------------------
option(collab_custom "Build custom static lib" OFF)

if(collab_custom)
    message(STATUS "Build custom static lib")
    include("CMake/Dependencies/AddMsgPack.cmake")
    file(GLOB_RECURSE srcFiles "${CMAKE_SOURCE_DIR}/src/*.cpp")
    add_library(collabdata STATIC ${srcFiles})
endif()


# ------------------------------------------------------------------------------
# Examples
# ------------------------------------------------------------------------------
option(collab_examples "Build examples" OFF)

if(collab_examples)
    message(STATUS "Build examples")
    include("CMake/AddExamples.cmake")
endif()


# ------------------------------------------------------------------------------
# Tests
# ------------------------------------------------------------------------------
option(collab_tests "Build tests" OFF)

if(collab_tests)
    message(STATUS "Build tests")
    include("CMake/AddTests.cmake")
endif()


