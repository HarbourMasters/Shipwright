#include "soh/Enhancements/game-interactor/GameInteractor_Hooks.h"
#include "soh/ShipInit.hpp"

extern "C" {
#include "functions.h"
#include "macros.h"
#include "src/overlays/actors/ovl_En_Po_Relay/z_en_o_relay.h"
extern SaveContext gSaveContext;
extern PlayState* gPlayState;
}

#define CVAR_DAMPE_BOTH_PRIZES_NAME CVAR_ENHANCEMENT("DampeBothPrizes")
#define CVAR_DAMPE_BOTH_PRIZES_VALUE CVarGetInteger(CVAR_DAMPE_BOTH_PRIZES_NAME, 0)

static void RegisterDampeBothPrizes() {
    COND_VB_SHOULD(VB_DAMPE_AWARD_PRIZES, CVAR_DAMPE_BOTH_PRIZES_VALUE || IS_RANDO, {
        EnPoRelay* enPoRelay = va_arg(args, EnPoRelay*);
        Vec3f posAtGround;
        posAtGround.x = enPoRelay->actor.world.pos.x;
        posAtGround.y = enPoRelay->actor.floorHeight;
        posAtGround.z = enPoRelay->actor.world.pos.z;

        if (gSaveContext.timerSeconds < HIGH_SCORE(HS_DAMPE_RACE)) {
            HIGH_SCORE(HS_DAMPE_RACE) = gSaveContext.timerSeconds;
        }
        // spawn Hookshot chest
        if (!Flags_GetTreasure(gPlayState, 0)) {
            Flags_SetTempClear(gPlayState, 4);
        }
        // spawn Piece of Heart
        if (!Flags_GetCollectible(gPlayState, enPoRelay->actor.params) && gSaveContext.timerSeconds <= 60) {
            Item_DropCollectible2(gPlayState, &posAtGround,
                                  (enPoRelay->actor.params << 8) + (0x4000 | ITEM00_HEART_PIECE));
        } else {
            Actor_Spawn(&gPlayState->actorCtx, gPlayState, ACTOR_EN_ITEM00, posAtGround.x, posAtGround.y, posAtGround.z,
                        0, 0, 0, 2);
        }

        Actor_Kill(&enPoRelay->actor);
        *should = false;
    });
}

static RegisterShipInitFunc initFunc(RegisterDampeBothPrizes, { CVAR_DAMPE_BOTH_PRIZES_NAME, "IS_RANDO" });
