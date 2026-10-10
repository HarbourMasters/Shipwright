# 2ship's part of the root build, included from the root CMakeLists.txt when BUILD_2SHIP is on.
# The root project is SoH's, so 2ship keeps its own version and build name here.
set(TWOSHIP_VERSION 5.0.1)
set(TWOSHIP_BUILD_NAME "Battler Bravo")
set(TWOSHIP_TEAM "github.com/2ship2harkinian")

if (CMAKE_SYSTEM_NAME MATCHES "Windows|Linux")
    if(NOT DEFINED BUILD_CROWD_CONTROL)
        set(BUILD_CROWD_CONTROL OFF)
    endif()
endif()

# Builds ZAPD and OTRExporter for MM, without it they read MM's archive files wrong
set(GAME_STR "MM")
add_subdirectory(2s2h/ZAPDTR/ZAPD ${CMAKE_BINARY_DIR}/ZAPD)
add_subdirectory(2s2h/OTRExporter)
# OTRExporter looks for libultraship and mm/ next to itself, which held when it sat at 2ship's root.
# It includes "../../mm/2s2h/resource/type/2shResourceType.h", so copy that header to where
# an include dir two levels down finds it. Drop this once OTRExporter uses the new path.
set(OTREXPORTER_MM_SHIM ${CMAKE_BINARY_DIR}/otrexporter-mm)
configure_file(${CMAKE_SOURCE_DIR}/2s2h/2s2h/resource/type/2shResourceType.h
    ${OTREXPORTER_MM_SHIM}/mm/2s2h/resource/type/2shResourceType.h COPYONLY)
file(MAKE_DIRECTORY ${OTREXPORTER_MM_SHIM}/a/b)
target_include_directories(OTRExporter PRIVATE
    ${OTREXPORTER_MM_SHIM}/a/b
    ${CMAKE_SOURCE_DIR}/libultraship/include
    ${CMAKE_SOURCE_DIR}/libultraship
    ${CMAKE_SOURCE_DIR}/libultraship/src
    ${CMAKE_SOURCE_DIR}/libultraship/extern
    ${CMAKE_SOURCE_DIR}/2s2h/2s2h
)
add_subdirectory(2s2h)

set_property(TARGET 2ship PROPERTY APPIMAGE_DESKTOP_FILE_TERMINAL YES)
set_property(TARGET 2ship PROPERTY APPIMAGE_DESKTOP_FILE "${CMAKE_SOURCE_DIR}/2s2h/linux/2s2h.desktop")
set_property(TARGET 2ship PROPERTY APPIMAGE_ICON_FILE "${CMAKE_BINARY_DIR}/2s2hIcon.png")

if("${CMAKE_SYSTEM_NAME}" STREQUAL "Linux")
install(FILES "${CMAKE_BINARY_DIR}/2s2h/2ship.o2r" DESTINATION . COMPONENT ship)
install(TARGETS ZAPD DESTINATION ./assets/extractor COMPONENT extractor)
install(DIRECTORY "${CMAKE_SOURCE_DIR}/2s2h/assets/extractor/" DESTINATION ./assets/ COMPONENT extractor)
install(DIRECTORY "${CMAKE_SOURCE_DIR}/2s2h/assets/xml/" DESTINATION ./assets/xml COMPONENT extractor)
endif()

if ("${CMAKE_SYSTEM_NAME}" STREQUAL "Windows")
install(DIRECTORY "${CMAKE_SOURCE_DIR}/2s2h/assets/extractor/" DESTINATION ./assets/ COMPONENT 2s2h)
install(DIRECTORY "${CMAKE_SOURCE_DIR}/2s2h/assets/xml/" DESTINATION ./assets/xml COMPONENT 2s2h)
endif()

find_package(Python3 COMPONENTS Interpreter)

