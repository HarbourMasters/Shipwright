/*
 * File: z_bg_ikana_rotaryroom.c
 * Overlay: ovl_Bg_Ikana_Rotaryroom
 * Description: Stone Tower Temple - Rotating Room
 */

#include "z_bg_ikana_rotaryroom.h"
#include "z64quake.h"
#include "overlays/actors/ovl_Bg_Ikana_Block/z_bg_ikana_block.h"
#include "overlays/actors/ovl_En_Torch2/z_en_torch2.h"
#include "overlays/actors/ovl_En_Water_Effect/z_en_water_effect.h"
#include "objects/object_ikana_obj/object_ikana_obj.h"

#define FLAGS (ACTOR_FLAG_UPDATE_CULLING_DISABLED | ACTOR_FLAG_DRAW_CULLING_DISABLED)

void BgIkanaRotaryroom_Init(Actor* thisx, PlayState* play);
void BgIkanaRotaryroom_Destroy(Actor* thisx, PlayState* play);
void BgIkanaRotaryroom_Update(Actor* thisx, PlayState* play);
void BgIkanaRotaryroom_Draw(Actor* thisx, PlayState* play);

void func_80B814B8(BgIkanaRotaryroom* this, PlayState* play);
void func_80B818B4(BgIkanaRotaryroom* this);
void func_80B818C8(Actor* thisx, PlayState* play);
void func_80B81978(BgIkanaRotaryroom* this);
void func_80B8198C(Actor* thisx, PlayState* play);
void func_80B819DC(BgIkanaRotaryroom* this);
void func_80B819F0(Actor* thisx, PlayState* play);
void func_80B81A64(BgIkanaRotaryroom* this);
void func_80B81A80(Actor* thisx, PlayState* play);
void func_80B81B84(BgIkanaRotaryroom* this);
void func_80B81BA0(Actor* thisx, PlayState* play);
void func_80B81DAC(BgIkanaRotaryroom* this);
void func_80B81DC8(Actor* thisx, PlayState* play);

ActorProfile Bg_Ikana_Rotaryroom_Profile = {
    /**/ ACTOR_BG_IKANA_ROTARYROOM,
    /**/ ACTORCAT_BG,
    /**/ FLAGS,
    /**/ OBJECT_IKANA_OBJ,
    /**/ sizeof(BgIkanaRotaryroom),
    /**/ BgIkanaRotaryroom_Init,
    /**/ BgIkanaRotaryroom_Destroy,
    /**/ BgIkanaRotaryroom_Update,
    /**/ BgIkanaRotaryroom_Draw,
};

static ColliderJntSphElementInit sJntSphElementsInit1[2] = {
    {
        {
            ELEM_MATERIAL_UNK4,
            { 0x00000000, 0x00, 0x00 },
            { 0x00002000, 0x00, 0x00 },
            ATELEM_NONE | ATELEM_SFX_NORMAL,
            ACELEM_ON,
            OCELEM_NONE,
        },
        { 0, { { -3450, 450, 0 }, 60 }, 100 },
    },
    {
        {
            ELEM_MATERIAL_UNK4,
            { 0x00000000, 0x00, 0x00 },
            { 0x00002000, 0x00, 0x00 },
            ATELEM_NONE | ATELEM_SFX_NORMAL,
            ACELEM_ON,
            OCELEM_NONE,
        },
        { 0, { { 3450, 450, 0 }, 60 }, 100 },
    },
};

static ColliderJntSphInit sJntSphInit1 = {
    {
        COL_MATERIAL_NONE,
        AT_NONE,
        AC_ON | AC_TYPE_PLAYER,
        OC1_NONE,
        OC2_NONE,
        COLSHAPE_JNTSPH,
    },
    ARRAY_COUNT(sJntSphElementsInit1),
    sJntSphElementsInit1,
};

static ColliderJntSphElementInit sJntSphElementsInit2[1] = {
    {
        {
            ELEM_MATERIAL_UNK4,
            { 0x00000000, 0x00, 0x00 },
            { 0x00002000, 0x00, 0x00 },
            ATELEM_NONE | ATELEM_SFX_NORMAL,
            ACELEM_ON,
            OCELEM_NONE,
        },
        { 0, { { -3750, 1200, 0 }, 60 }, 100 },
    },
};

static ColliderJntSphInit sJntSphInit2 = {
    {
        COL_MATERIAL_NONE,
        AT_NONE,
        AC_ON | AC_TYPE_PLAYER,
        OC1_NONE,
        OC2_NONE,
        COLSHAPE_JNTSPH,
    },
    ARRAY_COUNT(sJntSphElementsInit2),
    sJntSphElementsInit2,
};

Vec3f D_80B8216C = { 225.0f, -280.0f, -210.0f };

Vec3f D_80B82178 = { -255.0f, -280.0f, 210.0f };

typedef struct {
    /* 0x0 */ s8 unk_00;
    /* 0x1 */ s8 unk_01;
    /* 0x4 */ Vec3f unk_04;
} BgIkanaRotaryroomStruct4; // size = 0x10

BgIkanaRotaryroomStruct4 D_80B82184[2][2] = {
    { { 0, 0, { -375.0f, -170.0f, 0.0f } }, { 0, 0, { 375.0f, -170.0f, 0.0f } } },
    { { 0, 0, { -435.0f, -180.0f, 0.0f } }, { 0, 1, { 435.0f, -180.0f, 0.0f } } },
};

