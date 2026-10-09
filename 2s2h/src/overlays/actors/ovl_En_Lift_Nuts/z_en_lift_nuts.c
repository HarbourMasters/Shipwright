/*
 * File: z_en_lift_nuts.c
 * Overlay: ovl_En_Lift_Nuts
 * Description: Deku Scrub Playground - Employee
 */

#include "z_en_lift_nuts.h"
#include "overlays/actors/ovl_En_Gamelupy/z_en_gamelupy.h"

#define FLAGS                                                                                  \
    (ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_FRIENDLY | ACTOR_FLAG_UPDATE_CULLING_DISABLED | \
     ACTOR_FLAG_UPDATE_DURING_OCARINA)

void EnLiftNuts_Init(Actor* thisx, PlayState* play);
void EnLiftNuts_Destroy(Actor* thisx, PlayState* play);
void EnLiftNuts_Update(Actor* thisx, PlayState* play);
void EnLiftNuts_Draw(Actor* thisx, PlayState* play);
void EnLiftNuts_Reset(void);

void EnLiftNuts_HandleConversationEvent(EnLiftNuts* this, PlayState* play);

void EnLiftNuts_SetupIdleHidden(EnLiftNuts* this);
void EnLiftNuts_IdleHidden(EnLiftNuts* this, PlayState* play);
void EnLiftNuts_Burrow(EnLiftNuts* this, PlayState* play);
void EnLiftNuts_SetupIdle(EnLiftNuts* this);
void EnLiftNuts_RiseUp(EnLiftNuts* this, PlayState* play);
void EnLiftNuts_Idle(EnLiftNuts* this, PlayState* play);
void EnLiftNuts_SetupStartConversation(EnLiftNuts* this);
void EnLiftNuts_StartConversation(EnLiftNuts* this, PlayState* play);
void EnLiftNuts_HandleConversation(EnLiftNuts* this, PlayState* play);
void EnLiftNuts_SetupMove(EnLiftNuts* this);
void EnLiftNuts_Move(EnLiftNuts* this, PlayState* play);
void EnLiftNuts_SetupMovePlayerToActor(EnLiftNuts* this);
void EnLiftNuts_MovePlayerToActor(EnLiftNuts* this, PlayState* play);
void EnLiftNuts_SetupStartGame(EnLiftNuts* this);
void EnLiftNuts_StartGame(EnLiftNuts* this, PlayState* play);
void EnLiftNuts_SetupStartGameImmediately(EnLiftNuts* this);
void EnLiftNuts_StartGameImmediately(EnLiftNuts* this, PlayState* play);
void EnLiftNuts_SetupRunGame(EnLiftNuts* this);
void EnLiftNuts_RunGame(EnLiftNuts* this, PlayState* play);
void EnLiftNuts_SetupEndGame(EnLiftNuts* this, PlayState* play);
void EnLiftNuts_EndGame(EnLiftNuts* this, PlayState* play);
void EnLiftNuts_SetupGiveReward(EnLiftNuts* this);
void EnLiftNuts_GiveReward(EnLiftNuts* this, PlayState* play);
void EnLiftNuts_SetupResumeConversation(EnLiftNuts* this);
void EnLiftNuts_ResumeConversation(EnLiftNuts* this, PlayState* play);
void EnLiftNuts_SetupStartHiding(EnLiftNuts* this);
void EnLiftNuts_StartHiding(EnLiftNuts* this, PlayState* play);
void EnLiftNuts_Hide(EnLiftNuts* this, PlayState* play);
void EnLiftNuts_UpdateEyes(EnLiftNuts* this);
void EnLiftNuts_SpawnDust(EnLiftNuts* this, PlayState* play);

ActorProfile En_Lift_Nuts_Profile = {
    /**/ ACTOR_EN_LIFT_NUTS,
    /**/ ACTORCAT_NPC,
    /**/ FLAGS,
    /**/ OBJECT_DNT,
    /**/ sizeof(EnLiftNuts),
    /**/ EnLiftNuts_Init,
    /**/ EnLiftNuts_Destroy,
    /**/ EnLiftNuts_Update,
    /**/ EnLiftNuts_Draw,
    /**/ EnLiftNuts_Reset,
};

typedef enum {
    /*  0 */ ENLIFTNUTS_ANIM_STANDING,
    /*  1 */ ENLIFTNUTS_ANIM_WALK,
    /*  2 */ ENLIFTNUTS_ANIM_RISE_UP,
    /*  3 */ ENLIFTNUTS_ANIM_BURROW_LONG,
    /*  4 */ ENLIFTNUTS_ANIM_EXCITED_START,
    /*  5 */ ENLIFTNUTS_ANIM_EXCITED_LOOP,
    /*  6 */ ENLIFTNUTS_ANIM_EXCITED_END,
    /*  7 */ ENLIFTNUTS_ANIM_TAKE_OFF_HAT,
    /*  8 */ ENLIFTNUTS_ANIM_FLY_START,
    /*  9 */ ENLIFTNUTS_ANIM_FLY_LOOP,
    /* 10 */ ENLIFTNUTS_ANIM_SHOCKED_START,
    /* 11 */ ENLIFTNUTS_ANIM_SHOCKED_SHAKE_HEAD,
    /* 12 */ ENLIFTNUTS_ANIM_SHOCKED_POUND,
    /* 13 */ ENLIFTNUTS_ANIM_SHOCKED_END,
    /* 14 */ ENLIFTNUTS_ANIM_THINK,
    /* 15 */ ENLIFTNUTS_ANIM_BOB,
    /* 16 */ ENLIFTNUTS_ANIM_JUMP,
    /* 17 */ ENLIFTNUTS_ANIM_BURROW_HALF,
    /* 18 */ ENLIFTNUTS_ANIM_BURROW_SHORT,
    /* 19 */ ENLIFTNUTS_ANIM_MAX
} EnLiftNutsAnimation;

