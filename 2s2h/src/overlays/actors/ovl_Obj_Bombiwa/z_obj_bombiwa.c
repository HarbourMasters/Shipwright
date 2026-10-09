/*
 * File: z_obj_bombiwa.c
 * Overlay: ovl_Obj_Bombiwa
 * Description: Bombable boulder
 */

#include "z_obj_bombiwa.h"
#include "objects/object_bombiwa/object_bombiwa.h"

#include "2s2h/ShipUtils.h"

#define FLAGS 0x00000000

void ObjBombiwa_Init(Actor* thisx, PlayState* play);
void ObjBombiwa_Destroy(Actor* thisx, PlayState* play2);
void ObjBombiwa_Update(Actor* thisx, PlayState* play);

s32 func_809393B0(Actor* thisx);
s32 func_80939470(Actor* thisx);
void func_80939EE0(ObjBombiwa* this);
void func_80939EF4(ObjBombiwa* this, PlayState* play);
void func_8093A080(ObjBombiwa* this);
void func_8093A1F0(ObjBombiwa* this, PlayState* play);
void func_8093A418(Actor* thisx, PlayState* play);
void func_8093A608(Actor* thisx, PlayState* play);

ActorProfile Obj_Bombiwa_Profile = {
    /**/ ACTOR_OBJ_BOMBIWA,
    /**/ ACTORCAT_PROP,
    /**/ FLAGS,
    /**/ OBJECT_BOMBIWA,
    /**/ sizeof(ObjBombiwa),
    /**/ ObjBombiwa_Init,
    /**/ ObjBombiwa_Destroy,
    /**/ ObjBombiwa_Update,
    /**/ NULL,
};

static ColliderCylinderInit sCylinderInit1 = {
    {
        COL_MATERIAL_HARD,
        AT_NONE,
        AC_ON | AC_HARD | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_2,
        COLSHAPE_CYLINDER,
    },
    {
        ELEM_MATERIAL_UNK0,
        { 0x00000000, 0x00, 0x00 },
        { 0x81C37FBE, 0x00, 0x00 },
        ATELEM_NONE | ATELEM_SFX_NORMAL,
        ACELEM_ON,
        OCELEM_ON,
    },
    { 55, 70, 0, { 0, 0, 0 } },
};

static ColliderCylinderInit sCylinderInit2 = {
    {
        COL_MATERIAL_HARD,
        AT_NONE,
        AC_ON | AC_HARD | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_2,
        COLSHAPE_CYLINDER,
    },
    {
        ELEM_MATERIAL_UNK0,
        { 0x00000000, 0x00, 0x00 },
        { 0x81C37BBE, 0x00, 0x00 },
        ATELEM_NONE | ATELEM_SFX_NORMAL,
        ACELEM_ON,
        OCELEM_ON,
    },
    { 75, 130, 0, { 0, 0, 0 } },
};

typedef struct {
    /* 0x0 */ ColliderCylinderInit* collider;
    /* 0x4 */ ActorShadowFunc unk_04;
    /* 0x8 */ ActorFunc unk_08;
    /* 0xC */ s32 (*unk_0C)(Actor*);
} ObjBombiwaStruct2; // size = 0x10

static ObjBombiwaStruct2 D_8093A998[] = {
    { &sCylinderInit1, ActorShadow_DrawCircle, func_8093A418, func_809393B0 },
    { &sCylinderInit2, NULL, func_8093A608, func_80939470 },
};

static CollisionCheckInfoInit sColChkInfoInit = { 0, 12, 60, MASS_IMMOVABLE };

static InitChainEntry sInitChain[] = {
    ICHAIN_VEC3F_DIV1000(scale, 100, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeDistance, 2000, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeScale, 400, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeDownward, 500, ICHAIN_STOP),
};

static s16 D_8093A9D0[] = {
    17, 14, 10, 8, 7, 5, 3, 2,
};

static s16 D_8093A9E0[] = {
    16, 12, 8, 6, 5, 4, 3, 2,
};

