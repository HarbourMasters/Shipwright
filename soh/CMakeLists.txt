cmake_minimum_required(VERSION 3.16.0 FATAL_ERROR)

set(CMAKE_SYSTEM_VERSION 10.0 CACHE STRING "" FORCE)

project(soh C CXX)
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
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../libultraship/libultraship ${CMAKE_BINARY_DIR}/libultraship)
endif()

if (NOT TARGET ZAPDUtils)
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../ZAPDTR/ZAPDUtils ${CMAKE_BINARY_DIR}/ZAPDUtils)
endif()

set(PROJECT_NAME soh)

################################################################################
# Source groups
################################################################################
configure_file( ${CMAKE_CURRENT_SOURCE_DIR}/src/boot/build.c.in ${CMAKE_BINARY_DIR}/build.c @ONLY)

set(Header_Files
    "resource.h"
)
source_group("Header Files" FILES ${Header_Files})

set(Header_Files__include
    "include/alloca.h"
    #"include/bgm.h"
    "include/command_macros_base.h"
    "include/fp.h"
    "include/functions.h"
    "include/global.h"
    "include/ichain.h"
    "include/macro.inc"
    "include/macros.h"
    "include/message_data_fmt.h"
    "include/message_data_static.h"
    "include/message_data_textbox_types.h"
    #"include/math_n64.h"
    "include/regs.h"
    "include/segment_symbols.h"
    "include/sfx.h"
    "include/libc/stdarg.h"
    #"include/stdbool_n64.h"
    #"include/stddef_n64.h"
    #"include/stdlib_n64.h"
    "include/ultra64.h"
    "include/unk.h"
    "include/variables.h"
    "include/vt.h"
    "include/z64.h"
    "include/z64actor.h"
    "include/z64animation.h"
    "include/z64audio.h"
    "include/z64bgcheck.h"
    "include/z64camera.h"
    "include/z64collision_check.h"
    "include/z64cutscene.h"
    "include/z64cutscene_commands.h"
    "include/z64dma.h"
    "include/z64effect.h"
    "include/z64environment.h"
    "include/z64interface.h"
    "include/z64item.h"
    "include/z64light.h"
    "include/z64map_mark.h"
    "include/z64math.h"
    "include/z64object.h"
    "include/z64player.h"
    "include/z64save.h"
    "include/z64scene.h"
    "include/z64transition.h"
)
source_group("Header Files\\include" FILES ${Header_Files__include})

set(Header_Files__soh__Enhancements
    "soh/Enhancements/bootcommands.h"
    #"soh/Enhancements/cvar.h"
    "soh/Enhancements/debugconsole.h"
    "soh/Enhancements/gameconsole.h"
    "soh/Enhancements/savestates.h"
    "soh/Enhancements/savestates_extern.inc"
)
source_group("Header Files\\soh\\Enhancements" FILES ${Header_Files__soh__Enhancements})

set(Header_Files__soh__Enhancements__controls
    "soh/Enhancements/controls/GameControlEditor.h"
)
source_group("Header Files\\soh\\Enhancements\\controls" FILES ${Header_Files__soh__Enhancements__controls})


set(Header_Files__soh__Enhancements__cosmetics
    "soh/Enhancements/cosmetics/CosmeticsEditor.h"
)
source_group("Header Files\\soh\\Enhancements\\cosmetics" FILES ${Header_Files__soh__Enhancements__cosmetics})

set(Header_Files__soh__Enhancements__debugger
    "soh/Enhancements/debugger/actorViewer.h"
    "soh/Enhancements/debugger/colViewer.h"
    "soh/Enhancements/debugger/debugger.h"
    "soh/Enhancements/debugger/debugSaveEditor.h"
)
source_group("Header Files\\soh\\Enhancements\\debugger" FILES ${Header_Files__soh__Enhancements__debugger})

set(Header_Files__soh__Enhancements__randomizer
    "soh/Enhancements/randomizer/randomizer.h"
    "soh/Enhancements/randomizer/randomizer_inf.h"
    "soh/Enhancements/randomizer/randomizer_item_tracker.h"
    "soh/Enhancements/randomizer/adult_trade_shuffle.h"
    "soh/Enhancements/randomizer/randomizer_check_objects.h"
    "soh/Enhancements/randomizer/draw.h"
    "soh/Enhancements/randomizer/rando_hash.h"
)
source_group("Header Files\\soh\\Enhancements\\randomizer" FILES ${Header_Files__soh__Enhancements__randomizer})

set(Header_Files__soh__Enhancements__randomizer__3drando
    "soh/Enhancements/randomizer/3drando/category.hpp"
    "soh/Enhancements/randomizer/3drando/cosmetics.hpp"
    "soh/Enhancements/randomizer/3drando/custom_messages.hpp"
    "soh/Enhancements/randomizer/3drando/debug.hpp"
    "soh/Enhancements/randomizer/3drando/dungeon.hpp"
    "soh/Enhancements/randomizer/3drando/entrance.hpp"
    "soh/Enhancements/randomizer/3drando/fill.hpp"
    "soh/Enhancements/randomizer/3drando/hint_list.hpp"
    "soh/Enhancements/randomizer/3drando/hints.hpp"
    "soh/Enhancements/randomizer/3drando/item.hpp"
    "soh/Enhancements/randomizer/3drando/item_list.hpp"
    "soh/Enhancements/randomizer/3drando/item_location.hpp"
    "soh/Enhancements/randomizer/3drando/item_pool.hpp"
    "soh/Enhancements/randomizer/3drando/keys.hpp"
    "soh/Enhancements/randomizer/3drando/location_access.hpp"
    "soh/Enhancements/randomizer/3drando/logic.hpp"
    "soh/Enhancements/randomizer/3drando/menu.hpp"
    "soh/Enhancements/randomizer/3drando/music.hpp"
    "soh/Enhancements/randomizer/3drando/patch.hpp"
    "soh/Enhancements/randomizer/3drando/playthrough.hpp"
    "soh/Enhancements/randomizer/3drando/pool_functions.hpp"
    "soh/Enhancements/randomizer/3drando/preset.hpp"
    "soh/Enhancements/randomizer/3drando/rando_main.hpp"
    "soh/Enhancements/randomizer/3drando/random.hpp"
    "soh/Enhancements/randomizer/3drando/setting_descriptions.hpp"
    "soh/Enhancements/randomizer/3drando/settings.hpp"
    "soh/Enhancements/randomizer/3drando/shops.hpp"
    "soh/Enhancements/randomizer/3drando/sound_effects.hpp"
    "soh/Enhancements/randomizer/3drando/spoiler_log.hpp"
    "soh/Enhancements/randomizer/3drando/starting_inventory.hpp"
    "soh/Enhancements/randomizer/3drando/text.hpp"
    "soh/Enhancements/randomizer/3drando/tinyxml2.h"
    "soh/Enhancements/randomizer/3drando/trial.hpp"
    "soh/Enhancements/randomizer/3drando/utils.hpp"
)

source_group("Header Files\\soh\\Enhancements\\randomizer\\3drando" FILES ${Header_Files__soh__Enhancements__randomizer__3drando})

set(Header_Files__soh__Enhancements__custom_message
    "soh/Enhancements/custom-message/CustomMessageTypes.h"
    "soh/Enhancements/custom-message/CustomMessageManager.h"
)

source_group("Header Files\\soh\\Enhancements\\custom-message" FILES ${Header_Files__soh__Enhancements__custom_message})

set(Header_Files__soh__Enhancements__item_tables
    "soh/Enhancements/item-tables/ItemTableManager.h"
    "soh/Enhancements/item-tables/ItemTableTypes.h"
)

source_group("Header Files\\soh\\Enhancements\\item-tables" FILES ${Header_Files__soh__Enhancements__item_tables})

if (CMAKE_SYSTEM_NAME STREQUAL "Windows")
set(Header_Files__soh__Enhancements__crowd_control
    "soh/Enhancements/crowd-control/CrowdControl.h"
)
source_group("Header Files\\soh\\Enhancements\\crowd-control" FILES ${Header_Files__soh__Enhancements__crowd_control})
endif()

set(Source_Files__soh
    "soh/GbiWrap.cpp"
    "soh/OTRAudio.h"
    "soh/OTRGlobals.cpp"
    "soh/OTRGlobals.h"
    "soh/SaveManager.h"
    "soh/SaveManager.cpp"
    "soh/frame_interpolation.h"
    "soh/frame_interpolation.cpp"
    "soh/gu_pc.c"
    "soh/stubs.c"
    "soh/util.h"
    "soh/util.cpp"
    "soh/z_message_OTR.cpp"
    "soh/z_play_otr.cpp"
    "soh/z_scene_otr.cpp"
    "soh/GameMenuBar.hpp"
    "soh/GameMenuBar.cpp"
    "soh/UIWidgets.hpp"
    "soh/UIWidgets.cpp"
    "soh/CrashHandlerExt.cpp"
)
source_group("Source Files\\soh" FILES ${Source_Files__soh})

set(Source_Files__soh__Enhancements
    "soh/Enhancements/bootcommands.c"
    "soh/Enhancements/debugconsole.cpp"
    "soh/Enhancements/gameconsole.c"
    "soh/Enhancements/savestates.cpp"
)
source_group("Source Files\\soh\\Enhancements" FILES ${Source_Files__soh__Enhancements})

set(Source_Files__soh__Enhancements__controls
    "soh/Enhancements/controls/GameControlEditor.cpp"
)
source_group("Source Files\\soh\\Enhancements\\controls" FILES ${Source_Files__soh__Enhancements__controls})

set(Source_Files__soh__Enhancements__cosmetics
    "soh/Enhancements/cosmetics/CosmeticsEditor.cpp"
)
source_group("Source Files\\soh\\Enhancements\\cosmetics" FILES ${Source_Files__soh__Enhancements__cosmetics})

set(Source_Files__soh__Enhancements__debugger
    "soh/Enhancements/debugger/actorViewer.cpp"
    "soh/Enhancements/debugger/colViewer.cpp"
    "soh/Enhancements/debugger/debugger.cpp"
    "soh/Enhancements/debugger/debugSaveEditor.cpp"
)
source_group("Source Files\\soh\\Enhancements\\debugger" FILES ${Source_Files__soh__Enhancements__debugger})

set(Source_Files__soh__Enhancements__randomizer
    "soh/Enhancements/randomizer/randomizer.cpp"
    "soh/Enhancements/randomizer/randomizer_item_tracker.cpp"
    "soh/Enhancements/randomizer/adult_trade_shuffle.c"
    "soh/Enhancements/randomizer/randomizer_check_objects.cpp"
    "soh/Enhancements/randomizer/draw.cpp"
)
source_group("Source Files\\soh\\Enhancements\\randomizer" FILES ${Source_Files__soh__Enhancements__randomizer})

set(Source_Files__soh__Enhancements__randomizer__3drando
    "soh/Enhancements/randomizer/3drando/cosmetics.cpp"
    "soh/Enhancements/randomizer/3drando/custom_messages.cpp"
    "soh/Enhancements/randomizer/3drando/debug.cpp"
    "soh/Enhancements/randomizer/3drando/dungeon.cpp"
    "soh/Enhancements/randomizer/3drando/entrance.cpp"
    "soh/Enhancements/randomizer/3drando/fill.cpp"
    "soh/Enhancements/randomizer/3drando/hint_list.cpp"
    "soh/Enhancements/randomizer/3drando/hints.cpp"
    "soh/Enhancements/randomizer/3drando/item.cpp"
    "soh/Enhancements/randomizer/3drando/item_list.cpp"
    "soh/Enhancements/randomizer/3drando/item_location.cpp"
    "soh/Enhancements/randomizer/3drando/item_pool.cpp"
    "soh/Enhancements/randomizer/3drando/location_access.cpp"
    "soh/Enhancements/randomizer/3drando/logic.cpp"
    "soh/Enhancements/randomizer/3drando/menu.cpp"
    "soh/Enhancements/randomizer/3drando/music.cpp"
    "soh/Enhancements/randomizer/3drando/patch.cpp"
    "soh/Enhancements/randomizer/3drando/playthrough.cpp"
    "soh/Enhancements/randomizer/3drando/preset.cpp"
    "soh/Enhancements/randomizer/3drando/rando_main.cpp"
    "soh/Enhancements/randomizer/3drando/random.cpp"
    "soh/Enhancements/randomizer/3drando/setting_descriptions.cpp"
    "soh/Enhancements/randomizer/3drando/settings.cpp"
    "soh/Enhancements/randomizer/3drando/shops.cpp"
    "soh/Enhancements/randomizer/3drando/sound_effects.cpp"
    "soh/Enhancements/randomizer/3drando/spoiler_log.cpp"
    "soh/Enhancements/randomizer/3drando/starting_inventory.cpp"
    "soh/Enhancements/randomizer/3drando/tinyxml2.cpp"
    "soh/Enhancements/randomizer/3drando/trial.cpp"
    "soh/Enhancements/randomizer/3drando/utils.cpp"
)
source_group("Source Files\\soh\\Enhancements\\randomizer\\3drando" FILES ${Source_Files__soh__Enhancements__randomizer__3drando})

set(Source_Files__soh__Enhancements__randomizer__3drando__hint_list
    "soh/Enhancements/randomizer/3drando/hint_list/hint_list_exclude_dungeon.cpp"
    "soh/Enhancements/randomizer/3drando/hint_list/hint_list_exclude_overworld.cpp"
    "soh/Enhancements/randomizer/3drando/hint_list/hint_list_item.cpp"
)
source_group("Source Files\\soh\\Enhancements\\randomizer\\3drando\\hint_list" FILES ${Source_Files__soh__Enhancements__randomizer__3drando__hint_list}) 

set(Source_Files__soh__Enhancements__randomizer__3drando__location_access
    "soh/Enhancements/randomizer/3drando/location_access/locacc_bottom_of_the_well.cpp"
    "soh/Enhancements/randomizer/3drando/location_access/locacc_castle_town.cpp"
    "soh/Enhancements/randomizer/3drando/location_access/locacc_death_mountain.cpp"
    "soh/Enhancements/randomizer/3drando/location_access/locacc_deku_tree.cpp"
    "soh/Enhancements/randomizer/3drando/location_access/locacc_dodongos_cavern.cpp"
    "soh/Enhancements/randomizer/3drando/location_access/locacc_fire_temple.cpp"
    "soh/Enhancements/randomizer/3drando/location_access/locacc_forest_temple.cpp"
    "soh/Enhancements/randomizer/3drando/location_access/locacc_ganons_castle.cpp"
    "soh/Enhancements/randomizer/3drando/location_access/locacc_gerudo_training_grounds.cpp"
    "soh/Enhancements/randomizer/3drando/location_access/locacc_gerudo_valley.cpp"
    "soh/Enhancements/randomizer/3drando/location_access/locacc_hyrule_field.cpp"
    "soh/Enhancements/randomizer/3drando/location_access/locacc_ice_cavern.cpp"
    "soh/Enhancements/randomizer/3drando/location_access/locacc_jabujabus_belly.cpp"
    "soh/Enhancements/randomizer/3drando/location_access/locacc_kakariko.cpp"
    "soh/Enhancements/randomizer/3drando/location_access/locacc_lost_woods.cpp"
    "soh/Enhancements/randomizer/3drando/location_access/locacc_shadow_temple.cpp"
    "soh/Enhancements/randomizer/3drando/location_access/locacc_spirit_temple.cpp"
    "soh/Enhancements/randomizer/3drando/location_access/locacc_water_temple.cpp"
    "soh/Enhancements/randomizer/3drando/location_access/locacc_zoras_domain.cpp"
)
source_group("Source Files\\soh\\Enhancements\\randomizer\\3drando\\location_access" FILES ${Source_Files__soh__Enhancements__randomizer__3drando__location_access})   

