/*
 * File: z_en_ma_yts.c
 * Overlay: ovl_En_Ma_Yts
 * Description: Romani. As opposed as EnMa4, this actor is used in conjunction with another actor, like EnMaYto.
 */

#include "z_en_ma_yts.h"

#define FLAGS                                                                           \
    (ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_FRIENDLY | ACTOR_FLAG_FREEZE_EXCEPTION | \
     ACTOR_FLAG_UPDATE_DURING_OCARINA)

void EnMaYts_Init(Actor* thisx, PlayState* play);
void EnMaYts_Destroy(Actor* thisx, PlayState* play);
void EnMaYts_Update(Actor* thisx, PlayState* play);
void EnMaYts_Draw(Actor* thisx, PlayState* play);

void EnMaYts_SetupDoNothing(EnMaYts* this);
void EnMaYts_DoNothing(EnMaYts* this, PlayState* play);
void EnMaYts_SetupStartDialogue(EnMaYts* this);
void EnMaYts_StartDialogue(EnMaYts* this, PlayState* play);
void EnMaYts_SetupDialogueHandler(EnMaYts* this);
void EnMaYts_DialogueHandler(EnMaYts* this, PlayState* play);
void EnMaYts_SetupEndCreditsHandler(EnMaYts* this);
void EnMaYts_EndCreditsHandler(EnMaYts* this, PlayState* play);
void EnMaYts_ChooseNextDialogue(EnMaYts* this, PlayState* play);

void EnMaYts_SetFaceExpression(EnMaYts* this, s16 overrideEyeTexIndex, s16 mouthTexIndex);

void EnMaYts_DrawSleeping(Actor* thisx, PlayState* play);

ActorProfile En_Ma_Yts_Profile = {
    /**/ ACTOR_EN_MA_YTS,
    /**/ ACTORCAT_NPC,
    /**/ FLAGS,
    /**/ OBJECT_MA1,
    /**/ sizeof(EnMaYts),
    /**/ EnMaYts_Init,
    /**/ EnMaYts_Destroy,
    /**/ EnMaYts_Update,
    /**/ EnMaYts_Draw,
};

void EnMaYts_UpdateEyes(EnMaYts* this) {
    if (this->overrideEyeTexIndex != 0) {
        this->eyeTexIndex = this->overrideEyeTexIndex;
    } else if (DECR(this->blinkTimer) == 0) {
        this->eyeTexIndex++;
        if (this->eyeTexIndex >= 3) {
            this->blinkTimer = Rand_S16Offset(30, 30);
            this->eyeTexIndex = 0;
        }
    }
}

static ColliderCylinderInit sCylinderInit = {
    {
        COL_MATERIAL_NONE,
        AT_NONE,
        AC_NONE,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_2,
        COLSHAPE_CYLINDER,
    },
    {
        ELEM_MATERIAL_UNK0,
        { 0x00000000, 0x00, 0x00 },
        { 0x00000000, 0x00, 0x00 },
        ATELEM_NONE | ATELEM_SFX_NORMAL,
        ACELEM_NONE,
        OCELEM_ON,
    },
    { 18, 46, 0, { 0, 0, 0 } },
};

static CollisionCheckInfoInit2 sColChkInfoInit2 = {
    0, 0, 0, 0, MASS_IMMOVABLE,
};

