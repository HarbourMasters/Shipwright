/*
 * File: z_en_honotrap.c
 * Overlay: ovl_En_Honotrap
 * Description: Eye switch that shoots fire
 */

#include "z_en_honotrap.h"
#include "assets/objects/gameplay_keep/gameplay_keep.h"
#include "assets/objects/gameplay_dangeon_keep/gameplay_dangeon_keep.h"

#define FLAGS (ACTOR_FLAG_UPDATE_CULLING_DISABLED)

#define HONOTRAP_AT_ACTIVE (1 << 0)
#define HONOTRAP_AC_ACTIVE (1 << 1)
#define HONOTRAP_OC_ACTIVE (1 << 2)

typedef enum {
    /* 0 */ HONOTRAP_EYE_OPEN,
    /* 1 */ HONOTRAP_EYE_HALF,
    /* 2 */ HONOTRAP_EYE_CLOSE,
    /* 3 */ HONOTRAP_EYE_SHUT,
    /* 4 */ HONOTRAP_EYE_MAX
} EnHonotrapEyeState;

void EnHonotrap_Init(Actor* thisx, PlayState* play);
void EnHonotrap_Destroy(Actor* thisx, PlayState* play);
void EnHonotrap_Update(Actor* thisx, PlayState* play);
void EnHonotrap_Draw(Actor* thisx, PlayState* play);

void EnHonotrap_GetNormal(Vec3f* normal, Vec3f* vec);
void EnHonotrap_InitEye(EnHonotrap* this, PlayState* play);
void EnHonotrap_InitFlame(EnHonotrap* this, PlayState* play);
void EnHonotrap_InitFlameGroup(EnHonotrap* this, PlayState* play);
void EnHonotrap_SetupEyeIdle(EnHonotrap* this);
void EnHonotrap_EyeIdle(EnHonotrap* this, PlayState* play);
void EnHonotrap_SetupEyeOpen(EnHonotrap* this);
void EnHonotrap_EyeOpen(EnHonotrap* this, PlayState* play);
void EnHonotrap_SetupEyeAttack(EnHonotrap* this);
void EnHonotrap_EyeAttack(EnHonotrap* this, PlayState* play);
void EnHonotrap_SetupEyeClose(EnHonotrap* this);
void EnHonotrap_EyeClose(EnHonotrap* this, PlayState* play);
void EnHonotrap_SetupEyeIdle2(EnHonotrap* this);
void EnHonotrap_EyeIdle2(EnHonotrap* this, PlayState* play);
void EnHonotrap_SetupEyeOpen2(EnHonotrap* this);
void EnHonotrap_EyeOpen2(EnHonotrap* this, PlayState* play);
void EnHonotrap_SetupEyeAttack2(EnHonotrap* this);
void EnHonotrap_EyeAttack2(EnHonotrap* this, PlayState* play);
void EnHonotrap_SetupEyeClose2(EnHonotrap* this);
void EnHonotrap_EyeClose2(EnHonotrap* this, PlayState* play);
void EnHonotrap_SetupFlameGrow(EnHonotrap* this);
void EnHonotrap_FlameGrow(EnHonotrap* this, PlayState* play);
void EnHonotrap_SetupFlameDrop(EnHonotrap* this);
void EnHonotrap_FlameDrop(EnHonotrap* this, PlayState* play);
void EnHonotrap_SetupFlameMove(EnHonotrap* this);
void EnHonotrap_FlameMove(EnHonotrap* this, PlayState* play);
void EnHonotrap_SetupFlameChase(EnHonotrap* this);
void EnHonotrap_FlameChase(EnHonotrap* this, PlayState* play);
void EnHonotrap_SetupFlameVanish(EnHonotrap* this);
void EnHonotrap_FlameVanish(EnHonotrap* this, PlayState* play);
void EnHonotrap_SetupFlameGroup(EnHonotrap* this);
void EnHonotrap_FlameGroup(EnHonotrap* this, PlayState* play);
void EnHonotrap_UpdateFlame(Actor* thisx, PlayState* play);
void EnHonotrap_UpdateFlameGroup(Actor* thisx, PlayState* play);
void EnHonotrap_DrawFlame(Actor* thisx, PlayState* play);
void EnHonotrap_DrawFlameGroup(Actor* thisx, PlayState* play);

static TexturePtr sSilverEyeTextures[HONOTRAP_EYE_MAX] = {
    gEyeSwitchSilverOpenTex,
    gEyeSwitchSilverHalfTex,
    gEyeSwitchSilverClosedTex,
    gEyeSwitchSilverClosedTex,
};

static s32 sTexturesNotDesegmented = true;

ActorProfile En_Honotrap_Profile = {
    /**/ ACTOR_EN_HONOTRAP,
    /**/ ACTORCAT_PROP,
    /**/ FLAGS,
    /**/ GAMEPLAY_DANGEON_KEEP,
    /**/ sizeof(EnHonotrap),
    /**/ EnHonotrap_Init,
    /**/ EnHonotrap_Destroy,
    /**/ EnHonotrap_Update,
    /**/ EnHonotrap_Draw,
};

