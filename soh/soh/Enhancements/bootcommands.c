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

uint8_t gLoadFileSelect = 0, gSkipLogoTest = 0;

extern BootCommandFunc BootCommands_Command_SkipLogo(char** argv, s32 argc);
extern BootCommandFunc BootCommands_Command_LoadFileSelect(char** argv, s32 argc);

static BootCommand sCommands[] = { { "--skiplogo", BootCommands_Command_SkipLogo },
                                   { "--loadfileselect", BootCommands_Command_LoadFileSelect } };

void BootCommands_Init()
{
    // Clears vars to prevent randomizer menu from being disabled
    CVarClear("gRandoGenerating"); // Clear when a crash happened during rando seed generation
    CVarClear("gNewSeedGenerated");
    CVarClear("gOnFileSelectNameEntry"); // Clear when soh is killed on the file name entry page
    CVarClear("gBetterDebugWarpScreenMQMode");
    CVarClear("gBetterDebugWarpScreenMQModeScene");
    CVarClear("gCheatEasyPauseBufferLastInputs");
    CVarClear("gCheatEasyPauseBufferTimer");
#if defined(__SWITCH__) || defined(__WIIU__)
    CVarRegisterInteger("gControlNav", 1); // always enable controller nav on switch/wii u
#endif
}

//void BootCommands_ParseBootArgs(char* str)
void BootCommands_ParseBootArgs(s32 argc, char** argv)
{
    s32 i;

    // Parse the commands
    for (i = 0; i < argc; i++) {
        s32 j;

        for (j = 0; j < ARRAY_COUNT(sCommands); j++) {
            if (!strcmp(argv[i], sCommands[j].name)) {
                s32 numArgsProcessed = sCommands[j].func(&argv[i], argc - i);
                i += numArgsProcessed;
                break;
            }
        }
    }
}

/*
 * Command Name: --skiplogo
 * Description: Skips the N64 Logo Screen
 * Arguments: None
 */
BootCommandFunc BootCommands_Command_SkipLogo(char** argv, s32 argc) {
    gSkipLogoTest = 1;
    return 0;
}

/*
 * Command Name: --loadfileselect
 * Description: Loads the file select screen on bootup.
 * Arguments: None
 */
BootCommandFunc BootCommands_Command_LoadFileSelect(char** argv, s32 argc) {
    gLoadFileSelect = 1;
    return 0;
}
