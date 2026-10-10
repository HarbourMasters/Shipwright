/*
 * File: z_en_bombers.c
 * Overlay: ovl_En_Bombers
 * Description: Bombers - Blue-Hatted Bomber
 */

#include "z_en_bombers.h"
#include "overlays/actors/ovl_En_Bom_Bowl_Man/z_en_bom_bowl_man.h"

#define FLAGS (ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_FRIENDLY)

void EnBombers_Init(Actor* thisx, PlayState* play);
void EnBombers_Destroy(Actor* thisx, PlayState* play);
void EnBombers_Update(Actor* thisx, PlayState* play);
void EnBombers_Draw(Actor* thisx, PlayState* play);

void func_80C03ACC(EnBombers* this);
void func_80C03AF4(EnBombers* this, PlayState* play);
void func_80C03F64(EnBombers* this);
void func_80C03FAC(EnBombers* this, PlayState* play);
void func_80C042F8(EnBombers* this);
void func_80C04354(EnBombers* this, PlayState* play);
void func_80C043C8(EnBombers* this, PlayState* play);

ActorProfile En_Bombers_Profile = {
    /**/ ACTOR_EN_BOMBERS,
    /**/ ACTORCAT_NPC,
    /**/ FLAGS,
    /**/ OBJECT_CS,
    /**/ sizeof(EnBombers),
    /**/ EnBombers_Init,
    /**/ EnBombers_Destroy,
    /**/ EnBombers_Update,
    /**/ EnBombers_Draw,
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

typedef enum {
    /*   -1 */ ENBOMBERS_ANIM_NONE = -1,
    /* 0x00 */ ENBOMBERS_ANIM_0,
    /* 0x01 */ ENBOMBERS_ANIM_1,
    /* 0x02 */ ENBOMBERS_ANIM_2,
    /* 0x03 */ ENBOMBERS_ANIM_3,
    /* 0x04 */ ENBOMBERS_ANIM_4,
    /* 0x05 */ ENBOMBERS_ANIM_5,
    /* 0x06 */ ENBOMBERS_ANIM_6,
    /* 0x07 */ ENBOMBERS_ANIM_7,
    /* 0x08 */ ENBOMBERS_ANIM_8,
    /* 0x09 */ ENBOMBERS_ANIM_9,
    /* 0x0A */ ENBOMBERS_ANIM_10,
    /* 0x0B */ ENBOMBERS_ANIM_11,
    /* 0x0C */ ENBOMBERS_ANIM_12,
    /* 0x0D */ ENBOMBERS_ANIM_13,
    /* 0x0E */ ENBOMBERS_ANIM_14,
    /* 0x0F */ ENBOMBERS_ANIM_15,
    /* 0x10 */ ENBOMBERS_ANIM_16,
    /* 0x11 */ ENBOMBERS_ANIM_MAX
} EnBombersAnimation;

static AnimationHeader* sAnimations[ENBOMBERS_ANIM_MAX] = {
    &gBomberIdleAnim,       // ENBOMBERS_ANIM_0
    &object_cs_Anim_0053F4, // ENBOMBERS_ANIM_1
    &object_cs_Anim_01007C, // ENBOMBERS_ANIM_2
    &object_cs_Anim_00349C, // ENBOMBERS_ANIM_3
    &object_cs_Anim_004960, // ENBOMBERS_ANIM_4
    &object_cs_Anim_005128, // ENBOMBERS_ANIM_5
    &object_cs_Anim_004C1C, // ENBOMBERS_ANIM_6
    &object_cs_Anim_001A1C, // ENBOMBERS_ANIM_7
    &object_cs_Anim_003EE4, // ENBOMBERS_ANIM_8
    &object_cs_Anim_00478C, // ENBOMBERS_ANIM_9
    &object_cs_Anim_00433C, // ENBOMBERS_ANIM_10
    &object_cs_Anim_0060E8, // ENBOMBERS_ANIM_11
    &object_cs_Anim_001708, // ENBOMBERS_ANIM_12
    &object_cs_Anim_005DC4, // ENBOMBERS_ANIM_13
    &object_cs_Anim_0026B0, // ENBOMBERS_ANIM_14
    &object_cs_Anim_0036B0, // ENBOMBERS_ANIM_15
    &object_cs_Anim_0031C4, // ENBOMBERS_ANIM_16
};

static u8 sAnimationModes[ENBOMBERS_ANIM_MAX] = {
    ANIMMODE_LOOP, // ENBOMBERS_ANIM_0
    ANIMMODE_LOOP, // ENBOMBERS_ANIM_1
    ANIMMODE_LOOP, // ENBOMBERS_ANIM_2
    ANIMMODE_ONCE, // ENBOMBERS_ANIM_3
    ANIMMODE_ONCE, // ENBOMBERS_ANIM_4
    ANIMMODE_LOOP, // ENBOMBERS_ANIM_5
    ANIMMODE_LOOP, // ENBOMBERS_ANIM_6
    ANIMMODE_LOOP, // ENBOMBERS_ANIM_7
    ANIMMODE_ONCE, // ENBOMBERS_ANIM_8
    ANIMMODE_LOOP, // ENBOMBERS_ANIM_9
    ANIMMODE_LOOP, // ENBOMBERS_ANIM_10
    ANIMMODE_LOOP, // ENBOMBERS_ANIM_11
    ANIMMODE_ONCE, // ENBOMBERS_ANIM_12
    ANIMMODE_LOOP, // ENBOMBERS_ANIM_13
    ANIMMODE_ONCE, // ENBOMBERS_ANIM_14
    ANIMMODE_LOOP, // ENBOMBERS_ANIM_15
    ANIMMODE_LOOP, // ENBOMBERS_ANIM_16
};

#include "overlays/ovl_En_Bombers/ovl_En_Bombers.h"

Gfx* D_80C04818[] = {
    ovl_En_Bombers_DL_12C8, ovl_En_Bombers_DL_12D8, ovl_En_Bombers_DL_12D8,
    ovl_En_Bombers_DL_12D8, ovl_En_Bombers_DL_12D8,
};

TexturePtr D_80C0482C[] = {
    object_cs_Tex_00C520,
    object_cs_Tex_00CD20,
    object_cs_Tex_00D520,
};

TexturePtr D_80C04838[] = {
    object_cs_Tex_00E620, object_cs_Tex_00EA20, object_cs_Tex_00EE20, object_cs_Tex_00DD20, object_cs_Tex_00F220,
};

void EnBombers_Init(Actor* thisx, PlayState* play) {
    EnBombers* this = (EnBombers*)thisx;

    this->actor.colChkInfo.mass = MASS_IMMOVABLE;
    ActorShape_Init(&this->actor.shape, 0.0f, ActorShadow_DrawCircle, 19.0f);
    this->actor.gravity = -1.0f;
    SkelAnime_InitFlex(play, &this->skelAnime, &object_cs_Skel_00F82C, &gBomberIdleAnim, this->jointTable,
                       this->morphTable, OBJECT_CS_LIMB_MAX);
    Collider_InitAndSetCylinder(play, &this->collider, &this->actor, &sCylinderInit);
    this->actor.attentionRangeType = ATTENTION_RANGE_0;
    Actor_SetScale(&this->actor, 0.01f);

    this->unk_2BC = ENBOMBERS_GET_F0(&this->actor);
    this->unk_2BE = ENBOMBERS_GET_F(&this->actor);

    if (this->unk_2BC == ENBOMBERS_F0_0) {
        if (CHECK_WEEKEVENTREG(WEEKEVENTREG_73_10) || CHECK_WEEKEVENTREG(WEEKEVENTREG_85_02)) {
            Actor_Kill(&this->actor);
            return;
        }

        this->unk_2BE++;
        func_80C03ACC(this);
    } else if ((CHECK_WEEKEVENTREG(WEEKEVENTREG_73_10) || CHECK_WEEKEVENTREG(WEEKEVENTREG_85_02)) &&
               (((this->unk_2BE == ENBOMBERS_F_0) && CHECK_WEEKEVENTREG(WEEKEVENTREG_76_01)) ||
                ((this->unk_2BE == ENBOMBERS_F_1) && CHECK_WEEKEVENTREG(WEEKEVENTREG_76_02)) ||
                ((this->unk_2BE == ENBOMBERS_F_2) && CHECK_WEEKEVENTREG(WEEKEVENTREG_76_04)) ||
                ((this->unk_2BE == ENBOMBERS_F_3) && CHECK_WEEKEVENTREG(WEEKEVENTREG_76_08)) ||
                ((this->unk_2BE == ENBOMBERS_F_4) && CHECK_WEEKEVENTREG(WEEKEVENTREG_76_10)))) {
        if (CHECK_WEEKEVENTREG(WEEKEVENTREG_75_40)) {
            if (this->unk_2BE == ENBOMBERS_F_0) {
                EnBomBowlMan* bomBowlMan = (EnBomBowlMan*)Actor_Spawn(
                    &play->actorCtx, play, ACTOR_EN_BOM_BOWL_MAN, this->actor.world.pos.x, this->actor.world.pos.y,
                    this->actor.world.pos.z, 0, this->actor.world.rot.y, 0, 0);

                if (bomBowlMan != NULL) {
                    s32 csId = this->actor.csId;
                    s32 i = 0;

                    // clang-format off
                    while (csId != CS_ID_NONE) { bomBowlMan->csIdList[i] = csId; csId = CutsceneManager_GetAdditionalCsId(csId); i++; }
                    // clang-format on

                    CLEAR_WEEKEVENTREG(WEEKEVENTREG_76_01);
                    CLEAR_WEEKEVENTREG(WEEKEVENTREG_76_02);
                    CLEAR_WEEKEVENTREG(WEEKEVENTREG_76_04);
                    CLEAR_WEEKEVENTREG(WEEKEVENTREG_76_08);
                    CLEAR_WEEKEVENTREG(WEEKEVENTREG_76_10);
                }
            }
            Actor_Kill(&this->actor);
            return;
        }

        func_80C042F8(this);
    } else {
        Actor_Kill(&this->actor);
    }
}

void EnBombers_Destroy(Actor* thisx, PlayState* play) {
    EnBombers* this = (EnBombers*)thisx;

    Collider_DestroyCylinder(play, &this->collider);
}

void EnBombers_ChangeAnim(EnBombers* this, s32 animIndex, f32 playSpeed) {
    this->animIndex = animIndex;
    this->animEndFrame = Animation_GetLastFrame(sAnimations[animIndex]);
    Animation_Change(&this->skelAnime, sAnimations[this->animIndex], playSpeed, 0.0f, this->animEndFrame,
                     sAnimationModes[this->animIndex], -10.0f);
}

void func_80C038B4(EnBombers* this) {
    if ((this->animIndex == ENBOMBERS_ANIM_2) &&
        (Animation_OnFrame(&this->skelAnime, 9.0f) || Animation_OnFrame(&this->skelAnime, 10.0f) ||
         Animation_OnFrame(&this->skelAnime, 17.0f) || Animation_OnFrame(&this->skelAnime, 18.0f))) {
        Actor_PlaySfx(&this->actor, NA_SE_EV_BOMBERS_WALK);
    }

    if ((this->animIndex == ENBOMBERS_ANIM_15) &&
        (Animation_OnFrame(&this->skelAnime, 0.0f) || Animation_OnFrame(&this->skelAnime, 2.0f) ||
         Animation_OnFrame(&this->skelAnime, 4.0f) || Animation_OnFrame(&this->skelAnime, 6.0f))) {
        Actor_PlaySfx(&this->actor, NA_SE_EV_BOMBERS_WALK);
    }
}

void func_80C039A8(EnBombers* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    this->unk_2A6 = TEXT_STATE_EVENT;

    switch (player->transformation) {
        case PLAYER_FORM_HUMAN:
            this->actor.textId = 0x73D;
            if (CHECK_WEEKEVENTREG(WEEKEVENTREG_84_80)) {
                this->actor.textId = 0x74B;
            }
            break;

        case PLAYER_FORM_GORON:
            this->actor.textId = 0x73E;
            break;

        case PLAYER_FORM_ZORA:
            this->actor.textId = 0x73F;
            break;

        case PLAYER_FORM_DEKU:
            if (CHECK_WEEKEVENTREG(WEEKEVENTREG_73_20)) {
                this->actor.textId = 0x75A;
            } else if (CHECK_WEEKEVENTREG(WEEKEVENTREG_73_40)) {
                this->actor.textId = 0x749;
                if (((this->unk_2BE == ENBOMBERS_F_1) && CHECK_WEEKEVENTREG(WEEKEVENTREG_74_01)) ||
                    ((this->unk_2BE == ENBOMBERS_F_2) && CHECK_WEEKEVENTREG(WEEKEVENTREG_74_02)) ||
                    ((this->unk_2BE == ENBOMBERS_F_3) && CHECK_WEEKEVENTREG(WEEKEVENTREG_74_04)) ||
                    ((this->unk_2BE == ENBOMBERS_F_4) && CHECK_WEEKEVENTREG(WEEKEVENTREG_74_08))) {
                    this->actor.textId = 0x74A;
                }
            } else {
                this->actor.textId = 0x744;
            }
            break;
        // #region 2S2H - Enhancements. Fierce Deity gets same dialog as Human without notebook
        case PLAYER_FORM_FIERCE_DEITY:
            this->actor.textId = 0x73D;
            break;
        // #endregion
        default:
            break;
    }
}

void func_80C03ACC(EnBombers* this) {
    this->collider.dim.radius = 10;
    this->collider.dim.height = 30;
    this->unk_2C0 = 0;
    this->actionFunc = func_80C03AF4;
}

void func_80C03AF4(EnBombers* this, PlayState* play) {
    CollisionPoly* colPoly;
    f32 curFrame = this->skelAnime.curFrame;
    Vec3f sp6C;
    Vec3f sp60;
    Vec3f sp54;
    f32 x;
    f32 z;
    s32 bgId;
    s16 abs;

    switch (this->unk_2A0) {
        case 0:
            if (this->unk_2A8 == 0) {
                Math_Vec3f_Copy(&sp60, &this->actor.home.pos);

                sp60.x += Rand_CenteredFloat(150.0f);
                sp60.z += Rand_CenteredFloat(150.0f);

                abs = ABS_ALT(BINANG_SUB(this->actor.world.rot.y, Math_Vec3f_Yaw(&this->actor.world.pos, &sp60)));
                if ((abs < 0x4000) && !BgCheck_EntityLineTest1(&play->colCtx, &this->actor.world.pos, &sp60, &sp6C,
                                                               &colPoly, true, false, false, true, &bgId)) {
                    EnBombers_ChangeAnim(this, ENBOMBERS_ANIM_2, 1.0f);
                    Math_Vec3f_Copy(&this->unk_294, &sp60);
                    this->unk_2AA = Rand_S16Offset(30, 50);
                    this->unk_2A0++;
                }
            }
            break;

        case 1:
            if (curFrame >= 0.0f) {
                this->unk_2A4 = Math_Vec3f_Yaw(&this->actor.world.pos, &this->unk_294);
                Math_SmoothStepToS(&this->actor.world.rot.y, this->unk_2A4, 0xA, 0x7D0, 0x14);
            }

            if ((s16)ABS_ALT(BINANG_SUB(this->actor.world.rot.y, this->unk_2A4)) < 0x100) {
                Math_Vec3f_Copy(&sp54, &this->actor.world.pos);
                sp54.x += Math_SinS(this->actor.world.rot.y) * 60.0f;
                sp54.z += Math_CosS(this->actor.world.rot.y) * 60.0f;

                if (BgCheck_EntityLineTest1(&play->colCtx, &this->actor.world.pos, &sp54, &sp6C, &colPoly, true, false,
                                            false, true, &bgId)) {
                    this->unk_2A8 = 0;
                    if (Rand_ZeroOne() < 0.5f) {
                        EnBombers_ChangeAnim(this, ENBOMBERS_ANIM_16, 1.0f);
                    } else {
                        EnBombers_ChangeAnim(this, ENBOMBERS_ANIM_0, 1.0f);
                    }
                    this->unk_2A0 = 0;
                    this->unk_2B4 = 0.0f;
                    break;
                }
            }

            x = this->unk_294.x - this->actor.world.pos.x;
            z = this->unk_294.z - this->actor.world.pos.z;

            if ((this->unk_2AA == 0) || (sqrtf(SQ(x) + SQ(z)) < 4.0f)) {
                this->unk_2A8 = Rand_S16Offset(20, 20);
                if (!(this->unk_2A8 & 1)) {
                    EnBombers_ChangeAnim(this, ENBOMBERS_ANIM_16, 1.0f);
                } else {
                    EnBombers_ChangeAnim(this, ENBOMBERS_ANIM_0, 1.0f);
                }
                this->unk_2A0 = 0;
                this->unk_2B4 = 0.0f;
            } else if (curFrame >= 0.0f) {
                Math_ApproachF(&this->actor.world.pos.x, this->unk_294.x, 0.3f, this->unk_2B4);
                Math_ApproachF(&this->actor.world.pos.z, this->unk_294.z, 0.3f, this->unk_2B4);
                Math_ApproachF(&this->unk_2B4, 1.0f, 0.3f, 0.5f);
            }
            break;

        default:
            break;
    }

    if (Text_GetFaceReaction(play, FACE_REACTION_SET_BOMBERS) == 0) {
        func_80C039A8(this, play);
    } else {
        this->unk_2A6 = TEXT_STATE_EVENT;
        this->actor.textId = Text_GetFaceReaction(play, FACE_REACTION_SET_BOMBERS);
    }

    if (Actor_TalkOfferAccepted(&this->actor, &play->state)) {
        this->unk_2A4 = this->actor.yawTowardsPlayer;
        this->collider.dim.radius = 20;
        this->collider.dim.height = 60;
        func_80C03F64(this);
    } else {
        Actor_OfferTalk(&this->actor, play, 70.0f);
    }
}

void func_80C03F64(EnBombers* this) {
    this->unk_2A4 = this->actor.yawTowardsPlayer;
    EnBombers_ChangeAnim(this, ENBOMBERS_ANIM_1, 1.0f);
    this->unk_2C0 = 1;
    this->actionFunc = func_80C03FAC;
}

void func_80C03FAC(EnBombers* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    s16 sp2A;

    Math_SmoothStepToS(&this->actor.world.rot.y, this->unk_2A4, 1, 0x1388, 0);

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

    if ((this->unk_2A6 == Message_GetState(&play->msgCtx)) && Message_ShouldAdvance(play)) {
        sp2A = 0;
        Message_CloseTextbox(play);
        this->unk_2A6 = TEXT_STATE_EVENT;

        if ((this->actor.textId == 0x73D) || (this->actor.textId == 0x73E) || (this->actor.textId == 0x73F)) {
            this->actor.textId = 0x740;
            this->unk_2A6 = TEXT_STATE_CHOICE;
            sp2A = 1;
        } else if (this->actor.textId == 0x740) {
            if (play->msgCtx.choiceIndex == 0) {
                Audio_PlaySfx_MessageDecide();
                this->actor.textId = 0x742;
                sp2A = 1;
            } else {
                Audio_PlaySfx_MessageCancel();
                this->actor.textId = 0x741;
                sp2A = 1;
            }
        } else if (this->actor.textId == 0x742) {
            this->actor.textId = 0x737;
            sp2A = 1;
        } else if (this->actor.textId == 0x737) {
            this->actor.textId = 0x743;
            sp2A = 1;
        } else if (this->actor.textId == 0x74B) {
            this->actor.textId = 0x74C;
            this->unk_2A6 = TEXT_STATE_CHOICE;
            sp2A = 1;
        } else if (this->actor.textId == 0x74C) {
            if (play->msgCtx.choiceIndex == 1) {
                Audio_PlaySfx_MessageCancel();
                this->actor.textId = 0x737;
                sp2A = 1;
            } else {
                Audio_PlaySfx_MessageDecide();
                this->actor.textId = 0x74D;
                EnBombers_ChangeAnim(this, ENBOMBERS_ANIM_14, 1.0f);
                sp2A = 1;
            }
        } else if (this->actor.textId == 0x744) {
            s32 day = gSaveContext.save.day - 1;

            if (day == 2) {
                this->actor.textId = 0x746;
                sp2A = 1;
            } else {
                this->actor.textId = 0x745;
                sp2A = 1;
            }
        } else if ((this->actor.textId == 0x745) || (this->actor.textId == 0x746)) {
            this->actor.textId = 0x747;
            sp2A = 1;
        } else if (this->actor.textId == 0x747) {
            this->actor.textId = 0x748;
            sp2A = 1;
        } else if (this->actor.textId == 0x748) {
            switch (this->unk_2BE) {
                case ENBOMBERS_F_1:
                    SET_WEEKEVENTREG(WEEKEVENTREG_74_01);
                    break;

                case ENBOMBERS_F_2:
                    SET_WEEKEVENTREG(WEEKEVENTREG_74_02);
                    break;

                case ENBOMBERS_F_3:
                    SET_WEEKEVENTREG(WEEKEVENTREG_74_04);
                    break;

                case ENBOMBERS_F_4:
                    SET_WEEKEVENTREG(WEEKEVENTREG_74_08);
                    break;

                default:
                    break;
            }
            SET_WEEKEVENTREG(WEEKEVENTREG_73_40);
        }

        switch (sp2A) {
            case 0:
                this->unk_28E = 0;
                func_80C03ACC(this);
                break;

            case 1:
                Message_ContinueTextbox(play, this->actor.textId);
                break;

            default:
                break;
        }
    }
}

void func_80C042F8(EnBombers* this) {
    if (this->animIndex != ENBOMBERS_ANIM_6) {
        EnBombers_ChangeAnim(this, ENBOMBERS_ANIM_6, 1.0f);
    }
    this->actor.textId = 0x72D;
    this->unk_28E = 0x1F40;
    this->unk_2C0 = 2;
    this->actionFunc = func_80C04354;
}

void func_80C04354(EnBombers* this, PlayState* play) {
    Math_SmoothStepToS(&this->unk_288, this->unk_28E, 1, 0x3E8, 0);
    if (Actor_TalkOfferAccepted(&this->actor, &play->state)) {
        this->unk_28E = 0;
        this->actionFunc = func_80C043C8;
    } else {
        Actor_OfferTalk(&this->actor, play, 70.0f);
    }
}

void func_80C043C8(EnBombers* this, PlayState* play) {
    Math_SmoothStepToS(&this->unk_288, this->unk_28E, 1, 0x3E8, 0);
    if ((Message_GetState(&play->msgCtx) == TEXT_STATE_EVENT) && Message_ShouldAdvance(play)) {
        Message_CloseTextbox(play);
        func_80C042F8(this);
    }
}

void EnBombers_Update(Actor* thisx, PlayState* play) {
    s32 pad;
    EnBombers* this = (EnBombers*)thisx;

    if (this->unk_2AA != 0) {
        this->unk_2AA--;
    }

    if (this->unk_2A8 != 0) {
        this->unk_2A8--;
    }

    if (this->unk_2AC != 0) {
        this->unk_2AC--;
    }

    SkelAnime_Update(&this->skelAnime);
    this->actor.shape.rot.y = this->actor.world.rot.y;
    func_80C038B4(this);
    Actor_SetFocus(&this->actor, 20.0f);
    this->actionFunc(this, play);
    Actor_MoveWithGravity(&this->actor);
    Math_SmoothStepToS(&this->unk_28A, this->unk_290, 1, 0x1388, 0);
    Math_SmoothStepToS(&this->unk_288, this->unk_28E, 1, 0x1388, 0);

    if (this->unk_2BA == 0) {
        this->unk_2B8++;
        if (this->unk_2B8 >= 3) {
            this->unk_2B8 = 0;
            this->unk_2BA = TRUNCF_BINANG(Rand_ZeroFloat(60.0f)) + 20;
        }
    }

    Actor_UpdateBgCheckInfo(play, &this->actor, 20.0f, 20.0f, 50.0f,
                            UPDBGCHECKINFO_FLAG_1 | UPDBGCHECKINFO_FLAG_4 | UPDBGCHECKINFO_FLAG_8 |
                                UPDBGCHECKINFO_FLAG_10);
    this->actor.cullingVolumeDistance = 500.0f;
    Collider_UpdateCylinder(&this->actor, &this->collider);
    CollisionCheck_SetOC(play, &play->colChkCtx, &this->collider.base);
}

s32 EnBombers_OverrideLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, Actor* thisx) {
    EnBombers* this = (EnBombers*)thisx;

    if (limbIndex == OBJECT_CS_LIMB_0F) {
        *dList = NULL;
    }

    if (limbIndex == OBJECT_CS_LIMB_11) {
        rot->x += this->unk_28A;
        rot->z += this->unk_288;
    }

    if (limbIndex == OBJECT_CS_LIMB_13) {
        *dList = NULL;
    }

    return false;
}

void EnBombers_Draw(Actor* thisx, PlayState* play) {
    EnBombers* this = (EnBombers*)thisx;

    OPEN_DISPS(play->state.gfxCtx);

    Gfx_SetupDL25_Opa(play->state.gfxCtx);
    Gfx_SetupDL25_Xlu(play->state.gfxCtx);

    gSPSegment(POLY_OPA_DISP++, 0x08, Lib_SegmentedToVirtual(D_80C0482C[this->unk_2B8]));
    gSPSegment(POLY_OPA_DISP++, 0x09, Lib_SegmentedToVirtual(D_80C04838[this->unk_2BE]));
    gSPSegment(POLY_OPA_DISP++, 0x0A, Lib_SegmentedToVirtual(D_80C04818[this->unk_2BE]));

    Scene_SetRenderModeXlu(play, 0, 1);
    SkelAnime_DrawFlexOpa(play, this->skelAnime.skeleton, this->skelAnime.jointTable, this->skelAnime.dListCount,
                          EnBombers_OverrideLimbDraw, NULL, &this->actor);

    CLOSE_DISPS(play->state.gfxCtx);
}
