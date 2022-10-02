#include "bootcommands.h"
#include "gameconsole.h"
#include <macros.h>
#include <z64.h>
#include <ultra64.h>
#include <functions.h>
#include <variables.h>
#include <string.h>
#include <stdarg.h>
#include <z64.h>
#include <ultra64/gbi.h>
#include <ultra64/gs2dex.h>
#include <ultra64/controller.h>

uint8_t gLoadFileSelect = 0, gSkipLogoTest = 0;

extern BootCommandFunc BootCommands_Command_SkipLogo(char** argv, s32 argc);
extern BootCommandFunc BootCommands_Command_LoadFileSelect(char** argv, s32 argc);

static BootCommand sCommands[] = { { "--skiplogo", BootCommands_Command_SkipLogo },
                                   { "--loadfileselect", BootCommands_Command_LoadFileSelect } };

void BootCommands_Init()
{
    CVar_RegisterS32("gDebugEnabled", 0);
    CVar_RegisterS32("gLanguages", 0); //0 = English / 1 = German / 2 = French
    CVar_RegisterS32("gHudColors", 1); //0 = N64 / 1 = NGC / 2 = Custom
	CVar_RegisterS32("gInvertYAxis", 1);
    CVar_RegisterS32("gTrailDuration", 4); // 4 = Default trail duration
    if (ResourceMgr_IsGameMasterQuest()) {
        CVar_SetS32("gRandomizer", 0);
    } else {
        CVar_RegisterS32("gRandomizer", 0);
    }
#if defined(__SWITCH__) || defined(__WIIU__)
    CVar_RegisterS32("gControlNav", 1); // always enable controller nav on switch/wii u
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
