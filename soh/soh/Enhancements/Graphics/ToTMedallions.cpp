#include <libultraship/bridge.h>
#include "soh/Enhancements/game-interactor/GameInteractor_Hooks.h"
#include "soh/ShipInit.hpp"
#include "functions.h"
#include "macros.h"
#include "variables.h"

extern "C" PlayState* gPlayState;

static constexpr int32_t CVAR_TOT_MEDALLION_COLORS_DEFAULT = 0;
#define CVAR_TOT_MEDALLION_COLORS_NAME CVAR_ENHANCEMENT("ToTMedallionsColors")
#define CVAR_TOT_MEDALLION_COLORS_VALUE \
    CVarGetInteger(CVAR_TOT_MEDALLION_COLORS_NAME, CVAR_TOT_MEDALLION_COLORS_DEFAULT)

static void PatchToTMedallions();
static void ResetToTMedallions();

void UpdateToTMedallions() {
    if (CVAR_TOT_MEDALLION_COLORS_VALUE) {
        PatchToTMedallions();
    } else {
        ResetToTMedallions();
    }
}

static void PatchToTMedallions() {
    // TODO: Refactor the DemoEffect_UpdateJewelAdult and DemoEffect_UpdateJewelChild from z_demo_effect
    // effects to take effect in there
    ResourceMgr_PatchGfxByName(tokinoma_room_0DL_007A70, "ToTMedallions_StartGrayscale", 7, gsSPGrayscale(true));
    ResourceMgr_PatchGfxByName(tokinoma_room_0DL_007FD0, "ToTMedallions_2_StartGrayscale", 7, gsSPGrayscale(true));

    if (CHECK_QUEST_ITEM(QUEST_MEDALLION_WATER)) {
        ResourceMgr_PatchGfxByName(tokinoma_room_0DL_007A70, "ToTMedallions_MakeBlue", 16,
                                   gsDPSetGrayscaleColor(0, 161, 255, 255));
    } else {
        ResourceMgr_PatchGfxByName(tokinoma_room_0DL_007A70, "ToTMedallions_MakeBlue", 16,
                                   gsDPSetGrayscaleColor(255, 255, 255, 255));
    }

    if (CHECK_QUEST_ITEM(QUEST_MEDALLION_SPIRIT)) {
        ResourceMgr_PatchGfxByName(tokinoma_room_0DL_007A70, "ToTMedallions_MakeOrange", 45,
                                   gsDPSetGrayscaleColor(255, 135, 0, 255));
    } else {
        ResourceMgr_PatchGfxByName(tokinoma_room_0DL_007A70, "ToTMedallions_MakeOrange", 45,
                                   gsDPSetGrayscaleColor(255, 255, 255, 255));
    }

    if (CHECK_QUEST_ITEM(QUEST_MEDALLION_LIGHT)) {
        ResourceMgr_PatchGfxByName(tokinoma_room_0DL_007A70, "ToTMedallions_MakeYellow", 69,
                                   gsDPSetGrayscaleColor(255, 255, 0, 255));
        ResourceMgr_PatchGfxByName(tokinoma_room_0DL_007FD0, "ToTMedallions_2_MakeYellow", 16,
                                   gsDPSetGrayscaleColor(255, 255, 0, 255));
    } else {
        ResourceMgr_PatchGfxByName(tokinoma_room_0DL_007A70, "ToTMedallions_MakeYellow", 69,
                                   gsDPSetGrayscaleColor(255, 255, 255, 255));
        ResourceMgr_PatchGfxByName(tokinoma_room_0DL_007FD0, "ToTMedallions_2_MakeYellow", 16,
                                   gsDPSetGrayscaleColor(255, 255, 255, 255));
    }

    if (CHECK_QUEST_ITEM(QUEST_MEDALLION_FOREST)) {
        ResourceMgr_PatchGfxByName(tokinoma_room_0DL_007A70, "ToTMedallions_MakeGreen", 94,
                                   gsDPSetGrayscaleColor(0, 255, 0, 255));
    } else {
        ResourceMgr_PatchGfxByName(tokinoma_room_0DL_007A70, "ToTMedallions_MakeGreen", 94,
                                   gsDPSetGrayscaleColor(255, 255, 255, 255));
    }

    if (CHECK_QUEST_ITEM(QUEST_MEDALLION_FIRE)) {
        ResourceMgr_PatchGfxByName(tokinoma_room_0DL_007A70, "ToTMedallions_MakeRed", 118,
                                   gsDPSetGrayscaleColor(255, 0, 0, 255));
    } else {
        ResourceMgr_PatchGfxByName(tokinoma_room_0DL_007A70, "ToTMedallions_MakeRed", 118,
                                   gsDPSetGrayscaleColor(255, 255, 255, 255));
    }

    if (CHECK_QUEST_ITEM(QUEST_MEDALLION_SHADOW)) {
        ResourceMgr_PatchGfxByName(tokinoma_room_0DL_007A70, "ToTMedallions_MakePurple", 142,
                                   gsDPSetGrayscaleColor(212, 0, 255, 255));
        ResourceMgr_PatchGfxByName(tokinoma_room_0DL_007FD0, "ToTMedallions_2_MakePurple", 27,
                                   gsDPSetGrayscaleColor(212, 0, 255, 255));
    } else {
        ResourceMgr_PatchGfxByName(tokinoma_room_0DL_007A70, "ToTMedallions_MakePurple", 142,
                                   gsDPSetGrayscaleColor(255, 255, 255, 255));
        ResourceMgr_PatchGfxByName(tokinoma_room_0DL_007FD0, "ToTMedallions_2_MakePurple", 27,
                                   gsDPSetGrayscaleColor(255, 255, 255, 255));
    }

    ResourceMgr_PatchGfxByName(tokinoma_room_0DL_007A70, "ToTMedallions_EndGrayscaleAndEndDlist", 160,
                               gsSPBranchListOTRFilePath(gEndGrayscaleAndEndDlistDL));
    ResourceMgr_PatchGfxByName(tokinoma_room_0DL_007FD0, "ToTMedallions_2_EndGrayscaleAndEndDlist", 51,
                               gsSPBranchListOTRFilePath(gEndGrayscaleAndEndDlistDL));
}

