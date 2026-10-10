/*
 * File: z_en_syateki_wf.c
 * Overlay: ovl_En_Syateki_Wf
 * Description: Shooting Gallery Wolfos
 */

#include "z_en_syateki_wf.h"
#include "overlays/actors/ovl_En_Syateki_Man/z_en_syateki_man.h"

#define FLAGS (ACTOR_FLAG_UPDATE_CULLING_DISABLED | ACTOR_FLAG_DRAW_CULLING_DISABLED | ACTOR_FLAG_LOCK_ON_DISABLED)

void EnSyatekiWf_Init(Actor* thisx, PlayState* play);
void EnSyatekiWf_Destroy(Actor* thisx, PlayState* play);
void EnSyatekiWf_Update(Actor* thisx, PlayState* play2);
void EnSyatekiWf_Draw(Actor* thisx, PlayState* play);

void EnSyatekiWf_SetupWaitForSignal(EnSyatekiWf* this);
void EnSyatekiWf_WaitForSignal(EnSyatekiWf* this, PlayState* play);
void EnSyatekiWf_SetupWaitToMove(EnSyatekiWf* this);
void EnSyatekiWf_WaitToMove(EnSyatekiWf* this, PlayState* play);
void EnSyatekiWf_SetupRun(EnSyatekiWf* this);
void EnSyatekiWf_Run(EnSyatekiWf* this, PlayState* play);
void EnSyatekiWf_SetupJump(EnSyatekiWf* this);
void EnSyatekiWf_Jump(EnSyatekiWf* this, PlayState* play);
void EnSyatekiWf_SetupLand(EnSyatekiWf* this);
void EnSyatekiWf_Land(EnSyatekiWf* this, PlayState* play);
void EnSyatekiWf_SetupHowl(EnSyatekiWf* this);
void EnSyatekiWf_Howl(EnSyatekiWf* this, PlayState* play);
void EnSyatekiWf_Dead(EnSyatekiWf* this, PlayState* play);

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
        { WOLFOS_NORMAL_LIMB_HEAD, { { 800, 0, 0 }, 25 }, 100 },
    },
};

static ColliderCylinderInit sBodyCylinderInit = {
    {
        COL_MATERIAL_HIT5,
        AT_NONE,
        AC_ON | AC_TYPE_PLAYER,
        OC1_NONE,
        OC2_NONE,
        COLSHAPE_CYLINDER,
    },
    {
        ELEM_MATERIAL_UNK1,
        { 0x00000000, 0x00, 0x00 },
        { 0xF7CFFFFF, 0x00, 0x00 },
        ATELEM_NONE | ATELEM_SFX_NORMAL,
        ACELEM_ON,
        OCELEM_NONE,
    },
    { 40, 60, 0, { 0, 0, 0 } },
};

static ColliderJntSphInit sJntSphInit = {
    {
        COL_MATERIAL_HIT5,
        AT_ON | AT_TYPE_ENEMY,
        AC_ON | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_1,
        COLSHAPE_JNTSPH,
    },
    ARRAY_COUNT(sJntSphElementsInit),
    sJntSphElementsInit,
};

static ColliderCylinderInit sTailCylinderInit = {
    {
        COL_MATERIAL_HIT5,
        AT_NONE,
        AC_ON | AC_TYPE_PLAYER,
        OC1_NONE,
        OC2_NONE,
        COLSHAPE_CYLINDER,
    },
    {
        ELEM_MATERIAL_UNK1,
        { 0x00000000, 0x00, 0x00 },
        { 0xF7CFFFFF, 0x00, 0x00 },
        ATELEM_NONE | ATELEM_SFX_NORMAL,
        ACELEM_ON,
        OCELEM_NONE,
    },
    { 15, 20, -15, { 0, 0, 0 } },
};

static Vec3f sVelocity = { 0.0f, 20.0f, 0.0f };

static Vec3f sAccel = { 0.0f, 0.0f, 0.0f };

ActorProfile En_Syateki_Wf_Profile = {
    /**/ ACTOR_EN_SYATEKI_WF,
    /**/ ACTORCAT_ENEMY,
    /**/ FLAGS,
    /**/ OBJECT_WF,
    /**/ sizeof(EnSyatekiWf),
    /**/ EnSyatekiWf_Init,
    /**/ EnSyatekiWf_Destroy,
    /**/ EnSyatekiWf_Update,
    /**/ EnSyatekiWf_Draw,
};

