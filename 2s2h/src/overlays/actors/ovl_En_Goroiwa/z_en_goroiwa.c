/*
 * File: z_en_goroiwa.c
 * Overlay: ovl_En_Goroiwa
 * Description: Rolling boulders
 */

#include "z_en_goroiwa.h"
#include "z64quake.h"
#include "objects/object_goroiwa/object_goroiwa.h"
#include "objects/gameplay_keep/gameplay_keep.h"

#include "2s2h/Enhancements/FrameInterpolation/FrameInterpolation.h"

#define FLAGS (ACTOR_FLAG_UPDATE_CULLING_DISABLED | ACTOR_FLAG_MINIMAP_ICON_ENABLED)

void EnGoroiwa_Init(Actor* thisx, PlayState* play);
void EnGoroiwa_Destroy(Actor* thisx, PlayState* play);
void EnGoroiwa_Update(Actor* thisx, PlayState* play);
void EnGoroiwa_Draw(Actor* thisx, PlayState* play);

void func_809419D0(EnGoroiwa* this);
void func_80941A10(EnGoroiwa* this, PlayState* play);
void func_809421E0(EnGoroiwa* this);
void func_8094220C(EnGoroiwa* this, PlayState* play);
void func_809425CC(EnGoroiwa* this);
void func_80942604(EnGoroiwa* this, PlayState* play);
void func_80941DB4(EnGoroiwa* this);
void func_80941E28(EnGoroiwa* this, PlayState* play);
void func_80941EB4(EnGoroiwa* this);
void func_80941F10(EnGoroiwa* this, PlayState* play);
void func_80941F54(EnGoroiwa* this);
void func_80941FA4(EnGoroiwa* this, PlayState* play);
void func_80942084(EnGoroiwa* this);
void func_809420F0(EnGoroiwa* this, PlayState* play);

ActorProfile En_Goroiwa_Profile = {
    /**/ ACTOR_EN_GOROIWA,
    /**/ ACTORCAT_PROP,
    /**/ FLAGS,
    /**/ OBJECT_GOROIWA,
    /**/ sizeof(EnGoroiwa),
    /**/ EnGoroiwa_Init,
    /**/ EnGoroiwa_Destroy,
    /**/ EnGoroiwa_Update,
    /**/ EnGoroiwa_Draw,
};

static ColliderJntSphElementInit sJntSphElementsInit[1] = {
    {
        {
            ELEM_MATERIAL_UNK0,
            { 0x20000000, 0x00, 0x04 },
            { 0x01C37BB6, 0x00, 0x00 },
            ATELEM_ON | ATELEM_SFX_NONE,
            ACELEM_ON,
            OCELEM_ON,
        },
        { 0, { { 0, 0, 0 }, 58 }, 100 },
    },
};

static ColliderJntSphInit sJntSphInit = {
    {
        COL_MATERIAL_METAL,
        AT_ON | AT_TYPE_ENEMY,
        AC_ON | AC_HARD | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_2,
        COLSHAPE_JNTSPH,
    },
    ARRAY_COUNT(sJntSphElementsInit),
    sJntSphElementsInit,
};

static CollisionCheckInfoInit sColChkInfoInit = { 0, 12, 60, MASS_HEAVY };

static f32 D_80942DFC[] = {
    10.0f,
    12.0f,
    14.0f,
    10.0f,
};

static Gfx* D_80942E0C[][3] = {
    { object_goroiwa_DL_0042B0, object_goroiwa_DL_004960, object_goroiwa_DL_004EF0 },
    { object_goroiwa_DL_0003B0, object_goroiwa_DL_0028E0, object_goroiwa_DL_002D70 },
    { object_goroiwa_DL_0072F0, object_goroiwa_DL_0077D0, object_goroiwa_DL_007C60 },
};

static Color_RGBA8 D_80942E30[] = {
    { 190, 195, 200, 255 },
    { 170, 130, 90, 255 },
    { 250, 250, 250, 255 },
};

static Color_RGBA8 D_80942E3C[] = {
    { 130, 135, 140, 255 },
    { 100, 60, 20, 255 },
    { 180, 180, 180, 255 },
};

static Vec3f D_80942E48 = { 0.0f, 0.0f, 0.0f };
static Vec3f D_80942E54 = { 0.0f, 0.3f, 0.0f };
static Vec3f D_80942E60 = { 0.0f, 1.0f, 0.0f };
static Vec3f D_80942E6C = { 0.0f, 0.0f, 1.0f };

static InitChainEntry sInitChain[] = {
    ICHAIN_F32_DIV1000(gravity, -900, ICHAIN_CONTINUE),
    ICHAIN_F32_DIV1000(terminalVelocity, -26000, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeDistance, 2000, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeScale, 160, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeDownward, 350, ICHAIN_STOP),
};

static UNK_TYPE D_80942E8C[] = {
    0x0000003B,
    0x1A1A2864,
};

void func_8093E8A0(EnGoroiwa* this) {
    s32 params = ENGOROIWA_GET_3000(&this->actor);
    f32 phi_f2;

    if (params == ENGOROIWA_3000_0) {
        phi_f2 = 0.1f;
    } else if (params == ENGOROIWA_3000_1) {
        phi_f2 = 0.05f;
    } else {
        phi_f2 = (Rand_ZeroOne() * .04f) + 0.04f;
    }

    Actor_SetScale(&this->actor, phi_f2);
}

void func_8093E91C(EnGoroiwa* this) {
    this->unk_1DC = this->actor.scale.x * 595.0f;
}

void func_8093E938(EnGoroiwa* this) {
    Sphere16* worldSphere = &this->collider.elements[0].dim.worldSphere;

    worldSphere->center.x = this->actor.world.pos.x;
    worldSphere->center.y = this->actor.world.pos.y + this->unk_1DC;
    worldSphere->center.z = this->actor.world.pos.z;
    this->collider.elements[0].dim.worldSphere.radius = this->unk_1DC - 1.0f;
}

void func_8093E9B0(EnGoroiwa* this, PlayState* play) {
    s32 pad[2];
    s32 params = ENGOROIWA_GET_C000(&this->actor);

    Collider_InitJntSph(play, &this->collider);
    Collider_SetJntSph(play, &this->collider, &this->actor, &sJntSphInit, this->colliderElements);
    func_8093E938(this);
    this->collider.elements[0].dim.worldSphere.radius = this->unk_1DC - 1.0f;

    if ((params == ENGOROIWA_C000_1) || (params == ENGOROIWA_C000_2)) {
        this->collider.elements[0].base.acDmgInfo.dmgFlags |= (0x4000 | 0x400 | 0x100);
        if (params == ENGOROIWA_C000_1) {
            this->collider.base.colMaterial = COL_MATERIAL_WOOD;
        } else {
            this->collider.elements[0].base.acDmgInfo.dmgFlags &= ~(0x400000 | 0x200 | 0x2);
            this->collider.elements[0].base.acDmgInfo.dmgFlags |= (0x80000000 | 0x800 | 0x8);
            this->collider.base.colMaterial = COL_MATERIAL_NONE;
        }
    }
}

void func_8093EAB0(EnGoroiwa* this, u8 arg0) {
    this->unk_1E5 &= ~7;
    this->unk_1E5 |= arg0;
}

s32 func_8093EAD4(Vec3f* arg0, Vec3f* arg1) {
    f32 temp_f0 = Math3D_Vec3fMagnitude(arg1);

    if (temp_f0 < 0.001f) {
        return false;
    }
    arg0->x = arg1->x * (1.0f / temp_f0);
    arg0->y = arg1->y * (1.0f / temp_f0);
    arg0->z = arg1->z * (1.0f / temp_f0);
    return true;
}

void func_8093EB58(EnGoroiwa* this, PlayState* play) {
    this->unk_1E4 = (this->actor.home.rot.x >> 1) & 3;
}

f32 func_8093EB74(EnGoroiwa* this, PlayState* play) {
    s32 i;
    Path* path = &play->setupPathList[ENGOROIWA_GET_PATH_INDEX(&this->actor)];
    s32 temp_s4 = path->count;
    f32 temp_f20 = 0.0f;
    Vec3f sp54;
    Vec3f sp48;

    Math_Vec3s_ToVec3f(&sp48, &this->pathPoints[0]);

    for (i = 1; i < temp_s4; i++) {
        Math_Vec3f_Copy(&sp54, &sp48);
        Math_Vec3s_ToVec3f(&sp48, &this->pathPoints[i]);
        temp_f20 += Math3D_Vec3f_DistXYZ(&sp54, &sp48);
    }

    return temp_f20;
}