static ColliderTrisElementInit sTrisElementsInit[] = {
    {
        {
            ELEM_MATERIAL_UNK4,
            { 0x00000000, 0x00, 0x00 },
            { 0x00003820, 0x00, 0x00 },
            ATELEM_NONE | ATELEM_SFX_NORMAL,
            ACELEM_ON,
            OCELEM_NONE,
        },
        { { { 0.0f, 23.0f, 8.5f }, { -23.0f, 0.0f, 8.5f }, { 0.0f, -23.0f, 8.5f } } },
    },
    {
        {
            ELEM_MATERIAL_UNK4,
            { 0x00000000, 0x00, 0x00 },
            { 0x00003820, 0x00, 0x00 },
            ATELEM_NONE | ATELEM_SFX_NORMAL,
            ACELEM_ON,
            OCELEM_NONE,
        },
        { { { 0.0f, 23.0f, 8.5f }, { 0.0f, -23.0f, 8.5f }, { 23.0f, 0.0f, 8.5f } } },
    },
};

static ColliderTrisInit sTrisInit = {
    {
        COL_MATERIAL_NONE,
        AT_NONE,
        AC_ON | AC_TYPE_PLAYER,
        OC1_NONE,
        OC2_NONE,
        COLSHAPE_TRIS,
    },
    ARRAY_COUNT(sTrisElementsInit),
    sTrisElementsInit,
};

static ColliderCylinderInit sCylinderInit = {
    {
        COL_MATERIAL_NONE,
        AT_ON | AT_TYPE_ENEMY,
        AC_ON | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_2,
        COLSHAPE_CYLINDER,
    },
    {
        ELEM_MATERIAL_UNK0,
        { 0xF7CFFFFF, 0x01, 0x04 },
        { 0x00100000, 0x00, 0x00 },
        ATELEM_ON | ATELEM_SFX_NONE,
        ACELEM_ON,
        OCELEM_ON,
    },
    { 10, 25, 0, { 0, 0, 0 } },
};

static CollisionCheckInfoInit sColChkInfoInit = { 0, 9, 23, 1 };

static InitChainEntry sInitChain[] = {
    ICHAIN_F32(cullingVolumeDistance, 4000, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeScale, 100, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeDownward, 100, ICHAIN_STOP),
};

void EnHonotrap_FlameCollisionCheck(EnHonotrap* this, PlayState* play) {
    s32 pad[3];

    Collider_UpdateCylinder(&this->actor, &this->collider.cyl);
    CollisionCheck_SetAT(play, &play->colChkCtx, &this->collider.tris.base);
    CollisionCheck_SetAC(play, &play->colChkCtx, &this->collider.tris.base);
    CollisionCheck_SetOC(play, &play->colChkCtx, &this->collider.tris.base);
    this->colChkFlags |= HONOTRAP_AT_ACTIVE;
    this->colChkFlags |= HONOTRAP_AC_ACTIVE;
    this->colChkFlags |= HONOTRAP_OC_ACTIVE;
}

void EnHonotrap_GetNormal(Vec3f* normal, Vec3f* vec) {
    f32 magnitude = Math3D_Vec3fMagnitude(vec);

    if (magnitude < 0.001f) {
        normal->x = normal->y = 0.0f;
        normal->z = 1.0f;
    } else {
        normal->x = vec->x * (1.0f / magnitude);
        normal->y = vec->y * (1.0f / magnitude);
        normal->z = vec->z * (1.0f / magnitude);
    }
}

void EnHonotrap_InitEye(EnHonotrap* this, PlayState* play) {
    s32 i;
    s32 j;
    s32 k;
    f32 sin = Math_SinS(this->actor.home.rot.y);
    f32 cos = Math_CosS(this->actor.home.rot.y);
    Vec3f triangle[3];
    Vec3f* vtx;

    Actor_SetScale(&this->actor, 0.1f);

    if (sTexturesNotDesegmented) {
        sTexturesNotDesegmented = false;
        for (k = 0; k < HONOTRAP_EYE_MAX; k++) {
            sSilverEyeTextures[k] = Lib_SegmentedToVirtual(sSilverEyeTextures[k]);
        }
    }

    Collider_InitTris(play, &this->collider.tris);
    Collider_SetTris(play, &this->collider.tris, &this->actor, &sTrisInit, this->collider.elements);

    for (i = 0; i < 2; i++) {
        for (j = 0, vtx = triangle; j < 3; j++, vtx++) {
            Vec3f* baseVtx = &sTrisInit.elements[i].dim.vtx[j];

            vtx->x = (baseVtx->z * sin) + (baseVtx->x * cos);
            vtx->y = baseVtx->y;
            vtx->z = (baseVtx->z * cos) - (baseVtx->x * sin);

            Math_Vec3f_Sum(vtx, &this->actor.world.pos, vtx);
        }
        Collider_SetTrisVertices(&this->collider.tris, i, &triangle[0], &triangle[1], &triangle[2]);
    }
    Actor_SetFocus(&this->actor, 0.0f);
    if (this->actor.params == HONOTRAP_TYPE_EYE) {
        EnHonotrap_SetupEyeIdle(this);
    } else { // HONOTRAP_TYPE_EYE_MUTI_FLAME
        EnHonotrap_SetupEyeIdle2(this);
    }
}

