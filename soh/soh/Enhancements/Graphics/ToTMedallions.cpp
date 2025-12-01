#include "soh/Enhancements/game-interactor/GameInteractor_Hooks.h"
#include "soh/ShipInit.hpp"

extern "C" {
#include "align_asset_macro.h"
#include "macros.h"
#include "variables.h"
#include "soh/ResourceManagerHelpers.h"
extern PlayState* gPlayState;
}

static constexpr int32_t CVAR_TOT_MEDALLION_COLORS_DEFAULT = 0;
#define CVAR_TOT_MEDALLION_COLORS_NAME CVAR_ENHANCEMENT("ToTMedallionsColors")
#define CVAR_TOT_MEDALLION_COLORS_VALUE \
    CVarGetInteger(CVAR_TOT_MEDALLION_COLORS_NAME, CVAR_TOT_MEDALLION_COLORS_DEFAULT)

// GreyScaleEndDlist
#define dgEndGrayscaleAndEndDlistDL "__OTR__helpers/cosmetics/gEndGrayscaleAndEndDlistDL"
static const ALIGN_ASSET(2) char gEndGrayscaleAndEndDlistDL[] = dgEndGrayscaleAndEndDlistDL;

// This is used for the Temple of Time Medalions' color
#define dtokinoma_room_0DL_007A70 "__OTR__scenes/shared/tokinoma_scene/tokinoma_room_0DL_007A70"
static const ALIGN_ASSET(2) char tokinoma_room_0DL_007A70[] = dtokinoma_room_0DL_007A70;
#define dtokinoma_room_0DL_007FD0 "__OTR__scenes/shared/tokinoma_scene/tokinoma_room_0DL_007FD0"
static const ALIGN_ASSET(2) char tokinoma_room_0DL_007FD0[] = dtokinoma_room_0DL_007FD0;

static Gfx grayscaleWhite = gsDPSetGrayscaleColor(255, 255, 255, 255);

class ToTPatchSetup {
  public:
    ToTPatchSetup(Gfx ifColored, const char* patchName, int index, const char* patchName2 = "", int index2 = 0)
        : patchName(patchName), index(index), ifColored(ifColored), patchName2(patchName2), index2(index2) {
    }

    void ApplyPatch(bool colored = true) {
        Gfx colorGfx = colored ? ifColored : grayscaleWhite;
        ResourceMgr_PatchGfxByName(tokinoma_room_0DL_007A70, patchName, index, colorGfx);
        if (patchName2 && *patchName2) {
            ResourceMgr_PatchGfxByName(tokinoma_room_0DL_007FD0, patchName2, index2, colorGfx);
        }
    }

    void RevertPatch() {
        ResourceMgr_UnpatchGfxByName(tokinoma_room_0DL_007A70, patchName);
        if (patchName2 && *patchName2) {
            ResourceMgr_UnpatchGfxByName(tokinoma_room_0DL_007FD0, patchName2);
        }
    }

  private:
    const char* patchName;
    const char* patchName2;
    int index;
    int index2;
    Gfx ifColored;
};

typedef struct MedallionColorPatch {
    QuestItem questItemId;
    ToTPatchSetup patch;
} MedallionColorPatch;

static ToTPatchSetup startGrayscale =
    ToTPatchSetup(gsSPGrayscale(true), "ToTMedallions_StartGrayscale", 7, "ToTMedallions_2_StartGrayscale", 7);

static MedallionColorPatch medallionColorPatches[] = {
    { QUEST_MEDALLION_WATER, ToTPatchSetup(gsDPSetGrayscaleColor(0, 161, 255, 255), "ToTMedallions_MakeBlue", 16) },
    { QUEST_MEDALLION_SPIRIT, ToTPatchSetup(gsDPSetGrayscaleColor(255, 135, 0, 255), "ToTMedallions_MakeOrange", 45) },
    { QUEST_MEDALLION_LIGHT, ToTPatchSetup(gsDPSetGrayscaleColor(255, 255, 0, 255), "ToTMedallions_MakeYellow", 69,
                                           "ToTMedallions_2_MakeYellow", 16) },
    { QUEST_MEDALLION_FOREST, ToTPatchSetup(gsDPSetGrayscaleColor(0, 255, 0, 255), "ToTMedallions_MakeGreen", 94) },
    { QUEST_MEDALLION_FIRE, ToTPatchSetup(gsDPSetGrayscaleColor(255, 0, 0, 255), "ToTMedallions_MakeRed", 118) },
    { QUEST_MEDALLION_SHADOW, ToTPatchSetup(gsDPSetGrayscaleColor(212, 0, 255, 255), "ToTMedallions_MakePurple", 142,
                                            "ToTMedallions_2_MakePurple", 27) },
};

static ToTPatchSetup endGrayscale =
    ToTPatchSetup(gsSPBranchListOTRFilePath(gEndGrayscaleAndEndDlistDL), "ToTMedallions_EndGrayscaleAndEndDlist", 160,
                  "ToTMedallions_2_EndGrayscaleAndEndDlist", 51);

static void PatchToTMedallions() {
    // TODO: Refactor the DemoEffect_UpdateJewelAdult and DemoEffect_UpdateJewelChild from z_demo_effect
    // effects to take effect in there
    startGrayscale.ApplyPatch();

    for (auto& medallionPatch : medallionColorPatches) {
        medallionPatch.patch.ApplyPatch(CHECK_QUEST_ITEM(medallionPatch.questItemId));
    }

    endGrayscale.ApplyPatch();
}

static void ResetToTMedallions() {
    // Unpatch everything
    startGrayscale.RevertPatch();

    for (auto& medallionPatch : medallionColorPatches) {
        medallionPatch.patch.RevertPatch();
    }

    endGrayscale.RevertPatch();
}

static void CheckTempleOfTime(int16_t sceneNum) {
    if (sceneNum != SCENE_TEMPLE_OF_TIME) {
        return;
    }
    PatchToTMedallions();
}

static void RegisterToTMedallions() {
    if (CVAR_TOT_MEDALLION_COLORS_VALUE) {
        PatchToTMedallions();
    } else {
        ResetToTMedallions();
    }

    COND_HOOK(OnItemReceive, CVAR_TOT_MEDALLION_COLORS_VALUE, [](GetItemEntry) {
        if (gPlayState) {
            CheckTempleOfTime(gPlayState->sceneNum);
        }
    });
    COND_HOOK(OnSceneInit, CVAR_TOT_MEDALLION_COLORS_VALUE, CheckTempleOfTime);
}

static RegisterShipInitFunc initFunc(RegisterToTMedallions, { CVAR_TOT_MEDALLION_COLORS_NAME });
