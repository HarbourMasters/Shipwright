/*
 * File: z_en_sth.c
 * Overlay: ovl_En_Sth
 * Description: Guy looking at moon in South Clock Town,
 *              And the Cured Swamp Spider House man who gives you Mask of Truth
 *                (the cursed version is EnSsh)
 *              And the man who wants to buy Oceanside Spider House from you with rupees or Wallet
 */

#include "z_en_sth.h"
#include "2s2h/GameInteractor/GameInteractor.h"

#define FLAGS (ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_FRIENDLY)

void EnSth_Init(Actor* thisx, PlayState* play);
void EnSth_Destroy(Actor* thisx, PlayState* play);
void EnSth_UpdateWaitForObject(Actor* thisx, PlayState* play);

void EnSth_PanicIdle(EnSth* this, PlayState* play);
void EnSth_HandleOceansideSpiderHouseConversation(EnSth* this, PlayState* play);
void EnSth_OceansideSpiderHouseIdle(EnSth* this, PlayState* play);
void EnSth_MoonLookingIdle(EnSth* this, PlayState* play);
void EnSth_DefaultIdle(EnSth* this, PlayState* play);
void EnSth_HandleSwampSpiderHouseConversation(EnSth* this, PlayState* play);
void EnSth_SwampSpiderHouseIdle(EnSth* this, PlayState* play);
void EnSth_Update(Actor* thisx, PlayState* play);
void EnSth_Draw(Actor* thisx, PlayState* play);

ActorProfile En_Sth_Profile = {
    /**/ ACTOR_EN_STH,
    /**/ ACTORCAT_NPC,
    /**/ FLAGS,
    /**/ GAMEPLAY_KEEP,
    /**/ sizeof(EnSth),
    /**/ EnSth_Init,
    /**/ EnSth_Destroy,
    /**/ EnSth_UpdateWaitForObject,
    /**/ NULL,
};

#include "overlays/ovl_En_Sth/ovl_En_Sth.h"

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
    { 30, 40, 0, { 0, 0, 0 } },
};

typedef enum EnSthAnimation {
    /* -1 */ STH_ANIM_NONE = -1,
    /*  0 */ STH_ANIM_SIGNALLING,   // default, waving arms at you from telescope, OOT: cured happy animation
    /*  1 */ STH_ANIM_BENDING_DOWN, // default anim of cured spider house, but never seen before wait overrides it
    /*  2 */ STH_ANIM_TALK,
    /*  3 */ STH_ANIM_WAIT,
    /*  4 */ STH_ANIM_LOOK_UP,     // South Clock Town, looking at moon
    /*  5 */ STH_ANIM_LOOK_AROUND, // checking out Oceanside Spider House
    /*  6 */ STH_ANIM_PLEAD,       // wants to buy Oceanside Spider House
    /*  7 */ STH_ANIM_PANIC,       // after buying Oceanside Spider House, can be found at bottom of slide,
    /*  8 */ STH_ANIM_MAX          // set in init, not an actual index to the array
} EnSthAnimation;

static AnimationHeader* sAnimations[STH_ANIM_MAX] = {
    &gEnSthSignalAnim,         // STH_ANIM_SIGNALLING
    &gEnSthBendDownAnim,       // STH_ANIM_BENDING_DOWN
    &gEnSthTalkWithHandUpAnim, // STH_ANIM_TALK
    &gEnSthWaitAnim,           // STH_ANIM_WAIT
    &gEnSthLookUpAnim,         // STH_ANIM_LOOK_UP
    &gEnSthLookAroundAnim,     // STH_ANIM_LOOK_AROUND
    &gEnSthPleadAnim,          // STH_ANIM_PLEAD
    &gEnSthPanicAnim,          // STH_ANIM_PANIC
};

// three slightly different variants of "Only a little time left, oh goddess please save me"
static u16 sSthOceanspiderhousePanicText[] = { 0x1144, 0x1145, 0x1146 };

static u16 sSthFirstOceansideSpiderHouseGreet2TextIds[] = {
    0x1139, // I thought I heard noises... I'm glad I found it early
    0x113E, // I had no idea this place was here, perhaps it can keep me safe
    0x1143  // There's no time we have to hide!
};

static u16 sSthFirstOceansideSpiderHouseGreet1TextIds[] = {
    0x1132, // I thought I heard some noise.. is this some sort of underground shelter?
    0x113A, // I had no idea there was a basement here..
    0x113F  // I heard loud noises.. I never thought I'd find a place like this..
};

static Vec3f sFocusOffset = { 700.0f, 400.0f, 0.0f };

