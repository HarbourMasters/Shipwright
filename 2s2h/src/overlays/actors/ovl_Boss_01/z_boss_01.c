/*
 * File: z_boss_01.c
 * Overlay: ovl_Boss_01
 * Description: Odolwa, his bugs, and his afterimages.
 *
 * In addition to handling Odolwa, this actor is also responsible for handling the bugs that Odolwa can spawn as well as
 * the afterimages that appear when Odolwa jumps or does a spin attack. The bugs are effectively their own actor with
 * their own Update and Draw functions and a whole dedicated section of the file specifically for them. Afterimages are
 * much simpler, and their code is more interleaved with Odolwa's code. This actor also handles the falling block and
 * ring of fire effects that Odolwa can summon.
 *
 * Odolwa's behavior can primarily be divided into two states: waiting and attacking. He has a variety of wait actions
 * and attack types that he can select between, and this can make it difficult to tell when he is waiting and when he is
 * attacking. Two of his wait actions look like attacks, and when selecting an attack, he can randomly choose to dance
 * for a bit before attacking. Additionally, Odolwa can attack the player via more indirect means while waiting by
 * summoning bugs, by dropping falling blocks on them, or by summoning a ring of fire to surround them.
 *
 * Outside of waiting and attacking, most of Odolwa's behavior involves either dodging attacks (e.g., by jumping,
 * running around, or guarding) or reacting to attacks. He also handles all his cutscenes (his intro cutscene, the
 * cutscene where he summons bugs, and his death cutscene) manually via actionFuncs.
 *
 * Odolwa's fight can be divided roughly into two phases, but unlike most bosses, the second phase does not happen when
 * his health reaches a certain point. Rather, the second phase starts when 1000 or more frames have passed since the
 * fight began. At this point, he will play the bug summoning cutscene, and more wait actions will be available for him
 * to randomly select; these new wait actions are what allow him to summon a ring of fire, drop falling blocks, etc.
 */

#include "z_boss_01.h"
#include "z64rumble.h"
#include "z64shrink_window.h"
#include "assets/objects/gameplay_keep/gameplay_keep.h"
#include "overlays/actors/ovl_Door_Warp1/z_door_warp1.h"
#include "overlays/actors/ovl_En_Clear_Tag/z_en_clear_tag.h"
#include "overlays/actors/ovl_En_Tanron1/z_en_tanron1.h"
#include "overlays/actors/ovl_Item_B_Heart/z_item_b_heart.h"

#include "2s2h/BenPort.h"
#include "2s2h/GameInteractor/GameInteractor.h"

#define FLAGS                                                                                 \
    (ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_HOSTILE | ACTOR_FLAG_UPDATE_CULLING_DISABLED | \
     ACTOR_FLAG_DRAW_CULLING_DISABLED)

#define ODOLWA_EFFECT_COUNT 100

// This actor has an array of timers in its instance, but it only ever uses the first entry.
#define TIMER_CURRENT_ACTION 0
#define TIMER_BUG_CURRENT_ACTION 0
#define TIMER_AFTERIMAGE_DESPAWN 0

void Boss01_Init(Actor* thisx, PlayState* play);
void Boss01_Destroy(Actor* thisx, PlayState* play);
void Boss01_Update(Actor* thisx, PlayState* play2);
void Boss01_Draw(Actor* thisx, PlayState* play);
void Boss01_Reset(void);

void Boss01_SetupIntroCutscene(Boss01* this, PlayState* play);
void Boss01_IntroCutscene(Boss01* this, PlayState* play);
void Boss01_SummonBugsCutscene(Boss01* this, PlayState* play);
void Boss01_Afterimage_SetupWaitToDespawn(Boss01* this, PlayState* play);
void Boss01_Afterimage_WaitToDespawn(Boss01* this, PlayState* play);
void Boss01_SetupWait(Boss01* this, PlayState* play, u8 waitType);
void Boss01_Wait(Boss01* this, PlayState* play);
void Boss01_Dazed(Boss01* this, PlayState* play);
void Boss01_SetupSpinAttack(Boss01* this, PlayState* play);
void Boss01_SpinAttack(Boss01* this, PlayState* play);
void Boss01_SetupDanceBeforeAttack(Boss01* this, PlayState* play);
void Boss01_DanceBeforeAttack(Boss01* this, PlayState* play);
void Boss01_SetupRun(Boss01* this, PlayState* play);
void Boss01_Run(Boss01* this, PlayState* play);
void Boss01_SetupJump(Boss01* this, PlayState* play, u8 shouldPerformFallingSlash);
void Boss01_JumpSquat(Boss01* this, PlayState* play);
void Boss01_Jump(Boss01* this, PlayState* play);
void Boss01_JumpLand(Boss01* this, PlayState* play);
void Boss01_SetupVerticalSlash(Boss01* this, PlayState* play);
void Boss01_VerticalSlash(Boss01* this, PlayState* play);
void Boss01_SetupHorizontalSlash(Boss01* this, PlayState* play);
void Boss01_HorizontalSlash(Boss01* this, PlayState* play);
void Boss01_Guard(Boss01* this, PlayState* play);
void Boss01_SetupKick(Boss01* this, PlayState* play);
void Boss01_Kick(Boss01* this, PlayState* play);
void Boss01_SetupShieldBash(Boss01* this, PlayState* play);
void Boss01_ShieldBash(Boss01* this, PlayState* play);
void Boss01_Damaged(Boss01* this, PlayState* play);
void Boss01_SetupSummonMoths(Boss01* this, PlayState* play);
void Boss01_SummonMoths(Boss01* this, PlayState* play);
void Boss01_SetupDeathCutscene(Boss01* this, PlayState* play);
void Boss01_DeathCutscene(Boss01* this, PlayState* play);
void Boss01_SetupStunned(Boss01* this, PlayState* play);
void Boss01_Stunned(Boss01* this, PlayState* play);
void Boss01_Afterimage_Draw(Actor* thisx, PlayState* play);
void Boss01_GenShadowTex(u8* tex, Boss01* this, PlayState* play);
void Boss01_DrawShadowTex(u8* tex, Boss01* this, PlayState* play);
void Boss01_Bug_SetupCrawl(Boss01* this, PlayState* play);
void Boss01_Bug_Crawl(Boss01* this, PlayState* play);
void Boss01_Bug_Damaged(Boss01* this, PlayState* play);
void Boss01_Bug_SetupDead(Boss01* this, PlayState* play);
void Boss01_Bug_Stunned(Boss01* this, PlayState* play);
void Boss01_Bug_Dead(Boss01* this, PlayState* play);
void Boss01_Bug_Update(Actor* thisx, PlayState* play);
void Boss01_Bug_Draw(Actor* thisx, PlayState* play);
void Boss01_UpdateEffects(Boss01* this, PlayState* play);
void Boss01_DrawEffects(PlayState* play);

typedef enum {
    /* 0 */ ODOLWA_EFFECT_NONE,
    /* 1 */ ODOLWA_EFFECT_FALLING_BLOCK,   // The blocks that fall from the ceiling and their fragments
    /* 3 */ ODOLWA_EFFECT_RING_OF_FIRE = 3 // The ring of fire that surrounds the player
} OdolwaEffectType;

typedef enum {
    /*   0 */ ODOLWA_WAIT_READY,
    /*   1 */ ODOLWA_WAIT_SPIN_SWORD,
    /*   2 */ ODOLWA_WAIT_VERTICAL_HOP,
    /*   3 */ ODOLWA_WAIT_SHAKE_DANCE,
    /*   4 */ ODOLWA_WAIT_UP_AND_DOWN_DANCE,
    /*   5 */ ODOLWA_WAIT_ARM_SWING_DANCE,
    /*   6 */ ODOLWA_WAIT_THRUST_ATTACK,
    /*   7 */ ODOLWA_WAIT_DOUBLE_SLASH,
    /*   8 */ ODOLWA_WAIT_SIDE_TO_SIDE_HOP,
    /*   9 */ ODOLWA_WAIT_SIDE_TO_SIDE_DANCE,
    /*  10 */ ODOLWA_WAIT_SPIN_DANCE,
    /*  11 */ ODOLWA_WAIT_JUMP_DANCE,
    /*  12 */ ODOLWA_WAIT_MAX,

    // This doesn't correspond to an actual wait action that Odolwa can perform, but it can be passed as a parameter to
    // Boss01_SetupWait to randomly select between one of Odolwa's available wait types (as well as having a random
    // chance to summon moths instead), assuming that the fight is in its second phase.
    /* 100 */ ODOLWA_WAIT_RANDOM = 100
} OdolwaWaitType;

typedef enum {
    // Waits for the player to walk forward, then starts the cutscene and transitions to the next state immediately.
    // This state is also used to indiciate the cutscene ended.
    /* 0 */ ODOLWA_INTRO_CS_STATE_WAITING_FOR_PLAYER_OR_DONE,

    // Points the camera directly at the player, plays some of Odolwa's SFXs, and waits for 120 frames.
    /* 1 */ ODOLWA_INTRO_CS_STATE_LOOK_AT_PLAYER,

    // Pans up to watch Odolwa fall from the ceiling. Transitions to the next state once he's close to the ground.
    /* 2 */ ODOLWA_INTRO_CS_STATE_ODOLWA_FALLING,

    // Plays Odolwa's intro animation and shows his title card. Ends the cutscene after 140 frames.
    /* 3 */ ODOLWA_INTRO_CS_STATE_ODOLWA_LANDED
} OdolwaIntroCsState;

typedef enum {
    // Starts the cutscene and transitions to the next state immediately.
    /* 0 */ ODOLWA_BUG_SUMMONING_CS_STATE_STARTED,

    // This state lasts for the entire duration of the cutscene, and cutsceneState is left in this state when it ends.
    /* 1 */ ODOLWA_BUG_SUMMONING_CS_STATE_PLAYING_OR_DONE
} OdolwaBugSummoningCsState;

typedef enum {
    // Starts the cutscene and transitions to the next state immediately.
    /* 0 */ ODOLWA_DEATH_CS_STATE_STARTED,

    // Plays Odolwa's death animation and makes him fall face-first towards the ground. Ends after 71 frames.
    /* 1 */ ODOLWA_DEATH_CS_STATE_PLAY_ANIM_AND_FALL_FORWARD,

    // Surrounds Odolwa with fire and slowly shrinks him. After 180 frames, spawns the blue warp and ends the cutscene.
    /* 2 */ ODOLWA_DEATH_CS_STATE_BURST_INTO_FLAMES_AND_SHRINK,

    // Signals that the cutscene has ended. Also teleports Odolwa far above the ceiling.
    /* 3 */ ODOLWA_DEATH_CS_STATE_DONE
} OdolwaDeathCsState;

typedef enum {
    /*  0 */ ODOLWA_DRAW_DMGEFF_STATE_NONE,
    /*  1 */ ODOLWA_DRAW_DMGEFF_STATE_FIRE_INIT,
    /*  2 */ ODOLWA_DRAW_DMGEFF_STATE_FIRE_ACTIVE,
    /* 10 */ ODOLWA_DRAW_DMGEFF_STATE_FROZEN_INIT = 10,
    /* 11 */ ODOLWA_DRAW_DMGEFF_STATE_FROZEN_ACTIVE,
    /* 20 */ ODOLWA_DRAW_DMGEFF_STATE_LIGHT_ORB_INIT = 20,
    /* 21 */ ODOLWA_DRAW_DMGEFF_STATE_LIGHT_ORB_ACTIVE,
    /* 30 */ ODOLWA_DRAW_DMGEFF_STATE_BLUE_LIGHT_ORB_INIT = 30,
    /* 40 */ ODOLWA_DRAW_DMGEFF_STATE_ELECTRIC_SPARKS_INIT = 40,
    /* 41 */ ODOLWA_DRAW_DMGEFF_STATE_ELECTRIC_SPARKS_ACTIVE
} OdolwaDrawDmgEffState;

typedef enum {
    /* 0 */ ODOLWA_SHADOW_SIZE_MEDIUM,
    /* 1 */ ODOLWA_SHADOW_SIZE_LARGE,
    /* 2 */ ODOLWA_SHADOW_SIZE_EXTRA_LARGE,
    /* 3 */ ODOLWA_SHADOW_SIZE_SMALL
} OdolwaShadowSize;

typedef enum {
    // There are no AT colliders enabled; the player can pass through the sword without reacting or taking any damage.
    /* 0 */ ODOLWA_SWORD_STATE_INACTIVE,

    // The sword's two AT colliders are enabled; the player will be knocked back and take damage if they touch the
    // sword. There is a third collider originating from Odolwa's pelvis that is also active, but it is offset very far
    // out-of-bounds in this state, so the player can never touch it.
    /* 1 */ ODOLWA_SWORD_STATE_ACTIVE,

    // Similar to the previous state, but the pelvis collider is now placed on the floor in front of Odolwa.
    /* 2 */ ODOLWA_SWORD_STATE_HORIZONTAL_SLASH
} OdolwaSwordState;

/**
 * When falling blocks spawn, the sound effect of them falling always plays from this position, regardless of where they
 * actually spawn.
 */
static Vec3f sFallingBlockSfxPos = { 0.0f, 1000.0f, 0.0f };

/**
 * Odolwa's sword trail is a circular arc, and this variable is used to determine the angular range of this arc (and
 * thus the total size of the trail). The trail consists of 10 segments in an arc, and the angle of each of those
 * segments is M_PI / sSwordTrailAngularRangeDivisor; in other words, as this variable decreases, Odolwa's sword trail
 * covers a larger angular range, and as it increaes, the sword trail covers a smaller angular range.
 */
static f32 sSwordTrailAngularRangeDivisor = 10.0f;

typedef enum {
    // Named based on the fact that everything with this damage effect deals zero damage. If this effect is given to an
    // attack that deals non-zero damage, it will behave exactly like ODOLWA_DMGEFF_DAMAGE.
    /* 0x0 */ ODOLWA_DMGEFF_IMMUNE,

    // Deals no damage, but turns Odolwa blue, stops all animations, and makes him wait in place for 40 frames.
    /* 0x1 */ ODOLWA_DMGEFF_STUN,

    // Deals damage and surrounds Odolwa with fire.
    /* 0x2 */ ODOLWA_DMGEFF_FIRE,

    // Behaves exactly like ODOLWA_DMGEFF_STUN, but also surrounds Odolwa with ice.
    /* 0x3 */ ODOLWA_DMGEFF_FREEZE,

    // Deals damage and surrounds Odolwa with yellow light orbs.
    /* 0x4 */ ODOLWA_DMGEFF_LIGHT_ORB,

    // Behaves exactly like ODOLWA_DMGEFF_STUN, but also surrounds Odolwa in electric sparks.
    /* 0xB */ ODOLWA_DMGEFF_ELECTRIC_STUN = 0xB,

    // Deals damage and surrounds Odolwa with blue light orbs.
    /* 0xC */ ODOLWA_DMGEFF_BLUE_LIGHT_ORB,

    // Deals damage and has no special effect.
    /* 0xD */ ODOLWA_DMGEFF_DAMAGE,

    // Deals damage and checks the timer that tracks how long Odolwa should be in his damaged state. If the timer is 5
    // or more, it will reset the timer to 20 frames keep Odolwa in the damaged state for longer. If the timer is 4 or
    // less, it will disable Odolwa's collision for 20 frames to ensure he can jump away without taking further damage.
    /* 0xE */ ODOLWA_DMGEFF_DAMAGE_TIMER_CHECK,

    // Deals no damage, but makes Odolwa play his dazed animation for 70 frames and be vulnerable to attacks.
    /* 0xF */ ODOLWA_DMGEFF_DAZE
} OdolwaDamageEffect;

static DamageTable sOdolwaDamageTable = {
    /* Deku Nut       */ DMG_ENTRY(0, ODOLWA_DMGEFF_STUN),
    /* Deku Stick     */ DMG_ENTRY(1, ODOLWA_DMGEFF_DAZE),
    /* Horse trample  */ DMG_ENTRY(0, ODOLWA_DMGEFF_IMMUNE),
    /* Explosives     */ DMG_ENTRY(1, ODOLWA_DMGEFF_DAMAGE_TIMER_CHECK),
    /* Zora boomerang */ DMG_ENTRY(1, ODOLWA_DMGEFF_DAZE),
    /* Normal arrow   */ DMG_ENTRY(1, ODOLWA_DMGEFF_DAZE),
    /* UNK_DMG_0x06   */ DMG_ENTRY(0, ODOLWA_DMGEFF_IMMUNE),
    /* Hookshot       */ DMG_ENTRY(0, ODOLWA_DMGEFF_IMMUNE),
    /* Goron punch    */ DMG_ENTRY(1, ODOLWA_DMGEFF_DAMAGE_TIMER_CHECK),
    /* Sword          */ DMG_ENTRY(1, ODOLWA_DMGEFF_DAMAGE_TIMER_CHECK),
    /* Goron pound    */ DMG_ENTRY(1, ODOLWA_DMGEFF_DAZE),
    /* Fire arrow     */ DMG_ENTRY(2, ODOLWA_DMGEFF_FIRE),
    /* Ice arrow      */ DMG_ENTRY(2, ODOLWA_DMGEFF_FREEZE),
    /* Light arrow    */ DMG_ENTRY(2, ODOLWA_DMGEFF_LIGHT_ORB),
    /* Goron spikes   */ DMG_ENTRY(1, ODOLWA_DMGEFF_DAZE),
    /* Deku spin      */ DMG_ENTRY(1, ODOLWA_DMGEFF_DAZE),
    /* Deku bubble    */ DMG_ENTRY(0, ODOLWA_DMGEFF_STUN),
    /* Deku launch    */ DMG_ENTRY(1, ODOLWA_DMGEFF_DAMAGE_TIMER_CHECK),
    /* UNK_DMG_0x12   */ DMG_ENTRY(0, ODOLWA_DMGEFF_STUN),
    /* Zora barrier   */ DMG_ENTRY(0, ODOLWA_DMGEFF_ELECTRIC_STUN),
    /* Normal shield  */ DMG_ENTRY(0, ODOLWA_DMGEFF_IMMUNE),
    /* Light ray      */ DMG_ENTRY(0, ODOLWA_DMGEFF_IMMUNE),
    /* Thrown object  */ DMG_ENTRY(1, ODOLWA_DMGEFF_DAZE),
    /* Zora punch     */ DMG_ENTRY(1, ODOLWA_DMGEFF_DAMAGE_TIMER_CHECK),
    /* Spin attack    */ DMG_ENTRY(1, ODOLWA_DMGEFF_DAMAGE),
    /* Sword beam     */ DMG_ENTRY(2, ODOLWA_DMGEFF_BLUE_LIGHT_ORB),
    /* Normal Roll    */ DMG_ENTRY(0, ODOLWA_DMGEFF_IMMUNE),
    /* UNK_DMG_0x1B   */ DMG_ENTRY(0, ODOLWA_DMGEFF_IMMUNE),
    /* UNK_DMG_0x1C   */ DMG_ENTRY(0, ODOLWA_DMGEFF_IMMUNE),
    /* Unblockable    */ DMG_ENTRY(0, ODOLWA_DMGEFF_IMMUNE),
    /* UNK_DMG_0x1E   */ DMG_ENTRY(0, ODOLWA_DMGEFF_IMMUNE),
    /* Powder Keg     */ DMG_ENTRY(1, ODOLWA_DMGEFF_DAMAGE_TIMER_CHECK),
};

typedef enum {
    // Named based on the fact that everything with this damage effect deals zero damage. If this effect is given to an
    // attack that deals non-zero damage, it will behave exactly like BUG_DMGEFF_DAMAGE.
    /* 0x0 */ BUG_DMGEFF_IMMUNE,

    // Deals no damage, but turns the bug blue, stops all animations, and makes it wait in place for 40 frames.
    /* 0x1 */ BUG_DMGEFF_STUN,

    // Named after the only attack that uses it. Behaves exactly like BUG_DMGEFF_DAMAGE.
    /* 0x2 */ BUG_DMGEFF_FIRE_ARROW,

    // Named after the only attack that uses it. Behaves exactly like BUG_DMGEFF_DAMAGE.
    /* 0x3 */ BUG_DMGEFF_ICE_ARROW,

    // Named after the only attack that uses it. Behaves exactly like BUG_DMGEFF_DAMAGE.
    /* 0x4 */ BUG_DMGEFF_LIGHT_ARROW,

    // Named after the only two attacks that use it. Behaves exactly like BUG_DMGEFF_DAMAGE.
    /* 0xD */ BUG_DMGEFF_SPIN_ATTACK_AND_SWORD_BEAM = 0xD,

    // Named after the only attack that uses it. Behaves exactly like BUG_DMGEFF_DAMAGE.
    /* 0xE */ BUG_DMGEFF_ZORA_PUNCH,

    // Deals damage and has no special effect.
    /* 0xF */ BUG_DMGEFF_DAMAGE
} BugDamageEffect;

static DamageTable sBugDamageTable = {
    /* Deku Nut       */ DMG_ENTRY(0, BUG_DMGEFF_STUN),
    /* Deku Stick     */ DMG_ENTRY(1, BUG_DMGEFF_DAMAGE),
    /* Horse trample  */ DMG_ENTRY(0, BUG_DMGEFF_IMMUNE),
    /* Explosives     */ DMG_ENTRY(2, BUG_DMGEFF_DAMAGE),
    /* Zora boomerang */ DMG_ENTRY(0, BUG_DMGEFF_STUN),
    /* Normal arrow   */ DMG_ENTRY(2, BUG_DMGEFF_DAMAGE),
    /* UNK_DMG_0x06   */ DMG_ENTRY(0, BUG_DMGEFF_IMMUNE),
    /* Hookshot       */ DMG_ENTRY(0, BUG_DMGEFF_IMMUNE),
    /* Goron punch    */ DMG_ENTRY(1, BUG_DMGEFF_DAMAGE),
    /* Sword          */ DMG_ENTRY(1, BUG_DMGEFF_DAMAGE),
    /* Goron pound    */ DMG_ENTRY(1, BUG_DMGEFF_DAMAGE),
    /* Fire arrow     */ DMG_ENTRY(2, BUG_DMGEFF_FIRE_ARROW),
    /* Ice arrow      */ DMG_ENTRY(2, BUG_DMGEFF_ICE_ARROW),
    /* Light arrow    */ DMG_ENTRY(2, BUG_DMGEFF_LIGHT_ARROW),
    /* Goron spikes   */ DMG_ENTRY(1, BUG_DMGEFF_DAMAGE),
    /* Deku spin      */ DMG_ENTRY(0, BUG_DMGEFF_STUN),
    /* Deku bubble    */ DMG_ENTRY(0, BUG_DMGEFF_STUN),
    /* Deku launch    */ DMG_ENTRY(1, BUG_DMGEFF_DAMAGE),
    /* UNK_DMG_0x12   */ DMG_ENTRY(0, BUG_DMGEFF_STUN),
    /* Zora barrier   */ DMG_ENTRY(0, BUG_DMGEFF_STUN),
    /* Normal shield  */ DMG_ENTRY(0, BUG_DMGEFF_IMMUNE),
    /* Light ray      */ DMG_ENTRY(0, BUG_DMGEFF_IMMUNE),
    /* Thrown object  */ DMG_ENTRY(1, BUG_DMGEFF_DAMAGE),
    /* Zora punch     */ DMG_ENTRY(1, BUG_DMGEFF_ZORA_PUNCH),
    /* Spin attack    */ DMG_ENTRY(2, BUG_DMGEFF_SPIN_ATTACK_AND_SWORD_BEAM),
    /* Sword beam     */ DMG_ENTRY(2, BUG_DMGEFF_SPIN_ATTACK_AND_SWORD_BEAM),
    /* Normal Roll    */ DMG_ENTRY(0, BUG_DMGEFF_IMMUNE),
    /* UNK_DMG_0x1B   */ DMG_ENTRY(0, BUG_DMGEFF_IMMUNE),
    /* UNK_DMG_0x1C   */ DMG_ENTRY(0, BUG_DMGEFF_IMMUNE),
    /* Unblockable    */ DMG_ENTRY(0, BUG_DMGEFF_IMMUNE),
    /* UNK_DMG_0x1E   */ DMG_ENTRY(0, BUG_DMGEFF_IMMUNE),
    /* Powder Keg     */ DMG_ENTRY(1, BUG_DMGEFF_DAMAGE),
};

