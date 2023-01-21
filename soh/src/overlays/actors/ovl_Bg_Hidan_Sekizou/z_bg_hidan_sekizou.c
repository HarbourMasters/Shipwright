/*
 * File: z_bg_hidan_sekizou.c
 * Overlay: ovl_Bg_Hidan_Sekizou
 * Description: Stationary flame thrower statue
 */

#include "z_bg_hidan_sekizou.h"
#include "objects/object_hidan_objects/object_hidan_objects.h"

#define FLAGS 0

void BgHidanSekizou_Init(Actor* thisx, PlayState* play);
void BgHidanSekizou_Destroy(Actor* thisx, PlayState* play);
void BgHidanSekizou_Update(Actor* thisx, PlayState* play);
void BgHidanSekizou_Draw(Actor* thisx, PlayState* play);

void func_8088D434(BgHidanSekizou* this, PlayState* play);
void func_8088D720(BgHidanSekizou* this, PlayState* play);

const ActorInit Bg_Hidan_Sekizou_InitVars = {
    ACTOR_BG_HIDAN_SEKIZOU,
    ACTORCAT_BG,
    FLAGS,
    OBJECT_HIDAN_OBJECTS,
    sizeof(BgHidanSekizou),
    (ActorFunc)BgHidanSekizou_Init,
    (ActorFunc)BgHidanSekizou_Destroy,
    (ActorFunc)BgHidanSekizou_Update,
    (ActorFunc)BgHidanSekizou_Draw,
    NULL,
};

static ColliderJntSphElementInit sJntSphElementsInit[6] = {
    {
        {
            ELEMTYPE_UNK0,
            { 0x20000000, 0x01, 0x04 },
            { 0x00000000, 0x00, 0x00 },
            TOUCH_ON | TOUCH_SFX_NONE,
            BUMP_NONE,
            OCELEM_ON,
        },
        { 1, { { 0, 30, 40 }, 23 }, 100 },
    },
    {
        {
            ELEMTYPE_UNK0,
            { 0x20000000, 0x01, 0x04 },
            { 0x00000000, 0x00, 0x00 },
            TOUCH_ON | TOUCH_SFX_NONE,
            BUMP_NONE,
            OCELEM_ON,
        },
        { 1, { { 0, 32, 87 }, 30 }, 100 },
    },
    {
        {
            ELEMTYPE_UNK0,
            { 0x20000000, 0x01, 0x04 },
            { 0x00000000, 0x00, 0x00 },
            TOUCH_ON | TOUCH_SFX_NONE,
            BUMP_NONE,
            OCELEM_ON,
        },
        { 1, { { 0, 35, 150 }, 40 }, 100 },
    },
    {
        {
            ELEMTYPE_UNK0,
            { 0x20000000, 0x01, 0x04 },
            { 0x00000000, 0x00, 0x00 },
            TOUCH_ON | TOUCH_SFX_NONE,
            BUMP_NONE,
            OCELEM_ON,
        },
        { 1, { { 0, 30, 40 }, 23 }, 100 },
    },
    {
        {
            ELEMTYPE_UNK0,
            { 0x20000000, 0x01, 0x04 },
            { 0x00000000, 0x00, 0x00 },
            TOUCH_ON | TOUCH_SFX_NONE,
            BUMP_NONE,
            OCELEM_ON,
        },
        { 1, { { 0, 32, 87 }, 30 }, 100 },
    },
    {
        {
            ELEMTYPE_UNK0,
            { 0x20000000, 0x01, 0x04 },
            { 0x00000000, 0x00, 0x00 },
            TOUCH_ON | TOUCH_SFX_NONE,
            BUMP_NONE,
            OCELEM_ON,
        },
        { 1, { { 0, 35, 150 }, 40 }, 100 },
    },
};

static ColliderJntSphInit sJntSphInit = {
    {
        COLTYPE_NONE,
        AT_ON | AT_TYPE_ENEMY,
        AC_NONE,
        OC1_ON | OC1_TYPE_PLAYER,
        OC2_TYPE_2,
        COLSHAPE_JNTSPH,
    },
    6,
    sJntSphElementsInit,
};

static CollisionCheckInfoInit sColChkInfoInit = { 1, 40, 240, MASS_IMMOVABLE };

static InitChainEntry sInitChain[] = {
    ICHAIN_VEC3F_DIV1000(scale, 100, ICHAIN_CONTINUE),
    ICHAIN_F32(uncullZoneScale, 400, ICHAIN_CONTINUE),
    ICHAIN_F32(uncullZoneForward, 1500, ICHAIN_STOP),
};

static void* sFireballsTexs[] = {
    gFireTempleFireball0Tex, gFireTempleFireball1Tex, gFireTempleFireball2Tex, gFireTempleFireball3Tex,
    gFireTempleFireball4Tex, gFireTempleFireball5Tex, gFireTempleFireball6Tex, gFireTempleFireball7Tex,
};