// Sth is hardcoded to only use element [1].
static Color_RGB8 sShirtColors[] = {
    { 190, 110, 0 },   // darker orange
    { 0, 180, 110 },   // green
    { 0, 255, 80 },    // lime green
    { 255, 160, 60 },  // brighter orange
    { 190, 230, 250 }, // white
    { 240, 230, 120 }, // pale yellow
};

void EnSth_Init(Actor* thisx, PlayState* play) {
    s32 pad;
    EnSth* this = (EnSth*)thisx;
    s32 objectSlot;

    // this actor can draw two separate bodies that use different objects
    if (STH_GET_SWAMP_BODY(&this->actor)) {
        objectSlot = Object_GetSlot(&play->objectCtx, OBJECT_AHG);
    } else {
        objectSlot = Object_GetSlot(&play->objectCtx, OBJECT_STH);
    }
    this->mainObjectSlot = objectSlot;
    this->maskOfTruthObjectSlot = Object_GetSlot(&play->objectCtx, OBJECT_MASK_TRUTH);

    Actor_SetScale(&this->actor, 0.01f);
    Collider_InitAndSetCylinder(play, &this->collider, &this->actor, &sCylinderInit);
    this->actor.colChkInfo.mass = MASS_IMMOVABLE;
    ActorShape_Init(&this->actor.shape, 0.0f, ActorShadow_DrawCircle, 36.0f);

    this->sthFlags = 0; // clear
    this->animIndex = STH_ANIM_MAX;
    this->actor.terminalVelocity = -9.0f;
    this->actor.gravity = -1.0f;

    switch (STH_GET_TYPE(&this->actor)) {
        case STH_TYPE_UNUSED_1:
            if (play->actorCtx.flags & ACTORCTX_FLAG_TELESCOPE_ON) {
                this->actor.flags |= (ACTOR_FLAG_UPDATE_CULLING_DISABLED | ACTOR_FLAG_DRAW_CULLING_DISABLED);
                this->actionFunc = EnSth_DefaultIdle;
            } else {
                Actor_Kill(&this->actor);
                return;
            }
            break;

        case STH_TYPE_SWAMP_SPIDER_HOUSE_CURED:
            if (GameInteractor_Should(VB_HAVE_ALL_SKULLTULA_TOKENS,
                                      Inventory_GetSkullTokenCount(play->sceneId) >= SPIDER_HOUSE_TOKENS_REQUIRED)) {
                this->actionFunc = EnSth_SwampSpiderHouseIdle;
            } else {
                Actor_Kill(&this->actor);
            }
            this->actor.textId = 0;
            if (!CHECK_WEEKEVENTREG(WEEKEVENTREG_RECEIVED_MASK_OF_TRUTH) ||
                !CHECK_WEEKEVENTREG(WEEKEVENTREG_TALKED_SWAMP_SPIDER_HOUSE_MAN)) {
                this->sthFlags |= STH_FLAG_DRAW_MASK_OF_TRUTH;
            }
            break;

        case STH_TYPE_MOON_LOOKING: // South Clock Town
            if (GameInteractor_Should(VB_HAVE_ALL_SKULLTULA_TOKENS, (gSaveContext.save.saveInfo.skullTokenCount &
                                                                     0xFFFF) >= SPIDER_HOUSE_TOKENS_REQUIRED)) {
                Actor_Kill(&this->actor);
                return;
            }

            this->actionFunc = EnSth_MoonLookingIdle;
            this->sthFlags |= STH_FLAG_DISABLE_HEAD_TRACK;
            this->actor.attentionRangeType = ATTENTION_RANGE_3;
            this->actor.cullingVolumeDistance = 800.0f;
            break;

        case STH_TYPE_OCEANSIDE_SPIDER_HOUSE_GREET:
            if (CHECK_WEEKEVENTREG(WEEKEVENTREG_OCEANSIDE_SPIDER_HOUSE_BUYER_MOVED_IN)) {
                // The player has already exited the house after complete; EnSth has moved deeper into the house.
                Actor_Kill(&this->actor);
                return;
            }
            this->actor.textId = 0;
            this->actionFunc = EnSth_OceansideSpiderHouseIdle;
            if (CHECK_WEEKEVENTREG(WEEKEVENTREG_OCEANSIDE_SPIDER_HOUSE_COLLECTED_REWARD)) {
                this->sthFlags |= STH_FLAG_OCEANSIDE_SPIDER_HOUSE_GREET;
            }
            break;

        case STH_TYPE_OCEANSIDE_SPIDER_HOUSE_PANIC:
            if (!CHECK_WEEKEVENTREG(WEEKEVENTREG_OCEANSIDE_SPIDER_HOUSE_BUYER_MOVED_IN) ||
                (GameInteractor_Should(VB_NOT_HAVE_ALL_SKULLTULA_TOKENS,
                                       Inventory_GetSkullTokenCount(play->sceneId) < SPIDER_HOUSE_TOKENS_REQUIRED,
                                       this))) {
                // Has not moved in, and has not completed the house; do NOT spawn yet.
                Actor_Kill(&this->actor);
                return;
            }
            this->actionFunc = EnSth_PanicIdle;
            this->actor.textId = 0;
            this->sthFlags |= STH_FLAG_DISABLE_HEAD_TRACK;
            break;

        default:
            this->actionFunc = EnSth_DefaultIdle;
            break;
    }
}

