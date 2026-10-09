cmake_minimum_required(VERSION 3.26.0 FATAL_ERROR)

set(CMAKE_SYSTEM_VERSION 10.0 CACHE STRING "" FORCE)

project(2ship LANGUAGES C CXX)
set(CMAKE_CXX_STANDARD 20 CACHE STRING "The C++ standard to use")

if (CMAKE_SYSTEM_NAME STREQUAL "Darwin")
    enable_language(OBJCXX)
    set(CMAKE_OBJC_FLAGS "${CMAKE_OBJC_FLAGS} -fobjc-arc")
    set(CMAKE_OBJCXX_FLAGS "${CMAKE_OBJCXX_FLAGS} -fobjc-arc")
endif()

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


FetchContent_Declare(
    dr_libs
    GIT_REPOSITORY https://github.com/mackron/dr_libs.git
	GIT_TAG da35f9d6c7374a95353fd1df1d394d44ab66cf01
)
FetchContent_MakeAvailable(dr_libs)

################################################################################
# Global configuration types
################################################################################
set(CMAKE_CONFIGURATION_TYPES
    "Debug"
    "Release"
    CACHE STRING "" FORCE
)

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

if (NOT TARGET ZAPDLib)
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../ZAPDTR/ZAPD ${CMAKE_BINARY_DIR}/ZAPD)
endif()

set(PROJECT_NAME 2ship)

################################################################################
# Sources
################################################################################
configure_file( ${CMAKE_CURRENT_SOURCE_DIR}/src/boot/build.c.in ${CMAKE_CURRENT_SOURCE_DIR}/src/boot/build.c @ONLY)
configure_file( ${CMAKE_CURRENT_SOURCE_DIR}/windows/properties.h.in ${CMAKE_CURRENT_SOURCE_DIR}/windows/properties.h @ONLY)

#set(Header_Files "resource.h")
source_group("headers" FILES ${Header_Files})

# include {{{
file(GLOB Header_Files__include "include/*.h" "include/*.inc")
list(APPEND Header_Files__include ${CMAKE_CURRENT_SOURCE_DIR}/include/libc/stdarg.h)
list(REMOVE_ITEM Header_Files__include ${CMAKE_CURRENT_SOURCE_DIR}/include/bgm.h)
list(REMOVE_ITEM Header_Files__include ${CMAKE_CURRENT_SOURCE_DIR}/include/math_n64.h)
list(REMOVE_ITEM Header_Files__include ${CMAKE_CURRENT_SOURCE_DIR}/include/stdbool_n64.h)
list(REMOVE_ITEM Header_Files__include ${CMAKE_CURRENT_SOURCE_DIR}/include/stddef_n64.h)
list(REMOVE_ITEM Header_Files__include ${CMAKE_CURRENT_SOURCE_DIR}/include/stdlib_n64.h)
list(REMOVE_ITEM Header_Files__include ${CMAKE_CURRENT_SOURCE_DIR}/include/ultra64.h)
source_group("include" FILES ${Header_Files__include})
# }}}

file(GLOB_RECURSE ship__ RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "2s2h/*.c" "2s2h/*.cpp" "2s2h/*.h" "2s2h/*.hpp")
# Create source groups that match the real file directory paths
foreach(source IN LISTS ship__)
    get_filename_component(source_path "${source}" PATH)
    string(REPLACE "/" "\\" source_path_adjusted "${source_path}")
    source_group("${source_path_adjusted}" FILES "${source}")
endforeach()

file(GLOB_RECURSE libultra_headers RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
    "include/PR/*.h"
)

