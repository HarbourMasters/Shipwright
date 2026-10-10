/*
 * File: z_bg_hakugin_bombwall.c
 * Overlay: ovl_Bg_Hakugin_Bombwall
 * Description: Snowhead Temple - Bombable Wall
 */

#include "z_bg_hakugin_bombwall.h"
#include "objects/object_hakugin_obj/object_hakugin_obj.h"

#define FLAGS 0x00000000

void BgHakuginBombwall_Init(Actor* thisx, PlayState* play);
void BgHakuginBombwall_Destroy(Actor* thisx, PlayState* play);
void BgHakuginBombwall_Update(Actor* thisx, PlayState* play);
void BgHakuginBombwall_Draw(Actor* thisx, PlayState* play);

void func_80ABBFC0(BgHakuginBombwall* this, PlayState* play);
void func_80ABC2E0(BgHakuginBombwall* this, PlayState* play);
void func_80ABC58C(BgHakuginBombwall* this, PlayState* play);
void func_80ABC7FC(BgHakuginBombwall* this, PlayState* play);
s32 func_80ABCB5C(BgHakuginBombwall* this, PlayState* play);
s32 func_80ABCC00(BgHakuginBombwall* this, PlayState* play);
void func_80ABCCE4(BgHakuginBombwall* this, PlayState* play);
void func_80ABCD98(BgHakuginBombwall* this, PlayState* play);
void func_80ABCE60(BgHakuginBombwall* this, PlayState* play);

ActorProfile Bg_Hakugin_Bombwall_Profile = {
    /**/ ACTOR_BG_HAKUGIN_BOMBWALL,
    /**/ ACTORCAT_BG,
    /**/ FLAGS,
    /**/ OBJECT_HAKUGIN_OBJ,
    /**/ sizeof(BgHakuginBombwall),
    /**/ BgHakuginBombwall_Init,
    /**/ BgHakuginBombwall_Destroy,
    /**/ BgHakuginBombwall_Update,
    /**/ BgHakuginBombwall_Draw,
};

static ColliderCylinderInit sCylinderInit = {
    {
        COL_MATERIAL_NONE,
        AT_NONE,
        AC_ON | AC_TYPE_PLAYER,
        OC1_NONE,
        OC2_NONE,
        COLSHAPE_CYLINDER,
    },
    {
        ELEM_MATERIAL_UNK0,
        { 0x00000000, 0x00, 0x00 },
        { 0x00000008, 0x00, 0x00 },
        ATELEM_NONE | ATELEM_SFX_NORMAL,
        ACELEM_ON,
        OCELEM_NONE,
    },
    { 80, 80, 0, { 0, 0, 0 } },
};

Color_RGBA8 D_80ABCFAC = { 210, 210, 210, 255 };
Color_RGBA8 D_80ABCFB0 = { 140, 140, 140, 255 };

Vec3f D_80ABCFB4 = { 0.0f, 0.33f, 0.0f };

typedef struct {
    /* 0x00 */ CollisionHeader* unk_00;
    /* 0x04 */ Gfx* unk_04;
    /* 0x08 */ f32 unk_08;
    /* 0x0C */ f32 unk_0C;
    /* 0x10 */ f32 unk_10;
    /* 0x14 */ s16 unk_14;
    /* 0x16 */ s16 unk_16;
    /* 0x18 */ s32 unk_18;
    /* 0x1C */ f32 unk_1C;
    /* 0x20 */ BgHakuginBombwallUnkFunc unk_20;
    /* 0x24 */ BgHakuginBombwallUnkFunc2 unk_24;
    /* 0x28 */ BgHakuginBombwallUnkFunc2 unk_28;
    /* 0x2C */ s32 unk_2C;
} BgHakuginBombwallStruct; // size = 0x30

BgHakuginBombwallStruct D_80ABCFC0[] = {
    {
        &object_hakugin_obj_Colheader_009768,
        object_hakugin_obj_DL_009658,
        70.0f,
        600.0f,
        400.0f,
        80,
        80,
        0x8,
        4225.0f,
        func_80ABCB5C,
        func_80ABBFC0,
        func_80ABC58C,
        6,
    },
    {
        &object_hakugin_obj_Colheader_009AF0,
        object_hakugin_obj_DL_0099A8,
        10.0f,
        600.0f,
        300.0f,
        50,
        20,
        0x408,
        4225.0f,
        func_80ABCC00,
        func_80ABC2E0,
        func_80ABC7FC,
        18,
    },
};

s32 D_80ABD020[] = { -73, -40, -8, 24, 57 };

Vec3f D_80ABD034 = { 0.0f, 3.0f, 0.0f };

static InitChainEntry sInitChain[] = {
    ICHAIN_F32(cullingVolumeDistance, 4000, ICHAIN_CONTINUE),
    ICHAIN_VEC3F_DIV1000(scale, 100, ICHAIN_STOP),
};

