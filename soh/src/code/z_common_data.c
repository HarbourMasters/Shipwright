#include "global.h"
#include <string.h>

SaveContext gSaveContext;

void SaveContext_Init(void) {
    memset(&gSaveContext, 0, sizeof(gSaveContext));
    gSaveContext.seqId = (u8)NA_BGM_DISABLED;
    gSaveContext.natureAmbienceId = NATURE_ID_DISABLED;
    gSaveContext.forcedSeqId = NA_BGM_GENERAL_SFX;
    gSaveContext.nextCutsceneIndex = CVarGetInteger("gBetaQuestWorld", 0xFFEF);
    gSaveContext.cutsceneTrigger = 0;
    gSaveContext.chamberCutsceneNum = 0;
    gSaveContext.nextDayTime = 0xFFFF;
    gSaveContext.skyboxTime = 0;
    gSaveContext.dogIsLost = true;
    gSaveContext.nextTransitionType = 0xFF;
    gSaveContext.unk_13EE = 50;
    //CUSTOM  - DONT AUTOMATE UNLESS USING ALL CUSTOM ITEMS IN CODE
    gSaveContext.inventory.items2[0] = ITEM_JUMP;
    gSaveContext.inventory.items2[1] = ITEM_GLIDER;
    gSaveContext.inventory.items2[2] = ITEM_LANTERN;
    gSaveContext.inventory.items2[3] = ITEM_ULTRAHAND;
    gSaveContext.inventory.items2[4] = ITEM_ARMCANNON;
    gSaveContext.inventory.items2[5] = ITEM_MASK_FOX;
    for (int i = 6; i < 24; i++) gSaveContext.inventory.items2[i] = ITEM_NONE;
}
