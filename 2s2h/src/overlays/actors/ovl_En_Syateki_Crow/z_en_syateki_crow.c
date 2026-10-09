/*
 * File: z_en_syateki_crow.c
 * Overlay: ovl_En_Syateki_Crow
 * Description: Shooting Gallery Guay
 */

#include "z_en_syateki_crow.h"
#include "overlays/actors/ovl_En_Syateki_Man/z_en_syateki_man.h"

#define FLAGS (ACTOR_FLAG_UPDATE_CULLING_DISABLED | ACTOR_FLAG_DRAW_CULLING_DISABLED | ACTOR_FLAG_LOCK_ON_DISABLED)

void EnSyatekiCrow_Init(Actor* thisx, PlayState* play2);
void EnSyatekiCrow_Destroy(Actor* thisx, PlayState* play);
void EnSyatekiCrow_Update(Actor* thisx, PlayState* play);
void EnSyatekiCrow_Draw(Actor* thisx, PlayState* play);

void EnSyatekiCrow_SetupWaitForSignal(EnSyatekiCrow* this);
void EnSyatekiCrow_WaitForSignal(EnSyatekiCrow* this, PlayState* play);
void EnSyatekiCrow_SetupWaitToMove(EnSyatekiCrow* this);
void EnSyatekiCrow_WaitToMove(EnSyatekiCrow* this, PlayState* play);
void EnSyatekiCrow_Fly(EnSyatekiCrow* this, PlayState* play);
void EnSyatekiCrow_Dead(EnSyatekiCrow* this, PlayState* play);

static Vec3f sZeroVec = { 0.0f, 0.0f, 0.0f };

ActorProfile En_Syateki_Crow_Profile = {
    /**/ ACTOR_EN_SYATEKI_CROW,
    /**/ ACTORCAT_ENEMY,
    /**/ FLAGS,
    /**/ OBJECT_CROW,
    /**/ sizeof(EnSyatekiCrow),
    /**/ EnSyatekiCrow_Init,
    /**/ EnSyatekiCrow_Destroy,
    /**/ EnSyatekiCrow_Update,
    /**/ EnSyatekiCrow_Draw,
};

static ColliderJntSphElementInit sJntSphElementsInit[1] = {
    {
        {
            ELEM_MATERIAL_UNK0,
            { 0xF7CFFFFF, 0x00, 0x00 },
            { 0xF7CFFFFF, 0x00, 0x00 },
            ATELEM_NONE | ATELEM_SFX_NORMAL,
            ACELEM_ON,
            OCELEM_ON,
        },
        { 1, { { 0, 60, 0 }, 50 }, 100 },
    },
};

static ColliderJntSphInit sJntSphInit = {
    {
        COL_MATERIAL_HIT3,
        AT_ON | AT_TYPE_ENEMY,
        AC_ON | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_1,
        COLSHAPE_JNTSPH,
    },
    ARRAY_COUNT(sJntSphElementsInit),
    sJntSphElementsInit,
};

static InitChainEntry sInitChain[] = {
    ICHAIN_F32(cullingVolumeScale, 3000, ICHAIN_CONTINUE),
    ICHAIN_S8(hintId, TATL_HINT_ID_GUAY, ICHAIN_CONTINUE),
    ICHAIN_F32_DIV1000(gravity, -500, ICHAIN_CONTINUE),
    ICHAIN_F32(lockOnArrowOffset, 2000, ICHAIN_STOP),
};

