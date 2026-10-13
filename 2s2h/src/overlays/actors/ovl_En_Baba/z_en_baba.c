/*
 * File: z_en_baba.c
 * Overlay: ovl_En_Baba
 * Description: Bomb Shop Lady
 */

#include "z_en_baba.h"
#include "2s2h/GameInteractor/GameInteractor.h"

#define FLAGS (ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_FRIENDLY | ACTOR_FLAG_UPDATE_CULLING_DISABLED)

#define BOMB_SHOP_LADY_STATE_END_CONVERSATION (1 << 0)
#define BOMB_SHOP_LADY_STATE_VISIBLE (1 << 1)
#define BOMB_SHOP_LADY_STATE_KNOCKED_OVER (1 << 2) // Don't track player
#define BOMB_SHOP_LADY_STATE_AUTOTALK (1 << 3)
#define BOMB_SHOP_LADY_STATE_GIVE_BLAST_MASK (1 << 5)
#define BOMB_SHOP_LADY_STATE_GAVE_BLAST_MASK (1 << 6)
#define BOMB_SHOP_LADY_STATE_DRAW_SHADOW (1 << 7)

void EnBaba_Init(Actor* thisx, PlayState* play);
void EnBaba_Destroy(Actor* thisx, PlayState* play);
void EnBaba_Update(Actor* thisx, PlayState* play);
void EnBaba_Draw(Actor* thisx, PlayState* play);

void EnBaba_FinishInit(EnBaba* this, PlayState* play);
void EnBaba_Idle(EnBaba* this, PlayState* play);
void EnBaba_FollowSchedule_Talk(EnBaba* this, PlayState* play);
void EnBaba_Talk(EnBaba* this, PlayState* play);
void EnBaba_GiveBlastMask(EnBaba* this, PlayState* play);
void EnBaba_GaveBlastMask(EnBaba* this, PlayState* play);
void EnBaba_FollowSchedule(EnBaba* this, PlayState* play);
void EnBaba_KnockedOver(EnBaba* this, PlayState* play);
void EnBaba_DoNothing(EnBaba* this, PlayState* play);
void EnBaba_Walk(EnBaba* this, PlayState* play);
void EnBaba_FaceForward(EnBaba* this, PlayState* play);

typedef enum {
    /* 0 */ BOMB_SHOP_LADY_SCH_NONE,
    /* 1 */ BOMB_SHOP_LADY_SCH_KNOCKED_OVER,
    /* 2 */ BOMB_SHOP_LADY_SCH_FOLLOW_TIME_PATH
} BombShopLadyScheduleResult;

ActorProfile En_Baba_Profile = {
    /**/ ACTOR_EN_BABA,
    /**/ ACTORCAT_NPC,
    /**/ FLAGS,
    /**/ OBJECT_BBA,
    /**/ sizeof(EnBaba),
    /**/ EnBaba_Init,
    /**/ EnBaba_Destroy,
    /**/ EnBaba_Update,
    /**/ EnBaba_Draw,
};

typedef enum {
    /* 0 */ BOMB_SHOP_LADY_ANIM_IDLE_HOLDING_BAG,
    /* 1 */ BOMB_SHOP_LADY_ANIM_IDLE,
    /* 2 */ BOMB_SHOP_LADY_ANIM_WALKING_HOLDING_BAG,
    /* 3 */ BOMB_SHOP_LADY_ANIM_KNOCKED_OVER,
    /* 4 */ BOMB_SHOP_LADY_ANIM_LYING_DOWN,
    /* 5 */ BOMB_SHOP_LADY_ANIM_SWAY,
    /* 6 */ BOMB_SHOP_LADY_ANIM_MAX
} BombShopLadyAnimation;

static AnimationInfo sAnimationInfo[BOMB_SHOP_LADY_ANIM_MAX] = {
    { &gBombShopLadyIdleHoldingBagAnim, 1.0f, 0.0f, 0.0f, ANIMMODE_LOOP, 0.0f }, // BOMB_SHOP_LADY_ANIM_IDLE_HOLDING_BAG
    { &gBombShopLadyIdleAnim, 1.0f, 0.0f, 0.0f, ANIMMODE_LOOP, 0.0f },           // BOMB_SHOP_LADY_ANIM_IDLE
    { &gBombShopLadyWalkingHoldingBagAnim, 1.0f, 0.0f, 0.0f, ANIMMODE_LOOP,
      0.0f },                                                                 // BOMB_SHOP_LADY_ANIM_WALKING_HOLDING_BAG
    { &gBombShopLadyKnockedOverAnim, 1.0f, 0.0f, 0.0f, ANIMMODE_ONCE, 0.0f }, // BOMB_SHOP_LADY_ANIM_KNOCKED_OVER
    { &gBombShopLadyLyingDownAnim, 1.0f, 0.0f, 0.0f, ANIMMODE_LOOP, 0.0f },   // BOMB_SHOP_LADY_ANIM_LYING_DOWN
    { &gBombShopLadySwayAnim, 1.0f, 0.0f, 0.0f, ANIMMODE_LOOP, 0.0f },        // BOMB_SHOP_LADY_ANIM_SWAY
};

static ColliderCylinderInit sCylinderInit = {
    {
        COL_MATERIAL_NONE,
        AT_NONE,
        AC_ON | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_1,
        COLSHAPE_CYLINDER,
    },
    {
        ELEM_MATERIAL_UNK1,
        { 0x00000000, 0x00, 0x00 },
        { 0xF7CFFFFF, 0x00, 0x00 },
        ATELEM_NONE | ATELEM_SFX_NORMAL,
        ACELEM_ON,
        OCELEM_ON,
    },
    { 18, 64, 0, { 0, 0, 0 } },
};