void func_8093EC50(EnGoroiwa* this) {
    Vec3s* var = &this->pathPoints[this->unk_1D8];
    Vec3f sp18;

    sp18.x = var->x;
    sp18.y = var->y;
    sp18.z = var->z;
    this->actor.world.rot.y = Math_Vec3f_Yaw(&this->actor.world.pos, &sp18);
}

void func_8093ECD4(EnGoroiwa* this) {
    s32 params = ENGOROIWA_GET_300(&this->actor);

    if (this->unk_1D8 < 0) {
        if ((params == ENGOROIWA_300_0) || (params == ENGOROIWA_300_1)) {
            this->unk_1D8 = this->unk_1D4 - 1;
            this->unk_1DA = -1;
            this->unk_1D6 = this->unk_1D4;
        } else if (params == ENGOROIWA_300_3) {
            this->unk_1D6 = 0;
            this->unk_1D8 = 1;
            this->unk_1DA = 1;
        }
    } else if (this->unk_1D4 < this->unk_1D8) {
        if ((params == ENGOROIWA_300_0) || (params == ENGOROIWA_300_1)) {
            this->unk_1D6 = 0;
            this->unk_1D8 = 1;
            this->unk_1DA = 1;
        } else if (params == ENGOROIWA_300_3) {
            this->unk_1D6 = this->unk_1D4;
            this->unk_1D8 = this->unk_1D4 - 1;
            this->unk_1DA = -1;
        }
    }
}

void func_8093ED80(EnGoroiwa* this) {
    this->unk_1D6 = this->unk_1D8;
    this->unk_1D8 += this->unk_1DA;
    func_8093ECD4(this);
}

void func_8093EDB0(EnGoroiwa* this) {
    this->unk_1DA = -this->unk_1DA;
    this->unk_1D6 = this->unk_1D8;
    this->unk_1D8 += this->unk_1DA;
}

void func_8093EDD8(EnGoroiwa* this, PlayState* play) {
    this->unk_1D4 = play->setupPathList[ENGOROIWA_GET_PATH_INDEX(&this->actor)].count - 1;
    this->unk_1D6 = 0;
    this->unk_1D8 = 1;
    this->unk_1DA = 1;
}

void func_8093EE18(EnGoroiwa* this, PlayState* play) {
    this->unk_1D4 = play->setupPathList[ENGOROIWA_GET_PATH_INDEX(&this->actor)].count - 1;
    this->unk_1D6 = this->actor.home.rot.y;
    this->unk_1D8 = this->unk_1D6 + 1;
    this->unk_1DA = 1;
}

void func_8093EE64(EnGoroiwa* this, s32 arg1) {
    Vec3s* temp_v0 = &this->pathPoints[arg1];

    this->actor.world.pos.x = temp_v0->x;
    this->actor.world.pos.y = temp_v0->y;
    this->actor.world.pos.z = temp_v0->z;
}

void func_8093EEBC(EnGoroiwa* this) {
    this->unk_1B4.x = 1.0f;
    this->unk_1C4 = 1.0f;
    this->unk_1A8.x = 1.0f;
}

s32 func_8093EEDC(EnGoroiwa* this) {
    Vec3s* temp_v1 = &this->pathPoints[this->unk_1D8];
    Vec3s* temp_v0 = &this->pathPoints[this->unk_1D6];

    if ((temp_v1->x == temp_v0->x) && (temp_v1->z == temp_v0->z)) {
        if (temp_v0->y < temp_v1->y) {
            return 1;
        }
        return -1;
    }
    return 0;
}

void func_8093EF54(PlayState* play, Vec3f* arg1, Color_RGBA8* arg2, Color_RGBA8* arg3, f32 arg4) {
    Vec3f spC4;
    f32 temp_f0;
    f32 temp_f20;
    f32 temp_f24;
    f32 temp_f26;
    s32 temp_f16;
    s16 phi_s0;
    s16 temp;
    f32 temp_f28;
    f32 temp_f30;
    s32 i;

    temp_f0 = (arg4 + 0.1f) * 0.5f;
    temp_f16 = temp_f0 * 165.0f;
    temp_f24 = 470.0f * arg4;
    temp_f30 = 400.0f * arg4;
    temp_f26 = 800.0f * temp_f0;
    temp_f28 = 600.0f * temp_f0;
    temp = 0x10000 / temp_f16;

    for (i = 0, phi_s0 = 0; i < temp_f16; i++, phi_s0 += temp) {
        // clang-format off
        temp_f20 = Rand_ZeroOne(); spC4.x = (Math_SinS(phi_s0) * (temp_f24 * ((temp_f20 * 0.5f) + 0.5f))) + arg1->x;
        spC4.y = ((Rand_ZeroOne() - 0.5f) * temp_f30) + arg1->y;
        temp_f20 = Rand_ZeroOne(); spC4.z = (Math_CosS(phi_s0) * (temp_f24 * ((temp_f20 * 0.5f) + 0.5f))) + arg1->z;
        // clang-format on

        func_800B0E48(play, &spC4, &D_80942E48, &D_80942E54, arg2, arg3, (Rand_ZeroOne() * temp_f28) + temp_f26,
                      temp_f26);
    }
}

void func_8093F198(PlayState* play, Vec3f* arg1, f32 arg2) {
    Vec3f sp74;
    f32 temp_f20;
    s32 temp_f16;
    s16 temp_lo;
    s16 phi_s0;
    s32 i;

    temp_f16 = (arg2 + 0.1f) * 58.0f;
    sp74.y = arg1->y;
    temp_f20 = 550.0f * arg2;
    temp_lo = 0x10000 / temp_f16;

    for (i = 0, phi_s0 = 0; i < temp_f16; i++, phi_s0 += temp_lo) {
        sp74.x = arg1->x + (Math_SinS((s32)(Rand_ZeroOne() * temp_lo) + phi_s0) * temp_f20);
        sp74.z = arg1->z + (Math_CosS((s32)(Rand_ZeroOne() * temp_lo) + phi_s0) * temp_f20);
        EffectSsGSplash_Spawn(play, &sp74, NULL, NULL, 0, 0x15E);
    }

    EffectSsGRipple_Spawn(play, arg1, 0x1F4, 0x384, 4);
}

s32 func_8093F34C(EnGoroiwa* this) {
    s32 pad[3];
    Vec3s* temp_v0;
    f32 tempX;
    f32 tempZ;
    f32 x;
    f32 z;

    Math_StepToF(&this->actor.speed, D_80942DFC[this->unk_1E4], 0.3f);
    Actor_UpdateVelocityWithGravity(&this->actor);
    temp_v0 = &this->pathPoints[this->unk_1D8];
    this->actor.velocity.y *= 0.97f;
    x = temp_v0->x;
    z = temp_v0->z;

    tempX = x - this->actor.world.pos.x;
    tempZ = z - this->actor.world.pos.z;

    if (SQ(this->actor.speed + 1.0f) < (SQ(tempX) + SQ(tempZ))) {
        this->actor.world.pos.x += this->actor.velocity.x;
        this->actor.world.pos.y += this->actor.velocity.y;
        this->actor.world.pos.z += this->actor.velocity.z;
        this->unk_1E5 &= ~0x40;
        return false;
    }

    this->actor.world.pos.x = x;
    this->actor.world.pos.y += this->actor.velocity.y;
    this->actor.world.pos.z = z;
    this->unk_1E5 |= 0x40;
    return true;
}

s32 func_8093F498(EnGoroiwa* this) {
    s32 pad;
    f32 temp_f0;
    Vec3f sp2C;
    Vec3s* temp_v0 = &this->pathPoints[this->unk_1D8];

    sp2C.x = temp_v0->x;
    sp2C.y = temp_v0->y;
    sp2C.z = temp_v0->z;

    Math_StepToF(&this->actor.speed, D_80942DFC[this->unk_1E4], 0.3f);
    Math_Vec3f_Diff(&sp2C, &this->actor.world.pos, &this->actor.velocity);

    temp_f0 = Math3D_Vec3fMagnitude(&this->actor.velocity);
    if ((this->actor.speed + 1.0f) < temp_f0) {
        Math_Vec3f_Scale(&this->actor.velocity, this->actor.speed / temp_f0);
        this->actor.world.pos.x += this->actor.velocity.x;
        this->actor.world.pos.y += this->actor.velocity.y;
        this->actor.world.pos.z += this->actor.velocity.z;
        this->unk_1E5 &= ~0x40;
        return false;
    }

    this->actor.world.pos.x = sp2C.x;
    this->actor.world.pos.y = sp2C.y;
    this->actor.world.pos.z = sp2C.z;
    this->unk_1E5 |= 0x40;
    return true;
}

