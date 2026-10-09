/*
 * File: z_bg_spdweb.c
 * Overlay: ovl_Bg_Spdweb
 * Description: Spiderweb
 */

#include "z_bg_spdweb.h"
#include "objects/object_spdweb/object_spdweb.h"

#define FLAGS 0x00000000

void BgSpdweb_Init(Actor* thisx, PlayState* play);
void BgSpdweb_Destroy(Actor* thisx, PlayState* play);
void BgSpdweb_Update(Actor* thisx, PlayState* play);
void BgSpdweb_Draw(Actor* thisx, PlayState* play);

void func_809CE068(BgSpdweb* this);
void func_809CE234(BgSpdweb* this, PlayState* play);
void func_809CE4C8(BgSpdweb* this, PlayState* play);
void func_809CE830(BgSpdweb* this, PlayState* play);
void func_809CEBC0(BgSpdweb* this, PlayState* play);
void func_809CEE74(BgSpdweb* this);
void func_809CEEAC(BgSpdweb* this, PlayState* play);

ActorProfile Bg_Spdweb_Profile = {
    /**/ ACTOR_BG_SPDWEB,
    /**/ ACTORCAT_BG,
    /**/ FLAGS,
    /**/ OBJECT_SPDWEB,
    /**/ sizeof(BgSpdweb),
    /**/ BgSpdweb_Init,
    /**/ BgSpdweb_Destroy,
    /**/ BgSpdweb_Update,
    /**/ BgSpdweb_Draw,
};

static ColliderTrisElementInit sTrisElementsInit1[2] = {
    {
        {
            ELEM_MATERIAL_UNK0,
            { 0xF7CFFFFF, 0x00, 0x00 },
            { 0x00000C00, 0x00, 0x00 },
            ATELEM_NONE | ATELEM_SFX_NORMAL,
            ACELEM_ON,
            OCELEM_NONE,
        },
        { { { 75.0f, -8.0f, 75.0f }, { 75.0f, -8.0f, -75.0f }, { -75.0f, -8.0f, -75.0f } } },
    },
    {
        {
            ELEM_MATERIAL_UNK0,
            { 0xF7CFFFFF, 0x00, 0x00 },
            { 0x00000C00, 0x00, 0x00 },
            ATELEM_NONE | ATELEM_SFX_NORMAL,
            ACELEM_ON,
            OCELEM_NONE,
        },
        { { { 75.0f, -8.0f, 75.0f }, { -75.0f, -8.0f, -75.0f }, { -75.0f, -8.0f, 75.0f } } },
    },
};

static ColliderTrisInit sTrisInit1 = {
    {
        COL_MATERIAL_NONE,
        AT_NONE,
        AC_ON | AC_TYPE_PLAYER,
        OC1_NONE,
        OC2_TYPE_2,
        COLSHAPE_TRIS,
    },
    ARRAY_COUNT(sTrisElementsInit1),
    sTrisElementsInit1,
};

static ColliderTrisElementInit sTrisElementsInit2[4] = {
    {
        {
            ELEM_MATERIAL_UNK0,
            { 0xF7CFFFFF, 0x00, 0x00 },
            { 0x00000800, 0x00, 0x00 },
            ATELEM_NONE | ATELEM_SFX_NORMAL,
            ACELEM_ON,
            OCELEM_NONE,
        },
        { { { 70.0f, 160.0f, 15.0f }, { -70.0f, 160.0f, 15.0f }, { -70.0f, 20.0f, 15.0f } } },
    },
    {
        {
            ELEM_MATERIAL_UNK0,
            { 0xF7CFFFFF, 0x00, 0x00 },
            { 0x00000800, 0x00, 0x00 },
            ATELEM_NONE | ATELEM_SFX_NORMAL,
            ACELEM_ON,
            OCELEM_NONE,
        },
        { { { 70.0f, 160.0f, 15.0f }, { -70.0f, 20.0f, 15.0f }, { 70.0f, 20.0f, 15.0f } } },
    },
    {
        {
            ELEM_MATERIAL_UNK0,
            { 0xF7CFFFFF, 0x00, 0x00 },
            { 0x00000800, 0x00, 0x00 },
            ATELEM_NONE | ATELEM_SFX_NORMAL,
            ACELEM_ON,
            OCELEM_NONE,
        },
        { { { -70.0f, 160.0f, -15.0f }, { 70.0f, 160.0f, -15.0f }, { 70.0f, 20.0f, -15.0f } } },
    },
    {
        {
            ELEM_MATERIAL_UNK0,
            { 0xF7CFFFFF, 0x00, 0x00 },
            { 0x00000800, 0x00, 0x00 },
            ATELEM_NONE | ATELEM_SFX_NORMAL,
            ACELEM_ON,
            OCELEM_NONE,
        },
        { { { -70.0f, 160.0f, -15.0f }, { 70.0f, 20.0f, -15.0f }, { -70.0f, 20.0f, -15.0f } } },
    },
};

