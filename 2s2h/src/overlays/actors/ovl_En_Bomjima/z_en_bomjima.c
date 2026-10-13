/*
 * File: z_en_bomjima.c
 * Overlay: ovl_En_Bomjima
 * Description: Bombers - Jim
 */

#include "z_en_bomjima.h"

#include "overlays/effects/ovl_Effect_Ss_Hitmark/z_eff_ss_hitmark.h"

#define FLAGS (ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_FRIENDLY | ACTOR_FLAG_UPDATE_CULLING_DISABLED)

void EnBomjima_Init(Actor* thisx, PlayState* play);
void EnBomjima_Destroy(Actor* thisx, PlayState* play);
void EnBomjima_Update(Actor* thisx, PlayState* play);
void EnBomjima_Draw(Actor* thisx, PlayState* play);

void func_80BFEA94(EnBomjima* this, PlayState* play);
void func_80BFEB1C(EnBomjima* this);
void func_80BFEB64(EnBomjima* this, PlayState* play);
void func_80BFEFF0(EnBomjima* this);
void func_80BFF03C(EnBomjima* this, PlayState* play);
void func_80BFF120(EnBomjima* this);
void func_80BFF174(EnBomjima* this, PlayState* play);
void func_80BFF430(EnBomjima* this, PlayState* play);
void func_80BFF52C(EnBomjima* this, PlayState* play);
void func_80BFF6CC(EnBomjima* this, PlayState* play);
void func_80BFF754(EnBomjima* this, PlayState* play);
void func_80BFF9B0(EnBomjima* this, PlayState* play);
void func_80BFFB40(EnBomjima* this, PlayState* play);
void func_80BFFBC4(EnBomjima* this, PlayState* play);
void func_80BFFCFC(EnBomjima* this);
void func_80BFFD48(EnBomjima* this, PlayState* play);
void func_80BFFE48(EnBomjima* this, PlayState* play);
void func_80BFFF54(EnBomjima* this, PlayState* play);
void func_80C00168(EnBomjima* this, PlayState* play);
void func_80C00234(EnBomjima* this);
void func_80C00284(EnBomjima* this, PlayState* play);

typedef enum EnBomjimaAction {
    /* 0 */ EN_BOMJIMA_ACTION_0,
    /* 1 */ EN_BOMJIMA_ACTION_1,
    /* 2 */ EN_BOMJIMA_ACTION_2,
    /* 3 */ EN_BOMJIMA_ACTION_3,
    /* 4 */ EN_BOMJIMA_ACTION_4,
    /* 5 */ EN_BOMJIMA_ACTION_5,
    /* 6 */ EN_BOMJIMA_ACTION_6,
    /* 7 */ EN_BOMJIMA_ACTION_7
} EnBomjimaAction;

static s32 D_80C009F0 = 0;
static s32 D_80C009F4 = 0;

ActorProfile En_Bomjima_Profile = {
    /**/ ACTOR_EN_BOMJIMA,
    /**/ ACTORCAT_NPC,
    /**/ FLAGS,
    /**/ OBJECT_CS,
    /**/ sizeof(EnBomjima),
    /**/ EnBomjima_Init,
    /**/ EnBomjima_Destroy,
    /**/ EnBomjima_Update,
    /**/ EnBomjima_Draw,
};

static ColliderCylinderInit sCylinderInit = {
    {
        COL_MATERIAL_NONE,
        AT_NONE,
        AC_NONE,
        OC1_ON | OC1_TYPE_PLAYER,
        OC2_TYPE_2,
        COLSHAPE_CYLINDER,
    },
    {
        ELEM_MATERIAL_UNK0,
        { 0x00000000, 0x00, 0x00 },
        { 0xF7CFFFFF, 0x00, 0x00 },
        ATELEM_NONE | ATELEM_SFX_NORMAL,
        ACELEM_NONE,
        OCELEM_ON,
    },
    { 10, 30, 0, { 0, 0, 0 } },
};

u16 D_80C00A44[] = { 0x719, 0x71A, 0x71B, 0x708 }; // text IDs before popping balloon

u16 D_80C00A4C[] = { 0x739, 0x73A, 0x73B, 0x000 }; // text IDs after popping balloon

u16 D_80C00A54[] = {
    0x739, 0x73A, 0x73B, 0x714, 0x709, 0x70A, 0x70B, 0x70C, 0x70D, 0x70E, 0x70F, 0x712, 0x713,
}; // text IDs for Deku after popping balloon

u16 D_80C00A70[] = {
    0x739, 0x73A, 0x73B, 0x759, 0x753, 0x754, 0x755, 0x756, 0x70D, 0x757, 0x758, 0x712, 0x713,
}; // text IDs for Human after popping balloon

u16 D_80C00A8C[] = { 0x736, 0x737, 0x738, 0x74E }; // text IDs after hide and seek

typedef enum {
    /*   -1 */ ENBOMJIMA_ANIM_NONE = -1,
    /* 0x00 */ ENBOMJIMA_ANIM_0,
    /* 0x01 */ ENBOMJIMA_ANIM_1,
    /* 0x02 */ ENBOMJIMA_ANIM_2,
    /* 0x03 */ ENBOMJIMA_ANIM_3,
    /* 0x04 */ ENBOMJIMA_ANIM_4,
    /* 0x05 */ ENBOMJIMA_ANIM_5,
    /* 0x06 */ ENBOMJIMA_ANIM_6,
    /* 0x07 */ ENBOMJIMA_ANIM_7,
    /* 0x08 */ ENBOMJIMA_ANIM_8,
    /* 0x09 */ ENBOMJIMA_ANIM_9,
    /* 0x0A */ ENBOMJIMA_ANIM_10,
    /* 0x0B */ ENBOMJIMA_ANIM_11,
    /* 0x0C */ ENBOMJIMA_ANIM_12,
    /* 0x0D */ ENBOMJIMA_ANIM_13,
    /* 0x0E */ ENBOMJIMA_ANIM_14,
    /* 0x0F */ ENBOMJIMA_ANIM_15,
    /* 0x10 */ ENBOMJIMA_ANIM_16,
    /* 0x11 */ ENBOMJIMA_ANIM_17,
    /* 0x12 */ ENBOMJIMA_ANIM_18,
    /* 0x13 */ ENBOMJIMA_ANIM_19,
    /* 0x16 */ ENBOMJIMA_ANIM_MAX
} EnBomjimaAnimation;

