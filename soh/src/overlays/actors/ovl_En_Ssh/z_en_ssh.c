#include "z_en_ssh.h"
#include "objects/object_ssh/object_ssh.h"

#define FLAGS (ACTOR_FLAG_0 | ACTOR_FLAG_2 | ACTOR_FLAG_4 | ACTOR_FLAG_5)

#define SSH_STATE_STUNNED (1 << 0)
#define SSH_STATE_GROUND_START (1 << 2)
#define SSH_STATE_ATTACKED (1 << 3)
#define SSH_STATE_SPIN (1 << 4)

typedef enum {
    SSH_ANIM_UNK0, // Unused animation. Possibly being knocked back?
    SSH_ANIM_UP,
    SSH_ANIM_WAIT,
    SSH_ANIM_LAND,
    SSH_ANIM_DROP,
    SSH_ANIM_UNK5, // Slower version of ANIM_DROP
    SSH_ANIM_UNK6  // Faster repeating version of ANIM_UNK0
} EnSshAnimation;

void EnSsh_Init(Actor* thisx, PlayState* play);
void EnSsh_Destroy(Actor* thisx, PlayState* play);
void EnSsh_Update(Actor* thisx, PlayState* play);
void EnSsh_Draw(Actor* thisx, PlayState* play);

void EnSsh_Idle(EnSsh* this, PlayState* play);
void EnSsh_Drop(EnSsh* this, PlayState* play);
void EnSsh_Return(EnSsh* this, PlayState* play);
void EnSsh_Start(EnSsh* this, PlayState* play);

#include "overlays/ovl_En_Ssh/ovl_En_Ssh.h"

const ActorInit En_Ssh_InitVars = {
    ACTOR_EN_SSH,
    ACTORCAT_NPC,
    FLAGS,
    OBJECT_SSH,
    sizeof(EnSsh),
    (ActorFunc)EnSsh_Init,
    (ActorFunc)EnSsh_Destroy,
    (ActorFunc)EnSsh_Update,
    (ActorFunc)EnSsh_Draw,
    NULL,
};

static ColliderCylinderInit sCylinderInit1 = {
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

static CollisionCheckInfoInit2 sColChkInfoInit = { 1, 0, 0, 0, MASS_IMMOVABLE };

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
    ARRAY_COUNT(sJntSphElementsInit),
    sJntSphElementsInit,
};

void EnSsh_SetupAction(EnSsh* this, EnSshActionFunc actionFunc) {
    this->actionFunc = actionFunc;
}

void EnSsh_SpawnShockwave(EnSsh* this, PlayState* play) {
    Vec3f zeroVec = { 0.0f, 0.0f, 0.0f };
    Vec3f pos;

    pos.x = this->actor.world.pos.x;
    pos.y = this->actor.floorHeight;
    pos.z = this->actor.world.pos.z;
    EffectSsBlast_SpawnWhiteCustomScale(play, &pos, &zeroVec, &zeroVec, 100, 220, 8);
}

