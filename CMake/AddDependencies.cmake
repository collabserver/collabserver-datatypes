# ------------------------------------------------------------------------------
# Download and build msgpack (At cmake time)
# ------------------------------------------------------------------------------

configure_file(
    "${CMAKE_SOURCE_DIR}/CMake/ExternalProjects/MsgPack.cmake"
    "msgpack-download/CMakeLists.txt")

execute_process(COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" .
    RESULT_VARIABLE result
    WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/msgpack-download")
if(result)
    message(FATAL_ERROR "CMake step for msgpack failed: ${result}")
endif()

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