void EnSth_Destroy(Actor* thisx, PlayState* play) {
    EnSth* this = (EnSth*)thisx;

    Collider_DestroyCylinder(play, &this->collider);
}

s32 EnSth_CanSpeakToPlayer(EnSth* this, PlayState* play) {
    if ((this->actor.xzDistToPlayer < 100.0f) && Player_IsFacingActor(&this->actor, 0x3000, play) &&
        Actor_IsFacingPlayer(&this->actor, 0x2000)) {
        return true;
    } else {
        return false;
    }
}

void EnSth_ChangeAnim(EnSth* this, s16 animIndex) {
    if ((animIndex > STH_ANIM_NONE) && (animIndex < STH_ANIM_MAX) && (animIndex != this->animIndex)) {
        Animation_Change(&this->skelAnime, sAnimations[animIndex], 1.0f, 0.0f,
                         Animation_GetLastFrame(sAnimations[animIndex]), ANIMMODE_LOOP, -5.0f);
        this->animIndex = animIndex;
    }
}

void EnSth_GetInitialPanicText(EnSth* this, PlayState* play) {
    s32 day = CURRENT_DAY - 1;
    u16 nextTextId;

    if (day < 0) {
        day = 0;
    }
    nextTextId = sSthOceanspiderhousePanicText[day];

    Message_StartTextbox(play, nextTextId, &this->actor);
}

void EnSth_HandlePanicConversation(EnSth* this, PlayState* play) {
    SkelAnime_Update(&this->skelAnime);

    if ((Message_GetState(&play->msgCtx) == TEXT_STATE_EVENT) && Message_ShouldAdvance(play)) {
        this->actionFunc = EnSth_PanicIdle;
        Message_CloseTextbox(play);
    }
}

void EnSth_PanicIdle(EnSth* this, PlayState* play) {
    SkelAnime_Update(&this->skelAnime);

    if (Actor_TalkOfferAccepted(&this->actor, &play->state)) {
        EnSth_GetInitialPanicText(this, play);
        this->actionFunc = EnSth_HandlePanicConversation;
    } else if ((this->actor.xzDistToPlayer < 100.0f) && Player_IsFacingActor(&this->actor, 0x3000, play)) {
        Actor_OfferTalk(&this->actor, play, 110.0f);
    }
}

void EnSth_GetInitialOceansideSpiderHouseText(EnSth* this, PlayState* play) {
    u16 nextTextId;
    s32 day = CURRENT_DAY - 1;

    if (day < 0) {
        day = 0;
    }

    if (this->sthFlags & STH_FLAG_OCEANSIDE_SPIDER_HOUSE_GREET) {
        s32 pad;

        nextTextId = sSthFirstOceansideSpiderHouseGreet2TextIds[day];
        if (day == 2) {
            EnSth_ChangeAnim(this, STH_ANIM_LOOK_AROUND);
        }
    } else {
        nextTextId = sSthFirstOceansideSpiderHouseGreet1TextIds[day];
    }
    Message_StartTextbox(play, nextTextId, &this->actor);
}

void EnSth_PostOceanspiderhouseReward(EnSth* this, PlayState* play) {
    u16 nextTextId;

    SkelAnime_Update(&this->skelAnime);

    if (Actor_TalkOfferAccepted(&this->actor, &play->state)) {
        this->actor.flags &= ~ACTOR_FLAG_TALK_OFFER_AUTO_ACCEPTED;
        this->actionFunc = EnSth_HandleOceansideSpiderHouseConversation;

        switch (STH_GI_ID(&this->actor)) {
            case GI_WALLET_ADULT:
            case GI_WALLET_GIANT:
                nextTextId = 0x1137; // I'm just glad it was something you needed
                break;

            case GI_RUPEE_SILVER:
                // unused code, as he only gives wallet, purple and red, silver is never assigned to STH_GI_ID
                nextTextId = 0x1138; // That's my life's fortune
                break;

            case GI_RUPEE_PURPLE:
                if (CHECK_WEEKEVENTREG(WEEKEVENTREG_RECEIVED_OCEANSIDE_WALLET_UPGRADE)) {
                    nextTextId = 0x113D; // That's my life's savings
                } else {
                    nextTextId = 0x113C; // That's my life's savings (2)
                }
                break;

            default:
                if (CHECK_WEEKEVENTREG(WEEKEVENTREG_RECEIVED_OCEANSIDE_WALLET_UPGRADE)) {
                    nextTextId = 0x1142; // Well, thats all I have on me
                } else {
                    nextTextId = 0x1141; // Well, thats all I have on me
                }
                break;
        }
        Message_StartTextbox(play, nextTextId, &this->actor);
    } else {
        Actor_OfferTalkExchange(&this->actor, play, 1000.0f, 1000.0f, PLAYER_IA_MINUS1);
    }
}

