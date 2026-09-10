#include "functions.h"
#include "macros.h"
#include "variables.h"
#include "soh/ShipUtils.h"
#include "soh/Enhancements/enhancementTypes.h"
#include "soh/Enhancements/game-interactor/GameInteractor.h"
#include "soh/ResourceManagerHelpers.h"
#include "soh/ShipInit.hpp"

extern "C" {
#include <z64.h>
#include "src/overlays/actors/ovl_Door_Shutter/z_door_shutter.h"
#include "src/overlays/actors/ovl_En_GeldB/z_en_geldb.h"
#include "src/overlays/actors/ovl_En_Ik/z_en_ik.h"
#include "src/overlays/actors/ovl_En_Test/z_en_test.h"
#include "src/overlays/actors/ovl_En_Zf/z_en_zf.h"

extern PlayState* gPlayState;
extern void EnTest_WaitGround(EnTest*, PlayState*);
extern void Player_Action_80845EF8(Player*, PlayState*);
extern void Player_Action_80845CA4(Player*, PlayState*);
}

extern bool IsClearRoom(bool mq, s16 sceneNum, s8 roomNum);
extern bool IsTimedRoom(bool mq, s16 sceneNum, s8 roomNum);

#define CVAR_ENEMY_RANDOMIZER_NAME CVAR_ENHANCEMENT("RandomizedEnemies")
#define CVAR_ENEMY_RANDOMIZER_DEFAULT ENEMY_RANDOMIZER_OFF
#define CVAR_ENEMY_RANDOMIZER_VALUE CVarGetInteger(CVAR_ENEMY_RANDOMIZER_NAME, CVAR_ENEMY_RANDOMIZER_DEFAULT)
#define ENEMY_RANDOMIZER_ENABLED CVAR_ENEMY_RANDOMIZER_VALUE != CVAR_ENEMY_RANDOMIZER_DEFAULT
#define CVAR_MINIBOSS_BGM_NAME CVAR_ENHANCEMENT("EnemyRandoMinibossBgm")
#define CVAR_MINIBOSS_BGM_DEFAULT false
#define MINIBOSS_BGM_ENABLED CVarGetInteger(CVAR_MINIBOSS_BGM_NAME, CVAR_MINIBOSS_BGM_DEFAULT)
#define CVAR_INVIS_STALFOS_NAME CVAR_ENHANCEMENT("EnemyRandoInvisStalfosBgm")
#define CVAR_INVIS_STALFOS_DEFAULT false
#define INVIS_STALFOS_ENABLED CVarGetInteger(CVAR_INVIS_STALFOS_NAME, CVAR_INVIS_STALFOS_DEFAULT)

static u8 sControlMusicArea = false; // Area that player update should control miniboss music for

// Check if active Invisible Stalfos present for stop music check.
// Return false if any present, true if none present or setting disabled
u8 EnemyRando_CheckInvisStalfos(Actor* actor) {
    if (INVIS_STALFOS_ENABLED) {
        Actor* enemy = gPlayState->actorCtx.actorLists[ACTORCAT_ENEMY].head;
        while (enemy != NULL) {
            if ((enemy->id == ACTOR_EN_TEST && enemy->params == STALFOS_TYPE_INVISIBLE) && enemy != actor &&
                enemy->update != NULL && ((EnTest*)enemy)->actionFunc != EnTest_WaitGround &&
                Actor_WorldDistXYZToActor(actor, enemy) <= 8000.0f) {
                return false;
            }
            enemy = enemy->next;
        }
    }
    return true;
}