s32 func_8093F5EC(EnGoroiwa* this) {
    s32 pad;
    Vec3s* sp18 = &this->pathPoints[this->unk_1D8];

    if (this->actor.velocity.y < 0.0f) {
        this->actor.velocity.y = 0.0f;
    }

    Math_StepToF(&this->actor.velocity.y, D_80942DFC[this->unk_1E4] * 0.5f, 0.18f);
    this->actor.world.pos.x = sp18->x;
    this->actor.world.pos.z = sp18->z;

    if (Math_StepToF(&this->actor.world.pos.y, sp18->y, fabsf(this->actor.velocity.y))) {
        this->unk_1E5 |= 0x40;
        return true;
    }

    this->unk_1E5 &= ~0x40;
    return false;
}

s32 func_8093F6F8(EnGoroiwa* this, PlayState* play) {
    f32 temp_f14;
    Vec3s* sp80 = &this->pathPoints[this->unk_1D8];
    f32 sp7C = sp80->y;
    f32 sp78;
    f32 temp_f2;

    Math_StepToF(&this->actor.velocity.y, -18.367346f, 1.0f);
    this->actor.velocity.y *= 0.98f;
    this->actor.world.pos.x = sp80->x;
    this->actor.world.pos.z = sp80->z;
    sp78 = this->actor.world.pos.y;
    this->actor.world.pos.y += this->actor.velocity.y;

    if ((this->actor.velocity.y < 0.0f) && (this->actor.world.pos.y <= sp7C)) {
        if (this->unk_1CA == 0) {
            if (this->actor.xzDistToPlayer < 400.0f) {
                s16 quakeIndex = Quake_Request(GET_ACTIVE_CAM(play), QUAKE_TYPE_3);

                Quake_SetSpeed(quakeIndex, 17232);
                Quake_SetPerturbations(quakeIndex, 3, 0, 0, 0);
                Quake_SetDuration(quakeIndex, 7);
            }

            this->unk_1C4 = 0.0f;

            if (!(this->unk_1E5 & 0x20)) {
                CollisionPoly* poly;
                Vec3f sp60;
                s32 pad[2];
                s32 bgId;
                Vec3f sp48;

                sp60.x = this->actor.world.pos.x;
                sp60.y = this->actor.world.pos.y + 50.0f;
                sp60.z = this->actor.world.pos.z;

                temp_f14 = BgCheck_EntityRaycastFloor5_2(play, &play->colCtx, &poly, &bgId, &this->actor, &sp60);
                temp_f2 = temp_f14 - this->actor.world.pos.y;

                if (fabsf(temp_f2) < (fabsf(this->actor.velocity.y) + 0.01f)) {
                    if (this->actor.flags & ACTOR_FLAG_INSIDE_CULLING_VOLUME) {
                        sp48.x = this->actor.world.pos.x;
                        sp48.y = temp_f14 + 10.0f;
                        sp48.z = this->actor.world.pos.z;

                        func_8093EF54(play, &sp48, &D_80942E30[ENGOROIWA_GET_C000(&this->actor)],
                                      &D_80942E3C[ENGOROIWA_GET_C000(&this->actor)], this->actor.scale.x);
                    }
                    Actor_PlaySfx(&this->actor, NA_SE_EV_BIGBALL_BOUND);
                }
            }
        }

        if (this->unk_1CA > 0) {
            this->unk_1E5 |= 0x40;
            return true;
        }

        this->unk_1CA++;
        this->actor.velocity.y *= -0.3f;
        this->actor.world.pos.y = sp7C - ((this->actor.world.pos.y - sp7C) * 0.3f);
    }

    if (this->unk_1CA == 0) {
        WaterBox* waterBox;
        f32 sp40;

        if (WaterBox_GetSurface1_2(play, &play->colCtx, this->actor.world.pos.x, this->actor.world.pos.z, &sp40,
                                   &waterBox)) {
            if ((this->actor.world.pos.y + this->unk_1DC) <= sp40) {
                this->unk_1E5 |= 0x20;
                if (sp40 < (this->unk_1DC + sp78)) {
                    if (this->actor.flags & ACTOR_FLAG_INSIDE_CULLING_VOLUME) {
                        Vec3f sp34;

                        sp34.x = this->actor.world.pos.x;
                        sp34.y = sp40;
                        sp34.z = this->actor.world.pos.z;
                        func_8093F198(play, &sp34, this->actor.scale.x);
                    }
                    this->actor.velocity.y *= 0.2f;
                }

                if (this->actor.velocity.y < -8.0f) {
                    this->actor.velocity.y = -8.0f;
                }
            }
        }
    }

    this->unk_1E5 &= ~0x40;
    return false;
}

void func_8093FAA4(EnGoroiwa* this, PlayState* play) {
    f32 temp;
    f32 tmp2;
    f32 sp7C;
    Vec3f sp70;
    Vec3f sp64;
    MtxF sp24;

    if (this->unk_1E5 & 0x10) {
        sp7C = this->unk_1C0;
    } else {
        temp = Math3D_Vec3f_DistXYZ(&this->actor.world.pos, &this->actor.prevPos);
        tmp2 = temp / this->unk_1DC;
        this->unk_1C0 = tmp2;
        sp7C = this->unk_1C0;
    }

    sp7C *= this->unk_1C4;
    if (!(this->unk_1E5 & 0x10)) {
        if (Math3D_Vec3fMagnitudeSq(&this->actor.velocity) > 0.1f) {
            Math_Vec3f_Copy(&this->unk_1A8, &this->actor.velocity);
        }
    }

    Math3D_Vec3f_Cross(&D_80942E60, &this->unk_1A8, &sp70);

    if (func_8093EAD4(&sp64, &sp70)) {
        this->unk_1B4 = sp64;
    } else {
        sp64 = this->unk_1B4;
    }

    Matrix_RotateAxisF(sp7C, &sp64, MTXMODE_NEW);
    Matrix_RotateYS(this->actor.shape.rot.y, MTXMODE_APPLY);
    Matrix_RotateXS(this->actor.shape.rot.x, MTXMODE_APPLY);
    Matrix_RotateZS(this->actor.shape.rot.z, MTXMODE_APPLY);
    Matrix_Get(&sp24);
    Matrix_MtxFToYXZRot(&sp24, &this->actor.shape.rot, false);
}

void func_8093FC00(EnGoroiwa* this) {
    s32 params = ENGOROIWA_GET_300(&this->actor);

    func_8093ED80(this);
    if (((params == ENGOROIWA_300_0) || (params == ENGOROIWA_300_1)) &&
        ((this->unk_1D6 == 0) || (this->unk_1D6 == this->unk_1D4))) {
        func_8093EE64(this, this->unk_1D6);
    }
}

void func_8093FC6C(EnGoroiwa* this, PlayState* play) {
    s32 i;
    Vec3f spC0;
    Vec3f spB4;
    Vec3f spA8;
    s16 temp_s3;
    s16 phi_s0;
    s16 temp;
    s32 pad;
    s32 sp80;
    f32 temp_f20;
    f32 temp_f22;
    f32 temp_f24;
    f32 temp_f28;

    temp_f24 = (this->actor.scale.x + 0.1f) * 0.5f;
    sp80 = temp_f24 * 150.0f;
    temp_f28 = this->actor.scale.x * 600.0f;
    temp = 0x10000 / sp80;

    for (i = 0, phi_s0 = 0; i < sp80; i++, phi_s0 += temp) {
        temp_s3 = Rand_Next() >> 0x10;
        temp_f20 = Math_SinS(temp_s3);
        temp_f22 = Math_CosS(temp_s3);

        spC0.x = Math_SinS(phi_s0);
        spC0.z = Math_CosS(phi_s0);

        spB4.x = 2.0f * spC0.x;
        spB4.y = (2.0f * Rand_ZeroOne()) + 1.0f;
        spB4.z = 2.0f * spC0.z;

        spC0.x *= temp_f22 * temp_f28;
        spC0.y = temp_f20 * temp_f28;
        spC0.z *= (temp_f22 * temp_f28);

        spC0.x += this->actor.world.pos.x;
        spC0.y += this->actor.world.pos.y;
        spC0.z += this->actor.world.pos.z;

        spA8.x = spB4.x * -0.02f;
        spA8.y = spB4.y * -0.05f;
        spA8.z = spB4.z * -0.02f;

        EffectSsIceSmoke_Spawn(play, &spC0, &spB4, &spA8, ((s32)(Rand_ZeroOne() * 170.0f) + 150) * temp_f24 * 10.0f);
    }

    for (i = 0, phi_s0 = 0; i < sp80; i++, phi_s0 += temp) {
        temp_f20 = (Rand_ZeroOne() * (450.0f * this->actor.scale.x)) + 50.0f;

        spC0.x = Math_SinS(phi_s0);
        spC0.z = Math_CosS(phi_s0);

        spB4.x = spC0.x * 30.0f * temp_f24;
        spB4.y = 0.0f;
        spB4.z = spC0.z * 30.0f * temp_f24;

        spA8.x = spB4.x * -0.02f;
        spA8.y = 0.03f;
        spA8.z = spB4.z * -0.02f;

        spC0.x = (spC0.x * temp_f20) + this->actor.world.pos.x;
        spC0.y = (Rand_ZeroOne() * 20.0f) + this->actor.world.pos.y;
        spC0.z = (spC0.z * temp_f20) + this->actor.world.pos.z;

        EffectSsIceSmoke_Spawn(play, &spC0, &spB4, &spA8, (s32)(Rand_ZeroOne() * 1400.0f * temp_f24) + 100);
    }
}

