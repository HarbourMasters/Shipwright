/*
 * File: z_bg_ice_turara.c
 * Overlay: ovl_Bg_Ice_Turara
 * Description: Icicles
 */

#include "z_bg_ice_turara.h"
#include "objects/object_ice_objects/object_ice_objects.h"

#define FLAGS 0

void BgIceTurara_Init(Actor* thisx, PlayState* play);
void BgIceTurara_Destroy(Actor* thisx, PlayState* play);
void BgIceTurara_Update(Actor* thisx, PlayState* play);
void BgIceTurara_Draw(Actor* thisx, PlayState* play);

void BgIceTurara_Stalagmite(BgIceTurara* this, PlayState* play);
void BgIceTurara_Wait(BgIceTurara* this, PlayState* play);
void BgIceTurara_Shiver(BgIceTurara* this, PlayState* play);
void BgIceTurara_Fall(BgIceTurara* this, PlayState* play);
void BgIceTurara_Regrow(BgIceTurara* this, PlayState* play);

static ColliderCylinderInit sCylinderInit = {
    {
        COLTYPE_NONE,
        AT_ON | AT_TYPE_ENEMY,
        AC_ON | AC_TYPE_PLAYER,
        OC1_NONE,
        OC2_TYPE_2,
        COLSHAPE_CYLINDER,
    },
    {
        ELEMTYPE_UNK0,
        { 0xFFCFFFFF, 0x00, 0x04 },
        { 0x4FC007CA, 0x00, 0x00 },
        TOUCH_ON | TOUCH_SFX_NORMAL,
        BUMP_ON,
        OCELEM_NONE,
    },
    { 13, 120, 0, { 0, 0, 0 } },
};

const ActorInit Bg_Ice_Turara_InitVars = {
    ACTOR_BG_ICE_TURARA,
    ACTORCAT_PROP,
    FLAGS,
    OBJECT_ICE_OBJECTS,
    sizeof(BgIceTurara),
    (ActorFunc)BgIceTurara_Init,
    (ActorFunc)BgIceTurara_Destroy,
    (ActorFunc)BgIceTurara_Update,
    (ActorFunc)BgIceTurara_Draw,
    NULL,
};

static InitChainEntry sInitChain[] = {
    ICHAIN_F32(uncullZoneScale, 600, ICHAIN_CONTINUE),
    ICHAIN_F32(gravity, -3, ICHAIN_CONTINUE),
    ICHAIN_F32(minVelocityY, -30, ICHAIN_CONTINUE),
    ICHAIN_VEC3F_DIV1000(scale, 100, ICHAIN_STOP),
};

void BgIceTurara_Init(Actor* thisx, PlayState* play) {
    BgIceTurara* this = (BgIceTurara*)thisx;
    s32 pad;
    CollisionHeader* colHeader = NULL;

    Actor_ProcessInitChain(&this->dyna.actor, sInitChain);
    DynaPolyActor_Init(&this->dyna, DPM_UNK);
    CollisionHeader_GetVirtual(&object_ice_objects_Col_002594, &colHeader);
    Collider_InitCylinder(play, &this->collider);
    Collider_SetCylinder(play, &this->collider, &this->dyna.actor, &sCylinderInit);
    Collider_UpdateCylinder(&this->dyna.actor, &this->collider);
    this->dyna.bgId = DynaPoly_SetBgActor(play, &play->colCtx.dyna, &this->dyna.actor, colHeader);
    if (this->dyna.actor.params == TURARA_STALAGMITE) {
        this->actionFunc = BgIceTurara_Stalagmite;
    } else {
        this->dyna.actor.shape.rot.x = -0x8000;
        this->dyna.actor.shape.yOffset = 1200.0f;
        this->actionFunc = BgIceTurara_Wait;
    }
}

void BgIceTurara_Destroy(Actor* thisx, PlayState* play) {
    BgIceTurara* this = (BgIceTurara*)thisx;

    DynaPoly_DeleteBgActor(play, &play->colCtx.dyna, this->dyna.bgId);
    Collider_DestroyCylinder(play, &this->collider);
}

void BgIceTurara_Break(BgIceTurara* this, PlayState* play, f32 arg2) {
    static Vec3f accel = { 0.0f, -1.0f, 0.0f };
    static Color_RGBA8 primColor = { 170, 255, 255, 255 };
    static Color_RGBA8 envColor = { 0, 50, 100, 255 };
    Vec3f vel;
    Vec3f pos;
    s32 j;
    s32 i;

    SoundSource_PlaySfxAtFixedWorldPos(play, &this->dyna.actor.world.pos, 30, NA_SE_EV_ICE_BROKEN);
    for (i = 0; i < 2; i++) {
        for (j = 0; j < 10; j++) {
            pos.x = this->dyna.actor.world.pos.x + Rand_CenteredFloat(8.0f);
            pos.y = this->dyna.actor.world.pos.y + (Rand_ZeroOne() * arg2) + (i * arg2);
            pos.z = this->dyna.actor.world.pos.z + Rand_CenteredFloat(8.0f);

            vel.x = Rand_CenteredFloat(7.0f);
            vel.z = Rand_CenteredFloat(7.0f);
            vel.y = (Rand_ZeroOne() * 4.0f) + 8.0f;

            EffectSsEnIce_Spawn(play, &pos, (Rand_ZeroOne() * 0.2f) + 0.1f, &vel, &accel, &primColor, &envColor,
                                30);
        }
    }
}

