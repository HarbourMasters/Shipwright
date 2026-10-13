/*
 * File: z_obj_hugebombiwa.c
 * Overlay: ovl_Obj_Hugebombiwa
 * Description: Boulder Blocking Goron Racetrack/Milk Road
 */

#include "z_obj_hugebombiwa.h"
#include "z64quake.h"
#include "z64rumble.h"
#include "objects/object_bombiwa/object_bombiwa.h"
#include "2s2h/Enhancements/FrameInterpolation/FrameInterpolation.h"

#define FLAGS (ACTOR_FLAG_UPDATE_CULLING_DISABLED)

void ObjHugebombiwa_Init(Actor* thisx, PlayState* play);
void ObjHugebombiwa_Destroy(Actor* thisx, PlayState* play2);
void ObjHugebombiwa_Update(Actor* thisx, PlayState* play);
void ObjHugebombiwa_Draw(Actor* thisx, PlayState* play);

void func_80A54BF0(ObjHugebombiwa* this);
void func_80A54C04(ObjHugebombiwa* this, PlayState* play);
void func_80A54CD8(ObjHugebombiwa* this);
void func_80A54CEC(ObjHugebombiwa* this, PlayState* play);
void func_80A54E10(ObjHugebombiwa* this);
void func_80A55064(ObjHugebombiwa* this, PlayState* play);
void func_80A55310(ObjHugebombiwa* this);
void func_80A55564(ObjHugebombiwa* this, PlayState* play);
void func_80A55B34(Actor* thisx, PlayState* play);

ActorProfile Obj_Hugebombiwa_Profile = {
    /**/ ACTOR_OBJ_HUGEBOMBIWA,
    /**/ ACTORCAT_PROP,
    /**/ FLAGS,
    /**/ OBJECT_BOMBIWA,
    /**/ sizeof(ObjHugebombiwa),
    /**/ ObjHugebombiwa_Init,
    /**/ ObjHugebombiwa_Destroy,
    /**/ ObjHugebombiwa_Update,
    /**/ ObjHugebombiwa_Draw,
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
        { 0x81C37BB6, 0x00, 0x00 },
        ATELEM_NONE | ATELEM_SFX_NORMAL,
        ACELEM_ON,
        OCELEM_ON,
    },
    { 180, 226, 0, { 0, 0, 0 } },
};

static Vec3f D_80A55D2C = { 0.0f, 0.3f, 0.0f };

void func_80A53BE0(PlayState* play, Vec3f* arg1) {
    Vec3f pos;
    Vec3f velocity;
    s32 i;
    s32 gravity;
    s16 phi_v0;
    s16 life;
    s16 phi_v1;

    for (i = 0, gravity = -300; i < 16; i++, gravity -= 30) {
        pos.x = (Rand_ZeroOne() - 0.5f) * 260.0f;
        pos.y = i * (40.0f / 3);
        pos.z = (Rand_ZeroOne() - 0.5f) * 260.0f;

        velocity.x = ((Rand_ZeroOne() - 0.5f) * 5.7f) + (pos.x * 0.035f);
        velocity.y = (Rand_ZeroOne() * 16.0f) + 5.0f + ((16 - i) * 0.25f);
        velocity.z = ((Rand_ZeroOne() - 0.5f) * 5.7f) + (pos.z * 0.035f);

        pos.x += arg1->x;
        pos.y += arg1->y;
        pos.z += arg1->z;

        if (i >= 14) {
            phi_v0 = 37;
            life = 70;
        } else {
            phi_v0 = 65;
            if (i >= 12) {
                life = 70;
            } else {
                life = 40;
                if (Rand_ZeroOne() < 0.7f) {
                    phi_v0 = 64;
                } else {
                    phi_v0 = 32;
                }
            }
        }

        phi_v1 = i;
        if (phi_v1 <= 0) {
            phi_v1 = 1;
            if (1) {}
        }

        EffectSsKakera_Spawn(play, &pos, &velocity, &pos, gravity, phi_v0, 15, 0, 0, phi_v1, 1, 0, life, -1,
                             OBJECT_BOMBIWA, object_bombiwa_DL_001990);
    }
}

