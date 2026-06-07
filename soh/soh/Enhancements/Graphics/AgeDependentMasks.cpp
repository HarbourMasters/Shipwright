#include "soh/Enhancements/game-interactor/GameInteractor.h"
#include "soh/ShipInit.hpp"
#include "soh/ResourceManagerHelpers.h"

extern "C" {
#include "macros.h"
#include "functions.h"
#include "objects/object_link_boy/object_link_boy.h"
extern SaveContext gSaveContext;
}

static constexpr int32_t CVAR_EXTENDED_MODDING_DEFAULT = 0;
#define CVAR_EXTENDED_MODDING_NAME CVAR_ENHANCEMENT("ExtendedModdingSupport")
#define CVAR_EXTENDED_MODDING_VALUE CVarGetInteger(CVAR_EXTENDED_MODDING_NAME, CVAR_EXTENDED_MODDING_DEFAULT)

static const char* sAdultMaskDLists[] = {
    gLinkAdultKeatonMaskDL, gLinkAdultSkullMaskDL, gLinkAdultSpookyMaskDL, gLinkAdultBunnyHoodDL,
    gLinkAdultGoronMaskDL,  gLinkAdultZoraMaskDL,  gLinkAdultGerudoMaskDL, gLinkAdultMaskOfTruthDL,
};

static void RegisterAgeDependentMasks() {
    COND_VB_SHOULD(VB_DRAW_PLAYER_MASK, CVAR_EXTENDED_MODDING_VALUE, {
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

static RegisterShipInitFunc initFunc(RegisterAgeDependentMasks, { CVAR_EXTENDED_MODDING_NAME });
