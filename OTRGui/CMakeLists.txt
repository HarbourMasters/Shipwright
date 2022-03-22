cmake_minimum_required(VERSION 3.16)
project(OTRGui)

set(PLATFORM "Desktop")
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/build)
set(APP_ICON_RESOURCE_WINDOWS ${CMAKE_CURRENT_SOURCE_DIR}/appicon.rc)

add_subdirectory(libs/raylib)

include_directories(src)
include_directories(src/game)
include_directories(include)

include_external_msproject(ZAPD ../../ZAPDTR/ZAPD/ZAPD.vcproj)
include_external_msproject(ZAPDUtils ../../ZAPDTR/ZAPDUtils/ZAPDUtils.vcproj)
include_external_msproject(libultraship ../../libultraship/libultraship/libultraship.vcproj)
include_external_msproject(OTRExporter ../../OTRExporter/OTRExporter/OTRExporter.vcproj)

file(GLOB_RECURSE HEADERS src/*.h)
file(GLOB_RECURSE SOURCES src/*.cpp)
file(GLOB_RECURSE C_SOURCES src/*.c)

add_executable(${PROJECT_NAME} ${SOURCES} ${C_SOURCES} ${HEADERS} ${APP_ICON_RESOURCE_WINDOWS})
add_custom_command(TARGET ${PROJECT_NAME} PRE_BUILD COMMAND ${CMAKE_COMMAND} -Dsrc_dir="${CMAKE_SOURCE_DIR}/assets" -Ddst_dir="${CMAKE_CURRENT_BINARY_DIR}/assets" -P "${CMAKE_CURRENT_SOURCE_DIR}/Overwrite.cmake")
add_custom_command(TARGET ${PROJECT_NAME} PRE_BUILD COMMAND ${CMAKE_COMMAND} -Dsrc_dir="${CMAKE_SOURCE_DIR}/../OTRExporter/assets" -Ddst_dir="${CMAKE_CURRENT_BINARY_DIR}/assets/game" -P "${CMAKE_CURRENT_SOURCE_DIR}/Overwrite.cmake")
add_custom_command(TARGET ${PROJECT_NAME} PRE_BUILD COMMAND ${CMAKE_COMMAND} -Dsrc_dir="${CMAKE_SOURCE_DIR}/../soh/assets/xml" -Ddst_dir="${CMAKE_CURRENT_BINARY_DIR}/assets/extractor/xmls" -P "${CMAKE_CURRENT_SOURCE_DIR}/Overwrite.cmake")

target_link_libraries(${PROJECT_NAME} PUBLIC raylib)