void EnSyatekiCrow_Init(Actor* thisx, PlayState* play2) {
    PlayState* play = play2;
    EnSyatekiCrow* this = (EnSyatekiCrow*)thisx;
    Path* path;
    EnSyatekiMan* syatekiMan = (EnSyatekiMan*)this->actor.parent;
    s32 i;

    path = syatekiMan->path;
    while (path->customValue != SG_PATH_TYPE_GUAY) {
        path = &play->setupPathList[path->additionalPathIndex];
    }

    for (i = 0; i < SG_GUAY_GET_INDEX(&this->actor); i++) {
        path = &play->setupPathList[path->additionalPathIndex];
    }

    Actor_ProcessInitChain(&this->actor, sInitChain);
    SkelAnime_InitFlex(play, &this->skelAnime, &gGuaySkel, &gGuayFlyAnim, this->jointTable, this->morphTable,
                       OBJECT_CROW_LIMB_MAX);
    Collider_InitJntSph(play, &this->collider);
    Collider_SetJntSph(play, &this->collider, &this->actor, &sJntSphInit, this->colliderElements);
    this->collider.elements[0].dim.worldSphere.radius = sJntSphInit.elements[0].dim.modelSphere.radius;
    ActorShape_Init(&this->actor.shape, 2000.0f, ActorShadow_DrawCircle, 20.0f);

    if ((path == NULL) || (SG_GUAY_GET_INDEX(&this->actor) >= 0x80)) {
        Actor_Kill(&this->actor);
        return;
    }

    this->pathPoints = Lib_SegmentedToVirtual(path->points);
    this->currentPointIndex = 1;
    this->maxPointIndex = path->count;
    this->deathTimer = 20;
    this->waitTimer = 0;
    EnSyatekiCrow_SetupWaitForSignal(this);
}

void EnSyatekiCrow_Destroy(Actor* thisx, PlayState* play) {
    EnSyatekiCrow* this = (EnSyatekiCrow*)thisx;

    Collider_DestroyJntSph(play, &this->collider);
}

void EnSyatekiCrow_SetupWaitForSignal(EnSyatekiCrow* this) {
    Actor_SetScale(&this->actor, 0.03f);
    this->actor.speed = 0.0f;
    this->actor.gravity = 0.0f;
    this->actor.world = this->actor.home;
    this->actor.prevPos = this->actor.home.pos;
    this->actor.shape.rot = this->actor.world.rot;
    this->currentPointIndex = 1;
    this->actor.draw = NULL;
    this->actionFunc = EnSyatekiCrow_WaitForSignal;
}

/**
 * Waits until the shooting gallery man sets the appropriate Guay flag.
 */
void EnSyatekiCrow_WaitForSignal(EnSyatekiCrow* this, PlayState* play) {
    EnSyatekiMan* syatekiMan = (EnSyatekiMan*)this->actor.parent;

    if ((syatekiMan->shootingGameState == SG_GAME_STATE_RUNNING) && (this->isActive == true) &&
        (syatekiMan->guayFlags & (1 << SG_GUAY_GET_INDEX(&this->actor)))) {
        EnSyatekiCrow_SetupWaitToMove(this);
    } else if (syatekiMan->shootingGameState != SG_GAME_STATE_RUNNING) {
        this->isActive = true;
    }

    if ((syatekiMan->guayFlags == 0) && (syatekiMan->guayFlags == 0)) {
        this->isActive = true;
    }
}

/**
 * Positions the Guay at the start of its assigned path and rotates it to face the
 * next point on the path, then sets it up to start waiting.
 */
void EnSyatekiCrow_SetupWaitToMove(EnSyatekiCrow* this) {
    Vec3f targetPos;

    this->actor.world.pos.x = this->pathPoints[0].x;
    this->actor.world.pos.y = this->pathPoints[0].y;
    this->actor.world.pos.z = this->pathPoints[0].z;
    targetPos.x = this->pathPoints[this->currentPointIndex].x;
    targetPos.y = this->pathPoints[this->currentPointIndex].y;
    targetPos.z = this->pathPoints[this->currentPointIndex].z;
    this->actor.world.rot.y = this->actor.shape.rot.y = Math_Vec3f_Yaw(&this->actor.world.pos, &targetPos);
    this->actor.world.rot.x = this->actor.shape.rot.x = Math_Vec3f_Pitch(&this->actor.world.pos, &targetPos);
    this->actor.draw = EnSyatekiCrow_Draw;
    this->actionFunc = EnSyatekiCrow_WaitToMove;
}

/**
 * Waits until the wait timer reaches a certain point, then starts flying forward.
 * How long the Guay should wait is controlled by its params.
 */
void EnSyatekiCrow_WaitToMove(EnSyatekiCrow* this, PlayState* play) {
    if (((SG_GUAY_GET_WAIT_MOD(&this->actor) * 20) + 20) < this->waitTimer) {
        Actor_PlaySfx(this->actor.parent, NA_SE_EN_KAICHO_CRY);
        this->waitTimer = 0;
        this->actor.speed = SG_GUAY_GET_SPEED_MOD(&this->actor) + 6.0f;
        this->actor.gravity = -0.5f;
        this->actionFunc = EnSyatekiCrow_Fly;
    } else {
        this->waitTimer++;
    }
}