static CollisionCheckInfoInit2 sColChkInfoInit = { 0, 0, 0, 0, MASS_IMMOVABLE };

static DamageTable sDamageTable = {
    /* Deku Nut       */ DMG_ENTRY(1, 0x0),
    /* Deku Stick     */ DMG_ENTRY(1, 0x0),
    /* Horse trample  */ DMG_ENTRY(1, 0x0),
    /* Explosives     */ DMG_ENTRY(1, 0x0),
    /* Zora boomerang */ DMG_ENTRY(1, 0x0),
    /* Normal arrow   */ DMG_ENTRY(1, 0x0),
    /* UNK_DMG_0x06   */ DMG_ENTRY(1, 0x0),
    /* Hookshot       */ DMG_ENTRY(1, 0x0),
    /* Goron punch    */ DMG_ENTRY(1, 0x0),
    /* Sword          */ DMG_ENTRY(1, 0x0),
    /* Goron pound    */ DMG_ENTRY(1, 0x0),
    /* Fire arrow     */ DMG_ENTRY(1, 0x0),
    /* Ice arrow      */ DMG_ENTRY(1, 0x0),
    /* Light arrow    */ DMG_ENTRY(1, 0x0),
    /* Goron spikes   */ DMG_ENTRY(1, 0x0),
    /* Deku spin      */ DMG_ENTRY(1, 0x0),
    /* Deku bubble    */ DMG_ENTRY(1, 0x0),
    /* Deku launch    */ DMG_ENTRY(1, 0x0),
    /* UNK_DMG_0x12   */ DMG_ENTRY(1, 0x0),
    /* Zora barrier   */ DMG_ENTRY(1, 0x0),
    /* Normal shield  */ DMG_ENTRY(1, 0x0),
    /* Light ray      */ DMG_ENTRY(1, 0x0),
    /* Thrown object  */ DMG_ENTRY(1, 0x0),
    /* Zora punch     */ DMG_ENTRY(1, 0x0),
    /* Spin attack    */ DMG_ENTRY(1, 0x0),
    /* Sword beam     */ DMG_ENTRY(1, 0x0),
    /* Normal Roll    */ DMG_ENTRY(1, 0x0),
    /* UNK_DMG_0x1B   */ DMG_ENTRY(1, 0x0),
    /* UNK_DMG_0x1C   */ DMG_ENTRY(1, 0x0),
    /* Unblockable    */ DMG_ENTRY(1, 0x0),
    /* UNK_DMG_0x1E   */ DMG_ENTRY(1, 0x0),
    /* Powder Keg     */ DMG_ENTRY(1, 0x0),
};

static u8 sSchedule[] = {
    /* 0x00 */ SCHEDULE_CMD_CHECK_NOT_IN_DAY_S(1, 0x1D - 0x04),
    /* 0x04 */ SCHEDULE_CMD_CHECK_NOT_IN_SCENE_S(SCENE_BACKTOWN, 0x1C - 0x08),
    /* 0x08 */ SCHEDULE_CMD_CHECK_TIME_RANGE_S(0, 0, 0, 30, 0x16 - 0x0E),
    /* 0x0E */ SCHEDULE_CMD_CHECK_BEFORE_TIME_S(0, 30, 0x15 - 0x12),
    /* 0x12 */ SCHEDULE_CMD_RET_VAL_L(BOMB_SHOP_LADY_SCH_KNOCKED_OVER),
    /* 0x15 */ SCHEDULE_CMD_RET_NONE(),
    /* 0x16 */ SCHEDULE_CMD_RET_TIME(0, 0, 0, 30, BOMB_SHOP_LADY_SCH_FOLLOW_TIME_PATH),
    /* 0x1C */ SCHEDULE_CMD_RET_NONE(),
    /* 0x1D */ SCHEDULE_CMD_RET_NONE(),
};

static s32 sSearchTimePathLimit[] = { -1, -1, 0 };

static TrackOptionsSet sTrackOptions = {
    { 0xFA0, 4, 1, 3 },
    { 0x1770, 4, 1, 6 },
    { 0xFA0, 4, 1, 3 },
    { 0x1770, 4, 1, 6 },
};

s32 EnBaba_FindBombShopkeeper(EnBaba* this, PlayState* play) {
    //! The bomb shopkeeper is an EnSob1, but initalizes itself with id `ACTOR_EN_OSSAN`
    //! Note if there are other `EnOssan` actors, it may find that instance instead
    //! in which case `EnSob1` struct acceses would be incorrect
    this->bombShopkeeper = (EnSob1*)SubS_FindActor(play, &this->bombShopkeeper->actor, ACTORCAT_NPC, ACTOR_EN_OSSAN);

    if (this->bombShopkeeper != NULL) {
        return true;
    }
    return false;
}

