/*
 * File: z_dm_stk.c
 * Overlay: ovl_Dm_Stk
 * Description: Skull Kid. Also manages Majora's Mask in the cutscene where its backstory is explained.
 *
 * This actor is responsible for some somewhat unintuitive things. For example, during certain cutscenes,
 * it is responsible for handling the player's sound effects as well. As another example, this actor makes
 * time pass when the player looks through the Astral Observatory telescope.
 */

#include "z_dm_stk.h"
#include "objects/object_stk2/object_stk2.h"
#include "objects/object_stk3/object_stk3.h"
#include "2s2h/GameInteractor/GameInteractor.h"

#define FLAGS (ACTOR_FLAG_UPDATE_CULLING_DISABLED | ACTOR_FLAG_DRAW_CULLING_DISABLED | ACTOR_FLAG_UPDATE_DURING_OCARINA)

void DmStk_Init(Actor* thisx, PlayState* play);
void DmStk_Destroy(Actor* thisx, PlayState* play);
void DmStk_Update(Actor* thisx, PlayState* play);
void DmStk_Draw(Actor* thisx, PlayState* play);

void DmStk_ClockTower_DoNothing(DmStk* this, PlayState* play);
void DmStk_DoNothing(DmStk* this, PlayState* play);
void DmStk_WaitForTelescope(DmStk* this, PlayState* play);
void DmStk_StartTelescopeCutscene(DmStk* this, PlayState* play);
void DmStk_ClockTower_StartIntroCutsceneVersion1(DmStk* this, PlayState* play);
void DmStk_ClockTower_WaitForIntroCutsceneVersion1ToEnd(DmStk* this, PlayState* play);
void DmStk_ClockTower_StartIntroCutsceneVersion2(DmStk* this, PlayState* play);
void DmStk_ClockTower_WaitForIntroCutsceneVersion2ToEnd(DmStk* this, PlayState* play);
void DmStk_ClockTower_StartDropOcarinaCutscene(DmStk* this, PlayState* play);
void DmStk_ClockTower_WaitForDropOcarinaCutsceneToEnd(DmStk* this, PlayState* play);
void DmStk_ClockTower_DeflectHit(DmStk* this, PlayState* play);
void DmStk_ClockTower_WaitForDeflectionToEnd(DmStk* this, PlayState* play);
void DmStk_ClockTower_IdleWithOcarina(DmStk* this, PlayState* play);
void DmStk_ClockTower_Idle(DmStk* this, PlayState* play);

typedef enum {
    /*  0 */ SK_ANIM_SHAKE_HEAD,
    /*  1 */ SK_ANIM_WALK,
    /*  2 */ SK_ANIM_LOOK_AROUND, // Not used in the final game
    /*  3 */ SK_ANIM_IDLE,
    /*  4 */ SK_ANIM_UNUSED_IDLE, // Unused duplicate of SK_ANIM_IDLE
    /*  5 */ SK_ANIM_BENT_OVER_HEAD_TWITCH,
    /*  6 */ SK_ANIM_GLITCHY_HORSE_RIDE_START, // Not used in the final game
    /*  7 */ SK_ANIM_RECLINING_FLOAT,
    /*  8 */ SK_ANIM_CURSE_START,
    /*  9 */ SK_ANIM_CURSE_LOOP,
    /* 10 */ SK_ANIM_LAUGH_START,
    /* 11 */ SK_ANIM_LAUGH_LOOP,
    /* 12 */ SK_ANIM_HIDE_OCARINA_START,
    /* 13 */ SK_ANIM_HIDE_OCARINA_LOOP,
    /* 14 */ SK_ANIM_PICK_UP_OCARINA_AND_START_PLAYING,
    /* 15 */ SK_ANIM_PLAY_OCARINA_AND_LAUGH,
    /* 16 */ SK_ANIM_PICK_UP_OCARINA, // Not used in the final game
    /* 17 */ SK_ANIM_LOOK_AT_OCARINA, // Not used in the final game
    /* 18 */ SK_ANIM_KICK_OVER_LINK,
    /* 19 */ SK_ANIM_SEARCH_LINK,
    /* 20 */ SK_ANIM_UNUSED_KICK_OVER_LINK, // Unused duplicate of SK_ANIM_KICK_OVER_LINK
    /* 21 */ SK_ANIM_UNUSED_SEARCH_LINK,    // Unused duplicate of SK_ANIM_SEARCH_LINK
    /* 22 */ SK_ANIM_HORSE_RIDE_START,
    /* 23 */ SK_ANIM_HORSE_RIDE_LOOP,
    /* 24 */ SK_ANIM_RAISE_MASK_START,
    /* 25 */ SK_ANIM_RAISE_MASK_LOOP,
    /* 26 */ SK_ANIM_LOWER_MASK,
    /* 27 */ SK_ANIM_JUMP_WHILE_HIDING_OCARINA,
    /* 28 */ SK_ANIM_HORSE_RIDE_AND_ROTATE, // Used when Link gets thrown off his horse
    /* 29 */ SK_ANIM_PLAY_OCARINA_WHILE_FLOATING,
    /* 30 */ SK_ANIM_FLOATING_TURN_AROUND,
    /* 31 */ SK_ANIM_OCARINA_JUGGLE,
    /* 32 */ SK_ANIM_CALL_DOWN_MOON_START,
    /* 33 */ SK_ANIM_CALL_DOWN_MOON_LOOP,
    /* 34 */ SK_ANIM_SMACK_FAIRY_START,
    /* 35 */ SK_ANIM_SMACK_FAIRY_LOOP,
    /* 36 */ SK_ANIM_HIT_BY_BUBBLE,
    /* 37 */ SK_ANIM_DROP_OCARINA,
    /* 38 */ SK_ANIM_FLOATING_ARMS_CROSSED,
    /* 39 */ SK_ANIM_DEFLECT_ATTACK,
    /* 40 */ SK_ANIM_TELESCOPE_LOOK_UP_START,
    /* 41 */ SK_ANIM_TELESCOPE_LOOK_UP_LOOP,
    /* 42 */ SK_ANIM_SURPRISE_START,
    /* 43 */ SK_ANIM_SURPRISE_LOOP,
    /* 44 */ SK_ANIM_LOOK_AROUND_FOR_GIANTS_START,
    /* 45 */ SK_ANIM_LOOK_AROUND_FOR_GIANTS_LOOP,
    /* 46 */ SK_ANIM_HOLD_HEAD_AND_SHAKE_START,
    /* 47 */ SK_ANIM_HOLD_HEAD_AND_SHAKE_LOOP,
    /* 48 */ SK_ANIM_HOLD_HEAD_AND_SCREAM_START,
    /* 49 */ SK_ANIM_HOLD_HEAD_AND_SCREAM_LOOP,
    /* 50 */ SK_ANIM_HUDDLE_WITH_FAIRIES,
    /* 51 */ SK_ANIM_SEARCH_MASK_SALESMAN,
    /* 52 */ SK_ANIM_HOLD_UP_MASK_START,
    /* 53 */ SK_ANIM_HOLD_UP_MASK_LOOP,
    /* 54 */ SK_ANIM_SHIVER,
    /* 55 */ SK_ANIM_DRAW,
    /* 56 */ SK_ANIM_BENT_OVER_LOOK_UP,
    /* 57 */ SK_ANIM_SPANK,
    /* 58 */ SK_ANIM_HIP_SHAKE_AND_JUMP,
    /* 59 */ SK_ANIM_PLAY_FLUTE,
    /* 60 */ SK_ANIM_CARTWHEEL,
    /* 61 */ SK_ANIM_LIE_FLAT,
    /* 62 */ SK_ANIM_DANGLE_FROM_MASK_START,
    /* 63 */ SK_ANIM_DANGLE_FROM_MASK_LOOP,
    /* 64 */ SK_ANIM_DROPPED_FROM_MASK,
    /* 65 */ SK_ANIM_LOOK_UP_AT_GIANTS,
    /* 66 */ SK_ANIM_ASHAMED_START,
    /* 67 */ SK_ANIM_ASHAMED_LOOP,
    /* 68 */ SK_ANIM_LOOK_LEFT_START,
    /* 69 */ SK_ANIM_LOOK_LEFT_LOOP,
    /* 70 */ SK_ANIM_SNIFF,
    /* 71 */ SK_ANIM_LAUGH_AFTER_SNIFF,
    /* 72 */ SK_ANIM_LAUGH_AFTER_MAX
} SkullKidAnimation;

typedef enum {
    /* 0 */ SK_MASK_TYPE_NO_MASK,
    /* 1 */ SK_MASK_TYPE_NORMAL,
    /* 2 */ SK_MASK_TYPE_GLOWING_EYES,
    /* 3 */ SK_MASK_TYPE_FADING_OUT,
    /* 9 */ SK_MASK_TYPE_RAISED = 9
} SkullKidMaskType;

typedef enum {
    /* 0 */ SK_HAND_TYPE_HOLDING_LINK_MASK_AND_FLUTE,
    /* 1 */ SK_HAND_TYPE_HOLDING_LINK_MASK,
    /* 2 */ SK_HAND_TYPE_HOLDING_MAJORAS_MASK,
    /* 3 */ SK_HAND_TYPE_HOLDING_OCARINA,
    /* 4 */ SK_HAND_TYPE_JUGGLING_OR_DROPPING_OCARINA,
    /* 5 */ SK_HAND_TYPE_HOLDING_MAJORAS_MASK_AND_FLUTE,
    /* 6 */ SK_HAND_TYPE_HOLDING_FLUTE,
    /* 9 */ SK_HAND_TYPE_DEFAULT = 9
} SkullKidHandType;

typedef enum {
    /* 0 */ SK_FADE_IN_STATE_NONE,
    /* 1 */ SK_FADE_IN_STATE_START,
    /* 2 */ SK_FADE_IN_STATE_INCREASE_FOG,
    /* 3 */ SK_FADE_IN_STATE_INCREASE_ALPHA
} SkullKidFadeInState;

typedef enum {
    /* 0 */ SK_FADE_OUT_STATE_NONE,
    /* 1 */ SK_FADE_OUT_STATE_FADING_OUT
} SkullKidFadeOutState;

typedef enum {
    /* 0 */ SK_DEKU_PIPES_CS_STATE_NOT_READY,
    /* 1 */ SK_DEKU_PIPES_CS_STATE_READY,
    /* 2 */ SK_DEKU_PIPES_CS_STATE_PLAYER_USED_OCARINA,
    /* 3 */ SK_DEKU_PIPES_CS_STATE_START,
    /* 4 */ SK_DEKU_PIPES_CS_STATE_END
} SkullKidDekuPipesCutsceneState;

ActorProfile Dm_Stk_Profile = {
    /**/ ACTOR_DM_STK,
    /**/ ACTORCAT_ITEMACTION,
    /**/ FLAGS,
    /**/ OBJECT_STK,
    /**/ sizeof(DmStk),
    /**/ DmStk_Init,
    /**/ DmStk_Destroy,
    /**/ DmStk_Update,
    /**/ DmStk_Draw,
};

