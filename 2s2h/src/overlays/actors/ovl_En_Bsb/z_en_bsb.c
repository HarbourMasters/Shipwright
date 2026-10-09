/*
 * File: z_en_bsb.c
 * Overlay: ovl_En_Bsb
 * Description: Captain Keeta
 */

#include "z_en_bsb.h"
#include "z64rumble.h"
#include "z64shrink_window.h"
#include "2s2h/GameInteractor/GameInteractor.h"

#include "overlays/actors/ovl_En_Clear_Tag/z_en_clear_tag.h"
#include "overlays/effects/ovl_Effect_Ss_Hahen/z_eff_ss_hahen.h"
#include "overlays/effects/ovl_Effect_Ss_Hitmark/z_eff_ss_hitmark.h"

#include "objects/gameplay_keep/gameplay_keep.h"

#define FLAGS                                                                                 \
    (ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_HOSTILE | ACTOR_FLAG_UPDATE_CULLING_DISABLED | \
     ACTOR_FLAG_DRAW_CULLING_DISABLED | ACTOR_FLAG_UPDATE_DURING_OCARINA)

void EnBsb_Init(Actor* thisx, PlayState* play);
void EnBsb_Destroy(Actor* thisx, PlayState* play);
void EnBsb_Update(Actor* thisx, PlayState* play);
void EnBsb_Draw(Actor* thisx, PlayState* play);

void func_80C0BF2C(EnBsb* this);
void func_80C0BFE8(EnBsb* this, PlayState* play);
void func_80C0C0F4(EnBsb* this, PlayState* play);
void func_80C0C238(EnBsb* this, PlayState* play);
void func_80C0C32C(EnBsb* this);
void func_80C0C364(EnBsb* this, PlayState* play);
void func_80C0C430(EnBsb* this);
void func_80C0C484(EnBsb* this, PlayState* play);
void func_80C0C6A8(EnBsb* this, PlayState* play);
void func_80C0C86C(EnBsb* this);
void func_80C0C8EC(EnBsb* this, PlayState* play);
void func_80C0CA28(EnBsb* this, PlayState* play);
void func_80C0CB3C(EnBsb* this, PlayState* play);
void func_80C0CCCC(EnBsb* this);
void func_80C0CD04(EnBsb* this, PlayState* play);
void func_80C0CD90(EnBsb* this);
void func_80C0CDE4(EnBsb* this, PlayState* play);
void func_80C0CF4C(EnBsb* this);
void func_80C0CFDC(EnBsb* this, PlayState* play);
void func_80C0D10C(EnBsb* this, PlayState* play);
void func_80C0D27C(EnBsb* this, PlayState* play);
void func_80C0D384(EnBsb* this, PlayState* play);
void func_80C0D51C(EnBsb* this, PlayState* play);
void func_80C0D964(EnBsb* this, PlayState* play);
void func_80C0D9B4(EnBsb* this, PlayState* play);
void func_80C0DA58(EnBsb* this);
void func_80C0DB18(EnBsb* this, PlayState* play);
void func_80C0E1C0(EnBsb* this, PlayState* play);
void func_80C0E3B8(EnBsb* this);
void func_80C0E480(EnBsb* this, PlayState* play);
void func_80C0E4FC(EnBsb* this, PlayState* play);
s32 func_80C0E9CC(EnBsb* this, PlayState* play);
void func_80C0B31C(PlayState* play, EnBsb* this, Vec3f* pos);
s32 func_80C0B888(EnBsb* this, PlayState* play);
void func_80C0C32C(EnBsb* this);
void func_80C0C430(EnBsb* this);
void func_80C0D00C(EnBsb* this);
void func_80C0E3B8(EnBsb* this);
void func_80C0E618(EnBsb* this, PlayState* play);

s32 func_80C0E9CC(EnBsb* this, PlayState* play);

void EnBsb_InitializeEffect(EnBsb* this, Vec3f* pos, Vec3f* velocity, Vec3f* accel, f32 scale, s16 life);
void EnBsb_UpdateEffects(EnBsb* this, PlayState* play);
void EnBsb_DrawEffects(EnBsb* this, PlayState* play);

f32 D_80C0F8D0 = 0.0f;

static ColliderJntSphElementInit sJntSphElementsInit[7] = {
    {
        {
            ELEM_MATERIAL_UNK0,
            { 0x00000000, 0x00, 0x00 },
            { 0xF7CFFFFF, 0x00, 0x00 },
            ATELEM_NONE | ATELEM_SFX_NORMAL,
            ACELEM_ON,
            OCELEM_ON,
        },
        { 10, { { 1000, 400, 0 }, 40 }, 100 },
    },
    {
        {
            ELEM_MATERIAL_UNK0,
            { 0xF7CFFFFF, 0x04, 0x08 },
            { 0x00000000, 0x00, 0x00 },
            ATELEM_ON | ATELEM_SFX_NORMAL,
            ACELEM_ON,
            OCELEM_ON,
        },
        { 9, { { 0, 700, 200 }, 35 }, 100 },
    },
    {
        {
            ELEM_MATERIAL_UNK0,
            { 0x00000000, 0x00, 0x00 },
            { 0xF7CFFFFF, 0x00, 0x00 },
            ATELEM_NONE | ATELEM_SFX_NORMAL,
            ACELEM_ON,
            OCELEM_ON,
        },
        { 6, { { 100, 600, 0 }, 35 }, 100 },
    },
    {
        {
            ELEM_MATERIAL_UNK0,
            { 0x00000000, 0x00, 0x00 },
            { 0xF7CFFFFF, 0x00, 0x00 },
            ATELEM_NONE | ATELEM_SFX_NORMAL,
            ACELEM_ON,
            OCELEM_NONE,
        },
        { 3, { { 400, 200, 0 }, 40 }, 100 },
    },
    {
        {
            ELEM_MATERIAL_UNK0,
            { 0x00000000, 0x00, 0x00 },
            { 0xF7CFFFFF, 0x00, 0x00 },
            ATELEM_NONE | ATELEM_SFX_NORMAL,
            ACELEM_ON,
            OCELEM_ON,
        },
        { 13, { { 700, -100, 0 }, 35 }, 100 },
    },
    {
        {
            ELEM_MATERIAL_UNK0,
            { 0x00000000, 0x00, 0x00 },
            { 0xF7CFFFFF, 0x00, 0x00 },
            ATELEM_NONE | ATELEM_SFX_NORMAL,
            ACELEM_ON,
            OCELEM_ON,
        },
        { 16, { { 200, 300, 0 }, 30 }, 100 },
    },
    {
        {
            ELEM_MATERIAL_UNK0,
            { 0x00000000, 0x00, 0x00 },
            { 0xF7CFFFFF, 0x00, 0x00 },
            ATELEM_NONE | ATELEM_SFX_NORMAL,
            ACELEM_ON,
            OCELEM_ON,
        },
        { 19, { { 200, 300, 0 }, 30 }, 100 },
    },
};

static ColliderJntSphInit sJntSphInit = {
    {
        COL_MATERIAL_HIT6,
        AT_ON | AT_TYPE_ENEMY,
        AC_ON | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_PLAYER,
        OC2_TYPE_1,
        COLSHAPE_JNTSPH,
    },
    ARRAY_COUNT(sJntSphElementsInit),
    sJntSphElementsInit,
};

static DamageTable sDamageTable = {
    /* Deku Nut       */ DMG_ENTRY(0, 0x1),
    /* Deku Stick     */ DMG_ENTRY(1, 0xD),
    /* Horse trample  */ DMG_ENTRY(0, 0x0),
    /* Explosives     */ DMG_ENTRY(1, 0xE),
    /* Zora boomerang */ DMG_ENTRY(0, 0xF),
    /* Normal arrow   */ DMG_ENTRY(0, 0xF),
    /* UNK_DMG_0x06   */ DMG_ENTRY(0, 0x0),
    /* Hookshot       */ DMG_ENTRY(0, 0xF),
    /* Goron punch    */ DMG_ENTRY(1, 0xD),
    /* Sword          */ DMG_ENTRY(1, 0xD),
    /* Goron pound    */ DMG_ENTRY(1, 0xD),
    /* Fire arrow     */ DMG_ENTRY(0, 0x2),
    /* Ice arrow      */ DMG_ENTRY(0, 0x3),
    /* Light arrow    */ DMG_ENTRY(0, 0x4),
    /* Goron spikes   */ DMG_ENTRY(0, 0xF),
    /* Deku spin      */ DMG_ENTRY(0, 0xC),
    /* Deku bubble    */ DMG_ENTRY(0, 0xF),
    /* Deku launch    */ DMG_ENTRY(1, 0xE),
    /* UNK_DMG_0x12   */ DMG_ENTRY(0, 0x1),
    /* Zora barrier   */ DMG_ENTRY(1, 0x5),
    /* Normal shield  */ DMG_ENTRY(0, 0x0),
    /* Light ray      */ DMG_ENTRY(0, 0x0),
    /* Thrown object  */ DMG_ENTRY(0, 0xF),
    /* Zora punch     */ DMG_ENTRY(1, 0xD),
    /* Spin attack    */ DMG_ENTRY(1, 0xD),
    /* Sword beam     */ DMG_ENTRY(0, 0x0),
    /* Normal Roll    */ DMG_ENTRY(0, 0x0),
    /* UNK_DMG_0x1B   */ DMG_ENTRY(0, 0x0),
    /* UNK_DMG_0x1C   */ DMG_ENTRY(0, 0x0),
    /* Unblockable    */ DMG_ENTRY(0, 0x0),
    /* UNK_DMG_0x1E   */ DMG_ENTRY(0, 0x0),
    /* Powder Keg     */ DMG_ENTRY(1, 0xD),
};

ActorProfile En_Bsb_Profile = {
    /**/ ACTOR_EN_BSB,
    /**/ ACTORCAT_PROP,
    /**/ FLAGS,
    /**/ OBJECT_BSB,
    /**/ sizeof(EnBsb),
    /**/ EnBsb_Init,
    /**/ EnBsb_Destroy,
    /**/ EnBsb_Update,
    /**/ EnBsb_Draw,
};

typedef enum EnBsbAnimation {
    /*  0 */ ENBSB_ANIM_0,
    /*  1 */ ENBSB_ANIM_1,
    /*  2 */ ENBSB_ANIM_2,
    /*  3 */ ENBSB_ANIM_3,
    /*  4 */ ENBSB_ANIM_4,
    /*  5 */ ENBSB_ANIM_5,
    /*  6 */ ENBSB_ANIM_6,
    /*  7 */ ENBSB_ANIM_7,
    /*  8 */ ENBSB_ANIM_8,
    /*  9 */ ENBSB_ANIM_9,
    /* 10 */ ENBSB_ANIM_10,
    /* 11 */ ENBSB_ANIM_11,
    /* 12 */ ENBSB_ANIM_12,
    /* 13 */ ENBSB_ANIM_13,
    /* 14 */ ENBSB_ANIM_14,
    /* 15 */ ENBSB_ANIM_15,
    /* 16 */ ENBSB_ANIM_16,
    /* 17 */ ENBSB_ANIM_17,
    /* 18 */ ENBSB_ANIM_18,
    /* 19 */ ENBSB_ANIM_19,
    /* 20 */ ENBSB_ANIM_20,
    /* 21 */ ENBSB_ANIM_21,
    /* 22 */ ENBSB_ANIM_22,
    /* 23 */ ENBSB_ANIM_23,
    /* 24 */ ENBSB_ANIM_24,
    /* 25 */ ENBSB_ANIM_MAX
} EnBsbAnimation;

