/*
 * File: z_en_kakasi.c
 * Overlay: ovl_En_Kakasi
 * Description: Pierre the Scarecorw
 */

#include "prevent_bss_reordering.h"
#include "z_en_kakasi.h"
#include "objects/object_ka/object_ka.h"
#include "2s2h/GameInteractor/GameInteractor.h"

#define FLAGS                                                                                  \
    (ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_FRIENDLY | ACTOR_FLAG_UPDATE_CULLING_DISABLED | \
     ACTOR_FLAG_UPDATE_DURING_OCARINA)

void EnKakasi_Init(Actor* thisx, PlayState* play);
void EnKakasi_Destroy(Actor* thisx, PlayState* play);
void EnKakasi_Update(Actor* thisx, PlayState* play);
void EnKakasi_Draw(Actor* thisx, PlayState* play);

void EnKakasi_SetupPostSongLearnDialogue(EnKakasi* this, PlayState* play);
void EnKakasi_InitTimeSkipDialogue(EnKakasi* this);
void EnKakasi_SetupIdleStanding(EnKakasi* this);

s32 EnKakasi_ValidatePictograph(PlayState* play, Actor* thisx);

void EnKakasi_TimeSkipDialogue(EnKakasi* this, PlayState* play);
void EnKakasi_IdleStanding(EnKakasi* this, PlayState* play);
void EnKakasi_OcarinaRemark(EnKakasi* this, PlayState* play);
void EnKakasi_TeachingSong(EnKakasi* this, PlayState* play);
void EnKakasi_PostSongLearnTwirl(EnKakasi* this, PlayState* play);
void EnKakasi_PostSongLearnDialogue(EnKakasi* this, PlayState* play);
void EnKakasi_SetupDigAway(EnKakasi* this);
void EnKakasi_DiggingAway(EnKakasi* this, PlayState* play);
void EnKakasi_RisenDialogue(EnKakasi* this, PlayState* play);
void EnKakasi_SetupIdleUnderground(EnKakasi* this);
void EnKakasi_IdleUnderground(EnKakasi* this, PlayState* play);
void EnKakasi_SetupIdleRisen(EnKakasi* this);
void EnKakasi_IdleRisen(EnKakasi* this, PlayState* play);
void EnKakasi_SetupRiseOutOfGround(EnKakasi* this, PlayState* play);
void EnKakasi_RisingOutOfGround(EnKakasi* this, PlayState* play);
void EnKakasi_DancingRemark(EnKakasi* this, PlayState* play);
void EnKakasi_SetupDanceNightAway(EnKakasi* this);
void EnKakasi_DancingNightAway(EnKakasi* this, PlayState* play);
void EnKakasi_DoNothing(EnKakasi* this, PlayState* play);
void EnKakasi_RegularDialogue(EnKakasi* this, PlayState* play);
void EnKakasi_SetupSongTeach(EnKakasi* this, PlayState* play);

void EnKakasi_SetupDialogue(EnKakasi* this);

static ColliderCylinderInit D_80971D80 = {
    {
        COL_MATERIAL_NONE,
        AT_NONE,
        AC_ON | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_2,
        COLSHAPE_CYLINDER,
    },
    {
        ELEM_MATERIAL_UNK0,
        { 0xF7CFFFFF, 0x00, 0x00 },
        { 0xF7CFFFFF, 0x00, 0x00 },
        ATELEM_NONE | ATELEM_SFX_NORMAL,
        ACELEM_ON | ACELEM_HOOKABLE,
        OCELEM_ON,
    },
    { 20, 70, 0, { 0, 0, 0 } },
};

ActorProfile En_Kakasi_Profile = {
    /**/ ACTOR_EN_KAKASI,
    /**/ ACTORCAT_NPC,
    /**/ FLAGS,
    /**/ OBJECT_KA,
    /**/ sizeof(EnKakasi),
    /**/ EnKakasi_Init,
    /**/ EnKakasi_Destroy,
    /**/ EnKakasi_Update,
    /**/ EnKakasi_Draw,
};

Vec3f D_80971DCC[] = {
    { 0.0f, 60.0f, 60.0f },   { 40.0f, 40.0f, 50.0f },   { -40.0f, 40.0f, 50.0f },
    { 40.0f, 20.0f, 110.0f }, { -40.0f, 20.0f, 110.0f }, { 0.0f, 80.0f, 60.0f },
    { 50.0f, 40.0f, -30.0f }, { -50.0f, 40.0f, -30.0f }, { 0.0f, 50.0f, 60.0f },
};
Vec3f D_80971E38[] = {
    { 0.0f, 55.0f, 60.0f },   { 0.0f, 55.0f, 60.0f },   { -80.0f, 35.0f, 15.0f }, { 80.0f, 45.0f, 15.0f },
    { -10.0f, 35.0f, 65.0f }, { 0.0f, 110.0f, 180.0f }, { 0.0f, 60.0f, 70.0f },   { 0.0f, 60.0f, 70.0f },
    { -80.0f, 50.0f, 40.0f }, { 0.0f, -10.0f, 50.0f },  { 80.0f, 50.0f, 40.0f },  { 0.0f, 120.0f, 40.0f },
    { 0.0f, 28.0f, -50.0f },  { 0.0f, 60.0f, 50.0f },   { 0.0f, 80.0f, 160.0f },
};

Vec3f D_80971EEC[] = {
    { 12.0f, 1.0f, 2.0f },     { 12.0f, 1.0f, 2.0f },  { 2.0f, -6.0f, 0.0f },  { 12.0f, -6.0f, -10.0f },
    { -88.0f, 14.0f, -10.0f }, { 0.0f, 0.0f, 0.0f },   { 0.0f, 0.0f, 0.0f },   { 0.0f, 0.0f, 0.0f },
    { 0.0f, -10.0f, 0.0f },    { 0.0f, -10.0f, 0.0f }, { 0.0f, -10.0f, 0.0f }, { 0.0f, -10.0f, 0.0f },
    { 0.0f, -10.0f, 0.0f },    { 0.0f, 0.0f, 0.0f },   { 0.0f, 0.0f, 0.0f },
};

Vec3f D_80971FA0[] = {
    { -50.0f, 160.0f, -40.0f }, { -50.0f, 160.0f, -40.0f }, { -20.0f, 80.0f, 60.0f },
    { 57.0f, 60.0f, 120.0f },   { 27.0f, 80.0f, 60.0f },    { -3.0f, 50.0f, 70.0f },
};

Vec3f D_80971FE8[] = {
    { 0.0f, 0.0f, 0.0f },    { 0.0f, 0.0f, 0.0f },  { -10.0f, -5.0f, 0.0f },
    { 10.0f, -10.0f, 7.0f }, { 20.0f, 5.0f, 2.0f }, { 10.0f, 5.0f, 2.0f },
};

// both use in digging away function
Vec3f D_80972030 = { -3.0f, 50.0f, 90.0f };
Vec3f D_8097203C = { 10.0f, -15.0f, 2.0f };

typedef enum {
    /* 0 */ ENKAKASI_ANIM_ARMS_CROSSED_ROCKING, // arms crossed and rocking back and forth
    /* 1 */ ENKAKASI_ANIM_SIDEWAYS_SHAKING,
    /* 2 */ ENKAKASI_ANIM_HOPPING_REGULAR,    // bounce bounce bounce (repeat)
    /* 3 */ ENKAKASI_ANIM_SPIN_REACH_OFFER,   // spin and then reaching toward player
    /* 4 */ ENKAKASI_ANIM_TWIRL,              // slower cinimatic twirl
    /* 5 */ ENKAKASI_ANIM_ARMS_CROSSED_STILL, // arms crossed but still, also some noise sfx
    /* 6 */ ENKAKASI_ANIM_WAVE,               // "wave" short sideways shake, stops early, partial? unused?
    /* 7 */ ENKAKASI_ANIM_SLOWROLL,           // partial bounch, ends looking left, OFFER anim takes over
    /* 8 */ ENKAKASI_ANIM_IDLE,               // slow stretching wiggle, ends in regular position
    /* 9 */ ENKAKASI_ANIM_MAX
} EnKakasiAnimation;