void func_8088CEC0(BgHidanSekizou* this, s32 arg1, s16 arg2) {
    s32 i;
    s32 start = arg1 * 3;
    s32 end = start + 3;
    f32 sp30 = Math_SinS(arg2);
    f32 sp2C = Math_CosS(arg2);

    for (i = start; i < end; i++) {
        ColliderJntSphElement* element = &this->collider.elements[i];

        element->dim.worldSphere.center.x = this->dyna.actor.home.pos.x + (sp2C * element->dim.modelSphere.center.x) +
                                            (sp30 * element->dim.modelSphere.center.z);
        element->dim.worldSphere.center.y = (s16)this->dyna.actor.home.pos.y + element->dim.modelSphere.center.y;
        element->dim.worldSphere.center.z = this->dyna.actor.home.pos.z - (sp30 * element->dim.modelSphere.center.x) +
                                            (sp2C * element->dim.modelSphere.center.z);
        element->info.toucherFlags |= TOUCH_ON;
        element->info.ocElemFlags |= OCELEM_ON;
    }
}

void BgHidanSekizou_Init(Actor* thisx, PlayState* play) {
    s32 pad;
    BgHidanSekizou* this = (BgHidanSekizou*)thisx;
    s32 i;
    CollisionHeader* colHeader = NULL;

    Actor_ProcessInitChain(&this->dyna.actor, sInitChain);
    DynaPolyActor_Init(&this->dyna, DPM_UNK);
    Collider_InitJntSph(play, &this->collider);
    Collider_SetJntSph(play, &this->collider, &this->dyna.actor, &sJntSphInit, this->elements);
    for (i = 0; i < ARRAY_COUNT(this->elements); i++) {
        this->collider.elements[i].dim.worldSphere.radius = this->collider.elements[i].dim.modelSphere.radius;
    }
    if (this->dyna.actor.params == 0) {
        this->unk_168[0] = 36;
        for (i = 0; i < 2; i++) {
            func_8088CEC0(this, i, this->dyna.actor.shape.rot.y + ((i == 0) ? 0x2000 : -0x2000));
        }
        CollisionHeader_GetVirtual(&gFireTempleStationaryFlamethrowerShortCol, &colHeader);
        this->updateFunc = func_8088D720;
    } else {
        this->unk_168[0] = this->unk_168[1] = this->unk_168[2] = this->unk_168[3] = 0;
        CollisionHeader_GetVirtual(&gFireTempleStationaryFlamethrowerTallCol, &colHeader);
        this->updateFunc = func_8088D434;
    }
    this->unk_170 = 0;
    this->dyna.bgId = DynaPoly_SetBgActor(play, &play->colCtx.dyna, &this->dyna.actor, colHeader);
    CollisionCheck_SetInfo(&this->dyna.actor.colChkInfo, NULL, &sColChkInfoInit);
}

void BgHidanSekizou_Destroy(Actor* thisx, PlayState* play2) {
    PlayState* play = play2;
    BgHidanSekizou* this = (BgHidanSekizou*)thisx;

    DynaPoly_DeleteBgActor(play, &play->colCtx.dyna, this->dyna.bgId);
    Collider_DestroyJntSph(play, &this->collider);
}

void func_8088D434(BgHidanSekizou* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    s32 i;
    s32 isAligned[2];
    s32 isClose;
    s32 phi_s4;

    isClose = this->dyna.actor.xzDistToPlayer < 300.0f;
    isAligned[0] = fabsf(this->dyna.actor.world.pos.x - player->actor.world.pos.x) < 80.0f;
    isAligned[1] = fabsf(this->dyna.actor.world.pos.z - player->actor.world.pos.z) < 80.0f;
    phi_s4 = 0;
    for (i = 0; i < 4; i++) {
        s16 diff;
        s16* temp = &this->unk_168[i];

        DECR(*temp);
        diff = this->dyna.actor.yawTowardsPlayer - i * 0x4000;
        if (isAligned[i % 2] && isClose) {
            if (ABS(diff) <= 0x4000) {
                if (*temp < 4) {
                    *temp = 35 - *temp;
                }
                func_8088CEC0(this, (phi_s4 > 1) ? 1 : phi_s4, this->dyna.actor.shape.rot.y + i * 0x4000);
                phi_s4++;
            }
        }
    }
    for (i = 3 * phi_s4; i < ARRAY_COUNT(this->elements); i++) {
        this->collider.elements[i].info.toucherFlags &= ~TOUCH_ON;
        this->collider.elements[i].info.ocElemFlags &= ~OCELEM_ON;
    }
}

void func_8088D720(BgHidanSekizou* this, PlayState* play) {
    this->unk_168[0]--;
    if (this->unk_168[0] <= -36) {
        this->unk_168[0] = 36;
    }
}

