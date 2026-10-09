/*
 * File: z_en_invadepoh.c
 * Overlay: ovl_En_Invadepoh
 * Description: Various ranch actors (aliens, the UFO, the dog during the alien invasion, Romani and Cremia, etc.)
 *
 * This actor handles a large number of things related to Romani Ranch. As its name suggests, it handles almost
 * everything associated with the alien invasion that occurs on Night 1, though it is responsible for a few other things
 * as well. To be specific, this actor handles the following things:
 * - The aliens that invade the ranch. This includes both the aliens the slowly approach the barn as well as an unused
 *   "abductor" variant that carries cows or Romani into the sky.
 * - The UFO that spawns the aliens and sits over the barn during the invasion. It appears as a spinning ball of light.
 * - An invisible "invasion handler" that manages the state of the alien invasion, including playing the right
 *   cutscenes, starting and ending the invasion, making the warp effects gradually fade out, etc.
 * - The dog that barks at the alien nearest to the barn during the invasion.
 * - A cow that gets carried up into the sky by the unused "abductor" alien, thus making it also unused.
 * - A cow tail attached to the unused cow, thus making it also unused.
 * - A variant of Romani that gets carried up into the sky by the unused "abductor" alien, thus making it also unused.
 * - A variant of Romani that walks from the house to the barn on Night 1 before the invasion begins.
 * - A variant of Romani that waits in the barn on Night 1 until the invasion begins.
 * - A variant of Romani that rewards the player with a bottle of milk if they defend the ranch from the alien invasion.
 * - A variant of Romani that wanders the ranch on Day 2 if the player failed to defend the ranch from the alien
 *   invasion. If the player tries to talk with her, she is silent and looks confused.
 * - A variant of Romani that walks from the barn to the house on Night 3 if the player defended the ranch.
 * - A variant of Cremia that walks from the barn to the house on Night 3 if the player defended the ranch. She walks a
 *   fixed distance from the above Romani variant.
 *
 * This actor also implements a custom animation system exclusively used to control Romani and Cremia's eye and mouth
 * animations. However, the Romani and Cremia variants used by this actor always have static mouths, so this animation
 * system is mainly used to manage their blinking.
 */

#include "prevent_bss_reordering.h"
#include "sys_cfb.h"
#include "z_en_invadepoh.h"
#include "z64horse.h"
#include "overlays/actors/ovl_En_Door/z_en_door.h"
#include "assets/objects/gameplay_keep/gameplay_keep.h"
#include "overlays/actors/ovl_En_Clear_Tag/z_en_clear_tag.h"
#include "2s2h/GameInteractor/GameInteractor.h"

#define FLAGS (ACTOR_FLAG_UPDATE_CULLING_DISABLED)

#define ALIEN_COUNT 8
#define EFFECT_COUNT 10
#define MAX_KILL_COUNT 12

#define ALIEN_STATE_FLAG_ACTIVE (1 << 0)

// This flag will be set for the living alien that is closest to the barn, so long as that alien is within 2000 units of
// the barn. Since it's possible that no living aliens are within 2000 units of the barn, it's also possible that no
// alien has this flag set. In other words, this flag will only be set for a single alien or for no alien at all.
#define ALIEN_STATE_FLAG_CLOSEST_THREAT (1 << 1)

typedef enum InvasionState {
    // This value is only used when `sInvasionState` has not been initialized yet. It should be immediately set to one
    // of the below values as soon as the invasion handler is initialized.
    /* 0 */ INVASION_STATE_NONE,

    // This value indicates that the invasion hasn't started yet, i.e. it's before 2:31 AM on the First Day.
    /* 1 */ INVASION_STATE_WAIT,

    // This value indicates that the invasion is in-progress. The aliens have spawned and are approaching the barn, but
    // they have not reached it yet; the player still has the chance to defend the ranch from the aliens.
    /* 2 */ INVASION_STATE_ACTIVE,

    // This value indicates that the player successfully defended the ranch from the aliens, i.e., it's after 5:15 AM on
    // the First Day, and no aliens reached the barn before that point in time.
    /* 3 */ INVASION_STATE_SUCCESS,

    // This value indicates that the player failed to defend the ranch from the aliens, i.e., an alien reached the barn
    // before 5:15 AM on the First Day.
    /* 4 */ INVASION_STATE_FAILURE
} InvasionState;

typedef enum SilentRomaniStareState {
    /* 0 */ SILENT_ROMANI_STARE_STATE_STARE_AT_NEARBY_PLAYER,
    /* 1 */ SILENT_ROMANI_STARE_STATE_STARE_RANDOM_FAST,
    /* 2 */ SILENT_ROMANI_STARE_STATE_STARE_RANDOM_MEDIUM,
    /* 3 */ SILENT_ROMANI_STARE_STATE_STARE_RANDOM_SLOW,
    /* 4 */ SILENT_ROMANI_STARE_STATE_MAX
} SilentRomaniStareState;

typedef enum FaceAnimationType {
    // Plays the animation, then displays the final frame of the animation indefinitely
    /* 0 */ FACE_ANIMATION_TYPE_ONCE,

    // Plays the animation, then immediately transitions to another animation
    /* 1 */ FACE_ANIMATION_TYPE_CHAINED,

    // Plays the animation, then displays the final frame of the animation for a random amount of time, after which it
    // will transition to another animation
    /* 2 */ FACE_ANIMATION_TYPE_CHAINED_DELAY,

    /* 3 */ FACE_ANIMATION_TYPE_MAX
} FaceAnimationType;

void EnInvadepoh_Init(Actor* thisx, PlayState* play2);
void EnInvadepoh_Destroy(Actor* thisx, PlayState* play2);
void EnInvadepoh_InvasionHandler_Update(Actor* thisx, PlayState* play2);
void EnInvadepoh_Reset(void);

// Update functions
void EnInvadepoh_Alien_WaitForObject(Actor* thisx, PlayState* play2);
void EnInvadepoh_Alien_Update(Actor* thisx, PlayState* play2);
void EnInvadepoh_Cow_WaitForObject(Actor* thisx, PlayState* play2);
void EnInvadepoh_Cow_Update(Actor* thisx, PlayState* play2);
void EnInvadepoh_CowTail_WaitForObject(Actor* thisx, PlayState* play2);
void EnInvadepoh_CowTail_Update(Actor* thisx, PlayState* play2);
void EnInvadepoh_AbductedRomani_WaitForObject(Actor* thisx, PlayState* play2);
void EnInvadepoh_AbductedRomani_Update(Actor* thisx, PlayState* play2);
void EnInvadepoh_SilentRomani_WaitForObject(Actor* thisx, PlayState* play2);
void EnInvadepoh_SilentRomani_Update(Actor* thisx, PlayState* play2);
void EnInvadepoh_Ufo_Update(Actor* thisx, PlayState* play2);
void EnInvadepoh_Night1Romani_WaitForObject(Actor* thisx, PlayState* play2);
void EnInvadepoh_Night1Romani_WaitForTime(Actor* thisx, PlayState* play2);
void EnInvadepoh_Night1Romani_Update(Actor* thisx, PlayState* play2);
void EnInvadepoh_BarnRomani_WaitForObject(Actor* thisx, PlayState* play2);
void EnInvadepoh_BarnRomani_WaitForTime(Actor* thisx, PlayState* play2);
void EnInvadepoh_BarnRomani_Update(Actor* thisx, PlayState* play2);
void EnInvadepoh_RewardRomani_WaitForObject(Actor* thisx, PlayState* play2);
void EnInvadepoh_RewardRomani_Update(Actor* thisx, PlayState* play2);
void EnInvadepoh_Dog_WaitForObject(Actor* thisx, PlayState* play2);
void EnInvadepoh_Dog_WaitForInvasion(Actor* thisx, PlayState* play2);
void EnInvadepoh_Dog_Update(Actor* thisx, PlayState* play2);
void EnInvadepoh_Night3Cremia_WaitForObject(Actor* thisx, PlayState* play2);
void EnInvadepoh_Night3Cremia_WaitForTime(Actor* thisx, PlayState* play2);
void EnInvadepoh_Night3Cremia_Update(Actor* thisx, PlayState* play2);
void EnInvadepoh_Night3Romani_WaitForObject(Actor* thisx, PlayState* play2);
void EnInvadepoh_Night3Romani_WaitForTime(Actor* thisx, PlayState* play2);
void EnInvadepoh_Night3Romani_Update(Actor* thisx, PlayState* play2);
void EnInvadepoh_AlienAbductor_WaitForObject(Actor* thisx, PlayState* play2);
void EnInvadepoh_AlienAbductor_Update(Actor* thisx, PlayState* play2);

// Draw functions
void EnInvadepoh_InvasionHandler_Draw(Actor* thisx, PlayState* play);
void EnInvadepoh_Alien_Draw(Actor* thisx, PlayState* play2);
void EnInvadepoh_Cow_Draw(Actor* thisx, PlayState* play2);
void EnInvadepoh_CowTail_Draw(Actor* thisx, PlayState* play2);
void EnInvadepoh_Romani_Draw(Actor* thisx, PlayState* play);
void EnInvadepoh_Ufo_Draw(Actor* thisx, PlayState* play2);
void EnInvadepoh_Dog_Draw(Actor* thisx, PlayState* play);
void EnInvadepoh_Cremia_Draw(Actor* thisx, PlayState* play);

// Action and Setup Action functions
void EnInvadepoh_InvasionHandler_SetupWaitForInvasion(EnInvadepoh* this);
void EnInvadepoh_InvasionHandler_WaitForInvasion(EnInvadepoh* this, PlayState* play);
void EnInvadepoh_InvasionHandler_SetupStartIntroCutscene(EnInvadepoh* this);
void EnInvadepoh_InvasionHandler_StartIntroCutscene(EnInvadepoh* this, PlayState* play);
void EnInvadepoh_InvasionHandler_SetupIntroCutscene(EnInvadepoh* this);
void EnInvadepoh_InvasionHandler_IntroCutscene(EnInvadepoh* this, PlayState* play);
void EnInvadepoh_InvasionHandler_SetupHandle(EnInvadepoh* this);
void EnInvadepoh_InvasionHandler_Handle(EnInvadepoh* this, PlayState* play);
void EnInvadepoh_InvasionHandler_SetupStartSuccessCutscene(EnInvadepoh* this);
void EnInvadepoh_InvasionHandler_StartSuccessCutscene(EnInvadepoh* this, PlayState* play);
void EnInvadepoh_InvasionHandler_SetupSuccessCutscene(EnInvadepoh* this);
void EnInvadepoh_InvasionHandler_SuccessCutscene(EnInvadepoh* this, PlayState* play);
void EnInvadepoh_InvasionHandler_SetupWaitForRomaniReward(EnInvadepoh* this);
void EnInvadepoh_InvasionHandler_WaitForRomaniReward(EnInvadepoh* this, PlayState* play);
void EnInvadepoh_InvasionHandler_SetupSuccessEnd(EnInvadepoh* this);
void EnInvadepoh_InvasionHandler_SuccessEnd(EnInvadepoh* this, PlayState* play);
void EnInvadepoh_InvasionHandler_SetupStartFailureCutscene(EnInvadepoh* this);
void EnInvadepoh_InvasionHandler_StartFailureCutscene(EnInvadepoh* this, PlayState* play);
void EnInvadepoh_InvasionHandler_SetupFailureEnd(EnInvadepoh* this);
void EnInvadepoh_InvasionHandler_FailureEnd(EnInvadepoh* this, PlayState* play);
void EnInvadepoh_Alien_WaitForInvasion(EnInvadepoh* this, PlayState* play);
void EnInvadepoh_Alien_WaitToRespawn(EnInvadepoh* this, PlayState* play);
void EnInvadepoh_Alien_SetupWarpIn(EnInvadepoh* this);
void EnInvadepoh_Alien_WarpIn(EnInvadepoh* this, PlayState* play);
void EnInvadepoh_Alien_SetupFloatForward(EnInvadepoh* this);
void EnInvadepoh_Alien_FloatForward(EnInvadepoh* this, PlayState* play);
void EnInvadepoh_Alien_Damaged(EnInvadepoh* this, PlayState* play);
void EnInvadepoh_Alien_SetupDead(EnInvadepoh* this);
void EnInvadepoh_Alien_Dead(EnInvadepoh* this, PlayState* play);
void EnInvadepoh_AbductedRomani_Wait(EnInvadepoh* this, PlayState* play);
void EnInvadepoh_AbductedRomani_SetupYell(EnInvadepoh* this);
void EnInvadepoh_AbductedRomani_Yell(EnInvadepoh* this, PlayState* play);
void EnInvadepoh_AbductedRomani_SetupStruggle(EnInvadepoh* this);
void EnInvadepoh_AbductedRomani_Struggle(EnInvadepoh* this, PlayState* play);
void EnInvadepoh_AbductedRomani_SetupEnd(EnInvadepoh* this);
void EnInvadepoh_AbductedRomani_End(EnInvadepoh* this, PlayState* play);
void EnInvadepoh_SilentRomani_Walk(EnInvadepoh* this, PlayState* play);
void EnInvadepoh_SilentRomani_SetupIdle(EnInvadepoh* this);
void EnInvadepoh_SilentRomani_Idle(EnInvadepoh* this, PlayState* play);
void EnInvadepoh_SilentRomani_Talk(EnInvadepoh* this, PlayState* play);
void EnInvadepoh_Ufo_SetupIntroDescend(EnInvadepoh* this);
void EnInvadepoh_Ufo_IntroDescend(EnInvadepoh* this, PlayState* play);
void EnInvadepoh_Ufo_SetupIntroWait(EnInvadepoh* this);
void EnInvadepoh_Ufo_IntroWait(EnInvadepoh* this, PlayState* play);
void EnInvadepoh_Ufo_SetupIntroFlyAround(EnInvadepoh* this);
void EnInvadepoh_Ufo_IntroFlyAround(EnInvadepoh* this, PlayState* play);
void EnInvadepoh_Ufo_SetupHoverAboveBarn(EnInvadepoh* this);
void EnInvadepoh_Ufo_HoverAboveBarn(EnInvadepoh* this, PlayState* play);
void EnInvadepoh_Ufo_OutroCircle(EnInvadepoh* this, PlayState* play);
void EnInvadepoh_Ufo_SetupOutroFlee(EnInvadepoh* this);
void EnInvadepoh_Ufo_OutroFlee(EnInvadepoh* this, PlayState* play);
void EnInvadepoh_Ufo_SetupOutroDescend(EnInvadepoh* this);
void EnInvadepoh_Ufo_OutroDescend(EnInvadepoh* this, PlayState* play);
void EnInvadepoh_Night1Romani_Walk(EnInvadepoh* this, PlayState* play);
void EnInvadepoh_Night1Romani_Talk(EnInvadepoh* this, PlayState* play);
void EnInvadepoh_BarnRomani_Idle(EnInvadepoh* this, PlayState* play);
void EnInvadepoh_BarnRomani_SetupLookAround(EnInvadepoh* this);
void EnInvadepoh_BarnRomani_LookAround(EnInvadepoh* this, PlayState* play);
void EnInvadepoh_BarnRomani_Walk(EnInvadepoh* this, PlayState* play);
void EnInvadepoh_BarnRomani_Talk(EnInvadepoh* this, PlayState* play);
void EnInvadepoh_RewardRomani_WaitForSuccess(EnInvadepoh* this, PlayState* play);
void EnInvadepoh_RewardRomani_SetupStartTalking(EnInvadepoh* this);
void EnInvadepoh_RewardRomani_StartTalking(EnInvadepoh* this, PlayState* play);
void EnInvadepoh_RewardRomani_SetupTalk(EnInvadepoh* this);
void EnInvadepoh_RewardRomani_Talk(EnInvadepoh* this, PlayState* play);
void EnInvadepoh_RewardRomani_SetupGiveBottle(EnInvadepoh* this);
void EnInvadepoh_RewardRomani_GiveBottle(EnInvadepoh* this, PlayState* play);
void EnInvadepoh_RewardRomani_SetupAfterGivingBottle(EnInvadepoh* this);
void EnInvadepoh_RewardRomani_AfterGivingBottle(EnInvadepoh* this, PlayState* play);
void EnInvadepoh_RewardRomani_SetupFinish(EnInvadepoh* this);
void EnInvadepoh_RewardRomani_Finish(EnInvadepoh* this, PlayState* play);
void EnInvadepoh_Dog_Walk(EnInvadepoh* this, PlayState* play);
void EnInvadepoh_Dog_SetupRun(EnInvadepoh* this);
void EnInvadepoh_Dog_Run(EnInvadepoh* this, PlayState* play);
void EnInvadepoh_Dog_SetupBark(EnInvadepoh* this);
void EnInvadepoh_Dog_Bark(EnInvadepoh* this, PlayState* play);
void EnInvadepoh_Dog_Jump(EnInvadepoh* this, PlayState* play);
void EnInvadepoh_Night3Cremia_Walk(EnInvadepoh* this, PlayState* play);
void EnInvadepoh_Night3Cremia_Talk(EnInvadepoh* this, PlayState* play);
void EnInvadepoh_Night3Cremia_SetupIdle(EnInvadepoh* this);
void EnInvadepoh_Night3Cremia_Idle(EnInvadepoh* this, PlayState* play);
void EnInvadepoh_Night3Romani_Walk(EnInvadepoh* this, PlayState* play);
void EnInvadepoh_Night3Romani_Talk(EnInvadepoh* this, PlayState* play);
void EnInvadepoh_Night3Romani_SetupIdle(EnInvadepoh* this);
void EnInvadepoh_Night3Romani_Idle(EnInvadepoh* this, PlayState* play);
void EnInvadepoh_AlienAbductor_AbductCow(EnInvadepoh* this, PlayState* play);
void EnInvadepoh_AlienAbductor_AbductRomani(EnInvadepoh* this, PlayState* play);

// Library functions
s32 EnInvadepoh_SnapToFloor(EnInvadepoh* this);
s32 EnInvadepoh_StepToXZ(f32* pValueX, f32* pValueZ, f32 targetX, f32 targetZ, f32 step);

ActorProfile En_Invadepoh_Profile = {
    /**/ ACTOR_EN_INVADEPOH,
    /**/ ACTORCAT_PROP,
    /**/ FLAGS,
    /**/ GAMEPLAY_KEEP,
    /**/ sizeof(EnInvadepoh),
    /**/ EnInvadepoh_Init,
    /**/ EnInvadepoh_Destroy,
    /**/ EnInvadepoh_InvasionHandler_Update,
    /**/ NULL,
    /**/ EnInvadepoh_Reset,
};

static ColliderCylinderInit sAlienCylinderInit = {
    {
        COL_MATERIAL_HIT3,
        AT_ON | AT_TYPE_ENEMY,
        AC_ON | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_1,
        COLSHAPE_CYLINDER,
    },
    {
        ELEM_MATERIAL_UNK4,
        { 0xF7CFFFFF, 0x00, 0x04 },
        { 0x00003820, 0x00, 0x00 },
        ATELEM_ON | ATELEM_SFX_NONE,
        ACELEM_ON,
        OCELEM_ON,
    },
    { 40, 95, 10, { 0, 0, 0 } },
};

static ColliderCylinderInit sRomaniAndCremiaCylinderInit = {
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
    { 18, 46, 0, { 0, 0, 0 } },
};

