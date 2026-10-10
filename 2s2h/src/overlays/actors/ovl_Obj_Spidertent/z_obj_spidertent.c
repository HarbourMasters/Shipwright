/*
 * File: z_obj_spidertent.c
 * Overlay: ovl_Obj_Spidertent
 * Description: Tent-Shaped Spider Web
 */

#include "z_obj_spidertent.h"
#include "objects/object_spidertent/object_spidertent.h"

#define FLAGS (ACTOR_FLAG_UCODE_POINT_LIGHT_ENABLED)

void ObjSpidertent_Init(Actor* thisx, PlayState* play);
void ObjSpidertent_Destroy(Actor* thisx, PlayState* play);
void ObjSpidertent_Update(Actor* thisx, PlayState* play);
void ObjSpidertent_Draw(Actor* thisx, PlayState* play);

void func_80B307E0(ObjSpidertent* this);
void func_80B30808(ObjSpidertent* this, PlayState* play);
void func_80B30A2C(ObjSpidertent* this);
void func_80B30A4C(ObjSpidertent* this, PlayState* play);
void func_80B30AD4(ObjSpidertent* this);
void func_80B30AF8(ObjSpidertent* this, PlayState* play);

ActorProfile Obj_Spidertent_Profile = {
    /**/ ACTOR_OBJ_SPIDERTENT,
    /**/ ACTORCAT_BG,
    /**/ FLAGS,
    /**/ OBJECT_SPIDERTENT,
    /**/ sizeof(ObjSpidertent),
    /**/ ObjSpidertent_Init,
    /**/ ObjSpidertent_Destroy,
    /**/ ObjSpidertent_Update,
    /**/ ObjSpidertent_Draw,
};

static ColliderTrisElementInit sTrisElementsInit1[] = {
    {
        {
            ELEM_MATERIAL_UNK4,
            { 0x00000000, 0x00, 0x00 },
            { 0x00000800, 0x00, 0x00 },
            ATELEM_NONE | ATELEM_SFX_NORMAL,
            ACELEM_ON,
            OCELEM_NONE,
        },
        { { { 68.80000305175781f, 0.0f, 18.799999237060547f },
            { 76.30000305175781f, 0.0f, -63.79999923706055f },
            { 1.2999999523162842f, 40.0f, -3.799999952316284f } } },
    },
    {
        {
            ELEM_MATERIAL_UNK4,
            { 0x00000000, 0x00, 0x00 },
            { 0x00000800, 0x00, 0x00 },
            ATELEM_NONE | ATELEM_SFX_NORMAL,
            ACELEM_ON,
            OCELEM_NONE,
        },
        { { { 76.30000305175781f, 0.0f, -63.79999923706055f },
            { 23.799999237060547f, 0.0f, -116.30000305175781f },
            { 1.2999999523162842f, 40.0f, -3.799999952316284f } } },
    },
    {
        {
            ELEM_MATERIAL_UNK4,
            { 0x00000000, 0x00, 0x00 },
            { 0x00000800, 0x00, 0x00 },
            ATELEM_NONE | ATELEM_SFX_NORMAL,
            ACELEM_ON,
            OCELEM_NONE,
        },
        { { { 23.799999237060547f, 0.0f, -116.30000305175781f },
            { -111.30000305175781f, 0.0f, -26.299999237060547f },
            { 1.2999999523162842f, 40.0f, -3.799999952316284f } } },
    },
    {
        {
            ELEM_MATERIAL_UNK4,
            { 0x00000000, 0x00, 0x00 },
            { 0x00000800, 0x00, 0x00 },
            ATELEM_NONE | ATELEM_SFX_NORMAL,
            ACELEM_ON,
            OCELEM_NONE,
        },
        { { { -111.30000305175781f, 0.0f, -26.299999237060547f },
            { -81.30000305175781f, 0.0f, 78.80000305175781f },
            { 1.2999999523162842f, 40.0f, -3.799999952316284f } } },
    },
    {
        {
            ELEM_MATERIAL_UNK4,
            { 0x00000000, 0x00, 0x00 },
            { 0x00000800, 0x00, 0x00 },
            ATELEM_NONE | ATELEM_SFX_NORMAL,
            ACELEM_ON,
            OCELEM_NONE,
        },
        { { { -81.30000305175781f, 0.0f, 78.80000305175781f },
            { 23.799999237060547f, 0.0f, 108.80000305175781f },
            { 1.2999999523162842f, 40.0f, -3.799999952316284f } } },
    },
    {
        {
            ELEM_MATERIAL_UNK4,
            { 0x00000000, 0x00, 0x00 },
            { 0x00000800, 0x00, 0x00 },
            ATELEM_NONE | ATELEM_SFX_NORMAL,
            ACELEM_ON,
            OCELEM_NONE,
        },
        { { { 23.799999237060547f, 0.0f, 108.80000305175781f },
            { 68.80000305175781f, 0.0f, 18.799999237060547f },
            { 1.2999999523162842f, 40.0f, -3.799999952316284f } } },
    },
};

