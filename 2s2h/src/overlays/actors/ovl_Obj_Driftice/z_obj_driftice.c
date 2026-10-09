/*
 * File: z_obj_driftice.c
 * Overlay: ovl_Obj_Driftice
 * Description: Floating Ice Platforms in Mountain Village
 */

#include "z_obj_driftice.h"
#include "objects/object_driftice/object_driftice.h"

#define FLAGS 0x00000000

void ObjDriftice_Init(Actor* thisx, PlayState* play);
void ObjDriftice_Destroy(Actor* thisx, PlayState* play);
void ObjDriftice_Update(Actor* thisx, PlayState* play);
void ObjDriftice_Draw(Actor* thisx, PlayState* play);

void func_80A671A8(ObjDriftice* this);
void func_80A671BC(ObjDriftice* this, PlayState* play);
void func_80A671CC(ObjDriftice* this);
void func_80A671E0(ObjDriftice* this, PlayState* play);
void func_80A6743C(ObjDriftice* this);
void func_80A67450(ObjDriftice* this, PlayState* play);
void func_80A674A8(ObjDriftice* this);
void func_80A674C4(ObjDriftice* this, PlayState* play);

ActorProfile Obj_Driftice_Profile = {
    /**/ ACTOR_OBJ_DRIFTICE,
    /**/ ACTORCAT_BG,
    /**/ FLAGS,
    /**/ OBJECT_DRIFTICE,
    /**/ sizeof(ObjDriftice),
    /**/ ObjDriftice_Init,
    /**/ ObjDriftice_Destroy,
    /**/ ObjDriftice_Update,
    /**/ ObjDriftice_Draw,
};

static f32 D_80A67620[][3] = {
    { 1.0f, 150.0f, 150.0f },
    { 1.5f, 250.0f, 200.0f },
    { 2.0f, 350.0f, 250.0f },
};

static f32 D_80A67644[] = { 1.5f, 3.0f, 4.5f, 6.0f, 7.5f, 9.0f, 10.5f, 0.0f };

static f32 D_80A67664[][4] = {
    { 0.6f, 0.5f, 0.2f, 0.4f },
    { 0.8f, 0.7f, 0.35f, 0.7f },
    { 1.0f, 1.0f, 0.8f, 1.0f },
};

typedef struct {
    /* 0x0 */ s16 unk_00;
    /* 0x2 */ s16 unk_02;
    /* 0x4 */ f32 unk_04;
    /* 0x8 */ f32 unk_08;
} ObjDrifticeDataStruct; // size = 0xC

static ObjDrifticeDataStruct D_80A67694[] = {
    { 0x12C, 0x64, 1.3f, 2.0f },
    { 0x258, 0x1F4, 1.2f, 1.8f },
    { 0x4B0, 0x3E8, 1.1f, 1.5f },
};

static ObjDrifticeDataStruct D_80A676B8[] = { { 0x320, 0x190, 200.0f, 400.0f }, { 0x5DC, 0x258, 100.0f, 300.0f } };

static ObjDrifticeDataStruct D_80A676D0[] = {
    { 0x12C, 0x64, 300.0f, 700.0f },
    { 0x258, 0x258, 200.0f, 400.0f },
    { 0x4B0, 0x4B0, 100.0f, 300.0f },
};

static InitChainEntry sInitChain[] = {
    ICHAIN_F32(cullingVolumeDistance, 4000, ICHAIN_STOP),
};

void func_80A66570(ObjDriftice* this, s32 arg1) {
    Math_Vec3s_ToVec3f(&this->dyna.actor.world.pos, &this->pathPoints[arg1]);
}

void func_80A665AC(s16* arg0, s16 arg1) {
    if (arg1 < *arg0) {
        *arg0 = arg1;
    } else if (*arg0 < -arg1) {
        *arg0 = -arg1;
    }
}

