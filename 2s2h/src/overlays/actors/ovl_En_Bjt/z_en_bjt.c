/*
 * File: z_en_bjt.c
 * Overlay: ovl_En_Bjt
 * Description: ??? (Hand in toilet)
 */

#include "z_en_bjt.h"
#include "2s2h/GameInteractor/GameInteractor.h"

#define FLAGS (ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_FRIENDLY)

void EnBjt_Init(Actor* thisx, PlayState* play);
void EnBjt_Destroy(Actor* thisx, PlayState* play);
void EnBjt_Update(Actor* thisx, PlayState* play);
void EnBjt_Draw(Actor* thisx, PlayState* play);

void EnBjt_Talk(EnBjt* this, PlayState* play);
void EnBjt_FollowSchedule(EnBjt* this, PlayState* play);

#define TOILET_HAND_STATE_TALKING (1 << 3)   // Actually talking to Player
#define TOILET_HAND_STATE_TEXTBOX (1 << 4)   // Whenever a textbox is on screen
#define TOILET_HAND_STATE_APPEARING (1 << 7) // Appearing
#define TOILET_HAND_STATE_VANISHING (1 << 8) // Vanishing
#define TOILET_HAND_STATE_VISIBLE (1 << 9)   // Out

typedef enum {
    /* 0 */ TOILET_HAND_SCH_NONE,
    /* 1 */ TOILET_HAND_SCH_AVAILABLE
} ToiletHandScheduleResult;

static u8 sScheduleScript[] = {
    /* 0x00 */ SCHEDULE_CMD_CHECK_NOT_IN_SCENE_S(SCENE_YADOYA, 0x11 - 0x04),
    /* 0x04 */ SCHEDULE_CMD_CHECK_TIME_RANGE_S(0, 0, 6, 0, 0x0B - 0x0A),
    /* 0x0A */ SCHEDULE_CMD_RET_NONE(),
    /* 0x0B */ SCHEDULE_CMD_RET_TIME(0, 0, 6, 0, TOILET_HAND_SCH_AVAILABLE),
    /* 0x11 */ SCHEDULE_CMD_RET_NONE(),
};