s32 EnSsh_CreateBlureEffect(PlayState* play) {
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

s32 EnSsh_CheckCeilingPos(EnSsh* this, PlayState* play) {
    CollisionPoly* poly;
    s32 bgId;
    Vec3f posB;

    posB.x = this->actor.world.pos.x;
    posB.y = this->actor.world.pos.y + 1000.0f;
    posB.z = this->actor.world.pos.z;
    if (!BgCheck_EntityLineTest1(&play->colCtx, &this->actor.world.pos, &posB, &this->ceilingPos, &poly, false,
                                 false, true, true, &bgId)) {
        return false;
    } else {
        return true;
    }
}

void EnSsh_AddBlureVertex(EnSsh* this) {
    Vec3f p1base = { 834.0f, 834.0f, 0.0f };
    Vec3f p2base = { 834.0f, -584.0f, 0.0f };
    Vec3f p1;
    Vec3f p2;

    p1base.x *= this->colliderScale;
    p1base.y *= this->colliderScale;
    p1base.z *= this->colliderScale;
    p2base.x *= this->colliderScale;
    p2base.y *= this->colliderScale;
    p2base.z *= this->colliderScale;
    Matrix_Push();
    Matrix_MultVec3f(&p1base, &p1);
    Matrix_MultVec3f(&p2base, &p2);
    Matrix_Pop();
    EffectBlure_AddVertex(Effect_GetByIndex(this->blureIdx), &p1, &p2);
}

void EnSsh_AddBlureSpace(EnSsh* this) {
    EffectBlure_AddSpace(Effect_GetByIndex(this->blureIdx));
}

void EnSsh_InitColliders(EnSsh* this, PlayState* play) {
    ColliderCylinderInit* cylinders[6] = {
        &sCylinderInit1, &sCylinderInit1, &sCylinderInit1, &sCylinderInit2, &sCylinderInit2, &sCylinderInit2,
    };
    s32 i;
    s32 pad;

    for (i = 0; i < ARRAY_COUNT(cylinders); i++) {
        Collider_InitCylinder(play, &this->colCylinder[i]);
        Collider_SetCylinder(play, &this->colCylinder[i], &this->actor, cylinders[i]);
    }

    this->colCylinder[0].info.bumper.dmgFlags = 0x0003F8E9;
    this->colCylinder[1].info.bumper.dmgFlags = 0xFFC00716;
    this->colCylinder[2].base.colType = COLTYPE_METAL;
    this->colCylinder[2].info.bumperFlags = BUMP_ON | BUMP_HOOKABLE | BUMP_NO_AT_INFO;
    this->colCylinder[2].info.elemType = ELEMTYPE_UNK2;
    this->colCylinder[2].info.bumper.dmgFlags = 0xFFCC0716;

    CollisionCheck_SetInfo2(&this->actor.colChkInfo, DamageTable_Get(2), &sColChkInfoInit);

    Collider_InitJntSph(play, &this->colSph);
    Collider_SetJntSph(play, &this->colSph, &this->actor, &sJntSphInit, this->colSphElements);
}

f32 EnSsh_SetAnimation(EnSsh* this, s32 animIndex) {
    AnimationHeader* animation[] = {
        &object_ssh_Anim_005BE8, &object_ssh_Anim_000304, &object_ssh_Anim_000304, &object_ssh_Anim_0055F8,
        &object_ssh_Anim_000304, &object_ssh_Anim_000304, &object_ssh_Anim_005BE8,
    };
    f32 playbackSpeed[] = { 1.0f, 4.0f, 1.0f, 1.0f, 8.0f, 6.0f, 2.0f };
    u8 mode[] = { 3, 3, 1, 3, 1, 1, 1 };
    f32 frameCount = Animation_GetLastFrame(animation[animIndex]);
    s32 pad;

    Animation_Change(&this->skelAnime, animation[animIndex], playbackSpeed[animIndex], 0.0f, frameCount,
                     mode[animIndex], -6.0f);

    return frameCount;
}

void EnSsh_SetWaitAnimation(EnSsh* this) {
    EnSsh_SetAnimation(this, SSH_ANIM_WAIT);
}

void EnSsh_SetReturnAnimation(EnSsh* this) {
    Audio_PlayActorSound2(&this->actor, NA_SE_EN_STALTU_UP);
    EnSsh_SetAnimation(this, SSH_ANIM_UP);
}

void EnSsh_SetLandAnimation(EnSsh* this) {
    this->actor.world.pos.y = this->floorHeightOffset + this->actor.floorHeight;
    this->animTimer = EnSsh_SetAnimation(this, SSH_ANIM_LAND);
}

void EnSsh_SetDropAnimation(EnSsh* this) {
    if (this->unkTimer == 0) {
        this->animTimer = EnSsh_SetAnimation(this, SSH_ANIM_DROP);
    }
    this->actor.velocity.y = -10.0f;
}

void EnSsh_SetStunned(EnSsh* this) {
    if (this->stunTimer == 0) {
        this->stateFlags |= SSH_STATE_ATTACKED;
        this->stunTimer = 120;
        this->actor.colorFilterTimer = 0;
    }
}

void EnSsh_SetColliderScale(EnSsh* this, f32 scale, f32 radiusMod) {
    f32 radius;
    f32 height;
    f32 yShift;
    s32 i;

    radius = this->colSph.elements[0].dim.modelSphere.radius;
    radius *= scale;
    this->colSph.elements[0].dim.modelSphere.radius = radius;

    for (i = 0; i < 6; i++) {
        yShift = this->colCylinder[i].dim.yShift;
        radius = this->colCylinder[i].dim.radius;
        height = this->colCylinder[i].dim.height;
        yShift *= scale;
        radius *= scale * radiusMod;
        height *= scale;

        this->colCylinder[i].dim.yShift = yShift;
        this->colCylinder[i].dim.radius = radius;
        this->colCylinder[i].dim.height = height;
    }
    Actor_SetScale(&this->actor, 0.04f * scale);
    this->floorHeightOffset = 40.0f * scale;
    this->colliderScale = scale * 1.5f;
}

s32 EnSsh_Damaged(EnSsh* this) {
    if ((this->stunTimer == 120) && (this->stateFlags & SSH_STATE_STUNNED)) {
        Actor_SetColorFilter(&this->actor, 0, 0xC8, 0, this->stunTimer);
    }
    if (DECR(this->stunTimer) != 0) {
        Math_SmoothStepToS(&this->maxTurnRate, 0x2710, 0xA, 0x3E8, 1);
        return false;
    } else {
        this->stunTimer = 0;
        this->stateFlags &= ~SSH_STATE_STUNNED;
        this->spinTimer = 0;
        if (this->swayTimer == 0) {
            this->spinTimer = 30;
        }
        Audio_PlayActorSound2(&this->actor, NA_SE_EN_STALTU_ROLL);
        Audio_PlayActorSound2(&this->actor, NA_SE_VO_ST_ATTACK);
        return true;
    }
}

void EnSsh_Turn(EnSsh* this, PlayState* play) {
    if (this->hitTimer != 0) {
        this->hitTimer--;
    }
    if (DECR(this->spinTimer) != 0) {
        this->actor.world.rot.y += 10000.0f * (this->spinTimer / 30.0f);
    } else if ((this->swayTimer == 0) && (this->stunTimer == 0)) {
        Math_SmoothStepToS(&this->actor.world.rot.y, this->actor.yawTowardsPlayer, 4, 0x2710, 1);
    }
    this->actor.shape.rot.y = this->actor.world.rot.y;
}

void EnSsh_Stunned(EnSsh* this, PlayState* play) {
    if ((this->swayTimer == 0) && (this->stunTimer == 0)) {
        Math_SmoothStepToS(&this->actor.world.rot.y, this->actor.yawTowardsPlayer ^ 0x8000, 4, this->maxTurnRate, 1);
    }
    this->actor.shape.rot.y = this->actor.world.rot.y;
    if (this->stunTimer < 30) {
        if (this->stunTimer & 1) {
            this->actor.shape.rot.y += 0x7D0;
        } else {
            this->actor.shape.rot.y -= 0x7D0;
        }
    }
}

void EnSsh_UpdateYaw(EnSsh* this, PlayState* play) {
    if (this->stunTimer != 0) {
        EnSsh_Stunned(this, play);
    } else {
        EnSsh_Turn(this, play);
    }
}

void EnSsh_Bob(EnSsh* this, PlayState* play) {
    f32 bobVel = 0.5f;

    if ((play->state.frames & 8) != 0) {
        bobVel *= -1.0f;
    }
    Math_SmoothStepToF(&this->actor.velocity.y, bobVel, 0.4f, 1000.0f, 0.0f);
}

s32 EnSsh_IsCloseToLink(EnSsh* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    f32 yDist;

    if (this->stateFlags & SSH_STATE_GROUND_START) {
        return true;
    }
    if (this->unkTimer != 0) {
        return true;
    }
    if (this->swayTimer != 0) {
        return true;
    }
    if (this->animTimer != 0) {
        return true;
    }

    if (this->actor.xzDistToPlayer > 160.0f) {
        return false;
    }

    yDist = this->actor.world.pos.y - player->actor.world.pos.y;
    if (yDist < 0.0f || yDist > 400.0f) {
        return false;
    }

    if (player->actor.world.pos.y < this->actor.floorHeight) {
        return false;
    }
    return true;
}

s32 EnSsh_IsCloseToHome(EnSsh* this) {
    f32 vel = this->actor.velocity.y;
    f32 nextY = this->actor.world.pos.y + 2.0f * this->actor.velocity.y;

    if (nextY >= this->actor.home.pos.y) {
        return 1;
    }
    return 0;
}

s32 EnSsh_IsCloseToGround(EnSsh* this) {
    f32 vel = this->actor.velocity.y;
    f32 nextY = this->actor.world.pos.y + 2.0f * this->actor.velocity.y;

    if ((nextY - this->actor.floorHeight) <= this->floorHeightOffset) {
        return 1;
    }
    return 0;
}

void EnSsh_Sway(EnSsh* this) {
    Vec3f swayVecBase;
    Vec3f swayVec;
    f32 temp;
    s16 swayAngle;

    if (this->swayTimer != 0) {
        this->swayAngle += 0x640;
        this->swayTimer--;
        if (this->swayTimer == 0) {
            this->swayAngle = 0;
        }
        temp = this->swayTimer * (1.0f / 6);
        swayAngle = temp * (0x10000 / 360.0f) * Math_SinS(this->swayAngle);
        temp = this->actor.world.pos.y - this->ceilingPos.y;
        swayVecBase.x = Math_SinS(swayAngle) * temp;
        swayVecBase.y = Math_CosS(swayAngle) * temp;
        swayVecBase.z = 0.0f;
        Matrix_Push();
        Matrix_Translate(this->ceilingPos.x, this->ceilingPos.y, this->ceilingPos.z, MTXMODE_NEW);
        Matrix_RotateY(this->actor.world.rot.y * (M_PI / 0x8000), MTXMODE_APPLY);
        Matrix_MultVec3f(&swayVecBase, &swayVec);
        Matrix_Pop();
        this->actor.shape.rot.z = -(swayAngle * 2);
        this->actor.world.pos.x = swayVec.x;
        this->actor.world.pos.z = swayVec.z;
    }
}

void EnSsh_CheckBodyStickHit(EnSsh* this, PlayState* play) {
    ColliderInfo* info = &this->colCylinder[0].info;
    Player* player = GET_PLAYER(play);

    if (player->unk_860 != 0) {
        info->bumper.dmgFlags |= 2;
        this->colCylinder[1].info.bumper.dmgFlags &= ~2;
        this->colCylinder[2].info.bumper.dmgFlags &= ~2;
    } else {
        info->bumper.dmgFlags &= ~2;
        this->colCylinder[1].info.bumper.dmgFlags |= 2;
        this->colCylinder[2].info.bumper.dmgFlags |= 2;
    }
}

s32 EnSsh_CheckHitPlayer(EnSsh* this, PlayState* play) {
    s32 i;
    s32 hit = false;

    if ((this->hitCount == 0) && (this->spinTimer == 0)) {
        return false;
    }
    for (i = 0; i < 3; i++) {
        if (this->colCylinder[i + 3].base.ocFlags2 & OC2_HIT_PLAYER) {
            this->colCylinder[i + 3].base.ocFlags2 &= ~OC2_HIT_PLAYER;
            hit = true;
        }
    }
    if (!hit) {
        return false;
    }
    this->hitTimer = 30;
    if (this->swayTimer == 0) {
        this->spinTimer = this->hitTimer;
    }
    Audio_PlayActorSound2(&this->actor, NA_SE_EN_STALTU_ROLL);
    Audio_PlayActorSound2(&this->actor, NA_SE_VO_ST_ATTACK);
    play->damagePlayer(play, -8);
    func_8002F71C(play, &this->actor, 4.0f, this->actor.yawTowardsPlayer, 6.0f);
    this->hitCount--;
    return true;
}

s32 EnSsh_CheckHitFront(EnSsh* this) {
    u32 acFlags;

    if (this->colCylinder[2].base.acFlags) {} // Needed for matching
    acFlags = this->colCylinder[2].base.acFlags;

    if (!!(acFlags & AC_HIT) == 0) {
        return 0;
    } else {
        this->colCylinder[2].base.acFlags &= ~AC_HIT;
        this->invincibilityTimer = 8;
        if ((this->swayTimer == 0) && (this->hitTimer == 0) && (this->stunTimer == 0)) {
            this->swayTimer = 60;
        }
        return 1;
    }
}

s32 EnSsh_CheckHitBack(EnSsh* this, PlayState* play) {
    ColliderCylinder* cyl = &this->colCylinder[0];
    s32 hit = false;

    if (cyl->base.acFlags & AC_HIT) {
        cyl->base.acFlags &= ~AC_HIT;
        hit = true;
    }
    cyl = &this->colCylinder[1];
    if (cyl->base.acFlags & AC_HIT) {
        cyl->base.acFlags &= ~AC_HIT;
        hit = true;
    }
    if (!hit) {
        return false;
    }
    this->invincibilityTimer = 8;
    if (this->hitCount <= 0) {
        this->hitCount++;
    }
    if (this->stunTimer == 0) {
        Audio_PlayActorSound2(&this->actor, NA_SE_EN_GOMA_JR_FREEZE);
        Audio_PlayActorSound2(&this->actor, NA_SE_VO_ST_DAMAGE);
    }
    EnSsh_SetStunned(this);
    this->stateFlags |= SSH_STATE_STUNNED;
    return false;
}

s32 EnSsh_CollisionCheck(EnSsh* this, PlayState* play) {
    if (this->stunTimer == 0) {
        EnSsh_CheckHitPlayer(this, play);
    }
    if (EnSsh_CheckHitFront(this)) {
        return false;
    } else if (play->actorCtx.unk_02 != 0) {
        this->invincibilityTimer = 8;
        if (this->stunTimer == 0) {
            Audio_PlayActorSound2(&this->actor, NA_SE_EN_GOMA_JR_FREEZE);
            Audio_PlayActorSound2(&this->actor, NA_SE_VO_ST_DAMAGE);
        }
        EnSsh_SetStunned(this);
        this->stateFlags |= SSH_STATE_STUNNED;
        return false;
    } else {
        return EnSsh_CheckHitBack(this, play);
        // Always returns false
    }
}

void EnSsh_SetBodyCylinderAC(EnSsh* this, PlayState* play) {
    Collider_UpdateCylinder(&this->actor, &this->colCylinder[0]);
    CollisionCheck_SetAC(play, &play->colChkCtx, &this->colCylinder[0].base);
}

void EnSsh_SetLegsCylinderAC(EnSsh* this, PlayState* play) {
    s16 angleTowardsLink = ABS((s16)(this->actor.yawTowardsPlayer - this->actor.shape.rot.y));

    if (angleTowardsLink < 90 * (0x10000 / 360)) {
        Collider_UpdateCylinder(&this->actor, &this->colCylinder[2]);
        CollisionCheck_SetAC(play, &play->colChkCtx, &this->colCylinder[2].base);
    } else {
        Collider_UpdateCylinder(&this->actor, &this->colCylinder[1]);
        CollisionCheck_SetAC(play, &play->colChkCtx, &this->colCylinder[1].base);
    }
}

s32 EnSsh_SetCylinderOC(EnSsh* this, PlayState* play) {
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
        Matrix_RotateY((this->initialYaw / (f32)0x8000) * M_PI, MTXMODE_APPLY);
        Matrix_MultVec3f(&cyloffsets[i], &cylPos);
        Matrix_Pop();
        this->colCylinder[i + 3].dim.pos.x = cylPos.x;
        this->colCylinder[i + 3].dim.pos.y = cylPos.y;
        this->colCylinder[i + 3].dim.pos.z = cylPos.z;
        CollisionCheck_SetOC(play, &play->colChkCtx, &this->colCylinder[i + 3].base);
    }
    return 1;
}

