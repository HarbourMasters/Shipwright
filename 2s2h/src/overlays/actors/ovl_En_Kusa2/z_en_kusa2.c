/*
 * File: z_en_kusa2.c
 * Overlay: ovl_En_Kusa2
 * Description: Keaton grass
 */

#include "prevent_bss_reordering.h"
#include "z_en_kusa2.h"
#include "objects/gameplay_field_keep/gameplay_field_keep.h"
#include "objects/gameplay_keep/gameplay_keep.h"

#include "2s2h/Enhancements/FrameInterpolation/FrameInterpolation.h"
#include "2s2h/ShipUtils.h"
#include "GameInteractor/GameInteractor.h"
#include <string.h>

#define FLAGS (ACTOR_FLAG_UPDATE_CULLING_DISABLED | ACTOR_FLAG_THROW_ONLY)

void EnKusa2_Init(Actor* thisx, PlayState* play);
void EnKusa2_Destroy(Actor* thisx, PlayState* play);
void EnKusa2_Update(Actor* thisx, PlayState* play);
void EnKusa2_Draw(Actor* thisx, PlayState* play);
void EnKusa2_Reset(void);

void func_80A5C70C(EnKusa2UnkBssSubStruct* arg0);
void func_80A5CF44(EnKusa2* this);
void func_80A5D5E0(EnKusa2* this);
void func_80A5D5F4(EnKusa2* this, PlayState* play);
void func_80A5D618(EnKusa2* this);
void func_80A5D62C(EnKusa2* this, PlayState* play);
void func_80A5D6B0(EnKusa2* this);
void func_80A5D6C4(EnKusa2* this, PlayState* play);
void func_80A5D754(EnKusa2* this);
void func_80A5D794(EnKusa2* this, PlayState* play);
void func_80A5D7A4(EnKusa2* this);
void func_80A5D7C4(EnKusa2* this, PlayState* play);
void func_80A5D964(EnKusa2* this);
void func_80A5D9C8(EnKusa2* this, PlayState* play);
void func_80A5DC70(EnKusa2* this);
void func_80A5DC98(EnKusa2* this, PlayState* play);
void func_80A5DE18(EnKusa2* this);
void func_80A5DEB4(EnKusa2* this, PlayState* play);
void func_80A5E1D8(EnKusa2* this);
void func_80A5E210(EnKusa2* this, PlayState* play);
void func_80A5E418(EnKusa2* this);
void func_80A5E4BC(EnKusa2* this, PlayState* play);
void func_80A5E604(Actor* thisx, PlayState* play);
void func_80A5E6F0(Actor* thisx, PlayState* play);
void func_80A5E9B4(Actor* thisx, PlayState* play);
void func_80A5EA48(Actor* thisx, PlayState* play);

EnKusa2UnkBssStruct D_80A5F1C0;
u32 D_80A60900;
MtxF D_80A60908[8];
s16 D_80A60B08;
s16 D_80A60B0A;
s16 D_80A60B0C;
s16 D_80A60B0E;
s16 D_80A60B10;

ActorProfile En_Kusa2_Profile = {
    /**/ ACTOR_EN_KUSA2,
    /**/ ACTORCAT_PROP,
    /**/ FLAGS,
    /**/ GAMEPLAY_FIELD_KEEP,
    /**/ sizeof(EnKusa2),
    /**/ EnKusa2_Init,
    /**/ EnKusa2_Destroy,
    /**/ EnKusa2_Update,
    /**/ EnKusa2_Draw,
    /**/ EnKusa2_Reset,
};

static ColliderCylinderInit sCylinderInit = {
    {
        COL_MATERIAL_NONE,
        AT_NONE,
        AC_ON | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_PLAYER | OC1_TYPE_2,
        OC2_TYPE_2,
        COLSHAPE_CYLINDER,
    },
    {
        ELEM_MATERIAL_UNK0,
        { 0x00000000, 0x00, 0x00 },
        { 0x0580C71C, 0x00, 0x00 },
        ATELEM_NONE | ATELEM_SFX_NORMAL,
        ACELEM_ON,
        OCELEM_ON,
    },
    { 6, 44, 0, { 0, 0, 0 } },
};

u8 D_80A5EAEC = true;
s16 D_80A5EAF0 = 0;
Vec3s D_80A5EAF4 = { 0, 0, 0 };
Vec3s D_80A5EAFC = { 0, 0, 0 };
Vec3s D_80A5EB04 = { 0, 0, 0 };

void func_80A5B160(EnKusa2* this, PlayState* play) {
    s32 i;
    s16 temp_s1;
    EnKusa2** ptr;
    EnKusa2* actor;

    if (this->unk_194[0] == NULL) {
        ptr = this->unk_194;
        actor = (EnKusa2*)Actor_SpawnAsChildAndCutscene(
            &play->actorCtx, play, ACTOR_EN_KUSA2, this->actor.world.pos.x, this->actor.world.pos.y,
            this->actor.world.pos.z, 0, Rand_Next() >> 0x10, 0, 1, this->actor.csId, this->actor.halfDaysBits, NULL);
        *ptr = actor;

        if (*ptr != NULL) {
            (*ptr)->actor.room = this->actor.room;
            (*ptr)->unk_1C0 = this;
            GameInteractor_Should(VB_KEATON_GRASS_ATTACH_CHILD, true, *ptr, 0);
        }
    }

    for (i = 1; i < ARRAY_COUNT(this->unk_194); i++) {
        temp_s1 = (i * 0x2000) - 0x2000;
        if (this->unk_194[i] == NULL) {
            ptr = &this->unk_194[i];
            actor = (EnKusa2*)Actor_SpawnAsChildAndCutscene(
                &play->actorCtx, play, ACTOR_EN_KUSA2, (Math_SinS(temp_s1) * 80.0f) + this->actor.world.pos.x,
                this->actor.world.pos.y, (Math_CosS(temp_s1) * 80.0f) + this->actor.world.pos.z, 0, Rand_Next() >> 0x10,
                0, 1, this->actor.csId, this->actor.halfDaysBits, NULL);
            *ptr = actor;
            if (*ptr != NULL) {
                (*ptr)->actor.room = this->actor.room;
                (*ptr)->unk_1C0 = this;
                GameInteractor_Should(VB_KEATON_GRASS_ATTACH_CHILD, true, *ptr, i);
            }
        }
    }
}

void func_80A5B334(EnKusa2* this, PlayState* play) {
    s32 i;
    EnKusa2** ptr;

    for (i = 0; i < ARRAY_COUNT(this->unk_194); i++) {
        ptr = &this->unk_194[i];

        if (*ptr != NULL) {
            if (!Actor_HasParent(&(*ptr)->actor, play)) {
                Actor_Kill(&(*ptr)->actor);
            }
            *ptr = NULL;
        }
    }
}

void func_80A5B3BC(EnKusa2* this) {
    s32 i;
    EnKusa2** ptr;

    for (i = 0; i < ARRAY_COUNT(this->unk_194); i++) {
        ptr = &this->unk_194[i];
        if ((*ptr != NULL) && ((*ptr)->actor.update == NULL)) {
            *ptr = NULL;
        }
    }
}

void func_80A5B490(EnKusa2* this, PlayState* play) {
    Actor_SpawnAsChildAndCutscene(&play->actorCtx, play, ACTOR_EN_KITAN, this->actor.world.pos.x,
                                  this->actor.world.pos.y, this->actor.world.pos.z, 0, 0, 0,
                                  ENKUSA2_GET_7F00(&this->actor) << 9, this->actor.csId, this->actor.halfDaysBits,
                                  NULL);
}