// The limbs referenced here are not used. The spheres are positioned manually by Boss01_PostLimbDraw.
static ColliderJntSphElementInit sSwordColliderJntSphElementsInit[3] = {
    {
        {
            ELEM_MATERIAL_UNK2,
            { 0xF7CFFFFF, 0x04, 0x04 },
            { 0xF7CFFFFF, 0x00, 0x00 },
            ATELEM_ON | ATELEM_SFX_NORMAL,
            ACELEM_ON,
            OCELEM_ON,
        },
        { ODOLWA_LIMB_NONE, { { 0, 0, 0 }, 35 }, 100 },
    },
    {
        {
            ELEM_MATERIAL_UNK2,
            { 0xF7CFFFFF, 0x04, 0x04 },
            { 0xF7CFFFFF, 0x00, 0x00 },
            ATELEM_ON | ATELEM_SFX_NORMAL,
            ACELEM_ON,
            OCELEM_ON,
        },
        { ODOLWA_LIMB_NONE, { { 0, 0, 0 }, 35 }, 100 },
    },
    {
        {
            ELEM_MATERIAL_UNK2,
            { 0xF7CFFFFF, 0x04, 0x04 },
            { 0xF7CFFFFF, 0x00, 0x00 },
            ATELEM_ON | ATELEM_SFX_NORMAL,
            ACELEM_ON,
            OCELEM_ON,
        },
        { ODOLWA_LIMB_NONE, { { 0, 0, 0 }, 70 }, 100 },
    },
};

static ColliderJntSphInit sSwordColliderJntSphInit = {
    {
        COL_MATERIAL_METAL,
        AT_ON | AT_TYPE_ENEMY,
        AC_ON | AC_HARD | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_PLAYER,
        OC2_TYPE_1,
        COLSHAPE_JNTSPH,
    },
    ARRAY_COUNT(sSwordColliderJntSphElementsInit),
    sSwordColliderJntSphElementsInit,
};

// The limbs referenced here are not used. The spheres are positioned manually by Boss01_PostLimbDraw.
static ColliderJntSphElementInit sShieldColliderJntSphElementsInit[1] = {
    {
        {
            ELEM_MATERIAL_UNK2,
            { 0xF7CFFFFF, 0x00, 0x04 },
            { 0xF7CFFFFF, 0x00, 0x00 },
            ATELEM_ON | ATELEM_SFX_NORMAL,
            ACELEM_ON,
            OCELEM_ON,
        },
        { ODOLWA_LIMB_NONE, { { 0, 0, 0 }, 36 }, 100 },
    },
};

static ColliderJntSphInit sShieldColliderJntSphInit = {
    {
        COL_MATERIAL_METAL,
        AT_ON | AT_TYPE_ENEMY,
        AC_ON | AC_HARD | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_PLAYER,
        OC2_TYPE_1,
        COLSHAPE_JNTSPH,
    },
    ARRAY_COUNT(sShieldColliderJntSphElementsInit),
    sShieldColliderJntSphElementsInit,
};

// The limbs referenced here are not used. The spheres are positioned manually by Boss01_PostLimbDraw.
static ColliderJntSphElementInit sBodyColliderJntSphElementsInit[11] = {
    {
        {
            ELEM_MATERIAL_UNK3,
            { 0xF7CFFFFF, 0x00, 0x04 },
            { 0xF7CFFFFF, 0x00, 0x00 },
            ATELEM_ON | ATELEM_SFX_NORMAL,
            ACELEM_ON,
            OCELEM_ON,
        },
        { ODOLWA_LIMB_NONE, { { 0, 0, 0 }, 20 }, 100 },
    },
    {
        {
            ELEM_MATERIAL_UNK3,
            { 0xF7CFFFFF, 0x00, 0x04 },
            { 0xF7CFFFFF, 0x00, 0x00 },
            ATELEM_ON | ATELEM_SFX_NORMAL,
            ACELEM_ON,
            OCELEM_ON,
        },
        { ODOLWA_LIMB_ROOT, { { 0, 0, 0 }, 30 }, 100 },
    },
    {
        {
            ELEM_MATERIAL_UNK3,
            { 0xF7CFFFFF, 0x00, 0x04 },
            { 0xF7CFFFFF, 0x00, 0x00 },
            ATELEM_ON | ATELEM_SFX_NORMAL,
            ACELEM_ON,
            OCELEM_ON,
        },
        { ODOLWA_LIMB_ROOT, { { 0, 0, 0 }, 25 }, 100 },
    },
    {
        {
            ELEM_MATERIAL_UNK3,
            { 0xF7CFFFFF, 0x00, 0x04 },
            { 0xF7CFFFFF, 0x00, 0x00 },
            ATELEM_ON | ATELEM_SFX_NORMAL,
            ACELEM_ON,
            OCELEM_ON,
        },
        { ODOLWA_LIMB_ROOT, { { 0, 0, 0 }, 15 }, 100 },
    },
    {
        {
            ELEM_MATERIAL_UNK3,
            { 0xF7CFFFFF, 0x00, 0x04 },
            { 0xF7CFFFFF, 0x00, 0x00 },
            ATELEM_ON | ATELEM_SFX_NORMAL,
            ACELEM_ON,
            OCELEM_ON,
        },
        { ODOLWA_LIMB_ROOT, { { 0, 0, 0 }, 15 }, 100 },
    },
    {
        {
            ELEM_MATERIAL_UNK3,
            { 0xF7CFFFFF, 0x00, 0x04 },
            { 0xF7CFFFFF, 0x00, 0x00 },
            ATELEM_ON | ATELEM_SFX_NORMAL,
            ACELEM_ON,
            OCELEM_ON,
        },
        { ODOLWA_LIMB_ROOT, { { 0, 0, 0 }, 15 }, 100 },
    },
    {
        {
            ELEM_MATERIAL_UNK3,
            { 0xF7CFFFFF, 0x00, 0x04 },
            { 0xF7CFFFFF, 0x00, 0x00 },
            ATELEM_ON | ATELEM_SFX_NORMAL,
            ACELEM_ON,
            OCELEM_ON,
        },
        { ODOLWA_LIMB_ROOT, { { 0, 0, 0 }, 15 }, 100 },
    },
    {
        {
            ELEM_MATERIAL_UNK3,
            { 0xF7CFFFFF, 0x00, 0x04 },
            { 0xF7CFFFFF, 0x00, 0x00 },
            ATELEM_ON | ATELEM_SFX_NORMAL,
            ACELEM_ON,
            OCELEM_ON,
        },
        { ODOLWA_LIMB_ROOT, { { 0, 0, 0 }, 15 }, 100 },
    },
    {
        {
            ELEM_MATERIAL_UNK3,
            { 0xF7CFFFFF, 0x00, 0x04 },
            { 0xF7CFFFFF, 0x00, 0x00 },
            ATELEM_ON | ATELEM_SFX_NORMAL,
            ACELEM_ON,
            OCELEM_ON,
        },
        { ODOLWA_LIMB_ROOT, { { 0, 0, 0 }, 15 }, 100 },
    },
    {
        {
            ELEM_MATERIAL_UNK3,
            { 0xF7CFFFFF, 0x00, 0x04 },
            { 0xF7CFFFFF, 0x00, 0x00 },
            ATELEM_ON | ATELEM_SFX_NORMAL,
            ACELEM_ON,
            OCELEM_ON,
        },
        { ODOLWA_LIMB_ROOT, { { 0, 0, 0 }, 15 }, 100 },
    },
    {
        {
            ELEM_MATERIAL_UNK3,
            { 0xF7CFFFFF, 0x00, 0x04 },
            { 0xF7CFFFFF, 0x00, 0x00 },
            ATELEM_ON | ATELEM_SFX_NORMAL,
            ACELEM_ON,
            OCELEM_ON,
        },
        { ODOLWA_LIMB_ROOT, { { 0, 0, 0 }, 15 }, 100 },
    },
};

static ColliderJntSphInit sBodyColliderJntSphInit = {
    {
        COL_MATERIAL_HIT3,
        AT_ON | AT_TYPE_ENEMY,
        AC_ON | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_PLAYER,
        OC2_TYPE_1,
        COLSHAPE_JNTSPH,
    },
    ARRAY_COUNT(sBodyColliderJntSphElementsInit),
    sBodyColliderJntSphElementsInit,
};

// The limbs referenced here are not used. The spheres are positioned manually by Boss01_PostLimbDraw.
static ColliderJntSphElementInit sKickAndShieldBashColliderJntSphElementsInit[2] = {
    {
        {
            ELEM_MATERIAL_UNK3,
            { 0xF7CFFFFF, 0x04, 0x04 },
            { 0xF7CFFFFF, 0x00, 0x00 },
            ATELEM_ON | ATELEM_SFX_HARD,
            ACELEM_ON,
            OCELEM_ON,
        },
        { ODOLWA_LIMB_NONE, { { 0, 0, 0 }, 36 }, 100 },
    },
    {
        {
            ELEM_MATERIAL_UNK3,
            { 0xF7CFFFFF, 0x04, 0x04 },
            { 0xF7CFFFFF, 0x00, 0x00 },
            ATELEM_ON | ATELEM_SFX_HARD,
            ACELEM_ON,
            OCELEM_ON,
        },
        { ODOLWA_LIMB_NONE, { { 0, 0, 0 }, 36 }, 100 },
    },
};

static ColliderJntSphInit sKickAndShieldBashColliderJntSphInit = {
    {
        COL_MATERIAL_HIT3,
        AT_ON | AT_TYPE_ENEMY,
        AC_ON | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_PLAYER,
        OC2_TYPE_1,
        COLSHAPE_JNTSPH,
    },
    ARRAY_COUNT(sKickAndShieldBashColliderJntSphElementsInit),
    sKickAndShieldBashColliderJntSphElementsInit,
};

static ColliderCylinderInit sBugACColliderCylinderInit = {
    {
        COL_MATERIAL_HIT3,
        AT_ON | AT_TYPE_ENEMY,
        AC_ON | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_1,
        COLSHAPE_CYLINDER,
    },
    {
        ELEM_MATERIAL_UNK3,
        { 0xF7CFFFFF, 0x00, 0x04 },
        { 0xF7FFFFFF, 0x00, 0x00 },
        ATELEM_ON | ATELEM_SFX_NORMAL,
        ACELEM_ON,
        OCELEM_ON,
    },
    { 15, 15, 10, { 0, 0, 0 } },
};

static ColliderCylinderInit sBugATColliderCylinderInit = {
    {
        COL_MATERIAL_HIT3,
        AT_ON | AT_TYPE_ENEMY,
        AC_ON | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_1,
        COLSHAPE_CYLINDER,
    },
    {
        ELEM_MATERIAL_UNK3,
        { 0xF7CFFFFF, 0x00, 0x04 },
        { 0xF7FFFFFF, 0x00, 0x00 },
        ATELEM_ON | ATELEM_SFX_NORMAL,
        ACELEM_ON,
        OCELEM_ON,
    },
    { 8, 15, 10, { 0, 0, 0 } },
};

ActorProfile Boss_01_Profile = {
    /**/ ACTOR_BOSS_01,
    /**/ ACTORCAT_BOSS,
    /**/ FLAGS,
    /**/ OBJECT_BOSS01,
    /**/ sizeof(Boss01),
    /**/ Boss01_Init,
    /**/ Boss01_Destroy,
    /**/ Boss01_Update,
    /**/ Boss01_Draw,
    /**/ Boss01_Reset,
};

static Color_RGBA8 sDustPrimColor = { 60, 50, 20, 255 };

static Color_RGBA8 sDustEnvColor = { 40, 30, 30, 255 };

typedef struct {
    /* 0x00 */ Vec3f pos;
    /* 0x0C */ Vec3f velocity;
    /* 0x18 */ f32 gravity;
    /* 0x1C */ s16 rotX;
    /* 0x1E */ s16 rotY;
    /* 0x20 */ UNK_TYPE1 unk_20[0x8];
    /* 0x28 */ u8 type;
    /* 0x2A */ s16 timer;
    /* 0x2C */ s16 isFallingBlockFragment;
    /* 0x2E */ s16 angularVelocityX;
    /* 0x30 */ s16 angularVelocityY;
    /* 0x34 */ f32 scale;
    /* 0x38 */ f32 alpha;
} OdolwaEffect; // size = 0x3C

/**
 * Keeps track of how many bugs are currently spawned and alive. When there are four or fewer bugs alive, Odolwa can
 * summon more bugs or falling blocks.
 */
s16 sOdolwaBugCount;

/**
 * Pointer to the main Odolwa instance so that bugs and effects can check and manipulate his state.
 */
Boss01* sOdolwa;

/**
 * Pointer to the swarm of moths that Odolwa can summon.
 */
EnTanron1* sMothSwarm;

/**
 * Odolwa draws his sword trail manually by manipulating these variables during his horizontal and vertical slashes.
 * Note that the position of the sword is offset from Odolwa's position.
 */
f32 sOdolwaSwordTrailPosX;
f32 sOdolwaSwordTrailPosY;
f32 sOdolwaSwordTrailPosZ;
f32 sOdolwaSwordTrailRotX;
f32 sOdolwaSwordTrailRotY;
f32 sOdolwaSwordTrailRotZ;
f32 sOdolwaSwordTrailAlpha;

/**
 * If the intro cutscene was skipped (because it was already watched), this timer will be used to determine when to
 * start playing the boss theme.
 */
u8 sOdolwaMusicStartTimer;

/**
 * When Odolwa is damaged or dazed, the sound effect will play from this position. It is set to his projectedPos.
 */
Vec3f sOdolwaDamageSfxPos;

s32 sOdolwaRandSeed1;
s32 sOdolwaRandSeed2;
s32 sOdolwaRandSeed3;

/**
 * Stores all of Odolwa's effects. Note that the first entry in this array is always reserved for the ring of fire, so
 * falling blocks can only be placed from the second entry onwards.
 */
OdolwaEffect sOdolwaEffects[ODOLWA_EFFECT_COUNT];

void Boss01_InitRand(s32 seedInit1, s32 seedInit2, s32 seedInit3) {
    sOdolwaRandSeed1 = seedInit1;
    sOdolwaRandSeed2 = seedInit2;
    sOdolwaRandSeed3 = seedInit3;
}

f32 Boss01_RandZeroOne(void) {
    // Wichmann-Hill algorithm
    f32 randFloat;

    sOdolwaRandSeed1 = (sOdolwaRandSeed1 * 171) % 30269;
    sOdolwaRandSeed2 = (sOdolwaRandSeed2 * 172) % 30307;
    sOdolwaRandSeed3 = (sOdolwaRandSeed3 * 170) % 30323;

    randFloat = (sOdolwaRandSeed1 / 30269.0f) + (sOdolwaRandSeed2 / 30307.0f) + (sOdolwaRandSeed3 / 30323.0f);

    while (randFloat >= 1.0f) {
        randFloat -= 1.0f;
    }

    return fabsf(randFloat);
}

/**
 * Spawns a falling block or a fragment of a block that has hit the ground at the specified position.
 */
void Boss01_SpawnEffectFallingBlock(OdolwaEffect* effect, Vec3f* pos, s16 isFragment) {
    s16 i;

    for (i = 1; i < ODOLWA_EFFECT_COUNT; i++, effect++) {
        if (effect->type == ODOLWA_EFFECT_NONE) {
            effect->type = ODOLWA_EFFECT_FALLING_BLOCK;
            effect->pos = *pos;
            effect->timer = 0;
            effect->rotX = Rand_ZeroFloat(0x10000);
            effect->rotY = Rand_ZeroFloat(0x10000);
            effect->isFallingBlockFragment = isFragment;
            effect->gravity = -1.0f;

            if (!isFragment) {
                effect->angularVelocityY = Rand_CenteredFloat(0x320);
                effect->angularVelocityX = Rand_CenteredFloat(0x320);
                effect->scale = 8.0f * 0.001f;
                effect->velocity = gZeroVec3f;
            } else {
                effect->angularVelocityY = Rand_CenteredFloat(0x1F40);
                effect->angularVelocityX = Rand_CenteredFloat(0x1F40);
                effect->scale = Rand_CenteredFloat(1.6f * 0.001f) + (3.2f * 0.001f);
                effect->velocity.x = Rand_CenteredFloat(13.0f);
                effect->velocity.y = Rand_ZeroFloat(4.0f) + 7.0f;
                effect->velocity.z = Rand_CenteredFloat(13.0f);
            }

            break;
        }
    }
}

/**
 * Spawns the ring of fire at the specified position.
 */
void Boss01_SpawnEffectRingOfFire(OdolwaEffect* effect, Vec3f* pos) {
    if (effect->type == ODOLWA_EFFECT_NONE) {
        effect->type = ODOLWA_EFFECT_RING_OF_FIRE;
        effect->pos = *pos;
        effect->timer = 0;
        effect->velocity = gZeroVec3f;
        effect->gravity = 0.0f;
        effect->scale = 0.0f;
        effect->alpha = 230.0f;
    }
}

/**
 * Manually sets the position of a sphere collider to a specific position.
 */
void Boss01_SetColliderSphere(s32 index, ColliderJntSph* collider, Vec3f* sphereCenter) {
    collider->elements[index].dim.worldSphere.center.x = sphereCenter->x;
    collider->elements[index].dim.worldSphere.center.y = sphereCenter->y;
    collider->elements[index].dim.worldSphere.center.z = sphereCenter->z;
    collider->elements[index].dim.worldSphere.radius =
        collider->elements[index].dim.modelSphere.radius * collider->elements[index].dim.scale;
}

/**
 * For the most part, this function is responsible for selecting which attack Odolwa will do after he's done waiting.
 * Which attack he chooses depends on Odolwa's health, his distance to the player, and random chance. However, this
 * function can also make Odolwa do a specific dance before attacking; if the mustAttack parameter is false, then Odolwa
 * has a 20% chance of choosing to dance instead of attacking. This function will also make Odolwa go back to waiting if
 * the player is too far off the ground (e.g., by jumping out of the Deku Flower in the center of the arena).
 */
void Boss01_SelectAttack(Boss01* this, PlayState* play, u8 mustAttack) {
    Player* player = GET_PLAYER(play);

    if (player->actor.world.pos.y > 200.0f) {
        Boss01_SetupWait(this, play, ODOLWA_WAIT_RANDOM);
    } else if ((Rand_ZeroOne() < 0.2f) && !mustAttack) {
        // When Odolwa is done dancing, this function calls Boss01_SelectAttack with mustAttack set to true, so he will
        // be guaranteed to choose an attack later, so long as the player isn't too far off the ground.
        Boss01_SetupDanceBeforeAttack(this, play);
    } else if (this->actor.xzDistToPlayer <= 250.0f) {
        if (this->actor.xzDistToPlayer <= 150.0f) {
            if (Rand_ZeroOne() < 0.5f) {
                Boss01_SetupKick(this, play);
            } else {
                Boss01_SetupShieldBash(this, play);
            }
        } else {
            Boss01_SetupHorizontalSlash(this, play);
        }
    } else if (((s8)this->actor.colChkInfo.health < 8) && (Rand_ZeroOne() < 0.75f)) {
        Boss01_SetupSpinAttack(this, play);
    } else {
        Boss01_SetupVerticalSlash(this, play);
    }
}

/**
 * Spawns dust at both of Odolwa's feet if *any* of the following conditions are true:
 * - Odolwa's speed is greater than 1.0f
 * - Odolwa's additional velocity (in either the X or Z direction) is greater than 1.0f
 * - this->frameCounter & dustSpawnFrameMask is equal to 0
 * - dustSpawnFrameMask is 0
 */
void Boss01_SpawnDustAtFeet(Boss01* this, PlayState* play, u8 dustSpawnFrameMask) {
    u8 i;
    Vec3f pos;
    Vec3f velocity;
    Vec3f accel;

    if (((this->frameCounter & dustSpawnFrameMask) == 0) &&
        ((this->additionalVelocityX > 1.0f) || (this->additionalVelocityZ > 1.0f) || (dustSpawnFrameMask == 0) ||
         (this->actor.speed > 1.0f))) {
        for (i = 0; i < ARRAY_COUNT(this->feetPos); i++) {
            velocity.x = Rand_CenteredFloat(5.0f);
            velocity.y = Rand_ZeroFloat(2.0f) + 1.0f;
            velocity.z = Rand_CenteredFloat(5.0f);
            accel.y = -0.1f;
            accel.x = accel.z = 0.0f;
            pos.x = this->feetPos[i].x + Rand_CenteredFloat(20.0f);
            pos.y = Rand_ZeroFloat(10.0f) + 3.0f;
            pos.z = this->feetPos[i].z + Rand_CenteredFloat(20.0f);
            func_800B0EB0(play, &pos, &velocity, &accel, &sDustPrimColor, &sDustEnvColor,
                          Rand_ZeroFloat(150.0f) + 350.0f, 10, Rand_ZeroFloat(5.0f) + 14.0f);
        }
    }
}

void Boss01_Init(Actor* thisx, PlayState* play) {
    Boss01* this = (Boss01*)thisx;
    s32 pad;
    s16 i;

    Actor_SetScale(&this->actor, 0.015f);
    if (ODOLWA_GET_TYPE(&this->actor) == ODOLWA_TYPE_BUG) {
        SkelAnime_InitFlex(play, &this->skelAnime, &gOdolwaBugSkel, &gOdolwaBugCrawlAnim, this->jointTable,
                           this->morphTable, ODOLWA_BUG_LIMB_MAX);
        this->actor.update = Boss01_Bug_Update;
        this->actor.draw = Boss01_Bug_Draw;
        this->bugDrawDmgEffType = ACTOR_DRAW_DMGEFF_BLUE_FIRE;
        Boss01_Bug_SetupCrawl(this, play);
        this->actor.gravity = -2.0f;
        Collider_InitAndSetCylinder(play, &this->bugACCollider, &this->actor, &sBugACColliderCylinderInit);
        Collider_InitAndSetCylinder(play, &this->bugATCollider, &this->actor, &sBugATColliderCylinderInit);
        ActorShape_Init(&this->actor.shape, 0.0f, ActorShadow_DrawCircle, 12.0f);
        Actor_SetScale(&this->actor, 0.025f);
        this->actor.colChkInfo.health = 2;
        sOdolwaBugCount++;
        Actor_PlaySfx(&this->actor, NA_SE_EV_ROCK_FALL);
        this->actor.colChkInfo.damageTable = &sBugDamageTable;
        sOdolwa->actor.hintId = TATL_HINT_ID_ODOLWA_PHASE_TWO;
    } else if (ODOLWA_GET_TYPE(&this->actor) == ODOLWA_TYPE_AFTERIMAGE) {
        SkelAnime_InitFlex(play, &this->skelAnime, &gOdolwaSkel, &gOdolwaReadyAnim, this->jointTable, this->morphTable,
                           ODOLWA_LIMB_MAX);
        Boss01_Afterimage_SetupWaitToDespawn(this, play);
        this->timers[TIMER_AFTERIMAGE_DESPAWN] = ODOLWA_GET_AFTERIMAGE_DESPAWN_TIMER(&this->actor);
        this->actor.world.rot.z = 0;
        this->actor.draw = Boss01_Afterimage_Draw;
        this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
    } else {
        if (CHECK_WEEKEVENTREG(WEEKEVENTREG_CLEARED_WOODFALL_TEMPLE)) {
            Actor_SpawnAsChild(&play->actorCtx, &this->actor, play, ACTOR_DOOR_WARP1, 0.0f, 0.0f, 0.0f, 0, 0, 0,
                               ENDOORWARP1_FF_1);
            Actor_Spawn(&play->actorCtx, play, ACTOR_ITEM_B_HEART, 0.0f, 0.0f, 250.0f, 0, 0, 0, BHEART_PARAM_NORMAL);
            Actor_Kill(&this->actor);
            return;
        }

        play->envCtx.lightSettingOverride = 0;
        play->envCtx.lightBlendOverride = LIGHT_BLEND_OVERRIDE_FULL_CONTROL;
        play->envCtx.lightBlend = 0.0f;
        sOdolwa = this;
        sOdolwaBugCount = 0;
        play->specialEffects = sOdolwaEffects;

        for (i = 0; i < ODOLWA_EFFECT_COUNT; i++) {
            sOdolwaEffects[i].type = ODOLWA_EFFECT_NONE;
        }

        this->actor.hintId = TATL_HINT_ID_ODOLWA_PHASE_ONE;
        this->actor.attentionRangeType = ATTENTION_RANGE_5;
        this->actor.colChkInfo.mass = MASS_HEAVY;
        this->actor.colChkInfo.damageTable = &sOdolwaDamageTable;
        this->actor.colChkInfo.health = 20;

        ActorShape_Init(&this->actor.shape, 0.0f, NULL, 0.0f);
        Collider_InitAndSetJntSph(play, &this->swordCollider, &this->actor, &sSwordColliderJntSphInit,
                                  this->swordColliderElements);
        Collider_InitAndSetJntSph(play, &this->shieldCollider, &this->actor, &sShieldColliderJntSphInit,
                                  this->shieldColliderElements);
        Collider_InitAndSetJntSph(play, &this->bodyCollider, &this->actor, &sBodyColliderJntSphInit,
                                  this->bodyColliderElements);
        Collider_InitAndSetJntSph(play, &this->kickAndShieldBashCollider, &this->actor,
                                  &sKickAndShieldBashColliderJntSphInit, this->kickAndShieldBashColliderElements);
        SkelAnime_InitFlex(play, &this->skelAnime, &gOdolwaSkel, &gOdolwaReadyAnim, this->jointTable, this->morphTable,
                           ODOLWA_LIMB_MAX);

        if ((KREG(64) != 0) || CHECK_EVENTINF(EVENTINF_INTRO_CS_WATCHED_ODOLWA)) {
            Boss01_SetupWait(this, play, ODOLWA_WAIT_READY);
            this->actor.gravity = -2.5f;
            sOdolwaMusicStartTimer = KREG(15) + 20;
        } else {
            Boss01_SetupIntroCutscene(this, play);
        }

        sMothSwarm = (EnTanron1*)Actor_SpawnAsChild(&play->actorCtx, &this->actor, play, ACTOR_EN_TANRON1, 0.0f, 437.0f,
                                                    -750.0f, 0, 0, 0, 0x100);
    }

    this->animMorphFrames1 = -2.0f;
    this->animMorphFrames2 = -4.0f;
}