typedef enum ShootingGalleryWolfosAnimation {
    /* 0 */ SG_WOLFOS_ANIM_WAIT, // unused
    /* 1 */ SG_WOLFOS_ANIM_RUN,
    /* 2 */ SG_WOLFOS_ANIM_JUMP,
    /* 3 */ SG_WOLFOS_ANIM_LAND,
    /* 4 */ SG_WOLFOS_ANIM_BACKFLIP, // unused
    /* 5 */ SG_WOLFOS_ANIM_DAMAGED,
    /* 6 */ SG_WOLFOS_ANIM_REAR_UP_FALL_OVER,
    /* 7 */ SG_WOLFOS_ANIM_MAX
} ShootingGalleryWolfosAnimation;

static AnimationInfo sAnimationInfo[SG_WOLFOS_ANIM_MAX] = {
    { &gWolfosWaitAnim, 2.0f, 0.0f, 0.0f, ANIMMODE_LOOP, -1.0f },           // SG_WOLFOS_ANIM_WAIT
    { &gWolfosRunAnim, 1.0f, 0.0f, 0.0f, ANIMMODE_LOOP, -8.0f },            // SG_WOLFOS_ANIM_RUN
    { &gWolfosRunAnim, 1.0f, 0.0f, 4.0f, ANIMMODE_ONCE, 1.0f },             // SG_WOLFOS_ANIM_JUMP
    { &gWolfosRunAnim, 1.0f, 4.0f, 8.0f, ANIMMODE_ONCE, 1.0f },             // SG_WOLFOS_ANIM_LAND
    { &gWolfosBackflipAnim, 1.0f, 0.0f, 0.0f, ANIMMODE_ONCE, -1.0f },       // SG_WOLFOS_ANIM_BACKFLIP
    { &gWolfosDamagedAnim, 1.0f, 0.0f, 0.0f, ANIMMODE_ONCE, 8.0f },         // SG_WOLFOS_ANIM_DAMAGED
    { &gWolfosRearUpFallOverAnim, 1.0f, 0.0f, 0.0f, ANIMMODE_ONCE, -1.0f }, // SG_WOLFOS_ANIM_REAR_UP_FALL_OVER
};

static InitChainEntry sInitChain[] = {
    ICHAIN_F32(lockOnArrowOffset, 2000, ICHAIN_CONTINUE),
    ICHAIN_F32_DIV1000(gravity, -2000, ICHAIN_STOP),
};

void EnSyatekiWf_Init(Actor* thisx, PlayState* play) {
    s32 pad;
    EnSyatekiWf* this = (EnSyatekiWf*)thisx;
    Path* path;
    EnSyatekiMan* syatekiMan = (EnSyatekiMan*)this->actor.parent;
    s32 i;

    path = syatekiMan->path;
    while (path->customValue != SG_PATH_TYPE_WOLFOS) {
        path = &play->setupPathList[path->additionalPathIndex];
    }

    for (i = 0; i < SG_WOLFOS_GET_INDEX(&this->actor); i++) {
        path = &play->setupPathList[path->additionalPathIndex];
    }

    if (path == NULL) {
        Actor_Kill(&this->actor);
        return;
    }

    this->pathPoints = Lib_SegmentedToVirtual(path->points);
    this->currentPointIndex = 1;
    this->maxPointIndex = path->count;

    Actor_ProcessInitChain(&this->actor, sInitChain);
    this->waitTimer = 0;
    ActorShape_Init(&this->actor.shape, 0.0f, ActorShadow_DrawCircle, 0.0f);
    this->actor.focus.pos = this->actor.world.pos;
    this->actor.colChkInfo.mass = MASS_IMMOVABLE;
    this->actor.colChkInfo.health = 2;
    this->actor.colChkInfo.cylRadius = 50;
    this->actor.colChkInfo.cylHeight = 100;
    this->eyeIndex = 0;
    this->unk_2AC = 10.0f;

    Collider_InitCylinder(play, &this->bodyCollider);
    Collider_SetCylinder(play, &this->bodyCollider, &this->actor, &sBodyCylinderInit);
    Collider_InitCylinder(play, &this->tailCollider);
    Collider_SetCylinder(play, &this->tailCollider, &this->actor, &sTailCylinderInit);
    Collider_InitJntSph(play, &this->headCollider);
    Collider_SetJntSph(play, &this->headCollider, &this->actor, &sJntSphInit, this->headColliderElements);
    this->headCollider.elements[0].dim.worldSphere.radius = sJntSphInit.elements[0].dim.modelSphere.radius;

    SkelAnime_InitFlex(play, &this->skelAnime, &gWolfosNormalSkel, &gWolfosWaitAnim, this->jointTable, this->morphTable,
                       WOLFOS_NORMAL_LIMB_MAX);
    Actor_SetScale(&this->actor, 0.01f);
    this->actor.hintId = TATL_HINT_ID_WOLFOS;

    EnSyatekiWf_SetupWaitForSignal(this);
}

