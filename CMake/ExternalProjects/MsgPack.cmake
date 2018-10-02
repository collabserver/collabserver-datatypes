cmake_minimum_required(VERSION 2.8.2 FATAL_ERROR)

project(msgpack-download NONE)

include(ExternalProject)
ExternalProject_Add(msgpack
    GIT_REPOSITORY      "https://github.com/msgpack/msgpack-c.git"
    GIT_TAG             "cpp-3.1.1"
    SOURCE_DIR          "${COLLAB_MSGPACK_SOURCES}"
    BINARY_DIR          "${COLLAB_MSGPACK_BUILD}"
    CONFIGURE_COMMAND   ""
    BUILD_COMMAND       ""
    INSTALL_COMMAND     ""
    TEST_COMMAND        ""
    CMAKE_ARGS          "-DMSGPACK_CXX11=ON"
)