static AnimationHeader* sAnimations[ENKAKASI_ANIM_MAX] = {
    &object_ka_Anim_007444, // ENKAKASI_ANIM_ARMS_CROSSED_ROCKING
    &object_ka_Anim_00686C, // ENKAKASI_ANIM_SIDEWAYS_SHAKING
    &object_ka_Anim_0081A4, // ENKAKASI_ANIM_HOPPING_REGULAR
    &object_ka_Anim_007B90, // ENKAKASI_ANIM_SPIN_REACH_OFFER
    &object_ka_Anim_0071EC, // ENKAKASI_ANIM_TWIRL
    &object_ka_Anim_007444, // ENKAKASI_ANIM_ARMS_CROSSED_STILL
    &object_ka_Anim_00686C, // ENKAKASI_ANIM_WAVE
    &object_ka_Anim_0081A4, // ENKAKASI_ANIM_SLOWROLL
    &object_ka_Anim_000214, // ENKAKASI_ANIM_IDLE
};

static u8 sAnimationModes[ENKAKASI_ANIM_MAX] = {
    ANIMMODE_LOOP, // ENKAKASI_ANIM_ARMS_CROSSED_ROCKING
    ANIMMODE_LOOP, // ENKAKASI_ANIM_SIDEWAYS_SHAKING
    ANIMMODE_LOOP, // ENKAKASI_ANIM_HOPPING_REGULAR
    ANIMMODE_ONCE, // ENKAKASI_ANIM_SPIN_REACH_OFFER
    ANIMMODE_ONCE, // ENKAKASI_ANIM_TWIRL
    ANIMMODE_ONCE, // ENKAKASI_ANIM_ARMS_CROSSED_STILL
    ANIMMODE_ONCE, // ENKAKASI_ANIM_WAVE
    ANIMMODE_ONCE, // ENKAKASI_ANIM_SLOWROLL
    ANIMMODE_ONCE, // ENKAKASI_ANIM_IDLE
};

void EnKakasi_Destroy(Actor* thisx, PlayState* play) {
    EnKakasi* this = (EnKakasi*)thisx;

    Collider_DestroyCylinder(play, &this->collider);
}

void EnKakasi_Init(Actor* thisx, PlayState* play) {
    EnKakasi* this = (EnKakasi*)thisx;
    s32 csId;
    s32 i;

    Collider_InitAndSetCylinder(play, &this->collider, &this->picto.actor, &D_80971D80);
    SkelAnime_InitFlex(play, &this->skelAnime, &object_ka_Skel_0065B0, &object_ka_Anim_000214, NULL, NULL, 0);

    this->songSummonDist = KAKASI_GET_SUMMON_DISTANCE(&this->picto.actor) * 20.0f;
    if (this->songSummonDist < 40.0f) {
        this->songSummonDist = 40.0f;
    }

    this->unkHeight = (this->picto.actor.world.rot.z * 20.0f) + 60.0f;
    this->picto.actor.world.rot.z = 0;
    this->picto.actor.attentionRangeType = ATTENTION_RANGE_0;
    if ((this->picto.actor.world.rot.x > 0) && (this->picto.actor.world.rot.x < 8)) {
        this->picto.actor.attentionRangeType = KAKASI_GET_ATTENTION_RANGE_TYPE(thisx);
    }
    this->picto.actor.shape.rot.y = this->picto.actor.world.rot.y;

    this->aboveGroundStatus = KAKASI_GET_ABOVE_GROUND(&this->picto.actor);
    this->picto.actor.world.rot.x = 0;
    this->picto.actor.flags |= ACTOR_FLAG_HOOKSHOT_PULLS_PLAYER;
    this->picto.actor.colChkInfo.mass = MASS_IMMOVABLE;
    Actor_SetScale(&this->picto.actor, 0.01f);

    i = 0;
    csId = this->picto.actor.csId;
    while (csId != CS_ID_NONE) {
        // clang-format off
        this->csIdList[i] = csId; \
        csId = CutsceneManager_GetAdditionalCsId(csId);
        // clang-format on
        i++;
    }

    if (this->aboveGroundStatus != 0) {
        if (CHECK_WEEKEVENTREG(WEEKEVENTREG_79_08)) {
            this->aboveGroundStatus = ENKAKASI_ABOVE_GROUND_TYPE;
            this->songSummonDist = 80.0f;
            EnKakasi_SetupIdleUnderground(this);
        } else {
            Actor_SetFocus(&this->picto.actor, 60.0f);
            this->picto.validationFunc = EnKakasi_ValidatePictograph;
            if (CHECK_WEEKEVENTREG(WEEKEVENTREG_83_01)) {
                EnKakasi_InitTimeSkipDialogue(this);
            } else {
                EnKakasi_SetupIdleStanding(this);
            }
        }
    } else {
        EnKakasi_SetupIdleUnderground(this);
    }
}

void EnKakasi_ChangeAnim(EnKakasi* this, s32 animIndex) {
    this->animIndex = animIndex;
    this->animEndFrame = Animation_GetLastFrame(&sAnimations[this->animIndex]->common);
    // 1: regular playback speed, 0: starting frame
    Animation_Change(&this->skelAnime, sAnimations[this->animIndex], 1.0f, 0.0f, this->animEndFrame,
                     sAnimationModes[this->animIndex], -4.0f);
}

s32 EnKakasi_ValidatePictograph(PlayState* play, Actor* thisx) {
    return Snap_ValidatePictograph(play, thisx, 0x7, &thisx->focus.pos, &thisx->shape.rot, 280.0f, 1800.0f, -1);
}

void EnKakasi_CheckAnimationSfx(EnKakasi* this) {
    if ((this->animIndex == ENKAKASI_ANIM_SIDEWAYS_SHAKING) || (this->animIndex == ENKAKASI_ANIM_ARMS_CROSSED_STILL)) {
        if (Animation_OnFrame(&this->skelAnime, 1.0f) || Animation_OnFrame(&this->skelAnime, 8.0f)) {
            Actor_PlaySfx(&this->picto.actor, NA_SE_EV_KAKASHI_SWING);
        }
    }
    if ((this->animIndex == ENKAKASI_ANIM_HOPPING_REGULAR) || (this->animIndex == ENKAKASI_ANIM_SLOWROLL)) {
        if (Animation_OnFrame(&this->skelAnime, 4.0f) || Animation_OnFrame(&this->skelAnime, 8.0f)) {
            Actor_PlaySfx(&this->picto.actor, NA_SE_EV_KAKASHI_SWING);
        }
        if (Animation_OnFrame(&this->skelAnime, 1.0f) || Animation_OnFrame(&this->skelAnime, 9.0f) ||
            Animation_OnFrame(&this->skelAnime, 16.0f)) {
            Actor_PlaySfx(&this->picto.actor, NA_SE_IT_KAKASHI_JUMP);
        }
        if (Animation_OnFrame(&this->skelAnime, 18.0f)) {
            Actor_PlaySfx(&this->picto.actor, NA_SE_EV_KAKASHI_ROLL);
        }
    }
    if ((this->animIndex == ENKAKASI_ANIM_SPIN_REACH_OFFER) || (this->animIndex == ENKAKASI_ANIM_TWIRL)) {
        if (Animation_OnFrame(&this->skelAnime, 1.0f)) {
            Actor_PlaySfx(&this->picto.actor, NA_SE_EV_KAKASH_LONGI_ROLL);
        }
    }
}