static ColliderCylinderInit sCylinderInit = {
    {
        COL_MATERIAL_HIT1,
        AT_NONE,
        AC_ON | AC_HARD | AC_TYPE_PLAYER | AC_TYPE_ENEMY,
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
    { 14, 38, 0, { 0, 0, 0 } },
};

static CollisionCheckInfoInit2 sColChkInfoInit = { 1, 0, 0, 0, MASS_IMMOVABLE };

static DamageTable sDamageTable = {
    /* Deku Nut       */ DMG_ENTRY(1, 0xF),
    /* Deku Stick     */ DMG_ENTRY(1, 0xF),
    /* Horse trample  */ DMG_ENTRY(1, 0xF),
    /* Explosives     */ DMG_ENTRY(1, 0xF),
    /* Zora boomerang */ DMG_ENTRY(1, 0xF),
    /* Normal arrow   */ DMG_ENTRY(1, 0xF),
    /* UNK_DMG_0x06   */ DMG_ENTRY(1, 0xF),
    /* Hookshot       */ DMG_ENTRY(1, 0xF),
    /* Goron punch    */ DMG_ENTRY(1, 0xF),
    /* Sword          */ DMG_ENTRY(1, 0xF),
    /* Goron pound    */ DMG_ENTRY(1, 0xF),
    /* Fire arrow     */ DMG_ENTRY(1, 0xF),
    /* Ice arrow      */ DMG_ENTRY(1, 0xF),
    /* Light arrow    */ DMG_ENTRY(1, 0xF),
    /* Goron spikes   */ DMG_ENTRY(1, 0xF),
    /* Deku spin      */ DMG_ENTRY(1, 0xF),
    /* Deku bubble    */ DMG_ENTRY(1, 0xF),
    /* Deku launch    */ DMG_ENTRY(1, 0xF),
    /* UNK_DMG_0x12   */ DMG_ENTRY(1, 0xF),
    /* Zora barrier   */ DMG_ENTRY(1, 0xF),
    /* Normal shield  */ DMG_ENTRY(1, 0xF),
    /* Light ray      */ DMG_ENTRY(1, 0xF),
    /* Thrown object  */ DMG_ENTRY(1, 0xF),
    /* Zora punch     */ DMG_ENTRY(1, 0xF),
    /* Spin attack    */ DMG_ENTRY(1, 0xF),
    /* Sword beam     */ DMG_ENTRY(1, 0xF),
    /* Normal Roll    */ DMG_ENTRY(1, 0xF),
    /* UNK_DMG_0x1B   */ DMG_ENTRY(1, 0xF),
    /* UNK_DMG_0x1C   */ DMG_ENTRY(1, 0xF),
    /* Unblockable    */ DMG_ENTRY(1, 0xF),
    /* UNK_DMG_0x1E   */ DMG_ENTRY(1, 0xF),
    /* Powder Keg     */ DMG_ENTRY(1, 0xF),
};

static AnimationInfo sAnimationInfo[SK_ANIM_LAUGH_AFTER_MAX] = {
    { &gSkullKidShakeHeadAnim, 1.0f, 0.0f, -1.0f, ANIMMODE_LOOP, 0.0f },             // SK_ANIM_SHAKE_HEAD
    { &gSkullKidWalkAnim, 1.0f, 0.0f, -1.0f, ANIMMODE_LOOP, 0.0f },                  // SK_ANIM_WALK
    { &gSkullKidLookAroundAnim, 1.0f, 0.0f, -1.0f, ANIMMODE_ONCE, 0.0f },            // SK_ANIM_LOOK_AROUND
    { &gSkullKidIdleAnim, 1.0f, 0.0f, -1.0f, ANIMMODE_LOOP, 0.0f },                  // SK_ANIM_IDLE
    { &gSkullKidIdleAnim, 1.0f, 0.0f, -1.0f, ANIMMODE_LOOP, 0.0f },                  // SK_ANIM_UNUSED_IDLE
    { &gSkullKidBentOverHeadTwitchAnim, 1.0f, 0.0f, -1.0f, ANIMMODE_LOOP, 0.0f },    // SK_ANIM_BENT_OVER_HEAD_TWITCH
    { &gSkullKidGlitchyHorseRideStartAnim, 1.0f, 0.0f, -1.0f, ANIMMODE_LOOP, 0.0f }, // SK_ANIM_GLITCHY_HORSE_RIDE_START
    { &gSkullKidRecliningFloatAnim, 1.0f, 0.0f, -1.0f, ANIMMODE_LOOP, 0.0f },        // SK_ANIM_RECLINING_FLOAT
    { &gSkullKidCurseStartAnim, 1.0f, 0.0f, -1.0f, ANIMMODE_ONCE, 0.0f },            // SK_ANIM_CURSE_START
    { &gSkullKidCurseLoopAnim, 1.0f, 0.0f, -1.0f, ANIMMODE_LOOP, 0.0f },             // SK_ANIM_CURSE_LOOP
    { &gSkullKidLaughStartAnim, 1.0f, 0.0f, -1.0f, ANIMMODE_ONCE, 0.0f },            // SK_ANIM_LAUGH_START
    { &gSkullKidLaughLoopAnim, 1.0f, 0.0f, -1.0f, ANIMMODE_LOOP, 0.0f },             // SK_ANIM_LAUGH_LOOP
    { &gSkullKidHideOcarinaStartAnim, 1.0f, 0.0f, -1.0f, ANIMMODE_ONCE, 0.0f },      // SK_ANIM_HIDE_OCARINA_START
    { &gSkullKidHideOcarinaLoopAnim, 1.0f, 0.0f, -1.0f, ANIMMODE_LOOP, 0.0f },       // SK_ANIM_HIDE_OCARINA_LOOP
    // SK_ANIM_PICK_UP_OCARINA_AND_START_PLAYING
    { &gSkullKidPickUpOcarinaAndStartPlayingAnim, 1.0f, 0.0f, -1.0f, ANIMMODE_ONCE, 0.0f },
    { &gSkullKidPlayOcarinaAndLaughAnim, 1.0f, 0.0f, -1.0f, ANIMMODE_LOOP, 0.0f }, // SK_ANIM_PLAY_OCARINA_AND_LAUGH
    { &gSkullKidPickUpOcarinaAnim, 1.0f, 0.0f, -1.0f, ANIMMODE_ONCE, 0.0f },       // SK_ANIM_PICK_UP_OCARINA
    { &gSkullKidLookAtOcarinaAnim, 1.0f, 0.0f, -1.0f, ANIMMODE_LOOP, 0.0f },       // SK_ANIM_LOOK_AT_OCARINA
    { &gSkullKidKickOverLinkAnim, 1.0f, 0.0f, -1.0f, ANIMMODE_ONCE, 0.0f },        // SK_ANIM_KICK_OVER_LINK
    { &gSkullKidSearchLinkAnim, 1.0f, 0.0f, -1.0f, ANIMMODE_LOOP, 0.0f },          // SK_ANIM_SEARCH_LINK
    { &gSkullKidKickOverLinkAnim, 1.0f, 0.0f, -1.0f, ANIMMODE_ONCE, 0.0f },        // SK_ANIM_UNUSED_KICK_OVER_LINK
    { &gSkullKidSearchLinkAnim, 1.0f, 0.0f, -1.0f, ANIMMODE_LOOP, 0.0f },          // SK_ANIM_UNUSED_SEARCH_LINK
    { &gSkullKidHorseRideStartAnim, 1.0f, 0.0f, -1.0f, ANIMMODE_ONCE, 0.0f },      // SK_ANIM_HORSE_RIDE_START
    { &gSkullKidHorseRideLoopAnim, 1.0f, 0.0f, -1.0f, ANIMMODE_LOOP, 0.0f },       // SK_ANIM_HORSE_RIDE_LOOP
    { &gSkullKidRaiseMaskStartAnim, 1.0f, 0.0f, -1.0f, ANIMMODE_ONCE, 0.0f },      // SK_ANIM_RAISE_MASK_START
    { &gSkullKidRaiseMaskLoopAnim, 1.0f, 0.0f, -1.0f, ANIMMODE_LOOP, 0.0f },       // SK_ANIM_RAISE_MASK_LOOP
    { &gSkullKidLowerMaskAnim, 1.0f, 0.0f, -1.0f, ANIMMODE_ONCE, 0.0f },           // SK_ANIM_LOWER_MASK
    // SK_ANIM_JUMP_WHILE_HIDING_OCARINA
    { &gSkullKidJumpWhileHidingOcarinaAnim, 1.0f, 0.0f, -1.0f, ANIMMODE_ONCE, 0.0f },
    { &gSkullKidHorseRideAndRotateAnim, 1.0f, 0.0f, -1.0f, ANIMMODE_ONCE, 0.0f }, // SK_ANIM_HORSE_RIDE_AND_ROTATE
    // SK_ANIM_PLAY_OCARINA_WHILE_FLOATING
    { &gSkullKidPlayOcarinaWhileFloatingAnim, 1.0f, 0.0f, -1.0f, ANIMMODE_LOOP, 0.0f },
    { &gSkullKidFloatingTurnAroundAnim, 1.0f, 0.0f, -1.0f, ANIMMODE_ONCE, 0.0f },   // SK_ANIM_FLOATING_TURN_AROUND
    { &gSkullKidOcarinaJuggleAnim, 1.0f, 0.0f, -1.0f, ANIMMODE_LOOP, 0.0f },        // SK_ANIM_OCARINA_JUGGLE
    { &gSkullKidCallDownMoonStartAnim, 1.0f, 0.0f, -1.0f, ANIMMODE_ONCE, 0.0f },    // SK_ANIM_CALL_DOWN_MOON_START
    { &gSkullKidCallDownMoonLoopAnim, 1.0f, 0.0f, -1.0f, ANIMMODE_LOOP, 0.0f },     // SK_ANIM_CALL_DOWN_MOON_LOOP
    { &gSkullKidSmackFairyStartAnim, 1.0f, 0.0f, -1.0f, ANIMMODE_ONCE, 0.0f },      // SK_ANIM_SMACK_FAIRY_START
    { &gSkullKidSmackFairyLoopAnim, 1.0f, 0.0f, -1.0f, ANIMMODE_LOOP, 0.0f },       // SK_ANIM_SMACK_FAIRY_LOOP
    { &gSkullKidHitByBubbleAnim, 1.0f, 0.0f, -1.0f, ANIMMODE_ONCE, 0.0f },          // SK_ANIM_HIT_BY_BUBBLE
    { &gSkullKidDropOcarinaAnim, 1.0f, 0.0f, -1.0f, ANIMMODE_ONCE, 0.0f },          // SK_ANIM_DROP_OCARINA
    { &gSkullKidFloatingArmsCrossedAnim, 1.0f, 0.0f, -1.0f, ANIMMODE_LOOP, 0.0f },  // SK_ANIM_FLOATING_ARMS_CROSSED
    { &gSkullKidDeflectAttackAnim, 1.0f, 0.0f, -1.0f, ANIMMODE_ONCE, 0.0f },        // SK_ANIM_DEFLECT_ATTACK
    { &gSkullKidTelescopeLookUpStartAnim, 1.0f, 0.0f, -1.0f, ANIMMODE_ONCE, 0.0f }, // SK_ANIM_TELESCOPE_LOOK_UP_START
    { &gSkullKidTelescopeLookUpLoopAnim, 1.0f, 0.0f, -1.0f, ANIMMODE_LOOP, 0.0f },  // SK_ANIM_TELESCOPE_LOOK_UP_LOOP
    { &gSkullKidSurpriseStartAnim, 1.0f, 0.0f, -1.0f, ANIMMODE_ONCE, 0.0f },        // SK_ANIM_SURPRISE_START
    { &gSkullKidSurpriseLoopAnim, 1.0f, 0.0f, -1.0f, ANIMMODE_LOOP, 0.0f },         // SK_ANIM_SURPRISE_LOOP
    // SK_ANIM_LOOK_AROUND_FOR_GIANTS_START
    { &gSkullKidLookAroundForGiantsStartAnim, 1.0f, 0.0f, -1.0f, ANIMMODE_ONCE, 0.0f },
    // SK_ANIM_LOOK_AROUND_FOR_GIANTS_LOOP
    { &gSkullKidLookAroundForGiantsLoopAnim, 1.0f, 0.0f, -1.0f, ANIMMODE_LOOP, 0.0f },
    { &gSkullKidHoldHeadAndShakeStartAnim, 1.0f, 0.0f, -1.0f, ANIMMODE_ONCE,
      0.0f },                                                                       // SK_ANIM_HOLD_HEAD_AND_SHAKE_START
    { &gSkullKidHoldHeadAndShakeLoopAnim, 1.0f, 0.0f, -1.0f, ANIMMODE_LOOP, 0.0f }, // SK_ANIM_HOLD_HEAD_AND_SHAKE_LOOP
    // SK_ANIM_HOLD_HEAD_AND_SCREAM_START
    { &gSkullKidHoldHeadAndScreamStartAnim, 1.0f, 0.0f, -1.0f, ANIMMODE_ONCE, 0.0f },
    // SK_ANIM_HOLD_HEAD_AND_SCREAM_LOOP
    { &gSkullKidHoldHeadAndScreamLoopAnim, 1.0f, 0.0f, -1.0f, ANIMMODE_LOOP, 0.0f },
    { &gSkullKidHuddleWithFairiesAnim, 1.0f, 0.0f, -1.0f, ANIMMODE_LOOP, 0.0f },   // SK_ANIM_HUDDLE_WITH_FAIRIES
    { &gSkullKidSearchMaskSalesmanAnim, 1.0f, 0.0f, -1.0f, ANIMMODE_LOOP, 0.0f },  // SK_ANIM_SEARCH_MASK_SALESMAN
    { &gSkullKidHoldUpMaskStartAnim, 1.0f, 0.0f, -1.0f, ANIMMODE_ONCE, 0.0f },     // SK_ANIM_HOLD_UP_MASK_START
    { &gSkullKidHoldUpMaskLoopAnim, 1.0f, 0.0f, -1.0f, ANIMMODE_LOOP, 0.0f },      // SK_ANIM_HOLD_UP_MASK_LOOP
    { &gSkullKidShiverAnim, 1.0f, 0.0f, -1.0f, ANIMMODE_LOOP, 0.0f },              // SK_ANIM_SHIVER
    { &gSkullKidDrawAnim, 1.0f, 0.0f, -1.0f, ANIMMODE_ONCE, 0.0f },                // SK_ANIM_DRAW
    { &gSkullKidBentOverLookUpAnim, 1.0f, 0.0f, -1.0f, ANIMMODE_ONCE, 0.0f },      // SK_ANIM_BENT_OVER_LOOK_UP
    { &gSkullKidSpankAnim, 1.0f, 0.0f, -1.0f, ANIMMODE_LOOP, 0.0f },               // SK_ANIM_SPANK
    { &gSkullKidHipShakeAndJumpAnim, 1.0f, 0.0f, -1.0f, ANIMMODE_ONCE, 0.0f },     // SK_ANIM_HIP_SHAKE_AND_JUMP
    { &gSkullKidPlayFluteAnim, 1.0f, 0.0f, -1.0f, ANIMMODE_LOOP, 0.0f },           // SK_ANIM_PLAY_FLUTE
    { &gSkullKidCartwheelAnim, 1.0f, 0.0f, -1.0f, ANIMMODE_ONCE, 0.0f },           // SK_ANIM_CARTWHEEL
    { &gSkullKidDangleFromMaskStartAnim, 1.0f, 0.0f, -1.0f, ANIMMODE_ONCE, 0.0f }, // SK_ANIM_LIE_FLAT
    { &gSkullKidDangleFromMaskStartAnim, 1.0f, 0.0f, -1.0f, ANIMMODE_ONCE, 0.0f }, // SK_ANIM_DANGLE_FROM_MASK_START
    { &gSkullKidDangleFromMaskLoopAnim, 1.0f, 0.0f, -1.0f, ANIMMODE_LOOP, 0.0f },  // SK_ANIM_DANGLE_FROM_MASK_LOOP
    { &gSkullKidDroppedFromMaskAnim, 1.0f, 0.0f, -1.0f, ANIMMODE_ONCE, 0.0f },     // SK_ANIM_DROPPED_FROM_MASK
    { &gSkullKidLookUpAtGiantsAnim, 1.0f, 0.0f, -1.0f, ANIMMODE_LOOP, 0.0f },      // SK_ANIM_LOOK_UP_AT_GIANTS
    { &gSkullKidAshamedStartAnim, 1.0f, 0.0f, -1.0f, ANIMMODE_ONCE, 0.0f },        // SK_ANIM_ASHAMED_START
    { &gSkullKidAshamedLoopAnim, 1.0f, 0.0f, -1.0f, ANIMMODE_LOOP, 0.0f },         // SK_ANIM_ASHAMED_LOOP
    { &gSkullKidLookLeftStartAnim, 1.0f, 0.0f, -1.0f, ANIMMODE_ONCE, 0.0f },       // SK_ANIM_LOOK_LEFT_START
    { &gSkullKidLookLeftLoopAnim, 1.0f, 0.0f, -1.0f, ANIMMODE_LOOP, 0.0f },        // SK_ANIM_LOOK_LEFT_LOOP
    { &gSkullKidSniffAnim, 1.0f, 0.0f, -1.0f, ANIMMODE_ONCE, 0.0f },               // SK_ANIM_SNIFF
    { &gSkullKidLaughAfterSniffAnim, 1.0f, 0.0f, -1.0f, ANIMMODE_LOOP, 0.0f },     // SK_ANIM_LAUGH_AFTER_SNIFF
};

/**
 * Ensures the correct object for the current animation is in segment 6.
 */
void DmStk_LoadObjectForAnimation(DmStk* this, PlayState* play) {
    s32 objectSlot;

    if (((this->animIndex >= SK_ANIM_SHAKE_HEAD) && (this->animIndex <= SK_ANIM_BENT_OVER_HEAD_TWITCH)) ||
        (this->animIndex == SK_ANIM_CALL_DOWN_MOON_START) || (this->animIndex == SK_ANIM_CALL_DOWN_MOON_LOOP) ||
        (this->animIndex == SK_ANIM_TELESCOPE_LOOK_UP_START) || (this->animIndex == SK_ANIM_TELESCOPE_LOOK_UP_LOOP)) {
        objectSlot = this->objectStkObjectSlot;
    } else if (this->animIndex >= SK_ANIM_LOOK_UP_AT_GIANTS) {
        objectSlot = this->objectStk3ObjectSlot;
    } else {
        objectSlot = this->objectStk2ObjectSlot;
    }

    if (objectSlot > OBJECT_SLOT_NONE) {
        gSegments[6] = OS_K0_TO_PHYSICAL(play->objectCtx.slots[objectSlot].segment);
    }
}

void DmStk_ChangeAnim(DmStk* this, PlayState* play, SkelAnime* skelAnime, AnimationInfo* animInfo, u16 animIndex) {
    f32 endFrame;

    DmStk_LoadObjectForAnimation(this, play);

    animInfo += animIndex;

    if (animInfo->frameCount < 0.0f) {
        endFrame = Animation_GetLastFrame(animInfo->animation);
    } else {
        endFrame = animInfo->frameCount;
    }

    Animation_Change(skelAnime, animInfo->animation, animInfo->playSpeed, animInfo->startFrame, endFrame,
                     animInfo->mode, animInfo->morphFrames);
}

/**
 * Handles sound effects for the first part of the intro cutscene, i.e., the cutscene
 * that starts when the player start a new game and ends when the screen fades to white
 * after Skull Kid steals Epona.
 */
void DmStk_PlaySfxForIntroCutsceneFirstPart(DmStk* this, PlayState* play) {
    switch (play->csCtx.curFrame + 20) {
        case 1195:
            Actor_PlaySfx(&this->actor, NA_SE_EN_STALKIDS_APPEAR);
            break;

        case 1232:
        case 1241:
        case 1252:
        case 1255:
        case 1257:
            Actor_PlaySfx(&this->actor, NA_SE_EN_STALKIDS_SHAKEHEAD);
            break;

        case 1285:
            Actor_PlaySfx(&this->actor, NA_SE_EN_STAL03_LAUGH_BIG);
            break;

        case 1343:
            Actor_PlaySfx(&this->actor, NA_SE_EN_STALKIDS_MASK_OFF);
            break;

        case 1410:
            Actor_PlaySfx(&this->actor, NA_SE_EN_STALKIDS_MASK_ON);
            break;

        case 1603:
            Actor_PlaySfx(&this->actor, NA_SE_EN_STALKIDS_WALK);
            break;

        case 1610:
            Actor_PlaySfx(&this->actor, NA_SE_EN_STALKIDS_REVERSE);
            break;

        case 2095:
            Actor_PlaySfx(&this->actor, NA_SE_EN_STALKIDS_SURPRISED);
            break;

        case 2190:
            Actor_PlaySfx(&this->actor, NA_SE_EN_STALKIDS_JUMP);
            break;

        case 2212:
            Actor_PlaySfx(&this->actor, NA_SE_EN_STALKIDS_ONGND);
            break;

        case 2214:
            Actor_PlaySfx(&this->actor, NA_SE_EN_STAL02_LAUGH_SHORT);
            break;

        case 2250:
            Actor_PlaySfx(&this->actor, NA_SE_EN_STAL06_SURPRISED);
            Actor_PlaySfx(&this->actor, NA_SE_EN_STALKIDS_DOWN_K);
            break;

        case 2255:
        case 2266:
        case 2277:
        case 2288:
        case 2299:
        case 2310:
        case 2321:
        case 2332:
        case 2343:
        case 2354:
        case 2365:
        case 2376:
        case 2387:
        case 2398:
        case 2409:
        case 2420:
        case 2431:
        case 2442:
        case 2453:
        case 2464:
        case 2475:
        case 2486:
        case 2497:
        case 2508:
        case 2519:
        case 2530:
            Actor_PlaySfx(&this->actor, NA_SE_EN_STALKIDS_RIDE);
            break;

        default:
            break;
    }
}

/**
 * This is supposed to play a bell sound in the title cutscene where the Skull Kid is
 * looking at the moon on top of the Clock Tower. However, it doesn't actually play
 * any sound in-game, since all sound effects are muted when it plays.
 */
void DmStk_PlaySfxForTitleCutscene(DmStk* this, PlayState* play) {
    if (play->csCtx.curFrame == 535) {
        Audio_PlaySfx_2(NA_SE_EV_CLOCK_TOWER_BELL);
    }
}

/**
 * Handles sound effects for the second part of the intro cutscene, i.e., the cutscene
 * that starts after the fade-to-white and ends when the player gains control.
 */
void DmStk_PlaySfxForIntroCutsceneSecondPart(DmStk* this, PlayState* play) {
    switch (play->csCtx.curFrame) {
        case 78:
        case 89:
        case 100:
        case 111:
        case 122:
        case 133:
        case 144:
        case 155:
        case 166:
        case 177:
        case 188:
        case 199:
        case 210:
        case 221:
        case 232:
        case 243:
        case 254:
            Actor_PlaySfx(&this->actor, NA_SE_EN_STALKIDS_RIDE);
            break;

        case 173:
            Actor_PlaySfx(&this->actor, NA_SE_EN_STAL03_LAUGH_BIG);
            break;

        default:
            break;
    }
}

/**
 * Handles sound effects for the cutscene where Skull Kid steals Majora's Mask from
 * the Happy Mask Salesman.
 */
void DmStk_PlaySfxForObtainingMajorasMaskCutscene(DmStk* this, PlayState* play) {
    switch (play->csCtx.curFrame) {
        case 18:
            Actor_PlaySfx(&this->actor, NA_SE_EN_STALKIDS_GASAGOSO);
            break;

        case 90:
            Actor_PlaySfx(&this->actor, NA_SE_EN_STALKIDS_MASK_ON);
            break;

        case 142:
            Actor_PlaySfx(&this->actor, NA_SE_EN_EVIL_POWER);
            break;

        default:
            break;
    }
}

/**
 * Handles sound effects for the first part of the curse cutscene, i.e., the cutscene
 * that starts when Link falls down the large hole and ends with a fade-to-black to
 * the hallucinatory Deku Scrubs scene.
 */
void DmStk_PlaySfxForCurseCutsceneFirstPart(DmStk* this, PlayState* play) {
    switch (play->csCtx.curFrame) {
        case 415:
            Audio_PlaySfx_AtPosWithVolumeTransition(&this->actor.projectedPos, NA_SE_EN_STALKIDS_FLOAT, 100);
            break;

        case 785:
            Audio_PlaySfx_2(NA_SE_SY_STALKIDS_PSYCHO);
            Audio_SetSfxVolumeTransition(&gSfxVolume, 0.0f, 150);
            break;

        case 560:
            Actor_PlaySfx(&this->actor, NA_SE_EN_STAL02_LAUGH_SHORT);
            break;

        case 890:
            Actor_PlaySfx(&this->actor, NA_SE_EN_STAL21_PSYCHO_VOICE);
            break;

        default:
            break;
    }
}

/**
 * Handles sound effects for the second part of the curse cutscene, i.e., the cutscene
 * that starts once the hallucinatory Deku Scrubs scene is over and ends when the player
 * gains control.
 */
void DmStk_PlaySfxForCurseCutsceneSecondPart(DmStk* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    switch (play->csCtx.curFrame) {
        case 10:
            Audio_PlaySfx_AtPosWithVolumeTransition(&this->actor.projectedPos, NA_SE_EN_STALKIDS_FLOAT, 50);
            break;

        case 71:
            Actor_PlaySfx(&this->actor, NA_SE_EN_STALKIDS_BODY);
            break;

        case 365:
            AudioSfx_SetChannelIO(&this->actor.projectedPos, NA_SE_EN_STALKIDS_FLOAT, 0);
            break;

        case 650:
            Audio_SetSfxVolumeTransition(&gSfxVolume, 0.0f, 80);
            break;

        case 265:
            Actor_PlaySfx(&this->actor, NA_SE_EN_STAL03_LAUGH_BIG);
            break;

        case 126:
            Actor_PlaySfx(&player->actor, NA_SE_VO_DUMMY_150);
            break;

        case 197:
            Actor_PlaySfx(&player->actor, NA_SE_VO_DUMMY_134);
            break;

        case 207:
            Actor_PlaySfx(&player->actor, NA_SE_VO_DUMMY_135);
            break;

        case 217:
            Actor_PlaySfx(&player->actor, NA_SE_VO_DUMMY_136);
            break;

        default:
            break;
    }

    if ((play->csCtx.curFrame >= 263) && (play->csCtx.curFrame < 698)) {
        Actor_PlaySfx(&player->actor, NA_SE_EN_STALKIDS_BODY_LEV - SFX_FLAG);
    }
}

/**
 * Handles sound effects for the intro cutscene at the top of the Clock Tower. Specifically,
 * it handles the variation of the cutscene that plays the first time the player reaches the
 * top of the Clock Tower, which is slightly longer.
 */
void DmStk_PlaySfxForClockTowerIntroCutsceneVersion1(DmStk* this, PlayState* play) {
    static s32 sMoonCallTimer = 0;

    switch (play->csCtx.curFrame) {
        case 140:
            Audio_PlaySfx_AtPosWithVolumeTransition(&this->actor.projectedPos, NA_SE_EN_STALKIDS_FLOAT, 80);
            break;

        case 258:
            Actor_PlaySfx(&this->actor, NA_SE_EN_STALKIDS_TURN);
            break;

        case 524:
            Actor_PlaySfx(&this->actor, NA_SE_EN_STALKIDS_TURN);
            Actor_PlaySfx(&this->actor, NA_SE_EN_STAL04_ANGER);
            break;

        case 534:
            Actor_PlaySfx(&this->actor, NA_SE_EN_PO_ROLL);
            break;

        case 678:
            Actor_PlaySfx(&this->actor, NA_SE_EN_STALKIDS_STRETCH);
            break;

        default:
            break;
    }

    if ((this->animIndex == SK_ANIM_OCARINA_JUGGLE) && (play->csCtx.curFrame < 700)) {
        if (Animation_OnFrame(&this->skelAnime, 5.0f) || Animation_OnFrame(&this->skelAnime, 25.0f)) {
            Actor_PlaySfx(&this->actor, NA_SE_EN_STALKIDS_OTEDAMA1);
        } else if (Animation_OnFrame(&this->skelAnime, 17.0f) || Animation_OnFrame(&this->skelAnime, 40.0f)) {
            Actor_PlaySfx(&this->actor, NA_SE_EN_STALKIDS_OTEDAMA2);
        }
    }

    if (play->csCtx.curFrame >= 700) {
        if (sMoonCallTimer < 128) {
            if ((sMoonCallTimer & 0x1F) == 0) {
                Actor_PlaySfx(&this->actor, NA_SE_EN_STAL20_CALL_MOON);
            } else if ((sMoonCallTimer & 0x1F) == 16) {
                Actor_PlaySfx(&this->actor, NA_SE_EN_STAL20_CALL_MOON2);
            }

            sMoonCallTimer++;
        }
    } else {
        sMoonCallTimer = 0;
    }
}

/**
 * Handles sound effects for the cutscene where Skull Kid drops the Ocarina of Time.
 */
void DmStk_PlaySfxForDroppingOcarinaCutscene(DmStk* this, PlayState* play) {
    if (play->csCtx.curFrame == 3) {
        Actor_PlaySfx(&this->actor, NA_SE_EN_STAL06_SURPRISED);
        Actor_PlaySfx(&this->actor, NA_SE_EN_STALKIDS_DOWN_K);
    }
}

/**
 * Handles sound effects for the cutscene where Skull Kid is shivering in the rain.
 */
void DmStk_PlaySfxForShiveringInRainCutscene(DmStk* this, PlayState* play) {
    if ((play->csCtx.curFrame >= 642) && (play->csCtx.curFrame < 845)) {
        Actor_PlaySfx(&this->actor, NA_SE_NE_STAL23_COLD - SFX_FLAG);
    }
}

/**
 * Handles sound effects for the cutscene where Skull Kid is playing with Tatl and Tael
 * in Termina Field.
 */
void DmStk_PlaySfxForPlayingWithFairiesCutscene(DmStk* this, PlayState* play) {
    switch (play->csCtx.curFrame) {
        case 58:
        case 61:
        case 68:
        case 72:
        case 77:
        case 79:
            Actor_PlaySfx(&this->actor, NA_SE_PL_WALK_GROUND + SURFACE_SFX_OFFSET_TALL_GRASS);
            Actor_PlaySfx(&this->actor, NA_SE_EN_STALKIDS_WALK);
            break;

        case 186:
            Actor_PlaySfx(&this->actor, NA_SE_EN_NUTS_UP);
            break;

        case 230:
            Actor_PlaySfx(&this->actor, NA_SE_EN_STAL22_LAUGH_KID_L);
            break;

        default:
            break;
    }
}

/**
 * Handles sound effects for the first part of the ending cutscene, i.e., the cutscene
 * that starts after the Dawn of the New Day screen and ends with a fade to black as
 * the Giants are walking away.
 */
void DmStk_PlaySfxForEndingCutsceneFirstPart(DmStk* this, PlayState* play) {
    switch (play->csCtx.curFrame) {
        case 5:
            Audio_PlayAmbience(AMBIENCE_ID_0C);
            break;

        case 660:
            Actor_PlaySfx(&this->actor, NA_SE_EN_STALKIDS_SHAKEHEAD);
            break;

        default:
            break;
    }
}

/**
 * Handles sound effects for the second part of the ending cutscene, i.e., the cutscene
 * that starts after a fade-to-black and ends after the credits start.
 */
void DmStk_PlaySfxForEndingCutsceneSecondPart(DmStk* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    switch (play->csCtx.curFrame) {
        case 5:
            Audio_PlayAmbience(AMBIENCE_ID_0C);
            break;

        case 45:
            Actor_PlaySfx(&this->actor, NA_SE_EN_STALKIDS_RIDE);
            break;

        case 93:
            Actor_PlaySfx(&this->actor, NA_SE_EN_STALKIDS_MASK_OFF);
            break;

        case 245:
            Actor_PlaySfx(&this->actor, NA_SE_EN_STALKIDS_RIDE);
            break;

        case 269:
            Actor_PlaySfx(&this->actor, NA_SE_EN_STAL11_LAUGH_SHY2);
            break;

        case 327:
            Actor_PlaySfx(&this->actor, NA_SE_EN_STALKIDS_SHAKEHEAD);
            break;

        case 455:
            Actor_PlaySfx(&this->actor, NA_SE_EN_STALKIDS_RIDE);
            break;

        case 1730:
            SEQCMD_STOP_SEQUENCE(SEQ_PLAYER_AMBIENCE, 20);
            break;

        case 1395:
            Player_PlaySfx(player, NA_SE_VO_DUMMY_34);
            break;

        case 1850:
            SEQCMD_SET_SEQPLAYER_VOLUME(SEQ_PLAYER_SFX, 50, 0);
            break;

        case 2000:
            AudioSfx_MuteBanks((1 << BANK_PLAYER) | (1 << BANK_ITEM) | (1 << BANK_ENV) | (1 << BANK_ENEMY) |
                               (1 << BANK_SYSTEM) | (1 << BANK_OCARINA) | (1 << BANK_VOICE));
            break;

        default:
            break;
    }

    if (this->animIndex == SK_ANIM_SHAKE_HEAD) {
        if (Animation_OnFrame(&this->skelAnime, 8.0f) || Animation_OnFrame(&this->skelAnime, 17.0f)) {
            Actor_PlaySfx(&this->actor, NA_SE_EN_STALKIDS_MASK_OFF);
        }

        if (Animation_OnFrame(&this->skelAnime, 28.0f)) {
            Actor_PlaySfx(&this->actor, NA_SE_EN_STALKIDS_RIDE);
        }
    } else if (this->animIndex == SK_ANIM_LAUGH_AFTER_SNIFF) {
        if (Animation_OnFrame(&this->skelAnime, 2.0f) || Animation_OnFrame(&this->skelAnime, 6.0f) ||
            Animation_OnFrame(&this->skelAnime, 12.0f) || Animation_OnFrame(&this->skelAnime, 18.0f)) {
            Actor_PlaySfx(&this->actor, NA_SE_EN_STALKIDS_MASK_OFF);
        }
    } else if ((this->animIndex == SK_ANIM_SNIFF) &&
               (Animation_OnFrame(&this->skelAnime, 16.0f) || Animation_OnFrame(&this->skelAnime, 23.0f))) {
        Actor_PlaySfx(&this->actor, NA_SE_EN_STALKIDS_NOSE);
    }
}

/**
 * Handles sound effects for the intro cutscene at the top of the Clock Tower. Specifically,
 * it handles the variation of the cutscene that plays the second or later time the player
 * reaches the top of the Clock Tower, which is slightly shorter.
 */
void DmStk_PlaySfxForClockTowerIntroCutsceneVersion2(DmStk* this, PlayState* play) {
    static s32 sMoonCallTimer = 0;

    switch (play->csCtx.curFrame) {
        case 40:
            Audio_PlaySfx_AtPosWithVolumeTransition(&this->actor.projectedPos, NA_SE_EN_STALKIDS_FLOAT, 80);
            break;

        case 234:
            Actor_PlaySfx(&this->actor, NA_SE_EN_STALKIDS_TURN);
            Actor_PlaySfx(&this->actor, NA_SE_EN_STAL04_ANGER);
            break;

        case 244:
            Actor_PlaySfx(&this->actor, NA_SE_EN_PO_ROLL);
            break;

        case 388:
            Actor_PlaySfx(&this->actor, NA_SE_EN_STALKIDS_STRETCH);
            break;

        default:
            break;
    }

    if (play->csCtx.curFrame >= 408) {
        if (sMoonCallTimer < 128) {
            if ((sMoonCallTimer & 0x1F) == 0) {
                Actor_PlaySfx(&this->actor, NA_SE_EN_STAL20_CALL_MOON);
            } else if ((sMoonCallTimer & 0x1F) == 16) {
                Actor_PlaySfx(&this->actor, NA_SE_EN_STAL20_CALL_MOON2);
            }

            sMoonCallTimer++;
        }
    } else {
        sMoonCallTimer = 0;
    }
}

/**
 * Handles sound effects for the cutscene that plays after the player plays the
 * Oath to Order at the top of the Clock Tower.
 */
void DmStk_PlaySfxForCutsceneAfterPlayingOathToOrder(DmStk* this, PlayState* play) {
    this->oathToOrderCutsceneVoicePos.x = this->actor.projectedPos.x;
    this->oathToOrderCutsceneVoicePos.y = this->actor.projectedPos.y;
    this->oathToOrderCutsceneVoicePos.z = this->actor.projectedPos.z;

    switch (play->csCtx.curFrame) {
        case 64:
            Audio_PlaySfx_AtPos(&this->oathToOrderCutsceneVoicePos, NA_SE_EN_STAL06_SURPRISED);
            break;

        case 327:
        case 332:
        case 335:
        case 344:
            Actor_PlaySfx(&this->actor, NA_SE_EN_STALKIDS_MASK_OFF);
            break;

        case 367:
            Actor_PlaySfx(&this->actor, NA_SE_EN_STALKIDS_BODY_LEV);
            break;

        case 470:
            Actor_PlaySfx(&this->actor, NA_SE_EN_STALKIDS_HEADACHE);
            break;

        case 486:
            Actor_PlaySfx(&this->actor, NA_SE_EN_STALKIDS_MASK_OFF);
            Audio_PlaySfx_AtPos(&this->oathToOrderCutsceneVoicePos, NA_SE_EN_STAL08_CRY_BIG);
            break;

        case 496:
            Audio_PlaySfx_AtPos(&this->oathToOrderCutsceneVoicePos, NA_SE_EN_STAL09_SCREAM);
            break;

        case 590:
            Actor_PlaySfx(&this->actor, NA_SE_EN_STALKIDS_MASK_OFF);
            break;

        case 592:
            Actor_PlaySfx(&this->actor, NA_SE_EN_STALKIDS_RIDE);
            break;

        case 594:
            Audio_PlaySfx_AtPos(&this->oathToOrderCutsceneVoicePos, NA_SE_EN_STAL24_SCREAM2);
            break;

        default:
            break;
    }

    if ((play->csCtx.curFrame >= 62) && (play->csCtx.curFrame < 273)) {
        if ((Rand_ZeroOne() < 0.75f) && ((play->state.frames % 2) != 0)) {
            Actor_PlaySfx(&this->actor, NA_SE_EN_STALKIDS_EARTHQUAKE);
        }
    }

    if ((play->csCtx.curFrame >= 498) && (play->csCtx.curFrame < 577)) {
        if ((play->state.frames % 4) == 0) {
            if ((play->state.frames & 4) != 0) {
                Actor_PlaySfx(&this->actor, NA_SE_EN_STALKIDS_BODY_LEV);
            } else {
                Actor_PlaySfx(&this->actor, NA_SE_EN_STALKIDS_MASK_OFF);
            }
        }
    }

    if (play->csCtx.curFrame >= 290) {
        Audio_PlaySfx_2(NA_SE_EV_KYOJIN_VOICE_SUCCESS - SFX_FLAG);
    }
}

/**
 * Handles sound effects for the cutscene before the player warps to the moon. Specifically,
 * it handles the variation of the cutscene that plays the first time the player warps to the
 * moon, which is slightly longer.
 */
void DmStk_PlaySfxForMoonWarpCutsceneVersion1(DmStk* this, PlayState* play) {
    switch (play->csCtx.curFrame) {
        case 551:
            Actor_PlaySfx(&this->actor, NA_SE_EN_STALKIDS_PULLED);
            break;

        case 711:
            Actor_PlaySfx(&this->actor, NA_SE_EN_STALKIDS_MASK_OFF);
            break;

        case 716:
            Actor_PlaySfx(&this->actor, NA_SE_EN_STALKIDS_MASK_ON);
            Actor_PlaySfx(&this->actor, NA_SE_EN_STALKIDS_ONGND);
            break;

        default:
            break;
    }
}

/**
 * Handles sound effects for the cutscene before the player warps to the moon. Specifically,
 * it handles the variation of the cutscene that plays the second or later time the player
 * warps to the moon, which is slightly shorter.
 */
void DmStk_PlaySfxForMoonWarpCutsceneVersion2(DmStk* this, PlayState* play) {
    switch (play->csCtx.curFrame) {
        case 311:
            Actor_PlaySfx(&this->actor, NA_SE_EN_STALKIDS_PULLED);
            break;

        case 365:
            Actor_PlaySfx(&this->actor, NA_SE_EN_STALKIDS_MASK_OFF);
            break;

        case 372:
            Actor_PlaySfx(&this->actor, NA_SE_EN_STALKIDS_MASK_ON);
            Actor_PlaySfx(&this->actor, NA_SE_EN_STALKIDS_ONGND);
            break;

        default:
            break;
    }
}

/**
 * Handles sound effects for all cutscenes.
 */
void DmStk_PlaySfxForCutscenes(DmStk* this, PlayState* play) {
    if (play->csCtx.state != CS_STATE_IDLE) {
        switch (play->sceneId) {
            case SCENE_LOST_WOODS:
                if (gSaveContext.sceneLayer == 1) {
                    DmStk_PlaySfxForIntroCutsceneFirstPart(this, play);
                } else if (gSaveContext.sceneLayer == 0) {
                    DmStk_PlaySfxForIntroCutsceneSecondPart(this, play);
                } else if ((gSaveContext.sceneLayer == 2) && (play->csCtx.scriptIndex == 0)) {
                    DmStk_PlaySfxForObtainingMajorasMaskCutscene(this, play);
                }
                break;

            case SCENE_CLOCKTOWER:
                if (gSaveContext.sceneLayer == 1) {
                    DmStk_PlaySfxForTitleCutscene(this, play);
                }
                break;

            case SCENE_OPENINGDAN:
                if (gSaveContext.sceneLayer == 0) {
                    if (play->csCtx.scriptIndex == 0) {
                        DmStk_PlaySfxForCurseCutsceneFirstPart(this, play);
                    } else if (play->csCtx.scriptIndex == 1) {
                        DmStk_PlaySfxForCurseCutsceneSecondPart(this, play);
                    }
                }
                break;

            case SCENE_OKUJOU:
                if (gSaveContext.sceneLayer == 0) {
                    if (play->csCtx.scriptIndex == 0) {
                        DmStk_PlaySfxForClockTowerIntroCutsceneVersion1(this, play);
                    } else if (play->csCtx.scriptIndex == 1) {
                        DmStk_PlaySfxForDroppingOcarinaCutscene(this, play);
                    } else if (play->csCtx.scriptIndex == 2) {
                        DmStk_PlaySfxForClockTowerIntroCutsceneVersion2(this, play);
                    } else if (play->csCtx.scriptIndex == 3) {
                        DmStk_PlaySfxForCutsceneAfterPlayingOathToOrder(this, play);
                    }
                } else if (gSaveContext.sceneLayer == 2) {
                    if (play->csCtx.scriptIndex == 0) {
                        DmStk_PlaySfxForMoonWarpCutsceneVersion1(this, play);
                    } else if (play->csCtx.scriptIndex == 1) {
                        DmStk_PlaySfxForMoonWarpCutsceneVersion2(this, play);
                    }
                }
                break;

            case SCENE_00KEIKOKU:
                if (gSaveContext.sceneLayer == 3) {
                    if (play->csCtx.scriptIndex == 0) {
                        DmStk_PlaySfxForShiveringInRainCutscene(this, play);
                    } else if (play->csCtx.scriptIndex == 2) {
                        DmStk_PlaySfxForPlayingWithFairiesCutscene(this, play);
                    }
                } else if (gSaveContext.sceneLayer == 7) {
                    if (play->csCtx.scriptIndex == 0) {
                        DmStk_PlaySfxForEndingCutsceneFirstPart(this, play);
                    } else if (play->csCtx.scriptIndex == 1) {
                        DmStk_PlaySfxForEndingCutsceneSecondPart(this, play);
                    }
                }
                break;

            default:
                break;
        }
    }

    if (this->animIndex == SK_ANIM_WALK) {
        if (Animation_OnFrame(&this->skelAnime, 2.0f) || Animation_OnFrame(&this->skelAnime, 6.0f)) {
            Actor_PlaySfx(&this->actor, NA_SE_EN_STALKIDS_WALK);
        }
    } else if (this->animIndex == SK_ANIM_SEARCH_LINK) {
        if (Animation_OnFrame(&this->skelAnime, 0.0f) || Animation_OnFrame(&this->skelAnime, 13.0f) ||
            Animation_OnFrame(&this->skelAnime, 20.0f) || Animation_OnFrame(&this->skelAnime, 27.0f)) {
            Actor_PlaySfx(&this->actor, NA_SE_PL_CALM_HIT);
        }
    } else if (this->animIndex == SK_ANIM_PICK_UP_OCARINA_AND_START_PLAYING) {
        if (Animation_OnFrame(&this->skelAnime, 3.0f)) {
            Actor_PlaySfx(&this->actor, NA_SE_PL_PUT_OUT_ITEM);
        }
    } else if (this->animIndex == SK_ANIM_PLAY_OCARINA_AND_LAUGH) {
        if (Animation_OnFrame(&this->skelAnime, 14.0f)) {
            Actor_PlaySfx(&this->actor, NA_SE_EV_UNSKILLFUL_OCARINA);
        }

        if (Animation_OnFrame(&this->skelAnime, 45.0f)) {
            Actor_PlaySfx(&this->actor, NA_SE_EN_STAL01_LAUGH);
        }
    }
}

void DmStk_Init(Actor* thisx, PlayState* play) {
    s32 pad;
    DmStk* this = (DmStk*)thisx;

    this->shouldDraw = true;
    if (DM_STK_GET_TYPE(&this->actor) != DM_STK_TYPE_MAJORAS_MASK) {
        this->dekuPipesCutsceneState = SK_DEKU_PIPES_CS_STATE_NOT_READY;
        this->objectStkObjectSlot = Object_GetSlot(&play->objectCtx, OBJECT_STK);
        this->objectStk2ObjectSlot = Object_GetSlot(&play->objectCtx, OBJECT_STK2);
        this->objectStk3ObjectSlot = Object_GetSlot(&play->objectCtx, OBJECT_STK3);
        if (this->objectStkObjectSlot <= OBJECT_SLOT_NONE) {
            Actor_Kill(&this->actor);
        }

        this->tatlMessageTimer = 0;
        this->deflectCount = 0;
        this->maskType = SK_MASK_TYPE_NORMAL;
        this->animIndex = SK_ANIM_IDLE;
        this->fogR = play->lightCtx.fogColor[0];
        this->fogG = play->lightCtx.fogColor[1];
        this->fogB = play->lightCtx.fogColor[2];

        if ((play->sceneId == SCENE_LOST_WOODS) && (gSaveContext.sceneLayer == 1)) {
            this->alpha = 0;
            this->fogN = 0;
            this->fogF = 1000;
            this->fogScale = 1.0f;
            this->actionFunc = DmStk_DoNothing;
        } else if (play->sceneId == SCENE_OKUJOU) {
            this->alpha = 255;
            this->fogN = 996;
            this->fogF = 1000;
            this->fogScale = 0.7f;
            this->hasBeenHit = false;

            Collider_InitCylinder(play, &this->collider);

            if (gSaveContext.save.entrance == ENTRANCE(CLOCK_TOWER_ROOFTOP, 0)) {
                if (gSaveContext.sceneLayer == 0) {
                    if (gSaveContext.timerStates[TIMER_ID_MOON_CRASH] == TIMER_STATE_OFF) {
                        // Starts a 5 minute (300 second) timer until the moon falls.
                        Interface_StartTimer(TIMER_ID_MOON_CRASH, 300);
                        R_MOON_CRASH_TIMER_Y = 200;
                        R_MOON_CRASH_TIMER_X = 115;
                    }

                    if (GameInteractor_Should(VB_STK_HAVE_OCARINA,
                                              gSaveContext.save.saveInfo.inventory.items[SLOT_OCARINA] == ITEM_NONE)) {
                        sCylinderInit.base.colMaterial = COL_MATERIAL_WOOD;
                        this->actionFunc = DmStk_ClockTower_StartIntroCutsceneVersion1;
                    } else {
                        sCylinderInit.base.colMaterial = COL_MATERIAL_WOOD;
                        this->actionFunc = DmStk_ClockTower_StartIntroCutsceneVersion2;
                    }

                } else if (gSaveContext.sceneLayer == 3) {
                    this->animIndex = SK_ANIM_FLOATING_ARMS_CROSSED;
                    if (gSaveContext.timerStates[TIMER_ID_MOON_CRASH] == TIMER_STATE_OFF) {
                        // This code is called when the Giants fail to stop the moon.
                        // Starts a 1 minute (60 second) timer until the moon falls.
                        Interface_StartTimer(TIMER_ID_MOON_CRASH, 60);
                        R_MOON_CRASH_TIMER_Y = 200;
                        R_MOON_CRASH_TIMER_X = 115;
                    }

                    this->actor.world.pos.y = 120.0f;
                    sCylinderInit.base.colMaterial = COL_MATERIAL_WOOD;
                    this->actionFunc = DmStk_ClockTower_Idle;
                } else {
                    this->animIndex = SK_ANIM_FLOATING_ARMS_CROSSED;
                    this->actionFunc = DmStk_ClockTower_DoNothing;
                }
            } else {
                this->dekuPipesCutsceneState = SK_DEKU_PIPES_CS_STATE_READY;
                this->animIndex = SK_ANIM_FLOATING_ARMS_CROSSED;
                this->actor.world.pos.y = 120.0f;
                sCylinderInit.base.colMaterial = COL_MATERIAL_WOOD;
                this->actionFunc = DmStk_ClockTower_Idle;
            }

            Collider_SetCylinder(play, &this->collider, &this->actor, &sCylinderInit);
            CollisionCheck_SetInfo2(&this->actor.colChkInfo, &sDamageTable, &sColChkInfoInit);

        } else if ((play->sceneId == SCENE_00KEIKOKU) && (gSaveContext.sceneLayer == 0)) {
            if (!(play->actorCtx.flags & ACTORCTX_FLAG_TELESCOPE_ON)) {
                Actor_Kill(&this->actor);
            }

            this->maskType = SK_MASK_TYPE_GLOWING_EYES;
            this->alpha = 255;
            this->fogN = 996;
            this->fogF = 1000;
            this->fogScale = 0.7f;
            this->animIndex = SK_ANIM_BENT_OVER_HEAD_TWITCH;
            this->actionFunc = DmStk_WaitForTelescope;
        } else {
            if ((play->sceneId == SCENE_LOST_WOODS) && !Cutscene_IsPlaying(play)) {
                Actor_Kill(&this->actor);
            }

            this->maskType = SK_MASK_TYPE_GLOWING_EYES;
            this->alpha = 255;
            this->fogN = 996;
            this->fogF = 1000;
            this->fogScale = 0.7f;
            this->actionFunc = DmStk_DoNothing;
        }

        this->handType = SK_HAND_TYPE_DEFAULT;
        this->fadeInState = SK_FADE_IN_STATE_NONE;
        this->fadeOutState = SK_FADE_OUT_STATE_NONE;
        this->fadeOutTimer = 0;
        this->alpha = this->alpha; // Set to itself
        this->actor.lockOnArrowOffset = 1100.0f;
        this->cueId = 99;
        ActorShape_Init(&this->actor.shape, 0.0f, ActorShadow_DrawCircle, 24.0f);
        SkelAnime_InitFlex(play, &this->skelAnime, &gSkullKidSkel, NULL, NULL, NULL, 0);
        DmStk_ChangeAnim(this, play, &this->skelAnime, &sAnimationInfo[this->animIndex], 0);
    }

    Actor_SetScale(&this->actor, 0.01f);

    if ((play->sceneId == SCENE_00KEIKOKU) && (gSaveContext.sceneLayer == 3) && (play->csCtx.scriptIndex > 0)) {
        play->envCtx.skyboxConfig = SKYBOX_CONFIG_15;
        play->envCtx.changeSkyboxNextConfig = SKYBOX_CONFIG_15;
    }
}

void DmStk_Destroy(Actor* thisx, PlayState* play) {
}

void DmStk_ClockTower_DoNothing(DmStk* this, PlayState* play) {
}

void DmStk_DoNothing(DmStk* this, PlayState* play) {
}

/**
 * Waits around until the player zooms the telescope in on Skull Kid,
 * then starts the telescope cutscene.
 */
void DmStk_WaitForTelescope(DmStk* this, PlayState* play) {
    Vec3f screenPos;

    if (!CHECK_WEEKEVENTREG(WEEKEVENTREG_74_20)) {
        Play_GetScreenPos(play, &this->actor.world.pos, &screenPos);
        if (play->view.fovy < 25.0f) {
            if ((screenPos.x >= 70.0f) && (screenPos.x < (SCREEN_WIDTH - 70.0f)) && (screenPos.y >= 30.0f) &&
                (screenPos.y < (SCREEN_HEIGHT - 30.0f))) {
                Environment_StopTime();
                this->actionFunc = DmStk_StartTelescopeCutscene;
            }
        }
    }
}

/**
 * Plays the cutscene in the telescope where the Moon's Tear falls.
 */
void DmStk_StartTelescopeCutscene(DmStk* this, PlayState* play) {
    s16 dayOneAndTwoCsId = this->actor.csId;
    s16 dayThreeCsId = CutsceneManager_GetAdditionalCsId(dayOneAndTwoCsId);
    s16 finalHoursCsId = CutsceneManager_GetAdditionalCsId(dayThreeCsId);
    s16 csId;

    if (gSaveContext.save.day < 3) {
        csId = dayOneAndTwoCsId;
    } else if (CHECK_WEEKEVENTREG(WEEKEVENTREG_CLOCK_TOWER_OPENED) ||
               ((CURRENT_DAY == 3) && (CURRENT_TIME < CLOCK_TIME(6, 0)))) {
        csId = finalHoursCsId;
    } else {
        csId = dayThreeCsId;
    }

    if (CutsceneManager_IsNext(csId)) {
        CutsceneManager_Start(csId, &this->actor);
        Environment_StartTime();
        this->actionFunc = DmStk_DoNothing;
    } else {
        CutsceneManager_Queue(csId);
    }
}

void DmStk_ClockTower_StartIntroCutsceneVersion1(DmStk* this, PlayState* play) {
    if (CutsceneManager_IsNext(9)) {
        CutsceneManager_Start(9, &this->actor);
        this->actionFunc = DmStk_ClockTower_WaitForIntroCutsceneVersion1ToEnd;
    } else {
        CutsceneManager_Queue(9);
    }
}

void DmStk_ClockTower_WaitForIntroCutsceneVersion1ToEnd(DmStk* this, PlayState* play) {
    if (play->csCtx.state == CS_STATE_IDLE) {
        this->animIndex = SK_ANIM_CALL_DOWN_MOON_LOOP;
        this->handType = SK_HAND_TYPE_HOLDING_OCARINA;
        DmStk_ChangeAnim(this, play, &this->skelAnime, &sAnimationInfo[this->animIndex], 0);
        this->actionFunc = DmStk_ClockTower_IdleWithOcarina;
    }
}

void DmStk_ClockTower_StartIntroCutsceneVersion2(DmStk* this, PlayState* play) {
    if (CutsceneManager_IsNext(11)) {
        CutsceneManager_Start(11, &this->actor);
        this->actionFunc = DmStk_ClockTower_WaitForIntroCutsceneVersion2ToEnd;
    } else {
        CutsceneManager_Queue(11);
    }
}

void DmStk_ClockTower_WaitForIntroCutsceneVersion2ToEnd(DmStk* this, PlayState* play) {
    if (play->csCtx.state == CS_STATE_IDLE) {
        this->animIndex = SK_ANIM_FLOATING_ARMS_CROSSED;
        DmStk_ChangeAnim(this, play, &this->skelAnime, &sAnimationInfo[this->animIndex], 0);
        this->actionFunc = DmStk_ClockTower_Idle;
    }
}

void DmStk_ClockTower_StartDropOcarinaCutscene(DmStk* this, PlayState* play) {
    if (CutsceneManager_IsNext(10)) {
        Actor_PlaySfx(&this->actor, NA_SE_EN_STALKIDS_DAMAGE);
        CutsceneManager_Start(10, &this->actor);
        this->actor.shape.rot.x = 0;
        this->actor.world.rot.x = this->actor.shape.rot.x;
        this->actor.shape.rot.y = this->actor.shape.rot.x;
        this->actor.world.rot.y = this->actor.shape.rot.x;
        this->actionFunc = DmStk_ClockTower_WaitForDropOcarinaCutsceneToEnd;
    } else {
        CutsceneManager_Queue(10);
    }
}

void DmStk_ClockTower_WaitForDropOcarinaCutsceneToEnd(DmStk* this, PlayState* play) {
    if ((play->csCtx.state != CS_STATE_IDLE) && (play->csCtx.curFrame > 20)) {
        this->actionFunc = DmStk_ClockTower_Idle;
    }
}

/**
 * Makes Skull Kid bob up and down and face the player.
 */
void DmStk_ClockTower_AdjustHeightAndRotation(DmStk* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    f32 sin;

    this->bobPhase += 0x4B0;
    sin = Math_SinS(this->bobPhase) * 10.0f;
    Math_SmoothStepToF(&this->actor.world.pos.y, 160.0f + sin, 0.2f, 1.0f, 0.0001f);

    this->actor.world.rot.y = Actor_WorldYawTowardActor(&this->actor, &player->actor);
    this->actor.shape.rot.y = this->actor.world.rot.y;

    this->actor.world.rot.x = 0x1B58;
    this->actor.shape.rot.x = this->actor.world.rot.x;
}

/**
 * Starts the deflection animation and, depending on how many times Skull Kid has been hit,
 * prints a message taunting the player.
 */
void DmStk_ClockTower_DeflectHit(DmStk* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    DmStk_ClockTower_AdjustHeightAndRotation(this, play);
    this->deflectCount++;
    if (this->deflectCount >= 3) {
        this->deflectCount = 0;
        if (!(player->stateFlags2 & PLAYER_STATE2_USING_OCARINA)) {
            // That won't do you any good
            Message_StartTextbox(play, 0x2013, &this->actor);
        }
    }

    this->animIndex = SK_ANIM_DEFLECT_ATTACK;
    DmStk_ChangeAnim(this, play, &this->skelAnime, &sAnimationInfo[this->animIndex], 0);
    Actor_PlaySfx(&this->actor, NA_SE_EN_STALKIDS_DOWN_K);
    this->actionFunc = DmStk_ClockTower_WaitForDeflectionToEnd;
}

/**
 * Resets Skull Kid back to his idle state once the deflection animation ends.
 * If he is hit again before the animation ends, this function will make his deflection restart.
 */
void DmStk_ClockTower_WaitForDeflectionToEnd(DmStk* this, PlayState* play) {
    DmStk_ClockTower_AdjustHeightAndRotation(this, play);
    if (Animation_OnFrame(&this->skelAnime, this->skelAnime.endFrame)) {
        this->animIndex = SK_ANIM_FLOATING_ARMS_CROSSED;
        DmStk_ChangeAnim(this, play, &this->skelAnime, &sAnimationInfo[this->animIndex], 0);
        this->actionFunc = DmStk_ClockTower_Idle;
    }

    if ((this->collider.base.acFlags & AC_HIT) && (this->actor.colChkInfo.damageEffect == 0xF)) {
        this->actionFunc = DmStk_ClockTower_DeflectHit;
    }
}

/**
 * Updates a variety of states based on Skull Kid's current cutscene, including his current
 * animation, his hand/mask type, his fade in/fade out state, and his current cutscene action.
 */
void DmStk_HandleCutscene(DmStk* this, PlayState* play) {
    s32 pad;
    s32 cueChannel;

    if (Cutscene_IsCueInChannel(play, CS_CMD_ACTOR_CUE_107)) {
        cueChannel = Cutscene_GetCueChannel(play, CS_CMD_ACTOR_CUE_107);

        if (play->csCtx.curFrame == play->csCtx.actorCues[cueChannel]->startFrame) {
            if (this->cueId != play->csCtx.actorCues[cueChannel]->id) {
                this->cueId = play->csCtx.actorCues[cueChannel]->id;
                if (play->sceneId == SCENE_CLOCKTOWER) {
                    this->handType = SK_HAND_TYPE_HOLDING_FLUTE;
                } else {
                    this->handType = SK_HAND_TYPE_DEFAULT;
                }

                switch (play->csCtx.actorCues[cueChannel]->id) {
                    case 0:
                    case 1:
                        this->animIndex = SK_ANIM_IDLE;
                        break;

                    case 2:
                        this->animIndex = SK_ANIM_WALK;
                        break;

                    case 3:
                        this->animIndex = SK_ANIM_UNUSED_KICK_OVER_LINK;
                        break;

                    case 4:
                        this->animIndex = SK_ANIM_KICK_OVER_LINK;
                        break;

                    case 6:
                        this->animIndex = SK_ANIM_PICK_UP_OCARINA;
                        this->handType = SK_HAND_TYPE_HOLDING_OCARINA;
                        break;

                    case 7:
                        this->animIndex = SK_ANIM_PICK_UP_OCARINA_AND_START_PLAYING;
                        this->handType = SK_HAND_TYPE_HOLDING_OCARINA;
                        break;

                    case 8:
                        this->animIndex = SK_ANIM_SHAKE_HEAD;
                        break;

                    case 9:
                        this->animIndex = SK_ANIM_IDLE;
                        this->fadeInState = SK_FADE_IN_STATE_START;
                        break;

                    case 12:
                        this->animIndex = SK_ANIM_HIDE_OCARINA_START;
                        this->handType = SK_HAND_TYPE_HOLDING_OCARINA;
                        break;

                    case 13:
                        this->animIndex = SK_ANIM_JUMP_WHILE_HIDING_OCARINA;
                        this->handType = SK_HAND_TYPE_HOLDING_OCARINA;
                        break;

                    case 14:
                        this->animIndex = SK_ANIM_HORSE_RIDE_START;
                        break;

                    case 15:
                        this->animIndex = SK_ANIM_HORSE_RIDE_LOOP;
                        break;

                    case 16:
                        this->animIndex = SK_ANIM_HORSE_RIDE_AND_ROTATE;
                        break;

                    case 17:
                        this->animIndex = SK_ANIM_RECLINING_FLOAT;
                        break;

                    case 18:
                        this->animIndex = SK_ANIM_CURSE_START;
                        break;

                    case 19:
                        this->animIndex = SK_ANIM_LAUGH_START;
                        break;

                    case 20:
                        this->animIndex = SK_ANIM_RAISE_MASK_START;
                        break;

                    case 21:
                        this->animIndex = SK_ANIM_LOWER_MASK;
                        break;

                    case 22:
                        this->animIndex = SK_ANIM_PLAY_OCARINA_WHILE_FLOATING;
                        if (gSaveContext.save.saveInfo.inventory.items[SLOT_OCARINA] == ITEM_NONE) {
                            this->handType = SK_HAND_TYPE_HOLDING_OCARINA;
                        }
                        break;

                    case 23:
                        this->animIndex = SK_ANIM_FLOATING_TURN_AROUND;
                        this->handType = SK_HAND_TYPE_JUGGLING_OR_DROPPING_OCARINA;
                        break;

                    case 24:
                        this->animIndex = SK_ANIM_CALL_DOWN_MOON_START;
                        if (gSaveContext.save.saveInfo.inventory.items[SLOT_OCARINA] == ITEM_NONE) {
                            this->handType = SK_HAND_TYPE_HOLDING_OCARINA;
                        }
                        break;

                    case 25:
                        Actor_Kill(&this->actor);
                        break;

                    case 26:
                        this->animIndex = SK_ANIM_SMACK_FAIRY_START;
                        if (gSaveContext.save.saveInfo.inventory.items[SLOT_OCARINA] == ITEM_NONE) {
                            this->handType = SK_HAND_TYPE_HOLDING_OCARINA;
                        }
                        break;

                    case 27:
                        this->animIndex = SK_ANIM_HIT_BY_BUBBLE;
                        if (gSaveContext.save.saveInfo.inventory.items[SLOT_OCARINA] == ITEM_NONE) {
                            this->handType = SK_HAND_TYPE_HOLDING_OCARINA;
                        }
                        break;

                    case 28:
                        this->animIndex = SK_ANIM_DROP_OCARINA;
                        this->handType = SK_HAND_TYPE_JUGGLING_OR_DROPPING_OCARINA;
                        break;

                    case 30:
                        this->animIndex = SK_ANIM_FLOATING_ARMS_CROSSED;
                        break;

                    case 31:
                        this->animIndex = SK_ANIM_DEFLECT_ATTACK;
                        break;

                    case 32:
                        this->animIndex = SK_ANIM_SURPRISE_START;
                        break;

                    case 33:
                        this->animIndex = SK_ANIM_LOOK_AROUND_FOR_GIANTS_START;
                        break;

                    case 34:
                        this->animIndex = SK_ANIM_HOLD_HEAD_AND_SHAKE_START;
                        break;

                    case 35:
                        this->animIndex = SK_ANIM_HOLD_HEAD_AND_SCREAM_START;
                        break;

                    case 36:
                        this->animIndex = SK_ANIM_HUDDLE_WITH_FAIRIES;
                        break;

                    case 37:
                        this->animIndex = SK_ANIM_SEARCH_MASK_SALESMAN;
                        break;

                    case 38:
                        this->animIndex = SK_ANIM_HOLD_UP_MASK_START;
                        break;

                    case 39:
                        this->animIndex = SK_ANIM_SHIVER;
                        break;

                    case 40:
                        this->animIndex = SK_ANIM_DRAW;
                        break;

                    case 41:
                        this->animIndex = SK_ANIM_TELESCOPE_LOOK_UP_START;
                        break;

                    case 42:
                        this->animIndex = SK_ANIM_BENT_OVER_HEAD_TWITCH;
                        break;

                    case 43:
                        this->animIndex = SK_ANIM_BENT_OVER_LOOK_UP;
                        break;

                    case 44:
                        this->animIndex = SK_ANIM_SPANK;
                        break;

                    case 45:
                        this->animIndex = SK_ANIM_HIP_SHAKE_AND_JUMP;
                        this->fadeOutState = SK_FADE_OUT_STATE_FADING_OUT;
                        break;

                    case 46:
                        this->handType = SK_HAND_TYPE_HOLDING_FLUTE;
                        this->animIndex = SK_ANIM_PLAY_FLUTE;
                        break;

                    case 47:
                        this->animIndex = SK_ANIM_CARTWHEEL;
                        break;

                    case 48:
                        this->animIndex = SK_ANIM_CARTWHEEL;
                        break;

                    case 49:
                        this->animIndex = SK_ANIM_LOOK_UP_AT_GIANTS;
                        break;

                    case 50:
                        this->animIndex = SK_ANIM_ASHAMED_START;
                        break;

                    case 51:
                        this->animIndex = SK_ANIM_LOOK_LEFT_START;
                        break;

                    case 52:
                        this->animIndex = SK_ANIM_SNIFF;
                        break;

                    case 53:
                        this->animIndex = SK_ANIM_CARTWHEEL;
                        break;

                    case 54:
                        this->animIndex = SK_ANIM_LIE_FLAT;
                        break;

                    case 55:
                        this->animIndex = SK_ANIM_DANGLE_FROM_MASK_START;
                        break;

                    case 56:
                        this->animIndex = SK_ANIM_DROPPED_FROM_MASK;
                        break;

                    case 57:
                        this->animIndex = SK_ANIM_LOOK_UP_AT_GIANTS;
                        break;

                    case 58:
                        this->animIndex = SK_ANIM_ASHAMED_START;
                        break;

                    case 59:
                        this->animIndex = SK_ANIM_LOOK_LEFT_START;
                        break;

                    case 60:
                        this->animIndex = SK_ANIM_SNIFF;
                        break;

                    case 5:
                    case 10:
                        break;

                    default:
                        this->animIndex = SK_ANIM_SHAKE_HEAD;
                        break;
                }

                DmStk_ChangeAnim(this, play, &this->skelAnime, &sAnimationInfo[this->animIndex], 0);
            }
        }

        Cutscene_ActorTranslateAndYaw(&this->actor, play, cueChannel);
    } else {
        this->cueId = 99;
    }

    if (this->fadeInState == SK_FADE_IN_STATE_START) {
        Math_SmoothStepToF(&this->fogScale, 0.7f, 0.1f, 0.007f, 0.005f);
        if (this->fogScale < 0.71f) {
            this->fogScale = 0.7f;
            this->fogN = 800;
            this->fadeInState++;
        }

        this->fogR = play->lightCtx.fogColor[0] * this->fogScale;
        this->fogG = play->lightCtx.fogColor[1] * this->fogScale;
        this->fogB = play->lightCtx.fogColor[2] * this->fogScale;
    } else if (this->fadeInState == SK_FADE_IN_STATE_INCREASE_FOG) {
        if (this->fogN < 996) {
            this->fogN += 10;
        }

        if (this->fogN > 996) {
            this->fadeInState++;
            this->fogN = 996;
        }
    } else if (this->fadeInState == SK_FADE_IN_STATE_INCREASE_ALPHA) {
        if (this->alpha < 128) {
            this->alpha += 3;
        }

        if (this->alpha < 255) {
            this->alpha += 20;
        } else {
            this->alpha = 255;
            this->fadeInState = SK_FADE_IN_STATE_NONE;
        }
    }

    if (this->fadeOutState == SK_FADE_OUT_STATE_FADING_OUT) {
        if (this->fadeOutTimer > 40) {
            this->maskType = SK_MASK_TYPE_FADING_OUT;
        }

        this->fadeOutTimer++;
        if (this->fadeOutTimer >= 44) {
            this->alpha -= 35;
            if (this->alpha < 0) {
                this->alpha = 0;
                this->fadeOutState = SK_FADE_OUT_STATE_NONE;
                SET_WEEKEVENTREG(WEEKEVENTREG_12_04);
                if (!(play->actorCtx.flags & ACTORCTX_FLAG_TELESCOPE_ON)) {
                    Actor_Kill(&this->actor);
                } else {
                    this->shouldDraw = false;
                }
            }
        }
    }

    if (Animation_OnFrame(&this->skelAnime, this->skelAnime.endFrame)) {
        switch (this->animIndex) {
            case SK_ANIM_LOOK_AROUND:
            case SK_ANIM_CURSE_START:
            case SK_ANIM_LAUGH_START:
            case SK_ANIM_HIDE_OCARINA_START:
            case SK_ANIM_PICK_UP_OCARINA_AND_START_PLAYING:
            case SK_ANIM_PICK_UP_OCARINA:
            case SK_ANIM_KICK_OVER_LINK:
            case SK_ANIM_UNUSED_KICK_OVER_LINK:
            case SK_ANIM_HORSE_RIDE_START:
            case SK_ANIM_RAISE_MASK_START:
            case SK_ANIM_FLOATING_TURN_AROUND:
            case SK_ANIM_CALL_DOWN_MOON_START:
            case SK_ANIM_SMACK_FAIRY_START:
            case SK_ANIM_TELESCOPE_LOOK_UP_START:
            case SK_ANIM_SURPRISE_START:
            case SK_ANIM_LOOK_AROUND_FOR_GIANTS_START:
            case SK_ANIM_HOLD_HEAD_AND_SHAKE_START:
            case SK_ANIM_HOLD_HEAD_AND_SCREAM_START:
            case SK_ANIM_HOLD_UP_MASK_START:
            case SK_ANIM_DANGLE_FROM_MASK_START:
            case SK_ANIM_ASHAMED_START:
            case SK_ANIM_LOOK_LEFT_START:
            case SK_ANIM_SNIFF:
                this->animIndex++;
                DmStk_ChangeAnim(this, play, &this->skelAnime, &sAnimationInfo[this->animIndex], 0);
                break;

            case SK_ANIM_LOWER_MASK:
                this->animIndex = SK_ANIM_IDLE;
                this->maskType = SK_MASK_TYPE_NORMAL;
                this->handType = SK_HAND_TYPE_DEFAULT;
                DmStk_ChangeAnim(this, play, &this->skelAnime, &sAnimationInfo[this->animIndex], 0);
                break;

            default:
                break;
        }
    }

    if (((this->animIndex == SK_ANIM_RAISE_MASK_START) && (this->skelAnime.curFrame >= 16.0f)) ||
        (this->animIndex == SK_ANIM_RAISE_MASK_LOOP) || (this->animIndex == SK_ANIM_LOWER_MASK)) {
        this->maskType = SK_MASK_TYPE_RAISED;
        this->handType = SK_HAND_TYPE_HOLDING_MAJORAS_MASK;
    } else if (((this->animIndex >= SK_ANIM_HUDDLE_WITH_FAIRIES) && (this->animIndex <= SK_ANIM_DRAW)) ||
               ((this->animIndex >= SK_ANIM_PLAY_FLUTE) && (this->animIndex <= SK_ANIM_CARTWHEEL)) ||
               ((play->sceneId == SCENE_00KEIKOKU) && (gSaveContext.sceneLayer == 7))) {
        this->maskType = SK_MASK_TYPE_NO_MASK;
        if ((this->animIndex == SK_ANIM_HOLD_UP_MASK_START) || (this->animIndex == SK_ANIM_HOLD_UP_MASK_LOOP)) {
            this->handType = SK_HAND_TYPE_HOLDING_MAJORAS_MASK_AND_FLUTE;
        }
    }

    if (this->animIndex == SK_ANIM_DROPPED_FROM_MASK) {
        this->maskType = SK_MASK_TYPE_NO_MASK;
    }
}

void DmStk_UpdateCollision(DmStk* this, PlayState* play) {
    s32 pad;

    Collider_UpdateCylinder(&this->actor, &this->collider);
    CollisionCheck_SetAC(play, &play->colChkCtx, &this->collider.base);
    CollisionCheck_SetOC(play, &play->colChkCtx, &this->collider.base);
}

/**
 * Handles Skull Kid when he is at the top of the Clock Tower with the Ocarina of Time.
 * If he is hit in this state, he will drop the Ocarina.
 *
 * If the player waits a while while Skull Kid is in this state, they will see a message
 * from Tatl telling them to hurry up and do something.
 */
void DmStk_ClockTower_IdleWithOcarina(DmStk* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    if (play->csCtx.state == CS_STATE_IDLE) {
        DmStk_ClockTower_AdjustHeightAndRotation(this, play);
        this->actor.flags |= ACTOR_FLAG_ATTENTION_ENABLED;
        this->tatlMessageTimer++;
        if (this->tatlMessageTimer > 800) {
            this->tatlMessageTimer = 0;
            if (!(player->stateFlags2 & PLAYER_STATE2_USING_OCARINA)) {
                // Why are you just standing around?
                Message_StartTextbox(play, 0x2014, &this->actor);
            }
        }

        if ((this->collider.base.acFlags & AC_HIT) && (this->actor.colChkInfo.damageEffect == 0xF)) {
            this->hasBeenHit = true;
            this->actionFunc = DmStk_ClockTower_StartDropOcarinaCutscene;
        }
    }
}

/**
 * Handles Skull Kid when he is at the top of the Clock Tower after the Ocarina of Time
 * has been returned to the player.
 * If he is hit in this state, he will just deflect the attack.
 */
void DmStk_ClockTower_Idle(DmStk* this, PlayState* play) {
    if (play->csCtx.state == CS_STATE_IDLE) {
        DmStk_ClockTower_AdjustHeightAndRotation(this, play);
        this->actor.flags |= ACTOR_FLAG_ATTENTION_ENABLED;

        if (this->animIndex == SK_ANIM_CALL_DOWN_MOON_LOOP) {
            this->actor.lockOnArrowOffset = 3100.0f;
        } else {
            this->actor.lockOnArrowOffset = 200.0f;
        }

        if ((this->collider.base.acFlags & AC_HIT) && (this->actor.colChkInfo.damageEffect == 0xF)) {
            this->hasBeenHit = true;
            this->actionFunc = DmStk_ClockTower_DeflectHit;
        }
    }
}

void DmStk_Update(Actor* thisx, PlayState* play) {
    DmStk* this = (DmStk*)thisx;

    if (DM_STK_GET_TYPE(&this->actor) != DM_STK_TYPE_MAJORAS_MASK) {
        if (this->animIndex == SK_ANIM_CALL_DOWN_MOON_LOOP) {
            Actor_SetFocus(&this->actor, 40.0f);
        } else {
            Actor_SetFocus(&this->actor, 6.0f);
        }

        DmStk_LoadObjectForAnimation(this, play);

        if (this->animIndex != SK_ANIM_LIE_FLAT) {
            SkelAnime_Update(&this->skelAnime);
        }

        this->alpha = this->alpha; // Set to itself

        this->actionFunc(this, play);

        if (play->sceneId == SCENE_OKUJOU) {
            DmStk_UpdateCollision(this, play);
        }

        DmStk_HandleCutscene(this, play);
        DmStk_PlaySfxForCutscenes(this, play);

        // This handles the cutscene where the player takes out the Deku Pipes for the first time.
        switch (this->dekuPipesCutsceneState) {
            case SK_DEKU_PIPES_CS_STATE_READY:
                if (Actor_OcarinaInteractionAccepted(&this->actor, &play->state)) {
                    this->dekuPipesCutsceneState = SK_DEKU_PIPES_CS_STATE_PLAYER_USED_OCARINA;
                } else {
                    Actor_OfferOcarinaInteraction(&this->actor, play, this->actor.xzDistToPlayer,
                                                  fabsf(this->actor.playerHeightRel));
                }
                break;

            case SK_DEKU_PIPES_CS_STATE_PLAYER_USED_OCARINA:
                if (CutsceneManager_IsNext(16)) {
                    this->dekuPipesCutsceneState = SK_DEKU_PIPES_CS_STATE_START;
                    CutsceneManager_Start(16, &this->actor);
                    this->actionFunc = DmStk_ClockTower_Idle;
                } else {
                    CutsceneManager_Queue(16);
                }
                break;

            case SK_DEKU_PIPES_CS_STATE_START:
                if (play->csCtx.state == CS_STATE_IDLE) {
                    this->dekuPipesCutsceneState = SK_DEKU_PIPES_CS_STATE_END;
                }
                break;

            default:
                break;
        }

        // This code is responsible for making in-game time pass while using the telescope in the Astral Observatory.
        // Skull Kid is always loaded in the scene, even if he isn't visible, hence why time always passes.
        if ((play->actorCtx.flags & ACTORCTX_FLAG_TELESCOPE_ON) && (play->msgCtx.msgMode != MSGMODE_NONE) &&
            (play->msgCtx.currentTextId == 0x5E6) && !FrameAdvance_IsEnabled(play) &&
            (play->transitionTrigger == TRANS_TRIGGER_OFF) && (CutsceneManager_GetCurrentCsId() == CS_ID_NONE) &&
            (play->csCtx.state == CS_STATE_IDLE)) {
            gSaveContext.save.time = CURRENT_TIME + (u16)R_TIME_SPEED;
            if (R_TIME_SPEED != 0) {
                gSaveContext.save.time = CURRENT_TIME + (u16)((void)0, gSaveContext.save.timeSpeedOffset);
            }
        }
    }

    if ((play->sceneId == SCENE_00KEIKOKU) && (gSaveContext.sceneLayer == 3) && (play->csCtx.scriptIndex > 0)) {
        play->envCtx.skyboxConfig = SKYBOX_CONFIG_15;
        play->envCtx.changeSkyboxNextConfig = SKYBOX_CONFIG_15;
    }
}

s32 DmStk_OverrideLimbDrawOpa(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, Actor* thisx) {
    DmStk* this = (DmStk*)thisx;

    if (limbIndex == SKULL_KID_LIMB_RIGHT_HAND) {
        if ((this->handType == SK_HAND_TYPE_HOLDING_LINK_MASK_AND_FLUTE) ||
            (this->handType == SK_HAND_TYPE_HOLDING_LINK_MASK) || (this->handType == SK_HAND_TYPE_HOLDING_OCARINA)) {
            *dList = NULL;
        }
    } else if (limbIndex == SKULL_KID_LIMB_LEFT_HAND) {
        switch (this->handType) {
            case SK_HAND_TYPE_HOLDING_LINK_MASK:
            case SK_HAND_TYPE_HOLDING_MAJORAS_MASK:
            case SK_HAND_TYPE_HOLDING_OCARINA:
            case SK_HAND_TYPE_JUGGLING_OR_DROPPING_OCARINA:
            case SK_HAND_TYPE_HOLDING_FLUTE:
                *dList = NULL;
                break;

            case SK_HAND_TYPE_DEFAULT:
                if (this->alpha == 255) {
                    *dList = NULL;
                }
                break;

            default:
                break;
        }
    } else if (limbIndex == SKULL_KID_LIMB_HEAD) {
        *dList = NULL;
    }

    return false;
}

void DmStk_PostLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot, Actor* thisx, Gfx** gfx) {
    s32 pad;
    s32 pad2;
    DmStk* this = (DmStk*)thisx;

    if (limbIndex == SKULL_KID_LIMB_HEAD) {
        Matrix_MultZero(&this->headPos);

        OPEN_DISPS(play->state.gfxCtx);

        MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx);

        if ((this->animIndex == SK_ANIM_LOOK_LEFT_LOOP) || (this->animIndex == SK_ANIM_LAUGH_LOOP) ||
            (this->animIndex == SK_ANIM_LAUGH_AFTER_SNIFF)) {
            gSPDisplayList(POLY_OPA_DISP++, gSkullKidLaughingHeadDL);
            gSPDisplayList(POLY_OPA_DISP++, gSkullKidLaughingEyesDL);
        } else {
            gSPDisplayList(POLY_OPA_DISP++, gSkullKidNormalHeadDL);
            gSPDisplayList(POLY_OPA_DISP++, gSkullKidNormalEyesDL);
        }

        switch (this->maskType) {
            case SK_MASK_TYPE_NO_MASK:
                break;

            case SK_MASK_TYPE_NORMAL:
                if ((play->sceneId == SCENE_LOST_WOODS) && (gSaveContext.sceneLayer == 1) &&
                    (play->csCtx.curFrame < 1400)) {
                    if (this->fogN == this->fogF) {
                        this->fogF = this->fogN;
                    }
                    POLY_OPA_DISP = Gfx_SetFog(POLY_OPA_DISP, this->fogR, this->fogG, this->fogB, this->fogA,
                                               this->fogN, this->fogF);
                    gSPDisplayList(POLY_OPA_DISP++, gSkullKidMajorasMask1DL);
                    POLY_OPA_DISP = Play_SetFog(play, POLY_OPA_DISP);
                } else {
                    gSPDisplayList(POLY_OPA_DISP++, gSkullKidMajorasMask1DL);
                }
                break;

            case SK_MASK_TYPE_GLOWING_EYES:
                gSPDisplayList(POLY_OPA_DISP++, gSkullKidMajorasMask1DL);
                gSPDisplayList(POLY_OPA_DISP++, gSkullKidMajorasMaskEyesDL);

                if (Cutscene_IsCueInChannel(play, CS_CMD_ACTOR_CUE_513) &&
                    (play->csCtx.actorCues[Cutscene_GetCueChannel(play, CS_CMD_ACTOR_CUE_513)]->id == 2) &&
                    (this->objectStk2ObjectSlot > OBJECT_SLOT_NONE)) {
                    Matrix_Push();
                    Matrix_Scale(2.0f, 2.0f, 2.0f, MTXMODE_APPLY);
                    gSegments[6] = OS_K0_TO_PHYSICAL(play->objectCtx.slots[this->objectStk2ObjectSlot].segment);

                    gSPSegment(POLY_OPA_DISP++, 0x06, play->objectCtx.slots[this->objectStk2ObjectSlot].segment);

                    AnimatedMat_Draw(play, Lib_SegmentedToVirtual(gSkullKidMajorasMaskCurseOverlayTexAnim));
                    Gfx_DrawDListOpa(play, gSkullKidMajorasMaskCurseOverlayDL);
                    gSegments[6] = OS_K0_TO_PHYSICAL(play->objectCtx.slots[this->objectStkObjectSlot].segment);

                    gSPSegment(POLY_OPA_DISP++, 0x06, play->objectCtx.slots[this->objectStkObjectSlot].segment);

                    Matrix_Pop();
                }
                break;

            default:
                break;
        }

        CLOSE_DISPS(play->state.gfxCtx);

    } else if (limbIndex == SKULL_KID_LIMB_RIGHT_HAND) {

        OPEN_DISPS(play->state.gfxCtx);

        if (this->handType != SK_HAND_TYPE_HOLDING_MAJORAS_MASK_AND_FLUTE) {
            MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx);
        }

        switch (this->handType) {
            case SK_HAND_TYPE_HOLDING_LINK_MASK_AND_FLUTE:
                gSPDisplayList(POLY_OPA_DISP++, gSkullKidUntexturedRightHandDL);
                gSPDisplayList(POLY_OPA_DISP++, gSkullKidLinkMask2DL);
                break;

            case SK_HAND_TYPE_HOLDING_LINK_MASK:
                gSPDisplayList(POLY_OPA_DISP++, gSkullKidMaskHoldingRightHandDL);
                gSPDisplayList(POLY_OPA_DISP++, gSkullKidLinkMask3DL);
                break;

            case SK_HAND_TYPE_HOLDING_OCARINA:
                gSPDisplayList(POLY_OPA_DISP++, gSkullKidOcarinaHoldingRightHandDL);

                if ((play->sceneId == SCENE_LOST_WOODS) && (gSaveContext.sceneLayer == 1)) {
                    gSPDisplayList(POLY_OPA_DISP++, gSkullKidOcarinaOfTimeDL);
                }
                break;

            case SK_HAND_TYPE_HOLDING_MAJORAS_MASK_AND_FLUTE:
                Matrix_Translate(-20.0f, -660.0f, 860.0f, MTXMODE_APPLY);
                Matrix_RotateYS(0x6142, MTXMODE_APPLY);
                Matrix_RotateXS(-0x1988, MTXMODE_APPLY);

                MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx);
                gSPDisplayList(POLY_OPA_DISP++, gSkullKidMajorasMask1DL);
                break;

            default:
                break;
        }

        CLOSE_DISPS(play->state.gfxCtx);

    } else if (limbIndex == SKULL_KID_LIMB_LEFT_HAND) {

        OPEN_DISPS(play->state.gfxCtx);

        MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx);

        switch (this->handType) {
            case SK_HAND_TYPE_HOLDING_LINK_MASK_AND_FLUTE:
                break;

            case SK_HAND_TYPE_HOLDING_LINK_MASK:
                gSPDisplayList(POLY_OPA_DISP++, gSkullKidUntexturedLeftHandDL);
                break;

            case SK_HAND_TYPE_HOLDING_MAJORAS_MASK:
                gSPDisplayList(POLY_OPA_DISP++, gSkullKidTwoFingersExtendedLeftHandDL);
                gSPDisplayList(POLY_OPA_DISP++, gSkullKidMajorasMask2DL);
                break;

            case SK_HAND_TYPE_HOLDING_OCARINA:
                gSPDisplayList(POLY_OPA_DISP++, gSkullKidTwoFingersExtendedLeftHandDL);
                if (GameInteractor_Should(VB_DRAW_OCARINA_IN_STK_HAND,
                                          (play->sceneId != SCENE_LOST_WOODS) || (gSaveContext.sceneLayer != 1),
                                          this)) {
                    gSPDisplayList(POLY_OPA_DISP++, gSkullKidOcarinaOfTimeDL);
                }
                break;

            case SK_HAND_TYPE_JUGGLING_OR_DROPPING_OCARINA:
                gSPDisplayList(POLY_OPA_DISP++, gSkullKidTwoFingersExtendedLeftHandDL);
                break;

            case SK_HAND_TYPE_HOLDING_FLUTE:
                gSPDisplayList(POLY_OPA_DISP++, gSkullKidFluteHoldingLeftHandDL);
                gSPDisplayList(POLY_OPA_DISP++, gSkullKidFluteDL);
                break;

            case SK_HAND_TYPE_DEFAULT:
                if (this->alpha == 255) {
                    gSPDisplayList(POLY_OPA_DISP++, gSkullKidOpenLeftHandDL);
                }
                break;

            default:
                break;
        }

        CLOSE_DISPS(play->state.gfxCtx);
    }
}

