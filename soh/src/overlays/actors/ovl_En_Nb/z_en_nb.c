/*
 * File: z_en_nb.c
 * Overlay: ovl_En_Nb
 * Description: Nabooru
 */

#include "z_en_nb.h"
#include "vt.h"
#include "objects/object_nb/object_nb.h"
#include "overlays/actors/ovl_Door_Warp1/z_door_warp1.h"

#define FLAGS ACTOR_FLAG_4

typedef enum {
    /* 0x00 */ NB_CHAMBER_INIT,
    /* 0x01 */ NB_CHAMBER_UNDERGROUND,
    /* 0x02 */ NB_CHAMBER_APPEAR,
    /* 0x03 */ NB_CHAMBER_IDLE,
    /* 0x04 */ NB_CHAMBER_RAISE_ARM,
    /* 0x05 */ NB_CHAMBER_RAISE_ARM_TRANSITION,
    /* 0x06 */ NB_GIVE_MEDALLION,
    /* 0x07 */ NB_ACTION_7,
    /* 0x08 */ NB_SEAL_HIDE,
    /* 0x09 */ NB_ACTION_9,
    /* 0x0A */ NB_KIDNAPPED,
    /* 0x0B */ NB_KIDNAPPED_LOOK_AROUND,
    /* 0x0C */ NB_PORTAL_FALLTHROUGH,
    /* 0x0D */ NB_IN_CONFRONTATION,
    /* 0x0E */ NB_ACTION_14,
    /* 0x0F */ NB_KNEEL,
    /* 0x10 */ NB_LOOK_RIGHT,
    /* 0x11 */ NB_LOOK_LEFT,
    /* 0x12 */ NB_RUN,
    /* 0x13 */ NB_CONFRONTATION_DESTROYED,
    /* 0x14 */ NB_CREDITS_INIT,
    /* 0x15 */ NB_CREDITS_FADEIN,
    /* 0x16 */ NB_CREDITS_SIT,
    /* 0x17 */ NB_CREDITS_HEAD_TURN,
    /* 0x18 */ NB_CROUCH_CRAWLSPACE,
    /* 0x19 */ NB_NOTICE_PLAYER,
    /* 0x1A */ NB_IDLE_CRAWLSPACE,
    /* 0x1B */ NB_IN_DIALOG,
    /* 0x1C */ NB_IN_PATH,
    /* 0x1D */ NB_IDLE_AFTER_TALK,
    /* 0x1E */ NB_ACTION_30
} EnNbAction;

typedef enum {
    /* 0x00 */ NB_DRAW_NOTHING,
    /* 0x01 */ NB_DRAW_DEFAULT,
    /* 0x02 */ NB_DRAW_HIDE,
    /* 0x03 */ NB_DRAW_KNEEL,
    /* 0x04 */ NB_DRAW_LOOK_DIRECTION
} EnNbDrawMode;

void EnNb_Init(Actor* thisx, PlayState* play);
void EnNb_Destroy(Actor* thisx, PlayState* play);
void EnNb_Update(Actor* thisx, PlayState* play);
void EnNb_Draw(Actor* thisx, PlayState* play);

static ColliderCylinderInitType1 sCylinderInit = {
    {
        COLTYPE_HIT0,
        AT_NONE,
        AC_NONE,
        OC1_ON | OC1_TYPE_PLAYER,
        COLSHAPE_CYLINDER,
    },
    {
        ELEMTYPE_UNK0,
        { 0x00000000, 0x00, 0x00 },
        { 0x00000000, 0x00, 0x00 },
        TOUCH_NONE,
        BUMP_NONE,
        OCELEM_ON,
    },
    { 25, 80, 0, { 0, 0, 0 } },
};

static void* sEyeTextures[] = {
    gNabooruEyeOpenTex,
    gNabooruEyeHalfTex,
    gNabooruEyeClosedTex,
};

static s32 D_80AB4318 = 0;

#include "z_en_nb_cutscene_data.c" EARLY

s32 EnNb_GetPath(EnNb* this) {
    s32 path = this->actor.params >> 8;

    return path & 0xFF;
}

s32 EnNb_GetType(EnNb* this) {
    s32 type = this->actor.params;

    return type & 0xFF;
}

void EnNb_UpdatePath(EnNb* this, PlayState* play) {
    Vec3s* pointPos;
    Path* pathList;
    s32 pad;
    s32 path;

    pathList = play->setupPathList;

    if (pathList != NULL) {
        path = EnNb_GetPath(this);
        pathList += path;
        pointPos = SEGMENTED_TO_VIRTUAL(pathList->points);
        this->initialPos.x = pointPos[0].x;
        this->initialPos.y = pointPos[0].y;
        this->initialPos.z = pointPos[0].z;
        this->finalPos.x = pointPos[1].x;
        this->finalPos.y = pointPos[1].y;
        this->finalPos.z = pointPos[1].z;
        this->pathYaw = (Math_FAtan2F(this->finalPos.x - this->initialPos.x, this->finalPos.z - this->initialPos.z) *
                         (0x8000 / M_PI));
        // "En_Nb_Get_path_info Rail Data Get! = %d!!!!!!!!!!!!!!"
        osSyncPrintf("En_Nb_Get_path_info レールデータをゲットだぜ = %d!!!!!!!!!!!!!!\n", path);
    } else {
        // "En_Nb_Get_path_info Rail Data Doesn't Exist!!!!!!!!!!!!!!!!!!!!"
        osSyncPrintf("En_Nb_Get_path_info レールデータが無い!!!!!!!!!!!!!!!!!!!!\n");
    }
}

void EnNb_SetupCollider(Actor* thisx, PlayState* play) {
    EnNb* this = (EnNb*)thisx;

    Collider_InitCylinder(play, &this->collider);
    Collider_SetCylinderType1(play, &this->collider, thisx, &sCylinderInit);
}

void EnNb_UpdateCollider(EnNb* this, PlayState* play) {
    s32 pad[4];
    ColliderCylinder* collider = &this->collider;

    Collider_UpdateCylinder(&this->actor, collider);
    CollisionCheck_SetOC(play, &play->colChkCtx, &collider->base);
}

void EnNb_Destroy(Actor* thisx, PlayState* play) {
    EnNb* this = (EnNb*)thisx;
    
    D_80AB4318 = 0;
    Collider_DestroyCylinder(play, &this->collider);
}

void func_80AB0FBC(EnNb* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    this->unk_300.unk_18 = player->actor.world.pos;
    this->unk_300.unk_14 = kREG(16) + 9.0f;
    func_80034A14(&this->actor, &this->unk_300, kREG(17) + 0xC, 2);
}

void func_80AB1040(EnNb* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    this->unk_300.unk_18 = player->actor.world.pos;
    this->unk_300.unk_14 = kREG(16) + 9.0f;
    func_80034A14(&this->actor, &this->unk_300, kREG(17) + 0xC, 4);
}

void func_80AB10C4(EnNb* this) {
    s32 pad2[2];
    Vec3s* tempPtr;
    Vec3s* tempPtr2;

    tempPtr = &this->unk_300.unk_08;
    Math_SmoothStepToS(&tempPtr->x, 0, 20, 6200, 100);
    Math_SmoothStepToS(&tempPtr->y, 0, 20, 6200, 100);
    tempPtr2 = &this->unk_300.unk_0E;
    Math_SmoothStepToS(&tempPtr2->x, 0, 20, 6200, 100);
    Math_SmoothStepToS(&tempPtr2->y, 0, 20, 6200, 100);
}

void EnNb_UpdateEyes(EnNb* this) {
    s32 pad[3];
    s16* blinkTimer = &this->blinkTimer;
    s16* eyeIdx = &this->eyeIdx;

    if (DECR(*blinkTimer) == 0) {
        *blinkTimer = Rand_S16Offset(60, 60);
    }

    *eyeIdx = *blinkTimer;
    if (*eyeIdx >= ARRAY_COUNT(sEyeTextures)) {
        *eyeIdx = 0;
    }
}

