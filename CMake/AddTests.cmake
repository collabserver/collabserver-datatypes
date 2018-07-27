enable_testing()

include_directories("${CMAKE_SOURCE_DIR}/include/")
file(GLOB_RECURSE srcFilesTests "${CMAKE_SOURCE_DIR}/test/*.cpp")

add_executable(tests ${srcFilesTests})
target_link_libraries(tests gtest collabdata)
add_test(NAME googletests COMMAND tests)
add_custom_target(runTests tests)