void func_80A5B508(void) {
    s32 i;
    s32 pad;
    f32 spB4;
    f32* ptr;
    f32 spAC;
    f32 spA8;
    f32 tempf1;
    f32 tempf2;
    f32 tempf3;
    f32 tempf4;
    f32 tempf5;
    f32 sp74[8];
    f32 temp_f20;
    f32 temp_f22;
    f32 temp_f24;
    f32 temp_f26;
    f32 temp_f28;
    f32 temp_f2;
    f32 temp_f30;

    D_80A60B08 += 70;
    D_80A60B0A += 300;
    D_80A60B0C += 700;
    D_80A60B0E += 1300;
    D_80A60B10 += 8900;
    temp_f28 = Math_SinS(D_80A60B08);
    spB4 = Math_SinS(D_80A60B0A);
    temp_f30 = Math_SinS(D_80A60B0C);
    spAC = Math_SinS(D_80A60B0E) * 1.2f;
    spA8 = Math_SinS(D_80A60B10) * 1.5f;
    temp_f26 = Math_CosS(D_80A60B08);
    temp_f20 = Math_CosS(D_80A60B0A);
    temp_f22 = Math_CosS(D_80A60B0C);
    temp_f24 = Math_CosS(D_80A60B0E) * 1.3f;
    temp_f2 = Math_CosS(D_80A60B10) * 1.7f;

    sp74[0] = (temp_f28 - temp_f20) * temp_f30 * temp_f26 * temp_f28;
    sp74[1] = (spB4 - temp_f22) * spAC * temp_f20 * temp_f28;
    sp74[2] = (temp_f30 - temp_f24) * temp_f22 * temp_f28 * temp_f26;
    sp74[3] = (spAC - temp_f20) * temp_f24 * spB4 * temp_f26;
    sp74[4] = (temp_f28 - temp_f22) * temp_f28 * spB4 * spA8;
    sp74[5] = (spB4 - temp_f24) * temp_f30 * spAC * spA8;
    sp74[6] = (temp_f30 - temp_f26) * temp_f26 * temp_f20 * temp_f2;
    sp74[7] = (spAC - temp_f20) * temp_f22 * temp_f24 * temp_f2;

    for (i = 0; i < ARRAY_COUNT(D_80A60908); i++) {
        ptr = &D_80A60908[i].xx;

        tempf1 = sp74[(i + 0) & 7];
        tempf2 = sp74[(i + 1) & 7];
        tempf3 = sp74[(i + 2) & 7];
        tempf4 = sp74[(i + 3) & 7];
        tempf5 = sp74[(i + 4) & 7];

        ptr[0] = sp74[1] * 0.2f;
        ptr[1] = tempf1;
        ptr[2] = tempf2;
        ptr[3] = 0.0f;

        ptr[4] = tempf3;
        ptr[5] = sp74[0];
        ptr[6] = tempf3;
        ptr[7] = 0.0f;

        ptr[8] = tempf4;
        ptr[9] = tempf5;
        ptr[10] = sp74[3] * 0.2f;
        ptr[11] = 0.0f;

        ptr[12] = 0.0f;
        ptr[13] = 0.0f;
        ptr[14] = 0.0f;
        ptr[15] = 0.0f;
    }
}

void func_80A5B954(MtxF* matrix, f32 arg1) {
    s32 i;
    MtxF* temp = Matrix_GetCurrent();
    f32* tmp = &temp->xx;
    f32* tmp2 = &matrix->xx;

    for (i = 0; i < 16; i++) {
        *tmp++ += *tmp2++ * arg1;
    }
}

s32 func_80A5BA58(EnKusa2* this, PlayState* play) {
    Vec3f sp24;
    s32 bgId;

    sp24.x = this->actor.world.pos.x;
    sp24.y = this->actor.world.pos.y + 30.0f;
    sp24.z = this->actor.world.pos.z;
    this->actor.floorHeight =
        BgCheck_EntityRaycastFloor5(&play->colCtx, &this->actor.floorPoly, &bgId, &this->actor, &sp24);
    if (this->actor.floorHeight > BGCHECK_Y_MIN) {
        this->actor.floorBgId = bgId;
        this->actor.world.pos.y = this->actor.floorHeight;
        Math_Vec3f_Copy(&this->actor.home.pos, &this->actor.world.pos);
        return true;
    }
    return false;
}

void func_80A5BAFC(EnKusa2* this, PlayState* play) {
    Actor_UpdateBgCheckInfo(play, &this->actor, 15.0f, 35.0f, 0.0f,
                            UPDBGCHECKINFO_FLAG_1 | UPDBGCHECKINFO_FLAG_4 | UPDBGCHECKINFO_FLAG_40);
}

void func_80A5BB40(EnKusa2* this, PlayState* play, s32 arg2) {
    static Vec3f D_80A5EB0C = { 0.0f, 0.3f, 0.0f };
    static Vec3f D_80A5EB18 = { 0.0f, 0.0f, 0.0f };
    s32 pad;
    s32 i;
    Vec3f sp84;
    f32 temp_f20;
    f32 temp_f22;
    f32 temp_f24;
    s16 temp_s0;

    if (this->actor.flags & ACTOR_FLAG_INSIDE_CULLING_VOLUME) {
        for (i = 0; i <= arg2; i++) {
            temp_s0 = Rand_S16Offset(-16000, 32000) + this->actor.world.rot.y;
            temp_f20 = Math_SinS(temp_s0);
            temp_f22 = Math_CosS(temp_s0);
            temp_f24 = Rand_ZeroOne() * -12.0f;

            sp84.x = (temp_f20 * temp_f24) + this->actor.world.pos.x;
            sp84.y = (Rand_ZeroOne() * 8.0f) + 2.0f + this->actor.world.pos.y;
            sp84.z = (temp_f22 * temp_f24) + this->actor.world.pos.z;

            D_80A5EB18.x = temp_f20 * -0.9f;
            D_80A5EB18.z = temp_f22 * -0.9f;

            D_80A5EB0C.x = D_80A5EB18.x * -0.09f;
            D_80A5EB0C.z = D_80A5EB18.z * -0.09f;

            func_800B1210(play, &sp84, &D_80A5EB18, &D_80A5EB0C, 10, 50);
        }
    }
}

void func_80A5BD14(EnKusa2* this, PlayState* play, s32 arg2) {
    static s32 D_80A5EB24[] = {
        ITEM00_RUPEE_GREEN, ITEM00_RUPEE_GREEN, ITEM00_RUPEE_GREEN, ITEM00_RUPEE_GREEN, ITEM00_RUPEE_GREEN,
        ITEM00_RUPEE_GREEN, ITEM00_RUPEE_GREEN, ITEM00_RUPEE_GREEN, ITEM00_RUPEE_RED,
    };
    EnKusa2* kusa2 = this->unk_1C0;

    if (kusa2 != NULL) {
        if (kusa2->unk_1BC > 8) {
            kusa2->unk_1BC = 8;
        }
        if (GameInteractor_Should(VB_GRASS_DROP_COLLECTIBLE, true, ACTOR_EN_KUSA2, this)) {
            Item_DropCollectible(play, &this->actor.world.pos, D_80A5EB24[kusa2->unk_1BC]);
        }
        kusa2->unk_1BC += arg2;
    }
}

void func_80A5BD94(EnKusa2* this) {
    if (this->unk_1C0 != NULL) {
        this->unk_1C0->unk_1BE = 1;
    }
}

