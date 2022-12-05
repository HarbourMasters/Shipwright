set(PROJECT_NAME ZAPD)

set(CMAKE_CXX_STANDARD 20 CACHE STRING "The C++ standard to use")
#set(CMAKE_C_STANDARD 11 CACHE STRING "The C standard to use")
set(CMAKE_C_STANDARD 11)

################################################################################
# Source groups
################################################################################
set(Header_Files
    "../lib/tinyxml2/tinyxml2.h"
    "CRC32.h"
    "Declaration.h"
    "FileWorker.h"
    "GameConfig.h"
    "Globals.h"
    "ImageBackend.h"
    "OutputFormatter.h"
    "WarningHandler.h"
)
source_group("Header Files" FILES ${Header_Files})

set(Header_Files__Libraries
    "../../libultraship/extern/stb/stb_image.h"
    "../../libultraship/extern/stb/stb_image_write.h"
    "ctpl_stl.h"
)
source_group("Header Files\\Libraries" FILES ${Header_Files__Libraries})

set(Header_Files__Libraries__elfio
    "../lib/elfio/elfio/elf_types.hpp"
    "../lib/elfio/elfio/elfio.hpp"
    "../lib/elfio/elfio/elfio_dump.hpp"
    "../lib/elfio/elfio/elfio_dynamic.hpp"
    "../lib/elfio/elfio/elfio_header.hpp"
    "../lib/elfio/elfio/elfio_note.hpp"
    "../lib/elfio/elfio/elfio_relocation.hpp"
    "../lib/elfio/elfio/elfio_section.hpp"
    "../lib/elfio/elfio/elfio_segment.hpp"
    "../lib/elfio/elfio/elfio_strings.hpp"
    "../lib/elfio/elfio/elfio_symbols.hpp"
    "../lib/elfio/elfio/elfio_utils.hpp"
)
source_group("Header Files\\Libraries\\elfio" FILES ${Header_Files__Libraries__elfio})

set(Header_Files__Libraries__libgfxd
    "../lib/libgfxd/gbi.h"
    "../lib/libgfxd/gfxd.h"
    "../lib/libgfxd/priv.h"
)
source_group("Header Files\\Libraries\\libgfxd" FILES ${Header_Files__Libraries__libgfxd})

set(Header_Files__Yaz0
    "yaz0/readwrite.h"
    "yaz0/yaz0.h"
)
source_group("Header Files\\Yaz0" FILES ${Header_Files__Yaz0})

set(Header_Files__Z64
    "OtherStructs/SkinLimbStructs.h"
    "Overlays/ZOverlay.h"
    "ZAnimation.h"
    "ZArray.h"
    "ZAudio.h"
    "ZBackground.h"
    "ZBlob.h"
    "ZCollision.h"
    "ZCutscene.h"
    "ZCutsceneMM.h"
    "ZDisplayList.h"
    "ZFile.h"
    "ZLimb.h"
    "ZMtx.h"
    "ZPath.h"
    "ZPlayerAnimationData.h"
    "ZResource.h"
    "ZRom.h"
    "ZScalar.h"
    "ZSkeleton.h"
    "ZString.h"
    "ZSymbol.h"
    "ZText.h"
    "ZTexture.h"
    "ZTextureAnimation.h"
    "ZVector.h"
    "ZVtx.h"
)
source_group("Header Files\\Z64" FILES ${Header_Files__Z64})

set(Header_Files__Z64__ZRoom
    "ZRoom/ZRoom.h"
    "ZRoom/ZRoomCommand.h"
)
source_group("Header Files\\Z64\\ZRoom" FILES ${Header_Files__Z64__ZRoom})

set(Header_Files__Z64__ZRoom__Commands
    "ZRoom/Commands/EndMarker.h"
    "ZRoom/Commands/SetActorCutsceneList.h"
    "ZRoom/Commands/SetActorList.h"
    "ZRoom/Commands/SetAlternateHeaders.h"
    "ZRoom/Commands/SetAnimatedMaterialList.h"
    "ZRoom/Commands/SetCameraSettings.h"
    "ZRoom/Commands/SetCollisionHeader.h"
    "ZRoom/Commands/SetCsCamera.h"
    "ZRoom/Commands/SetCutscenes.h"
    "ZRoom/Commands/SetEchoSettings.h"
    "ZRoom/Commands/SetEntranceList.h"
    "ZRoom/Commands/SetExitList.h"
    "ZRoom/Commands/SetLightingSettings.h"
    "ZRoom/Commands/SetLightList.h"
    "ZRoom/Commands/SetMesh.h"
    "ZRoom/Commands/SetMinimapChests.h"
    "ZRoom/Commands/SetMinimapList.h"
    "ZRoom/Commands/SetObjectList.h"
    "ZRoom/Commands/SetPathways.h"
    "ZRoom/Commands/SetRoomBehavior.h"
    "ZRoom/Commands/SetRoomList.h"
    "ZRoom/Commands/SetSkyboxModifier.h"
    "ZRoom/Commands/SetSkyboxSettings.h"
    "ZRoom/Commands/SetSoundSettings.h"
    "ZRoom/Commands/SetSpecialObjects.h"
    "ZRoom/Commands/SetStartPositionList.h"
    "ZRoom/Commands/SetTimeSettings.h"
    "ZRoom/Commands/SetTransitionActorList.h"
    "ZRoom/Commands/SetWind.h"
    "ZRoom/Commands/SetWorldMapVisited.h"
    "ZRoom/Commands/Unused09.h"
    "ZRoom/Commands/Unused1D.h"
    "ZRoom/Commands/ZRoomCommandUnk.h"
)
source_group("Header Files\\Z64\\ZRoom\\Commands" FILES ${Header_Files__Z64__ZRoom__Commands})

