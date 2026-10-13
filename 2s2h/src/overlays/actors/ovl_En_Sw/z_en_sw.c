/*
 * File: z_en_sw.c
 * Overlay: ovl_En_Sw
 * Description: Skullwalltula
 */

#include "z_en_sw.h"
#include "overlays/actors/ovl_En_Clear_Tag/z_en_clear_tag.h"

#define FLAGS (ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_HOSTILE)

void EnSw_Init(Actor* thisx, PlayState* play);
void EnSw_Destroy(Actor* thisx, PlayState* play);
void EnSw_Update(Actor* thisx, PlayState* play);
void EnSw_Draw(Actor* thisx, PlayState* play);

void func_808DA350(EnSw* this, PlayState* play);
void func_808DA3F4(EnSw* this, PlayState* play);
void func_808DA578(EnSw* this, PlayState* play);
void func_808DA6FC(EnSw* this, PlayState* play);
void func_808DA89C(EnSw* this, PlayState* play);
void func_808DAA60(EnSw* this, PlayState* play);
void func_808DACF4(EnSw* this, PlayState* play);
void func_808DAEB4(EnSw* this, PlayState* play);
void func_808DB100(EnSw* this, PlayState* play);
void func_808DB25C(EnSw* this, PlayState* play);
void func_808DB2E0(EnSw* this, PlayState* play);

ActorProfile En_Sw_Profile = {
    /**/ ACTOR_EN_SW,
    /**/ ACTORCAT_NPC,
    /**/ FLAGS,
    /**/ OBJECT_ST,
    /**/ sizeof(EnSw),
    /**/ EnSw_Init,
    /**/ EnSw_Destroy,
    /**/ EnSw_Update,
    /**/ EnSw_Draw,
};

static ColliderSphereInit sSphereInit = {
    {
        COL_MATERIAL_NONE,
        AT_ON | AT_TYPE_ENEMY,
        AC_ON | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_1,
        COLSHAPE_SPHERE,
    },
    {
        ELEM_MATERIAL_UNK0,
        { 0xF7CFFFFF, 0x00, 0x00 },
        { 0xF7CFFFFF, 0x00, 0x00 },
        ATELEM_ON | ATELEM_SFX_NORMAL,
        ACELEM_ON,
        OCELEM_ON,
    },
    { 0, { { 0, 0, 0 }, 16 }, 100 },
};

static CollisionCheckInfoInit2 sColChkInfoInit = { 1, 0, 0, 0, MASS_IMMOVABLE };

static DamageTable sDamageTable = {
    /* Deku Nut       */ DMG_ENTRY(1, 0x0),
    /* Deku Stick     */ DMG_ENTRY(1, 0x0),
    /* Horse trample  */ DMG_ENTRY(0, 0x0),
    /* Explosives     */ DMG_ENTRY(1, 0x0),
    /* Zora boomerang */ DMG_ENTRY(1, 0x0),
    /* Normal arrow   */ DMG_ENTRY(1, 0x0),
    /* UNK_DMG_0x06   */ DMG_ENTRY(0, 0x0),
    /* Hookshot       */ DMG_ENTRY(1, 0x0),
    /* Goron punch    */ DMG_ENTRY(1, 0x0),
    /* Sword          */ DMG_ENTRY(1, 0x0),
    /* Goron pound    */ DMG_ENTRY(2, 0x0),
    /* Fire arrow     */ DMG_ENTRY(2, 0x2),
    /* Ice arrow      */ DMG_ENTRY(2, 0x3),
    /* Light arrow    */ DMG_ENTRY(2, 0x4),
    /* Goron spikes   */ DMG_ENTRY(1, 0x0),
    /* Deku spin      */ DMG_ENTRY(1, 0x0),
    /* Deku bubble    */ DMG_ENTRY(1, 0x0),
    /* Deku launch    */ DMG_ENTRY(2, 0x0),
    /* UNK_DMG_0x12   */ DMG_ENTRY(1, 0x0),
    /* Zora barrier   */ DMG_ENTRY(1, 0x5),
    /* Normal shield  */ DMG_ENTRY(1, 0x0),
    /* Light ray      */ DMG_ENTRY(1, 0x0),
    /* Thrown object  */ DMG_ENTRY(1, 0x0),
    /* Zora punch     */ DMG_ENTRY(1, 0x0),
    /* Spin attack    */ DMG_ENTRY(1, 0x0),
    /* Sword beam     */ DMG_ENTRY(0, 0x0),
    /* Normal Roll    */ DMG_ENTRY(0, 0x0),
    /* UNK_DMG_0x1B   */ DMG_ENTRY(0, 0x0),
    /* UNK_DMG_0x1C   */ DMG_ENTRY(0, 0x0),
    /* Unblockable    */ DMG_ENTRY(0, 0x0),
    /* UNK_DMG_0x1E   */ DMG_ENTRY(0, 0x0),
    /* Powder Keg     */ DMG_ENTRY(1, 0x0),
};

static CollisionCheckInfoInit2 sColChkInfoInit2 = { 1, 0, 0, 0, MASS_IMMOVABLE };

static DamageTable sDamageTable2 = {
    /* Deku Nut       */ DMG_ENTRY(0, 0x1),
    /* Deku Stick     */ DMG_ENTRY(1, 0x0),
    /* Horse trample  */ DMG_ENTRY(0, 0x0),
    /* Explosives     */ DMG_ENTRY(1, 0x0),
    /* Zora boomerang */ DMG_ENTRY(1, 0x0),
    /* Normal arrow   */ DMG_ENTRY(1, 0x0),
    /* UNK_DMG_0x06   */ DMG_ENTRY(0, 0x0),
    /* Hookshot       */ DMG_ENTRY(1, 0x0),
    /* Goron punch    */ DMG_ENTRY(1, 0x0),
    /* Sword          */ DMG_ENTRY(1, 0x0),
    /* Goron pound    */ DMG_ENTRY(2, 0x0),
    /* Fire arrow     */ DMG_ENTRY(2, 0x2),
    /* Ice arrow      */ DMG_ENTRY(2, 0x3),
    /* Light arrow    */ DMG_ENTRY(2, 0x4),
    /* Goron spikes   */ DMG_ENTRY(1, 0x0),
    /* Deku spin      */ DMG_ENTRY(1, 0x0),
    /* Deku bubble    */ DMG_ENTRY(1, 0x0),
    /* Deku launch    */ DMG_ENTRY(2, 0x0),
    /* UNK_DMG_0x12   */ DMG_ENTRY(0, 0x1),
    /* Zora barrier   */ DMG_ENTRY(1, 0x5),
    /* Normal shield  */ DMG_ENTRY(1, 0x0),
    /* Light ray      */ DMG_ENTRY(1, 0x0),
    /* Thrown object  */ DMG_ENTRY(1, 0x0),
    /* Zora punch     */ DMG_ENTRY(1, 0x0),
    /* Spin attack    */ DMG_ENTRY(1, 0x0),
    /* Sword beam     */ DMG_ENTRY(0, 0x0),
    /* Normal Roll    */ DMG_ENTRY(0, 0x0),
    /* UNK_DMG_0x1B   */ DMG_ENTRY(0, 0x0),
    /* UNK_DMG_0x1C   */ DMG_ENTRY(0, 0x0),
    /* Unblockable    */ DMG_ENTRY(0, 0x0),
    /* UNK_DMG_0x1E   */ DMG_ENTRY(0, 0x0),
    /* Powder Keg     */ DMG_ENTRY(1, 0x0),
};

