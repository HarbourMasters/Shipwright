/*
 * File: z_en_dt.c
 * Overlay: ovl_En_Dt
 * Description: Mayor Dotour
 *
 * This actor handles the logic for the meeting event between the mayor, Mutoh, and Baisen.
 * It is also the source of a heart piece and two bomber notebook events.
 */

#include "z_en_dt.h"
#include "objects/object_dt/object_dt.h"
#include "overlays/actors/ovl_En_Muto/z_en_muto.h"
#include "overlays/actors/ovl_En_Baisen/z_en_baisen.h"

#include "objects/object_dt/object_dt.h"
#include "overlays/actors/ovl_En_Baisen/z_en_baisen.h"
#include "overlays/actors/ovl_En_Muto/z_en_muto.h"

#define FLAGS (ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_FRIENDLY)

void EnDt_Init(Actor* thisx, PlayState* play);
void EnDt_Destroy(Actor* thisx, PlayState* play);
void EnDt_Update(Actor* thisx, PlayState* play);
void EnDt_Draw(Actor* thisx, PlayState* play);

void EnDt_ChangeAnim(EnDt* this, s32 animIndex);
void EnDt_SetupRegularState(EnDt* this, PlayState* play);
void EnDt_SetupCutsceneNpcs(EnDt* this, PlayState* play);
void EnDt_OfferRegularTalk(EnDt* this, PlayState* play);
void EnDt_SetupMeetingCutscene(EnDt* this, PlayState* play);
void EnDt_UpdateMeetingCutscene(EnDt* this, PlayState* play);
void EnDt_FinishMeetingCutscene(EnDt* this, PlayState* play);
void EnDt_OfferMeetingReward(EnDt* this, PlayState* play);
void EnDt_OfferFinalNightTalk(EnDt* this, PlayState* play);
void EnDt_TriggerFinalNightTalkEvent(EnDt* this, PlayState* play);
void EnDt_TriggerMeetingRewardEvent(EnDt* this, PlayState* play);
void EnDt_TriggerMeetingNotebookEvent(EnDt* this, PlayState* play);
void EnDt_SetupFinalNightState(EnDt* this, PlayState* play);
void EnDt_UpdateAppearance(EnDt* this);
void EnDt_StartFinalNightTalk(EnDt* this);
void EnDt_UpdateCutsceneFocusTarget(EnDt* this);

typedef enum {
    /* 0 */ EN_DT_NPC_STATE_DEFAULT,
    /* 1 */ EN_DT_NPC_STATE_IDLE,
    /* 2 */ EN_DT_NPC_STATE_VIEWING_MEETING,
    /* 3 */ EN_DT_NPC_STATE_OFFERED_MEETING_REWARD,
    /* 4 */ EN_DT_NPC_STATE_WAIT_FINAL_NIGHT_TALK,
    /* 5 */ EN_DT_NPC_STATE_DONE_FINAL_NIGHT_TALK
} EnDtNpcState;

typedef enum {
    /* 0 */ EN_DT_CS_STATE_NONE,
    /* 1 */ EN_DT_CS_STATE_WAITING,
    /* 2 */ EN_DT_CS_STATE_PLAYING
} EnDtCutsceneState;

typedef enum {
    /* 0 */ EN_DT_CS_FOCUS_TARGET_MUTO,
    /* 1 */ EN_DT_CS_FOCUS_TARGET_BAISEN,
    /* 2 */ EN_DT_CS_FOCUS_TARGET_MAYOR
} EnDtCutsceneFocusTarget;

typedef enum {
    /* 0 */ EN_DT_APPEARANCE_DEFAULT,
    /* 1 */ EN_DT_APPEARANCE_PRE_MEETING,
    /* 2 */ EN_DT_APPEARANCE_POST_MEETING,
    /* 3 */ EN_DT_APPEARANCE_UNUSED,
    /* 4 */ EN_DT_APPEARANCE_RESOLUTION_TALK,
    /* 5 */ EN_DT_APPEARANCE_RESOLVED_MEETING,
    /* 6 */ EN_DT_APPEARANCE_MAX
} EnDtAppearance;

typedef enum {
    /* 0 */ EN_DT_ANIMATION_WAIT,
    /* 1 */ EN_DT_ANIMATION_DISTRESS,
    /* 2 */ EN_DT_ANIMATION_SHOCK,
    /* 3 */ EN_DT_ANIMATION_TAP_DESK,
    /* 4 */ EN_DT_ANIMATION_UPRIGHT,
    /* 5 */ EN_DT_ANIMATION_SIT_UP,
    /* 6 */ EN_DT_ANIMATION_MAX
} EnDtAnimation;

typedef enum {
    /* 0 */ EN_DT_EYE_TEXTURE_SHOCK,
    /* 1 */ EN_DT_EYE_TEXTURE_OPEN,
    /* 2 */ EN_DT_EYE_TEXTURE_CLOSED,
    /* 3 */ EN_DT_EYE_TEXTURE_LOOK_DOWN,
    /* 4 */ EN_DT_EYE_TEXTURE_SQUINT,
    /* 5 */ EN_DT_EYE_TEXTURE_MAX
} EnDtEyeTextures;

