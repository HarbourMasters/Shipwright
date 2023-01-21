/*
 * File: z_en_st.c
 * Overlay: ovl_En_St
 * Description: Skulltula (normal, big, invisible)
 */

#include "z_en_st.h"
#include "objects/object_st/object_st.h"

#define FLAGS (ACTOR_FLAG_0 | ACTOR_FLAG_2 | ACTOR_FLAG_4 | ACTOR_FLAG_5)

void EnSt_Init(Actor* thisx, PlayState* play);
void EnSt_Destroy(Actor* thisx, PlayState* play);
void EnSt_Update(Actor* thisx, PlayState* play);
void EnSt_Draw(Actor* thisx, PlayState* play);
void EnSt_ReturnToCeiling(EnSt* this, PlayState* play);
void EnSt_MoveToGround(EnSt* this, PlayState* play);
void EnSt_StartOnCeilingOrGround(EnSt* this, PlayState* play);
void EnSt_WaitOnGround(EnSt* this, PlayState* play);
void EnSt_Die(EnSt* this, PlayState* play);
void EnSt_BounceAround(EnSt* this, PlayState* play);
void EnSt_FinishBouncing(EnSt* this, PlayState* play);

#include "overlays/ovl_En_St/ovl_En_St.h"

const ActorInit En_St_InitVars = {
    ACTOR_EN_ST,
    ACTORCAT_ENEMY,
    FLAGS,
    OBJECT_ST,
    sizeof(EnSt),
    (ActorFunc)EnSt_Init,
    (ActorFunc)EnSt_Destroy,
    (ActorFunc)EnSt_Update,
    (ActorFunc)EnSt_Draw,
    NULL,
};

static ColliderCylinderInit sCylinderInit = {
    {
        COLTYPE_HIT6,
        AT_NONE,
        AC_ON | AC_TYPE_PLAYER,
        OC1_NONE,
        OC2_TYPE_1,
        COLSHAPE_CYLINDER,
    },
    {
        ELEMTYPE_UNK0,
        { 0x00000000, 0x00, 0x00 },
        { 0x00000000, 0x00, 0x00 },
        TOUCH_ON | TOUCH_SFX_NORMAL,
        BUMP_ON,
        OCELEM_NONE,
    },
    { 32, 50, -24, { 0, 0, 0 } },
};

static CollisionCheckInfoInit2 sColChkInit = { 2, 0, 0, 0, MASS_IMMOVABLE };