typedef enum EnSwAnimation {
    /* -1 */ ENSW_ANIM_NONE = -1,
    /*  0 */ ENSW_ANIM_0,
    /*  1 */ ENSW_ANIM_1,
    /*  2 */ ENSW_ANIM_2,
    /*  3 */ ENSW_ANIM_3,
    /*  4 */ ENSW_ANIM_MAX
} EnSwAnimation;

static AnimationInfoS sAnimationInfo[ENSW_ANIM_MAX] = {
    { &object_st_Anim_000304, 1.0f, 0, -1, ANIMMODE_ONCE_INTERP, 0 },  // ENSW_ANIM_0
    { &object_st_Anim_000304, 1.0f, 0, -1, ANIMMODE_ONCE_INTERP, -4 }, // ENSW_ANIM_1
    { &object_st_Anim_0055A8, 1.0f, 0, -1, ANIMMODE_LOOP_INTERP, -4 }, // ENSW_ANIM_2
    { &object_st_Anim_005B98, 1.0f, 0, -1, ANIMMODE_LOOP_INTERP, -4 }, // ENSW_ANIM_3
};

void func_808D8940(EnSw* this, PlayState* play) {
    static Color_RGBA8 sPrimColor = { 170, 130, 90, 255 };
    static Color_RGBA8 sEnvColor = { 100, 60, 20, 0 };
    s32 i;
    Vec3f spB8;
    Vec3f spAC;
    Vec3f spA0;
    Vec3f sp94;
    s32 temp_f4;
    s16 temp_s0;

    temp_s0 = (Rand_ZeroOne() - 0.5f) * 0x10000;
    spA0.y = this->actor.floorHeight;

    for (i = 0; i < 8; i++, temp_s0 += 0x1FFE) {
        temp_f4 = (Rand_ZeroOne() * 4.0f) + 8.0f;
        sp94.x = 0.0f;
        sp94.y = (Rand_ZeroOne() * 0.2f) + 0.1f;
        sp94.z = Rand_ZeroOne() + 1.0f;
        Lib_Vec3f_TranslateAndRotateY(&gZeroVec3f, temp_s0, &sp94, &spAC);
        sp94.x = 0.0f;
        sp94.y = 0.7f;
        sp94.z = 2.0f;
        Lib_Vec3f_TranslateAndRotateY(&gZeroVec3f, temp_s0, &sp94, &spB8);
        spA0.x = this->actor.world.pos.x + (2.0f * spB8.x);
        spA0.z = this->actor.world.pos.z + (2.0f * spB8.z);
        func_800B0EB0(play, &spA0, &spB8, &spAC, &sPrimColor, &sEnvColor, 60, 30, temp_f4);
    }
}

s32 func_808D8B58(EnSw* this) {
    s16 phi_s2 = (s16)(TRUNCF_BINANG(Rand_ZeroOne() * 1000.0f) % ENSW_BODYPART_MAX) * 0x1555;
    s32 i;

    for (i = 0; i < ENSW_BODYPART_MAX; i++, phi_s2 += 0x1555) {
        if (this->drawDmgEffType != ACTOR_DRAW_DMGEFF_FROZEN_NO_SFX) {
            this->unk_464[i] = (Rand_ZeroOne() * 16.0f) + 8.0f;
        } else {
            this->unk_464[i] = 80;
        }
        this->unk_47C[i] = this->unk_464[i];
        this->drawDmgEffFrozenSteamScales[i] = 0.45000002f;
        if ((this->drawDmgEffType == ACTOR_DRAW_DMGEFF_FIRE) || (this->drawDmgEffType == ACTOR_DRAW_DMGEFF_BLUE_FIRE) ||
            (this->drawDmgEffType == ACTOR_DRAW_DMGEFF_FROZEN_NO_SFX)) {
            this->bodyPartsPos[i].y = (Rand_ZeroOne() - 0.5f) * 20.0f;
        } else {
            this->bodyPartsPos[i].y = ((Rand_ZeroOne() - 0.5f) * 20.0f) + 10.0f;
        }
        this->bodyPartsPos[i].x = Math_SinS(phi_s2) * 10.0f;
        this->bodyPartsPos[i].z = Math_CosS(phi_s2) * 10.0f;
    }
    this->unk_462 = 1;
    return 0;
}

s32 func_808D8D60(EnSw* this, PlayState* play, s32 bodyPartIndex) {
    s32 ret = 0;
    u8 drawDmgEffType;
    Vec3f bodyPartsPos[1];
    f32 drawDmgEffAlpha;

    if (bodyPartIndex < this->unk_462) {
        if (this->unk_464[bodyPartIndex] != 0) {
            drawDmgEffAlpha = (f32)this->unk_464[bodyPartIndex] / this->unk_47C[bodyPartIndex];
            drawDmgEffType = this->drawDmgEffType;
            Math_ApproachF(&this->drawDmgEffFrozenSteamScales[bodyPartIndex], 0.3f, 0.3f, 0.5f);
            Math_Vec3f_Copy(&bodyPartsPos[0], &this->actor.world.pos);
            bodyPartsPos[0].x += this->bodyPartsPos[bodyPartIndex].x;
            bodyPartsPos[0].y += this->bodyPartsPos[bodyPartIndex].y;
            bodyPartsPos[0].z += this->bodyPartsPos[bodyPartIndex].z;
            if (drawDmgEffType == ACTOR_DRAW_DMGEFF_FROZEN_NO_SFX) {
                if ((this->unk_47C[bodyPartIndex] - this->unk_464[bodyPartIndex]) < 20) {
                    drawDmgEffType = ACTOR_DRAW_DMGEFF_FROZEN_SFX;
                }
                drawDmgEffAlpha = 1.0f;
            }
            Actor_DrawDamageEffects(play, &this->actor, bodyPartsPos, ARRAY_COUNT(bodyPartsPos), 0.3f,
                                    this->drawDmgEffFrozenSteamScales[bodyPartIndex], drawDmgEffAlpha, drawDmgEffType);
            ret = 1;
        }
    }
    return ret;
}

void func_808D8ED0(EnSw* this, PlayState* play) {
    Vec3f sp54;
    s32 i;

    for (i = 0; i < ENSW_BODYPART_MAX; i++) {
        Math_Vec3f_Copy(&sp54, &this->actor.world.pos);
        sp54.x += this->bodyPartsPos[i].x;
        sp54.y += this->bodyPartsPos[i].y;
        sp54.z += this->bodyPartsPos[i].z;
        Math_Vec3f_Copy(&this->bodyPartsPos[i], &sp54);
    }
    Actor_SpawnIceEffects(play, &this->actor, this->bodyPartsPos, ENSW_BODYPART_MAX, 3, 0.1f, 0.3f);
}