void func_80A53E60(PlayState* play, Vec3f* arg1, f32 arg2, f32 arg3) {
    static Color_RGBA8 D_80A55D38 = { 210, 210, 210, 255 };
    static Color_RGBA8 D_80A55D3C = { 140, 140, 140, 255 };
    static f32 D_80A55D40[] = { 3.0f, 5.0f, 9.0f, 18.0f };
    Vec3f spDC;
    Vec3f spD0;
    s32 i;
    f32 temp_f0;
    s32 phi_s1;
    s32 phi_s2;
    s32 phi_v0;
    s32 phi_v1;
    s16 phi_s0;
    s32 pad;

    for (phi_s2 = -300, i = 0; phi_s2 > -540; phi_s2 -= 60, i++) {
        spDC.x = (Rand_ZeroOne() - 0.5f) * 11.0f;
        spDC.y = (Rand_ZeroOne() - 0.2f) * 8.0f;
        spDC.z = (Rand_ZeroOne() - 0.5f) * 11.0f;

        spD0.x = ((Rand_ZeroOne() - 0.5f) * 5.0f) + (spDC.x * 1.4f);
        spD0.y = (Rand_ZeroOne() * 13.0f) + 8.2f + (arg2 * -0.26f);
        if (1) {}
        spD0.z = ((Rand_ZeroOne() - 0.5f) * 5.0f) + (spDC.z * 1.4f);

        spDC.x += arg1->x;
        spDC.y += arg1->y;
        spDC.z += arg1->z;

        if ((s32)(phi_s2 & 0xFFFFFFFF) == -480) {
            phi_v0 = 33;
            phi_s0 = 70;
        } else {
            phi_v0 = 65;
            if (phi_s2 == -420) {
                phi_s0 = 70;
            } else {
                phi_s0 = 40;
                if (Rand_ZeroOne() < 0.7f) {
                    phi_v0 = 64;
                } else {
                    phi_v0 = 32;
                }
            }
        }

        temp_f0 = D_80A55D40[i] * arg3;
        if (temp_f0 <= 1.0f) {
            phi_v1 = 1;
        } else {
            phi_v1 = temp_f0;
        }

        EffectSsKakera_Spawn(play, &spDC, &spD0, &spDC, phi_s2, phi_v0, 15, 0, 0, phi_v1, 1, 0, phi_s0, -1,
                             OBJECT_BOMBIWA, object_bombiwa_DL_001990);

        spDC.x += (Rand_ZeroOne() - 0.5f) * 270.0f;
        spDC.y += (Rand_ZeroOne() - 0.1f) * 150.0f;
        spDC.z += (Rand_ZeroOne() - 0.5f) * 270.0f;

        phi_s0 = (Rand_ZeroOne() * 160.0f) + 140.0f;
        phi_s1 = (Rand_ZeroOne() * 180.0f) + 120.0f;
        func_800B0E48(play, &spDC, &gZeroVec3f, &D_80A55D2C, &D_80A55D38, &D_80A55D3C, phi_s0, phi_s1);
    }
}

