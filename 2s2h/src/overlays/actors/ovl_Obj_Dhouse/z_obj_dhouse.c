/*
 * File: z_obj_dhouse.c
 * Overlay: ovl_Obj_Dhouse
 * Description: Stone bridge that Keeta sleeps under
 */

#include "z_obj_dhouse.h"
#include "z64quake.h"
#include "objects/object_dhouse/object_dhouse.h"

#define FLAGS (ACTOR_FLAG_UPDATE_CULLING_DISABLED | ACTOR_FLAG_IGNORE_LEGACY_POINT_LIGHTS)

void ObjDhouse_Init(Actor* thisx, PlayState* play);
void ObjDhouse_Destroy(Actor* thisx, PlayState* play);
void ObjDhouse_Update(Actor* thisx, PlayState* play);
void ObjDhouse_Draw(Actor* thisx, PlayState* play);

void func_80B12E7C(ObjDhouse* this, PlayState* play, ObjDhouseStruct1* ptr, ObjDhouseStruct3* ptr3);
void func_80B13170(ObjDhouse* this, PlayState* play, ObjDhouseStruct1* ptr, ObjDhouseStruct3* ptr3);
void func_80B13908(ObjDhouse* this);
void func_80B1391C(ObjDhouse* this, PlayState* play);
void func_80B1392C(ObjDhouse* this);
void func_80B13940(ObjDhouse* this, PlayState* play2);
void func_80B139D8(ObjDhouse* this);
void func_80B139F4(ObjDhouse* this, PlayState* play);
void func_80B13C08(Actor* thisx, PlayState* play);
void func_80B13E40(Actor* thisx, PlayState* play);

ActorProfile Obj_Dhouse_Profile = {
    /**/ ACTOR_OBJ_DHOUSE,
    /**/ ACTORCAT_BG,
    /**/ FLAGS,
    /**/ OBJECT_DHOUSE,
    /**/ sizeof(ObjDhouse),
    /**/ ObjDhouse_Init,
    /**/ ObjDhouse_Destroy,
    /**/ ObjDhouse_Update,
    /**/ ObjDhouse_Draw,
};

ObjDhouseStruct3 D_80B13E90[] = {
    {
        object_dhouse_DL_006E28,
        object_dhouse_DL_006CE0,
        { 84.2f, 30.480001f, 109.68001f },
        { 3.0f, 21.0f, 11.0f },
        5500,
        500,
        -500,
        7,
    },
    {
        object_dhouse_DL_0070A8,
        object_dhouse_DL_006EF8,
        { 40.8f, -38.72f, 62.12f },
        { -5.0f, 22.0f, 8.0f },
        600,
        -300,
        2000,
        4,
    },
    {
        object_dhouse_DL_007348,
        object_dhouse_DL_007190,
        { 125.31999f, -34.52f, 62.12f },
        { 8.0f, 13.0f, 6.0f },
        -2400,
        200,
        -300,
        4,
    },
    {
        object_dhouse_DL_007568,
        object_dhouse_DL_007438,
        { 150.56f, -80.520004f, 62.12f },
        { -4.0f, 7.0f, 3.0f },
        100,
        50,
        850,
        8,
    },
    {
        object_dhouse_DL_007780,
        object_dhouse_DL_007638,
        { 82.08f, 21.32f, -99.64f },
        { -7.0f, 34.0f, -1.0f },
        -2400,
        -100,
        1000,
        5,
    },
    {
        object_dhouse_DL_007A00,
        object_dhouse_DL_007850,
        { 45.64f, -38.16f, -56.24f },
        { -1.0f, 20.0f, -16.0f },
        -2000,
        700,
        500,
        6,
    },
    {
        object_dhouse_DL_007C98,
        object_dhouse_DL_007AE8,
        { 140.4f, -60.84f, -56.24f },
        { 2.0f, 27.0f, -10.0f },
        -1000,
        -500,
        -500,
        9,
    },
};

Vec3f D_80B13FA8 = { 0.0f, 0.3f, 0.0f };