void func_808D8FC4(EnSw* this, PlayState* play) {
    Math_Vec3f_ToVec3s(&this->collider.dim.worldSphere.center, &this->actor.world.pos);
    this->collider.dim.worldSphere.radius = this->collider.dim.modelSphere.radius * this->collider.dim.scale;
    if (!(this->unk_410 & 0x10) && (this->actor.colChkInfo.health != 0) && (DECR(this->unk_458) <= 10)) {
        CollisionCheck_SetAC(play, &play->colChkCtx, &this->collider.base);
    }

    if (!(this->unk_410 & 0x10) && (this->actor.colChkInfo.health != 0)) {
        CollisionCheck_SetAT(play, &play->colChkCtx, &this->collider.base);
    }

    if (this->unk_410 & 4) {
        CollisionCheck_SetOC(play, &play->colChkCtx, &this->collider.base);
    }
}

s32 func_808D90C4(EnSw* this) {
    if (this->actor.colChkInfo.damage < this->actor.colChkInfo.health) {
        this->actor.colChkInfo.health -= this->actor.colChkInfo.damage;
    } else {
        this->actor.colChkInfo.health = 0;
    }
    return this->actor.colChkInfo.health;
}

void func_808D90F0(EnSw* this, s32 arg1, s16 arg2) {
    Vec3f sp2C;
    s16 sp2A;
    s16 temp;

    if (arg1 == 1) {
        sp2A = this->unk_494;
        Math_ApproachS(&this->unk_494, this->unk_496, 4, 0xE38);
        temp = this->unk_494 - sp2A;
    } else {
        temp = arg2;
    }

    Matrix_RotateAxisF(BINANG_TO_RAD_ALT(temp), &this->unk_368, MTXMODE_NEW);
    Matrix_MultVec3f(&this->unk_350, &sp2C);
    Math_Vec3f_Copy(&this->unk_350, &sp2C);
    Math3D_Vec3f_Cross(&this->unk_368, &this->unk_350, &this->unk_35C);
}

s32 func_808D91C4(EnSw* this, CollisionPoly* floorPoly) {
    f32 sp4C;
    f32 temp_f12;
    f32 temp_f0;
    Vec3f sp38;
    Vec3f sp2C;

    this->actor.floorPoly = floorPoly;

    if (floorPoly != NULL) {
        sp38.x = COLPOLY_GET_NORMAL(floorPoly->normal.x);
        sp38.y = COLPOLY_GET_NORMAL(floorPoly->normal.y);
        sp38.z = COLPOLY_GET_NORMAL(floorPoly->normal.z);
    } else {
        return false;
    }

    temp_f12 = DOTXYZ(sp38, this->unk_368);
    if (fabsf(temp_f12) >= 0.999f) {
        return false;
    }

    sp4C = Math_FAcosF(temp_f12);
    if (sp4C < 0.001f) {
        return false;
    }

    Math3D_Vec3f_Cross(&this->unk_368, &sp38, &sp2C);
    temp_f0 = Math3D_Vec3fMagnitude(&sp2C);
    if (temp_f0 < 0.001f) {
        return false;
    }

    Math_Vec3f_Scale(&sp2C, 1.0f / temp_f0);
    Matrix_RotateAxisF(sp4C, &sp2C, MTXMODE_NEW);
    Matrix_MultVec3f(&this->unk_35C, &sp2C);
    Math_Vec3f_Copy(&this->unk_35C, &sp2C);
    Math3D_Vec3f_Cross(&this->unk_35C, &sp38, &this->unk_350);

    temp_f0 = Math3D_Vec3fMagnitude(&this->unk_350);
    if (temp_f0 < 0.001f) {
        return false;
    }

    Math_Vec3f_Scale(&this->unk_350, 1.0f / temp_f0);
    Math_Vec3f_Copy(&this->unk_368, &sp38);

    return true;
}

void func_808D93BC(EnSw* this) {
    MtxF sp18;

    sp18.xx = this->unk_35C.x;
    sp18.yx = this->unk_35C.y;
    sp18.zx = this->unk_35C.z;
    sp18.xy = this->unk_368.x;
    sp18.yy = this->unk_368.y;
    sp18.zy = this->unk_368.z;
    sp18.xz = this->unk_350.x;
    sp18.yz = this->unk_350.y;
    sp18.zz = this->unk_350.z;
    Matrix_MtxFToYXZRot(&sp18, &this->actor.world.rot, false);
    this->actor.world.rot.x = -this->actor.world.rot.x;
}

s32 func_808D9440(PlayState* play, Vec3f* posA, Vec3f* posB, Vec3f* posResult, CollisionPoly** outPoly, s32* bgId) {
    s32 ret = false;

    if (BgCheck_EntityLineTest1(&play->colCtx, posA, posB, posResult, outPoly, true, true, true, true, bgId) &&
        !(SurfaceType_GetWallFlags(&play->colCtx, *outPoly, *bgId) & (WALL_FLAG_4 | WALL_FLAG_5))) {
        ret = true;
    }
    return ret;
}

void func_808D94D0(EnSw* this, PlayState* play, s32 arg2, s32 arg3, s16 arg4) {
    CollisionPoly* spA4;
    CollisionPoly* spA0;
    s32 pad;
    Vec3f sp90;
    Vec3f sp84;
    Vec3f sp78;
    Vec3f sp6C;
    s32 sp68;
    s32 sp64;
    Actor* thisx = &this->actor;
    f32 temp_f20;
    s32 i;

    spA4 = NULL;
    spA0 = NULL;
    sp68 = BGCHECK_SCENE;
    sp64 = BGCHECK_SCENE;
    func_808D90F0(this, arg3, arg4);
    this->actor.speed = this->unk_44C;
    temp_f20 = thisx->speed * 2.0f;

    sp90.x = this->actor.world.pos.x + (this->unk_368.x * 2.0f);
    sp90.y = this->actor.world.pos.y + (this->unk_368.y * 2.0f);
    sp90.z = this->actor.world.pos.z + (this->unk_368.z * 2.0f);

    sp84.x = this->actor.world.pos.x - (this->unk_368.x * 4.0f);
    sp84.y = this->actor.world.pos.y - (this->unk_368.y * 4.0f);
    sp84.z = this->actor.world.pos.z - (this->unk_368.z * 4.0f);

    if (func_808D9440(play, &sp90, &sp84, &sp6C, &spA0, &sp68)) {
        sp84.x = (this->unk_350.x * temp_f20) + sp90.x;
        sp84.y = (this->unk_350.y * temp_f20) + sp90.y;
        sp84.z = (this->unk_350.z * temp_f20) + sp90.z;
        if (func_808D9440(play, &sp90, &sp84, &sp78, &spA4, &sp64)) {
            func_808D91C4(this, spA4);
            Math_Vec3f_Copy(&this->actor.world.pos, &sp78);
            this->actor.floorBgId = sp64;
            this->actor.speed = 0.0f;
        } else {
            if (spA0 != this->actor.floorPoly) {
                func_808D91C4(this, spA0);
            }
            Math_Vec3f_Copy(&this->actor.world.pos, &sp6C);
            this->actor.floorBgId = sp68;
        }
    } else {
        this->actor.speed = 0.0f;
        temp_f20 *= 3.0f;
        Math_Vec3f_Copy(&sp90, &sp84);

        for (i = 0; i < 3; i++) {
            if (i == 0) {
                sp84.x = sp90.x - (this->unk_350.x * temp_f20);
                sp84.y = sp90.y - (this->unk_350.y * temp_f20);
                sp84.z = sp90.z - (this->unk_350.z * temp_f20);
            } else if (i == 1) {
                sp84.x = sp90.x + (this->unk_35C.x * temp_f20);
                sp84.y = sp90.y + (this->unk_35C.y * temp_f20);
                sp84.z = sp90.z + (this->unk_35C.z * temp_f20);
            } else {
                sp84.x = sp90.x - (this->unk_35C.x * temp_f20);
                sp84.y = sp90.y - (this->unk_35C.y * temp_f20);
                sp84.z = sp90.z - (this->unk_35C.z * temp_f20);
            }

            if (func_808D9440(play, &sp90, &sp84, &sp78, &spA4, &sp64)) {
                func_808D91C4(this, spA4);
                Math_Vec3f_Copy(&this->actor.world.pos, &sp78);
                this->actor.floorBgId = sp64;
                break;
            }
        }

        //! FAKE
        if (i == 3) {}
    }

    func_808D93BC(this);
    this->actor.shape.rot.x = -this->actor.world.rot.x;
    this->actor.shape.rot.y = this->actor.world.rot.y;
    this->actor.shape.rot.z = this->actor.world.rot.z;

    if (this->actor.speed != 0.0f) {
        this->unk_44C = this->actor.speed;
    }

    if (arg2 == 1) {
        Actor_MoveWithoutGravity(&this->actor);
    }
}