static ColliderCylinderInit sDogCylinderInit = {
    {
        COL_MATERIAL_NONE,
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
    { 13, 19, 0, { 0, 0, 0 } },
};

static Vec3f sUfoSpawnOffset = { 216.0f, -20.0f, 1395.0f };

static s32 sInvasionState = INVASION_STATE_NONE;

typedef enum RomaniEyeTexture {
    /* 0 */ ROMANI_EYE_OPEN,
    /* 1 */ ROMANI_EYE_HALF,
    /* 2 */ ROMANI_EYE_CLOSED,
    /* 3 */ ROMANI_EYE_HAPPY,
    /* 4 */ ROMANI_EYE_SAD,
    /* 5 */ ROMANI_EYE_MAX
} RomaniEyeTexture;

static TexturePtr sRomaniEyeTextures[ROMANI_EYE_MAX] = {
    gRomaniEyeOpenTex,   // ROMANI_EYE_OPEN
    gRomaniEyeHalfTex,   // ROMANI_EYE_HALF
    gRomaniEyeClosedTex, // ROMANI_EYE_CLOSED
    gRomaniEyeHappyTex,  // ROMANI_EYE_HAPPY
    gRomaniEyeSadTex,    // ROMANI_EYE_SAD
};

typedef enum RomaniMouthTexture {
    /* 0 */ ROMANI_MOUTH_HAPPY,
    /* 1 */ ROMANI_MOUTH_FROWN,
    /* 2 */ ROMANI_MOUTH_HANGING_OPEN,
    /* 3 */ ROMANI_MOUTH_SMILE,
    /* 4 */ ROMANI_MOUTH_MAX
} RomaniMouthTexture;

static TexturePtr sRomaniMouthTextures[ROMANI_MOUTH_MAX] = {
    gRomaniMouthHappyTex,       // ROMANI_MOUTH_HAPPY
    gRomaniMouthFrownTex,       // ROMANI_MOUTH_FROWN
    gRomaniMouthHangingOpenTex, // ROMANI_MOUTH_HANGING_OPEN
    gRomaniMouthSmileTex,       // ROMANI_MOUTH_SMILE
};

static s8 sRomaniTexturesDesegmented = false;

typedef enum CremiaEyeTexture {
    /* 0 */ CREMIA_EYE_OPEN,
    /* 1 */ CREMIA_EYE_HALF,
    /* 2 */ CREMIA_EYE_CLOSED,
    /* 3 */ CREMIA_EYE_HAPPY,
    /* 4 */ CREMIA_EYE_ANGRY,
    /* 5 */ CREMIA_EYE_SAD,
    /* 6 */ CREMIA_EYE_MAX
} CremiaEyeTexture;

static TexturePtr sCremiaEyeTextures[CREMIA_EYE_MAX] = {
    gCremiaEyeOpenTex,   // CREMIA_EYE_OPEN
    gCremiaEyeHalfTex,   // CREMIA_EYE_HALF
    gCremiaEyeClosedTex, // CREMIA_EYE_CLOSED
    gCremiaEyeHappyTex,  // CREMIA_EYE_HAPPY
    gCremiaEyeAngryTex,  // CREMIA_EYE_ANGRY
    gCremiaEyeSadTex,    // CREMIA_EYE_SAD
};

typedef enum CremiaMouthTexture {
    /* 0 */ CREMIA_MOUTH_NORMAL,
    /* 1 */ CREMIA_MOUTH_SLIGHT_SMILE,
    /* 2 */ CREMIA_MOUTH_FROWN,
    /* 3 */ CREMIA_MOUTH_HANGING_OPEN,
    /* 4 */ CREMIA_MOUTH_MAX
} CremiaMouthTexture;

static TexturePtr sCremiaMouthTextures[CREMIA_MOUTH_MAX] = {
    gCremiaMouthNormalTex,      // CREMIA_MOUTH_NORMAL
    gCremiaMouthSlightSmileTex, // CREMIA_MOUTH_SLIGHT_SMILE
    gCremiaMouthFrownTex,       // CREMIA_MOUTH_FROWN
    gCremiaMouthHangingOpenTex, // CREMIA_MOUTH_HANGING_OPEN
};

static s8 sCremiaTexturesDesegmented = false;

/**
 * Used to determine if Romani is done giving the Milk Bottle reward. This is checked by the invasion handler instance
 * of EnInvadepoh to transition the player to a different version of the ranch once Romani is done giving her reward.
 */
static s8 sRewardFinished = false;

MtxF sInvadepohAlienLeftEyeBeamMtxF;
MtxF sInvadepohAlienRightEyeBeamMtxF;

EnInvadepoh* sAliens[ALIEN_COUNT];
u8 sAlienStateFlags[ALIEN_COUNT];

/**
 * This variable is true if any alien is within 340 units of the barn and is otherwise false.
 */
s8 sAliensExtremeThreat;

typedef enum EnInvadepohEffectType {
    /* 0 */ EN_INVADEPOH_EFFECT_TYPE_WARP,
    /* 1 */ EN_INVADEPOH_EFFECT_TYPE_MAX
} EnInvadepohEffectType;

typedef struct EnInvadepohEffect {
    /* 0x0 */ s8 type;
    /* 0x1 */ s8 timer;
    /* 0x2 */ u8 alpha;
    /* 0x4 */ Vec3f pos;
} EnInvadepohEffect; // size = 0x10

EnInvadepohEffect sEffects[EFFECT_COUNT];

EnInvadepoh* sUfo;
EnInvadepoh* sNight3Romani;
EnInvadepoh* sNight3Cremia;

AnimatedMaterial* sAlienEyeBeamTexAnim;
AnimatedMaterial* sAlienEmptyTexAnim;

s16 sInvadepohCsIdList[3];

/**
 * If there are any aliens within 2000 units of the barn, then this will point to the alien that is closest to the barn.
 * Otherwise, this will be set to `NULL`.
 */
EnInvadepoh* sClosestAlienThreat;

void EnInvadepoh_Alien_SetSpawnTime(s32 index, s32 spawnTime) {
    // The time that each alien spawns is stored as an offset from 2:30 AM, so we need to subtract that from the spawn
    // time in order to get the offset we need. We also clamp this offset so that it's never negative, though this
    // operation should always be redundant in practice.
    spawnTime -= CLOCK_TIME(2, 30);
    spawnTime = CLAMP_MIN(spawnTime, 0);

    ALIEN_SET_SPAWN_TIME_OFFSET(index, spawnTime);
}

s32 EnInvadepoh_Alien_GetSpawnTime(s32 index) {
    return CLOCK_TIME(2, 30) + ALIEN_GET_SPAWN_TIME_OFFSET(index);
}

void EnInvadepoh_Alien_SetKillCount(s32 count) {
    ALIEN_SET_KILL_COUNT(count);
}

s32 EnInvadepoh_Alien_GetKillCount(void) {
    return ALIEN_GET_KILL_COUNT();
}

/**
 * If the player has killed fewer than `MAX_KILL_COUNT` aliens, this function will increment the kill counter.
 */
s32 EnInvadepoh_Alien_AddKill(void) {
    s32 killCount = EnInvadepoh_Alien_GetKillCount();

    if (killCount < MAX_KILL_COUNT) {
        killCount++;
        EnInvadepoh_Alien_SetKillCount(killCount);
    }

    return killCount;
}

/**
 * Sets the alien's spawn time with a delay based on how many aliens the player has already killed. As the player kills
 * more aliens, the delay gets shorter and shorter; once the player has killed `MAX_KILL_COUNT` number of aliens, the
 * delay will become zero and aliens will instantly respawn after being defeated.
 */
void EnInvadepoh_Alien_SetRespawnTime(s32 index) {
    s32 currentTime = CURRENT_TIME;

    if ((CURRENT_DAY == 1) && (currentTime >= CLOCK_TIME(2, 30)) && (currentTime < CLOCK_TIME(5, 15))) {
        s32 spawnDelay = (MAX_KILL_COUNT - EnInvadepoh_Alien_GetKillCount()) * 25.0f;

        EnInvadepoh_Alien_SetSpawnTime(index, currentTime + spawnDelay);
    }
}

/**
 * Determines the alien's current point by comparing its path progress to the checkpoints for its path.
 */
s32 EnInvadepoh_Alien_GetCurrentPoint(EnInvadepoh* this) {
    s32 i;
    s32 secondToLastPoint = this->endPoint - 1;

    for (i = 0; i < secondToLastPoint; i++) {
        if (this->pathProgress < this->pathCheckpoints[i]) {
            break;
        }
    }

    return i;
}

/**
 * Updates Romani's position and current point index based on her current path progress.
 */
void EnInvadepoh_Romani_PathUpdate(EnInvadepoh* this, s8* currentPoint, Vec3f* pos) {
    f32 distanceToCurrent = 0.0f;
    f32 currentCheckpoint = 0.0f;
    f32 invTotalPathDistance = 1.0f / this->totalPathDistance;
    s32 endPoint = this->endPoint;
    s32 i;
    Vec3f currentToNext;
    Vec3s* currentPathPoint = &this->pathPoints[0];
    Vec3s* nextPathPoint = currentPathPoint + 1;
    f32 nextCheckpoint;
    f32 distanceToNext;
    f32 segmentProgress;
    f32 pathSegmentDistance;

    for (i = 0; i < endPoint; i++) {
        currentToNext.x = nextPathPoint->x - currentPathPoint->x;
        currentToNext.y = nextPathPoint->y - currentPathPoint->y;
        currentToNext.z = nextPathPoint->z - currentPathPoint->z;
        pathSegmentDistance = Math3D_Vec3fMagnitude(&currentToNext);
        distanceToNext = distanceToCurrent + pathSegmentDistance;
        nextCheckpoint = distanceToNext * invTotalPathDistance;

        if (this->pathProgress <= nextCheckpoint) {
            *currentPoint = i;
            segmentProgress = (this->pathProgress - currentCheckpoint) / (nextCheckpoint - currentCheckpoint);
            pos->x = (segmentProgress * currentToNext.x) + currentPathPoint->x;
            pos->y = (segmentProgress * currentToNext.y) + currentPathPoint->y;
            pos->z = (segmentProgress * currentToNext.z) + currentPathPoint->z;
            return;
        }

        currentPathPoint = nextPathPoint;
        nextPathPoint++;
        distanceToCurrent = distanceToNext;
        currentCheckpoint = nextCheckpoint;
    }

    *currentPoint = endPoint;
    pos->x = this->pathPoints[endPoint].x;
    pos->y = this->pathPoints[endPoint].y;
    pos->z = this->pathPoints[endPoint].z;
}

/**
 * This function can be called repeatedly to make the alien gradually turn to face the next point in its path. Unlike
 * the below function, this function takes an `offset` parameter; if this parameter is non-zero, then the alien will not
 * be perfectly aligned with the path.
 */
void EnInvadepoh_Alien_StepYawAlongPath(EnInvadepoh* this, s16 step, s16 offset) {
    s32 pad;
    Vec3s* currentPathPoint = &this->pathPoints[this->currentPoint];
    Vec3s* nextPathPoint = currentPathPoint + 1;
    Vec3f currentPathPointPos;
    Vec3f nextPathPointPos;

    if (this->currentPoint != this->endPoint) {
        Math_Vec3s_ToVec3f(&currentPathPointPos, currentPathPoint);
        Math_Vec3s_ToVec3f(&nextPathPointPos, nextPathPoint);
        Math_ScaledStepToS(&this->actor.shape.rot.y, Math_Vec3f_Yaw(&currentPathPointPos, &nextPathPointPos) + offset,
                           step);
    }
}

/**
 * This function can be called repeatedly to make Romani gradually turn to face the next point in her path. Unlike the
 * above function, this function uses a smooth step rather than a scaled step.
 */
void EnInvadepoh_Romani_StepYawAlongPath(EnInvadepoh* this, s16 scale, s16 step, s16 minStep) {
    s32 pad;
    Vec3s* currentPathPoint = &this->pathPoints[this->currentPoint];
    Vec3s* nextPathPoint = currentPathPoint + 1;
    Vec3f currentPathPointPos;
    Vec3f nextPathPointPos;

    if (this->currentPoint != this->endPoint) {
        Math_Vec3s_ToVec3f(&currentPathPointPos, currentPathPoint);
        Math_Vec3s_ToVec3f(&nextPathPointPos, nextPathPoint);
        Math_SmoothStepToS(&this->actor.shape.rot.y, Math_Vec3f_Yaw(&currentPathPointPos, &nextPathPointPos), scale,
                           step, minStep);
    }
}

/**
 * This function immediately sets the y-rotation of the actor to face the next point in its path.
 */
void EnInvadepoh_SetYawAlongPath(EnInvadepoh* this) {
    s32 pad;
    Vec3s* currentPathPoint = &this->pathPoints[this->currentPoint];
    Vec3s* nextPathPoint = currentPathPoint + 1;
    Vec3f currentPathPointPos;
    Vec3f nextPathPointPos;

    Math_Vec3s_ToVec3f(&currentPathPointPos, currentPathPoint);
    Math_Vec3s_ToVec3f(&nextPathPointPos, nextPathPoint);
    this->actor.shape.rot.y = Math_Vec3f_Yaw(&currentPathPointPos, &nextPathPointPos);
}

f32 EnInvadepoh_GetTotalPathDistance(EnInvadepoh* this) {
    s32 pointCount = this->endPoint + 1;
    s32 i;
    Vec3f previousPathPointPos;
    Vec3f currentPathPointPos;
    Vec3s* currentPathPoint = &this->pathPoints[0];
    f32 totalPathDistance = 0.0f;

    Math_Vec3s_ToVec3f(&currentPathPointPos, currentPathPoint);
    currentPathPoint++;

    for (i = 1; i < pointCount; currentPathPoint++, i++) {
        Math_Vec3f_Copy(&previousPathPointPos, &currentPathPointPos);
        Math_Vec3s_ToVec3f(&currentPathPointPos, currentPathPoint);
        totalPathDistance += Math3D_Vec3f_DistXYZ(&previousPathPointPos, &currentPathPointPos);
    }

    return totalPathDistance;
}

void EnInvadepoh_InitPath(EnInvadepoh* this, PlayState* play) {
    Path* path = &play->setupPathList[EN_INVADEPOH_GET_PATH(&this->actor)];

    this->endPoint = path->count - 1;
    this->pathPoints = Lib_SegmentedToVirtual(path->points);
}

/**
 * Sets the actor's current position to be the same as the point along with the path with the index of `point`.
 */
void EnInvadepoh_SetPosToPathPoint(EnInvadepoh* this, s32 point) {
    Math_Vec3s_ToVec3f(&this->actor.world.pos, &this->pathPoints[point]);
}

/**
 * Returns true if the dog is considered sufficiently "close enough" to the path it's running along and false otherwise.
 * Specifically, it checks to see if the dog's position is contained within a rectangle that looks like this:
 *           ____________________________
 *          |                            |
 *  (a - b) |                            |
 *          |                            |
 *          C----------------------------N
 *          |                            |
 *  (a - b) |                            |
 *          |____________________________|
 *
 * Based on the dog's current point along the path (labeled C in the above diagram) and the dog's "next" point (labeled
 * as N), the rectangle extends `(a - b)` units in both perpendicular directions from the line formed by those two
 * points. Note that this function does not work properly when the dog is running clockwise due to a bug.
 */
s32 EnInvadepoh_Dog_IsCloseToPath(EnInvadepoh* this, f32 a, f32 b) {
    //! @bug: When the dog is running clockwise, this calculation is incorrect. The path points are laid out such that
    //! if the dog is on point N, then point N+1 is the next point in the counterclockwise direction, and point N-1 is
    //! the next point in the clockwise direction. Using `pathStep` instead of hardcoding +1 here would fix the bug.
    //! Because of this bug, the dog is never considered to be "close enough" to the path when running clockwise.
    Vec3s* nextPathPoint = &this->pathPoints[this->currentPoint] + 1;
    Vec3s* currentPathPoint = &this->pathPoints[this->currentPoint];
    s32 pad;
    f32 diffX;
    f32 diffZ;
    f32 cos;
    f32 sin;
    f32 offsetFromPointX;
    f32 offsetFromPointZ;
    f32 distanceAlongPath;
    f32 perpendicularDistance;
    f32 pathSegmentDistance;
    s16 pathYaw;

    // The dog's current point should never be set to the `endPoint`, so this code should never run in practice. The dog
    // is placed on a circular path in the final game, where the last point on the path overlaps with the first one. To
    // account for this, `EnInvadepoh_Dog_MoveAlongPath` will ensure that the last point will always be skipped.
    if (this->currentPoint >= this->endPoint) {
        return false;
    }

    diffX = nextPathPoint->x - currentPathPoint->x;
    diffZ = nextPathPoint->z - currentPathPoint->z;
    pathYaw = Math_Atan2S_XY(diffZ, diffX);
    cos = Math_CosS(pathYaw);
    sin = Math_SinS(pathYaw);
    offsetFromPointX = this->actor.world.pos.x - currentPathPoint->x;
    offsetFromPointZ = this->actor.world.pos.z - currentPathPoint->z;

    // Computes the perpendicular distance from the dog to the line formed by the current and next path points and
    // checks to see if this distance is less than `(a - b)`. In other words, the dog must be less than `(a - b)` units
    // away from the path in either perpendicular direction in order to be considered close to the path.
    perpendicularDistance = fabsf((offsetFromPointX * cos) - (offsetFromPointZ * sin));
    if (perpendicularDistance > (a - b)) {
        return false;
    }

    // Projects the dog's current position onto the line formed by the current and next path points and checks to see if
    // the projected position lies between the current and next points along the path. In other words, it makes sure the
    // dog is not "behind" the current point or "ahead of" the next point along the axis formed by the two points.
    pathSegmentDistance = Math3D_Dist1D(diffX, diffZ);
    distanceAlongPath = (offsetFromPointZ * cos) + (offsetFromPointX * sin);
    if ((distanceAlongPath < 0.0f) || (pathSegmentDistance < distanceAlongPath)) {
        return false;
    }

    return true;
}

/**
 * Returns the index of the point along the dog's path that is closest to the supplied `target` position. This is used
 * by the dog to move along its path towards the closest threatening alien.
 */
s32 EnInvadepoh_Dog_FindClosestPointToTarget(EnInvadepoh* this, Vec3f* target) {
    s32 point;
    s32 endPoint = this->endPoint;
    Vec3s* pathPoint;
    Vec3f pointPos;
    f32 minDistSqToTarget = FLT_MAX;
    f32 distSqToTarget;
    s32 closestPoint = 0;

    for (point = 0, pathPoint = this->pathPoints; point < endPoint; point++, pathPoint++) {
        Math_Vec3s_ToVec3f(&pointPos, pathPoint);
        distSqToTarget = Math3D_Vec3fDistSq(&pointPos, target);

        if (distSqToTarget < minDistSqToTarget) {
            minDistSqToTarget = distSqToTarget;
            closestPoint = point;
        }
    }

    return closestPoint;
}

/**
 * Computes the alien's progress along the path based on how much in-game time has passed since the alien spawned, then
 * sets the alien's current point based on its path progress.
 */
void EnInvadepoh_Alien_PathComputeProgress(EnInvadepoh* this) {
    s32 pad;
    s32 currentTime = CURRENT_TIME;
    s32 warpInTime = EnInvadepoh_Alien_GetSpawnTime(EN_INVADEPOH_GET_INDEX(&this->actor));

    if (sInvasionState == INVASION_STATE_WAIT) {
        this->pathProgress = 0.0f;
    } else if (sInvasionState == INVASION_STATE_ACTIVE) {
        GameInteractor_Should(VB_SET_ALIEN_SPEED, false, &warpInTime, EN_INVADEPOH_GET_INDEX(&this->actor));
        if ((currentTime - warpInTime) < 0) {
            this->pathProgress = 0.0f;
        } else {
            this->pathProgress = (currentTime - warpInTime) * (1.0f / (80 * CLOCK_TIME_MINUTE));

            if (this->pathProgress > 1.0f) {
                this->pathProgress = 1.0f;
            }
        }
    }

    this->currentPoint = EnInvadepoh_Alien_GetCurrentPoint(this);
}

/**
 * This function calculates a "checkpoint" for each point on the alien's path; when the alien's path progress surpasses
 * the value of a given checkpoint, that is used as a signal to move the alien's current point forward along the path.
 */
void EnInvadepoh_Alien_SetCheckpoints(EnInvadepoh* this) {
    f32 invTotalPathDistance = 1.0f / this->totalPathDistance;
    s32 endPoint = this->endPoint;
    s32 i;
    Vec3f previousPathPointPos;
    Vec3f currentPathPointPos;
    Vec3s* currentPathPoint = &this->pathPoints[0];
    f32 pathSegmentDistance = 0.0f;
    f32* pathCheckpoint;

    Math_Vec3s_ToVec3f(&currentPathPointPos, currentPathPoint);
    currentPathPoint++;
    pathCheckpoint = &this->pathCheckpoints[0];

    for (i = 1; i < endPoint; i++, currentPathPoint++, pathCheckpoint++) {
        Math_Vec3f_Copy(&previousPathPointPos, &currentPathPointPos);
        Math_Vec3s_ToVec3f(&currentPathPointPos, currentPathPoint);
        pathSegmentDistance += Math3D_Vec3f_DistXYZ(&previousPathPointPos, &currentPathPointPos);
        *pathCheckpoint = pathSegmentDistance * invTotalPathDistance;

        if (*pathCheckpoint < 0.0f) {
            *pathCheckpoint = 0.0f;
        } else if (*pathCheckpoint > 1.0f) {
            *pathCheckpoint = 1.0f;
        }
    }
}

void EnInvadepoh_Alien_InitPath(EnInvadepoh* this, PlayState* play) {
    EnInvadepoh_InitPath(this, play);
    this->totalPathDistance = EnInvadepoh_GetTotalPathDistance(this);
    EnInvadepoh_Alien_SetCheckpoints(this);
}

void EnInvadepoh_SilentRomani_InitPath(EnInvadepoh* this, PlayState* play) {
    EnInvadepoh_InitPath(this, play);
}

void EnInvadepoh_SilentRomani_SetPathPointToNext(EnInvadepoh* this) {
    this->currentPoint++;

    if (this->currentPoint >= this->endPoint) {
        this->currentPoint = 0;
    }
}

void EnInvadepoh_Night1Romani_InitPath(EnInvadepoh* this, PlayState* play) {
    EnInvadepoh_InitPath(this, play);
    this->totalPathDistance = EnInvadepoh_GetTotalPathDistance(this);
}

/**
 * Computes Romani's progress along the path based on the current in-game time.
 */
void EnInvadepoh_Night1Romani_PathComputeProgress(EnInvadepoh* this) {
    if ((CURRENT_TIME < CLOCK_TIME(2, 00)) || (CURRENT_TIME >= CLOCK_TIME(6, 00))) {
        this->pathProgress = 0.0f;
    } else if ((CURRENT_TIME >= CLOCK_TIME(2, 15)) && (CURRENT_TIME < CLOCK_TIME(6, 00))) {
        this->pathProgress = 1.0f;
    } else {
        f32 progress = (CURRENT_TIME - CLOCK_TIME(2, 00)) * (1.0f / (CLOCK_TIME(2, 15) - CLOCK_TIME(2, 00)));

        this->pathProgress = progress;
        this->pathProgress = CLAMP(this->pathProgress, 0.0f, 1.0f);
    }
}

void EnInvadepoh_BarnRomani_InitPath(EnInvadepoh* this, PlayState* play) {
    EnInvadepoh_InitPath(this, play);
}

void EnInvadepoh_BarnRomani_SetPathPointToNext(EnInvadepoh* this) {
    if (this->currentPoint < this->endPoint) {
        this->currentPoint++;
    }
}

void EnInvadepoh_Dog_InitPath(EnInvadepoh* this, PlayState* play) {
    EnInvadepoh_InitPath(this, play);
    this->pathStep = 1;
}

void EnInvadepoh_Dog_SetPathPointToNext(EnInvadepoh* this) {
    this->currentPoint += this->pathStep;

    // The dog is placed on a circular path where the first and last point overlap. This code ensures that the
    // `currentPoint` will never be the last point.
    if (this->currentPoint >= this->endPoint) {
        this->currentPoint = 0;
    } else if (this->currentPoint < 0) {
        this->currentPoint = this->endPoint - 1;
    }
}

void EnInvadepoh_Night3Romani_InitPath(EnInvadepoh* this, PlayState* play) {
    EnInvadepoh_InitPath(this, play);
    this->totalPathDistance = EnInvadepoh_GetTotalPathDistance(this);
}

/**
 * Computes Romani's progress along the path based on the current in-game time.
 */
void EnInvadepoh_Night3Romani_PathComputeProgress(EnInvadepoh* this) {
    if ((CURRENT_TIME < CLOCK_TIME(20, 00)) && (CURRENT_TIME >= CLOCK_TIME(6, 00))) {
        this->pathProgress = 0.0f;
    } else if ((CURRENT_TIME >= CLOCK_TIME(20, 14) + 15) || (CURRENT_TIME < CLOCK_TIME(6, 00))) {
        this->pathProgress = 1.0f;
    } else {
        f32 progress = (CURRENT_TIME - CLOCK_TIME(20, 00)) * (1.0f / (CLOCK_TIME(20, 14) + 15 - CLOCK_TIME(20, 00)));

        this->pathProgress = progress;
        this->pathProgress = CLAMP(this->pathProgress, 0.0f, 1.0f);
    }
}

/**
 * Updates the alien's position and y-velocity based on its current path progress.
 */
void EnInvadepoh_Alien_PathUpdate(EnInvadepoh* this, PlayState* play) {
    s32 pad;
    Vec3s* currentPathPoint;
    Vec3s* nextPathPoint;
    Vec3f currentPathPointPos;
    Vec3f nextPathPointPos;
    f32 tempPosY = this->actor.world.pos.y;
    f32 currentCheckpoint;
    f32 nextCheckpoint;

    currentPathPoint = &this->pathPoints[this->currentPoint];
    nextPathPoint = currentPathPoint + 1;
    currentCheckpoint = (this->currentPoint <= 0) ? 0.0f : this->pathCheckpoints[this->currentPoint - 1];
    nextCheckpoint = (this->currentPoint < (this->endPoint - 1)) ? this->pathCheckpoints[this->currentPoint] : 1.0f;

    if (nextCheckpoint - currentCheckpoint < 0.001f) {
        Math_Vec3s_ToVec3f(&this->currentPos, currentPathPoint);
    } else {
        f32 nextWeight = this->pathProgress - currentCheckpoint;
        f32 currentWeight = nextCheckpoint - this->pathProgress;
        f32 invCheckpointDistance = 1.0f / (nextCheckpoint - currentCheckpoint);
        s32 pad2;

        Math_Vec3s_ToVec3f(&currentPathPointPos, currentPathPoint);
        Math_Vec3s_ToVec3f(&nextPathPointPos, nextPathPoint);
        this->currentPos.x =
            ((currentPathPointPos.x * currentWeight) + (nextPathPointPos.x * nextWeight)) * invCheckpointDistance;
        this->currentPos.y =
            ((currentPathPointPos.y * currentWeight) + (nextPathPointPos.y * nextWeight)) * invCheckpointDistance;
        this->currentPos.z =
            ((currentPathPointPos.z * currentWeight) + (nextPathPointPos.z * nextWeight)) * invCheckpointDistance;
    }

    Math_Vec3f_Copy(&this->actor.world.pos, &this->currentPos);
    func_800B4AEC(play, &this->actor, 0.0f);

    if (this->actor.floorHeight > (BGCHECK_Y_MIN + 1.0f)) {
        if (tempPosY < this->actor.floorHeight) {
            if (this->actor.velocity.y < 0.0f) {
                this->actor.velocity.y = 0.0f;
            } else {
                this->actor.velocity.y += 2.0f;
                this->actor.velocity.y = CLAMP_MAX(this->actor.velocity.y, 30.0f);
            }

            this->actor.world.pos.y = this->actor.velocity.y + tempPosY;

            if (this->actor.floorHeight < this->actor.world.pos.y) {
                this->actor.world.pos.y = this->actor.floorHeight;
            }
        } else {
            if (this->actor.velocity.y > 0.0f) {
                this->actor.velocity.y = 0.0f;
            } else {
                this->actor.velocity.y -= 2.0f;
            }

            this->actor.world.pos.y = this->actor.velocity.y + tempPosY;

            if (this->actor.world.pos.y < this->actor.floorHeight) {
                this->actor.world.pos.y = this->actor.floorHeight;
                this->actor.velocity.y = CLAMP_MIN(this->actor.velocity.y, -30.0f);
            }
        }
    } else {
        this->actor.world.pos.y = tempPosY;
    }
}

/**
 * Decreases the alien's y-velocity and speed when hit by an arrow. The extent of the decrease depends on whether the
 * alien is touching the ground or a wall.
 */
void EnInvadepoh_Alien_Knockback(EnInvadepoh* this, PlayState* play) {
    if (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND) {
        this->actor.velocity.y *= 0.3f;
        this->actor.speed *= 0.8f;
    } else if (this->actor.bgCheckFlags & BGCHECKFLAG_WALL) {
        this->actor.velocity.y *= 0.8f;
        this->actor.speed *= 0.3f;
    } else {
        this->actor.velocity.y *= 0.8f;
        this->actor.speed *= 0.8f;
    }

    Actor_MoveWithGravity(&this->actor);
    Actor_UpdateBgCheckInfo(play, &this->actor, 30.0f, 40.0f, 0.0f, UPDBGCHECKINFO_FLAG_1 | UPDBGCHECKINFO_FLAG_4);
}

void EnInvadepoh_Alien_DoNothing(EnInvadepoh* this) {
}

/**
 * Moves Romani towards the next point along her path based on the supplied `speed`. Returns true if she reached the
 * next point, false otherwise.
 */
s32 EnInvadepoh_Romani_MoveAlongPath(EnInvadepoh* this, PlayState* play, f32 speed, f32 height) {
    s32 pad;
    Vec3s* nextPathPoint = &this->pathPoints[this->currentPoint + 1];
    s32 reachedNextPoint = EnInvadepoh_StepToXZ(&this->actor.world.pos.x, &this->actor.world.pos.z, nextPathPoint->x,
                                                nextPathPoint->z, speed);

    func_800B4AEC(play, &this->actor, height);
    EnInvadepoh_SnapToFloor(this);
    return reachedNextPoint;
}

void EnInvadepoh_Night1Romani_MoveAlongTimePath(EnInvadepoh* this, PlayState* play) {
    s32 pad;
    f32 tempPosY = this->actor.world.pos.y;

    EnInvadepoh_Romani_PathUpdate(this, &this->currentPoint, &this->actor.world.pos);
    this->actor.world.pos.y = tempPosY;
    func_800B4AEC(play, &this->actor, 50.0f);
    EnInvadepoh_SnapToFloor(this);
}

/**
 * This function makes the dog rotate towards the next point along its path (with some slight deviation) and moves the
 * dog forward. It returns true if the dog has reached its next point; this is used in other functions as a signal that
 * it's time to update the current point.
 */
s32 EnInvadepoh_Dog_MoveAlongPath(EnInvadepoh* this, PlayState* play) {
    s32 pad;
    Vec3s* currentPathPoint = &this->pathPoints[this->currentPoint];
    Vec3s* nextPathPoint;
    s32 nextPoint;
    f32 nextPathPointX;
    f32 nextPathPointZ;
    f32 currentPathPointX;
    f32 currentPathPointZ;
    Vec3f currentToNext;
    Vec3f worldToNext;
    Vec3f worldToCurrent;
    Vec3f worldToTarget;
    f32 angleToNext;
    s32 reachedNextPoint = false;
    u32 updBgCheckInfoFlags;

    nextPoint = this->currentPoint + this->pathStep;

    // The dog is placed on a circular path where the first and last point overlap. This code ensures that the
    // `nextPoint` will never be the last point.
    if (nextPoint >= this->endPoint) {
        nextPoint = 0;
    } else if (nextPoint < 0) {
        nextPoint = this->endPoint - 1;
    }

    nextPathPoint = &this->pathPoints[nextPoint];
    currentPathPointX = currentPathPoint->x;
    currentPathPointZ = currentPathPoint->z;
    nextPathPointX = nextPathPoint->x;
    nextPathPointZ = nextPathPoint->z;

    currentToNext.x = nextPathPointX - currentPathPointX;
    currentToNext.y = 0.0f;
    currentToNext.z = nextPathPointZ - currentPathPointZ;

    worldToNext.x = nextPathPointX - this->actor.world.pos.x;
    worldToNext.y = 0.0f;
    worldToNext.z = nextPathPointZ - this->actor.world.pos.z;

    worldToCurrent.x = currentPathPointX - this->actor.world.pos.x;
    worldToCurrent.y = 0.0f;
    worldToCurrent.z = currentPathPointZ - this->actor.world.pos.z;

    if (this->actor.speed > 0.0f) {
        if (Math3D_CosOut(&currentToNext, &worldToNext, &angleToNext)) {
            reachedNextPoint = true;
        } else if (angleToNext <= 0.0f) {
            reachedNextPoint = true;
        }
    }

    // Instead of targeting the next path point, this code will make the dog target a point slightly "beyond" the next
    // point along the line of the path. Specifically, the dog targets a point that is 1/18th (or 5.55%) of the path
    // length beyond the end of the path.
    worldToTarget.x = ((worldToNext.x + worldToCurrent.x) * 0.9f) + currentToNext.x;
    worldToTarget.y = 0.0f;
    worldToTarget.z = ((worldToNext.z + worldToCurrent.z) * 0.9f) + currentToNext.z;

    Math_SmoothStepToS(&this->actor.world.rot.y, Math_Atan2S_XY(worldToTarget.z, worldToTarget.x), 4, 0xFA0, 0x64);
    Actor_MoveWithGravity(&this->actor);

    if (EnInvadepoh_Dog_IsCloseToPath(this, 50.0f, 15.0f)) {
        updBgCheckInfoFlags = UPDBGCHECKINFO_FLAG_4;
    } else {
        updBgCheckInfoFlags = UPDBGCHECKINFO_FLAG_1 | UPDBGCHECKINFO_FLAG_4;
    }

    Actor_UpdateBgCheckInfo(play, &this->actor, 30.0f, 15.0f, 0.0f, updBgCheckInfoFlags);
    Math_SmoothStepToS(&this->actor.shape.rot.y, this->actor.world.rot.y, 3, 0x1F40, 0x64);

    return reachedNextPoint;
}

/**
 * Moves the dog forward without doing any sort of path checks; it simply moves forward based on its current rotation.
 */
void EnInvadepoh_Dog_Move(EnInvadepoh* this, PlayState* play) {
    Actor_MoveWithGravity(&this->actor);
    Actor_UpdateBgCheckInfo(play, &this->actor, 30.0f, 15.0f, 0.0f, UPDBGCHECKINFO_FLAG_1 | UPDBGCHECKINFO_FLAG_4);
    Math_SmoothStepToS(&this->actor.shape.rot.y, this->actor.world.rot.y, 3, 0x1F40, 0x64);
}

void EnInvadepoh_Night3Romani_MoveAlongTimePath(EnInvadepoh* this, PlayState* play) {
    s32 pad;
    f32 tempPosY = this->actor.world.pos.y;

    EnInvadepoh_Romani_PathUpdate(this, &this->currentPoint, &this->actor.world.pos);
    this->actor.world.pos.y = tempPosY;
    func_800B4AEC(play, &this->actor, 50.0f);
    EnInvadepoh_SnapToFloor(this);
}

void EnInvadepoh_Romani_DesegmentTextures(void) {
    TexturePtr* texture;
    s32 i;

    if (!sRomaniTexturesDesegmented) {
        sRomaniTexturesDesegmented = true;

        for (i = 0, texture = sRomaniEyeTextures; i < ROMANI_EYE_MAX; i++, texture++) {
            *texture = Lib_SegmentedToVirtual(*texture);
        }

        for (i = 0, texture = sRomaniMouthTextures; i < ROMANI_MOUTH_MAX; i++, texture++) {
            *texture = Lib_SegmentedToVirtual(*texture);
        }
    }
}

void EnInvadepoh_Cremia_DesegmentTextures(void) {
    TexturePtr* texture;
    s32 i;

    if (!sCremiaTexturesDesegmented) {
        sCremiaTexturesDesegmented = true;

        for (i = 0, texture = sCremiaEyeTextures; i < CREMIA_EYE_MAX; i++, texture++) {
            *texture = Lib_SegmentedToVirtual(*texture);
        }

        for (i = 0, texture = sCremiaMouthTextures; i < CREMIA_MOUTH_MAX; i++, texture++) {
            *texture = Lib_SegmentedToVirtual(*texture);
        }
    }
}

void EnInvadepoh_Alien_DesegmentTexAnims(void) {
    sAlienEmptyTexAnim = Lib_SegmentedToVirtual(gAlienEmptyTexAnim);
    sAlienEyeBeamTexAnim = Lib_SegmentedToVirtual(gAlienEyeBeamTexAnim);
}

s32 EnInvadepoh_StepToXZ(f32* pValueX, f32* pValueZ, f32 targetX, f32 targetZ, f32 step) {
    f32 diffX = targetX - *pValueX;
    f32 diffZ = targetZ - *pValueZ;
    f32 distToTarget = Math3D_Dist1D(diffX, diffZ);

    if (step < distToTarget) {
        f32 stepFraction = step / distToTarget;

        *pValueX += stepFraction * diffX;
        *pValueZ += stepFraction * diffZ;
        return false;
    } else {
        *pValueX = targetX;
        *pValueZ = targetZ;
        return true;
    }
}

/**
 * If there is a floor under the actor, this function will set the actor's y-position to the floor's y-position and
 * return true. Otherwise, it will return false.
 */
s32 EnInvadepoh_SnapToFloor(EnInvadepoh* this) {
    if (this->actor.floorHeight > BGCHECK_Y_MIN + 1.0f) {
        this->actor.world.pos.y = this->actor.floorHeight;
        return true;
    }

    return false;
}

/**
 * Sets the initial value of `sInvasionState` based on the current date and time and whether the weekeventreg for
 * defending the ranch is set. If the state is anything other than `INVASION_STATE_NONE`, this function
 * does nothing; the invasion state is already set in this circumstance.
 */
void EnInvadepoh_InvasionHandler_SetInitialInvasionState(EnInvadepoh* this, PlayState* play) {
    if (sInvasionState == INVASION_STATE_NONE) {
        if (CURRENT_DAY < 1) {
            // It's before the first day, so the invasion hasn't started yet.
            sInvasionState = INVASION_STATE_WAIT;
        } else if (CURRENT_DAY == 1) {
            s32 currentTime = CURRENT_TIME;

            if ((currentTime < CLOCK_TIME(2, 30)) || (currentTime >= CLOCK_TIME(6, 00))) {
                // It's before 2:30 AM on the first day, so the invasion hasn't started yet.
                sInvasionState = INVASION_STATE_WAIT;
            } else if (currentTime < CLOCK_TIME(5, 15)) {
                s32 i;
                s32 firstSpawn = CLOCK_TIME(5, 15);
                s32 spawnTime;

                for (i = 0; i < this->alienCount; i++) {
                    spawnTime = EnInvadepoh_Alien_GetSpawnTime(i);
                    firstSpawn = MIN(spawnTime, firstSpawn);
                }
                GameInteractor_Should(VB_SET_ALIEN_SPEED, false, &firstSpawn, 0);

                if (currentTime < (firstSpawn + (80 * CLOCK_TIME_MINUTE) + 1)) {
                    // The alien with the earliest spawn time hasn't reached the barn, so the invasion is ongoing.
                    sInvasionState = INVASION_STATE_ACTIVE;
                }
            }
        }

        if (sInvasionState == INVASION_STATE_NONE) {
            // The only way to reach this point is if the invasion is over, so check to see if the player successfully
            // defended the ranch to determine the state of the invasion.
            if (CHECK_WEEKEVENTREG(WEEKEVENTREG_DEFENDED_AGAINST_ALIENS)) {
                sInvasionState = INVASION_STATE_SUCCESS;
            } else {
                sInvasionState = INVASION_STATE_FAILURE;
            }
        }
    }
}

void EnInvadepoh_InvasionHandler_SpawnAliens(EnInvadepoh* this, PlayState* play) {
    s32 pathIndex = EN_INVADEPOH_GET_PATH(&this->actor);
    s32 i;

    for (i = 0; i < this->alienCount; i++) {
        sAliens[i] = (EnInvadepoh*)Actor_Spawn(&play->actorCtx, play, ACTOR_EN_INVADEPOH, this->actor.world.pos.x,
                                               this->actor.world.pos.y, this->actor.world.pos.z, 0, 0, 0,
                                               EN_INVADEPOH_PARAMS(pathIndex, EN_INVADEPOH_TYPE_ALIEN, i));

        if (pathIndex != EN_INVADEPOH_PATH_INDEX_NONE) {
            Path* path = &play->setupPathList[pathIndex];

            pathIndex = path->additionalPathIndex;
        }
    }
}

void EnInvadepoh_AlienAbductor_SpawnCow(EnInvadepoh* this, PlayState* play, s32 index) {
    Actor_SpawnAsChild(&play->actorCtx, &this->actor, play, ACTOR_EN_INVADEPOH, this->actor.home.pos.x,
                       this->actor.home.pos.y, this->actor.home.pos.z, 0, 0, 0,
                       EN_INVADEPOH_PARAMS(0, EN_INVADEPOH_TYPE_COW, index));
}

void EnInvadepoh_AlienAbductor_SpawnRomani(EnInvadepoh* this, PlayState* play) {
    Actor_SpawnAsChild(&play->actorCtx, &this->actor, play, ACTOR_EN_INVADEPOH, this->actor.home.pos.x,
                       this->actor.home.pos.y, this->actor.home.pos.z, 0, 0, 0,
                       EN_INVADEPOH_PARAMS(0, EN_INVADEPOH_TYPE_ROMANI_ABDUCTED, 0));
}

void EnInvadepoh_InvasionHandler_SpawnUfo(EnInvadepoh* this, PlayState* play) {
    sUfo = (EnInvadepoh*)Actor_Spawn(&play->actorCtx, play, ACTOR_EN_INVADEPOH, this->actor.world.pos.x,
                                     this->actor.world.pos.y, this->actor.world.pos.z, 0, 0, 0,
                                     EN_INVADEPOH_PARAMS(0, EN_INVADEPOH_TYPE_UFO, 0));
}

void EnInvadepoh_Ufo_SetMatrixTranslation(Vec3f* translation) {
    MtxF* currentMatrix = Matrix_GetCurrent();

    currentMatrix->xw = translation->x;
    currentMatrix->yw = translation->y;
    currentMatrix->zw = translation->z;
}

s32 EnInvadepoh_Romani_OpenDoor(EnInvadepoh* this, PlayState* play, f32 rangeSq, s32 doorTimer) {
    Actor* doorActor = play->actorCtx.actorLists[ACTORCAT_DOOR].first;
    EnDoor* door;
    s32 doorOpened = false;

    while (doorActor != NULL) {
        if ((doorActor->id == ACTOR_EN_DOOR) && (doorActor->update != NULL)) {
            door = (EnDoor*)doorActor;

            if ((door->knobDoor.dyna.actor.room == this->actor.room) &&
                (Math3D_Vec3fDistSq(&door->knobDoor.dyna.actor.world.pos, &this->actor.world.pos) < rangeSq)) {

                door->openTimer = doorTimer;
                doorOpened = true;
                break;
            }
        }

        doorActor = doorActor->next;
    }

    return doorOpened;
}

void EnInvadepoh_Romani_StartTextbox(EnInvadepoh* this, PlayState* play, u16 textId) {
    this->textId = textId;
    Message_StartTextbox(play, textId, &this->actor);
}

void EnInvadepoh_InvasionHandler_SetCutscenes(EnInvadepoh* this) {
    s16 csId = this->actor.csId;
    s32 i;

    for (i = 0; i < ARRAY_COUNT(sInvadepohCsIdList); i++) {
        sInvadepohCsIdList[i] = csId;
        csId = CutsceneManager_GetAdditionalCsId(csId);
    }
}

s32 EnInvadepoh_LensFlareCheck(PlayState* play, Vec3f* pos) {
    Vec3f projectedPos;
    f32 invW;

    Actor_GetProjectedPos(play, pos, &projectedPos, &invW);

    if ((projectedPos.z > 1.0f) && (fabsf(projectedPos.x * invW) < 1.0f) && (fabsf(projectedPos.y * invW) < 1.0f)) {
        f32 screenPosX = PROJECTED_TO_SCREEN_X(projectedPos, invW);
        f32 screenPosY = PROJECTED_TO_SCREEN_Y(projectedPos, invW);
        s32 wZ = (s32)(projectedPos.z * invW * ((G_MAXZ / 2) * 32)) + ((G_MAXZ / 2) * 32);

        if (wZ < SysCfb_GetZBufferInt(screenPosX, screenPosY)) {
            return true;
        }
    }

    return false;
}

/**
 * This function calculates how close every living alien is to the barn; it actually checks how close they are to the
 * invasion handler, but that is spawned within the barn. If the alien closest to the barn is fewer than 2000 units
 * away, then this function will set the `ALIEN_STATE_FLAG_CLOSEST_THREAT` flag on that alien and clear that flag on all
 * other aliens. If every living alien is *greater* than 2000 units away from the barn, however, then this function does
 * not think any alien is "threatening", so no alien will have the `ALIEN_STATE_FLAG_CLOSEST_THREAT` flag set.
 */
void EnInvadepoh_InvasionHandler_SetClosestAlienThreat(EnInvadepoh* this) {
    s32 i;
    f32 minDistSqToBarn = FLT_MAX;
    s32 closestAlienIndex = -1;

    for (i = 0; i < this->alienCount; i++) {
        if ((sAliens[i] != NULL) && sAliens[i]->shouldDraw) {
            f32 distSqToBarn = Math3D_Vec3fDistSq(&sAliens[i]->actor.world.pos, &this->actor.world.pos);

            if (minDistSqToBarn > distSqToBarn) {
                minDistSqToBarn = distSqToBarn;
                closestAlienIndex = i;
            }
        }

        sAlienStateFlags[i] &= ~ALIEN_STATE_FLAG_CLOSEST_THREAT;
    }

    sAliensExtremeThreat = false;

    if (minDistSqToBarn <= SQ(2000.0f)) {
        sAlienStateFlags[closestAlienIndex] |= ALIEN_STATE_FLAG_CLOSEST_THREAT;

        if (minDistSqToBarn <= SQ(340.0f)) {
            sAliensExtremeThreat = true;
        }
    }
}

/**
 * Returns a pointer to the alien that is the most threatening to the barn (in other words, the alien that is the
 * closest). However, it is possible for no alien to actually be considered a threat; this can be the case if every
 * living alien is greater than 2000 units away from the barn. In that case, this function will return NULL.
 */
EnInvadepoh* EnInvadepoh_Dog_GetClosestAlienThreat(void) {
    s32 i;

    for (i = 0; i < ALIEN_COUNT; i++) {
        if (sAlienStateFlags[i] & ALIEN_STATE_FLAG_CLOSEST_THREAT) {
            return sAliens[i];
        }
    }

    return NULL;
}

typedef enum RomaniEyeAnimation {
    /* 0 */ ROMANI_EYE_ANIM_OPEN, // unused
    /* 1 */ ROMANI_EYE_ANIM_OPEN_THEN_RANDOM_BLINK,
    /* 2 */ ROMANI_EYE_ANIM_FAST_BLINK,
    /* 3 */ ROMANI_EYE_ANIM_MEDIUM_BLINK,
    /* 4 */ ROMANI_EYE_ANIM_SLOW_BLINK,
    /* 5 */ ROMANI_EYE_ANIM_DOUBLE_BLINK,
    /* 6 */ ROMANI_EYE_ANIM_HALF,
    /* 7 */ ROMANI_EYE_ANIM_HAPPY, // unused
    /* 8 */ ROMANI_EYE_ANIM_MAX
} RomaniEyeAnimation;

static s8 sRomaniEyeOpenIndices[] = { ROMANI_EYE_OPEN };

static s8 sRomaniFastBlinkIndices[] = { ROMANI_EYE_OPEN, ROMANI_EYE_HALF, ROMANI_EYE_CLOSED, ROMANI_EYE_OPEN };

static s8 sRomaniMediumBlinkIndices[] = {
    ROMANI_EYE_OPEN, ROMANI_EYE_HALF, ROMANI_EYE_CLOSED, ROMANI_EYE_HALF, ROMANI_EYE_OPEN,
};

static s8 sRomaniSlowBlinkIndices[] = {
    ROMANI_EYE_OPEN, ROMANI_EYE_HALF, ROMANI_EYE_CLOSED, ROMANI_EYE_CLOSED, ROMANI_EYE_HALF, ROMANI_EYE_OPEN,
};

static s8 sRomaniDoubleBlinkIndices[] = {
    ROMANI_EYE_OPEN, ROMANI_EYE_HALF, ROMANI_EYE_CLOSED, ROMANI_EYE_HALF,
    ROMANI_EYE_OPEN, ROMANI_EYE_HALF, ROMANI_EYE_CLOSED, ROMANI_EYE_OPEN,
};

static s8 sRomaniEyeHalfIndices[] = { ROMANI_EYE_HALF };

static s8 sRomaniEyeHappyIndices[] = { ROMANI_EYE_HAPPY };

static EnInvadepohFaceFrames sRomaniEyeOpenFrames = {
    sRomaniEyeOpenIndices,
    ARRAY_COUNT(sRomaniEyeOpenIndices),
};

static EnInvadepohFaceFrames sRomaniFastBlinkFrames = {
    sRomaniFastBlinkIndices,
    ARRAY_COUNT(sRomaniFastBlinkIndices),
};

static EnInvadepohFaceFrames sRomaniMediumBlinkFrames = {
    sRomaniMediumBlinkIndices,
    ARRAY_COUNT(sRomaniMediumBlinkIndices),
};

static EnInvadepohFaceFrames sRomaniSlowBlinkFrames = {
    sRomaniSlowBlinkIndices,
    ARRAY_COUNT(sRomaniSlowBlinkIndices),
};

static EnInvadepohFaceFrames sRomaniDoubleBlinkFrames = {
    sRomaniDoubleBlinkIndices,
    ARRAY_COUNT(sRomaniDoubleBlinkIndices),
};

static EnInvadepohFaceFrames sRomaniEyeHalfFrames = {
    sRomaniEyeHalfIndices,
    ARRAY_COUNT(sRomaniEyeHalfIndices),
};

static EnInvadepohFaceFrames sRomaniEyeHappyFrames = {
    sRomaniEyeHappyIndices,
    ARRAY_COUNT(sRomaniEyeHappyIndices),
};

static EnInvadepohFaceAnimOnce sRomaniEyeOpenAnim = {
    { FACE_ANIMATION_TYPE_ONCE, &sRomaniEyeOpenFrames },
};

static EnInvadepohFaceAnimNext sRomaniRandomBlinkNext[] = {
    { ROMANI_EYE_ANIM_FAST_BLINK, 0.5f },
    { ROMANI_EYE_ANIM_MEDIUM_BLINK, 0.9f },
    { ROMANI_EYE_ANIM_SLOW_BLINK, 0.97f },
    { ROMANI_EYE_ANIM_DOUBLE_BLINK, 1.0f },
};

static EnInvadepohFaceAnimNext sRomaniEyeOpenNext[] = { ROMANI_EYE_ANIM_OPEN_THEN_RANDOM_BLINK, 1.0f };

static EnInvadepohFaceAnimChainedDelay sRomaniEyeOpenThenRandomBlinkAnim = {
    { FACE_ANIMATION_TYPE_CHAINED_DELAY, &sRomaniEyeOpenFrames },
    ARRAY_COUNT(sRomaniRandomBlinkNext),
    sRomaniRandomBlinkNext,
    40,
    60,
};

static EnInvadepohFaceAnimChained sRomaniFastBlinkAnim = {
    { FACE_ANIMATION_TYPE_CHAINED, &sRomaniFastBlinkFrames },
    ARRAY_COUNT(sRomaniEyeOpenNext),
    sRomaniEyeOpenNext,
};

static EnInvadepohFaceAnimChained sRomaniMediumBlinkAnim = {
    { FACE_ANIMATION_TYPE_CHAINED, &sRomaniMediumBlinkFrames },
    ARRAY_COUNT(sRomaniEyeOpenNext),
    sRomaniEyeOpenNext,
};

static EnInvadepohFaceAnimChained sRomaniSlowBlinkAnim = {
    { FACE_ANIMATION_TYPE_CHAINED, &sRomaniSlowBlinkFrames },
    ARRAY_COUNT(sRomaniEyeOpenNext),
    sRomaniEyeOpenNext,
};

static EnInvadepohFaceAnimChained sRomaniDoubleBlinkAnim = {
    { FACE_ANIMATION_TYPE_CHAINED, &sRomaniDoubleBlinkFrames },
    ARRAY_COUNT(sRomaniEyeOpenNext),
    sRomaniEyeOpenNext,
};

static EnInvadepohFaceAnimOnce sRomaniEyeHalfAnim = {
    { FACE_ANIMATION_TYPE_ONCE, &sRomaniEyeHalfFrames },
};

static EnInvadepohFaceAnimOnce sRomaniEyeHappyAnim = {
    { FACE_ANIMATION_TYPE_ONCE, &sRomaniEyeHappyFrames },
};

static EnInvadepohFaceAnimBase* sRomaniEyeAnimations[ROMANI_EYE_ANIM_MAX] = {
    &sRomaniEyeOpenAnim.base,                // ROMANI_EYE_ANIM_OPEN
    &sRomaniEyeOpenThenRandomBlinkAnim.base, // ROMANI_EYE_ANIM_OPEN_THEN_RANDOM_BLINK
    &sRomaniFastBlinkAnim.base,              // ROMANI_EYE_ANIM_FAST_BLINK
    &sRomaniMediumBlinkAnim.base,            // ROMANI_EYE_ANIM_MEDIUM_BLINK
    &sRomaniSlowBlinkAnim.base,              // ROMANI_EYE_ANIM_SLOW_BLINK
    &sRomaniDoubleBlinkAnim.base,            // ROMANI_EYE_ANIM_DOUBLE_BLINK
    &sRomaniEyeHalfAnim.base,                // ROMANI_EYE_ANIM_HALF
    &sRomaniEyeHappyAnim.base,               // ROMANI_EYE_ANIM_HAPPY
};

typedef enum RomaniMouthAnimation {
    /* 0 */ ROMANI_MOUTH_ANIM_HAPPY, // unused
    /* 1 */ ROMANI_MOUTH_ANIM_FROWN,
    /* 2 */ ROMANI_MOUTH_ANIM_HANGING_OPEN,
    /* 3 */ ROMANI_MOUTH_ANIM_SMILE,
    /* 4 */ ROMANI_MOUTH_ANIM_MAX
} RomaniMouthAnimation;

static s8 sRomaniMouthHappyIndices[] = { ROMANI_MOUTH_HAPPY };

static s8 sRomaniMouthFrownIndices[] = { ROMANI_MOUTH_FROWN };

static s8 sRomaniMouthHangingOpenIndices[] = { ROMANI_MOUTH_HANGING_OPEN };

static s8 sRomaniMouthSmileIndices[] = { ROMANI_MOUTH_SMILE };

static EnInvadepohFaceFrames sRomaniMouthHappyFrames = {
    sRomaniMouthHappyIndices,
    ARRAY_COUNT(sRomaniMouthHappyIndices),
};

static EnInvadepohFaceFrames sRomaniMouthFrownFrames = {
    sRomaniMouthFrownIndices,
    ARRAY_COUNT(sRomaniMouthFrownIndices),
};

static EnInvadepohFaceFrames sRomaniMouthHangingOpenFrames = {
    sRomaniMouthHangingOpenIndices,
    ARRAY_COUNT(sRomaniMouthHangingOpenIndices),
};

static EnInvadepohFaceFrames sRomaniMouthSmileFrames = {
    sRomaniMouthSmileIndices,
    ARRAY_COUNT(sRomaniMouthSmileIndices),
};

static EnInvadepohFaceAnimOnce sRomaniMouthHappyAnim = {
    { FACE_ANIMATION_TYPE_ONCE, &sRomaniMouthHappyFrames },
};

static EnInvadepohFaceAnimOnce sRomaniMouthFrownAnim = {
    { FACE_ANIMATION_TYPE_ONCE, &sRomaniMouthFrownFrames },
};

static EnInvadepohFaceAnimOnce sRomaniMouthHangingOpenAnim = {
    { FACE_ANIMATION_TYPE_ONCE, &sRomaniMouthHangingOpenFrames },
};

static EnInvadepohFaceAnimOnce sRomaniMouthSmileAnim = {
    { FACE_ANIMATION_TYPE_ONCE, &sRomaniMouthSmileFrames },
};

static EnInvadepohFaceAnimBase* sRomaniMouthAnimations[ROMANI_MOUTH_ANIM_MAX] = {
    &sRomaniMouthHappyAnim.base,       // ROMANI_MOUTH_ANIM_HAPPY
    &sRomaniMouthFrownAnim.base,       // ROMANI_MOUTH_ANIM_FROWN
    &sRomaniMouthHangingOpenAnim.base, // ROMANI_MOUTH_ANIM_HANGING_OPEN
    &sRomaniMouthSmileAnim.base,       // ROMANI_MOUTH_ANIM_SMILE
};

typedef enum CremiaEyeAnimation {
    /* 0 */ CREMIA_EYE_ANIM_OPEN, // unused
    /* 1 */ CREMIA_EYE_ANIM_OPEN_THEN_RANDOM_BLINK,
    /* 2 */ CREMIA_EYE_ANIM_FAST_BLINK,
    /* 3 */ CREMIA_EYE_ANIM_MEDIUM_BLINK,
    /* 4 */ CREMIA_EYE_ANIM_SLOW_BLINK,
    /* 5 */ CREMIA_EYE_ANIM_DOUBLE_BLINK,
    /* 6 */ CREMIA_EYE_ANIM_MAX
} CremiaEyeAnimation;

static s8 sCremiaEyeOpenIndices[] = { CREMIA_EYE_OPEN };

static s8 sCremiaFastBlinkIndices[] = { CREMIA_EYE_OPEN, CREMIA_EYE_HALF, CREMIA_EYE_CLOSED, CREMIA_EYE_OPEN };

static s8 sCremiaMediumBlinkIndices[] = {
    CREMIA_EYE_OPEN, CREMIA_EYE_HALF, CREMIA_EYE_CLOSED, CREMIA_EYE_HALF, CREMIA_EYE_OPEN,
};

static s8 sCremiaSlowBlinkIndices[] = {
    CREMIA_EYE_OPEN, CREMIA_EYE_HALF, CREMIA_EYE_CLOSED, CREMIA_EYE_CLOSED, CREMIA_EYE_HALF, CREMIA_EYE_OPEN,
};

static s8 sCremiaDoubleBlinkIndices[] = {
    CREMIA_EYE_OPEN, CREMIA_EYE_HALF, CREMIA_EYE_CLOSED, CREMIA_EYE_HALF,
    CREMIA_EYE_OPEN, CREMIA_EYE_HALF, CREMIA_EYE_CLOSED, CREMIA_EYE_OPEN,
};

static EnInvadepohFaceFrames sCremiaEyeOpenFrames = {
    sCremiaEyeOpenIndices,
    ARRAY_COUNT(sCremiaEyeOpenIndices),
};

static EnInvadepohFaceFrames sCremiaFastBlinkFrames = {
    sCremiaFastBlinkIndices,
    ARRAY_COUNT(sCremiaFastBlinkIndices),
};

static EnInvadepohFaceFrames sCremiaMediumBlinkFrames = {
    sCremiaMediumBlinkIndices,
    ARRAY_COUNT(sCremiaMediumBlinkIndices),
};

static EnInvadepohFaceFrames sCremiaSlowBlinkFrames = {
    sCremiaSlowBlinkIndices,
    ARRAY_COUNT(sCremiaSlowBlinkIndices),
};

static EnInvadepohFaceFrames sCremiaDoubleBlinkFrames = {
    sCremiaDoubleBlinkIndices,
    ARRAY_COUNT(sCremiaDoubleBlinkIndices),
};

static EnInvadepohFaceAnimOnce sCremiaEyeOpenAnim = {
    { FACE_ANIMATION_TYPE_ONCE, &sCremiaEyeOpenFrames },
};

static EnInvadepohFaceAnimNext sCremiaRandomBlinkNext[] = {
    { CREMIA_EYE_ANIM_FAST_BLINK, 0.5f },
    { CREMIA_EYE_ANIM_MEDIUM_BLINK, 0.9f },
    { CREMIA_EYE_ANIM_SLOW_BLINK, 0.95f },
    { CREMIA_EYE_ANIM_DOUBLE_BLINK, 1.0f },
};

static EnInvadepohFaceAnimNext sCremiaEyeOpenNext[] = { CREMIA_EYE_ANIM_OPEN_THEN_RANDOM_BLINK, 1.0f };

static EnInvadepohFaceAnimChainedDelay sCremiaEyeOpenThenRandomBlinkAnim = {
    { FACE_ANIMATION_TYPE_CHAINED_DELAY, &sCremiaEyeOpenFrames },
    ARRAY_COUNT(sCremiaRandomBlinkNext),
    sCremiaRandomBlinkNext,
    40,
    60,
};

static EnInvadepohFaceAnimChained sCremiaFastBlinkAnim = {
    { FACE_ANIMATION_TYPE_CHAINED, &sCremiaFastBlinkFrames },
    ARRAY_COUNT(sCremiaEyeOpenNext),
    sCremiaEyeOpenNext,
};

static EnInvadepohFaceAnimChained sCremiaMediumBlinkAnim = {
    { FACE_ANIMATION_TYPE_CHAINED, &sCremiaMediumBlinkFrames },
    ARRAY_COUNT(sCremiaEyeOpenNext),
    sCremiaEyeOpenNext,
};

static EnInvadepohFaceAnimChained sCremiaSlowBlinkAnim = {
    { FACE_ANIMATION_TYPE_CHAINED, &sCremiaSlowBlinkFrames },
    ARRAY_COUNT(sCremiaEyeOpenNext),
    sCremiaEyeOpenNext,
};

static EnInvadepohFaceAnimChained sCremiaDoubleBlinkAnim = {
    { FACE_ANIMATION_TYPE_CHAINED, &sCremiaDoubleBlinkFrames },
    ARRAY_COUNT(sCremiaEyeOpenNext),
    sCremiaEyeOpenNext,
};

static EnInvadepohFaceAnimBase* sCremiaEyeAnimations[CREMIA_EYE_ANIM_MAX] = {
    &sCremiaEyeOpenAnim.base,                // CREMIA_EYE_ANIM_OPEN
    &sCremiaEyeOpenThenRandomBlinkAnim.base, // CREMIA_EYE_ANIM_OPEN_THEN_RANDOM_BLINK
    &sCremiaFastBlinkAnim.base,              // CREMIA_EYE_ANIM_FAST_BLINK
    &sCremiaMediumBlinkAnim.base,            // CREMIA_EYE_ANIM_MEDIUM_BLINK
    &sCremiaSlowBlinkAnim.base,              // CREMIA_EYE_ANIM_SLOW_BLINK
    &sCremiaDoubleBlinkAnim.base,            // CREMIA_EYE_ANIM_DOUBLE_BLINK
};

typedef enum CremiaMouthAnimation {
    /* 0 */ CREMIA_MOUTH_ANIM_NORMAL,
    /* 1 */ CREMIA_MOUTH_ANIM_MAX
} CremiaMouthAnimation;

static s8 sCremiaMouthNormalIndices[] = { CREMIA_MOUTH_NORMAL };

static EnInvadepohFaceFrames sCremiaMouthNormalFrames = {
    sCremiaMouthNormalIndices,
    ARRAY_COUNT(sCremiaMouthNormalIndices),
};

static EnInvadepohFaceAnimOnce sCremiaMouthNormalAnim = {
    { FACE_ANIMATION_TYPE_ONCE, &sCremiaMouthNormalFrames },
};

static EnInvadepohFaceAnimBase* sCremiaMouthAnimations[CREMIA_MOUTH_ANIM_MAX] = {
    &sCremiaMouthNormalAnim.base, // CREMIA_MOUTH_ANIM_NORMAL
};

s8 EnInvadepoh_ModelInfo_GetNextFaceAnim(EnInvadepohFaceAnimNext* nextAnims, s32 nextAnimCount) {
    f32 rand = Rand_ZeroOne();
    EnInvadepohFaceAnimNext* nextAnim = nextAnims;
    s32 nextIndex;

    nextAnimCount--;
    for (nextIndex = 0; nextIndex < nextAnimCount; nextIndex++, nextAnim++) {
        if (rand <= nextAnim->chance) {
            break;
        }
    }

    nextAnim = &nextAnims[nextIndex];
    return nextAnim->index;
}

void EnInvadepoh_ModelInfo_SetNextFaceAnim(EnInvadepohFaceAnimInfo* faceInfo, EnInvadepohFaceAnimBase* faceAnim) {
    faceInfo->type = faceAnim->type;
    faceInfo->curAnim = faceAnim;
    faceInfo->curFrame = 0;
    faceInfo->curTexIndex = faceAnim->frames->texIndex[0];

    if (faceInfo->type == FACE_ANIMATION_TYPE_CHAINED_DELAY) {
        EnInvadepohFaceAnimChainedDelay* chainedDelayFaceAnim = (EnInvadepohFaceAnimChainedDelay*)faceAnim;

        faceInfo->delayTimer = Rand_S16Offset(chainedDelayFaceAnim->minDelay, chainedDelayFaceAnim->maxDelay);
    }
}

/**
 * Simply plays the face animation to its conclusion once. When the animation runs out of frames, the eyes or mouth will
 * remain on the last frame of the animation until the animation is changed.
 */
void EnInvadepoh_ModelInfo_UpdateFaceAnimOnce(EnInvadepohFaceAnimInfo* faceInfo, EnInvadepohFaceAnimBase** animations) {
    EnInvadepohFaceAnimOnce* faceAnim = (EnInvadepohFaceAnimOnce*)faceInfo->curAnim;
    EnInvadepohFaceFrames* faceFrames = faceAnim->base.frames;

    if (faceInfo->curFrame < (faceFrames->count - 1)) {
        faceInfo->curFrame++;
        faceInfo->curTexIndex = faceFrames->texIndex[faceInfo->curFrame];
    }
}

/**
 * Plays the face animation to its conclusion, then sets the next face animation based on what the current animation
 * says should come next.
 */
void EnInvadepoh_ModelInfo_UpdateFaceAnimChained(EnInvadepohFaceAnimInfo* faceInfo,
                                                 EnInvadepohFaceAnimBase** animations) {
    EnInvadepohFaceAnimChained* chainedFaceAnim = (EnInvadepohFaceAnimChained*)faceInfo->curAnim;
    EnInvadepohFaceFrames* faceFrames = chainedFaceAnim->base.frames;

    if (faceInfo->curFrame < (faceFrames->count - 1)) {
        faceInfo->curFrame++;
        faceInfo->curTexIndex = faceFrames->texIndex[faceInfo->curFrame];
    } else {
        EnInvadepohFaceAnimBase* nextAnim =
            animations[EnInvadepoh_ModelInfo_GetNextFaceAnim(chainedFaceAnim->nextAnims, chainedFaceAnim->nextCount)];

        EnInvadepoh_ModelInfo_SetNextFaceAnim(faceInfo, nextAnim);
    }
}

/**
 * Plays the face animation to its conclusion, then waits until the delay timer reaches 0; during this time, the eyes or
 * mouth will remain on the last frame of the animation. Once the timer runs out, this will set the next face animation
 * based on what the current animation says should come next.
 */
void EnInvadepoh_ModelInfo_UpdateFaceAnimChainedDelay(EnInvadepohFaceAnimInfo* faceInfo,
                                                      EnInvadepohFaceAnimBase** animations) {
    EnInvadepohFaceAnimChainedDelay* chainedDelayFaceAnim = (EnInvadepohFaceAnimChainedDelay*)faceInfo->curAnim;
    EnInvadepohFaceFrames* faceFrames = chainedDelayFaceAnim->base.frames;

    if (faceInfo->curFrame < (faceFrames->count - 1)) {
        faceInfo->curFrame++;
        faceInfo->curTexIndex = faceFrames->texIndex[faceInfo->curFrame];
    } else if (faceInfo->delayTimer > 0) {
        faceInfo->delayTimer--;
    } else {
        EnInvadepohFaceAnimBase* nextAnim = animations[EnInvadepoh_ModelInfo_GetNextFaceAnim(
            chainedDelayFaceAnim->nextAnims, chainedDelayFaceAnim->nextCount)];

        EnInvadepoh_ModelInfo_SetNextFaceAnim(faceInfo, nextAnim);
    }
}

void EnInvadepoh_ModelInfo_UpdateFaceAnim(EnInvadepohFaceAnimInfo* faceInfo) {
    static EnInvadepohFaceAnimFunc sFaceAnimUpdateFuncs[FACE_ANIMATION_TYPE_MAX] = {
        EnInvadepoh_ModelInfo_UpdateFaceAnimOnce,
        EnInvadepoh_ModelInfo_UpdateFaceAnimChained,
        EnInvadepoh_ModelInfo_UpdateFaceAnimChainedDelay,
    };

    if ((faceInfo->animations != NULL) && (faceInfo->curAnim != NULL)) {
        sFaceAnimUpdateFuncs[faceInfo->type](faceInfo, faceInfo->animations);
    }
}

void EnInvadepoh_ModelInfo_Init(EnInvadepohModelInfo* modelInfo, EnInvadepohFaceAnimBase** eyeAnimations,
                                s32 eyeAnimIndex, EnInvadepohFaceAnimBase** mouthAnimations, s32 mouthAnimIndex,
                                Vec3s* headRotTarget, s16 headRotMaxStep, f32 headRotStepScale,
                                f32 torsoRotScaleTargetY, f32 torsoRotScaleStepY) {
    Math_Vec3s_Copy(&modelInfo->headRotTarget, headRotTarget);
    modelInfo->headRotMaxStep = headRotMaxStep;
    modelInfo->headRotStepScale = headRotStepScale;
    modelInfo->torsoRotScaleTargetY = torsoRotScaleTargetY;
    modelInfo->torsoRotScaleStepY = torsoRotScaleStepY;

    if (eyeAnimations != NULL) {
        EnInvadepohFaceAnimBase* eyeAnim = eyeAnimations[eyeAnimIndex];

        modelInfo->eyeAnim.animations = eyeAnimations;
        modelInfo->eyeAnim.type = eyeAnim->type;
        modelInfo->eyeAnim.curAnim = eyeAnim;
        modelInfo->eyeAnim.delayTimer = 0;
        modelInfo->eyeAnim.curFrame = 0;
        modelInfo->eyeAnim.curTexIndex = eyeAnim->frames->texIndex[0];
    }

    if (mouthAnimations != NULL) {
        EnInvadepohFaceAnimBase* mouthAnim = mouthAnimations[mouthAnimIndex];

        modelInfo->mouthAnim.animations = mouthAnimations;
        modelInfo->mouthAnim.type = mouthAnim->type;
        modelInfo->mouthAnim.curAnim = mouthAnim;
        modelInfo->mouthAnim.delayTimer = 0;
        modelInfo->mouthAnim.curFrame = 0;
        modelInfo->mouthAnim.curTexIndex = mouthAnim->frames->texIndex[0];
    }
}

void EnInvadepoh_ModelInfo_Update(EnInvadepohModelInfo* modelInfo) {
    Vec3f scaledRotStep;
    Vec3s rotStep;

    rotStep.x = modelInfo->headRotTarget.x - modelInfo->headRot.x;
    rotStep.y = modelInfo->headRotTarget.y - modelInfo->headRot.y;
    rotStep.z = modelInfo->headRotTarget.z - modelInfo->headRot.z;

    Math_Vec3s_ToVec3f(&scaledRotStep, &rotStep);
    Math_Vec3f_Scale(&scaledRotStep, modelInfo->headRotStepScale);
    Math_Vec3f_ToVec3s(&rotStep, &scaledRotStep);

    rotStep.x = ABS(rotStep.x);
    rotStep.y = ABS(rotStep.y);
    rotStep.z = ABS(rotStep.z);

    rotStep.x = CLAMP_MAX(rotStep.x, modelInfo->headRotMaxStep);
    rotStep.y = CLAMP_MAX(rotStep.y, modelInfo->headRotMaxStep);
    rotStep.z = CLAMP_MAX(rotStep.z, modelInfo->headRotMaxStep);

    Math_ScaledStepToS(&modelInfo->headRot.x, modelInfo->headRotTarget.x, rotStep.x);
    Math_ScaledStepToS(&modelInfo->headRot.y, modelInfo->headRotTarget.y, rotStep.y);
    Math_ScaledStepToS(&modelInfo->headRot.z, modelInfo->headRotTarget.z, rotStep.z);
    Math_StepToF(&modelInfo->torsoRotScaleY, modelInfo->torsoRotScaleTargetY, modelInfo->torsoRotScaleStepY);

    if (modelInfo->torsoRotX != modelInfo->torsoTargetRotX) {
        s16 torsoRotStep = modelInfo->torsoTargetRotX - modelInfo->torsoRotX;

        torsoRotStep *= modelInfo->torsoRotStepScale;
        torsoRotStep = ABS(torsoRotStep);
        torsoRotStep = CLAMP(torsoRotStep, 0x64, modelInfo->torsoRotMaxStep);
        Math_ScaledStepToS(&modelInfo->torsoRotX, modelInfo->torsoTargetRotX, torsoRotStep);
    }

    EnInvadepoh_ModelInfo_UpdateFaceAnim(&modelInfo->eyeAnim);
    EnInvadepoh_ModelInfo_UpdateFaceAnim(&modelInfo->mouthAnim);
}

void EnInvadepoh_Ufo_SpawnSparkles(EnInvadepoh* this, PlayState* play, s32 spawnCount) {
    static Color_RGBA8 sPrimColor = { 255, 255, 200, 255 };
    static Color_RGBA8 sEnvColor = { 255, 200, 0, 0 };
    s32 i;
    Vec3f pos;
    Vec3f velocity;
    Vec3f accel;
    f32 offset;
    s16 angle = 0;

    for (i = 0; i < spawnCount; i++) {
        angle += (s16)(0x10000 * 1.0f / spawnCount);
        offset = (Rand_ZeroOne() * 0.5f) + 0.5f;

        pos.x = Math_SinS(angle) * offset;
        pos.z = Math_CosS(angle) * offset;

        velocity.x = (Rand_ZeroOne() * 16.0f) + (pos.x * 30.0f) - 8.0f;
        velocity.y = -8.0f;
        velocity.z = (Rand_ZeroOne() * 16.0f) + (pos.z * 30.0f) - 8.0f;

        accel.x = velocity.x * -0.025f;
        accel.y = velocity.y * -0.025f;
        accel.z = velocity.z * -0.025f;

        pos.x = (pos.x * 100.0f) + this->actor.world.pos.x;
        pos.y = (Rand_ZeroOne() * 180.0f) + this->actor.world.pos.y - 90.0f;
        pos.z = (pos.z * 100.0f) + this->actor.world.pos.z;

        EffectSsKirakira_SpawnDispersed(play, &pos, &velocity, &accel, &sPrimColor, &sEnvColor, 6000, -40);
    }
}

/**
 * Attempts to spawn an alien warp effect at the specified position. If at least one entry in the list of effects is
 * inactive (i.e., its timer is equal to less than or equal to zero), then this function will use that entry for the
 * warp effect and return true. Otherwise, it returns false.
 */
s32 EnInvadepoh_SpawnWarpEffect(Vec3f* pos) {
    EnInvadepohEffect* warpEffect = sEffects;
    s32 i;

    for (i = 0; i < EFFECT_COUNT; i++, warpEffect++) {
        if (warpEffect->timer <= 0) {
            warpEffect->type = EN_INVADEPOH_EFFECT_TYPE_WARP;
            warpEffect->timer = 40;
            Math_Vec3f_Copy(&warpEffect->pos, pos);
            warpEffect->alpha = 0;
            return true;
        }
    }

    return false;
}

/**
 * If the warp effect has been active for less than 20 frames, this function will gradually increase the alpha of the
 * warp effect until it reaches a value of 135. Otherwise, it will gradually decrease the alpha of the warp effect until
 * it reaches 0.
 */
void EnInvadepoh_UpdateWarpEffect(EnInvadepohEffect* warpEffect) {
    if (warpEffect->timer > 20) {
        if (warpEffect->alpha < 125) {
            warpEffect->alpha += 10;
        } else {
            warpEffect->alpha = 135;
        }
    } else {
        if (warpEffect->alpha > 13) {
            warpEffect->alpha -= 13;
        } else {
            warpEffect->alpha = 0;
        }
    }
}

/**
 * This function iterates through all effects and checks to see if the effect is active (i.e., the effect's timer is
 * greater than zero). If it is, then this function will update the effect and eventually return true. If no effect is
 * active, then this function will return false.
 */
s32 EnInvadepoh_UpdateEffects(void) {
    static EnInvadepohEffectUpdateFunc sEffectUpdateFuncs[EN_INVADEPOH_EFFECT_TYPE_MAX] = {
        EnInvadepoh_UpdateWarpEffect // EN_INVADEPOH_EFFECT_TYPE_WARP
    };
    s32 effectActive = false;
    s32 i;
    EnInvadepohEffect* effect;

    for (i = 0, effect = sEffects; i < EFFECT_COUNT; i++, effect++) {
        if (effect->timer > 0) {
            sEffectUpdateFuncs[effect->type](effect);
            effectActive = true;
            effect->timer--;
        }
    }

    return effectActive;
}

void EnInvadepoh_InvasionHandler_Init(EnInvadepoh* this, PlayState* play) {
    s32 alienCount;
    s32 pathIndex;

    this->actor.flags |= ACTOR_FLAG_DRAW_CULLING_DISABLED;
    pathIndex = EN_INVADEPOH_GET_PATH(&this->actor);

    for (alienCount = 1; alienCount < ALIEN_COUNT; alienCount++) {
        Path* path = &play->setupPathList[pathIndex];

        pathIndex = path->additionalPathIndex;
        if (pathIndex == EN_INVADEPOH_PATH_INDEX_NONE) {
            break;
        }
    }

    this->alienCount = alienCount;
    EnInvadepoh_InvasionHandler_SetInitialInvasionState(this, play);
    EnInvadepoh_InvasionHandler_SetCutscenes(this);
    Actor_ChangeCategory(play, &play->actorCtx, &this->actor, ACTORCAT_SWITCH);

    if (sInvasionState == INVASION_STATE_WAIT) {
        EnInvadepoh_InvasionHandler_SetupWaitForInvasion(this);
    } else if (sInvasionState == INVASION_STATE_ACTIVE) {
        if (CURRENT_TIME < CLOCK_TIME(2, 31)) {
            EnInvadepoh_InvasionHandler_SetupWaitForInvasion(this);
        } else {
            EnInvadepoh_InvasionHandler_SpawnUfo(this, play);
            EnInvadepoh_InvasionHandler_SpawnAliens(this, play);
            SEQCMD_PLAY_SEQUENCE(SEQ_PLAYER_BGM_MAIN, 0, NA_BGM_ALIEN_INVASION | SEQ_FLAG_ASYNC);
            EnInvadepoh_InvasionHandler_SetupHandle(this);
        }
    } else if (sInvasionState == INVASION_STATE_SUCCESS) {
        if (gSaveContext.save.entrance == ENTRANCE(ROMANI_RANCH, 6)) {
            EnInvadepoh_InvasionHandler_SetupWaitForRomaniReward(this);
        } else if (gSaveContext.save.entrance == ENTRANCE(ROMANI_RANCH, 7)) {
            EnInvadepoh_InvasionHandler_SetupSuccessEnd(this);
        } else {
            EnInvadepoh_InvasionHandler_SetupSuccessEnd(this);
        }
    } else if (sInvasionState == INVASION_STATE_FAILURE) {
        EnInvadepoh_InvasionHandler_SetupFailureEnd(this);
    }
}

static InitChainEntry sAlienInitChain[] = {
    ICHAIN_F32(cullingVolumeDistance, 20000, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeScale, 500, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeDownward, 600, ICHAIN_CONTINUE),
    ICHAIN_VEC3F_DIV1000(scale, 10, ICHAIN_STOP),
};

void EnInvadepoh_Alien_Init(EnInvadepoh* this, PlayState* play) {
    s32 pad;

    Actor_ProcessInitChain(&this->actor, sAlienInitChain);
    Collider_InitCylinder(play, &this->collider);
    ActorShape_Init(&this->actor.shape, 6800.0f, ActorShadow_DrawWhiteCircle, 150.0f);
    this->actor.shape.shadowAlpha = 140;
    this->actor.flags = ACTOR_FLAG_UPDATE_CULLING_DISABLED | ACTOR_FLAG_IGNORE_QUAKE | ACTOR_FLAG_MINIMAP_ICON_ENABLED;

    if (EN_INVADEPOH_GET_TYPE(&this->actor) == EN_INVADEPOH_TYPE_ALIEN_ABDUCTOR) {
        this->actor.update = EnInvadepoh_AlienAbductor_WaitForObject;
        this->actor.world.pos.y = this->actor.home.pos.y + 150.0f;
    } else {
        this->actor.update = EnInvadepoh_Alien_WaitForObject;
        Collider_SetCylinder(play, &this->collider, &this->actor, &sAlienCylinderInit);
        this->actor.colChkInfo.mass = 40;
    }

    this->objectSlot = Object_GetSlot(&play->objectCtx, OBJECT_UCH);
    if (this->objectSlot <= OBJECT_SLOT_NONE) {
        Actor_Kill(&this->actor);
    }
}

static InitChainEntry sCowInitChain[] = {
    ICHAIN_F32(cullingVolumeDistance, 20000, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeScale, 200, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeDownward, 300, ICHAIN_CONTINUE),
    ICHAIN_VEC3F_DIV1000(scale, 10, ICHAIN_STOP),
};

void EnInvadepoh_Cow_Init(EnInvadepoh* this, PlayState* play) {
    Actor_ProcessInitChain(&this->actor, sCowInitChain);
    this->actor.update = EnInvadepoh_Cow_WaitForObject;
    Actor_SpawnAsChild(&play->actorCtx, &this->actor, play, ACTOR_EN_INVADEPOH, 0.0f, 0.0f, 0.0f, 0, 0, 0,
                       EN_INVADEPOH_PARAMS(0, EN_INVADEPOH_TYPE_COW_TAIL, 0));

    this->objectSlot = Object_GetSlot(&play->objectCtx, OBJECT_COW);
    if (this->objectSlot <= OBJECT_SLOT_NONE) {
        Actor_Kill(&this->actor);
    }
}

static InitChainEntry sCowTailInitChain[] = {
    ICHAIN_F32(cullingVolumeDistance, 20000, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeScale, 100, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeDownward, 100, ICHAIN_CONTINUE),
    ICHAIN_VEC3F_DIV1000(scale, 10, ICHAIN_STOP),
};

void EnInvadepoh_CowTail_Init(EnInvadepoh* this, PlayState* play) {
    Actor_ProcessInitChain(&this->actor, sCowTailInitChain);
    this->actor.update = EnInvadepoh_CowTail_WaitForObject;

    this->objectSlot = Object_GetSlot(&play->objectCtx, OBJECT_COW);
    if (this->objectSlot <= OBJECT_SLOT_NONE) {
        Actor_Kill(&this->actor);
    }
}

static InitChainEntry sRomaniInitChain[] = {
    ICHAIN_F32(cullingVolumeDistance, 20000, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeScale, 100, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeDownward, 100, ICHAIN_CONTINUE),
    ICHAIN_F32(lockOnArrowOffset, 1500, ICHAIN_CONTINUE),
    ICHAIN_VEC3F_DIV1000(scale, 10, ICHAIN_STOP),
};

void EnInvadepoh_Romani_Init(EnInvadepoh* this, PlayState* play) {
    s32 pad;
    s32 type = EN_INVADEPOH_GET_TYPE(&this->actor);

    Actor_ProcessInitChain(&this->actor, sRomaniInitChain);

    if ((type == EN_INVADEPOH_TYPE_ROMANI_NIGHT_1) || (type == EN_INVADEPOH_TYPE_ROMANI_NIGHT_3)) {
        this->actor.attentionRangeType = ATTENTION_RANGE_3;
    } else {
        this->actor.attentionRangeType = ATTENTION_RANGE_6;
    }

    Actor_ChangeCategory(play, &play->actorCtx, &this->actor, ACTORCAT_NPC);
    Collider_InitCylinder(play, &this->collider);

    if (type != EN_INVADEPOH_TYPE_ROMANI_ABDUCTED) {
        ActorShape_Init(&this->actor.shape, 0.0f, ActorShadow_DrawCircle, 18.0f);
        Collider_SetCylinder(play, &this->collider, &this->actor, &sRomaniAndCremiaCylinderInit);
        this->actor.colChkInfo.mass = MASS_IMMOVABLE;
    }

    if (type == EN_INVADEPOH_TYPE_ROMANI_ABDUCTED) {
        this->actor.update = EnInvadepoh_AbductedRomani_WaitForObject;
    } else if (type == EN_INVADEPOH_TYPE_ROMANI_SILENT) {
        this->actor.update = EnInvadepoh_SilentRomani_WaitForObject;
        this->actor.flags = ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_FRIENDLY | ACTOR_FLAG_UPDATE_CULLING_DISABLED;
    } else if (type == EN_INVADEPOH_TYPE_ROMANI_NIGHT_1) {
        this->actor.update = EnInvadepoh_Night1Romani_WaitForObject;
    } else if (type == EN_INVADEPOH_TYPE_ROMANI_BARN) {
        this->actor.update = EnInvadepoh_BarnRomani_WaitForObject;
    } else if (type == EN_INVADEPOH_TYPE_ROMANI_REWARD) {
        this->actor.update = EnInvadepoh_RewardRomani_WaitForObject;
    } else {
        this->actor.update = EnInvadepoh_Night3Romani_WaitForObject;
    }

    this->objectSlot = Object_GetSlot(&play->objectCtx, OBJECT_MA1);
    if (this->objectSlot <= OBJECT_SLOT_NONE) {
        Actor_Kill(&this->actor);
    }

    if (type == EN_INVADEPOH_TYPE_ROMANI_SILENT) {
        if (CHECK_WEEKEVENTREG(WEEKEVENTREG_DEFENDED_AGAINST_ALIENS)) {
            Actor_Kill(&this->actor);
        }
    } else if (type == EN_INVADEPOH_TYPE_ROMANI_NIGHT_1) {
        if ((CURRENT_TIME < CLOCK_TIME(6, 00)) && (CURRENT_TIME >= CLOCK_TIME(2, 15))) {
            Actor_Kill(&this->actor);
        }
    } else if (type == EN_INVADEPOH_TYPE_ROMANI_BARN) {
        // nothing
    } else if (type == EN_INVADEPOH_TYPE_ROMANI_REWARD) {
        if (gSaveContext.save.entrance != ENTRANCE(ROMANI_RANCH, 6)) {
            Actor_Kill(&this->actor);
        }
    } else if (type == EN_INVADEPOH_TYPE_ROMANI_NIGHT_3) {
        if (!CHECK_WEEKEVENTREG(WEEKEVENTREG_DEFENDED_AGAINST_ALIENS)) {
            Actor_Kill(&this->actor);
        }

        sNight3Romani = this;
    }
}

static InitChainEntry sUfoInitChain[] = {
    ICHAIN_F32(cullingVolumeDistance, 20000, ICHAIN_CONTINUE), ICHAIN_F32(cullingVolumeScale, 1000, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeDownward, 1000, ICHAIN_CONTINUE),  ICHAIN_VEC3S(shape, 0, ICHAIN_CONTINUE),
    ICHAIN_F32(terminalVelocity, -100, ICHAIN_CONTINUE),       ICHAIN_VEC3F_DIV1000(scale, 1000, ICHAIN_STOP),
};

void EnInvadepoh_Ufo_Init(EnInvadepoh* this, PlayState* play) {
    Actor_ProcessInitChain(&this->actor, sUfoInitChain);
    this->actor.update = EnInvadepoh_Ufo_Update;
    this->actor.draw = EnInvadepoh_Ufo_Draw;
    Actor_ChangeCategory(play, &play->actorCtx, &this->actor, ACTORCAT_NPC);

    if ((sInvasionState == INVASION_STATE_WAIT) || (CURRENT_TIME < CLOCK_TIME(2, 31))) {
        this->actor.world.pos.x += sUfoSpawnOffset.x;
        this->actor.world.pos.y += sUfoSpawnOffset.y + 3000.0f;
        this->actor.world.pos.z += sUfoSpawnOffset.z;
        EnInvadepoh_Ufo_SetupIntroDescend(this);
    } else if (sInvasionState == INVASION_STATE_ACTIVE) {
        this->actor.world.pos.y += 1500.0f;
        EnInvadepoh_Ufo_SetupHoverAboveBarn(this);
    } else {
        Actor_Kill(&this->actor);
    }
}

static InitChainEntry sDogInitChain[] = {
    ICHAIN_F32(cullingVolumeDistance, 2000, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeScale, 50, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeDownward, 50, ICHAIN_CONTINUE),
    ICHAIN_F32(gravity, -3, ICHAIN_CONTINUE),
    ICHAIN_U8(attentionRangeType, ATTENTION_RANGE_4, ICHAIN_CONTINUE),
    ICHAIN_VEC3F_DIV1000(scale, 7, ICHAIN_STOP),
};

void EnInvadepoh_Dog_Init(EnInvadepoh* this, PlayState* play) {
    s32 pad;

    Actor_ProcessInitChain(&this->actor, sDogInitChain);
    this->actor.update = EnInvadepoh_Dog_WaitForObject;
    Collider_InitCylinder(play, &this->collider);
    Collider_SetCylinder(play, &this->collider, &this->actor, &sDogCylinderInit);
    this->actor.colChkInfo.mass = 80;
    ActorShape_Init(&this->actor.shape, 0.0f, ActorShadow_DrawCircle, 24.0f);

    this->objectSlot = Object_GetSlot(&play->objectCtx, OBJECT_DOG);
    if (this->objectSlot <= OBJECT_SLOT_NONE) {
        Actor_Kill(&this->actor);
    }
}

static InitChainEntry sCremiaInitChain[] = {
    ICHAIN_F32(cullingVolumeDistance, 20000, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeScale, 100, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeDownward, 150, ICHAIN_CONTINUE),
    ICHAIN_F32(lockOnArrowOffset, 1500, ICHAIN_CONTINUE),
    ICHAIN_U8(attentionRangeType, ATTENTION_RANGE_3, ICHAIN_CONTINUE),
    ICHAIN_VEC3F_DIV1000(scale, 10, ICHAIN_STOP),
};

void EnInvadepoh_Cremia_Init(EnInvadepoh* this, PlayState* play) {
    s32 pad;

    Actor_ProcessInitChain(&this->actor, sCremiaInitChain);
    this->actor.update = EnInvadepoh_Night3Cremia_WaitForObject;
    Actor_ChangeCategory(play, &play->actorCtx, &this->actor, ACTORCAT_NPC);
    Collider_InitCylinder(play, &this->collider);
    Collider_SetCylinder(play, &this->collider, &this->actor, &sRomaniAndCremiaCylinderInit);
    this->actor.colChkInfo.mass = MASS_HEAVY;
    ActorShape_Init(&this->actor.shape, 0.0f, ActorShadow_DrawCircle, 18.0f);

    this->objectSlot = Object_GetSlot(&play->objectCtx, OBJECT_MA2);
    if (this->objectSlot <= OBJECT_SLOT_NONE) {
        Actor_Kill(&this->actor);
    }

    if (!CHECK_WEEKEVENTREG(WEEKEVENTREG_DEFENDED_AGAINST_ALIENS)) {
        Actor_Kill(&this->actor);
    }

    sNight3Cremia = this;
}

void EnInvadepoh_Init(Actor* thisx, PlayState* play2) {
    static EnInvadepohInitFunc sInitFuncs[EN_INVADEPOH_TYPE_MAX] = {
        EnInvadepoh_InvasionHandler_Init, // EN_INVADEPOH_TYPE_INVASION_HANDLER
        EnInvadepoh_Alien_Init,           // EN_INVADEPOH_TYPE_ALIEN
        EnInvadepoh_Cow_Init,             // EN_INVADEPOH_TYPE_COW
        EnInvadepoh_CowTail_Init,         // EN_INVADEPOH_TYPE_COW_TAIL
        EnInvadepoh_Romani_Init,          // EN_INVADEPOH_TYPE_ROMANI_ABDUCTED
        EnInvadepoh_Romani_Init,          // EN_INVADEPOH_TYPE_ROMANI_SILENT
        EnInvadepoh_Ufo_Init,             // EN_INVADEPOH_TYPE_UFO
        EnInvadepoh_Romani_Init,          // EN_INVADEPOH_TYPE_ROMANI_NIGHT_1
        EnInvadepoh_Romani_Init,          // EN_INVADEPOH_TYPE_ROMANI_BARN
        EnInvadepoh_Romani_Init,          // EN_INVADEPOH_TYPE_ROMANI_REWARD
        EnInvadepoh_Dog_Init,             // EN_INVADEPOH_TYPE_DOG
        EnInvadepoh_Cremia_Init,          // EN_INVADEPOH_TYPE_CREMIA_NIGHT_3
        EnInvadepoh_Romani_Init,          // EN_INVADEPOH_TYPE_ROMANI_NIGHT_3
        EnInvadepoh_Alien_Init,           // EN_INVADEPOH_TYPE_ALIEN_ABDUCTOR
    };
    PlayState* play = play2;
    EnInvadepoh* this = (EnInvadepoh*)thisx;

    sInitFuncs[EN_INVADEPOH_GET_TYPE(&this->actor)](this, play);
}

void EnInvadepoh_InvasionHandler_Destroy(EnInvadepoh* this, PlayState* play) {
}

void EnInvadepoh_Alien_Destroy(EnInvadepoh* this, PlayState* play) {
    s32 pad;
    s32 index = EN_INVADEPOH_GET_INDEX(&this->actor);

    Collider_DestroyCylinder(play, &this->collider);
    sAliens[index] = NULL;
}

void EnInvadepoh_Cow_Destroy(EnInvadepoh* this, PlayState* play) {
    if (this->actor.parent != NULL) {
        this->actor.parent->child = NULL;
    }

    if (this->actor.child != NULL) {
        this->actor.child->parent = NULL;
    }
}

void EnInvadepoh_CowTail_Destroy(EnInvadepoh* this, PlayState* play) {
    if (this->actor.parent != NULL) {
        this->actor.parent->child = NULL;
    }
}

void EnInvadepoh_Romani_Destroy(EnInvadepoh* this, PlayState* play) {
    Collider_DestroyCylinder(play, &this->collider);
}

void EnInvadepoh_Ufo_Destroy(EnInvadepoh* this, PlayState* play) {
    sUfo = NULL;
}

void EnInvadepoh_Dog_Destroy(EnInvadepoh* this, PlayState* play) {
    Collider_DestroyCylinder(play, &this->collider);
}

void EnInvadepoh_Cremia_Destroy(EnInvadepoh* this, PlayState* play) {
    Collider_DestroyCylinder(play, &this->collider);
    sNight3Cremia = NULL;
}

void EnInvadepoh_Night3Romani_Destroy(EnInvadepoh* this, PlayState* play) {
    Collider_DestroyCylinder(play, &this->collider);
    sNight3Romani = NULL;
}

void EnInvadepoh_AlienAbductor_Destroy(EnInvadepoh* this, PlayState* play) {
    Actor* thisx = &this->actor;

    Collider_DestroyCylinder(play, &this->collider);

    if (thisx->child != NULL) {
        thisx->child->parent = NULL;
    }
}

void EnInvadepoh_Destroy(Actor* thisx, PlayState* play2) {
    static EnInvadepohDestroyFunc sDestroyFuncs[EN_INVADEPOH_TYPE_MAX] = {
        EnInvadepoh_InvasionHandler_Destroy, // EN_INVADEPOH_TYPE_INVASION_HANDLER
        EnInvadepoh_Alien_Destroy,           // EN_INVADEPOH_TYPE_ALIEN
        EnInvadepoh_Cow_Destroy,             // EN_INVADEPOH_TYPE_COW
        EnInvadepoh_CowTail_Destroy,         // EN_INVADEPOH_TYPE_COW_TAIL
        EnInvadepoh_Romani_Destroy,          // EN_INVADEPOH_TYPE_ROMANI_ABDUCTED
        EnInvadepoh_Romani_Destroy,          // EN_INVADEPOH_TYPE_ROMANI_SILENT
        EnInvadepoh_Ufo_Destroy,             // EN_INVADEPOH_TYPE_UFO
        EnInvadepoh_Romani_Destroy,          // EN_INVADEPOH_TYPE_ROMANI_NIGHT_1
        EnInvadepoh_Romani_Destroy,          // EN_INVADEPOH_TYPE_ROMANI_BARN
        EnInvadepoh_Romani_Destroy,          // EN_INVADEPOH_TYPE_ROMANI_REWARD
        EnInvadepoh_Dog_Destroy,             // EN_INVADEPOH_TYPE_DOG
        EnInvadepoh_Cremia_Destroy,          // EN_INVADEPOH_TYPE_CREMIA_NIGHT_3
        EnInvadepoh_Night3Romani_Destroy,    // EN_INVADEPOH_TYPE_ROMANI_NIGHT_3
        EnInvadepoh_AlienAbductor_Destroy,   // EN_INVADEPOH_TYPE_ALIEN_ABDUCTOR
    };
    PlayState* play = play2;
    EnInvadepoh* this = (EnInvadepoh*)thisx;

    sDestroyFuncs[EN_INVADEPOH_GET_TYPE(&this->actor)](this, play);
}

void EnInvadepoh_InvasionHandler_SetupWaitForInvasion(EnInvadepoh* this) {
    sInvasionState = INVASION_STATE_WAIT;
    this->actionFunc = EnInvadepoh_InvasionHandler_WaitForInvasion;
}

/**
 * Waits until 2:30 AM, then spawns the UFO and Aliens and starts the invasion intro cutscene.
 */
void EnInvadepoh_InvasionHandler_WaitForInvasion(EnInvadepoh* this, PlayState* play) {
    if ((CURRENT_TIME < CLOCK_TIME(6, 00)) && (CURRENT_TIME >= CLOCK_TIME(2, 30))) {
        EnInvadepoh_InvasionHandler_SpawnUfo(this, play);
        EnInvadepoh_InvasionHandler_SpawnAliens(this, play);
        EnInvadepoh_InvasionHandler_SetupStartIntroCutscene(this);
    }
}

void EnInvadepoh_InvasionHandler_SetupStartIntroCutscene(EnInvadepoh* this) {
    sInvasionState = INVASION_STATE_ACTIVE;
    this->timer = 2;
    this->actionFunc = EnInvadepoh_InvasionHandler_StartIntroCutscene;
}

/**
 * Waits 2 frames, then queues up the invasion intro cutscene and starts it.
 */
void EnInvadepoh_InvasionHandler_StartIntroCutscene(EnInvadepoh* this, PlayState* play) {
    if (this->timer > 0) {
        this->timer--;
    } else if (CutsceneManager_IsNext(sInvadepohCsIdList[0])) {
        CutsceneManager_StartWithPlayerCs(sInvadepohCsIdList[0], &this->actor);
        EnInvadepoh_InvasionHandler_SetupIntroCutscene(this);
    } else {
        CutsceneManager_Queue(sInvadepohCsIdList[0]);
    }
}

void EnInvadepoh_InvasionHandler_SetupIntroCutscene(EnInvadepoh* this) {
    sInvasionState = INVASION_STATE_ACTIVE;
    this->timer = 160;
    this->actionFunc = EnInvadepoh_InvasionHandler_IntroCutscene;
}

/**
 * Activates the eight aliens at various points in the cutscene. After 160 frames pass, this function stops the cutscene
 * and starts handling the invasion proper.
 */
void EnInvadepoh_InvasionHandler_IntroCutscene(EnInvadepoh* this, PlayState* play) {
    static s16 sAlienSpawnTimes[ALIEN_COUNT] = {
        130, 125, 115, 100, 80, 78, 76, 74,
    };
    s32 i;

    for (i = 0; i < ALIEN_COUNT; i++) {
        if (this->timer == sAlienSpawnTimes[i]) {
            sAlienStateFlags[i] |= ALIEN_STATE_FLAG_ACTIVE;
        }
    }

    this->timer--;
    if (this->timer <= 0) {
        CutsceneManager_Stop(sInvadepohCsIdList[0]);
        SEQCMD_PLAY_SEQUENCE(SEQ_PLAYER_BGM_MAIN, 0, NA_BGM_ALIEN_INVASION | SEQ_FLAG_ASYNC);
        EnInvadepoh_InvasionHandler_SetupHandle(this);
    }
}

void EnInvadepoh_InvasionHandler_SetupHandle(EnInvadepoh* this) {
    sInvasionState = INVASION_STATE_ACTIVE;
    this->actionFunc = EnInvadepoh_InvasionHandler_Handle;
}

/**
 * If it's 5:15 AM or later, this function will start the success cutscene. Otherwise, this function decides which alien
 * (if any) is the most threatening and checks to see if any alien has reached the end of its path (i.e., it reached the
 * barn). If this happens, then this function will start the failure cutscene.
 */
void EnInvadepoh_InvasionHandler_Handle(EnInvadepoh* this, PlayState* play) {
    if ((CURRENT_TIME < CLOCK_TIME(6, 00)) && (CURRENT_TIME >= CLOCK_TIME(5, 15))) {
        SET_WEEKEVENTREG(WEEKEVENTREG_DEFENDED_AGAINST_ALIENS);
        EnInvadepoh_InvasionHandler_SetupStartSuccessCutscene(this);
    } else {
        s32 i;

        EnInvadepoh_InvasionHandler_SetClosestAlienThreat(this);

        for (i = 0; i < this->alienCount; i++) {
            if ((sAliens[i] != NULL) && (sAliens[i]->pathCompleted)) {
                EnInvadepoh_InvasionHandler_SetupStartFailureCutscene(this);
                break;
            }
        }
    }
}

void EnInvadepoh_InvasionHandler_SetupStartSuccessCutscene(EnInvadepoh* this) {
    sInvasionState = INVASION_STATE_SUCCESS;
    this->actionFunc = EnInvadepoh_InvasionHandler_StartSuccessCutscene;
}

/**
 * Immediately cues up the success cutscene and starts it.
 */
void EnInvadepoh_InvasionHandler_StartSuccessCutscene(EnInvadepoh* this, PlayState* play) {
    if (CutsceneManager_IsNext(sInvadepohCsIdList[1])) {
        CutsceneManager_StartWithPlayerCs(sInvadepohCsIdList[1], &this->actor);
        EnInvadepoh_InvasionHandler_SetupSuccessCutscene(this);
    } else {
        CutsceneManager_Queue(sInvadepohCsIdList[1]);
    }
}

void EnInvadepoh_InvasionHandler_SetupSuccessCutscene(EnInvadepoh* this) {
    sInvasionState = INVASION_STATE_SUCCESS;
    this->timer = 110;
    this->actionFunc = EnInvadepoh_InvasionHandler_SuccessCutscene;
}

/**
 * This function is responsible for playing the fanfare for defending the ranch. After a total of 110 frames pass, this
 * function transitions the player to a special layer of Romani Ranch where Romani gives the player the Bottle of Milk.
 */
void EnInvadepoh_InvasionHandler_SuccessCutscene(EnInvadepoh* this, PlayState* play) {
    if (this->timer == 100) {
        Audio_PlayFanfare(NA_BGM_CLEAR_EVENT);
    }

    this->timer--;
    if (this->timer <= 0) {
        play->nextEntrance = ENTRANCE(ROMANI_RANCH, 6);
        gSaveContext.nextCutsceneIndex = 0;
        play->transitionTrigger = TRANS_TRIGGER_START;
        play->transitionType = TRANS_TYPE_73;
        gSaveContext.nextTransitionType = TRANS_TYPE_72;
        D_801BDAA0 = true;
        gHorseIsMounted = false;
        EnInvadepoh_InvasionHandler_SetupSuccessEnd(this);
    }
}

void EnInvadepoh_InvasionHandler_SetupWaitForRomaniReward(EnInvadepoh* this) {
    sInvasionState = INVASION_STATE_SUCCESS;
    this->actionFunc = EnInvadepoh_InvasionHandler_WaitForRomaniReward;
}

/**
 * Waits until Romani is done giving her reward, then transitions the player back to the normal version of Romani Ranch.
 */
void EnInvadepoh_InvasionHandler_WaitForRomaniReward(EnInvadepoh* this, PlayState* play) {
    if (sRewardFinished) {
        play->nextEntrance = ENTRANCE(ROMANI_RANCH, 7);
        gSaveContext.nextCutsceneIndex = 0;
        play->transitionTrigger = TRANS_TRIGGER_START;
        play->transitionType = TRANS_TYPE_72;
        gSaveContext.nextTransitionType = TRANS_TYPE_72;
        EnInvadepoh_InvasionHandler_SetupSuccessEnd(this);
    }
}

void EnInvadepoh_InvasionHandler_SetupSuccessEnd(EnInvadepoh* this) {
    sInvasionState = INVASION_STATE_SUCCESS;
    this->actionFunc = EnInvadepoh_InvasionHandler_SuccessEnd;
}

/**
 * After the player successfully defended the ranch, the invasion handler will remain in this state until it is killed.
 */
void EnInvadepoh_InvasionHandler_SuccessEnd(EnInvadepoh* this, PlayState* play) {
}

void EnInvadepoh_InvasionHandler_SetupStartFailureCutscene(EnInvadepoh* this) {
    sInvasionState = INVASION_STATE_FAILURE;
    this->actionFunc = EnInvadepoh_InvasionHandler_StartFailureCutscene;
}

/**
 * Starts the failure cutscene where the aliens abduct the cows and Romani. Unlike other cutscenes, this cutscene is
 * triggered by transitioning the player to Romani Ranch with a specific `nextCutsceneIndex`. As a consequence, the game
 * will display a transition animation before the cutscene starts.
 */
void EnInvadepoh_InvasionHandler_StartFailureCutscene(EnInvadepoh* this, PlayState* play) {
    play->nextEntrance = ENTRANCE(ROMANI_RANCH, 0);
    gSaveContext.nextCutsceneIndex = 0xFFF3;
    play->transitionTrigger = TRANS_TRIGGER_START;
    play->transitionType = TRANS_TYPE_72;
    gSaveContext.nextTransitionType = TRANS_TYPE_72;
    SET_WEEKEVENTREG(WEEKEVENTREG_FAILED_TO_DEFEND_AGAINST_ALIENS);
    EnInvadepoh_InvasionHandler_SetupFailureEnd(this);
}

void EnInvadepoh_InvasionHandler_SetupFailureEnd(EnInvadepoh* this) {
    sInvasionState = INVASION_STATE_FAILURE;
    this->actionFunc = EnInvadepoh_InvasionHandler_FailureEnd;
}

/**
 * After the player failed to defend the ranch, the invasion handler will remain in this state until it is killed.
 */
void EnInvadepoh_InvasionHandler_FailureEnd(EnInvadepoh* this, PlayState* play) {
}

void EnInvadepoh_InvasionHandler_Update(Actor* thisx, PlayState* play2) {
    PlayState* play = play2;
    EnInvadepoh* this = (EnInvadepoh*)thisx;

    this->actionFunc(this, play);

    this->actor.draw = EnInvadepoh_UpdateEffects() ? EnInvadepoh_InvasionHandler_Draw : NULL;
}

void EnInvadepoh_Alien_SetupWaitForInvasion(EnInvadepoh* this) {
    this->collider.base.atFlags &= ~AT_ON;
    this->collider.base.acFlags &= ~AC_ON;
    this->collider.base.ocFlags1 &= ~OC1_ON;
    this->actor.flags &= ~ACTOR_FLAG_MINIMAP_ICON_ENABLED;
    this->alpha = 0;
    this->actor.draw = NULL;
    this->shouldDraw = false;
    this->shouldDrawDeathFlash = false;
    this->eyeBeamAlpha = 0;
    this->actionFunc = EnInvadepoh_Alien_WaitForInvasion;
}

/**
 * Waits until the invasion handler has signaled that the invasion has started, then the alien will spawn a warp effect
 * and warp in.
 */
void EnInvadepoh_Alien_WaitForInvasion(EnInvadepoh* this, PlayState* play) {
    EnInvadepoh_Alien_PathComputeProgress(this);
    EnInvadepoh_Alien_PathUpdate(this, play);
    EnInvadepoh_Alien_StepYawAlongPath(this, 0x320, 0);

    if (sAlienStateFlags[EN_INVADEPOH_GET_INDEX(&this->actor)] & ALIEN_STATE_FLAG_ACTIVE) {
        Actor_SetScale(&this->actor, 0.01f);
        EnInvadepoh_SnapToFloor(this);
        EnInvadepoh_SpawnWarpEffect(&this->actor.world.pos);
        EnInvadepoh_Alien_SetupWarpIn(this);
    }
}

void EnInvadepoh_Alien_SetupWaitToRespawn(EnInvadepoh* this) {
    this->collider.base.atFlags &= ~AT_ON;
    this->collider.base.acFlags &= ~AC_ON;
    this->collider.base.ocFlags1 &= ~OC1_ON;
    this->actor.flags &= ~ACTOR_FLAG_MINIMAP_ICON_ENABLED;
    this->alpha = 0;
    this->actor.draw = NULL;
    this->shouldDraw = false;
    this->shouldDrawDeathFlash = false;
    this->eyeBeamAlpha = 0;
    this->actionFunc = EnInvadepoh_Alien_WaitToRespawn;
}

/**
 * Waits until the current time is later than the alien's respawn time, then makes the alien warp in.
 */
void EnInvadepoh_Alien_WaitToRespawn(EnInvadepoh* this, PlayState* play) {
    EnInvadepoh_Alien_PathComputeProgress(this);
    EnInvadepoh_Alien_PathUpdate(this, play);
    EnInvadepoh_Alien_StepYawAlongPath(this, 0x320, 0);

    if (this->pathProgress > 0.0f) {
        Actor_SetScale(&this->actor, 0.01f);
        EnInvadepoh_SnapToFloor(this);
        EnInvadepoh_SpawnWarpEffect(&this->actor.world.pos);
        EnInvadepoh_Alien_SetupWarpIn(this);
    }
}

void EnInvadepoh_Alien_SetupWarpIn(EnInvadepoh* this) {
    Animation_MorphToLoop(&this->skelAnime, &gAlienFloatAnim, -6.0f);
    this->collider.base.atFlags &= ~AT_ON;
    this->collider.base.acFlags &= ~AC_ON;
    this->collider.base.ocFlags1 &= ~OC1_ON;
    this->alpha = 0;
    this->actor.draw = EnInvadepoh_Alien_Draw;
    this->shouldDraw = true;
    this->shouldDrawDeathFlash = false;
    this->eyeBeamAlpha = 0;
    this->actor.flags |= ACTOR_FLAG_MINIMAP_ICON_ENABLED;
    this->actionFunc = EnInvadepoh_Alien_WarpIn;
}

/**
 * Slowly fade in the alien and its eye beams as it floats forward along its path. When the alien is halfway done with
 * fading in, its collider becomes enabled, allowing the player to hit it and allowing it to hit the player.
 */
void EnInvadepoh_Alien_WarpIn(EnInvadepoh* this, PlayState* play) {
    EnInvadepoh_Alien_PathComputeProgress(this);
    EnInvadepoh_Alien_PathUpdate(this, play);
    EnInvadepoh_Alien_StepYawAlongPath(this, 0x320, 0);
    Actor_PlaySfx_Flagged(&this->actor, NA_SE_EN_FOLLOWERS_BEAM_PRE - SFX_FLAG);

    if (this->pathProgress >= 0.9999f) {
        this->pathCompleted = true;
    }

    if (this->alpha >= (255 - 6)) {
        this->alpha = 255;
    } else {
        this->alpha += 6;
    }
    if (this->alpha > 128) {
        this->collider.base.atFlags |= AT_ON;
        this->collider.base.acFlags |= AC_ON;
        this->collider.base.ocFlags1 |= OC1_ON;
    }

    if (this->alpha == 255) {
        if (this->eyeBeamAlpha >= (255 - 10)) {
            this->eyeBeamAlpha = 255;
            EnInvadepoh_Alien_SetupFloatForward(this);
        } else {
            this->eyeBeamAlpha += 10;
        }
    }
}

void EnInvadepoh_Alien_SetupFloatForward(EnInvadepoh* this) {
    if (this->skelAnime.animation != &gAlienFloatAnim) {
        Animation_MorphToLoop(&this->skelAnime, &gAlienFloatAnim, -6.0f);
    }

    this->collider.base.atFlags |= AT_ON;
    this->collider.base.acFlags |= AC_ON;
    this->collider.base.ocFlags1 |= OC1_ON;
    this->alpha = 255;
    this->actor.draw = EnInvadepoh_Alien_Draw;
    this->shouldDraw = true;
    this->shouldDrawDeathFlash = false;
    this->eyeBeamAlpha = 255;
    this->actor.flags |= ACTOR_FLAG_MINIMAP_ICON_ENABLED;
    this->actionFunc = EnInvadepoh_Alien_FloatForward;
}

/**
 * Slowly moves the alien forward along its path while playing its floating animation.
 */
void EnInvadepoh_Alien_FloatForward(EnInvadepoh* this, PlayState* play) {
    EnInvadepoh_Alien_PathComputeProgress(this);
    EnInvadepoh_Alien_PathUpdate(this, play);
    EnInvadepoh_Alien_StepYawAlongPath(this, 0x320, 0);
    Actor_PlaySfx_Flagged(&this->actor, NA_SE_EN_FOLLOWERS_BEAM_PRE - SFX_FLAG);

    if (this->pathProgress >= 0.9999f) {
        this->pathCompleted = true;
    }
}

void EnInvadepoh_Alien_SetupDamaged(EnInvadepoh* this) {
    this->collider.base.atFlags &= ~AT_ON;
    this->collider.base.acFlags &= ~AC_ON;
    this->collider.base.ocFlags1 |= OC1_ON;
    Animation_PlayLoop(&this->skelAnime, &gAlienJerkingAnim);
    Actor_SetColorFilter(&this->actor, COLORFILTER_COLORFLAG_RED, 255, COLORFILTER_BUFFLAG_OPA, 16);
    this->alpha = 255;
    this->actor.draw = EnInvadepoh_Alien_Draw;
    this->shouldDraw = true;
    this->shouldDrawDeathFlash = false;
    this->eyeBeamAlpha = 0;
    this->timer = 8;
    this->frameCounter = 0;
    this->actor.flags |= ACTOR_FLAG_MINIMAP_ICON_ENABLED;
    this->actionFunc = EnInvadepoh_Alien_Damaged;
}

/**
 * Knocks the alien backwards for 8 frames, then starts the alien's death sequence.
 */
void EnInvadepoh_Alien_Damaged(EnInvadepoh* this, PlayState* play) {
    EnInvadepoh_Alien_Knockback(this, play);

    this->timer--;
    if (this->timer <= 0) {
        EnInvadepoh_Alien_SetupDead(this);
    }
}

void EnInvadepoh_Alien_SetupDead(EnInvadepoh* this) {
    this->collider.base.atFlags &= ~AT_ON;
    this->collider.base.acFlags &= ~AC_ON;
    this->collider.base.ocFlags1 &= ~OC1_ON;
    Animation_PlayLoop(&this->skelAnime, &gAlienDeathAnim);
    this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
    this->timer = 10;
    this->alpha = 255;
    this->actor.draw = EnInvadepoh_Alien_Draw;
    this->shouldDraw = true;
    this->shouldDrawDeathFlash = false;
    this->eyeBeamAlpha = 255;
    this->actor.flags |= ACTOR_FLAG_MINIMAP_ICON_ENABLED;
    this->actionFunc = EnInvadepoh_Alien_Dead;
}

#define DEATH_SCALE 0.000001f

/**
 * Plays the alien's death animation for 10 frames, rapidly changing the scale of both the alien and a bright flash of
 * light. Afterwards, the alien disappears into a puff of smoke, drops 30 arrows, and then begins waiting to respawn.
 */
void EnInvadepoh_Alien_Dead(EnInvadepoh* this, PlayState* play) {
    static Vec3f sDeathScales[] = {
        { 10000 * DEATH_SCALE, 10000 * DEATH_SCALE, 10000 * DEATH_SCALE },
        { 20000 * DEATH_SCALE, 10000 * DEATH_SCALE, 5000 * DEATH_SCALE },
        { -10000 * DEATH_SCALE, 0 * DEATH_SCALE, 0 * DEATH_SCALE },
        { 10000 * DEATH_SCALE, 10000 * DEATH_SCALE, 10000 * DEATH_SCALE },
        { 5000 * DEATH_SCALE, 10000 * DEATH_SCALE, 20000 * DEATH_SCALE },
    };
    static Vec3f sDeathFlashScales[] = {
        { 500 * DEATH_SCALE, 28000 * DEATH_SCALE, 10000 * DEATH_SCALE },
        { -10000 * DEATH_SCALE, 0 * DEATH_SCALE, 0 * DEATH_SCALE },
        { -10000 * DEATH_SCALE, 0 * DEATH_SCALE, 0 * DEATH_SCALE },
        { 16000 * DEATH_SCALE, 400 * DEATH_SCALE, 10000 * DEATH_SCALE },
        { -10000 * DEATH_SCALE, 0 * DEATH_SCALE, 0 * DEATH_SCALE },
        { 500 * DEATH_SCALE, 500 * DEATH_SCALE, 500 * DEATH_SCALE },
        { 200 * DEATH_SCALE, 200 * DEATH_SCALE, 200 * DEATH_SCALE },
    };
    Vec3f* scale;

    EnInvadepoh_Alien_DoNothing(this);

    if (this->frameCounter < 5) {
        scale = &sDeathScales[this->frameCounter];

        if (scale->x > 0.0f) {
            this->shouldDraw = true;
            this->eyeBeamAlpha = 255;
            Math_Vec3f_Copy(&this->actor.scale, scale);
        } else {
            this->shouldDraw = false;
            this->eyeBeamAlpha = 0;
        }
    } else {
        this->shouldDraw = false;
        this->eyeBeamAlpha = 0;
    }

    if ((this->frameCounter >= 2) && (this->frameCounter < 9)) {
        scale = &sDeathFlashScales[this->frameCounter - 2];

        if (scale->x > 0.0f) {
            this->shouldDrawDeathFlash = true;
            Math_Vec3f_Copy(&this->deathFlashScale, scale);
        } else {
            this->shouldDrawDeathFlash = false;
        }
    } else {
        this->shouldDrawDeathFlash = false;
    }

    this->frameCounter++;

    if (this->timer == 8) {
        Actor_Spawn(&play->actorCtx, play, ACTOR_EN_CLEAR_TAG, this->actor.world.pos.x, this->actor.world.pos.y - 10.0f,
                    this->actor.world.pos.z, 0, 0, 3, CLEAR_TAG_PARAMS(CLEAR_TAG_SMOKE));
    }

    if (this->timer == 8) {
        Enemy_StartFinishingBlow(play, &this->actor);
    }

    this->timer--;
    if (this->timer <= 0) {
        EnInvadepoh_Alien_SetRespawnTime(EN_INVADEPOH_GET_INDEX(&this->actor));
        EnInvadepoh_Alien_AddKill();
        Item_DropCollectible(play, &this->actor.world.pos, ITEM00_ARROWS_30);
        EnInvadepoh_Alien_SetupWaitToRespawn(this);
    }
}

void EnInvadepoh_Alien_WaitForObject(Actor* thisx, PlayState* play2) {
    PlayState* play = play2;
    EnInvadepoh* this = (EnInvadepoh*)thisx;

    if (Object_IsLoaded(&play->objectCtx, this->objectSlot)) {
        this->actor.objectSlot = this->objectSlot;
        Actor_SetObjectDependency(play, &this->actor);
        EnInvadepoh_Alien_DesegmentTexAnims();
        this->actor.update = EnInvadepoh_Alien_Update;
        SkelAnime_InitFlex(play, &this->skelAnime, &gAlienSkel, &gAlienFloatAnim, this->jointTable, this->morphTable,
                           ALIEN_LIMB_MAX);
        this->skelAnime.curFrame = EN_INVADEPOH_GET_INDEX(&this->actor) * this->skelAnime.endFrame / 8.0f;
        EnInvadepoh_Alien_InitPath(this, play);
        EnInvadepoh_Alien_PathComputeProgress(this);
        EnInvadepoh_Alien_PathUpdate(this, play);
        EnInvadepoh_SetYawAlongPath(this);
        EnInvadepoh_SnapToFloor(this);

        if ((sInvasionState == INVASION_STATE_WAIT) || (CURRENT_TIME < CLOCK_TIME(2, 31))) {
            EnInvadepoh_Alien_SetupWaitForInvasion(this);
        } else if (sInvasionState == INVASION_STATE_ACTIVE) {
            if (this->pathProgress >= 0.0001f) {
                EnInvadepoh_Alien_SetupFloatForward(this);
            } else {
                EnInvadepoh_Alien_SetupWarpIn(this);
            }
        } else {
            Actor_Kill(&this->actor);
        }
    }
}

void EnInvadepoh_Alien_Update(Actor* thisx, PlayState* play2) {
    PlayState* play = play2;
    EnInvadepoh* this = (EnInvadepoh*)thisx;

    if (sInvasionState == INVASION_STATE_SUCCESS) {
        // The player successfully defended the ranch from the aliens, so this alien should either play its death
        // animation if it's currently alive or have its instance silently killed if it's waiting to respawn.
        if ((this->actionFunc == EnInvadepoh_Alien_FloatForward) || (this->actionFunc == EnInvadepoh_Alien_WarpIn)) {
            thisx->speed = 0.0f;
            thisx->velocity.y = 0.0f;
            thisx->gravity = 0.0f;
            EnInvadepoh_Alien_SetupDamaged(this);
        } else if ((this->actionFunc == EnInvadepoh_Alien_WaitToRespawn) ||
                   (this->actionFunc == EnInvadepoh_Alien_WaitForInvasion)) {
            Actor_Kill(thisx);
            return;
        }
    } else if (this->collider.base.acFlags & AC_HIT) {
        Actor* ac = this->collider.base.ac;

        thisx->speed = ac->speed * 0.5f;
        thisx->speed = CLAMP(thisx->speed, -60.0f, 60.0f);

        thisx->world.rot.y = ac->world.rot.y;
        thisx->gravity = 0.0f;

        thisx->velocity.y = ac->velocity.y * 0.5f;
        thisx->velocity.y = CLAMP(thisx->velocity.y, -30.0f, 30.0f);

        SoundSource_PlaySfxAtFixedWorldPos(play, &thisx->world.pos, 50, NA_SE_EN_INVADER_DEAD);
        EnInvadepoh_Alien_SetupDamaged(this);
    }

    this->actionFunc(this, play);

    if (thisx->draw != NULL) {
        this->isAnimFinished = SkelAnime_Update(&this->skelAnime);
    }

    Collider_UpdateCylinder(thisx, &this->collider);

    if (this->collider.base.atFlags & AT_ON) {
        CollisionCheck_SetAT(play, &play->colChkCtx, &this->collider.base);
    } else {
        this->collider.base.atFlags &= ~AT_HIT;
    }
    if (this->collider.base.acFlags & AC_ON) {
        CollisionCheck_SetAC(play, &play->colChkCtx, &this->collider.base);
    } else {
        this->collider.base.acFlags &= ~AC_HIT;
    }
    if (this->collider.base.ocFlags1 & OC1_ON) {
        CollisionCheck_SetOC(play, &play->colChkCtx, &this->collider.base);
    } else {
        this->collider.base.ocFlags1 &= ~OC1_HIT;
    }
}

void EnInvadepoh_Cow_WaitForObject(Actor* thisx, PlayState* play2) {
    PlayState* play = play2;
    EnInvadepoh* this = (EnInvadepoh*)thisx;

    if (Object_IsLoaded(&play->objectCtx, this->objectSlot)) {
        this->actor.objectSlot = this->objectSlot;
        Actor_SetObjectDependency(play, &this->actor);
        this->actor.update = EnInvadepoh_Cow_Update;
        this->actor.draw = EnInvadepoh_Cow_Draw;
        SkelAnime_InitFlex(play, &this->skelAnime, &gCowSkel, NULL, this->jointTable, this->morphTable, COW_LIMB_MAX);
        Animation_PlayLoop(&this->skelAnime, &gCowMooAnim);
    }
}

void EnInvadepoh_Cow_Update(Actor* thisx, PlayState* play2) {
    static s16 sCowAngularVelocity[EN_INVADEPOH_COW_INDEX_MAX] = {
        -0x1F40, // EN_INVADEPOH_COW_INDEX_0
        -0x1770, // EN_INVADEPOH_COW_INDEX_1
        -0x2AF8  // EN_INVADEPOH_COW_INDEX_2
    };
    PlayState* play = play2;
    EnInvadepoh* this = (EnInvadepoh*)thisx;
    s32 index;

    if ((sUfo == NULL) || (this->actor.parent == NULL)) {
        Actor_Kill(&this->actor);
        return;
    }

    index = EN_INVADEPOH_GET_INDEX(&this->actor);
    SkelAnime_Update(&this->skelAnime);
    Math_ScaledStepToS(&this->actor.shape.rot.x, sCowAngularVelocity[index], 0x32);

    if (this->actor.child != NULL) {
        s32 pad;
        MtxF tailMtxF;

        Matrix_Push();
        Matrix_SetTranslateRotateYXZ(this->actor.world.pos.x, this->actor.world.pos.y, this->actor.world.pos.z,
                                     &this->actor.shape.rot);
        Matrix_Translate(0.0f, 57.0f, -36.0f, MTXMODE_APPLY);
        Matrix_RotateXS(this->actor.shape.rot.x * -0.7f, MTXMODE_APPLY);
        Matrix_RotateZS(this->actor.shape.rot.z * -0.7f, MTXMODE_APPLY);
        Matrix_MultZero(&this->actor.child->world.pos);
        Matrix_Get(&tailMtxF);
        Matrix_MtxFToYXZRot(&tailMtxF, &this->actor.child->shape.rot, false);
        Matrix_Pop();
    }
}

void EnInvadepoh_CowTail_WaitForObject(Actor* thisx, PlayState* play2) {
    PlayState* play = play2;
    EnInvadepoh* this = (EnInvadepoh*)thisx;

    if (Object_IsLoaded(&play->objectCtx, this->objectSlot)) {
        this->actor.objectSlot = this->objectSlot;
        Actor_SetObjectDependency(play, &this->actor);
        this->actor.update = EnInvadepoh_CowTail_Update;
        this->actor.draw = EnInvadepoh_CowTail_Draw;
        SkelAnime_InitFlex(play, &this->skelAnime, &gCowTailSkel, NULL, this->jointTable, this->morphTable,
                           COW_TAIL_LIMB_MAX);
        Animation_PlayLoop(&this->skelAnime, &gCowTailSwishAnim);
    }
}

void EnInvadepoh_CowTail_Update(Actor* thisx, PlayState* play2) {
    PlayState* play = play2;
    EnInvadepoh* this = (EnInvadepoh*)thisx;

    if ((sUfo == NULL) || (this->actor.parent == NULL)) {
        Actor_Kill(&this->actor);
    } else {
        SkelAnime_Update(&this->skelAnime);
    }
}

void EnInvadepoh_AbductedRomani_SetupWait(EnInvadepoh* this) {
    this->timer = 40;
    Animation_MorphToLoop(&this->skelAnime, &gRomaniLookAroundAnim, -10.0f);
    this->actor.draw = NULL;
    this->actionFunc = EnInvadepoh_AbductedRomani_Wait;
}

/**
 * Waits for 40 frames, then Romani becomes visible, looks around, and starts yelling.
 */
void EnInvadepoh_AbductedRomani_Wait(EnInvadepoh* this, PlayState* play) {
    this->timer--;

    if (this->timer <= 0) {
        // Romani's scream when abducted by the aliens
        EnInvadepoh_Romani_StartTextbox(this, play, 0x332F);
        this->actor.draw = EnInvadepoh_Romani_Draw;
        EnInvadepoh_AbductedRomani_SetupYell(this);
    }
}

void EnInvadepoh_AbductedRomani_SetupYell(EnInvadepoh* this) {
    this->timer = 60;
    Animation_MorphToLoop(&this->skelAnime, &gRomaniLookAroundAnim, -10.0f);
    this->actor.draw = EnInvadepoh_Romani_Draw;
    this->actionFunc = EnInvadepoh_AbductedRomani_Yell;
}

/**
 * Looks around for 60 frames, then makes Romani start struggling.
 */
void EnInvadepoh_AbductedRomani_Yell(EnInvadepoh* this, PlayState* play) {
    if (this->timer == 20) {
        EnInvadepohModelInfo* modelInfo = &this->modelInfo;

        modelInfo->headRotTarget.x = 0x7D0;
        modelInfo->headRotTarget.y = 0;
        modelInfo->headRotTarget.z = 0;
        modelInfo->headRotStepScale = 0.1f;
        modelInfo->headRotMaxStep = 0x3E8;
    }

    this->timer--;

    if (this->timer <= 0) {
        EnInvadepoh_AbductedRomani_SetupStruggle(this);
    }
}

void EnInvadepoh_AbductedRomani_SetupStruggle(EnInvadepoh* this) {
    this->modelInfo.headRotTarget.x = 0xBB8;
    this->modelInfo.headRotTarget.y = 0;
    this->modelInfo.headRotTarget.z = 0;
    this->modelInfo.headRotStepScale = 0.24f;
    this->modelInfo.headRotMaxStep = 0xBB8;
    this->modelInfo.torsoTargetRotX = 0x3A98;
    this->modelInfo.torsoRotStepScale = 0.1f;
    this->modelInfo.torsoRotMaxStep = 0x7D0;
    this->timer = 50;
    Animation_Change(&this->skelAnime, &gRomaniRunAnim, 2.0f, 0.0f, 0.0f, ANIMMODE_LOOP, -5.0f);
    this->actor.draw = EnInvadepoh_Romani_Draw;
    this->actionFunc = EnInvadepoh_AbductedRomani_Struggle;
}

/**
 * Makes Romani struggle for 50 frames by running in place while looking side-to-side. Once the timer reaches zero,
 * Romani will become idle and disappear.
 */
void EnInvadepoh_AbductedRomani_Struggle(EnInvadepoh* this, PlayState* play) {
    if (this->timer == 40) {
        this->modelInfo.headRotTarget.y = 0x1B58;
    } else if (this->timer == 30) {
        this->modelInfo.headRotTarget.y = -0x1B58;
    } else if (this->timer == 20) {
        this->modelInfo.headRotTarget.y = 0x1B58;
    } else if (this->timer == 10) {
        this->modelInfo.headRotTarget.y = 0;
    }

    this->timer--;

    if (this->timer <= 0) {
        EnInvadepoh_AbductedRomani_SetupEnd(this);
    }
}

void EnInvadepoh_AbductedRomani_SetupEnd(EnInvadepoh* this) {
    this->modelInfo.headRotTarget.x = 0;
    this->modelInfo.headRotTarget.y = 0;
    this->modelInfo.headRotTarget.z = 0;
    this->modelInfo.headRotStepScale = 0.24f;
    this->modelInfo.headRotMaxStep = 0x1388;
    this->modelInfo.torsoTargetRotX = 0;
    this->modelInfo.torsoRotStepScale = 0.28f;
    this->modelInfo.torsoRotMaxStep = 0x1B58;
    Animation_MorphToPlayOnce(&this->skelAnime, &gRomaniIdleAnim, -10.0f);
    this->actor.draw = EnInvadepoh_Romani_Draw;
    this->actionFunc = EnInvadepoh_AbductedRomani_End;
}

/**
 * Plays Romani's idle animation once. Once the alien holding Romani is killed, `EnInvadepoh_AbductedRomani_Update` will
 * kill this instance of Romani, thus preventing the player from seeing that the idle animation doesn't loop.
 */
void EnInvadepoh_AbductedRomani_End(EnInvadepoh* this, PlayState* play) {
}

void EnInvadepoh_AbductedRomani_WaitForObject(Actor* thisx, PlayState* play2) {
    PlayState* play = play2;
    EnInvadepoh* this = (EnInvadepoh*)thisx;
    s32 pad;

    if (Object_IsLoaded(&play->objectCtx, this->objectSlot)) {
        this->actor.objectSlot = this->objectSlot;
        Actor_SetObjectDependency(play, &this->actor);
        EnInvadepoh_Romani_DesegmentTextures();
        this->actor.update = EnInvadepoh_AbductedRomani_Update;
        SkelAnime_InitFlex(play, &this->skelAnime, &gRomaniSkel, &gRomaniIdleAnim, this->jointTable, this->morphTable,
                           ROMANI_LIMB_MAX);
        EnInvadepoh_ModelInfo_Init(&this->modelInfo, sRomaniEyeAnimations, ROMANI_EYE_ANIM_HALF, sRomaniMouthAnimations,
                                   ROMANI_MOUTH_ANIM_HANGING_OPEN, &gZeroVec3s, 0x1388, 0.05f, 0.3f, 0.12f);
        Animation_PlayLoop(&this->skelAnime, &gRomaniIdleAnim);
        EnInvadepoh_AbductedRomani_SetupWait(this);
    }
}

void EnInvadepoh_AbductedRomani_Update(Actor* thisx, PlayState* play2) {
    PlayState* play = play2;
    EnInvadepoh* this = (EnInvadepoh*)thisx;
    EnInvadepohModelInfo* modelInfo = &this->modelInfo;

    if (this->actor.parent == NULL) {
        Actor_Kill(&this->actor);
        return;
    }

    this->actionFunc(this, play);

    SkelAnime_Update(&this->skelAnime);
    EnInvadepoh_ModelInfo_Update(&this->modelInfo);

    if (modelInfo->torsoRotX != 0) {
        this->actor.shape.rot.x = -modelInfo->torsoRotX;
    }
}

void EnInvadepoh_SilentRomani_SetupWalk(EnInvadepoh* this) {
    static s16 sHeadRotTargetX[] = { -0x708, -0x3E8, 0, 0x7D0 };
    EnInvadepohModelInfo* modelInfo = &this->modelInfo;

    this->timer = Rand_S16Offset(150, 250);
    modelInfo->headRotTarget.x = sHeadRotTargetX[Rand_Next() >> 0x1E];
    modelInfo->headRotTarget.y = 0;
    modelInfo->headRotTarget.z = 0;
    modelInfo->headRotStepScale = 0.1f;
    modelInfo->headRotMaxStep = 0x320;
    Animation_MorphToLoop(&this->skelAnime, &gRomaniWalkAnim, -10.0f);
    this->actionFunc = EnInvadepoh_SilentRomani_Walk;
}

/**
 * Makes Romani walk along her path for a random amount of time between 150 and 400 frames. Once the timer reaches zero,
 * Romani will stop walking and wait.
 */
void EnInvadepoh_SilentRomani_Walk(EnInvadepoh* this, PlayState* play) {
    s32 pad;

    Math_StepToF(&this->actor.speed, 1.6f, 0.1f);

    if (EnInvadepoh_Romani_MoveAlongPath(this, play, this->actor.speed, 50.0f)) {
        EnInvadepoh_SilentRomani_SetPathPointToNext(this);
        this->shapeAngularVelocityY = 0xC8;
        this->actor.speed *= 0.25f;
    } else {
        Math_StepToS(&this->shapeAngularVelocityY, 0x7D0, 0x46);
    }

    EnInvadepoh_Romani_StepYawAlongPath(this, 6, this->shapeAngularVelocityY, 0x46);

    if (CHECK_FLAG_ALL(this->actor.flags, ACTOR_FLAG_INSIDE_CULLING_VOLUME) &&
        (Animation_OnFrame(&this->skelAnime, 0.0f) || Animation_OnFrame(&this->skelAnime, 7.0f))) {
        Actor_PlaySfx(&this->actor, NA_SE_EN_ROMANI_WALK);
    }

    if (this->timer > 0) {
        this->timer--;
    } else {
        EnInvadepoh_SilentRomani_SetupIdle(this);
    }
}

void EnInvadepoh_SilentRomani_SetupIdle(EnInvadepoh* this) {
    EnInvadepohModelInfo* modelInfo = &this->modelInfo;
    f32 rand = Rand_ZeroOne();

    this->timer = Rand_S16Offset(150, 150);

    if (rand < 0.5f) {
        this->silentRomaniStareState = SILENT_ROMANI_STARE_STATE_STARE_AT_NEARBY_PLAYER;
        Math_Vec3s_Copy(&modelInfo->headRotTarget, &gZeroVec3s);
        modelInfo->headRotStepScale = 0.1f;
        modelInfo->headRotMaxStep = 0x3E8;
    } else if (rand < 0.75f) {
        this->silentRomaniStareState = SILENT_ROMANI_STARE_STATE_STARE_RANDOM_FAST;
        modelInfo->headRotTarget.x = Rand_S16Offset(0, 0x7D0);
        modelInfo->headRotTarget.y = 0;
        modelInfo->headRotTarget.z = 0;
        modelInfo->headRotStepScale = 0.06f;
        modelInfo->headRotMaxStep = 0x3E8;
    } else if (rand < 0.8f) {
        this->silentRomaniStareState = SILENT_ROMANI_STARE_STATE_STARE_RANDOM_MEDIUM;
        modelInfo->headRotTarget.x = Rand_S16Offset(-0x7D0, 0x7D0);
        modelInfo->headRotTarget.y = 0;
        modelInfo->headRotTarget.z = 0;
        modelInfo->headRotStepScale = 0.05f;
        modelInfo->headRotMaxStep = 0x3E8;
    } else {
        this->silentRomaniStareState = SILENT_ROMANI_STARE_STATE_STARE_RANDOM_SLOW;
        modelInfo->headRotTarget.x = 0;
        modelInfo->headRotTarget.y = 0;
        modelInfo->headRotTarget.z = Rand_S16Offset(-0x9C4, 0x1388);
        modelInfo->headRotStepScale = 0.04f;
        modelInfo->headRotMaxStep = 0x3E8;
    }

    Animation_MorphToLoop(&this->skelAnime, &gRomaniIdleAnim, -10.0f);
    this->actionFunc = EnInvadepoh_SilentRomani_Idle;
}

/**
 * Waits for a random amount of time between 150 and 300 frames, during which Romani can either stare at the player if
 * they're nearby or stare off in a random direction. When the timer reaches 0, Romani will begin walking again.
 */
void EnInvadepoh_SilentRomani_Idle(EnInvadepoh* this, PlayState* play) {
    EnInvadepohModelInfo* modelInfo = &this->modelInfo;

    Math_StepToF(&this->actor.speed, 0.0f, 0.2f);

    if (EnInvadepoh_Romani_MoveAlongPath(this, play, this->actor.speed, 50.0f)) {
        EnInvadepoh_SilentRomani_SetPathPointToNext(this);
    }

    if (this->silentRomaniStareState == SILENT_ROMANI_STARE_STATE_STARE_AT_NEARBY_PLAYER) {
        if ((this->actor.xzDistToPlayer < 350.0f) && (play->gameplayFrames & 0x60)) {
            Player* player = GET_PLAYER(play);
            s16 pitch = (s16)(Math_Vec3f_Pitch(&this->actor.focus.pos, &player->actor.focus.pos) * 0.85f) -
                        this->actor.shape.rot.x;
            s16 yaw;

            modelInfo->headRotTarget.x = CLAMP(pitch, -0x9C4, 0x9C4);

            yaw = this->actor.yawTowardsPlayer - this->actor.shape.rot.y;
            modelInfo->headRotTarget.y = CLAMP((s16)(yaw * 0.7f), -0x1F40, 0x1F40);
        }
    } else {
        // For any state other than `SILENT_ROMANI_STARE_STATE_STARE_AT_NEARBY_PLAYER`, this will essentially clear the
        // target X and Y-rotation of her head, making the code that sets these target rotations somewhat pointless.
        modelInfo->headRotTarget.x = 0;
        modelInfo->headRotTarget.y = 0;
    }

    if (this->timer > 0) {
        s32 timerMod32 = (u32)this->timer % 32;

        if ((timerMod32 == 0) && (Rand_ZeroOne() < 0.3f)) {
            //! @bug: This calculation can result in negative values for `nextSilentRomaniStareState`, which doesn't
            //! seem like that's supposed to happen. When this happens, Romani will act the same as she does in
            //! `SILENT_ROMANI_STARE_STATE_STARE_RANDOM_SLOW`, which effectively makes this state far more likely than
            //! the developers probably intended.
            s32 nextSilentRomaniStareState = (s32)Rand_Next() % SILENT_ROMANI_STARE_STATE_MAX;

            if (nextSilentRomaniStareState != this->silentRomaniStareState) {
                this->silentRomaniStareState = nextSilentRomaniStareState;

                if (this->silentRomaniStareState == SILENT_ROMANI_STARE_STATE_STARE_AT_NEARBY_PLAYER) {
                    Math_Vec3s_Copy(&modelInfo->headRotTarget, &gZeroVec3s);
                    modelInfo->headRotStepScale = 0.07f;
                } else if (this->silentRomaniStareState == SILENT_ROMANI_STARE_STATE_STARE_RANDOM_FAST) {
                    modelInfo->headRotTarget.x = Rand_S16Offset(0x3E8, 0x3E8);
                    modelInfo->headRotTarget.y = Rand_S16Offset(-0x3E8, 0x7D0);
                    modelInfo->headRotTarget.z = Rand_S16Offset(-0x320, 0x640);
                    modelInfo->headRotStepScale = 0.06f;
                } else if (this->silentRomaniStareState == SILENT_ROMANI_STARE_STATE_STARE_RANDOM_MEDIUM) {
                    modelInfo->headRotTarget.x = Rand_S16Offset(-0x7D0, 0x3E8);
                    modelInfo->headRotTarget.y = Rand_S16Offset(-0x3E8, 0x7D0);
                    modelInfo->headRotTarget.z = Rand_S16Offset(-0x320, 0x640);
                    modelInfo->headRotStepScale = 0.05f;
                } else {
                    modelInfo->headRotTarget.x = Rand_S16Offset(-0x3E8, 0x7D0);
                    modelInfo->headRotTarget.y = Rand_S16Offset(-0x7D0, 0xFA0);
                    modelInfo->headRotTarget.z = Rand_S16Offset(-0x7D0, 0xFA0);
                    modelInfo->headRotStepScale = 0.04f;
                }
            }
        }

        this->timer--;
        return;
    }

    EnInvadepoh_SilentRomani_SetupWalk(this);
}

void EnInvadepoh_SilentRomani_SetupTalk(EnInvadepoh* this) {
    EnInvadepohModelInfo* modelInfo = &this->modelInfo;

    this->actor.speed = 0.0f;
    Animation_MorphToLoop(&this->skelAnime, &gRomaniIdleAnim, -10.0f);
    this->shapeAngularVelocityY = 0;
    modelInfo->headRotStepScale = 0.05f;
    modelInfo->headRotMaxStep = 0x4B0;
    this->actionFunc = EnInvadepoh_SilentRomani_Talk;
}

/**
 * Rotates Romani to face the player and handles the conversation she has with them. When she's done talking, Romani
 * will become idle, even if she was walking before the conversation started.
 */
void EnInvadepoh_SilentRomani_Talk(EnInvadepoh* this, PlayState* play) {
    EnInvadepohModelInfo* modelInfo = &this->modelInfo;
    Player* player = GET_PLAYER(play);
    s16 pitch;
    s16 yaw;

    Math_StepToS(&this->shapeAngularVelocityY, 0x7D0, 0x32);
    Math_SmoothStepToS(&this->actor.shape.rot.y, this->actor.yawTowardsPlayer, 6, this->shapeAngularVelocityY, 0x23);

    pitch = (s16)(Math_Vec3f_Pitch(&this->actor.focus.pos, &player->actor.focus.pos) * 0.85f) - this->actor.shape.rot.x;
    modelInfo->headRotTarget.x = CLAMP(pitch, -0xBB8, 0xBB8);

    yaw = this->actor.yawTowardsPlayer - this->actor.shape.rot.y;
    modelInfo->headRotTarget.y = CLAMP((s16)(yaw * 0.7f), -0x1F40, 0x1F40);

    if (Actor_TextboxIsClosing(&this->actor, play)) {
        EnInvadepoh_SilentRomani_SetupIdle(this);
    }
}

void EnInvadepoh_SilentRomani_WaitForObject(Actor* thisx, PlayState* play2) {
    PlayState* play = play2;
    EnInvadepoh* this = (EnInvadepoh*)thisx;

    if (Object_IsLoaded(&play->objectCtx, this->objectSlot)) {
        this->actor.objectSlot = this->objectSlot;
        Actor_SetObjectDependency(play, &this->actor);
        EnInvadepoh_Romani_DesegmentTextures();
        this->actor.update = EnInvadepoh_SilentRomani_Update;
        this->actor.draw = EnInvadepoh_Romani_Draw;
        this->actor.textId = 0x3330;
        SkelAnime_InitFlex(play, &this->skelAnime, &gRomaniSkel, &gRomaniIdleAnim, this->jointTable, this->morphTable,
                           ROMANI_LIMB_MAX);
        EnInvadepoh_ModelInfo_Init(&this->modelInfo, sRomaniEyeAnimations, ROMANI_EYE_ANIM_HALF, sRomaniMouthAnimations,
                                   ROMANI_MOUTH_ANIM_HANGING_OPEN, &gZeroVec3s, 100, 0.03f, 0.3f, 0.03f);
        EnInvadepoh_SilentRomani_InitPath(this, play);
        EnInvadepoh_SetPosToPathPoint(this, 0);
        func_800B4AEC(play, &this->actor, 50.0f);
        EnInvadepoh_SnapToFloor(this);
        EnInvadepoh_SilentRomani_SetupWalk(this);
    }
}

void EnInvadepoh_SilentRomani_Update(Actor* thisx, PlayState* play2) {
    PlayState* play = play2;
    EnInvadepoh* this = (EnInvadepoh*)thisx;
    s32 inUncullRange = CHECK_FLAG_ALL(this->actor.flags, ACTOR_FLAG_INSIDE_CULLING_VOLUME);
    s32 talkAccepted = Actor_TalkOfferAccepted(&this->actor, &play->state);

    if (talkAccepted) {
        Message_BombersNotebookQueueEvent(play, BOMBERS_NOTEBOOK_EVENT_MET_ROMANI);
        EnInvadepoh_SilentRomani_SetupTalk(this);
    }

    this->actionFunc(this, play);

    if (inUncullRange) {
        SkelAnime_Update(&this->skelAnime);
        EnInvadepoh_ModelInfo_Update(&this->modelInfo);

        if ((this->actionFunc != EnInvadepoh_SilentRomani_Talk) && !talkAccepted && this->actor.isLockedOn) {
            Actor_OfferTalk(&this->actor, play, 100.0f);
        }

        Collider_UpdateCylinder(&this->actor, &this->collider);
        CollisionCheck_SetOC(play, &play->colChkCtx, &this->collider.base);
    }
}

void EnInvadepoh_Ufo_SetupIntroDescend(EnInvadepoh* this) {
    this->actor.gravity = -15.0f;
    this->ufoPulseScale = 0.0f;
    this->ufoPulseScaleTarget = 1.0f;
    this->ufoPulseScaleSpeed = 0.0f;
    this->ufoPulsePhaseVelocity = 0;
    this->actionFunc = EnInvadepoh_Ufo_IntroDescend;
}

/**
 * This function handles the beginning of the invasion intro cutscene where the UFO descends from the sky. Once it gets
 * close enough to its target y-coordinate, the UFO will transition to waiting.
 */
void EnInvadepoh_Ufo_IntroDescend(EnInvadepoh* this, PlayState* play) {
    s32 pad;
    f32 distToTargetY;

    Actor_UpdateVelocityWithGravity(&this->actor);
    this->actor.velocity.y *= 0.97f;
    distToTargetY = Math_SmoothStepToF(&this->actor.world.pos.y, this->actor.home.pos.y + sUfoSpawnOffset.y + 300.0f,
                                       0.7f, fabsf(this->actor.velocity.y), 1.0f);
    Actor_PlaySfx_Flagged(&this->actor, NA_SE_EV_UFO_APPEAR - SFX_FLAG);

    if (fabsf(distToTargetY) < 1.0f) {
        EnInvadepoh_Ufo_SpawnSparkles(this, play, 50);
        EnInvadepoh_Ufo_SetupIntroWait(this);
    }
}

void EnInvadepoh_Ufo_SetupIntroWait(EnInvadepoh* this) {
    this->actor.velocity.y *= 0.1f;
    this->timer = 20;
    this->ufoPulseScaleTarget = 0.3f;
    this->ufoPulseScaleSpeed = 0.03f;
    this->ufoPulsePhaseVelocity = 0xBB8;
    this->actionFunc = EnInvadepoh_Ufo_IntroWait;
}

/**
 * This function handles the middle of the invasion intro cutscene where the UFO waits for 20 frames while gently
 * bobbing up and down. Once 20 frames pass, the UFO will start flying around while the aliens spawn.
 */
void EnInvadepoh_Ufo_IntroWait(EnInvadepoh* this, PlayState* play) {
    f32 targetY = this->actor.home.pos.y + sUfoSpawnOffset.y + 300.0f;

    if (this->actor.world.pos.y < targetY) {
        this->actor.gravity = 3.0f;
    } else {
        this->actor.gravity = -2.0f;
    }

    this->actor.velocity.y *= 0.96f;
    Actor_MoveWithGravity(&this->actor);
    Actor_PlaySfx_Flagged(&this->actor, NA_SE_EV_UFO_APPEAR - SFX_FLAG);

    this->timer--;
    if (this->timer <= 0) {
        EnInvadepoh_Ufo_SetupIntroFlyAround(this);
    }
}

void EnInvadepoh_Ufo_SetupIntroFlyAround(EnInvadepoh* this) {
    this->timer = 120;
    this->ufoPulseScaleTarget = 0.2f;
    this->ufoPulseScaleSpeed = 0.01f;
    this->actor.gravity = 33.0f;
    this->ufoPulsePhaseVelocity = 0xBB8;
    this->ufoTargetPosIndex = 0;
    this->frameCounter = 0;
    this->actionFunc = EnInvadepoh_Ufo_IntroFlyAround;
}

/**
 * This function handles the end of the invasion intro cutscene where the UFO zips around the ranch while the aliens
 * spawn. Note that this function doesn't actually spawn the aliens (`EnInvadepoh_InvasionHandler_SpawnAliens` is
 * responsible for that), but the in-game visuals make it appear as if the UFO is spawning the aliens as it flies
 * around. The UFO will move to five different positions, and will switch its target position every 25 frames. After 120
 * frames pass, this function will make the UFO fly to a point above the barn.
 */
void EnInvadepoh_Ufo_IntroFlyAround(EnInvadepoh* this, PlayState* play) {
    static Vec3f sUfoTargetOffsets[] = {
        { -1813.0f, 374.0f, 1900.0f }, { 2198.0f, 153.0f, 3365.0f }, { -1434.0f, 262.0f, 3365.0f },
        { -393.0f, 396.0f, 1084.0f },  { 0.0f, 1500.0f, 0.0f },
    };
    s32 pad;
    Vec3f ufoTargetPos;
    s32 pad2;

    if (this->frameCounter < 25) {
        this->frameCounter++;
    } else {
        this->frameCounter = 0;
        this->ufoTargetPosIndex++;
        this->ufoTargetPosIndex = CLAMP_MAX(this->ufoTargetPosIndex, ARRAY_COUNT(sUfoTargetOffsets) - 1);
        this->actor.gravity = 33.0f;
        EnInvadepoh_Ufo_SpawnSparkles(this, play, 20);
    }

    Math_Vec3f_Sum(&sUfoTargetOffsets[this->ufoTargetPosIndex], &this->actor.home.pos, &ufoTargetPos);

    if (Math3D_Vec3fDistSq(&this->actor.world.pos, &ufoTargetPos) < SQ(400.0f)) {
        this->actor.speed *= 0.8f;
    } else {
        Math_StepToF(&this->actor.speed, 170.0f, 21.0f);
        this->actor.speed *= 0.98f;
    }

    if (EnInvadepoh_StepToXZ(&this->actor.world.pos.x, &this->actor.world.pos.z, ufoTargetPos.x, ufoTargetPos.z,
                             this->actor.speed)) {
        this->actor.speed = 0.0f;
    }

    if (ufoTargetPos.y < this->actor.world.pos.y) {
        Math_StepToF(&this->actor.gravity, -12.0f, 7.0f);
    } else {
        Math_StepToF(&this->actor.gravity, 5.0f, 4.0f);
    }

    this->actor.velocity.y += this->actor.gravity;
    this->actor.velocity.y *= 0.97f;
    this->actor.world.pos.y += this->actor.velocity.y;

    Actor_PlaySfx_Flagged(&this->actor, NA_SE_EV_UFO_APPEAR - SFX_FLAG);

    if (this->timer > 0) {
        this->timer--;
    } else {
        EnInvadepoh_Ufo_SetupHoverAboveBarn(this);
    }
}

void EnInvadepoh_Ufo_SetupHoverAboveBarn(EnInvadepoh* this) {
    this->ufoPulseScaleTarget = 0.2f;
    this->ufoPulseScaleSpeed = 0.01f;
    this->ufoPulsePhaseVelocity = 0xBB8;
    this->actor.velocity.y *= 0.8f;
    this->actionFunc = EnInvadepoh_Ufo_HoverAboveBarn;
}

/**
 * Moves the UFO to directly above the barn if it is not already there, or makes it gently bob in place if it is. If the
 * player sucessfully defends the ranch from the aliens, then this function will make the UFO leave the barn.
 */
void EnInvadepoh_Ufo_HoverAboveBarn(EnInvadepoh* this, PlayState* play) {
    s32 pad;
    Vec3f ufoTargetPos;

    ufoTargetPos.x = this->actor.home.pos.x;
    ufoTargetPos.y = this->actor.home.pos.y + 1500.0f;
    ufoTargetPos.z = this->actor.home.pos.z;
    Math_SmoothStepToS(&this->actor.world.rot.y, Math_Vec3f_Yaw(&this->actor.world.pos, &ufoTargetPos), 0xA, 0xBB8,
                       0x64);

    if ((play->gameplayFrames % 64) < 14) {
        Math_StepToF(&this->actor.speed, 5.0f, 1.0f);
    } else {
        this->actor.speed *= 0.97f;
    }

    this->actor.gravity = (ufoTargetPos.y < this->actor.world.pos.y) ? -0.5f : 2.0f;
    this->actor.velocity.y *= 0.97f;

    Actor_MoveWithGravity(&this->actor);

    if (sInvasionState == INVASION_STATE_SUCCESS) {
        EnInvadepoh_Ufo_SetupOutroDescend(this);
    }
}

void EnInvadepoh_Ufo_SetupOutroCircle(EnInvadepoh* this) {
    this->ufoPulseScaleTarget = 0.2f;
    this->ufoPulseScaleSpeed = 0.01f;
    this->actor.gravity = -1.5f;
    this->ufoPulsePhaseVelocity = 0xBB8;
    this->timer = 35;
    this->actionFunc = EnInvadepoh_Ufo_OutroCircle;
}

/**
 * This function handles the middle of the invasion outro cutscene where the UFO flies to a position dictated by
 * `sUfoSpawnOffset` and circles in place for 35 frames before flying away.
 */
void EnInvadepoh_Ufo_OutroCircle(EnInvadepoh* this, PlayState* play) {
    s32 pad;
    Vec3f ufoTargetPos;

    ufoTargetPos.x = this->actor.home.pos.x + sUfoSpawnOffset.x;
    ufoTargetPos.y = this->actor.home.pos.y + sUfoSpawnOffset.y + 400.0f;
    ufoTargetPos.z = this->actor.home.pos.z + sUfoSpawnOffset.z;
    Math_SmoothStepToS(&this->actor.world.rot.y, Math_Vec3f_Yaw(&this->actor.world.pos, &ufoTargetPos), 4, 0x1F40,
                       0x64);
    Math_StepToF(&this->actor.speed, 70.0f, 3.0f);
    this->actor.gravity = (ufoTargetPos.y < this->actor.world.pos.y) ? -2.0f : 2.0f;
    this->actor.velocity.y *= 0.97f;

    Actor_MoveWithGravity(&this->actor);

    if (this->timer > 0) {
        this->timer--;
    } else {
        EnInvadepoh_Ufo_SetupOutroFlee(this);
    }
}

void EnInvadepoh_Ufo_SetupOutroFlee(EnInvadepoh* this) {
    this->ufoPulseScaleTarget = 0.2f;
    this->ufoPulseScaleSpeed = 0.01f;
    this->actor.gravity = 1.0f;
    this->ufoPulsePhaseVelocity = 0xBB8;
    this->timer = 60;
    this->actionFunc = EnInvadepoh_Ufo_OutroFlee;
}

/**
 * This function handles the end of the invasion outro cutscene where the UFO quickly flies away from the ranch. Once 60
 * frames pass, the UFO actor is killed.
 */
void EnInvadepoh_Ufo_OutroFlee(EnInvadepoh* this, PlayState* play) {
    Math_StepToF(&this->actor.speed, 150.0f, 4.0f);
    this->actor.velocity.y *= 0.95f;
    Actor_MoveWithGravity(&this->actor);

    if (this->timer > 0) {
        this->timer--;
    } else {
        Actor_Kill(&this->actor);
    }
}

void EnInvadepoh_Ufo_SetupOutroDescend(EnInvadepoh* this) {
    this->timer = 40;
    this->ufoPulseScaleTarget = 0.2f;
    this->ufoPulseScaleSpeed = 0.01f;
    this->actor.speed = 0.0f;
    this->ufoPulsePhaseVelocity = 0xBB8;
    this->actionFunc = EnInvadepoh_Ufo_OutroDescend;
}

/**
 * This function handles the beginning of the invasion outro cutscene where the UFO moves 700 units closer to the ground
 * and waits for 40 frames. During the outro cutscene, this entire process happens off-camera, so the player can't
 * actually see this descent.
 */
void EnInvadepoh_Ufo_OutroDescend(EnInvadepoh* this, PlayState* play) {
    s32 pad;
    Vec3f ufoTargetPos;

    ufoTargetPos.x = this->actor.home.pos.x;
    ufoTargetPos.y = this->actor.home.pos.y + 800.0f;
    ufoTargetPos.z = this->actor.home.pos.z;
    Math_SmoothStepToS(&this->actor.world.rot.y, Math_Vec3f_Yaw(&this->actor.world.pos, &ufoTargetPos), 4, 0x1F40,
                       0x64);
    Math_StepToF(&this->actor.speed, 30.0f, 3.0f);
    this->actor.velocity.y *= 0.98f;
    this->actor.gravity = (ufoTargetPos.y < this->actor.world.pos.y) ? -0.5f : 2.0f;
    Actor_MoveWithGravity(&this->actor);
    Actor_UpdateBgCheckInfo(play, &this->actor, 0.0f, 0.0f, 0.0f, UPDBGCHECKINFO_FLAG_4);

    if (this->timer > 0) {
        this->timer--;
    } else {
        EnInvadepoh_Ufo_SetupOutroCircle(this);
    }
}

void EnInvadepoh_Ufo_Update(Actor* thisx, PlayState* play2) {
    PlayState* play = play2;
    EnInvadepoh* this = (EnInvadepoh*)thisx;
    f32 scaleMod;

    this->actionFunc(this, play);

    this->ufoPulsePhase += this->ufoPulsePhaseVelocity;
    Math_StepToF(&this->ufoPulseScale, this->ufoPulseScaleTarget, this->ufoPulseScaleSpeed);

    scaleMod = 1.0f + (Math_SinS(this->ufoPulsePhase) * this->ufoPulseScale);
    Actor_SetScale(&this->actor, 0.27f * scaleMod);

    Math_StepToS(&this->angularVelocity, 0x258, 8);
    this->actor.world.rot.y += this->angularVelocity;
    this->angle += 0x258;
}

void EnInvadepoh_Night1Romani_SetupWalk(EnInvadepoh* this) {
    EnInvadepohModelInfo* modelInfo = &this->modelInfo;

    Animation_MorphToLoop(&this->skelAnime, &gRomaniWalkAnim, -10.0f);
    modelInfo->headRotTarget.x = 0;
    modelInfo->headRotTarget.y = 0;
    modelInfo->headRotTarget.z = 0;
    modelInfo->headRotStepScale = 0.1f;
    modelInfo->headRotMaxStep = 0x320;
    this->actionFunc = EnInvadepoh_Night1Romani_Walk;
}

/**
 * Walks Romani along the path to the barn, opening the door at the beginning and end of her path. Once Romani gets very
 * close to the end of her path, this function kills the actor.
 */
void EnInvadepoh_Night1Romani_Walk(EnInvadepoh* this, PlayState* play) {
    s32 pad;

    EnInvadepoh_Night1Romani_PathComputeProgress(this);
    EnInvadepoh_Night1Romani_MoveAlongTimePath(this, play);
    EnInvadepoh_Romani_StepYawAlongPath(this, 6, 0x7D0, 0x64);

    if ((this->currentPoint == 0) || ((this->currentPoint + 1) == this->endPoint)) {
        if (!this->doorOpened) {
            s32 doorTimer = Environment_GetTimeSpeed(play);

            if (doorTimer > 0) {
                doorTimer = (R_TIME_SPEED * -16.0f / doorTimer) - 0.5f;
                this->doorOpened = EnInvadepoh_Romani_OpenDoor(this, play, SQ(80.0f), doorTimer);
            }
        }

        this->actor.flags &= ~(ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_FRIENDLY);
    } else {
        this->doorOpened = false;
        this->actor.flags |= (ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_FRIENDLY);
    }

    if (CHECK_FLAG_ALL(this->actor.flags, ACTOR_FLAG_INSIDE_CULLING_VOLUME) &&
        (Animation_OnFrame(&this->skelAnime, 0.0f) || Animation_OnFrame(&this->skelAnime, 7.0f))) {
        Actor_PlaySfx(&this->actor, NA_SE_EN_ROMANI_WALK);
    }

    if (this->pathProgress >= 0.9999f) {
        Actor_Kill(&this->actor);
    }
}

void EnInvadepoh_Night1Romani_SetupTalk(EnInvadepoh* this) {
    this->modelInfo.headRotStepScale = 0.08f;
    this->modelInfo.headRotMaxStep = 0x7D0;
    this->shapeAngularVelocityY = 0;
    Animation_MorphToLoop(&this->skelAnime, &gRomaniIdleAnim, -10.0f);
    this->actionFunc = EnInvadepoh_Night1Romani_Talk;
}

/**
 * Rotates Romani to face the player and handles the conversation she has with them. When she's done talking, Romani
 * will continue walking towards the barn.
 */
void EnInvadepoh_Night1Romani_Talk(EnInvadepoh* this, PlayState* play) {
    EnInvadepohModelInfo* modelInfo = &this->modelInfo;
    Player* player = GET_PLAYER(play);
    s16 pitch;
    s16 yaw;

    Math_StepToS(&this->shapeAngularVelocityY, 0x7D0, 0x1F4);
    Math_SmoothStepToS(&this->actor.shape.rot.y, this->actor.yawTowardsPlayer, 5, this->shapeAngularVelocityY, 0x28);

    pitch = (s16)(Math_Vec3f_Pitch(&this->actor.focus.pos, &player->actor.focus.pos) * 0.85f) - this->actor.shape.rot.x;
    modelInfo->headRotTarget.x = CLAMP(pitch, -0xBB8, 0xBB8);

    yaw = this->actor.yawTowardsPlayer - this->actor.shape.rot.y;
    modelInfo->headRotTarget.y = CLAMP((s16)(yaw * 0.7f), -0x1F40, 0x1F40);

    if (Actor_TextboxIsClosing(&this->actor, play)) {
        if (this->actor.textId == 0x332D) {
            SET_WEEKEVENTREG(WEEKEVENTREG_TALKED_ROMANI_ON_NIGHT_1);
            this->actor.textId = 0x332E;
        }

        EnInvadepoh_Night1Romani_SetupWalk(this);
    }
}

void EnInvadepoh_Night1Romani_WaitForObject(Actor* thisx, PlayState* play2) {
    PlayState* play = play2;
    EnInvadepoh* this = (EnInvadepoh*)thisx;
    s32 pad;

    if (Object_IsLoaded(&play->objectCtx, this->objectSlot)) {
        s32 currentTime = CURRENT_TIME;

        this->actor.objectSlot = this->objectSlot;
        Actor_SetObjectDependency(play, &this->actor);
        EnInvadepoh_Romani_DesegmentTextures();
        SkelAnime_InitFlex(play, &this->skelAnime, &gRomaniSkel, &gRomaniWalkAnim, this->jointTable, this->morphTable,
                           ROMANI_LIMB_MAX);
        EnInvadepoh_ModelInfo_Init(&this->modelInfo, sRomaniEyeAnimations, ROMANI_EYE_ANIM_OPEN_THEN_RANDOM_BLINK,
                                   sRomaniMouthAnimations, ROMANI_MOUTH_ANIM_FROWN, &gZeroVec3s, 0x64, 0.03f, 0.3f,
                                   0.03f);
        EnInvadepoh_Night1Romani_InitPath(this, play);
        EnInvadepoh_Night1Romani_PathComputeProgress(this);
        EnInvadepoh_Night1Romani_MoveAlongTimePath(this, play);
        EnInvadepoh_SetYawAlongPath(this);
        EnInvadepoh_SnapToFloor(this);

        if (CHECK_WEEKEVENTREG(WEEKEVENTREG_PROMISED_TO_HELP_WITH_ALIENS)) {
            if (CHECK_WEEKEVENTREG(WEEKEVENTREG_TALKED_ROMANI_ON_NIGHT_1)) {
                this->actor.textId = 0x332E;
            } else {
                this->actor.textId = 0x332D;
            }
        } else {
            this->actor.textId = 0x332C;
        }

        if ((currentTime >= CLOCK_TIME(18, 00)) || (currentTime < CLOCK_TIME(2, 00))) {
            this->actor.update = EnInvadepoh_Night1Romani_WaitForTime;
            this->actor.draw = NULL;
        } else if ((currentTime < CLOCK_TIME(6, 00)) && (currentTime >= CLOCK_TIME(2, 00)) &&
                   (currentTime < CLOCK_TIME(2, 15))) {
            this->actor.update = EnInvadepoh_Night1Romani_Update;
            this->actor.draw = EnInvadepoh_Romani_Draw;
            EnInvadepoh_Night1Romani_SetupWalk(this);
        } else {
            Actor_Kill(&this->actor);
        }
    }
}

/**
 * Waits until 2:00 AM, then Romani becomes visible and starts walking.
 */
void EnInvadepoh_Night1Romani_WaitForTime(Actor* thisx, PlayState* play2) {
    PlayState* play = play2;
    EnInvadepoh* this = (EnInvadepoh*)thisx;

    if ((CURRENT_TIME < CLOCK_TIME(6, 00)) && (CURRENT_TIME >= CLOCK_TIME(2, 00))) {
        this->actor.update = EnInvadepoh_Night1Romani_Update;
        this->actor.draw = EnInvadepoh_Romani_Draw;
        EnInvadepoh_Night1Romani_SetupWalk(this);
    }
}

void EnInvadepoh_Night1Romani_Update(Actor* thisx, PlayState* play2) {
    PlayState* play = play2;
    EnInvadepoh* this = (EnInvadepoh*)thisx;
    s32 inUncullRange = CHECK_FLAG_ALL(this->actor.flags, ACTOR_FLAG_INSIDE_CULLING_VOLUME);
    s32 talkAccepted = Actor_TalkOfferAccepted(&this->actor, &play->state);

    if (talkAccepted) {
        Message_BombersNotebookQueueEvent(play, BOMBERS_NOTEBOOK_EVENT_MET_ROMANI);
        EnInvadepoh_Night1Romani_SetupTalk(this);
    }

    this->actionFunc(this, play);

    if (inUncullRange && (this->actor.update != NULL)) {
        SkelAnime_Update(&this->skelAnime);
        EnInvadepoh_ModelInfo_Update(&this->modelInfo);

        if ((this->actionFunc != EnInvadepoh_Night1Romani_Talk) && !talkAccepted && this->actor.isLockedOn) {
            Actor_OfferTalk(&this->actor, play, 350.0f);
        }

        Collider_UpdateCylinder(&this->actor, &this->collider);
        CollisionCheck_SetOC(play, &play->colChkCtx, &this->collider.base);
    }
}

void EnInvadepoh_BarnRomani_SetupIdle(EnInvadepoh* this) {
    EnInvadepohModelInfo* modelInfo = &this->modelInfo;

    Animation_MorphToLoop(&this->skelAnime, &gRomaniIdleAnim, -10.0f);
    modelInfo->headRotTarget.x = 0;
    modelInfo->headRotTarget.y = 0;
    modelInfo->headRotTarget.z = 0;
    modelInfo->headRotStepScale = 0.1f;
    modelInfo->headRotMaxStep = 0x5DC;
    this->timer = Rand_S16Offset(200, 200);
    this->angle = this->actor.shape.rot.y;
    this->actor.flags |= (ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_FRIENDLY);
    this->actionFunc = EnInvadepoh_BarnRomani_Idle;
}

/**
 * Waits for a random amount of time between 200 and 400 frames, then begins looking around. Her rotation will initially
 * be unchanged, but after a certain amount of time passes (specifically, when `play->gameplayFrames` is divisible by
 * 256), Romani will turn and look towards the door instead.
 */
void EnInvadepoh_BarnRomani_Idle(EnInvadepoh* this, PlayState* play) {
    s32 pad;
    EnInvadepohModelInfo* modelInfo = &this->modelInfo;

    if ((play->gameplayFrames % 256) == 0) {
        Vec3f initialPoint;
        s16 yawToDoor;

        Math_Vec3s_ToVec3f(&initialPoint, &this->pathPoints[0]);
        yawToDoor = Math_Vec3f_Yaw(&this->actor.world.pos, &initialPoint);
        this->angle = Rand_S16Offset(-0x1F40, 0x3E80) + yawToDoor;
        this->shapeAngularVelocityY = 0;
    }

    Math_StepToS(&this->shapeAngularVelocityY, 0x7D0, 0x28);
    Math_SmoothStepToS(&this->actor.shape.rot.y, this->angle, 6, this->shapeAngularVelocityY, 0x28);

    if (this->actor.xzDistToPlayer < 300.0f) {
        Player* player = GET_PLAYER(play);
        s16 pitch =
            (s16)(Math_Vec3f_Pitch(&this->actor.focus.pos, &player->actor.focus.pos) * 0.85f) - this->actor.shape.rot.x;
        s16 yaw;

        modelInfo->headRotTarget.x = CLAMP(pitch, -0x9C4, 0x9C4);

        yaw = this->actor.yawTowardsPlayer - this->actor.shape.rot.y;
        modelInfo->headRotTarget.y = CLAMP((s16)(yaw * 0.7f), -0x1F40, 0x1F40);

        if ((play->gameplayFrames % 256) == 0) {
            modelInfo->headRotTarget.z = Rand_S16Offset(-0x5DC, 0xBB8);
        }
    } else {
        modelInfo->headRotTarget.x = 0;
        modelInfo->headRotTarget.y = 0;
        modelInfo->headRotTarget.z = 0;
    }

    if (this->timer > 0) {
        this->timer--;
    } else {
        EnInvadepoh_BarnRomani_SetupLookAround(this);
    }
}

void EnInvadepoh_BarnRomani_SetupLookAround(EnInvadepoh* this) {
    EnInvadepohModelInfo* modelInfo = &this->modelInfo;

    Animation_MorphToPlayOnce(&this->skelAnime, &gRomaniLookAroundAnim, -10.0f);
    modelInfo->headRotTarget.x = 0;
    modelInfo->headRotTarget.y = 0;
    modelInfo->headRotTarget.z = 0;
    modelInfo->headRotStepScale = 0.1f;
    modelInfo->headRotMaxStep = 0x320;
    this->actor.flags |= (ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_FRIENDLY);
    this->actionFunc = EnInvadepoh_BarnRomani_LookAround;
}

/**
 * Plays Romani's look around animation to completion, then goes back to being idle.
 */
void EnInvadepoh_BarnRomani_LookAround(EnInvadepoh* this, PlayState* play) {
    if (this->isAnimFinished) {
        EnInvadepoh_BarnRomani_SetupIdle(this);
    }
}

void EnInvadepoh_BarnRomani_SetupWalk(EnInvadepoh* this) {
    EnInvadepohModelInfo* modelInfo = &this->modelInfo;

    Animation_MorphToLoop(&this->skelAnime, &gRomaniWalkAnim, 0.0f);
    modelInfo->headRotTarget.x = 0;
    modelInfo->headRotTarget.y = 0;
    modelInfo->headRotTarget.z = 0;
    modelInfo->headRotStepScale = 0.1f;
    modelInfo->headRotMaxStep = 0x320;
    this->actionFunc = EnInvadepoh_BarnRomani_Walk;
}

/**
 * Walks Romani along the path in the barn, opening the door if necessary. Each time she reaches a new path point, she
 * gradually slows down; once she reaches the final path point, she stops entirely and becomes idle.
 */
void EnInvadepoh_BarnRomani_Walk(EnInvadepoh* this, PlayState* play) {
    s32 pad;

    Math_StepToF(&this->actor.speed, 5.0f, 1.0f);

    if (EnInvadepoh_Romani_MoveAlongPath(this, play, this->actor.speed, 50.0f)) {
        EnInvadepoh_BarnRomani_SetPathPointToNext(this);
        this->shapeAngularVelocityY = 0x5DC;
        this->actor.speed *= 0.5f;
    } else {
        Math_StepToS(&this->shapeAngularVelocityY, 0x190, 0x32);
    }

    EnInvadepoh_Romani_StepYawAlongPath(this, 6, this->shapeAngularVelocityY, 50);

    if (this->currentPoint == 0) {
        if (!this->doorOpened) {
            this->doorOpened = EnInvadepoh_Romani_OpenDoor(this, play, SQ(80.0f), -15);
        }

        this->actor.flags &= ~(ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_FRIENDLY);
    } else {
        this->doorOpened = false;
        this->actor.flags |= (ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_FRIENDLY);
    }

    if (CHECK_FLAG_ALL(this->actor.flags, ACTOR_FLAG_INSIDE_CULLING_VOLUME) &&
        (Animation_OnFrame(&this->skelAnime, 0.0f) || Animation_OnFrame(&this->skelAnime, 7.0f))) {
        Actor_PlaySfx(&this->actor, NA_SE_EN_ROMANI_WALK);
    }
    if (this->currentPoint == this->endPoint) {
        EnInvadepoh_BarnRomani_SetupIdle(this);
    }
}

void EnInvadepoh_BarnRomani_SetupTalk(EnInvadepoh* this) {
    this->modelInfo.headRotStepScale = 0.08f;
    this->modelInfo.headRotMaxStep = 0x7D0;
    this->shapeAngularVelocityY = 0;
    Animation_MorphToLoop(&this->skelAnime, &gRomaniIdleAnim, 0.0f);
    this->actionFunc = EnInvadepoh_BarnRomani_Talk;
}

/**
 * Rotates Romani to face the player and handles the conversation she has with them. When she's done talking, Romani
 * will either begin walking or stand idly, depending on whether or not she has reached the end of her path yet.
 */
void EnInvadepoh_BarnRomani_Talk(EnInvadepoh* this, PlayState* play) {
    EnInvadepohModelInfo* modelInfo = &this->modelInfo;
    Player* player = GET_PLAYER(play);
    s16 pitch;
    s16 yaw;

    Math_StepToS(&this->shapeAngularVelocityY, 0x7D0, 0x1F4);
    Math_SmoothStepToS(&this->actor.shape.rot.y, this->actor.yawTowardsPlayer, 5, this->shapeAngularVelocityY, 0x28);

    pitch = (s16)(Math_Vec3f_Pitch(&this->actor.focus.pos, &player->actor.focus.pos) * 0.85f) - this->actor.shape.rot.x;
    modelInfo->headRotTarget.x = CLAMP(pitch, -0xBB8, 0xBB8);

    yaw = this->actor.yawTowardsPlayer - this->actor.shape.rot.y;
    modelInfo->headRotTarget.y = CLAMP((s16)(yaw * 0.7f), -0x1F40, 0x1F40);

    if (Actor_TextboxIsClosing(&this->actor, play)) {
        if (this->actor.textId == 0x332D) {
            SET_WEEKEVENTREG(WEEKEVENTREG_TALKED_ROMANI_ON_NIGHT_1);
            this->actor.textId = 0x332E;
        }

        if (this->currentPoint == this->endPoint) {
            EnInvadepoh_BarnRomani_SetupIdle(this);
        } else {
            EnInvadepoh_BarnRomani_SetupWalk(this);
        }
    }
}

void EnInvadepoh_BarnRomani_WaitForObject(Actor* thisx, PlayState* play2) {
    PlayState* play = play2;
    EnInvadepoh* this = (EnInvadepoh*)thisx;
    s32 pad;

    if (Object_IsLoaded(&play->objectCtx, this->objectSlot)) {
        s32 currentTime = CURRENT_TIME;

        this->actor.objectSlot = this->objectSlot;
        Actor_SetObjectDependency(play, &this->actor);
        EnInvadepoh_Romani_DesegmentTextures();
        SkelAnime_InitFlex(play, &this->skelAnime, &gRomaniSkel, &gRomaniWalkAnim, this->jointTable, this->morphTable,
                           ROMANI_LIMB_MAX);
        EnInvadepoh_ModelInfo_Init(&this->modelInfo, sRomaniEyeAnimations, ROMANI_EYE_ANIM_OPEN_THEN_RANDOM_BLINK,
                                   sRomaniMouthAnimations, ROMANI_MOUTH_ANIM_FROWN, &gZeroVec3s, 100, 0.03f, 0.3f,
                                   0.03f);
        EnInvadepoh_BarnRomani_InitPath(this, play);

        if ((currentTime < CLOCK_TIME(2, 15)) || (currentTime >= CLOCK_TIME(6, 00))) {
            this->currentPoint = 0;
            this->actor.update = EnInvadepoh_BarnRomani_WaitForTime;
        } else {
            this->currentPoint = this->endPoint;
            this->actor.update = EnInvadepoh_BarnRomani_Update;
            this->actor.draw = EnInvadepoh_Romani_Draw;
            EnInvadepoh_BarnRomani_SetupIdle(this);
        }
        EnInvadepoh_SetPosToPathPoint(this, this->currentPoint);
        EnInvadepoh_SetYawAlongPath(this);
        func_800B4AEC(play, &this->actor, 50.0f);
        EnInvadepoh_SnapToFloor(this);

        if (CHECK_WEEKEVENTREG(WEEKEVENTREG_PROMISED_TO_HELP_WITH_ALIENS)) {
            if (CHECK_WEEKEVENTREG(WEEKEVENTREG_TALKED_ROMANI_ON_NIGHT_1)) {
                this->actor.textId = 0x332E;
            } else {
                this->actor.textId = 0x332D;
            }
        } else {
            this->actor.textId = 0x332C;
        }
    }
}

/**
 * Waits until 2:15 AM, then Romani becomes visible and starts walking.
 */
void EnInvadepoh_BarnRomani_WaitForTime(Actor* thisx, PlayState* play2) {
    PlayState* play = play2;
    EnInvadepoh* this = (EnInvadepoh*)thisx;

    if ((CURRENT_TIME < CLOCK_TIME(6, 00)) && (CURRENT_TIME >= CLOCK_TIME(2, 15))) {
        this->actor.update = EnInvadepoh_BarnRomani_Update;
        this->actor.draw = EnInvadepoh_Romani_Draw;
        EnInvadepoh_BarnRomani_SetupWalk(this);
    }
}

void EnInvadepoh_BarnRomani_Update(Actor* thisx, PlayState* play2) {
    PlayState* play = play2;
    EnInvadepoh* this = (EnInvadepoh*)thisx;
    s32 inUncullRange = CHECK_FLAG_ALL(this->actor.flags, ACTOR_FLAG_INSIDE_CULLING_VOLUME);
    s32 talkAccepted = Actor_TalkOfferAccepted(&this->actor, &play->state);

    if (talkAccepted) {
        Message_BombersNotebookQueueEvent(play, BOMBERS_NOTEBOOK_EVENT_MET_ROMANI);
        EnInvadepoh_BarnRomani_SetupTalk(this);
    }

    this->actionFunc(this, play);

    if (inUncullRange) {
        this->isAnimFinished = SkelAnime_Update(&this->skelAnime);
        EnInvadepoh_ModelInfo_Update(&this->modelInfo);

        if ((this->actionFunc != EnInvadepoh_BarnRomani_Talk) && !talkAccepted && this->actor.isLockedOn) {
            Actor_OfferTalk(&this->actor, play, 100.0f);
        }

        Collider_UpdateCylinder(&this->actor, &this->collider);
        CollisionCheck_SetOC(play, &play->colChkCtx, &this->collider.base);
    }
}

void EnInvadepoh_RewardRomani_SetupWaitForSuccess(EnInvadepoh* this) {
    this->actionFunc = EnInvadepoh_RewardRomani_WaitForSuccess;
}

/**
 * Waits until the weekeventreg for defending the ranch is set, then becomes visible and starts talking.
 */
void EnInvadepoh_RewardRomani_WaitForSuccess(EnInvadepoh* this, PlayState* play) {
    if (CHECK_WEEKEVENTREG(WEEKEVENTREG_DEFENDED_AGAINST_ALIENS)) {
        this->actor.draw = EnInvadepoh_Romani_Draw;
        this->actor.flags |= (ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_FRIENDLY);
        EnInvadepoh_RewardRomani_SetupStartTalking(this);
    }
}

void EnInvadepoh_RewardRomani_SetupStartTalking(EnInvadepoh* this) {
    this->actor.flags |= ACTOR_FLAG_TALK_OFFER_AUTO_ACCEPTED;
    this->actionFunc = EnInvadepoh_RewardRomani_StartTalking;
}

/**
 * Starts the conversation with Romani where she thanks you for defending the ranch.
 */
void EnInvadepoh_RewardRomani_StartTalking(EnInvadepoh* this, PlayState* play) {
    if (Actor_TalkOfferAccepted(&this->actor, &play->state)) {
        EnInvadepoh_Romani_StartTextbox(this, play, 0x3331);
        EnInvadepoh_RewardRomani_SetupTalk(this);
    } else {
        Actor_OfferTalk(&this->actor, play, 2000.0f);
    }
}

void EnInvadepoh_RewardRomani_SetupTalk(EnInvadepoh* this) {
    this->actionFunc = EnInvadepoh_RewardRomani_Talk;
}

/**
 * Handles the conversation with Romani, including giving the player the Milk Bottle reward (if they have not already
 * received it in a previous cycle) and ending the conversation.
 */
void EnInvadepoh_RewardRomani_Talk(EnInvadepoh* this, PlayState* play) {
    if ((Message_GetState(&play->msgCtx) == TEXT_STATE_EVENT) && Message_ShouldAdvance(play)) {
        if (this->textId == 0x3331) {
            if (CHECK_WEEKEVENTREG(WEEKEVENTREG_RECEIVED_ALIENS_BOTTLE)) {
                EnInvadepoh_Romani_StartTextbox(this, play, 0x3334);
                Message_BombersNotebookQueueEvent(play, BOMBERS_NOTEBOOK_EVENT_DEFENDED_AGAINST_ALIENS);
                Message_BombersNotebookQueueEvent(play, BOMBERS_NOTEBOOK_EVENT_MET_ROMANI);
            } else {
                EnInvadepoh_Romani_StartTextbox(this, play, 0x3333);
            }
        } else if (this->textId == 0x3333) {
            EnInvadepoh_RewardRomani_SetupGiveBottle(this);
        } else if (this->textId == 0x3334) {
            Message_CloseTextbox(play);
            EnInvadepoh_RewardRomani_SetupFinish(this);
        }
    }
}

void EnInvadepoh_RewardRomani_SetupGiveBottle(EnInvadepoh* this) {
    this->timer = 2;
    this->actionFunc = EnInvadepoh_RewardRomani_GiveBottle;
}

/**
 * Waits two frames, then offers the player the Milk Bottle.
 */
void EnInvadepoh_RewardRomani_GiveBottle(EnInvadepoh* this, PlayState* play) {
    if (this->timer > 0) {
        this->timer--;
        if (this->timer == 0) {
            Message_CloseTextbox(play);
        }
    }

    if (Actor_HasParent(&this->actor, play)) {
        this->actor.parent = NULL;
        SET_WEEKEVENTREG(WEEKEVENTREG_RECEIVED_ALIENS_BOTTLE);
        EnInvadepoh_RewardRomani_SetupAfterGivingBottle(this);
    } else {
        Actor_OfferGetItem(&this->actor, play, GI_MILK_BOTTLE, 2000.0f, 2000.0f);
    }
}

void EnInvadepoh_RewardRomani_SetupAfterGivingBottle(EnInvadepoh* this) {
    this->actionFunc = EnInvadepoh_RewardRomani_AfterGivingBottle;
}

/**
 * Restarts the conversation after the player has received the Milk Bottle from Romani.
 */
void EnInvadepoh_RewardRomani_AfterGivingBottle(EnInvadepoh* this, PlayState* play) {
    if (Actor_TalkOfferAccepted(&this->actor, &play->state)) {
        EnInvadepoh_Romani_StartTextbox(this, play, 0x3334);
        Message_BombersNotebookQueueEvent(play, BOMBERS_NOTEBOOK_EVENT_RECEIVED_ALIENS_BOTTLE);
        Message_BombersNotebookQueueEvent(play, BOMBERS_NOTEBOOK_EVENT_DEFENDED_AGAINST_ALIENS);
        Message_BombersNotebookQueueEvent(play, BOMBERS_NOTEBOOK_EVENT_MET_ROMANI);
        EnInvadepoh_RewardRomani_SetupTalk(this);
    } else {
        Actor_OfferTalkExchangeEquiCylinder(&this->actor, play, 2000.0f, PLAYER_IA_MINUS1);
    }
}

void EnInvadepoh_RewardRomani_SetupFinish(EnInvadepoh* this) {
    this->actor.flags &= ~(ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_FRIENDLY);
    this->actionFunc = EnInvadepoh_RewardRomani_Finish;
}

/**
 * Waits until all messages and Bomber's Notebook events are done, then sets `sRewardFinished` to true. Doing this will
 * cause the invasion handler to transition the player back to the normal version of Romani Ranch where Romani is no
 * longer present.
 */
void EnInvadepoh_RewardRomani_Finish(EnInvadepoh* this, PlayState* play) {
    if (play->msgCtx.bombersNotebookEventQueueCount == 0) {
        if (play->msgCtx.msgMode == MSGMODE_NONE) {
            sRewardFinished = true;
        } else if ((Message_GetState(&play->msgCtx) == TEXT_STATE_DONE) ||
                   (Message_GetState(&play->msgCtx) == TEXT_STATE_EVENT)) {
            sRewardFinished = true;
        }
    }
}

void EnInvadepoh_RewardRomani_WaitForObject(Actor* thisx, PlayState* play2) {
    PlayState* play = play2;
    EnInvadepoh* this = (EnInvadepoh*)thisx;
    EnInvadepohModelInfo* modelInfo = &this->modelInfo;

    if (Object_IsLoaded(&play2->objectCtx, this->objectSlot)) {
        this->actor.objectSlot = this->objectSlot;
        Actor_SetObjectDependency(play, &this->actor);
        EnInvadepoh_Romani_DesegmentTextures();
        this->actor.update = EnInvadepoh_RewardRomani_Update;
        SkelAnime_InitFlex(play, &this->skelAnime, &gRomaniSkel, &gRomaniWalkAnim, this->jointTable, this->morphTable,
                           ROMANI_LIMB_MAX);
        Animation_MorphToLoop(&this->skelAnime, &gRomaniIdleAnim, 0.0f);
        EnInvadepoh_ModelInfo_Init(&this->modelInfo, sRomaniEyeAnimations, ROMANI_EYE_ANIM_OPEN_THEN_RANDOM_BLINK,
                                   sRomaniMouthAnimations, ROMANI_MOUTH_ANIM_SMILE, &gZeroVec3s, 0x7D0, 0.08f, 0.3f,
                                   0.03f);
        modelInfo->headRotStepScale = 0.08f;
        modelInfo->headRotMaxStep = 0x7D0;
        func_800B4AEC(play, &this->actor, 50.0f);
        EnInvadepoh_SnapToFloor(this);
        EnInvadepoh_RewardRomani_SetupWaitForSuccess(this);
    }
}

void EnInvadepoh_RewardRomani_Update(Actor* thisx, PlayState* play2) {
    PlayState* play = play2;
    EnInvadepoh* this = (EnInvadepoh*)thisx;
    EnInvadepohModelInfo* modelInfo = &this->modelInfo;
    s32 inUncullRange = CHECK_FLAG_ALL(this->actor.flags, ACTOR_FLAG_INSIDE_CULLING_VOLUME);

    this->actionFunc(this, play);

    if (inUncullRange && (this->actor.update != NULL)) {
        Player* player;
        s16 pitch;
        s16 yaw;

        SkelAnime_Update(&this->skelAnime);
        player = GET_PLAYER(play);
        Math_StepToS(&this->shapeAngularVelocityY, 0x7D0, 0x28);
        Math_SmoothStepToS(&this->actor.shape.rot.y, this->actor.yawTowardsPlayer, 6, this->shapeAngularVelocityY, 40);

        pitch =
            (s16)(Math_Vec3f_Pitch(&this->actor.focus.pos, &player->actor.focus.pos) * 0.9f) - this->actor.shape.rot.x;
        modelInfo->headRotTarget.x = CLAMP(pitch, -0xBB8, 0xBB8);

        yaw = this->actor.yawTowardsPlayer - this->actor.shape.rot.y;
        modelInfo->headRotTarget.y = CLAMP((s16)(yaw * 0.7f), -0x1F40, 0x1F40);

        EnInvadepoh_ModelInfo_Update(&this->modelInfo);
        Collider_UpdateCylinder(&this->actor, &this->collider);
        CollisionCheck_SetOC(play, &play->colChkCtx, &this->collider.base);
    }
}

void EnInvadepoh_Dog_PlayWalkSfx(EnInvadepoh* this) {
    s32 pad;

    if (Animation_OnFrame(&this->skelAnime, 1.0f) || Animation_OnFrame(&this->skelAnime, 7.0f)) {
        Actor_PlaySfx(&this->actor, NA_SE_EV_MONKEY_WALK);
    }
}

void EnInvadepoh_Dog_SetupWalk(EnInvadepoh* this) {
    Animation_MorphToLoop(&this->skelAnime, &gDogWalkAnim, -6.0f);
    this->timer = Rand_S16Offset(50, 80);
    this->actionFunc = EnInvadepoh_Dog_Walk;
}

/**
 * Makes the dog walk for a random amount of time between 50 and 80 frames, then makes the dog run again. This function
 * will only run if the dog does not currently have a point along its path to target; this only happens if all living
 * aliens are greater than 2000 units away from the barn, and thus none of them are considered a threat. If an alien
 * gets within 2000 units of the barn while the dog is walking, then the dog will have a new target point along its
 * path, and this function will make the dog immediately start running regardless of how long it was previously walking.
 */
void EnInvadepoh_Dog_Walk(EnInvadepoh* this, PlayState* play) {
    Math_StepToF(&this->actor.speed, 1.1f, 0.5f);

    if (EnInvadepoh_Dog_MoveAlongPath(this, play)) {
        EnInvadepoh_Dog_SetPathPointToNext(this);
    }

    EnInvadepoh_Dog_PlayWalkSfx(this);

    if (this->dogTargetPoint >= 0) {
        this->timer = 0;
    }

    if (this->timer > 0) {
        this->timer--;
    } else {
        EnInvadepoh_Dog_SetupRun(this);
    }
}

void EnInvadepoh_Dog_SetupRun(EnInvadepoh* this) {
    Animation_MorphToLoop(&this->skelAnime, &gDogRunAnim, -6.0f);
    this->timer = Rand_S16Offset(50, 200);
    this->actionFunc = EnInvadepoh_Dog_Run;
}

/**
 * Makes the dog run for a random amount of time between 50 and 200 frames or until it reaches the target point along
 * its path. Once either of these conditions are met, the dog can do one of two things. If there are no living aliens
 * within 2000 units of the barn, then the dog will start walking. Otherwise, it will stop and bark (and sometimes
 * randomly growl) at whichever alien is closest to the barn.
 */
void EnInvadepoh_Dog_Run(EnInvadepoh* this, PlayState* play) {
    Math_StepToF(&this->actor.speed, 3.8f, 0.45f);

    if (this->dogTargetPoint >= 0) {
        f32 distToTarget;
        Vec3f targetPathPoint;

        Math_Vec3s_ToVec3f(&targetPathPoint, &this->pathPoints[this->dogTargetPoint]);
        distToTarget = Math3D_Vec3fDistSq(&this->actor.world.pos, &targetPathPoint);
        if (distToTarget < SQ(80.0f)) {
            this->actor.speed *= 0.85f;
        } else if (distToTarget < SQ(150.0f)) {
            this->actor.speed *= 0.93f;
        } else if (distToTarget < SQ(250.0f)) {
            this->actor.speed *= 0.96f;
        }

        if ((this->currentPoint == this->dogTargetPoint) || (distToTarget < SQ(50.0f))) {
            this->timer = 0;
        }
    }

    EnInvadepoh_Dog_PlayWalkSfx(this);

    if (EnInvadepoh_Dog_MoveAlongPath(this, play)) {
        EnInvadepoh_Dog_SetPathPointToNext(this);
    }

    if (this->timer > 0) {
        this->timer--;
    } else if (this->dogTargetPoint >= 0) {
        // Assuming no alien is within 340 units of the barn, the dog has a 40% chance of growling before barking.
        if (!sAliensExtremeThreat && (Rand_ZeroOne() < 0.4f)) {
            Actor_PlaySfx(&this->actor, NA_SE_EV_SMALL_DOG_GROAN);
        }

        EnInvadepoh_Dog_SetupBark(this);
    } else {
        EnInvadepoh_Dog_SetupWalk(this);
    }
}

void EnInvadepoh_Dog_SetupBark(EnInvadepoh* this) {
    Animation_MorphToPlayOnce(&this->skelAnime, &gDogBarkAnim, -6.0f);
    this->actionFunc = EnInvadepoh_Dog_Bark;
}

/**
 * Turns the dog to face the closest alien threat, then plays its barking animation before running again.
 */
void EnInvadepoh_Dog_Bark(EnInvadepoh* this, PlayState* play) {
    s32 pad;

    Math_StepToF(&this->actor.speed, 0.0f, 1.0f);
    Math_SmoothStepToS(&this->actor.world.rot.y, Actor_WorldYawTowardActor(&this->actor, &sClosestAlienThreat->actor),
                       5, 0x1388, 0x64);
    EnInvadepoh_Dog_Move(this, play);

    if (Animation_OnFrame(&this->skelAnime, 13.0f) || Animation_OnFrame(&this->skelAnime, 19.0f)) {
        Actor_PlaySfx(&this->actor, NA_SE_EV_SMALL_DOG_ANG_BARK);
    }

    if (this->isAnimFinished) {
        EnInvadepoh_Dog_SetupRun(this);
    }
}

void EnInvadepoh_Dog_SetupJump(EnInvadepoh* this) {
    Animation_MorphToPlayOnce(&this->skelAnime, &gDogJumpAnim, -6.0f);
    this->actionFunc = EnInvadepoh_Dog_Jump;
}

/**
 * Makes the dog play its jump animation to completion before running again.
 */
void EnInvadepoh_Dog_Jump(EnInvadepoh* this, PlayState* play) {
    Math_StepToF(&this->actor.speed, 0.5f, 1.0f);
    EnInvadepoh_Dog_Move(this, play);

    if (this->isAnimFinished) {
        EnInvadepoh_Dog_SetupRun(this);
    }
}

void EnInvadepoh_Dog_WaitForObject(Actor* thisx, PlayState* play2) {
    PlayState* play = play2;
    EnInvadepoh* this = (EnInvadepoh*)thisx;

    if (!Object_IsLoaded(&play->objectCtx, this->objectSlot)) {
        return;
    }

    this->actor.objectSlot = this->objectSlot;
    Actor_SetObjectDependency(play, &this->actor);
    SkelAnime_InitFlex(play, &this->skelAnime, &gDogSkel, &gDogWalkAnim, this->jointTable, this->morphTable,
                       DOG_LIMB_MAX);
    EnInvadepoh_ModelInfo_Init(&this->modelInfo, NULL, 0, NULL, 0, &gZeroVec3s, 0xBB8, 0.1f, 0.0f, 0.0f);
    EnInvadepoh_Dog_InitPath(this, play);
    EnInvadepoh_SetPosToPathPoint(this, 0);
    func_800B4AEC(play, &this->actor, 50.0f);
    EnInvadepoh_SnapToFloor(this);
    Math_Vec3f_Copy(&this->currentPos, &this->actor.world.pos);

    if (sInvasionState == INVASION_STATE_ACTIVE) {
        this->actor.update = EnInvadepoh_Dog_Update;
        this->actor.draw = EnInvadepoh_Dog_Draw;
        this->actor.flags |= ACTOR_FLAG_ATTENTION_ENABLED;
        EnInvadepoh_Dog_SetupWalk(this);
    } else if (sInvasionState == INVASION_STATE_WAIT) {
        this->actor.update = EnInvadepoh_Dog_WaitForInvasion;
    } else {
        Actor_Kill(&this->actor);
    }
}

/**
 * Waits for the invasion to start by checking the `sInvasionState`. If the invasion hasn't started yet, then the dog
 * will be invisble. Once it starts, the dog becomes visible and starts walking.
 */
void EnInvadepoh_Dog_WaitForInvasion(Actor* thisx, PlayState* play2) {
    PlayState* play = play2;
    EnInvadepoh* this = (EnInvadepoh*)thisx;

    if (sInvasionState == INVASION_STATE_ACTIVE) {
        this->actor.update = EnInvadepoh_Dog_Update;
        this->actor.draw = EnInvadepoh_Dog_Draw;
        this->actor.flags |= ACTOR_FLAG_ATTENTION_ENABLED;
        EnInvadepoh_Dog_SetupWalk(this);
    }
}

void EnInvadepoh_Dog_Update(Actor* thisx, PlayState* play2) {
    PlayState* play = play2;
    EnInvadepoh* this = (EnInvadepoh*)thisx;
    s32 inUncullRange = CHECK_FLAG_ALL(this->actor.flags, ACTOR_FLAG_INSIDE_CULLING_VOLUME);

    sClosestAlienThreat = EnInvadepoh_Dog_GetClosestAlienThreat();
    if (sClosestAlienThreat == NULL) {
        // To enter this block, there must be no living aliens within 2000 units of the barn. If the dog was previously
        // targeting an alien before this point, then this code will make the dog do a little jump for joy.
        s32 hadTarget = (this->dogTargetPoint >= 0);

        this->dogTargetPoint = -1;
        if (hadTarget) {
            EnInvadepoh_Dog_SetupJump(this);
        }
    } else {
        s32 previousTargetPoint = this->dogTargetPoint;
        s32 counterclockwisePointsToTarget;
        s32 halfPathLength;

        this->dogTargetPoint = EnInvadepoh_Dog_FindClosestPointToTarget(this, &sClosestAlienThreat->actor.world.pos);
        if (previousTargetPoint != this->dogTargetPoint) {
            counterclockwisePointsToTarget = this->dogTargetPoint - this->currentPoint;

            // This assumes that the dog is on a circular path, which is what it uses in the final game. This code, and
            // the code that follows it, is trying to determine if the dog should run counterclockwise (where the dog
            // runs from point N to point N+1) or clockwise (where the dog runs from point N to point N-1) along that
            // circular path. To do that, we need to know how many points the dog must run through to reach the target
            // point while running counterclockwise. If `counterclockwisePointsToTarget` is negative, then we need to
            // adjust it by adding the length of the path. In the final game, the first point and the last point on the
            // dog's path overlap, which means that the length of the path is effectively the same as the index of the
            // end point.
            if (counterclockwisePointsToTarget < 0) {
                counterclockwisePointsToTarget += this->endPoint;
            }

            //! @bug The intent of the code below is to implement the following logic:
            //! - If the dog needs to run through *more than* half the total number of path points in order to run
            //!   counterclockwise, then make the dog run clockwise.
            //! - If the dog needs to run through *fewer than* half the total number of path points in order to run
            //!   counterclockwise, then make the dog counterclockwise.
            //! - If the dog needs to run through *exactly* half of the total number of path points in order to run
            //!   counterclockwise, then don't change the dog's direction; either direction is fine.
            //!
            //! This code mostly works, but it can occasionally get the wrong result for paths with an odd number of
            //! points; the path that the dog runs along in the final game is effectively a 17-point path, so the bug
            //! can manifest in-game. For an odd-numbered length, dividing the length by 2 results in a non-whole
            //! number, so the result is truncated when stored as an integer. If the `counterclockwisePointsToTarget`
            //! happens to exactly equal this truncated value, then this code will fail to recognize that going
            //! counterclockwise is more efficient, and `pathStep` will be left untouched. This can result in the dog
            //! taking a less-efficient clockwise path under certain circumstances.
            halfPathLength = this->endPoint >> 1;
            if (halfPathLength < counterclockwisePointsToTarget) {
                this->pathStep = -1;
            } else if (counterclockwisePointsToTarget < halfPathLength) {
                this->pathStep = 1;
            }
        }
    }

    this->actionFunc(this, play);

    this->isAnimFinished = SkelAnime_Update(&this->skelAnime);

    if (inUncullRange && (this->actor.update != NULL)) {
        EnInvadepoh_ModelInfo_Update(&this->modelInfo);
        Collider_UpdateCylinder(&this->actor, &this->collider);
        CollisionCheck_SetOC(play, &play->colChkCtx, &this->collider.base);
    }
}

void EnInvadepoh_Night3Cremia_SetupWalk(EnInvadepoh* this) {
    EnInvadepohModelInfo* modelInfo = &this->modelInfo;

    Animation_MorphToLoop(&this->skelAnime, &gCremiaWalkAnim, -6.0f);
    modelInfo->headRotTarget.x = 0;
    modelInfo->headRotTarget.y = 0;
    modelInfo->headRotTarget.z = 0;
    modelInfo->headRotStepScale = 0.1f;
    modelInfo->headRotMaxStep = 0x320;
    this->shapeAngularVelocityY = 0;
    this->actionFunc = EnInvadepoh_Night3Cremia_Walk;
}

/**
 * Walks from the barn to the house by maintaining a set distance from Romani. If Romani is currently talking, then
 * Cremia will become idle. After 8:15 PM, this function will kill the actor instance.
 */
void EnInvadepoh_Night3Cremia_Walk(EnInvadepoh* this, PlayState* play) {
    s32 pad;
    EnInvadepoh* romani = sNight3Romani;
    EnInvadepohModelInfo* modelInfo = &this->modelInfo;
    s32 tempFrames;
    s16 pitch;
    s16 yaw;
    s16 angleToRomani;
    s16 targetYaw;

    if (romani != NULL) {
        if (romani->currentPoint == 0) {
            // Romani is opening the door to the barn
            this->distanceToRomani = 40.0f;
            this->angle = -0x8000;
            this->actor.flags &= ~(ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_FRIENDLY);
        } else if (romani->currentPoint < (romani->endPoint - 1)) {
            this->distanceToRomani = 40.0f;
            Math_ScaledStepToS(&this->angle, -0x4800, 0xC8);
            this->actor.flags |= (ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_FRIENDLY);
        } else {
            // Romani is opening the door to the house
            Math_StepToF(&this->distanceToRomani, 5.0f, 3.0f);
            Math_ScaledStepToS(&this->angle, -0x8000, 0x12C);
            this->actor.flags &= ~(ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_FRIENDLY);
        }

        angleToRomani = this->angle + romani->actor.world.rot.y;
        this->actor.world.pos.x = (Math_SinS(angleToRomani) * this->distanceToRomani) + romani->actor.world.pos.x;
        this->actor.world.pos.y = romani->actor.world.pos.y;
        this->actor.world.pos.z = (Math_CosS(angleToRomani) * this->distanceToRomani) + romani->actor.world.pos.z;
        func_800B4AEC(play, &this->actor, 50.0f);
        EnInvadepoh_SnapToFloor(this);
        Math_StepToS(&this->shapeAngularVelocityY, 0xBB8, 0x1F5);

        if (Math3D_Vec3fDistSq(&this->actor.prevPos, &this->actor.world.pos) > SQ(0.01f)) {
            Math_SmoothStepToS(&this->actor.shape.rot.y, Math_Vec3f_Yaw(&this->actor.prevPos, &this->actor.world.pos),
                               3, this->shapeAngularVelocityY, 0x1F4);
        }

        tempFrames = (play->gameplayFrames + 20) % 0x80;

        if (tempFrames & 0x40) {
            targetYaw = Math_Vec3f_Yaw(&this->actor.world.pos, &romani->actor.world.pos);

            if (tempFrames == 0x40) {
                this->shapeAngularVelocityY = 0;
            }

            Math_StepToS(&this->shapeAngularVelocityY, 0x7D0, 0x28);
            Math_SmoothStepToS(&this->actor.shape.rot.y, targetYaw, 6, this->shapeAngularVelocityY, 0x28);

            pitch = (s16)(Math_Vec3f_Pitch(&this->actor.focus.pos, &romani->actor.focus.pos) * 0.85f) -
                    this->actor.shape.rot.x;
            modelInfo->headRotTarget.x = CLAMP(pitch, -0xBB8, 0xBB8);

            yaw = targetYaw - this->actor.shape.rot.y;
            modelInfo->headRotTarget.y = CLAMP((s16)(yaw * 0.7f), -0x1F40, 0x1F40);
        }
    }

    if (CHECK_FLAG_ALL(this->actor.flags, ACTOR_FLAG_INSIDE_CULLING_VOLUME) &&
        (Animation_OnFrame(&this->skelAnime, 0.0f) || Animation_OnFrame(&this->skelAnime, 12.0f))) {
        Actor_PlaySfx(&this->actor, NA_SE_EN_ROMANI_WALK);
    }

    if (CURRENT_TIME > CLOCK_TIME(20, 15)) {
        Actor_Kill(&this->actor);
    } else if ((romani != NULL) && (romani->actionFunc == EnInvadepoh_Night3Romani_Talk)) {
        EnInvadepoh_Night3Cremia_SetupIdle(this);
    }
}

void EnInvadepoh_Night3Cremia_SetupTalk(EnInvadepoh* this) {
    this->modelInfo.headRotStepScale = 0.08f;
    this->modelInfo.headRotMaxStep = 0x7D0;
    Animation_MorphToLoop(&this->skelAnime, &gCremiaIdleAnim, -6.0f);
    this->shapeAngularVelocityY = 0;
    this->actionFunc = EnInvadepoh_Night3Cremia_Talk;
}

/**
 * Rotates Cremia to face the player and handles the conversation she has with them. When she's done talking, Cremia
 * will begin to walk again.
 */
void EnInvadepoh_Night3Cremia_Talk(EnInvadepoh* this, PlayState* play) {
    EnInvadepohModelInfo* modelInfo = &this->modelInfo;
    Player* player = GET_PLAYER(play);
    s16 pitch;
    s16 yaw;

    Math_StepToS(&this->shapeAngularVelocityY, 0x9C4, 0x1C2);
    Math_SmoothStepToS(&this->actor.shape.rot.y, this->actor.yawTowardsPlayer, 5, this->shapeAngularVelocityY, 0x28);

    pitch = (s16)(Math_Vec3f_Pitch(&this->actor.focus.pos, &player->actor.focus.pos) * 0.85f) - this->actor.shape.rot.x;
    modelInfo->headRotTarget.x = CLAMP(pitch, -0xBB8, 0xBB8);

    yaw = this->actor.yawTowardsPlayer - this->actor.shape.rot.y;
    modelInfo->headRotTarget.y = CLAMP((s16)(yaw * 0.7f), -0x1F40, 0x1F40);

    if (Actor_TextboxIsClosing(&this->actor, play)) {
        EnInvadepoh_Night3Cremia_SetupWalk(this);
    }
}

void EnInvadepoh_Night3Cremia_SetupIdle(EnInvadepoh* this) {
    this->modelInfo.headRotStepScale = 0.08f;
    this->modelInfo.headRotMaxStep = 0x7D0;
    Animation_MorphToLoop(&this->skelAnime, &gCremiaIdleAnim, -6.0f);
    this->shapeAngularVelocityY = 0;
    this->actionFunc = EnInvadepoh_Night3Cremia_Idle;
}

/**
 * Rotates Cremia to face the player and waits until Romani is done talking, at which point Cremia will walk again.
 */
void EnInvadepoh_Night3Cremia_Idle(EnInvadepoh* this, PlayState* play) {
    EnInvadepohModelInfo* modelInfo = &this->modelInfo;
    Player* player = GET_PLAYER(play);
    s16 pitch;
    s16 yaw;

    Math_StepToS(&this->shapeAngularVelocityY, 0x7D0, 0xC8);
    Math_SmoothStepToS(&this->actor.shape.rot.y, this->actor.yawTowardsPlayer, 6, this->shapeAngularVelocityY, 0x28);

    pitch = (s16)(Math_Vec3f_Pitch(&this->actor.focus.pos, &player->actor.focus.pos) * 0.85f) - this->actor.shape.rot.x;
    modelInfo->headRotTarget.x = CLAMP(pitch, -0xBB8, 0xBB8);

    yaw = this->actor.yawTowardsPlayer - this->actor.shape.rot.y;
    modelInfo->headRotTarget.y = CLAMP((s16)(yaw * 0.7f), -0x1F40, 0x1F40);

    if (sNight3Romani == NULL) {
        EnInvadepoh_Night3Cremia_SetupWalk(this);
    } else if ((sNight3Romani != NULL) && (sNight3Romani->actionFunc != EnInvadepoh_Night3Romani_Talk)) {
        EnInvadepoh_Night3Cremia_SetupWalk(this);
    }
}

void EnInvadepoh_Night3Cremia_WaitForObject(Actor* thisx, PlayState* play2) {
    PlayState* play = play2;
    EnInvadepoh* this = (EnInvadepoh*)thisx;
    s32 pad;

    if (Object_IsLoaded(&play2->objectCtx, this->objectSlot)) {
        s32 currentTime = CURRENT_TIME;

        this->actor.objectSlot = this->objectSlot;
        Actor_SetObjectDependency(play, &this->actor);
        EnInvadepoh_Cremia_DesegmentTextures();
        SkelAnime_InitFlex(play, &this->skelAnime, &gCremiaSkel, &gCremiaWalkAnim, this->jointTable, this->morphTable,
                           CREMIA_LIMB_MAX);
        EnInvadepoh_ModelInfo_Init(&this->modelInfo, sCremiaEyeAnimations, CREMIA_EYE_ANIM_OPEN_THEN_RANDOM_BLINK,
                                   sCremiaMouthAnimations, CREMIA_MOUTH_ANIM_NORMAL, &gZeroVec3s, 100, 0.03f, 0.3f,
                                   0.03f);
        this->actor.textId = 0x33CD;

        if (currentTime < CLOCK_TIME(20, 01) + 30) {
            this->angle = -0x8000;
            this->distanceToRomani = 40.0f;
        } else if (currentTime > CLOCK_TIME(20, 14) + 1) {
            this->angle = -0x4800;
            this->distanceToRomani = 20.0f;
        } else {
            this->angle = -0x8000;
            this->distanceToRomani = 40.0f;
        }

        if ((currentTime >= CLOCK_TIME(6, 00)) && (currentTime < CLOCK_TIME(20, 00) + 30)) {
            this->actor.update = EnInvadepoh_Night3Cremia_WaitForTime;
            this->actor.draw = NULL;
        } else if ((currentTime >= CLOCK_TIME(20, 00) + 30) && (currentTime < CLOCK_TIME(20, 15))) {
            this->actor.update = EnInvadepoh_Night3Cremia_Update;
            this->actor.draw = EnInvadepoh_Cremia_Draw;
            EnInvadepoh_Night3Cremia_SetupWalk(this);
        } else {
            Actor_Kill(&this->actor);
        }
    }
}

/**
 * Waits until slightly after 8:00 PM, then Cremia becomes visible and starts walking.
 */
void EnInvadepoh_Night3Cremia_WaitForTime(Actor* thisx, PlayState* play2) {
    PlayState* play = play2;
    EnInvadepoh* this = (EnInvadepoh*)thisx;

    if ((CURRENT_TIME >= CLOCK_TIME(20, 00) + 30) && (CURRENT_TIME < CLOCK_TIME(20, 15))) {
        this->actor.update = EnInvadepoh_Night3Cremia_Update;
        this->actor.draw = EnInvadepoh_Cremia_Draw;
        EnInvadepoh_Night3Cremia_SetupWalk(this);
    }
}

void EnInvadepoh_Night3Cremia_Update(Actor* thisx, PlayState* play2) {
    PlayState* play = play2;
    EnInvadepoh* this = (EnInvadepoh*)thisx;
    s32 inUncullRange = CHECK_FLAG_ALL(this->actor.flags, ACTOR_FLAG_INSIDE_CULLING_VOLUME);
    s32 talkAccepted = Actor_TalkOfferAccepted(&this->actor, &play->state);

    if (talkAccepted) {
        Message_BombersNotebookQueueEvent(play, BOMBERS_NOTEBOOK_EVENT_MET_CREMIA);
        EnInvadepoh_Night3Cremia_SetupTalk(this);
    }

    this->actionFunc(this, play2);

    if (inUncullRange && (this->actor.update != NULL)) {
        SkelAnime_Update(&this->skelAnime);
        EnInvadepoh_ModelInfo_Update(&this->modelInfo);

        if ((this->actionFunc != EnInvadepoh_Night3Cremia_Talk) && !talkAccepted && this->actor.isLockedOn) {
            Actor_OfferTalk(&this->actor, play, 350.0f);
        }

        Collider_UpdateCylinder(&this->actor, &this->collider);
        CollisionCheck_SetOC(play, &play->colChkCtx, &this->collider.base);
    }
}

void EnInvadepoh_Night3Romani_SetupWalk(EnInvadepoh* this) {
    EnInvadepohModelInfo* modelInfo = &this->modelInfo;

    Animation_MorphToLoop(&this->skelAnime, &gRomaniWalkAnim, -10.0f);
    modelInfo->headRotTarget.x = 0;
    modelInfo->headRotTarget.y = 0;
    modelInfo->headRotTarget.z = 0;
    modelInfo->headRotStepScale = 0.1f;
    modelInfo->headRotMaxStep = 0x320;
    this->actionFunc = EnInvadepoh_Night3Romani_Walk;
}

/**
 * Walks from the barn to the house by following the path between them. If Cremia is currently talking, then Romani will
 * become idle. Once Romani has reached the end of the path, this function will kill the actor instance.
 */
void EnInvadepoh_Night3Romani_Walk(EnInvadepoh* this, PlayState* play) {
    s32 pad;
    EnInvadepoh* cremia = sNight3Cremia;
    EnInvadepohModelInfo* modelInfo = &this->modelInfo;
    s32 tempFrames;
    s32 pad2;
    s32 currentPoint;
    Vec3f currentPathPointPos;
    Vec3f nextPathPointPos;

    EnInvadepoh_Night3Romani_PathComputeProgress(this);
    EnInvadepoh_Night3Romani_MoveAlongTimePath(this, play);
    EnInvadepoh_Romani_StepYawAlongPath(this, 6, 0x7D0, 0x64);

    currentPoint = (this->currentPoint < this->endPoint) ? this->currentPoint : this->endPoint - 1;

    Math_Vec3s_ToVec3f(&currentPathPointPos, &this->pathPoints[currentPoint]);
    Math_Vec3s_ToVec3f(&nextPathPointPos, &this->pathPoints[currentPoint + 1]);
    Math_SmoothStepToS(&this->actor.world.rot.y, Math_Vec3f_Yaw(&currentPathPointPos, &nextPathPointPos), 5, 0x7D0,
                       100);

    if ((this->currentPoint == 0) || (this->currentPoint == this->endPoint - 1)) {
        if (!this->doorOpened) {
            s32 doorTimer = Environment_GetTimeSpeed(play);

            if (doorTimer > 0) {
                doorTimer = (R_TIME_SPEED * -23.0f / doorTimer) - 0.5f;
                this->doorOpened = EnInvadepoh_Romani_OpenDoor(this, play, SQ(80.0f), doorTimer);
            }
        }

        this->actor.flags &= ~(ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_FRIENDLY);
    } else {
        this->doorOpened = false;
        this->actor.flags |= (ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_FRIENDLY);
    }

    tempFrames = play->gameplayFrames % 0x80;

    if (tempFrames & 0x40) {
        s16 targetYaw = Math_Vec3f_Yaw(&this->actor.world.pos, &cremia->actor.world.pos);
        s16 pitch;
        s16 yaw;

        if (tempFrames == 0x40) {
            this->shapeAngularVelocityY = 0;
        }

        Math_StepToS(&this->shapeAngularVelocityY, 0x7D0, 0x28);
        Math_SmoothStepToS(&this->actor.shape.rot.y, targetYaw, 6, this->shapeAngularVelocityY, 40);

        pitch =
            (s16)(Math_Vec3f_Pitch(&this->actor.focus.pos, &cremia->actor.focus.pos) * 0.85f) - this->actor.shape.rot.x;
        modelInfo->headRotTarget.x = CLAMP(pitch, -0xBB8, 0xBB8);

        yaw = targetYaw - this->actor.shape.rot.y;
        modelInfo->headRotTarget.y = CLAMP((s16)(yaw * 0.7f), -0x1F40, 0x1F40);
    }

    if (CHECK_FLAG_ALL(this->actor.flags, ACTOR_FLAG_INSIDE_CULLING_VOLUME) &&
        (Animation_OnFrame(&this->skelAnime, 0.0f) || Animation_OnFrame(&this->skelAnime, 7.0f))) {
        Actor_PlaySfx(&this->actor, NA_SE_EN_ROMANI_WALK);
    }

    if (this->pathProgress >= 0.9999f) {
        Actor_Kill(&this->actor);
    } else if ((cremia != NULL) && (cremia->actionFunc == EnInvadepoh_Night3Cremia_Talk)) {
        EnInvadepoh_Night3Romani_SetupIdle(this);
    }
}

void EnInvadepoh_Night3Romani_SetupTalk(EnInvadepoh* this) {
    this->modelInfo.headRotStepScale = 0.08f;
    this->modelInfo.headRotMaxStep = 0xFA0;
    Animation_MorphToLoop(&this->skelAnime, &gRomaniIdleAnim, -10.0f);
    this->shapeAngularVelocityY = 0;
    this->actionFunc = EnInvadepoh_Night3Romani_Talk;
}

/**
 * Rotates Romani to face the player and handles the conversation she has with them. When she's done talking, Romani
 * will begin to walk again.
 */
void EnInvadepoh_Night3Romani_Talk(EnInvadepoh* this, PlayState* play) {
    EnInvadepohModelInfo* modelInfo = &this->modelInfo;
    Player* player = GET_PLAYER(play);
    s16 pitch;
    s16 yaw;

    Math_StepToS(&this->shapeAngularVelocityY, 0xBB8, 0x1F4);
    Math_SmoothStepToS(&this->actor.shape.rot.y, this->actor.yawTowardsPlayer, 6, this->shapeAngularVelocityY, 40);

    pitch = (s16)(Math_Vec3f_Pitch(&this->actor.focus.pos, &player->actor.focus.pos) * 0.85f) - this->actor.shape.rot.x;
    modelInfo->headRotTarget.x = CLAMP(pitch, -0xBB8, 0xBB8);

    yaw = this->actor.yawTowardsPlayer - this->actor.shape.rot.y;
    modelInfo->headRotTarget.y = CLAMP((s16)(yaw * 0.7f), -0x1F40, 0x1F40);

    if (Actor_TextboxIsClosing(&this->actor, play)) {
        EnInvadepoh_Night3Romani_SetupWalk(this);
    }
}

void EnInvadepoh_Night3Romani_SetupIdle(EnInvadepoh* this) {
    this->modelInfo.headRotStepScale = 0.08f;
    this->modelInfo.headRotMaxStep = 0x7D0;
    Animation_MorphToLoop(&this->skelAnime, &gRomaniIdleAnim, -10.0f);
    this->shapeAngularVelocityY = 0;
    this->actionFunc = EnInvadepoh_Night3Romani_Idle;
}

/**
 * Rotates Romani to face the player and waits until Cremia is done talking, at which point Romani will walk again.
 */
void EnInvadepoh_Night3Romani_Idle(EnInvadepoh* this, PlayState* play) {
    EnInvadepohModelInfo* modelInfo = &this->modelInfo;
    Player* player = GET_PLAYER(play);
    s16 pitch;
    s16 yaw;

    Math_StepToS(&this->shapeAngularVelocityY, 0x7D0, 0xC8);
    Math_SmoothStepToS(&this->actor.shape.rot.y, this->actor.yawTowardsPlayer, 6, this->shapeAngularVelocityY, 40);

    pitch = (s16)(Math_Vec3f_Pitch(&this->actor.focus.pos, &player->actor.focus.pos) * 0.85f) - this->actor.shape.rot.x;
    modelInfo->headRotTarget.x = CLAMP(pitch, -0xBB8, 0xBB8);

    yaw = this->actor.yawTowardsPlayer - this->actor.shape.rot.y;
    modelInfo->headRotTarget.y = CLAMP((s16)(yaw * 0.7f), -0x1F40, 0x1F40);

    if (sNight3Cremia == NULL) {
        EnInvadepoh_Night3Romani_SetupWalk(this);
    } else if ((sNight3Cremia != NULL) && (sNight3Cremia->actionFunc != EnInvadepoh_Night3Cremia_Talk)) {
        EnInvadepoh_Night3Romani_SetupWalk(this);
    }
}

void EnInvadepoh_Night3Romani_WaitForObject(Actor* thisx, PlayState* play2) {
    PlayState* play = play2;
    EnInvadepoh* this = (EnInvadepoh*)thisx;
    s32 pad;

    if (Object_IsLoaded(&play2->objectCtx, this->objectSlot)) {
        s32 currentTime = CURRENT_TIME;

        this->actor.objectSlot = this->objectSlot;
        Actor_SetObjectDependency(play, &this->actor);
        EnInvadepoh_Romani_DesegmentTextures();
        SkelAnime_InitFlex(play, &this->skelAnime, &gRomaniSkel, &gRomaniWalkAnim, this->jointTable, this->morphTable,
                           ROMANI_LIMB_MAX);
        EnInvadepoh_ModelInfo_Init(&this->modelInfo, sRomaniEyeAnimations, ROMANI_EYE_ANIM_OPEN_THEN_RANDOM_BLINK,
                                   sRomaniMouthAnimations, ROMANI_MOUTH_ANIM_SMILE, &gZeroVec3s, 100, 0.03f, 0.3f,
                                   0.03f);
        EnInvadepoh_Night3Romani_InitPath(this, play);
        this->actor.world.rot.y = this->actor.shape.rot.y;
        EnInvadepoh_Night3Romani_PathComputeProgress(this);
        EnInvadepoh_Night3Romani_MoveAlongTimePath(this, play);
        EnInvadepoh_SetYawAlongPath(this);
        EnInvadepoh_SnapToFloor(this);
        this->actor.textId = 0x33CE;

        if ((currentTime >= CLOCK_TIME(6, 00)) && (currentTime < CLOCK_TIME(20, 00))) {
            this->actor.update = EnInvadepoh_Night3Romani_WaitForTime;
            this->actor.draw = NULL;
        } else if ((currentTime >= CLOCK_TIME(20, 00)) && (currentTime < CLOCK_TIME(20, 14) + 15)) {
            this->actor.update = EnInvadepoh_Night3Romani_Update;
            this->actor.draw = EnInvadepoh_Romani_Draw;
            EnInvadepoh_Night3Romani_SetupWalk(this);
        } else {
            Actor_Kill(&this->actor);
        }
    }
}

/**
 * Waits until slightly after 8:00 PM, then Cremia becomes visible and starts walking.
 */
void EnInvadepoh_Night3Romani_WaitForTime(Actor* thisx, PlayState* play2) {
    PlayState* play = play2;
    EnInvadepoh* this = (EnInvadepoh*)thisx;

    if ((CURRENT_TIME >= CLOCK_TIME(20, 00)) && (CURRENT_TIME < CLOCK_TIME(20, 14) + 15)) {
        this->actor.update = EnInvadepoh_Night3Romani_Update;
        this->actor.draw = EnInvadepoh_Romani_Draw;
        EnInvadepoh_Night3Romani_SetupWalk(this);
    }
}

void EnInvadepoh_Night3Romani_Update(Actor* thisx, PlayState* play2) {
    PlayState* play = play2;
    EnInvadepoh* this = (EnInvadepoh*)thisx;
    s32 inUncullRange = CHECK_FLAG_ALL(this->actor.flags, ACTOR_FLAG_INSIDE_CULLING_VOLUME);
    s32 talkAccepted = Actor_TalkOfferAccepted(&this->actor, &play->state);

    if (talkAccepted) {
        Message_BombersNotebookQueueEvent(play, BOMBERS_NOTEBOOK_EVENT_MET_ROMANI);
        EnInvadepoh_Night3Romani_SetupTalk(this);
    }

    this->actionFunc(this, play);

    if (inUncullRange && (this->actor.update != NULL)) {
        SkelAnime_Update(&this->skelAnime);
        EnInvadepoh_ModelInfo_Update(&this->modelInfo);

        if ((this->actionFunc != EnInvadepoh_Night3Romani_Talk) && !talkAccepted && this->actor.isLockedOn) {
            Actor_OfferTalk(&this->actor, play, 350.0f);
        }

        Collider_UpdateCylinder(&this->actor, &this->collider);
        CollisionCheck_SetOC(play, &play->colChkCtx, &this->collider.base);
    }
}

typedef struct EnInvadepohCowAbductionInfo {
    /* 0x0 */ f32 accel;
    /* 0x4 */ s16 angularVelocity;
    /* 0x6 */ s16 angularAcceleration;
} EnInvadepohCowAbductionInfo; // size = 0x8

void EnInvadepoh_AlienAbductor_SetupAbductCow(EnInvadepoh* this) {
    static EnInvadepohCowAbductionInfo sCowAbductionInfo[EN_INVADEPOH_ALIEN_ABDUCTOR_INDEX_COW_MAX] = {
        { 0.08f, 0x2BC, -0xA }, // EN_INVADEPOH_ALIEN_ABDUCTOR_INDEX_COW_0
        { 0.09f, 0x12C, -0x5 }, // EN_INVADEPOH_ALIEN_ABDUCTOR_INDEX_COW_1
        { 0.05f, 0x190, 0x0 },  // EN_INVADEPOH_ALIEN_ABDUCTOR_INDEX_COW_2
    };
    s32 pad;
    s32 index = EN_INVADEPOH_GET_INDEX(&this->actor);
    s32 pad2;

    Animation_PlayLoop(&this->skelAnime, &gAlienHoldingCowAnim);
    this->skelAnime.curFrame = EN_INVADEPOH_GET_INDEX(&this->actor) * this->skelAnime.endFrame * 0.25f;
    this->alpha = 255;
    this->actor.draw = EnInvadepoh_Alien_Draw;
    this->shouldDraw = true;
    this->shouldDrawDeathFlash = false;
    this->eyeBeamAlpha = 255;
    this->alienAbductorAccelY = sCowAbductionInfo[index].accel;
    this->angle = index * 0x5555;
    this->actor.world.pos.x = Math_SinS(this->angle) * 80.0f + this->actor.home.pos.x;
    this->actor.world.pos.y = this->actor.home.pos.y;
    this->actor.world.pos.z = Math_CosS(this->angle) * 80.0f + this->actor.home.pos.z;
    this->actor.shape.rot.y = this->angle + 0x4000;
    this->shapeAngularVelocityY = sCowAbductionInfo[index].angularVelocity;
    this->shapeAngularAccelerationY = sCowAbductionInfo[index].angularAcceleration;
    this->actionFunc = EnInvadepoh_AlienAbductor_AbductCow;
    this->actor.velocity.y = 0.0f;
}

/**
 * Rises into the air while carrying a cow in its arms. After the alien gets within 5 units of its target y-coordinate
 * (850 units above its spawn position), this function will kill the actor.
 */
void EnInvadepoh_AlienAbductor_AbductCow(EnInvadepoh* this, PlayState* play) {
    Actor* thisx = &this->actor;
    f32 distToTarget;

    if (sUfo == NULL) {
        Actor_Kill(thisx);
        return;
    }

    Math_StepToF(&thisx->velocity.y, 15.0f, this->alienAbductorAccelY);
    distToTarget = Math_SmoothStepToF(&thisx->world.pos.y, thisx->home.pos.y + 850.0f, 0.2f, thisx->velocity.y, 0.01f);
    this->angle += 0x2BC;
    thisx->world.pos.x = Math_SinS(this->angle) * 80.0f + thisx->home.pos.x;
    thisx->world.pos.z = Math_CosS(this->angle) * 80.0f + thisx->home.pos.z;
    this->shapeAngularVelocityY += this->shapeAngularAccelerationY;
    thisx->shape.rot.y += this->shapeAngularVelocityY;

    if (thisx->child != NULL) {
        Actor* cow = thisx->child;

        cow->world.pos.x = thisx->world.pos.x;
        cow->world.pos.y = thisx->world.pos.y - 38.0f;
        cow->world.pos.z = thisx->world.pos.z;
        cow->shape.rot.y = thisx->shape.rot.y;
    }

    if (distToTarget < 5.0f) {
        Actor_Kill(&this->actor);
    }
}

void EnInvadepoh_AlienAbductor_SetupAbductRomani(EnInvadepoh* this) {
    Animation_PlayLoop(&this->skelAnime, &gAlienHoldingCowAnim);
    this->skelAnime.curFrame = EN_INVADEPOH_GET_INDEX(&this->actor) * this->skelAnime.endFrame * 0.25f;
    this->alpha = 255;
    this->actor.draw = NULL;
    this->shouldDraw = true;
    this->shouldDrawDeathFlash = false;
    this->eyeBeamAlpha = 255;
    this->angularVelocity = 0x190;
    this->angle = 0;
    this->timer = 200;
    this->actionFunc = EnInvadepoh_AlienAbductor_AbductRomani;
    this->actor.velocity.y = 0.0f;
}

/**
 * Waits around and draws nothing for 40 frames, then slowly rises into the air while carrying Romani in its arms. After
 * 200 frames have passed, or after the alien has reached its target y-coordinate (850 units above its spawn position),
 * this function will kill the actor.
 */
void EnInvadepoh_AlienAbductor_AbductRomani(EnInvadepoh* this, PlayState* play) {
    Actor* thisx = &this->actor;
    f32 tempAngularVelocity;
    s32 reachedTargetY = false;

    if (this->timer > 0) {
        this->timer--;
    }

    if (this->timer > 160) {
        thisx->draw = NULL;
    } else {
        thisx->draw = EnInvadepoh_Alien_Draw;

        if ((this->timer < 105) && (this->timer >= 100)) {
            thisx->gravity = -1.0f;
            Math_SmoothStepToS(&thisx->shape.rot.x, 0x2000, 8, 0x320, 0x28);
        } else {
            thisx->gravity = 0.7f;
            Math_SmoothStepToS(&thisx->shape.rot.x, 0, 8, 0x320, 0x28);
        }

        thisx->velocity.y += thisx->gravity;
        thisx->velocity.y *= 0.92f;

        if (this->timer > 80) {
            thisx->world.pos.y += thisx->velocity.y;
        } else {
            f32 targetY = thisx->home.pos.y + 850.0f;

            reachedTargetY = Math_StepToF(&thisx->world.pos.y, targetY, fabsf(thisx->velocity.y));
        }

        tempAngularVelocity = (this->angle * -0.06f) + this->angularVelocity;
        tempAngularVelocity *= 0.98f;
        this->angularVelocity = tempAngularVelocity;
        thisx->shape.rot.y += this->angularVelocity;

        if (thisx->child != NULL) {
            Actor* romani = thisx->child;

            romani->world.pos.x = thisx->world.pos.x;
            romani->world.pos.y = thisx->world.pos.y - 30.0f;
            romani->world.pos.z = thisx->world.pos.z;
            romani->shape.rot.y = thisx->shape.rot.y;
        }
    }

    if ((this->timer <= 0) || reachedTargetY) {
        Actor_Kill(&this->actor);
    }
}

void EnInvadepoh_AlienAbductor_WaitForObject(Actor* thisx, PlayState* play2) {
    PlayState* play = play2;
    EnInvadepoh* this = (EnInvadepoh*)thisx;
    s32 index;

    if (Object_IsLoaded(&play->objectCtx, this->objectSlot)) {
        index = EN_INVADEPOH_GET_INDEX(&this->actor);
        this->actor.objectSlot = this->objectSlot;
        Actor_SetObjectDependency(play, &this->actor);
        EnInvadepoh_Alien_DesegmentTexAnims();
        this->actor.update = EnInvadepoh_AlienAbductor_Update;
        SkelAnime_InitFlex(play, &this->skelAnime, &gAlienSkel, &gAlienHoldingCowAnim, this->jointTable,
                           this->morphTable, ALIEN_LIMB_MAX);

        if (index < EN_INVADEPOH_ALIEN_ABDUCTOR_INDEX_COW_MAX) {
            EnInvadepoh_AlienAbductor_SpawnCow(this, play, index);
            EnInvadepoh_AlienAbductor_SetupAbductCow(this);
        } else {
            EnInvadepoh_AlienAbductor_SpawnRomani(this, play);
            EnInvadepoh_AlienAbductor_SetupAbductRomani(this);
        }
    }
}

void EnInvadepoh_AlienAbductor_Update(Actor* thisx, PlayState* play2) {
    PlayState* play = play2;
    EnInvadepoh* this = (EnInvadepoh*)thisx;

    this->actionFunc(this, play);

    if (this->actor.update != NULL) {
        SkelAnime_Update(&this->skelAnime);
        Actor_PlaySfx_Flagged(&this->actor, NA_SE_EN_FOLLOWERS_BEAM_PRE - SFX_FLAG);
    }
}

void EnInvadepoh_DrawWarpEffects(PlayState* play) {
    s32 i;
    EnInvadepohEffect* warpEffect;

    OPEN_DISPS(play->state.gfxCtx);

    Gfx_SetupDL25_Xlu(play->state.gfxCtx);

    for (i = 0, warpEffect = sEffects; i < EFFECT_COUNT; i++, warpEffect++) {
        if (warpEffect->timer > 0) {
            u8 offset = i * 16;
            u32 warpScrollX1 = (play->gameplayFrames + offset) & 0x7F;
            u32 warpScrollY2 = (play->gameplayFrames * -15) & 0xFF;

            Matrix_Translate(warpEffect->pos.x, warpEffect->pos.y, warpEffect->pos.z, MTXMODE_NEW);
            Matrix_Scale(0.1f, 0.1f, 0.1f, MTXMODE_APPLY);
            gDPPipeSync(POLY_XLU_DISP++);
            gDPSetPrimColor(POLY_XLU_DISP++, 0x80, 0x80, 255, 255, 170, warpEffect->alpha);
            gDPSetEnvColor(POLY_XLU_DISP++, 255, 50, 0, 0);
            gSPSegment(POLY_XLU_DISP++, 0x8,
                       Gfx_TwoTexScrollEx(play->state.gfxCtx, G_TX_RENDERTILE, warpScrollX1, 0, 32, 64, 1, 0,
                                          warpScrollY2, 32, 64, 1, 0, 0, -15));
            MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx);
            gSPDisplayList(POLY_XLU_DISP++, gameplay_keep_DL_02E510);
        }
    }

    CLOSE_DISPS(play->state.gfxCtx);
}

