/*
 * File: z_obj_hamishi.c
 * Overlay: ovl_Obj_Hamishi
 * Description: Bronze boulder
 */

#include "z_obj_hamishi.h"
#include "objects/gameplay_field_keep/gameplay_field_keep.h"

#include "2s2h/ShipUtils.h"

#define FLAGS (ACTOR_FLAG_UPDATE_CULLING_DISABLED)

void ObjHamishi_Init(Actor* thisx, PlayState* play);
void ObjHamishi_Destroy(Actor* thisx, PlayState* play2);
void ObjHamishi_Update(Actor* thisx, PlayState* play);
void ObjHamishi_Draw(Actor* thisx, PlayState* play);

ActorProfile Obj_Hamishi_Profile = {
    /**/ ACTOR_OBJ_HAMISHI,
    /**/ ACTORCAT_PROP,
    /**/ FLAGS,
    /**/ GAMEPLAY_FIELD_KEEP,
    /**/ sizeof(ObjHamishi),
    /**/ ObjHamishi_Init,
    /**/ ObjHamishi_Destroy,
    /**/ ObjHamishi_Update,
    /**/ ObjHamishi_Draw,
};

static ColliderCylinderInit sCylinderInit = {
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
        { 0x81C37FB6, 0x00, 0x00 },
        ATELEM_NONE | ATELEM_SFX_NORMAL,
        ACELEM_ON,
        OCELEM_ON,
    },
    { 50, 70, 0, { 0, 0, 0 } },
};

static CollisionCheckInfoInit sColChkInfoInit = { 0, 12, 60, MASS_IMMOVABLE };

s16 D_809A1AD4[] = {
    145, 135, 115, 85, 75, 53, 45, 40, 35,
};

static InitChainEntry sInitChain[] = {
    ICHAIN_VEC3F_DIV1000(scale, 400, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeDistance, 2000, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeScale, 250, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeDownward, 500, ICHAIN_STOP),
};

void func_809A0F20(Actor* thisx, PlayState* play) {
    ObjHamishi* this = (ObjHamishi*)thisx;

    Collider_InitCylinder(play, &this->collider);
    Collider_SetCylinder(play, &this->collider, &this->actor, &sCylinderInit);
    Collider_UpdateCylinder(&this->actor, &this->collider);
}

void func_809A0F78(ObjHamishi* this) {
    if (this->unk_198 > 0) {
        this->unk_198--;
        this->unk_19A += 0x1388;
        this->unk_19C += 0xE10;

        Math_StepToF(&this->unk_190, 0.0f, 0.15f);
        Math_StepToF(&this->unk_194, 0.0f, 40.0f);

        this->actor.world.pos.x = (Math_SinS(this->unk_19A * 4) * this->unk_190) + this->actor.home.pos.x;
        this->actor.world.pos.z = (Math_CosS(this->unk_19A * 7) * this->unk_190) + this->actor.home.pos.z;

        this->actor.shape.rot.x = (s32)(Math_SinS(this->unk_19C * 4) * this->unk_194) + this->actor.home.rot.x;
        this->actor.shape.rot.z = (s32)(Math_CosS(this->unk_19C * 7) * this->unk_194) + this->actor.home.rot.z;
    } else {
        Math_StepToF(&this->actor.world.pos.x, this->actor.home.pos.x, 1.0f);
        Math_StepToF(&this->actor.world.pos.z, this->actor.home.pos.z, 1.0f);
        Math_ScaledStepToS(&this->actor.shape.rot.x, this->actor.home.rot.x, 0xBB8);
        Math_ScaledStepToS(&this->actor.shape.rot.z, this->actor.home.rot.z, 0xBB8);
    }
}

void func_809A10F4(ObjHamishi* this, PlayState* play) {
    s32 i;
    Vec3f spC8;
    Vec3f spBC;
    f32 temp_f20;
    s16 temp_s0 = 1000;
    s32 gravity;
    s32 phi_v0;

    for (i = 0; i < ARRAY_COUNT(D_809A1AD4); i++) {
        temp_s0 += 0x4E20;
        temp_f20 = Rand_ZeroOne() * 10.0f;

        spBC.x = (Math_SinS(temp_s0) * temp_f20) + this->actor.world.pos.x;
        spBC.y = (Rand_ZeroOne() * 40.0f) + this->actor.world.pos.y + 5.0f;
        spBC.z = (Math_CosS(temp_s0) * temp_f20) + this->actor.world.pos.z;

        temp_f20 = (Rand_ZeroOne() * 10.0f) + 2.0f;
        spC8.x = Math_SinS(temp_s0) * temp_f20;

        spC8.y = (Rand_ZeroOne() * 15.0f) + (Rand_ZeroOne() * i * 2.5f);
        spC8.z = Math_CosS(temp_s0) * temp_f20;

        if (i == 0) {
            phi_v0 = 41;
            gravity = -450;
        } else if (i < 4) {
            phi_v0 = 37;
            gravity = -380;
        } else {
            phi_v0 = 69;
            gravity = -320;
        }

        EffectSsKakera_Spawn(play, &spBC, &spC8, &this->actor.world.pos, gravity, phi_v0, 30, 5, 0, D_809A1AD4[i], 3, 0,
                             70, 1, GAMEPLAY_FIELD_KEEP, gFieldSilverBoulderDebrisDL);
    }

    func_800BBFB0(play, &this->actor.world.pos, 140.0f, 6, 180, 90, 1);
    func_800BBFB0(play, &this->actor.world.pos, 140.0f, 12, 80, 90, 1);
}

