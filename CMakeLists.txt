cmake_minimum_required(VERSION 3.26.0 FATAL_ERROR)

set(CMAKE_SYSTEM_VERSION 10.0 CACHE STRING "" FORCE)
set(CMAKE_CXX_STANDARD 20 CACHE STRING "The C++ standard to use")
set(CMAKE_C_STANDARD 23 CACHE STRING "The C standard to use")

set(CMAKE_OSX_DEPLOYMENT_TARGET "10.15" CACHE STRING "Minimum OS X deployment version")

project(Ship VERSION 9.2.3 LANGUAGES C CXX)
include(CMake/soh-cvars.cmake)
include(CMake/lus-cvars.cmake)
set(SPDLOG_LEVEL_TRACE 0)
set(SPDLOG_LEVEL_OFF 6)
set(SPDLOG_MIN_CUTOFF SPDLOG_LEVEL_TRACE CACHE STRING "cutoff at trace")
option(USE_ASAN "Use Address Sanitizer" OFF)

option(SUPPRESS_WARNINGS "Suppress warnings in LUS and src (decomp)" ON)
if(SUPPRESS_WARNINGS)
    MESSAGE("Suppressing warnings in LUS and src")
    if(MSVC)
        set(WARNING_OVERRIDE /w)
    else()
        set(WARNING_OVERRIDE -w)
    endif()
else()
    MESSAGE("Skipping warning suppression")
endif()

set(NATO_PHONETIC_ALPHABET
  "Alfa" "Bravo" "Charlie" "Delta" "Echo" "Foxtrot" "Golf" "Hotel"
  "India" "Juliett" "Kilo" "Lima" "Mike" "November" "Oscar" "Papa"
  "Quebec" "Romeo" "Sierra" "Tango" "Uniform" "Victor" "Whiskey"
  "Xray" "Yankee" "Zulu"
)

# Get the patch version number from the project version
math(EXPR PATCH_INDEX "${PROJECT_VERSION_PATCH}")

# Use the patch number to select the correct word
list(GET NATO_PHONETIC_ALPHABET ${PATCH_INDEX} PROJECT_PATCH_WORD)

set(PROJECT_BUILD_NAME "Ackbar ${PROJECT_PATCH_WORD}" CACHE STRING "" FORCE)
set(PROJECT_TEAM "github.com/harbourmasters" CACHE STRING "" FORCE)