typedef enum EnMaYtsAnimation {
    /* -1 */ ENMATYS_ANIM_NONE = -1,
    /*  0 */ ENMATYS_ANIM_0,
    /*  1 */ ENMATYS_ANIM_1,
    /*  2 */ ENMATYS_ANIM_2,
    /*  3 */ ENMATYS_ANIM_3,
    /*  4 */ ENMATYS_ANIM_4,
    /*  5 */ ENMATYS_ANIM_5,
    /*  6 */ ENMATYS_ANIM_6,
    /*  7 */ ENMATYS_ANIM_7,
    /*  8 */ ENMATYS_ANIM_8,
    /*  9 */ ENMATYS_ANIM_9,
    /* 10 */ ENMATYS_ANIM_10,
    /* 11 */ ENMATYS_ANIM_11,
    /* 12 */ ENMATYS_ANIM_12,
    /* 13 */ ENMATYS_ANIM_13,
    /* 14 */ ENMATYS_ANIM_14,
    /* 15 */ ENMATYS_ANIM_15,
    /* 16 */ ENMATYS_ANIM_16,
    /* 17 */ ENMATYS_ANIM_17,
    /* 18 */ ENMATYS_ANIM_18,
    /* 19 */ ENMATYS_ANIM_19,
    /* 20 */ ENMATYS_ANIM_20,
    /* 21 */ ENMATYS_ANIM_21,
    /* 22 */ ENMATYS_ANIM_MAX
} EnMaYtsAnimation;

static AnimationSpeedInfo sAnimationSpeedInfo[ENMATYS_ANIM_MAX] = {
    { &gRomaniIdleAnim, 1.0f, ANIMMODE_LOOP, 0.0f },                    // ENMATYS_ANIM_0
    { &gRomaniIdleAnim, 1.0f, ANIMMODE_LOOP, -6.0f },                   // ENMATYS_ANIM_1
    { &gRomaniSingStartAnim, 1.0f, ANIMMODE_ONCE, 0.0f },               // ENMATYS_ANIM_2
    { &gRomaniSingStartAnim, 1.0f, ANIMMODE_ONCE, -6.0f },              // ENMATYS_ANIM_3
    { &gRomaniSingLoopAnim, 1.0f, ANIMMODE_LOOP, 0.0f },                // ENMATYS_ANIM_4
    { &gRomaniSingLoopAnim, 1.0f, ANIMMODE_LOOP, -6.0f },               // ENMATYS_ANIM_5
    { &gRomaniRunAnim, 1.0f, ANIMMODE_LOOP, 0.0f },                     // ENMATYS_ANIM_6
    { &gRomaniRunAnim, 1.0f, ANIMMODE_LOOP, -6.0f },                    // ENMATYS_ANIM_7
    { &gRomaniWalkAnim, 1.0f, ANIMMODE_LOOP, 0.0f },                    // ENMATYS_ANIM_8
    { &gRomaniWalkAnim, 1.0f, ANIMMODE_LOOP, -6.0f },                   // ENMATYS_ANIM_9
    { &gRomaniLookAroundAnim, 1.0f, ANIMMODE_LOOP, 0.0f },              // ENMATYS_ANIM_10
    { &gRomaniLookAroundAnim, 1.0f, ANIMMODE_LOOP, -6.0f },             // ENMATYS_ANIM_11
    { &gRomaniShootBowAnim, 1.0f, ANIMMODE_LOOP, 0.0f },                // ENMATYS_ANIM_12
    { &gRomaniShootBowAnim, 1.0f, ANIMMODE_LOOP, -6.0f },               // ENMATYS_ANIM_13
    { &gRomaniSittingAnim, 1.0f, ANIMMODE_LOOP, 0.0f },                 // ENMATYS_ANIM_14
    { &gRomaniSittingAnim, 1.0f, ANIMMODE_LOOP, -6.0f },                // ENMATYS_ANIM_15
    { &gRomaniSittingHeadShakeAnim, 1.0f, ANIMMODE_LOOP, 0.0f },        // ENMATYS_ANIM_16
    { &gRomaniSittingHeadShakeAnim, 1.0f, ANIMMODE_LOOP, -6.0f },       // ENMATYS_ANIM_17
    { &gRomaniSittingLookDownAnim, 1.0f, ANIMMODE_LOOP, 0.0f },         // ENMATYS_ANIM_18
    { &gRomaniSittingLookDownAnim, 1.0f, ANIMMODE_LOOP, -6.0f },        // ENMATYS_ANIM_19
    { &gRomaniTurnAroundAndFlickHairAnim, 1.0f, ANIMMODE_ONCE, 0.0f },  // ENMATYS_ANIM_20
    { &gRomaniTurnAroundAndFlickHairAnim, 1.0f, ANIMMODE_ONCE, -6.0f }, // ENMATYS_ANIM_21
};

