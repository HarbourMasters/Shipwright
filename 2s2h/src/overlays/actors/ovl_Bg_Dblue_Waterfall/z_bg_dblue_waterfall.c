/*
 * File: z_bg_dblue_waterfall.c
 * Overlay: ovl_Bg_Dblue_Waterfall
 * Description: Great Bay Temple - Freezable Waterfall
 */

#include "z_bg_dblue_waterfall.h"
#include "objects/object_dblue_object/object_dblue_object.h"

#define FLAGS (ACTOR_FLAG_UPDATE_CULLING_DISABLED)

void BgDblueWaterfall_Init(Actor* thisx, PlayState* play);
void BgDblueWaterfall_Destroy(Actor* thisx, PlayState* play);
void BgDblueWaterfall_Update(Actor* thisx, PlayState* play);
void BgDblueWaterfall_Draw(Actor* thisx, PlayState* play);

void func_80B8484C(BgDblueWaterfall* this, PlayState* play);
void func_80B84928(BgDblueWaterfall* this, PlayState* play);
void func_80B84AD4(BgDblueWaterfall* this, PlayState* play);
void func_80B84AEC(BgDblueWaterfall* this, PlayState* play);
void func_80B84B9C(BgDblueWaterfall* this, PlayState* play);
void func_80B84BCC(BgDblueWaterfall* this, PlayState* play);
void func_80B84EF0(BgDblueWaterfall* this, PlayState* play);
void func_80B84F20(BgDblueWaterfall* this, PlayState* play);

ActorProfile Bg_Dblue_Waterfall_Profile = {
    /**/ ACTOR_BG_DBLUE_WATERFALL,
    /**/ ACTORCAT_PROP,
    /**/ FLAGS,
    /**/ OBJECT_DBLUE_OBJECT,
    /**/ sizeof(BgDblueWaterfall),
    /**/ BgDblueWaterfall_Init,
    /**/ BgDblueWaterfall_Destroy,
    /**/ BgDblueWaterfall_Update,
    /**/ BgDblueWaterfall_Draw,
};

static ColliderCylinderInit sCylinderInit = {
    {
        COL_MATERIAL_NONE,
        AT_NONE,
        AC_ON | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_2,
        COLSHAPE_CYLINDER,
    },
    {
        ELEM_MATERIAL_UNK4,
        { 0x00000000, 0x00, 0x00 },
        { 0x00CBFBB0, 0x00, 0x00 },
        ATELEM_NONE | ATELEM_SFX_NORMAL,
        ACELEM_ON,
        OCELEM_ON,
    },
    { 50, 740, -740, { 0, 0, 0 } },
};

Color_RGBA8 D_80B8539C = { 200, 255, 255, 255 };
Color_RGBA8 D_80B853A0 = { 200, 200, 200, 0 };

s32 func_80B83C80(Vec3f* arg0, Vec3f* arg1) {
    f32 temp_f0 = Math3D_Vec3fMagnitudeSq(arg0);
    f32 temp_f2;

    if (temp_f0 < 0.0000010000001f) {
        return false;
    }

    temp_f2 = 1.0f / sqrtf(temp_f0);
    arg1->x = arg0->x * temp_f2;
    arg1->y = arg0->y * temp_f2;
    arg1->z = arg0->z * temp_f2;

    return true;
}

s32 func_80B83D04(BgDblueWaterfall* this, PlayState* play) {
    s32 phi_v1;
    s32 sp18 = BGDBLUEWATERFALL_GET_100(&this->actor);

    if (Flags_GetSwitch(play, BGDBLUEWATERFALL_GET_SWITCH_FLAG(&this->actor))) {
        phi_v1 = true;
    } else {
        phi_v1 = false;
    }
    return phi_v1 ^ sp18;
}

s32 func_80B83D58(Actor* thisx, PlayState* play) {
    BgDblueWaterfall* this = (BgDblueWaterfall*)thisx;

    if (Flags_GetSwitch(play, BGDBLUEWATERFALL_GET_SWITCH_FLAG(&this->actor))) {
        return false;
    }
    return true;
}

