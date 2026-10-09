/*
 * File: z_en_st.c
 * Overlay: ovl_En_St
 * Description: Skulltula (large suspended one)
 */

#include "z_en_st.h"
#include "overlays/actors/ovl_En_Clear_Tag/z_en_clear_tag.h"
#include "objects/gameplay_keep/gameplay_keep.h"

#define FLAGS                                                                                 \
    (ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_HOSTILE | ACTOR_FLAG_UPDATE_CULLING_DISABLED | \
     ACTOR_FLAG_DRAW_CULLING_DISABLED | ACTOR_FLAG_CAN_ATTACH_TO_ARROW | ACTOR_FLAG_SFX_FOR_PLAYER_BODY_HIT)

void EnSt_Init(Actor* thisx, PlayState* play);
void EnSt_Destroy(Actor* thisx, PlayState* play);
void EnSt_Update(Actor* thisx, PlayState* play);

void func_808A6A78(EnSt* this, PlayState* play);
void func_808A6C04(EnSt* this, PlayState* play);
void func_808A6D84(EnSt* this, PlayState* play);
void func_808A6E24(EnSt* this, PlayState* play);
void func_808A701C(EnSt* this, PlayState* play);
void func_808A7478(Actor* thisx, PlayState* play);

ActorProfile En_St_Profile = {
    /**/ ACTOR_EN_ST,
    /**/ ACTORCAT_ENEMY,
    /**/ FLAGS,
    /**/ OBJECT_ST,
    /**/ sizeof(EnSt),
    /**/ EnSt_Init,
    /**/ EnSt_Destroy,
    /**/ EnSt_Update,
    /**/ NULL,
};

static ColliderCylinderInit sCylinderInit1 = {
    {
        COL_MATERIAL_HIT2,
        AT_ON | AT_TYPE_ENEMY,
        AC_NONE,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_1,
        COLSHAPE_CYLINDER,
    },
    {
        ELEM_MATERIAL_UNK0,
        { 0xF7CFFFFF, 0x00, 0x00 },
        { 0x800C3829, 0x00, 0x00 },
        ATELEM_ON | ATELEM_SFX_NORMAL,
        ACELEM_NONE,
        OCELEM_ON,
    },
    { 18, 48, -20, { 0, 0, 0 } },
};

static ColliderCylinderInit sCylinderInit2 = {
    {
        COL_MATERIAL_HIT2,
        AT_NONE,
        AC_ON | AC_TYPE_PLAYER,
        OC1_NONE,
        OC2_TYPE_1,
        COLSHAPE_CYLINDER,
    },
    {
        ELEM_MATERIAL_UNK0,
        { 0x00000000, 0x00, 0x00 },
        { 0x77C3C7D6, 0x00, 0x00 },
        ATELEM_NONE | ATELEM_SFX_NORMAL,
        ACELEM_ON,
        OCELEM_NONE,
    },
    { 18, 48, -20, { 0, 0, 0 } },
};

static ColliderCylinderInit sCylinderInit3 = {
    {
        COL_MATERIAL_METAL,
        AT_NONE,
        AC_ON | AC_HARD | AC_TYPE_PLAYER,
        OC1_NONE,
        OC2_TYPE_1,
        COLSHAPE_CYLINDER,
    },
    {
        ELEM_MATERIAL_UNK2,
        { 0x00000000, 0x00, 0x00 },
        { 0x77C3C7D6, 0x00, 0x00 },
        ATELEM_NONE | ATELEM_SFX_NORMAL,
        ACELEM_ON,
        OCELEM_NONE,
    },
    { 18, 48, -20, { 0, 0, 0 } },
};

static ColliderCylinderInit sCylinderInit4 = {
    {
        COL_MATERIAL_HIT2,
        AT_NONE,
        AC_ON | AC_TYPE_PLAYER,
        OC1_NONE,
        OC2_TYPE_1,
        COLSHAPE_CYLINDER,
    },
    {
        ELEM_MATERIAL_UNK0,
        { 0x00000000, 0x00, 0x00 },
        { 0x800C3829, 0x00, 0x00 },
        ATELEM_NONE | ATELEM_SFX_NORMAL,
        ACELEM_ON,
        OCELEM_NONE,
    },
    { 18, 48, -20, { 0, 0, 0 } },
};

static CollisionCheckInfoInit2 sColChkInfoInit = { 2, 0, 0, 0, MASS_IMMOVABLE };

