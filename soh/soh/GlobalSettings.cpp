#include "GlobalSettings.h"
#include "z64.h"
#include "variables.h"
#include "core/bridge/consolevariablebridge.h"
#include "core/Window.h"

extern "C" GlobalSettingsStruct gGlobalSettings;

namespace GlobalSettings {

void Init() {
    gGlobalSettings.language = LANGUAGE_ENG;
    gGlobalSettings.zTargetSetting = 0;
    gGlobalSettings.audioSetting = 0;
}

void RegisterCVars() {
    std::shared_ptr<Ship::ConsoleVariable> cvar = Ship::Window::GetInstance()->GetConsoleVariables();

    cvar->RegisterManaged("gLanguages", gGlobalSettings.language);
    cvar->RegisterManaged("gZTargetSetting", gGlobalSettings.zTargetSetting);
    cvar->RegisterManaged("gAudioSetting", gGlobalSettings.audioSetting);
}

} // namespace GlobalSettings