void func_80A5BDB0(EnKusa2* this, PlayState* play) {
    s32 pad[2];
    s32 i;
    Vec3f sp50;
    s16 phi_s0;

    sp50.y = this->actor.world.pos.y + this->actor.depthInWater;

    for (phi_s0 = 0, i = 0; i < 4; i++, phi_s0 += 0x4000) {
        sp50.x = this->actor.world.pos.x + (Math_SinS((s32)(Rand_ZeroOne() * 7200.0f) + phi_s0) * 15.0f);
        sp50.z = this->actor.world.pos.z + (Math_CosS((s32)(Rand_ZeroOne() * 7200.0f) + phi_s0) * 15.0f);
        EffectSsGSplash_Spawn(play, &sp50, NULL, NULL, 0, 190);
    }

    sp50.x = this->actor.world.pos.x;
    sp50.z = this->actor.world.pos.z;
    EffectSsGSplash_Spawn(play, &sp50, NULL, NULL, 0, 280);
    EffectSsGRipple_Spawn(play, &sp50, 300, 700, 0);
    SoundSource_PlaySfxAtFixedWorldPos(play, &this->actor.world.pos, 40, NA_SE_EV_DIVE_INTO_WATER_L);
}

void func_80A5BF38(EnKusa2* this, s32 arg1) {
    this->actor.shape.shadowAlpha += (u8)arg1;
    if (this->actor.shape.shadowAlpha > 60) {
        this->actor.shape.shadowAlpha = 60;
    }
}

void func_80A5BF60(EnKusa2* this, s32 arg1) {
    s32 alpha = this->actor.shape.shadowAlpha;

    alpha -= arg1;
    if (alpha > 0) {
        this->actor.shape.shadowAlpha = alpha;
    } else {
        this->actor.shape.shadowAlpha = 0;
    }
}

void func_80A5BF84(EnKusa2* this, PlayState* play) {
    Collider_UpdateCylinder(&this->actor, &this->collider);
    CollisionCheck_SetAC(play, &play->colChkCtx, &this->collider.base);
    CollisionCheck_SetOC(play, &play->colChkCtx, &this->collider.base);
}

s32 func_80A5BFD8(EnKusa2* this, PlayState* play) {
    if (this->collider.base.acFlags & AC_HIT) {
        s32 pad;

        func_80A5CF44(this);
        func_80A5BD14(this, play, (this->collider.elem.acHitElem->atDmgInfo.dmgFlags & 0x1000000) ? 1 : 0);
        SoundSource_PlaySfxAtFixedWorldPos(play, &this->actor.world.pos, 20, NA_SE_EV_PLANT_BROKEN);
        func_80A5BD94(this);
        Actor_Kill(&this->actor);
        return true;
    }
    return false;
}

EnKusa2UnkBssSubStruct* func_80A5C074(EnKusa2UnkBssStruct* arg0) {
    s32 i;
    EnKusa2UnkBssSubStruct* phi_v1 = &arg0->unk_0000[0];

    for (i = 1; i < ARRAY_COUNT(D_80A5F1C0.unk_0000); i++) {
        if (phi_v1->unk_26 > arg0->unk_0000[i].unk_26) {
            phi_v1 = &arg0->unk_0000[i];
            if (phi_v1->unk_26 <= 0) {
                break;
            }
        }
    }

    return phi_v1;
}

EnKusa2UnkBssSubStruct2* func_80A5C0B8(EnKusa2UnkBssStruct* arg0) {
    s32 i;
    EnKusa2UnkBssSubStruct2* phi_v1 = &arg0->unk_0480[0];

    for (i = 1; i < ARRAY_COUNT(D_80A5F1C0.unk_0480); i++) {
        EnKusa2UnkBssSubStruct2* iter = &arg0->unk_0480[i];

        if (iter->unk_2C < phi_v1->unk_2C) {
            phi_v1 = iter;

            if (phi_v1->unk_2C <= 0) {
                break;
            }
        }
    }

    return phi_v1;
}

EnKusa2UnkBssSubStruct* func_80A5C104(EnKusa2UnkBssStruct* arg0, Vec3f* arg1, f32 arg2, f32 arg3, s16 arg4, s16 arg5) {
    static s32 D_80A5EB48 = 0;
    EnKusa2UnkBssSubStruct* ptr = func_80A5C074(arg0);
    f32 sp20;
    f32 temp_f6;

    Math_Vec3f_Copy(&ptr->unk_00, arg1);
    Math_Vec3f_Copy(&ptr->unk_0C, &gZeroVec3f);

    ptr->unk_18 = arg2;
    ptr->unk_1C = arg3;
    ptr->unk_20 = arg4;
    ptr->unk_22 = arg5;

    D_80A5EB48++;
    if (D_80A5EB48 >= 3) {
        D_80A5EB48 = 0;
        ptr->unk_26 = Rand_S16Offset(40, 40);
    } else {
        ptr->unk_26 = Rand_S16Offset(30, 20);
    }

    ptr->unk_24 = ptr->unk_26 - 20;
    sp20 = 1.0f / ptr->unk_24;
    ptr->unk_28 = func_80A5C70C;
    ptr->unk_2C = 0.0f;
    ptr->unk_30 = 0.0f;
    temp_f6 = Rand_ZeroOne() * 60.0f;
    ptr->unk_34 = (temp_f6 + 20.0f) * 0.05f;
    ptr->unk_38 = (temp_f6 + 20.0f) * sp20;
    ptr->unk_3C = 40.0f;
    ptr->unk_40 = -1.5f;
    ptr->unk_44 = -50.0f * sp20;
    ptr->unk_48 = 140.0f;
    ptr->unk_4C = SQ(140.0f);
    ptr->unk_50 = 819.2f;
    ptr->unk_54 = 327.68f;
    ptr->unk_58 = 2.5f;
    ptr->unk_5C = 0.05f;
    ptr->unk_60 = 3.5f * sp20;
    ptr->unk_64 = 0.35f;
    ptr->unk_68 = 0.0875f;
    ptr->unk_6C = 160.0f;
    ptr->unk_70 = SQ(160.0f);
    ptr->unk_74 = 204.8f;
    ptr->unk_78 = 100.0f;
    ptr->unk_7C = 327.68f;
    return ptr;
}

EnKusa2UnkBssSubStruct2* func_80A5C2FC(EnKusa2UnkBssStruct* arg0, f32 arg1, Vec3f* arg2, Vec3f* arg3, Vec3s* arg4,
                                       Vec3s* arg5) {
    Vec3f* temp_a0;
    EnKusa2UnkBssSubStruct2* temp_s0;
    f32 phi_f8;

    temp_s0 = func_80A5C0B8(arg0);
    temp_s0->unk_00 = arg1;
    Math_Vec3f_Copy(&temp_s0->unk_04, arg2);
    Math_Vec3f_Copy(&temp_s0->unk_10, arg3);

    if (arg1 >= 0.35f) {
        temp_s0->unk_1C = (6.0f / 7);
    } else if (arg1 <= 0.1f) {
        temp_s0->unk_1C = 1.0f;
    } else {
        temp_s0->unk_1C = (0.02f / arg1) + 0.8f;
    }

    temp_s0->unk_20 = *arg4;
    temp_s0->unk_26 = *arg5;
    temp_s0->unk_2C = Rand_S16Offset(65, 30);

    return temp_s0;
}