void Boss01_Destroy(Actor* thisx, PlayState* play) {
}

/**
 * Checks every explosive actor to see if Odolwa is close enough to any of them. If he is, then he'll jump.
 */
void Boss01_JumpAwayFromExplosive(Boss01* this, PlayState* play) {
    Actor* explosive;

    for (explosive = play->actorCtx.actorLists[ACTORCAT_EXPLOSIVES].first; explosive != NULL;
         explosive = explosive->next) {
        if (sqrtf(SQ(explosive->world.pos.x - this->actor.world.pos.x) +
                  SQ(explosive->world.pos.y - this->actor.world.pos.y) +
                  SQ(explosive->world.pos.z - this->actor.world.pos.z)) < 150.0f) {
            Boss01_SetupJump(this, play, false);
        }
    }
}

/**
 * Note that this function will move Odolwa to (0, 2400, 0), regardless of where he was originally spawned. This is so
 * he can fall from the ceiling in his intro cutscene, and this is also why he's not visible until the cutscene starts.
 */
void Boss01_SetupIntroCutscene(Boss01* this, PlayState* play) {
    this->actionFunc = Boss01_IntroCutscene;
    this->actor.world.pos.x = 0.0f;
    this->actor.world.pos.z = 0.0f;
    this->actor.gravity = 0.0f;
    this->actor.world.pos.y = 2400.0f;
    Animation_MorphToLoop(&this->skelAnime, &gOdolwaCrouchAnim, 0.0f);
}

/**
 * Handles the entirety of Odolwa's intro cutscene, including manipulating the camera, starting the boss theme, showing
 * Odolwa's title card, etc. It also handles waiting for the player to move forward from the door before actually
 * starting the cutscene.
 */
void Boss01_IntroCutscene(Boss01* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    this->cutsceneTimer++;
    SkelAnime_Update(&this->skelAnime);

    switch (this->cutsceneState) {
        case ODOLWA_INTRO_CS_STATE_WAITING_FOR_PLAYER_OR_DONE:
            if ((CutsceneManager_GetCurrentCsId() != CS_ID_NONE) || !(player->actor.world.pos.z < 590.0f)) {
                break;
            }

            Cutscene_StartManual(play, &play->csCtx);
            Player_SetCsActionWithHaltedActors(play, &this->actor, PLAYER_CSACTION_WAIT);
            this->subCamId = Play_CreateSubCamera(play);
            Play_ChangeCameraStatus(play, CAM_ID_MAIN, CAM_STATUS_WAIT);
            Play_ChangeCameraStatus(play, this->subCamId, CAM_STATUS_ACTIVE);
            this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
            this->cutsceneTimer = 0;
            this->cutsceneState = ODOLWA_INTRO_CS_STATE_LOOK_AT_PLAYER;
            this->subCamUp.x = 0.0f;
            this->subCamUp.y = 1.0f;
            this->subCamUp.z = 0.0f;
            // fallthrough
        case ODOLWA_INTRO_CS_STATE_LOOK_AT_PLAYER:
            player->actor.world.rot.y = -0x8000;
            player->actor.shape.rot.y = -0x8000;
            player->actor.world.pos.x = -9.0f;
            player->actor.world.pos.z = 587.0f;

            this->subCamEye.x = -9.0f;
            this->subCamEye.y = (Player_GetHeight(player) + player->actor.world.pos.y) - 24.0f;
            this->subCamEye.z = (player->actor.world.pos.z - 200.0f) + 110.0f;

            this->subCamAt.x = player->actor.world.pos.x;
            this->subCamAt.y = (Player_GetHeight(player) + player->actor.world.pos.y) - 14.0f;
            this->subCamAt.z = player->actor.world.pos.z;

            if (player->transformation == PLAYER_FORM_FIERCE_DEITY) {
                this->subCamEye.y -= 30.0f + BREG(16);
                this->subCamAt.y -= 30.0f + BREG(17);
            }

            if (this->cutsceneTimer >= 20) {
                Actor_PlaySfx(&this->actor, NA_SE_EN_MIBOSS_RHYTHM_OLD - SFX_FLAG);
            }

            if (this->cutsceneTimer == 40) {
                Player_SetCsActionWithHaltedActors(play, &this->actor, PLAYER_CSACTION_21);
            }

            if (this->cutsceneTimer == 100) {
                Player_SetCsActionWithHaltedActors(play, &this->actor, PLAYER_CSACTION_4);
            }

            if (this->cutsceneTimer >= 90) {
                Audio_PlaySfx(NA_SE_EN_MIBOSS_FALL_OLD_OLD - SFX_FLAG);
            }

            if (this->cutsceneTimer == 120) {
                this->actor.world.pos.y = 1500.0f;
                this->actor.gravity = -2.5f;
                this->cutsceneState = ODOLWA_INTRO_CS_STATE_ODOLWA_FALLING;
                this->cutsceneTimer = 0;
                Animation_MorphToPlayOnce(&this->skelAnime, &gOdolwaJumpAnim, 0.0f);
                this->subCamEye.x = this->actor.world.pos.x;
                this->subCamAt.x = this->actor.world.pos.x;
                this->subCamAt.y = 80.0f;
                this->subCamEye.y = 30.0f;
                this->subCamEye.z = (this->actor.world.pos.z + 200.0f + 50.0f) - 150.0f;
                this->subCamAt.z = this->actor.world.pos.z;
                this->subCamUp.x = 2.0f;
            }
            break;

        case ODOLWA_INTRO_CS_STATE_ODOLWA_FALLING:
            Actor_PlaySfx(&this->actor, NA_SE_EN_MIBOSS_RHYTHM_OLD - SFX_FLAG);
            Math_ApproachZeroF(&this->subCamUp.x, 0.1f, 0.1f);
            Audio_PlaySfx(NA_SE_EN_MIBOSS_FALL_OLD_OLD - SFX_FLAG);
            this->afterimageSpawnFrameMask = 2;
            Math_ApproachF(&this->subCamAt.y, this->actor.world.pos.y + 80.0f, 0.25f, 30.0f);
            if (this->actor.world.pos.y < 40.0f) {
                Animation_MorphToPlayOnce(&this->skelAnime, &gOdolwaCrouchAnim, -2.0f);
                this->cutsceneState = ODOLWA_INTRO_CS_STATE_ODOLWA_LANDED;
                this->cutsceneTimer = 0;
            }
            break;

        case ODOLWA_INTRO_CS_STATE_ODOLWA_LANDED:
            if (this->cutsceneTimer < 51) {
                Actor_PlaySfx(&this->actor, NA_SE_EN_MIBOSS_RHYTHM_OLD - SFX_FLAG);
            }

            if ((this->cutsceneTimer == 1) || (this->cutsceneTimer == 3)) {
                u8 i;

                for (i = 0; i < 10; i++) {
                    Boss01_SpawnDustAtFeet(this, play, 0);
                }

                Audio_PlaySfx(NA_SE_EN_MIBOSS_GND1_OLD);
                Rumble_Override(0.0f, 200, 20, 20);
                this->screenShakeMagnitude = 10.0f;
            }

            if (this->cutsceneTimer == 5) {
                Animation_MorphToLoop(&this->skelAnime, &gOdolwaIntroSlashAnim, -20.0f);
            }

            if ((this->cutsceneTimer >= 6) &&
                (Animation_OnFrame(&this->skelAnime, 30.0f) || Animation_OnFrame(&this->skelAnime, 54.0f))) {
                Actor_PlaySfx(&this->actor, NA_SE_EN_MIBOSS_SWORD_OLD);
            }

            if (this->cutsceneTimer < 30) {
                Math_ApproachF(&this->subCamAt.y, this->actor.world.pos.y + 80.0f, 0.25f, 1000.0f);
            } else {
                Math_ApproachF(&this->subCamEye.x, this->actor.world.pos.x + 70.0f, 0.05f,
                               this->subCamVelocity * 70.0f);
                Math_ApproachF(&this->subCamEye.y, 200.0f, 0.05f, this->subCamVelocity * 170.0f);
                Math_ApproachF(&this->subCamEye.z, (this->actor.world.pos.z + 200.0f + 50.0f) - 30.0f, 0.05f,
                               this->subCamVelocity * 120.0f);
                Math_ApproachF(&this->subCamAt.y, this->actor.world.pos.y + 80.0f + 20.0f, 0.05f,
                               this->subCamVelocity * 20.0f);
                Math_ApproachF(&this->subCamVelocity, 1.0f, 1.0f, 0.001f);
            }

            if (this->cutsceneTimer == 20) {
                SEQCMD_PLAY_SEQUENCE(SEQ_PLAYER_BGM_MAIN, 0, NA_BGM_BOSS | SEQ_FLAG_ASYNC);
            }

            if (this->cutsceneTimer == 50) {
                TitleCard_InitBossName(&play->state, &play->actorCtx.titleCtx,
                                       Lib_SegmentedToVirtual(&gOdolwaTitleCardTex), 160, 180, 128, 40);
            }

            if (this->cutsceneTimer == 140) {
                Camera* mainCam = Play_GetCamera(play, CAM_ID_MAIN);

                this->cutsceneState = ODOLWA_INTRO_CS_STATE_WAITING_FOR_PLAYER_OR_DONE;
                this->phaseFrameCounter = 0;
                Boss01_SetupWait(this, play, ODOLWA_WAIT_READY);
                mainCam->eye = this->subCamEye;
                mainCam->eyeNext = this->subCamEye;
                mainCam->at = this->subCamAt;
                func_80169AFC(play, this->subCamId, 0);
                this->subCamId = SUB_CAM_ID_DONE;
                Cutscene_StopManual(play, &play->csCtx);
                Player_SetCsActionWithHaltedActors(play, &this->actor, PLAYER_CSACTION_END);
                this->actor.flags |= ACTOR_FLAG_ATTENTION_ENABLED;
                SET_EVENTINF(EVENTINF_INTRO_CS_WATCHED_ODOLWA);
            }
            break;

        default:
            break;
    }

    if (this->subCamId != SUB_CAM_ID_DONE) {
        Vec3f at;

        ShrinkWindow_Letterbox_SetSizeTarget(27);
        this->screenShakeOffsetY = Math_CosS(play->gameplayFrames * 0x8000) * this->screenShakeMagnitude;
        Math_ApproachZeroF(&this->screenShakeMagnitude, 1.0f, 0.75f);
        at = this->subCamAt;
        at.y += this->screenShakeOffsetY;
        Play_SetCameraAtEyeUp(play, this->subCamId, &at, &this->subCamEye, &this->subCamUp);
    }
}

void Boss01_SetupSummonBugsCutscene(Boss01* this, PlayState* play) {
    this->actionFunc = Boss01_SummonBugsCutscene;
    Animation_MorphToLoop(&this->skelAnime, &gOdolwaSideToSideDanceAnim, -10.0f);
    this->cutsceneState = ODOLWA_BUG_SUMMONING_CS_STATE_STARTED;
    this->cutsceneTimer = 0;
    this->hasPlayedSummonBugCs++;
    this->disableCollisionTimer = 30;
    this->actor.speed = 0.0f;
    this->additionalVelocityZ = 0.0f;
    this->additionalVelocityX = 0.0f;
}

/**
 * Handles everything involving the bug summoning cutscene, including manipulating the camera, spawning the bugs, etc.
 */
void Boss01_SummonBugsCutscene(Boss01* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    Vec3f offset;
    Vec3f pos;

    this->cutsceneTimer++;
    this->disableCollisionTimer = 30;
    SkelAnime_Update(&this->skelAnime);

    switch (this->cutsceneState) {
        case ODOLWA_BUG_SUMMONING_CS_STATE_STARTED:
            if (CutsceneManager_GetCurrentCsId() != CS_ID_NONE) {
                break;
            }

            Cutscene_StartManual(play, &play->csCtx);
            Player_SetCsActionWithHaltedActors(play, &this->actor, PLAYER_CSACTION_WAIT);
            this->subCamId = Play_CreateSubCamera(play);
            Play_ChangeCameraStatus(play, CAM_ID_MAIN, CAM_STATUS_WAIT);
            Play_ChangeCameraStatus(play, this->subCamId, CAM_STATUS_ACTIVE);
            this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
            this->cutsceneState = ODOLWA_BUG_SUMMONING_CS_STATE_PLAYING_OR_DONE;
            this->actor.shape.rot.y = 0;
            this->actor.world.pos.z = 0.0f;
            this->actor.world.pos.x = 0.0f;
            this->subCamVelocity = 0.0f;
            this->subCamEyeNext.y = 100.0f;
            // fallthrough
        case ODOLWA_BUG_SUMMONING_CS_STATE_PLAYING_OR_DONE:
            Actor_PlaySfx(&this->actor, NA_SE_EN_MIBOSS_VOICE1_OLD - SFX_FLAG);
            Matrix_RotateYS(this->actor.shape.rot.y, MTXMODE_NEW);
            Matrix_MultVecZ(250.0f, &offset);

            this->subCamEye.x = this->actor.world.pos.x + offset.x;
            this->subCamEye.y = this->actor.world.pos.y + offset.y + 40.0f;
            this->subCamEye.z = this->actor.world.pos.z + offset.z;

            this->subCamAt.x = this->actor.world.pos.x;
            this->subCamAt.y = this->actor.world.pos.y + this->subCamEyeNext.y;
            this->subCamAt.z = this->actor.world.pos.z;

            if (this->cutsceneTimer > 40) {
                Math_ApproachF(&this->subCamEyeNext.y, 800.0f, 0.1f, this->subCamVelocity);
                Math_ApproachF(&this->subCamVelocity, 100.0f, 1.0f, 1.0f);
                if ((this->cutsceneTimer > 80) && ((this->cutsceneTimer % 16) == 0)) {
                    Matrix_MultVecZ(100.0f, &offset);
                    pos.x = Rand_CenteredFloat(200.0f) + (this->actor.world.pos.x + offset.x);
                    pos.z = Rand_CenteredFloat(200.0f) + (this->actor.world.pos.z + offset.z);
                    Audio_PlaySfx(NA_SE_PL_DEKUNUTS_DROP_BOMB);
                    Actor_Spawn(&play->actorCtx, play, ACTOR_BOSS_01, pos.x, 1200.0f, pos.z, 0, Rand_ZeroFloat(0x10000),
                                0, ODOLWA_PARAMS(ODOLWA_TYPE_BUG));
                }
            }

            if (this->cutsceneTimer >= 170) {
                Boss01_SetupWait(this, play, ODOLWA_WAIT_READY);
                func_80169AFC(play, this->subCamId, 0);
                this->subCamId = SUB_CAM_ID_DONE;
                Cutscene_StopManual(play, &play->csCtx);
                Player_SetCsActionWithHaltedActors(play, &this->actor, PLAYER_CSACTION_END);
                this->actor.flags |= ACTOR_FLAG_ATTENTION_ENABLED;
                player->actor.world.rot.y = player->actor.shape.rot.y = -0x8000;
                player->actor.world.pos.x = 0.0f;
                player->actor.world.pos.z = -600.0f;
            }
            break;
    }

    if (this->subCamId != SUB_CAM_ID_DONE) {
        ShrinkWindow_Letterbox_SetSizeTarget(27);
        Play_SetCameraAtEye(play, this->subCamId, &this->subCamAt, &this->subCamEye);
    }
}

void Boss01_Afterimage_SetupWaitToDespawn(Boss01* this, PlayState* play) {
    this->actionFunc = Boss01_Afterimage_WaitToDespawn;
}

/**
 * Waits until the despawn timer reaches 0, then kills the actor.
 */
void Boss01_Afterimage_WaitToDespawn(Boss01* this, PlayState* play) {
    if (this->timers[TIMER_AFTERIMAGE_DESPAWN] == 0) {
        Actor_Kill(&this->actor);
    }
}

static AnimationHeader* sWaitAnimations[ODOLWA_WAIT_MAX] = {
    &gOdolwaReadyAnim,           // ODOLWA_WAIT_READY,
    &gOdolwaSpinSwordAnim,       // ODOLWA_WAIT_SPIN_SWORD,
    &gOdolwaVerticalHopAnim,     // ODOLWA_WAIT_VERTICAL_HOP,
    &gOdolwaHipShakeDanceAnim,   // ODOLWA_WAIT_SHAKE_DANCE,
    &gOdolwaUpAndDownDanceAnim,  // ODOLWA_WAIT_UP_AND_DOWN_DANCE,
    &gOdolwaArmSwingDanceAnim,   // ODOLWA_WAIT_ARM_SWING_DANCE,
    &gOdolwaThurstAttackAnim,    // ODOLWA_WAIT_THRUST_ATTACK,
    &gOdolwaDoubleSlashAnim,     // ODOLWA_WAIT_DOUBLE_SLASH,
    &gOdolwaSideToSideHopAnim,   // ODOLWA_WAIT_SIDE_TO_SIDE_HOP,
    &gOdolwaSideToSideDanceAnim, // ODOLWA_WAIT_SIDE_TO_SIDE_DANCE,
    &gOdolwaSpinDanceAnim,       // ODOLWA_WAIT_SPIN_DANCE
    &gOdolwaJumpDanceAnim,       // ODOLWA_WAIT_JUMP_DANCE
};

/**
 * Prepares Odolwa to enter his "wait" state, i.e., his primary state where he waits for a bit of time before attacking.
 * If the fight has been going for 1000 or more frames, this function is also responsible for starting the bug summoning
 * cutscene. Callers of this function can choose which type of wait action he will do using the waitType parameter,
 * though this parameter is ignored if the fight is still in its first phase (i.e, when the fight has been going on for
 * 999 or fewer frames); Odolwa will always perform the "ready" wait action in the first phase.
 *
 * If ODOLWA_WAIT_RANDOM is passed for the waitType, then Odolwa has a 30% chance of summoning moths instead of doing a
 * standard wait action. Otherwise, he will randomly choose his own wait type.
 */
void Boss01_SetupWait(Boss01* this, PlayState* play, u8 waitType) {
    if (this->phaseFrameCounter > 1000) {
        if (!this->hasPlayedSummonBugCs) {
            Boss01_SetupSummonBugsCutscene(this, play);
            return;
        }

        if (waitType == ODOLWA_WAIT_RANDOM) {
            if (Rand_ZeroOne() < 0.3f) {
                Boss01_SetupSummonMoths(this, play);
                return;
            }

            this->waitType = Rand_ZeroFloat(ODOLWA_WAIT_MAX - 0.001f);
        } else {
            this->waitType = waitType;
        }
    } else {
        this->waitType = ODOLWA_WAIT_READY;
    }

    Animation_MorphToLoop(&this->skelAnime, sWaitAnimations[this->waitType], 2.0f * this->animMorphFrames2);
    this->animEndFrame = Animation_GetLastFrame(sWaitAnimations[this->waitType]);
    this->actionFunc = Boss01_Wait;
    this->timers[TIMER_CURRENT_ACTION] = 80;
    this->jumpIfPlayerIsClose = Rand_ZeroFloat(1.999f);
    this->waitTimer = 0;
}

/**
 * This function will make Odolwa perform one of 12 different "wait actions," after which he will either select an
 * attack, jump, or start running. Because of the variety of actions this function handles, it has a large number of
 * responsibilities, including summoning the ring of fire and spawning more bugs and falling blocks.
 */