static AnimationInfo sAnimationInfo[ENLIFTNUTS_ANIM_MAX] = {
    { &gBusinessScrubStandingAnim, 1.0f, 0.0f, 0.0f, ANIMMODE_LOOP, -4.0f },     // ENLIFTNUTS_ANIM_STANDING
    { &gBusinessScrubWalkAnim, 1.0f, 0.0f, 0.0f, ANIMMODE_LOOP, -4.0f },         // ENLIFTNUTS_ANIM_WALK
    { &gBusinessScrubRiseUpAnim, 1.0f, 0.0f, 0.0f, ANIMMODE_ONCE, -4.0f },       // ENLIFTNUTS_ANIM_RISE_UP
    { &gBusinessScrubBurrowAnim, 1.0f, 0.0f, 0.0f, ANIMMODE_ONCE, -4.0f },       // ENLIFTNUTS_ANIM_BURROW_LONG
    { &gBusinessScrubExcitedStartAnim, 1.0f, 0.0f, 0.0f, ANIMMODE_ONCE, -1.0f }, // ENLIFTNUTS_ANIM_EXCITED_START
    { &gBusinessScrubExcitedLoopAnim, 1.0f, 0.0f, 0.0f, ANIMMODE_LOOP, -4.0f },  // ENLIFTNUTS_ANIM_EXCITED_LOOP
    { &gBusinessScrubExcitedEndAnim, 1.0f, 0.0f, 0.0f, ANIMMODE_ONCE, -1.0f },   // ENLIFTNUTS_ANIM_EXCITED_END
    { &gBusinessScrubTakeOffHatAnim, 1.0f, 0.0f, 0.0f, ANIMMODE_ONCE, -4.0f },   // ENLIFTNUTS_ANIM_TAKE_OFF_HAT
    { &gBusinessScrubFlyStartAnim, 0.0f, 0.0f, 0.0f, ANIMMODE_ONCE, -4.0f },     // ENLIFTNUTS_ANIM_FLY_START
    { &gBusinessScrubFlyLoopAnim, 1.0f, 0.0f, 0.0f, ANIMMODE_LOOP, -4.0f },      // ENLIFTNUTS_ANIM_FLY_LOOP
    { &gBusinessScrubShockedStartAnim, 1.0f, 0.0f, 0.0f, ANIMMODE_ONCE, -2.0f }, // ENLIFTNUTS_ANIM_SHOCKED_START
                                                                                 // ENLIFTNUTS_ANIM_SHOCKED_SHAKE_HEAD
    { &gBusinessScrubShockedShakeHeadAnim, 1.0f, 0.0f, 0.0f, ANIMMODE_LOOP, -4.0f },
    { &gBusinessScrubShockedPoundAnim, 1.0f, 0.0f, 0.0f, ANIMMODE_ONCE, -4.0f }, // ENLIFTNUTS_ANIM_SHOCKED_POUND
    { &gBusinessScrubShockedEndAnim, 1.0f, 0.0f, 0.0f, ANIMMODE_ONCE, -1.0f },   // ENLIFTNUTS_ANIM_SHOCKED_END
    { &gBusinessScrubThinkAnim, 1.0f, 0.0f, 0.0f, ANIMMODE_LOOP, -4.0f },        // ENLIFTNUTS_ANIM_THINK
    { &gBusinessScrubBobAnim, 1.0f, 0.0f, 0.0f, ANIMMODE_LOOP, -4.0f },          // ENLIFTNUTS_ANIM_BOB
    { &gBusinessScrubJumpAnim, 1.0f, 0.0f, 0.0f, ANIMMODE_ONCE, -4.0f },         // ENLIFTNUTS_ANIM_JUMP
    { &gBusinessScrubBurrowAnim, 1.0f, 4.0f, 0.0f, ANIMMODE_ONCE, -4.0f },       // ENLIFTNUTS_ANIM_BURROW_HALF
    { &gBusinessScrubBurrowAnim, 1.0f, 8.0f, 0.0f, ANIMMODE_ONCE, 0.0f },        // ENLIFTNUTS_ANIM_BURROW_SHORT
};

Gfx* sEyeTextures[] = { gBusinessScrubEyesDL, gBusinessScrubEyesWideDL, gBusinessScrubEyesSquintDL };

static s32 sPad = 0;

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
    { 25, 75, 0, { 0, 0, 0 } },
};

static CollisionCheckInfoInit2 sColChkInfoInit = { 0, 0, 0, 0, MASS_IMMOVABLE };

// #region 2S2H [Port]
// Moved static vars out of function scope so they can be cleared in actor reset
static s16 sMinigameScore = 0;
static s32 sAddedSharedMemory = false;
static s16 sFreedSharedMemory = false;
static s32 sIsAutotalkOn = false;
// #endregion

void EnLiftNuts_AddSharedMemoryEntry(EnLiftNuts* this, PlayState* play) {
    // 2S2H [Port] Moved to file level scope
    // static s16 sMinigameScore = 0;
    // static s32 sAddedSharedMemory = false;

    if (!sAddedSharedMemory) {
        Actor_AddSharedMemoryEntry(play, ACTOR_EN_GAMELUPY, &sMinigameScore, sizeof(s16));
        sAddedSharedMemory = true;
    }
    this->minigameScore = &sMinigameScore;
}

void EnLiftNuts_FreeSharedMemoryEntry(EnLiftNuts* this, PlayState* play) {
    // 2S2H [Port] Moved to file level scope
    // static s16 sFreedSharedMemory = false;

    if (!sFreedSharedMemory) {
        Actor_FreeSharedMemoryEntry(play, ACTOR_EN_GAMELUPY);
        sFreedSharedMemory = true;
    }
}

typedef enum {
    /* 0 */ ENLIFTNUTS_AUTOTALK_MODE_CHECK_OFF,
    /* 1 */ ENLIFTNUTS_AUTOTALK_MODE_SET_ON,
    /* 2 */ ENLIFTNUTS_AUTOTALK_MODE_SET_OFF
} EnLiftNutsAutotalkMode;

s32 EnLiftNuts_Autotalk(EnLiftNuts* this, EnLiftNutsAutotalkMode mode) {
    // 2S2H [Port] Moved to file level scope
    // static s32 sIsAutotalkOn = false;

    switch (mode) {
        case ENLIFTNUTS_AUTOTALK_MODE_CHECK_OFF:
            if (!sIsAutotalkOn) {
                return true;
            }
            break;

        case ENLIFTNUTS_AUTOTALK_MODE_SET_ON:
            if (!sIsAutotalkOn) {
                this->autotalk = true;
                sIsAutotalkOn = true;
                return true;
            }
            break;

        case ENLIFTNUTS_AUTOTALK_MODE_SET_OFF:
            if (sIsAutotalkOn == true) {
                this->autotalk = false;
                sIsAutotalkOn = false;
                return true;
            }
            break;

        default:
            break;
    }

    return false;
}

typedef enum {
    /* 0 */ ENLIFTNUTS_MINIGAME_STATE_MODE_CHECK,
    /* 1 */ ENLIFTNUTS_MINIGAME_STATE_MODE_SET
} EnLiftNutsMiniGameStateMode;

typedef enum {
    /* 0 */ ENLIFTNUTS_MINIGAME_STATE_NONE,
    /* 1 */ ENLIFTNUTS_MINIGAME_STATE_STARTING,
    /* 2 */ ENLIFTNUTS_MINIGAME_STATE_RUNNING,
    /* 3 */ ENLIFTNUTS_MINIGAME_STATE_AFTER,
    /* 4 */ ENLIFTNUTS_MINIGAME_STATE_HIDING
} EnLiftNutsMiniGameState;

/**
 * Will either check or set the current minigame state based on the given mode
 *
 * @return boolean, based on the mode
 *  ENLIFTNUTS_MINIGAME_STATE_MODE_CHECK: true if the current minigame state equals the provided state, false otherwise
 *  ENLIFTNUTS_MINIGAME_STATE_MODE_SET: true if the minigame state was set successfully to the provided state, false
 * otherwise
 */
