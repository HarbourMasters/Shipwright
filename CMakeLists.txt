cmake_minimum_required(VERSION 3.16.0 FATAL_ERROR)

set(CMAKE_SYSTEM_VERSION 10.0 CACHE STRING "" FORCE)
set(CMAKE_CXX_STANDARD 20 CACHE STRING "The C++ standard to use")
#set(CMAKE_C_STANDARD 11 CACHE STRING "The C standard to use") - issue with soh compile with MSVC

set(CMAKE_OSX_DEPLOYMENT_TARGET "10.15" CACHE STRING "Minimum OS X deployment version" FORCE)

project(Ship C CXX)

set(PROJECT_VERSION_MAJOR "4")
set(PROJECT_VERSION_MINOR "0")
set(PROJECT_VERSION_PATCH "1")

set_property(DIRECTORY ${CMAKE_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT soh)
add_compile_options($<$<CXX_COMPILER_ID:MSVC>:/MP>)

if (CMAKE_SYSTEM_NAME STREQUAL "Windows")
include(CMake/automate-vcpkg.cmake)

set(VCPKG_TRIPLET x64-windows-static)
set(VCPKG_TARGET_TRIPLET x64-windows-static)

vcpkg_bootstrap()
vcpkg_install_packages(zlib bzip2 libpng SDL2 SDL2-net GLEW glfw3)
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
        OR "${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Win32"))
        message(FATAL_ERROR "${CMAKE_VS_PLATFORM_NAME} arch is not supported!")
    endif()
endif()

################################################################################
# Global configuration types
################################################################################
if (CMAKE_SYSTEM_NAME STREQUAL "NintendoSwitch")
set(CMAKE_C_FLAGS_DEBUG "-O3 -ffast-math")
set(CMAKE_CXX_FLAGS_DEBUG "-O3 -ffast-math")
set(CMAKE_C_FLAGS_RELEASE "-O3 -ffast-math -DNDEBUG")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -ffast-math -DNDEBUG")
else()
set(CMAKE_C_FLAGS_RELEASE "-O2 -DNDEBUG")
set(CMAKE_CXX_FLAGS_RELEASE "-O2 -DNDEBUG")
set(CMAKE_OBJCXX_FLAGS_RELEASE "-O2 -DNDEBUG")
endif()

if(NOT CMAKE_BUILD_TYPE )
    set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "Choose the type of build." FORCE)
endif()

################################################################################
# Global compiler options
################################################################################
if(MSVC)
    # remove default flags provided with CMake for MSVC
    set(CMAKE_C_FLAGS "")
    set(CMAKE_C_FLAGS_DEBUG "")
    set(CMAKE_C_FLAGS_RELEASE "")
    set(CMAKE_CXX_FLAGS "")
    set(CMAKE_CXX_FLAGS_DEBUG "")
    set(CMAKE_CXX_FLAGS_RELEASE "")
endif()

################################################################################
# Global linker options
################################################################################
if(MSVC)
    # remove default flags provided with CMake for MSVC
    set(CMAKE_EXE_LINKER_FLAGS "")
    set(CMAKE_MODULE_LINKER_FLAGS "")
    set(CMAKE_SHARED_LINKER_FLAGS "")
    set(CMAKE_STATIC_LINKER_FLAGS "")
    set(CMAKE_EXE_LINKER_FLAGS_DEBUG "${CMAKE_EXE_LINKER_FLAGS}")
    set(CMAKE_MODULE_LINKER_FLAGS_DEBUG "${CMAKE_MODULE_LINKER_FLAGS}")
    set(CMAKE_SHARED_LINKER_FLAGS_DEBUG "${CMAKE_SHARED_LINKER_FLAGS}")
    set(CMAKE_STATIC_LINKER_FLAGS_DEBUG "${CMAKE_STATIC_LINKER_FLAGS}")
    set(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS}")
    set(CMAKE_MODULE_LINKER_FLAGS_RELEASE "${CMAKE_MODULE_LINKER_FLAGS}")
    set(CMAKE_SHARED_LINKER_FLAGS_RELEASE "${CMAKE_SHARED_LINKER_FLAGS}")
    set(CMAKE_STATIC_LINKER_FLAGS_RELEASE "${CMAKE_STATIC_LINKER_FLAGS}")
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
# Sub-projects
################################################################################
add_subdirectory(libultraship/libultraship ${CMAKE_BINARY_DIR}/libultraship)
add_subdirectory(ZAPDTR/ZAPD ${CMAKE_BINARY_DIR}/ZAPD)
add_subdirectory(ZAPDTR/ZAPDUtils ${CMAKE_BINARY_DIR}/ZAPDUtils)
add_subdirectory(OTRExporter)
add_subdirectory(soh)
if(NOT CMAKE_SYSTEM_NAME MATCHES "Darwin|NintendoSwitch|CafeOS")
add_subdirectory(OTRGui)
endif()

set_property(TARGET soh PROPERTY APPIMAGE_DESKTOP_FILE_TERMINAL YES)
set_property(TARGET soh PROPERTY APPIMAGE_DESKTOP_FILE "${CMAKE_SOURCE_DIR}/scripts/linux/appimage/soh.desktop")
set_property(TARGET soh PROPERTY APPIMAGE_ICON_FILE "${CMAKE_BINARY_DIR}/sohIcon.png")

if("${CMAKE_SYSTEM_NAME}" STREQUAL "Linux")
install(PROGRAMS "${CMAKE_SOURCE_DIR}/scripts/linux/appimage/soh.sh" DESTINATION . COMPONENT appimage)
endif()

