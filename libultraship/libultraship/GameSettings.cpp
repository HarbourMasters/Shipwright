﻿#include "GameSettings.h"

// Audio
#include <PR/ultra64/sptask.h>
#include <PR/ultra64/pi.h>
#include <PR/ultra64/message.h>
#include <PR/ultra64/types.h>

#include "ConfigFile.h"
#include "Cvar.h"
#include "GlobalCtx2.h"
#include "SohImGuiImpl.h"
#include "stox.h"
#include "../../soh/include/z64audio.h"
#include <string>
#include "SohHooks.h"

#define ABS(var) var < 0 ? -(var) : var

using namespace Ship;

namespace Game {

    bool DeSyncAudio = false;
    SoHConfigType Settings;
    const std::string ConfSection = DEBUG_SECTION;
    const std::string AudioSection = AUDIO_SECTION;
    const std::string ControllerSection = CONTROLLER_SECTION;
    const std::string EnhancementSection = ENHANCEMENTS_SECTION;
    const std::string CosmeticsSection = COSMETICS_SECTION;

    void UpdateAudio() {
        Audio_SetGameVolume(SEQ_BGM_MAIN, Settings.audio.music_main);
        Audio_SetGameVolume(SEQ_BGM_SUB, Settings.audio.music_sub);
        Audio_SetGameVolume(SEQ_FANFARE, Settings.audio.fanfare);
        Audio_SetGameVolume(SEQ_SFX, Settings.audio.sfx);
    }

