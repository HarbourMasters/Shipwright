/*
 * File: z_bg_hakugin_post.c
 * Overlay: ovl_Bg_Hakugin_Post
 * Description: Snowhead Temple Central Pillar
 */

#include "z_bg_hakugin_post.h"
#include "z64quake.h"
#include "z64rumble.h"
#include "objects/object_hakugin_obj/object_hakugin_obj.h"
#include <string.h>

#define FLAGS (ACTOR_FLAG_UPDATE_CULLING_DISABLED | ACTOR_FLAG_DRAW_CULLING_DISABLED)

void BgHakuginPost_Init(Actor* thisx, PlayState* play);
void BgHakuginPost_Destroy(Actor* thisx, PlayState* play);
void BgHakuginPost_Update(Actor* thisx, PlayState* play);
void BgHakuginPost_Reset(void);

void func_80A9CA94(BgHakuginPost* this);
void func_80A9CAA8(BgHakuginPost* this, PlayState* play);
void func_80A9CC84(BgHakuginPost* this);
void func_80A9CCA0(BgHakuginPost* this, PlayState* play);
void func_80A9CD00(BgHakuginPost* this);
void func_80A9CD14(BgHakuginPost* this, PlayState* play);
void func_80A9CE00(BgHakuginPost* this);
void func_80A9CE1C(BgHakuginPost* this, PlayState* play);
void func_80A9D0A0(BgHakuginPost* this);
void func_80A9D0B4(BgHakuginPost* this, PlayState* play);
void func_80A9D1E0(BgHakuginPost* this, BgHakuginPostFunc unkFunc, f32 arg2, s16 csLength, s16 csId);
void func_80A9D260(BgHakuginPost* this, PlayState* play);
void func_80A9D2C4(BgHakuginPost* this, BgHakuginPostFunc unkFunc, f32 arg2, s16 csId, s16 additionalCsId);
void func_80A9D360(BgHakuginPost* this, PlayState* play);
void func_80A9D3E4(BgHakuginPost* this);
void func_80A9D434(BgHakuginPost* this, PlayState* play);
void func_80A9D61C(Actor* thisx, PlayState* play);

BgHakuginPostColliders D_80A9DDC0;
BgHakuginPostUnkStruct D_80A9E028;

ActorProfile Bg_Hakugin_Post_Profile = {
    /**/ ACTOR_BG_HAKUGIN_POST,
    /**/ ACTORCAT_BG,
    /**/ FLAGS,
    /**/ OBJECT_HAKUGIN_OBJ,
    /**/ sizeof(BgHakuginPost),
    /**/ BgHakuginPost_Init,
    /**/ BgHakuginPost_Destroy,
    /**/ BgHakuginPost_Update,
    /**/ NULL,
    /**/ BgHakuginPost_Reset,
};

typedef struct {
    /* 0x0 */ f32 unk_00;
    /* 0x4 */ u8 unk_04;
} BgHakuginPostUnkStruct3; // size = 0x8

static BgHakuginPostUnkStruct3 D_80A9D880[] = {
    { 1200.0f, false }, { 600.0f, false }, { 900.0f, false }, { 870.0f, false },
    { 120.0f, false },  { 120.0f, true },  { 200.0f, true },
};

static ColliderCylinderInit sCylinderInit = {
    {
        COL_MATERIAL_NONE,
        AT_NONE,
        AC_ON | AC_TYPE_PLAYER,
        OC1_NONE,
        OC2_NONE,
        COLSHAPE_CYLINDER,
    },
    {
        ELEM_MATERIAL_UNK0,
        { 0x00000000, 0x00, 0x00 },
        { 0x00000100, 0x00, 0x00 },
        ATELEM_NONE | ATELEM_SFX_NORMAL,
        ACELEM_ON,
        OCELEM_NONE,
    },
    { 276, 0, 20, { 0, 0, 0 } },
};

static Color_RGBA8 D_80A9D8E4 = { 250, 250, 250, 255 };
static Color_RGBA8 D_80A9D8E8 = { 180, 180, 180, 255 };
static Vec3f D_80A9D8EC = { 0.0f, 0.3f, 0.0f };

static InitChainEntry sInitChain[] = {
    ICHAIN_VEC3F_DIV1000(scale, 100, ICHAIN_STOP),
};

void func_80A9ACD0(BgHakuginPostUnkStruct* arg0) {
    memset(arg0, 0, sizeof(BgHakuginPostUnkStruct));
}

void func_80A9ACF0(void) {
    memset(&D_80A9DDC0, 0, sizeof(BgHakuginPostColliders));
}

void func_80A9AD18(BgHakuginPost* this, PlayState* play, BgHakuginPostUnkStruct* unkStruct) {
    s32 i;

    for (i = 0; i < unkStruct->count; i++) {
        BgHakuginPostUnkStruct3* unkStruct3 = &D_80A9D880[unkStruct->unk_0000[i].unk_00];

        if ((unkStruct3->unk_04) && (D_80A9DDC0.count < 8)) {
            ColliderCylinder* collider = &D_80A9DDC0.colliders[D_80A9DDC0.count];

            Collider_InitCylinder(play, collider);
            Collider_SetCylinder(play, collider, &this->dyna.actor, &sCylinderInit);
            collider->dim.height = (s32)unkStruct3->unk_00 - 40;
            unkStruct->unk_0000[i].collider = collider;
            D_80A9DDC0.count++;
        }
    }
}

void func_80A9AE3C(BgHakuginPost* this, PlayState* play) {
    while (D_80A9DDC0.count != 0) {
        D_80A9DDC0.count--;
        Collider_DestroyCylinder(play, &D_80A9DDC0.colliders[D_80A9DDC0.count]);
    }
}

void func_80A9AEB8(BgHakuginPost* this, PlayState* play, BgHakuginPostUnkStruct* unkStruct) {
    s32 i;

    for (i = 0; i < unkStruct->count; i++) {
        ColliderCylinder* collider = unkStruct->unk_0000[i].collider;

        if ((collider != NULL) && (unkStruct->unk_0000[i].unk_34 == 1)) {
            collider->dim.pos.x = unkStruct->unk_0000[i].unk_14.x + this->dyna.actor.home.pos.x;
            collider->dim.pos.y = unkStruct->unk_0000[i].unk_14.y + this->unk_16C;
            collider->dim.pos.z = unkStruct->unk_0000[i].unk_14.z + this->dyna.actor.home.pos.z;
            CollisionCheck_SetAC(play, &play->colChkCtx, &collider->base);
        }
    }
}