void EnSsh_SetColliders(EnSsh* this, PlayState* play) {
    if (this->actor.colChkInfo.health == 0) {
        CollisionCheck_SetAT(play, &play->colChkCtx, &this->colSph.base);
        CollisionCheck_SetOC(play, &play->colChkCtx, &this->colSph.base);
    } else {
        if (this->hitTimer == 0) {
            EnSsh_SetCylinderOC(this, play);
        }
        if (DECR(this->invincibilityTimer) == 0) {
            EnSsh_SetBodyCylinderAC(this, play);
            EnSsh_SetLegsCylinderAC(this, play);
        }
    }
}

void EnSsh_Init(Actor* thisx, PlayState* play) {
    f32 frameCount;
    s32 pad;
    EnSsh* this = (EnSsh*)thisx;

    frameCount = Animation_GetLastFrame(&object_ssh_Anim_000304);
    if (this->actor.params == ENSSH_FATHER) {
        if (gSaveContext.inventory.gsTokens >= 100) {
            Actor_Kill(&this->actor);
            return;
        }
    } else if (gSaveContext.inventory.gsTokens >= (this->actor.params * 10)) {
        Actor_Kill(&this->actor);
        return;
    }
    ActorShape_Init(&this->actor.shape, 0.0f, ActorShadow_DrawCircle, 30.0f);
    SkelAnime_Init(play, &this->skelAnime, &object_ssh_Skel_0052E0, NULL, this->jointTable, this->morphTable, 30);
    Animation_Change(&this->skelAnime, &object_ssh_Anim_000304, 1.0f, 0.0f, frameCount, ANIMMODE_LOOP_INTERP, 0.0f);
    this->blureIdx = EnSsh_CreateBlureEffect(play);
    EnSsh_InitColliders(this, play);
    this->stateFlags = 0;
    this->hitCount = 0;
    EnSsh_CheckCeilingPos(this, play);
    if (this->actor.params != ENSSH_FATHER) {
        EnSsh_SetColliderScale(this, 0.5f, 1.0f);
    } else {
        EnSsh_SetColliderScale(this, 0.75f, 1.0f);
    }
    this->actor.gravity = 0.0f;
    this->initialYaw = this->actor.world.rot.y;
    EnSsh_SetupAction(this, EnSsh_Start);
}