set(Resource_Files
    "../../OTRExporter/CFG/SymbolMap_OoTMqDbg.txt"
)
source_group("Resource Files" FILES ${Resource_Files})

set(Source_Files
    "Declaration.cpp"
    "FileWorker.cpp"
    "GameConfig.cpp"
    "Globals.cpp"
    "ImageBackend.cpp"
    "Main.cpp"
    "OutputFormatter.cpp"
    "WarningHandler.cpp"
)
source_group("Source Files" FILES ${Source_Files})

set(Source_Files__Libraries__libgfxd
    "../lib/libgfxd/gfxd.c"
    "../lib/libgfxd/uc.c"
    "../lib/libgfxd/uc_f3d.c"
    "../lib/libgfxd/uc_f3db.c"
    "../lib/libgfxd/uc_f3dex.c"
    "../lib/libgfxd/uc_f3dex2.c"
    "../lib/libgfxd/uc_f3dexb.c"
)
source_group("Source Files\\Libraries\\libgfxd" FILES ${Source_Files__Libraries__libgfxd})

set(Source_Files__Yaz0
    "yaz0/yaz0.cpp"
)
source_group("Source Files\\Yaz0" FILES ${Source_Files__Yaz0})

set(Source_Files__Z64
    "OtherStructs/SkinLimbStructs.cpp"
    "Overlays/ZOverlay.cpp"
    "ZAnimation.cpp"
    "ZArray.cpp"
    "ZAudio.cpp"
    "ZAudioDecode.cpp"
    "ZBackground.cpp"
    "ZBlob.cpp"
    "ZCollision.cpp"
    "ZCutscene.cpp"
    "ZCutsceneMM.cpp"
    "ZDisplayList.cpp"
    "ZFile.cpp"
    "ZLimb.cpp"
    "ZMtx.cpp"
    "ZPath.cpp"
    "ZPlayerAnimationData.cpp"
    "ZResource.cpp"
    "ZRom.cpp"
    "ZScalar.cpp"
    "ZSkeleton.cpp"
    "ZString.cpp"
    "ZSymbol.cpp"
    "ZText.cpp"
    "ZTexture.cpp"
    "ZTextureAnimation.cpp"
    "ZVector.cpp"
    "ZVtx.cpp"
)
source_group("Source Files\\Z64" FILES ${Source_Files__Z64})

set(Source_Files__Z64__ZRoom
    "ZRoom/ZRoom.cpp"
    "ZRoom/ZRoomCommand.cpp"
)
source_group("Source Files\\Z64\\ZRoom" FILES ${Source_Files__Z64__ZRoom})

