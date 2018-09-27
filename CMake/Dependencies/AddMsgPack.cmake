# ------------------------------------------------------------------------------
# Download msgpack at cmake time
# Inspired from googletest
# See: https://github.com/google/googletest/tree/master/googletest
# ------------------------------------------------------------------------------

# Alias variables
set(COLLAB_MSGPACK_DIR      "${COLLAB_DEPENDENCIES_DIR}/msgpack-c")
set(COLLAB_MSGPACK_SOURCES  "${COLLAB_MSGPACK_DIR}/sources")
set(COLLAB_MSGPACK_HEADERS  "${COLLAB_MSGPACK_DIR}/include")
set(COLLAB_MSGPACK_DOWNLOAD "${CMAKE_BINARY_DIR}/msgpack-download")
set(COLLAB_MSGPACK_BUILD    "${CMAKE_BINARY_DIR}/msgpack-build")


if(COLLAB_DEPENDENCIES_DOWNLOAD)
    # Create dep folder content
    file(MAKE_DIRECTORY "${COLLAB_MSGPACK_SOURCES}")
    file(MAKE_DIRECTORY "${COLLAB_MSGPACK_HEADERS}")


    # Create Download CMakeLists
    configure_file(
        "${CMAKE_SOURCE_DIR}/CMake/ExternalProjects/MsgPack.cmake"
        "${COLLAB_MSGPACK_DOWNLOAD}/CMakeLists.txt")

    # Setup Download (in download folder)
    execute_process(COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" .
        RESULT_VARIABLE result
        WORKING_DIRECTORY "${COLLAB_MSGPACK_DOWNLOAD}")
    if(result)
        message(FATAL_ERROR "CMake step for msgpack failed: ${result}")
    endif()

    execute_process(COMMAND ${CMAKE_COMMAND} --build .
        RESULT_VARIABLE result
        WORKING_DIRECTORY "${COLLAB_MSGPACK_DOWNLOAD}")
    if(result)
        message(FATAL_ERROR "Build step for msgpack failed: ${result}")
    endif()

    # Copy headers (To follow dependency folder style)
    file(COPY "${COLLAB_MSGPACK_SOURCES}/include"
         DESTINATION "${COLLAB_MSGPACK_DIR}")

elseif(NOT IS_DIRECTORY "${COLLAB_MSGPACK_DIR}")
    message("WARNING: MsgPack dependency is missing.")
    message("To download it automatically, set -DCOLLAB_DEPENDENCIES_DOWNLOAD=ON")
    message(FATAL_ERROR "Missing dependency...")
endif()


include_directories("${COLLAB_MSGPACK_HEADERS}")


