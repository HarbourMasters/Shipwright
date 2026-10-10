/*
 * File: z_en_ge2.c
 * Overlay: ovl_En_Ge2
 * Description: Purple-clad Gerudo Pirate
 */

#include "z_en_ge2.h"

#define FLAGS (ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_FRIENDLY | ACTOR_FLAG_MINIMAP_ICON_ENABLED)

void EnGe2_Init(Actor* thisx, PlayState* play);
void EnGe2_Destroy(Actor* thisx, PlayState* play);
void EnGe2_Update(Actor* thisx, PlayState* play);
void EnGe2_Draw(Actor* thisx, PlayState* play);

s32 EnGe2_SetupPath(EnGe2* this, PlayState* play);

void EnGe2_LookAround(EnGe2* this, PlayState* play);
void EnGe2_Walk(EnGe2* this, PlayState* play);
void EnGe2_GuardStationary(EnGe2* this, PlayState* play);

s32 EnGe2_ValidatePictograph(PlayState* play, Actor* thisx);

ActorProfile En_Ge2_Profile = {
    /**/ ACTOR_EN_GE2,
    /**/ ACTORCAT_NPC,
    /**/ FLAGS,
    /**/ OBJECT_GLA,
    /**/ sizeof(EnGe2),
    /**/ EnGe2_Init,
    /**/ EnGe2_Destroy,
    /**/ EnGe2_Update,
    /**/ EnGe2_Draw,
};

typedef enum {
    /* 0 */ GERUDO_PURPLE_DETECTION_UNDETECTED,
    /* 1 */ GERUDO_PURPLE_DETECTION_HEARD,
    /* 2 */ GERUDO_PURPLE_DETECTION_PROXIMITY //!< Higher priority
} GerudoPurpleDetection;

typedef enum {
    /* 0 */ GERUDO_PURPLE_PATHSTATUS_NORMAL,   //!< not near waypoint
    /* 1 */ GERUDO_PURPLE_PATHSTATUS_AT_POINT, //!< no path or new waypoint
    /* 2 */ GERUDO_PURPLE_PATHSTATUS_END       //!< reached end of path
} GerudoPurplePathStatus;

#define GERUDO_PURPLE_STATE_PATH_REVERSE (1 << 0) //!< Follow path backwards instead of forwards.
#define GERUDO_PURPLE_STATE_KO (1 << 1)
#define GERUDO_PURPLE_STATE_STUNNED (1 << 2) //!< Specifically by Deku Nuts
#define GERUDO_PURPLE_STATE_DISABLE_MOVEMENT \
    (1 << 3) //!< Disable normal movement to let pathing function control it completely.
#define GERUDO_PURPLE_STATE_CAPTURING (1 << 5) //!< Set but not used

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
        ELEM_MATERIAL_UNK0,
        { 0x00000000, 0x00, 0x00 },
        { 0x038BFBB3, 0x00, 0x00 },
        ATELEM_NONE | ATELEM_SFX_NORMAL,
        ACELEM_ON,
        OCELEM_ON,
    },
    { 30, 60, 0, { 0, 0, 0 } },
};