static MsgScript sMsgScript[] = {
    /* 0x0000 0x03 */ MSCRIPT_CMD_BEGIN_TEXT(0x2948),
    /* 0x0003 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x0004 0x03 */ MSCRIPT_CMD_BEGIN_TEXT(0x00FF),
    /* 0x0007 0x07 */ MSCRIPT_CMD_CHECK_CALLBACK_MULTI(0x0, 0x0060 - 0x000E, 0x006D - 0x000E),
    /* 0x000E 0x03 */ MSCRIPT_CMD_PLAYER_TALK(0x294A),
    /* 0x0011 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x0012 0x03 */ MSCRIPT_CMD_JUMP_3(0x0),
    /* 0x0015 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x0016 0x01 */ MSCRIPT_CMD_PAUSE(),
    /* 0x0017 0x03 */ MSCRIPT_CMD_CHECK_CALLBACK(0x0),
    /* 0x001A 0x03 */ MSCRIPT_CMD_BEGIN_TEXT(0x294B),
    /* 0x001D 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x001E 0x01 */ MSCRIPT_CMD_PAUSE(),
    /* 0x001F 0x03 */ MSCRIPT_CMD_CHECK_CALLBACK(0x0),
    /* 0x0022 0x03 */ MSCRIPT_CMD_BEGIN_TEXT(0x294C),
    /* 0x0025 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x0026 0x01 */ MSCRIPT_CMD_CLOSE_TEXT(),
    /* 0x0027 0x05 */ MSCRIPT_CMD_CHECK_WEEK_EVENT_REG(WEEKEVENTREG_73_08, 0x0049 - 0x002C),
    /* 0x002C 0x03 */ MSCRIPT_CMD_CHECK_CALLBACK(0x0),
    /* 0x002F 0x05 */ MSCRIPT_CMD_OFFER_ITEM(GI_HEART_PIECE, 0x0),
    /* 0x0034 0x03 */ MSCRIPT_CMD_SET_COLLECTIBLE(0x000C),
    /* 0x0037 0x03 */ MSCRIPT_CMD_JUMP_3(0x0),
    /* 0x003A 0x01 */ MSCRIPT_CMD_AWAIT_TEXT_DONE(),
    /* 0x003B 0x03 */ MSCRIPT_CMD_NOTEBOOK_EVENT(BOMBERS_NOTEBOOK_EVENT_RECEIVED_TOILET_HAND_HP),
    /* 0x003E 0x03 */ MSCRIPT_CMD_NOTEBOOK_EVENT(BOMBERS_NOTEBOOK_EVENT_MET_TOLIET_HAND),
    /* 0x0041 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x0042 0x03 */ MSCRIPT_CMD_SET_WEEK_EVENT_REG(WEEKEVENTREG_73_08),
    /* 0x0045 0x03 */ MSCRIPT_CMD_SET_WEEK_EVENT_REG(WEEKEVENTREG_90_80),
    /* 0x0048 0x01 */ MSCRIPT_CMD_DONE(),

    /* 0x0049 0x03 */ MSCRIPT_CMD_CHECK_CALLBACK(0x0),
    /* 0x004C 0x05 */ MSCRIPT_CMD_OFFER_ITEM(GI_RUPEE_BLUE, 0x0),
    /* 0x0051 0x03 */ MSCRIPT_CMD_SET_COLLECTIBLE(0x0002),
    /* 0x0054 0x03 */ MSCRIPT_CMD_JUMP_3(0x0),
    /* 0x0057 0x01 */ MSCRIPT_CMD_AWAIT_TEXT_DONE(),
    /* 0x0058 0x03 */ MSCRIPT_CMD_NOTEBOOK_EVENT(BOMBERS_NOTEBOOK_EVENT_MET_TOLIET_HAND),
    /* 0x005B 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x005C 0x03 */ MSCRIPT_CMD_SET_WEEK_EVENT_REG(WEEKEVENTREG_90_80),
    /* 0x005F 0x01 */ MSCRIPT_CMD_DONE(),

    /* 0x0060 0x03 */ MSCRIPT_CMD_PLAYER_TALK(0x2949),
    /* 0x0063 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x0064 0x03 */ MSCRIPT_CMD_JUMP_3(0x0),
    /* 0x0067 0x01 */ MSCRIPT_CMD_AWAIT_TEXT(),
    /* 0x0068 0x03 */ MSCRIPT_CMD_NOTEBOOK_EVENT(BOMBERS_NOTEBOOK_EVENT_MET_TOLIET_HAND),
    /* 0x006B 0x01 */ MSCRIPT_CMD_CLOSE_TEXT(),
    /* 0x006C 0x01 */ MSCRIPT_CMD_DONE(),

    /* 0x006D 0x03 */ MSCRIPT_CMD_NOTEBOOK_EVENT(BOMBERS_NOTEBOOK_EVENT_MET_TOLIET_HAND),
    /* 0x0070 0x01 */ MSCRIPT_CMD_CLOSE_TEXT(),
    /* 0x0071 0x01 */ MSCRIPT_CMD_DONE(),
};

ActorProfile En_Bjt_Profile = {
    /**/ ACTOR_EN_BJT,
    /**/ ACTORCAT_NPC,
    /**/ FLAGS,
    /**/ OBJECT_BJT,
    /**/ sizeof(EnBjt),
    /**/ EnBjt_Init,
    /**/ EnBjt_Destroy,
    /**/ EnBjt_Update,
    /**/ EnBjt_Draw,
};

static ColliderCylinderInit sCylinderInit = {
    {
        COL_MATERIAL_HIT1,
        AT_NONE,
        AC_NONE,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_1,
        COLSHAPE_CYLINDER,
    },
    {
        ELEM_MATERIAL_UNK1,
        { 0x00000000, 0x00, 0x00 },
        { 0x00000000, 0x00, 0x00 },
        ATELEM_NONE | ATELEM_SFX_NORMAL,
        ACELEM_NONE,
        OCELEM_ON,
    },
    { 10, 68, 0, { 0, 0, 0 } },
};

static CollisionCheckInfoInit2 sColChkInfoInit = { 0, 0, 0, 0, MASS_IMMOVABLE };

typedef enum {
    /* -1 */ TOILET_HAND_ANIM_NONE = -1,
    /*  0 */ TOILET_HAND_ANIM_WAITING,
    /*  1 */ TOILET_HAND_ANIM_WAITING_MORPH,  // Same as TOILET_HAND_ANIM_WAITING, but with morph frames
    /*  2 */ TOILET_HAND_ANIM_WAGGING_FINGER, // Wrong
    /*  3 */ TOILET_HAND_ANIM_THUMBS_UP,      // Right
    /*  4 */ TOILET_HAND_ANIM_OPEN_HAND,
    /*  5 */ TOILET_HAND_ANIM_FIST, // i.e. holding the reward
    /*  6 */ TOILET_HAND_ANIM_MAX
} ToiletHandAnimation;