static AnimationHeader* sAnimations[ENBSB_ANIM_MAX] = {
    &object_bsb_Anim_0086BC, // ENBSB_ANIM_0
    &object_bsb_Anim_00CD88, // ENBSB_ANIM_1
    &object_bsb_Anim_000400, // ENBSB_ANIM_2
    &object_bsb_Anim_0065D8, // ENBSB_ANIM_3
    &object_bsb_Anim_000FF0, // ENBSB_ANIM_4
    &object_bsb_Anim_000C50, // ENBSB_ANIM_5
    &object_bsb_Anim_006C48, // ENBSB_ANIM_6
    &object_bsb_Anim_001390, // ENBSB_ANIM_7
    &object_bsb_Anim_002AF4, // ENBSB_ANIM_8
    &object_bsb_Anim_002590, // ENBSB_ANIM_9
    &object_bsb_Anim_007120, // ENBSB_ANIM_10
    &object_bsb_Anim_0043A4, // ENBSB_ANIM_11
    &object_bsb_Anim_007B18, // ENBSB_ANIM_12
    &object_bsb_Anim_001CD8, // ENBSB_ANIM_13
    &object_bsb_Anim_003E1C, // ENBSB_ANIM_14
    &object_bsb_Anim_003238, // ENBSB_ANIM_15
    &object_bsb_Anim_00606C, // ENBSB_ANIM_16
    &object_bsb_Anim_005440, // ENBSB_ANIM_17
    &object_bsb_Anim_004E2C, // ENBSB_ANIM_18
    &object_bsb_Anim_004894, // ENBSB_ANIM_19
    &object_bsb_Anim_004208, // ENBSB_ANIM_20
    &object_bsb_Anim_00D3CC, // ENBSB_ANIM_21
    &object_bsb_Anim_004510, // ENBSB_ANIM_22
    &object_bsb_Anim_001F90, // ENBSB_ANIM_23
    &object_bsb_Anim_00C790, // ENBSB_ANIM_24
};

static u8 sAnimationModes[ENBSB_ANIM_MAX] = {
    ANIMMODE_ONCE, // ENBSB_ANIM_0
    ANIMMODE_LOOP, // ENBSB_ANIM_1
    ANIMMODE_ONCE, // ENBSB_ANIM_2
    ANIMMODE_LOOP, // ENBSB_ANIM_3
    ANIMMODE_LOOP, // ENBSB_ANIM_4
    ANIMMODE_ONCE, // ENBSB_ANIM_5
    ANIMMODE_LOOP, // ENBSB_ANIM_6
    ANIMMODE_LOOP, // ENBSB_ANIM_7
    ANIMMODE_ONCE, // ENBSB_ANIM_8
    ANIMMODE_ONCE, // ENBSB_ANIM_9
    ANIMMODE_LOOP, // ENBSB_ANIM_10
    ANIMMODE_ONCE, // ENBSB_ANIM_11
    ANIMMODE_LOOP, // ENBSB_ANIM_12
    ANIMMODE_LOOP, // ENBSB_ANIM_13
    ANIMMODE_ONCE, // ENBSB_ANIM_14
    ANIMMODE_LOOP, // ENBSB_ANIM_15
    ANIMMODE_ONCE, // ENBSB_ANIM_16
    ANIMMODE_LOOP, // ENBSB_ANIM_17
    ANIMMODE_ONCE, // ENBSB_ANIM_18
    ANIMMODE_LOOP, // ENBSB_ANIM_19
    ANIMMODE_LOOP, // ENBSB_ANIM_20
    ANIMMODE_LOOP, // ENBSB_ANIM_21
    ANIMMODE_ONCE, // ENBSB_ANIM_22
    ANIMMODE_ONCE, // ENBSB_ANIM_23
    ANIMMODE_ONCE, // ENBSB_ANIM_24
};

void EnBsb_ChangeAnim(EnBsb* this, s32 animIndex) {
    this->animIndex = animIndex;
    this->animEndFrame = Animation_GetLastFrame(sAnimations[animIndex]);
    Animation_Change(&this->skelAnime, sAnimations[this->animIndex], 1.0f, 0.0f, this->animEndFrame,
                     sAnimationModes[this->animIndex], -2.0f);
}

void func_80C0B31C(PlayState* play, EnBsb* this, Vec3f* pos) {
    Vec3f effectPos;
    Vec3f effectAccel;
    Vec3f effectVelocity;
    Vec3f hahenVelocity = { 0.0f, 8.0f, 0.0f };
    Vec3f hahenAccel = { 0.0f, -1.5f, 0.0f };
    f32 angle = (Rand_ZeroOne() - 0.5f) * 12.56f;
    f32 effectScale;
    s32 i;
    s32 pad[4];

    effectPos.y = this->actor.floorHeight;
    effectPos.x = Math_SinF(angle) * 5.0f + pos->x;
    effectPos.z = Math_CosF(angle) * 5.0f + pos->z;
    hahenAccel.x = Rand_CenteredFloat(1.0f) * 0.5f;
    hahenAccel.z = Rand_CenteredFloat(1.0f) * 0.5f;
    hahenVelocity.y += (Rand_ZeroOne() - 0.5f) * 15.0f;

    EffectSsHahen_Spawn(play, &effectPos, &hahenVelocity, &hahenAccel, 0, Rand_ZeroOne() * 5.0f + 10.0f,
                        HAHEN_OBJECT_DEFAULT, 10, NULL);
    func_800BBFB0(play, &effectPos, 20.0f, 1, 300, 10, 5);

    if (!this->unk_02AE) {
        this->unk_02AE = true;

        for (i = 0; i < 10; i++) {
            if ((i % 2) == 0) {
                Math_Vec3f_Copy(&effectPos, &this->unk_0304);
            } else {
                Math_Vec3f_Copy(&effectPos, &this->unk_02F8);
            }
            effectPos.x += Rand_CenteredFloat(5.0f);
            effectPos.y += Rand_CenteredFloat(5.0f);
            effectPos.z += Rand_CenteredFloat(5.0f);
            effectAccel.y = -1.0f;
            effectVelocity.x = (Rand_ZeroOne() - 0.5f) * 4.0f;
            effectVelocity.y = (Rand_ZeroOne() * 10.0f) + 4.0f;
            effectVelocity.z = (Rand_ZeroOne() - 0.5f) * 4.0f;
            effectScale = Rand_CenteredFloat(1.0f) * 0.002f + 0.005f;
            EnBsb_InitializeEffect(this, &effectPos, &effectVelocity, &effectAccel, effectScale,
                                   (s32)Rand_ZeroFloat(10.0f) + 30);
        }
    }
}

void EnBsb_Init(Actor* thisx, PlayState* play) {
    EnBsb* this = (EnBsb*)thisx;
    s32 csId;
    s32 i;

    Actor_SetScale(&this->actor, 0.04f);
    this->alpha = 255;
    this->actor.colChkInfo.mass = MASS_IMMOVABLE;

    if (ENBSB_GET_PARAMS_8000(&this->actor)) {
        SkelAnime_Init(play, &this->skelAnime, &object_bsb_Skel_00C3E0, &object_bsb_Anim_004894, this->jointTable,
                       this->morphTable, OBJECT_BSB_LIMB_MAX);
        this->unk_02B0 = ENBSB_GET_LIMB_INDEX(&this->actor);
        func_80C0E3B8(this);
        return;
    }

    this->actor.colChkInfo.damageTable = &sDamageTable;

    ActorShape_Init(&this->actor.shape, 0.0f, ActorShadow_DrawCircle, 0.0f);
    SkelAnime_Init(play, &this->skelAnime, &object_bsb_Skel_00C3E0, &object_bsb_Anim_0086BC, this->jointTable,
                   this->morphTable, OBJECT_BSB_LIMB_MAX);

    this->pathIndex = ENBSB_GET_PATH_INDEX(&this->actor);
    this->switchFlag1 = ENBSB_GET_SWITCH_FLAG1(&this->actor);
    this->switchFlag2 = ENBSB_GET_SWITCH_FLAG2(&this->actor);
    this->actor.colChkInfo.health = 24;
    this->actor.hintId = TATL_HINT_ID_CAPTAIN_KEETA;
    this->actor.gravity = -2.0f;

    Collider_InitAndSetJntSph(play, &this->collider, &this->actor, &sJntSphInit, this->colliderElements);

    if (this->pathIndex == ENBSB_PATH_INDEX_NONE) {
        Actor_Kill(&this->actor);
        return;
    }

    csId = this->actor.csId;
    i = 0;

    // clang-format off
    while (csId != CS_ID_NONE) { this->csIdList[i] = csId; csId = CutsceneManager_GetAdditionalCsId(csId); i++; }
    // clang-format on

    this->actor.attentionRangeType = ATTENTION_RANGE_10;

    if (CHECK_WEEKEVENTREG(WEEKEVENTREG_23_04)) {
        Actor_Kill(&this->actor);
    } else {
        func_80C0BF2C(this);
    }
}

void EnBsb_Destroy(Actor* thisx, PlayState* play) {
    EnBsb* this = (EnBsb*)thisx;

    if (this->unk_02B0 == OBJECT_BSB_LIMB_NONE) {
        Audio_RestorePrevBgm();
        Collider_DestroyJntSph(play, &this->collider);
    }
    if (CutsceneManager_GetCurrentCsId() == this->csIdList[3]) {
        CutsceneManager_Stop(this->csIdList[3]);
    }
}

s32 func_80C0B888(EnBsb* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    s16 yaw;

    if (!this->unk_02DC) {
        yaw = ABS_ALT((s16)(this->actor.yawTowardsPlayer - this->unk_02BC));
        if (yaw < (s16)(BREG(1) + 0x4000) && (fabsf(this->actor.world.pos.y - player->actor.world.pos.y) < 20.0f) &&
            (this->actor.xzDistToPlayer < 200.0f)) {
            this->unk_02DC = true;
            Audio_PlayBgm_StorePrevBgm(NA_BGM_MINI_BOSS);
            return true;
        }
    } else {
        return true;
    }

    return false;
}

void func_80C0B970(EnBsb* this, PlayState* play) {
    s16 quakeY;

    if (Animation_OnFrame(&this->skelAnime, 8.0f) || Animation_OnFrame(&this->skelAnime, 14.0f)) {
        Actor_PlaySfx(&this->actor, NA_SE_EN_KTIA_WALK);

        quakeY = (700.0f - this->actor.xzDistToPlayer) * 0.01f;

        if (quakeY > 4) {
            quakeY = 4;
        }

        if (quakeY > 0) {
            Actor_RequestQuakeAndRumble(&this->actor, play, quakeY, 2);
        }

        if (Animation_OnFrame(&this->skelAnime, 8.0f)) {
            func_80C0B31C(play, this, &this->unk_0304);
        } else {
            func_80C0B31C(play, this, &this->unk_02F8);
        }
    }
}

