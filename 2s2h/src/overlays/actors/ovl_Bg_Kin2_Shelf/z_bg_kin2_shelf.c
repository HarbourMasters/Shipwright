/*
 * File: z_bg_kin2_shelf.c
 * Overlay: ovl_Bg_Kin2_Shelf
 * Description: Ocean Spider House - Drawers
 */

#include "z_bg_kin2_shelf.h"
#include "objects/object_kin2_obj/object_kin2_obj.h"

#define FLAGS 0x00000000

void BgKin2Shelf_Init(Actor* thisx, PlayState* play);
void BgKin2Shelf_Destroy(Actor* thisx, PlayState* play);
void BgKin2Shelf_Update(Actor* thisx, PlayState* play);
void BgKin2Shelf_Draw(Actor* thisx, PlayState* play);

void func_80B700A8(BgKin2Shelf* this);
void func_80B700C0(BgKin2Shelf* this, PlayState* play);
void func_80B70214(BgKin2Shelf* this);
void func_80B70230(BgKin2Shelf* this, PlayState* play);
void func_80B70498(BgKin2Shelf* this);
void func_80B704B4(BgKin2Shelf* this, PlayState* play);

ActorProfile Bg_Kin2_Shelf_Profile = {
    /**/ ACTOR_BG_KIN2_SHELF,
    /**/ ACTORCAT_BG,
    /**/ FLAGS,
    /**/ OBJECT_KIN2_OBJ,
    /**/ sizeof(BgKin2Shelf),
    /**/ BgKin2Shelf_Init,
    /**/ BgKin2Shelf_Destroy,
    /**/ BgKin2Shelf_Update,
    /**/ BgKin2Shelf_Draw,
};

f32 D_80B70750[] = { 60.0f, 120.0f };

f32 D_80B70758[] = { 40.0f, 0.0f };

f32 D_80B70760[] = { 10.0f, 15.0f };

f32 D_80B70768[] = { 15.0f, 60.0f };

f32 D_80B70770[] = { 10.0f, 15.0f };

u8 D_80B70778[] = { 0x0F, 0x0A };

static InitChainEntry sInitChain[] = {
    ICHAIN_F32(cullingVolumeDistance, 4000, ICHAIN_STOP),
};

CollisionHeader* D_80B70780[] = { &gOceanSpiderHouseChestOfDrawersCol, &gOceanSpiderHouseBookshelfCol };

Gfx* D_80B70788[] = { gOceanSpiderHouseChestOfDrawersDL, gOceanSpiderHouseBookshelfDL };

s32 func_80B6FB30(BgKin2Shelf* this, PlayState* play) {
    s32 temp_v1 = BGKIN2SHELF_GET_1(&this->dyna.actor);
    f32 phi_f0;
    f32 spA4;
    s16 spA2;
    Vec3f sp94;
    Vec3f sp88;
    Vec3f sp7C;
    Vec3f sp70;
    Vec3f sp64;
    Vec3f sp58;
    CollisionPoly* sp54;
    s32 bgId;

    if (this->unk_164 & 2) {
        spA2 = 0x4000;
    } else {
        spA2 = -0x4000;
    }

    spA4 = D_80B70750[temp_v1] + ((this->dyna.pushForce < 0.0f) ? 28.0f : -2.0f) + D_80B70768[temp_v1];

    Matrix_Translate(0.0f, 10.0f, D_80B70760[temp_v1], MTXMODE_NEW);
    Matrix_RotateYS(spA2, MTXMODE_APPLY);

    sp58.x = 0.0f;
    sp58.y = 0.0f;
    sp58.z = 0.0f;

    Matrix_MultVec3f(&sp58, &sp94);

    sp58.z = spA4;

    Matrix_MultVec3f(&sp58, &sp88);
    Matrix_SetTranslateRotateYXZ(this->dyna.actor.world.pos.x, this->dyna.actor.world.pos.y,
                                 this->dyna.actor.world.pos.z, &this->dyna.actor.shape.rot);
    Matrix_MultVec3f(&sp94, &sp7C);
    Matrix_MultVec3f(&sp88, &sp70);

    return BgCheck_EntityLineTest3(&play->colCtx, &sp7C, &sp70, &sp64, &sp54, true, false, false, true, &bgId,
                                   &this->dyna.actor, 0.0f);
}