    void LoadSettings() {

        const std::shared_ptr<ConfigFile> pConf = GlobalCtx2::GetInstance()->GetConfig();
        ConfigFile& Conf = *pConf;

        // Debug
        SohImGui::console->opened = stob(Conf[ConfSection]["console"]);
        Settings.debug.menu_bar = stob(Conf[ConfSection]["menu_bar"]);
        Settings.debug.soh = stob(Conf[ConfSection]["soh_debug"]);

    	Settings.debug.n64mode = stob(Conf[ConfSection]["n64_mode"]);

        // Enhancements
        Settings.enhancements.fast_text = stob(Conf[EnhancementSection]["fast_text"]);
        CVar_SetS32(const_cast<char*>("gFastText"), Settings.enhancements.fast_text);

        Settings.enhancements.disable_lod = stob(Conf[EnhancementSection]["disable_lod"]);
        CVar_SetS32(const_cast<char*>("gDisableLOD"), Settings.enhancements.disable_lod);

        Settings.enhancements.animated_pause_menu = stob(Conf[EnhancementSection]["animated_pause_menu"]);
        CVar_SetS32(const_cast<char*>("gPauseLiveLink"), Settings.enhancements.animated_pause_menu);

        Settings.enhancements.debug_mode = stob(Conf[EnhancementSection]["debug_mode"]);
        CVar_SetS32(const_cast<char*>("gDebugEnabled"), Settings.enhancements.debug_mode);

        Settings.audio.master = Ship::stof(Conf[AudioSection]["master"]);
        CVar_SetFloat(const_cast<char*>("gGameMasterVolume"), Settings.audio.master);

        Settings.audio.music_main = Ship::stof(Conf[AudioSection]["music_main"]);
        CVar_SetFloat(const_cast<char*>("gMainMusicVolume"), Settings.audio.music_main);

        Settings.audio.music_sub = Ship::stof(Conf[AudioSection]["music_sub"]);
        CVar_SetFloat(const_cast<char*>("gSubMusicVolume"), Settings.audio.music_sub);

        Settings.audio.sfx = Ship::stof(Conf[AudioSection]["sfx"]);
        CVar_SetFloat(const_cast<char*>("gSFXMusicVolume"), Settings.audio.sfx);

        Settings.audio.fanfare = Ship::stof(Conf[AudioSection]["fanfare"]);
        CVar_SetFloat(const_cast<char*>("gFanfareVolume"), Settings.audio.fanfare);

        Settings.controller.gyro_sensitivity = Ship::stof(Conf[ControllerSection]["gyro_sensitivity"]);
        CVar_SetFloat(const_cast<char*>("gGyroSensitivity"), Settings.controller.gyro_sensitivity);

        Settings.controller.rumble_strength = Ship::stof(Conf[ControllerSection]["rumble_strength"]);
        CVar_SetFloat(const_cast<char*>("gRumbleStrength"), Settings.controller.rumble_strength);

        Settings.controller.input_scale = Ship::stof(Conf[ControllerSection]["input_scale"]);
        CVar_SetFloat(const_cast<char*>("gInputScale"), Settings.controller.input_scale);

        Settings.controller.input_enabled = stob(Conf[ControllerSection]["input_enabled"]);
        CVar_SetS32(const_cast<char*>("gInputEnabled"), Settings.controller.input_enabled);
        //Tunics
        Settings.cosmetic.tunic_kokiri_red = (Conf[CosmeticsSection]["tunic_kokiri_red"] != "") ? Ship::stoi(Conf[CosmeticsSection]["tunic_kokiri_red"]) : Settings.cosmetic.tunic_kokiri_red;
        CVar_SetS32(const_cast<char*>("gTunic_Kokiri_Red"), Settings.cosmetic.tunic_kokiri_red);
        Settings.cosmetic.tunic_kokiri_green = (Conf[CosmeticsSection]["tunic_kokiri_green"] != "") ? Ship::stoi(Conf[CosmeticsSection]["tunic_kokiri_green"]) : Settings.cosmetic.tunic_kokiri_green;
        CVar_SetS32(const_cast<char*>("gTunic_Kokiri_Green"), Settings.cosmetic.tunic_kokiri_green);
        Settings.cosmetic.tunic_kokiri_blue = (Conf[CosmeticsSection]["tunic_kokiri_blue"] != "") ? Ship::stoi(Conf[CosmeticsSection]["tunic_kokiri_blue"]) : Settings.cosmetic.tunic_kokiri_blue;
        CVar_SetS32(const_cast<char*>("gTunic_Kokiri_Blue"), Settings.cosmetic.tunic_kokiri_blue);

        Settings.cosmetic.tunic_goron_red = (Conf[CosmeticsSection]["tunic_goron_red"] != "") ? Ship::stoi(Conf[CosmeticsSection]["tunic_goron_red"]) : Settings.cosmetic.tunic_goron_red;
        CVar_SetS32(const_cast<char*>("gTunic_Goron_Red"), Settings.cosmetic.tunic_goron_red);
        Settings.cosmetic.tunic_goron_green = (Conf[CosmeticsSection]["tunic_goron_green"] != "") ? Ship::stoi(Conf[CosmeticsSection]["tunic_goron_green"]) : Settings.cosmetic.tunic_goron_green;
        CVar_SetS32(const_cast<char*>("gTunic_Goron_Green"), Settings.cosmetic.tunic_goron_green);
        Settings.cosmetic.tunic_goron_blue = (Conf[CosmeticsSection]["tunic_goron_blue"] != "") ? Ship::stoi(Conf[CosmeticsSection]["tunic_goron_blue"]) : Settings.cosmetic.tunic_goron_blue;
        CVar_SetS32(const_cast<char*>("gTunic_Goron_Blue"), Settings.cosmetic.tunic_goron_blue);

        Settings.cosmetic.tunic_zora_red = (Conf[CosmeticsSection]["tunic_zora_green"] != "") ? Ship::stoi(Conf[CosmeticsSection]["tunic_zora_red"]) : Settings.cosmetic.tunic_zora_red;
        CVar_SetS32(const_cast<char*>("gTunic_Zora_Red"), Settings.cosmetic.tunic_zora_red);
        Settings.cosmetic.tunic_zora_green = (Conf[CosmeticsSection]["tunic_zora_green"] != "") ? Ship::stoi(Conf[CosmeticsSection]["tunic_zora_green"]) : Settings.cosmetic.tunic_zora_green;
        CVar_SetS32(const_cast<char*>("gTunic_Zora_Green"), Settings.cosmetic.tunic_zora_green);
        Settings.cosmetic.tunic_zora_blue = (Conf[CosmeticsSection]["tunic_zora_blue"] != "" ) ? Ship::stoi(Conf[CosmeticsSection]["tunic_zora_blue"]) : Settings.cosmetic.tunic_zora_blue;
        CVar_SetS32(const_cast<char*>("gTunic_Zora_Blue"), Settings.cosmetic.tunic_zora_blue);
        //Navi
        Settings.cosmetic.navi_idle_inner_red = (Conf[CosmeticsSection]["navi_idle_inner_red"] != "") ? Ship::stoi(Conf[CosmeticsSection]["navi_idle_inner_red"]) : Settings.cosmetic.navi_idle_inner_red;
        CVar_SetS32(const_cast<char*>("gNavi_Idle_Inner_Red"), Settings.cosmetic.navi_idle_inner_red);
        Settings.cosmetic.navi_idle_inner_green = (Conf[CosmeticsSection]["navi_idle_inner_green"] != "") ? Ship::stoi(Conf[CosmeticsSection]["navi_idle_inner_green"]) : Settings.cosmetic.navi_idle_inner_green;
        CVar_SetS32(const_cast<char*>("gNavi_Idle_Inner_Green"), Settings.cosmetic.navi_idle_inner_green);
        Settings.cosmetic.navi_idle_inner_blue = (Conf[CosmeticsSection]["navi_idle_inner_blue"] != "") ? Ship::stoi(Conf[CosmeticsSection]["navi_idle_inner_blue"]) : Settings.cosmetic.navi_idle_inner_blue;
        CVar_SetS32(const_cast<char*>("gNavi_Idle_Inner_Blue"), Settings.cosmetic.navi_idle_inner_blue);
        Settings.cosmetic.navi_idle_outer_red = (Conf[CosmeticsSection]["navi_idle_outer_red"] != "") ? Ship::stoi(Conf[CosmeticsSection]["navi_idle_outer_red"]) : Settings.cosmetic.navi_idle_outer_red;
        CVar_SetS32(const_cast<char*>("gNavi_Idle_Outer_Red"), Settings.cosmetic.navi_idle_outer_red);
        Settings.cosmetic.navi_idle_outer_green = (Conf[CosmeticsSection]["navi_idle_outer_green"] != "") ? Ship::stoi(Conf[CosmeticsSection]["navi_idle_outer_green"]) : Settings.cosmetic.navi_idle_outer_green;
        CVar_SetS32(const_cast<char*>("gNavi_Idle_Outer_Green"), Settings.cosmetic.navi_idle_outer_green);
        Settings.cosmetic.navi_idle_outer_blue = (Conf[CosmeticsSection]["navi_idle_outer_blue"] != "") ? Ship::stoi(Conf[CosmeticsSection]["navi_idle_outer_blue"]) : Settings.cosmetic.navi_idle_outer_blue;
        CVar_SetS32(const_cast<char*>("gNavi_Idle_Outer_Blue"), Settings.cosmetic.navi_idle_outer_blue);

        Settings.cosmetic.navi_npc_inner_red = (Conf[CosmeticsSection]["navi_npc_inner_red"] != "") ? Ship::stoi(Conf[CosmeticsSection]["navi_npc_inner_red"]) : Settings.cosmetic.navi_npc_inner_red;
        CVar_SetS32(const_cast<char*>("gNavi_NPC_Inner_Red"), Settings.cosmetic.navi_npc_inner_red);
        Settings.cosmetic.navi_npc_inner_green = (Conf[CosmeticsSection]["navi_npc_inner_green"] != "") ? Ship::stoi(Conf[CosmeticsSection]["navi_npc_inner_green"]) : Settings.cosmetic.navi_npc_inner_green;
        CVar_SetS32(const_cast<char*>("gNavi_NPC_Inner_Green"), Settings.cosmetic.navi_npc_inner_green);
        Settings.cosmetic.navi_npc_inner_blue = (Conf[CosmeticsSection]["navi_npc_inner_blue"] != "") ? Ship::stoi(Conf[CosmeticsSection]["navi_npc_inner_blue"]) : Settings.cosmetic.navi_npc_inner_blue;
        CVar_SetS32(const_cast<char*>("gNavi_NPC_Inner_Blue"), Settings.cosmetic.navi_npc_inner_blue);
        Settings.cosmetic.navi_npc_outer_red = (Conf[CosmeticsSection]["navi_npc_outer_red"] != "") ? Ship::stoi(Conf[CosmeticsSection]["navi_npc_outer_red"]) : Settings.cosmetic.navi_npc_outer_red;
        CVar_SetS32(const_cast<char*>("gNavi_NPC_Outer_Red"), Settings.cosmetic.navi_npc_outer_red);
        Settings.cosmetic.navi_npc_outer_green = (Conf[CosmeticsSection]["navi_npc_outer_green"] != "") ? Ship::stoi(Conf[CosmeticsSection]["navi_npc_outer_green"]) : Settings.cosmetic.navi_npc_outer_green;
        CVar_SetS32(const_cast<char*>("gNavi_NPC_Outer_Green"), Settings.cosmetic.navi_npc_outer_green);
        Settings.cosmetic.navi_npc_outer_blue = (Conf[CosmeticsSection]["navi_npc_outer_blue"] != "") ? Ship::stoi(Conf[CosmeticsSection]["navi_npc_outer_blue"]) : Settings.cosmetic.navi_npc_outer_blue;
        CVar_SetS32(const_cast<char*>("gNavi_NPC_Outer_Blue"), Settings.cosmetic.navi_npc_outer_blue);

        Settings.cosmetic.navi_enemy_inner_red = (Conf[CosmeticsSection]["navi_enemy_inner_red"] != "") ? Ship::stoi(Conf[CosmeticsSection]["navi_enemy_inner_red"]) : Settings.cosmetic.navi_enemy_inner_red;
        CVar_SetS32(const_cast<char*>("gNavi_Enemy_Inner_Red"), Settings.cosmetic.navi_enemy_inner_red);
        Settings.cosmetic.navi_enemy_inner_green = (Conf[CosmeticsSection]["navi_enemy_inner_green"] != "") ? Ship::stoi(Conf[CosmeticsSection]["navi_enemy_inner_green"]) : Settings.cosmetic.navi_enemy_inner_green;
        CVar_SetS32(const_cast<char*>("gNavi_Enemy_Inner_Green"), Settings.cosmetic.navi_enemy_inner_green);
        Settings.cosmetic.navi_enemy_inner_blue = (Conf[CosmeticsSection]["navi_enemy_inner_blue"] != "") ? Ship::stoi(Conf[CosmeticsSection]["navi_enemy_inner_blue"]) : Settings.cosmetic.navi_enemy_inner_blue;
        CVar_SetS32(const_cast<char*>("gNavi_Enemy_Inner_Blue"), Settings.cosmetic.navi_enemy_inner_blue);
        Settings.cosmetic.navi_enemy_outer_red = (Conf[CosmeticsSection]["navi_enemy_outer_red"] != "") ? Ship::stoi(Conf[CosmeticsSection]["navi_enemy_outer_red"]) : Settings.cosmetic.navi_enemy_outer_red;
        CVar_SetS32(const_cast<char*>("gNavi_Enemy_Outer_Red"), Settings.cosmetic.navi_enemy_outer_red);
        Settings.cosmetic.navi_enemy_outer_green = (Conf[CosmeticsSection]["navi_enemy_outer_green"] != "") ? Ship::stoi(Conf[CosmeticsSection]["navi_enemy_outer_green"]) : Settings.cosmetic.navi_enemy_outer_green;
        CVar_SetS32(const_cast<char*>("gNavi_Enemy_Outer_Green"), Settings.cosmetic.navi_enemy_outer_green);
        Settings.cosmetic.navi_enemy_outer_blue = (Conf[CosmeticsSection]["navi_enemy_outer_blue"] != "") ? Ship::stoi(Conf[CosmeticsSection]["navi_enemy_outer_blue"]) : Settings.cosmetic.navi_enemy_outer_blue;
        CVar_SetS32(const_cast<char*>("gNavi_Enemy_Outer_Blue"), Settings.cosmetic.navi_enemy_outer_blue);

        Settings.cosmetic.navi_prop_inner_red = (Conf[CosmeticsSection]["navi_prop_inner_red"] != "") ? Ship::stoi(Conf[CosmeticsSection]["navi_prop_inner_red"]) : Settings.cosmetic.navi_prop_inner_red;
        CVar_SetS32(const_cast<char*>("gNavi_Prop_Inner_Red"), Settings.cosmetic.navi_prop_inner_red);
        Settings.cosmetic.navi_prop_inner_green = (Conf[CosmeticsSection]["navi_prop_inner_green"] != "") ? Ship::stoi(Conf[CosmeticsSection]["navi_prop_inner_green"]) : Settings.cosmetic.navi_prop_inner_green;
        CVar_SetS32(const_cast<char*>("gNavi_Prop_Inner_Green"), Settings.cosmetic.navi_prop_inner_green);
        Settings.cosmetic.navi_prop_inner_blue = (Conf[CosmeticsSection]["navi_prop_inner_blue"] != "") ? Ship::stoi(Conf[CosmeticsSection]["navi_prop_inner_blue"]) : Settings.cosmetic.navi_prop_inner_blue;
        CVar_SetS32(const_cast<char*>("gNavi_Prop_Inner_Blue"), Settings.cosmetic.navi_prop_inner_blue);
        Settings.cosmetic.navi_prop_outer_red = (Conf[CosmeticsSection]["navi_prop_outer_red"] != "") ? Ship::stoi(Conf[CosmeticsSection]["navi_prop_outer_red"]) : Settings.cosmetic.navi_prop_outer_red;
        CVar_SetS32(const_cast<char*>("gNavi_Prop_Outer_Red"), Settings.cosmetic.navi_prop_outer_red);
        Settings.cosmetic.navi_prop_outer_green = (Conf[CosmeticsSection]["navi_prop_outer_green"] != "") ? Ship::stoi(Conf[CosmeticsSection]["navi_prop_outer_green"]) : Settings.cosmetic.navi_prop_outer_green;
        CVar_SetS32(const_cast<char*>("gNavi_Prop_Outer_Green"), Settings.cosmetic.navi_prop_outer_green);
        Settings.cosmetic.navi_prop_outer_blue = (Conf[CosmeticsSection]["navi_prop_outer_blue"] != "") ? Ship::stoi(Conf[CosmeticsSection]["navi_prop_outer_blue"]) : Settings.cosmetic.navi_prop_outer_blue;
        CVar_SetS32(const_cast<char*>("gNavi_Prop_Outer_Blue"), Settings.cosmetic.navi_prop_outer_blue);



        UpdateAudio();
    }

