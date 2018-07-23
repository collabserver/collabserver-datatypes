include_directories("${CMAKE_SOURCE_DIR}/include/")

# CvRDT Examples
add_executable(examplesCvRDT "${CMAKE_SOURCE_DIR}/examples/CvRDT/runAllExamples.cpp")
add_custom_target(runExamplesCvRDT examplesCvRDT)

# CmRDT Examples
add_executable(examplesCmRDT "${CMAKE_SOURCE_DIR}/examples/CmRDT/runAllExamples.cpp")
add_custom_target(runExamplesCmRDT examplesCmRDT)

# Custom Examples
add_executable(examplesCustom "${CMAKE_SOURCE_DIR}/examples/custom/runAllExamples.cpp")
add_dependencies(examplesCustom collabdata)
add_custom_target(runExamplesCustom examplesCustom)
target_link_libraries(examplesCustom collabdata)


