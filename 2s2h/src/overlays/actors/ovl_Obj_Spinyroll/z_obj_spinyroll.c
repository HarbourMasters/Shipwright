/*
 * File: z_obj_spinyroll.c
 * Overlay: ovl_Obj_Spinyroll
 * Description: Horizontal Spike-Covered Log
 */

#include "z_obj_spinyroll.h"
#include "overlays/effects/ovl_Effect_Ss_Hitmark/z_eff_ss_hitmark.h"
#include "objects/object_spinyroll/object_spinyroll.h"

#define FLAGS (ACTOR_FLAG_UPDATE_CULLING_DISABLED)

void ObjSpinyroll_Init(Actor* thisx, PlayState* play);
void ObjSpinyroll_Destroy(Actor* thisx, PlayState* play);
void ObjSpinyroll_Update(Actor* thisx, PlayState* play2);
void ObjSpinyroll_Draw(Actor* thisx, PlayState* play);

void func_80A1E9C4(ObjSpinyroll* this);
void func_80A1E9E0(ObjSpinyroll* this, PlayState* play);
void func_80A1EA4C(ObjSpinyroll* this, PlayState* play);
void func_80A1EAAC(ObjSpinyroll* this);
void func_80A1EAE0(ObjSpinyroll* this, PlayState* play);
void func_80A1EB40(ObjSpinyroll* this);
void func_80A1EB54(ObjSpinyroll* this, PlayState* play);
void func_80A1EC24(ObjSpinyroll* this);
void func_80A1EC38(ObjSpinyroll* this, PlayState* play);
void func_80A1ECC0(ObjSpinyroll* this);
void func_80A1ECD4(ObjSpinyroll* this, PlayState* play);

ActorProfile Obj_Spinyroll_Profile = {
    /**/ ACTOR_OBJ_SPINYROLL,
    /**/ ACTORCAT_PROP,
    /**/ FLAGS,
    /**/ OBJECT_SPINYROLL,
    /**/ sizeof(ObjSpinyroll),
    /**/ ObjSpinyroll_Init,
    /**/ ObjSpinyroll_Destroy,
    /**/ ObjSpinyroll_Update,
    /**/ ObjSpinyroll_Draw,
};

static ColliderTrisElementInit sTrisElementsInit[] = {
    {
        {
            ELEM_MATERIAL_UNK0,
            { 0x20000000, 0x00, 0x04 },
            { 0x01C37BB6, 0x00, 0x00 },
            ATELEM_ON | ATELEM_SFX_NORMAL,
            ACELEM_ON,
            OCELEM_NONE,
        },
        { { { -60.0f, 0.0f, 17.0f }, { 60.0f, 0.0f, 17.0f }, { 60.0f, 40.0f, 17.0f } } },
    },
    {
        {
            ELEM_MATERIAL_UNK0,
            { 0x20000000, 0x00, 0x04 },
            { 0x01C37BB6, 0x00, 0x00 },
            ATELEM_ON | ATELEM_SFX_NORMAL,
            ACELEM_ON,
            OCELEM_NONE,
        },
        { { { -60.0f, 0.0f, 17.0f }, { 60.0f, 40.0f, 17.0f }, { -60.0f, 40.0f, 17.0f } } },
    },
    {
        {
            ELEM_MATERIAL_UNK0,
            { 0x20000000, 0x00, 0x04 },
            { 0x01C37BB6, 0x00, 0x00 },
            ATELEM_ON | ATELEM_SFX_NORMAL,
            ACELEM_ON,
            OCELEM_NONE,
        },
        { { { 60.0f, 0.0f, -17.0f }, { -60.0f, 0.0f, -17.0f }, { -60.0f, 40.0f, -17.0f } } },
    },
    {
        {
            ELEM_MATERIAL_UNK0,
            { 0x20000000, 0x00, 0x04 },
            { 0x01C37BB6, 0x00, 0x00 },
            ATELEM_ON | ATELEM_SFX_NORMAL,
            ACELEM_ON,
            OCELEM_NONE,
        },
        { { { 60.0f, 0.0f, -17.0f }, { -60.0f, 40.0f, -17.0f }, { 60.0f, 40.0f, -17.0f } } },
    },
    {
        {
            ELEM_MATERIAL_UNK0,
            { 0xF7CFFFFF, 0x04, 0x04 },
            { 0x01C37BB6, 0x00, 0x00 },
            ATELEM_ON | ATELEM_SFX_NORMAL,
            ACELEM_ON,
            OCELEM_NONE,
        },
        { { { 60.0f, 20.0f, -12.0f }, { -60.0f, 20.0f, -12.0f }, { -60.0f, 20.0f, 12.0f } } },
    },
    {
        {
            ELEM_MATERIAL_UNK0,
            { 0xF7CFFFFF, 0x04, 0x04 },
            { 0x01C37BB6, 0x00, 0x00 },
            ATELEM_ON | ATELEM_SFX_NORMAL,
            ACELEM_ON,
            OCELEM_NONE,
        },
        { { { 60.0f, 20.0f, -12.0f }, { -60.0f, 20.0f, 12.0f }, { 60.0f, 20.0f, 12.0f } } },
    },
};

