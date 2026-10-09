/*
 * File: z_ovl_obj_bell.c
 * Overlay: ovl_Obj_Bell
 * Description: Stock Pot Inn Bell
 */

#include "z_obj_bell.h"
#include "objects/object_f52_obj/object_f52_obj.h"

#define FLAGS (ACTOR_FLAG_UPDATE_CULLING_DISABLED | ACTOR_FLAG_DRAW_CULLING_DISABLED)

void ObjBell_Init(Actor* thisx, PlayState* play);
void ObjBell_Destroy(Actor* thisx, PlayState* play);
void ObjBell_Update(Actor* thisx, PlayState* play);
void ObjBell_Draw(Actor* thisx, PlayState* play);

s32 func_80A356D8(ObjBell* this);
s32 func_80A357A8(ObjBell* this, PlayState* play);

ActorProfile Obj_Bell_Profile = {
    /**/ ACTOR_OBJ_BELL,
    /**/ ACTORCAT_PROP,
    /**/ FLAGS,
    /**/ OBJECT_F52_OBJ,
    /**/ sizeof(ObjBell),
    /**/ ObjBell_Init,
    /**/ ObjBell_Destroy,
    /**/ ObjBell_Update,
    /**/ ObjBell_Draw,
};

// Bell Post?
static ColliderSphereInit sCylinderInit1 = {
    {
        COL_MATERIAL_METAL,
        AT_ON | AT_TYPE_ENEMY,
        AC_NONE,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_1,
        COLSHAPE_SPHERE,
    },
    {
        ELEM_MATERIAL_UNK2,
        { 0x20000000, 0x00, 0x04 },
        { 0x00000000, 0x00, 0x00 },
        ATELEM_ON | ATELEM_SFX_NORMAL,
        ACELEM_NONE,
        OCELEM_ON,
    },
    { 0, { { 0, 960, 0 }, 70 }, 100 },
};

// Bell
static ColliderSphereInit sCylinderInit2 = {
    {
        COL_MATERIAL_METAL,
        AT_NONE,
        AC_ON | AC_TYPE_PLAYER,
        OC1_NONE,
        OC2_TYPE_1,
        COLSHAPE_SPHERE,
    },
    {
        ELEM_MATERIAL_UNK2,
        { 0x00000000, 0x00, 0x00 },
        { 0xF7CFFFFF, 0x00, 0x00 },
        ATELEM_NONE | ATELEM_SFX_NORMAL,
        ACELEM_ON,
        OCELEM_NONE,
    },
    { 0, { { 0, 1100, 0 }, 74 }, 100 },
};

static CollisionCheckInfoInit2 sColChkInfoInit2 = {
    0, 0, 0, 0, MASS_IMMOVABLE,
};

static DamageTable sDamageTable = {
    /* Deku Nut       */ DMG_ENTRY(1, 0x0),
    /* Deku Stick     */ DMG_ENTRY(1, 0x0),
    /* Horse trample  */ DMG_ENTRY(1, 0x0),
    /* Explosives     */ DMG_ENTRY(1, 0xE),
    /* Zora boomerang */ DMG_ENTRY(1, 0x0),
    /* Normal arrow   */ DMG_ENTRY(1, 0x0),
    /* UNK_DMG_0x06   */ DMG_ENTRY(1, 0x0),
    /* Hookshot       */ DMG_ENTRY(1, 0x0),
    /* Goron punch    */ DMG_ENTRY(1, 0xF),
    /* Sword          */ DMG_ENTRY(1, 0x0),
    /* Goron pound    */ DMG_ENTRY(1, 0x0),
    /* Fire arrow     */ DMG_ENTRY(1, 0x0),
    /* Ice arrow      */ DMG_ENTRY(1, 0x0),
    /* Light arrow    */ DMG_ENTRY(1, 0x0),
    /* Goron spikes   */ DMG_ENTRY(1, 0xF),
    /* Deku spin      */ DMG_ENTRY(1, 0x0),
    /* Deku bubble    */ DMG_ENTRY(1, 0x0),
    /* Deku launch    */ DMG_ENTRY(1, 0x0),
    /* UNK_DMG_0x12   */ DMG_ENTRY(1, 0x0),
    /* Zora barrier   */ DMG_ENTRY(1, 0x0),
    /* Normal shield  */ DMG_ENTRY(1, 0x0),
    /* Light ray      */ DMG_ENTRY(1, 0x0),
    /* Thrown object  */ DMG_ENTRY(1, 0x0),
    /* Zora punch     */ DMG_ENTRY(1, 0x0),
    /* Spin attack    */ DMG_ENTRY(1, 0x0),
    /* Sword beam     */ DMG_ENTRY(1, 0x0),
    /* Normal Roll    */ DMG_ENTRY(1, 0x0),
    /* UNK_DMG_0x1B   */ DMG_ENTRY(1, 0x0),
    /* UNK_DMG_0x1C   */ DMG_ENTRY(1, 0x0),
    /* Unblockable    */ DMG_ENTRY(1, 0x0),
    /* UNK_DMG_0x1E   */ DMG_ENTRY(1, 0x0),
    /* Powder Keg     */ DMG_ENTRY(1, 0xE),
};