/*
 * moves the player's position relative to scarecrow during song teach, also each frame of dance the night away
 */
void EnKakasi_CheckPlayerPosition(EnKakasi* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    if (play->sceneId == SCENE_8ITEMSHOP) {
        player->actor.world.pos.x = -50.0f;
        player->actor.world.pos.z = 155.0f;
    } else if (play->sceneId == SCENE_TENMON_DAI) {
        player->actor.world.pos.x = 60.0f;
        player->actor.world.pos.z = -190.0f;
    }

    Math_SmoothStepToS(&player->actor.shape.rot.y, (this->picto.actor.yawTowardsPlayer + 0x8000), 5, 0x3E8, 0);
}

/*
 * this goes off every frame of dancing the night away, and song teaching
 * something to do with cutscene camera?
 */
void func_8096FAAC(EnKakasi* this, PlayState* play) {
    if (this->subCamId != SUB_CAM_ID_DONE) {
        Math_ApproachF(&this->subCamEye.x, this->subCamEyeNext.x, 0.4f, 4.0f);
        Math_ApproachF(&this->subCamEye.y, this->subCamEyeNext.y, 0.4f, 4.0f);
        Math_ApproachF(&this->subCamEye.z, this->subCamEyeNext.z, 0.4f, 4.0f);

        Math_ApproachF(&this->subCamAt.x, this->subCamAtNext.x, 0.4f, 4.0f);
        Math_ApproachF(&this->subCamAt.y, this->subCamAtNext.y, 0.4f, 4.0f);
        Math_ApproachF(&this->subCamAt.z, this->subCamAtNext.z, 0.4f, 4.0f);

        Math_ApproachF(&this->subCamFov, this->subCamFovNext, 0.3f, 10.0f);

        Play_SetCameraAtEye(play, this->subCamId, &this->subCamAt, &this->subCamEye);
        Play_SetCameraFov(play, this->subCamId, this->subCamFov);
    }
}

/*
 * goes off every frame of song teach, but... doing what?
 */
void func_8096FBB8(EnKakasi* this, PlayState* play) {
    if ((play->msgCtx.ocarinaButtonIndex == OCARINA_BTN_A) || (play->msgCtx.ocarinaButtonIndex == OCARINA_BTN_C_DOWN) ||
        (play->msgCtx.ocarinaButtonIndex == OCARINA_BTN_C_RIGHT) ||
        (play->msgCtx.ocarinaButtonIndex == OCARINA_BTN_C_LEFT) ||
        (play->msgCtx.ocarinaButtonIndex == OCARINA_BTN_C_UP)) {
        this->unk190++;
    }
    if ((this->unk190 != 0) && (this->animIndex != ENKAKASI_ANIM_SIDEWAYS_SHAKING)) {
        EnKakasi_ChangeAnim(this, ENKAKASI_ANIM_SIDEWAYS_SHAKING);
    }
    if (this->unk190 > 8) {
        this->unk190 = 8;
    }
    if (this->unk190 != 0) {
        Math_ApproachF(&this->skelAnime.playSpeed, 1.0f, 0.1f, 0.2f);
        this->picto.actor.shape.rot.y += 0x800;
    }
}

void EnKakasi_InitTimeSkipDialogue(EnKakasi* this) {
    EnKakasi_ChangeAnim(this, ENKAKASI_ANIM_SLOWROLL);
    this->actionFunc = EnKakasi_TimeSkipDialogue;
}

void EnKakasi_TimeSkipDialogue(EnKakasi* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    if ((gSaveContext.respawnFlag != -4) && (gSaveContext.respawnFlag != -8)) {
        if ((CURRENT_TIME != CLOCK_TIME(6, 0)) && (CURRENT_TIME != CLOCK_TIME(18, 0)) && !CHECK_EVENTINF(EVENTINF_17)) {

            if (this->picto.actor.textId == 0) {
                // dialogue after skipped time 'did you feel that? went by in an instant'
                this->picto.actor.textId = 0x1653;
                CLEAR_WEEKEVENTREG(WEEKEVENTREG_83_01);
                this->talkState = TEXT_STATE_EVENT;
                player->stateFlags1 |= PLAYER_STATE1_20;
                this->picto.actor.flags |= ACTOR_FLAG_TALK_OFFER_AUTO_ACCEPTED;
            }

            if (Actor_TalkOfferAccepted(&this->picto.actor, &play->state)) {
                player->stateFlags1 &= ~PLAYER_STATE1_20;
                this->unkState196 = 2;
                this->picto.actor.flags &= ~ACTOR_FLAG_TALK_OFFER_AUTO_ACCEPTED;
                this->actionFunc = EnKakasi_RegularDialogue;
            } else {
                Actor_OfferTalkExchange(&this->picto.actor, play, 9999.9f, 9999.9f, PLAYER_IA_MINUS1);
            }
        }
    }
}

void EnKakasi_SetupIdleStanding(EnKakasi* this) {
    this->unkState196 = 0;
    this->actionFunc = EnKakasi_IdleStanding;
}

void EnKakasi_IdleStanding(EnKakasi* this, PlayState* play) {
    u32 day = gSaveContext.save.day;
    s16 screenPosX;
    s16 screenPosY;

    // first talk to scarecrow dialogue
    this->picto.actor.textId = 0x1644;
    if (Actor_OcarinaInteractionAccepted(&this->picto.actor, &play->state)) {
        this->skelAnime.playSpeed = 1.0f;
        EnKakasi_SetupSongTeach(this, play);
        return;
    }
    if (Actor_TalkOfferAccepted(&this->picto.actor, &play->state)) {
        this->skelAnime.playSpeed = 1.0f;
        EnKakasi_SetupDialogue(this);
        return;
    }
    if (play->actorCtx.flags & ACTORCTX_FLAG_PICTO_BOX_ON) {
        Actor_GetScreenPos(play, &this->picto.actor, &screenPosX, &screenPosY);
        if ((this->picto.actor.projectedPos.z > -20.0f) && (screenPosX > 0) && (screenPosX < SCREEN_WIDTH) &&
            (screenPosY > 0) && (screenPosY < SCREEN_HEIGHT) && (this->animIndex != ENKAKASI_ANIM_SIDEWAYS_SHAKING)) {
            // faster shaking
            EnKakasi_ChangeAnim(this, ENKAKASI_ANIM_SIDEWAYS_SHAKING);
            this->skelAnime.playSpeed = 2.0f;
        }
    } else if (Player_GetMask(play) == PLAYER_MASK_KAMARO) {
        if (this->animIndex != ENKAKASI_ANIM_SIDEWAYS_SHAKING) {
            EnKakasi_ChangeAnim(this, ENKAKASI_ANIM_SIDEWAYS_SHAKING);
            this->skelAnime.playSpeed = 2.0f;
        }
    } else if ((day == 3) && gSaveContext.save.isNight) {
        this->skelAnime.playSpeed = 1.0f;
        if (this->animIndex != ENKAKASI_ANIM_SIDEWAYS_SHAKING) {
            EnKakasi_ChangeAnim(this, ENKAKASI_ANIM_SIDEWAYS_SHAKING);
        }
    } else if (this->animIndex != ENKAKASI_ANIM_IDLE) {
        EnKakasi_ChangeAnim(this, ENKAKASI_ANIM_IDLE);
    }
    if (this->picto.actor.xzDistToPlayer < 120.0f) {
        Actor_OfferTalk(&this->picto.actor, play, 100.0f);
        Actor_OfferOcarinaInteraction(&this->picto.actor, play, 100.0f, 80.0f);
    }
}