void EnInvadepoh_InvasionHandler_Draw(Actor* thisx, PlayState* play) {
    EnInvadepoh_DrawWarpEffects(play);
}

s32 EnInvadepoh_Alien_OverrideLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot,
                                       Actor* thisx, Gfx** gfx) {
    return false;
}

void EnInvadepoh_Alien_PostLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* pos, Actor* thisx, Gfx** gfx) {
    static Vec3f sFocusOffset = { 2000.0f, 1000.0f, 0.0f };
    EnInvadepoh* this = (EnInvadepoh*)thisx;

    if ((limbIndex == ALIEN_LIMB_LEFT_EYE) && (this->eyeBeamAlpha != 0)) {
        Matrix_Push();
        Matrix_RotateZS(-0x53ED, MTXMODE_APPLY);
        Matrix_RotateYS(-0x3830, MTXMODE_APPLY);
        Matrix_Scale(1.0f, 1.0f, 1.5f, MTXMODE_APPLY);
        Matrix_Get(&sInvadepohAlienLeftEyeBeamMtxF);
        Matrix_Pop();
    } else if ((limbIndex == ALIEN_LIMB_RIGHT_EYE) && (this->eyeBeamAlpha != 0)) {
        Matrix_Push();
        Matrix_RotateZS(-0x53ED, MTXMODE_APPLY);
        Matrix_RotateYS(-0x47D0, MTXMODE_APPLY);
        Matrix_Scale(1.0f, 1.0f, 1.5f, MTXMODE_APPLY);
        Matrix_Get(&sInvadepohAlienRightEyeBeamMtxF);
        Matrix_Pop();
    }

    if (limbIndex == ALIEN_LIMB_HEAD) {
        Matrix_MultVec3f(&sFocusOffset, &this->actor.focus.pos);
    }
}