Vec3s D_80B821C4[] = {
    { -150, 194, -120 }, { -120, 194, -30 }, { -150, 194, 120 }, { 120, 194, 120 }, { 180, 194, 30 },
    { 150, 194, -90 },   { 60, 194, -120 },  { -60, 194, 90 },   { 30, 194, 120 },  { 49, 194, -105 },
};

f32 D_80B82200[] = { 64.0f, 66.0f, 66.0f, 30.0f, 41.0f };

static InitChainEntry sInitChain[] = {
    ICHAIN_VEC3F_DIV1000(scale, 100, ICHAIN_STOP),
};

CollisionHeader* D_80B82218[] = { &object_ikana_obj_Colheader_006368, &object_ikana_obj_Colheader_0084F8 };

void func_80B802E0(BgIkanaRotaryroom* this) {
    s32 pad;

    Matrix_SetTranslateRotateYXZ(this->dyna.actor.world.pos.x, this->dyna.actor.world.pos.y,
                                 this->dyna.actor.world.pos.z, &this->dyna.actor.shape.rot);
    Matrix_Scale(this->dyna.actor.scale.x, this->dyna.actor.scale.y, this->dyna.actor.scale.z, MTXMODE_APPLY);

    Collider_UpdateSpheres(0, &this->collider);
    if (!BGIKANAROTARYROOM_GET_1(&this->dyna.actor)) {
        Collider_UpdateSpheres(1, &this->collider);
    }
}

void func_80B80358(Vec3f* arg0) {
    arg0->x += (arg0->x >= 0.0f) ? 0.5f : -0.5f;
    arg0->y += (arg0->y >= 0.0f) ? 0.5f : -0.5f;
    arg0->z += (arg0->z >= 0.0f) ? 0.5f : -0.5f;

    arg0->x = (s32)arg0->x;
    arg0->y = (s32)arg0->y;
    arg0->z = (s32)arg0->z;
}

void func_80B80440(BgIkanaRotaryroom* this, PlayState* play) {
    s32 pad;
    Vec3f sp50;

    Matrix_Push();
    Matrix_SetTranslateRotateYXZ(this->dyna.actor.world.pos.x, this->dyna.actor.world.pos.y,
                                 this->dyna.actor.world.pos.z, &this->dyna.actor.shape.rot);
    if (Flags_GetSwitch(play, BGIKANAROTARYROOM_GET_SWITCH_FLAG_2(&this->dyna.actor))) {
        Matrix_Translate(D_80B82178.x, D_80B82178.y, D_80B82178.z, MTXMODE_APPLY);
    } else {
        Matrix_Translate(D_80B8216C.x, D_80B8216C.y, D_80B8216C.z, MTXMODE_APPLY);
    }
    Matrix_MultZero(&sp50);
    func_80B80358(&sp50);
    this->unk_204.unk_00 = Actor_SpawnAsChildAndCutscene(
        &play->actorCtx, play, ACTOR_BG_IKANA_BLOCK, sp50.x, sp50.y, sp50.z, this->dyna.actor.shape.rot.x,
        this->dyna.actor.shape.rot.y, this->dyna.actor.shape.rot.z, -1,
        CutsceneManager_GetAdditionalCsId(this->dyna.actor.csId), this->dyna.actor.halfDaysBits, NULL);
    Matrix_Pop();
}

void func_80B80550(BgIkanaRotaryroom* this, PlayState* play) {
    s32 pad;
    s32 spC8 = BGIKANAROTARYROOM_GET_1(&this->dyna.actor);
    s32 spC4;
    s32 pad2;
    Vec3f spB4;
    Actor* actor;
    s32 i;
    BgIkanaRotaryroomStruct4* ptr;
    MtxF sp68;
    Vec3s sp60;

    if (Flags_GetSwitch(play, BGIKANAROTARYROOM_GET_SWITCH_FLAG_1(&this->dyna.actor))) {
        spC4 = 1;
    } else {
        spC4 = 0;
    }

    actor = play->actorCtx.actorLists[ACTORCAT_DOOR].first;
    Matrix_Push();
    sp60.x = BINANG_ROT180(this->dyna.actor.home.rot.x);
    sp60.y = this->dyna.actor.home.rot.y;
    sp60.z = this->dyna.actor.home.rot.z;
    Matrix_SetTranslateRotateYXZ(this->dyna.actor.world.pos.x, this->dyna.actor.world.pos.y,
                                 this->dyna.actor.world.pos.z, &sp60);

    while (actor != NULL) {
        if (actor->id == ACTOR_DOOR_SHUTTER) {
            Math_Vec3f_Diff(&actor->world.pos, &this->dyna.actor.world.pos, &spB4);

            for (i = 0; i < ARRAY_COUNT(D_80B82184); i++) {
                ptr = &D_80B82184[spC8][i];

                if ((Math3D_Vec3fDistSq(&spB4, &D_80B82184[spC8][i].unk_04) < SQ(250.0f)) &&
                    ((ptr->unk_00 ^ spC4) && (ptr->unk_01 == 0))) {
                    Matrix_Push();
                    Matrix_Translate(spB4.x, spB4.y, spB4.z, MTXMODE_APPLY);
                    Matrix_RotateYS(actor->shape.rot.y - this->dyna.actor.home.rot.y, MTXMODE_APPLY);
                    Matrix_RotateXS(actor->shape.rot.x - this->dyna.actor.home.rot.x, MTXMODE_APPLY);
                    Matrix_RotateZS(actor->shape.rot.z - this->dyna.actor.home.rot.z, MTXMODE_APPLY);
                    Matrix_MultZero(&actor->world.pos);
                    func_80B80358(&actor->world.pos);
                    Matrix_Get(&sp68);
                    Matrix_MtxFToYXZRot(&sp68, &actor->shape.rot, false);
                    Matrix_Pop();
                }
            }
        }
        actor = actor->next;
    }

    Matrix_Pop();
}