static AnimationHeader* sAnimations[ENBOMJIMA_ANIM_MAX] = {
    &gBomberIdleAnim,       // ENBOMJIMA_ANIM_0
    &object_cs_Anim_00FAF4, // ENBOMJIMA_ANIM_1
    &object_cs_Anim_0057C8, // ENBOMJIMA_ANIM_2
    &object_cs_Anim_0053F4, // ENBOMJIMA_ANIM_3
    &object_cs_Anim_002044, // ENBOMJIMA_ANIM_4
    &object_cs_Anim_01007C, // ENBOMJIMA_ANIM_5
    &object_cs_Anim_00349C, // ENBOMJIMA_ANIM_6
    &object_cs_Anim_004960, // ENBOMJIMA_ANIM_7
    &object_cs_Anim_005128, // ENBOMJIMA_ANIM_8
    &object_cs_Anim_004C1C, // ENBOMJIMA_ANIM_9
    &object_cs_Anim_001A1C, // ENBOMJIMA_ANIM_10
    &object_cs_Anim_003EE4, // ENBOMJIMA_ANIM_11
    &object_cs_Anim_00478C, // ENBOMJIMA_ANIM_12
    &object_cs_Anim_00433C, // ENBOMJIMA_ANIM_13
    &object_cs_Anim_0060E8, // ENBOMJIMA_ANIM_14
    &object_cs_Anim_001708, // ENBOMJIMA_ANIM_15
    &object_cs_Anim_005DC4, // ENBOMJIMA_ANIM_16
    &object_cs_Anim_0026B0, // ENBOMJIMA_ANIM_17
    &object_cs_Anim_0036B0, // ENBOMJIMA_ANIM_18
    &object_cs_Anim_0031C4, // ENBOMJIMA_ANIM_19
};

static u8 sAnimationModes[ENBOMJIMA_ANIM_MAX] = {
    ANIMMODE_LOOP, // ENBOMJIMA_ANIM_0
    ANIMMODE_LOOP, // ENBOMJIMA_ANIM_1
    ANIMMODE_LOOP, // ENBOMJIMA_ANIM_2
    ANIMMODE_LOOP, // ENBOMJIMA_ANIM_3
    ANIMMODE_LOOP, // ENBOMJIMA_ANIM_4
    ANIMMODE_LOOP, // ENBOMJIMA_ANIM_5
    ANIMMODE_ONCE, // ENBOMJIMA_ANIM_6
    ANIMMODE_ONCE, // ENBOMJIMA_ANIM_7
    ANIMMODE_LOOP, // ENBOMJIMA_ANIM_8
    ANIMMODE_LOOP, // ENBOMJIMA_ANIM_9
    ANIMMODE_LOOP, // ENBOMJIMA_ANIM_10
    ANIMMODE_ONCE, // ENBOMJIMA_ANIM_11
    ANIMMODE_LOOP, // ENBOMJIMA_ANIM_12
    ANIMMODE_LOOP, // ENBOMJIMA_ANIM_13
    ANIMMODE_LOOP, // ENBOMJIMA_ANIM_14
    ANIMMODE_ONCE, // ENBOMJIMA_ANIM_15
    ANIMMODE_LOOP, // ENBOMJIMA_ANIM_16
    ANIMMODE_ONCE, // ENBOMJIMA_ANIM_17
    ANIMMODE_LOOP, // ENBOMJIMA_ANIM_18
    ANIMMODE_LOOP, // ENBOMJIMA_ANIM_19
};

s16 D_80C00AF8[] = {
    0x4000, 60, 0x4000, 30, 0xC000, 30, 0xC000, 60,
};

void EnBomjima_Init(Actor* thisx, PlayState* play) {
    EnBomjima* this = (EnBomjima*)thisx;
    s32 csId;
    s32 i;

    this->actor.colChkInfo.mass = MASS_IMMOVABLE;
    ActorShape_Init(&this->actor.shape, 0.0f, ActorShadow_DrawCircle, 19.0f);
    this->actor.gravity = -3.0f;
    SkelAnime_InitFlex(play, &this->skelAnime, &object_cs_Skel_00F82C, &gBomberIdleAnim, this->jointTable,
                       this->morphTable, OBJECT_CS_LIMB_MAX);
    Collider_InitAndSetCylinder(play, &this->collider, &this->actor, &sCylinderInit);
    CLEAR_WEEKEVENTREG(WEEKEVENTREG_83_04);
    this->actor.attentionRangeType = ATTENTION_RANGE_0;
    this->unk_2E6 = ENBOMJIMA_GET_F0(&this->actor);
    this->unk_2E4 = ENBOMJIMA_GET_F(&this->actor);
    Actor_SetScale(&this->actor, 0.01f);

    if (this->unk_2E6 == 0) {
        csId = this->actor.csId;
        i = 0;

        while (csId != CS_ID_NONE) {
            // clang-format off
            this->csIdList[i] = csId; csId = CutsceneManager_GetAdditionalCsId(csId);
            // clang-format on
            i++;
        }

        this->actionFunc = func_80BFEA94;
    } else if (this->unk_2E6 == 2) {
        func_80BFFCFC(this);
    }

    if (CHECK_WEEKEVENTREG(WEEKEVENTREG_75_40) || CHECK_WEEKEVENTREG(WEEKEVENTREG_73_10) ||
        CHECK_WEEKEVENTREG(WEEKEVENTREG_85_02)) {
        Actor_Kill(&this->actor);
    }
}

void EnBomjima_Destroy(Actor* thisx, PlayState* play) {
    EnBomjima* this = (EnBomjima*)thisx;

    Collider_DestroyCylinder(play, &this->collider);
}

void func_80BFE32C(EnBomjima* this, PlayState* play, s32 arg2) {
    Player* player = GET_PLAYER(play);

    this->unk_2CA = arg2;
    if (player->transformation == PLAYER_FORM_GORON) {
        this->unk_2C8 = 1;
    }
    // #region 2S2H - Enhancements. Fierce Deity gets same dialog as Zora
    if (player->transformation == PLAYER_FORM_ZORA || player->transformation == PLAYER_FORM_FIERCE_DEITY) {
        this->unk_2C8 = 2;
    }
    // #endregion

    switch (this->unk_2CA) {
        case 0:
            if (player->transformation == PLAYER_FORM_DEKU) {
                this->actor.textId = 0x759;
                if (!CHECK_WEEKEVENTREG(WEEKEVENTREG_73_20)) {
                    this->actor.textId = 0x708;
                }
            } else if (player->transformation == PLAYER_FORM_HUMAN) {
                this->actor.textId = 0x75A;
                if (!CHECK_WEEKEVENTREG(WEEKEVENTREG_84_80)) {
                    this->actor.textId = 0x719;
                }
            } else if ((this->unk_2C8 == 1) || (this->unk_2C8 == 2)) {
                this->actor.textId = D_80C00A44[this->unk_2C8];
            }
            break;

        case 1:
            this->actor.textId = D_80C00A4C[this->unk_2C8];
            break;

        case 2:
            if (player->transformation == PLAYER_FORM_DEKU) {
                this->actor.textId = D_80C00A54[this->unk_2C8];
                return;
            }
            this->actor.textId = D_80C00A70[this->unk_2C8];
            break;

        case 3:
            this->actor.textId = D_80C00A8C[this->unk_2C8];
            break;

        default:
            break;
    }
}

