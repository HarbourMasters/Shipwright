#include "soh/Enhancements/game-interactor/GameInteractor.h"
#include "soh/ShipInit.hpp"

extern "C" {
#include "src/overlays/actors/ovl_En_Fr/z_en_fr.h"
extern PlayState* gPlayState;
extern void EnFr_SetupReward(EnFr* enFr, PlayState* play, u8 unkCondition);
}

#define CVAR_FROGS_CUSTOMIZE_NAME CVAR_ENHANCEMENT("CustomizeFrogsOcarinaGame")
#define CVAR_FROGS_CUSTOMIZE_VALUE CVarGetInteger(CVAR_FROGS_CUSTOMIZE_NAME, 0)

#define CVAR_FROGS_MODIFY_TIME_NAME CVAR_ENHANCEMENT("FrogsModifyFailTime")
#define CVAR_FROGS_MODIFY_TIME_VALUE CVarGetInteger(CVAR_FROGS_MODIFY_TIME_NAME, 1)

#define CVAR_FROGS_INFINITE_TIME_NAME CVAR_ENHANCEMENT("FrogsUnlimitedFailTime")
#define CVAR_FROGS_INFINITE_TIME_VALUE CVarGetInteger(CVAR_FROGS_INFINITE_TIME_NAME, 0)

#define CVAR_FROGS_INSTANT_WIN_NAME CVAR_ENHANCEMENT("InstantFrogsGameWin")
#define CVAR_FROGS_INSTANT_WIN_VALUE CVarGetInteger(CVAR_FROGS_INSTANT_WIN_NAME, 0)

static void RegisterFrogsOcarinaGameModifyTime() {
    COND_VB_SHOULD(VB_SET_FROG_OCARINA_GAME_TIME_LIMIT,
                   CVAR_FROGS_CUSTOMIZE_VALUE && (CVAR_FROGS_MODIFY_TIME_VALUE != 1), {
                       EnFr* enFr = va_arg(args, EnFr*);
                       s32 timeLimit = va_arg(args, s32);
                       enFr->frogSongTimer = timeLimit * CVAR_FROGS_MODIFY_TIME_VALUE;
                       *should = false;
                   });
}

static void RegisterFrogsOcarinaGameInfiniteTime() {
    COND_VB_SHOULD(VB_FROGS_OCARINA_GAME_TIMER_TICK, CVAR_FROGS_CUSTOMIZE_VALUE && CVAR_FROGS_INFINITE_TIME_VALUE,
                   { *should = false; });
}

static void RegisterFrogsOcarinaGameInstantWin() {
    COND_VB_SHOULD(VB_PLAY_FROG_OCARINA_GAME, CVAR_FROGS_CUSTOMIZE_VALUE && CVAR_FROGS_INSTANT_WIN_VALUE, {
        EnFr* enFr = va_arg(args, EnFr*);
        enFr->actor.textId = 0x40AC;
        EnFr_SetupReward(enFr, gPlayState, false);
        *should = false;
    });
}

static RegisterShipInitFunc initFunc_ModifyTime(RegisterFrogsOcarinaGameModifyTime,
                                                { CVAR_FROGS_CUSTOMIZE_NAME, CVAR_FROGS_MODIFY_TIME_NAME });
static RegisterShipInitFunc initFunc_InfiniteTime(RegisterFrogsOcarinaGameInfiniteTime,
                                                  { CVAR_FROGS_CUSTOMIZE_NAME, CVAR_FROGS_INFINITE_TIME_NAME });
static RegisterShipInitFunc initFunc_InstantWin(RegisterFrogsOcarinaGameInstantWin,
                                                { CVAR_FROGS_CUSTOMIZE_NAME, CVAR_FROGS_INSTANT_WIN_NAME });