s32 func_809393B0(Actor* thisx) {
    ObjBombiwa* this = (ObjBombiwa*)thisx;

    if (this->collider.base.acFlags & AC_HIT) {
        Actor* ac = this->collider.base.ac;

        if (this->collider.elem.acHitElem->atDmgInfo.dmgFlags & 0x80000000) {
            if ((ac != NULL) && (Math3D_Vec3fDistSq(&this->actor.world.pos, &ac->world.pos) < SQ(150.0f))) {
                return true;
            }
        } else if (this->collider.elem.acHitElem->atDmgInfo.dmgFlags & 8) {
            if ((ac != NULL) && (Math3D_Vec3fDistSq(&this->actor.world.pos, &ac->world.pos) < SQ(95.0f))) {
                return true;
            }
        } else if (this->collider.elem.acHitElem->atDmgInfo.dmgFlags & 0x500) {
            return true;
        }
    }
    return false;
}

s32 func_80939470(Actor* thisx) {
    ObjBombiwa* this = (ObjBombiwa*)thisx;

    if (this->collider.base.acFlags & AC_HIT) {
        Actor* temp_v0 = this->collider.base.ac;

        if (temp_v0 != NULL) {
            if (this->collider.elem.acHitElem->atDmgInfo.dmgFlags & 0x80000000) {
                if (Math3D_Vec3fDistSq(&this->actor.world.pos, &temp_v0->world.pos) < SQ(175.0f)) {
                    return true;
                }
            } else if ((this->collider.elem.acHitElem->atDmgInfo.dmgFlags & 8) &&
                       (Math3D_Vec3fDistSq(&this->actor.world.pos, &temp_v0->world.pos) < SQ(115.0f))) {
                return true;
            }
        }
    }
    return false;
}

s32 ObjBombiwa_IsUnderwater(ObjBombiwa* this, PlayState* play) {
    s32 pad;
    WaterBox* waterBox;
    f32 waterSurface;
    s32 bgId;

    if (WaterBox_GetSurfaceImpl(play, &play->colCtx, this->actor.world.pos.x, this->actor.world.pos.z, &waterSurface,
                                &waterBox, &bgId) &&
        (this->actor.world.pos.y < waterSurface)) {
        return true;
    }
    return false;
}

void func_80939594(ObjBombiwa* this, PlayState* play) {
    s32 pad;
    Vec3f sp28;
    s32 bgId;

    sp28.x = this->actor.world.pos.x;
    sp28.y = this->actor.world.pos.y + 30.0f;
    sp28.z = this->actor.world.pos.z;

    this->actor.floorHeight =
        BgCheck_EntityRaycastFloor5(&play->colCtx, &this->actor.floorPoly, &bgId, &this->actor, &sp28);
}

void ObjBombiwa_Init(Actor* thisx, PlayState* play) {
    s32 pad;
    ObjBombiwa* this = (ObjBombiwa*)thisx;
    s32 sp34 = OBJBOMBIWA_GET_100(&this->actor);
    s32 pad2;

    Actor_ProcessInitChain(&this->actor, sInitChain);
    Collider_InitCylinder(play, &this->collider);

    if (Flags_GetSwitch(play, OBJBOMBIWA_GET_SWITCH_FLAG(&this->actor))) {
        Actor_Kill(&this->actor);
        return;
    }

    Collider_SetCylinder(play, &this->collider, &this->actor, D_8093A998[sp34].collider);
    Collider_UpdateCylinder(&this->actor, &this->collider);
    CollisionCheck_SetInfo(&this->actor.colChkInfo, NULL, &sColChkInfoInit);

    if (sp34 == OBJBOMBIWA_100_0) {
        if (this->actor.shape.rot.y == 0) {
            this->actor.shape.rot.y = this->actor.world.rot.y = Rand_Next() >> 0x10;
        }
        func_80939594(this, play);
    }
    ActorShape_Init(&this->actor.shape, -200.0f, D_8093A998[sp34].unk_04, 9.8f);
    this->actor.world.pos.y = this->actor.home.pos.y + 20.0f;
    this->actor.draw = D_8093A998[sp34].unk_08;
    if (ObjBombiwa_IsUnderwater(this, play)) {
        this->unk_203 |= 1;
    }
    func_80939EE0(this);
}

void ObjBombiwa_Destroy(Actor* thisx, PlayState* play2) {
    PlayState* play = play2;
    ObjBombiwa* this = (ObjBombiwa*)thisx;

    Collider_DestroyCylinder(play, &this->collider);
}