BgIkanaRotaryroomStruct4* func_80B80778(BgIkanaRotaryroom* this, PlayState* play, Actor* arg2) {
    s32 i;
    Vec3f sp68;
    s32 pad[2];
    s32 temp_s3 = BGIKANAROTARYROOM_GET_1(&this->dyna.actor);
    BgIkanaRotaryroomStruct4* sp58 = NULL;
    BgIkanaRotaryroomStruct4* ptr;

    if (1) {}

    if (arg2->id == ACTOR_DOOR_SHUTTER) {
        Matrix_Push();

        for (i = 0; i < ARRAY_COUNT(D_80B82184); i++) {
            ptr = &D_80B82184[temp_s3][i];
            if ((ptr->unk_01 == 0) &&
                (Matrix_SetTranslateRotateYXZ(this->dyna.actor.world.pos.x, this->dyna.actor.world.pos.y,
                                              this->dyna.actor.world.pos.z, &this->dyna.actor.shape.rot),
                 Matrix_MultVec3f(&ptr->unk_04, &sp68), (Math3D_Vec3fDistSq(&arg2->world.pos, &sp68) < SQ(250.0f)))) {
                sp58 = ptr;
                break;
            }
        }

        Matrix_Pop();
    }
    return sp58;
}

void func_80B80894(BgIkanaRotaryroom* this, PlayState* play) {
    s32 pad[7];
    Actor* actor = this->unk_204.unk_00;
    s32 i;

    Matrix_Push();

    Matrix_RotateZS(-this->dyna.actor.shape.rot.z, MTXMODE_NEW);
    Matrix_RotateXS(-this->dyna.actor.shape.rot.x, MTXMODE_APPLY);
    Matrix_RotateYS(-this->dyna.actor.shape.rot.y, MTXMODE_APPLY);
    Matrix_Translate(-this->dyna.actor.world.pos.x, -this->dyna.actor.world.pos.y, -this->dyna.actor.world.pos.z,
                     MTXMODE_APPLY);

    if (actor != NULL) {
        Matrix_Push();

        Matrix_Translate(actor->world.pos.x, actor->world.pos.y, actor->world.pos.z, MTXMODE_APPLY);
        Matrix_RotateYS(actor->shape.rot.y, MTXMODE_APPLY);
        Matrix_RotateXS(actor->shape.rot.x, MTXMODE_APPLY);
        Matrix_RotateZS(actor->shape.rot.z, MTXMODE_APPLY);
        Matrix_Get(&this->unk_204.unk_04);

        Matrix_Pop();
    }

    for (i = 0; i < ARRAY_COUNT(this->unk_248); i++) {
        this->unk_248[i].unk_00 = NULL;
    }

    actor = play->actorCtx.actorLists[ACTORCAT_DOOR].first;
    i = 0;
    while (actor != NULL) {
        if (func_80B80778(this, play, actor) && (i < 2)) {
            this->unk_248[i].unk_00 = actor;

            Matrix_Push();

            Matrix_Translate(actor->world.pos.x, actor->world.pos.y, actor->world.pos.z, MTXMODE_APPLY);
            Matrix_RotateYS(actor->shape.rot.y, MTXMODE_APPLY);
            Matrix_RotateXS(actor->shape.rot.x, MTXMODE_APPLY);
            Matrix_RotateZS(actor->shape.rot.z, MTXMODE_APPLY);
            Matrix_Get(&this->unk_248[i].unk_04);

            Matrix_Pop();
            i++;
        }
        actor = actor->next;
    }

    for (i = 0; i < ARRAY_COUNT(this->unk_2D0); i++) {
        this->unk_2D0[i].unk_00 = NULL;
    }

    actor = play->actorCtx.actorLists[ACTORCAT_ITEMACTION].first;
    i = 0;
    while (actor != NULL) {
        if ((actor->id == ACTOR_EN_TORCH2) && (actor->update != NULL) && (i < ARRAY_COUNT(this->unk_2D0))) {
            this->unk_2D0[i].unk_00 = actor;

            Matrix_Push();

            Matrix_Translate(actor->world.pos.x, actor->world.pos.y, actor->world.pos.z, MTXMODE_APPLY);
            Matrix_RotateYS(actor->shape.rot.y, MTXMODE_APPLY);
            Matrix_RotateXS(actor->shape.rot.x, MTXMODE_APPLY);
            Matrix_RotateZS(actor->shape.rot.z, MTXMODE_APPLY);
            Matrix_Get(&this->unk_2D0[i].unk_04);

            Matrix_Pop();
            i++;
        }
        actor = actor->next;
    }

    for (i = 0; i < ARRAY_COUNT(this->unk_3E0); i++) {
        this->unk_3E0[i].unk_00 = NULL;
    }

    actor = play->actorCtx.actorLists[ACTORCAT_ENEMY].first;
    i = 0;
    while (actor != NULL) {
        if ((actor->update != NULL) && (actor->id != ACTOR_EN_WATER_EFFECT) && (i < ARRAY_COUNT(this->unk_3E0))) {
            this->unk_3E0[i].unk_00 = actor;
            this->unk_3E0[i].unk_44 = actor->shape.rot;
            this->unk_3E0[i].unk_4C = 0.0f;

            Matrix_Push();

            Matrix_Translate(actor->world.pos.x, actor->world.pos.y, actor->world.pos.z, MTXMODE_APPLY);
            Matrix_RotateYS(actor->shape.rot.y, MTXMODE_APPLY);
            Matrix_RotateXS(actor->shape.rot.x, MTXMODE_APPLY);
            Matrix_RotateZS(actor->shape.rot.z, MTXMODE_APPLY);
            Matrix_Get(&this->unk_3E0[i].unk_04);

            Matrix_Pop();
            i++;
        }
        actor = actor->next;
    }

    actor = &GET_PLAYER(play)->actor;
    if (actor->update != NULL) {
        this->unk_520.unk_40 = actor->shape.rot;

        Matrix_Push();

        Matrix_Translate(actor->world.pos.x, actor->world.pos.y, actor->world.pos.z, MTXMODE_APPLY);
        Matrix_RotateYS(actor->shape.rot.y, MTXMODE_APPLY);
        Matrix_RotateXS(actor->shape.rot.x, MTXMODE_APPLY);
        Matrix_RotateZS(actor->shape.rot.z, MTXMODE_APPLY);
        Matrix_Get(&this->unk_520.unk_00);

        Matrix_Pop();
    }

    Matrix_Pop();
}

