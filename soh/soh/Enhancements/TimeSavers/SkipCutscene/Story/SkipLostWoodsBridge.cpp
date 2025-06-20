#include "soh/Enhancements/game-interactor/GameInteractor.h"
#include "soh/Enhancements/game-interactor/GameInteractor_Hooks.h"
#include "soh/ShipInit.hpp"

extern "C" {
#include "z64save.h"
#include "functions.h"
extern PlayState* gPlayState;
extern SaveContext gSaveContext;
}

void RegisterSkipLostWoodsBridge() {
    /**
     * This skips the cutscene where you speak to Saria on the bridge in Lost Woods, where she gives you the Fairy
     * Ocarina.
     */
    COND_VB_SHOULD(VB_PLAY_TRANSITION_CS, CVarGetInteger(CVAR_ENHANCEMENT("TimeSavers.SkipCutscene.Story"), IS_RANDO), {
        if ((gSaveContext.entranceIndex == ENTR_LOST_WOODS_BRIDGE_EAST_EXIT) &&
            !Flags_GetEventChkInf(EVENTCHKINF_SPOKE_TO_SARIA_ON_BRIDGE)) {
            Flags_SetEventChkInf(EVENTCHKINF_SPOKE_TO_SARIA_ON_BRIDGE);
            if (GameInteractor_Should(VB_GIVE_ITEM_FAIRY_OCARINA, true)) {
                Item_Give(gPlayState, ITEM_OCARINA_FAIRY);
            }
            *should = false;
        }
    });

    /**
     * While we could rely on the Item_Give that's normally called (and that we have above), it's not very clear to the
     * player that they received the item when skipping the cutscene, so we'll prevent it, and queue it up to be given
     * instead.
     */
    COND_VB_SHOULD(VB_GIVE_ITEM_FAIRY_OCARINA,
                   CVarGetInteger(CVAR_ENHANCEMENT("TimeSavers.SkipCutscene.Story"), IS_RANDO), { *should = false; });

    // Todo: Move item queueing here
}

static RegisterShipInitFunc initFunc(RegisterSkipLostWoodsBridge,
                                     { CVAR_ENHANCEMENT("TimeSavers.SkipCutscene.Story"), "IS_RANDO" });