static InitChainEntry sInitChain[] = {
    ICHAIN_F32(cullingVolumeDistance, 8000, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeScale, 1000, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeDownward, 800, ICHAIN_CONTINUE),
    ICHAIN_VEC3F_DIV1000(scale, 100, ICHAIN_STOP),
};

Vec3f D_80B13FC4 = { 160.0f, 0.0f, 240.0f };

void ObjDhouse_Init(Actor* thisx, PlayState* play) {
    ObjDhouse* this = (ObjDhouse*)thisx;

    Actor_ProcessInitChain(&this->dyna.actor, sInitChain);

    DynaPolyActor_Init(&this->dyna, 0);

    if (Flags_GetSwitch(play, OBJDHOUSE_GET_SWITCH_FLAG(&this->dyna.actor))) {
        this->dyna.actor.draw = func_80B13E40;
        func_80B13908(this);
    } else {
        DynaPolyActor_LoadMesh(play, &this->dyna, &object_dhouse_Colheader_008040);
        this->dyna.actor.flags |= ACTOR_FLAG_UPDATE_CULLING_DISABLED;
        func_80B1392C(this);
    }
}

void ObjDhouse_Destroy(Actor* thisx, PlayState* play) {
    ObjDhouse* this = (ObjDhouse*)thisx;

    DynaPoly_DeleteBgActor(play, &play->colCtx.dyna, this->dyna.bgId);
}

void func_80B12A50(ObjDhouseStruct1* this, ObjDhouseStruct3* ptr3, Vec3f* arg2) {
    arg2->x = this->unk_00.x + ptr3->unk_08.x;
    arg2->y = this->unk_00.y + ptr3->unk_08.y;
    arg2->z = this->unk_00.z + ptr3->unk_08.z;
}

void func_80B12A88(Actor* thisx) {
    ObjDhouse* this = (ObjDhouse*)thisx;
    s32 i;
    ObjDhouseStruct1* ptr;
    ObjDhouseStruct3* ptr3;

    for (i = 0, ptr = &this->unk_160[0], ptr3 = &D_80B13E90[0]; i < ARRAY_COUNT(this->unk_160); i++, ptr3++, ptr++) {
        Math_Vec3f_Copy(&ptr->unk_00, &this->dyna.actor.world.pos);
        Math_Vec3f_Copy(&ptr->unk_0C, &ptr3->unk_14);
        ptr->unk_18 = this->dyna.actor.shape.rot;
        ptr->unk_1E = 0;
    }
}

void func_80B12B38(ObjDhouse* this, PlayState* play) {
    Actor* thisx = &this->dyna.actor;
    s32 i;
    f32 temp_f0;
    CollisionPoly* sp90;
    s32 phi_s3;
    Vec3f sp80;
    s32 bgId;
    ObjDhouseStruct1* ptr;
    ObjDhouseStruct3* ptr3;

    for (i = 0, ptr = &this->unk_160[0], ptr3 = &D_80B13E90[0]; i < ARRAY_COUNT(this->unk_160); i++, ptr3++, ptr++) {
        if (ptr->unk_1E >= 0) {
            if (ptr->unk_1E >= ptr3->unk_26) {
                ptr->unk_0C.y -= 2.5f;

                Math_Vec3f_Scale(&ptr->unk_0C, 0.96f);

                ptr->unk_00.x += ptr->unk_0C.x;
                ptr->unk_00.y += ptr->unk_0C.y;
                ptr->unk_00.z += ptr->unk_0C.z;

                ptr->unk_18.x += ptr3->unk_20.x;
                ptr->unk_18.y += ptr3->unk_20.y;
                ptr->unk_18.z += ptr3->unk_20.z;

                if (ptr->unk_1E == ptr3->unk_26) {
                    func_80B12E7C(this, play, ptr, ptr3);
                }
            }

            phi_s3 = false;
            if (ptr->unk_0C.y < 0.0f) {
                func_80B12A50(ptr, ptr3, &sp80);
                temp_f0 = BgCheck_EntityRaycastFloor5(&play->colCtx, &sp90, &bgId, thisx, &sp80);
                if (((sp80.y - 35.0f) < temp_f0) && (temp_f0 > (BGCHECK_Y_MIN + 1.0f))) {
                    phi_s3 = true;
                }
            }

            if (phi_s3) {
                ptr->unk_1E = -1;
                func_80B13170(this, play, ptr, ptr3);
            } else if (ptr->unk_1E > 60) {
                ptr->unk_1E = -1;
            } else {
                ptr->unk_1E++;
            }
        }
    }
}