void EnInvadepoh_Alien_Draw(Actor* thisx, PlayState* play2) {
    PlayState* play = play2;
    EnInvadepoh* this = (EnInvadepoh*)thisx;

    OPEN_DISPS(play->state.gfxCtx);

    Gfx_SetupDL25_Xlu(play->state.gfxCtx);
    Matrix_Push();

    if (this->shouldDraw) {
        if (this->alpha == 255) {
            Gfx_SetupDL25_Opa(play->state.gfxCtx);
            AnimatedMat_Draw(play, sAlienEmptyTexAnim);
            Scene_SetRenderModeXlu(play, 0, 1);
            gDPSetEnvColor(POLY_OPA_DISP++, 0, 0, 0, 255);
            POLY_OPA_DISP = SkelAnime_DrawFlex(play, this->skelAnime.skeleton, this->skelAnime.jointTable,
                                               this->skelAnime.dListCount, EnInvadepoh_Alien_OverrideLimbDraw,
                                               EnInvadepoh_Alien_PostLimbDraw, &this->actor, POLY_OPA_DISP);
        } else {
            AnimatedMat_Draw(play, sAlienEmptyTexAnim);
            Scene_SetRenderModeXlu(play, 1, 2);
            gDPSetEnvColor(POLY_XLU_DISP++, 0, 0, 0, this->alpha);
            POLY_XLU_DISP = SkelAnime_DrawFlex(play, this->skelAnime.skeleton, this->skelAnime.jointTable,
                                               this->skelAnime.dListCount, EnInvadepoh_Alien_OverrideLimbDraw,
                                               EnInvadepoh_Alien_PostLimbDraw, &this->actor, POLY_XLU_DISP);
        }

        if (this->eyeBeamAlpha != 0) {
            Gfx* gfx;

            AnimatedMat_Draw(play, sAlienEyeBeamTexAnim);

            OPEN_DISPS(play->state.gfxCtx);

            gfx = POLY_XLU_DISP;

            gDPPipeSync(gfx++);
            gDPSetPrimColor(gfx++, 0, 0xFF, 240, 180, 100, 60);
            gDPSetEnvColor(gfx++, 255, 255, 255, 100.0f / 170.0f * this->eyeBeamAlpha);

            Matrix_Mult(&sInvadepohAlienLeftEyeBeamMtxF, MTXMODE_NEW);
            MATRIX_FINALIZE_AND_LOAD(gfx++, play->state.gfxCtx);
            gSPDisplayList(gfx++, gAlienEyeBeamDL);

            Matrix_Mult(&sInvadepohAlienRightEyeBeamMtxF, MTXMODE_NEW);
            MATRIX_FINALIZE_AND_LOAD(gfx++, play->state.gfxCtx);
            gSPDisplayList(gfx++, gAlienEyeBeamDL);

            POLY_XLU_DISP = gfx;

            CLOSE_DISPS(play->state.gfxCtx);
        }
    }

    if (this->shouldDrawDeathFlash) {
        Matrix_SetTranslateRotateYXZ(this->actor.world.pos.x, this->actor.world.pos.y + 68.0f, this->actor.world.pos.z,
                                     &this->actor.shape.rot);
        Matrix_Scale(this->deathFlashScale.x, this->deathFlashScale.y, this->deathFlashScale.z, MTXMODE_APPLY);
        MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx);
        gSPDisplayList(POLY_XLU_DISP++, gAlienDeathFlashDL);
    }

    if (this->shouldDraw) {
        Gfx* gfx;
        Vec3f glowOffset;
        Vec3f glowPos;
        s32 glowAlpha;

        OPEN_DISPS(play->state.gfxCtx);

        gfx = POLY_XLU_DISP;
        gfx = Gfx_SetupDL20_NoCD(gfx);

        gDPSetDither(gfx++, G_CD_NOISE);
        gDPSetCombineLERP(gfx++, 0, 0, 0, PRIMITIVE, TEXEL0, 0, PRIMITIVE, 0, 0, 0, 0, PRIMITIVE, TEXEL0, 0, PRIMITIVE,
                          0);

        Matrix_Mult(&play->billboardMtxF, MTXMODE_NEW);
        Matrix_MultVecZ(60.0f, &glowOffset);

        glowPos.x = this->actor.world.pos.x + glowOffset.x;
        glowPos.y = this->actor.world.pos.y + glowOffset.y + 68.0f;
        glowPos.z = this->actor.world.pos.z + glowOffset.z;

        Matrix_Translate(glowPos.x, glowPos.y, glowPos.z, MTXMODE_NEW);
        Matrix_Scale(0.25f, 0.25f, 0.25f, MTXMODE_APPLY);

        glowAlpha = 100.0f / 255.0f * this->alpha;

        gSPDisplayList(gfx++, gameplay_keep_DL_029CB0);
        gDPSetPrimColor(gfx++, 0, 0, 240, 180, 100, glowAlpha);
        MATRIX_FINALIZE_AND_LOAD(gfx++, play->state.gfxCtx);
        gSPDisplayList(gfx++, gameplay_keep_DL_029CF0);

        POLY_XLU_DISP = gfx;

        if ((this->alpha > 128) && EnInvadepoh_LensFlareCheck(play, &glowPos)) {
            Environment_DrawLensFlare(play, &play->envCtx, &play->view, play->state.gfxCtx, glowPos, 10.0f, 9.0f, 0, 0);
        }

        CLOSE_DISPS(play->state.gfxCtx);
    }
    Matrix_Pop();

    CLOSE_DISPS(play->state.gfxCtx);
}