# Target to generate OTRs
add_custom_target(
    ExtractAssets2Ship
    # CMake versions prior to 3.17 do not have the rm command, use remove instead for older versions
    COMMAND ${CMAKE_COMMAND} -E $<IF:$<VERSION_LESS:${CMAKE_VERSION},3.17>,remove,rm> -f mm.zip 2ship.o2r
    COMMAND ${Python3_EXECUTABLE} ${CMAKE_SOURCE_DIR}/2s2h/OTRExporter/extract_assets.py -z "$<TARGET_FILE:ZAPD>" --non-interactive --xml-root ../2s2h/assets/xml --custom-otr-file 2ship.o2r "--custom-assets-path" ${CMAKE_SOURCE_DIR}/2s2h/assets/custom --port-ver "${TWOSHIP_VERSION}"
    COMMAND ${CMAKE_COMMAND} -DSYSTEM_NAME=${CMAKE_SYSTEM_NAME} -DTARGET_DIR="$<TARGET_FILE_DIR:ZAPD>" -DSOURCE_DIR=${CMAKE_SOURCE_DIR} -DBINARY_DIR=${CMAKE_BINARY_DIR} -P ${CMAKE_SOURCE_DIR}/2s2h/CMake/copy-existing-otrs.cmake
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/2s2h
    COMMENT "Running asset extraction..."
    DEPENDS ZAPD
    BYPRODUCTS mm.o2r ${CMAKE_SOURCE_DIR}/mm.o2r
)
add_dependencies(ExtractAssets ExtractAssets2Ship)

# Target to generate headers
add_custom_target(
    ExtractAssetHeaders2Ship
    COMMAND ${Python3_EXECUTABLE} ${CMAKE_SOURCE_DIR}/2s2h/OTRExporter/extract_assets.py -z "$<TARGET_FILE:ZAPD>" --non-interactive --xml-root ../2s2h/assets/xml --gen-headers
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/2s2h
    COMMENT "Generating asset headers..."
    DEPENDS ZAPD
)
add_dependencies(ExtractAssetHeaders ExtractAssetHeaders2Ship)

