/*
 * File: z_en_ice_hono.c
 * Overlay: ovl_En_Ice_Hono
 * Description: The various types of Blue Fire
 */

#include "z_en_ice_hono.h"
#include "objects/gameplay_keep/gameplay_keep.h"

#define FLAGS 0

void EnIceHono_Init(Actor* thisx, PlayState* play);
void EnIceHono_Destroy(Actor* thisx, PlayState* play);
void EnIceHono_Update(Actor* thisx, PlayState* play);
void EnIceHono_Draw(Actor* thisx, PlayState* play);

void EnIceHono_CapturableFlame(EnIceHono* this, PlayState* play);
void EnIceHono_DropFlame(EnIceHono* this, PlayState* play);
void EnIceHono_SpreadFlames(EnIceHono* this, PlayState* play);
void EnIceHono_SmallFlameMove(EnIceHono* this, PlayState* play);

void EnIceHono_SetupActionCapturableFlame(EnIceHono* this);
void EnIceHono_SetupActionDroppedFlame(EnIceHono* this);
void EnIceHono_SetupActionSpreadFlames(EnIceHono* this);
void EnIceHono_SetupActionSmallFlame(EnIceHono* this);

const ActorInit En_Ice_Hono_InitVars = {
    ACTOR_EN_ICE_HONO,
    ACTORCAT_ITEMACTION,
    FLAGS,
    OBJECT_GAMEPLAY_KEEP,
    sizeof(EnIceHono),
    (ActorFunc)EnIceHono_Init,
    (ActorFunc)EnIceHono_Destroy,
    (ActorFunc)EnIceHono_Update,
    (ActorFunc)EnIceHono_Draw,
    NULL,
};