void func_80B83D94(BgDblueWaterfall* this, PlayState* play) {
    s32 pad;
    s32 switchFlag = BGDBLUEWATERFALL_GET_SWITCH_FLAG(&this->actor);
    s32 sp1C = BGDBLUEWATERFALL_GET_100(&this->actor);
    s32 phi_v0;

    if (Flags_GetSwitch(play, switchFlag)) {
        phi_v0 = true;
    } else {
        phi_v0 = false;
    }

    if (phi_v0 != sp1C) {
        if (phi_v0) {
            Flags_UnsetSwitch(play, switchFlag);
        } else {
            Flags_SetSwitch(play, switchFlag);
        }
    }
}

void func_80B83E1C(BgDblueWaterfall* this, PlayState* play) {
    s32 pad;
    s32 switchFlag = BGDBLUEWATERFALL_GET_SWITCH_FLAG(&this->actor);
    s32 sp1C = BGDBLUEWATERFALL_GET_100(&this->actor);
    s32 phi_v0;

    if (Flags_GetSwitch(play, switchFlag)) {
        phi_v0 = true;
    } else {
        phi_v0 = false;
    }

    if (phi_v0 == sp1C) {
        if (phi_v0) {
            Flags_UnsetSwitch(play, switchFlag);
        } else {
            Flags_SetSwitch(play, switchFlag);
        }
    }
}

void func_80B83EA4(BgDblueWaterfall* this, PlayState* play) {
    s32 i;
    s32 temp_s1;
    Vec3f spD4;
    Vec3f spC8;
    Vec3f spBC;
    Vec3f spB0;
    Vec3f spA4;
    Vec3f sp98;
    s32 phi_s3;

    if (this->collider.elem.acElemFlags & ACELEM_HIT) {
        f32 temp_f0_2;
        f32 temp_f20;

        spB0.x = this->collider.elem.acDmgInfo.hitPos.x;
        spB0.y = this->collider.elem.acDmgInfo.hitPos.y;
        spB0.z = this->collider.elem.acDmgInfo.hitPos.z;

        sp98.x = spB0.x - this->actor.world.pos.x;
        sp98.y = 0.0f;
        sp98.z = spB0.z - this->actor.world.pos.z;

        if (!func_80B83C80(&sp98, &spA4)) {
            Math_Vec3f_Copy(&spA4, &gZeroVec3f);
        }

        sp98.y = 0.0f;
        spBC.y = -1.4f;

        for (i = 0, phi_s3 = 0; i < 15; i++, phi_s3 += 0x1111) {
            temp_s1 = (s32)(Rand_ZeroOne() * 4369.0f) + phi_s3;
            temp_f0_2 = Rand_ZeroOne();
            temp_f20 = 1.0f - SQ(temp_f0_2);

            sp98.x = Math_SinS(temp_s1) * temp_f20;
            sp98.z = Math_CosS(temp_s1) * temp_f20;
            temp_f20 *= 5.0f;

            spD4.x = (sp98.x * temp_f20) + spB0.x;
            spD4.y = ((Rand_ZeroOne() * 16.0f) - 8.0f) + spB0.y;
            spD4.z = (sp98.z * temp_f20) + spB0.z;

            spC8.x = ((2.0f * Rand_ZeroOne()) - 1.0f) + ((spA4.x * 3.0f) + (sp98.x * 5.0f));
            spC8.y = (Rand_ZeroOne() * 10.0f) + 5.0f;
            spC8.z = ((2.0f * Rand_ZeroOne()) - 1.0f) + ((spA4.z * 3.0f) + (sp98.z * 5.0f));

            spBC.x = spC8.x * -0.02f;
            spBC.z = spC8.z * -0.02f;

            EffectSsEnIce_Spawn(play, &spD4, (Rand_ZeroOne() * 0.3f) + 0.1f, &spC8, &spBC, &D_80B8539C, &D_80B853A0,
                                30);
        }
    }
}