# 2ship.o2r is rebuilt only when its inputs change, and is part of ALL like soh.o2r.
# Calls ZAPD directly: extract_assets.py exits 0 even when ZAPD fails.
file(GLOB_RECURSE TWOSHIP_O2R_ASSETS CONFIGURE_DEPENDS ${CMAKE_SOURCE_DIR}/2s2h/assets/custom/*)
if(CMAKE_SYSTEM_NAME MATCHES "Windows")
    # Next to 2ship.exe, so it runs from the build folder
    set(TWOSHIP_O2R_COPY_TO_EXE COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_BINARY_DIR}/2s2h/2ship.o2r $<TARGET_FILE_DIR:2ship>)
endif()
add_custom_command(
    OUTPUT ${CMAKE_BINARY_DIR}/2s2h/2ship.o2r
    # ZAPD leaves an existing file alone
    COMMAND ${CMAKE_COMMAND} -E rm -f ${CMAKE_BINARY_DIR}/2s2h/2ship.o2r
    COMMAND $<TARGET_FILE:ZAPD> botr -se OTR --norom
            --customAssetsPath ${CMAKE_SOURCE_DIR}/2s2h/assets/custom
            --customOtrFile ${CMAKE_BINARY_DIR}/2s2h/2ship.o2r
            --portVer ${TWOSHIP_VERSION}
    COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_BINARY_DIR}/2s2h/2ship.o2r ${CMAKE_SOURCE_DIR}/2ship.o2r
    ${TWOSHIP_O2R_COPY_TO_EXE}
    COMMENT "Generating 2ship.o2r..."
    DEPENDS ZAPD ${TWOSHIP_O2R_ASSETS}
    BYPRODUCTS ${CMAKE_SOURCE_DIR}/2ship.o2r
    VERBATIM
)
add_custom_target(Generate2ShipOtr ALL DEPENDS ${CMAKE_BINARY_DIR}/2s2h/2ship.o2r)

if(CMAKE_SYSTEM_NAME MATCHES "Linux")
file(COPY ${CMAKE_SOURCE_DIR}/2s2h/linux/2s2hIcon.png DESTINATION ${CMAKE_BINARY_DIR})
endif()

if(CMAKE_SYSTEM_NAME MATCHES "Darwin")
add_custom_target(CreateOSXIcons2Ship
   COMMAND mkdir -p ${CMAKE_BINARY_DIR}/macosx/2s2h.iconset
   COMMAND sips -z 16 16     2s2h/macosx/2s2hIcon.png --out ${CMAKE_BINARY_DIR}/macosx/2s2h.iconset/icon_16x16.png
   COMMAND sips -z 32 32     2s2h/macosx/2s2hIcon.png --out ${CMAKE_BINARY_DIR}/macosx/2s2h.iconset/icon_16x16@2x.png
   COMMAND sips -z 32 32     2s2h/macosx/2s2hIcon.png --out ${CMAKE_BINARY_DIR}/macosx/2s2h.iconset/icon_32x32.png
   COMMAND sips -z 64 64     2s2h/macosx/2s2hIcon.png --out ${CMAKE_BINARY_DIR}/macosx/2s2h.iconset/icon_32x32@2x.png
   COMMAND sips -z 128 128   2s2h/macosx/2s2hIcon.png --out ${CMAKE_BINARY_DIR}/macosx/2s2h.iconset/icon_128x128.png
   COMMAND sips -z 256 256   2s2h/macosx/2s2hIcon.png --out ${CMAKE_BINARY_DIR}/macosx/2s2h.iconset/icon_128x128@2x.png
   COMMAND sips -z 256 256   2s2h/macosx/2s2hIcon.png --out ${CMAKE_BINARY_DIR}/macosx/2s2h.iconset/icon_256x256.png
   COMMAND sips -z 512 512   2s2h/macosx/2s2hIcon.png --out ${CMAKE_BINARY_DIR}/macosx/2s2h.iconset/icon_256x256@2x.png
   COMMAND sips -z 512 512   2s2h/macosx/2s2hIcon.png --out ${CMAKE_BINARY_DIR}/macosx/2s2h.iconset/icon_512x512.png
   COMMAND cp                2s2h/macosx/2s2hIcon.png ${CMAKE_BINARY_DIR}/macosx/2s2h.iconset/icon_512x512@2x.png
   COMMAND iconutil -c icns -o ${CMAKE_BINARY_DIR}/macosx/2s2h.icns ${CMAKE_BINARY_DIR}/macosx/2s2h.iconset
   WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
   COMMENT "Creating OSX icons ..."
   )
add_dependencies(2ship CreateOSXIcons2Ship)

install(TARGETS ZAPD DESTINATION ${CMAKE_BINARY_DIR}/assets/extractor)

install(DIRECTORY "${CMAKE_SOURCE_DIR}/2s2h/assets/extractor/" DESTINATION ./assets/)
install(DIRECTORY "${CMAKE_SOURCE_DIR}/2s2h/assets/xml/" DESTINATION ./assets/xml)

# Rename the installed 2ship binary to drop the macos suffix
INSTALL(CODE "FILE(RENAME \${CMAKE_INSTALL_PREFIX}/../MacOS/2s2h-macos \${CMAKE_INSTALL_PREFIX}/../MacOS/2s2h)")
install(CODE "
   include(BundleUtilities)
  fixup_bundle(\"\${CMAKE_INSTALL_PREFIX}/../MacOS/2s2h\" \"\" \"${dirs}\")
   ")

endif()

if(CMAKE_SYSTEM_NAME MATCHES "Windows|NintendoSwitch|CafeOS")
install(FILES ${CMAKE_SOURCE_DIR}/2s2h/README.md DESTINATION . COMPONENT 2s2h RENAME readme.txt )
install(CODE "file(MAKE_DIRECTORY \"\${CMAKE_INSTALL_PREFIX}/mods\")" COMPONENT 2s2h)
install(CODE "file(TOUCH \"\${CMAKE_INSTALL_PREFIX}/mods/custom_mod_files_go_here.txt\")" COMPONENT 2s2h)
endif()