void func_80B80C88(BgIkanaRotaryroom* this, PlayState* play) {
    s32 pad[5];
    BgIkanaBlock* ikanaBlock = (BgIkanaBlock*)this->unk_204.unk_00;
    Player* player;
    Actor* ptr;
    Actor* ptr2;
    Actor* ptr3;
    s32 i;
    MtxF sp3C;

    Matrix_Push();
    Matrix_SetTranslateRotateYXZ(this->dyna.actor.world.pos.x, this->dyna.actor.world.pos.y,
                                 this->dyna.actor.world.pos.z, &this->dyna.actor.shape.rot);

    if (ikanaBlock != NULL) {
        Matrix_Push();

        Matrix_Mult(&this->unk_204.unk_04, MTXMODE_APPLY);
        Matrix_MultZero(&ikanaBlock->dyna.actor.world.pos);
        func_80B80358(&ikanaBlock->dyna.actor.world.pos);
        Matrix_Get(&sp3C);
        Matrix_MtxFToYXZRot(&sp3C, &ikanaBlock->dyna.actor.shape.rot, false);

        Matrix_Pop();
    }

    for (i = 0; i < ARRAY_COUNT(this->unk_248); i++) {
        ptr = this->unk_248[i].unk_00;
        if (ptr != NULL) {
            Matrix_Push();

            Matrix_Mult(&this->unk_248[i].unk_04, MTXMODE_APPLY);
            Matrix_MultZero(&ptr->world.pos);
            func_80B80358(&ptr->world.pos);
            Matrix_Get(&sp3C);
            Matrix_MtxFToYXZRot(&sp3C, &ptr->shape.rot, false);

            Matrix_Pop();
        }
    }

    for (i = 0; i < ARRAY_COUNT(this->unk_2D0); i++) {
        ptr2 = this->unk_2D0[i].unk_00;
        if (ptr2 != NULL) {
            Matrix_Push();

            Matrix_Mult(&this->unk_2D0[i].unk_04, MTXMODE_APPLY);
            Matrix_MultZero(&ptr2->world.pos);
            func_80B80358(&ptr2->world.pos);
            Matrix_Get(&sp3C);
            Matrix_MtxFToYXZRot(&sp3C, &ptr2->shape.rot, false);

            Matrix_Pop();
        }
    }

    for (i = 0; i < ARRAY_COUNT(this->unk_3E0); i++) {
        ptr3 = this->unk_3E0[i].unk_00;
        if (ptr3 != NULL) {
            Matrix_Push();

            Matrix_Mult(&this->unk_3E0[i].unk_04, MTXMODE_APPLY);
            Matrix_MultZero(&ptr3->world.pos);
            func_80B80358(&ptr3->world.pos);
            Matrix_Get(&sp3C);
            Matrix_MtxFToYXZRot(&sp3C, &ptr3->shape.rot, false);

            Matrix_Pop();
        }
    }

    player = GET_PLAYER(play);
    if (player != NULL) {
        Matrix_Push();

        Matrix_Mult(&this->unk_520.unk_00, MTXMODE_APPLY);
        Matrix_MultZero(&player->actor.world.pos);
        Math_Vec3f_Copy(&player->actor.home.pos, &player->actor.world.pos);
        func_80B80358(&player->actor.world.pos);
        Matrix_Get(&sp3C);
        Matrix_MtxFToYXZRot(&sp3C, &player->actor.shape.rot, false);

        Matrix_Pop();
        player->actor.freezeTimer = 2;
    }

    Matrix_Pop();
}