void EnGe2_Init(Actor* thisx, PlayState* play) {
    s32 pad;
    EnGe2* this = (EnGe2*)thisx;

    ActorShape_Init(&this->picto.actor.shape, 0.0f, ActorShadow_DrawCircle, 36.0f);
    SkelAnime_InitFlex(play, &this->skelAnime, &gGerudoPurpleSkel, NULL, this->jointTable, this->morphTable,
                       GERUDO_PURPLE_LIMB_MAX);
    Animation_PlayLoop(&this->skelAnime, &gGerudoPurpleWalkingAnim);

    Collider_InitAndSetCylinder(play, &this->collider, &this->picto.actor, &sCylinderInit);
    this->picto.actor.colChkInfo.mass = MASS_IMMOVABLE;
    Actor_SetScale(&this->picto.actor, 0.01f);
    this->picto.actor.cullingVolumeDistance = 1200.0f;

    if (this->picto.actor.world.rot.z == 0) {
        this->verticalDetectRange = 40.0f;
    } else {
        this->verticalDetectRange = fabsf(this->picto.actor.world.rot.z * 20.0f);
    }

    this->picto.actor.world.rot.x = this->picto.actor.shape.rot.x = 0;
    this->picto.actor.world.rot.z = this->picto.actor.shape.rot.z = 0;

    this->picto.actor.attentionRangeType = ATTENTION_RANGE_6;
    this->stateFlags = 0;
    this->detectedStatus = GERUDO_PURPLE_DETECTION_UNDETECTED;
    this->cueId = -1;

    this->picto.actor.terminalVelocity = -9.0f;
    this->picto.actor.gravity = -1.0f;
    this->picto.actor.speed = 1.5f;

    this->actionFunc = EnGe2_Walk;
    this->picto.validationFunc = EnGe2_ValidatePictograph;

    EnGe2_SetupPath(this, play);

    this->picto.actor.flags |= ACTOR_FLAG_UPDATE_CULLING_DISABLED;
    if (play->actorCtx.flags & ACTORCTX_FLAG_TELESCOPE_ON) {
        this->picto.actor.flags |= (ACTOR_FLAG_UPDATE_CULLING_DISABLED | ACTOR_FLAG_DRAW_CULLING_DISABLED);
    }

    switch (GERUDO_PURPLE_GET_TYPE(&this->picto.actor)) {
        case GERUDO_PURPLE_TYPE_BOAT_SENTRY:
            Animation_Change(&this->skelAnime, &gGerudoPurpleLookingAboutAnim, 1.0f, 0.0f,
                             Animation_GetLastFrame(&gGerudoPurpleLookingAboutAnim), ANIMMODE_LOOP, 0.0f);
            this->actionFunc = EnGe2_GuardStationary;
            this->picto.actor.speed = 0.0f;
            this->picto.actor.cullingVolumeDistance = 4000.0f;
            break;

        case GERUDO_PURPLE_TYPE_AVEIL_GUARD:
            if (CHECK_WEEKEVENTREG(WEEKEVENTREG_83_02)) {
                Actor_Kill(&this->picto.actor);
            }
            break;

        default:
            break;
    }
}

void EnGe2_Destroy(Actor* thisx, PlayState* play) {
}

// Detect Player through hearing or proximity
GerudoPurpleDetection EnGe2_DetectPlayer(PlayState* play, EnGe2* this) {
    if (this->picto.actor.xzDistToPlayer > 250.0f) {
        return GERUDO_PURPLE_DETECTION_UNDETECTED;
    } else if ((Player_GetMask(play) != PLAYER_MASK_STONE) && (this->picto.actor.xzDistToPlayer < 50.0f)) {
        return GERUDO_PURPLE_DETECTION_PROXIMITY;
    } else if (func_800B715C(play)) {
        return GERUDO_PURPLE_DETECTION_HEARD;
    } else {
        return GERUDO_PURPLE_DETECTION_UNDETECTED;
    }
}

/**
 * Spotted Player with line-of-sight
 *
 * @param play PlayState
 * @param thisx Actor to use for range and angle checks
 * @param pos Position to check from
 * @param yaw Center of yaw range to check
 * @param yawRange Spread of angles from `yaw` to check. If 0, do not check yaw
 * @param xzRange Horizontal range to check
 * @param yRange Vertical range to check
 * @return true if Player is visible with these settings
 */
s32 EnGe2_LookForPlayer(PlayState* play, Actor* actor, Vec3f* pos, s16 yaw, s16 yawRange, f32 xzRange, f32 yRange) {
    s16 yawToPlayer;
    Vec3f posResult;
    CollisionPoly* outPoly;
    Player* player = GET_PLAYER(play);

    if (Player_GetMask(play) == PLAYER_MASK_STONE) {
        return false;
    }
    if (actor->xzDistToPlayer > xzRange) {
        return false;
    }
    if (fabsf(actor->playerHeightRel) > yRange) {
        return false;
    }

    yawToPlayer = actor->yawTowardsPlayer - yaw;
    if ((yawRange > 0) && (yawRange < ABS_ALT(yawToPlayer))) {
        return false;
    }

    if (BgCheck_AnyLineTest1(&play->colCtx, pos, &player->bodyPartsPos[PLAYER_BODYPART_HEAD], &posResult, &outPoly,
                             false)) {
        return false;
    } else {
        return true;
    }
}

/* Path functions */

/**
 * Set up the path if it exists, choose the end and direction
 *
 * @return true if path is set up in reverse
 */
