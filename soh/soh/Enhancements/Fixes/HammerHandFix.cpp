#include "soh/Enhancements/game-interactor/GameInteractor_Hooks.h"
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

static void FixHammerHand() {
    if (LINK_IS_ADULT) {
        ResourceMgr_PatchGfxByName(gLinkAdultLeftHandHoldingHammerNearDL, "hammerHand1", 92,
                                   gsSPDisplayListOTRFilePath(gLinkAdultLeftHandClosedNearDL));
        ResourceMgr_PatchGfxByName(gLinkAdultLeftHandHoldingHammerNearDL, "hammerHand2", 93, gsSPEndDisplayList());
    }
}

static void ResetHammerHand() {
    ResourceMgr_UnpatchGfxByName(gLinkAdultLeftHandHoldingHammerNearDL, "hammerHand1");
    ResourceMgr_UnpatchGfxByName(gLinkAdultLeftHandHoldingHammerNearDL, "hammerHand2");
}

static void RegisterHammerHandFix() {
    if (CVAR_HAMMER_HAND_VALUE) {
        FixHammerHand();
    } else {
        ResetHammerHand();
    }

    COND_HOOK(OnSceneInit, CVAR_HAMMER_HAND_VALUE, [](int32_t) { FixHammerHand(); });
}

static RegisterShipInitFunc initFunc(RegisterHammerHandFix, { CVAR_HAMMER_HAND_NAME });