void func_80A9AFB4(BgHakuginPost* this, PlayState* play, BgHakuginPostUnkStruct* unkStruct) {
    s32 i;
    s32 j;

    if (unkStruct->count < ARRAY_COUNT(unkStruct->unk_0000)) {
        for (i = 0; i < unkStruct->count; i++) {
            if ((unkStruct->unk_0000[i].unk_34 == 0) ||
                (this->dyna.actor.world.pos.y < unkStruct->unk_0000[i].unk_08.y)) {
                for (j = 10; j >= i; j--) {
                    memcpy(&unkStruct->unk_0000[j + 1], &unkStruct->unk_0000[j], sizeof(BgHakuginPostUnkStruct1));
                }
                break;
            }
        }

        unkStruct->unk_0000[i].unk_00 = BGHAKUGINPOST_GET_7(&this->dyna.actor);
        Math_Vec3f_Copy(&unkStruct->unk_0000[i].unk_08, &this->dyna.actor.world.pos);
        Math_Vec3f_Copy(&unkStruct->unk_0000[i].unk_14, &this->dyna.actor.world.pos);
        unkStruct->unk_0000[i].switchFlag = BGHAKUGINPOST_GET_SWITCH_FLAG(&this->dyna.actor);
        if (D_80A9D880[unkStruct->unk_0000[i].unk_00].unk_04) {
            if (Flags_GetSwitch(play, unkStruct->unk_0000[i].switchFlag)) {
                unkStruct->unk_0000[i].unk_34 = 5;
            } else {
                unkStruct->unk_0000[i].unk_34 = 1;
            }
            unkStruct->unk_0000[i].csId = this->dyna.actor.csId;
            unkStruct->unk_0000[i].additionalCsId = CutsceneManager_GetAdditionalCsId(unkStruct->unk_0000[i].csId);
        } else {
            unkStruct->unk_0000[i].unk_34 = 1;
            unkStruct->unk_0000[i].csId = CS_ID_NONE;
            unkStruct->unk_0000[i].additionalCsId = CS_ID_NONE;
        }

        unkStruct->unk_0000[i].unk_2F = 1;
        unkStruct->count++;
    }
}

void func_80A9B160(BgHakuginPostUnkStruct* unkStruct, PlayState* play) {
    s32 i;

    for (i = 0; i < unkStruct->count; i++) {
        Math_Vec3f_Copy(&unkStruct->unk_0000[i].unk_14, &unkStruct->unk_0000[i].unk_08);
        unkStruct->unk_0000[i].unk_20 = 0.0f;
        unkStruct->unk_0000[i].unk_24 = 0.0f;
        unkStruct->unk_0000[i].unk_34 = 1;
        if (D_80A9D880[unkStruct->unk_0000[i].unk_00].unk_04 != 0) {
            Flags_UnsetSwitch(play, unkStruct->unk_0000[i].switchFlag);
        }
    }
}

f32 func_80A9B244(BgHakuginPostUnkStruct* unkStruct) {
    s32 i;
    f32 ret = 0.0f;

    for (i = 0; i < unkStruct->count; i++) {
        ret += D_80A9D880[unkStruct->unk_0000[i].unk_00].unk_00;
    }

    return ret;
}

f32 func_80A9B2B8(BgHakuginPostUnkStruct* unkStruct) {
    s32 i;

    for (i = unkStruct->count - 1; i >= 0; i--) {
        if ((unkStruct->unk_0000[i].unk_34 == 1) || (unkStruct->unk_0000[i].unk_34 == 2)) {
            return unkStruct->unk_0000[i].unk_14.y + D_80A9D880[unkStruct->unk_0000[i].unk_00].unk_00;
        }
    }

    return 0.0f;
}

BgHakuginPostUnkStruct1* func_80A9B32C(BgHakuginPostUnkStruct* unkStruct, BgHakuginPostUnkStruct1* unkStruct1) {
    s32 i;

    for (i = 0; i < unkStruct->count; i++) {
        if (unkStruct1 == &unkStruct->unk_0000[i]) {
            break;
        }
    }

    i++;

    if (i < unkStruct->count) {
        return &unkStruct->unk_0000[i];
    }

    return NULL;
}

void func_80A9B384(Vec3f* arg0) {
    MtxF* matrix = Matrix_GetCurrent();

    matrix->mf[3][0] = arg0->x;
    matrix->mf[3][1] = arg0->y;
    matrix->mf[3][2] = arg0->z;
}

void func_80A9B3BC(BgHakuginPost* this, PlayState* play) {
    s32 pad;
    s32 switchFlag1 = BGHAKUGINPOST_GET_SWITCH_FLAG(&this->dyna.actor);
    s32 switchFlag2 = BGHAKUGINPOST_GET_SWITCH_FLAG(&this->dyna.actor) + 1;
    s32 sp20;
    s32 sp1C;

    if (Flags_GetSwitch(play, switchFlag1)) {
        sp20 = true;
    } else {
        sp20 = false;
    }

    if (Flags_GetSwitch(play, switchFlag2)) {
        sp1C = true;
    } else {
        sp1C = false;
    }

    if (!(sp20 | sp1C)) {
        Flags_SetSwitch(play, switchFlag1);
        this->unk_170 = true;
    } else {
        this->unk_170 = sp20;
    }

    this->unk_174 = sp1C;
}

void func_80A9B46C(BgHakuginPost* this, PlayState* play) {
    s32 pad;
    s32 switchFlag1 = BGHAKUGINPOST_GET_SWITCH_FLAG(&this->dyna.actor);
    s32 switchFlag2 = BGHAKUGINPOST_GET_SWITCH_FLAG(&this->dyna.actor) + 1;
    s32 sp28;
    s32 sp24;

    if (Flags_GetSwitch(play, switchFlag1)) {
        sp28 = true;
    } else {
        sp28 = false;
    }

    if (Flags_GetSwitch(play, switchFlag2)) {
        sp24 = true;
    } else {
        sp24 = false;
    }

    if (!this->unk_170 && (sp28 == 1)) {
        Flags_UnsetSwitch(play, switchFlag2);
    } else if (!this->unk_174 && (sp24 == 1)) {
        Flags_UnsetSwitch(play, switchFlag1);
    }

    this->unk_170 = sp28;
    this->unk_174 = sp24;
}