void func_80AB11EC(EnNb* this) {
    this->action = NB_ACTION_7;
    this->drawMode = NB_DRAW_NOTHING;
    this->alpha = 0;
    this->flag = 0;
    this->actor.shape.shadowAlpha = 0;
    this->alphaTimer = 0.0f;
}

void func_80AB1210(EnNb* this, PlayState* play) {
    s32 one; // required to match

    if (play->csCtx.state == CS_STATE_IDLE) {
        if (D_80AB4318) {
            if (this->actor.params == NB_TYPE_DEMO02) {
                func_80AB11EC(this);
            }

            D_80AB4318 = 0;
        }
    } else {
        one = 1;
        if (!D_80AB4318) {
            D_80AB4318 = one;
        }
    }
}

void func_80AB1284(EnNb* this, PlayState* play) {
    Actor_UpdateBgCheckInfo(play, &this->actor, 75.0f, 30.0f, 30.0f, 4);
}

s32 EnNb_UpdateSkelAnime(EnNb* this) {
    return SkelAnime_Update(&this->skelAnime);
}

CsCmdActorAction* EnNb_GetNpcCsAction(PlayState* play, s32 npcActionIdx) {
    if (play->csCtx.state != CS_STATE_IDLE) {
        return play->csCtx.npcActions[npcActionIdx];
    }
    return NULL;
}

void EnNb_SetupCsPosRot(EnNb* this, PlayState* play, s32 npcActionIdx) {
    CsCmdActorAction* csCmdNPCAction = EnNb_GetNpcCsAction(play, npcActionIdx);
    s16 newRotY;
    Actor* thisx = &this->actor;

    if (csCmdNPCAction != NULL) {
        thisx->world.pos.x = csCmdNPCAction->startPos.x;
        thisx->world.pos.y = csCmdNPCAction->startPos.y;
        thisx->world.pos.z = csCmdNPCAction->startPos.z;
        thisx->world.rot.y = thisx->shape.rot.y = csCmdNPCAction->rot.y;
    }
}

s32 func_80AB1390(EnNb* this, PlayState* play, u16 arg2, s32 npcActionIdx) {
    CsCmdActorAction* csCmdNPCAction;

    if ((play->csCtx.state != CS_STATE_IDLE) &&
        (csCmdNPCAction = play->csCtx.npcActions[npcActionIdx], csCmdNPCAction != NULL) &&
        (csCmdNPCAction->action == arg2)) {
        return true;
    }
    return false;
}

s32 func_80AB13D8(EnNb* this, PlayState* play, u16 arg2, s32 npcActionIdx) {
    CsCmdActorAction* csCmdNPCAction;

    if ((play->csCtx.state != CS_STATE_IDLE) &&
        (csCmdNPCAction = play->csCtx.npcActions[npcActionIdx], csCmdNPCAction != NULL) &&
        (csCmdNPCAction->action != arg2)) {
        return true;
    }
    return false;
}

void EnNb_SetInitialCsPosRot(EnNb* this, PlayState* play, s32 npcActionIdx) {
    CsCmdActorAction* csCmdNPCAction = EnNb_GetNpcCsAction(play, npcActionIdx);
    Actor* thisx = &this->actor;

    if (csCmdNPCAction != NULL) {
        thisx->world.pos.x = csCmdNPCAction->startPos.x;
        thisx->world.pos.y = csCmdNPCAction->startPos.y;
        thisx->world.pos.z = csCmdNPCAction->startPos.z;
        thisx->world.rot.y = thisx->shape.rot.y = csCmdNPCAction->rot.y;
    }
}

void EnNb_SetCurrentAnim(EnNb* this, AnimationHeader* animation, u8 mode, f32 transitionRate, s32 arg4) {
    f32 frameCount = Animation_GetLastFrame(animation);
    f32 playbackSpeed;
    f32 unk0;
    f32 fc;

    if (!arg4) {
        unk0 = 0.0f;
        fc = frameCount;
        playbackSpeed = 1.0f;
    } else {
        unk0 = frameCount;
        fc = 0.0f;
        playbackSpeed = -1.0f;
    }

    Animation_Change(&this->skelAnime, animation, playbackSpeed, unk0, fc, mode, transitionRate);
}

void EnNb_SetChamberAnim(EnNb* this, PlayState* play) {
    EnNb_SetCurrentAnim(this, &gNabooruStandingHandsOnHipsChamberOfSagesAnim, 0, 0, 0);
    this->actor.shape.yOffset = -10000.0f;
}

void EnNb_SpawnBlueWarp(EnNb* this, PlayState* play) {
    f32 posX = this->actor.world.pos.x;
    f32 posY = this->actor.world.pos.y;
    f32 posZ = this->actor.world.pos.z;

    Actor_SpawnAsChild(&play->actorCtx, &this->actor, play, ACTOR_DOOR_WARP1, posX, posY, posZ, 0, 0, 0,
                       WARP_SAGES);
}

void EnNb_GiveMedallion(EnNb* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    f32 posX = player->actor.world.pos.x;
    f32 posY = player->actor.world.pos.y + 50.0f;
    f32 posZ = player->actor.world.pos.z;

    Actor_SpawnAsChild(&play->actorCtx, &this->actor, play, ACTOR_DEMO_EFFECT, posX, posY, posZ, 0, 0, 0,
                       0xC);
    Item_Give(play, ITEM_MEDALLION_SPIRIT);
}

void EnNb_ComeUpImpl(EnNb* this, PlayState* play) {
    this->actor.shape.yOffset += 250.0f / 3.0f;
}

void EnNb_SetupChamberCsImpl(EnNb* this, PlayState* play) {
    s32 pad[2];
    Player* player;

    if ((gSaveContext.chamberCutsceneNum == 3) && (gSaveContext.sceneSetupIndex < 4)) {
        player = GET_PLAYER(play);
        this->action = NB_CHAMBER_UNDERGROUND;
        play->csCtx.segment = &D_80AB431C;
        gSaveContext.cutsceneTrigger = 2;
        Item_Give(play, ITEM_MEDALLION_SPIRIT);
        player->actor.world.rot.y = player->actor.shape.rot.y = this->actor.world.rot.y + 0x8000;
    }
}

void EnNb_SetupChamberWarpImpl(EnNb* this, PlayState* play) {
    CutsceneContext* csCtx = &play->csCtx;
    CsCmdActorAction* csCmdNPCAction;

    if (csCtx->state != CS_STATE_IDLE) {
        csCmdNPCAction = csCtx->npcActions[1];
        if (csCmdNPCAction != NULL && csCmdNPCAction->action == 2) {
            this->action = NB_CHAMBER_APPEAR;
            this->drawMode = NB_DRAW_DEFAULT;
            EnNb_SpawnBlueWarp(this, play);
        }
    }
}

void EnNb_SetupDefaultChamberIdle(EnNb* this) {
    if (this->actor.shape.yOffset >= 0.0f) {
        this->action = NB_CHAMBER_IDLE;
        this->actor.shape.yOffset = 0.0f;
    }
}

void EnNb_SetupArmRaise(EnNb* this, PlayState* play) {
    AnimationHeader* animation = &gNabooruRaisingArmsGivingMedallionAnim;
    CsCmdActorAction* csCmdNPCAction;

    if (play->csCtx.state != CS_STATE_IDLE) {
        csCmdNPCAction = play->csCtx.npcActions[1];
        if (csCmdNPCAction != NULL && csCmdNPCAction->action == 3) {
            Animation_Change(&this->skelAnime, animation, 1.0f, 0.0f, Animation_GetLastFrame(animation), ANIMMODE_ONCE,
                             0.0f);
            this->action = NB_CHAMBER_RAISE_ARM;
        }
    }
}