void func_80ABBFC0(BgHakuginBombwall* this, PlayState* play) {
    f32 temp;
    Vec3f spF0;
    Vec3f spE4;
    Vec3f spD8;
    Vec3f spCC;
    s32 pad;
    s16 phi_s1;
    s16 phi_s0;
    s32 i;
    s32 j;

    Matrix_RotateYS(this->dyna.actor.shape.rot.y, MTXMODE_NEW);

    for (i = 0; i < 6; i++) {
        temp = (i + 1) * (80.0f / 3.0f);
        for (j = 0; j < ARRAY_COUNT(D_80ABD020); j++) {
            spD8.x = D_80ABD020[j] + (s32)(Rand_Next() >> 0x1C);
            spD8.y = ((Rand_ZeroOne() - 0.5f) * 15.0f) + temp;
            spD8.z = (Rand_ZeroOne() * 20.0f) - 10.0f;

            spCC.x = ((Rand_ZeroOne() - 0.5f) * 7.0f) + (spD8.x * (7.0f / 90.0f));
            spCC.y = (Rand_ZeroOne() * 7.0f) - 2.0f;
            spCC.z = spD8.z * 0.3f;

            Matrix_MultVec3f(&spD8, &spF0);
            Matrix_MultVec3f(&spCC, &spE4);

            spF0.x += this->dyna.actor.world.pos.x;
            spF0.y += this->dyna.actor.world.pos.y;
            spF0.z += this->dyna.actor.world.pos.z;

            if ((Rand_Next() % 4) == 0) {
                phi_s0 = 32;
            } else {
                phi_s0 = 64;
            }

            if ((s32)Rand_Next() > 0) {
                phi_s0 |= 1;
                phi_s1 = 1;
                func_800B0E48(play, &spF0, &gZeroVec3f, &D_80ABCFB4, &D_80ABCFAC, &D_80ABCFB0,
                              (Rand_Next() >> 0x1B) + 70, (Rand_Next() >> 0x1A) + 60);
            } else {
                phi_s1 = 0;
            }

            EffectSsKakera_Spawn(play, &spF0, &spE4, &spF0, -550, phi_s0, 30, 0, 0, (s32)(Rand_ZeroOne() * 22.0f) + 5,
                                 phi_s1, 0, 50, -1, OBJECT_HAKUGIN_OBJ, object_hakugin_obj_DL_009830);
        }
    }
}

void func_80ABC2E0(BgHakuginBombwall* this, PlayState* play) {
    s32 pad;
    s32 i;
    s16 phi_s2;
    s16 phi_v0;
    s16 phi_v1;
    s16 phi_t0;
    s16 temp_s1;
    Vec3f spC8;
    Vec3f spBC;
    f32 temp_f20;
    f32 temp_f22;
    f32 temp_f24;

    for (i = 0, phi_s2 = 0; i < 25; i++, phi_s2 += 0x4E20) {
        temp_f20 = (25 - i) * 2.4f;
        temp_f22 = Math_SinS(phi_s2) * temp_f20;
        temp_f24 = Math_CosS(phi_s2) * temp_f20;

        spC8.x = this->dyna.actor.world.pos.x + temp_f22;
        spC8.y = this->dyna.actor.world.pos.y;
        spC8.z = this->dyna.actor.world.pos.z + temp_f24;

        spBC.x = ((Rand_ZeroOne() - 0.5f) * 10.0f) + (temp_f22 * 0.13333334f);
        spBC.y = (Rand_ZeroOne() * 17.0f) + 7.0f;
        spBC.z = ((Rand_ZeroOne() - 0.5f) * 10.0f) + (temp_f24 * 0.13333334f);

        temp_s1 = (Rand_Next() & 3) + (i >> 2) + 4;

        if ((Rand_Next() % 4) == 0) {
            phi_v0 = 32;
        } else {
            phi_v0 = 64;
        }

        if (temp_s1 >= 8) {
            phi_v0 |= 1;
            phi_v1 = 1;
            phi_t0 = -550;
        } else {
            phi_v1 = 0;
            phi_t0 = -400;
        }

        EffectSsKakera_Spawn(play, &spC8, &spBC, &spC8, phi_t0, phi_v0, 30, 0, 0, temp_s1, phi_v1, 0, 50, -1,
                             OBJECT_HAKUGIN_OBJ, object_hakugin_obj_DL_009830);

        if ((i & 1) == 0) {
            func_800B0E48(play, &spC8, &D_80ABD034, &D_80ABCFB4, &D_80ABCFAC, &D_80ABCFB0, (Rand_Next() >> 0x1B) + 60,
                          (Rand_Next() >> 0x1A) + 50);
        }
    }
}