void func_80A9B554(BgHakuginPost* this, PlayState* play, BgHakuginPostUnkStruct* unkStruct,
                   BgHakuginPostUnkStruct1* unkStruct1) {
    f32 spE4 = D_80A9D880[unkStruct1->unk_00].unk_00;
    s32 i;
    s16 val;
    BgHakuginPostUnkStruct2* unkStruct2;
    BgHakuginPostUnkStruct1* unkStruct1Temp;
    f32 temp_f24;
    f32 temp_f0;
    f32 temp_f20;
    f32 temp_f22;
    Vec3f spB8;
    Vec3f spAC;
    Vec3f spA0;
    f32 temp_f28;
    f32 temp_f6;
    s32 pad[2];
    s16 temp_s1_3;

    spB8.x = unkStruct1->unk_14.x + this->dyna.actor.world.pos.x;
    spB8.y = unkStruct1->unk_14.y + this->unk_16C;
    spB8.z = unkStruct1->unk_14.z + this->dyna.actor.world.pos.z;

    spAC.x = Math_SinS(unkStruct1->unk_28) * unkStruct1->unk_20;
    spAC.y = unkStruct1->unk_24;
    spAC.z = Math_CosS(unkStruct1->unk_28) * unkStruct1->unk_20;

    for (i = 0, val = 0; i < 65; i++, val += 0x3F0) {
        unkStruct2 = &unkStruct->unk_02A4[i];
        unkStruct2->unk_00 = Rand_ZeroOne() * 0.038f + 0.002f;
        temp_f24 = Math_SinS(val);
        temp_f28 = Math_CosS(val);
        temp_f20 = Rand_ZeroOne();
        temp_f20 = (1.0f - SQ(temp_f20)) * 198.0f;
        unkStruct2->unk_04.x = temp_f24 * temp_f20 + spB8.x;
        unkStruct2->unk_04.y = Rand_ZeroOne() * spE4 + spB8.y;
        unkStruct2->unk_04.z = temp_f28 * temp_f20 + spB8.z;

        temp_f0 = unkStruct2->unk_00 - 0.002f;
        temp_f22 = temp_f0 * -(450.0f / 19.0f) + 1.0f;
        unkStruct2->unk_10.x = Rand_ZeroOne() * 60.0f;
        temp_f20 = temp_f0 * (10.0f / 19.0f) + 0.06f;

        unkStruct2->unk_10.x = ((unkStruct2->unk_10.x - 30.0f) + temp_f24 * 50.0f) * temp_f22 + spAC.x * temp_f20;
        unkStruct2->unk_10.y = (Rand_ZeroOne() * 102.0f + 18.0f) * temp_f22 + spAC.y * temp_f20;
        temp_f0 = spAC.z;
        unkStruct2->unk_10.z = ((Rand_ZeroOne() * 60.0f - 30.0f) + temp_f28 * 50.0f) * temp_f22 + temp_f0 * temp_f20;
        unkStruct2->unk_1C = 0.90999997f - (0.04f - unkStruct2->unk_00) * (500.0f / 19.0f) * 0.02f;
        unkStruct2->unk_20.x = (s32)Rand_Next() >> 0x10;
        unkStruct2->unk_20.y = (s32)Rand_Next() >> 0x10;
        unkStruct2->unk_20.z = (s32)Rand_Next() >> 0x10;
        unkStruct2->unk_26 = (Rand_Next() & 0x3FFF) - 0x1FFF;
        unkStruct2->unk_28 = (Rand_Next() & 0x1FFF) - 0xFFF;
        unkStruct2->unk_2A = (Rand_Next() & 0x1FFF) - 0xFFF;
        unkStruct2->unk_2C = (Rand_Next() & 0x1F) + 0x4B;
        unkStruct2->unk_2D = 2;
    }

    for (i = 0, val = 0; i < 22; i++, val += 0xBA2) {
        temp_f20 = Rand_ZeroOne() * 60.0f + 220.0f;
        spA0.x = Math_SinS(val) * temp_f20 + spB8.x;
        spA0.y = (Rand_ZeroOne() * 1.2f - 0.1f) * spE4 + spB8.y;
        spA0.z = Math_CosS(val) * temp_f20 + spB8.z;
        func_800B0E48(play, &spA0, &gZeroVec3f, &D_80A9D8EC, &D_80A9D8E4, &D_80A9D8E8,
                      ((s32)Rand_Next() >> 0x1A) + 0x82, ((s32)Rand_Next() >> 0x1A) + 0x6E);
    }

    unkStruct1Temp = func_80A9B32C(unkStruct, unkStruct1);
    if (unkStruct1Temp != NULL) {
        temp_f6 = this->unk_16C + func_80A9B2B8(unkStruct) - 40.0f;
        spB8.x = unkStruct1Temp->unk_14.x + this->dyna.actor.world.pos.x;
        spB8.y = unkStruct1Temp->unk_14.y + this->unk_16C;
        spB8.z = unkStruct1Temp->unk_14.z + this->dyna.actor.world.pos.z;

        for (i = 65, val = 0; i < ARRAY_COUNT(unkStruct->unk_02A4); i++, val += 0x4A7) {
            unkStruct2 = &unkStruct->unk_02A4[i];
            unkStruct2->unk_00 = Rand_ZeroOne() * 0.007f + 0.002f;
            temp_s1_3 = (Rand_Next() & 0x7FF) + val;
            temp_f24 = Math_SinS(temp_s1_3);
            temp_f28 = Math_CosS(temp_s1_3);
            unkStruct2->unk_04.x = temp_f24 * 225.5f + spB8.x;
            if (i & 1) {
                unkStruct2->unk_04.y = Rand_ZeroOne() * D_80A9D880[unkStruct1Temp->unk_00].unk_00 + spB8.y;
            } else {
                unkStruct2->unk_04.y = temp_f6 - Rand_ZeroOne() * 350.0f;
            }
            unkStruct2->unk_04.z = temp_f28 * 225.5f + spB8.z;
            unkStruct2->unk_10.x = Rand_ZeroOne() + temp_f24 * 7.0f;
            unkStruct2->unk_10.y = 0.0f;
            unkStruct2->unk_10.z = Rand_ZeroOne() + temp_f28 * 7.0f;
            unkStruct2->unk_1C = 0.90999997f - (0.04f - unkStruct2->unk_00) * (500.0f / 19.0f) * 0.075f;
            unkStruct2->unk_20.x = (s32)Rand_Next() >> 0x10;
            unkStruct2->unk_20.y = (s32)Rand_Next() >> 0x10;
            unkStruct2->unk_20.z = (s32)Rand_Next() >> 0x10;
            unkStruct2->unk_26 = (Rand_Next() & 0x1FFF) - 0xFFF;
            unkStruct2->unk_28 = (Rand_Next() & 0x1FFF) - 0xFFF;
            unkStruct2->unk_2A = (Rand_Next() & 0x1FFF) - 0xFFF;
            unkStruct2->unk_2C = (Rand_Next() & 0x1F) + 0x41;
            unkStruct2->unk_2D = 0;
        }
    }
    unkStruct->unk_1924 = true;
}