void func_80C0BA58(EnBsb* this, PlayState* play) {
    s16 sourceIntensity;

    if (Animation_OnFrame(&this->skelAnime, 2.0f) || Animation_OnFrame(&this->skelAnime, 4.0f) ||
        Animation_OnFrame(&this->skelAnime, 6.0f) || Animation_OnFrame(&this->skelAnime, 8.0f)) {
        Actor_PlaySfx(&this->actor, NA_SE_EN_KTIA_WALK);
        Actor_RequestQuake(play, BREG(10) + 1, BREG(11) + 2);
        if (Animation_OnFrame(&this->skelAnime, 4.0f) || Animation_OnFrame(&this->skelAnime, 8.0f)) {
            func_80C0B31C(play, this, &this->unk_0304);
        } else {
            func_80C0B31C(play, this, &this->unk_02F8);
        }
    }

    if (this->actor.xzDistToPlayer < (BREG(7) + 400.0f)) {
        sourceIntensity = BREG(8) + 160.0f - this->actor.xzDistToPlayer * 0.3f;

        if (sourceIntensity > 70) {
            sourceIntensity = 70;
        } else if (sourceIntensity < 0) {
            sourceIntensity = 0;
        }

        if (Animation_OnFrame(&this->skelAnime, 1.0f)) {
            Rumble_Override(BREG(15), BREG(12) + sourceIntensity, BREG(13) + 15, BREG(14) + 70);
        }
    }
}

s32 func_80C0BC30(EnBsb* this) {
    if ((this->actor.world.pos.z < -2180.0f) && (this->actor.world.pos.z > -2470.0f) &&
        (fabsf(this->actor.world.pos.y - this->actor.home.pos.y) < 30.0f)) {
        Vec3s sp38 = { 0x2EE0, -0x2710, -0x4A38 };
        Vec3s sp30 = { -0x2710, 0x2710, -0x2710 };

        Math_SmoothStepToS(&this->unk_0316.x, sp38.x, 1, 0x7D0, 0);
        Math_SmoothStepToS(&this->unk_0316.y, sp38.y, 1, 0x7D0, 0);
        Math_SmoothStepToS(&this->unk_0316.z, sp38.z, 1, 0x7D0, 0);
        Math_SmoothStepToS(&this->unk_0310.x, sp30.x, 1, 0x7D0, 0);
        Math_SmoothStepToS(&this->unk_0310.y, sp30.y, 1, 0x7D0, 0);
        Math_SmoothStepToS(&this->unk_0310.z, sp30.z, 1, 0x7D0, 0);
        return true;
    }

    if (this->unk_0316.z != 0) {
        Math_SmoothStepToS(&this->unk_0316.x, 0, 1, 0x7D0, 0);
        Math_SmoothStepToS(&this->unk_0316.y, 0, 1, 0x7D0, 0);
        Math_SmoothStepToS(&this->unk_0316.z, 0, 1, 0x7D0, 0);
        Math_SmoothStepToS(&this->unk_0310.x, 0, 1, 0x7D0, 0);
        Math_SmoothStepToS(&this->unk_0310.y, 0, 1, 0x7D0, 0);
        Math_SmoothStepToS(&this->unk_0310.z, 0, 1, 0x7D0, 0);
    }

    return false;
}

void func_80C0BE1C(EnBsb* this, PlayState* play) {
    if (this->subCamId != SUB_CAM_ID_DONE) {
        Math_ApproachF(&this->subCamEye.x, this->subCamEyeNext.x, 0.5f, 30.0f);
        Math_ApproachF(&this->subCamEye.y, this->subCamEyeNext.y, 0.5f, 30.0f);
        Math_ApproachF(&this->subCamEye.z, this->subCamEyeNext.z, 0.5f, 30.0f);
        Math_ApproachF(&this->subCamAt.x, this->subCamAtNext.x, 0.5f, 30.0f);
        Math_ApproachF(&this->subCamAt.y, this->subCamAtNext.y, 0.5f, 30.0f);
        Math_ApproachF(&this->subCamAt.z, this->subCamAtNext.z, 0.5f, 30.0f);
        Math_ApproachF(&this->subCamFov, this->subCamFovNext, 0.5f, 10.0f);

        Play_SetCameraAtEye(play, this->subCamId, &this->subCamAt, &this->subCamEye);
        Play_SetCameraFov(play, this->subCamId, this->subCamFov);

        ShrinkWindow_Letterbox_SetSizeTarget(27);
    }
}

void func_80C0BF2C(EnBsb* this) {
    s32 i;

    this->collider.elements[0].dim.modelSphere.radius = 110;
    this->collider.elements[0].dim.modelSphere.center.x = 300;
    this->collider.elements[0].dim.modelSphere.center.y = 400;
    this->collider.base.colMaterial = COL_MATERIAL_HARD;

    for (i = 0; i < ARRAY_COUNT(this->colliderElements); i++) {
        this->collider.elements[i].base.elemMaterial = ELEM_MATERIAL_UNK2;
    }

    this->actor.flags |= ACTOR_FLAG_LOCK_ON_DISABLED;
    CLEAR_WEEKEVENTREG(WEEKEVENTREG_85_40);
    this->unk_02B4 = 0;
    this->actionFunc = func_80C0BFE8;
}

void func_80C0BFE8(EnBsb* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    s16 yaw = ABS_ALT((s16)(this->actor.yawTowardsPlayer - this->actor.shape.rot.y));

    if ((yaw < 0x4300) && !(this->actor.xzDistToPlayer > 300.0f)) {
        if (player->stateFlags2 & PLAYER_STATE2_USING_OCARINA) {
            if (!this->playedSfx) {
                Audio_PlaySfx(NA_SE_SY_TRE_BOX_APPEAR);
                this->playedSfx = true;
            }
        } else {
            this->playedSfx = false;
        }

        if ((play->msgCtx.ocarinaMode == OCARINA_MODE_EVENT) && (play->msgCtx.lastPlayedSong == OCARINA_SONG_SONATA)) {
            play->msgCtx.ocarinaMode = OCARINA_MODE_END;
            Actor_ChangeCategory(play, &play->actorCtx, &this->actor, ACTORCAT_BOSS);
            func_80C0C0F4(this, play);
        }
    }
}

void func_80C0C0F4(EnBsb* this, PlayState* play) {
    s32 i;

    this->actor.flags |= ACTOR_FLAG_UPDATE_DURING_OCARINA;
    this->unk_02A4 = 0;
    this->collider.elements[0].dim.modelSphere.radius = 40;
    this->collider.elements[0].dim.modelSphere.center.x = 1000;
    this->collider.elements[0].dim.modelSphere.center.y = 400;
    this->collider.base.colMaterial = COL_MATERIAL_NONE;

    for (i = 0; i < ARRAY_COUNT(this->colliderElements); i++) {
        this->collider.elements[i].base.elemMaterial = ELEM_MATERIAL_UNK0;
    }

    this->unk_02AE = false;

    Actor_PlaySfx(&this->actor, NA_SE_EN_STALTU_UP);

    EnBsb_ChangeAnim(this, ENBSB_ANIM_0);

    if (!CutsceneManager_IsNext(this->csIdList[0])) {
        CutsceneManager_Queue(this->csIdList[0]);
        this->unk_02B4 = 1;
        this->actionFunc = func_80C0C238;
    } else {
        CutsceneManager_StartWithPlayerCs(this->csIdList[0], &this->actor);

        this->actor.flags &= ~ACTOR_FLAG_LOCK_ON_DISABLED;

        Flags_SetSwitch(play, this->switchFlag1);

        this->unk_02A4 = 1;
        this->unk_02B4 = 1;
        this->actionFunc = func_80C0C238;
    }
}

void func_80C0C238(EnBsb* this, PlayState* play) {
    func_80C0BC30(this);

    if (this->unk_02A4 == 0) {
        if (!CutsceneManager_IsNext(this->csIdList[0])) {
            CutsceneManager_Queue(this->csIdList[0]);
            return;
        }
        CutsceneManager_StartWithPlayerCs(this->csIdList[0], &this->actor);
        this->actor.flags &= ~ACTOR_FLAG_LOCK_ON_DISABLED;
        Flags_SetSwitch(play, this->switchFlag1);
        this->unk_02A4 = 1;
    }

    if (this->unk_02A4 == 1) {
        if ((play->gameplayFrames % 2) == 0) {
            func_80C0B31C(play, this, &this->actor.world.pos);
        }
        Math_ApproachF(&this->actor.shape.shadowScale, 25.0f, 1.0f, 2.5f);
        if (this->skelAnime.curFrame >= this->animEndFrame) {
            func_80C0C32C(this);
        }
    }
}

void func_80C0C32C(EnBsb* this) {
    this->unk_02A4 = 0;
    EnBsb_ChangeAnim(this, ENBSB_ANIM_1);
    this->actionFunc = func_80C0C364;
}

void func_80C0C364(EnBsb* this, PlayState* play) {
    f32 sp2C;
    s16 sp2A = this->actor.yawTowardsPlayer;
    s32 yaw;

    this->path = SubS_GetPathByIndex(play, this->pathIndex, ENBSB_PATH_INDEX_NONE);

    func_80C0BC30(this);

    if (this->path != NULL) {
        sp2A = SubS_GetDistSqAndOrientPath(this->path, this->waypoint, &this->actor.world.pos, &sp2C);
    }

    this->actor.world.rot.y -= 750;

    yaw = ABS_ALT((s16)(this->actor.world.rot.y - sp2A));

    func_80C0B970(this, play);

    if (yaw < 0x1000) {
        this->unk_02BC = this->actor.world.rot.y;
        func_80C0C430(this);
    }
}

void func_80C0C430(EnBsb* this) {
    this->unk_02A4 = 0;
    if (this->animIndex != ENBSB_ANIM_1) {
        EnBsb_ChangeAnim(this, ENBSB_ANIM_1);
    }
    this->unk_0294 = 70;
    this->unk_02B4 = 2;
    this->actionFunc = func_80C0C484;
}

void func_80C0C484(EnBsb* this, PlayState* play) {
    f32 sp34;
    s16 var_a1;
    s16 temp_v1;

    func_80C0BC30(this);

    if (func_80C0B888(this, play)) {
        this->actor.flags &= ~ACTOR_FLAG_UPDATE_DURING_OCARINA;
        func_80C0C86C(this);
        return;
    }

    var_a1 = this->actor.yawTowardsPlayer;

    if (this->unk_0294 == 1) {
        this->actor.flags &= ~ACTOR_FLAG_UPDATE_DURING_OCARINA;
    }

    if (this->path != NULL) {
        var_a1 = SubS_GetDistSqAndOrientPath(this->path, this->waypoint, &this->actor.world.pos, &sp34);
    }

    Math_SmoothStepToS(&this->actor.world.rot.y, var_a1, 2, 0x2EE, 5);

    temp_v1 = ABS_ALT((s16)(this->actor.world.rot.y - var_a1));

    if (temp_v1 < 0x1000) {
        this->unk_02BC = this->actor.world.rot.y;
        this->actor.speed = 3.5f;
        if (!this->unk_02AF) {
            this->unk_02AF = true;
            Audio_PlayBgm_StorePrevBgm(NA_BGM_CHASE);
        }
    }

    func_80C0B970(this, play);

    if (sp34 < SQ(5.0f)) {
        if (this->path != NULL) {
            this->waypoint++;
            if (this->waypoint >= this->path->count) {
                this->waypoint--;
                func_80C0DA58(this);
            }
        }
    }
}