find_package(Python3 COMPONENTS Interpreter)

add_custom_target(
    ExtractAssets
    # CMake versions prior to 3.17 do not have the rm command, use remove instead for older versions
    COMMAND ${CMAKE_COMMAND} -E $<IF:$<VERSION_LESS:${CMAKE_VERSION},3.17>,remove,rm> -f oot.otr
    COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/OTRExporter/extract_assets.py -z "$<TARGET_FILE:ZAPD>"
    COMMAND ${CMAKE_COMMAND} -E copy oot.otr ${CMAKE_SOURCE_DIR}
    COMMAND ${CMAKE_COMMAND} -E copy oot.otr ${CMAKE_BINARY_DIR}/soh
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/OTRExporter
    COMMENT "Running asset extraction..."
    DEPENDS ZAPD
    BYPRODUCTS oot.otr ${CMAKE_SOURCE_DIR}/oot.otr
)

if(CMAKE_SYSTEM_NAME MATCHES "Linux")
    find_package(ImageMagick COMPONENTS convert)
    if (ImageMagick_FOUND)
        execute_process (
            COMMAND ${ImageMagick_convert_EXECUTABLE} soh/macosx/sohIcon.png -resize 512x512 ${CMAKE_BINARY_DIR}/sohIcon.png
            OUTPUT_VARIABLE outVar
        )
    endif()
endif()

if(CMAKE_SYSTEM_NAME MATCHES "Darwin")
add_custom_target(CreateOSXIcons
    COMMAND mkdir -p ${CMAKE_BINARY_DIR}/macosx/soh.iconset
    COMMAND sips -z 16 16     soh/macosx/sohIcon.png --out ${CMAKE_BINARY_DIR}/macosx/soh.iconset/icon_16x16.png
    COMMAND sips -z 32 32     soh/macosx/sohIcon.png --out ${CMAKE_BINARY_DIR}/macosx/soh.iconset/icon_16x16@2x.png
    COMMAND sips -z 32 32     soh/macosx/sohIcon.png --out ${CMAKE_BINARY_DIR}/macosx/soh.iconset/icon_32x32.png
    COMMAND sips -z 64 64     soh/macosx/sohIcon.png --out ${CMAKE_BINARY_DIR}/macosx/soh.iconset/icon_32x32@2x.png
    COMMAND sips -z 128 128   soh/macosx/sohIcon.png --out ${CMAKE_BINARY_DIR}/macosx/soh.iconset/icon_128x128.png
    COMMAND sips -z 256 256   soh/macosx/sohIcon.png --out ${CMAKE_BINARY_DIR}/macosx/soh.iconset/icon_128x128@2x.png
    COMMAND sips -z 256 256   soh/macosx/sohIcon.png --out ${CMAKE_BINARY_DIR}/macosx/soh.iconset/icon_256x256.png
    COMMAND sips -z 512 512   soh/macosx/sohIcon.png --out ${CMAKE_BINARY_DIR}/macosx/soh.iconset/icon_256x256@2x.png
    COMMAND sips -z 512 512   soh/macosx/sohIcon.png --out ${CMAKE_BINARY_DIR}/macosx/soh.iconset/icon_512x512.png
    COMMAND cp                soh/macosx/sohIcon.png ${CMAKE_BINARY_DIR}/macosx/soh.iconset/icon_512x512@2x.png
    COMMAND iconutil -c icns -o ${CMAKE_BINARY_DIR}/macosx/soh.icns ${CMAKE_BINARY_DIR}/macosx/soh.iconset
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    COMMENT "Creating OSX icons ..."
    )
add_dependencies(soh CreateOSXIcons)

add_custom_target(Assets ALL
    COMMAND ${CMAKE_COMMAND} -Dsrc_dir="${CMAKE_SOURCE_DIR}/OTRGui/assets/extractor" -Ddst_dir="${CMAKE_BINARY_DIR}/assets/extractor" -P "${CMAKE_SOURCE_DIR}/OTRGui/Overwrite.cmake"
    COMMAND ${CMAKE_COMMAND} -Dsrc_dir="${CMAKE_SOURCE_DIR}/OTRExporter/assets" -Ddst_dir="${CMAKE_BINARY_DIR}/assets/game" -P "${CMAKE_SOURCE_DIR}/OTRGui/Overwrite.cmake"
    COMMAND ${CMAKE_COMMAND} -Dsrc_dir="${CMAKE_SOURCE_DIR}/soh/assets/xml" -Ddst_dir="${CMAKE_BINARY_DIR}/assets/extractor/xmls" -P "${CMAKE_SOURCE_DIR}/OTRGui/Overwrite.cmake"
    )
add_dependencies(soh Assets)

install(TARGETS ZAPD DESTINATION ${CMAKE_BINARY_DIR}/assets/extractor)

set(PROGRAM_PERMISSIONS_EXECUTE OWNER_EXECUTE OWNER_WRITE OWNER_READ GROUP_EXECUTE GROUP_READ WORLD_EXECUTE WORLD_READ)

install(DIRECTORY ${CMAKE_BINARY_DIR}/assets
        DESTINATION .
        PATTERN ZAPD.out
        PERMISSIONS ${PROGRAM_PERMISSIONS_EXECUTE}
        )

install(CODE "
    include(BundleUtilities)
    fixup_bundle(\"\${CMAKE_INSTALL_PREFIX}/soh-macos\" \"\" \"${dirs}\")
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
