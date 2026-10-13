/*
 * File: z_bg_dblue_balance.c
 * Overlay: ovl_Bg_Dblue_Balance
 * Description: Great Bay Temple - Seesaw and Waterwheel w/ Platforms
 */

#include "z_bg_dblue_balance.h"
#include "objects/object_dblue_object/object_dblue_object.h"
#include "prevent_bss_reordering.h"

#define FLAGS 0x00000000

void BgDblueBalance_Init(Actor* thisx, PlayState* play);
void BgDblueBalance_Destroy(Actor* thisx, PlayState* play);
void BgDblueBalance_Update(Actor* thisx, PlayState* play);
void BgDblueBalance_Draw(Actor* thisx, PlayState* play);

void func_80B82DC4(BgDblueBalance* this);
void func_80B82DE0(BgDblueBalance* this, PlayState* play);
void func_80B8330C(Actor* thisx, PlayState* play);
void func_80B83344(BgDblueBalance* this);
void func_80B8335C(BgDblueBalance* this, PlayState* play);
void func_80B833A8(BgDblueBalance* this);
void func_80B833C4(BgDblueBalance* this, PlayState* play);
void func_80B83518(Actor* thisx, PlayState* play);
void func_80B83758(Actor* thisx, PlayState* play);

AnimatedMaterial* D_80B83C70;
AnimatedMaterial* D_80B83C74;

ActorProfile Bg_Dblue_Balance_Profile = {
    /**/ ACTOR_BG_DBLUE_BALANCE,
    /**/ ACTORCAT_BG,
    /**/ FLAGS,
    /**/ OBJECT_DBLUE_OBJECT,
    /**/ sizeof(BgDblueBalance),
    /**/ BgDblueBalance_Init,
    /**/ BgDblueBalance_Destroy,
    /**/ BgDblueBalance_Update,
    /**/ BgDblueBalance_Draw,
};

typedef struct {
    /* 0x00 */ Gfx* opaDList;
    /* 0x04 */ CollisionHeader* colHeader;
    /* 0x08 */ u32 unk_08;
    /* 0x0C */ f32 unk_0C;
    /* 0x10 */ f32 unk_10;
    /* 0x14 */ ActorFunc update;
    /* 0x18 */ ActorFunc draw;
} BgDblueBalanceTypeInfo; // size = 0x1C

BgDblueBalanceTypeInfo sTypeInfo[] = {
    {
        gGreatBayTempleObjectSeesawShaftDL,
        &gGreatBayTempleObjectSeesawShaftCol,
        0x10,
        360.0f,
        300.0f,
        BgDblueBalance_Update,
        BgDblueBalance_Draw,
    },
    {
        gGreatBayTempleObjectLargeSeesawPlatformDL,
        &gGreatBayTempleObjectLargeSeesawPlatformCol,
        0x10,
        210.0f,
        190.0f,
        func_80B8330C,
        BgDblueBalance_Draw,
    },
    {
        gGreatBayTempleObjectSmallSeesawPlatformDL,
        &gGreatBayTempleObjectSmallSeesawPlatformCol,
        0x10,
        180.0f,
        180.0f,
        func_80B8330C,
        BgDblueBalance_Draw,
    },
    {
        gGreatBayTempleObjectWaterwheelWithPlatformsDL,
        &gGreatBayTempleObjectWaterwheelWithPlatformsCol,
        0x30,
        1500.0f,
        1500.0f,
        func_80B83518,
        func_80B83758,
    },
};

s8 D_80B83A90[] = {
    0,
    4,
};
s8 D_80B83A94[] = {
    0,
    2,
    4,
    6,
};

static InitChainEntry sInitChain[] = {
    ICHAIN_F32(cullingVolumeDistance, 4000, ICHAIN_CONTINUE),
    ICHAIN_VEC3F_DIV1000(scale, 100, ICHAIN_STOP),
};