set(Source_Files__Z64__ZRoom__Commands
    "ZRoom/Commands/EndMarker.cpp"
    "ZRoom/Commands/SetActorCutsceneList.cpp"
    "ZRoom/Commands/SetActorList.cpp"
    "ZRoom/Commands/SetAlternateHeaders.cpp"
    "ZRoom/Commands/SetAnimatedMaterialList.cpp"
    "ZRoom/Commands/SetCameraSettings.cpp"
    "ZRoom/Commands/SetCollisionHeader.cpp"
    "ZRoom/Commands/SetCsCamera.cpp"
    "ZRoom/Commands/SetCutscenes.cpp"
    "ZRoom/Commands/SetEchoSettings.cpp"
    "ZRoom/Commands/SetEntranceList.cpp"
    "ZRoom/Commands/SetExitList.cpp"
    "ZRoom/Commands/SetLightingSettings.cpp"
    "ZRoom/Commands/SetLightList.cpp"
    "ZRoom/Commands/SetMesh.cpp"
    "ZRoom/Commands/SetMinimapChests.cpp"
    "ZRoom/Commands/SetMinimapList.cpp"
    "ZRoom/Commands/SetObjectList.cpp"
    "ZRoom/Commands/SetPathways.cpp"
    "ZRoom/Commands/SetRoomBehavior.cpp"
    "ZRoom/Commands/SetRoomList.cpp"
    "ZRoom/Commands/SetSkyboxModifier.cpp"
    "ZRoom/Commands/SetSkyboxSettings.cpp"
    "ZRoom/Commands/SetSoundSettings.cpp"
    "ZRoom/Commands/SetSpecialObjects.cpp"
    "ZRoom/Commands/SetStartPositionList.cpp"
    "ZRoom/Commands/SetTimeSettings.cpp"
    "ZRoom/Commands/SetTransitionActorList.cpp"
    "ZRoom/Commands/SetWind.cpp"
    "ZRoom/Commands/SetWorldMapVisited.cpp"
    "ZRoom/Commands/Unused09.cpp"
    "ZRoom/Commands/Unused1D.cpp"
    "ZRoom/Commands/ZRoomCommandUnk.cpp"
)
source_group("Source Files\\Z64\\ZRoom\\Commands" FILES ${Source_Files__Z64__ZRoom__Commands})

set(ALL_FILES
    ${Header_Files}
    ${Header_Files__Libraries}
    ${Header_Files__Libraries__elfio}
    ${Header_Files__Libraries__libgfxd}
    ${Header_Files__Yaz0}
    ${Header_Files__Z64}
    ${Header_Files__Z64__ZRoom}
    ${Header_Files__Z64__ZRoom__Commands}
    ${Resource_Files}
    ${Source_Files}
    ${Source_Files__Libraries__libgfxd}
    ${Source_Files__Yaz0}
    ${Source_Files__Z64}
    ${Source_Files__Z64__ZRoom}
    ${Source_Files__Z64__ZRoom__Commands}
    ${any__any}
)

################################################################################
# Target
################################################################################
add_executable(${PROJECT_NAME} ${ALL_FILES})

if (CMAKE_SYSTEM_NAME STREQUAL "Windows")
use_props(${PROJECT_NAME} "${CMAKE_CONFIGURATION_TYPES}" "${DEFAULT_CXX_PROPS}")
endif()
################################################################################
# Includes for CMake from *.props
################################################################################

set(ROOT_NAMESPACE ZAPD)

if (CMAKE_SYSTEM_NAME STREQUAL "Windows")
	if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
		set_target_properties(${PROJECT_NAME} PROPERTIES
			INTERPROCEDURAL_OPTIMIZATION_RELEASE "TRUE"
		)
	elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x86")
		set_target_properties(${PROJECT_NAME} PROPERTIES
			INTERPROCEDURAL_OPTIMIZATION_RELEASE "TRUE"
		)
	endif()