void func_80A9BC0C(BgHakuginPostUnkStruct* unkStruct) {
    s32 i;

    if (unkStruct->unk_1924) {
        unkStruct->unk_1924 = false;

        for (i = 0; i < ARRAY_COUNT(unkStruct->unk_02A4); i++) {
            BgHakuginPostUnkStruct2* unkStruct2 = &unkStruct->unk_02A4[i];

            if (unkStruct2->unk_2C > 0) {
                unkStruct2->unk_10.y = unkStruct2->unk_10.y + -4.0f;
                Math_Vec3f_Scale(&unkStruct2->unk_10, unkStruct2->unk_1C);
                unkStruct2->unk_04.x += unkStruct2->unk_10.x;
                unkStruct2->unk_04.y += unkStruct2->unk_10.y;
                unkStruct2->unk_04.z += unkStruct2->unk_10.z;
                unkStruct2->unk_20.x += unkStruct2->unk_26;
                unkStruct2->unk_20.y += unkStruct2->unk_28;
                unkStruct2->unk_20.z += unkStruct2->unk_2A;
                unkStruct2->unk_2C--;
                unkStruct->unk_1924 = true;
            }
        }
    }
}

void func_80A9BD24(BgHakuginPost* this, PlayState* play, BgHakuginPostUnkStruct* unkStruct) {
    s32 i;
    s32 j;

    for (i = 0; i < unkStruct->count; i++) {
        if ((unkStruct->unk_0000[i].unk_34 == 5) || (unkStruct->unk_0000[i].unk_34 == 4)) {
            unkStruct->unk_0000[i].unk_24 = 0.0f;
        } else {
            unkStruct->unk_0000[i].unk_24 -= 2.8f;
            unkStruct->unk_0000[i].unk_24 *= 0.9f;
        }

        if ((unkStruct->unk_0000[i].unk_34 == 2) || (unkStruct->unk_0000[i].unk_34 == 3)) {
            unkStruct->unk_0000[i].unk_20 = 25.0f;
        } else {
            unkStruct->unk_0000[i].unk_20 = 0.0f;
        }

        unkStruct->unk_0000[i].unk_14.x += Math_SinS(unkStruct->unk_0000[i].unk_28) * unkStruct->unk_0000[i].unk_20;
        unkStruct->unk_0000[i].unk_14.z =
            Math_CosS(unkStruct->unk_0000[i].unk_28) * unkStruct->unk_0000[i].unk_20 + unkStruct->unk_0000[i].unk_14.z;
        unkStruct->unk_0000[i].unk_14.y += unkStruct->unk_0000[i].unk_24;

        if ((unkStruct->unk_0000[i].unk_34 == 1) || (unkStruct->unk_0000[i].unk_34 == 2)) {
            for (j = i - 1; j >= 0; j--) {
                if ((unkStruct->unk_0000[j].unk_34 == 1) || (unkStruct->unk_0000[j].unk_34 == 2)) {
                    break;
                }
            }

            if (j < 0) {
                if (unkStruct->unk_0000[i].unk_14.y < 0.0f) {
                    unkStruct->unk_0000[i].unk_14.y = 0.0f;
                    unkStruct->unk_0000[i].unk_24 = 0.0f;
                    if (unkStruct->unk_0000[i].unk_2F & 1) {
                        unkStruct->unk_0000[i].unk_2F &= ~2;
                    } else {
                        unkStruct->unk_0000[i].unk_2F |= (2 | 1);
                    }
                } else {
                    unkStruct->unk_0000[i].unk_2F &= ~(2 | 1);
                }
            } else {
                if (unkStruct->unk_0000[i].unk_14.y <
                    (unkStruct->unk_0000[j].unk_14.y + D_80A9D880[unkStruct->unk_0000[j].unk_00].unk_00)) {
                    unkStruct->unk_0000[i].unk_14.y =
                        unkStruct->unk_0000[j].unk_14.y + D_80A9D880[unkStruct->unk_0000[j].unk_00].unk_00;
                    unkStruct->unk_0000[i].unk_24 = 0.0f;
                    if (unkStruct->unk_0000[i].unk_2F & 1) {
                        unkStruct->unk_0000[i].unk_2F &= ~2;
                    } else {
                        unkStruct->unk_0000[i].unk_2F |= (2 | 1);
                    }
                } else {
                    unkStruct->unk_0000[i].unk_2F &= ~(2 | 1);
                }
            }
        }

        if (unkStruct->unk_0000[i].unk_34 == 2) {
            if (Math3D_Dist1DSq(unkStruct->unk_0000[i].unk_14.x, unkStruct->unk_0000[i].unk_14.z) >
                SQ(440.0f) + 0.02f) {
                unkStruct->unk_0000[i].unk_34 = 3;
            }
        } else if (unkStruct->unk_0000[i].unk_34 == 3) {
            if (Math3D_Dist1DSq(unkStruct->unk_0000[i].unk_14.x, unkStruct->unk_0000[i].unk_14.z) > 278784.03f) {
                func_80A9B554(this, play, unkStruct, &unkStruct->unk_0000[i]);
                Audio_PlaySfx_2(NA_SE_EV_GLASSBROKEN_IMPACT);
                unkStruct->unk_0000[i].unk_34 = 4;
                unkStruct->unk_0000[i].unk_30 = 30;
            }
        } else if (unkStruct->unk_0000[i].unk_34 == 4) {
            unkStruct->unk_0000[i].unk_30--;
            if (unkStruct->unk_0000[i].unk_30 <= 0) {
                unkStruct->unk_0000[i].unk_34 = 5;
            }
        }
    }
}