void func_80A541F4(ObjHugebombiwa* this, PlayState* play) {
    static s16 D_80A55D50[] = { 24, 17, 13, 7, 6, 5, 3, 2 };
    s32 i;
    s32 pad[2];
    s32 phi_s2;
    Vec3f spF4;
    Vec3f spE8;
    Vec3f spDC;
    Vec3f spD0;
    Vec3f spC4;
    f32 temp_f0;
    f32 temp_f20;
    f32 temp_f22;
    f32 temp_f24;
    s32 phi_s0;
    f32 phi_f30;

    for (i = 0, phi_s2 = 0, phi_f30 = 0.0f; i < 13; i++, phi_s2 += 0x4E20, phi_f30 += (230.0f / 13)) {
        temp_f0 = Rand_ZeroOne();
        temp_f22 = (1.0f - SQ(temp_f0)) * 120.0f;
        temp_f20 = Math_SinS(phi_s2 & 0xFFFF);
        temp_f24 = Math_CosS(phi_s2 & 0xFFFF);

        spF4.x = (temp_f20 * temp_f22) + this->actor.world.pos.x;
        spF4.y = this->actor.world.pos.y + phi_f30;
        spF4.z = (temp_f24 * temp_f22) + this->actor.world.pos.z;

        spE8.x = temp_f20 * 10.0f;
        spE8.y = (Rand_ZeroOne() * 18.0f) + 10.0f;
        spE8.z = temp_f24 * 10.0f;

        EffectSsKakera_Spawn(play, &spF4, &spE8, &spF4, -650, 37, 15, 0, 0, D_80A55D50[i & 7], 1, 0, 60, -1,
                             OBJECT_BOMBIWA, object_bombiwa_DL_0009E0);

        spDC.x = ((Rand_ZeroOne() - 0.5f) * 230.0f) + spF4.x;
        spDC.y = ((Rand_ZeroOne() - 0.2f) * 200.0f) + spF4.y;
        spDC.z = ((Rand_ZeroOne() - 0.5f) * 230.0f) + spF4.z;

        spD0.x = temp_f20 * 7.0f;
        spD0.y = -7.0f;
        spD0.z = temp_f24 * 7.0f;

        spC4.x = temp_f20 * -0.07f;
        spC4.y = 0.24f;
        spC4.z = temp_f24 * -0.07f;

        func_800B12F0(play, &spDC, &spD0, &spC4, (s32)(Rand_ZeroOne() * 800.0f) + 1000, -49, 20);

        spDC.x = ((Rand_ZeroOne() - 0.5f) * 160.0f) + spF4.x;
        spDC.y = ((Rand_ZeroOne() - 0.2f) * 140.0f) + spF4.y;
        spDC.z = ((Rand_ZeroOne() - 0.5f) * 160.0f) + spF4.z;

        spD0.x = temp_f20 * 15.0f;
        spD0.y = 0.0f;
        spD0.z = temp_f24 * 15.0f;

        spC4.x = temp_f20 * -0.09f;
        spC4.y = 0.3f;
        spC4.z = temp_f24 * -0.09f;

        func_800B12F0(play, &spDC, &spD0, &spC4, (s32)(Rand_ZeroOne() * 100.0f) + 40,
                      (s32)(Rand_ZeroOne() * 200.0f) + 20, 10);
    }
}

void func_80A54600(PlayState* play, Vec3f* arg1, f32 arg2, f32 arg3) {
    static f32 D_80A55D60[] = { 3.0f, 5.0f, 9.0f, 18.0f };
    static s8 D_80A55D70 = 0;
    Vec3f spCC;
    Vec3f spC0;
    s32 i;
    f32 spA0;
    s16 temp_s0;
    s16 temp_s1;
    s32 phi_v0;
    s16 phi_v1;
    f32 temp;

    for (i = 0; i < 2; i++) {
        spCC.x = (Rand_ZeroOne() - 0.5f) * 11.0f;
        spCC.y = (Rand_ZeroOne() - 0.2f) * 8.0f;
        spCC.z = (Rand_ZeroOne() - 0.5f) * 11.0f;

        spC0.x = ((Rand_ZeroOne() - 0.5f) * 5.0f) + (spCC.x * 1.4f);
        spC0.y = (Rand_ZeroOne() * 13.0f) + 8.2f + (arg2 * -0.38f);
        spC0.z = ((Rand_ZeroOne() - 0.5f) * 5.0f) + (spCC.z * 1.4f);

        spCC.x += arg1->x;
        spCC.y += arg1->y;
        spCC.z += arg1->z;

        temp = D_80A55D60[D_80A55D70] * arg3;
        phi_v1 = (D_80A55D70 * -90) - 350;
        D_80A55D70++;
        D_80A55D70 &= 3;

        if (temp <= 1.0f) {
            phi_v0 = 1;
        } else {
            phi_v0 = temp;
        }

        EffectSsKakera_Spawn(play, &spCC, &spC0, &spCC, phi_v1, 33, 15, 0, 0, phi_v0, 1, 0, 70, -1, OBJECT_BOMBIWA,
                             object_bombiwa_DL_0009E0);

        spCC.x += (Rand_ZeroOne() - 0.5f) * 270.0f;
        spCC.y += (Rand_ZeroOne() - 0.1f) * 150.0f;
        spCC.z += (Rand_ZeroOne() - 0.5f) * 270.0f;

        temp_s0 = (Rand_ZeroOne() * 160.0f) + 140.0f;
        temp_s1 = (Rand_ZeroOne() * 180.0f) + 120.0f;
        func_800B1210(play, &spCC, &gZeroVec3f, &D_80A55D2C, temp_s0, temp_s1);
    }
}