execute_process(
    COMMAND git branch --show-current
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    OUTPUT_VARIABLE GIT_BRANCH
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

set(CMAKE_PROJECT_GIT_BRANCH "${GIT_BRANCH}" CACHE STRING "Git branch" FORCE)

execute_process(
    COMMAND git rev-parse HEAD
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    OUTPUT_VARIABLE GIT_COMMIT_HASH
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

# Get only the first 7 characters of the hash
string(SUBSTRING "${GIT_COMMIT_HASH}" 0 7 SHORT_COMMIT_HASH)

set(CMAKE_PROJECT_GIT_COMMIT_HASH "${SHORT_COMMIT_HASH}" CACHE STRING "Git commit hash" FORCE)

execute_process(
    COMMAND git describe --tags --abbrev=0 --exact-match HEAD
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    OUTPUT_VARIABLE GIT_COMMIT_TAG
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

if(NOT GIT_COMMIT_TAG)
    set(GIT_COMMIT_TAG "" CACHE STRING "Git commit tag" FORCE)
endif()

set(CMAKE_PROJECT_GIT_COMMIT_TAG "${GIT_COMMIT_TAG}" CACHE STRING "Git commit tag" FORCE)

set_property(DIRECTORY ${CMAKE_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT soh)
add_compile_options($<$<CXX_COMPILER_ID:MSVC>:/MP>)
add_compile_options($<$<CXX_COMPILER_ID:MSVC>:/utf-8>)
add_compile_options($<$<CXX_COMPILER_ID:MSVC>:/Zc:preprocessor>)
add_compile_options($<$<CXX_COMPILER_ID:MSVC>:/external:W0>)

if (CMAKE_SYSTEM_NAME STREQUAL "Windows")
    if(NOT CMAKE_VS_PLATFORM_NAME)
        set(CMAKE_VS_PLATFORM_NAME "x64")
    endif()

    if("${CMAKE_VS_PLATFORM_NAME}" MATCHES "^[Aa][Rr][Mm]64$")
        set(SOH_WINDOWS_ARM64 TRUE)
    else()
        set(SOH_WINDOWS_ARM64 FALSE)
    endif()

    include(CMake/automate-vcpkg.cmake)

    set(VCPKG_TRIPLET x64-windows-static)
    set(VCPKG_TARGET_TRIPLET x64-windows-static)
    if(SOH_WINDOWS_ARM64)
        set(VCPKG_TRIPLET arm64-windows-static)
        set(VCPKG_TARGET_TRIPLET arm64-windows-static)
    endif()

    vcpkg_bootstrap()
    vcpkg_install_packages(zlib bzip2 libzip libpng sdl2 sdl2-net glew glfw3 nlohmann-json tinyxml2 spdlog libogg libvorbis opus opusfile)
    if (CMAKE_C_COMPILER_LAUNCHER MATCHES "ccache|sccache")
        set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT Embedded)
    endif()
endif()

################################################################################
# Set target arch type if empty. Visual studio solution generator provides it.
################################################################################
if (CMAKE_SYSTEM_NAME STREQUAL "Windows")
    if(NOT CMAKE_VS_PLATFORM_NAME)
        set(CMAKE_VS_PLATFORM_NAME "x64")
    endif()
    message("${CMAKE_VS_PLATFORM_NAME} architecture in use")

    if(NOT ("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64"
        OR "${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Win32"
        OR SOH_WINDOWS_ARM64))
        message(FATAL_ERROR "${CMAKE_VS_PLATFORM_NAME} arch is not supported!")
    endif()
endif()

################################################################################
# Global configuration types
################################################################################
if (CMAKE_SYSTEM_NAME STREQUAL "NintendoSwitch")
set(CMAKE_C_FLAGS_DEBUG "-g -DDEBUG")
set(CMAKE_CXX_FLAGS_DEBUG "-g -DDEBUG")
set(CMAKE_C_FLAGS_RELEASE "-O3 -DNDEBUG")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG")
else()
set(CMAKE_C_FLAGS_RELEASE "-O2 -DNDEBUG")
set(CMAKE_CXX_FLAGS_RELEASE "-O2 -DNDEBUG")
set(CMAKE_OBJCXX_FLAGS_RELEASE "-O2 -DNDEBUG")
endif()

# Enforce strict, deterministic IEEE-754 floating-point to match N64 (MIPS) semantics.
# Never enable fast-math, and disable FMA contraction: the R4300 CPU and RSP have no
# fused multiply-add, so contracting mul+add into an FMA produces extra precision.
add_compile_options($<$<CXX_COMPILER_ID:GNU,Clang,AppleClang>:-fno-fast-math>)
add_compile_options($<$<CXX_COMPILER_ID:GNU,Clang,AppleClang>:-ffp-contract=off>)
add_compile_options($<$<CXX_COMPILER_ID:MSVC>:/fp:precise>)
add_compile_options($<$<CXX_COMPILER_ID:MSVC>:/fp:except->)

if(NOT CMAKE_BUILD_TYPE )
    set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "Choose the type of build." FORCE)
endif()

################################################################################
# Common utils
################################################################################
include(CMake/Utils.cmake)

if(CMAKE_SYSTEM_NAME MATCHES "Linux")
    get_linux_lsb_release_information()
    message(STATUS "Linux ${LSB_RELEASE_ID_SHORT} ${LSB_RELEASE_VERSION_SHORT} ${LSB_RELEASE_CODENAME_SHORT}")
else()
    message(STATUS ${CMAKE_SYSTEM_NAME})
endif()

################################################################################
# Additional Global Settings(add specific info there)
################################################################################
include(CMake/GlobalSettingsInclude.cmake OPTIONAL)

################################################################################
# Use solution folders feature
################################################################################
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

################################################################################
# Set LUS vars
################################################################################

# Enable the Gfx debugger in LUS, which fetches its own libgfxd
set(GFX_DEBUG_DISASSEMBLER ON)

# Tell LUS we're using F3DEX_GBI_2 (in a way that doesn't break libgfxd)
set(GBI_UCODE F3DEX_GBI_2)
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/CMake")

# Enable MPQ and OTR support
set(INCLUDE_MPQ_SUPPORT ON)

################################################################################
# Set CONTROLLERBUTTONS_T
################################################################################
add_compile_definitions(CONTROLLERBUTTONS_T=uint32_t)

################################################################################
# Torch (ROM asset extraction)
################################################################################
find_package(ZLIB REQUIRED)

include(FetchContent)

# spdlog runs fmt's consteval format check in every log macro, and clang >= 21 rejects it as
# not constant with fmt < 11.1: FMT_STRING's compile string converts to two distinct literal
# objects, so the pointer subtraction fmt does while parsing crosses objects (#7017). Ignore a
# system spdlog built against such an fmt; the fetched one bundles fmt 12.
set(_use_system_spdlog ON)
if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang"
   AND CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL "21.0")
    find_package(fmt QUIET)
    if(fmt_FOUND AND fmt_VERSION VERSION_LESS "11.1")
        message(STATUS "clang ${CMAKE_CXX_COMPILER_VERSION} can't build against fmt ${fmt_VERSION}, fetching spdlog")
        set(_use_system_spdlog OFF)
    endif()
endif()

# Workaround: torch declares spdlog without OVERRIDE_FIND_PACKAGE and the first declaration
# wins, which leaves libultraship's find_package(spdlog REQUIRED) with no config to find.
# Declaring it here first keeps that working. See HarbourMasters/Torch#233.
if(_use_system_spdlog)
    # 1.10 is minimum for spdlog::fmt_lib
    find_package(spdlog 1.10 QUIET)
endif()
if(NOT spdlog_FOUND)
    FetchContent_Declare(
        spdlog
        GIT_REPOSITORY https://github.com/gabime/spdlog.git
        GIT_TAG v1.16.0
        OVERRIDE_FIND_PACKAGE
    )
    FetchContent_MakeAvailable(spdlog)
endif()

set(USE_STANDALONE          OFF CACHE BOOL "" FORCE)  # static lib, no CLI
set(PORT_VERSION_ENDIANNESS ON  CACHE BOOL "" FORCE)  # 7-byte portVersion
set(ROM_CRC_BSWAP           OFF CACHE BOOL "" FORCE)
set(BUILD_UI                OFF CACHE BOOL "" FORCE)  # would fetch a 2nd libultraship
set(BUILD_STORMLIB          OFF CACHE BOOL "" FORCE)  # target name clashes with LUS's `storm`
set(BUILD_NAUDIO            OFF CACHE BOOL "" FORCE)

set(BUILD_OOT               ON  CACHE BOOL "" FORCE)
foreach(_torch_game SM64 MK64 SF64 PM64 FZERO BK64 MARIO_ARTIST)
    set(BUILD_${_torch_game} OFF CACHE BOOL "" FORCE)
endforeach()

# SYSTEM only covers headers we include; torch and the libraries it fetches still warn while
# compiling their own sources. None of that is ours to fix.
if(MSVC)
    set(DEP_WARNING_OVERRIDE /w)
else()
    set(DEP_WARNING_OVERRIDE -w)
endif()

# Drops the target's own warning level before adding ours: cl warns (D9025) about the
# conflict instead of just taking the last flag. /O3 goes too, MSVC does not know it
# (D9002) and the -O2 in CMAKE_CXX_FLAGS_RELEASE is what these actually get.
function(quiet_target target override)
    if(NOT override)
        return()
    endif()
    get_target_property(_opts ${target} COMPILE_OPTIONS)
    if(_opts)
        list(REMOVE_ITEM _opts /O3 /W0 /W1 /W2 /W3 /W4 /Wall -Wall)
        set_target_properties(${target} PROPERTIES COMPILE_OPTIONS "${_opts}")
    endif()
    target_compile_options(${target} PRIVATE ${override})
endfunction()

function(quiet_dependency_tree dir)
    get_property(_targets DIRECTORY "${dir}" PROPERTY BUILDSYSTEM_TARGETS)
    foreach(_target IN LISTS _targets)
        get_target_property(_type ${_target} TYPE)
        if(_type STREQUAL "INTERFACE_LIBRARY" OR _type STREQUAL "UTILITY")
            continue()
        endif()
        quiet_target(${_target} "${DEP_WARNING_OVERRIDE}")
    endforeach()
    get_property(_subdirs DIRECTORY "${dir}" PROPERTY SUBDIRECTORIES)
    foreach(_subdir IN LISTS _subdirs)
        quiet_dependency_tree("${_subdir}")
    endforeach()
endfunction()

add_subdirectory(torch SYSTEM)
quiet_dependency_tree(${CMAKE_CURRENT_SOURCE_DIR}/torch)

target_compile_definitions(torch INTERFACE YAML_CPP_STATIC_DEFINE)

# Build-time ROM extraction. soh links torch as a static library, which compiles out torch's
# own CLI, so this supplies an entry point around the same code the game runs.
add_executable(soh-torch EXCLUDE_FROM_ALL
    ${CMAKE_CURRENT_SOURCE_DIR}/soh/assets/tools/torch-cli/main.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/soh/soh/Extractor/TorchExtract.cpp
)
target_include_directories(soh-torch PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/soh/soh/Extractor)
target_link_libraries(soh-torch PRIVATE torch)

add_executable(soh-o2r-packer EXCLUDE_FROM_ALL
    ${CMAKE_CURRENT_SOURCE_DIR}/soh/assets/tools/soh-o2r-packer/main.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/soh/assets/tools/soh-o2r-packer/PngTexture.cpp
)
target_link_libraries(soh-o2r-packer PRIVATE torch)

if(MSVC)
    set_target_properties(soh-torch soh-o2r-packer PROPERTIES
        MSVC_RUNTIME_LIBRARY "$<IF:$<CONFIG:Debug>,MultiThreadedDebug,MultiThreaded>")
endif()

# Target to generate OTRs. SOH_ROM_PATH takes roms and/or directories of roms; torch names each
# archive (oot.o2r or oot-mq.o2r) from its hash, so a vanilla and a master quest rom produce both
# in one run. soh.o2r comes from GenerateSohOtr, chained below.
set(SOH_ROM_PATH "${CMAKE_SOURCE_DIR}/roms" CACHE STRING "Roms, or directories of roms, to extract")
add_custom_target(
    ExtractAssets
    COMMAND ${CMAKE_COMMAND} -E rm -f ${CMAKE_BINARY_DIR}/soh/oot.o2r ${CMAKE_BINARY_DIR}/soh/oot-mq.o2r
    COMMAND $<TARGET_FILE:soh-torch>
            --src ${CMAKE_CURRENT_SOURCE_DIR}/soh/assets/yml
            --dest ${CMAKE_BINARY_DIR}/soh
            --version ${CMAKE_PROJECT_VERSION}
            ${SOH_ROM_PATH}
    # torch caches extraction state next to the archive; drop it so the game's directory stays clean
    COMMAND ${CMAKE_COMMAND} -E rm -f ${CMAKE_BINARY_DIR}/soh/torch.hash.yml
    COMMENT "Running asset extraction..."
    DEPENDS soh-torch
    BYPRODUCTS ${CMAKE_BINARY_DIR}/soh/oot.o2r ${CMAKE_BINARY_DIR}/soh/oot-mq.o2r
)

# Target to generate headers. Torch has no OoT header exporter yet -- every REGISTER in
# src/factories/oot/ is REGISTER(Binary, ...) -- and it emits no `#define d<sym>` line, which
# soh source references ~24k times. Kept so the failure explains itself.
add_custom_target(
    ExtractAssetHeaders
    COMMAND ${CMAKE_COMMAND} -E echo "ExtractAssetHeaders is currently unavailable: torch does not emit OoT asset headers yet."
    COMMAND ${CMAKE_COMMAND} -E echo "The checked-in headers under soh/assets are unaffected. See docs/BUILDING.md."
    COMMAND ${CMAKE_COMMAND} -E false
)

# Target to generate only soh.o2r
add_custom_target(
    GenerateSohOtr
    # copy LUS default shaders into assets/custom
    COMMAND ${CMAKE_COMMAND} -E rm -r -f ${CMAKE_CURRENT_SOURCE_DIR}/soh/assets/custom/shaders/
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/libultraship/src/fast/shaders/ ${CMAKE_CURRENT_SOURCE_DIR}/soh/assets/custom/shaders/

    COMMAND $<TARGET_FILE:soh-o2r-packer>
            ${CMAKE_CURRENT_SOURCE_DIR}/soh/assets/custom
            ${CMAKE_BINARY_DIR}/soh/soh.o2r
            ${CMAKE_PROJECT_VERSION}
    COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_BINARY_DIR}/soh/soh.o2r ${CMAKE_CURRENT_SOURCE_DIR}/soh.o2r
    COMMENT "Generating soh.o2r..."
    DEPENDS soh-o2r-packer
    BYPRODUCTS ${CMAKE_SOURCE_DIR}/soh.o2r ${CMAKE_BINARY_DIR}/soh/soh.o2r
)

