/*
 * File: z_obj_aqua.c
 * Overlay: ovl_Obj_Aqua
 * Description: Water poured out of a bottle
 */

#include "z_obj_aqua.h"
#include "objects/gameplay_keep/gameplay_keep.h"

#define FLAGS (ACTOR_FLAG_UPDATE_CULLING_DISABLED)

void ObjAqua_Init(Actor* thisx, PlayState* play);
void ObjAqua_Destroy(Actor* thisx, PlayState* play);
void ObjAqua_Update(Actor* thisx, PlayState* play);
void ObjAqua_Draw(Actor* thisx, PlayState* play);

void func_80ACBC70(ObjAqua* this);
void func_80ACBDCC(ObjAqua* this);
void func_80ACBC8C(ObjAqua* this, PlayState* play);
void func_80ACBD48(ObjAqua* this, PlayState* play);
void func_80ACBDFC(ObjAqua* this, PlayState* play);

void func_80ACBD34(ObjAqua* this);

ActorProfile Obj_Aqua_Profile = {
    /**/ ACTOR_OBJ_AQUA,
    /**/ ACTORCAT_ITEMACTION,
    /**/ FLAGS,
    /**/ GAMEPLAY_KEEP,
    /**/ sizeof(ObjAqua),
    /**/ ObjAqua_Init,
    /**/ ObjAqua_Destroy,
    /**/ ObjAqua_Update,
    /**/ ObjAqua_Draw,
};

static ColliderCylinderInit sCylinderInit = {
    {
        COL_MATERIAL_NONE,
        AT_ON | AT_TYPE_OTHER,
        AC_NONE,
        OC1_NONE,
        OC2_NONE,
        COLSHAPE_CYLINDER,
    },
    {
        ELEM_MATERIAL_UNK0,
        { 0xF7CFFFFF, 0x00, 0x00 },
        { 0x00000000, 0x00, 0x00 },
        ATELEM_ON | ATELEM_SFX_NONE,
        ACELEM_NONE,
        OCELEM_NONE,
    },
    { 6, 10, 0, { 0, 0, 0 } },
};

static InitChainEntry sInitChain[] = {
    ICHAIN_VEC3S(shape.rot, 0, ICHAIN_CONTINUE),
    ICHAIN_VEC3S(world.rot, 0, ICHAIN_CONTINUE),
    ICHAIN_F32_DIV1000(gravity, -900, ICHAIN_CONTINUE),
    ICHAIN_F32_DIV1000(terminalVelocity, -4000, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeDistance, 4000, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeScale, 300, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeDownward, 300, ICHAIN_STOP),
};

Vec3f D_80ACC308 = { 1.0f / 1000.0f, 7.0f / 10000.0f, 1.0f / 1000.0f };
Vec3f D_80ACC314 = { 8.6f / 1000.0f, 8.0f / 10000.0f, 8.6f / 1000.0f };
Vec3f D_80ACC320 = { 1.0f / 100.0f, 2.6f / 1000.0f, 1.0f / 100.0f };

void func_80ACB6A0(ObjAqua* this, PlayState* play) {
    s32 pad;
    Vec3f sp58;
    s32 angleOffset = 0;
    s32 i;

    sp58.y = this->actor.world.pos.y + this->actor.depthInWater;
    for (i = 0; i < 4; i++) {
        sp58.x = this->actor.world.pos.x + Math_SinS((s32)(Rand_ZeroOne() * 7200.0f) + angleOffset) * 8.0f;
        sp58.z = this->actor.world.pos.z + Math_CosS((s32)(Rand_ZeroOne() * 7200.0f) + angleOffset) * 8.0f;
        EffectSsGSplash_Spawn(play, &sp58, NULL, NULL, 0, 120);
        angleOffset += 0x4000;
    }
    sp58.x = this->actor.world.pos.x;
    sp58.z = this->actor.world.pos.z;
    EffectSsGSplash_Spawn(play, &sp58, NULL, NULL, 0, 300);
}