void ObjHugebombiwa_RequestQuakeAndRumble(ObjHugebombiwa* this, PlayState* play, s32 quakeVerticalMag) {
    s32 pad[2];
    s16 quakeIndex = Quake_Request(GET_ACTIVE_CAM(play), QUAKE_TYPE_3);

    Quake_SetSpeed(quakeIndex, 20000);
    Quake_SetPerturbations(quakeIndex, quakeVerticalMag, 0, 0, 0);
    Quake_SetDuration(quakeIndex, 7);

    Rumble_Request(this->actor.xyzDistToPlayerSq, 255, 20, 150);
}

s32 func_80A54A0C(ObjHugebombiwa* this) {
    static f32 D_80A55D74[] = { 62500.0f, 108900.0f };
    s32 sp2C;
    Actor* ac;
    s32 params;
    Vec3f sp20;

    if ((this->collider.base.acFlags & AC_HIT) && (this->collider.elem.acHitElem->atDmgInfo.dmgFlags & 0x80000000)) {
        ac = this->collider.base.ac;
        params = ENHUGEBOMBIWA_GET_100(&this->actor);

        sp20.x = this->actor.world.pos.x;
        sp20.y = this->actor.world.pos.y + 50.0f;
        sp20.z = this->actor.world.pos.z;
        if (ac != NULL) {
            if (Math3D_Vec3fDistSq(&sp20, &ac->world.pos) < D_80A55D74[params]) {
                return true;
            }
        }
    }
    return false;
}

static InitChainEntry sInitChain[] = {
    ICHAIN_F32(cullingVolumeDistance, 3700, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeScale, 900, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeDownward, 900, ICHAIN_STOP),
};

void ObjHugebombiwa_Init(Actor* thisx, PlayState* play) {
    s32 pad;
    ObjHugebombiwa* this = (ObjHugebombiwa*)thisx;

    Actor_ProcessInitChain(&this->actor, sInitChain);
    Collider_InitCylinder(play, &this->collider);

    if (Flags_GetSwitch(play, ENHUGEBOMBIWA_GET_SWITCH_FLAG(&this->actor))) {
        Actor_Kill(&this->actor);
        return;
    }

    Collider_SetCylinder(play, &this->collider, &this->actor, &sCylinderInit);
    Collider_UpdateCylinder(&this->actor, &this->collider);
    this->actor.colChkInfo.mass = MASS_IMMOVABLE;
    if ((ENHUGEBOMBIWA_GET_100(&this->actor)) == 1) {
        this->actor.draw = func_80A55B34;
        Actor_SetScale(&this->actor, 0.74f);
        this->collider.dim.radius = 204;
        this->collider.dim.height = 230;
    } else {
        Actor_SetScale(&this->actor, 0.067f);
        this->collider.dim.radius = 120;
        this->collider.dim.height = 151;
    }
    func_80A54BF0(this);
}