s32 func_80B12D78(ObjDhouse* this) {
    s32 i;
    ObjDhouseStruct2* ptr2 = &this->unk_240[0];
    s32 count = 0;
    ObjDhouseStruct2* ptr22 = &this->unk_240[1];

    for (i = 1; i < ARRAY_COUNT(this->unk_240); i++, ptr22++) {
        if (ptr22->unk_28 < ptr2->unk_28) {
            ptr2 = ptr22;
            count = i;
        }
    }

    return count;
}

void func_80B12E7C(ObjDhouse* this, PlayState* play, ObjDhouseStruct1* ptr, ObjDhouseStruct3* ptr3) {
    s32 i;
    Vec3f spA0;
    Vec3f sp94;
    Vec3f sp88;
    ObjDhouseStruct2* ptr2;

    Matrix_RotateYS(this->dyna.actor.shape.rot.y, MTXMODE_NEW);
    Matrix_MultVec3fXZ(&D_80B13FC4, &sp94);
    sp94.y = 100.0f;
    func_80B12A50(ptr, ptr3, &spA0);

    for (i = 0; i < 15; i++) {
        ptr2 = &this->unk_240[func_80B12D78(this)];

        ptr2->unk_00.x = Rand_ZeroOne() - 0.5f;
        ptr2->unk_00.y = Rand_ZeroOne();
        ptr2->unk_00.z = Rand_ZeroOne() - 0.5f;

        ptr2->unk_0C.x = ptr2->unk_00.x * 24.0f;
        ptr2->unk_0C.y = ptr2->unk_00.y * 34.0f;
        ptr2->unk_0C.z = ptr2->unk_00.z * 24.0f;

        ptr2->unk_00.x = (ptr2->unk_00.x * 140.0f) + spA0.x;
        ptr2->unk_00.y = ((ptr2->unk_00.y - 0.2f) * 50.0f) + spA0.y;
        ptr2->unk_00.z = (ptr2->unk_00.z * 140.0f) + spA0.z;

        ptr2->unk_18 = (Rand_ZeroOne() * 0.057f) + 0.003f;

        ptr2->unk_1C.x = Rand_Next() >> 0x10;
        ptr2->unk_1C.y = Rand_Next() >> 0x10;
        ptr2->unk_1C.z = Rand_Next() >> 0x10;

        ptr2->unk_22 = (Rand_Next() >> 0x11) - 0x3FFF;
        ptr2->unk_24 = (Rand_Next() >> 0x13) - 0xFFF;
        ptr2->unk_26 = (Rand_Next() >> 0x12) - 0x1FFF;
        ptr2->unk_29 = 0;
        ptr2->unk_28 = 40;

        if ((i % 2) != 0) {
            sp88.x = ((Rand_ZeroOne() - 0.5f) * sp94.x) + ptr2->unk_00.x;
            sp88.y = ((Rand_ZeroOne() - 0.3f) * sp94.y) + ptr2->unk_00.y;
            sp88.z = ((Rand_ZeroOne() - 0.5f) * sp94.z) + ptr2->unk_00.z;

            func_800B1210(play, &sp88, &gZeroVec3f, &D_80B13FA8, (s32)(Rand_ZeroOne() * 130.0f) + 20,
                          (s32)(Rand_ZeroOne() * 140.0f) + 60);
        }
    }
}