void func_808D9894(EnSw* this, Vec3f* vec) {
    Vec3f sp5C;
    MtxF sp1C;

    sp1C.xx = this->unk_35C.x;
    sp1C.xy = this->unk_35C.y;
    sp1C.xz = this->unk_35C.z;
    sp1C.xw = 0.0f;

    sp1C.yx = this->unk_368.x;
    sp1C.yy = this->unk_368.y;
    sp1C.yz = this->unk_368.z;
    sp1C.yw = 0.0f;

    sp1C.zx = this->unk_350.x;
    sp1C.zy = this->unk_350.y;
    sp1C.zz = this->unk_350.z;
    sp1C.zw = 0.0f;

    sp1C.wx = 0.0f;
    sp1C.wy = 0.0f;
    sp1C.wz = 0.0f;
    sp1C.ww = 0.0f;

    Matrix_Put(&sp1C);
    sp5C.x = vec->x - this->actor.world.pos.x;
    sp5C.y = vec->y - this->actor.world.pos.y;
    sp5C.z = vec->z - this->actor.world.pos.z;
    Matrix_MultVec3f(&sp5C, vec);
}

s32 func_808D9968(EnSw* this, PlayState* play) {
    s32 ret = false;
    s32 param = ENSW_GET_3FC(&this->actor);

    if (ENSW_GET_3(&this->actor)) {
        if ((param != 0x3F) && Flags_GetTreasure(play, param)) {
            ret = true;
        }
    }

    return ret;
}

s32 func_808D99C8(EnSw* this, PlayState* play) {
    s32 phi_v1 = 0;

    if (ENSW_GET_3(&this->actor) || (this->unk_458 > 10) || (this->actor.colChkInfo.health == 0)) {
        return false;
    }

    if ((this->actor.xyzDistToPlayerSq < ((sREG(16) * 10) + 60000)) && (play->actorCtx.playerImpact.timer != 0) &&
        (play->actorCtx.playerImpact.type == PLAYER_IMPACT_GORON_GROUND_POUND)) {
        this->actor.colChkInfo.damage = 4;
        phi_v1 = true;
    }

    return phi_v1;
}

s32 func_808D9A70(EnSw* this, PlayState* play) {
    s16 sp2E = 0;
    f32 frame;

    if (DECR(this->unk_454) == 0) {
        if (!Animation_OnFrame(&this->skelAnime, this->skelAnime.endFrame)) {
            frame = this->skelAnime.endFrame - this->skelAnime.curFrame;
            sp2E = (80.0f * frame) * this->unk_450;
        } else {
            if (this->unk_456 != 0) {
                if (!ENSW_GET_3(&this->actor)) {
                    Actor_PlaySfx(&this->actor, NA_SE_EN_STALWALL_ROLL);
                } else {
                    Actor_PlaySfx(&this->actor, NA_SE_EN_STALGOLD_ROLL);
                }
                this->unk_456--;
                this->skelAnime.curFrame = 0.0f;
            } else {
                this->unk_454 = Rand_S16Offset(20, 20);
                this->unk_456 = (Rand_ZeroOne() * 10.0f) + 3.0f;
            }

            if (this->unk_456 % 2) {
                if (Rand_ZeroOne() < 0.5f) {
                    this->unk_450 = -1.0f;
                } else {
                    this->unk_450 = 1.0f;
                }
            }
        }
        func_808D94D0(this, play, 0, 0, sp2E);
    }

    return false;
}

s32 func_808D9C18(EnSw* this) {
    Vec3f sp3C;
    Vec3f sp30;
    f32 new_var;

    if ((this->unk_498 == 0xF9) || (this->unk_498 == 0x82) || (this->unk_498 == 0xE4) || (this->unk_498 == 0xE5)) {
        this->actor.velocity.x = this->actor.speed;
        this->actor.velocity.z = this->actor.speed;
        this->actor.velocity.x *= Math_SinS(this->actor.world.rot.y);
        this->actor.velocity.z *= Math_CosS(this->actor.world.rot.y);
    } else {
        new_var = this->actor.speed * this->unk_350.x;
        this->actor.velocity.x = new_var + (this->actor.speed * this->unk_368.x);
        new_var = this->actor.speed * this->unk_350.z;
        this->actor.velocity.z = new_var + this->actor.speed * this->unk_368.z;
        this->actor.velocity.y = 14.0f;
        Math_Vec3f_Copy(&sp3C, &this->actor.world.pos);
        Math_Vec3f_Copy(&sp30, &this->actor.world.pos);
        sp30.x += this->actor.velocity.x;
        sp30.y += this->actor.velocity.y;
        sp30.z += this->actor.velocity.z;
        this->actor.world.rot.x = 0;
        this->actor.world.rot.z = 0;
        this->actor.world.rot.y = Math_Vec3f_Yaw(&sp3C, &sp30);
    }

    Actor_PlaySfx(&this->actor, NA_SE_EN_STALTURA_APPEAR);

    if (ENSW_GET_3(&this->actor) == 1) {
        Actor_SetScale(&this->actor, 0.0f);
        this->actor.world.rot.x = 0;
        this->actor.world.rot.z = 0;
        Math_Vec3s_Copy(&this->actor.shape.rot, &this->actor.world.rot);
        this->unk_410 |= 4;
    }

    this->actor.gravity = -1.4f;
    this->actor.parent = NULL;
    return false;
}