static ColliderTrisInit sTrisInit = {
    {
        COL_MATERIAL_METAL,
        AT_ON | AT_TYPE_ENEMY,
        AC_ON | AC_HARD | AC_TYPE_PLAYER,
        OC1_NONE,
        OC2_NONE,
        COLSHAPE_TRIS,
    },
    ARRAY_COUNT(sTrisElementsInit),
    sTrisElementsInit,
};

f32 D_80A1F1B8[] = { 1.0f, 2.0f, 4.0f };

f32 D_80A1F1C4[] = {
    1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f,
};

s16 D_80A1F1E4[] = {
    1, 10, 20, 30, 40, 50, 60, 70,
};

f32 D_80A1F1F4[] = { 30.0f, -30.0f };

f32 D_80A1F1FC[] = { 9.0f, -9.0f };

static InitChainEntry sInitChain[] = {
    ICHAIN_F32(cullingVolumeDistance, 4000, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeDownward, 500, ICHAIN_STOP),
};

s16 D_80A1F20C[] = { 0xFA0, -0xFA0 };

void func_80A1DA50(PlayState* play, ObjSpinyroll* this, Vec3f* arg2, Vec3f* arg3) {
    Vec3f sp1C;

    Math_Vec3f_Sum(arg2, arg3, &sp1C);
    Math_Vec3f_Scale(&sp1C, 0.5f);
    EffectSsHitmark_SpawnFixedScale(play, EFFECT_HITMARK_METAL, &sp1C);
    Actor_PlaySfx(&this->dyna.actor, NA_SE_IT_SHIELD_REFLECT_SW);
}

void func_80A1DAAC(Vec3f* arg0, Vec3f* arg1, s16 arg2) {
    f32 sp1C = Math_SinS(arg2);
    f32 sp18 = Math_CosS(arg2);

    arg0->x = (arg1->z * sp1C) + (arg1->x * sp18);
    arg0->y = arg1->y;
    arg0->z = (arg1->z * sp18) - (arg1->x * sp1C);
}

void func_80A1DB2C(Actor* thisx) {
    ObjSpinyroll* this = (ObjSpinyroll*)thisx;
    s32 i;
    s32 j;
    s32 params = OBJSPINYROLL_GET_C000(&this->dyna.actor);
    Vec3f sp7C[3];
    Vec3f sp70;
    f32 temp = D_80A1F1B8[params];
    ColliderTrisElementInit* ptr;

    for (i = 0; i < ARRAY_COUNT(this->colliderElements); i++) {
        for (j = 0; j < ARRAY_COUNT(sp7C); j++) {
            ptr = &sTrisInit.elements[i];

            sp70.x = ptr->dim.vtx[j].x * temp;
            sp70.y = ptr->dim.vtx[j].y;
            sp70.z = ptr->dim.vtx[j].z;

            func_80A1DAAC(&sp7C[j], &sp70, this->dyna.actor.shape.rot.y);
            Math_Vec3f_Sum(&sp7C[j], &this->dyna.actor.world.pos, &sp7C[j]);
        }
        Collider_SetTrisVertices(&this->collider, i, &sp7C[0], &sp7C[1], &sp7C[2]);
    }
}