void func_80A5C410(EnKusa2UnkBssStruct* arg0, EnKusa2UnkBssSubStruct2* arg1, Vec3f* arg2) {
    s32 i;
    Vec3f sp98;
    EnKusa2UnkBssSubStruct* s;
    s32 pad;

    Math_Vec3f_Copy(arg2, &gZeroVec3f);

    for (i = 0; i < ARRAY_COUNT(D_80A5F1C0.unk_0000); i++) {
        s = &arg0->unk_0000[i];

        if (s->unk_26 > 0) {
            f32 temp_f0;
            s32 phi_v0 = true;
            s32 phi_s2 = true;

            Math_Vec3f_Diff(&arg1->unk_04, &s->unk_00, &sp98);
            temp_f0 = Math3D_Vec3fMagnitudeSq(&sp98);

            phi_v0 = false;
            if (temp_f0 <= s->unk_4C) {
                phi_v0 = true;
            }

            phi_s2 = false;
            if (temp_f0 <= s->unk_70) {
                phi_s2 = true;
            }

            if (phi_v0 || phi_s2) {
                f32 phi_f22;

                if (temp_f0 < SQ(0.01f)) {
                    phi_f22 = 0.0f;
                } else {
                    phi_f22 = sqrtf(temp_f0);
                }

                if (phi_v0) {
                    f32 phi_f2;
                    f32 phi_f20;

                    if (phi_f22 > 1.0f) {
                        phi_f20 = 1.0f / phi_f22;
                    } else {
                        phi_f20 = 1.0f;
                    }

                    if (phi_f22 < s->unk_3C) {
                        phi_f2 = Math_SinS(s->unk_50 * phi_f22) * -s->unk_2C;
                    } else {
                        phi_f2 = (Math_CosS(s->unk_54 * (phi_f22 - s->unk_3C)) + 1.0f) * s->unk_30;
                    }

                    arg2->x += sp98.x * -(phi_f20 * phi_f2);
                    arg2->z += sp98.z * -(phi_f20 * phi_f2);

                    if (phi_s2) {
                        arg2->x += phi_f20 * sp98.z * s->unk_58;
                        arg2->z -= phi_f20 * sp98.x * s->unk_58;
                    }
                }

                if (phi_s2 && (fabsf(sp98.y) < s->unk_78)) {
                    arg2->y +=
                        s->unk_68 * (Math_CosS(s->unk_74 * phi_f22) + 1.0f) * (Math_CosS(s->unk_7C * sp98.y) + 1.0f);
                }
            }
        }
    }

    arg2->y -= 0.5f;
}

void func_80A5C70C(EnKusa2UnkBssSubStruct* arg0) {
}

void func_80A5C718(EnKusa2UnkBssSubStruct* arg0) {
    Math_ScaledStepToS(&arg0->unk_20, arg0->unk_22, 0x4B0);

    if ((arg0->unk_1C > 0.0f) && (Rand_ZeroOne() < 0.05f)) {
        arg0->unk_1C = -arg0->unk_1C;
    }

    arg0->unk_0C.x = Math_SinS(arg0->unk_22) * arg0->unk_18;
    arg0->unk_0C.y += arg0->unk_1C;
    arg0->unk_0C.z = Math_CosS(arg0->unk_22) * arg0->unk_18;

    arg0->unk_00.x += arg0->unk_0C.x;
    arg0->unk_00.y += arg0->unk_0C.y;
    arg0->unk_00.z += arg0->unk_0C.z;
}

void func_80A5C7F0(EnKusa2UnkBssStruct* arg0, EnKusa2UnkBssSubStruct* arg1) {
    if (arg1->unk_26 > 0) {
        arg1->unk_26--;
        arg1->unk_28(arg1);
        if (arg1->unk_26 >= arg1->unk_24) {
            arg1->unk_2C += arg1->unk_34;
            arg1->unk_3C += arg1->unk_40;
            arg1->unk_58 += arg1->unk_5C;
        } else {
            arg1->unk_2C -= arg1->unk_38;
            if (arg1->unk_2C < 0.0f) {
                arg1->unk_2C = 0.0f;
            }
            arg1->unk_3C -= arg1->unk_44;
            arg1->unk_58 -= arg1->unk_60;
            if (arg1->unk_58 < 0.0f) {
                arg1->unk_58 = 0.0f;
            }
        }
        arg1->unk_30 = arg1->unk_2C * 0.5f;
        arg1->unk_50 = 0x8000 / arg1->unk_3C;
        arg1->unk_54 = 0x8000 / (arg1->unk_48 - arg1->unk_3C);
    }
}

void func_80A5C918(EnKusa2UnkBssStruct* arg0, EnKusa2UnkBssSubStruct2* arg1) {
    Vec3f sp4C;
    s32 pad;
    s32 pad2;
    Vec3f sp38;
    Vec3f sp2C;
    f32 phi_f0;

    if (arg1->unk_2C > 0) {
        arg1->unk_2C--;

        func_80A5C410(arg0, arg1, &sp4C);

        arg1->unk_10.x += sp4C.x;
        arg1->unk_10.y += sp4C.y;
        arg1->unk_10.z += sp4C.z;

        sp38.x = arg1->unk_10.x * (arg1->unk_1C * 0.06f);
        sp38.x = arg1->unk_10.x * (arg1->unk_1C * 0.06f);
        sp38.y = arg1->unk_10.y * (arg1->unk_1C * 0.06f);
        sp38.z = arg1->unk_10.z * (arg1->unk_1C * 0.06f);

        sp2C.x = SQ(arg1->unk_10.x) * (arg1->unk_1C * 0.004f);
        sp2C.y = SQ(arg1->unk_10.y) * (arg1->unk_1C * 0.004f);
        sp2C.z = SQ(arg1->unk_10.z) * (arg1->unk_1C * 0.004f);

        if (arg1->unk_10.x > 0.0f) {
            phi_f0 = sp2C.x;
        } else {
            phi_f0 = -sp2C.x;
        }
        arg1->unk_10.x -= sp38.x + phi_f0;

        if (arg1->unk_10.y > 0.0f) {
            phi_f0 = sp2C.y;
        } else {
            phi_f0 = -sp2C.y;
        }
        arg1->unk_10.y -= sp38.y + phi_f0;

        if (arg1->unk_10.z > 0.0f) {
            phi_f0 = sp2C.z;
        } else {
            phi_f0 = -sp2C.z;
        }
        arg1->unk_10.z -= sp38.z + phi_f0;

        arg1->unk_04.x += arg1->unk_10.x;
        arg1->unk_04.y += arg1->unk_10.y;
        arg1->unk_04.z += arg1->unk_10.z;
        arg1->unk_20.x += arg1->unk_26.x;
        arg1->unk_20.y += arg1->unk_26.y;
        arg1->unk_20.z += arg1->unk_26.z;
    }
}

void func_80A5CAD4(EnKusa2UnkBssStruct* arg0) {
    memset(arg0, 0, sizeof(EnKusa2UnkBssStruct));
}

void func_80A5CAF4(EnKusa2UnkBssStruct* arg0) {
    s32 i;

    for (i = 0; i < ARRAY_COUNT(D_80A5F1C0.unk_0000); i++) {
        func_80A5C7F0(arg0, &arg0->unk_0000[i]);
    }

    for (i = 0; i < ARRAY_COUNT(D_80A5F1C0.unk_0480); i++) {
        func_80A5C918(arg0, &arg0->unk_0480[i]);
    }
}