f32 func_80A665EC(ObjDrifticeStruct* arg0, ObjDriftice* this) {
    f32 phi_f2 = 0.0f;
    f32 sp20;

    if (this->unk_248 >= 10) {
        sp20 = -8.0f;
    } else if (this->unk_248 < -9) {
        sp20 = 0.0f;
    } else if (this->unk_248 > 0) {
        sp20 = (Math_CosS(fabsf(this->unk_248) * (0x8000 * 0.1f)) - 1.0f) * 4.0f;
    } else {
        sp20 = (Math_CosS(fabsf(this->unk_248) * (0x8000 * 0.1f)) + 1.0f) * -4.0f;
    }

    if (this->unk_248 > 0) {
        if (arg0->unk_00.unk_00 > 0) {
            arg0->unk_00.unk_00--;
        } else {
            arg0->unk_00.unk_00 = Rand_S16Offset(30, 70);
            arg0->unk_00.unk_04 = Rand_S16Offset(1000, 1000);
            arg0->unk_00.unk_0C = (Rand_ZeroOne() * 1.5f) + 1.0f;
            arg0->unk_00.unk_10 = fabsf(arg0->unk_00.unk_0C - arg0->unk_00.unk_08) * 0.05f;
        }
    } else {
        arg0->unk_00.unk_00 = 0;
        arg0->unk_00.unk_0C = phi_f2;
        arg0->unk_00.unk_10 = (fabsf(arg0->unk_00.unk_0C - arg0->unk_00.unk_08) * 0.05f) + 0.02f;
    }

    arg0->unk_00.unk_02 += arg0->unk_00.unk_04;
    Math_StepToF(&arg0->unk_00.unk_08, arg0->unk_00.unk_0C, arg0->unk_00.unk_10);

    sp20 += (Math_SinS(arg0->unk_00.unk_02) * arg0->unk_00.unk_08);
    return sp20;
}

f32 func_80A667F0(ObjDrifticeStruct3* arg0, ObjDriftice* this) {
    f32 temp_f20 = 0.0f;
    ObjDrifticeStruct3* temp_s0;
    s32 i;

    for (i = 0; i < 3; i++) {
        temp_s0 = &arg0[i];

        if (temp_s0->unk_00 > 0) {
            temp_s0->unk_00--;
        } else {
            temp_s0->unk_00 = Rand_S16Offset(30, 70);
            temp_s0->unk_04 = Rand_S16Offset(D_80A67694[i].unk_00, D_80A67694[i].unk_02);
            temp_s0->unk_0C = D_80A67694[i].unk_04 + (D_80A67694[i].unk_08 * Rand_ZeroOne());
            temp_s0->unk_10 = fabsf(temp_s0->unk_0C - temp_s0->unk_08) * 0.05f;
        }
        temp_s0->unk_02 += temp_s0->unk_04;
        Math_StepToF(&temp_s0->unk_08, temp_s0->unk_0C, temp_s0->unk_10);
        temp_f20 += Math_SinS(temp_s0->unk_02) * temp_s0->unk_08;
    }

    return temp_f20;
}