if (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    set_source_files_properties(2s2h/BenPort.cpp PROPERTIES COMPILE_FLAGS "/utf-8")
endif()

list(FILTER ship__ EXCLUDE REGEX "2s2h/Extractor/*")

if(NOT CMAKE_SYSTEM_NAME MATCHES "NintendoSwitch|CafeOS")
    file(GLOB_RECURSE ship__Extractor RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
        "2s2h/Extractor/*.c"
        "2s2h/Extractor/*.cpp"
        "2s2h/Extractor/*.h"
        "2s2h/Extractor/*.hpp"
    )
else()
    file(GLOB_RECURSE ship__Extractor RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
        "2s2h/Extractor/*.h"
        "2s2h/Extractor/*.hpp"
    )
endif()

# src (decomp) {{{
file(GLOB_RECURSE src__ RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "src/*.c" "src/*.h")

if (CMAKE_SYSTEM_NAME STREQUAL "Windows")
    list(APPEND src__ ${CMAKE_CURRENT_SOURCE_DIR}/windows/Resource.rc)
endif()

list(FILTER src__ EXCLUDE REGEX "src/dmadata/*")
list(FILTER src__ EXCLUDE REGEX "src/elf_message/*")
list(FILTER src__ EXCLUDE REGEX "src/libultra/io/*")
list(FILTER src__ EXCLUDE REGEX "src/libultra/libc/*")
list(FILTER src__ EXCLUDE REGEX "src/libultra/os/*")
list(FILTER src__ EXCLUDE REGEX "src/libultra/rmon/*")
list(FILTER src__ EXCLUDE REGEX "src/libultra/*")
#list(APPEND src__ "src/libultra/libc/sprintf.c")
#list(REMOVE_ITEM src__ "src/libultra/gu/cosf.c")
#list(REMOVE_ITEM src__ "src/libultra/gu/lookat.c")
#list(REMOVE_ITEM src__ "src/libultra/gu/lookathil.c")
#list(REMOVE_ITEM src__ "src/libultra/gu/perspective.c")
#list(REMOVE_ITEM src__ "src/libultra/gu/position.c")
#list(REMOVE_ITEM src__ "src/libultra/gu/sinf.c")
#list(REMOVE_ITEM src__ "src/libultra/gu/sinf.c")
#list(REMOVE_ITEM src__ "src/libultra/gu/sqrtf.c")
#list(REMOVE_ITEM src__ "src/libultra/gu/us2dex.c")

source_group("src" REGULAR_EXPRESSION "src/*")
source_group("src\\boot" REGULAR_EXPRESSION "src/boot/*")
source_group("src\\buffers" REGULAR_EXPRESSION "src/buffers/*")
source_group("src\\code" REGULAR_EXPRESSION "src/code/*")
#source_group("src\\libultra" REGULAR_EXPRESSION "src/libultra/*")
source_group("src\\overlays\\actors" REGULAR_EXPRESSION "src/overlays/actors/*")
source_group("src\\overlays\\effects" REGULAR_EXPRESSION "src/overlays/effects/*")
source_group("src\\overlays\\fbdemos" REGULAR_EXPRESSION "src/overlays/fbdemos/*")
source_group("src\\overlays\\gamestates" REGULAR_EXPRESSION  "src/overlays/gamestates/*")
source_group("src\\overlays\\misc" REGULAR_EXPRESSION  "src/overlays/misc/*")
# }}}

set(ALL_FILES
    ${Header_Files}
    ${Header_Files__include}
    ${ship__}
	${libultra_headers}
    ${ship__Extractor}
    ${src__}
)

################################################################################
# Target
################################################################################
add_executable(${PROJECT_NAME} ${ALL_FILES})

if (CMAKE_SYSTEM_NAME STREQUAL "Windows")
use_props(${PROJECT_NAME} "${CMAKE_CONFIGURATION_TYPES}" "${DEFAULT_CXX_PROPS}")
endif()

set(ROOT_NAMESPACE 2s2h)

if (CMAKE_SYSTEM_NAME STREQUAL "Windows")
	set_target_properties(${PROJECT_NAME} PROPERTIES
		VS_GLOBAL_KEYWORD "Win32Proj"
	)
	set_target_properties(${PROJECT_NAME} PROPERTIES
		INTERPROCEDURAL_OPTIMIZATION_RELEASE "TRUE"
	)
elseif(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
    set_target_properties(${PROJECT_NAME} PROPERTIES
        XCODE_ATTRIBUTE_CLANG_ENABLE_OBJC_ARC YES
        OUTPUT_NAME "2s2h-macos"
    )
elseif(CMAKE_SYSTEM_NAME STREQUAL "Linux")
	set_target_properties(${PROJECT_NAME} PROPERTIES
	OUTPUT_NAME "2s2h.elf"
	)
endif()
################################################################################
# MSVC runtime library
################################################################################
if (CMAKE_SYSTEM_NAME STREQUAL "Windows")
	get_property(MSVC_RUNTIME_LIBRARY_DEFAULT TARGET ${PROJECT_NAME} PROPERTY MSVC_RUNTIME_LIBRARY)
	string(CONCAT "MSVC_RUNTIME_LIBRARY_STR"
		$<$<CONFIG:Debug>:
			MultiThreadedDebug
		>
		$<$<CONFIG:Release>:
			MultiThreaded
		>
		$<$<NOT:$<OR:$<CONFIG:Debug>,$<CONFIG:Release>>>:${MSVC_RUNTIME_LIBRARY_DEFAULT}>
	)
	set_target_properties(${PROJECT_NAME} PROPERTIES MSVC_RUNTIME_LIBRARY ${MSVC_RUNTIME_LIBRARY_STR})
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
	${CMAKE_CURRENT_SOURCE_DIR}/include/PR
	${CMAKE_CURRENT_SOURCE_DIR}/src/
    ${CMAKE_CURRENT_SOURCE_DIR}/../libultraship/include
    ${CMAKE_CURRENT_SOURCE_DIR}/../ZAPDTR/ZAPD/resource/type
	${CMAKE_CURRENT_SOURCE_DIR}
	${CMAKE_CURRENT_SOURCE_DIR}/2s2h/
	${SDL2-INCLUDE}
    ${SDL2-NET-INCLUDE}
	${CMAKE_CURRENT_SOURCE_DIR}/assets/
	${dr_libs_SOURCE_DIR}
	.
)

if (CMAKE_SYSTEM_NAME STREQUAL "Windows")
	target_compile_definitions(${PROJECT_NAME} PRIVATE
		"$<$<CONFIG:Debug>:"
			"_DEBUG;"
			"_CRT_SECURE_NO_WARNINGS;"
			"ENABLE_DX11;"
		">"
		"$<$<CONFIG:Release>:"
			"NDEBUG;"
		">"
           #"$<$<BOOL:${BUILD_CROWD_CONTROL}>:ENABLE_CROWD_CONTROL>"
		"INCLUDE_GAME_PRINTF;"
           #"ENABLE_CROWD_CONTROL;"
		"F3DEX_GBI_2"
		"UNICODE;"
		"_UNICODE"
        SPDLOG_ACTIVE_LEVEL=${SPDLOG_MIN_CUTOFF}
        LOG_LEVEL_GAME_PRINTS=${SPDLOG_LEVEL_OFF}
		STORMLIB_NO_AUTO_LINK
		"_CRT_SECURE_NO_WARNINGS;"
		NOMINMAX
	)
elseif (CMAKE_SYSTEM_NAME STREQUAL "CafeOS")
	target_compile_definitions(${PROJECT_NAME} PRIVATE
		"$<$<CONFIG:Debug>:"
			"_DEBUG;"
		">"
		"$<$<CONFIG:Release>:"
			"NDEBUG;"
		">"
		"F3DEX_GBI_2;"
		"SPDLOG_NO_THREAD_ID;"
        "SPDLOG_NO_TLS;"
		"STBI_NO_THREAD_LOCALS;"
        SPDLOG_ACTIVE_LEVEL=${SPDLOG_MIN_CUTOFF}
        LOG_LEVEL_GAME_PRINTS=${SPDLOG_LEVEL_OFF}
	)
elseif ("${CMAKE_CXX_COMPILER_ID}" MATCHES "GNU|Clang|AppleClang")
	target_compile_definitions(${PROJECT_NAME} PRIVATE
		"$<$<CONFIG:Debug>:"
			"_DEBUG;"
		">"
		"$<$<CONFIG:Release>:"
			"NDEBUG;"
		">"
        "F3DEX_GBI_2;"
		# "$<$<BOOL:${BUILD_CROWD_CONTROL}>:ENABLE_CROWD_CONTROL>"
		"_CONSOLE;"
		"_CRT_SECURE_NO_WARNINGS;"
		"ENABLE_OPENGL;"
		"UNICODE;"
		"_UNICODE;"
        SPDLOG_ACTIVE_LEVEL=${SPDLOG_MIN_CUTOFF}
        LOG_LEVEL_GAME_PRINTS=${SPDLOG_LEVEL_OFF}
        "NON_MATCHING;"
        "NON_EQUIVALENT;"
	)
endif()
################################################################################
# Compile and link options
################################################################################
if(MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE
        $<$<CONFIG:Debug>:
            /w;
            /Od
        >
        $<$<CONFIG:Release>:
            /Oi
            /Gy;
            /W3
        >
	
		/wd4024 #different types for formal and actual parameter [x]. Usually seen along side 4047.
		/wd4047 #'uintptr_t' differs in levels of indirection from 'Gfx *'. Usually caused by passing a uintptr_t into a function expecting a pointer-types
		/wd4090 #'initializing': different 'const' qualifiers. Caused when initializing a non const struct member with a const char*.		
		/wd4101 # unreferenced local variable. Things like stack pads.
		/wd4133 #incompatible types. Usually caused by passing a c string into a function that takes a pointer.
		/wd4244 #conversion from [a] to [b]. Used to warn when copying to smaller type without a cast.
        /sdl-;
        /permissive-;
        /MP;
        ${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT};
        ${DEFAULT_CXX_EXCEPTION_HANDLING}
    )
    # /ZI is Edit and Continue, usable only by the Visual Studio debugger. It overrides the /Z7 or
    # /Zi chosen above, and forces every parallel cl.exe through one mspdbsrv.exe.
    if (CMAKE_GENERATOR MATCHES "Visual Studio")
        target_compile_options(${PROJECT_NAME} PRIVATE  $<$<CONFIG:Debug>:/ZI;>)
    endif()
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
        /MANIFEST:NO;
        /DEBUG;
        /SUBSYSTEM:WINDOWS
    )
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
            -Wno-missing-braces
            $<$<COMPILE_LANGUAGE:C>:
                #-Werror-implicit-function-declaration
                -Wno-ignored-qualifiers
                -Wno-incompatible-pointer-types
                -Wno-int-conversion
            >
            $<$<COMPILE_LANGUAGE:CXX>:-fpermissive>
            $<$<COMPILE_LANGUAGE:CXX>:
                -Wno-c++11-narrowing
                -Wno-deprecated-enum-enum-conversion
            >
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
            -Wno-missing-braces
            $<$<COMPILE_LANGUAGE:C>:
                -Werror-implicit-function-declaration
                -Wno-incompatible-pointer-types
            >
            $<$<COMPILE_LANGUAGE:CXX>:-fpermissive>
            $<$<COMPILE_LANGUAGE:CXX>:
                -Wno-c++11-narrowing
                -Wno-deprecated-enum-enum-conversion
            >
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
                -Werror-implicit-function-declaration
                -Wno-incompatible-pointer-types
                -Wno-discarded-array-qualifiers
                -Wno-discarded-qualifiers
                -Wno-int-conversion
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
            $<$<COMPILE_LANGUAGE:C>:
                -Wno-discarded-qualifiers
                -Wno-discarded-array-qualifiers
                -Wno-int-conversion
                #-Werror-implicit-function-declaration
                -Wno-incompatible-pointer-types
            >
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

