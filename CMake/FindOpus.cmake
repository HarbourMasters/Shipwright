# - FindOpus.cmake
# Find the native opus includes and libraries
#
# OPUS_INCLUDE_DIRS - where to find opus/opus.h, etc.
# OPUS_LIBRARIES - List of libraries when using libopus(file).
# OPUS_FOUND - True if libopus found.

if(OPUS_INCLUDE_DIR AND OPUS_LIBRARY AND OPUSFILE_LIBRARY)
    # Already in cache, be silent
    set(OPUS_FIND_QUIETLY TRUE)
endif(OPUS_INCLUDE_DIR AND OPUS_LIBRARY AND OPUSFILE_LIBRARY)

find_path(OPUS_INCLUDE_DIR
    NAMES opusfile.h
    PATH_SUFFIXES opus
)

# MSVC built opus may be named opus_static
# The provided project files name the library with the lib prefix.
find_library(OPUS_LIBRARY
    NAMES opus opus_static libopus libopus_static
)
#find_library(OPUSFILE_LIBRARY
#    NAMES opusfile opusfile_static libopusfile libopusfile_static
#)

# Handle the QUIETLY and REQUIRED arguments and set OPUS_FOUND
# to TRUE if all listed variables are TRUE.
include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(Opus DEFAULT_MSG
    OPUS_LIBRARY OPUS_INCLUDE_DIR
)

if(OPUS_FOUND)
    set(OPUS_LIBRARIES  ${OPUS_LIBRARY})
    set(OPUS_INCLUDE_DIRS ${OPUS_INCLUDE_DIR})
    if(NOT TARGET Opus::opus)
    add_library(Opus::opus UNKNOWN IMPORTED)
        set_target_properties(Opus::opus PROPERTIES
            INTERFACE_INCLUDE_DIRECTORIES "${OPUS_INCLUDE_DIRS}"
            IMPORTED_LOCATION "${OPUS_LIBRARIES}"
        )
    endif()
endif(OPUS_FOUND)