static AnimationInfoS sAnimationInfo[TOILET_HAND_ANIM_MAX] = {
    { &gToiletHandWaitingAnim, 1.0f, 0, -1, ANIMMODE_LOOP, 0 },        // TOILET_HAND_ANIM_WAITING
    { &gToiletHandWaitingAnim, 1.0f, 0, -1, ANIMMODE_LOOP, -4 },       // TOILET_HAND_ANIM_WAITING_MORPH
    { &gToiletHandWaggingFingerAnim, 1.0f, 0, -1, ANIMMODE_LOOP, -4 }, // TOILET_HAND_ANIM_WAGGING_FINGER
    { &gToiletHandThumbsUpAnim, 1.0f, 0, -1, ANIMMODE_LOOP, -4 },      // TOILET_HAND_ANIM_THUMBS_UP
    { &gToiletHandOpenHandAnim, 1.0f, 0, -1, ANIMMODE_LOOP, -4 },      // TOILET_HAND_ANIM_OPEN_HAND
    { &gToiletHandFistAnim, 1.0f, 0, -1, ANIMMODE_LOOP, -4 },          // TOILET_HAND_ANIM_FIST
};

void EnBjt_UpdateSkelAnime(EnBjt* this) {
    this->skelAnime.playSpeed = this->animPlaySpeed;
    SkelAnime_Update(&this->skelAnime);
}

s32 EnBjt_ChangeAnim(EnBjt* this, s32 animIndex) {
    s32 changeAnim = false;
    s32 didAnimChange = false;

    if ((animIndex == TOILET_HAND_ANIM_WAITING) || (animIndex == TOILET_HAND_ANIM_WAITING_MORPH)) {
        if ((this->animIndex != TOILET_HAND_ANIM_WAITING) && (this->animIndex != TOILET_HAND_ANIM_WAITING_MORPH)) {
            changeAnim = true;
        }
    } else if (this->animIndex != animIndex) {
        changeAnim = true;
    }

    if (changeAnim) {
        this->animIndex = animIndex;
        didAnimChange = SubS_ChangeAnimationByInfoS(&this->skelAnime, sAnimationInfo, animIndex);
        this->animPlaySpeed = this->skelAnime.playSpeed;
    }

    return didAnimChange;
}

void EnBjt_UpdateCollision(EnBjt* this, PlayState* play) {
    static Vec3f sColliderBasePos = { 0.0f, 8.0f, 10.0f };
    s32 pad;
    Vec3f pos;
    f32 height;

    if (this->stateFlags & (TOILET_HAND_STATE_APPEARING | TOILET_HAND_STATE_VISIBLE)) {
        Lib_Vec3f_TranslateAndRotateY(&this->actor.world.pos, this->actor.shape.rot.y, &sColliderBasePos, &pos);
        Math_Vec3f_ToVec3s(&this->collider.dim.pos, &pos);
        height = this->actor.focus.pos.y - this->actor.world.pos.y;
        this->collider.dim.height = height;
        this->collider.dim.radius = 32;
        CollisionCheck_SetOC(play, &play->colChkCtx, &this->collider.base);
    }
}

s32 EnBjt_TakeItem(s32 exchangeItem) {
    switch (exchangeItem) {
        case PLAYER_IA_LETTER_TO_KAFEI:
            Inventory_DeleteItem(ITEM_LETTER_TO_KAFEI, SLOT(ITEM_LETTER_TO_KAFEI));
            break;

        case PLAYER_IA_DEED_SWAMP:
            Inventory_DeleteItem(ITEM_DEED_SWAMP, SLOT(ITEM_DEED_SWAMP));
            break;

        case PLAYER_IA_DEED_MOUNTAIN:
            Inventory_DeleteItem(ITEM_DEED_MOUNTAIN, SLOT(ITEM_DEED_MOUNTAIN));
            break;

        case PLAYER_IA_DEED_OCEAN:
            Inventory_DeleteItem(ITEM_DEED_OCEAN, SLOT(ITEM_DEED_OCEAN));
            break;

        case PLAYER_IA_DEED_LAND:
            Inventory_DeleteItem(ITEM_DEED_LAND, SLOT(ITEM_DEED_LAND));
            break;

        case PLAYER_IA_LETTER_MAMA:
            Inventory_DeleteItem(ITEM_LETTER_MAMA, SLOT(ITEM_LETTER_MAMA));
            break;

        default:
            break;
    }
    return 0;
}