void func_80ABC58C(BgHakuginBombwall* this, PlayState* play) {
    s32 pad;
    Vec3f spD8;
    Vec3f spCC;
    Vec3f spC0;
    Vec3f spB4;
    Vec3f spA8;
    Vec3f sp9C;
    s32 i;
    s16 phi_s1;
    f32 temp_f20;
    f32 temp_f22;

    spA8.y = 0.0f;
    spB4.y = 0.3f;

    Matrix_RotateYS(this->dyna.actor.shape.rot.y, MTXMODE_NEW);

    for (i = 0, phi_s1 = 0; i < 21; i++, phi_s1 += 0x618) {
        temp_f20 = Math_SinS(phi_s1);
        temp_f22 = Math_CosS(phi_s1);

        sp9C.x = (i - 10) * 9.0f;
        sp9C.y = (Rand_ZeroOne() * 40.0f) + 15.0f;
        sp9C.z = temp_f20 * 10.0f;

        spA8.x = ((Rand_ZeroOne() - 0.5f) * 7.0f) + (temp_f22 * -12.0f);
        spA8.z = ((Rand_ZeroOne() - 0.5f) * 7.0f) + (temp_f20 * 16.0f);

        spB4.x = spA8.x * -0.09f;
        spB4.z = spA8.z * -0.09f;

        Matrix_MultVec3f(&sp9C, &spC0);
        Matrix_MultVec3f(&spA8, &spCC);
        Matrix_MultVec3f(&spB4, &spD8);

        spC0.x += this->dyna.actor.world.pos.x;
        spC0.y += this->dyna.actor.world.pos.y;
        spC0.z += this->dyna.actor.world.pos.z;

        func_800B0E48(play, &spC0, &spCC, &spD8, &D_80ABCFAC, &D_80ABCFB0, (Rand_Next() >> 0x1A) + 60,
                      (Rand_Next() >> 0x1B) + 60);
    }
}

void func_80ABC7FC(BgHakuginBombwall* this, PlayState* play) {
    s32 pad;
    Vec3f spB8;
    Vec3f spAC;
    Vec3f spA0;
    f32 temp_f20;
    f32 temp_f22;
    f32 temp_f24;
    s16 phi_s0;
    s32 i;

    spAC.y = 0.0f;
    spB8.y = 0.3f;

    for (i = 0, phi_s0 = 0; i < 20; i++, phi_s0 += 0xCCC) {
        temp_f20 = Math_SinS(phi_s0);
        temp_f22 = Math_CosS(phi_s0);
        temp_f24 = Rand_ZeroOne() * 60.0f;

        spA0.x = (temp_f22 * temp_f24) + this->dyna.actor.world.pos.x;
        spA0.y = (Rand_ZeroOne() * 20.0f) + this->dyna.actor.world.pos.y;
        spA0.z = (temp_f20 * temp_f24) + this->dyna.actor.world.pos.z;

        spAC.x = ((Rand_ZeroOne() - 0.5f) * 7.0f) + (temp_f20 * 15.0f);
        spAC.z = ((Rand_ZeroOne() - 0.5f) * 7.0f) + (temp_f22 * 15.0f);

        spB8.x = spAC.x * -0.095f;
        spB8.z = spAC.z * -0.095f;

        func_800B0E48(play, &spA0, &spAC, &spB8, &D_80ABCFAC, &D_80ABCFB0, (Rand_Next() >> 0x1A) + 60,
                      (Rand_Next() >> 0x1B) + 60);
    }
}

void BgHakuginBombwall_Init(Actor* thisx, PlayState* play) {
    s32 pad;
    BgHakuginBombwall* this = (BgHakuginBombwall*)thisx;
    BgHakuginBombwallStruct* ptr = &D_80ABCFC0[BGHAKUGIN_BOMBWALL_100(&this->dyna.actor)];

    Actor_ProcessInitChain(&this->dyna.actor, sInitChain);
    DynaPolyActor_Init(&this->dyna, 0);
    Collider_InitCylinder(play, &this->collider);

    if (Flags_GetSwitch(play, BGHAKUGIN_BOMBWALL_SWITCH_FLAG(&this->dyna.actor))) {
        Actor_Kill(&this->dyna.actor);
        return;
    }

    DynaPolyActor_LoadMesh(play, &this->dyna, ptr->unk_00);

    Collider_SetCylinder(play, &this->collider, &this->dyna.actor, &sCylinderInit);
    this->collider.dim.radius = ptr->unk_14;
    this->collider.dim.height = ptr->unk_16;
    this->collider.elem.acDmgInfo.dmgFlags = ptr->unk_18;
    Collider_UpdateCylinder(&this->dyna.actor, &this->collider);

    Actor_SetFocus(&this->dyna.actor, ptr->unk_08);
    this->dyna.actor.cullingVolumeScale = ptr->unk_0C;
    this->dyna.actor.cullingVolumeDownward = ptr->unk_10;
    this->actionFunc = func_80ABCCE4;
}

