#include "soh/Enhancements/game-interactor/GameInteractor.h"
#include "soh/ShipInit.hpp"

extern "C" {
#include "z64save.h"
#include "macros.h"
#include "variables.h"
#include "functions.h"
extern PlayState* gPlayState;
extern SaveContext gSaveContext;
}

/**
 * This simply skips the Mido interaction in Kokiri Forest, once you equip the Kokiri
 * Sword and Deku Shield he will move out of the way without you needing to talk to him.
 */
void RegisterMoveMidoInKokiriForest() {
    COND_VB_SHOULD(
        VB_MOVE_MIDO_IN_KOKIRI_FOREST, CVarGetInteger(CVAR_ENHANCEMENT("TimeSavers.SkipMiscInteractions"), IS_RANDO), {
            if (!Flags_GetEventChkInf(EVENTCHKINF_SHOWED_MIDO_SWORD_SHIELD) &&
                (CUR_EQUIP_VALUE(EQUIP_TYPE_SHIELD) == EQUIP_VALUE_SHIELD_DEKU) &&
                (CUR_EQUIP_VALUE(EQUIP_TYPE_SWORD) == EQUIP_VALUE_SWORD_KOKIRI) && gSaveContext.cutsceneIndex == 0) {
                Flags_SetEventChkInf(EVENTCHKINF_SHOWED_MIDO_SWORD_SHIELD);
                *should = true;
            }
        });
}

static RegisterShipInitFunc initFunc(RegisterMoveMidoInKokiriForest,
                                     { CVAR_ENHANCEMENT("TimeSavers.SkipMiscInteractions"), "IS_RANDO" });