void func_80B823B0(BgDblueBalance* this) {
    s32 pad;
    Vec3f sp28;

    Matrix_Translate(this->dyna.actor.world.pos.x, this->dyna.actor.world.pos.y, this->dyna.actor.world.pos.z,
                     MTXMODE_NEW);
    Matrix_RotateYS(this->dyna.actor.shape.rot.y, MTXMODE_APPLY);
    Matrix_RotateZS(this->dyna.actor.shape.rot.z, MTXMODE_APPLY);

    sp28.x = -138.28f;
    sp28.y = 0.0f;
    sp28.z = 0.0f;

    if (this->unk_15C != NULL) {
        Matrix_MultVec3f(&sp28, &this->unk_15C->dyna.actor.world.pos);
    }

    sp28.x = 138.28f;
    if (this->unk_160 != NULL) {
        Matrix_MultVec3f(&sp28, &this->unk_160->dyna.actor.world.pos);
    }
}

s32 func_80B82454(BgDblueBalance* this, PlayState* play) {
    s32 pad;
    s32 sp58 = true;
    BgDblueBalance* balance;
    Vec3f sp48;
    Vec3f sp3C;

    Matrix_Translate(this->dyna.actor.world.pos.x, this->dyna.actor.world.pos.y, this->dyna.actor.world.pos.z,
                     MTXMODE_NEW);
    Matrix_RotateYS(this->dyna.actor.shape.rot.y, MTXMODE_APPLY);
    Matrix_RotateZS(this->dyna.actor.shape.rot.z, MTXMODE_APPLY);

    sp48.x = -138.28f;
    sp48.y = 0.0f;
    sp48.z = 0.0f;

    Matrix_MultVec3f(&sp48, &sp3C);

    this->unk_15C = (BgDblueBalance*)Actor_Spawn(&play->actorCtx, play, ACTOR_BG_DBLUE_BALANCE, sp3C.x, sp3C.y, sp3C.z,
                                                 0, this->dyna.actor.shape.rot.y, 0, 0x100);
    if (this->unk_15C == NULL) {
        sp58 = false;
    } else {
        balance = this->unk_15C;
        balance->unk_164 = this;
    }

    sp48.x = 138.28f;
    Matrix_MultVec3f(&sp48, &sp3C);

    this->unk_160 = (BgDblueBalance*)Actor_Spawn(&play->actorCtx, play, ACTOR_BG_DBLUE_BALANCE, sp3C.x, sp3C.y, sp3C.z,
                                                 0, this->dyna.actor.shape.rot.y, 0, 0x200);
    if (this->unk_160 == NULL) {
        sp58 = false;
    } else {
        balance = this->unk_160;
        balance->unk_164 = this;
    }

    return sp58;
}

void func_80B8259C(BgDblueBalance* this, PlayState* play) {
    s32 pad;
    WaterBox* waterBox;
    f32 sp2C;

    if (WaterBox_GetSurface1_2(play, &play->colCtx, this->dyna.actor.world.pos.x, this->dyna.actor.world.pos.z, &sp2C,
                               &waterBox)) {
        this->dyna.actor.depthInWater = sp2C - this->dyna.actor.world.pos.y;
        this->unk_228 = sp2C;
        if ((this->dyna.actor.depthInWater < 150.0f) && (this->dyna.actor.depthInWater > -150.0f)) {
            this->unk_182 = true;
        } else {
            this->unk_182 = false;
        }
    } else {
        this->unk_182 = false;
    }
}

