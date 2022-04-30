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
#include "../../soh/soh/Enhancements/debugconsole.h"

#include "Window.h"

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
    const std::string CheatSection = CHEATS_SECTION;


    void UpdateAudio() {
        Audio_SetGameVolume(SEQ_BGM_MAIN, CVar_GetFloat("gMainMusicVolume", 1));
        Audio_SetGameVolume(SEQ_BGM_SUB, CVar_GetFloat("gSubMusicVolume", 1));
        Audio_SetGameVolume(SEQ_FANFARE, CVar_GetFloat("gSFXMusicVolume", 1));
        Audio_SetGameVolume(SEQ_SFX, CVar_GetFloat("gFanfareVolume", 1));
    }

    void LoadSettings() {
        const std::shared_ptr<ConfigFile> pConf = GlobalCtx2::GetInstance()->GetConfig();
        ConfigFile& Conf = *pConf;

        // Debug
        SohImGui::console->opened = stob(Conf[ConfSection]["console"]);
        Settings.debug.menu_bar = stob(Conf[ConfSection]["menu_bar"]);
        Settings.debug.soh = stob(Conf[ConfSection]["soh_debug"]);

        UpdateAudio();
    }

    void LoadPadSettings() {
        const std::shared_ptr<ConfigFile> pConf = GlobalCtx2::GetInstance()->GetConfig();
        ConfigFile& Conf = *pConf;

        for (const auto& [i, controllers] : Ship::Window::Controllers) {
            for (const auto& controller : controllers) {
                if (auto padConfSection = controller->GetPadConfSection()) {
                }
            }
        }
    }

    void SaveSettings() {
	    const std::shared_ptr<ConfigFile> pConf = GlobalCtx2::GetInstance()->GetConfig();
        ConfigFile& Conf = *pConf;

        // Debug
        Conf[ConfSection]["console"] = std::to_string(SohImGui::console->opened);
        Conf[ConfSection]["menu_bar"] = std::to_string(Settings.debug.menu_bar);
        Conf[ConfSection]["soh_debug"] = std::to_string(Settings.debug.soh);

        Conf.Save();
        DebugConsole_SaveCVars();
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