void func_80A66930(ObjDrifticeStruct2* arg0, ObjDriftice* this, s16* arg2, s16* arg3) {
    ObjDrifticeStruct3* temp_s0;
    f32 temp_f22 = 0.0f;
    s16 phi_s0;
    s32 i;

    if (this->unk_248 > 0) {
        f32 temp_f20 = this->dyna.actor.xzDistToPlayer * this->unk_240 * 0.0007075472f;

        if (temp_f20 > 1.0f) {
            temp_f20 = 1.0f;
        } else if (temp_f20 < 0.01f) {
            temp_f20 = 0.01f;
        }

        Math_StepToS(&arg0->unk_02, 1200.0f * temp_f20, 0x64);
        phi_s0 = 2500.0f * temp_f20;
        Math_StepToS(&arg0->unk_06, Math_CosS(arg0->unk_04 * 6.5536f) * (120.0f * temp_f20), 0x28);
        Math_StepToF(&arg0->unk_08, 1.0f, 0.02f);
    } else {
        Math_StepToS(&arg0->unk_02, 0, 0x96);
        phi_s0 = 0;
        Math_StepToS(&arg0->unk_06, 20, 7);
        Math_StepToF(&arg0->unk_08, 0.0f, 0.02f);
    }
    Math_ScaledStepToS(&arg0->unk_00, this->dyna.actor.yawTowardsPlayer, arg0->unk_02);
    *arg3 = arg0->unk_00;
    Math_ScaledStepToS(&arg0->unk_04, phi_s0, arg0->unk_06);

    for (i = 0; i < 2; i++) {
        temp_s0 = &arg0->unk_0C[i];

        if (temp_s0->unk_00 > 0) {
            temp_s0->unk_00--;
        } else {
            temp_s0->unk_00 = Rand_S16Offset(30, 70);
            temp_s0->unk_04 = Rand_S16Offset(D_80A676B8[i].unk_00, D_80A676B8[i].unk_02);
            temp_s0->unk_0C = D_80A676B8[i].unk_04 + (D_80A676B8[i].unk_08 * Rand_ZeroOne());
            temp_s0->unk_10 = fabsf(temp_s0->unk_0C - temp_s0->unk_08) * 0.033333335f;
        }
        temp_s0->unk_02 += temp_s0->unk_04;
        Math_StepToF(&temp_s0->unk_08, temp_s0->unk_0C, temp_s0->unk_10);
        temp_f22 += Math_SinS(temp_s0->unk_02) * temp_s0->unk_08;
    }

    temp_f22 *= arg0->unk_08;

    *arg2 = (s32)temp_f22 + arg0->unk_04;
}

void func_80A66C4C(ObjDrifticeStruct4* arg0, ObjDriftice* this, s16* arg2, s16* arg3) {
    ObjDrifticeStruct3* temp_s0;
    f32 temp_f20 = 0.0f;
    s32 i;

    if (arg0->unk_00[0] > 0) {
        arg0->unk_00[0]--;
    } else {
        arg0->unk_00[0] = Rand_S16Offset(30, 70);
        arg0->unk_00[2] = Rand_S16Offset(-200, 200);
        arg0->unk_00[4] = Rand_S16Offset(-400, 400);
    }

    Math_StepToS(&arg0->unk_00[1], arg0->unk_00[2], 0x32);
    Math_StepToS(&arg0->unk_00[3], arg0->unk_00[4], 0x32);

    *arg3 = arg0->unk_00[3] + arg0->unk_00[1];

    for (i = 0; i < 3; i++) {
        temp_s0 = &arg0->unk_0C[i];

        if (temp_s0->unk_00 > 0) {
            temp_s0->unk_00--;
        } else {
            temp_s0->unk_00 = Rand_S16Offset(30, 70);
            temp_s0->unk_04 = Rand_S16Offset(D_80A676D0[i].unk_00, D_80A676D0[i].unk_02);
            temp_s0->unk_0C = D_80A676D0[i].unk_04 + (D_80A676D0[i].unk_08 * Rand_ZeroOne());
            temp_s0->unk_10 = fabsf(temp_s0->unk_0C - temp_s0->unk_08) * 0.033333335f;
        }
        temp_s0->unk_02 += temp_s0->unk_04;
        Math_StepToF(&temp_s0->unk_08, temp_s0->unk_0C, temp_s0->unk_10);
        temp_f20 += Math_SinS(temp_s0->unk_02) * temp_s0->unk_08;
    }

    *arg2 = temp_f20;
}