void EnSsh_Destroy(Actor* thisx, PlayState* play) {
    s32 pad;
    EnSsh* this = (EnSsh*)thisx;
    s32 i;

    Effect_Delete(play, this->blureIdx);
    for (i = 0; i < 6; i++) {
        Collider_DestroyCylinder(play, &this->colCylinder[i]);
    }
    Collider_DestroyJntSph(play, &this->colSph);
}

void EnSsh_Wait(EnSsh* this, PlayState* play) {
    if (EnSsh_IsCloseToLink(this, play)) {
        EnSsh_SetDropAnimation(this);
        EnSsh_SetupAction(this, EnSsh_Drop);
    } else {
        EnSsh_Bob(this, play);
    }
}

void EnSsh_Talk(EnSsh* this, PlayState* play) {
    EnSsh_Bob(this, play);
    if (Actor_TextboxIsClosing(&this->actor, play)) {
        this->actionFunc = EnSsh_Idle;
    }
}

void EnSsh_Idle(EnSsh* this, PlayState* play) {
    if (Actor_ProcessTalkRequest(&this->actor, play)) {
        this->actionFunc = EnSsh_Talk;
        if (this->actor.params == ENSSH_FATHER) {
            gSaveContext.eventChkInf[9] |= 0x40;
        }
        if ((this->actor.textId == 0x26) || (this->actor.textId == 0x27)) {
            gSaveContext.infTable[25] |= 0x40;
        }
        if ((this->actor.textId == 0x24) || (this->actor.textId == 0x25)) {
            gSaveContext.infTable[25] |= 0x80;
        }
    } else {
        if ((this->unkTimer != 0) && (DECR(this->unkTimer) == 0)) {
            EnSsh_SetAnimation(this, SSH_ANIM_WAIT);
        }
        if ((this->animTimer != 0) && (DECR(this->animTimer) == 0)) {
            EnSsh_SetAnimation(this, SSH_ANIM_WAIT);
        }
        if (!EnSsh_IsCloseToLink(this, play)) {
            EnSsh_SetReturnAnimation(this);
            EnSsh_SetupAction(this, EnSsh_Return);
        } else {
            if (DECR(this->sfxTimer) == 0) {
                Audio_PlayActorSound2(&this->actor, NA_SE_EN_STALTU_LAUGH);
                this->sfxTimer = 64;
            }
            EnSsh_Bob(this, play);
            if ((this->unkTimer == 0) && (this->animTimer == 0)) {
                this->actor.textId = Text_GetFaceReaction(play, 0xD);
                if (this->actor.textId == 0) {
                    if (this->actor.params == ENSSH_FATHER) {
                        if (gSaveContext.inventory.gsTokens >= 50) {
                            this->actor.textId = 0x29;
                        } else if (gSaveContext.inventory.gsTokens >= 10) {
                            if (gSaveContext.infTable[25] & 0x80) {
                                this->actor.textId = 0x24;
                            } else {
                                this->actor.textId = 0x25;
                            }
                        } else {
                            if (gSaveContext.infTable[25] & 0x40) {
                                this->actor.textId = 0x27;
                            } else {
                                this->actor.textId = 0x26;
                            }
                        }
                    } else {
                        this->actor.textId = 0x22;
                    }
                }
                func_8002F2CC(&this->actor, play, 100.0f);
            }
        }
    }
}