void EnHonotrap_InitFlame(EnHonotrap* this, PlayState* play) {
    s32 pad[2];
    s32 params = this->actor.params;

    Actor_SetScale(&this->actor, 0.0001f);
    Collider_InitCylinder(play, &this->collider.cyl);
    Collider_SetCylinder(play, &this->collider.cyl, &this->actor, &sCylinderInit);
    Collider_UpdateCylinder(&this->actor, &this->collider.cyl);
    this->actor.terminalVelocity = -1.0f;
    CollisionCheck_SetInfo(&this->actor.colChkInfo, NULL, &sColChkInfoInit);
    ActorShape_Init(&this->actor.shape, 0.0f, ActorShadow_DrawCircle, 30.0f);
    this->actor.shape.shadowAlpha = 128;
    this->targetPos = GET_PLAYER(play)->actor.world.pos;
    this->targetPos.y += 10.0f;
    this->flameScroll = Rand_ZeroOne() * 511.0f;
    Actor_PlaySfx(&this->actor, NA_SE_EV_FLAME_IGNITION);
    if (params == HONOTRAP_TYPE_FLAME_DROP) {
        this->actor.room = -1;
        this->collider.cyl.dim.radius = 12;
        this->collider.cyl.dim.height = 30;
        this->actor.shape.yOffset = -1000.0f;
    }
    EnHonotrap_SetupFlameGrow(this);
}

void EnHonotrap_InitFlameGroup(EnHonotrap* this, PlayState* play) {
    EnHonotrapFlameGroup* flameGroup;
    s32 flameScroll;
    s32 i;

    Actor_SetScale(&this->actor, 0.0001f);
    Collider_InitCylinder(play, &this->collider.cyl);
    Collider_SetCylinder(play, &this->collider.cyl, &this->actor, &sCylinderInit);
    CollisionCheck_SetInfo(&this->actor.colChkInfo, NULL, &sColChkInfoInit);

    flameScroll = Rand_ZeroOne() * 511.0f;
    flameGroup = &this->flameGroup;
    for (i = 0; i < ARRAY_COUNT(flameGroup->flameList); i++) {
        flameGroup->flameList[i].flameScroll = flameScroll;
        flameScroll += (s32)Rand_ZeroFloat(300.0f) + 100;
        flameScroll %= 0x200U;
    }

    Actor_PlaySfx(&this->actor, NA_SE_EV_FLAME_IGNITION);
    EnHonotrap_SetupFlameGroup(this);
}

void EnHonotrap_Init(Actor* thisx, PlayState* play) {
    s32 params;
    EnHonotrap* this = (EnHonotrap*)thisx;

    params = this->actor.params;
    Actor_ProcessInitChain(&this->actor, sInitChain);
    if ((params == HONOTRAP_TYPE_EYE) || (params == HONOTRAP_TYPE_EYE_MUTI_FLAME)) {
        EnHonotrap_InitEye(this, play);
    } else if (params == HONOTRAP_TYPE_FLAME_GROUP) {
        EnHonotrap_InitFlameGroup(this, play);
        this->actor.update = EnHonotrap_UpdateFlameGroup;
        this->actor.draw = EnHonotrap_DrawFlameGroup;
        this->actor.cullingVolumeScale = 500.0f;
        this->actor.cullingVolumeDownward = 500.0f;
    } else { // HONOTRAP_TYPE_FLAME_MOVE, HONOTRAP_TYPE_FLAME_DROP
        EnHonotrap_InitFlame(this, play);
        this->actor.update = EnHonotrap_UpdateFlame;
        this->actor.draw = EnHonotrap_DrawFlame;
    }
}

void EnHonotrap_Destroy(Actor* thisx, PlayState* play) {
    s16 params;
    EnHonotrap* this = (EnHonotrap*)thisx;

    params = this->actor.params;
    if ((params == HONOTRAP_TYPE_EYE) || (params == HONOTRAP_TYPE_EYE_MUTI_FLAME)) {
        Collider_DestroyTris(play, &this->collider.tris);
    } else {
        Collider_DestroyCylinder(play, &this->collider.cyl);
    }
}

void EnHonotrap_SetupEyeIdle(EnHonotrap* this) {
    this->actionFunc = EnHonotrap_EyeIdle;
    this->eyeState = HONOTRAP_EYE_SHUT;
}

void EnHonotrap_EyeIdle(EnHonotrap* this, PlayState* play) {
    if (this->actor.child != NULL) {
        this->timer = 200;
    } else if ((this->timer <= 0) && (this->actor.xzDistToPlayer < 750.0f)) {
        if ((this->actor.playerHeightRel < 0.0f) && (this->actor.playerHeightRel > -700.0f)) {
            s16 angle = this->actor.yawTowardsPlayer - this->actor.shape.rot.y;

            if ((angle > -0x4000) && (angle < 0x4000)) {
                EnHonotrap_SetupEyeOpen(this);
            }
        }
    }
}