s32 EnGe2_SetupPath(EnGe2* this, PlayState* play) {
    if (GERUDO_PURPLE_GET_PATH_INDEX(&this->picto.actor) != GERUDO_PRUPLE_PATH_INDEX_NONE) {
        this->path = &play->setupPathList[GERUDO_PURPLE_GET_PATH_INDEX(&this->picto.actor)];
        if (this->path != NULL) {
            Path* path = this->path;
            Vec3s* points = Lib_SegmentedToVirtual(path->points);
            f32 diffX = points[0].x - this->picto.actor.world.pos.x;
            f32 diffZ = points[0].z - this->picto.actor.world.pos.z;

            if ((SQ(diffX) + SQ(diffZ)) < SQ(10.0f)) {
                this->curPointIndex = 0;
                this->stateFlags &= ~GERUDO_PURPLE_STATE_PATH_REVERSE;
            } else {
                this->curPointIndex = path->count - 1;
                this->stateFlags |= GERUDO_PURPLE_STATE_PATH_REVERSE;
                return true;
            }
        }
    } else {
        this->path = NULL;
        this->curPointIndex = 0;
    }
    return false;
}

void EnGe2_GetNextPath(EnGe2* this, PlayState* play) {
    Path* curPath;
    Path* nextPath;
    Vec3s* points;
    u8 nextPathIndex;

    this->curPointIndex = 0;

    if (GERUDO_PURPLE_GET_PATH_INDEX(&this->picto.actor) != GERUDO_PRUPLE_PATH_INDEX_NONE) {
        curPath = &play->setupPathList[GERUDO_PURPLE_GET_PATH_INDEX(&this->picto.actor)];
        nextPathIndex = curPath->additionalPathIndex;
        nextPath = &play->setupPathList[nextPathIndex];
        this->path = nextPath;
        points = Lib_SegmentedToVirtual(nextPath->points);
        this->picto.actor.world.pos.x = points[0].x;
        this->picto.actor.world.pos.z = points[0].z;
    } else {
        this->path = NULL;
    }
}

void EnGe2_SetupBlownAwayPath(EnGe2* this, PlayState* play) {
    s32 pad;
    Vec3s* points;
    Vec3f nextPoint;

    this->curPointIndex = 0;

    if (GERUDO_PURPLE_GET_PATH_INDEX(&this->picto.actor) != GERUDO_PRUPLE_PATH_INDEX_NONE) {
        this->path = &play->setupPathList[GERUDO_PURPLE_GET_PATH_INDEX(&this->picto.actor)];
        if (this->path != NULL) {
            points = Lib_SegmentedToVirtual(this->path->points);
            Math_Vec3s_ToVec3f(&this->picto.actor.world.pos, points);
            this->curPointIndex++;
            points++;
            Math_Vec3s_ToVec3f(&nextPoint, points);

            this->picto.actor.world.rot.y = Math_Vec3f_Yaw(&this->picto.actor.world.pos, &nextPoint);
            this->picto.actor.world.rot.x = Math_Vec3f_Pitch(&this->picto.actor.world.pos, &nextPoint);
            this->picto.actor.speed = 15.0f;
        }
    } else {
        this->path = NULL;
    }
}

GerudoPurplePathStatus EnGe2_FollowPath(EnGe2* this) {
    Path* path = this->path;
    Vec3s* curPoint;
    f32 diffX;
    f32 diffZ;

    if (path == NULL) {
        return GERUDO_PURPLE_PATHSTATUS_AT_POINT;
    }

    curPoint = (Vec3s*)Lib_SegmentedToVirtual(path->points);
    curPoint += this->curPointIndex;
    diffX = curPoint->x - this->picto.actor.world.pos.x;
    diffZ = curPoint->z - this->picto.actor.world.pos.z;
    this->picto.actor.world.rot.y = Math_Atan2S(diffX, diffZ);
    Math_SmoothStepToS(&this->picto.actor.shape.rot.y, this->picto.actor.world.rot.y, 2, 0x7D0, 0xC8);

    if ((SQ(diffX) + SQ(diffZ)) < SQ(10.0f)) {
        if (this->stateFlags & GERUDO_PURPLE_STATE_PATH_REVERSE) {
            this->curPointIndex--;
            if (this->curPointIndex < 0) {
                return GERUDO_PURPLE_PATHSTATUS_END;
            }
        } else {
            this->curPointIndex++;
            if (this->curPointIndex >= path->count) {
                return GERUDO_PURPLE_PATHSTATUS_END;
            }
        }
        return GERUDO_PURPLE_PATHSTATUS_AT_POINT;
    }

    return GERUDO_PURPLE_PATHSTATUS_NORMAL;
}

/**
 * Used for Blown Away cutscene path.
 */