static ColliderTrisInit sTrisInit1 = {
    {
        COL_MATERIAL_NONE,
        AT_NONE,
        AC_ON | AC_TYPE_PLAYER,
        OC1_NONE,
        OC2_NONE,
        COLSHAPE_TRIS,
    },
    ARRAY_COUNT(sTrisElementsInit1),
    sTrisElementsInit1,
};

static ColliderTrisElementInit sTrisElementsInit2[] = {
    {
        {
            ELEM_MATERIAL_UNK4,
            { 0x00000000, 0x00, 0x00 },
            { 0x00000800, 0x00, 0x00 },
            ATELEM_NONE | ATELEM_SFX_NORMAL,
            ACELEM_ON,
            OCELEM_NONE,
        },
        { { { 17.799999237060547f, 0.0f, 81.5999984741211f },
            { 51.599998474121094f, 0.0f, 14.100000381469727f },
            { 0.8999999761581421f, 30.0f, -2.799999952316284f } } },
    },
    {
        {
            ELEM_MATERIAL_UNK4,
            { 0x00000000, 0x00, 0x00 },
            { 0x00000800, 0x00, 0x00 },
            ATELEM_NONE | ATELEM_SFX_NORMAL,
            ACELEM_ON,
            OCELEM_NONE,
        },
        { { { -60.900001525878906f, 0.0f, 59.099998474121094f },
            { 17.799999237060547f, 0.0f, 81.5999984741211f },
            { 0.8999999761581421f, 30.0f, -2.799999952316284f } } },
    },
    {
        {
            ELEM_MATERIAL_UNK4,
            { 0x00000000, 0x00, 0x00 },
            { 0x00000800, 0x00, 0x00 },
            ATELEM_NONE | ATELEM_SFX_NORMAL,
            ACELEM_ON,
            OCELEM_NONE,
        },
        { { { -83.4000015258789f, 0.0f, -19.700000762939453f },
            { -60.900001525878906f, 0.0f, 59.099998474121094f },
            { 0.8999999761581421f, 30.0f, -2.799999952316284f } } },
    },
    {
        {
            ELEM_MATERIAL_UNK4,
            { 0x00000000, 0x00, 0x00 },
            { 0x00000800, 0x00, 0x00 },
            ATELEM_NONE | ATELEM_SFX_NORMAL,
            ACELEM_ON,
            OCELEM_NONE,
        },
        { { { 17.799999237060547f, 0.0f, -87.19999694824219f },
            { -83.4000015258789f, 0.0f, -19.700000762939453f },
            { 0.8999999761581421f, 30.0f, -2.799999952316284f } } },
    },
    {
        {
            ELEM_MATERIAL_UNK4,
            { 0x00000000, 0x00, 0x00 },
            { 0x00000800, 0x00, 0x00 },
            ATELEM_NONE | ATELEM_SFX_NORMAL,
            ACELEM_ON,
            OCELEM_NONE,
        },
        { { { 57.20000076293945f, 0.0f, -47.79999923706055f },
            { 17.799999237060547f, 0.0f, -87.19999694824219f },
            { 0.8999999761581421f, 30.0f, -2.799999952316284f } } },
    },
    {
        {
            ELEM_MATERIAL_UNK4,
            { 0x00000000, 0x00, 0x00 },
            { 0x00000800, 0x00, 0x00 },
            ATELEM_NONE | ATELEM_SFX_NORMAL,
            ACELEM_ON,
            OCELEM_NONE,
        },
        { { { 51.599998474121094f, 0.0f, 14.100000381469727f },
            { 57.20000076293945f, 0.0f, -47.79999923706055f },
            { 0.8999999761581421f, 30.0f, -2.799999952316284f } } },
    },
};