void BgIceTurara_Stalagmite(BgIceTurara* this, PlayState* play) {
    if (this->collider.base.acFlags & AC_HIT) {
        BgIceTurara_Break(this, play, 50.0f);
        Actor_Kill(&this->dyna.actor);
        return;
    }
    CollisionCheck_SetAC(play, &play->colChkCtx, &this->collider.base);
}

void BgIceTurara_Wait(BgIceTurara* this, PlayState* play) {
    if (this->dyna.actor.xzDistToPlayer < 60.0f) {
        this->shiverTimer = 10;
        this->actionFunc = BgIceTurara_Shiver;
    }
}

void BgIceTurara_Shiver(BgIceTurara* this, PlayState* play) {
    s16 phi_v0_3;
    s16 phi_v0_2;
    f32 sp28;

    if (this->shiverTimer != 0) {
        this->shiverTimer--;
    }
    if (!(this->shiverTimer % 4)) {
        Audio_PlayActorSound2(&this->dyna.actor, NA_SE_EV_ICE_SWING);
    }
    if (this->shiverTimer == 0) {
        this->dyna.actor.world.pos.x = this->dyna.actor.home.pos.x;
        this->dyna.actor.world.pos.z = this->dyna.actor.home.pos.z;
        Collider_UpdateCylinder(&this->dyna.actor, &this->collider);
        CollisionCheck_SetAT(play, &play->colChkCtx, &this->collider.base);
        func_8003EBF8(play, &play->colCtx.dyna, this->dyna.bgId);
        this->actionFunc = BgIceTurara_Fall;
    } else {
        sp28 = Rand_ZeroOne();
        phi_v0_2 = (Rand_ZeroOne() < 0.5f ? -1 : 1);
        this->dyna.actor.world.pos.x = (phi_v0_2 * ((0.5f * sp28) + 0.5f)) + this->dyna.actor.home.pos.x;
        sp28 = Rand_ZeroOne();
        phi_v0_3 = (Rand_ZeroOne() < 0.5f ? -1 : 1);
        this->dyna.actor.world.pos.z = (phi_v0_3 * ((0.5f * sp28) + 0.5f)) + this->dyna.actor.home.pos.z;
    }
}

void BgIceTurara_Fall(BgIceTurara* this, PlayState* play) {
    if ((this->collider.base.atFlags & AT_HIT) || (this->dyna.actor.bgCheckFlags & 1)) {
        this->collider.base.atFlags &= ~AT_HIT;
        this->dyna.actor.bgCheckFlags &= ~1;
        if (this->dyna.actor.world.pos.y < this->dyna.actor.floorHeight) {
            this->dyna.actor.world.pos.y = this->dyna.actor.floorHeight;
        }
        BgIceTurara_Break(this, play, 40.0f);
        if (this->dyna.actor.params == TURARA_STALACTITE_REGROW) {
            this->dyna.actor.world.pos.y = this->dyna.actor.home.pos.y + 120.0f;
            func_8003EC50(play, &play->colCtx.dyna, this->dyna.bgId);
            this->actionFunc = BgIceTurara_Regrow;
        } else {
            Actor_Kill(&this->dyna.actor);
            return;
        }
    } else {
        Actor_MoveForward(&this->dyna.actor);
        this->dyna.actor.world.pos.y += 40.0f;
        Actor_UpdateBgCheckInfo(play, &this->dyna.actor, 0.0f, 0.0f, 0.0f, 4);
        this->dyna.actor.world.pos.y -= 40.0f;
        Collider_UpdateCylinder(&this->dyna.actor, &this->collider);
        CollisionCheck_SetAT(play, &play->colChkCtx, &this->collider.base);
    }
}

void BgIceTurara_Regrow(BgIceTurara* this, PlayState* play) {
    if (Math_StepToF(&this->dyna.actor.world.pos.y, this->dyna.actor.home.pos.y, 1.0f)) {
        this->actionFunc = BgIceTurara_Wait;
        this->dyna.actor.velocity.y = 0.0f;
    }
}

void BgIceTurara_Update(Actor* thisx, PlayState* play) {
    BgIceTurara* this = (BgIceTurara*)thisx;

    this->actionFunc(this, play);
}

void BgIceTurara_Draw(Actor* thisx, PlayState* play) {
    Gfx_DrawDListOpa(play, object_ice_objects_DL_0023D0);
}