# This file contains a few SIMD functions. Those functions are much more inefficient without optimizations. MSVC and GCC allow for optimizing a single function, but clang does not.
set_source_files_properties(2s2h/mixer.c PROPERTIES
COMPILE_FLAGS "-O2"
SKIP_PRECOMPILE_HEADERS ON
)

################################################################################
# Precompiled Headers
################################################################################
target_precompile_headers(${PROJECT_NAME} PRIVATE
    "$<$<COMPILE_LANGUAGE:CXX>:<libultraship/bridge/consolevariablebridge.h$<ANGLE-R>>"
    "$<$<COMPILE_LANGUAGE:CXX>:<ship/Context.h$<ANGLE-R>>"
    "$<$<COMPILE_LANGUAGE:CXX>:<ship/resource/ResourceManager.h$<ANGLE-R>>"
    "$<$<COMPILE_LANGUAGE:CXX>:<ship/window/Window.h$<ANGLE-R>>"
    "$<$<COMPILE_LANGUAGE:CXX>:<spdlog/spdlog.h$<ANGLE-R>>"
    "$<$<COMPILE_LANGUAGE:CXX>:<string$<ANGLE-R>>"
    "$<$<COMPILE_LANGUAGE:CXX>:<vector$<ANGLE-R>>"
    "$<$<COMPILE_LANGUAGE:CXX>:<map$<ANGLE-R>>"
    "$<$<COMPILE_LANGUAGE:CXX>:<unordered_map$<ANGLE-R>>"
    "$<$<COMPILE_LANGUAGE:CXX>:<filesystem$<ANGLE-R>>"
    "$<$<COMPILE_LANGUAGE:CXX>:<fstream$<ANGLE-R>>"
    "$<$<COMPILE_LANGUAGE:CXX>:<algorithm$<ANGLE-R>>"
    "$<$<COMPILE_LANGUAGE:CXX>:<functional$<ANGLE-R>>"
    "$<$<COMPILE_LANGUAGE:CXX>:<variant$<ANGLE-R>>"
    "$<$<COMPILE_LANGUAGE:CXX>:${CMAKE_CURRENT_SOURCE_DIR}/2s2h/GameInteractor/GameInteractor.h>"
    "$<$<COMPILE_LANGUAGE:CXX>:${CMAKE_CURRENT_SOURCE_DIR}/include/variables.h>"
    "$<$<COMPILE_LANGUAGE:CXX>:${CMAKE_CURRENT_SOURCE_DIR}/include/functions.h>"
    "$<$<COMPILE_LANGUAGE:CXX>:${CMAKE_CURRENT_SOURCE_DIR}/include/macros.h>"
    "$<$<COMPILE_LANGUAGE:CXX>:${CMAKE_CURRENT_SOURCE_DIR}/include/z64.h>"
    "$<$<COMPILE_LANGUAGE:C>:${CMAKE_CURRENT_SOURCE_DIR}/include/ultra64.h>"
    "$<$<COMPILE_LANGUAGE:C>:${CMAKE_CURRENT_SOURCE_DIR}/include/global.h>"
    "$<$<COMPILE_LANGUAGE:C>:${CMAKE_CURRENT_SOURCE_DIR}/include/z64.h>"
    "$<$<COMPILE_LANGUAGE:C>:${CMAKE_CURRENT_SOURCE_DIR}/include/macros.h>"
    "$<$<COMPILE_LANGUAGE:C>:${CMAKE_CURRENT_SOURCE_DIR}/include/functions.h>"
    "$<$<COMPILE_LANGUAGE:C>:${CMAKE_CURRENT_SOURCE_DIR}/include/variables.h>"
)

