#include "soh/Enhancements/game-interactor/GameInteractor_Hooks.h"
#include "soh/ShipInit.hpp"

extern "C" {
#include "functions.h"
#include "src/overlays/actors/ovl_En_Ge1/z_en_ge1.h"
extern SaveContext gSaveContext;

extern void EnGe1_TalkAfterGame_Archery(EnGe1* enGe1, PlayState* play);
}

#define CVAR_ARCHERY_CUSTOMIZE_NAME CVAR_ENHANCEMENT("CustomizeHorsebackArchery")
#define CVAR_ARCHERY_CUSTOMIZE_VALUE CVarGetInteger(CVAR_ARCHERY_CUSTOMIZE_NAME, 0)

static constexpr s32 CVAR_ARCHERY_AMMO_DEFAULT = 20;

#define CVAR_ARCHERY_AMMO_NAME CVAR_ENHANCEMENT("HorsebackArcheryAmmo")
#define CVAR_ARCHERY_AMMO_VALUE CVarGetInteger(CVAR_ARCHERY_AMMO_NAME, CVAR_ARCHERY_AMMO_DEFAULT)
#define CVAR_ARCHERY_AMMO_CHANGED (CVAR_ARCHERY_AMMO_VALUE != CVAR_ARCHERY_AMMO_DEFAULT)

#define CVAR_ARCHERY_ALWAYS_SCORE_NAME CVAR_ENHANCEMENT("HorsebackArcheryAlwaysScore")
#define CVAR_ARCHERY_ALWAYS_SCORE_VALUE CVarGetInteger(CVAR_ARCHERY_ALWAYS_SCORE_NAME, 0)

#define CVAR_ARCHERY_INSTANT_WIN_NAME CVAR_ENHANCEMENT("InstantHorsebackArcheryWin")
#define CVAR_ARCHERY_INSTANT_WIN_VALUE CVarGetInteger(CVAR_ARCHERY_INSTANT_WIN_NAME, 0)

#define CVAR_ARCHERY_BOTH_PRIZES_NAME CVAR_ENHANCEMENT("HorsebackArcheryBothPrizes")
#define CVAR_ARCHERY_BOTH_PRIZES_VALUE CVarGetInteger(CVAR_ARCHERY_BOTH_PRIZES_NAME, 0)

static void RegisterHorsebackArcheryInstantWin() {
    COND_VB_SHOULD(VB_PLAY_HORSEBACK_ARCHERY, CVAR_ARCHERY_CUSTOMIZE_VALUE && CVAR_ARCHERY_INSTANT_WIN_VALUE, {
        EnGe1* enGe1 = va_arg(args, EnGe1*);
        PlayState* play = va_arg(args, PlayState*);
        Rupees_ChangeBy(-20);
        Flags_SetEventChkInf(EVENTCHKINF_PLAYED_HORSEBACK_ARCHERY);
        gSaveContext.minigameScore = 1500;
        Message_CloseTextbox(play);
        gSaveContext.minigameState = 3;
        gSaveContext.eventInf[0] |= 0x100;
        play->nextEntranceIndex = ENTR_GERUDOS_FORTRESS_16;
        play->transitionType = TRANS_TYPE_CIRCLE(TCA_STARBURST, TCC_BLACK, TCS_FAST);
        play->transitionTrigger = TRANS_TRIGGER_START;
        *should = false;
    });
}

static void RegisterHorsebackArcheryAlwaysScore() {
    COND_VB_SHOULD(VB_SCORE_HORSEBACK_ARCHERY_TARGET, CVAR_ARCHERY_CUSTOMIZE_VALUE && CVAR_ARCHERY_ALWAYS_SCORE_VALUE, {
        s32* scoreIndex = va_arg(args, s32*);
        *scoreIndex = 2; // inner ring = 100 points
    });
}

static void RegisterHorsebackArcheryAmmo() {
    COND_VB_SHOULD(VB_SET_HORSEBACK_ARCHERY_AMMO, CVAR_ARCHERY_CUSTOMIZE_VALUE && CVAR_ARCHERY_AMMO_CHANGED, {
        InterfaceContext* interfaceCtx = va_arg(args, InterfaceContext*);
        interfaceCtx->hbaAmmo = CVAR_ARCHERY_AMMO_VALUE;
        *should = false;
    });
}

static void RegisterHorsebackArcheryBothPrizes() {
    COND_VB_SHOULD(VB_END_HORSEBACK_ARCHERY, CVAR_ARCHERY_BOTH_PRIZES_VALUE, {
        EnGe1* enGe1 = va_arg(args, EnGe1*);
        enGe1->actionFunc = EnGe1_TalkAfterGame_Archery;
        *should = false;
    })
}

static RegisterShipInitFunc initFunc_InstantWin(RegisterHorsebackArcheryInstantWin,
                                                { CVAR_ARCHERY_CUSTOMIZE_NAME, CVAR_ARCHERY_INSTANT_WIN_NAME });
static RegisterShipInitFunc initFunc_AlwaysScore(RegisterHorsebackArcheryAlwaysScore,
                                                 { CVAR_ARCHERY_CUSTOMIZE_NAME, CVAR_ARCHERY_ALWAYS_SCORE_NAME });
static RegisterShipInitFunc initFunc_Ammo(RegisterHorsebackArcheryAmmo,
                                          { CVAR_ARCHERY_CUSTOMIZE_NAME, CVAR_ARCHERY_AMMO_NAME });
static RegisterShipInitFunc initFunc_BothPrizes(RegisterHorsebackArcheryBothPrizes, { CVAR_ARCHERY_BOTH_PRIZES_NAME });