void EnSth_GiveOceansideSpiderHouseReward(EnSth* this, PlayState* play) {
    SkelAnime_Update(&this->skelAnime);

    if (Actor_HasParent(&this->actor, play)) {
        this->actor.parent = NULL;
        this->actionFunc = EnSth_PostOceanspiderhouseReward;
        this->actor.flags |= ACTOR_FLAG_TALK_OFFER_AUTO_ACCEPTED;
        Actor_OfferTalkExchange(&this->actor, play, 1000.0f, 1000.0f, PLAYER_IA_MINUS1);
        if (CURRENT_DAY == 3) {
            EnSth_ChangeAnim(this, STH_ANIM_PLEAD);
        } else {
            EnSth_ChangeAnim(this, STH_ANIM_WAIT);
        }
    } else {
        Actor_OfferGetItem(&this->actor, play, STH_GI_ID(&this->actor), 10000.0f, 500.0f);
    }
}

void EnSth_HandleOceansideSpiderHouseConversation(EnSth* this, PlayState* play) {
    s32 day = CURRENT_DAY - 1;

    if (day < 0) {
        day = 0;
    }

    if (this->animIndex == STH_ANIM_PLEAD) {
        Math_SmoothStepToS(&this->actor.shape.rot.y, this->actor.yawTowardsPlayer, 2, 0x1000, 0x200);
        this->actor.world.rot.y = this->actor.shape.rot.y;
    }

    SkelAnime_Update(&this->skelAnime);

    switch (Message_GetState(&play->msgCtx)) {
        case TEXT_STATE_EVENT:
            if (Message_ShouldAdvance(play)) {
                switch (play->msgCtx.currentTextId) {
                    case 0x1134: // (does not exist)
                        Message_ContinueTextbox(play, play->msgCtx.currentTextId + 1);
                        break;

                    case 0x1132: // Heard noise, came in to see
                    case 0x113A: // Had no idea there was a basement here
                    case 0x113F: // Heard noise... I never thought I'd find a place like this
                        Message_ContinueTextbox(play, 0x1133); // did you find this place?
                        break;

                    case 0x1133:                               // did you find this place?
                        Message_ContinueTextbox(play, 0x1136); // I want to buy this place from you
                        EnSth_ChangeAnim(this, STH_ANIM_PLEAD);
                        break;

                    case 0x1136: // I want to buy this house from you
                        // This flag prevents multiple rewards, switching to secondary dialogue after
                        SET_WEEKEVENTREG(WEEKEVENTREG_OCEANSIDE_SPIDER_HOUSE_COLLECTED_REWARD);

                        switch (day) {
                            case 0: // first day
                                if (CHECK_WEEKEVENTREG(WEEKEVENTREG_RECEIVED_OCEANSIDE_WALLET_UPGRADE)) {
                                    STH_GI_ID(&this->actor) = GI_RUPEE_SILVER;
                                } else {
                                    // This flag prevents getting two wallets from the same place.
                                    //   Instead, getting silver rupee above.
                                    SET_WEEKEVENTREG(WEEKEVENTREG_RECEIVED_OCEANSIDE_WALLET_UPGRADE);
                                    switch (CUR_UPG_VALUE(UPG_WALLET)) {
                                        case 0:
                                            STH_GI_ID(&this->actor) = GI_WALLET_ADULT;
                                            break;

                                        case 1:
                                            STH_GI_ID(&this->actor) = GI_WALLET_GIANT;
                                            break;
                                    }
                                }
                                break;

                            case 1: // second day
                                STH_GI_ID(&this->actor) = GI_RUPEE_PURPLE;
                                break;

                            default: // final day
                                STH_GI_ID(&this->actor) = GI_RUPEE_RED;
                                break;
                        }
                        Message_CloseTextbox(play);
                        this->actionFunc = EnSth_GiveOceansideSpiderHouseReward;
                        EnSth_GiveOceansideSpiderHouseReward(this, play);
                        break;

                    case 0x113C:                               // (Second day) I am giving you my life savings
                        Message_ContinueTextbox(play, 0x113B); // If only I had gotten here yesterday...
                        break;

                    case 0x1141:                               // (Final day) This is all I have
                        Message_ContinueTextbox(play, 0x1140); // If only I had gotten here two days ago...
                        EnSth_ChangeAnim(this, STH_ANIM_WAIT);
                        break;

                    default:
                        this->actionFunc = EnSth_OceansideSpiderHouseIdle;
                        Message_CloseTextbox(play);
                        this->sthFlags |= STH_FLAG_OCEANSIDE_SPIDER_HOUSE_GREET;
                        break;
                }
            }
            break;

        case TEXT_STATE_CLOSING:
            this->actionFunc = EnSth_OceansideSpiderHouseIdle;
            this->sthFlags |= STH_FLAG_OCEANSIDE_SPIDER_HOUSE_GREET;
            break;
    }
}