void func_80A66E30(ObjDrifticeStruct* arg0, ObjDriftice* this) {
    f32 temp;
    f32* temp_s1 = D_80A67664[OBJDRIFTICE_GET_ROT(&this->dyna.actor) - 1];
    s16 sp36;
    s16 sp34;
    s16 sp32;
    s16 sp30;
    f32 sp2C;
    f32 sp28;
    f32 sp24;
    s16 sp22;

    sp2C = func_80A665EC(arg0, this);
    sp28 = func_80A667F0(arg0->unk_14, this);
    func_80A66930(&arg0->unk_50, this, &sp32, &sp30);
    func_80A66C4C(&arg0->unk_84, this, &sp36, &sp34);

    this->dyna.actor.shape.yOffset = this->unk_240 * ((sp28 * temp_s1[1]) + (sp2C * temp_s1[0]));

    sp24 = (sp32 * temp_s1[2]) + (sp36 * temp_s1[3]) * (temp = 1.0f);
    sp22 = (sp34 + sp30) - this->dyna.actor.shape.rot.y;

    this->dyna.actor.shape.rot.x = Math_CosS(sp22) * sp24;
    func_80A665AC(&this->dyna.actor.shape.rot.x, 0xA28);
    this->dyna.actor.shape.rot.z = -Math_SinS(sp22) * sp24;
    func_80A665AC(&this->dyna.actor.shape.rot.z, 0xA28);
}

void ObjDriftice_Init(Actor* thisx, PlayState* play) {
    s32 pad;
    ObjDriftice* this = (ObjDriftice*)thisx;
    f32* sp2C = D_80A67620[OBJDRIFTICE_GET_3(&this->dyna.actor)];
    Path* path;
    s32 phi_a1;
    s32 sp20;

    Actor_ProcessInitChain(&this->dyna.actor, sInitChain);

    this->dyna.actor.shape.rot.x = 0;
    this->dyna.actor.world.rot.x = 0;
    this->dyna.actor.shape.rot.z = 0;
    this->dyna.actor.world.rot.z = 0;

    Actor_SetScale(&this->dyna.actor, sp2C[0] * 0.035377357f);
    this->dyna.actor.cullingVolumeScale = sp2C[1];
    this->dyna.actor.cullingVolumeDownward = sp2C[2];
    this->unk_240 = 1.0f / this->dyna.actor.scale.x;
    this->unk_23C = D_80A67644[OBJDRIFTICE_GET_E00(&this->dyna.actor)];

    sp20 = 0;
    if (this->unk_23C < 0.01f) {
        sp20 = 1;
    }

    this->unk_244 = thisx->home.rot.z * 45.511112f;

    phi_a1 = 0;
    if (sp20 == 0) {
        phi_a1 = 1;
    }

    if (thisx->home.rot.z != 0) {
        phi_a1 |= 3;
    }

    DynaPolyActor_Init(&this->dyna, phi_a1);
    DynaPolyActor_LoadMesh(play, &this->dyna, &object_driftice_Colheader_001AA8);

    if (sp20 != 0) {
        func_80A671A8(this);
    } else {
        this->dyna.actor.flags |= ACTOR_FLAG_UPDATE_CULLING_DISABLED;

        path = &play->setupPathList[OBJDRIFTICE_GET_PATH_INDEX(&this->dyna.actor)];
        this->unk_164 = 0;
        this->unk_160 = path->count - 1;
        this->unk_168 = 1;

        this->pathPoints = Lib_SegmentedToVirtual(path->points);
        func_80A66570(this, this->unk_164);
        func_80A671CC(this);
    }
}

void ObjDriftice_Destroy(Actor* thisx, PlayState* play) {
    ObjDriftice* this = (ObjDriftice*)thisx;

    DynaPoly_DeleteBgActor(play, &play->colCtx.dyna, this->dyna.bgId);
}

void func_80A671A8(ObjDriftice* this) {
    this->actionFunc = func_80A671BC;
}

void func_80A671BC(ObjDriftice* this, PlayState* play) {
}

void func_80A671CC(ObjDriftice* this) {
    this->actionFunc = func_80A671E0;
}

