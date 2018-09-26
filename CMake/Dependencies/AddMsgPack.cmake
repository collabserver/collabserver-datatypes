# ------------------------------------------------------------------------------
# Download msgpack at cmake time
# Inspired from googletest
# See: https://github.com/google/googletest/tree/master/googletest
# ------------------------------------------------------------------------------

set(COLLAB_DEP_MSGPACK "${COLLAB_DEPENDENCIES_DIR}/msgpack-c")

file(MAKE_DIRECTORY "${COLLAB_DEP_MSGPACK}/download")
file(MAKE_DIRECTORY "${COLLAB_DEP_MSGPACK}/sources")
file(MAKE_DIRECTORY "${COLLAB_DEP_MSGPACK}/include")


# Create Download CMakeLists
configure_file(
    "${CMAKE_SOURCE_DIR}/CMake/ExternalProjects/MsgPack.cmake"
    "${COLLAB_DEP_MSGPACK}/download/CMakeLists.txt")

# Setup Download (in download folder)
execute_process(COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" .
    RESULT_VARIABLE result
    WORKING_DIRECTORY "${COLLAB_DEP_MSGPACK}/download")
if(result)
    message(FATAL_ERROR "CMake step for msgpack failed: ${result}")
endif()


# Clone (in source folder)
if(COLLAB_DEPENDENCIES_DOWNLOAD)
    execute_process(COMMAND ${CMAKE_COMMAND} --build .
        RESULT_VARIABLE result
        WORKING_DIRECTORY "${COLLAB_DEP_MSGPACK}/download")
    if(result)
        message(FATAL_ERROR "Build step for msgpack failed: ${result}")
    endif()
endif()


include_directories("${COLLAB_DEP_MSGPACK}/include")

file(COPY        "${COLLAB_DEP_MSGPACK}/sources/include"
     DESTINATION "${COLLAB_DEP_MSGPACK}")


