/*
 * File: z_eff_ss_kakera.c
 * Overlay: ovl_Effect_Ss_Kakera
 * Description:
 */

#include "z_eff_ss_kakera.h"
#include "debug.h"

#define rReg0 regs[0]
#define rGravity regs[1]
#define rPitch regs[2]
#define rYaw regs[3]
#define rReg4 regs[4]
#define rReg5 regs[5]
#define rReg6 regs[6]
#define rScale regs[7]
#define rReg8 regs[8]
#define rReg9 regs[9]
#define rObjectId regs[10]
#define rObjectSlot regs[11]
#define rColorIndex regs[12]

#define PARAMS ((EffectSsKakeraInitParams*)initParamsx)

u32 EffectSsKakera_Init(PlayState* play, u32 index, EffectSs* this, void* initParamsx);
void EffectSsKakera_Update(PlayState* play, u32 index, EffectSs* this);
void EffectSsKakera_Draw(PlayState* play, u32 index, EffectSs* this);

void EffectSsKakera_CheckForObject(EffectSs* this, PlayState* play);

EffectSsProfile Effect_Ss_Kakera_Profile = {
    EFFECT_SS_KAKERA,
    EffectSsKakera_Init,
};

KakeraColorStruct D_8097EAD8[] = {
    { 0, { 255, 255, 255 } },
    { 0, { 235, 170, 130 } },
    { 1, { 210, 190, 170 } },
};

f32 D_8097EAE4[] = { 1.0f, 100.0f, 40.0f, 5.0f, 100.0f, 40.0f, 5.0f, 100.0f, 40.0f, 5.0f };

u32 EffectSsKakera_Init(PlayState* play, u32 index, EffectSs* this, void* initParamsx) {
    EffectSsKakeraInitParams* initParams = PARAMS;

    this->pos = initParams->pos;
    this->velocity = initParams->velocity;
    this->life = initParams->life;
    this->priority = 101;
    if (initParams->dList != NULL) {
        this->gfx = initParams->dList;
        switch (initParams->objectId) {
            case GAMEPLAY_KEEP:
            case GAMEPLAY_FIELD_KEEP:
            case GAMEPLAY_DANGEON_KEEP:
                this->rObjectId = KAKERA_OBJECT_DEFAULT;
                break;
            default:
                this->rObjectId = initParams->objectId;
                EffectSsKakera_CheckForObject(this, play);
                break;
        }
    } else {
        _dbg_hungup("../z_eff_kakera.c", 193);
    }
    this->draw = EffectSsKakera_Draw;
    this->update = EffectSsKakera_Update;
    this->vec = initParams->unk_18;
    this->rReg0 = initParams->unk_2C;
    this->rGravity = initParams->gravity;
    this->rPitch = Rand_ZeroOne() * 0x8000;
    this->rYaw = Rand_ZeroOne() * 0x8000;
    this->rReg4 = initParams->unk_26;
    this->rReg5 = initParams->unk_28;
    this->rReg6 = initParams->unk_2A;
    this->rScale = initParams->scale;
    this->rReg8 = initParams->unk_30;
    this->rReg9 = initParams->unk_32;
    this->rColorIndex = initParams->colorIdx;
    return 1;
}

f32 func_8097DE30(f32 center, f32 range) {
    return (2.0f * (Rand_ZeroOne() * range) - range) + center;
}

void EffectSsKakera_Draw(PlayState* play, u32 index, EffectSs* this) {
    GraphicsContext* gfxCtx = play->state.gfxCtx;
    s16 pad;
    f32 scale = this->rScale / 256.0f;
    s32 colorIndex = this->rColorIndex;

    OPEN_DISPS(gfxCtx);
    if (this->rObjectId != KAKERA_OBJECT_DEFAULT) {
        if ((((this->rReg4 >> 7) & 1) << 7) == 0x80) {
            gSPSegment(POLY_XLU_DISP++, 0x06, play->objectCtx.slots[this->rObjectSlot].segment);
        } else {
            gSPSegment(POLY_OPA_DISP++, 0x06, play->objectCtx.slots[this->rObjectSlot].segment);
        }
    }

    Matrix_Translate(this->pos.x, this->pos.y, this->pos.z, MTXMODE_NEW);
    Matrix_RotateYS(this->rYaw, MTXMODE_APPLY);
    Matrix_RotateXS(this->rPitch, MTXMODE_APPLY);
    Matrix_Scale(scale, scale, scale, MTXMODE_APPLY);

    if ((((this->rReg4 >> 7) & 1) << 7) == 0x80) {
        MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, gfxCtx);
        Gfx_SetupDL25_Xlu(play->state.gfxCtx);
        if (colorIndex >= 0) {
            gDPSetPrimColor(POLY_XLU_DISP++, 0, D_8097EAD8[colorIndex].lod, D_8097EAD8[colorIndex].color.r,
                            D_8097EAD8[colorIndex].color.g, D_8097EAD8[colorIndex].color.b, 255);
        }
        gSPDisplayList(POLY_XLU_DISP++, this->gfx);
    } else {
        MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, gfxCtx);
        Gfx_SetupDL25_Opa(play->state.gfxCtx);
        if (colorIndex >= 0) {
            gDPSetPrimColor(POLY_OPA_DISP++, 0, D_8097EAD8[colorIndex].lod, D_8097EAD8[colorIndex].color.r,
                            D_8097EAD8[colorIndex].color.g, D_8097EAD8[colorIndex].color.b, 255);
        }
        gSPDisplayList(POLY_OPA_DISP++, this->gfx);
    }
    CLOSE_DISPS(gfxCtx);
}