void EnBomjima_ChangeAnim(EnBomjima* this, s32 animIndex, f32 playSpeed) {
    this->animIndex = animIndex;
    this->animEndFrame = Animation_GetLastFrame(sAnimations[animIndex]);
    Animation_Change(&this->skelAnime, sAnimations[this->animIndex], playSpeed, 0.0f, this->animEndFrame,
                     sAnimationModes[this->animIndex], -4.0f);
}

void func_80BFE524(EnBomjima* this) {
    if ((this->animIndex == ENBOMJIMA_ANIM_5) &&
        (Animation_OnFrame(&this->skelAnime, 9.0f) || Animation_OnFrame(&this->skelAnime, 10.0f) ||
         Animation_OnFrame(&this->skelAnime, 17.0f) || Animation_OnFrame(&this->skelAnime, 18.0f))) {
        Actor_PlaySfx(&this->actor, NA_SE_EV_BOMBERS_WALK);
    }

    if ((this->animIndex == ENBOMJIMA_ANIM_18) &&
        (Animation_OnFrame(&this->skelAnime, 0.0f) || Animation_OnFrame(&this->skelAnime, 2.0f) ||
         Animation_OnFrame(&this->skelAnime, 4.0f) || Animation_OnFrame(&this->skelAnime, 6.0f))) {
        Actor_PlaySfx(&this->actor, NA_SE_EV_BOMBERS_WALK);
    }

    if ((this->animIndex == ENBOMJIMA_ANIM_15) && Animation_OnFrame(&this->skelAnime, 15.0f)) {
        Actor_PlaySfx(&this->actor, NA_SE_EV_BOMBERS_LAND);
    }

    if ((this->animIndex == ENBOMJIMA_ANIM_6) && Animation_OnFrame(&this->skelAnime, 8.0f)) {
        Actor_PlaySfx(&this->actor, NA_SE_EV_BOMBERS_LAND);
    }
}

void func_80BFE65C(EnBomjima* this) {
    this->unk_2BC = 0;
    this->unk_2C0 = 0;
    this->unk_2BE = 0;
    this->unk_2A2 = 0;
    this->unk_290 = 0;
    this->unk_29A = 0;
}

void func_80BFE67C(EnBomjima* this, PlayState* play) {
    f32 curFrame = this->skelAnime.curFrame;
    f32 x;
    f32 z;
    s16 abs;
    s16 abs2;
    Vec3f sp6C;
    Vec3f sp60;
    Vec3f sp54;
    CollisionPoly* sp50;
    s32 bgId;

    this->unk_2DC = Math_Vec3f_Yaw(&this->actor.world.pos, &this->unk_2A4);
    Math_SmoothStepToS(&this->actor.world.rot.y, this->unk_2DC, 1, 0x1388, 0);

    switch (this->unk_2A2) {
        case 0:
            if (this->unk_2C0 == 0) {
                Math_Vec3f_Copy(&sp54, &this->actor.home.pos);

                sp54.x += Rand_CenteredFloat(150.0f);
                sp54.z += Rand_CenteredFloat(150.0f);

                abs = ABS_ALT(BINANG_SUB(this->actor.world.rot.y, Math_Vec3f_Yaw(&this->actor.world.pos, &sp54)));
                if ((abs < 0x4000) && !BgCheck_EntityLineTest1(&play->colCtx, &this->actor.world.pos, &sp54, &sp6C,
                                                               &sp50, true, false, false, true, &bgId)) {
                    EnBomjima_ChangeAnim(this, ENBOMJIMA_ANIM_5, 1.0f);
                    Math_Vec3f_Copy(&this->unk_2A4, &sp54);
                    this->unk_2BE = Rand_S16Offset(30, 50);
                    this->unk_2A2++;
                }
            }
            break;

        case 1:
            if (curFrame >= 0.0f) {
                this->unk_2DC = Math_Vec3f_Yaw(&this->actor.world.pos, &this->unk_2A4);
                Math_SmoothStepToS(&this->actor.world.rot.y, this->unk_2DC, 10, 0x7D0, 0x14);
            }

            abs2 = BINANG_SUB(this->actor.world.rot.y, this->unk_2DC);
            if ((s16)ABS_ALT(abs2) < 0x100) {
                Math_Vec3f_Copy(&sp60, &this->actor.world.pos);

                sp60.x += Math_SinS(this->actor.world.rot.y) * 60.0f;
                sp60.z += Math_CosS(this->actor.world.rot.y) * 60.0f;

                if (BgCheck_EntityLineTest1(&play->colCtx, &this->actor.world.pos, &sp60, &sp6C, &sp50, true, false,
                                            false, true, &bgId)) {
                    this->unk_2C0 = 0;
                    if (Rand_ZeroOne() < 0.5f) {
                        EnBomjima_ChangeAnim(this, ENBOMJIMA_ANIM_19, 1.0f);
                    } else {
                        EnBomjima_ChangeAnim(this, ENBOMJIMA_ANIM_0, 1.0f);
                    }
                    this->unk_2A2 = 0;
                    this->unk_2D0 = 0.0f;
                    break;
                }
            }

            x = this->unk_2A4.x - this->actor.world.pos.x;
            z = this->unk_2A4.z - this->actor.world.pos.z;

            if ((this->unk_2BE == 0) || (sqrtf(SQ(x) + SQ(z)) < 4.0f)) {
                this->unk_2C0 = Rand_S16Offset(20, 20);
                if (!(this->unk_2C0 & 1)) {
                    EnBomjima_ChangeAnim(this, ENBOMJIMA_ANIM_19, 1.0f);
                } else {
                    EnBomjima_ChangeAnim(this, ENBOMJIMA_ANIM_0, 1.0f);
                }
                this->unk_2A2 = 0;
                this->unk_2D0 = 0.0f;
            } else if (curFrame >= 0.0f) {
                Math_ApproachF(&this->actor.world.pos.x, this->unk_2A4.x, 0.3f, this->unk_2D0);
                Math_ApproachF(&this->actor.world.pos.z, this->unk_2A4.z, 0.3f, this->unk_2D0);
                Math_ApproachF(&this->unk_2D0, 1.0f, 0.3f, 0.5f);
            }
            break;
    }
}