void EnKakasi_SetupDialogue(EnKakasi* this) {
    // bug? starts one animation then changes it a few lines down?
    if (this->animIndex != ENKAKASI_ANIM_SIDEWAYS_SHAKING) {
        EnKakasi_ChangeAnim(this, ENKAKASI_ANIM_SIDEWAYS_SHAKING);
    }

    this->talkState = TEXT_STATE_EVENT;
    this->unkState196 = 1;
    EnKakasi_ChangeAnim(this, ENKAKASI_ANIM_SPIN_REACH_OFFER);
    this->actionFunc = EnKakasi_RegularDialogue;
}

void EnKakasi_RegularDialogue(EnKakasi* this, PlayState* play) {
    u32 day = gSaveContext.save.day;
    f32 curFrame = this->skelAnime.curFrame;

    Math_SmoothStepToS(&this->picto.actor.shape.rot.y, this->picto.actor.yawTowardsPlayer, 5, 0x7D0, 0);
    // if first dialogue
    if ((this->picto.actor.textId != 0x1644) && (curFrame >= this->animEndFrame) &&
        (this->animIndex == ENKAKASI_ANIM_SLOWROLL)) {

        EnKakasi_ChangeAnim(this, ENKAKASI_ANIM_SPIN_REACH_OFFER);
        this->unkCounter1A4 = 0;
    }

    // if dialogue: oh sorry come back again
    if ((this->picto.actor.textId == 0x1651) || (this->picto.actor.textId == 0x1659)) {
        if ((curFrame >= this->animEndFrame) && (this->animIndex != ENKAKASI_ANIM_SPIN_REACH_OFFER)) {
            this->unkCounter1A4++;
            if (this->unkCounter1A4 >= 2) {
                this->unkCounter1A4 = 0;
                EnKakasi_ChangeAnim(this, ENKAKASI_ANIM_SPIN_REACH_OFFER);
            }
        }
    }

    if ((this->unkState1A8 == 2) && (this->unkState196 == 2)) {
        Player_SetCsActionWithHaltedActors(play, &this->picto.actor, PLAYER_CSACTION_73);
        this->unkState1A8 = 0;
    }

    if ((this->talkState == Message_GetState(&play->msgCtx)) && Message_ShouldAdvance(play)) {
        Message_CloseTextbox(play);
        if (this->talkState == TEXT_STATE_EVENT) {
            // bad song input
            if ((this->unkState196 == 2) && (this->picto.actor.textId == 0x1647)) {
                Player_SetCsActionWithHaltedActors(play, &this->picto.actor, PLAYER_CSACTION_END);
            }

            // after timeskip
            if (this->picto.actor.textId == 0x1653) {
                u32 saveContextDay2 = gSaveContext.save.day;

                if (this->animIndex != ENKAKASI_ANIM_SIDEWAYS_SHAKING) {
                    EnKakasi_ChangeAnim(this, ENKAKASI_ANIM_SIDEWAYS_SHAKING);
                }

                if ((saveContextDay2 == 3) && gSaveContext.save.isNight) {
                    // text: dangerous outside
                    this->picto.actor.textId = 0x164F;
                } else if (gSaveContext.save.isNight) {
                    // text: would you like to skip time?
                    this->picto.actor.textId = 0x164E;
                } else {
                    // text: would you like to skip time? 2
                    this->picto.actor.textId = 0x1645;
                }

                Message_ContinueTextbox(play, this->picto.actor.textId);
                return;

            } else if ((this->picto.actor.textId == 0x165D) || (this->picto.actor.textId == 0x165F) ||
                       (this->picto.actor.textId == 0x1660) || (this->picto.actor.textId == 0x1652)) {
                Player_SetCsActionWithHaltedActors(play, &this->picto.actor, PLAYER_CSACTION_4);
                if (CutsceneManager_GetCurrentCsId() == CS_ID_GLOBAL_TALK) {
                    CutsceneManager_Stop(CS_ID_GLOBAL_TALK);
                    CutsceneManager_Queue(this->csIdList[0]);
                    this->actionFunc = EnKakasi_DancingRemark;
                } else {
                    if (!CutsceneManager_IsNext(this->csIdList[0])) {
                        CutsceneManager_Queue(this->csIdList[0]);
                        this->actionFunc = EnKakasi_DancingRemark;
                    } else {
                        CutsceneManager_StartWithPlayerCs(this->csIdList[0], &this->picto.actor);
                        this->subCamId = CutsceneManager_GetCurrentSubCamId(this->picto.actor.csId);
                        this->actionFunc = EnKakasi_DancingRemark;
                    }
                }
                return;

            } else if ((this->picto.actor.textId == 0x1645) || (this->picto.actor.textId == 0x164E)) {
                this->picto.actor.textId = 0x1650; // "Shall we dance?  No Yes"
                if (this->animIndex != ENKAKASI_ANIM_SIDEWAYS_SHAKING) {
                    EnKakasi_ChangeAnim(this, ENKAKASI_ANIM_SIDEWAYS_SHAKING);
                }
                this->talkState = TEXT_STATE_CHOICE;

            } else if (this->picto.actor.textId == 0x1644) {
                if (this->animIndex != ENKAKASI_ANIM_SIDEWAYS_SHAKING) {
                    EnKakasi_ChangeAnim(this, ENKAKASI_ANIM_SIDEWAYS_SHAKING);
                }
                if (gSaveContext.save.isNight) {
                    this->picto.actor.textId = 0x164E;
                } else {
                    this->picto.actor.textId = 0x1645;
                }

            } else if (this->picto.actor.textId == 0x164F) {
                if (this->animIndex != ENKAKASI_ANIM_SIDEWAYS_SHAKING) {
                    EnKakasi_ChangeAnim(this, ENKAKASI_ANIM_SIDEWAYS_SHAKING);
                }
                this->picto.actor.textId = 0x165A;
            } else if (this->picto.actor.textId == 0x1651) {
                if (this->animIndex != ENKAKASI_ANIM_SIDEWAYS_SHAKING) {
                    EnKakasi_ChangeAnim(this, ENKAKASI_ANIM_SIDEWAYS_SHAKING);
                }
                this->picto.actor.textId = 0x1654;
            } else if (this->picto.actor.textId == 0x1654) {
                this->picto.actor.textId = 0x1655;
            } else if (this->picto.actor.textId == 0x1655) {
                this->picto.actor.textId = 0x1656;
                this->talkState = TEXT_STATE_CHOICE;
            } else if (this->picto.actor.textId == 0x1658) {
                this->picto.actor.textId = 0x1659;
            } else if (this->picto.actor.textId == 0x165A) {
                this->picto.actor.textId = 0x165B;
            } else if (this->picto.actor.textId == 0x165B) {
                this->picto.actor.textId = 0x165C;
                this->talkState = TEXT_STATE_CHOICE;

            } else if (this->picto.actor.textId == 0x165E) {
                this->picto.actor.textId = 0x165F;
            } else {
                EnKakasi_SetupIdleStanding(this);
                return;
            }
        } else {
            this->talkState = TEXT_STATE_EVENT;

            if (play->msgCtx.choiceIndex == 1) {
                Audio_PlaySfx_MessageDecide();
                if (this->picto.actor.textId == 0x1656) {
                    this->picto.actor.textId = 0x1658;
                } else if (this->picto.actor.textId == 0x165C) {
                    this->picto.actor.textId = 0x165E;
                } else if ((day == 3) && gSaveContext.save.isNight) {
                    this->picto.actor.textId = 0x164F;
                } else {
                    this->picto.actor.textId = 0x1652;
                }
                EnKakasi_ChangeAnim(this, ENKAKASI_ANIM_HOPPING_REGULAR);
            } else {
                Audio_PlaySfx_MessageCancel();
                if (this->picto.actor.textId == 0x1656) { // would you like to learn a song? yes/no
                    this->picto.actor.textId = 0x1657;
                } else if (this->picto.actor.textId == 0x165C) { // would you like to learn a song? yes/no
                    this->picto.actor.textId = 0x165D;
                } else {
                    this->picto.actor.textId = 0x1651;
                }
                this->unkCounter1A4 = 0;
                if (this->animIndex != ENKAKASI_ANIM_ARMS_CROSSED_ROCKING) {
                    EnKakasi_ChangeAnim(this, ENKAKASI_ANIM_ARMS_CROSSED_ROCKING);
                }
            }
        }
        Message_ContinueTextbox(play, this->picto.actor.textId);
    }
}

