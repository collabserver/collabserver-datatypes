option(COLLAB_DEPENDENCIES_DOWNLOAD "Download dependencies" ON)

file(MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/dependencies")
set(COLLAB_DEPENDENCIES_DIR "${CMAKE_BINARY_DIR}/dependencies"
    CACHE PATH "Path to the dependencies folder")

include("CMake/Dependencies/AddMsgPack.cmake")
include("CMake/Dependencies/AddGoogleTest.cmake")