void func_80B841A0(BgDblueWaterfall* this, PlayState* play) {
    Vec3f sp94;
    s32 i;
    f32 temp_f0;
    f32 temp_f20;
    f32 temp_f22;
    f32 temp_f24;
    f32 temp_f26;
    s32 temp_s3;
    s32 phi_s2;

    if (this->collider.elem.acElemFlags & ACELEM_HIT) {
        temp_f22 = this->collider.elem.acDmgInfo.hitPos.x;
        temp_f24 = this->collider.elem.acDmgInfo.hitPos.y;
        temp_f26 = this->collider.elem.acDmgInfo.hitPos.z;

        for (i = 0, phi_s2 = 0; i < 10; i++, phi_s2 += 0x1999) {
            temp_s3 = (s32)(Rand_ZeroOne() * 6553.0f) + phi_s2;
            temp_f0 = Rand_ZeroOne();
            temp_f20 = (1.0f - SQ(temp_f0)) * 14.0f;

            sp94.x = (Math_SinS(temp_s3) * temp_f20) + temp_f22;
            sp94.y = ((Rand_ZeroOne() * 20.0f) - 10.0f) + temp_f24;
            sp94.z = (Math_CosS(temp_s3) * temp_f20) + temp_f26;

            EffectSsGSplash_Spawn(play, &sp94, NULL, NULL, 0, 250);
        }

        Actor_PlaySfx(&this->actor, NA_SE_IT_REFLECTION_WATER);
    }
}

void func_80B84348(BgDblueWaterfall* this, PlayState* play, f32 arg2, f32 arg3, f32 arg4, f32 arg5, f32 arg6, f32 arg7,
                   s32 arg8) {
    static s16 D_80B853A4 = 0;
    s32 pad;
    f32 spB8 = 1.0f / arg8;
    s32 i;
    f32 spB0 = this->actor.world.pos.y + arg3;
    s32 pad2;
    f32 temp_f22;
    f32 temp_f24;
    Vec3f sp98;
    Vec3f sp8C;
    Vec3f sp80;
    f32 temp_f26;
    f32 temp_f20;

    for (i = 0; i < arg8; i++) {
        D_80B853A4 += 0x4E20;

        temp_f20 = Math_SinS(D_80B853A4);
        temp_f24 = (Rand_ZeroOne() * (arg2 - arg3)) + (spB8 * i);
        temp_f22 = Math_CosS(D_80B853A4);
        temp_f26 = ((Rand_ZeroOne() * 0.8f) + 0.2f) * arg4;

        sp8C.x = temp_f20 * temp_f26;
        sp8C.y = (Rand_ZeroOne() * 4.0f) + arg5;
        sp8C.z = temp_f22 * temp_f26;

        sp80.x = sp8C.x * -0.016f;
        sp80.y = sp8C.y * -0.015f;
        sp80.z = sp8C.z * -0.016f;

        sp98.x = (temp_f20 * 50.0f) + this->actor.world.pos.x;
        sp98.y = temp_f24 + spB0;
        sp98.z = (temp_f22 * 50.0f) + this->actor.world.pos.z;

        EffectSsIceSmoke_Spawn(play, &sp98, &sp8C, &sp80, (Rand_ZeroOne() * arg7) + arg6);
    }
}

void func_80B84568(BgDblueWaterfall* this, PlayState* play) {
    s32 pad;
    CollisionPoly* sp40;
    WaterBox* waterBox;
    s32 bgId;
    f32 sp34 = BgCheck_EntityRaycastFloor5(&play->colCtx, &sp40, &bgId, &this->actor, &this->actor.world.pos);
    f32 sp30;

    if (WaterBox_GetSurface1_2(play, &play->colCtx, this->actor.world.pos.x, this->actor.world.pos.z, &sp30,
                               &waterBox)) {
        if (sp30 < sp34) {
            this->unk_198 = sp34;
        } else {
            this->unk_198 = sp30;
        }
    } else {
        this->unk_198 = sp34;
    }
}