s32 EnInvadepoh_Cow_OverrideLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot,
                                     Actor* thisx) {
    if (limbIndex == COW_LIMB_NOSE_RING) {
        EnInvadepoh* this = (EnInvadepoh*)thisx;

        rot->x -= this->actor.shape.rot.x;
    }

    return false;
}

void EnInvadepoh_Cow_Draw(Actor* thisx, PlayState* play2) {
    PlayState* play = play2;
    EnInvadepoh* this = (EnInvadepoh*)thisx;

    Gfx_SetupDL37_Opa(play->state.gfxCtx);
    SkelAnime_DrawFlexOpa(play, this->skelAnime.skeleton, this->skelAnime.jointTable, this->skelAnime.dListCount,
                          EnInvadepoh_Cow_OverrideLimbDraw, NULL, &this->actor);
}

void EnInvadepoh_CowTail_Draw(Actor* thisx, PlayState* play2) {
    PlayState* play = play2;
    EnInvadepoh* this = (EnInvadepoh*)thisx;

    Gfx_SetupDL37_Opa(play->state.gfxCtx);
    SkelAnime_DrawFlexOpa(play, this->skelAnime.skeleton, this->skelAnime.jointTable, this->skelAnime.dListCount, NULL,
                          NULL, &this->actor);
}