    void SaveSettings() {
	    const std::shared_ptr<ConfigFile> pConf = GlobalCtx2::GetInstance()->GetConfig();
        ConfigFile& Conf = *pConf;

        // Debug
        Conf[ConfSection]["console"] = std::to_string(SohImGui::console->opened);
        Conf[ConfSection]["menu_bar"] = std::to_string(Settings.debug.menu_bar);
        Conf[ConfSection]["soh_debug"] = std::to_string(Settings.debug.soh);
        Conf[ConfSection]["n64_mode"] = std::to_string(Settings.debug.n64mode);

        // Audio
        Conf[AudioSection]["master"] = std::to_string(Settings.audio.master);
        Conf[AudioSection]["music_main"] = std::to_string(Settings.audio.music_main);
        Conf[AudioSection]["music_sub"] = std::to_string(Settings.audio.music_sub);
        Conf[AudioSection]["sfx"] = std::to_string(Settings.audio.sfx);
        Conf[AudioSection]["fanfare"] = std::to_string(Settings.audio.fanfare);

        // Enhancements
        Conf[EnhancementSection]["fast_text"] = std::to_string(Settings.enhancements.fast_text);
        Conf[EnhancementSection]["disable_lod"] = std::to_string(Settings.enhancements.disable_lod);
        Conf[EnhancementSection]["animated_pause_menu"] = std::to_string(Settings.enhancements.animated_pause_menu);
        Conf[EnhancementSection]["debug_mode"] = std::to_string(Settings.enhancements.debug_mode);

        Conf[ControllerSection]["gyro_sensitivity"] = std::to_string(Settings.controller.gyro_sensitivity);
        Conf[ControllerSection]["rumble_strength"]  = std::to_string(Settings.controller.rumble_strength);
        Conf[ControllerSection]["input_scale"]   = std::to_string(Settings.controller.input_scale);
        Conf[ControllerSection]["input_enabled"] = std::to_string(Settings.controller.input_enabled);

        // Cosmetics 
        Conf[CosmeticsSection]["tunic_kokiri_red"] = std::to_string(Settings.cosmetic.tunic_kokiri_red);
        Conf[CosmeticsSection]["tunic_kokiri_green"] = std::to_string(Settings.cosmetic.tunic_kokiri_green);
        Conf[CosmeticsSection]["tunic_kokiri_blue"] = std::to_string(Settings.cosmetic.tunic_kokiri_blue);

        Conf[CosmeticsSection]["tunic_goron_red"] = std::to_string(Settings.cosmetic.tunic_goron_red);
        Conf[CosmeticsSection]["tunic_goron_green"] = std::to_string(Settings.cosmetic.tunic_goron_green);
        Conf[CosmeticsSection]["tunic_goron_blue"] = std::to_string(Settings.cosmetic.tunic_goron_blue);

        Conf[CosmeticsSection]["tunic_zora_red"] = std::to_string(Settings.cosmetic.tunic_zora_red);
        Conf[CosmeticsSection]["tunic_zora_green"] = std::to_string(Settings.cosmetic.tunic_zora_green);
        Conf[CosmeticsSection]["tunic_zora_blue"] = std::to_string(Settings.cosmetic.tunic_zora_blue);

        Conf[CosmeticsSection]["navi_idle_inner_red"] = std::to_string(Settings.cosmetic.navi_idle_inner_red);
        Conf[CosmeticsSection]["navi_idle_inner_green"] = std::to_string(Settings.cosmetic.navi_idle_inner_green);
        Conf[CosmeticsSection]["navi_idle_inner_blue"] = std::to_string(Settings.cosmetic.navi_idle_inner_blue);
        Conf[CosmeticsSection]["navi_idle_outer_red"] = std::to_string(Settings.cosmetic.navi_idle_outer_red);
        Conf[CosmeticsSection]["navi_idle_outer_green"] = std::to_string(Settings.cosmetic.navi_idle_outer_green);
        Conf[CosmeticsSection]["navi_idle_outer_blue"] = std::to_string(Settings.cosmetic.navi_idle_outer_blue);

        Conf[CosmeticsSection]["navi_npc_inner_red"] = std::to_string(Settings.cosmetic.navi_npc_inner_red);
        Conf[CosmeticsSection]["navi_npc_inner_green"] = std::to_string(Settings.cosmetic.navi_npc_inner_green);
        Conf[CosmeticsSection]["navi_npc_inner_blue"] = std::to_string(Settings.cosmetic.navi_npc_inner_blue);
        Conf[CosmeticsSection]["navi_npc_outer_red"] = std::to_string(Settings.cosmetic.navi_npc_outer_red);
        Conf[CosmeticsSection]["navi_npc_outer_green"] = std::to_string(Settings.cosmetic.navi_npc_outer_green);
        Conf[CosmeticsSection]["navi_npc_outer_blue"] = std::to_string(Settings.cosmetic.navi_npc_outer_blue);

        Conf[CosmeticsSection]["navi_enemy_inner_red"] = std::to_string(Settings.cosmetic.navi_enemy_inner_red);
        Conf[CosmeticsSection]["navi_enemy_inner_green"] = std::to_string(Settings.cosmetic.navi_enemy_inner_green);
        Conf[CosmeticsSection]["navi_enemy_inner_blue"] = std::to_string(Settings.cosmetic.navi_enemy_inner_blue);
        Conf[CosmeticsSection]["navi_enemy_outer_red"] = std::to_string(Settings.cosmetic.navi_enemy_outer_red);
        Conf[CosmeticsSection]["navi_enemy_outer_green"] = std::to_string(Settings.cosmetic.navi_enemy_outer_green);
        Conf[CosmeticsSection]["navi_enemy_outer_blue"] = std::to_string(Settings.cosmetic.navi_enemy_outer_blue);

        Conf[CosmeticsSection]["navi_prop_inner_red"] = std::to_string(Settings.cosmetic.navi_prop_inner_red);
        Conf[CosmeticsSection]["navi_prop_inner_green"] = std::to_string(Settings.cosmetic.navi_prop_inner_green);
        Conf[CosmeticsSection]["navi_prop_inner_blue"] = std::to_string(Settings.cosmetic.navi_prop_inner_blue);
        Conf[CosmeticsSection]["navi_prop_outer_red"] = std::to_string(Settings.cosmetic.navi_prop_outer_red);
        Conf[CosmeticsSection]["navi_prop_outer_green"] = std::to_string(Settings.cosmetic.navi_prop_outer_green);
        Conf[CosmeticsSection]["navi_prop_outer_blue"] = std::to_string(Settings.cosmetic.navi_prop_outer_blue);

        Conf.Save();
    }

    void InitSettings() {
        ModInternal::registerHookListener({ AUDIO_INIT, [](HookEvent ev) {
            UpdateAudio();
        }});
    }

    void SetSeqPlayerVolume(SeqPlayers playerId, float volume) {
        Audio_SetGameVolume(playerId, volume);
    }
}