void func_80A5CB74(EnKusa2* this) {
    static s8 D_80A5EB4C = 0;
    s32 pad;
    Vec3f sp40;
    s16 sp3E;
    s16 sp3C;
    f32 sp38;
    s16 sp36;
    f32 temp_f0;

    sp40.x = this->actor.world.pos.x;
    sp40.y = this->actor.world.pos.y + 20.0f;
    sp40.z = this->actor.world.pos.z;
    sp3E = Rand_S16Offset(-10000, 20000) + this->actor.world.rot.y;

    if (this->unk_1C0 != NULL) {
        sp36 = Actor_WorldYawTowardActor(&this->actor, &this->unk_1C0->actor);
        sp3C = Rand_S16Offset(-4000, 8000) + sp36;
    } else {
        sp3C = 0;
    }

    D_80A5EB4C++;
    D_80A5EB4C &= 0x7;

    temp_f0 = Rand_ZeroOne();
    if (D_80A5EB4C == 0) {
        sp38 = temp_f0 * 0.70000005f;
    } else {
        sp38 = SQ(temp_f0) * 0.4f;
    }
    this->unk_1B8 = func_80A5C104(&D_80A5F1C0, &sp40, (Rand_ZeroOne() * 3.5f) + 4.0f, sp38, sp3E, sp3C);
}

void func_80A5CCD4(EnKusa2* this) {
    if (this->unk_1B8 != NULL) {
        EnKusa2UnkBssSubStruct* ptr = this->unk_1B8;

        ptr->unk_00.x = this->actor.world.pos.x;
        ptr->unk_00.y = this->actor.world.pos.y + 20.0f;
        ptr->unk_00.z = this->actor.world.pos.z;
    }
}

void func_80A5CD0C(EnKusa2* this) {
    s32 i;
    Vec3f spA8;
    Vec3f sp9C;
    Vec3s sp94;
    Vec3s sp8C;
    f32 temp_f20;
    s16 temp_s0;
    s32 pad;

    for (i = 0; i < 2; i++) {
        temp_s0 = Rand_Next() & 0xFFFF;
        temp_f20 = Rand_ZeroOne() * 30.0f;

        spA8.x = Math_SinS(temp_s0) * temp_f20;
        spA8.y = Rand_ZeroOne() * 30.0f;
        spA8.z = Math_CosS(temp_s0) * temp_f20;

        sp9C.x = ((Rand_ZeroOne() * 4.0f) - 2.0f) + (spA8.x * 0.18f);
        sp9C.y = (Rand_ZeroOne() * 8.0f) + 3.0f;
        sp9C.z = ((Rand_ZeroOne() * 4.0f) - 2.0f) + (spA8.z * 0.18f);

        spA8.x += this->actor.world.pos.x;
        spA8.y += this->actor.world.pos.y;
        spA8.z += this->actor.world.pos.z;

        sp94.x = Rand_Next() & 0xFFFF;
        sp94.y = Rand_Next() & 0xFFFF;
        sp94.z = Rand_Next() & 0xFFFF;

        sp8C.x = (Rand_Next() % 0x4000) - 0x1FFF;
        sp8C.y = (Rand_Next() % 0x2000) - 0xFFF;
        sp8C.z = (Rand_Next() % 0x4000) - 0x1FFF;

        func_80A5C2FC(&D_80A5F1C0, (Rand_ZeroOne() * 0.4f) + 0.02f, &spA8, &sp9C, &sp94, &sp8C);
    }
}

void func_80A5CF44(EnKusa2* this) {
    s32 i;
    Vec3f spA8;
    Vec3f sp9C;
    Vec3s sp94;
    Vec3s sp8C;
    f32 temp_f20;
    s16 temp_s0;
    s32 phi_s2;

    for (i = 0, phi_s2 = 0; i < 8; i++, phi_s2 += 0x2000) {
        temp_s0 = (Rand_Next() % 0x2000) + phi_s2;
        temp_f20 = Rand_ZeroOne() * 30.0f;

        spA8.x = Math_SinS(temp_s0) * temp_f20;
        spA8.y = Rand_ZeroOne() * 30.0f;
        spA8.z = Math_CosS(temp_s0) * temp_f20;

        sp9C.x = ((Rand_ZeroOne() * 6.0f) - 3.0f) + (spA8.x * 0.19f);
        sp9C.y = (Rand_ZeroOne() * 9.0f) + 3.0f;
        sp9C.z = ((Rand_ZeroOne() * 6.0f) - 3.0f) + (spA8.z * 0.19f);

        spA8.x += this->actor.world.pos.x;
        spA8.y += this->actor.world.pos.y;
        spA8.z += this->actor.world.pos.z;

        sp94.x = Rand_Next() & 0xFFFF;
        sp94.y = Rand_Next() & 0xFFFF;
        sp94.z = Rand_Next() & 0xFFFF;

        sp8C.x = (Rand_Next() % 0x4000) - 0x1FFF;
        sp8C.y = (Rand_Next() % 0x2000) - 0xFFF;
        sp8C.z = (Rand_Next() % 0x4000) - 0x1FFF;

        func_80A5C2FC(&D_80A5F1C0, (Rand_ZeroOne() * 0.45f) + 0.04f, &spA8, &sp9C, &sp94, &sp8C);
    }
}

void func_80A5D178(EnKusa2* this) {
    s32 i;
    Vec3f spB0;
    Vec3f spA4;
    Vec3s sp9C;
    Vec3s sp94;
    f32 temp_f20;
    f32 temp_f22;
    f32 temp_f24;
    s16 temp_s0;
    s32 pad;

    for (i = 0; i < 8; i++) {
        temp_s0 = Rand_Next();
        temp_f22 = Math_SinS(temp_s0);
        temp_f20 = Math_CosS(temp_s0);
        temp_f24 = (Rand_ZeroOne() * 40.0f) + 10.0f;

        spB0.x = temp_f24 * temp_f22;
        spB0.y = Rand_ZeroOne() * 30.0f;
        spB0.z = temp_f24 * temp_f20;

        spA4.x = ((Rand_ZeroOne() * 6.0f) - 3.0f) + ((temp_f20 * 6.0f) + (spB0.x * 0.1f));
        spA4.y = (Rand_ZeroOne() * 3.0f) + 1.0f;
        spA4.z = ((Rand_ZeroOne() * 6.0f) - 3.0f) + ((temp_f22 * -6.0f) + (spB0.z * 0.1f));

        spB0.x += this->actor.world.pos.x;
        spB0.y += this->actor.world.pos.y;
        spB0.z += this->actor.world.pos.z;

        sp9C.x = Rand_Next() & 0xFFFF;
        sp9C.y = Rand_Next() & 0xFFFF;
        sp9C.z = Rand_Next() & 0xFFFF;

        sp94.x = Rand_S16Offset(-12000, 24000);
        sp94.y = (Rand_Next() % 0x4000) - 0x1FFF;
        sp94.z = Rand_S16Offset(-12000, 24000);
        func_80A5C2FC(&D_80A5F1C0, (Rand_ZeroOne() * 0.38f) + 0.02f, &spB0, &spA4, &sp9C, &sp94);
    }
}

static InitChainEntry sInitChain[] = {
    ICHAIN_F32_DIV1000(gravity, -2000, ICHAIN_CONTINUE),  ICHAIN_F32_DIV1000(terminalVelocity, -17000, ICHAIN_CONTINUE),
    ICHAIN_VEC3F_DIV1000(scale, 400, ICHAIN_CONTINUE),    ICHAIN_F32(cullingVolumeDistance, 1200, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeScale, 100, ICHAIN_CONTINUE), ICHAIN_F32(cullingVolumeDownward, 100, ICHAIN_STOP),
};

