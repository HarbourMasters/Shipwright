cmake_minimum_required(VERSION 3.16.0 FATAL_ERROR)

set(CMAKE_SYSTEM_VERSION 10.0 CACHE STRING "" FORCE)

project(soh LANGUAGES C CXX)
set(CMAKE_CXX_STANDARD 20 CACHE STRING "The C++ standard to use")

set (BUILD_UTILS OFF CACHE STRING "no utilities")
set (BUILD_SHARED_LIBS OFF CACHE STRING "install/link shared instead of static libs")

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
set(CMAKE_CONFIGURATION_TYPES
    "Debug"
    "Release"
    CACHE STRING "" FORCE
)

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
if (NOT TARGET libultraship)
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../libultraship ${CMAKE_BINARY_DIR}/libultraship)
endif()

if (NOT TARGET ZAPDUtils)
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../ZAPDTR/ZAPDUtils ${CMAKE_BINARY_DIR}/ZAPDUtils)
endif()

set(PROJECT_NAME soh)

################################################################################
# Sources
################################################################################
configure_file( ${CMAKE_CURRENT_SOURCE_DIR}/src/boot/build.c.in ${CMAKE_BINARY_DIR}/build.c @ONLY)

set(Header_Files "resource.h")
source_group("Header Files" FILES ${Header_Files})

# include
file(GLOB Header_Files__include "include/*.h" "include/*.inc")
list(APPEND Header_Files__include ${CMAKE_CURRENT_SOURCE_DIR}/include/libc/stdarg.h)
list(REMOVE_ITEM Header_Files__include ${CMAKE_CURRENT_SOURCE_DIR}/include/bgm.h)
list(REMOVE_ITEM Header_Files__include ${CMAKE_CURRENT_SOURCE_DIR}/include/math_n64.h)
list(REMOVE_ITEM Header_Files__include ${CMAKE_CURRENT_SOURCE_DIR}/include/stdbool_n64.h)
list(REMOVE_ITEM Header_Files__include ${CMAKE_CURRENT_SOURCE_DIR}/include/stddef_n64.h)
list(REMOVE_ITEM Header_Files__include ${CMAKE_CURRENT_SOURCE_DIR}/include/stdlib_n64.h)
list(REMOVE_ITEM Header_Files__include ${CMAKE_CURRENT_SOURCE_DIR}/include/ultra64.h)
source_group("include" FILES ${Header_Files__include})

# soh (root)
file(GLOB soh__ RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "soh/*.c" "soh/*.cpp" "soh/*.h")
source_group("soh" FILES ${soh__})

# soh/enhancements
file(GLOB soh__Enhancements RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
    "soh/Enhancements/*.c"
    "soh/Enhancements/*.cpp"
    "soh/Enhancements/*.h"
    "soh/Enhancements/*_extern.inc"
)
list(REMOVE_ITEM soh__Enhancements "soh/Enhancements/gamecommand.h")
list(FILTER soh__Enhancements EXCLUDE REGEX "soh/Enhancements/gfx.*")
source_group("soh\\Enhancements" FILES ${soh__Enhancements})

# soh/enhancements/audio
file (GLOB_RECURSE soh__Enhancements__audio RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "soh/Enhancements/audio/*")
source_group("soh\\Enhancements\\audio" FILES ${soh__Enhancements__audio})

# soh/enhancements/controls
file(GLOB soh__Enhancements__controls RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "soh/Enhancements/controls/*")
source_group("soh\\Enhancements\\controls" FILES ${soh__Enhancements__controls})

# soh/enhancements/cosmetics
file(GLOB soh__Enhancements__cosmetics RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "soh/Enhancements/cosmetics/*")
source_group("soh\\Enhancements\\cosmetics" FILES ${soh__Enhancements__cosmetics})

# soh/enhancements/crowd-control
if (BUILD_CROWD_CONTROL)
    file(GLOB soh__Enhancements__crowd_control RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "soh/Enhancements/crowd-control/*")
    list(REMOVE_ITEM soh__Enhancements__crowd_control "soh/Enhancements/crowd-control/soh.cs")