void func_80C0C610(EnBsb* this) {
    this->unk_02A4 = 0;
    this->actor.speed = 0.0f;

    EnBsb_ChangeAnim(this, ENBSB_ANIM_2);
    Actor_PlaySfx(&this->actor, NA_SE_EN_KITA_DAMAGE);

    this->unk_0294 = 0;

    if (this->unk_02DC) {
        this->unk_0294 = 40;
        Actor_PlaySfx(&this->actor, NA_SE_EN_COMMON_FREEZE);
        Actor_SetColorFilter(&this->actor, COLORFILTER_COLORFLAG_BLUE, 255, COLORFILTER_BUFFLAG_OPA, 40);
    }

    this->unk_02B4 = 3;
    this->actionFunc = func_80C0C6A8;
}

void func_80C0C6A8(EnBsb* this, PlayState* play) {
    f32 curFrame = this->skelAnime.curFrame;

    func_80C0B888(this, play);

    if ((!this->unk_02DC || (this->unk_02DC && (this->animIndex == ENBSB_ANIM_2) && (curFrame >= this->animEndFrame) &&
                             (this->unk_0294 == 0))) &&
        ((this->drawDmgEffType == ACTOR_DRAW_DMGEFF_FROZEN_SFX) ||
         (this->drawDmgEffType == ACTOR_DRAW_DMGEFF_FROZEN_NO_SFX)) &&
        (this->drawDmgEffTimer != 0)) {
        Actor_SpawnIceEffects(play, &this->actor, this->bodyPartsPos, ENBSB_BODYPART_MAX, 2,
                              this->drawDmgEffFrozenSteamScale, 0.4f);
        this->drawDmgEffTimer = 0;
        this->drawDmgEffType = ACTOR_DRAW_DMGEFF_FIRE;
    }

    if ((this->animIndex == ENBSB_ANIM_2) && (curFrame >= this->animEndFrame) && (this->unk_0294 == 0)) {
        if (!this->unk_02DC) {
            EnBsb_ChangeAnim(this, ENBSB_ANIM_3);
        } else {
            func_80C0C86C(this);
        }
    } else if (this->animIndex == ENBSB_ANIM_3) {
        Math_SmoothStepToS(&this->actor.world.rot.y, this->actor.yawTowardsPlayer, 1, 0x7D0, 0);
        if (Animation_OnFrame(&this->skelAnime, 7.0f) || Animation_OnFrame(&this->skelAnime, 15.0f)) {
            Actor_PlaySfx(&this->actor, NA_SE_EN_KTIA_PAUSE_K);
        }

        if (curFrame >= this->animEndFrame) {
            this->unk_02A4++;
            if (this->unk_02A4 >= 2) {
                func_80C0C430(this);
            }
        }
    }
}

void func_80C0C86C(EnBsb* this) {
    this->unk_02A4 = 0;
    this->unk_02DC = true;
    this->actor.speed = 2.0f;
    EnBsb_ChangeAnim(this, ENBSB_ANIM_4);
    this->unk_0294 = Rand_S16Offset(0, 30);
    SET_WEEKEVENTREG(WEEKEVENTREG_85_40);
    this->unk_02B4 = 4;
    this->actionFunc = func_80C0C8EC;
}

void func_80C0C8EC(EnBsb* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    if (!func_80C0BC30(this)) {
        Math_SmoothStepToS(&this->actor.world.rot.y, this->actor.yawTowardsPlayer, 1, 0xBB8, 0);
    }

    if ((this->unk_0294 == 0) && (fabsf(player->actor.world.pos.y - this->actor.world.pos.y) >= 100.0f) &&
        (this->actor.xzDistToPlayer < 500.0f)) {
        func_80C0CA28(this, play);
    } else {
        func_80C0BA58(this, play);

        if ((this->unk_0294 == 0) && Actor_IsFacingPlayer(&this->actor, 0x11C7) &&
            (fabsf(this->actor.world.pos.y - player->actor.world.pos.y) < 20.0f) &&
            (this->actor.xzDistToPlayer < 180.0f)) {
            func_80C0CD90(this);
        }
    }
}

void func_80C0CA28(EnBsb* this, PlayState* play) {
    this->unk_02A4 = 0;
    this->actor.speed = 0.0f;

    Actor_PlaySfx(&this->actor, NA_SE_EN_KITA_ATTACK_W);

    this->actor.velocity.y = 85.0f;
    this->actor.gravity = -8.0f;

    EnBsb_ChangeAnim(this, ENBSB_ANIM_11);
    Actor_SpawnFloorDustRing(play, &this->actor, &this->unk_0304, 5.0f, 10, 8.0f, 1000, 100, true);
    Actor_SpawnFloorDustRing(play, &this->actor, &this->unk_02F8, 5.0f, 10, 8.0f, 1000, 100, true);
    Actor_PlaySfx(&this->actor, NA_SE_EN_TEKU_JUMP);

    this->actor.flags |= ACTOR_FLAG_LOCK_ON_DISABLED;
    this->unk_02B4 = 5;
    this->actionFunc = func_80C0CB3C;
}

void func_80C0CB3C(EnBsb* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    if (this->actor.velocity.y > 0.0f) {
        Math_ApproachF(&this->actor.world.pos.x, player->actor.world.pos.x, 0.5f, 30.0f);
        Math_ApproachF(&this->actor.world.pos.z, player->actor.world.pos.z, 0.5f, 30.0f);
        return;
    }

    this->actor.gravity = -8.0f;

    if (this->animIndex != ENBSB_ANIM_22) {
        EnBsb_ChangeAnim(this, ENBSB_ANIM_22);
    }

    if ((fabsf(player->actor.world.pos.y - this->actor.world.pos.y) < 40.0f) && (this->actor.xzDistToPlayer < 70.0f) &&
        (player->invincibilityTimer == 0)) {
        func_800B8D50(play, &this->actor, 2.0f, this->actor.yawTowardsPlayer, 5.0f, 16);
    }

    if (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND) {
        Actor_SpawnFloorDustRing(play, &this->actor, &this->actor.world.pos, this->actor.shape.shadowScale, 10, 8.0f,
                                 2000, 100, true);
        Actor_RequestQuakeAndRumble(&this->actor, play, 10, 10);
        Actor_PlaySfx(&this->actor, NA_SE_EN_EYEGOLE_ATTACK);
        func_80C0CCCC(this);
    }
}

void func_80C0CCCC(EnBsb* this) {
    EnBsb_ChangeAnim(this, ENBSB_ANIM_23);
    this->actionFunc = func_80C0CD04;
}

void func_80C0CD04(EnBsb* this, PlayState* play) {
    f32 curFrame = this->skelAnime.curFrame;

    if ((this->animIndex == ENBSB_ANIM_24) && (curFrame >= this->animEndFrame)) {
        this->actor.flags &= ~ACTOR_FLAG_LOCK_ON_DISABLED;
        this->actor.gravity = -2.0f;
        this->unk_0294 = 10;
        func_80C0C86C(this);
    } else if (this->animIndex == ENBSB_ANIM_23) {
        EnBsb_ChangeAnim(this, ENBSB_ANIM_24);
    }
}

void func_80C0CD90(EnBsb* this) {
    this->unk_02A4 = 0;
    this->actor.speed = 0.0f;
    Actor_PlaySfx(&this->actor, NA_SE_EN_KITA_ATTACK_W);
    EnBsb_ChangeAnim(this, ENBSB_ANIM_5);
    this->unk_02B4 = 6;
    this->actionFunc = func_80C0CDE4;
}

void func_80C0CDE4(EnBsb* this, PlayState* play) {
    f32 dist;
    f32 curFrame = this->skelAnime.curFrame;
    Vec3f hitPos;
    Player* player = GET_PLAYER(play);
    f32 dx;
    f32 dy;
    f32 dz;

    hitPos.x = this->collider.elements[1].base.acDmgInfo.hitPos.x;
    hitPos.y = this->collider.elements[1].base.acDmgInfo.hitPos.y;
    hitPos.z = this->collider.elements[1].base.acDmgInfo.hitPos.z;

    dx = hitPos.x - player->actor.world.pos.x;
    dy = hitPos.y - player->actor.world.pos.y;
    dz = hitPos.z - player->actor.world.pos.z;

    dist = sqrtf((SQ(dx) + SQ(dy)) + SQ(dz));

    if ((this->collider.base.atFlags & AT_BOUNCED) ||
        ((player->stateFlags1 & PLAYER_STATE1_400000) && (dist <= 70.0f) &&
         (BINANG_ROT180(player->actor.shape.rot.y - this->actor.shape.rot.y) < 0x2000) &&
         (BINANG_ROT180(player->actor.shape.rot.y - this->actor.shape.rot.y) > -0x2000))) {
        this->collider.base.atFlags &= ~(AT_BOUNCED | AT_HIT);
        EffectSsHitmark_SpawnFixedScale(play, EFFECT_HITMARK_METAL, &hitPos);
        Actor_PlaySfx(&this->actor, NA_SE_IT_SHIELD_BOUND);
        func_80C0CF4C(this);
    } else if (curFrame >= this->animEndFrame) {
        func_80C0C86C(this);
    }
}

void func_80C0CF4C(EnBsb* this) {
    this->actor.speed = 0.0f;
    this->unk_02A4 = 0;
    Animation_Change(&this->skelAnime, &object_bsb_Anim_000C50, -1.0f, this->skelAnime.curFrame - 1.0f, 0.0f,
                     ANIMMODE_ONCE, 0.0f);
    this->unk_0294 = 10;
    Actor_PlaySfx(&this->actor, NA_SE_EN_KTIA_PAUSE_K);
    this->unk_02B4 = 7;
    this->actionFunc = func_80C0CFDC;
}

void func_80C0CFDC(EnBsb* this, PlayState* play) {
    if (this->unk_0294 == 0) {
        func_80C0C86C(this);
    }
}

void func_80C0D00C(EnBsb* this) {
    Vec3f vec;

    this->unk_02A4 = 0;
    this->actor.speed = 0.0f;

    if (this->actor.world.pos.z < -1300.0f) {
        Matrix_RotateYS((s32)Rand_CenteredFloat(0x4000) + this->actor.yawTowardsPlayer, MTXMODE_NEW);
        Matrix_MultVecZ(-30.0f, &vec);
    } else {
        Matrix_RotateYS(this->actor.yawTowardsPlayer, MTXMODE_NEW);
        Matrix_MultVecZ(-50.0f, &vec);
        vec.z = 0.0f;
    }

    Math_Vec3f_Copy(&this->unk_0298, &vec);
    Actor_PlaySfx(&this->actor, NA_SE_EN_KITA_DAMAGE);
    EnBsb_ChangeAnim(this, ENBSB_ANIM_7);

    this->unk_02AE = true;
    this->unk_02B4 = 8;
    this->actionFunc = func_80C0D10C;
}