#define FULLY_GROWN_SCALE 0.017f

/**
 * @return boolean true if scale is set to final value
 */
s32 EnBjt_Appear(EnBjt* this) {
    s32 finished = false;

    if (!this->playedSfx) {
        Actor_PlaySfx(&this->actor, NA_SE_EV_TOILET_HAND_APPEAR);
        this->playedSfx = true;
    }

    Math_ApproachF(&this->actor.scale.x, FULLY_GROWN_SCALE, 0.21f, 0.3f);
    if ((FULLY_GROWN_SCALE - this->actor.scale.x) < FULLY_GROWN_SCALE / 100.0f) {
        this->actor.scale.x = FULLY_GROWN_SCALE;
        this->stateFlags |= TOILET_HAND_STATE_VISIBLE;
        this->stateFlags &= ~TOILET_HAND_STATE_APPEARING;
        finished = true;
    }
    this->heightOffset = (this->actor.scale.x / FULLY_GROWN_SCALE) * 4.0f;
    this->actor.world.pos.y = this->actor.home.pos.y + this->heightOffset;
    Actor_SetScale(&this->actor, this->actor.scale.x);

    return finished;
}

/**
 * @return boolean true if scale is set to final value
 */
s32 EnBjt_Vanish(EnBjt* this) {
    s32 finished = false;

    if (!this->playedSfx) {
        Actor_PlaySfx(&this->actor, NA_SE_EV_TOILET_HAND_VANISH);
        this->playedSfx = true;
    }

    Math_ApproachF(&this->actor.scale.x, 0.0f, 0.21f, 0.3f);
    if (this->actor.scale.x < FULLY_GROWN_SCALE / 100.0f) {
        this->actor.scale.x = 0.0f;
        this->stateFlags &= ~TOILET_HAND_STATE_VANISHING;
        finished = true;
    }
    this->heightOffset = (this->actor.scale.x / FULLY_GROWN_SCALE) * 4.0f;
    this->actor.world.pos.y = this->actor.home.pos.y + this->heightOffset;
    Actor_SetScale(&this->actor, this->actor.scale.x);

    return finished;
}

typedef enum {
    /* 0 */ TOILET_HAND_BEHAVIOUR_WAIT_FOR_ITEM, // and check if item is suitable
    /* 1 */ TOILET_HAND_BEHAVIOUR_TAKE_ITEM,     // take paper and disappear to use it
    /* 2 */ TOILET_HAND_BEHAVIOUR_USE_ITEM,      // using paper, flushing
    /* 3 */ TOILET_HAND_BEHAVIOUR_REAPPEAR,      // reappear with reward
    /* 4 */ TOILET_HAND_BEHAVIOUR_REWARD_GIVEN
} ToiletHandBehaviour;

// msgevent callback/communication. Follow and choose parts of script to run
s32 EnBjt_ChooseBehaviour(Actor* thisx, PlayState* play) {
    Player* player = GET_PLAYER(play);
    EnBjt* this = (EnBjt*)thisx;
    PlayerItemAction itemAction;
    s32 scriptBranch = 0;

    switch (this->behaviour) {
        case TOILET_HAND_BEHAVIOUR_WAIT_FOR_ITEM:
            switch (Message_GetState(&play->msgCtx)) {
                case TEXT_STATE_CHOICE:
                case TEXT_STATE_EVENT:
                    if (!Message_ShouldAdvance(play)) {
                        break;
                    }
                    // Fallthrough
                case TEXT_STATE_PAUSE_MENU:
                    itemAction = func_80123810(play);

                    if ((itemAction == PLAYER_IA_DEED_LAND) || (itemAction == PLAYER_IA_LETTER_TO_KAFEI) ||
                        (itemAction == PLAYER_IA_DEED_SWAMP) || (itemAction == PLAYER_IA_DEED_MOUNTAIN) ||
                        (itemAction == PLAYER_IA_DEED_OCEAN) || (itemAction == PLAYER_IA_LETTER_MAMA)) {
                        EnBjt_ChangeAnim(this, TOILET_HAND_ANIM_WAITING_MORPH);
                        this->playedSfx = false;
                        this->behaviour++;
                        scriptBranch = 1; // Right item
                    } else if (itemAction <= PLAYER_IA_MINUS1) {
                        this->playedSfx = false;
                        this->behaviour++;
                        scriptBranch = 3; // Not showing item
                    } else if (itemAction != PLAYER_IA_NONE) {
                        this->playedSfx = false;
                        this->behaviour++;
                        scriptBranch = 2; // Wrong item
                    }
                    break;

                default:
                    break;
            }
            break;

        case TOILET_HAND_BEHAVIOUR_TAKE_ITEM:
            if (player->exchangeItemAction != PLAYER_IA_NONE) {
                if (GameInteractor_Should(VB_TOILET_HAND_TAKE_ITEM, true, this)) {
                    EnBjt_TakeItem(player->exchangeItemAction);
                }
                player->exchangeItemAction = PLAYER_IA_NONE;
            }
            if (EnBjt_Vanish(this)) {
                this->timer = 60;
                this->behaviour++;
                scriptBranch = 1; // Right item
            }
            break;

        case TOILET_HAND_BEHAVIOUR_USE_ITEM:
            if (DECR(this->timer) == 0) {
                EnBjt_ChangeAnim(this, TOILET_HAND_ANIM_FIST); // change while not visible
                this->playedSfx = false;
                this->behaviour++;
            } else if (this->timer == 10) {
                Actor_PlaySfx(&this->actor, NA_SE_EV_TOILET_WATER);
            }
            break;

        case TOILET_HAND_BEHAVIOUR_REAPPEAR:
            if (EnBjt_Appear(this)) {
                this->behaviour++;
                scriptBranch = 1; // Right item
            }
            break;

        case TOILET_HAND_BEHAVIOUR_REWARD_GIVEN:
            EnBjt_ChangeAnim(this, TOILET_HAND_ANIM_OPEN_HAND);
            this->behaviour++;
            scriptBranch = 1; // Right item
            break;

        default:
            break;
    }

    return scriptBranch;
}