void func_80A9C058(BgHakuginPost* this, PlayState* play, BgHakuginPostUnkStruct* unkStruct) {
    s32 i;
    s32 pad;
    Vec3f sp44;
    s32 pad2[2];
    s16 quakeIndex;

    if (this->unk_170 == 0) {
        for (i = 0; i < unkStruct->count; i++) {
            BgHakuginPostUnkStruct1* unkStruct1 = &unkStruct->unk_0000[i];

            if (unkStruct1->unk_2F & 2) {
                sp44.x = this->dyna.actor.home.pos.x + unkStruct1->unk_14.x;
                sp44.y = this->unk_16C + unkStruct1->unk_14.y;
                sp44.z = this->dyna.actor.home.pos.z + unkStruct1->unk_14.z;
                Rumble_Request(Math3D_Vec3fDistSq(&sp44, &GET_PLAYER(play)->actor.world.pos), 255, 20, 150);

                quakeIndex = Quake_Request(GET_ACTIVE_CAM(play), QUAKE_TYPE_3);
                Quake_SetSpeed(quakeIndex, 20000);
                Quake_SetPerturbations(quakeIndex, 7, 0, 0, 0);
                Quake_SetDuration(quakeIndex, 12);

                if (this->unk_179 <= 0) {
                    Audio_PlaySfx_2(NA_SE_EV_STONEDOOR_STOP);
                    this->unk_179 = 40;
                }
                break;
            }
        }
    }
}

void BgHakuginPost_RequestQuakeAndRumble(BgHakuginPost* this, PlayState* play) {
    s32 pad;
    Player* player = GET_PLAYER(play);
    Camera* activeCam = GET_ACTIVE_CAM(play);
    s16 quakeIndex;

    Rumble_Request(Math3D_Dist2DSq(player->actor.world.pos.x, player->actor.world.pos.z, this->dyna.actor.home.pos.x,
                                   this->dyna.actor.home.pos.z),
                   255, 20, 150);

    quakeIndex = Quake_Request(activeCam, QUAKE_TYPE_3);
    Quake_SetSpeed(quakeIndex, 17232);
    Quake_SetPerturbations(quakeIndex, 6, 0, 0, 0);
    Quake_SetDuration(quakeIndex, 20);
}

void func_80A9C228(BgHakuginPost* this, PlayState* play, BgHakuginPostUnkStruct* unkStruct) {
    s32 val;
    BgHakuginPostUnkStruct1* spC8 = NULL;
    BgHakuginPostUnkStruct1* spC4;
    Vec3f spB8;
    Vec3f spAC;
    Vec3f spA0;
    s32 i;
    f32 temp_f20;
    s16 temp_s0;
    s32 j;

    for (i = 0; i < unkStruct->count; i++) {
        spC4 = spC8;
        spC8 = &unkStruct->unk_0000[i];

        if (unkStruct->unk_0000[i].unk_34 == 2) {
            for (j = 0, val = 0; j < 3; j++, val += 0x5555) {
                temp_f20 = Rand_ZeroOne() * 264.0f;
                temp_s0 = (Rand_Next() & 0xFFF) + val;
                spB8.x = Math_SinS(temp_s0) * temp_f20 + spC8->unk_14.x;
                spB8.z = Math_CosS(temp_s0) * temp_f20 + spC8->unk_14.z;

                if (Math3D_Dist1DSq(spB8.x, spB8.z) < SQ(550.0f) + 0.03f) {
                    spB8.x += this->dyna.actor.home.pos.x;
                    spB8.y = spC8->unk_14.y + this->unk_16C + 20.0f;
                    spB8.z += this->dyna.actor.home.pos.z;

                    spAC.x = Math_SinS(spC8->unk_28) * 3.0f;
                    spAC.y = 0.0f;
                    spAC.z = Math_CosS(spC8->unk_28) * 3.0f;

                    spA0.x = spAC.x * -0.08f;
                    spA0.y = 0.2f;
                    spA0.z = spAC.z * -0.08f;

                    func_800B0E48(play, &spB8, &spAC, &spA0, &D_80A9D8E4, &D_80A9D8E8, 0x50,
                                  (s32)(Rand_ZeroOne() * 60.0f) + 110);
                }
            }
        }

        if ((unkStruct->unk_0000[i].unk_2F & 2) && (spC4 != NULL) && ((spC4->unk_34 == 3) || (spC4->unk_34 == 4))) {
            for (j = 0, val = 0; j < 18; j++, val += 0xE38) {
                temp_f20 = (Rand_ZeroOne() * 0.3f + 1.0f) * 220.0f;
                temp_s0 = (Rand_Next() & 0xFFF) + val;
                spB8.x = Math_SinS(temp_s0) * temp_f20;
                spB8.z = Math_CosS(temp_s0) * temp_f20;

                spAC.x = spB8.x * (4.0f / 275.0f);
                spAC.y = 0.0f;
                spAC.z = spB8.z * (4.0f / 275.0f);

                spB8.x += unkStruct->unk_0000[i].unk_14.x + this->dyna.actor.home.pos.x;
                spB8.y = unkStruct->unk_0000[i].unk_14.y + this->unk_16C;
                spB8.z += unkStruct->unk_0000[i].unk_14.z + this->dyna.actor.home.pos.z;

                spA0.x = spAC.x * -0.08f;
                spA0.y = 0.2f;
                spA0.z = spAC.z * -0.08f;

                func_800B0E48(play, &spB8, &spAC, &spA0, &D_80A9D8E4, &D_80A9D8E8, 0xC8,
                              (s32)(Rand_ZeroOne() * 70.0f) + 100);
            }
        }
    }
}

