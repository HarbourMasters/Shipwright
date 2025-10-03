#include "ConfigUpdaters.h"
#include "ConfigMigrators.h"
#include "soh/Enhancements/audio/AudioCollection.h"

namespace SOH {
ConfigVersion1Updater::ConfigVersion1Updater() : ConfigVersionUpdater(1) {
}
ConfigVersion2Updater::ConfigVersion2Updater() : ConfigVersionUpdater(2) {
}
ConfigVersion3Updater::ConfigVersion3Updater() : ConfigVersionUpdater(3) {
}
ConfigVersion4Updater::ConfigVersion4Updater() : ConfigVersionUpdater(4) {
}

void ConfigVersion1Updater::Update(Ship::Config* conf) {
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
    if (conf->GetInt("Shortcuts.Fullscreen", Ship::KbScancode::LUS_KB_F11) == Ship::KbScancode::LUS_KB_F10) {
        conf->Erase("Shortcuts.Fullscreen");
    }
    if (conf->GetInt("Shortcuts.Console", Ship::KbScancode::LUS_KB_OEM_3) == Ship::KbScancode::LUS_KB_OEM_3) {
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
        CVarSetInteger(CVAR_Z_FIGHTING_MODE, CVarGetInteger("gDirtPathFix", 0));
        CVarClear("gDirtPathFix");
    }
    if (CVarGetInteger("gRandomizedEnemies", 0) != 0) {
        if (CVarGetInteger("gSeededRandomizedEnemies", 0)) {
            CVarSetInteger("gRandomizedEnemies", 2);
        }
    }
    CVarClear("gSeededRandomizedEnemies");
}

void ConfigVersion2Updater::Update(Ship::Config* conf) {
    CVarClearBlock("gAudioEditor.ReplacedSequences");
}

void ConfigVersion3Updater::Update(Ship::Config* conf) {
    conf->EraseBlock("Controllers");

    if (conf->GetNestedJson().contains("CVars") && conf->GetNestedJson()["CVars"].contains("gInjectItemCounts")) {
        CVarClear("gInjectItemCounts");
        CVarSetInteger("gEnhancements.InjectItemCounts.GoldSkulltula", 1);
        CVarSetInteger("gEnhancements.InjectItemCounts.HeartContainer", 1);
        CVarSetInteger("gEnhancements.InjectItemCounts.HeartPiece", 1);
    }

    // Migrate all audio settings to ints
    if (conf->GetNestedJson().contains("CVars") && conf->GetNestedJson()["CVars"].contains("gGameMasterVolume")) {
        CVarSetInteger("gSettings.Volume.Master", (int32_t)(CVarGetFloat("gGameMasterVolume", 1.0f) * 100));
        CVarClear("gGameMasterVolume");
    }
    if (conf->GetNestedJson().contains("CVars") && conf->GetNestedJson()["CVars"].contains("gMainMusicVolume")) {
        CVarSetInteger("gSettings.Volume.MainMusic", (int32_t)(CVarGetFloat("gMainMusicVolume", 1.0f) * 100));
        CVarClear("gMainMusicVolume");
    }
    if (conf->GetNestedJson().contains("CVars") && conf->GetNestedJson()["CVars"].contains("gSubMusicVolume")) {
        CVarSetInteger("gSettings.Volume.SubMusic", (int32_t)(CVarGetFloat("gSubMusicVolume", 1.0f) * 100));
        CVarClear("gSubMusicVolume");
    }
    if (conf->GetNestedJson().contains("CVars") && conf->GetNestedJson()["CVars"].contains("gSFXMusicVolume")) {
        CVarSetInteger("gSettings.Volume.SFX", (int32_t)(CVarGetFloat("gSFXMusicVolume", 1.0f) * 100));
        CVarClear("gSFXMusicVolume");
    }
    if (conf->GetNestedJson().contains("CVars") && conf->GetNestedJson()["CVars"].contains("gFanfareVolume")) {
        CVarSetInteger("gSettings.Volume.Fanfare", (int32_t)(CVarGetFloat("gFanfareVolume", 1.0f) * 100));
        CVarClear("gFanfareVolume");
    }

    for (Migration migration : version3Migrations) {
        if (migration.action == MigrationAction::Rename) {
            CVarCopy(migration.from.c_str(), migration.to.value().c_str());
        }
        CVarClear(migration.from.c_str());
    }
}

void ConfigVersion4Updater::Update(Ship::Config* conf) {
    for (Migration migration : version4Migrations) {
        if (migration.action == MigrationAction::Rename) {
            CVarCopy(migration.from.c_str(), migration.to.value().c_str());
        }
        CVarClear(migration.from.c_str());
    }
}
} // namespace SOH
