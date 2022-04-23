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
    const std::string LanguagesSection = LANGUAGES_SECTION;

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
        Settings.enhancements.skip_text = stob(Conf[EnhancementSection]["skip_text"]);
        CVar_SetS32("gSkipText", Settings.enhancements.skip_text);

        Settings.enhancements.text_speed = Ship::stoi(Conf[EnhancementSection]["text_speed"]);
        CVar_SetS32("gTextSpeed", Settings.enhancements.text_speed);

        Settings.enhancements.disable_lod = stob(Conf[EnhancementSection]["disable_lod"]);
        CVar_SetS32("gDisableLOD", Settings.enhancements.disable_lod);

        Settings.enhancements.animated_pause_menu = stob(Conf[EnhancementSection]["animated_pause_menu"]);
        CVar_SetS32("gPauseLiveLink", Settings.enhancements.animated_pause_menu);

        Settings.enhancements.dynamic_wallet_icon = stob(Conf[EnhancementSection]["dynamic_wallet_icon"]);
        CVar_SetS32(const_cast<char*>("gDynamicWalletIcon"), Settings.enhancements.dynamic_wallet_icon);

        Settings.enhancements.minimal_ui = stob(Conf[EnhancementSection]["minimal_ui"]);
        CVar_SetS32("gMinimalUI", Settings.enhancements.minimal_ui);

        Settings.enhancements.visualagony = stob(Conf[EnhancementSection]["visualagony"]);
        CVar_SetS32("gVisualAgony", Settings.enhancements.visualagony);
      
        Settings.enhancements.mm_bunny_hood = stob(Conf[EnhancementSection]["mm_bunny_hood"]);
        CVar_SetS32("gMMBunnyHood", Settings.enhancements.mm_bunny_hood);

        // Audio
        Settings.audio.master = Ship::stof(Conf[AudioSection]["master"]);
        CVar_SetFloat("gGameMasterVolume", Settings.audio.master);

        Settings.audio.music_main = Ship::stof(Conf[AudioSection]["music_main"]);
        CVar_SetFloat("gMainMusicVolume", Settings.audio.music_main);

        Settings.audio.music_sub = Ship::stof(Conf[AudioSection]["music_sub"]);
        CVar_SetFloat("gSubMusicVolume", Settings.audio.music_sub);

        Settings.audio.sfx = Ship::stof(Conf[AudioSection]["sfx"]);
        CVar_SetFloat("gSFXMusicVolume", Settings.audio.sfx);

        Settings.audio.fanfare = Ship::stof(Conf[AudioSection]["fanfare"]);
        CVar_SetFloat("gFanfareVolume", Settings.audio.fanfare);

        // Controllers
        Settings.controller.gyro_sensitivity = Ship::stof(Conf[ControllerSection]["gyro_sensitivity"]);
        CVar_SetFloat("gGyroSensitivity", Settings.controller.gyro_sensitivity);

        Settings.controller.rumble_strength = Ship::stof(Conf[ControllerSection]["rumble_strength"]);
        CVar_SetFloat("gRumbleStrength", Settings.controller.rumble_strength);

        Settings.controller.input_scale = Ship::stof(Conf[ControllerSection]["input_scale"]);
        CVar_SetFloat("gInputScale", Settings.controller.input_scale);

        Settings.controller.input_enabled = stob(Conf[ControllerSection]["input_enabled"]);
        CVar_SetS32("gInputEnabled", Settings.controller.input_enabled);

        Settings.controller.dpad_pause_name = stob(Conf[ControllerSection]["dpad_pause_name"]);
        CVar_SetS32("gDpadPauseName", Settings.controller.dpad_pause_name);

        Settings.controller.dpad_ocarina_text = stob(Conf[ControllerSection]["dpad_ocarina_text"]);
        CVar_SetS32("gDpadOcarinaText", Settings.controller.dpad_ocarina_text);

        Settings.controller.dpad_shop = stob(Conf[ControllerSection]["dpad_shop"]);
        CVar_SetS32("gDpadShop", Settings.controller.dpad_shop);
        
        // Cheats
        Settings.cheats.debug_mode = stob(Conf[CheatSection]["debug_mode"]);
        CVar_SetS32("gDebugEnabled", Settings.cheats.debug_mode);

        Settings.cheats.infinite_money = stob(Conf[CheatSection]["infinite_money"]);
        CVar_SetS32("gInfiniteMoney", Settings.cheats.infinite_money);

        Settings.cheats.infinite_health = stob(Conf[CheatSection]["infinite_health"]);
        CVar_SetS32("gInfiniteHealth", Settings.cheats.infinite_health);

        Settings.cheats.infinite_ammo = stob(Conf[CheatSection]["infinite_ammo"]);
        CVar_SetS32("gInfiniteAmmo", Settings.cheats.infinite_ammo);

        Settings.cheats.infinite_magic = stob(Conf[CheatSection]["infinite_magic"]);
        CVar_SetS32("gInfiniteMagic", Settings.cheats.infinite_magic);

        Settings.cheats.infinite_nayru = stob(Conf[CheatSection]["infinite_nayru"]);
        CVar_SetS32("gInfiniteNayru", Settings.cheats.infinite_nayru);

        Settings.cheats.no_clip = stob(Conf[CheatSection]["no_clip"]);
        CVar_SetS32("gNoClip", Settings.cheats.no_clip);

        Settings.cheats.climb_everything = stob(Conf[CheatSection]["climb_everything"]);
        CVar_SetS32("gClimbEverything", Settings.cheats.climb_everything);

        Settings.cheats.moon_jump_on_l = stob(Conf[CheatSection]["moon_jump_on_l"]);
        CVar_SetS32("gMoonJumpOnL", Settings.cheats.moon_jump_on_l);

        Settings.cheats.super_tunic = stob(Conf[CheatSection]["super_tunic"]);
        CVar_SetS32("gSuperTunic", Settings.cheats.super_tunic);

        Settings.cheats.ez_isg = stob(Conf[CheatSection]["ez_isg"]);
        CVar_SetS32("gEzISG", Settings.cheats.ez_isg);

        Settings.cheats.no_restrict_item = stob(Conf[CheatSection]["no_restrict_item"]);
        CVar_SetS32("gNoRestrictItems", Settings.cheats.no_restrict_item);

        Settings.cheats.freeze_time = stob(Conf[CheatSection]["freeze_time"]);
        CVar_SetS32("gFreezeTime", Settings.cheats.freeze_time);

        Settings.hudcolors.n64_colors = stob(Conf[HUDColorSection]["n64_colors"]);
        CVar_SetS32("gN64Colors", Settings.hudcolors.n64_colors);

        Settings.hudcolors.gc_colors = stob(Conf[HUDColorSection]["gc_colors"]);
        CVar_SetS32("gGameCubeColors", Settings.hudcolors.gc_colors);

        Settings.hudcolors.custom_colors = stob(Conf[HUDColorSection]["custom_colors"]);
        CVar_SetS32("gCustomColors", Settings.hudcolors.custom_colors);
        //hearts main colors
        Settings.hudcolors.ccheartsprimr = (Conf[HUDColorSection]["cc_heartsprimr"] != "") ? Ship::stoi(Conf[HUDColorSection]["cc_heartsprimr"]) : Settings.hudcolors.ccheartsprimr;
        CVar_SetInt("gCCHeartsPrimR", Settings.hudcolors.ccheartsprimr);
        Settings.hudcolors.ccheartsprimg = (Conf[HUDColorSection]["cc_heartsprimr"] != "") ? Ship::stoi(Conf[HUDColorSection]["cc_heartsprimg"]) : Settings.hudcolors.ccheartsprimg;
        CVar_SetInt("gCCHeartsPrimG", Settings.hudcolors.ccheartsprimg);
        Settings.hudcolors.ccheartsprimb = (Conf[HUDColorSection]["cc_heartsprimr"] != "") ? Ship::stoi(Conf[HUDColorSection]["cc_heartsprimb"]) : Settings.hudcolors.ccheartsprimb;
        CVar_SetInt("gCCHeartsPrimB", Settings.hudcolors.ccheartsprimb);
        //hearts double defense main colors
        Settings.hudcolors.ddccheartsprimr = (Conf[HUDColorSection]["cc_ddheartsprimr"] != "") ? Ship::stoi(Conf[HUDColorSection]["cc_ddheartsprimr"]) : Settings.hudcolors.ddccheartsprimr;
        CVar_SetInt("gDDCCHeartsPrimR", Settings.hudcolors.ddccheartsprimr);
        Settings.hudcolors.ddccheartsprimg = (Conf[HUDColorSection]["cc_ddheartsprimg"] != "") ? Ship::stoi(Conf[HUDColorSection]["cc_ddheartsprimg"]) : Settings.hudcolors.ddccheartsprimg;
        CVar_SetInt("gDDCCHeartsPrimG", Settings.hudcolors.ddccheartsprimg);
        Settings.hudcolors.ddccheartsprimb = (Conf[HUDColorSection]["cc_ddheartsprimb"] != "") ? Ship::stoi(Conf[HUDColorSection]["cc_ddheartsprimb"]) : Settings.hudcolors.ddccheartsprimb;
        CVar_SetInt("gDDCCHeartsPrimB", Settings.hudcolors.ddccheartsprimb);
        //a button
        Settings.hudcolors.ccabtnprimr = (Conf[HUDColorSection]["cc_abtnprimr"] != "") ? Ship::stoi(Conf[HUDColorSection]["cc_abtnprimr"]) : Settings.hudcolors.ccabtnprimr;
        CVar_SetInt("gCCABtnPrimR", Settings.hudcolors.ccabtnprimr);
        Settings.hudcolors.ccabtnprimg = (Conf[HUDColorSection]["cc_abtnprimg"] != "") ? Ship::stoi(Conf[HUDColorSection]["cc_abtnprimg"]) : Settings.hudcolors.ccabtnprimg;
        CVar_SetInt("gCCABtnPrimG", Settings.hudcolors.ccabtnprimg);
        Settings.hudcolors.ccabtnprimb = (Conf[HUDColorSection]["cc_abtnprimb"] != "") ? Ship::stoi(Conf[HUDColorSection]["cc_abtnprimb"]) : Settings.hudcolors.ccabtnprimb;
        CVar_SetInt("gCCABtnPrimB", Settings.hudcolors.ccabtnprimb);
        //b button
        Settings.hudcolors.ccbbtnprimr = (Conf[HUDColorSection]["cc_bbtnprimr"] != "") ? Ship::stoi(Conf[HUDColorSection]["cc_bbtnprimr"]) : Settings.hudcolors.ccbbtnprimr;
        CVar_SetInt("gCCBBtnPrimR", Settings.hudcolors.ccbbtnprimr);
        Settings.hudcolors.ccbbtnprimg = (Conf[HUDColorSection]["cc_bbtnprimg"] != "") ? Ship::stoi(Conf[HUDColorSection]["cc_bbtnprimg"]) : Settings.hudcolors.ccbbtnprimg;
        CVar_SetInt("gCCBBtnPrimG", Settings.hudcolors.ccbbtnprimg);
        Settings.hudcolors.ccbbtnprimb = (Conf[HUDColorSection]["cc_bbtnprimb"] != "") ? Ship::stoi(Conf[HUDColorSection]["cc_bbtnprimb"]) : Settings.hudcolors.ccbbtnprimb;
        CVar_SetInt("gCCBBtnPrimB", Settings.hudcolors.ccbbtnprimb);
        //start button
        Settings.hudcolors.ccstartbtnprimr = (Conf[HUDColorSection]["cc_startbtnprimr"] != "") ? Ship::stoi(Conf[HUDColorSection]["cc_startbtnprimr"]) : Settings.hudcolors.ccstartbtnprimr;
        CVar_SetInt("gCCStartBtnPrimR", Settings.hudcolors.ccstartbtnprimr);
        Settings.hudcolors.ccstartbtnprimg = (Conf[HUDColorSection]["cc_startbtnprimg"] != "") ? Ship::stoi(Conf[HUDColorSection]["cc_startbtnprimg"]) : Settings.hudcolors.ccstartbtnprimg;
        CVar_SetInt("gCCStartBtnPrimG", Settings.hudcolors.ccstartbtnprimg);
        Settings.hudcolors.ccstartbtnprimb = (Conf[HUDColorSection]["cc_startbtnprimb"] != "") ? Ship::stoi(Conf[HUDColorSection]["cc_startbtnprimb"]) : Settings.hudcolors.ccstartbtnprimb;
        CVar_SetInt("gCCStartBtnPrimB", Settings.hudcolors.ccstartbtnprimb);
        //c button
        Settings.hudcolors.cccbtnprimr = (Conf[HUDColorSection]["cc_cbtnprimr"] != "") ? Ship::stoi(Conf[HUDColorSection]["cc_cbtnprimr"]) : Settings.hudcolors.cccbtnprimr;
        CVar_SetInt("gCCCBtnPrimR", Settings.hudcolors.cccbtnprimr);
        Settings.hudcolors.cccbtnprimg = (Conf[HUDColorSection]["cc_cbtnprimg"] != "") ? Ship::stoi(Conf[HUDColorSection]["cc_cbtnprimg"]) : Settings.hudcolors.cccbtnprimg;
        CVar_SetInt("gCCCBtnPrimG", Settings.hudcolors.cccbtnprimg);
        Settings.hudcolors.cccbtnprimb = (Conf[HUDColorSection]["cc_cbtnprimb"] != "") ? Ship::stoi(Conf[HUDColorSection]["cc_cbtnprimb"]) : Settings.hudcolors.cccbtnprimb;
        CVar_SetInt("gCCCBtnPrimB", Settings.hudcolors.cccbtnprimb);
        //magic metter borders
        Settings.hudcolors.ccmagicborderprimr = (Conf[HUDColorSection]["cc_magicborderprimr"] != "") ? Ship::stoi(Conf[HUDColorSection]["cc_magicborderprimr"]) : Settings.hudcolors.ccmagicborderprimr;
        CVar_SetInt("gCCMagicBorderPrimR", Settings.hudcolors.ccmagicborderprimr);
        Settings.hudcolors.ccmagicborderprimg = (Conf[HUDColorSection]["cc_magicborderprimg"] != "") ? Ship::stoi(Conf[HUDColorSection]["cc_magicborderprimg"]) : Settings.hudcolors.ccmagicborderprimg;
        CVar_SetInt("gCCMagicBorderPrimG", Settings.hudcolors.ccmagicborderprimg);
        Settings.hudcolors.ccmagicborderprimb = (Conf[HUDColorSection]["cc_magicborderprimb"] != "") ? Ship::stoi(Conf[HUDColorSection]["cc_magicborderprimb"]) : Settings.hudcolors.ccmagicborderprimb;
        CVar_SetInt("gCCMagicBorderPrimB", Settings.hudcolors.ccmagicborderprimb);
        //magic metter remaining 
        Settings.hudcolors.ccmagicprimr = (Conf[HUDColorSection]["cc_magicprimr"] != "") ? Ship::stoi(Conf[HUDColorSection]["cc_magicprimr"]) : Settings.hudcolors.ccmagicprimr;
        CVar_SetInt("gCCMagicPrimR", Settings.hudcolors.ccmagicprimr);
        Settings.hudcolors.ccmagicprimg = (Conf[HUDColorSection]["cc_magicprimg"] != "") ? Ship::stoi(Conf[HUDColorSection]["cc_magicprimg"]) : Settings.hudcolors.ccmagicprimg;
        CVar_SetInt("gCCMagicPrimG", Settings.hudcolors.ccmagicprimg);
        Settings.hudcolors.ccmagicprimb = (Conf[HUDColorSection]["cc_magicprimb"] != "") ? Ship::stoi(Conf[HUDColorSection]["cc_magicprimb"]) : Settings.hudcolors.ccmagicprimb;
        CVar_SetInt("gCCMagicPrimB", Settings.hudcolors.ccmagicprimb);
        //magic metter being used
        Settings.hudcolors.ccmagicuseprimr = (Conf[HUDColorSection]["cc_magicuseprimr"] != "") ? Ship::stoi(Conf[HUDColorSection]["cc_magicuseprimr"]) : Settings.hudcolors.ccmagicuseprimr;
        CVar_SetInt("gCCMagicUsePrimR", Settings.hudcolors.ccmagicuseprimr);
        Settings.hudcolors.ccmagicuseprimg = (Conf[HUDColorSection]["cc_magicuseprimg"] != "") ? Ship::stoi(Conf[HUDColorSection]["cc_magicuseprimg"]) : Settings.hudcolors.ccmagicuseprimg;
        CVar_SetInt("gCCMagicUsePrimG", Settings.hudcolors.ccmagicuseprimg);
        Settings.hudcolors.ccmagicuseprimb = (Conf[HUDColorSection]["cc_magicuseprimb"] != "") ? Ship::stoi(Conf[HUDColorSection]["cc_magicuseprimb"]) : Settings.hudcolors.ccmagicuseprimb;
        CVar_SetInt("gCCMagicUsePrimB", Settings.hudcolors.ccmagicuseprimb);
        //minimap
        Settings.hudcolors.ccminimapprimr = (Conf[HUDColorSection]["cc_minimapprimr"] != "") ? Ship::stoi(Conf[HUDColorSection]["cc_minimapprimr"]) : Settings.hudcolors.ccminimapprimr;
        CVar_SetInt("gCCMinimapPrimR", Settings.hudcolors.ccminimapprimr);
        Settings.hudcolors.ccminimapprimg = (Conf[HUDColorSection]["cc_minimapprimg"] != "") ? Ship::stoi(Conf[HUDColorSection]["cc_minimapprimg"]) : Settings.hudcolors.ccminimapprimg;
        CVar_SetInt("gCCMinimapPrimG", Settings.hudcolors.ccminimapprimg);
        Settings.hudcolors.ccminimapprimb = (Conf[HUDColorSection]["cc_minimapprimb"] != "") ? Ship::stoi(Conf[HUDColorSection]["cc_minimapprimb"]) : Settings.hudcolors.ccminimapprimb;
        CVar_SetInt("gCCMinimapPrimB", Settings.hudcolors.ccminimapprimb);
        //rupee icon
        Settings.hudcolors.ccrupeeprimr = (Conf[HUDColorSection]["cc_rupeeprimr"] != "") ? Ship::stoi(Conf[HUDColorSection]["cc_rupeeprimr"]) : Settings.hudcolors.ccrupeeprimr;
        CVar_SetInt("gCCRupeePrimR", Settings.hudcolors.ccrupeeprimr);
        Settings.hudcolors.ccrupeeprimg = (Conf[HUDColorSection]["cc_rupeeprimg"] != "") ? Ship::stoi(Conf[HUDColorSection]["cc_rupeeprimg"]) : Settings.hudcolors.ccrupeeprimg;
        CVar_SetInt("gCCRupeePrimG", Settings.hudcolors.ccrupeeprimg);
        Settings.hudcolors.ccrupeeprimb = (Conf[HUDColorSection]["cc_rupeeprimb"] != "") ? Ship::stoi(Conf[HUDColorSection]["cc_rupeeprimb"]) : Settings.hudcolors.ccrupeeprimb;
        CVar_SetInt("gCCRupeePrimB", Settings.hudcolors.ccrupeeprimb);

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
        Conf[EnhancementSection]["skip_text"] = std::to_string(Settings.enhancements.skip_text);
        Conf[EnhancementSection]["text_speed"] = std::to_string(Settings.enhancements.text_speed);
        Conf[EnhancementSection]["disable_lod"] = std::to_string(Settings.enhancements.disable_lod);
        Conf[EnhancementSection]["animated_pause_menu"] = std::to_string(Settings.enhancements.animated_pause_menu);
        Conf[EnhancementSection]["dynamic_wallet_icon"] = std::to_string(Settings.enhancements.dynamic_wallet_icon);
        Conf[EnhancementSection]["minimal_ui"] = std::to_string(Settings.enhancements.minimal_ui);
        Conf[EnhancementSection]["visualagony"] = std::to_string(Settings.enhancements.visualagony);
        Conf[EnhancementSection]["mm_bunny_hood"] = std::to_string(Settings.enhancements.mm_bunny_hood);


        // Controllers
        Conf[ControllerSection]["gyro_sensitivity"] = std::to_string(Settings.controller.gyro_sensitivity);
        Conf[ControllerSection]["rumble_strength"]  = std::to_string(Settings.controller.rumble_strength);
        Conf[ControllerSection]["input_scale"]   = std::to_string(Settings.controller.input_scale);
        Conf[ControllerSection]["input_enabled"] = std::to_string(Settings.controller.input_enabled);
        Conf[ControllerSection]["dpad_pause_name"] = std::to_string(Settings.controller.dpad_pause_name);
        Conf[ControllerSection]["dpad_ocarina_text"] = std::to_string(Settings.controller.dpad_ocarina_text);
        Conf[ControllerSection]["dpad_shop"] = std::to_string(Settings.controller.dpad_shop);

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

        //HUD Color
        Conf[HUDColorSection]["n64_colors"] = std::to_string(Settings.hudcolors.n64_colors);
        Conf[HUDColorSection]["gc_colors"] = std::to_string(Settings.hudcolors.gc_colors);
        Conf[HUDColorSection]["custom_colors"] = std::to_string(Settings.hudcolors.custom_colors);
        Conf[HUDColorSection]["cc_heartsprimr"] = std::to_string(Settings.hudcolors.ccheartsprimr);
        Conf[HUDColorSection]["cc_heartsprimg"] = std::to_string(Settings.hudcolors.ccheartsprimg);
        Conf[HUDColorSection]["cc_heartsprimb"] = std::to_string(Settings.hudcolors.ccheartsprimb);
        Conf[HUDColorSection]["cc_ddheartsprimr"] = std::to_string(Settings.hudcolors.ddccheartsprimr);
        Conf[HUDColorSection]["cc_ddheartsprimg"] = std::to_string(Settings.hudcolors.ddccheartsprimg);
        Conf[HUDColorSection]["cc_ddheartsprimb"] = std::to_string(Settings.hudcolors.ddccheartsprimb);
        Conf[HUDColorSection]["cc_abtnprimr"] = std::to_string(Settings.hudcolors.ccabtnprimr);
        Conf[HUDColorSection]["cc_abtnprimg"] = std::to_string(Settings.hudcolors.ccabtnprimg);
        Conf[HUDColorSection]["cc_abtnprimb"] = std::to_string(Settings.hudcolors.ccabtnprimb);
        Conf[HUDColorSection]["cc_bbtnprimr"] = std::to_string(Settings.hudcolors.ccbbtnprimr);
        Conf[HUDColorSection]["cc_bbtnprimg"] = std::to_string(Settings.hudcolors.ccbbtnprimg);
        Conf[HUDColorSection]["cc_bbtnprimb"] = std::to_string(Settings.hudcolors.ccbbtnprimb);
        Conf[HUDColorSection]["cc_cbtnprimr"] = std::to_string(Settings.hudcolors.cccbtnprimr);
        Conf[HUDColorSection]["cc_cbtnprimg"] = std::to_string(Settings.hudcolors.cccbtnprimg);
        Conf[HUDColorSection]["cc_cbtnprimb"] = std::to_string(Settings.hudcolors.cccbtnprimb);
        Conf[HUDColorSection]["cc_startbtnprimr"] = std::to_string(Settings.hudcolors.ccstartbtnprimr);
        Conf[HUDColorSection]["cc_startbtnprimg"] = std::to_string(Settings.hudcolors.ccstartbtnprimg);
        Conf[HUDColorSection]["cc_startbtnprimb"] = std::to_string(Settings.hudcolors.ccstartbtnprimb);
        Conf[HUDColorSection]["cc_magicborderprimr"] = std::to_string(Settings.hudcolors.ccmagicborderprimr);
        Conf[HUDColorSection]["cc_magicborderprimg"] = std::to_string(Settings.hudcolors.ccmagicborderprimg);
        Conf[HUDColorSection]["cc_magicborderprimb"] = std::to_string(Settings.hudcolors.ccmagicborderprimb);
        Conf[HUDColorSection]["cc_magicprimr"] = std::to_string(Settings.hudcolors.ccmagicprimr);
        Conf[HUDColorSection]["cc_magicprimg"] = std::to_string(Settings.hudcolors.ccmagicprimg);
        Conf[HUDColorSection]["cc_magicprimb"] = std::to_string(Settings.hudcolors.ccmagicprimb);
        Conf[HUDColorSection]["cc_magicuseprimr"] = std::to_string(Settings.hudcolors.ccmagicuseprimr);
        Conf[HUDColorSection]["cc_magicuseprimg"] = std::to_string(Settings.hudcolors.ccmagicuseprimg);
        Conf[HUDColorSection]["cc_magicuseprimb"] = std::to_string(Settings.hudcolors.ccmagicuseprimb);
        Conf[HUDColorSection]["cc_minimapprimr"] = std::to_string(Settings.hudcolors.ccminimapprimr);
        Conf[HUDColorSection]["cc_minimapprimg"] = std::to_string(Settings.hudcolors.ccminimapprimg);
        Conf[HUDColorSection]["cc_minimapprimb"] = std::to_string(Settings.hudcolors.ccminimapprimb);
        Conf[HUDColorSection]["cc_rupeeprimr"] = std::to_string(Settings.hudcolors.ccrupeeprimr);
        Conf[HUDColorSection]["cc_rupeeprimg"] = std::to_string(Settings.hudcolors.ccrupeeprimg);
        Conf[HUDColorSection]["cc_rupeeprimb"] = std::to_string(Settings.hudcolors.ccrupeeprimb);

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