static TexturePtr sMouthTextures[] = {
    gRomaniMouthHappyTex,
    gRomaniMouthFrownTex,
    gRomaniMouthHangingOpenTex,
    gRomaniMouthSmileTex,
};

static TexturePtr sEyeTextures[] = {
    gRomaniEyeOpenTex, gRomaniEyeHalfTex, gRomaniEyeClosedTex, gRomaniEyeHappyTex, gRomaniEyeSadTex,
};

void EnMaYts_ChangeAnim(EnMaYts* this, s32 animIndex) {
    Animation_Change(&this->skelAnime, sAnimationSpeedInfo[animIndex].animation, 1.0f, 0.0f,
                     Animation_GetLastFrame(sAnimationSpeedInfo[animIndex].animation),
                     sAnimationSpeedInfo[animIndex].mode, sAnimationSpeedInfo[animIndex].morphFrames);
}

void func_80B8D12C(EnMaYts* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    s16 trackingMode = (this->unk_32C == 2) ? NPC_TRACKING_NONE : NPC_TRACKING_PLAYER_AUTO_TURN;

    if ((this->unk_32C == 0) || (this->actor.parent == NULL)) {
        this->interactInfo.trackPos = player->actor.world.pos;
        this->interactInfo.trackPos.y -= -10.0f;
    } else {
        Math_Vec3f_StepTo(&this->interactInfo.trackPos, &this->actor.parent->world.pos, 8.0f);
        this->interactInfo.trackPos.y -= -10.0f;
    }

    Npc_TrackPoint(&this->actor, &this->interactInfo, 0, trackingMode);
}

void EnMaYts_InitAnimation(EnMaYts* this, PlayState* play) {
    switch (this->type) {
        case MA_YTS_TYPE_BARN:
            this->actor.attentionRangeType = ATTENTION_RANGE_0;
            EnMaYts_ChangeAnim(this, ENMATYS_ANIM_0);
            break;

        case MA_YTS_TYPE_SITTING:
            this->actor.attentionRangeType = ATTENTION_RANGE_6;
            // Day 1 or "Winning" the alien invasion
            if ((CURRENT_DAY == 1) || CHECK_WEEKEVENTREG(WEEKEVENTREG_DEFENDED_AGAINST_ALIENS)) {
                EnMaYts_ChangeAnim(this, ENMATYS_ANIM_14);
            } else {
                EnMaYts_ChangeAnim(this, ENMATYS_ANIM_18);
            }
            break;

        case MA_YTS_TYPE_SLEEPING:
            this->actor.attentionRangeType = ATTENTION_RANGE_0;
            this->actor.draw = EnMaYts_DrawSleeping;
            EnMaYts_ChangeAnim(this, ENMATYS_ANIM_0);
            break;

        case MA_YTS_TYPE_ENDCREDITS:
            this->actor.attentionRangeType = ATTENTION_RANGE_0;
            EnMaYts_ChangeAnim(this, ENMATYS_ANIM_0);
            break;

        default:
            EnMaYts_ChangeAnim(this, ENMATYS_ANIM_0);
            break;
    }
}