void EnSyatekiWf_Destroy(Actor* thisx, PlayState* play) {
    EnSyatekiWf* this = (EnSyatekiWf*)thisx;

    Collider_DestroyCylinder(play, &this->bodyCollider);
    Collider_DestroyCylinder(play, &this->tailCollider);
}

/**
 * Positions the Wolfos at the start of its assigned path and rotates it to face
 * the next point on the path.
 */
void EnSyatekiWf_InitPathStart(EnSyatekiWf* this) {
    Vec3f targetPos;

    this->actor.world.pos.x = this->pathPoints[0].x;
    this->actor.world.pos.y = this->pathPoints[0].y;
    this->actor.world.pos.z = this->pathPoints[0].z;
    targetPos.x = this->pathPoints[this->currentPointIndex].x;
    targetPos.y = this->pathPoints[this->currentPointIndex].y;
    targetPos.z = this->pathPoints[this->currentPointIndex].z;
    this->actor.world.rot.y = this->actor.shape.rot.y = Math_Vec3f_Yaw(&this->actor.world.pos, &targetPos);
}

void EnSyatekiWf_SetupWaitForSignal(EnSyatekiWf* this) {
    EnSyatekiMan* syatekiMan = (EnSyatekiMan*)this->actor.parent;

    this->actor.speed = 0.0f;
    this->actor.world = this->actor.home;
    this->actor.prevPos = this->actor.home.pos;
    this->actor.shape.rot = this->actor.world.rot;
    this->actor.colChkInfo.health = 2;
    this->actor.draw = NULL;
    this->currentPointIndex = 1;
    this->isActive = false;
    syatekiMan->wolfosFlags &= ~(1 << SG_WOLFOS_GET_INDEX(&this->actor));
    this->actionFunc = EnSyatekiWf_WaitForSignal;
}

/**
 * Waits until the shooting gallery man sets the appropriate Wolfos flag.
 */
void EnSyatekiWf_WaitForSignal(EnSyatekiWf* this, PlayState* play) {
    EnSyatekiMan* syatekiMan;

    if (this->actor.parent != NULL) {
        syatekiMan = (EnSyatekiMan*)this->actor.parent;
        if ((syatekiMan->shootingGameState == SG_GAME_STATE_RUNNING) && (this->isActive == true)) {
            EnSyatekiWf_InitPathStart(this);
            EnSyatekiWf_SetupWaitToMove(this);
        } else if (syatekiMan->wolfosFlags & (1 << SG_WOLFOS_GET_INDEX(&this->actor))) {
            this->isActive = true;
        }
    }
}

void EnSyatekiWf_SetupWaitToMove(EnSyatekiWf* this) {
    this->actionFunc = EnSyatekiWf_WaitToMove;
}

/**
 * Waits 11 frames, then makes the Wolfos start running forward.
 */
void EnSyatekiWf_WaitToMove(EnSyatekiWf* this, PlayState* play) {
    if (this->waitTimer >= 11) {
        Actor_PlaySfx(this->actor.parent, NA_SE_EN_WOLFOS_APPEAR);
        this->waitTimer = 0;
        EnSyatekiWf_SetupRun(this);
    } else {
        this->waitTimer++;
    }
}

