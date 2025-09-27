/*
 * File: z_en_ru2.c
 * Overlay: En_Ru2
 * Description: Adult Ruto
 */

#include "z_en_ru2.h"
#include "objects/object_ru2/object_ru2.h"
#include "overlays/actors/ovl_Door_Warp1/z_door_warp1.h"
#include "vt.h"
#include "soh/ResourceManagerHelpers.h"
#include "soh/Enhancements/game-interactor/GameInteractor_Hooks.h"

#define FLAGS ACTOR_FLAG_UPDATE_CULLING_DISABLED

void EnRu2_Init(Actor* thisx, PlayState* play);
void EnRu2_Destroy(Actor* thisx, PlayState* play);
void EnRu2_Update(Actor* thisx, PlayState* play);
void EnRu2_Draw(Actor* thisx, PlayState* play);

void EnRu2_SetupWaterMedallionCutscene(EnRu2* this, PlayState* play);
void EnRu2_AwaitBlueWarp(EnRu2* this, PlayState* play);
void EnRu2_RiseThroughBlueWarp(EnRu2* this, PlayState* play);
void EnRu2_SageOfWaterDialog(EnRu2* this, PlayState* play);
void EnRu2_RaiseArms(EnRu2* this, PlayState* play);
void EnRu2_AwaitWaterMedallion(EnRu2* this, PlayState* play);
void EnRu2_FinishWaterMedallionCutscene(EnRu2* this, PlayState* play);
void EnRu2_WaterTrialInvisible(EnRu2* this, PlayState* play);
void EnRu2_WaterTrialFade(EnRu2* this, PlayState* play);
void EnRu2_AwaitSpawnLightBall(EnRu2* this, PlayState* play);
void EnRu2_CreditsInvisible(EnRu2* this, PlayState* play);
void EnRu2_CreditsFadeIn(EnRu2* this, PlayState* play);
void EnRu2_CreditsVisible(EnRu2* this, PlayState* play);
void EnRu2_CreditsTurnHeadDownLeft(EnRu2* this, PlayState* play);
void EnRu2_WaterTempleEncounterRangeCheck(EnRu2* this, PlayState* play);
void EnRu2_WaterTempleEncounterUnconditional(EnRu2* this, PlayState* play);
void EnRu2_WaterTempleEncounterBegin(EnRu2* this, PlayState* play);
void EnRu2_WaterTempleEncounterDialog(EnRu2* this, PlayState* play);
void EnRu2_WaterTempleEncounterEnd(EnRu2* this, PlayState* play);
void EnRu2_WaterTempleSwimmingUp(EnRu2* this, PlayState* play);

void EnRu2_DrawNothing(EnRu2* this, PlayState* play);
void EnRu2_DrawOpa(EnRu2* this, PlayState* play);
void EnRu2_DrawXlu(EnRu2* this, PlayState* play);

void EnRu2_CheckWaterMedallionCutscene(EnRu2* this, PlayState* play);

typedef enum {
    /* 00 */ ENRU2_SETUP_WATER_MEDALLION_CS,
    /* 01 */ ENRU2_AWAIT_BLUE_WARP,
    /* 02 */ ENRU2_RISE_THROUGH_BLUE_WARP,
    /* 03 */ ENRU2_SAGE_OF_WATER_DIALOG,
    /* 04 */ ENRU2_RAISE_ARMS,
    /* 05 */ ENRU2_AWAIT_SPAWN_WATER_MEDALLION,
    /* 06 */ ENRU2_FINISH_WATER_MEDALLION_CS,
    /* 07 */ ENRU2_WATER_TRIAL_INVISIBLE,
    /* 08 */ ENRU2_WATER_TRIAL_FADE,
    /* 09 */ ENRU2_AWAIT_SPAWN_LIGHT_BALL,
    /* 10 */ ENRU2_CREDITS_INVISIBLE,
    /* 11 */ ENRU2_CREDITS_FADE_IN,
    /* 12 */ ENRU2_CREDITS_VISIBLE,
    /* 13 */ ENRU2_CREDITS_TURN_HEAD_DOWN_LEFT,
    /* 14 */ ENRU2_WATER_TEMPLE_ENCOUNTER_RANGE_CHECK,
    /* 15 */ ENRU2_WATER_TEMPLE_ENCOUNTER_UNCONDITIONAL, // unused
    /* 16 */ ENRU2_WATER_TEMPLE_ENCOUNTER_BEGINNING,
    /* 17 */ ENRU2_WATER_TEMPLE_ENCOUNTER_DIALOG,
    /* 18 */ ENRU2_WATER_TEMPLE_ENCOUNTER_END,
    /* 19 */ ENRU2_WATER_TEMPLE_SWIMMING_UP,
} EnRu2Action;

typedef enum {
    /* 00 */ ENRU2_DRAW_NOTHING,
    /* 01 */ ENRU2_DRAW_OPA,
    /* 02 */ ENRU2_DRAW_XLU,
} EnRu2DrawConfig;

static ColliderCylinderInitType1 sCylinderInit = {
    {
        COLTYPE_NONE,
        AT_NONE,
        AC_ON | AC_TYPE_PLAYER,
        OC1_NONE,
        COLSHAPE_CYLINDER,
    },
    { 0x00, { 0x00000000, 0x00, 0x00 }, { 0x00000080, 0x00, 0x00 }, 0x00, 0x01, 0x00 },
    { 30, 100, 0, { 0 } },
};

