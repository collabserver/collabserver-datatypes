include(FetchContent)
FetchContent_Declare(
    googletest
    GIT_REPOSITORY  https://github.com/google/googletest.git
    GIT_TAG         release-1.8.1
    SOURCE_DIR      "${CMAKE_CURRENT_SOURCE_DIR}/extern/googletest"
)
FetchContent_MakeAvailable(googletest)