s32 EnMaYts_CheckValidSpawn(EnMaYts* this, PlayState* play) {
    switch (this->type) {
        case MA_YTS_TYPE_SITTING:
            switch (CURRENT_DAY) {
                case 1:
                    break;

                case 2:
                    // Failing the alien invasion
                    if (!CHECK_WEEKEVENTREG(WEEKEVENTREG_DEFENDED_AGAINST_ALIENS)) {
                        return false;
                    }
                    break;

                case 3:
                    // "Winning" the alien invasion
                    if (CHECK_WEEKEVENTREG(WEEKEVENTREG_DEFENDED_AGAINST_ALIENS)) {
                        return false;
                    }
                    break;

                default:
                    break;
            }
            break;

        case MA_YTS_TYPE_BARN:
            // Failing the alien invasion
            if (!CHECK_WEEKEVENTREG(WEEKEVENTREG_DEFENDED_AGAINST_ALIENS)) {
                return false;
            } else if ((CURRENT_TIME >= CLOCK_TIME(20, 0)) && (CURRENT_DAY == 3)) {
                return false;
            }
            break;

        case MA_YTS_TYPE_SLEEPING:
            // "Winning" the alien invasion
            if (CHECK_WEEKEVENTREG(WEEKEVENTREG_DEFENDED_AGAINST_ALIENS)) {
                return false;
            }
            break;

        case MA_YTS_TYPE_ENDCREDITS:
            break;

        default:
            break;
    }

    return true;
}

void EnMaYts_Init(Actor* thisx, PlayState* play) {
    EnMaYts* this = (EnMaYts*)thisx;
    s32 pad;

    this->type = EN_MA_YTS_GET_TYPE(thisx);
    if (!EnMaYts_CheckValidSpawn(this, play)) {
        Actor_Kill(&this->actor);
    }

    ActorShape_Init(&this->actor.shape, 0.0f, ActorShadow_DrawCircle, 18.0f);
    SkelAnime_InitFlex(play, &this->skelAnime, &gRomaniSkel, NULL, this->jointTable, this->morphTable, ROMANI_LIMB_MAX);
    EnMaYts_InitAnimation(this, play);

    Collider_InitCylinder(play, &this->collider);
    Collider_SetCylinder(play, &this->collider, &this->actor, &sCylinderInit);
    CollisionCheck_SetInfo2(&this->actor.colChkInfo, DamageTable_Get(0x16), &sColChkInfoInit2);

    if (this->type == MA_YTS_TYPE_SLEEPING) {
        this->collider.dim.radius = 40;
    }

    Actor_UpdateBgCheckInfo(play, &this->actor, 0.0f, 0.0f, 0.0f, UPDBGCHECKINFO_FLAG_4);
    Actor_SetScale(&this->actor, 0.01f);

    this->interactInfo.talkState = NPC_TALK_STATE_IDLE;
    this->unk_200 = 0;
    this->blinkTimer = 0;

    if (this->type == MA_YTS_TYPE_ENDCREDITS) {
        this->hasBow = true;
    } else {
        this->hasBow = false;
    }

    if ((CURRENT_DAY == 1) || CHECK_WEEKEVENTREG(WEEKEVENTREG_DEFENDED_AGAINST_ALIENS)) {
        this->overrideEyeTexIndex = 0;
        this->eyeTexIndex = 0;
        this->mouthTexIndex = 0;
        this->unk_32C = 0;
    } else {
        this->overrideEyeTexIndex = 1;
        this->eyeTexIndex = 1;
        this->mouthTexIndex = 2;
        this->unk_32C = 2;
    }

    if (this->type == MA_YTS_TYPE_ENDCREDITS) {
        this->overrideEyeTexIndex = 0;
        this->eyeTexIndex = 0;
        this->mouthTexIndex = 0;
        this->unk_32C = 2;
        EnMaYts_SetupEndCreditsHandler(this);
    } else if ((CURRENT_DAY == 2) && (gSaveContext.save.isNight == 1) &&
               CHECK_WEEKEVENTREG(WEEKEVENTREG_DEFENDED_AGAINST_ALIENS)) {
        EnMaYts_SetupStartDialogue(this);
    } else {
        EnMaYts_SetupDoNothing(this);
    }
}

void EnMaYts_Destroy(Actor* thisx, PlayState* play) {
    EnMaYts* this = (EnMaYts*)thisx;

    Collider_DestroyCylinder(play, &this->collider);
}

void EnMaYts_SetupDoNothing(EnMaYts* this) {
    this->actionFunc = EnMaYts_DoNothing;
}

void EnMaYts_DoNothing(EnMaYts* this, PlayState* play) {
}

