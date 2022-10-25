cmake_minimum_required(VERSION 3.16)
project(OTRGui)

set(PLATFORM "Desktop")
set(CMAKE_CXX_STANDARD 20)
#set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/build)
set(APP_ICON_RESOURCE_WINDOWS ${CMAKE_CURRENT_SOURCE_DIR}/appicon.rc)

if (CMAKE_SYSTEM_NAME STREQUAL "Linux")
    set(OpenGL_GL_PREFERENCE "GLVND")
endif()

include(CMake/Utils.cmake)

add_subdirectory(libs/raylib EXCLUDE_FROM_ALL)

include_directories(src)
include_directories(src/game)
include_directories(include)

if (NOT TARGET libultraship)
    add_subdirectory(../libultraship/libultraship ${CMAKE_BINARY_DIR}/libultraship)
endif()
if (NOT TARGET ZAPD)
    add_subdirectory(../ZAPDTR/ZAPD ${CMAKE_BINARY_DIR}/ZAPD)
endif()
if (NOT TARGET ZAPDUtils)
    add_subdirectory(../ZAPDTR/ZAPDUtils ${CMAKE_BINARY_DIR}/ZAPDUtils)
endif()
if (NOT TARGET OTRExporter)
    add_subdirectory(../OTRExporter/OTRExporter ${CMAKE_BINARY_DIR}/OTRExporter)
endif()
if (NOT TARGET storm)
    add_subdirectory(../StormLib ${CMAKE_BINARY_DIR}/StormLib)
endif()

file(GLOB_RECURSE HEADERS src/*.h)
file(GLOB_RECURSE SOURCES src/*.cpp)
file(GLOB_RECURSE C_SOURCES src/*.c)

add_executable(${PROJECT_NAME} EXCLUDE_FROM_ALL ${SOURCES} ${C_SOURCES} ${HEADERS} ${APP_ICON_RESOURCE_WINDOWS})

if (CMAKE_SYSTEM_NAME STREQUAL "Windows")
use_props(${PROJECT_NAME} "${CMAKE_CONFIGURATION_TYPES}" "${DEFAULT_CXX_PROPS}")
endif()

add_custom_target(Assets ALL
    COMMAND ${CMAKE_COMMAND} -Dsrc_dir="${CMAKE_CURRENT_SOURCE_DIR}/assets" -Ddst_dir="${CMAKE_CURRENT_BINARY_DIR}/assets" -P "${CMAKE_CURRENT_SOURCE_DIR}/Overwrite.cmake"
    COMMAND ${CMAKE_COMMAND} -Dsrc_dir="${CMAKE_CURRENT_SOURCE_DIR}/../OTRExporter/assets" -Ddst_dir="${CMAKE_CURRENT_BINARY_DIR}/assets/game" -P "${CMAKE_CURRENT_SOURCE_DIR}/Overwrite.cmake"
    COMMAND ${CMAKE_COMMAND} -Dsrc_dir="${CMAKE_CURRENT_SOURCE_DIR}/../soh/assets/xml" -Ddst_dir="${CMAKE_CURRENT_BINARY_DIR}/assets/extractor/xmls" -P "${CMAKE_CURRENT_SOURCE_DIR}/Overwrite.cmake"
    )
 
add_dependencies(OTRGui Assets)

target_link_libraries(${PROJECT_NAME} PUBLIC raylib)

INSTALL(TARGETS OTRGui DESTINATION . COMPONENT ship)
install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/assets
        DESTINATION .
	COMPONENT ship
)
INSTALL(TARGETS ZAPD DESTINATION assets/extractor COMPONENT ship)