endif()
source_group("soh\\Enhancements\\crowd-control" FILES ${soh__Enhancements__crowd_control})

# soh/enhancements/custom-message
file(GLOB soh__Enhancements__custom_message RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "soh/Enhancements/custom-message/*")
source_group("soh\\Enhancements\\custom-message" FILES ${soh__Enhancements__custom_message})

# soh/enhancements/debugger
file(GLOB soh__Enhancements__debugger RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "soh/Enhancements/debugger/*")
source_group("soh\\Enhancements\\debugger" FILES ${soh__Enhancements__debugger})

# soh/enhancements/game-interactor
file(GLOB soh__Enhancements__game_interactor RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "soh/Enhancements/game-interactor/*")
source_group("soh\\Enhancements\\game-interactor" FILES ${soh__Enhancements__game_interactor})

# soh/enhancements/item-tables
file(GLOB soh__Enhancements__item_tables RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "soh/Enhancements/item-tables/*")
source_group("soh\\Enhancements\\item-tables" FILES ${soh__Enhancements__item_tables})

# soh/enhancements/randomizer
file(GLOB soh__Enhancements__randomizer RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "soh/Enhancements/randomizer/*.c" "soh/Enhancements/randomizer/*.cpp" "soh/Enhancements/randomizer/*.h")
source_group("soh\\Enhancements\\randomizer" FILES ${soh__Enhancements__randomizer})

# soh/enhancements/randomizer/3drando
file(GLOB soh__Enhancements__randomizer__3drando RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "soh/Enhancements/randomizer/3drando/*.hpp" "soh/Enhancements/randomizer/3drando/*.cpp")
source_group("soh\\Enhancements\\randomizer\\3drando" FILES ${soh__Enhancements__randomizer__3drando})

# soh/enhancements/randomizer/3drando/hint_list
file(GLOB soh__Enhancements__randomizer__3drando__hint_list RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "soh/Enhancements/randomizer/3drando/hint_list/*.hpp" "soh/Enhancements/randomizer/3drando/hint_list/*.cpp")
source_group("soh\\Enhancements\\randomizer\\3drando\\hint_list" FILES ${soh__Enhancements__randomizer__3drando__hint_list})

# soh/enhancements/randomizer/3drando/location_access
file(GLOB soh__Enhancements__randomizer__3drando__location_access RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "soh/Enhancements/randomizer/3drando/location_access/*.hpp" "soh/Enhancements/randomizer/3drando/location_access/*.cpp")
source_group("soh\\Enhancements\\randomizer\\3drando\\location_access" FILES ${soh__Enhancements__randomizer__3drando__location_access})

# soh/resource/type
file(GLOB soh__resource_types RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "soh/resource/type/*.h" "soh/resource/type/*.cpp")
source_group("soh\\resource\\type" FILES ${soh__resource_types})

# soh/resource/type/scenecommand
file(GLOB soh__resource_types__scenecommand RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "soh/resource/type/scenecommand/*.h" "soh/resource/type/scenecommand/*.cpp")
source_group("soh\\resource\\type\\scenecommand" FILES ${soh__resource_types__scenecommand})

# soh/resource/importer
file(GLOB soh__resource_factories RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "soh/resource/importer/*.h" "soh/resource/importer/*.cpp")
source_group("soh\\resource\\importer" FILES ${soh__resource_factories})

# soh/resource/importer/scenecommand
file(GLOB soh__resource_factories__scenecommand RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "soh/resource/importer/scenecommand/*.h" "soh/resource/importer/scenecommand/*.cpp")
source_group("soh\\resource\\importer\\scenecommand" FILES ${soh__resource_factories__scenecommand})

# src/boot (decomp)
file(GLOB src__boot RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "src/boot/*.c")
list(APPEND src__boot ${CMAKE_BINARY_DIR}/build.c)
source_group("src\\boot" FILES ${src__boot})

# src/buffers (decomp)
file(GLOB src__buffers RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "src/buffers/*.c")
source_group("src\\buffers" FILES ${src__buffers})