void func_80ACB7F4(ObjAqua* this, PlayState* play) {
    s32 pad;
    Vec3f effectPos;
    s32 angleOffset = 0;
    s32 i;

    effectPos.y = this->actor.floorHeight;
    for (i = 0; i < 4; i++) {
        effectPos.x = this->actor.world.pos.x + Math_SinS((s32)(Rand_ZeroOne() * 7200.0f) + angleOffset) * 8.0f;
        effectPos.z = this->actor.world.pos.z + Math_CosS((s32)(Rand_ZeroOne() * 7200.0f) + angleOffset) * 8.0f;
        EffectSsGSplash_Spawn(play, &effectPos, NULL, NULL, 0, 120);
        angleOffset += 0x4000;
    }
    effectPos.x = this->actor.world.pos.x;
    effectPos.z = this->actor.world.pos.z;
    EffectSsGSplash_Spawn(play, &effectPos, NULL, NULL, 0, 300);
}

void func_80ACB940(ObjAqua* this, PlayState* play) {
    s32 pad;
    Vec3f effectPos;
    Vec3f effectVelocity;

    effectVelocity.x = Rand_ZeroOne() - 0.5f;
    effectVelocity.y = 2.0f;
    effectVelocity.z = Rand_ZeroOne() - 0.5f;
    effectPos.x = this->actor.world.pos.x + (effectVelocity.x * 40.0f);
    effectPos.y = this->actor.world.pos.y;
    effectPos.z = this->actor.world.pos.z + (effectVelocity.z * 40.0f);
    EffectSsIceSmoke_Spawn(play, &effectPos, &effectVelocity, &gZeroVec3f, (s32)(Rand_ZeroOne() * 24.0f) + 70);
}

void func_80ACBA10(ObjAqua* this) {
    s32 pad;
    MtxF sp2C;

    func_800C0094(this->actor.floorPoly, this->actor.world.pos.x, this->actor.floorHeight, this->actor.world.pos.z,
                  &sp2C);
    Matrix_MtxFToYXZRot(&sp2C, &this->actor.shape.rot, false);
}