void func_80B13170(ObjDhouse* this, PlayState* play, ObjDhouseStruct1* ptr, ObjDhouseStruct3* ptr3) {
    s32 i;
    Vec3f sp98;
    Vec3f sp8C;
    ObjDhouseStruct2* ptr2;

    func_80B12A50(ptr, ptr3, &sp98);

    for (i = 0; i < 11; i++) {
        ptr2 = &this->unk_240[func_80B12D78(this)];

        ptr2->unk_00.x = Rand_ZeroOne() - 0.5f;
        ptr2->unk_00.y = Rand_ZeroOne();
        ptr2->unk_00.z = Rand_ZeroOne() - 0.5f;

        ptr2->unk_0C.x = ptr2->unk_00.x * 23.0f;
        ptr2->unk_0C.y = (ptr2->unk_00.y * 19.0f) + 5.0f;
        ptr2->unk_0C.z = ptr2->unk_00.z * 23.0f;

        ptr2->unk_00.x = (ptr2->unk_00.x * 80.0f) + sp98.x;
        ptr2->unk_00.y = (ptr2->unk_00.y * 23.0f) + sp98.y + 15.0f;
        ptr2->unk_00.z = (ptr2->unk_00.z * 80.0f) + sp98.z;

        ptr2->unk_1C.x = Rand_Next() >> 0x10;
        ptr2->unk_1C.y = Rand_Next() >> 0x10;
        ptr2->unk_1C.z = Rand_Next() >> 0x10;

        ptr2->unk_22 = (Rand_Next() >> 0x12) - 0x1FFF;
        ptr2->unk_24 = (Rand_Next() >> 0x13) - 0xFFF;
        ptr2->unk_26 = (Rand_Next() >> 0x12) - 0x1FFF;

        ptr2->unk_28 = 40;

        if (i < 3) {
            ptr2->unk_18 = (Rand_ZeroOne() * 0.06f) + 0.12f;
            ptr2->unk_29 = 1;
        } else {
            ptr2->unk_18 = (Rand_ZeroOne() * 0.05f) + 0.003f;
            ptr2->unk_29 = 0;
        }

        if ((i % 2) != 0) {
            sp8C.x = ((Rand_ZeroOne() - 0.5f) * 160.0f) + ptr2->unk_00.x;
            sp8C.y = (Rand_ZeroOne() * 120.0f) + ptr2->unk_00.y;
            sp8C.z = ((Rand_ZeroOne() - 0.5f) * 160.0f) + ptr2->unk_00.z;

            func_800B1210(play, &sp8C, &gZeroVec3f, &D_80B13FA8, (s32)(Rand_ZeroOne() * 140.0f) + 20,
                          (s32)(Rand_ZeroOne() * 140.0f) + 40);
        }
    }
}

void func_80B13474(ObjDhouse* this, PlayState* play, Vec3f* arg2) {
    s32 i;
    Vec3f sp88;
    ObjDhouseStruct2* ptr2;

    for (i = 0; i < 5; i++) {
        ptr2 = &this->unk_240[func_80B12D78(this)];

        ptr2->unk_00.x = Rand_ZeroOne() - 0.5f;
        ptr2->unk_00.y = Rand_ZeroOne();
        ptr2->unk_00.z = Rand_ZeroOne() - 0.5f;

        ptr2->unk_0C.x = ptr2->unk_00.x * 18.0f;
        ptr2->unk_0C.y = ptr2->unk_00.y * 23.0f;
        ptr2->unk_0C.z = ptr2->unk_00.z * 18.0f;

        ptr2->unk_00.x = (ptr2->unk_00.x * 40.0f) + arg2->x;
        ptr2->unk_00.y = ((ptr2->unk_00.y - 0.4f) * 20.0f) + arg2->y;
        ptr2->unk_00.z = (ptr2->unk_00.z * 40.0f) + arg2->z;

        ptr2->unk_1C.x = Rand_Next() >> 0x10;
        ptr2->unk_1C.y = Rand_Next() >> 0x10;
        ptr2->unk_1C.z = Rand_Next() >> 0x10;

        ptr2->unk_22 = (Rand_Next() >> 0x12) - 0x1FFF;
        ptr2->unk_24 = (Rand_Next() >> 0x13) - 0xFFF;
        ptr2->unk_26 = (Rand_Next() >> 0x12) - 0x1FFF;

        ptr2->unk_28 = 40;
        ptr2->unk_18 = (Rand_ZeroOne() * 0.07f) + 0.003f;
        ptr2->unk_29 = 0;

        sp88.x = ((Rand_ZeroOne() * 70.0f) - 35.0f) + ptr2->unk_00.x;
        sp88.y = (Rand_ZeroOne() * 60.0f) + ptr2->unk_00.y;
        sp88.z = ((Rand_ZeroOne() * 70.0f) - 35.0f) + ptr2->unk_00.z;

        func_800B1210(play, &sp88, &gZeroVec3f, &D_80B13FA8, (s32)(Rand_ZeroOne() * 80.0f) + 20,
                      (s32)(Rand_ZeroOne() * 90.0f) + 40);
    }
}

