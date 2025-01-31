#include "bootcommands.h"
#include "gameconsole.h"
#include <macros.h>
#include <z64.h>
#include <libultraship/libultra.h>
#include <libultraship/bridge.h>
#include <functions.h>
#include <variables.h>
#include <string.h>
#include <stdarg.h>
#include <z64.h>
#include "soh/OTRGlobals.h"
#include "soh/cvar_prefixes.h"

void BootCommands_Init()
{
    // Clears vars to prevent randomizer menu from being disabled
    CVarClear(CVAR_GENERAL("RandoGenerating")); // Clear when a crash happened during rando seed generation
    CVarClear(CVAR_GENERAL("NewSeedGenerated"));
    CVarClear(CVAR_GENERAL("OnFileSelectNameEntry")); // Clear when soh is killed on the file name entry page
    CVarClear(CVAR_GENERAL("BetterDebugWarpScreenMQMode"));
    CVarClear(CVAR_GENERAL("BetterDebugWarpScreenMQModeScene"));
#if defined(__SWITCH__) || defined(__WIIU__)
    CVarRegisterInteger(CVAR_IMGUI_CONTROLLER_NAV, 1); // always enable controller nav on switch/wii u
#endif
}
