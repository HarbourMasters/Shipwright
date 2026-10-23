#include <libultraship/bridge/consolevariablebridge.h>

#include "soh/cvar_prefixes.h"
#include "soh/Enhancements/game-interactor/GameInteractor.h"
#include "soh/ShipInit.hpp"

static constexpr const char* CVAR_KING_ZORA_1PT0 = CVAR_ENHANCEMENT("EarlyEyeballFrog");

extern "C" {
#include "src/overlays/actors/ovl_En_Kz/z_en_kz.h"
}

static void RegisterKingZora1pt0() {
    COND_VB_SHOULD(VB_KING_ZORA_1PT0, CVarGetInteger(CVAR_KING_ZORA_1PT0, 0), { *should = true; });

    COND_VB_SHOULD(VB_KING_ZORA_1PT0_TIMER, CVarGetInteger(CVAR_KING_ZORA_1PT0, 0), {
        EnKz* kingZora = va_arg(args, EnKz*);
        *should = kingZora->interactInfo.talkState == NPC_TALK_STATE_ITEM_GIVEN;
    });
}

static RegisterShipInitFunc initFunc(RegisterKingZora1pt0, { CVAR_KING_ZORA_1PT0 });