s32 EnLiftNuts_MinigameState(EnLiftNutsMiniGameStateMode mode, EnLiftNutsMiniGameState state) {
    static EnLiftNutsMiniGameState sMinigameState = ENLIFTNUTS_MINIGAME_STATE_NONE;

    if (mode == ENLIFTNUTS_MINIGAME_STATE_MODE_CHECK) {
        if (sMinigameState == state) {
            return true;
        }
    } else if (mode == ENLIFTNUTS_MINIGAME_STATE_MODE_SET) {
        sMinigameState = state;
        return true;
    }
    return false;
}

s32 EnLiftNuts_GetNumDaysWon(void) {
    s32 numDays = 0;

    if (CHECK_WEEKEVENTREG(WEEKEVENTREG_WON_DEKU_PLAYGROUND_DAY_1)) {
        numDays++;
    }

    if (CHECK_WEEKEVENTREG(WEEKEVENTREG_WON_DEKU_PLAYGROUND_DAY_2)) {
        numDays++;
    }

    if (CHECK_WEEKEVENTREG(WEEKEVENTREG_WON_DEKU_PLAYGROUND_DAY_3)) {
        numDays++;
    }

    return numDays;
}

/**
 * Will check if the actor should hide, and if so starts hiding.
 */
void EnLiftNuts_TryHide(EnLiftNuts* this, PlayState* play) {
    if (((this->actionFunc == EnLiftNuts_Hide) || (this->actionFunc == EnLiftNuts_StartHiding) ||
         (this->actionFunc == EnLiftNuts_HandleConversation) || (this->actionFunc == EnLiftNuts_StartConversation) ||
         (this->actionFunc == EnLiftNuts_Burrow) || (this->actionFunc == EnLiftNuts_GiveReward) ||
         (this->actionFunc == EnLiftNuts_ResumeConversation))) {
        return;
    }

    if (EnLiftNuts_MinigameState(ENLIFTNUTS_MINIGAME_STATE_MODE_CHECK, ENLIFTNUTS_MINIGAME_STATE_AFTER)) {
        return;
    }

    if ((EnLiftNuts_GetNumDaysWon() == 3) && (GET_PLAYER_FORM == PLAYER_FORM_DEKU) &&
        (this->actor.xzDistToPlayer < 150.0f)) {
        EnLiftNuts_SetupStartHiding(this);
    }
}

void EnLiftNuts_Init(Actor* thisx, PlayState* play) {
    s32 pad;
    EnLiftNuts* this = (EnLiftNuts*)thisx;
    Path* path;
    Vec3s* points;

    ActorShape_Init(&this->actor.shape, 0.0f, ActorShadow_DrawCircle, 35.0f);
    SkelAnime_InitFlex(play, &this->skelAnime, &gBusinessScrubSkel, &gBusinessScrubBurrowAnim, this->jointTable,
                       this->morphTable, BUSINESS_SCRUB_LIMB_MAX);
    Collider_InitCylinder(play, &this->collider);
    Collider_SetCylinder(play, &this->collider, &this->actor, &sCylinderInit);
    CollisionCheck_SetInfo2(&this->actor.colChkInfo, DamageTable_Get(0x16), &sColChkInfoInit);
    Actor_UpdateBgCheckInfo(play, &this->actor, 0.0f, 0.0f, 0.0f, UPDBGCHECKINFO_FLAG_4);
    if (this->actor.floorBgId != BGCHECK_SCENE) {
        DynaPolyActor* bgActor = DynaPoly_GetActor(&play->colCtx, this->actor.floorBgId);

        if (bgActor != NULL) {
            this->actor.world.pos = bgActor->actor.world.pos;
            this->actor.home.pos = bgActor->actor.world.pos;
        }
    }
    this->actor.attentionRangeType = ATTENTION_RANGE_0;
    this->timer = 0;
    this->autotalk = false;
    this->isFirstTimeHiding = false;
    this->eyeTexIndex = 0;
    this->unk1E8 = 0;
    this->actor.gravity = -2.0f;

    path = &play->setupPathList[ENLIFTNUTS_GET_PATH_INDEX(&this->actor)];
    points = Lib_SegmentedToVirtual(path->points);
    this->waypointPos.x = points[0].x;
    this->waypointPos.y = points[0].y;
    this->waypointPos.z = points[0].z;
    EnLiftNuts_AddSharedMemoryEntry(this, play);
    if (!Flags_GetSwitch(play, 0x41)) {
        EnLiftNuts_MinigameState(ENLIFTNUTS_MINIGAME_STATE_MODE_SET, ENLIFTNUTS_MINIGAME_STATE_NONE);
        EnLiftNuts_SetupIdleHidden(this);
    } else if (EnLiftNuts_Autotalk(this, ENLIFTNUTS_AUTOTALK_MODE_CHECK_OFF)) {
        Player* player = GET_PLAYER(play);

        player->stateFlags1 |= PLAYER_STATE1_20;
        EnLiftNuts_Autotalk(this, ENLIFTNUTS_AUTOTALK_MODE_SET_ON);
        EnLiftNuts_MinigameState(ENLIFTNUTS_MINIGAME_STATE_MODE_SET, ENLIFTNUTS_MINIGAME_STATE_AFTER);
        EnLiftNuts_SetupIdle(this);
    } else {
        EnLiftNuts_SetupIdleHidden(this);
    }
}

void EnLiftNuts_Destroy(Actor* thisx, PlayState* play) {
    EnLiftNuts* this = (EnLiftNuts*)thisx;

    Collider_DestroyCylinder(play, &this->collider);
    EnLiftNuts_FreeSharedMemoryEntry(this, play);
}

void EnLiftNuts_SetupIdleHidden(EnLiftNuts* this) {
    Actor_ChangeAnimationByInfo(&this->skelAnime, sAnimationInfo, ENLIFTNUTS_ANIM_BURROW_SHORT);
    this->actionFunc = EnLiftNuts_IdleHidden;
    this->timer = 0;
}

void EnLiftNuts_IdleHidden(EnLiftNuts* this, PlayState* play) {
    if (this->timer < 40) {
        this->timer++;
    } else if (this->actor.xzDistToPlayer < 100.0f) {
        EnLiftNuts_SetupIdle(this);
    }
}

void EnLiftNuts_SetupBurrow(EnLiftNuts* this) {
    if (this->actionFunc == EnLiftNuts_Idle) {
        Actor_ChangeAnimationByInfo(&this->skelAnime, sAnimationInfo, ENLIFTNUTS_ANIM_BURROW_HALF);
    } else {
        Actor_ChangeAnimationByInfo(&this->skelAnime, sAnimationInfo, ENLIFTNUTS_ANIM_BURROW_LONG);
    }

    Actor_PlaySfx(&this->actor, NA_SE_EN_AKINDONUTS_HIDE);
    this->actionFunc = EnLiftNuts_Burrow;
}