void EnSsh_Land(EnSsh* this, PlayState* play) {
    if ((this->unkTimer != 0) && (DECR(this->unkTimer) == 0)) {
        EnSsh_SetAnimation(this, SSH_ANIM_WAIT);
    }
    if ((this->animTimer != 0) && (DECR(this->animTimer) == 0)) {
        EnSsh_SetAnimation(this, SSH_ANIM_WAIT);
    }
    if ((this->actor.floorHeight + this->floorHeightOffset) <= this->actor.world.pos.y) {
        EnSsh_SetupAction(this, EnSsh_Idle);
    } else {
        Math_SmoothStepToF(&this->actor.velocity.y, 2.0f, 0.6f, 1000.0f, 0.0f);
    }
}

void EnSsh_Drop(EnSsh* this, PlayState* play) {
    if ((this->unkTimer != 0) && (DECR(this->unkTimer) == 0)) {
        EnSsh_SetAnimation(this, SSH_ANIM_DROP);
    }
    if (!EnSsh_IsCloseToLink(this, play)) {
        EnSsh_SetReturnAnimation(this);
        EnSsh_SetupAction(this, EnSsh_Return);
    } else if (EnSsh_IsCloseToGround(this)) {
        EnSsh_SpawnShockwave(this, play);
        EnSsh_SetLandAnimation(this);
        EnSsh_SetupAction(this, EnSsh_Land);
    } else if (DECR(this->sfxTimer) == 0) {
        Audio_PlayActorSound2(&this->actor, NA_SE_EN_STALTU_DOWN);
        this->sfxTimer = 3;
    }
}