void Boss01_Wait(Boss01* this, PlayState* play) {
    s16 i;
    Player* player = GET_PLAYER(play);

    this->lookAtPlayer = true;

    if ((this->waitType == ODOLWA_WAIT_SPIN_SWORD) || (this->waitType == ODOLWA_WAIT_VERTICAL_HOP) ||
        (this->waitType == ODOLWA_WAIT_ARM_SWING_DANCE) || (this->waitType == ODOLWA_WAIT_THRUST_ATTACK)) {
        Actor_PlaySfx(&this->actor, NA_SE_EN_MIBOSS_VOICE1_OLD - SFX_FLAG);
    } else {
        Actor_PlaySfx(&this->actor, NA_SE_EN_MIBOSS_VOICE2_OLD - SFX_FLAG);
    }

    if ((this->waitType == ODOLWA_WAIT_READY) || (this->waitType == ODOLWA_WAIT_SPIN_SWORD) ||
        (this->waitType == ODOLWA_WAIT_VERTICAL_HOP) || (this->waitType == ODOLWA_WAIT_SHAKE_DANCE)) {
        this->canGuardOrEvade = true;
        this->swordAndShieldCollisionEnabled = true;
    }

    if (((this->waitType == ODOLWA_WAIT_VERTICAL_HOP) || (this->waitType == ODOLWA_WAIT_SIDE_TO_SIDE_HOP) ||
         (this->waitType == ODOLWA_WAIT_SIDE_TO_SIDE_DANCE)) &&
        Animation_OnFrame(&this->skelAnime, 6.0f)) {
        Actor_PlaySfx(&this->actor, NA_SE_EN_MIBOSS_GND1_OLD);
        for (i = 0; i < 3; i++) {
            Boss01_SpawnDustAtFeet(this, play, 0);
        }
    }

    if (this->waitType == ODOLWA_WAIT_SIDE_TO_SIDE_DANCE) {
        this->animMorphFrames2 = 0.0f;
        this->animMorphFrames1 = 0.0f;
    }

    this->swordState = ODOLWA_SWORD_STATE_ACTIVE;
    this->waitTimer++;

    // This will play "slash" sound effects at appropriate points in the thrust attack or double slash action.
    if (this->waitType == ODOLWA_WAIT_THRUST_ATTACK) {
        if (this->waitTimer == 7) {
            Actor_PlaySfx(&this->actor, NA_SE_EN_ANSATSUSYA_ENTRY);
        }
    } else if ((this->waitType == ODOLWA_WAIT_DOUBLE_SLASH) && (((this->waitTimer == 12)) || (this->waitTimer == 20))) {
        Actor_PlaySfx(&this->actor, NA_SE_EN_ANSATSUSYA_ENTRY);
    }

    SkelAnime_Update(&this->skelAnime);
    Math_ApproachS(&this->actor.world.rot.y, this->actor.yawTowardsPlayer, 0xA, 0x800);
    Math_ApproachZeroF(&this->actor.speed, 1.0f, 1.5f);

    // Whether or not Odolwa should jump if the player gets close was randomly decided earlier in Boss01_SetupWait, and
    // if it was decided that he *should* jump, this can interrupt any wait action.
    if ((this->actor.xzDistToPlayer <= 150.0f) && this->jumpIfPlayerIsClose) {
        Boss01_SetupJump(this, play, false);
        this->canGuardOrEvade = false;
    }

    // The thrust attack and double slash wait types select Odolwa's next action after their animations end rather than
    // waiting until the full 80 frames like every other wait type. The timer for the current action is not set to 0
    // here, though, which may cause some unintended behavior in Boss01_HorizontalSlash.
    if (((this->timers[TIMER_CURRENT_ACTION] == 0) && (this->waitType != ODOLWA_WAIT_THRUST_ATTACK) &&
         (this->waitType != ODOLWA_WAIT_DOUBLE_SLASH)) ||
        (Animation_OnFrame(&this->skelAnime, this->animEndFrame) &&
         ((this->waitType == ODOLWA_WAIT_THRUST_ATTACK) || (this->waitType == ODOLWA_WAIT_DOUBLE_SLASH)))) {
        if (this->actor.xzDistToPlayer <= 450.0f) {
            Boss01_SelectAttack(this, play, false);
        } else if (Rand_ZeroOne() < 0.5f) {
            Boss01_SetupJump(this, play, true);
        } else {
            Boss01_SetupRun(this, play);
        }
    }

    Boss01_SpawnDustAtFeet(this, play, 1);
    this->animMorphFrames1 = -2.0f;
    this->animMorphFrames2 = -4.0f;

    if (((this->waitType == ODOLWA_WAIT_SHAKE_DANCE) || (this->waitType == ODOLWA_WAIT_UP_AND_DOWN_DANCE) ||
         (this->waitType == ODOLWA_WAIT_DOUBLE_SLASH) || (this->waitType == ODOLWA_WAIT_SIDE_TO_SIDE_HOP)) &&
        (this->waitTimer == 30)) {
        Boss01_SpawnEffectRingOfFire((OdolwaEffect*)play->specialEffects, &player->actor.world.pos);
        this->timers[TIMER_CURRENT_ACTION] = 120;
    }

    if (((this->timers[TIMER_CURRENT_ACTION] % 16) == 0) && (this->waitType != ODOLWA_WAIT_READY) &&
        (sOdolwaBugCount < 5)) {
        Vec3f pos;
        Player* player2 = GET_PLAYER(play);
        s32 pad;

        if (Rand_ZeroOne() < 0.2f) {
            pos = player2->actor.world.pos;
        } else {
            pos.x = Rand_CenteredFloat(1200.0f);
            pos.z = Rand_CenteredFloat(1200.0f);
        }

        pos.y = 1200.0f;

        switch (this->waitType) {
            case ODOLWA_WAIT_SPIN_SWORD:
            case ODOLWA_WAIT_VERTICAL_HOP:
            case ODOLWA_WAIT_ARM_SWING_DANCE:
            case ODOLWA_WAIT_THRUST_ATTACK:
                Actor_Spawn(&play->actorCtx, play, ACTOR_BOSS_01, pos.x, pos.y, pos.z, 0, Rand_ZeroFloat(0x10000), 0,
                            ODOLWA_PARAMS(ODOLWA_TYPE_BUG));
                break;

            case ODOLWA_WAIT_SIDE_TO_SIDE_DANCE:
            case ODOLWA_WAIT_SPIN_DANCE:
            case ODOLWA_WAIT_JUMP_DANCE:
                Audio_PlaySfx_AtPos(&sFallingBlockSfxPos, NA_SE_EV_ROCK_FALL);
                Boss01_SpawnEffectFallingBlock((OdolwaEffect*)play->specialEffects, &pos, false);
                break;

            default:
                break;
        }
    }
}

void Boss01_SetupDazed(Boss01* this, PlayState* play) {
    Animation_MorphToLoop(&this->skelAnime, &gOdolwaStunAnim, this->animMorphFrames2);
    if (this->actionFunc != Boss01_Dazed) {
        this->timers[TIMER_CURRENT_ACTION] = 70;
        this->actionFunc = Boss01_Dazed;
    }

    this->canGuardOrEvade = false;
}

/**
 * Plays the dazed animation for 70 frames, leaving Odolwa vulnerable to attacks. This state is often referred to as
 * Odolwa being "stunned," though it was deliberately named something different here to avoid confusion with the state
 * where Odolwa turns blue and stops all his animations (e.g., when he is hit with a Deku Nut). The latter state is
 * consistently named "stunned" across all enemies in the codebase, so this function was given a different name to
 * signify it does something else.
 */
void Boss01_Dazed(Boss01* this, PlayState* play) {
    SkelAnime_Update(&this->skelAnime);
    Actor_PlaySfx(&this->actor, NA_SE_EN_COMMON_WEAKENED - SFX_FLAG);
    Math_ApproachZeroF(&this->actor.speed, 1.0f, 1.5f);
    if (this->timers[TIMER_CURRENT_ACTION] == 0) {
        Boss01_SetupWait(this, play, ODOLWA_WAIT_RANDOM);
    }

    this->canGuardOrEvade = false;
    Boss01_SpawnDustAtFeet(this, play, 1);
}

void Boss01_SetupSpinAttack(Boss01* this, PlayState* play) {
    Animation_MorphToLoop(&this->skelAnime, &gOdolwaSpinAttackAnim, this->animMorphFrames2);
    this->timers[TIMER_CURRENT_ACTION] = 120;
    this->actionFunc = Boss01_SpinAttack;
    this->canGuardOrEvade = false;
}

/**
 * Rapidly spin around and approach the player for 120 frames, spawning a new afterimage every frame.
 * Transitions back to waiting once the attack completes.
 */
void Boss01_SpinAttack(Boss01* this, PlayState* play) {
    SkelAnime_Update(&this->skelAnime);
    Math_ApproachF(&this->actor.speed, 7.0f, 1.0f, 1.5f);
    Math_ApproachS(&this->actor.world.rot.y, this->actor.yawTowardsPlayer, 4, 0x1000);

    if (Animation_OnFrame(&this->skelAnime, 5.0f)) {
        Actor_PlaySfx(&this->actor, NA_SE_EN_MIBOSS_SWORD_OLD);
        Actor_PlaySfx(&this->actor, NA_SE_EN_MIBOSS_ROLLING_OLD);
    }

    if (this->timers[TIMER_CURRENT_ACTION] == 0) {
        Boss01_SetupWait(this, play, ODOLWA_WAIT_RANDOM);
    }

    this->canGuardOrEvade = false;
    this->swordState = ODOLWA_SWORD_STATE_ACTIVE;
    this->swordAndShieldCollisionEnabled = true;
    Boss01_SpawnDustAtFeet(this, play, 1);
    this->afterimageSpawnFrameMask = 1;
}

void Boss01_SetupDanceBeforeAttack(Boss01* this, PlayState* play) {
    Animation_MorphToLoop(&this->skelAnime, &gOdolwaJumpDanceAnim, this->animMorphFrames2);
    this->timers[TIMER_CURRENT_ACTION] = 40;
    this->actionFunc = Boss01_DanceBeforeAttack;
    this->canGuardOrEvade = false;
}

/**
 * Dance for 40 frames, then select an attack. Odolwa can only end up in this state if he randomly chose to dance
 * instead of attack the last time Boss01_SelectAttack was called. This function calls Boss01_SelectAttack with the
 * mustAttack parameter set to true, ensuring that he will not dance again immediately after this.
 */
void Boss01_DanceBeforeAttack(Boss01* this, PlayState* play) {
    SkelAnime_Update(&this->skelAnime);
    Math_ApproachZeroF(&this->actor.speed, 1.0f, 1.5f);
    Actor_PlaySfx(&this->actor, NA_SE_EN_MIBOSS_VOICE2_OLD - SFX_FLAG);

    if (this->timers[TIMER_CURRENT_ACTION] == 0) {
        Boss01_SelectAttack(this, play, true);
    }

    this->canGuardOrEvade = false;
    this->swordAndShieldCollisionEnabled = true;
    Boss01_SpawnDustAtFeet(this, play, 1);
}

void Boss01_SetupRun(Boss01* this, PlayState* play) {
    Animation_MorphToLoop(&this->skelAnime, &gOdolwaRunAnim, this->animMorphFrames2);
    this->actionFunc = Boss01_Run;
    this->timers[TIMER_CURRENT_ACTION] = Rand_ZeroFloat(100.0f) + 50.0f;
    this->runTargetPosAngularVelocityY = 0.07f;

    if (Rand_ZeroOne() < 0.5f) {
        this->runTargetPosAngularVelocityY *= -1.0f;
    }

    this->actor.gravity = -3.0f;
}

/**
 * Run in a circle for 50-150 frames by chasing a rotating target around the room, then go back to waiting.
 */
void Boss01_Run(Boss01* this, PlayState* play) {
    Vec3f targetPos;
    f32 diffX;
    f32 diffZ;

    this->lookAtPlayer = true;
    SkelAnime_Update(&this->skelAnime);

    if ((this->actor.bgCheckFlags & BGCHECKFLAG_GROUND) &&
        (Animation_OnFrame(&this->skelAnime, 6.0f) || Animation_OnFrame(&this->skelAnime, 16.0f))) {
        this->actor.velocity.y = 10.0f;
        Actor_PlaySfx(&this->actor, NA_SE_EN_MIBOSS_GND1_OLD);
    }

    this->runTargetPosRotY += this->runTargetPosAngularVelocityY;
    Matrix_RotateYF(this->runTargetPosRotY, MTXMODE_NEW);
    Matrix_MultVecZ(450.0f, &targetPos);
    diffX = targetPos.x - this->actor.world.pos.x;
    diffZ = targetPos.z - this->actor.world.pos.z;
    Math_ApproachS(&this->actor.world.rot.y, Math_Atan2S_XY(diffZ, diffX), 0xA, 0x1000);
    Math_ApproachF(&this->actor.speed, 12.0f, 1.0f, 3.0f);

    if (this->timers[TIMER_CURRENT_ACTION] == 0) {
        Boss01_SetupWait(this, play, ODOLWA_WAIT_RANDOM);
    }

    Boss01_SpawnDustAtFeet(this, play, 3);
    this->swordState = ODOLWA_SWORD_STATE_ACTIVE;
    this->swordAndShieldCollisionEnabled = true;
}

/**
 * This starts the process making Odolwa jump, though the actual jump is divided into three different action functions.
 * The shouldPerformFallingSlash parameter controls whether Odolwa should try to perform a falling slash during the
 * middle part of the jump or if he should just jump normally.
 */
void Boss01_SetupJump(Boss01* this, PlayState* play, u8 shouldPerformFallingSlash) {
    this->shouldPerformFallingSlash = shouldPerformFallingSlash;
    Animation_MorphToLoop(&this->skelAnime, &gOdolwaCrouchAnim, this->animMorphFrames2);
    this->actionFunc = Boss01_JumpSquat;
    this->timers[TIMER_CURRENT_ACTION] = 5;
}

/**
 * Plays the crouch animation for 5 frames, then jumps off.
 */
void Boss01_JumpSquat(Boss01* this, PlayState* play) {
    s32 pad[2];
    u8 i;
    Vec3f additionalVelocity;
    f32 magnitude;

    SkelAnime_Update(&this->skelAnime);

    if (this->timers[TIMER_CURRENT_ACTION] == 0) {
        this->actionFunc = Boss01_Jump;
        Animation_MorphToPlayOnce(&this->skelAnime, &gOdolwaJumpAnim, this->animMorphFrames1);
        this->actor.velocity.y = 35.0f;
        this->actor.gravity = -2.5f;
        Matrix_RotateYS(this->actor.world.rot.y, MTXMODE_NEW);

        if (!this->shouldPerformFallingSlash) {
            magnitude = Rand_ZeroFloat(10.0f) + 10.0f;
        } else {
            magnitude = 12.0f;
        }

        Matrix_MultVecZ(magnitude, &additionalVelocity);
        this->additionalVelocityX = additionalVelocity.x;
        this->additionalVelocityZ = additionalVelocity.z;

        for (i = 0; i < 5; i++) {
            Boss01_SpawnDustAtFeet(this, play, 0);
        }

        Actor_PlaySfx(&this->actor, NA_SE_EN_MIBOSS_JUMP1);
        Actor_PlaySfx(&this->actor, NA_SE_EN_MIBOSS_JUMP2 - SFX_FLAG);
        this->disableCollisionTimer = 5;
    }

    this->swordAndShieldCollisionEnabled = true;
}

/**
 * As Odolwa moves through the air, this function will spawn an afterimage every other frame and rotate him to face the
 * player. If Odolwa should perform a falling slash, and if he's falling fast enough, then this function will stop
 * rotating him and transition him to the slash animation. Lastly, this function will transition Odolwa to his "landing"
 * action once he gets close enough to the ground.
 */
void Boss01_Jump(Boss01* this, PlayState* play) {
    SkelAnime_Update(&this->skelAnime);
    Actor_PlaySfx(&this->actor, NA_SE_EN_MIBOSS_JUMP2 - SFX_FLAG);

    if (!this->shouldPerformFallingSlash) {
        Math_ApproachS(&this->actor.world.rot.y, this->actor.yawTowardsPlayer, 5, 0x1000);
    } else if ((this->actor.velocity.y < -5.0f) && (this->prevJumpVelocityY >= -5.0f)) {
        Animation_MorphToPlayOnce(&this->skelAnime, &gOdolwaFallingSlashAnim, this->animMorphFrames1);
        Actor_PlaySfx(&this->actor, NA_SE_EN_MIBOSS_SWORD_OLD);
    }

    if (this->actor.world.pos.y < 40.0f) {
        this->actionFunc = Boss01_JumpLand;
        Animation_MorphToPlayOnce(&this->skelAnime, &gOdolwaCrouchAnim, this->animMorphFrames1);
        this->timers[TIMER_CURRENT_ACTION] = 5;
        this->landedFromJump = false;
    }

    this->swordState = ODOLWA_SWORD_STATE_ACTIVE;
    this->swordAndShieldCollisionEnabled = true;
    this->afterimageSpawnFrameMask = 2;
    this->prevJumpVelocityY = this->actor.velocity.y;
}

/**
 * Plays the crouch animation for five frames, then transitions either to selecting an attack or waiting based on
 * Odolwa's distance to the player.
 */
void Boss01_JumpLand(Boss01* this, PlayState* play) {
    u8 i;

    SkelAnime_Update(&this->skelAnime);

    if (!this->landedFromJump) {
        if (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND) {
            this->landedFromJump = true;
            for (i = 0; i < 5; i++) {
                Boss01_SpawnDustAtFeet(this, play, 0);
            }

            Actor_PlaySfx(&this->actor, NA_SE_EN_MIBOSS_GND1_OLD);
        }
    }

    if (!this->shouldPerformFallingSlash) {
        Math_ApproachS(&this->actor.world.rot.y, this->actor.yawTowardsPlayer, 4, 0x2800);
    }

    if (this->timers[TIMER_CURRENT_ACTION] == 0) {
        if (this->actor.xzDistToPlayer <= 450.0f) {
            Boss01_SelectAttack(this, play, false);
        } else {
            Boss01_SetupWait(this, play, ODOLWA_WAIT_RANDOM);
        }
    }

    this->swordState = ODOLWA_SWORD_STATE_ACTIVE;
    this->swordAndShieldCollisionEnabled = true;
}

void Boss01_SetupVerticalSlash(Boss01* this, PlayState* play) {
    Animation_MorphToPlayOnce(&this->skelAnime, &gOdolwaVerticalSlashAnim, this->animMorphFrames2);
    this->animEndFrame = Animation_GetLastFrame(&gOdolwaVerticalSlashAnim);
    this->actionFunc = Boss01_VerticalSlash;
    this->timers[TIMER_CURRENT_ACTION] = 20;
}

/**
 * Charges forward and performs a vertical slash. Transitions back to waiting once the attack completes.
 */
void Boss01_VerticalSlash(Boss01* this, PlayState* play) {
    Vec3f additionalVelocity;

    SkelAnime_Update(&this->skelAnime);

    if ((this->skelAnime.curFrame >= 10.0f) && (this->skelAnime.curFrame <= 15.0f)) {
        this->isPerformingVerticalSlash = true;
    }

    if ((this->timers[TIMER_CURRENT_ACTION] >= 7) && (this->timers[TIMER_CURRENT_ACTION] < 13)) {
        Matrix_RotateYF(BINANG_TO_RAD_ALT(this->actor.world.rot.y), MTXMODE_NEW);
        Matrix_MultVecZ(20.0f, &additionalVelocity);
        this->additionalVelocityX = additionalVelocity.x;
        this->additionalVelocityZ = additionalVelocity.z;
        Boss01_SpawnDustAtFeet(this, play, 0);
    }

    sOdolwaSwordTrailPosX = 0.0f;
    sOdolwaSwordTrailPosY = 90.0f;
    sOdolwaSwordTrailPosZ = -70.0f;
    sOdolwaSwordTrailRotX = 0.4712388f;
    sOdolwaSwordTrailRotY = M_PIf;
    sOdolwaSwordTrailRotZ = 1.7278761f;

    if (Animation_OnFrame(&this->skelAnime, 12.0f)) {
        sOdolwaSwordTrailAlpha = 255.0f;
        sSwordTrailAngularRangeDivisor = 100.0f;
    }

    if (Animation_OnFrame(&this->skelAnime, 13.0f)) {
        sSwordTrailAngularRangeDivisor = 20.0f;
    }

    if (Animation_OnFrame(&this->skelAnime, 14.0f)) {
        sSwordTrailAngularRangeDivisor = 7.0f;
    }

    if (Animation_OnFrame(&this->skelAnime, 7.0f)) {
        Actor_PlaySfx(&this->actor, NA_SE_EN_MIBOSS_DASH_OLD);
    }

    if (Animation_OnFrame(&this->skelAnime, 10.0f)) {
        Actor_PlaySfx(&this->actor, NA_SE_EN_MIBOSS_SWORD_OLD);
    }

    if (Animation_OnFrame(&this->skelAnime, this->animEndFrame)) {
        Boss01_SetupWait(this, play, ODOLWA_WAIT_RANDOM);
        this->additionalVelocityZ = 0.0f;
        this->additionalVelocityX = 0.0f;
    }

    this->swordState = ODOLWA_SWORD_STATE_ACTIVE;
    this->swordAndShieldCollisionEnabled = true;
}

void Boss01_SetupHorizontalSlash(Boss01* this, PlayState* play) {
    Animation_MorphToPlayOnce(&this->skelAnime, &gOdolwaHorizontalSlashAnim, this->animMorphFrames2);
    this->animEndFrame = Animation_GetLastFrame(&gOdolwaHorizontalSlashAnim);
    this->actionFunc = Boss01_HorizontalSlash;
}

/**
 * Stands in place and performs a horizontal slash. Transitions back to waiting once the attack completes.
 */
void Boss01_HorizontalSlash(Boss01* this, PlayState* play) {
    Vec3f additionalVelocity;

    SkelAnime_Update(&this->skelAnime);

    if ((this->skelAnime.curFrame >= 10.0f) && (this->skelAnime.curFrame <= 15.0f)) {
        this->swordState = ODOLWA_SWORD_STATE_HORIZONTAL_SLASH;
    }

    // This code is written very strangely and might never have been intended to run. The timer values checked here seem
    // copied from Boss01_VerticalSlash, and they make sense there to control when Odolwa should dash forward. Unlike
    // Boss01_VerticalSlash, however, this function uses Matrix_MultZero to initialize additionalVelocity; in other
    // words, the additional velocity in all directions will be zero, and Odolwa will stay in place.
    //
    // Additionally, Boss01_SetupHorizontalSlash doesn't initialize this->timers[TIMER_CURRENT_ACTION] to anything, so
    // most of the time, the timer will just be zero, and this code will never run in the first place. However, if
    // Odolwa did a double slash or thrust attack wait action in Boss01_Wait, it is possible to enter this function with
    // a non-zero current action timer, potentially allowing this code to run. The only consequence of this is that
    // Odolwa will sometimes spawn dust at his feet during a horizontal slash when he was probably never supposed to.
    if ((this->timers[TIMER_CURRENT_ACTION] >= 7) && (this->timers[TIMER_CURRENT_ACTION] < 13)) {
        Matrix_RotateYF(BINANG_TO_RAD_ALT(this->actor.world.rot.y), MTXMODE_NEW);
        Matrix_MultZero(&additionalVelocity);
        this->additionalVelocityX = additionalVelocity.x;
        this->additionalVelocityZ = additionalVelocity.z;
        Boss01_SpawnDustAtFeet(this, play, 0);
    }

    sOdolwaSwordTrailPosX = 0.0f;
    sOdolwaSwordTrailPosY = 140.0f;
    sOdolwaSwordTrailPosZ = 0.0f;
    sOdolwaSwordTrailRotX = 0.4712388f;
    sOdolwaSwordTrailRotY = 0.0f;
    sOdolwaSwordTrailRotZ = 0.0f;

    if (Animation_OnFrame(&this->skelAnime, 12.0f)) {
        sOdolwaSwordTrailAlpha = 255.0f;
        sSwordTrailAngularRangeDivisor = 100.0f;
    }

    if (Animation_OnFrame(&this->skelAnime, 13.0f)) {
        sSwordTrailAngularRangeDivisor = 20.0f;
    }

    if (Animation_OnFrame(&this->skelAnime, 14.0f)) {
        sSwordTrailAngularRangeDivisor = 7.0f;
    }

    if (Animation_OnFrame(&this->skelAnime, 10.0f)) {
        Actor_PlaySfx(&this->actor, NA_SE_EN_MIBOSS_SWORD_OLD);
    }

    if (Animation_OnFrame(&this->skelAnime, this->animEndFrame)) {
        Boss01_SetupWait(this, play, ODOLWA_WAIT_RANDOM);
        this->additionalVelocityZ = 0.0f;
        this->additionalVelocityX = 0.0f;
    }

    this->swordAndShieldCollisionEnabled = true;
}

/**
 * Prepares Odolwa to guard using either his shield or sword depending on the supplied parameter. The difference is
 * entirely aesthetic, with him behaving in the exact same way regardless of which type of guard is chosen.
 */
void Boss01_SetupGuard(Boss01* this, PlayState* play, u8 guardUsingSword) {
    if (this->actionFunc != Boss01_Guard) {
        if (!guardUsingSword) {
            Animation_MorphToPlayOnce(&this->skelAnime, &gOdolwaShieldGuardAnim, this->animMorphFrames1);
            this->animEndFrame = Animation_GetLastFrame(&gOdolwaShieldGuardAnim);
        } else {
            Animation_MorphToPlayOnce(&this->skelAnime, &gOdolwaSwordGuardAnim, this->animMorphFrames1);
            this->animEndFrame = Animation_GetLastFrame(&gOdolwaSwordGuardAnim);
        }

        this->actionFunc = Boss01_Guard;
    }

    this->timers[TIMER_CURRENT_ACTION] = 5;
}