static ColliderTrisInit sTrisInit2 = {
    {
        COL_MATERIAL_NONE,
        AT_NONE,
        AC_ON | AC_TYPE_PLAYER,
        OC1_NONE,
        OC2_NONE,
        COLSHAPE_TRIS,
    },
    ARRAY_COUNT(sTrisElementsInit2),
    sTrisElementsInit2,
};

typedef struct {
    /* 0x00 */ Gfx* unk_00;
    /* 0x04 */ CollisionHeader* unk_04;
    /* 0x08 */ ColliderTrisInit* unk_08;
    /* 0x0C */ f32 unk_0C;
    /* 0x10 */ f32 unk_10;
    /* 0x14 */ f32 unk_14;
    /* 0x18 */ f32 unk_18;
    /* 0x1C */ f32 unk_1C;
    /* 0x20 */ f32 unk_20;
    /* 0x24 */ f32 unk_24;
} ObjSpidertentStruct; // size = 0x28

ObjSpidertentStruct D_80B31350[] = {
    {
        object_spidertent_DL_000070,
        &object_spidertent_Colheader_0011AC,
        &sTrisInit1,
        0.9f,
        1.3f,
        40.0f,
        -3.8f,
        SQ(120.0f),
        240.0f,
        200.0f,
    },
    {
        object_spidertent_DL_001250,
        &object_spidertent_Colheader_00238C,
        &sTrisInit2,
        0.9f,
        0.9f,
        30.0f,
        -2.8f,
        SQ(88.0f),
        176.0f,
        168.0f,
    },
};

typedef struct {
    /* 0x00 */ f32 unk_00;
    /* 0x04 */ f32 unk_04;
    /* 0x08 */ f32 unk_08;
    /* 0x0C */ s16 unk_0C;
    /* 0x0E */ s8 unk_0E;
    /* 0x0F */ s8 unk_0F;
    /* 0x10 */ Color_RGBA8 unk_10;
    /* 0x14 */ Color_RGBA8 unk_14;
} ObjSpidertentStruct2; // size = 0x18

ObjSpidertentStruct2 D_80B313A0[] = {
    {
        -20.0f,
        6.0f,
        0.0f,
        100,
        14,
        5,
        { 255, 255, 160, 160 },
        { 255, 0, 0, 0 },
    },
    {
        0.0f,
        4.0f,
        1.2f,
        80,
        9,
        3,
        { 255, 255, 160, 120 },
        { 255, 0, 0, 0 },
    },
    {
        0.0f,
        4.2f,
        0.3f,
        70,
        12,
        2,
        { 255, 255, 160, 100 },
        { 55, 0, 0, 0 },
    },
    {
        0.0f,
        3.0f,
        0.0f,
        20,
        6,
        2,
        { 255, 100, 20, 80 },
        { 255, 0, 0, 0 },
    },
};

Vec3f D_80B31400 = { 1.0f, 0.0f, 0.0f };
Vec3f D_80B3140C = { 0.0f, 1.0f, 0.0f };

static InitChainEntry sInitChain[] = {
    ICHAIN_F32(cullingVolumeDistance, 4000, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeScale, 200, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeDownward, 200, ICHAIN_CONTINUE),
    ICHAIN_VEC3F_DIV1000(scale, 1000, ICHAIN_STOP),
};