void func_80940090(EnGoroiwa* this, PlayState* play) {
    s32 pad[3];
    s32 sp120 = ENGOROIWA_GET_C000(&this->actor);
    s32 phi_s0;
    s32 phi_s3;
    Vec3f sp10C;
    Vec3f sp100;
    Vec3f spF4;
    s32 phi_s6;
    f32 spEC;
    Gfx* phi_s1;
    s16 phi_s7;
    f32 phi_f22;
    s32 phi_s2;
    s32 spD8;
    s16 spA8;
    f32 temp_f20;
    s32 i;

    if (!(this->actor.flags & ACTOR_FLAG_INSIDE_CULLING_VOLUME)) {
        return;
    }

    spEC = (this->actor.scale.x + 0.1f) * 0.5f;
    sp10C.x = this->actor.world.pos.x;
    sp10C.y = this->actor.world.pos.y + this->unk_1DC;
    sp10C.z = this->actor.world.pos.z;
    spD8 = spEC * 150.0f;
    spA8 = 0x10000 / spD8;

    for (i = 0, phi_s7 = 0; i < spD8; i++, phi_s7 += spA8) {
        if ((i & 3) == 0) {
            phi_f22 = 1.0f;
            phi_s1 = D_80942E0C[sp120][2];
            phi_s2 = -0x118;
            phi_s3 = 0;
            phi_s0 = 0x40;
        } else {
            phi_s2 = -0x190;
            if ((i & 3) == 1) {
                phi_s1 = D_80942E0C[sp120][1];
                phi_s2 = -0x154;
                phi_s3 = 0;
                phi_f22 = 0.9f;
                if (Rand_ZeroOne() < 0.4f) {
                    phi_s0 = 0x20;
                } else {
                    phi_s0 = 0x40;
                }
            } else {
                phi_s1 = D_80942E0C[sp120][0];
                phi_s3 = 1;
                phi_f22 = 0.8f;
                if ((s32)Rand_Next() > 0) {
                    phi_s0 = 0x21;
                } else {
                    phi_s0 = 0x41;
                }
            }
        }

        temp_f20 = (Rand_ZeroOne() * this->actor.scale.x * 400.0f) + 20.0f;

        sp100.x = Math_SinS((s32)(Rand_ZeroOne() * spA8) + phi_s7) * temp_f20;
        sp100.y = (Rand_ZeroOne() - 0.4f) * temp_f20 * 1.6666666f;
        sp100.z = Math_CosS((s32)(Rand_ZeroOne() * spA8) + phi_s7) * temp_f20;

        spF4.x = sp100.x * 0.16f * phi_f22;
        spF4.y = (Rand_ZeroOne() * 16.0f) + 3.0f;
        spF4.z = sp100.z * 0.16f * phi_f22;

        Math_Vec3f_Sum(&sp100, &sp10C, &sp100);
        EffectSsKakera_Spawn(play, &sp100, &spF4, &sp100, phi_s2, phi_s0, 30, 0, 0,
                             ((Rand_ZeroOne() * 150.0f) + 300.0f) * this->actor.scale.x, phi_s3, 0, 0x32, -1,
                             OBJECT_GOROIWA, phi_s1);

        if (this->unk_1E6) {
            continue;
        }

        sp100.x += ((Rand_ZeroOne() * 1200.0f) - 600.0f) * this->actor.scale.x;
        sp100.y += ((Rand_ZeroOne() * 1400.0f) - 600.0f) * this->actor.scale.y;
        sp100.z += ((Rand_ZeroOne() * 1200.0f) - 600.0f) * this->actor.scale.z;

        func_800B0E48(play, &sp100, &D_80942E48, &D_80942E54, &D_80942E30[sp120], &D_80942E3C[sp120],
                      (Rand_ZeroOne() * 50.0f) + (400.0f * spEC), (Rand_ZeroOne() * 60.0f) + (500.0f * spEC));
    }

    if (this->unk_1E6) {
        func_8093FC6C(this, play);
    }
}

void func_80940588(PlayState* play, Vec3f* arg1, Gfx* arg2[], Color_RGBA8* arg3, Color_RGBA8* arg4, f32 arg5) {
    Gfx* phi_s7;
    Vec3f sp100;
    Vec3f spF4;
    Vec3f spE8;
    f32 temp_f20;
    f32 spB0;
    s32 j;
    s32 i;
    s32 phi_s0;
    s32 spD0;
    s16 spCE;
    s16 spA8;
    s16 phi_fp;
    s16 spC8;
    f32 spAC;

    spD0 = (s32)(arg5 * 35.0f) + 6;
    temp_f20 = (arg5 + 0.1f) * 0.5f;
    spB0 = 550.0f * temp_f20;
    spAC = 500.0f * temp_f20;
    spA8 = 0x10000 / spD0;

    for (i = 0, spCE = 0; i < spD0; i++, spCE += spA8) {
        temp_f20 = (Rand_ZeroOne() * arg5 * 500.0f) + 20.0f;

        sp100.x = Math_SinS((s32)(Rand_ZeroOne() * spA8) + spCE) * temp_f20;
        sp100.y = (Rand_ZeroOne() - 0.4f) * temp_f20 * 1.6666666f;
        sp100.z = Math_CosS((s32)(Rand_ZeroOne() * spA8) + spCE) * temp_f20;

        spF4.x = sp100.x * 0.19f;
        spF4.y = (Rand_ZeroOne() * 16.0f) + 3.0f;
        spF4.z = sp100.z * 0.19f;

        Math_Vec3f_Sum(&sp100, arg1, &sp100);

        if ((i & 3) == 0) {
            phi_s7 = arg2[0];
            phi_fp = -0x190;
            spC8 = 1;
            if ((s32)Rand_Next() > 0) {
                phi_s0 = 0x21;
            } else {
                phi_s0 = 0x41;
            }
        } else {
            spC8 = 0;
            if ((i & 3) == 1) {
                phi_fp = -0x154;
                phi_s7 = arg2[1];
                if (Rand_ZeroOne() < 0.4f) {
                    phi_s0 = 0x20;
                } else {
                    phi_s0 = 0x40;
                }
            } else {
                phi_fp = -0x118;
                phi_s0 = 0x40;
                phi_s7 = arg2[2];
            }
        }

        EffectSsKakera_Spawn(play, &sp100, &spF4, &sp100, phi_fp, phi_s0, 30, 0, 0,
                             ((Rand_ZeroOne() * 100.0f) + 170.0f) * arg5, spC8, 0, 0x36, -1, OBJECT_GOROIWA, phi_s7);

        for (j = 0; j < 2; j++) {
            spE8.x = (((Rand_ZeroOne() * 1000.0f) - 500.0f) * arg5) + sp100.x;
            spE8.y = (((Rand_ZeroOne() * 1300.0f) - 500.0f) * arg5) + sp100.y;
            spE8.z = (((Rand_ZeroOne() * 1000.0f) - 500.0f) * arg5) + sp100.z;
            func_800B0E48(play, &spE8, &D_80942E48, &D_80942E54, arg3, arg4, (Rand_ZeroOne() * 80.0f) + spB0,
                          (Rand_ZeroOne() * 70.0f) + spAC);
        }
    }
}