void EnSyatekiCrow_Fly(EnSyatekiCrow* this, PlayState* play) {
    Vec3f targetPoint;
    f32 distToTarget;
    EnSyatekiMan* syatekiMan = (EnSyatekiMan*)this->actor.parent;

    if (syatekiMan->shootingGameState != SG_GAME_STATE_RUNNING) {
        EnSyatekiCrow_SetupWaitForSignal(this);
        return;
    }

    targetPoint.x = this->pathPoints[this->currentPointIndex].x;
    targetPoint.y = this->pathPoints[this->currentPointIndex].y;
    targetPoint.z = this->pathPoints[this->currentPointIndex].z;

    distToTarget = Math_Vec3f_DistXZ(&this->actor.world.pos, &targetPoint);
    this->yawTarget = Math_Vec3f_Yaw(&this->actor.world.pos, &targetPoint);
    this->pitchTarget = Math_Vec3f_Pitch(&this->actor.world.pos, &targetPoint);

    if (distToTarget > 100.0f) {
        Math_SmoothStepToS(&this->actor.world.rot.y, this->yawTarget, 5, 0x3000, 0x100);
        this->actor.shape.rot.y = this->actor.world.rot.y;
        Math_SmoothStepToS(&this->actor.shape.rot.x, this->pitchTarget, 5, 0x3000, 0x100);
        this->actor.world.rot.x = -this->actor.shape.rot.x;
    } else if (this->currentPointIndex < (this->maxPointIndex - 1)) {
        this->currentPointIndex++;
    } else {
        this->isActive = false;
        syatekiMan->guayFlags &= ~(1 << SG_GUAY_GET_INDEX(&this->actor));
        EnSyatekiCrow_SetupWaitForSignal(this);
    }

    SkelAnime_Update(&this->skelAnime);
    this->actor.shape.yOffset = (fabsf(this->skelAnime.curFrame - 3.0f) * 150.0f) + 1700.0f;
    if ((syatekiMan->guayAppearTimer % 90) == 0) {
        Actor_PlaySfx(&this->actor, NA_SE_EN_KAICHO_CRY);
    }
}

void EnSyatekiCrow_SetupDead(EnSyatekiCrow* this) {
    EnSyatekiMan* syatekiMan = (EnSyatekiMan*)this->actor.parent;

    syatekiMan->score += SG_POINTS_GUAY;
    this->isActive = false;
    this->actor.speed *= Math_CosS(this->actor.world.rot.x);
    this->actor.velocity.y = 0.0f;
    Animation_Change(&this->skelAnime, &gGuayFlyAnim, 0.4f, 0.0f, 0.0f, ANIMMODE_LOOP_INTERP, -3.0f);
    this->actor.bgCheckFlags &= ~BGCHECKFLAG_GROUND;
    Actor_PlaySfx(&this->actor, NA_SE_EN_KAICHO_DEAD);
    Actor_SetColorFilter(&this->actor, COLORFILTER_COLORFLAG_RED, 255, COLORFILTER_BUFFLAG_OPA, 40);
    this->actionFunc = EnSyatekiCrow_Dead;
}

void EnSyatekiCrow_Dead(EnSyatekiCrow* this, PlayState* play) {
    EnSyatekiMan* syatekiMan = (EnSyatekiMan*)this->actor.parent;

    Math_StepToF(&this->actor.speed, 0.0f, 0.5f);
    this->actor.colorFilterTimer = 40;

    if (this->actor.colorFilterParams & 0x4000) {
        Math_ScaledStepToS(&this->actor.shape.rot.x, 0x4000, 0x200);
        this->actor.shape.rot.z += 0x1780;
    }

    if (this->deathTimer > 20) {
        func_800B3030(play, &this->actor.world.pos, &sZeroVec, &sZeroVec, this->actor.scale.x * 10000.0f, 0, 0);
        syatekiMan->guayHitCounter++;
        syatekiMan->guayFlags &= ~(1 << SG_GUAY_GET_INDEX(&this->actor));
        EnSyatekiCrow_SetupWaitForSignal(this);
    }

    this->deathTimer++;
}