void func_80B8264C(BgDblueBalance* this) {
    s32 pad;
    s32 i;
    BgDblueBalanceStruct* phi_s0;
    Vec3f sp78;
    f32 temp_f0;
    s16 phi_s2;

    if (this->isSwitchFlagSet) {
        if (this->unk_183 < 8) {
            this->unk_183 = 0;
        } else {
            this->unk_183 -= 7;
        }
    } else {
        if (this->unk_183 > 250) {
            this->unk_183 = 255;
        } else {
            this->unk_183 += 4;
        }
    }

    Matrix_Translate(this->dyna.actor.world.pos.x, this->dyna.actor.world.pos.y, this->dyna.actor.world.pos.z,
                     MTXMODE_NEW);
    Matrix_RotateYS(this->dyna.actor.shape.rot.y, MTXMODE_APPLY);

    for (i = 0, phi_s0 = &this->unk_188[0], phi_s2 = 0; i < ARRAY_COUNT(this->unk_188);
         i++, phi_s2 += 0x2000, phi_s0++) {
        phi_s0->unk_0C = this->dyna.actor.shape.rot.x + phi_s2;

        Matrix_Push();
        Matrix_RotateXS(phi_s0->unk_0C, MTXMODE_NEW);
        Matrix_Translate(-545.0f, 0.0f, 270.0f, MTXMODE_APPLY);
        Matrix_MultZero(&sp78);
        Matrix_Pop();
        Matrix_MultVec3f(&sp78, &phi_s0->unk_00);

        temp_f0 = (sp78.z - 135.0f);
        if (sp78.z > 135.0f) {
            temp_f0 *= 1.8962963f;
            if (temp_f0 > 255.0f) {
                phi_s0->unk_0E = 0xFF;
                phi_s0->unk_10 = 0.1f;
            } else if (temp_f0 < 1.0f) {
                phi_s0->unk_0E = 0;
                phi_s0->unk_10 = 0.0f;
            } else {
                phi_s0->unk_0E = temp_f0;
                phi_s0->unk_10 = temp_f0 * 0.000390625f;
            }
        } else {
            phi_s0->unk_0E = 0;
        }
    }
}

void func_80B828E4(BgDblueBalance* this, PlayState* play) {
    if (Flags_GetSwitch(play, BGDBLUEBALANCE_GET_SWITCH_FLAG(&this->dyna.actor)) && (this->unk_17D <= 0)) {
        if (this->unk_183 >= 252) {
            this->unk_183 = 255;
        } else {
            this->unk_183 += 3;
        }
    } else if (this->unk_183 < 7) {
        this->unk_183 = 0;
    } else {
        this->unk_183 -= 6;
    }
}

void func_80B8296C(PlayState* play, Vec3f* arg1, f32 arg2) {
    s32 i;
    Vec3f sp60;
    f32 temp_f0;
    f32 temp_f20 = arg2 * 2;

    for (i = 0; i < ARRAY_COUNT(D_80B83A90); i++) {
        sp60.x = ((Rand_ZeroOne() - 0.5f) * temp_f20) + arg1->x;
        sp60.y = arg1->y;
        sp60.z = ((Rand_ZeroOne() - 0.5f) * temp_f20) + arg1->z;
        EffectSsGRipple_Spawn(play, &sp60, 500, 2000, D_80B83A90[i]);
    }

    for (i = 0; i < ARRAY_COUNT(D_80B83A94); i++) {
        temp_f0 = Rand_ZeroOne();
        temp_f20 = 1.0f - SQ(temp_f0);
        if ((s32)Rand_Next() > 0) {
            temp_f20 = -temp_f20;
        }

        sp60.x = (temp_f20 * arg2) + arg1->x;
        sp60.y = arg1->y;

        temp_f0 = Rand_ZeroOne();
        temp_f20 = 1.0f - SQ(temp_f0);
        if ((s32)Rand_Next() > 0) {
            temp_f20 = -temp_f20;
        }
        sp60.z = (temp_f20 * arg2) + arg1->z;

        EffectSsGRipple_Spawn(play, &sp60, 400, 800, D_80B83A94[i]);
    }
}

bool func_80B82B00(s16 arg0, s16 arg1, s16 arg2) {
    if (arg0 < arg1) {
        return (arg0 < arg2 && arg1 >= arg2);
    }
    return (arg0 < arg2 || arg1 >= arg2);
}