void func_80940A1C(PlayState* play, Vec3f* arg1, Gfx** arg2, Color_RGBA8* arg3, Color_RGBA8* arg4, f32 arg5) {
    s32 i;
    Vec3f spE8;
    Vec3f spDC;
    s32 phi_s2;
    s32 phi_s0;
    s32 phi_s3;
    Gfx* phi_s1;
    s32 spC8;
    f32 temp_f20;
    f32 spAC;
    s16 spA8;
    s16 phi_s6;

    spC8 = (s32)(arg5 * 35.0f) + 5;
    temp_f20 = (arg5 + 0.1f) * 0.5f;
    spAC = 600.0f * temp_f20;
    spA8 = 0x10000 / spC8;

    for (i = 0, phi_s6 = 0; i < spC8; i++, phi_s6 += spA8) {
        temp_f20 = (Rand_ZeroOne() * arg5 * 400.0f) + 20.0f;

        spE8.x = Math_SinS((s32)(Rand_ZeroOne() * spA8) + phi_s6) * temp_f20;
        spE8.y = (Rand_ZeroOne() - 0.2f) * temp_f20 * 0.8f;
        spE8.z = Math_CosS((s32)(Rand_ZeroOne() * spA8) + phi_s6) * temp_f20;

        spDC.x = spE8.x * 0.17f;
        spDC.y = (Rand_ZeroOne() * 14.0f) + 3.0f;
        spDC.z = spE8.z * 0.17f;

        Math_Vec3f_Sum(&spE8, arg1, &spE8);

        if ((i & 3) == 0) {
            phi_s1 = arg2[2];
            phi_s2 = -0x118;
            phi_s3 = 0;
            phi_s0 = 0x40;
        } else {
            phi_s3 = 1;
            if ((i & 3) == 1) {
                phi_s1 = arg2[1];
                phi_s2 = -0x154;
                if ((s32)Rand_Next() > 0) {
                    phi_s0 = 0x21;
                } else {
                    phi_s0 = 0x41;
                }
            } else {
                phi_s1 = arg2[0];
                phi_s2 = -0x190;
                if ((s32)Rand_Next() > 0) {
                    phi_s0 = 0x21;
                } else {
                    phi_s0 = 0x41;
                }
            }
        }

        EffectSsKakera_Spawn(play, &spE8, &spDC, &spE8, phi_s2, phi_s0, 30, 0, 0,
                             ((Rand_ZeroOne() * 150.0f) + 250.0f) * arg5, phi_s3, 0, 0x36, -1, OBJECT_GOROIWA, phi_s1);

        spE8.x += ((Rand_ZeroOne() * 800.0f) - 400.0f) * arg5;
        spE8.y += ((Rand_ZeroOne() * 800.0f) - 250.0f) * arg5;
        spE8.z += ((Rand_ZeroOne() * 800.0f) - 400.0f) * arg5;

        func_800B0E48(play, &spE8, &D_80942E48, &D_80942E54, arg3, arg4, (Rand_ZeroOne() * 60.0f) + spAC,
                      (Rand_ZeroOne() * 30.0f) + spAC);
    }
}

void func_80940E38(EnGoroiwa* this, PlayState* play) {
    f32 sp5C;
    s32 pad;
    f32 sp54;
    Vec3f sp48;
    s16 sp46;
    s16 temp_a0;

    if (this->actor.flags & ACTOR_FLAG_INSIDE_CULLING_VOLUME) {
        if (this->actor.xzDistToPlayer < 1000.0f) {
            sp5C = (1000.0f - this->actor.xzDistToPlayer) * 0.0012f * (this->actor.speed * 0.1f);
            if (Rand_ZeroOne() < sp5C) {
                this->unk_1CE += 20000;
                sp46 = (s32)Rand_ZeroFloat(20000.0f) + this->unk_1CE;
                temp_a0 = sp46 - this->actor.world.rot.y;
                if (ABS(temp_a0) < 0x4000) {
                    sp54 = Math_CosS(temp_a0) * 1.6f * this->unk_1DC;
                } else {
                    sp54 = this->unk_1DC;
                }

                sp48.x = (Math_SinS(sp46) * sp54) + this->actor.world.pos.x;
                sp48.y = this->actor.world.pos.y + 20.0f;
                sp48.z = (Math_CosS(sp46) * sp54) + this->actor.world.pos.z;

                func_800B0E48(play, &sp48, &D_80942E48, &D_80942E54, &D_80942E30[ENGOROIWA_C000_2],
                              &D_80942E3C[ENGOROIWA_C000_2],
                              (Rand_ZeroOne() * 600.0f) + (600.0f * (this->actor.scale.x + 0.1f) * 0.5f),
                              (s32)(Rand_ZeroOne() * 50.0f) + 30);
            }
        }
    }
}

void func_80941060(EnGoroiwa* this, PlayState* play) {
    Vec3f spAC;
    Vec3f spA0;
    Vec3f sp94;
    Vec3s* vec = &this->collider.elements[0].base.acDmgInfo.hitPos;
    s32 i;

    for (i = 0; i < 4; i++) {
        sp94.x = ((Rand_ZeroOne() * 14.0f) - 7.0f) + vec->x;
        sp94.y = ((Rand_ZeroOne() * 14.0f) - 7.0f) + vec->y;
        sp94.z = ((Rand_ZeroOne() * 14.0f) - 7.0f) + vec->z;

        spA0.x = (Rand_ZeroOne() - 0.5f) * 1.6f;
        spA0.y = -0.8f;
        spA0.z = (Rand_ZeroOne() - 0.5f) * 1.6f;

        spAC.x = spA0.x * -0.06f;
        spAC.y = spA0.y * -0.06f;
        spAC.z = spA0.z * -0.06f;

        func_800B0E48(play, &sp94, &spA0, &spAC, &D_80942E30[ENGOROIWA_C000_2], &D_80942E3C[ENGOROIWA_C000_2],
                      (s32)(Rand_ZeroOne() * 30.0f) + 15, (s32)(Rand_ZeroOne() * 40.0f) + 30);
    }
}

void func_80941274(EnGoroiwa* this, PlayState* play) {
    SoundSource_PlaySfxAtFixedWorldPos(play, &this->actor.world.pos, 30, NA_SE_EV_SNOWBALL_BROKEN);
}

void EnGoroiwa_Init(Actor* thisx, PlayState* play) {
    s32 pad;
    EnGoroiwa* this = (EnGoroiwa*)thisx;
    f32 temp_f0;
    s32 pathIndex = ENGOROIWA_GET_PATH_INDEX(&this->actor);
    Path* path = &play->setupPathList[pathIndex];

    Actor_ProcessInitChain(&this->actor, sInitChain);
    this->actor.world.rot.x = 0;
    this->actor.world.rot.z = 0;
    this->actor.world.rot.y = Rand_Next() & 0xFFFF;
    this->actor.shape.rot.y = this->actor.world.rot.y;
    this->actor.shape.rot.x = 0;
    this->actor.shape.rot.z = 0;
    func_8093E8A0(this);
    func_8093E91C(this);
    func_8093E9B0(this, play);

    if (pathIndex == ENGOROIWA_PATH_INDEX_NONE) {
        Actor_Kill(&this->actor);
        return;
    }

    if (path->count < 2) {
        Actor_Kill(&this->actor);
        return;
    }

    //! @bug: Impossible to reach, && should be an ||
    // 2S2H [Port] Opting to fix this to address a rolling snowball in snowhead being placed with garbage values
    if ((this->actor.home.rot.y >= (path->count - 1)) || (this->actor.home.rot.y < 0)) {
        this->actor.home.rot.y = 0;
    }

    CollisionCheck_SetInfo(&this->actor.colChkInfo, NULL, &sColChkInfoInit);
    ActorShape_Init(&this->actor.shape, 595.0f, ActorShadow_DrawCircle, 9.4f);
    this->actor.shape.shadowAlpha = 200;
    func_8093EB58(this, play);

    this->pathPoints = Lib_SegmentedToVirtual(path->points);

    func_8093EE18(this, play);
    func_8093EE64(this, this->actor.home.rot.y);
    func_8093EEBC(this);
    func_8093EC50(this);

    if (ENGOROIWA_GET_3000(&this->actor) == ENGOROIWA_3000_2) {
        temp_f0 = func_8093EB74(this, play);

        if (temp_f0 < 0.1f) {
            this->unk_1E0 = 0.0f;
        } else {
            this->unk_1E0 = (D_80942DFC[this->unk_1E4] * ((s32)play->state.framerateDivisor * 0.5f)) / temp_f0;
            this->unk_1E0 *= 0.020000001f;
            if (this->unk_1E0 > 0.00037f) {
                this->unk_1E0 = 0.00037f;
            } else if (this->unk_1E0 < 0.00015f) {
                this->unk_1E0 = 0.00015f;
            }
        }
    }

    Effect_Add(play, &this->unk_248, EFFECT_TIRE_MARK, 0, 0, &D_80942E8C);
    func_809419D0(this);
}