static Vec3f sVelocity = { 0.0f, 20.0f, 0.0f };

static Vec3f sAccel = { 0.0f, 0.0f, 0.0f };

void EnSyatekiCrow_UpdateDamage(EnSyatekiCrow* this, PlayState* play) {
    if (this->actionFunc == EnSyatekiCrow_Fly) {
        if (this->collider.base.acFlags & AC_HIT) {
            Audio_PlaySfx(NA_SE_SY_TRE_BOX_APPEAR);
            this->deathTimer = 0;
            this->collider.base.acFlags &= ~AC_HIT;
            EffectSsExtra_Spawn(play, &this->actor.world.pos, &sVelocity, &sAccel, 5, EXTRA_SCORE_INDEX_60);
            EnSyatekiCrow_SetupDead(this);
        } else {
            this->collider.elements[0].dim.worldSphere.center.x = this->actor.world.pos.x;
            this->collider.elements[0].dim.worldSphere.center.y =
                sJntSphInit.elements[0].dim.modelSphere.center.y + this->actor.world.pos.y;
            this->collider.elements[0].dim.worldSphere.center.z = this->actor.world.pos.z;
            CollisionCheck_SetAC(play, &play->colChkCtx, &this->collider.base);
        }
    }
}

void EnSyatekiCrow_Update(Actor* thisx, PlayState* play) {
    EnSyatekiCrow* this = (EnSyatekiCrow*)thisx;

    this->actionFunc(this, play);

    if (this->actionFunc != EnSyatekiCrow_Dead) {
        Actor_MoveWithoutGravity(&this->actor);
    } else {
        Actor_MoveWithGravity(&this->actor);
    }

    EnSyatekiCrow_UpdateDamage(this, play);
}

s32 EnSyatekiCrow_OverrideLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, Actor* thisx) {
    EnSyatekiCrow* this = (EnSyatekiCrow*)thisx;

    if (limbIndex == OBJECT_CROW_LIMB_UPPER_TAIL) {
        rot->y += TRUNCF_BINANG(0xC00 * Math_SinF(this->skelAnime.curFrame * (M_PIf / 4)));
    } else if (limbIndex == OBJECT_CROW_LIMB_TAIL) {
        rot->y += TRUNCF_BINANG(0x1400 * Math_SinF((this->skelAnime.curFrame + 2.5f) * (M_PIf / 4)));
    }

    return false;
}

static Vec3f sBodyOffset = { 2500.0f, 0.0f, 0.0f };

void EnSyatekiCrow_PostLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot, Actor* thisx) {
    EnSyatekiCrow* this = (EnSyatekiCrow*)thisx;
    Vec3f* bodyPartPos;

    if (limbIndex == OBJECT_CROW_LIMB_BODY) {
        Matrix_MultVec3f(&sBodyOffset, &this->bodyPartsPos[SG_GUAY_BODYPART_BODY]);
        this->bodyPartsPos[SG_GUAY_BODYPART_BODY].y -= 20.0f;
    } else if ((limbIndex == OBJECT_CROW_LIMB_RIGHT_WING_TIP) || (limbIndex == OBJECT_CROW_LIMB_LEFT_WING_TIP) ||
               (limbIndex == OBJECT_CROW_LIMB_TAIL)) {
        bodyPartPos = &this->bodyPartsPos[(limbIndex >> 1) - 1];
        Matrix_MultVec3f(&sZeroVec, bodyPartPos);
        bodyPartPos->y -= 20.0f;
    }
}

void EnSyatekiCrow_Draw(Actor* thisx, PlayState* play) {
    EnSyatekiCrow* this = (EnSyatekiCrow*)thisx;

    Gfx_SetupDL25_Opa(play->state.gfxCtx);
    SkelAnime_DrawFlexOpa(play, this->skelAnime.skeleton, this->skelAnime.jointTable, this->skelAnime.dListCount,
                          EnSyatekiCrow_OverrideLimbDraw, EnSyatekiCrow_PostLimbDraw, &this->actor);
}