static void ResetToTMedallions() {
    // Unpatch everything
    ResourceMgr_UnpatchGfxByName(tokinoma_room_0DL_007A70, "ToTMedallions_StartGrayscale");
    ResourceMgr_UnpatchGfxByName(tokinoma_room_0DL_007FD0, "ToTMedallions_2_StartGrayscale");

    ResourceMgr_UnpatchGfxByName(tokinoma_room_0DL_007A70, "ToTMedallions_MakeBlue");
    ResourceMgr_UnpatchGfxByName(tokinoma_room_0DL_007A70, "ToTMedallions_MakeOrange");
    ResourceMgr_UnpatchGfxByName(tokinoma_room_0DL_007A70, "ToTMedallions_MakeYellow");
    ResourceMgr_UnpatchGfxByName(tokinoma_room_0DL_007FD0, "ToTMedallions_2_MakeYellow");
    ResourceMgr_UnpatchGfxByName(tokinoma_room_0DL_007A70, "ToTMedallions_MakeGreen");
    ResourceMgr_UnpatchGfxByName(tokinoma_room_0DL_007A70, "ToTMedallions_MakeRed");
    ResourceMgr_UnpatchGfxByName(tokinoma_room_0DL_007A70, "ToTMedallions_MakePurple");
    ResourceMgr_UnpatchGfxByName(tokinoma_room_0DL_007FD0, "ToTMedallions_2_MakePurple");

    ResourceMgr_UnpatchGfxByName(tokinoma_room_0DL_007A70, "ToTMedallions_EndGrayscaleAndEndDlist");
    ResourceMgr_UnpatchGfxByName(tokinoma_room_0DL_007FD0, "ToTMedallions_2_EndGrayscaleAndEndDlist");
}

static void CheckTempleOfTime(int16_t sceneNum) {
    if (sceneNum != SCENE_TEMPLE_OF_TIME)
        return;
    PatchToTMedallions();
}

static void RegisterToTMedallions() {
    COND_HOOK(OnItemReceive, CVAR_TOT_MEDALLION_COLORS_VALUE, [](GetItemEntry) {
        if (gPlayState)
            CheckTempleOfTime(gPlayState->sceneNum);
    });
    COND_HOOK(OnSceneInit, CVAR_TOT_MEDALLION_COLORS_VALUE, CheckTempleOfTime);
}

static RegisterShipInitFunc initFunc(RegisterToTMedallions, { CVAR_TOT_MEDALLION_COLORS_NAME });
