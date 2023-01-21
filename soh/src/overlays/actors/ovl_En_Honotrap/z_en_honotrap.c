/**
 * File: z_en_honotrap.c
 * Overlay: ovl_En_Honotrap
 * Description: Fake eye switches and Dampe flames
 */

#include "z_en_honotrap.h"
#include "objects/gameplay_keep/gameplay_keep.h"
#include "objects/gameplay_dangeon_keep/gameplay_dangeon_keep.h"

#define FLAGS ACTOR_FLAG_4

#define HONOTRAP_AT_ACTIVE (1 << 0)
#define HONOTRAP_AC_ACTIVE (1 << 1)
#define HONOTRAP_OC_ACTIVE (1 << 2)

typedef enum {
    /* 0 */ HONOTRAP_EYE_OPEN,
    /* 1 */ HONOTRAP_EYE_HALF,
    /* 2 */ HONOTRAP_EYE_CLOSE,
    /* 3 */ HONOTRAP_EYE_SHUT
} EnHonotrapEyeState;

void EnHonotrap_Init(Actor* thisx, PlayState* play);
void EnHonotrap_Destroy(Actor* thisx, PlayState* play);
void EnHonotrap_Update(Actor* thisx, PlayState* play);
void EnHonotrap_Draw(Actor* thisx, PlayState* play);

void EnHonotrap_SetupEyeIdle(EnHonotrap* this);
void EnHonotrap_EyeIdle(EnHonotrap* this, PlayState* play);
void EnHonotrap_SetupEyeOpen(EnHonotrap* this);
void EnHonotrap_EyeOpen(EnHonotrap* this, PlayState* play);
void EnHonotrap_SetupEyeAttack(EnHonotrap* this);
void EnHonotrap_EyeAttack(EnHonotrap* this, PlayState* play);
void EnHonotrap_SetupEyeClose(EnHonotrap* this);
void EnHonotrap_EyeClose(EnHonotrap* this, PlayState* play);

void EnHonotrap_SetupFlame(EnHonotrap* this);
void EnHonotrap_Flame(EnHonotrap* this, PlayState* play);
void EnHonotrap_SetupFlameDrop(EnHonotrap* this);
void EnHonotrap_FlameDrop(EnHonotrap* this, PlayState* play);

void EnHonotrap_SetupFlameMove(EnHonotrap* this);
void EnHonotrap_FlameMove(EnHonotrap* this, PlayState* play);
void EnHonotrap_SetupFlameChase(EnHonotrap* this);
void EnHonotrap_FlameChase(EnHonotrap* this, PlayState* play);
void EnHonotrap_SetupFlameVanish(EnHonotrap* this);
void EnHonotrap_FlameVanish(EnHonotrap* this, PlayState* play);

const ActorInit En_Honotrap_InitVars = {
    ACTOR_EN_HONOTRAP,
    ACTORCAT_PROP,
    FLAGS,
    OBJECT_GAMEPLAY_DANGEON_KEEP,
    sizeof(EnHonotrap),
    (ActorFunc)EnHonotrap_Init,
    (ActorFunc)EnHonotrap_Destroy,
    (ActorFunc)EnHonotrap_Update,
    (ActorFunc)EnHonotrap_Draw,
    NULL,
};

static ColliderTrisElementInit sTrisElementsInit[2] = {
    {
        {
            ELEMTYPE_UNK4,
            { 0x00000000, 0x00, 0x00 },
            { 0x0001F824, 0x00, 0x00 },
            TOUCH_NONE,
            BUMP_ON,
            OCELEM_NONE,
        },
        { { { 0.0f, 23.0f, 8.5f }, { -23.0f, 0.0f, 8.5f }, { 0.0f, -23.0f, 8.5f } } },
    },
    {
        {
            ELEMTYPE_UNK0,
            { 0x00000000, 0x00, 0x00 },
            { 0x0001F824, 0x00, 0x00 },
            TOUCH_NONE,
            BUMP_ON,
            OCELEM_NONE,
        },
        { { { 0.0f, 23.0f, 8.5f }, { 0.0f, -23.0f, 8.5f }, { 23.0f, 0.0f, 8.5f } } },
    },
};

