/*
 * File: z_mir_ray3.c
 * Overlay: ovl_Mir_Ray3
 * Description: Mirror shield reflection and glow
 */

#include "z_mir_ray3.h"
#include "objects/object_mir_ray/object_mir_ray.h"

#define FLAGS (ACTOR_FLAG_UPDATE_CULLING_DISABLED | ACTOR_FLAG_DRAW_CULLING_DISABLED)

void MirRay3_Init(Actor* thisx, PlayState* play);
void MirRay3_Destroy(Actor* thisx, PlayState* play);
void MirRay3_Update(Actor* thisx, PlayState* play);
void MirRay3_Draw(Actor* thisx, PlayState* play);

ActorProfile Mir_Ray3_Profile = {
    /**/ ACTOR_MIR_RAY3,
    /**/ ACTORCAT_ITEMACTION,
    /**/ FLAGS,
    /**/ OBJECT_MIR_RAY,
    /**/ sizeof(MirRay3),
    /**/ MirRay3_Init,
    /**/ MirRay3_Destroy,
    /**/ MirRay3_Update,
    /**/ MirRay3_Draw,
};

static ColliderQuadInit sQuadInit = {
    {
        COL_MATERIAL_NONE,
        AT_ON | AT_TYPE_PLAYER,
        AC_NONE,
        OC1_NONE,
        OC2_NONE,
        COLSHAPE_QUAD,
    },
    {
        ELEM_MATERIAL_UNK0,
        { 0x00200000, 0x00, 0x00 },
        { 0xF7CFFFFF, 0x00, 0x00 },
        ATELEM_ON | ATELEM_SFX_NORMAL,
        ACELEM_NONE,
        OCELEM_NONE,
    },
    { { { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } } },
};

static ColliderCylinderInit sCylinderInit = {
    {
        COL_MATERIAL_NONE,
        AT_NONE,
        AC_ON | AC_TYPE_PLAYER | AC_TYPE_OTHER,
        OC1_NONE,
        OC2_TYPE_2,
        COLSHAPE_CYLINDER,
    },
    {
        ELEM_MATERIAL_UNK0,
        { 0x00000000, 0x00, 0x00 },
        { 0x00200000, 0x00, 0x00 },
        ATELEM_NONE | ATELEM_SFX_NORMAL,
        ACELEM_ON,
        OCELEM_NONE,
    },
    { 10, 10, 0, { 0, 0, 0 } },
};

typedef struct {
    /* 0x00 */ Vec3f unk_00;
    /* 0x0C */ MtxF unk_0C;
    /* 0x4C */ CollisionPoly* unk_4C;
    /* 0x50 */ u8 unk_50;
} MirRay3Struct; // size = 0x54

void MirRay3_Init(Actor* thisx, PlayState* play) {
    s32 pad;
    MirRay3* this = (MirRay3*)thisx;

    ActorShape_Init(&this->actor.shape, 0.0f, NULL, 0.0f);
    Actor_SetScale(&this->actor, 1.0f);

    this->unk_218[0].x = -536.0f;
    this->unk_218[0].y = -939.0f;

    this->unk_218[1].x = -1690.0f;
    this->unk_218[1].y = 0.0f;

    this->unk_218[2].x = -536.0f;
    this->unk_218[2].y = 938.0f;

    this->unk_218[3].x = 921.0f;
    this->unk_218[3].y = 0.0f;

    this->unk_218[4].x = 758.0f;
    this->unk_218[4].y = 800.0f;

    this->unk_218[5].x = 758.0f;
    this->unk_218[5].y = -800.0f;

    Collider_InitQuad(play, &this->colliderQuad);
    Collider_SetQuad(play, &this->colliderQuad, &this->actor, &sQuadInit);
    Collider_InitCylinder(play, &this->colliderCylinder);
    Collider_SetCylinder(play, &this->colliderCylinder, &this->actor, &sCylinderInit);
    this->actor.world.rot.x = this->actor.shape.rot.x = 0;
}

