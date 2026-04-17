#include "soh/Enhancements/game-interactor/GameInteractor_Hooks.h"
#include "soh/ShipInit.hpp"

extern "C" {
#include "functions.h"
#include "src/overlays/actors/ovl_En_Ge1/z_en_ge1.h"
extern SaveContext gSaveContext;

extern void EnGe1_TalkAfterGame_Archery(EnGe1* enGe1, PlayState* play);
}

static void RegisterHorsebackArcheryEnhancements() {
    COND_VB_SHOULD(VB_PLAY_HORSEBACK_ARCHERY,
                   CVarGetInteger(CVAR_ENHANCEMENT("CustomizeHorsebackArchery"), 0) &&
                       CVarGetInteger(CVAR_ENHANCEMENT("InstantHorsebackArcheryWin"), 0),
                   {
                       EnGe1* enGe1 = va_arg(args, EnGe1*);
                       PlayState* play = va_arg(args, PlayState*);
                       Rupees_ChangeBy(-20);
                       Flags_SetEventChkInf(EVENTCHKINF_PLAYED_HORSEBACK_ARCHERY);
                       gSaveContext.minigameScore = 1500;
                       Message_CloseTextbox(play);
                       enGe1->actionFunc = EnGe1_TalkAfterGame_Archery;
                       *should = false;
                   });

    COND_VB_SHOULD(VB_SCORE_HORSEBACK_ARCHERY_TARGET,
                   CVarGetInteger(CVAR_ENHANCEMENT("CustomizeHorsebackArchery"), 0) &&
                       CVarGetInteger(CVAR_ENHANCEMENT("HorsebackArcheryAlwaysScore"), 0),
                   {
                       s32* scoreIndex = va_arg(args, s32*);
                       *scoreIndex = 2; // inner ring = 100 points
                   });

    COND_VB_SHOULD(VB_SET_HORSEBACK_ARCHERY_AMMO, CVarGetInteger(CVAR_ENHANCEMENT("CustomizeHorsebackArchery"), 0), {
        InterfaceContext* interfaceCtx = va_arg(args, InterfaceContext*);
        interfaceCtx->hbaAmmo = CVarGetInteger(CVAR_ENHANCEMENT("HorsebackArcheryAmmo"), 20);
        *should = false;
    });
}

static RegisterShipInitFunc
    initFunc(RegisterHorsebackArcheryEnhancements,
             { CVAR_ENHANCEMENT("CustomizeHorsebackArchery"), CVAR_ENHANCEMENT("InstantHorsebackArcheryWin"),
               CVAR_ENHANCEMENT("HorsebackArcheryAlwaysScore"), CVAR_ENHANCEMENT("HorsebackArcheryAmmo") });