void func_80A1DC5C(ObjSpinyroll* this) {
    this->unk_4D8 += 0x5DC0;
    Math_StepToF(&this->unk_4D4, 0.0f, 0.32f);
    this->unk_4D0 = ((Math_SinS(this->unk_4D8) + 1.0f) * this->unk_4D4) + 24.0f;
}

void func_80A1DCCC(ObjSpinyroll* this) {
    f32 phi_f2;

    if (this->dyna.actor.speed > 3.0f) {
        phi_f2 = 3.0f;
    } else {
        phi_f2 = this->dyna.actor.speed;
    }

    if (this->unk_4D4 < phi_f2) {
        this->unk_4D4 = phi_f2;
    }
}

void func_80A1DD18(ObjSpinyroll* this) {
    f32 temp_f0;
    Vec3f* sp20 = &this->unk_4AC[this->unk_4A8 ^ 1];

    Math_Vec3f_Diff(sp20, &this->dyna.actor.world.pos, &this->unk_4C4);

    temp_f0 = Math3D_Vec3fMagnitude(&this->unk_4C4);
    if (temp_f0 < 0.01f) {
        Math_Vec3f_Diff(sp20, &this->unk_4AC[this->unk_4A8], &this->unk_4C4);

        temp_f0 = Math3D_Vec3fMagnitude(&this->unk_4C4);
        if (temp_f0 < 0.01f) {
            Math_Vec3f_Copy(&this->unk_4C4, &gZeroVec3f);
            return;
        }
    }
    Math_Vec3f_Scale(&this->unk_4C4, 1.0f / temp_f0);
}

void func_80A1DE10(ObjSpinyroll* this) {
    this->dyna.actor.world.rot.y = Math_Vec3f_Yaw(&this->dyna.actor.world.pos, &this->unk_4AC[this->unk_4A8 ^ 1]);
}

void func_80A1DE58(ObjSpinyroll* this) {
    this->unk_4A8 = 0;
    func_80A1DE10(this);
    func_80A1DD18(this);
}

void func_80A1DE84(ObjSpinyroll* this) {
    this->unk_4A8 ^= 1;
    func_80A1DE10(this);
    func_80A1DD18(this);
}

s32 func_80A1DEB8(ObjSpinyroll* this) {
    s32 pad;
    s32 sp30 = this->unk_4A8 ^ 1;
    Vec3f sp24;

    Math_StepToF(&this->dyna.actor.speed, this->unk_4A4, this->unk_4A4 * 0.2f);

    this->dyna.actor.world.pos.x += this->dyna.actor.speed * this->unk_4C4.x;
    this->dyna.actor.world.pos.y += this->dyna.actor.speed * this->unk_4C4.y;
    this->dyna.actor.world.pos.z += this->dyna.actor.speed * this->unk_4C4.z;

    Math_Vec3f_Diff(&this->unk_4AC[sp30], &this->dyna.actor.world.pos, &sp24);

    return Math3D_Vec3fMagnitudeSq(&sp24) < (SQ(this->dyna.actor.speed) + 0.05f);
}

void func_80A1DFA0(ObjSpinyroll* this) {
    f32 temp_f0;
    f32 phi_f12;
    s32 i;
    s32 j;
    f32 temp_f2;

    this->unk_3A4.unk_F4 = (s32)(D_80A1F1B8[OBJSPINYROLL_GET_C000(&this->dyna.actor)] * 120.0f * (1.0f / 58)) + 2;

    temp_f0 = (D_80A1F1B8[OBJSPINYROLL_GET_C000(&this->dyna.actor)] * 120.0f) - 2.0f;
    temp_f2 = temp_f0 / (this->unk_3A4.unk_F4 - 1);
    temp_f0 *= 0.5f;

    for (i = 0; i < ARRAY_COUNT(this->unk_3A4.unk_00); i++) {
        for (j = 0, phi_f12 = temp_f0; j < this->unk_3A4.unk_F4; j++, phi_f12 -= temp_f2) {
            this->unk_3A4.unk_00[i][j].unk_00 = phi_f12;
        }
    }
}