void func_8088D750(BgHidanSekizou* this, PlayState* play) {
    s16 phi_a3;

    if (this->dyna.actor.xzDistToPlayer > 200.0f) {
        phi_a3 = this->dyna.actor.yawTowardsPlayer;
    } else if (this->dyna.actor.params == 0) {
        phi_a3 = this->dyna.actor.yawTowardsPlayer - this->dyna.actor.shape.rot.y;
        if (phi_a3 > 0x2000) {
            phi_a3 = this->dyna.actor.shape.rot.y + 0x6000;
        } else if (phi_a3 < -0x2000) {
            phi_a3 = this->dyna.actor.shape.rot.y - 0x6000;
        } else if (phi_a3 > 0) {
            phi_a3 = this->dyna.actor.shape.rot.y - 0x2000;
        } else {
            phi_a3 = this->dyna.actor.shape.rot.y + 0x2000;
        }
    } else {
        phi_a3 = this->dyna.actor.yawTowardsPlayer;
        if (phi_a3 > 0x6000) {
            phi_a3 = 0x4000;
        } else if (phi_a3 > 0x4000) {
            phi_a3 = -0x8000;
        } else if (phi_a3 > 0x2000) {
            phi_a3 = 0;
        } else if (phi_a3 > 0) {
            phi_a3 = 0x4000;
        } else if (phi_a3 < -0x6000) {
            phi_a3 = -0x4000;
        } else if (phi_a3 < -0x4000) {
            phi_a3 = -0x8000;
        } else if (phi_a3 < -0x2000) {
            phi_a3 = 0;
        } else {
            phi_a3 = -0x4000;
        }
    }
    func_8002F71C(play, &this->dyna.actor, 5.0f, phi_a3, 1.0f);
}

void BgHidanSekizou_Update(Actor* thisx, PlayState* play2) {
    PlayState* play = play2;
    BgHidanSekizou* this = (BgHidanSekizou*)thisx;

    if (this->unk_170 != 0) {
        this->unk_170--;
    }
    if (this->unk_170 == 0) {
        this->unk_170 = 4;
    }

    if (this->collider.base.atFlags & AT_HIT) {
        this->collider.base.atFlags &= ~AT_HIT;
        func_8088D750(this, play);
    }

    this->updateFunc(this, play);

    if (this->dyna.actor.params == 0) {
        if (this->unk_168[0] > 0) {
            CollisionCheck_SetAT(play, &play->colChkCtx, &this->collider.base);
            CollisionCheck_SetOC(play, &play->colChkCtx, &this->collider.base);
            func_8002F974(&this->dyna.actor, NA_SE_EV_FIRE_PILLAR - SFX_FLAG);
        }
    } else {
        if ((this->unk_168[0] > 0) || (this->unk_168[1] > 0) || (this->unk_168[2] > 0) || (this->unk_168[3] > 0)) {
            CollisionCheck_SetAT(play, &play->colChkCtx, &this->collider.base);
            CollisionCheck_SetOC(play, &play->colChkCtx, &this->collider.base);
            func_8002F974(&this->dyna.actor, NA_SE_EV_FIRE_PILLAR - SFX_FLAG);
        }
    }
}

Gfx* func_8088D9F4(PlayState* play, BgHidanSekizou* this, s16 arg2, MtxF* arg3, f32 arg4, f32 arg5, s16 arg6,
                   Gfx* arg7) {
    f32 temp_f0;
    f32 temp_f2;
    f32 phi_f12;

    arg6 = (((arg6 + arg2) % 8) * 7) * (1 / 7.0f);
    arg2++;
    gSPSegment(arg7++, 9, SEGMENTED_TO_VIRTUAL(sFireballsTexs[arg6]));
    if (arg2 != 4) {
        phi_f12 = arg2 + ((4 - this->unk_170) / 4.0f);
    } else {
        phi_f12 = arg2;
    }
    gDPSetPrimColor(arg7++, 0, 1, 255, 255, 0, 150);
    gDPSetEnvColor(arg7++, 255, 0, 0, 255);

    arg3->xx = arg3->yy = arg3->zz = (0.7f * phi_f12) + 0.5f;

    temp_f2 = (arg3->xx * 10.0f * phi_f12) + 20.0f;
    arg3->xw = (temp_f2 * arg4) + this->dyna.actor.world.pos.x;
    arg3->yw = this->dyna.actor.world.pos.y + 30.0f + (.7f * phi_f12);
    arg3->zw = (temp_f2 * arg5) + this->dyna.actor.world.pos.z;
    gSPMatrix(arg7++,
              Matrix_MtxFToMtx(MATRIX_CHECKFLOATS(arg3),
                               Graph_Alloc(play->state.gfxCtx, sizeof(Mtx))),
              G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);

    gSPDisplayList(arg7++, gFireTempleFireballDL);

    return arg7;
}