/**
 * Plays a guard animation (either blocking with his shield or his sword) for five frames, then transitions back to
 * waiting.
 */
void Boss01_Guard(Boss01* this, PlayState* play) {
    SkelAnime_Update(&this->skelAnime);
    Math_ApproachS(&this->actor.world.rot.y, this->actor.yawTowardsPlayer, 2, 0x500);
    Math_ApproachZeroF(&this->actor.speed, 1.0f, 1.0f);
    if (this->timers[TIMER_CURRENT_ACTION] != 0) {
        this->swordAndShieldCollisionEnabled = true;
    } else {
        Boss01_SetupWait(this, play, ODOLWA_WAIT_RANDOM);
    }

    this->canGuardOrEvade = true;
}

void Boss01_SetupKick(Boss01* this, PlayState* play) {
    Animation_MorphToPlayOnce(&this->skelAnime, &gOdolwaKickAnim, this->animMorphFrames2);
    this->animEndFrame = Animation_GetLastFrame(&gOdolwaKickAnim);
    this->actionFunc = Boss01_Kick;
}

/**
 * Plays the kick animation to completion, then transitions back to waiting.
 */
void Boss01_Kick(Boss01* this, PlayState* play) {
    SkelAnime_Update(&this->skelAnime);
    Math_ApproachZeroF(&this->actor.speed, 1.0f, 1.0f);
    if (Animation_OnFrame(&this->skelAnime, this->animEndFrame)) {
        Boss01_SetupWait(this, play, ODOLWA_WAIT_RANDOM);
    }

    this->kickAndShieldBashCollisionEnabled = true;
}

void Boss01_SetupShieldBash(Boss01* this, PlayState* play) {
    Animation_MorphToPlayOnce(&this->skelAnime, &gOdolwaShieldBashAnim, this->animMorphFrames2);
    this->animEndFrame = Animation_GetLastFrame(&gOdolwaShieldBashAnim);
    this->actionFunc = Boss01_ShieldBash;
}

/**
 * Plays the shield bash animation to completion, then transitions back to waiting.
 */
void Boss01_ShieldBash(Boss01* this, PlayState* play) {
    SkelAnime_Update(&this->skelAnime);
    Math_ApproachZeroF(&this->actor.speed, 1.0f, 1.0f);
    if (Animation_OnFrame(&this->skelAnime, this->animEndFrame)) {
        Boss01_SetupWait(this, play, ODOLWA_WAIT_RANDOM);
    }

    this->kickAndShieldBashCollisionEnabled = true;
}

/**
 * If Odolwa is not currently in the "damaged" state (where he's crouched on one knee and breathing heavily), this
 * function will transition him to that state and start a 20 frame timer; when this timer reaches 0, he will recover and
 * jump away. However, if he is already in the "damaged" state, and if the player attacked him with an attack that has
 * the ODOLWA_DMGEFF_DAMAGE_TIMER_CHECK damage effect, then one of two things will happen:
 * - If the attack hit while his current action timer is 5 or more, the timer will be set to 20 again; this will keep
 *   Odolwa in the "damaged" state for longer and allow the player to attack him more.
 * - If the attack hit while his current action timer is 4 or less, Odolwa will disable all of his collision for 20
 *   frames to ensure that the player cannot hit him with subsequent attacks.
 */
void Boss01_SetupDamaged(Boss01* this, PlayState* play, u8 damageEffect) {
    if (this->actionFunc != Boss01_Damaged) {
        this->timers[TIMER_CURRENT_ACTION] = 20;
        Animation_MorphToPlayOnce(&this->skelAnime, &gOdolwaDamagedStartAnim, 0.0f);
        this->animEndFrame = Animation_GetLastFrame(&gOdolwaDamagedStartAnim);
        this->actionFunc = Boss01_Damaged;
    } else if (damageEffect == ODOLWA_DMGEFF_DAMAGE_TIMER_CHECK) {
        if (this->timers[TIMER_CURRENT_ACTION] > 5) {
            this->disableCollisionTimer = 20;
        } else {
            this->timers[TIMER_CURRENT_ACTION] = 20;
        }
    }
}

/**
 * Plays Odolwa's damaged animation until the current action timer reaches 0, then makes him jump away. If the player
 * attacks Odolwa while he's "downed" in this state, the current action timer can be manipulated in Boss01_SetupDamaged,
 * so it's possible for Odolwa to stay in this state for a long time with well-timed attacks.
 */
void Boss01_Damaged(Boss01* this, PlayState* play) {
    SkelAnime_Update(&this->skelAnime);
    Math_ApproachZeroF(&this->actor.speed, 1.0f, 1.0f);

    if (Animation_OnFrame(&this->skelAnime, this->animEndFrame)) {
        Animation_MorphToLoop(&this->skelAnime, &gOdolwaDamagedLoopAnim, this->animMorphFrames2);
        this->animEndFrame = 1000.0f;
    }

    if (this->timers[TIMER_CURRENT_ACTION] == 0) {
        Boss01_SetupJump(this, play, false);
    }

    Boss01_SpawnDustAtFeet(this, play, 1);
}

void Boss01_UpdateDamage(Boss01* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    u8 damage;
    s32 i;

    if (this->shieldCollider.elements[ODOLWA_SHIELD_COLLIDER_SHIELD].base.acElemFlags & ACELEM_HIT) {
        this->bodyInvincibilityTimer = 5;
        if (this->damagedTimer == 0) {
            ColliderElement* acHitElem = this->shieldCollider.elements[ODOLWA_SHIELD_COLLIDER_SHIELD].base.acHitElem;

            if (acHitElem->atDmgInfo.dmgFlags == DMG_SWORD_BEAM) {
                Actor_Spawn(&play->actorCtx, play, ACTOR_EN_CLEAR_TAG, this->actor.focus.pos.x, this->actor.focus.pos.y,
                            this->actor.focus.pos.z, 0, 0, 3, CLEAR_TAG_PARAMS(CLEAR_TAG_LARGE_LIGHT_RAYS));
                Actor_PlaySfx(&this->actor, NA_SE_IT_SHIELD_BOUND);
                this->damagedTimer = 5;
            }
        }
    } else if (this->damagedTimer == 0) {
        for (i = 0; i < ODOLWA_SWORD_COLLIDER_MAX; i++) {
            if (this->swordCollider.elements[i].base.atElemFlags & ATELEM_HIT) {
                this->swordCollider.elements[i].base.atElemFlags &= ~ATELEM_HIT;
                player->pushedYaw = this->actor.yawTowardsPlayer;
                player->pushedSpeed = 15.0f;
            }
        }

        for (i = 0; i < ODOLWA_KICK_AND_SHIELD_BASH_COLLIDER_MAX; i++) {
            if (this->kickAndShieldBashCollider.elements[i].base.atElemFlags & ATELEM_HIT) {
                this->kickAndShieldBashCollider.elements[i].base.atElemFlags &= ~ATELEM_HIT;
                player->pushedYaw = this->actor.yawTowardsPlayer;
                player->pushedSpeed = 20.0f;
            }
        }

        for (i = 0; i < ODOLWA_COLLIDER_BODYPART_MAX; i++) {
            if (this->bodyCollider.elements[i].base.acElemFlags & ACELEM_HIT) {
                this->bodyCollider.elements[i].base.acElemFlags &= ~ACELEM_HIT;

                switch (this->actor.colChkInfo.damageEffect) {
                    case ODOLWA_DMGEFF_FREEZE:
                        this->drawDmgEffState = ODOLWA_DRAW_DMGEFF_STATE_FROZEN_INIT;
                        goto stunned;

                    case ODOLWA_DMGEFF_FIRE:
                        this->drawDmgEffState = ODOLWA_DRAW_DMGEFF_STATE_FIRE_INIT;
                        break;

                    case ODOLWA_DMGEFF_LIGHT_ORB:
                        this->drawDmgEffState = ODOLWA_DRAW_DMGEFF_STATE_LIGHT_ORB_INIT;
                        Actor_Spawn(&play->actorCtx, play, ACTOR_EN_CLEAR_TAG, this->actor.focus.pos.x,
                                    this->actor.focus.pos.y, this->actor.focus.pos.z, 0, 0, 0,
                                    CLEAR_TAG_PARAMS(CLEAR_TAG_LARGE_LIGHT_RAYS));
                        break;

                    case ODOLWA_DMGEFF_ELECTRIC_STUN:
                        this->drawDmgEffState = ODOLWA_DRAW_DMGEFF_STATE_ELECTRIC_SPARKS_INIT;
                        goto stunned;

                    case ODOLWA_DMGEFF_BLUE_LIGHT_ORB:
                        this->drawDmgEffState = ODOLWA_DRAW_DMGEFF_STATE_BLUE_LIGHT_ORB_INIT;
                        Actor_Spawn(&play->actorCtx, play, ACTOR_EN_CLEAR_TAG, this->actor.focus.pos.x,
                                    this->actor.focus.pos.y, this->actor.focus.pos.z, 0, 0, 3,
                                    CLEAR_TAG_PARAMS(CLEAR_TAG_LARGE_LIGHT_RAYS));
                        break;

                    case ODOLWA_DMGEFF_STUN:
                    stunned:
                        Boss01_SetupStunned(this, play);
                        this->damagedTimer = 15;
                        Actor_PlaySfx(&this->actor, NA_SE_EN_COMMON_FREEZE);
                        this->canGuardOrEvade = false;
                        return;

                    default:
                        break;
                }

                damage = this->actor.colChkInfo.damage;
                sMothSwarm->unk_148 = 0;

                if (this->actor.colChkInfo.damageEffect == ODOLWA_DMGEFF_STUN) {
                    //! @bug: unreachable code. If Odolwa's damage effect is ODOLWA_DMGEFF_STUN, we early-return out of
                    //! the function in the above switch statement.
                    Boss01_SetupStunned(this, play);
                    this->damagedTimer = 15;
                } else if (this->actor.colChkInfo.damageEffect == ODOLWA_DMGEFF_DAZE) {
                    Boss01_SetupDazed(this, play);
                    Audio_PlaySfx_AtPos(&sOdolwaDamageSfxPos, NA_SE_EN_MIBOSS_DAMAGE_OLD);
                    this->damagedTimer = 15;
                } else {
                    this->damagedFlashTimer = 15;
                    this->damagedTimer = 5;
                    this->actor.colChkInfo.health -= damage;
                    if ((s8)this->actor.colChkInfo.health <= 0) {
                        Boss01_SetupDeathCutscene(this, play);
                        Actor_PlaySfx(&this->actor, NA_SE_EN_MIBOSS_DEAD_OLD);
                        Enemy_StartFinishingBlow(play, &this->actor);
                        GameInteractor_ExecuteOnBossDefeated(this->actor.id); // 2S2H Time Splits
                    } else {
                        Boss01_SetupDamaged(this, play, this->actor.colChkInfo.damageEffect);
                        Audio_PlaySfx_AtPos(&sOdolwaDamageSfxPos, NA_SE_EN_MIBOSS_DAMAGE_OLD);
                    }
                }

                this->canGuardOrEvade = false;
                break;
            }
        }
    }
}

void Boss01_SetupSummonMoths(Boss01* this, PlayState* play) {
    Animation_MorphToLoop(&this->skelAnime, &gOdolwaMothSummonDanceAnim, -5.0f);
    this->animEndFrame = Animation_GetLastFrame(&gOdolwaMothSummonDanceAnim);
    this->actionFunc = Boss01_SummonMoths;
    this->summonMothsTimer = 0;
}

/**
 * Plays a specific dancing animation and summons moths after 30 frames. After 131 frames pass, Odolwa will transition
 * back to waiting; notably, this function always transitions him to a specific wait rather than choosing a random wait
 * action like every other non-cutscene function.
 */
void Boss01_SummonMoths(Boss01* this, PlayState* play) {
    this->summonMothsTimer++;
    SkelAnime_Update(&this->skelAnime);
    Actor_PlaySfx(&this->actor, NA_SE_EN_MIBOSS_VOICE3_OLD - SFX_FLAG);

    if (Animation_OnFrame(&this->skelAnime, this->animEndFrame)) {
        this->skelAnime.curFrame = this->animEndFrame - 20.0f;
    }

    if (this->summonMothsTimer >= 30) {
        sMothSwarm->actor.world.pos.x = this->bodyPartsPos[ODOLWA_BODYPART_HEAD].x;
        sMothSwarm->actor.world.pos.y = this->bodyPartsPos[ODOLWA_BODYPART_HEAD].y;
        sMothSwarm->actor.world.pos.z = this->bodyPartsPos[ODOLWA_BODYPART_HEAD].z;
        sMothSwarm->actor.world.rot.y = this->actor.world.rot.y;

        if (this->summonMothsTimer == 30) {
            sMothSwarm->unk_148 = 100;
            Actor_PlaySfx(&this->actor, NA_SE_SY_TRANSFORM_MASK_FLASH);
        }
    }

    Math_ApproachS(&this->actor.world.rot.y, this->actor.yawTowardsPlayer, 5, 0x500);
    Math_ApproachZeroF(&this->actor.speed, 1.0f, 1.0f);
    if (this->summonMothsTimer >= 131) {
        Boss01_SetupWait(this, play, ODOLWA_WAIT_SPIN_DANCE);
    }
}

void Boss01_SetupDeathCutscene(Boss01* this, PlayState* play) {
    Animation_MorphToPlayOnce(&this->skelAnime, &gOdolwaDeathAnim, this->animMorphFrames1);
    this->animEndFrame = Animation_GetLastFrame(&gOdolwaDeathAnim);
    this->actionFunc = Boss01_DeathCutscene;
    Actor_PlaySfx(&this->actor, NA_SE_EN_DAIOCTA_DAMAGE);
    this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
    this->disableCollisionTimer = 1000;
    this->cutsceneTimer = 0;
    this->cutsceneState = ODOLWA_DEATH_CS_STATE_STARTED;
    SEQCMD_STOP_SEQUENCE(SEQ_PLAYER_BGM_MAIN, 1);
    sMothSwarm->unk_144 = 250;
    Actor_ChangeCategory(play, &play->actorCtx, &sMothSwarm->actor, ACTORCAT_BOSS);
}

/**
 * Handles the entirety of Odolwa's death cutscene, including manipulating the camera, starting the "boss defeated"
 * theme, spawning the Heart Container and blue warp, surrounding Odolwa with flames, etc. Notably, the main Odolwa
 * instance is *not* killed in this function, and it stays in this state forever. Instead, Odolwa is teleported far
 * above the ceiling so the player cannot see or interact with him.
 */
void Boss01_DeathCutscene(Boss01* this, PlayState* play) {
    Vec3f subCamOffset;
    f32 diffX;
    f32 diffZ;
    Camera* mainCam = Play_GetCamera(play, CAM_ID_MAIN);

    this->disableCollisionTimer = 1000;
    this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
    SkelAnime_Update(&this->skelAnime);
    this->cutsceneTimer++;
    Math_ApproachZeroF(&this->actor.speed, 1.0f, 1.0f);

    switch (this->cutsceneState) {
        case ODOLWA_DEATH_CS_STATE_STARTED:
            if (CutsceneManager_GetCurrentCsId() != CS_ID_NONE) {
                break;
            }

            Cutscene_StartManual(play, &play->csCtx);
            Player_SetCsActionWithHaltedActors(play, &this->actor, PLAYER_CSACTION_1);
            this->subCamId = Play_CreateSubCamera(play);
            Play_ChangeCameraStatus(play, CAM_ID_MAIN, CAM_STATUS_WAIT);
            Play_ChangeCameraStatus(play, this->subCamId, CAM_STATUS_ACTIVE);
            this->cutsceneTimer = 0;
            this->cutsceneState = ODOLWA_DEATH_CS_STATE_PLAY_ANIM_AND_FALL_FORWARD;
            this->subCamEye.x = mainCam->eye.x;
            this->subCamEye.y = mainCam->eye.y;
            this->subCamEye.z = mainCam->eye.z;
            this->subCamAt.x = mainCam->at.x;
            this->subCamAt.y = mainCam->at.y;
            this->subCamAt.z = mainCam->at.z;
            diffX = this->subCamEye.x - this->actor.world.pos.x;
            diffZ = this->subCamEye.z - this->actor.world.pos.z;
            this->deathCsInitialSubCamRotY = Math_Atan2F_XY(diffZ, diffX);
            this->deathCsSubCamRotY = -0.5f;
            // fallthrough
        case ODOLWA_DEATH_CS_STATE_PLAY_ANIM_AND_FALL_FORWARD:
            if (this->cutsceneTimer < 15) {
                Math_ApproachF(&this->actor.world.pos.x, 0.0f, 0.1f, 5.0f);
                Math_ApproachF(&this->actor.world.pos.z, 0.0f, 0.1f, 5.0f);
            }

            if (this->cutsceneTimer == 70) {
                SEQCMD_PLAY_SEQUENCE(SEQ_PLAYER_BGM_MAIN, 0, NA_BGM_CLEAR_BOSS | SEQ_FLAG_ASYNC);
            }

            if (this->cutsceneTimer == 71) {
                Actor_PlaySfx(&this->actor, NA_SE_EN_MIBOSS_GND2_OLD);
                this->cutsceneState = ODOLWA_DEATH_CS_STATE_BURST_INTO_FLAMES_AND_SHRINK;
                this->cutsceneTimer = 0;
            }
            // fallthrough
        case ODOLWA_DEATH_CS_STATE_BURST_INTO_FLAMES_AND_SHRINK:
            Math_ApproachF(&this->deathCsSubCamRotY, 1.3f, 0.1f, 0.008f);
            subCamOffset.x = 0.0f;
            subCamOffset.y = 30.0f;
            subCamOffset.z = 300.0f;
            Matrix_RotateYF(this->deathCsInitialSubCamRotY + this->deathCsSubCamRotY, MTXMODE_NEW);
            Matrix_MultVec3f(&subCamOffset, &this->subCamEyeNext);
            this->subCamEyeNext.x += this->pelvisPos.x;
            this->subCamEyeNext.y += this->pelvisPos.y;
            this->subCamEyeNext.z += this->pelvisPos.z;
            Math_ApproachF(&this->subCamEye.x, this->subCamEyeNext.x, 0.1f, 40.0f);
            Math_ApproachF(&this->subCamEye.y, this->subCamEyeNext.y, 0.1f, 40.0f);
            Math_ApproachF(&this->subCamEye.z, this->subCamEyeNext.z, 0.1f, 40.0f);
            Math_ApproachF(&this->subCamAt.x, this->pelvisPos.x, 0.1f, 70.0f);
            Math_ApproachF(&this->subCamAt.y, this->pelvisPos.y + 50.0f, 0.1f, 70.0f);
            Math_ApproachF(&this->subCamAt.z, this->pelvisPos.z, 0.1f, 70.0f);

            if (this->cutsceneTimer >= 71) {
                s16 i;
                s16 fireCount;
                Vec3f fireAccel;
                Vec3f firePos;
                s16 bodyPartIndex;

                fireAccel = gZeroVec3f;
                fireAccel.y = 0.03f;

                if (this->cutsceneTimer > 80) {
                    Math_ApproachZeroF(&this->actor.scale.y, 0.1f, this->deathShrinkSpeed * 0.00075f);
                    Math_ApproachF(&this->deathShrinkSpeed, 1.0f, 1.0f, 0.01f);
                }

                if (this->cutsceneTimer > 120) {
                    fireCount = 1;
                } else {
                    fireCount = 2;
                }

                for (i = 0; i < fireCount; i++) {
                    bodyPartIndex = Rand_ZeroFloat(14.9f);
                    firePos.x = this->bodyPartsPos[bodyPartIndex].x + Rand_CenteredFloat(40.0f);
                    firePos.y = this->bodyPartsPos[bodyPartIndex].y - 10.0f;
                    firePos.z = this->bodyPartsPos[bodyPartIndex].z + Rand_CenteredFloat(40.0f);
                    EffectSsKFire_Spawn(play, &firePos, &gZeroVec3f, &fireAccel, Rand_ZeroFloat(30.0f) + 30.0f, 0);
                }

                Actor_PlaySfx(&this->actor, NA_SE_EN_COMMON_EXTINCT_LEV - SFX_FLAG);
            }

            if (this->cutsceneTimer == 71) {
                Actor_Spawn(&play->actorCtx, play, ACTOR_ITEM_B_HEART, this->actor.focus.pos.x, this->actor.focus.pos.y,
                            this->actor.focus.pos.z, 0, 0, 0, BHEART_PARAM_NORMAL);
            }

            if (this->cutsceneTimer == 180) {
                static f32 sBlueWarpSpawnsX[] = { 0.0f, 350.0f, -350.0f, 350.0f, -350.0f };
                static f32 sBlueWarpSpawnsZ[] = { 0.0f, 350.0f, 350.0f, -350.0f, -350.0f };
                Player* player = GET_PLAYER(play);
                f32 warpX;
                f32 warpZ;
                s32 i;

                for (i = 0; i < ARRAY_COUNT(sBlueWarpSpawnsX); i++) {
                    warpX = sBlueWarpSpawnsX[i];
                    warpZ = sBlueWarpSpawnsZ[i];

                    if (((fabsf(warpX - this->actor.focus.pos.x) < 220.0f) &&
                         (fabsf(warpZ - this->actor.focus.pos.z) < 220.0f)) ||
                        ((fabsf(warpX - player->actor.world.pos.x) < 220.0f) &&
                         (fabsf(warpZ - player->actor.world.pos.z) < 220.0f))) {
                    } else {
                        break;
                    }
                }

                Actor_SpawnAsChild(&play->actorCtx, &this->actor, play, ACTOR_DOOR_WARP1, warpX, 0.0f, warpZ, 0, 0, 0,
                                   ENDOORWARP1_FF_1);
                this->cutsceneState = ODOLWA_DEATH_CS_STATE_DONE;
                mainCam->eye = this->subCamEye;
                mainCam->eyeNext = this->subCamEye;
                mainCam->at = this->subCamAt;
                func_80169AFC(play, this->subCamId, 0);
                this->subCamId = SUB_CAM_ID_DONE;
                Cutscene_StopManual(play, &play->csCtx);
                Player_SetCsActionWithHaltedActors(play, &this->actor, PLAYER_CSACTION_END);
                this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
            }
            break;

        case ODOLWA_DEATH_CS_STATE_DONE:
            this->actor.world.pos.y = 5000.0f;
            break;
    }

    if (this->subCamId != SUB_CAM_ID_DONE) {
        Vec3f at;

        // It seems like this code was copied from Boss01_IntroCutscene, since this function doesn't set
        // screenShakeMagnitude to anything, and thus it is always 0.0f; all of the code involving screen
        // shake here could be removed, since it is effectively useless.
        this->screenShakeOffsetY = Math_CosS(play->gameplayFrames * 0x8000) * this->screenShakeMagnitude;
        Math_ApproachZeroF(&this->screenShakeMagnitude, 1.0f, 0.75f);
        at = this->subCamAt;
        at.y += this->screenShakeOffsetY;
        Play_SetCameraAtEye(play, this->subCamId, &at, &this->subCamEye);
    }
}

void Boss01_SetupStunned(Boss01* this, PlayState* play) {
    this->actionFunc = Boss01_Stunned;
    this->timers[TIMER_CURRENT_ACTION] = 40;
    Actor_SetColorFilter(&this->actor, COLORFILTER_COLORFLAG_BLUE, 120, COLORFILTER_BUFFLAG_OPA, 40);
}

/**
 * Turns Odolwa blue, disables all animations, and makes him wait in place for 40 frames, after which he will transition
 * to waiting. This is not to be confused with his "dazed" state, which is commonly referred to as Odolwa "being
 * stunned"; this form of stun, where the enemy turns blue and stops their animations, is very common among regular
 * enemies, but it's very rare to see on a boss, so the term "stunned" was used here for consistency with other enemies.
 */
void Boss01_Stunned(Boss01* this, PlayState* play) {
    Math_ApproachZeroF(&this->actor.speed, 1.0f, 1.0f);
    if (this->timers[TIMER_CURRENT_ACTION] == 0) {
        Boss01_SetupWait(this, play, ODOLWA_WAIT_RANDOM);
    }
}