elseif (CMAKE_SYSTEM_NAME MATCHES "Linux|Darwin")
        set_target_properties(${PROJECT_NAME} PROPERTIES
		OUTPUT_NAME "ZAPD.out"
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
	endif()
	set_target_properties(${PROJECT_NAME} PROPERTIES MSVC_RUNTIME_LIBRARY ${MSVC_RUNTIME_LIBRARY_STR})
endif()
################################################################################
# Compile definitions
################################################################################
if (CMAKE_SYSTEM_NAME STREQUAL "Windows")
	if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
		target_compile_definitions(${PROJECT_NAME} PRIVATE
			"_CRT_SECURE_NO_WARNINGS;"
			"_MBCS"
			STORMLIB_NO_AUTO_LINK
		)
	elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x86")
		target_compile_definitions(${PROJECT_NAME} PRIVATE
			"$<$<CONFIG:Debug>:"
				"_CRT_SECURE_NO_WARNINGS"
			">"
			"_MBCS"
			STORMLIB_NO_AUTO_LINK
		)
	endif()
endif()

################################################################################
# Compile and link options
################################################################################

if (CMAKE_SYSTEM_NAME STREQUAL "Darwin")
	set(CMAKE_FIND_FRAMEWORK LAST)
endif()
find_package(PNG REQUIRED)

target_include_directories(${PROJECT_NAME} PRIVATE
	${CMAKE_CURRENT_SOURCE_DIR}/../../libultraship/extern/ZAPDUtils
	${CMAKE_CURRENT_SOURCE_DIR}/../../ZAPDTR/lib/tinyxml2
	${CMAKE_CURRENT_SOURCE_DIR}/../../ZAPDTR/lib/libgfxd
	${PNG_PNG_INCLUDE_DIR}/
	.
	)

if(MSVC)
    if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
        target_compile_options(${PROJECT_NAME} PRIVATE
            $<$<CONFIG:Debug>:
                /Od;
                /RTC1
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
    elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x86")
        target_compile_options(${PROJECT_NAME} PRIVATE
            $<$<CONFIG:Debug>:
                /Od
            >
            $<$<CONFIG:Release>:
                /O2;
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
            $<$<CONFIG:Debug>:
                /PROFILE
            >
            $<$<CONFIG:Release>:
                /OPT:REF;
                /OPT:ICF
            >
            /DEBUG:FULL
        )
    elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x86")
        target_link_options(${PROJECT_NAME} PRIVATE
            $<$<CONFIG:Debug>:
                /PROFILE
            >
            $<$<CONFIG:Release>:
                /OPT:REF;
                /OPT:ICF
            >
            /DEBUG:FULL
        )
    endif()
endif()

if ("${CMAKE_CXX_COMPILER_ID}" MATCHES "GNU|Clang|AppleClang")
	target_compile_options(${PROJECT_NAME} PRIVATE
		-Wall -Wextra -Wno-error
        -Wno-unused-parameter
        -Wno-unused-function
        -Wno-unused-variable
		-Wno-missing-field-initializers
        -Wno-parentheses
        -Wno-narrowing
        $<$<COMPILE_LANGUAGE:CXX>:-Wno-deprecated-enum-enum-conversion>
		-pthread
	)
	
    if (CMAKE_SYSTEM_NAME STREQUAL "Darwin")
        target_link_options(${PROJECT_NAME} PRIVATE
            -pthread
        )
    else()
        target_link_options(${PROJECT_NAME} PRIVATE
            -pthread
            -Wl,-export-dynamic
        )
    endif()

endif()

################################################################################
# Dependencies
################################################################################
add_dependencies(${PROJECT_NAME}
    OTRExporter
    ZAPDUtils
    libultraship
)

if (CMAKE_SYSTEM_NAME STREQUAL "Windows")
	if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
		set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
		set(ADDITIONAL_LIBRARY_DEPENDENCIES
			"ZAPDUtils;"
			"-WHOLEARCHIVE:$<TARGET_LINKER_FILE_DIR:OTRExporter>/$<TARGET_LINKER_FILE_NAME:OTRExporter>"
			"libultraship;"
			storm
			PNG::PNG
		)
	endif()
elseif(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
	set(THREADS_PREFER_PTHREAD_FLAG ON)
	find_package(Threads REQUIRED)
	set(ADDITIONAL_LIBRARY_DEPENDENCIES
		"ZAPDUtils;"
		-Wl,-force_load $<TARGET_LINKER_FILE_DIR:OTRExporter>/$<TARGET_LINKER_FILE_NAME:OTRExporter>
		"libultraship;"
		PNG::PNG
		${CMAKE_DL_LIBS}
		Threads::Threads
	)
elseif(CMAKE_SYSTEM_NAME STREQUAL "NintendoSwitch")
       set(THREADS_PREFER_PTHREAD_FLAG ON)
       find_package(Threads REQUIRED)
       set(ADDITIONAL_LIBRARY_DEPENDENCIES
               "ZAPDUtils;"
               -Wl,--whole-archive $<TARGET_LINKER_FILE_DIR:OTRExporter>/$<TARGET_LINKER_FILE_NAME:OTRExporter> -Wl,--no-whole-archive
               "libultraship;"
               PNG::PNG
               Threads::Threads
       )
elseif(CMAKE_SYSTEM_NAME STREQUAL "CafeOS")
    set(ADDITIONAL_LIBRARY_DEPENDENCIES
        "ZAPDUtils;"
        "libultraship;"
        PNG::PNG
    )
else()
	set(THREADS_PREFER_PTHREAD_FLAG ON)
	find_package(Threads REQUIRED)
	set(ADDITIONAL_LIBRARY_DEPENDENCIES
		"ZAPDUtils;"
		-Wl,--whole-archive $<TARGET_LINKER_FILE_DIR:OTRExporter>/$<TARGET_LINKER_FILE_NAME:OTRExporter> -Wl,--no-whole-archive
		"libultraship;"
		PNG::PNG
		${CMAKE_DL_LIBS}
		Threads::Threads
	)
endif()

if(CMAKE_SYSTEM_NAME MATCHES "NintendoSwitch|CafeOS")
add_library(pathconf OBJECT pathconf.c)
target_link_libraries(${PROJECT_NAME} PRIVATE "${ADDITIONAL_LIBRARY_DEPENDENCIES}" $<TARGET_OBJECTS:pathconf> )
else()
target_link_libraries(${PROJECT_NAME} PRIVATE "${ADDITIONAL_LIBRARY_DEPENDENCIES}")
endif()
