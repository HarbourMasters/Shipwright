#include "bootcommands.h"
#include "gameconsole.h"
#include <macros.h>
#include <z64.h>
#include <ultra64.h>
#include <functions.h>
#include <variables.h>
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
    CVar_RegisterS32("gDisableLOD", 0);
    CVar_RegisterS32("gDebugEnabled", 0);
    CVar_RegisterS32("gPauseLiveLink", 0);
    CVar_RegisterS32("gMinimalUI", 0);
    CVar_RegisterS32("gRumbleEnabled", 0);
    CVar_RegisterS32("gUniformLR", 1);
    CVar_RegisterS32("gNewDrops", 0);
    CVar_RegisterS32("gVisualAgony", 0);
    CVar_RegisterS32("gLanguages", 0); //0 = English / 1 = German / 2 = French
    CVar_RegisterS32("gHudColors", 1); //0 = N64 / 1 = NGC / 2 = Custom
    CVar_RegisterS32("gUseNaviCol", 0);
    CVar_RegisterS32("gUseTunicsCol", 0);
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

    for (i = 0; i < argc; i++)
        DebugArena_Free(argv[i]);

    DebugArena_Free(argv);
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