void EnHonotrap_SetupEyeOpen(EnHonotrap* this) {
    this->actionFunc = EnHonotrap_EyeOpen;
    Actor_SetColorFilter(&this->actor, COLORFILTER_COLORFLAG_RED, 255, COLORFILTER_BUFFLAG_OPA, 40);
    this->timer = 30;
    Actor_PlaySfx(&this->actor, NA_SE_EV_RED_EYE);
}

void EnHonotrap_EyeOpen(EnHonotrap* this, PlayState* play) {
    this->eyeState--;
    if (this->eyeState <= HONOTRAP_EYE_OPEN) {
        EnHonotrap_SetupEyeAttack(this);
        Actor_SpawnAsChild(
            &play->actorCtx, &this->actor, play, ACTOR_EN_HONOTRAP,
            (Math_SinS(this->actor.shape.rot.y) * 12.0f) + this->actor.home.pos.x, this->actor.home.pos.y - 10.0f,
            (Math_CosS(this->actor.shape.rot.y) * 12.0f) + this->actor.home.pos.z, this->actor.home.rot.x,
            this->actor.home.rot.y, this->actor.home.rot.z, HONOTRAP_TYPE_FLAME_MOVE);
    }
}

void EnHonotrap_SetupEyeAttack(EnHonotrap* this) {
    this->actionFunc = EnHonotrap_EyeAttack;
    this->eyeState = HONOTRAP_EYE_OPEN;
}

void EnHonotrap_EyeAttack(EnHonotrap* this, PlayState* play) {
    if (this->timer <= 0) {
        EnHonotrap_SetupEyeClose(this);
    }
}

void EnHonotrap_SetupEyeClose(EnHonotrap* this) {
    this->actionFunc = EnHonotrap_EyeClose;
}

void EnHonotrap_EyeClose(EnHonotrap* this, PlayState* play) {
    this->eyeState++;
    if (this->eyeState >= HONOTRAP_EYE_SHUT) {
        EnHonotrap_SetupEyeIdle(this);
        this->timer = 200;
    }
}

void EnHonotrap_SetupEyeIdle2(EnHonotrap* this) {
    this->eyeState = HONOTRAP_EYE_SHUT;
    this->timer = 80;
    this->unk224 = 0;
    this->actionFunc = EnHonotrap_EyeIdle2;
}

void EnHonotrap_EyeIdle2(EnHonotrap* this, PlayState* play) {
    if ((this->timer <= 0) && (this->actor.xzDistToPlayer < 120.0f)) {
        if ((this->actor.playerHeightRel < 0.0f) && (this->actor.playerHeightRel > -700.0f)) {
            s16 angle = this->actor.yawTowardsPlayer - this->actor.shape.rot.y;

            if ((angle > -0x4000) && (angle < 0x4000)) {
                EnHonotrap_SetupEyeOpen2(this);
            }
        }
    }
}

void EnHonotrap_SetupEyeOpen2(EnHonotrap* this) {
    Actor_PlaySfx(&this->actor, NA_SE_EV_RED_EYE);
    Actor_SetColorFilter(&this->actor, COLORFILTER_COLORFLAG_RED, 255, COLORFILTER_BUFFLAG_OPA, 40);
    this->timer = 40;
    this->actionFunc = EnHonotrap_EyeOpen2;
}

void EnHonotrap_EyeOpen2(EnHonotrap* this, PlayState* play) {
    s32 pad;

    this->eyeState--;
    if (this->eyeState <= HONOTRAP_EYE_OPEN) {
        Actor_SpawnAsChild(
            &play->actorCtx, &this->actor, play, ACTOR_EN_HONOTRAP,
            (Math_SinS(this->actor.shape.rot.y) * 12.0f) + this->actor.home.pos.x, this->actor.home.pos.y,
            (Math_CosS(this->actor.shape.rot.y) * 12.0f) + this->actor.home.pos.z, this->actor.home.rot.x,
            this->actor.home.rot.y, this->actor.home.rot.z, HONOTRAP_TYPE_FLAME_GROUP);
        EnHonotrap_SetupEyeAttack2(this);
    }
}

void EnHonotrap_SetupEyeAttack2(EnHonotrap* this) {
    this->actionFunc = EnHonotrap_EyeAttack2;
    this->eyeState = HONOTRAP_EYE_OPEN;
}

void EnHonotrap_EyeAttack2(EnHonotrap* this, PlayState* play) {
    if (this->timer <= 0) {
        EnHonotrap_SetupEyeClose2(this);
    }
}

void EnHonotrap_SetupEyeClose2(EnHonotrap* this) {
    this->actionFunc = EnHonotrap_EyeClose2;
}