s32 EnInvadepoh_Romani_OverrideLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot,
                                        Actor* thisx) {
    if (limbIndex == ROMANI_LIMB_HEAD) {
        EnInvadepoh* this = (EnInvadepoh*)thisx;

        rot->x += this->modelInfo.headRot.y;
        rot->y += this->modelInfo.headRot.z;
        rot->z += this->modelInfo.headRot.x;
    } else if (limbIndex == ROMANI_LIMB_TORSO) {
        EnInvadepoh* this = (EnInvadepoh*)thisx;

        rot->x += (s16)(this->modelInfo.torsoRotScaleY * this->modelInfo.headRot.y);
        rot->z += this->modelInfo.torsoRotX;
    }

    return false;
}

void EnInvadepoh_Romani_PostLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* pos, Actor* thisx) {
    static Vec3f sFocusOffset = { 400.0f, 270.0f, 0.0f };
    EnInvadepoh* this = (EnInvadepoh*)thisx;

    if (limbIndex == ROMANI_LIMB_LEFT_HAND) {
        OPEN_DISPS(play->state.gfxCtx);

        gSPDisplayList(POLY_OPA_DISP++, gRomaniBowDL);

        CLOSE_DISPS(play->state.gfxCtx);
    } else if (limbIndex == ROMANI_LIMB_HEAD) {
        Matrix_MultVec3f(&sFocusOffset, &this->actor.focus.pos);
    }
}