void EffectSsKakera_CheckForObject(EffectSs* this, PlayState* play) {
    this->rObjectSlot = Object_GetSlot(&play->objectCtx, this->rObjectId);
    if ((this->rObjectSlot <= OBJECT_SLOT_NONE) || (!Object_IsLoaded(&play->objectCtx, this->rObjectSlot))) {
        this->life = 0;
        this->draw = NULL;
    }
}

void func_8097E19C(EffectSs* this) {
    f32 range;
    f32 temp_f12;
    f32 temp_f16;
    f32 temp_f2;
    f32 temp_f18;
    f32 temp_f0;
    f32 temp_f20;
    f32 temp_f2_2;

    temp_f18 = this->rReg5 / 1024.0f;
    temp_f20 = this->rReg6 / 1024.0f;
    range = (this->rReg9 / 1024.0f) * 4.0f;
    temp_f2 = this->velocity.x - func_8097DE30(0.0f, range);
    temp_f16 = this->velocity.y - func_8097DE30(0.0f, range);
    temp_f12 = this->velocity.z - func_8097DE30(0.0f, range);
    temp_f0 = temp_f2 * temp_f18;
    temp_f2_2 = SQ(temp_f2) * temp_f20;
    if (temp_f2 > 0.0f) {
        this->velocity.x -= (temp_f0 + temp_f2_2);
    } else {
        this->velocity.x -= (temp_f0 - temp_f2_2);
    }
    temp_f0 = temp_f16 * temp_f18;
    temp_f2_2 = SQ(temp_f16) * temp_f20;
    if (temp_f16 > 0.0f) {
        this->velocity.y -= (temp_f0 + temp_f2_2);
    } else {
        this->velocity.y -= (temp_f0 - temp_f2_2);
    }
    if (temp_f12 > 0.0f) {
        this->velocity.z -= (temp_f0 + temp_f2_2);
    } else {
        this->velocity.z -= (temp_f0 - temp_f2_2);
    }
}

void func_8097E34C(EffectSs* this) {
    this->accel.x = this->accel.y = this->accel.z = 0.0f;
}

f32 func_8097E368(f32 arg0, s32 arg1) {
    return 1.0f;
}

f32 func_8097E384(f32 arg0, s32 index) {
    if (D_8097EAE4[index] < arg0) {
        return D_8097EAE4[index] / arg0;
    } else {
        return 1.0f;
    }
}

f32 func_8097E3C0(f32 arg0, s32 index) {
    f32 square = SQ(arg0);

    if (D_8097EAE4[index] < square) {
        return D_8097EAE4[index] / square;
    } else {
        return 1.0f;
    }
}

f32 func_8097E400(f32 arg0, s32 index) {
    return func_8097E3C0(arg0, index);
}

s32 func_8097E420(EffectSs* this, Vec3f* diff, f32 distance) {
    static f32 D_8097EB0C[] = { 0.05f, 1.0f };
    f32 phi_f0;
    s32 index = this->rReg0 & 3;

    if (index != 0) {
        if (distance > 1.0f) {
            phi_f0 = 1.0f / distance;
        } else {
            phi_f0 = 1.0f;
        }
        this->accel.x += D_8097EB0C[index - 1] * diff->z * phi_f0;
        this->accel.z -= D_8097EB0C[index - 1] * diff->x * phi_f0;
    }
    return true;
}

s32 func_8097E4B0(EffectSs* this, Vec3f* diff, f32 distance) {
    static f32 D_8097EB14[] = { 4.0f, 0.1f, 0.3f, 0.9f, -0.1f, -0.3f, -0.9f };
    s32 index = (this->rReg0 >> 2) & 7;

    if (index != 0) {
        this->accel.y += D_8097EB14[index];
    }
    return true;
}

s32 func_8097E4F0(EffectSs* this, Vec3f* diff, f32 distance) {
    static f32 D_8097EB30[] = { 0.1f, 1.0f, 6.0f };
    f32 phi_f0;
    s32 index = (this->rReg0 >> 5) & 3;

    if (index != 0) {
        if (distance > 1.0f) {
            phi_f0 = 1.0f / distance;
        } else {
            phi_f0 = 1.0f;
        }
        this->accel.x -= diff->x * D_8097EB30[index - 1] * phi_f0;
        this->accel.z -= diff->z * D_8097EB30[index - 1] * phi_f0;
    }
    return true;
}