static DamageTable sDamageTable = {
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
    /* Goron pound    */ DMG_ENTRY(0, 0x0),
    /* Fire arrow     */ DMG_ENTRY(2, 0x2),
    /* Ice arrow      */ DMG_ENTRY(2, 0x3),
    /* Light arrow    */ DMG_ENTRY(2, 0x4),
    /* Goron spikes   */ DMG_ENTRY(1, 0x0),
    /* Deku spin      */ DMG_ENTRY(0, 0x0),
    /* Deku bubble    */ DMG_ENTRY(1, 0x0),
    /* Deku launch    */ DMG_ENTRY(2, 0x0),
    /* UNK_DMG_0x12   */ DMG_ENTRY(0, 0x1),
    /* Zora barrier   */ DMG_ENTRY(0, 0x5),
    /* Normal shield  */ DMG_ENTRY(1, 0x0),
    /* Light ray      */ DMG_ENTRY(1, 0x0),
    /* Thrown object  */ DMG_ENTRY(0, 0x0),
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

typedef enum EnStAnimation {
    /* -1 */ ENST_ANIM_NONE = -1,
    /*  0 */ ENST_ANIM_0,
    /*  1 */ ENST_ANIM_1,
    /*  2 */ ENST_ANIM_2,
    /*  3 */ ENST_ANIM_3,
    /*  4 */ ENST_ANIM_4,
    /*  5 */ ENST_ANIM_5,
    /*  6 */ ENST_ANIM_6,
    /*  7 */ ENST_ANIM_7,
    /*  8 */ ENST_ANIM_MAX
} EnStAnimation;

static AnimationInfoS sAnimationInfo[ENST_ANIM_MAX] = {
    { &object_st_Anim_000304, 1.0f, 0, -1, ANIMMODE_LOOP, 0 },  // ENST_ANIM_0
    { &object_st_Anim_005B98, 1.0f, 0, -1, ANIMMODE_ONCE, -4 }, // ENST_ANIM_1
    { &object_st_Anim_000304, 4.0f, 0, -1, ANIMMODE_ONCE, -4 }, // ENST_ANIM_2
    { &object_st_Anim_000304, 1.0f, 0, -1, ANIMMODE_LOOP, -4 }, // ENST_ANIM_3
    { &object_st_Anim_0055A8, 1.0f, 0, -1, ANIMMODE_ONCE, -4 }, // ENST_ANIM_4
    { &object_st_Anim_000304, 8.0f, 0, -1, ANIMMODE_LOOP, -4 }, // ENST_ANIM_5
    { &object_st_Anim_000304, 6.0f, 0, -1, ANIMMODE_ONCE, -4 }, // ENST_ANIM_6
    { &object_st_Anim_005B98, 2.0f, 0, -1, ANIMMODE_LOOP, -4 }, // ENST_ANIM_7
};

void func_808A5050(EnSt* this, PlayState* play) {
    static Color_RGBA8 sPrimColor = { 170, 130, 90, 255 };
    static Color_RGBA8 sEnvColor = { 100, 60, 20, 0 };
    s32 i;
    Vec3f velocity;
    Vec3f accel;
    Vec3f pos;
    Vec3f sp94;
    s32 rand;
    s16 temp_s0 = (Rand_ZeroOne() - 0.5f) * 0x10000;

    pos.y = this->actor.floorHeight;

    for (i = 0; i < 16; i++, temp_s0 += 0xFFF) {
        rand = (Rand_ZeroOne() * 4.0f) + 8.0f;

        sp94.x = 0.0f;
        sp94.y = (Rand_ZeroOne() * 0.2f) + 0.1f;
        sp94.z = Rand_ZeroOne() + 1.0f;
        Lib_Vec3f_TranslateAndRotateY(&gZeroVec3f, temp_s0, &sp94, &accel);
        sp94.x = 0.0f;
        sp94.y = 1.0f;
        sp94.z = 5.0f;
        Lib_Vec3f_TranslateAndRotateY(&gZeroVec3f, temp_s0, &sp94, &velocity);

        pos.x = this->actor.world.pos.x + (2.0f * velocity.x);
        pos.z = this->actor.world.pos.z + (2.0f * velocity.z);

        func_800B0EB0(play, &pos, &velocity, &accel, &sPrimColor, &sEnvColor, 100, 40, rand);
    }

    pos.x = this->actor.world.pos.x;
    pos.z = this->actor.world.pos.z;
    pos.y = this->actor.floorHeight;
    EffectSsBlast_SpawnWhiteCustomScale(play, &pos, &gZeroVec3f, &gZeroVec3f, 100, 220, 8);
}

void func_808A52A8(EnSt* this, PlayState* play) {
    static Color_RGBA8 sPrimColor = { 170, 130, 90, 255 };
    static Color_RGBA8 sEnvColor = { 100, 60, 20, 0 };
    s32 pad;
    Vec3f velocity;
    Vec3f accel;
    Vec3f pos;
    Vec3f sp94;
    s32 i;
    s16 temp_s0 = (Rand_ZeroOne() - 0.5f) * 0x10000;
    s32 rand;

    pos.y = this->actor.floorHeight;

    for (i = 0; i < 8; i++, temp_s0 += 0x1FFE) {
        rand = (Rand_ZeroOne() * 4.0f) + 8.0f;

        sp94.x = 0.0f;
        sp94.y = (Rand_ZeroOne() * 0.2f) + 0.1f;
        sp94.z = Rand_ZeroOne() + 1.0f;

        Lib_Vec3f_TranslateAndRotateY(&gZeroVec3f, temp_s0, &sp94, &accel);

        sp94.x = 0.0f;
        sp94.y = 1.0f;
        sp94.z = 4.0f;

        Lib_Vec3f_TranslateAndRotateY(&gZeroVec3f, temp_s0, &sp94, &velocity);

        pos.x = this->actor.world.pos.x + (2.0f * velocity.x);
        pos.z = this->actor.world.pos.z + (2.0f * velocity.z);
        func_800B0EB0(play, &pos, &velocity, &accel, &sPrimColor, &sEnvColor, 100, 40, rand);
    }
}

void func_808A54B0(EnSt* this, PlayState* play) {
    s32 pad;
    f32 temp_f0;

    if ((this->unk_18C & 1) && (this->actor.colChkInfo.health != 0)) {
        OPEN_DISPS(play->state.gfxCtx);

        gSegments[6] = OS_K0_TO_PHYSICAL(play->objectCtx.slots[this->objectSlot].segment);

        gSPSegment(POLY_XLU_DISP++, 0x06, play->objectCtx.slots[this->objectSlot].segment);

        Gfx_SetupDL25_Xlu(play->state.gfxCtx);
        Matrix_Translate(this->actor.world.pos.x, this->actor.world.pos.y, this->actor.world.pos.z, MTXMODE_NEW);
        Matrix_RotateYS(this->actor.shape.rot.y, MTXMODE_APPLY);
        Matrix_Scale(0.06f, 0.12f, 0.06f, MTXMODE_APPLY);

        MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx);
        gSPSegment(POLY_XLU_DISP++, 0x08, Gfx_TwoTexScroll(play->state.gfxCtx, 0, 195, 0, 0x40, 0x20, 1, 215, 0, 8, 8));

        temp_f0 = (f32)this->unk_310 / 8;
        if (temp_f0 > 1.0f) {
            temp_f0 = 1.0f;
        }

        gDPSetPrimColor(POLY_XLU_DISP++, 0, 0x80, 170, 255, 255, (u8)(255 * temp_f0));
        gSPDisplayList(POLY_XLU_DISP++, gSpinAttackDiskDL);
        gSPDisplayList(POLY_XLU_DISP++, gSpinAttackCylinderDL);

        CLOSE_DISPS(play->state.gfxCtx);
    }
}