static void* sEyeTextures[] = {
    gAdultRutoEyeOpenTex,
    gAdultRutoEyeHalfTex,
    gAdultRutoEyeClosedTex,
};

static UNK_TYPE D_80AF4118 = 0;

#include "z_en_ru2_cutscene_data.c" EARLY

static EnRu2ActionFunc sActionFuncs[] = {
    EnRu2_SetupWaterMedallionCutscene,
    EnRu2_AwaitBlueWarp,
    EnRu2_RiseThroughBlueWarp,
    EnRu2_SageOfWaterDialog,
    EnRu2_RaiseArms,
    EnRu2_AwaitWaterMedallion,
    EnRu2_FinishWaterMedallionCutscene,
    EnRu2_WaterTrialInvisible,
    EnRu2_WaterTrialFade,
    EnRu2_AwaitSpawnLightBall,
    EnRu2_CreditsInvisible,
    EnRu2_CreditsFadeIn,
    EnRu2_CreditsVisible,
    EnRu2_CreditsTurnHeadDownLeft,
    EnRu2_WaterTempleEncounterRangeCheck,
    EnRu2_WaterTempleEncounterUnconditional,
    EnRu2_WaterTempleEncounterBegin,
    EnRu2_WaterTempleEncounterDialog,
    EnRu2_WaterTempleEncounterEnd,
    EnRu2_WaterTempleSwimmingUp,
};

static EnRu2DrawFunc sDrawFuncs[] = {
    EnRu2_DrawNothing,
    EnRu2_DrawOpa,
    EnRu2_DrawXlu,
};

const ActorInit En_Ru2_InitVars = {
    ACTOR_EN_RU2,
    ACTORCAT_NPC,
    FLAGS,
    OBJECT_RU2,
    sizeof(EnRu2),
    (ActorFunc)EnRu2_Init,
    (ActorFunc)EnRu2_Destroy,
    (ActorFunc)EnRu2_Update,
    (ActorFunc)EnRu2_Draw,
    NULL,
};

void EnRu2_InitCollider(Actor* thisx, PlayState* play) {
    EnRu2* this = (EnRu2*)thisx;

    Collider_InitCylinder(play, &this->collider);
    Collider_SetCylinderType1(play, &this->collider, &this->actor, &sCylinderInit);
}

void EnRu2_UpdateCollider(EnRu2* this, PlayState* play) {
    s32 pad[5];

    Collider_UpdateCylinder(&this->actor, &this->collider);
    CollisionCheck_SetAC(play, &play->colChkCtx, &this->collider.base);
}

void EnRu2_Destroy(Actor* thisx, PlayState* play) {
    EnRu2* this = (EnRu2*)thisx;
    D_80AF4118 = 0;
    Collider_DestroyCylinder(play, &this->collider);

    ResourceMgr_UnregisterSkeleton(&this->skelAnime);
}

void EnRu2_UpdateEyes(EnRu2* this) {
    s32 pad[3];
    s16* blinkTimer = &this->blinkTimer;
    s16* eyeIndex = &this->eyeIndex;

    if (!DECR(*blinkTimer)) {
        *blinkTimer = Rand_S16Offset(0x3C, 0x3C);
    }

    *eyeIndex = *blinkTimer;
    if (*eyeIndex >= 3) {
        *eyeIndex = 0;
    }
}

s32 EnRu2_GetSwitchFlag(EnRu2* this) {
    s32 switchFlag = this->actor.params >> 8;

    return switchFlag & 0xFF;
}

s32 EnRu2_GetType(EnRu2* this) {
    s16 params = this->actor.params;

    return params & 0xFF;
}

void func_80AF26AC(EnRu2* this) {
    this->action = ENRU2_WATER_TRIAL_INVISIBLE;
    this->drawConfig = ENRU2_DRAW_NOTHING;
    this->alpha = 0;
    this->isLightBall = false;
    this->actor.shape.shadowAlpha = 0;
    this->fadeTimer = 0.0f;
}

void func_80AF26D0(EnRu2* this, PlayState* play) {
    s32 one; // Needed to match

    if (play->csCtx.state == CS_STATE_IDLE) {
        if (D_80AF4118 != 0) {
            // Seems like this state is never reached
            if (this->actor.params == 2) {
                func_80AF26AC(this);
            }
            D_80AF4118 = 0;
            return;
        }
    } else {
        one = 1;
        if (D_80AF4118 == 0) {
            D_80AF4118 = one;
        }
    }
}

void EnRu2_UpdateBgCheckInfo(EnRu2* this, PlayState* play) {
    Actor_UpdateBgCheckInfo(play, &this->actor, 75.0f, 30.0f, 30.0f, 4);
}

s32 EnRu2_UpdateSkelAnime(EnRu2* this) {
    return SkelAnime_Update(&this->skelAnime);
}

CsCmdActorCue* EnRu2_GetCue(PlayState* play, s32 cueChannel) {
    if (play->csCtx.state != CS_STATE_IDLE) {
        return play->csCtx.npcActions[cueChannel];
    }
    return NULL;
}