void ObjHugebombiwa_Destroy(Actor* thisx, PlayState* play2) {
    PlayState* play = play2;
    ObjHugebombiwa* this = (ObjHugebombiwa*)thisx;

    Collider_DestroyCylinder(play, &this->collider);
}

void func_80A54BF0(ObjHugebombiwa* this) {
    this->actionFunc = func_80A54C04;
}

void func_80A54C04(ObjHugebombiwa* this, PlayState* play) {
    s32 pad;

    if (this->collider.base.acFlags & AC_HIT) {
        this->unk_4B3 = 5;
    }

    if (func_80A54A0C(this)) {
        CutsceneManager_Queue(this->actor.csId);
        func_80A54CD8(this);
        return;
    }

    this->collider.base.acFlags &= ~AC_HIT;

    if (this->unk_4B3 > 0) {
        this->unk_4B3--;
        if (this->unk_4B3 == 0) {
            this->collider.base.colMaterial = COL_MATERIAL_HARD;
        } else {
            this->collider.base.colMaterial = COL_MATERIAL_NONE;
        }
    }

    CollisionCheck_SetAC(play, &play->colChkCtx, &this->collider.base);
    CollisionCheck_SetOC(play, &play->colChkCtx, &this->collider.base);
}

void func_80A54CD8(ObjHugebombiwa* this) {
    this->actionFunc = func_80A54CEC;
}

void func_80A54CEC(ObjHugebombiwa* this, PlayState* play) {
    s32 pad;

    if (CutsceneManager_IsNext(this->actor.csId)) {
        CutsceneManager_StartWithPlayerCs(this->actor.csId, &this->actor);
        Flags_SetSwitch(play, ENHUGEBOMBIWA_GET_SWITCH_FLAG(&this->actor));
        if (!ENHUGEBOMBIWA_GET_100(&this->actor) &&
            ((play->sceneId == SCENE_17SETUGEN) || (play->sceneId == SCENE_17SETUGEN2))) {
            SET_WEEKEVENTREG(WEEKEVENTREG_19_02);
        }

        if (!ENHUGEBOMBIWA_GET_100(&this->actor)) {
            SoundSource_PlaySfxAtFixedWorldPos(play, &this->actor.world.pos, 80, NA_SE_EV_WALL_BROKEN);
        } else {
            SoundSource_PlaySfxAtFixedWorldPos(play, &this->actor.world.pos, 80, NA_SE_EV_SNOWBALL_BROKEN);
        }

        if (!ENHUGEBOMBIWA_GET_100(&this->actor)) {
            func_80A53BE0(play, &this->actor.world.pos);
            func_80A54E10(this);
        } else {
            func_80A541F4(this, play);
            func_80A55310(this);
        }
    } else {
        CutsceneManager_Queue(this->actor.csId);
    }
}

void func_80A54E10(ObjHugebombiwa* this) {
    s32 i;
    EnHugebombiwaStruct* ptr;
    f32 temp_f20;
    f32 temp_f20_2;
    s16 phi_s2;
    s32 pad;
    Vec3f sp84;

    Matrix_Push();
    Matrix_RotateYS(this->actor.shape.rot.y, MTXMODE_NEW);

    for (i = 0, phi_s2 = 0x1000; i < 20; i++, phi_s2 += 0x4000) {
        ptr = &this->unk_190[i];

        temp_f20 = (Rand_ZeroOne() * 0.06f) + 0.013f;
        ptr->unk_00.x = ((Rand_ZeroOne() * 0.6f) + 0.6f) * temp_f20;
        ptr->unk_00.y = ((Rand_ZeroOne() * 0.6f) + 0.4f) * temp_f20;
        ptr->unk_00.z = ((Rand_ZeroOne() * 0.6f) + 0.6f) * temp_f20;

        temp_f20_2 = (Rand_ZeroOne() * 55.0f) + 47.0f;
        sp84.x = Math_SinS(phi_s2) * temp_f20_2;
        sp84.y = (i + 1) * 10.0f;
        sp84.z = fabsf(Math_CosS(phi_s2)) * temp_f20_2;

        Matrix_MultVec3f(&sp84, &ptr->unk_0C);

        ptr->unk_0C.x += this->actor.world.pos.x;
        ptr->unk_0C.y += this->actor.world.pos.y;
        ptr->unk_0C.z += this->actor.world.pos.z;

        ptr->unk_18 = (i * 1.04f) + 2.4f;
        ptr->unk_1C.x = phi_s2;
        ptr->unk_1C.y = Rand_Next() >> 0x10;
        ptr->unk_1C.z = 0;
        ptr->unk_22 = Rand_ZeroFloat(5000.0f);
        ptr->unk_24 = 0;
    }

    this->unk_4B0 = 0;
    this->unk_4B2 = 100;
    Matrix_Pop();
    this->actionFunc = func_80A55064;
}