void EnNb_SetupRaisedArmTransition(EnNb* this, s32 animFinished) {
    AnimationHeader* animation = &gNabooruArmsRaisedGivingMedallionAnim;

    if (animFinished) {
        Animation_Change(&this->skelAnime, animation, 1.0f, 0.0f, Animation_GetLastFrame(animation), ANIMMODE_LOOP,
                         0.0f);
        this->action = NB_CHAMBER_RAISE_ARM_TRANSITION;
    }
}

void EnNb_SetupMedallion(EnNb* this, PlayState* play) {
    CsCmdActorAction* csCmdNPCAction;

    if (play->csCtx.state != CS_STATE_IDLE) {
        csCmdNPCAction = play->csCtx.npcActions[6];
        if (csCmdNPCAction != NULL && csCmdNPCAction->action == 2) {
            this->action = NB_GIVE_MEDALLION;
            EnNb_GiveMedallion(this, play);
        }
    }
}

// Action func is never explicitly set to this, but it runs when the memory gets zero cleared
void EnNb_SetupChamberCs(EnNb* this, PlayState* play) {
    EnNb_SetupChamberCsImpl(this, play);
}

void EnNb_SetupChamberWarp(EnNb* this, PlayState* play) {
    EnNb_SetupChamberWarpImpl(this, play);
}

void EnNb_ComeUp(EnNb* this, PlayState* play) {
    EnNb_ComeUpImpl(this, play);
    EnNb_UpdateSkelAnime(this);
    EnNb_UpdateEyes(this);
    EnNb_SetupDefaultChamberIdle(this);
}

void func_80AB193C(EnNb* this, PlayState* play) {
    func_80AB1284(this, play);
    EnNb_UpdateSkelAnime(this);
    EnNb_UpdateEyes(this);
    EnNb_SetupArmRaise(this, play);
}

void EnNb_RaiseArm(EnNb* this, PlayState* play) {
    s32 animFinished;

    func_80AB1284(this, play);
    animFinished = EnNb_UpdateSkelAnime(this);
    EnNb_UpdateEyes(this);
    EnNb_SetupRaisedArmTransition(this, animFinished);
}

void func_80AB19BC(EnNb* this, PlayState* play) {
    func_80AB1284(this, play);
    EnNb_UpdateSkelAnime(this);
    EnNb_UpdateEyes(this);
    EnNb_SetupMedallion(this, play);
}

void func_80AB19FC(EnNb* this, PlayState* play) {
    func_80AB1284(this, play);
    EnNb_UpdateSkelAnime(this);
    EnNb_UpdateEyes(this);
}

void EnNb_SetupLightArrowOrSealingCs(EnNb* this, PlayState* play) {
    EnNb_SetCurrentAnim(this, &gNabooruPuttingHandsTogetherCastingMagicAnim, 2, 0.0f, 0);
    this->action = NB_ACTION_7;
    this->actor.shape.shadowAlpha = 0;
}

void EnNb_PlaySealingSound(void) {
    func_800788CC(NA_SE_SY_WHITE_OUT_T);
}

void EnNb_InitializeDemo6K(EnNb* this, PlayState* play) {
    Actor_SpawnAsChild(&play->actorCtx, &this->actor, play, ACTOR_DEMO_6K, this->actor.world.pos.x,
                       kREG(21) + 22.0f + this->actor.world.pos.y, this->actor.world.pos.z, 0, 0, 0, 7);
}

void EnNb_SetupHide(EnNb* this, PlayState* play) {
    if (func_80AB1390(this, play, 4, 1)) {
        this->action = NB_SEAL_HIDE;
        this->drawMode = NB_DRAW_HIDE;
        this->alpha = 0;
        this->actor.shape.shadowAlpha = 0;
        this->alphaTimer = 0.0f;
        EnNb_PlaySealingSound();
    }
}

void EnNb_CheckToFade(EnNb* this, PlayState* play) {
    f32* alphaTimer = &this->alphaTimer;
    s32 alpha;

    if (func_80AB1390(this, play, 4, 1)) {
        *alphaTimer += 1.0f;
        if (*alphaTimer >= kREG(5) + 10.0f) {
            this->action = NB_ACTION_9;
            this->drawMode = NB_DRAW_DEFAULT;
            *alphaTimer = kREG(5) + 10.0f;
            this->alpha = 255;
            this->actor.shape.shadowAlpha = 0xFF;
            return;
        }
    } else {
        *alphaTimer -= 1.0f;
        if (*alphaTimer <= 0.0f) {
            this->action = NB_ACTION_7;
            this->drawMode = NB_DRAW_NOTHING;
            *alphaTimer = 0.0f;
            this->alpha = 0;
            this->actor.shape.shadowAlpha = 0;
            return;
        }
    }

    alpha = (*alphaTimer / (kREG(5) + 10.0f)) * 255.0f;
    this->alpha = alpha;
    this->actor.shape.shadowAlpha = alpha;
}

void EnNb_SetupLightOrb(EnNb* this, PlayState* play) {
    if (func_80AB13D8(this, play, 4, 1)) {
        this->action = NB_SEAL_HIDE;
        this->drawMode = NB_DRAW_HIDE;
        this->alphaTimer = kREG(5) + 10.0f;
        this->alpha = 255;

        if (this->flag == 0) {
            EnNb_InitializeDemo6K(this, play);
            this->flag = 1;
        }

        this->actor.shape.shadowAlpha = 0xFF;
    }
}

void EnNb_Hide(EnNb* this, PlayState* play) {
    EnNb_SetupHide(this, play);
    func_80AB1210(this, play);
}

void EnNb_Fade(EnNb* this, PlayState* play) {
    func_80AB1284(this, play);
    EnNb_UpdateSkelAnime(this);
    EnNb_UpdateEyes(this);
    EnNb_CheckToFade(this, play);
    func_80AB1210(this, play);
}

void EnNb_CreateLightOrb(EnNb* this, PlayState* play) {
    func_80AB1284(this, play);
    EnNb_UpdateSkelAnime(this);
    EnNb_UpdateEyes(this);
    EnNb_SetupLightOrb(this, play);
    func_80AB1210(this, play);
}

void EnNb_DrawTransparency(EnNb* this, PlayState* play) {
    s32 pad[2];
    s16 eyeSegIdx = this->eyeIdx;
    void* eyeTex = sEyeTextures[eyeSegIdx];
    SkelAnime* skelAnime = &this->skelAnime;

    OPEN_DISPS(play->state.gfxCtx);

    Gfx_SetupDL_25Xlu(play->state.gfxCtx);
    gSPSegment(POLY_XLU_DISP++, 0x08, SEGMENTED_TO_VIRTUAL(eyeTex));
    gSPSegment(POLY_XLU_DISP++, 0x09, SEGMENTED_TO_VIRTUAL(eyeTex));
    gDPSetEnvColor(POLY_XLU_DISP++, 0, 0, 0, this->alpha);
    gSPSegment(POLY_XLU_DISP++, 0x0C, &D_80116280[0]);
    POLY_XLU_DISP = SkelAnime_DrawFlex(play, skelAnime->skeleton, skelAnime->jointTable, skelAnime->dListCount,
                                       NULL, NULL, NULL, POLY_XLU_DISP);

    CLOSE_DISPS(play->state.gfxCtx);
}

void EnNb_InitKidnap(EnNb* this, PlayState* play) {
    EnNb_SetCurrentAnim(this, &gNabooruTrappedInVortexPushingGroundAnim, 0, 0.0f, 0);
    this->action = NB_KIDNAPPED;
    this->actor.shape.shadowAlpha = 0;
    gSaveContext.eventChkInf[9] |= 0x20;
}

void EnNb_PlayCrySFX(EnNb* this, PlayState* play) {
    if (play->csCtx.frames == 3) {
        func_80078914(&this->actor.projectedPos, NA_SE_VO_NB_CRY_0);
    }
}