s32 EnBjt_CheckTalk(EnBjt* this, PlayState* play) {
    s32 ret = false;

    if (((this->stateFlags & SUBS_OFFER_MODE_MASK) != SUBS_OFFER_MODE_NONE) &&
        Actor_TalkOfferAccepted(&this->actor, &play->state)) {
        this->stateFlags |= TOILET_HAND_STATE_TALKING;
        SubS_SetOfferMode(&this->stateFlags, SUBS_OFFER_MODE_NONE, SUBS_OFFER_MODE_MASK);
        this->msgScriptCallback = EnBjt_ChooseBehaviour;
        this->behaviour = 0;
        this->actionFunc = EnBjt_Talk;
        ret = true;
    }
    return ret;
}

s32 EnBjt_ChooseAnimation(EnBjt* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    u16 curTextId = play->msgCtx.currentTextId;

    if (player->stateFlags1 & (PLAYER_STATE1_TALKING | PLAYER_STATE1_400)) { // Talking, show item?
        this->stateFlags |= TOILET_HAND_STATE_TEXTBOX;
        if (this->textId != curTextId) {
            switch (curTextId) {
                case 0x2949: // Wrong item
                    EnBjt_ChangeAnim(this, TOILET_HAND_ANIM_WAGGING_FINGER);
                    break;

                case 0x294A: // Right item
                    EnBjt_ChangeAnim(this, TOILET_HAND_ANIM_THUMBS_UP);
                    break;

                default:
                    break;
            }
        }
        this->textId = curTextId;
    } else if (this->stateFlags & TOILET_HAND_STATE_TEXTBOX) {
        this->stateFlags &= ~TOILET_HAND_STATE_TEXTBOX;
        EnBjt_ChangeAnim(this, TOILET_HAND_ANIM_WAITING);
    }
    return 0;
}

// Action function, based on msgEvent
void EnBjt_Talk(EnBjt* this, PlayState* play) {
    s16 yaw = this->actor.yawTowardsPlayer;

    if (MsgEvent_RunScript(&this->actor, play, sMsgScript, this->msgScriptCallback, &this->msgScriptPos)) {
        this->actor.flags &= ~ACTOR_FLAG_TALK;
        SubS_SetOfferMode(&this->stateFlags, SUBS_OFFER_MODE_ONSCREEN, SUBS_OFFER_MODE_MASK);
        this->stateFlags &= ~TOILET_HAND_STATE_TALKING;
        this->msgScriptPos = 0;
        this->actionFunc = EnBjt_FollowSchedule;
    } else {
        Math_ApproachS(&this->actor.shape.rot.y, yaw, 4, 0x2AA8);
    }
}