void func_80939794(ObjBombiwa* this, PlayState* play) {
    Vec3f spB4;
    Vec3f spA8;
    s32 phi_v1;
    s32 i;

    for (i = 0; i < ARRAY_COUNT(D_8093A9D0); i++) {
        spB4.x = ((Rand_ZeroOne() - 0.5f) * 10.0f) + this->actor.home.pos.x;
        spB4.y = (Rand_ZeroOne() * 5.0f) + this->actor.home.pos.y + 8.0f;
        spB4.z = ((Rand_ZeroOne() - 0.5f) * 10.0f) + this->actor.home.pos.z;

        spA8.x = (Rand_ZeroOne() - 0.5f) * 15.0f;
        spA8.y = (Rand_ZeroOne() * 16.0f) + 5.0f;
        spA8.z = (Rand_ZeroOne() - 0.5f) * 15.0f;

        if (D_8093A9D0[i] > 10) {
            phi_v1 = 0x25;
        } else {
            phi_v1 = 0x21;
        }
        EffectSsKakera_Spawn(play, &spB4, &spA8, &spB4, -400, phi_v1, 10, 2, 0, D_8093A9D0[i], 1, 0, 60, -1,
                             OBJECT_BOMBIWA, object_bombiwa_DL_0009E0);
    }

    func_800BBFB0(play, &this->actor.world.pos, 60.0f, 8, 80, 140, 1);
}

void func_80939994(PlayState* play, Vec3f* arg1) {
    Vec3f spAC;
    Vec3f spA0;
    s16 phi_v0;
    s16 life;
    s32 i;

    for (i = 0; i < 16; i++) {
        spAC.x = (Rand_ZeroOne() - 0.5f) * 80.0f;
        spAC.y = Rand_ZeroOne() * 120.0f;
        spAC.z = (Rand_ZeroOne() - 0.5f) * 80.0f;

        spA0.x = ((Rand_ZeroOne() - 0.5f) * 3.0f) + (spAC.x * 0.2f);
        spA0.y = (Rand_ZeroOne() * 16.0f) + 5.0f;
        spA0.z = ((Rand_ZeroOne() - 0.5f) * 3.0f) + (spAC.z * 0.2f);

        spAC.x += arg1->x;
        spAC.y += arg1->y;
        spAC.z += arg1->z;

        if (i >= 14) {
            phi_v0 = 33;
            life = 60;
        } else if (i >= 12) {
            phi_v0 = 65;
            life = 60;
        } else {
            life = 40;
            if (Rand_ZeroOne() < 0.7f) {
                phi_v0 = 64;
            } else {
                phi_v0 = 32;
            }
        }
        EffectSsKakera_Spawn(play, &spAC, &spA0, &spAC, -450, phi_v0, 15, 0, 0, i, 1, 0, life, -1, OBJECT_BOMBIWA,
                             object_bombiwa_DL_005990);
    }

    func_800BBFB0(play, arg1, 120.0f, 3, 80, 140, 1);

    spAC.x = arg1->x;
    spAC.y = arg1->y + 70.0f;
    spAC.z = arg1->z;

    func_800BBFB0(play, &spAC, 120.0f, 3, 80, 140, 1);
}

void func_80939C50(PlayState* play, Vec3f* arg1) {
    Vec3f spBC;
    Vec3f spB0;
    s32 phi_v0;
    s32 phi_s0;
    s32 i;

    for (i = 0; i < ARRAY_COUNT(D_8093A9E0); i++) {
        spBC.x = (Rand_ZeroOne() - 0.5f) * 15.0f;
        spBC.y = (Rand_ZeroOne() - 0.2f) * 10.0f;
        spBC.z = (Rand_ZeroOne() - 0.5f) * 15.0f;

        spB0.x = ((Rand_ZeroOne() - 0.5f) * 3.0f) + (spBC.x * 0.85f);
        spB0.y = (Rand_ZeroOne() * 15.0f) + 8.0f;
        spB0.z = ((Rand_ZeroOne() - 0.5f) * 3.0f) + (spBC.z * 0.85f);

        spBC.x += arg1->x;
        spBC.y += arg1->y;
        spBC.z += arg1->z;

        if (i == 0) {
            phi_v0 = 33;
            phi_s0 = 60;
        } else if (i == 1) {
            phi_v0 = 65;
            phi_s0 = 60;
        } else {
            phi_s0 = 40;
            if (Rand_ZeroOne() < 0.7f) {
                phi_v0 = 64;
            } else {
                phi_v0 = 32;
            }
        }
        EffectSsKakera_Spawn(play, &spBC, &spB0, &spBC, -450, phi_v0, 15, 0, 0, D_8093A9E0[i], 1, 0, phi_s0, -1,
                             OBJECT_BOMBIWA, object_bombiwa_DL_005990);
    }

    func_800BBFB0(play, arg1, 60.0f, 4, 80, 140, 1);
}