static ColliderTrisInit sTrisInit2 = {
    {
        COL_MATERIAL_NONE,
        AT_NONE,
        AC_ON | AC_TYPE_PLAYER,
        OC1_NONE,
        OC2_TYPE_2,
        COLSHAPE_TRIS,
    },
    ARRAY_COUNT(sTrisElementsInit2),
    sTrisElementsInit2,
};

Color_RGBA8 D_809CF208 = { 255, 255, 150, 170 };

Color_RGBA8 D_809CF20C = { 255, 0, 0, 0 };

static InitChainEntry sInitChain[] = {
    ICHAIN_F32(cullingVolumeDistance, 1500, ICHAIN_CONTINUE),
    ICHAIN_VEC3F_DIV1000(scale, 100, ICHAIN_STOP),
};

void BgSpdweb_Init(Actor* thisx, PlayState* play) {
    BgSpdweb* this = (BgSpdweb*)thisx;

    Actor_ProcessInitChain(&this->dyna.actor, sInitChain);
    this->unk_161 = 0;
    this->switchFlag = BGSPDWEB_GET_SWITCH_FLAG(&this->dyna.actor);
    thisx->params &= 0xFF;
    DynaPolyActor_Init(&this->dyna, DYNA_TRANSFORM_POS);

    if (this->dyna.actor.params == BGSPDWEB_FF_0) {
        Collider_InitAndSetTris(play, &this->collider, &this->dyna.actor, &sTrisInit1, this->colliderElements);
        func_809CE068(this);
        DynaPolyActor_LoadMesh(play, &this->dyna, &object_spdweb_Colheader_002678);
        this->unk_2F8 = play->colCtx.dyna.bgActors[this->dyna.bgId].colHeader->vtxList;
        this->unk_164 = 0.0f;
        this->actionFunc = func_809CE4C8;
    } else {
        Collider_InitAndSetTris(play, &this->collider, &this->dyna.actor, &sTrisInit2, this->colliderElements);
        func_809CE068(this);
        DynaPolyActor_LoadMesh(play, &this->dyna, &object_spdweb_Colheader_0011C0);
        this->actionFunc = func_809CEBC0;
        Actor_SetFocus(&this->dyna.actor, 30.0f);
    }

    this->unk_162 = 0;

    if (Flags_GetSwitch(play, this->switchFlag)) {
        Actor_Kill(&this->dyna.actor);
    }
}

void BgSpdweb_Destroy(Actor* thisx, PlayState* play) {
    BgSpdweb* this = (BgSpdweb*)thisx;

    DynaPoly_DeleteBgActor(play, &play->colCtx.dyna, this->dyna.bgId);
    Collider_DestroyTris(play, &this->collider);
}

void func_809CE068(BgSpdweb* this) {
    Vec3f sp64[3];
    s32 i;
    s32 j;

    Matrix_SetTranslateRotateYXZ(this->dyna.actor.world.pos.x, this->dyna.actor.world.pos.y,
                                 this->dyna.actor.world.pos.z, &this->dyna.actor.shape.rot);

    for (i = 0; i < this->collider.count; i++) {
        for (j = 0; j < ARRAY_COUNT(sp64); j++) {
            Matrix_MultVec3f(this->collider.elements[i].dim.vtx + j, &sp64[j]);
        }
        Collider_SetTrisVertices(&this->collider, i, &sp64[0], &sp64[1], &sp64[2]);
    }
}

void func_809CE15C(BgSpdweb* this) {
    this->unk_2F8[0].y = this->unk_2F8[3].y = this->unk_2F8[10].y = this->unk_2F8[11].y = this->unk_2F8[12].y =
        this->unk_2F8[13].y = this->unk_2F8[14].y = this->unk_2F8[15].y =
            (this->dyna.actor.home.pos.y - this->dyna.actor.world.pos.y) * 10.0f;
}

void func_809CE1D0(BgSpdweb* this, PlayState* play) {
    this->unk_162 = 30;
    Flags_SetSwitch(play, this->switchFlag);

    if (this->dyna.actor.params == BGSPDWEB_FF_0) {
        this->actionFunc = func_809CE234;
    } else {
        this->actionFunc = func_809CE830;
    }
}