void EnSth_OceansideSpiderHouseIdle(EnSth* this, PlayState* play) {
    SkelAnime_Update(&this->skelAnime);

    if (Actor_TalkOfferAccepted(&this->actor, &play->state)) {
        EnSth_GetInitialOceansideSpiderHouseText(this, play);
        this->actionFunc = EnSth_HandleOceansideSpiderHouseConversation;
    } else if (EnSth_CanSpeakToPlayer(this, play)) {
        Actor_OfferTalk(&this->actor, play, 110.0f);
    }
}

void EnSth_HandleMoonLookingConversation(EnSth* this, PlayState* play) {
    SkelAnime_Update(&this->skelAnime);

    if ((Message_GetState(&play->msgCtx) == TEXT_STATE_EVENT) && Message_ShouldAdvance(play)) {
        this->actionFunc = EnSth_MoonLookingIdle;
        Message_CloseTextbox(play);
    }
    this->headRot.x = -0x1388;
}

void EnSth_MoonLookingIdle(EnSth* this, PlayState* play) {
    SkelAnime_Update(&this->skelAnime);

    if (Actor_TalkOfferAccepted(&this->actor, &play->state)) {
        this->actionFunc = EnSth_HandleMoonLookingConversation;
    } else if (EnSth_CanSpeakToPlayer(this, play) || this->actor.isLockedOn) {
        if ((CURRENT_TIME >= CLOCK_TIME(6, 0)) && (CURRENT_TIME <= CLOCK_TIME(18, 0))) {
            this->actor.textId = 0x1130; // Huh? The Moon...
        } else {
            this->actor.textId = 0x1131; // (The Moon) gotten bigger again
        }
        Actor_OfferTalk(&this->actor, play, 110.0f);
    }
    this->headRot.x = -0x1388;
}

// used by type: STH_TYPE_UNUSED_1 and undefined types
void EnSth_DefaultIdle(EnSth* this, PlayState* play) {
    SkelAnime_Update(&this->skelAnime);
}

void EnSth_GetInitialSwampSpiderHouseText(EnSth* this, PlayState* play) {
    u16 nextTextId;

    if (CHECK_WEEKEVENTREG(WEEKEVENTREG_34_10)) {
        nextTextId = 0x903; // (does not exist)
        EnSth_ChangeAnim(this, STH_ANIM_TALK);
    } else if (CHECK_WEEKEVENTREG(WEEKEVENTREG_34_20)) {
        nextTextId = 0x90F; // (does not exist)
        EnSth_ChangeAnim(this, STH_ANIM_TALK);
    } else if (CHECK_WEEKEVENTREG(WEEKEVENTREG_RECEIVED_MASK_OF_TRUTH)) {
        if (!CHECK_WEEKEVENTREG(WEEKEVENTREG_TALKED_SWAMP_SPIDER_HOUSE_MAN)) {
            nextTextId = 0x91B; // As soon as I calm down, getting rid of it
        } else {
            nextTextId = 0x918; // I've had enough of this, going home
        }
        EnSth_ChangeAnim(this, STH_ANIM_TALK);
    } else if (GameInteractor_Should(VB_HAVE_ALL_SKULLTULA_TOKENS,
                                     Inventory_GetSkullTokenCount(play->sceneId) >= SPIDER_HOUSE_TOKENS_REQUIRED)) {
        if (INV_CONTENT(ITEM_MASK_TRUTH) == ITEM_MASK_TRUTH) {
            this->sthFlags |= STH_FLAG_SWAMP_SPIDER_HOUSE_SAVED;
            nextTextId = 0x919; // I've been saved!
        } else {
            nextTextId = 0x916; // I've been saved! (Duplicate)
        }
    } else if (CHECK_WEEKEVENTREG(WEEKEVENTREG_34_02)) {
        nextTextId = 0x8FF; // (does not exist)
    } else {
        nextTextId = 0x8FC; // (does not exist)
        SET_WEEKEVENTREG(WEEKEVENTREG_34_02);
    }

    Message_StartTextbox(play, nextTextId, &this->actor);
}