// Change state based on schedule and stateFlags
void EnBjt_FollowSchedule(EnBjt* this, PlayState* play) {
    ScheduleOutput scheduleOutput;

    if (!Schedule_RunScript(play, sScheduleScript, &scheduleOutput)) {
        scheduleOutput.result = TOILET_HAND_SCH_NONE;
    }
    if (scheduleOutput.result == TOILET_HAND_SCH_AVAILABLE) {
        if (this->stateFlags & TOILET_HAND_STATE_APPEARING) {
            if (EnBjt_Appear(this)) {
                SubS_SetOfferMode(&this->stateFlags, SUBS_OFFER_MODE_ONSCREEN, SUBS_OFFER_MODE_MASK);
            }
        } else if (this->stateFlags & TOILET_HAND_STATE_VANISHING) {
            EnBjt_Vanish(this);
        } else if (this->stateFlags & TOILET_HAND_STATE_VISIBLE) {
            // Vanish if player goes too far away or heart piece given
            if ((fabsf(this->actor.playerHeightRel) > 70.0f) || (this->actor.xzDistToPlayer > 140.0f) ||
                CHECK_WEEKEVENTREG(WEEKEVENTREG_90_80)) {
                SubS_SetOfferMode(&this->stateFlags, SUBS_OFFER_MODE_NONE, SUBS_OFFER_MODE_MASK);
                this->playedSfx = false;
                this->stateFlags &= ~TOILET_HAND_STATE_VISIBLE;
                this->stateFlags |= TOILET_HAND_STATE_VANISHING;
            }
        } else {
            // Appear if player approaches and heart piece not given
            if ((fabsf(this->actor.playerHeightRel) < 20.0f) && (this->actor.xzDistToPlayer < 70.0f) &&
                !CHECK_WEEKEVENTREG(WEEKEVENTREG_90_80)) {
                this->stateFlags |= TOILET_HAND_STATE_APPEARING;
                this->playedSfx = false;
            }
        }
        this->scheduleResult = scheduleOutput.result;
    } else {
        this->actor.flags |= ACTOR_FLAG_LOCK_ON_DISABLED;
        Actor_SetScale(&this->actor, 0.0f);
        this->stateFlags = 0;
        this->msgScriptCallback = NULL;
        this->scheduleResult = TOILET_HAND_SCH_NONE;
    }
}

void EnBjt_Init(Actor* thisx, PlayState* play) {
    EnBjt* this = (EnBjt*)thisx;

    ActorShape_Init(&this->actor.shape, 0.0f, NULL, 0.0f);
    SkelAnime_InitFlex(play, &this->skelAnime, &gToiletHandSkel, NULL, this->jointTable, this->morphTable,
                       TOILET_HAND_LIMB_MAX);

    this->animIndex = TOILET_HAND_ANIM_NONE;
    EnBjt_ChangeAnim(this, TOILET_HAND_ANIM_WAITING);

    Collider_InitAndSetCylinder(play, &this->collider, &this->actor, &sCylinderInit);
    CollisionCheck_SetInfo2(&this->actor.colChkInfo, DamageTable_Get(0x16), &sColChkInfoInit);
    this->actor.flags |= ACTOR_FLAG_LOCK_ON_DISABLED;
    Actor_SetScale(&this->actor, 0.0f);

    this->scheduleResult = TOILET_HAND_SCH_NONE;
    this->stateFlags = 0;
    this->actionFunc = EnBjt_FollowSchedule;
}

void EnBjt_Destroy(Actor* thisx, PlayState* play) {
}

void EnBjt_Update(Actor* thisx, PlayState* play) {
    EnBjt* this = (EnBjt*)thisx;

    EnBjt_CheckTalk(this, play);
    this->actionFunc(this, play);
    EnBjt_ChooseAnimation(this, play);

    if (this->scheduleResult != TOILET_HAND_SCH_NONE) {
        EnBjt_UpdateSkelAnime(this);
        SubS_Offer(&this->actor, play, 60.0f, 10.0f, PLAYER_IA_NONE, this->stateFlags & SUBS_OFFER_MODE_MASK);
        Actor_SetFocus(&this->actor, 26.0f);
        EnBjt_UpdateCollision(this, play);
    }
}

void EnBjt_Draw(Actor* thisx, PlayState* play) {
    EnBjt* this = (EnBjt*)thisx;

    if (this->scheduleResult != TOILET_HAND_SCH_NONE) {
        Gfx_SetupDL25_Opa(play->state.gfxCtx);
        SkelAnime_DrawFlexOpa(play, this->skelAnime.skeleton, this->skelAnime.jointTable, this->skelAnime.dListCount,
                              NULL, NULL, &this->actor);
    }
}
