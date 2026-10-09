/*
 * File: z_en_kitan.c
 * Overlay: ovl_En_Kitan
 * Description: Keaton
 */

#include "z_en_kitan.h"

#define FLAGS (ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_FRIENDLY)

void EnKitan_Init(Actor* thisx, PlayState* play);
void EnKitan_Destroy(Actor* thisx, PlayState* play);
void EnKitan_Update(Actor* thisx, PlayState* play);
void EnKitan_Draw(Actor* thisx, PlayState* play);

void EnKitan_Talk(EnKitan* this, PlayState* play);
void EnKitan_WaitToAppear(EnKitan* this, PlayState* play);

ActorProfile En_Kitan_Profile = {
    /**/ ACTOR_EN_KITAN,
    /**/ ACTORCAT_NPC,
    /**/ FLAGS,
    /**/ OBJECT_KITAN,
    /**/ sizeof(EnKitan),
    /**/ EnKitan_Init,
    /**/ EnKitan_Destroy,
    /**/ EnKitan_Update,
    /**/ NULL,
};

static ColliderCylinderInit sCylinderInit = {
    {
        COL_MATERIAL_NONE,
        AT_NONE,
        AC_ON | AC_TYPE_ENEMY,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_1,
        COLSHAPE_CYLINDER,
    },
    {
        ELEM_MATERIAL_UNK0,
        { 0x00000000, 0x00, 0x00 },
        { 0xF7CFFFFF, 0x00, 0x00 },
        ATELEM_NONE | ATELEM_SFX_NORMAL,
        ACELEM_ON,
        OCELEM_ON,
    },
    { 20, 40, 0, { 0, 0, 0 } },
};

void EnKitan_Init(Actor* thisx, PlayState* play) {
    EnKitan* this = (EnKitan*)thisx;
    s32 pad;

    Actor_SetScale(&this->actor, 0.0f);
    this->actionFunc = EnKitan_WaitToAppear;

    ActorShape_Init(&this->actor.shape, 0.0f, ActorShadow_DrawCircle, 12.0f);
    SkelAnime_InitFlex(play, &this->skelAnime, &gKeatonSkel, &gKeatonIdleAnim, this->jointTable, this->morphTable,
                       KEATON_LIMB_MAX);
    Animation_PlayLoop(&this->skelAnime, &gKeatonIdleAnim);

    Collider_InitAndSetCylinder(play, &this->collider, &this->actor, &sCylinderInit);
    this->actor.colChkInfo.mass = MASS_IMMOVABLE;
    Collider_UpdateCylinder(&this->actor, &this->collider);

    this->actor.velocity.y = -9.0f;
    this->actor.terminalVelocity = -9.0f;
    this->actor.gravity = -1.0f;

    if ((Player_GetMask(play) != PLAYER_MASK_KEATON) ||
        Flags_GetCollectible(play, ENKITAN_GET_COLLECT_FLAG(&this->actor))) {
        Actor_Kill(&this->actor);
        return;
    }

    this->timer = 120;
    this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
}

void EnKitan_Destroy(Actor* thisx, PlayState* play) {
    EnKitan* this = (EnKitan*)thisx;

    Collider_DestroyCylinder(play, &this->collider);
}

void EnKitan_SpawnEffects(EnKitan* this, PlayState* play, s32 numEffects) {
    static Color_RGBA8 sEffPrimColor = { 255, 255, 255, 255 };
    static Color_RGBA8 sEffEnvColor = { 255, 255, 200, 255 };
    s32 i;
    Vec3f accel;
    Vec3f velocity;
    Vec3f pos;

    pos.x = this->actor.world.pos.x;
    pos.y = this->actor.world.pos.y;
    pos.z = this->actor.world.pos.z;

    for (i = 0; i < numEffects; i++) {
        velocity.x = Rand_CenteredFloat(10.0f);
        velocity.y = Rand_ZeroFloat(6.0f);
        velocity.z = Rand_CenteredFloat(10.0f);
        accel.x = -velocity.x * 0.05f;
        accel.y = 0.1f;
        accel.z = -velocity.x * 0.05f;
        func_800B0F18(play, &pos, &velocity, &accel, &sEffPrimColor, &sEffEnvColor, 400, 20, 20);
    }
}