typedef enum {
    /* 0 */ EN_DT_BROW_TEXTURE_HIGH,
    /* 1 */ EN_DT_BROW_TEXTURE_MID,
    /* 2 */ EN_DT_BROW_TEXTURE_LOW,
    /* 3 */ EN_DT_BROW_TEXTURE_MAX
} EnDtBrowTextures;

ActorProfile En_Dt_Profile = {
    /**/ ACTOR_EN_DT,
    /**/ ACTORCAT_NPC,
    /**/ FLAGS,
    /**/ OBJECT_DT,
    /**/ sizeof(EnDt),
    /**/ EnDt_Init,
    /**/ EnDt_Destroy,
    /**/ EnDt_Update,
    /**/ EnDt_Draw,
};

static u16 sTextIds[] = {
    0x2ABD, 0x2ABE, 0x2ABF, 0x2AC0, 0x2ABE, 0x2AC1, 0x2AC2, 0x2AC3, 0x2AC4, 0x2ABB, 0x2ABC, 0x2AC6, 0x2AC7, 0x2AC8,
    0x2AC9, 0x2ACA, 0x2ACB, 0x2ACC, 0x2ACD, 0x2ACE, 0x2ACF, 0x2AD0, 0x2AD1, 0x2AC5, 0x2AD3, 0x2AD4, 0x2AD2,
};

// The code uses EnDt::textIdIndex as an index into this lookup table
static s32 sCutsceneFocusTargetTable[] = {
    // Start of initial mayor meeting cutscene
    /*  0 */ EN_DT_CS_FOCUS_TARGET_BAISEN,
    /*  1 */ EN_DT_CS_FOCUS_TARGET_MAYOR,
    /*  2 */ EN_DT_CS_FOCUS_TARGET_MUTO,
    /*  3 */ EN_DT_CS_FOCUS_TARGET_MUTO,
    /*  4 */ EN_DT_CS_FOCUS_TARGET_MAYOR,
    /*  5 */ EN_DT_CS_FOCUS_TARGET_BAISEN,
    /*  6 */ EN_DT_CS_FOCUS_TARGET_MAYOR,
    /*  7 */ EN_DT_CS_FOCUS_TARGET_MUTO,
    /*  8 */ EN_DT_CS_FOCUS_TARGET_MAYOR,
    // End of intitial mayor meeting cutscene

    // Start of repeat mayor meeting cutscene
    /*  9 */ EN_DT_CS_FOCUS_TARGET_BAISEN,
    /* 10 */ EN_DT_CS_FOCUS_TARGET_MUTO,
    // End of repeat mayor meeting cutscene

    // Start of meeting resolution cutscene
    /* 11 */ EN_DT_CS_FOCUS_TARGET_BAISEN,
    /* 12 */ EN_DT_CS_FOCUS_TARGET_MAYOR,
    /* 13 */ EN_DT_CS_FOCUS_TARGET_MUTO,
    /* 14 */ EN_DT_CS_FOCUS_TARGET_MAYOR,
    /* 15 */ EN_DT_CS_FOCUS_TARGET_MUTO,
    /* 16 */ EN_DT_CS_FOCUS_TARGET_BAISEN,
    /* 17 */ EN_DT_CS_FOCUS_TARGET_MAYOR,
    /* 18 */ EN_DT_CS_FOCUS_TARGET_BAISEN,
    /* 19 */ EN_DT_CS_FOCUS_TARGET_MAYOR,
    /* 20 */ EN_DT_CS_FOCUS_TARGET_MUTO,
    /* 21 */ EN_DT_CS_FOCUS_TARGET_MAYOR,
    /* 22 */ EN_DT_CS_FOCUS_TARGET_MAYOR,
    // End of meeting resolution cutscene

    // Final night cutscene with mayor
    /* 23 */ EN_DT_CS_FOCUS_TARGET_MAYOR
};

static s16 sStringIdCsIndexTable[] = {
    0x2ABB, 0,  //
    0x2ABD, 0,  //
    0x2AC0, 1,  //
    0x2AC1, 2,  //
    0x2AC4, 3,  //
    0x2AC6, 4,  //
    0x2AC7, 5,  //
    0x2AC8, 6,  //
    0x2AC9, 7,  //
    0x2ACC, 8,  //
    0x2ACF, 9,  //
    0x2AD0, 10, //
};

static s16 sUnused[] = { 777, 777 };

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
        { 0xF7CFFFFF, 0x00, 0x00 },
        ATELEM_NONE | ATELEM_SFX_NORMAL,
        ACELEM_NONE,
        OCELEM_ON,
    },
    { 25, 70, 0, { 0, 0, 0 } },
};

static AnimationHeader* sAnimations[EN_DT_ANIMATION_MAX] = {
    &gDotourWaitAnim,     // EN_DT_ANIMATION_WAIT
    &gDotourDistressAnim, // EN_DT_ANIMATION_DISTRESS
    &gDotourShockAnim,    // EN_DT_ANIMATION_SHOCK
    &gDotourTapDeskAnim,  // EN_DT_ANIMATION_TAP_DESK
    &gDotourUprightAnim,  // EN_DT_ANIMATION_UPRIGHT
    &gDotourSitUpAnim,    // EN_DT_ANIMATION_SIT_UP
};

