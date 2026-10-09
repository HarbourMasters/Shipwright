/*
 * File: z_en_racedog.c
 * Overlay: ovl_En_Racedog
 * Description: The dogs that run around the racetrack during the dog race.
 *
 * Note that the dogs milling about the Doggy Racetrack prior to and after the
 * race are actually En_Dg. This actor is solely concerned with the dogs that
 * actually perform the race by running around the track.
 */

#include "z_en_racedog.h"
#include "overlays/actors/ovl_En_Aob_01/z_en_aob_01.h"
#include "overlays/actors/ovl_En_Dg/z_en_dg.h"

#include "2s2h/GameInteractor/GameInteractor.h"
#include "2s2h/BenGui/CosmeticEditor.h"
#include <libultraship/bridge/consolevariablebridge.h>

#define FLAGS (ACTOR_FLAG_UPDATE_CULLING_DISABLED | ACTOR_FLAG_MINIMAP_ICON_ENABLED)

void EnRacedog_Init(Actor* thisx, PlayState* play);
void EnRacedog_Destroy(Actor* thisx, PlayState* play);
void EnRacedog_Update(Actor* thisx, PlayState* play);
void EnRacedog_Draw(Actor* thisx, PlayState* play);
void EnRacedog_Reset(void);

void EnRacedog_RaceStart(EnRacedog* this, PlayState* play);
void EnRacedog_Race(EnRacedog* this, PlayState* play);
void EnRacedog_UpdateTextId(EnRacedog* this);
void EnRacedog_UpdateSpeed(EnRacedog* this);
void EnRacedog_CalculateFinalStretchTargetSpeed(EnRacedog* this);
void EnRacedog_UpdateRaceVariables(EnRacedog* this);
void EnRacedog_CheckForFinish(EnRacedog* this);
void EnRacedog_UpdateRunAnimationPlaySpeed(EnRacedog* this);
s32 EnRacedog_IsOverFinishLine(EnRacedog* this, Vec2f* finishLineCoordinates);
void EnRacedog_SpawnFloorDustRing(EnRacedog* this, PlayState* play);
void EnRacedog_PlaySfxWalk(EnRacedog* this);

/**
 * Dogs can be in three conditions, which is, for the most part, indicated by the
 * message it says when you pick it up prior to entering the race.
 * If it starts with "Ruff!", it's in good condition.
 * If it starts with "Rrr-Ruff!", it's in normal condition.
 * If it starts with "Hoo-whine", it's in bad condition.
 * These text boxes are grouped up like so:
 * - 0x3538 - 0x353D: Good condition
 * - 0x353E - 0x3541: Normal condition
 * - 0x3542 - 0x3546: Bad condition
 *
 * There are two caveats, though, that are useful to keep in mind:
 * - Because of differences between how EnDg computes the text ID and how EnRacedog
 *   computes it, there are two dogs whose text upon picking them up does not
 *   accurately reflect their condition during the race. Check the comment above
 *   EnRacedog_UpdateTextId for more information on this phenomenon.
 * - Note that text ID 0x353D is actually used for the Romani Ranch dog; its text
 *   just so happens to be in the middle of the race dog text block. In EnDg, the
 *   dog that gets this text ID will instead use text ID 0x3538 when the player
 *   picks it up. Since EnRacedog cannot be picked up, however, it can use 0x353D
 *   as an additional good condition text ID.
 */
#define DOG_IS_IN_GOOD_CONDITION(this) (sDogInfo[this->index].textId < 0x353E)
#define DOG_IS_IN_BAD_CONDITION(this) (sDogInfo[this->index].textId >= 0x3542)

/**
 * Stores various information for each dog in the race, mostly related to speed.
 */
typedef struct {
    /* 0x0 */ f32 sprintSpeedMultiplier;        // Multiplies target speed when the dog is in the last 1/4 of the race
    /* 0x4 */ f32 goodConditionSpeedMultiplier; // Multiplies target speed if the dog is in good condition
    /* 0x8 */ s16 color;                        // The dog's color, which is used as an index into sBaseSpeeds
    /* 0xA */ s16 index;                        // The dog's index within sDogInfo
    /* 0xC */ s16 pointToUseSecondBaseSpeed; // When the dog is at or after this point, use the second sBaseSpeeds value
    /* 0xE */ s16 textId;                    // Used to determine the dog's condition
} RaceDogInfo;                               // size = 0x10

ActorProfile En_Racedog_Profile = {
    /**/ ACTOR_EN_RACEDOG,
    /**/ ACTORCAT_NPC,
    /**/ FLAGS,
    /**/ OBJECT_DOG,
    /**/ sizeof(EnRacedog),
    /**/ EnRacedog_Init,
    /**/ EnRacedog_Destroy,
    /**/ EnRacedog_Update,
    /**/ EnRacedog_Draw,
    /**/ EnRacedog_Reset,
};

static s16 sNumberOfDogsFinished = 0;

/**
 * The furthest point along the race track path that any dog has reached.
 */
static s16 sFurthestPoint = -1;

/**
 * Starts counting up as soon as the first-place dog gets 3/4ths of the way through the race track,
 * and stops counting up as soon as the first-place dog finishes. Used to scale up the sprint
 * multiplier in EnRacedog_CalculateFinalStretchTargetSpeed.
 */
static s16 sSprintTimer = 0;

/**
 * The index of the dog currently in first place. It's determined by checking, for each dog on the track,
 * if its current point along the race track path is greater than or equal to sFurthestPoint.
 */
static s16 sFirstPlaceIndex = -1;