s32 EnKitan_CanTalk(EnKitan* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    if (player->stateFlags1 & PLAYER_STATE1_800000) {
        return false;
    }
    if (Player_IsFacingActor(&this->actor, 0x3000, play) && Actor_IsFacingPlayer(&this->actor, 0x3000) &&
        (this->actor.xzDistToPlayer < 120.0f)) {
        return true;
    }
    return false;
}

u16 EnKitan_GetQuestionMessageId(EnKitan* this) {
    s32 i = 0;

    while (true) {
        s32 rand = Rand_ZeroFloat(30.0f);

        // Keep track of which questions have already been asked with a bitset
        if (!(this->textBitSet & (1 << rand))) {
            this->textBitSet |= 1 << rand;
            // 0x04B6 is the start of the question + answer choice textboxes, each question textbox is followed by the
            // choice textbox containing the answer choices
            return 0x4B6 + (rand * 2);
        }

        i++;
        if (i > 1000) {
            // There's an assert(false) here in the debug version to catch any unforeseen issues in testing. It is
            // assumed that if this function is called there is always at least one question available to be selected.
        }
    }
}

void EnKitan_Leave(EnKitan* this, PlayState* play) {
    SkelAnime_Update(&this->skelAnime);

    if (this->timer > 0) {
        // Scale down
        this->timer--;
        this->actor.scale.x *= 0.7f;
        Actor_SetScale(&this->actor, this->actor.scale.x);
    } else {
        Actor_Kill(&this->actor);
    }
}

void EnKitan_TalkAfterGivingPrize(EnKitan* this, PlayState* play) {
    s32 pad;

    SkelAnime_Update(&this->skelAnime);

    if (Actor_TalkOfferAccepted(&this->actor, &play->state)) {
        this->actionFunc = EnKitan_Talk;
        Message_ContinueTextbox(play, 0x04B5);
        this->actor.flags &= ~ACTOR_FLAG_TALK_OFFER_AUTO_ACCEPTED;
        Animation_MorphToLoop(&this->skelAnime, &gKeatonChuckleAnim, -5.0f);
    } else {
        Actor_OfferTalkExchange(&this->actor, play, 1000.0f, 1000.0f, PLAYER_IA_MINUS1);
    }
}

void EnKitan_WaitForPrizeTextboxClosed(EnKitan* this, PlayState* play) {
    SkelAnime_Update(&this->skelAnime);

    if (Actor_TextboxIsClosing(&this->actor, play)) {
        this->actor.flags |= ACTOR_FLAG_TALK_OFFER_AUTO_ACCEPTED;
        this->actionFunc = EnKitan_TalkAfterGivingPrize;
        Actor_OfferTalkExchange(&this->actor, play, 1000.0f, 1000.0f, PLAYER_IA_MINUS1);
    }
}

void EnKitan_OfferPrize(EnKitan* this, PlayState* play) {
    SkelAnime_Update(&this->skelAnime);

    if (Actor_HasParent(&this->actor, play)) {
        this->actor.parent = NULL;
        this->actionFunc = EnKitan_WaitForPrizeTextboxClosed;
        SET_WEEKEVENTREG(WEEKEVENTREG_RECEIVED_KEATON_HEART_PIECE);
        return;
    }

    // Reward the player with a heart piece, or a red rupee if the heart piece was already obtained.
    if (CHECK_WEEKEVENTREG(WEEKEVENTREG_RECEIVED_KEATON_HEART_PIECE)) {
        Actor_OfferGetItem(&this->actor, play, GI_RUPEE_RED, 2000.0f, 1000.0f);
    } else {
        Actor_OfferGetItem(&this->actor, play, GI_HEART_PIECE, 2000.0f, 1000.0f);
    }
}

