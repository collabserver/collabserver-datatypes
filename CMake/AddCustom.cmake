include_directories("${CMAKE_SOURCE_DIR}/include/")

file(GLOB_RECURSE srcFiles "${CMAKE_SOURCE_DIR}/src/*.cpp")

add_library(collabdata STATIC ${srcFiles})