/**
 * Spawns two ice shards on all of Odolwa's body parts that fly off in random directions.
 */
void Boss01_Thaw(Boss01* this, PlayState* play) {
    static Color_RGBA8 sIcePrimColor = { 170, 255, 255, 255 };
    static Color_RGBA8 sIceEnvColor = { 200, 200, 255, 255 };
    static Vec3f sIceAccel = { 0.0f, -1.0f, 0.0f };
    Vec3f pos;
    Vec3f velocity;
    s32 i;

    SoundSource_PlaySfxAtFixedWorldPos(play, this->bodyPartsPos, 30, NA_SE_EV_ICE_BROKEN);

    for (i = 0; i < ODOLWA_BODYPART_MAX * 2; i++) {
        velocity.x = Rand_CenteredFloat(7.0f);
        velocity.z = Rand_CenteredFloat(7.0f);
        velocity.y = Rand_ZeroFloat(6.0f) + 4.0f;

        pos.x = this->bodyPartsPos[i / 2].x + velocity.x;
        pos.y = this->bodyPartsPos[i / 2].y + velocity.y;
        pos.z = this->bodyPartsPos[i / 2].z + velocity.z;

        EffectSsEnIce_Spawn(play, &pos, Rand_ZeroFloat(1.0f) + 1.5f, &velocity, &sIceAccel, &sIcePrimColor,
                            &sIceEnvColor, 30);
    }
}

/**
 * Returns true if Odolwa's model is rotated such that he is looking at the player *and* if the player's model is
 * rotated such that they are looking at Odolwa.
 */
s32 Boss01_ArePlayerAndActorFacing(Boss01* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    if ((ABS_ALT(BINANG_SUB(this->actor.yawTowardsPlayer, this->actor.shape.rot.y)) < 0x3000) &&
        (ABS_ALT(BINANG_SUB(this->actor.yawTowardsPlayer, BINANG_ROT180(player->actor.shape.rot.y))) < 0x3000)) {
        return true;
    }

    return false;
}

void Boss01_Update(Actor* thisx, PlayState* play2) {
    Boss01* this = (Boss01*)thisx;
    PlayState* play = play2;
    s32 i;
    Player* player = GET_PLAYER(play);
    f32 diffX;
    f32 diffY;
    f32 diffZ;
    s16 targetHeadRotY;
    s16 targetHeadRotX;
    s32 pad;

    if (ODOLWA_GET_TYPE(&this->actor) == ODOLWA_TYPE_AFTERIMAGE) {
        if (KREG(63) == 0) {
            DECR(this->timers[TIMER_AFTERIMAGE_DESPAWN]);
            this->actionFunc(this, play);
        }
        return;
    }

    Math_Vec3f_Copy(&sOdolwaDamageSfxPos, &this->actor.projectedPos);

    play->envCtx.lightSetting = 0;
    play->envCtx.prevLightSetting = 1;
    Math_ApproachZeroF(&play->envCtx.lightBlend, 1.0f, 0.03f);

    this->frameCounter++;

    if (KREG(63) == 0) {
        this->phaseFrameCounter++;
        if (this->phaseFrameCounter == 800) {
            sOdolwa->actor.hintId = TATL_HINT_ID_ODOLWA_CLOSE_TO_PHASE_TWO;
        }

        this->canGuardOrEvade = false;
        this->swordAndShieldCollisionEnabled = false;
        this->isPerformingVerticalSlash = false;
        this->afterimageSpawnFrameMask = 0;
        this->kickAndShieldBashCollisionEnabled = false;
        this->swordState = ODOLWA_SWORD_STATE_INACTIVE;

        for (i = 0; i < ARRAY_COUNT(this->timers); i++) {
            DECR(this->timers[i]);
        }

        DECR(this->damagedTimer);
        DECR(this->damagedFlashTimer);

        this->actor.flags |= ACTOR_FLAG_ATTENTION_ENABLED;
        this->actionFunc(this, play);
        Actor_MoveWithGravity(&this->actor);
        this->actor.world.pos.x += this->additionalVelocityX;
        this->actor.world.pos.z += this->additionalVelocityZ;
    }

    Actor_UpdateBgCheckInfo(play, &this->actor, 50.0f, 150.0f, 100.0f, UPDBGCHECKINFO_FLAG_1 | UPDBGCHECKINFO_FLAG_4);
    if (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND) {
        Math_ApproachZeroF(&this->additionalVelocityX, 1.0f, 1.0f);
        Math_ApproachZeroF(&this->additionalVelocityZ, 1.0f, 1.0f);
    }

    this->actor.shape.rot = this->actor.world.rot;

    if (this->disableCollisionTimer == 0) {
        Boss01_UpdateDamage(this, play);

        if (this->bodyInvincibilityTimer == 0) {
            CollisionCheck_SetAC(play, &play->colChkCtx, &this->bodyCollider.base);
        } else {
            this->bodyInvincibilityTimer--;
            for (i = 0; i < ODOLWA_COLLIDER_BODYPART_MAX; i++) {
                this->bodyCollider.elements[i].base.acElemFlags &= ~ACELEM_HIT;
            }
        }

        if (this->swordAndShieldCollisionEnabled) {
            CollisionCheck_SetAC(play, &play->colChkCtx, &this->shieldCollider.base);
            CollisionCheck_SetOC(play, &play->colChkCtx, &this->shieldCollider.base);
            CollisionCheck_SetAC(play, &play->colChkCtx, &this->swordCollider.base);
        }

        if (this->swordState != ODOLWA_SWORD_STATE_INACTIVE) {
            CollisionCheck_SetAT(play, &play->colChkCtx, &this->swordCollider.base);
        }

        if (this->kickAndShieldBashCollisionEnabled) {
            CollisionCheck_SetAT(play, &play->colChkCtx, &this->kickAndShieldBashCollider.base);
        }
    } else {
        this->disableCollisionTimer--;
        for (i = 0; i < ODOLWA_COLLIDER_BODYPART_MAX; i++) {
            this->bodyCollider.elements[i].base.acElemFlags &= ~ACELEM_HIT;
        }
    }

    CollisionCheck_SetOC(play, &play->colChkCtx, &this->bodyCollider.base);

    // If Odolwa and the player are *not* facing each other, Odolwa will *not* block or jump away from the player's
    // attacks, even if canGuardOrEvade is set to true. This allows the player to hit Odolwa even during states where he
    // normally evades attacks, so long as the player is far enough to the side or behind him.
    if (this->canGuardOrEvade &&
        ((player->unk_D57 != 0) || ((player->unk_ADC != 0) && (this->actor.xzDistToPlayer <= 120.0f))) &&
        Boss01_ArePlayerAndActorFacing(this, play)) {
        if ((Rand_ZeroOne() < 0.25f) && (this->actionFunc != Boss01_Guard)) {
            Boss01_SetupJump(this, play, false);
            this->disableCollisionTimer = 10;
        } else if ((player->unk_ADC != 0) && (this->actor.xzDistToPlayer <= 120.0f)) {
            Boss01_SetupGuard(this, play, true);
        } else {
            Boss01_SetupGuard(this, play, false);
        }
    }

    if (this->canGuardOrEvade) {
        Boss01_JumpAwayFromExplosive(this, play);
    }

    if (((this->frameCounter & (this->afterimageSpawnFrameMask - 1)) == 0) && (this->afterimageSpawnFrameMask != 0)) {
        s16 afterimageTimer = (this->actionFunc == Boss01_SpinAttack) ? 4 : 10;
        s32 pad;
        Boss01* child = (Boss01*)Actor_SpawnAsChild(
            &play->actorCtx, &this->actor, play, ACTOR_BOSS_01, this->actor.world.pos.x, this->actor.world.pos.y,
            this->actor.world.pos.z, this->actor.world.rot.x, this->actor.world.rot.y, afterimageTimer,
            ODOLWA_PARAMS(ODOLWA_TYPE_AFTERIMAGE));

        if (child != NULL) {
            for (i = 0; i < ODOLWA_LIMB_MAX; i++) {
                child->skelAnime.jointTable[i] = this->skelAnime.jointTable[i];
            }
        }
    }

    Boss01_UpdateEffects(this, play);

    if (sOdolwaMusicStartTimer != 0) {
        sOdolwaMusicStartTimer--;
        if (sOdolwaMusicStartTimer == 0) {
            SEQCMD_PLAY_SEQUENCE(SEQ_PLAYER_BGM_MAIN, 0, NA_BGM_BOSS | SEQ_FLAG_ASYNC);
        }
    }

    if (this->lookAtPlayer) {
        targetHeadRotY = this->actor.yawTowardsPlayer - this->actor.shape.rot.y;
        if ((targetHeadRotY > 0x3800) || (targetHeadRotY < -0x3800)) {
            targetHeadRotY = 0;
        }

        diffX = player->actor.world.pos.x - this->actor.focus.pos.x;
        diffY = KREG(36) + ((player->actor.world.pos.y + 25.0f) - this->actor.focus.pos.y);
        diffZ = player->actor.world.pos.z - this->actor.focus.pos.z;
        targetHeadRotX = Math_Atan2S(diffY, sqrtf(SQ(diffX) + SQ(diffZ)));

        // This line of code ensures that *not* following the player is the default behavior; if you want Odolwa's head
        // to track the player, you'll need to set this variable to true for every single frame you want this behavior.
        this->lookAtPlayer = false;
    } else {
        targetHeadRotX = 0;
        targetHeadRotY = 0;
    }

    Math_ApproachS(&this->headRotY, targetHeadRotY, 3, 0x2000);
    Math_ApproachS(&this->headRotX, targetHeadRotX, 3, 0x2000);

    DECR(this->drawDmgEffTimer);

    switch (this->drawDmgEffState) {
        case ODOLWA_DRAW_DMGEFF_STATE_NONE:
            this->drawDmgEffType = ACTOR_DRAW_DMGEFF_FIRE;
            this->drawDmgEffTimer = 0;
            this->drawDmgEffAlpha = 0.0f;
            break;

        case ODOLWA_DRAW_DMGEFF_STATE_FIRE_INIT:
            this->drawDmgEffAlpha = 1.0f;
            this->drawDmgEffScale = 0.0f;
            this->drawDmgEffType = ACTOR_DRAW_DMGEFF_FIRE;
            this->drawDmgEffTimer = 40;
            this->drawDmgEffState++;
            Actor_SetColorFilter(&this->actor, COLORFILTER_COLORFLAG_RED, 120, COLORFILTER_BUFFLAG_OPA, 60);
            // fallthrough
        case ODOLWA_DRAW_DMGEFF_STATE_FIRE_ACTIVE:
            if (this->drawDmgEffTimer == 0) {
                Math_ApproachZeroF(&this->drawDmgEffAlpha, 1.0f, 0.02f);
                if (this->drawDmgEffAlpha == 0.0f) {
                    this->drawDmgEffState = ODOLWA_DRAW_DMGEFF_STATE_NONE;
                }
            } else {
                Math_ApproachF(&this->drawDmgEffScale, 1.0f, 0.1f, 0.5f);
            }
            break;

        case ODOLWA_DRAW_DMGEFF_STATE_FROZEN_INIT:
            this->drawDmgEffType = ACTOR_DRAW_DMGEFF_FROZEN_SFX;
            this->drawDmgEffTimer = 40;
            this->drawDmgEffState++;
            this->drawDmgEffAlpha = 1.0f;
            this->drawDmgEffScale = 0.0f;
            this->drawDmgEffFrozenSteamScale = 1.0f;
            // fallthrough
        case ODOLWA_DRAW_DMGEFF_STATE_FROZEN_ACTIVE:
            if (this->drawDmgEffTimer == 0) {
                Boss01_Thaw(this, play);
                this->drawDmgEffState = ODOLWA_DRAW_DMGEFF_STATE_NONE;
                break;
            }

            if (this->drawDmgEffTimer == 50) {
                this->drawDmgEffType = ACTOR_DRAW_DMGEFF_FROZEN_NO_SFX;
            }

            Math_ApproachF(&this->drawDmgEffScale, 1.0f, 1.0f, 0.08f);
            Math_ApproachF(&this->drawDmgEffFrozenSteamScale, 1.0f, 0.05f, 0.05f);
            break;

        case ODOLWA_DRAW_DMGEFF_STATE_LIGHT_ORB_INIT:
            this->drawDmgEffType = ACTOR_DRAW_DMGEFF_LIGHT_ORBS;
            this->drawDmgEffTimer = 40;
            this->drawDmgEffScale = 1.0f;
            goto lightOrbInitCommon;

        case ODOLWA_DRAW_DMGEFF_STATE_BLUE_LIGHT_ORB_INIT:
            this->drawDmgEffType = ACTOR_DRAW_DMGEFF_BLUE_LIGHT_ORBS;
            this->drawDmgEffTimer = 40;
            this->drawDmgEffScale = 3.0f;
        lightOrbInitCommon:
            this->drawDmgEffState = ODOLWA_DRAW_DMGEFF_STATE_LIGHT_ORB_ACTIVE;
            this->drawDmgEffAlpha = 1.0f;
            // fallthrough
        case ODOLWA_DRAW_DMGEFF_STATE_LIGHT_ORB_ACTIVE:
            if (this->drawDmgEffTimer == 0) {
                Math_ApproachZeroF(&this->drawDmgEffScale, 1.0f, 0.03f);
                if (this->drawDmgEffScale == 0.0f) {
                    this->drawDmgEffState = ODOLWA_DRAW_DMGEFF_STATE_NONE;
                    this->drawDmgEffAlpha = 0.0f;
                }
            } else {
                Math_ApproachF(&this->drawDmgEffScale, 1.5f, 0.5f, 0.5f);
            }
            break;

        case ODOLWA_DRAW_DMGEFF_STATE_ELECTRIC_SPARKS_INIT:
            this->drawDmgEffType = ACTOR_DRAW_DMGEFF_ELECTRIC_SPARKS_SMALL;
            this->drawDmgEffTimer = 50;
            this->drawDmgEffAlpha = 1.0f;
            this->drawDmgEffScale = (KREG(18) * 0.1f) + 1.0f;
            this->drawDmgEffState++;
            // fallthrough
        case ODOLWA_DRAW_DMGEFF_STATE_ELECTRIC_SPARKS_ACTIVE:
            if (this->drawDmgEffTimer == 0) {
                Math_ApproachZeroF(&this->drawDmgEffScale, 1.0f, 0.05f);
                if (this->drawDmgEffScale == 0.0f) {
                    this->drawDmgEffState = ODOLWA_DRAW_DMGEFF_STATE_NONE;
                    this->drawDmgEffAlpha = 0.0f;
                }
            }
            break;

        default:
            break;
    }
}

/**
 * Draws the sword trail that follows Odolwa's sword during horizontal and vertical slashes.
 */
void Boss01_DrawSwordTrail(Boss01* this, PlayState* play) {
    static u8 sSwordTrailOuterVertexIndices[] = { 0, 2, 4, 6, 8, 10, 12, 14, 16, 18, 20 };
    static u8 sSwordTrailInnerVertexIndices[] = { 1, 3, 5, 7, 9, 11, 13, 15, 17, 19, 21 };
    Vtx* vtx;
    u32 i;

    OPEN_DISPS(play->state.gfxCtx);

    vtx = ResourceMgr_LoadVtxByName(Lib_SegmentedToVirtual(&gOdolwaSwordTrailVtx));

    for (i = 0; i < ARRAY_COUNT(sSwordTrailOuterVertexIndices); i++) {
        vtx[sSwordTrailOuterVertexIndices[i]].v.ob[0] = cosf((i * M_PIf) / sSwordTrailAngularRangeDivisor) * 200.0f;
        vtx[sSwordTrailOuterVertexIndices[i]].v.ob[1] = 0;
        vtx[sSwordTrailOuterVertexIndices[i]].v.ob[2] = sinf((i * M_PIf) / sSwordTrailAngularRangeDivisor) * 200.0f;

        vtx[sSwordTrailInnerVertexIndices[i]].v.ob[0] = cosf((i * M_PIf) / sSwordTrailAngularRangeDivisor) * 100.0f;
        vtx[sSwordTrailInnerVertexIndices[i]].v.ob[1] = 0;
        vtx[sSwordTrailInnerVertexIndices[i]].v.ob[2] = sinf((i * M_PIf) / sSwordTrailAngularRangeDivisor) * 100.0f;
    }

    gSPSegment(POLY_XLU_DISP++, 0x08,
               Gfx_TwoTexScrollEx(play->state.gfxCtx, G_TX_RENDERTILE, 0, 0, 32, 32, 1, play->gameplayFrames * 18, 0,
                                  32, 32, 0, 0, 18, 0));
    gDPPipeSync(POLY_XLU_DISP++);
    gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 255, 255, 255, (u8)sOdolwaSwordTrailAlpha);

    Matrix_Translate(this->actor.world.pos.x + sOdolwaSwordTrailPosX, this->actor.world.pos.y + sOdolwaSwordTrailPosY,
                     this->actor.world.pos.z + sOdolwaSwordTrailPosZ, MTXMODE_NEW);
    Matrix_RotateYF(BINANG_TO_RAD(this->actor.shape.rot.y), MTXMODE_APPLY);
    Matrix_RotateXFApply(sOdolwaSwordTrailRotX);
    Matrix_RotateZF(sOdolwaSwordTrailRotZ, MTXMODE_APPLY);
    Matrix_RotateYF(sOdolwaSwordTrailRotY, MTXMODE_APPLY);

    MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx);
    gSPDisplayList(POLY_XLU_DISP++, gOdolwaSwordTrailDL);

    CLOSE_DISPS(play->state.gfxCtx);
}

s32 Boss01_OverrideLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, Actor* thisx) {
    Boss01* this = (Boss01*)thisx;

    if (limbIndex == ODOLWA_LIMB_HEAD) {
        // The rot variable here is in model space, whereas the headRot variables are in world space.
        // Odolwa's head is lying on its side in model space, which is why this assignment looks weird.
        rot->x += this->headRotY;
        rot->y += this->headRotX;
    }

    return false;
}

static s8 sLimbToColliderBodyParts[] = {
    BODYPART_NONE,                            // ODOLWA_LIMB_NONE
    BODYPART_NONE,                            // ODOLWA_LIMB_ROOT
    ODOLWA_COLLIDER_BODYPART_PELVIS,          // ODOLWA_LIMB_PELVIS
    BODYPART_NONE,                            // ODOLWA_LIMB_RIGHT_LEG_ROOT
    ODOLWA_COLLIDER_BODYPART_RIGHT_THIGH,     // ODOLWA_LIMB_RIGHT_THIGH
    BODYPART_NONE,                            // ODOLWA_LIMB_RIGHT_LOWER_LEG_ROOT
    ODOLWA_COLLIDER_BODYPART_RIGHT_SHIN,      // ODOLWA_LIMB_RIGHT_SHIN
    BODYPART_NONE,                            // ODOLWA_LIMB_RIGHT_FOOT
    BODYPART_NONE,                            // ODOLWA_LIMB_LEFT_LEG_ROOT
    ODOLWA_COLLIDER_BODYPART_LEFT_THIGH,      // ODOLWA_LIMB_LEFT_THIGH
    BODYPART_NONE,                            // ODOLWA_LIMB_LEFT_LOWER_LEG_ROOT
    ODOLWA_COLLIDER_BODYPART_LEFT_SHIN,       // ODOLWA_LIMB_LEFT_SHIN
    BODYPART_NONE,                            // ODOLWA_LIMB_LEFT_FOOT
    BODYPART_NONE,                            // ODOLWA_LIMB_UPPER_BODY_ROOT
    BODYPART_NONE,                            // ODOLWA_LIMB_UPPER_BODY_WRAPPER
    ODOLWA_COLLIDER_BODYPART_TORSO,           // ODOLWA_LIMB_TORSO
    BODYPART_NONE,                            // ODOLWA_LIMB_LEFT_ARM_ROOT
    ODOLWA_COLLIDER_BODYPART_LEFT_UPPER_ARM,  // ODOLWA_LIMB_LEFT_UPPER_ARM
    BODYPART_NONE,                            // ODOLWA_LIMB_LEFT_LOWER_ARM_ROOT
    ODOLWA_COLLIDER_BODYPART_LEFT_FOREARM,    // ODOLWA_LIMB_LEFT_FOREARM
    BODYPART_NONE,                            // ODOLWA_LIMB_LEFT_BANGLE
    BODYPART_NONE,                            // ODOLWA_LIMB_LEFT_HAND_ROOT
    BODYPART_NONE,                            // ODOLWA_LIMB_LEFT_HAND
    BODYPART_NONE,                            // ODOLWA_LIMB_SHIELD
    BODYPART_NONE,                            // ODOLWA_LIMB_RIGHT_ARM_ROOT
    ODOLWA_COLLIDER_BODYPART_RIGHT_UPPER_ARM, // ODOLWA_LIMB_RIGHT_UPPER_ARM
    BODYPART_NONE,                            // ODOLWA_LIMB_RIGHT_LOWER_ARM_ROOT
    ODOLWA_COLLIDER_BODYPART_RIGHT_FOREARM,   // ODOLWA_LIMB_RIGHT_FOREARM
    BODYPART_NONE,                            // ODOLWA_LIMB_RIGHT_BANGLE
    BODYPART_NONE,                            // ODOLWA_LIMB_RIGHT_HAND_ROOT
    BODYPART_NONE,                            // ODOLWA_LIMB_RIGHT_HAND
    BODYPART_NONE,                            // ODOLWA_LIMB_SWORD
    ODOLWA_COLLIDER_BODYPART_HEAD,            // ODOLWA_LIMB_HEAD
    BODYPART_NONE,                            // ODOLWA_LIMB_RIGHT_EARRING_ROOT
    BODYPART_NONE,                            // ODOLWA_LIMB_RIGHT_EARRING
    BODYPART_NONE,                            // ODOLWA_LIMB_LEFT_EARRING_ROOT
    BODYPART_NONE,                            // ODOLWA_LIMB_LEFT_EARRING
    BODYPART_NONE,                            // ODOLWA_LIMB_RIGHT_PLUME_ROOT
    BODYPART_NONE,                            // ODOLWA_LIMB_RIGHT_PLUME_BASE
    BODYPART_NONE,                            // ODOLWA_LIMB_RIGHT_LOWER_PLUME_ROOT
    BODYPART_NONE,                            // ODOLWA_LIMB_RIGHT_PLUME_MIDDLE
    BODYPART_NONE,                            // ODOLWA_LIMB_RIGHT_PLUME_TIP
    BODYPART_NONE,                            // ODOLWA_LIMB_LEFT_PLUME_ROOT
    BODYPART_NONE,                            // ODOLWA_LIMB_LEFT_PLUME_BASE
    BODYPART_NONE,                            // ODOLWA_LIMB_LEFT_LOWER_PLUME_ROOT
    BODYPART_NONE,                            // ODOLWA_LIMB_LEFT_PLUME_MIDDLE
    BODYPART_NONE,                            // ODOLWA_LIMB_LEFT_PLUME_TIP
    BODYPART_NONE,                            // ODOLWA_LIMB_CENTER_PLUME_ROOT
    BODYPART_NONE,                            // ODOLWA_LIMB_CENTER_PLUME_BASE
    BODYPART_NONE,                            // ODOLWA_LIMB_CENTER_LOWER_PLUME_ROOT
    BODYPART_NONE,                            // ODOLWA_LIMB_CENTER_PLUME_MIDDLE
    BODYPART_NONE,                            // ODOLWA_LIMB_CENTER_PLUME_TIP
    BODYPART_NONE,                            // Doesn't correspond to a real limb on Odolwa
    BODYPART_NONE,                            // Doesn't correspond to a real limb on Odolwa
    BODYPART_NONE,                            // Doesn't correspond to a real limb on Odolwa
    BODYPART_NONE,                            // Doesn't correspond to a real limb on Odolwa
    BODYPART_NONE,                            // Doesn't correspond to a real limb on Odolwa
    BODYPART_NONE,                            // Doesn't correspond to a real limb on Odolwa
    BODYPART_NONE,                            // Doesn't correspond to a real limb on Odolwa
    BODYPART_NONE,                            // Doesn't correspond to a real limb on Odolwa
};

