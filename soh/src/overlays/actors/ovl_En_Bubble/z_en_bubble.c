#include "z_en_bubble.h"
#include "objects/object_bubble/object_bubble.h"

#define FLAGS ACTOR_FLAG_0

void EnBubble_Init(Actor* thisx, PlayState* play);
void EnBubble_Destroy(Actor* thisx, PlayState* play);
void EnBubble_Update(Actor* thisx, PlayState* play);
void EnBubble_Draw(Actor* thisx, PlayState* play);

void EnBubble_Wait(EnBubble* this, PlayState* play);
void EnBubble_Pop(EnBubble* this, PlayState* play);
void EnBubble_Regrow(EnBubble* this, PlayState* play);

const ActorInit En_Bubble_InitVars = {
    ACTOR_EN_BUBBLE,
    ACTORCAT_ENEMY,
    FLAGS,
    OBJECT_BUBBLE,
    sizeof(EnBubble),
    (ActorFunc)EnBubble_Init,
    (ActorFunc)EnBubble_Destroy,
    (ActorFunc)EnBubble_Update,
    (ActorFunc)EnBubble_Draw,
    NULL,
};

static ColliderJntSphElementInit sJntSphElementsInit[2] = {
    {
        {
            ELEMTYPE_UNK0,
            { 0x00000000, 0x00, 0x04 },
            { 0xFFCFD753, 0x00, 0x00 },
            TOUCH_NONE,
            BUMP_ON,
            OCELEM_ON,
        },
        { 0, { { 0, 0, 0 }, 16 }, 100 },
    },
    {
        {
            ELEMTYPE_UNK0,
            { 0x00000000, 0x00, 0x00 },
            { 0x00002824, 0x00, 0x00 },
            TOUCH_NONE,
            BUMP_ON | BUMP_NO_AT_INFO | BUMP_NO_DAMAGE | BUMP_NO_SWORD_SFX | BUMP_NO_HITMARK,
            OCELEM_NONE,
        },
        { 0, { { 0, 0, 0 }, 16 }, 100 },
    },
};

static ColliderJntSphInit sJntSphInit = {
    {
        COLTYPE_HIT6,
        AT_ON | AT_TYPE_ENEMY,
        AC_ON | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_1,
        COLSHAPE_JNTSPH,
    },
    2,
    sJntSphElementsInit,
};

static CollisionCheckInfoInit2 sColChkInfoInit2 = {
    1, 2, 25, 25, MASS_IMMOVABLE,
};

static Vec3f sEffectAccel = { 0.0f, -0.5f, 0.0f };

static Color_RGBA8 sEffectPrimColor = { 255, 255, 255, 255 };

static Color_RGBA8 sEffectEnvColor = { 150, 150, 150, 0 };

void EnBubble_SetDimensions(EnBubble* this, f32 dim) {
    f32 a;
    f32 b;
    f32 c;
    f32 d;

    this->actor.flags |= ACTOR_FLAG_0;
    Actor_SetScale(&this->actor, 1.0f);
    this->actor.shape.yOffset = 16.0f;
    this->graphicRotSpeed = 16.0f;
    this->graphicEccentricity = 0.08f;
    this->expansionWidth = dim;
    this->expansionHeight = dim;
    a = Rand_ZeroOne();
    b = Rand_ZeroOne();
    c = Rand_ZeroOne();
    this->unk_218 = 1.0f;
    this->unk_21C = 1.0f;
    d = (a * a) + (b * b) + (c * c);
    this->unk_1FC.x = a / d;
    this->unk_1FC.y = b / d;
    this->unk_1FC.z = c / d;
}

u32 func_809CBCBC(EnBubble* this) {
    ColliderInfo* info = &this->colliderSphere.elements[0].info;

    info->toucher.dmgFlags = 0x8;
    info->toucher.effect = 0;
    info->toucher.damage = 4;
    info->toucherFlags = TOUCH_ON;
    this->actor.velocity.y = 0.0f;
    return 6;
}

// only called in an unused actionFunc
u32 func_809CBCEC(EnBubble* this) {
    EnBubble_SetDimensions(this, -1.0f);
    return 12;
}