/**
 * The base speeds for each dog indexed by its color. The two values are used during different parts of the race.
 * - At the very start of the race, all dogs will use the first value for their color, but only for a single point
 *   along the race track's path. After that, they will have a base speed of 5.0, regardless of what's in the table
 *   for their color. The sole exception to this is the blue dog, who will use its first value for the entire
 *   first 1/4th of the race.
 * - For the last 3/4ths of the race, all dogs will check to see if their current point along the race track path is
 *   greater than or equal to the pointToUseSecondBaseSpeed for their RaceDogInfo. If it is, then they will use the
 *   second value for their color as their base speed; otherwise, they will use 5.0.
 */
static f32 sBaseSpeeds[][2] = {
    { 0.0f, 0.0f }, { 5.0f, 5.5f }, { 5.0f, 5.0f }, { 5.5f, 5.0f }, { 4.5f, 5.5f }, { 6.0f, 4.0f }, { 4.0f, 6.0f },
};

/**
 * A table of RaceDogInfo for every dog in the race. Note that the sprintSpeedMultiplier and
 * textId values in this table are updated by functions within this actor.
 */
static RaceDogInfo sDogInfo[] = {
    { -1.0f, 1.2f, DOG_COLOR_BEIGE, 0, 9, 0x3539 },  { -1.0f, 1.2f, DOG_COLOR_WHITE, 1, 9, 0x353A },
    { -1.0f, 1.2f, DOG_COLOR_BLUE, 2, 10, 0x353B },  { -1.0f, 1.2f, DOG_COLOR_GRAY, 3, 9, 0x353C },
    { -1.0f, 1.2f, DOG_COLOR_BROWN, 4, 8, 0x353D },  { -1.0f, 1.2f, DOG_COLOR_GRAY, 5, 9, 0x353E },
    { -1.0f, 1.2f, DOG_COLOR_BEIGE, 6, 9, 0x353F },  { -1.0f, 1.2f, DOG_COLOR_WHITE, 7, 9, 0x3540 },
    { -1.0f, 1.2f, DOG_COLOR_WHITE, 8, 9, 0x3541 },  { -1.0f, 1.2f, DOG_COLOR_GOLD, 9, 8, 0x3542 },
    { -1.0f, 1.2f, DOG_COLOR_GRAY, 10, 9, 0x3543 },  { -1.0f, 1.2f, DOG_COLOR_BEIGE, 11, 9, 0x3544 },
    { -1.0f, 1.2f, DOG_COLOR_WHITE, 12, 9, 0x3545 }, { -1.0f, 1.2f, DOG_COLOR_BROWN, 13, 8, 0x3546 },
};

/**
 * Stores the RacedogInfo for the dog that is selected by the player. These values are just
 * placeholders, and the actual value gets grabbed from sDogInfo in EnRacedog_Init.
 */
static RaceDogInfo sSelectedDogInfo = { -1.0f, 1.0f, DOG_COLOR_DEFAULT, -1, 0, 0x353E };

/**
 * The XZ-coordinates used to determine if the dog is inside the finish line.
 * They're a little bit bigger than the in-game visual.
 */
static Vec2f sFinishLineCoordinates[] = {
    { -3914.0f, 1283.0f },
    { -3747.0f, 1104.0f },
    { -3717.0f, 1169.0f },
    { -3897.0f, 1308.0f },
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
    { 13, 19, 0, { 0, 0, 0 } },
};

static CollisionCheckInfoInit2 sColChkInfoInit = { 0, 0, 0, 0, 1 };

static DamageTable sDamageTable = {
    /* Deku Nut       */ DMG_ENTRY(0, 0x0),
    /* Deku Stick     */ DMG_ENTRY(0, 0x0),
    /* Horse trample  */ DMG_ENTRY(0, 0x0),
    /* Explosives     */ DMG_ENTRY(0, 0x0),
    /* Zora boomerang */ DMG_ENTRY(0, 0x0),
    /* Normal arrow   */ DMG_ENTRY(0, 0x0),
    /* UNK_DMG_0x06   */ DMG_ENTRY(0, 0x0),
    /* Hookshot       */ DMG_ENTRY(0, 0x0),
    /* Goron punch    */ DMG_ENTRY(0, 0x0),
    /* Sword          */ DMG_ENTRY(0, 0x0),
    /* Goron pound    */ DMG_ENTRY(0, 0x0),
    /* Fire arrow     */ DMG_ENTRY(0, 0x0),
    /* Ice arrow      */ DMG_ENTRY(0, 0x0),
    /* Light arrow    */ DMG_ENTRY(0, 0x0),
    /* Goron spikes   */ DMG_ENTRY(0, 0x0),
    /* Deku spin      */ DMG_ENTRY(0, 0x0),
    /* Deku bubble    */ DMG_ENTRY(0, 0x0),
    /* Deku launch    */ DMG_ENTRY(0, 0x0),
    /* UNK_DMG_0x12   */ DMG_ENTRY(0, 0x0),
    /* Zora barrier   */ DMG_ENTRY(0, 0x0),
    /* Normal shield  */ DMG_ENTRY(0, 0x0),
    /* Light ray      */ DMG_ENTRY(0, 0x0),
    /* Thrown object  */ DMG_ENTRY(0, 0x0),
    /* Zora punch     */ DMG_ENTRY(0, 0x0),
    /* Spin attack    */ DMG_ENTRY(0, 0x0),
    /* Sword beam     */ DMG_ENTRY(0, 0x0),
    /* Normal Roll    */ DMG_ENTRY(0, 0x0),
    /* UNK_DMG_0x1B   */ DMG_ENTRY(0, 0x0),
    /* UNK_DMG_0x1C   */ DMG_ENTRY(0, 0x0),
    /* Unblockable    */ DMG_ENTRY(0, 0x0),
    /* UNK_DMG_0x1E   */ DMG_ENTRY(0, 0x0),
    /* Powder Keg     */ DMG_ENTRY(0, 0x0),
};