void func_80A9C634(BgHakuginPost* this, PlayState* play) {
    s32 i;
    Vec3f spB8;
    Vec3f spAC;
    Vec3f spA0;
    s32 pad[2];
    s32 temp_s4;
    f32 temp_f20;

    for (i = 0; i < 70; i++) {
        temp_s4 = (s32)(Rand_ZeroOne() * 6553.0f) + i * 0x1999;
        temp_f20 = ((Rand_ZeroOne() * 0.3f) + 1.0f) * 220.0f;

        spB8.x = Math_SinS(temp_s4);
        spB8.y = i * -(60.0f / 7.0f);
        spB8.z = Math_CosS(temp_s4);

        spAC.x = spB8.x * 19.0f;
        spAC.y = 0.0f;
        spAC.z = spB8.z * 19.0f;

        spB8.x = (spB8.x * temp_f20) + this->dyna.actor.home.pos.x;
        spB8.y += this->unk_164 + this->unk_160;
        spB8.z = (spB8.z * temp_f20) + this->dyna.actor.home.pos.z;

        spA0.x = spAC.x * -0.08f;
        spA0.y = 0.8f;
        spA0.z = spAC.z * -0.08f;
        func_800B0DE0(play, &spB8, &spAC, &spA0, &D_80A9D8E4, &D_80A9D8E8, (s32)(Rand_ZeroOne() * 400.0f) + 2500, -250);
    }
}

void func_80A9C854(BgHakuginPost* this, PlayState* play) {
    s32 pad;
    s32 sp38 = false;
    s32 pad2;
    s32 i;
    s32 pad3;
    s32 sp28;

    if (this->unk_174 != 0) {
        if (Flags_GetSwitch(play, this->dyna.actor.home.rot.z & 0x7F)) {
            sp28 = true;

            for (i = 0; i < D_80A9E028.count; i++) {
                ColliderCylinder* collider = D_80A9E028.unk_0000[i].collider;

                if ((collider != NULL) && Flags_GetSwitch(play, D_80A9E028.unk_0000[i].switchFlag)) {
                    sp28 = false;
                    break;
                }
            }

            if (sp28) {
                sp38 = true;
            }
        }
    }

    if (sp38) {
        Flags_SetSwitch(play, BGHAKUGINPOST_GET_SWITCH_FLAG_2(&this->dyna.actor));
    } else {
        Flags_UnsetSwitch(play, BGHAKUGINPOST_GET_SWITCH_FLAG_2(&this->dyna.actor));
    }
}

// #region 2S2H [Port]
// Moved static vars out of function scope so they can be cleared in actor reset
static s32 D_80A9D8FC = 1;

void BgHakuginPost_Init(Actor* thisx, PlayState* play) {
    // static s32 D_80A9D8FC = 1;
    // #endregion
    BgHakuginPost* this = (BgHakuginPost*)thisx;

    if (D_80A9D8FC != 0) {
        D_80A9D8FC = 0;
        func_80A9ACD0(&D_80A9E028);
        func_80A9ACF0();
    }

    if (BGHAKUGINPOST_GET_7(&this->dyna.actor) == 7) {
        Actor_ProcessInitChain(&this->dyna.actor, sInitChain);
        this->dyna.actor.world.rot.x = 0;
        this->dyna.actor.world.rot.z = 0;
        this->dyna.actor.shape.rot.x = 0;
        this->dyna.actor.shape.rot.z = 0;
        DynaPolyActor_Init(&this->dyna, DYNA_TRANSFORM_POS);
        DynaPolyActor_LoadMesh(play, &this->dyna, &object_hakugin_obj_Colheader_00D3B0);
        func_80A9B3BC(this, play);
        func_80A9CA94(this);
    } else {
        func_80A9AFB4(this, play, &D_80A9E028);
        Actor_Kill(&this->dyna.actor);
    }
}

void BgHakuginPost_Destroy(Actor* thisx, PlayState* play) {
    BgHakuginPost* this = (BgHakuginPost*)thisx;

    if (BGHAKUGINPOST_GET_7(&this->dyna.actor) == 7) {
        DynaPoly_DeleteBgActor(play, &play->colCtx.dyna, this->dyna.bgId);
        func_80A9AE3C(this, play);
    }
}

void func_80A9CA94(BgHakuginPost* this) {
    this->actionFunc = func_80A9CAA8;
}

void func_80A9CAA8(BgHakuginPost* this, PlayState* play) {
    s32 pad[2];
    s32 i;

    this->unk_160 = func_80A9B244(&D_80A9E028);
    Math_Vec3f_Copy(&this->dyna.actor.home.pos, &D_80A9E028.unk_0000[0].unk_08);
    Math_Vec3f_Copy(&this->dyna.actor.world.pos, &this->dyna.actor.home.pos);
    this->unk_168 = this->dyna.actor.home.pos.y;

    if (1) {}

    this->unk_164 = this->unk_168 - 1800.0f;
    if (this->unk_170 != 0) {
        for (i = 0; i < D_80A9E028.count; i++) {
            D_80A9E028.unk_0000[i].unk_08.x -= this->dyna.actor.home.pos.x;
            D_80A9E028.unk_0000[i].unk_08.y -= this->dyna.actor.home.pos.y;
            D_80A9E028.unk_0000[i].unk_08.z -= this->dyna.actor.home.pos.z;

            D_80A9E028.unk_0000[i].unk_14.x = D_80A9E028.unk_0000[i].unk_08.x;
            D_80A9E028.unk_0000[i].unk_14.y = D_80A9E028.unk_0000[i].unk_08.y;
            D_80A9E028.unk_0000[i].unk_14.z = D_80A9E028.unk_0000[i].unk_08.z;
        }
    } else {
        f32 phi_f0 = 0.0f;

        for (i = 0; i < D_80A9E028.count; i++) {
            D_80A9E028.unk_0000[i].unk_08.x -= this->dyna.actor.home.pos.x;
            D_80A9E028.unk_0000[i].unk_08.y -= this->dyna.actor.home.pos.y;
            D_80A9E028.unk_0000[i].unk_08.z -= this->dyna.actor.home.pos.z;

            D_80A9E028.unk_0000[i].unk_14.x = D_80A9E028.unk_0000[i].unk_08.x;
            D_80A9E028.unk_0000[i].unk_14.y = phi_f0;
            D_80A9E028.unk_0000[i].unk_14.z = D_80A9E028.unk_0000[i].unk_08.z;

            if (D_80A9E028.unk_0000[i].unk_34 == 1) {
                phi_f0 += D_80A9D880[D_80A9E028.unk_0000[i].unk_00].unk_00;
            }
        }
    }

    func_80A9AD18(this, play, &D_80A9E028);
    this->dyna.actor.draw = func_80A9D61C;
    if (this->unk_170 != 0) {
        func_80A9CC84(this);
    } else {
        func_80A9CE00(this);
    }
}