// Miniboss music hooks for all enemy randomizer, regardless of music settings
void RegisterEnemyRando_MinibossMusicGeneral(void) {
    // Stop miniboss music on room switch. Always stop, in case any cvar was toggled during miniboss music
    COND_HOOK(AfterSceneCommands, ENEMY_RANDOMIZER_ENABLED, [](int16_t sceneId) {
        if (Audio_GetActiveSeqId(SEQ_PLAYER_BGM_MAIN) == NA_BGM_MINI_BOSS) {
            func_800F5B58();
        }
    });

    // Never let randomized miniboss enemy play music by itself, with exception below
    COND_VB_SHOULD(VB_PLAY_MINIBOSS_MUSIC, ENEMY_RANDOMIZER_ENABLED, { *should = false; });

    // Ensure non-randomized hooked minibosses play music
    COND_VB_SHOULD(VB_PLAY_MINIBOSS_MUSIC_IK, ENEMY_RANDOMIZER_ENABLED, {
        EnIk* enIk = va_arg(args, EnIk*);
        if (enIk->actor.params == 1280 ||
            (gPlayState->sceneNum == SCENE_INSIDE_GANONS_CASTLE &&
             !ResourceMgr_IsSceneMasterQuest(gPlayState->sceneNum) && gPlayState->roomCtx.curRoom.num == 17)) {
            *should = true;
        } else {
            *should = false;
        }
    });

    COND_VB_SHOULD(VB_PLAY_MINIBOSS_MUSIC_GELDB, ENEMY_RANDOMIZER_ENABLED, {
        EnGeldB* enGeldB = va_arg(args, EnGeldB*);
        if (enGeldB->keyFlag) {
            *should = true;
        } else {
            *should = false;
        }
    });

    COND_VB_SHOULD(VB_PLAY_MINIBOSS_MUSIC_TORCH2, ENEMY_RANDOMIZER_ENABLED, {
        Player* enTorch2 = va_arg(args, Player*);
        if (!(gPlayState->sceneNum == SCENE_WATER_TEMPLE && gPlayState->roomCtx.curRoom.num == 13)) {
            *should = false;
        }
    });

    // Always play miniboss music for Invisible Stalfos if setting enabled
    COND_VB_SHOULD(VB_PLAY_MINIBOSS_MUSIC_TEST, ENEMY_RANDOMIZER_ENABLED && !MINIBOSS_BGM_ENABLED, {
        EnTest* enTest = va_arg(args, EnTest*);
        if (enTest->actor.params == STALFOS_TYPE_INVISIBLE && INVIS_STALFOS_ENABLED) {
            *should = true;
        } else {
            *should = false;
        }
    });

    // Only let non-randomized minibosses stop miniboss music by themselves,
    // but ensure no Invisible Stalfos active
    COND_VB_SHOULD(VB_STOP_MINIBOSS_MUSIC, ENEMY_RANDOMIZER_ENABLED && !MINIBOSS_BGM_ENABLED, {
        Actor* actor = va_arg(args, Actor*);
        *should = EnemyRando_CheckInvisStalfos(actor);
    });
}