s32 func_80A1E074(ObjSpinyroll* this, PlayState* play, Vec3f* arg2, s32 arg3) {
    f32 temp_f0;
    ObjSpinyrollStruct* ptr = &this->unk_3A4;
    f32 temp_f20;
    s32 i;
    s32 j;
    Vec3f spC8;
    Vec3f spBC;
    Vec3f spB0;
    ObjSpinyrollSubStruct* temp_s1;
    f32 temp_f22;
    f32 temp_f24;
    f32 temp;
    s32 pad;
    s32 sp98;

    spC8.y = this->dyna.actor.world.pos.y + 10.0f;
    spBC.y = spC8.y;
    sp98 = false;
    temp_f20 = FLT_MAX;
    ptr->unk_F0 = NULL;

    for (i = 0; i < ARRAY_COUNT(this->unk_3A4.unk_00); i++) {
        temp = D_80A1F1F4[i];
        temp_f22 = temp * this->unk_4C4.x;
        temp_f24 = temp * this->unk_4C4.z;

        for (j = 0; j < ptr->unk_F4; j++) {
            temp_s1 = &ptr->unk_00[i][j];
            spC8.x = (temp_s1->unk_00 * Math_CosS(this->dyna.actor.world.rot.y)) + this->dyna.actor.world.pos.x;
            spC8.z = (temp_s1->unk_00 * -Math_SinS(this->dyna.actor.world.rot.y)) + this->dyna.actor.world.pos.z;

            spBC.x = temp_f22 + spC8.x;
            spBC.z = temp_f24 + spC8.z;

            if (BgCheck_EntityLineTest3(&play->colCtx, &spC8, &spBC, &spB0, &temp_s1->unk_04, true, false, false, true,
                                        &temp_s1->bgId, &this->dyna.actor, 0.0f)) {
                if (arg3 && (this->dyna.actor.flags & ACTOR_FLAG_INSIDE_CULLING_VOLUME)) {
                    func_80A1DA50(play, this, &spC8, &spB0);
                }

                temp_f0 = Math3D_Vec3fDistSq(&spC8, &spB0);
                if (temp_f0 < temp_f20) {
                    temp_f20 = temp_f0;
                    sp98 = true;
                    Math_Vec3f_Diff(&spB0, &spBC, arg2);
                    ptr->unk_F0 = temp_s1;
                }
            }
        }
    }

    return sp98;
}

s32 func_80A1E2D8(ObjSpinyroll* this, PlayState* play, s32 arg2) {
    Vec3f sp1C;

    if (func_80A1E074(this, play, &sp1C, arg2)) {
        this->dyna.actor.world.pos.x += sp1C.x;
        this->dyna.actor.world.pos.z += sp1C.z;
        return true;
    }
    return false;
}

s32 func_80A1E334(CollisionContext* colCtx, f32* arg1, CollisionPoly** polyOut, s32* bgId, Vec3f* arg4,
                  ObjSpinyroll* this) {
    f32 temp_f0 = BgCheck_EntityRaycastFloor5(colCtx, polyOut, bgId, &this->dyna.actor, arg4);

    *arg1 = temp_f0;
    return *arg1 > (BGCHECK_Y_MIN + 1);
}

s32 func_80A1E394(CollisionContext* colCtx, f32* arg1, CollisionPoly** polyOut, s32* bgId, Vec3f* arg4,
                  ObjSpinyroll* this) {
    return BgCheck_EntityCheckCeiling(colCtx, arg1, arg4, 24.0f, polyOut, bgId, &this->dyna.actor);
}