static Vec3f sLimbColliderOffsets[ODOLWA_COLLIDER_BODYPART_MAX] = {
    { 1300.0f, 0.0f, 0.0f }, // ODOLWA_COLLIDER_BODYPART_HEAD
    { 1000.0f, 0.0f, 0.0f }, // ODOLWA_COLLIDER_BODYPART_TORSO
    { 0.0f, 0.0f, 0.0f },    // ODOLWA_COLLIDER_BODYPART_PELVIS
    { 1000.0f, 0.0f, 0.0f }, // ODOLWA_COLLIDER_BODYPART_LEFT_UPPER_ARM
    { 1000.0f, 0.0f, 0.0f }, // ODOLWA_COLLIDER_BODYPART_LEFT_FOREARM
    { 1000.0f, 0.0f, 0.0f }, // ODOLWA_COLLIDER_BODYPART_RIGHT_UPPER_ARM
    { 1000.0f, 0.0f, 0.0f }, // ODOLWA_COLLIDER_BODYPART_RIGHT_FOREARM
    { 1500.0f, 0.0f, 0.0f }, // ODOLWA_COLLIDER_BODYPART_LEFT_THIGH
    { 1500.0f, 0.0f, 0.0f }, // ODOLWA_COLLIDER_BODYPART_LEFT_SHIN
    { 1500.0f, 0.0f, 0.0f }, // ODOLWA_COLLIDER_BODYPART_RIGHT_THIGH
    { 1500.0f, 0.0f, 0.0f }, // ODOLWA_COLLIDER_BODYPART_RIGHT_SHIN
};

static Vec3f sShieldColliderOffset = { 0.0f, 500.0f, 0.0f };

static Vec3f sSwordBaseColliderOffset = { 500.0f, -2500.0f, 0.0f };

static Vec3f sSwordTipColliderOffset = { 1500.0f, -7000.0f, 0.0f };

/**
 * When Odolwa performs a horizontal slash, his sword is very likely to be too high to hit the player. In order to make
 * the attack effective, there is an additional collider projected from Odolwa's pelvis that sweeps across the floor
 * during his horizontal slash. This variable controls the offset of this collider from his pelvis limb.
 */
static Vec3f sHorizontalSlashPelvisColliderOffset = { 5000.0f, 0.0f, 9000.0f };

static s8 sLimbToBodyParts[] = {
    BODYPART_NONE,                   // ODOLWA_LIMB_NONE
    BODYPART_NONE,                   // ODOLWA_LIMB_ROOT
    ODOLWA_BODYPART_PELVIS,          // ODOLWA_LIMB_PELVIS
    BODYPART_NONE,                   // ODOLWA_LIMB_RIGHT_LEG_ROOT
    ODOLWA_BODYPART_RIGHT_THIGH,     // ODOLWA_LIMB_RIGHT_THIGH
    BODYPART_NONE,                   // ODOLWA_LIMB_RIGHT_LOWER_LEG_ROOT
    ODOLWA_BODYPART_RIGHT_SHIN,      // ODOLWA_LIMB_RIGHT_SHIN
    ODOLWA_BODYPART_RIGHT_FOOT,      // ODOLWA_LIMB_RIGHT_FOOT
    BODYPART_NONE,                   // ODOLWA_LIMB_LEFT_LEG_ROOT
    ODOLWA_BODYPART_LEFT_THIGH,      // ODOLWA_LIMB_LEFT_THIGH
    BODYPART_NONE,                   // ODOLWA_LIMB_LEFT_LOWER_LEG_ROOT
    ODOLWA_BODYPART_LEFT_SHIN,       // ODOLWA_LIMB_LEFT_SHIN
    ODOLWA_BODYPART_LEFT_FOOT,       // ODOLWA_LIMB_LEFT_FOOT
    BODYPART_NONE,                   // ODOLWA_LIMB_UPPER_BODY_ROOT
    BODYPART_NONE,                   // ODOLWA_LIMB_UPPER_BODY_WRAPPER
    ODOLWA_BODYPART_TORSO,           // ODOLWA_LIMB_TORSO
    BODYPART_NONE,                   // ODOLWA_LIMB_LEFT_ARM_ROOT
    ODOLWA_BODYPART_LEFT_UPPER_ARM,  // ODOLWA_LIMB_LEFT_UPPER_ARM
    BODYPART_NONE,                   // ODOLWA_LIMB_LEFT_LOWER_ARM_ROOT
    ODOLWA_BODYPART_LEFT_FOREARM,    // ODOLWA_LIMB_LEFT_FOREARM
    BODYPART_NONE,                   // ODOLWA_LIMB_LEFT_BANGLE
    BODYPART_NONE,                   // ODOLWA_LIMB_LEFT_HAND_ROOT
    BODYPART_NONE,                   // ODOLWA_LIMB_LEFT_HAND
    ODOLWA_BODYPART_SHIELD,          // ODOLWA_LIMB_SHIELD
    BODYPART_NONE,                   // ODOLWA_LIMB_RIGHT_ARM_ROOT
    ODOLWA_BODYPART_RIGHT_UPPER_ARM, // ODOLWA_LIMB_RIGHT_UPPER_ARM
    BODYPART_NONE,                   // ODOLWA_LIMB_RIGHT_LOWER_ARM_ROOT
    ODOLWA_BODYPART_RIGHT_FOREARM,   // ODOLWA_LIMB_RIGHT_FOREARM
    BODYPART_NONE,                   // ODOLWA_LIMB_RIGHT_BANGLE
    BODYPART_NONE,                   // ODOLWA_LIMB_RIGHT_HAND_ROOT
    BODYPART_NONE,                   // ODOLWA_LIMB_RIGHT_HAND
    ODOLWA_BODYPART_SWORD,           // ODOLWA_LIMB_SWORD
    ODOLWA_BODYPART_HEAD,            // ODOLWA_LIMB_HEAD
    BODYPART_NONE,                   // ODOLWA_LIMB_RIGHT_EARRING_ROOT
    BODYPART_NONE,                   // ODOLWA_LIMB_RIGHT_EARRING
    BODYPART_NONE,                   // ODOLWA_LIMB_LEFT_EARRING_ROOT
    BODYPART_NONE,                   // ODOLWA_LIMB_LEFT_EARRING
    BODYPART_NONE,                   // ODOLWA_LIMB_RIGHT_PLUME_ROOT
    BODYPART_NONE,                   // ODOLWA_LIMB_RIGHT_PLUME_BASE
    BODYPART_NONE,                   // ODOLWA_LIMB_RIGHT_LOWER_PLUME_ROOT
    BODYPART_NONE,                   // ODOLWA_LIMB_RIGHT_PLUME_MIDDLE
    BODYPART_NONE,                   // ODOLWA_LIMB_RIGHT_PLUME_TIP
    BODYPART_NONE,                   // ODOLWA_LIMB_LEFT_PLUME_ROOT
    BODYPART_NONE,                   // ODOLWA_LIMB_LEFT_PLUME_BASE
    BODYPART_NONE,                   // ODOLWA_LIMB_LEFT_LOWER_PLUME_ROOT
    BODYPART_NONE,                   // ODOLWA_LIMB_LEFT_PLUME_MIDDLE
    BODYPART_NONE,                   // ODOLWA_LIMB_LEFT_PLUME_TIP
    BODYPART_NONE,                   // ODOLWA_LIMB_CENTER_PLUME_ROOT
    BODYPART_NONE,                   // ODOLWA_LIMB_CENTER_PLUME_BASE
    BODYPART_NONE,                   // ODOLWA_LIMB_CENTER_LOWER_PLUME_ROOT
    BODYPART_NONE,                   // ODOLWA_LIMB_CENTER_PLUME_MIDDLE
    BODYPART_NONE,                   // ODOLWA_LIMB_CENTER_PLUME_TIP
    BODYPART_NONE,                   // Doesn't correspond to a real limb on Odolwa
};

void Boss01_PostLimbDraw(PlayState* play2, s32 limbIndex, Gfx** dList, Vec3s* rot, Actor* thisx) {
    Boss01* this = (Boss01*)thisx;
    PlayState* play = play2;
    s8 bodyPartIndex;
    Vec3f pos;

    if (limbIndex == ODOLWA_LIMB_HEAD) {
        Matrix_MultZero(&this->actor.focus.pos);
    }

    bodyPartIndex = sLimbToBodyParts[limbIndex];
    if (bodyPartIndex > BODYPART_NONE) {
        Matrix_MultZero(&this->bodyPartsPos[bodyPartIndex]);
    }

    bodyPartIndex = sLimbToColliderBodyParts[limbIndex];
    if (bodyPartIndex > BODYPART_NONE) {
        Matrix_MultVec3f(&sLimbColliderOffsets[bodyPartIndex], &pos);
        Boss01_SetColliderSphere(bodyPartIndex, &this->bodyCollider, &pos);
    }

    if (limbIndex == ODOLWA_LIMB_PELVIS) {
        Matrix_MultZero(&this->pelvisPos);
        if (this->swordState == ODOLWA_SWORD_STATE_HORIZONTAL_SLASH) {
            Matrix_MultVec3f(&sHorizontalSlashPelvisColliderOffset, &pos);
            Boss01_SetColliderSphere(ODOLWA_SWORD_COLLIDER_PELVIS, &this->swordCollider, &pos);
        }
    }

    if (limbIndex == ODOLWA_LIMB_SWORD) {
        Matrix_MultVec3f(&sSwordBaseColliderOffset, &pos);
        Boss01_SetColliderSphere(ODOLWA_SWORD_COLLIDER_SWORD_BASE, &this->swordCollider, &pos);
        Matrix_MultVec3f(&sSwordTipColliderOffset, &pos);
        Boss01_SetColliderSphere(ODOLWA_SWORD_COLLIDER_SWORD_TIP, &this->swordCollider, &pos);
    }

    if (limbIndex == ODOLWA_LIMB_SHIELD) {
        Matrix_MultVec3f(&sShieldColliderOffset, &pos);
        Boss01_SetColliderSphere(ODOLWA_SHIELD_COLLIDER_SHIELD, &this->shieldCollider, &pos);
        Boss01_SetColliderSphere(ODOLWA_KICK_AND_SHIELD_BASH_COLLIDER_SHIELD, &this->kickAndShieldBashCollider, &pos);
    }

    if (limbIndex == ODOLWA_LIMB_RIGHT_FOOT) {
        Matrix_MultZero(&this->feetPos[0]);
    }

    if (limbIndex == ODOLWA_LIMB_LEFT_FOOT) {
        Matrix_MultZero(&this->feetPos[1]);
        Matrix_MultZero(&pos);
        Boss01_SetColliderSphere(ODOLWA_KICK_AND_SHIELD_BASH_COLLIDER_LEFT_FOOT, &this->kickAndShieldBashCollider,
                                 &pos);
    }

    if (limbIndex == ODOLWA_LIMB_HEAD) {
        OPEN_DISPS(play->state.gfxCtx);

        gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 255, 0, 0, 255);
        gDPSetEnvColor(POLY_XLU_DISP++, 255, 0, 0, 0);

        Matrix_Push();
        Matrix_Translate(1470.0f, 400.0f, 450.0f, MTXMODE_APPLY);
        Matrix_Scale(0.35f, 0.35f, 0.35f, MTXMODE_APPLY);
        Matrix_ReplaceRotation(&play->billboardMtxF);
        MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx);
        gSPDisplayList(POLY_XLU_DISP++, gOdolwaEyeDL);
        Matrix_Pop();

        Matrix_Push();
        Matrix_Translate(1470.0f, -360.0f, 450.0f, MTXMODE_APPLY);
        Matrix_Scale(0.35f, 0.35f, 0.35f, MTXMODE_APPLY);
        Matrix_ReplaceRotation(&play->billboardMtxF);
        MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx);
        gSPDisplayList(POLY_XLU_DISP++, gOdolwaEyeDL);
        Matrix_Pop();

        CLOSE_DISPS(play->state.gfxCtx);
    }
}

void Boss01_Draw(Actor* thisx, PlayState* play) {
    static Vec3f sDefaultPelvisColliderOffset = { 10000.0f, 10000.0f, 10000.0f };
    Boss01* this = (Boss01*)thisx;
    s32 pad;
    u8* tex = GRAPH_ALLOC(play->state.gfxCtx, ODOLWA_SHADOW_TEX_SIZE);

    OPEN_DISPS(play->state.gfxCtx);

    // When Odolwa is performing a horizontal slash, an additional collider offset from his pelvis will sweep across the
    // floor and damage the player. However, this collider should not be present all the time, so it is offset very far
    // out-of-bounds here. If Odolwa is performing a horizontal slash, then Boss01_PostLimbDraw (which is called later
    // in this function) will offset it properly for this frame.
    Boss01_SetColliderSphere(ODOLWA_SWORD_COLLIDER_PELVIS, &this->swordCollider, &sDefaultPelvisColliderOffset);

    Gfx_SetupDL25_Opa(play->state.gfxCtx);
    Gfx_SetupDL25_Xlu(play->state.gfxCtx);

    if ((this->damagedFlashTimer % 2) != 0) {
        POLY_OPA_DISP = Gfx_SetFog(POLY_OPA_DISP, 255, 0, 0, 255, 900, 1099);
    }

    SkelAnime_DrawFlexOpa(play, this->skelAnime.skeleton, this->skelAnime.jointTable, this->skelAnime.dListCount,
                          Boss01_OverrideLimbDraw, Boss01_PostLimbDraw, &this->actor);

    POLY_OPA_DISP = Play_SetFog(play, POLY_OPA_DISP);

    Boss01_GenShadowTex(tex, this, play);
    Boss01_DrawShadowTex(tex, this, play);

    if (sOdolwaSwordTrailAlpha > 0.0f) {
        Boss01_DrawSwordTrail(this, play);
        Math_ApproachZeroF(&sOdolwaSwordTrailAlpha, 1.0f, 50.0f);
    }

    Actor_DrawDamageEffects(play, &this->actor, this->bodyPartsPos, ODOLWA_BODYPART_MAX, this->drawDmgEffScale,
                            this->drawDmgEffFrozenSteamScale, this->drawDmgEffAlpha, this->drawDmgEffType);

    Boss01_DrawEffects(play);

    CLOSE_DISPS(play->state.gfxCtx);
}

void Boss01_Afterimage_Draw(Actor* thisx, PlayState* play) {
    Boss01* this = (Boss01*)thisx;
    s32 pad;
    Boss01* parent = (Boss01*)this->actor.parent;

    OPEN_DISPS(play->state.gfxCtx);

    Gfx_SetupDL25_Opa(play->state.gfxCtx);
    Gfx_SetupDL25_Xlu(play->state.gfxCtx);
    POLY_OPA_DISP = Gfx_SetFog(POLY_OPA_DISP, 50, 0, 40, 255, 900, 1099);
    SkelAnime_DrawFlexOpa(play, parent->skelAnime.skeleton, this->skelAnime.jointTable, parent->skelAnime.dListCount,
                          NULL, NULL, &this->actor);
    POLY_OPA_DISP = Play_SetFog(play, POLY_OPA_DISP);

    CLOSE_DISPS(play->state.gfxCtx);
}

/**
 * These four arrays encode circular shadow maps of various sizes. For an array of length N, the shadow map is N rows
 * tall, and each entry in the array describes the start and end point of the shadow within a given row (the exact
 * values of the start and end points are determined by the loops within Boss01_FillShadowTex). To illustrate using the
 * sShadowSmallMap as an example:
 * -3 -2 -1  0  1
 *  -------------
 *  0  0  1  0  0
 *  0  1  1  1  0
 *  1  1  1  1  1
 *  1  1  1  1  1
 *  0  1  1  1  0
 *  0  0  1  0  0
 */
static s32 sShadowSmallMap[] = {
    1, 2, 3, 3, 2, 1,
};

static s32 sShadowMediumMap[] = {
    2, 3, 4, 4, 4, 3, 2,
};

static s32 sShadowLargeMap[] = {
    2, 3, 4, 4, 4, 4, 3, 2,
};

static s32 sShadowExtraLargeMap[] = {
    2, 4, 5, 5, 6, 6, 6, 6, 5, 5, 4, 2,
};

static s32 sParentShadowBodyParts[ODOLWA_BODYPART_MAX] = {
    ODOLWA_BODYPART_TORSO,           // ODOLWA_BODYPART_HEAD
    BODYPART_NONE,                   // ODOLWA_BODYPART_TORSO
    ODOLWA_BODYPART_TORSO,           // ODOLWA_BODYPART_PELVIS
    ODOLWA_BODYPART_TORSO,           // ODOLWA_BODYPART_LEFT_UPPER_ARM
    ODOLWA_BODYPART_LEFT_UPPER_ARM,  // ODOLWA_BODYPART_LEFT_FOREARM
    ODOLWA_BODYPART_LEFT_FOREARM,    // ODOLWA_BODYPART_SHIELD
    ODOLWA_BODYPART_TORSO,           // ODOLWA_BODYPART_RIGHT_UPPER_ARM
    ODOLWA_BODYPART_RIGHT_UPPER_ARM, // ODOLWA_BODYPART_RIGHT_FOREARM
    ODOLWA_BODYPART_RIGHT_FOREARM,   // ODOLWA_BODYPART_SWORD
    ODOLWA_BODYPART_PELVIS,          // ODOLWA_BODYPART_RIGHT_THIGH
    ODOLWA_BODYPART_RIGHT_THIGH,     // ODOLWA_BODYPART_RIGHT_SHIN
    ODOLWA_BODYPART_RIGHT_SHIN,      // ODOLWA_BODYPART_RIGHT_FOOT
    ODOLWA_BODYPART_PELVIS,          // ODOLWA_BODYPART_LEFT_THIGH
    ODOLWA_BODYPART_LEFT_THIGH,      // ODOLWA_BODYPART_LEFT_SHIN
    ODOLWA_BODYPART_LEFT_SHIN,       // ODOLWA_BODYPART_LEFT_FOOT
};

static u8 sShadowSizes[ODOLWA_BODYPART_MAX] = {
    ODOLWA_SHADOW_SIZE_SMALL,       // ODOLWA_BODYPART_HEAD
    ODOLWA_SHADOW_SIZE_EXTRA_LARGE, // ODOLWA_BODYPART_TORSO
    ODOLWA_SHADOW_SIZE_EXTRA_LARGE, // ODOLWA_BODYPART_PELVIS
    ODOLWA_SHADOW_SIZE_LARGE,       // ODOLWA_BODYPART_LEFT_UPPER_ARM
    ODOLWA_SHADOW_SIZE_SMALL,       // ODOLWA_BODYPART_LEFT_FOREARM
    ODOLWA_SHADOW_SIZE_EXTRA_LARGE, // ODOLWA_BODYPART_SHIELD
    ODOLWA_SHADOW_SIZE_LARGE,       // ODOLWA_BODYPART_RIGHT_UPPER_ARM
    ODOLWA_SHADOW_SIZE_SMALL,       // ODOLWA_BODYPART_RIGHT_FOREARM
    ODOLWA_SHADOW_SIZE_SMALL,       // ODOLWA_BODYPART_SWORD
    ODOLWA_SHADOW_SIZE_LARGE,       // ODOLWA_BODYPART_RIGHT_THIGH
    ODOLWA_SHADOW_SIZE_MEDIUM,      // ODOLWA_BODYPART_RIGHT_SHIN
    ODOLWA_SHADOW_SIZE_SMALL,       // ODOLWA_BODYPART_RIGHT_FOOT
    ODOLWA_SHADOW_SIZE_LARGE,       // ODOLWA_BODYPART_LEFT_THIGH
    ODOLWA_SHADOW_SIZE_MEDIUM,      // ODOLWA_BODYPART_LEFT_SHIN
    ODOLWA_SHADOW_SIZE_SMALL,       // ODOLWA_BODYPART_LEFT_FOOT
};

void Boss01_FillShadowTex(Boss01* this, u8* tex, f32 weight) {
    s32 index;
    s32 i;
    s32 baseX;
    s32 baseY;
    s32 x;
    s32 y = 0;
    s32 addY;
    Vec3f lerp;
    Vec3f pos;
    Vec3f startVec;

    for (i = 0; i < ODOLWA_BODYPART_MAX; i++) {
        // TODO: match with a continue
        if ((weight == 0.0f) || (y = sParentShadowBodyParts[i]) > BODYPART_NONE) {
            if (weight > 0.0f) {
                VEC3F_LERPIMPDST(&lerp, &this->bodyPartsPos[i], &this->bodyPartsPos[y], weight);

                pos.x = lerp.x - this->actor.world.pos.x;
                pos.y = lerp.y - this->actor.world.pos.y + 76.0f + 30.0f + 30.0f + 100.0f;
                pos.z = lerp.z - this->actor.world.pos.z;
            } else {
                pos.x = this->bodyPartsPos[i].x - this->actor.world.pos.x;
                pos.y = this->bodyPartsPos[i].y - this->actor.world.pos.y + 76.0f + 30.0f + 30.0f + 100.0f;
                pos.z = this->bodyPartsPos[i].z - this->actor.world.pos.z;
            }

            Matrix_MultVec3f(&pos, &startVec);

            startVec.x *= 0.2f;
            startVec.y *= 0.2f;

            baseX = (u16)(s32)(startVec.x + 32.0f);
            baseY = (u16)((s32)startVec.y * 64);

            if (sShadowSizes[i] == ODOLWA_SHADOW_SIZE_EXTRA_LARGE) {
                for (y = 0, addY = -384; y < ARRAY_COUNT(sShadowExtraLargeMap); y++, addY += 64) {
                    for (x = -sShadowExtraLargeMap[y]; x < sShadowExtraLargeMap[y]; x++) {
                        index = baseX + x + baseY + addY;
                        if ((index >= 0) && (index < ODOLWA_SHADOW_TEX_SIZE)) {
                            tex[index] = 255;
                        }
                    }
                }
            } else if (sShadowSizes[i] == ODOLWA_SHADOW_SIZE_LARGE) {
                for (y = 0, addY = -256; y < ARRAY_COUNT(sShadowLargeMap); y++, addY += 64) {
                    for (x = -sShadowLargeMap[y]; x < sShadowLargeMap[y]; x++) {
                        index = baseX + x + baseY + addY;
                        if ((index >= 0) && (index < ODOLWA_SHADOW_TEX_SIZE)) {
                            tex[index] = 255;
                        }
                    }
                }
            } else if (sShadowSizes[i] == ODOLWA_SHADOW_SIZE_MEDIUM) {
                for (y = 0, addY = -192; y < ARRAY_COUNT(sShadowMediumMap); y++, addY += 64) {
                    for (x = -sShadowMediumMap[y]; x < sShadowMediumMap[y] - 1; x++) {
                        index = baseX + x + baseY + addY;
                        if ((index >= 0) && (index < ODOLWA_SHADOW_TEX_SIZE)) {
                            tex[index] = 255;
                        }
                    }
                }
            } else {
                for (y = 0, addY = -128; y < ARRAY_COUNT(sShadowSmallMap); y++, addY += 64) {
                    for (x = -sShadowSmallMap[y]; x < sShadowSmallMap[y] - 1; x++) {
                        index = baseX + x + baseY + addY;
                        if ((index >= 0) && (index < ODOLWA_SHADOW_TEX_SIZE)) {
                            tex[index] = 255;
                        }
                    }
                }
            }
        }
    }
}

void Boss01_GenShadowTex(u8* tex, Boss01* this, PlayState* play) {
    s32* iter = (s32*)tex;
    s16 i;

    for (i = 0; i < (s32)(ODOLWA_SHADOW_TEX_SIZE / sizeof(s32)); i++, iter++) {
        *iter = 0;
    }

    Matrix_RotateXFNew(1.0f);

    for (i = 0; i <= 5; i++) {
        Boss01_FillShadowTex(this, tex, i / 5.0f);
    }
}