static u8 sAnimationModes[EN_DT_ANIMATION_MAX] = {
    ANIMMODE_LOOP, // EN_DT_ANIMATION_WAIT
    ANIMMODE_LOOP, // EN_DT_ANIMATION_DISTRESS
    ANIMMODE_ONCE, // EN_DT_ANIMATION_SHOCK
    ANIMMODE_LOOP, // EN_DT_ANIMATION_TAP_DESK
    ANIMMODE_LOOP, // EN_DT_ANIMATION_UPRIGHT
    ANIMMODE_ONCE, // EN_DT_ANIMATION_SIT_UP
};

// Based on the usage in its accompanying method, this must be an array where each row
// is 4 values wide. The first value is never used and always 0, while indexes 1, 2,
// and 3 are the animation index, the eye texture index, and a flag determining if
// the actor should have a dead-panned expression (not blinking).
static s32 sAppearancePropertiesTable[] = {
    0, EN_DT_ANIMATION_DISTRESS, EN_DT_EYE_TEXTURE_LOOK_DOWN, true,  // EN_DT_APPEARANCE_DEFAULT
    0, EN_DT_ANIMATION_SHOCK,    EN_DT_EYE_TEXTURE_LOOK_DOWN, true,  // EN_DT_APPEARANCE_PRE_MEETING
    0, EN_DT_ANIMATION_TAP_DESK, EN_DT_EYE_TEXTURE_LOOK_DOWN, true,  // EN_DT_APPEARANCE_POST_MEETING
    0, EN_DT_ANIMATION_TAP_DESK, EN_DT_EYE_TEXTURE_SHOCK,     false, // EN_DT_APPEARANCE_UNUSED
    0, EN_DT_ANIMATION_SIT_UP,   EN_DT_EYE_TEXTURE_SHOCK,     false, // EN_DT_APPEARANCE_RESOLUTION_TALK
    0, EN_DT_ANIMATION_UPRIGHT,  EN_DT_EYE_TEXTURE_SHOCK,     false  // EN_DT_APPEARANCE_RESOLVED_MEETING
};

static TexturePtr sEyeTextures[] = {
    gDotourEyeShockTex,    // EN_DT_EYE_TEXTURE_SHOCK
    gDotourEyeOpenTex,     // EN_DT_EYE_TEXTURE_OPEN
    gDotourEyeClosedTex,   // EN_DT_EYE_TEXTURE_CLOSED
    gDotourEyeLookDownTex, // EN_DT_EYE_TEXTURE_LOOK_DOWN
    gDotourEyeSquintTex,   // EN_DT_EYE_TEXTURE_SQUINT
};

static TexturePtr sEyebrowTextures[] = {
    gDotourEyebrowHighTex, // EN_DT_BROW_TEXTURE_HIGH
    gDotourEyebrowMidTex,  // EN_DT_BROW_TEXTURE_MID
    gDotourEyebrowLowTex,  // EN_DT_BROW_TEXTURE_LOW
};

void EnDt_Init(Actor* thisx, PlayState* play) {
    EnDt* this = (EnDt*)thisx;

    this->actor.colChkInfo.mass = MASS_IMMOVABLE;
    ActorShape_Init(&this->actor.shape, 0.0f, ActorShadow_DrawCircle, 19.0f);
    SkelAnime_InitFlex(play, &this->skelAnime, &object_dt_Skel_00B0CC, &gDotourWaitAnim, this->jointTable,
                       this->morphTable, OBJECT_DT_LIMB_MAX);

    this->actor.attentionRangeType = ATTENTION_RANGE_6;
    this->npcEnMuto = NULL;
    this->npcEnBaisen = NULL;
    Collider_InitAndSetCylinder(play, &this->collider, &this->actor, &sCylinderInit);

    if ((gSaveContext.save.day == 3) && gSaveContext.save.isNight) {
        EnDt_SetupFinalNightState(this, play);
    } else {
        s32 csId = this->actor.csId;
        s32 i = 0;

        // clang-format off
        while (csId != CS_ID_NONE) { this->csIds[i] = csId; csId = CutsceneManager_GetAdditionalCsId(csId); i++; }
        // clang-format on
    }
    this->actionFunc = EnDt_SetupCutsceneNpcs;
}

void EnDt_Destroy(Actor* thisx, PlayState* play) {
    EnDt* this = (EnDt*)thisx;

    Collider_DestroyCylinder(play, &this->collider);
}

void EnDt_UpdateHeadRotate(EnDt* this) {
    s32 yaw = ABS_ALT(BINANG_SUB(this->actor.yawTowardsPlayer, this->actor.world.rot.y));

    this->headRotTarget.y = 0;
    if (yaw < 0x4E20) {
        this->headRotTarget.y = BINANG_SUB(this->actor.yawTowardsPlayer, this->actor.world.rot.y);

        if (this->headRotTarget.y > 0x2710) {
            this->headRotTarget.y = 0x2710;
        } else if (this->headRotTarget.y < -0x2710) {
            this->headRotTarget.y = -0x2710;
        }
    }
}