void func_808D9DA0(EnSw* this) {
    static Vec3f D_808DBAAC = { 0.0f, 1.0f, 0.0f };
    s32 pad;
    MtxF sp24;

    SkinMatrix_SetRotateYRP(&sp24, this->actor.shape.rot.x, this->actor.shape.rot.y, this->actor.shape.rot.z);
    SkinMatrix_Vec3fMtxFMultXYZ(&sp24, &D_808DBAAC, &this->unk_368);
    this->unk_350.x = Math_SinS(this->actor.shape.rot.y);
    this->unk_350.y = 0.0f;
    this->unk_350.z = Math_CosS(this->actor.shape.rot.y);
    this->unk_35C.x = Math_SinS(BINANG_ADD(this->actor.shape.rot.y, 0x4000));
    this->unk_35C.y = 0.0f;
    this->unk_35C.z = Math_CosS(BINANG_ADD(this->actor.shape.rot.y, 0x4000));
}

void func_808D9E44(EnSw* this) {
    Vec3f sp1C;

    this->actor.shape.rot.z = -0x8000;
    this->actor.shape.rot.x = 0;
    this->actor.shape.rot.y = 0;
    Math_Vec3s_Copy(&this->actor.world.rot, &this->actor.shape.rot);
    this->actor.gravity = -1.4f;
    this->actor.velocity.y = 0.0f;
    this->unk_456 = 2;
    Math_Vec3f_Copy(&sp1C, &this->actor.world.pos);
    sp1C.x += this->unk_368.x * 16.0f;
    sp1C.y += this->unk_368.y * 16.0f;
    sp1C.z += this->unk_368.z * 16.0f;
    Math_Vec3f_Copy(&this->actor.world.pos, &sp1C);
}

void func_808D9F08(EnSw* this) {
    this->unk_454 = Rand_S16Offset(20, 20);
    this->unk_456 = (Rand_ZeroOne() * 10.0f) + 3.0f;
    this->unk_414 = 0.0f;
    this->unk_45E = this->unk_460;
}

void func_808D9F78(EnSw* this, PlayState* play, s32 arg2) {
    if (arg2) {
        Actor_ChangeCategory(play, &play->actorCtx, &this->actor, ACTORCAT_ENEMY);
    }
    Actor_SetScale(&this->actor, 0.02f);
    func_808D9DA0(this);
    this->actor.speed = 10.0f;
    this->unk_44C = 10.0f;
    func_808D94D0(this, play, 0, 0, 0);
    this->actor.speed = 0.0f;
    this->unk_44C = 0.0f;
    this->unk_450 = 1.0f;
    Math_Vec3f_Copy(&this->actor.home.pos, &this->actor.world.pos);
    this->unk_410 |= 4;
}

void func_808DA024(EnSw* this, PlayState* play) {
    func_808D9DA0(this);
    this->actor.speed = 10.0f;
    this->unk_44C = 10.0f;
    func_808D94D0(this, play, 0, 0, 0);
    this->actor.speed = 0.0f;
    this->unk_44C = 0.0f;
    this->unk_450 = 1.0f;
}

s32 func_808DA08C(EnSw* this, PlayState* play) {
    s32 ret = 0;
    s32 i;

    if (func_808D99C8(this, play) || (this->collider.base.acFlags & AC_HIT)) {
        this->collider.base.acFlags &= ~AC_HIT;

        if (this->actor.colChkInfo.damageEffect == 4) {
            Actor_Spawn(&play->actorCtx, play, ACTOR_EN_CLEAR_TAG, this->actor.world.pos.x, this->actor.world.pos.y,
                        this->actor.world.pos.z, 0, 0, 0, CLEAR_TAG_PARAMS(CLEAR_TAG_LARGE_LIGHT_RAYS));
        }

        if (this->drawDmgEffType == ACTOR_DRAW_DMGEFF_FROZEN_NO_SFX) {
            // clang-format off
            for (i = 0; i < ARRAY_COUNT(this->unk_464); i++) { this->unk_464[i] = 0; }
            // clang-format on

            this->unk_45C = 0;
        } else if (!func_808D90C4(this)) {
            SoundSource_PlaySfxAtFixedWorldPos(play, &this->actor.world.pos, 40, NA_SE_EN_STALTU_DEAD);
            Enemy_StartFinishingBlow(play, &this->actor);
            this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
            if (!ENSW_GET_3(&this->actor)) {
                SubS_ChangeAnimationByInfoS(&this->skelAnime, sAnimationInfo, ENSW_ANIM_3);
            }

            switch (this->actor.colChkInfo.damageEffect) {
                case 4:
                    this->drawDmgEffType = ACTOR_DRAW_DMGEFF_LIGHT_ORBS;
                    this->unk_45C = 20;
                    func_808D8B58(this);
                    break;

                case 3:
                    this->drawDmgEffType = ACTOR_DRAW_DMGEFF_FROZEN_NO_SFX;
                    this->unk_45C = 0;
                    func_808D8B58(this);
                    break;

                case 2:
                    this->drawDmgEffType = ACTOR_DRAW_DMGEFF_FIRE;
                    this->unk_45C = 20;
                    func_808D8B58(this);
                    break;

                case 5:
                    this->drawDmgEffType = ACTOR_DRAW_DMGEFF_ELECTRIC_SPARKS_SMALL;
                    this->unk_45C = 20;
                    func_808D8B58(this);
                    break;

                default:
                    this->drawDmgEffType = ACTOR_DRAW_DMGEFF_BLUE_FIRE;
                    this->unk_45C = 0;
                    break;
            }

            if (!ENSW_GET_3(&this->actor) && (this->drawDmgEffType != ACTOR_DRAW_DMGEFF_FROZEN_NO_SFX)) {
                func_808D9E44(this);
            }
            this->unk_458 = 20;
            this->unk_45A = 0;
            Actor_SetColorFilter(&this->actor, COLORFILTER_COLORFLAG_RED, 200, COLORFILTER_BUFFLAG_OPA, this->unk_458);
            ret = true;
        } else if (this->actor.colChkInfo.damageEffect == 1) {
            if (this->unk_45A == 0) {
                Actor_PlaySfx(&this->actor, NA_SE_EN_COMMON_FREEZE);
                this->unk_45A = 40;
                Actor_SetColorFilter(&this->actor, COLORFILTER_COLORFLAG_BLUE, 200, COLORFILTER_BUFFLAG_OPA,
                                     this->unk_45A);
            }
        } else {
            Actor_PlaySfx(&this->actor, NA_SE_EN_STALTU_DAMAGE);
            this->unk_458 = 20;
            this->unk_45A = 0;
            Actor_SetColorFilter(&this->actor, COLORFILTER_COLORFLAG_RED, 200, COLORFILTER_BUFFLAG_OPA, this->unk_458);
        }
    }
    return ret;
}

void func_808DA350(EnSw* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    if ((player->stateFlags1 & PLAYER_STATE1_200000) && (this->actor.xyzDistToPlayerSq < 8000.0f)) {
        Actor_PlaySfx(&this->actor, NA_SE_EN_STALWALL_LAUGH);
        Math_Vec3f_Copy(&this->unk_374, &player->actor.world.pos);
        this->unk_410 &= ~0x20;
        this->unk_414 = 0.0f;
        this->actionFunc = func_808DA3F4;
    } else {
        func_808D9A70(this, play);
    }
}