set(Source_Files__soh__Enhancements__custom_message
    "soh/Enhancements/custom-message/CustomMessageManager.cpp"
)

source_group("Source Files\\soh\\Enhancements\\custom-message" FILES ${Source_Files__soh__Enhancements__custom_message})

set(Source_Files__soh__Enhancements__item_tables
    "soh/Enhancements/item-tables/ItemTableManager.cpp"
)

source_group("Source Files\\soh\\Enhancements\\item-tables" FILES ${Source_Files__soh__Enhancements__item_tables})

if (CMAKE_SYSTEM_NAME STREQUAL "Windows")
set(Source_Files__soh__Enhancements__crowd_control
    "soh/Enhancements/crowd-control/CrowdControl.cpp"
)
source_group("Source Files\\soh\\Enhancements\\crowd-control" FILES ${Source_Files__soh__Enhancements__crowd_control})
endif()

set(Source_Files__src__boot
    "src/boot/assert.c"
    "src/boot/boot_main.c"
    "${CMAKE_BINARY_DIR}/build.c"
    "src/boot/idle.c"
    "src/boot/is_debug.c"
    "src/boot/logutils.c"
    "src/boot/stackcheck.c"
    "src/boot/viconfig.c"
    "src/boot/yaz0.c"
    "src/boot/z_locale.c"
    "src/boot/z_std_dma.c"
)
source_group("Source Files\\src\\boot" FILES ${Source_Files__src__boot})

set(Source_Files__src__buffers
    "src/buffers/gfxbuffers.c"
    "src/buffers/heaps.c"
    "src/buffers/zbuffer.c"
)
source_group("Source Files\\src\\buffers" FILES ${Source_Files__src__buffers})

set(Source_Files__src__code
    "src/code/__osMalloc.c"
    "src/code/audio_data.c"
    "src/code/audio_effects.c"
    "src/code/audio_heap.c"
    "src/code/audio_init_params.c"
    "src/code/audio_load.c"
    "src/code/audio_playback.c"
    "src/code/audio_seqplayer.c"
    "src/code/audio_sound_params.c"
    "src/code/audio_synthesis.c"
    "src/code/audioMgr.c"
    "src/code/code_800430A0.c"
    "src/code/code_80043480.c"
    "src/code/code_8006C3A0.c"
    "src/code/code_8006C510.c"
    "src/code/code_80097A00.c"
    "src/code/code_800A9F30.c"
    "src/code/code_800ACE70.c"
    "src/code/code_800AD920.c"
    "src/code/code_800BB0A0.c"
    "src/code/code_800C3C20.c"
    "src/code/code_800D2E30.c"
    "src/code/code_800D31A0.c"
    "src/code/code_800E4FE0.c"
    "src/code/code_800E6840.c"
    "src/code/code_800EC960.c"
    "src/code/code_800F7260.c"
    "src/code/code_800F9280.c"
    "src/code/code_800FBCE0.c"
    "src/code/code_800FC620.c"
    "src/code/code_800FCE80.c"
    "src/code/code_800FD970.c"
    "src/code/code_801067F0.c"
    "src/code/code_801068B0.c"
    "src/code/db_camera.c"
    "src/code/debug_malloc.c"
    "src/code/fault.c"
    "src/code/fault_drawer.c"
    "src/code/flg_set.c"
    "src/code/game.c"
    "src/code/gamealloc.c"
    "src/code/gfxprint.c"
    "src/code/graph.c"
    "src/code/irqmgr.c"
    "src/code/jpegdecoder.c"
    "src/code/jpegutils.c"
    "src/code/listalloc.c"
    "src/code/loadfragment2.c"
    "src/code/logseverity.c"
    "src/code/main.c"
    "src/code/mempak.c"
    "src/code/mtxuty-cvt.c"
    "src/code/padmgr.c"
    "src/code/padsetup.c"
    "src/code/padutils.c"
    "src/code/PreRender.c"
    "src/code/printutils.c"
    "src/code/relocation.c"
    "src/code/sched.c"
    "src/code/shrink_window.c"
    "src/code/sleep.c"
    "src/code/speed_meter.c"
    "src/code/sys_cfb.c"
    "src/code/sys_math.c"
    "src/code/sys_math3d.c"
    "src/code/sys_math_atan.c"
    "src/code/sys_matrix.c"
    "src/code/sys_ucode.c"
    "src/code/system_malloc.c"
    "src/code/title_setup.c"
    "src/code/TwoHeadArena.c"
    "src/code/ucode_disas.c"
    "src/code/z_actor.c"
    "src/code/z_actor_dlftbls.c"
    "src/code/z_bgcheck.c"
    "src/code/z_camera.c"
    "src/code/z_cheap_proc.c"
    "src/code/z_collision_btltbls.c"
    "src/code/z_collision_check.c"
    "src/code/z_common_data.c"
    "src/code/z_construct.c"
    "src/code/z_debug.c"
    "src/code/z_debug_display.c"
    "src/code/z_demo.c"
    "src/code/z_DLF.c"
    "src/code/z_draw.c"
    "src/code/z_eff_blure.c"
    "src/code/z_eff_shield_particle.c"
    "src/code/z_eff_spark.c"
    "src/code/z_eff_ss_dead.c"
    "src/code/z_effect.c"
    "src/code/z_effect_soft_sprite.c"
    "src/code/z_effect_soft_sprite_dlftbls.c"
    "src/code/z_effect_soft_sprite_old_init.c"
    "src/code/z_elf_message.c"
    "src/code/z_en_a_keep.c"
    "src/code/z_en_item00.c"
    "src/code/z_face_reaction.c"
    "src/code/z_fbdemo.c"
    "src/code/z_fbdemo_circle.c"
    "src/code/z_fbdemo_fade.c"
    "src/code/z_fbdemo_triforce.c"
    "src/code/z_fbdemo_wipe1.c"
    "src/code/z_fcurve_data_skelanime.c"
    "src/code/z_frame_advance.c"
    "src/code/z_game_dlftbls.c"
    "src/code/z_game_over.c"
    "src/code/z_horse.c"
    "src/code/z_jpeg.c"
    "src/code/z_kaleido_manager.c"
    "src/code/z_kaleido_scope_call.c"
    "src/code/z_kaleido_setup.c"
    "src/code/z_kanfont.c"
    "src/code/z_kankyo.c"
    "src/code/z_lib.c"
    "src/code/z_lifemeter.c"
    "src/code/z_lights.c"
    "src/code/z_malloc.c"
    "src/code/z_map_data.c"
    "src/code/z_map_exp.c"
    "src/code/z_map_mark.c"
    "src/code/z_message_PAL.c"
    "src/code/z_moji.c"
    "src/code/z_msgevent.c"
    "src/code/z_olib.c"
    "src/code/z_onepointdemo.c"
    "src/code/z_onepointdemo_data.inc"
    "src/code/z_parameter.c"
    "src/code/z_path.c"
    "src/code/z_play.c"
    "src/code/z_player_call.c"
    "src/code/z_player_lib.c"
    "src/code/z_prenmi.c"
    "src/code/z_prenmi_buff.c"
    "src/code/z_quake.c"
    "src/code/z_rcp.c"
    "src/code/z_room.c"
    "src/code/z_sample.c"
    "src/code/z_scene.c"
    "src/code/z_scene_table.c"
    "src/code/z_skelanime.c"
    "src/code/z_skin.c"
    "src/code/z_skin_awb.c"
    "src/code/z_skin_matrix.c"
    "src/code/z_sound_source.c"
    "src/code/z_sram.c"
    "src/code/z_ss_sram.c"
    "src/code/z_view.c"
    "src/code/z_vimode.c"
    "src/code/z_vismono.c"
    "src/code/z_vr_box.c"
    "src/code/z_vr_box_draw.c"
)
source_group("Source Files\\src\\code" FILES ${Source_Files__src__code})