static ColliderTrisInit sTrisInit = {
    {
        COLTYPE_NONE,
        AT_NONE,
        AC_ON | AC_TYPE_PLAYER,
        OC1_NONE,
        OC2_NONE,
        COLSHAPE_TRIS,
    },
    2,
    sTrisElementsInit,
};

static ColliderCylinderInit sCylinderInit = {
    {
        COLTYPE_NONE,
        AT_ON | AT_TYPE_ENEMY,
        AC_ON | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_2,
        COLSHAPE_CYLINDER,
    },
    {
        ELEMTYPE_UNK0,
        { 0xFFCFFFFF, 0x01, 0x04 },
        { 0x00100000, 0x00, 0x00 },
        TOUCH_ON | TOUCH_SFX_NONE,
        BUMP_ON,
        OCELEM_ON,
    },
    { 10, 25, 0, { 0, 0, 0 } },
};

static CollisionCheckInfoInit sColChkInfoInit = { 0, 9, 23, 1 };

static InitChainEntry sInitChain[] = {
    ICHAIN_F32(uncullZoneForward, 1000, ICHAIN_CONTINUE),
    ICHAIN_F32(uncullZoneScale, 500, ICHAIN_CONTINUE),
    ICHAIN_F32(uncullZoneDownward, 1000, ICHAIN_STOP),
};

void EnHonotrap_FlameCollisionCheck(EnHonotrap* this, PlayState* play) {
    s32 pad[3];

    Collider_UpdateCylinder(&this->actor, &this->collider.cyl);
    CollisionCheck_SetAT(play, &play->colChkCtx, &this->collider.cyl.base);
    CollisionCheck_SetAC(play, &play->colChkCtx, &this->collider.cyl.base);
    CollisionCheck_SetOC(play, &play->colChkCtx, &this->collider.cyl.base);
    this->colChkFlags |= HONOTRAP_AT_ACTIVE;
    this->colChkFlags |= HONOTRAP_AC_ACTIVE;
    this->colChkFlags |= HONOTRAP_OC_ACTIVE;
}

void EnHonotrap_GetNormal(Vec3f* normal, Vec3f* vec) {
    f32 mag = Math3D_Vec3fMagnitude(vec);

    if (mag < 0.001f) {
        osSyncPrintf("Warning : vector size zero (%s %d)\n", __FILE__, __LINE__, normal);

        normal->x = normal->y = 0.0f;
        normal->z = 1.0f;
    } else {
        normal->x = vec->x * (1.0f / mag);
        normal->y = vec->y * (1.0f / mag);
        normal->z = vec->z * (1.0f / mag);
    }
}

void EnHonotrap_InitEye(Actor* thisx, PlayState* play) {
    s32 pad;
    EnHonotrap* this = (EnHonotrap*)thisx;
    s32 i;
    s32 j;
    Vec3f* vtx;
    Vec3f triangle[3];
    f32 cos;
    f32 sin;

    Actor_SetScale(thisx, 0.1f);
    sin = Math_SinS(thisx->home.rot.y);
    cos = Math_CosS(thisx->home.rot.y);
    Collider_InitTris(play, &this->collider.tris);
    Collider_SetTris(play, &this->collider.tris, thisx, &sTrisInit, this->collider.elements);

    for (i = 0; i < 2; i++) {
        for (j = 0, vtx = triangle; j < 3; j++, vtx++) {
            Vec3f* baseVtx = &sTrisInit.elements[i].dim.vtx[j];

            vtx->x = baseVtx->z * sin + baseVtx->x * cos;
            vtx->y = baseVtx->y;
            vtx->z = baseVtx->z * cos - baseVtx->x * sin;
            Math_Vec3f_Sum(vtx, &thisx->world.pos, vtx);
        }
        Collider_SetTrisVertices(&this->collider.tris, i, &triangle[0], &triangle[1], &triangle[2]);
    }
    EnHonotrap_SetupEyeIdle(this);
    Actor_SetFocus(thisx, 0.0f);
}

