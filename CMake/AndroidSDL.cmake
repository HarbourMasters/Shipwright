if(NOT ANDROID)
    message(FATAL_ERROR "CMake/AndroidSDL.cmake is only intended for Android builds")
endif()

if(NOT TARGET SDL2::SDL2)
    message(FATAL_ERROR "The libultraship Android setup did not provide SDL2::SDL2")
endif()

include(FetchContent)

# SDL2_net 2.2.0 is compatible with the SDL 2.32.10 dependency pulled in by LUS.
# Keep it shared: the native app and SDLActivity use Android's shared-library loader.
get_property(_android_had_build_shared_libs_cache CACHE BUILD_SHARED_LIBS PROPERTY TYPE SET)
if(_android_had_build_shared_libs_cache)
    get_property(_android_old_build_shared_libs_type CACHE BUILD_SHARED_LIBS PROPERTY TYPE)
    get_property(_android_old_build_shared_libs_value CACHE BUILD_SHARED_LIBS PROPERTY VALUE)
endif()
if(NOT _android_had_build_shared_libs_cache AND DEFINED BUILD_SHARED_LIBS)
    set(_android_had_build_shared_libs_normal TRUE)
    set(_android_old_build_shared_libs_normal "${BUILD_SHARED_LIBS}")
else()
    set(_android_had_build_shared_libs_normal FALSE)
endif()

# SDL_net's option() honors this temporary cache setting even if another
# dependency selected static libraries earlier in the build.
set(BUILD_SHARED_LIBS ON CACHE BOOL "Build SDL2_net as a shared library" FORCE)
set(BUILD_SHARED_LIBS ON)
set(SDL2NET_INSTALL OFF CACHE BOOL "Do not install SDL2_net from the Android build" FORCE)
set(SDL2NET_SAMPLES OFF CACHE BOOL "Do not build SDL2_net samples for Android" FORCE)

FetchContent_Declare(
    sdl2_net
    GIT_REPOSITORY https://github.com/libsdl-org/SDL_net.git
    GIT_TAG 669e75b84632e2c6cc5c65974ec9e28052cb7a4e
)
FetchContent_MakeAvailable(sdl2_net)

if(_android_had_build_shared_libs_cache)
    set(BUILD_SHARED_LIBS "${_android_old_build_shared_libs_value}"
        CACHE ${_android_old_build_shared_libs_type} "Restored after SDL2_net configuration" FORCE)
else()
    unset(BUILD_SHARED_LIBS CACHE)
endif()
if(_android_had_build_shared_libs_normal)
    set(BUILD_SHARED_LIBS "${_android_old_build_shared_libs_normal}")
else()
    unset(BUILD_SHARED_LIBS)
endif()

if(NOT TARGET SDL2_net)
    message(FATAL_ERROR "SDL2_net was not created from the pinned SDL_net source")
endif()

# SOH's legacy includes use <SDL2/SDL_net.h>; upstream SDL_net exposes SDL_net.h
# at the source root. The tiny forwarding header preserves both include layouts.
set(SOH_ANDROID_SDL2_NET_INCLUDE_DIR "${CMAKE_CURRENT_LIST_DIR}/AndroidSDL2NetInclude")