void func_80BFEA94(EnBomjima* this, PlayState* play) {
    Actor* actor = play->actorCtx.actorLists[ACTORCAT_PROP].first;

    while (actor != NULL) {
        if (actor->id != ACTOR_EN_BOMBAL) {
            actor = actor->next;
            continue;
        }

        this->bombal = (EnBombal*)actor;
        Math_Vec3f_Copy(&this->unk_2B0, &actor->world.pos);

        if (this->bombalCsId == 0) {
            this->bombalCsId = this->bombal->actor.csId;
        }
        func_80BFEB1C(this);
        break;
    }
}

void func_80BFEB1C(EnBomjima* this) {
    EnBomjima_ChangeAnim(this, ENBOMJIMA_ANIM_1, 1.0f);
    func_80BFE65C(this);
    this->action = EN_BOMJIMA_ACTION_0;
    this->actionFunc = func_80BFEB64;
}

void func_80BFEB64(EnBomjima* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    s32 pad[2];
    Vec3f sp40;

    func_80BFE32C(this, play, 0);
    if (player->transformation == PLAYER_FORM_DEKU) {
        if (CHECK_WEEKEVENTREG(WEEKEVENTREG_73_20)) {
            this->unk_2C8 = 3;
            func_80BFE32C(this, play, 3);
        } else if (CHECK_WEEKEVENTREG(WEEKEVENTREG_77_02)) {
            this->unk_2C8 = 11;
            func_80BFE32C(this, play, 2);
        }
    } else if (player->transformation == PLAYER_FORM_HUMAN) {
        if (CHECK_WEEKEVENTREG(WEEKEVENTREG_84_80)) {
            this->unk_2C8 = 0;
            func_80BFE32C(this, play, 3);
        } else if (CHECK_WEEKEVENTREG(WEEKEVENTREG_85_01)) {
            this->unk_2C8 = 11;
            func_80BFE32C(this, play, 2);
        }
    }

    if (Text_GetFaceReaction(play, FACE_REACTION_SET_JIM) != 0) {
        this->actor.textId = Text_GetFaceReaction(play, FACE_REACTION_SET_JIM);
    }

    if (Actor_TalkOfferAccepted(&this->actor, &play->state)) {
        this->unk_2DC = this->actor.yawTowardsPlayer;
        func_80C00234(this);
        return;
    }

    if (CutsceneManager_GetCurrentCsId() == CS_ID_NONE) {
        Actor_OfferTalk(&this->actor, play, 70.0f);
    }

    if ((this->bombal->actor.update == NULL) || (this->bombal->actor.colChkInfo.health == 0)) {
        func_80BFEFF0(this);
        return;
    }

    switch (this->unk_2A2) {
        case 0:
            EnBomjima_ChangeAnim(this, ENBOMJIMA_ANIM_4, 1.0f);
            this->unk_29A = -7000;
            this->unk_2A2++;
            break;

        case 1:
            this->unk_2DC = Math_Vec3f_Yaw(&this->actor.world.pos, &this->bombal->actor.world.pos);
            if (Animation_OnFrame(&this->skelAnime, 19.0f)) {
                this->unk_2C0 = 5;
                Actor_PlaySfx(&this->actor, NA_SE_EV_BOMBERS_SHOT_BREATH);
            }

            if (this->unk_2C0 == 1) {
                s16 sp3E = Math_Vec3f_Yaw(&this->actor.world.pos, &this->bombal->actor.world.pos);

                if (Rand_ZeroOne() < 0.5f) {
                    sp3E += 0x4000;
                } else {
                    sp3E += 0xC000;
                }

                sp40.x = (Math_SinS(sp3E) * (Rand_ZeroFloat(20.0f) + 40.0f)) + this->bombal->actor.world.pos.x;
                sp40.y = this->bombal->actor.world.pos.y - Rand_CenteredFloat(40.0f);
                sp40.z = (Math_CosS(sp3E) * (Rand_ZeroFloat(20.0f) + 40.0f)) + this->bombal->actor.world.pos.z;

                SoundSource_PlaySfxAtFixedWorldPos(play, &sp40, 50, NA_SE_EV_BOMBERS_SHOT_EXPLOSUIN);
                EffectSsHitmark_SpawnFixedScale(play, EFFECT_HITMARK_WHITE, &sp40);
                this->unk_2BC++;

                if ((TRUNCF_BINANG(Rand_ZeroFloat(2.0f)) + 3) < this->unk_2BC) {
                    EnBomjima_ChangeAnim(this, ENBOMJIMA_ANIM_5, 1.0f);
                    this->unk_29A = 0;
                    Math_Vec3f_Copy(&this->unk_2A4, &this->actor.home.pos);
                    this->unk_2A4.x += Rand_CenteredFloat(150.0f);
                    this->unk_2A4.z += Rand_CenteredFloat(150.0f);
                    this->unk_2A2++;
                }
            }
            break;

        case 2:
            this->unk_2DC = Math_Vec3f_Yaw(&this->actor.world.pos, &this->unk_2A4);
            Math_ApproachF(&this->actor.world.pos.x, this->unk_2A4.x, 0.3f, 2.0f);
            Math_ApproachF(&this->actor.world.pos.z, this->unk_2A4.z, 0.3f, 2.0f);
            if (sqrtf(SQ(this->actor.world.pos.x - this->unk_2A4.x) + SQ(this->actor.world.pos.z - this->unk_2A4.z)) <
                4.0f) {
                func_80BFE65C(this);
            }
            break;
    }

    Math_SmoothStepToS(&this->actor.world.rot.y, this->unk_2DC, 1, 0x1388, 0);
}

void func_80BFEFF0(EnBomjima* this) {
    this->bombal = NULL;
    EnBomjima_ChangeAnim(this, ENBOMJIMA_ANIM_19, 1.0f);
    func_80BFE65C(this);
    this->action = EN_BOMJIMA_ACTION_1;
    this->actionFunc = func_80BFF03C;
}

void func_80BFF03C(EnBomjima* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    if (CutsceneManager_GetCurrentCsId() == CS_ID_GLOBAL_TALK) {
        CutsceneManager_Stop(CS_ID_GLOBAL_TALK);
        CutsceneManager_Queue(this->csIdList[0]);
    } else if (!CutsceneManager_IsNext(this->csIdList[0])) {
        CutsceneManager_Queue(this->csIdList[0]);
    } else {
        player->stateFlags1 &= ~PLAYER_STATE1_20;
        CLEAR_WEEKEVENTREG(WEEKEVENTREG_83_04);
        this->actor.world.rot.y = Camera_GetCamDirYaw(GET_ACTIVE_CAM(play));
        this->unk_2DC = Camera_GetCamDirYaw(GET_ACTIVE_CAM(play));
        CutsceneManager_StartWithPlayerCs(this->csIdList[0], &this->actor);
        func_80BFF120(this);
    }
}

