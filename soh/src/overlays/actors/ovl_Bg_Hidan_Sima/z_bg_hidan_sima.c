/*
 * File: z_bg_hidan_sima.c
 * Overlay: ovl_Bg_Hidan_Sima
 * Description: Stone platform (Fire Temple)
 */

#include "z_bg_hidan_sima.h"
#include "objects/object_hidan_objects/object_hidan_objects.h"

#define FLAGS 0

void BgHidanSima_Init(Actor* thisx, PlayState* play);
void BgHidanSima_Destroy(Actor* thisx, PlayState* play);
void BgHidanSima_Update(Actor* thisx, PlayState* play);
void BgHidanSima_Draw(Actor* thisx, PlayState* play);

void func_8088E518(BgHidanSima* this, PlayState* play);
void func_8088E5D0(BgHidanSima* this, PlayState* play);
void func_8088E6D0(BgHidanSima* this, PlayState* play);
void func_8088E760(BgHidanSima* this, PlayState* play);
void func_8088E7A8(BgHidanSima* this, PlayState* play);
void func_8088E90C(BgHidanSima* this);

const ActorInit Bg_Hidan_Sima_InitVars = {
    ACTOR_BG_HIDAN_SIMA,
    ACTORCAT_BG,
    FLAGS,
    OBJECT_HIDAN_OBJECTS,
    sizeof(BgHidanSima),
    (ActorFunc)BgHidanSima_Init,
    (ActorFunc)BgHidanSima_Destroy,
    (ActorFunc)BgHidanSima_Update,
    (ActorFunc)BgHidanSima_Draw,
    NULL,
};

static ColliderJntSphElementInit sJntSphElementsInit[2] = {
    {
        {
            ELEMTYPE_UNK0,
            { 0x20000000, 0x01, 0x04 },
            { 0x00000000, 0x00, 0x00 },
            TOUCH_ON | TOUCH_SFX_NONE,
            BUMP_NONE,
            OCELEM_NONE,
        },
        { 1, { { 0, 40, 100 }, 22 }, 100 },
    },
    {
        {
            ELEMTYPE_UNK0,
            { 0x20000000, 0x01, 0x04 },
            { 0x00000000, 0x00, 0x00 },
            TOUCH_ON | TOUCH_SFX_NONE,
            BUMP_NONE,
            OCELEM_NONE,
        },
        { 1, { { 0, 40, 145 }, 30 }, 100 },
    },
};

static ColliderJntSphInit sJntSphInit = {
    {
        COLTYPE_NONE,
        AT_ON | AT_TYPE_ENEMY,
        AC_NONE,
        OC1_NONE,
        OC2_TYPE_2,
        COLSHAPE_JNTSPH,
    },
    ARRAY_COUNT(sJntSphElementsInit),
    sJntSphElementsInit,
};

static InitChainEntry sInitChain[] = {
    ICHAIN_VEC3F_DIV1000(scale, 100, ICHAIN_STOP),
};

static void* sFireballsTexs[] = {
    gFireTempleFireball0Tex, gFireTempleFireball1Tex, gFireTempleFireball2Tex, gFireTempleFireball3Tex,
    gFireTempleFireball4Tex, gFireTempleFireball5Tex, gFireTempleFireball6Tex, gFireTempleFireball7Tex,
};

void BgHidanSima_Init(Actor* thisx, PlayState* play) {
    BgHidanSima* this = (BgHidanSima*)thisx;
    s32 pad;
    CollisionHeader* colHeader = NULL;
    s32 i;

    Actor_ProcessInitChain(&this->dyna.actor, sInitChain);
    DynaPolyActor_Init(&this->dyna, DPM_PLAYER);
    if (this->dyna.actor.params == 0) {
        CollisionHeader_GetVirtual(&gFireTempleStonePlatform1Col, &colHeader);
    } else {
        CollisionHeader_GetVirtual(&gFireTempleStonePlatform2Col, &colHeader);
    }
    this->dyna.bgId = DynaPoly_SetBgActor(play, &play->colCtx.dyna, &this->dyna.actor, colHeader);
    Collider_InitJntSph(play, &this->collider);
    Collider_SetJntSph(play, &this->collider, &this->dyna.actor, &sJntSphInit, this->elements);
    for (i = 0; i < ARRAY_COUNT(sJntSphElementsInit); i++) {
        this->collider.elements[i].dim.worldSphere.radius = this->collider.elements[i].dim.modelSphere.radius;
    }
    if (this->dyna.actor.params == 0) {
        this->actionFunc = func_8088E518;
    } else {
        this->actionFunc = func_8088E760;
    }
}