void BgHakuginBombwall_Destroy(Actor* thisx, PlayState* play) {
    BgHakuginBombwall* this = (BgHakuginBombwall*)thisx;

    DynaPoly_DeleteBgActor(play, &play->colCtx.dyna, this->dyna.bgId);
    Collider_DestroyCylinder(play, &this->collider);
}

s32 func_80ABCB5C(BgHakuginBombwall* this, PlayState* play) {
    if (this->collider.base.acFlags & AC_HIT) {
        if (this->collider.base.ac != NULL) {
            Actor* thisx = &this->dyna.actor;

            if (Math3D_Vec3fDistSq(&thisx->world.pos, &this->collider.base.ac->world.pos) <
                D_80ABCFC0[BGHAKUGIN_BOMBWALL_100(thisx)].unk_1C) {
                SoundSource_PlaySfxAtFixedWorldPos(play, &thisx->world.pos, 60, NA_SE_EV_WALL_BROKEN);
                return true;
            }
        }
    }
    return false;
}

s32 func_80ABCC00(BgHakuginBombwall* this, PlayState* play) {
    if (this->collider.base.acFlags & AC_HIT) {
        if (this->collider.elem.acHitElem->atDmgInfo.dmgFlags & 8) {
            if (this->collider.base.ac != NULL) {
                Actor* thisx = &this->dyna.actor;

                if (Math3D_Vec3fDistSq(&thisx->world.pos, &this->collider.base.ac->world.pos) <
                    D_80ABCFC0[BGHAKUGIN_BOMBWALL_100(thisx)].unk_1C) {
                    SoundSource_PlaySfxAtFixedWorldPos(play, &thisx->world.pos, 50, NA_SE_EV_WALL_BROKEN);
                    return true;
                }
            }
        } else if (DynaPolyActor_IsPlayerOnTop(&this->dyna)) {
            SoundSource_PlaySfxAtFixedWorldPos(play, &this->dyna.actor.world.pos, 50, NA_SE_EV_WALL_BROKEN);
            return true;
        }
    }
    return false;
}

void func_80ABCCE4(BgHakuginBombwall* this, PlayState* play) {
    BgHakuginBombwallStruct* ptr = &D_80ABCFC0[BGHAKUGIN_BOMBWALL_100(&this->dyna.actor)];

    if (ptr->unk_20(this, play)) {
        this->dyna.actor.flags |= ACTOR_FLAG_UPDATE_CULLING_DISABLED;
        CutsceneManager_Queue(this->dyna.actor.csId);
        this->actionFunc = func_80ABCD98;
    } else {
        this->collider.base.acFlags &= ~AC_HIT;
        CollisionCheck_SetAC(play, &play->colChkCtx, &this->collider.base);
    }
}

void func_80ABCD98(BgHakuginBombwall* this, PlayState* play) {
    s32 pad;

    if (CutsceneManager_IsNext(this->dyna.actor.csId)) {
        BgHakuginBombwallStruct* ptr = &D_80ABCFC0[BGHAKUGIN_BOMBWALL_100(&this->dyna.actor)];

        CutsceneManager_StartWithPlayerCs(this->dyna.actor.csId, &this->dyna.actor);
        ptr->unk_24(this, play);
        this->dyna.actor.draw = NULL;
        this->unk_1AC = 20;
        Flags_SetSwitch(play, BGHAKUGIN_BOMBWALL_SWITCH_FLAG(&this->dyna.actor));
        DynaPoly_DisableCollision(play, &play->colCtx.dyna, this->dyna.bgId);
        this->actionFunc = func_80ABCE60;
    } else {
        CutsceneManager_Queue(this->dyna.actor.csId);
    }
}

void func_80ABCE60(BgHakuginBombwall* this, PlayState* play) {
    BgHakuginBombwallStruct* ptr = &D_80ABCFC0[BGHAKUGIN_BOMBWALL_100(&this->dyna.actor)];

    this->unk_1AC--;
    if (this->unk_1AC <= 0) {
        CutsceneManager_Stop(this->dyna.actor.csId);
        Actor_Kill(&this->dyna.actor);
    } else if (this->unk_1AC == ptr->unk_2C) {
        ptr->unk_28(this, play);
    }
}

void BgHakuginBombwall_Update(Actor* thisx, PlayState* play) {
    BgHakuginBombwall* this = (BgHakuginBombwall*)thisx;

    this->actionFunc(this, play);
}

void BgHakuginBombwall_Draw(Actor* thisx, PlayState* play) {
    Gfx_DrawDListOpa(play, D_80ABCFC0[BGHAKUGIN_BOMBWALL_100(thisx)].unk_04);
}