s32 func_80A35510(ObjBell* this, s32 arg1) {
    Vec3f bumperPos;
    Vec3f worldPos;
    s32 phi_a3 = false;

    if (((arg1 == 0) && (this->unk_21C < 1000.0f)) || ((arg1 == 1) && (this->unk_21C < 4000.0f)) || (arg1 == 2)) {
        phi_a3 = true;
    }

    switch (arg1) {
        case 0:
            this->unk_21C += ((this->unk_21C > 1000.0f) ? 250.0f : 1000.0f);
            break;
        case 1:
            this->unk_21C += ((this->unk_21C > 3000.0f) ? 750.0f : 3000.0f);
            break;
        case 2:
            this->unk_21C += 9000.0f;
            break;
    }

    this->unk_21C = CLAMP(this->unk_21C, 0.0f, 18000.0f);
    if (phi_a3 == true) {
        Math_Vec3s_ToVec3f(&bumperPos, &this->collider2.elem.acDmgInfo.hitPos);
        Math_Vec3f_Copy(&worldPos, &this->dyna.actor.world.pos);
        this->dyna.actor.world.rot.y = Math_Vec3f_Yaw(&bumperPos, &worldPos);
        if (this->unk_20C <= 0x4000 && this->unk_20C >= -0x4000) {
            this->unk_20C -= 0x4000;
        } else {
            this->unk_20C += 0x4000;
        }
    }
    return phi_a3;
}

s32 func_80A356D8(ObjBell* this) {
    f32 scaleProjection;

    this->unk_212 = this->dyna.actor.world.rot.y;
    if (this->unk_20C >= 0x4000 || this->unk_20C <= -0x4000) {
        this->unk_212 -= 0x8000;
    }
    scaleProjection = Math_SinS(this->unk_20C) * this->unk_21C;
    this->dyna.actor.world.rot.x = this->dyna.actor.home.rot.x;
    this->unk_220 = scaleProjection;
    this->dyna.actor.world.rot.x += TRUNCF_BINANG(scaleProjection);
    Math_ApproachF(&this->unk_21C, 0.0f, 0.03f, 70.0f);
    if (this->unk_21C > 0.0f) {
        this->unk_20C -= 0x800;
    }
    return false;
}

s32 func_80A357A8(ObjBell* this, PlayState* play) {
    f32 temp_f0;
    s16 temp_v1;

    if (this->collider1.base.ocFlags1 & OC1_HIT) {
        this->collider1.base.ocFlags1 &= ~OC1_HIT;
        temp_v1 = this->dyna.actor.yawTowardsPlayer - this->unk_212;
        temp_f0 = this->unk_21C / 18000.0f;
        if (ABS_ALT(temp_v1) < 0x3FFC) {
            if (this->unk_214 == 0) {
                if (temp_f0 > 0.18f) {
                    func_800B8D98(play, &this->dyna.actor, 8.0f * temp_f0, this->dyna.actor.yawTowardsPlayer,
                                  11.0f * temp_f0);
                    this->unk_214 = 30;
                }
            }
        }
    }
    if (this->collider2.base.acFlags & AC_HIT) {
        this->collider2.base.acFlags &= ~AC_HIT;
        this->unk_20E = 10;
        switch (this->dyna.actor.colChkInfo.damageEffect) {
            case 15:
                Actor_PlaySfx(&this->dyna.actor, NA_SE_EV_BIGBELL);
                func_80A35510(this, 1);
                break;
            case 14:
                Actor_PlaySfx(&this->dyna.actor, NA_SE_EV_BIGBELL);
                func_80A35510(this, 2);
                break;
            default:
                func_80A35510(this, 0);
                break;
        }
    }
    return false;
}

void func_80A358FC(ObjBell* this, PlayState* play) {
    this->collider1.dim.worldSphere.radius = (this->collider1.dim.modelSphere.radius * this->collider1.dim.scale);
    this->collider2.dim.worldSphere.radius = (this->collider2.dim.modelSphere.radius * this->collider2.dim.scale);
    if (DECR(this->unk_20E) == 0) {
        CollisionCheck_SetAC(play, &play->colChkCtx, &this->collider2.base);
    }
    CollisionCheck_SetOC(play, &play->colChkCtx, &this->collider1.base);
}