s32 func_808A576C(EnSt* this) {
    s32 i;
    s16 phi_s2 = (s16)(TRUNCF_BINANG(Rand_ZeroOne() * 1000.0f) % 12) * 0x1555;

    for (i = 0; i < ENST_BODYPART_MAX; i++, phi_s2 += 0x1555) {
        if (this->drawDmgEffType != ACTOR_DRAW_DMGEFF_FROZEN_NO_SFX) {
            this->unk_31C[i] = (Rand_ZeroOne() * 16.0f) + 8.0f;
        } else {
            this->unk_31C[i] = 80;
        }
        this->unk_334[i] = this->unk_31C[i];
        this->drawDmgEffFrozenSteamScales[i] = 0.90000004f;

        if ((this->drawDmgEffType == ACTOR_DRAW_DMGEFF_FIRE) || (this->drawDmgEffType == ACTOR_DRAW_DMGEFF_BLUE_FIRE)) {
            this->bodyPartsPos[i].y = ((Rand_ZeroOne() - 0.5f) * 40.0f) - 10.0f;
        } else {
            this->bodyPartsPos[i].y = ((Rand_ZeroOne() - 0.5f) * 30.0f) + 10.0f;
        }

        this->bodyPartsPos[i].x = Math_SinS(phi_s2) * 18.0f;
        this->bodyPartsPos[i].z = Math_CosS(phi_s2) * 18.0f;
    }

    this->unk_31A = 1;
    return false;
}

s32 func_808A5988(EnSt* this, PlayState* play, s32 bodyPartIndex) {
    s32 ret = 0;
    u8 drawDmgEffType;
    Vec3f bodyPartPos[1];
    f32 drawDmgEffAlpha;

    if (bodyPartIndex < this->unk_31A) {
        if (this->unk_31C[bodyPartIndex] != 0) {
            drawDmgEffAlpha = (f32)this->unk_31C[bodyPartIndex] / this->unk_334[bodyPartIndex];
            drawDmgEffType = this->drawDmgEffType;

            Math_ApproachF(&this->drawDmgEffFrozenSteamScales[bodyPartIndex], 0.6f, 0.3f, 0.5f);
            Math_Vec3f_Copy(&bodyPartPos[0], &this->actor.world.pos);

            bodyPartPos[0].x += this->bodyPartsPos[bodyPartIndex].x;
            bodyPartPos[0].y += this->bodyPartsPos[bodyPartIndex].y;
            bodyPartPos[0].z += this->bodyPartsPos[bodyPartIndex].z;

            if (drawDmgEffType == ACTOR_DRAW_DMGEFF_FROZEN_NO_SFX) {
                if ((this->unk_334[bodyPartIndex] - this->unk_31C[bodyPartIndex]) < 20) {
                    drawDmgEffType = ACTOR_DRAW_DMGEFF_FROZEN_SFX;
                }
                drawDmgEffAlpha = 1.0f;
            }
            Actor_DrawDamageEffects(play, &this->actor, bodyPartPos, ARRAY_COUNT(bodyPartPos), 0.6f,
                                    this->drawDmgEffFrozenSteamScales[bodyPartIndex], drawDmgEffAlpha, drawDmgEffType);
            ret = 1;
        }
    }
    return ret;
}