s32 func_80B2FB10(Vec3f* arg0, Vec3f* arg1) {
    f32 temp_f0 = Math3D_Vec3fMagnitudeSq(arg0);
    f32 temp_f2;

    if (temp_f0 < 9.999999e-9f) {
        return false;
    }
    temp_f2 = 1.0f / sqrtf(temp_f0);
    arg1->x = arg0->x * temp_f2;
    arg1->y = arg0->y * temp_f2;
    arg1->z = arg0->z * temp_f2;
    return true;
}

s32 func_80B2FB94(Vec3f* arg0, f32 arg1, TriNorm* triNorm, Vec3f* arg3, f32* arg4) {
    f32 temp_f0;
    f32 temp_f0_2;
    f32 temp_f16;
    Vec3f sp48;
    Vec3f sp3C;
    f32 temp_f2;
    f32 temp_f2_2;

    temp_f0 = triNorm->plane.normal.x * arg1;
    temp_f2 = triNorm->plane.normal.y * arg1;
    temp_f16 = triNorm->plane.normal.z * arg1;

    sp48.x = arg0->x + temp_f0;
    sp48.y = arg0->y + temp_f2;
    sp48.z = arg0->z + temp_f16;

    sp3C.x = arg0->x - temp_f0;
    sp3C.y = arg0->y - temp_f2;
    sp3C.z = arg0->z - temp_f16;

    if (Math3D_LineSegVsPlane(triNorm->plane.normal.x, triNorm->plane.normal.y, triNorm->plane.normal.z,
                              triNorm->plane.originDist, &sp48, &sp3C, arg3, 0)) {
        temp_f0_2 = Math3D_Vec3fDistSq(arg0, arg3);
        temp_f2_2 = SQ(arg1) - temp_f0_2;
        if (SQ(arg1) < temp_f0_2) {
            return false;
        }
        *arg4 = sqrtf(temp_f2_2);
        return true;
    }
    return false;
}

bool func_80B2FC98(TriNorm* triNorm, Vec3f* arg1) {
    Vec3f sp84;
    Vec3f sp78;
    Vec3f* vtx;
    Vec3f* vtx2;
    Vec3f sp4C[3];
    s32 i;

    if (triNorm->plane.normal.x > 0.5f) {
        vtx = &triNorm->vtx[0];
        vtx2 = &triNorm->vtx[2];

        for (i = 0; i < ARRAY_COUNT(sp4C); i++, vtx = &triNorm->vtx[i]) {
            sp84.x = 0.0f;
            sp84.y = vtx->y - arg1->y;
            sp84.z = vtx->z - arg1->z;

            sp78.x = 0.0f;
            sp78.y = vtx2->y - arg1->y;
            sp78.z = vtx2->z - arg1->z;

            Math3D_Vec3f_Cross(&sp84, &sp78, &sp4C[i]);

            vtx2 = vtx;
        }

        return ((sp4C[0].x >= 0.0f) && (sp4C[1].x >= 0.0f) && (sp4C[2].x >= 0.0f)) ||
               ((sp4C[0].x < 0.0f) && (sp4C[1].x < 0.0f) && (sp4C[2].x < 0.0f));
    } else if (triNorm->plane.normal.y > 0.5f) {
        vtx = &triNorm->vtx[0];
        vtx2 = &triNorm->vtx[2];

        for (i = 0; i < ARRAY_COUNT(sp4C); i++, vtx = &triNorm->vtx[i]) {
            sp84.x = vtx->x - arg1->x;
            sp84.y = 0.0f;
            sp84.z = vtx->z - arg1->z;

            sp78.x = vtx2->x - arg1->x;
            sp78.y = 0.0f;
            sp78.z = vtx2->z - arg1->z;

            Math3D_Vec3f_Cross(&sp84, &sp78, &sp4C[i]);

            vtx2 = vtx;
        }

        return ((sp4C[0].y >= 0.0f) && (sp4C[1].y >= 0.0f) && (sp4C[2].y >= 0.0f)) ||
               ((sp4C[0].y < 0.0f) && (sp4C[1].y < 0.0f) && (sp4C[2].y < 0.0f));
    } else {
        vtx = &triNorm->vtx[0];
        vtx2 = &triNorm->vtx[2];

        for (i = 0; i < ARRAY_COUNT(sp4C); i++, vtx = &triNorm->vtx[i]) {
            sp84.x = vtx->x - arg1->x;
            sp84.y = vtx->y - arg1->y;
            sp84.z = 0.0f;

            sp78.x = vtx2->x - arg1->x;
            sp78.y = vtx2->y - arg1->y;
            sp78.z = 0.0f;

            Math3D_Vec3f_Cross(&sp84, &sp78, &sp4C[i]);

            vtx2 = vtx;
        }

        return ((sp4C[0].z >= 0.0f) && (sp4C[1].z >= 0.0f) && (sp4C[2].z >= 0.0f)) ||
               ((sp4C[0].z < 0.0f) && (sp4C[1].z < 0.0f) && (sp4C[2].z < 0.0f));
    }
}