# MSVC only: the header is expensive to parse there, few sources include it, and MSVC's PCH load
# barely grows with content. On Clang the parse is cheap by comparison and the bigger PCH is paid
# for by every source in the target, making it a net loss.
if (MSVC)
    target_precompile_headers(${PROJECT_NAME} PRIVATE
        "$<$<COMPILE_LANGUAGE:CXX>:${CMAKE_CURRENT_SOURCE_DIR}/2s2h/BenGui/UIWidgets.hpp>"
    )
endif()

# Instantiate templates once while building the PCH instead of replaying them in every TU that
# consumes it. Slightly slower and larger PCH, much cheaper for every object that uses it.
if (CMAKE_CXX_COMPILER_ID MATCHES "Clang|AppleClang")
    target_compile_options(${PROJECT_NAME} PRIVATE
        $<$<COMPILE_LANGUAGE:CXX>:-fpch-instantiate-templates>
    )
endif()

################################################################################
# Pre build events
################################################################################
if (CMAKE_GENERATOR MATCHES "Visual Studio")
    set(VS_COPY_ASSETS_CMD ${CMAKE_COMMAND} -E copy_directory_if_different $<TARGET_FILE_DIR:2ship>/assets ${CMAKE_BINARY_DIR}/mm/assets)
endif()
if(NOT CMAKE_SYSTEM_NAME MATCHES "NintendoSwitch|CafeOS")
    add_custom_command(
        TARGET ${PROJECT_NAME}
        POST_BUILD
        COMMENT "Copying asset xmls..."
        COMMAND ${CMAKE_COMMAND} -E copy_directory_if_different ${CMAKE_SOURCE_DIR}/mm/assets/extractor $<TARGET_FILE_DIR:2ship>/assets
        COMMAND ${CMAKE_COMMAND} -E copy_directory_if_different ${CMAKE_SOURCE_DIR}/mm/assets/xml $<TARGET_FILE_DIR:2ship>/assets/xml
        COMMAND ${CMAKE_COMMAND} -E make_directory $<TARGET_FILE_DIR:2ship>/assets/symbols
        COMMAND ${VS_COPY_ASSETS_CMD}
    )