s32 EnRu2_CheckCueMatchingId(EnRu2* this, PlayState* play, u16 cueId, s32 cueChannel) {
    CsCmdActorCue* cue = EnRu2_GetCue(play, cueChannel);

    if ((cue != NULL) && (cue->action == cueId)) {
        return true;
    }
    return false;
}

s32 EnRu2_CheckCueNotMatchingId(EnRu2* this, PlayState* play, u16 cueId, s32 cueChannel) {
    CsCmdActorCue* cue = EnRu2_GetCue(play, cueChannel);

    if ((cue != NULL) && (cue->action != cueId)) {
        return true;
    }
    return false;
}

/**
 * Checks cutscene data and, if applicable, configures Ruto's position accordingly.
 */
void EnRu2_InitPositionFromCue(EnRu2* this, PlayState* play, u32 npcActionIdx) {
    CsCmdActorCue* cue = EnRu2_GetCue(play, npcActionIdx);
    s16 newRotY;
    Actor* thisx = &this->actor;

    if (cue != NULL) {
        thisx->world.pos.x = cue->startPos.x;
        thisx->world.pos.y = cue->startPos.y;
        thisx->world.pos.z = cue->startPos.z;
        newRotY = cue->rot.y;
        thisx->shape.rot.y = newRotY;
        thisx->world.rot.y = newRotY;
    }
}

/**
 * Changes the animation for Ruto's actor. The direction argument decides whether to play the animation
 * forwards (if 0) or backwards (otherwise).
 */
void EnRu2_AnimationChange(EnRu2* this, AnimationHeader* animation, u8 mode, f32 transitionRate, s32 direction) {
    f32 frameCount = Animation_GetLastFrame(animation);
    f32 playbackSpeed;
    f32 startFrame;
    f32 endFrame;

    if (direction == 0) {
        startFrame = 0.0f;
        endFrame = frameCount;
        playbackSpeed = 1.0f;
    } else {
        startFrame = frameCount;
        endFrame = 0.0f;
        playbackSpeed = -1.0f;
    }

    Animation_Change(&this->skelAnime, animation, playbackSpeed, startFrame, endFrame, mode, transitionRate);
}

/**
 * Gradually increases Ruto's model's Y-offset as she rises up through the blue warp in the Chamber of Sages.
 */
void EnRu2_Rise(EnRu2* this, PlayState* play) {
    this->actor.shape.yOffset += 250.0f / 3.0f;
}

/**
 * Sets up Ruto's actor in the Chamber of Sages.
 * Note: All sages actors are present in the Chamber of Sages, regardless of which dungeon was just completed.
 * This function runs unconditionally, even if it is not relevant for Ruto.
 */
void EnRu2_InitChamberOfSages(EnRu2* this, PlayState* play) {
    EnRu2_AnimationChange(this, &gAdultRutoIdleAnim, 0, 0.0f, 0);
    this->actor.shape.yOffset = -10000.0f;
}

/**
 * Spawns the blue warp for Ruto to rise up through in the Chamber of Sages.
 */
void EnRu2_SpawnBlueWarp(EnRu2* this, PlayState* play) {
    Actor* thisx = &this->actor;
    f32 posX = thisx->world.pos.x;
    f32 posY = thisx->world.pos.y;
    f32 posZ = thisx->world.pos.z;

    Actor_SpawnAsChild(&play->actorCtx, &this->actor, play, ACTOR_DOOR_WARP1, posX, posY, posZ, 0, 0, 0, WARP_SAGES);
}

/**
 * Spawns the Water Medallion.
 */
void EnRu2_SpawnWaterMedallion(EnRu2* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    f32 posX = player->actor.world.pos.x;
    f32 posY = player->actor.world.pos.y + 50.0f;
    f32 posZ = player->actor.world.pos.z;

    Actor_SpawnAsChild(&play->actorCtx, &this->actor, play, ACTOR_DEMO_EFFECT, posX, posY, posZ, 0, 0, 0, 10);
    // Give the water medallion. This is redundant as it was already given in `EnRu2_CheckWaterMedallionCutscene`
    if (GameInteractor_Should(VB_GIVE_ITEM_WATER_MEDALLION, true)) {
        Item_Give(play, ITEM_MEDALLION_WATER);
    }
}

/**
 * Sets up the Water Medallion Cutscene if coming from Water Temple.
 * All sage actors are present in the Chamber of Sages regardless of which dungeon was just completed.
 * This function will loop endlessly if the current sage cutscene is not for the Water Medallion.
 */
void EnRu2_CheckWaterMedallionCutscene(EnRu2* this, PlayState* play) {
    s32 pad[2];
    Player* player;
    s16 yaw;

    if ((gSaveContext.chamberCutsceneNum == 2) && (gSaveContext.sceneSetupIndex < 4)) {
        player = GET_PLAYER(play);
        this->action = ENRU2_AWAIT_BLUE_WARP;
        play->csCtx.segment = &gWaterMedallionCs;
        gSaveContext.cutsceneTrigger = 2;
        if (GameInteractor_Should(VB_GIVE_ITEM_WATER_MEDALLION, true)) {
            Item_Give(play, ITEM_MEDALLION_WATER);
        }
        yaw = this->actor.world.rot.y + 0x8000;
        player->actor.shape.rot.y = yaw;
        player->actor.world.rot.y = yaw;
    }
}