void EnBubble_DamagePlayer(EnBubble* this, PlayState* play) {
    s32 damage = -this->colliderSphere.elements[0].info.toucher.damage;

    play->damagePlayer(play, damage);
    func_8002F7A0(play, &this->actor, 6.0f, this->actor.yawTowardsPlayer, 6.0f);
}

s32 EnBubble_Explosion(EnBubble* this, PlayState* play) {
    u32 i;
    Vec3f effectAccel;
    Vec3f effectVel;
    Vec3f effectPos;

    effectAccel = sEffectAccel;
    Math_SmoothStepToF(&this->expansionWidth, 4.0f, 0.1f, 1000.0f, 0.0f);
    Math_SmoothStepToF(&this->expansionHeight, 4.0f, 0.1f, 1000.0f, 0.0f);
    Math_SmoothStepToF(&this->graphicRotSpeed, 54.0f, 0.1f, 1000.0f, 0.0f);
    Math_SmoothStepToF(&this->graphicEccentricity, 0.2f, 0.1f, 1000.0f, 0.0f);
    this->actor.shape.yOffset = ((this->expansionHeight + 1.0f) * 16.0f);

    if (DECR(this->explosionCountdown) != 0) {
        return -1;
    }
    effectPos.x = this->actor.world.pos.x;
    effectPos.y = this->actor.world.pos.y + this->actor.shape.yOffset;
    effectPos.z = this->actor.world.pos.z;
    for (i = 0; i < 20; i++) {
        effectVel.x = (Rand_ZeroOne() - 0.5f) * 7.0f;
        effectVel.y = Rand_ZeroOne() * 7.0f;
        effectVel.z = (Rand_ZeroOne() - 0.5f) * 7.0f;
        EffectSsDtBubble_SpawnCustomColor(play, &effectPos, &effectVel, &effectAccel, &sEffectPrimColor,
                                          &sEffectEnvColor, Rand_S16Offset(100, 50), 0x19, 0);
    }
    Item_DropCollectibleRandom(play, NULL, &this->actor.world.pos, 0x50);
    this->actor.flags &= ~ACTOR_FLAG_0;
    return Rand_S16Offset(90, 60);
}

// only called in an unused actionFunc
u32 func_809CBFD4(EnBubble* this) {
    if (DECR(this->explosionCountdown) != 0) {
        return -1;
    }
    return func_809CBCEC(this);
}

// only called in an unused actionFunc
s32 func_809CC020(EnBubble* this) {
    this->expansionWidth += 1.0f / 12.0f;
    this->expansionHeight += 1.0f / 12.0f;

    if (DECR(this->explosionCountdown) != 0) {
        return false;
    }
    return true;
}

void EnBubble_Vec3fNormalizedRelfect(Vec3f* vec1, Vec3f* vec2, Vec3f* ret) {
    f32 norm;

    Math3D_Vec3fReflect(vec1, vec2, ret);
    norm = sqrtf((ret->x * ret->x) + (ret->y * ret->y) + (ret->z * ret->z));
    if (norm != 0.0f) {
        ret->x /= norm;
        ret->y /= norm;
        ret->z /= norm;
    } else {
        ret->x = ret->y = ret->z = 0.0f;
    }
}

void EnBubble_Vec3fNormalize(Vec3f* vec) {
    f32 norm = sqrt((vec->x * vec->x) + (vec->y * vec->y) + (vec->z * vec->z));

    if (norm != 0.0f) {
        vec->x /= norm;
        vec->y /= norm;
        vec->z /= norm;
    } else {
        vec->x = vec->y = vec->z = 0.0f;
    }
}

