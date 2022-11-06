/*
 *  File: z_bg_jya_goroiwa.c
 *  Overlay: ovl_Bg_Jya_Goroiwa
 *  Description: Rolling Boulder
 *  moves very slowly in some cases
 */

#include "z_bg_jya_goroiwa.h"
#include "objects/object_goroiwa/object_goroiwa.h"

#define FLAGS ACTOR_FLAG_4

void BgJyaGoroiwa_Init(Actor* thisx, PlayState* play);
void BgJyaGoroiwa_Destroy(Actor* thisx, PlayState* play);
void BgJyaGoroiwa_Update(Actor* thisx, PlayState* play);
void BgJyaGoroiwa_Draw(Actor* thisx, PlayState* play);

void BgJyaGoroiwa_Wait(BgJyaGoroiwa* this, PlayState* play);
void BgJyaGoroiwa_Move(BgJyaGoroiwa* this, PlayState* play);

void BgJyaGoroiwa_SetupWait(BgJyaGoroiwa* this);
void BgJyaGoroiwa_SetupMove(BgJyaGoroiwa* this);
void BgJyaGoroiwa_UpdateRotation(BgJyaGoroiwa* this);
void BgJyaGoroiwa_UpdateCollider(BgJyaGoroiwa* this);

const ActorInit Bg_Jya_Goroiwa_InitVars = {
    ACTOR_BG_JYA_GOROIWA,
    ACTORCAT_PROP,
    FLAGS,
    OBJECT_GOROIWA,
    sizeof(BgJyaGoroiwa),
    (ActorFunc)BgJyaGoroiwa_Init,
    (ActorFunc)BgJyaGoroiwa_Destroy,
    (ActorFunc)BgJyaGoroiwa_Update,
    (ActorFunc)BgJyaGoroiwa_Draw,
    NULL,
};

static ColliderJntSphElementInit sJntSphElementsInit[] = {
    {
        {
            ELEMTYPE_UNK0,
            { 0x20000000, 0x00, 0x04 },
            { 0x00000000, 0x00, 0x00 },
            TOUCH_ON | TOUCH_SFX_NORMAL,
            BUMP_NONE,
            OCELEM_ON,
        },
        { 0, { { 0, 0, 0 }, 58 }, 100 },
    },
};

static ColliderJntSphInit sJntSphInit = {
    {
        COLTYPE_NONE,
        AT_ON | AT_TYPE_ENEMY,
        AC_NONE,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_2,
        COLSHAPE_JNTSPH,
    },
    1,
    sJntSphElementsInit,
};

static CollisionCheckInfoInit sColChkInfoInit = { 1, 15, 0, MASS_HEAVY };

static InitChainEntry sInitChain[] = {
    ICHAIN_VEC3F_DIV1000(scale, 100, ICHAIN_CONTINUE),
    ICHAIN_F32(uncullZoneForward, 1000, ICHAIN_CONTINUE),
    ICHAIN_F32(uncullZoneScale, 500, ICHAIN_CONTINUE),
    ICHAIN_F32(uncullZoneDownward, 1000, ICHAIN_STOP),
};

void BgJyaGoroiwa_UpdateCollider(BgJyaGoroiwa* this) {
    Sphere16* worldSphere = &this->collider.elements[0].dim.worldSphere;

    worldSphere->center.x = this->actor.world.pos.x;
    worldSphere->center.y = this->actor.world.pos.y + 59.5f;
    worldSphere->center.z = this->actor.world.pos.z;
}

void BgJyaGoroiwa_InitCollider(BgJyaGoroiwa* this, PlayState* play) {
    s32 pad;

    Collider_InitJntSph(play, &this->collider);
    Collider_SetJntSph(play, &this->collider, &this->actor, &sJntSphInit, &this->colliderItem);
    BgJyaGoroiwa_UpdateCollider(this);
    this->collider.elements[0].dim.worldSphere.radius = 58;
}

void BgJyaGoroiwa_UpdateRotation(BgJyaGoroiwa* this) {
    f32 xDiff = this->actor.world.pos.x - this->actor.prevPos.x;

    this->actor.shape.rot.z -= 0x10000 / (119 * M_PI) * xDiff;
}

void BgJyaGoroiwa_Init(Actor* thisx, PlayState* play) {
    BgJyaGoroiwa* this = (BgJyaGoroiwa*)thisx;

    Actor_ProcessInitChain(&this->actor, sInitChain);
    BgJyaGoroiwa_InitCollider(this, play);
    this->actor.shape.rot.x = this->actor.shape.rot.y = this->actor.shape.rot.z = 0;
    CollisionCheck_SetInfo(&this->actor.colChkInfo, NULL, &sColChkInfoInit);
    ActorShape_Init(&this->actor.shape, 595.0f, ActorShadow_DrawCircle, 9.0f);
    this->actor.shape.shadowAlpha = 128;
    BgJyaGoroiwa_SetupMove(this);
}

