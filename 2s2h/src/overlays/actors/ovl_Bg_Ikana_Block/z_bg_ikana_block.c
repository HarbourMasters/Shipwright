/*
 * File: z_bg_ikana_block.c
 * Overlay: ovl_Bg_Ikana_Block
 * Description: Stone Tower Temple - Rotating Room Pushblock
 */

#include "z_bg_ikana_block.h"
#include "objects/gameplay_dangeon_keep/gameplay_dangeon_keep.h"

#include "2s2h/GameInteractor/GameInteractor.h"

#define FLAGS (ACTOR_FLAG_UPDATE_CULLING_DISABLED)

void BgIkanaBlock_Init(Actor* thisx, PlayState* play);
void BgIkanaBlock_Destroy(Actor* thisx, PlayState* play);
void BgIkanaBlock_Update(Actor* thisx, PlayState* play);

void func_80B7F00C(BgIkanaBlock* this);
void func_80B7F034(BgIkanaBlock* this, PlayState* play);
void func_80B7F0A4(BgIkanaBlock* this);
void func_80B7F0D0(BgIkanaBlock* this, PlayState* play);
void func_80B7F1A8(BgIkanaBlock* this);
void func_80B7F290(BgIkanaBlock* this, PlayState* play);
void func_80B7F360(BgIkanaBlock* this);
void func_80B7F398(BgIkanaBlock* this, PlayState* play);
void func_80B7F564(Actor* thisx, PlayState* play);

ActorProfile Bg_Ikana_Block_Profile = {
    /**/ ACTOR_BG_IKANA_BLOCK,
    /**/ ACTORCAT_BG,
    /**/ FLAGS,
    /**/ GAMEPLAY_DANGEON_KEEP,
    /**/ sizeof(BgIkanaBlock),
    /**/ BgIkanaBlock_Init,
    /**/ BgIkanaBlock_Destroy,
    /**/ BgIkanaBlock_Update,
    /**/ NULL,
};

static InitChainEntry sInitChain[] = {
    ICHAIN_F32(cullingVolumeDistance, 4000, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeScale, 250, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeDownward, 250, ICHAIN_CONTINUE),
    ICHAIN_VEC3F_DIV1000(scale, 100, ICHAIN_STOP),
};

void func_80B7EA60(BgIkanaBlock* this) {
    s32 phi_v0;

    this->unk_17A &= ~(0x8 | 0x4 | 0x2 | 0x1);

    if (fabsf(this->dyna.pushForce) > 0.1f) {
        if (this->dyna.pushForce > 0.0f) {
            phi_v0 = this->dyna.yRotation;
        } else {
            phi_v0 = BINANG_ROT180(this->dyna.yRotation);
        }

        if ((phi_v0 < -0x6000) || (phi_v0 >= 0x6000)) {
            this->unk_17A |= 8;
        } else if (phi_v0 < -0x2000) {
            this->unk_17A |= 2;
        } else if (phi_v0 < 0x2000) {
            this->unk_17A |= 4;
        } else {
            this->unk_17A |= 1;
        }
    }
}

void func_80B7EB30(BgIkanaBlock* this) {
    if (this->unk_17A & (0x8 | 0x4 | 0x2 | 0x1)) {
        if (GameInteractor_Should(VB_PUSH_BLOCK_SET_TIMER, true, this)) {
            if (this->unk_17B < 127) {
                this->unk_17B++;
            }
        }
    } else {
        this->unk_17B = 0;
    }
}

bool func_80B7EB64(BgIkanaBlock* this, PlayState* play) {
    return !(this->unk_17C & 8);
}

bool func_80B7EB7C(BgIkanaBlock* this, PlayState* play) {
    return this->unk_17B > 10;
}

s32 func_80B7EB94(BgIkanaBlock* this, PlayState* play) {
    s32 pad;
    Vec3f sp70;
    Vec3f sp64;
    Vec3f sp58;
    CollisionPoly* sp54;
    s32 bgId;
    s32 sp4C = false;
    s16 phi_a0;
    f32 phi_f12;

    if (this->dyna.pushForce > 0.0f) {
        sp4C = true;
    }

    if (sp4C) {
        phi_a0 = this->dyna.yRotation;
    } else {
        phi_a0 = BINANG_ROT180(this->dyna.yRotation);
    }

    sp70.x = this->dyna.actor.world.pos.x;
    sp70.y = this->dyna.actor.world.pos.y + this->unk_170 + 2.0f;
    sp70.z = this->dyna.actor.world.pos.z;

    Matrix_RotateYS(phi_a0, MTXMODE_NEW);

    if (sp4C) {
        phi_f12 = 89.0f;
    } else {
        phi_f12 = 119.0f;
    }

    Matrix_MultVecZ(phi_f12, &sp64);

    sp64.x += this->dyna.actor.world.pos.x;
    sp64.y += this->dyna.actor.world.pos.y + this->unk_170 + 2.0f;
    sp64.z += this->dyna.actor.world.pos.z;

    return !BgCheck_EntityLineTest3(&play->colCtx, &sp70, &sp64, &sp58, &sp54, true, false, false, true, &bgId,
                                    &this->dyna.actor, 0.0f);
}