void EnBubble_Fly(EnBubble* this, PlayState* play) {
    CollisionPoly* sp94;
    Actor* bumpActor;
    Vec3f sp84;
    Vec3f sp78;
    Vec3f sp6C;
    Vec3f sp60;
    Vec3f sp54;
    f32 bounceSpeed;
    s32 bgId;
    u8 bounceCount;

    if (this->colliderSphere.elements[1].info.bumperFlags & BUMP_HIT) {
        bumpActor = this->colliderSphere.base.ac;
        this->normalizedBumpVelocity = bumpActor->velocity;
        EnBubble_Vec3fNormalize(&this->normalizedBumpVelocity);
        this->velocityFromBump.x += (this->normalizedBumpVelocity.x * 3.0f);
        this->velocityFromBump.y += (this->normalizedBumpVelocity.y * 3.0f);
        this->velocityFromBump.z += (this->normalizedBumpVelocity.z * 3.0f);
    }
    this->sinkSpeed -= 0.1f;
    if (this->sinkSpeed < this->actor.minVelocityY) {
        this->sinkSpeed = this->actor.minVelocityY;
    }
    sp54.x = this->velocityFromBounce.x + this->velocityFromBump.x;
    sp54.y = this->velocityFromBounce.y + this->velocityFromBump.y + this->sinkSpeed;
    sp54.z = this->velocityFromBounce.z + this->velocityFromBump.z;
    EnBubble_Vec3fNormalize(&sp54);

    sp78.x = this->actor.world.pos.x;
    sp78.y = this->actor.world.pos.y + this->actor.shape.yOffset;
    sp78.z = this->actor.world.pos.z;
    sp6C = sp78;

    sp6C.x += (sp54.x * 24.0f);
    sp6C.y += (sp54.y * 24.0f);
    sp6C.z += (sp54.z * 24.0f);
    if (BgCheck_EntityLineTest1(&play->colCtx, &sp78, &sp6C, &sp84, &sp94, true, true, true, false, &bgId)) {
        sp60.x = COLPOLY_GET_NORMAL(sp94->normal.x);
        sp60.y = COLPOLY_GET_NORMAL(sp94->normal.y);
        sp60.z = COLPOLY_GET_NORMAL(sp94->normal.z);
        EnBubble_Vec3fNormalizedRelfect(&sp54, &sp60, &sp54);
        this->bounceDirection = sp54;
        bounceCount = this->bounceCount;
        this->bounceCount = ++bounceCount;
        if (bounceCount > (s16)(Rand_ZeroOne() * 10.0f)) {
            this->bounceCount = 0;
        }
        bounceSpeed = (this->bounceCount == 0) ? 3.6000001f : 3.0f;
        this->velocityFromBump.x = this->velocityFromBump.y = this->velocityFromBump.z = 0.0f;
        this->velocityFromBounce.x = (this->bounceDirection.x * bounceSpeed);
        this->velocityFromBounce.y = (this->bounceDirection.y * bounceSpeed);
        this->velocityFromBounce.z = (this->bounceDirection.z * bounceSpeed);
        this->sinkSpeed = 0.0f;
        Audio_PlayActorSound2(&this->actor, NA_SE_EN_AWA_BOUND);
        this->graphicRotSpeed = 128.0f;
        this->graphicEccentricity = 0.48f;
    } else if (this->actor.bgCheckFlags & 0x20 && sp54.y < 0.0f) {
        sp60.x = sp60.z = 0.0f;
        sp60.y = 1.0f;
        EnBubble_Vec3fNormalizedRelfect(&sp54, &sp60, &sp54);
        this->bounceDirection = sp54;
        bounceCount = this->bounceCount;
        this->bounceCount = ++bounceCount;
        if (bounceCount > (s16)(Rand_ZeroOne() * 10.0f)) {
            this->bounceCount = 0;
        }
        bounceSpeed = (this->bounceCount == 0) ? 3.6000001f : 3.0f;
        this->velocityFromBump.x = this->velocityFromBump.y = this->velocityFromBump.z = 0.0f;
        this->velocityFromBounce.x = (this->bounceDirection.x * bounceSpeed);
        this->velocityFromBounce.y = (this->bounceDirection.y * bounceSpeed);
        this->velocityFromBounce.z = (this->bounceDirection.z * bounceSpeed);
        this->sinkSpeed = 0.0f;
        Audio_PlayActorSound2(&this->actor, NA_SE_EN_AWA_BOUND);
        this->graphicRotSpeed = 128.0f;
        this->graphicEccentricity = 0.48f;
    }
    this->actor.velocity.x = this->velocityFromBounce.x + this->velocityFromBump.x;
    this->actor.velocity.y = this->velocityFromBounce.y + this->velocityFromBump.y + this->sinkSpeed;
    this->actor.velocity.z = this->velocityFromBounce.z + this->velocityFromBump.z;
    Math_ApproachF(&this->velocityFromBump.x, 0.0f, 0.3f, 0.1f);
    Math_ApproachF(&this->velocityFromBump.y, 0.0f, 0.3f, 0.1f);
    Math_ApproachF(&this->velocityFromBump.z, 0.0f, 0.3f, 0.1f);
}

