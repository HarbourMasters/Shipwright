/*
 * File: z_bg_spout_fire.c
 * Overlay: ovl_Bg_Spout_Fire
 * Description: Proximity-Activated Fire Wall Spawner
 */

#include "z_bg_spout_fire.h"
#include "objects/object_fwall/object_fwall.h"

#define FLAGS 0x00000000

void BgSpoutFire_Init(Actor* thisx, PlayState* play);
void BgSpoutFire_Destroy(Actor* thisx, PlayState* play);
void BgSpoutFire_Update(Actor* thisx, PlayState* play);
void BgSpoutFire_Draw(Actor* thisx, PlayState* play);

s32 func_80A60C24(BgSpoutFire* this, PlayState* play);
void func_80A60C94(BgSpoutFire* this, PlayState* play);
void func_80A60CDC(BgSpoutFire* this, PlayState* play);
void func_80A60D10(BgSpoutFire* this, PlayState* play);
void func_80A60E08(BgSpoutFire* this, PlayState* play);

ActorProfile Bg_Spout_Fire_Profile = {
    /**/ ACTOR_BG_SPOUT_FIRE,
    /**/ ACTORCAT_BG,
    /**/ FLAGS,
    /**/ OBJECT_FWALL,
    /**/ sizeof(BgSpoutFire),
    /**/ BgSpoutFire_Init,
    /**/ BgSpoutFire_Destroy,
    /**/ BgSpoutFire_Update,
    /**/ NULL,
};

static ColliderCylinderInit sCylinderInit = {
    {
        COL_MATERIAL_NONE,
        AT_ON | AT_TYPE_ENEMY,
        AC_NONE,
        OC1_ON | OC1_TYPE_PLAYER,
        OC2_TYPE_2,
        COLSHAPE_CYLINDER,
    },
    {
        ELEM_MATERIAL_UNK0,
        { 0x20000000, 0x01, 0x04 },
        { 0xF7CFFFFF, 0x00, 0x00 },
        ATELEM_ON | ATELEM_SFX_NONE,
        ACELEM_NONE,
        OCELEM_ON,
    },
    { 30, 83, 0, { 0, 0, 0 } },
};

static CollisionCheckInfoInit sColChkInfoInit = { 1, 80, 100, MASS_IMMOVABLE };

static TexturePtr sFlameTextures[] = {
    gFwallFireball0Tex, gFwallFireball1Tex, gFwallFireball2Tex, gFwallFireball3Tex,
    gFwallFireball4Tex, gFwallFireball5Tex, gFwallFireball6Tex, gFwallFireball7Tex,
};

static s32 sTexturesDesegmented = false;

void BgSpoutFire_Init(Actor* thisx, PlayState* play) {
    s32 i;
    BgSpoutFire* this = (BgSpoutFire*)thisx;

    this->actor.scale.z = 1350.0f * 0.0001f;
    this->actor.scale.x = 1350.0f * 0.0001f;
    this->actor.scale.y = 0.01f;
    this->flameTexIndex = 0;
    Collider_InitAndSetCylinder(play, &this->collider, &this->actor, &sCylinderInit);
    this->collider.dim.pos.y = this->actor.world.pos.y;
    CollisionCheck_SetInfo(&this->actor.colChkInfo, NULL, &sColChkInfoInit);

    if (!sTexturesDesegmented) {
        for (i = 0; i < ARRAY_COUNT(sFlameTextures); i++) {
            sFlameTextures[i] = Lib_SegmentedToVirtual(sFlameTextures[i]);
        }
        sTexturesDesegmented = true;
    }
    this->actionFunc = func_80A60C94;
}

void BgSpoutFire_Destroy(Actor* thisx, PlayState* play) {
    BgSpoutFire* this = (BgSpoutFire*)thisx;

    Collider_DestroyCylinder(play, &this->collider);
}

s32 func_80A60C24(BgSpoutFire* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    Vec3f sp18;

    Actor_WorldToActorCoords(&this->actor, &sp18, &player->actor.world.pos);
    if ((fabsf(sp18.x) < 100.0f) && (fabsf(sp18.z) < 120.0f)) {
        return true;
    } else {
        return false;
    }
}