void func_808A5AF8(EnSt* this, PlayState* play) {
    Vec3f sp54;
    s32 i;

    for (i = 0; i < ENST_BODYPART_MAX; i++) {
        Math_Vec3f_Copy(&sp54, &this->actor.world.pos);
        sp54.x += this->bodyPartsPos[i].x;
        sp54.y += this->bodyPartsPos[i].y;
        sp54.z += this->bodyPartsPos[i].z;
        Math_Vec3f_Copy(&this->bodyPartsPos[i], &sp54);
    }
    Actor_SpawnIceEffects(play, &this->actor, this->bodyPartsPos, ENST_BODYPART_MAX, 3, 0.1f, 0.3f);
}

s16 func_808A5BEC(EnSt* this) {
    s16 ret;

    if (!(this->unk_18C & 4) || (this->actor.xzDistToPlayer > 180.0f)) {
        ret = this->actor.home.rot.y;
    } else {
        ret = this->actor.yawTowardsPlayer;
        if (DECR(this->unk_30E) == 0) {
            Actor_PlaySfx(&this->actor, NA_SE_EN_STALTU_ROLL);
            this->unk_18C ^= 2;
            this->unk_310 = 8;
            if (this->unk_18C & 1) {
                this->unk_310 *= 2;
            }
            this->unk_30E = 30;
        }

        if (this->unk_18C & 2) {
            ret = BINANG_ROT180(ret);
        }
    }

    return ret;
}

void func_808A5CCC(EnSt* this) {
    if (this->unk_310 != 0) {
        this->unk_310--;
    }

    if (this->unk_310 < 8) {
        Math_ApproachS(&this->actor.world.rot.y, func_808A5BEC(this), 3, 0x1FFE);
    } else {
        this->actor.world.rot.y += 0x2AA8;
    }
    this->actor.shape.rot.y = this->actor.world.rot.y;

    if (DECR(this->unk_310) == 0) {
        this->unk_18C &= ~1;
    }
}

void func_808A5D7C(EnSt* this) {
    if (this->unk_2C8 > 0.0f) {
        this->actor.world.pos.y = this->actor.home.pos.y - 10.0f;
    } else {
        this->actor.world.pos.y = this->actor.floorHeight + 20.0f;
    }
}

void func_808A5DCC(EnSt* this, Color_RGB8* colour) {
    if (this->unk_2D8 == 0.0f) {
        Math_SmoothStepToF(&this->unk_34C, 0.0f, 0.3f, 1000.0f, 0.0f);
    } else {
        this->unk_34C += this->unk_2D8;
        if (this->unk_34C >= 255.0f) {
            this->unk_34C = 255.0f;
            this->unk_2D8 *= -1.0f;
        }

        if (this->unk_34C <= 0.0f) {
            this->unk_34C = 0.0f;
            this->unk_2D8 *= -1.0f;
        }
    }

    colour->r = this->unk_34C;
    colour->g = 0;
    colour->b = 0;
}

s32 func_808A5F28(EnSt* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    s32 ret = false;

    if (Player_GetMask(play) == PLAYER_MASK_STONE) {
        if (this->unk_2C8 <= 0.0f) {
            return false;
        }
        return true;
    }

    if ((((this->unk_2C8 > 0.0f) && (this->actor.world.pos.y < player->actor.world.pos.y)) ||
         ((this->unk_2C8 > 0.0f) && (this->actor.xzDistToPlayer > 180.0f)) ||
         ((this->unk_2C8 < 0.0f) && (player->actor.world.pos.y <= this->actor.world.pos.y) &&
          (this->actor.floorHeight <= player->actor.world.pos.y) && (this->actor.xzDistToPlayer < 120.0f))) &&
        !(this->unk_18C & 8)) {
        ret = true;
    }

    return ret;
}

s32 func_808A6064(EnSt* this) {
    s32 ret = false;

    if (this->unk_2C8 > 0.0f) {
        if ((this->actor.home.pos.y - 10.0f) < this->actor.world.pos.y) {
            ret = true;
        }
    } else if (this->actor.world.pos.y < (this->actor.floorHeight + 20.0f)) {
        ret = true;
    }
    return ret;
}

void func_808A60E0(EnSt* this) {
    s32 animIndex = (this->unk_2C8 > 0.0f) ? ENST_ANIM_2 : ENST_ANIM_6;
    f32 sp20 = (this->unk_2C8 > 0.0f) ? 4.0f : 9.0f;
    f32 sp1C;
    u16 sfxId = (this->unk_2C8 < 0.0f) ? NA_SE_EN_STALTU_DOWN : NA_SE_EN_STALTU_UP;

    sp1C = this->skelAnime.curFrame / (this->skelAnime.animLength - 1.0f);

    if (sp1C == 1.0f) {
        SubS_ChangeAnimationByInfoS(&this->skelAnime, sAnimationInfo, animIndex);
        Actor_PlaySfx(&this->actor, sfxId);
    }

    this->unk_2D4 = (1.0f - sp1C) * sp20 * this->unk_2C8;

    if (this->unk_2C8 < 0.0f) {
        this->unk_2D4 = this->unk_2C8 * sp20;
    }
}

s32 func_808A61F4(EnSt* this) {
    if (this->actor.colChkInfo.damage < this->actor.colChkInfo.health) {
        this->actor.colChkInfo.health -= this->actor.colChkInfo.damage;
    } else {
        this->actor.colChkInfo.health = 0;
    }
    return this->actor.colChkInfo.health;
}