s32 func_80A1E3D8(ObjSpinyroll* this, PlayState* play, f32* arg2, s32 arg3) {
    f32 phi_f26;
    ObjSpinyrollStruct* spC8 = &this->unk_3A4;
    f32 phi_f22;
    ObjSpinyrollColFunc spC0;
    s32 i;
    s32 j;
    Vec3f spAC;
    Vec3f spA0;
    Vec3f sp94;
    ObjSpinyrollSubStruct* temp_s0;
    f32 temp_f20;
    s32 pad;
    s32 sp84;
    s32 pad2;
    f32* temp;

    if (this->unk_4C4.y > 0.0f) {
        spC0 = func_80A1E394;
        phi_f26 = this->dyna.actor.world.pos.y + 48.0f;
    } else {
        spC0 = func_80A1E334;
        phi_f26 = this->dyna.actor.world.pos.y;
    }

    sp84 = false;
    sp94.y = this->dyna.actor.world.pos.y + 24.0f;
    phi_f22 = FLT_MAX;
    spC8->unk_F0 = NULL;

    for (i = 0, temp = D_80A1F1FC; i < ARRAY_COUNT(this->unk_3A4.unk_00); i++, temp++) {
        sp94.z = *temp;

        for (j = 0; j < spC8->unk_F4; j++) {
            temp_s0 = &spC8->unk_00[i][j];

            sp94.x = temp_s0->unk_00;
            func_80A1DAAC(&spAC, &sp94, this->dyna.actor.world.rot.y);

            spAC.x += this->dyna.actor.world.pos.x;
            spAC.z += this->dyna.actor.world.pos.z;

            if (spC0(&play->colCtx, &spA0.y, &temp_s0->unk_04, &temp_s0->bgId, &spAC, this)) {
                temp_f20 = fabsf(spA0.y - spAC.y);
                if (temp_f20 <= 24.0f) {
                    if (arg3 && (this->dyna.actor.flags & ACTOR_FLAG_INSIDE_CULLING_VOLUME)) {
                        spA0.x = spAC.x;
                        spA0.z = spAC.z;
                        func_80A1DA50(play, this, &spAC, &spA0);
                    }

                    if (temp_f20 < phi_f22) {
                        sp84 = true;
                        *arg2 = spA0.y - phi_f26;
                        phi_f22 = temp_f20;
                        spC8->unk_F0 = temp_s0;
                    }
                }
            }
        }
    }
    return sp84;
}

s32 func_80A1E648(ObjSpinyroll* this, PlayState* play, s32 arg2) {
    f32 sp1C;

    if (func_80A1E3D8(this, play, &sp1C, arg2)) {
        this->dyna.actor.world.pos.y += sp1C;
        return true;
    }
    return false;
}

DynaPolyActor* func_80A1E694(ObjSpinyroll* this, PlayState* play) {
    if (this->unk_3A4.unk_F0 != NULL) {
        return DynaPoly_GetActor(&play->colCtx, this->unk_3A4.unk_F0->bgId);
    }
    return NULL;
}

s32 func_80A1E6D4(ObjSpinyroll* this, PlayState* play) {
    DynaPolyActor* temp_v0 = func_80A1E694(this, play);

    if ((temp_v0 != NULL) && (Math3D_Vec3fDistSq(&temp_v0->actor.world.pos, &temp_v0->actor.prevPos) > SQ(0.01f))) {
        return true;
    }
    return false;
}

void ObjSpinyroll_Init(Actor* thisx, PlayState* play) {
    s32 pad;
    ObjSpinyroll* this = (ObjSpinyroll*)thisx;
    f32 sp44;
    Path* path;
    s32 pad2;
    Vec3s* points;
    Vec3s* sp34;
    Vec3s* sp30;

    sp44 = D_80A1F1B8[OBJSPINYROLL_GET_C000(&this->dyna.actor)];
    Actor_ProcessInitChain(&this->dyna.actor, sInitChain);
    this->dyna.actor.world.rot.x = 0;
    this->dyna.actor.world.rot.z = 0;

    this->dyna.actor.shape.rot.x = 0;
    this->dyna.actor.shape.rot.z = 0;

    this->dyna.actor.scale.x = 0.1f * sp44;
    this->dyna.actor.scale.y = 0.1f;
    this->dyna.actor.scale.z = 0.1f;

    this->dyna.actor.cullingVolumeScale = 250.0f * sp44;
    DynaPolyActor_Init(&this->dyna, 0);
    DynaPolyActor_LoadMesh(play, &this->dyna, &object_spinyroll_Colheader_000E68);

    Collider_InitTris(play, &this->collider);
    Collider_SetTris(play, &this->collider, &this->dyna.actor, &sTrisInit, this->colliderElements);

    if (OBJSPINYROLL_GET_PATH_INDEX(&this->dyna.actor) == OBJSPINYROLL_PATH_INDEX_NONE) {
        func_80A1E9C4(this);
        return;
    }

    this->dyna.actor.world.rot.y = 0;
    func_80A1DFA0(this);
    this->unk_4A4 = D_80A1F1C4[OBJSPINYROLL_GET_380(thisx)];

    path = &play->setupPathList[OBJSPINYROLL_GET_PATH_INDEX(&this->dyna.actor)];
    points = Lib_SegmentedToVirtual(path->points);

    sp34 = &points[0];
    sp30 = &points[1];

    Math_Vec3s_ToVec3f(&this->unk_4AC[0], sp34);
    Math_Vec3s_ToVec3f(&this->unk_4AC[1], sp30);
    Math_Vec3f_Copy(&this->dyna.actor.world.pos, &this->unk_4AC[0]);

    if ((sp34->x == sp30->x) && (sp34->y != sp30->y) && (sp34->z == sp30->z)) {
        this->unk_49C = true;
    } else if (((sp34->x != sp30->x) && (sp34->y == sp30->y) && (sp34->z == sp30->z)) ||
               ((sp34->x == sp30->x) && (sp34->y == sp30->y) && (sp34->z != sp30->z))) {
        this->unk_49C = false;
    } else {
        Actor_Kill(&this->dyna.actor);
        return;
    }

    func_80A1DE58(this);
    this->dyna.actor.shape.rot.y = this->dyna.actor.world.rot.y;
    func_80A1EAAC(this);
}