# src/code (decomp)
file(GLOB src__code RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "src/code/*.c")
source_group("src\\code" FILES ${src__code})

# src/libultra (decomp)
set(src__libultra
    "src/libultra/gu/coss.c"
    "src/libultra/gu/guLookAt.c"
    "src/libultra/gu/guLookAtHilite.c"
    "src/libultra/gu/guPerspectiveF.c"
    "src/libultra/gu/guPosition.c"
    "src/libultra/gu/guS2DInitBg.c"
    "src/libultra/gu/ortho.c"
    "src/libultra/gu/rotate.c"
    "src/libultra/gu/sins.c"
    "src/libultra/gu/sintable.c"
    "src/libultra/libc/sprintf.c"
)
source_group("src\\libultra" FILES ${src__libultra})

# src/overlays/actors (decomp)
file(GLOB_RECURSE src__overlays__actors RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "src/overlays/actors/*.c" "src/overlays/actors/*.h")
source_group("src\\overlays\\actors" FILES ${src__overlays__actors})

# src/overlays/effects (decomp)
file(GLOB_RECURSE src__overlays__effects RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "src/overlays/effects/*.c" "src/overlays/effects/*.h")
source_group("src\\overlays\\effects" FILES ${src__overlays__effects})

# src/overlays/gamestates (decomp)
file(GLOB_RECURSE src__overlays__gamestates RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "src/overlays/gamestates/*.c")
source_group("src\\overlays\\gamestates" FILES ${src__overlays__gamestates})

# src/overlays/misc (decomp)
file(GLOB_RECURSE src__overlays__misc RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "src/overlays/misc/*.c" "src/overlays/misc/*.h") # TODO: Before we didn't include the header, not sure why...
source_group("src\\overlays\\misc" FILES ${src__overlays__misc})

set(ALL_FILES
    ${Header_Files}
    ${Header_Files__include}
    ${soh__}
    ${soh__Enhancements}
    ${soh__Enhancements__controls}
    ${soh__Enhancements__cosmetics}
    ${soh__Enhancements__crowd_control}
    ${soh__Enhancements__custom_message}
    ${soh__Enhancements__audio}
    ${soh__Enhancements__debugger}
    ${soh__Enhancements__game_interactor}
    ${soh__Enhancements__item_tables}
    ${soh__Enhancements__randomizer}
    ${soh__Enhancements__randomizer__3drando}
    ${soh__Enhancements__randomizer__3drando__hint_list}
    ${soh__Enhancements__randomizer__3drando__location_access}
    ${soh__resource_types}
    ${soh__resource_types__scenecommand}
    ${soh__resource_factories}
    ${soh__resource_factories__scenecommand}
    ${src__boot}
    ${src__buffers}
    ${src__code}
    ${src__libultra}
    ${src__overlays__actors}
    ${src__overlays__effects}
    ${src__overlays__gamestates}
    ${src__overlays__misc}
)

################################################################################
# Target
################################################################################
add_executable(${PROJECT_NAME} ${ALL_FILES} ${CMAKE_CURRENT_SOURCE_DIR}/Resource.rc)

if (CMAKE_SYSTEM_NAME STREQUAL "Windows")
use_props(${PROJECT_NAME} "${CMAKE_CONFIGURATION_TYPES}" "${DEFAULT_CXX_PROPS}")
endif()

set(ROOT_NAMESPACE soh)

if (CMAKE_SYSTEM_NAME STREQUAL "Windows")
	set_target_properties(${PROJECT_NAME} PROPERTIES
		VS_GLOBAL_KEYWORD "Win32Proj"
	)
	if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
		set_target_properties(${PROJECT_NAME} PROPERTIES
			INTERPROCEDURAL_OPTIMIZATION_RELEASE "TRUE"
		)
	elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Win32")
		set_target_properties(${PROJECT_NAME} PROPERTIES
			INTERPROCEDURAL_OPTIMIZATION_RELEASE "TRUE"
		)
	endif()
