cmake_minimum_required(VERSION 2.8.2)

project(msgpack-download NONE)

include(ExternalProject)
ExternalProject_Add(msgpack
    GIT_REPOSITORY      "https://github.com/msgpack/msgpack-c.git"
    GIT_TAG             "master"
    SOURCE_DIR          "${collab_dependency_dir}/msgpack"
    BINARY_DIR          "${CMAKE_BINARY_DIR}/msgpack-build"
    CONFIGURE_COMMAND   ""
    BUILD_COMMAND       ""
    INSTALL_COMMAND     ""
    TEST_COMMAND        ""
)