void EnLiftNuts_Burrow(EnLiftNuts* this, PlayState* play) {
    if (Animation_OnFrame(&this->skelAnime, this->skelAnime.endFrame)) {
        EnLiftNuts_SetupIdleHidden(this);
    } else if (Animation_OnFrame(&this->skelAnime, 5.0f)) {
        EnLiftNuts_SpawnDust(this, play);
    }
}

void EnLiftNuts_SetupIdle(EnLiftNuts* this) {
    if (EnLiftNuts_MinigameState(ENLIFTNUTS_MINIGAME_STATE_MODE_CHECK, ENLIFTNUTS_MINIGAME_STATE_STARTING)) {
        this->actionFunc = EnLiftNuts_Idle;
    } else {
        Actor_ChangeAnimationByInfo(&this->skelAnime, sAnimationInfo, ENLIFTNUTS_ANIM_RISE_UP);
        Actor_PlaySfx(&this->actor, NA_SE_EN_AKINDONUTS_HIDE);
        this->actionFunc = EnLiftNuts_RiseUp;
    }
}

void EnLiftNuts_RiseUp(EnLiftNuts* this, PlayState* play) {
    if (Animation_OnFrame(&this->skelAnime, this->skelAnime.endFrame)) {
        Actor_ChangeAnimationByInfo(&this->skelAnime, sAnimationInfo, ENLIFTNUTS_ANIM_BOB);
        this->actionFunc = EnLiftNuts_Idle;
    } else if (Animation_OnFrame(&this->skelAnime, 8.0f)) {
        EnLiftNuts_SpawnDust(this, play);
    }
}

void EnLiftNuts_Idle(EnLiftNuts* this, PlayState* play) {
    if ((EnLiftNuts_MinigameState(ENLIFTNUTS_MINIGAME_STATE_MODE_CHECK, ENLIFTNUTS_MINIGAME_STATE_AFTER) ||
         EnLiftNuts_MinigameState(ENLIFTNUTS_MINIGAME_STATE_MODE_CHECK, ENLIFTNUTS_MINIGAME_STATE_STARTING)) &&
        (this->autotalk == true)) {
        this->actor.flags |= ACTOR_FLAG_TALK_OFFER_AUTO_ACCEPTED;
    } else if (this->actor.xzDistToPlayer > 120.0f) {
        EnLiftNuts_SetupBurrow(this);
    }
    if (Actor_TalkOfferAccepted(&this->actor, &play->state)) {
        if (GET_PLAYER_FORM == PLAYER_FORM_DEKU) {
            if (EnLiftNuts_MinigameState(ENLIFTNUTS_MINIGAME_STATE_MODE_CHECK, ENLIFTNUTS_MINIGAME_STATE_NONE)) {
                switch (CURRENT_DAY) {
                    case 1:
                        if ((CURRENT_TIME > CLOCK_TIME(23, 30)) || (CURRENT_TIME <= CLOCK_TIME(6, 0))) {
                            Message_StartTextbox(play, 0x27F7, &this->actor);
                            this->textId = 0x27F7;
                        } else if (CHECK_WEEKEVENTREG(WEEKEVENTREG_WON_DEKU_PLAYGROUND_DAY_1)) {
                            Message_StartTextbox(play, 0x27D9, &this->actor);
                            this->textId = 0x27D9;
                        } else {
                            Message_StartTextbox(play, 0x27DA, &this->actor);
                            this->textId = 0x27DA;
                        }
                        break;

                    case 2:
                        if ((CURRENT_TIME > CLOCK_TIME(23, 30)) || (CURRENT_TIME <= CLOCK_TIME(6, 0))) {
                            Message_StartTextbox(play, 0x27F7, &this->actor);
                            this->textId = 0x27F7;
                        } else {
                            if (CHECK_WEEKEVENTREG(WEEKEVENTREG_WON_DEKU_PLAYGROUND_DAY_2)) {
                                Message_StartTextbox(play, 0x27DB, &this->actor);
                                this->textId = 0x27DB;
                                break;
                            }

                            if (CHECK_WEEKEVENTREG(WEEKEVENTREG_WON_DEKU_PLAYGROUND_DAY_1)) {
                                Message_StartTextbox(play, 0x27DC, &this->actor);
                                this->textId = 0x27DC;
                            } else {
                                Message_StartTextbox(play, 0x27DD, &this->actor);
                                this->textId = 0x27DD;
                            }
                        }
                        break;

                    case 3:
                        if ((CURRENT_TIME > CLOCK_TIME(23, 30)) || (CURRENT_TIME <= CLOCK_TIME(6, 0))) {
                            Message_StartTextbox(play, 0x27F7, &this->actor);
                            this->textId = 0x27F7;
                        } else if (CHECK_WEEKEVENTREG(WEEKEVENTREG_WON_DEKU_PLAYGROUND_DAY_3)) {
                            Message_StartTextbox(play, 0x27DE, &this->actor);
                            this->textId = 0x27DE;
                        } else if (EnLiftNuts_GetNumDaysWon() == 2) {
                            Message_StartTextbox(play, 0x27DF, &this->actor);
                            this->textId = 0x27DF;
                        } else if (EnLiftNuts_GetNumDaysWon() == 1) {
                            Message_StartTextbox(play, 0x27E0, &this->actor);
                            this->textId = 0x27E0;
                        } else {
                            Message_StartTextbox(play, 0x27E1, &this->actor);
                            this->textId = 0x27E1;
                        }
                        break;

                    default:
                        break;
                }
            } else if (EnLiftNuts_MinigameState(ENLIFTNUTS_MINIGAME_STATE_MODE_CHECK,
                                                ENLIFTNUTS_MINIGAME_STATE_AFTER)) {
                if (Flags_GetSwitch(play, 0x40)) {
                    Flags_UnsetSwitch(play, 0x40);
                    Inventory_SaveDekuPlaygroundHighScore(TIMER_ID_MINIGAME_2);
                    if (CHECK_WEEKEVENTREG(WEEKEVENTREG_WON_DEKU_PLAYGROUND_DAY_1) &&
                        CHECK_WEEKEVENTREG(WEEKEVENTREG_WON_DEKU_PLAYGROUND_DAY_2) && (CURRENT_DAY == 3)) {
                        this->timer = 0;
                        Message_StartTextbox(play, 0x27F4, &this->actor);
                        this->textId = 0x27F4;
                    } else {
                        Message_StartTextbox(play, 0x27EE, &this->actor);
                        this->textId = 0x27EE;
                    }
                } else {
                    if (((void)0, gSaveContext.timerCurTimes[TIMER_ID_MINIGAME_2]) >=
                        gSaveContext.save.saveInfo.dekuPlaygroundHighScores[CURRENT_DAY - 1]) {
                        if (gSaveContext.timerCurTimes[TIMER_ID_MINIGAME_2] < SECONDS_TO_TIMER(120)) {
                            Message_StartTextbox(play, 0x27F9, &this->actor);
                            this->textId = 0x27F9;
                        } else {
                            Message_StartTextbox(play, 0x27EB, &this->actor);
                            this->textId = 0x27EB;
                        }
                    } else {
                        Message_StartTextbox(play, 0x27ED, &this->actor);
                        this->textId = 0x27ED;
                    }
                }
                Flags_UnsetSwitch(play, 0x41);
                this->actor.flags &= ~ACTOR_FLAG_TALK_OFFER_AUTO_ACCEPTED;
            } else if (!Flags_GetSwitch(play, 0x42)) { // Explain Rules
                Flags_SetSwitch(play, 0x42);
                Message_StartTextbox(play, 0x27E6, &this->actor);
                this->textId = 0x27E6;
            } else {
                switch (CURRENT_DAY) {
                    case 1:
                        Message_StartTextbox(play, 0x27E7, &this->actor);
                        this->textId = 0x27E7;
                        break;

                    case 2:
                        Message_StartTextbox(play, 0x27E8, &this->actor);
                        this->textId = 0x27E8;
                        break;

                    case 3:
                        Message_StartTextbox(play, 0x27E9, &this->actor);
                        this->textId = 0x27E9;
                        break;

                    default:
                        break;
                }
            }
        } else {
            Message_StartTextbox(play, 0x27D8, &this->actor);
            this->textId = 0x27D8;
        }
        EnLiftNuts_SetupStartConversation(this);
    } else if (EnLiftNuts_Autotalk(this, ENLIFTNUTS_AUTOTALK_MODE_CHECK_OFF) || (this->autotalk == true)) {
        if (this->autotalk == true) {
            Actor_OfferTalk(&this->actor, play, 200.0f);
        } else if (this->actor.playerHeightRel >= -13.0f) {
            Actor_OfferTalk(&this->actor, play, 100.0f);
        }
    }
    Math_SmoothStepToS(&this->actor.shape.rot.y, this->actor.yawTowardsPlayer, 0xA, 0x1000, 0x500);
}