s32 EnGe2_FollowPathWithoutGravity(EnGe2* this) {
    s32 pad;
    Vec3s* curPoint;
    Path* path = this->path;
    Vec3f point;
    s16 yawTarget;
    s16 pitchTarget;

    if (this->path == NULL) {
        return true;
    }

    curPoint = Lib_SegmentedToVirtual(path->points);
    curPoint += this->curPointIndex;

    Math_Vec3s_ToVec3f(&point, curPoint);
    yawTarget = Math_Vec3f_Yaw(&this->picto.actor.world.pos, &point);
    pitchTarget = Math_Vec3f_Pitch(&this->picto.actor.world.pos, &point);
    Math_SmoothStepToS(&this->picto.actor.world.rot.y, yawTarget, 0xA, 0x3E8, 0x64);
    Math_SmoothStepToS(&this->picto.actor.world.rot.x, pitchTarget, 6, 0x7D0, 0xC8);
    this->picto.actor.speed = 15.0f;

    Actor_MoveWithoutGravityReverse(&this->picto.actor);

    if (Math_Vec3f_DistXYZ(&this->picto.actor.world.pos, &point) < 40.0f) {
        return true;
    } else {
        return false;
    }
}

/* Action and helper functions */

void EnGe2_SpawnEffects(EnGe2* this, PlayState* play) {
    static Vec3f sEffectVelocity = { 0.0f, -0.05f, 0.0f };
    static Vec3f sEffectAccel = { 0.0f, -0.025f, 0.0f };
    static Color_RGBA8 sEffectPrimColor = { 255, 255, 255, 0 };
    static Color_RGBA8 sEffectEnvColor = { 255, 150, 0, 0 };
    s16 effectAngle = play->state.frames * 0x2800;
    Vec3f effectPos;

    effectPos.x = (Math_CosS(effectAngle) * 5.0f) + this->picto.actor.focus.pos.x;
    effectPos.y = this->picto.actor.focus.pos.y + 10.0f;
    effectPos.z = (Math_SinS(effectAngle) * 5.0f) + this->picto.actor.focus.pos.z;
    EffectSsKirakira_SpawnDispersed(play, &effectPos, &sEffectVelocity, &sEffectAccel, &sEffectPrimColor,
                                    &sEffectEnvColor, 1000, 16);
}

void EnGe2_Scream(EnGe2* this) {
    if ((s32)Rand_ZeroFloat(2.0f) == 0) {
        Actor_PlaySfx(&this->picto.actor, NA_SE_VO_FPVO00);
    } else {
        Actor_PlaySfx(&this->picto.actor, NA_SE_VO_FPVO01);
    }
}

/**
 * Wait for timer to finish, then set up the captured/thrown out transition.
 */
void EnGe2_ThrowPlayerOut(EnGe2* this, PlayState* play) {
    if (this->timer > 0) {
        this->timer--;
    } else if (play->nextEntrance != play->setupExitList[GERUDO_PURPLE_GET_EXIT(&this->picto.actor)]) {
        play->nextEntrance = play->setupExitList[GERUDO_PURPLE_GET_EXIT(&this->picto.actor)];
        play->transitionTrigger = TRANS_TRIGGER_START;
        play->transitionType = TRANS_TYPE_38;
    }
}

/**
 * Used if Aveil spots Player.
 */
void EnGe2_TurnToPlayerFast(EnGe2* this, PlayState* play) {
    SkelAnime_Update(&this->skelAnime);
    Math_SmoothStepToS(&this->picto.actor.shape.rot.y, this->picto.actor.yawTowardsPlayer, 2, 0x1000, 0x200);
}

void EnGe2_CapturePlayer(EnGe2* this, PlayState* play) {
    SkelAnime_Update(&this->skelAnime);
    Math_SmoothStepToS(&this->picto.actor.shape.rot.y, this->picto.actor.yawTowardsPlayer, 2, 0x400, 0x100);
    EnGe2_ThrowPlayerOut(this, play);
}

void EnGe2_SetupCapturePlayer(EnGe2* this) {
    this->picto.actor.speed = 0.0f;
    this->actionFunc = EnGe2_CapturePlayer;
    Animation_Change(&this->skelAnime, &gGerudoPurpleLookingAboutAnim, 1.0f, 0.0f,
                     Animation_GetLastFrame(&gGerudoPurpleLookingAboutAnim), ANIMMODE_LOOP, -8.0f);
}

void EnGe2_Charge(EnGe2* this, PlayState* play) {
    SkelAnime_Update(&this->skelAnime);
    Math_SmoothStepToS(&this->picto.actor.shape.rot.y, this->picto.actor.yawTowardsPlayer, 2, 0x400, 0x100);
    this->picto.actor.world.rot.y = this->picto.actor.shape.rot.y;

    if (this->picto.actor.xzDistToPlayer < 50.0f) {
        EnGe2_SetupCapturePlayer(this);
    } else if (!(this->picto.actor.bgCheckFlags & BGCHECKFLAG_GROUND)) {
        this->picto.actor.world.pos = this->picto.actor.prevPos;
        EnGe2_SetupCapturePlayer(this);
    }

    if (this->timer > 0) {
        this->timer--;
    } else {
        EnGe2_ThrowPlayerOut(this, play);
    }

    if (Animation_OnFrame(&this->skelAnime, 2.0f) || Animation_OnFrame(&this->skelAnime, 6.0f)) {
        Actor_PlaySfx(&this->picto.actor, NA_SE_EV_PIRATE_WALK);
    }
}