void func_808DA3F4(EnSw* this, PlayState* play) {
    s16 temp_v0;
    s16 temp_s1 = 0;
    Vec3f sp38;
    f32 temp_f16;

    if (!Animation_OnFrame(&this->skelAnime, this->skelAnime.endFrame)) {
        temp_f16 = this->skelAnime.endFrame - this->skelAnime.curFrame;
        temp_s1 = TRUNCF_BINANG(80.0f * temp_f16);
        Math_Vec3f_Copy(&sp38, &this->unk_374);
        func_808D9894(this, &sp38);

        temp_v0 = Math_Atan2S_XY(sp38.z, sp38.x);
        if (ABS_ALT(temp_v0) < temp_s1) {
            this->skelAnime.curFrame = 0.0f;
            Actor_PlaySfx(&this->actor, NA_SE_EN_STALWALL_DASH);
            this->unk_414 = 0.0f;
            if (this->unk_410 & 0x20) {
                this->actionFunc = func_808DA6FC;
            } else {
                this->actionFunc = func_808DA578;
            }
            temp_s1 = ABS_ALT(temp_v0);
        }
        temp_s1 *= (temp_v0 < 0) ? -1 : 1;
    } else {
        Actor_PlaySfx(&this->actor, NA_SE_EN_STALWALL_ROLL);
        this->skelAnime.curFrame = 0.0f;
    }
    func_808D94D0(this, play, 0, 0, temp_s1);
}

void func_808DA578(EnSw* this, PlayState* play) {
    f32 temp_f0;
    f32 temp;
    s16 temp2;
    Vec3f sp30;

    if (!Animation_OnFrame(&this->skelAnime, this->skelAnime.endFrame)) {
        temp_f0 = this->skelAnime.endFrame - this->skelAnime.curFrame;
        this->unk_44C = 0.3f * temp_f0;
        func_808D94D0(this, play, 1, 0, 0);
        if ((this->actor.speed == 0.0f) && (this->unk_44C != 0.0f)) {
            Math_Vec3f_Copy(&sp30, &this->unk_374);
            func_808D9894(this, &sp30);
            temp2 = Math_Atan2S_XY(sp30.z, sp30.x);
            func_808D94D0(this, play, 0, 0, temp2);
        }
    } else if (this->unk_410 & 0x20) {
        Math_Vec3f_Copy(&this->unk_374, &this->actor.home.pos);
        this->actionFunc = func_808DA3F4;
    } else {
        Actor_PlaySfx(&this->actor, NA_SE_EN_STALWALL_DASH);
        this->skelAnime.curFrame = 0.0f;
    }

    temp = Math_Vec3f_DistXYZ(&this->actor.world.pos, &this->unk_374);
    if (!(this->unk_410 & 0x20) && ((s32)this->unk_414 != 0) && ((s32)this->unk_414 < (s32)temp)) {
        this->unk_410 |= 0x20;
    }
    this->unk_414 = temp;
}

void func_808DA6FC(EnSw* this, PlayState* play) {
    f32 sp4C;
    f32 temp_f0;
    s16 temp2;
    Vec3f sp38;

    if (!Animation_OnFrame(&this->skelAnime, this->skelAnime.endFrame)) {
        temp_f0 = this->skelAnime.endFrame - this->skelAnime.curFrame;
        this->unk_44C = 0.14f * temp_f0;
        func_808D94D0(this, play, 1, 0, 0);
        if ((this->actor.speed == 0.0f) && (this->unk_44C != 0.0f)) {
            Math_Vec3f_Copy(&sp38, &this->unk_374);
            func_808D9894(this, &sp38);
            temp2 = Math_Atan2S_XY(sp38.z, sp38.x);
            func_808D94D0(this, play, 0, 0, temp2);
        }
    } else {
        Actor_PlaySfx(&this->actor, NA_SE_EN_STALWALL_DASH);
        this->skelAnime.curFrame = 0.0f;
    }

    sp4C = Math_Vec3f_DistXYZ(&this->actor.world.pos, &this->unk_374);

    if (((s32)this->unk_414 != 0) && ((s32)this->unk_414 < (s32)sp4C)) {
        Math_Vec3f_Copy(&this->actor.world.pos, &this->unk_374);
        this->unk_454 = Rand_S16Offset(20, 20);
        this->unk_456 = (Rand_ZeroOne() * 10.0f) + 3.0f;
        this->actionFunc = func_808DA350;
        this->skelAnime.curFrame = 0.0f;
    }
    this->unk_414 = sp4C;
}

void func_808DA89C(EnSw* this, PlayState* play) {
    if (this->drawDmgEffType == ACTOR_DRAW_DMGEFF_FROZEN_NO_SFX) {
        s32 i;
        s32 count;
        s16 phi_a0;

        for (i = 0, count = 0; i < ARRAY_COUNT(this->unk_464); i++) {
            if (this->unk_464[i] == 0) {
                phi_a0 = 0;
            } else {
                this->unk_464[i]--;
                phi_a0 = this->unk_464[i];
            }
            if (phi_a0 == 0) {
                count++;
            }
        }

        if (count == ARRAY_COUNT(this->unk_464)) {
            if (!ENSW_GET_3(&this->actor)) {
                func_808D9E44(this);
            }
            this->drawDmgEffType = ACTOR_DRAW_DMGEFF_BLUE_FIRE;
            func_808D8ED0(this, play);
        }
    } else if (ENSW_GET_3(&this->actor)) {
        this->actionFunc = func_808DAEB4;
    } else {
        if (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND) {
            f32 temp_f2;

            this->actor.shape.yOffset = 400.0f;
            temp_f2 = fabsf(this->actor.velocity.y) * 0.6f;
            this->actor.velocity.y = temp_f2;
            this->unk_448 = temp_f2;
            this->actor.speed = 0.0f;
            if ((s32)temp_f2 != 0) {
                Actor_PlaySfx(&this->actor, NA_SE_EN_STALTURA_BOUND);
            } else {
                this->actionFunc = func_808DAEB4;
                this->actor.velocity.y = 0.0f;
            }
            if ((s32)this->actor.velocity.y >= 2) {
                func_808D8940(this, play);
            }
        } else {
            Math_ApproachF(&this->actor.shape.yOffset, 400.0f, 0.3f, 1000.0f);
        }

        Actor_MoveWithGravity(&this->actor);
        Actor_UpdateBgCheckInfo(play, &this->actor, 30.0f, 12.0f, 0.0f, UPDBGCHECKINFO_FLAG_4);
    }
}