typedef enum {
    /*  0 */ RACEDOG_ANIM_IDLE,
    /*  1 */ RACEDOG_ANIM_WALK_1, // unused
    /*  2 */ RACEDOG_ANIM_RUN,
    /*  3 */ RACEDOG_ANIM_BARK,               // unused
    /*  4 */ RACEDOG_ANIM_SIT_DOWN_ONCE,      // unused
    /*  5 */ RACEDOG_ANIM_SIT_DOWN,           // unused
    /*  6 */ RACEDOG_ANIM_LYING_DOWN_START_1, // unused
    /*  7 */ RACEDOG_ANIM_LYING_DOWN_LOOP,    // unused
    /*  8 */ RACEDOG_ANIM_LYING_DOWN_START_2, // unused
    /*  9 */ RACEDOG_ANIM_LYING_DOWN_START_3, // unused
    /* 10 */ RACEDOG_ANIM_LYING_DOWN_START_4, // unused
    /* 11 */ RACEDOG_ANIM_WALK_BACKWARDS,     // unused
    /* 12 */ RACEDOG_ANIM_JUMP,
    /* 13 */ RACEDOG_ANIM_LONG_JUMP,   // unused
    /* 14 */ RACEDOG_ANIM_JUMP_ATTACK, // unused
    /* 15 */ RACEDOG_ANIM_SWIM,        // unused
    /* 16 */ RACEDOG_ANIM_MAX
} RacedogAnimation;

static AnimationInfoS sAnimationInfo[RACEDOG_ANIM_MAX] = {
    { &gDogWalkAnim, 1.0f, 0, -1, ANIMMODE_LOOP, 0 },           // RACEDOG_ANIM_IDLE
    { &gDogWalkAnim, 1.0f, 0, -1, ANIMMODE_LOOP, -6 },          // RACEDOG_ANIM_WALK_1
    { &gDogRunAnim, 1.0f, 0, -1, ANIMMODE_LOOP, 0 },            // RACEDOG_ANIM_RUN
    { &gDogBarkAnim, 1.0f, 0, -1, ANIMMODE_LOOP, -6 },          // RACEDOG_ANIM_BARK
    { &gDogSitAnim, 1.0f, 0, -1, ANIMMODE_ONCE, -6 },           // RACEDOG_ANIM_SIT_DOWN_ONCE
    { &gDogSitAnim, 1.0f, 0, -1, ANIMMODE_LOOP_PARTIAL, -6 },   // RACEDOG_ANIM_SIT_DOWN
    { &gDogLyingDownAnim, 1.0f, 0, -1, ANIMMODE_ONCE, -6 },     // RACEDOG_ANIM_LYING_DOWN_START_1
    { &gDogLyingDownLoopAnim, 1.0f, 0, -1, ANIMMODE_LOOP, -6 }, // RACEDOG_ANIM_LYING_DOWN_LOOP
    { &gDogLyingDownAnim, 1.0f, 0, 27, ANIMMODE_ONCE, -6 },     // RACEDOG_ANIM_LYING_DOWN_START_2
    { &gDogLyingDownAnim, 1.0f, 28, -1, ANIMMODE_ONCE, -6 },    // RACEDOG_ANIM_LYING_DOWN_START_3
    { &gDogLyingDownAnim, 1.0f, 54, 54, ANIMMODE_ONCE, -6 },    // RACEDOG_ANIM_LYING_DOWN_START_4
    { &gDogWalkAnim, -1.5f, -1, 0, ANIMMODE_LOOP, -6 },         // RACEDOG_ANIM_WALK_BACKWARDS
    { &gDogJumpAnim, 1.0f, 0, -1, ANIMMODE_ONCE, 0 },           // RACEDOG_ANIM_JUMP
    { &gDogLongJumpAnim, 1.2f, 0, -1, ANIMMODE_ONCE, 0 },       // RACEDOG_ANIM_LONG_JUMP
    { &gDogJumpAttackAnim, 1.2f, 0, -1, ANIMMODE_ONCE, 0 },     // RACEDOG_ANIM_JUMP_ATTACK
    { &gDogWalkAnim, 0.5f, 0, -1, ANIMMODE_LOOP, 0 },           // RACEDOG_ANIM_SWIM
};

static InitChainEntry sInitChain[] = {
    ICHAIN_F32(cullingVolumeDistance, 1000, ICHAIN_STOP),
};

void EnRacedog_ChangeAnim(SkelAnime* skelAnime, AnimationInfoS* animationInfo, s32 animIndex) {
    f32 endFrame;

    animationInfo += animIndex;
    if (animationInfo->frameCount < 0) {
        endFrame = Animation_GetLastFrame(animationInfo->animation);
    } else {
        endFrame = animationInfo->frameCount;
    }

    Animation_Change(skelAnime, animationInfo->animation, animationInfo->playSpeed + (BREG(88) * 0.1f),
                     animationInfo->startFrame, endFrame, animationInfo->mode, animationInfo->morphFrames);
}

