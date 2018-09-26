cmake_minimum_required(VERSION 2.8.2)

project(googletest-download NONE)

include(ExternalProject)
ExternalProject_Add(googletest
    GIT_REPOSITORY      "https://github.com/google/googletest.git"
    GIT_TAG             "master"
    SOURCE_DIR          "${COLLAB_DEP_GTEST}/sources"
    BINARY_DIR          "${COLLAB_DEP_GTEST}/build"
    CONFIGURE_COMMAND   ""
    BUILD_COMMAND       ""
    INSTALL_COMMAND     ""
    TEST_COMMAND        ""
)