void EnSth_TalkAfterSwampSpiderHouseGiveMask(EnSth* this, PlayState* play) {
    SkelAnime_Update(&this->skelAnime);

    if (Actor_TalkOfferAccepted(&this->actor, &play->state)) {
        this->actionFunc = EnSth_HandleSwampSpiderHouseConversation;
        SET_WEEKEVENTREG(WEEKEVENTREG_RECEIVED_MASK_OF_TRUTH);
        Message_StartTextbox(play, 0x918, &this->actor); // I've had enough of this, going home
    } else {
        Actor_OfferTalkExchange(&this->actor, play, 1000.0f, 1000.0f, PLAYER_IA_MINUS1);
    }
}

void EnSth_SwampSpiderHouseGiveMask(EnSth* this, PlayState* play) {
    SkelAnime_Update(&this->skelAnime);

    if (Actor_HasParent(&this->actor, play)) {
        this->actor.parent = NULL;
        this->actionFunc = EnSth_TalkAfterSwampSpiderHouseGiveMask;
        this->actor.flags |= ACTOR_FLAG_TALK_OFFER_AUTO_ACCEPTED;
        Actor_OfferTalkExchange(&this->actor, play, 1000.0f, 1000.0f, PLAYER_IA_MINUS1);
    } else {
        this->sthFlags &= ~STH_FLAG_DRAW_MASK_OF_TRUTH;
        // This flag is used to keep track if the player has already spoken to the actor, triggering secondary dialogue.
        SET_WEEKEVENTREG(WEEKEVENTREG_TALKED_SWAMP_SPIDER_HOUSE_MAN);
        Actor_OfferGetItem(&this->actor, play, GI_MASK_TRUTH, 10000.0f, 50.0f);
    }
}

void EnSth_HandleSwampSpiderHouseConversation(EnSth* this, PlayState* play) {
    s32 pad;

    SkelAnime_Update(&this->skelAnime);

    if ((Message_GetState(&play->msgCtx) == TEXT_STATE_EVENT) && Message_ShouldAdvance(play)) {
        switch (play->msgCtx.currentTextId) {
            case 0x90C: // (does not exist)
                EnSth_ChangeAnim(this, STH_ANIM_TALK);
                Message_ContinueTextbox(play, play->msgCtx.currentTextId + 1);
                break;

            case 0x916: // I have been saved! I thought I was doomed
            case 0x919: // I have been saved! I thought I was doomed (duplicate)
                EnSth_ChangeAnim(this, STH_ANIM_WAIT);
                Message_ContinueTextbox(play, play->msgCtx.currentTextId + 1);
                break;

            case 0x8FC: // (does not exist)
            case 0x8FD: // (does not exist)
            case 0x900: // (does not exist)
            case 0x90A: // (does not exist)
            case 0x90D: // (does not exist)
                Message_ContinueTextbox(play, play->msgCtx.currentTextId + 1);
                break;

            case 0x901: // (does not exist)
            case 0x90B: // (does not exist)
            case 0x917: // Someone gave me this mask and said it would make me rich, Take it
                Message_CloseTextbox(play);
                this->actionFunc = EnSth_SwampSpiderHouseGiveMask;
                EnSth_SwampSpiderHouseGiveMask(this, play);
                break;

            case 0x91A: // Someone gave me this mask and said it would make me rich, getting rid of it
                SET_WEEKEVENTREG(WEEKEVENTREG_RECEIVED_MASK_OF_TRUTH);
                CLEAR_WEEKEVENTREG(WEEKEVENTREG_TALKED_SWAMP_SPIDER_HOUSE_MAN);

            case 0x902: // (does not exist)
            case 0x903: // (does not exist)
            case 0x90E: // (does not exist)
            case 0x90F: // (does not exist)
            case 0x918: // I have had enough, going home
            case 0x91B: // As soon as I calm down, getting rid of it
                EnSth_ChangeAnim(this, STH_ANIM_WAIT);

            default:
                this->actor.flags &= ~ACTOR_FLAG_TALK_OFFER_AUTO_ACCEPTED;
                Message_CloseTextbox(play);
                this->actionFunc = EnSth_SwampSpiderHouseIdle;
                break;
        }
    }
}