void EnHonotrap_InitFlame(Actor* thisx, PlayState* play) {
    s32 pad;
    EnHonotrap* this = (EnHonotrap*)thisx;

    Actor_SetScale(&this->actor, 0.0001f);
    Collider_InitCylinder(play, &this->collider.cyl);
    Collider_SetCylinder(play, &this->collider.cyl, &this->actor, &sCylinderInit);
    Collider_UpdateCylinder(&this->actor, &this->collider.cyl);
    this->actor.minVelocityY = -1.0f;
    CollisionCheck_SetInfo(&this->actor.colChkInfo, NULL, &sColChkInfoInit);
    ActorShape_Init(&this->actor.shape, 0.0f, ActorShadow_DrawCircle, 30.0f);
    this->actor.shape.shadowAlpha = 128;
    this->targetPos = GET_PLAYER(play)->actor.world.pos;
    this->targetPos.y += 10.0f;
    this->flameScroll = Rand_ZeroOne() * 511.0f;
    EnHonotrap_SetupFlame(this);
    Audio_PlayActorSound2(&this->actor, NA_SE_EV_FLAME_IGNITION);
    if (this->actor.params == HONOTRAP_FLAME_DROP) {
        this->actor.room = -1;
        this->collider.cyl.dim.radius = 12;
        this->collider.cyl.dim.height = 30;
        this->actor.shape.yOffset = -1000.0f;
    }
}

void EnHonotrap_Init(Actor* thisx, PlayState* play) {
    Actor_ProcessInitChain(thisx, sInitChain);
    if (thisx->params == HONOTRAP_EYE) {
        EnHonotrap_InitEye(thisx, play);
    } else {
        EnHonotrap_InitFlame(thisx, play);
    }
}

void EnHonotrap_Destroy(Actor* thisx, PlayState* play) {
    s32 pad;
    EnHonotrap* this = (EnHonotrap*)thisx;

    if (this->actor.params == HONOTRAP_EYE) {
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
    } else if ((this->timer <= 0) && (this->actor.xzDistToPlayer < 750.0f) && (0.0f > this->actor.yDistToPlayer) &&
               (this->actor.yDistToPlayer > -700.0f) &&
               (-0x4000 < (this->actor.yawTowardsPlayer - this->actor.shape.rot.y)) &&
               ((this->actor.yawTowardsPlayer - this->actor.shape.rot.y) < 0x4000)) {
        EnHonotrap_SetupEyeOpen(this);
    }
}

void EnHonotrap_SetupEyeOpen(EnHonotrap* this) {
    this->actionFunc = EnHonotrap_EyeOpen;
    Actor_SetColorFilter(&this->actor, 0x4000, 0xFF, 0, 0x28);
    this->timer = 30;
    Audio_PlayActorSound2(&this->actor, NA_SE_EV_RED_EYE);
}

