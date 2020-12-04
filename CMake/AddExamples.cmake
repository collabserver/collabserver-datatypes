include_directories("${PROJECT_SOURCE_DIR}/include/")
add_executable(${PROJECT_NAME}-examplesCmRDT "${PROJECT_SOURCE_DIR}/examples/CmRDT/runAllExamples.cpp")
add_custom_target(runExamplesCmRDT ${PROJECT_NAME}-examplesCmRDT)