void func_809CE234(BgSpdweb* this, PlayState* play) {
    Vec3f spB4;
    Vec3f spA8;
    f32 temp_f20;
    f32 temp_f22;
    f32 temp_f24;
    s16 phi_s2;
    s16 temp_s0;
    s32 i;

    if (this->unk_162 != 0) {
        this->unk_162--;
    }

    if (this->unk_162 == 0) {
        CutsceneManager_Stop(this->dyna.actor.csId);
        Actor_Kill(&this->dyna.actor);
        return;
    }

    if ((this->unk_162 % 3) == 0) {
        phi_s2 = Rand_ZeroOne() * 10922.0f;
        spB4.y = 0.0f;
        spA8.y = this->dyna.actor.world.pos.y;

        for (i = 0; i < 6; i++) {
            temp_s0 = (s32)Rand_CenteredFloat(0x2800) + phi_s2;
            temp_f24 = Math_SinS(temp_s0);
            temp_f22 = Math_CosS(temp_s0);

            spA8.x = this->dyna.actor.world.pos.x + (120.0f * temp_f24);
            spA8.z = this->dyna.actor.world.pos.z + (120.0f * temp_f22);

            temp_f20 = Math_Vec3f_DistXZ(&this->dyna.actor.home.pos, &spA8) * (1.0f / 120.0f);
            if (temp_f20 < 0.7f) {
                temp_f20 = 1.0f - temp_f20;
                temp_f24 = Math_SinS(BINANG_ROT180(temp_s0));
                temp_f22 = Math_CosS(BINANG_ROT180(temp_s0));
            }

            spB4.x = 7.0f * temp_f24 * temp_f20;
            spB4.y = 0.0f;
            spB4.z = 7.0f * temp_f22 * temp_f20;

            EffectSsDeadDb_Spawn(play, &this->dyna.actor.home.pos, &spB4, &gZeroVec3f, &D_809CF208, &D_809CF20C, 0x32,
                                 8, 0xE);
            phi_s2 += 0x2AAA;
        }

        SoundSource_PlaySfxAtFixedWorldPos(play, &this->dyna.actor.home.pos, 11, NA_SE_EN_EXTINCT);
    }
}

void func_809CE4C8(BgSpdweb* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    Vec3f sp40;
    ColliderTrisElement* trisElem;
    s16 sp3A;
    s32 i;
    f32 temp_f12;

    sp40.x = this->dyna.actor.world.pos.x;
    sp40.y = this->dyna.actor.world.pos.y - 50.0f;
    sp40.z = this->dyna.actor.world.pos.z;
    sp3A = player->fallDistance;

    if (Player_IsBurningStickInRange(play, &sp40, 70.0f, 50.0f)) {
        this->dyna.actor.home.pos.x = player->meleeWeaponInfo[0].tip.x;
        this->dyna.actor.home.pos.z = player->meleeWeaponInfo[0].tip.z;
        func_809CEE74(this);
        return;
    }

    if (this->collider.base.acFlags & AC_HIT) {
        for (i = 0; i < 2; i++) {
            trisElem = &this->collider.elements[i];
            if (trisElem->base.acElemFlags & ACELEM_HIT) {
                if (this->collider.elements[i].base.acHitElem->atDmgInfo.dmgFlags & 0x800) {
                    Math_Vec3s_ToVec3f(&this->dyna.actor.home.pos, &trisElem->base.acDmgInfo.hitPos);
                    func_809CEE74(this);
                    return;
                }

                if (DynaPolyActor_IsPlayerOnTop(&this->dyna)) {
                    sp3A = 300;
                    break;
                }
            }
        }
    }

    if (DynaPolyActor_IsPlayerOnTop(&this->dyna)) {
        temp_f12 = 2.0f * sqrtf(CLAMP_MIN(sp3A, 0));
        if ((this->unk_164 < temp_f12) && (temp_f12 > 2.0f)) {
            this->unk_164 = temp_f12;
            this->unk_162 = 12;
            if (sp3A > 50) {
                player->stateFlags1 |= PLAYER_STATE1_20;
                this->unk_161 = 1;
            }
        } else if (player->actor.speed != 0.0f) {
            this->unk_164 = CLAMP_MIN(this->unk_164, 2.0f);
        }
    }

    if (this->unk_162 != 0) {
        this->unk_162--;
    }

    this->dyna.actor.world.pos.y =
        (Math_SinF(this->unk_162 * (M_PIf / 6)) * this->unk_164) + this->dyna.actor.home.pos.y;
    Math_ApproachZeroF(&this->unk_164, 1.0f, 0.8f);

    if (this->unk_162 == 4) {
        if ((this->unk_161 != 0) || (DynaPolyActor_IsPlayerOnTop(&this->dyna) && (this->unk_164 > 2.0f))) {
            player->actor.velocity.y = this->unk_164 * 0.7f;
            player->fallStartHeight = (SQ(this->unk_164) * 0.15f) + this->dyna.actor.world.pos.y;
            this->unk_161 = 0;
            player->stateFlags1 &= ~PLAYER_STATE1_20;
        }
    } else if (this->unk_162 == 11) {
        if (this->unk_164 > 3.0f) {
            Actor_PlaySfx(&this->dyna.actor, NA_SE_EV_WEB_VIBRATION);
        } else {
            AudioSfx_StopById(NA_SE_EV_WEB_VIBRATION);
        }
    }

    if (this->unk_162 == 0) {
        this->unk_162 = 12;
    }

    func_809CE15C(this);
    CollisionCheck_SetAC(play, &play->colChkCtx, &this->collider.base);
}