void BgHidanSima_Destroy(Actor* thisx, PlayState* play) {
    BgHidanSima* this = (BgHidanSima*)thisx;

    DynaPoly_DeleteBgActor(play, &play->colCtx.dyna, this->dyna.bgId);
    Collider_DestroyJntSph(play, &this->collider);
}

void func_8088E518(BgHidanSima* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    Math_StepToF(&this->dyna.actor.world.pos.y, this->dyna.actor.home.pos.y, 3.4f);
    if (func_8004356C(&this->dyna) && !(player->stateFlags1 & 0x6000)) {
        this->timer = 20;
        this->dyna.actor.world.rot.y = Camera_GetCamDirYaw(GET_ACTIVE_CAM(play)) + 0x4000;
        if (this->dyna.actor.home.pos.y <= this->dyna.actor.world.pos.y) {
            this->actionFunc = func_8088E5D0;
        } else {
            this->actionFunc = func_8088E6D0;
        }
    }
}

void func_8088E5D0(BgHidanSima* this, PlayState* play) {
    if (this->timer != 0) {
        this->timer--;
    }
    if (this->timer != 0) {
        this->dyna.actor.world.pos.x =
            Math_SinS(this->dyna.actor.world.rot.y + (this->timer * 0x4000)) * 5.0f + this->dyna.actor.home.pos.x;
        this->dyna.actor.world.pos.z =
            Math_CosS(this->dyna.actor.world.rot.y + (this->timer * 0x4000)) * 5.0f + this->dyna.actor.home.pos.z;
    } else {
        this->actionFunc = func_8088E6D0;
        this->dyna.actor.world.pos.x = this->dyna.actor.home.pos.x;
        this->dyna.actor.world.pos.z = this->dyna.actor.home.pos.z;
    }
    if (!(this->timer % 4)) {
        func_800AA000(this->dyna.actor.xyzDistToPlayerSq, 180, 10, 100);
        Audio_PlayActorSound2(&this->dyna.actor, NA_SE_EV_BLOCK_SHAKE);
    }
}

void func_8088E6D0(BgHidanSima* this, PlayState* play) {
    if (func_8004356C(&this->dyna)) {
        this->timer = 20;
    } else if (this->timer != 0) {
        this->timer--;
    }
    Math_StepToF(&this->dyna.actor.world.pos.y, this->dyna.actor.home.pos.y - 100.0f, 1.7f);
    if (this->timer == 0) {
        this->actionFunc = func_8088E518;
    }
}

void func_8088E760(BgHidanSima* this, PlayState* play) {
    if (this->timer != 0) {
        this->timer--;
    }
    if (this->timer == 0) {
        this->dyna.actor.world.rot.y += 0x8000;
        this->timer = 60;
        this->actionFunc = func_8088E7A8;
    }
}

void func_8088E7A8(BgHidanSima* this, PlayState* play) {
    f32 temp;

    if (this->timer != 0) {
        this->timer--;
    }
    if (this->dyna.actor.world.rot.y != this->dyna.actor.home.rot.y) {
        temp = (sinf(((60 - this->timer) * 0.01667 - 0.5) * M_PI) + 1) * 200;
    } else {
        temp = (sinf((this->timer * 0.01667 - 0.5) * M_PI) + 1) * -200;
    }
    this->dyna.actor.world.pos.x = Math_SinS(this->dyna.actor.world.rot.y) * temp + this->dyna.actor.home.pos.x;
    this->dyna.actor.world.pos.z = Math_CosS(this->dyna.actor.world.rot.y) * temp + this->dyna.actor.home.pos.z;
    if (this->timer == 0) {
        this->timer = 20;
        this->actionFunc = func_8088E760;
    }
    func_8002F974(&this->dyna.actor, NA_SE_EV_FIRE_PILLAR - SFX_FLAG);
}

void func_8088E90C(BgHidanSima* this) {
    ColliderJntSphElement* elem;
    s32 i;
    f32 cos = Math_CosS(this->dyna.actor.world.rot.y + 0x8000);
    f32 sin = Math_SinS(this->dyna.actor.world.rot.y + 0x8000);

    for (i = 0; i < 2; i++) {
        elem = &this->collider.elements[i];
        elem->dim.worldSphere.center.x = this->dyna.actor.world.pos.x + sin * elem->dim.modelSphere.center.z;
        elem->dim.worldSphere.center.y = (s16)this->dyna.actor.world.pos.y + elem->dim.modelSphere.center.y;
        elem->dim.worldSphere.center.z = this->dyna.actor.world.pos.z + cos * elem->dim.modelSphere.center.z;
    }
}