void EnRu2_CheckIfBlueWarpShouldSpawn(EnRu2* this, PlayState* play) {
    CutsceneContext* csCtx = &play->csCtx;
    CsCmdActorCue* cue;

    if (csCtx->state != CS_STATE_IDLE) {
        cue = csCtx->npcActions[3];
        if ((cue != NULL) && (cue->action == 2)) {
            this->action = ENRU2_RISE_THROUGH_BLUE_WARP;
            this->drawConfig = ENRU2_DRAW_OPA;
            EnRu2_SpawnBlueWarp(this, play);
        }
    }
}

/* Halts Ruto's rise up through the blue warp in the Chamber of Sages once finished. */
void EnRu2_EndRise(EnRu2* this) {
    if (this->actor.shape.yOffset >= 0.0f) {
        this->action = ENRU2_SAGE_OF_WATER_DIALOG;
        this->actor.shape.yOffset = 0.0f;
    }
}

/**
 * Sets up the animation for Ruto to raise her arms to give Link the Water Medallion.
 */
void EnRu2_CheckStartRaisingArms(EnRu2* this, PlayState* play) {
    AnimationHeader* animation = &gAdultRutoRaisingArmsUpAnim;
    CsCmdActorCue* cue;

    if (play->csCtx.state != CS_STATE_IDLE) {
        cue = play->csCtx.npcActions[3];
        if ((cue != NULL) && (cue->action == 3)) {
            Animation_Change(&this->skelAnime, animation, 1.0f, 0.0f, Animation_GetLastFrame(animation), ANIMMODE_ONCE,
                             0.0f);
            this->action = ENRU2_RAISE_ARMS;
        }
    }
}

/**
 * At the end of Ruto's arms-raising animation, cues the next action: spawning the
 * Water Medallion.
 */
void EnRu2_HoldArmsUp(EnRu2* this, s32 doneRaising) {
    if (doneRaising != 0) {
        this->action = ENRU2_AWAIT_SPAWN_WATER_MEDALLION;
    }
}

/**
 * Checks to see if the Water Medallion should spawn.
 */
void EnRu2_CheckIfWaterMedallionShouldSpawn(EnRu2* this, PlayState* play) {
    CsCmdActorCue* cue;

    if (play->csCtx.state != CS_STATE_IDLE) {
        cue = play->csCtx.npcActions[6];
        if ((cue != NULL) && (cue->action == 2)) {
            this->action = ENRU2_FINISH_WATER_MEDALLION_CS;
            EnRu2_SpawnWaterMedallion(this, play);
        }
    }
}

void EnRu2_SetupWaterMedallionCutscene(EnRu2* this, PlayState* play) {
    EnRu2_CheckWaterMedallionCutscene(this, play);
}

void EnRu2_AwaitBlueWarp(EnRu2* this, PlayState* play) {
    EnRu2_CheckIfBlueWarpShouldSpawn(this, play);
}

void EnRu2_RiseThroughBlueWarp(EnRu2* this, PlayState* play) {
    EnRu2_Rise(this, play);
    EnRu2_UpdateSkelAnime(this);
    EnRu2_UpdateEyes(this);
    EnRu2_EndRise(this);
}

void EnRu2_SageOfWaterDialog(EnRu2* this, PlayState* play) {
    EnRu2_UpdateBgCheckInfo(this, play);
    EnRu2_UpdateSkelAnime(this);
    EnRu2_UpdateEyes(this);
    EnRu2_CheckStartRaisingArms(this, play);
}

void EnRu2_RaiseArms(EnRu2* this, PlayState* play) {
    s32 animDone;

    EnRu2_UpdateBgCheckInfo(this, play);
    animDone = EnRu2_UpdateSkelAnime(this);
    EnRu2_UpdateEyes(this);
    EnRu2_HoldArmsUp(this, animDone);
}

void EnRu2_AwaitWaterMedallion(EnRu2* this, PlayState* play) {
    EnRu2_UpdateBgCheckInfo(this, play);
    EnRu2_UpdateSkelAnime(this);
    EnRu2_UpdateEyes(this);
    EnRu2_CheckIfWaterMedallionShouldSpawn(this, play);
}

void EnRu2_FinishWaterMedallionCutscene(EnRu2* this, PlayState* play) {
    EnRu2_UpdateBgCheckInfo(this, play);
    EnRu2_UpdateSkelAnime(this);
    EnRu2_UpdateEyes(this);
}

/**
 * Sets up Ruto in her arms-crossing pose. Used in the Water Trial in Ganon's Castle and in the
 * Chamber of Sages during the "Sealing Ganon" cutscene.
 */
void EnRu2_InitWaterTrial(EnRu2* this, PlayState* play) {
    EnRu2_AnimationChange(this, &gAdultRutoCrossingArmsAnim, 2, 0.0f, 0);
    this->action = ENRU2_WATER_TRIAL_INVISIBLE;
    this->actor.shape.shadowAlpha = 0;
}

void EnRu2_PlayWhiteOutSound() {
    Sfx_PlaySfxCentered2(NA_SE_SY_WHITE_OUT_T);
}