void DmStk_PostLimbDrawOpa(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot, Actor* thisx) {
    DmStk* this = (DmStk*)thisx;

    DmStk_PostLimbDraw(play, limbIndex, dList, rot, &this->actor, NULL);
}

void DmStk_Draw(Actor* thisx, PlayState* play) {
    DmStk* this = (DmStk*)thisx;

    if (this->shouldDraw) {
        if (DM_STK_GET_TYPE(&this->actor) == DM_STK_TYPE_MAJORAS_MASK) {
            Gfx_DrawDListOpa(play, gSkullKidMajorasMask1DL);
            return;
        }

        gSegments[6] = OS_K0_TO_PHYSICAL(play->objectCtx.slots[this->objectStkObjectSlot].segment);

        OPEN_DISPS(play->state.gfxCtx);

        this->alpha = this->alpha; // Set to itself
        Gfx_SetupDL25_Opa(play->state.gfxCtx);

        if (this->alpha < 255) {
            Gfx_SetupDL25_Xlu(play->state.gfxCtx);
            Scene_SetRenderModeXlu(play, 1, 2);
            gDPPipeSync(POLY_XLU_DISP++);
            gDPSetEnvColor(POLY_XLU_DISP++, 0, 0, 0, this->alpha);

            POLY_XLU_DISP =
                SkelAnime_DrawFlex(play, this->skelAnime.skeleton, this->skelAnime.jointTable,
                                   this->skelAnime.dListCount, NULL, DmStk_PostLimbDraw, &this->actor, POLY_XLU_DISP);
        } else {
            Scene_SetRenderModeXlu(play, 0, 1);

            gDPPipeSync(POLY_OPA_DISP++);
            gDPSetEnvColor(POLY_OPA_DISP++, 255, 255, 255, 255);

            SkelAnime_DrawFlexOpa(play, this->skelAnime.skeleton, this->skelAnime.jointTable,
                                  this->skelAnime.dListCount, DmStk_OverrideLimbDrawOpa, DmStk_PostLimbDrawOpa,
                                  &this->actor);
        }

        CLOSE_DISPS(play->state.gfxCtx);
    }
}
