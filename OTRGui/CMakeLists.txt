cmake_minimum_required(VERSION 3.16)
project(OTRGui)

set(PLATFORM "Desktop")
set(CMAKE_CXX_STANDARD 20)
#set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/build)
set(APP_ICON_RESOURCE_WINDOWS ${CMAKE_CURRENT_SOURCE_DIR}/appicon.rc)

include(CMake/Utils.cmake)

add_subdirectory(libs/raylib)

include_directories(src)
include_directories(src/game)
include_directories(include)

#include_external_msproject(ZAPD ../../ZAPDTR/ZAPD/ZAPD.vcproj)
#include_external_msproject(ZAPDUtils ../../ZAPDTR/ZAPDUtils/ZAPDUtils.vcproj)
#include_external_msproject(libultraship ../../libultraship/libultraship/libultraship.vcproj)
#include_external_msproject(OTRExporter ../../OTRExporter/OTRExporter/OTRExporter.vcproj)
add_subdirectory(../libultraship/libultraship ${CMAKE_BINARY_DIR}/libultraship)
add_subdirectory(../ZAPDTR/ZAPD ${CMAKE_BINARY_DIR}/ZAPD)
add_subdirectory(../ZAPDTR/ZAPDUtils ${CMAKE_BINARY_DIR}/ZAPDUtils)
add_subdirectory(../OTRExporter/OTRExporter ${CMAKE_BINARY_DIR}/OTRExporter)

if (NOT CMAKE_SYSTEM_NAME STREQUAL "Windows")
add_subdirectory(../external/StormLib ${CMAKE_BINARY_DIR}/StormLib)
endif()

file(GLOB_RECURSE HEADERS src/*.h)
file(GLOB_RECURSE SOURCES src/*.cpp)
file(GLOB_RECURSE C_SOURCES src/*.c)

add_executable(${PROJECT_NAME} ${SOURCES} ${C_SOURCES} ${HEADERS} ${APP_ICON_RESOURCE_WINDOWS})
add_custom_target(Assets1 ALL COMMAND ${CMAKE_COMMAND} -Dsrc_dir="${CMAKE_SOURCE_DIR}/assets" -Ddst_dir="${CMAKE_CURRENT_BINARY_DIR}/assets" -P "${CMAKE_CURRENT_SOURCE_DIR}/Overwrite.cmake")
add_custom_target(Assets2 ALL COMMAND ${CMAKE_COMMAND} -Dsrc_dir="${CMAKE_SOURCE_DIR}/../OTRExporter/assets" -Ddst_dir="${CMAKE_CURRENT_BINARY_DIR}/assets/game" -P "${CMAKE_CURRENT_SOURCE_DIR}/Overwrite.cmake")
add_custom_target(Assets3 ALL COMMAND ${CMAKE_COMMAND} -Dsrc_dir="${CMAKE_SOURCE_DIR}/../soh/assets/xml" -Ddst_dir="${CMAKE_CURRENT_BINARY_DIR}/assets/extractor/xmls" -P "${CMAKE_CURRENT_SOURCE_DIR}/Overwrite.cmake")

add_dependencies(${PROJECT_NAME} Assets1 Assets2 Assets3)

target_link_libraries(${PROJECT_NAME} PUBLIC raylib)
