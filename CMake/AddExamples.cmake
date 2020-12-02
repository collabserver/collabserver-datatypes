include_directories("${CMAKE_CURRENT_SOURCE_DIR}/include/")

# CmRDT examples
add_executable(examplesCmRDT "${CMAKE_CURRENT_SOURCE_DIR}/examples/CmRDT/runAllExamples.cpp")
add_custom_target(runExamplesCmRDT examplesCmRDT)