void EnLiftNuts_HandleConversationChoice(EnLiftNuts* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    if (Message_ShouldAdvance(play)) {
        switch (this->textId) {
            case 0x27E2:
                if (play->msgCtx.choiceIndex == 0) { // Yes
                    if (gSaveContext.save.saveInfo.playerData.rupees >= 10) {
                        Audio_PlaySfx_MessageDecide();
                        Message_StartTextbox(play, 0x27E5, &this->actor);
                        this->textId = 0x27E5;
                        Rupees_ChangeBy(-10);
                    } else {
                        Audio_PlaySfx(NA_SE_SY_ERROR);
                        Actor_ChangeAnimationByInfo(&this->skelAnime, sAnimationInfo, ENLIFTNUTS_ANIM_WALK);
                        Message_StartTextbox(play, 0x27E4, &this->actor);
                        this->textId = 0x27E4;
                    }
                } else {
                    Audio_PlaySfx_MessageCancel();
                    Actor_ChangeAnimationByInfo(&this->skelAnime, sAnimationInfo, ENLIFTNUTS_ANIM_WALK);
                    Message_StartTextbox(play, 0x27E3, &this->actor);
                    this->textId = 0x27E3;
                }
                break;

            case 0x238D:
                if (play->msgCtx.choiceIndex == 0) { // Yes
                    player->stateFlags1 |= PLAYER_STATE1_20;
                    EnLiftNuts_SetupStartGameImmediately(this);
                } else {
                    EnLiftNuts_SetupBurrow(this);
                }
                break;

            default:
                EnLiftNuts_SetupBurrow(this);
                break;
        }
    }
}

void EnLiftNuts_HandleConversationEvent(EnLiftNuts* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    if (Message_ShouldAdvance(play)) {
        switch (this->textId) {
            case 0x27D9:
            case 0x27DA:
            case 0x27DB:
            case 0x27DC:
            case 0x27DD:
            case 0x27DE:
            case 0x27DF:
            case 0x27E0:
            case 0x27E1:
                Message_StartTextbox(play, 0x27E2, &this->actor);
                this->textId = 0x27E2;
                break;

            case 0x27E3:
            case 0x27E4:
                EnLiftNuts_SetupBurrow(this);
                break;

            case 0x27E5:
                Message_CloseTextbox(play);
                player->stateFlags1 |= PLAYER_STATE1_20;
                EnLiftNuts_SetupMove(this);
                break;

            case 0x27E6:
                Actor_ChangeAnimationByInfo(&this->skelAnime, sAnimationInfo, ENLIFTNUTS_ANIM_WALK);
                switch (CURRENT_DAY) {
                    case 1:
                        Message_StartTextbox(play, 0x27E7, &this->actor);
                        this->textId = 0x27E7;
                        break;

                    case 2:
                        Message_StartTextbox(play, 0x27E8, &this->actor);
                        this->textId = 0x27E8;
                        break;

                    case 3:
                        Message_StartTextbox(play, 0x27E9, &this->actor);
                        this->textId = 0x27E9;
                        break;

                    default:
                        break;
                }
                break;

            case 0x27E7:
            case 0x27E8:
            case 0x27E9:
                Message_StartTextbox(play, 0x27FA, &this->actor);
                this->textId = 0x27FA;
                break;

            case 0x27FA:
                Message_CloseTextbox(play);
                player->stateFlags1 &= ~PLAYER_STATE1_20;
                EnLiftNuts_SetupStartGame(this);
                break;

            case 0x27EE:
                Actor_ChangeAnimationByInfo(&this->skelAnime, sAnimationInfo, ENLIFTNUTS_ANIM_JUMP);
                Message_StartTextbox(play, 0x27EF, &this->actor);
                this->textId = 0x27EF;
                break;

            case 0x27EF:
                Message_CloseTextbox(play);
                EnLiftNuts_SetupGiveReward(this);
                EnLiftNuts_GiveReward(this, play);
                break;

            case 0x27F1:
                Message_StartTextbox(play, 0x27F2, &this->actor);
                this->textId = 0x27F2;
                break;

            case 0x27F2:
                Actor_ChangeAnimationByInfo(&this->skelAnime, sAnimationInfo, ENLIFTNUTS_ANIM_WALK);
                Message_StartTextbox(play, 0x27F3, &this->actor);
                this->textId = 0x27F3;
                break;

            case 0x27F4:
                Message_CloseTextbox(play);
                EnLiftNuts_SetupGiveReward(this);
                EnLiftNuts_GiveReward(this, play);
                break;

            case 0x27F5:
                Message_CloseTextbox(play);
                EnLiftNuts_MinigameState(ENLIFTNUTS_MINIGAME_STATE_MODE_SET, ENLIFTNUTS_MINIGAME_STATE_NONE);
                player->stateFlags1 &= ~PLAYER_STATE1_20;
                EnLiftNuts_SetupBurrow(this);
                break;

            case 0x27F9:
                Message_StartTextbox(play, 0x27ED, &this->actor);
                this->textId = 0x27ED;
                break;

            default:
                break;
        }
    }
}

