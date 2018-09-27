# ------------------------------------------------------------------------------
# Download and build GoogleTests at cmake time
# Inspired from googletest cmake
# See: https://github.com/google/googletest/tree/master/googletest
# ------------------------------------------------------------------------------

# Alias variables
set(COLLAB_GTEST_DIR        "${COLLAB_DEPENDENCIES_DIR}/googletest")
set(COLLAB_GTEST_SOURCES    "${COLLAB_GTEST_DIR}/sources")
set(COLLAB_GTEST_HEADERS    "${COLLAB_GTEST_DIR}/include")
set(COLLAB_GTEST_DOWNLOAD   "${CMAKE_BINARY_DIR}/googletest-download")
set(COLLAB_GTEST_BUILD      "${CMAKE_BINARY_DIR}/googletest-build")


if(COLLAB_DEPENDENCIES_DOWNLOAD)
    # Create dep folder content
    file(MAKE_DIRECTORY "${COLLAB_GTEST_SOURCES}")
    file(MAKE_DIRECTORY "${COLLAB_GTEST_HEADERS}")

    # Create Download CMakeLists
    configure_file(
        "${CMAKE_SOURCE_DIR}/CMake/ExternalProjects/GoogleTest.cmake"
        "${COLLAB_GTEST_DOWNLOAD}/CMakeLists.txt")

    # Setup Download (in download folder)
    execute_process(COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" .
        RESULT_VARIABLE result
        WORKING_DIRECTORY "${COLLAB_GTEST_DOWNLOAD}")
    if(result)
        message(FATAL_ERROR "CMake step for googletest failed: ${result}")
    endif()

    # Clone (in sources folder)
    execute_process(COMMAND ${CMAKE_COMMAND} --build .
        RESULT_VARIABLE result
        WORKING_DIRECTORY "${COLLAB_GTEST_DOWNLOAD}")
    if(result)
        message(FATAL_ERROR "Build step for googletest failed: ${result}")
    endif()

    # Just to place headers at googletest root folder as well
    file(COPY "${COLLAB_GTEST_SOURCES}/googletest/include"
         DESTINATION "${COLLAB_GTEST_DIR}")

elseif(NOT IS_DIRECTORY "${COLLAB_GTEST_DIR}")
    message("WARNING: GoogleTest dependency is missing.")
    message("To download it automatically, set -DCOLLAB_DEPENDENCIES_DOWNLOAD=ON")
    message(FATAL_ERROR "Missing dependency...")
endif()


# Prevent overriding the parent project's compiler/linker settings on Windows
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

# Add googletest directly to our build. This defines
# the gtest and gtest_main targets.
add_subdirectory("${COLLAB_GTEST_SOURCES}"
                 "${COLLAB_GTEST_BUILD}"
                 EXCLUDE_FROM_ALL)

# The gtest/gtest_main targets carry header search path
# dependencies automatically when using CMake 2.8.11 or
# later. Otherwise we have to add them here ourselves.
if (CMAKE_VERSION VERSION_LESS 2.8.11)
    include_directories("${gtest_SOURCE_DIR}/include")
endif()


