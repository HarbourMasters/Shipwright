if(NOT ANDROID)
    message(FATAL_ERROR "CMake/AndroidDependencies.cmake is only intended for Android builds")
endif()

include(FetchContent)

# These codec libraries are also used by the desktop build, but Android cannot
# use host-installed binaries. Keep their source revisions pinned for NDK builds.
set(CMAKE_POLICY_DEFAULT_CMP0077 NEW)
set(BUILD_TESTING OFF CACHE BOOL "Do not build dependency tests for Android" FORCE)
set(INSTALL_DOCS OFF CACHE BOOL "Do not install dependency documentation" FORCE)
set(INSTALL_PKG_CONFIG_MODULE OFF CACHE BOOL "Do not generate dependency pkg-config files" FORCE)
set(INSTALL_CMAKE_PACKAGE_MODULE OFF CACHE BOOL "Do not generate dependency package configs" FORCE)
set(OPUS_BUILD_TESTING OFF CACHE BOOL "Do not build Opus tests for Android" FORCE)
set(OPUS_BUILD_PROGRAMS OFF CACHE BOOL "Do not build Opus command-line tools for Android" FORCE)
set(OPUS_BUILD_SHARED_LIBRARY OFF CACHE BOOL "Build Opus as a static library" FORCE)
set(OPUS_INSTALL_PKG_CONFIG_MODULE OFF CACHE BOOL "Do not generate Opus pkg-config files" FORCE)
set(OPUS_INSTALL_CMAKE_CONFIG_MODULE OFF CACHE BOOL "Do not generate Opus package configs" FORCE)

FetchContent_Declare(
    ogg
    GIT_REPOSITORY https://github.com/xiph/ogg.git
    GIT_TAG e1774cd77f471443541596e09078e78fdc342e4f
)
FetchContent_MakeAvailable(ogg)

# Vorbis' CMakeLists calls FindOgg.cmake instead of consuming an existing
# FetchContent target. Seed the module's variables so it resolves our target.
set(OGG_FOUND TRUE)
set(OGG_INCLUDE_DIR "${ogg_SOURCE_DIR}/include")
set(OGG_INCLUDE_DIRS "${OGG_INCLUDE_DIR}")
set(OGG_LIBRARY ogg)
set(OGG_LIBRARIES ogg)

FetchContent_Declare(
    vorbis
    GIT_REPOSITORY https://github.com/xiph/vorbis.git
    GIT_TAG 0657aee69dec8508a0011f47f3b69d7538e9d262
)
FetchContent_MakeAvailable(vorbis)

FetchContent_Declare(
    opus
    GIT_REPOSITORY https://github.com/xiph/opus.git
    GIT_TAG ddbe48383984d56acd9e1ab6a090c54ca6b735a
)
FetchContent_MakeAvailable(opus)

# libopusfile v0.12 has no upstream CMake project. Build its four core source
# files directly; HTTP/SSL support is unused by SOH's in-memory audio loader.
FetchContent_Declare(
    opusfile
    GIT_REPOSITORY https://github.com/xiph/opusfile.git
    GIT_TAG a55c164e9891a9326188b7d4d216ec9a88373739
)
FetchContent_GetProperties(opusfile)
if(NOT opusfile_POPULATED)
    # v0.12 contains no CMakeLists.txt, so it must be populated without
    # FetchContent_MakeAvailable. CMP0169 preserves the supported direct-populate
    # behavior on newer CMake releases.
    if(POLICY CMP0169)
        cmake_policy(PUSH)
        cmake_policy(SET CMP0169 OLD)
    endif()
    FetchContent_Populate(opusfile)
    if(POLICY CMP0169)
        cmake_policy(POP)
    endif()
endif()

add_library(soh_android_opusfile STATIC
    "${opusfile_SOURCE_DIR}/src/info.c"
    "${opusfile_SOURCE_DIR}/src/internal.c"
    "${opusfile_SOURCE_DIR}/src/opusfile.c"
    "${opusfile_SOURCE_DIR}/src/stream.c"
)
add_library(Opusfile::Opusfile ALIAS soh_android_opusfile)
target_include_directories(soh_android_opusfile
    PUBLIC "${opusfile_SOURCE_DIR}/include"
    PRIVATE "${opusfile_SOURCE_DIR}/src"
)
target_link_libraries(soh_android_opusfile PUBLIC Opus::opus Ogg::ogg)

message(STATUS "Android codec targets configured: Ogg, Vorbis, Opus and Opusfile")