s32 func_8097E584(EffectSs* this, Vec3f* diff, f32 distance) {
    static f32 (*D_8097EB3C[])(f32, s32) = {
        func_8097E368, func_8097E384, func_8097E384, func_8097E384, func_8097E3C0,
        func_8097E3C0, func_8097E3C0, func_8097E400, func_8097E400, func_8097E400,
    };
    f32 center;
    f32 rand;
    s32 index;

    index = (this->rReg0 >> 7) & 0xF;
    center = D_8097EB3C[index](distance, index);
    rand = func_8097DE30(center, (this->rReg9 * center) / 1024.0f);
    this->accel.x *= rand;
    this->accel.y *= rand;
    this->accel.z *= rand;
    this->accel.x += rand * 0.01f;
    this->accel.y += rand * 0.01f;
    this->accel.z += rand * 0.01f;
    return true;
}

s32 func_8097E660(EffectSs* this, Vec3f* diff, f32 distance) {
    this->accel.y += this->rGravity / 256.0f;
    return true;
}

s32 func_8097E698(EffectSs* this) {
    Vec3f diff;
    f32 distance;

    func_8097E34C(this);
    diff.x = this->pos.x - this->vec.x;
    diff.y = this->pos.y - this->vec.y;
    diff.z = this->pos.z - this->vec.z;
    distance = sqrtf(SQXYZ(diff));
    if (distance > 1000.0f) {
        return false;
    }
    if (this->rReg0 != 0) {
        if (!func_8097E420(this, &diff, distance)) {
            return false;
        }
        if (!func_8097E4B0(this, &diff, distance)) {
            return false;
        }
        if (!func_8097E4F0(this, &diff, distance)) {
            return false;
        }
        if (!func_8097E584(this, &diff, distance)) {
            return false;
        }
    }
    if (!func_8097E660(this, &diff, distance)) {
        return false;
    } else {
        return true;
    }
}

void func_8097E7E0(EffectSs* this, PlayState* play) {
    static f32 D_8097EB64[] = { 10.0f, 20.0f, 40.0f };
    Player* player = GET_PLAYER(play);

    if (this->rReg8 == 0) {
        if ((((this->rReg4 >> 4) & 1) << 4) == (1 << 4)) {
            if (this->pos.y <= (player->actor.floorHeight - ((this->rReg4 >> 2) & 3))) {
                this->rReg9 = 0;
                this->rReg0 = 0;
                this->rReg4 &= ~0x60;
                this->accel.x = this->accel.y = this->accel.z = 0.0f;
                this->velocity.x = this->velocity.y = this->velocity.z = 0.0f;
                this->rReg5 = this->rReg9;
                this->rGravity = this->rReg9;
            }
        } else {
            if (this->pos.y <= ((player->actor.floorHeight - ((this->rReg4 >> 2) & 3)) - 600.0f)) {
                this->life = 0;
            }
        }
    } else {
        switch (this->rReg4 & 3) {
            case 0:
                this->rReg8 = 0;
                break;
            case 1:
                if ((this->velocity.y < 0.0f) &&
                    (BgCheck_SphVsFirstPoly(&play->colCtx, &this->pos, D_8097EB64[(this->rReg4 >> 2) & 3]))) {
                    this->velocity.x *= func_8097DE30(0.9f, 0.2f);
                    this->velocity.y *= -0.8f;
                    this->velocity.z *= func_8097DE30(0.9f, 0.2f);
                    if (this->rReg8 > 0) {
                        this->rReg8--;
                    }
                }
                break;
            case 2:
                if (BgCheck_SphVsFirstPoly(&play->colCtx, &this->pos, D_8097EB64[(this->rReg4 >> 2) & 3])) {
                    this->rReg8 = 0;
                }
                break;
        }
    }
}

void EffectSsKakera_Update(PlayState* play, u32 index, EffectSs* this) {
    switch (((this->rReg4 >> 5) & 3) << 5) {
        case 0x20:
            this->rPitch += 0x47B;
            this->rYaw += 0x139;
            break;
        case 0x40:
            this->rPitch += 0x1A7C;
            this->rYaw += 0x47B;
            break;
        case 0x60:
            this->rPitch += 0x3F27;
            this->rYaw += 0xCA1;
            break;
    }
    func_8097E19C(this);
    if (!func_8097E698(this)) {
        this->life = 0;
    }
    func_8097E7E0(this, play);
    if (this->rObjectId != KAKERA_OBJECT_DEFAULT) {
        EffectSsKakera_CheckForObject(this, play);
    }
}