void ObjSpinyroll_Destroy(Actor* thisx, PlayState* play) {
    ObjSpinyroll* this = (ObjSpinyroll*)thisx;

    DynaPoly_DeleteBgActor(play, &play->colCtx.dyna, this->dyna.bgId);
    Collider_DestroyTris(play, &this->collider);
}

void func_80A1E9C4(ObjSpinyroll* this) {
    this->dyna.actor.speed = 0.0f;
    this->actionFunc = func_80A1E9E0;
}

void func_80A1E9E0(ObjSpinyroll* this, PlayState* play) {
    this->unk_4DA += 0x6E;
    if (this->unk_4DA > 0xFA0) {
        this->unk_4DA = 0xFA0;
    }
}

void func_80A1EA10(ObjSpinyroll* this) {
    this->actionFunc = func_80A1EA4C;
    this->unk_4E2 = D_80A1F1E4[OBJSPINYROLL_GET_1C00(&this->dyna.actor)];
    this->dyna.actor.speed = 0.0f;
}

void func_80A1EA4C(ObjSpinyroll* this, PlayState* play) {
    if (Math_ScaledStepToS(&this->unk_4DA, 0, this->unk_4DE)) {
        if (this->unk_4E2 > 0) {
            this->unk_4E2--;
            if (this->unk_4E2 <= 0) {
                func_80A1EAAC(this);
            }
        }
    }
}

void func_80A1EAAC(ObjSpinyroll* this) {
    this->actionFunc = func_80A1EAE0;
    this->unk_4DC = D_80A1F20C[this->unk_4A8];
    this->dyna.actor.speed = 0.0f;
}

void func_80A1EAE0(ObjSpinyroll* this, PlayState* play) {
    if (Math_ScaledStepToS(&this->unk_4DA, this->unk_4DC, 0x6E)) {
        if (!this->unk_49C) {
            func_80A1EB40(this);
        } else {
            func_80A1ECC0(this);
        }
    }
}

void func_80A1EB40(ObjSpinyroll* this) {
    this->actionFunc = func_80A1EB54;
}

void func_80A1EB54(ObjSpinyroll* this, PlayState* play) {
    s32 sp24 = func_80A1DEB8(this);

    if (func_80A1E2D8(this, play, 1)) {
        if (func_80A1E694(this, play) != NULL) {
            func_80A1DCCC(this);
            this->unk_4DE = 0x7D0;
            func_80A1EC24(this);
        } else {
            func_80A1DE84(this);
            func_80A1DCCC(this);
            this->unk_4DE = 0x7D0;
            func_80A1EA10(this);
        }
    } else if (sp24 != 0) {
        func_80A1DE84(this);
        func_80A1DCCC(this);
        this->unk_4DE = 0x78;
        func_80A1EA10(this);
    }
}

void func_80A1EC24(ObjSpinyroll* this) {
    this->actionFunc = func_80A1EC38;
}