void EnHonotrap_EyeClose2(EnHonotrap* this, PlayState* play) {
    this->eyeState++;
    if (this->eyeState >= HONOTRAP_EYE_SHUT) {
        EnHonotrap_SetupEyeIdle2(this);
    }
}

void EnHonotrap_SetupFlameGrow(EnHonotrap* this) {
    this->actionFunc = EnHonotrap_FlameGrow;
}

void EnHonotrap_FlameGrow(EnHonotrap* this, PlayState* play) {
    f32 targetScale = (this->actor.params == HONOTRAP_TYPE_FLAME_MOVE) ? 0.004f : 0.0048f;
    s32 targetReached = Math_StepToF(&this->actor.scale.x, targetScale, 0.0006f);

    this->actor.scale.z = this->actor.scale.y = this->actor.scale.x;
    if (targetReached) {
        if (this->actor.params == HONOTRAP_TYPE_FLAME_MOVE) {
            EnHonotrap_SetupFlameMove(this);
        } else { // HONOTRAP_TYPE_FLAME_DROP
            EnHonotrap_SetupFlameDrop(this);
        }
    }
}

void EnHonotrap_SetupFlameDrop(EnHonotrap* this) {
    this->timer = 40;
    this->actor.velocity.y = 1.0f;
    this->actor.velocity.x = 2.0f * Math_SinS(this->actor.world.rot.y);
    this->actor.velocity.z = 2.0f * Math_CosS(this->actor.world.rot.y);
    this->actionFunc = EnHonotrap_FlameDrop;
}

void EnHonotrap_FlameDrop(EnHonotrap* this, PlayState* play) {
    if ((this->collider.tris.base.atFlags & AT_HIT) || (this->timer <= 0)) {
        if ((this->collider.tris.base.atFlags & AT_HIT) && !(this->collider.tris.base.atFlags & AT_BOUNCED)) {
            func_800B8D98(play, &this->actor, 5.0f, this->actor.yawTowardsPlayer, 0.0f);
        }
        this->actor.velocity.z = 0.0f;
        this->actor.velocity.y = 0.0f;
        this->actor.velocity.x = 0.0f;
        EnHonotrap_SetupFlameVanish(this);
        return;
    }
    if (this->actor.velocity.y > 0.0f) {
        this->actor.world.pos.x += this->actor.velocity.x;
        this->actor.world.pos.z += this->actor.velocity.z;
        Actor_UpdateBgCheckInfo(play, &this->actor, 10.0f, 12.0f, 0.0f, UPDBGCHECKINFO_FLAG_1 | UPDBGCHECKINFO_FLAG_4);
    }
    if (!Math_StepToF(&this->actor.world.pos.y, this->actor.floorHeight + 1.0f, this->actor.velocity.y)) {
        this->actor.velocity.y += 1.0f;
    } else {
        this->actor.velocity.y = 0.0f;
    }
    EnHonotrap_FlameCollisionCheck(this, play);
}

void EnHonotrap_SetupFlameMove(EnHonotrap* this) {
    f32 distInverse;

    this->actionFunc = EnHonotrap_FlameMove;
    distInverse = 1.0f / (Actor_WorldDistXYZToPoint(&this->actor, &this->targetPos) + 1.0f);
    this->actor.velocity.x = (this->targetPos.x - this->actor.world.pos.x) * distInverse;
    this->actor.velocity.y = (this->targetPos.y - this->actor.world.pos.y) * distInverse;
    this->actor.velocity.z = (this->targetPos.z - this->actor.world.pos.z) * distInverse;
    this->speedMod = 0.0f;
    this->timer = 160;
}

void EnHonotrap_FlameMove(EnHonotrap* this, PlayState* play) {
    Actor* thisx = &this->actor;
    Vec3f speed;
    s32 cond;

    Math_StepToF(&this->speedMod, 13.0f, 0.5f);
    speed.x = fabsf(this->speedMod * thisx->velocity.x);
    speed.y = fabsf(this->speedMod * thisx->velocity.y);
    speed.z = fabsf(this->speedMod * thisx->velocity.z);
    cond = true;
    cond &= Math_StepToF(&thisx->world.pos.x, this->targetPos.x, speed.x);
    cond &= Math_StepToF(&thisx->world.pos.y, this->targetPos.y, speed.y);
    cond &= Math_StepToF(&thisx->world.pos.z, this->targetPos.z, speed.z);
    Actor_UpdateBgCheckInfo(play, thisx, 10.0f, 10.0f, 0.0f,
                            UPDBGCHECKINFO_FLAG_1 | UPDBGCHECKINFO_FLAG_4 | UPDBGCHECKINFO_FLAG_8 |
                                UPDBGCHECKINFO_FLAG_10);

    if (this->collider.tris.base.atFlags & AT_BOUNCED) {
        Player* player = GET_PLAYER(play);
        Vec3f shieldNorm;
        Vec3f tempVel;
        Vec3f shieldVec;

        shieldVec.x = -player->shieldMf.xz;
        shieldVec.y = -player->shieldMf.yz;
        shieldVec.z = -player->shieldMf.zz;
        EnHonotrap_GetNormal(&shieldNorm, &shieldVec);
        tempVel = thisx->velocity;
        Math3D_Vec3fReflect(&tempVel, &shieldNorm, &thisx->velocity);
        thisx->speed = this->speedMod * 0.5f;
        thisx->world.rot.y = Math_Atan2S_XY(thisx->velocity.z, thisx->velocity.x);
        EnHonotrap_SetupFlameVanish(this);
    } else if (this->collider.tris.base.atFlags & AT_HIT) {
        thisx->speed = 0.0f;
        thisx->velocity.y = 0.0f;
        EnHonotrap_SetupFlameVanish(this);
    } else if (this->timer <= 0) {
        EnHonotrap_SetupFlameVanish(this);
    } else {
        EnHonotrap_FlameCollisionCheck(this, play);
        if (cond != 0) {
            EnHonotrap_SetupFlameChase(this);
        }
    }
}