endif()
################################################################################
# Dependencies
################################################################################
add_dependencies(${PROJECT_NAME}
    libultraship
)
if(NOT CMAKE_SYSTEM_NAME MATCHES "NintendoSwitch|CafeOS")
add_dependencies(${PROJECT_NAME}
    ZAPDLib
)
endif()

if (CMAKE_SYSTEM_NAME STREQUAL "Windows")
    find_package(glfw3 REQUIRED)

    find_package(Ogg CONFIG REQUIRED)
    link_libraries(Ogg::ogg)

    find_package(Vorbis CONFIG REQUIRED)
    link_libraries(Vorbis::vorbisfile)
    find_package(Opus CONFIG REQUIRED)
    link_libraries(Opus::opus)
	find_package(OpusFile CONFIG REQUIRED)
    link_libraries(OpusFile::opusfile CONFIG REQUIRED)

	set(ADDITIONAL_LIBRARY_DEPENDENCIES
		"libultraship;"
		"ZAPDLib;"
		"glu32;"
		"SDL2::SDL2;"
		"SDL2::SDL2main;"
        #"$<$<BOOL:${BUILD_CROWD_CONTROL}>:SDL2_net::SDL2_net-static>"
		"glfw;"
		"winmm;"
		"imm32;"
		"version;"
		"setupapi;"
        "Ogg::ogg"
        "Opus::opus"
        "Vorbis::vorbis"
        "Vorbis::vorbisenc"
        "Vorbis::vorbisfile"
		"OpusFile::opusfile"
	)