s32 ObjAqua_IsUnderwater(ObjAqua* this, PlayState* play) {
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

void ObjAqua_Init(Actor* thisx, PlayState* play) {
    ObjAqua* this = (ObjAqua*)thisx;
    s32 i;

    Actor_ProcessInitChain(thisx, sInitChain);
    this->actor.scale.x = 0.0009f;
    this->actor.scale.y = 0.0005f;
    this->actor.scale.z = 0.0009f;
    Collider_InitCylinder(play, &this->collider);
    Collider_SetCylinder(play, &this->collider, thisx, &sCylinderInit);
    ActorShape_Init(&thisx->shape, 0.0f, ActorShadow_DrawCircle, 60.0f);

    thisx->shape.shadowAlpha = 140;
    this->alpha = 255;
    if (ObjAqua_IsUnderwater(this, play)) {
        for (i = 0; i < 8; i++) {
            EffectSsBubble_Spawn(play, &thisx->world.pos, -4.0f, 4.0f, 4.0f, (Rand_ZeroOne() * 0.09f) + 0.03f);
        }
        func_80ACBDCC(this);
    } else {
        func_80ACBC70(this);
    }
}

void ObjAqua_Destroy(Actor* thisx, PlayState* play) {
    ObjAqua* this = (ObjAqua*)thisx;
    Collider_DestroyCylinder(play, &this->collider);
}

void func_80ACBC70(ObjAqua* this) {
    this->counter = 200;
    this->actionFunc = func_80ACBC8C;
}

void func_80ACBC8C(ObjAqua* this, PlayState* play) {
    if (this->actor.bgCheckFlags & (BGCHECKFLAG_GROUND | BGCHECKFLAG_WATER)) {
        if (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND) {
            func_80ACB7F4(this, play);
            func_80ACBA10(this);
            Actor_PlaySfx(&this->actor, NA_SE_EV_BOTTLE_WATERING);
            func_80ACBD34(this);
        } else {
            func_80ACB6A0(this, play);
            SoundSource_PlaySfxAtFixedWorldPos(play, &this->actor.world.pos, 0x28, NA_SE_EV_BOMB_DROP_WATER);
            Actor_Kill(&this->actor);
        }
    } else if (this->counter <= 0) {
        Actor_Kill(&this->actor);
    }
}

void func_80ACBD34(ObjAqua* this) {
    this->actionFunc = func_80ACBD48;
}

void func_80ACBD48(ObjAqua* this, PlayState* play) {
    if ((AQUA_GET_TYPE(&this->actor) == AQUA_TYPE_HOT) && (this->alpha > 90)) {
        func_80ACB940(this, play);
    }
    if (this->alpha > 5) {
        this->alpha -= 5;
    } else {
        this->alpha = 0;
    }
    if (this->actor.shape.shadowAlpha > 2) {
        this->actor.shape.shadowAlpha -= 2;
    } else {
        Actor_Kill(&this->actor);
    }
}

void func_80ACBDCC(ObjAqua* this) {
    this->actor.gravity = -0.1f;
    this->counter = 40;
    this->alpha = 140;
    this->actionFunc = func_80ACBDFC;
}

void func_80ACBDFC(ObjAqua* this, PlayState* play) {
    s32 pad;
    f32 temp = this->counter;

    this->alpha = (s32)(temp * 3.25f) + 10;
    this->actor.shape.shadowAlpha = this->alpha;
    this->unk_198 += 1000;
    if (AQUA_GET_TYPE(&this->actor) == AQUA_TYPE_HOT) {
        f32 temp_f2 = this->actor.scale.x * 10000.0f;

        EffectSsBubble_Spawn(play, &this->actor.world.pos, temp_f2 * -0.5f, temp_f2, temp_f2,
                             (Rand_ZeroOne() * 0.1f) + 0.03f);
    }
    if (this->counter <= 0) {
        Actor_Kill(&this->actor);
    }
}

void ObjAqua_Update(Actor* thisx, PlayState* play) {
    ObjAqua* this = (ObjAqua*)thisx;
    s32 pad;

    if (this->counter > 0) {
        this->counter--;
    }
    this->actionFunc(this, play);
    if (this->actor.update != NULL) {
        if (this->actionFunc == func_80ACBC8C) {
            Math_Vec3f_StepTo(&this->actor.scale, &D_80ACC308, 0.00006f);
        } else if (this->actionFunc == func_80ACBD48) {
            Math_Vec3f_StepTo(&this->actor.scale, &D_80ACC314, 0.00095f);
        } else {
            Math_Vec3f_StepTo(&this->actor.scale, &D_80ACC320, 0.0004f);
        }
        this->actor.velocity.y *= 0.9f;
        Actor_MoveWithGravity(&this->actor);
        Actor_UpdateBgCheckInfo(play, &this->actor, 12.0f, 4.0f, 0.0f, UPDBGCHECKINFO_FLAG_1 | UPDBGCHECKINFO_FLAG_4);
        if (this->actionFunc != func_80ACBDFC) {
            Collider_UpdateCylinder(&this->actor, &this->collider);
            this->collider.dim.radius = this->actor.scale.x * 3000.0f;
            CollisionCheck_SetAT(play, &play->colChkCtx, &this->collider.base);
        }
    }
}

void ObjAqua_Draw(Actor* thisx, PlayState* play) {
    ObjAqua* this = (ObjAqua*)thisx;
    s32 framesTemp;
    s32 pad;
    s16 yaw = Camera_GetCamDirYaw(GET_ACTIVE_CAM(play)) + 0x8000;
    s32 actionFuncTemp = this->actionFunc == func_80ACBDFC;

    OPEN_DISPS(play->state.gfxCtx);

    Gfx_SetupDL25_Xlu(play->state.gfxCtx);
    framesTemp = ((play->gameplayFrames & 0x7FFFFFFF) * -0xA) & 0x1FF;
    if (actionFuncTemp) {
        framesTemp >>= 1;
    }
    gSPSegment(POLY_XLU_DISP++, 0x08,
               Gfx_TwoTexScrollEx(play->state.gfxCtx, 0, 0, 0, 0x20, 0x40, 1, 0, framesTemp, 0x20, 0x80, 0, 0, 0,
                                  actionFuncTemp ? -5 : -0xA));
    gDPSetPrimColor(POLY_XLU_DISP++, 0x80, 0x80, 170, 255, 255, this->alpha);
    gDPSetEnvColor(POLY_XLU_DISP++, 0, 150, 255, 0);
    if (actionFuncTemp) {
        s16 rotation = Math_SinS(this->unk_198) * 8000.0f;

        Matrix_RotateZS(rotation, MTXMODE_APPLY);
        Matrix_Scale(1.3f, 1.0f, 1.0f, MTXMODE_APPLY);
        Matrix_RotateZS(rotation * -1, MTXMODE_APPLY);
        Matrix_Scale(10.0f / 13.0f, 1.0f, 1.0f, MTXMODE_APPLY);
    }

    Matrix_RotateYS(yaw, MTXMODE_APPLY);
    MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx);
    gSPDisplayList(POLY_XLU_DISP++, gEffFire1DL);

    CLOSE_DISPS(play->state.gfxCtx);
}