void EnHonotrap_SetupFlameChase(EnHonotrap* this) {
    this->actionFunc = EnHonotrap_FlameChase;
    this->actor.speed = 0.0f;
    this->actor.velocity.x = this->actor.velocity.y = this->actor.velocity.z = 0.0f;
    this->timer = 100;
    this->actor.world.rot.x = this->actor.world.rot.y = this->actor.world.rot.z = 0;
}

void EnHonotrap_FlameChase(EnHonotrap* this, PlayState* play) {
    s32 pad;

    Math_ScaledStepToS(&this->actor.world.rot.y, this->actor.yawTowardsPlayer, 0x300);
    Math_StepToF(&this->actor.speed, 3.0f, 0.1f);
    this->actor.gravity = (-this->actor.playerHeightRel < 10.0f) ? 0.08f : -0.08f;
    Actor_UpdateVelocityWithGravity(&this->actor);
    if (this->actor.velocity.y > 1.0f) {
        this->actor.velocity.y = 1.0f;
    }
    this->actor.velocity.y *= 0.95f;
    Actor_UpdatePos(&this->actor);
    Actor_UpdateBgCheckInfo(play, &this->actor, 10.0f, 10.0f, 0.0f,
                            UPDBGCHECKINFO_FLAG_1 | UPDBGCHECKINFO_FLAG_4 | UPDBGCHECKINFO_FLAG_8 |
                                UPDBGCHECKINFO_FLAG_10);

    if (this->collider.tris.base.atFlags & AT_BOUNCED) {
        Player* player = GET_PLAYER(play);
        Vec3s shieldRot;

        Matrix_MtxFToYXZRot(&player->shieldMf, &shieldRot, 0);
        this->actor.world.rot.y = ((shieldRot.y * 2) - this->actor.world.rot.y) + 0x8000;
        EnHonotrap_SetupFlameVanish(this);
    } else if (this->collider.tris.base.atFlags & AT_HIT) {
        this->actor.speed *= 0.1f;
        this->actor.velocity.y *= 0.1f;
        EnHonotrap_SetupFlameVanish(this);
    } else if ((this->actor.bgCheckFlags & BGCHECKFLAG_WALL) || (this->timer <= 0)) {
        EnHonotrap_SetupFlameVanish(this);
    } else {
        EnHonotrap_FlameCollisionCheck(this, play);
    }
}

void EnHonotrap_SetupFlameVanish(EnHonotrap* this) {
    this->actionFunc = EnHonotrap_FlameVanish;
}

void EnHonotrap_FlameVanish(EnHonotrap* this, PlayState* play) {
    s32 pad;
    s32 targetReached = Math_StepToF(&this->actor.scale.x, 0.0001f, 0.00015f);

    this->actor.scale.z = this->actor.scale.y = this->actor.scale.x;
    Actor_MoveWithGravity(&this->actor);
    Actor_UpdateBgCheckInfo(play, &this->actor, 10.0f, 10.0f, 0.0f,
                            UPDBGCHECKINFO_FLAG_1 | UPDBGCHECKINFO_FLAG_4 | UPDBGCHECKINFO_FLAG_8 |
                                UPDBGCHECKINFO_FLAG_10);
    if (targetReached) {
        Actor_Kill(&this->actor);
    }
}

void EnHonotrap_SetupFlameGroup(EnHonotrap* this) {
    this->actionFunc = EnHonotrap_FlameGroup;
    this->timer = 80;
    this->flameGroup.unk8 = 0.0f;
}