void EnInvadepoh_Romani_Draw(Actor* thisx, PlayState* play) {
    EnInvadepoh* this = (EnInvadepoh*)thisx;

    OPEN_DISPS(play->state.gfxCtx);

    Gfx_SetupDL25_Opa(play->state.gfxCtx);
    gSPSegment(POLY_OPA_DISP++, 0x09, sRomaniMouthTextures[this->modelInfo.mouthAnim.curTexIndex]);
    gSPSegment(POLY_OPA_DISP++, 0x08, sRomaniEyeTextures[this->modelInfo.eyeAnim.curTexIndex]);
    SkelAnime_DrawFlexOpa(play, this->skelAnime.skeleton, this->skelAnime.jointTable, this->skelAnime.dListCount,
                          EnInvadepoh_Romani_OverrideLimbDraw, EnInvadepoh_Romani_PostLimbDraw, &this->actor);

    CLOSE_DISPS(play->state.gfxCtx);
}

void EnInvadepoh_Ufo_Draw(Actor* thisx, PlayState* play2) {
    PlayState* play = play2;
    EnInvadepoh* this = (EnInvadepoh*)thisx;
    Vec3f flashPos;

    Matrix_Push();
    Matrix_Mult(&play->billboardMtxF, MTXMODE_NEW);
    Matrix_MultVecZ(200.0f, &flashPos);
    Matrix_Pop();

    flashPos.x += this->actor.world.pos.x;
    flashPos.y += this->actor.world.pos.y;
    flashPos.z += this->actor.world.pos.z;

    EnInvadepoh_Ufo_SetMatrixTranslation(&flashPos);
    Matrix_ReplaceRotation(&play->billboardMtxF);
    Matrix_RotateZS(this->angle, MTXMODE_APPLY);

    OPEN_DISPS(play->state.gfxCtx);

    Gfx_SetupDL25_Xlu(play->state.gfxCtx);
    MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx);
    gDPSetPrimColor(POLY_XLU_DISP++, 0xFF, 0x80, 255, 255, 0, 180);
    gDPSetEnvColor(POLY_XLU_DISP++, 255, 50, 0, 0);
    gSPDisplayList(POLY_XLU_DISP++, gEffFlash1DL);

    if (EnInvadepoh_LensFlareCheck(play, &flashPos)) {
        Environment_DrawLensFlare(play, &play->envCtx, &play->view, play->state.gfxCtx, flashPos, 20.0f, 9.0f, 0,
                                  false);
    }

    CLOSE_DISPS(play->state.gfxCtx);
}