void func_80A671E0(ObjDriftice* this, PlayState* play) {
    f32 phi_f0;
    Vec3f sp40;
    f32 sp3C;
    f32 phi_f12;
    Vec3s* points;
    s32 sp30;
    Actor* thisx = &this->dyna.actor;

    Math_Vec3s_ToVec3f(&sp40, &(&this->pathPoints[this->unk_164])[this->unk_168]);
    Math_Vec3f_Diff(&sp40, &this->dyna.actor.world.pos, &thisx->velocity);

    sp3C = Math3D_Vec3fMagnitude(&thisx->velocity);
    if (sp3C < (this->unk_23C * 8.0f)) {
        phi_f0 = this->unk_23C * 0.4f;
        phi_f12 = this->unk_23C * 0.05f;
    } else {
        phi_f0 = this->unk_23C;
        phi_f12 = this->unk_23C * 0.13f;
    }

    Math_StepToF(&this->dyna.actor.speed, phi_f0, phi_f12);

    if ((this->dyna.actor.speed + 0.05f) < sp3C) {
        Math_Vec3f_Scale(&this->dyna.actor.velocity, this->dyna.actor.speed / sp3C);
        this->dyna.actor.world.pos.x += this->dyna.actor.velocity.x;
        this->dyna.actor.world.pos.y += this->dyna.actor.velocity.y;
        this->dyna.actor.world.pos.z += this->dyna.actor.velocity.z;
    } else {
        sp30 = true;
        this->unk_164 += this->unk_168;

        if (1) {}

        this->dyna.actor.speed *= 0.5f;
        if (((this->unk_164 >= this->unk_160) && (this->unk_168 > 0)) ||
            ((this->unk_164 <= 0) && (this->unk_168 < 0))) {
            if (!OBJDRIFTICE_GET_1000(&this->dyna.actor)) {
                this->unk_168 = -this->unk_168;
                func_80A674A8(this);
            } else {
                points = &this->pathPoints[this->unk_160];

                if (this->unk_168 > 0) {
                    this->unk_164 = 0;
                } else {
                    this->unk_164 = this->unk_160;
                }

                if ((this->pathPoints[0].x != points->x) || (this->pathPoints[0].y != points->y) ||
                    (this->pathPoints[0].z != points->z)) {
                    func_80A6743C(this);
                    DynaPoly_DisableCollision(play, &play->colCtx.dyna, this->dyna.bgId);
                    sp30 = false;
                }
            }
        }

        if (sp30) {
            func_80A66570(this, this->unk_164);
        }
    }
}

void func_80A6743C(ObjDriftice* this) {
    this->actionFunc = func_80A67450;
}

void func_80A67450(ObjDriftice* this, PlayState* play) {
    if (this->unk_248 < 0) {
        func_80A66570(this, this->unk_164);
        DynaPoly_EnableCollision(play, &play->colCtx.dyna, this->dyna.bgId);
        func_80A671CC(this);
    }
}

void func_80A674A8(ObjDriftice* this) {
    this->unk_24C = 10;
    this->actionFunc = func_80A674C4;
}

void func_80A674C4(ObjDriftice* this, PlayState* play) {
    this->unk_24C--;
    if (this->unk_24C <= 0) {
        this->dyna.actor.speed = 0.0f;
        func_80A671CC(this);
    }
}

void ObjDriftice_Update(Actor* thisx, PlayState* play) {
    ObjDriftice* this = (ObjDriftice*)thisx;

    if (DynaPolyActor_IsPlayerOnTop(&this->dyna)) {
        if (this->unk_248 < 0) {
            this->unk_248 = 1;
        } else {
            this->unk_248++;
        }
    } else {
        if (this->unk_248 > 0) {
            this->unk_248 = -1;
        } else {
            this->unk_248--;
        }
    }

    this->dyna.actor.shape.rot.y += this->unk_244;

    this->actionFunc(this, play);

    if (OBJDRIFTICE_GET_ROT(&this->dyna.actor) && (this->dyna.actor.flags & ACTOR_FLAG_INSIDE_CULLING_VOLUME)) {
        func_80A66E30(&this->unk_170, this);
    }
}

void ObjDriftice_Draw(Actor* thisx, PlayState* play) {
    ObjDriftice* this = (ObjDriftice*)thisx;

    Gfx_DrawDListOpa(play, object_driftice_DL_0016A0);
}
