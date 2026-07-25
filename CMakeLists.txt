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

# Enable the Gfx debugger in LUS to use libgfxd from ZAPDTR
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

FetchContent_Declare(
    torch
    GIT_REPOSITORY https://github.com/HarbourMasters/Torch.git
    GIT_TAG        4cae44160693e1beb562e39dc301bd42278be1f9
)
FetchContent_MakeAvailable(torch)

################################################################################
# Sub-projects
################################################################################
add_subdirectory(libultraship ${CMAKE_BINARY_DIR}/libultraship)
target_compile_options(libultraship PRIVATE "${WARNING_OVERRIDE}")
target_compile_definitions(libultraship PUBLIC INCLUDE_MPQ_SUPPORT)
add_subdirectory(ZAPDTR/ZAPD ${CMAKE_BINARY_DIR}/ZAPD)
add_subdirectory(OTRExporter)
add_subdirectory(soh)

set_property(TARGET soh PROPERTY APPIMAGE_DESKTOP_FILE_TERMINAL YES)
set_property(TARGET soh PROPERTY APPIMAGE_DESKTOP_FILE "${CMAKE_SOURCE_DIR}/scripts/linux/appimage/soh.desktop")
set_property(TARGET soh PROPERTY APPIMAGE_ICON_FILE "${CMAKE_BINARY_DIR}/sohIcon.png")

if("${CMAKE_SYSTEM_NAME}" STREQUAL "Linux")
install(FILES "${CMAKE_BINARY_DIR}/soh/soh.o2r" DESTINATION . COMPONENT ship)
install(TARGETS ZAPD DESTINATION ./assets/extractor COMPONENT extractor)
install(DIRECTORY "${CMAKE_SOURCE_DIR}/soh/assets/extractor/" DESTINATION ./assets COMPONENT extractor)
install(DIRECTORY "${CMAKE_SOURCE_DIR}/soh/assets/xml/" DESTINATION ./assets/xml COMPONENT extractor)
endif()

if ("${CMAKE_SYSTEM_NAME}" STREQUAL "Windows")
install(DIRECTORY "${CMAKE_SOURCE_DIR}/soh/assets/extractor/" DESTINATION ./assets/ COMPONENT ship)
install(DIRECTORY "${CMAKE_SOURCE_DIR}/soh/assets/xml/" DESTINATION ./assets/xml COMPONENT ship)
endif()

find_package(Python3 COMPONENTS Interpreter)

# Target to generate OTRs
add_custom_target(
    ExtractAssets
    COMMAND ${CMAKE_COMMAND} -E rm -f oot.o2r oot-mq.o2r soh.o2r

    # copy LUS default shaders into assets/custom
    COMMAND ${CMAKE_COMMAND} -E rm -r -f ${CMAKE_CURRENT_SOURCE_DIR}/soh/assets/custom/shaders/
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/libultraship/src/fast/shaders/ ${CMAKE_CURRENT_SOURCE_DIR}/soh/assets/custom/shaders/

    COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/OTRExporter/extract_assets.py -z "$<TARGET_FILE:ZAPD>" --non-interactive --xml-root assets/xml --custom-otr-file soh.o2r "--custom-assets-path" ${CMAKE_CURRENT_SOURCE_DIR}/soh/assets/custom --port-ver "${CMAKE_PROJECT_VERSION}"
    COMMAND ${CMAKE_COMMAND} -DSYSTEM_NAME=${CMAKE_SYSTEM_NAME} -DTARGET_DIR="$<TARGET_FILE_DIR:ZAPD>" -DSOURCE_DIR=${CMAKE_CURRENT_SOURCE_DIR} -DBINARY_DIR=${CMAKE_BINARY_DIR} -P ${CMAKE_CURRENT_SOURCE_DIR}/copy-existing-otrs.cmake
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/soh
    COMMENT "Running asset extraction..."
    DEPENDS ZAPD
    BYPRODUCTS oot.o2r ${CMAKE_SOURCE_DIR}/oot.o2r oot-mq.o2r ${CMAKE_SOURCE_DIR}/oot-mq.o2r ${CMAKE_SOURCE_DIR}/soh.o2r
)

# Target to generate headers
add_custom_target(
    ExtractAssetHeaders
    COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/OTRExporter/extract_assets.py -z "$<TARGET_FILE:ZAPD>" --non-interactive --xml-root assets/xml --gen-headers
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/soh
    COMMENT "Generating asset headers..."
    DEPENDS ZAPD
)