void EnMaYts_SetupStartDialogue(EnMaYts* this) {
    EnMaYts_SetFaceExpression(this, 0, 0);
    this->actionFunc = EnMaYts_StartDialogue;
}

void EnMaYts_StartDialogue(EnMaYts* this, PlayState* play) {
    s16 sp26 = this->actor.shape.rot.y - this->actor.yawTowardsPlayer;

    if (Actor_TalkOfferAccepted(&this->actor, &play->state)) {
        if (GET_PLAYER_FORM != PLAYER_FORM_HUMAN) {
            if (!CHECK_WEEKEVENTREG(WEEKEVENTREG_65_80)) {
                // Saying to non-human Link: "Cremia went to town."
                SET_WEEKEVENTREG(WEEKEVENTREG_65_80);
                EnMaYts_SetFaceExpression(this, 0, 0);
                Message_StartTextbox(play, 0x335F, &this->actor);
                this->textId = 0x335F;
            } else {
                // Saying to non-human Link: "Pretend you did not hear that."
                EnMaYts_SetFaceExpression(this, 4, 3);
                Message_StartTextbox(play, 0x3362, &this->actor);
                this->textId = 0x3362;
                Message_BombersNotebookQueueEvent(play, BOMBERS_NOTEBOOK_EVENT_MET_ROMANI);
            }
        } else if (Player_GetMask(play) != PLAYER_MASK_NONE) {
            if (!CHECK_WEEKEVENTREG(WEEKEVENTREG_65_40)) {
                SET_WEEKEVENTREG(WEEKEVENTREG_65_40);
                EnMaYts_SetFaceExpression(this, 0, 0);
                Message_StartTextbox(play, 0x3363, &this->actor);
                this->textId = 0x3363;
            } else {
                EnMaYts_SetFaceExpression(this, 4, 2);
                Message_StartTextbox(play, 0x3366, &this->actor);
                this->textId = 0x3366;
                Message_BombersNotebookQueueEvent(play, BOMBERS_NOTEBOOK_EVENT_MET_ROMANI);
            }
        } else if (!CHECK_WEEKEVENTREG(WEEKEVENTREG_PROMISED_TO_HELP_WITH_ALIENS)) {
            EnMaYts_SetFaceExpression(this, 0, 0);
            Message_StartTextbox(play, 0x3367, &this->actor);
            this->textId = 0x3367;
        } else {
            if (!CHECK_WEEKEVENTREG(WEEKEVENTREG_65_20)) {
                // Saying to Grasshopper: "Cremia went to town."
                SET_WEEKEVENTREG(WEEKEVENTREG_65_20);
                EnMaYts_SetFaceExpression(this, 4, 2);
                Message_StartTextbox(play, 0x3369, &this->actor);
                this->textId = 0x3369;
            } else {
                // Saying to Grasshopper: "You're our bodyguard."
                EnMaYts_SetFaceExpression(this, 0, 0);
                Message_StartTextbox(play, 0x336C, &this->actor);
                this->textId = 0x336C;
                Message_BombersNotebookQueueEvent(play, BOMBERS_NOTEBOOK_EVENT_MET_ROMANI);
            }
        }
        EnMaYts_SetupDialogueHandler(this);
    } else if (ABS_ALT(sp26) < 0x4000) {
        Actor_OfferTalk(&this->actor, play, 120.0f);
    }
}

void EnMaYts_SetupDialogueHandler(EnMaYts* this) {
    this->actionFunc = EnMaYts_DialogueHandler;
}

void EnMaYts_DialogueHandler(EnMaYts* this, PlayState* play) {
    switch (Message_GetState(&play->msgCtx)) {
        case TEXT_STATE_EVENT: // End message block
            EnMaYts_ChooseNextDialogue(this, play);
            break;

        case TEXT_STATE_DONE: // End conversation
            if (Message_ShouldAdvance(play)) {
                EnMaYts_SetupStartDialogue(this);
            }
            break;

        case TEXT_STATE_NONE:
        case TEXT_STATE_NEXT:
        case TEXT_STATE_CLOSING:
        case TEXT_STATE_FADING:
        case TEXT_STATE_CHOICE:
        default:
            break;
    }
}

