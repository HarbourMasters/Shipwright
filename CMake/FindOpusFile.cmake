# FindOpusFile.cmake
# Locate the libopusfile library and its dependencies (libopus and libogg).
# Defines the following variables on success:
#   OPUSFILE_FOUND       - Indicates if opusfile was found
#   OPUSFILE_INCLUDE_DIR - Directory containing opusfile.h
#   OPUSFILE_LIBRARY     - Path to the opusfile library
#   OPUSFILE_LIBRARIES   - Full list of libraries to link (opusfile, opus, ogg)

# Search for the OpusFile header
find_path(OPUSFILE_INCLUDE_DIR
    NAMES opusfile.h
    PATHS /usr/include/opus /usr/local/include/opus /opt/local/include/opus /opt/homebrew/include/opus
    DOC "Directory where opusfile.h is located"
)

# Search for the OpusFile library
find_library(OPUSFILE_LIBRARY
    NAMES opusfile
    DOC "Path to the libopusfile library"
)

# Search for the Opus library (dependency of OpusFile)
find_library(OPUS_LIBRARY
    NAMES opus
    DOC "Path to the libopus library (dependency of libopusfile)"
)

# Search for the Ogg library (dependency of OpusFile)
find_library(OGG_LIBRARY
    NAMES ogg
    DOC "Path to the libogg library (dependency of libopusfile)"
)

# Check if all required components are found
include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(OpusFile
    REQUIRED_VARS OPUSFILE_LIBRARY OPUSFILE_INCLUDE_DIR OPUS_LIBRARY OGG_LIBRARY
    VERSION_VAR OPUSFILE_VERSION
)

# Define an imported target if everything is found
if (OPUSFILE_FOUND)
    add_library(Opusfile::Opusfile INTERFACE IMPORTED)

    set_target_properties(Opusfile::Opusfile PROPERTIES
        INTERFACE_INCLUDE_DIRECTORIES "${OPUSFILE_INCLUDE_DIR}"
        INTERFACE_LINK_LIBRARIES "${OPUSFILE_LIBRARY};${OPUS_LIBRARY};${OGG_LIBRARY}"
    )

    # Optionally expose the include and libraries separately
    set(OPUSFILE_LIBRARIES ${OPUSFILE_LIBRARY} ${OPUS_LIBRARY} ${OGG_LIBRARY})
    set(OPUSFILE_INCLUDE_DIRS ${OPUSFILE_INCLUDE_DIR})
else()
    set(OPUSFILE_FOUND FALSE)
endif()