void func_80B300F4(ObjSpidertent* thisx, PlayState* play, TriNorm* triNorm, Vec3f* arg3, f32 arg4, s32 arg5) {
    ObjSpidertent* this = (ObjSpidertent*)thisx;
    ObjSpidertentStruct* spE0 = &D_80B31350[OBJSPIDERTENT_GET_1(&this->dyna.actor)];
    f32 temp_f24;
    f32 phi_f22;
    s32 i;
    f32 temp_f0;
    Vec3f spC4;
    Vec3f spB8;
    Vec3f spAC;
    Vec3f spA0;
    Vec3f sp94;
    Vec3f sp88;
    f32 temp_f2;
    ObjSpidertentStruct2* sp80 = &D_80B313A0[arg5];

    spAC.x = triNorm->plane.normal.x;
    spAC.y = triNorm->plane.normal.y;
    spAC.z = triNorm->plane.normal.z;

    if (triNorm->plane.normal.y < 0.5f) {
        Math3D_Vec3f_Cross(&spAC, &D_80B3140C, &sp88);
    } else {
        Math3D_Vec3f_Cross(&spAC, &D_80B31400, &sp88);
    }

    if (func_80B2FB10(&sp88, &spA0)) {
        phi_f22 = 0.0f;
        temp_f24 = (2 * M_PIf) / sp80->unk_0F;

        for (i = 0; i < sp80->unk_0F; i++) {
            temp_f2 = (Rand_ZeroOne() * temp_f24) + phi_f22;
            Matrix_RotateAxisF(temp_f2, &spAC, MTXMODE_NEW);
            Matrix_MultVec3f(&spA0, &sp94);

            if (arg5 == 0) {
                spC4.x = (sp94.x * arg4) + arg3->x;
                spC4.y = (sp94.y * arg4) + arg3->y;
                spC4.z = (sp94.z * arg4) + arg3->z;
            } else {
                temp_f0 = Rand_ZeroOne();
                temp_f0 = (1.0f - SQ(temp_f0)) * arg4;
                spC4.x = (sp94.x * temp_f0) + arg3->x;
                spC4.y = (sp94.y * temp_f0) + arg3->y;
                spC4.z = (sp94.z * temp_f0) + arg3->z;
            }

            if ((Math3D_Vec3fDistSq(&spC4, &this->dyna.actor.world.pos) < spE0->unk_1C) &&
                func_80B2FC98(triNorm, &spC4)) {
                spB8.x = sp80->unk_08 * sp94.x;
                spB8.y = sp80->unk_08 * sp94.y;
                spB8.z = sp80->unk_08 * sp94.z;

                EffectSsDeadDb_Spawn(play, &spC4, &spB8, &gZeroVec3f, &sp80->unk_10, &sp80->unk_14, sp80->unk_0C, 0,
                                     sp80->unk_0E);
            }

            phi_f22 += temp_f24;
        }
    }
}