void func_80B84610(BgDblueWaterfall* this, PlayState* play) {
    s32 pad;
    Player* player = GET_PLAYER(play);
    Vec3f sp34;

    if (this->unk_1A7 <= 0) {
        this->unk_1A7 = 16;
    } else {
        this->unk_1A7 -= (s8)(Rand_Next() >> 0x1F);
    }

    if (this->unk_1A7 >= 6) {
        this->unk_1A8 += Rand_ZeroOne() * 0.1f;
        this->unk_1A8 = CLAMP_MAX(this->unk_1A8, 0.5f);
    } else {
        this->unk_1A8 -= Rand_ZeroOne() * 0.2f;
        this->unk_1A8 = CLAMP_MAX(this->unk_1A8, -0.5f);
    }

    Matrix_Push();
    Matrix_RotateYS(BINANG_ADD(this->actor.yawTowardsPlayer, 0x4000), MTXMODE_NEW);
    Matrix_MultVecZ(this->unk_1A8, &sp34);
    Matrix_Pop();

    player->actor.world.pos.x += sp34.x;
    player->actor.world.pos.z += sp34.z;

    {
        s32 requiredScopeTemp;

        player->pushedSpeed = 8.0f;
        player->pushedYaw = this->actor.yawTowardsPlayer;
    }
}

static InitChainEntry sInitChain[] = {
    ICHAIN_F32(cullingVolumeDistance, 4000, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeScale, 1500, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeDownward, 1500, ICHAIN_CONTINUE),
    ICHAIN_VEC3F_DIV1000(scale, 100, ICHAIN_STOP),
};

void BgDblueWaterfall_Init(Actor* thisx, PlayState* play) {
    s32 pad;
    BgDblueWaterfall* this = (BgDblueWaterfall*)thisx;

    Actor_ProcessInitChain(&this->actor, sInitChain);
    this->actor.shape.rot.z = 0;
    this->actor.world.rot.z = 0;

    Collider_InitCylinder(play, &this->collider);
    Collider_SetCylinder(play, &this->collider, &this->actor, &sCylinderInit);
    Collider_UpdateCylinder(&this->actor, &this->collider);

    this->unk_190 = Lib_SegmentedToVirtual(gGreatBayTempleObjectWaterfallTexAnim);

    Actor_SetFocus(&this->actor, -100.0f);
    func_80B84568(this, play);
    func_80B8484C(this, play);
}

void BgDblueWaterfall_Destroy(Actor* thisx, PlayState* play) {
    BgDblueWaterfall* this = (BgDblueWaterfall*)thisx;

    Collider_DestroyCylinder(play, &this->collider);
}

void func_80B8484C(BgDblueWaterfall* this, PlayState* play) {
    s32 pad;
    s32 temp = func_80B83D04(this, play);
    s32 sp1C = this->actor.home.rot.z * 10;

    if (temp) {
        this->unk_19E = 0;
        this->unk_19F = 255;
        this->unk_1A0 = 0;
        this->collider.dim.radius = 55;
        this->collider.dim.height = 150;
        this->collider.dim.yShift = -150;
        this->actor.colChkInfo.mass = MASS_IMMOVABLE;
    } else {
        this->unk_19E = 255;
        this->unk_19F = 0;
        this->unk_1A0 = 0;
        this->collider.dim.radius = 50;
        this->collider.dim.height = 740;
        this->collider.dim.yShift = -740;
        this->actor.colChkInfo.mass = MASS_IMMOVABLE;
    }

    if ((sp1C != 0) && !func_80B83D58(&this->actor, play)) {
        this->unk_19C = sp1C;
    } else {
        this->unk_19C = 0;
    }

    this->unk_1A3 = false;
    this->actionFunc = func_80B84928;
}