void EnRacedog_UpdateCollision(EnRacedog* this, PlayState* play) {
    this->collider.dim.pos.x = this->actor.world.pos.x;
    this->collider.dim.pos.y = this->actor.world.pos.y;
    this->collider.dim.pos.z = this->actor.world.pos.z;
    CollisionCheck_SetOC(play, &play->colChkCtx, &this->collider.base);
    Actor_UpdateBgCheckInfo(play, &this->actor, 26.0f, 10.0f, 0.0f, UPDBGCHECKINFO_FLAG_1 | UPDBGCHECKINFO_FLAG_4);
}

/**
 * Returns the Y-rotation the dog should have to move to the next point along the race track path.
 * There is a small degree of randomness incorporated into this angle.
 */
s16 EnRacedog_GetYRotation(Path* path, s32 pointIndex, Vec3f* pos, f32* distSQ) {
    Vec3s* point;
    f32 diffXRand;
    f32 diffZRand;
    f32 diffX;
    f32 diffZ;

    if (path != NULL) {
        point = Lib_SegmentedToVirtual(path->points);
        point = &point[pointIndex];
        diffXRand = (Rand_CenteredFloat(100.0f) + point->x) - pos->x;
        diffZRand = (Rand_CenteredFloat(100.0f) + point->z) - pos->z;
        diffX = point->x - pos->x;
        diffZ = point->z - pos->z;
    } else {
        diffXRand = 0.0f;
        diffZRand = 0.0f;
        diffX = 0.0f;
        diffZ = 0.0f;
    }

    *distSQ = SQ(diffX) + SQ(diffZ);
    return RAD_TO_BINANG(Math_Atan2F_XY(diffZRand, diffXRand));
}

void EnRacedog_GetFloorRot(EnRacedog* this, Vec3f* floorRot) {
    f32 ny;
    f32 nz;

    if (this->actor.floorPoly != NULL) {
        ny = COLPOLY_GET_NORMAL(this->actor.floorPoly->normal.y);
        nz = COLPOLY_GET_NORMAL(this->actor.floorPoly->normal.z);

        floorRot->x = -Math_Atan2F_XY(1.0f, -nz * ny);
    }
}

void EnRacedog_Init(Actor* thisx, PlayState* play) {
    EnRacedog* this = (EnRacedog*)thisx;
    ColliderCylinder* collider = &this->collider;

    ActorShape_Init(&this->actor.shape, 0.0f, ActorShadow_DrawCircle, 24.0f);
    SkelAnime_InitFlex(play, &this->skelAnime, &gDogSkel, NULL, this->jointTable, this->morphTable, DOG_LIMB_MAX);
    Collider_InitCylinder(play, collider);
    Collider_SetCylinder(play, collider, &this->actor, &sCylinderInit);
    CollisionCheck_SetInfo2(&this->actor.colChkInfo, &sDamageTable, &sColChkInfoInit);

    Actor_ProcessInitChain(&this->actor, sInitChain);
    this->path = SubS_GetPathByIndex(play, ENRACEDOG_GET_PATH_INDEX(&this->actor), ENRACEDOG_PATH_INDEX_NONE);
    Actor_SetScale(&this->actor, 0.0075f);
    this->actor.gravity = -3.0f;
    if (ENRACEDOG_GET_INDEX(&this->actor) < RACEDOG_COUNT) {
        this->index = ENRACEDOG_GET_INDEX(&this->actor);
    } else {
        Actor_Kill(&this->actor);
    }

    this->selectionArrowGreenPrimColor = 255;
    this->selectionArrowGreenEnvColor = 50;
    this->selectionArrowTimer = 12;
    this->prevRot.x = 0.0f;
    this->prevRot.y = 0.0f;
    this->prevRot.z = 0.0f;
    this->selectionArrowScale = 1.0f;

    // The first part of this check is a bit strange. If they intended to check for dogs that were
    // in good condition, they should've stopped at 0x353D instead of 0x353E. Additionally, this
    // runs before EnRacedog_UpdateTextId is called to set the text IDs to their "correct" values,
    // meaning that the IDs are just whatever their default values in sDogInfo are. As a result,
    // the blue dog, one beige dog, one white dog, one brown dog, and two gray dogs never bother
    // to do the random 1/20 check here, regardless of anything else.
    if ((sDogInfo[this->index].textId > 0x353E) && (this->index == TRUNCF_BINANG(Rand_ZeroFloat(20.0f)))) {
        this->extraTimeBeforeRaceStart = 5;
    } else {
        this->extraTimeBeforeRaceStart = 0;
    }

    this->raceStartTimer = 60;
    this->raceStartTimer += this->extraTimeBeforeRaceStart;
    this->targetSpeed = sBaseSpeeds[sDogInfo[this->index].color][0];
    this->raceStatus = RACEDOG_RACE_STATUS_BEFORE_POINT_9;
    this->pointForCurrentTargetSpeed = -1;

    EnRacedog_UpdateTextId(this);
    this->actor.flags |= ACTOR_FLAG_UPDATE_CULLING_DISABLED;
    this->actor.flags |= ACTOR_FLAG_DRAW_CULLING_DISABLED;

    sSelectedDogInfo = sDogInfo[(s16)GET_EVENTINF_DOG_RACE_SELECTED_DOG_INDEX];
    this->selectedDogIndex = sSelectedDogInfo.index;

    EnRacedog_ChangeAnim(&this->skelAnime, sAnimationInfo, RACEDOG_ANIM_IDLE);
    sAnimationInfo[RACEDOG_ANIM_IDLE].playSpeed = Rand_ZeroFloat(0.5f) + 1.0f;
    this->actionFunc = EnRacedog_RaceStart;
}

void EnRacedog_Destroy(Actor* thisx, PlayState* play) {
    EnRacedog* this = (EnRacedog*)thisx;

    Collider_DestroyCylinder(play, &this->collider);
}