void EnSsh_Return(EnSsh* this, PlayState* play) {
    f32 frameRatio = this->skelAnime.curFrame / (this->skelAnime.animLength - 1.0f);

    if (frameRatio == 1.0f) {
        EnSsh_SetReturnAnimation(this);
    }
    if (EnSsh_IsCloseToLink(this, play)) {
        EnSsh_SetDropAnimation(this);
        EnSsh_SetupAction(this, EnSsh_Drop);
    } else if (EnSsh_IsCloseToHome(this)) {
        EnSsh_SetWaitAnimation(this);
        EnSsh_SetupAction(this, EnSsh_Wait);
    } else {
        this->actor.velocity.y = 4.0f * frameRatio;
    }
}

void EnSsh_UpdateColliderScale(EnSsh* this) {
    if (this->stateFlags & SSH_STATE_SPIN) {
        if (this->spinTimer == 0) {
            this->stateFlags &= ~SSH_STATE_SPIN;
            if (this->actor.params != ENSSH_FATHER) {
                EnSsh_SetColliderScale(this, 0.5f, 1.0f);
            } else {
                EnSsh_SetColliderScale(this, 0.75f, 1.0f);
            }
        }
    } else {
        if (this->spinTimer != 0) {
            this->stateFlags |= SSH_STATE_SPIN;
            if (this->actor.params != ENSSH_FATHER) {
                EnSsh_SetColliderScale(this, 0.5f, 2.0f);
            } else {
                EnSsh_SetColliderScale(this, 0.75f, 2.0f);
            }
        }
    }
}