void BgDblueBalance_Init(Actor* thisx, PlayState* play) {
    s32 pad;
    BgDblueBalance* this = (BgDblueBalance*)thisx;
    s32 sp2C = BGDBLUEBALANCE_GET_300(&this->dyna.actor);
    s32 pad2;
    s32 isSwitchFlagSet = Flags_GetSwitch(play, BGDBLUEBALANCE_GET_SWITCH_FLAG(&this->dyna.actor));

    Actor_ProcessInitChain(&this->dyna.actor, sInitChain);

    this->dyna.actor.flags = sTypeInfo[sp2C].unk_08;
    this->dyna.actor.cullingVolumeScale = sTypeInfo[sp2C].unk_0C;
    this->dyna.actor.cullingVolumeDownward = sTypeInfo[sp2C].unk_10;
    this->dyna.actor.update = sTypeInfo[sp2C].update;
    this->dyna.actor.draw = sTypeInfo[sp2C].draw;

    DynaPolyActor_Init(&this->dyna, DYNA_TRANSFORM_POS);
    DynaPolyActor_LoadMesh(play, &this->dyna, sTypeInfo[sp2C].colHeader);

    if (sp2C == 3) {
        D_80B83C70 = Lib_SegmentedToVirtual(gGreatBayTempleObjectWaterwheelSplashTexAnim);
    } else if (sp2C == 0) {
        D_80B83C74 = Lib_SegmentedToVirtual(gGreatBayTempleObjectSeesawSplashTexAnim);
    }

    if (sp2C == 0) {
        if (isSwitchFlagSet) {
            this->dyna.actor.shape.rot.z = -0x1C72;
            this->unk_174 = -0x1C72;
        } else {
            this->dyna.actor.shape.rot.z = 0x1C72;
            this->unk_174 = 0x1C72;
        }
        func_80B82454(this, play);
        func_80B8259C(this, play);
        func_80B82DC4(this);
    } else if (sp2C == 3) {
        this->isSwitchFlagSet2 = isSwitchFlagSet;
        this->isSwitchFlagSet = isSwitchFlagSet;
        if (isSwitchFlagSet) {
            func_80B83344(this);
        } else {
            this->unk_178 = 0x1F4;
            func_80B833A8(this);
        }
    }
}

void BgDblueBalance_Destroy(Actor* thisx, PlayState* play) {
    s32 pad;
    BgDblueBalance* this = (BgDblueBalance*)thisx;
    s32 sp1C = BGDBLUEBALANCE_GET_300(&this->dyna.actor);

    DynaPoly_DeleteBgActor(play, &play->colCtx.dyna, this->dyna.bgId);

    if (sp1C == 0) {
        if (this->unk_15C != NULL) {
            this->unk_15C->unk_164 = NULL;
        }
        if (this->unk_160 != NULL) {
            this->unk_160->unk_164 = NULL;
        }
    } else if (sp1C == 1) {
        if (this->unk_164 != NULL) {
            this->unk_164->unk_15C = NULL;
        }
    } else if (sp1C == 2) {
        if (this->unk_164 != NULL) {
            this->unk_164->unk_160 = NULL;
        }
    }
}

void func_80B82DC4(BgDblueBalance* this) {
    this->unk_17D = 20;
    this->actionFunc = func_80B82DE0;
}