void EnBaba_HandleConversation(EnBaba* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    switch (this->textId) {
        case 0:
            if (this->stateFlags & BOMB_SHOP_LADY_STATE_AUTOTALK) {
                if (CHECK_WEEKEVENTREG(WEEKEVENTREG_RECOVERED_STOLEN_BOMB_BAG)) {
                    // Thanks. Can stock Bomb Bags tomorrow
                    this->textId = 0x2A34;
                    break;
                }

                if (CHECK_WEEKEVENTREG(WEEKEVENTREG_SAKON_DEAD)) {
                    this->stateFlags |= BOMB_SHOP_LADY_STATE_END_CONVERSATION;
                    // Oh my, learned my lesson. Can't stock Bomb Bags tomorrow
                    this->textId = 0x2A33;
                    break;
                }

                this->stateFlags |= BOMB_SHOP_LADY_STATE_END_CONVERSATION;
                // Can't stock Bomb Bags tomorrow
                this->textId = 0x2A32;
                break;
            }

            if (player->transformation == PLAYER_FORM_DEKU) {
                if (!CHECK_WEEKEVENTREG(WEEKEVENTREG_79_20)) {
                    SET_WEEKEVENTREG(WEEKEVENTREG_79_20);
                    this->stateFlags |= BOMB_SHOP_LADY_STATE_END_CONVERSATION;
                    // Small customer, use bombs as adult
                    this->textId = 0x2A37;
                    break;
                } else {
                    this->stateFlags |= BOMB_SHOP_LADY_STATE_END_CONVERSATION;
                    // use bombs as adult
                    this->textId = 0x2A38;
                }
                break;
            }

            if (!CHECK_WEEKEVENTREG(WEEKEVENTREG_RECOVERED_STOLEN_BOMB_BAG)) {
                if (!CHECK_WEEKEVENTREG(WEEKEVENTREG_73_01)) {
                    // Thought could sell Big Bomb Bags
                    this->textId = 0x660;
                    break;
                }
                // Can't judge people
                this->textId = 0x662;
                break;
            }

            if (!CHECK_WEEKEVENTREG(WEEKEVENTREG_73_02)) {
                // Someone helped me out
                this->textId = 0x65A;
                break;
            }

            // Buy Big Bomb Bag
            this->textId = 0x65E;
            break;

        case 0x660:
            Actor_ChangeFocus(&this->actor, play, &this->bombShopkeeper->actor);
            // Don't go by yourself
            this->textId = 0x661;
            break;

        case 0x661:
            Actor_ChangeFocus(&this->bombShopkeeper->actor, play, &this->actor);
            // Can't judge people
            this->textId = 0x662;
            break;

        case 0x662:
            Actor_ChangeFocus(&this->actor, play, &this->bombShopkeeper->actor);
            // I'll go next time
            this->textId = 0x663;
            SET_WEEKEVENTREG(WEEKEVENTREG_73_01);
            this->stateFlags |= BOMB_SHOP_LADY_STATE_END_CONVERSATION;
            break;

        case 0x65A:
            Actor_ChangeFocus(&this->actor, play, &this->bombShopkeeper->actor);
            // Don't pick up Bomb bags at night
            this->textId = 0x65B;
            break;

        case 0x65B:
            Actor_ChangeFocus(&this->bombShopkeeper->actor, play, &this->actor);
            // Lifelong dream to sell Big Bomb Bags
            this->textId = 0x65C;
            break;

        case 0x65C:
            Actor_ChangeFocus(&this->actor, play, &this->bombShopkeeper->actor);
            // I worry about you
            this->textId = 0x65D;
            SET_WEEKEVENTREG(WEEKEVENTREG_73_02);
            this->stateFlags |= BOMB_SHOP_LADY_STATE_END_CONVERSATION;
            break;

        case 0x65E:
            Actor_ChangeFocus(&this->actor, play, &this->bombShopkeeper->actor);
            // I worry about you
            this->textId = 0x65F;
            this->stateFlags |= BOMB_SHOP_LADY_STATE_END_CONVERSATION;
            break;

        case 0x2A34:
            if (GameInteractor_Should(VB_HAVE_BLAST_MASK, INV_CONTENT(ITEM_MASK_BLAST) == ITEM_MASK_BLAST)) {
                this->stateFlags |= BOMB_SHOP_LADY_STATE_END_CONVERSATION;
                // Thank you
                this->textId = 0x2A36;
                break;
            }
            this->stateFlags |= BOMB_SHOP_LADY_STATE_GIVE_BLAST_MASK;
            // It's a dangerous mask
            this->textId = 0x2A35;
            break;

        case 0x2A35:
            this->stateFlags |= BOMB_SHOP_LADY_STATE_END_CONVERSATION;
            // Thank you
            this->textId = 0x2A36;
            break;

        case 0x2A30:
        case 0x2A31:
            this->stateFlags |= BOMB_SHOP_LADY_STATE_END_CONVERSATION;
            break;

        default:
            break;
    }

    Message_StartTextbox(play, this->textId, &this->actor);
    if (this->stateFlags & BOMB_SHOP_LADY_STATE_END_CONVERSATION) {
        if (this->stateFlags & BOMB_SHOP_LADY_STATE_GAVE_BLAST_MASK) {
            this->stateFlags &= ~BOMB_SHOP_LADY_STATE_GAVE_BLAST_MASK;
            Message_BombersNotebookQueueEvent(play, BOMBERS_NOTEBOOK_EVENT_RECEIVED_BLAST_MASK);
        }
        Message_BombersNotebookQueueEvent(play, BOMBERS_NOTEBOOK_EVENT_MET_BOMB_SHOP_LADY);
    }
}

void EnBaba_TriggerTransition(PlayState* play, u16 nextEntrance) {
    play->nextEntrance = nextEntrance;
    play->transitionType = TRANS_TYPE_64;
    gSaveContext.nextTransitionType = TRANS_TYPE_64;
    play->transitionTrigger = TRANS_TRIGGER_START;
}