void EnGe2_SetupCharge(EnGe2* this, PlayState* play) {
    SkelAnime_Update(&this->skelAnime);
    Math_SmoothStepToS(&this->picto.actor.shape.rot.y, this->picto.actor.yawTowardsPlayer, 2, 0x400, 0x100);
    this->picto.actor.world.rot.y = this->picto.actor.shape.rot.y;

    if (this->picto.actor.shape.rot.y == this->picto.actor.yawTowardsPlayer) {
        Animation_Change(&this->skelAnime, &gGerudoPurpleChargingAnim, 1.0f, 0.0f,
                         Animation_GetLastFrame(&gGerudoPurpleChargingAnim), ANIMMODE_LOOP, -8.0f);
        this->timer = 50;
        this->picto.actor.speed = 4.0f;
        this->actionFunc = EnGe2_Charge;
    }
}

void EnGe2_SetupLookAround(EnGe2* this) {
    Animation_Change(&this->skelAnime, &gGerudoPurpleLookingAboutAnim, 1.0f, 0.0f,
                     Animation_GetLastFrame(&gGerudoPurpleLookingAboutAnim), ANIMMODE_LOOP, -8.0f);
    this->timer = 60;
    this->picto.actor.speed = 0.0f;
    this->actionFunc = EnGe2_LookAround;
}

void EnGe2_Stunned(EnGe2* this, PlayState* play) {
    if (this->picto.actor.colorFilterTimer == 0) {
        EnGe2_SetupLookAround(this);
        this->detectedStatus = GERUDO_PURPLE_DETECTION_UNDETECTED;
        CollisionCheck_SetAC(play, &play->colChkCtx, &this->collider.base);
        this->stateFlags &= ~GERUDO_PURPLE_STATE_STUNNED;
    }
}

void EnGe2_KnockedOut(EnGe2* this, PlayState* play) {
    SkelAnime_Update(&this->skelAnime);
    EnGe2_SpawnEffects(this, play);

    if (this->timer > 0) {
        this->timer--;
    } else {
        EnGe2_SetupLookAround(this);
        this->detectedStatus = GERUDO_PURPLE_DETECTION_UNDETECTED;
        CollisionCheck_SetAC(play, &play->colChkCtx, &this->collider.base);
        this->stateFlags &= ~GERUDO_PURPLE_STATE_KO;
        this->picto.actor.flags |= ACTOR_FLAG_ATTENTION_ENABLED;
    }
}

/**
 * Carries out various patrol functions:
 * - Turning to Player if being arrested
 * - Turning to Player if spotted by Aveil
 * - Looking for Player
 * - Managing collisions and collider
 */