void EnDt_ChangeAnim(EnDt* this, s32 animIndex) {
    f32 morphFrames;

    this->animIndex = animIndex;

    morphFrames = -4.0f;
    if ((this->animIndex == EN_DT_ANIMATION_SHOCK) || (this->animIndex == EN_DT_ANIMATION_SIT_UP)) {
        morphFrames = 0.0f;
    }

    this->animEndFrame = Animation_GetLastFrame(sAnimations[this->animIndex]);
    Animation_Change(&this->skelAnime, sAnimations[this->animIndex], 1.0f, 0.0f, this->animEndFrame,
                     sAnimationModes[this->animIndex], morphFrames);
}

void EnDt_UpdateAppearance(EnDt* this) {
    s32 index = this->appearancePhase * 4;

    index++;
    EnDt_ChangeAnim(this, sAppearancePropertiesTable[index]);

    index++;
    this->eyeTexIndex = sAppearancePropertiesTable[index];

    index++;
    this->disableBlinking = sAppearancePropertiesTable[index];
}

void EnDt_UpdateCutsceneFocusTarget(EnDt* this) {
    if ((this->npcEnMuto != NULL) && (this->npcEnBaisen != NULL)) {
        EnMuto* npcEnMuto = (EnMuto*)this->npcEnMuto;
        EnBaisen* npcEnBaisen = (EnBaisen*)this->npcEnBaisen;

        switch (sCutsceneFocusTargetTable[this->textIdIndex]) {
            case EN_DT_CS_FOCUS_TARGET_MUTO:
                npcEnMuto->targetActor = this->npcEnMuto;
                npcEnBaisen->targetActor = this->npcEnMuto;
                this->targetActor = this->npcEnMuto;
                break;

            case EN_DT_CS_FOCUS_TARGET_BAISEN:
                npcEnMuto->targetActor = this->npcEnBaisen;
                npcEnBaisen->targetActor = this->npcEnBaisen;
                this->targetActor = this->npcEnBaisen;
                break;

            case EN_DT_CS_FOCUS_TARGET_MAYOR:
                npcEnMuto->targetActor = &this->actor;
                npcEnBaisen->targetActor = &this->actor;
                this->targetActor = &this->actor;
                break;

            default:
                break;
        }
    }
}

void EnDt_SetupCutsceneNpcs(EnDt* this, PlayState* play) {
    Actor* npc = play->actorCtx.actorLists[ACTORCAT_NPC].first;

    while (npc != NULL) {
        if (npc->id == ACTOR_EN_MUTO) {
            this->npcEnMuto = npc;
        } else if (npc->id == ACTOR_EN_BAISEN) {
            this->npcEnBaisen = npc;
        }
        npc = npc->next;
    }

    EnDt_SetupRegularState(this, play);
}

void EnDt_SetupRegularState(EnDt* this, PlayState* play) {
    this->textIdIndex = 0;

    if (!CHECK_WEEKEVENTREG(WEEKEVENTREG_RECEIVED_MAYOR_HEART_PIECE)) {
        if (CHECK_WEEKEVENTREG(WEEKEVENTREG_RESOLVED_MAYOR_MEETING)) {
            this->textIdIndex = 21;
            this->appearancePhase = EN_DT_APPEARANCE_RESOLVED_MEETING;
            EnDt_UpdateAppearance(this);

            this->actor.textId = sTextIds[this->textIdIndex];
            Message_StartTextbox(play, this->actor.textId, &this->actor);
            Player_SetCsActionWithHaltedActors(play, &this->actor, PLAYER_CSACTION_WAIT);

            this->state = EN_DT_NPC_STATE_IDLE;
            this->actionFunc = EnDt_UpdateMeetingCutscene;
            return;
        }
    } else if (CHECK_WEEKEVENTREG(WEEKEVENTREG_RESOLVED_MAYOR_MEETING)) {
        this->textIdIndex = 23;
        this->appearancePhase = EN_DT_APPEARANCE_RESOLVED_MEETING;
        this->meetingFinished = true;
        EnDt_UpdateAppearance(this);
        Message_BombersNotebookQueueEvent(play, BOMBERS_NOTEBOOK_PERSON_MAYOR_DOTOUR);
    }

    if (this->appearancePhase == EN_DT_APPEARANCE_DEFAULT) {
        if (CHECK_WEEKEVENTREG(WEEKEVENTREG_ATTENDED_MAYOR_MEETING)) {
            this->textIdIndex = 9;
            this->appearancePhase = EN_DT_APPEARANCE_POST_MEETING;
        }

        EnDt_UpdateAppearance(this);
        if (!CHECK_WEEKEVENTREG(WEEKEVENTREG_ATTENDED_MAYOR_MEETING)) {
            this->appearancePhase = EN_DT_APPEARANCE_PRE_MEETING;
        }
    }

    this->actor.textId = sTextIds[this->textIdIndex];
    if (!CHECK_WEEKEVENTREG(WEEKEVENTREG_RESOLVED_MAYOR_MEETING)) {
        EnDt_UpdateCutsceneFocusTarget(this);
    }

    this->state = EN_DT_NPC_STATE_IDLE;
    this->actionFunc = EnDt_OfferRegularTalk;
}

