#include "GameSettings.h"

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
    const std::string CheatSection = CHEATS_SECTION;

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

        // Audio
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

        // Controllers
        Settings.controller.gyro_sensitivity = Ship::stof(Conf[ControllerSection]["gyro_sensitivity"]);
        CVar_SetFloat(const_cast<char*>("gGyroSensitivity"), Settings.controller.gyro_sensitivity);

        Settings.controller.rumble_strength = Ship::stof(Conf[ControllerSection]["rumble_strength"]);
        CVar_SetFloat(const_cast<char*>("gRumbleStrength"), Settings.controller.rumble_strength);

        Settings.controller.input_scale = Ship::stof(Conf[ControllerSection]["input_scale"]);
        CVar_SetFloat(const_cast<char*>("gInputScale"), Settings.controller.input_scale);

        Settings.controller.input_enabled = stob(Conf[ControllerSection]["input_enabled"]);
        CVar_SetS32(const_cast<char*>("gInputEnabled"), Settings.controller.input_enabled);
        
        // Cheats
        Settings.cheats.debug_mode = stob(Conf[CheatSection]["debug_mode"]);
        CVar_SetS32(const_cast<char*>("gDebugEnabled"), Settings.cheats.debug_mode);

        Settings.cheats.infinite_money = stob(Conf[CheatSection]["infinite_money"]);
        CVar_SetS32(const_cast<char*>("gInfiniteMoney"), Settings.cheats.infinite_money);

        Settings.cheats.infinite_health = stob(Conf[CheatSection]["infinite_health"]);
        CVar_SetS32(const_cast<char*>("gInfiniteHealth"), Settings.cheats.infinite_health);

        Settings.cheats.infinite_ammo = stob(Conf[CheatSection]["infinite_ammo"]);
        CVar_SetS32(const_cast<char*>("gInfiniteAmmo"), Settings.cheats.infinite_ammo);

        Settings.cheats.infinite_magic = stob(Conf[CheatSection]["infinite_magic"]);
        CVar_SetS32(const_cast<char*>("gInfiniteMagic"), Settings.cheats.infinite_magic);

        Settings.cheats.no_clip = stob(Conf[CheatSection]["no_clip"]);
        CVar_SetS32(const_cast<char*>("gNoClip"), Settings.cheats.no_clip);

        Settings.cheats.climb_everything = stob(Conf[CheatSection]["climb_everything"]);
        CVar_SetS32(const_cast<char*>("gClimbEverything"), Settings.cheats.climb_everything);

        Settings.cheats.moon_jump_on_l = stob(Conf[CheatSection]["moon_jump_on_l"]);
        CVar_SetS32(const_cast<char*>("gMoonJumpOnL"), Settings.cheats.moon_jump_on_l);

        Settings.cheats.super_tunic = stob(Conf[CheatSection]["super_tunic"]);
        CVar_SetS32(const_cast<char*>("gSuperTunic"), Settings.cheats.super_tunic);

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
        

        // Controllers
        Conf[ControllerSection]["gyro_sensitivity"] = std::to_string(Settings.controller.gyro_sensitivity);
        Conf[ControllerSection]["rumble_strength"]  = std::to_string(Settings.controller.rumble_strength);
        Conf[ControllerSection]["input_scale"]   = std::to_string(Settings.controller.input_scale);
        Conf[ControllerSection]["input_enabled"] = std::to_string(Settings.controller.input_enabled);

        // Cheats
        Conf[CheatSection]["debug_mode"] = std::to_string(Settings.cheats.debug_mode);
        Conf[CheatSection]["infinite_money"] = std::to_string(Settings.cheats.infinite_money);
        Conf[CheatSection]["infinite_health"] = std::to_string(Settings.cheats.infinite_health);
        Conf[CheatSection]["infinite_ammo"] = std::to_string(Settings.cheats.infinite_ammo);
        Conf[CheatSection]["infinite_magic"] = std::to_string(Settings.cheats.infinite_magic);
        Conf[CheatSection]["no_clip"] = std::to_string(Settings.cheats.no_clip);
        Conf[CheatSection]["climb_everything"] = std::to_string(Settings.cheats.climb_everything);
        Conf[CheatSection]["moon_jump_on_l"] = std::to_string(Settings.cheats.moon_jump_on_l);
        Conf[CheatSection]["super_tunic"] = std::to_string(Settings.cheats.super_tunic);

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