void func_80939EE0(ObjBombiwa* this) {
    this->actionFunc = func_80939EF4;
}

void func_80939EF4(ObjBombiwa* this, PlayState* play) {
    s32 pad;
    s32 params = OBJBOMBIWA_GET_100(&this->actor);
    ObjBombiwaStruct2* sp28 = &D_8093A998[params];

    if (this->collider.base.acFlags & AC_HIT) {
        this->unk_202 = 5;
    }

    if (sp28->unk_0C(&this->actor)) {
        Flags_SetSwitch(play, OBJBOMBIWA_GET_SWITCH_FLAG(&this->actor));
        SoundSource_PlaySfxAtFixedWorldPos(play, &this->actor.world.pos, 80, NA_SE_EV_WALL_BROKEN);
        if (OBJBOMBIWA_GET_8000(&this->actor)) {
            Audio_PlaySfx(NA_SE_SY_CORRECT_CHIME);
        }

        if (params == OBJBOMBIWA_100_0) {
            func_80939794(this, play);
            Actor_Kill(&this->actor);
        } else {
            func_80939994(play, &this->actor.world.pos);
            this->actor.flags |= ACTOR_FLAG_UPDATE_CULLING_DISABLED;
            func_8093A080(this);
        }
    } else {
        this->collider.base.acFlags &= ~AC_HIT;

        if (this->actor.xzDistToPlayer < 1400.0f) {
            if (this->unk_202 > 0) {
                this->unk_202--;
                if (this->unk_202 == 0) {
                    this->collider.base.colMaterial = COL_MATERIAL_HARD;
                } else {
                    this->collider.base.colMaterial = COL_MATERIAL_NONE;
                }
            }
            CollisionCheck_SetAC(play, &play->colChkCtx, &this->collider.base);
            CollisionCheck_SetOC(play, &play->colChkCtx, &this->collider.base);
        }
    }
}

void func_8093A080(ObjBombiwa* this) {
    ObjBombiwaStruct* ptr;
    s16 phi_s2;
    s32 i;

    for (i = 0, phi_s2 = 0; i < ARRAY_COUNT(this->unk_190); i++, phi_s2 += 0x4000) {
        ptr = &this->unk_190[i];

        ptr->unk_00 = (Rand_ZeroOne() * 0.1f) + 0.05f;

        ptr->unk_04.x = (Math_SinS(phi_s2) * 50.0f) + this->actor.world.pos.x;
        ptr->unk_04.y = ((i + 1) * 31.0f) + this->actor.world.pos.y;
        ptr->unk_04.z = (Math_CosS(phi_s2) * 50.0f) + this->actor.world.pos.z;

        ptr->unk_10 = i + 3.0f;
        ptr->unk_14.x = phi_s2;
        ptr->unk_14.y = Rand_Next() >> 0x10;
        ptr->unk_14.z = 0;
        ptr->unk_1A = 0;
    }

    this->unk_200 = 0;
    this->unk_201 = 80;
    this->actionFunc = func_8093A1F0;
}

void func_8093A1F0(ObjBombiwa* this, PlayState* play) {
    s32 pad;
    s32 i;
    Vec3f sp9C;
    ObjBombiwaStruct* ptr;
    CollisionPoly* sp94;
    s32 bgId;
    f32 temp_f0;
    s16 phi_s1;

    for (i = 0, phi_s1 = 0; i < ARRAY_COUNT(this->unk_190); i++, phi_s1 += 0x4000) {
        ptr = &this->unk_190[i];

        if (ptr->unk_1A != 0) {
            continue;
        }

        ptr->unk_10 -= 3.0f;

        ptr->unk_04.x += Math_SinS(phi_s1) * 1.5f;
        ptr->unk_04.y += ptr->unk_10;
        ptr->unk_04.z += Math_CosS(phi_s1) * 1.5f;

        ptr->unk_14.x += 0x5DC;

        sp9C.x = ptr->unk_04.x;
        sp9C.y = ptr->unk_04.y + 30.0f;
        sp9C.z = ptr->unk_04.z;

        temp_f0 = BgCheck_EntityRaycastFloor5(&play->colCtx, &sp94, &bgId, &this->actor, &sp9C);
        if ((temp_f0 <= (BGCHECK_Y_MIN + 10.0f)) || ((ptr->unk_04.y - (200.0f * ptr->unk_00)) < temp_f0)) {
            this->unk_200++;
            ptr->unk_1A = 1;
            func_80939C50(play, &ptr->unk_04);
        }
    }

    this->unk_201--;
    if ((this->unk_200 >= 4) || (this->unk_201 <= 0)) {
        Actor_Kill(&this->actor);
    }
}

