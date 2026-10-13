#include <libultraship/bridge/consolevariablebridge.h>
#include "2s2h/GameInteractor/GameInteractor.h"
#include "2s2h/ShipInit.hpp"

extern "C" {
#include "src/overlays/kaleido_scope/ovl_kaleido_scope/z_kaleido_scope.h"
extern s16 sEquipState;
extern s16 sEquipAnimTimer;
}

#define CVAR_NAME "gEnhancements.Equipment.MagicArrowEquipSpeed"
#define CVAR CVarGetInteger(CVAR_NAME, 0)

void RegisterSkipMagicArrowEquip() {
    COND_HOOK(OnKaleidoUpdate, CVAR, [](PauseContext* pauseCtx) {
        if (sEquipState == EQUIP_STATE_MAGIC_ARROW_GROW_ORB) {
            sEquipState = EQUIP_STATE_MAGIC_ARROW_MOVE_TO_BOW_SLOT;
            sEquipAnimTimer = 2;
            pauseCtx->equipAnimAlpha = 255;
        }
    });
}

static RegisterShipInitFunc initFunc(RegisterSkipMagicArrowEquip, { CVAR_NAME });
