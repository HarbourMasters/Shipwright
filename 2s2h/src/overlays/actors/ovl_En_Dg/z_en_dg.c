/*
 * File: z_en_dg.c
 * Overlay: ovl_En_Dg
 * Description: Dog
 */

#include "z_en_dg.h"
#include "overlays/actors/ovl_En_Aob_01/z_en_aob_01.h"
#include "2s2h/BenGui/CosmeticEditor.h"
#include <libultraship/bridge/consolevariablebridge.h>

#define FLAGS \
    (ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_FRIENDLY | ACTOR_FLAG_UPDATE_CULLING_DISABLED | ACTOR_FLAG_THROW_ONLY)

void EnDg_Init(Actor* thisx, PlayState* play);
void EnDg_Destroy(Actor* thisx, PlayState* play);
void EnDg_Update(Actor* thisx, PlayState* play);
void EnDg_Draw(Actor* thisx, PlayState* play);
void EnDg_Reset(void);

void EnDg_IdleMove(EnDg* this, PlayState* play);
void EnDg_IdleBark(EnDg* this, PlayState* play);
void EnDg_BackAwayFromGoron(EnDg* this, PlayState* play);
void EnDg_RunAwayFromGoron(EnDg* this, PlayState* play);
void EnDg_BarkAtGoron(EnDg* this, PlayState* play);
void EnDg_ApproachPlayerToAttack(EnDg* this, PlayState* play);
void EnDg_SitNextToPlayer(EnDg* this, PlayState* play);
void EnDg_JumpAttack(EnDg* this, PlayState* play);
void EnDg_SetupBremenMaskApproachPlayer(EnDg* this, PlayState* play);
void EnDg_Fall(EnDg* this, PlayState* play);
void EnDg_ApproachPlayer(EnDg* this, PlayState* play);
void EnDg_SlowlyBackUpBeforeAttacking(EnDg* this, PlayState* play);
void EnDg_BackAwayFromPlayer(EnDg* this, PlayState* play);
void EnDg_BarkAtPlayer(EnDg* this, PlayState* play);
void EnDg_Swim(EnDg* this, PlayState* play);
void EnDg_JumpOutOfWater(EnDg* this, PlayState* play);
void EnDg_Held(EnDg* this, PlayState* play);
void EnDg_Thrown(EnDg* this, PlayState* play);
void EnDg_SetupTalk(EnDg* this, PlayState* play);
void EnDg_Talk(EnDg* this, PlayState* play);

ActorProfile En_Dg_Profile = {
    /**/ ACTOR_EN_DG,
    /**/ ACTORCAT_ENEMY,
    /**/ FLAGS,
    /**/ OBJECT_DOG,
    /**/ sizeof(EnDg),
    /**/ EnDg_Init,
    /**/ EnDg_Destroy,
    /**/ EnDg_Update,
    /**/ EnDg_Draw,
    /**/ EnDg_Reset,
};

#define DOG_FLAG_NONE 0
#define DOG_FLAG_HELD (1 << 0)
#define DOG_FLAG_JUMP_ATTACKING (1 << 1)
#define DOG_FLAG_SWIMMING (1 << 2)
#define DOG_FLAG_BOUNCED (1 << 3)
#define DOG_FLAG_THROWN (1 << 4)
#define DOG_FLAG_FOLLOWING_BREMEN_MASK (1 << 5)

#define ENDG_INDEX_NO_BREMEN_MASK_FOLLOWER 99

typedef enum {
    /* 0 */ DOG_GRAB_STATE_NONE,
    /* 1 */ DOG_GRAB_STATE_HELD,
    /* 2 */ DOG_GRAB_STATE_THROWN_OR_SITTING_AFTER_THROW
} DogGrabState;

typedef enum {
    /* 0 */ DOG_BEHAVIOR_INITIAL, // Gets immediately replaced by DOG_BEHAVIOR_DEFAULT in EnDg_Update
    /* 1 */ DOG_BEHAVIOR_HUMAN,   // Gets immediately replaced by DOG_BEHAVIOR_DEFAULT in EnDg_Update
    /* 2 */ DOG_BEHAVIOR_GORON,
    /* 3 */ DOG_BEHAVIOR_GORON_WAIT,
    /* 4 */ DOG_BEHAVIOR_ZORA,
    /* 5 */ DOG_BEHAVIOR_ZORA_WAIT,
    /* 6 */ DOG_BEHAVIOR_DEKU,
    /* 7 */ DOG_BEHAVIOR_DEKU_WAIT,
    /* 8 */ DOG_BEHAVIOR_DEFAULT
} DogBehavior;

static u8 sIsAnyDogHeld = false;

static s16 sBremenMaskFollowerIndex = ENDG_INDEX_NO_BREMEN_MASK_FOLLOWER;

/**
 * Stores the state for the dogs milling about at the Doggy Racetrack.
 */
typedef struct {
    /* 0x0 */ s16 color;  // The dog's color, which is used as an index into sBaseSpeeds
    /* 0x2 */ s16 index;  // The dog's index within sDogInfo
    /* 0x4 */ s16 textId; // The ID of the text to display when the dog is picked up
} RacetrackDogInfo;       // size = 0x6

/**
 * A table of RacetrackDogInfo for every dog at the Doggy Racetrack. Note that the textId values
 * in this table are updated by functions within this actor.
 */
static RacetrackDogInfo sRacetrackDogInfo[] = {
    { DOG_COLOR_BEIGE, 0, 0x3539 },  { DOG_COLOR_WHITE, 1, 0x353A },  { DOG_COLOR_BLUE, 2, 0x353B },
    { DOG_COLOR_GRAY, 3, 0x353C },   { DOG_COLOR_BROWN, 4, 0x3538 },  { DOG_COLOR_GRAY, 5, 0x353E },
    { DOG_COLOR_BEIGE, 6, 0x353F },  { DOG_COLOR_WHITE, 7, 0x3540 },  { DOG_COLOR_WHITE, 8, 0x3541 },
    { DOG_COLOR_GOLD, 9, 0x3542 },   { DOG_COLOR_GRAY, 10, 0x3543 },  { DOG_COLOR_BEIGE, 11, 0x3544 },
    { DOG_COLOR_WHITE, 12, 0x3545 }, { DOG_COLOR_BROWN, 13, 0x3546 },
};

/**
 * Stores the RacetrackDogInfo for the dog that is selected by the player. These values are just
 * placeholders, and the actual value gets grabbed from sRacetrackDogInfo in EnDg_TryPickUp.
 */
static RacetrackDogInfo sSelectedRacetrackDogInfo = { DOG_COLOR_DEFAULT, -1, 0x353E };

