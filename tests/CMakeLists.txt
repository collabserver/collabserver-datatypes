enable_testing()

# Tests executable
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/../include/")
file(GLOB_RECURSE srcFilesTests "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")
add_executable(tests ${srcFilesTests})

# Googletest dependency
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/extern/googletest/googletest/include/")
target_link_libraries(tests gtest)

# Tests target
add_test(NAME googletests COMMAND tests)
add_custom_target(runTests tests)