void MirRay3_Destroy(Actor* thisx, PlayState* play) {
    MirRay3* this = (MirRay3*)thisx;

    Collider_DestroyQuad(play, &this->colliderQuad);
    Collider_DestroyCylinder(play, &this->colliderCylinder);
}

void MirRay3_Update(Actor* thisx, PlayState* play) {
    s32 pad[2];
    MirRay3* this = (MirRay3*)thisx;
    Player* player = GET_PLAYER(play);

    this->unk_210 &= ~1;

    if (this->unk_214 > 0.5f) {
        CollisionCheck_SetAT(play, &play->colChkCtx, &this->colliderQuad.base);
    }

    this->unk_210 &= ~2;

    if (Player_HasMirrorShieldEquipped(play)) {
        if (this->colliderCylinder.base.acFlags & AC_HIT) {
            this->unk_210 |= 2;
        }
        this->actor.world.pos.x = player->shieldMf.mf[3][0];
        this->actor.world.pos.y = player->shieldMf.mf[3][1];
        this->actor.world.pos.z = player->shieldMf.mf[3][2];
        Collider_UpdateCylinder(&this->actor, &this->colliderCylinder);
        CollisionCheck_SetAC(play, &play->colChkCtx, &this->colliderCylinder.base);
    }

    if (this->unk_214 > 0.1f) {
        Actor_PlaySfx_Flagged2(&player->actor, NA_SE_IT_SHIELD_BEAM - SFX_FLAG);
    }

    Math_ApproachZeroF(&this->unk_214, 1.0f, 0.1f);
}

void func_80B9E544(MirRay3* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    MtxF* shieldMtx = &player->shieldMf;
    f32 temp_f0;

    if (this->unk_210 & 2) {
        temp_f0 = sqrtf(SQ(shieldMtx->mf[2][0]) + SQ(shieldMtx->mf[2][1]) + SQ(shieldMtx->mf[2][2]));
        if (temp_f0 == 0.0f) {
            this->unk_260 = 1.0f;
        } else {
            this->unk_260 = 1.0f / temp_f0;
        }
        Math_ApproachF(&this->unk_214, 1.0f, 0.5f, 0.25f);
    }
}

void func_80B9E5F4(MirRay3* this, PlayState* play, MirRay3Struct* ptr) {
    Player* player = GET_PLAYER(play);
    MtxF* shieldMtx = &player->shieldMf;
    s32 i;
    Vec3f sp88;
    Vec3f sp7C;
    Vec3f sp70;
    CollisionPoly* sp6C;
    f32 sp60[3];

    sp60[0] = -(shieldMtx->mf[2][0] * this->unk_260) * this->unk_214 * 400.0f;
    sp60[1] = -(shieldMtx->mf[2][1] * this->unk_260) * this->unk_214 * 400.0f;
    sp60[2] = -(shieldMtx->mf[2][2] * this->unk_260) * this->unk_214 * 400.0f;

    for (i = 0; i < ARRAY_COUNT(this->unk_218); i++) {
        sp88.x = (shieldMtx->mf[3][0] + (this->unk_218[i].x * shieldMtx->mf[0][0])) +
                 (this->unk_218[i].y * shieldMtx->mf[1][0]);
        sp88.y = (shieldMtx->mf[3][1] + (this->unk_218[i].x * shieldMtx->mf[0][1])) +
                 (this->unk_218[i].y * shieldMtx->mf[1][1]);
        sp88.z = (shieldMtx->mf[3][2] + (this->unk_218[i].x * shieldMtx->mf[0][2])) +
                 (this->unk_218[i].y * shieldMtx->mf[1][2]);

        sp7C.x = sp60[0] + sp88.x;
        sp7C.y = sp60[1] + sp88.y;
        sp7C.z = sp60[2] + sp88.z;

        if (BgCheck_AnyLineTest1(&play->colCtx, &sp88, &sp7C, &sp70, &sp6C, true)) {
            ptr[i].unk_4C = sp6C;
        } else {
            ptr[i].unk_4C = NULL;
        }
    }
}

