#include "soh/Enhancements/game-interactor/GameInteractor.h"
#include "soh/Enhancements/game-interactor/GameInteractor_Hooks.h"
#include "soh/ShipInit.hpp"

extern "C" {
#include "variables.h"
#include "seqcmd.h"
#include "src/overlays/actors/ovl_Boss_Tw/z_boss_tw.h"
#include "assets/objects/object_tw/object_tw.h"
extern BossTw* sTwinrovaPtr;
extern void BossTw_SetupFlyTo(BossTw*, PlayState*);
extern void BossTw_SetupWait(BossTw*, PlayState*);
}

typedef enum {
    /* 0x00 */ TW_KOTAKE,
    /* 0x01 */ TW_KOUME,
} TwinrovaType;

// Adjust alpha for spawning portals on the platforms
extern "C" void BossTwShip_TwinrovaHandlePortal(BossTw* bossTw, PlayState* play) {
    if (bossTw->timers[0]-- > 0) {
        Math_StepToF(&bossTw->spawnPortalAlpha, 255.0f, 10.0f);
    } else if (Math_StepToF(&bossTw->spawnPortalAlpha, 0.0f, 10.0f)) {
        BossTw_SetupWait(bossTw, play);
    }
}

extern "C" void BossTwShip_TwinrovaSetupHandlePortal(BossTw* bossTw, PlayState* play) {
    bossTw->timers[0] = 80;
    bossTw->spawnPortalAlpha = 1.0f;
    bossTw->spawnPortalScale = 0.05f;
    bossTw->actionFunc = BossTwShip_TwinrovaHandlePortal;
}

// Rise from below platform, then enter normal gameplay
extern "C" void BossTwShip_Rise(BossTw* bossTw, PlayState* play) {
    Player* player = GET_PLAYER(play);

    Math_SmoothStepToF(&bossTw->actor.world.pos.y, 400.0f, 300.0f, 6.0f, 4.0f);
    bossTw->actor.world.rot.y = bossTw->actor.shape.rot.y = Actor_WorldYawTowardActor(&bossTw->actor, &player->actor);
    SkelAnime_Update(&bossTw->skelAnime);
    Audio_PlayActorSound2(&bossTw->actor, NA_SE_EN_TWINROBA_FLY - SFX_FLAG);
    Math_ApproachF(&bossTw->scepterAlpha, 0.0f, 1.0f, 10.0f);
    if (bossTw->actor.world.pos.y >= 400.0f) {
        BossTw_SetupFlyTo(bossTw, play);
        if (bossTw->actor.params == TW_KOUME) {
            Audio_PlayActorSound2(&bossTw->actor, NA_SE_EN_TWINROBA_LAUGH2);
        }
    }
}

extern "C" void BossTwShip_SetupRise(BossTw* bossTw, PlayState* play) {
    Player* player = GET_PLAYER(play);

    bossTw->actor.flags |= ACTOR_FLAG_ATTENTION_ENABLED;
    bossTw->actionFunc = BossTwShip_Rise;
    if (bossTw->actor.params == TW_KOTAKE) {
        Audio_PlayActorSound2(&bossTw->actor, NA_SE_EN_TWINROBA_LAUGH);
    }
}

// Wait for player to climb up to start fight
extern "C" void BossTwShip_WaitHeight(BossTw* bossTw, PlayState* play) {
    Player* player = GET_PLAYER(play);

    if (player->actor.world.pos.y >= 150.0f) {
        Flags_SetEventChkInf(EVENTCHKINF_BEGAN_TWINROVA_BATTLE);
        SEQCMD_PLAY_SEQUENCE(SEQ_PLAYER_BGM_MAIN, 0, 0, NA_BGM_BOSS);
        BossTwShip_SetupRise(bossTw, play);
        // The spawn portal is drawn by the Twinrova type BossTw actor, not Kotake/Koume
        BossTwShip_TwinrovaSetupHandlePortal(sTwinrovaPtr, play);
    }
}

extern "C" void BossTwShip_SetupWaitHeight(BossTw* bossTw, PlayState* play) {
    Player* player = GET_PLAYER(play);

    // X and Z are swapped
    bossTw->actor.world.pos.x = (bossTw->actor.params == TW_KOTAKE) ? -600.0f : 600.0f;
    bossTw->actor.world.pos.y = 60.0f;
    bossTw->actor.world.rot.y = bossTw->actor.shape.rot.y = Actor_WorldYawTowardActor(&bossTw->actor, &player->actor);
    bossTw->unk_5F8 = true; // Hair activated
    bossTw->actionFunc = BossTwShip_WaitHeight;
}

void RegisterTwinrovaSkipIntroCS() {
    COND_VB_SHOULD(VB_PLAY_TWINROVA_INTRO_CS,
                   (CVarGetInteger(CVAR_ENHANCEMENT("TimeSavers.SkipCutscene.BossIntro"), IS_RANDO)), {
                       BossTw* bossTw = va_arg(args, BossTw*);
                       PlayState* play = va_arg(args, PlayState*);

                       if (bossTw->actor.params == TW_KOTAKE || bossTw->actor.params == TW_KOUME) {
                           BossTwShip_SetupWaitHeight(bossTw, play);
                       } else if (!Flags_GetClear(play, play->roomCtx.curRoom.num)) {
                           // Twinrova
                           SEQCMD_PLAY_SEQUENCE(SEQ_PLAYER_BGM_MAIN, 0, 0, NA_BGM_KOTAKE_KOUME);
                           BossTw_SetupWait(bossTw, play);
                           bossTw->visible = false;
                           bossTw->actor.world.pos.y = -2000.0f;
                           bossTw->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
                       }

                       *should = false;
                   });

    // Swap portal XZ to match new Kotake/Koume spawn position
    COND_VB_SHOULD(VB_TWINROVA_SPAWN_PORTAL_TRANSLATION_KOTAKE,
                   (CVarGetInteger(CVAR_ENHANCEMENT("TimeSavers.SkipCutscene.BossIntro"), IS_RANDO)), {
                       Matrix_Translate(-600.0f, 232.0f, 0.0f, MTXMODE_NEW); // Ice/Kotake
                       *should = false;
                   });
    COND_VB_SHOULD(VB_TWINROVA_SPAWN_PORTAL_TRANSLATION_KOUME,
                   (CVarGetInteger(CVAR_ENHANCEMENT("TimeSavers.SkipCutscene.BossIntro"), IS_RANDO)), {
                       Matrix_Translate(600.0f, 232.0f, 0.0f, MTXMODE_NEW); // Fire/Koume
                       *should = false;
                   });
}

static RegisterShipInitFunc initTwinrovaSkipIntroCS(RegisterTwinrovaSkipIntroCS,
                                                    { CVAR_ENHANCEMENT("TimeSavers.SkipCutscene.BossIntro"),
                                                      "IS_RANDO" });