static ColliderCylinderInit sCylinderInit = {
    {
        COL_MATERIAL_NONE,
        AT_ON | AT_TYPE_ENEMY,
        AC_ON | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_1,
        COLSHAPE_CYLINDER,
    },
    {
        ELEM_MATERIAL_UNK1,
        { 0xF7CFFFFF, 0x04, 0x00 },
        { 0xF7CFFFFF, 0x00, 0x00 },
        ATELEM_ON | ATELEM_SFX_NORMAL,
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
    /*  0 */ DOG_ANIM_WALK_AFTER_TALKING,
    /*  1 */ DOG_ANIM_WALK,
    /*  2 */ DOG_ANIM_RUN,
    /*  3 */ DOG_ANIM_BARK,
    /*  4 */ DOG_ANIM_SIT_DOWN_ONCE, // unused
    /*  5 */ DOG_ANIM_SIT_DOWN,
    /*  6 */ DOG_ANIM_LYING_DOWN_START_1, // unused
    /*  7 */ DOG_ANIM_LYING_DOWN_LOOP,    // unused
    /*  8 */ DOG_ANIM_LYING_DOWN_START_2, // unused
    /*  9 */ DOG_ANIM_LYING_DOWN_START_3, // unused
    /* 10 */ DOG_ANIM_LYING_DOWN_START_4, // unused
    /* 11 */ DOG_ANIM_WALK_BACKWARDS,
    /* 12 */ DOG_ANIM_JUMP,
    /* 13 */ DOG_ANIM_LONG_JUMP, // unused
    /* 14 */ DOG_ANIM_JUMP_ATTACK,
    /* 15 */ DOG_ANIM_SWIM,
    /* 16 */ DOG_ANIM_MAX
} DogAnimation;

static AnimationInfoS sAnimationInfo[DOG_ANIM_MAX] = {
    { &gDogWalkAnim, 1.0f, 0, -1, ANIMMODE_LOOP, 0 },           // DOG_ANIM_WALK_AFTER_TALKING
    { &gDogWalkAnim, 1.0f, 0, -1, ANIMMODE_LOOP, -6 },          // DOG_ANIM_WALK
    { &gDogRunAnim, 1.0f, 0, -1, ANIMMODE_LOOP, 0 },            // DOG_ANIM_RUN
    { &gDogBarkAnim, 1.0f, 0, -1, ANIMMODE_LOOP, -6 },          // DOG_ANIM_BARK
    { &gDogSitAnim, 1.0f, 0, -1, ANIMMODE_ONCE, -6 },           // DOG_ANIM_SIT_DOWN_ONCE
    { &gDogSitAnim, 1.0f, 0, -1, ANIMMODE_LOOP_PARTIAL, -6 },   // DOG_ANIM_SIT_DOWN
    { &gDogLyingDownAnim, 1.0f, 0, -1, ANIMMODE_ONCE, -6 },     // DOG_ANIM_LYING_DOWN_START_1
    { &gDogLyingDownLoopAnim, 1.0f, 0, -1, ANIMMODE_LOOP, -6 }, // DOG_ANIM_LYING_DOWN_LOOP
    { &gDogLyingDownAnim, 1.0f, 0, 27, ANIMMODE_ONCE, -6 },     // DOG_ANIM_LYING_DOWN_START_2
    { &gDogLyingDownAnim, 1.0f, 28, -1, ANIMMODE_ONCE, -6 },    // DOG_ANIM_LYING_DOWN_START_3
    { &gDogLyingDownAnim, 1.0f, 54, 54, ANIMMODE_ONCE, -6 },    // DOG_ANIM_LYING_DOWN_START_4
    { &gDogWalkAnim, -1.5f, -1, 0, ANIMMODE_LOOP, -6 },         // DOG_ANIM_WALK_BACKWARDS
    { &gDogJumpAnim, 1.0f, 0, -1, ANIMMODE_ONCE, 0 },           // DOG_ANIM_JUMP
    { &gDogLongJumpAnim, 1.2f, 0, -1, ANIMMODE_ONCE, 0 },       // DOG_ANIM_LONG_JUMP
    { &gDogJumpAttackAnim, 1.2f, 0, -1, ANIMMODE_ONCE, 0 },     // DOG_ANIM_JUMP_ATTACK
    { &gDogWalkAnim, 0.5f, 0, -1, ANIMMODE_LOOP, 0 },           // DOG_ANIM_SWIM
};

static InitChainEntry sInitChain[] = {
    ICHAIN_F32(cullingVolumeDistance, 1000, ICHAIN_STOP),
};

void EnDg_ChangeAnim(SkelAnime* skelAnime, AnimationInfoS* animInfo, s32 animIndex) {
    f32 endFrame;

    animInfo += animIndex;
    if (animInfo->frameCount < 0) {
        endFrame = Animation_GetLastFrame(animInfo->animation);
    } else {
        endFrame = animInfo->frameCount;
    }

    Animation_Change(skelAnime, animInfo->animation, animInfo->playSpeed + (BREG(88) * 0.1f), animInfo->startFrame,
                     endFrame, animInfo->mode, animInfo->morphFrames);
}

void EnDg_UpdateCollision(EnDg* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    this->collider.dim.pos.x = this->actor.world.pos.x;
    this->collider.dim.pos.y = this->actor.world.pos.y;
    this->collider.dim.pos.z = this->actor.world.pos.z;
    Collider_UpdateCylinder(&this->actor, &this->collider);

    if ((player->transformation == PLAYER_FORM_DEKU) && (this->actionFunc == EnDg_JumpAttack)) {
        CollisionCheck_SetAT(play, &play->colChkCtx, &this->collider.base);
    } else {
        Collider_ResetCylinderAT(play, &this->collider.base);
    }

    // The check for DOG_FLAG_JUMP_ATTACKING here makes it so the dog passes through the
    // player if it hits them with their jump attack.
    if ((this->grabState != DOG_GRAB_STATE_HELD) && !(this->dogFlags & DOG_FLAG_JUMP_ATTACKING)) {
        CollisionCheck_SetOC(play, &play->colChkCtx, &this->collider.base);
    } else {
        Collider_ResetCylinderOC(play, &this->collider.base);
    }

    Actor_UpdateBgCheckInfo(play, &this->actor, 26.0f, 10.0f, 0.0f, UPDBGCHECKINFO_FLAG_1 | UPDBGCHECKINFO_FLAG_4);
}

void EnDg_GetFloorRot(EnDg* this, Vec3f* floorRot) {
    f32 nx;
    f32 ny;
    f32 nz;

    if (this->actor.floorPoly != NULL) {
        nx = COLPOLY_GET_NORMAL(this->actor.floorPoly->normal.x);
        ny = COLPOLY_GET_NORMAL(this->actor.floorPoly->normal.y);
        nz = COLPOLY_GET_NORMAL(this->actor.floorPoly->normal.z);

        sinf(0.0f);
        cosf(0.0f);
        floorRot->x = -Math_Atan2F_XY(1.0f, -nz * ny);
        floorRot->z = Math_Atan2F_XY(1.0f, -nx * ny);
    }
}

s32 EnDg_HasReachedPoint(EnDg* this, Path* path, s32 pointIndex) {
    Vec3s* points = Lib_SegmentedToVirtual(path->points);
    s32 count = path->count;
    s32 index = pointIndex;
    s32 reached = false;
    f32 diffX;
    f32 diffZ;
    f32 px;
    f32 pz;
    f32 d;
    Vec3f point;

    Math_Vec3s_ToVec3f(&point, &points[index]);

    if (index == 0) {
        diffX = points[1].x - points[0].x;
        diffZ = points[1].z - points[0].z;
    } else if (index == (count - 1)) {
        diffX = points[count - 1].x - points[count - 2].x;
        diffZ = points[count - 1].z - points[count - 2].z;
    } else {
        diffX = points[index + 1].x - points[index - 1].x;
        diffZ = points[index + 1].z - points[index - 1].z;
    }

    Math3D_RotateXZPlane(&point, RAD_TO_BINANG(Math_FAtan2F(diffX, diffZ)), &px, &pz, &d);

    if (((px * this->actor.world.pos.x) + (pz * this->actor.world.pos.z) + d) > 0.0f) {
        reached = true;
    }

    return reached;
}

/**
 * Returns the Y-rotation the dog should have to move to the next point along its path.
 */
s16 EnDg_GetYRotation(Path* path, s32 index, Vec3f* pos, f32* distSq) {
    Vec3s* points;
    f32 diffX;
    f32 diffZ;

    if (path != NULL) {
        points = Lib_SegmentedToVirtual(path->points);
        points = &points[index];
        diffX = points->x - pos->x;
        diffZ = points->z - pos->z;
    } else {
        diffX = 0.0f;
        diffZ = 0.0f;
    }

    *distSq = SQ(diffX) + SQ(diffZ);

    return RAD_TO_BINANG(Math_Atan2F_XY(diffZ, diffX));
}

/**
 * Updates the dog's Y-rotation and speed so that it moves along its path.
 */
void EnDg_MoveAlongPath(EnDg* this, PlayState* play) {
    s16 yRotation;
    f32 distSq;

    if (this->path != NULL) {
        yRotation = EnDg_GetYRotation(this->path, this->currentPoint, &this->actor.world.pos, &distSq);
        if (this->actor.bgCheckFlags & BGCHECKFLAG_WALL) {
            yRotation = this->actor.wallYaw;
        }

        Math_SmoothStepToS(&this->actor.world.rot.y, yRotation, 4, 0x3E8, 1);
        this->actor.shape.rot.y = this->actor.world.rot.y;
        if (EnDg_HasReachedPoint(this, this->path, this->currentPoint)) {
            if (this->currentPoint >= (this->path->count - 1)) {
                this->currentPoint = 0;
            } else {
                this->currentPoint++;
            }
        }

        if ((this->index == ENDG_INDEX_SWAMP_SPIDER_HOUSE) ||
            ((this->index == ENDG_INDEX_ROMANI_RANCH) && (play->sceneId == SCENE_OMOYA))) {
            Math_ApproachF(&this->actor.speed, 1.0f, 0.2f, 1.0f);
        } else if (this->index == ENDG_INDEX_ROMANI_RANCH) {
            Math_ApproachF(&this->actor.speed, 3.5f, 0.2f, 1.0f);
        } else if (play->sceneId == SCENE_CLOCKTOWER) {
            Math_ApproachF(&this->actor.speed, 3.5f, 0.2f, 1.0f);
        } else if (sRacetrackDogInfo[this->index].textId & 0x11) {
            Math_ApproachF(&this->actor.speed, 1.0f, 0.2f, 1.0f);
        } else {
            Math_ApproachF(&this->actor.speed, 3.5f, 0.2f, 1.0f);
        }
    } else {
        Actor_Kill(&this->actor);
    }
}

void EnDg_SpawnFloorDustRing(EnDg* this, PlayState* play) {
    s16 curFrame = this->skelAnime.curFrame;
    s16 mod = (this->actor.speed > 6.0f) ? 2 : 3;
    Vec3f pos;

    if (((this->index + curFrame) % mod) == 0) {
        pos.x = Rand_CenteredFloat(15.0f) + this->actor.world.pos.x;
        pos.y = this->actor.world.pos.y;
        pos.z = Rand_CenteredFloat(15.0f) + this->actor.world.pos.z;
        Actor_SpawnFloorDustRing(play, &this->actor, &pos, 10.0f, 0, 2.0f, 300, 0, true);
    }
}

void EnDg_PlaySfxWalk(EnDg* this) {
    if (Animation_OnFrame(&this->skelAnime, 1.0f) || Animation_OnFrame(&this->skelAnime, 7.0f)) {
        Actor_PlaySfx(&this->actor, NA_SE_EV_MONKEY_WALK);
    }
}

void EnDg_PlaySfxBark(EnDg* this, f32 frame) {
    if (Animation_OnFrame(&this->skelAnime, frame)) {
        Actor_PlaySfx(&this->actor, NA_SE_EV_SMALL_DOG_BARK);
    }
}

void EnDg_PlaySfxAngryBark(EnDg* this, f32 frame) {
    if (Animation_OnFrame(&this->skelAnime, frame)) {
        Actor_PlaySfx(&this->actor, NA_SE_EV_SMALL_DOG_ANG_BARK);
    }
}

void EnDg_PlaySfxWhine(EnDg* this) {
    if (Animation_OnFrame(&this->skelAnime, 23.0f) || Animation_OnFrame(&this->skelAnime, 28.0f)) {
        Actor_PlaySfx(&this->actor, NA_SE_EV_SMALL_DOG_WHINE);
    }
}

void EnDg_PlaySfxGrowl(EnDg* this, f32 frame) {
    if (Animation_OnFrame(&this->skelAnime, frame)) {
        Actor_PlaySfx(&this->actor, NA_SE_EV_SMALL_DOG_GROAN);
    }
}

void EnDg_SetupIdleMove(EnDg* this, PlayState* play) {
    if (!(this->actor.bgCheckFlags & BGCHECKFLAG_WATER)) {
        if ((this->index == ENDG_INDEX_SWAMP_SPIDER_HOUSE) ||
            ((this->index == ENDG_INDEX_ROMANI_RANCH) && (play->sceneId == SCENE_OMOYA))) {
            EnDg_ChangeAnim(&this->skelAnime, sAnimationInfo, DOG_ANIM_WALK);
        } else if (this->index == ENDG_INDEX_ROMANI_RANCH) {
            EnDg_ChangeAnim(&this->skelAnime, sAnimationInfo, DOG_ANIM_RUN);
        } else if (play->sceneId == SCENE_CLOCKTOWER) {
            EnDg_ChangeAnim(&this->skelAnime, sAnimationInfo, DOG_ANIM_RUN);
        } else if (sRacetrackDogInfo[this->index].textId & 0x11) {
            //! @bug: There is no bounds check on sRacetrackDogInfo access.
            //! The dog in the Romani Ranch credits uses params of 0x03FF which means an index equal to
            //! `ENDG_INDEX_SOUTH_CLOCK_TOWN`. Since the above condition just checks the scene not the index, this
            //! results in an OOB access of `sRacetrackDogInfo` in this condition. With IDO, the OOB access results in
            //! this condition evaluating as true and the dog uses the walking animation with morph frames. Due to this,
            //! and since the dog doesn't update in the credits due to being considered an enemy, it ends up being
            //! upside down. It isn't certain but it is speculated its default pose is upside down as well, so when
            //! morphing from no animation it gets drawn upside down.
            EnDg_ChangeAnim(&this->skelAnime, sAnimationInfo, DOG_ANIM_WALK);
        } else {
            EnDg_ChangeAnim(&this->skelAnime, sAnimationInfo, DOG_ANIM_RUN);
        }

        this->actionFunc = EnDg_IdleMove;
    }
}

/**
 * Updates the text ID in sRacetrackDogInfo based on what was set in the weekEventRegs by
 * En_Aob_01. This makes it so the proper message can be displayed when the player picks up
 * the dog with the Mask of Truth equipped.
 */
void EnDg_UpdateTextId(EnDg* this) {
    if (this->index < RACEDOG_COUNT) {
        // This will produce a text ID in the range of 0x3538 to 0x3545.
        sRacetrackDogInfo[this->index].textId = GET_WEEKEVENTREG_DOG_RACE_TEXT(this->index, 0x3538);
    } else {
        Actor_Kill(&this->actor);
    }

    // As a sanity check, this makes sure the text ID is something in the expected range of 0x3538 to 0x3546.
    if ((sRacetrackDogInfo[this->index].textId > 0x3546) || (sRacetrackDogInfo[this->index].textId < 0x3538)) {
        sRacetrackDogInfo[this->index].textId = 0x353E;
    }

    // Text ID 0x353D is the text for the Romani Ranch dog, which is in the middle of the race dog block. If
    // the dog ends up with this text ID, adjust it so the dog says a different message indcating it's in
    // good condition.
    if (sRacetrackDogInfo[this->index].textId == 0x353D) {
        sRacetrackDogInfo[this->index].textId = 0x3538;
    }
}

void EnDg_StartTextBox(EnDg* this, PlayState* play) {
    if (this->index == ENDG_INDEX_SWAMP_SPIDER_HOUSE) {
        if (CURRENT_DAY == 1) {
            // Stupid master...
            Message_StartTextbox(play, 0x91C, NULL);
        } else {
            // I miss the ranch...
            Message_StartTextbox(play, 0x91E, NULL);
        }
    } else if ((this->index >= ENDG_INDEX_RACETRACK_0) && (this->index <= ENDG_INDEX_RACETRACK_13)) {
        Message_StartTextbox(play, sRacetrackDogInfo[this->index].textId, NULL);
    } else if (this->index == ENDG_INDEX_ROMANI_RANCH) {
        // I wonder if I can stop practicing...
        Message_StartTextbox(play, 0x353D, NULL);
    } else {
        // Where did that Deku Scrub go?
        Message_StartTextbox(play, 0x627, NULL);
    }
}

/**
 * Checks to see if the player picked up the dog and sets the appropriate state if they did.
 */
void EnDg_TryPickUp(EnDg* this, PlayState* play) {
    if (sIsAnyDogHeld && !(this->dogFlags & DOG_FLAG_HELD)) {
        this->actor.flags |= ACTOR_FLAG_LOCK_ON_DISABLED;
        this->dogFlags |= DOG_FLAG_HELD;
    } else if (!sIsAnyDogHeld && (this->dogFlags & DOG_FLAG_HELD)) {
        this->actor.flags &= ~ACTOR_FLAG_LOCK_ON_DISABLED;
        this->dogFlags &= ~DOG_FLAG_HELD;
    }

    if (Actor_HasParent(&this->actor, play)) {
        Actor_PlaySfx(&this->actor, NA_SE_EV_SMALL_DOG_BARK);
        this->grabState = DOG_GRAB_STATE_HELD;
        if (this->index < RACEDOG_COUNT) {
            sSelectedRacetrackDogInfo = sRacetrackDogInfo[this->index];
        }
        if (!sIsAnyDogHeld) {
            this->actor.flags |= ACTOR_FLAG_LOCK_ON_DISABLED;
            sIsAnyDogHeld = true;
            this->dogFlags |= DOG_FLAG_HELD;
        }

        EnDg_ChangeAnim(&this->skelAnime, sAnimationInfo, DOG_ANIM_SIT_DOWN);
        this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
        this->actor.speed = 0.0f;
        if (Player_GetMask(play) == PLAYER_MASK_TRUTH) {
            this->actor.flags |= ACTOR_FLAG_TALK_OFFER_AUTO_ACCEPTED;
            Actor_OfferTalk(&this->actor, play, 100.0f);
            this->actionFunc = EnDg_SetupTalk;
        } else {
            this->actionFunc = EnDg_Held;
        }
    } else {
        Actor_OfferCarry(&this->actor, play);
    }
}

/**
 * Returns true if it finds a dog to follow the player around while they're marching with
 * the Bremen Mask. The index of the dog it finds is stored in sBremenMaskFollowerIndex.
 */
s32 EnDg_FindFollowerForBremenMask(PlayState* play) {
    Actor* enemy = play->actorCtx.actorLists[ACTORCAT_ENEMY].first;
    f32 minDist = 9999.0f;
    f32 dist;

    while (enemy != NULL) {
        if (enemy->id == ACTOR_EN_DG) {
            if (enemy->isLockedOn) {
                sBremenMaskFollowerIndex = ((EnDg*)enemy)->index;
                return true;
            }

            dist = enemy->xzDistToPlayer;
            if (dist < minDist) {
                sBremenMaskFollowerIndex = ((EnDg*)enemy)->index;
                minDist = dist;
            }
        }

        enemy = enemy->next;
    }

    if (sBremenMaskFollowerIndex != ENDG_INDEX_NO_BREMEN_MASK_FOLLOWER) {
        return true;
    }

    return false;
}

/**
 * This checks for two conditions:
 * - If the player has started marching with the Bremen Mask, then it makes the dog
 *   start following the player around (assuming it's the best candidate found in
 *   EnDg_FindFollowerForBremenMask above).
 * - If the player has stopped marching with the Bremen Mask, and this dog was
 *   following them around, then it returns the dog to its idle state.
 */
void EnDg_CheckForBremenMaskMarch(EnDg* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    if (player->stateFlags3 & PLAYER_STATE3_20000000) {
        if (sBremenMaskFollowerIndex == ENDG_INDEX_NO_BREMEN_MASK_FOLLOWER) {
            EnDg_FindFollowerForBremenMask(play);
        }

        if (this->index == sBremenMaskFollowerIndex) {
            if (!(this->dogFlags & DOG_FLAG_FOLLOWING_BREMEN_MASK)) {
                this->dogFlags |= DOG_FLAG_FOLLOWING_BREMEN_MASK;
                EnDg_ChangeAnim(&this->skelAnime, sAnimationInfo, DOG_ANIM_WALK);
                this->actionFunc = EnDg_SetupBremenMaskApproachPlayer;
            } else if ((this->actionFunc == EnDg_ApproachPlayer) || (this->actionFunc == EnDg_SitNextToPlayer)) {
                if (DECR(this->bremenBarkTimer) == 0) {
                    this->bremenBarkTimer = 10;
                    Actor_PlaySfx(&this->actor, NA_SE_EV_SMALL_DOG_BARK);
                }
            }
        }
    } else if (this->index == sBremenMaskFollowerIndex) {
        this->dogFlags &= ~DOG_FLAG_FOLLOWING_BREMEN_MASK;
        sBremenMaskFollowerIndex = ENDG_INDEX_NO_BREMEN_MASK_FOLLOWER;
        EnDg_SetupIdleMove(this, play);
        this->actionFunc = EnDg_IdleMove;
    }
}

/**
 * Returns true if the player is not human and the dog is close enough to start
 * reacting to their current form.
 */
s32 EnDg_ShouldReactToNonHumanPlayer(EnDg* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    switch (player->transformation) {
        case PLAYER_FORM_GORON:
        case PLAYER_FORM_ZORA:
            if (this->actor.xzDistToPlayer < 300.0f) {
                return true;
            }
            // fallthrough
        case PLAYER_FORM_DEKU:
            if (this->actor.xzDistToPlayer < 250.0f) {
                return true;
            }
            break;
    }

    return false;
}

/**
 * Sets the appropriate actionFunc given the player's current form and the dog's current behavior.
 * Specifically, it makes sure that the dog doesn't start reacting to the player's form until the
 * player starts moving a bit. This was presumably done so the player could more easily react to
 * the dog changing its behavior; it would be strange if the dog attacked the player the instant
 * they put on the Deku Mask, for example.
 */
void EnDg_ChooseActionForForm(EnDg* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    if (!(this->actor.bgCheckFlags & BGCHECKFLAG_WATER)) {
        switch (player->transformation) {
            case PLAYER_FORM_HUMAN:
                if (this->behavior != DOG_BEHAVIOR_HUMAN) {
                    this->behavior = DOG_BEHAVIOR_HUMAN;
                    this->dogFlags &= ~DOG_FLAG_JUMP_ATTACKING;
                    EnDg_SetupIdleMove(this, play);
                }
                break;

            case PLAYER_FORM_ZORA:
                this->dogFlags &= ~DOG_FLAG_JUMP_ATTACKING;
                if ((this->behavior != DOG_BEHAVIOR_ZORA) && (player->actor.speed > 1.0f)) {
                    this->behavior = DOG_BEHAVIOR_ZORA;
                    EnDg_ChangeAnim(&this->skelAnime, sAnimationInfo, DOG_ANIM_RUN);
                    this->actionFunc = EnDg_ApproachPlayer;
                }

                if ((this->behavior != DOG_BEHAVIOR_ZORA_WAIT) && (this->behavior != DOG_BEHAVIOR_ZORA)) {
                    this->behavior = DOG_BEHAVIOR_ZORA_WAIT;
                    EnDg_SetupIdleMove(this, play);
                }
                break;

            case PLAYER_FORM_GORON:
                this->dogFlags &= ~DOG_FLAG_JUMP_ATTACKING;
                if ((this->behavior != DOG_BEHAVIOR_GORON) && (player->actor.speed > 1.0f)) {
                    this->behavior = DOG_BEHAVIOR_GORON;
                    EnDg_ChangeAnim(&this->skelAnime, sAnimationInfo, DOG_ANIM_WALK_BACKWARDS);
                    this->timer = 50;
                    this->actionFunc = EnDg_BackAwayFromGoron;
                }

                if ((this->behavior != DOG_BEHAVIOR_GORON_WAIT) && (this->behavior != DOG_BEHAVIOR_GORON)) {
                    this->behavior = DOG_BEHAVIOR_GORON_WAIT;
                    EnDg_SetupIdleMove(this, play);
                }
                break;

            case PLAYER_FORM_DEKU:
                this->dogFlags &= ~DOG_FLAG_JUMP_ATTACKING;
                if ((this->behavior != DOG_BEHAVIOR_DEKU) && (player->actor.speed > 1.0f)) {
                    this->behavior = DOG_BEHAVIOR_DEKU;
                    EnDg_ChangeAnim(&this->skelAnime, sAnimationInfo, DOG_ANIM_RUN);
                    this->actionFunc = EnDg_ApproachPlayerToAttack;
                }

                if ((this->behavior != DOG_BEHAVIOR_DEKU_WAIT) && (this->behavior != DOG_BEHAVIOR_DEKU)) {
                    this->behavior = DOG_BEHAVIOR_DEKU_WAIT;
                    EnDg_SetupIdleMove(this, play);
                }
                break;
        }
    }
}

/**
 * Moves the dog along its path, stopping to bark at random intervals.
 */
void EnDg_IdleMove(EnDg* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    this->actor.velocity.y = 0.0f;
    this->actor.gravity = -3.0f;
    EnDg_MoveAlongPath(this, play);
    Actor_MoveWithGravity(&this->actor);
    if (player->transformation == PLAYER_FORM_HUMAN) {
        EnDg_TryPickUp(this, play);
    }

    EnDg_CheckForBremenMaskMarch(this, play);
    EnDg_PlaySfxWalk(this);
    if (!(this->actor.bgCheckFlags & BGCHECKFLAG_GROUND)) {
        this->actionFunc = EnDg_Fall;
    }

    if (DECR(this->timer) == 0) {
        this->timer = Rand_S16Offset(20, 20);
        EnDg_ChangeAnim(&this->skelAnime, sAnimationInfo, DOG_ANIM_BARK);
        this->actionFunc = EnDg_IdleBark;
    }
}

/**
 * Stops and barks, before returning to moving along its path.
 */
void EnDg_IdleBark(EnDg* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    if (player->transformation == PLAYER_FORM_HUMAN) {
        EnDg_TryPickUp(this, play);
    }

    EnDg_PlaySfxBark(this, 13.0f);
    EnDg_PlaySfxBark(this, 19.0f);

    if (!(this->actor.bgCheckFlags & BGCHECKFLAG_GROUND)) {
        this->actionFunc = EnDg_Fall;
    }

    if (DECR(this->timer) == 0) {
        this->timer = Rand_S16Offset(60, 60);
        EnDg_SetupIdleMove(this, play);
    }
}

/**
 * Slowly back away from the player while growling. After a short time backing away,
 * this function will make the dog away from the player.
 */
void EnDg_BackAwayFromGoron(EnDg* this, PlayState* play) {
    this->actor.velocity.y = 0.0f;
    this->actor.gravity = -3.0f;

    if (DECR(this->timer) == 0) {
        this->timer = 50;
        EnDg_ChangeAnim(&this->skelAnime, sAnimationInfo, DOG_ANIM_RUN);
        this->actionFunc = EnDg_RunAwayFromGoron;
    } else {
        Math_SmoothStepToS(&this->actor.shape.rot.y, this->actor.yawTowardsPlayer, 4, 0x3E8, 1);
        if (this->actor.bgCheckFlags & BGCHECKFLAG_WALL) {
            this->actor.shape.rot.y = this->actor.wallYaw;
        }

        this->actor.world.rot.y = this->actor.shape.rot.y;
        Math_ApproachF(&this->actor.speed, -1.5f, 0.2f, 1.0f);
        Actor_MoveWithGravity(&this->actor);
    }

    EnDg_PlaySfxWalk(this);
    EnDg_PlaySfxGrowl(this, 0);
}

/**
 * Quickly run away from the player. Once the dog is far enough from the player,
 * this function will make them start barking at them.
 */
void EnDg_RunAwayFromGoron(EnDg* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    s16 yRotation;

    if (this->actor.xzDistToPlayer < 250.0f) {
        Math_ApproachS(&this->actor.shape.rot.y, -this->actor.yawTowardsPlayer, 4, 0xC00);

        if (this->actor.bgCheckFlags & BGCHECKFLAG_WALL) {
            yRotation = this->actor.wallYaw;
        } else {
            yRotation = 0;
        }

        Math_SmoothStepToS(&this->actor.world.rot.y, yRotation, 4, 0x3E8, 1);
        this->actor.world.rot.y = this->actor.shape.rot.y;
        if (player->actor.speed != 0.0f) {
            Math_ApproachF(&this->actor.speed, player->actor.speed, 0.2f, 1.0f);
        } else {
            Math_ApproachF(&this->actor.speed, 3.5f, 0.2f, 1.0f);
        }
    } else {
        EnDg_ChangeAnim(&this->skelAnime, sAnimationInfo, DOG_ANIM_BARK);
        this->actionFunc = EnDg_BarkAtGoron;
    }

    if (this->actor.speed > 7.0f) {
        this->actor.speed = 7.0f;
    }

    Actor_MoveWithGravity(&this->actor);
    EnDg_PlaySfxWalk(this);

    if (Animation_OnFrame(&this->skelAnime, 3.0f)) {
        Actor_PlaySfx(&this->actor, NA_SE_EV_SMALL_DOG_CRY);
    } else if (Animation_OnFrame(&this->skelAnime, 6.0f)) {
        Actor_PlaySfx(&this->actor, NA_SE_EV_MONKEY_WALK);
    }
}

/**
 * Bark at the player until they get close enough, at which point the dog starts backing away.
 */
void EnDg_BarkAtGoron(EnDg* this, PlayState* play) {
    Math_ApproachS(&this->actor.shape.rot.y, this->actor.yawTowardsPlayer, 4, 0xC00);
    this->actor.world.rot.y = this->actor.shape.rot.y;
    if (this->actor.xzDistToPlayer < 250.0f) {
        EnDg_ChangeAnim(&this->skelAnime, sAnimationInfo, DOG_ANIM_WALK_BACKWARDS);
        this->timer = 50;
        this->actionFunc = EnDg_BackAwayFromGoron;
    }

    EnDg_PlaySfxAngryBark(this, 13.0f);
    EnDg_PlaySfxAngryBark(this, 19.0f);
}

void EnDg_ApproachPlayerToAttack(EnDg* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    this->actor.velocity.y = 0.0f;
    this->actor.gravity = -3.0f;
    if (this->actor.bgCheckFlags & BGCHECKFLAG_WALL) {
        this->actor.shape.rot.y = this->actor.wallYaw;
    }

    this->actor.world.rot.y = this->actor.shape.rot.y;
    if (this->actor.xzDistToPlayer < 70.0f) {
        Math_ApproachZeroF(&this->actor.speed, 0.2f, 1.0f);
        if (Animation_OnFrame(&this->skelAnime, 7.0f)) {
            s16 yawDiff = ABS_ALT(player->actor.shape.rot.y - this->actor.shape.rot.y);

            this->attackTimer = 20;
            if (yawDiff < 0x4000) {
                EnDg_ChangeAnim(&this->skelAnime, sAnimationInfo, DOG_ANIM_JUMP_ATTACK);
                this->actionFunc = EnDg_JumpAttack;
            } else {
                EnDg_ChangeAnim(&this->skelAnime, sAnimationInfo, DOG_ANIM_WALK_BACKWARDS);
                sAnimationInfo[DOG_ANIM_WALK_BACKWARDS].playSpeed = -1.0f;
                this->actionFunc = EnDg_SlowlyBackUpBeforeAttacking;
            }
        }
    } else {
        Math_ApproachS(&this->actor.shape.rot.y, this->actor.yawTowardsPlayer, 4, 0xC00);
        Math_ApproachF(&this->actor.speed, 5.0f, 0.2f, 1.0f);
    }

    Actor_MoveWithGravity(&this->actor);
    EnDg_PlaySfxWalk(this);
    EnDg_PlaySfxAngryBark(this, 5.0f);
}

/**
 * Makes the dog run around for a bit after attacking before it
 * starts to approach the player for another attack.
 */
void EnDg_RunAfterAttacking(EnDg* this, PlayState* play) {
    this->dogFlags &= ~DOG_FLAG_BOUNCED;
    Math_ApproachF(&this->actor.speed, 3.5f, 0.1f, 0.5f);
    Actor_MoveWithGravity(&this->actor);
    if (DECR(this->attackTimer) == 0) {
        this->attackTimer = 20;
        this->actionFunc = EnDg_ApproachPlayerToAttack;
    }

    EnDg_PlaySfxWalk(this);
    EnDg_PlaySfxAngryBark(this, 3.0f);
}

/**
 * Makes the dog sit next to the player until they get too far away, at which point
 * it will attempt to approach them again.
 */
void EnDg_SitNextToPlayer(EnDg* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    if (!(this->actor.bgCheckFlags & BGCHECKFLAG_GROUND)) {
        this->actionFunc = EnDg_Fall;
    }

    if (this->actor.xzDistToPlayer < 50.0f) {
        Math_ApproachS(&this->actor.shape.rot.y, this->actor.yawTowardsPlayer, 4, 0xC00);
        this->actor.world.rot.y = this->actor.shape.rot.y;
    } else {
        if (player->stateFlags3 & PLAYER_STATE3_20000000) {
            EnDg_ChangeAnim(&this->skelAnime, sAnimationInfo, DOG_ANIM_WALK);
        } else {
            EnDg_ChangeAnim(&this->skelAnime, sAnimationInfo, DOG_ANIM_RUN);
        }

        this->actionFunc = EnDg_ApproachPlayer;
    }

    EnDg_CheckForBremenMaskMarch(this, play);

    // If we're in this function, the player is either marching with the Bremen Mask, or
    // they're a Zora. The below code will make the dog repeatedly whine in the latter case.
    if (!(this->dogFlags & DOG_FLAG_FOLLOWING_BREMEN_MASK)) {
        EnDg_PlaySfxWhine(this);
    }
}

void EnDg_JumpAttack(EnDg* this, PlayState* play) {
    s16 curFrame = this->skelAnime.curFrame;

    if ((this->actor.xyzDistToPlayerSq < 800.0f) && (this->collider.base.atFlags & AT_BOUNCED)) {
        this->attackTimer = 60;
        this->dogFlags &= ~DOG_FLAG_JUMP_ATTACKING;
        this->dogFlags |= DOG_FLAG_BOUNCED;
        this->collider.base.atFlags &= ~AT_BOUNCED;
        this->actor.speed *= -1.0f;
        EnDg_ChangeAnim(&this->skelAnime, sAnimationInfo, DOG_ANIM_RUN);
        this->actionFunc = EnDg_RunAfterAttacking;
        return;
    }

    if (DECR(this->attackTimer) == 0) {
        this->attackTimer = 60;
        this->dogFlags &= ~DOG_FLAG_JUMP_ATTACKING;
        EnDg_ChangeAnim(&this->skelAnime, sAnimationInfo, DOG_ANIM_RUN);
        this->actionFunc = EnDg_RunAfterAttacking;
    }

    if (curFrame < 9) {
        if (Animation_OnFrame(&this->skelAnime, 0.0f)) {
            sAnimationInfo[DOG_ANIM_JUMP_ATTACK].playSpeed = Rand_CenteredFloat(1.0f) + 3.0f;
        }

        EnDg_SpawnFloorDustRing(this, play);
    } else {
        this->dogFlags |= DOG_FLAG_JUMP_ATTACKING;
        if (Animation_OnFrame(&this->skelAnime, 9.0f)) {
            f32 rand = Rand_CenteredFloat(1.5f);

            sAnimationInfo[DOG_ANIM_JUMP_ATTACK].playSpeed = 1.2f;
            this->actor.velocity.y = 2.0f * rand + 3.0f;
            this->actor.speed = 8.0f + rand;
        } else if (curFrame > 20) {
            Math_ApproachF(&this->actor.speed, 2.5f, 0.2f, 1.0f);
        }

        if (curFrame > 23) {
            EnDg_SpawnFloorDustRing(this, play);
        }

        Actor_MoveWithGravity(&this->actor);
    }

    EnDg_PlaySfxGrowl(this, 10.0f);
}

/**
 * An unused function that will make the dog walk slowly towards the player until they're
 * in a specific range, at which point they will bark at them. This might be part of an
 * unused or early variation of how dogs react to Goron Link.
 */
void EnDg_WalkToPlayer(EnDg* this, PlayState* play) {
    if (this->actor.xzDistToPlayer < 150.0f) {
        EnDg_ChangeAnim(&this->skelAnime, sAnimationInfo, DOG_ANIM_WALK_BACKWARDS);
        this->actionFunc = EnDg_BackAwayFromPlayer;
    } else if (this->actor.xzDistToPlayer < 200.0f) {
        EnDg_ChangeAnim(&this->skelAnime, sAnimationInfo, DOG_ANIM_BARK);
        this->actionFunc = EnDg_BarkAtPlayer;
    } else {
        Math_ApproachS(&this->actor.shape.rot.y, this->actor.yawTowardsPlayer, 4, 0xC00);
        this->actor.world.rot.y = this->actor.shape.rot.y;
        Math_ApproachF(&this->actor.speed, 2.0f, 0.2f, 1.0f);
        Actor_MoveWithGravity(&this->actor);
    }

    EnDg_PlaySfxWalk(this);
    EnDg_PlaySfxBark(this, 5.0f);
}

void EnDg_SetupBremenMaskApproachPlayer(EnDg* this, PlayState* play) {
    if (func_801A46F8() == 1) {
        this->bremenBarkTimer = 10;
        Actor_PlaySfx(&this->actor, NA_SE_EV_SMALL_DOG_BARK);
        this->actionFunc = EnDg_ApproachPlayer;
    }
}

void EnDg_Fall(EnDg* this, PlayState* play) {
    if (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND) {
        EnDg_ChangeAnim(&this->skelAnime, sAnimationInfo, DOG_ANIM_RUN);
        this->actionFunc = EnDg_IdleMove;
    }

    Actor_MoveWithGravity(&this->actor);
    EnDg_PlaySfxBark(this, 3.0f);
}

/**
 * Approach the player non-aggressively. If the player stops moving, then
 * eventually the dog will sit down next to them.
 */
void EnDg_ApproachPlayer(EnDg* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    this->actor.velocity.y = 0.0f;
    this->actor.gravity = -3.0f;
    if ((this->actor.xzDistToPlayer < 60.0f) && (this->collider.base.ocFlags1 & OC1_HIT)) {
        this->actor.shape.rot.y += 0x71C;
    } else {
        Math_ApproachS(&this->actor.shape.rot.y, this->actor.yawTowardsPlayer, 4, 0xC00);
    }

    this->actor.world.rot.y = this->actor.shape.rot.y;
    if (this->actor.xzDistToPlayer < 40.0f) {
        EnDg_ChangeAnim(&this->skelAnime, sAnimationInfo, DOG_ANIM_SIT_DOWN);
        this->actionFunc = EnDg_SitNextToPlayer;
    } else if (player->stateFlags3 & PLAYER_STATE3_20000000) {
        if ((this->actor.xzDistToPlayer > 40.0f) && (player->speedXZ == 0.0f)) {
            Math_ApproachF(&this->actor.speed, 1.5f, 0.2f, 1.0f);
        } else {
            Math_ApproachF(&this->actor.speed, player->actor.speed, 0.2f, 1.0f);
        }
    } else {
        Math_ApproachF(&this->actor.speed, 3.5f, 0.2f, 1.0f);
    }

    EnDg_CheckForBremenMaskMarch(this, play);
    Actor_MoveWithGravity(&this->actor);
    EnDg_PlaySfxWalk(this);

    // If we're in this function, the player is either marching with the Bremen Mask, or
    // they're a Zora. The below code will make the dog repeatedly bark in the latter case.
    if (!(this->dogFlags & DOG_FLAG_FOLLOWING_BREMEN_MASK)) {
        EnDg_PlaySfxBark(this, 3.0f);
    }
}

/**
 * Makes the dog slowly back up and face the player before performing a jump attack. Used to
 * give the dog a bit of time to correct its angle if they were unlikely to hit the player.
 */
void EnDg_SlowlyBackUpBeforeAttacking(EnDg* this, PlayState* play) {
    if (this->actor.xzDistToPlayer > 72.0f) {
        EnDg_ChangeAnim(&this->skelAnime, sAnimationInfo, DOG_ANIM_JUMP_ATTACK);
        this->actionFunc = EnDg_JumpAttack;
    }

    Math_SmoothStepToS(&this->actor.shape.rot.y, this->actor.yawTowardsPlayer, 4, 0x3E8, 1);
    if (this->actor.bgCheckFlags & BGCHECKFLAG_WALL) {
        this->actor.shape.rot.y = this->actor.wallYaw;
        EnDg_ChangeAnim(&this->skelAnime, sAnimationInfo, DOG_ANIM_JUMP_ATTACK);
        this->actionFunc = EnDg_JumpAttack;
    }

    this->actor.world.rot.y = this->actor.shape.rot.y;
    Math_ApproachF(&this->actor.speed, -1.0f, 0.2f, 1.0f);
    Actor_MoveWithGravity(&this->actor);
    EnDg_PlaySfxWalk(this);
    EnDg_PlaySfxGrowl(this, 4.0f);
}

/**
 * An unused function that will make the dog back away from the player while growling.
 * It is quite similar to EnDg_BackAwayFromGoron; perhaps this is an earlier version of it?
 * Unlike that function, this will make the dog approach the player if they move too far away.
 * It also lacks the behavior where the dog runs away in a panic after a short time.
 */
void EnDg_BackAwayFromPlayer(EnDg* this, PlayState* play) {
    if (this->actor.xzDistToPlayer > 200.0f) {
        EnDg_ChangeAnim(&this->skelAnime, sAnimationInfo, DOG_ANIM_WALK);
        this->actionFunc = EnDg_WalkToPlayer;
    } else if (this->actor.xzDistToPlayer > 150.0f) {
        EnDg_ChangeAnim(&this->skelAnime, sAnimationInfo, DOG_ANIM_BARK);
        this->actionFunc = EnDg_BarkAtPlayer;
    } else {
        Math_SmoothStepToS(&this->actor.shape.rot.y, this->actor.yawTowardsPlayer, 4, 0x3E8, 1);
        if (this->actor.bgCheckFlags & BGCHECKFLAG_WALL) {
            this->actor.shape.rot.y = this->actor.wallYaw;
        }

        this->actor.world.rot.y = this->actor.shape.rot.y;
        Math_ApproachF(&this->actor.speed, -2.0f, 0.2f, 1.0f);
        Actor_MoveWithGravity(&this->actor);
    }

    EnDg_PlaySfxWalk(this);
    Actor_PlaySfx(&this->actor, NA_SE_EV_SMALL_DOG_GROAN);
}

/**
 * An unused function that will make the dog bark at the player endlessly.
 * It is quite similar to EnDg_BarkAtGoron; perhaps this is an earlier version of it?
 * Unlike that function, this will make the dog approach the player if they move too far away.
 * It also mixes in multiple growls into the bark.
 */
void EnDg_BarkAtPlayer(EnDg* this, PlayState* play) {
    if (this->actor.xzDistToPlayer < 150.0f) {
        EnDg_ChangeAnim(&this->skelAnime, sAnimationInfo, DOG_ANIM_WALK_BACKWARDS);
        this->actionFunc = EnDg_BackAwayFromPlayer;
    } else if (this->actor.xzDistToPlayer > 200.0f) {
        EnDg_ChangeAnim(&this->skelAnime, sAnimationInfo, DOG_ANIM_WALK);
        this->actionFunc = EnDg_WalkToPlayer;
    }

    EnDg_PlaySfxGrowl(this, 0.0f);
    EnDg_PlaySfxGrowl(this, 1.0f);
    EnDg_PlaySfxGrowl(this, 2.0f);
    EnDg_PlaySfxGrowl(this, 3.0f);
    EnDg_PlaySfxBark(this, 5.0f);
}

/**
 * Makes a splash effect and plays the splash sfx when the dog enters the water, then
 * makes them start swimming.
 */
void EnDg_SetupSwim(EnDg* this, PlayState* play) {
    Vec3f pos;

    pos.x = this->actor.world.pos.x;
    pos.y = this->actor.world.pos.y + this->actor.depthInWater;
    pos.z = this->actor.world.pos.z + 20.0f;
    EffectSsGSplash_Spawn(play, &pos, NULL, NULL, 0, 800);
    EffectSsGRipple_Spawn(play, &pos, 100, 500, 30);

    if (this->actor.depthInWater > 0.0f) {
        this->actor.gravity = 0.0f;
        this->actor.velocity.y = -3.0f;
        this->swimTimer = 10;
        Actor_PlaySfx(&this->actor, NA_SE_EV_BOMB_DROP_WATER);
        this->actionFunc = EnDg_Swim;
    }

    Math_ApproachF(&this->actor.speed, 1.0f, 0.2f, 1.0f);
    Actor_MoveWithGravity(&this->actor);
}

/**
 * Makes the dog swim until it either:
 * - Touches land, at which point it will just resume its idle movement.
 * - Touches a wall, at which point it will attempt to jump out of the water.
 */
void EnDg_Swim(EnDg* this, PlayState* play) {
    Vec3f ripplePos;
    Vec3f pos;
    s16 yRotation = 0;
    WaterBox* waterBox;
    f32 waterSurface;
    CollisionPoly* poly;
    f32 floorHeight;

    ripplePos.x = this->actor.world.pos.x;
    ripplePos.y = this->actor.world.pos.y + this->actor.depthInWater;
    ripplePos.z = this->actor.world.pos.z + 20.0f;
    pos.x = (Math_SinS(this->actor.world.rot.y) * 50.0f) + this->actor.world.pos.x;
    pos.y = this->actor.home.pos.y + 100.0f;
    pos.z = (Math_CosS(this->actor.world.rot.y) * 50.0f) + this->actor.world.pos.z;

    if (DECR(this->swimTimer) == 0) {
        if (!(this->dogFlags & DOG_FLAG_SWIMMING)) {
            this->dogFlags |= DOG_FLAG_SWIMMING;
            Actor_PlaySfx(&this->actor, NA_SE_EV_OUT_OF_WATER);
        }

        this->swimTimer = 5;
        EffectSsGRipple_Spawn(play, &ripplePos, 100, 500, 30);
    }

    if (this->actor.depthInWater > 15.0f) {
        this->actor.velocity.y = 0.5f;
    } else if (this->actor.depthInWater < 10.0f) {
        this->actor.velocity.y = -0.5f;
    }

    floorHeight = BgCheck_EntityRaycastFloor2(play, &play->colCtx, &poly, &pos);

    // The below code checks *only* that the dog is touching a wall, which can result in
    // some strange behavior if the dog is touching a wall that is too tall for it to jump
    // over. For example, if the player throws the dog at a tall wall next to some water,
    // this code will make the dog "skip" along the water's surface, assuming the floor
    // height is low enough to make it try to jump out.
    if (this->actor.bgCheckFlags & BGCHECKFLAG_WALL) {
        if (!WaterBox_GetSurface1(play, &play->colCtx, pos.x, pos.z, &waterSurface, &waterBox)) {
            if (floorHeight > -100.0f) {
                this->dogFlags &= ~DOG_FLAG_SWIMMING;
                this->actionFunc = EnDg_JumpOutOfWater;
            } else {
                yRotation = this->actor.wallYaw;
            }
        } else if (floorHeight > -100.0f) {
            if (waterSurface < floorHeight) {
                this->dogFlags &= ~DOG_FLAG_SWIMMING;
                this->actionFunc = EnDg_JumpOutOfWater;
            } else {
                yRotation = this->actor.wallYaw;
            }
        } else {
            yRotation = this->actor.wallYaw;
        }
    } else if ((this->actor.bgCheckFlags & BGCHECKFLAG_GROUND) && !(this->actor.bgCheckFlags & BGCHECKFLAG_WATER)) {
        this->actor.gravity = -3.0f;
        this->dogFlags &= ~DOG_FLAG_SWIMMING;
        this->behavior = DOG_BEHAVIOR_DEFAULT;
        this->timer = Rand_S16Offset(60, 60);
        Actor_PlaySfx(&this->actor, NA_SE_EV_OUT_OF_WATER);
        EnDg_ChangeAnim(&this->skelAnime, sAnimationInfo, DOG_ANIM_RUN);
        Math_ApproachF(&this->actor.speed, 3.5f, 0.2f, 1.0f);
        this->actionFunc = EnDg_IdleMove;
    }

    Math_SmoothStepToS(&this->actor.world.rot.y, yRotation, 4, 0x3E8, 1);
    this->actor.shape.rot.y = this->actor.world.rot.y;
    Math_ApproachF(&this->actor.speed, 0.5f, 0.2f, 1.0f);
    Actor_MoveWithGravity(&this->actor);
}

/**
 * This function attempts to get the dog out of water by making it jump near walls.
 * The point of this is seemingly to let the dog escape the shallow water pools in
 * South Clock Town.
 */
void EnDg_JumpOutOfWater(EnDg* this, PlayState* play) {
    Vec3f pos;

    pos.x = this->actor.world.pos.x;
    pos.y = this->actor.world.pos.y + this->actor.depthInWater;
    pos.z = this->actor.world.pos.z + 20.0f;

    if (DECR(this->swimTimer) == 0) {
        this->swimTimer = 5;
        EffectSsGRipple_Spawn(play, &pos, 100, 500, 30);
    }

    if (this->actor.depthInWater > 15.0f) {
        this->actor.velocity.y = 1.0f;
    } else if (this->actor.depthInWater < 10.0f) {
        this->actor.velocity.y = -1.0f;
    }

    if (this->actor.bgCheckFlags & BGCHECKFLAG_WALL) {
        this->actor.world.pos.y = pos.y;
        this->actor.velocity.y = 10.0f;
        EffectSsGSplash_Spawn(play, &pos, NULL, NULL, 0, 800);
    }

    if (!(this->actor.bgCheckFlags & BGCHECKFLAG_WATER)) {
        this->behavior = DOG_BEHAVIOR_DEFAULT;
        this->actor.velocity.y = 10.0f;
        this->actor.gravity = -3.0f;
        this->timer = Rand_S16Offset(60, 60);
        Actor_PlaySfx(&this->actor, NA_SE_EV_OUT_OF_WATER);
        EnDg_ChangeAnim(&this->skelAnime, sAnimationInfo, DOG_ANIM_RUN);
        this->actionFunc = EnDg_IdleMove;
        Math_ApproachF(&this->actor.speed, 3.5f, 0.2f, 1.0f);
    } else {
        Math_ApproachF(&this->actor.speed, 0.5f, 0.2f, 1.0f);
    }

    Actor_MoveWithGravity(&this->actor);
}

void EnDg_Held(EnDg* this, PlayState* play) {
    if (Actor_HasNoParent(&this->actor, play)) {
        this->grabState = DOG_GRAB_STATE_THROWN_OR_SITTING_AFTER_THROW;
        this->actor.flags |= ACTOR_FLAG_ATTENTION_ENABLED;
        if (sIsAnyDogHeld) {
            this->actor.flags &= ~ACTOR_FLAG_LOCK_ON_DISABLED;
            sIsAnyDogHeld = false;
            this->dogFlags &= ~DOG_FLAG_HELD;
        }

        this->selectedDogIndex = -1;
        sSelectedRacetrackDogInfo.index = this->selectedDogIndex;
        this->sitAfterThrowTimer = 100;
        Actor_MoveWithGravity(&this->actor);
        this->dogFlags |= DOG_FLAG_THROWN;
        this->actionFunc = EnDg_Thrown;
    }
}

void EnDg_Thrown(EnDg* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    if (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND) {
        if (this->dogFlags & DOG_FLAG_THROWN) {
            this->dogFlags &= ~DOG_FLAG_THROWN;
            Actor_PlaySfx(&this->actor, NA_SE_EV_MONKEY_WALK);
        }

        this->actor.speed = 0.0f;
        this->actor.gravity = -3.0f;
        if (player->transformation == PLAYER_FORM_HUMAN) {
            EnDg_TryPickUp(this, play);
        }
    }

    if (DECR(this->sitAfterThrowTimer) == 0) {
        this->grabState = DOG_GRAB_STATE_NONE;
        EnDg_SetupIdleMove(this, play);
        this->actionFunc = EnDg_IdleMove;
    }

    Actor_MoveWithGravity(&this->actor);
}

void EnDg_SetupTalk(EnDg* this, PlayState* play) {
    if (Actor_TalkOfferAccepted(&this->actor, &play->state)) {
        this->actor.flags &= ~ACTOR_FLAG_TALK_OFFER_AUTO_ACCEPTED;
        EnDg_StartTextBox(this, play);
        this->actionFunc = EnDg_Talk;
    } else {
        Actor_OfferTalk(&this->actor, play, 100.0f);
    }
}

void EnDg_Talk(EnDg* this, PlayState* play) {
    if (Message_GetState(&play->msgCtx) == TEXT_STATE_CLOSING) {
        EnDg_ChangeAnim(&this->skelAnime, sAnimationInfo, DOG_ANIM_WALK_AFTER_TALKING);
        this->actionFunc = EnDg_Held;
    }
}

void EnDg_Init(Actor* thisx, PlayState* play) {
    EnDg* this = (EnDg*)thisx;
    s32 pad;

    ActorShape_Init(&this->actor.shape, 0.0f, ActorShadow_DrawCircle, 24.0f);
    SkelAnime_InitFlex(play, &this->skelAnime, &gDogSkel, NULL, this->jointTable, this->morphTable, DOG_LIMB_MAX);
    Collider_InitCylinder(play, &this->collider);
    Collider_SetCylinder(play, &this->collider, &this->actor, &sCylinderInit);
    CollisionCheck_SetInfo2(&this->actor.colChkInfo, &sDamageTable, &sColChkInfoInit);
    Actor_ProcessInitChain(&this->actor, sInitChain);

    this->path = SubS_GetPathByIndex(play, ENDG_GET_PATH_INDEX(&this->actor), ENDG_PATH_INDEX_NONE);
    Actor_SetScale(&this->actor, 0.0075f);
    this->actor.attentionRangeType = ATTENTION_RANGE_1;
    this->actor.gravity = -3.0f;
    this->timer = Rand_S16Offset(60, 60);
    this->dogFlags = DOG_FLAG_NONE;
    this->attackTimer = 20;
    this->swimTimer = 10;
    this->index = ENDG_GET_INDEX(&this->actor);
    this->behavior = DOG_BEHAVIOR_INITIAL;
    this->grabState = DOG_GRAB_STATE_NONE;
    if (play->sceneId == SCENE_F01_B) {
        this->sitAfterThrowTimer = 100;
        EnDg_UpdateTextId(this);
    }

    EnDg_SetupIdleMove(this, play);
}

void EnDg_Destroy(Actor* thisx, PlayState* play) {
    EnDg* this = (EnDg*)thisx;

    Collider_DestroyCylinder(play, &this->collider);
}

void EnDg_Update(Actor* thisx, PlayState* play) {
    EnDg* this = (EnDg*)thisx;
    Player* player = GET_PLAYER(play);
    s32 pad;
    Vec3f floorRot = { 0.0f, 0.0f, 0.0f };

    this->selectedDogIndex = sSelectedRacetrackDogInfo.index;
    if (!(player->stateFlags1 & PLAYER_STATE1_20) || (play->sceneId != SCENE_CLOCKTOWER)) {
        if (EnDg_ShouldReactToNonHumanPlayer(this, play)) {
            EnDg_ChooseActionForForm(this, play);
        } else if (this->behavior != DOG_BEHAVIOR_DEFAULT) {
            this->behavior = DOG_BEHAVIOR_DEFAULT;
            EnDg_SetupIdleMove(this, play);
        }

        if ((this->actor.bgCheckFlags & BGCHECKFLAG_WATER_TOUCH) && Actor_HasNoParent(&this->actor, play)) {
            EnDg_ChangeAnim(&this->skelAnime, sAnimationInfo, DOG_ANIM_SWIM);
            this->actionFunc = EnDg_SetupSwim;
        }

        this->actionFunc(this, play);
        EnDg_UpdateCollision(this, play);
        EnDg_GetFloorRot(this, &floorRot);
        Math_ApproachF(&this->curRot.x, floorRot.x, 0.2f, 0.1f);
        Math_ApproachF(&this->curRot.z, floorRot.z, 0.2f, 0.1f);
        SkelAnime_Update(&this->skelAnime);
    }
}

s32 EnDg_OverrideLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, Actor* thisx) {
    return false;
}