void EnKitan_Talk(EnKitan* this, PlayState* play) {
    if (SkelAnime_Update(&this->skelAnime)) {
        Animation_MorphToLoop(&this->skelAnime, &gKeatonIdleAnim, -10.0f);
        if (play->msgCtx.currentTextId != 0x04B4) {
            // If the quiz is ongoing, select a question
            Message_ContinueTextbox(play, EnKitan_GetQuestionMessageId(this));
        }
    }

    switch (Message_GetState(&play->msgCtx)) {
        case TEXT_STATE_CHOICE:
            if (!Message_ShouldAdvance(play)) {
                break;
            }

            if ((play->msgCtx.choiceIndex + 1) == play->msgCtx.unk1206C) {
                // Correct answer, continue quiz or end if enough questions have been answered correctly
                Audio_PlaySfx(NA_SE_SY_QUIZ_CORRECT);

                // Here the timer is being used as a counter for number of correct answers
                this->timer++;
                if (this->timer < 5) {
                    play->msgCtx.msgLength = 0;
                } else {
                    // Enough questions have been answered, continue to prize
                    this->timer = 0;
                    this->textBitSet = 0;
                    Message_ContinueTextbox(play, 0x04B4);
                }
                Animation_MorphToPlayOnce(&this->skelAnime, &gKeatonCelebrateAnim, -5.0f);
            } else {
                // Wrong answer, end quiz
                Audio_PlaySfx(NA_SE_SY_QUIZ_INCORRECT);
                Animation_MorphToLoop(&this->skelAnime, &gKeatonChuckleAnim, -5.0f);
                Message_ContinueTextbox(play, 0x04B3);
                this->timer = 0;
                this->textBitSet = 0;
            }
            break;

        case TEXT_STATE_EVENT:
            if (!Message_ShouldAdvance(play)) {
                break;
            }

            switch (play->msgCtx.currentTextId) {
                case 0x04B0:
                case 0x04B1:
                    // Intro text
                    Message_ContinueTextbox(play, play->msgCtx.currentTextId + 1);
                    break;

                case 0x04B2:
                    // Quiz begins
                    Animation_MorphToLoop(&this->skelAnime, &gKeatonIdleAnim, -5.0f);
                    Message_ContinueTextbox(play, EnKitan_GetQuestionMessageId(this));
                    break;

                case 0x04B4:
                    // Won the quiz
                    Message_CloseTextbox(play);
                    this->actionFunc = EnKitan_OfferPrize;
                    EnKitan_OfferPrize(this, play);
                    break;

                case 0x04B3:
                    // Answered a question incorrectly, stop
                    SEQCMD_STOP_SEQUENCE(SEQ_PLAYER_FANFARE, 0);
                    FALLTHROUGH;
                case 0x04B5:
                    // Keaton leaving
                    Message_CloseTextbox(play);
                    this->actionFunc = EnKitan_Leave;
                    this->timer = 4;
                    EnKitan_SpawnEffects(this, play, 30);
                    SoundSource_PlaySfxAtFixedWorldPos(play, &this->actor.world.pos, 30, NA_SE_EN_NPC_FADEAWAY);
                    Flags_SetCollectible(play, ENKITAN_GET_COLLECT_FLAG(&this->actor));
                    break;

                default:
                    if (!(play->msgCtx.currentTextId & 1)) {
                        // Even-numbered textboxes are question textboxes
                        // The following textbox contains the associated answer choices for this question
                        Message_ContinueTextbox(play, play->msgCtx.currentTextId + 1);
                    }
                    break;
            }
            break;

        default:
            break;
    }
}