void func_808A6220(EnSt* this, PlayState* play) {
    s32 pad;

    if ((DECR(this->unk_314) == 0) && (this->actor.colChkInfo.health != 0)) {
        Collider_UpdateCylinder(&this->actor, &this->collider2);
        Collider_UpdateCylinder(&this->actor, &this->collider3);
        Collider_UpdateCylinder(&this->actor, &this->collider4);
        if ((s16)ABS_ALT(BINANG_SUB(this->actor.yawTowardsPlayer, this->actor.shape.rot.y)) < 0x4E00) {
            CollisionCheck_SetAC(play, &play->colChkCtx, &this->collider3.base);
        } else {
            CollisionCheck_SetAC(play, &play->colChkCtx, &this->collider2.base);
        }
        CollisionCheck_SetAC(play, &play->colChkCtx, &this->collider4.base);
    }

    if (this->actor.colChkInfo.health != 0) {
        Collider_UpdateCylinder(&this->actor, &this->collider1);
        if (((this->unk_2C8 < 0.0f) && (this->unk_18C & 0x10)) || (this->unk_18C & 1)) {
            this->collider1.dim.yShift = -40;
            this->collider1.dim.radius = 56;
            this->collider1.dim.height = 80;
        } else {
            this->collider1.dim.yShift = -20;
            this->collider1.dim.radius = 18;
            this->collider1.dim.height = 48;
        }
        CollisionCheck_SetOC(play, &play->colChkCtx, &this->collider1.base);
        CollisionCheck_SetAT(play, &play->colChkCtx, &this->collider1.base);
    }
}

void func_808A63E8(EnSt* this) {
    SubS_ChangeAnimationByInfoS(&this->skelAnime, sAnimationInfo, ENST_ANIM_3);
    this->unk_2C8 = 1.0f;
    func_808A5D7C(this);
    this->unk_30C = 0;
    this->unk_18C &= ~0x10;
    this->unk_2C8 = -1.0f;
    this->unk_2D4 = 0.0f;
    this->unk_2CC = 0.0f;
    this->unk_2C4 = 1.0f;
}

void func_808A6468(EnSt* this, PlayState* play) {
    func_808A5050(this, play);
    SubS_ChangeAnimationByInfoS(&this->skelAnime, sAnimationInfo, ENST_ANIM_4);
    this->unk_18C |= (0x8 | 0x4);
    this->unk_18C &= ~(0x10 | 0x2);
    this->unk_2C8 = -1.0f;
    func_808A5D7C(this);
    this->unk_30E = 30;
    this->unk_30C = 0;
    this->unk_2C8 = 1.0f;
    this->unk_2D8 = 30.0f;
    this->unk_2D4 = 0.0f;
    this->unk_2CC = 0.0f;
    this->unk_2C4 = 6.0f;
}

void func_808A650C(EnSt* this) {
    s32 animIndex = (this->unk_2C8 > 0.0f) ? ENST_ANIM_2 : ENST_ANIM_6;

    SubS_ChangeAnimationByInfoS(&this->skelAnime, sAnimationInfo, animIndex);
    this->unk_2CC = 0.0f;
    this->unk_2D4 = 0.0f;
    this->unk_2D8 = 0.0f;
    this->unk_18C &= ~4;
    this->unk_18C |= 0x10;
}