/**
 * This function makes the dog wait at the starting line until the race actually begins.
 * It's also responsible for playing the starting shot sound once the race begins.
 */
void EnRacedog_RaceStart(EnRacedog* this, PlayState* play) {
    if (DECR(this->raceStartTimer) == 0) {
        this->raceStartTimer = Rand_S16Offset(50, 50);
        if (this->extraTimeBeforeRaceStart == 0) {
            Audio_PlaySfx(NA_SE_SY_START_SHOT);
        }

        EnRacedog_ChangeAnim(&this->skelAnime, sAnimationInfo, RACEDOG_ANIM_RUN);
        this->actionFunc = EnRacedog_Race;
    }
}

/**
 * This function handles the dog's behavior and state for the entire time the race is going.
 */
void EnRacedog_Race(EnRacedog* this, PlayState* play) {
    s16 yRotation;
    f32 distSq;

    this->collider.dim.radius = 15;
    if (this->path != NULL) {
        yRotation = EnRacedog_GetYRotation(this->path, this->currentPoint, &this->actor.world.pos, &distSq);
        if (this->actor.bgCheckFlags & BGCHECKFLAG_WALL) {
            yRotation = this->actor.wallYaw;
        }

        Math_SmoothStepToS(&this->actor.world.rot.y, yRotation, 4, 0x3E8, 1);
        this->actor.shape.rot.y = this->actor.world.rot.y;

        if (distSq <= SQ(50.0f)) {
            this->currentPoint++;
            if (this->currentPoint >= (this->path->count - 1)) {
                this->currentPoint = 0;
            }
        }

        EnRacedog_UpdateSpeed(this);

        // Putting this after EnRacedog_UpdateSpeed will ensure that when EnRacedog_CalculateFinalStretchTargetSpeed
        // is called for the first-place dog, the sprint timer will be 0, so the first-place dog will be guaranteed
        // to have a sprint speed multiplier of 1.
        if ((this->currentPoint >= ((this->path->count / 4) * 3)) && (this->index == sFirstPlaceIndex)) {
            sSprintTimer++;
        }

        EnRacedog_UpdateRaceVariables(this);
        EnRacedog_CheckForFinish(this);
        Actor_MoveWithGravity(&this->actor);
    }

    EnRacedog_UpdateRunAnimationPlaySpeed(this);
    EnRacedog_PlaySfxWalk(this);
    EnRacedog_SpawnFloorDustRing(this, play);
}

/**
 * Updates the text ID in sDogInfo based on what was set in the weekEventRegs by
 * En_Aob_01. This makes it so sDogInfo can be used in other functions to determine
 * the condition of the dog.
 *
 * Note that the text IDs generated by this function are off-by-one compared to the
 * similar function in EnDg. The end result of this is that one of the dogs that says
 * it's in good condition when you pick it up is actually in normal condition once the
 * race starts, and one of the dogs that says it's in normal condition when you pick
 * it up is actually in bad condition. It's unknown whether this is a simple oversight
 * or an intentional choice to introduce a bit of extra variance to the race.
 */
void EnRacedog_UpdateTextId(EnRacedog* this) {
    // This will produce a text ID in the range of 0x3539 to 0x3546.
    sDogInfo[this->index].textId = GET_WEEKEVENTREG_DOG_RACE_TEXT(this->index, 0x3539);

    // As a sanity check, this makes sure the text ID is something in the expected range of 0x3539 to 0x3546.
    if ((sDogInfo[this->index].textId > 0x3546) || (sDogInfo[this->index].textId < 0x3539)) {
        sDogInfo[this->index].textId = 0x353E;
    }

    // Actual valid text IDs for the race dogs range between 0x3538 and 0x3546, so this
    // code makes the two ranges match up. Perhaps the text got shifted?
    if (sDogInfo[this->index].textId == 0x3547) {
        sDogInfo[this->index].textId = 0x3538;
    }
}

/**
 * Responsible for calculating the dog's target speed and for making its actual speed
 * approach the target speed.
 */
