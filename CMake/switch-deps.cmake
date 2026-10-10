# Game-side dependencies for the Nintendo Switch build: SDL2_net (Anchor / Crowd Control networking)
# and Ogg/Vorbis (custom audio in mods). Uses devkitPro portlibs when present, otherwise builds
# them from source. Sets SOH_SWITCH_NET_LIBRARY and SOH_SWITCH_VORBIS_LIBRARIES.

include(FetchContent)

if(NOT DEFINED DEVKITPRO)
    if(DEFINED ENV{DEVKITPRO})
        set(DEVKITPRO $ENV{DEVKITPRO})
    else()
        set(DEVKITPRO /opt/devkitpro)
    endif()
endif()
set(_switch_portlibs ${DEVKITPRO}/portlibs/switch)

#=================== SDL2_net ===================
find_library(SOH_SWITCH_SDL2_NET_LIB SDL2_net PATHS ${_switch_portlibs}/lib NO_DEFAULT_PATH)
if(SOH_SWITCH_SDL2_NET_LIB AND EXISTS ${_switch_portlibs}/include/SDL2/SDL_net.h)
    add_library(soh_switch_sdl2_net STATIC IMPORTED GLOBAL)
    set_target_properties(soh_switch_sdl2_net PROPERTIES
        IMPORTED_LOCATION ${SOH_SWITCH_SDL2_NET_LIB}
        INTERFACE_INCLUDE_DIRECTORIES ${_switch_portlibs}/include
        INTERFACE_LINK_LIBRARIES SDL2::SDL2)
    set(SOH_SWITCH_NET_LIBRARY soh_switch_sdl2_net)
else()
    message(STATUS "Switch: SDL2_net not in portlibs, building from source")
    set(SDL2NET_INSTALL OFF CACHE BOOL "" FORCE)
    set(SDL2NET_SAMPLES OFF CACHE BOOL "" FORCE)
    FetchContent_Declare(SDL2_net
        GIT_REPOSITORY https://github.com/libsdl-org/SDL_net.git
        GIT_TAG release-2.2.0)
    FetchContent_MakeAvailable(SDL2_net)
    # The game includes <SDL2/SDL_net.h>; expose the header under that prefix.
    set(_net_inc ${CMAKE_BINARY_DIR}/sdl2_net_include)
    file(COPY ${sdl2_net_SOURCE_DIR}/SDL_net.h DESTINATION ${_net_inc}/SDL2)
    if(TARGET SDL2_net::SDL2_net-static)
        set(SOH_SWITCH_NET_LIBRARY SDL2_net::SDL2_net-static)
    elseif(TARGET SDL2_net::SDL2_net)
        set(SOH_SWITCH_NET_LIBRARY SDL2_net::SDL2_net)
    else()
        set(SOH_SWITCH_NET_LIBRARY SDL2_net)
    endif()
    target_include_directories(soh SYSTEM PRIVATE ${_net_inc})
endif()

#=================== Ogg / Vorbis / Opus ===================
# Ogg+Vorbis for custom sequences, Opus+opusfile for the mixer. Either all four come from portlibs or all four
# are built from source, so the two never mix copies of libogg.
set(_audio_libs vorbisfile vorbis opusfile opus ogg)
set(_have_all_audio TRUE)
foreach(_l IN LISTS _audio_libs)
    find_library(SOH_SWITCH_${_l}_LIB ${_l} PATHS ${_switch_portlibs}/lib NO_DEFAULT_PATH)
    if(NOT SOH_SWITCH_${_l}_LIB)
        set(_have_all_audio FALSE)
    endif()
endforeach()
if(NOT EXISTS ${_switch_portlibs}/include/opus/opusfile.h)
    set(_have_all_audio FALSE)
endif()

if(_have_all_audio)
    # Static link order matters: users before the libraries they depend on.
    set(SOH_SWITCH_VORBIS_LIBRARIES)
    foreach(_l IN LISTS _audio_libs)
        list(APPEND SOH_SWITCH_VORBIS_LIBRARIES ${SOH_SWITCH_${_l}_LIB})
    endforeach()
    # The game includes <opusfile.h> / <opus.h> without the opus/ prefix.
    target_include_directories(soh SYSTEM PRIVATE ${_switch_portlibs}/include/opus)
else()
    message(STATUS "Switch: Ogg/Vorbis/Opus not all in portlibs, building all from source")
    set(INSTALL_DOCS OFF CACHE BOOL "" FORCE)
    set(INSTALL_PKG_CONFIG_MODULE OFF CACHE BOOL "" FORCE)
    set(INSTALL_CMAKE_PACKAGE_MODULE OFF CACHE BOOL "" FORCE)
    set(BUILD_TESTING OFF CACHE BOOL "" FORCE)
    set(OPUS_BUILD_PROGRAMS OFF CACHE BOOL "" FORCE)
    set(OPUS_BUILD_TESTING OFF CACHE BOOL "" FORCE)
    set(OPUS_INSTALL_PKG_CONFIG_MODULE OFF CACHE BOOL "" FORCE)
    set(OPUS_INSTALL_CMAKE_CONFIG_MODULE OFF CACHE BOOL "" FORCE)
    set(OP_DISABLE_HTTP ON CACHE BOOL "" FORCE)
    set(OP_DISABLE_DOCS ON CACHE BOOL "" FORCE)
    set(OP_DISABLE_EXAMPLES ON CACHE BOOL "" FORCE)
    set(OP_DISABLE_FLOAT_API OFF CACHE BOOL "" FORCE)
    FetchContent_Declare(Ogg
        GIT_REPOSITORY https://github.com/xiph/ogg.git
        GIT_TAG v1.3.5
        OVERRIDE_FIND_PACKAGE)
    FetchContent_Declare(Opus
        GIT_REPOSITORY https://github.com/xiph/opus.git
        GIT_TAG v1.5.2
        OVERRIDE_FIND_PACKAGE)
    FetchContent_MakeAvailable(Ogg Opus)
    FetchContent_Declare(Vorbis
        GIT_REPOSITORY https://github.com/xiph/vorbis.git
        GIT_TAG v1.3.7)
    FetchContent_Declare(OpusFile
        GIT_REPOSITORY https://github.com/xiph/opusfile.git
        # v0.12 is autotools-only; this main-branch commit adds the CMake build.
        GIT_TAG 6dfd29e7adb87f2e193575fc3fa88cbf1a0b27df)
    FetchContent_MakeAvailable(Vorbis OpusFile)
    set(SOH_SWITCH_VORBIS_LIBRARIES Vorbis::vorbisfile Vorbis::vorbis OpusFile::opusfile Opus::opus Ogg::ogg)
endif()