void EnDt_OfferRegularTalk(EnDt* this, PlayState* play) {
    EnMuto* npcMuto = NULL;
    EnBaisen* npcBaisen = NULL;

    if (Actor_TalkOfferAccepted(&this->actor, &play->state)) {
        EnDt_SetupMeetingCutscene(this, play);
        return;
    }

    if (!CHECK_WEEKEVENTREG(WEEKEVENTREG_RESOLVED_MAYOR_MEETING)) {
        if ((this->npcEnMuto != NULL) && (this->npcEnBaisen)) {
            npcMuto = (EnMuto*)this->npcEnMuto;
            npcBaisen = (EnBaisen*)this->npcEnBaisen;
        }

        this->showedCouplesMask = false;
        this->textIdIndex = 0;

        if (Player_GetMask(play) == PLAYER_MASK_COUPLE) {
            this->textIdIndex = 11;
            this->showedCouplesMask = true;

            if (this->npcEnMuto && this->npcEnBaisen) {
                npcMuto->textIdIndex = 4;
                npcBaisen->textIdIndex = 6;
            }
        } else if (CHECK_WEEKEVENTREG(WEEKEVENTREG_ATTENDED_MAYOR_MEETING)) {
            this->textIdIndex = 9;
        }
    }

    this->actor.textId = sTextIds[this->textIdIndex];
    if (!CHECK_WEEKEVENTREG(WEEKEVENTREG_RESOLVED_MAYOR_MEETING) && (this->npcEnMuto != NULL) &&
        (this->npcEnBaisen != NULL) && (npcMuto->cutsceneState == 1 || npcBaisen->cutsceneState == 1)) {
        EnDt_SetupMeetingCutscene(this, play);
    }

    // After completing Couple's Mask event and wearing Kafeis Mask
    if (CHECK_WEEKEVENTREG(WEEKEVENTREG_RESOLVED_MAYOR_MEETING) && (Player_GetMask(play) == PLAYER_MASK_KAFEIS_MASK)) {
        this->actor.textId = 0x2368; // "My wife hired you? Go ask the Curiosity Shop"
    }

    Actor_OfferTalk(&this->actor, play, 150.0f);
}

void EnDt_SetupMeetingCutscene(EnDt* this, PlayState* play) {
    EnMuto* npcMuto;
    EnBaisen* npcBaisen;
    s32 index;

    if (!CHECK_WEEKEVENTREG(WEEKEVENTREG_RESOLVED_MAYOR_MEETING)) {
        if ((this->npcEnMuto != NULL) && (this->npcEnBaisen != NULL)) {
            npcMuto = (EnMuto*)this->npcEnMuto;
            npcBaisen = (EnBaisen*)this->npcEnBaisen;
        }

        if ((this->npcEnMuto != NULL) && (this->npcEnBaisen != NULL)) {
            npcMuto->cutsceneState = 1;
            npcBaisen->cutsceneState = 1;

            if (Player_GetMask(play) == PLAYER_MASK_COUPLE) {
                npcMuto->textIdIndex = 4;
                npcBaisen->textIdIndex = 6;
                this->appearancePhase = EN_DT_APPEARANCE_RESOLVED_MEETING;
                if (CHECK_WEEKEVENTREG(WEEKEVENTREG_ATTENDED_MAYOR_MEETING)) {
                    this->appearancePhase = EN_DT_APPEARANCE_RESOLUTION_TALK;
                }
                EnDt_UpdateAppearance(this);
            }
        }
    }

    this->cutsceneState = EN_DT_CS_STATE_NONE;
    for (index = 0; index < ARRAY_COUNT(sStringIdCsIndexTable); index += 2) {
        if ((play->msgCtx.currentTextId == sStringIdCsIndexTable[index]) ||
            (this->actor.textId == sStringIdCsIndexTable[index])) {
            this->cutsceneState = EN_DT_CS_STATE_WAITING;
            this->csIdIndex = index;
            break;
        }
    }

    this->state = EN_DT_NPC_STATE_VIEWING_MEETING;
    this->actionFunc = EnDt_UpdateMeetingCutscene;
}

