#include "GameSettings.h"

// Audio
#include <cstddef>
#include <PR/ultra64/types.h>
#include <PR/ultra64/sptask.h>
#include <PR/ultra64/pi.h>
#include <PR/ultra64/message.h>

#include "ConfigFile.h"
#include "Cvar.h"
#include "GlobalCtx2.h"
#include "SohImGuiImpl.h"
#include "../../soh/include/z64audio.h"
#include "SohHooks.h"
#include "../../soh/soh/Enhancements/debugconsole.h"

#include "Window.h"
#include "Lib/Fast3D/gfx_rendering_api.h"

#define ABS(var) var < 0 ? -(var) : var

using namespace Ship;

namespace Game {

    bool DeSyncAudio = false;

    void UpdateAudio() {
        Audio_SetGameVolume(SEQ_BGM_MAIN, CVar_GetFloat("gMainMusicVolume", 1));
        Audio_SetGameVolume(SEQ_BGM_SUB, CVar_GetFloat("gSubMusicVolume", 1));
        Audio_SetGameVolume(SEQ_FANFARE, CVar_GetFloat("gSFXMusicVolume", 1));
        Audio_SetGameVolume(SEQ_SFX, CVar_GetFloat("gFanfareVolume", 1));
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

    void LoadSettings() {
        DebugConsole_LoadCVars();
    }

    void SaveSettings() {
        DebugConsole_SaveCVars();
    }

    void InitSettings() {
        ModInternal::registerHookListener({ AUDIO_INIT, [](HookEvent ev) {
            UpdateAudio();
        }});
        ModInternal::registerHookListener({ GFX_INIT, [](HookEvent ev) {
            gfx_get_current_rendering_api()->set_texture_filter((FilteringMode) CVar_GetS32("gTextureFilter", THREE_POINT));
            SohImGui::console->opened = CVar_GetS32("gConsoleEnabled", 0);
            UpdateAudio();
        }});
    }

    void SetSeqPlayerVolume(SeqPlayers playerId, float volume) {
        Audio_SetGameVolume(playerId, volume);
    }
}