void func_80B9E7D0(MirRay3Struct* ptr) {
    s32 i;
    s32 j;

    for (i = 0; i < 6; i++) {
        for (j = i + 1; j < 6; j++) {
            if ((ptr[i].unk_4C != NULL) && (ptr[j].unk_4C != NULL)) {
                if ((ABS(ptr[i].unk_4C->normal.x - ptr[j].unk_4C->normal.x) < 100) &&
                    (ABS(ptr[i].unk_4C->normal.y - ptr[j].unk_4C->normal.y) < 100) &&
                    (ABS(ptr[i].unk_4C->normal.z - ptr[j].unk_4C->normal.z) < 100) &&
                    (ptr[i].unk_4C->dist == ptr[j].unk_4C->dist)) {
                    ptr[j].unk_4C = NULL;
                }
            }
        }
    }
}

void func_80B9E8D4(MirRay3* this, PlayState* play, MirRay3Struct* ptr) {
    Player* player = GET_PLAYER(play);
    s32 i;
    MtxF* shieldMf = &player->shieldMf;
    Vec3f sp140;
    Vec3f sp134;
    Vec3f sp128;
    Vec3f sp11C;
    Vec3f sp110;
    Vec3f sp104;
    f32 spF8[3];
    f32 spEC[3];
    f32 temp_f0;
    f32 temp_f26;
    f32 temp_f2;
    Vec3f spD4;
    Vec3f spC8;

    spF8[0] = -(shieldMf->mf[2][0] * this->unk_260) * this->unk_214 * 400.0f;
    spF8[1] = -(shieldMf->mf[2][1] * this->unk_260) * this->unk_214 * 400.0f;
    spF8[2] = -(shieldMf->mf[2][2] * this->unk_260) * this->unk_214 * 400.0f;

    sp140.x = shieldMf->mf[3][0];
    sp140.y = shieldMf->mf[3][1];
    sp140.z = shieldMf->mf[3][2];

    sp134.x = spF8[0] + sp140.x;
    sp134.y = spF8[1] + sp140.y;
    sp134.z = spF8[2] + sp140.z;

    temp_f26 = this->unk_214 * 400.0f;

    for (i = 0; i < ARRAY_COUNT(this->unk_218); i++) {
        if (ptr[i].unk_4C != NULL) {
            spEC[0] = COLPOLY_GET_NORMAL(ptr[i].unk_4C->normal.x);
            spEC[1] = COLPOLY_GET_NORMAL(ptr[i].unk_4C->normal.y);
            spEC[2] = COLPOLY_GET_NORMAL(ptr[i].unk_4C->normal.z);

            if (Math3D_LineSegVsPlane(spEC[0], spEC[1], spEC[2], ptr[i].unk_4C->dist, &sp140, &sp134, &sp128, true)) {
                ptr[i].unk_00.x = sp128.x;
                ptr[i].unk_00.y = sp128.y;
                ptr[i].unk_00.z = sp128.z;

                temp_f0 = sqrtf(SQ(sp128.x - sp140.x) + SQ(sp128.y - sp140.y) + SQ(sp128.z - sp140.z));

                if (temp_f0 < (temp_f26 * 0.9f)) {
                    ptr[i].unk_50 = 0xFF;
                } else if (temp_f26 < temp_f0) {
                    ptr[i].unk_50 = 0;
                } else {
                    ptr[i].unk_50 = (s32)((10.0f - ((10.0f / temp_f26) * temp_f0)) * 255.0f);
                }

                sp11C.x = (shieldMf->mf[0][0] * 100.0f) + sp140.x;
                sp11C.y = (shieldMf->mf[0][1] * 100.0f) + sp140.y;
                sp11C.z = (shieldMf->mf[0][2] * 100.0f) + sp140.z;

                sp110.x = (spF8[0] * 4.0f) + sp11C.x;
                sp110.y = (spF8[1] * 4.0f) + sp11C.y;
                sp110.z = (spF8[2] * 4.0f) + sp11C.z;

                ptr[i].unk_0C.mf[0][0] = ptr[i].unk_0C.mf[1][1] = ptr[i].unk_0C.mf[2][2] = ptr[i].unk_0C.mf[3][3] =
                    1.0f;

                ptr[i].unk_0C.mf[0][1] = ptr[i].unk_0C.mf[0][2] = ptr[i].unk_0C.mf[0][3] = ptr[i].unk_0C.mf[1][0] =
                    ptr[i].unk_0C.mf[1][2] = ptr[i].unk_0C.mf[1][3] = ptr[i].unk_0C.mf[2][0] = ptr[i].unk_0C.mf[2][1] =
                        ptr[i].unk_0C.mf[2][3] = ptr[i].unk_0C.mf[3][0] = ptr[i].unk_0C.mf[3][1] =
                            ptr[i].unk_0C.mf[3][2] = 0.0f;

                if (Math3D_LineSegVsPlane(spEC[0], spEC[1], spEC[2], ptr[i].unk_4C->dist, &sp11C, &sp110, &sp104,
                                          true)) {
                    ptr[i].unk_0C.mf[0][0] = sp104.x - sp128.x;
                    ptr[i].unk_0C.mf[0][1] = sp104.y - sp128.y;
                    ptr[i].unk_0C.mf[0][2] = sp104.z - sp128.z;
                }

                sp11C.x = (shieldMf->mf[1][0] * 100.0f) + sp140.x;
                sp11C.y = (shieldMf->mf[1][1] * 100.0f) + sp140.y;
                sp11C.z = (shieldMf->mf[1][2] * 100.0f) + sp140.z;

                sp110.x = (spF8[0] * 4.0f) + sp11C.x;
                sp110.y = (spF8[1] * 4.0f) + sp11C.y;
                sp110.z = (spF8[2] * 4.0f) + sp11C.z;

                if (Math3D_LineSegVsPlane(spEC[0], spEC[1], spEC[2], ptr[i].unk_4C->dist, &sp11C, &sp110, &sp104,
                                          true)) {
                    ptr[i].unk_0C.mf[1][0] = sp104.x - sp128.x;
                    ptr[i].unk_0C.mf[1][1] = sp104.y - sp128.y;
                    ptr[i].unk_0C.mf[1][2] = sp104.z - sp128.z;
                }
            } else {
                ptr[i].unk_4C = NULL;
            }
        }
    }

    temp_f2 = this->unk_214 * 400.0f;

    spF8[0] = -(this->unk_260 * shieldMf->mf[2][0]);
    spF8[1] = -(this->unk_260 * shieldMf->mf[2][1]);
    spF8[2] = -(this->unk_260 * shieldMf->mf[2][2]);

    sp134.x = (spF8[0] * temp_f2) + sp140.x;
    sp134.y = (spF8[1] * temp_f2) + sp140.y;
    sp134.z = (spF8[2] * temp_f2) + sp140.z;

    {
        CollisionPoly* spC4;

        if (!BgCheck_AnyLineTest1(&play->colCtx, &sp140, &sp134, &sp128, &spC4, true)) {
            Math_Vec3f_Copy(&sp128, &sp134);
        }

        sp128.x += spF8[0] * 5.0f;
        sp128.y += spF8[1] * 5.0f;
        sp128.z += spF8[2] * 5.0f;

        spD4.x = (shieldMf->mf[0][0] * 300.0f) + sp140.x;
        spD4.y = (shieldMf->mf[0][1] * 300.0f) + sp140.y;
        spD4.z = (shieldMf->mf[0][2] * 300.0f) + sp140.z;

        spC8.x = (shieldMf->mf[0][0] * 300.0f) + sp128.x;
        spC8.y = (shieldMf->mf[0][1] * 300.0f) + sp128.y;
        spC8.z = (shieldMf->mf[0][2] * 300.0f) + sp128.z;

        Collider_SetQuadVertices(&this->colliderQuad, &spD4, &sp140, &spC8, &sp128);
    }
}