s32 func_808A6580(EnSt* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    s32 ret = false;
    s32 i;

    if (this->unk_316 != 0) {
        this->unk_316--;
    }

    if (this->collider3.base.acFlags & AC_HIT) {
        if (this->drawDmgEffType == ACTOR_DRAW_DMGEFF_FROZEN_NO_SFX) {
            // clang-format off
            for (i = 0; i < ENST_BODYPART_MAX; i++) { this->unk_31C[i] = 0; }
            // clang-format on

            this->unk_318 = 0;
        }

        if (this->actor.colChkInfo.damageEffect == 4) {
            Actor_Spawn(&play->actorCtx, play, ACTOR_EN_CLEAR_TAG, this->actor.world.pos.x, this->actor.world.pos.y,
                        this->actor.world.pos.z, 0, 0, 0, CLEAR_TAG_PARAMS(CLEAR_TAG_LARGE_LIGHT_RAYS));
        }
        this->collider3.base.acFlags &= ~AC_HIT;
    }

    if ((this->collider2.base.acFlags & AC_HIT) || (this->collider4.base.acFlags & AC_HIT)) {
        this->collider2.base.acFlags &= ~AC_HIT;
        this->collider4.base.acFlags &= ~AC_HIT;
        if (this->actor.colChkInfo.damageEffect == 4) {
            Actor_Spawn(&play->actorCtx, play, ACTOR_EN_CLEAR_TAG, this->actor.world.pos.x, this->actor.world.pos.y,
                        this->actor.world.pos.z, 0, 0, 0, CLEAR_TAG_PARAMS(CLEAR_TAG_LARGE_LIGHT_RAYS));
        }

        if (this->drawDmgEffType == ACTOR_DRAW_DMGEFF_FROZEN_NO_SFX) {
            // clang-format off
            for (i = 0; i < ENST_BODYPART_MAX; i++) { this->unk_31C[i] = 0; }
            // clang-format on

            this->unk_318 = 0;
        } else if (func_808A61F4(this)) {
            switch (this->actor.colChkInfo.damageEffect) {
                case 1:
                    Actor_PlaySfx(&this->actor, NA_SE_EN_COMMON_FREEZE);
                    this->unk_312 = 40;
                    Actor_SetColorFilter(&this->actor, COLORFILTER_COLORFLAG_BLUE, 200, COLORFILTER_BUFFLAG_OPA,
                                         this->unk_312);
                    break;

                case 5:
                    this->drawDmgEffType = ACTOR_DRAW_DMGEFF_ELECTRIC_SPARKS_SMALL;
                    this->unk_312 = 40;
                    func_808A576C(this);
                    Actor_SetColorFilter(&this->actor, COLORFILTER_COLORFLAG_BLUE, 200, COLORFILTER_BUFFLAG_OPA,
                                         this->unk_312);
                    break;

                default:
                    Actor_PlaySfx(&this->actor, NA_SE_EN_STALTU_DAMAGE);
                    this->unk_314 = 20;
                    this->unk_312 = 0;
                    Actor_SetColorFilter(&this->actor, COLORFILTER_COLORFLAG_RED, 200, COLORFILTER_BUFFLAG_OPA,
                                         this->unk_314);
                    SubS_ChangeAnimationByInfoS(&this->skelAnime, sAnimationInfo, ENST_ANIM_1);
                    this->unk_18C |= 8;
                    this->actionFunc = func_808A6D84;
                    this->unk_2C8 = -1.0f;
                    break;
            }
        } else {
            if (ENST_GET_SWITCH_FLAG(&this->actor) != 0x3F) {
                Flags_SetSwitch(play, ENST_GET_SWITCH_FLAG(&this->actor));
            }
            SoundSource_PlaySfxAtFixedWorldPos(play, &this->actor.world.pos, 40, NA_SE_EN_STALTU_DEAD);
            Enemy_StartFinishingBlow(play, &this->actor);

            this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;

            switch (this->actor.colChkInfo.damageEffect) {
                case 4:
                    this->drawDmgEffType = ACTOR_DRAW_DMGEFF_LIGHT_ORBS;
                    this->unk_318 = 20;
                    func_808A576C(this);
                    break;

                case 3:
                    this->drawDmgEffType = ACTOR_DRAW_DMGEFF_FROZEN_NO_SFX;
                    this->unk_318 = 0;
                    func_808A576C(this);
                    break;

                case 2:
                    this->drawDmgEffType = ACTOR_DRAW_DMGEFF_FIRE;
                    this->unk_318 = 20;
                    func_808A576C(this);
                    break;

                default:
                    this->drawDmgEffType = ACTOR_DRAW_DMGEFF_BLUE_FIRE;
                    this->unk_318 = 0;
                    break;
            }

            if (this->drawDmgEffType != ACTOR_DRAW_DMGEFF_FROZEN_NO_SFX) {
                SubS_ChangeAnimationByInfoS(&this->skelAnime, sAnimationInfo, ENST_ANIM_7);
                this->unk_2CC = 0.0f;
                this->unk_2D4 = 0.0f;
                this->actor.gravity = -1.0f;
            }
            this->unk_314 = 20;
            this->unk_312 = 0;
            Actor_SetColorFilter(&this->actor, COLORFILTER_COLORFLAG_RED, 200, COLORFILTER_BUFFLAG_OPA, this->unk_314);
            ret = true;
        }
        this->unk_310 = 0;
        this->unk_18C &= ~1;
    }

    if ((this->collider1.base.ocFlags2 & OC2_HIT_PLAYER) || (this->collider1.base.atFlags & AT_HIT)) {
        if ((this->unk_316 == 0) && (this->unk_314 == 0) && (this->unk_312 == 0) &&
            !(this->collider1.base.atFlags & AT_BOUNCED) && (this->actor.colChkInfo.health != 0)) {
            play->damagePlayer(play, -8);
            Actor_PlaySfx(&player->actor, NA_SE_PL_BODY_HIT);
            func_800B8D98(play, &this->actor, 4.0f, this->actor.yawTowardsPlayer, 6.0f);
            this->unk_316 = 10;
            this->unk_18C |= 1;
            this->unk_30E = 0;
        }
        this->collider1.base.ocFlags1 &= ~OC1_HIT;
        this->collider1.base.atFlags &= ~AT_HIT;
    }
    return ret;
}

s32 func_808A6A3C(EnSt* this) {
    s32 ret = false;

    if ((ENST_GET_1C0(&this->actor) == ENST_1C0_1) && (this->actor.colChkInfo.health != 0)) {
        ret = true;
    }
    return ret;
}