void ObjBombiwa_Update(Actor* thisx, PlayState* play) {
    ObjBombiwa* this = (ObjBombiwa*)thisx;

    this->actionFunc(this, play);
}

void func_8093A418(Actor* thisx, PlayState* play) {
    ObjBombiwa* this = (ObjBombiwa*)thisx;
    f32 sp28;

    Ship_ExtendedCullingActorAdjustProjectedZ(&this->actor);

    if ((this->actor.projectedPos.z <= 2200.0f) || ((this->unk_203 & 1) && (this->actor.projectedPos.z < 2300.0f))) {
        this->actor.shape.shadowAlpha = 160;
        Gfx_DrawDListOpa(play, object_bombiwa_DL_0009E0);
        return;
    }

    if (this->actor.projectedPos.z < 2300.0f) {
        OPEN_DISPS(play->state.gfxCtx);

        sp28 = (2300.0f - this->actor.projectedPos.z) * 2.55f;

        this->actor.shape.shadowAlpha = sp28 * (160.0f / 255.0f);
        Gfx_SetupDL25_Xlu(play->state.gfxCtx);

        MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx);
        gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 255, 255, 255, (s32)sp28);
        gSPDisplayList(POLY_XLU_DISP++, object_bombiwa_DL_000AF0);

        CLOSE_DISPS(play->state.gfxCtx);
    } else {
        this->actor.shape.shadowAlpha = 0;
    }

    Ship_ExtendedCullingActorRestoreProjectedPos(play, &this->actor);
}

void func_8093A608(Actor* thisx, PlayState* play) {
    s32 pad[8];
    ObjBombiwa* this = (ObjBombiwa*)thisx;
    f32 sp38;
    s32 i;
    ObjBombiwaStruct* ptr;

    OPEN_DISPS(play->state.gfxCtx);

    Ship_ExtendedCullingActorAdjustProjectedZ(&this->actor);

    if (this->actionFunc == func_80939EF4) {
        if ((this->actor.projectedPos.z <= 2200.0f) ||
            ((this->unk_203 & 1) && (this->actor.projectedPos.z < 2300.0f))) {
            Gfx_SetupDL25_Opa(play->state.gfxCtx);

            gSPSegment(POLY_OPA_DISP++, 0x08, D_801AEFA0);
            MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx);
            gDPSetPrimColor(POLY_OPA_DISP++, 0, 0x9B, 255, 255, 255, 255);
            gSPDisplayList(POLY_OPA_DISP++, object_bombiwa_DL_004560);

            Gfx_SetupDL25_Xlu(play->state.gfxCtx);

            MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx);
            gSPDisplayList(POLY_XLU_DISP++, object_bombiwa_DL_004688);
        } else if (this->actor.projectedPos.z < 2300.0f) {
            sp38 = (2300.0f - this->actor.projectedPos.z) * 2.55f;
            Gfx_SetupDL25_Xlu(play->state.gfxCtx);

            gSPSegment(POLY_XLU_DISP++, 0x08, D_801AEF88);
            MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx);
            gDPSetPrimColor(POLY_XLU_DISP++, 0, 0x9B, 255, 255, 255, (s32)sp38);
            gSPDisplayList(POLY_XLU_DISP++, object_bombiwa_DL_004560);
        }
    } else {
        Gfx_SetupDL25_Opa(play->state.gfxCtx);

        for (i = 0; i < ARRAY_COUNT(this->unk_190); i++) {
            ptr = &this->unk_190[i];

            if (ptr->unk_1A == 0) {
                Matrix_SetTranslateRotateYXZ(ptr->unk_04.x, ptr->unk_04.y, ptr->unk_04.z, &ptr->unk_14);
                Matrix_Scale(ptr->unk_00, ptr->unk_00, ptr->unk_00, MTXMODE_APPLY);

                MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx);
                gSPDisplayList(POLY_OPA_DISP++, object_bombiwa_DL_005990);
            }
        }
    }

    Ship_ExtendedCullingActorRestoreProjectedPos(play, &this->actor);

    CLOSE_DISPS(play->state.gfxCtx);
}
