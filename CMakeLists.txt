cmake_minimum_required(VERSION 2.8.2 FATAL_ERROR)

if("${PROJECT_SOURCE_DIR}" STREQUAL "${PROJECT_BINARY_DIR}")
    message(FATAL_ERROR "in-source build not allowed. Use another directory.")
endif()

include("CMake/AddTests.cmake")