static ColliderCylinderInit sCylinderInitCapturableFlame = {
    {
        COLTYPE_NONE,
        AT_NONE,
        AC_NONE,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_2,
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

static ColliderCylinderInit sCylinderInitDroppedFlame = {
    {
        COLTYPE_NONE,
        AT_ON | AT_TYPE_OTHER,
        AC_NONE,
        OC1_ON | OC1_TYPE_2,
        OC2_TYPE_2,
        COLSHAPE_CYLINDER,
    },
    {
        ELEMTYPE_UNK0,
        { 0xFFCFFFFF, 0x00, 0x00 },
        { 0x00000000, 0x00, 0x00 },
        TOUCH_ON | TOUCH_SFX_NORMAL,
        BUMP_NONE,
        OCELEM_ON,
    },
    { 12, 60, 0, { 0, 0, 0 } },
};

static InitChainEntry sInitChainCapturableFlame[] = {
    ICHAIN_U8(targetMode, 0, ICHAIN_CONTINUE),
    ICHAIN_F32(targetArrowOffset, 60, ICHAIN_CONTINUE),
    ICHAIN_F32(uncullZoneForward, 1000, ICHAIN_CONTINUE),
    ICHAIN_F32(uncullZoneScale, 400, ICHAIN_CONTINUE),
    ICHAIN_F32(uncullZoneDownward, 1000, ICHAIN_STOP),
};

static InitChainEntry sInitChainDroppedFlame[] = {
    ICHAIN_F32(uncullZoneForward, 1000, ICHAIN_CONTINUE),
    ICHAIN_F32(uncullZoneScale, 400, ICHAIN_CONTINUE),
    ICHAIN_F32(uncullZoneDownward, 1000, ICHAIN_STOP),
};

static InitChainEntry sInitChainSmallFlame[] = {
    ICHAIN_F32(uncullZoneForward, 1000, ICHAIN_CONTINUE),
    ICHAIN_F32(uncullZoneScale, 400, ICHAIN_CONTINUE),
    ICHAIN_F32(uncullZoneDownward, 1000, ICHAIN_STOP),
};

f32 EnIceHono_XZDistanceSquared(Vec3f* v1, Vec3f* v2) {
    return SQ(v1->x - v2->x) + SQ(v1->z - v2->z);
}

void EnIceHono_InitCapturableFlame(Actor* thisx, PlayState* play) {
    EnIceHono* this = (EnIceHono*)thisx;

    Actor_ProcessInitChain(&this->actor, sInitChainCapturableFlame);
    Actor_SetScale(&this->actor, 0.0074f);
    this->actor.flags |= ACTOR_FLAG_0;
    Actor_SetFocus(&this->actor, 10.0f);

    Collider_InitCylinder(play, &this->collider);
    Collider_SetCylinder(play, &this->collider, &this->actor, &sCylinderInitCapturableFlame);
    Collider_UpdateCylinder(&this->actor, &this->collider);

    this->actor.colChkInfo.mass = MASS_IMMOVABLE;
    EnIceHono_SetupActionCapturableFlame(this);
}

void EnIceHono_InitDroppedFlame(Actor* thisx, PlayState* play) {
    EnIceHono* this = (EnIceHono*)thisx;

    Actor_ProcessInitChain(&this->actor, sInitChainDroppedFlame);
    this->actor.scale.x = this->actor.scale.z = this->actor.scale.y = 0.00002f;
    this->actor.gravity = -0.3f;
    this->actor.minVelocityY = -4.0f;
    this->actor.shape.yOffset = 0.0f;
    this->actor.shape.rot.x = this->actor.shape.rot.y = this->actor.shape.rot.z = this->actor.world.rot.x =
        this->actor.world.rot.y = this->actor.world.rot.z = 0;

    Collider_InitCylinder(play, &this->collider);
    Collider_SetCylinder(play, &this->collider, &this->actor, &sCylinderInitDroppedFlame);
    Collider_UpdateCylinder(&this->actor, &this->collider);

    this->collider.dim.radius = this->actor.scale.x * 4000.4f;
    this->collider.dim.height = this->actor.scale.y * 8000.2f;
    this->actor.colChkInfo.mass = 253;
    EnIceHono_SetupActionDroppedFlame(this);
}

void EnIceHono_InitSmallFlame(Actor* thisx, PlayState* play) {
    EnIceHono* this = (EnIceHono*)thisx;

    Actor_ProcessInitChain(&this->actor, sInitChainSmallFlame);
    this->actor.scale.x = this->actor.scale.z = this->actor.scale.y = 0.0008f;
    this->actor.gravity = -0.3f;
    this->actor.minVelocityY = -4.0f;
    this->actor.shape.yOffset = 0.0f;

    EnIceHono_SetupActionSmallFlame(this);
}

void EnIceHono_Init(Actor* thisx, PlayState* play) {
    EnIceHono* this = (EnIceHono*)thisx;
    s16 params = this->actor.params;

    switch (this->actor.params) {
        case -1:
            EnIceHono_InitCapturableFlame(&this->actor, play);
            break;
        case 0:
            EnIceHono_InitDroppedFlame(&this->actor, play);
            break;
        case 1:
        case 2:
            EnIceHono_InitSmallFlame(&this->actor, play);
            break;
    }

    if ((this->actor.params == -1) || (this->actor.params == 0)) {
        Lights_PointNoGlowSetInfo(&this->lightInfo, this->actor.world.pos.x, (s16)this->actor.world.pos.y + 10,
                                  this->actor.world.pos.z, 155, 210, 255, 0);
        this->lightNode = LightContext_InsertLight(play, &play->lightCtx, &this->lightInfo);
        this->unk_154 = Rand_ZeroOne() * (0x1FFFF / 2.0f);
        this->unk_156 = Rand_ZeroOne() * (0x1FFFF / 2.0f);
        osSyncPrintf("(ice 炎)(arg_data 0x%04x)\n", this->actor.params); // "(ice flame)"
    }
}

void EnIceHono_Destroy(Actor* thisx, PlayState* play) {
    EnIceHono* this = (EnIceHono*)thisx;

    if ((this->actor.params == -1) || (this->actor.params == 0)) {
        LightContext_RemoveLight(play, &play->lightCtx, this->lightNode);
        Collider_DestroyCylinder(play, &this->collider);
    }
}

u32 EnIceHono_InBottleRange(EnIceHono* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    if (this->actor.xzDistToPlayer < 60.0f) {
        Vec3f tempPos;
        tempPos.x = Math_SinS(this->actor.yawTowardsPlayer + 0x8000) * 40.0f + player->actor.world.pos.x;
        tempPos.y = player->actor.world.pos.y;
        tempPos.z = Math_CosS(this->actor.yawTowardsPlayer + 0x8000) * 40.0f + player->actor.world.pos.z;

        //! @bug: this check is superfluous: it is automatically satisfied if the coarse check is satisfied. It may have
        //! been intended to check the actor is in front of Player, but yawTowardsPlayer does not depend on Player's
        //! world rotation.
        if (EnIceHono_XZDistanceSquared(&tempPos, &this->actor.world.pos) <= SQ(40.0f)) {
            return true;
        }
    }
    return false;
}

void EnIceHono_SetupActionCapturableFlame(EnIceHono* this) {
    this->actionFunc = EnIceHono_CapturableFlame;
    this->alpha = 255;
    this->actor.shape.yOffset = -1000.0f;
}

void EnIceHono_CapturableFlame(EnIceHono* this, PlayState* play) {
    if (Actor_HasParent(&this->actor, play)) {
        this->actor.parent = NULL;
    } else if (EnIceHono_InBottleRange(this, play)) {
        // GI_MAX in this case allows the player to catch the actor in a bottle
        func_8002F434(&this->actor, play, GI_MAX, 60.0f, 100.0f);
    }

    if (this->actor.xzDistToPlayer < 200.0f) {
        CollisionCheck_SetOC(play, &play->colChkCtx, &this->collider.base);
    }
    func_8002F8F0(&this->actor, NA_SE_EV_FIRE_PILLAR_S - SFX_FLAG);
}

void EnIceHono_SetupActionDroppedFlame(EnIceHono* this) {
    this->actionFunc = EnIceHono_DropFlame;
    this->timer = 200;
    this->alpha = 255;
}

void EnIceHono_DropFlame(EnIceHono* this, PlayState* play) {
    u32 bgFlag = this->actor.bgCheckFlags & 1;

    Math_StepToF(&this->actor.scale.x, 0.0017f, 0.00008f);
    this->actor.scale.z = this->actor.scale.x;
    Math_StepToF(&this->actor.scale.y, 0.0017f, 0.00008f);

    if (bgFlag != 0) {
        s32 i;
        for (i = 0; i < 8; i++) {
            Actor_Spawn(&play->actorCtx, play, ACTOR_EN_ICE_HONO, this->actor.world.pos.x,
                        this->actor.world.pos.y, this->actor.world.pos.z, 0,
                        ((s32)(Rand_ZeroOne() * 1000.0f) + i * 0x2000) - 0x1F4, 0, 1);
        }
        EnIceHono_SetupActionSpreadFlames(this);
    }
    Actor_MoveForward(&this->actor);
    Actor_UpdateBgCheckInfo(play, &this->actor, 10.0f, this->actor.scale.x * 3500.0f, 0.0f, 5);

    Collider_UpdateCylinder(&this->actor, &this->collider);
    this->collider.dim.radius = this->actor.scale.x * 4000.0f;
    this->collider.dim.height = this->actor.scale.y * 8000.0f;
    CollisionCheck_SetOC(play, &play->colChkCtx, &this->collider.base);

    if (this->timer <= 0) {
        Actor_Kill(&this->actor);
    }
}

void EnIceHono_SetupActionSpreadFlames(EnIceHono* this) {
    this->actionFunc = EnIceHono_SpreadFlames;
    this->timer = 60;
    this->alpha = 255;
}

void EnIceHono_SpreadFlames(EnIceHono* this, PlayState* play) {
    if (this->timer > 20) {
        Math_StepToF(&this->actor.scale.x, 0.011f, 0.00014f);
        Math_StepToF(&this->actor.scale.y, 0.006f, 0.00012f);
    } else {
        Math_StepToF(&this->actor.scale.x, 0.0001f, 0.00015f);
        Math_StepToF(&this->actor.scale.y, 0.0001f, 0.00015f);
    }
    this->actor.scale.z = this->actor.scale.x;
    Actor_MoveForward(&this->actor);
    Actor_UpdateBgCheckInfo(play, &this->actor, 10.0f, this->actor.scale.x * 3500.0f, 0.0f, 4);
    if (this->timer < 25) {
        this->alpha -= 10;
        this->alpha = CLAMP(this->alpha, 0, 255);
    }

    if ((this->alpha > 100) && (this->timer < 40)) {
        Collider_UpdateCylinder(&this->actor, &this->collider);
        this->collider.dim.radius = this->actor.scale.x * 6000.0f;
        this->collider.dim.height = this->actor.scale.y * 8000.0f;
        CollisionCheck_SetAT(play, &play->colChkCtx, &this->collider.base);
    }
    if (this->timer == 46) {
        s32 i;
        for (i = 0; i < 10; i++) {
            s32 rot = i * 0x1999;
            Actor_Spawn(&play->actorCtx, play, ACTOR_EN_ICE_HONO, this->actor.world.pos.x,
                        this->actor.world.pos.y, this->actor.world.pos.z, 0,
                        ((s32)(Rand_ZeroOne() * 1000.0f) + rot) - 0x1F4, 0, 2);
        }
    }

    if (this->timer <= 0) {
        Actor_Kill(&this->actor);
    }
}

void EnIceHono_SetupActionSmallFlame(EnIceHono* this) {
    this->actionFunc = EnIceHono_SmallFlameMove;
    this->timer = 44;
    this->alpha = 255;
    if (this->actor.params == 1) {
        this->smallFlameTargetYScale = (Rand_ZeroOne() * 0.005f) + 0.004f;
        this->actor.speedXZ = (Rand_ZeroOne() * 1.6f) + 0.5f;
    } else {
        this->smallFlameTargetYScale = (Rand_ZeroOne() * 0.005f) + 0.003f;
        this->actor.speedXZ = (Rand_ZeroOne() * 2.0f) + 0.5f;
    }
}

void EnIceHono_SmallFlameMove(EnIceHono* this, PlayState* play) {
    if (this->timer > 20) {
        Math_StepToF(&this->actor.scale.x, 0.006f, 0.00016f);
        Math_StepToF(&this->actor.scale.y, this->smallFlameTargetYScale * 0.667f, 0.00014f);
    } else {
        Math_StepToF(&this->actor.scale.x, 0.0001f, 0.00015f);
        Math_StepToF(&this->actor.scale.y, 0.0001f, 0.00015f);
    }
    this->actor.scale.z = this->actor.scale.x;
    Math_StepToF(&this->actor.speedXZ, 0, 0.06f);
    Actor_MoveForward(&this->actor);
    Actor_UpdateBgCheckInfo(play, &this->actor, 10.0f, 10.0f, 0.0f, 5);

    if (this->timer < 25) {
        this->alpha -= 10;
        this->alpha = CLAMP(this->alpha, 0, 255);
    }
    if (this->timer <= 0) {
        Actor_Kill(&this->actor);
    }
}

void EnIceHono_Update(Actor* thisx, PlayState* play) {
    EnIceHono* this = (EnIceHono*)thisx;
    s32 pad1;
    f32 intensity;
    s32 pad2;
    f32 sin154;
    f32 sin156;

    if (this->timer > 0) {
        this->timer--;
    }
    if (this->actor.params == 0) {
        func_8002F8F0(&this->actor, NA_SE_IT_FLAME - SFX_FLAG);
    }
    if ((this->actor.params == -1) || (this->actor.params == 0)) {
        this->unk_154 += 0x1111;
        this->unk_156 += 0x4000;
        sin156 = Math_SinS(this->unk_156);
        sin154 = Math_SinS(this->unk_154);
        intensity = (Rand_ZeroOne() * 0.05f) + ((sin154 * 0.125f) + (sin156 * 0.1f)) + 0.425f;
        if ((intensity > 0.7f) || (intensity < 0.2f)) {
            osSyncPrintf("ありえない値(ratio = %f)\n", intensity); // "impossible value(ratio = %f)"
        }
        Lights_PointNoGlowSetInfo(&this->lightInfo, this->actor.world.pos.x, (s16)this->actor.world.pos.y + 10,
                                  this->actor.world.pos.z, (s32)(155.0f * intensity), (s32)(210.0f * intensity),
                                  (s32)(255.0f * intensity), 1400);
    }

    if (this->actionFunc != NULL) {
        this->actionFunc(this, play);
    }
}

void EnIceHono_Draw(Actor* thisx, PlayState* play) {
    EnIceHono* this = (EnIceHono*)thisx;
    u32 pad;

    OPEN_DISPS(play->state.gfxCtx);
    func_80093D84(play->state.gfxCtx);

    gSPSegment(POLY_XLU_DISP++, 0x08,
               Gfx_TwoTexScroll(play->state.gfxCtx, 0, 0, 0, 32, 64, 1, 0, (play->state.frames * -20) % 512,
                                32, 128));

    gDPSetPrimColor(POLY_XLU_DISP++, 0x80, 0x80, 170, 255, 255, this->alpha);

    gDPSetEnvColor(POLY_XLU_DISP++, 0, 150, 255, 0);

    Matrix_RotateY((s16)(Camera_GetCamDirYaw(GET_ACTIVE_CAM(play)) - this->actor.shape.rot.y + 0x8000) *
                       (M_PI / 0x8000),
                   MTXMODE_APPLY);

    gSPMatrix(POLY_XLU_DISP++, MATRIX_NEWMTX(play->state.gfxCtx),
              G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
    gSPDisplayList(POLY_XLU_DISP++, gEffFire1DL);

    CLOSE_DISPS(play->state.gfxCtx);
}
