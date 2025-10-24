#include <vector>
#include "soh/Enhancements/game-interactor/GameInteractor_Hooks.h"
#include "soh/Enhancements/mods.h"
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
    ToTPatchSetup(const char* path, const char* patchName, int index, Gfx ifColored)
        : path(path), patchName(patchName), index(index), ifColored(ifColored) {
    }

    void ApplyPatch(bool colored = true) {
        ResourceMgr_PatchGfxByName(path, patchName, index, colored ? ifColored : grayscaleWhite);
    }

    void RevertPatch() {
        ResourceMgr_UnpatchGfxByName(path, patchName);
    }

  private:
    const char* path;
    const char* patchName;
    int index;
    Gfx ifColored;
};

typedef struct MedallionColorPatch {
    QuestItem questItemId;
    std::vector<ToTPatchSetup> patches;
} MedallionColorPatch;

static ToTPatchSetup startGrayscale[] = {
    ToTPatchSetup(tokinoma_room_0DL_007A70, "ToTMedallions_StartGrayscale", 7, gsSPGrayscale(true)),
    ToTPatchSetup(tokinoma_room_0DL_007FD0, "ToTMedallions_2_StartGrayscale", 7, gsSPGrayscale(true)),
};

static MedallionColorPatch medallionColorPatches[] = {
    { QUEST_MEDALLION_WATER,
      { ToTPatchSetup(tokinoma_room_0DL_007A70, "ToTMedallions_MakeBlue", 16,
                      gsDPSetGrayscaleColor(0, 161, 255, 255)) } },
    { QUEST_MEDALLION_SPIRIT,
      { ToTPatchSetup(tokinoma_room_0DL_007A70, "ToTMedallions_MakeOrange", 45,
                      gsDPSetGrayscaleColor(255, 135, 0, 255)) } },
    { QUEST_MEDALLION_LIGHT,
      { ToTPatchSetup(tokinoma_room_0DL_007A70, "ToTMedallions_MakeYellow", 69,
                      gsDPSetGrayscaleColor(255, 255, 0, 255)),
        ToTPatchSetup(tokinoma_room_0DL_007FD0, "ToTMedallions_2_MakeYellow", 16,
                      gsDPSetGrayscaleColor(255, 255, 0, 255)) } },
    { QUEST_MEDALLION_FOREST,
      { ToTPatchSetup(tokinoma_room_0DL_007A70, "ToTMedallions_MakeGreen", 94,
                      gsDPSetGrayscaleColor(0, 255, 0, 255)) } },
    { QUEST_MEDALLION_FIRE,
      { ToTPatchSetup(tokinoma_room_0DL_007A70, "ToTMedallions_MakeRed", 118,
                      gsDPSetGrayscaleColor(255, 0, 0, 255)) } },
    { QUEST_MEDALLION_SHADOW,
      { ToTPatchSetup(tokinoma_room_0DL_007A70, "ToTMedallions_MakePurple", 142,
                      gsDPSetGrayscaleColor(212, 0, 255, 255)),
        ToTPatchSetup(tokinoma_room_0DL_007FD0, "ToTMedallions_2_MakePurple", 27,
                      gsDPSetGrayscaleColor(212, 0, 255, 255) } },
};

static ToTPatchSetup endGrayscale[] = {
    ToTPatchSetup(tokinoma_room_0DL_007A70, "ToTMedallions_EndGrayscaleAndEndDlist", 160,
                  gsSPBranchListOTRFilePath(gEndGrayscaleAndEndDlistDL)),
    ToTPatchSetup(tokinoma_room_0DL_007FD0, "ToTMedallions_2_EndGrayscaleAndEndDlist", 51,
                  gsSPBranchListOTRFilePath(gEndGrayscaleAndEndDlistDL)),
};

static void PatchToTMedallions() {
    // TODO: Refactor the DemoEffect_UpdateJewelAdult and DemoEffect_UpdateJewelChild from z_demo_effect
    // effects to take effect in there
    for (auto& patch : startGrayscale) {
        patch.ApplyPatch();
    }

    for (auto& medallionPatch : medallionColorPatches) {
        bool hasMedallion = CHECK_QUEST_ITEM(medallionPatch.questItemId);
        for (auto& patch : medallionPatch.patches) {
            patch.ApplyPatch(hasMedallion);
        }
    }

    for (auto& patch : endGrayscale) {
        patch.ApplyPatch();
    }
}

static void ResetToTMedallions() {
    // Unpatch everything
    for (auto& patch : startGrayscale) {
        patch.RevertPatch();
    }

    for (auto& medallionPatch : medallionColorPatches) {
        for (auto& patch : medallionPatch.patches) {
            patch.RevertPatch();
        }
    }

    for (auto& patch : endGrayscale) {
        patch.RevertPatch();
    }
}

void UpdateToTMedallions() {
    if (CVAR_TOT_MEDALLION_COLORS_VALUE) {
        PatchToTMedallions();
    } else {
        ResetToTMedallions();
    }
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