/**
 * Draws Odolwa's dynamic shadow underneath him.
 */
void Boss01_DrawShadowTex(u8* tex, Boss01* this, PlayState* play) {
    s32 pad[2];
    f32 alpha;
    GraphicsContext* gfxCtx = play->state.gfxCtx;

    OPEN_DISPS(gfxCtx);

    gSPInvalidateTexCache(POLY_OPA_DISP++, tex);

    Gfx_SetupDL25_Opa(play->state.gfxCtx);

    alpha = (400.0f - this->actor.world.pos.y) * (1.0f / 400.0f);
    alpha = CLAMP_MIN(alpha, 0.0f);
    alpha = CLAMP_MAX(alpha, 1.0f);

    gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 0, 0, 0, (s8)(alpha * 80.0f));
    gDPSetEnvColor(POLY_OPA_DISP++, 0, 0, 0, 0);
    Matrix_Translate(this->actor.world.pos.x, this->actor.floorHeight, this->actor.world.pos.z - 20.0f, MTXMODE_NEW);
    Matrix_Scale(1.65f, 1.0f, 1.65f, MTXMODE_APPLY);
    MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx);
    gSPDisplayList(POLY_OPA_DISP++, gOdolwaShadowMaterialDL);
    gDPLoadTextureBlock(POLY_OPA_DISP++, tex, G_IM_FMT_I, G_IM_SIZ_8b, ODOLWA_SHADOW_TEX_WIDTH,
                        ODOLWA_SHADOW_TEX_HEIGHT, 0, G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMIRROR | G_TX_CLAMP, 6, 6,
                        G_TX_NOLOD, G_TX_NOLOD);
    gSPDisplayList(POLY_OPA_DISP++, gOdolwaShadowModelDL);

    CLOSE_DISPS(gfxCtx);
}

void Boss01_Bug_SetupCrawl(Boss01* this, PlayState* play) {
    this->actionFunc = Boss01_Bug_Crawl;
    Animation_MorphToLoop(&this->skelAnime, &gOdolwaBugCrawlAnim, -5.0f);
}

/**
 * If an explosive is present, the bug will crawl quickly towards it. Otherwise, it will crawl slowly towards the player
 * and chase them around.
 */
void Boss01_Bug_Crawl(Boss01* this, PlayState* play) {
    s32 pad;
    Player* player = GET_PLAYER(play);
    f32 targetSpeed = 3.0f;
    Actor* targetActor = &player->actor;
    Actor* explosive = play->actorCtx.actorLists[ACTORCAT_EXPLOSIVES].first;
    s16 maxStep = 0x3E8;

    if (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND) {
        this->skelAnime.playSpeed = 1.0f;

        while (explosive != NULL) {
            if (explosive->params == 1) {
                explosive = explosive->next;
            } else {
                targetSpeed = 5.0f;
                targetActor = explosive;
                maxStep = 0x7D0;
                this->skelAnime.playSpeed = 1.5f;
                break;
            }
        }

        SkelAnime_Update(&this->skelAnime);
        Actor_PlaySfx(&this->actor, NA_SE_EN_MB_INSECT_WALK - SFX_FLAG);
        Math_ApproachF(&this->actor.speed, targetSpeed, 1.0f, 1.0f);
        Math_ApproachS(&this->actor.world.rot.y,
                       Math_Atan2S(targetActor->world.pos.x - this->actor.world.pos.x,
                                   targetActor->world.pos.z - this->actor.world.pos.z),
                       5, maxStep);
    }
}

void Boss01_Bug_SetupDamaged(Boss01* this, PlayState* play) {
    if ((s8)this->actor.colChkInfo.health > 0) {
        this->actionFunc = Boss01_Bug_Damaged;
        this->timers[TIMER_BUG_CURRENT_ACTION] = 30;
    } else {
        this->timers[TIMER_BUG_CURRENT_ACTION] = 15;
        Enemy_StartFinishingBlow(play, &this->actor);
        Boss01_Bug_SetupDead(this, play);
    }
}

/**
 * Stops the bug's animations and makes it wait in place for 30 frames, then it starts crawling again.
 */
void Boss01_Bug_Damaged(Boss01* this, PlayState* play) {
    Math_ApproachZeroF(&this->actor.speed, 1.0f, 1.0f);
    if (this->timers[TIMER_BUG_CURRENT_ACTION] == 0) {
        Boss01_Bug_SetupCrawl(this, play);
    }
}

void Boss01_Bug_SetupDead(Boss01* this, PlayState* play) {
    this->actionFunc = Boss01_Bug_Dead;
    this->additionalVelocityZ = 0.0f;
    this->additionalVelocityX = 0.0f;
    this->bugDrawDmgEffAlpha = 1.0f;
    this->actor.speed = -15.0f;
    this->actor.velocity.y = 12.0f;
    this->actor.world.rot.y = this->actor.yawTowardsPlayer;
    this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
}

void Boss01_Bug_SetupStunned(Boss01* this, PlayState* play) {
    this->actionFunc = Boss01_Bug_Stunned;
    this->timers[TIMER_BUG_CURRENT_ACTION] = 40;
    Actor_SetColorFilter(&this->actor, COLORFILTER_COLORFLAG_BLUE, 120, COLORFILTER_BUFFLAG_OPA, 40);
}

/**
 * Turns the bug blue, stops its animations, and makes it wait in place for 40 frames, then it starts crawling again.
 */
void Boss01_Bug_Stunned(Boss01* this, PlayState* play) {
    Math_ApproachZeroF(&this->actor.speed, 1.0f, 1.0f);
    if (this->timers[TIMER_BUG_CURRENT_ACTION] == 0) {
        Boss01_Bug_SetupCrawl(this, play);
    }
}

/**
 * Flings the bug backwards, spinning randomly, until it hits the ground. After it lands, it shrinks and spawns a single
 * blue flame. The bug will die and drop an item once the flame fades enough.
 */
void Boss01_Bug_Dead(Boss01* this, PlayState* play) {
    if (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND) {
        Actor_PlaySfx(&this->actor, NA_SE_EN_COMMON_EXTINCT_LEV - SFX_FLAG);
        Math_ApproachZeroF(&this->actor.speed, 1.0f, 2.0f);
        Math_ApproachZeroF(&this->actor.scale.y, 1.0f, 0.00075f);
        Math_ApproachF(&this->bugDrawDmgEffScale, 1.0f, 0.5f, 0.15f);

        if (this->actor.scale.y < 0.001f) {
            Math_ApproachZeroF(&this->bugDrawDmgEffAlpha, 1.0f, 0.05f);
            if (this->bugDrawDmgEffAlpha < 0.01f) {
                Actor_Kill(&this->actor);
                Item_DropCollectibleRandom(play, NULL, &this->actor.world.pos, 0x60);
                sOdolwaBugCount--;
            }
        }

        Math_ApproachS(&this->actor.shape.rot.x, 0, 1, 0x800);
    } else {
        this->actor.shape.rot.x -= 0x2000;
        this->actor.shape.rot.z += 0x1000;
    }
}

void Boss01_Bug_UpdateDamage(Boss01* this, PlayState* play) {
    Vec3f additionalVelocity;
    s32 pad[2];
    u8 damage;
    ColliderElement* acHitElem;
    OdolwaEffect* effect = play->specialEffects;

    if (this->bugACCollider.base.acFlags & AC_HIT) {
        this->bugACCollider.base.acFlags &= ~AC_HIT;
        acHitElem = this->bugACCollider.elem.acHitElem;

        if (this->damagedTimer == 0) {
            Matrix_RotateYS(this->actor.yawTowardsPlayer, MTXMODE_NEW);
            if (acHitElem->atDmgInfo.dmgFlags & 0x300000) {
                this->damagedTimer = 10;
                Matrix_MultVecZ(-10.0f, &additionalVelocity);
                this->additionalVelocityX = additionalVelocity.x;
                this->additionalVelocityZ = additionalVelocity.z;
            } else {
                this->damagedTimer = 15;
                this->damagedFlashTimer = 15;
                Matrix_MultVecZ(-20.0f, &additionalVelocity);
                this->additionalVelocityX = additionalVelocity.x;
                this->additionalVelocityZ = additionalVelocity.z;
                if (this->actor.colChkInfo.damageEffect == BUG_DMGEFF_STUN) {
                    Boss01_Bug_SetupStunned(this, play);
                    Actor_PlaySfx(&this->actor, NA_SE_EN_COMMON_FREEZE);
                } else {
                    damage = this->actor.colChkInfo.damage;
                    this->actor.colChkInfo.health -= damage;
                    Boss01_Bug_SetupDamaged(this, play);
                    Actor_PlaySfx(&this->actor, NA_SE_EN_MIZUBABA2_DAMAGE);
                }
            }
        }
    }

    // If Odolwa dies, instantly kill any bugs in the room too. Setting their y-velocity to 0 will skip the part of
    // their death where they fly backwards; they will immediately shrink and burst into a single blue flame.
    if ((sOdolwa != NULL) && (sOdolwa->actionFunc == Boss01_DeathCutscene)) {
        Boss01_Bug_SetupDead(this, play);
        this->actor.velocity.y = 0.0f;
        this->actor.speed = 0.0f;
    }

    if ((effect->type == ODOLWA_EFFECT_RING_OF_FIRE) && (effect->timer < 150)) {
        f32 distXZ = sqrtf(SQ(effect->pos.x - this->actor.world.pos.x) + SQ(effect->pos.z - this->actor.world.pos.z));

        // If the bugs touch the ring of fire, it will instantly kill them. Like before, setting the y-velocity to zero
        // here will immediately make the bug shrink and burst into a single red flame.
        if ((distXZ < (KREG(49) + 210.0f)) && (distXZ > (KREG(49) + 190.0f))) {
            Actor_PlaySfx(&this->actor, NA_SE_EN_MIZUBABA2_DAMAGE);
            Boss01_Bug_SetupDead(this, play);
            this->damagedFlashTimer = 15;
            this->bugDrawDmgEffType = ACTOR_DRAW_DMGEFF_FIRE;
            this->actor.speed = 0.0f;
            this->actor.velocity.y = 5.0f;
        }
    }
}

void Boss01_Bug_Update(Actor* thisx, PlayState* play) {
    Boss01* this = (Boss01*)thisx;
    s32 pad;
    s32 i;

    this->frameCounter++;

    for (i = 0; i < ARRAY_COUNT(this->timers); i++) {
        DECR(this->timers[i]);
    }

    DECR(this->damagedTimer);
    DECR(this->damagedFlashTimer);

    this->actionFunc(this, play);

    Actor_MoveWithGravity(&this->actor);
    this->actor.world.pos.x += this->additionalVelocityX;
    this->actor.world.pos.z += this->additionalVelocityZ;
    Actor_SetFocus(&this->actor, 10.0f);
    Actor_UpdateBgCheckInfo(play, &this->actor, 50.0f, 21.0f, 100.0f, UPDBGCHECKINFO_FLAG_1 | UPDBGCHECKINFO_FLAG_4);
    if (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND) {
        Math_ApproachZeroF(&this->additionalVelocityX, 1.0f, 1.0f);
        Math_ApproachZeroF(&this->additionalVelocityZ, 1.0f, 1.0f);
    }

    if (this->actionFunc != Boss01_Bug_Dead) {
        Boss01_Bug_UpdateDamage(this, play);
        Collider_UpdateCylinder(&this->actor, &this->bugACCollider);
        CollisionCheck_SetAC(play, &play->colChkCtx, &this->bugACCollider.base);
        Collider_UpdateCylinder(&this->actor, &this->bugATCollider);
        CollisionCheck_SetAT(play, &play->colChkCtx, &this->bugATCollider.base);
        this->actor.shape.rot = this->actor.world.rot;
    }
}

s32 Boss01_Bug_OverrideLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, Actor* thisx) {
    if ((limbIndex == ODOLWA_BUG_LIMB_BODY) && (sOdolwa->actionFunc == Boss01_SummonBugsCutscene)) {
        *dList = gOdolwaBugDarkBodyDL;
    }

    if (limbIndex == ODOLWA_BUG_LIMB_FRONT_RIGHT_UPPER_LEG) {
        OPEN_DISPS(play->state.gfxCtx);

        if (sOdolwa->actionFunc == Boss01_SummonBugsCutscene) {
            gSPDisplayList(POLY_OPA_DISP++, gOdolwaBugBrightLegMaterialDL);
        } else {
            gSPDisplayList(POLY_OPA_DISP++, gOdolwaBugDullLegMaterialDL);
        }

        CLOSE_DISPS(play->state.gfxCtx);
    }

    return false;
}

void Boss01_Bug_Draw(Actor* thisx, PlayState* play) {
    Boss01* this = (Boss01*)thisx;
    s32 pad;

    OPEN_DISPS(play->state.gfxCtx);

    Gfx_SetupDL25_Opa(play->state.gfxCtx);

    if ((this->damagedFlashTimer % 2) != 0) {
        POLY_OPA_DISP = Gfx_SetFog(POLY_OPA_DISP, 255, 0, 0, 255, 900, 1099);
    }

    SkelAnime_DrawFlexOpa(play, this->skelAnime.skeleton, this->skelAnime.jointTable, this->skelAnime.dListCount,
                          Boss01_Bug_OverrideLimbDraw, NULL, &this->actor);

    POLY_OPA_DISP = Play_SetFog(play, POLY_OPA_DISP);

    Actor_DrawDamageEffects(play, &this->actor, &this->actor.world.pos, 1, this->bugDrawDmgEffScale, 0.0f,
                            this->bugDrawDmgEffAlpha, this->bugDrawDmgEffType);

    CLOSE_DISPS(play->state.gfxCtx);
}

/**
 * Spawns five dust clouds randomly around the block's current position.
 */
void Boss01_SpawnDustForFallingBlock(PlayState* play, Vec3f* blockPos, f32 scale) {
    s16 i;
    Vec3f velocity;
    Vec3f accel;
    Vec3f pos;

    for (i = 0; i < 5; i++) {
        velocity.x = Rand_CenteredFloat(5.0f);
        velocity.y = Rand_ZeroFloat(2.0f) + 1.0f;
        velocity.z = Rand_CenteredFloat(5.0f);
        accel.x = accel.z = 0.0f;
        accel.y = -0.1f;
        pos.x = (Rand_CenteredFloat(70.0f) * scale) + blockPos->x;
        pos.y = (Rand_ZeroFloat(10.0f) * scale) + blockPos->y;
        pos.z = (Rand_CenteredFloat(70.0f) * scale) + blockPos->z;
        func_800B0EB0(play, &pos, &velocity, &accel, &sDustPrimColor, &sDustEnvColor,
                      (Rand_ZeroFloat(150.0f) + 350.0f) * scale, 10, Rand_ZeroFloat(5.0f) + 14.0f);
    }
}

/**
 * Responsible for updating the falling blocks and the ring of fire, including spawning block fragments when a block
 * hits the floor and damaging the player.
 */
void Boss01_UpdateEffects(Boss01* this, PlayState* play) {
    OdolwaEffect* effect = (OdolwaEffect*)play->specialEffects;
    Player* player = GET_PLAYER(play);
    s16 i;
    s16 j;
    s16 temp;
    f32 diffX;
    f32 diffZ;
    f32 temp2;
    s32 type;

    for (i = 0; i < ODOLWA_EFFECT_COUNT; i++, effect++) {
        if (effect->type != ODOLWA_EFFECT_NONE) {
            effect->timer++;
            type = effect->type;
            if (type == ODOLWA_EFFECT_RING_OF_FIRE) {
                if (effect->timer < 150) {
                    Math_ApproachF(&play->envCtx.lightBlend, (Math_SinS(effect->timer * 0x1000) * 0.1f) + 0.9f, 1.0f,
                                   0.2f);
                    sMothSwarm->unk_144 = 250;

                    // If Odolwa dies, this will make the ring of fire start fading away immediately.
                    if ((sOdolwa != NULL) && (sOdolwa->actionFunc == Boss01_DeathCutscene)) {
                        effect->timer = 150;
                    }

                    Audio_PlaySfx(NA_SE_EV_FIRE_PLATE - SFX_FLAG);
                    Math_ApproachF(&effect->scale, 1.0f, 0.1f, 0.3f);

                    // The ring of fire has no collision, so this code is responsible for checking to see if the player
                    // is touching it. If they are, then it will set them on fire and damage them manually.
                    if (!(player->stateFlags3 & PLAYER_STATE3_1000) && (player->actor.world.pos.y < 70.0f)) {
                        diffX = effect->pos.x - player->actor.world.pos.x;
                        diffZ = effect->pos.z - player->actor.world.pos.z;
                        temp2 = sqrtf(SQ(diffX) + SQ(diffZ));

                        if (player->invincibilityTimer == 0) {
                            if ((temp2 < (KREG(49) + 210.0f)) && ((KREG(49) + 190.0f) < temp2)) {
                                for (j = 0; j < PLAYER_BODYPART_MAX; j++) {
                                    player->bodyFlameTimers[j] = Rand_S16Offset(0, 200);
                                }

                                player->bodyIsBurning = true;
                                temp = Math_Atan2S_XY(diffZ, diffX);
                                if ((KREG(49) + 100.0f) < temp2) {
                                    temp += 0x8000;
                                }

                                func_800B8D50(play, &this->actor, 10.0f, temp, 0.0f, 8);
                            }
                        }
                    }
                } else {
                    Math_ApproachZeroF(&effect->alpha, 1.0f, 10.0f);
                    if (effect->alpha < 0.1f) {
                        effect->type = ODOLWA_EFFECT_NONE;
                    }
                }
            } else {
                effect->pos.x += effect->velocity.x;
                effect->pos.y += effect->velocity.y;
                effect->pos.z += effect->velocity.z;
                effect->velocity.y += effect->gravity;

                if (type == ODOLWA_EFFECT_FALLING_BLOCK) {
                    effect->rotY += effect->angularVelocityY;
                    effect->rotX += effect->angularVelocityX;

                    if (!effect->isFallingBlockFragment) {
                        // Falling blocks have no collision, so this code is responsible for checking to see if the
                        // player is touching one. If they are, then it will damage them manually.
                        diffX = player->actor.world.pos.x - effect->pos.x;
                        temp2 = (player->actor.world.pos.y + 20.0f) - effect->pos.y;
                        diffZ = player->actor.world.pos.z - effect->pos.z;

                        if ((SQ(diffX) + SQ(diffZ) + SQ(temp2)) < SQ(50.0f)) {
                            func_800B8D50(play, NULL, 0.0f, Rand_ZeroFloat(65526.0f), 0.0f, 8);
                        }

                        if (effect->pos.y < 10.0f) {
                            effect->pos.y = 10.0f;
                            temp = (Rand_ZeroFloat(2.0f) + 4.0f);
                            for (j = 0; j < temp; j++) {
                                Boss01_SpawnEffectFallingBlock((OdolwaEffect*)play->specialEffects, &effect->pos, true);
                            }

                            effect->type = ODOLWA_EFFECT_NONE;
                            SoundSource_PlaySfxAtFixedWorldPos(play, &effect->pos, 40, NA_SE_EV_WALL_BROKEN);
                            Boss01_SpawnDustForFallingBlock(play, &effect->pos, 1.0f);
                            CollisionCheck_SpawnShieldParticles(play, &effect->pos);
                            Actor_RequestQuakeAndRumble(&this->actor, play, 3, 10);
                        }
                    } else if ((effect->pos.y < 10.0f) && (effect->velocity.y < 0.0f)) {
                        effect->pos.y = 10.0f;
                        effect->type = ODOLWA_EFFECT_NONE;
                        Boss01_SpawnDustForFallingBlock(play, &effect->pos, 0.5f);
                    }
                }
            }
        }
    }
}

/**
 * Draws the falling blocks, the circle shadows that appear under the blocks as they fall, and the ring of fire.
 */
void Boss01_DrawEffects(PlayState* play) {
    s32 pad;
    s16 i;
    f32 alpha;
    OdolwaEffect* effect = play->specialEffects;

    OPEN_DISPS(play->state.gfxCtx);

    Gfx_SetupDL25_Opa(play->state.gfxCtx);
    Gfx_SetupDL25_Xlu(play->state.gfxCtx);

    for (i = 1; i < ODOLWA_EFFECT_COUNT; i++, effect++) {
        if (effect->type == ODOLWA_EFFECT_FALLING_BLOCK) {
            Matrix_Translate(effect->pos.x, effect->pos.y, effect->pos.z, MTXMODE_NEW);
            Matrix_RotateYS(effect->rotY, MTXMODE_APPLY);
            Matrix_RotateXS(effect->rotX, MTXMODE_APPLY);
            Matrix_Scale(effect->scale, effect->scale, effect->scale, MTXMODE_APPLY);
            MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx);
            gSPDisplayList(POLY_OPA_DISP++, gOdolwaFallingBlockDL);
        }
    }

    effect = play->specialEffects;
    Gfx_SetupDL44_Xlu(play->state.gfxCtx);
    gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 0, 0, 0, 100);

    for (i = 1; i < ODOLWA_EFFECT_COUNT; i++, effect++) {
        if (effect->type == ODOLWA_EFFECT_FALLING_BLOCK) {
            Matrix_Translate(effect->pos.x, 0.0f, effect->pos.z, MTXMODE_NEW);
            Matrix_Scale(effect->scale * 50.0f, 1.0f, effect->scale * 50.0f, MTXMODE_APPLY);
            MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx);
            gSPDisplayList(POLY_XLU_DISP++, SEGMENTED_TO_K0(gCircleShadowDL));
        }
    }

    Gfx_SetupDL25_Xlu(play->state.gfxCtx);
    effect = play->specialEffects;
    Boss01_InitRand(1, 0x71A5, 0x263A);

    if (effect->type == ODOLWA_EFFECT_RING_OF_FIRE) {
        gDPSetEnvColor(POLY_XLU_DISP++, 255, 10, 0, 0);
        Matrix_Translate(effect->pos.x, 0.0f, effect->pos.z, MTXMODE_NEW);

        for (i = 0; i < 32; i++) {
            Matrix_Push();
            alpha = effect->alpha - (Boss01_RandZeroOne() * 50.0f);
            if (alpha < 0.0f) {
                alpha = 0.0f;
            }

            gDPSetPrimColor(POLY_XLU_DISP++, 0x80, 0x80, 255, 255, 0, (u8)alpha);
            gSPSegment(POLY_XLU_DISP++, 0x08,
                       Gfx_TwoTexScrollEx(play->state.gfxCtx, G_TX_RENDERTILE, 0, 0, 32, 64, 1, 0,
                                          ((effect->timer + (i * 10)) * -20) & 0x1FF, 32, 128, 0, 0, 0, -20));

            Matrix_RotateYF(i * (M_PIf / 16.0f), MTXMODE_APPLY);
            Matrix_Translate(0.0f, 0.0f, KREG(49) + 200.0f, MTXMODE_APPLY);
            Matrix_ReplaceRotation(&play->billboardMtxF);
            if (Boss01_RandZeroOne() < 0.5f) {
                Matrix_RotateYF(M_PIf, MTXMODE_APPLY);
            }

            Matrix_Scale(KREG(48) * 0.0001f + 0.018f,
                         ((0.007f + KREG(54) * 0.0001f) + (Boss01_RandZeroOne() * 30.0f * 0.0001f)) * effect->scale,
                         1.0f, MTXMODE_APPLY);

            MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx);
            gSPDisplayList(POLY_XLU_DISP++, gEffFire1DL);
            Matrix_Pop();
        }
    }

    CLOSE_DISPS(play->state.gfxCtx);
}

void Boss01_Reset(void) {
    sOdolwaBugCount = 0;
    sOdolwa = NULL;
    sMothSwarm = NULL;
    sOdolwaMusicStartTimer = 0;

    for (int i = 0; i < ODOLWA_EFFECT_COUNT; i++) {
        sOdolwaEffects[i].type = ODOLWA_EFFECT_NONE;
    }
}