void EnDt_UpdateMeetingCutscene(EnDt* this, PlayState* play) {
    EnMuto* muto = NULL;
    EnBaisen* baisen = NULL;
    s32 index = sStringIdCsIndexTable[this->csIdIndex + 1];
    s32 csIdIndex = sStringIdCsIndexTable[this->csIdIndex + 1];

    if (this->cutsceneState == EN_DT_CS_STATE_WAITING) {
        if (CutsceneManager_GetCurrentCsId() == CS_ID_GLOBAL_TALK) {
            CutsceneManager_Stop(CS_ID_GLOBAL_TALK);
            CutsceneManager_Queue(this->csIds[index]);
            return;
        }

        if (!CutsceneManager_IsNext(this->csIds[index])) {
            CutsceneManager_Queue(this->csIds[index]);
            return;
        }

        CutsceneManager_StartWithPlayerCs(this->csIds[index], this->targetActor);
        Actor_ChangeFocus(&this->actor, play, this->targetActor);
        this->cutsceneState = EN_DT_CS_STATE_PLAYING;
    }

    if (!CHECK_WEEKEVENTREG(WEEKEVENTREG_RESOLVED_MAYOR_MEETING) &&
        ((this->npcEnMuto != NULL) || (this->npcEnBaisen != NULL))) {
        muto = (EnMuto*)this->npcEnMuto;
        baisen = (EnBaisen*)this->npcEnBaisen;
    }

    if ((this->timer == 0) && (Message_GetState(&play->msgCtx) == TEXT_STATE_EVENT) && Message_ShouldAdvance(play)) {
        if (this->textIdIndex == 21) {
            Player_SetCsActionWithHaltedActors(play, &this->actor, PLAYER_CSACTION_END);
            if (this->cutsceneState == EN_DT_CS_STATE_PLAYING) {
                CutsceneManager_Stop(this->csIds[index]);
                this->cutsceneState = EN_DT_CS_STATE_NONE;
            }
            EnDt_OfferMeetingReward(this, play);
            return;
        }

        // After Muto and Baisen react to the Couple's Mask,
        // they leave the office and trigger a transition.
        if (this->textIdIndex == 20) { // Muto reacts to Mayor's final comments
            Message_CloseTextbox(play);
            play->nextEntrance = ENTRANCE(MAYORS_RESIDENCE, 1);
            Scene_SetExitFade(play);
            play->transitionTrigger = TRANS_TRIGGER_START;

            SET_WEEKEVENTREG(WEEKEVENTREG_RESOLVED_MAYOR_MEETING);
            this->meetingFinished = true;

            if (this->cutsceneState == EN_DT_CS_STATE_PLAYING) {
                CutsceneManager_Stop(this->csIds[index]);
                this->cutsceneState = EN_DT_CS_STATE_NONE;
            }
        } else {
            // At the end of each cutscene dialog, trigger an event flag
            if ((this->textIdIndex == 8) || (this->textIdIndex == 10) || (this->textIdIndex == 22) ||
                (this->textIdIndex == 23)) {
                SET_WEEKEVENTREG(WEEKEVENTREG_ATTENDED_MAYOR_MEETING);
                Message_CloseTextbox(play);

                // If the meeting is ongoing, reset all of the npc targets
                if (!CHECK_WEEKEVENTREG(WEEKEVENTREG_RESOLVED_MAYOR_MEETING)) {
                    if ((this->npcEnMuto != NULL) && (this->npcEnBaisen != NULL)) {
                        muto->cutsceneState = 2;
                        baisen->cutsceneState = 2;
                        muto->targetActor = this->npcEnMuto;
                        baisen->targetActor = this->npcEnBaisen;
                    }
                    this->targetActor = &this->actor;
                }

                if (this->cutsceneState == EN_DT_CS_STATE_PLAYING) {
                    CutsceneManager_Stop(this->csIds[index]);
                    this->cutsceneState = EN_DT_CS_STATE_NONE;
                }
                EnDt_SetupRegularState(this, play);
                return;
            }

            this->textIdIndex++;

            // End of initial meeting cutscene
            if (this->textIdIndex == 8) {
                play->msgCtx.msgLength = 0;
                EnDt_UpdateCutsceneFocusTarget(this);
                Actor_ChangeFocus(&this->actor, play, this->targetActor);
                Camera_SetTargetActor(Play_GetCamera(play, CutsceneManager_GetCurrentSubCamId(this->csIds[index])),
                                      this->targetActor);

                this->timer = 2;
                this->actionFunc = EnDt_FinishMeetingCutscene;
                return;
            }

            if (this->textIdIndex == 12) { // Mayor reacts to Couple's mask
                if (CHECK_WEEKEVENTREG(WEEKEVENTREG_ATTENDED_MAYOR_MEETING)) {
                    EnDt_UpdateAppearance(this);
                    this->timer = 25;
                } else {
                    EnDt_ChangeAnim(this, EN_DT_ANIMATION_UPRIGHT);
                    SkelAnime_Update(&this->skelAnime);
                }
            }

            if (this->textIdIndex == 13) { // Muto reacts to Couple's mask
                EnDt_ChangeAnim(this, EN_DT_ANIMATION_UPRIGHT);
                SkelAnime_Update(&this->skelAnime);
            }

            EnDt_UpdateCutsceneFocusTarget(this);
            Actor_ChangeFocus(&this->actor, play, this->targetActor);
            Camera_SetTargetActor(Play_GetCamera(play, CutsceneManager_GetCurrentSubCamId(this->csIds[index])),
                                  this->targetActor);

            this->actor.textId = sTextIds[this->textIdIndex];
            Message_CloseTextbox(play);
            Message_StartTextbox(play, this->actor.textId, &this->actor);

            // Sets the cutscene ID index based off the current textId
            for (index = 0; index < ARRAY_COUNT(sStringIdCsIndexTable); index += 2) {
                if (play->msgCtx.currentTextId == sStringIdCsIndexTable[index] ||
                    this->actor.textId == sStringIdCsIndexTable[index]) {
                    this->csIdIndex = index;
                    break;
                }
            }

            index = sStringIdCsIndexTable[this->csIdIndex + 1];
            if (this->cutsceneState == EN_DT_CS_STATE_PLAYING && (index != csIdIndex)) {
                this->cutsceneState = EN_DT_CS_STATE_WAITING;
                CutsceneManager_Stop(this->csIds[csIdIndex]);
                CutsceneManager_Queue(this->csIds[index]);
            }

            // Mutoh turns his head to focus on the Mayor after saying "Isn't that right Mayor?"
            if ((this->textIdIndex == 3) && (this->npcEnMuto != NULL) && (this->npcEnBaisen != NULL)) {
                muto->targetActor = &this->actor;
            }
        }
    }
}