void EnLiftNuts_SetupStartConversation(EnLiftNuts* this) {
    this->timer = 0;

    if (this->actionFunc != EnLiftNuts_ResumeConversation) {
        if (EnLiftNuts_MinigameState(ENLIFTNUTS_MINIGAME_STATE_MODE_CHECK, ENLIFTNUTS_MINIGAME_STATE_NONE)) {
            Actor_PlaySfx(&this->actor, NA_SE_EN_AKINDONUTS_HIDE);
        }
        Actor_ChangeAnimationByInfo(&this->skelAnime, sAnimationInfo, ENLIFTNUTS_ANIM_JUMP);
    }
    this->actionFunc = EnLiftNuts_StartConversation;
}

void EnLiftNuts_StartConversation(EnLiftNuts* this, PlayState* play) {
    if (Animation_OnFrame(&this->skelAnime, this->skelAnime.endFrame)) {
        if ((this->textId == 0x27EE) || (this->textId == 0x27F4) || (this->textId == 0x27F5)) {
            Actor_ChangeAnimationByInfo(&this->skelAnime, sAnimationInfo, ENLIFTNUTS_ANIM_SHOCKED_START);
        } else {
            Actor_ChangeAnimationByInfo(&this->skelAnime, sAnimationInfo, ENLIFTNUTS_ANIM_STANDING);
        }
        this->actionFunc = EnLiftNuts_HandleConversation;
    }
}

void EnLiftNuts_HandleConversation(EnLiftNuts* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    switch (Message_GetState(&play->msgCtx)) {
        case TEXT_STATE_NONE:
        case TEXT_STATE_NEXT:
        case TEXT_STATE_CLOSING:
        case TEXT_STATE_FADING:
            break;

        case TEXT_STATE_CHOICE:
            EnLiftNuts_HandleConversationChoice(this, play);
            break;

        case TEXT_STATE_EVENT:
            EnLiftNuts_HandleConversationEvent(this, play);
            break;

        case TEXT_STATE_DONE:
            if (Message_ShouldAdvance(play)) {
                player->stateFlags1 &= ~PLAYER_STATE1_20;
                EnLiftNuts_SetupBurrow(this);
                EnLiftNuts_Autotalk(this, ENLIFTNUTS_AUTOTALK_MODE_SET_OFF);
                if (EnLiftNuts_MinigameState(ENLIFTNUTS_MINIGAME_STATE_MODE_CHECK, ENLIFTNUTS_MINIGAME_STATE_AFTER)) {
                    EnLiftNuts_MinigameState(ENLIFTNUTS_MINIGAME_STATE_MODE_SET, ENLIFTNUTS_MINIGAME_STATE_NONE);
                }
            }
            break;

        default:
            break;
    }

    Math_SmoothStepToS(&this->actor.shape.rot.y, this->actor.yawTowardsPlayer, 0xA, 0x2000, 0x500);
    if (Animation_OnFrame(&this->skelAnime, this->skelAnime.endFrame)) {
        switch (this->textId) {
            case 0x27EE:
                Actor_ChangeAnimationByInfo(&this->skelAnime, sAnimationInfo, ENLIFTNUTS_ANIM_SHOCKED_SHAKE_HEAD);
                break;

            case 0x27EF:
                Actor_ChangeAnimationByInfo(&this->skelAnime, sAnimationInfo, ENLIFTNUTS_ANIM_WALK);
                break;

            case 0x27F4:
                if (this->timer == 0) {
                    Actor_ChangeAnimationByInfo(&this->skelAnime, sAnimationInfo, ENLIFTNUTS_ANIM_SHOCKED_SHAKE_HEAD);
                    this->timer++;
                } else if (this->timer == 4) {
                    Actor_ChangeAnimationByInfo(&this->skelAnime, sAnimationInfo, ENLIFTNUTS_ANIM_SHOCKED_POUND);
                    this->timer = 0;
                } else {
                    this->timer++;
                }
                break;

            default:
                break;
        }
    }
    EnLiftNuts_UpdateEyes(this);
}

void EnLiftNuts_SetupMove(EnLiftNuts* this) {
    this->actor.speed = 2.0f;
    Actor_ChangeAnimationByInfo(&this->skelAnime, sAnimationInfo, ENLIFTNUTS_ANIM_WALK);
    EnLiftNuts_Autotalk(this, ENLIFTNUTS_AUTOTALK_MODE_SET_ON);
    EnLiftNuts_MinigameState(ENLIFTNUTS_MINIGAME_STATE_MODE_SET, ENLIFTNUTS_MINIGAME_STATE_STARTING);
    this->actionFunc = EnLiftNuts_Move;
}

void EnLiftNuts_Move(EnLiftNuts* this, PlayState* play) {
    f32 dist;

    Math_SmoothStepToS(&this->actor.shape.rot.y, this->actor.home.rot.y, 10, 0x1000, 0x500);
    dist = Math_Vec3f_StepTo(&this->actor.world.pos, &this->waypointPos, this->actor.speed);
    this->actor.world.pos.y += this->actor.gravity;

    if (dist == 0.0f) {
        EnLiftNuts_SetupMovePlayerToActor(this);
    }
}

void EnLiftNuts_SetupMovePlayerToActor(EnLiftNuts* this) {
    this->actionFunc = EnLiftNuts_MovePlayerToActor;
}

void EnLiftNuts_MovePlayerToActor(EnLiftNuts* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    f32 distXZ;
    f32 controlStickMagnitude;
    s16 controlStickAngle;
    s16 yaw;
    s16 yawDiff;

    yaw = this->actor.yawTowardsPlayer - 0x8000;
    controlStickAngle = Math_Vec3f_Yaw(&player->actor.world.pos, &this->actor.home.pos);
    yawDiff = controlStickAngle - yaw;
    distXZ = Math_Vec3f_DistXZ(&player->actor.world.pos, &this->actor.home.pos);

    if (this->actor.xzDistToPlayer < distXZ) {
        if (ABS_ALT(yawDiff) < 0x2000) {
            controlStickAngle = (yawDiff > 0) ? (controlStickAngle + 0x2000) : (controlStickAngle - 0x2000);
        }
    }

    if (distXZ < 5.0f) {
        controlStickMagnitude = 10.0f;
    } else if (distXZ < 30.0f) {
        controlStickMagnitude = 40.0f;
    } else {
        controlStickMagnitude = 80.0f;
    }

    play->actorCtx.isOverrideInputOn = true;
    Actor_SetControlStickData(play, &play->actorCtx.overrideInput, controlStickMagnitude, controlStickAngle);

    if (distXZ < 5.0f) {
        EnLiftNuts_SetupIdle(this);
    }
}

void EnLiftNuts_SetupStartGame(EnLiftNuts* this) {
    Audio_PlaySubBgm(NA_BGM_TIMED_MINI_GAME);
    this->actionFunc = EnLiftNuts_StartGame;
}

void EnLiftNuts_StartGame(EnLiftNuts* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    if (player->stateFlags3 & PLAYER_STATE3_200) {
        this->actor.speed = 2.0f;
        SET_EVENTINF(EVENTINF_34);
        Interface_StartTimer(TIMER_ID_MINIGAME_2, 0);
        EnLiftNuts_MinigameState(ENLIFTNUTS_MINIGAME_STATE_MODE_SET, ENLIFTNUTS_MINIGAME_STATE_RUNNING);
        Actor_PlaySfx(&this->actor, NA_SE_SY_FOUND);
        EnLiftNuts_SetupRunGame(this);
    }
}