// Hooks specific to miniboss music settings
void RegisterEnemyRando_MinibossMusicSpecific(void) {
    // On scene/room switch, set appropriate sControlMusicArea value
    COND_HOOK(AfterSceneCommands, MINIBOSS_BGM_ENABLED, [](int16_t sceneId) {
        s8 roomNum = gPlayState->roomCtx.curRoom.num;
        bool mq = ResourceMgr_IsSceneMasterQuest(sceneId);

        // Control music in clear and timed rooms, but not in rooms with non-randomized minibosses
        // Exclude Shadow clear room 11 as doors are not controlled by enemy death
        if ((IsClearRoom(mq, sceneId, roomNum) || IsTimedRoom(mq, sceneId, roomNum)) &&
            !Flags_GetClear(gPlayState, roomNum) &&
            (!(sceneId == SCENE_BOTTOM_OF_THE_WELL && roomNum == 4) &&
             !(sceneId == SCENE_SHADOW_TEMPLE && (roomNum == 4 || roomNum == 11 || roomNum == 21)) &&
             !(sceneId == SCENE_FOREST_TEMPLE && roomNum == 2) &&
             !(sceneId == SCENE_SPIRIT_TEMPLE_BOSS && roomNum == 1) && !(sceneId == SCENE_JABU_JABU && roomNum == 6) &&
             !(sceneId == SCENE_DODONGOS_CAVERN && (roomNum == 3 || roomNum == 5)) &&
             !(sceneId == SCENE_INSIDE_GANONS_CASTLE && roomNum == 17 && mq) && !(sceneId == SCENE_THIEVES_HIDEOUT))) {
            sControlMusicArea = true;
        } else {
            sControlMusicArea = false;
        }
    });

    // Always play miniboss music for Invisible Stalfos if Stalfos setting enabled, but let
    // player hook handle it if clear/timed room
    COND_VB_SHOULD(VB_PLAY_MINIBOSS_MUSIC_TEST, MINIBOSS_BGM_ENABLED, {
        EnTest* enTest = va_arg(args, EnTest*);
        if (!sControlMusicArea && enTest->actor.params == STALFOS_TYPE_INVISIBLE && INVIS_STALFOS_ENABLED) {
            *should = true;
        } else {
            *should = false;
        }
    });

    // Only let non-randomized minibosses stop miniboss music by themselves.
    // Exception: Invisible Stalfos anywhere, if setting is enabled and not in control area
    COND_VB_SHOULD(VB_STOP_MINIBOSS_MUSIC, MINIBOSS_BGM_ENABLED, {
        Actor* actor = va_arg(args, Actor*);

        if (sControlMusicArea) {
            *should = false;
        } else if (INVIS_STALFOS_ENABLED) {
            *should = EnemyRando_CheckInvisStalfos(actor);
        }
    });

    // Let player update decide whether to play miniboss music for randomized miniboss enemies
    COND_HOOK(OnPlayerUpdate, MINIBOSS_BGM_ENABLED, [&]() {
        Player* player = GET_PLAYER(gPlayState);
        Actor* actor;
        s16 curSeq;
        static u8 switchTimer = 0;

        if (!sControlMusicArea) {
            return; // Miniboss or similar area
        } else if (curSeq = Audio_GetActiveSeqId(SEQ_PLAYER_BGM_MAIN), curSeq == 65535) {
            switchTimer = 0; // Original music not started, won't get stored
            return;
        } else if (gPlayState->transitionTrigger != TRANS_TRIGGER_OFF ||
                   (player->stateFlags1 & PLAYER_STATE1_LOADING) || player->actionFunc == Player_Action_80845EF8 ||
                   player->actionFunc == Player_Action_80845CA4 ||
                   (player->csAction == 2 && player->doorActor != NULL && player->doorActor->xzDistToPlayer < 100.0f)) {
            switchTimer = 28; // Ready to instantly play on room switch (and after door barred),
            return;           // but previous room enemy actors need to unload first
        } else if (switchTimer != 30) {
            switchTimer++;
            return;
        }

        // Check enemy actors for miniboss enemies
        actor = gPlayState->actorCtx.actorLists[ACTORCAT_ENEMY].head;
        while (actor != NULL) {
            if (actor->id == ACTOR_EN_TORCH2 || actor->id == ACTOR_EN_IK ||
                (actor->id == ACTOR_EN_GELDB && !(actor->shape.shadowScale != 90.0f)) ||
                (actor->id == ACTOR_EN_TEST && !(((EnTest*)actor)->actionFunc == EnTest_WaitGround)) ||
                (actor->id == ACTOR_EN_ZF &&
                 ((actor->params == ENZF_TYPE_LIZALFOS_LONE && actor->shape.shadowAlpha > 60) ||
                  (actor->params == ENZF_TYPE_DINOLFOS)))) {
                if (curSeq != NA_BGM_MINI_BOSS) {
                    func_800F5ACC(NA_BGM_MINI_BOSS);
                    switchTimer = 0;
                }
                return; // Music is playing or started now
            }
            actor = actor->next;
        }
        // No enemy found. Restore music. But unless cleared, could still be enemies hiding, don't stop control
        if (curSeq == NA_BGM_MINI_BOSS) {
            func_800F5B58();
            switchTimer = 0;
            if (Flags_GetClear(gPlayState, gPlayState->roomCtx.curRoom.num)) {
                sControlMusicArea = false;
            }
        }
    });
}

static RegisterShipInitFunc initEnemyRando_MinibossMusicGeneral(RegisterEnemyRando_MinibossMusicGeneral,
                                                                { CVAR_ENEMY_RANDOMIZER_NAME });
static RegisterShipInitFunc initEnemyRando_MinibossMusicSpecific(RegisterEnemyRando_MinibossMusicSpecific,
                                                                 { CVAR_ENEMY_RANDOMIZER_NAME,
                                                                   CVAR_MINIBOSS_BGM_NAME });