void func_80C0D10C(EnBsb* this, PlayState* play) {
    f32 curFrame = this->skelAnime.curFrame;

    if (((this->drawDmgEffType == ACTOR_DRAW_DMGEFF_FROZEN_SFX) ||
         (this->drawDmgEffType == ACTOR_DRAW_DMGEFF_FROZEN_NO_SFX)) &&
        (this->drawDmgEffTimer != 0)) {
        Actor_SpawnIceEffects(play, &this->actor, this->bodyPartsPos, ENBSB_BODYPART_MAX, 2,
                              this->drawDmgEffFrozenSteamScale, 0.4f);
        this->drawDmgEffTimer = 0;
        this->drawDmgEffType = ACTOR_DRAW_DMGEFF_FIRE;
    }

    func_80C0B31C(play, this, &this->unk_0304);
    func_80C0B31C(play, this, &this->unk_02F8);

    if (curFrame >= this->animEndFrame) {
        if ((this->actor.world.pos.z > -1300.0f) || (this->actor.colChkInfo.health < 10)) {
            func_80C0CA28(this, play);
        } else {
            func_80C0C86C(this);
        }
    }
}

void func_80C0D214(EnBsb* this) {
    this->unk_0294 = 40;
    this->actor.speed = 0.0f;
    Actor_SetColorFilter(&this->actor, COLORFILTER_COLORFLAG_BLUE, 120, COLORFILTER_BUFFLAG_OPA, 40);
    Actor_PlaySfx(&this->actor, NA_SE_EN_COMMON_FREEZE);
    this->unk_02B4 = 10;
    this->actionFunc = func_80C0D27C;
}

void func_80C0D27C(EnBsb* this, PlayState* play) {
    if ((this->drawDmgEffType == ACTOR_DRAW_DMGEFF_FROZEN_SFX) && (this->drawDmgEffTimer != 0) &&
        (this->drawDmgEffTimer < 60)) {
        this->drawDmgEffType = ACTOR_DRAW_DMGEFF_FROZEN_NO_SFX;
    }

    if (((this->drawDmgEffType == ACTOR_DRAW_DMGEFF_FROZEN_SFX) ||
         (this->drawDmgEffType == ACTOR_DRAW_DMGEFF_FROZEN_NO_SFX)) &&
        (this->drawDmgEffTimer != 0)) {
        Actor_SpawnIceEffects(play, &this->actor, this->bodyPartsPos, ENBSB_BODYPART_MAX, 2,
                              this->drawDmgEffFrozenSteamScale, 0.4f);
        this->drawDmgEffTimer = 0;
        this->drawDmgEffType = ACTOR_DRAW_DMGEFF_FIRE;
    }

    if (this->unk_0294 == 0) {
        func_80C0C86C(this);
    }
}

void func_80C0D334(EnBsb* this) {
    this->actor.speed = 0.0f;
    Actor_PlaySfx(&this->actor, NA_SE_EN_KITA_LAUGH_K);
    EnBsb_ChangeAnim(this, ENBSB_ANIM_6);
    this->unk_02B4 = 11;
    this->actionFunc = func_80C0D384;
}

void func_80C0D384(EnBsb* this, PlayState* play) {
    f32 curFrame = this->skelAnime.curFrame;

    if (curFrame >= this->animEndFrame) {
        func_80C0C86C(this);
    }
}

void func_80C0D3C0(EnBsb* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    Audio_RestorePrevBgm();

    this->unk_02A4 = 0;
    this->unk_02A8 = 0;
    this->unk_02AC = 0x4000;
    this->actor.speed = 0.0f;

    player->actor.world.rot.y = player->actor.shape.rot.y = this->actor.yawTowardsPlayer + 0x8000;

    this->subCamEye.x = this->subCamEyeNext.x = this->unk_02E0.x + Math_SinS(this->actor.yawTowardsPlayer) * 300.0f;
    this->subCamEye.y = this->subCamEyeNext.y = this->unk_02E0.y - 30.0f;
    this->subCamEye.z = this->subCamEyeNext.z = this->unk_02E0.z + Math_CosS(this->actor.yawTowardsPlayer) * 300.0f;

    this->subCamAt.x = this->subCamAtNext.x = this->unk_02E0.x + Math_SinS(this->actor.yawTowardsPlayer) * 10.0f;
    this->subCamAt.y = this->subCamAtNext.y = this->unk_02E0.y - 10.0f;
    this->subCamAt.z = this->subCamAtNext.z = this->unk_02E0.z + Math_CosS(this->actor.yawTowardsPlayer) * 10.0f;

    this->subCamFovNext = 60.0f;
    EnBsb_ChangeAnim(this, ENBSB_ANIM_2);
    this->unk_02B4 = 9;
    this->actionFunc = func_80C0D51C;
    this->actor.velocity.y = 30.0f;
}

void func_80C0D51C(EnBsb* this, PlayState* play) {
    f32 curFrame = this->skelAnime.curFrame;
    s32 yaw;

    if (((this->drawDmgEffType == ACTOR_DRAW_DMGEFF_FROZEN_SFX) ||
         (this->drawDmgEffType == ACTOR_DRAW_DMGEFF_FROZEN_NO_SFX)) &&
        (this->drawDmgEffTimer != 0)) {
        Actor_SpawnIceEffects(play, &this->actor, this->bodyPartsPos, ENBSB_BODYPART_MAX, 2,
                              this->drawDmgEffFrozenSteamScale, 0.4f);
        this->drawDmgEffTimer = 0;
        this->drawDmgEffType = ACTOR_DRAW_DMGEFF_FIRE;
    }

    if (this->unk_02A4 == 0) {
        if (!CutsceneManager_IsNext(this->csIdList[1])) {
            CutsceneManager_Queue(this->csIdList[1]);
            return;
        }
        CutsceneManager_StartWithPlayerCs(this->csIdList[1], &this->actor);
        Player_SetCsActionWithHaltedActors(play, &this->actor, PLAYER_CSACTION_81);
        this->subCamId = CutsceneManager_GetCurrentSubCamId(this->actor.csId);
        this->unk_02A4 = 1;
    }

    if (this->unk_02A8 == 2) {
        f32 sin = Math_SinS(this->actor.world.rot.y) * 380.f;
        f32 cos = Math_CosS(this->actor.world.rot.y) * 380.f;

        this->subCamEyeNext.x = this->unk_02E0.x + sin;
        this->subCamEyeNext.y = this->unk_02E0.y + 30.0f;
        this->subCamEyeNext.z = this->unk_02E0.z + cos;

        this->subCamAtNext.x = Math_SinS(this->actor.world.rot.y) * 10.0f + this->unk_02E0.x;
        this->subCamAtNext.y = this->unk_02E0.y - 63.0f;
        this->subCamAtNext.z = Math_CosS(this->actor.world.rot.y) * 10.0f + this->unk_02E0.z;
    } else {
        this->subCamAtNext.x = Math_SinS(this->actor.world.rot.y) * 10.0f + this->unk_02E0.x;
        this->subCamAtNext.y = this->unk_02E0.y - 10.0f;
        this->subCamAtNext.z = Math_CosS(this->actor.world.rot.y) * 10.0f + this->unk_02E0.z;
    }

    if (this->unk_02A8 == 0) {
        this->actor.world.rot.y += this->unk_02AC;
        Math_SmoothStepToS(&this->unk_02AC, 0x1000, 1, 0x1F4, 0);
        yaw = ABS_ALT((s16)(this->actor.yawTowardsPlayer - this->actor.world.rot.y));

        if ((this->actor.velocity.y < -5.0f) && (yaw < 0x1000)) {
            this->unk_02A8 = 1;
        }
    } else {
        Math_SmoothStepToS(&this->actor.world.rot.y, this->actor.yawTowardsPlayer, 1, 0x3E8, 0);
        yaw = ABS_ALT((s16)(this->actor.yawTowardsPlayer - this->actor.world.rot.y));
    }

    if ((this->unk_02A8 != 0) && (yaw < 0x100) && (this->actor.world.pos.y < (this->actor.floorHeight + 30.0f))) {
        this->unk_02A8 = 2;
        if (this->animIndex < ENBSB_ANIM_9) {
            Player* player = GET_PLAYER(play);

            EnBsb_ChangeAnim(this, ENBSB_ANIM_9);
            Player_SetCsActionWithHaltedActors(play, &this->actor, PLAYER_CSACTION_4);
            player->actor.velocity.y = 5.0f;
            Actor_SpawnFloorDustRing(play, &this->actor, &this->actor.world.pos, this->actor.shape.shadowScale - 20.0f,
                                     20, 8.0f, 1000, 100, true);
            Actor_PlaySfx(&this->actor, NA_SE_EN_EYEGOLE_ATTACK);
            Actor_RequestQuakeAndRumble(&this->actor, play, 4, 10);
        } else if ((this->animIndex == ENBSB_ANIM_9) && (curFrame >= this->animEndFrame)) {
            EnBsb_ChangeAnim(this, ENBSB_ANIM_10);
            Player_SetCsActionWithHaltedActors(play, &this->actor, PLAYER_CSACTION_81);
        }
    }
    func_80C0BE1C(this, play);
    if ((this->unk_02A4 != 0) && (this->unk_02A8 != 0) && (this->animIndex == ENBSB_ANIM_10) &&
        (curFrame >= this->animEndFrame)) {
        func_80C0D964(this, play);
    }
}

void func_80C0D964(EnBsb* this, PlayState* play) {
    this->unk_02A4 = 0;
    this->unk_02A8 = 0;
    this->actor.textId = 0x1535;
    if (GameInteractor_Should(VB_PLAY_DEFEAT_CAPTAIN_SEQUENCE, true)) {
        Message_StartTextbox(play, this->actor.textId, &this->actor);
        this->actionFunc = func_80C0D9B4;
    }
}

void func_80C0D9B4(EnBsb* this, PlayState* play) {
    if ((Message_GetState(&play->msgCtx) == TEXT_STATE_EVENT) && Message_ShouldAdvance(play)) {
        Message_CloseTextbox(play);
        play->nextEntrance = Entrance_CreateFromSpawn(5);
        gSaveContext.nextCutsceneIndex = 0;
        play->transitionTrigger = TRANS_TRIGGER_START;
        play->transitionType = TRANS_TYPE_FADE_BLACK;
        gSaveContext.nextTransitionType = TRANS_TYPE_FADE_WHITE;
        this->subCamId = SUB_CAM_ID_DONE;
    }
}