u32 func_809CC648(EnBubble* this) {
    if (((this->colliderSphere.base.acFlags & AC_HIT) != 0) == false) {
        return false;
    }
    this->colliderSphere.base.acFlags &= ~AC_HIT;
    if (this->colliderSphere.elements[1].info.bumperFlags & BUMP_HIT) {
        this->unk_1F0.x = this->colliderSphere.base.ac->velocity.x / 10.0f;
        this->unk_1F0.y = this->colliderSphere.base.ac->velocity.y / 10.0f;
        this->unk_1F0.z = this->colliderSphere.base.ac->velocity.z / 10.0f;
        this->graphicRotSpeed = 128.0f;
        this->graphicEccentricity = 0.48f;
        return false;
    }
    this->unk_208 = 8;
    return true;
}

u32 EnBubble_DetectPop(EnBubble* this, PlayState* play) {
    if (DECR(this->unk_208) != 0 || this->actionFunc == EnBubble_Pop) {
        return false;
    }
    if (this->colliderSphere.base.ocFlags2 & OC2_HIT_PLAYER) {
        this->colliderSphere.base.ocFlags2 &= ~OC2_HIT_PLAYER;
        EnBubble_DamagePlayer(this, play);
        this->unk_208 = 8;
        return true;
    }
    return func_809CC648(this);
}

void func_809CC774(EnBubble* this) {
    ColliderJntSphElementDim* dim;
    Vec3f src;
    Vec3f dest;

    dim = &this->colliderSphere.elements[0].dim;
    src.x = dim->modelSphere.center.x;
    src.y = dim->modelSphere.center.y;
    src.z = dim->modelSphere.center.z;

    Matrix_MultVec3f(&src, &dest);
    dim->worldSphere.center.x = dest.x;
    dim->worldSphere.center.y = dest.y;
    dim->worldSphere.center.z = dest.z;
    dim->worldSphere.radius = dim->modelSphere.radius * (1.0f + this->expansionWidth);
    this->colliderSphere.elements[1].dim = *dim;
}

void EnBubble_Init(Actor* thisx, PlayState* play) {
    EnBubble* this = (EnBubble*)thisx;
    u32 pad;

    ActorShape_Init(&this->actor.shape, 16.0f, ActorShadow_DrawCircle, 0.2f);
    Collider_InitJntSph(play, &this->colliderSphere);
    Collider_SetJntSph(play, &this->colliderSphere, &this->actor, &sJntSphInit, this->colliderSphereItems);
    CollisionCheck_SetInfo2(&this->actor.colChkInfo, DamageTable_Get(9), &sColChkInfoInit2);
    this->actor.naviEnemyId = 0x16;
    this->bounceDirection.x = Rand_ZeroOne();
    this->bounceDirection.y = Rand_ZeroOne();
    this->bounceDirection.z = Rand_ZeroOne();
    EnBubble_Vec3fNormalize(&this->bounceDirection);
    this->velocityFromBounce.x = this->bounceDirection.x * 3.0f;
    this->velocityFromBounce.y = this->bounceDirection.y * 3.0f;
    this->velocityFromBounce.z = this->bounceDirection.z * 3.0f;
    EnBubble_SetDimensions(this, 0.0f);
    this->actionFunc = EnBubble_Wait;
}