void func_80B84928(BgDblueWaterfall* this, PlayState* play) {
    s32 pad;
    s32 sp30 = func_80B83D04(this, play);
    s32 sp2C = (this->collider.base.acFlags & AC_HIT) != 0;
    s32 sp28 = false;

    if (sp2C) {
        this->collider.base.acFlags &= ~AC_HIT;
    }

    if ((sp30 == 0) && (this->collider.base.ocFlags2 & OC2_HIT_PLAYER)) {
        func_80B84610(this, play);
    }

    if (this->unk_19C > 0) {
        this->unk_19C--;
        if (this->unk_19C == 0) {
            if (sp30 != 0) {
                func_80B83D94(this, play);
                func_80B84EF0(this, play);
            } else {
                func_80B83E1C(this, play);
                func_80B84B9C(this, play);
            }
            sp28 = true;
        }
    }

    if (!sp28) {
        if (sp2C) {
            if (sp30 != 0) {
                func_80B83EA4(this, play);
                if (this->collider.elem.acHitElem->atDmgInfo.dmgFlags & 0x800) {
                    this->csId = this->actor.csId;
                    func_80B84AD4(this, play);
                }
            } else {
                func_80B841A0(this, play);
                if (this->collider.elem.acHitElem->atDmgInfo.dmgFlags & 0x1000) {
                    this->csId = CutsceneManager_GetAdditionalCsId(this->actor.csId);
                    func_80B84AD4(this, play);
                }
            }
        } else {
            CollisionCheck_SetAC(play, &play->colChkCtx, &this->collider.base);
            CollisionCheck_SetOC(play, &play->colChkCtx, &this->collider.base);
        }
    }
}

void func_80B84AD4(BgDblueWaterfall* this, PlayState* play) {
    this->actionFunc = func_80B84AEC;
}

void func_80B84AEC(BgDblueWaterfall* this, PlayState* play) {
    s32 pad;
    s32 sp20;

    if (CutsceneManager_IsNext(this->csId)) {
        sp20 = func_80B83D04(this, play);
        CutsceneManager_StartWithPlayerCs(this->csId, &this->actor);
        this->unk_1A3 = true;
        if (sp20) {
            func_80B83D94(this, play);
            func_80B84EF0(this, play);
        } else {
            func_80B83E1C(this, play);
            func_80B84B9C(this, play);
        }
    } else {
        CutsceneManager_Queue(this->csId);
    }
}

void func_80B84B9C(BgDblueWaterfall* this, PlayState* play) {
    this->unk_19C = 60;
    this->unk_19E = 255;
    this->unk_19F = 0;
    this->unk_1A0 = 0;
    this->actionFunc = func_80B84BCC;
}

void func_80B84BCC(BgDblueWaterfall* this, PlayState* play) {
    s32 pad;

    this->unk_19C--;
    if (this->unk_19C > 0) {
        s32 sp38 = this->unk_19C & 1;

        if (this->unk_19C > 56) {
            func_80B84348(this, play, 0.0f, -5500.0f, 0.4f, -15.0f, 370.0f, 100.0f, 6);
        }

        if (!sp38) {
            if ((this->unk_19C < 24) && (this->unk_19C > 10)) {
                if (this->unk_198 > -32000.0f) {
                    func_80B84348(this, play, (this->unk_198 - this->actor.world.pos.y) + 50.0f,
                                  (this->unk_198 - this->actor.world.pos.y), 7.0f, -1.0f, 280.0f, 100.0f, 3);
                }
            }

            if (this->unk_19C > 50) {
                func_80B84348(this, play, 0.0f, -400.0f, 0.6f, -12.0f, 370.0f, 100.0f, 2);
            } else if (this->unk_19C > 40) {
                func_80B84348(this, play, 0.0f, -400.0f, 1.0f, -12.0f, 370.0f, 100.0f, 2);
            } else if (this->unk_19C > 20) {
                func_80B84348(this, play, 0.0f, -400.0f, 1.8f, -12.0f, 370.0f, 100.0f, 2);
            }
        }

        if (this->unk_19E > 6) {
            this->unk_19E -= 6;
        } else {
            this->unk_19E = 0;
        }

        if (this->unk_19C >= 55) {
            if (this->unk_1A0 < 205) {
                this->unk_1A0 += 50;
            } else {
                this->unk_1A0 = 255;
            }
        } else if (this->unk_19C >= 36) {
            if (this->unk_1A0 >= 16) {
                this->unk_1A0 -= 15;
            } else {
                this->unk_1A0 = 0;
            }
        } else {
            this->unk_1A0 = 0;
        }

        if (this->unk_19C < 60) {
            if (this->unk_19F < 245) {
                this->unk_19F += 10;
            } else {
                this->unk_19F = 255;
            }
        }

        Actor_PlaySfx_Flagged(&this->actor, NA_SE_EV_ICE_FREEZE - SFX_FLAG);
    } else {
        if (this->unk_1A3) {
            CutsceneManager_Stop(this->csId);
        }
        func_80B8484C(this, play);
    }
}