void EnSyatekiWf_SetupRun(EnSyatekiWf* this) {
    Actor_ChangeAnimationByInfo(&this->skelAnime, sAnimationInfo, SG_WOLFOS_ANIM_RUN);
    this->actor.speed = 10.0f;
    this->actor.world.rot.y = this->actor.shape.rot.y;
    this->actor.draw = EnSyatekiWf_Draw;
    this->actionFunc = EnSyatekiWf_Run;
}

/**
 * Runs forward along its assigned path. If the Wolfos encounters a wall directly
 * in front of it, this function will make it jump. If the Wolfos reaches a certain
 * point along the path (which is specified via its params), this function will make
 * it stop running and howl in place.
 */
void EnSyatekiWf_Run(EnSyatekiWf* this, PlayState* play) {
    Vec3f targetPoint;
    f32 distToTarget;
    s16 wallYawDiff;
    EnSyatekiMan* syatekiMan = (EnSyatekiMan*)this->actor.parent;

    if (syatekiMan->shootingGameState != SG_GAME_STATE_RUNNING) {
        EnSyatekiWf_SetupWaitForSignal(this);
    }

    targetPoint.x = this->pathPoints[this->currentPointIndex].x;
    targetPoint.y = this->pathPoints[this->currentPointIndex].y;
    targetPoint.z = this->pathPoints[this->currentPointIndex].z;
    wallYawDiff = (this->actor.wallYaw - this->actor.world.rot.y) + 0x8000;

    if (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND) {
        if (this->actor.bgCheckFlags & BGCHECKFLAG_WALL) {
            if ((ABS(wallYawDiff) < 0x1555) && (this->actor.wallPoly != this->actor.floorPoly)) {
                EnSyatekiWf_SetupJump(this);
                return;
            }
        }

        if (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND_LEAVE) {
            this->actor.velocity.y = 2.0f;
        }

        distToTarget = Math_Vec3f_DistXZ(&this->actor.world.pos, &targetPoint);
        this->yawTarget = Math_Vec3f_Yaw(&this->actor.world.pos, &targetPoint);

        if (distToTarget > 15.0f) {
            Math_SmoothStepToS(&this->actor.world.rot.y, this->yawTarget, 5, 0x3000, 0x100);
            this->actor.shape.rot.y = this->actor.world.rot.y;
            if (distToTarget < 50.0f) {
                if (this->actor.speed > 3.0f) {
                    this->actor.speed -= 0.5f;
                } else {
                    this->actor.speed = this->actor.speed;
                }
            }
        } else {
            if (this->currentPointIndex < (this->maxPointIndex - 1)) {
                if (this->currentPointIndex == SG_WOLFOS_GET_POINT_TO_HOWL(&this->actor)) {
                    EnSyatekiWf_SetupHowl(this);
                }

                this->currentPointIndex++;
            } else {
                this->isActive = false;
                this->currentPointIndex = 1;
                EnSyatekiWf_SetupWaitForSignal(this);
            }
        }

        if (Animation_OnFrame(&this->skelAnime, this->skelAnime.endFrame)) {
            Actor_SpawnFloorDustRing(play, &this->actor, &this->actor.world.pos, 10.0f, 3, 2.0f, 0, 0, false);
        }
    }
}

void EnSyatekiWf_SetupJump(EnSyatekiWf* this) {
    Actor_PlaySfx(&this->actor, NA_SE_EN_TEKU_JUMP);
    this->actor.velocity.y = 20.0f;
    this->actor.speed = 5.0f;
    Actor_ChangeAnimationByInfo(&this->skelAnime, sAnimationInfo, SG_WOLFOS_ANIM_JUMP);
    this->actionFunc = EnSyatekiWf_Jump;
}

void EnSyatekiWf_Jump(EnSyatekiWf* this, PlayState* play) {
    if (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND_TOUCH) {
        EnSyatekiWf_SetupLand(this);
    }
}

void EnSyatekiWf_SetupLand(EnSyatekiWf* this) {
    this->actor.speed = 0.0f;
    Actor_ChangeAnimationByInfo(&this->skelAnime, sAnimationInfo, SG_WOLFOS_ANIM_LAND);
    this->actionFunc = EnSyatekiWf_Land;
}

/**
 * Freezes the Wolfos in place until its landing animation is complete, then makes it start running.
 */