bool func_80B7ECFC(BgIkanaBlock* this, PlayState* play) {
    return func_80B7EB64(this, play) && func_80B7EB7C(this, play) && func_80B7EB94(this, play);
}

void func_80B7ED54(BgIkanaBlock* this) {
    s32 pad;
    Vec3f sp18;

    Matrix_RotateYS(this->dyna.actor.shape.rot.y, MTXMODE_NEW);
    Matrix_RotateXS(this->dyna.actor.shape.rot.x, MTXMODE_APPLY);
    Matrix_RotateZS(this->dyna.actor.shape.rot.z, MTXMODE_APPLY);
    Matrix_MultVecY(30.0f, &sp18);
    this->unk_170 = sp18.y - 30.0f;
}

s32 func_80B7EDC4(BgIkanaBlock* this, PlayState* play) {
    s32 pad;
    Vec3f sp30;
    s32 bgId;

    sp30.x = this->dyna.actor.world.pos.x;
    sp30.y = this->dyna.actor.world.pos.y + this->unk_170 + 40.0f;
    sp30.z = this->dyna.actor.world.pos.z;

    this->dyna.actor.floorHeight = BgCheck_EntityRaycastFloor5_2(play, &play->colCtx, &this->dyna.actor.floorPoly,
                                                                 &bgId, &this->dyna.actor, &sp30);
    this->dyna.actor.floorBgId = bgId;

    return ((this->dyna.actor.world.pos.y + this->unk_170) - this->dyna.actor.floorHeight) < 2.0f;
}

s32 func_80B7EE70(BgIkanaBlock* this, PlayState* play) {
    if (func_80B7EDC4(this, play)) {
        this->dyna.actor.world.pos.y = this->dyna.actor.floorHeight - this->unk_170;
        return true;
    }
    return false;
}

s32 func_80B7EEB4(BgIkanaBlock* this, PlayState* play) {
    func_80B7EDC4(this, play);

    if (this->dyna.actor.floorHeight > (BGCHECK_Y_MIN + 1.0f)) {
        if (DynaPoly_GetActor(&play->colCtx, this->dyna.actor.floorBgId)) {
            this->dyna.actor.world.pos.y = this->dyna.actor.floorHeight - this->unk_170;
            return true;
        }
    }
    return false;
}

void BgIkanaBlock_Init(Actor* thisx, PlayState* play) {
    BgIkanaBlock* this = (BgIkanaBlock*)thisx;

    Actor_ProcessInitChain(&this->dyna.actor, sInitChain);
    DynaPolyActor_Init(&this->dyna, DYNA_TRANSFORM_POS);
    DynaPolyActor_LoadMesh(play, &this->dyna, &gameplay_dangeon_keep_Colheader_007498);
    DynaPoly_DisableCollision(play, &play->colCtx.dyna, this->dyna.bgId);
    this->unk_15C = Lib_SegmentedToVirtual(gameplay_dangeon_keep_Matanimheader_01B370);
    this->unk_174 = this->dyna.actor.shape.rot;
    func_80B7ED54(this);
    func_80B7F00C(this);
}

void BgIkanaBlock_Destroy(Actor* thisx, PlayState* play) {
    BgIkanaBlock* this = (BgIkanaBlock*)thisx;

    DynaPoly_DeleteBgActor(play, &play->colCtx.dyna, this->dyna.bgId);
}

void func_80B7F00C(BgIkanaBlock* this) {
    this->unk_17C &= ~7;
    this->unk_17D = 40;
    this->actionFunc = func_80B7F034;
}

void func_80B7F034(BgIkanaBlock* this, PlayState* play) {
    if (this->unk_17D > 0) {
        this->unk_17D--;
    }

    if (func_80B7EEB4(this, play)) {
        DynaPoly_EnableCollision(play, &play->colCtx.dyna, this->dyna.bgId);
        this->dyna.actor.draw = func_80B7F564;
        func_80B7F0A4(this);
    }
}

void func_80B7F0A4(BgIkanaBlock* this) {
    this->unk_17B = 0;
    this->unk_17C &= ~7;
    this->unk_17C |= 1;
    this->actionFunc = func_80B7F0D0;
}

void func_80B7F0D0(BgIkanaBlock* this, PlayState* play) {
    s32 sp24 = 0;
    s32 phi_v1;

    func_80B7EA60(this);
    func_80B7EB30(this);

    if (this->unk_17C & 4) {
        sp24 = 1;
    } else if (func_80B7ECFC(this, play)) {
        sp24 = 2;
    }

    if ((sp24 != 2) && (this->unk_17A & (0x8 | 0x4 | 0x2 | 0x1))) {
        Player* player = GET_PLAYER(play);

        player->stateFlags2 &= ~PLAYER_STATE2_10;
        this->dyna.pushForce = 0.0f;
    }

    if (sp24 == 1) {
        func_80B7F360(this);
    } else if (sp24 == 2) {
        func_80B7F1A8(this);
    }
}