void EnKusa2_Init(Actor* thisx, PlayState* play) {
    s32 pad;
    EnKusa2* this = (EnKusa2*)thisx;

    Actor_ProcessInitChain(&this->actor, sInitChain);
    if (!ENKUSA2_GET_1(&this->actor)) {
        this->actor.update = func_80A5E604;
        this->actor.draw = NULL;
        this->actor.flags |= ACTOR_FLAG_DRAW_CULLING_DISABLED;
        Actor_ChangeCategory(play, &play->actorCtx, &this->actor, ACTORCAT_BG);
        this->unk_1BE = 0;
        if (D_80A5EAEC) {
            D_80A5EAEC = false;
            D_80A60900 = play->gameplayFrames;
            func_80A5CAD4(&D_80A5F1C0);
            D_80A60B08 = Rand_Next() >> 0x10;
            D_80A60B0A = Rand_Next() >> 0x10;
            D_80A60B0C = Rand_Next() >> 0x10;
            D_80A60B0E = Rand_Next() >> 0x10;
            D_80A60B10 = Rand_Next() >> 0x10;
            func_80A5B508();
        }
        func_80A5D5E0(this);
    } else {
        Collider_InitCylinder(play, &this->collider);
        if (!func_80A5BA58(this, play)) {
            Actor_Kill(&this->actor);
            return;
        }

        Collider_SetCylinder(play, &this->collider, &this->actor, &sCylinderInit);
        Collider_UpdateCylinder(&this->actor, &this->collider);
        this->actor.colChkInfo.mass = MASS_IMMOVABLE;
        this->unk_1CE = D_80A5EAF0 & 7;
        D_80A5EAF0++;
        this->actor.shape.shadowAlpha = 0;
        this->unk_1CF = 255;
        this->actor.shape.shadowScale = 1.0f;
        func_80A5D7A4(this);
    }
}

void EnKusa2_Destroy(Actor* thisx, PlayState* play) {
    EnKusa2* this = (EnKusa2*)thisx;

    if (ENKUSA2_GET_1(&this->actor) == 1) {
        Collider_DestroyCylinder(play, &this->collider);
    }
}

void func_80A5D5E0(EnKusa2* this) {
    this->actionFunc = func_80A5D5F4;
}

void func_80A5D5F4(EnKusa2* this, PlayState* play) {
    func_80A5D618(this);
}

void func_80A5D618(EnKusa2* this) {
    this->actionFunc = func_80A5D62C;
}

void func_80A5D62C(EnKusa2* this, PlayState* play) {
    Ship_ExtendedCullingActorAdjustProjectedX(&this->actor);
    Ship_ExtendedCullingActorAdjustProjectedZ(&this->actor);

    if (this->unk_1BE != 0) {
        func_80A5B490(this, play);
        func_80A5D754(this);
    } else if (Math3D_Dist1DSq(this->actor.projectedPos.x, this->actor.projectedPos.z) < SQ(1600.0f)) {
        func_80A5B160(this, play);
        func_80A5D6B0(this);
    }

    Ship_ExtendedCullingActorRestoreProjectedPos(play, &this->actor);
}

void func_80A5D6B0(EnKusa2* this) {
    this->actionFunc = func_80A5D6C4;
}

void func_80A5D6C4(EnKusa2* this, PlayState* play) {
    func_80A5B3BC(this);

    Ship_ExtendedCullingActorAdjustProjectedX(&this->actor);
    Ship_ExtendedCullingActorAdjustProjectedZ(&this->actor);

    if (this->unk_1BE != 0) {
        func_80A5B490(this, play);
        func_80A5D754(this);
    } else if (Math3D_Dist1DSq(this->actor.projectedPos.x, this->actor.projectedPos.z) > SQ(1750.0f)) {
        func_80A5B334(this, play);
        func_80A5D618(this);
    }

    Ship_ExtendedCullingActorRestoreProjectedPos(play, &this->actor);
}

void func_80A5D754(EnKusa2* this) {
    s32 i;

    for (i = 0; i < ARRAY_COUNT(this->unk_194); i++) {
        this->unk_194[i] = NULL;
    }

    this->actionFunc = func_80A5D794;
}

void func_80A5D794(EnKusa2* this, PlayState* play) {
}

void func_80A5D7A4(EnKusa2* this) {
    this->actor.draw = EnKusa2_Draw;
    this->actionFunc = func_80A5D7C4;
}

void func_80A5D7C4(EnKusa2* this, PlayState* play) {
    Actor* thisx = &this->actor;
    s16 sp2A;

    if (Actor_HasParent(&this->actor, play)) {
        SoundSource_PlaySfxAtFixedWorldPos(play, &thisx->world.pos, 20, NA_SE_PL_PULL_UP_PLANT);
        thisx->shape.shadowDraw = ActorShadow_DrawCircle;
        thisx->shape.shadowAlpha = 60;
        thisx->room = -1;
        func_80A5BD94(this);
        func_80A5D964(this);
        return;
    }

    if (!func_80A5BFD8(this, play)) {
        if ((this->unk_1C0 != NULL) && (this->unk_1C0->unk_1BE != 0)) {
            thisx->shape.shadowDraw = ActorShadow_DrawCircle;
            if (this->unk_1C0 != NULL) {
                sp2A = Actor_WorldYawTowardActor(&this->unk_1C0->actor, thisx);
                thisx->home.rot.y = Rand_S16Offset(-1500, 3000) + sp2A;
            }
            this->unk_1C8 = Rand_S16Offset(72, 16);
            thisx->velocity.y = 8.8f;
            Actor_PlaySfx(thisx, NA_SE_EN_NUTS_WALK);
            func_80A5DC70(this);
        }

        if (thisx->xzDistToPlayer < 600.0f) {
            CollisionCheck_SetAC(play, &play->colChkCtx, &this->collider.base);
            if (thisx->xzDistToPlayer < 400.0f) {
                CollisionCheck_SetOC(play, &play->colChkCtx, &this->collider.base);
                if (thisx->xzDistToPlayer < 100.0f) {
                    Actor_OfferCarry(thisx, play);
                }
            }
        }
    }
}

void func_80A5D964(EnKusa2* this) {
    this->actor.draw = func_80A5E9B4;
    this->actionFunc = func_80A5D9C8;
    D_80A5EAF4.x = 0;
    D_80A5EAF4.y = 0;
    D_80A5EAF4.z = 0;
    D_80A5EAFC.x = 0;
    D_80A5EAFC.y = 0;
    D_80A5EAFC.z = 0;
    D_80A5EB04.x = 0;
    D_80A5EB04.y = 0;
    D_80A5EB04.z = 0;
    this->unk_1D0 = 30;
}