void EnSsh_Start(EnSsh* this, PlayState* play) {
    if (!EnSsh_IsCloseToGround(this)) {
        EnSsh_SetupAction(this, EnSsh_Wait);
        EnSsh_Wait(this, play);
    } else {
        EnSsh_SetLandAnimation(this);
        this->stateFlags |= 4;
        EnSsh_SetupAction(this, EnSsh_Land);
        EnSsh_Land(this, play);
    }
}

void EnSsh_Update(Actor* thisx, PlayState* play) {
    s32 pad;
    EnSsh* this = (EnSsh*)thisx;

    EnSsh_UpdateColliderScale(this);
    if (EnSsh_CollisionCheck(this, play)) {
        return; // EnSsh_CollisionCheck always returns false, so this never happens
    }
    if (this->stunTimer != 0) {
        EnSsh_Damaged(this);
    } else {
        SkelAnime_Update(&this->skelAnime);
        func_8002D7EC(&this->actor);
        Actor_UpdateBgCheckInfo(play, &this->actor, 0.0f, 0.0f, 0.0f, 4);
        this->actionFunc(this, play);
    }
    EnSsh_UpdateYaw(this, play);
    if (DECR(this->blinkTimer) == 0) {
        this->blinkTimer = Rand_S16Offset(60, 60);
    }
    this->blinkState = this->blinkTimer;
    if (this->blinkState >= 3) {
        this->blinkState = 0;
    }
    EnSsh_SetColliders(this, play);
    Actor_SetFocus(&this->actor, 0.0f);
}