void func_80A9CC84(BgHakuginPost* this) {
    this->actionFunc = func_80A9CCA0;
    this->unk_16C = this->unk_164;
}

void func_80A9CCA0(BgHakuginPost* this, PlayState* play) {
    if (this->unk_174 != 0) {
        func_80A9D1E0(this, func_80A9CD00, (this->unk_164 - this->dyna.actor.home.pos.y) + 100.0f, 60,
                      this->dyna.actor.csId);
    }
}

void func_80A9CD00(BgHakuginPost* this) {
    this->actionFunc = func_80A9CD14;
}

void func_80A9CD14(BgHakuginPost* this, PlayState* play) {
    f32 temp_f12 =
        Math_SinS((this->unk_16C - this->unk_164) * 0x8000 / (this->unk_168 - this->unk_164)) * 140.0f + 4.0f;

    temp_f12 = CLAMP_MAX(temp_f12, 40.0f);
    this->unk_16C += temp_f12;
    if (this->unk_168 <= this->unk_16C) {
        BgHakuginPost_RequestQuakeAndRumble(this, play);
        Audio_PlaySfx_2(NA_SE_EV_STONEDOOR_STOP);
        func_80A9CE00(this);
    } else {
        Actor_PlaySfx_FlaggedCentered2(&this->dyna.actor, NA_SE_EV_ICE_PILLAR_RISING - SFX_FLAG);
    }
}

void func_80A9CE00(BgHakuginPost* this) {
    this->actionFunc = func_80A9CE1C;
    this->unk_16C = this->unk_168;
}

void func_80A9CE1C(BgHakuginPost* this, PlayState* play) {
    s32 pad;
    Player* player = GET_PLAYER(play);
    ColliderCylinder* collider;
    s32 yDiff;
    s32 i;
    s16 temp;
    f32 temp_f0;
    f32 temp_f2;

    if (this->unk_170 != 0) {
        func_80A9D1E0(this, func_80A9D0A0, (this->unk_164 - this->dyna.actor.home.pos.y) + 100.0f, 0x3C,
                      CutsceneManager_GetAdditionalCsId(this->dyna.actor.csId));
        return;
    }

    for (i = 0; i < D_80A9E028.count; i++) {
        collider = D_80A9E028.unk_0000[i].collider;
        if ((collider != NULL) && (collider->base.acFlags & AC_HIT) && (D_80A9E028.unk_0000[i].unk_34 == 1)) {
            temp_f2 = this->unk_16C;
            yDiff = ABS_ALT(BINANG_SUB(this->dyna.actor.yawTowardsPlayer, player->actor.shape.rot.y));
            temp_f0 = temp_f2 + D_80A9E028.unk_0000[i].unk_14.y;
            if ((yDiff > 0x5000) && ((temp_f0 - 60.0f) < player->actor.world.pos.y) &&
                (player->actor.world.pos.y < ((temp_f0 + D_80A9D880[D_80A9E028.unk_0000[i].unk_00].unk_00) - 70.0f))) {
                D_80A9E028.unk_0000[i].unk_20 = 20.0f;
                temp = (s16)(this->dyna.actor.yawTowardsPlayer + 0x58F0);
                D_80A9E028.unk_0000[i].unk_28 = ((s16)(player->actor.shape.rot.y - temp) / 3) + temp;
                D_80A9E028.unk_0000[i].unk_34 = 2;
                Player_PlaySfx(player, NA_SE_IT_HAMMER_HIT);
                Audio_PlaySfx_2(NA_SE_EV_SLIDE_DOOR_OPEN);
                Flags_SetSwitch(play, D_80A9E028.unk_0000[i].switchFlag);
                this->unk_178 = 20;
                func_80A9D2C4(this, func_80A9CE00, D_80A9E028.unk_0000[i].unk_14.y + 50.0f, D_80A9E028.unk_0000[i].csId,
                              D_80A9E028.unk_0000[i].additionalCsId);
                break;
            }
        }
    }

    for (i = 0; i < D_80A9E028.count; i++) {
        collider = D_80A9E028.unk_0000[i].collider;
        if (collider != NULL) {
            collider->base.acFlags &= ~AC_HIT;
        }
    }

    if (this->unk_178 <= 0) {
        func_80A9AEB8(this, play, &D_80A9E028);
    }
}

void func_80A9D0A0(BgHakuginPost* this) {
    this->actionFunc = func_80A9D0B4;
}

void func_80A9D0B4(BgHakuginPost* this, PlayState* play) {
    f32 sp24 = (this->unk_164 + this->unk_160) - func_80A9B2B8(&D_80A9E028);
    f32 temp_f14 = Math_SinS((this->unk_16C - sp24) * 0x8000 / (this->unk_168 - sp24)) * 140.0f + 4.0f;

    temp_f14 = CLAMP_MAX(temp_f14, 40.0f);
    this->unk_16C -= temp_f14;
    if (this->unk_16C <= sp24) {
        func_80A9C634(this, play);
        func_80A9B160(&D_80A9E028, play);
        this->unk_16C = this->unk_164;
        BgHakuginPost_RequestQuakeAndRumble(this, play);
        Audio_PlaySfx_2(NA_SE_EV_STONEDOOR_STOP);
        func_80A9CC84(this);
    } else {
        Actor_PlaySfx_FlaggedCentered2(&this->dyna.actor, NA_SE_EV_ICE_PILLAR_FALL - SFX_FLAG);
    }
}

void func_80A9D1E0(BgHakuginPost* this, BgHakuginPostFunc unkFunc, f32 arg2, s16 csLength, s16 csId) {
    this->unkFunc = unkFunc;
    this->csId = csId;
    this->csLength = CutsceneManager_GetLength(csId);
    if (this->csLength < 0) {
        this->csLength = csLength;
    }
    this->dyna.actor.focus.pos.x = this->dyna.actor.home.pos.x;
    this->dyna.actor.focus.pos.y = this->unk_16C + arg2;
    this->dyna.actor.focus.pos.z = this->dyna.actor.home.pos.z;
    this->actionFunc = func_80A9D260;
}

void func_80A9D260(BgHakuginPost* this, PlayState* play) {
    if (CutsceneManager_IsNext(this->csId)) {
        CutsceneManager_StartWithPlayerCs(this->csId, &this->dyna.actor);
        this->unkFunc(this);
    } else {
        CutsceneManager_Queue(this->csId);
    }
}