void func_80A1EC38(ObjSpinyroll* this, PlayState* play) {
    s32 sp24 = true;

    Math_ScaledStepToS(&this->unk_4DA, 0, this->unk_4DE);

    if (func_80A1E2D8(this, play, 0) && func_80A1E6D4(this, play)) {
        sp24 = false;
    }

    if (sp24) {
        func_80A1DE84(this);
        func_80A1EA10(this);
    }
}

void func_80A1ECC0(ObjSpinyroll* this) {
    this->actionFunc = func_80A1ECD4;
}

void func_80A1ECD4(ObjSpinyroll* this, PlayState* play) {
    s32 sp24 = func_80A1DEB8(this);

    if (func_80A1E648(this, play, 1)) {
        func_80A1DE84(this);
        func_80A1DCCC(this);
        this->unk_4DE = 0x7D0;
        func_80A1EA10(this);
    } else if (sp24) {
        func_80A1DE84(this);
        func_80A1DCCC(this);
        this->unk_4DE = 0x78;
        func_80A1EA10(this);
    }
}

void ObjSpinyroll_Update(Actor* thisx, PlayState* play2) {
    PlayState* play = play2;
    ObjSpinyroll* this = (ObjSpinyroll*)thisx;

    if (this->collider.base.atFlags & AT_HIT) {
        this->collider.base.atFlags &= ~AT_HIT;
        func_80A1DCCC(this);
    }

    this->actionFunc(this, play);

    this->unk_4E0 += this->unk_4DA;
    func_80A1DC5C(this);
    func_80A1DB2C(thisx);

    CollisionCheck_SetAT(play, &play->colChkCtx, &this->collider.base);
    CollisionCheck_SetAC(play, &play->colChkCtx, &this->collider.base);
}

void ObjSpinyroll_Draw(Actor* thisx, PlayState* play) {
    ObjSpinyroll* this = (ObjSpinyroll*)thisx;
    f32 temp_f26;
    f32 temp_f28;
    s32 temp_s1 = OBJSPINYROLL_GET_C000(&this->dyna.actor);
    s32 phi_s4;
    f32 phi_f22;
    f32 phi_f24;
    s32 i;
    f32 temp_f30;
    Vec3s sp84;

    OPEN_DISPS(play->state.gfxCtx);

    Gfx_SetupDL25_Opa(play->state.gfxCtx);

    temp_f26 = Math_CosS(this->dyna.actor.shape.rot.y) * 120.0f;
    temp_f28 = Math_SinS(this->dyna.actor.shape.rot.y) * -120.0f;
    temp_f30 = this->dyna.actor.world.pos.y + this->unk_4D0;

    if (temp_s1 == OBJSPINYROLL_GET_C000_0) {
        phi_s4 = 1;
        phi_f22 = this->dyna.actor.world.pos.x;
        phi_f24 = this->dyna.actor.world.pos.z;
    } else if (temp_s1 == OBJSPINYROLL_GET_C000_1) {
        phi_s4 = 2;
        phi_f22 = this->dyna.actor.world.pos.x - (temp_f26 * 0.5f);
        phi_f24 = this->dyna.actor.world.pos.z - (temp_f28 * 0.5f);
    } else {
        phi_s4 = 4;
        phi_f22 = this->dyna.actor.world.pos.x - (temp_f26 * 1.5f);
        phi_f24 = this->dyna.actor.world.pos.z - (temp_f28 * 1.5f);
    }

    sp84.x = this->dyna.actor.shape.rot.x + this->unk_4E0;
    sp84.y = this->dyna.actor.shape.rot.y;
    sp84.z = this->dyna.actor.shape.rot.z;

    for (i = 0; i < phi_s4; i++) {
        Matrix_SetTranslateRotateYXZ(phi_f22, temp_f30, phi_f24, &sp84);
        Matrix_Scale(0.1f, 0.1f, 0.1f, MTXMODE_APPLY);

        MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx);
        gSPDisplayList(POLY_OPA_DISP++, object_spinyroll_DL_000460);

        phi_f22 += temp_f26;
        phi_f24 += temp_f28;
    }

    CLOSE_DISPS(play->state.gfxCtx);
}