void EnKakasi_SetupSongTeach(EnKakasi* this, PlayState* play) {
    this->picto.actor.textId = 0x1646;
    Message_StartTextbox(play, this->picto.actor.textId, &this->picto.actor);
    this->subCamId = SUB_CAM_ID_DONE;
    this->subCamFov = 0.0f;
    this->subCamFovNext = 60.0f;
    EnKakasi_ChangeAnim(this, ENKAKASI_ANIM_TWIRL);
    this->unkState196 = 2;
    this->actionFunc = EnKakasi_OcarinaRemark;
}

/*
 * you took out ocarina hes talking about how he wants to hear you play him a song
 * before actually teaching
 */
void EnKakasi_OcarinaRemark(EnKakasi* this, PlayState* play) {
    if ((Message_GetState(&play->msgCtx) == TEXT_STATE_EVENT) && Message_ShouldAdvance(play)) {
        Message_DisplayOcarinaStaff(play, OCARINA_ACTION_SCARECROW_SPAWN_RECORDING);
        this->unkState1A8 = 0;
        if (CutsceneManager_GetCurrentCsId() == CS_ID_GLOBAL_TALK) {
            CutsceneManager_Stop(CS_ID_GLOBAL_TALK);
            CutsceneManager_Queue(this->csIdList[0]);
            this->actionFunc = EnKakasi_TeachingSong;

        } else if (!CutsceneManager_IsNext(this->csIdList[0])) {
            CutsceneManager_Queue(this->csIdList[0]);
            this->actionFunc = EnKakasi_TeachingSong;

        } else {
            this->unkState1A8 = 1;
            CutsceneManager_StartWithPlayerCs(this->csIdList[0], &this->picto.actor);
            this->subCamId = CutsceneManager_GetCurrentSubCamId(this->picto.actor.csId);
            Math_Vec3f_Copy(&this->unk22C, &this->picto.actor.home.pos);
            this->actionFunc = EnKakasi_TeachingSong;
        }
    }
}

void EnKakasi_TeachingSong(EnKakasi* this, PlayState* play) {
    Vec3f tempVec;

    EnKakasi_CheckPlayerPosition(this, play);
    Math_SmoothStepToS(&this->picto.actor.shape.rot.y, this->picto.actor.home.rot.y, 1, 0xBB8, 0);
    if (this->unkState1A8 == 0) {
        if (CutsceneManager_GetCurrentCsId() == CS_ID_GLOBAL_TALK) {
            CutsceneManager_Stop(CS_ID_GLOBAL_TALK);
            CutsceneManager_Queue(this->csIdList[0]);
            return;
        }
        if (!CutsceneManager_IsNext(this->csIdList[0])) {
            CutsceneManager_Queue(this->csIdList[0]);
            return;
        }
        CutsceneManager_StartWithPlayerCs(this->csIdList[0], &this->picto.actor);
        this->subCamId = CutsceneManager_GetCurrentSubCamId(this->picto.actor.csId);
        Math_Vec3f_Copy(&this->unk22C, &this->picto.actor.home.pos);
        this->unkState1A8 = 1;
        this->unkState1A8 = 1;
    }

    if (this->unkState1A8 == 1) {
        this->unk22C.y = this->picto.actor.home.pos.y + 50.0f;
        this->subCamEyeNext.x = D_80971DCC[this->unk190].x;
        this->subCamEyeNext.y = D_80971DCC[this->unk190].y;
        this->subCamEyeNext.z = D_80971DCC[this->unk190].z;

        Math_Vec3f_Copy(&tempVec, &this->subCamEyeNext);
        OLib_Vec3fAdd(&this->picto.actor.home, &tempVec, &this->subCamEyeNext, OLIB_ADD_OFFSET);
        Math_Vec3f_Copy(&this->subCamAtNext, &this->unk22C);
        Math_Vec3f_Copy(&this->subCamEye, &this->subCamEyeNext);
        Math_Vec3f_Copy(&this->subCamAt, &this->subCamAtNext);
        func_8096FAAC(this, play);
        func_8096FBB8(this, play);

        if (play->msgCtx.ocarinaMode == OCARINA_MODE_END) { // song failed
            this->unk190 = 0;
            this->unkCounter1A4 = 0;
            CutsceneManager_Stop(this->csIdList[0]);
            Actor_PlaySfx(&this->picto.actor, NA_SE_EN_YASE_DEAD);
            this->unkState196 = 2;
            this->subCamId = SUB_CAM_ID_DONE;
            this->picto.actor.textId = 0x1647;
            this->unkState1A8 = 2;
            this->talkState = TEXT_STATE_EVENT;
            EnKakasi_ChangeAnim(this, ENKAKASI_ANIM_ARMS_CROSSED_ROCKING);
            this->actionFunc = EnKakasi_RegularDialogue;

        } else if (play->msgCtx.ocarinaMode == OCARINA_MODE_EVENT) { // song success
            this->postTeachTimer = 30;
            this->skelAnime.playSpeed = 2.0f;
            EnKakasi_ChangeAnim(this, ENKAKASI_ANIM_HOPPING_REGULAR);
            this->actionFunc = EnKakasi_PostSongLearnTwirl;
        }
    }
}

void EnKakasi_PostSongLearnTwirl(EnKakasi* this, PlayState* play) {
    f32 curFrame = this->skelAnime.curFrame;

    if ((this->postTeachTimer == 0) && (this->animIndex != ENKAKASI_ANIM_TWIRL)) {
        EnKakasi_ChangeAnim(this, ENKAKASI_ANIM_TWIRL);
        this->skelAnime.playSpeed = 2.0f;
    }
    if ((this->postTeachTimer == 0) && (this->animIndex == ENKAKASI_ANIM_TWIRL) && (curFrame >= this->animEndFrame)) {
        EnKakasi_SetupPostSongLearnDialogue(this, play);
    }
}

void EnKakasi_SetupPostSongLearnDialogue(EnKakasi* this, PlayState* play) {
    CutsceneManager_Stop(this->csIdList[0]);
    play->msgCtx.ocarinaMode = OCARINA_MODE_END;
    this->unk190 = 0;
    this->unkCounter1A4 = 0;
    EnKakasi_ChangeAnim(this, ENKAKASI_ANIM_HOPPING_REGULAR);
    this->subCamId = SUB_CAM_ID_DONE;
    this->talkState = TEXT_STATE_EVENT;
    this->unkState1A8 = 1;
    this->actionFunc = EnKakasi_PostSongLearnDialogue;
    this->subCamFov = 0.0f;
    this->subCamFovNext = 60.0f;
}