Gfx* func_8088DC50(PlayState* play, BgHidanSekizou* this, s16 arg2, s16 arg3, Gfx* arg4) {
    s32 pad;
    s16 temp_v1;
    s32 phi_s1;
    s32 phi_s2;
    f32 temp_f20;
    f32 temp_f22;
    MtxF sp68;
    s32 i;

    if (arg3 < 4) {
        phi_s1 = 4 - arg3;
        phi_s2 = 4;
    } else {
        phi_s1 = 0;
        phi_s2 = 36 - arg3;
        phi_s2 = CLAMP_MAX(phi_s2, 4);
    }
    temp_f20 = Math_SinS(arg2);
    temp_f22 = Math_CosS(arg2);
    Matrix_MtxFCopy(&sp68, &gMtxFClear);
    temp_v1 = Camera_GetCamDirYaw(GET_ACTIVE_CAM(play)) - arg2;

    if (ABS(temp_v1) < 0x4000) {
        for (i = phi_s2 - 1; i >= phi_s1; i--) {
            arg4 = func_8088D9F4(play, this, i, &sp68, temp_f20, temp_f22, arg3, arg4);
        }
    } else {
        for (i = phi_s1; i < phi_s2; i++) {
            arg4 = func_8088D9F4(play, this, i, &sp68, temp_f20, temp_f22, arg3, arg4);
        }
    }
    return arg4;
}

void func_8088DE08(s16 arg0, s16 arg1, s32 arg2[]) {
    s16 diff = arg0 - arg1;

    if (ABS(diff) <= 0x2000) {
        arg2[0] = 0;
        arg2[1] = 1;
        arg2[2] = 3;
        arg2[3] = 2;
    } else if (ABS(diff) >= 0x6000) {
        arg2[0] = 2;
        arg2[1] = 3;
        arg2[2] = 1;
        arg2[3] = 0;
    } else if (diff > 0x2000) {
        arg2[0] = 1;
        arg2[1] = 0;
        arg2[2] = 2;
        arg2[3] = 3;
    } else {
        arg2[0] = 3;
        arg2[1] = 2;
        arg2[2] = 0;
        arg2[3] = 1;
    }
}

void BgHidanSekizou_Draw(Actor* thisx, PlayState* play2) {
    PlayState* play = play2;
    BgHidanSekizou* this = (BgHidanSekizou*)thisx;
    s32 i;
    s32 sp6C[4];

    OPEN_DISPS(play->state.gfxCtx);
    Gfx_SetupDL_25Opa(play->state.gfxCtx);
    gSPMatrix(POLY_OPA_DISP++, MATRIX_NEWMTX(play->state.gfxCtx),
              G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
    if (this->dyna.actor.params == 0) {
        gSPDisplayList(POLY_OPA_DISP++, gFireTempleStationaryFlamethrowerShortDL);
    } else {
        gSPDisplayList(POLY_OPA_DISP++, gFireTempleStationaryFlamethrowerTallDL);
    }
    POLY_XLU_DISP = Gfx_SetupDL(POLY_XLU_DISP, 0x14);
    if (this->dyna.actor.params == 0) {
        if (this->unk_168[0] > 0) {
            if ((s16)(Camera_GetCamDirYaw(GET_ACTIVE_CAM(play)) - this->dyna.actor.shape.rot.y) >= 0) {
                POLY_XLU_DISP = func_8088DC50(play, this, this->dyna.actor.shape.rot.y + 0x2000, this->unk_168[0],
                                              POLY_XLU_DISP);
                POLY_XLU_DISP = func_8088DC50(play, this, this->dyna.actor.shape.rot.y - 0x2000, this->unk_168[0],
                                              POLY_XLU_DISP);
            } else {
                POLY_XLU_DISP = func_8088DC50(play, this, this->dyna.actor.shape.rot.y - 0x2000, this->unk_168[0],
                                              POLY_XLU_DISP);
                POLY_XLU_DISP = func_8088DC50(play, this, this->dyna.actor.shape.rot.y + 0x2000, this->unk_168[0],
                                              POLY_XLU_DISP);
            }
        }
    } else {
        func_8088DE08(Camera_GetCamDirYaw(GET_ACTIVE_CAM(play)), this->dyna.actor.shape.rot.y, sp6C);
        for (i = 0; i < 4; i++) {
            s32 index = sp6C[i];

            if (this->unk_168[index] > 0) {
                POLY_XLU_DISP = func_8088DC50(play, this, this->dyna.actor.shape.rot.y + index * 0x4000,
                                              this->unk_168[index], POLY_XLU_DISP);
            }
        }
    }
    CLOSE_DISPS(play->state.gfxCtx);
}