void func_80B82DE0(BgDblueBalance* this, PlayState* play) {
    s32 pad;
    f32 sp88 = Math_CosS(this->dyna.actor.shape.rot.z * 2.2499313f);
    Actor* actor = &this->dyna.actor;
    s32 pad2;
    s16 abs;
    s32 phi_a0;
    s32 phi_a1;
    f32 phi_f2;
    f32 temp2;
    Vec3f sp60;
    BgDblueBalance* sp5C;
    Vec3f sp50;
    s32 pad3;
    Vec3f sp40;
    BgDblueBalance* sp3C;
    Vec3f sp30;

    if (Flags_GetSwitch(play, BGDBLUEBALANCE_GET_SWITCH_FLAG(&this->dyna.actor))) {
        if (this->unk_17D > 0) {
            this->unk_17D--;
        } else {
            this->unk_174 = -0x1C72;
            this->unk_176 = (s32)(sp88 * -300.0f) - 60;
        }
    } else {
        BgDblueBalance* balance1 = this->unk_160;
        BgDblueBalance* balance2 = this->unk_15C;

        phi_a0 = 0;
        phi_a1 = 1;
        this->unk_17D = 20;
        if (balance1 != NULL) {
            if (balance1->isHeavySwitchPressed) {
                phi_a0 = 4;
            } else if (balance1->isSwitchPressed) {
                phi_a0 = 2;
            }
        }

        if (balance2 != NULL) {
            if (balance2->isHeavySwitchPressed) {
                phi_a1 = 5;
            } else if (balance2->isSwitchPressed) {
                phi_a1 = 3;
            }
        }

        if (phi_a0 < phi_a1) {
            this->unk_174 = 0x1C72;
            this->unk_176 = (s32)(sp88 * 370.0f) + 80;
        } else {
            this->unk_174 = -0x1C72;
            this->unk_176 = (s32)(sp88 * -160.0f) - 80;
        }
    }

    if (this->dyna.actor.shape.rot.z == this->unk_174) {
        this->unk_178 = 0;
        this->unk_17C = 0;
        return;
    }

    abs = this->dyna.actor.shape.rot.z - this->unk_174;
    abs = ABS_ALT(abs);

    if ((this->unk_17C > 0) && (abs < 0x7D0)) {
        phi_a0 = 3;
    } else {
        phi_a0 = 40;
        this->unk_17C = 0;
    }

    if (this->unk_176 >= 0) {
        this->unk_178 += phi_a0;
        if (this->unk_176 < this->unk_178) {
            this->unk_178 = this->unk_176;
        }
    } else {
        this->unk_178 -= phi_a0;
        if (this->unk_178 < this->unk_176) {
            this->unk_178 = this->unk_176;
        }
    }

    phi_f2 = this->unk_178;
    temp2 = this->unk_17A;
    if ((phi_f2 * temp2) <= 0.0f) {
        AudioSfx_StopByPosAndId(&this->dyna.actor.projectedPos, NA_SE_EV_SEESAW_INCLINE - SFX_FLAG);
    }

    phi_f2 *= 0.0022222223f;
    phi_f2 = fabsf(phi_f2);
    if (phi_f2 > 1.0f) {
        phi_f2 = 1.0f;
    }

    Audio_PlaySfx_AtPosWithFreq(&this->dyna.actor.projectedPos, NA_SE_EV_SEESAW_INCLINE - SFX_FLAG, phi_f2 + 1.0f);
    actor->shape.rot.z += this->unk_178;

    if (this->dyna.actor.shape.rot.z > 0x1C71) {
        if (this->unk_17C < 2) {
            actor->shape.rot.z = 0x1C71;
            this->unk_178 *= -0.6f;
            this->unk_17C++;
            if (this->unk_182) {
                Actor_PlaySfx(&this->dyna.actor, NA_SE_EV_SEESAW_WATER_BOUND);
                sp5C = this->unk_15C;
                Matrix_Push();
                Matrix_RotateYS(BINANG_SUB(this->dyna.actor.shape.rot.y, 0x4000), MTXMODE_NEW);
                Matrix_MultVecZ(45.0f, &sp60);
                Matrix_Pop();

                sp50.x = sp5C->dyna.actor.world.pos.x + sp60.x;
                sp50.y = this->unk_228;
                sp50.z = sp5C->dyna.actor.world.pos.z + sp60.z;

                func_80B8296C(play, &sp50, 70.0f);
            } else {
                Actor_PlaySfx(&this->dyna.actor, NA_SE_EV_SEESAW_BOUND);
            }
        } else {
            this->dyna.actor.shape.rot.z = 0x1C72;
            this->unk_178 = 0;
        }
    } else if (this->dyna.actor.shape.rot.z < -0x1C71) {
        if (this->unk_17C < 2) {
            actor->shape.rot.z = -0x1C71;
            this->unk_178 *= -0.6f;
            this->unk_17C++;
            if (this->unk_182) {
                Actor_PlaySfx(&this->dyna.actor, NA_SE_EV_SEESAW_WATER_BOUND);
                sp3C = this->unk_160;
                Matrix_Push();
                Matrix_RotateYS(BINANG_ADD(this->dyna.actor.shape.rot.y, 0x4000), MTXMODE_NEW);
                Matrix_MultVecZ(30.0f, &sp40);
                Matrix_Pop();

                sp30.x = sp3C->dyna.actor.world.pos.x + sp40.x;
                sp30.y = this->unk_228;
                sp30.z = sp3C->dyna.actor.world.pos.z + sp40.z;

                func_80B8296C(play, &sp30, 47.0f);
            } else {
                Actor_PlaySfx(&this->dyna.actor, NA_SE_EV_SEESAW_BOUND);
            }
        } else {
            this->dyna.actor.shape.rot.z = -0x1C72;
            this->unk_178 = 0;
        }
    }
    func_80B823B0(this);
}