set(Source_Files__src__libultra
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
source_group("Source Files\\src\\libultra" FILES ${Source_Files__src__libultra})

set(Source_Files__src__overlays__actors
    "src/overlays/actors/ovl_Arms_Hook/z_arms_hook.c"
    "src/overlays/actors/ovl_Arms_Hook/z_arms_hook.h"
    "src/overlays/actors/ovl_Arrow_Fire/z_arrow_fire.c"
    "src/overlays/actors/ovl_Arrow_Fire/z_arrow_fire.h"
    "src/overlays/actors/ovl_Arrow_Ice/z_arrow_ice.c"
    "src/overlays/actors/ovl_Arrow_Ice/z_arrow_ice.h"
    "src/overlays/actors/ovl_Arrow_Light/z_arrow_light.c"
    "src/overlays/actors/ovl_Arrow_Light/z_arrow_light.h"
    "src/overlays/actors/ovl_Bg_Bdan_Objects/z_bg_bdan_objects.c"
    "src/overlays/actors/ovl_Bg_Bdan_Objects/z_bg_bdan_objects.h"
    "src/overlays/actors/ovl_Bg_Bdan_Switch/z_bg_bdan_switch.c"
    "src/overlays/actors/ovl_Bg_Bdan_Switch/z_bg_bdan_switch.h"
    "src/overlays/actors/ovl_Bg_Bom_Guard/z_bg_bom_guard.c"
    "src/overlays/actors/ovl_Bg_Bom_Guard/z_bg_bom_guard.h"
    "src/overlays/actors/ovl_Bg_Bombwall/z_bg_bombwall.c"
    "src/overlays/actors/ovl_Bg_Bombwall/z_bg_bombwall.h"
    "src/overlays/actors/ovl_Bg_Bowl_Wall/z_bg_bowl_wall.c"
    "src/overlays/actors/ovl_Bg_Bowl_Wall/z_bg_bowl_wall.h"
    "src/overlays/actors/ovl_Bg_Breakwall/z_bg_breakwall.c"
    "src/overlays/actors/ovl_Bg_Breakwall/z_bg_breakwall.h"
    "src/overlays/actors/ovl_Bg_Ddan_Jd/z_bg_ddan_jd.c"
    "src/overlays/actors/ovl_Bg_Ddan_Jd/z_bg_ddan_jd.h"
    "src/overlays/actors/ovl_Bg_Ddan_Kd/z_bg_ddan_kd.c"
    "src/overlays/actors/ovl_Bg_Ddan_Kd/z_bg_ddan_kd.h"
    "src/overlays/actors/ovl_Bg_Dodoago/z_bg_dodoago.c"
    "src/overlays/actors/ovl_Bg_Dodoago/z_bg_dodoago.h"
    "src/overlays/actors/ovl_Bg_Dy_Yoseizo/z_bg_dy_yoseizo.c"
    "src/overlays/actors/ovl_Bg_Dy_Yoseizo/z_bg_dy_yoseizo.h"
    "src/overlays/actors/ovl_Bg_Ganon_Otyuka/z_bg_ganon_otyuka.c"
    "src/overlays/actors/ovl_Bg_Ganon_Otyuka/z_bg_ganon_otyuka.h"
    "src/overlays/actors/ovl_Bg_Gate_Shutter/z_bg_gate_shutter.c"
    "src/overlays/actors/ovl_Bg_Gate_Shutter/z_bg_gate_shutter.h"
    "src/overlays/actors/ovl_Bg_Gjyo_Bridge/z_bg_gjyo_bridge.c"
    "src/overlays/actors/ovl_Bg_Gjyo_Bridge/z_bg_gjyo_bridge.h"
    "src/overlays/actors/ovl_Bg_Gnd_Darkmeiro/z_bg_gnd_darkmeiro.c"
    "src/overlays/actors/ovl_Bg_Gnd_Darkmeiro/z_bg_gnd_darkmeiro.h"
    "src/overlays/actors/ovl_Bg_Gnd_Firemeiro/z_bg_gnd_firemeiro.c"
    "src/overlays/actors/ovl_Bg_Gnd_Firemeiro/z_bg_gnd_firemeiro.h"
    "src/overlays/actors/ovl_Bg_Gnd_Iceblock/z_bg_gnd_iceblock.c"
    "src/overlays/actors/ovl_Bg_Gnd_Iceblock/z_bg_gnd_iceblock.h"
    "src/overlays/actors/ovl_Bg_Gnd_Nisekabe/z_bg_gnd_nisekabe.c"
    "src/overlays/actors/ovl_Bg_Gnd_Nisekabe/z_bg_gnd_nisekabe.h"
    "src/overlays/actors/ovl_Bg_Gnd_Soulmeiro/z_bg_gnd_soulmeiro.c"
    "src/overlays/actors/ovl_Bg_Gnd_Soulmeiro/z_bg_gnd_soulmeiro.h"
    "src/overlays/actors/ovl_Bg_Haka/z_bg_haka.c"
    "src/overlays/actors/ovl_Bg_Haka/z_bg_haka.h"
    "src/overlays/actors/ovl_Bg_Haka_Gate/z_bg_haka_gate.c"
    "src/overlays/actors/ovl_Bg_Haka_Gate/z_bg_haka_gate.h"
    "src/overlays/actors/ovl_Bg_Haka_Huta/z_bg_haka_huta.c"
    "src/overlays/actors/ovl_Bg_Haka_Huta/z_bg_haka_huta.h"
    "src/overlays/actors/ovl_Bg_Haka_Megane/z_bg_haka_megane.c"
    "src/overlays/actors/ovl_Bg_Haka_Megane/z_bg_haka_megane.h"
    "src/overlays/actors/ovl_Bg_Haka_MeganeBG/z_bg_haka_meganebg.c"
    "src/overlays/actors/ovl_Bg_Haka_MeganeBG/z_bg_haka_meganebg.h"
    "src/overlays/actors/ovl_Bg_Haka_Sgami/z_bg_haka_sgami.c"
    "src/overlays/actors/ovl_Bg_Haka_Sgami/z_bg_haka_sgami.h"
    "src/overlays/actors/ovl_Bg_Haka_Ship/z_bg_haka_ship.c"
    "src/overlays/actors/ovl_Bg_Haka_Ship/z_bg_haka_ship.h"
    "src/overlays/actors/ovl_Bg_Haka_Trap/z_bg_haka_trap.c"
    "src/overlays/actors/ovl_Bg_Haka_Trap/z_bg_haka_trap.h"
    "src/overlays/actors/ovl_Bg_Haka_Tubo/z_bg_haka_tubo.c"
    "src/overlays/actors/ovl_Bg_Haka_Tubo/z_bg_haka_tubo.h"
    "src/overlays/actors/ovl_Bg_Haka_Water/z_bg_haka_water.c"
    "src/overlays/actors/ovl_Bg_Haka_Water/z_bg_haka_water.h"
    "src/overlays/actors/ovl_Bg_Haka_Zou/z_bg_haka_zou.c"
    "src/overlays/actors/ovl_Bg_Haka_Zou/z_bg_haka_zou.h"
    "src/overlays/actors/ovl_Bg_Heavy_Block/z_bg_heavy_block.c"
    "src/overlays/actors/ovl_Bg_Heavy_Block/z_bg_heavy_block.h"
    "src/overlays/actors/ovl_Bg_Hidan_Curtain/z_bg_hidan_curtain.c"
    "src/overlays/actors/ovl_Bg_Hidan_Curtain/z_bg_hidan_curtain.h"
    "src/overlays/actors/ovl_Bg_Hidan_Dalm/z_bg_hidan_dalm.c"
    "src/overlays/actors/ovl_Bg_Hidan_Dalm/z_bg_hidan_dalm.h"
    "src/overlays/actors/ovl_Bg_Hidan_Firewall/z_bg_hidan_firewall.c"
    "src/overlays/actors/ovl_Bg_Hidan_Firewall/z_bg_hidan_firewall.h"
    "src/overlays/actors/ovl_Bg_Hidan_Fslift/z_bg_hidan_fslift.c"
    "src/overlays/actors/ovl_Bg_Hidan_Fslift/z_bg_hidan_fslift.h"
    "src/overlays/actors/ovl_Bg_Hidan_Fwbig/z_bg_hidan_fwbig.c"
    "src/overlays/actors/ovl_Bg_Hidan_Fwbig/z_bg_hidan_fwbig.h"
    "src/overlays/actors/ovl_Bg_Hidan_Hamstep/z_bg_hidan_hamstep.c"
    "src/overlays/actors/ovl_Bg_Hidan_Hamstep/z_bg_hidan_hamstep.h"
    "src/overlays/actors/ovl_Bg_Hidan_Hrock/z_bg_hidan_hrock.c"
    "src/overlays/actors/ovl_Bg_Hidan_Hrock/z_bg_hidan_hrock.h"
    "src/overlays/actors/ovl_Bg_Hidan_Kousi/z_bg_hidan_kousi.c"
    "src/overlays/actors/ovl_Bg_Hidan_Kousi/z_bg_hidan_kousi.h"
    "src/overlays/actors/ovl_Bg_Hidan_Kowarerukabe/z_bg_hidan_kowarerukabe.c"
    "src/overlays/actors/ovl_Bg_Hidan_Kowarerukabe/z_bg_hidan_kowarerukabe.h"
    "src/overlays/actors/ovl_Bg_Hidan_Rock/z_bg_hidan_rock.c"
    "src/overlays/actors/ovl_Bg_Hidan_Rock/z_bg_hidan_rock.h"
    "src/overlays/actors/ovl_Bg_Hidan_Rsekizou/z_bg_hidan_rsekizou.c"
    "src/overlays/actors/ovl_Bg_Hidan_Rsekizou/z_bg_hidan_rsekizou.h"
    "src/overlays/actors/ovl_Bg_Hidan_Sekizou/z_bg_hidan_sekizou.c"
    "src/overlays/actors/ovl_Bg_Hidan_Sekizou/z_bg_hidan_sekizou.h"
    "src/overlays/actors/ovl_Bg_Hidan_Sima/z_bg_hidan_sima.c"
    "src/overlays/actors/ovl_Bg_Hidan_Sima/z_bg_hidan_sima.h"
    "src/overlays/actors/ovl_Bg_Hidan_Syoku/z_bg_hidan_syoku.c"
    "src/overlays/actors/ovl_Bg_Hidan_Syoku/z_bg_hidan_syoku.h"
    "src/overlays/actors/ovl_Bg_Ice_Objects/z_bg_ice_objects.c"
    "src/overlays/actors/ovl_Bg_Ice_Objects/z_bg_ice_objects.h"
    "src/overlays/actors/ovl_Bg_Ice_Shelter/z_bg_ice_shelter.c"
    "src/overlays/actors/ovl_Bg_Ice_Shelter/z_bg_ice_shelter.h"
    "src/overlays/actors/ovl_Bg_Ice_Shutter/z_bg_ice_shutter.c"
    "src/overlays/actors/ovl_Bg_Ice_Shutter/z_bg_ice_shutter.h"
    "src/overlays/actors/ovl_Bg_Ice_Turara/z_bg_ice_turara.c"
    "src/overlays/actors/ovl_Bg_Ice_Turara/z_bg_ice_turara.h"
    "src/overlays/actors/ovl_Bg_Ingate/z_bg_ingate.c"
    "src/overlays/actors/ovl_Bg_Ingate/z_bg_ingate.h"
    "src/overlays/actors/ovl_Bg_Jya_1flift/z_bg_jya_1flift.c"
    "src/overlays/actors/ovl_Bg_Jya_1flift/z_bg_jya_1flift.h"
    "src/overlays/actors/ovl_Bg_Jya_Amishutter/z_bg_jya_amishutter.c"
    "src/overlays/actors/ovl_Bg_Jya_Amishutter/z_bg_jya_amishutter.h"
    "src/overlays/actors/ovl_Bg_Jya_Bigmirror/z_bg_jya_bigmirror.c"
    "src/overlays/actors/ovl_Bg_Jya_Bigmirror/z_bg_jya_bigmirror.h"
    "src/overlays/actors/ovl_Bg_Jya_Block/z_bg_jya_block.c"
    "src/overlays/actors/ovl_Bg_Jya_Block/z_bg_jya_block.h"
    "src/overlays/actors/ovl_Bg_Jya_Bombchuiwa/z_bg_jya_bombchuiwa.c"
    "src/overlays/actors/ovl_Bg_Jya_Bombchuiwa/z_bg_jya_bombchuiwa.h"
    "src/overlays/actors/ovl_Bg_Jya_Bombiwa/z_bg_jya_bombiwa.c"
    "src/overlays/actors/ovl_Bg_Jya_Bombiwa/z_bg_jya_bombiwa.h"
    "src/overlays/actors/ovl_Bg_Jya_Cobra/z_bg_jya_cobra.c"
    "src/overlays/actors/ovl_Bg_Jya_Cobra/z_bg_jya_cobra.h"
    "src/overlays/actors/ovl_Bg_Jya_Goroiwa/z_bg_jya_goroiwa.c"
    "src/overlays/actors/ovl_Bg_Jya_Goroiwa/z_bg_jya_goroiwa.h"
    "src/overlays/actors/ovl_Bg_Jya_Haheniron/z_bg_jya_haheniron.c"
    "src/overlays/actors/ovl_Bg_Jya_Haheniron/z_bg_jya_haheniron.h"
    "src/overlays/actors/ovl_Bg_Jya_Ironobj/z_bg_jya_ironobj.c"
    "src/overlays/actors/ovl_Bg_Jya_Ironobj/z_bg_jya_ironobj.h"
    "src/overlays/actors/ovl_Bg_Jya_Kanaami/z_bg_jya_kanaami.c"
    "src/overlays/actors/ovl_Bg_Jya_Kanaami/z_bg_jya_kanaami.h"
    "src/overlays/actors/ovl_Bg_Jya_Lift/z_bg_jya_lift.c"
    "src/overlays/actors/ovl_Bg_Jya_Lift/z_bg_jya_lift.h"
    "src/overlays/actors/ovl_Bg_Jya_Megami/z_bg_jya_megami.c"
    "src/overlays/actors/ovl_Bg_Jya_Megami/z_bg_jya_megami.h"
    "src/overlays/actors/ovl_Bg_Jya_Zurerukabe/z_bg_jya_zurerukabe.c"
    "src/overlays/actors/ovl_Bg_Jya_Zurerukabe/z_bg_jya_zurerukabe.h"
    "src/overlays/actors/ovl_Bg_Menkuri_Eye/z_bg_menkuri_eye.c"
    "src/overlays/actors/ovl_Bg_Menkuri_Eye/z_bg_menkuri_eye.h"
    "src/overlays/actors/ovl_Bg_Menkuri_Kaiten/z_bg_menkuri_kaiten.c"
    "src/overlays/actors/ovl_Bg_Menkuri_Kaiten/z_bg_menkuri_kaiten.h"
    "src/overlays/actors/ovl_Bg_Menkuri_Nisekabe/z_bg_menkuri_nisekabe.c"
    "src/overlays/actors/ovl_Bg_Menkuri_Nisekabe/z_bg_menkuri_nisekabe.h"
    "src/overlays/actors/ovl_Bg_Mizu_Bwall/z_bg_mizu_bwall.c"
    "src/overlays/actors/ovl_Bg_Mizu_Bwall/z_bg_mizu_bwall.h"
    "src/overlays/actors/ovl_Bg_Mizu_Movebg/z_bg_mizu_movebg.c"
    "src/overlays/actors/ovl_Bg_Mizu_Movebg/z_bg_mizu_movebg.h"
    "src/overlays/actors/ovl_Bg_Mizu_Shutter/z_bg_mizu_shutter.c"
    "src/overlays/actors/ovl_Bg_Mizu_Shutter/z_bg_mizu_shutter.h"
    "src/overlays/actors/ovl_Bg_Mizu_Uzu/z_bg_mizu_uzu.c"
    "src/overlays/actors/ovl_Bg_Mizu_Uzu/z_bg_mizu_uzu.h"
    "src/overlays/actors/ovl_Bg_Mizu_Water/z_bg_mizu_water.c"
    "src/overlays/actors/ovl_Bg_Mizu_Water/z_bg_mizu_water.h"
    "src/overlays/actors/ovl_Bg_Mjin/z_bg_mjin.c"
    "src/overlays/actors/ovl_Bg_Mjin/z_bg_mjin.h"
    "src/overlays/actors/ovl_Bg_Mori_Bigst/z_bg_mori_bigst.c"
    "src/overlays/actors/ovl_Bg_Mori_Bigst/z_bg_mori_bigst.h"
    "src/overlays/actors/ovl_Bg_Mori_Elevator/z_bg_mori_elevator.c"
    "src/overlays/actors/ovl_Bg_Mori_Elevator/z_bg_mori_elevator.h"
    "src/overlays/actors/ovl_Bg_Mori_Hashigo/z_bg_mori_hashigo.c"
    "src/overlays/actors/ovl_Bg_Mori_Hashigo/z_bg_mori_hashigo.h"
    "src/overlays/actors/ovl_Bg_Mori_Hashira4/z_bg_mori_hashira4.c"
    "src/overlays/actors/ovl_Bg_Mori_Hashira4/z_bg_mori_hashira4.h"
    "src/overlays/actors/ovl_Bg_Mori_Hineri/z_bg_mori_hineri.c"
    "src/overlays/actors/ovl_Bg_Mori_Hineri/z_bg_mori_hineri.h"
    "src/overlays/actors/ovl_Bg_Mori_Idomizu/z_bg_mori_idomizu.c"
    "src/overlays/actors/ovl_Bg_Mori_Idomizu/z_bg_mori_idomizu.h"
    "src/overlays/actors/ovl_Bg_Mori_Kaitenkabe/z_bg_mori_kaitenkabe.c"
    "src/overlays/actors/ovl_Bg_Mori_Kaitenkabe/z_bg_mori_kaitenkabe.h"
    "src/overlays/actors/ovl_Bg_Mori_Rakkatenjo/z_bg_mori_rakkatenjo.c"
    "src/overlays/actors/ovl_Bg_Mori_Rakkatenjo/z_bg_mori_rakkatenjo.h"
    "src/overlays/actors/ovl_Bg_Po_Event/z_bg_po_event.c"
    "src/overlays/actors/ovl_Bg_Po_Event/z_bg_po_event.h"
    "src/overlays/actors/ovl_Bg_Po_Syokudai/z_bg_po_syokudai.c"
    "src/overlays/actors/ovl_Bg_Po_Syokudai/z_bg_po_syokudai.h"
    "src/overlays/actors/ovl_Bg_Pushbox/z_bg_pushbox.c"
    "src/overlays/actors/ovl_Bg_Pushbox/z_bg_pushbox.h"
    "src/overlays/actors/ovl_Bg_Relay_Objects/z_bg_relay_objects.c"
    "src/overlays/actors/ovl_Bg_Relay_Objects/z_bg_relay_objects.h"
    "src/overlays/actors/ovl_Bg_Spot00_Break/z_bg_spot00_break.c"
    "src/overlays/actors/ovl_Bg_Spot00_Break/z_bg_spot00_break.h"
    "src/overlays/actors/ovl_Bg_Spot00_Hanebasi/z_bg_spot00_hanebasi.c"
    "src/overlays/actors/ovl_Bg_Spot00_Hanebasi/z_bg_spot00_hanebasi.h"
    "src/overlays/actors/ovl_Bg_Spot01_Fusya/z_bg_spot01_fusya.c"
    "src/overlays/actors/ovl_Bg_Spot01_Fusya/z_bg_spot01_fusya.h"
    "src/overlays/actors/ovl_Bg_Spot01_Idohashira/z_bg_spot01_idohashira.c"
    "src/overlays/actors/ovl_Bg_Spot01_Idohashira/z_bg_spot01_idohashira.h"
    "src/overlays/actors/ovl_Bg_Spot01_Idomizu/z_bg_spot01_idomizu.c"
    "src/overlays/actors/ovl_Bg_Spot01_Idomizu/z_bg_spot01_idomizu.h"
    "src/overlays/actors/ovl_Bg_Spot01_Idosoko/z_bg_spot01_idosoko.c"
    "src/overlays/actors/ovl_Bg_Spot01_Idosoko/z_bg_spot01_idosoko.h"
    "src/overlays/actors/ovl_Bg_Spot01_Objects2/z_bg_spot01_objects2.c"
    "src/overlays/actors/ovl_Bg_Spot01_Objects2/z_bg_spot01_objects2.h"
    "src/overlays/actors/ovl_Bg_Spot02_Objects/z_bg_spot02_objects.c"
    "src/overlays/actors/ovl_Bg_Spot02_Objects/z_bg_spot02_objects.h"
    "src/overlays/actors/ovl_Bg_Spot03_Taki/z_bg_spot03_taki.c"
    "src/overlays/actors/ovl_Bg_Spot03_Taki/z_bg_spot03_taki.h"
    "src/overlays/actors/ovl_Bg_Spot05_Soko/z_bg_spot05_soko.c"
    "src/overlays/actors/ovl_Bg_Spot05_Soko/z_bg_spot05_soko.h"
    "src/overlays/actors/ovl_Bg_Spot06_Objects/z_bg_spot06_objects.c"
    "src/overlays/actors/ovl_Bg_Spot06_Objects/z_bg_spot06_objects.h"
    "src/overlays/actors/ovl_Bg_Spot07_Taki/z_bg_spot07_taki.c"
    "src/overlays/actors/ovl_Bg_Spot07_Taki/z_bg_spot07_taki.h"
    "src/overlays/actors/ovl_Bg_Spot08_Bakudankabe/z_bg_spot08_bakudankabe.c"
    "src/overlays/actors/ovl_Bg_Spot08_Bakudankabe/z_bg_spot08_bakudankabe.h"
    "src/overlays/actors/ovl_Bg_Spot08_Iceblock/z_bg_spot08_iceblock.c"
    "src/overlays/actors/ovl_Bg_Spot08_Iceblock/z_bg_spot08_iceblock.h"
    "src/overlays/actors/ovl_Bg_Spot09_Obj/z_bg_spot09_obj.c"
    "src/overlays/actors/ovl_Bg_Spot09_Obj/z_bg_spot09_obj.h"
    "src/overlays/actors/ovl_Bg_Spot11_Bakudankabe/z_bg_spot11_bakudankabe.c"
    "src/overlays/actors/ovl_Bg_Spot11_Bakudankabe/z_bg_spot11_bakudankabe.h"
    "src/overlays/actors/ovl_Bg_Spot11_Oasis/z_bg_spot11_oasis.c"
    "src/overlays/actors/ovl_Bg_Spot11_Oasis/z_bg_spot11_oasis.h"
    "src/overlays/actors/ovl_Bg_Spot12_Gate/z_bg_spot12_gate.c"
    "src/overlays/actors/ovl_Bg_Spot12_Gate/z_bg_spot12_gate.h"
    "src/overlays/actors/ovl_Bg_Spot12_Saku/z_bg_spot12_saku.c"
    "src/overlays/actors/ovl_Bg_Spot12_Saku/z_bg_spot12_saku.h"
    "src/overlays/actors/ovl_Bg_Spot15_Rrbox/z_bg_spot15_rrbox.c"
    "src/overlays/actors/ovl_Bg_Spot15_Rrbox/z_bg_spot15_rrbox.h"
    "src/overlays/actors/ovl_Bg_Spot15_Saku/z_bg_spot15_saku.c"
    "src/overlays/actors/ovl_Bg_Spot15_Saku/z_bg_spot15_saku.h"
    "src/overlays/actors/ovl_Bg_Spot16_Bombstone/z_bg_spot16_bombstone.c"
    "src/overlays/actors/ovl_Bg_Spot16_Bombstone/z_bg_spot16_bombstone.h"
    "src/overlays/actors/ovl_Bg_Spot16_Doughnut/z_bg_spot16_doughnut.c"
    "src/overlays/actors/ovl_Bg_Spot16_Doughnut/z_bg_spot16_doughnut.h"
    "src/overlays/actors/ovl_Bg_Spot17_Bakudankabe/z_bg_spot17_bakudankabe.c"
    "src/overlays/actors/ovl_Bg_Spot17_Bakudankabe/z_bg_spot17_bakudankabe.h"
    "src/overlays/actors/ovl_Bg_Spot17_Funen/z_bg_spot17_funen.c"
    "src/overlays/actors/ovl_Bg_Spot17_Funen/z_bg_spot17_funen.h"
    "src/overlays/actors/ovl_Bg_Spot18_Basket/z_bg_spot18_basket.c"
    "src/overlays/actors/ovl_Bg_Spot18_Basket/z_bg_spot18_basket.h"
    "src/overlays/actors/ovl_Bg_Spot18_Futa/z_bg_spot18_futa.c"
    "src/overlays/actors/ovl_Bg_Spot18_Futa/z_bg_spot18_futa.h"
    "src/overlays/actors/ovl_Bg_Spot18_Obj/z_bg_spot18_obj.c"
    "src/overlays/actors/ovl_Bg_Spot18_Obj/z_bg_spot18_obj.h"
    "src/overlays/actors/ovl_Bg_Spot18_Shutter/z_bg_spot18_shutter.c"
    "src/overlays/actors/ovl_Bg_Spot18_Shutter/z_bg_spot18_shutter.h"
    "src/overlays/actors/ovl_Bg_Sst_Floor/z_bg_sst_floor.c"
    "src/overlays/actors/ovl_Bg_Sst_Floor/z_bg_sst_floor.h"
    "src/overlays/actors/ovl_Bg_Toki_Hikari/z_bg_toki_hikari.c"
    "src/overlays/actors/ovl_Bg_Toki_Hikari/z_bg_toki_hikari.h"
    "src/overlays/actors/ovl_Bg_Toki_Swd/z_bg_toki_swd.c"
    "src/overlays/actors/ovl_Bg_Toki_Swd/z_bg_toki_swd.h"
    "src/overlays/actors/ovl_Bg_Toki_Swd/z_bg_toki_swd_cutscene_data_1.c"
    "src/overlays/actors/ovl_Bg_Toki_Swd/z_bg_toki_swd_cutscene_data_2.c"
    "src/overlays/actors/ovl_Bg_Toki_Swd/z_bg_toki_swd_cutscene_data_3.c"
    "src/overlays/actors/ovl_Bg_Treemouth/z_bg_treemouth.c"
    "src/overlays/actors/ovl_Bg_Treemouth/z_bg_treemouth.h"
    "src/overlays/actors/ovl_Bg_Treemouth/z_bg_treemouth_cutscene_data.c"
    "src/overlays/actors/ovl_Bg_Umajump/z_bg_umajump.c"
    "src/overlays/actors/ovl_Bg_Umajump/z_bg_umajump.h"
    "src/overlays/actors/ovl_Bg_Vb_Sima/z_bg_vb_sima.c"
    "src/overlays/actors/ovl_Bg_Vb_Sima/z_bg_vb_sima.h"
    "src/overlays/actors/ovl_Bg_Ydan_Hasi/z_bg_ydan_hasi.c"
    "src/overlays/actors/ovl_Bg_Ydan_Hasi/z_bg_ydan_hasi.h"
    "src/overlays/actors/ovl_Bg_Ydan_Maruta/z_bg_ydan_maruta.c"
    "src/overlays/actors/ovl_Bg_Ydan_Maruta/z_bg_ydan_maruta.h"
    "src/overlays/actors/ovl_Bg_Ydan_Sp/z_bg_ydan_sp.c"
    "src/overlays/actors/ovl_Bg_Ydan_Sp/z_bg_ydan_sp.h"
    "src/overlays/actors/ovl_Bg_Zg/z_bg_zg.c"
    "src/overlays/actors/ovl_Bg_Zg/z_bg_zg.h"
    "src/overlays/actors/ovl_Boss_Dodongo/z_boss_dodongo.c"
    "src/overlays/actors/ovl_Boss_Dodongo/z_boss_dodongo.h"
    "src/overlays/actors/ovl_Boss_Dodongo/z_boss_dodongo_data.c"
    "src/overlays/actors/ovl_Boss_Fd/z_boss_fd.c"
    "src/overlays/actors/ovl_Boss_Fd/z_boss_fd.h"
    "src/overlays/actors/ovl_Boss_Fd/z_boss_fd_colchk.c"
    "src/overlays/actors/ovl_Boss_Fd2/z_boss_fd2.c"
    "src/overlays/actors/ovl_Boss_Fd2/z_boss_fd2.h"
    "src/overlays/actors/ovl_Boss_Fd2/z_boss_fd2_colchk.c"
    "src/overlays/actors/ovl_Boss_Ganon/z_boss_ganon.c"
    "src/overlays/actors/ovl_Boss_Ganon/z_boss_ganon.h"
    "src/overlays/actors/ovl_Boss_Ganon2/z_boss_ganon2.c"
    "src/overlays/actors/ovl_Boss_Ganon2/z_boss_ganon2.h"
    "src/overlays/actors/ovl_Boss_Ganon2/z_boss_ganon2_data.c"
    "src/overlays/actors/ovl_Boss_Ganondrof/z_boss_ganondrof.c"
    "src/overlays/actors/ovl_Boss_Ganondrof/z_boss_ganondrof.h"
    "src/overlays/actors/ovl_Boss_Goma/z_boss_goma.c"
    "src/overlays/actors/ovl_Boss_Goma/z_boss_goma.h"
    "src/overlays/actors/ovl_Boss_Mo/z_boss_mo.c"
    "src/overlays/actors/ovl_Boss_Mo/z_boss_mo.h"
    "src/overlays/actors/ovl_Boss_Mo/z_boss_mo_colchk.c"
    "src/overlays/actors/ovl_Boss_Sst/z_boss_sst.c"
    "src/overlays/actors/ovl_Boss_Sst/z_boss_sst.h"
    "src/overlays/actors/ovl_Boss_Sst/z_boss_sst_colchk.c"
    "src/overlays/actors/ovl_Boss_Tw/z_boss_tw.c"
    "src/overlays/actors/ovl_Boss_Tw/z_boss_tw.h"
    "src/overlays/actors/ovl_Boss_Va/z_boss_va.c"
    "src/overlays/actors/ovl_Boss_Va/z_boss_va.h"
    "src/overlays/actors/ovl_Demo_6K/z_demo_6k.c"
    "src/overlays/actors/ovl_Demo_6K/z_demo_6k.h"
    "src/overlays/actors/ovl_Demo_Du/z_demo_du.c"
    "src/overlays/actors/ovl_Demo_Du/z_demo_du.h"
    "src/overlays/actors/ovl_Demo_Du/z_demo_du_cutscene_data.c"
    "src/overlays/actors/ovl_Demo_Ec/z_demo_ec.c"
    "src/overlays/actors/ovl_Demo_Ec/z_demo_ec.h"
    "src/overlays/actors/ovl_Demo_Effect/z_demo_effect.c"
    "src/overlays/actors/ovl_Demo_Effect/z_demo_effect.h"
    "src/overlays/actors/ovl_Demo_Ext/z_demo_ext.c"
    "src/overlays/actors/ovl_Demo_Ext/z_demo_ext.h"
    "src/overlays/actors/ovl_Demo_Geff/z_demo_geff.c"
    "src/overlays/actors/ovl_Demo_Geff/z_demo_geff.h"
    "src/overlays/actors/ovl_Demo_Gj/z_demo_gj.c"
    "src/overlays/actors/ovl_Demo_Gj/z_demo_gj.h"
    "src/overlays/actors/ovl_Demo_Go/z_demo_go.c"
    "src/overlays/actors/ovl_Demo_Go/z_demo_go.h"
    "src/overlays/actors/ovl_Demo_Gt/z_demo_gt.c"
    "src/overlays/actors/ovl_Demo_Gt/z_demo_gt.h"
    "src/overlays/actors/ovl_Demo_Ik/z_demo_ik.c"
    "src/overlays/actors/ovl_Demo_Ik/z_demo_ik.h"
    "src/overlays/actors/ovl_Demo_Im/z_demo_im.c"
    "src/overlays/actors/ovl_Demo_Im/z_demo_im.h"
    "src/overlays/actors/ovl_Demo_Im/z_demo_im_cutscene_data.c"
    "src/overlays/actors/ovl_Demo_Kankyo/z_demo_kankyo.c"
    "src/overlays/actors/ovl_Demo_Kankyo/z_demo_kankyo.h"
    "src/overlays/actors/ovl_Demo_Kankyo/z_demo_kankyo_cutscene_data1.c"
    "src/overlays/actors/ovl_Demo_Kankyo/z_demo_kankyo_cutscene_data2.c"
    "src/overlays/actors/ovl_Demo_Kankyo/z_demo_kankyo_cutscene_data3.c"
    "src/overlays/actors/ovl_Demo_Kankyo/z_demo_kankyo_cutscene_data4.c"
    "src/overlays/actors/ovl_Demo_Kankyo/z_demo_kankyo_cutscene_data5.c"
    "src/overlays/actors/ovl_Demo_Kankyo/z_demo_kankyo_cutscene_data6.c"
    "src/overlays/actors/ovl_Demo_Kankyo/z_demo_kankyo_cutscene_data7.c"
    "src/overlays/actors/ovl_Demo_Kankyo/z_demo_kankyo_cutscene_data8.c"
    "src/overlays/actors/ovl_Demo_Kekkai/z_demo_kekkai.c"
    "src/overlays/actors/ovl_Demo_Kekkai/z_demo_kekkai.h"
    "src/overlays/actors/ovl_Demo_Sa/z_demo_sa.c"
    "src/overlays/actors/ovl_Demo_Sa/z_demo_sa.h"
    "src/overlays/actors/ovl_Demo_Sa/z_demo_sa_cutscene_data.c"
    "src/overlays/actors/ovl_Demo_Shd/z_demo_shd.c"
    "src/overlays/actors/ovl_Demo_Shd/z_demo_shd.h"
    "src/overlays/actors/ovl_Demo_Tre_Lgt/z_demo_tre_lgt.c"
    "src/overlays/actors/ovl_Demo_Tre_Lgt/z_demo_tre_lgt.h"
    "src/overlays/actors/ovl_Door_Ana/z_door_ana.c"
    "src/overlays/actors/ovl_Door_Ana/z_door_ana.h"
    "src/overlays/actors/ovl_Door_Gerudo/z_door_gerudo.c"
    "src/overlays/actors/ovl_Door_Gerudo/z_door_gerudo.h"
    "src/overlays/actors/ovl_Door_Killer/z_door_killer.c"
    "src/overlays/actors/ovl_Door_Killer/z_door_killer.h"
    "src/overlays/actors/ovl_Door_Shutter/z_door_shutter.c"
    "src/overlays/actors/ovl_Door_Shutter/z_door_shutter.h"
    "src/overlays/actors/ovl_Door_Toki/z_door_toki.c"
    "src/overlays/actors/ovl_Door_Toki/z_door_toki.h"
    "src/overlays/actors/ovl_Door_Warp1/z_door_warp1.c"
    "src/overlays/actors/ovl_Door_Warp1/z_door_warp1.h"
    "src/overlays/actors/ovl_Efc_Erupc/z_efc_erupc.c"
    "src/overlays/actors/ovl_Efc_Erupc/z_efc_erupc.h"
    "src/overlays/actors/ovl_Eff_Dust/z_eff_dust.c"
    "src/overlays/actors/ovl_Eff_Dust/z_eff_dust.h"
    "src/overlays/actors/ovl_Elf_Msg/z_elf_msg.c"
    "src/overlays/actors/ovl_Elf_Msg/z_elf_msg.h"
    "src/overlays/actors/ovl_Elf_Msg2/z_elf_msg2.c"
    "src/overlays/actors/ovl_Elf_Msg2/z_elf_msg2.h"
    "src/overlays/actors/ovl_En_Am/z_en_am.c"
    "src/overlays/actors/ovl_En_Am/z_en_am.h"
    "src/overlays/actors/ovl_En_Ani/z_en_ani.c"
    "src/overlays/actors/ovl_En_Ani/z_en_ani.h"
    "src/overlays/actors/ovl_En_Anubice/z_en_anubice.c"
    "src/overlays/actors/ovl_En_Anubice/z_en_anubice.h"
    "src/overlays/actors/ovl_En_Anubice_Fire/z_en_anubice_fire.c"
    "src/overlays/actors/ovl_En_Anubice_Fire/z_en_anubice_fire.h"
    "src/overlays/actors/ovl_En_Anubice_Tag/z_en_anubice_tag.c"
    "src/overlays/actors/ovl_En_Anubice_Tag/z_en_anubice_tag.h"
    "src/overlays/actors/ovl_En_Arow_Trap/z_en_arow_trap.c"
    "src/overlays/actors/ovl_En_Arow_Trap/z_en_arow_trap.h"
    "src/overlays/actors/ovl_En_Arrow/z_en_arrow.c"
    "src/overlays/actors/ovl_En_Arrow/z_en_arrow.h"
    "src/overlays/actors/ovl_En_Attack_Niw/z_en_attack_niw.c"
    "src/overlays/actors/ovl_En_Attack_Niw/z_en_attack_niw.h"
    "src/overlays/actors/ovl_En_Ba/z_en_ba.c"
    "src/overlays/actors/ovl_En_Ba/z_en_ba.h"
    "src/overlays/actors/ovl_En_Bb/z_en_bb.c"
    "src/overlays/actors/ovl_En_Bb/z_en_bb.h"
    "src/overlays/actors/ovl_En_Bdfire/z_en_bdfire.c"
    "src/overlays/actors/ovl_En_Bdfire/z_en_bdfire.h"
    "src/overlays/actors/ovl_En_Bigokuta/z_en_bigokuta.c"
    "src/overlays/actors/ovl_En_Bigokuta/z_en_bigokuta.h"
    "src/overlays/actors/ovl_En_Bili/z_en_bili.c"
    "src/overlays/actors/ovl_En_Bili/z_en_bili.h"
    "src/overlays/actors/ovl_En_Bird/z_en_bird.c"
    "src/overlays/actors/ovl_En_Bird/z_en_bird.h"
    "src/overlays/actors/ovl_En_Blkobj/z_en_blkobj.c"
    "src/overlays/actors/ovl_En_Blkobj/z_en_blkobj.h"
    "src/overlays/actors/ovl_En_Bom/z_en_bom.c"
    "src/overlays/actors/ovl_En_Bom/z_en_bom.h"
    "src/overlays/actors/ovl_En_Bom_Bowl_Man/z_en_bom_bowl_man.c"
    "src/overlays/actors/ovl_En_Bom_Bowl_Man/z_en_bom_bowl_man.h"
    "src/overlays/actors/ovl_En_Bom_Bowl_Pit/z_en_bom_bowl_pit.c"
    "src/overlays/actors/ovl_En_Bom_Bowl_Pit/z_en_bom_bowl_pit.h"
    "src/overlays/actors/ovl_En_Bom_Chu/z_en_bom_chu.c"
    "src/overlays/actors/ovl_En_Bom_Chu/z_en_bom_chu.h"
    "src/overlays/actors/ovl_En_Bombf/z_en_bombf.c"
    "src/overlays/actors/ovl_En_Bombf/z_en_bombf.h"
    "src/overlays/actors/ovl_En_Boom/z_en_boom.c"
    "src/overlays/actors/ovl_En_Boom/z_en_boom.h"
    "src/overlays/actors/ovl_En_Box/z_en_box.c"
    "src/overlays/actors/ovl_En_Box/z_en_box.h"
    "src/overlays/actors/ovl_En_Brob/z_en_brob.c"
    "src/overlays/actors/ovl_En_Brob/z_en_brob.h"
    "src/overlays/actors/ovl_En_Bubble/z_en_bubble.c"
    "src/overlays/actors/ovl_En_Bubble/z_en_bubble.h"
    "src/overlays/actors/ovl_En_Butte/z_en_butte.c"
    "src/overlays/actors/ovl_En_Butte/z_en_butte.h"
    "src/overlays/actors/ovl_En_Bw/z_en_bw.c"
    "src/overlays/actors/ovl_En_Bw/z_en_bw.h"
    "src/overlays/actors/ovl_En_Bx/z_en_bx.c"
    "src/overlays/actors/ovl_En_Bx/z_en_bx.h"
    "src/overlays/actors/ovl_En_Changer/z_en_changer.c"
    "src/overlays/actors/ovl_En_Changer/z_en_changer.h"
    "src/overlays/actors/ovl_En_Clear_Tag/z_en_clear_tag.c"
    "src/overlays/actors/ovl_En_Clear_Tag/z_en_clear_tag.h"
    "src/overlays/actors/ovl_En_Cow/z_en_cow.c"
    "src/overlays/actors/ovl_En_Cow/z_en_cow.h"
    "src/overlays/actors/ovl_En_Crow/z_en_crow.c"
    "src/overlays/actors/ovl_En_Crow/z_en_crow.h"
    "src/overlays/actors/ovl_En_Cs/z_en_cs.c"
    "src/overlays/actors/ovl_En_Cs/z_en_cs.h"
    "src/overlays/actors/ovl_En_Daiku/z_en_daiku.c"
    "src/overlays/actors/ovl_En_Daiku/z_en_daiku.h"
    "src/overlays/actors/ovl_En_Daiku_Kakariko/z_en_daiku_kakariko.c"
    "src/overlays/actors/ovl_En_Daiku_Kakariko/z_en_daiku_kakariko.h"
    "src/overlays/actors/ovl_En_Dekubaba/z_en_dekubaba.c"
    "src/overlays/actors/ovl_En_Dekubaba/z_en_dekubaba.h"
    "src/overlays/actors/ovl_En_Dekunuts/z_en_dekunuts.c"
    "src/overlays/actors/ovl_En_Dekunuts/z_en_dekunuts.h"
    "src/overlays/actors/ovl_En_Dh/z_en_dh.c"
    "src/overlays/actors/ovl_En_Dh/z_en_dh.h"
    "src/overlays/actors/ovl_En_Dha/z_en_dha.c"
    "src/overlays/actors/ovl_En_Dha/z_en_dha.h"
    "src/overlays/actors/ovl_En_Diving_Game/z_en_diving_game.c"
    "src/overlays/actors/ovl_En_Diving_Game/z_en_diving_game.h"
    "src/overlays/actors/ovl_En_Dns/z_en_dns.c"
    "src/overlays/actors/ovl_En_Dns/z_en_dns.h"
    "src/overlays/actors/ovl_En_Dnt_Demo/z_en_dnt_demo.c"
    "src/overlays/actors/ovl_En_Dnt_Demo/z_en_dnt_demo.h"
    "src/overlays/actors/ovl_En_Dnt_Jiji/z_en_dnt_jiji.c"
    "src/overlays/actors/ovl_En_Dnt_Jiji/z_en_dnt_jiji.h"
    "src/overlays/actors/ovl_En_Dnt_Nomal/z_en_dnt_nomal.c"
    "src/overlays/actors/ovl_En_Dnt_Nomal/z_en_dnt_nomal.h"
    "src/overlays/actors/ovl_En_Dodojr/z_en_dodojr.c"
    "src/overlays/actors/ovl_En_Dodojr/z_en_dodojr.h"
    "src/overlays/actors/ovl_En_Dodongo/z_en_dodongo.c"
    "src/overlays/actors/ovl_En_Dodongo/z_en_dodongo.h"
    "src/overlays/actors/ovl_En_Dog/z_en_dog.c"
    "src/overlays/actors/ovl_En_Dog/z_en_dog.h"
    "src/overlays/actors/ovl_En_Door/z_en_door.c"
    "src/overlays/actors/ovl_En_Door/z_en_door.h"
    "src/overlays/actors/ovl_En_Ds/z_en_ds.c"
    "src/overlays/actors/ovl_En_Ds/z_en_ds.h"
    "src/overlays/actors/ovl_En_Du/z_en_du.c"
    "src/overlays/actors/ovl_En_Du/z_en_du.h"
    "src/overlays/actors/ovl_En_Dy_Extra/z_en_dy_extra.c"
    "src/overlays/actors/ovl_En_Dy_Extra/z_en_dy_extra.h"
    "src/overlays/actors/ovl_En_Eg/z_en_eg.c"
    "src/overlays/actors/ovl_En_Eg/z_en_eg.h"
    "src/overlays/actors/ovl_En_Eiyer/z_en_eiyer.c"
    "src/overlays/actors/ovl_En_Eiyer/z_en_eiyer.h"
    "src/overlays/actors/ovl_En_Elf/z_en_elf.c"
    "src/overlays/actors/ovl_En_Elf/z_en_elf.h"
    "src/overlays/actors/ovl_En_Encount1/z_en_encount1.c"
    "src/overlays/actors/ovl_En_Encount1/z_en_encount1.h"
    "src/overlays/actors/ovl_En_Encount2/z_en_encount2.c"
    "src/overlays/actors/ovl_En_Encount2/z_en_encount2.h"
    "src/overlays/actors/ovl_En_Ex_Item/z_en_ex_item.c"
    "src/overlays/actors/ovl_En_Ex_Item/z_en_ex_item.h"
    "src/overlays/actors/ovl_En_Ex_Ruppy/z_en_ex_ruppy.c"
    "src/overlays/actors/ovl_En_Ex_Ruppy/z_en_ex_ruppy.h"
    "src/overlays/actors/ovl_En_Fd/z_en_fd.c"
    "src/overlays/actors/ovl_En_Fd/z_en_fd.h"
    "src/overlays/actors/ovl_En_Fd_Fire/z_en_fd_fire.c"
    "src/overlays/actors/ovl_En_Fd_Fire/z_en_fd_fire.h"
    "src/overlays/actors/ovl_En_fHG/z_en_fhg.c"
    "src/overlays/actors/ovl_En_fHG/z_en_fhg.h"
    "src/overlays/actors/ovl_En_Fhg_Fire/z_en_fhg_fire.c"
    "src/overlays/actors/ovl_En_Fhg_Fire/z_en_fhg_fire.h"
    "src/overlays/actors/ovl_En_Fire_Rock/z_en_fire_rock.c"
    "src/overlays/actors/ovl_En_Fire_Rock/z_en_fire_rock.h"
    "src/overlays/actors/ovl_En_Firefly/z_en_firefly.c"
    "src/overlays/actors/ovl_En_Firefly/z_en_firefly.h"
    "src/overlays/actors/ovl_En_Fish/z_en_fish.c"
    "src/overlays/actors/ovl_En_Fish/z_en_fish.h"
    "src/overlays/actors/ovl_En_Floormas/z_en_floormas.c"
    "src/overlays/actors/ovl_En_Floormas/z_en_floormas.h"
    "src/overlays/actors/ovl_En_Fr/z_en_fr.c"
    "src/overlays/actors/ovl_En_Fr/z_en_fr.h"
    "src/overlays/actors/ovl_En_Fu/z_en_fu.c"
    "src/overlays/actors/ovl_En_Fu/z_en_fu.h"
    "src/overlays/actors/ovl_En_Fw/z_en_fw.c"
    "src/overlays/actors/ovl_En_Fw/z_en_fw.h"
    "src/overlays/actors/ovl_En_Fz/z_en_fz.c"
    "src/overlays/actors/ovl_En_Fz/z_en_fz.h"
    "src/overlays/actors/ovl_En_G_Switch/z_en_g_switch.c"
    "src/overlays/actors/ovl_En_G_Switch/z_en_g_switch.h"
    "src/overlays/actors/ovl_En_Ganon_Mant/z_en_ganon_mant.c"
    "src/overlays/actors/ovl_En_Ganon_Mant/z_en_ganon_mant.h"
    "src/overlays/actors/ovl_En_Ganon_Organ/z_en_ganon_organ.c"
    "src/overlays/actors/ovl_En_Ganon_Organ/z_en_ganon_organ.h"
    "src/overlays/actors/ovl_En_Gb/z_en_gb.c"
    "src/overlays/actors/ovl_En_Gb/z_en_gb.h"
    "src/overlays/actors/ovl_En_Ge1/z_en_ge1.c"
    "src/overlays/actors/ovl_En_Ge1/z_en_ge1.h"
    "src/overlays/actors/ovl_En_Ge2/z_en_ge2.c"
    "src/overlays/actors/ovl_En_Ge2/z_en_ge2.h"
    "src/overlays/actors/ovl_En_Ge3/z_en_ge3.c"
    "src/overlays/actors/ovl_En_Ge3/z_en_ge3.h"
    "src/overlays/actors/ovl_En_GeldB/z_en_geldb.c"
    "src/overlays/actors/ovl_En_GeldB/z_en_geldb.h"
    "src/overlays/actors/ovl_En_GirlA/z_en_girla.c"
    "src/overlays/actors/ovl_En_GirlA/z_en_girla.h"
    "src/overlays/actors/ovl_En_Gm/z_en_gm.c"
    "src/overlays/actors/ovl_En_Gm/z_en_gm.h"
    "src/overlays/actors/ovl_En_Go/z_en_go.c"
    "src/overlays/actors/ovl_En_Go/z_en_go.h"
    "src/overlays/actors/ovl_En_Go2/z_en_go2.c"
    "src/overlays/actors/ovl_En_Go2/z_en_go2.h"
    "src/overlays/actors/ovl_En_Goma/z_en_goma.c"
    "src/overlays/actors/ovl_En_Goma/z_en_goma.h"
    "src/overlays/actors/ovl_En_Goroiwa/z_en_goroiwa.c"
    "src/overlays/actors/ovl_En_Goroiwa/z_en_goroiwa.h"
    "src/overlays/actors/ovl_En_Gs/z_en_gs.c"
    "src/overlays/actors/ovl_En_Gs/z_en_gs.h"
    "src/overlays/actors/ovl_En_Guest/z_en_guest.c"
    "src/overlays/actors/ovl_En_Guest/z_en_guest.h"
    "src/overlays/actors/ovl_En_Hata/z_en_hata.c"
    "src/overlays/actors/ovl_En_Hata/z_en_hata.h"
    "src/overlays/actors/ovl_En_Heishi1/z_en_heishi1.c"
    "src/overlays/actors/ovl_En_Heishi1/z_en_heishi1.h"
    "src/overlays/actors/ovl_En_Heishi2/z_en_heishi2.c"
    "src/overlays/actors/ovl_En_Heishi2/z_en_heishi2.h"
    "src/overlays/actors/ovl_En_Heishi3/z_en_heishi3.c"
    "src/overlays/actors/ovl_En_Heishi3/z_en_heishi3.h"
    "src/overlays/actors/ovl_En_Heishi4/z_en_heishi4.c"
    "src/overlays/actors/ovl_En_Heishi4/z_en_heishi4.h"
    "src/overlays/actors/ovl_En_Hintnuts/z_en_hintnuts.c"
    "src/overlays/actors/ovl_En_Hintnuts/z_en_hintnuts.h"
    "src/overlays/actors/ovl_En_Holl/z_en_holl.c"
    "src/overlays/actors/ovl_En_Holl/z_en_holl.h"
    "src/overlays/actors/ovl_En_Honotrap/z_en_honotrap.c"
    "src/overlays/actors/ovl_En_Honotrap/z_en_honotrap.h"
    "src/overlays/actors/ovl_En_Horse/z_en_horse.c"
    "src/overlays/actors/ovl_En_Horse/z_en_horse.h"
    "src/overlays/actors/ovl_En_Horse_Game_Check/z_en_horse_game_check.c"
    "src/overlays/actors/ovl_En_Horse_Game_Check/z_en_horse_game_check.h"
    "src/overlays/actors/ovl_En_Horse_Ganon/z_en_horse_ganon.c"
    "src/overlays/actors/ovl_En_Horse_Ganon/z_en_horse_ganon.h"
    "src/overlays/actors/ovl_En_Horse_Link_Child/z_en_horse_link_child.c"
    "src/overlays/actors/ovl_En_Horse_Link_Child/z_en_horse_link_child.h"
    "src/overlays/actors/ovl_En_Horse_Normal/z_en_horse_normal.c"
    "src/overlays/actors/ovl_En_Horse_Normal/z_en_horse_normal.h"
    "src/overlays/actors/ovl_En_Horse_Zelda/z_en_horse_zelda.c"
    "src/overlays/actors/ovl_En_Horse_Zelda/z_en_horse_zelda.h"
    "src/overlays/actors/ovl_En_Hs/z_en_hs.c"
    "src/overlays/actors/ovl_En_Hs/z_en_hs.h"
    "src/overlays/actors/ovl_En_Hs2/z_en_hs2.c"
    "src/overlays/actors/ovl_En_Hs2/z_en_hs2.h"
    "src/overlays/actors/ovl_En_Hy/z_en_hy.c"
    "src/overlays/actors/ovl_En_Hy/z_en_hy.h"
    "src/overlays/actors/ovl_En_Ice_Hono/z_en_ice_hono.c"
    "src/overlays/actors/ovl_En_Ice_Hono/z_en_ice_hono.h"
    "src/overlays/actors/ovl_En_Ik/z_en_ik.c"
    "src/overlays/actors/ovl_En_Ik/z_en_ik.h"
    "src/overlays/actors/ovl_En_In/z_en_in.c"
    "src/overlays/actors/ovl_En_In/z_en_in.h"
    "src/overlays/actors/ovl_En_Insect/z_en_insect.c"
    "src/overlays/actors/ovl_En_Insect/z_en_insect.h"
    "src/overlays/actors/ovl_En_Ishi/z_en_ishi.c"
    "src/overlays/actors/ovl_En_Ishi/z_en_ishi.h"
    "src/overlays/actors/ovl_En_It/z_en_it.c"
    "src/overlays/actors/ovl_En_It/z_en_it.h"
    "src/overlays/actors/ovl_En_Jj/z_en_jj.c"
    "src/overlays/actors/ovl_En_Jj/z_en_jj.h"
    "src/overlays/actors/ovl_En_Jj/z_en_jj_cutscene_data.c"
    "src/overlays/actors/ovl_En_Js/z_en_js.c"
    "src/overlays/actors/ovl_En_Js/z_en_js.h"
    "src/overlays/actors/ovl_En_Jsjutan/z_en_jsjutan.c"
    "src/overlays/actors/ovl_En_Jsjutan/z_en_jsjutan.h"
    "src/overlays/actors/ovl_En_Kakasi/z_en_kakasi.c"
    "src/overlays/actors/ovl_En_Kakasi/z_en_kakasi.h"
    "src/overlays/actors/ovl_En_Kakasi2/z_en_kakasi2.c"
    "src/overlays/actors/ovl_En_Kakasi2/z_en_kakasi2.h"
    "src/overlays/actors/ovl_En_Kakasi3/z_en_kakasi3.c"
    "src/overlays/actors/ovl_En_Kakasi3/z_en_kakasi3.h"
    "src/overlays/actors/ovl_En_Kanban/z_en_kanban.c"
    "src/overlays/actors/ovl_En_Kanban/z_en_kanban.h"
    "src/overlays/actors/ovl_En_Kanban/z_en_kanban_gfx.c"
    "src/overlays/actors/ovl_En_Karebaba/z_en_karebaba.c"
    "src/overlays/actors/ovl_En_Karebaba/z_en_karebaba.h"
    "src/overlays/actors/ovl_En_Ko/z_en_ko.c"
    "src/overlays/actors/ovl_En_Ko/z_en_ko.h"
    "src/overlays/actors/ovl_En_Kusa/z_en_kusa.c"
    "src/overlays/actors/ovl_En_Kusa/z_en_kusa.h"
    "src/overlays/actors/ovl_En_Kz/z_en_kz.c"
    "src/overlays/actors/ovl_En_Kz/z_en_kz.h"
    "src/overlays/actors/ovl_En_Light/z_en_light.c"
    "src/overlays/actors/ovl_En_Light/z_en_light.h"
    "src/overlays/actors/ovl_En_Lightbox/z_en_lightbox.c"
    "src/overlays/actors/ovl_En_Lightbox/z_en_lightbox.h"
    "src/overlays/actors/ovl_En_M_Fire1/z_en_m_fire1.c"
    "src/overlays/actors/ovl_En_M_Fire1/z_en_m_fire1.h"
    "src/overlays/actors/ovl_En_M_Thunder/z_en_m_thunder.c"
    "src/overlays/actors/ovl_En_M_Thunder/z_en_m_thunder.h"
    "src/overlays/actors/ovl_En_Ma1/z_en_ma1.c"
    "src/overlays/actors/ovl_En_Ma1/z_en_ma1.h"
    "src/overlays/actors/ovl_En_Ma2/z_en_ma2.c"
    "src/overlays/actors/ovl_En_Ma2/z_en_ma2.h"
    "src/overlays/actors/ovl_En_Ma3/z_en_ma3.c"
    "src/overlays/actors/ovl_En_Ma3/z_en_ma3.h"
    "src/overlays/actors/ovl_En_Mag/z_en_mag.c"
    "src/overlays/actors/ovl_En_Mag/z_en_mag.h"
    "src/overlays/actors/ovl_En_Mb/z_en_mb.c"
    "src/overlays/actors/ovl_En_Mb/z_en_mb.h"
    "src/overlays/actors/ovl_En_Md/z_en_md.c"
    "src/overlays/actors/ovl_En_Md/z_en_md.h"
    "src/overlays/actors/ovl_En_Mk/z_en_mk.c"
    "src/overlays/actors/ovl_En_Mk/z_en_mk.h"
    "src/overlays/actors/ovl_En_Mm/z_en_mm.c"
    "src/overlays/actors/ovl_En_Mm/z_en_mm.h"
    "src/overlays/actors/ovl_En_Mm2/z_en_mm2.c"
    "src/overlays/actors/ovl_En_Mm2/z_en_mm2.h"
    "src/overlays/actors/ovl_En_Ms/z_en_ms.c"
    "src/overlays/actors/ovl_En_Ms/z_en_ms.h"
    "src/overlays/actors/ovl_En_Mu/z_en_mu.c"
    "src/overlays/actors/ovl_En_Mu/z_en_mu.h"
    "src/overlays/actors/ovl_En_Nb/z_en_nb.c"
    "src/overlays/actors/ovl_En_Nb/z_en_nb.h"
    "src/overlays/actors/ovl_En_Nb/z_en_nb_cutscene_data.c"
    "src/overlays/actors/ovl_En_Niw/z_en_niw.c"
    "src/overlays/actors/ovl_En_Niw/z_en_niw.h"
    "src/overlays/actors/ovl_En_Niw_Girl/z_en_niw_girl.c"
    "src/overlays/actors/ovl_En_Niw_Girl/z_en_niw_girl.h"
    "src/overlays/actors/ovl_En_Niw_Lady/z_en_niw_lady.c"
    "src/overlays/actors/ovl_En_Niw_Lady/z_en_niw_lady.h"
    "src/overlays/actors/ovl_En_Nutsball/z_en_nutsball.c"
    "src/overlays/actors/ovl_En_Nutsball/z_en_nutsball.h"
    "src/overlays/actors/ovl_En_Nwc/z_en_nwc.c"
    "src/overlays/actors/ovl_En_Nwc/z_en_nwc.h"
    "src/overlays/actors/ovl_En_Ny/z_en_ny.c"
    "src/overlays/actors/ovl_En_Ny/z_en_ny.h"
    "src/overlays/actors/ovl_En_OE2/z_en_oe2.c"
    "src/overlays/actors/ovl_En_OE2/z_en_oe2.h"
    "src/overlays/actors/ovl_En_Okarina_Effect/z_en_okarina_effect.c"
    "src/overlays/actors/ovl_En_Okarina_Effect/z_en_okarina_effect.h"
    "src/overlays/actors/ovl_En_Okarina_Tag/z_en_okarina_tag.c"
    "src/overlays/actors/ovl_En_Okarina_Tag/z_en_okarina_tag.h"
    "src/overlays/actors/ovl_En_Okarina_Tag/z_en_okarina_tag_cutscene_data.c"
    "src/overlays/actors/ovl_En_Okuta/z_en_okuta.c"
    "src/overlays/actors/ovl_En_Okuta/z_en_okuta.h"
    "src/overlays/actors/ovl_En_Ossan/z_en_ossan.c"
    "src/overlays/actors/ovl_En_Ossan/z_en_ossan.h"
    "src/overlays/actors/ovl_En_Owl/z_en_owl.c"
    "src/overlays/actors/ovl_En_Owl/z_en_owl.h"
    "src/overlays/actors/ovl_En_Part/z_en_part.c"
    "src/overlays/actors/ovl_En_Part/z_en_part.h"
    "src/overlays/actors/ovl_En_Peehat/z_en_peehat.c"
    "src/overlays/actors/ovl_En_Peehat/z_en_peehat.h"
    "src/overlays/actors/ovl_En_Po_Desert/z_en_po_desert.c"
    "src/overlays/actors/ovl_En_Po_Desert/z_en_po_desert.h"
    "src/overlays/actors/ovl_En_Po_Field/z_en_po_field.c"
    "src/overlays/actors/ovl_En_Po_Field/z_en_po_field.h"
    "src/overlays/actors/ovl_En_Po_Relay/z_en_po_relay.c"
    "src/overlays/actors/ovl_En_Po_Relay/z_en_po_relay.h"
    "src/overlays/actors/ovl_En_Po_Sisters/z_en_po_sisters.c"
    "src/overlays/actors/ovl_En_Po_Sisters/z_en_po_sisters.h"
    "src/overlays/actors/ovl_En_Poh/z_en_poh.c"
    "src/overlays/actors/ovl_En_Poh/z_en_poh.h"
    "src/overlays/actors/ovl_En_Pu_box/z_en_pu_box.c"
    "src/overlays/actors/ovl_En_Pu_box/z_en_pu_box.h"
    "src/overlays/actors/ovl_En_Rd/z_en_rd.c"
    "src/overlays/actors/ovl_En_Rd/z_en_rd.h"
    "src/overlays/actors/ovl_En_Reeba/z_en_reeba.c"
    "src/overlays/actors/ovl_En_Reeba/z_en_reeba.h"
    "src/overlays/actors/ovl_En_River_Sound/z_en_river_sound.c"
    "src/overlays/actors/ovl_En_River_Sound/z_en_river_sound.h"
    "src/overlays/actors/ovl_En_Rl/z_en_rl.c"
    "src/overlays/actors/ovl_En_Rl/z_en_rl.h"
    "src/overlays/actors/ovl_En_Rr/z_en_rr.c"
    "src/overlays/actors/ovl_En_Rr/z_en_rr.h"
    "src/overlays/actors/ovl_En_Ru1/z_en_ru1.c"
    "src/overlays/actors/ovl_En_Ru1/z_en_ru1.h"
    "src/overlays/actors/ovl_En_Ru1/z_en_ru1_cutscene_data.c"
    "src/overlays/actors/ovl_En_Ru2/z_en_ru2.c"
    "src/overlays/actors/ovl_En_Ru2/z_en_ru2.h"
    "src/overlays/actors/ovl_En_Ru2/z_en_ru2_cutscene_data.c"
    "src/overlays/actors/ovl_En_Sa/z_en_sa.c"
    "src/overlays/actors/ovl_En_Sa/z_en_sa.h"
    "src/overlays/actors/ovl_En_Sb/z_en_sb.c"
    "src/overlays/actors/ovl_En_Sb/z_en_sb.h"
    "src/overlays/actors/ovl_En_Scene_Change/z_en_scene_change.c"
    "src/overlays/actors/ovl_En_Scene_Change/z_en_scene_change.h"
    "src/overlays/actors/ovl_En_Sda/z_en_sda.c"
    "src/overlays/actors/ovl_En_Sda/z_en_sda.h"
    "src/overlays/actors/ovl_En_Shopnuts/z_en_shopnuts.c"
    "src/overlays/actors/ovl_En_Shopnuts/z_en_shopnuts.h"
    "src/overlays/actors/ovl_En_Si/z_en_si.c"
    "src/overlays/actors/ovl_En_Si/z_en_si.h"
    "src/overlays/actors/ovl_En_Siofuki/z_en_siofuki.c"
    "src/overlays/actors/ovl_En_Siofuki/z_en_siofuki.h"
    "src/overlays/actors/ovl_En_Skb/z_en_skb.c"
    "src/overlays/actors/ovl_En_Skb/z_en_skb.h"
    "src/overlays/actors/ovl_En_Skj/z_en_skj.c"
    "src/overlays/actors/ovl_En_Skj/z_en_skj.h"
    "src/overlays/actors/ovl_En_Skjneedle/z_en_skjneedle.c"
    "src/overlays/actors/ovl_En_Skjneedle/z_en_skjneedle.h"
    "src/overlays/actors/ovl_En_Ssh/z_en_ssh.c"
    "src/overlays/actors/ovl_En_Ssh/z_en_ssh.h"
    "src/overlays/actors/ovl_En_St/z_en_st.c"
    "src/overlays/actors/ovl_En_St/z_en_st.h"
    "src/overlays/actors/ovl_En_Sth/z_en_sth.c"
    "src/overlays/actors/ovl_En_Sth/z_en_sth.h"
    "src/overlays/actors/ovl_En_Stream/z_en_stream.c"
    "src/overlays/actors/ovl_En_Stream/z_en_stream.h"
    "src/overlays/actors/ovl_En_Sw/z_en_sw.c"
    "src/overlays/actors/ovl_En_Sw/z_en_sw.h"
    "src/overlays/actors/ovl_En_Syateki_Itm/z_en_syateki_itm.c"
    "src/overlays/actors/ovl_En_Syateki_Itm/z_en_syateki_itm.h"
    "src/overlays/actors/ovl_En_Syateki_Man/z_en_syateki_man.c"
    "src/overlays/actors/ovl_En_Syateki_Man/z_en_syateki_man.h"
    "src/overlays/actors/ovl_En_Syateki_Niw/z_en_syateki_niw.c"
    "src/overlays/actors/ovl_En_Syateki_Niw/z_en_syateki_niw.h"
    "src/overlays/actors/ovl_En_Ta/z_en_ta.c"
    "src/overlays/actors/ovl_En_Ta/z_en_ta.h"
    "src/overlays/actors/ovl_En_Takara_Man/z_en_takara_man.c"
    "src/overlays/actors/ovl_En_Takara_Man/z_en_takara_man.h"
    "src/overlays/actors/ovl_En_Tana/z_en_tana.c"
    "src/overlays/actors/ovl_En_Tana/z_en_tana.h"
    "src/overlays/actors/ovl_En_Test/z_en_test.c"
    "src/overlays/actors/ovl_En_Test/z_en_test.h"
    "src/overlays/actors/ovl_En_Tg/z_en_tg.c"
    "src/overlays/actors/ovl_En_Tg/z_en_tg.h"
    "src/overlays/actors/ovl_En_Tite/z_en_tite.c"
    "src/overlays/actors/ovl_En_Tite/z_en_tite.h"
    "src/overlays/actors/ovl_En_Tk/z_en_tk.c"
    "src/overlays/actors/ovl_En_Tk/z_en_tk.h"
    "src/overlays/actors/ovl_En_Torch/z_en_torch.c"
    "src/overlays/actors/ovl_En_Torch/z_en_torch.h"
    "src/overlays/actors/ovl_En_Torch2/z_en_torch2.c"
    "src/overlays/actors/ovl_En_Torch2/z_en_torch2.h"
    "src/overlays/actors/ovl_En_Toryo/z_en_toryo.c"
    "src/overlays/actors/ovl_En_Toryo/z_en_toryo.h"
    "src/overlays/actors/ovl_En_Tp/z_en_tp.c"
    "src/overlays/actors/ovl_En_Tp/z_en_tp.h"
    "src/overlays/actors/ovl_En_Tr/z_en_tr.c"
    "src/overlays/actors/ovl_En_Tr/z_en_tr.h"
    "src/overlays/actors/ovl_En_Trap/z_en_trap.c"
    "src/overlays/actors/ovl_En_Trap/z_en_trap.h"
    "src/overlays/actors/ovl_En_Tubo_Trap/z_en_tubo_trap.c"
    "src/overlays/actors/ovl_En_Tubo_Trap/z_en_tubo_trap.h"
    "src/overlays/actors/ovl_En_Vali/z_en_vali.c"
    "src/overlays/actors/ovl_En_Vali/z_en_vali.h"
    "src/overlays/actors/ovl_En_Vase/z_en_vase.c"
    "src/overlays/actors/ovl_En_Vase/z_en_vase.h"
    "src/overlays/actors/ovl_En_Vb_Ball/z_en_vb_ball.c"
    "src/overlays/actors/ovl_En_Vb_Ball/z_en_vb_ball.h"
    "src/overlays/actors/ovl_En_Viewer/z_en_viewer.c"
    "src/overlays/actors/ovl_En_Viewer/z_en_viewer.h"
    "src/overlays/actors/ovl_En_Vm/z_en_vm.c"
    "src/overlays/actors/ovl_En_Vm/z_en_vm.h"
    "src/overlays/actors/ovl_En_Wall_Tubo/z_en_wall_tubo.c"
    "src/overlays/actors/ovl_En_Wall_Tubo/z_en_wall_tubo.h"
    "src/overlays/actors/ovl_En_Wallmas/z_en_wallmas.c"
    "src/overlays/actors/ovl_En_Wallmas/z_en_wallmas.h"
    "src/overlays/actors/ovl_En_Weather_Tag/z_en_weather_tag.c"
    "src/overlays/actors/ovl_En_Weather_Tag/z_en_weather_tag.h"
    "src/overlays/actors/ovl_En_Weiyer/z_en_weiyer.c"
    "src/overlays/actors/ovl_En_Weiyer/z_en_weiyer.h"
    "src/overlays/actors/ovl_En_Wf/z_en_wf.c"
    "src/overlays/actors/ovl_En_Wf/z_en_wf.h"
    "src/overlays/actors/ovl_En_Wonder_Item/z_en_wonder_item.c"
    "src/overlays/actors/ovl_En_Wonder_Item/z_en_wonder_item.h"
    "src/overlays/actors/ovl_En_Wonder_Talk/z_en_wonder_talk.c"
    "src/overlays/actors/ovl_En_Wonder_Talk/z_en_wonder_talk.h"
    "src/overlays/actors/ovl_En_Wonder_Talk2/z_en_wonder_talk2.c"
    "src/overlays/actors/ovl_En_Wonder_Talk2/z_en_wonder_talk2.h"
    "src/overlays/actors/ovl_En_Wood02/z_en_wood02.c"
    "src/overlays/actors/ovl_En_Wood02/z_en_wood02.h"
    "src/overlays/actors/ovl_En_Xc/z_en_xc.c"
    "src/overlays/actors/ovl_En_Xc/z_en_xc.h"
    "src/overlays/actors/ovl_En_Yabusame_Mark/z_en_yabusame_mark.c"
    "src/overlays/actors/ovl_En_Yabusame_Mark/z_en_yabusame_mark.h"
    "src/overlays/actors/ovl_En_Yukabyun/z_en_yukabyun.c"
    "src/overlays/actors/ovl_En_Yukabyun/z_en_yukabyun.h"
    "src/overlays/actors/ovl_En_Zf/z_en_zf.c"
    "src/overlays/actors/ovl_En_Zf/z_en_zf.h"
    "src/overlays/actors/ovl_En_Zl1/z_en_zl1.c"
    "src/overlays/actors/ovl_En_Zl1/z_en_zl1.h"
    "src/overlays/actors/ovl_En_Zl1/z_en_zl1_camera_data.c"
    "src/overlays/actors/ovl_En_Zl1/z_en_zl1_cutscene_data.c"
    "src/overlays/actors/ovl_En_Zl2/z_en_zl2.c"
    "src/overlays/actors/ovl_En_Zl2/z_en_zl2.h"
    "src/overlays/actors/ovl_En_Zl3/z_en_zl3.c"
    "src/overlays/actors/ovl_En_Zl3/z_en_zl3.h"
    "src/overlays/actors/ovl_En_Zl4/z_en_zl4.c"
    "src/overlays/actors/ovl_En_Zl4/z_en_zl4.h"
    "src/overlays/actors/ovl_En_Zl4/z_en_zl4_cutscene_data.c"
    "src/overlays/actors/ovl_En_Zo/z_en_zo.c"
    "src/overlays/actors/ovl_En_Zo/z_en_zo.h"
    "src/overlays/actors/ovl_End_Title/z_end_title.c"
    "src/overlays/actors/ovl_End_Title/z_end_title.h"
    "src/overlays/actors/ovl_Fishing/z_fishing.c"
    "src/overlays/actors/ovl_Fishing/z_fishing.h"
    "src/overlays/actors/ovl_Item_B_Heart/z_item_b_heart.c"
    "src/overlays/actors/ovl_Item_B_Heart/z_item_b_heart.h"
    "src/overlays/actors/ovl_Item_Etcetera/z_item_etcetera.c"
    "src/overlays/actors/ovl_Item_Etcetera/z_item_etcetera.h"
    "src/overlays/actors/ovl_Item_Inbox/z_item_inbox.c"
    "src/overlays/actors/ovl_Item_Inbox/z_item_inbox.h"
    "src/overlays/actors/ovl_Item_Ocarina/z_item_ocarina.c"
    "src/overlays/actors/ovl_Item_Ocarina/z_item_ocarina.h"
    "src/overlays/actors/ovl_Item_Shield/z_item_shield.c"
    "src/overlays/actors/ovl_Item_Shield/z_item_shield.h"
    "src/overlays/actors/ovl_Magic_Dark/z_magic_dark.c"
    "src/overlays/actors/ovl_Magic_Dark/z_magic_dark.h"
    "src/overlays/actors/ovl_Magic_Fire/z_magic_fire.c"
    "src/overlays/actors/ovl_Magic_Fire/z_magic_fire.h"
    "src/overlays/actors/ovl_Magic_Wind/z_magic_wind.c"
    "src/overlays/actors/ovl_Magic_Wind/z_magic_wind.h"
    "src/overlays/actors/ovl_Mir_Ray/z_mir_ray.c"
    "src/overlays/actors/ovl_Mir_Ray/z_mir_ray.h"
    "src/overlays/actors/ovl_Obj_Bean/z_obj_bean.c"
    "src/overlays/actors/ovl_Obj_Bean/z_obj_bean.h"
    "src/overlays/actors/ovl_Obj_Blockstop/z_obj_blockstop.c"
    "src/overlays/actors/ovl_Obj_Blockstop/z_obj_blockstop.h"
    "src/overlays/actors/ovl_Obj_Bombiwa/z_obj_bombiwa.c"
    "src/overlays/actors/ovl_Obj_Bombiwa/z_obj_bombiwa.h"
    "src/overlays/actors/ovl_Obj_Comb/z_obj_comb.c"
    "src/overlays/actors/ovl_Obj_Comb/z_obj_comb.h"
    "src/overlays/actors/ovl_Obj_Dekujr/z_obj_dekujr.c"
    "src/overlays/actors/ovl_Obj_Dekujr/z_obj_dekujr.h"
    "src/overlays/actors/ovl_Obj_Elevator/z_obj_elevator.c"
    "src/overlays/actors/ovl_Obj_Elevator/z_obj_elevator.h"
    "src/overlays/actors/ovl_Obj_Hamishi/z_obj_hamishi.c"
    "src/overlays/actors/ovl_Obj_Hamishi/z_obj_hamishi.h"
    "src/overlays/actors/ovl_Obj_Hana/z_obj_hana.c"
    "src/overlays/actors/ovl_Obj_Hana/z_obj_hana.h"
    "src/overlays/actors/ovl_Obj_Hsblock/z_obj_hsblock.c"
    "src/overlays/actors/ovl_Obj_Hsblock/z_obj_hsblock.h"
    "src/overlays/actors/ovl_Obj_Ice_Poly/z_obj_ice_poly.c"
    "src/overlays/actors/ovl_Obj_Ice_Poly/z_obj_ice_poly.h"
    "src/overlays/actors/ovl_Obj_Kibako/z_obj_kibako.c"
    "src/overlays/actors/ovl_Obj_Kibako/z_obj_kibako.h"
    "src/overlays/actors/ovl_Obj_Kibako2/z_obj_kibako2.c"
    "src/overlays/actors/ovl_Obj_Kibako2/z_obj_kibako2.h"
    "src/overlays/actors/ovl_Obj_Lift/z_obj_lift.c"
    "src/overlays/actors/ovl_Obj_Lift/z_obj_lift.h"
    "src/overlays/actors/ovl_Obj_Lightswitch/z_obj_lightswitch.c"
    "src/overlays/actors/ovl_Obj_Lightswitch/z_obj_lightswitch.h"
    "src/overlays/actors/ovl_Obj_Makekinsuta/z_obj_makekinsuta.c"
    "src/overlays/actors/ovl_Obj_Makekinsuta/z_obj_makekinsuta.h"
    "src/overlays/actors/ovl_Obj_Makeoshihiki/z_obj_makeoshihiki.c"
    "src/overlays/actors/ovl_Obj_Makeoshihiki/z_obj_makeoshihiki.h"
    "src/overlays/actors/ovl_Obj_Mure/z_obj_mure.c"
    "src/overlays/actors/ovl_Obj_Mure/z_obj_mure.h"
    "src/overlays/actors/ovl_Obj_Mure2/z_obj_mure2.c"
    "src/overlays/actors/ovl_Obj_Mure2/z_obj_mure2.h"
    "src/overlays/actors/ovl_Obj_Mure3/z_obj_mure3.c"
    "src/overlays/actors/ovl_Obj_Mure3/z_obj_mure3.h"
    "src/overlays/actors/ovl_Obj_Oshihiki/z_obj_oshihiki.c"
    "src/overlays/actors/ovl_Obj_Oshihiki/z_obj_oshihiki.h"
    "src/overlays/actors/ovl_Obj_Roomtimer/z_obj_roomtimer.c"
    "src/overlays/actors/ovl_Obj_Roomtimer/z_obj_roomtimer.h"
    "src/overlays/actors/ovl_Obj_Switch/z_obj_switch.c"
    "src/overlays/actors/ovl_Obj_Switch/z_obj_switch.h"
    "src/overlays/actors/ovl_Obj_Syokudai/z_obj_syokudai.c"
    "src/overlays/actors/ovl_Obj_Syokudai/z_obj_syokudai.h"
    "src/overlays/actors/ovl_Obj_Timeblock/z_obj_timeblock.c"
    "src/overlays/actors/ovl_Obj_Timeblock/z_obj_timeblock.h"
    "src/overlays/actors/ovl_Obj_Tsubo/z_obj_tsubo.c"
    "src/overlays/actors/ovl_Obj_Tsubo/z_obj_tsubo.h"
    "src/overlays/actors/ovl_Obj_Warp2block/z_obj_warp2block.c"
    "src/overlays/actors/ovl_Obj_Warp2block/z_obj_warp2block.h"
    "src/overlays/actors/ovl_Object_Kankyo/z_object_kankyo.c"
    "src/overlays/actors/ovl_Object_Kankyo/z_object_kankyo.h"
    "src/overlays/actors/ovl_Oceff_Spot/z_oceff_spot.c"
    "src/overlays/actors/ovl_Oceff_Spot/z_oceff_spot.h"
    "src/overlays/actors/ovl_Oceff_Storm/z_oceff_storm.c"
    "src/overlays/actors/ovl_Oceff_Storm/z_oceff_storm.h"
    "src/overlays/actors/ovl_Oceff_Wipe/z_oceff_wipe.c"
    "src/overlays/actors/ovl_Oceff_Wipe/z_oceff_wipe.h"
    "src/overlays/actors/ovl_Oceff_Wipe2/z_oceff_wipe2.c"
    "src/overlays/actors/ovl_Oceff_Wipe2/z_oceff_wipe2.h"
    "src/overlays/actors/ovl_Oceff_Wipe3/z_oceff_wipe3.c"
    "src/overlays/actors/ovl_Oceff_Wipe3/z_oceff_wipe3.h"
    "src/overlays/actors/ovl_Oceff_Wipe4/z_oceff_wipe4.c"
    "src/overlays/actors/ovl_Oceff_Wipe4/z_oceff_wipe4.h"
    "src/overlays/actors/ovl_player_actor/z_player.c"
    "src/overlays/actors/ovl_Shot_Sun/z_shot_sun.c"
    "src/overlays/actors/ovl_Shot_Sun/z_shot_sun.h"
)
source_group("Source Files\\src\\overlays\\actors" FILES ${Source_Files__src__overlays__actors})

set(Source_Files__src__overlays__effects
    "src/overlays/effects/ovl_Effect_Ss_Blast/z_eff_ss_blast.c"
    "src/overlays/effects/ovl_Effect_Ss_Blast/z_eff_ss_blast.h"
    "src/overlays/effects/ovl_Effect_Ss_Bomb/z_eff_ss_bomb.c"
    "src/overlays/effects/ovl_Effect_Ss_Bomb/z_eff_ss_bomb.h"
    "src/overlays/effects/ovl_Effect_Ss_Bomb2/z_eff_ss_bomb2.c"
    "src/overlays/effects/ovl_Effect_Ss_Bomb2/z_eff_ss_bomb2.h"
    "src/overlays/effects/ovl_Effect_Ss_Bubble/z_eff_ss_bubble.c"
    "src/overlays/effects/ovl_Effect_Ss_Bubble/z_eff_ss_bubble.h"
    "src/overlays/effects/ovl_Effect_Ss_D_Fire/z_eff_ss_d_fire.c"
    "src/overlays/effects/ovl_Effect_Ss_D_Fire/z_eff_ss_d_fire.h"
    "src/overlays/effects/ovl_Effect_Ss_Dead_Db/z_eff_ss_dead_db.c"
    "src/overlays/effects/ovl_Effect_Ss_Dead_Db/z_eff_ss_dead_db.h"
    "src/overlays/effects/ovl_Effect_Ss_Dead_Dd/z_eff_ss_dead_dd.c"
    "src/overlays/effects/ovl_Effect_Ss_Dead_Dd/z_eff_ss_dead_dd.h"
    "src/overlays/effects/ovl_Effect_Ss_Dead_Ds/z_eff_ss_dead_ds.c"
    "src/overlays/effects/ovl_Effect_Ss_Dead_Ds/z_eff_ss_dead_ds.h"
    "src/overlays/effects/ovl_Effect_Ss_Dead_Sound/z_eff_ss_dead_sound.c"
    "src/overlays/effects/ovl_Effect_Ss_Dead_Sound/z_eff_ss_dead_sound.h"
    "src/overlays/effects/ovl_Effect_Ss_Dt_Bubble/z_eff_ss_dt_bubble.c"
    "src/overlays/effects/ovl_Effect_Ss_Dt_Bubble/z_eff_ss_dt_bubble.h"
    "src/overlays/effects/ovl_Effect_Ss_Dust/z_eff_ss_dust.c"
    "src/overlays/effects/ovl_Effect_Ss_Dust/z_eff_ss_dust.h"
    "src/overlays/effects/ovl_Effect_Ss_En_Fire/z_eff_ss_en_fire.c"
    "src/overlays/effects/ovl_Effect_Ss_En_Fire/z_eff_ss_en_fire.h"
    "src/overlays/effects/ovl_Effect_Ss_En_Ice/z_eff_ss_en_ice.c"
    "src/overlays/effects/ovl_Effect_Ss_En_Ice/z_eff_ss_en_ice.h"
    "src/overlays/effects/ovl_Effect_Ss_Extra/z_eff_ss_extra.c"
    "src/overlays/effects/ovl_Effect_Ss_Extra/z_eff_ss_extra.h"
    "src/overlays/effects/ovl_Effect_Ss_Fcircle/z_eff_ss_fcircle.c"
    "src/overlays/effects/ovl_Effect_Ss_Fcircle/z_eff_ss_fcircle.h"
    "src/overlays/effects/ovl_Effect_Ss_Fhg_Flash/z_eff_ss_fhg_flash.c"
    "src/overlays/effects/ovl_Effect_Ss_Fhg_Flash/z_eff_ss_fhg_flash.h"
    "src/overlays/effects/ovl_Effect_Ss_Fire_Tail/z_eff_ss_fire_tail.c"
    "src/overlays/effects/ovl_Effect_Ss_Fire_Tail/z_eff_ss_fire_tail.h"
    "src/overlays/effects/ovl_Effect_Ss_G_Fire/z_eff_ss_g_fire.c"
    "src/overlays/effects/ovl_Effect_Ss_G_Fire/z_eff_ss_g_fire.h"
    "src/overlays/effects/ovl_Effect_Ss_G_Magma/z_eff_ss_g_magma.c"
    "src/overlays/effects/ovl_Effect_Ss_G_Magma/z_eff_ss_g_magma.h"
    "src/overlays/effects/ovl_Effect_Ss_G_Magma2/z_eff_ss_g_magma2.c"
    "src/overlays/effects/ovl_Effect_Ss_G_Magma2/z_eff_ss_g_magma2.h"
    "src/overlays/effects/ovl_Effect_Ss_G_Ripple/z_eff_ss_g_ripple.c"
    "src/overlays/effects/ovl_Effect_Ss_G_Ripple/z_eff_ss_g_ripple.h"
    "src/overlays/effects/ovl_Effect_Ss_G_Spk/z_eff_ss_g_spk.c"
    "src/overlays/effects/ovl_Effect_Ss_G_Spk/z_eff_ss_g_spk.h"
    "src/overlays/effects/ovl_Effect_Ss_G_Splash/z_eff_ss_g_splash.c"
    "src/overlays/effects/ovl_Effect_Ss_G_Splash/z_eff_ss_g_splash.h"
    "src/overlays/effects/ovl_Effect_Ss_Hahen/z_eff_ss_hahen.c"
    "src/overlays/effects/ovl_Effect_Ss_Hahen/z_eff_ss_hahen.h"
    "src/overlays/effects/ovl_Effect_Ss_HitMark/z_eff_ss_hitmark.c"
    "src/overlays/effects/ovl_Effect_Ss_HitMark/z_eff_ss_hitmark.h"
    "src/overlays/effects/ovl_Effect_Ss_Ice_Piece/z_eff_ss_ice_piece.c"
    "src/overlays/effects/ovl_Effect_Ss_Ice_Piece/z_eff_ss_ice_piece.h"
    "src/overlays/effects/ovl_Effect_Ss_Ice_Smoke/z_eff_ss_ice_smoke.c"
    "src/overlays/effects/ovl_Effect_Ss_Ice_Smoke/z_eff_ss_ice_smoke.h"
    "src/overlays/effects/ovl_Effect_Ss_K_Fire/z_eff_ss_k_fire.c"
    "src/overlays/effects/ovl_Effect_Ss_K_Fire/z_eff_ss_k_fire.h"
    "src/overlays/effects/ovl_Effect_Ss_Kakera/z_eff_ss_kakera.c"
    "src/overlays/effects/ovl_Effect_Ss_Kakera/z_eff_ss_kakera.h"
    "src/overlays/effects/ovl_Effect_Ss_KiraKira/z_eff_ss_kirakira.c"
    "src/overlays/effects/ovl_Effect_Ss_KiraKira/z_eff_ss_kirakira.h"
    "src/overlays/effects/ovl_Effect_Ss_Lightning/z_eff_ss_lightning.c"
    "src/overlays/effects/ovl_Effect_Ss_Lightning/z_eff_ss_lightning.h"
    "src/overlays/effects/ovl_Effect_Ss_Sibuki/z_eff_ss_sibuki.c"
    "src/overlays/effects/ovl_Effect_Ss_Sibuki/z_eff_ss_sibuki.h"
    "src/overlays/effects/ovl_Effect_Ss_Sibuki2/z_eff_ss_sibuki2.c"
    "src/overlays/effects/ovl_Effect_Ss_Sibuki2/z_eff_ss_sibuki2.h"
    "src/overlays/effects/ovl_Effect_Ss_Solder_Srch_Ball/z_eff_ss_solder_srch_ball.c"
    "src/overlays/effects/ovl_Effect_Ss_Solder_Srch_Ball/z_eff_ss_solder_srch_ball.h"
    "src/overlays/effects/ovl_Effect_Ss_Stick/z_eff_ss_stick.c"
    "src/overlays/effects/ovl_Effect_Ss_Stick/z_eff_ss_stick.h"
    "src/overlays/effects/ovl_Effect_Ss_Stone1/z_eff_ss_stone1.c"
    "src/overlays/effects/ovl_Effect_Ss_Stone1/z_eff_ss_stone1.h"
)
source_group("Source Files\\src\\overlays\\effects" FILES ${Source_Files__src__overlays__effects})

set(Source_Files__src__overlays__gamestates__ovl_file_choose
    "src/overlays/gamestates/ovl_file_choose/z_file_choose.c"
    "src/overlays/gamestates/ovl_file_choose/z_file_copy_erase.c"
    "src/overlays/gamestates/ovl_file_choose/z_file_nameset_data.c"
    "src/overlays/gamestates/ovl_file_choose/z_file_nameset_PAL.c"
)
source_group("Source Files\\src\\overlays\\gamestates\\ovl_file_choose" FILES ${Source_Files__src__overlays__gamestates__ovl_file_choose})

set(Source_Files__src__overlays__gamestates__ovl_opening
    "src/overlays/gamestates/ovl_opening/z_opening.c"
)
source_group("Source Files\\src\\overlays\\gamestates\\ovl_opening" FILES ${Source_Files__src__overlays__gamestates__ovl_opening})

set(Source_Files__src__overlays__gamestates__ovl_select
    "src/overlays/gamestates/ovl_select/z_select.c"
)
source_group("Source Files\\src\\overlays\\gamestates\\ovl_select" FILES ${Source_Files__src__overlays__gamestates__ovl_select})

set(Source_Files__src__overlays__gamestates__ovl_title
    "src/overlays/gamestates/ovl_title/z_title.c"
)
source_group("Source Files\\src\\overlays\\gamestates\\ovl_title" FILES ${Source_Files__src__overlays__gamestates__ovl_title})

set(Source_Files__src__overlays__misc__ovl_kaleido_scope
    #"src/overlays/actors/ovl_kaleido_scope/z_kaleido_scope.h"
    "src/overlays/misc/ovl_kaleido_scope/z_kaleido_collect.c"
    "src/overlays/misc/ovl_kaleido_scope/z_kaleido_debug.c"
    "src/overlays/misc/ovl_kaleido_scope/z_kaleido_equipment.c"
    "src/overlays/misc/ovl_kaleido_scope/z_kaleido_item.c"
    "src/overlays/misc/ovl_kaleido_scope/z_kaleido_map_PAL.c"
    "src/overlays/misc/ovl_kaleido_scope/z_kaleido_prompt.c"
    "src/overlays/misc/ovl_kaleido_scope/z_kaleido_scope.h"
    "src/overlays/misc/ovl_kaleido_scope/z_kaleido_scope_PAL.c"
    "src/overlays/misc/ovl_kaleido_scope/z_lmap_mark.c"
    "src/overlays/misc/ovl_kaleido_scope/z_lmap_mark_data.c"
)
source_group("Source Files\\src\\overlays\\misc\\ovl_kaleido_scope" FILES ${Source_Files__src__overlays__misc__ovl_kaleido_scope})

set(Source_Files__src__overlays__misc__ovl_map_mark_data
    "src/overlays/misc/ovl_map_mark_data/z_map_mark_data.c"
)
source_group("Source Files\\src\\overlays\\misc\\ovl_map_mark_data" FILES ${Source_Files__src__overlays__misc__ovl_map_mark_data})

set(ALL_FILES
    ${Header_Files}
    ${Header_Files__include}
    ${Header_Files__soh__Enhancements}
    ${Header_Files__soh__Enhancements__controls}
    ${Header_Files__soh__Enhancements__cosmetics}
    ${Header_Files__soh__Enhancements__debugger}
    ${Header_Files__soh__Enhancements__randomizer}
    ${Header_Files__soh__Enhancements__randomizer__3drando}
    ${Header_Files__soh__Enhancements__item_tables}
    ${Header_Files__soh__Enhancements__custom_message}
    ${Header_Files__soh__Enhancements__crowd_control}
    ${Source_Files__soh}
    ${Source_Files__soh__Enhancements}
    ${Source_Files__soh__Enhancements__controls}
    ${Source_Files__soh__Enhancements__cosmetics}
    ${Source_Files__soh__Enhancements__debugger}
    ${Source_Files__soh__Enhancements__randomizer}
    ${Source_Files__soh__Enhancements__randomizer__3drando}
    ${Source_Files__soh__Enhancements__randomizer__3drando__hint_list}
    ${Source_Files__soh__Enhancements__randomizer__3drando__location_access}
    ${Source_Files__soh__Enhancements__item_tables}
    ${Source_Files__soh__Enhancements__custom_message}
    ${Source_Files__soh__Enhancements__crowd_control}
    ${Source_Files__src__boot}
    ${Source_Files__src__buffers}
    ${Source_Files__src__code}
    ${Source_Files__src__libultra}
    ${Source_Files__src__overlays__actors}
    ${Source_Files__src__overlays__effects}
    ${Source_Files__src__overlays__gamestates__ovl_file_choose}
    ${Source_Files__src__overlays__gamestates__ovl_opening}
    ${Source_Files__src__overlays__gamestates__ovl_select}
    ${Source_Files__src__overlays__gamestates__ovl_title}
    ${Source_Files__src__overlays__misc__ovl_kaleido_scope}
    ${Source_Files__src__overlays__misc__ovl_map_mark_data}
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
# Compile definitions
################################################################################
find_package(SDL2)
set(SDL2-INCLUDE ${SDL2_INCLUDE_DIRS})

if (CMAKE_SYSTEM_NAME STREQUAL "Windows")
    find_package(SDL2-net)
    set(SDL2-NET-INCLUDE ${SDL_NET_INCLUDE_DIRS})
endif()

target_include_directories(${PROJECT_NAME} PRIVATE assets
	${CMAKE_CURRENT_SOURCE_DIR}/include/
	${CMAKE_CURRENT_SOURCE_DIR}/src/
    ${CMAKE_CURRENT_SOURCE_DIR}/../libultraship
    ${CMAKE_CURRENT_SOURCE_DIR}/../libultraship/libultraship/Lib/
	${CMAKE_CURRENT_SOURCE_DIR}/../libultraship/libultraship/Lib/libjpeg/include/
	${CMAKE_CURRENT_SOURCE_DIR}/../libultraship/libultraship/Lib/spdlog/include/
    ${CMAKE_CURRENT_SOURCE_DIR}/../libultraship/libultraship/Lib/Fast3D/U64/PR
	${CMAKE_CURRENT_SOURCE_DIR}/../ZAPDTR/ZAPDUtils
	${SDL2-INCLUDE}
    ${SDL2-NET-INCLUDE}
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
                /INCREMENTAL:NO
            >
            /DEBUG;
            /SUBSYSTEM:WINDOWS;
            /FORCE:MULTIPLE
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
            "SDL2::SDL2_net;"
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