void EnHonotrap_FlameGroup(EnHonotrap* this, PlayState* play) {
    s32 i;
    EnHonotrapFlameGroup* flameGroup = &this->flameGroup;
    f32 var_fs0;
    f32 temp_fs0;
    f32 temp_fs1;
    f32 sp88;
    f32 sp84;
    f32 sp80;
    s32 flameScrollDisplacement;
    s32 sp78 = false;
    f32 var_fs0_2;
    Vec3f sp68;
    EnHonotrapFlameElement* flameElem;

    sp80 = fabsf(Math_CosS(Camera_GetCamDirPitch(GET_ACTIVE_CAM(play))));
    flameScrollDisplacement = (s32)(sp80 * -10.5f) - 10;
    Math_StepToF(&flameGroup->unk0, 1.0f, 0.05f);
    if (this->timer <= 40) {
        sp78 = Math_StepToF(&flameGroup->unk4, 1.0f, 0.05f);
    } else if (this->actor.parent == NULL) {
        this->timer = 40;
    }
    for (i = 0; i < ARRAY_COUNT(flameGroup->flameList); i++) {
        flameGroup->flameList[i].isDrawn = false;
    }

    sp88 = Math_SinS(this->actor.shape.rot.y) * 120.0f;
    sp84 = Math_CosS(this->actor.shape.rot.y) * 120.0f;

    flameGroup->unk8 += 0.050f * (1.0f - flameGroup->unk4);

    if (flameGroup->unk8 > 1.0f / 6) {
        flameGroup->unk8 -= 1.0f / 6;
    }
    var_fs0 = flameGroup->unk4 + flameGroup->unk8;

    for (i = 0; i < ARRAY_COUNT(flameGroup->flameList) && (var_fs0 <= flameGroup->unk0); i++) {
        flameElem = &flameGroup->flameList[i];
        flameElem->isDrawn = true;

        flameElem->pos.x = (sp88 * var_fs0) + this->actor.world.pos.x;
        flameElem->pos.y = this->actor.world.pos.y;
        flameElem->pos.z = (sp84 * var_fs0) + this->actor.world.pos.z;

        flameElem->unkC = Math_SinS(TRUNCF_BINANG(var_fs0 * 25486.223f)) * 1.6f;
        if (flameElem->unkC > 1.0f) {
            flameElem->unkC = 1.0f;
        } else if (flameElem->unkC < 0.0f) {
            flameElem->unkC = 0.0f;
        }

        var_fs0 += 1.0f / 6;
        flameElem->unkC *= (0.006f * (((1.0f - flameGroup->unk4) * 0.8f) + 0.2f));
        flameElem->flameScroll += flameScrollDisplacement;
        flameElem->flameScroll %= 0x200U;
    }

    if (sp78 || (this->timer <= 0)) {
        Actor_Kill(&this->actor);
        return;
    }
    temp_fs0 = flameGroup->unk0 * 120.0f;
    temp_fs1 = flameGroup->unk4 * 120.0f;
    Actor_WorldToActorCoords(&this->actor, &sp68, &GET_PLAYER(play)->actor.world.pos);

    if (sp68.z < temp_fs1) {
        sp68.z = temp_fs1;
    } else if (temp_fs0 < sp68.z) {
        sp68.z = temp_fs0;
    }

    var_fs0_2 = Math_SinS(TRUNCF_BINANG(sp68.z * 212.3852f)) * 1.6f;
    if (var_fs0_2 > 1.0f) {
        var_fs0_2 = 1.0f;
    }
    sp80 *= ((1.0f - flameGroup->unk4) * 0.8f) + 0.2f;
    if (sp80 > 0.2f) {
        this->collider.cyl.dim.pos.x =
            TRUNCF_BINANG((Math_SinS(this->actor.shape.rot.y) * sp68.z) + this->actor.world.pos.x);
        this->collider.cyl.dim.pos.y = TRUNCF_BINANG(this->actor.world.pos.y - (24.0f * var_fs0_2));
        this->collider.cyl.dim.pos.z =
            TRUNCF_BINANG((Math_CosS(this->actor.shape.rot.y) * sp68.z) + this->actor.world.pos.z);
        this->collider.cyl.dim.radius = TRUNCF_BINANG(15.0f * var_fs0_2);
        this->collider.cyl.dim.height = TRUNCF_BINANG(37.5f * var_fs0_2);
        CollisionCheck_SetAT(play, &play->colChkCtx, &this->collider.tris.base);
        CollisionCheck_SetAC(play, &play->colChkCtx, &this->collider.tris.base);
        CollisionCheck_SetOC(play, &play->colChkCtx, &this->collider.tris.base);
    }
}

void EnHonotrap_Update(Actor* thisx, PlayState* play) {
    static Vec3f sVelocity = { 0.0f, 0.0f, 0.0f };
    static Vec3f sAccel = { 0.0f, 0.1f, 0.0f };
    EnHonotrap* this = (EnHonotrap*)thisx;

    if (this->timer > 0) {
        this->timer--;
    }
    if ((this->actor.child != NULL) && (this->actor.child->update == NULL)) {
        this->actor.child = NULL;
    }
    this->actionFunc(this, play);
    if (this->collider.tris.base.acFlags & AC_HIT) {
        EffectSsBomb2_SpawnLayered(play, &this->actor.world.pos, &sVelocity, &sAccel, 15, 8);
        Actor_Kill(&this->actor);
        return;
    }
    if (this->eyeState < HONOTRAP_EYE_SHUT) {
        this->collider.tris.base.acFlags &= ~AC_HIT;
        CollisionCheck_SetAC(play, &play->colChkCtx, &this->collider.tris.base);
    }
}