void EnDg_PostLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot, Actor* thisx) {
    EnDg* this = (EnDg*)thisx;
    Vec3f sFocusOffset = { 0.0f, 20.0f, 0.0f };

    if (limbIndex == DOG_LIMB_HEAD) {
        if (this->actionFunc == EnDg_SetupTalk) {
            sFocusOffset.x = 5000.0f;
            Matrix_MultVec3f(&sFocusOffset, &this->actor.focus.pos);
        } else if (this->actionFunc != EnDg_Talk) {
            Matrix_MultVec3f(&sFocusOffset, &this->actor.focus.pos);
        }
    }
}

void EnDg_Draw(Actor* thisx, PlayState* play) {
    EnDg* this = (EnDg*)thisx;

    OPEN_DISPS(play->state.gfxCtx);

    Gfx_SetupDL25_Opa(play->state.gfxCtx);

    gDPPipeSync(POLY_OPA_DISP++);

    if (this->index < RACEDOG_COUNT) {
        switch (sRacetrackDogInfo[this->index].color) {
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
    } else {
        gDPSetEnvColor(POLY_OPA_DISP++, 255, 255, 200, 0);
    }

    Matrix_Translate(this->actor.world.pos.x, this->actor.world.pos.y, this->actor.world.pos.z, MTXMODE_NEW);
    Matrix_RotateXFApply(this->curRot.x);
    Matrix_RotateZF(this->curRot.z, MTXMODE_APPLY);
    Matrix_RotateYS(this->actor.shape.rot.y, MTXMODE_APPLY);
    Matrix_Scale(this->actor.scale.x, this->actor.scale.y, this->actor.scale.z, MTXMODE_APPLY);
    SkelAnime_DrawFlexOpa(play, this->skelAnime.skeleton, this->skelAnime.jointTable, this->skelAnime.dListCount,
                          EnDg_OverrideLimbDraw, EnDg_PostLimbDraw, &this->actor);

    CLOSE_DISPS(play->state.gfxCtx);
}

void EnDg_Reset(void) {
    sIsAnyDogHeld = false;
    sBremenMaskFollowerIndex = ENDG_INDEX_NO_BREMEN_MASK_FOLLOWER;
    sSelectedRacetrackDogInfo.index = -1;
}