void EnSyatekiWf_Land(EnSyatekiWf* this, PlayState* play) {
    if (Animation_OnFrame(&this->skelAnime, this->skelAnime.endFrame)) {
        EnSyatekiWf_SetupRun(this);
    }
}

void EnSyatekiWf_SetupHowl(EnSyatekiWf* this) {
    this->timer = 40;
    this->actor.speed = 0.0f;
    Actor_PlaySfx(&this->actor, NA_SE_EN_WOLFOS_APPEAR);
    Actor_ChangeAnimationByInfo(&this->skelAnime, sAnimationInfo, SG_WOLFOS_ANIM_DAMAGED);
    this->actionFunc = EnSyatekiWf_Howl;
}

/**
 * Makes the Wolfos stand completely still until 40 frames after its damaged animation is complete.
 */
void EnSyatekiWf_Howl(EnSyatekiWf* this, PlayState* play) {
    if (Animation_OnFrame(&this->skelAnime, this->skelAnime.endFrame)) {
        this->timer--;
        if (this->timer == 0) {
            EnSyatekiWf_SetupRun(this);
        }
    }
}

void EnSyatekiWf_SetupDead(EnSyatekiWf* this, PlayState* play) {
    EnSyatekiMan* syatekiMan = (EnSyatekiMan*)this->actor.parent;

    this->isActive = false;
    this->actor.speed = 0.0f;
    EffectSsExtra_Spawn(play, &this->actor.world.pos, &sVelocity, &sAccel, 5, EXTRA_SCORE_INDEX_100);
    Actor_PlaySfx(&this->actor, NA_SE_EN_WOLFOS_DEAD);
    Actor_ChangeAnimationByInfo(&this->skelAnime, sAnimationInfo, SG_WOLFOS_ANIM_REAR_UP_FALL_OVER);
    syatekiMan->score += SG_POINTS_WOLFOS;
    this->actionFunc = EnSyatekiWf_Dead;
}

void EnSyatekiWf_Dead(EnSyatekiWf* this, PlayState* play) {
    s32 pad;

    if (Animation_OnFrame(&this->skelAnime, this->skelAnime.endFrame)) {
        EnSyatekiWf_SetupWaitForSignal(this);
    } else {
        Vec3f firePos;
        Vec3f sFireVelocityAndAccel = { 0.0f, 0.5f, 0.0f };
        s32 i;

        for (i = (s32)this->skelAnime.animLength - (s32)this->skelAnime.curFrame; i >= 0; i--) {
            firePos.x = Rand_CenteredFloat(60.0f) + this->actor.world.pos.x;
            firePos.z = Rand_CenteredFloat(60.0f) + this->actor.world.pos.z;
            firePos.y = Rand_CenteredFloat(50.0f) + (this->actor.world.pos.y + 20.0f);
            func_800B3030(play, &firePos, &sFireVelocityAndAccel, &sFireVelocityAndAccel, 100, 0, 2);
        }
    }
}