void EnRacedog_UpdateSpeed(EnRacedog* this) {
    s32 quarterPathCount;
    s32 pathCount = this->path->count;

    // Only update the target speed if the dog is at a further point along the path
    // than it was when the target speed was last updated.
    if (this->pointForCurrentTargetSpeed < this->currentPoint) {
        this->pointForCurrentTargetSpeed = this->currentPoint;
        if (this->currentPoint == 0) {
            // The dog is at the very start of the race track.
            this->targetSpeed = sBaseSpeeds[sDogInfo[this->index].color][0];
        } else {
            quarterPathCount = pathCount / 4;
            if (this->currentPoint < quarterPathCount) {
                // The dog is past the very start, but is still less than 1/4th of the way through the race track.
                // This code will give the blue dog a higher base speed than any other dog (6.0 instead of 5.0).
                if (sDogInfo[this->index].color == DOG_COLOR_BLUE) {
                    this->targetSpeed = sBaseSpeeds[sDogInfo[this->index].color][0] + Rand_CenteredFloat(1.0f);
                } else {
                    this->targetSpeed = 5.0f + Rand_CenteredFloat(1.0f);
                }

                if (DOG_IS_IN_GOOD_CONDITION(this) && (this->index != sFirstPlaceIndex)) {
                    this->targetSpeed *= sDogInfo[this->index].goodConditionSpeedMultiplier;
                }
            } else if (this->currentPoint < (quarterPathCount * 3)) {
                // The dog is between 1/4th and 3/4ths of the way through the race track.
                if (this->currentPoint < sDogInfo[this->index].pointToUseSecondBaseSpeed) {
                    this->targetSpeed = 5.0f + Rand_CenteredFloat(1.0f);
                } else {
                    this->targetSpeed = sBaseSpeeds[sDogInfo[this->index].color][1] + Rand_CenteredFloat(1.0f);

                    if (DOG_IS_IN_GOOD_CONDITION(this) && (this->index != sFirstPlaceIndex)) {
                        this->targetSpeed *= sDogInfo[this->index].goodConditionSpeedMultiplier;
                    }
                }
            } else if (this->currentPoint < pathCount) {
                // The dog is more than 3/4ths of the way through the race track.
                EnRacedog_CalculateFinalStretchTargetSpeed(this);
            } else {
                this->targetSpeed = Rand_CenteredFloat(1.0f) + 5.0f;
            }
        }
    }

    // Seemingly the only point of this raceStatus check is to ensure this code still runs when
    // the dog has finished the race and is at point 0 along their second lap.
    if ((this->currentPoint != 0) || (this->raceStatus != RACEDOG_RACE_STATUS_BEFORE_POINT_9)) {
        this->actor.shape.rot.y = this->actor.world.rot.y;
    }

    Math_ApproachF(&this->actor.speed, this->targetSpeed, 0.5f, 3.0f);

    // The dog that the player has selected has a slightly higher max speed than the other dogs.
    if (this->index == this->selectedDogIndex) {

        // Cap the speed of the dog, or always max it out with the 'Always Win Doggy Race' enhancement.
        bool vanillaCondition = this->actor.speed > 7.5f;
        if (GameInteractor_Should(VB_DOGGY_RACE_SET_MAX_SPEED, vanillaCondition)) {
            this->actor.speed = 7.5f;
        }
    } else {
        if (this->actor.speed > 7.0f) {
            this->actor.speed = 7.0f;
        }
    }
}

/**
 * This function handles updating targetSpeed for the final stretch of the race,
 * where the dog starts sprinting towards the finish line.
 */
void EnRacedog_CalculateFinalStretchTargetSpeed(EnRacedog* this) {
    f32 sprintSpeedMultiplier;

    // Dogs are only allowed to update their sprintSpeedMultiplier once, so its value will
    // depend on the value of sSprintTimer when they first hit the 3/4th mark on the track.
    if (sDogInfo[this->index].sprintSpeedMultiplier == -1.0f) {
        if (sSprintTimer < 100.0f) {
            sDogInfo[this->index].sprintSpeedMultiplier = 200.0f / (200.0f - sSprintTimer);
        } else {
            sDogInfo[this->index].sprintSpeedMultiplier = 2.0f;
        }
    }

    if (!DOG_IS_IN_BAD_CONDITION(this)) {
        sprintSpeedMultiplier = sDogInfo[this->index].sprintSpeedMultiplier;
        this->targetSpeed = sprintSpeedMultiplier * sBaseSpeeds[sDogInfo[this->index].color][1];
    }

    if (DOG_IS_IN_GOOD_CONDITION(this) && (this->index != sFirstPlaceIndex)) {
        this->targetSpeed *= sDogInfo[this->index].goodConditionSpeedMultiplier;
    }
}

/**
 * Responsible for updating the raceStatus variable for this dog, as well as updating the
 * sFurthestPoint and sFirstPlaceIndex variables.
 */
void EnRacedog_UpdateRaceVariables(EnRacedog* this) {
    if ((this->currentPoint >= 9) && (this->raceStatus == RACEDOG_RACE_STATUS_BEFORE_POINT_9)) {
        this->raceStatus = RACEDOG_RACE_STATUS_BETWEEN_POINTS_9_AND_11;
    }

    if ((this->currentPoint >= 11) && (this->raceStatus == RACEDOG_RACE_STATUS_BETWEEN_POINTS_9_AND_11)) {
        this->raceStatus = RACEDOG_RACE_STATUS_AFTER_POINT_11;
    }

    if (((this->currentPoint >= sFurthestPoint) || (this->raceStatus <= RACEDOG_RACE_STATUS_BEFORE_POINT_9)) &&
        (this->currentPoint > sFurthestPoint)) {
        sFurthestPoint = this->currentPoint;
        sFirstPlaceIndex = this->index;
    }
}

/**
 * Checks to see if this dog has finished the race. This function is responsible for changing the
 * music when the first dog finishes the race, and it is also responsible for updating the event
 * flags with what position the player's selected dog finished in.
 */
void EnRacedog_CheckForFinish(EnRacedog* this) {
    if (EnRacedog_IsOverFinishLine(this, sFinishLineCoordinates) &&
        this->raceStatus == RACEDOG_RACE_STATUS_AFTER_POINT_11) {
        sNumberOfDogsFinished++;
        if (sNumberOfDogsFinished == 1) {
            SEQCMD_PLAY_SEQUENCE(SEQ_PLAYER_BGM_MAIN, 0, NA_BGM_HORSE_GOAL | SEQ_FLAG_ASYNC);
            Audio_PlaySfx(NA_SE_SY_START_SHOT);
        }

        this->raceStatus = RACEDOG_RACE_STATUS_FINISHED;
        if (this->index == this->selectedDogIndex) {
            SET_EVENTINF_DOG_RACE_RACE_STANDING(sNumberOfDogsFinished);
        }
    }
}

/**
 * Slows the dog's running speed animation if its speed is less than 3.0, and sets it
 * to normal speed otherwise.
 */