void EnBubble_Destroy(Actor* thisx, PlayState* play) {
    EnBubble* this = (EnBubble*)thisx;

    Collider_DestroyJntSph(play, &this->colliderSphere);
}

void EnBubble_Wait(EnBubble* this, PlayState* play) {
    if (EnBubble_DetectPop(this, play)) {
        this->explosionCountdown = func_809CBCBC(this);
        this->actionFunc = EnBubble_Pop;
    } else {
        EnBubble_Fly(this, play);
        this->actor.shape.yOffset = ((this->expansionHeight + 1.0f) * 16.0f);
        CollisionCheck_SetAC(play, &play->colChkCtx, &this->colliderSphere.base);
        CollisionCheck_SetOC(play, &play->colChkCtx, &this->colliderSphere.base);
    }
}

void EnBubble_Pop(EnBubble* this, PlayState* play) {
    if (EnBubble_Explosion(this, play) >= 0) {
        SoundSource_PlaySfxAtFixedWorldPos(play, &this->actor.world.pos, 60, NA_SE_EN_AWA_BREAK);
        Actor_Kill(&this->actor);
        gSaveContext.sohStats.count[COUNT_ENEMIES_DEFEATED_SHABOM]++;
    }
}

// unused
void EnBubble_Disappear(EnBubble* this, PlayState* play) {
    s32 temp_v0;

    temp_v0 = func_809CBFD4(this);
    if (temp_v0 >= 0) {
        this->actor.shape.shadowDraw = ActorShadow_DrawCircle;
        this->explosionCountdown = temp_v0;
        this->actionFunc = EnBubble_Regrow;
    }
}

// unused
void EnBubble_Regrow(EnBubble* this, PlayState* play) {
    if (func_809CC020(this)) {
        this->actionFunc = EnBubble_Wait;
    }
    CollisionCheck_SetAC(play, &play->colChkCtx, &this->colliderSphere.base);
    CollisionCheck_SetOC(play, &play->colChkCtx, &this->colliderSphere.base);
}

void EnBubble_Update(Actor* thisx, PlayState* play) {
    EnBubble* this = (EnBubble*)thisx;

    func_8002D7EC(&this->actor);
    Actor_UpdateBgCheckInfo(play, &this->actor, 16.0f, 16.0f, 0.0f, 7);
    this->actionFunc(this, play);
    Actor_SetFocus(&this->actor, this->actor.shape.yOffset);
}

void EnBubble_Draw(Actor* thisx, PlayState* play) {
    EnBubble* this = (EnBubble*)thisx;
    u32 pad;

    OPEN_DISPS(play->state.gfxCtx);

    if (this->actionFunc != EnBubble_Disappear) {
        func_80093D84(play->state.gfxCtx);
        Math_SmoothStepToF(&this->graphicRotSpeed, 16.0f, 0.2f, 1000.0f, 0.0f);
        Math_SmoothStepToF(&this->graphicEccentricity, 0.08f, 0.2f, 1000.0f, 0.0f);
        Matrix_ReplaceRotation(&play->billboardMtxF);

        Matrix_Scale(this->expansionWidth + 1.0f, this->expansionHeight + 1.0f, 1.0f, MTXMODE_APPLY);
        Matrix_RotateZ(((f32)play->state.frames * (M_PI / 180.0f)) * this->graphicRotSpeed, MTXMODE_APPLY);
        Matrix_Scale(this->graphicEccentricity + 1.0f, 1.0f, 1.0f, MTXMODE_APPLY);
        Matrix_RotateZ((-(f32)play->state.frames * (M_PI / 180.0f)) * this->graphicRotSpeed, MTXMODE_APPLY);

        gSPMatrix(POLY_XLU_DISP++, MATRIX_NEWMTX(play->state.gfxCtx),
                  G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
        gSPDisplayList(POLY_XLU_DISP++, gBubbleDL);
    }

    CLOSE_DISPS(play->state.gfxCtx);

    if (this->actionFunc != EnBubble_Disappear) {
        this->actor.shape.shadowScale = (f32)((this->expansionWidth + 1.0f) * 0.2f);
        func_809CC774(this);
    }
}