/**
 * Spawns the ball of light that replaces Ruto's actor in the Water Trial.
 */
void EnRu2_SpawnLightBall(EnRu2* this, PlayState* play) {
    Actor_SpawnAsChild(&play->actorCtx, &this->actor, play, ACTOR_DEMO_6K, this->actor.world.pos.x,
                       kREG(19) + 24.0f + this->actor.world.pos.y, this->actor.world.pos.z, 0, 0, 0, 8);
}

/**
 * Checks to see if it's time for Ruto to fade in while crossing her arms.
 */
void EnRu2_CheckFadeIn(EnRu2* this, PlayState* play) {
    if (EnRu2_CheckCueMatchingId(this, play, 4, 3)) {
        this->action = ENRU2_WATER_TRIAL_FADE;
        this->drawConfig = ENRU2_DRAW_XLU;
        this->alpha = 0;
        this->actor.shape.shadowAlpha = 0;
        this->fadeTimer = 0.0f;
        EnRu2_PlayWhiteOutSound();
    }
}

/**
 * Fades Ruto's actor in or out. Both happen during the Water Trial.
 */
void EnRu2_Fade(EnRu2* this, PlayState* play) {
    f32* fadeTimer = &this->fadeTimer;
    s32 alpha;

    if (EnRu2_CheckCueMatchingId(this, play, 4, 3)) {
        *fadeTimer += 1.0f;
        if (*fadeTimer >= kREG(5) + 10.0f) {
            this->action = ENRU2_AWAIT_SPAWN_LIGHT_BALL;
            this->drawConfig = ENRU2_DRAW_OPA;
            *fadeTimer = kREG(5) + 10.0f;
            this->alpha = 255;
            this->actor.shape.shadowAlpha = 0xFF;
            return;
        }
    } else {
        *fadeTimer -= 1.0f;
        if (*fadeTimer <= 0.0f) {
            this->action = ENRU2_WATER_TRIAL_INVISIBLE;
            this->drawConfig = ENRU2_DRAW_NOTHING;
            *fadeTimer = 0.0f;
            this->alpha = 0;
            this->actor.shape.shadowAlpha = 0;
            return;
        }
    }
    alpha = (*fadeTimer / (kREG(5) + 10.0f)) * 255.0f;
    this->alpha = alpha;
    this->actor.shape.shadowAlpha = alpha;
}

/**
 * Checks to see if it's time for Ruto to fade out while her arms are crossed.
 */
void EnRu2_CheckFadeOut(EnRu2* this, PlayState* play) {
    if (EnRu2_CheckCueNotMatchingId(this, play, 4, 3)) {
        this->action = ENRU2_WATER_TRIAL_FADE;
        this->drawConfig = ENRU2_DRAW_XLU;
        this->fadeTimer = kREG(5) + 10.0f;
        this->alpha = 255;
        if (!this->isLightBall) {
            EnRu2_SpawnLightBall(this, play);
            this->isLightBall = true;
        }
        this->actor.shape.shadowAlpha = 0xFF;
    }
}

void EnRu2_WaterTrialInvisible(EnRu2* this, PlayState* play) {
    EnRu2_CheckFadeIn(this, play);
    func_80AF26D0(this, play);
}

void EnRu2_WaterTrialFade(EnRu2* this, PlayState* play) {
    EnRu2_UpdateBgCheckInfo(this, play);
    EnRu2_UpdateSkelAnime(this);
    EnRu2_UpdateEyes(this);
    EnRu2_Fade(this, play);
    func_80AF26D0(this, play);
}

void EnRu2_AwaitSpawnLightBall(EnRu2* this, PlayState* play) {
    EnRu2_UpdateBgCheckInfo(this, play);
    EnRu2_UpdateSkelAnime(this);
    EnRu2_UpdateEyes(this);
    EnRu2_CheckFadeOut(this, play);
    func_80AF26D0(this, play);
}

void EnRu2_DrawXlu(EnRu2* this, PlayState* play) {
    s32 pad[2];
    s16 eyeIndex = this->eyeIndex;
    void* tex = sEyeTextures[eyeIndex];
    SkelAnime* skelAnime = &this->skelAnime;

    OPEN_DISPS(play->state.gfxCtx);

    Gfx_SetupDL_25Xlu(play->state.gfxCtx);

    gSPSegment(POLY_XLU_DISP++, 0x08, SEGMENTED_TO_VIRTUAL(tex));
    gSPSegment(POLY_XLU_DISP++, 0x09, SEGMENTED_TO_VIRTUAL(tex));
    gDPSetEnvColor(POLY_XLU_DISP++, 0, 0, 0, this->alpha);
    gSPSegment(POLY_XLU_DISP++, 0x0C, &D_80116280[0]);

    POLY_XLU_DISP = SkelAnime_DrawFlex(play, skelAnime->skeleton, skelAnime->jointTable, skelAnime->dListCount, NULL,
                                       NULL, NULL, POLY_XLU_DISP);

    CLOSE_DISPS(play->state.gfxCtx);
}

/**
 * Sets up Ruto's hands-on-hips pose during the credits sequence.
 */