void EnSyatekiWf_Update(Actor* thisx, PlayState* play2) {
    PlayState* play = play2;
    EnSyatekiWf* this = (EnSyatekiWf*)thisx;

    if (this->actionFunc != EnSyatekiWf_WaitForSignal) {
        SkelAnime_Update(&this->skelAnime);
    }

    Actor_MoveWithGravity(&this->actor);
    Actor_UpdateBgCheckInfo(play, &this->actor, 32.0f, 30.0f, 60.0f, UPDBGCHECKINFO_FLAG_1 | UPDBGCHECKINFO_FLAG_4);
    this->actionFunc(this, play);

    if (this->actor.bgCheckFlags & (BGCHECKFLAG_GROUND | BGCHECKFLAG_GROUND_TOUCH)) {
        func_800BE3D0(&this->actor, this->actor.shape.rot.y, &this->actor.shape.rot);
    } else {
        Math_SmoothStepToS(&this->actor.shape.rot.x, 0, 1, 0x3E8, 0);
        Math_SmoothStepToS(&this->actor.shape.rot.z, 0, 1, 0x3E8, 0);
    }

    if ((this->bodyCollider.base.acFlags & AC_HIT) || (this->tailCollider.base.acFlags & AC_HIT) ||
        (this->headCollider.base.acFlags & AC_HIT)) {
        this->bodyCollider.base.acFlags &= ~AC_HIT;
        this->tailCollider.base.acFlags &= ~AC_HIT;
        this->headCollider.base.acFlags &= ~AC_HIT;

        //! @bug: The Wolfos always starts with 2 health, so the subtraction below is guaranteed to kill it;
        //! the else-block is never reached in practice. If you *could* damage the Wolfos without killing
        //! it, then the number "30" would appear every time you hit it, and the player's displayed score
        //! would increase by 30. However, the else-block doesn't increase the shooting gallery man's
        //! "score" variable, so it would become desynchronized from the displayed score. This could cause
        //! weird behavior, like not getting a free replay after finishing a game with 2000 or more points.
        this->actor.colChkInfo.health -= 2;
        if (this->actor.colChkInfo.health == 0) {
            // BENTODO This had | 0x900 which interfered with the 16 bit sequence IDs. Removing it doesn't seem to
            // anything bad.
            Audio_PlayFanfare(NA_BGM_GET_ITEM);
            EnSyatekiWf_SetupDead(this, play);
        } else {
            Audio_PlaySfx(NA_SE_SY_TRE_BOX_APPEAR);
            EffectSsExtra_Spawn(play, &this->actor.world.pos, &sVelocity, &sAccel, 3, EXTRA_SCORE_INDEX_30);
        }
    }

    Collider_UpdateCylinder(&this->actor, &this->bodyCollider);
    if ((this->actionFunc != EnSyatekiWf_WaitForSignal) && (this->actionFunc != EnSyatekiWf_Dead) &&
        (this->actor.draw != NULL)) {
        CollisionCheck_SetAC(play, &play->colChkCtx, &this->tailCollider.base);
        CollisionCheck_SetAC(play, &play->colChkCtx, &this->bodyCollider.base);
        CollisionCheck_SetAC(play, &play->colChkCtx, &this->headCollider.base);
        this->actor.focus.pos = this->actor.world.pos;
        this->actor.focus.pos.y += 25.0f;
    }

    if (this->eyeIndex == 0) {
        if ((Rand_ZeroOne() < 0.2f) && ((play->gameplayFrames & 3) == 0) && (this->actor.colorFilterTimer == 0)) {
            this->eyeIndex++;
        }
    } else {
        this->eyeIndex = (this->eyeIndex + 1) & 3;
    }
}

s32 EnSyatekiWf_OverrideLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, Actor* thisx) {
    return false;
}

void EnSyatekiWf_PostLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot, Actor* thisx) {
    static Vec3f sTailColliderOffset = { 1200.0f, 0.0f, 0.0f };
    EnSyatekiWf* this = (EnSyatekiWf*)thisx;
    Vec3f tailColliderPos;

    Collider_UpdateSpheres(limbIndex, &this->headCollider);
    if (limbIndex == WOLFOS_NORMAL_LIMB_TAIL) {
        Matrix_MultVec3f(&sTailColliderOffset, &tailColliderPos);
        this->tailCollider.dim.pos.x = tailColliderPos.x;
        this->tailCollider.dim.pos.y = tailColliderPos.y;
        this->tailCollider.dim.pos.z = tailColliderPos.z;
    }
}

void EnSyatekiWf_Draw(Actor* thisx, PlayState* play) {
    static TexturePtr sEyeTextures[] = {
        gWolfosNormalEyeOpenTex,
        gWolfosNormalEyeHalfTex,
        gWolfosNormalEyeNarrowTex,
        gWolfosNormalEyeHalfTex,
    };
    EnSyatekiWf* this = (EnSyatekiWf*)thisx;

    OPEN_DISPS(play->state.gfxCtx);

    Gfx_SetupDL25_Opa(play->state.gfxCtx);
    gSPSegment(POLY_OPA_DISP++, 0x08, Lib_SegmentedToVirtual(sEyeTextures[this->eyeIndex]));
    SkelAnime_DrawFlexOpa(play, this->skelAnime.skeleton, this->skelAnime.jointTable, this->skelAnime.dListCount,
                          EnSyatekiWf_OverrideLimbDraw, EnSyatekiWf_PostLimbDraw, &this->actor);

    CLOSE_DISPS(play->state.gfxCtx);
}