static ColliderCylinderInit sCylinderInit2 = {
    {
        COLTYPE_HIT6,
        AT_NONE,
        AC_NONE,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_1,
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
    { 20, 60, -30, { 0, 0, 0 } },
};

static ColliderJntSphElementInit sJntSphElementsInit[1] = {
    {
        {
            ELEMTYPE_UNK0,
            { 0xFFCFFFFF, 0x00, 0x04 },
            { 0x00000000, 0x00, 0x00 },
            TOUCH_ON | TOUCH_SFX_NORMAL,
            BUMP_NONE,
            OCELEM_ON,
        },
        { 1, { { 0, -240, 0 }, 28 }, 100 },
    },
};

static ColliderJntSphInit sJntSphInit = {
    {
        COLTYPE_HIT6,
        AT_ON | AT_TYPE_ENEMY,
        AC_NONE,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_1,
        COLSHAPE_JNTSPH,
    },
    1,
    sJntSphElementsInit,
};

typedef enum {
    /* 0 */ ENST_ANIM_0,
    /* 1 */ ENST_ANIM_1,
    /* 2 */ ENST_ANIM_2,
    /* 3 */ ENST_ANIM_3,
    /* 4 */ ENST_ANIM_4,
    /* 5 */ ENST_ANIM_5,
    /* 6 */ ENST_ANIM_6,
    /* 7 */ ENST_ANIM_7
} EnStAnimation;

static AnimationInfo sAnimationInfo[] = {
    { &object_st_Anim_000304, 1.0f, 0.0f, -1.0f, ANIMMODE_LOOP_INTERP, 0.0f },
    { &object_st_Anim_005B98, 1.0f, 0.0f, -1.0f, ANIMMODE_ONCE_INTERP, -8.0f },
    { &object_st_Anim_000304, 4.0f, 0.0f, -1.0f, ANIMMODE_ONCE_INTERP, -8.0f },
    { &object_st_Anim_000304, 1.0f, 0.0f, -1.0f, ANIMMODE_LOOP_INTERP, -8.0f },
    { &object_st_Anim_0055A8, 1.0f, 0.0f, -1.0f, ANIMMODE_ONCE_INTERP, -8.0f },
    { &object_st_Anim_000304, 8.0f, 0.0f, -1.0f, ANIMMODE_LOOP_INTERP, -8.0f },
    { &object_st_Anim_000304, 6.0f, 0.0f, -1.0f, ANIMMODE_LOOP_INTERP, -8.0f },
    { &object_st_Anim_005B98, 2.0f, 0.0f, -1.0f, ANIMMODE_LOOP_INTERP, -8.0f },
};

void EnSt_SetupAction(EnSt* this, EnStActionFunc actionFunc) {
    this->actionFunc = actionFunc;
}

/**
 * Spawns `dustCnt` dust particles in a random pattern around the Skulltula
 */
void EnSt_SpawnDust(EnSt* this, PlayState* play, s32 dustCnt) {
    Color_RGBA8 primColor = { 170, 130, 90, 255 };
    Color_RGBA8 envColor = { 100, 60, 20, 0 };
    Vec3f dustVel = { 0.0f, 0.0f, 0.0f };
    Vec3f dustAccel = { 0.0f, 0.3f, 0.0f };
    Vec3f dustPos;
    s16 yAngle;
    s32 i;

    yAngle = (Rand_ZeroOne() - 0.5f) * 65536.0f;
    dustPos.y = this->actor.floorHeight;
    for (i = dustCnt; i >= 0; i--, yAngle += (s16)(0x10000 / dustCnt)) {
        dustAccel.x = (Rand_ZeroOne() - 0.5f) * 4.0f;
        dustAccel.z = (Rand_ZeroOne() - 0.5f) * 4.0f;
        dustPos.x = this->actor.world.pos.x + (Math_SinS(yAngle) * 22.0f);
        dustPos.z = this->actor.world.pos.z + (Math_CosS(yAngle) * 22.0f);
        func_8002836C(play, &dustPos, &dustVel, &dustAccel, &primColor, &envColor, 120, 40, 10);
    }
}

void EnSt_SpawnBlastEffect(EnSt* this, PlayState* play) {
    Vec3f zeroVec = { 0.0f, 0.0f, 0.0f };
    Vec3f blastPos;

    blastPos.x = this->actor.world.pos.x;
    blastPos.y = this->actor.floorHeight;
    blastPos.z = this->actor.world.pos.z;

    EffectSsBlast_SpawnWhiteCustomScale(play, &blastPos, &zeroVec, &zeroVec, 100, 220, 8);
}

void EnSt_SpawnDeadEffect(EnSt* this, PlayState* play) {
    Vec3f zeroVec = { 0.0f, 0.0f, 0.0f };
    Vec3f firePos;

    firePos.x = this->actor.world.pos.x + ((Rand_ZeroOne() - 0.5f) * 60.0f);
    firePos.y = (this->actor.world.pos.y + 10.0f) + ((Rand_ZeroOne() - 0.5f) * 45.0f);
    firePos.z = this->actor.world.pos.z + ((Rand_ZeroOne() - 0.5f) * 60.0f);
    EffectSsDeadDb_Spawn(play, &firePos, &zeroVec, &zeroVec, 100, 0, 255, 255, 255, 255, 255, 0, 0, 1, 9, true);
}

s32 EnSt_CreateBlureEffect(PlayState* play) {
    EffectBlureInit1 blureInit;
    u8 p1StartColor[] = { 255, 255, 255, 75 };
    u8 p2StartColor[] = { 255, 255, 255, 75 };
    u8 p1EndColor[] = { 255, 255, 255, 0 };
    u8 p2EndColor[] = { 255, 255, 255, 0 };
    s32 i;
    s32 blureIdx;

    for (i = 0; i < 4; i++) {
        blureInit.p1StartColor[i] = p1StartColor[i];
        blureInit.p2StartColor[i] = p2StartColor[i];
        blureInit.p1EndColor[i] = p1EndColor[i];
        blureInit.p2EndColor[i] = p2EndColor[i];
    }

    blureInit.elemDuration = 6;
    blureInit.unkFlag = 0;
    blureInit.calcMode = 3;

    Effect_Add(play, &blureIdx, EFFECT_BLURE1, 0, 0, &blureInit);
    return blureIdx;
}

/**
 * Checks for the position of the ceiling above the Skulltula.
 * If no ceiling is found it is set to 1000 units above the Skulltula
 */
s32 EnSt_CheckCeilingPos(EnSt* this, PlayState* play) {
    CollisionPoly* poly;
    s32 bgId;
    Vec3f checkPos;

    checkPos.x = this->actor.world.pos.x;
    checkPos.y = this->actor.world.pos.y + 1000.0f;
    checkPos.z = this->actor.world.pos.z;
    if (!BgCheck_EntityLineTest1(&play->colCtx, &this->actor.world.pos, &checkPos, &this->ceilingPos, &poly, false,
                                 false, true, true, &bgId)) {
        return false;
    }
    this->unusedPos = this->actor.world.pos;
    this->unusedPos.y -= 100.0f;
    return true;
}

void EnSt_AddBlurVertex(EnSt* this) {
    Vec3f v1 = { 834.0f, 834.0f, 0.0f };
    Vec3f v2 = { 834.0f, -584.0f, 0.0f };
    Vec3f v1Pos;
    Vec3f v2Pos;

    v1.x *= this->colliderScale;
    v1.y *= this->colliderScale;
    v1.z *= this->colliderScale;

    v2.x *= this->colliderScale;
    v2.y *= this->colliderScale;
    v2.z *= this->colliderScale;

    Matrix_Push();
    Matrix_MultVec3f(&v1, &v1Pos);
    Matrix_MultVec3f(&v2, &v2Pos);
    Matrix_Pop();
    EffectBlure_AddVertex(Effect_GetByIndex(this->blureIdx), &v1Pos, &v2Pos);
}

void EnSt_AddBlurSpace(EnSt* this) {
    EffectBlure_AddSpace(Effect_GetByIndex(this->blureIdx));
}

void EnSt_SetWaitingAnimation(EnSt* this) {
    Animation_ChangeByInfo(&this->skelAnime, sAnimationInfo, ENST_ANIM_3);
}

void EnSt_SetReturnToCeilingAnimation(EnSt* this) {
    Audio_PlayActorSound2(&this->actor, NA_SE_EN_STALTU_UP);
    Animation_ChangeByInfo(&this->skelAnime, sAnimationInfo, ENST_ANIM_2);
}

void EnSt_SetLandAnimation(EnSt* this) {
    this->actor.world.pos.y = this->actor.floorHeight + this->floorHeightOffset;
    Animation_ChangeByInfo(&this->skelAnime, sAnimationInfo, ENST_ANIM_4);
    this->sfxTimer = 0;
    this->animFrames = this->skelAnime.animLength;
}

void EnSt_SetDropAnimAndVel(EnSt* this) {
    if (this->takeDamageSpinTimer == 0) {
        Animation_ChangeByInfo(&this->skelAnime, sAnimationInfo, ENST_ANIM_4);
        this->animFrames = this->skelAnime.animLength;
    }
    this->sfxTimer = 0;
    this->actor.velocity.y = -10.0f;
}

/**
 * Initalizes the Skulltula's 6 cylinders, and sphere collider.
 */
void EnSt_InitColliders(EnSt* this, PlayState* play) {
    ColliderCylinderInit* cylinders[6] = {
        &sCylinderInit, &sCylinderInit, &sCylinderInit, &sCylinderInit2, &sCylinderInit2, &sCylinderInit2,
    };

    s32 i;
    s32 pad;

    for (i = 0; i < ARRAY_COUNT(cylinders); i++) {
        Collider_InitCylinder(play, &this->colCylinder[i]);
        Collider_SetCylinder(play, &this->colCylinder[i], &this->actor, cylinders[i]);
    }

    this->colCylinder[0].info.bumper.dmgFlags = 0x0003F8F9;
    this->colCylinder[1].info.bumper.dmgFlags = 0xFFC00706;
    this->colCylinder[2].base.colType = COLTYPE_METAL;
    this->colCylinder[2].info.bumperFlags = BUMP_ON | BUMP_HOOKABLE | BUMP_NO_AT_INFO;
    this->colCylinder[2].info.elemType = ELEMTYPE_UNK2;
    this->colCylinder[2].info.bumper.dmgFlags = 0xFFCC0706;

    CollisionCheck_SetInfo2(&this->actor.colChkInfo, DamageTable_Get(2), &sColChkInit);

    Collider_InitJntSph(play, &this->colSph);
    Collider_SetJntSph(play, &this->colSph, &this->actor, &sJntSphInit, this->colSphItems);
}

void EnSt_CheckBodyStickHit(EnSt* this, PlayState* play) {
    ColliderInfo* body = &this->colCylinder[0].info;
    Player* player = GET_PLAYER(play);

    if (player->unk_860 != 0) {
        body->bumper.dmgFlags |= 2;
        this->colCylinder[1].info.bumper.dmgFlags &= ~2;
        this->colCylinder[2].info.bumper.dmgFlags &= ~2;
    } else {
        body->bumper.dmgFlags &= ~2;
        this->colCylinder[1].info.bumper.dmgFlags |= 2;
        this->colCylinder[2].info.bumper.dmgFlags |= 2;
    }
}

void EnSt_SetBodyCylinderAC(EnSt* this, PlayState* play) {
    Collider_UpdateCylinder(&this->actor, &this->colCylinder[0]);
    CollisionCheck_SetAC(play, &play->colChkCtx, &this->colCylinder[0].base);
}

void EnSt_SetLegsCylinderAC(EnSt* this, PlayState* play) {
    s16 angleTowardsLink = ABS((s16)(this->actor.yawTowardsPlayer - this->actor.shape.rot.y));

    if (angleTowardsLink < 0x3FFC) {
        Collider_UpdateCylinder(&this->actor, &this->colCylinder[2]);
        CollisionCheck_SetAC(play, &play->colChkCtx, &this->colCylinder[2].base);
    } else {
        Collider_UpdateCylinder(&this->actor, &this->colCylinder[1]);
        CollisionCheck_SetAC(play, &play->colChkCtx, &this->colCylinder[1].base);
    }
}

s32 EnSt_SetCylinderOC(EnSt* this, PlayState* play) {
    Vec3f cyloffsets[] = {
        { 40.0f, 0.0f, 0.0f },
        { 0.0f, 0.0f, 0.0f },
        { -40.0f, 0.0f, 0.0f },
    };
    Vec3f cylPos;
    s32 i;

    for (i = 0; i < 3; i++) {
        cylPos = this->actor.world.pos;
        cyloffsets[i].x *= this->colliderScale;
        cyloffsets[i].y *= this->colliderScale;
        cyloffsets[i].z *= this->colliderScale;
        Matrix_Push();
        Matrix_Translate(cylPos.x, cylPos.y, cylPos.z, MTXMODE_NEW);
        Matrix_RotateY((this->initalYaw / 32768.0f) * M_PI, MTXMODE_APPLY);
        Matrix_MultVec3f(&cyloffsets[i], &cylPos);
        Matrix_Pop();
        this->colCylinder[i + 3].dim.pos.x = cylPos.x;
        this->colCylinder[i + 3].dim.pos.y = cylPos.y;
        this->colCylinder[i + 3].dim.pos.z = cylPos.z;
        CollisionCheck_SetOC(play, &play->colChkCtx, &this->colCylinder[i + 3].base);
    }

    return true;
}

void EnSt_UpdateCylinders(EnSt* this, PlayState* play) {
    if ((this->actor.colChkInfo.health != 0) || (this->actionFunc == EnSt_FinishBouncing)) {
        if (DECR(this->gaveDamageSpinTimer) == 0) {
            EnSt_SetCylinderOC(this, play);
        }

        DECR(this->invulnerableTimer);
        DECR(this->takeDamageSpinTimer);

        if (this->invulnerableTimer == 0 && this->takeDamageSpinTimer == 0) {
            EnSt_SetBodyCylinderAC(this, play);
            EnSt_SetLegsCylinderAC(this, play);
        }
    }
}

s32 EnSt_CheckHitLink(EnSt* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    s32 hit;
    s32 i;

    for (i = 0, hit = 0; i < 3; i++) {
        if (((this->colCylinder[i + 3].base.ocFlags2 & OC2_HIT_PLAYER) != 0) == 0) {
            continue;
        }
        this->colCylinder[i + 3].base.ocFlags2 &= ~OC2_HIT_PLAYER;
        hit = true;
    }

    if (!hit) {
        return false;
    }

    if (this->swayTimer == 0) {
        Audio_PlayActorSound2(&this->actor, NA_SE_EN_STALTU_ROLL);
    }

    this->gaveDamageSpinTimer = 30;
    play->damagePlayer(play, -8);
    Audio_PlayActorSound2(&player->actor, NA_SE_PL_BODY_HIT);
    func_8002F71C(play, &this->actor, 4.0f, this->actor.yawTowardsPlayer, 6.0f);
    return true;
}

s32 EnSt_CheckHitFrontside(EnSt* this) {
    u8 acFlags = this->colCylinder[2].base.acFlags;

    if (!!(acFlags & AC_HIT) == 0) {
        // not hit
        return false;
    } else {
        this->colCylinder[2].base.acFlags &= ~AC_HIT;
        this->invulnerableTimer = 8;
        this->playSwayFlag = 0;
        this->swayTimer = 60;
        return true;
    }
}

s32 EnSt_CheckHitBackside(EnSt* this, PlayState* play) {
    ColliderCylinder* cyl = &this->colCylinder[0];
    s32 flags = 0; // ac hit flags from colliders 0 and 1
    s32 hit = false;

    if (cyl->base.acFlags & AC_HIT) {
        cyl->base.acFlags &= ~AC_HIT;
        hit = true;
        flags |= cyl->info.acHitInfo->toucher.dmgFlags;
    }

    cyl = &this->colCylinder[1];
    if (cyl->base.acFlags & AC_HIT) {
        cyl->base.acFlags &= ~AC_HIT;
        hit = true;
        flags |= cyl->info.acHitInfo->toucher.dmgFlags;
    }

    if (!hit) {
        return false;
    }

    this->invulnerableTimer = 8;
    if (this->actor.colChkInfo.damageEffect == 1) {
        if (this->stunTimer == 0) {
            Audio_PlayActorSound2(&this->actor, NA_SE_EN_GOMA_JR_FREEZE);
            this->stunTimer = 120;
            Actor_SetColorFilter(&this->actor, 0, 0xC8, 0, this->stunTimer);
        }
        return false;
    }

    this->swayTimer = this->stunTimer = 0;
    this->gaveDamageSpinTimer = 1;
    Animation_ChangeByInfo(&this->skelAnime, sAnimationInfo, ENST_ANIM_3);
    this->takeDamageSpinTimer = this->skelAnime.animLength;
    Actor_SetColorFilter(&this->actor, 0x4000, 0xC8, 0, this->takeDamageSpinTimer);
    if (Actor_ApplyDamage(&this->actor)) {
        Audio_PlayActorSound2(&this->actor, NA_SE_EN_STALTU_DAMAGE);
        return false;
    }
    Enemy_StartFinishingBlow(play, &this->actor);
    this->actor.flags &= ~ACTOR_FLAG_0;
    this->groundBounces = 3;
    this->deathTimer = 20;
    this->actor.gravity = -1.0f;
    Audio_PlayActorSound2(&this->actor, NA_SE_EN_STALWALL_DEAD);
    if (this->actor.params == 1) {
        gSaveContext.sohStats.count[COUNT_ENEMIES_DEFEATED_SKULLTULA_BIG]++;
    } else {
        gSaveContext.sohStats.count[COUNT_ENEMIES_DEFEATED_SKULLTULA]++;
    }

    if (flags & 0x1F820) {
        // arrow, fire arrow, ice arrow, light arrow,
        // and three unknows, unused arrows?
        EnSt_SetupAction(this, EnSt_Die);
        this->finishDeathTimer = 8;
    } else {
        EnSt_SetupAction(this, EnSt_BounceAround);
    }

    return true;
}

/**
 * Checks if the Skulltula's colliders have been hit, returns true if the hit has dealt damage to the Skulltula
 */
s32 EnSt_CheckColliders(EnSt* this, PlayState* play) {
    if (EnSt_CheckHitFrontside(this)) {
        // player has hit the front shield area of the Skulltula
        return false;
    }

    if (play->actorCtx.unk_02 != 0) {
        return true;
    }

    if (EnSt_CheckHitBackside(this, play)) {
        // player has hit the backside of the Skulltula
        return true;
    }

    if (this->stunTimer == 0 && this->takeDamageSpinTimer == 0) {
        // check if the Skulltula has hit link.
        EnSt_CheckHitLink(this, play);
    }
    return false;
}

void EnSt_SetColliderScale(EnSt* this) {
    f32 scaleAmount = 1.0f;
    f32 radius;
    f32 height;
    f32 yShift;
    s32 i;

    if (this->actor.params == 1) {
        scaleAmount = 1.4f;
    }

    radius = this->colSph.elements[0].dim.modelSphere.radius;
    radius *= scaleAmount;
    this->colSph.elements[0].dim.modelSphere.radius = radius;

    for (i = 0; i < 6; i++) {
        yShift = this->colCylinder[i].dim.yShift;
        radius = this->colCylinder[i].dim.radius;
        height = this->colCylinder[i].dim.height;
        yShift *= scaleAmount;
        radius *= scaleAmount;
        height *= scaleAmount;

        this->colCylinder[i].dim.yShift = yShift;
        this->colCylinder[i].dim.radius = radius;
        this->colCylinder[i].dim.height = height;
    }
    Actor_SetScale(&this->actor, 0.04f * scaleAmount);
    this->colliderScale = scaleAmount;
    this->floorHeightOffset = 32.0f * scaleAmount;
}

s32 EnSt_SetTeethColor(EnSt* this, s16 redTarget, s16 greenTarget, s16 blueTarget, s16 minMaxStep) {
    s16 red = this->teethR;
    s16 green = this->teethG;
    s16 blue = this->teethB;

    minMaxStep = 255 / (s16)(0.6f * minMaxStep);
    if (minMaxStep <= 0) {
        minMaxStep = 1;
    }

    Math_SmoothStepToS(&red, redTarget, 1, minMaxStep, minMaxStep);
    Math_SmoothStepToS(&green, greenTarget, 1, minMaxStep, minMaxStep);
    Math_SmoothStepToS(&blue, blueTarget, 1, minMaxStep, minMaxStep);
    this->teethR = red;
    this->teethG = green;
    this->teethB = blue;
    return 1;
}

s32 EnSt_DecrStunTimer(EnSt* this) {
    if (this->stunTimer == 0) {
        return 0;
    }
    this->stunTimer--; //! @bug  no return but v0 ends up being stunTimer before decrement
}

/**
 * Updates the yaw of the Skulltula, used for the shaking animation right before
 * turning, and the actual turning to face away from the player, and then back to
 * face the player
 */
void EnSt_UpdateYaw(EnSt* this, PlayState* play) {
    u16 yawDir = 0;
    Vec3s rot;
    s16 yawDiff;
    s16 timer;
    s16 yawTarget;

    // Shake towards the end of the stun.
    if (this->stunTimer != 0) {
        if (this->stunTimer < 30) {
            if ((this->stunTimer % 2) != 0) {
                this->actor.shape.rot.y += 0x800;
            } else {
                this->actor.shape.rot.y -= 0x800;
            }
        }
        return;
    }

    if (this->swayTimer == 0 && this->deathTimer == 0 && this->finishDeathTimer == 0) {
        // not swaying or dying
        if (this->takeDamageSpinTimer != 0 || this->gaveDamageSpinTimer != 0) {
            // Skulltula is doing a spinning animation
            this->actor.shape.rot.y += 0x2000;
            return;
        }

        if (this->actionFunc != EnSt_WaitOnGround) {
            // set the timers to turn away or turn towards the player
            this->rotAwayTimer = 30;
            this->rotTowardsTimer = 0;
        }

        if (this->rotAwayTimer != 0) {
            // turn away from the player
            this->rotAwayTimer--;
            if (this->rotAwayTimer == 0) {
                Audio_PlayActorSound2(&this->actor, NA_SE_EN_STALTU_ROLL);
                this->rotTowardsTimer = 30;
            }
        } else if (this->rotTowardsTimer != 0) {
            // turn towards the player
            this->rotTowardsTimer--;
            if (this->rotTowardsTimer == 0) {
                Audio_PlayActorSound2(&this->actor, NA_SE_EN_STALTU_ROLL);
                this->rotAwayTimer = 30;
            }
            yawDir = 0x8000;
        }

        // calculate the new yaw to or away from the player.
        rot = this->actor.shape.rot;
        yawTarget = (this->actionFunc == EnSt_WaitOnGround ? this->actor.yawTowardsPlayer : this->initalYaw);
        yawDiff = rot.y - (yawTarget ^ yawDir);
        if (ABS(yawDiff) <= 0x4000) {
            Math_SmoothStepToS(&rot.y, yawTarget ^ yawDir, 4, 0x2000, 1);
        } else {
            rot.y += 0x2000;
        }

        this->actor.shape.rot = this->actor.world.rot = rot;

        // Do the shaking animation.
        if (yawDir == 0 && this->rotAwayTimer < 0xA) {
            timer = this->rotAwayTimer;
        } else if (yawDir == 0x8000 && this->rotTowardsTimer < 0xA) {
            timer = this->rotTowardsTimer;
        } else {
            return;
        }

        if ((timer % 2) != 0) {
            this->actor.shape.rot.y += 0x800;
        } else {
            this->actor.shape.rot.y -= 0x800;
        }
    }
}

/**
 * Checks to see if the Skulltula is done bouncing on the ground,
 * spawns dust particles as the Skulltula hits the ground
 */
s32 EnSt_IsDoneBouncing(EnSt* this, PlayState* play) {
    if (this->actor.velocity.y > 0.0f || this->groundBounces == 0) {
        // the Skulltula is moving upwards or the groundBounces is 0
        return false;
    }

    if (!(this->actor.bgCheckFlags & 1)) {
        // the Skulltula is not on the ground.
        return false;
    }

    Audio_PlayActorSound2(&this->actor, NA_SE_EN_DODO_M_GND);
    EnSt_SpawnDust(this, play, 10);
    // creates an elastic bouncing effect, boucing up less for each hit on the ground.
    this->actor.velocity.y = 6.0f / (4 - this->groundBounces);
    this->groundBounces--;
    if (this->groundBounces != 0) {
        return false;
    } else {
        // make sure the Skulltula stays on the ground.
        this->actor.velocity.y = 0.0f;
    }
    return true;
}

void EnSt_Bob(EnSt* this, PlayState* play) {
    f32 ySpeedTarget = 0.5f;

    if ((play->state.frames & 8) != 0) {
        ySpeedTarget *= -1.0f;
    }
    Math_SmoothStepToF(&this->actor.velocity.y, ySpeedTarget, 0.4f, 1000.0f, 0.0f);
}

s32 EnSt_IsCloseToPlayer(EnSt* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    f32 yDist;

    if (this->takeDamageSpinTimer != 0) {
        // skull is spinning from damage.
        return false;
    } else if (this->actor.xzDistToPlayer > 160.0f) {
        // player is more than 160 xz units from the Skulltula
        return false;
    }

    yDist = this->actor.world.pos.y - player->actor.world.pos.y;
    if (yDist < 0.0f || yDist > 400.0f) {
        // player is above the Skulltula or more than 400 units below
        // the Skulltula
        return false;
    }

    if (player->actor.world.pos.y < this->actor.floorHeight) {
        // player is below the Skulltula's ground position
        return false;
    }
    return true;
}

s32 EnSt_IsCloseToInitalPos(EnSt* this) {
    f32 velY = this->actor.velocity.y;
    f32 checkY = this->actor.world.pos.y + (velY * 2.0f);

    if (checkY >= this->actor.home.pos.y) {
        return true;
    }
    return false;
}

s32 EnSt_IsCloseToGround(EnSt* this) {
    f32 velY = this->actor.velocity.y;
    f32 checkY = this->actor.world.pos.y + (velY * 2.0f);

    if (checkY - this->actor.floorHeight <= this->floorHeightOffset) {
        return true;
    }
    return false;
}

/**
 * Does the animation of the Skulltula swaying back and forth after the Skulltula
 * has been hit in the front by a sword
 */
void EnSt_Sway(EnSt* this) {
    Vec3f amtToTranslate;
    Vec3f translatedPos;
    f32 swayAmt;
    s16 rotAngle;

    if (this->swayTimer != 0) {

        this->swayAngle += 0xA28;
        this->swayTimer--;

        if (this->swayTimer == 0) {
            this->swayAngle = 0;
        }

        swayAmt = this->swayTimer * (7.0f / 15.0f);
        rotAngle = Math_SinS(this->swayAngle) * (swayAmt * (65536.0f / 360.0f));

        if (this->absPrevSwayAngle >= ABS(rotAngle) && this->playSwayFlag == 0) {
            Audio_PlayActorSound2(&this->actor, NA_SE_EN_STALTU_WAVE);
            this->playSwayFlag = 1;
        }

        if (this->absPrevSwayAngle < ABS(rotAngle)) {
            this->playSwayFlag = 0;
        }

        this->absPrevSwayAngle = ABS(rotAngle);
        amtToTranslate.x = Math_SinS(rotAngle) * -200.0f;
        amtToTranslate.y = Math_CosS(rotAngle) * -200.0f;
        amtToTranslate.z = 0.0f;
        Matrix_Push();
        Matrix_Translate(this->ceilingPos.x, this->ceilingPos.y, this->ceilingPos.z, MTXMODE_NEW);
        Matrix_RotateY(this->actor.world.rot.y * (M_PI / 32768.0f), MTXMODE_APPLY);
        Matrix_MultVec3f(&amtToTranslate, &translatedPos);
        Matrix_Pop();
        this->actor.shape.rot.z = -(rotAngle * 2);
        this->actor.world.pos.x = translatedPos.x;
        this->actor.world.pos.z = translatedPos.z;
    }
}

void EnSt_Init(Actor* thisx, PlayState* play) {
    EnSt* this = (EnSt*)thisx;
    s32 pad;

    ActorShape_Init(&this->actor.shape, 0.0f, ActorShadow_DrawCircle, 14.0f);
    SkelAnime_Init(play, &this->skelAnime, &object_st_Skel_005298, NULL, this->jointTable, this->morphTable, 30);
    Animation_ChangeByInfo(&this->skelAnime, sAnimationInfo, ENST_ANIM_0);
    this->blureIdx = EnSt_CreateBlureEffect(play);
    EnSt_InitColliders(this, play);
    if (thisx->params == 2) {
        this->actor.flags |= ACTOR_FLAG_7;
    }
    if (this->actor.params == 1) {
        this->actor.naviEnemyId = 0x05;
    } else {
        this->actor.naviEnemyId = 0x04;
    }
    EnSt_CheckCeilingPos(this, play);
    this->actor.flags |= ACTOR_FLAG_14;
    this->actor.flags |= ACTOR_FLAG_24;
    EnSt_SetColliderScale(this);
    this->actor.gravity = 0.0f;
    this->initalYaw = this->actor.world.rot.y;
    EnSt_SetupAction(this, EnSt_StartOnCeilingOrGround);
}

void EnSt_Destroy(Actor* thisx, PlayState* play) {
    EnSt* this = (EnSt*)thisx;
    s32 i;

    Effect_Delete(play, this->blureIdx);
    for (i = 0; i < 6; i++) {
        Collider_DestroyCylinder(play, &this->colCylinder[i]);
    }
    Collider_DestroyJntSph(play, &this->colSph);
}

void EnSt_WaitOnCeiling(EnSt* this, PlayState* play) {
    if (EnSt_IsCloseToPlayer(this, play)) {
        EnSt_SetDropAnimAndVel(this);
        EnSt_SetupAction(this, EnSt_MoveToGround);
    } else {
        EnSt_Bob(this, play);
    }
}

/**
 * Skulltula is waiting on the ground for the player to move away, or for
 * a collider to have contact
 */
void EnSt_WaitOnGround(EnSt* this, PlayState* play) {
    if (this->takeDamageSpinTimer != 0) {
        this->takeDamageSpinTimer--;
        if (this->takeDamageSpinTimer == 0) {
            Animation_ChangeByInfo(&this->skelAnime, sAnimationInfo, ENST_ANIM_3);
        }
    }

    if (this->animFrames != 0) {
        this->animFrames--;
        if (this->animFrames == 0) {
            Animation_ChangeByInfo(&this->skelAnime, sAnimationInfo, ENST_ANIM_3);
        }
    }

    if (!EnSt_IsCloseToPlayer(this, play)) {
        // Player is no longer within range, return to ceiling.
        EnSt_SetReturnToCeilingAnimation(this);
        EnSt_SetupAction(this, EnSt_ReturnToCeiling);
        return;
    }

    if (DECR(this->sfxTimer) == 0) {
        // play the "laugh" sfx every 64 frames.
        Audio_PlayActorSound2(&this->actor, NA_SE_EN_STALTU_LAUGH);
        this->sfxTimer = 64;
    }

    // simply bob up and down.
    EnSt_Bob(this, play);
}

void EnSt_LandOnGround(EnSt* this, PlayState* play) {
    if (this->animFrames != 0) {
        this->animFrames--;
        if (this->animFrames == 0) {
            Animation_ChangeByInfo(&this->skelAnime, sAnimationInfo, ENST_ANIM_3);
        }
    }

    if (this->takeDamageSpinTimer != 0) {
        this->takeDamageSpinTimer--;
        if (this->takeDamageSpinTimer == 0) {
            Animation_ChangeByInfo(&this->skelAnime, sAnimationInfo, ENST_ANIM_3);
        }
    }

    this->sfxTimer++;
    if (this->sfxTimer == 14) {
        // play the sound effect of the Skulltula hitting the ground.
        Audio_PlayActorSound2(&this->actor, NA_SE_EN_STALTU_DOWN_SET);
    }

    if ((this->actor.floorHeight + this->floorHeightOffset) < this->actor.world.pos.y) {
        // the Skulltula has hit the ground.
        this->sfxTimer = 0;
        EnSt_SetupAction(this, EnSt_WaitOnGround);
    } else {
        Math_SmoothStepToF(&this->actor.velocity.y, 2.0f, 0.3f, 1.0f, 0.0f);
    }
}

void EnSt_MoveToGround(EnSt* this, PlayState* play) {
    if (this->takeDamageSpinTimer != 0) {
        this->takeDamageSpinTimer--;
        if (this->takeDamageSpinTimer == 0) {
            Animation_ChangeByInfo(&this->skelAnime, sAnimationInfo, ENST_ANIM_5);
        }
    }

    if (!EnSt_IsCloseToPlayer(this, play)) {
        // the player moved out of range, return to the ceiling.
        EnSt_SetReturnToCeilingAnimation(this);
        EnSt_SetupAction(this, EnSt_ReturnToCeiling);
    } else if (EnSt_IsCloseToGround(this)) {
        // The Skulltula has become close to the ground.
        EnSt_SpawnBlastEffect(this, play);
        EnSt_SetLandAnimation(this);
        EnSt_SetupAction(this, EnSt_LandOnGround);
    } else if (DECR(this->sfxTimer) == 0) {
        Audio_PlayActorSound2(&this->actor, NA_SE_EN_STALTU_DOWN);
        this->sfxTimer = 3;
    }
}

void EnSt_ReturnToCeiling(EnSt* this, PlayState* play) {
    f32 animPctDone = this->skelAnime.curFrame / (this->skelAnime.animLength - 1.0f);

    if (animPctDone == 1.0f) {
        EnSt_SetReturnToCeilingAnimation(this);
    }

    if (EnSt_IsCloseToPlayer(this, play)) {
        // player came back into range
        EnSt_SetDropAnimAndVel(this);
        EnSt_SetupAction(this, EnSt_MoveToGround);
    } else if (EnSt_IsCloseToInitalPos(this)) {
        // the Skulltula is close to the initial postion.
        EnSt_SetWaitingAnimation(this);
        EnSt_SetupAction(this, EnSt_WaitOnCeiling);
    } else {
        // accelerate based on the current animation frame.
        this->actor.velocity.y = 4.0f * animPctDone;
    }
}

/**
 * The Skulltula has been killed, bounce around
 */
void EnSt_BounceAround(EnSt* this, PlayState* play) {
    this->actor.colorFilterTimer = this->deathTimer;
    func_8002D868(&this->actor);
    this->actor.world.rot.x += 0x800;
    this->actor.world.rot.z -= 0x800;
    this->actor.shape.rot = this->actor.world.rot;
    if (EnSt_IsDoneBouncing(this, play)) {
        this->actor.shape.yOffset = 400.0f;
        this->actor.speedXZ = 1.0f;
        this->actor.gravity = -2.0f;
        EnSt_SetupAction(this, EnSt_FinishBouncing);
    } else {
        Math_SmoothStepToF(&this->actor.shape.yOffset, 400.0f, 0.4f, 10000.0f, 0.0f);
    }
}

/**
 * Finish up the bouncing animation, and rotate towards the final position
 */
void EnSt_FinishBouncing(EnSt* this, PlayState* play) {
    Vec3f zeroVec = { 0.0f, 0.0f, 0.0f };

    if (DECR(this->deathTimer) == 0) {
        this->actor.velocity = zeroVec;
        this->finishDeathTimer = 8;
        EnSt_SetupAction(this, EnSt_Die);
        return;
    }

    if (DECR(this->setTargetYawTimer) == 0) {
        this->deathYawTarget = Math_Vec3f_Yaw(&this->actor.world.pos, &this->actor.home.pos);
        this->setTargetYawTimer = 8;
    }

    Math_SmoothStepToS(&this->actor.world.rot.x, 0x3FFC, 4, 0x2710, 1);
    Math_SmoothStepToS(&this->actor.world.rot.z, 0, 4, 0x2710, 1);
    Math_SmoothStepToS(&this->actor.world.rot.y, this->deathYawTarget, 0xA, 0x2710, 1);

    this->actor.shape.rot = this->actor.world.rot;

    func_8002D868(&this->actor);
    this->groundBounces = 2;
    EnSt_IsDoneBouncing(this, play);
}

/**
 * Spawn the enemy dying effects, and drop a random item
 */
void EnSt_Die(EnSt* this, PlayState* play) {
    if (DECR(this->finishDeathTimer) != 0) {
        EnSt_SpawnDeadEffect(this, play);
    } else {
        Item_DropCollectibleRandom(play, NULL, &this->actor.world.pos, 0xE0);
        Actor_Kill(&this->actor);
    }
}

void EnSt_StartOnCeilingOrGround(EnSt* this, PlayState* play) {
    if (!EnSt_IsCloseToGround(this)) {
        this->rotAwayTimer = 60;
        EnSt_SetupAction(this, EnSt_WaitOnCeiling);
        EnSt_WaitOnCeiling(this, play);
    } else {
        EnSt_SetLandAnimation(this);
        EnSt_SetupAction(this, EnSt_LandOnGround);
        EnSt_LandOnGround(this, play);
    }
}

void EnSt_Update(Actor* thisx, PlayState* play) {
    EnSt* this = (EnSt*)thisx;
    s32 pad;
    Color_RGBA8 color = { 0, 0, 0, 0 };

    if (this->actor.flags & ACTOR_FLAG_15) {
        SkelAnime_Update(&this->skelAnime);
    } else if (!EnSt_CheckColliders(this, play)) {
        // no collision has been detected.

        if (this->stunTimer == 0) {
            SkelAnime_Update(&this->skelAnime);
        }

        if (this->swayTimer == 0 && this->stunTimer == 0) {
            func_8002D7EC(&this->actor);
        }

        Actor_UpdateBgCheckInfo(play, &this->actor, 0.0f, 0.0f, 0.0f, 4);

        if ((this->stunTimer == 0) && (this->swayTimer == 0)) {
            // run the current action if the Skulltula isn't stunned
            // or swaying.
            this->actionFunc(this, play);
        } else if (this->stunTimer != 0) {
            // decrement the stun timer.
            EnSt_DecrStunTimer(this);
        } else {
            // sway the Skulltula.
            EnSt_Sway(this);
        }

        EnSt_UpdateYaw(this, play);

        if (this->actionFunc == EnSt_WaitOnGround) {
            if ((play->state.frames & 0x10) != 0) {
                color.r = 255;
            }
        }

        EnSt_SetTeethColor(this, color.r, color.g, color.b, 8);
        EnSt_UpdateCylinders(this, play);
        Actor_SetFocus(&this->actor, 0.0f);
    }
}

s32 EnSt_OverrideLimbDraw(PlayState* play, s32 limbIndex, Gfx** dListP, Vec3f* pos, Vec3s* rot, void* thisx) {
    EnSt* this = (EnSt*)thisx;

    OPEN_DISPS(play->state.gfxCtx);
    switch (limbIndex) {
        case 1:
            if (this->gaveDamageSpinTimer != 0 && this->swayTimer == 0) {
                if (this->gaveDamageSpinTimer >= 2) {
                    EnSt_AddBlurVertex(this);
                } else {
                    EnSt_AddBlurSpace(this);
                }
            }
            break;
        case 4:
            // teeth
            gDPPipeSync(POLY_OPA_DISP++);
            gDPSetEnvColor(POLY_OPA_DISP++, this->teethR, this->teethG, this->teethB, 0);
            break;
    }
    CLOSE_DISPS(play->state.gfxCtx);
    return false;
}

void EnSt_PostLimbDraw(PlayState* play, s32 limbIndex, Gfx** dListP, Vec3s* rot, void* thisx) {
    EnSt* this = (EnSt*)thisx;

    Collider_UpdateSpheres(limbIndex, &this->colSph);
}

void EnSt_Draw(Actor* thisx, PlayState* play) {
    EnSt* this = (EnSt*)thisx;

    EnSt_CheckBodyStickHit(this, play);
    Gfx_SetupDL_25Opa(play->state.gfxCtx);
    SkelAnime_DrawOpa(play, this->skelAnime.skeleton, this->skelAnime.jointTable, EnSt_OverrideLimbDraw,
                      EnSt_PostLimbDraw, this);
}