s32 func_80B80F08(BgIkanaRotaryroom* this, PlayState* play) {
    s32 pad;
    BgIkanaBlock* ikanaBlock = (BgIkanaBlock*)this->unk_204.unk_00;
    Vec3f sp34;
    Vec3f sp28;
    s32 sp24 = false;

    if (ikanaBlock != NULL) {
        Matrix_Push();

        Matrix_SetTranslateRotateYXZ(this->dyna.actor.world.pos.x, this->dyna.actor.world.pos.y,
                                     this->dyna.actor.world.pos.z, &this->dyna.actor.shape.rot);
        Matrix_Translate(D_80B82178.x, D_80B82178.y, D_80B82178.z, MTXMODE_APPLY);
        Matrix_MultZero(&sp34);
        Matrix_SetTranslateRotateYXZ(ikanaBlock->dyna.actor.world.pos.x,
                                     ikanaBlock->dyna.actor.world.pos.y + ikanaBlock->unk_170,
                                     ikanaBlock->dyna.actor.world.pos.z, &ikanaBlock->dyna.actor.shape.rot);
        Matrix_MultZero(&sp28);

        Matrix_Pop();

        if (Math3D_Vec3fDistSq(&sp34, &sp28) < 3.0f) {
            if (!Flags_GetSwitch(play, BGIKANAROTARYROOM_GET_SWITCH_FLAG_2(&this->dyna.actor))) {
                sp24 = true;
            }
            Flags_SetSwitch(play, BGIKANAROTARYROOM_GET_SWITCH_FLAG_2(&this->dyna.actor));
        }
    }
    return sp24;
}

void func_80B81010(BgIkanaRotaryroom* this, PlayState* play) {
    s32 pad;
    f32 temp_f0;
    s32 temp_s2;
    s32 phi_s7;
    s32 i;
    Actor* ptr;
    Vec3f sp84;
    BgIkanaRotaryroomStruct2* ptr2;
    CollisionPoly* sp7C;
    s32 bgId;

    if (CutsceneManager_GetCurrentCsId() == this->dyna.actor.csId) {
        phi_s7 = true;

        for (i = 0; i < ARRAY_COUNT(this->unk_3E0); i++) {
            ptr = this->unk_3E0[i].unk_00;
            ptr2 = &this->unk_3E0[i];

            if (ptr != NULL) {
                temp_s2 = Math_ScaledStepToS(&ptr->shape.rot.x, ptr2->unk_44.x, 0xBB8) & 1;
                temp_s2 &= Math_ScaledStepToS(&ptr->shape.rot.y, ptr2->unk_44.y, 0xBB8);
                temp_s2 &= Math_ScaledStepToS(&ptr->shape.rot.z, ptr2->unk_44.z, 0xBB8);

                ptr2->unk_4C -= 2.0f;
                if (ptr2->unk_4C < -30.0f) {
                    ptr2->unk_4C = -30.0f;
                }

                Math_Vec3f_Copy(&ptr->prevPos, &ptr->world.pos);

                ptr->world.pos.y += ptr2->unk_4C;

                sp84.x = ptr->prevPos.x;
                sp84.y = ptr->prevPos.y + 50.0f;
                sp84.z = ptr->prevPos.z;

                temp_f0 = BgCheck_EntityRaycastFloor5_2(play, &play->colCtx, &sp7C, &bgId, NULL, &sp84);
                if (ptr->world.pos.y <= temp_f0) {
                    ptr->world.pos.y = temp_f0;
                } else {
                    temp_s2 = false;
                }

                if (temp_s2) {
                    ptr2->unk_00 = NULL;
                } else {
                    phi_s7 = false;
                }
            }
        }

        if (phi_s7) {
            this->unk_578 = NULL;
        }
    } else {
        for (i = 0; i < ARRAY_COUNT(this->unk_3E0); i++) {
            ptr = this->unk_3E0[i].unk_00;
            if (ptr != NULL) {
                ptr->shape.rot = this->unk_3E0[i].unk_44;
                Math_Vec3f_Copy(&ptr->prevPos, &ptr->world.pos);
            }
        }
        this->unk_578 = NULL;
    }
}