void EnRu2_InitCredits(EnRu2* this, PlayState* play) {
    EnRu2_AnimationChange(this, &gAdultRutoIdleHandsOnHipsAnim, 0, 0.0f, 0);
    this->action = ENRU2_CREDITS_INVISIBLE;
    this->drawConfig = ENRU2_DRAW_NOTHING;
    this->actor.shape.shadowAlpha = 0;
}

/**
 * Fades in Ruto's actor during the credits sequence.
 */
void EnRu2_FadeInCredits(EnRu2* this) {
    f32* fadeTimer = &this->fadeTimer;
    f32 fadeDuration;
    s32 alpha;

    *fadeTimer += 1.0f;

    fadeDuration = kREG(17) + 10.0f;
    if (fadeDuration <= *fadeTimer) {
        this->alpha = 255;
        this->actor.shape.shadowAlpha = 0xFF;
    } else {
        alpha = (*fadeTimer / fadeDuration) * 255.0f;
        this->alpha = alpha;
        this->actor.shape.shadowAlpha = alpha;
    }
}

void EnRu2_InitCreditsPosition(EnRu2* this, PlayState* play) {
    EnRu2_InitPositionFromCue(this, play, 3);
    this->action = ENRU2_CREDITS_FADE_IN;
    this->drawConfig = ENRU2_DRAW_XLU;
}

/**
 * Checks for the end of Ruto's fade-in during the credits sequence.
 */
void EnRu2_CheckVisibleInCredits(EnRu2* this) {
    if (this->fadeTimer >= kREG(17) + 10.0f) {
        this->action = ENRU2_CREDITS_VISIBLE;
        this->drawConfig = ENRU2_DRAW_OPA;
    }
}

/**
 * Starts Ruto's animation to look down towards Nabooru during the credits sequence.
 */
void EnRu2_SetupTurnHeadDownLeftAnimation(EnRu2* this) {
    EnRu2_AnimationChange(this, &gAdultRutoHeadTurnDownLeftAnim, 2, 0.0f, 0);
    this->action = ENRU2_CREDITS_TURN_HEAD_DOWN_LEFT;
}

/**
 * Holds Ruto's pose looking down towards Nabooru during the credits sequence.
 */
void EnRu2_HoldLookingDownLeftPose(EnRu2* this, s32 isDoneTurning) {
    if (isDoneTurning != 0) {
        EnRu2_AnimationChange(this, &gAdultRutoLookingDownLeftAnim, 0, 0.0f, 0);
    }
}

/**
 * Advances Ruto's actions in two different places.
 */
void EnRu2_NextCreditsAction(EnRu2* this, PlayState* play) {
    CsCmdActorCue* cue = EnRu2_GetCue(play, 3);
    s32 nextCueId;
    s32 currentCueId;

    if (cue != NULL) {
        nextCueId = cue->action;
        currentCueId = this->cueId;
        if (nextCueId != currentCueId) {
            switch (nextCueId) {
                case 7:
                    EnRu2_InitCreditsPosition(this, play);
                    break;
                case 8:
                    EnRu2_SetupTurnHeadDownLeftAnimation(this);
                    break;
                default:
                    // "There is no such action!"
                    osSyncPrintf("En_Ru2_inEnding_Check_DemoMode:そんな動作は無い!!!!!!!!\n");
                    break;
            }
            this->cueId = nextCueId;
        }
    }
}

void EnRu2_CreditsInvisible(EnRu2* this, PlayState* play) {
    EnRu2_NextCreditsAction(this, play);
}

void EnRu2_CreditsFadeIn(EnRu2* this, PlayState* play) {
    EnRu2_UpdateBgCheckInfo(this, play);
    EnRu2_UpdateSkelAnime(this);
    EnRu2_UpdateEyes(this);
    EnRu2_FadeInCredits(this);
    EnRu2_CheckVisibleInCredits(this);
}

void EnRu2_CreditsVisible(EnRu2* this, PlayState* play) {
    EnRu2_UpdateBgCheckInfo(this, play);
    EnRu2_UpdateSkelAnime(this);
    EnRu2_UpdateEyes(this);
    EnRu2_NextCreditsAction(this, play);
}

void EnRu2_CreditsTurnHeadDownLeft(EnRu2* this, PlayState* play) {
    s32 animDone;

    EnRu2_UpdateBgCheckInfo(this, play);
    animDone = EnRu2_UpdateSkelAnime(this);
    EnRu2_UpdateEyes(this);
    EnRu2_HoldLookingDownLeftPose(this, animDone);
}

void EnRu2_SetEncounterSwitchFlag(EnRu2* this, PlayState* play) {
    Flags_SetSwitch(play, EnRu2_GetSwitchFlag(this));
}

s32 EnRu2_GetEncounterSwitchFlag(EnRu2* this, PlayState* play) {
    return Flags_GetSwitch(play, EnRu2_GetSwitchFlag(this));
}

/**
 * Initializes Ruto's actor in the Water Temple, or destroys it if the encounter already happened.
 */
void EnRu2_InitWaterTempleEncounter(EnRu2* this, PlayState* play) {
    if (EnRu2_GetEncounterSwitchFlag(this, play)) {
        Actor_Kill(&this->actor);
    } else {
        EnRu2_AnimationChange(this, &gAdultRutoIdleAnim, 0, 0.0f, 0);
        this->action = ENRU2_WATER_TEMPLE_ENCOUNTER_RANGE_CHECK;
        this->drawConfig = ENRU2_DRAW_OPA;
    }
}