void BgJyaGoroiwa_Destroy(Actor* thisx, PlayState* play) {
    BgJyaGoroiwa* this = (BgJyaGoroiwa*)thisx;

    Collider_DestroyJntSph(play, &this->collider);
}

void BgJyaGoroiwa_SetupMove(BgJyaGoroiwa* this) {
    this->actionFunc = BgJyaGoroiwa_Move;
    this->collider.base.atFlags |= AT_ON;
    this->hasHit = false;
    this->speedFactor = 1.0f;
}

void BgJyaGoroiwa_Move(BgJyaGoroiwa* this, PlayState* play) {
    Actor* thisx = &this->actor;
    s16 relYawTowardsPlayer;
    f32 speedXZsqBase = (-100.0f - thisx->world.pos.y) * 2.5f;
    f32 posYfac;

    if (speedXZsqBase < 0.01f) {
        speedXZsqBase = 0.01f;
    }

    thisx->speedXZ = sqrtf(speedXZsqBase) * this->speedFactor;
    thisx->velocity.x = Math_SinS(thisx->world.rot.y) * thisx->speedXZ;
    thisx->velocity.z = Math_CosS(thisx->world.rot.y) * thisx->speedXZ;

    thisx->world.pos.x = thisx->world.pos.x + thisx->velocity.x;
    thisx->world.pos.z = thisx->world.pos.z + thisx->velocity.z;

    if ((thisx->world.pos.x > 1466.0f) && (thisx->world.pos.x < 1673.0f)) {
        thisx->world.pos.y = -129.5f;
    } else {
        posYfac = 1569.0f - thisx->world.pos.x;
        posYfac = fabsf(posYfac) - 103.0f;
        thisx->world.pos.y = ((35.0f / 92.0f) * posYfac) - 129.5f;
    }

    if (this->collider.base.atFlags & AT_HIT) {
        this->collider.base.atFlags &= ~AT_HIT & ~AT_ON;

        relYawTowardsPlayer = thisx->yawTowardsPlayer - thisx->world.rot.y;
        if ((relYawTowardsPlayer > -0x4000) && (relYawTowardsPlayer < 0x4000)) {
            thisx->world.rot.y += 0x8000;
        }

        func_8002F6D4(play, thisx, 2.0f, thisx->yawTowardsPlayer, 0.0f, 0);
        func_8002F7DC(&GET_PLAYER(play)->actor, NA_SE_PL_BODY_HIT);

        this->yOffsetSpeed = 10.0f;
        this->speedFactor = 0.5f;
        this->hasHit = true;
    }

    if (this->hasHit) {
        this->yOffsetSpeed -= 1.5f;
        thisx->shape.yOffset += this->yOffsetSpeed * 10.0f;
        if (thisx->shape.yOffset < 595.0f) {
            thisx->shape.yOffset = 595.0f;
            BgJyaGoroiwa_SetupWait(this);
        }
    } else {
        Math_StepToF(&this->speedFactor, 1.0f, 0.04f);
    }

    if (thisx->world.pos.x > 1745.0f) {
        thisx->world.rot.y = -0x4000;
    } else if (thisx->world.pos.x < 1393.0f) {
        thisx->world.rot.y = 0x4000;
    }

    Audio_PlayActorSound2(thisx, NA_SE_EV_BIGBALL_ROLL - SFX_FLAG);
}

void BgJyaGoroiwa_SetupWait(BgJyaGoroiwa* this) {
    this->actionFunc = BgJyaGoroiwa_Wait;
    this->waitTimer = 0;
}

void BgJyaGoroiwa_Wait(BgJyaGoroiwa* this, PlayState* play) {
    this->waitTimer++;
    if (this->waitTimer > 60) {
        BgJyaGoroiwa_SetupMove(this);
        this->speedFactor = 0.1f;
    }
}

void BgJyaGoroiwa_Update(Actor* thisx, PlayState* play) {
    s32 pad;
    BgJyaGoroiwa* this = (BgJyaGoroiwa*)thisx;
    Player* player = GET_PLAYER(play);
    s32 bgId;
    Vec3f pos;

    if (!(player->stateFlags1 & 0x300000C0)) {
        this->actionFunc(this, play);
        BgJyaGoroiwa_UpdateRotation(this);
        pos.x = this->actor.world.pos.x;
        pos.y = this->actor.world.pos.y + 59.5f;
        pos.z = this->actor.world.pos.z;
        this->actor.floorHeight =
            BgCheck_EntityRaycastFloor4(&play->colCtx, &this->actor.floorPoly, &bgId, &this->actor, &pos);
        BgJyaGoroiwa_UpdateCollider(this);
        if (this->collider.base.atFlags & AT_ON) {
            CollisionCheck_SetAT(play, &play->colChkCtx, &this->collider.base);
        }
        CollisionCheck_SetOC(play, &play->colChkCtx, &this->collider.base);
    }
}

void BgJyaGoroiwa_Draw(Actor* thisx, PlayState* play) {
    Gfx_DrawDListOpa(play, gRollingRockDL);
}