void EnBaba_UpdateCollider(EnBaba* this, PlayState* play) {
    this->collider.dim.pos.x = this->actor.world.pos.x + 5.0f;
    this->collider.dim.pos.y = this->actor.world.pos.y;
    this->collider.dim.pos.z = this->actor.world.pos.z + 22.0f;

    CollisionCheck_SetAC(play, &play->colChkCtx, &this->collider.base);
    CollisionCheck_SetOC(play, &play->colChkCtx, &this->collider.base);
}

s32 EnBaba_MoveForward(EnBaba* this, f32 speedTarget) {
    s16 rotStep;
    s32 reachedEnd = false;
    Vec3f point;

    Math_SmoothStepToF(&this->actor.speed, speedTarget, 0.4f, 1000.0f, 0.0f);
    rotStep = this->actor.speed * 400.0f;
    if (SubS_CopyPointFromPath(this->path, this->waypoint, &point) &&
        SubS_MoveActorToPoint(&this->actor, &point, rotStep)) {
        this->waypoint++;
        if (this->waypoint >= this->path->count) {
            reachedEnd = true;
        }
    }
    return reachedEnd;
}

void EnBaba_UpdateModel(EnBaba* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    Vec3f point;

    SkelAnime_Update(&this->skelAnime);

    if (SubS_AngleDiffLessEqual(this->actor.shape.rot.y, 0x36B0, this->actor.yawTowardsPlayer) &&
        !(this->stateFlags & BOMB_SHOP_LADY_STATE_KNOCKED_OVER)) {
        point.x = player->actor.world.pos.x;
        point.y = player->bodyPartsPos[PLAYER_BODYPART_HEAD].y + 3.0f;
        point.z = player->actor.world.pos.z;

        SubS_TrackPoint(&point, &this->actor.focus.pos, &this->actor.shape.rot, &this->trackTarget, &this->headRot,
                        &this->torsoRot, &sTrackOptions);
    } else {
        Math_SmoothStepToS(&this->trackTarget.x, 0, 4, 0x3E8, 1);
        Math_SmoothStepToS(&this->trackTarget.y, 0, 4, 0x3E8, 1);
        Math_SmoothStepToS(&this->headRot.x, 0, 4, 0x3E8, 1);
        Math_SmoothStepToS(&this->headRot.y, 0, 4, 0x3E8, 1);
        Math_SmoothStepToS(&this->torsoRot.x, 0, 4, 0x3E8, 1);
        Math_SmoothStepToS(&this->torsoRot.y, 0, 4, 0x3E8, 1);
    }

    SubS_UpdateFidgetTables(play, this->fidgetTableY, this->fidgetTableZ, BOMB_SHOP_LADY_LIMB_MAX);

    if (this->stateFlags & BOMB_SHOP_LADY_STATE_VISIBLE) {
        EnBaba_UpdateCollider(this, play);
    }
}

s32 EnBaba_InitTimePath(EnBaba* this, PlayState* play, ScheduleOutput* scheduleOutput) {
    u16 now = SCRIPT_TIME_NOW;
    u16 startTime;
    u8 pathIndex = BOMB_SHOP_LADY_GET_PATH_INDEX(&this->actor);
    u16 numWaypoints;

    if (sSearchTimePathLimit[scheduleOutput->result] >= 0) {
        this->timePath = SubS_GetAdditionalPath(play, pathIndex, sSearchTimePathLimit[scheduleOutput->result]);
    }

    if (this->timePath == NULL) {
        return false;
    }

    if ((this->scheduleResult != BOMB_SHOP_LADY_SCH_NONE) && (this->timePathTimeSpeed >= 0)) {
        startTime = now;
    } else {
        startTime = scheduleOutput->time0;
    }

    if (scheduleOutput->time1 < startTime) {
        this->timePathTotalTime = (startTime - scheduleOutput->time1) + (DAY_LENGTH - 1);
    } else {
        this->timePathTotalTime = scheduleOutput->time1 - startTime;
    }

    this->timePathElapsedTime = now - startTime;

    numWaypoints = startTime = this->timePath->count - (SUBS_TIME_PATHING_ORDER - 1);
    this->timePathWaypointTime = this->timePathTotalTime / numWaypoints;

    this->timePathIsSetup = false;
    this->timePathWaypoint = (this->timePathElapsedTime / this->timePathWaypointTime) + (SUBS_TIME_PATHING_ORDER - 1);
    this->timePathHasReachedEnd = false;

    return true;
}

s32 EnBaba_ProcessScheduleOutput(EnBaba* this, PlayState* play, ScheduleOutput* scheduleOutput) {
    s32 success;

    switch (scheduleOutput->result) {
        case BOMB_SHOP_LADY_SCH_FOLLOW_TIME_PATH:
            success = EnBaba_InitTimePath(this, play, scheduleOutput);
            break;

        case BOMB_SHOP_LADY_SCH_KNOCKED_OVER:
            success = true;
            break;

        default:
            success = false;
            break;
    }

    return success;
}