s32 func_80B6FCA4(BgKin2Shelf* this, PlayState* play) {
    s32 pad;
    f32 spA0;
    s16 sp9E;
    Vec3f sp90;
    Vec3f sp84;
    Vec3f sp78;
    Vec3f sp6C;
    Vec3f sp60;
    Vec3f sp54;
    CollisionPoly* sp50;
    s32 bgId;

    if (this->unk_164 & 1) {
        sp9E = 0;
    } else {
        sp9E = -0x8000;
    }

    spA0 = D_80B70758[0] + ((this->dyna.pushForce < 0.0f) ? 28.0f : -2.0f) + D_80B70770[0];

    Matrix_Translate(0.0f, 10.0f, *D_80B70760, MTXMODE_NEW);
    Matrix_RotateYS(sp9E, MTXMODE_APPLY);

    sp54.x = 0.0f;
    sp54.y = 0.0f;
    sp54.z = 0.0f;

    Matrix_MultVec3f(&sp54, &sp90);

    sp54.z = spA0;

    Matrix_MultVec3f(&sp54, &sp84);
    Matrix_SetTranslateRotateYXZ(this->dyna.actor.world.pos.x, this->dyna.actor.world.pos.y,
                                 this->dyna.actor.world.pos.z, &this->dyna.actor.shape.rot);
    Matrix_MultVec3f(&sp90, &sp78);
    Matrix_MultVec3f(&sp84, &sp6C);

    return BgCheck_EntityLineTest3(&play->colCtx, &sp78, &sp6C, &sp60, &sp50, true, false, false, true, &bgId,
                                   &this->dyna.actor, 0.0f);
}

s32 func_80B6FE08(BgKin2Shelf* this, PlayState* play) {
    if (this->unk_164 & (0x8 | 0x2)) {
        return func_80B6FB30(this, play);
    }
    return func_80B6FCA4(this, play);
}

s32 func_80B6FE48(BgKin2Shelf* this, PlayState* play) {
    s32 params = BGKIN2SHELF_GET_1(&this->dyna.actor);
    u8 temp_v1 = D_80B70778[params] & this->unk_164;

    if ((this->unk_166 != 0) && (this->unk_167 != 0)) {
        return false;
    }

    if ((this->unk_166 == 0) && (this->unk_167 == 0)) {
        return temp_v1 != 0;
    }

    if (this->unk_166 != 0) {
        return temp_v1 & (0x8 | 0x2);
    }

    return temp_v1 & 5;
}

s32 func_80B6FEBC(BgKin2Shelf* this, PlayState* play) {
    if (this->unk_164 & 1) {
        return this->unk_167 < 1;
    }

    if (this->unk_164 & 2) {
        return this->unk_166 < 1;
    }

    if (this->unk_164 & 4) {
        return this->unk_167 >= 0;
    }

    if (this->unk_164 & 8) {
        return this->unk_166 >= 0;
    }
    return false;
}

bool func_80B6FF28(BgKin2Shelf* this, PlayState* play) {
    return (this->unk_165 > 8) && func_80B6FE48(this, play) && func_80B6FEBC(this, play) && !func_80B6FE08(this, play);
}

void BgKin2Shelf_Init(Actor* thisx, PlayState* play) {
    s32 pad;
    BgKin2Shelf* this = (BgKin2Shelf*)thisx;
    s32 sp24 = BGKIN2SHELF_GET_1(&this->dyna.actor);

    Actor_ProcessInitChain(&this->dyna.actor, sInitChain);

    if (sp24 == 0) {
        this->dyna.actor.cullingVolumeScale = 150.0f;
        this->dyna.actor.cullingVolumeDownward = 140.0f;
        Actor_SetScale(&this->dyna.actor, 0.1f);
    } else {
        this->dyna.actor.cullingVolumeScale = 250.0f;
        this->dyna.actor.cullingVolumeDownward = 300.0f;
        Actor_SetScale(&this->dyna.actor, 1.0f);
        this->dyna.actor.flags |= ACTOR_FLAG_UCODE_POINT_LIGHT_ENABLED;
    }

    DynaPolyActor_Init(&this->dyna, DYNA_TRANSFORM_POS);
    DynaPolyActor_LoadMesh(play, &this->dyna, D_80B70780[sp24]);
    func_80B700A8(this);
}

void BgKin2Shelf_Destroy(Actor* thisx, PlayState* play) {
    BgKin2Shelf* this = (BgKin2Shelf*)thisx;

    DynaPoly_DeleteBgActor(play, &play->colCtx.dyna, this->dyna.bgId);
}

void func_80B700A8(BgKin2Shelf* this) {
    this->unk_165 = 0;
    this->actionFunc = func_80B700C0;
}

void func_80B700C0(BgKin2Shelf* this, PlayState* play) {
    s16 temp_v0;
    u8 phi_v0_2;

    if (fabsf(this->dyna.pushForce) > 0.1f) {
        temp_v0 = this->dyna.yRotation - this->dyna.actor.shape.rot.y;
        if (this->dyna.pushForce < 0.0f) {
            temp_v0 = BINANG_ROT180(temp_v0);
        }

        if ((temp_v0 < -0x6000) || (temp_v0 >= 0x6000)) {
            phi_v0_2 = 4;
        } else if (temp_v0 < -0x2000) {
            phi_v0_2 = 8;
        } else if (temp_v0 < 0x2000) {
            phi_v0_2 = 1;
        } else {
            phi_v0_2 = 2;
        }

        if ((phi_v0_2 == this->unk_164) || (this->unk_164 == 0)) {
            this->unk_164 = phi_v0_2;
            this->unk_165++;
        } else {
            this->unk_165 = 0;
            this->unk_164 = 0;
        }

        if (func_80B6FF28(this, play)) {
            if (this->unk_164 & (0x8 | 0x2)) {
                func_80B70214(this);
            } else {
                func_80B70498(this);
            }
        } else {
            Player* player = GET_PLAYER(play);

            player->stateFlags2 &= ~PLAYER_STATE2_10;
            this->dyna.pushForce = 0.0f;
        }
    } else {
        this->unk_165 = 0;
        this->unk_164 = 0;
    }
}