void func_80BFF120(EnBomjima* this) {
    func_80BFE65C(this);
    this->cutsceneTimer = 30;
    EnBomjima_ChangeAnim(this, ENBOMJIMA_ANIM_6, 1.0f);
    this->cutsceneEnded = false;
    this->action = EN_BOMJIMA_ACTION_2;
    this->actionFunc = func_80BFF174;
}

void func_80BFF174(EnBomjima* this, PlayState* play) {
    f32 curFrame = this->skelAnime.curFrame;
    Player* player = GET_PLAYER(play);

    if (this->cutsceneTimer == 1) {
        CutsceneManager_Stop(this->csIdList[0]);
        this->cutsceneEnded = true;
    }

    if (Text_GetFaceReaction(play, FACE_REACTION_SET_JIM) != 0) {
        this->actor.textId = Text_GetFaceReaction(play, FACE_REACTION_SET_JIM);
    }

    if (Actor_TalkOfferAccepted(&this->actor, &play->state)) {
        this->unk_2DC = this->actor.yawTowardsPlayer;
        func_80C00234(this);
        return;
    }

    Math_SmoothStepToS(&this->actor.world.rot.y, this->unk_2DC, 1, 0x1388, 0);

    if ((curFrame >= this->animEndFrame) && (this->unk_2BC < 5)) {
        this->unk_2BC++;
        if (this->animIndex != ENBOMJIMA_ANIM_19) {
            EnBomjima_ChangeAnim(this, ENBOMJIMA_ANIM_19, 1.0f);
        }
    }

    if (player->transformation == PLAYER_FORM_DEKU) {
        if (CHECK_WEEKEVENTREG(WEEKEVENTREG_73_20)) {
            this->unk_2C8 = 3;
            func_80BFE32C(this, play, 3);
        } else {
            if (!CHECK_WEEKEVENTREG(WEEKEVENTREG_77_02)) {
                if (this->unk_2E8 == 0) {
                    this->unk_2C8 = 4;
                } else {
                    this->unk_2C8 = 3;
                }
            } else {
                this->unk_2C8 = 11;
            }
            func_80BFE32C(this, play, 2);
        }
    } else if (player->transformation == PLAYER_FORM_HUMAN) {
        if (CHECK_WEEKEVENTREG(WEEKEVENTREG_84_80)) {
            this->unk_2C8 = 0;
            func_80BFE32C(this, play, 3);
        } else {
            if (!CHECK_WEEKEVENTREG(WEEKEVENTREG_85_01)) {
                if (this->unk_2EA == 0) {
                    this->unk_2C8 = 4;
                } else {
                    this->unk_2C8 = 3;
                }
            } else {
                this->unk_2C8 = 11;
            }
            func_80BFE32C(this, play, 2);
        }
    } else if (this->unk_2CA != 1) {
        func_80BFE32C(this, play, 1);
    }

    if (this->cutsceneEnded != false) {
        if (this->unk_2BC >= 5) {
            func_80BFE67C(this, play);
        }
        Actor_OfferTalk(&this->actor, play, 70.0f);
    }
}

void func_80BFF3F0(EnBomjima* this) {
    EnBomjima_ChangeAnim(this, ENBOMJIMA_ANIM_15, 1.0f);
    this->action = EN_BOMJIMA_ACTION_3;
    this->actionFunc = func_80BFF430;
}

void func_80BFF430(EnBomjima* this, PlayState* play) {
    f32 curFrame = this->skelAnime.curFrame;

    if (curFrame >= this->animEndFrame) {
        EnBombal* bombal = (EnBombal*)Actor_Spawn(&play->actorCtx, play, ACTOR_EN_BOMBAL, this->unk_2B0.x,
                                                  this->unk_2B0.y, this->unk_2B0.z, 0, 0, 0, 0);

        if (bombal != NULL) {
            bombal->scale = 0.0f;
            bombal->csId = this->bombalCsId;
            Actor_ChangeFocus(&this->actor, play, &bombal->actor);
            CLEAR_WEEKEVENTREG(WEEKEVENTREG_83_04);
            func_80BFE65C(this);
            Message_CloseTextbox(play);
            this->actionFunc = func_80BFEA94;
        }
    }
}

void func_80BFF4F4(EnBomjima* this) {
    func_80BFE65C(this);
    this->action = EN_BOMJIMA_ACTION_4;
    this->actionFunc = func_80BFF52C;
}

void func_80BFF52C(EnBomjima* this, PlayState* play) {
    if ((Message_GetState(&play->msgCtx) == TEXT_STATE_CHOICE) && Message_ShouldAdvance(play)) {
        Message_CloseTextbox(play);
        if (play->msgCtx.choiceIndex == 0) {
            Player* player = GET_PLAYER(play);

            Audio_PlaySfx_MessageDecide();
            func_80BFE65C(this);
            this->unk_28E = 0;
            this->unk_29A = 0;
            this->unk_2C8 = 8;
            if (player->transformation == PLAYER_FORM_DEKU) {
                this->actor.textId = D_80C00A54[this->unk_2C8];
            } else {
                this->actor.textId = D_80C00A70[this->unk_2C8];
            }
            Message_ContinueTextbox(play, this->actor.textId);
            Audio_PlaySfx(NA_SE_SY_FOUND);
            EnBomjima_ChangeAnim(this, ENBOMJIMA_ANIM_15, 1.0f);
            this->action = EN_BOMJIMA_ACTION_5;
            this->actionFunc = func_80BFF6CC;
        } else {
            Player* player = GET_PLAYER(play);

            Audio_PlaySfx_MessageCancel();
            func_80BFE65C(this);
            this->unk_2C8 = 10;
            if (player->transformation == PLAYER_FORM_DEKU) {
                this->actor.textId = D_80C00A54[this->unk_2C8];
                this->unk_2E8 = 1;
            } else {
                this->actor.textId = D_80C00A70[this->unk_2C8];
                this->unk_2EA = 1;
            }
            Message_ContinueTextbox(play, this->actor.textId);
            func_80C00234(this);
        }
    }
}

void func_80BFF6CC(EnBomjima* this, PlayState* play) {
    f32 curFrame = this->skelAnime.curFrame;

    if (curFrame >= this->animEndFrame) {
        if ((Message_GetState(&play->msgCtx) == TEXT_STATE_EVENT) && Message_ShouldAdvance(play)) {
            Message_CloseTextbox(play);
            EnBomjima_ChangeAnim(this, ENBOMJIMA_ANIM_1, 1.0f);
            this->actionFunc = func_80BFF754;
        }
    }
}