elseif(CMAKE_SYSTEM_NAME STREQUAL "NintendoSwitch")
    find_package(SDL2)
    set(THREADS_PREFER_PTHREAD_FLAG ON)
    find_package(Threads REQUIRED)
    set(ADDITIONAL_LIBRARY_DEPENDENCIES
        "libultraship;"
        SDL2::SDL2
        -lglad
        Threads::Threads
    )
elseif(CMAKE_SYSTEM_NAME STREQUAL "CafeOS")
    find_package(SDL2 REQUIRED)
    set(ADDITIONAL_LIBRARY_DEPENDENCIES
        "libultraship;"
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
    find_package(Ogg REQUIRED)
    find_package(Vorbis REQUIRED)
    find_package(Opus REQUIRED)
    find_package(OpusFile REQUIRED)
    set(ADDITIONAL_LIBRARY_DEPENDENCIES
        "libultraship;"
		"ZAPDLib;"
        "Ogg::ogg"
        "Vorbis::vorbis"
        "Vorbis::vorbisenc"
        "Vorbis::vorbisfile"
        "Opus::opus"
        "Opusfile::Opusfile"
		SDL2::SDL2
 #       "$<$<BOOL:${BUILD_CROWD_CONTROL}>:SDL2_net::SDL2_net>"
		${CMAKE_DL_LIBS}
		Threads::Threads
	)

endif()

if(NOT CMAKE_SYSTEM_NAME MATCHES "Darwin|NintendoSwitch|CafeOS")
INSTALL(TARGETS 2ship DESTINATION . COMPONENT 2s2h)
endif()

if ("${CMAKE_SYSTEM_NAME}" STREQUAL "Windows")
INSTALL(FILES $<TARGET_PDB_FILE:2ship> DESTINATION ./debug COMPONENT 2s2h)
INSTALL(FILES ${CMAKE_BINARY_DIR}/mm/2ship.o2r DESTINATION . COMPONENT 2s2h)
endif()

find_program(CURL NAMES curl DOC "Path to the curl program.  Used to download files.")
execute_process(COMMAND ${CURL} -sSfL https://raw.githubusercontent.com/gabomdq/SDL_GameControllerDB/master/gamecontrollerdb.txt -o ${CMAKE_BINARY_DIR}/gamecontrollerdb.txt OUTPUT_VARIABLE RESULT)

if("${CMAKE_SYSTEM_NAME}" STREQUAL "Darwin")
configure_file( ${CMAKE_CURRENT_SOURCE_DIR}/macosx/Info.plist.in ${CMAKE_BINARY_DIR}/macosx/Info.plist @ONLY)
INSTALL(TARGETS 2ship DESTINATION ../MacOS COMPONENT 2s2h)
INSTALL(FILES ${CMAKE_BINARY_DIR}/gamecontrollerdb.txt DESTINATION ../MacOS COMPONENT 2s2h)
INSTALL(FILES ${CMAKE_BINARY_DIR}/mm/2ship.o2r DESTINATION ../Resources COMPONENT 2s2h)
elseif(NOT "${CMAKE_SYSTEM_NAME}" MATCHES "NintendoSwitch|CafeOS")
INSTALL(FILES ${CMAKE_BINARY_DIR}/gamecontrollerdb.txt DESTINATION . COMPONENT 2s2h)
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

nx_generate_nacp(2s2h.nacp
   NAME "2s2h of Harkinian"
   AUTHOR "${PROJECT_TEAM}"
   VERSION "${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH}"
)

nx_create_nro(2s2h
    NACP 2s2h.nacp
    ICON ${CMAKE_CURRENT_SOURCE_DIR}/icon.jpg
)

INSTALL(FILES ${CMAKE_CURRENT_BINARY_DIR}/2s2h.nro DESTINATION . COMPONENT 2s2h)

elseif(CMAKE_SYSTEM_NAME MATCHES "CafeOS")

wut_create_rpx(${PROJECT_NAME})

wut_create_wuhb(${PROJECT_NAME}
	NAME       "Two Ship Two Harkinian"
	SHORTNAME  "2S2H"
	AUTHOR     "${PROJECT_TEAM}"
	ICON       ${CMAKE_CURRENT_SOURCE_DIR}/icon.jpg
)

INSTALL(FILES ${CMAKE_CURRENT_BINARY_DIR}/2s2h.rpx ${CMAKE_CURRENT_BINARY_DIR}/2s2h.wuhb DESTINATION . COMPONENT 2s2h)

endif()
