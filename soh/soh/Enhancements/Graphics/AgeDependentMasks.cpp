#include <libultraship/bridge/resourcebridge.h>
#include "soh/Enhancements/game-interactor/GameInteractor.h"
#include "soh/ShipInit.hpp"
#include "soh/ResourceManagerHelpers.h"

extern "C" {
#include "macros.h"
#include "functions.h"
#include "objects/object_link_boy/object_link_boy.h"
extern SaveContext gSaveContext;
}

static const char* sAdultMaskDLists[] = {
    gLinkAdultKeatonMaskDL, gLinkAdultSkullMaskDL, gLinkAdultSpookyMaskDL, gLinkAdultBunnyHoodDL,
    gLinkAdultGoronMaskDL,  gLinkAdultZoraMaskDL,  gLinkAdultGerudoMaskDL, gLinkAdultMaskOfTruthDL,
};

static void RegisterAgeDependentMasks() {
    COND_VB_SHOULD(VB_DRAW_PLAYER_MASK, CVarGetInteger(CVAR_SETTING("AltAssets"), 1), {
        if (!LINK_IS_ADULT)
            return;

        PlayerMask currentMask = (PlayerMask)va_arg(args, int);
        PlayState* play = va_arg(args, PlayState*);

        int maskIndex = currentMask - 1;
        if (maskIndex < 0 || maskIndex >= 8)
            return;

        const char* adultDL = sAdultMaskDLists[maskIndex];
        if (!ResourceGetIsCustomByName(adultDL) && !ResourceMgr_FileExists(adultDL))
            return;

        *should = false;
        gSPDisplayList(play->state.gfxCtx->polyOpa.p++, (Gfx*)adultDL);
    });
}

static RegisterShipInitFunc initFunc(RegisterAgeDependentMasks, { CVAR_SETTING("AltAssets") });