void EnKakasi_PostSongLearnDialogue(EnKakasi* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    f32 curFrame = this->skelAnime.curFrame;
    Vec3f vec3fCopy;

    Math_SmoothStepToS(&this->picto.actor.shape.rot.y, this->picto.actor.home.rot.y, 1, 0xBB8, 0);
    Math_SmoothStepToS(&player->actor.shape.rot.y, this->picto.actor.yawTowardsPlayer + 0x8000, 5, 0x3E8, 0);

    if (this->unk190 == 0) {
        Message_CloseTextbox(play);
        Player_SetCsActionWithHaltedActors(play, &this->picto.actor, PLAYER_CSACTION_86);
        this->picto.actor.textId = 0x1648;
        Message_StartTextbox(play, (this->picto.actor.textId), &this->picto.actor);
        this->unkState1A8 = 0;
        this->unk190 = 1;
    }

    if ((this->picto.actor.textId == 0x1648) && (this->animIndex == ENKAKASI_ANIM_HOPPING_REGULAR) &&
        (curFrame >= this->animEndFrame)) {
        this->unkCounter1A4++;
        if (this->unkCounter1A4 >= 2) {
            EnKakasi_ChangeAnim(this, ENKAKASI_ANIM_ARMS_CROSSED_ROCKING);
        }
    }

    if ((this->picto.actor.textId == 0x164B) && (this->animIndex == ENKAKASI_ANIM_ARMS_CROSSED_ROCKING) &&
        (curFrame >= this->animEndFrame)) {
        this->unkCounter1A4++;
        if (this->unkCounter1A4 >= 2) {
            EnKakasi_ChangeAnim(this, ENKAKASI_ANIM_SPIN_REACH_OFFER);
        }
    }

    if (this->unkState1A8 == 0) {
        if (CutsceneManager_GetCurrentCsId() == CS_ID_GLOBAL_TALK) {
            CutsceneManager_Stop(CS_ID_GLOBAL_TALK);
            CutsceneManager_Queue(this->csIdList[0]);
            return;
        }
        if (!CutsceneManager_IsNext(this->csIdList[0])) {
            CutsceneManager_Queue(this->csIdList[0]);
            return;
        }
        Math_Vec3f_Copy(&this->unk22C, &this->picto.actor.home.pos);
        CutsceneManager_StartWithPlayerCs(this->csIdList[0], &this->picto.actor);
        this->subCamId = CutsceneManager_GetCurrentSubCamId(this->picto.actor.csId);
        Player_SetCsActionWithHaltedActors(play, &this->picto.actor, PLAYER_CSACTION_86);
        this->unkState1A8 = 1;
    }

    if (this->subCamId != SUB_CAM_ID_DONE) {
        this->unk22C.y = this->picto.actor.home.pos.y + 50.0f;
        EnKakasi_CheckPlayerPosition(this, play);
        this->subCamEyeNext.x = D_80971FA0[this->unk190].x;
        this->subCamEyeNext.y = D_80971FA0[this->unk190].y;
        this->subCamEyeNext.z = D_80971FA0[this->unk190].z;
        Math_Vec3f_Copy(&vec3fCopy, &this->subCamEyeNext);
        OLib_Vec3fAdd(&this->picto.actor.home, &vec3fCopy, &this->subCamEyeNext, OLIB_ADD_OFFSET);
        this->subCamAtNext.x = D_80971FE8[this->unk190].x + this->unk22C.x;
        this->subCamAtNext.y = D_80971FE8[this->unk190].y + this->unk22C.y;
        this->subCamAtNext.z = D_80971FE8[this->unk190].z + this->unk22C.z;
        Math_Vec3f_Copy(&this->subCamEye, &this->subCamEyeNext);
        Math_Vec3f_Copy(&this->subCamAt, &this->subCamAtNext);
    }

    func_8096FAAC(this, play);

    if ((this->unkState1A8 != 0) && (Message_GetState(&play->msgCtx) == this->talkState) &&
        Message_ShouldAdvance(play)) {

        Message_CloseTextbox(play);

        if (this->talkState == TEXT_STATE_EVENT) {
            this->unk190++;
            if (this->unk190 > 5) {
                this->unk190 = 5;
            }

            if (this->picto.actor.textId == 0x1648) {
                Player_SetCsActionWithHaltedActors(play, &this->picto.actor, PLAYER_CSACTION_WAIT);
                this->picto.actor.textId = 0x1649;
                if (this->animIndex != ENKAKASI_ANIM_ARMS_CROSSED_ROCKING) {
                    EnKakasi_ChangeAnim(this, ENKAKASI_ANIM_ARMS_CROSSED_ROCKING);
                }

            } else if (this->picto.actor.textId == 0x1649) {
                this->picto.actor.textId = 0x1660;
                this->talkState = TEXT_STATE_CHOICE;

            } else if (this->picto.actor.textId == 0x164A) {
                this->picto.actor.textId = 0x164B;

            } else if (this->picto.actor.textId == 0x164B) {
                this->picto.actor.textId = 0x164C;
                EnKakasi_ChangeAnim(this, ENKAKASI_ANIM_TWIRL);

            } else {
                if ((this->picto.actor.textId == 0x164C) || (this->picto.actor.textId == 0x1661)) {
                    EnKakasi_SetupDigAway(this);
                    return;
                }
            }

        } else {
            this->talkState = TEXT_STATE_EVENT;
            if (play->msgCtx.choiceIndex == 1) {
                Audio_PlaySfx_MessageDecide();
                this->picto.actor.textId = 0x164A;
            } else {
                Audio_PlaySfx_MessageCancel();
                this->picto.actor.textId = 0x1661;
            }
        }
        Message_ContinueTextbox(play, this->picto.actor.textId);
    }
}

/*
 * talking before dancing the night away, and cutscene setup
 */
void EnKakasi_DancingRemark(EnKakasi* this, PlayState* play) {
    u32 currentDay = gSaveContext.save.day;

    this->unkState196 = 3;
    if (CutsceneManager_GetCurrentCsId() == CS_ID_GLOBAL_TALK) {
        CutsceneManager_Stop(CS_ID_GLOBAL_TALK);
        CutsceneManager_Queue(this->csIdList[0]);
    } else if (!CutsceneManager_IsNext(this->csIdList[0])) {
        CutsceneManager_Queue(this->csIdList[0]);
    } else {
        CutsceneManager_StartWithPlayerCs(this->csIdList[0], &this->picto.actor);
        this->subCamId = CutsceneManager_GetCurrentSubCamId(this->picto.actor.csId);
        if ((currentDay == 3) && gSaveContext.save.isNight) {
            EnKakasi_SetupDigAway(this);
        } else {
            Audio_PlaySubBgm(NA_BGM_SARIAS_SONG);
            EnKakasi_SetupDanceNightAway(this);
        }
    }
}

void EnKakasi_SetupDanceNightAway(EnKakasi* this) {
    this->unk190 = 0;
    this->unkCounter1A4 = 0;
    this->subCamFov = 0.0f;
    this->subCamFovNext = 60.0f;
    EnKakasi_ChangeAnim(this, ENKAKASI_ANIM_TWIRL);
    Math_Vec3f_Copy(&this->unk22C, &this->picto.actor.home.pos);
    Play_EnableMotionBlur(180);
    this->actionFunc = EnKakasi_DancingNightAway;
}