# TEMPORARY (Torch migration): soh.o2r is copied from prebuilt/, not generated.
#
# The real producer drives ZAPD via OTRExporter/extract_assets.py, and the migration deletes
# both. Its replacement -- a small in-tree packer, Phase 4 of torch-migration/PLAN.md -- has to
# reproduce ZAPD's texture quantisation exactly, so rather than block Phases 2/3/5 on it, the
# archive is checked in and copied into place. See prebuilt/README.md for provenance and for how
# to restore a real build step (which must also bring back the libultraship shader copy that the
# copy-only version below has no use for -- the shaders are already inside the archive).
#
# Fail at configure time if the prebuilt archive's baked-in portVersion no longer matches the
# project version. OTRGlobals.cpp:283 demands exact major.minor.patch equality and RunExtract
# exit(1)s with "soh.o2r is outdated" -- an obscure symptom for what is really a stale checked-in
# file. portVersion is 7 bytes: endianness, then u16 major/minor/patch big-endian.
set(SOH_PREBUILT_O2R "${CMAKE_CURRENT_SOURCE_DIR}/prebuilt/soh.o2r")
if(NOT EXISTS "${SOH_PREBUILT_O2R}")
    message(FATAL_ERROR "Missing ${SOH_PREBUILT_O2R} -- see prebuilt/README.md")
endif()
file(READ "${SOH_PREBUILT_O2R}" _soh_o2r_hex HEX)
string(FIND "${_soh_o2r_hex}" "706f727456657273696f6e" _pv_at)  # "portVersion" as hex
if(_pv_at EQUAL -1)
    message(FATAL_ERROR "No portVersion entry in ${SOH_PREBUILT_O2R} -- see prebuilt/README.md")
endif()
math(EXPR _pv_at "${_pv_at} + 22")  # skip the 11-character name
string(SUBSTRING "${_soh_o2r_hex}" ${_pv_at} 14 _pv)
string(SUBSTRING "${_pv}" 2 4 _pv_major)
string(SUBSTRING "${_pv}" 6 4 _pv_minor)
string(SUBSTRING "${_pv}" 10 4 _pv_patch)
math(EXPR _pv_major "0x${_pv_major}")
math(EXPR _pv_minor "0x${_pv_minor}")
math(EXPR _pv_patch "0x${_pv_patch}")
if(NOT "${_pv_major}.${_pv_minor}.${_pv_patch}" STREQUAL "${CMAKE_PROJECT_VERSION}")
    message(FATAL_ERROR
        "prebuilt/soh.o2r is stamped ${_pv_major}.${_pv_minor}.${_pv_patch} but this build is "
        "${CMAKE_PROJECT_VERSION}. The game would reject it at startup (\"soh.o2r is outdated\"). "
        "Regenerate it -- see prebuilt/README.md.")
endif()
unset(_soh_o2r_hex)

# Target to place soh.o2r (copy of the prebuilt archive)
add_custom_target(
    GenerateSohOtr
    COMMAND ${CMAKE_COMMAND} -E copy_if_different "${SOH_PREBUILT_O2R}" ${CMAKE_CURRENT_SOURCE_DIR}/soh.o2r
    COMMAND ${CMAKE_COMMAND} -E copy_if_different "${SOH_PREBUILT_O2R}" ${CMAKE_BINARY_DIR}/soh/soh.o2r
    # No BYPRODUCTS: ExtractAssets already claims ${CMAKE_SOURCE_DIR}/soh.o2r as one of its
    # own (see above), and two rules generating the same path is a hard ninja error.
    COMMENT "Copying prebuilt soh.o2r (TEMPORARY -- see prebuilt/README.md)..."
)

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

install(TARGETS ZAPD DESTINATION ${CMAKE_BINARY_DIR}/assets)

set(PROGRAM_PERMISSIONS_EXECUTE OWNER_EXECUTE OWNER_WRITE OWNER_READ GROUP_EXECUTE GROUP_READ WORLD_EXECUTE WORLD_READ)

install(DIRECTORY "${CMAKE_SOURCE_DIR}/soh/assets/extractor/" DESTINATION ./assets/)
install(DIRECTORY "${CMAKE_SOURCE_DIR}/soh/assets/xml/" DESTINATION ./assets/xml)

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