void func_80C0DA58(EnBsb* this) {
    this->actor.speed = 0.0f;
    this->unk_02A4 = 0;
    this->unk_1118 = this->unk_111C = 0;
    this->subCamId = SUB_CAM_ID_DONE;

    this->subCamFov = 0.0f;
    this->subCamFovNext = 60.0f;

    this->subCamEye.x = this->subCamEyeNext.x = -480.0f;
    this->subCamEye.y = this->subCamEyeNext.y = 375.0f;
    this->subCamEye.z = this->subCamEyeNext.z = -1630.0f;
    this->subCamAt.x = this->subCamAtNext.x = -360.0f;
    this->subCamAt.y = this->subCamAtNext.y = 500.0f;
    this->subCamAt.z = this->subCamAtNext.z = -2250.0f;
    this->unk_1118 = SUB_CAM_ID_DONE;
    SET_WEEKEVENTREG(WEEKEVENTREG_85_40);
    this->unk_02B4 = 13;
    this->actionFunc = func_80C0DB18;
}

void func_80C0DB18(EnBsb* this, PlayState* play) {
    Vec3f sp64 = { -80.0f, 475.0f, -2330.0f };
    f32 curFrame = this->skelAnime.curFrame;
    f32 sin;
    f32 cos;
    s32 pad;
    s16 yaw;
    s32 pad2;

    if (this->unk_02A4 == 0) {
        if (!CutsceneManager_IsNext(this->csIdList[3])) {
            CutsceneManager_Queue(this->csIdList[3]);
            return;
        }
        CutsceneManager_StartWithPlayerCs(this->csIdList[3], &this->actor);
        EnBsb_ChangeAnim(this, ENBSB_ANIM_1);
        Player_SetCsActionWithHaltedActors(play, &this->actor, PLAYER_CSACTION_WAIT);
        this->subCamId = CutsceneManager_GetCurrentSubCamId(this->actor.csId);
        this->unk_02A4 = 1;
    }

    if (this->animIndex == ENBSB_ANIM_1) {
        func_80C0B970(this, play);
    }

    this->subCamEye.x = this->subCamEyeNext.x = -480.0f;
    this->subCamEye.y = this->subCamEyeNext.y = 375.0f;
    this->subCamEye.z = this->subCamEyeNext.z = -1630.0f;

    this->subCamAt.x = this->subCamAtNext.x = -360.0f;
    this->subCamAt.y = this->subCamAtNext.y = 500.0f;
    this->subCamAt.z = this->subCamAtNext.z = -2250.0f;

    func_80C0BE1C(this, play);

    switch (this->unk_1118) {
        case 0:
            Math_SmoothStepToS(&this->actor.world.rot.y, Math_Vec3f_Yaw(&this->actor.world.pos, &sp64), 1, 0x7D0, 0);

            sin = Math_SinS(this->actor.world.rot.y);
            Math_ApproachF(&this->actor.world.pos.x, sp64.x, 0.5f, fabsf(sin * 3.2f));

            cos = Math_CosS(this->actor.world.rot.y);
            Math_ApproachF(&this->actor.world.pos.z, sp64.z, 0.5f, fabsf(cos * 3.2f));

            if (sqrtf(SQ(this->actor.world.pos.x - sp64.x) + SQ(this->actor.world.pos.z - sp64.z)) < 3.0f) {
                this->waypoint = this->path->count - 1;
                if (SubS_CopyPointFromPath(this->path, this->waypoint, &this->unk_02EC)) {
                    this->unk_02EC.y = this->actor.world.pos.y;
                    EnBsb_ChangeAnim(this, ENBSB_ANIM_3);
                    this->unk_1118++;
                }
            }
            break;

        case 1:
            if (Animation_OnFrame(&this->skelAnime, 7.0f) || Animation_OnFrame(&this->skelAnime, 15.0f)) {
                Actor_PlaySfx(&this->actor, NA_SE_EN_KTIA_PAUSE_K);
            }

            Math_SmoothStepToS(&this->actor.world.rot.y, Math_Vec3f_Yaw(&this->actor.world.pos, &this->unk_02EC), 1,
                               0x7D0, 0);

            if (curFrame >= this->animEndFrame) {
                this->unk_111C++;
                if (this->unk_111C >= 3) {
                    EnBsb_ChangeAnim(this, ENBSB_ANIM_1);
                    this->unk_111C = 0;
                    this->unk_1118++;
                }
            }
            break;

        case 2:
            yaw = Math_Vec3f_Yaw(&this->actor.world.pos, &this->subCamEye);
            Math_SmoothStepToS(&this->actor.world.rot.y, yaw, 1, 0x7D0, 0);

            if (ABS_ALT((s16)(this->actor.world.rot.y - yaw)) < 0x100) {
                Actor_PlaySfx(&this->actor, NA_SE_EN_KITA_LAUGH_K);
                EnBsb_ChangeAnim(this, ENBSB_ANIM_21);
                this->unk_1118++;
                Message_StartTextbox(play, 0x1541, &this->actor);
            }
            break;

        case 3:
            if ((Message_GetState(&play->msgCtx) == TEXT_STATE_EVENT) && Message_ShouldAdvance(play)) {
                Message_CloseTextbox(play);
                EnBsb_ChangeAnim(this, ENBSB_ANIM_1);
                this->unk_1118++;
            }
            break;

        case 4:
            sp64.x = -100.0f;
            yaw = Math_Vec3f_Yaw(&this->actor.world.pos, &sp64);
            Math_SmoothStepToS(&this->actor.world.rot.y, yaw, 1, 0x7D0, 0);

            if (ABS_ALT((s16)(this->actor.world.rot.y - yaw)) < 0x100) {
                Actor_PlaySfx(&this->actor, NA_SE_EN_TEKU_JUMP);
                this->actor.velocity.y = 20.0f;
                this->actor.speed = 3.0f;
                EnBsb_ChangeAnim(this, ENBSB_ANIM_11);
                this->unk_1118++;
            }
            break;

        case 5:
            if (this->actor.world.pos.y < (this->actor.home.pos.y + 30.0f)) {
                Actor_SpawnFloorDustRing(play, &this->actor, &this->actor.world.pos,
                                         this->actor.shape.shadowScale - 20.0f, 20, 8.0f, 1000, 100, true);
                this->actor.speed = 0.0f;
                Actor_PlaySfx(&this->actor, NA_SE_EN_GOMA_DOWN);
                Actor_RequestQuakeAndRumble(&this->actor, play, 4, 10);
                this->unk_1118++;
            }
            break;

        case 6:
            this->actor.world.rot.y += 0x2000;

            if ((play->gameplayFrames % 8) == 0) {
                Math_Vec3f_Copy(&sp64, &this->actor.world.pos);
                sp64.x += Rand_CenteredFloat(40.0f);
                sp64.z += Rand_CenteredFloat(40.0f);
                Actor_SpawnFloorDustRing(play, &this->actor, &sp64, Rand_ZeroFloat(10.0f) + 5.0f, Rand_S16Offset(1, 3),
                                         4.0f, 1000, 100, true);
            }

            Math_ApproachF(&this->actor.shape.yOffset, -7000.0f, 0.5f, 200.0f);

            if (this->actor.shape.yOffset < -6999.0f) {
                CutsceneManager_Stop(this->csIdList[3]);
                Actor_Kill(&this->actor);
            }
            break;

        default:
            break;
    }
}

void func_80C0E178(EnBsb* this) {
    this->actor.flags |= ACTOR_FLAG_LOCK_ON_DISABLED;
    this->unk_02AE = false;
    this->unk_02A4 = 0;
    this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
    this->unk_02B4 = 14;
    this->actionFunc = func_80C0E1C0;
    this->actor.speed = 0.0f;
}

void func_80C0E1C0(EnBsb* this, PlayState* play) {
    s32 i;
    f32 curFrame = this->skelAnime.curFrame;

    if (this->animIndex == ENBSB_ANIM_16) {
        if (Animation_OnFrame(&this->skelAnime, 30.0f)) {
            Actor_PlaySfx(&this->actor, NA_SE_EN_KITA_SALUTE);
        } else if ((curFrame < 30.0f) && ((play->csCtx.curFrame == 472) || (play->csCtx.curFrame == 487))) {
            Actor_PlaySfx(&this->actor, NA_SE_EN_KTIA_WALK);
        }
    }

    if ((this->animIndex == ENBSB_ANIM_18) && (play->csCtx.curFrame == 897)) {
        Actor_PlaySfx(&this->actor, NA_SE_EN_KITA_SALUTE);
    }

    if (this->unk_02A4 == 0) {
        if (CHECK_WEEKEVENTREG(WEEKEVENTREG_23_04)) {
            curFrame = this->skelAnime.curFrame;
            D_80C0F8D0 = curFrame;

            for (i = 0; i < ENBSB_BODYPART_MAX; i++) {
                EnBsb* newActor = (EnBsb*)Actor_Spawn(&play->actorCtx, play, ACTOR_EN_BSB, this->actor.world.pos.x,
                                                      this->actor.world.pos.y, this->actor.world.pos.z,
                                                      this->actor.shape.rot.x, this->actor.shape.rot.y,
                                                      this->actor.shape.rot.z, ENBSB_PARAMS2(this->unk_03FC[i]));
                if (newActor != NULL) {
                    newActor->unk_02C0 = this->bodyPartsPos[i].y;
                }
            }

            this->unk_02A4 = 1;
            this->actor.draw = NULL;
        }
    }

    if (play->csCtx.curFrame == 950) {
        Actor_PlaySfx(&this->actor, NA_SE_EN_GOMA_DOWN);
    }

    if (!func_80C0E9CC(this, play)) {
        Actor_Kill(&this->actor);
    }
}

void func_80C0E3B8(EnBsb* this) {
    this->actor.gravity = 0.0f;
    this->actor.speed = 0.0f;
    this->unk_02AE = false;
    this->unk_02A4 = 0;

    Math_Vec3s_Copy(&this->unk_031C, &gZeroVec3s);

    this->actor.flags |= ACTOR_FLAG_LOCK_ON_DISABLED;
    this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;

    Animation_Change(&this->skelAnime, &object_bsb_Anim_004894, 1.0f, D_80C0F8D0,
                     Animation_GetLastFrame(&object_bsb_Anim_004894), 2, 0.0f);
    SkelAnime_Update(&this->skelAnime);

    this->unk_02B4 = 15;
    this->actionFunc = func_80C0E480;
}

void func_80C0E480(EnBsb* this, PlayState* play) {
    if (this->unk_02C0 != 0.0f) {
        if (this->unk_02C0 <= this->actor.world.pos.y) {
            this->unk_02C0 = this->actor.world.pos.y - 40.0f;
        }
        this->unk_0294 = 2;
        this->alpha = 255;
        this->actionFunc = func_80C0E4FC;
        this->unk_02C0 = this->actor.home.pos.y + (this->actor.world.pos.y - this->unk_02C0);
    }
}

void func_80C0E4FC(EnBsb* this, PlayState* play) {
    if (this->unk_0294 != 0) {
        if (this->unk_0294 == 1) {
            this->actor.speed = 0.0f;
            this->actor.velocity.y = 10.0f;
        }
    } else {
        if (this->actor.velocity.y <= -11.0f) {
            this->alpha -= 48;
            if (this->alpha < 0) {
                this->alpha = 0;
            }
        }
        Math_ApproachF(&this->actor.gravity, -3.0f, 0.5f, 0.5f);
        Math_ApproachF(&this->actor.velocity.y, -10.0f, 0.5f, 0.5f);
        this->unk_031C.x += 0x320;
        this->unk_031C.y += 0x320;
        this->unk_031C.z += 0x320;
        if ((this->actor.gravity != 0.0f) && (this->actor.world.pos.y <= this->unk_02C0)) {
            Actor_Kill(&this->actor);
        }
    }
}

