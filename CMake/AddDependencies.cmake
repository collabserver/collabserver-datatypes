option(COLLAB_DEPENDENCIES_DOWNLOAD "Download dependencies" OFF)

# ------------------------------------------------------------------------------
# Dependencies path
# ------------------------------------------------------------------------------

# Priority: argument > env > default
if(DEFINED COLLAB_DEPENDENCIES_DIR)
    set(COLLAB_DEPENDENCIES_DIR ${COLLAB_DEPENDENCIES_DIR}
        CACHE PATH "Path to the dependencies folder")

elseif(IS_DIRECTORY $ENV{COLLAB_DEPENDENCIES_DIR})
    set(COLLAB_DEPENDENCIES_DIR $ENV{COLLAB_DEPENDENCIES_DIR}
        CACHE PATH "Path to the dependencies folder")

else()
    set(COLLAB_DEPENDENCIES_DIR "${CMAKE_BINARY_DIR}/dependencies"
        CACHE PATH "Path to the dependencies folder")
    file(MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/dependencies")

endif()

message(STATUS "Dependencies path: ${COLLAB_DEPENDENCIES_DIR}")
if(NOT IS_DIRECTORY ${COLLAB_DEPENDENCIES_DIR})
    # Here, must stop right now
    message("WARNING: Invalid dependencies folder")
    message(FATAL_ERROR "The path to dependencies is not a valid directory")
endif()


# ------------------------------------------------------------------------------
# Add Dependencies
# ------------------------------------------------------------------------------

include("CMake/Dependencies/AddGoogleTest.cmake")