void func_808DAA60(EnSw* this, PlayState* play) {
    Vec3s* points;
    s16 temp_v0;
    s16 sp40;
    Vec3f sp34;
    f32 temp_f16;

    points = Lib_SegmentedToVirtual(this->path->points);
    sp40 = 0;

    if (DECR(this->unk_454) == 0) {
        if (Animation_OnFrame(&this->skelAnime, this->skelAnime.endFrame) == 0) {
            temp_f16 = this->skelAnime.endFrame - this->skelAnime.curFrame;
            sp40 = 80.0f * temp_f16;
            if (this->unk_45E == 0) {
                Math_Vec3s_ToVec3f(&sp34, &points[this->unk_4A0]);
                func_808D9894(this, &sp34);
                temp_v0 = Math_Atan2S_XY(sp34.z, sp34.x);
                if (ABS_ALT(temp_v0) < sp40) {
                    this->skelAnime.curFrame = 0.0f;
                    Actor_PlaySfx(&this->actor, NA_SE_EN_STALWALL_DASH);
                    Math_Vec3s_ToVec3f(&this->unk_374, &points[this->unk_4A0]);
                    this->actionFunc = func_808DACF4;
                    this->unk_414 = 0.0f;
                    sp40 = ABS_ALT(temp_v0);
                }
                sp40 *= (temp_v0 < 0) ? -1 : 1;
            }
        } else {
            if (this->unk_456 != 0) {
                Actor_PlaySfx(&this->actor, NA_SE_EN_STALGOLD_ROLL);
                this->unk_456--;
                this->skelAnime.curFrame = 0.0f;
            } else {
                this->unk_454 = Rand_S16Offset(20, 20);
                this->unk_456 = (Rand_ZeroOne() * 10.0f) + 3.0f;
                if (this->unk_45E != 0) {
                    this->unk_45E--;
                }
            }

            if (this->unk_456 % 2) {
                if (Rand_ZeroOne() < 0.5f) {
                    this->unk_450 = -1.0f;
                } else {
                    this->unk_450 = 1.0f;
                }
            }
        }
    }
    func_808D94D0(this, play, 0, 0, sp40);
}

void func_808DACF4(EnSw* this, PlayState* play) {
    f32 sp4C;
    f32 temp;
    s16 temp_f6;

    if (Animation_OnFrame(&this->skelAnime, this->skelAnime.endFrame) == 0) {
        Vec3f sp38;

        temp = this->skelAnime.endFrame - this->skelAnime.curFrame;
        this->unk_44C = 0.1f * temp;
        func_808D94D0(this, play, 1, 0, 0);
        if ((this->actor.speed == 0.0f) && (this->unk_44C != 0.0f)) {

            Math_Vec3f_Copy(&sp38, &this->unk_374);
            func_808D9894(this, &sp38);
            temp_f6 = Math_Atan2S_XY(sp38.z, sp38.x);
            func_808D94D0(this, play, 0, 0, temp_f6);
        }
    } else {
        Actor_PlaySfx(&this->actor, NA_SE_EN_STALWALL_DASH);
        this->skelAnime.curFrame = 0.0f;
    }

    sp4C = Math_Vec3f_DistXYZ(&this->actor.world.pos, &this->unk_374);

    if (((s32)this->unk_414 != 0) && ((s32)this->unk_414 < (s32)sp4C)) {
        Math_Vec3f_Copy(&this->actor.world.pos, &this->unk_374);
        this->unk_4A0 += this->unk_49C;
        if ((this->unk_4A0 >= this->path->count) || (this->unk_4A0 < 0)) {
            this->unk_49C = -this->unk_49C;
            this->unk_4A0 += this->unk_49C * 2;
        }

        if (this->unk_410 & 8) {
            func_808D9F08(this);
            this->actionFunc = func_808DAA60;
        } else {
            this->actionFunc = func_808DB100;
        }
    }
    this->unk_414 = sp4C;
}

void func_808DAEB4(EnSw* this, PlayState* play) {
    Vec3f sp5C;
    s32 i;
    s32 count;
    s16 phi_a0;

    if (this->unk_410 & 2) {
        if (!ENSW_GET_3(&this->actor)) {
            if (this->unk_410 & 4) {
                phi_a0 = DECR(this->unk_45C);
                if (phi_a0 == 0) {
                    this->unk_410 &= ~4;
                }
            }

            for (i = 0, count = 0; i < ARRAY_COUNT(this->unk_464); i++) {
                if (this->unk_464[i] == 0) {
                    phi_a0 = 0;
                } else {
                    this->unk_464[i]--;
                    phi_a0 = this->unk_464[i];
                }
                if (phi_a0 == 0) {
                    count++;
                }
            }

            if (count == ARRAY_COUNT(this->unk_464)) {
                Actor_Kill(&this->actor);
            }
        } else {
            Math_ApproachF(&this->actor.scale.x, 0.0f, 0.08f, 1.0f);
            Actor_SetScale(&this->actor, this->actor.scale.x);
            if ((s32)(this->actor.scale.x * 100.0f) == 0) {

                Math_Vec3f_Copy(&sp5C, &this->actor.world.pos);
                sp5C.x += this->unk_368.x * 10.0f;
                sp5C.y += this->unk_368.y * 10.0f;
                sp5C.z += this->unk_368.z * 10.0f;
                if (Actor_SpawnAsChild(&play->actorCtx, &this->actor, play, ACTOR_EN_SI, sp5C.x, sp5C.y, sp5C.z, 0, 0,
                                       0, this->actor.params) != NULL) {
                    Audio_PlaySfx(NA_SE_SY_KINSTA_MARK_APPEAR);
                }
                Actor_Kill(&this->actor);
            }
            func_808D94D0(this, play, 0, 0, 0x1554);
        }
    } else {
        phi_a0 = DECR(this->unk_45C);
        if (phi_a0 == 0) {
            this->unk_410 |= 2;
            if (!ENSW_GET_3(&this->actor) && (this->drawDmgEffType == ACTOR_DRAW_DMGEFF_BLUE_FIRE)) {
                func_808D8B58(this);
                this->unk_45C = 10;
            } else {
                this->unk_45C = 20;
            }
        }
    }
}

void func_808DB100(EnSw* this, PlayState* play) {
    if (this->actor.parent != NULL) {
        this->unk_498 = this->actor.parent->id;
        this->unk_456 = 0;
        this->unk_454 = 0;
        this->skelAnime.curFrame = 0.0f;
        Actor_ChangeCategory(play, &play->actorCtx, &this->actor, ACTORCAT_NPC);
        this->actionFunc = func_808DB25C;
        return;
    }

    if (!(this->unk_410 & 1)) {
        func_808D9A70(this, play);
        return;
    }

    if ((DECR(this->unk_454) == 0) && Animation_OnFrame(&this->skelAnime, this->skelAnime.endFrame)) {
        if (this->unk_456 != 0) {
            if (!ENSW_GET_3(&this->actor)) {
                Actor_PlaySfx(&this->actor, NA_SE_EN_STALWALL_ROLL);
            } else {
                Actor_PlaySfx(&this->actor, NA_SE_EN_STALGOLD_ROLL);
            }
            this->unk_456--;
            this->skelAnime.curFrame = 0.0f;
        } else {
            this->unk_454 = Rand_S16Offset(20, 20);
            this->unk_456 = (Rand_ZeroOne() * 10.0f) + 3.0f;
        }
    }
}

void func_808DB25C(EnSw* this, PlayState* play) {
    if (CutsceneManager_GetCurrentCsId() == CS_ID_GLOBAL_TALK) {
        CutsceneManager_Stop(CS_ID_GLOBAL_TALK);
    } else if (CutsceneManager_IsNext(this->actor.csId)) {
        CutsceneManager_StartWithPlayerCs(this->actor.csId, &this->actor);
        func_808D9C18(this);
        this->actionFunc = func_808DB2E0;
    } else {
        CutsceneManager_Queue(this->actor.csId);
    }
}