void EnMaYts_SetupEndCreditsHandler(EnMaYts* this) {
    this->actor.flags |= ACTOR_FLAG_UPDATE_CULLING_DISABLED;
    EnMaYts_SetFaceExpression(this, 0, 0);
    this->actionFunc = EnMaYts_EndCreditsHandler;
}

static u16 sCueId = 99;

void EnMaYts_EndCreditsHandler(EnMaYts* this, PlayState* play) {
    if (Cutscene_IsCueInChannel(play, CS_CMD_ACTOR_CUE_120)) {
        s32 cueChannel = Cutscene_GetCueChannel(play, CS_CMD_ACTOR_CUE_120);

        if (play->csCtx.curFrame == play->csCtx.actorCues[cueChannel]->startFrame) {
            if (sCueId != play->csCtx.actorCues[cueChannel]->id) {
                sCueId = play->csCtx.actorCues[cueChannel]->id;
                this->endCreditsFlag = 0;
                switch (play->csCtx.actorCues[cueChannel]->id) {
                    case 1:
                        this->hasBow = true;
                        EnMaYts_ChangeAnim(this, ENMATYS_ANIM_0);
                        break;

                    case 2:
                        this->hasBow = false;
                        EnMaYts_ChangeAnim(this, ENMATYS_ANIM_2);
                        break;

                    case 3:
                        this->hasBow = true;
                        EnMaYts_ChangeAnim(this, ENMATYS_ANIM_12);
                        break;

                    case 4:
                        this->hasBow = true;
                        EnMaYts_ChangeAnim(this, ENMATYS_ANIM_20);
                        break;

                    default:
                        break;
                }
            }
        }

        Cutscene_ActorTranslateAndYaw(&this->actor, play, cueChannel);
        if ((sCueId == 2) && (this->endCreditsFlag == 0) &&
            Animation_OnFrame(&this->skelAnime, this->skelAnime.endFrame)) {
            this->endCreditsFlag++;
            EnMaYts_ChangeAnim(this, ENMATYS_ANIM_5);
        }
    } else {
        sCueId = 99;
        this->hasBow = true;
    }
}

// Select the following dialogue based on the current one, and an appropiate face expression
void EnMaYts_ChooseNextDialogue(EnMaYts* this, PlayState* play) {
    if (Message_ShouldAdvance(play)) {
        switch (this->textId) {
            case 0x335F:
                EnMaYts_SetFaceExpression(this, 0, 2);
                Message_StartTextbox(play, 0x3360, &this->actor);
                this->textId = 0x3360;
                break;

            case 0x3360:
                EnMaYts_SetFaceExpression(this, 4, 3);
                Message_StartTextbox(play, 0x3361, &this->actor);
                this->textId = 0x3361;
                Message_BombersNotebookQueueEvent(play, BOMBERS_NOTEBOOK_EVENT_MET_ROMANI);
                break;

            case 0x3363:
                EnMaYts_SetFaceExpression(this, 1, 1);
                Message_StartTextbox(play, 0x3364, &this->actor);
                this->textId = 0x3364;
                break;

            case 0x3364:
                EnMaYts_SetFaceExpression(this, 4, 2);
                Message_StartTextbox(play, 0x3365, &this->actor);
                this->textId = 0x3365;
                Message_BombersNotebookQueueEvent(play, BOMBERS_NOTEBOOK_EVENT_MET_ROMANI);
                break;

            case 0x3367:
                EnMaYts_SetFaceExpression(this, 4, 3);
                Message_StartTextbox(play, 0x3368, &this->actor);
                this->textId = 0x3368;
                Message_BombersNotebookQueueEvent(play, BOMBERS_NOTEBOOK_EVENT_MET_ROMANI);
                break;

            case 0x3369:
                EnMaYts_SetFaceExpression(this, 0, 0);
                Message_StartTextbox(play, 0x336A, &this->actor);
                this->textId = 0x336A;
                break;

            case 0x336A:
                EnMaYts_SetFaceExpression(this, 3, 3);
                Message_StartTextbox(play, 0x336B, &this->actor);
                this->textId = 0x336B;
                Message_BombersNotebookQueueEvent(play, BOMBERS_NOTEBOOK_EVENT_MET_ROMANI);
                break;

            default:
                break;
        }
    }
}