void func_80A55064(ObjHugebombiwa* this, PlayState* play) {
    s32 i;
    s32 pad;
    Vec3f spA4;
    s32 pad2;
    CollisionPoly* sp9C;
    s32 bgId;
    f32 temp_f0;
    EnHugebombiwaStruct* ptr;
    s16 phi_s3 = this->actor.shape.rot.y - 0x4000;

    for (i = 0; i < ARRAY_COUNT(this->unk_190); i++, phi_s3 += 0x666) {
        ptr = &this->unk_190[i];
        if (ptr->unk_24 != 0) {
            continue;
        }

        ptr->unk_18 -= 3.0f;
        if (ptr->unk_18 < -30.0f) {
            ptr->unk_18 = -30.0f;
        }

        ptr->unk_0C.x += Math_SinS(phi_s3) * 4.0f;
        ptr->unk_0C.y += ptr->unk_18;
        ptr->unk_0C.z += Math_CosS(phi_s3) * 4.0f;

        ptr->unk_1C.x += ptr->unk_22;

        spA4.x = ptr->unk_0C.x;
        spA4.y = ptr->unk_0C.y + 60.0f;
        spA4.z = ptr->unk_0C.z;

        temp_f0 = BgCheck_EntityRaycastFloor5(&play->colCtx, &sp9C, &bgId, &this->actor, &spA4);
        if ((temp_f0 <= BGCHECK_Y_MIN + 10.0f) || ((ptr->unk_0C.y - (350.0f * ptr->unk_00.y)) < temp_f0)) {
            this->unk_4B0++;
            ptr->unk_24 = 1;
            func_80A53E60(play, &ptr->unk_0C, ptr->unk_18, ptr->unk_00.y * 9.8f);
            if ((play->gameplayFrames % 4) == 0) {
                ObjHugebombiwa_RequestQuakeAndRumble(this, play, (s32)(Rand_ZeroOne() * 5.5f) + 1);
            }
        }
    }

    this->unk_4B2--;
    if ((this->unk_4B0 >= 20) || (this->unk_4B2 <= 0)) {
        CutsceneManager_Stop(this->actor.csId);
        Actor_Kill(&this->actor);
    }
}