/**
 * Does not wait for the player to pop out of the flower to start the minigame.
 */
void EnLiftNuts_SetupStartGameImmediately(EnLiftNuts* this) {
    this->actor.speed = 2.0f;
    SET_EVENTINF(EVENTINF_34);
    Interface_StartTimer(TIMER_ID_MINIGAME_2, 0);
    EnLiftNuts_MinigameState(ENLIFTNUTS_MINIGAME_STATE_MODE_SET, ENLIFTNUTS_MINIGAME_STATE_RUNNING);
    this->actionFunc = EnLiftNuts_StartGameImmediately;
}

void EnLiftNuts_StartGameImmediately(EnLiftNuts* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    if (gSaveContext.timerCurTimes[TIMER_ID_MINIGAME_2] > SECONDS_TO_TIMER(0)) {
        player->stateFlags1 &= ~PLAYER_STATE1_20;
        EnLiftNuts_SetupRunGame(this);
    }
}

void EnLiftNuts_SetupRunGame(EnLiftNuts* this) {
    this->actionFunc = EnLiftNuts_RunGame;
}

void EnLiftNuts_RunGame(EnLiftNuts* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    if (((player->actor.bgCheckFlags & BGCHECKFLAG_GROUND) && (player->actor.floorBgId == BG_ACTOR_MAX) &&
         (player->actor.world.pos.y < 20.0f)) ||
        (gSaveContext.timerCurTimes[TIMER_ID_MINIGAME_2] >= SECONDS_TO_TIMER(120))) {
        player->stateFlags1 |= PLAYER_STATE1_20;
        Flags_SetSwitch(play, 0x41);
        EnLiftNuts_SetupEndGame(this, play);
    }

    if (*this->minigameScore == (ENGAMELUPY_POINTS * 6)) {
        player->stateFlags1 |= PLAYER_STATE1_20;

        if (((void)0, gSaveContext.timerCurTimes[TIMER_ID_MINIGAME_2]) <
            gSaveContext.save.saveInfo.dekuPlaygroundHighScores[CURRENT_DAY - 1]) {
            Flags_SetSwitch(play, 0x40);
        }
        Flags_SetSwitch(play, 0x41);
        EnLiftNuts_SetupEndGame(this, play);
    }
}

void EnLiftNuts_SetupEndGame(EnLiftNuts* this, PlayState* play) {
    Audio_PlaySfx(NA_SE_SY_FOUND);
    this->timer = 0;
    gSaveContext.timerStates[TIMER_ID_MINIGAME_2] = TIMER_STATE_6;
    this->actionFunc = EnLiftNuts_EndGame;
}

void EnLiftNuts_EndGame(EnLiftNuts* this, PlayState* play) {
    s32 pad;

    if (this->timer == 10) {
        if (((void)0, gSaveContext.timerCurTimes[TIMER_ID_MINIGAME_2]) >
            gSaveContext.save.saveInfo.dekuPlaygroundHighScores[CURRENT_DAY - 1]) {
            Message_StartTextbox(play, 0x27EA, &this->actor);
            this->textId = 0x27EA;
        } else if (*this->minigameScore == (ENGAMELUPY_POINTS * 6)) {
            Message_StartTextbox(play, 0x27F8, &this->actor);
            this->textId = 0x27F8;
        } else {
            Message_StartTextbox(play, 0x27EC, &this->actor);
            this->textId = 0x27EC;
        }
    } else if (this->timer == 30) {
        CLEAR_EVENTINF(EVENTINF_34);
        gSaveContext.respawn[RESPAWN_MODE_DOWN].entrance = ENTRANCE(DEKU_SCRUB_PLAYGROUND, 1);
        gSaveContext.nextCutsceneIndex = 0;
        func_80169EFC(play);
        gSaveContext.respawnFlag = -2;
        play->transitionType = TRANS_TYPE_64;
        gSaveContext.nextTransitionType = TRANS_TYPE_FADE_BLACK;
    }
    this->timer++;
}

void EnLiftNuts_SetupGiveReward(EnLiftNuts* this) {
    this->actionFunc = EnLiftNuts_GiveReward;
}

void EnLiftNuts_GiveReward(EnLiftNuts* this, PlayState* play) {
    if (Actor_HasParent(&this->actor, play)) {
        this->actor.parent = NULL;
        if (CHECK_WEEKEVENTREG(WEEKEVENTREG_WON_DEKU_PLAYGROUND_DAY_1) &&
            CHECK_WEEKEVENTREG(WEEKEVENTREG_WON_DEKU_PLAYGROUND_DAY_2) && (CURRENT_DAY == 3) &&
            !CHECK_WEEKEVENTREG(WEEKEVENTREG_RECEIVED_DEKU_PLAYGROUND_HEART_PIECE)) {
            SET_WEEKEVENTREG(WEEKEVENTREG_RECEIVED_DEKU_PLAYGROUND_HEART_PIECE);
        }
        EnLiftNuts_SetupResumeConversation(this);
    } else if ((this->textId == 0x27F4) && !CHECK_WEEKEVENTREG(WEEKEVENTREG_RECEIVED_DEKU_PLAYGROUND_HEART_PIECE)) {
        Actor_OfferGetItem(&this->actor, play, GI_HEART_PIECE, 500.0f, 100.0f);
    } else {
        Actor_OfferGetItem(&this->actor, play, GI_RUPEE_PURPLE, 500.0f, 100.0f);
    }
}

void EnLiftNuts_SetupResumeConversation(EnLiftNuts* this) {
    this->actionFunc = EnLiftNuts_ResumeConversation;
}

/**
 * Resumes the current conversation after giving player the reward for winning the minigame.
 */
void EnLiftNuts_ResumeConversation(EnLiftNuts* this, PlayState* play) {
    if (Actor_TalkOfferAccepted(&this->actor, &play->state)) {
        if (CHECK_WEEKEVENTREG(WEEKEVENTREG_WON_DEKU_PLAYGROUND_DAY_1) &&
            CHECK_WEEKEVENTREG(WEEKEVENTREG_WON_DEKU_PLAYGROUND_DAY_2) && (CURRENT_DAY == 3)) {
            Actor_ChangeAnimationByInfo(&this->skelAnime, sAnimationInfo, ENLIFTNUTS_ANIM_SHOCKED_END);
            Message_StartTextbox(play, 0x27F5, &this->actor);
            this->textId = 0x27F5;
        } else if (EnLiftNuts_GetNumDaysWon() > 0) {
            Actor_ChangeAnimationByInfo(&this->skelAnime, sAnimationInfo, ENLIFTNUTS_ANIM_WALK);
            Message_StartTextbox(play, 0x27F0, &this->actor);
            this->textId = 0x27F0;
        } else {
            Message_StartTextbox(play, 0x27F1, &this->actor);
            this->textId = 0x27F1;
        }
        this->actor.flags &= ~ACTOR_FLAG_TALK_OFFER_AUTO_ACCEPTED;
        EnLiftNuts_SetupStartConversation(this);
        switch (CURRENT_DAY) {
            case 1:
                SET_WEEKEVENTREG(WEEKEVENTREG_WON_DEKU_PLAYGROUND_DAY_1);
                break;

            case 2:
                SET_WEEKEVENTREG(WEEKEVENTREG_WON_DEKU_PLAYGROUND_DAY_2);
                break;

            case 3:
                SET_WEEKEVENTREG(WEEKEVENTREG_WON_DEKU_PLAYGROUND_DAY_3);
                break;

            default:
                break;
        }
    } else {
        Actor_OfferTalkExchangeEquiCylinder(&this->actor, play, 200.0f, PLAYER_IA_MINUS1);
    }
}

