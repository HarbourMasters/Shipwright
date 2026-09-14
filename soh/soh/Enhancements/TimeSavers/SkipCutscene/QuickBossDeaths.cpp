#include "soh/Enhancements/game-interactor/GameInteractor.h"
#include "soh/Enhancements/game-interactor/GameInteractor_Hooks.h"
#include "soh/ShipInit.hpp"

extern "C" {
#include <objects/object_gnd/object_gnd.h>
#include "src/overlays/actors/ovl_Boss_Dodongo/z_boss_dodongo.h"
#include "src/overlays/actors/ovl_Boss_Fd2/z_boss_fd2.h"
#include "src/overlays/actors/ovl_Boss_Ganondrof/z_boss_ganondrof.h"
#include "src/overlays/actors/ovl_Boss_Goma/z_boss_goma.h"
#include "src/overlays/actors/ovl_Boss_Mo/z_boss_mo.h"
#include "src/overlays/actors/ovl_Boss_Sst/z_boss_sst.h"
#include "src/overlays/actors/ovl_Boss_Tw/z_boss_tw.h"
#include "src/overlays/actors/ovl_Boss_Va/z_boss_va.h"
#include "src/overlays/actors/ovl_Door_Warp1/z_door_warp1.h"
extern PlayState* gPlayState;
void BossSst_HandSetupThrash(BossSst* hand);
void BossSst_HandSetupDarken(BossSst* hand);
void BossSst_HeadSetupDarken(BossSst* head);
void BossSst_HeadDarken(BossSst* head, PlayState* play);
void BossSst_UpdateDeathCamera(BossSst* head, PlayState* play);
}

// These match enums that live in the actor .c files
constexpr s16 MO_DEATH_FINISH = 105;
constexpr s16 FD2_DEATH_FINISH = 5;
constexpr s8 VA_DEATH_START = 14;
constexpr s8 VA_DEATH_SHELL_BURST = 19;

// Gohma's camera spin normally lasts 270 frames, turning 0.022 radians each frame
constexpr int GOHMA_SPIN_FRAMES = 270;
constexpr int GOHMA_QUICK_SPIN_FRAMES = 81; // clear music plays at 80
constexpr float GOHMA_SPIN_SPEED = 0.022f;

// Bongo Bongo's darkening counts its timer down from 160, something special at 80 and 0.
// Counting by 4 still lands on both.
constexpr s16 SST_DARKEN_SPEED = 4;

#define CVAR_NAME CVAR_ENHANCEMENT("TimeSavers.SkipCutscene.QuickBossDeaths")
#define CVAR_VALUE CVarGetInteger(CVAR_NAME, IS_RANDO || IS_BOSS_RUSH)