void EnSth_SwampSpiderHouseIdle(EnSth* this, PlayState* play) {
    SkelAnime_Update(&this->skelAnime);

    if (Actor_TalkOfferAccepted(&this->actor, &play->state)) {
        EnSth_GetInitialSwampSpiderHouseText(this, play);
        this->actionFunc = EnSth_HandleSwampSpiderHouseConversation;
    } else if (EnSth_CanSpeakToPlayer(this, play)) {
        this->actor.textId = 0;
        Actor_OfferTalk(&this->actor, play, 110.0f);
    }
}

/**
 * Oceanside Spider House EnSth shows up after you collect all 30 tokens.
 * Here we wait invisible until the player has finished.
 */
void EnSth_UpdateOceansideSpiderHouseWaitForTokens(Actor* thisx, PlayState* play) {
    EnSth* this = (EnSth*)thisx;

    if (GameInteractor_Should(VB_HAVE_ALL_SKULLTULA_TOKENS,
                              Inventory_GetSkullTokenCount(play->sceneId) >= SPIDER_HOUSE_TOKENS_REQUIRED)) {
        this->actor.update = EnSth_Update;
        this->actor.draw = EnSth_Draw;
        this->actor.flags |= ACTOR_FLAG_ATTENTION_ENABLED;
    }
}

/**
 * Dual object actors have to wait for the object to finish loading.
 * This dev chose to use a temporary update instead of temporary actionFunc.
 */
void EnSth_UpdateWaitForObject(Actor* thisx, PlayState* play) {
    s32 pad;
    EnSth* this = (EnSth*)thisx;

    if (Object_IsLoaded(&play->objectCtx, this->mainObjectSlot)) {
        this->actor.objectSlot = this->mainObjectSlot;
        Actor_SetObjectDependency(play, &this->actor);

        if (STH_GET_SWAMP_BODY(&this->actor)) {
            SkelAnime_InitFlex(play, &this->skelAnime, &gAhgSkel, &gEnSthBendDownAnim, this->jointTable,
                               this->morphTable, AHG_LIMB_MAX);
            Animation_PlayLoop(&this->skelAnime, &gEnSthBendDownAnim);
            this->animIndex = STH_ANIM_BENDING_DOWN;
            if (CHECK_WEEKEVENTREG(WEEKEVENTREG_34_10) || CHECK_WEEKEVENTREG(WEEKEVENTREG_34_20) ||
                CHECK_WEEKEVENTREG(WEEKEVENTREG_RECEIVED_MASK_OF_TRUTH) ||
                (GameInteractor_Should(VB_HAVE_ALL_SKULLTULA_TOKENS,
                                       Inventory_GetSkullTokenCount(play->sceneId) >= SPIDER_HOUSE_TOKENS_REQUIRED))) {
                EnSth_ChangeAnim(this, STH_ANIM_WAIT);
            }
        } else {
            SkelAnime_InitFlex(play, &this->skelAnime, &gSthSkel, &gEnSthSignalAnim, this->jointTable, this->morphTable,
                               STH_LIMB_MAX);
            Animation_PlayLoop(&this->skelAnime, &gEnSthSignalAnim);
        }

        this->actor.update = EnSth_Update;
        this->actor.draw = EnSth_Draw;

        switch (STH_GET_TYPE(&this->actor)) {
            case STH_TYPE_MOON_LOOKING:
                EnSth_ChangeAnim(this, STH_ANIM_LOOK_UP);
                break;

            case STH_TYPE_OCEANSIDE_SPIDER_HOUSE_GREET:
                if (CHECK_WEEKEVENTREG(WEEKEVENTREG_OCEANSIDE_SPIDER_HOUSE_COLLECTED_REWARD)) {
                    EnSth_ChangeAnim(this, STH_ANIM_LOOK_AROUND);
                } else {
                    EnSth_ChangeAnim(this, STH_ANIM_LOOK_AROUND);
                }
                break;

            case STH_TYPE_OCEANSIDE_SPIDER_HOUSE_PANIC:
                EnSth_ChangeAnim(this, STH_ANIM_PANIC);
                break;
        }

        // not ready to appear yet
        if ((STH_GET_TYPE(&this->actor) == STH_TYPE_OCEANSIDE_SPIDER_HOUSE_GREET) &&
            (GameInteractor_Should(VB_NOT_HAVE_ALL_SKULLTULA_TOKENS,
                                   Inventory_GetSkullTokenCount(play->sceneId) < SPIDER_HOUSE_TOKENS_REQUIRED, this))) {
            this->actor.update = EnSth_UpdateOceansideSpiderHouseWaitForTokens;
            this->actor.draw = NULL;
            this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
        }
    }
}