elseif(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
    set_target_properties(${PROJECT_NAME} PROPERTIES
    OUTPUT_NAME "soh-macos"
    )
elseif(CMAKE_SYSTEM_NAME STREQUAL "Linux")
	set_target_properties(${PROJECT_NAME} PROPERTIES
	OUTPUT_NAME "soh.elf"
	)
endif()
################################################################################
# MSVC runtime library
################################################################################
if (CMAKE_SYSTEM_NAME STREQUAL "Windows")
	get_property(MSVC_RUNTIME_LIBRARY_DEFAULT TARGET ${PROJECT_NAME} PROPERTY MSVC_RUNTIME_LIBRARY)
	if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
		string(CONCAT "MSVC_RUNTIME_LIBRARY_STR"
			$<$<CONFIG:Debug>:
				MultiThreadedDebug
			>
			$<$<CONFIG:Release>:
				MultiThreaded
			>
			$<$<NOT:$<OR:$<CONFIG:Debug>,$<CONFIG:Release>>>:${MSVC_RUNTIME_LIBRARY_DEFAULT}>
		)
	elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Win32")
		string(CONCAT "MSVC_RUNTIME_LIBRARY_STR"
			$<$<CONFIG:Debug>:
				MultiThreadedDebug
			>
			$<$<CONFIG:Release>:
				MultiThreaded
			>
			$<$<NOT:$<OR:$<CONFIG:Debug>,$<CONFIG:Release>>>:${MSVC_RUNTIME_LIBRARY_DEFAULT}>
		)
	endif()
	set_target_properties(${PROJECT_NAME} PROPERTIES MSVC_RUNTIME_LIBRARY ${MSVC_RUNTIME_LIBRARY_STR})
endif()
################################################################################
# Find/download Boost
################################################################################
include(FetchContent)
FetchContent_Declare(
    Boost
    URL      https://boostorg.jfrog.io/artifactory/main/release/1.81.0/source/boost_1_81_0.tar.gz
    URL_HASH SHA256=205666dea9f6a7cfed87c7a6dfbeb52a2c1b9de55712c9c1a87735d7181452b6
    SOURCE_SUBDIR "null" # Set to a nonexistent directory so boost is not built (we don't need to build it)
    DOWNLOAD_EXTRACT_TIMESTAMP false # supress timestamp warning, not needed since the url wont change
)

set(Boost_NO_BOOST_CMAKE false)
set(BOOST_INCLUDEDIR ${FETCHCONTENT_BASE_DIR}/boost-src) # Location where FetchContent stores the source
message("Searching for Boost installation")
find_package(Boost)

if (NOT ${Boost_FOUND})
    message("Boost not found. Downloading now...")
    FetchContent_MakeAvailable(Boost)
    message("Boost downloaded to " ${FETCHCONTENT_BASE_DIR}/boost-src)
    set(BOOST-INCLUDE ${FETCHCONTENT_BASE_DIR}/boost-src)
else()
    message("Boost found in " ${Boost_INCLUDE_DIRS})
    set(BOOST-INCLUDE ${Boost_INCLUDE_DIRS})
endif()
################################################################################
# Compile definitions
################################################################################
find_package(SDL2)
set(SDL2-INCLUDE ${SDL2_INCLUDE_DIRS})

if (BUILD_CROWD_CONTROL)
    find_package(SDL2_net)
    set(SDL2-NET-INCLUDE ${SDL_NET_INCLUDE_DIRS})
endif()

target_include_directories(${PROJECT_NAME} PRIVATE assets
	${CMAKE_CURRENT_SOURCE_DIR}/include/
	${CMAKE_CURRENT_SOURCE_DIR}/src/
    ${CMAKE_CURRENT_SOURCE_DIR}/../libultraship
    ${CMAKE_CURRENT_SOURCE_DIR}/../libultraship/include
    ${CMAKE_CURRENT_SOURCE_DIR}/../libultraship/src/binarytools
    ${CMAKE_CURRENT_SOURCE_DIR}/../libultraship/src/log
    ${CMAKE_CURRENT_SOURCE_DIR}/../libultraship/src/debug
    ${CMAKE_CURRENT_SOURCE_DIR}/../libultraship/src/menu
    ${CMAKE_CURRENT_SOURCE_DIR}/../libultraship/src/misc
    ${CMAKE_CURRENT_SOURCE_DIR}/../libultraship/src/core
    ${CMAKE_CURRENT_SOURCE_DIR}/../libultraship/src/resource
    ${CMAKE_CURRENT_SOURCE_DIR}/../libultraship/src/resource/type
    ${CMAKE_CURRENT_SOURCE_DIR}/../libultraship/src/audio
    ${CMAKE_CURRENT_SOURCE_DIR}/../libultraship/extern
    ${CMAKE_CURRENT_SOURCE_DIR}/../libultraship/extern/Mercury
    ${CMAKE_CURRENT_SOURCE_DIR}/../libultraship/extern/tinyxml2
    ${CMAKE_CURRENT_SOURCE_DIR}/../libultraship/libultraship/Lib/
	${CMAKE_CURRENT_SOURCE_DIR}/../libultraship/libultraship/Lib/libjpeg/include/
	${CMAKE_CURRENT_SOURCE_DIR}/../libultraship/libultraship/Lib/spdlog/include/
    ${CMAKE_CURRENT_SOURCE_DIR}/../libultraship/src/graphic/Fast3D/U64/PR
    ${CMAKE_CURRENT_SOURCE_DIR}/../libultraship/src/graphic
	${CMAKE_CURRENT_SOURCE_DIR}/../ZAPDTR/ZAPDUtils
    ${CMAKE_CURRENT_SOURCE_DIR}/../ZAPDTR/ZAPD/resource/type
	${SDL2-INCLUDE}
    ${SDL2-NET-INCLUDE}
    ${BOOST-INCLUDE}
	${CMAKE_CURRENT_SOURCE_DIR}/assets/
	.
)

if (CMAKE_SYSTEM_NAME STREQUAL "Windows")
	if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
		target_compile_definitions(${PROJECT_NAME} PRIVATE
			"$<$<CONFIG:Debug>:"
				"_DEBUG;"
				"_CRT_SECURE_NO_WARNINGS;"
				"ENABLE_DX11;"
			">"
			"$<$<CONFIG:Release>:"
				"NDEBUG"
			">"
            "$<$<BOOL:${BUILD_CROWD_CONTROL}>:ENABLE_CROWD_CONTROL>"
			"INCLUDE_GAME_PRINTF;"
            "ENABLE_CROWD_CONTROL;"
			"UNICODE;"
			"_UNICODE"
			STORMLIB_NO_AUTO_LINK
			"_CRT_SECURE_NO_WARNINGS;"
		)
	elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Win32")
		target_compile_definitions(${PROJECT_NAME} PRIVATE
			"$<$<CONFIG:Debug>:"
				"NOINCLUDE_GAME_PRINTF;"
				"_DEBUG;"
				"_CRT_SECURE_NO_WARNINGS;"
				"ENABLE_OPENGL"
			">"
			"$<$<CONFIG:Release>:"
				"NDEBUG;"
			">"
			"INCLUDE_GAME_PRINTF;"
			"WIN32;"
			"UNICODE;"
			"_UNICODE"
			STORMLIB_NO_AUTO_LINK
		)
	endif()
elseif (CMAKE_SYSTEM_NAME STREQUAL "CafeOS")
	target_compile_definitions(${PROJECT_NAME} PRIVATE
		"$<$<CONFIG:Debug>:"
			"_DEBUG"
		">"
		"$<$<CONFIG:Release>:"
			"NDEBUG"
		">"
        "SPDLOG_ACTIVE_LEVEL=3;"
		"SPDLOG_NO_THREAD_ID;"
        "SPDLOG_NO_TLS;"
		"STBI_NO_THREAD_LOCALS;"
	)
elseif ("${CMAKE_CXX_COMPILER_ID}" MATCHES "GNU|Clang|AppleClang")
	target_compile_definitions(${PROJECT_NAME} PRIVATE
		"$<$<CONFIG:Debug>:"
			"_DEBUG"
		">"
		"$<$<CONFIG:Release>:"
			"NDEBUG"
		">"
        "$<$<BOOL:${BUILD_CROWD_CONTROL}>:ENABLE_CROWD_CONTROL>"
		"SPDLOG_ACTIVE_LEVEL=0;"
		"_CONSOLE;"
		"_CRT_SECURE_NO_WARNINGS;"
		"ENABLE_OPENGL;"
		"UNICODE;"
		"_UNICODE"
	)
endif()
################################################################################
# Compile and link options
################################################################################
if(MSVC)
    if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
        target_compile_options(${PROJECT_NAME} PRIVATE
            $<$<CONFIG:Debug>:
                /w;
                /Od
            >
            $<$<CONFIG:Release>:
                /Oi;
                /Gy;
                /W3
            >
            /sdl-;
            /permissive-;
            /MP;
            ${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT};
            ${DEFAULT_CXX_EXCEPTION_HANDLING}
        )
        target_compile_options(${PROJECT_NAME} PRIVATE  $<$<CONFIG:Debug>:/ZI;>)
    elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Win32")
        target_compile_options(${PROJECT_NAME} PRIVATE
            $<$<CONFIG:Debug>:
                /RTCs
            >
            $<$<CONFIG:Release>:
                /O2;
                /Oi;
                /Gy
            >
            /permissive-;
            /MP;
            /sdl-;
            /w;
            ${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT};
            ${DEFAULT_CXX_EXCEPTION_HANDLING}
        )
    endif()
    if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
        target_link_options(${PROJECT_NAME} PRIVATE
            $<$<CONFIG:Debug>:
                /INCREMENTAL
            >
            $<$<CONFIG:Release>:
                /OPT:REF;
                /OPT:ICF;
                /INCREMENTAL:NO;
                /FORCE:MULTIPLE
            >
            /DEBUG;
            /SUBSYSTEM:WINDOWS           
        )
    elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Win32")
        target_link_options(${PROJECT_NAME} PRIVATE
            $<$<CONFIG:Debug>:
                /STACK:8777216
            >
            $<$<CONFIG:Release>:
                /OPT:REF;
                /OPT:ICF;
                /INCREMENTAL:NO;
                /FORCE:MULTIPLE
            >
            /DEBUG;
            /SUBSYSTEM:WINDOWS
        )
    endif()
endif()

if (CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang|AppleClang")
    if (CMAKE_SYSTEM_NAME STREQUAL "Darwin")
        target_compile_options(${PROJECT_NAME} PRIVATE
            -Wall -Wextra -Wno-error
            -Wno-return-type
            -Wno-unused-parameter
            -Wno-unused-function
            -Wno-unused-variable
            -Wno-missing-field-initializers
            -Wno-parentheses
            -Wno-narrowing
            -Wno-c++11-narrowing
            -Wno-implicit-function-declaration
            -Wno-missing-braces
            -Wno-incompatible-pointer-types
            $<$<COMPILE_LANGUAGE:CXX>:-fpermissive>
            $<$<COMPILE_LANGUAGE:CXX>:-Wno-deprecated-enum-enum-conversion>
            -pthread
        )

        target_link_options(${PROJECT_NAME} PRIVATE
            -pthread
        )
    elseif (CMAKE_SYSTEM_NAME STREQUAL "NintendoSwitch")
        target_compile_options(${PROJECT_NAME} PRIVATE
            -Wall -Wextra -Wno-error
            -Wno-return-type
            -Wno-unused-parameter
            -Wno-unused-function
            -Wno-unused-variable
            -Wno-missing-field-initializers
            -Wno-parentheses
            -Wno-narrowing
            -Wno-c++11-narrowing
            -Wno-implicit-function-declaration
            -Wno-missing-braces
            -Wno-incompatible-pointer-types
            $<$<COMPILE_LANGUAGE:CXX>:-fpermissive>
            $<$<COMPILE_LANGUAGE:CXX>:-Wno-deprecated-enum-enum-conversion>
            -pthread
        )

        target_link_options(${PROJECT_NAME} PRIVATE
            -pthread
        )
    elseif (CMAKE_SYSTEM_NAME STREQUAL "CafeOS")
        target_compile_options(${PROJECT_NAME} PRIVATE
            -O2

            # disable some warnings to not clutter output
            -Wno-multichar
            -Wno-return-type
            -Wno-narrowing
            -Wno-switch-outside-range
            $<$<COMPILE_LANGUAGE:C>:
                -Wno-incompatible-pointer-types
                -Wno-discarded-array-qualifiers
                -Wno-discarded-qualifiers
                -Wno-int-conversion
                -Wno-implicit-function-declaration
                -Wno-builtin-declaration-mismatch
                -Wno-switch-unreachable
                -Wno-stringop-overflow
            >
        )
    else()
        if(CMAKE_SYSTEM_PROCESSOR MATCHES "x86_64")
		set(CPU_OPTION -msse2 -mfpmath=sse)
        endif()

        target_compile_options(${PROJECT_NAME} PRIVATE
            -Wall -Wextra -Wno-error
            -Wno-unused-parameter
            -Wno-unused-function
            -Wno-unused-variable
            -Wno-missing-field-initializers
            -Wno-parentheses
            -Wno-narrowing
            -Wno-missing-braces
            -Wno-incompatible-pointer-types
            $<$<COMPILE_LANGUAGE:CXX>:-fpermissive>
            $<$<COMPILE_LANGUAGE:CXX>:-Wno-deprecated-enum-enum-conversion>
            -pthread
	    ${CPU_OPTION}
        )

        target_link_options(${PROJECT_NAME} PRIVATE
            -pthread
            -Wl,-export-dynamic
        )
    endif()
endif()
################################################################################
# Pre build events
################################################################################
if (CMAKE_SYSTEM_NAME STREQUAL "Windows")
	add_custom_command_if(
		TARGET ${PROJECT_NAME}
		PRE_BUILD
		COMMANDS
		COMMAND   $<CONFIG:Debug> copy /b $<SHELL_PATH:${CMAKE_BINARY_DIR}/>build.c +,,
	)
endif()
################################################################################
# Dependencies
################################################################################
add_dependencies(${PROJECT_NAME}
    ZAPDUtils
    libultraship
)

if (CMAKE_SYSTEM_NAME STREQUAL "Windows")
	if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
		set(ADDITIONAL_LIBRARY_DEPENDENCIES
			"libultraship;"
			"ZAPDUtils;"
			"glu32;"
			"SDL2::SDL2;"
			"SDL2::SDL2main;"
            "$<$<BOOL:${BUILD_CROWD_CONTROL}>:SDL2_net::SDL2_net-static>"
			"glfw;"
			"winmm;"
			"imm32;"
			"version;"
			"setupapi"
		)
	elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Win32") 
		set(ADDITIONAL_LIBRARY_DEPENDENCIES
			"libultraship;"
			"ZAPDUtils;"
			"glu32;"
			"SDL2::SDL2;"
			"SDL2::SDL2main;"
			"glfw;"
			"winmm;"
			"imm32;"
			"version;"
			"setupapi"
		)
	endif()
elseif(CMAKE_SYSTEM_NAME STREQUAL "NintendoSwitch")
    find_package(SDL2)
    set(THREADS_PREFER_PTHREAD_FLAG ON)
    find_package(Threads REQUIRED)
    set(ADDITIONAL_LIBRARY_DEPENDENCIES
        "libultraship;"
        "ZAPDUtils;"
        SDL2::SDL2
        -lglad
        Threads::Threads
    )
elseif(CMAKE_SYSTEM_NAME STREQUAL "CafeOS")
    find_package(SDL2 REQUIRED)
    set(ADDITIONAL_LIBRARY_DEPENDENCIES
        "libultraship;"
        "ZAPDUtils;"
        SDL2::SDL2-static

        "$<$<CONFIG:Debug>:-Wl,--wrap=abort>"
    )
    target_include_directories(${PROJECT_NAME} PRIVATE
        ${DEVKITPRO}/portlibs/wiiu/include/
    )
else()
    find_package(SDL2)
    set(THREADS_PREFER_PTHREAD_FLAG ON)
    find_package(Threads REQUIRED)
 	set(ADDITIONAL_LIBRARY_DEPENDENCIES
        "libultraship;"
        "ZAPDUtils;"
		SDL2::SDL2
        "$<$<BOOL:${BUILD_CROWD_CONTROL}>:SDL2_net::SDL2_net>"
		${CMAKE_DL_LIBS}
		Threads::Threads
	)
endif()

if(NOT CMAKE_SYSTEM_NAME MATCHES "NintendoSwitch|CafeOS")
INSTALL(TARGETS soh DESTINATION . COMPONENT ship)
endif()

if (CMAKE_SYSTEM_NAME STREQUAL "Windows")
INSTALL(FILES $<TARGET_PDB_FILE:soh> DESTINATION ./debug COMPONENT ship)
endif()

find_program(CURL NAMES curl DOC "Path to the curl program.  Used to download files.")
execute_process(COMMAND ${CURL} -sSfL https://raw.githubusercontent.com/gabomdq/SDL_GameControllerDB/master/gamecontrollerdb.txt -o ${CMAKE_BINARY_DIR}/gamecontrollerdb.txt OUTPUT_VARIABLE RESULT)

if("${CMAKE_SYSTEM_NAME}" STREQUAL "Darwin")
configure_file( ${CMAKE_CURRENT_SOURCE_DIR}/macosx/Info.plist.in ${CMAKE_BINARY_DIR}/macosx/Info.plist @ONLY)
INSTALL(FILES ${CMAKE_BINARY_DIR}/gamecontrollerdb.txt DESTINATION ../MacOS COMPONENT ship)
elseif(NOT "${CMAKE_SYSTEM_NAME}" MATCHES "NintendoSwitch|CafeOS")
INSTALL(FILES ${CMAKE_BINARY_DIR}/gamecontrollerdb.txt DESTINATION . COMPONENT ship)
endif()

if(CMAKE_SYSTEM_NAME MATCHES "NintendoSwitch|CafeOS")
    if (NOT TARGET pathconf)
        add_library(pathconf OBJECT platform/pathconf.c)
    endif()
    target_link_libraries(${PROJECT_NAME} PRIVATE "${ADDITIONAL_LIBRARY_DEPENDENCIES}" $<TARGET_OBJECTS:pathconf> )
else()
    target_link_libraries(${PROJECT_NAME} PRIVATE "${ADDITIONAL_LIBRARY_DEPENDENCIES}")
endif()

if(CMAKE_SYSTEM_NAME MATCHES "NintendoSwitch")

nx_generate_nacp(Ship.nacp
   NAME "Ship of Harkinian"
   AUTHOR "${PROJECT_TEAM}"
   VERSION "${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH}"
)
 
nx_create_nro(soh
    NACP Ship.nacp
    ICON ${CMAKE_CURRENT_SOURCE_DIR}/icon.jpg
)

INSTALL(FILES ${CMAKE_CURRENT_BINARY_DIR}/soh.nro DESTINATION . COMPONENT ship)

elseif(CMAKE_SYSTEM_NAME MATCHES "CafeOS")

wut_create_rpx(${PROJECT_NAME})

wut_create_wuhb(${PROJECT_NAME}
	NAME       "Ship of Harkinian"
	SHORTNAME  "SoH"
	AUTHOR     "${PROJECT_TEAM}"
	ICON       ${CMAKE_CURRENT_SOURCE_DIR}/icon.jpg
)

INSTALL(FILES ${CMAKE_CURRENT_BINARY_DIR}/soh.rpx ${CMAKE_CURRENT_BINARY_DIR}/soh.wuhb DESTINATION . COMPONENT ship)

endif()