void func_80B81234(BgIkanaRotaryroom* this, PlayState* play) {
    s32 pad[2];
    Player* player = GET_PLAYER(play);
    BgIkanaRotaryroomStruct3* ptr;
    s32 sp64;
    Vec3f sp58;
    Vec3f sp4C;
    f32 temp_f0;
    f32 sp44 = D_80B82200[player->transformation] * 0.5f;
    CollisionPoly* sp40;
    s32 bgId;

    if (CutsceneManager_GetCurrentCsId() == this->dyna.actor.csId) {
        if (player == NULL) {
            return;
        }

        ptr = &this->unk_520;

        if (player->actor.freezeTimer > 0) {
            sp58.x = ptr->unk_4C.x;
            sp58.y = ptr->unk_4C.y + 50.0f;
            sp58.z = ptr->unk_4C.z;

            sp64 = Math_ScaledStepToS(&player->actor.shape.rot.x, ptr->unk_40.x, 0x7D0) & 1;
            sp64 &= Math_ScaledStepToS(&player->actor.shape.rot.y, ptr->unk_40.y, 0x7D0);
            sp64 &= Math_ScaledStepToS(&player->actor.shape.rot.z, ptr->unk_40.z, 0x7D0);

            ptr->unk_48 -= 2.1f;
            ptr->unk_48 *= 0.98f;
            if (ptr->unk_48 < -30.0f) {
                ptr->unk_48 = -30.0f;
            }
            ptr->unk_4C.y += ptr->unk_48;

            Matrix_Push();

            Matrix_RotateYS(player->actor.shape.rot.y, MTXMODE_NEW);
            Matrix_RotateXS(player->actor.shape.rot.x, MTXMODE_APPLY);
            Matrix_RotateZS(player->actor.shape.rot.z, MTXMODE_APPLY);
            Matrix_MultVecY(sp44, &sp4C);

            Matrix_Pop();

            temp_f0 = BgCheck_EntityRaycastFloor5_2(play, &play->colCtx, &sp40, &bgId, NULL, &sp58);
            if (ptr->unk_4C.y <= temp_f0) {
                ptr->unk_4C.y = temp_f0;
            } else {
                sp64 = 0;
            }

            Math_Vec3f_Copy(&player->actor.home.pos, &player->actor.world.pos);

            player->actor.world.pos.x = ptr->unk_4C.x - sp4C.x;
            player->actor.world.pos.y = (ptr->unk_4C.y - sp4C.y) + sp44;
            player->actor.world.pos.z = ptr->unk_4C.z - sp4C.z;

            if (sp64 != 0) {
                Math_Vec3f_Copy(&player->actor.home.pos, &player->actor.world.pos);
                this->unk_57C = NULL;
            } else {
                player->actor.freezeTimer = 2;
            }
        }
    } else {
        if (player != NULL) {
            player->actor.shape.rot = this->unk_520.unk_40;
            Math_Vec3f_Copy(&player->actor.home.pos, &player->actor.world.pos);
        }
        this->unk_57C = NULL;
    }
}

void func_80B814B8(BgIkanaRotaryroom* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    if (CutsceneManager_GetCurrentCsId() == this->dyna.actor.csId) {
        if (player->actor.bgCheckFlags & BGCHECKFLAG_CRUSHED) {
            Player_PlaySfx(player, NA_SE_VO_LI_DAMAGE_S + player->ageProperties->voiceSfxIdOffset);
            func_80169EFC(play);
            Player_PlaySfx(player, NA_SE_VO_LI_TAKEN_AWAY + player->ageProperties->voiceSfxIdOffset);
            play->haltAllActors = true;
            Audio_PlaySfx(NA_SE_OC_ABYSS);
            this->actionFunc = NULL;
        }
    } else {
        this->actionFunc = NULL;
    }
}

void func_80B81570(BgIkanaRotaryroom* this, PlayState* play) {
    s32 i;
    Vec3f sp70;
    Vec3f sp64;
    s32 pad;
    s32 pad2;

    Matrix_Push();
    Matrix_RotateYS(this->dyna.actor.shape.rot.y, MTXMODE_NEW);

    for (i = 0; i < ARRAY_COUNT(D_80B821C4); i++) {
        sp64.x = D_80B821C4[i].x;
        sp64.y = D_80B821C4[i].y;
        sp64.z = D_80B821C4[i].z;

        Matrix_MultVec3f(&sp64, &sp70);

        sp70.x += this->dyna.actor.world.pos.x;
        sp70.y += this->dyna.actor.world.pos.y;
        sp70.z += this->dyna.actor.world.pos.z;

        Actor_Spawn(&play->actorCtx, play, ACTOR_EN_WATER_EFFECT, sp70.x, sp70.y, sp70.z, 0, 0, 0,
                    ENWATEREFFECT_TYPE_FALLING_ROCK_SPAWNER);
    }

    Matrix_Pop();
}

s32 func_80B816A4(BgIkanaRotaryroom* this) {
    s32 i;
    s32 count = 0;

    for (i = 0; i < ARRAY_COUNT(this->unk_3E0); i++) {
        if (this->unk_3E0[i].unk_00 != NULL) {
            count++;
        }
    }

    return count;
}

void BgIkanaRotaryroom_Init(Actor* thisx, PlayState* play) {
    s32 pad;
    BgIkanaRotaryroom* this = (BgIkanaRotaryroom*)thisx;
    s32 sp34 = BGIKANAROTARYROOM_GET_1(&this->dyna.actor);

    Actor_ProcessInitChain(&this->dyna.actor, sInitChain);

    if (Flags_GetSwitch(play, BGIKANAROTARYROOM_GET_SWITCH_FLAG_1(&this->dyna.actor))) {
        this->dyna.actor.shape.rot.x = -0x8000;
    } else {
        this->dyna.actor.shape.rot.x = 0;
    }

    DynaPolyActor_Init(&this->dyna, DYNA_TRANSFORM_POS);
    DynaPolyActor_LoadMesh(play, &this->dyna, D_80B82218[sp34]);

    Collider_InitJntSph(play, &this->collider);
    if (!sp34) {
        Collider_SetJntSph(play, &this->collider, &this->dyna.actor, &sJntSphInit1, this->colliderElements);
    } else {
        Collider_SetJntSph(play, &this->collider, &this->dyna.actor, &sJntSphInit2, this->colliderElements);
    }

    func_80B802E0(this);

    if (sp34 == true) {
        this->unk_1FC = Lib_SegmentedToVirtual(&object_ikana_obj_Matanimheader_007B68);
    }

    if (!sp34) {
        func_80B80440(this, play);
    } else {
        func_80B81570(this, play);
    }

    func_80B80550(this, play);
    func_80B818B4(this);
}