void RegisterQuickBossDeaths() {
    COND_VB_SHOULD(VB_GOHMA_DEATH_SCENE, CVAR_VALUE, {
        BossGoma* goma = va_arg(args, BossGoma*);
        if (goma->actionState == 1) {
            // On the first frame of the spin, start decaying right away and shorten the spin
            if (goma->timer > GOHMA_QUICK_SPIN_FRAMES) {
                goma->timer = GOHMA_QUICK_SPIN_FRAMES;
                goma->framesUntilNextAction = 1080;
            }
            // Turn the camera faster during the shortened spin so it still goes all the way around
            goma->defeatedCameraEyeAngle +=
                GOHMA_SPIN_SPEED * (GOHMA_SPIN_FRAMES - GOHMA_QUICK_SPIN_FRAMES) / GOHMA_QUICK_SPIN_FRAMES;
        } else if (goma->actionState == 3) {
            // Skip the wait and the shrinking. Gohma's own code ends the cutscene once she has no height.
            goma->actor.scale.y = 0.0f;
            *should = true;
        }
    });

    COND_VB_SHOULD(VB_KING_DODONGO_DEATH_SCENE, CVAR_VALUE, {
        BossDodongo* kd = va_arg(args, BossDodongo*);
        // End the cutscene when the heart container spawns instead of waiting for him to sink
        if (kd->unk_1DA == 820) {
            // Sink him and cool the lava, which normally happen in the skipped part
            kd->unk_228 = -6600.0f;
            kd->unk_1C6 = 1;
            *should = true;
        }
    });

    COND_VB_SHOULD(VB_BARINADE_DEATH_SCENE, CVAR_VALUE, {
        BossVa* va = va_arg(args, BossVa*);
        s8* csState = va_arg(args, s8*);
        if (*csState == VA_DEATH_START) {
            // Skip the growths on the body and zappers, go straight to the arms bursting
            Actor* part = gPlayState->actorCtx.actorLists[ACTORCAT_BOSS].head;
            for (; part != NULL; part = part->next) {
                if (part->id == ACTOR_BOSS_VA && part->params >= BOSSVA_ZAPPER_1 && part->params <= BOSSVA_ZAPPER_3) {
                    // Hide the zapper. It stays alive since its sparks still read the zapper's position.
                    ((BossVa*)part)->isDead = true;
                }
            }
            *csState = VA_DEATH_SHELL_BURST;
        } else {
            va->timer = 1;
        }
        *should = false;
    });

    COND_VB_SHOULD(VB_PHANTOM_GANON_DEATH_SCENE, CVAR_VALUE, {
        *should = false;
        BossGanondrof* pg = va_arg(args, BossGanondrof*);
        Player* player = GET_PLAYER(gPlayState);
        if (pg->work[GND_ACTION_STATE] == DEATH_SPASM) {
            // Skip to death scream animation and move to middle
            pg->deathState = DEATH_SCREAM;
            pg->timers[0] = 50;
            AnimationHeader* screamAnim = (AnimationHeader*)gPhantomGanonScreamAnim;
            Animation_MorphToLoop(&pg->skelAnime, screamAnim, -10.0f);
            pg->actor.world.pos.x = GND_BOSSROOM_CENTER_X;
            pg->actor.world.pos.y = GND_BOSSROOM_CENTER_Y + 83.0f;
            pg->actor.world.pos.z = GND_BOSSROOM_CENTER_Z;
            pg->actor.shape.rot.y = 0;
            pg->work[GND_BODY_DECAY_INDEX] = 0;
            Audio_PlayActorSound2(&pg->actor, NA_SE_EN_FANTOM_LAST);

            // Move Player out of center of room
            player->actor.world.pos.x = GND_BOSSROOM_CENTER_X - 200.0f;
            player->actor.world.pos.z = GND_BOSSROOM_CENTER_Z;
        }
    });

    COND_VB_SHOULD(VB_VOLVAGIA_DEATH_SCENE, CVAR_VALUE, {
        if (*should) {
            BossFd2* fd2 = va_arg(args, BossFd2*);
            Camera* mainCam = Play_GetCamera(gPlayState, CAM_ID_MAIN);

            // Skip Volvagia flying out of the lava and falling apart, end cutscene here
            fd2->actor.draw = NULL;
            fd2->deathState = FD2_DEATH_FINISH;
            mainCam->eye = fd2->camData.eye;
            mainCam->eyeNext = fd2->camData.eye;
            mainCam->at = fd2->camData.at;
            func_800C08AC(gPlayState, fd2->deathCamera, 0);
            fd2->deathCamera = 0;
            func_80064534(gPlayState, &gPlayState->csCtx);
            Player_SetCsActionWithHaltedActors(gPlayState, &fd2->actor, 7);

            if (GameInteractor_Should(VB_SPAWN_BLUE_WARP, true, fd2)) {
                Actor_SpawnAsChild(&gPlayState->actorCtx, &fd2->actor, gPlayState, ACTOR_DOOR_WARP1, 0.0f, 100.0f, 0.0f,
                                   0, 0, 0, WARP_DUNGEON_ADULT);
            }
            // Where the skull lands in the skipped part
            if (GameInteractor_Should(VB_SPAWN_HEART_CONTAINER, true)) {
                Actor_Spawn(&gPlayState->actorCtx, gPlayState, ACTOR_ITEM_B_HEART, 0.0f, 110.0f, 150.0f, 0, 0, 0, 0);
            }
            Audio_QueueSeqCmd(SEQ_PLAYER_BGM_MAIN << 24 | NA_BGM_BOSS_CLEAR);
            Flags_SetClear(gPlayState, gPlayState->roomCtx.curRoom.num);
            *should = false;
        }
    });

    COND_VB_SHOULD(VB_MORPHA_DEATH_SCENE, CVAR_VALUE, {
        if (*should) {
            BossMo* core = va_arg(args, BossMo*);
            BossMo* tent = va_arg(args, BossMo*);
            Camera* mainCam = Play_GetCamera(gPlayState, CAM_ID_MAIN);

            // Skip draining the water and the tentacle dripping from the ceiling, end the cutscene here
            core->csState = MO_DEATH_FINISH;
            core->waterLevel = -295.0f;
            tent->actor.world.pos.x = 0.0f;
            tent->actor.world.pos.y = -1000.0f;
            tent->actor.world.pos.z = 0.0f;
            mainCam->eye = core->cameraEye;
            mainCam->eyeNext = core->cameraEye;
            mainCam->at = core->cameraAt;
            func_800C08AC(gPlayState, core->csCamera, 0);
            core->csCamera = 0;
            func_80064534(gPlayState, &gPlayState->csCtx);
            Player_SetCsActionWithHaltedActors(gPlayState, &core->actor, 7);

            if (GameInteractor_Should(VB_SPAWN_BLUE_WARP, true, tent)) {
                Actor_SpawnAsChild(&gPlayState->actorCtx, &tent->actor, gPlayState, ACTOR_DOOR_WARP1, 0.0f, -280.0f,
                                   0.0f, 0, 0, 0, WARP_DUNGEON_ADULT);
            }
            if (GameInteractor_Should(VB_SPAWN_HEART_CONTAINER, true)) {
                Actor_Spawn(&gPlayState->actorCtx, gPlayState, ACTOR_ITEM_B_HEART, 200.0f, -280.0f, 0.0f, 0, 0, 0, 0);
            }
            Audio_QueueSeqCmd(SEQ_PLAYER_BGM_MAIN << 24 | NA_BGM_BOSS_CLEAR);
            Flags_SetClear(gPlayState, gPlayState->roomCtx.curRoom.num);
            *should = false;
        }
    });

    COND_VB_SHOULD(VB_BONGO_BONGO_DEATH_SCENE, CVAR_VALUE, {
        BossSst* head = va_arg(args, BossSst*);
        BossSst* leftHand = va_arg(args, BossSst*);
        BossSst* rightHand = va_arg(args, BossSst*);

        if (head->actionFunc == BossSst_HeadDarken) {
            // Speed up darkening. Body colors come from timer.
            // Camera also moves faster to reach its marks in time.
            if (head->timer >= SST_DARKEN_SPEED) {
                head->timer -= SST_DARKEN_SPEED - 1;
                for (int i = 1; i < SST_DARKEN_SPEED; i++) {
                    BossSst_UpdateDeathCamera(head, gPlayState);
                }
            }
        } else if (*should) {
            // Skip thrashing, go straight to body darkening.
            for (BossSst* hand : { leftHand, rightHand }) {
                BossSst_HandSetupThrash(hand);
                // Put the hands where thrashing leaves them
                hand->actor.world.pos.x = hand->actor.home.pos.x + Math_SinS(head->actor.shape.rot.y) * 200.0f;
                hand->actor.world.pos.z = hand->actor.home.pos.z + Math_CosS(head->actor.shape.rot.y) * 200.0f;
                BossSst_HandSetupDarken(hand);
            }
            BossSst_HeadSetupDarken(head);
            *should = false;
        }
    });

    COND_VB_SHOULD(VB_TWINROVA_DEATH_SCENE, CVAR_VALUE, {
        BossTw* tw = va_arg(args, BossTw*);
        // Skip sisters arguing, go to them floating away
        if (tw->work[CS_TIMER_2] == 9) {
            tw->work[CS_TIMER_2] = 859;
        }
        // Beam around sisters normally opens during skipped part
        if (tw->work[CS_TIMER_2] < 900) {
            *should = true;
        } else if (tw->work[CS_TIMER_2] > 910) {
            Math_ApproachF(&tw->workf[UNK_F18], 0.0f, 1.0f, 3.0f);
        }
    });
}

static RegisterShipInitFunc initFunc(RegisterQuickBossDeaths, { CVAR_NAME, "IS_RANDO", "IS_BOSS_RUSH" });