void func_808A6A78(EnSt* this, PlayState* play) {
    s32 pad[2];

    if (Object_IsLoaded(&play->objectCtx, this->objectSlot)) {
        ActorShape_Init(&this->actor.shape, 0.0f, ActorShadow_DrawCircle, 8.0f);
        SkelAnime_Init(play, &this->skelAnime, &object_st_Skel_005298, NULL, this->jointTable, this->morphTable,
                       OBJECT_ST_LIMB_MAX);
        SubS_ChangeAnimationByInfoS(&this->skelAnime, sAnimationInfo, ENST_ANIM_0);

        Collider_InitAndSetCylinder(play, &this->collider1, &this->actor, &sCylinderInit1);
        Collider_InitAndSetCylinder(play, &this->collider2, &this->actor, &sCylinderInit2);
        Collider_InitAndSetCylinder(play, &this->collider3, &this->actor, &sCylinderInit3);
        Collider_InitAndSetCylinder(play, &this->collider4, &this->actor, &sCylinderInit4);
        CollisionCheck_SetInfo2(&this->actor.colChkInfo, &sDamageTable, &sColChkInfoInit);

        if (ENST_GET_1C0(&this->actor) == ENST_1C0_1) {
            this->actor.flags |= ACTOR_FLAG_REACT_TO_LENS;
        }

        Actor_SetScale(&this->actor, 0.04f);
        this->actor.draw = func_808A7478;
        this->actor.hintId = TATL_HINT_ID_SKULLTULA;
        this->actor.gravity = 0.0f;
        func_808A63E8(this);
        this->unk_18C = 0;
        this->unk_18C &= ~0x10;
        this->actionFunc = func_808A6C04;
    }
}

void func_808A6C04(EnSt* this, PlayState* play) {
    this->unk_2CC = Math_SinS(this->unk_30C) * this->unk_2C4;
    this->unk_30C += 0xE00;
    Math_ApproachF(&this->unk_2C4, 1.0f, 0.1f, 0.3f);
    this->actor.velocity.y = this->unk_2D4 + this->unk_2CC;
    Actor_MoveWithGravity(&this->actor);

    if ((this->unk_18C & 8) && Animation_OnFrame(&this->skelAnime, this->skelAnime.endFrame)) {
        SubS_ChangeAnimationByInfoS(&this->skelAnime, sAnimationInfo, ENST_ANIM_3);
        this->unk_18C &= ~8;
    } else if ((this->unk_310 == 0) && func_808A5F28(this, play) && !func_808A6064(this)) {
        func_808A650C(this);
        this->actionFunc = func_808A6D84;
    } else {
        if ((this->unk_316 == 0) && (this->unk_314 == 0) && (this->unk_312 == 0) && (this->unk_2C8 > 0.0f) &&
            (this->actor.xzDistToPlayer < 62.0f)) {
            this->unk_30E = 0;
            this->unk_18C |= 1;
        }
        func_808A5CCC(this);
    }
}

void func_808A6D84(EnSt* this, PlayState* play) {
    func_808A60E0(this);
    this->actor.velocity.y = this->unk_2D4 + this->unk_2CC;
    Actor_MoveWithGravity(&this->actor);
    if (func_808A6064(this)) {
        if (this->unk_2C8 < 0.0f) {
            func_808A6468(this, play);
        } else {
            func_808A63E8(this);
        }
        this->actionFunc = func_808A6C04;
    }
    func_808A5CCC(this);
}

void func_808A6E24(EnSt* this, PlayState* play) {
    s32 i;
    s32 bodyPartsCount = 0;

    if (this->drawDmgEffType == ACTOR_DRAW_DMGEFF_FROZEN_NO_SFX) {
        for (i = 0; i < ENST_BODYPART_MAX; i++) {
            if (DECR(this->unk_31C[i]) == 0) {
                bodyPartsCount++;
            }
        }

        if (bodyPartsCount == ENST_BODYPART_MAX) {
            SubS_ChangeAnimationByInfoS(&this->skelAnime, sAnimationInfo, ENST_ANIM_7);
            this->drawDmgEffType = ACTOR_DRAW_DMGEFF_BLUE_FIRE;
            this->unk_2CC = 0.0f;
            this->unk_2D4 = 0.0f;
            this->actor.gravity = -1.0f;
            func_808A5AF8(this, play);
        }
    } else if (this->unk_18C & 0x80) {
        this->actionFunc = func_808A701C;
        this->actor.gravity = 0.0f;
        this->unk_2CC = 0.0f;
        this->unk_2D4 = 0.0f;
    } else {
        if (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND) {
            this->actor.shape.yOffset = 400.0f;
            this->actor.world.rot.x = 0x4000;
            this->actor.shape.rot.x = this->actor.world.rot.x;
            this->unk_2D0 = this->actor.velocity.y = fabsf(this->actor.velocity.y) * 0.6f;
            this->actor.speed = 0.0f;

            if ((s32)this->unk_2D0 != 0) {
                Actor_PlaySfx(&this->actor, NA_SE_EN_EYEGOLE_ATTACK);
            } else {
                this->actor.velocity.y = 0.0f;
                this->actionFunc = func_808A701C;
            }

            if ((s32)this->actor.velocity.y >= 2) {
                func_808A52A8(this, play);
            }
        } else {
            Math_ApproachF(&this->actor.shape.yOffset, 400.0f, 0.3f, 1000.0f);
            this->actor.world.rot.x += 0x1000;
            if (this->actor.world.rot.x >= 0x4000) {
                this->actor.world.rot.x = 0x4000;
            }
            this->actor.shape.rot.x = this->actor.world.rot.x;
        }
        Actor_MoveWithGravity(&this->actor);
    }
}

