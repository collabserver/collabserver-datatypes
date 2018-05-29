# ------------------------------------------------------------------------------
# Examples
# ------------------------------------------------------------------------------


include_directories("${CMAKE_SOURCE_DIR}/include/")
add_executable(examplesCvRDT "${CMAKE_SOURCE_DIR}/examples/CvRDT/runAllExamples.cpp")
add_custom_target(runExamplesCvRDT examplesCvRDT)