void EnGe2_PatrolDuties(EnGe2* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    f32 visionRange = gSaveContext.save.isNight ? 200.0f : 280.0f;

    if (player->csAction == PLAYER_CSACTION_26) {
        this->picto.actor.speed = 0.0f;
        this->actionFunc = EnGe2_SetupCharge;
        Animation_Change(&this->skelAnime, &gGerudoPurpleLookingAboutAnim, 1.0f, 0.0f,
                         Animation_GetLastFrame(&gGerudoPurpleLookingAboutAnim), ANIMMODE_LOOP, -8.0f);
        this->stateFlags |= GERUDO_PURPLE_STATE_CAPTURING;
    } else if (CHECK_WEEKEVENTREG(WEEKEVENTREG_80_08)) {
        this->picto.actor.speed = 0.0f;
        this->actionFunc = EnGe2_TurnToPlayerFast;
        Animation_Change(&this->skelAnime, &gGerudoPurpleLookingAboutAnim, 1.0f, 0.0f,
                         Animation_GetLastFrame(&gGerudoPurpleLookingAboutAnim), ANIMMODE_LOOP, -8.0f);
    } else if (EnGe2_LookForPlayer(play, &this->picto.actor, &this->picto.actor.focus.pos,
                                   this->picto.actor.shape.rot.y, 0x1800, visionRange, this->verticalDetectRange)) {
        if ((GERUDO_PURPLE_GET_EXIT(&this->picto.actor) != GERUDO_PURPLE_EXIT_NONE) && !Play_InCsMode(play)) {
            this->picto.actor.speed = 0.0f;
            Player_SetCsActionWithHaltedActors(play, &this->picto.actor, PLAYER_CSACTION_26);
            Lib_PlaySfx(NA_SE_SY_FOUND);
            Message_StartTextbox(play, 0x1194, &this->picto.actor);
            this->actionFunc = EnGe2_SetupCharge;
            Animation_Change(&this->skelAnime, &gGerudoPurpleLookingAboutAnim, 1.0f, 0.0f,
                             Animation_GetLastFrame(&gGerudoPurpleLookingAboutAnim), ANIMMODE_LOOP, -8.0f);
        }
    } else if (this->collider.base.acFlags & AC_HIT) {
        if ((this->collider.elem.acHitElem != NULL) &&
            (this->collider.elem.acHitElem->atDmgInfo.dmgFlags & DMG_DEKU_NUT)) {
            Actor_SetColorFilter(&this->picto.actor, COLORFILTER_COLORFLAG_BLUE, 120, COLORFILTER_BUFFLAG_OPA, 400);
            this->picto.actor.speed = 0.0f;
            this->actionFunc = EnGe2_Stunned;
            this->stateFlags |= GERUDO_PURPLE_STATE_STUNNED;
        } else {
            Animation_Change(&this->skelAnime, &gGerudoPurpleFallingToGroundAnim, 1.0f, 0.0f,
                             Animation_GetLastFrame(&gGerudoPurpleFallingToGroundAnim), ANIMMODE_ONCE, -8.0f);
            this->timer = 200;
            this->picto.actor.speed = 0.0f;
            this->actionFunc = EnGe2_KnockedOut;
            Actor_PlaySfx(&this->picto.actor, NA_SE_EN_PIRATE_DEAD);
            this->picto.actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
            this->stateFlags |= GERUDO_PURPLE_STATE_KO;
        }
    } else if (this->picto.actor.home.rot.x == 0) {
        CollisionCheck_SetAC(play, &play->colChkCtx, &this->collider.base);
    }
}

void EnGe2_LookAround(EnGe2* this, PlayState* play) {
    u8 detectedStatus;

    SkelAnime_Update(&this->skelAnime);

    detectedStatus = EnGe2_DetectPlayer(play, this);
    if (detectedStatus != GERUDO_PURPLE_DETECTION_UNDETECTED) {
        this->timer = 100;
        this->yawTarget = this->picto.actor.yawTowardsPlayer;

        if (this->detectedStatus < detectedStatus) {
            this->detectedStatus = detectedStatus;
        }
    }

    if (this->timer > 0) {
        this->timer--;
    } else {
        this->actionFunc = EnGe2_Walk;
        Animation_Change(&this->skelAnime, &gGerudoPurpleWalkingAnim, 1.0f, 0.0f,
                         Animation_GetLastFrame(&gGerudoPurpleWalkingAnim), ANIMMODE_LOOP, -8.0f);
        this->headRot.y = 0;
        this->detectedStatus = GERUDO_PURPLE_DETECTION_UNDETECTED;
    }

    switch (this->detectedStatus) {
        case GERUDO_PURPLE_DETECTION_HEARD:
            Math_SmoothStepToS(&this->picto.actor.shape.rot.y, this->yawTarget, 2, 0x3E8, 0x1F4);
            break;

        case GERUDO_PURPLE_DETECTION_PROXIMITY:
            Math_SmoothStepToS(&this->picto.actor.shape.rot.y, this->yawTarget, 2, 0xBB8, 0x3E8);
            break;

        default:
            break;
    }

    this->picto.actor.world.rot.y = this->picto.actor.shape.rot.y;
    EnGe2_PatrolDuties(this, play);
}

void EnGe2_Walk(EnGe2* this, PlayState* play) {
    SkelAnime_Update(&this->skelAnime);
    this->picto.actor.speed = 1.5f;

    switch (EnGe2_FollowPath(this)) {
        case GERUDO_PURPLE_PATHSTATUS_END:
            EnGe2_SetupPath(this, play);
            break;

        case GERUDO_PURPLE_PATHSTATUS_AT_POINT:
            EnGe2_SetupLookAround(this);
            this->detectedStatus = GERUDO_PURPLE_DETECTION_UNDETECTED;
            break;

        default: // GERUDO_PURPLE_PATHSTATUS_NORMAL
            break;
    }

    this->detectedStatus = EnGe2_DetectPlayer(play, this);
    if (this->detectedStatus != GERUDO_PURPLE_DETECTION_UNDETECTED) {
        EnGe2_SetupLookAround(this);
        this->yawTarget = this->picto.actor.yawTowardsPlayer;
    }
    EnGe2_PatrolDuties(this, play);
}