void EnNb_PlayAgonySFX(EnNb* this, PlayState* play) {
    if (play->csCtx.frames == 420) {
        func_80078914(&this->actor.projectedPos, NA_SE_VO_NB_AGONY);
    }
}

void EnNb_SetPosInPortal(EnNb* this, PlayState* play) {
    CsCmdActorAction* csCmdNPCAction = EnNb_GetNpcCsAction(play, 1);
    Vec3f* pos = &this->actor.world.pos;
    f32 f0;
    s32 pad;
    Vec3f startPos;
    Vec3f endPos;

    if (csCmdNPCAction != NULL) {
        f0 = Environment_LerpWeightAccelDecel(csCmdNPCAction->endFrame, csCmdNPCAction->startFrame,
                                              play->csCtx.frames, 4, 4);
        startPos.x = csCmdNPCAction->startPos.x;
        startPos.y = csCmdNPCAction->startPos.y;
        startPos.z = csCmdNPCAction->startPos.z;
        endPos.x = csCmdNPCAction->endPos.x;
        endPos.y = csCmdNPCAction->endPos.y;
        endPos.z = csCmdNPCAction->endPos.z;
        pos->x = ((endPos.x - startPos.x) * f0) + startPos.x;
        pos->y = ((endPos.y - startPos.y) * f0) + startPos.y;
        pos->z = ((endPos.z - startPos.z) * f0) + startPos.z;
    }
}

void EnNb_SetupCaptureCutsceneState(EnNb* this, PlayState* play) {
    EnNb_SetupCsPosRot(this, play, 1);
    this->action = NB_KIDNAPPED;
    this->drawMode = NB_DRAW_NOTHING;
    this->actor.shape.shadowAlpha = 0;
}

void EnNb_SetRaisedArmCaptureAnim(EnNb* this, s32 animFinished) {
    AnimationHeader* animation = &gNabooruSuckedByVortexAnim;

    if (animFinished) {
        Animation_Change(&this->skelAnime, animation, 1.0f, 0.0f, Animation_GetLastFrame(animation), ANIMMODE_LOOP,
                         0.0f);
    }
}

void EnNb_SetupLookAroundInKidnap(EnNb* this) {
    AnimationHeader* animation = &gNabooruTrappedInVortexPushingGroundAnim;

    Animation_Change(&this->skelAnime, animation, 1.0f, 0.0f, Animation_GetLastFrame(animation), ANIMMODE_LOOP, -8.0f);
    this->action = NB_KIDNAPPED_LOOK_AROUND;
    this->drawMode = NB_DRAW_DEFAULT;
}

void EnNb_SetupKidnap(EnNb* this) {
    AnimationHeader* animation = &gNabooruTrappedInVortexRaisingArmAnim;

    Animation_Change(&this->skelAnime, animation, 1.0f, 0.0f, Animation_GetLastFrame(animation), ANIMMODE_ONCE, -8.0f);
    this->action = NB_PORTAL_FALLTHROUGH;
    this->drawMode = NB_DRAW_DEFAULT;
}

void EnNb_CheckKidnapCsMode(EnNb* this, PlayState* play) {
    CsCmdActorAction* csCmdNPCAction = EnNb_GetNpcCsAction(play, 1);
    s32 action;
    s32 previousCsAction;

    if (csCmdNPCAction != NULL) {
        action = csCmdNPCAction->action;
        previousCsAction = this->previousCsAction;
        if (action != previousCsAction) {
            switch (action) {
                case 1:
                    EnNb_SetupCaptureCutsceneState(this, play);
                    break;
                case 7:
                    EnNb_SetupLookAroundInKidnap(this);
                    break;
                case 8:
                    EnNb_SetupKidnap(this);
                    break;
                case 9:
                    Actor_Kill(&this->actor);
                    break;
                default:
                    // "Operation Doesn't Exist!!!!!!!!"
                    osSyncPrintf("En_Nb_Kidnap_Check_DemoMode:そんな動作は無い!!!!!!!!\n");
                    break;
            }
            this->previousCsAction = action;
        }
    }
}

void func_80AB23A8(EnNb* this, PlayState* play) {
    EnNb_PlayCrySFX(this, play);
    EnNb_CheckKidnapCsMode(this, play);
}

void EnNb_MovingInPortal(EnNb* this, PlayState* play) {
    EnNb_PlayCrySFX(this, play);
    EnNb_PlayAgonySFX(this, play);
    EnNb_UpdateEyes(this);
    EnNb_UpdateSkelAnime(this);
    EnNb_CheckKidnapCsMode(this, play);
}

void EnNb_SuckedInByPortal(EnNb* this, PlayState* play) {
    s32 animFinished;

    EnNb_UpdateEyes(this);
    animFinished = EnNb_UpdateSkelAnime(this);
    EnNb_SetRaisedArmCaptureAnim(this, animFinished);
    EnNb_SetPosInPortal(this, play);
    EnNb_CheckKidnapCsMode(this, play);
}

void EnNb_SetupConfrontation(EnNb* this, PlayState* play) {
    AnimationHeader* animation = &gNabooruCollapseFromStandingToKneelingTransitionAnim;

    EnNb_SetCurrentAnim(this, animation, 0, 0.0f, 0);
    this->action = NB_IN_CONFRONTATION;
    this->actor.shape.shadowAlpha = 0;
}

void EnNb_PlayKnuckleDefeatSFX(EnNb* this, PlayState* play) {
    s32 pad[2];

    if (play->csCtx.frames == 548) {
        func_80078914(&this->actor.projectedPos, NA_SE_VO_NB_CRY_0);
        func_80078914(&this->actor.projectedPos, NA_SE_EN_FANTOM_HIT_THUNDER);
    }
}

void EnNb_PlayKneelingOnGroundSFX(EnNb* this) {
    s32 pad[2];

    if ((this->skelAnime.mode == 2) &&
        (Animation_OnFrame(&this->skelAnime, 18.0f) || Animation_OnFrame(&this->skelAnime, 25.0f))) {
        func_80078914(&this->actor.projectedPos, NA_SE_EV_HUMAN_BOUND);
    }
}

void EnNb_PlayLookRightSFX(EnNb* this) {
    s32 pad[2];

    if ((this->skelAnime.mode == 2) && Animation_OnFrame(&this->skelAnime, 9.0f)) {
        func_80078914(&this->actor.projectedPos, NA_SE_PL_WALK_CONCRETE);
    }
}

void EnNb_PlayLookLeftSFX(EnNb* this) {
    s32 pad[2];

    if (Animation_OnFrame(&this->skelAnime, 9.0f) || Animation_OnFrame(&this->skelAnime, 13.0f)) {
        func_80078914(&this->actor.projectedPos, NA_SE_PL_WALK_CONCRETE);
    }
}

void EnNb_InitDemo6KInConfrontation(EnNb* this, PlayState* play) {
    Actor_Spawn(&play->actorCtx, play, ACTOR_DEMO_6K, this->actor.world.pos.x,
                kREG(21) + 22.0f + this->actor.world.pos.y, this->actor.world.pos.z, 0, 0, 0, 0xB);
}

void func_80AB2688(EnNb* this, PlayState* play) {
    this->skelAnime.moveFlags |= 1;
    AnimationContext_SetMoveActor(play, &this->actor, &this->skelAnime, 1.0f);
}

void func_80AB26C8(EnNb* this) {
    this->action = NB_IN_CONFRONTATION;
    this->drawMode = NB_DRAW_NOTHING;
    this->actor.shape.shadowAlpha = 0;
}

