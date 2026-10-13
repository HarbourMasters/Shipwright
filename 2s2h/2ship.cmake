# 2ship's part of the root build, included from the root CMakeLists.txt when BUILD_2SHIP is on.
# Version, asset tools and extraction targets are in 2ship-tools.cmake.

if (CMAKE_SYSTEM_NAME MATCHES "Windows|Linux")
    if(NOT DEFINED BUILD_CROWD_CONTROL)
        set(BUILD_CROWD_CONTROL OFF)
    endif()
endif()

add_subdirectory(2s2h)

set_property(TARGET 2ship PROPERTY APPIMAGE_DESKTOP_FILE_TERMINAL YES)
set_property(TARGET 2ship PROPERTY APPIMAGE_DESKTOP_FILE "${CMAKE_SOURCE_DIR}/2s2h/linux/2s2h.desktop")
set_property(TARGET 2ship PROPERTY APPIMAGE_ICON_FILE "${CMAKE_BINARY_DIR}/2s2hIcon.png")

if("${CMAKE_SYSTEM_NAME}" STREQUAL "Linux")
install(FILES "${CMAKE_BINARY_DIR}/2s2h/2ship.o2r" DESTINATION . COMPONENT ship)
install(DIRECTORY "${CMAKE_SOURCE_DIR}/2s2h/assets/yml/" DESTINATION assets COMPONENT ship)
endif()

if ("${CMAKE_SYSTEM_NAME}" STREQUAL "Windows")
install(DIRECTORY "${CMAKE_SOURCE_DIR}/2s2h/assets/yml/" DESTINATION assets COMPONENT 2s2h)
endif()

if(CMAKE_SYSTEM_NAME MATCHES "Windows")
    # Next to 2ship.exe, so it runs from the build folder
    add_dependencies(2ship Generate2ShipOtr)
    add_custom_command(
        TARGET 2ship
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_BINARY_DIR}/2s2h/2ship.o2r $<TARGET_FILE_DIR:2ship>
    )
endif()
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

install(DIRECTORY "${CMAKE_SOURCE_DIR}/2s2h/assets/yml/" DESTINATION assets)

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
