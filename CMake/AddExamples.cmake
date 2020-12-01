include_directories("${CMAKE_SOURCE_DIR}/include/")

# CmRDT examples
add_executable(examplesCmRDT "${CMAKE_SOURCE_DIR}/examples/CmRDT/runAllExamples.cpp")
add_custom_target(runExamplesCmRDT examplesCmRDT)

# Custom examples
add_executable(examplesCustom "${CMAKE_SOURCE_DIR}/examples/custom/runAllExamples.cpp")
add_dependencies(examplesCustom collabdata)
add_custom_target(runExamplesCustom examplesCustom)
target_link_libraries(examplesCustom collabdata)