void EnGe2_PerformCutsceneActions(EnGe2* this, PlayState* play) {
    SkelAnime_Update(&this->skelAnime);
    if (Cutscene_IsCueInChannel(play, CS_CMD_ACTOR_CUE_476)) {
        s16 cueId = play->csCtx.actorCues[Cutscene_GetCueChannel(play, CS_CMD_ACTOR_CUE_476)]->id;

        if (this->cueId != cueId) {
            this->cueId = cueId;

            switch (cueId) {
                case ENGE2_CUEID_BEEHIVE_PATROL:
                    Animation_Change(&this->skelAnime, &gGerudoPurpleLookingAboutAnim, 1.0f, 0.0f,
                                     Animation_GetLastFrame(&gGerudoPurpleLookingAboutAnim), ANIMMODE_LOOP, -8.0f);
                    EnGe2_GetNextPath(this, play);
                    break;

                case ENGE2_CUEID_BEEHIVE_RUN_AWAY:
                    Animation_Change(&this->skelAnime, &gGerudoPurpleRunningAwayCutsceneAnim, 1.0f, 0.0f,
                                     Animation_GetLastFrame(&gGerudoPurpleRunningAwayCutsceneAnim), ANIMMODE_LOOP,
                                     -5.0f);
                    this->screamTimer = (s32)(Rand_ZeroFloat(10.0f) + 20.0f);
                    break;

                case ENGE2_CUEID_BEEHIVE_EXIT:
                    Actor_Kill(&this->picto.actor);
                    break;

                case ENGE2_CUEID_GBT_ENTR_STAND_STILL:
                    Animation_Change(&this->skelAnime, &gGerudoPurpleGreatBayCutsceneAnim, 0.0f, 0.0f, 0.0f,
                                     ANIMMODE_ONCE, 0.0f);
                    break;

                case ENGE2_CUEID_GBT_ENTR_BLOWN_AWAY:
                    Animation_Change(&this->skelAnime, &gGerudoPurpleGreatBayCutsceneAnim, 0.0f, 1.0f, 1.0f,
                                     ANIMMODE_ONCE, 0.0f);
                    EnGe2_SetupBlownAwayPath(this, play);
                    this->stateFlags |= GERUDO_PURPLE_STATE_DISABLE_MOVEMENT;
                    this->screamTimer = (s32)(Rand_ZeroFloat(10.0f) + 20.0f);
                    break;

                default:
                    break;
            }
        }
    }

    switch (this->cueId) {
        case ENGE2_CUEID_BEEHIVE_RUN_AWAY:
            EnGe2_FollowPath(this);
            this->picto.actor.speed = 5.0f;

            if (Animation_OnFrame(&this->skelAnime, 2.0f) || Animation_OnFrame(&this->skelAnime, 6.0f)) {
                Actor_PlaySfx(&this->picto.actor, NA_SE_EV_PIRATE_WALK);
            }

            if (this->screamTimer > 0) {
                this->screamTimer--;
            } else {
                this->screamTimer = (s32)(Rand_ZeroFloat(10.0f) + 20.0f);
                EnGe2_Scream(this);
            }
            break;

        case ENGE2_CUEID_GBT_ENTR_BLOWN_AWAY:
            if ((this->curPointIndex < this->path->count) && EnGe2_FollowPathWithoutGravity(this)) {
                this->curPointIndex++;
            }

            // Tumble in the air
            this->picto.actor.shape.rot.x += 0x3E8;
            this->picto.actor.shape.rot.y += 0x7D0;
            this->picto.actor.shape.rot.z += 0x1F4;

            if (this->screamTimer > 0) {
                this->screamTimer--;
            } else {
                this->screamTimer = (s32)(Rand_ZeroFloat(10.0f) + 20.0f);
                EnGe2_Scream(this);
            }
            break;

        default:
            break;
    }
}

