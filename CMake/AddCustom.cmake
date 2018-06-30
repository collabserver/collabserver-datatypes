include_directories("${CMAKE_SOURCE_DIR}/include/")
file(GLOB_RECURSE srcFilesCustom "${CMAKE_SOURCE_DIR}/src/*.cpp")
add_library(collabdata STATIC ${srcFilesCustom})