s32 EnBaba_FollowTimePath(EnBaba* this, PlayState* play) {
    f32 weightArray[265];
    Vec3f worldPos;
    Vec3f timePathPointNew; // used to compute yaw after update
    Vec3f timePathPoint;    // used in setup and to store a backup before update
    s32 prevTimePathElapsedTime = 0;
    s32 prevTimePathWaypoint = 0;
    s32 pad;

    SubS_TimePathing_FillKnots(weightArray, SUBS_TIME_PATHING_ORDER, this->timePath->count + SUBS_TIME_PATHING_ORDER);

    if (!this->timePathIsSetup) {
        timePathPoint = gZeroVec3f;

        SubS_TimePathing_Update(this->timePath, &this->timePathProgress, &this->timePathElapsedTime,
                                this->timePathWaypointTime, this->timePathTotalTime, &this->timePathWaypoint,
                                weightArray, &timePathPoint, this->timePathTimeSpeed);
        SubS_TimePathing_ComputeInitialY(play, this->timePath, this->timePathWaypoint, &timePathPoint);
        this->actor.world.pos.y = timePathPoint.y;
        this->timePathIsSetup = true;
    } else {
        timePathPoint = this->timePathPoint;
    }

    this->actor.world.pos.x = timePathPoint.x;
    this->actor.world.pos.z = timePathPoint.z;

    if (SubS_InCsMode(play)) {
        prevTimePathElapsedTime = this->timePathElapsedTime;
        prevTimePathWaypoint = this->timePathWaypoint;
        timePathPoint = this->actor.world.pos;
    }

    this->timePathPoint = gZeroVec3f;

    if (SubS_TimePathing_Update(this->timePath, &this->timePathProgress, &this->timePathElapsedTime,
                                this->timePathWaypointTime, this->timePathTotalTime, &this->timePathWaypoint,
                                weightArray, &this->timePathPoint, this->timePathTimeSpeed)) {
        this->timePathHasReachedEnd = true;
    } else {
        worldPos = this->actor.world.pos;
        timePathPointNew = this->timePathPoint;
        this->actor.world.rot.y = Math_Vec3f_Yaw(&worldPos, &timePathPointNew);
    }

    if (SubS_InCsMode(play)) {
        this->timePathElapsedTime = prevTimePathElapsedTime;
        this->timePathWaypoint = prevTimePathWaypoint;
        this->timePathPoint = timePathPoint;
    }

    return false;
}

void EnBaba_HandleSchedule(EnBaba* this, PlayState* play) {
    switch (this->scheduleResult) {
        case BOMB_SHOP_LADY_SCH_FOLLOW_TIME_PATH:
            SET_WEEKEVENTREG(WEEKEVENTREG_58_40);
            this->stateFlags |= BOMB_SHOP_LADY_STATE_VISIBLE;
            EnBaba_FollowTimePath(this, play);
            break;

        case BOMB_SHOP_LADY_SCH_KNOCKED_OVER:
            this->animIndex = BOMB_SHOP_LADY_ANIM_KNOCKED_OVER;
            // Ouch
            this->textId = 0x2A30;
            this->actor.speed = 0.0f;
            Enemy_StartFinishingBlow(play, &this->actor);
            this->stateFlags |= BOMB_SHOP_LADY_STATE_KNOCKED_OVER;
            Actor_ChangeAnimationByInfo(&this->skelAnime, sAnimationInfo, this->animIndex);
            this->actionFunc = EnBaba_KnockedOver;
            break;

        default:
            break;
    }
    Math_ApproachS(&this->actor.shape.rot.y, this->actor.world.rot.y, 4, 0x1554);
}

void EnBaba_FinishInit(EnBaba* this, PlayState* play) {
    SkelAnime_InitFlex(play, &this->skelAnime, &gBombShopLadySkel, &gBombShopLadyWalkingHoldingBagAnim,
                       this->jointTable, this->morphTable, BOMB_SHOP_LADY_LIMB_MAX);

    this->actor.draw = EnBaba_Draw;
    this->stateFlags |= BOMB_SHOP_LADY_STATE_DRAW_SHADOW;
    this->actor.flags |= ACTOR_FLAG_ATTENTION_ENABLED;

    if (play->sceneId == SCENE_BOMYA) {
        this->stateFlags |= BOMB_SHOP_LADY_STATE_VISIBLE;
        this->animIndex = BOMB_SHOP_LADY_ANIM_IDLE;
        Actor_ChangeAnimationByInfo(&this->skelAnime, sAnimationInfo, this->animIndex);
        this->actionFunc = EnBaba_Idle;
    } else if (play->sceneId == SCENE_BACKTOWN) {
        if ((BOMB_SHOP_LADY_GET_TYPE(&this->actor) == BOMB_SHOP_LADY_TYPE_FOLLOW_SCHEDULE) &&
            (gSaveContext.save.entrance != ENTRANCE(NORTH_CLOCK_TOWN, 7)) &&
            (BOMB_SHOP_LADY_GET_PATH_INDEX(&this->actor) != BOMB_SHOP_LADY_PATH_INDEX_NONE)) {
            if (CHECK_WEEKEVENTREG(WEEKEVENTREG_58_40) ||
                ((CURRENT_TIME >= CLOCK_TIME(0, 20)) && (CURRENT_TIME < CLOCK_TIME(6, 0)))) {
                Actor_Kill(&this->actor);
                return;
            }

            this->sakonDeadTimer = 50;
            this->animIndex = BOMB_SHOP_LADY_ANIM_WALKING_HOLDING_BAG;
            Actor_ChangeAnimationByInfo(&this->skelAnime, sAnimationInfo, this->animIndex);
            this->actionFunc = EnBaba_FollowSchedule;
        } else if ((BOMB_SHOP_LADY_GET_TYPE(&this->actor) == BOMB_SHOP_LADY_TYPE_IDLE) &&
                   (gSaveContext.save.entrance == ENTRANCE(NORTH_CLOCK_TOWN, 7))) {
            if (CHECK_WEEKEVENTREG(WEEKEVENTREG_81_02)) {
                Actor_Kill(&this->actor);
                return;
            }

            this->stateFlags |= BOMB_SHOP_LADY_STATE_VISIBLE;
            if (CHECK_WEEKEVENTREG(WEEKEVENTREG_RECOVERED_STOLEN_BOMB_BAG)) {
                this->animIndex = BOMB_SHOP_LADY_ANIM_IDLE_HOLDING_BAG;
            } else {
                this->animIndex = BOMB_SHOP_LADY_ANIM_IDLE;
            }

            Actor_ChangeAnimationByInfo(&this->skelAnime, sAnimationInfo, this->animIndex);
            this->stateFlags |= BOMB_SHOP_LADY_STATE_AUTOTALK;
            this->actionFunc = EnBaba_Idle;
        } else {
            Actor_Kill(&this->actor);
            return;
        }
    } else {
        this->stateFlags |= BOMB_SHOP_LADY_STATE_VISIBLE;
        if (BOMB_SHOP_LADY_GET_TYPE(&this->actor) == BOMB_SHOP_LADY_TYPE_SWAY) {
            this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
            this->animIndex = BOMB_SHOP_LADY_ANIM_SWAY;
            Actor_ChangeAnimationByInfo(&this->skelAnime, sAnimationInfo, this->animIndex);
            this->actionFunc = EnBaba_DoNothing;
        } else if (BOMB_SHOP_LADY_GET_PATH_INDEX(&this->actor) != BOMB_SHOP_LADY_PATH_INDEX_NONE) {
            this->animIndex = BOMB_SHOP_LADY_ANIM_WALKING_HOLDING_BAG;
            Actor_ChangeAnimationByInfo(&this->skelAnime, sAnimationInfo, this->animIndex);
            this->actionFunc = EnBaba_Walk;
        } else {
            this->animIndex = BOMB_SHOP_LADY_ANIM_IDLE_HOLDING_BAG;
            Actor_ChangeAnimationByInfo(&this->skelAnime, sAnimationInfo, this->animIndex);
            this->actionFunc = EnBaba_FaceForward;
        }
    }
}