void EnDt_FinishMeetingCutscene(EnDt* this, PlayState* play) {
    f32 curFrame = this->skelAnime.curFrame;

    if (this->timer != 0) {
        if (this->timer == 1) {
            if ((this->textIdIndex == 8) || (this->appearancePhase == EN_DT_APPEARANCE_RESOLUTION_TALK)) {
                EnDt_UpdateAppearance(this);
            }

            this->appearancePhase++;
            if (this->appearancePhase >= EN_DT_APPEARANCE_MAX) {
                this->appearancePhase = EN_DT_APPEARANCE_RESOLVED_MEETING;
            }
        }
    } else if (curFrame >= this->animEndFrame) {
        Camera* subCam;
        s32 index;
        s32 csIdIndex = sStringIdCsIndexTable[this->csIdIndex + 1];

        EnDt_UpdateAppearance(this);

        Message_CloseTextbox(play);
        this->actor.textId = sTextIds[this->textIdIndex];
        Message_StartTextbox(play, this->actor.textId, &this->actor);

        if (this->textIdIndex == 8) { // End of initial meeting cutscene
            Message_BombersNotebookQueueEvent(play, BOMBERS_NOTEBOOK_PERSON_MAYOR_DOTOUR);
        }

        // Sets the cutscene ID index based off the current textId
        //! @bug Bug Relatively harmless, but the index should actually be incremented by 2 like similar loops
        for (index = 0; index < ARRAY_COUNT(sStringIdCsIndexTable); index++) {
            if ((play->msgCtx.currentTextId == sStringIdCsIndexTable[index]) ||
                (this->actor.textId == sStringIdCsIndexTable[index])) {
                this->csIdIndex = index;
            }
        }

        index = sStringIdCsIndexTable[this->csIdIndex + 1];
        if ((this->cutsceneState == EN_DT_CS_STATE_PLAYING) && (index != csIdIndex)) {
            this->cutsceneState = EN_DT_CS_STATE_WAITING;
            CutsceneManager_Stop(this->csIds[csIdIndex]);
            CutsceneManager_Queue(this->csIds[index]);
        }

        EnDt_UpdateCutsceneFocusTarget(this);
        Actor_ChangeFocus(&this->actor, play, this->targetActor);

        subCam = Play_GetCamera(play, CutsceneManager_GetCurrentSubCamId(this->csIds[index]));
        Camera_SetTargetActor(subCam, this->targetActor);

        this->actionFunc = EnDt_UpdateMeetingCutscene;
    }
}

void EnDt_OfferMeetingReward(EnDt* this, PlayState* play) {
    Actor_OfferGetItem(&this->actor, play, GI_HEART_PIECE, 300.0f, 300.0f);
    this->state = EN_DT_NPC_STATE_OFFERED_MEETING_REWARD;
    this->actionFunc = EnDt_TriggerMeetingRewardEvent;
}

void EnDt_TriggerMeetingRewardEvent(EnDt* this, PlayState* play) {
    if (Actor_HasParent(&this->actor, play)) {
        this->textIdIndex = 22;
        this->actor.parent = NULL;
        this->actor.textId = sTextIds[this->textIdIndex];

        Actor_OfferTalkExchange(&this->actor, play, 400.0f, 400.0f, PLAYER_IA_MINUS1);

        SET_WEEKEVENTREG(WEEKEVENTREG_RECEIVED_MAYOR_HEART_PIECE);
        this->actionFunc = EnDt_TriggerMeetingNotebookEvent;
    } else {
        Actor_OfferGetItem(&this->actor, play, GI_HEART_PIECE, 300.0f, 300.0f);
    }
}

void EnDt_TriggerMeetingNotebookEvent(EnDt* this, PlayState* play) {
    SkelAnime_Update(&this->skelAnime);

    if (Actor_TalkOfferAccepted(&this->actor, &play->state)) {
        Message_BombersNotebookQueueEvent(play, BOMBERS_NOTEBOOK_EVENT_RECEIVED_MAYOR_HP);
        Message_BombersNotebookQueueEvent(play, BOMBERS_NOTEBOOK_PERSON_MAYOR_DOTOUR);
        this->actionFunc = EnDt_UpdateMeetingCutscene;
    } else {
        Actor_OfferTalkExchange(&this->actor, play, 400.0f, 400.0f, PLAYER_IA_MINUS1);
    }
}

void EnDt_SetupFinalNightState(EnDt* this, PlayState* play) {
    EnDt_ChangeAnim(this, EN_DT_ANIMATION_TAP_DESK);

    this->disableBlinking = true;
    this->textIdIndex = 24;
    Message_BombersNotebookQueueEvent(play, BOMBERS_NOTEBOOK_PERSON_MAYOR_DOTOUR);

    if (CHECK_WEEKEVENTREG(WEEKEVENTREG_TALKED_MAYOR_NIGHT_3)) {
        this->textIdIndex = 26;
    }

    this->actor.textId = sTextIds[this->textIdIndex];
    this->eyeTexIndex = EN_DT_EYE_TEXTURE_LOOK_DOWN;
    this->disableBlinking = true;
    this->state = EN_DT_NPC_STATE_WAIT_FINAL_NIGHT_TALK;
    this->actionFunc = EnDt_OfferFinalNightTalk;
}