void EnRacedog_UpdateRunAnimationPlaySpeed(EnRacedog* this) {
    if (this->actor.speed < 3.0f) {
        sAnimationInfo[RACEDOG_ANIM_RUN].playSpeed = 0.9f;
    } else {
        sAnimationInfo[RACEDOG_ANIM_RUN].playSpeed = 1.0f;
    }
}

/**
 * Returns true if the dog's current position is in the finish line.
 */
s32 EnRacedog_IsOverFinishLine(EnRacedog* this, Vec2f* finishLineCoordinates) {
    f32 xDistToTopFront;
    f32 zDistToTopFront;
    f32 xDistToBottomFront;
    f32 xDistToBottomBack;
    f32 zDistToBottomBack;
    f32 zDistToBottomFront;
    f32 zDistToTopBack;
    f32 xDistToTopBack;
    f32 frontPointsCrossProduct;
    f32 crossProductTemp;

    xDistToTopFront = this->actor.world.pos.x - finishLineCoordinates[0].x;
    zDistToTopFront = this->actor.world.pos.z - finishLineCoordinates[0].z;
    xDistToBottomFront = this->actor.world.pos.x - finishLineCoordinates[1].x;
    zDistToBottomFront = this->actor.world.pos.z - finishLineCoordinates[1].z;
    xDistToBottomBack = this->actor.world.pos.x - finishLineCoordinates[2].x;
    zDistToBottomBack = this->actor.world.pos.z - finishLineCoordinates[2].z;
    xDistToTopBack = this->actor.world.pos.x - finishLineCoordinates[3].x;
    zDistToTopBack = this->actor.world.pos.z - finishLineCoordinates[3].z;

    // frontPointsCrossProduct is positive if the dog is to the left of the line formed by the front points
    // crossProductTemp is positive if the dog is above the line formed by the bottom points
    // This is checking that the dog within the region defined by front and bottom lines like so:
    //         |
    //    X    | Front
    //         |
    // --------
    //  Bottom
    frontPointsCrossProduct = ((xDistToTopFront * zDistToBottomFront) - (xDistToBottomFront * zDistToTopFront));
    crossProductTemp = (((xDistToBottomFront * zDistToBottomBack) - (xDistToBottomBack * zDistToBottomFront)));
    //! @bug If any dog is precisely (with floating-point precision) on top of the line formed by the front points,
    //! then frontPointsCrossProduct will be zero. This will cause this multiplication (and all future multiplications)
    //! to be zero, which will make this function think the dog has crossed the finish line. The line formed by the
    //! front points extends throughout the entire racetrack, so a dog can trigger this when they're not even close to
    //! the actual finish line, causing them to finish the race incredibly early.
    if (frontPointsCrossProduct * crossProductTemp < 0.0f) {
        return false;
    }

    // crossProductTemp is positive if the dog is to the right of the line formed by the back points
    // This is checking that the dog within the region defined by front and back lines like so:
    //       |       |
    //  Back |   X   | Front
    //       |       |
    frontPointsCrossProduct = ((xDistToTopFront * zDistToBottomFront) - (xDistToBottomFront * zDistToTopFront));
    crossProductTemp = ((xDistToBottomBack * zDistToTopBack) - (xDistToTopBack * zDistToBottomBack));
    if (frontPointsCrossProduct * crossProductTemp < 0.0f) {
        return false;
    }

    // crossProductTemp is positive if the dog is below the line formed by the top points
    // This is checking that the dog within the region defined by front and top lines like so:
    //   Top
    // --------
    //         |
    //    X    | Front
    //         |
    frontPointsCrossProduct = ((xDistToTopFront * zDistToBottomFront) - (xDistToBottomFront * zDistToTopFront));
    crossProductTemp = ((xDistToTopBack * zDistToTopFront) - (xDistToTopFront * zDistToTopBack));
    if (frontPointsCrossProduct * crossProductTemp < 0.0f) {
        return false;
    }

    return true;
}

void EnRacedog_SpawnFloorDustRing(EnRacedog* this, PlayState* play) {
    s16 curFrame = this->skelAnime.curFrame;
    s16 mod = (this->actor.speed > 6.0f) ? 2 : 3;
    Vec3f pos;

    if (((this->index + curFrame) % mod) == 0) {
        pos.x = this->actor.world.pos.x + Rand_CenteredFloat(15.0f);
        pos.y = this->actor.world.pos.y;
        pos.z = this->actor.world.pos.z + Rand_CenteredFloat(15.0f);
        Actor_SpawnFloorDustRing(play, &this->actor, &pos, 10.0f, 0, 2.0f, 300, 0, true);
    }
}

void EnRacedog_PlaySfxWalk(EnRacedog* this) {
    s16 curFrame = this->skelAnime.curFrame;

    if ((curFrame == 1) || (curFrame == 7)) {
        Actor_PlaySfx(&this->actor, NA_SE_EV_MONKEY_WALK);
    }
}

void EnRacedog_Update(Actor* thisx, PlayState* play) {
    s32 pad;
    EnRacedog* this = (EnRacedog*)thisx;
    Vec3f floorRot = { 0.0f, 0.0f, 0.0f };

    this->selectedDogIndex = sSelectedDogInfo.index;

    this->actionFunc(this, play);

    EnRacedog_UpdateCollision(this, play);
    EnRacedog_GetFloorRot(this, &floorRot);
    Math_ApproachF(&this->curRot.x, floorRot.x, 0.2f, 0.1f);

    if (this->prevRot.x > 0.0f) {
        if ((this->curRot.x < 0.0f) && (this->curRot.x > -0.1f)) {
            // Moves to the part of the running animation where the dog has four feet
            // on the ground and is about to lift its feet off the ground.
            this->skelAnime.curFrame = 4.0f;
            this->actor.velocity.y = 5.5f;
        }
    }

    if (!(this->actor.bgCheckFlags & BGCHECKFLAG_GROUND)) {
        this->skelAnime.curFrame = 0.0f;
    }

    this->prevRot = this->curRot;
    SkelAnime_Update(&this->skelAnime);
}