void func_80AB26DC(EnNb* this, PlayState* play) {
    s32 pad;
    AnimationHeader* animation = &gNabooruCollapseFromStandingToKneelingTransitionAnim;
    f32 lastFrame = Animation_GetLastFrame(animation);

    EnNb_SetupCsPosRot(this, play, 1);
    Animation_Change(&this->skelAnime, animation, 1.0f, 0.0f, lastFrame, ANIMMODE_ONCE, 0.0f);
    this->action = NB_ACTION_14;
    this->drawMode = NB_DRAW_KNEEL;
    this->actor.shape.shadowAlpha = 0xFF;
}

void EnNb_SetupKneel(EnNb* this) {
    AnimationHeader* animation = &gNabooruCollapseFromStandingToKneelingTransitionAnim;
    f32 lastFrame = Animation_GetLastFrame(animation);

    Animation_Change(&this->skelAnime, animation, 1.0f, 0.0f, lastFrame, ANIMMODE_ONCE, 0.0f);
    this->action = NB_KNEEL;
    this->drawMode = NB_DRAW_KNEEL;
    this->actor.shape.shadowAlpha = 0xFF;
}

void EnNb_CheckIfKneeling(EnNb* this, s32 animFinished) {
    AnimationHeader* animation = &gNabooruOnAllFoursAnim;

    if (animFinished) {
        Animation_Change(&this->skelAnime, animation, 1.0f, 0.0f, Animation_GetLastFrame(animation), ANIMMODE_LOOP,
                         0.0f);
        this->drawMode = NB_DRAW_KNEEL;
    }
}

void EnNb_SetupLookRight(EnNb* this) {
    AnimationHeader* animation = &gNabooruOnAllFoursToOnOneKneeLookingRightTransitionAnim;
    f32 lastFrame = Animation_GetLastFrame(animation);

    Animation_Change(&this->skelAnime, animation, 1.0f, 0.0f, lastFrame, ANIMMODE_ONCE, -8.0f);
    this->action = NB_LOOK_RIGHT;
    this->drawMode = NB_DRAW_DEFAULT;
    this->actor.shape.shadowAlpha = 0xFF;
}

void EnNb_CheckIfLookingRight(EnNb* this, s32 animFinished) {
    AnimationHeader* animation = &gNabooruOnOneKneeLookingRightAnim;

    if (animFinished) {
        Animation_Change(&this->skelAnime, animation, 1.0f, 0.0f, Animation_GetLastFrame(animation), ANIMMODE_LOOP,
                         0.0f);
        this->drawMode = NB_DRAW_LOOK_DIRECTION;
    }
}

void EnNb_SetupLookLeft(EnNb* this) {
    AnimationHeader* animation = &gNabooruOnOneKneeTurningHeadRightToLeftTransitionAnim;
    f32 lastFrame = Animation_GetLastFrame(animation);

    Animation_Change(&this->skelAnime, animation, 1.0f, 0.0f, lastFrame, ANIMMODE_ONCE, -8.0f);
    this->action = NB_LOOK_LEFT;
    this->drawMode = NB_DRAW_LOOK_DIRECTION;
    this->actor.shape.shadowAlpha = 0xFF;
}

void EnNb_CheckIfLookLeft(EnNb* this, s32 animFinished) {
    AnimationHeader* animation = &gNabooruOnOneKneeLookingLeftAnim;

    if (animFinished) {
        Animation_Change(&this->skelAnime, animation, 1.0f, 0.0f, Animation_GetLastFrame(animation), ANIMMODE_LOOP,
                         0.0f);
    }
}

void EnNb_SetupDemo6KInConfrontation(EnNb* this, PlayState* play, s32 animFinished) {
    if (!this->flag && animFinished) {
        EnNb_InitDemo6KInConfrontation(this, play);
        this->flag = 1;
    }
}

void EnNb_SetupRun(EnNb* this) {
    AnimationHeader* animation = &gNabooruKneeingToRunningToHitAnim;
    f32 lastFrame = Animation_GetLastFrame(animation);

    Animation_Change(&this->skelAnime, animation, 1.0f, 0.0f, lastFrame, ANIMMODE_ONCE, -8.0f);
    this->action = NB_RUN;
    this->drawMode = NB_DRAW_LOOK_DIRECTION;
    this->actor.shape.shadowAlpha = 0xFF;
}

void EnNb_SetupConfrontationDestroy(EnNb* this) {
    this->action = NB_CONFRONTATION_DESTROYED;
    this->drawMode = NB_DRAW_NOTHING;
    this->actor.shape.shadowAlpha = 0;
}

void EnNb_CheckConfrontationCsMode(EnNb* this, PlayState* play) {
    CsCmdActorAction* csCmdNPCAction;
    s32 csAction;
    s32 previousCsAction;

    csCmdNPCAction = EnNb_GetNpcCsAction(play, 1);
    if (csCmdNPCAction != NULL) {
        csAction = csCmdNPCAction->action;
        previousCsAction = this->previousCsAction;

        if (csAction != previousCsAction) {
            switch (csAction) {
                case 1:
                    func_80AB26C8(this);
                    break;
                case 10:
                    func_80AB26DC(this, play);
                    break;
                case 11:
                    EnNb_SetupKneel(this);
                    break;
                case 12:
                    EnNb_SetupLookRight(this);
                    break;
                case 13:
                    EnNb_SetupLookLeft(this);
                    break;
                case 14:
                    EnNb_SetupRun(this);
                    break;
                case 9:
                    EnNb_SetupConfrontationDestroy(this);
                    break;
                default:
                    // "En_Nb_Confrontion_Check_DemoMode: Operation doesn't exist!!!!!!!!"
                    osSyncPrintf("En_Nb_Confrontion_Check_DemoMode:そんな動作は無い!!!!!!!!\n");
                    break;
            }
            this->previousCsAction = csAction;
        }
    }
}

void EnNb_CheckConfrontationCsModeWrapper(EnNb* this, PlayState* play) {
    EnNb_CheckConfrontationCsMode(this, play);
}

void func_80AB2C18(EnNb* this, PlayState* play) {
    EnNb_UpdateEyes(this);
    func_80AB2688(this, play);
    func_80AB1284(this, play);
    EnNb_CheckConfrontationCsMode(this, play);
}

void EnNb_Kneel(EnNb* this, PlayState* play) {
    s32 animFinished;

    EnNb_UpdateEyes(this);
    animFinished = EnNb_UpdateSkelAnime(this);
    EnNb_CheckIfKneeling(this, animFinished);
    EnNb_PlayKneelingOnGroundSFX(this);
    func_80AB2688(this, play);
    func_80AB1284(this, play);
    EnNb_CheckConfrontationCsMode(this, play);
}

void EnNb_LookRight(EnNb* this, PlayState* play) {
    s32 animFinished;

    EnNb_UpdateEyes(this);
    animFinished = EnNb_UpdateSkelAnime(this);
    EnNb_CheckIfLookingRight(this, animFinished);
    EnNb_PlayLookRightSFX(this);
    func_80AB2688(this, play);
    func_80AB1284(this, play);
    EnNb_CheckConfrontationCsMode(this, play);
}

void EnNb_LookLeft(EnNb* this, PlayState* play) {
    s32 animFinished;

    EnNb_UpdateEyes(this);
    animFinished = EnNb_UpdateSkelAnime(this);
    EnNb_CheckIfLookLeft(this, animFinished);
    func_80AB2688(this, play);
    func_80AB1284(this, play);
    EnNb_CheckConfrontationCsMode(this, play);
}

void EnNb_Run(EnNb* this, PlayState* play) {
    s32 animFinished;

    EnNb_PlayKnuckleDefeatSFX(this, play);
    EnNb_UpdateEyes(this);
    animFinished = EnNb_UpdateSkelAnime(this);
    EnNb_PlayLookLeftSFX(this);
    func_80AB2688(this, play);
    func_80AB1284(this, play);
    EnNb_SetupDemo6KInConfrontation(this, play, animFinished);
    EnNb_CheckConfrontationCsMode(this, play);
}