void func_80A55310(ObjHugebombiwa* this) {
    s32 i;
    EnHugebombiwaStruct* ptr;
    s32 pad;
    f32 temp_f20;
    f32 temp_f20_2;
    s16 phi_s2;
    Vec3f sp84;

    Matrix_Push();
    Matrix_RotateYS(this->actor.shape.rot.y, MTXMODE_NEW);

    for (i = 0, phi_s2 = 0x1000; i < ARRAY_COUNT(this->unk_190); i++, phi_s2 += 0x4000) {
        ptr = &this->unk_190[i];

        temp_f20 = (Rand_ZeroOne() * 0.09f) + 0.016f;
        ptr->unk_00.x = ((Rand_ZeroOne() * 0.1f) + 0.95f) * temp_f20;
        ptr->unk_00.y = ((Rand_ZeroOne() * 0.1f) + 0.95f) * temp_f20;
        ptr->unk_00.z = ((Rand_ZeroOne() * 0.1f) + 0.95f) * temp_f20;

        temp_f20_2 = (Rand_ZeroOne() * 85.0f) + 77.0f;
        sp84.x = Math_SinS(phi_s2) * temp_f20_2;
        sp84.y = (i + 1) * 14.0f;
        sp84.z = fabsf(Math_CosS(phi_s2)) * temp_f20_2;
        Matrix_MultVec3f(&sp84, &ptr->unk_0C);

        ptr->unk_0C.x += this->actor.world.pos.x;
        ptr->unk_0C.y += this->actor.world.pos.y;
        ptr->unk_0C.z += this->actor.world.pos.z;

        ptr->unk_18 = (i * 1.04f) + 2.4f;

        ptr->unk_1C.x = phi_s2;
        ptr->unk_1C.y = Rand_Next() >> 0x10;
        ptr->unk_1C.z = 0;

        ptr->unk_22 = Rand_ZeroFloat(5000.0f);
        ptr->unk_24 = 0;
    }

    this->unk_4B0 = 0;
    this->unk_4B2 = 100;
    Matrix_Pop();
    this->actionFunc = func_80A55564;
}

void func_80A55564(ObjHugebombiwa* this, PlayState* play) {
    s32 i;
    EnHugebombiwaStruct* ptr;
    Vec3f spA4;
    s32 pad;
    CollisionPoly* sp9C;
    s32 bgId;
    f32 temp_f0;
    s16 phi_s3 = this->actor.shape.rot.y - 0x4000;

    for (i = 0; i < ARRAY_COUNT(this->unk_190); i++, phi_s3 += 0x666) {
        ptr = &this->unk_190[i];

        if (ptr->unk_24 != 0) {
            continue;
        }

        ptr->unk_18 -= 3.0f;
        if (ptr->unk_18 < -30.0f) {
            ptr->unk_18 = -30.0f;
        }

        ptr->unk_0C.x += (Math_SinS(phi_s3) * 4.0f);
        ptr->unk_0C.y += ptr->unk_18;
        ptr->unk_0C.z += (Math_CosS(phi_s3) * 4.0f);

        ptr->unk_1C.x += ptr->unk_22;

        spA4.x = ptr->unk_0C.x;
        spA4.y = ptr->unk_0C.y + 60.0f;
        spA4.z = ptr->unk_0C.z;

        temp_f0 = BgCheck_EntityRaycastFloor5(&play->colCtx, &sp9C, &bgId, &this->actor, &spA4);
        if ((temp_f0 <= BGCHECK_Y_MIN + 10.0f) || (ptr->unk_0C.y < temp_f0)) {
            this->unk_4B0++;
            ptr->unk_24 = 1;
            func_80A54600(play, &ptr->unk_0C, ptr->unk_18, ptr->unk_00.y * 10.1f);
            if ((play->gameplayFrames % 4) == 0) {
                ObjHugebombiwa_RequestQuakeAndRumble(this, play, (s32)(Rand_ZeroOne() * 5.5f) + 1);
            }
        }
    }

    this->unk_4B2--;
    if ((this->unk_4B0 >= 20) || (this->unk_4B2 <= 0)) {
        CutsceneManager_Stop(this->actor.csId);
        Actor_Kill(&this->actor);
    }
}

void ObjHugebombiwa_Update(Actor* thisx, PlayState* play) {
    ObjHugebombiwa* this = (ObjHugebombiwa*)thisx;

    this->actionFunc(this, play);
}

