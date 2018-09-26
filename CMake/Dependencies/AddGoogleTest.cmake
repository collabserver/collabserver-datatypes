# ------------------------------------------------------------------------------
# Download and build GoogleTests at cmake time
# Inspired from googletest cmake
# See: https://github.com/google/googletest/tree/master/googletest
# ------------------------------------------------------------------------------

set(COLLAB_DEP_GTEST "${COLLAB_DEPENDENCIES_DIR}/googletest")

file(MAKE_DIRECTORY "${COLLAB_DEP_GTEST}/download")
file(MAKE_DIRECTORY "${COLLAB_DEP_GTEST}/sources")
file(MAKE_DIRECTORY "${COLLAB_DEP_GTEST}/include")
file(MAKE_DIRECTORY "${COLLAB_DEP_GTEST}/build")


# Create Download CMakeLists
configure_file(
    "${CMAKE_SOURCE_DIR}/CMake/ExternalProjects/GoogleTest.cmake"
    "${COLLAB_DEP_GTEST}/download/CMakeLists.txt")

# Setup Download (in download folder)
execute_process(COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" .
    RESULT_VARIABLE result
    WORKING_DIRECTORY "${COLLAB_DEP_GTEST}/download")
if(result)
    message(FATAL_ERROR "CMake step for googletest failed: ${result}")
endif()

# Clone (in sources folder)
if(COLLAB_DEPENDENCIES_DOWNLOAD)
    execute_process(COMMAND ${CMAKE_COMMAND} --build .
        RESULT_VARIABLE result
        WORKING_DIRECTORY "${COLLAB_DEP_GTEST}/download")
    if(result)
        message(FATAL_ERROR "Build step for googletest failed: ${result}")
    endif()
endif()

# Prevent overriding the parent project's compiler/linker settings on Windows
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

# Add googletest directly to our build. This defines
# the gtest and gtest_main targets.
add_subdirectory("${COLLAB_DEP_GTEST}/sources"
                 "${COLLAB_DEP_GTEST}/build"
                 EXCLUDE_FROM_ALL)

# The gtest/gtest_main targets carry header search path
# dependencies automatically when using CMake 2.8.11 or
# later. Otherwise we have to add them here ourselves.
if (CMAKE_VERSION VERSION_LESS 2.8.11)
    include_directories("${gtest_SOURCE_DIR}/include")
endif()


file(COPY        "${COLLAB_DEP_GTEST}/sources/googletest/include"
     DESTINATION "${COLLAB_DEP_GTEST}")