void func_80A359B4(Actor* thisx, PlayState* play) {
    Matrix_Translate(thisx->world.pos.x, thisx->world.pos.y, thisx->world.pos.z, MTXMODE_NEW);
    Matrix_Scale(thisx->scale.x, thisx->scale.y, thisx->scale.z, MTXMODE_APPLY);
    Matrix_Translate(0.0f, 2600.0f, 0.0f, MTXMODE_APPLY);
    Matrix_RotateYS(thisx->world.rot.y, MTXMODE_APPLY);
    Matrix_RotateXS(thisx->world.rot.x, MTXMODE_APPLY);
    Matrix_RotateYS(-thisx->world.rot.y, MTXMODE_APPLY);
    Matrix_Translate(0.0f, -2600.0f, 0.0f, MTXMODE_APPLY);

    OPEN_DISPS(play->state.gfxCtx);

    Gfx_SetupDL25_Opa(play->state.gfxCtx);
    MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx);
    gSPDisplayList(POLY_OPA_DISP++, object_f52_obj_DL_000698);
    gSPDisplayList(POLY_OPA_DISP++, object_f52_obj_DL_0008D0);
    gSPDisplayList(POLY_OPA_DISP++, object_f52_obj_DL_000960);
    gSPDisplayList(POLY_OPA_DISP++, object_f52_obj_DL_0007A8);

    CLOSE_DISPS(play->state.gfxCtx);
}

void func_80A35B18(Actor* thisx, PlayState* play) {
    Matrix_Translate(thisx->world.pos.x, thisx->world.pos.y, thisx->world.pos.z, MTXMODE_NEW);
    Matrix_Scale(thisx->scale.x, thisx->scale.y, thisx->scale.z, MTXMODE_APPLY);
    Matrix_RotateYS(thisx->shape.rot.y, MTXMODE_APPLY);

    OPEN_DISPS(play->state.gfxCtx);

    Gfx_SetupDL25_Opa(play->state.gfxCtx);
    MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx);
    gSPDisplayList(POLY_OPA_DISP++, object_f52_obj_DL_000570);

    CLOSE_DISPS(play->state.gfxCtx);
}

void func_80A35BD4(Actor* thisx, PlayState* play) {
    Matrix_Translate(thisx->world.pos.x, thisx->world.pos.y - 4.0f, thisx->world.pos.z, MTXMODE_NEW);
    Matrix_Scale(thisx->scale.x, thisx->scale.y, thisx->scale.z, MTXMODE_APPLY);

    OPEN_DISPS(play->state.gfxCtx);

    Gfx_SetupDL25_Xlu(play->state.gfxCtx);
    MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx);
    gSPDisplayList(POLY_XLU_DISP++, object_f52_obj_DL_000840);

    CLOSE_DISPS(play->state.gfxCtx);
}

void ObjBell_Init(Actor* thisx, PlayState* play) {
    ObjBell* this = (ObjBell*)thisx;

    DynaPolyActor_Init(&this->dyna, 0);
    DynaPolyActor_LoadMesh(play, &this->dyna, &object_f52_obj_Colheader_001BA8);
    Actor_SetScale(&this->dyna.actor, 0.08f);
    Collider_InitAndSetSphere(play, &this->collider1, &this->dyna.actor, &sCylinderInit1);
    Collider_InitAndSetSphere(play, &this->collider2, &this->dyna.actor, &sCylinderInit2);
    CollisionCheck_SetInfo2(&this->dyna.actor.colChkInfo, &sDamageTable, &sColChkInfoInit2);
}

void ObjBell_Destroy(Actor* thisx, PlayState* play) {
    ObjBell* this = (ObjBell*)thisx;

    DynaPoly_DeleteBgActor(play, &play->colCtx.dyna, this->dyna.bgId);
    Collider_DestroySphere(play, &this->collider1);
    Collider_DestroySphere(play, &this->collider2);
}

void ObjBell_Update(Actor* thisx, PlayState* play) {
    ObjBell* this = (ObjBell*)thisx;

    if (this->unk_214 != 0) {
        this->unk_214--;
    }
    func_80A357A8(this, play);
    func_80A356D8(this);
    func_80A358FC(this, play);
}

void ObjBell_Draw(Actor* thisx, PlayState* play) {
    ObjBell* this = (ObjBell*)thisx;
    Vec3f sp30;
    Vec3f sp24;

    func_80A35B18(thisx, play);
    func_80A35BD4(thisx, play);
    func_80A359B4(thisx, play);
    Math_Vec3s_ToVec3f(&sp30, &this->collider1.dim.modelSphere.center);
    Matrix_MultVec3f(&sp30, &sp24);
    Math_Vec3f_ToVec3s(&this->collider1.dim.worldSphere.center, &sp24);
    Math_Vec3s_ToVec3f(&sp30, &this->collider2.dim.modelSphere.center);
    Matrix_MultVec3f(&sp30, &sp24);
    Math_Vec3f_ToVec3s(&this->collider2.dim.worldSphere.center, &sp24);
}