s32 EnInvadepoh_Dog_OverrideLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot,
                                     Actor* thisx) {
    if ((limbIndex == DOG_LIMB_HEAD) || (limbIndex == DOG_LIMB_RIGHT_FACE_HAIR) ||
        (limbIndex == DOG_LIMB_LEFT_FACE_HAIR)) {
        EnInvadepoh* this = (EnInvadepoh*)thisx;

        rot->x += this->modelInfo.headRot.x;
        rot->y += this->modelInfo.headRot.y;
        rot->z += this->modelInfo.headRot.z;
    }

    return false;
}

void EnInvadepoh_Dog_PostLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* pos, Actor* thisx) {
    if (limbIndex == DOG_LIMB_HEAD) {
        EnInvadepoh* this = (EnInvadepoh*)thisx;

        Matrix_MultVecY(20.0f, &this->actor.focus.pos);
    }
}

void EnInvadepoh_Dog_Draw(Actor* thisx, PlayState* play) {
    EnInvadepoh* this = (EnInvadepoh*)thisx;

    OPEN_DISPS(play->state.gfxCtx);

    Gfx_SetupDL25_Opa(play->state.gfxCtx);
    gDPSetEnvColor(POLY_OPA_DISP++, 255, 255, 200, 0);
    SkelAnime_DrawFlexOpa(play, this->skelAnime.skeleton, this->skelAnime.jointTable, this->skelAnime.dListCount,
                          EnInvadepoh_Dog_OverrideLimbDraw, EnInvadepoh_Dog_PostLimbDraw, &this->actor);

    CLOSE_DISPS(play->state.gfxCtx);
}

s32 EnInvadepoh_Cremia_OverrideLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot,
                                        Actor* thisx) {
    if (limbIndex == CREMIA_LIMB_HEAD) {
        EnInvadepoh* this = (EnInvadepoh*)thisx;

        rot->x += this->modelInfo.headRot.y;
        rot->y += this->modelInfo.headRot.z;
        rot->z += this->modelInfo.headRot.x;
    } else if (limbIndex == CREMIA_LIMB_TORSO) {
        EnInvadepoh* this = (EnInvadepoh*)thisx;

        rot->x += (s16)(this->modelInfo.torsoRotScaleY * this->modelInfo.headRot.y);
    }

    return false;
}

void EnInvadepoh_Cremia_PostLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* pos, Actor* thisx) {
    if (limbIndex == CREMIA_LIMB_HEAD) {
        EnInvadepoh* this = (EnInvadepoh*)thisx;

        Matrix_MultZero(&this->actor.focus.pos);
    }
}

void EnInvadepoh_Cremia_Draw(Actor* thisx, PlayState* play) {
    EnInvadepoh* this = (EnInvadepoh*)thisx;

    OPEN_DISPS(play->state.gfxCtx);

    Gfx_SetupDL25_Opa(play->state.gfxCtx);
    gSPSegment(POLY_OPA_DISP++, 0x09, sCremiaMouthTextures[this->modelInfo.mouthAnim.curTexIndex]);
    gSPSegment(POLY_OPA_DISP++, 0x08, sCremiaEyeTextures[this->modelInfo.eyeAnim.curTexIndex]);
    SkelAnime_DrawFlexOpa(play, this->skelAnime.skeleton, this->skelAnime.jointTable, this->skelAnime.dListCount,
                          EnInvadepoh_Cremia_OverrideLimbDraw, EnInvadepoh_Cremia_PostLimbDraw, &this->actor);

    CLOSE_DISPS(play->state.gfxCtx);
}

void EnInvadepoh_Reset() {
    sInvasionState = INVASION_STATE_NONE;
    sRewardFinished = false;
    sAliensExtremeThreat = false;
    for (int i = 0; i < ALIEN_COUNT; i++) {
        sAliens[i] = NULL;
        sAlienStateFlags[i] = 0;
    }
}