void EnDt_OfferFinalNightTalk(EnDt* this, PlayState* play) {
    EnDt_UpdateHeadRotate(this);

    if (Actor_TalkOfferAccepted(&this->actor, &play->state)) {
        EnDt_StartFinalNightTalk(this);
        return;
    }

    // After completing Couple's Mask event and wearing Kafeis Mask
    if (CHECK_WEEKEVENTREG(WEEKEVENTREG_RESOLVED_MAYOR_MEETING) && (Player_GetMask(play) == PLAYER_MASK_KAFEIS_MASK)) {
        this->actor.textId = 0x2368; // "My wife hired you? Go ask the Curiosity Shop"
    }

    Actor_OfferTalk(&this->actor, play, 150.0f);
}

void EnDt_StartFinalNightTalk(EnDt* this) {
    this->state = EN_DT_NPC_STATE_DONE_FINAL_NIGHT_TALK;
    this->actionFunc = EnDt_TriggerFinalNightTalkEvent;
}

void EnDt_TriggerFinalNightTalkEvent(EnDt* this, PlayState* play) {
    EnDt_UpdateHeadRotate(this);

    if ((Message_GetState(&play->msgCtx) == TEXT_STATE_EVENT) && Message_ShouldAdvance(play)) {
        Message_CloseTextbox(play);
        if (!CHECK_WEEKEVENTREG(WEEKEVENTREG_TALKED_MAYOR_NIGHT_3)) {
            this->textIdIndex = 25;
            Message_ContinueTextbox(play, sTextIds[this->textIdIndex]);
            SET_WEEKEVENTREG(WEEKEVENTREG_TALKED_MAYOR_NIGHT_3);
        } else {
            EnDt_SetupFinalNightState(this, play);
        }
    }
}

void EnDt_Update(Actor* thisx, PlayState* play) {
    s32 pad;
    EnDt* this = (EnDt*)thisx;

    SkelAnime_Update(&this->skelAnime);
    Actor_SetScale(&this->actor, 0.01f);

    if ((this->state != 4) && (this->state != 5) && (gSaveContext.save.day == 3) && gSaveContext.save.isNight) {
        EnDt_SetupFinalNightState(this, play);
    }

    if (!CHECK_WEEKEVENTREG(WEEKEVENTREG_RESOLVED_MAYOR_MEETING) &&
        ((gSaveContext.save.day != 3) || ((gSaveContext.save.day == 3) && !gSaveContext.save.isNight))) {
        Audio_PlaySequenceAtPos(SEQ_PLAYER_BGM_SUB, &gSfxDefaultPos, NA_BGM_MAYORS_OFFICE, 1000.0f);
        Actor_PlaySfx(&this->actor, NA_SE_EV_CROWD - SFX_FLAG);
    }

    DECR(this->blinkTimer);
    DECR(this->timer);

    if (this->meetingFinished) {
        EnDt_UpdateHeadRotate(this);
    }

    // Blinking update
    if (!this->disableBlinking && (this->blinkTimer == 0)) {
        this->eyeTexIndex++;
        if (this->eyeTexIndex >= EN_DT_EYE_TEXTURE_LOOK_DOWN) {
            this->eyeTexIndex = EN_DT_EYE_TEXTURE_SHOCK;
            this->blinkTimer = (s32)Rand_ZeroFloat(60.0f) + 20;
        }
    }

    this->actionFunc(this, play);

    this->actor.shape.rot.y = this->actor.world.rot.y;
    Math_SmoothStepToS(&this->headRotValue.y, this->headRotTarget.y, 1, 3000, 0);

    Actor_SetFocus(&this->actor, 60.0f);
    Actor_MoveWithGravity(&this->actor);

    Collider_UpdateCylinder(&this->actor, &this->collider);
    CollisionCheck_SetOC(play, &play->colChkCtx, &this->collider.base);
}

s32 EnDt_OverrideLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, Actor* thisx) {
    EnDt* this = (EnDt*)thisx;

    if (limbIndex == OBJECT_DT_LIMB_0C) {
        rot->y += -1 * this->headRotValue.y;
    }

    return false;
}

void EnDt_Draw(Actor* thisx, PlayState* play) {
    EnDt* this = (EnDt*)thisx;
    s32 eyebrowIndex = 0;

    OPEN_DISPS(play->state.gfxCtx);

    Gfx_SetupDL25_Opa(play->state.gfxCtx);
    Gfx_SetupDL25_Xlu(play->state.gfxCtx);

    gSPSegment(POLY_OPA_DISP++, 0x08, Lib_SegmentedToVirtual(sEyeTextures[this->eyeTexIndex]));

    if (this->eyeTexIndex < EN_DT_BROW_TEXTURE_MAX) {
        eyebrowIndex = this->eyeTexIndex;
    }

    gSPSegment(POLY_OPA_DISP++, 0x09, Lib_SegmentedToVirtual(sEyebrowTextures[eyebrowIndex]));

    SkelAnime_DrawFlexOpa(play, this->skelAnime.skeleton, this->skelAnime.jointTable, this->skelAnime.dListCount,
                          EnDt_OverrideLimbDraw, NULL, &this->actor);

    CLOSE_DISPS(play->state.gfxCtx);
}