# ExtractAssets produced soh.o2r as well as the rom archives, so keep doing that.
add_dependencies(ExtractAssets GenerateSohOtr)

# The asset tools only need torch, so a tools-only configure can skip libultraship and soh
# entirely -- that is all CI needs to produce soh.o2r.
option(SOH_TOOLS_ONLY "Configure only the asset tools, skipping libultraship and soh" OFF)
if(SOH_TOOLS_ONLY)
    file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/soh)
    return()
endif()

################################################################################
# Sub-projects
################################################################################
add_subdirectory(libultraship ${CMAKE_BINARY_DIR}/libultraship SYSTEM)
quiet_target(libultraship "${WARNING_OVERRIDE}")
target_compile_definitions(libultraship PUBLIC INCLUDE_MPQ_SUPPORT)
add_subdirectory(soh)

set_property(TARGET soh PROPERTY APPIMAGE_DESKTOP_FILE_TERMINAL YES)
set_property(TARGET soh PROPERTY APPIMAGE_DESKTOP_FILE "${CMAKE_SOURCE_DIR}/scripts/linux/appimage/soh.desktop")
set_property(TARGET soh PROPERTY APPIMAGE_ICON_FILE "${CMAKE_BINARY_DIR}/sohIcon.png")

if("${CMAKE_SYSTEM_NAME}" STREQUAL "Linux")
install(FILES "${CMAKE_BINARY_DIR}/soh/soh.o2r" DESTINATION . COMPONENT ship)
install(DIRECTORY "${CMAKE_SOURCE_DIR}/soh/assets/yml/" DESTINATION assets COMPONENT ship)
endif()