void func_80C0E618(EnBsb* this, PlayState* play) {
    s32 var_s0 = 0;
    Vec3f sp48;
    s32 i;

    if ((this->unk_02B4 != 0) && (this->unk_02B4 != 1) && (this->unk_02B4 != 9) && (this->unk_02B4 != 12) &&
        (this->unk_02B4 != 13) && (this->unk_02B4 != 5) && ((this->unk_02B4 != 8) || !this->unk_02DC)) {
        if (!(this->collider.base.atFlags & AT_BOUNCED)) {
            if (this->collider.elements[1].base.atElemFlags & ATELEM_HIT) {
                this->collider.elements[1].base.atElemFlags &= ~ATELEM_HIT;
                if ((this->unk_02B4 != 11) && (this->unk_02B4 != 7)) {
                    func_80C0D334(this);
                }
            }
        }
        if (this->collider.base.acFlags & AC_HIT) {
            switch (this->actor.colChkInfo.damageEffect) {
                case 14:
                    if (this->unk_02DC) {
                        var_s0 = 1;
                        break;
                    }
                    // fallthrough
                case 15:
                    var_s0 = -1;
                    break;

                case 13:
                    if (!this->unk_02DC) {
                        this->unk_02DC = true;
                        Audio_PlayBgm_StorePrevBgm(NA_BGM_MINI_BOSS);
                    }
                    var_s0 = 1;
                    break;

                case 12:
                    if (!this->unk_02DC) {
                        this->unk_02DC = true;
                        Audio_PlayBgm_StorePrevBgm(NA_BGM_MINI_BOSS);
                    }
                    break;

                case 1:
                    if (this->unk_02DC) {
                        func_80C0D214(this);
                    }
                    break;

                case 2:
                    this->drawDmgEffTimer = 40;
                    this->drawDmgEffType = ACTOR_DRAW_DMGEFF_FIRE;
                    var_s0 = -1;
                    break;

                case 3:
                    if (((this->drawDmgEffType != ACTOR_DRAW_DMGEFF_FROZEN_SFX) &&
                         (this->drawDmgEffType != ACTOR_DRAW_DMGEFF_FROZEN_NO_SFX)) ||
                        (this->drawDmgEffTimer == 0)) {
                        this->drawDmgEffTimer = 80;
                        this->drawDmgEffType = ACTOR_DRAW_DMGEFF_FROZEN_SFX;
                        this->drawDmgEffScale = 0.0f;
                        this->drawDmgEffFrozenSteamScale = 1.5f;
                        var_s0 = -1;
                    }
                    break;

                case 4:
                    this->drawDmgEffTimer = 40;
                    this->drawDmgEffType = ACTOR_DRAW_DMGEFF_LIGHT_ORBS;
                    Actor_Spawn(&play->actorCtx, play, ACTOR_EN_CLEAR_TAG, this->actor.focus.pos.x,
                                this->actor.focus.pos.y, this->actor.focus.pos.z, 0, 0, 0, CLEAR_TAG_LARGE_LIGHT_RAYS);
                    var_s0 = -1;
                    break;

                case 5:
                    this->drawDmgEffTimer = 40;
                    this->drawDmgEffType = ACTOR_DRAW_DMGEFF_ELECTRIC_SPARKS_LARGE;

                    if (!this->unk_02DC) {
                        this->unk_02DC = true;
                        Audio_PlayBgm_StorePrevBgm(NA_BGM_MINI_BOSS);
                    } else {
                        func_80C0D214(this);
                    }
                    break;

                default:
                    break;
            }
        }

        if (var_s0 == -1) {
            if (this->unk_02B4 != 3) {
                func_80C0C610(this);
            }
        } else if (var_s0 == 1) {
            Actor_SetColorFilter(&this->actor, COLORFILTER_COLORFLAG_RED, 255, COLORFILTER_BUFFLAG_OPA, 8);
            Actor_ApplyDamage(&this->actor);
            if (this->actor.colChkInfo.health <= 0) {
                Enemy_StartFinishingBlow(play, &this->actor);
                Actor_PlaySfx(&this->actor, NA_SE_EN_KITA_DEAD);
                this->actor.flags |= ACTOR_FLAG_LOCK_ON_DISABLED;
                this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
                Actor_ChangeCategory(play, &play->actorCtx, &this->actor, ACTORCAT_NPC);
                func_80C0D3C0(this, play);
            } else {
                for (i = 0; i < ARRAY_COUNT(this->colliderElements); i++) {
                    if (this->collider.elements[i].base.acElemFlags & ACELEM_HIT) {
                        sp48.x = this->collider.elements[i].base.acDmgInfo.hitPos.x;
                        sp48.y = this->collider.elements[i].base.acDmgInfo.hitPos.y;
                        sp48.z = this->collider.elements[i].base.acDmgInfo.hitPos.z;
                        CollisionCheck_BlueBlood(play, NULL, &sp48);
                    }
                }
                func_80C0D00C(this);
            }
        }
    }
}

s32 func_80C0E9CC(EnBsb* this, PlayState* play) {
    s32 cueChannel;

    if (Cutscene_IsCueInChannel(play, CS_CMD_ACTOR_CUE_532)) {
        cueChannel = Cutscene_GetCueChannel(play, CS_CMD_ACTOR_CUE_532);
        if (this->cueId != play->csCtx.actorCues[cueChannel]->id) {
            if (this->unk_02B4 != 14) {
                func_80C0E178(this);
            }

            switch (play->csCtx.actorCues[cueChannel]->id) {
                case 1:
                    EnBsb_ChangeAnim(this, ENBSB_ANIM_13);
                    break;

                case 2:
                    if ((this->animIndex != ENBSB_ANIM_14) && (this->animIndex != ENBSB_ANIM_15)) {
                        EnBsb_ChangeAnim(this, ENBSB_ANIM_14);
                    }
                    break;

                case 3:
                    if ((this->animIndex != ENBSB_ANIM_16) && (this->animIndex != ENBSB_ANIM_17)) {
                        EnBsb_ChangeAnim(this, ENBSB_ANIM_16);
                    }
                    break;

                case 4:
                    if ((this->animIndex != ENBSB_ANIM_18) && (this->animIndex != ENBSB_ANIM_19)) {
                        EnBsb_ChangeAnim(this, ENBSB_ANIM_18);
                    }
                    break;

                case 5:
                    SET_WEEKEVENTREG(WEEKEVENTREG_23_04);
                    Actor_PlaySfx(&this->actor, NA_SE_EN_KITA_BREAK);
                    break;

                case 6:
                    Flags_SetSwitch(play, this->switchFlag2);
                    Actor_PlaySfx(&this->actor, NA_SE_SY_PIECE_OF_HEART);
                    break;

                default:
                    break;
            }
        }
        this->cueId = play->csCtx.actorCues[cueChannel]->id;
        Cutscene_ActorTranslateAndYaw(&this->actor, play, cueChannel);
        return true;
    }

    return false;
}

void EnBsb_Update(Actor* thisx, PlayState* play) {
    EnBsb* this = (EnBsb*)thisx;
    s32 pad;

    DECR(this->unk_0292);

    DECR(this->unk_0294);

    DECR(this->drawDmgEffTimer);

    if ((this->unk_02B4 != 0) && (this->unk_02B4 != 10) && !CHECK_WEEKEVENTREG(WEEKEVENTREG_23_04) &&
        (this->unk_02B0 == OBJECT_BSB_LIMB_NONE)) {
        SkelAnime_Update(&this->skelAnime);
    }

    this->actionFunc(this, play);
    Actor_MoveWithGravity(&this->actor);

    if (this->unk_02B0 == OBJECT_BSB_LIMB_NONE) {
        func_80C0E618(this, play);
        if ((!func_80C0E9CC(this, play)) && (this->unk_02B4 != 14)) {
            this->actor.shape.rot.y = this->actor.world.rot.y;
            this->actor.focus.pos.x = this->unk_02E0.x + Math_SinS(this->actor.world.rot.y) * 20.0f;
            this->actor.focus.pos.y = this->unk_02E0.y;
            this->actor.focus.pos.z = this->unk_02E0.z + Math_CosS(this->actor.world.rot.y) * 20.0f;
            Math_Vec3s_Copy(&this->actor.focus.rot, &this->actor.world.rot);
            if (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND) {
                if (this->actor.world.pos.z < -1300.0f) {
                    this->actor.world.pos.z += this->unk_0298.z;
                }
                this->actor.world.pos.x += this->unk_0298.x;
                Math_ApproachZeroF(&this->unk_0298.x, 1.0f, 2.0f);
                Math_ApproachZeroF(&this->unk_0298.z, 1.0f, 2.0f);
            }
        } else {
            f32 curFrame = this->skelAnime.curFrame;

            if ((this->animIndex == ENBSB_ANIM_14) && (curFrame >= this->animEndFrame)) {
                EnBsb_ChangeAnim(this, ENBSB_ANIM_15);
            }
            if ((this->animIndex == ENBSB_ANIM_16) && (curFrame >= this->animEndFrame)) {
                EnBsb_ChangeAnim(this, ENBSB_ANIM_17);
            }
            if ((this->animIndex == ENBSB_ANIM_18) && (curFrame >= this->animEndFrame)) {
                EnBsb_ChangeAnim(this, ENBSB_ANIM_19);
            }
        }

        Actor_UpdateBgCheckInfo(play, &this->actor, 30.0f, 60.0f, 40.0f,
                                UPDBGCHECKINFO_FLAG_1 | UPDBGCHECKINFO_FLAG_4 | UPDBGCHECKINFO_FLAG_8 |
                                    UPDBGCHECKINFO_FLAG_10);
        EnBsb_UpdateEffects(this, play);

        if (!func_80C0E9CC(this, play)) {
            if ((this->unk_02B4 != 9) && (this->unk_02B4 != 14) && (this->unk_02B4 != 15)) {
                CollisionCheck_SetAC(play, &play->colChkCtx, &this->collider.base);
                CollisionCheck_SetOC(play, &play->colChkCtx, &this->collider.base);
                if ((this->unk_02B4 == 6) || (this->unk_02B4 == 4)) {
                    CollisionCheck_SetAT(play, &play->colChkCtx, &this->collider.base);
                }
            }
        }
    }
}