void ObjHugebombiwa_Draw(Actor* thisx, PlayState* play) {
    ObjHugebombiwa* this = (ObjHugebombiwa*)thisx;
    s32 pad[8];
    f32 sp38;

    OPEN_DISPS(play->state.gfxCtx);

    if ((this->actionFunc == func_80A54C04) || (this->actionFunc == func_80A54CEC)) {
        if (this->actor.projectedPos.z <= 4300.0f) {
            Gfx_SetupDL25_Opa(play->state.gfxCtx);

            gSPSegment(POLY_OPA_DISP++, 0x08, D_801AEFA0);
            MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx);
            gDPSetPrimColor(POLY_OPA_DISP++, 0, 0xFF, 255, 255, 255, 255);
            gSPDisplayList(POLY_OPA_DISP++, object_bombiwa_DL_002F60);

            Gfx_SetupDL25_Xlu(play->state.gfxCtx);

            MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx);
            gSPDisplayList(POLY_XLU_DISP++, object_bombiwa_DL_003110);

        } else if (this->actor.projectedPos.z < 4500.0f) {
            sp38 = (4500.0f - this->actor.projectedPos.z) * 1.275f;
            Gfx_SetupDL25_Xlu(play->state.gfxCtx);

            gSPSegment(POLY_XLU_DISP++, 0x08, D_801AEF88);
            MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx);
            gDPSetPrimColor(POLY_XLU_DISP++, 0, 0xFF, 255, 255, 255, (s32)sp38);
            gSPDisplayList(POLY_XLU_DISP++, object_bombiwa_DL_002F60);
        }
    } else {
        s32 i;
        EnHugebombiwaStruct* ptr;
        Gfx* gfx = POLY_OPA_DISP;

        gSPDisplayList(gfx++, gSetupDLs[SETUPDL_25]);

        for (i = 0; i < ARRAY_COUNT(this->unk_190); i++) {
            ptr = &this->unk_190[i];

            if (ptr->unk_24 == 0) {
                FrameInterpolation_RecordOpenChild(ptr, i);
                Matrix_SetTranslateRotateYXZ(ptr->unk_0C.x, ptr->unk_0C.y, ptr->unk_0C.z, &ptr->unk_1C);
                Matrix_Scale(ptr->unk_00.x, ptr->unk_00.x, ptr->unk_00.x, MTXMODE_APPLY);

                MATRIX_FINALIZE_AND_LOAD(gfx++, play->state.gfxCtx);
                gSPDisplayList(gfx++, object_bombiwa_DL_001990);
                FrameInterpolation_RecordCloseChild();
            }
        }

        POLY_OPA_DISP = gfx;
    }

    CLOSE_DISPS(play->state.gfxCtx);
}

void func_80A55B34(Actor* thisx, PlayState* play) {
    ObjHugebombiwa* this = (ObjHugebombiwa*)thisx;
    s32 i;
    Gfx* gfx;
    EnHugebombiwaStruct* ptr;

    if ((this->actionFunc == func_80A54C04) || (this->actionFunc == func_80A54CEC)) {
        Gfx_DrawDListOpa(play, object_bombiwa_DL_001820);
        return;
    }

    OPEN_DISPS(play->state.gfxCtx);

    gfx = POLY_OPA_DISP;

    gSPDisplayList(gfx++, gSetupDLs[SETUPDL_25]);

    for (i = 0; i < ARRAY_COUNT(this->unk_190); i++) {
        ptr = &this->unk_190[i];

        if (ptr->unk_24 != 0) {
            continue;
        }

        FrameInterpolation_RecordOpenChild(ptr, i);
        Matrix_SetTranslateRotateYXZ(ptr->unk_0C.x, ptr->unk_0C.y + (325.0f * ptr->unk_00.y), ptr->unk_0C.z,
                                     &ptr->unk_1C);
        Matrix_Scale(ptr->unk_00.x, ptr->unk_00.y, ptr->unk_00.z, MTXMODE_APPLY);
        Matrix_Translate(0.0f, -325.0f, 0.0f, MTXMODE_APPLY);

        MATRIX_FINALIZE_AND_LOAD(gfx++, play->state.gfxCtx);
        gSPDisplayList(gfx++, object_bombiwa_DL_0009E0);
        FrameInterpolation_RecordCloseChild();
    }

    POLY_OPA_DISP = gfx;

    CLOSE_DISPS(play->state.gfxCtx);
}