void BgDblueBalance_Update(Actor* thisx, PlayState* play) {
    BgDblueBalance* this = (BgDblueBalance*)thisx;

    this->unk_17A = this->unk_178;

    this->actionFunc(this, play);

    func_80B828E4(this, play);
}

void func_80B8330C(Actor* thisx, PlayState* play) {
    BgDblueBalance* this = (BgDblueBalance*)thisx;

    this->isSwitchPressed = DynaPolyActor_IsSwitchPressed(&this->dyna);
    this->isHeavySwitchPressed = DynaPolyActor_IsHeavySwitchPressed(&this->dyna);
}

void func_80B83344(BgDblueBalance* this) {
    this->unk_178 = 0;
    this->actionFunc = func_80B8335C;
}

void func_80B8335C(BgDblueBalance* this, PlayState* play) {
    if (!Flags_GetSwitch(play, BGDBLUEBALANCE_GET_SWITCH_FLAG(&this->dyna.actor))) {
        this->unk_178 = -0x14;
        func_80B833A8(this);
    }
}

void func_80B833A8(BgDblueBalance* this) {
    this->unk_186 = 0;
    this->unk_187 = 0;
    this->actionFunc = func_80B833C4;
}

void func_80B833C4(BgDblueBalance* this, PlayState* play) {
    Actor* thisx = &this->dyna.actor;
    s32 sp28 = false;
    s16 sp26;
    s16 sp24;

    if (this->isSwitchFlagSet) {
        if (!this->isSwitchFlagSet2 && (this->unk_17F == 0)) {
            this->unk_17F = 1;
        }

        if (this->unk_186 != 0) {
            if (this->unk_178 >= 0x50) {
                this->unk_178 -= 5;
            }
        }
    } else {
        this->unk_187 = Math_StepToS(&this->unk_178, 0x1F4, 5);
    }

    sp26 = thisx->shape.rot.x;
    thisx->shape.rot.x += this->unk_178;
    sp24 = thisx->shape.rot.x;

    if (this->isSwitchFlagSet) {
        if (this->unk_186 != 0) {
            if (func_80B82B00(sp26, sp24, this->unk_184)) {
                sp28 = true;
            }
        } else if (this->unk_187 != 0) {
            if (func_80B82B00(sp26, sp24, 0x2097)) {
                this->unk_184 = -0x8000;
                this->unk_186 = 1;
            } else if (func_80B82B00(sp26, sp24, -0x5F69)) {
                this->unk_184 = 0;
                this->unk_186 = 1;
            }
        }
    }

    func_80B8264C(this);

    if (sp28) {
        thisx->shape.rot.x = this->unk_184;
        func_80B83344(this);
    }
}

void func_80B83518(Actor* thisx, PlayState* play) {
    BgDblueBalance* this = (BgDblueBalance*)thisx;

    this->isSwitchFlagSet = Flags_GetSwitch(play, BGDBLUEBALANCE_GET_SWITCH_FLAG(&this->dyna.actor));

    this->actionFunc(this, play);

    if (this->unk_17F == 2) {
        this->unk_17E--;
        if (this->unk_17E <= 0) {
            CutsceneManager_Stop(this->dyna.actor.csId);
            this->unk_17F = 0;
        }
    } else if ((this->unk_17F != 0) && (this->unk_17F == 1)) {
        if (CutsceneManager_IsNext(this->dyna.actor.csId)) {
            CutsceneManager_StartWithPlayerCs(this->dyna.actor.csId, &this->dyna.actor);
            this->unk_17F = 2;
            this->unk_17E = 0x50;
        } else {
            CutsceneManager_Queue(this->dyna.actor.csId);
        }
    }
    this->isSwitchFlagSet2 = this->isSwitchFlagSet;
}