if ("${CMAKE_SYSTEM_NAME}" STREQUAL "Windows")
install(DIRECTORY "${CMAKE_SOURCE_DIR}/soh/assets/yml/" DESTINATION assets COMPONENT ship)
endif()

if(CMAKE_SYSTEM_NAME MATCHES "Linux")
    find_package(ImageMagick COMPONENTS convert)
    if (ImageMagick_FOUND)
        execute_process (
            COMMAND ${ImageMagick_convert_EXECUTABLE} ${CMAKE_SOURCE_DIR}/soh/macosx/sohIcon.png -resize 512x512 ${CMAKE_BINARY_DIR}/sohIcon.png
            OUTPUT_VARIABLE outVar
        )
    endif()
endif()

if(CMAKE_SYSTEM_NAME MATCHES "Darwin")
add_custom_target(CreateOSXIcons
    COMMAND mkdir -p ${CMAKE_BINARY_DIR}/macosx/soh.iconset
    COMMAND sips -z 16 16     ${CMAKE_SOURCE_DIR}/soh/macosx/sohIcon.png --out ${CMAKE_BINARY_DIR}/macosx/soh.iconset/icon_16x16.png
    COMMAND sips -z 32 32     ${CMAKE_SOURCE_DIR}/soh/macosx/sohIcon.png --out ${CMAKE_BINARY_DIR}/macosx/soh.iconset/icon_16x16@2x.png
    COMMAND sips -z 32 32     ${CMAKE_SOURCE_DIR}/soh/macosx/sohIcon.png --out ${CMAKE_BINARY_DIR}/macosx/soh.iconset/icon_32x32.png
    COMMAND sips -z 64 64     ${CMAKE_SOURCE_DIR}/soh/macosx/sohIcon.png --out ${CMAKE_BINARY_DIR}/macosx/soh.iconset/icon_32x32@2x.png
    COMMAND sips -z 128 128   ${CMAKE_SOURCE_DIR}/soh/macosx/sohIcon.png --out ${CMAKE_BINARY_DIR}/macosx/soh.iconset/icon_128x128.png
    COMMAND sips -z 256 256   ${CMAKE_SOURCE_DIR}/soh/macosx/sohIcon.png --out ${CMAKE_BINARY_DIR}/macosx/soh.iconset/icon_128x128@2x.png
    COMMAND sips -z 256 256   ${CMAKE_SOURCE_DIR}/soh/macosx/sohIcon.png --out ${CMAKE_BINARY_DIR}/macosx/soh.iconset/icon_256x256.png
    COMMAND sips -z 512 512   ${CMAKE_SOURCE_DIR}/soh/macosx/sohIcon.png --out ${CMAKE_BINARY_DIR}/macosx/soh.iconset/icon_256x256@2x.png
    COMMAND sips -z 512 512   ${CMAKE_SOURCE_DIR}/soh/macosx/sohIcon.png --out ${CMAKE_BINARY_DIR}/macosx/soh.iconset/icon_512x512.png
    COMMAND cp                ${CMAKE_SOURCE_DIR}/soh/macosx/sohIcon.png ${CMAKE_BINARY_DIR}/macosx/soh.iconset/icon_512x512@2x.png
    COMMAND iconutil -c icns -o ${CMAKE_BINARY_DIR}/macosx/soh.icns ${CMAKE_BINARY_DIR}/macosx/soh.iconset
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    COMMENT "Creating OSX icons ..."
    )
