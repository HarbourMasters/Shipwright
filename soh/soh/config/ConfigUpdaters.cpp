#include "ConfigUpdaters.h"
#include "soh/Enhancements/audio/AudioCollection.h"

namespace LUS {
    ConfigVersion1Updater::ConfigVersion1Updater() : ConfigVersionUpdater(1) {}
    ConfigVersion2Updater::ConfigVersion2Updater() : ConfigVersionUpdater(2) {}
    
    void ConfigVersion1Updater::Update(Config* conf) {
        if (conf->GetInt("Window.Width", 640) == 640) {
            conf->Erase("Window.Width");
        }
        if (conf->GetInt("Window.Height", 480) == 480) {
            conf->Erase("Window.Height");
        }
        if (conf->GetInt("Window.PositionX", 100) == 100) {
            conf->Erase("Window.PositionX");
        }
        if (conf->GetInt("Window.PositionY", 100) == 100) {
            conf->Erase("Window.PositionY");
        }
        if (conf->GetString("Window.GfxBackend", "") == "") {
            conf->Erase("Window.GfxBackend");
        }
        if (conf->GetString("Window.GfxApi", "") == "") {
            conf->Erase("Window.GfxApi");
        }
        if (conf->GetString("Window.AudioBackend", "") == "") {
            conf->Erase("Window.AudioBackend");
        }
        if (conf->GetBool("Window.Fullscreen.Enabled", false) == false) {
            conf->Erase("Window.Fullscreen.Enabled");
        }
        if (conf->GetInt("Window.Fullscreen.Width", 1920) == 1920) {
            conf->Erase("Window.Fullscreen.Width");
        }
        if (conf->GetInt("Window.Fullscreen.Height", 1080) == 1080) {
            conf->Erase("Window.Fullscreen.Height");
        }
        if (conf->GetInt("Shortcuts.Fullscreen", KbScancode::LUS_KB_F11) == KbScancode::LUS_KB_F10) {
            conf->Erase("Shortcuts.Fullscreen");
        }
        if (conf->GetInt("Shortcuts.Console", KbScancode::LUS_KB_OEM_3) == KbScancode::LUS_KB_OEM_3) {
            conf->Erase("Shortcuts.Console");
        }
        if (conf->GetString("Game.SaveName", "") == "") {
            conf->Erase("Game.SaveName");
        }
        if (conf->GetString("Game.Main Archive", "") == "") {
            conf->Erase("Game.Main Archive");
        }
        if (conf->GetString("Game.Patches Archive", "") == "") {
            conf->Erase("Game.Patches Archive");
        }
        if (CVarGetInteger("gDirtPathFix", 0) != 0) {
            CVarSetInteger("gZFightingMode", CVarGetInteger("gDirtPathFix", 0));
            CVarClear("gDirtPathFix");
        }
        if (CVarGetInteger("gRandomizedEnemies", 0) != 0) {
            if (CVarGetInteger("gSeededRandomizedEnemies", 0)) {
                CVarSetInteger("gRandomizedEnemies", 2);
            }
        }
        CVarClear("gSeededRandomizedEnemies");
    }

    void ConfigVersion2Updater::Update(Config* conf) {
        for (auto seq : AudioCollection::Instance->GetAllSequences()) {
            CVarClear(std::string("gAudioEditor.ReplacedSequences." + seq.second.sfxKey).c_str());
        }
    }
}