void MirRay3_Draw(Actor* thisx, PlayState* play) {
    s32 pad[2];
    MirRay3* this = (MirRay3*)thisx;
    MirRay3Struct sp8C[6];
    Player* player = GET_PLAYER(play);
    s32 i;
    f32 temp;
    u16 time;

    if (!(this->unk_210 & 1) && Player_HasMirrorShieldEquipped(play)) {
        Matrix_Mult(&player->shieldMf, MTXMODE_NEW);
        func_80B9E544(this, play);

        if (this->unk_214 <= 0.1f) {
            return;
        }

        OPEN_DISPS(play->state.gfxCtx);

        Gfx_SetupDL25_Xlu(play->state.gfxCtx);
        Matrix_Scale(1.0f, 1.0f, this->unk_214, MTXMODE_APPLY);

        MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx);

        if (MIRRAY3_GET_F(&this->actor) == MIRRAY3_F_1) {
            time = CURRENT_TIME;

            if (time > CLOCK_TIME(12, 0)) {
                time = (DAY_LENGTH - 1) - time;
            }

            temp = (time * (1.0f / 0x8000));
            gDPSetPrimColor(POLY_XLU_DISP++, 0, 0x78, 255, 255, 255, (u8)(s8)(100 * this->unk_214));

            gDPSetEnvColor(POLY_XLU_DISP++, 218, 225, (u8)((100.0f * temp) + 105.0f), 255);
        } else {
            gDPSetPrimColor(POLY_XLU_DISP++, 0, 0x78, 255, 255, 255, (u8)(s8)(100 * this->unk_214));
            gDPSetEnvColor(POLY_XLU_DISP++, 218, 225, 205, 255);
        }

        AnimatedMat_Draw(play, Lib_SegmentedToVirtual(object_mir_ray_Matanimheader_0003F8));

        gSPDisplayList(POLY_XLU_DISP++, object_mir_ray_DL_000168);

        func_80B9E5F4(this, play, sp8C);
        func_80B9E7D0(sp8C);
        func_80B9E8D4(this, play, sp8C);

        if (sp8C[0].unk_4C == NULL) {
            sp8C[0].unk_50 = 0;
        }

        for (i = 1; i < ARRAY_COUNT(sp8C); i++) {
            if ((sp8C[i].unk_4C != NULL) && (sp8C[0].unk_50 < sp8C[i].unk_50)) {
                sp8C[0].unk_50 = sp8C[i].unk_50;
            }
        }

        gDPSetRenderMode(POLY_XLU_DISP++, G_RM_FOG_SHADE_A, G_RM_AA_ZB_XLU_DECAL2);

        for (i = 0; i < ARRAY_COUNT(sp8C); i++) {
            if (sp8C[i].unk_4C != NULL) {
                Matrix_Translate(sp8C[i].unk_00.x, sp8C[i].unk_00.y, sp8C[i].unk_00.z, MTXMODE_NEW);
                Matrix_Scale(0.01f, 0.01f, 0.01f, MTXMODE_APPLY);
                Matrix_Mult(&sp8C[i].unk_0C, MTXMODE_APPLY);

                MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx);
                gDPPipeSync(POLY_XLU_DISP++);
                gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 255, 255, 255, sp8C[0].unk_50);
                gSPDisplayList(POLY_XLU_DISP++, object_mir_ray_DL_0004B0);
            }
        }

        this->unk_210 |= 1;

        CLOSE_DISPS(play->state.gfxCtx);
    }
}