void EnRu2_PlayFanfare(void) {
    Audio_PlayFanfare(NA_BGM_APPEAR);
}

/**
 * Accelerates Ruto's actor upwards as she swims.
 */
void EnRu2_SwimUpProgress(EnRu2* this) {
    f32 funcFloat;

    this->swimmingUpFrame++;
    funcFloat = Environment_LerpWeightAccelDecel((kREG(2) + 0x96) & 0xFFFF, 0, this->swimmingUpFrame, 8, 0);
    this->actor.world.pos.y = this->actor.home.pos.y + (300.0f * funcFloat);
}

s32 EnRu2_IsPlayerInRangeForEncounter(EnRu2* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    f32 thisPosX = this->actor.world.pos.x;
    f32 playerPosX = player->actor.world.pos.x;

    if (playerPosX - thisPosX >= -202.0f) {
        return 1;
    }
    return 0;
}

/**
 * Checks if Link is close enough to Ruto and conditionally triggers the encounter cutscene in the Water Temple.
 */
void EnRu2_CheckRangeToStartEncounter(EnRu2* this, PlayState* play) {
    if (EnRu2_IsPlayerInRangeForEncounter(this, play) && !Play_InCsMode(play)) {
        this->action = ENRU2_WATER_TEMPLE_ENCOUNTER_BEGINNING;
        this->subCamId = OnePointCutscene_Init(play, 3130, -99, &this->actor, MAIN_CAM);
    }
}

/**
 * Triggers the encounter cutscene in the Water Temple, unconditionally. Appears to be unused.
 */
void EnRu2_StartEncounter(EnRu2* this, PlayState* play) {
    this->action = ENRU2_WATER_TEMPLE_ENCOUNTER_BEGINNING;
    this->subCamId = OnePointCutscene_Init(play, 3130, -99, &this->actor, MAIN_CAM);
}

/**
 * Handles the starting moments of Ruto's encounter with Link at the Water Temple. Responds to a running timer to
 * initiate, on cue, both the fanfare and Ruto's dialogue.
 */
void EnRu2_EncounterBeginningHandler(EnRu2* this, PlayState* play) {
    f32* encounterTimer = &this->encounterTimer;

    *encounterTimer += 1.0f;
    if (*encounterTimer == kREG(6) + 40.0f) {
        EnRu2_PlayFanfare();
    } else if (*encounterTimer > kREG(4) + 50.0f) {
        this->actor.textId = 0x403E;
        Message_StartTextbox(play, this->actor.textId, NULL);
        this->action = ENRU2_WATER_TEMPLE_ENCOUNTER_DIALOG;
    }
}

void EnRu2_DialogCameraHandler(EnRu2* this, PlayState* play) {
    s32 pad;
    MessageContext* msgCtx;
    s32 pad2;
    u8 dialogState;
    Player* player;
    s32 pad3;

    msgCtx = &play->msgCtx;
    dialogState = Message_GetState(msgCtx);

    if (dialogState == TEXT_STATE_DONE_FADING) {
        if (this->lastDialogState != TEXT_STATE_DONE_FADING) {
            // "I'm Komatsu!" (cinema scene dev)
            osSyncPrintf("おれが小松だ！ \n");
            this->textboxCount++;
            if (this->textboxCount % 6 == 3) {
                player = GET_PLAYER(play);
                // "uorya-!" (screeming sound)
                osSyncPrintf("うおりゃー！ \n");
                func_8005B1A4(GET_ACTIVE_CAM(play));
                player->actor.world.pos.x = 820.0f;
                player->actor.world.pos.y = 0.0f;
                player->actor.world.pos.z = 180.0f;
            }
        }
    }

    this->lastDialogState = dialogState;
    if (Message_GetState(msgCtx) == TEXT_STATE_CLOSING) {
        this->action = ENRU2_WATER_TEMPLE_ENCOUNTER_END;
        func_8005B1A4(GET_ACTIVE_CAM(play));
    }
}

void EnRu2_StartSwimmingUp(EnRu2* this, PlayState* play) {
    this->encounterTimer += 1.0f;
    if (this->encounterTimer > kREG(5) + 100.0f) {
        EnRu2_AnimationChange(this, &gAdultRutoSwimmingUpAnim, 0, -12.0f, 0);
        this->action = ENRU2_WATER_TEMPLE_SWIMMING_UP;
        EnRu2_SetEncounterSwitchFlag(this, play);
    }
}

void EnRu2_EndSwimmingUp(EnRu2* this, PlayState* play) {
    if (this->swimmingUpFrame > ((((u16)(kREG(3) + 0x28)) + ((u16)(kREG(2) + 0x96))) & 0xFFFF)) {
        Actor_Kill(&this->actor);
        OnePointCutscene_EndCutscene(play, this->subCamId);
    }
}

void EnRu2_WaterTempleEncounterRangeCheck(EnRu2* this, PlayState* play) {
    EnRu2_CheckRangeToStartEncounter(this, play);
    Actor_SetFocus(&this->actor, 50.0f);
    EnRu2_UpdateCollider(this, play);
}