void func_80BFF754(EnBomjima* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    Vec3f spA0;
    EnBomjima* bomjima;
    s32 i;
    f32 x;
    f32 y;
    f32 z;

    if (CutsceneManager_GetCurrentCsId() == CS_ID_GLOBAL_TALK) {
        CutsceneManager_Stop(CS_ID_GLOBAL_TALK);
        CutsceneManager_Queue(this->csIdList[1]);
        return;
    }

    if (!CutsceneManager_IsNext(this->csIdList[1])) {
        CutsceneManager_Queue(this->csIdList[1]);
        return;
    }

    for (i = 1; i < 5; i++) {
        Math_Vec3f_Copy(&spA0, &player->actor.world.pos);

        x = spA0.x - this->actor.world.pos.x;
        y = spA0.y - this->actor.world.pos.y;
        z = spA0.z - this->actor.world.pos.z;

        spA0.x += x * (2.0f + (i * 0.2f));
        spA0.y += y * (2.0f + (i * 0.2f));
        spA0.z += z * (2.0f + (i * 0.2f));

        bomjima = (EnBomjima*)Actor_SpawnAsChild(&play->actorCtx, &this->actor, play, ACTOR_EN_BOMJIMA, spA0.x, spA0.y,
                                                 spA0.z, 0, 0, 0, i + 32);
        if (bomjima != NULL) {
            s32 index = (i * 2) - 2;

            Math_Vec3f_Copy(&spA0, &this->actor.world.pos);

            spA0.x += Math_SinS(D_80C00AF8[(i * 2) - 2] + this->actor.world.rot.y) * D_80C00AF8[index + 1];
            spA0.z += Math_CosS(D_80C00AF8[index] + this->actor.world.rot.y) * D_80C00AF8[index + 1];

            Math_Vec3f_Copy(&bomjima->unk_2A4, &spA0);
        }
    }

    D_80C009F0 = 0;
    CutsceneManager_StartWithPlayerCs(this->csIdList[1], &this->actor);
    this->actionFunc = func_80BFF9B0;
}

void func_80BFF9B0(EnBomjima* this, PlayState* play) {
    if (D_80C009F0 >= 4) {
        Player* player = GET_PLAYER(play);

        D_80C009F0 = 0;
        this->unk_2C8 = 9;
        if (player->transformation == PLAYER_FORM_DEKU) {
            SET_WEEKEVENTREG(WEEKEVENTREG_73_10);
            SET_WEEKEVENTREG(WEEKEVENTREG_77_02);
        } else {
            SET_WEEKEVENTREG(WEEKEVENTREG_85_02);
            SET_WEEKEVENTREG(WEEKEVENTREG_85_01);
        }

        CLEAR_WEEKEVENTREG(WEEKEVENTREG_11_01);
        CLEAR_WEEKEVENTREG(WEEKEVENTREG_11_02);
        CLEAR_WEEKEVENTREG(WEEKEVENTREG_11_04);
        CLEAR_WEEKEVENTREG(WEEKEVENTREG_11_08);
        CLEAR_WEEKEVENTREG(WEEKEVENTREG_11_10);

        CLEAR_WEEKEVENTREG(WEEKEVENTREG_76_01);
        CLEAR_WEEKEVENTREG(WEEKEVENTREG_76_02);
        CLEAR_WEEKEVENTREG(WEEKEVENTREG_76_04);
        CLEAR_WEEKEVENTREG(WEEKEVENTREG_76_08);
        CLEAR_WEEKEVENTREG(WEEKEVENTREG_76_10);

        gSaveContext.save.saveInfo.bombersCaughtNum = 0;
        gSaveContext.save.saveInfo.bombersCaughtOrder[0] = 0;
        gSaveContext.save.saveInfo.bombersCaughtOrder[1] = 0;
        gSaveContext.save.saveInfo.bombersCaughtOrder[2] = 0;
        gSaveContext.save.saveInfo.bombersCaughtOrder[3] = 0;
        gSaveContext.save.saveInfo.bombersCaughtOrder[4] = 0;

        EnBomjima_ChangeAnim(this, ENBOMJIMA_ANIM_3, 1.0f);
        this->unk_2C8 = 9;

        if (player->transformation == PLAYER_FORM_DEKU) {
            this->actor.textId = D_80C00A54[this->unk_2C8];
        } else {
            this->actor.textId = D_80C00A70[this->unk_2C8];
        }
        Message_ContinueTextbox(play, this->actor.textId);
        this->actionFunc = func_80BFFB40;
    }
}

void func_80BFFB40(EnBomjima* this, PlayState* play) {
    if ((Message_GetState(&play->msgCtx) == TEXT_STATE_EVENT) && Message_ShouldAdvance(play)) {
        Message_CloseTextbox(play);
        EnBomjima_ChangeAnim(this, ENBOMJIMA_ANIM_15, 1.0f);
        D_80C009F0 = 100;
        this->unk_2DC = 0;
        this->actionFunc = func_80BFFBC4;
    }
}

void func_80BFFBC4(EnBomjima* this, PlayState* play) {
    f32 curFrame = this->skelAnime.curFrame;

    if ((this->animIndex != ENBOMJIMA_ANIM_1) && (curFrame >= this->animEndFrame)) {
        EnBomjima_ChangeAnim(this, ENBOMJIMA_ANIM_1, 1.0f);
    }

    if ((D_80C009F4 != 0) && (this->unk_2C2 == 0)) {
        this->unk_2C2 = Rand_S16Offset(5, 5);
    }

    if (this->unk_2C2 == 1) {
        this->unk_2DC = -10000;
    }

    Math_SmoothStepToS(&this->unk_290, this->unk_2DC, 1, 0x1388, 0);
    if (D_80C009F0 >= 104) {
        D_80C009F0 = 0;
        Message_CloseTextbox(play);
        play->nextEntrance = Entrance_CreateFromSpawn(6);
        gSaveContext.nextCutsceneIndex = 0;
        play->transitionTrigger = TRANS_TRIGGER_START;
        play->transitionType = TRANS_TYPE_86;
        gSaveContext.nextTransitionType = TRANS_TYPE_FADE_WHITE;
        CutsceneManager_Stop(this->csIdList[1]);
    }
}

void func_80BFFCFC(EnBomjima* this) {
    func_80BFE65C(this);
    EnBomjima_ChangeAnim(this, ENBOMJIMA_ANIM_18, 1.0f);
    this->action = EN_BOMJIMA_ACTION_6;
    this->actionFunc = func_80BFFD48;
}