void EnKakasi_DancingNightAway(EnKakasi* this, PlayState* play) {
    f32 curFrame = this->skelAnime.curFrame;
    Vec3f localVec3f;
    Player* player;

    EnKakasi_CheckPlayerPosition(this, play);
    Math_SmoothStepToS(&this->picto.actor.shape.rot.y, this->picto.actor.home.rot.y, 1, 0xBB8, 0);
    this->unk22C.y = this->picto.actor.home.pos.y + 50.0f;

    this->subCamEyeNext.x = D_80971E38[this->unk190].x;
    this->subCamEyeNext.y = D_80971E38[this->unk190].y;
    this->subCamEyeNext.z = D_80971E38[this->unk190].z;
    Math_Vec3f_Copy(&localVec3f, &this->subCamEyeNext);
    OLib_Vec3fAdd(&this->picto.actor.home, &localVec3f, &this->subCamEyeNext, OLIB_ADD_OFFSET);

    this->subCamAtNext.x = D_80971EEC[this->unk190].x + this->unk22C.x;
    this->subCamAtNext.y = D_80971EEC[this->unk190].y + this->unk22C.y;
    this->subCamAtNext.z = D_80971EEC[this->unk190].z + this->unk22C.z;
    if ((this->unk190 != 6) && (this->unk190 != 7)) {
        Math_Vec3f_Copy(&this->subCamEye, &this->subCamEyeNext);
        Math_Vec3f_Copy(&this->subCamAt, &this->subCamAtNext);
    }
    if ((this->unk190 >= 7) && (this->unk190 != 0xE)) {
        this->picto.actor.shape.rot.y += 0x800;
    }

    func_8096FAAC(this, play);

    // switching through manual cutscene states
    switch (this->unk190) {
        case 0:
            // twirling
            this->unk204 = 0x28;
            this->unk190++;
            break;

        case 1:
            // end of twirl, start shaking back and forth
            if ((this->unk204 == 0) && (curFrame >= this->animEndFrame)) {
                this->unk204 = 0x14;
                this->unk190++;
                EnKakasi_ChangeAnim(this, ENKAKASI_ANIM_SIDEWAYS_SHAKING);
            }
            break;

        case 2:
            // camera view on the left
            if (this->unk204 == 0) {
                this->unk204 = 0x14;
                this->unk190++;
            }
            break;

        case 3:
            // camera view on the right
            if (this->unk204 == 0) {
                this->unk204 = 0x14;
                this->unk190++;
            }
            break;

        case 4:
            // camera view close up front
            if (this->unk204 == 0) {
                this->unk204 = 0x14;
                this->unk190++;
            }
            break;

        case 5:
            // camera view zoomed out front
            if (this->unk204 == 0) {
                this->unk204 = 0xF;
                this->unk190++;
            }
            break;

        case 6:
            // zoom in and sweep down in from case 5
            if (this->unk204 == 0) {
                this->unk204 = 0xF;
                this->unk190++;
            }
            break;

        case 7:
            // zoom in close from case 6
            if (this->unk204 == 0) {
                this->unk204 = 0xA;
                this->unk190++;
            }
            break;

        case 8:
        case 9:
        case 10:
        case 11:
        case 12:
        case 13:
            // multiple quick angle cuts viewing from multiple sides
            if (this->unk204 == 0) {
                this->unk190++;
                this->unk204 = 0xA;
                if (this->unk190 == 0xE) {
                    Player_SetCsActionWithHaltedActors(play, &this->picto.actor, PLAYER_CSACTION_73);
                    Play_DisableMotionBlur();
                    this->unk204 = 0x14;
                }
            }
            break;

        case 14:
            // goes off once for some camera changes,
            // otherwise it's the end when camera is back to normal and Player is confused
            Math_SmoothStepToS(&this->picto.actor.shape.rot.y, this->picto.actor.yawTowardsPlayer, 5, 0x3E8, 0);
            if (this->unk204 == 0) {
                player = GET_PLAYER(play);

                Play_SetRespawnData(play, RESPAWN_MODE_DOWN, Entrance_CreateFromSpawn(0), player->unk_3CE,
                                    PLAYER_PARAMS(0xFF, PLAYER_START_MODE_B), &player->unk_3C0, player->unk_3CC);
                func_80169EFC(play);

                if (GameInteractor_Should(VB_SCARECROW_DANCE_SET_TIME, true)) {
                    if ((CURRENT_TIME > CLOCK_TIME(18, 0)) || (CURRENT_TIME < CLOCK_TIME(6, 0))) {
                        gSaveContext.save.time = CLOCK_TIME(6, 0);
                        gSaveContext.respawnFlag = -4;
                        SET_EVENTINF(EVENTINF_TRIGGER_DAYTELOP);
                    } else {
                        gSaveContext.save.time = CLOCK_TIME(18, 0);
                        gSaveContext.respawnFlag = -8;
                    }
                }
                SET_WEEKEVENTREG(WEEKEVENTREG_83_01);
                this->unk190 = 0;
                this->actionFunc = EnKakasi_DoNothing;
            }
            break;

        default:
            break;
    }
}

void EnKakasi_DoNothing(EnKakasi* this, PlayState* play) {
}

void EnKakasi_SetupDigAway(EnKakasi* this) {
    if (this->animIndex != ENKAKASI_ANIM_SIDEWAYS_SHAKING) {
        EnKakasi_ChangeAnim(this, ENKAKASI_ANIM_SIDEWAYS_SHAKING);
    }
    this->unk190 = 0;
    this->unkCounter1A4 = 0;
    this->subCamFovNext = 60.0f;
    this->subCamFov = 60.0f;
    Math_Vec3f_Copy(&this->unk22C, &this->picto.actor.home.pos);
    this->unkState196 = 4;
    this->actionFunc = EnKakasi_DiggingAway;
}

void EnKakasi_DiggingAway(EnKakasi* this, PlayState* play) {
    Vec3f tempunk238;
    Vec3f tempWorldPos;

    if (this->subCamId != SUB_CAM_ID_DONE) {
        this->unk22C.y = this->picto.actor.home.pos.y + 50.0f;
        this->subCamEyeNext.x = D_80972030.x;
        this->subCamEyeNext.y = D_80972030.y;
        this->subCamEyeNext.z = D_80972030.z;

        Math_Vec3f_Copy(&tempunk238, &this->subCamEyeNext);
        OLib_Vec3fAdd(&this->picto.actor.home, &tempunk238, &this->subCamEyeNext, OLIB_ADD_OFFSET);
        this->subCamAtNext.x = ((f32)D_8097203C.x) + this->unk22C.x; // cast req
        this->subCamAtNext.y = ((f32)D_8097203C.y) + this->unk22C.y;
        this->subCamAtNext.z = ((f32)D_8097203C.z) + this->unk22C.z;
        Math_Vec3f_Copy(&this->subCamEye, &this->subCamEyeNext);
        Math_Vec3f_Copy(&this->subCamAt, &this->subCamAtNext);
        func_8096FAAC(this, play);
    }

    if (this->unkCounter1A4 < 15) {
        this->unkCounter1A4++;
        return;
    }

    this->picto.actor.shape.rot.y += 0x3000;
    Math_SmoothStepToS(&this->unk190, 500, 5, 50, 0);
    if ((play->gameplayFrames % 4) == 0) {
        Math_Vec3f_Copy(&tempWorldPos, &this->picto.actor.world.pos);
        tempWorldPos.y = this->picto.actor.floorHeight;
        tempWorldPos.x += Rand_CenteredFloat(2.0f);
        tempWorldPos.z += Rand_CenteredFloat(2.0f);

        if (play->sceneId == SCENE_8ITEMSHOP) {
            EffectSsGSplash_Spawn(play, &tempWorldPos, 0, 0, 0, Rand_CenteredFloat(100.0f) + 200.0f);
            SoundSource_PlaySfxAtFixedWorldPos(play, &tempWorldPos, 0x32, NA_SE_EV_BOMB_DROP_WATER);

        } else {
            Actor_SpawnFloorDustRing(play, &this->picto.actor, &this->picto.actor.world.pos,
                                     this->picto.actor.shape.shadowScale - 20.0f, 5, 4.0f, 200, 10, true);
            Actor_PlaySfx(&this->picto.actor, NA_SE_EN_AKINDONUTS_HIDE);
        }
    }

    Math_ApproachF(&this->picto.actor.shape.yOffset, -6000.0f, 0.5f, 200.0f);
    if (fabsf(this->picto.actor.shape.yOffset + 6000.0f) < 10.0f) {
        SET_WEEKEVENTREG(WEEKEVENTREG_79_08);
        Player_SetCsActionWithHaltedActors(play, &this->picto.actor, PLAYER_CSACTION_END);
        CutsceneManager_Stop(this->csIdList[0]);
        this->aboveGroundStatus = ENKAKASI_ABOVE_GROUND_TYPE;
        this->songSummonDist = 80.0f;
        EnKakasi_SetupIdleUnderground(this);
    }
}