void EnRacedog_UpdateSelectionArrow(EnRacedog* this) {
    if (this->selectionArrowTimer > 6) {
        this->selectionArrowGreenPrimColor -= 16;
        this->selectionArrowGreenEnvColor += 8;
        this->selectionArrowScale += 0.05f;
    } else {
        this->selectionArrowGreenPrimColor += 16;
        this->selectionArrowGreenEnvColor -= 8;
        this->selectionArrowScale -= 0.05f;
    }

    if (DECR(this->selectionArrowTimer) == 0) {
        this->selectionArrowTimer = 12;
    }
}

void EnRacedog_DrawSelectionArrow(EnRacedog* this, PlayState* play) {
    Vec3s rotation = gZeroVec3s;
    s32 shouldDrawSelectionArrow = (this->index == this->selectedDogIndex) ? true : false;

    if (shouldDrawSelectionArrow) {
        OPEN_DISPS(play->state.gfxCtx);

        Gfx_SetupDL25_Opa(play->state.gfxCtx);
        EnRacedog_UpdateSelectionArrow(this);
        Matrix_SetTranslateRotateYXZ(this->actor.world.pos.x, this->actor.world.pos.y + 40.0f, this->actor.world.pos.z,
                                     &rotation);

        gDPPipeSync(POLY_OPA_DISP++);
        gDPSetPrimColor(POLY_OPA_DISP++, 0, 255, 255, this->selectionArrowGreenPrimColor, 0, 255);
        gDPSetEnvColor(POLY_OPA_DISP++, 255, this->selectionArrowGreenEnvColor, 0, 255);
        Matrix_Scale(this->selectionArrowScale * 2.0f, this->selectionArrowScale * 2.0f,
                     this->selectionArrowScale * 2.0f, MTXMODE_APPLY);
        MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx);
        gSPDisplayList(POLY_OPA_DISP++, gDogSelectionArrowEmptyDL);
        gSPDisplayList(POLY_OPA_DISP++, gDogSelectionArrowDL);

        CLOSE_DISPS(play->state.gfxCtx);
    }
}

s32 EnRacedog_OverrideLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, Actor* thisx) {
    return false;
}

void EnRacedog_PostLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot, Actor* thisx) {
    s32 pad;
    EnRacedog* this = (EnRacedog*)thisx;
    Vec3f focusOffset = { 0.0f, 20.0f, 0.0f };

    if (limbIndex == DOG_LIMB_HEAD) {
        Matrix_MultVec3f(&focusOffset, &this->actor.focus.pos);
    }

    if (limbIndex == DOG_LIMB_TAIL) {
        EnRacedog_DrawSelectionArrow(this, play);
    }
}

void EnRacedog_Draw(Actor* thisx, PlayState* play) {
    EnRacedog* this = (EnRacedog*)thisx;

    OPEN_DISPS(play->state.gfxCtx);

    Gfx_SetupDL25_Opa(play->state.gfxCtx);

    gDPPipeSync(POLY_OPA_DISP++);

    switch (sDogInfo[this->index].color) {
        case DOG_COLOR_BEIGE:
            gDPSetEnvColor(POLY_OPA_DISP++, 255, 255, 200, 0);
            break;

        case DOG_COLOR_WHITE:
            gDPSetEnvColor(POLY_OPA_DISP++, 255, 255, 255, 0);
            break;

        case DOG_COLOR_BLUE:
            gDPSetEnvColorOverride(POLY_OPA_DISP++, 79, 79, 143, 0, COSMETIC_ID("World.RaceDogBlue"));
            break;

        case DOG_COLOR_GOLD:
            gDPSetEnvColor(POLY_OPA_DISP++, 255, 207, 47, 0);
            break;

        case DOG_COLOR_BROWN:
            gDPSetEnvColor(POLY_OPA_DISP++, 143, 79, 47, 0);
            break;

        case DOG_COLOR_GRAY:
            gDPSetEnvColor(POLY_OPA_DISP++, 143, 143, 143, 0);
            break;

        default:
            gDPSetEnvColor(POLY_OPA_DISP++, 255, 255, 200, 0);
            break;
    }

    Matrix_Translate(this->actor.world.pos.x, this->actor.world.pos.y, this->actor.world.pos.z, MTXMODE_NEW);
    Matrix_RotateXFApply(this->curRot.x);
    Matrix_RotateZS(this->actor.shape.rot.z, MTXMODE_APPLY);
    Matrix_RotateYS(this->actor.shape.rot.y, MTXMODE_APPLY);
    Matrix_Scale(this->actor.scale.x, this->actor.scale.y, this->actor.scale.z, MTXMODE_APPLY);
    SkelAnime_DrawFlexOpa(play, this->skelAnime.skeleton, this->skelAnime.jointTable, this->skelAnime.dListCount,
                          EnRacedog_OverrideLimbDraw, EnRacedog_PostLimbDraw, &this->actor);

    CLOSE_DISPS(play->state.gfxCtx);
}

void EnRacedog_Reset(void) {
    sNumberOfDogsFinished = 0;
    sFirstPlaceIndex = -1;
    sSprintTimer = 0;
    sFurthestPoint = -1;
}