void EnHonotrap_UpdateFlame(Actor* thisx, PlayState* play) {
    EnHonotrap* this = (EnHonotrap*)thisx;

    if (this->timer > 0) {
        this->timer--;
    }
    this->colChkFlags = 0;
    this->bobPhase += 0x640;
    if (this->actor.params != HONOTRAP_TYPE_FLAME_GROUP) {
        this->actor.shape.yOffset = Math_SinS(this->bobPhase) * 1000.0f + 600.0f;
    }
    Actor_SetFocus(&this->actor, 5.0f);
    Actor_PlaySfx(&this->actor, NA_SE_EV_BURN_OUT - SFX_FLAG);
    this->actionFunc(this, play);
    this->flameScroll -= 20;
    this->flameScroll %= 0x200U;
}

void EnHonotrap_UpdateFlameGroup(Actor* thisx, PlayState* play) {
    EnHonotrap* this = (EnHonotrap*)thisx;
    Actor* parent = this->actor.parent;

    if ((parent != NULL) && (parent->update == NULL)) {
        this->actor.parent = NULL;
    }
    if (this->timer > 0) {
        this->timer--;
    }
    this->bobPhase += 0x640;
    Actor_PlaySfx(&this->actor, NA_SE_EV_BURN_OUT - SFX_FLAG);
    this->actionFunc(this, play);
}

void EnHonotrap_Draw(Actor* thisx, PlayState* play) {
    EnHonotrap* this = (EnHonotrap*)thisx;

    OPEN_DISPS(play->state.gfxCtx);

    Gfx_SetupDL25_Opa(play->state.gfxCtx);
    gSPSegment(POLY_OPA_DISP++, 0x08, sSilverEyeTextures[this->eyeState]);
    MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx);
    gSPDisplayList(POLY_OPA_DISP++, &gEyeSwitchSilverDL);

    CLOSE_DISPS(play->state.gfxCtx);
}

void EnHonotrap_DrawFlame(Actor* thisx, PlayState* play) {
    s32 pad;
    EnHonotrap* this = (EnHonotrap*)thisx;

    OPEN_DISPS(play->state.gfxCtx);

    Gfx_SetupDL25_Xlu(play->state.gfxCtx);
    gSPSegment(POLY_XLU_DISP++, 0x08,
               Gfx_TwoTexScrollEx(play->state.gfxCtx, 0, 0, 0, 32, 64, 1, 0, this->flameScroll, 32, 128, 0, 0, 0, -20));
    gDPSetPrimColor(POLY_XLU_DISP++, 0x80, 0x80, 255, 200, 0, 255);
    gDPSetEnvColor(POLY_XLU_DISP++, 255, 0, 0, 0);
    Matrix_RotateYS(Camera_GetCamDirYaw(GET_ACTIVE_CAM(play)) + 0x8000, MTXMODE_APPLY);
    MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx);
    gSPDisplayList(POLY_XLU_DISP++, gEffFire1DL);

    CLOSE_DISPS(play->state.gfxCtx);
}

void EnHonotrap_DrawFlameGroup(Actor* thisx, PlayState* play) {
    s32 pad;
    EnHonotrap* this = (EnHonotrap*)thisx;
    EnHonotrapFlameElement* flameElem;
    EnHonotrapFlameGroup* flameGroup = &this->flameGroup;
    s32 i;
    s32 pad2;
    Vec3s camDir;

    OPEN_DISPS(play->state.gfxCtx);

    Gfx_SetupDL25_Xlu(play->state.gfxCtx);
    gDPSetPrimColor(POLY_XLU_DISP++, 0x80, 0x80, 255, 200, 0, 255);
    gDPSetEnvColor(POLY_XLU_DISP++, 255, 0, 0, 0);
    camDir = Camera_GetCamDir(GET_ACTIVE_CAM(play));
    camDir.y += 0x8000;

    for (i = 0; i < ARRAY_COUNT(flameGroup->flameList); i++) {
        flameElem = &flameGroup->flameList[i];
        if (flameElem->isDrawn) {
            gSPSegment(POLY_XLU_DISP++, 0x08,
                       Gfx_TwoTexScrollEx(play->state.gfxCtx, 0, 0, 0, 32, 64, 1, 0, flameElem->flameScroll, 32, 128, 0,
                                          0, 0, -20));
            Matrix_SetTranslateRotateYXZ(flameElem->pos.x, flameElem->pos.y - (4000.0f * flameElem->unkC),
                                         flameElem->pos.z, &camDir);
            Matrix_Scale(((fabsf(Math_SinS((s16)(camDir.y - thisx->shape.rot.y) >> 1)) * 0.2f) + 1.7f) *
                             flameElem->unkC,
                         flameElem->unkC, 1.0f, MTXMODE_APPLY);
            MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx);
            gSPDisplayList(POLY_XLU_DISP++, gEffFire1DL);
        }
    }

    CLOSE_DISPS(play->state.gfxCtx);
}