void EnKakasi_SetupIdleUnderground(EnKakasi* this) {
    this->picto.actor.shape.yOffset = -7000.0f;
    this->picto.actor.draw = NULL;
    this->picto.actor.flags |= ACTOR_FLAG_LOCK_ON_DISABLED;
    this->unkState196 = 5;
    this->actionFunc = EnKakasi_IdleUnderground;
}

void EnKakasi_IdleUnderground(EnKakasi* this, PlayState* play) {
    if (GameInteractor_Should(VB_NEED_SCARECROW_SONG,
                              CHECK_WEEKEVENTREG(WEEKEVENTREG_79_08) &&
                                  (this->picto.actor.xzDistToPlayer < this->songSummonDist) &&
                                  ((BREG(1) != 0) || (play->msgCtx.ocarinaMode == OCARINA_MODE_PLAYED_SCARECROW_SPAWN)),
                              this)) {
        this->picto.actor.flags &= ~ACTOR_FLAG_LOCK_ON_DISABLED;
        play->msgCtx.ocarinaMode = OCARINA_MODE_END;
        this->actionFunc = EnKakasi_SetupRiseOutOfGround;
    }
}

void EnKakasi_SetupRiseOutOfGround(EnKakasi* this, PlayState* play) {
    s32 csIdIndex;

    csIdIndex = 0;
    if (this->aboveGroundStatus == ENKAKASI_ABOVE_GROUND_TYPE) {
        csIdIndex = 1;
    }

    if (CutsceneManager_GetCurrentCsId() == CS_ID_GLOBAL_TALK) {
        CutsceneManager_Stop(CS_ID_GLOBAL_TALK);
        CutsceneManager_Queue(this->csIdList[csIdIndex]);
    } else if (!CutsceneManager_IsNext(this->csIdList[csIdIndex])) {
        CutsceneManager_Queue(this->csIdList[csIdIndex]);
    } else {
        CutsceneManager_StartWithPlayerCs(this->csIdList[csIdIndex], &this->picto.actor);
        Actor_PlaySfx(&this->picto.actor, NA_SE_EN_AKINDONUTS_HIDE);
        this->picto.actor.draw = EnKakasi_Draw;
        this->unkState196 = 6;
        this->actionFunc = EnKakasi_RisingOutOfGround;
    }
}

void EnKakasi_RisingOutOfGround(EnKakasi* this, PlayState* play) {
    this->picto.actor.shape.rot.y += 0x3000;

    if (this->animIndex != ENKAKASI_ANIM_SIDEWAYS_SHAKING) {
        EnKakasi_ChangeAnim(this, ENKAKASI_ANIM_SIDEWAYS_SHAKING);
    }
    if (this->picto.actor.shape.yOffset < -10.0f) {
        if ((play->gameplayFrames % 8) == 0) {
            Actor_SpawnFloorDustRing(play, &this->picto.actor, &this->picto.actor.world.pos,
                                     this->picto.actor.shape.shadowScale - 20.0f, 10, 8.0f, 500, 10, true);
            Actor_PlaySfx(&this->picto.actor, NA_SE_EN_AKINDONUTS_HIDE);
        }
        Math_ApproachF(&this->picto.actor.shape.yOffset, 0.0f, 0.5f, 200.0f);
    } else {
        EnKakasi_SetupIdleRisen(this);
    }
}

void EnKakasi_SetupIdleRisen(EnKakasi* this) {
    // text: great to see you again
    this->picto.actor.textId = 0x164D;
    this->unkState196 = 7;
    this->actionFunc = EnKakasi_IdleRisen;
    this->picto.actor.shape.yOffset = 0.0f;
}

void EnKakasi_IdleRisen(EnKakasi* this, PlayState* play) {
    Math_SmoothStepToS(&this->picto.actor.shape.rot.y, this->picto.actor.yawTowardsPlayer, 5, 0x3E8, 0);
    if (Actor_TalkOfferAccepted(&this->picto.actor, &play->state)) {
        this->actionFunc = EnKakasi_RisenDialogue;
    } else {
        Actor_OfferTalk(&this->picto.actor, play, 70.0f);
    }
}

void EnKakasi_RisenDialogue(EnKakasi* this, PlayState* play) {
    Math_SmoothStepToS(&this->picto.actor.shape.rot.y, this->picto.actor.yawTowardsPlayer, 5, 0x3E8, 0);

    if ((Message_GetState(&play->msgCtx) == TEXT_STATE_EVENT) && Message_ShouldAdvance(play)) {
        Message_CloseTextbox(play);
        EnKakasi_SetupIdleRisen(this);
    }
}

void EnKakasi_Update(Actor* thisx, PlayState* play) {
    EnKakasi* this = (EnKakasi*)thisx;
    s32 pad;

    SkelAnime_Update(&this->skelAnime);
    if (this->picto.actor.draw != NULL) {
        EnKakasi_CheckAnimationSfx(this);
    }

    this->picto.actor.world.rot.y = this->picto.actor.shape.rot.y;
    DECR(this->postTeachTimer);
    DECR(this->unk204);

    if (this->unkState196 != 5) {
        if ((this->unk1BC.x != 0.0f) || (this->unk1BC.z != 0.0f)) {
            Math_Vec3f_Copy(&this->picto.actor.focus.pos, &this->unk1BC);
            this->picto.actor.focus.pos.y += 10.0f;
            if (this->subCamId == SUB_CAM_ID_DONE) {
                Math_Vec3s_Copy(&this->picto.actor.focus.rot, &this->picto.actor.world.rot);
            } else {
                Math_Vec3s_Copy(&this->picto.actor.focus.rot, &this->picto.actor.home.rot);
            }
        }
    } else {
        Actor_SetFocus(&this->picto.actor, this->unkHeight);
    }

    this->actionFunc(this, play);
    Actor_MoveWithGravity(&this->picto.actor);
    Actor_UpdateBgCheckInfo(play, &this->picto.actor, 50.0f, 50.0f, 100.0f,
                            UPDBGCHECKINFO_FLAG_4 | UPDBGCHECKINFO_FLAG_8 | UPDBGCHECKINFO_FLAG_10);
    if (this->picto.actor.draw != NULL) {
        Collider_UpdateCylinder(&this->picto.actor, &this->collider);
        CollisionCheck_SetAC(play, &play->colChkCtx, &this->collider.base);
        CollisionCheck_SetOC(play, &play->colChkCtx, &this->collider.base);
    }
}

void EnKakasi_PostLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot, Actor* thisx) {
    EnKakasi* this = (EnKakasi*)thisx;

    if (limbIndex == OBJECT_KA_LIMB_04) {
        Matrix_MultVec3f(&gZeroVec3f, &this->unk1BC);
    }
}

void EnKakasi_Draw(Actor* thisx, PlayState* play) {
    EnKakasi* this = (EnKakasi*)thisx;
    Gfx_SetupDL25_Opa(play->state.gfxCtx);
    SkelAnime_DrawFlexOpa(play, this->skelAnime.skeleton, this->skelAnime.jointTable, this->skelAnime.dListCount, NULL,
                          EnKakasi_PostLimbDraw, &this->picto.actor);
}