void func_80B30410(ObjSpidertent* this, Vec3f* arg1) {
    s32 i;

    Math_Vec3f_Copy(&this->unk_3A4, arg1);
    this->unk_3C2 = 255;
    this->unk_3C3 = 255;
    this->unk_3C4 = 255;
    this->unk_3C5 = 255;
    for (i = 0; i < ARRAY_COUNT(D_80B313A0); i++) {
        this->unk_3B0[i] = D_80B313A0[i].unk_00;
    }
    this->unk_3C0 = 0;
}

s32 func_80B30480(ObjSpidertent* this, PlayState* play, Vec3f* arg2) {
    Player* player = GET_PLAYER(play);
    TriNorm* triNorm;
    s32 i;
    Vec3f sp58;
    f32 sp54;

    if (player->heldItemAction == PLAYER_IA_DEKU_STICK) {
        if (player->unk_B28 > 0) {
            for (i = 0; i < ARRAY_COUNT(this->colliderElements); i++) {
                triNorm = &this->collider.elements[i].dim;

                if (func_80B2FB94(&player->meleeWeaponInfo[0].tip, 6.0f, triNorm, &sp58, &sp54) &&
                    func_80B2FC98(triNorm, &sp58)) {
                    break;
                }
            }

            if (i < ARRAY_COUNT(this->colliderElements)) {
                Math_Vec3f_Copy(arg2, &player->meleeWeaponInfo[0].tip);
                return true;
            }
        }
    }
    return false;
}

void ObjSpidertent_Init(Actor* thisx, PlayState* play) {
    s32 pad;
    ObjSpidertent* this = (ObjSpidertent*)thisx;
    s32 temp_s1 = OBJSPIDERTENT_GET_1(&this->dyna.actor);
    ObjSpidertentStruct* ptr = &D_80B31350[temp_s1];
    ColliderTrisElementInit* element;
    Vec3f sp70[3];
    Vec3f sp64;
    s32 i;
    s32 j;

    Actor_ProcessInitChain(&this->dyna.actor, sInitChain);
    DynaPolyActor_Init(&this->dyna, 0);
    Collider_InitTris(play, &this->collider);

    if (Flags_GetSwitch(play, OBJSPIDERTENT_GET_SWITCH_FLAG(&this->dyna.actor))) {
        Actor_Kill(&this->dyna.actor);
        return;
    }

    DynaPolyActor_LoadMesh(play, &this->dyna, ptr->unk_04);
    Collider_SetTris(play, &this->collider, &this->dyna.actor, D_80B31350[temp_s1].unk_08, this->colliderElements);
    Matrix_SetTranslateRotateYXZ(this->dyna.actor.world.pos.x, this->dyna.actor.world.pos.y,
                                 this->dyna.actor.world.pos.z, &this->dyna.actor.shape.rot);

    sp64.x = ptr->unk_10 * 1.2f;
    sp64.y = ptr->unk_10 * 1.2f;
    sp64.z = ptr->unk_10 * 1.2f;

    Matrix_MultVec3f(&sp64, &this->dyna.actor.focus.pos);
    Matrix_Translate(ptr->unk_10, ptr->unk_14 + 5.0f, ptr->unk_18, MTXMODE_APPLY);
    Matrix_Scale(ptr->unk_0C, ptr->unk_0C, ptr->unk_0C, MTXMODE_APPLY);
    Matrix_Translate(-ptr->unk_10, -ptr->unk_14, -ptr->unk_18, MTXMODE_APPLY);

    for (i = 0; i < 6; i++) {
        element = &ptr->unk_08->elements[i];

        for (j = 0; j < ARRAY_COUNT(sp70); j++) {
            Matrix_MultVec3f(&element->dim.vtx[j], &sp70[j]);
        }

        Collider_SetTrisVertices(&this->collider, i, &sp70[0], &sp70[1], &sp70[2]);
    }

    func_80B307E0(this);
}

void ObjSpidertent_Destroy(Actor* thisx, PlayState* play) {
    ObjSpidertent* this = (ObjSpidertent*)thisx;

    DynaPoly_DeleteBgActor(play, &play->colCtx.dyna, this->dyna.bgId);
    Collider_DestroyTris(play, &this->collider);
}