void func_808A701C(EnSt* this, PlayState* play) {
    s32 i;
    s32 bodyPartsCount = 0;

    if (this->unk_18C & 0x40) {
        if (DECR(this->unk_318) == 0) {
            this->unk_18C |= 0x20;
        }

        for (i = 0; i < ENST_BODYPART_MAX; i++) {
            if (DECR(this->unk_31C[i]) == 0) {
                bodyPartsCount++;
            }
        }

        if (bodyPartsCount == ENST_BODYPART_MAX) {
            Item_DropCollectibleRandom(play, NULL, &this->actor.world.pos, 0);
            Actor_Kill(&this->actor);
        }
    } else if (DECR(this->unk_318) == 0) {
        this->unk_18C |= 0x40;
        if (this->drawDmgEffType == ACTOR_DRAW_DMGEFF_BLUE_FIRE) {
            func_808A576C(this);
            this->unk_318 = 10;
        }
    }
}

void EnSt_Init(Actor* thisx, PlayState* play) {
    EnSt* this = (EnSt*)thisx;

    this->objectSlot = Object_GetSlot(&play->objectCtx, GAMEPLAY_KEEP);
    if (((ENST_GET_SWITCH_FLAG(&this->actor) != 0x3F) && Flags_GetSwitch(play, ENST_GET_SWITCH_FLAG(&this->actor))) ||
        (this->objectSlot <= OBJECT_SLOT_NONE)) {
        Actor_Kill(&this->actor);
        return;
    }

    this->actionFunc = func_808A6A78;
}

void EnSt_Destroy(Actor* thisx, PlayState* play) {
    EnSt* this = (EnSt*)thisx;

    Collider_DestroyCylinder(play, &this->collider1);
    Collider_DestroyCylinder(play, &this->collider2);
    Collider_DestroyCylinder(play, &this->collider3);
    Collider_DestroyCylinder(play, &this->collider4);
}

void EnSt_Update(Actor* thisx, PlayState* play) {
    EnSt* this = (EnSt*)thisx;

    if (this->actor.flags & ACTOR_FLAG_ATTACHED_TO_ARROW) {
        SkelAnime_Update(&this->skelAnime);
        this->unk_18C |= 0x80;
        return;
    }

    if (!(this->actor.flags & ACTOR_FLAG_REACT_TO_LENS) && func_808A6A3C(this)) {
        this->actor.flags |= ACTOR_FLAG_REACT_TO_LENS;
    }

    if (func_808A6580(this, play)) {
        this->actionFunc = func_808A6E24;
    } else if (DECR(this->unk_312) == 0) {
        this->actionFunc(this, play);
        if (this->drawDmgEffType != ACTOR_DRAW_DMGEFF_FROZEN_NO_SFX) {
            SkelAnime_Update(&this->skelAnime);
        }
        Actor_UpdateBgCheckInfo(play, &this->actor, 30.0f, 12.0f, 0.0f, UPDBGCHECKINFO_FLAG_1 | UPDBGCHECKINFO_FLAG_4);
    } else if (this->unk_312 < 20) {
        s16 idx = (this->unk_312 % 2) ? -1 : 1;

        idx *= 0x800;
        idx += this->actor.world.rot.y;
        this->actor.shape.rot.y = idx;

        if (this->drawDmgEffType == ACTOR_DRAW_DMGEFF_ELECTRIC_SPARKS_SMALL) {
            if (this->unk_312 == 1) {
                s32 i;

                // clang-format off
                for (i = 0; i < ENST_BODYPART_MAX; i++) { this->unk_31C[i] = 0; }
                // clang-format on
            }
        }
    }

    Actor_SetFocus(&this->actor, 0.0f);
    func_808A6220(this, play);
}

s32 EnSt_OverrideLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, Actor* thisx,
                          Gfx** gfx) {
    EnSt* this = (EnSt*)thisx;
    Color_RGB8 sp20;

    if (limbIndex == OBJECT_ST_LIMB_04) {
        func_808A5DCC(this, &sp20);

        gDPPipeSync((*gfx)++);
        gDPSetEnvColor((*gfx)++, sp20.r, sp20.g, sp20.b, 0);
    }
    return false;
}

void func_808A7478(Actor* thisx, PlayState* play) {
    EnSt* this = (EnSt*)thisx;
    s32 bodyPartIndex;
    s32 count;

    if (!(this->unk_18C & 0x20)) {
        OPEN_DISPS(play->state.gfxCtx);

        if (func_808A6A3C(this)) {
            Gfx_SetupDL25_Xlu(play->state.gfxCtx);
            POLY_XLU_DISP = SkelAnime_Draw(play, this->skelAnime.skeleton, this->skelAnime.jointTable,
                                           EnSt_OverrideLimbDraw, NULL, &this->actor, POLY_XLU_DISP);
        } else {
            Gfx_SetupDL25_Opa(play->state.gfxCtx);
            POLY_OPA_DISP = SkelAnime_Draw(play, this->skelAnime.skeleton, this->skelAnime.jointTable,
                                           EnSt_OverrideLimbDraw, NULL, &this->actor, POLY_OPA_DISP);
        }

        CLOSE_DISPS(play->state.gfxCtx);
    }

    for (bodyPartIndex = 0, count = 0; bodyPartIndex < ENST_BODYPART_MAX; bodyPartIndex++) {
        count += func_808A5988(this, play, bodyPartIndex);
    }

    if (count != 0) {
        this->unk_31A += 2;
    }

    func_808A54B0(this, play);
}