void func_809A13A0(ObjHamishi* this, PlayState* play) {
    s32 pad;
    Vec3f sp28;
    s32 bgId;

    sp28.x = this->actor.world.pos.x;
    sp28.y = this->actor.world.pos.y + 30.0f;
    sp28.z = this->actor.world.pos.z;

    this->actor.floorHeight =
        BgCheck_EntityRaycastFloor5(&play->colCtx, &this->actor.floorPoly, &bgId, &this->actor, &sp28);
}

s32 ObjHamishi_IsUnderwater(ObjHamishi* this, PlayState* play) {
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

void ObjHamishi_Init(Actor* thisx, PlayState* play) {
    ObjHamishi* this = (ObjHamishi*)thisx;

    Actor_ProcessInitChain(&this->actor, sInitChain);

    if (play->csCtx.state != CS_STATE_IDLE) {
        this->actor.cullingVolumeDistance += 1000.0f;
    }

    if (this->actor.shape.rot.y == 0) {
        this->actor.shape.rot.y = Rand_Next() >> 0x10;
        this->actor.world.rot.y = this->actor.shape.rot.y;
        this->actor.home.rot.y = this->actor.shape.rot.y;
    }

    func_809A0F20(&this->actor, play);
    CollisionCheck_SetInfo(&this->actor.colChkInfo, NULL, &sColChkInfoInit);
    func_809A13A0(this, play);
    ActorShape_Init(&this->actor.shape, 0.0f, ActorShadow_DrawCircle, 2.3f);

    if (Flags_GetSwitch(play, OBJHAMISHI_GET_SWITCH_FLAG(&this->actor))) {
        Actor_Kill(&this->actor);
        return;
    }

    this->actor.shape.yOffset = 80.0f;

    if (ObjHamishi_IsUnderwater(this, play)) {
        this->unk_1A2 |= 1;
    }
}

void ObjHamishi_Destroy(Actor* thisx, PlayState* play2) {
    PlayState* play = play2;
    ObjHamishi* this = (ObjHamishi*)thisx;

    Collider_DestroyCylinder(play, &this->collider);
}

void ObjHamishi_Update(Actor* thisx, PlayState* play) {
    s32 pad;
    ObjHamishi* this = (ObjHamishi*)thisx;
    s32 sp24 = (this->collider.base.acFlags & AC_HIT) != 0;

    func_809A0F78(this);

    if (sp24) {
        this->unk_1A1 = 5;
        this->collider.base.acFlags &= ~AC_HIT;
    }

    if (sp24) {
        if (this->collider.elem.acHitElem->atDmgInfo.dmgFlags & 0x80000500) {
            if (this->collider.elem.acHitElem->atDmgInfo.dmgFlags & 0x400) {
                this->unk_1A0 = 26;
            } else {
                this->unk_1A0 = 11;
            }

            if (this->collider.elem.acHitElem->atDmgInfo.dmgFlags & 0x80000000) {
                this->unk_19E = 2;
            } else {
                this->unk_19E++;
            }

            if (this->unk_19E < 2) {
                this->unk_198 = 15;
                this->unk_190 = 2.0f;
                this->unk_194 = 400.0f;
            } else {
                func_809A10F4(this, play);
                SoundSource_PlaySfxAtFixedWorldPos(play, &this->actor.world.pos, 40, NA_SE_EV_WALL_BROKEN);
                Flags_SetSwitch(play, OBJHAMISHI_GET_SWITCH_FLAG(&this->actor));
                Actor_Kill(&this->actor);
            }
        }
    }

    if (this->actor.update != NULL) {
        if (this->unk_1A1 > 0) {
            this->unk_1A1--;
            if (this->unk_1A1 == 0) {
                this->collider.base.colMaterial = COL_MATERIAL_HARD;
            } else {
                this->collider.base.colMaterial = COL_MATERIAL_NONE;
            }
        }

        if (this->unk_1A0 > 0) {
            this->unk_1A0--;
        } else if ((this->actor.flags & ACTOR_FLAG_INSIDE_CULLING_VOLUME) && (this->actor.xzDistToPlayer < 1000.0f)) {
            CollisionCheck_SetAC(play, &play->colChkCtx, &this->collider.base);
        }

        if (this->actor.xzDistToPlayer < 600.0f) {
            CollisionCheck_SetOC(play, &play->colChkCtx, &this->collider.base);
        }
    }
}

void ObjHamishi_Draw(Actor* thisx, PlayState* play) {
    ObjHamishi* this = (ObjHamishi*)thisx;

    OPEN_DISPS(play->state.gfxCtx);

    Ship_ExtendedCullingActorAdjustProjectedZ(&this->actor);

    if ((thisx->projectedPos.z <= 2150.0f) || ((this->unk_1A2 & 1) && (thisx->projectedPos.z < 2250.0f))) {
        thisx->shape.shadowAlpha = 160;
        Gfx_SetupDL25_Opa(play->state.gfxCtx);

        gSPSegment(POLY_OPA_DISP++, 0x08, D_801AEFA0);
        MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx);
        gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 255, 170, 130, 255);
        gSPDisplayList(POLY_OPA_DISP++, gFieldSilverBoulderDL);
    } else if (thisx->projectedPos.z < 2250.0f) {
        f32 sp20 = (2250.0f - thisx->projectedPos.z) * 2.55f;

        thisx->shape.shadowAlpha = sp20 * 0.627451f;
        Gfx_SetupDL25_Xlu(play->state.gfxCtx);

        gSPSegment(POLY_XLU_DISP++, 0x08, D_801AEF88);
        MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx);
        gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 255, 170, 130, (s32)sp20);
        gSPDisplayList(POLY_XLU_DISP++, gFieldSilverBoulderDL);
    } else {
        thisx->shape.shadowAlpha = 0;
    }

    Ship_ExtendedCullingActorRestoreProjectedPos(play, &this->actor);

    CLOSE_DISPS(play->state.gfxCtx);
}