void EnNb_ConfrontationDestroy(EnNb* this, PlayState* play) {
    this->timer++;

    if (this->timer > 60.0f) {
        Actor_Kill(&this->actor);
    }
}

void func_80AB2E70(EnNb* this, PlayState* play) {
    s32 pad;
    SkelAnime* skelAnime = &this->skelAnime;

    OPEN_DISPS(play->state.gfxCtx);

    Gfx_SetupDL_25Opa(play->state.gfxCtx);
    gSPSegment(POLY_OPA_DISP++, 0x08, SEGMENTED_TO_VIRTUAL(gNabooruEyeWideTex));
    gSPSegment(POLY_OPA_DISP++, 0x09, SEGMENTED_TO_VIRTUAL(gNabooruEyeWideTex));
    gDPSetEnvColor(POLY_OPA_DISP++, 0, 0, 0, 255);
    gSPSegment(POLY_OPA_DISP++, 0x0C, &D_80116280[2]);
    SkelAnime_DrawFlexOpa(play, skelAnime->skeleton, skelAnime->jointTable, skelAnime->dListCount, NULL, NULL,
                          &this->actor);

    CLOSE_DISPS(play->state.gfxCtx);
}

s32 func_80AB2FC0(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, void* thisx) {
    EnNb* this = (EnNb*)thisx;

    if (limbIndex == NB_LIMB_HEAD) {
        *dList = gNabooruHeadMouthOpenDL;
    }

    return 0;
}

void func_80AB2FE4(EnNb* this, PlayState* play) {
    s32 pad;
    s16 eyeIdx = this->eyeIdx;
    SkelAnime* skelAnime = &this->skelAnime;
    void* eyeTexture = sEyeTextures[eyeIdx];
    s32 pad1;

    OPEN_DISPS(play->state.gfxCtx);

    Gfx_SetupDL_25Opa(play->state.gfxCtx);
    gSPSegment(POLY_OPA_DISP++, 0x08, SEGMENTED_TO_VIRTUAL(eyeTexture));
    gSPSegment(POLY_OPA_DISP++, 0x09, SEGMENTED_TO_VIRTUAL(eyeTexture));
    gDPSetEnvColor(POLY_OPA_DISP++, 0, 0, 0, 255);
    gSPSegment(POLY_OPA_DISP++, 0x0C, &D_80116280[2]);
    SkelAnime_DrawFlexOpa(play, skelAnime->skeleton, skelAnime->jointTable, skelAnime->dListCount, func_80AB2FC0,
                          NULL, &this->actor);

    CLOSE_DISPS(play->state.gfxCtx);
}

void EnNb_SetupCreditsSpawn(EnNb* this, PlayState* play) {
    EnNb_SetCurrentAnim(this, &gNabooruSittingCrossLeggedAnim, 0, 0.0f, 0);
    this->action = NB_CREDITS_INIT;
    this->drawMode = NB_DRAW_NOTHING;
    this->actor.shape.shadowAlpha = 0;
}

void EnNb_SetAlphaInCredits(EnNb* this) {
    f32* alphaTimer = &this->alphaTimer;
    s32 alpha;

    this->alphaTimer++;

    if ((kREG(17) + 10.0f) <= this->alphaTimer) {
        this->alpha = 255;
        this->actor.shape.shadowAlpha = 255;
    } else {
        alpha = (*alphaTimer / (kREG(17) + 10.0f)) * 255.0f;
        this->alpha = alpha;
        this->actor.shape.shadowAlpha = alpha;
    }
}

void EnNb_SetupCreditsFadeIn(EnNb* this, PlayState* play) {
    EnNb_SetInitialCsPosRot(this, play, 1);
    this->action = NB_CREDITS_FADEIN;
    this->drawMode = NB_DRAW_HIDE;
}

void EnNb_SetupCreditsSit(EnNb* this) {
    if (this->alphaTimer >= kREG(17) + 10.0f) {
        this->action = NB_CREDITS_SIT;
        this->drawMode = NB_DRAW_DEFAULT;
    }
}

void EnNb_SetupCreditsHeadTurn(EnNb* this) {
    EnNb_SetCurrentAnim(this, &gNabooruSittingCrossLeggedTurningToLookUpRightTransitionAnim, 2, -8.0f, 0);
    this->action = NB_CREDITS_HEAD_TURN;
}

void EnNb_CheckIfLookingUp(EnNb* this, s32 animFinished) {
    if (animFinished) {
        EnNb_SetCurrentAnim(this, &gNabooruSittingCrossLeggedLookingUpRightAnim, 0, 0.0f, 0);
    }
}

void EnNb_CheckCreditsCsModeImpl(EnNb* this, PlayState* play) {
    CsCmdActorAction* csCmdNPCAction = EnNb_GetNpcCsAction(play, 1);
    s32 action;
    s32 previousCsAction;

    if (csCmdNPCAction != NULL) {
        action = csCmdNPCAction->action;
        previousCsAction = this->previousCsAction;
        if (action != previousCsAction) {
            switch (action) {
                case 15:
                    EnNb_SetupCreditsFadeIn(this, play);
                    break;
                case 16:
                    EnNb_SetupCreditsHeadTurn(this);
                    break;
                default:
                    // "En_Nb_inEnding_Check_DemoMode: Operation doesn't exist!!!!!!!!"
                    osSyncPrintf("En_Nb_inEnding_Check_DemoMode:そんな動作は無い!!!!!!!!\n");
                    break;
            }
            this->previousCsAction = action;
        }
    }
}

void EnNb_CheckCreditsCsMode(EnNb* this, PlayState* play) {
    EnNb_CheckCreditsCsModeImpl(this, play);
}

void EnNb_CreditsFade(EnNb* this, PlayState* play) {
    func_80AB1284(this, play);
    EnNb_UpdateSkelAnime(this);
    EnNb_UpdateEyes(this);
    EnNb_SetAlphaInCredits(this);
    EnNb_SetupCreditsSit(this);
}

void func_80AB3428(EnNb* this, PlayState* play) {
    func_80AB1284(this, play);
    EnNb_UpdateSkelAnime(this);
    EnNb_UpdateEyes(this);
    EnNb_CheckCreditsCsModeImpl(this, play);
}

void EnNb_LookUp(EnNb* this, PlayState* play) {
    s32 animFinished;

    func_80AB1284(this, play);
    animFinished = EnNb_UpdateSkelAnime(this);
    EnNb_UpdateEyes(this);
    EnNb_CheckIfLookingUp(this, animFinished);
}

void EnNb_CrawlspaceSpawnCheck(EnNb* this, PlayState* play) {
    if (!gSaveContext.n64ddFlag && !(gSaveContext.eventChkInf[9] & 0x20) && LINK_IS_CHILD) {
        EnNb_UpdatePath(this, play);

        // looking into crawlspace
        if (!(gSaveContext.eventChkInf[9] & 0x10)) {
            EnNb_SetCurrentAnim(this, &gNabooruKneeingAtCrawlspaceAnim, 0, 0.0f, 0);
            this->action = NB_CROUCH_CRAWLSPACE;
            this->drawMode = NB_DRAW_DEFAULT;
        } else {
            EnNb_SetCurrentAnim(this, &gNabooruStandingHandsOnHipsAnim, 0, 0.0f, 0);
            this->headTurnFlag = 1;
            this->actor.flags |= ACTOR_FLAG_0 | ACTOR_FLAG_3;
            this->actor.world.pos = this->finalPos;
            this->action = NB_IDLE_AFTER_TALK;
            this->drawMode = NB_DRAW_DEFAULT;
        }
    } else {
        Actor_Kill(&this->actor);
    }
}