void BgIkanaRotaryroom_Destroy(Actor* thisx, PlayState* play) {
    BgIkanaRotaryroom* this = (BgIkanaRotaryroom*)thisx;

    DynaPoly_DeleteBgActor(play, &play->colCtx.dyna, this->dyna.bgId);
    Collider_DestroyJntSph(play, &this->collider);
}

void func_80B818B4(BgIkanaRotaryroom* this) {
    this->unk_200 = func_80B818C8;
}

void func_80B818C8(Actor* thisx, PlayState* play) {
    BgIkanaRotaryroom* this = (BgIkanaRotaryroom*)thisx;
    s32 switchFlag;

    if (this->collider.base.acFlags & AC_HIT) {
        this->collider.base.acFlags &= ~AC_HIT;
        switchFlag = BGIKANAROTARYROOM_GET_SWITCH_FLAG_1(&this->dyna.actor);
        if (Flags_GetSwitch(play, switchFlag)) {
            Flags_UnsetSwitch(play, switchFlag);
        } else {
            Flags_SetSwitch(play, switchFlag);
        }
        func_80B81978(this);
    } else {
        CollisionCheck_SetAC(play, &play->colChkCtx, &this->collider.base);
    }
}

void func_80B81978(BgIkanaRotaryroom* this) {
    this->unk_200 = func_80B8198C;
}

void func_80B8198C(Actor* thisx, PlayState* play) {
    BgIkanaRotaryroom* this = (BgIkanaRotaryroom*)thisx;

    if (this->unk_204.unk_00 == NULL) {
        func_80B819DC(this);
    } else if (((BgIkanaBlock*)this->unk_204.unk_00)->unk_17C & 1) {
        ((BgIkanaBlock*)this->unk_204.unk_00)->unk_17C |= 8;
        func_80B819DC(this);
    }
}

void func_80B819DC(BgIkanaRotaryroom* this) {
    this->unk_200 = func_80B819F0;
}

void func_80B819F0(Actor* thisx, PlayState* play) {
    BgIkanaRotaryroom* this = (BgIkanaRotaryroom*)thisx;

    if (CutsceneManager_IsNext(this->dyna.actor.csId)) {
        CutsceneManager_StartWithPlayerCs(this->dyna.actor.csId, &this->dyna.actor);
        if (this->dyna.actor.csId > CS_ID_NONE) {
            Player_SetCsActionWithHaltedActors(play, &this->dyna.actor, PLAYER_CSACTION_WAIT);
        }
        func_80B81A64(this);
    } else {
        CutsceneManager_Queue(this->dyna.actor.csId);
    }
}

void func_80B81A64(BgIkanaRotaryroom* this) {
    this->unk_584 = 25;
    this->unk_200 = func_80B81A80;
}

void func_80B81A80(Actor* thisx, PlayState* play) {
    BgIkanaRotaryroom* this = (BgIkanaRotaryroom*)thisx;
    s32 pad;
    s32 i;
    BgIkanaRotaryroomStruct1* ptr;

    Actor_PlaySfx_Flagged(thisx, NA_SE_EV_EARTHQUAKE - SFX_FLAG);
    this->unk_584--;

    if (this->unk_584 <= 0) {
        if (1) {}
        if (1) {}
        if (1) {}
        if (1) {}
        func_80B80894(this, play);

        for (i = 0; i < ARRAY_COUNT(this->unk_2D0); i++) {
            ptr = &this->unk_2D0[i];
            if (ptr->unk_00 != NULL) {
                ((EnTorch2*)ptr->unk_00)->state = 3;
            }
        }

        func_80B81B84(this);
    } else if (this->unk_584 == 15) {
        s16 quakeIndex = Quake_Request(GET_ACTIVE_CAM(play), QUAKE_TYPE_3);

        Quake_SetSpeed(quakeIndex, 31536);
        Quake_SetPerturbations(quakeIndex, 6, 0, 100, 0);
        Quake_SetDuration(quakeIndex, 22);
    }
}

void func_80B81B84(BgIkanaRotaryroom* this) {
    this->unk_584 = 0x2001;
    this->unk_200 = func_80B81BA0;
}