// This one seems to be unused.
void EnRu2_WaterTempleEncounterUnconditional(EnRu2* this, PlayState* play) {
    EnRu2_UpdateBgCheckInfo(this, play);
    EnRu2_UpdateCollider(this, play);
    EnRu2_UpdateSkelAnime(this);
    EnRu2_UpdateEyes(this);
    Actor_SetFocus(&this->actor, 50.0f);
    EnRu2_StartEncounter(this, play);
}

void EnRu2_WaterTempleEncounterBegin(EnRu2* this, PlayState* play) {
    EnRu2_UpdateBgCheckInfo(this, play);
    EnRu2_UpdateSkelAnime(this);
    EnRu2_UpdateEyes(this);
    Actor_SetFocus(&this->actor, 50.0f);
    EnRu2_EncounterBeginningHandler(this, play);
}

void EnRu2_WaterTempleEncounterDialog(EnRu2* this, PlayState* play) {
    EnRu2_UpdateBgCheckInfo(this, play);
    EnRu2_UpdateSkelAnime(this);
    EnRu2_UpdateEyes(this);
    Actor_SetFocus(&this->actor, 50.0f);
    EnRu2_DialogCameraHandler(this, play);
}

void EnRu2_WaterTempleEncounterEnd(EnRu2* this, PlayState* play) {
    EnRu2_UpdateBgCheckInfo(this, play);
    EnRu2_UpdateSkelAnime(this);
    EnRu2_UpdateEyes(this);
    Actor_SetFocus(&this->actor, 50.0f);
    EnRu2_StartSwimmingUp(this, play);
}

void EnRu2_WaterTempleSwimmingUp(EnRu2* this, PlayState* play) {
    EnRu2_SwimUpProgress(this);
    EnRu2_UpdateBgCheckInfo(this, play);
    EnRu2_UpdateSkelAnime(this);
    EnRu2_UpdateEyes(this);
    Actor_SetFocus(&this->actor, 50.0f);
    EnRu2_EndSwimmingUp(this, play);
}

void EnRu2_Update(Actor* thisx, PlayState* play) {
    EnRu2* this = (EnRu2*)thisx;

    if ((this->action < 0) || (this->action >= ARRAY_COUNT(sActionFuncs)) || (sActionFuncs[this->action] == NULL)) {
        // "Main Mode is improper!"
        osSyncPrintf(VT_FGCOL(RED) "メインモードがおかしい!!!!!!!!!!!!!!!!!!!!!!!!!\n" VT_RST);
        return;
    }
    sActionFuncs[this->action](this, play);
}

void EnRu2_Init(Actor* thisx, PlayState* play) {
    EnRu2* this = (EnRu2*)thisx;

    ActorShape_Init(&this->actor.shape, 0.0f, ActorShadow_DrawCircle, 30.0f);
    EnRu2_InitCollider(thisx, play);
    SkelAnime_InitFlex(play, &this->skelAnime, &gAdultRutoSkel, NULL, this->jointTable, this->morphTable, 23);

    switch (EnRu2_GetType(this)) {
        case 2:
            EnRu2_InitWaterTrial(this, play);
            break;
        case 3:
            EnRu2_InitCredits(this, play);
            break;
        case 4:
            EnRu2_InitWaterTempleEncounter(this, play);
            break;
        default:
            EnRu2_InitChamberOfSages(this, play);
            break;
    }

    this->textboxCount = 0;
    this->lastDialogState = TEXT_STATE_DONE_FADING;
    this->subCamId = 0;
}

void EnRu2_DrawNothing(EnRu2* this, PlayState* play) {
}

void EnRu2_DrawOpa(EnRu2* this, PlayState* play) {
    s32 pad[2];
    s16 eyeIndex = this->eyeIndex;
    void* tex = sEyeTextures[eyeIndex];
    SkelAnime* skelAnime = &this->skelAnime;

    OPEN_DISPS(play->state.gfxCtx);

    Gfx_SetupDL_25Opa(play->state.gfxCtx);

    gSPSegment(POLY_OPA_DISP++, 0x08, SEGMENTED_TO_VIRTUAL(tex));
    gSPSegment(POLY_OPA_DISP++, 0x09, SEGMENTED_TO_VIRTUAL(tex));
    gDPSetEnvColor(POLY_OPA_DISP++, 0, 0, 0, 255);
    gSPSegment(POLY_OPA_DISP++, 0x0C, &D_80116280[2]);

    SkelAnime_DrawSkeletonOpa(play, skelAnime, NULL, NULL, this);

    CLOSE_DISPS(play->state.gfxCtx);
}

void EnRu2_Draw(Actor* thisx, PlayState* play) {
    EnRu2* this = (EnRu2*)thisx;

    if ((this->drawConfig < 0) || (this->drawConfig >= ARRAY_COUNT(sDrawFuncs)) ||
        (sDrawFuncs[this->drawConfig] == 0)) {
        // "Draw Mode is improper!"
        osSyncPrintf(VT_FGCOL(RED) "描画モードがおかしい!!!!!!!!!!!!!!!!!!!!!!!!!\n" VT_RST);
        return;
    }
    sDrawFuncs[this->drawConfig](this, play);
}