void EnHonotrap_EyeOpen(EnHonotrap* this, PlayState* play) {
    f32 cos;
    f32 sin;

    this->eyeState--;
    if (this->eyeState <= HONOTRAP_EYE_OPEN) {
        EnHonotrap_SetupEyeAttack(this);
        sin = Math_SinS(this->actor.shape.rot.y);
        cos = Math_CosS(this->actor.shape.rot.y);
        Actor_SpawnAsChild(&play->actorCtx, &this->actor, play, ACTOR_EN_HONOTRAP,
                           (sin * 12.0f) + this->actor.home.pos.x, this->actor.home.pos.y - 10.0f,
                           (cos * 12.0f) + this->actor.home.pos.z, this->actor.home.rot.x, this->actor.home.rot.y,
                           this->actor.home.rot.z, HONOTRAP_FLAME_MOVE);
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

void EnHonotrap_SetupFlame(EnHonotrap* this) {
    this->actionFunc = EnHonotrap_Flame;
}

void EnHonotrap_Flame(EnHonotrap* this, PlayState* play) {
    s32 pad;
    s32 ready =
        Math_StepToF(&this->actor.scale.x, (this->actor.params == HONOTRAP_FLAME_MOVE) ? 0.004f : 0.0048f, 0.0006f);

    this->actor.scale.z = this->actor.scale.y = this->actor.scale.x;
    if (ready) {
        if (this->actor.params == HONOTRAP_FLAME_MOVE) {
            EnHonotrap_SetupFlameMove(this);
        } else {
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
    if ((this->collider.cyl.base.atFlags & AT_HIT) || (this->timer <= 0)) {
        if ((this->collider.cyl.base.atFlags & AT_HIT) && !(this->collider.cyl.base.atFlags & AT_BOUNCED)) {
            func_8002F71C(play, &this->actor, 5.0f, this->actor.yawTowardsPlayer, 0.0f);
        }
        this->actor.velocity.x = this->actor.velocity.y = this->actor.velocity.z = 0.0f;
        EnHonotrap_SetupFlameVanish(this);
    } else {
        if (this->actor.velocity.y > 0.0f) {
            this->actor.world.pos.x += this->actor.velocity.x;
            this->actor.world.pos.z += this->actor.velocity.z;
            Actor_UpdateBgCheckInfo(play, &this->actor, 7.0f, 12.0f, 0.0f, 5);
        }
        if (!Math_StepToF(&this->actor.world.pos.y, this->actor.floorHeight + 1.0f, this->actor.velocity.y)) {
            this->actor.velocity.y += 1.0f;
        } else {
            this->actor.velocity.y = 0.0f;
        }
        EnHonotrap_FlameCollisionCheck(this, play);
    }
}

void EnHonotrap_SetupFlameMove(EnHonotrap* this) {
    f32 distFrac;

    this->actionFunc = EnHonotrap_FlameMove;

    distFrac = 1.0f / (Actor_WorldDistXYZToPoint(&this->actor, &this->targetPos) + 1.0f);
    this->actor.velocity.x = (this->targetPos.x - this->actor.world.pos.x) * distFrac;
    this->actor.velocity.y = (this->targetPos.y - this->actor.world.pos.y) * distFrac;
    this->actor.velocity.z = (this->targetPos.z - this->actor.world.pos.z) * distFrac;
    this->speedMod = 0.0f;

    this->timer = 160;
}

void EnHonotrap_FlameMove(EnHonotrap* this, PlayState* play) {
    s32 pad;
    Vec3f speed;
    s32 ready;

    Math_StepToF(&this->speedMod, 13.0f, 0.5f);
    speed.x = fabsf(this->speedMod * this->actor.velocity.x);
    speed.y = fabsf(this->speedMod * this->actor.velocity.y);
    speed.z = fabsf(this->speedMod * this->actor.velocity.z);
    ready = true;
    ready &= Math_StepToF(&this->actor.world.pos.x, this->targetPos.x, speed.x);
    ready &= Math_StepToF(&this->actor.world.pos.y, this->targetPos.y, speed.y);
    ready &= Math_StepToF(&this->actor.world.pos.z, this->targetPos.z, speed.z);
    Actor_UpdateBgCheckInfo(play, &this->actor, 7.0f, 10.0f, 0.0f, 0x1D);

    if (this->collider.tris.base.atFlags & AT_BOUNCED) {
        Player* player = GET_PLAYER(play);
        Vec3f shieldNorm;
        Vec3f tempVel;
        Vec3f shieldVec;

        shieldVec.x = -player->shieldMf.xz;
        shieldVec.y = -player->shieldMf.yz;
        shieldVec.z = -player->shieldMf.zz;
        EnHonotrap_GetNormal(&shieldNorm, &shieldVec);

        tempVel = this->actor.velocity;
        Math3D_Vec3fReflect(&tempVel, &shieldNorm, &this->actor.velocity);
        this->actor.speedXZ = this->speedMod * 0.5f;
        this->actor.world.rot.y = Math_Atan2S(this->actor.velocity.z, this->actor.velocity.x);
        EnHonotrap_SetupFlameVanish(this);
    } else if (this->collider.tris.base.atFlags & AT_HIT) {
        this->actor.velocity.y = this->actor.speedXZ = 0.0f;
        EnHonotrap_SetupFlameVanish(this);
    } else if (this->timer <= 0) {
        EnHonotrap_SetupFlameVanish(this);
    } else {
        EnHonotrap_FlameCollisionCheck(this, play);
        if (ready) {
            EnHonotrap_SetupFlameChase(this);
        }
    }
}

void EnHonotrap_SetupFlameChase(EnHonotrap* this) {
    this->actionFunc = EnHonotrap_FlameChase;

    this->actor.velocity.x = this->actor.velocity.y = this->actor.velocity.z = this->actor.speedXZ = 0.0f;
    this->actor.world.rot.x = this->actor.world.rot.y = this->actor.world.rot.z = 0;

    this->timer = 100;
}

void EnHonotrap_FlameChase(EnHonotrap* this, PlayState* play) {
    s32 pad;

    Math_ScaledStepToS(&this->actor.world.rot.y, this->actor.yawTowardsPlayer, 0x300);
    Math_StepToF(&this->actor.speedXZ, 3.0f, 0.1f);
    this->actor.gravity = (-this->actor.yDistToPlayer < 10.0f) ? 0.08f : -0.08f;
    func_8002D868(&this->actor);
    if (this->actor.velocity.y > 1.0f) {
        this->actor.velocity.y = 1.0f;
    }
    this->actor.velocity.y *= 0.95f;
    func_8002D7EC(&this->actor);
    Actor_UpdateBgCheckInfo(play, &this->actor, 7.0f, 10.0f, 0.0f, 0x1D);
    if (this->collider.cyl.base.atFlags & AT_BOUNCED) {
        Player* player = GET_PLAYER(play);
        Vec3s shieldRot;

        Matrix_MtxFToYXZRotS(&player->shieldMf, &shieldRot, false);
        this->actor.world.rot.y = ((shieldRot.y * 2) - this->actor.world.rot.y) + 0x8000;
        EnHonotrap_SetupFlameVanish(this);
    } else if (this->collider.cyl.base.atFlags & AT_HIT) {
        this->actor.speedXZ *= 0.1f;
        this->actor.velocity.y *= 0.1f;
        EnHonotrap_SetupFlameVanish(this);
    } else if ((this->actor.bgCheckFlags & 8) || (this->timer <= 0)) {
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
    s32 ready = Math_StepToF(&this->actor.scale.x, 0.0001f, 0.00015f);

    this->actor.scale.z = this->actor.scale.y = this->actor.scale.x;
    Actor_MoveForward(&this->actor);
    Actor_UpdateBgCheckInfo(play, &this->actor, 7.0f, 10.0f, 0.0f, 0x1D);
    if (ready) {
        Actor_Kill(&this->actor);
    }
}

void EnHonotrap_Update(Actor* thisx, PlayState* play) {
    static Vec3f velocity = { 0.0f, 0.0f, 0.0f };
    static Vec3f accel = { 0.0f, 0.1f, 0.0f };
    s32 pad;
    EnHonotrap* this = (EnHonotrap*)thisx;

    if (this->timer > 0) {
        this->timer--;
    }
    if (this->actor.params == HONOTRAP_EYE) {
        if ((this->actor.child != NULL) && (this->actor.child->update == NULL)) {
            this->actor.child = NULL;
        }
    } else {
        this->colChkFlags = 0;
        this->bobPhase += 0x640;
        this->actor.shape.yOffset = (Math_SinS(this->bobPhase) * 1000.0f) + 600.0f;
        Actor_SetFocus(&this->actor, 5.0f);
        Audio_PlayActorSound2(&this->actor, NA_SE_EV_BURN_OUT - SFX_FLAG);
    }
    this->actionFunc(this, play);
    if (this->actor.params == HONOTRAP_EYE) {
        if (this->collider.tris.base.acFlags & AC_HIT) {
            EffectSsBomb2_SpawnLayered(play, &this->actor.world.pos, &velocity, &accel, 15, 8);
            Actor_Kill(&this->actor);
        } else if (this->eyeState < HONOTRAP_EYE_SHUT) {
            this->collider.tris.base.acFlags &= ~AC_HIT;
            CollisionCheck_SetAC(play, &play->colChkCtx, &this->collider.tris.base);
        }
    }
}

void EnHonotrap_DrawEye(Actor* thisx, PlayState* play) {
    static void* eyeTextures[] = {
        gEyeSwitchSilverOpenTex,
        gEyeSwitchSilverHalfTex,
        gEyeSwitchSilverClosedTex,
        gEyeSwitchSilverClosedTex,
    };
    EnHonotrap* this = (EnHonotrap*)thisx;

    OPEN_DISPS(play->state.gfxCtx);

    Gfx_SetupDL_25Opa(play->state.gfxCtx);
    gSPSegment(POLY_OPA_DISP++, 0x08, SEGMENTED_TO_VIRTUAL(eyeTextures[this->eyeState]));
    gSPMatrix(POLY_OPA_DISP++, MATRIX_NEWMTX(play->state.gfxCtx),
              G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
    gSPDisplayList(POLY_OPA_DISP++, gEyeSwitch2DL);

    CLOSE_DISPS(play->state.gfxCtx);
}

void EnHonotrap_DrawFlame(Actor* thisx, PlayState* play) {
    s32 pad;
    EnHonotrap* this = (EnHonotrap*)thisx;

    OPEN_DISPS(play->state.gfxCtx);

    Gfx_SetupDL_25Xlu(play->state.gfxCtx);
    this->flameScroll -= 20;
    this->flameScroll &= 0x1FF;
    gSPSegment(POLY_XLU_DISP++, 0x08,
               Gfx_TwoTexScroll(play->state.gfxCtx, 0, 0, 0, 0x20, 0x40, 1, 0, this->flameScroll, 0x20, 0x80));
    gDPSetPrimColor(POLY_XLU_DISP++, 0x80, 0x80, 255, 200, 0, 255);
    gDPSetEnvColor(POLY_XLU_DISP++, 255, 0, 0, 0);
    Matrix_RotateY((s16)(Camera_GetCamDirYaw(GET_ACTIVE_CAM(play)) - this->actor.shape.rot.y + 0x8000) *
                       (M_PI / 0x8000),
                   MTXMODE_APPLY);
    gSPMatrix(POLY_XLU_DISP++, MATRIX_NEWMTX(play->state.gfxCtx),
              G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
    gSPDisplayList(POLY_XLU_DISP++, gEffFire1DL);

    CLOSE_DISPS(play->state.gfxCtx);
}

void EnHonotrap_Draw(Actor* thisx, PlayState* play) {
    switch (thisx->params) {
        case HONOTRAP_EYE:
            EnHonotrap_DrawEye(thisx, play);
            break;
        case HONOTRAP_FLAME_MOVE:
        case HONOTRAP_FLAME_DROP:
            EnHonotrap_DrawFlame(thisx, play);
            break;
    }
}
