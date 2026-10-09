#include <libultraship/bridge/consolevariablebridge.h>
#include "2s2h/GameInteractor/GameInteractor.h"
#include "2s2h/Rando/Rando.h"
#include "2s2h/Enhancements/FrameInterpolation/FrameInterpolation.h"
#include "2s2h/ShipInit.hpp"

extern "C" {
#include "variables.h"
#include "functions.h"
#include "overlays/actors/ovl_Dm_Stk/z_dm_stk.h"
#include "overlays/actors/ovl_Dm_Char02/z_dm_char02.h"
#include "objects/object_stk2/object_stk2.h"

extern f32 sBgmEnemyDistSq;
void DmStk_ClockTower_StartIntroCutsceneVersion1(DmStk* dmstk, PlayState* play);
void DmStk_ClockTower_StartIntroCutsceneVersion2(DmStk* dmstk, PlayState* play);
void DmStk_ClockTower_IdleWithOcarina(DmStk* dmstk, PlayState* play);
void DmStk_ClockTower_Idle(DmStk* dmstk, PlayState* play);
void DmStk_ChangeAnim(DmStk* dmstk, PlayState* play, SkelAnime* skelAnime, AnimationInfo* animInfo, u16 animIndex);
}

#define CVAR_NAME "gEnhancements.Cutscenes.SkipStoryCutscenes"
#define CVAR CVarGetInteger(CVAR_NAME, 0)

AnimationInfo moonLoop = { (AnimationHeader*)&gSkullKidCallDownMoonLoopAnim, 1.0f, 0.0f, -1.0f, ANIMMODE_LOOP, 0.0f };
AnimationInfo armsCrossedLoop = {
    (AnimationHeader*)&gSkullKidFloatingArmsCrossedAnim, 1.0f, 0.0f, -1.0f, ANIMMODE_LOOP, 0.0f
};

Vec3f afterCutscenePos = { 0, 200.0f, 0 };

// Skips the interaction with Skull Kid at the Clock Tower, both with and without the ocarina
void RegisterSkipClockTowerSkullKidEncounter() {
    COND_ID_HOOK(OnActorInit, ACTOR_DM_STK, CVAR, [](Actor* actor) {
        DmStk* dmstk = (DmStk*)actor;

        // Needs to be clock tower rooftop
        if (gPlayState->sceneId != SCENE_OKUJOU) {
            return;
        }

        if (dmstk->actionFunc == DmStk_ClockTower_StartIntroCutsceneVersion1) {
            // Place Skullkid in his final position from when the cutscene would have ended
            dmstk->actor.world.pos = afterCutscenePos;
            dmstk->animIndex = 33; // SK_ANIM_CALL_DOWN_MOON_LOOP
            dmstk->handType = 3;   // SK_HAND_TYPE_HOLDING_OCARINA
            DmStk_ChangeAnim(dmstk, gPlayState, &dmstk->skelAnime, &moonLoop, 0);
            dmstk->actionFunc = DmStk_ClockTower_IdleWithOcarina;
            Actor_PlaySfx(&dmstk->actor, NA_SE_EN_STAL20_CALL_MOON);
            Audio_PlaySequenceInCutscene(NA_BGM_MINI_BOSS);
        } else if (dmstk->actionFunc == DmStk_ClockTower_StartIntroCutsceneVersion2) {
            // Place Skullkid in his final position from when the cutscene would have ended
            dmstk->actor.world.pos = afterCutscenePos;
            dmstk->animIndex = 38; // SK_ANIM_FLOATING_ARMS_CROSSED
            DmStk_ChangeAnim(dmstk, gPlayState, &dmstk->skelAnime, &armsCrossedLoop, 0);
            dmstk->actionFunc = DmStk_ClockTower_Idle;
            Actor_PlaySfx(&dmstk->actor, NA_SE_EN_STAL20_CALL_MOON);
            Audio_PlaySequenceInCutscene(NA_BGM_MINI_BOSS);
        }
    });

    COND_ID_HOOK(OnActorInit, ACTOR_DM_CHAR02, CVAR, [](Actor* actor) {
        DmChar02* dmChar02 = (DmChar02*)actor;

        // Place ocarina in its final position from when the cutscene would have ended
        dmChar02->actor.world.pos = afterCutscenePos;
    });
}

static RegisterShipInitFunc initFunc(RegisterSkipClockTowerSkullKidEncounter, { CVAR_NAME });