void EnGoroiwa_Destroy(Actor* thisx, PlayState* play) {
    EnGoroiwa* this = (EnGoroiwa*)thisx;

    Collider_DestroyJntSph(play, &this->collider);
    Effect_Destroy(play, this->unk_248);
}

s32 func_8094156C(EnGoroiwa* this, PlayState* play) {
    Actor* actor = &this->actor;
    s32 params = ENGOROIWA_GET_C000(&this->actor);
    EnGoroiwaStruct* ptr;
    s32 i;
    s32 phi_s0_2 = false;
    Vec3f sp80;

    if ((this->collider.base.acFlags & AC_HIT) && ((params == ENGOROIWA_C000_1) || (params == ENGOROIWA_C000_2))) {
        if (this->collider.elements[0].base.acHitElem->atDmgInfo.dmgFlags & 0x4000) {
            s16 sp7E = BINANG_SUB(actor->yawTowardsPlayer, this->actor.world.rot.y);
            f32 temp;
            f32 temp2;
            f32 temp3;

            this->unk_1CC = 50;
            this->unk_1E8[0].unk_1E = BINANG_SUB(this->actor.yawTowardsPlayer, 0x4000);
            this->unk_1E8[0].unk_24 = Rand_ZeroOne() * -600.0f;
            this->unk_1E8[1].unk_1E = BINANG_ADD(this->actor.yawTowardsPlayer, 0x4000);
            this->unk_1E8[1].unk_24 = Rand_ZeroOne() * 600.0f;

            for (i = 0; i < ARRAY_COUNT(this->unk_1E8); i++) {
                ptr = &this->unk_1E8[i];

                ptr->unk_00.x = this->actor.world.pos.x;
                ptr->unk_00.y = this->actor.world.pos.y + this->unk_1DC;
                ptr->unk_00.z = this->actor.world.pos.z;

                temp = Rand_ZeroOne();
                temp2 = Math_SinS(ptr->unk_1E);
                temp3 = Math_SinS(this->actor.world.rot.y);

                ptr->unk_0C = ((1.0f / D_80942DFC[this->unk_1E4]) * (temp3 * 14.0f * this->actor.speed)) +
                              (temp2 * (temp + 5.0f));

                ptr->unk_10 = (Rand_ZeroOne() * 11.0f) + 20.0f;

                temp = Rand_ZeroOne();
                temp2 = Math_CosS(ptr->unk_1E);
                temp3 = Math_CosS(this->actor.world.rot.y);
                ptr->unk_14 = ((1.0f / D_80942DFC[this->unk_1E4]) * ((temp3 * 14.0f) * this->actor.speed)) +
                              (temp2 * (temp + 5.0f));

                ptr->unk_1C = 0;
                ptr->unk_20 = 0;
                ptr->unk_22 = (s32)(Rand_ZeroOne() * 400.0f) + 1100;

                temp3 = Rand_ZeroOne();
                temp2 = Math_CosS(sp7E);
                ptr->unk_26 = (s32)(temp2 * 3000.0f) + (s32)(600.0f * (temp3 - 0.5f));
                ptr->unk_2D = 0;
                ptr->unk_2C = 0;
            }

            func_809421E0(this);
            sp80.x = this->actor.world.pos.x;
            sp80.y = this->actor.world.pos.y + this->unk_1DC;
            sp80.z = this->actor.world.pos.z;
            func_80940588(play, &sp80, D_80942E0C[params], &D_80942E30[params], &D_80942E3C[params],
                          this->actor.scale.x);
            func_80941274(this, play);
            phi_s0_2 = true;
        } else if (((params == ENGOROIWA_C000_1) &&
                    (this->collider.elements[0].base.acHitElem->atDmgInfo.dmgFlags & (0x400 | 0x100))) ||
                   ((params == ENGOROIWA_C000_2) && (this->collider.elements[0].base.acHitElem->atDmgInfo.dmgFlags &
                                                     (0x80000000 | 0x800 | 0x400 | 0x100 | 0x8)))) {
            this->unk_1CC = 50;
            if ((params == ENGOROIWA_C000_2) &&
                (this->collider.elements[0].base.acHitElem->atDmgInfo.dmgFlags & 0x800)) {
                this->unk_1E6 = true;
            }
            func_80940090(this, play);
            func_80941274(this, play);
            func_809425CC(this);
            phi_s0_2 = true;
        } else if ((params == ENGOROIWA_C000_2) && (this->unk_1E7 <= 0)) {
            func_80941060(this, play);
            this->unk_1E7 = 10;
        }
    }

    if (params == ENGOROIWA_C000_2) {
        if (this->unk_1E7 > 0) {
            this->unk_1E7--;
        }

        if (phi_s0_2) {
            Item_DropCollectibleRandom(play, NULL, &this->actor.world.pos, 0x20);
        }
    }
    return phi_s0_2;
}

void func_809419D0(EnGoroiwa* this) {
    this->actionFunc = func_80941A10;
    func_8093EAB0(this, 7);
    this->unk_1C4 = 1.0f;
}

void func_80941A10(EnGoroiwa* this, PlayState* play) {
    static EnGoroiwaUnkFunc D_80942E94[] = {
        func_8093F498,
        func_8093F34C,
    };
    static u16 D_80942E9C[][2] = {
        { NA_SE_EV_BIGBALL_ROLL - SFX_FLAG, NA_SE_EV_BIGBALL_ROLL_SR - SFX_FLAG },
        { NA_SE_EV_BIGBALL_ROLL_2 - SFX_FLAG, NA_SE_EV_BIGBALL_ROLL_SR_2 - SFX_FLAG },
        { NA_SE_EV_BIGBALL_ROLL_3 - SFX_FLAG, NA_SE_EV_BIGBALL_ROLL_SR_3 - SFX_FLAG },
        { NA_SE_EV_BIGBALL_ROLL - SFX_FLAG, NA_SE_EV_BIGBALL_ROLL_SR - SFX_FLAG },
    };
    s32 pad;
    Player* player = GET_PLAYER(play);
    s32 sp44 = ENGOROIWA_GET_400(&this->actor);
    s32 sp40 = ENGOROIWA_GET_3000(&this->actor);
    s16 y;
    s32 pad2;

    if (!func_8094156C(this, play)) {
        if ((this->collider.base.atFlags & AT_HIT) && !(player->stateFlags3 & PLAYER_STATE3_80000)) {
            s32 sp34 = this->actor.home.rot.z & 3;

            if (sp34 == 2) {
                func_80940090(this, play);
                if (sp40 == ENGOROIWA_3000_2) {
                    func_8093E8A0(this);
                    func_8093E91C(this);
                }
                func_80941274(this, play);
                func_8093EDD8(this, play);
                func_8093EE64(this, 0);
                func_8093EEBC(this);
            } else {
                this->unk_1E5 &= ~8;
                y = this->actor.yawTowardsPlayer - this->actor.world.rot.y;
                if ((y > -0x4000) && (y < 0x4000)) {
                    this->unk_1E5 |= 8;
                    if (sp44 || (sp34 != 1)) {
                        func_8093EDB0(this);
                    }
                }
            }

            func_800B8D50(play, &this->actor, 2.0f, this->actor.yawTowardsPlayer, 0.0f, 0);

            if (sp34 == 2) {
                func_80941EB4(this);
            } else if (!sp44) {
                func_80941EB4(this);
            } else {
                func_80941DB4(this);
            }

            Player_PlaySfx(player, NA_SE_PL_BODY_HIT);

            if ((sp34 == 1) || (sp34 == 2)) {
                this->unk_1CC = 50;
            }
        } else {
            if (D_80942E94[sp44](this)) {
                s32 sp30 = ENGOROIWA_GET_300(&this->actor);

                if ((sp30 == ENGOROIWA_300_1) && ((this->unk_1D8 == 0) || (this->unk_1D8 == this->unk_1D4))) {
                    func_80940090(this, play);
                    func_80941274(this, play);
                }

                if (((sp30 == ENGOROIWA_300_1) || (sp30 == ENGOROIWA_300_0)) && (sp40 == ENGOROIWA_3000_2) &&
                    ((this->unk_1D8 == 0) || (this->unk_1D8 == this->unk_1D4))) {
                    func_8093E8A0(this);
                    func_8093E91C(this);
                }

                func_8093FC00(this);

                if ((sp30 == ENGOROIWA_300_3) && ((this->unk_1D6 == 0) || (this->unk_1D6 == this->unk_1D4))) {
                    func_80941EB4(this);
                } else if (!sp44 && (this->unk_1D6 != 0) && (this->unk_1D6 != this->unk_1D4)) {
                    sp30 = func_8093EEDC(this);
                    if (sp30 > 0) {
                        func_80941F54(this);
                    } else if (sp30 < 0) {
                        func_80942084(this);
                    } else {
                        func_809419D0(this);
                    }
                } else {
                    func_809419D0(this);
                }
            }
        }

        if (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND) {
            Actor_PlaySfx(&this->actor, D_80942E9C[this->unk_1E4][this->actor.home.rot.x & 1]);
        }
    }
}