void func_80B307E0(ObjSpidertent* this) {
    this->unk_3C2 = 255;
    this->unk_3C3 = 255;
    this->unk_3C4 = 255;
    this->unk_3C5 = 255;
    this->actionFunc = func_80B30808;
}

void func_80B30808(ObjSpidertent* this, PlayState* play) {
    s32 phi_s4;
    s32 i;
    ObjSpidertentStruct* ptr2 = &D_80B31350[OBJSPIDERTENT_GET_1(&this->dyna.actor)];
    Vec3f sp70;
    Vec3s* hitPos;
    ColliderTrisElement* trisElem;
    f32 temp_f0;
    s32 phi_s1;
    Vec3f sp54;
    f32 phi_f20;
    s32 phi_s0 = false;

    if (this->collider.base.acFlags & AC_HIT) {
        Player* player = GET_PLAYER(play);

        this->collider.base.acFlags &= ~AC_HIT;
        phi_s1 = 0;
        phi_s4 = -1;
        phi_f20 = FLT_MAX;

        for (i = 0; i < ARRAY_COUNT(this->colliderElements); i++) {
            trisElem = &this->collider.elements[i];

            if (trisElem->base.acElemFlags & ACELEM_HIT) {
                sp54.x = trisElem->base.acDmgInfo.hitPos.x;
                sp54.y = trisElem->base.acDmgInfo.hitPos.y;
                sp54.z = trisElem->base.acDmgInfo.hitPos.z;

                temp_f0 = Math3D_Vec3fDistSq(&sp54, &player->actor.world.pos);
                if (temp_f0 < phi_f20) {
                    phi_f20 = temp_f0;
                    phi_s4 = i;
                }
            }
        }

        if (phi_s4 >= 0) {
            hitPos = &this->collider.elements[phi_s4].base.acDmgInfo.hitPos;

            sp70.x = hitPos->x;
            sp70.y = hitPos->y;
            sp70.z = hitPos->z;
        } else {
            Math_Vec3f_Copy(&sp70, &this->dyna.actor.world.pos);
        }
        phi_s0 = true;
    } else if ((this->dyna.actor.xzDistToPlayer < ptr2->unk_24) && func_80B30480(this, play, &sp70)) {
        phi_s0 = true;
    }

    if (phi_s0) {
        func_80B30410(this, &sp70);
        func_80B30A2C(this);
    } else {
        CollisionCheck_SetAC(play, &play->colChkCtx, &this->collider.base);
    }
}

void func_80B30A2C(ObjSpidertent* this) {
    this->dyna.actor.flags |= ACTOR_FLAG_UPDATE_CULLING_DISABLED;
    this->actionFunc = func_80B30A4C;
}

void func_80B30A4C(ObjSpidertent* this, PlayState* play) {
    if (CutsceneManager_IsNext(this->dyna.actor.csId)) {
        CutsceneManager_StartWithPlayerCs(this->dyna.actor.csId, &this->dyna.actor);
        if (this->dyna.actor.csId > CS_ID_NONE) {
            Player_SetCsActionWithHaltedActors(play, &this->dyna.actor, PLAYER_CSACTION_1);
        }
        Flags_SetSwitch(play, OBJSPIDERTENT_GET_SWITCH_FLAG(&this->dyna.actor));
        func_80B30AD4(this);
    } else {
        CutsceneManager_Queue(this->dyna.actor.csId);
    }
}

void func_80B30AD4(ObjSpidertent* this) {
    this->unk_3C1 = 80;
    this->unk_3C6 = 0;
    this->unk_3C7 = 0;
    this->actionFunc = func_80B30AF8;
}