void func_80A60C94(BgSpoutFire* this, PlayState* play) {
    if (func_80A60C24(this, play)) {
        this->actor.draw = BgSpoutFire_Draw;
        this->timer = 5;
        this->actionFunc = func_80A60CDC;
    }
}

void func_80A60CDC(BgSpoutFire* this, PlayState* play) {
    if (this->timer != 0) {
        this->timer--;
    }
    if (this->timer == 0) {
        this->actionFunc = func_80A60D10;
    }
}

void func_80A60D10(BgSpoutFire* this, PlayState* play) {
    if (func_80A60C24(this, play)) {
        Math_StepToF(&this->actor.scale.y, 0.1f, 2.5f * 0.01f);
    } else if (Math_StepToF(&this->actor.scale.y, 0.01f, 0.01f)) {
        this->actor.draw = NULL;
        this->actionFunc = func_80A60C94;
    } else {
        this->timer = 0;
    }
}

void func_80A60DA0(BgSpoutFire* this, PlayState* play) {
    s16 phi_a3;

    if (Actor_IsFacingPlayer(&this->actor, 0x4000)) {
        phi_a3 = this->actor.shape.rot.y;
    } else {
        phi_a3 = (this->actor.shape.rot.y + 0x8000);
    }
    func_800B8D98(play, &this->actor, 5.0f, phi_a3, 1.0f);
}

void func_80A60E08(BgSpoutFire* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    Vec3f sp30;
    f32 cos;
    f32 sin;

    Actor_WorldToActorCoords(&this->actor, &sp30, &player->actor.world.pos);
    sp30.x = CLAMP(sp30.x, -74.25f, 74.25f);
    if (this->timer == 0) {
        if (sp30.z > 0.0f) {
            sp30.z = -25.0f;
            this->timer = -1;
        } else {
            sp30.z = 25.0f;
            this->timer = 1;
        }
    } else {
        sp30.z = this->timer * 25.0f;
    }
    sin = Math_SinS(this->actor.shape.rot.y);
    cos = Math_CosS(this->actor.shape.rot.y);
    this->collider.dim.pos.x = this->actor.world.pos.x + (sp30.x * cos) + (sp30.z * sin);
    this->collider.dim.pos.z = this->actor.world.pos.z - (sp30.x * sin) + (sp30.z * cos);
}

void BgSpoutFire_Update(Actor* thisx, PlayState* play) {
    s32 pad;
    BgSpoutFire* this = (BgSpoutFire*)thisx;

    this->flameTexIndex = (this->flameTexIndex + 1) % 8;
    if ((this->collider.base.atFlags & AT_HIT)) {
        this->collider.base.atFlags &= ~AT_HIT;
        func_80A60DA0(this, play);
    }
    this->actionFunc(this, play);
    if (this->actionFunc == func_80A60D10) {
        func_80A60E08(this, play);
        CollisionCheck_SetAT(play, &play->colChkCtx, &this->collider.base);
        CollisionCheck_SetOC(play, &play->colChkCtx, &this->collider.base);
        Actor_PlaySfx_Flagged(&this->actor, NA_SE_EV_FIRE_PLATE - SFX_FLAG);
    }
}

void BgSpoutFire_Draw(Actor* thisx, PlayState* play) {
    BgSpoutFire* this = (BgSpoutFire*)thisx;
    Gfx* gfx;

    OPEN_DISPS(play->state.gfxCtx);

    gfx = Gfx_SetupDL(POLY_XLU_DISP, SETUPDL_20);
    POLY_XLU_DISP = gfx;
    gSPSegment(&gfx[0], 0x08, sFlameTextures[this->flameTexIndex]);
    gDPSetPrimColor(&gfx[1], 0, 1, 255, 255, 0, 150);
    gDPSetEnvColor(&gfx[2], 255, 0, 0, 255);
    Matrix_Translate(-55.0f, 0.0f, 0.0f, MTXMODE_APPLY);
    MATRIX_FINALIZE_AND_LOAD(&gfx[3], play->state.gfxCtx);
    gSPDisplayList(&gfx[4], object_fwall_DL_000040);
    POLY_XLU_DISP = &gfx[5];

    CLOSE_DISPS(play->state.gfxCtx);
}