void func_80941DB4(EnGoroiwa* this) {
    this->actionFunc = func_80941E28;
    func_8093EAB0(this, 6);
    this->actor.gravity = -0.86f;
    this->actor.terminalVelocity = -15.0f;
    this->actor.speed *= 0.15f;
    this->actor.velocity.y = 5.0f;
    this->unk_1C4 = 1.0f;
}

void func_80941E28(EnGoroiwa* this, PlayState* play) {
    func_8093F34C(this);
    if ((this->actor.bgCheckFlags & BGCHECKFLAG_GROUND) && (this->actor.velocity.y < 0.0f)) {
        if ((this->unk_1E5 & 8) && ((this->actor.home.rot.z & 3) == 1)) {
            func_8093EDB0(this);
        }
        func_80941EB4(this);
    }
}

void func_80941EB4(EnGoroiwa* this) {
    static s16 D_80942EAC[] = { 20, 6, 20 };

    this->actionFunc = func_80941F10;
    this->actor.speed = 0.0f;
    func_8093EAB0(this, 6);
    this->unk_1C8 = D_80942EAC[this->actor.home.rot.z & 3];
    this->unk_1C4 = 0.0f;
}

void func_80941F10(EnGoroiwa* this, PlayState* play) {
    if (!func_8094156C(this, play)) {
        if (this->unk_1C8 > 0) {
            this->unk_1C8--;
        } else {
            func_809419D0(this);
        }
    }
}

void func_80941F54(EnGoroiwa* this) {
    this->actionFunc = func_80941FA4;
    func_8093EAB0(this, 7);
    this->unk_1C4 = 0.0f;
    this->actor.velocity.y = fabsf(this->actor.speed) * 0.1f;
}

void func_80941FA4(EnGoroiwa* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    if (func_8094156C(this, play) == 0) {
        if ((this->collider.base.atFlags & AT_HIT) && !(player->stateFlags3 & PLAYER_STATE3_80000)) {
            func_800B8D50(play, &this->actor, 2.0f, this->actor.yawTowardsPlayer, 0.0f, 0);
            Player_PlaySfx(player, NA_SE_PL_BODY_HIT);
            if (((this->actor.home.rot.z & 3) == 1) || ((this->actor.home.rot.z & 3) == 2)) {
                this->unk_1CC = 50;
            }
        } else if (func_8093F5EC(this)) {
            func_8093FC00(this);
            func_809419D0(this);
            this->actor.speed = 0.0f;
        }
    }
}

void func_80942084(EnGoroiwa* this) {
    this->actionFunc = func_809420F0;
    func_8093EAB0(this, 7);
    this->unk_1C4 = 0.3f;
    this->unk_1CA = 0;
    this->actor.velocity.y = fabsf(this->actor.speed) * -0.3f;
    this->unk_1E5 |= 0x10;
    this->unk_1E5 &= ~0x20;
}

void func_809420F0(EnGoroiwa* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    if (func_8094156C(this, play) == 0) {
        if ((this->collider.base.atFlags & AT_HIT) && !(player->stateFlags3 & PLAYER_STATE3_80000)) {
            func_800B8D50(play, &this->actor, 2.0f, this->actor.yawTowardsPlayer, 0.0f, 0);
            Player_PlaySfx(player, NA_SE_PL_BODY_HIT);
            if (((this->actor.home.rot.z & 3) == 1) || ((this->actor.home.rot.z & 3) == 2)) {
                this->unk_1CC = 50;
            }
        } else if (func_8093F6F8(this, play)) {
            func_8093FC00(this);
            func_809419D0(this);
            this->unk_1E5 &= ~0x10;
            this->actor.speed = 0.0f;
        }
    }
}

void func_809421E0(EnGoroiwa* this) {
    this->actionFunc = func_8094220C;
    func_8093EAB0(this, 0);
}

void func_8094220C(EnGoroiwa* this, PlayState* play) {
    s32 pad;
    EnGoroiwaStruct* ptr;
    s32 i;
    s32 bgId;
    Vec3f spC4;
    Vec3f spB8;
    f32 phi_f12;
    f32 temp_f20;
    f32 spAC;
    s32 params;
    Vec3f sp9C;
    s16 sp9A;

    for (i = 0; i < ARRAY_COUNT(this->unk_1E8); i++) {
        ptr = &this->unk_1E8[i];

        if (!(ptr->unk_2D & 1)) {
            ptr->unk_10 -= 6.0f;
            if (ptr->unk_10 < -20.0f) {
                ptr->unk_10 = -20.0f;
            }
            ptr->unk_00.x += ptr->unk_0C;
            ptr->unk_00.y += ptr->unk_10;
            ptr->unk_00.z += ptr->unk_14;

            ptr->unk_1C += ptr->unk_22;
            ptr->unk_1E += ptr->unk_24;
            ptr->unk_20 += ptr->unk_26;

            spC4.x = ptr->unk_00.x;
            spC4.y = ptr->unk_00.y + 25.0f;
            spC4.z = ptr->unk_00.z;

            ptr->unk_18 = BgCheck_EntityRaycastFloor5(&play->colCtx, &ptr->unk_28, &bgId, &this->actor, &spC4);

            if (ptr->unk_10 <= 0.0f) {
                Matrix_RotateZYX(ptr->unk_1C, ptr->unk_1E, ptr->unk_20, MTXMODE_NEW);
                Matrix_MultVec3f(&D_80942E6C, &spB8);
                temp_f20 = this->unk_1DC * 0.9f;

                if (spB8.y > 0.0f) {
                    if (Math3D_CosOut(&D_80942E60, &spB8, &spAC)) {
                        phi_f12 = 1.0f;
                    } else {
                        phi_f12 = 1.0f - SQ(spAC);
                    }

                    if (phi_f12 <= 0.0f) {
                        temp_f20 = 0.0f;
                    } else {
                        temp_f20 *= sqrtf(phi_f12);
                    }
                }

                if (((ptr->unk_00.y + (this->unk_1DC - temp_f20)) < ptr->unk_18) ||
                    (ptr->unk_18 < (BGCHECK_Y_MIN + 10))) {
                    params = ENGOROIWA_GET_C000(&this->actor);
                    ptr->unk_2D |= 1;
                    sp9C.x = ptr->unk_00.x;
                    sp9C.y = (ptr->unk_00.y - ptr->unk_10) + 10.0f;
                    sp9C.z = ptr->unk_00.z;
                    func_80940A1C(play, &sp9C, D_80942E0C[params], &D_80942E30[params], &D_80942E3C[params],
                                  this->actor.scale.x);
                }
            }
        }
    }

    if ((this->unk_1E8[0].unk_2D & 1) && (this->unk_1E8[1].unk_2D & 1)) {
        func_809425CC(this);
        return;
    }

    sp9A = this->actor.shape.shadowAlpha;
    Math_StepToS(&sp9A, 0, 40);
    this->actor.shape.shadowAlpha = sp9A;

    for (i = 0; i < ARRAY_COUNT(this->unk_1E8); i++) {
        ptr = &this->unk_1E8[i];

        if (ptr->unk_2D & 1) {
            ptr->unk_2C = 0;
        } else {
            sp9A = ptr->unk_2C;
            Math_StepToS(&sp9A, 160, 24);
            ptr->unk_2C = sp9A;
        }
    }
}

void func_809425CC(EnGoroiwa* this) {
    this->actionFunc = func_80942604;
    func_8093EAB0(this, 0);
    this->unk_1C8 = 100;
}

void func_80942604(EnGoroiwa* this, PlayState* play) {
    s32 pad;
    s16 sp22 = this->actor.shape.shadowAlpha;

    Math_StepToS(&sp22, 0, 40);
    this->actor.shape.shadowAlpha = sp22;
    if (this->unk_1C8 > 0) {
        this->unk_1C8--;
    } else {
        Actor_Kill(&this->actor);
    }
}