void EnMaYts_SetFaceExpression(EnMaYts* this, s16 overrideEyeTexIndex, s16 mouthTexIndex) {
    this->overrideEyeTexIndex = overrideEyeTexIndex;
    this->mouthTexIndex = mouthTexIndex;
    if (this->overrideEyeTexIndex == 0) {
        this->eyeTexIndex = 0;
    }
}

void EnMaYts_Update(Actor* thisx, PlayState* play) {
    EnMaYts* this = (EnMaYts*)thisx;
    ColliderCylinder* collider;

    this->actionFunc(this, play);
    collider = &this->collider;
    Collider_UpdateCylinder(&this->actor, collider);
    CollisionCheck_SetOC(play, &play->colChkCtx, &collider->base);
    SkelAnime_Update(&this->skelAnime);
    EnMaYts_UpdateEyes(this);
    func_80B8D12C(this, play);
}

s32 EnMaYts_OverrideLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, Actor* thisx) {
    EnMaYts* this = (EnMaYts*)thisx;
    Vec3s limbRot;

    if (limbIndex == ROMANI_LIMB_HEAD) {
        limbRot = this->interactInfo.headRot;
        rot->x += limbRot.y;
        if ((this->skelAnime.animation == &gRomaniIdleAnim) || (this->skelAnime.animation == &gRomaniSittingAnim)) {
            rot->z += limbRot.x;
        }
    } else if (limbIndex == ROMANI_LIMB_TORSO) {
        limbRot = this->interactInfo.torsoRot;
        rot->x += limbRot.y;
        rot->z += limbRot.x;
    }

    return false;
}

void EnMaYts_PostLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot, Actor* thisx) {
    EnMaYts* this = (EnMaYts*)thisx;

    if (limbIndex == ROMANI_LIMB_HEAD) {
        Matrix_MultZero(&this->actor.focus.pos);
    } else if (limbIndex == ROMANI_LIMB_LEFT_HAND) {
        if (this->hasBow == true) {
            OPEN_DISPS(play->state.gfxCtx);

            gSPDisplayList(POLY_OPA_DISP++, gRomaniBowDL);

            CLOSE_DISPS(play->state.gfxCtx);
        }
    }
}

void EnMaYts_Draw(Actor* thisx, PlayState* play) {
    EnMaYts* this = (EnMaYts*)thisx;

    OPEN_DISPS(play->state.gfxCtx);

    Gfx_SetupDL25_Opa(play->state.gfxCtx);

    gSPSegment(POLY_OPA_DISP++, 0x09, SEGMENTED_TO_K0(sMouthTextures[this->mouthTexIndex]));
    gSPSegment(POLY_OPA_DISP++, 0x08, SEGMENTED_TO_K0(sEyeTextures[this->eyeTexIndex]));

    SkelAnime_DrawFlexOpa(play, this->skelAnime.skeleton, this->skelAnime.jointTable, this->skelAnime.dListCount,
                          EnMaYts_OverrideLimbDraw, EnMaYts_PostLimbDraw, &this->actor);

    CLOSE_DISPS(play->state.gfxCtx);
}

// Alternative draw function
void EnMaYts_DrawSleeping(Actor* thisx, PlayState* play) {
    OPEN_DISPS(play->state.gfxCtx);

    Gfx_SetupDL25_Opa(play->state.gfxCtx);

    MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx);
    gSPDisplayList(POLY_OPA_DISP++, gRomaniSleepingDL);

    CLOSE_DISPS(play->state.gfxCtx);
}