void EnLiftNuts_SetupStartHiding(EnLiftNuts* this) {
    if (!EnLiftNuts_MinigameState(ENLIFTNUTS_MINIGAME_STATE_MODE_CHECK, ENLIFTNUTS_MINIGAME_STATE_HIDING)) {
        EnLiftNuts_MinigameState(ENLIFTNUTS_MINIGAME_STATE_MODE_SET, ENLIFTNUTS_MINIGAME_STATE_HIDING);
        this->isFirstTimeHiding = true;
    }
    if (this->actionFunc == EnLiftNuts_IdleHidden) {
        Actor_ChangeAnimationByInfo(&this->skelAnime, sAnimationInfo, ENLIFTNUTS_ANIM_BOB);
    }
    this->timer = 0;
    this->actionFunc = EnLiftNuts_StartHiding;
}

void EnLiftNuts_StartHiding(EnLiftNuts* this, PlayState* play) {
    if (this->timer == 22) {
        if (this->isFirstTimeHiding == true) {
            Message_StartTextbox(play, 0x27F6, &this->actor);
            this->textId = 0x27F6;
        }
        Actor_ChangeAnimationByInfo(&this->skelAnime, sAnimationInfo, ENLIFTNUTS_ANIM_BURROW_HALF);
        this->actionFunc = EnLiftNuts_Hide;
    }
    this->timer++;
}

void EnLiftNuts_Hide(EnLiftNuts* this, PlayState* play) {
    if (Animation_OnFrame(&this->skelAnime, 5.0f)) {
        EnLiftNuts_SpawnDust(this, play);
    }
}

void EnLiftNuts_UpdateEyes(EnLiftNuts* this) {
    switch (this->textId) {
        case 0x27EE:
        case 0x27EF:
            this->eyeTexIndex = 1;
            break;

        case 0x27F5:
            this->eyeTexIndex = 2;
            break;

        case 0x27F4:
            if (this->timer == 0) {
                this->eyeTexIndex = 2;
            } else {
                this->eyeTexIndex = 1;
            }
            break;

        default:
            this->eyeTexIndex = 0;
            break;
    }
}

void EnLiftNuts_SpawnDust(EnLiftNuts* this, PlayState* play) {
    Vec3f pos = this->actor.world.pos;
    Vec3f velocity;
    Vec3f accel;
    Color_RGBA8 primColor = { 170, 130, 90, 255 };
    Color_RGBA8 envColor = { 100, 60, 20, 255 };
    s32 i;

    accel.y = 0.0f;

    for (i = 0; i < 30; i++) {
        velocity.x = Rand_Centered() * 15.0f;
        velocity.y = Rand_ZeroOne() * 2.0f;
        velocity.z = Rand_Centered() * 15.0f;

        accel.x = -0.2f * velocity.x;
        accel.z = -0.2f * velocity.z;

        func_800B0EB0(play, &pos, &velocity, &accel, &primColor, &envColor, 80, 25, 10);
    }
}

void EnLiftNuts_UpdateCollision(EnLiftNuts* this, PlayState* play) {
    Collider_UpdateCylinder(&this->actor, &this->collider);
    CollisionCheck_SetOC(play, &play->colChkCtx, &this->collider.base);
}

void EnLiftNuts_Update(Actor* thisx, PlayState* play) {
    EnLiftNuts* this = (EnLiftNuts*)thisx;

    SkelAnime_Update(&this->skelAnime);
    this->actionFunc(this, play);
    EnLiftNuts_UpdateCollision(this, play);
    Actor_UpdateBgCheckInfo(play, &this->actor, 0.0f, 0.0f, 0.0f, UPDBGCHECKINFO_FLAG_4);
    EnLiftNuts_TryHide(this, play);

    if (EnLiftNuts_MinigameState(ENLIFTNUTS_MINIGAME_STATE_MODE_CHECK, ENLIFTNUTS_MINIGAME_STATE_RUNNING)) {
        this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
    }
}

s32 EnLiftNuts_OverrideLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, Actor* thisx) {
    EnLiftNuts* this = (EnLiftNuts*)thisx;

    if ((limbIndex == BUSINESS_SCRUB_LIMB_RIGHT_HAND_HAT) || (limbIndex == BUSINESS_SCRUB_LIMB_RIGHT_HAND_BAG) ||
        (limbIndex == BUSINESS_SCRUB_LIMB_LEFT_HAND_BAG) || (limbIndex == BUSINESS_SCRUB_LIMB_SCALP) ||
        (limbIndex == BUSINESS_SCRUB_LIMB_HAIR) || (limbIndex == BUSINESS_SCRUB_LIMB_LEFT_HAND_BAG) ||
        (limbIndex == BUSINESS_SCRUB_LIMB_LEFT_ARM_BAG)) {
        *dList = NULL;
    }

    if (limbIndex == BUSINESS_SCRUB_LIMB_EYES) {
        *dList = sEyeTextures[this->eyeTexIndex];
    }
    return false;
}

void EnLiftNuts_PostLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot, Actor* thisx) {
    static Vec3f sFocusOffset = { 0.0f, 0.0f, 0.0f };
    EnLiftNuts* this = (EnLiftNuts*)thisx;

    if (limbIndex == BUSINESS_SCRUB_LIMB_HAT) {
        Matrix_MultVec3f(&sFocusOffset, &this->actor.focus.pos);
    }
}

void EnLiftNuts_Draw(Actor* thisx, PlayState* play) {
    EnLiftNuts* this = (EnLiftNuts*)thisx;

    Gfx_SetupDL25_Opa(play->state.gfxCtx);
    SkelAnime_DrawFlexOpa(play, this->skelAnime.skeleton, this->skelAnime.jointTable, this->skelAnime.dListCount,
                          EnLiftNuts_OverrideLimbDraw, EnLiftNuts_PostLimbDraw, thisx);
}

void EnLiftNuts_Reset(void) {
    sMinigameScore = 0;
    sAddedSharedMemory = false;
    sFreedSharedMemory = false;
    sIsAutotalkOn = false;
}