void EnBaba_Idle(EnBaba* this, PlayState* play) {
    if ((this->stateFlags & BOMB_SHOP_LADY_STATE_AUTOTALK) || (this->bombShopkeeper != NULL) ||
        EnBaba_FindBombShopkeeper(this, play)) {
        if (Actor_TalkOfferAccepted(&this->actor, &play->state)) {
            EnBaba_HandleConversation(this, play);
            if (this->stateFlags & BOMB_SHOP_LADY_STATE_AUTOTALK) {
                this->actor.flags &= ~ACTOR_FLAG_TALK_OFFER_AUTO_ACCEPTED;
            }
            this->actionFunc = EnBaba_Talk;
        } else if (this->actor.xzDistToPlayer < 100.0f) {
            if (this->stateFlags & BOMB_SHOP_LADY_STATE_AUTOTALK) {
                this->actor.flags |= ACTOR_FLAG_TALK_OFFER_AUTO_ACCEPTED;
            }
            Actor_OfferTalk(&this->actor, play, 100.0f);
        }
    }
}

void EnBaba_FollowSchedule_Talk(EnBaba* this, PlayState* play) {
    u8 talkState = Message_GetState(&play->msgCtx);

    if (((talkState == TEXT_STATE_EVENT) || (talkState == TEXT_STATE_DONE)) && Message_ShouldAdvance(play)) {
        play->msgCtx.msgMode = MSGMODE_TEXT_CLOSING;
        play->msgCtx.stateTimer = 4;
        this->actionFunc = EnBaba_FollowSchedule;
    }
    Math_SmoothStepToS(&this->actor.shape.rot.y, this->actor.yawTowardsPlayer, 5, 0x1000, 0x100);
}

void EnBaba_Talk(EnBaba* this, PlayState* play) {
    u8 talkState = Message_GetState(&play->msgCtx);

    if (talkState == TEXT_STATE_EVENT) {
        if (Message_ShouldAdvance(play)) {
            if (this->stateFlags & BOMB_SHOP_LADY_STATE_END_CONVERSATION) {
                this->stateFlags &= ~BOMB_SHOP_LADY_STATE_END_CONVERSATION;
                play->msgCtx.msgMode = MSGMODE_TEXT_CLOSING;
                play->msgCtx.stateTimer = 4;
                if (this->stateFlags & BOMB_SHOP_LADY_STATE_AUTOTALK) {
                    if (CHECK_QUEST_ITEM(QUEST_BOMBERS_NOTEBOOK)) {
                        if (play->msgCtx.bombersNotebookEventQueueCount == 0) {
                            SET_WEEKEVENTREG(WEEKEVENTREG_81_02);
                            EnBaba_TriggerTransition(play, ENTRANCE(NORTH_CLOCK_TOWN, 7));
                            return;
                        }
                    } else {
                        SET_WEEKEVENTREG(WEEKEVENTREG_81_02);
                        EnBaba_TriggerTransition(play, ENTRANCE(NORTH_CLOCK_TOWN, 7));
                    }
                } else {
                    this->textId = 0;
                    this->actionFunc = EnBaba_Idle;
                }
            } else if (this->stateFlags & BOMB_SHOP_LADY_STATE_GIVE_BLAST_MASK) {
                this->stateFlags &= ~BOMB_SHOP_LADY_STATE_GIVE_BLAST_MASK;
                play->msgCtx.msgMode = MSGMODE_TEXT_CLOSING;
                play->msgCtx.stateTimer = 4;
                this->actionFunc = EnBaba_GiveBlastMask;
            } else {
                EnBaba_HandleConversation(this, play);
            }
        }
    } else if (talkState == TEXT_STATE_DONE) {
        if (Message_ShouldAdvance(play) && (play->msgCtx.bombersNotebookEventQueueCount == 0)) {
            SET_WEEKEVENTREG(WEEKEVENTREG_81_02);
            EnBaba_TriggerTransition(play, ENTRANCE(NORTH_CLOCK_TOWN, 7));
        }
    }
}

