enable_testing()

# Dependencies
include("CMake/Dependencies/AddGoogleTest.cmake")


# Executables
file(GLOB_RECURSE srcFilesTests "${CMAKE_SOURCE_DIR}/test/*.cpp")

add_executable(tests ${srcFilesTests})
add_dependencies(tests collabdata)
target_link_libraries(tests gtest collabdata)

add_test(NAME googletests COMMAND tests)
add_custom_target(runTests tests)