void func_80B70214(BgKin2Shelf* this) {
    this->unk_160 = 0.0f;
    this->actionFunc = func_80B70230;
}

void func_80B70230(BgKin2Shelf* this, PlayState* play) {
    f32 phi_f20;
    s32 sp40 = BGKIN2SHELF_GET_1(&this->dyna.actor);
    f32 phi_f2;
    s32 pad;
    s16 sp36 = BINANG_ADD(this->dyna.actor.shape.rot.y, 0x4000);

    if ((BGKIN2SHELF_GET_1(&this->dyna.actor)) == 0) {
        phi_f2 = (Math_SinS(this->unk_160 * 0x8000) * 0.012f) + 0.014f;
    } else {
        phi_f2 = (Math_SinS(this->unk_160 * 0x8000) * 0.003f) + 0.009f;
    }
    this->unk_160 += phi_f2;

    if (this->unk_160 >= 1.0f) {
        Player* player = GET_PLAYER(play);

        this->unk_160 = 1.0f;
        player->stateFlags2 &= ~PLAYER_STATE2_10;
        this->dyna.pushForce = 0.0f;

        if (this->unk_164 & 8) {
            this->unk_166--;
        } else {
            this->unk_166++;
        }

        phi_f20 = this->unk_166;

        this->dyna.actor.world.pos.x = ((Math_SinS(sp36) * phi_f20) * D_80B70750[sp40]) + this->dyna.actor.home.pos.x;
        this->dyna.actor.world.pos.z = ((Math_CosS(sp36) * phi_f20) * D_80B70750[sp40]) + this->dyna.actor.home.pos.z;

        if (this->unk_166 != 0) {
            Audio_PlaySfx(NA_SE_SY_TRE_BOX_APPEAR);
        }
        func_80B700A8(this);
    } else {
        if (this->unk_164 & 8) {
            phi_f20 = this->unk_166 - this->unk_160;
        } else {
            phi_f20 = this->unk_166 + this->unk_160;
        }
        this->dyna.actor.world.pos.x = ((Math_SinS(sp36) * phi_f20) * D_80B70750[sp40]) + this->dyna.actor.home.pos.x;
        this->dyna.actor.world.pos.z = ((Math_CosS(sp36) * phi_f20) * D_80B70750[sp40]) + this->dyna.actor.home.pos.z;
    }
}

void func_80B70498(BgKin2Shelf* this) {
    this->unk_160 = 0.0f;
    this->actionFunc = func_80B704B4;
}

void func_80B704B4(BgKin2Shelf* this, PlayState* play) {
    f32 temp_f20;
    s32 sp40 = BGKIN2SHELF_GET_1(&this->dyna.actor);
    f32 phi_f20;
    s32 pad;
    s16 sp36 = this->dyna.actor.shape.rot.y;

    this->unk_160 += (Math_SinS(this->unk_160 * 0x8000) * 0.022f) + 0.022f;

    if (this->unk_160 >= 1.0f) {
        Player* player = GET_PLAYER(play);

        this->unk_160 = 1.0f;
        player->stateFlags2 &= ~PLAYER_STATE2_10;
        this->dyna.pushForce = 0.0f;

        if (this->unk_164 & 4) {
            this->unk_167--;
        } else {
            this->unk_167++;
        }

        temp_f20 = this->unk_167;
        this->dyna.actor.world.pos.x = ((Math_SinS(sp36) * temp_f20) * D_80B70758[sp40]) + this->dyna.actor.home.pos.x;
        this->dyna.actor.world.pos.z = ((Math_CosS(sp36) * temp_f20) * D_80B70758[sp40]) + this->dyna.actor.home.pos.z;

        if (this->unk_167 != 0) {
            Audio_PlaySfx(NA_SE_SY_TRE_BOX_APPEAR);
        }
        func_80B700A8(this);
    } else {
        if (this->unk_164 & 4) {
            temp_f20 = this->unk_167 - this->unk_160;
        } else {
            temp_f20 = this->unk_167 + this->unk_160;
        }
        this->dyna.actor.world.pos.x = ((Math_SinS(sp36) * temp_f20) * D_80B70758[sp40]) + this->dyna.actor.home.pos.x;
        this->dyna.actor.world.pos.z = ((Math_CosS(sp36) * temp_f20) * D_80B70758[sp40]) + this->dyna.actor.home.pos.z;
    }
}

void BgKin2Shelf_Update(Actor* thisx, PlayState* play) {
    BgKin2Shelf* this = (BgKin2Shelf*)thisx;

    this->actionFunc(this, play);
}

void BgKin2Shelf_Draw(Actor* thisx, PlayState* play) {
    Gfx_DrawDListOpa(play, D_80B70788[BGKIN2SHELF_GET_1(thisx)]);
}