void EnBaba_GiveBlastMask(EnBaba* this, PlayState* play) {
    if (Actor_HasParent(&this->actor, play)) {
        this->actor.parent = NULL;
        this->stateFlags |= BOMB_SHOP_LADY_STATE_GAVE_BLAST_MASK;
        this->actionFunc = EnBaba_GaveBlastMask;
    } else {
        Actor_OfferGetItem(&this->actor, play, GI_MASK_BLAST, 300.0f, 300.0f);
    }
}

void EnBaba_GaveBlastMask(EnBaba* this, PlayState* play) {
    if (Actor_TalkOfferAccepted(&this->actor, &play->state)) {
        EnBaba_HandleConversation(this, play);
        this->actionFunc = EnBaba_Talk;
    } else {
        Actor_OfferTalkExchangeEquiCylinder(&this->actor, play, 400.0f, PLAYER_IA_MINUS1);
    }
}

void EnBaba_FollowSchedule(EnBaba* this, PlayState* play) {
    ScheduleOutput scheduleOutput;

    this->timePathTimeSpeed = R_TIME_SPEED + ((void)0, gSaveContext.save.timeSpeedOffset);

    if (!Schedule_RunScript(play, sSchedule, &scheduleOutput) ||
        ((this->scheduleResult != scheduleOutput.result) &&
         !EnBaba_ProcessScheduleOutput(this, play, &scheduleOutput))) {
        this->stateFlags &= ~BOMB_SHOP_LADY_STATE_DRAW_SHADOW;
        this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
        scheduleOutput.result = BOMB_SHOP_LADY_SCH_NONE;
    } else {
        this->stateFlags |= BOMB_SHOP_LADY_STATE_DRAW_SHADOW;
        this->actor.flags |= ACTOR_FLAG_ATTENTION_ENABLED;
    }
    this->scheduleResult = scheduleOutput.result;

    EnBaba_HandleSchedule(this, play);

    if (this->stateFlags & BOMB_SHOP_LADY_STATE_VISIBLE) {
        if (Actor_TalkOfferAccepted(&this->actor, &play->state)) {
            Message_StartTextbox(play, 0x2A39, &this->actor); // "I'm sorry"
            this->actionFunc = EnBaba_FollowSchedule_Talk;
        } else if ((this->actor.xzDistToPlayer < 100.0f) || this->actor.isLockedOn) {
            Actor_OfferTalkNearColChkInfoCylinder(&this->actor, play);
        }
    }
    Actor_MoveWithGravity(&this->actor);
}

void EnBaba_KnockedOver(EnBaba* this, PlayState* play) {
    s16 curFrame = this->skelAnime.curFrame;
    s16 endFrame = Animation_GetLastFrame(sAnimationInfo[this->animIndex].animation);

    this->collider.dim.height = 37;
    this->collider.dim.radius = 23;

    if (this->animIndex == BOMB_SHOP_LADY_ANIM_KNOCKED_OVER) {
        if (Animation_OnFrame(&this->skelAnime, 0.0f)) {
            Actor_PlaySfx(&this->actor, NA_SE_VO_BBVO00);
        }

        if (curFrame == endFrame) {
            this->animIndex = BOMB_SHOP_LADY_ANIM_LYING_DOWN;
            Actor_ChangeAnimationByInfo(&this->skelAnime, sAnimationInfo, this->animIndex);
        }
    } else {
        if (CHECK_WEEKEVENTREG(WEEKEVENTREG_SAKON_DEAD) && (DECR(this->sakonDeadTimer) == 0)) {
            SEQCMD_STOP_SEQUENCE(SEQ_PLAYER_BGM_MAIN, 20);
            EnBaba_TriggerTransition(play, ENTRANCE(NORTH_CLOCK_TOWN, 7));
        } else {
            Actor_MoveWithGravity(&this->actor);
        }
    }
}

void EnBaba_DoNothing(EnBaba* this, PlayState* play) {
}

void EnBaba_Walk(EnBaba* this, PlayState* play) {
    if (EnBaba_MoveForward(this, 1.0f)) {
        this->waypoint = 0;
    }
}

void EnBaba_FaceForward(EnBaba* this, PlayState* play) {
    this->actor.shape.rot = this->actor.world.rot;
}

void EnBaba_Init(Actor* thisx, PlayState* play) {
    s32 pad;
    EnBaba* this = (EnBaba*)thisx;

    Collider_InitCylinder(play, &this->collider);
    Collider_SetCylinder(play, &this->collider, &this->actor, &sCylinderInit);
    CollisionCheck_SetInfo2(&this->actor.colChkInfo, &sDamageTable, &sColChkInfoInit);

    this->path = SubS_GetPathByIndex(play, BOMB_SHOP_LADY_GET_PATH_INDEX(&this->actor), BOMB_SHOP_LADY_PATH_INDEX_NONE);

    Actor_SetScale(&this->actor, 0.01f);

    this->actor.attentionRangeType = ATTENTION_RANGE_0;
    this->actor.gravity = -4.0f;
    this->actionFunc = EnBaba_FinishInit;
}

