enable_testing()


# ------------------------------------------------------------------------------
# Download and build GoogleTests
# ------------------------------------------------------------------------------
# See: https://github.com/google/googletest/tree/master/googletest

configure_file(
    "${CMAKE_SOURCE_DIR}/CMake/ExternalProjects/GoogleTest.cmake"
    "googletest-download/CMakeLists.txt")

execute_process(COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" .
    RESULT_VARIABLE result
    WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/googletest-download")
if(result)
    message(FATAL_ERROR "CMake step for googletest failed: ${result}")
endif()

execute_process(COMMAND ${CMAKE_COMMAND} --build .
    RESULT_VARIABLE result
    WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/googletest-download")
if(result)
    message(FATAL_ERROR "Build step for googletest failed: ${result}")
endif()

# Prevent overriding the parent project's compiler/linker settings on Windows
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

# Add googletest directly to our build. This defines
# the gtest and gtest_main targets.
add_subdirectory("${collab_dependency_dir}/googletest"
                 "${CMAKE_BINARY_DIR}/googletest-build"
                 EXCLUDE_FROM_ALL)

# The gtest/gtest_main targets carry header search path
# dependencies automatically when using CMake 2.8.11 or
# later. Otherwise we have to add them here ourselves.
if (CMAKE_VERSION VERSION_LESS 2.8.11)
    include_directories("${gtest_SOURCE_DIR}/include")
endif()


# ------------------------------------------------------------------------------
# Tests
# ------------------------------------------------------------------------------

include_directories("${CMAKE_SOURCE_DIR}/include/")
file(GLOB_RECURSE srcFilesTests "${CMAKE_SOURCE_DIR}/test/*.cpp")

add_executable(tests ${srcFilesTests})
target_link_libraries(tests gtest)
add_test(NAME googletests COMMAND tests)
add_custom_target(runAllTests tests)


