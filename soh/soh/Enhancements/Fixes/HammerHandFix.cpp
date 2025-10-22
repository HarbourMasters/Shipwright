#include "soh/Enhancements/game-interactor/GameInteractor_Hooks.h"
#include "soh/Enhancements/mods.h"
#include "soh/ShipInit.hpp"

extern "C" {
#include "macros.h"
#include "soh/ResourceManagerHelpers.h"
#include "objects/object_link_boy/object_link_boy.h"
extern SaveContext gSaveContext;
}

static constexpr int32_t CVAR_HAMMER_HAND_DEFAULT = 0;
#define CVAR_HAMMER_HAND_NAME CVAR_ENHANCEMENT("FixHammerHand")
#define CVAR_HAMMER_HAND_VALUE CVarGetInteger(CVAR_HAMMER_HAND_NAME, CVAR_HAMMER_HAND_DEFAULT)

void UpdateHammerHand() {
    if (CVAR_HAMMER_HAND_VALUE && LINK_IS_ADULT) {
        ResourceMgr_PatchGfxByName(gLinkAdultLeftHandHoldingHammerNearDL, "hammerHand1", 92,
                                   gsSPDisplayListOTRFilePath(gLinkAdultLeftHandClosedNearDL));
        ResourceMgr_PatchGfxByName(gLinkAdultLeftHandHoldingHammerNearDL, "hammerHand2", 93, gsSPEndDisplayList());
    } else {
        ResourceMgr_UnpatchGfxByName(gLinkAdultLeftHandHoldingHammerNearDL, "hammerHand1");
        ResourceMgr_UnpatchGfxByName(gLinkAdultLeftHandHoldingHammerNearDL, "hammerHand2");
    }
}

static void RegisterHammerHandFix() {
    COND_HOOK(OnSceneInit, true, [](int32_t) { UpdateHammerHand(); });
}

static RegisterShipInitFunc initFunc(RegisterHammerHandFix, { CVAR_HAMMER_HAND_NAME });