void func_80B81BA0(Actor* thisx, PlayState* play) {
    BgIkanaRotaryroom* this = (BgIkanaRotaryroom*)thisx;
    s32 sp30 = 0;
    s32 i;

    Actor_PlaySfx_Flagged(&this->dyna.actor, NA_SE_EV_EARTHQUAKE - SFX_FLAG);

    if (this->unk_584 > 0) {
        this->unk_584--;
    }

    thisx->shape.rot.x += 0x1F4;

    if (!(this->unk_584 & 7)) {
        s16 quakeIndex = Quake_Request(GET_ACTIVE_CAM(play), QUAKE_TYPE_3);

        Quake_SetSpeed(quakeIndex, 31536);
        Quake_SetPerturbations(quakeIndex, (s32)(Rand_ZeroOne() * 2.5f) + 3, 0, 10, 0);
        Quake_SetDuration(quakeIndex, 15);
    }

    if (Flags_GetSwitch(play, BGIKANAROTARYROOM_GET_SWITCH_FLAG_1(&this->dyna.actor))) {
        if (this->dyna.actor.shape.rot.x < 0) {
            this->dyna.actor.shape.rot.x = -0x8000;
            sp30 = 1;
        }
    } else if (this->dyna.actor.shape.rot.x >= 0) {
        this->dyna.actor.shape.rot.x = 0;
        sp30 = 1;
    }

    func_80B80C88(this, play);

    if (sp30 != 0) {
        Player* player = GET_PLAYER(play);
        BgIkanaRotaryroomStruct1* ptr;
        BgIkanaBlock* block = (BgIkanaBlock*)this->unk_204.unk_00;

        if (block != NULL) {
            block->unk_17C &= ~8;
            block->unk_17C |= 4;
        }

        for (i = 0; i < ARRAY_COUNT(this->unk_2D0); i++) {
            ptr = &this->unk_2D0[i];
            if (ptr->unk_00 != NULL) {
                ((EnTorch2*)ptr->unk_00)->state = 4;
            }
        }

        this->unk_520.unk_48 = 0.0f;
        this->unk_520.unk_4C.x = player->actor.world.pos.x;
        this->unk_520.unk_4C.y = player->actor.world.pos.y - D_80B82200[player->transformation];
        this->unk_520.unk_4C.z = player->actor.world.pos.z;

        if (func_80B816A4(this) > 0) {
            this->unk_578 = func_80B81010;
        }

        this->unk_57C = func_80B81234;
        this->actionFunc = func_80B814B8;
        func_80B802E0(this);
        func_80B81DAC(this);
    }
}

void func_80B81DAC(BgIkanaRotaryroom* this) {
    this->unk_584 = 20;
    this->unk_200 = func_80B81DC8;
}

void func_80B81DC8(Actor* thisx, PlayState* play) {
    s32 pad;
    BgIkanaRotaryroom* this = (BgIkanaRotaryroom*)thisx;

    if (this->unk_584 > 10) {
        Actor_PlaySfx_Flagged(&this->dyna.actor, NA_SE_EV_EARTHQUAKE - SFX_FLAG);
    }
    this->unk_584--;

    if (this->unk_584 <= 0) {
        CutsceneManager_Stop(this->dyna.actor.csId);
        func_80B818B4(this);
    } else if (this->unk_584 == 19) {
        s16 quakeIndex = Quake_Request(GET_ACTIVE_CAM(play), QUAKE_TYPE_3);

        Quake_SetSpeed(quakeIndex, 20000);
        Quake_SetPerturbations(quakeIndex, 5, 0, 40, 60);
        Quake_SetDuration(quakeIndex, 17);
    }
}

void BgIkanaRotaryroom_Update(Actor* thisx, PlayState* play) {
    BgIkanaRotaryroom* this = (BgIkanaRotaryroom*)thisx;
    BgIkanaRotaryroomStruct1* ptr;
    BgIkanaRotaryroomStruct1* ptr2;
    BgIkanaRotaryroomStruct2* ptr3;
    s32 i;

    if ((this->unk_204.unk_00 != NULL) && (this->unk_204.unk_00->update == NULL)) {
        this->unk_204.unk_00 = NULL;
    }

    for (i = 0; i < ARRAY_COUNT(this->unk_248); i++) {
        ptr = &this->unk_248[i];
        if ((ptr->unk_00 != NULL) && (ptr->unk_00->update == NULL)) {
            ptr->unk_00 = NULL;
        }
    }

    for (i = 0; i < ARRAY_COUNT(this->unk_2D0); i++) {
        ptr2 = &this->unk_2D0[i];
        if ((ptr2->unk_00 != NULL) && (ptr2->unk_00->update == NULL)) {
            ptr2->unk_00 = NULL;
        }
    }

    for (i = 0; i < ARRAY_COUNT(this->unk_3E0); i++) {
        ptr3 = &this->unk_3E0[i];
        if ((ptr3->unk_00 != NULL) && (ptr3->unk_00->update == NULL)) {
            ptr3->unk_00 = NULL;
        }
    }

    if (func_80B80F08(this, play)) {
        if (this->unk_204.unk_00 != NULL) {
            ((BgIkanaBlock*)this->unk_204.unk_00)->unk_17E = 1;
        }
    }

    this->unk_200(&this->dyna.actor, play);

    if (this->unk_578 != NULL) {
        this->unk_578(this, play);
    }

    if (this->unk_57C != NULL) {
        this->unk_57C(this, play);
    }

    if (this->actionFunc != NULL) {
        this->actionFunc(this, play);
    }
}

void BgIkanaRotaryroom_Draw(Actor* thisx, PlayState* play) {
    BgIkanaRotaryroom* this = (BgIkanaRotaryroom*)thisx;
    s32 param = BGIKANAROTARYROOM_GET_1(&this->dyna.actor);

    if (!param) {
        Gfx_DrawDListOpa(play, object_ikana_obj_DL_0048A0);
        Gfx_DrawDListXlu(play, object_ikana_obj_DL_004710);
    } else {
        AnimatedMat_Draw(play, this->unk_1FC);
        Gfx_DrawDListOpa(play, object_ikana_obj_DL_007448);
        Gfx_DrawDListXlu(play, object_ikana_obj_DL_007360);
    }
}