void func_80B13724(ObjDhouse* this, PlayState* play) {
    f32 temp_f0;
    f32 temp_f20 = this->dyna.actor.home.pos.y - 300.0f;
    s32 i;
    CollisionPoly* sp80;
    ObjDhouseStruct2* ptr2;
    Vec3f sp70;
    s32 bgId;

    for (i = 0, ptr2 = &this->unk_240[0]; i < ARRAY_COUNT(this->unk_240); i++, ptr2++) {
        if (ptr2->unk_28 > 0) {
            ptr2->unk_0C.y -= 2.2f;

            Math_Vec3f_Scale(&ptr2->unk_0C, 0.95f);

            ptr2->unk_00.x += ptr2->unk_0C.x;
            ptr2->unk_00.y += ptr2->unk_0C.y;
            ptr2->unk_00.z += ptr2->unk_0C.z;

            if (ptr2->unk_00.y < temp_f20) {
                ptr2->unk_28 = 0;
            } else {
                ptr2->unk_1C.x += ptr2->unk_22;
                ptr2->unk_1C.y += ptr2->unk_24;
                ptr2->unk_1C.z += ptr2->unk_26;
                ptr2->unk_28--;

                if (ptr2->unk_29 & 1) {
                    temp_f0 =
                        BgCheck_EntityRaycastFloor5(&play->colCtx, &sp80, &bgId, &this->dyna.actor, &ptr2->unk_00);
                    if (((ptr2->unk_00.y - 20.0f) < temp_f0) && (temp_f0 > BGCHECK_Y_MIN + 1.0f)) {
                        Math_Vec3f_Copy(&sp70, &ptr2->unk_00);
                        ptr2->unk_28 = 0;
                        func_80B13474(this, play, &sp70);
                    }
                }
            }
        }
    }
}

void func_80B13908(ObjDhouse* this) {
    this->actionFunc = func_80B1391C;
}

void func_80B1391C(ObjDhouse* this, PlayState* play) {
}

void func_80B1392C(ObjDhouse* this) {
    this->actionFunc = func_80B13940;
}

void func_80B13940(ObjDhouse* this, PlayState* play2) {
    PlayState* play = play2;
    s32 sp20 = false;

    if (Flags_GetSwitch(play, OBJDHOUSE_GET_SWITCH_FLAG(&this->dyna.actor))) {
        sp20 = true;
        Actor_PlaySfx(&this->dyna.actor, NA_SE_EV_EXPLSION_LONG);
    }

    if (sp20) {
        func_80B12A88(&this->dyna.actor);
        DynaPoly_DisableCollision(play, &play->colCtx.dyna, this->dyna.bgId);
        this->dyna.actor.draw = func_80B13C08;
        this->dyna.actor.flags |= ACTOR_FLAG_DRAW_CULLING_DISABLED;
        func_80B139D8(this);
    }
}

void func_80B139D8(ObjDhouse* this) {
    this->unk_1370 = 120;
    this->actionFunc = func_80B139F4;
}