void func_80B84EF0(BgDblueWaterfall* this, PlayState* play) {
    this->unk_19C = 60;
    this->unk_19E = 0;
    this->unk_19F = 255;
    this->unk_1A0 = 0;
    this->actionFunc = func_80B84F20;
}

void func_80B84F20(BgDblueWaterfall* this, PlayState* play) {
    this->unk_19C--;

    if (this->unk_19C > 0) {
        if (this->unk_19C >= 58) {
            func_80B84348(this, play, 100.0f, -100.0f, 4.0f, -6.0f, 370.0f, 100.0f, 0xC);
        }

        if (this->unk_19C < 50) {
            if (this->unk_19E < 250) {
                this->unk_19E += 5;
            } else {
                this->unk_19E = 255;
            }
        }

        if (this->unk_19F >= 6) {
            this->unk_19F -= 5;
        } else {
            this->unk_19F = 0;
        }

        Actor_PlaySfx_Flagged(&this->actor, NA_SE_EV_ICE_MELT_LEVEL - SFX_FLAG);
    } else {
        if (this->unk_1A3) {
            CutsceneManager_Stop(this->csId);
        }
        func_80B8484C(this, play);
    }
}

void BgDblueWaterfall_Update(Actor* thisx, PlayState* play) {
    BgDblueWaterfall* this = (BgDblueWaterfall*)thisx;

    this->actionFunc(this, play);
}

void BgDblueWaterfall_Draw(Actor* thisx, PlayState* play) {
    s32 pad;
    BgDblueWaterfall* this = (BgDblueWaterfall*)thisx;

    OPEN_DISPS(play->state.gfxCtx);

    Gfx_SetupDL25_Xlu(play->state.gfxCtx);

    MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx);

    if (this->unk_19E > 0) {
        s32 sp38 = this->unk_19E * 0.49803922f;

        AnimatedMat_Draw(play, this->unk_190);

        gDPSetPrimColor(POLY_XLU_DISP++, 0, 0x8A, 255, 255, 255, sp38);
        gSPDisplayList(POLY_XLU_DISP++, gGreatBayTempleObjectWaterfallDL);
    }

    if (this->unk_19F > 0) {
        if (this->unk_19F < 255) {
            gSPSegment(POLY_XLU_DISP++, 0x09, D_801AEF88);
            gDPSetPrimColor(POLY_XLU_DISP++, 0, 0x9B, 255, 255, 255, this->unk_19F);
            gSPDisplayList(POLY_XLU_DISP++, gGreatBayTempleObjectIceStalactiteDL);
        } else {
            Gfx_SetupDL25_Opa(play->state.gfxCtx);

            gSPSegment(POLY_OPA_DISP++, 0x09, D_801AEFA0);
            gDPSetPrimColor(POLY_OPA_DISP++, 0, 0x9B, 255, 255, 255, 255);
            MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx);
            gSPDisplayList(POLY_OPA_DISP++, gGreatBayTempleObjectIceStalactiteDL);
        }

        gDPSetPrimColor(POLY_XLU_DISP++, 0, 0xFF, 255, 255, 255, this->unk_19F);
        gSPDisplayList(POLY_XLU_DISP++, gGreatBayTempleObjectIceStalactiteRimDL);
    }

    if (this->unk_1A0 > 0) {
        gDPSetPrimColor(POLY_XLU_DISP++, 0, 0xFF, 255, 255, 255, this->unk_1A0);
        gSPDisplayList(POLY_XLU_DISP++, gGreatBayTempleObjectFrozenWaterfallDL);
    }

    CLOSE_DISPS(play->state.gfxCtx);
}
