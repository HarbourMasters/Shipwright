#include <algorithm>

#include <libultraship/bridge/consolevariablebridge.h>

#include "soh/Enhancements/game-interactor/GameInteractor.h"
#include "soh/ShipInit.hpp"

extern "C" {
#include <objects/object_ganon2/object_ganon2.h>
#include <objects/object_ganon_anime3/object_ganon_anime3.h>
#include "src/overlays/actors/ovl_Boss_Ganon2/z_boss_ganon2.h"
#include "src/overlays/actors/ovl_En_Zl3/z_en_zl3.h"
extern PlayState* gPlayState;
void BossGanon2_SetObjectSegment(BossGanon2* ganon, PlayState* play, s32 objectId, u8 setRSPSegment);
}

// Ganon's intro state where he knocks the sword out of Link's hand
constexpr s16 GANON_KNOCK_SWORD_STATE = 23;
// The timer is not reset going into that state. It has counted since Ganon landed and started flailing.
constexpr u32 GANON_KNOCK_SWORD_TIMER = 215;

#define CVAR_NAME CVAR_ENHANCEMENT("TimeSavers.SkipCutscene.BossIntro")
#define CVAR_VALUE CVarGetInteger(CVAR_NAME, IS_RANDO)

void RegisterGanonIntros() {
    COND_HOOK(OnSceneInit, CVAR_VALUE || IS_RANDO || IS_BOSS_RUSH, [](int16_t sceneNum) {
        if (sceneNum == SCENE_GANONDORF_BOSS) {
            Flags_SetEventChkInf(EVENTCHKINF_BEGAN_GANONDORF_BATTLE);
        }
    });

    COND_VB_SHOULD(VB_PLAY_GANONDORF_INTRO_CS, CVAR_VALUE || IS_RANDO || IS_BOSS_RUSH, { *should = false; });

    // Skip Ganon rising from the rubble, transforming and landing. Skip to knocking sword away.
    COND_VB_SHOULD(VB_PLAY_GANON_INTRO_CS, CVAR_VALUE, {
        BossGanon2* ganon = va_arg(args, BossGanon2*);
        EnZl3* zelda = va_arg(args, EnZl3*);
        Player* player = GET_PLAYER(gPlayState);

        // The rubble watches unk_314: pieces fly off when Ganondorf rises (1),
        // then get removed or become breakable rubble once he floats (2).
        // Run their update once for each, with Ganondorf where he bursts out, so they push away from him.
        ganon->actor.world.pos.x = ganon->actor.world.pos.z = -200.0f;
        for (u8 mode : { 1, 2 }) {
            ganon->unk_314 = mode;
            for (Actor* actor = gPlayState->actorCtx.actorLists[ACTORCAT_PROP].head; actor != NULL;
                 actor = actor->next) {
                if (actor->id == ACTOR_DEMO_GJ && actor->update != NULL) {
                    actor->update(actor, gPlayState);
                }
            }
        }
        // Set when he finishes transforming. Stops the smoke.
        ganon->unk_314 = 3;

        // Swap Ganondorf's skeleton for Ganon's
        BossGanon2_SetObjectSegment(ganon, gPlayState, OBJECT_GANON2, false);
        SkelAnime_Free(&ganon->skelAnime, gPlayState);
        SkelAnime_InitFlex(gPlayState, &ganon->skelAnime, (FlexSkeletonHeader*)gGanonSkel, NULL, NULL, NULL, 0);
        BossGanon2_SetObjectSegment(ganon, gPlayState, OBJECT_GANON_ANIME3, false);
        Animation_PlayOnce(&ganon->skelAnime, (AnimationHeader*)gGanonUncurlAndFlailAnim);
        ganon->skelAnime.curFrame = std::min((float)GANON_KNOCK_SWORD_TIMER, ganon->skelAnime.endFrame);

        ganon->actor.world.pos.x = 50.0f;
        ganon->actor.world.pos.y = 1099.0f;
        ganon->actor.world.pos.z = -200.0f;
        ganon->actor.world.rot.y = 0x4000;
        ganon->actor.velocity.y = 0.0f;
        ganon->actor.shape.yOffset = 7000.0f;

        // Values the skipped states leave behind
        ganon->unk_337 = 2;
        ganon->unk_336 = 2;
        ganon->unk_324 = 255.0f;
        ganon->unk_228 = 1.0f;
        ganon->unk_224 = 0.0f;
        ganon->unk_30C = 0.0f;
        ganon->unk_394 = 0.0f;
        gPlayState->envCtx.unk_D8 = 0.0f;
        gPlayState->envCtx.unk_BE = gPlayState->envCtx.unk_BD = 0;

        player->actor.world.pos.x = 250.0f;
        player->actor.world.pos.y = 1086.0f;
        player->actor.world.pos.z = -266.0f;
        player->actor.shape.rot.y = -0x4000;
        Player_SetCsActionWithHaltedActors(gPlayState, &ganon->actor, 0x55);

        zelda->actor.world.pos.x = 724.0f;
        zelda->actor.world.pos.y = 1086.0f;
        zelda->actor.world.pos.z = -186.0f;
        zelda->actor.shape.rot.y = -0x5000;
        zelda->unk_3C8 = 5;

        Audio_QueueSeqCmd(SEQ_PLAYER_BGM_MAIN << 24 | NA_BGM_GANON_BOSS);

        ganon->csState = GANON_KNOCK_SWORD_STATE;
        ganon->csTimer = GANON_KNOCK_SWORD_TIMER;
        *should = false;
    });
}

static RegisterShipInitFunc initFunc(RegisterGanonIntros, { CVAR_NAME, "IS_RANDO", "IS_BOSS_RUSH" });