void func_80AB359C(EnNb* this) {
    PosRot* world = &this->actor.world;
    Vec3f* initialPos = &this->initialPos;
    Vec3f* finalPos = &this->finalPos;
    f32 f0;
    u16 temp_t1;
    s16 temp_2;

    this->movementTimer++;
    temp_2 = kREG(17);
    temp_t1 = temp_2;
    temp_t1 += 25;

    if (temp_t1 >= this->movementTimer) {
        f0 = Environment_LerpWeightAccelDecel(temp_t1, 0, this->movementTimer, 3, 3);
        world->pos.x = initialPos->x + (f0 * (finalPos->x - initialPos->x));
        world->pos.y = initialPos->y + (f0 * (finalPos->y - initialPos->y));
        world->pos.z = initialPos->z + (f0 * (finalPos->z - initialPos->z));
    }
}

void EnNb_SetNoticeSFX(EnNb* this) {
    func_80078914(&this->actor.projectedPos, NA_SE_VO_NB_NOTICE);
}

s32 EnNb_GetNoticedStatus(EnNb* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    f32 playerX = player->actor.world.pos.x;
    f32 playerZ = player->actor.world.pos.z;
    f32 thisX = this->actor.world.pos.x;
    f32 thisZ = this->actor.world.pos.z;

    if (SQ(playerX - thisX) + SQ(playerZ - thisZ) < SQ(80.0f)) {
        return true;
    } else {
        return false;
    }
}

void func_80AB36DC(EnNb* this, PlayState* play) {
    u16 moveTime = this->movementTimer;

    if ((((u16)((u16)(kREG(17) + 25) - 4))) > moveTime) {
        s16 invScale = 4 - moveTime;

        if (invScale > 0) {
            Math_SmoothStepToS(&this->actor.shape.rot.y, this->pathYaw, invScale, 6200, 100);
        }
    } else {
        s16 invScale = (u16)(kREG(17) + 25) - moveTime;

        if (invScale > 0) {
            Math_SmoothStepToS(&this->actor.shape.rot.y, this->actor.home.rot.y, invScale, 6200, 100);
        }
    }
}

void EnNb_CheckNoticed(EnNb* this, PlayState* play) {
    if (EnNb_GetNoticedStatus(this, play)) {
        EnNb_SetCurrentAnim(this, &gNabooruStandingToWalkingTransitionAnim, 2, -8.0f, 0);
        this->action = NB_NOTICE_PLAYER;
        EnNb_SetNoticeSFX(this);
    }
}

void EnNb_SetupIdleCrawlspace(EnNb* this, s32 animFinished) {
    if (animFinished) {
        EnNb_SetCurrentAnim(this, &gNabooruStandingHandsOnHipsAnim, 0, -8.0f, 0);
        this->headTurnFlag = 1;
        this->actor.flags |= ACTOR_FLAG_0 | ACTOR_FLAG_3;
        this->action = NB_IDLE_CRAWLSPACE;
    }
}

void func_80AB3838(EnNb* this, PlayState* play) {
    if (Actor_ProcessTalkRequest(&this->actor, play)) {
        this->action = NB_IN_DIALOG;
    } else {
        this->actor.flags |= ACTOR_FLAG_0 | ACTOR_FLAG_3;

        if (!(gSaveContext.infTable[22] & 0x1000)) {
            this->actor.textId = 0x601D;
        } else {
            this->actor.textId = 0x6024;
        }

        func_8002F2F4(&this->actor, play);
    }
}

void EnNb_SetupPathMovement(EnNb* this, PlayState* play) {
    EnNb_SetCurrentAnim(this, &gNabooruStandingToWalkingTransitionAnim, 2, -8.0f, 0);
    gSaveContext.eventChkInf[9] |= 0x10;
    this->action = NB_IN_PATH;
    this->actor.flags &= ~(ACTOR_FLAG_0 | ACTOR_FLAG_3);
}

void EnNb_SetTextIdAsChild(EnNb* this, PlayState* play) {
    s32 pad;
    u8 choiceIndex;
    s32 pad1;
    u16 textId;

    textId = this->actor.textId;

    if (Message_GetState(&play->msgCtx) == TEXT_STATE_CLOSING) {
        if (textId == 0x6025) {
            EnNb_SetupPathMovement(this, play);
        } else {
            if (textId == 0x6027) {
                gSaveContext.infTable[22] |= 0x1000;
            }
            this->action = NB_IDLE_CRAWLSPACE;
        }
        this->actor.flags &= ~(ACTOR_FLAG_0 | ACTOR_FLAG_3);
    } else if ((Message_GetState(&play->msgCtx) == TEXT_STATE_CHOICE) && Message_ShouldAdvance(play)) {
        choiceIndex = play->msgCtx.choiceIndex;

        if (textId == 0x601D) {
            switch (choiceIndex) {
                case 0:
                    this->actor.textId = 0x601E;
                    break;
                case 1:
                    this->actor.textId = 0x601F;
                    break;
                default:
                    this->actor.textId = 0x6020;
            }
        } else if (textId == 0x6020) {
            switch (choiceIndex) {
                case 0:
                    this->actor.textId = 0x6021;
                    break;
                default:
                    this->actor.textId = 0x6022;
                    break;
            }
        } else {
            switch (choiceIndex) {
                case 0:
                    this->actor.textId = 0x6025;
                    break;
                default:
                    this->actor.textId = 0x6027;
                    break;
            }
        }

        Message_ContinueTextbox(play, this->actor.textId);
    }
}

void func_80AB3A7C(EnNb* this, PlayState* play, s32 animFinished) {
    u16 movementTimer = this->movementTimer;

    if ((u16)(kREG(17) + 25) > movementTimer) {
        if (animFinished) {
            EnNb_SetCurrentAnim(this, &gNabooruWalkingAnim, 0, 0.0f, 0);
        }
    } else {
        EnNb_SetCurrentAnim(this, &gNabooruStandingHandsOnHipsAnim, 0, -8.0f, 0);
        this->action = NB_IDLE_AFTER_TALK;
    }
}

void func_80AB3B04(EnNb* this, PlayState* play) {
    if (Actor_ProcessTalkRequest(&this->actor, play)) {
        this->action = NB_ACTION_30;
    } else {
        this->actor.flags |= ACTOR_FLAG_0 | ACTOR_FLAG_3;
        this->actor.textId = Text_GetFaceReaction(play, 0x23);

        if ((this->actor.textId) == 0) {
            this->actor.textId = 0x6026;
        }

        func_8002F2F4(&this->actor, play);
    }
}

void func_80AB3B7C(EnNb* this, PlayState* play) {
    if (Message_GetState(&play->msgCtx) == TEXT_STATE_CLOSING) {
        this->action = NB_IDLE_AFTER_TALK;
        this->actor.flags &= ~(ACTOR_FLAG_0 | ACTOR_FLAG_3);
    }
}

void EnNb_WaitForNotice(EnNb* this, PlayState* play) {
    func_80AB1284(this, play);
    EnNb_UpdateCollider(this, play);
    EnNb_UpdateSkelAnime(this);
    EnNb_UpdateEyes(this);
    EnNb_CheckNoticed(this, play);
}

void EnNb_StandUpAfterNotice(EnNb* this, PlayState* play) {
    s32 animFinished;

    func_80AB1284(this, play);
    EnNb_UpdateCollider(this, play);
    animFinished = EnNb_UpdateSkelAnime(this);
    EnNb_UpdateEyes(this);
    EnNb_SetupIdleCrawlspace(this, animFinished);
}

void EnNb_BlockCrawlspace(EnNb* this, PlayState* play) {
    func_80AB1284(this, play);
    EnNb_UpdateCollider(this, play);
    func_80AB0FBC(this, play);
    EnNb_UpdateSkelAnime(this);
    EnNb_UpdateEyes(this);
    func_80AB3838(this, play);
}

void EnNb_InitCrawlspaceDialogue(EnNb* this, PlayState* play) {
    func_80AB1284(this, play);
    EnNb_UpdateCollider(this, play);
    func_80AB0FBC(this, play);
    EnNb_UpdateSkelAnime(this);
    EnNb_UpdateEyes(this);
    EnNb_SetTextIdAsChild(this, play);
}