void BgDblueBalance_Draw(Actor* thisx, PlayState* play) {
    s32 pad;
    BgDblueBalance* this = (BgDblueBalance*)thisx;
    BgDblueBalanceTypeInfo* ptr2 = &sTypeInfo[BGDBLUEBALANCE_GET_300(&this->dyna.actor)];
    BgDblueBalance* sp38;
    Gfx* gfx;

    Gfx_DrawDListOpa(play, ptr2->opaDList);

    if (!(BGDBLUEBALANCE_GET_300(&this->dyna.actor)) && (this->unk_160 != NULL)) {
        AnimatedMat_Draw(play, D_80B83C74);
        sp38 = this->unk_160;
        Matrix_SetTranslateRotateYXZ(sp38->dyna.actor.world.pos.x, sp38->dyna.actor.world.pos.y,
                                     sp38->dyna.actor.world.pos.z, &sp38->dyna.actor.shape.rot);
        Matrix_Translate(30.0f, 15.0f, 0.0f, MTXMODE_APPLY);
        Matrix_Scale(sp38->dyna.actor.scale.x, sp38->dyna.actor.scale.y, sp38->dyna.actor.scale.z, MTXMODE_APPLY);

        OPEN_DISPS(play->state.gfxCtx);

        gfx = POLY_XLU_DISP;

        gSPDisplayList(gfx++, gSetupDLs[SETUPDL_25]);
        MATRIX_FINALIZE_AND_LOAD(gfx++, play->state.gfxCtx);
        gDPSetEnvColor(gfx++, 0, 0, 0, this->unk_183);
        gSPDisplayList(gfx++, gGreatBayTempleObjectSeesawSplashDL);

        POLY_XLU_DISP = gfx;

        CLOSE_DISPS(play->state.gfxCtx);
    }
}

void func_80B83758(Actor* thisx, PlayState* play) {
    s32 pad;
    BgDblueBalance* this = (BgDblueBalance*)thisx;
    f32 temp_f0;
    Gfx* gfx;
    s32 i;
    BgDblueBalanceStruct* ptr;
    BgDblueBalanceTypeInfo* ptr2;
    s32 temp;

    if (this->unk_178 != 0) {
        if (this->unk_178 == 0x1F4) {
            temp_f0 = 1.0f;
        } else {
            temp_f0 = this->unk_178 * 0.002f;
            temp_f0 = CLAMP(temp_f0, 0.0f, 1.0f);
        }
        Audio_PlaySfx_AtPosWithFreqAndChannelIO(&this->dyna.actor.projectedPos, NA_SE_EV_SMALL_WATER_WHEEL - SFX_FLAG,
                                                temp_f0, 0x20);
    }

    if (this->dyna.actor.flags & ACTOR_FLAG_INSIDE_CULLING_VOLUME) {
        ptr2 = &sTypeInfo[BGDBLUEBALANCE_GET_300(&this->dyna.actor)];
        Gfx_DrawDListOpa(play, ptr2->opaDList);

        if (this->unk_183 != 0) {
            AnimatedMat_Draw(play, D_80B83C70);

            OPEN_DISPS(play->state.gfxCtx);

            gfx = POLY_XLU_DISP;

            gSPDisplayList(gfx++, gSetupDLs[SETUPDL_25]);

            for (i = 0, ptr = &this->unk_188[0]; i < ARRAY_COUNT(this->unk_188); i++, ptr++) {
                if (ptr->unk_0E != 0) {
                    Matrix_Translate(ptr->unk_00.x, ptr->unk_00.y, ptr->unk_00.z, MTXMODE_NEW);
                    Matrix_RotateYS(this->dyna.actor.shape.rot.y, MTXMODE_APPLY);
                    Matrix_RotateXS(ptr->unk_0C, MTXMODE_APPLY);
                    Matrix_Scale(ptr->unk_10, ptr->unk_10, ptr->unk_10, MTXMODE_APPLY);

                    temp = ptr->unk_0E * (f32)this->unk_183 * 0.003921569f;
                    gDPSetEnvColor(gfx++, 0, 0, 0, temp);
                    MATRIX_FINALIZE_AND_LOAD(gfx++, play->state.gfxCtx);
                    gSPDisplayList(gfx++, gGreatBayTempleObjectWaterwheelSplashDL);
                }
            }

            POLY_XLU_DISP = gfx;

            CLOSE_DISPS(play->state.gfxCtx);
        }
    }
}