void func_80B139F4(ObjDhouse* this, PlayState* play) {
    Camera* camera = GET_ACTIVE_CAM(play);
    s16 quakeIndex;

    if (this->unk_1370 == 117) {
        quakeIndex = Quake_Request(camera, QUAKE_TYPE_3);
        Quake_SetSpeed(quakeIndex, 20000);
        Quake_SetPerturbations(quakeIndex, 8, 0, 0, 0);
        Quake_SetDuration(quakeIndex, 17);
    } else if (this->unk_1370 == 105) {
        quakeIndex = Quake_Request(camera, QUAKE_TYPE_3);
        Quake_SetSpeed(quakeIndex, 20000);
        Quake_SetPerturbations(quakeIndex, 7, 0, 0, 0);
        Quake_SetDuration(quakeIndex, 20);
    } else if (this->unk_1370 == 90) {
        quakeIndex = Quake_Request(camera, QUAKE_TYPE_3);
        Quake_SetSpeed(quakeIndex, 20000);
        Quake_SetPerturbations(quakeIndex, 5, 0, 0, 0);
        Quake_SetDuration(quakeIndex, 62);
    }

    this->unk_1370--;
    if (this->unk_1370 <= 0) {
        this->dyna.actor.draw = func_80B13E40;
        this->dyna.actor.flags &= ~ACTOR_FLAG_DRAW_CULLING_DISABLED;
        this->dyna.actor.flags &= ~ACTOR_FLAG_UPDATE_CULLING_DISABLED;
        func_80B13908(this);
    } else {
        func_80B12B38(this, play);
        func_80B13724(this, play);
    }
}

void ObjDhouse_Update(Actor* thisx, PlayState* play) {
    ObjDhouse* this = (ObjDhouse*)thisx;

    this->actionFunc(this, play);
}

void ObjDhouse_Draw(Actor* thisx, PlayState* play) {
    Gfx_DrawDListOpa(play, object_dhouse_DL_005A78);
}

void func_80B13C08(Actor* thisx, PlayState* play) {
    ObjDhouse* this = (ObjDhouse*)thisx;
    ObjDhouseStruct1* ptr;
    ObjDhouseStruct2* ptr2;
    ObjDhouseStruct3* ptr3;
    s32 i;

    OPEN_DISPS(play->state.gfxCtx);

    Gfx_SetupDL25_Opa(play->state.gfxCtx);

    MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx);
    gSPDisplayList(POLY_OPA_DISP++, object_dhouse_DL_004928);

    for (i = 0, ptr = &this->unk_160[0], ptr3 = &D_80B13E90[0]; i < ARRAY_COUNT(this->unk_160); i++, ptr3++, ptr++) {
        if (ptr->unk_1E >= 0) {
            Matrix_SetTranslateRotateYXZ(ptr->unk_00.x + ptr3->unk_08.x, ptr->unk_00.y + ptr3->unk_08.y,
                                         ptr->unk_00.z + ptr3->unk_08.z, &ptr->unk_18);
            Matrix_Translate(-ptr3->unk_08.x, -ptr3->unk_08.y, -ptr3->unk_08.z, MTXMODE_APPLY);
            Matrix_Scale(0.1f, 0.1f, 0.1f, MTXMODE_APPLY);

            MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx);
            gSPDisplayList(POLY_OPA_DISP++, ptr3->unk_00);
            gSPDisplayList(POLY_OPA_DISP++, ptr3->unk_04);
        }
    }

    for (i = 0, ptr2 = &this->unk_240[0]; i < ARRAY_COUNT(this->unk_240); i++, ptr2++) {
        if (ptr2->unk_28 > 0) {
            Matrix_SetTranslateRotateYXZ(ptr2->unk_00.x, ptr2->unk_00.y, ptr2->unk_00.z, &ptr2->unk_1C);
            Matrix_Scale(ptr2->unk_18, ptr2->unk_18, ptr2->unk_18, MTXMODE_APPLY);

            MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx);
            gSPDisplayList(POLY_OPA_DISP++, object_dhouse_DL_0081D8);
        }
    }

    CLOSE_DISPS(play->state.gfxCtx);
}

void func_80B13E40(Actor* thisx, PlayState* play) {
    Gfx_DrawDListOpa(play, object_dhouse_DL_004928);
}