void BgHidanSima_Update(Actor* thisx, PlayState* play) {
    BgHidanSima* this = (BgHidanSima*)thisx;
    s32 pad;

    this->actionFunc(this, play);
    if (this->dyna.actor.params != 0) {
        s32 temp = (this->dyna.actor.world.rot.y == this->dyna.actor.shape.rot.y) ? this->timer : (this->timer + 80);

        if (this->actionFunc == func_8088E7A8) {
            temp += 20;
        }
        this->dyna.actor.world.pos.y = this->dyna.actor.home.pos.y - ((1.0f - cosf(temp * (M_PI / 20))) * 5.0f);
        if (this->actionFunc == func_8088E7A8) {
            func_8088E90C(this);
            CollisionCheck_SetAT(play, &play->colChkCtx, &this->collider.base);
        }
    }
}

Gfx* func_8088EB54(PlayState* play, BgHidanSima* this, Gfx* gfx) {
    MtxF mtxF;
    s32 phi_s5;
    s32 s3;
    s32 v0;
    f32 cos;
    f32 sin;
    s32 pad[2];

    Matrix_MtxFCopy(&mtxF, &gMtxFClear);
    cos = Math_CosS(this->dyna.actor.world.rot.y + 0x8000);
    sin = Math_SinS(this->dyna.actor.world.rot.y + 0x8000);

    phi_s5 = (60 - this->timer) >> 1;
    phi_s5 = CLAMP_MAX(phi_s5, 3);

    v0 = 3 - (this->timer >> 1);
    v0 = CLAMP_MIN(v0, 0);

    mtxF.xw = this->dyna.actor.world.pos.x + ((79 - ((this->timer % 6) * 4)) + v0 * 25) * sin;
    mtxF.zw = this->dyna.actor.world.pos.z + ((79 - ((this->timer % 6) * 4)) + v0 * 25) * cos;
    mtxF.yw = this->dyna.actor.world.pos.y + 40.0f;
    mtxF.zz = v0 * 0.4f + 1.0f;
    mtxF.yy = v0 * 0.4f + 1.0f;
    mtxF.xx = v0 * 0.4f + 1.0f;

    for (s3 = v0; s3 < phi_s5; s3++) {
        mtxF.xw += 25.0f * sin;
        mtxF.zw += 25.0f * cos;
        mtxF.xx += 0.4f;
        mtxF.yy += 0.4f;
        mtxF.zz += 0.4f;

        gSPSegment(gfx++, 0x09, SEGMENTED_TO_VIRTUAL(sFireballsTexs[(this->timer + s3) % 7]));
        gSPMatrix(gfx++,
                  Matrix_MtxFToMtx(MATRIX_CHECKFLOATS(&mtxF),
                                   Graph_Alloc(play->state.gfxCtx, sizeof(Mtx))),
                  G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
        gSPDisplayList(gfx++, gFireTempleFireballDL);
    }
    mtxF.xw = this->dyna.actor.world.pos.x + (phi_s5 * 25 + 80) * sin;
    mtxF.zw = this->dyna.actor.world.pos.z + (phi_s5 * 25 + 80) * cos;
    gSPSegment(gfx++, 0x09, SEGMENTED_TO_VIRTUAL(sFireballsTexs[(this->timer + s3) % 7]));
    gSPMatrix(gfx++,
              Matrix_MtxFToMtx(MATRIX_CHECKFLOATS(&mtxF),
                               Graph_Alloc(play->state.gfxCtx, sizeof(Mtx))),
              G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
    gSPDisplayList(gfx++, gFireTempleFireballDL);
    return gfx;
}

void BgHidanSima_Draw(Actor* thisx, PlayState* play) {
    BgHidanSima* this = (BgHidanSima*)thisx;

    OPEN_DISPS(play->state.gfxCtx);
    func_80093D18(play->state.gfxCtx);
    gSPMatrix(POLY_OPA_DISP++, MATRIX_NEWMTX(play->state.gfxCtx),
              G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
    if (this->dyna.actor.params == 0) {
        gSPDisplayList(POLY_OPA_DISP++, gFireTempleStonePlatform1DL);
    } else {
        gSPDisplayList(POLY_OPA_DISP++, gFireTempleStonePlatform2DL);
        if (this->actionFunc == func_8088E7A8) {
            POLY_XLU_DISP = Gfx_CallSetupDL(POLY_XLU_DISP, 0x14);
            gDPSetPrimColor(POLY_XLU_DISP++, 0, 1, 255, 255, 0, 150);
            gDPSetEnvColor(POLY_XLU_DISP++, 255, 0, 0, 255);
            POLY_XLU_DISP = func_8088EB54(play, this, POLY_XLU_DISP);
        }
    }
    CLOSE_DISPS(play->state.gfxCtx);
}
