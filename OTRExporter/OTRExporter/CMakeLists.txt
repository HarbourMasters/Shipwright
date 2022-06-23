set(PROJECT_NAME OTRExporter)

################################################################################
# Source groups
################################################################################
set(Header_Files
    "AnimationExporter.h"
    "AudioExporter.h"
    "ArrayExporter.h"
    "BackgroundExporter.h"
    "BlobExporter.h"
    "CollisionExporter.h"
    "command_macros_base.h"
    "CutsceneExporter.h"
    "DisplayListExporter.h"
    "Exporter.h"
    "Main.h"
    "MtxExporter.h"
    "PathExporter.h"
    "PlayerAnimationExporter.h"
    "RoomExporter.h"
    "SkeletonExporter.h"
    "SkeletonLimbExporter.h"
    "TextExporter.h"
    "TextureExporter.h"
    "VersionInfo.h"
    "VtxExporter.h"
    "z64cutscene.h"
    "z64cutscene_commands.h"
)
source_group("Header Files" FILES ${Header_Files})

set(Source_Files
    "AnimationExporter.cpp"
    "ArrayExporter.cpp"
    "AudioExporter.cpp"
    "BackgroundExporter.cpp"
    "BlobExporter.cpp"
    "CollisionExporter.cpp"
    "CutsceneExporter.cpp"
    "DisplayListExporter.cpp"
    "Exporter.cpp"
    "Main.cpp"
    "MtxExporter.cpp"
    "PathExporter.cpp"
    "PlayerAnimationExporter.cpp"
    "RoomExporter.cpp"
    "SkeletonExporter.cpp"
    "SkeletonLimbExporter.cpp"
    "TextExporter.cpp"
    "TextureExporter.cpp"
    "VersionInfo.cpp"
    "VtxExporter.cpp"
)
source_group("Source Files" FILES ${Source_Files})

set(ALL_FILES
    ${Header_Files}
    ${Source_Files}
)

################################################################################
# Target
################################################################################
add_library(${PROJECT_NAME} STATIC ${ALL_FILES})

use_props(${PROJECT_NAME} "${CMAKE_CONFIGURATION_TYPES}" "${DEFAULT_CXX_PROPS}")
set(ROOT_NAMESPACE OTRExporter)

if (CMAKE_SYSTEM_NAME STREQUAL "Windows")
	set_target_properties(${PROJECT_NAME} PROPERTIES
		VS_GLOBAL_KEYWORD "Win32Proj"
	)
	if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
		set_target_properties(${PROJECT_NAME} PROPERTIES
			INTERPROCEDURAL_OPTIMIZATION_RELEASE "TRUE"
		)
	elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x86")
		set_target_properties(${PROJECT_NAME} PROPERTIES
			INTERPROCEDURAL_OPTIMIZATION_RELEASE "TRUE"
		)
	endif()
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
	endif()
	set_target_properties(${PROJECT_NAME} PROPERTIES MSVC_RUNTIME_LIBRARY ${MSVC_RUNTIME_LIBRARY_STR})
endif()
################################################################################
# Compile definitions
################################################################################
if (CMAKE_SYSTEM_NAME STREQUAL "Windows")
	if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
		target_compile_definitions(${PROJECT_NAME} PRIVATE
			"$<$<CONFIG:Debug>:"
				"_DEBUG"
			">"
			"$<$<CONFIG:Release>:"
				"NDEBUG"
			">"
			"_CONSOLE;"
			"_CRT_SECURE_NO_WARNINGS;"
			"UNICODE;"
			"_UNICODE"
			STORMLIB_NO_AUTO_LINK
		)
	elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x86")
		target_compile_definitions(${PROJECT_NAME} PRIVATE
			"$<$<CONFIG:Debug>:"
				"_DEBUG;"
				"_CRT_SECURE_NO_WARNINGS"
			">"
			"$<$<CONFIG:Release>:"
				"NDEBUG"
			">"
			"WIN32;"
			"_CONSOLE;"
			"UNICODE;"
			"_UNICODE"
			STORMLIB_NO_AUTO_LINK
		)
	endif()
endif()

if ("${CMAKE_CXX_COMPILER_ID}" MATCHES "GNU|Clang|AppleClang")
	target_compile_definitions(${PROJECT_NAME} PRIVATE
		"$<$<CONFIG:Debug>:"
				"_DEBUG"
		">"
		"$<$<CONFIG:Release>:"
			"NDEBUG"
		">"
		"_CONSOLE;"
		"_CRT_SECURE_NO_WARNINGS;"
		"UNICODE;"
		"_UNICODE"
	)
endif()
################################################################################
# Compile and link options
################################################################################

target_include_directories(${PROJECT_NAME} PRIVATE
	${PROJECT_SOURCE_DIR}/../ZAPDTR/ZAPD/
	${PROJECT_SOURCE_DIR}/../ZAPDTR/ZAPDUtils
	${PROJECT_SOURCE_DIR}/../ZAPDTR/lib/tinyxml2
	${PROJECT_SOURCE_DIR}/../libultraship/libultraship
	${PROJECT_SOURCE_DIR}/../libultraship/libultraship/Lib/spdlog/include
	${PROJECT_SOURCE_DIR}/../libultraship/libultraship/Lib/Fast3D/U64
	${PROJECT_SOURCE_DIR}/../StormLib/src
	.
)

if(MSVC)
    if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
        target_compile_options(${PROJECT_NAME} PRIVATE
            $<$<CONFIG:Release>:
                /Oi;
                /Gy
            >
            /permissive-;
            /std:c++17;
            /sdl;
            /W3;
            ${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT};
            ${DEFAULT_CXX_EXCEPTION_HANDLING}
        )
    elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x86")
        target_compile_options(${PROJECT_NAME} PRIVATE
            $<$<CONFIG:Debug>:
                /std:c++17
            >
            $<$<CONFIG:Release>:
                /Oi;
                /Gy
            >
            /permissive-;
            /sdl;
            /W3;
            ${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT};
            ${DEFAULT_CXX_EXCEPTION_HANDLING}
        )
    endif()
    if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
        target_link_options(${PROJECT_NAME} PRIVATE
            $<$<CONFIG:Release>:
                /OPT:REF;
                /OPT:ICF
            >
            /SUBSYSTEM:CONSOLE
        )
    elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x86")
        target_link_options(${PROJECT_NAME} PRIVATE
            $<$<CONFIG:Release>:
                /DEBUG;
                /OPT:REF;
                /OPT:ICF;
                /INCREMENTAL:NO
            >
            /SUBSYSTEM:CONSOLE
        )
    endif()
endif()

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
	target_compile_options(${PROJECT_NAME} PRIVATE
		-Wall -Wextra -Wno-error
        -Wno-unused-parameter
        -Wno-unused-function
        -Wno-unused-variable
		-Wno-missing-field-initializers
        -Wno-parentheses
        -Wno-narrowing
        $<$<COMPILE_LANGUAGE:CXX>:-Wno-deprecated-enum-enum-conversion>
	)
endif()
################################################################################
# Dependencies
################################################################################
add_dependencies(${PROJECT_NAME}
    libultraship
)

# Link with other targets.

if (CMAKE_SYSTEM_NAME STREQUAL "Windows")
	if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
		set(ADDITIONAL_LIBRARY_DEPENDENCIES
			"$<$<CONFIG:Debug>:"
				"ZAPDUtils;"
				"OTRLib"
			">"
		)
	endif()
endif()
target_link_libraries(${PROJECT_NAME} PUBLIC "${ADDITIONAL_LIBRARY_DEPENDENCIES}")