void EnKitan_WaitForPlayer(EnKitan* this, PlayState* play) {
    SkelAnime_Update(&this->skelAnime);

    if (Actor_TalkOfferAccepted(&this->actor, &play->state)) {
        // Began talking
        this->actionFunc = EnKitan_Talk;
        Message_StartTextbox(play, 0x04B0, &this->actor);
        this->timer = 0;
        Animation_MorphToLoop(&this->skelAnime, &gKeatonChuckleAnim, -5.0f);
        Audio_PlayFanfare(NA_BGM_KEATON_QUIZ);
        return;
    }

    if ((this->timer <= 0) || (Player_GetMask(play) != PLAYER_MASK_KEATON)) {
        // If the player does not talk quickly enough or the player isn't wearing the keaton mask, leave
        this->actionFunc = EnKitan_Leave;
        this->timer = 4;
        EnKitan_SpawnEffects(this, play, 30);
        SoundSource_PlaySfxAtFixedWorldPos(play, &this->actor.world.pos, 30, NA_SE_EN_NPC_FADEAWAY);
        return;
    }

    if (EnKitan_CanTalk(this, play)) {
        // Broadcast talk request for the player to accept
        Actor_OfferTalk(&this->actor, play, 130.0f);
        this->timer--;
    }
}

void EnKitan_Appear(EnKitan* this, PlayState* play) {
    SkelAnime_Update(&this->skelAnime);

    if (this->timer > 0) {
        // Scale up
        this->timer--;
        this->actor.scale.x = this->actor.scale.x * 0.3f + 0.0105f;
        Actor_SetScale(&this->actor, this->actor.scale.x);
        return;
    }

    // Done scaling, continue
    Actor_SetScale(&this->actor, 0.015f);
    this->actionFunc = EnKitan_WaitForPlayer;
    this->actor.flags |= ACTOR_FLAG_ATTENTION_ENABLED;
    this->timer = 600;
}

void EnKitan_WaitToAppear(EnKitan* this, PlayState* play) {
    if (this->timer > 0) {
        this->timer--;
        return;
    }
    EnKitan_SpawnEffects(this, play, 30);
    Actor_PlaySfx(&this->actor, NA_SE_EN_NPC_APPEAR);
    this->actor.draw = EnKitan_Draw;
    this->actionFunc = EnKitan_Appear;
    this->timer = 20;
    this->actor.shape.rot.y = this->actor.yawTowardsPlayer;
    this->actor.world.rot.y = this->actor.shape.rot.y;
}

void EnKitan_Update(Actor* thisx, PlayState* play) {
    EnKitan* this = (EnKitan*)thisx;

    if (this->actor.draw != NULL) {
        CollisionCheck_SetOC(play, &play->colChkCtx, &this->collider.base);
    }
    Actor_MoveWithGravity(&this->actor);
    Actor_UpdateBgCheckInfo(play, &this->actor, 40.0f, 25.0f, 40.0f, UPDBGCHECKINFO_FLAG_1 | UPDBGCHECKINFO_FLAG_4);

    // Face the player
    Math_SmoothStepToS(&this->actor.shape.rot.y, this->actor.yawTowardsPlayer, 2, 0x1000, 0x200);
    this->actor.world.rot.y = this->actor.shape.rot.y;

    this->actionFunc(this, play);
}

s32 EnKitan_OverrideLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, Actor* thisx) {
    return false;
}

void EnKitan_PostLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot, Actor* thisx) {
    static Vec3f sFocusOffset = { 0.0f, 0.0f, 0.0f };
    EnKitan* this = (EnKitan*)thisx;

    if (limbIndex == KEATON_LIMB_RIGHT_SHOULDER) {
        Matrix_MultVec3f(&sFocusOffset, &this->actor.focus.pos);
    }
}

void EnKitan_Draw(Actor* thisx, PlayState* play) {
    EnKitan* this = (EnKitan*)thisx;

    Gfx_SetupDL37_Opa(play->state.gfxCtx);
    SkelAnime_DrawFlexOpa(play, this->skelAnime.skeleton, this->skelAnime.jointTable, this->skelAnime.dListCount,
                          EnKitan_OverrideLimbDraw, EnKitan_PostLimbDraw, &this->actor);
}