void EnSth_Update(Actor* thisx, PlayState* play) {
    s32 pad;
    EnSth* this = (EnSth*)thisx;

    Actor_MoveWithGravity(&this->actor);
    Collider_UpdateCylinder(&this->actor, &this->collider);
    CollisionCheck_SetOC(play, &play->colChkCtx, &this->collider.base);
    Actor_UpdateBgCheckInfo(play, &this->actor, 0.0f, 0.0f, 0.0f, UPDBGCHECKINFO_FLAG_4);

    this->actionFunc(this, play);

    if (EnSth_CanSpeakToPlayer(this, play) && !(this->sthFlags & STH_FLAG_DISABLE_HEAD_TRACK) &&
        (this->animIndex != STH_ANIM_LOOK_AROUND)) {
        Vec3s torsoRot;

        torsoRot.x = torsoRot.y = torsoRot.z = 0; // this should block torso rot from working

        Actor_TrackPlayer(play, &this->actor, &this->headRot, &torsoRot, this->actor.focus.pos);
    } else {
        Math_SmoothStepToS(&this->headRot.x, 0, 6, 0x1838, 0x64);
        Math_SmoothStepToS(&this->headRot.y, 0, 6, 0x1838, 0x64);
    }
}

s32 EnSth_OverrideLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, Actor* thisx) {
    s32 pad;
    EnSth* this = (EnSth*)thisx;

    if (limbIndex == STH_LIMB_HEAD) {
        rot->x += this->headRot.y;
        rot->z += this->headRot.x;

        // object_sth body has no head by default, forced here from overlay data
        *dList = gEnSthHeadDL;
    }

    if ((limbIndex == STH_LIMB_CHEST) || (limbIndex == STH_LIMB_LEFT_FOREARM) ||
        (limbIndex == STH_LIMB_RIGHT_FOREARM)) {
        rot->y += TRUNCF_BINANG(Math_SinS(play->state.frames * ((limbIndex * 50) + 0x814)) * 200.0f);
        rot->z += TRUNCF_BINANG(Math_CosS(play->state.frames * ((limbIndex * 50) + 0x940)) * 200.0f);
    }

    return false;
}

void EnSth_PostLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot, Actor* thisx) {
    EnSth* this = (EnSth*)thisx;

    if (limbIndex == STH_LIMB_HEAD) {
        s32 pad;

        Matrix_MultVec3f(&sFocusOffset, &this->actor.focus.pos);

        if (!STH_GET_SWAMP_BODY(&this->actor)) {
            OPEN_DISPS(play->state.gfxCtx);

            gSPDisplayList(POLY_OPA_DISP++, gEnSthExtraHairDL);

            CLOSE_DISPS(play->state.gfxCtx);
        } else {
            OPEN_DISPS(play->state.gfxCtx);

            if (this->sthFlags & STH_FLAG_DRAW_MASK_OF_TRUTH) {
                if (Object_IsLoaded(&play->objectCtx, this->maskOfTruthObjectSlot)) {
                    Matrix_Push();
                    Matrix_RotateZS(0x3A98, MTXMODE_APPLY);
                    Matrix_Translate(0.0f, 190.0f, 0.0f, MTXMODE_APPLY);

                    MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx);
                    gSPSegment(POLY_OPA_DISP++, 0x0A, play->objectCtx.slots[this->maskOfTruthObjectSlot].segment);
                    gSPDisplayList(POLY_OPA_DISP++, object_mask_truth_DL_0001A0);

                    Matrix_Pop();
                }
            }

            CLOSE_DISPS(play->state.gfxCtx);
        }
    }
}

void EnSth_Draw(Actor* thisx, PlayState* play) {
    s32 pad;
    EnSth* this = (EnSth*)thisx;

    OPEN_DISPS(play->state.gfxCtx);

    Gfx_SetupDL25_Opa(play->state.gfxCtx);

    gSPSegment(POLY_OPA_DISP++, 0x08,
               Gfx_EnvColor(play->state.gfxCtx, sShirtColors[1].r, sShirtColors[1].g, sShirtColors[1].b, 255));
    gSPSegment(POLY_OPA_DISP++, 0x09, Gfx_EnvColor(play->state.gfxCtx, 90, 110, 130, 255));

    SkelAnime_DrawFlexOpa(play, this->skelAnime.skeleton, this->skelAnime.jointTable, this->skelAnime.dListCount,
                          EnSth_OverrideLimbDraw, EnSth_PostLimbDraw, &this->actor);

    CLOSE_DISPS(play->state.gfxCtx);
}