s32 EnBsb_OverrideLimbDrawOpa(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, Actor* thisx) {
    EnBsb* this = (EnBsb*)thisx;

    OPEN_DISPS(play->state.gfxCtx);

    if (this->unk_02B0 == OBJECT_BSB_LIMB_NONE) {
        if (limbIndex == OBJECT_BSB_LIMB_0B) {
            if (this->unk_02B4 == 0) {
                *dList = NULL;
            } else {
                gDPSetEnvColor(POLY_OPA_DISP++, 255, 255, 255, this->alpha);
            }
        } else {
            gDPSetEnvColor(POLY_OPA_DISP++, 255, 255, 255, 255);
        }

        if (limbIndex == OBJECT_BSB_LIMB_07) {
            rot->x += this->unk_0310.x;
            rot->y += this->unk_0310.y;
            rot->z += this->unk_0310.z;
        }

        if (limbIndex == OBJECT_BSB_LIMB_04) {
            rot->x += this->unk_0316.x;
            rot->y += this->unk_0316.y;
            rot->z += this->unk_0316.z;
        }
    } else if (limbIndex != this->unk_02B0) {
        *dList = NULL;
    } else if ((limbIndex == OBJECT_BSB_LIMB_02) || (limbIndex == OBJECT_BSB_LIMB_03) ||
               (limbIndex == OBJECT_BSB_LIMB_04) || (limbIndex == OBJECT_BSB_LIMB_05) ||
               (limbIndex == OBJECT_BSB_LIMB_06) || (limbIndex == OBJECT_BSB_LIMB_07) ||
               (limbIndex == OBJECT_BSB_LIMB_08) || (limbIndex == OBJECT_BSB_LIMB_09) ||
               (limbIndex == OBJECT_BSB_LIMB_0A) || (limbIndex == OBJECT_BSB_LIMB_0C) ||
               (limbIndex == OBJECT_BSB_LIMB_0D) || (limbIndex == OBJECT_BSB_LIMB_0F) ||
               (limbIndex == OBJECT_BSB_LIMB_10) || (limbIndex == OBJECT_BSB_LIMB_11) ||
               (limbIndex == OBJECT_BSB_LIMB_12) || (limbIndex == OBJECT_BSB_LIMB_13) ||
               (limbIndex == OBJECT_BSB_LIMB_14)) {
        rot->x += this->unk_031C.x;
        rot->y += this->unk_031C.y;
        rot->z += this->unk_031C.z;
    }

    CLOSE_DISPS(play->state.gfxCtx);

    return false;
}

s32 EnBsb_OverrideLimbDrawXlu(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, Actor* thisx,
                              Gfx** gfx) {
    EnBsb* this = (EnBsb*)thisx;

    if (limbIndex != this->unk_02B0) {
        *dList = NULL;
    } else if (limbIndex != this->unk_02B0) {
        *dList = NULL;
    } else if ((limbIndex == OBJECT_BSB_LIMB_02) || (limbIndex == OBJECT_BSB_LIMB_03) ||
               (limbIndex == OBJECT_BSB_LIMB_04) || (limbIndex == OBJECT_BSB_LIMB_05) ||
               (limbIndex == OBJECT_BSB_LIMB_06) || (limbIndex == OBJECT_BSB_LIMB_07) ||
               (limbIndex == OBJECT_BSB_LIMB_08) || (limbIndex == OBJECT_BSB_LIMB_09) ||
               (limbIndex == OBJECT_BSB_LIMB_0A) || (limbIndex == OBJECT_BSB_LIMB_0C) ||
               (limbIndex == OBJECT_BSB_LIMB_0D) || (limbIndex == OBJECT_BSB_LIMB_0F) ||
               (limbIndex == OBJECT_BSB_LIMB_10) || (limbIndex == OBJECT_BSB_LIMB_11) ||
               (limbIndex == OBJECT_BSB_LIMB_12) || (limbIndex == OBJECT_BSB_LIMB_13) ||
               (limbIndex == OBJECT_BSB_LIMB_14)) {
        rot->x += this->unk_031C.x;
        rot->y += this->unk_031C.y;
        rot->z += this->unk_031C.z;
    }

    return false;
}

void EnBsb_PostLimbDrawOpa(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot, Actor* thisx) {
    EnBsb* this = (EnBsb*)thisx;

    if (this->unk_02B0 == OBJECT_BSB_LIMB_NONE) {
        if (limbIndex == OBJECT_BSB_LIMB_0A) {
            Matrix_MultVec3f(&gZeroVec3f, &this->unk_02E0);
        }
        if (limbIndex == OBJECT_BSB_LIMB_11) {
            Matrix_MultVec3f(&gZeroVec3f, &this->unk_0304);
        }
        if (limbIndex == OBJECT_BSB_LIMB_14) {
            Matrix_MultVec3f(&gZeroVec3f, &this->unk_02F8);
        }

        if ((limbIndex == OBJECT_BSB_LIMB_02) || (limbIndex == OBJECT_BSB_LIMB_03) ||
            (limbIndex == OBJECT_BSB_LIMB_04) || (limbIndex == OBJECT_BSB_LIMB_05) ||
            (limbIndex == OBJECT_BSB_LIMB_06) || (limbIndex == OBJECT_BSB_LIMB_07) ||
            (limbIndex == OBJECT_BSB_LIMB_08) || (limbIndex == OBJECT_BSB_LIMB_09) ||
            (limbIndex == OBJECT_BSB_LIMB_0A) || (limbIndex == OBJECT_BSB_LIMB_0C) ||
            (limbIndex == OBJECT_BSB_LIMB_0D) || (limbIndex == OBJECT_BSB_LIMB_0F) ||
            (limbIndex == OBJECT_BSB_LIMB_10) || (limbIndex == OBJECT_BSB_LIMB_11) ||
            (limbIndex == OBJECT_BSB_LIMB_12) || (limbIndex == OBJECT_BSB_LIMB_13) ||
            (limbIndex == OBJECT_BSB_LIMB_14)) {
            Matrix_MultZero(&this->bodyPartsPos[this->bodyPartIndex]);
            this->unk_03FC[this->bodyPartIndex] = limbIndex;

            this->bodyPartIndex++;
            if (this->bodyPartIndex >= ENBSB_BODYPART_MAX) {
                this->bodyPartIndex = 0;
            }
        }
    }
    Collider_UpdateSpheres(limbIndex, &this->collider);
}

void EnBsb_Draw(Actor* thisx, PlayState* play) {
    EnBsb* this = (EnBsb*)thisx;
    s32 pad;

    OPEN_DISPS(play->state.gfxCtx);

    EnBsb_DrawEffects(this, play);

    if (this->alpha == 255) {
        Gfx_SetupDL25_Opa(play->state.gfxCtx);
        Scene_SetRenderModeXlu(play, 0, 1);

        gDPPipeSync(POLY_OPA_DISP++);
        gDPSetEnvColor(POLY_OPA_DISP++, 255, 255, 255, this->alpha);

        SkelAnime_DrawOpa(play, this->skelAnime.skeleton, this->skelAnime.jointTable, EnBsb_OverrideLimbDrawOpa,
                          EnBsb_PostLimbDrawOpa, &this->actor);

        if (this->drawDmgEffTimer != 0) {
            f32 drawDmgEffAlpha = this->drawDmgEffTimer * 0.05f;

            if ((this->drawDmgEffType == ACTOR_DRAW_DMGEFF_FROZEN_SFX) ||
                (this->drawDmgEffType == ACTOR_DRAW_DMGEFF_FROZEN_NO_SFX)) {
                this->drawDmgEffScale += 0.3f;
                if (this->drawDmgEffScale > 1.0f) {
                    this->drawDmgEffScale = 1.0f;
                }
                Math_ApproachF(&this->drawDmgEffFrozenSteamScale, this->drawDmgEffScale, 0.1f, 0.04f);
            } else {
                this->drawDmgEffScale = 0.8f;
                this->drawDmgEffFrozenSteamScale = 0.8f;
            }

            Actor_DrawDamageEffects(play, &this->actor, this->bodyPartsPos, ENBSB_BODYPART_MAX, this->drawDmgEffScale,
                                    this->drawDmgEffFrozenSteamScale, drawDmgEffAlpha, this->drawDmgEffType);
        }
    } else {
        Gfx_SetupDL25_Xlu(play->state.gfxCtx);
        Scene_SetRenderModeXlu(play, 1, 2);

        gDPPipeSync(POLY_XLU_DISP++);
        gDPSetEnvColor(POLY_XLU_DISP++, 255, 255, 255, this->alpha);

        POLY_XLU_DISP = SkelAnime_Draw(play, this->skelAnime.skeleton, this->skelAnime.jointTable,
                                       EnBsb_OverrideLimbDrawXlu, NULL, &this->actor, POLY_XLU_DISP++);
    }

    CLOSE_DISPS(play->state.gfxCtx);
}

void EnBsb_InitializeEffect(EnBsb* this, Vec3f* pos, Vec3f* velocity, Vec3f* accel, f32 scale, s16 life) {
    EnBsbEffect* effect = this->effects;
    s16 i;

    for (i = 0; i < ENBSB_EFFECT_COUNT; i++, effect++) {
        if (!effect->isEnabled) {
            effect->isEnabled = true;
            effect->pos = *pos;
            effect->velocity = *velocity;
            effect->accel = *accel;
            effect->scale = scale;
            effect->life = life;
            effect->rot.x = TRUNCF_BINANG(Rand_CenteredFloat(0x7530));
            effect->rot.y = TRUNCF_BINANG(Rand_CenteredFloat(0x7530));
            effect->rot.z = TRUNCF_BINANG(Rand_CenteredFloat(0x7530));
            return;
        }
    }
}

void EnBsb_UpdateEffects(EnBsb* this, PlayState* play) {
    s32 i;
    EnBsbEffect* effect = this->effects;

    for (i = 0; i < ENBSB_EFFECT_COUNT; i++, effect++) {
        if (effect->isEnabled) {
            effect->rot.x += 0x100;
            effect->rot.z += 0x130;
            effect->pos.x += effect->velocity.x;
            effect->pos.y += effect->velocity.y;
            effect->pos.z += effect->velocity.z;

            effect->velocity.y += effect->accel.y;

            if (effect->life != 0) {
                effect->life--;
            } else {
                effect->isEnabled = false;
            }
        }
    }
}

void EnBsb_DrawEffects(EnBsb* this, PlayState* play) {
    GraphicsContext* gfxCtx = play->state.gfxCtx;
    s32 i;
    EnBsbEffect* effect = this->effects;

    OPEN_DISPS(play->state.gfxCtx);

    Gfx_SetupDL25_Opa(play->state.gfxCtx);

    for (i = 0; i < ENBSB_EFFECT_COUNT; i++, effect++) {
        if (effect->isEnabled) {
            Matrix_Push();
            Matrix_Translate(effect->pos.x, effect->pos.y, effect->pos.z, MTXMODE_NEW);
            Matrix_RotateXS(effect->rot.x, MTXMODE_APPLY);
            Matrix_RotateYS(effect->rot.y, MTXMODE_APPLY);
            Matrix_RotateZS(effect->rot.z, MTXMODE_APPLY);
            Matrix_Scale(effect->scale, effect->scale, effect->scale, MTXMODE_APPLY);

            MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, gfxCtx);

            gDPSetPrimColor(POLY_OPA_DISP++, 0, 0x80, 255, 255, 255, 255);

            gDPSetEnvColor(POLY_OPA_DISP++, 255, 255, 255, 255);

            gSPDisplayList(POLY_OPA_DISP++, gameplay_keep_DL_06AB30);

            Matrix_Pop();
        }
    }
    CLOSE_DISPS(play->state.gfxCtx);
}