void func_80BFFD48(EnBomjima* this, PlayState* play) {
    s32 pad[2];

    this->unk_2DC = Math_Vec3f_Yaw(&this->actor.world.pos, &this->unk_2A4);
    Math_SmoothStepToS(&this->actor.world.rot.y, this->unk_2DC, 1, 0x1388, 0);
    Math_ApproachF(&this->actor.world.pos.x, this->unk_2A4.x, 0.3f, 4.0f);
    Math_ApproachF(&this->actor.world.pos.z, this->unk_2A4.z, 0.3f, 4.0f);
    if (sqrtf(SQ(this->actor.world.pos.x - this->unk_2A4.x) + SQ(this->actor.world.pos.z - this->unk_2A4.z)) < 4.0f) {
        D_80C009F0++;
        this->unk_2DC = this->actor.parent->world.rot.y;
        EnBomjima_ChangeAnim(this, ENBOMJIMA_ANIM_0, 1.0f);
        this->actionFunc = func_80BFFE48;
    }
}

void func_80BFFE48(EnBomjima* this, PlayState* play) {
    Math_SmoothStepToS(&this->actor.world.rot.y, this->unk_2DC, 1, 0x1388, 0);
    Math_ApproachF(&this->actor.world.pos.x, this->unk_2A4.x, 0.3f, 4.0f);
    Math_ApproachF(&this->actor.world.pos.z, this->unk_2A4.z, 0.3f, 4.0f);

    if (D_80C009F0 >= 100) {
        if (this->unk_2E4 != 4) {
            EnBomjima_ChangeAnim(this, ENBOMJIMA_ANIM_15, 1.0f);
            this->unk_2DC = 0;
            func_80BFE65C(this);
            this->actionFunc = func_80BFFF54;
        } else {
            Math_SmoothStepToS(&this->unk_290, 10000, 1, 0x1388, 0);
            if (D_80C009F0 >= 103) {
                this->unk_2DC = 0;
                EnBomjima_ChangeAnim(this, ENBOMJIMA_ANIM_15, 1.0f);
                func_80BFE65C(this);
                this->actionFunc = func_80BFFF54;
            }
        }
    }
}

void func_80BFFF54(EnBomjima* this, PlayState* play) {
    f32 curFrame = this->skelAnime.curFrame;

    Math_SmoothStepToS(&this->unk_290, this->unk_2DC, 1, 0x1388, 0);

    if ((D_80C009F4 != 0) && (this->unk_2C2 == 0)) {
        this->unk_2C2 = Rand_S16Offset(5, 5);
    }

    if (this->unk_2C2 == 1) {
        this->unk_2DC = -10000;
    }

    if (this->unk_2E4 != 4) {
        if ((this->animIndex != ENBOMJIMA_ANIM_0) && (curFrame >= this->animEndFrame)) {
            D_80C009F0++;
            EnBomjima_ChangeAnim(this, ENBOMJIMA_ANIM_0, 1.0f);
        }
    } else if ((this->animIndex != ENBOMJIMA_ANIM_8) && (curFrame >= this->animEndFrame)) {
        EnBomjima_ChangeAnim(this, ENBOMJIMA_ANIM_8, 1.0f);
        D_80C009F4 = 1;
    }

    if (this->animIndex == ENBOMJIMA_ANIM_8) {
        if ((D_80C009F4 == 1) && Animation_OnFrame(&this->skelAnime, 7.0f)) {
            Actor_PlaySfx(&this->actor, NA_SE_EV_HUMAN_BOUND);
            D_80C009F4 = 2;
        }

        if ((curFrame >= this->animEndFrame) && (this->unk_2C0 == 0)) {
            this->unk_2C0 = 10;
        }

        if (this->unk_2C0 == 1) {
            D_80C009F0++;
        }
    }
}

void func_80C0011C(EnBomjima* this) {
    func_80BFE65C(this);
    EnBomjima_ChangeAnim(this, ENBOMJIMA_ANIM_0, 1.0f);
    this->action = EN_BOMJIMA_ACTION_7;
    this->actionFunc = func_80C00168;
}

void func_80C00168(EnBomjima* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    if (player->transformation == PLAYER_FORM_HUMAN) {
        this->unk_2C8 = 4;
    } else if (player->transformation == PLAYER_FORM_DEKU) {
        this->unk_2C8 = 7;
    }

    func_80BFE32C(this, play, 0);
    func_80BFE67C(this, play);

    if (Text_GetFaceReaction(play, FACE_REACTION_SET_JIM) != 0) {
        this->actor.textId = Text_GetFaceReaction(play, FACE_REACTION_SET_JIM);
    }

    if (Actor_TalkOfferAccepted(&this->actor, &play->state)) {
        this->unk_2DC = this->actor.yawTowardsPlayer;
        func_80C00234(this);
    } else {
        Actor_OfferTalk(&this->actor, play, 70.0f);
    }
}

void func_80C00234(EnBomjima* this) {
    EnBomjima_ChangeAnim(this, ENBOMJIMA_ANIM_3, 1.0f);
    this->collider.dim.radius = 15;
    this->collider.dim.height = 40;
    func_80BFE65C(this);
    this->actionFunc = func_80C00284;
}