void EnGoroiwa_Update(Actor* thisx, PlayState* play) {
    s32 pad;
    EnGoroiwa* this = (EnGoroiwa*)thisx;
    Player* player = GET_PLAYER(play);
    s32 bgId;
    s32 sp5C = false;
    Vec3f sp50;
    f32 sp4C;
    s32 sp48 = true;
    FloorType floorType;
    CollisionPoly* tmp;

    if (!(player->stateFlags1 &
          (PLAYER_STATE1_TALKING | PLAYER_STATE1_DEAD | PLAYER_STATE1_10000000 | PLAYER_STATE1_20000000))) {
        if (this->unk_1CC > 0) {
            this->unk_1CC--;
        }

        if ((ENGOROIWA_GET_3000(&this->actor) == ENGOROIWA_3000_2) && (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND) &&
            (this->actionFunc == func_80941A10) && (this->actor.speed > 2.0f)) {
            Math_StepToF(&this->actor.scale.x, 0.16f,
                         (this->actor.xzDistToPlayer < 400.0f) ? this->unk_1E0 * 1.4f : this->unk_1E0);

            this->actor.scale.y = this->actor.scale.x;
            this->actor.scale.z = this->actor.scale.x;
            func_8093E91C(this);
            sp5C = true;

            if (this->actor.flags & ACTOR_FLAG_INSIDE_CULLING_VOLUME) {
                tmp = this->actor.floorPoly;
                if (tmp != NULL) {
                    floorType = SurfaceType_GetFloorType(&play->colCtx, tmp, this->actor.floorBgId);

                    if ((floorType == FLOOR_TYPE_14) || (floorType == FLOOR_TYPE_15)) {
                        if (!(this->unk_1E5 & 0x40)) {
                            sp50.x = this->actor.world.pos.x;
                            sp50.y = this->actor.floorHeight;
                            sp50.z = this->actor.world.pos.z;
                            sp4C = (((Rand_ZeroOne() * 36.0f) + 250.0f) * this->actor.scale.x) + 10.0f;
                            func_800AE930(&play->colCtx, Effect_GetByIndex(this->unk_248), &sp50, sp4C,
                                          this->actor.world.rot.y, this->actor.floorPoly, this->actor.floorBgId);
                        }
                        sp48 = false;
                    }
                }
            }
        }

        if (sp48) {
            func_800AEF44(Effect_GetByIndex(this->unk_248));
        }

        this->actionFunc(this, play);

        if (this->actor.update != NULL) {
            func_8093EC50(this);

            if (sp5C) {
                func_80940E38(this, play);
            }

            switch (ENGOROIWA_GET_400(&this->actor)) {
                case ENGOROIWA_400_1:
                    Actor_UpdateBgCheckInfo(play, &this->actor, 0.0f, 0.0f, 0.0f,
                                            UPDBGCHECKINFO_FLAG_4 | UPDBGCHECKINFO_FLAG_8 | UPDBGCHECKINFO_FLAG_10);
                    break;

                case ENGOROIWA_400_0:
                    sp50.x = this->actor.world.pos.x;
                    sp50.y = this->actor.world.pos.y + 50.0f;
                    sp50.z = this->actor.world.pos.z;
                    this->actor.floorHeight =
                        BgCheck_EntityRaycastFloor5(&play->colCtx, &this->actor.floorPoly, &bgId, &this->actor, &sp50);
                    if (this->actor.floorHeight > BGCHECK_Y_MIN) {
                        this->actor.floorBgId = bgId;
                        if (this->actor.world.pos.y <= (this->actor.floorHeight + 2.0f)) {
                            this->actor.bgCheckFlags |= BGCHECKFLAG_GROUND;
                        } else {
                            this->actor.bgCheckFlags &= ~BGCHECKFLAG_GROUND;
                        }
                    } else {
                        this->actor.bgCheckFlags &= ~BGCHECKFLAG_GROUND;
                        this->actor.floorBgId = 50;
                    }
                    break;

                default:
                    break;
            }

            func_8093FAA4(this, play);

            if (this->actor.flags & ACTOR_FLAG_INSIDE_CULLING_VOLUME) {
                s32 params = ENGOROIWA_GET_C000(&this->actor);

                func_8093E938(this);

                if ((this->unk_1E5 & 1) && (this->unk_1CC <= 0) &&
                    (!(player->stateFlags3 & PLAYER_STATE3_2000000) || (player->transformation != PLAYER_FORM_GORON) ||
                     ((params != ENGOROIWA_C000_1) && (params != ENGOROIWA_C000_2)))) {
                    CollisionCheck_SetAT(play, &play->colChkCtx, &this->collider.base);
                } else {
                    this->collider.base.atFlags &= ~AT_HIT;
                }

                if (this->unk_1E5 & 2) {
                    CollisionCheck_SetAC(play, &play->colChkCtx, &this->collider.base);
                } else {
                    this->collider.base.acFlags &= ~AC_HIT;
                }

                if ((this->unk_1E5 & 4) && (this->unk_1CC <= 0)) {
                    CollisionCheck_SetOC(play, &play->colChkCtx, &this->collider.base);
                } else {
                    this->collider.base.ocFlags1 &= ~OC1_HIT;
                }
            } else {
                this->collider.base.atFlags &= ~AT_HIT;
                this->collider.base.ocFlags1 &= ~OC1_HIT;
                this->collider.base.acFlags &= ~AC_HIT;
            }
        }
    }
}

void func_80942B1C(EnGoroiwa* this, PlayState* play) {
    s32 pad;
    s32 pad2;
    s32 i;
    EnGoroiwaStruct* ptr;
    s32 params = ENGOROIWA_GET_C000(&this->actor);
    Gfx* phi_fp;
    MtxF sp88;
    Vec3s sp80;

    if (params == ENGOROIWA_C000_1) {
        phi_fp = object_goroiwa_DL_0032E0;
    } else {
        phi_fp = object_goroiwa_DL_0082D0;
    }

    for (i = 0; i < ARRAY_COUNT(this->unk_1E8); i++) {
        ptr = &this->unk_1E8[i];

        if (!(ptr->unk_2D & 1)) {
            sp80.x = ptr->unk_1C;
            sp80.y = ptr->unk_1E;
            sp80.z = ptr->unk_20;

            Matrix_SetTranslateRotateYXZ(ptr->unk_00.x, ptr->unk_00.y, ptr->unk_00.z, &sp80);
            Matrix_Scale(this->actor.scale.x, this->actor.scale.y, this->actor.scale.z, MTXMODE_APPLY);
            Gfx_DrawDListOpa(play, phi_fp);

            if ((ptr->unk_28 != 0) && (ptr->unk_2C > 0)) {
                OPEN_DISPS(play->state.gfxCtx);

                Gfx_SetupDL44_Xlu(play->state.gfxCtx);

                gDPSetCombineLERP(POLY_XLU_DISP++, 0, 0, 0, PRIMITIVE, TEXEL0, 0, PRIMITIVE, 0, 0, 0, 0, COMBINED, 0, 0,
                                  0, COMBINED);
                gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 0, 0, 0, ptr->unk_2C);

                func_800C0094(ptr->unk_28, ptr->unk_00.x, ptr->unk_18, ptr->unk_00.z, &sp88);
                Matrix_Put(&sp88);
                Matrix_Scale(this->actor.scale.x * 7.5f, 1.0f, this->actor.scale.z * 7.5f, MTXMODE_APPLY);

                MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx);
                gSPDisplayList(POLY_XLU_DISP++, gCircleShadowDL);

                CLOSE_DISPS(play->state.gfxCtx);
            }
        }
    }
}

void EnGoroiwa_Draw(Actor* thisx, PlayState* play) {
    static Gfx* D_80942EB4[] = {
        object_goroiwa_DL_005C20,
        object_goroiwa_DL_003B40,
        object_goroiwa_DL_008B90,
    };
    EnGoroiwa* this = (EnGoroiwa*)thisx;
    s32 params = ENGOROIWA_GET_C000(&this->actor);

    // #region 2S2H [Interpolation] Track when a boulder resets back to its "home" position and mark
    // interpolation to be skipped that frame
    bool boulderReset = false;
    s32 params300 = ENGOROIWA_GET_300(&this->actor);
    if (((params300 == ENGOROIWA_300_0) || (params300 == ENGOROIWA_300_1)) &&
        ((this->unk_1D6 == 0) || (this->unk_1D6 == this->unk_1D4))) {
        boulderReset = true;
    }
    // #endregion

    if (this->actionFunc == func_8094220C) {
        func_80942B1C(this, play);
    } else if (this->actionFunc != func_80942604) {
        FrameInterpolation_RecordOpenChild(this, boulderReset ? 1 : 0);
        FrameInterpolation_IgnoreActorMtx();
        Gfx_DrawDListOpa(play, D_80942EB4[params]);
        FrameInterpolation_RecordCloseChild();
    }
}