void EnNb_FollowPath(EnNb* this, PlayState* play) {
    s32 animFinished;

    func_80AB359C(this);
    func_80AB1284(this, play);
    EnNb_UpdateCollider(this, play);
    func_80AB36DC(this, play);
    func_80AB10C4(this);
    animFinished = EnNb_UpdateSkelAnime(this);
    EnNb_UpdateEyes(this);
    func_80AB3A7C(this, play, animFinished);
}

void func_80AB3DB0(EnNb* this, PlayState* play) {
    func_80AB1284(this, play);
    EnNb_UpdateCollider(this, play);
    func_80AB0FBC(this, play);
    EnNb_UpdateSkelAnime(this);
    EnNb_UpdateEyes(this);
    func_80AB3B04(this, play);
}

void func_80AB3E10(EnNb* this, PlayState* play) {
    func_80AB1284(this, play);
    EnNb_UpdateCollider(this, play);
    func_80AB1040(this, play);
    EnNb_UpdateSkelAnime(this);
    EnNb_UpdateEyes(this);
    func_80AB3B7C(this, play);
}

static EnNbActionFunc sActionFuncs[] = {
    EnNb_SetupChamberCs,
    EnNb_SetupChamberWarp,
    EnNb_ComeUp,
    func_80AB193C,
    EnNb_RaiseArm,
    func_80AB19BC,
    func_80AB19FC,
    EnNb_Hide,
    EnNb_Fade,
    EnNb_CreateLightOrb,
    func_80AB23A8,
    EnNb_MovingInPortal,
    EnNb_SuckedInByPortal,
    EnNb_CheckConfrontationCsModeWrapper,
    func_80AB2C18,
    EnNb_Kneel,
    EnNb_LookRight,
    EnNb_LookLeft,
    EnNb_Run,
    EnNb_ConfrontationDestroy,
    EnNb_CheckCreditsCsMode,
    EnNb_CreditsFade,
    func_80AB3428,
    EnNb_LookUp,
    EnNb_WaitForNotice,
    EnNb_StandUpAfterNotice,
    EnNb_BlockCrawlspace,
    EnNb_InitCrawlspaceDialogue,
    EnNb_FollowPath,
    func_80AB3DB0,
    func_80AB3E10,
};

void EnNb_Update(Actor* thisx, PlayState* play) {
    EnNb* this = (EnNb*)thisx;

    if (this->action < 0 || this->action > 30 || sActionFuncs[this->action] == NULL) {
        // "Main mode is wrong!!!!!!!!!!!!!!!!!!!!!!!!!"
        osSyncPrintf(VT_FGCOL(RED) "メインモードがおかしい!!!!!!!!!!!!!!!!!!!!!!!!!\n" VT_RST);
        return;
    }

    sActionFuncs[this->action](this, play);
}

void EnNb_Init(Actor* thisx, PlayState* play) {
    s32 pad;
    EnNb* this = (EnNb*)thisx;

    ActorShape_Init(&this->actor.shape, 0.0f, ActorShadow_DrawCircle, 30.0f);
    EnNb_SetupCollider(thisx, play);
    SkelAnime_InitFlex(play, &this->skelAnime, &gNabooruSkel, NULL, this->jointTable, this->morphTable,
                       NB_LIMB_MAX);

    switch (EnNb_GetType(this)) {
        case NB_TYPE_DEMO02:
            EnNb_SetupLightArrowOrSealingCs(this, play);
            break;
        case NB_TYPE_KIDNAPPED:
            EnNb_InitKidnap(this, play);
            break;
        case NB_TYPE_KNUCKLE:
            EnNb_SetupConfrontation(this, play);
            break;
        case NB_TYPE_CREDITS:
            EnNb_SetupCreditsSpawn(this, play);
            break;
        case NB_TYPE_CRAWLSPACE:
            EnNb_CrawlspaceSpawnCheck(this, play);
            break;
        default: // giving medallion
            EnNb_SetChamberAnim(this, play);
            break;
    }
}

s32 EnNb_OverrideLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, void* thisx) {
    EnNb* this = (EnNb*)thisx;
    struct_80034A14_arg1* unk_300 = &this->unk_300;
    s32 ret = false;

    if (this->headTurnFlag != 0) {
        if (limbIndex == NB_LIMB_TORSO) {
            rot->x += unk_300->unk_0E.y;
            rot->y -= unk_300->unk_0E.x;
            ret = false;
        } else if (limbIndex == NB_LIMB_HEAD) {
            rot->x += unk_300->unk_08.y;
            rot->z += unk_300->unk_08.x;
            ret = false;
        }
    }

    return ret;
}

void EnNb_PostLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot, void* thisx) {
    EnNb* this = (EnNb*)thisx;

    if (limbIndex == NB_LIMB_HEAD) {
        Vec3f vec1 = { 0.0f, 10.0f, 0.0f };
        Vec3f vec2;

        Matrix_MultVec3f(&vec1, &vec2);
        this->actor.focus.pos.x = vec2.x;
        this->actor.focus.pos.y = vec2.y;
        this->actor.focus.pos.z = vec2.z;
        this->actor.focus.rot.x = this->actor.world.rot.x;
        this->actor.focus.rot.y = this->actor.world.rot.y;
        this->actor.focus.rot.z = this->actor.world.rot.z;
    }
}

void EnNb_DrawNothing(EnNb* this, PlayState* play) {
}

void EnNb_DrawDefault(EnNb* this, PlayState* play) {
    s32 pad;
    s16 eyeIdx = this->eyeIdx;
    SkelAnime* skelAnime = &this->skelAnime;
    void* eyeTexture = sEyeTextures[eyeIdx];
    s32 pad1;

    OPEN_DISPS(play->state.gfxCtx);

    Gfx_SetupDL_25Opa(play->state.gfxCtx);
    gSPSegment(POLY_OPA_DISP++, 0x08, SEGMENTED_TO_VIRTUAL(eyeTexture));
    gSPSegment(POLY_OPA_DISP++, 0x09, SEGMENTED_TO_VIRTUAL(eyeTexture));
    gDPSetEnvColor(POLY_OPA_DISP++, 0, 0, 0, 255);
    gSPSegment(POLY_OPA_DISP++, 0x0C, &D_80116280[2]);
    SkelAnime_DrawFlexOpa(play, skelAnime->skeleton, skelAnime->jointTable, skelAnime->dListCount,
                          EnNb_OverrideLimbDraw, EnNb_PostLimbDraw, &this->actor);

    CLOSE_DISPS(play->state.gfxCtx);
}

static EnNbDrawFunc sDrawFuncs[] = {
    EnNb_DrawNothing, EnNb_DrawDefault, EnNb_DrawTransparency, func_80AB2E70, func_80AB2FE4,
};

void EnNb_Draw(Actor* thisx, PlayState* play) {
    EnNb* this = (EnNb*)thisx;

    if (this->drawMode < 0 || this->drawMode >= 5 || sDrawFuncs[this->drawMode] == NULL) {
        // "Draw mode is wrong!!!!!!!!!!!!!!!!!!!!!!!!!"
        osSyncPrintf(VT_FGCOL(RED) "描画モードがおかしい!!!!!!!!!!!!!!!!!!!!!!!!!\n" VT_RST);
        return;
    }

    sDrawFuncs[this->drawMode](this, play);
}

const ActorInit En_Nb_InitVars = {
    ACTOR_EN_NB,
    ACTORCAT_NPC,
    FLAGS,
    OBJECT_NB,
    sizeof(EnNb),
    (ActorFunc)EnNb_Init,
    (ActorFunc)EnNb_Destroy,
    (ActorFunc)EnNb_Update,
    (ActorFunc)EnNb_Draw,
    NULL,
};