void func_808DB2E0(EnSw* this, PlayState* play) {
    f32 temp_f2;

    if (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND) {
        func_808D9F78(this, play, false);
        temp_f2 = fabsf(this->actor.velocity.y) * 0.6f;
        this->actor.velocity.x *= 0.5f;
        this->actor.velocity.y = temp_f2;
        this->unk_448 = temp_f2;
        this->actor.velocity.z *= 0.5f;

        if ((s32)temp_f2 != 0) {
            Actor_PlaySfx(&this->actor, NA_SE_EN_STALTURA_BOUND);
        } else {
            Actor_ChangeCategory(play, &play->actorCtx, &this->actor, ACTORCAT_ENEMY);
            Math_Vec3f_Copy(&this->actor.velocity, &gZeroVec3f);
            this->unk_410 &= ~(0x10 | 0x1);
            this->actionFunc = func_808DB100;
        }

        if ((s32)this->actor.velocity.y >= 2) {
            func_808D8940(this, play);
        }
    }

    Math_ApproachF(&this->actor.scale.x, 0.02f, 0.4f, 1.0f);
    Actor_SetScale(&this->actor, this->actor.scale.x);
    this->actor.velocity.y += this->actor.gravity;
    Actor_UpdatePos(&this->actor);
    Actor_UpdateBgCheckInfo(play, &this->actor, 30.0f, 12.0f, 0.0f, UPDBGCHECKINFO_FLAG_4);
}

void EnSw_Init(Actor* thisx, PlayState* play) {
    EnSw* this = (EnSw*)thisx;
    s32 pad;

    if (!func_808D9968(this, play)) {
        ActorShape_Init(&this->actor.shape, 0.0f, NULL, 0.0f);
        SkelAnime_Init(play, &this->skelAnime, &object_st_Skel_005298, NULL, this->jointTable, this->morphTable,
                       OBJECT_ST_LIMB_MAX);
        SubS_ChangeAnimationByInfoS(&this->skelAnime, sAnimationInfo, ENSW_ANIM_0);
        this->skelAnime.playSpeed = 4.0f;

        Collider_InitAndSetSphere(play, &this->collider, &this->actor, &sSphereInit);
        if (!ENSW_GET_3(&this->actor)) {
            this->actor.hintId = TATL_HINT_ID_SKULLWALLTULA;
            CollisionCheck_SetInfo2(&this->actor.colChkInfo, &sDamageTable, &sColChkInfoInit);
            this->collider.elem.atDmgInfo.damage = 8;
        } else {
            this->actor.hintId = TATL_HINT_ID_GOLD_SKULLTULA;
            CollisionCheck_SetInfo2(&this->actor.colChkInfo, &sDamageTable2, &sColChkInfoInit2);
            this->collider.elem.atDmgInfo.damage = 16;
        }

        this->path =
            SubS_GetDayDependentPath(play, ENSW_GET_PATH_INDEX(&this->actor), ENSW_PATH_INDEX_NONE, &this->unk_4A0);
        if (this->path != NULL) {
            this->unk_4A0 = 1;
        }

        switch (ENSW_GET_3(&this->actor)) {
            case 0:
                func_808D9F78(this, play, true);
                this->actionFunc = func_808DA350;
                break;

            case 1:
                this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
                this->actor.flags |= ACTOR_FLAG_UPDATE_CULLING_DISABLED;

                this->unk_460 = ABS_ALT(thisx->world.rot.z);

                if (this->actor.world.rot.z >= 0) {
                    this->unk_410 |= 8;
                }

                func_808DA024(this, play);
                this->unk_410 |= (0x10 | 0x1);
                this->unk_410 &= ~4;
                this->actionFunc = func_808DB100;
                break;

            case 2:
            case 3:
                this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
                this->actor.flags |= ACTOR_FLAG_UPDATE_CULLING_DISABLED;

                this->unk_460 = ABS_ALT(thisx->world.rot.z);

                if (this->actor.world.rot.z >= 0) {
                    this->unk_410 |= 8;
                }

                func_808D9F78(this, play, true);
                if (this->path != NULL) {
                    this->unk_49C = 1;
                    func_808D9F08(this);
                    this->actionFunc = func_808DAA60;
                } else {
                    this->actionFunc = func_808DB100;
                }
                break;
        }
    } else {
        Actor_Kill(&this->actor);
    }
}

void EnSw_Destroy(Actor* thisx, PlayState* play) {
    EnSw* this = (EnSw*)thisx;

    Collider_DestroySphere(play, &this->collider);
}

void EnSw_Update(Actor* thisx, PlayState* play) {
    EnSw* this = (EnSw*)thisx;

    if (func_808DA08C(this, play)) {
        this->actionFunc = func_808DA89C;
    } else if (DECR(this->unk_45A) == 0) {
        this->actionFunc(this, play);
    }

    if ((this->drawDmgEffType != ACTOR_DRAW_DMGEFF_FROZEN_NO_SFX) || (this->unk_45A != 0)) {
        SkelAnime_Update(&this->skelAnime);
    }

    Actor_SetFocus(&this->actor, 0.0f);
    func_808D8FC4(this, play);
}

s32 EnSw_OverrideLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, Actor* thisx) {
    EnSw* this = (EnSw*)thisx;

    if (ENSW_GET_3(&this->actor)) {
        switch (limbIndex) {
            case OBJECT_ST_LIMB_17:
                *dList = object_st_DL_004788;
                break;

            case OBJECT_ST_LIMB_08:
                *dList = object_st_DL_0046F0;
                break;

            case OBJECT_ST_LIMB_0E:
                *dList = object_st_DL_004658;
                break;

            case OBJECT_ST_LIMB_0B:
                *dList = object_st_DL_0045C0;
                break;

            case OBJECT_ST_LIMB_1A:
                *dList = object_st_DL_004820;
                break;

            case OBJECT_ST_LIMB_14:
                *dList = object_st_DL_0048B8;
                break;

            case OBJECT_ST_LIMB_11:
                *dList = object_st_DL_004950;
                break;

            case OBJECT_ST_LIMB_1D:
                *dList = object_st_DL_0049E8;
                break;

            case OBJECT_ST_LIMB_05:
                *dList = object_st_DL_003FB0;
                break;

            case OBJECT_ST_LIMB_04:
                *dList = object_st_DL_0043D8;
                break;

            default:
                break;
        }
    }
    return false;
}

void EnSw_Draw(Actor* thisx, PlayState* play) {
    EnSw* this = (EnSw*)thisx;
    s32 bodyPartIndex;
    s32 count;

    if (this->unk_410 & 4) {
        if (ENSW_GET_3(&this->actor)) {
            func_800B8050(&this->actor, play, 0);
        }
        Gfx_SetupDL25_Opa(play->state.gfxCtx);
        Matrix_RotateXS(-0x3C72, MTXMODE_APPLY);
        SkelAnime_DrawOpa(play, this->skelAnime.skeleton, this->skelAnime.jointTable, EnSw_OverrideLimbDraw, NULL,
                          &this->actor);
    }

    for (bodyPartIndex = 0, count = 0; bodyPartIndex < ENSW_BODYPART_MAX; bodyPartIndex++) {
        count += func_808D8D60(this, play, bodyPartIndex);
    }

    if (count != 0) {
        this->unk_462 += 3;
    }
}