void func_80C00284(EnBomjima* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    f32 curFrame = this->skelAnime.curFrame;

    Math_SmoothStepToS(&this->actor.world.rot.y, this->unk_2DC, 1, 0x1388, 0);
    if (((this->action == EN_BOMJIMA_ACTION_0) || (this->unk_2C8 == 10) || (this->unk_2C8 == 11) ||
         (this->unk_2CA == 1)) &&
        (curFrame >= this->animEndFrame)) {
        if (!(this->unk_2BC & 1)) {
            EnBomjima_ChangeAnim(this, ENBOMJIMA_ANIM_3, 1.0f);
        } else {
            EnBomjima_ChangeAnim(this, ENBOMJIMA_ANIM_16, 1.0f);
        }
        this->unk_2BC++;
        this->unk_2BC &= 1;
    }

    switch (player->transformation) {
        case PLAYER_FORM_HUMAN:
            this->unk_28E = -0xFA0;
            break;

        case PLAYER_FORM_GORON:
        case PLAYER_FORM_ZORA:
            this->unk_28E = -0x1770;
            break;

        default:
            break;
    }

    if ((Message_GetState(&play->msgCtx) == TEXT_STATE_EVENT) && Message_ShouldAdvance(play)) {
        this->collider.dim.radius = 10;
        this->collider.dim.height = 30;
        if ((this->action == EN_BOMJIMA_ACTION_4) || (this->unk_2CA == 1) ||
            ((this->unk_2CA == 3) && (this->unk_2C8 >= 2))) {
            this->unk_28E = 0;
            if (player->stateFlags1 & PLAYER_STATE1_20) {
                player->stateFlags1 &= ~PLAYER_STATE1_20;
            }

            if ((this->bombal == 0) || (this->bombal->actor.update == NULL) ||
                (this->bombal->actor.colChkInfo.health <= 0)) {
                func_80BFF3F0(this);
            } else {
                func_80BFE65C(this);
                Message_CloseTextbox(play);
                this->actionFunc = func_80BFEA94;
            }
            return;
        }

        Message_CloseTextbox(play);

        switch (this->unk_2CA) {
            case 0:
                this->unk_28E = 0;
                if (this->action == EN_BOMJIMA_ACTION_7) {
                    func_80C0011C(this);
                } else {
                    func_80BFEB1C(this);
                }
                break;

            case 2:
                if (this->unk_2C8 == 10) {
                    func_80BFE65C(this);
                    this->unk_28E = 0;
                    EnBomjima_ChangeAnim(this, ENBOMJIMA_ANIM_1, 1.0f);
                    this->action = EN_BOMJIMA_ACTION_2;
                    this->actionFunc = func_80BFF174;
                    break;
                }

                this->unk_2C8++;
                if (player->transformation == PLAYER_FORM_DEKU) {
                    if ((this->unk_2E8 != 0) && (this->unk_2C8 == 4)) {
                        this->unk_2C8 = 6;
                    }
                    this->actor.textId = D_80C00A54[this->unk_2C8];
                } else {
                    if ((this->unk_2EA != 0) && (this->unk_2C8 == 4)) {
                        this->unk_2C8 = 6;
                    }
                    this->actor.textId = D_80C00A70[this->unk_2C8];
                }
                Message_ContinueTextbox(play, this->actor.textId);
                if ((this->unk_2C8 == 7) || (this->unk_2C8 == 12)) {
                    func_80BFF4F4(this);
                }
                break;

            case 3:
                this->unk_2C8++;
                this->actor.textId = D_80C00A8C[this->unk_2C8];
                Message_ContinueTextbox(play, this->actor.textId);
                if (this->unk_2C8 >= 2) {
                    EnBomjima_ChangeAnim(this, ENBOMJIMA_ANIM_17, 1.0f);
                }
                break;

            default:
                break;
        }
    }
}

void EnBomjima_Update(Actor* thisx, PlayState* play) {
    s32 pad;
    EnBomjima* this = (EnBomjima*)thisx;

    if (this->unk_2BE != 0) {
        this->unk_2BE--;
    }

    if (this->unk_2C2 != 0) {
        this->unk_2C2--;
    }

    if (this->unk_2C0 != 0) {
        this->unk_2C0--;
    }

    if (this->unk_2C6 != 0) {
        this->unk_2C6--;
    }

    if (this->cutsceneTimer != 0) {
        this->cutsceneTimer--;
    }

    SkelAnime_Update(&this->skelAnime);
    this->actor.shape.rot.y = this->actor.world.rot.y;
    func_80BFE524(this);
    Actor_SetFocus(&this->actor, 20.0f);
    this->actionFunc(this, play);
    Actor_MoveWithGravity(&this->actor);
    Math_SmoothStepToS(&this->unk_28A, this->unk_290, 1, 0x1388, 0);
    Math_SmoothStepToS(&this->unk_288, this->unk_28E, 1, 0x3E8, 0);
    Math_SmoothStepToS(&this->unk_294, this->unk_29A, 1, 0x3E8, 0);

    if (this->unk_2E2 == 0) {
        this->unk_2E0++;
        if (this->unk_2E0 >= 3) {
            this->unk_2E0 = 0;
            this->unk_2E2 = TRUNCF_BINANG(Rand_ZeroFloat(60.0f)) + 20;
        }
    }

    Actor_UpdateBgCheckInfo(play, &this->actor, 20.0f, 20.0f, 50.0f,
                            UPDBGCHECKINFO_FLAG_1 | UPDBGCHECKINFO_FLAG_4 | UPDBGCHECKINFO_FLAG_8 |
                                UPDBGCHECKINFO_FLAG_10);
    this->actor.cullingVolumeDistance = 500.0f;
    Collider_UpdateCylinder(&this->actor, &this->collider);
    CollisionCheck_SetOC(play, &play->colChkCtx, &this->collider.base);
}

s32 EnBomjima_OverrideLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, Actor* thisx) {
    EnBomjima* this = (EnBomjima*)thisx;

    if (limbIndex == OBJECT_CS_LIMB_08) {
        rot->z += this->unk_294;
    }

    if ((limbIndex == OBJECT_CS_LIMB_0F) && (this->unk_2E6 == 2)) {
        *dList = NULL;
    }

    if (limbIndex == OBJECT_CS_LIMB_11) {
        rot->x += this->unk_28A;
        rot->z += this->unk_288;
    }

    if ((limbIndex == OBJECT_CS_LIMB_13) && (this->unk_2E6 == 2)) {
        *dList = NULL;
    }

    if ((limbIndex == OBJECT_CS_LIMB_14) && (this->unk_2E6 == 0)) {
        *dList = NULL;
    }

    return false;
}

#include "overlays/ovl_En_Bomjima/ovl_En_Bomjima.h"

void EnBomjima_Draw(Actor* thisx, PlayState* play) {
    static Gfx* D_80C00B28[] = {
        gEnBomjima_D_80C00B08, gEnBomjima_D_80C00B18, gEnBomjima_D_80C00B18,
        gEnBomjima_D_80C00B18, gEnBomjima_D_80C00B18,
    };
    static TexturePtr D_80C00B3C[] = {
        object_cs_Tex_00C520,
        object_cs_Tex_00CD20,
        object_cs_Tex_00D520,
    };
    static TexturePtr D_80C00B48[] = {
        object_cs_Tex_00E620, object_cs_Tex_00EA20, object_cs_Tex_00EE20, object_cs_Tex_00DD20, object_cs_Tex_00F220,
    };
    EnBomjima* this = (EnBomjima*)thisx;

    OPEN_DISPS(play->state.gfxCtx);

    Gfx_SetupDL25_Opa(play->state.gfxCtx);
    Gfx_SetupDL25_Xlu(play->state.gfxCtx);

    gSPSegment(POLY_OPA_DISP++, 0x08, Lib_SegmentedToVirtual(D_80C00B3C[this->unk_2E0]));
    gSPSegment(POLY_OPA_DISP++, 0x09, Lib_SegmentedToVirtual(D_80C00B48[this->unk_2E4]));
    gSPSegment(POLY_OPA_DISP++, 0x0A, Lib_SegmentedToVirtual(D_80C00B28[this->unk_2E4]));

    Scene_SetRenderModeXlu(play, 0, 1);
    SkelAnime_DrawFlexOpa(play, this->skelAnime.skeleton, this->skelAnime.jointTable, this->skelAnime.dListCount,
                          EnBomjima_OverrideLimbDraw, NULL, &this->actor);

    CLOSE_DISPS(play->state.gfxCtx);
}