void func_809CE830(BgSpdweb* this, PlayState* play) {
    Vec3f spDC;
    Vec3f spD0;
    Vec3f spC4;
    s32 i;
    s16 temp_s3;
    s16 temp_s0;
    f32 cosF2;
    f32 sinF2;
    f32 cosQ;
    f32 sinQ;
    f32 cosF1;
    f32 sinF1;
    f32 temp_f28;

    if (this->unk_162 != 0) {
        this->unk_162--;
    }

    if (this->unk_162 == 0) {
        if (CutsceneManager_GetLength(this->dyna.actor.csId) == -1) {
            CutsceneManager_Stop(this->dyna.actor.csId);
        }
        Actor_Kill(&this->dyna.actor);
        return;
    }

    if ((this->unk_162 % 3) == 0) {
        temp_s3 = Rand_ZeroOne() * 0x2AAA;
        cosQ = Math_CosS(this->dyna.actor.shape.rot.x);
        sinQ = Math_SinS(this->dyna.actor.shape.rot.x);
        cosF1 = Math_CosS(this->dyna.actor.shape.rot.y);
        sinF1 = Math_SinS(this->dyna.actor.shape.rot.y);

        spC4.x = this->dyna.actor.world.pos.x + 90.0f * sinQ * sinF1;
        spC4.y = this->dyna.actor.world.pos.y + 90.0f * cosQ;
        spC4.z = this->dyna.actor.world.pos.z + 90.0f * sinQ * cosF1;

        for (i = 0; i < 6; i++) {
            temp_s0 = (s32)Rand_CenteredFloat(0x2800) + temp_s3;
            sinF2 = Math_SinS(temp_s0);
            cosF2 = Math_CosS(temp_s0);

            spD0.x = spC4.x + 90.0f * ((cosF1 * sinF2) + (sinQ * sinF1 * cosF2));
            spD0.y = spC4.y + 90.0f * cosQ * cosF2;
            spD0.z = spC4.z + 90.0f * ((sinQ * cosF1 * cosF2) - (sinF1 * sinF2));

            temp_f28 = Math_Vec3f_DistXYZ(&this->dyna.actor.home.pos, &spD0) * (1.0f / 90.0f);
            if (temp_f28 < 0.65f) {
                temp_f28 = 1.0f - temp_f28;
                sinF2 = Math_SinS(BINANG_ROT180(temp_s0));
                cosF2 = Math_CosS(BINANG_ROT180(temp_s0));
            }

            spDC.x = 6.5f * temp_f28 * ((cosF1 * sinF2) + (sinQ * sinF1 * cosF2));
            spDC.y = 6.5f * temp_f28 * cosQ * cosF2;
            spDC.z = 6.5f * temp_f28 * ((sinQ * cosF1 * cosF2) - (sinF1 * sinF2));

            EffectSsDeadDb_Spawn(play, &this->dyna.actor.home.pos, &spDC, &gZeroVec3f, &D_809CF208, &D_809CF20C, 0x3C,
                                 8, 0xE);
            temp_s3 += 0x2AAA;
        }

        SoundSource_PlaySfxAtFixedWorldPos(play, &this->dyna.actor.home.pos, 11, NA_SE_EN_EXTINCT);
    }
}

