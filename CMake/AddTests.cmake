enable_testing()

# Tests executable
include_directories("${PROJECT_SOURCE_DIR}/include/")
file(GLOB_RECURSE srcFilesTests "${PROJECT_SOURCE_DIR}/tests/*.cpp")
add_executable(${PROJECT_NAME}-tests ${srcFilesTests})

# Googletest dependency
include_directories("${PROJECT_SOURCE_DIR}/extern/googletest/googletest/include/")
target_link_libraries(${PROJECT_NAME}-tests gtest)

# Tests target
add_test(NAME googletests COMMAND ${PROJECT_NAME}-tests)
add_custom_target(runTests ${PROJECT_NAME}-tests)

