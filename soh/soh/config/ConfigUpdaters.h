#include "libultraship/libultraship.h"

namespace LUS {
class ConfigVersion0Updater : public ConfigVersionUpdater {
    public:
    void Update(std::shared_ptr<Config> conf) {
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
    }
};
}