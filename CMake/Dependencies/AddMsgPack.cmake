# ------------------------------------------------------------------------------
# Download and build msgpack at cmake time
# Inspired from googletest
# See: https://github.com/google/googletest/tree/master/googletest
# ------------------------------------------------------------------------------

configure_file(
    "${CMAKE_SOURCE_DIR}/CMake/ExternalProjects/MsgPack.cmake"
    "msgpack-download/CMakeLists.txt")

# Create Makefile
execute_process(COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" .
    RESULT_VARIABLE result
    WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/msgpack-download")
if(result)
    message(FATAL_ERROR "CMake step for msgpack failed: ${result}")
endif()

# Clone git project
execute_process(COMMAND ${CMAKE_COMMAND} --build .
    RESULT_VARIABLE result
    WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/msgpack-download")
if(result)
    message(FATAL_ERROR "Build step for msgpack failed: ${result}")
endif()

add_subdirectory("${collab_dependency_dir}/msgpack"
                 "${CMAKE_BINARY_DIR}/msg-pack-build"
                 EXCLUDE_FROM_ALL)

include_directories("${collab_dependency_dir}/msgpack/include")