s32 EnSsh_OverrideLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, void* thisx) {
    EnSsh* this = (EnSsh*)thisx;

    switch (limbIndex) {
        case 1:
            if ((this->spinTimer != 0) && (this->swayTimer == 0)) {
                if (this->spinTimer >= 2) {
                    EnSsh_AddBlureVertex(this);
                } else {
                    EnSsh_AddBlureSpace(this);
                }
            }
            break;
        case 4:
            if (this->actor.params == ENSSH_FATHER) {
                *dList = object_ssh_DL_0046C0;
            }
            break;
        case 5:
            if (this->actor.params == ENSSH_FATHER) {
                *dList = object_ssh_DL_004080;
            }
            break;
        case 8:
            if (this->actor.params == ENSSH_FATHER) {
                *dList = object_ssh_DL_004DE8;
            }
            break;
    }
    return false;
}

void EnSsh_PostLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot, void* thisx) {
    EnSsh* this = (EnSsh*)thisx;

    Collider_UpdateSpheres(limbIndex, &this->colSph);
}

void EnSsh_Draw(Actor* thisx, PlayState* play) {
    static void* blinkTex[] = {
        object_ssh_Tex_0007E0,
        object_ssh_Tex_000C60,
        object_ssh_Tex_001060,
    };
    s32 pad;
    EnSsh* this = (EnSsh*)thisx;

    EnSsh_CheckBodyStickHit(this, play);
    EnSsh_Sway(this);
    OPEN_DISPS(play->state.gfxCtx);
    gSPSegment(POLY_OPA_DISP++, 0x08, SEGMENTED_TO_VIRTUAL(blinkTex[this->blinkState]));
    CLOSE_DISPS(play->state.gfxCtx);
    SkelAnime_DrawOpa(play, this->skelAnime.skeleton, this->skelAnime.jointTable, EnSsh_OverrideLimbDraw,
                      EnSsh_PostLimbDraw, &this->actor);
}