void func_80A9D2C4(BgHakuginPost* this, BgHakuginPostFunc unkFunc, f32 arg2, s16 csId, s16 additionalCsId) {
    this->unkFunc = unkFunc;
    this->csId = csId;
    this->additionalCsId = additionalCsId;
    this->csLength = -1;
    this->csLength = CutsceneManager_GetLength(csId);
    if (this->csLength < 0) {
        this->csLength = 15;
    }
    this->dyna.actor.focus.pos.x = this->dyna.actor.home.pos.x;
    this->dyna.actor.focus.pos.y = this->unk_16C + arg2;
    this->dyna.actor.focus.pos.z = this->dyna.actor.home.pos.z;
    this->actionFunc = func_80A9D360;
}

void func_80A9D360(BgHakuginPost* this, PlayState* play) {
    if (CutsceneManager_IsNext(this->csId)) {
        CutsceneManager_StartWithPlayerCs(this->csId, &this->dyna.actor);
        if (this->additionalCsId > CS_ID_NONE) {
            func_80A9D3E4(this);
        } else {
            this->unkFunc(this);
        }
    } else {
        CutsceneManager_Queue(this->csId);
    }
}

void func_80A9D3E4(BgHakuginPost* this) {
    this->additionalCsLength = CutsceneManager_GetLength(this->additionalCsId);
    if (this->additionalCsLength < 0) {
        this->additionalCsLength = 30;
    }
    this->actionFunc = func_80A9D434;
}

void func_80A9D434(BgHakuginPost* this, PlayState* play) {
    if (CutsceneManager_IsNext(this->additionalCsId)) {
        CutsceneManager_StartWithPlayerCs(this->additionalCsId, &this->dyna.actor);
        this->unkFunc(this);
    } else {
        CutsceneManager_Queue(this->additionalCsId);
    }
}

void BgHakuginPost_Update(Actor* thisx, PlayState* play) {
    BgHakuginPost* this = (BgHakuginPost*)thisx;
    f32 temp;

    func_80A9B46C(this, play);
    if ((this->csLength >= 0) && ((this->actionFunc != func_80A9D260) || (this->actionFunc != func_80A9D360))) {
        this->csLength--;
        if (this->csLength < 0) {
            CutsceneManager_Stop(this->csId);
        }
    }

    if ((this->additionalCsLength >= 0) && (this->actionFunc != func_80A9D434)) {
        this->additionalCsLength--;
        if (this->additionalCsLength < 0) {
            CutsceneManager_Stop(this->additionalCsId);
        }
    }

    if (this->unk_178 > 0) {
        this->unk_178--;
    }

    if (this->unk_179 > 0) {
        this->unk_179--;
    }

    func_80A9BC0C(&D_80A9E028);
    this->actionFunc(this, play);
    func_80A9BD24(this, play, &D_80A9E028);
    func_80A9C058(this, play, &D_80A9E028);
    func_80A9C228(this, play, &D_80A9E028);
    temp = func_80A9B2B8(&D_80A9E028);
    this->dyna.actor.scale.y = temp + this->unk_16C - this->dyna.actor.home.pos.y;
    this->dyna.actor.scale.y *= 0.0025f;
    if (thisx->scale.y < 0.0001f) {
        thisx->scale.y = 0.0001f;
    }

    func_80A9C854(this, play);
}

void func_80A9D61C(Actor* thisx, PlayState* play) {
    static Gfx* D_80A9D900[] = {
        object_hakugin_obj_DL_00C1A8, object_hakugin_obj_DL_00C568, NULL, NULL,
        object_hakugin_obj_DL_00CA38, object_hakugin_obj_DL_00CEC8, NULL,
    };
    static Gfx* D_80A9D91C[] = {
        object_hakugin_obj_DL_00D098,
        object_hakugin_obj_DL_00D098,
        object_hakugin_obj_DL_00D098,
        object_hakugin_obj_DL_00D098,
    };
    BgHakuginPost* this = (BgHakuginPost*)thisx;
    BgHakuginPostUnkStruct1* unkStruct1;
    BgHakuginPostUnkStruct2* unkStruct2;
    Vec3f sp68;
    s32 i;

    OPEN_DISPS(play->state.gfxCtx);

    Gfx_SetupDL25_Opa(play->state.gfxCtx);
    Matrix_SetTranslateRotateYXZ(this->dyna.actor.world.pos.x, this->dyna.actor.world.pos.y,
                                 this->dyna.actor.world.pos.z, &this->dyna.actor.shape.rot);
    Matrix_Scale(0.1f, 0.1f, 0.1f, MTXMODE_APPLY);

    for (i = 0; i < D_80A9E028.count; i++) {
        unkStruct1 = &D_80A9E028.unk_0000[i];
        if ((unkStruct1->unk_34 != 5) && (unkStruct1->unk_34 != 4)) {
            sp68.x = unkStruct1->unk_14.x + this->dyna.actor.home.pos.x;
            sp68.y = unkStruct1->unk_14.y + this->unk_16C;
            sp68.z = unkStruct1->unk_14.z + this->dyna.actor.home.pos.z;
            func_80A9B384(&sp68);

            MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx);
            gSPDisplayList(POLY_OPA_DISP++, D_80A9D900[unkStruct1->unk_00]);
        }
    }

    if (D_80A9E028.unk_1924) {
        for (i = 0; i < ARRAY_COUNT(D_80A9E028.unk_02A4); i++) {
            unkStruct2 = &D_80A9E028.unk_02A4[i];
            if (unkStruct2->unk_2C > 0) {
                Matrix_SetTranslateRotateYXZ(unkStruct2->unk_04.x, unkStruct2->unk_04.y, unkStruct2->unk_04.z,
                                             &unkStruct2->unk_20);
                Matrix_Scale(unkStruct2->unk_00, unkStruct2->unk_00, unkStruct2->unk_00, MTXMODE_APPLY);

                MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx);
                gSPDisplayList(POLY_OPA_DISP++, D_80A9D91C[unkStruct2->unk_2D]);
            }
        }
    }

    CLOSE_DISPS(play->state.gfxCtx);
}

void BgHakuginPost_Reset(void) {
    D_80A9D8FC = 1;
}