add_dependencies(soh CreateOSXIcons)

set(PROGRAM_PERMISSIONS_EXECUTE OWNER_EXECUTE OWNER_WRITE OWNER_READ GROUP_EXECUTE GROUP_READ WORLD_EXECUTE WORLD_READ)

install(DIRECTORY "${CMAKE_SOURCE_DIR}/soh/assets/yml/" DESTINATION assets)

# Rename the installed soh binary to drop the macos suffix
INSTALL(CODE "FILE(RENAME \${CMAKE_INSTALL_PREFIX}/../MacOS/soh-macos \${CMAKE_INSTALL_PREFIX}/../MacOS/soh)")

install(CODE "
    include(BundleUtilities)
    fixup_bundle(\"\${CMAKE_INSTALL_PREFIX}/../MacOS/soh\" \"\" \"${dirs}\")
    ")

endif()

if(CMAKE_SYSTEM_NAME MATCHES "Windows|NintendoSwitch|CafeOS")
install(FILES ${CMAKE_SOURCE_DIR}/README.md DESTINATION . COMPONENT ship RENAME readme.txt )
endif()

if(CMAKE_SYSTEM_NAME MATCHES "Linux")
    set(CPACK_GENERATOR "External")
elseif(CMAKE_SYSTEM_NAME MATCHES "Windows|NintendoSwitch|CafeOS")
    set(CPACK_GENERATOR "ZIP")
elseif(CMAKE_SYSTEM_NAME MATCHES "Darwin")
    set(CPACK_GENERATOR "Bundle")
endif()

set(CPACK_PROJECT_CONFIG_FILE ${CMAKE_SOURCE_DIR}/CMake/Packaging-2.cmake)
include(CMake/Packaging.cmake)