void func_80A5D9C8(EnKusa2* this, PlayState* play) {
    s32 pad;
    Vec3f sp30;
    s32 bgId;

    D_80A5EAF4.x += 11000;
    D_80A5EAF4.y += 17000;
    D_80A5EAF4.z += 9000;

    this->actor.scale.x = (Math_SinS(D_80A5EAF4.x) * 0.020000001f) + 0.4f;
    this->actor.scale.y = (Math_SinS(D_80A5EAF4.y) * 0.020000001f) + 0.4f;
    this->actor.scale.z = (Math_SinS(D_80A5EAF4.z) * 0.020000001f) + 0.4f;

    D_80A5EB04.x += 18000;
    D_80A5EB04.y += 17000;
    D_80A5EB04.z += 16000;

    D_80A5EAFC.x = Math_SinS(D_80A5EB04.x) * 1000.0f;
    D_80A5EAFC.y = Math_SinS(D_80A5EB04.y) * 1000.0f;
    D_80A5EAFC.z = Math_SinS(D_80A5EB04.z) * 1000.0f;

    if (Actor_HasNoParent(&this->actor, play)) {
        this->actor.room = play->roomCtx.curRoom.num;
        this->actor.colChkInfo.mass = 80;
        this->actor.home.rot.y = this->actor.world.rot.y;
        this->actor.velocity.y = 12.5f;
        this->actor.speed += 3.0f;
        Actor_MoveWithGravity(&this->actor);
        func_80A5BAFC(this, play);
        func_80A5CD0C(this);
        this->unk_1C8 = 30;
        func_80A5DC70(this);
    } else {
        sp30.x = this->actor.world.pos.x;
        sp30.y = this->actor.world.pos.y + 20.0f;
        sp30.z = this->actor.world.pos.z;

        if (Rand_Next() < 0xFFFFFFF) {
            func_80A5CD0C(this);
        }

        this->unk_1D0--;
        if (this->unk_1D0 <= 0) {
            Actor_PlaySfx(&this->actor, NA_SE_EN_KUSAMUSHI_VIBE);
            this->unk_1D0 = (Rand_Next() >> 0x1D) + 14;
        }

        this->actor.floorHeight =
            BgCheck_EntityRaycastFloor5(&play->colCtx, &this->actor.floorPoly, &bgId, &this->actor, &sp30);
    }
}

void func_80A5DC70(EnKusa2* this) {
    this->actionFunc = func_80A5DC98;
    this->actor.draw = func_80A5EA48;
    this->unk_1CA = 80;
}

void func_80A5DC98(EnKusa2* this, PlayState* play) {
    if (!func_80A5BFD8(this, play)) {
        if (this->actor.velocity.y > 1.0f) {
            Math_StepToF(&this->actor.scale.y, 0.3f, 0.060000002f);
            Math_StepToF(&this->actor.scale.x, 0.328f, 0.040000003f);
        } else {
            Math_StepToF(&this->actor.scale.y, 0.48000002f, 0.080000006f);
            Math_StepToF(&this->actor.scale.x, 0.28f, 0.032f);
        }

        this->actor.scale.z = this->actor.scale.x;
        Math_StepToF(&this->actor.gravity, -7.0f, 1.8f);
        Actor_MoveWithGravity(&this->actor);
        func_80A5BAFC(this, play);
        func_80A5BF38(this, 4);
        func_80A5BF84(this, play);

        if (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND) {
            func_80A5CD0C(this);
            func_80A5BB40(this, play, 1);
            Actor_PlaySfx(&this->actor, NA_SE_EN_NUTS_WALK);
            func_80A5DE18(this);
        } else if (this->actor.bgCheckFlags & BGCHECKFLAG_WATER) {
            func_80A5BDB0(this, play);
            func_80A5E418(this);
        } else if (this->unk_1CA > 0) {
            this->unk_1CA--;
        } else {
            Actor_Kill(&this->actor);
        }
    }
}

void func_80A5DE18(EnKusa2* this) {
    this->actionFunc = func_80A5DEB4;
    this->actor.draw = func_80A5EA48;
    this->actor.gravity = -2.0f;
    this->unk_1D0 = (s32)(Rand_ZeroOne() * 3.0f);
    this->unk_1D1 = (s32)(Rand_ZeroOne() * 8.0f) + 7;
    this->unk_1CA = Rand_S16Offset(19, 2);
}

void func_80A5DEB4(EnKusa2* this, PlayState* play) {
    s32 pad;
    s32 pad2;
    s16 sp24;
    s32 sp20;

    if (!func_80A5BFD8(this, play)) {
        if (this->unk_1D0 > 0) {
            this->unk_1D0--;
            if (this->unk_1D0 == 0) {
                Actor_PlaySfx(&this->actor, NA_SE_EN_KUSAMUSHI_VIBE);
            }
        }

        if (this->unk_1D1 > 0) {
            this->unk_1D1--;
            if (this->unk_1D1 == 0) {
                Actor_PlaySfx(&this->actor, NA_SE_EN_KUSAMUSHI_VIBE);
            }
        }

        Math_StepToF(&this->actor.scale.y, 0.4f, 0.032f);
        Math_StepToF(&this->actor.scale.x, 0.4f, 0.032f);
        this->unk_1C4 += 0x4650;
        this->actor.scale.z = this->actor.scale.x;
        this->actor.speed = fabsf(Math_CosS(this->unk_1C4) + 0.5f) * 1.5f;

        if (this->actor.speed < 0.0f) {
            this->actor.speed = 0.0f;
        }

        Actor_MoveWithGravity(&this->actor);

        this->actor.world.rot.y = (s32)(Math_SinS(this->unk_1C4) * 3000.0f) + this->actor.home.rot.y;
        this->actor.shape.rot.y = this->actor.world.rot.y;
        this->actor.shape.rot.x = (s32)(fabsf(Math_SinS(this->unk_1C4)) * 4000.0f) - 0x6A4;

        func_80A5BAFC(this, play);
        func_80A5BF38(this, 4);
        func_80A5BF84(this, play);

        if (Rand_Next() < 0xFFFFFFF) {
            func_80A5CD0C(this);
        }

        if (this->actor.bgCheckFlags & BGCHECKFLAG_WATER) {
            func_80A5BDB0(this, play);
            func_80A5E418(this);
        } else {
            this->unk_1C8--;
            if (this->unk_1C8 <= 0) {
                func_80A5CD0C(this);
                func_80A5BB40(this, play, 4);
                this->actor.speed += 4.0f;
                this->actor.velocity.y = (Rand_ZeroOne() * 4.0f) + 15.0f;
                func_80A5E1D8(this);
            } else if (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND) {
                this->unk_1CA--;
                if (this->unk_1CA <= 0) {
                    func_80A5CD0C(this);
                    func_80A5BB40(this, play, 2);
                    this->actor.velocity.y = (Rand_ZeroOne() * 6.0f) + 7.0f;
                    this->actor.speed += 2.0f;
                    if (this->actor.yawTowardsPlayer < this->actor.world.rot.y) {
                        sp20 = 10000;
                    } else {
                        sp20 = -10000;
                    }
                    sp24 = sp20 + Rand_S16Offset(-3000, 6000);
                    this->actor.home.rot.y += sp24;
                    this->actor.world.rot.y = this->actor.home.rot.y;
                    func_80A5DC70(this);
                }
            }
        }
    }
}

void func_80A5E1D8(EnKusa2* this) {
    this->actionFunc = func_80A5E210;
    this->actor.draw = func_80A5EA48;
    this->unk_1CC = 0;
    func_80A5CB74(this);
}

