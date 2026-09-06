#include "soh/Enhancements/enhancementTypes.h"
#include "soh/Enhancements/game-interactor/GameInteractor.h"
#include "soh/ShipInit.hpp"

extern "C" {
#include "functions.h"
#include "src/overlays/actors/ovl_En_Bom_Bowl_Man/z_en_bom_bowl_man.h"
extern PlayState* gPlayState;
}

#define CVAR_BOWLING_NAME CVAR_ENHANCEMENT("CustomizeBombchuBowling")
#define CVAR_BOWLING_VALUE CVarGetInteger(CVAR_BOWLING_NAME, 0)

#define CVAR_CUCCO_SMALL_NAME CVAR_ENHANCEMENT("BombchuBowlingNoSmallCucco")
#define CVAR_CUCCO_SMALL_VALUE CVarGetInteger(CVAR_CUCCO_SMALL_NAME, 0)

#define CVAR_CUCCO_BIG_NAME CVAR_ENHANCEMENT("BombchuBowlingNoBigCucco")
#define CVAR_CUCCO_BIG_VALUE CVarGetInteger(CVAR_CUCCO_BIG_NAME, 0)

static constexpr s32 CUCCO_BOWLING_AMMO_DEFAULT = 10;
#define CVAR_BOWLING_AMMO_NAME CVAR_ENHANCEMENT("BombchuBowlingAmmo")
#define CVAR_BOWLING_AMMO_VALUE CVarGetInteger(CVAR_BOWLING_AMMO_NAME, CUCCO_BOWLING_AMMO_DEFAULT)

#define CVAR_BOWLING_FIRST_PRIZE_NAME CVAR_ENHANCEMENT("BombchuBowlingFirstPrize")
#define CVAR_BOWLING_FIRST_PRIZE_VALUE CVarGetInteger(CVAR_BOWLING_FIRST_PRIZE_NAME, BOWLING_FIRST_PRIZE_RANDOM)

static constexpr f32 CUCCO_SEARCH_Z = -520.0f;

typedef enum {
    CUCCO_INDEX_SMALL,
    CUCCO_INDEX_BIG,
} BombchuBowlingCuccoIndex;

static void KillSmallCucco() {
    Actor* cucco = gPlayState->actorCtx.actorLists[ACTORCAT_PROP].head;

    while (cucco != NULL) {
        if (cucco->id == ACTOR_EN_SYATEKI_NIW && cucco->home.pos.z > CUCCO_SEARCH_Z) {
            Actor_Kill(cucco);
            break;
        }
        cucco = cucco->next;
    }
}

static void KillBigCucco() {
    Actor* cucco = gPlayState->actorCtx.actorLists[ACTORCAT_PROP].head;

    while (cucco != NULL) {
        if (cucco->id == ACTOR_EN_SYATEKI_NIW && cucco->home.pos.z < CUCCO_SEARCH_Z) {
            Actor_Kill(cucco);
            break;
        }
        cucco = cucco->next;
    }
}

static void RegisterBombchuBowlingNoSmallCucco() {
    s32 noCucco = CVAR_BOWLING_VALUE && CVAR_CUCCO_SMALL_VALUE;

    if (noCucco && gPlayState != NULL && gPlayState->sceneNum == SCENE_BOMBCHU_BOWLING_ALLEY) {
        KillSmallCucco();
    }

    COND_VB_SHOULD(VB_SPAWN_BOMBCHU_BOWLING_CUCCOS, noCucco, {
        s32 index = va_arg(args, s32);
        if (index == CUCCO_INDEX_SMALL) {
            *should = false;
        }
    });
}

static void RegisterBombchuBowlingNoBigCucco() {
    s32 noCucco = CVAR_BOWLING_VALUE && CVAR_CUCCO_BIG_VALUE;

    if (noCucco && gPlayState != NULL && gPlayState->sceneNum == SCENE_BOMBCHU_BOWLING_ALLEY) {
        KillBigCucco();
    }

    COND_VB_SHOULD(VB_SPAWN_BOMBCHU_BOWLING_CUCCOS, noCucco, {
        s32 index = va_arg(args, s32);
        if (index == CUCCO_INDEX_BIG) {
            *should = false;
        }
    });
}

static void RegisterBombchuBowlingAmmo() {
    COND_VB_SHOULD(VB_SET_BOMBCHU_BOWLING_AMMO,
                   CVAR_BOWLING_VALUE && (CVAR_BOWLING_AMMO_VALUE != CUCCO_BOWLING_AMMO_DEFAULT), {
                       gPlayState->bombchuBowlingStatus = CVAR_BOWLING_AMMO_VALUE;
                       *should = false;
                   });
}

static void RegisterBombchuBowlingFirstPrize() {
    COND_VB_SHOULD(VB_SET_BOMBCHU_BOWLING_PRIZE_SELECT,
                   !IS_RANDO && CVAR_BOWLING_VALUE && (CVAR_BOWLING_FIRST_PRIZE_VALUE != BOWLING_FIRST_PRIZE_RANDOM), {
                       EnBomBowlMan* bowlMan = va_arg(args, EnBomBowlMan*);
                       s16 prizeSelect = CVAR_BOWLING_FIRST_PRIZE_VALUE - 1;
                       bowlMan->prizeSelect = prizeSelect;
                       *should = false;
                   });
}

static RegisterShipInitFunc initFunc_SmallCucco(RegisterBombchuBowlingNoSmallCucco,
                                                { CVAR_BOWLING_NAME, CVAR_CUCCO_SMALL_NAME });
static RegisterShipInitFunc initFunc_BigCucco(RegisterBombchuBowlingNoBigCucco,
                                              { CVAR_BOWLING_NAME, CVAR_CUCCO_BIG_NAME });
static RegisterShipInitFunc initFunc_Ammo(RegisterBombchuBowlingAmmo, { CVAR_BOWLING_NAME, CVAR_BOWLING_AMMO_NAME });
static RegisterShipInitFunc initFunc_FirstPrize(RegisterBombchuBowlingFirstPrize,
                                                { CVAR_BOWLING_NAME, CVAR_BOWLING_FIRST_PRIZE_NAME, "IS_RANDO" });