void func_80B7F1A8(BgIkanaBlock* this) {
    f32 temp_f0;
    f32* phi_v0;
    s32 phi_v0_2;

    if (this->unk_17A & (0x2 | 0x1)) {
        this->unk_164 = &this->dyna.actor.world.pos.x;
        phi_v0 = &this->dyna.actor.home.pos.x;
    } else {
        this->unk_164 = &this->dyna.actor.world.pos.z;
        phi_v0 = &this->dyna.actor.home.pos.z;
    }

    temp_f0 = (*this->unk_164 - *phi_v0) * 0.016666668f;

    if (temp_f0 >= 0.0f) {
        temp_f0 += 0.5f;
    } else {
        temp_f0 -= 0.5f;
    }

    if (this->unk_17A & (0x4 | 0x1)) {
        phi_v0_2 = 1;
    } else {
        phi_v0_2 = -1;
    }

    this->unk_168 = *phi_v0 + (((s32)temp_f0 + phi_v0_2) * 60.0f);
    this->unk_16C = 0.0f;
    this->unk_17C &= ~7;
    this->unk_17C |= 2;
    this->actionFunc = func_80B7F290;
}

void func_80B7F290(BgIkanaBlock* this, PlayState* play) {
    s32 pad;

    Math_StepToF(&this->unk_16C, 2.0f, 0.4f);

    if (GameInteractor_Should(VB_BLOCK_BE_FINISHED_PULLING, Math_StepToF(this->unk_164, this->unk_168, this->unk_16C),
                              this->unk_164, this->unk_168, this->unk_16C, this->unk_16C)) {
        Player* player = GET_PLAYER(play);

        if (!func_80B7EB94(this, play)) {
            Actor_PlaySfx(&this->dyna.actor, NA_SE_EV_BLOCK_BOUND);
        }

        player->stateFlags2 &= ~PLAYER_STATE2_10;
        this->dyna.pushForce = 0.0f;

        if (func_80B7EDC4(this, play)) {
            func_80B7F0A4(this);
        } else {
            func_80B7F360(this);
        }
    } else {
        Actor_PlaySfx_Flagged(&this->dyna.actor, NA_SE_EV_ROCK_SLIDE - SFX_FLAG);
    }
}

void func_80B7F360(BgIkanaBlock* this) {
    this->unk_17C &= ~7;
    this->unk_17C |= 4;
    this->unk_17D = 1;
    this->dyna.actor.velocity.y = 0.0f;
    this->actionFunc = func_80B7F398;
}

void func_80B7F398(BgIkanaBlock* this, PlayState* play) {
    if (this->unk_17D > 0) {
        this->unk_17D--;
        return;
    }

    this->dyna.actor.velocity.y -= 2.0f;
    this->dyna.actor.velocity.y *= 0.95f;

    if (this->dyna.actor.velocity.y < -30.0f) {
        this->dyna.actor.velocity.y = -30.0f;
    }

    this->dyna.actor.world.pos.y += this->dyna.actor.velocity.y;

    if (func_80B7EE70(this, play)) {
        Actor_PlaySfx(&this->dyna.actor, NA_SE_EV_BLOCK_BOUND);
        Actor_PlaySfx(&this->dyna.actor,
                      NA_SE_PL_WALK_GROUND + SurfaceType_GetSfxOffset(&play->colCtx, this->dyna.actor.floorPoly,
                                                                      this->dyna.actor.floorBgId));
        func_80B7F0A4(this);
    }
}

void BgIkanaBlock_Update(Actor* thisx, PlayState* play) {
    BgIkanaBlock* this = (BgIkanaBlock*)thisx;

    if ((this->dyna.actor.shape.rot.x != this->unk_174.x) || (this->dyna.actor.shape.rot.y != this->unk_174.y) ||
        (this->dyna.actor.shape.rot.z != this->unk_174.z)) {
        this->unk_174 = this->dyna.actor.shape.rot;
        func_80B7ED54(this);
    }

    this->actionFunc(this, play);

    Actor_SetFocus(&this->dyna.actor, 30.0f);

    if (this->unk_17E != 0) {
        if (this->unk_17F > 0) {
            this->unk_17F--;
            CutsceneManager_Stop(this->dyna.actor.csId);
            this->unk_17E = 0;
        } else if (CutsceneManager_IsNext(this->dyna.actor.csId)) {
            CutsceneManager_StartWithPlayerCs(this->dyna.actor.csId, &this->dyna.actor);
            this->unk_17F = 30;
        } else {
            CutsceneManager_Queue(this->dyna.actor.csId);
        }
    }
}

void func_80B7F564(Actor* thisx, PlayState* play) {
    s32 pad;
    BgIkanaBlock* this = (BgIkanaBlock*)thisx;

    OPEN_DISPS(play->state.gfxCtx);

    Gfx_SetupDL25_Opa(play->state.gfxCtx);
    AnimatedMat_DrawStep(play, this->unk_15C, 0);

    MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx);
    gDPSetPrimColor(POLY_OPA_DISP++, 0xFF, 0xFF, 255, 255, 255, 255);
    gSPDisplayList(POLY_OPA_DISP++, gameplay_dangeon_keep_DL_0182A8);

    CLOSE_DISPS(play->state.gfxCtx);
}