void func_80A5E210(EnKusa2* this, PlayState* play) {
    Math_StepToF(&this->actor.gravity, -4.0f, 0.5f);

    if (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND) {
        this->actor.speed *= 0.4f;
        if (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND_TOUCH) {
            func_80A5D178(this);
            SoundSource_PlaySfxAtFixedWorldPos(play, &this->actor.world.pos, 40, NA_SE_EN_KUSAMUSHI_HIDE);
        }
    }

    Actor_MoveWithGravity(&this->actor);
    func_80A5BAFC(this, play);

    if (this->actor.velocity.y > 0.01f) {
        func_80A5BF38(this, 4);
        this->unk_1CC += 1000;
    } else {
        func_80A5BF60(this, 10);
        this->unk_1CF -= 20;
        this->unk_1CC += 5000;
    }

    if (this->unk_1CC > 20000) {
        this->unk_1CC = 20000;
    }

    this->actor.shape.rot.y += this->unk_1CC;

    if (this->actor.velocity.y > 1.0f) {
        Math_StepToF(&this->actor.scale.y, 0.24000001f, 0.080000006f);
        Math_StepToF(&this->actor.scale.x, 0.32000002f, 0.040000003f);
    } else if (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND) {
        Math_StepToF(&this->actor.scale.y, 0.0f, 0.120000005f);
        Math_StepToF(&this->actor.scale.x, 0.48000002f, 0.120000005f);
    } else {
        Math_StepToF(&this->actor.scale.y, 0.56f, 0.1f);
        Math_StepToF(&this->actor.scale.x, 0.24000001f, 0.1f);
    }

    this->actor.scale.z = this->actor.scale.x;

    if (this->unk_1CF <= 20) {
        if (this->unk_1B8 != NULL) {
            this->unk_1B8->unk_28 = func_80A5C718;
        }
        Actor_Kill(&this->actor);
    }
}

void func_80A5E418(EnKusa2* this) {
    this->actor.terminalVelocity = -4.0f;
    this->actor.velocity.x *= 0.1f;
    this->actor.velocity.y *= 0.25f;
    this->actor.velocity.z *= 0.1f;
    this->actor.draw = func_80A5EA48;
    this->actor.speed *= 0.1f;
    this->actor.gravity *= 0.25f;
    this->unk_1CC = Rand_S16Offset(-800, 1600);
    this->actionFunc = func_80A5E4BC;
}

void func_80A5E4BC(EnKusa2* this, PlayState* play) {
    Math_StepToF(&this->actor.scale.y, 0.4f, 0.032f);
    Math_StepToF(&this->actor.scale.x, 0.4f, 0.032f);
    this->actor.scale.z = this->actor.scale.x;
    Actor_MoveWithGravity(&this->actor);
    func_80A5BAFC(this, play);
    func_80A5BF60(this, 6);
    this->actor.shape.rot.x -= 0x5DC;
    this->actor.shape.rot.y += this->unk_1CC;

    if ((this->actor.bgCheckFlags & BGCHECKFLAG_WATER) && (this->actor.depthInWater > 5.0f) &&
        (Rand_ZeroOne() < 0.8f)) {
        EffectSsBubble_Spawn(play, &this->actor.world.pos, 20.0f, 10.0f, 20.0f, (Rand_ZeroOne() * 0.08f) + 0.04f);
    }

    this->unk_1CF -= 15;
    if (this->unk_1CF <= 15) {
        Actor_Kill(&this->actor);
    }
}

void func_80A5E604(Actor* thisx, PlayState* play) {
    s32 pad;
    EnKusa2* this = (EnKusa2*)thisx;

    this->actionFunc(this, play);

    if (D_80A60900 == play->gameplayFrames) {
        this->actor.draw = NULL;
    } else {
        this->actor.draw = func_80A5E6F0;

        if (play->roomCtx.curRoom.type == ROOM_TYPE_NORMAL) {
            func_80A5B508();
        }
        func_80A5CAF4(&D_80A5F1C0);
        D_80A60900 = play->gameplayFrames;
    }
}

void EnKusa2_Update(Actor* thisx, PlayState* play) {
    EnKusa2* this = (EnKusa2*)thisx;

    if ((this->unk_1C0 != NULL) && (this->unk_1C0->actor.update == NULL)) {
        this->unk_1C0 = NULL;
    }

    this->actionFunc(this, play);

    func_80A5CCD4(this);
}

void func_80A5E6F0(Actor* thisx, PlayState* play) {
    static Gfx* D_80A5EB68[] = {
        gKakeraLeafTipDL,
        gKakeraLeafMiddleDL,
    };
    EnKusa2* this = (EnKusa2*)thisx;
    s32 i;

    OPEN_DISPS(play->state.gfxCtx);

    Gfx_SetupDL25_Opa(play->state.gfxCtx);

    for (i = 0; i < ARRAY_COUNT(D_80A5F1C0.unk_0480); i++) {
        EnKusa2UnkBssSubStruct2* s = &D_80A5F1C0.unk_0480[i];

        if (s->unk_2C > 0) {
            FrameInterpolation_RecordOpenChild(s, 0);
            FrameInterpolation_IgnoreActorMtx();
            Matrix_SetTranslateRotateYXZ(s->unk_04.x, s->unk_04.y, s->unk_04.z, &s->unk_20);
            Matrix_Scale(s->unk_00, s->unk_00, s->unk_00, MTXMODE_APPLY);

            MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx);
            gSPDisplayList(POLY_OPA_DISP++, D_80A5EB68[i & 1]);
            FrameInterpolation_RecordCloseChild();
        }
    }

    CLOSE_DISPS(play->state.gfxCtx);
}

void func_80A5E80C(PlayState* play, s32 arg1) {
    OPEN_DISPS(play->state.gfxCtx);

    Gfx_SetupDL25_Xlu(play->state.gfxCtx);

    MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx);
    gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 255, 255, 255, arg1);
    gSPDisplayList(POLY_XLU_DISP++, gKusaBushType2DL);

    CLOSE_DISPS(play->state.gfxCtx);
}

void EnKusa2_Draw(Actor* thisx, PlayState* play) {
    EnKusa2* this = (EnKusa2*)thisx;

    Ship_ExtendedCullingActorAdjustProjectedZ(&this->actor);

    if (this->actor.projectedPos.z <= 1200.0f) {
        if ((play->roomCtx.curRoom.type == ROOM_TYPE_NORMAL) && (this->actor.projectedPos.z > -150.0f) &&
            (this->actor.projectedPos.z < 400.0f)) {
            func_80A5B954(&D_80A60908[this->unk_1CE], 0.0015f);
        }
        if (GameInteractor_Should(VB_KEATON_GRASS_DRAW, true, this)) {
            Gfx_DrawDListOpa(play, gKusaBushType1DL);
        }
    } else if (this->actor.projectedPos.z < 1300.0f) {
        func_80A5E80C(play, (1300.0f - this->actor.projectedPos.z) * 2.55f);
    }

    Ship_ExtendedCullingActorRestoreProjectedPos(play, &this->actor);
}

void func_80A5E9B4(Actor* thisx, PlayState* play) {
    Vec3s sp18;

    sp18.x = thisx->shape.rot.x + D_80A5EAFC.x;
    sp18.y = thisx->shape.rot.y + D_80A5EAFC.y;
    sp18.z = thisx->shape.rot.z + D_80A5EAFC.z;
    Matrix_SetTranslateRotateYXZ(thisx->world.pos.x, thisx->world.pos.y, thisx->world.pos.z, &sp18);
    Matrix_Scale(thisx->scale.x, thisx->scale.y, thisx->scale.z, MTXMODE_APPLY);
    if (GameInteractor_Should(VB_KEATON_GRASS_DRAW, true, thisx)) {
        Gfx_DrawDListOpa(play, gKusaBushType1DL);
    }
}

void func_80A5EA48(Actor* thisx, PlayState* play) {
    EnKusa2* this = (EnKusa2*)thisx;

    if (this->unk_1CF == 0xFF) {
        if (GameInteractor_Should(VB_KEATON_GRASS_DRAW, true, this)) {
            Gfx_DrawDListOpa(play, gKusaBushType1DL);
        }
    } else {
        func_80A5E80C(play, this->unk_1CF);
    }
}

void EnKusa2_Reset(void) {
    D_80A5EAEC = 1;
    D_80A5EAF0 = 0;
}