void func_80B30AF8(ObjSpidertent* this, PlayState* play) {
    ObjSpidertentStruct* temp_s0 = &D_80B31350[OBJSPIDERTENT_GET_1(&this->dyna.actor)];
    s32 i;
    s32 j;
    s32 pad;

    this->unk_3C6++;

    if (this->unk_3C6 < 40) {
        if (this->unk_3C2 > 2) {
            this->unk_3C2 -= 2;
        } else {
            this->unk_3C2 = 0;
        }

        if (this->unk_3C3 > 5) {
            this->unk_3C3 -= 5;
        } else {
            this->unk_3C3 = 0;
        }

        if (this->unk_3C4 > 5) {
            this->unk_3C4 -= 5;
        } else {
            this->unk_3C4 = 0;
        }

        if (this->unk_3C5 > 1) {
            this->unk_3C5--;
        } else {
            this->unk_3C5 = 0;
        }
    } else {
        if (this->unk_3C2 > 4) {
            this->unk_3C2 -= 4;
        } else {
            this->unk_3C2 = 0;
        }

        if (this->unk_3C3 < 255) {
            this->unk_3C3++;
        } else {
            this->unk_3C3 = 255;
        }

        if (this->unk_3C4 < 255) {
            this->unk_3C4++;
        } else {
            this->unk_3C4 = 255;
        }

        if (this->unk_3C5 > 10) {
            this->unk_3C5 -= 10;
        } else {
            this->unk_3C5 = 0;
        }
    }

    this->unk_3C1--;
    if (this->unk_3C1 == 40) {
        DynaPoly_DisableCollision(play, &play->colCtx.dyna, this->dyna.bgId);
    }

    if (this->unk_3C1 >= 32) {
        if (this->unk_3C7 > 0) {
            this->unk_3C7--;
        } else if (this->unk_3C1 > 50) {
            Actor_PlaySfx(&this->dyna.actor, NA_SE_EN_EXTINCT);
            this->unk_3C7 = Rand_S16Offset(2, 2);
        } else {
            SoundSource_PlaySfxAtFixedWorldPos(play, &this->dyna.actor.world.pos, 11, NA_SE_EN_EXTINCT);
            this->unk_3C7 = Rand_S16Offset(2, 4);
        }

        for (j = 0; j < ARRAY_COUNT(this->unk_3B0); j++) {
            this->unk_3B0[j] += D_80B313A0[j].unk_04;
            if (temp_s0->unk_20 < this->unk_3B0[j]) {
                this->unk_3C0 |= (1 << j);
            }
        }

        for (i = 0; i < ARRAY_COUNT(this->colliderElements); i++) {
            TriNorm* triNorm = &this->collider.elements[i].dim;
            Vec3f sp60;
            f32 sp5C;

            for (j = 0; j < ARRAY_COUNT(this->unk_3B0); j++) {
                if ((this->unk_3B0[j] < 5.0f) || (this->unk_3C0 & (1 << j))) {
                    continue;
                }
                if (func_80B2FB94(&this->unk_3A4, this->unk_3B0[j], triNorm, &sp60, &sp5C)) {
                    func_80B300F4(this, play, triNorm, &sp60, sp5C, j);
                }
            }
        }
    } else if (this->unk_3C1 <= 0) {
        CutsceneManager_Stop(this->dyna.actor.csId);
        Actor_Kill(&this->dyna.actor);
    }
}

void ObjSpidertent_Update(Actor* thisx, PlayState* play) {
    ObjSpidertent* this = (ObjSpidertent*)thisx;

    this->actionFunc(this, play);
}

void ObjSpidertent_Draw(Actor* thisx, PlayState* play) {
    ObjSpidertent* this = (ObjSpidertent*)thisx;
    s32 params = OBJSPIDERTENT_GET_1(&this->dyna.actor);
    s32 temp_f18 = this->unk_3C5 * (29.0f / 51);
    Gfx* gfx;

    OPEN_DISPS(play->state.gfxCtx);

    gfx = POLY_XLU_DISP;

    gSPDisplayList(gfx++, gSetupDLs[SETUPDL_25]);
    MATRIX_FINALIZE_AND_LOAD(gfx++, play->state.gfxCtx);
    gDPSetPrimColor(gfx++, 0, 0xFF, this->unk_3C2, this->unk_3C3, this->unk_3C4, temp_f18);
    gSPDisplayList(gfx++, D_80B31350[params].unk_00);

    POLY_XLU_DISP = gfx;
    CLOSE_DISPS(play->state.gfxCtx);
}