// Used for those on boats
void EnGe2_GuardStationary(EnGe2* this, PlayState* play) {
    SkelAnime_Update(&this->skelAnime);
    if (EnGe2_LookForPlayer(play, &this->picto.actor, &this->picto.actor.focus.pos, this->picto.actor.shape.rot.y,
                            0x4000, 720.0f, this->verticalDetectRange)) {
        if ((GERUDO_PURPLE_GET_EXIT(&this->picto.actor) != GERUDO_PURPLE_EXIT_NONE) && !Play_InCsMode(play)) {
            Player_SetCsActionWithHaltedActors(play, &this->picto.actor, PLAYER_CSACTION_26);
            Lib_PlaySfx(NA_SE_SY_FOUND);
            Message_StartTextbox(play, 0x1194, &this->picto.actor);
            this->timer = 50;
            EnGe2_SetupCapturePlayer(this);
        }
    }

    if (this->picto.actor.playerHeightRel < -150.0f) {
        this->picto.actor.draw = NULL;
    } else {
        this->picto.actor.draw = EnGe2_Draw;
    }
}

void EnGe2_Update(Actor* thisx, PlayState* play) {
    s32 pad;
    EnGe2* this = (EnGe2*)thisx;

    if (!(this->stateFlags & GERUDO_PURPLE_STATE_DISABLE_MOVEMENT)) {
        Actor_MoveWithGravity(&this->picto.actor);
    }
    Actor_UpdateBgCheckInfo(play, &this->picto.actor, 40.0f, 25.0f, 40.0f,
                            UPDBGCHECKINFO_FLAG_1 | UPDBGCHECKINFO_FLAG_4);
    Collider_UpdateCylinder(&this->picto.actor, &this->collider);
    CollisionCheck_SetOC(play, &play->colChkCtx, &this->collider.base);

    if (Cutscene_IsCueInChannel(play, CS_CMD_ACTOR_CUE_476)) {
        this->actionFunc = EnGe2_PerformCutsceneActions;
        this->stateFlags &= ~GERUDO_PURPLE_STATE_KO;
        this->stateFlags &= ~GERUDO_PURPLE_STATE_PATH_REVERSE;
        this->picto.actor.flags |= ACTOR_FLAG_DRAW_CULLING_DISABLED;
        this->picto.actor.speed = 0.0f;
    }

    this->actionFunc(this, play);

    // Blinking
    if (this->stateFlags & GERUDO_PURPLE_STATE_KO) {
        this->eyeIndex = 2;
    } else if (!(this->stateFlags & GERUDO_PURPLE_STATE_STUNNED)) {
        if (DECR(this->blinkTimer) == 0) {
            this->blinkTimer = Rand_S16Offset(60, 60);
        }
        this->eyeIndex = this->blinkTimer;
        if (this->eyeIndex >= 3) {
            this->eyeIndex = 0;
        }
    }
}

s32 EnGe2_ValidatePictograph(PlayState* play, Actor* thisx) {
    s32 ret = Snap_ValidatePictograph(play, thisx, PICTO_VALID_PIRATE_GOOD, &thisx->focus.pos, &thisx->shape.rot, 10.0f,
                                      400.0f, -1);

    ret |= Snap_ValidatePictograph(play, thisx, PICTO_VALID_PIRATE_TOO_FAR, &thisx->focus.pos, &thisx->shape.rot, 10.0f,
                                   1200.0f, -1);
    return ret;
}

s32 EnGe2_OverrideLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, Actor* thisx) {
    EnGe2* this = (EnGe2*)thisx;

    if (limbIndex == GERUDO_PURPLE_NECK_LIMB) {
        rot->x += this->headRot.y;
        rot->z += this->headRot.x;
    }
    return false;
}

void EnGe2_PostLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot, Actor* thisx) {
    static Vec3f sFocusOffset = { 600.0f, 700.0f, 0.0f };

    if (limbIndex == GERUDO_PURPLE_HEAD_LIMB) {
        Matrix_MultVec3f(&sFocusOffset, &thisx->focus.pos);
    }
}

void EnGe2_Draw(Actor* thisx, PlayState* play) {
    static TexturePtr sEyeTextures[] = {
        gGerudoPurpleEyeOpenTex,
        gGerudoPurpleEyeHalfTex,
        gGerudoPurpleEyeClosedTex,
    };
    s32 pad;
    EnGe2* this = (EnGe2*)thisx;

    OPEN_DISPS(play->state.gfxCtx);

    Gfx_SetupDL37_Opa(play->state.gfxCtx);
    gSPSegment(POLY_OPA_DISP++, 0x08, SEGMENTED_TO_K0(sEyeTextures[this->eyeIndex]));
    func_800B8050(&this->picto.actor, play, 0);
    SkelAnime_DrawFlexOpa(play, this->skelAnime.skeleton, this->skelAnime.jointTable, this->skelAnime.dListCount,
                          EnGe2_OverrideLimbDraw, EnGe2_PostLimbDraw, &this->picto.actor);

    CLOSE_DISPS(play->state.gfxCtx);
}