void func_809CEBC0(BgSpdweb* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    f32 sp58;
    f32 temp_f10;
    f32 temp_f18;
    ColliderTrisElement* trisElem;
    s32 i;
    Vec3f sp3C;
    f32 sp38;
    f32 sp34;
    f32 sp30;
    f32 sp2C;

    if (this->collider.base.acFlags & AC_HIT) {
        for (i = 0; i < ARRAY_COUNT(this->colliderElements); i++) {
            trisElem = &this->collider.elements[i];
            if (trisElem->base.acElemFlags & ACELEM_HIT) {
                Math_Vec3s_ToVec3f(&this->dyna.actor.home.pos, &trisElem->base.acDmgInfo.hitPos);
                break;
            }
        }

        if (i == ARRAY_COUNT(this->colliderElements)) {
            this->dyna.actor.home.pos.x =
                ((90.0f * Math_SinS(this->dyna.actor.shape.rot.x)) * Math_SinS(this->dyna.actor.shape.rot.y)) +
                this->dyna.actor.world.pos.x;
            this->dyna.actor.home.pos.y =
                (90.0f * Math_CosS(this->dyna.actor.shape.rot.x)) + this->dyna.actor.world.pos.y;
            this->dyna.actor.home.pos.z =
                ((90.0f * Math_SinS(this->dyna.actor.shape.rot.x)) * Math_CosS(this->dyna.actor.shape.rot.y)) +
                this->dyna.actor.world.pos.z;
        }
        func_809CEE74(this);
    } else if ((player->heldItemAction == PLAYER_IA_DEKU_STICK) && (player->unk_B28 != 0)) {
        Math_Vec3f_Diff(&player->meleeWeaponInfo[0].tip, &this->dyna.actor.world.pos, &sp3C);
        sp38 = Math_SinS(-this->dyna.actor.shape.rot.x);
        sp34 = Math_CosS(-this->dyna.actor.shape.rot.x);
        sp30 = Math_SinS(-this->dyna.actor.shape.rot.y);
        sp2C = Math_CosS(-this->dyna.actor.shape.rot.y);

        temp_f18 = ((sp3C.x * sp38 * sp30) + (sp3C.y * sp34)) - (sp3C.z * sp38 * sp2C);
        sp58 = (-sp3C.x * sp34 * sp30) + (sp3C.y * sp38) + (sp3C.z * sp34 * sp2C);
        temp_f10 = (sp3C.x * sp2C) + (sp3C.z * sp30);

        if ((fabsf(temp_f10) < 70.0f) && (fabsf(sp58) < 10.0f) && (temp_f18 < 160.0f) && (temp_f18 > 20.0f)) {
            Math_Vec3f_Copy(&this->dyna.actor.home.pos, &player->meleeWeaponInfo[0].tip);
            func_809CEE74(this);
        }
    }

    CollisionCheck_SetAC(play, &play->colChkCtx, &this->collider.base);
}

void func_809CEE74(BgSpdweb* this) {
    CutsceneManager_Queue(this->dyna.actor.csId);
    this->actionFunc = func_809CEEAC;
}

void func_809CEEAC(BgSpdweb* this, PlayState* play) {
    if (CutsceneManager_IsNext(this->dyna.actor.csId)) {
        CutsceneManager_StartWithPlayerCs(this->dyna.actor.csId, &this->dyna.actor);
        func_809CE1D0(this, play);
    } else {
        CutsceneManager_Queue(this->dyna.actor.csId);
    }
}

void BgSpdweb_Update(Actor* thisx, PlayState* play) {
    BgSpdweb* this = (BgSpdweb*)thisx;

    this->actionFunc(this, play);
}

void BgSpdweb_Draw(Actor* thisx, PlayState* play) {
    Gfx* gfx;

    OPEN_DISPS(play->state.gfxCtx);

    gfx = POLY_XLU_DISP;

    gSPDisplayList(&gfx[0], gSetupDLs[SETUPDL_25]);

    if (thisx->params == BGSPDWEB_FF_1) {
        MATRIX_FINALIZE_AND_LOAD(&gfx[1], play->state.gfxCtx);
        gSPDisplayList(&gfx[2], object_spdweb_DL_000060);
    } else {
        Matrix_Translate(0.0f, (thisx->home.pos.y - thisx->world.pos.y) * 10.0f, 0.0f, MTXMODE_APPLY);
        Matrix_Scale(1.0f, ((thisx->home.pos.y - thisx->world.pos.y) + 10.0f) * 0.1f, 1.0f, MTXMODE_APPLY);

        MATRIX_FINALIZE_AND_LOAD(&gfx[1], play->state.gfxCtx);
        gSPDisplayList(&gfx[2], object_spdweb_DL_0012F0);
    }

    POLY_XLU_DISP = &gfx[3];

    CLOSE_DISPS(play->state.gfxCtx);
}