void EnBaba_Destroy(Actor* thisx, PlayState* play) {
    EnBaba* this = (EnBaba*)thisx;

    Collider_DestroyCylinder(play, &this->collider);
}

void EnBaba_Update(Actor* thisx, PlayState* play) {
    EnBaba* this = (EnBaba*)thisx;

    this->actionFunc(this, play);

    Actor_UpdateBgCheckInfo(play, &this->actor, 0.0f, 0.0f, 0.0f, UPDBGCHECKINFO_FLAG_4);
    EnBaba_UpdateModel(this, play);
}

s32 EnBaba_OverrideLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, Actor* thisx) {
    EnBaba* this = (EnBaba*)thisx;

    if (limbIndex == BOMB_SHOP_LADY_LIMB_NECK) {
        Matrix_Translate(1500.0f, 0.0f, 0.0f, MTXMODE_APPLY);
        Matrix_RotateXS(this->headRot.y, MTXMODE_APPLY);
        Matrix_RotateZS(-this->headRot.x, MTXMODE_APPLY);
        Matrix_Translate(-1500.0f, 0.0f, 0.0f, MTXMODE_APPLY);
    }

    if (limbIndex == BOMB_SHOP_LADY_LIMB_UPPER_ROOT) {
        Matrix_RotateXS(-this->torsoRot.y, MTXMODE_APPLY);
        Matrix_RotateZS(-this->torsoRot.x, MTXMODE_APPLY);
    }

    if ((limbIndex == BOMB_SHOP_LADY_LIMB_NECK) && this->msgFading && ((play->state.frames % 2) == 0)) {
        Matrix_Translate(40.0f, 0.0f, 0.0f, MTXMODE_APPLY);
    }

    if ((limbIndex == BOMB_SHOP_LADY_LIMB_UPPER_ROOT) || (limbIndex == BOMB_SHOP_LADY_LIMB_LEFT_UPPER_ARM) ||
        (limbIndex == BOMB_SHOP_LADY_LIMB_RIGHT_UPPER_ARM)) {
        rot->y += TRUNCF_BINANG(Math_SinS(this->fidgetTableY[limbIndex]) * 200.0f);
        rot->z += TRUNCF_BINANG(Math_CosS(this->fidgetTableZ[limbIndex]) * 200.0f);
    }

    if (((this->animIndex == BOMB_SHOP_LADY_ANIM_IDLE) || (this->animIndex == BOMB_SHOP_LADY_ANIM_KNOCKED_OVER) ||
         (this->animIndex == BOMB_SHOP_LADY_ANIM_LYING_DOWN) ||
         (BOMB_SHOP_LADY_GET_TYPE(&this->actor) == BOMB_SHOP_LADY_TYPE_SWAY)) &&
        (limbIndex == BOMB_SHOP_LADY_LIMB_BAG)) {
        *dList = NULL;
    }
    return false;
}

void EnBaba_PostLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot, Actor* thisx) {
    EnBaba* this = (EnBaba*)thisx;
    Vec3f sp18 = { 0.0f, 0.0f, 0.0f };

    if (limbIndex == BOMB_SHOP_LADY_LIMB_HEAD) {
        this->actor.focus.pos.x = this->actor.world.pos.x;
        this->actor.focus.pos.y = this->actor.world.pos.y;
        this->actor.focus.pos.z = this->actor.world.pos.z;
        Matrix_MultVec3f(&sp18, &this->actor.focus.pos);
    }
}

void EnBaba_TransformLimbDraw(PlayState* play, s32 limbIndex, Actor* thisx) {
}

void EnBaba_Draw(Actor* thisx, PlayState* play) {
    s32 pad;
    EnBaba* this = (EnBaba*)thisx;
    Vec3f pos;
    Vec3f scale;

    if (this->stateFlags & BOMB_SHOP_LADY_STATE_VISIBLE) {
        OPEN_DISPS(play->state.gfxCtx);

        Gfx_SetupDL37_Opa(play->state.gfxCtx);

        gSPSegment(POLY_OPA_DISP++, 0x08, Lib_SegmentedToVirtual(gBombShopLadyEyeTex));

        SkelAnime_DrawTransformFlexOpa(play, this->skelAnime.skeleton, this->skelAnime.jointTable,
                                       this->skelAnime.dListCount, EnBaba_OverrideLimbDraw, EnBaba_PostLimbDraw,
                                       EnBaba_TransformLimbDraw, &this->actor);

        if (this->stateFlags & BOMB_SHOP_LADY_STATE_DRAW_SHADOW) {
            if ((this->animIndex == BOMB_SHOP_LADY_ANIM_KNOCKED_OVER) ||
                (this->animIndex == BOMB_SHOP_LADY_ANIM_LYING_DOWN)) {
                Gfx_SetupDL25_Xlu(play->state.gfxCtx);
                pos.x = this->actor.world.pos.x + 20.0f;
                pos.y = this->actor.world.pos.y;
                pos.z = this->actor.world.pos.z + 20.0f;
            } else {
                pos = this->actor.world.pos;
            }

            scale.x = scale.y = scale.z = 0.3f;
            func_800BC620(&pos, &scale, 255, play);
        }

        CLOSE_DISPS(play->state.gfxCtx);
    }
}
