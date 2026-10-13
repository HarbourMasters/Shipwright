/*
 * File: z_en_jso2.c
 * Overlay: ovl_En_Jso2
 * Description: Garo Master
 */

#include "z_en_jso2.h"
#include "assets/objects/gameplay_keep/gameplay_keep.h"
#include "overlays/actors/ovl_En_Clear_Tag/z_en_clear_tag.h"
#include "overlays/actors/ovl_En_Col_Man/z_en_col_man.h"

#define FLAGS                                                                                 \
    (ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_HOSTILE | ACTOR_FLAG_UPDATE_CULLING_DISABLED | \
     ACTOR_FLAG_DRAW_CULLING_DISABLED | ACTOR_FLAG_FREEZE_EXCEPTION | ACTOR_FLAG_MINIMAP_ICON_ENABLED)

void EnJso2_Init(Actor* thisx, PlayState* play);
void EnJso2_Destroy(Actor* thisx, PlayState* play);
void EnJso2_Update(Actor* thisx, PlayState* play);
void EnJso2_Draw(Actor* thisx, PlayState* play2);

void EnJso2_SetupIntroCutscene(EnJso2* this);
void EnJso2_IntroCutscene(EnJso2* this, PlayState* play);
void EnJso2_SetupAppear(EnJso2* this);
void EnJso2_Appear(EnJso2* this, PlayState* play);
void EnJso2_SetupCirclePlayer(EnJso2* this, PlayState* play);
void EnJso2_CirclePlayer(EnJso2* this, PlayState* play);
void EnJso2_Guard(EnJso2* this, PlayState* play);
void EnJso2_SetupSpinBeforeAttack(EnJso2* this);
void EnJso2_SpinBeforeAttack(EnJso2* this, PlayState* play);
void EnJso2_SetupDashAttack(EnJso2* this);
void EnJso2_DashAttack(EnJso2* this, PlayState* play);
void EnJso2_SetupTeleport(EnJso2* this);
void EnJso2_Teleport(EnJso2* this, PlayState* play);
void EnJso2_SetupFallFromTeleport(EnJso2* this, PlayState* play);
void EnJso2_FallFromTeleport(EnJso2* this, PlayState* play);
void EnJso2_SetupSlash(EnJso2* this, PlayState* play);
void EnJso2_Slash(EnJso2* this, PlayState* play);
void EnJso2_SpinAttack(EnJso2* this, PlayState* play);
void EnJso2_SetupWaitAfterSlash(EnJso2* this);
void EnJso2_WaitAfterSlash(EnJso2* this, PlayState* play);
void EnJso2_Stunned(EnJso2* this, PlayState* play);
void EnJso2_Damaged(EnJso2* this, PlayState* play);
void EnJso2_SetupJumpBack(EnJso2* this);
void EnJso2_JumpBack(EnJso2* this, PlayState* play);
void EnJso2_Dead(EnJso2* this, PlayState* play);
void EnJso2_SetupDeathCutscene(EnJso2* this);
void EnJso2_DeathCutscene(EnJso2* this, PlayState* play);
void EnJso2_SetupBlowUp(EnJso2* this);
void EnJso2_BlowUp(EnJso2* this, PlayState* play);
void EnJso2_FadeAway(EnJso2* this, PlayState* play);

typedef enum EnJso2Action {
    /*  0 */ EN_JSO2_ACTION_INTRO_CUTSCENE,
    /*  1 */ EN_JSO2_ACTION_UNK_1, // Checked in EnJso2_Update, but never actually used
    /*  2 */ EN_JSO2_ACTION_UNK_2, // Checked in EnJso2_UpdateDamage, but never actually used
    /*  3 */ EN_JSO2_ACTION_CIRCLE_PLAYER,
    /*  4 */ EN_JSO2_ACTION_GUARD,
    /*  5 */ EN_JSO2_ACTION_SPIN_BEFORE_ATTACK,
    /*  6 */ EN_JSO2_ACTION_DASH_ATTACK,
    /*  7 */ EN_JSO2_ACTION_SLASH,
    /*  8 */ EN_JSO2_ACTION_SPIN_ATTACK,
    /*  9 */ EN_JSO2_ACTION_WAIT_AFTER_SLASH,
    /* 10 */ EN_JSO2_ACTION_STUNNED,
    /* 11 */ EN_JSO2_ACTION_DAMAGED,
    /* 12 */ EN_JSO2_ACTION_JUMP_BACK,
    /* 13 */ EN_JSO2_ACTION_DEAD,
    /* 14 */ EN_JSO2_ACTION_BLOW_UP,
    /* 15 */ EN_JSO2_ACTION_TELEPORT,
    /* 16 */ EN_JSO2_ACTION_FALL_FROM_TELEPORT
} EnJso2Action;

typedef enum EnJso2IntroCsState {
    // Waits for the player to walk toward the Garo Master, then starts the cutscene and transitions to the next state.
    /*  0 */ EN_JSO2_INTRO_CS_STATE_WAITING_FOR_PLAYER_TO_APPROACH,

    // Positions the Garo Master, player, and sub camera variables appropriately, then transitions to the next state.
    /*  1 */ EN_JSO2_INTRO_CS_STATE_STARTED,

    // Automatically moves the player forward a little bit to get closer to the center of the room.
    /*  2 */ EN_JSO2_INTRO_CS_STATE_MOVE_PLAYER_FORWARD,

    // Waits 9 frames, then spawns some rocks to fall from the ceiling.
    /*  3 */ EN_JSO2_INTRO_CS_STATE_SPAWN_FALLING_ROCKS,

    // Waits 25 frames for the rocks that were spawned in the previous state to fall down and hit the ground.
    /*  4 */ EN_JSO2_INTRO_CS_STATE_WAIT_FOR_ROCKS_TO_FALL,

    // Makes the player perform their "surprised" animation, and waits 20 frames.
    /*  5 */ EN_JSO2_INTRO_CS_STATE_PLAYER_SURPRISED,

    // Slowly pans the camera upwards to look at the Garo Master standing above the hole in the ceiling.
    /*  6 */ EN_JSO2_INTRO_CS_STATE_PAN_CAMERA_UPWARDS,

    // Waits for 5 frames, doing nothing, before the Garo Master laughs.
    /*  7 */ EN_JSO2_INTRO_CS_STATE_WAIT_BEFORE_LAUGH,

    // Waits for 45 frames to let the Garo Master's laugh play out.
    /*  8 */ EN_JSO2_INTRO_CS_STATE_LAUGH,

    // Makes the Garo Master jump down from the hole in the ceiling, drawing afterimages as it falls, until it lands.
    /*  9 */ EN_JSO2_INTRO_CS_STATE_JUMP_DOWN,

    // Spawns a ring of dust, draws afterimages, and plays the landing animation to completion.
    /* 10 */ EN_JSO2_INTRO_CS_STATE_LAND,

    // Makes the Garo Master draw its swords, and gradually scales up the flames on the swords. This state also ends the
    // cutscene once the animation for drawing the swords completes.
    /* 11 */ EN_JSO2_INTRO_CS_STATE_DRAW_SWORDS
} EnJso2IntroCsState;

typedef enum EnJso2AppearState {
    // Spawns a dust ring, disables the current BGM, plays the "appearance" SFX, and transitions to the next state.
    /* 0 */ EN_JSO2_APPEAR_STATE_STARTED,

    // This state handles all other aspects of the Garo Master's appearance (starting the mini-boss BGM, scaling up the
    // flames on the swords, etc.)
    /* 1 */ EN_JSO2_APPEAR_STATE_APPEARING
} EnJso2AppearState;

typedef enum EnJso2DeathCsState {
    // Starts the cutscene and immediately transitions to the next state.
    /* 0 */ EN_JSO2_DEATH_CS_STATE_STARTED,

    // Plays the animation of the Garo Master collapsing down onto one knee.
    /* 1 */ EN_JSO2_DEATH_CS_STATE_COLLAPSE,

    // Plays one loop of the animation where the Garo Master is trembling.
    /* 2 */ EN_JSO2_DEATH_CS_STATE_TREMBLE,

    // Tells the player a hint about using the Light Arrow. The Garo Master also takes its bomb out in this state.
    /* 3 */ EN_JSO2_DEATH_CS_STATE_TELL_HINT,

    // Detonates the Garo Master's bomb and slowly reduces its alpha until it completely fades away.
    /* 4 */ EN_JSO2_DEATH_CS_STATE_BLOW_UP_AND_FADE_AWAY,

    // Waits for 50 frames to let the Garo Master's laugh play out, then ends the cutscene.
    /* 5 */ EN_JSO2_DEATH_CS_STATE_WAIT_AND_END
} EnJso2DeathCsState;

typedef enum EnJso2SwordState {
    /* 0 */ EN_JSO2_SWORD_STATE_BOTH_DRAWN,

    // If the Garo Master is about to blow itself up, it will use this sword state, but EnJso2_OverrideLimbDraw will
    // draw the left sword anyway. When the Garo Master is defeated, the animations for its "weakened" state has it
    // support itself with its left sword, so it wouldn't look right if the sword wasn't drawn.
    /* 2 */ EN_JSO2_SWORD_STATE_NONE_DRAWN = 2
} EnJso2SwordState;

typedef enum EnJso2DamageEffect {
    /* 0x0 */ EN_JSO2_DMGEFF_IMMUNE,        // Deals no damage and has no special effect
    /* 0x1 */ EN_JSO2_DMGEFF_STUN,          // Deals no damage but stuns the Garo Master
    /* 0x2 */ EN_JSO2_DMGEFF_FIRE,          // Damages and sets the Garo Master on fire
    /* 0x3 */ EN_JSO2_DMGEFF_FREEZE,        // Damages and surrounds the Garo Master with ice
    /* 0x4 */ EN_JSO2_DMGEFF_LIGHT_ORB,     // Damages and surrounds the Garo Master with light orbs
    /* 0x5 */ EN_JSO2_DMGEFF_ELECTRIC_STUN, // Behaves like EN_JSO2_DMGEFF_STUN, but also applies an electric effect
    /* 0xF */ EN_JSO2_DMGEFF_NONE = 0xF     // Damages the Garo Master and has no special effect
} EnJso2DamageEffect;

static DamageTable sDamageTable = {
    /* Deku Nut       */ DMG_ENTRY(0, EN_JSO2_DMGEFF_STUN),
    /* Deku Stick     */ DMG_ENTRY(1, EN_JSO2_DMGEFF_NONE),
    /* Horse trample  */ DMG_ENTRY(0, EN_JSO2_DMGEFF_IMMUNE),
    /* Explosives     */ DMG_ENTRY(1, EN_JSO2_DMGEFF_NONE),
    /* Zora boomerang */ DMG_ENTRY(1, EN_JSO2_DMGEFF_NONE),
    /* Normal arrow   */ DMG_ENTRY(1, EN_JSO2_DMGEFF_NONE),
    /* UNK_DMG_0x06   */ DMG_ENTRY(0, EN_JSO2_DMGEFF_IMMUNE),
    /* Hookshot       */ DMG_ENTRY(0, EN_JSO2_DMGEFF_STUN),
    /* Goron punch    */ DMG_ENTRY(1, EN_JSO2_DMGEFF_NONE),
    /* Sword          */ DMG_ENTRY(1, EN_JSO2_DMGEFF_NONE),
    /* Goron pound    */ DMG_ENTRY(1, EN_JSO2_DMGEFF_NONE),
    /* Fire arrow     */ DMG_ENTRY(2, EN_JSO2_DMGEFF_FIRE),
    /* Ice arrow      */ DMG_ENTRY(2, EN_JSO2_DMGEFF_FREEZE),
    /* Light arrow    */ DMG_ENTRY(2, EN_JSO2_DMGEFF_LIGHT_ORB),
    /* Goron spikes   */ DMG_ENTRY(1, EN_JSO2_DMGEFF_NONE),
    /* Deku spin      */ DMG_ENTRY(0, EN_JSO2_DMGEFF_STUN),
    /* Deku bubble    */ DMG_ENTRY(1, EN_JSO2_DMGEFF_NONE),
    /* Deku launch    */ DMG_ENTRY(2, EN_JSO2_DMGEFF_NONE),
    /* UNK_DMG_0x12   */ DMG_ENTRY(0, EN_JSO2_DMGEFF_STUN),
    /* Zora barrier   */ DMG_ENTRY(0, EN_JSO2_DMGEFF_ELECTRIC_STUN),
    /* Normal shield  */ DMG_ENTRY(0, EN_JSO2_DMGEFF_IMMUNE),
    /* Light ray      */ DMG_ENTRY(0, EN_JSO2_DMGEFF_IMMUNE),
    /* Thrown object  */ DMG_ENTRY(1, EN_JSO2_DMGEFF_NONE),
    /* Zora punch     */ DMG_ENTRY(1, EN_JSO2_DMGEFF_NONE),
    /* Spin attack    */ DMG_ENTRY(1, EN_JSO2_DMGEFF_NONE),
    /* Sword beam     */ DMG_ENTRY(0, EN_JSO2_DMGEFF_IMMUNE),
    /* Normal Roll    */ DMG_ENTRY(0, EN_JSO2_DMGEFF_IMMUNE),
    /* UNK_DMG_0x1B   */ DMG_ENTRY(0, EN_JSO2_DMGEFF_IMMUNE),
    /* UNK_DMG_0x1C   */ DMG_ENTRY(0, EN_JSO2_DMGEFF_IMMUNE),
    /* Unblockable    */ DMG_ENTRY(0, EN_JSO2_DMGEFF_IMMUNE),
    /* UNK_DMG_0x1E   */ DMG_ENTRY(0, EN_JSO2_DMGEFF_IMMUNE),
    /* Powder Keg     */ DMG_ENTRY(1, EN_JSO2_DMGEFF_NONE),
};

/**
 * When the Garo Master draws its swords, the flames on the swords will gradually scale up to match these scales.
 */
static Vec3f sTargetFlameScales[] = {
    { 0.003f, 0.003f, 0.003f }, { 0.002f, 0.002f, 0.002f }, { 0.001f, 0.001f, 0.001f },
    { 0.003f, 0.003f, 0.003f }, { 0.002f, 0.002f, 0.002f }, { 0.001f, 0.001f, 0.001f },
};

static Vec3f sFlameOffsets[] = {
    { 800.0f, -20.0f, -50.0f }, { 500.0f, -20.0f, -70.0f }, { 300.0f, -20.0f, -90.0f },
    { 800.0f, -20.0f, 50.0f },  { 500.0f, -20.0f, 70.0f },  { 300.0f, -20.0f, 90.0f },
};

static Vec3f sSlashFlameOffsets[] = {
    { 600.0f, -100.0f, 100.0f },  { 300.0f, -100.0f, 80.0f },  { 100.0f, -100.0f, 60.0f },
    { 600.0f, -100.0f, -100.0f }, { 300.0f, -100.0f, -80.0f }, { 100.0f, -100.0f, -60.0f },
};

ActorProfile En_Jso2_Profile = {
    /**/ ACTOR_EN_JSO2,
    /**/ ACTORCAT_ENEMY,
    /**/ FLAGS,
    /**/ OBJECT_JSO,
    /**/ sizeof(EnJso2),
    /**/ EnJso2_Init,
    /**/ EnJso2_Destroy,
    /**/ EnJso2_Update,
    /**/ EnJso2_Draw,
};

static ColliderCylinderInit sCylinderInit = {
    {
        COL_MATERIAL_NONE,
        AT_ON | AT_TYPE_ENEMY,
        AC_ON | AC_HARD | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_1,
        COLSHAPE_CYLINDER,
    },
    {
        ELEM_MATERIAL_UNK0,
        { 0xF7CFFFFF, 0x08, 0x04 },
        { 0xF7CFFFFF, 0x00, 0x00 },
        ATELEM_ON | ATELEM_SFX_NORMAL,
        ACELEM_ON,
        OCELEM_ON,
    },
    { 22, 70, 0, { 0, 0, 0 } },
};

static ColliderQuadInit sQuadInit = {
    {
        COL_MATERIAL_NONE,
        AT_ON | AT_TYPE_ENEMY,
        AC_NONE,
        OC1_NONE,
        OC2_NONE,
        COLSHAPE_QUAD,
    },
    {
        ELEM_MATERIAL_UNK0,
        { 0xF7CFFFFF, 0x09, 0x10 },
        { 0x00000000, 0x00, 0x00 },
        ATELEM_ON | ATELEM_SFX_NORMAL | ATELEM_UNK7,
        ACELEM_NONE,
        OCELEM_NONE,
    },
    { { { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } } },
};

typedef enum EnJso2Animation {
    /*  0 */ EN_JSO2_ANIM_DASH_ATTACK,
    /*  1 */ EN_JSO2_ANIM_SLASH_START,
    /*  2 */ EN_JSO2_ANIM_SLASH_LOOP,
    /*  3 */ EN_JSO2_ANIM_JUMP_BACK,
    /*  4 */ EN_JSO2_ANIM_DAMAGED,
    /*  5 */ EN_JSO2_ANIM_GUARD,
    /*  6 */ EN_JSO2_ANIM_APPEAR, // unused
    /*  7 */ EN_JSO2_ANIM_IDLE,   // unused
    /*  8 */ EN_JSO2_ANIM_BOUNCE,
    /*  9 */ EN_JSO2_ANIM_FALL_DOWN,    // unused
    /* 10 */ EN_JSO2_ANIM_KNOCKED_BACK, // unused
    /* 11 */ EN_JSO2_ANIM_COWER,        // unused
    /* 12 */ EN_JSO2_ANIM_LOOK_AROUND,
    /* 13 */ EN_JSO2_ANIM_APPEAR_AND_DRAW_SWORDS,
    /* 14 */ EN_JSO2_ANIM_SPIN_ATTACK,
    /* 15 */ EN_JSO2_ANIM_LAND,
    /* 16 */ EN_JSO2_ANIM_JUMP_DOWN,
    /* 17 */ EN_JSO2_ANIM_LAUGH,
    /* 18 */ EN_JSO2_DRAW_SWORDS,
    /* 19 */ EN_JSO2_COLLAPSE,
    /* 20 */ EN_JSO2_TREMBLE,
    /* 21 */ EN_JSO2_TAKE_OUT_BOMB,
    /* 22 */ EN_JSO2_ANIM_MAX
} EnJso2Animation;

static AnimationHeader* sAnimations[EN_JSO2_ANIM_MAX] = {
    &gGaroDashAttackAnim,          // EN_JSO2_ANIM_DASH_ATTACK
    &gGaroSlashStartAnim,          // EN_JSO2_ANIM_SLASH_START
    &gGaroSlashLoopAnim,           // EN_JSO2_ANIM_SLASH_LOOP
    &gGaroJumpBackAnim,            // EN_JSO2_ANIM_JUMP_BACK
    &gGaroDamagedAnim,             // EN_JSO2_ANIM_DAMAGED
    &gGaroGuardAnim,               // EN_JSO2_ANIM_GUARD
    &gGaroAppearAnim,              // EN_JSO2_ANIM_APPEAR
    &gGaroIdleAnim,                // EN_JSO2_ANIM_IDLE
    &gGaroBounceAnim,              // EN_JSO2_ANIM_BOUNCE
    &gGaroFallDownAnim,            // EN_JSO2_ANIM_FALL_DOWN
    &gGaroKnockedBackAnim,         // EN_JSO2_ANIM_KNOCKED_BACK
    &gGaroCowerAnim,               // EN_JSO2_ANIM_COWER
    &gGaroLookAroundAnim,          // EN_JSO2_ANIM_LOOK_AROUND
    &gGaroAppearAndDrawSwordsAnim, // EN_JSO2_ANIM_APPEAR_AND_DRAW_SWORDS
    &gGaroSpinAttackAnim,          // EN_JSO2_ANIM_SPIN_ATTACK
    &gGaroLandAnim,                // EN_JSO2_ANIM_LAND
    &gGaroJumpDownAnim,            // EN_JSO2_ANIM_JUMP_DOWN
    &gGaroLaughAnim,               // EN_JSO2_ANIM_LAUGH
    &gGaroDrawSwordsAnim,          // EN_JSO2_DRAW_SWORDS
    &gGaroCollapseAnim,            // EN_JSO2_COLLAPSE
    &gGaroTrembleAnim,             // EN_JSO2_TREMBLE
    &gGaroTakeOutBombAnim,         // EN_JSO2_TAKE_OUT_BOMB
};

static u8 sAnimationModes[EN_JSO2_ANIM_MAX] = {
    ANIMMODE_ONCE, // EN_JSO2_ANIM_DASH_ATTACK
    ANIMMODE_ONCE, // EN_JSO2_ANIM_SLASH_START
    ANIMMODE_LOOP, // EN_JSO2_ANIM_SLASH_LOOP
    ANIMMODE_ONCE, // EN_JSO2_ANIM_JUMP_BACK
    ANIMMODE_LOOP, // EN_JSO2_ANIM_DAMAGED
    ANIMMODE_ONCE, // EN_JSO2_ANIM_GUARD
    ANIMMODE_LOOP, // EN_JSO2_ANIM_APPEAR
    ANIMMODE_LOOP, // EN_JSO2_ANIM_IDLE
    ANIMMODE_LOOP, // EN_JSO2_ANIM_BOUNCE
    ANIMMODE_ONCE, // EN_JSO2_ANIM_FALL_DOWN
    ANIMMODE_ONCE, // EN_JSO2_ANIM_KNOCKED_BACK
    ANIMMODE_LOOP, // EN_JSO2_ANIM_COWER
    ANIMMODE_LOOP, // EN_JSO2_ANIM_LOOK_AROUND
    ANIMMODE_ONCE, // EN_JSO2_ANIM_APPEAR_AND_DRAW_SWORDS
    ANIMMODE_LOOP, // EN_JSO2_ANIM_SPIN_ATTACK
    ANIMMODE_ONCE, // EN_JSO2_ANIM_LAND
    ANIMMODE_ONCE, // EN_JSO2_ANIM_JUMP_DOWN
    ANIMMODE_LOOP, // EN_JSO2_ANIM_LAUGH
    ANIMMODE_ONCE, // EN_JSO2_DRAW_SWORDS
    ANIMMODE_ONCE, // EN_JSO2_COLLAPSE
    ANIMMODE_LOOP, // EN_JSO2_TREMBLE
    ANIMMODE_ONCE, // EN_JSO2_TAKE_OUT_BOMB
};

void EnJso2_Init(Actor* thisx, PlayState* play) {
    EnJso2* this = (EnJso2*)thisx;
    EffectBlureInit1 rightSwordBlureInit;
    EffectBlureInit1 leftSwordBlureInit;

    this->actor.hintId = TATL_HINT_ID_GARO_MASTER;
    this->actor.attentionRangeType = ATTENTION_RANGE_5;
    this->actor.colChkInfo.mass = 80;
    this->actor.colChkInfo.health = 14;
    ActorShape_Init(&this->actor.shape, 0.0f, ActorShadow_DrawCircle, 0.0f);
    this->actor.colChkInfo.damageTable = &sDamageTable;
    this->actor.shape.shadowScale = 0.0f;
    SkelAnime_InitFlex(play, &this->skelAnime, &gGaroMasterSkel, &gGaroAppearAnim, this->jointTable, this->morphTable,
                       GARO_MASTER_LIMB_MAX);
    Collider_InitAndSetCylinder(play, &this->bodyCollider, &this->actor, &sCylinderInit);
    Collider_InitAndSetQuad(play, &this->rightSwordCollider, &this->actor, &sQuadInit);
    Collider_InitAndSetQuad(play, &this->leftSwordCollider, &this->actor, &sQuadInit);

    rightSwordBlureInit.p1StartColor[0] = leftSwordBlureInit.p1StartColor[0] = 255;
    rightSwordBlureInit.p1StartColor[1] = leftSwordBlureInit.p1StartColor[1] = 0;
    rightSwordBlureInit.p1StartColor[2] = leftSwordBlureInit.p1StartColor[2] = 0;
    rightSwordBlureInit.p1StartColor[3] = leftSwordBlureInit.p1StartColor[3] = 128;

    rightSwordBlureInit.p2StartColor[0] = leftSwordBlureInit.p2StartColor[0] = 255;
    rightSwordBlureInit.p2StartColor[1] = leftSwordBlureInit.p2StartColor[1] = 0;
    rightSwordBlureInit.p2StartColor[2] = leftSwordBlureInit.p2StartColor[2] = 0;

    rightSwordBlureInit.p1EndColor[0] = leftSwordBlureInit.p1EndColor[0] = 255;
    rightSwordBlureInit.p1EndColor[1] = leftSwordBlureInit.p1EndColor[1] = 0;
    rightSwordBlureInit.p1EndColor[2] = leftSwordBlureInit.p1EndColor[2] = 0;

    rightSwordBlureInit.p2EndColor[0] = leftSwordBlureInit.p2EndColor[0] = 255;
    rightSwordBlureInit.p2EndColor[1] = leftSwordBlureInit.p2EndColor[1] = 0;
    rightSwordBlureInit.p2EndColor[2] = leftSwordBlureInit.p2EndColor[2] = 255;

    rightSwordBlureInit.p2StartColor[3] = leftSwordBlureInit.p2StartColor[3] = 64;
    rightSwordBlureInit.p1EndColor[3] = leftSwordBlureInit.p1EndColor[3] = rightSwordBlureInit.p2EndColor[3] =
        leftSwordBlureInit.p2EndColor[3] = 0;

    rightSwordBlureInit.elemDuration = leftSwordBlureInit.elemDuration = 8;
    rightSwordBlureInit.unkFlag = leftSwordBlureInit.unkFlag = 0;
    rightSwordBlureInit.calcMode = leftSwordBlureInit.calcMode = 2;

    Effect_Add(play, &this->rightSwordBlureIndex, EFFECT_BLURE1, 0, 0, &rightSwordBlureInit);
    Effect_Add(play, &this->leftSwordBlureIndex, EFFECT_BLURE1, 0, 0, &leftSwordBlureInit);

    this->scale = 0.042f;
    this->type = EN_JSO2_GET_TYPE(&this->actor);
    this->flameScroll = Rand_S16Offset(0, 7);
    this->bodyPartsCount = EN_JSO2_BODYPART_MAX;
    this->alpha = 255;

    if (this->type == EN_JSO2_TYPE_LIGHT_ARROW_ROOM) {
        this->actor.draw = NULL;
        this->actor.flags |= ACTOR_FLAG_LOCK_ON_DISABLED;
        this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
        this->actor.shape.yOffset = 0.0f;
        EnJso2_SetupIntroCutscene(this);
    } else {
        this->actor.gravity = -3.0f;
        this->actor.world.rot.y = this->actor.shape.rot.y = this->actor.yawTowardsPlayer;
        this->actor.shape.yOffset = 960.0f;
        EnJso2_SetupAppear(this);
    }
}

void EnJso2_Destroy(Actor* thisx, PlayState* play) {
    EnJso2* this = (EnJso2*)thisx;

    Collider_DestroyCylinder(play, &this->bodyCollider);
    Collider_DestroyQuad(play, &this->rightSwordCollider);
    Collider_DestroyQuad(play, &this->leftSwordCollider);
    Effect_Destroy(play, this->rightSwordBlureIndex);
    Effect_Destroy(play, this->leftSwordBlureIndex);
    Audio_SetMainBgmVolume(0x7F, 0);
    Audio_RestorePrevBgm();
}

void EnJso2_ChangeAnim(EnJso2* this, s32 animIndex) {
    f32 playSpeed;

    this->animEndFrame = Animation_GetLastFrame(sAnimations[animIndex]);
    this->animIndex = animIndex;

    playSpeed = 1.0f;
    if (animIndex == EN_JSO2_ANIM_BOUNCE) {
        playSpeed = 2.0f;
    }

    Animation_Change(&this->skelAnime, sAnimations[animIndex], playSpeed, 0.0f, this->animEndFrame,
                     sAnimationModes[animIndex], -2.0f);
}

/**
 * Updates all of the various sub-camera variables to approach their "next" value, then updates the sub-camera. Used for
 * the Garo Master's intro and death cutscenes.
 */
void EnJso2_UpdateSubCam(EnJso2* this, PlayState* play) {
    if (this->subCamId != SUB_CAM_ID_DONE) {
        Math_ApproachF(&this->subCamEye.x, this->subCamEyeNext.x, this->subCamMaxVelocityFrac, this->subCamVelocity);
        Math_ApproachF(&this->subCamEye.y, this->subCamEyeNext.y, this->subCamMaxVelocityFrac, this->subCamVelocity);
        Math_ApproachF(&this->subCamEye.z, this->subCamEyeNext.z, this->subCamMaxVelocityFrac, this->subCamVelocity);
        Math_ApproachF(&this->subCamAt.x, this->subCamAtNext.x, this->subCamMaxVelocityFrac, this->subCamVelocity);
        Math_ApproachF(&this->subCamAt.y, this->subCamAtNext.y, this->subCamMaxVelocityFrac, this->subCamVelocity);
        Math_ApproachF(&this->subCamAt.z, this->subCamAtNext.z, this->subCamMaxVelocityFrac, this->subCamVelocity);
        Math_ApproachF(&this->subCamFov, this->subCamFovNext, 0.3f, 10.0f);
        Play_SetCameraAtEye(play, this->subCamId, &this->subCamAt, &this->subCamEye);
        Play_SetCameraFov(play, this->subCamId, this->subCamFov);
    }
}

s32 EnJso2_ShouldAdvanceMessage(PlayState* play) {
    if ((Message_GetState(&play->msgCtx) == TEXT_STATE_EVENT) && Message_ShouldAdvance(play)) {
        Message_CloseTextbox(play);
        return true;
    }

    return false;
}

void EnJso2_SetupIntroCutscene(EnJso2* this) {
    this->action = EN_JSO2_ACTION_INTRO_CUTSCENE;
    this->swordState = EN_JSO2_SWORD_STATE_NONE_DRAWN;
    this->actionFunc = EnJso2_IntroCutscene;
}

static Vec3s sPlayerOverrideInputPosList[] = { { 350, -20, -3430 } };

/**
 * Responsible for handling all aspects of the intro cutscene, including manipulating the sub-camera, making the Garo
 * Master jump down from the ceiling, spawning the falling rocks, igniting the Garo Master's swords, etc. When the
 * cutscene is over, the Garo Master will start circling the player.
 */
void EnJso2_IntroCutscene(EnJso2* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    f32 curFrame = this->skelAnime.curFrame;
    Vec3f pos;
    s32 i;
    s32 pad;

    switch (this->cutsceneState) {
        case EN_JSO2_INTRO_CS_STATE_WAITING_FOR_PLAYER_TO_APPROACH:
            if (this->actor.xzDistToPlayer < 400.0f) {
                Audio_SetMainBgmVolume(0, 10);
                this->actor.gravity = 0.0f;
                this->cutsceneState++;
            }
            break;

        case EN_JSO2_INTRO_CS_STATE_STARTED:
            if (!CutsceneManager_IsNext(this->actor.csId)) {
                CutsceneManager_Queue(this->actor.csId);
            } else {
                CutsceneManager_StartWithPlayerCs(this->actor.csId, &this->actor);
                this->subCamId = CutsceneManager_GetCurrentSubCamId(this->actor.csId);
                this->subCamMaxVelocityFrac = 0.4f;
                this->subCamVelocity = 40.0f;
                player->actor.world.pos.x = 420.0f;
                player->actor.world.pos.z = -3430.0f;
                this->actor.draw = EnJso2_Draw;
                EnJso2_ChangeAnim(this, EN_JSO2_ANIM_LAUGH);
                pos.x = sPlayerOverrideInputPosList[0].x;
                pos.y = sPlayerOverrideInputPosList[0].y;
                pos.z = sPlayerOverrideInputPosList[0].z;
                this->actor.world.rot.y = Math_Vec3f_Yaw(&this->actor.world.pos, &pos);
                this->subCamFov = this->subCamFovNext = 60.0f;
                this->actor.world.pos.x = -285.0f;
                this->actor.world.pos.y = 535.0f;
                this->actor.world.pos.z = -3425.0f;
                this->subCamEyeNext.x = 82.0f;
                this->subCamEyeNext.y = -586.0f;
                this->subCamEyeNext.z = 780.0f;
                this->subCamEyeNext.y += 500.0f;
                this->subCamAtNext.y = this->subCamEyeNext.y;
                Math_Vec3f_Copy(&pos, &this->subCamEyeNext);
                OLib_Vec3fAdd(&this->actor.world, &pos, &this->subCamEyeNext, OLIB_ADD_OFFSET);
                this->subCamAtNext.x = this->actor.world.pos.x - 90.0f;
                this->subCamAtNext.z = this->actor.world.pos.z + 100.0f;
                Math_Vec3f_Copy(&this->subCamEye, &this->subCamEyeNext);
                Math_Vec3f_Copy(&this->subCamAt, &this->subCamAtNext);
                Player_SetCsActionWithHaltedActors(play, &this->actor, PLAYER_CSACTION_END);
                Player_InitOverrideInput(play, &this->overrideInputEntry, ARRAY_COUNT(sPlayerOverrideInputPosList),
                                         sPlayerOverrideInputPosList);
                this->cutsceneState++;
            }
            break;

        case EN_JSO2_INTRO_CS_STATE_MOVE_PLAYER_FORWARD:
            this->actor.world.pos.x = -285.0f;
            this->actor.world.pos.y = 535.0f;
            this->actor.world.pos.z = -3425.0f;
            this->subCamEyeNext.x = 82.0f;
            this->subCamEyeNext.y = -516.0f;
            this->subCamEyeNext.z = 800.0f;
            Math_Vec3f_Copy(&pos, &this->subCamEyeNext);
            OLib_Vec3fAdd(&this->actor.world, &pos, &this->subCamEyeNext, OLIB_ADD_OFFSET);
            this->subCamAtNext.x = this->actor.world.pos.x - 90.0f;
            this->subCamAtNext.y = this->actor.world.pos.y - 591.0f;
            this->subCamAtNext.z = this->actor.world.pos.z + 100.0f;
            Math_Vec3f_Copy(&this->subCamEye, &this->subCamEyeNext);
            Math_Vec3f_Copy(&this->subCamAt, &this->subCamAtNext);

            if (Player_UpdateOverrideInput(play, &this->overrideInputEntry, 60.0f)) {
                Player_SetCsActionWithHaltedActors(play, NULL, PLAYER_CSACTION_19);
                this->cutsceneTimer = 10;
                this->cutsceneState++;
            }
            break;

        case EN_JSO2_INTRO_CS_STATE_SPAWN_FALLING_ROCKS:
            if (this->cutsceneTimer == 1) {
                Vec3f rockPos;
                s32 i;

                for (i = 0; i < 2; i++) {
                    Math_Vec3f_Copy(&rockPos, &this->actor.home.pos);
                    rockPos.x += Rand_CenteredFloat(80.0f);
                    rockPos.y = this->actor.world.pos.y + (i * 120.0f);
                    rockPos.z += Rand_CenteredFloat(80.0f);
                    Actor_Spawn(&play->actorCtx, play, ACTOR_EN_COL_MAN, rockPos.x, rockPos.y, rockPos.z, 0,
                                this->actor.world.rot.y, 0, EN_COL_MAN_FALLING_ROCK);
                }

                this->cutsceneTimer = 25;
                this->cutsceneState++;
            }
            break;

        case EN_JSO2_INTRO_CS_STATE_WAIT_FOR_ROCKS_TO_FALL:
            if (this->cutsceneTimer == 0) {
                Player_SetCsActionWithHaltedActors(play, NULL, PLAYER_CSACTION_4);
                this->cutsceneTimer = 20;
                this->cutsceneState++;
            }
            break;

        case EN_JSO2_INTRO_CS_STATE_PLAYER_SURPRISED:
            if (this->cutsceneTimer == 0) {
                this->cutsceneTimer = 20;
                this->cutsceneState++;
            }
            break;

        case EN_JSO2_INTRO_CS_STATE_PAN_CAMERA_UPWARDS:
            this->subCamEyeNext.x = 82.0f;
            this->subCamEyeNext.y = -533.0f;
            this->subCamEyeNext.z = 800.0f;
            Math_Vec3f_Copy(&pos, &this->subCamEyeNext);
            OLib_Vec3fAdd(&this->actor.world, &pos, &this->subCamEyeNext, OLIB_ADD_OFFSET);
            this->subCamAtNext.x = this->actor.focus.pos.x - 80.0f;
            this->subCamAtNext.y = this->actor.focus.pos.y - 360.0f;
            this->subCamAtNext.z = this->actor.focus.pos.z + 100.0f;

            if (this->cutsceneTimer == 0) {
                this->cutsceneTimer = 5;
                this->cutsceneState++;
            }
            break;

        case EN_JSO2_INTRO_CS_STATE_WAIT_BEFORE_LAUGH:
            if (this->cutsceneTimer == 0) {
                Actor_PlaySfx(&this->actor, NA_SE_EN_ANSATSUSYA_LAUGH);
                this->cutsceneTimer = 45;
                this->cutsceneState++;
            }
            break;

        case EN_JSO2_INTRO_CS_STATE_LAUGH:
            if (this->cutsceneTimer == 0) {
                this->actor.speed = 3.0f;
                this->actor.velocity.y = 10.0f;
                this->actor.gravity = -1.0f;
                EnJso2_ChangeAnim(this, EN_JSO2_ANIM_JUMP_DOWN);
                this->subCamMaxVelocityFrac = 0.4f;
                this->subCamVelocity = 40.0f;
                Actor_PlaySfx(&this->actor, NA_SE_EN_PO_ROLL);
                Math_Vec3f_Copy(&this->baseSubCamEye, &this->subCamEye);
                this->cutsceneTimer = 25;
                this->cutsceneState++;
            }
            break;

        case EN_JSO2_INTRO_CS_STATE_JUMP_DOWN:
            this->afterimageIndex++;
            if (this->afterimageIndex >= EN_JSO2_AFTERIMAGE_COUNT) {
                this->afterimageIndex = 0;
            }

            if (this->afterimageCount < EN_JSO2_AFTERIMAGE_COUNT - 1) {
                this->afterimageCount++;
            }

            Math_Vec3f_Copy(&this->afterimagePos[this->afterimageIndex], &this->actor.world.pos);
            Math_Vec3s_Copy(&this->afterimageRot[this->afterimageIndex], &this->actor.world.rot);
            this->afterimagePos[this->afterimageIndex].y += 40.0f;

            for (i = 0; i < EN_JSO2_AFTERIMAGE_COUNT; i++) {
                this->afterimageJointTable[this->afterimageIndex][i] = this->jointTable[i];
            }

            Math_ApproachF(&this->actor.shape.shadowScale, 17.0f, 0.4f, 4.0f);

            if (this->cutsceneTimer == 0) {
                this->subCamEyeNext.x = this->baseSubCamEye.x - 490.0f;
                this->subCamEyeNext.y = this->baseSubCamEye.y;
                this->subCamEyeNext.z = this->baseSubCamEye.z + 100.0f;
            }

            this->subCamAtNext.x = this->actor.focus.pos.x - 80.0f;
            this->subCamAtNext.y = this->actor.focus.pos.y - 130.0f;
            this->subCamAtNext.z = this->actor.focus.pos.z;

            if (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND) {
                Actor_PlaySfx(&this->actor, NA_SE_EN_ANSATSUSYA_ONGND);
                this->actor.gravity = 0.0f;
                this->actor.velocity.y = 0.0f;
                this->actor.speed = 0.0f;
                EnJso2_ChangeAnim(this, EN_JSO2_ANIM_LAND);
                Actor_SpawnFloorDustRing(play, &this->actor, &this->actor.world.pos, this->actor.shape.shadowScale, 10,
                                         4.0f, 500, 50, true);
                this->cutsceneTimer = 1;
                Actor_PlaySfx(&this->actor, NA_SE_EN_EYEGOLE_ATTACK);
                this->cutsceneState++;
            }
            break;

        case EN_JSO2_INTRO_CS_STATE_LAND:
            this->afterimageIndex++;
            if (this->afterimageIndex >= EN_JSO2_AFTERIMAGE_COUNT) {
                this->afterimageIndex = 0;
            }

            if (this->afterimageCount < EN_JSO2_AFTERIMAGE_COUNT - 1) {
                this->afterimageCount++;
            }

            Math_Vec3f_Copy(&this->afterimagePos[this->afterimageIndex], &this->actor.world.pos);
            Math_Vec3s_Copy(&this->afterimageRot[this->afterimageIndex], &this->actor.world.rot);
            this->afterimagePos[this->afterimageIndex].y += 40.0f;

            for (i = 0; i < EN_JSO2_AFTERIMAGE_COUNT; i++) {
                this->afterimageJointTable[this->afterimageIndex][i] = this->jointTable[i];
            }

            if (this->cutsceneTimer == 0) {
                this->subCamEyeNext.x = this->baseSubCamEye.x - 518.0f;
                this->subCamEyeNext.y = this->baseSubCamEye.y - 11.0f;
                this->subCamEyeNext.z = this->baseSubCamEye.z + 100.0f;
            }

            this->subCamAtNext.x = this->actor.focus.pos.x + 20.0f;
            this->subCamAtNext.y = this->actor.focus.pos.y - 50.0f;
            this->subCamAtNext.z = this->actor.focus.pos.z;

            if (curFrame >= this->animEndFrame) {
                this->afterimageCount = 0;
                this->afterimageIndex = 0;
                this->cutsceneTimer = 20;
                EnJso2_ChangeAnim(this, EN_JSO2_DRAW_SWORDS);
                this->cutsceneState++;
            }
            break;

        case EN_JSO2_INTRO_CS_STATE_DRAW_SWORDS:
            if (Animation_OnFrame(&this->skelAnime, 17.0f)) {
                Actor_PlaySfx(&this->actor, NA_SE_EN_ANSATSUSYA_SWORD);
                Actor_PlaySfx(&this->actor, NA_SE_EV_FLAME_IGNITION);
                this->swordState = EN_JSO2_SWORD_STATE_BOTH_DRAWN;
                Audio_SetMainBgmVolume(0x7F, 0);
                Audio_PlayBgm_StorePrevBgm(NA_BGM_MINI_BOSS);
            }

            if (this->cutsceneTimer == 0) {
                this->subCamEyeNext.x = this->baseSubCamEye.x - 470.0f;
                this->subCamEyeNext.y = this->baseSubCamEye.y - 10.0f;
                this->subCamEyeNext.z = this->baseSubCamEye.z + 100.0f;
                this->subCamAtNext.x = this->actor.focus.pos.x - 80.0f;
                this->subCamAtNext.y = this->actor.focus.pos.y - 30.0f;
                this->subCamAtNext.z = this->actor.focus.pos.z;

                if (this->swordState == EN_JSO2_SWORD_STATE_BOTH_DRAWN) {
                    for (i = 0; i < EN_JSO2_FLAME_COUNT; i++) {
                        Math_ApproachF(&this->flameScale[i].x, sTargetFlameScales[i].x, 0.3f, 0.0005f);
                        this->flameScale[i].y = this->flameScale[i].x;
                        this->flameScale[i].z = this->flameScale[i].x;
                    }
                }
            }

            if (curFrame >= this->animEndFrame) {
                CutsceneManager_Stop(this->actor.csId);
                this->subCamId = SUB_CAM_ID_DONE;
                this->actor.flags &= ~ACTOR_FLAG_FREEZE_EXCEPTION;
                this->actor.gravity = -3.0f;
                this->actor.flags &= ~ACTOR_FLAG_LOCK_ON_DISABLED;
                this->actor.flags |= ACTOR_FLAG_ATTENTION_ENABLED;
                EnJso2_SetupCirclePlayer(this, play);
            }
            break;

        default:
            break;
    }

    this->actor.shape.yOffset = 960.0f;
    EnJso2_UpdateSubCam(this, play);
}

void EnJso2_SetupAppear(EnJso2* this) {
    this->swordState = EN_JSO2_SWORD_STATE_NONE_DRAWN;
    this->bodyCollider.base.acFlags |= AC_HARD;
    this->actor.flags &= ~ACTOR_FLAG_FREEZE_EXCEPTION;
    EnJso2_ChangeAnim(this, EN_JSO2_ANIM_APPEAR_AND_DRAW_SWORDS);
    this->actionFunc = EnJso2_Appear;
}

/**
 * Makes the Garo Master jump out of the ground, draw its sword, and start the fight. This function is roughly analogous
 * to the function for handling the intro cutscene, however this function does not actually start a cutscene, and all of
 * the Garo Master's actions happen during gameplay.
 */
void EnJso2_Appear(EnJso2* this, PlayState* play) {
    f32 curFrame = this->skelAnime.curFrame;
    s32 i;

    this->actor.world.rot.y = this->actor.shape.rot.y = this->actor.yawTowardsPlayer;

    switch (this->appearState) {
        case EN_JSO2_APPEAR_STATE_STARTED:
            Actor_SpawnFloorDustRing(play, &this->actor, &this->actor.world.pos, this->actor.shape.shadowScale, 1, 8.0f,
                                     500, 10, true);
            Audio_SetMainBgmVolume(0, 10);
            Actor_PlaySfx(&this->actor, NA_SE_EN_ANSATSUSYA_ENTRY);
            this->appearState++;
            break;

        case EN_JSO2_APPEAR_STATE_APPEARING:
            if (Animation_OnFrame(&this->skelAnime, 18.0f)) {
                Actor_SpawnFloorDustRing(play, &this->actor, &this->actor.world.pos, this->actor.shape.shadowScale, 1,
                                         8.0f, 500, 10, true);
            }

            Math_ApproachF(&this->actor.shape.shadowScale, 17.0f, 0.4f, 4.0f);

            if (Animation_OnFrame(&this->skelAnime, 45.0f)) {
                Audio_SetMainBgmVolume(0x7F, 0);
                Audio_PlayBgm_StorePrevBgm(NA_BGM_MINI_BOSS);
                Actor_PlaySfx(&this->actor, NA_SE_EN_ANSATSUSYA_SWORD);
                Actor_PlaySfx(&this->actor, NA_SE_EV_FLAME_IGNITION);
                this->swordState = EN_JSO2_SWORD_STATE_BOTH_DRAWN;
            }

            if (this->swordState == EN_JSO2_SWORD_STATE_BOTH_DRAWN) {
                for (i = 0; i < EN_JSO2_FLAME_COUNT; i++) {
                    Math_ApproachF(&this->flameScale[i].x, sTargetFlameScales[i].x, 0.3f, 0.0005f);
                    this->flameScale[i].y = this->flameScale[i].x;
                    this->flameScale[i].z = this->flameScale[i].x;
                }
            }

            if (curFrame >= this->animEndFrame) {
                EnJso2_SetupCirclePlayer(this, play);
            }
            break;

        default:
            break;
    }
}

void EnJso2_SetupCirclePlayer(EnJso2* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    EnJso2_ChangeAnim(this, EN_JSO2_ANIM_BOUNCE);
    this->circlingAngle = player->actor.shape.rot.y;
    this->circlingAngularVelocity = 0x258;
    this->action = EN_JSO2_ACTION_CIRCLE_PLAYER;
    this->isTeleporting = false;
    this->bodyCollider.base.acFlags |= AC_HARD;
    this->actionFunc = EnJso2_CirclePlayer;
}

/**
 * Makes the Garo Master bounce in a circle around the player, sometimes randomly switching the direction it's
 * traveling, until the attack timer reaches 0. Once it does, then the Garo Master will prepare to attack.
 */
void EnJso2_CirclePlayer(EnJso2* this, PlayState* play) {
    f32 curFrame = this->skelAnime.curFrame;
    Player* player = GET_PLAYER(play);
    Vec3f targetPos;
    s32 pad;

    Actor_PlaySfx(&this->actor, NA_SE_EN_ANSATSUSYA_MOVING - SFX_FLAG);

    if (curFrame < this->animEndFrame) {
        SkelAnime_Update(&this->skelAnime);
    } else if (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND) {
        SkelAnime_Update(&this->skelAnime);
    }

    if (Animation_OnFrame(&this->skelAnime, 6.0f)) {
        this->actor.velocity.y = 10.0f;
        if ((play->gameplayFrames % 2) == 0) {
            Actor_PlaySfx(&this->actor, NA_SE_EN_ANSATSUSYA_CRYING);
        }
    }

    if (Animation_OnFrame(&this->skelAnime, 12.0f)) {
        Actor_PlaySfx(&this->actor, NA_SE_EN_ANSATSUSYA_SKIP);
        this->actor.speed = 0.0f;
        if (Rand_ZeroFloat(1.0f) < 0.5f) {
            this->circlingAngularVelocity = -this->circlingAngularVelocity;
        }
    }

    if (this->attackTimer == 0) {
        this->actor.speed = 0.0f;
        EnJso2_SetupSpinBeforeAttack(this);
        return;
    }

    Math_SmoothStepToS(&this->actor.shape.rot.y, this->actor.yawTowardsPlayer, 0xA, 0xFA0, 0x14);
    Math_ApproachF(&this->actor.speed, 5.0f, 0.3f, 2.0f);
    this->circlingAngle += this->circlingAngularVelocity;
    targetPos.x = (Math_SinS(this->circlingAngle) * 200.0f) + player->actor.world.pos.x;
    targetPos.y = this->actor.world.pos.y;
    targetPos.z = (Math_CosS(this->circlingAngle) * 200.0f) + player->actor.world.pos.z;
    Math_SmoothStepToS(&this->actor.world.rot.y, Math_Vec3f_Yaw(&this->actor.world.pos, &targetPos), 0xA, 0xFA0, 0x14);
}

void EnJso2_SetupGuard(EnJso2* this) {
    EnJso2_ChangeAnim(this, EN_JSO2_ANIM_GUARD);
    this->actor.world.rot.y = this->actor.yawTowardsPlayer;
    Actor_PlaySfx(&this->actor, NA_SE_IT_SHIELD_BOUND);
    this->bodyCollider.base.acFlags |= AC_HARD;
    this->action = EN_JSO2_ACTION_GUARD;
    this->actionFunc = EnJso2_Guard;
}

/**
 * Plays the guard animation to completion, then goes back to circling the player.
 */
void EnJso2_Guard(EnJso2* this, PlayState* play) {
    f32 curFrame = this->skelAnime.curFrame;

    if (curFrame >= this->animEndFrame) {
        EnJso2_SetupCirclePlayer(this, play);
    }
}

void EnJso2_SetupSpinBeforeAttack(EnJso2* this) {
    EnJso2_ChangeAnim(this, EN_JSO2_ANIM_DASH_ATTACK);
    this->actor.world.rot.y = -this->actor.yawTowardsPlayer;
    this->bodyCollider.base.acFlags |= AC_HARD;
    this->action = EN_JSO2_ACTION_SPIN_BEFORE_ATTACK;
    this->actor.speed = 10.0f;
    this->actor.velocity.y = 20.0f;
    this->actionFunc = EnJso2_SpinBeforeAttack;
}

/**
 * Leaps into the air while spinning forward. Once the Garo Master touches the ground, it will begin a dash attack.
 */
void EnJso2_SpinBeforeAttack(EnJso2* this, PlayState* play) {
    this->actor.world.rot.x += 0x1770;
    Math_SmoothStepToS(&this->actor.shape.rot.y, this->actor.yawTowardsPlayer, 0xA, 0xFA0, 0x14);

    if (this->actor.velocity.y > 0.0f) {
        return;
    }

    if (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND) {
        this->actor.world.rot.x = 0;
        this->actor.velocity.y = 0.0f;
        this->actor.speed = 0.0f;
        this->actor.world.rot.y = this->actor.shape.rot.y = this->actor.yawTowardsPlayer;
        EnJso2_SetupDashAttack(this);
    }
}

void EnJso2_SetupDashAttack(EnJso2* this) {
    this->attackMovementTimer = 40;
    this->bodyCollider.base.acFlags |= AC_HARD;
    this->actor.speed = 15.0f;
    this->actor.velocity.y = 13.0f;
    Actor_PlaySfx(&this->actor, NA_SE_EN_ANSATSUSYA_ENTRY);
    Actor_PlaySfx(&this->actor, NA_SE_EN_ANSATSUSYA_DASH_2);
    this->action = EN_JSO2_ACTION_DASH_ATTACK;
    this->actionFunc = EnJso2_DashAttack;
}

/**
 * Dashes toward the player with its swords out. If the player shields the attack, the Garo Master will jump backwards a
 * small amount. The Garo Master also has a random chance of teleporting every time it touches the ground. Assuming it
 * does not teleport, the Garo Master will stop dashing and perform a slash if it's close enough to the player, if the
 * difference between its y-rotation and the yaw towards the player is large enough (usually indicating that it has
 * dashed past the player), or if 40 frames have passed, whichever comes first.
 */
void EnJso2_DashAttack(EnJso2* this, PlayState* play) {
    f32 curFrame = this->skelAnime.curFrame;
    s16 yawDiff;
    s16 absYawDiff;
    Vec3f knockbackVelocity;

    if ((this->rightSwordCollider.base.atFlags & AT_BOUNCED) || (this->leftSwordCollider.base.atFlags & AT_BOUNCED)) {
        this->rightSwordCollider.base.atFlags &= ~(AT_HIT | AT_BOUNCED);
        this->leftSwordCollider.base.atFlags &= ~(AT_HIT | AT_BOUNCED);
        Matrix_RotateYS(this->actor.yawTowardsPlayer, MTXMODE_NEW);
        Matrix_MultVecZ(-10.0f, &knockbackVelocity);
        Math_Vec3f_Copy(&this->knockbackVelocity, &knockbackVelocity);
        this->disableBlure = true;
        this->attackMovementTimer = 0;
        AudioSfx_SetChannelIO(&this->actor.projectedPos, NA_SE_EN_ANSATSUSYA_DASH_2, 0);
        EnJso2_SetupJumpBack(this);
        return;
    }

    if ((this->actor.velocity.y < 0.0f) && (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND)) {
        if (Rand_ZeroOne() < ((BREG(22) * 0.1f) + 0.7f)) {
            this->actor.velocity.y = 13.0f;
        } else {
            AudioSfx_SetChannelIO(&this->actor.projectedPos, NA_SE_EN_ANSATSUSYA_DASH_2, 0);
            this->disableBlure = true;
            this->isTeleporting = true;
            this->actor.speed = 0.0f;
            EnJso2_SetupTeleport(this);
            return;
        }
    }

    if (curFrame < this->animEndFrame) {
        return;
    }

    yawDiff = this->actor.yawTowardsPlayer - this->actor.shape.rot.y;
    absYawDiff = ABS_ALT(yawDiff);

    if ((this->attackMovementTimer == 0) || (this->actor.xzDistToPlayer < 120.0f) || (absYawDiff > 0x4300)) {
        AudioSfx_SetChannelIO(&this->actor.projectedPos, NA_SE_EN_ANSATSUSYA_DASH_2, 0);
        Math_ApproachZeroF(&this->actor.speed, 0.3f, 3.0f);
        EnJso2_SetupSlash(this, play);
    }
}

void EnJso2_SetupTeleport(EnJso2* this) {
    EnJso2_ChangeAnim(this, EN_JSO2_ANIM_DASH_ATTACK);
    this->attackMovementTimer = 20;
    this->actor.speed = 0.0f;
    this->actor.gravity = 0.0f;
    this->actor.velocity.y = 10.0f;
    Actor_PlaySfx(&this->actor, NA_SE_EN_ANSATSUSYA_JUMP);
    this->actor.flags |= ACTOR_FLAG_LOCK_ON_DISABLED;
    this->action = EN_JSO2_ACTION_TELEPORT;
    this->actionFunc = EnJso2_Teleport;
}

/**
 * Jump into the air while spinning around and shrinking. After 20 frames, this function will move the Garo Master to be
 * directly above the player's current location.
 */
void EnJso2_Teleport(EnJso2* this, PlayState* play) {
    this->actor.shape.rot.y -= 0x1D4C;
    Math_ApproachZeroF(&this->scale, 0.3f, 0.01f);
    Math_ApproachZeroF(&this->actor.shape.shadowScale, 0.3f, 3.0f);

    if (this->attackMovementTimer == 0) {
        EnJso2_SetupFallFromTeleport(this, play);
    }
}

void EnJso2_SetupFallFromTeleport(EnJso2* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    Math_Vec3f_Copy(&this->actor.world.pos, &player->actor.world.pos);
    this->actor.world.pos.y += 300.0f + BREG(52);
    this->actor.velocity.y = 0.0f;
    this->actor.gravity = BREG(53) + -3.0f;
    Actor_PlaySfx(&this->actor, NA_SE_EN_ANSATSUSYA_FALL);
    this->action = EN_JSO2_ACTION_FALL_FROM_TELEPORT;
    this->actionFunc = EnJso2_FallFromTeleport;
}

/**
 * Makes the Garo Master grow back to its original size and fall down onto where the player was standing when
 * EnJso2_SetupFallFromTeleport was called. When it touches the ground, it will perform a slash.
 */
void EnJso2_FallFromTeleport(EnJso2* this, PlayState* play) {
    Math_ApproachF(&this->scale, 0.042f, 0.3f, 0.03f);
    Math_ApproachF(&this->actor.shape.shadowScale, 17.0f, 0.4f, 4.0f);

    if (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND) {
        this->isTeleporting = false;
        this->scale = 0.042f;
        this->actor.shape.shadowScale = 17.0f;
        this->actor.flags &= ~ACTOR_FLAG_LOCK_ON_DISABLED;
        EnJso2_SetupSlash(this, play);
    }
}

void EnJso2_SetupSlash(EnJso2* this, PlayState* play) {
    EnJso2_ChangeAnim(this, EN_JSO2_ANIM_SLASH_START);
    Actor_SpawnFloorDustRing(play, &this->actor, &this->actor.world.pos, this->actor.shape.shadowScale, 1, 8.0f, 500,
                             10, true);
    Math_ApproachZeroF(&this->actor.speed, 0.3f, 3.0f);
    this->slashHitSomething = false;
    Actor_PlaySfx(&this->actor, NA_SE_IT_SWORD_SWING_HARD);
    this->action = EN_JSO2_ACTION_SLASH;
    this->bodyCollider.base.acFlags &= ~AC_HARD;
    this->actionFunc = EnJso2_Slash;
}

/**
 * Slash in place with both swords. If the player shields the attack, the Garo Master will jump backwards.
 * Once the slash animation ends, this function will transition the Garo Master to a waiting state.
 */
void EnJso2_Slash(EnJso2* this, PlayState* play) {
    Vec3f knockbackVelocity;
    f32 curFrame = this->skelAnime.curFrame;

    Math_ApproachZeroF(&this->actor.speed, 0.5f, 5.0f);

    if ((play->gameplayFrames % 8) == 0) {
        Actor_SpawnFloorDustRing(play, &this->actor, &this->actor.world.pos, this->actor.shape.shadowScale, 1, 8.0f,
                                 500, 10, true);
    }

    if ((this->rightSwordCollider.base.atFlags & AT_HIT) || (this->leftSwordCollider.base.atFlags & AT_HIT)) {
        this->slashHitSomething = true;
        this->rightSwordCollider.base.atFlags &= ~AT_HIT;
        this->leftSwordCollider.base.atFlags &= ~AT_HIT;
    }

    if ((this->rightSwordCollider.base.atFlags & AT_BOUNCED) || (this->leftSwordCollider.base.atFlags & AT_BOUNCED)) {
        this->rightSwordCollider.base.atFlags &= ~(AT_HIT | AT_BOUNCED);
        this->leftSwordCollider.base.atFlags &= ~(AT_HIT | AT_BOUNCED);
        Matrix_RotateYS(this->actor.yawTowardsPlayer, MTXMODE_NEW);
        Matrix_MultVecZ(-10.0f, &knockbackVelocity);
        Math_Vec3f_Copy(&this->knockbackVelocity, &knockbackVelocity);
        this->disableBlure = true;
        EnJso2_SetupJumpBack(this);
        return;
    }

    if (curFrame >= this->animEndFrame) {
        this->actor.speed = 0.0f;
        this->disableBlure = true;
        EnJso2_SetupWaitAfterSlash(this);
    }
}

void EnJso2_SetupSpinAttack(EnJso2* this) {
    EnJso2_ChangeAnim(this, EN_JSO2_ANIM_SPIN_ATTACK);
    this->slashHitSomething = false;
    Actor_PlaySfx(&this->actor, NA_SE_IT_SWORD_SWING_HARD);
    this->disableBlure = false;
    this->attackTimer = 15;
    this->action = EN_JSO2_ACTION_SPIN_ATTACK;
    this->actor.speed = 12.0f;
    this->actionFunc = EnJso2_SpinAttack;
}

/**
 * Spins around rapidly with its swords out. If 15 frames pass, the Garo Master hits the player, or the Garo Master
 * bounces off the player's shield, it will end the spin attack and jump backwards.
 */
void EnJso2_SpinAttack(EnJso2* this, PlayState* play) {
    this->actor.shape.rot.y -= 0x1770;
    Actor_SpawnFloorDustRing(play, &this->actor, &this->actor.world.pos, this->actor.shape.shadowScale, 1, 4.0f, 300, 5,
                             true);
    Math_SmoothStepToS(&this->actor.world.rot.y, this->actor.yawTowardsPlayer, 1, 0xFA0, 0x14);

    if ((this->attackTimer == 0) || (this->rightSwordCollider.base.atFlags & AT_HIT) ||
        (this->rightSwordCollider.base.atFlags & AT_BOUNCED) || (this->leftSwordCollider.base.atFlags & AT_HIT) ||
        (this->leftSwordCollider.base.atFlags & AT_BOUNCED)) {
        this->rightSwordCollider.base.atFlags &= ~(AT_HIT | AT_BOUNCED);
        this->leftSwordCollider.base.atFlags &= ~(AT_HIT | AT_BOUNCED);
        EnJso2_SetupJumpBack(this);
    }
}

void EnJso2_SetupWaitAfterSlash(EnJso2* this) {
    if (this->slashHitSomething) {
        EnJso2_ChangeAnim(this, EN_JSO2_ANIM_SLASH_LOOP);
        this->timer = 20;
    } else {
        EnJso2_ChangeAnim(this, EN_JSO2_ANIM_LOOK_AROUND);
        this->timer = 40;
    }

    this->bodyCollider.base.acFlags &= ~AC_HARD;
    this->action = EN_JSO2_ACTION_WAIT_AFTER_SLASH;
    this->actionFunc = EnJso2_WaitAfterSlash;
}

/**
 * Waits either 20 or 40 frames, depending on whether or not the Garo Master hit the player with its slash attack. If
 * the slash didn't hit anything, the Garo Master will play its looking around animation; otherwise, it will just hold
 * its swords out. Once this waiting period ends, the Garo Master starts circling the player again.
 */
void EnJso2_WaitAfterSlash(EnJso2* this, PlayState* play) {
    if (this->timer == 0) {
        this->attackTimer = Rand_S16Offset(30, 30);
        this->slashHitSomething = false;
        this->bodyCollider.base.acFlags |= AC_HARD;
        EnJso2_SetupCirclePlayer(this, play);
    }
}

/**
 * Unlike most enemies, this will make the Garo Master play its damaged animation rather than stopping all animations.
 */
void EnJso2_SetupStunned(EnJso2* this) {
    Vec3f knockbackVelocity;

    AudioSfx_SetChannelIO(&this->actor.projectedPos, NA_SE_EN_ANSATSUSYA_DASH_2, 0);
    EnJso2_ChangeAnim(this, EN_JSO2_ANIM_DAMAGED);
    this->timer = 30;
    this->bodyCollider.base.acFlags &= ~AC_HARD;
    this->actor.speed = 0.0f;
    Matrix_RotateYS(this->actor.yawTowardsPlayer, MTXMODE_NEW);
    Matrix_MultVecZ(-10.0f, &knockbackVelocity);
    Math_Vec3f_Copy(&this->knockbackVelocity, &knockbackVelocity);

    if (((this->drawDmgEffType == ACTOR_DRAW_DMGEFF_FROZEN_SFX) ||
         (this->drawDmgEffType == ACTOR_DRAW_DMGEFF_FROZEN_NO_SFX)) &&
        (this->drawDmgEffAlpha == 0)) {
        this->drawDmgEffAlpha = 0;
        this->drawDmgEffType = ACTOR_DRAW_DMGEFF_FIRE;
    }

    if ((this->drawDmgEffType != ACTOR_DRAW_DMGEFF_FROZEN_SFX) &&
        (this->drawDmgEffType != ACTOR_DRAW_DMGEFF_FROZEN_NO_SFX)) {
        this->timer = 40;
    }

    this->action = EN_JSO2_ACTION_STUNNED;
    this->actionFunc = EnJso2_Stunned;
}

/**
 * Makes the Garo Master play its damaged animation and stop moving until 40 frames have passed, then it jumps back.
 */
void EnJso2_Stunned(EnJso2* this, PlayState* play) {
    if (this->drawDmgEffType == ACTOR_DRAW_DMGEFF_FROZEN_SFX) {
        if ((this->drawDmgEffAlpha != 0) && (this->drawDmgEffAlpha < 60)) {
            this->drawDmgEffType = ACTOR_DRAW_DMGEFF_FROZEN_NO_SFX;
        }
    }

    if ((this->timer == 0) && (this->drawDmgEffAlpha == 0)) {
        if ((this->drawDmgEffType == ACTOR_DRAW_DMGEFF_FROZEN_SFX) ||
            (this->drawDmgEffType == ACTOR_DRAW_DMGEFF_FROZEN_NO_SFX)) {
            Actor_SpawnIceEffects(play, &this->actor, this->bodyPartsPos, EN_JSO2_BODYPART_MAX, 2, 0.7f, 0.4f);
            this->drawDmgEffAlpha = 0;
            this->drawDmgEffType = ACTOR_DRAW_DMGEFF_FIRE;
        }

        EnJso2_SetupJumpBack(this);
    }
}

void EnJso2_SetupDamaged(EnJso2* this, PlayState* play) {
    Vec3f knockbackVelocity;

    AudioSfx_SetChannelIO(&this->actor.projectedPos, NA_SE_EN_ANSATSUSYA_DASH_2, 0);
    EnJso2_ChangeAnim(this, EN_JSO2_ANIM_DAMAGED);
    this->slashHitSomething = false;
    this->actor.velocity.y = 10.0f;
    this->actor.speed = 0.0f;
    Matrix_RotateYS(this->actor.yawTowardsPlayer, MTXMODE_NEW);
    Matrix_MultVecZ(-20.0f, &knockbackVelocity);
    Math_Vec3f_Copy(&this->knockbackVelocity, &knockbackVelocity);

    if (((this->drawDmgEffType == ACTOR_DRAW_DMGEFF_FROZEN_SFX) ||
         (this->drawDmgEffType == ACTOR_DRAW_DMGEFF_FROZEN_NO_SFX)) &&
        (this->drawDmgEffAlpha != 0)) {
        Actor_SpawnIceEffects(play, &this->actor, this->bodyPartsPos, EN_JSO2_BODYPART_MAX, 2, 0.7f, 0.4f);
        this->drawDmgEffAlpha = 0;
        this->drawDmgEffType = ACTOR_DRAW_DMGEFF_FIRE;
    }

    CollisionCheck_GreenBlood(play, NULL, &this->actor.focus.pos);
    CollisionCheck_GreenBlood(play, NULL, &this->actor.focus.pos);
    CollisionCheck_GreenBlood(play, NULL, &this->actor.focus.pos);
    Actor_SetColorFilter(&this->actor, COLORFILTER_COLORFLAG_RED, 255, COLORFILTER_BUFFLAG_OPA, 8);
    Actor_PlaySfx(&this->actor, NA_SE_EN_ANSATSUSYA_DAMAGE);
    this->action = EN_JSO2_ACTION_DAMAGED;
    this->actionFunc = EnJso2_Damaged;
}

/**
 * Plays the damaged animation until the Garo Master is touching the ground and no longer has its red color filter, at
 * which point it will perform a spin attack.
 */
void EnJso2_Damaged(EnJso2* this, PlayState* play) {
    if (this->actor.velocity.y > 0.0f) {
        return;
    }

    if ((this->actor.colorFilterTimer == 0) && (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND)) {
        EnJso2_SetupSpinAttack(this);
    }
}

void EnJso2_SetupJumpBack(EnJso2* this) {
    EnJso2_ChangeAnim(this, EN_JSO2_ANIM_JUMP_BACK);
    this->actor.world.rot.y = -this->actor.world.rot.y;
    this->isTeleporting = false;
    this->action = EN_JSO2_ACTION_JUMP_BACK;
    this->actor.speed = 7.0f;
    this->actor.velocity.y = 20.0f;
    this->actionFunc = EnJso2_JumpBack;
}

/**
 * Jump backwards away from the player. After the Garo Master touches the ground and the jump back animation completes,
 * the Garo Master will transition to circling the player.
 */
void EnJso2_JumpBack(EnJso2* this, PlayState* play) {
    f32 curFrame = this->skelAnime.curFrame;

    Math_SmoothStepToS(&this->actor.shape.rot.y, this->actor.yawTowardsPlayer, 0xA, 0xBB8, 0x14);

    if (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND) {
        this->actor.speed = 0.0f;
    }

    if ((curFrame >= this->animEndFrame) && (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND)) {
        this->actor.world.rot.x = 0;
        this->actor.velocity.y = 0.0f;
        this->actor.speed = 0.0f;
        this->actor.world.rot.y = this->actor.shape.rot.y;
        this->attackTimer = Rand_S16Offset(10, 10);
        EnJso2_SetupCirclePlayer(this, play);
    }
}

void EnJso2_SetupDead(EnJso2* this, PlayState* play) {
    AudioSfx_SetChannelIO(&this->actor.projectedPos, NA_SE_EN_ANSATSUSYA_DASH_2, 0);
    EnJso2_ChangeAnim(this, EN_JSO2_ANIM_DAMAGED);

    if (((this->drawDmgEffType == ACTOR_DRAW_DMGEFF_FROZEN_SFX) ||
         (this->drawDmgEffType == ACTOR_DRAW_DMGEFF_FROZEN_NO_SFX)) &&
        (this->drawDmgEffAlpha == 0)) {
        this->drawDmgEffType = ACTOR_DRAW_DMGEFF_FIRE;
    }

    this->actor.flags |= ACTOR_FLAG_LOCK_ON_DISABLED;
    this->actor.flags &= ~(ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_HOSTILE);
    this->actor.speed = 0.0f;
    this->disableBlure = true;
    this->timer = 30;
    this->swordState = EN_JSO2_SWORD_STATE_NONE_DRAWN;
    this->actor.world.rot.x = this->actor.shape.rot.x = this->actor.world.rot.z = this->actor.shape.rot.z = 0;
    Enemy_StartFinishingBlow(play, &this->actor);
    Actor_PlaySfx(&this->actor, NA_SE_EN_ANSATSUSYA_DEAD);
    Math_Vec3f_Copy(&this->knockbackVelocity, &gZeroVec3f);
    this->action = EN_JSO2_ACTION_DEAD;
    this->actionFunc = EnJso2_Dead;
}

/**
 * Plays the damaged animation for 30 frames, then transitions the Garo Master to collapsing onto one knee and blowing
 * itself up. Whether or not this happens in a cutscene or in gameplay depends on the Garo Master's type.
 */
void EnJso2_Dead(EnJso2* this, PlayState* play) {
    Math_SmoothStepToS(&this->actor.world.rot.y, this->actor.yawTowardsPlayer, 0xA, 0xFA0, 0x14);

    if ((this->drawDmgEffType == ACTOR_DRAW_DMGEFF_FROZEN_SFX) ||
        (this->drawDmgEffType == ACTOR_DRAW_DMGEFF_FROZEN_NO_SFX)) {
        if (this->drawDmgEffAlpha != 0) {
            Actor_SpawnIceEffects(play, &this->actor, this->bodyPartsPos, EN_JSO2_BODYPART_MAX, 2, 0.7f, 0.4f);
            this->drawDmgEffAlpha = 0;
            this->drawDmgEffType = ACTOR_DRAW_DMGEFF_FIRE;
        } else {
            return;
        }
    }

    if (this->timer == 0) {
        this->subCamFov = this->subCamFovNext = 60.0f;
        if (this->type == EN_JSO2_TYPE_LIGHT_ARROW_ROOM) {
            EnJso2_SetupDeathCutscene(this);
        } else {
            EnJso2_SetupBlowUp(this);
        }
    }
}

void EnJso2_SetupDeathCutscene(EnJso2* this) {
    this->cutsceneState = EN_JSO2_DEATH_CS_STATE_STARTED;
    this->cutsceneTimer = 0;
    this->subCamId = SUB_CAM_ID_DONE;
    this->actor.flags |= ACTOR_FLAG_FREEZE_EXCEPTION;
    this->timer = 30;
    this->action = EN_JSO2_ACTION_BLOW_UP;
    this->actionFunc = EnJso2_DeathCutscene;
}

/**
 * Responsible for handling all aspects of the intro cutscene, including making the Garo Master take out a bomb,
 * displaying the messages that give the player a hint on how to use the Light Arrow, detonating the bomb and making the
 * Garo Master fade away, etc.
 */
void EnJso2_DeathCutscene(EnJso2* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    f32 curFrame = this->skelAnime.curFrame;
    Vec3f subCamEyeNextOffset;

    if ((this->animIndex == EN_JSO2_TAKE_OUT_BOMB) && (curFrame >= this->animEndFrame) && (this->bomb == NULL)) {
        this->bomb = Actor_SpawnAsChild(&play->actorCtx, &this->actor, play, ACTOR_EN_COL_MAN, this->bombPos.x,
                                        this->bombPos.y, this->bombPos.z, 0, 0, 0, EN_COL_MAN_CUTSCENE_BOMB);
    }

    if (this->bomb != NULL) {
        this->bomb->world.pos.x = this->bombPos.x;
        this->bomb->world.pos.y = this->bombPos.y;
        this->bomb->world.pos.z = this->bombPos.z;
    }

    switch (this->cutsceneState) {
        case EN_JSO2_DEATH_CS_STATE_STARTED:
            if (!CutsceneManager_IsNext(this->actor.csId)) {
                CutsceneManager_Queue(this->actor.csId);
            } else {
                CutsceneManager_StartWithPlayerCs(this->actor.csId, &this->actor);
                Player_SetCsActionWithHaltedActors(play, &this->actor, PLAYER_CSACTION_WAIT);
                this->actor.world.rot.y = this->actor.shape.rot.y =
                    Math_Vec3f_Yaw(&this->actor.world.pos, &this->actor.home.pos);
                Audio_SetMainBgmVolume(0, 10);
                EnJso2_ChangeAnim(this, EN_JSO2_COLLAPSE);
                this->subCamId = CutsceneManager_GetCurrentSubCamId(this->actor.csId);
                Player_SetCsActionWithHaltedActors(play, &this->actor, PLAYER_CSACTION_WAIT);
                this->subCamMaxVelocityFrac = 0.4f;
                this->subCamVelocity = 40.0f;
                this->cutsceneState++;
            }
            break;

        case EN_JSO2_DEATH_CS_STATE_COLLAPSE:
            if (curFrame >= this->animEndFrame) {
                EnJso2_ChangeAnim(this, EN_JSO2_TREMBLE);
                this->cutsceneState++;
            }
            break;

        case EN_JSO2_DEATH_CS_STATE_TREMBLE:
            if (curFrame >= this->animEndFrame) {
                this->actor.textId = 0x13AE;
                Message_StartTextbox(play, this->actor.textId, &this->actor);
                this->cutsceneState++;
            }
            break;

        case EN_JSO2_DEATH_CS_STATE_TELL_HINT:
            if (EnJso2_ShouldAdvanceMessage(play)) {
                if (this->actor.textId == 0x13AE) {
                    this->actor.textId = 0x13AF;
                } else if (this->actor.textId == 0x13AF) {
                    this->actor.textId = 0x13B0;
                    EnJso2_ChangeAnim(this, EN_JSO2_TAKE_OUT_BOMB);
                } else if (this->actor.textId == 0x13B0) {
                    play->msgCtx.msgLength = 0;
                    if (this->bomb != NULL) {
                        this->bomb->world.rot.z = 1;
                        Player_SetCsActionWithHaltedActors(play, &this->actor, PLAYER_CSACTION_47);
                        this->isFadingAway = true;
                    }

                    this->cutsceneTimer = 30;
                    this->cutsceneState++;
                    break;
                }

                Message_ContinueTextbox(play, this->actor.textId);
            }
            break;

        case EN_JSO2_DEATH_CS_STATE_BLOW_UP_AND_FADE_AWAY:
            Math_SmoothStepToS(&this->alpha, 0, 1, 15, 50);
            Math_ApproachZeroF(&this->actor.shape.shadowScale, 0.3f, 3.0f);
            if (this->cutsceneTimer == 0) {
                this->actor.textId = 0x13B1;
                Message_StartTextbox(play, this->actor.textId, &this->actor);
                Actor_PlaySfx(&this->actor, NA_SE_EN_ANSATSUSYA_LAUGH);
                this->timer = 50;
                this->cutsceneState++;
            }
            break;

        case EN_JSO2_DEATH_CS_STATE_WAIT_AND_END:
            Player_SetCsActionWithHaltedActors(play, &this->actor, PLAYER_CSACTION_WAIT);
            if (this->timer == 0) {
                CutsceneManager_Stop(this->actor.csId);
                Player_SetCsActionWithHaltedActors(play, &this->actor, PLAYER_CSACTION_END);
                Actor_Kill(&this->actor);
            }
            break;

        default:
            break;
    }

    if (this->subCamId != SUB_CAM_ID_DONE) {
        player->actor.world.pos.x = (Math_SinS(this->actor.world.rot.y) * 170.0f) + this->actor.world.pos.x;
        player->actor.world.pos.z = (Math_CosS(this->actor.world.rot.y) * 170.0f) + this->actor.world.pos.z;
        player->actor.world.rot.y = player->actor.shape.rot.y = this->actor.world.rot.y + 0x8000;
        Matrix_RotateYS(((BREG(49) * 0x100) + this->actor.shape.rot.y + 0x1000), MTXMODE_NEW);
        Matrix_MultVecZ(BREG(48) + 230.0f, &subCamEyeNextOffset);
        this->subCamEyeNext.x = this->actor.world.pos.x + subCamEyeNextOffset.x;
        this->subCamEyeNext.y = BREG(50) + -43.0f + this->actor.world.pos.y + 50.0f;
        this->subCamEyeNext.z = this->actor.world.pos.z + subCamEyeNextOffset.z;
        this->subCamAtNext.x = F32_LERPIMP(player->actor.world.pos.x, this->actor.world.pos.x, 0.5f);
        this->subCamAtNext.y = BREG(51) + 6.0f + player->actor.world.pos.y + 5.0f;
        this->subCamAtNext.z = F32_LERPIMP(player->actor.world.pos.z, this->actor.world.pos.z, 0.5f);
    }

    EnJso2_UpdateSubCam(this, play);
}

void EnJso2_SetupBlowUp(EnJso2* this) {
    this->cutsceneTimer = 0;
    Audio_SetMainBgmVolume(0, 10);
    EnJso2_ChangeAnim(this, EN_JSO2_COLLAPSE);
    this->action = EN_JSO2_ACTION_BLOW_UP;
    this->actionFunc = EnJso2_BlowUp;
}

/**
 * This function makes the Garo Master collapse onto one knee, take out a bomb, and blow itself up. It is roughly
 * analogous to part of the death cutscene, however all of the Garo Master's actions happen during regular gameplay, and
 * the bomb can even damage the player if they're too close when it detonates. After the Garo Master blows up, this
 * function will make it start fading away.
 */
void EnJso2_BlowUp(EnJso2* this, PlayState* play) {
    f32 curFrame = this->skelAnime.curFrame;

    Math_SmoothStepToS(&this->actor.shape.rot.y, this->actor.yawTowardsPlayer, 0xA, 0xFA0, 0x14);

    if ((this->animIndex == EN_JSO2_COLLAPSE) && (curFrame >= this->animEndFrame)) {
        this->blowUpFrameCounter = 0;
        EnJso2_ChangeAnim(this, EN_JSO2_TREMBLE);
    }

    if ((this->animIndex == EN_JSO2_TREMBLE) && (curFrame >= this->animEndFrame)) {
        this->blowUpFrameCounter++;
        if (this->blowUpFrameCounter >= 2) {
            this->blowUpFrameCounter = 0;
            EnJso2_ChangeAnim(this, EN_JSO2_TAKE_OUT_BOMB);
        }
    }

    if ((this->animIndex == EN_JSO2_TAKE_OUT_BOMB) && (curFrame >= this->animEndFrame)) {
        if (this->bomb == NULL) {
            this->bomb = Actor_SpawnAsChild(&play->actorCtx, &this->actor, play, ACTOR_EN_COL_MAN, this->bombPos.x,
                                            this->bombPos.y, this->bombPos.z, 0, 0, 0, EN_COL_MAN_GAMEPLAY_BOMB);
        } else if (this->blowUpFrameCounter >= 10) {
            if (this->bomb != NULL) {
                this->bomb->world.rot.z = 1;
                this->isFadingAway = true;
                this->actionFunc = EnJso2_FadeAway;
                return;
            }
        } else {
            this->blowUpFrameCounter++;
        }
    }

    if (this->bomb != NULL) {
        this->bomb->world.pos.x = this->bombPos.x;
        this->bomb->world.pos.y = this->bombPos.y;
        this->bomb->world.pos.z = this->bombPos.z;
    }

    CollisionCheck_SetOC(play, &play->colChkCtx, &this->bodyCollider.base);
}

/**
 * Slowly reduces the Garo Master's alpha until it almost reaches zero, then kills the Garo Master actor.
 */
void EnJso2_FadeAway(EnJso2* this, PlayState* play) {
    Math_SmoothStepToS(&this->alpha, 0, 1, 15, 50);
    Math_ApproachZeroF(&this->actor.shape.shadowScale, 0.3f, 3.0f);

    if (this->alpha < 2) {
        Actor_Kill(&this->actor);
    }
}

void EnJso2_UpdateDamage(EnJso2* this, PlayState* play) {
    s32 attackDealsDamage = false;

    if ((this->action != EN_JSO2_ACTION_DAMAGED) && (this->action != EN_JSO2_ACTION_JUMP_BACK) &&
        (this->action != EN_JSO2_ACTION_DEAD) && (this->action != EN_JSO2_ACTION_BLOW_UP) &&
        (this->bodyCollider.base.acFlags & AC_HIT)) {
        this->bodyCollider.base.acFlags &= ~AC_HIT;
        if ((this->actor.colChkInfo.damageEffect == EN_JSO2_DMGEFF_STUN) ||
            (this->actor.colChkInfo.damageEffect == EN_JSO2_DMGEFF_ELECTRIC_STUN)) {
            this->actor.world.rot.x = this->actor.shape.rot.x = 0;

            if (((this->drawDmgEffType != ACTOR_DRAW_DMGEFF_FROZEN_SFX) &&
                 (this->drawDmgEffType != ACTOR_DRAW_DMGEFF_FROZEN_NO_SFX)) ||
                (this->drawDmgEffAlpha == 0)) {
                Actor_PlaySfx(&this->actor, NA_SE_EN_COMMON_FREEZE);
                if (this->actor.colChkInfo.damageEffect == EN_JSO2_DMGEFF_ELECTRIC_STUN) {
                    this->drawDmgEffAlpha = 40;
                    this->drawDmgEffType = ACTOR_DRAW_DMGEFF_ELECTRIC_SPARKS_LARGE;
                }

                Actor_SetColorFilter(&this->actor, COLORFILTER_COLORFLAG_BLUE, 255, COLORFILTER_BUFFLAG_OPA, 40);
                EnJso2_SetupStunned(this);
            }
        } else {
            switch (this->action) {
                case EN_JSO2_ACTION_UNK_2:
                case EN_JSO2_ACTION_CIRCLE_PLAYER:
                case EN_JSO2_ACTION_GUARD:
                    this->actor.speed = 0.0f;
                    EnJso2_SetupGuard(this);
                    attackDealsDamage = false;
                    break;

                case EN_JSO2_ACTION_SLASH:
                case EN_JSO2_ACTION_WAIT_AFTER_SLASH:
                case EN_JSO2_ACTION_STUNNED:
                    switch (this->actor.colChkInfo.damageEffect) {
                        case EN_JSO2_DMGEFF_NONE:
                            attackDealsDamage = true;
                            break;

                        case EN_JSO2_DMGEFF_FIRE:
                            this->drawDmgEffAlpha = 40;
                            this->drawDmgEffType = ACTOR_DRAW_DMGEFF_FIRE;
                            attackDealsDamage = true;
                            break;

                        case EN_JSO2_DMGEFF_LIGHT_ORB:
                            if (((this->drawDmgEffType != ACTOR_DRAW_DMGEFF_FROZEN_SFX) &&
                                 (this->drawDmgEffType != ACTOR_DRAW_DMGEFF_FROZEN_NO_SFX)) ||
                                (this->drawDmgEffAlpha == 0)) {
                                Actor_Spawn(&play->actorCtx, play, ACTOR_EN_CLEAR_TAG, this->actor.focus.pos.x,
                                            this->actor.focus.pos.y, this->actor.focus.pos.z, 0, 0, 0,
                                            CLEAR_TAG_PARAMS(CLEAR_TAG_LARGE_LIGHT_RAYS));
                                this->drawDmgEffAlpha = 20;
                                this->drawDmgEffType = ACTOR_DRAW_DMGEFF_LIGHT_ORBS;
                                attackDealsDamage = true;
                            }
                            break;

                        case EN_JSO2_DMGEFF_FREEZE:
                            if (((this->drawDmgEffType != ACTOR_DRAW_DMGEFF_FROZEN_SFX) &&
                                 (this->drawDmgEffType != ACTOR_DRAW_DMGEFF_FROZEN_NO_SFX)) ||
                                (this->drawDmgEffAlpha == 0)) {
                                attackDealsDamage = false;
                                Actor_ApplyDamage(&this->actor);
                                this->drawDmgEffAlpha = 80;
                                this->drawDmgEffType = ACTOR_DRAW_DMGEFF_FROZEN_SFX;
                                this->drawDmgEffScale = 0.0f;
                                this->drawDmgEffFrozenSteamScale = 1.5f;
                            }

                            if (this->actor.colChkInfo.health <= 0) {
                                EnJso2_SetupDead(this, play);
                                attackDealsDamage = false;
                            } else {
                                EnJso2_SetupStunned(this);
                                attackDealsDamage = false;
                            }
                            break;

                        default:
                            break;
                    }
                    break;

                default:
                    break;
            }

            if (attackDealsDamage) {
                Actor_ApplyDamage(&this->actor);
                if (this->actor.colChkInfo.health > 0) {
                    EnJso2_SetupDamaged(this, play);
                } else {
                    EnJso2_SetupDead(this, play);
                }
            }
        }
    }
}

void EnJso2_Update(Actor* thisx, PlayState* play) {
    EnJso2* this = (EnJso2*)thisx;
    s32 pad;
    s32 i;

    if (this->action != EN_JSO2_ACTION_CIRCLE_PLAYER) {
        SkelAnime_Update(&this->skelAnime);
    }

    DECR(this->attackMovementTimer);
    DECR(this->attackTimer);
    DECR(this->timer);
    DECR(this->cutsceneTimer);
    DECR(this->drawDmgEffAlpha);

    EnJso2_UpdateDamage(this, play);
    Actor_SetScale(&this->actor, this->scale);
    this->actionFunc(this, play);
    Actor_SetFocus(&this->actor, 80.0f);
    Actor_MoveWithGravity(&this->actor);

    if (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND) {
        this->actor.world.pos.x += this->knockbackVelocity.x;
        this->actor.world.pos.z += this->knockbackVelocity.z;
        Math_ApproachZeroF(&this->knockbackVelocity.x, 1.0f, 2.0f);
        Math_ApproachZeroF(&this->knockbackVelocity.z, 1.0f, 2.0f);
    }

    Actor_UpdateBgCheckInfo(play, &this->actor, 35.0f, 60.0f, 60.0f,
                            UPDBGCHECKINFO_FLAG_1 | UPDBGCHECKINFO_FLAG_4 | UPDBGCHECKINFO_FLAG_8 |
                                UPDBGCHECKINFO_FLAG_10);

    if ((this->action == EN_JSO2_ACTION_SPIN_BEFORE_ATTACK) || (this->action == EN_JSO2_ACTION_DASH_ATTACK) ||
        (this->action == EN_JSO2_ACTION_TELEPORT) || (this->action == EN_JSO2_ACTION_FALL_FROM_TELEPORT)) {
        this->afterimageIndex++;
        if (this->afterimageIndex >= EN_JSO2_AFTERIMAGE_COUNT) {
            this->afterimageIndex = 0;
        }

        if (this->afterimageCount < EN_JSO2_AFTERIMAGE_COUNT - 1) {
            this->afterimageCount++;
        }

        Math_Vec3f_Copy(&this->afterimagePos[this->afterimageIndex], &this->actor.world.pos);
        Math_Vec3s_Copy(&this->afterimageRot[this->afterimageIndex], &this->actor.world.rot);
        this->afterimagePos[this->afterimageIndex].y += 40.0f;

        for (i = 0; i < EN_JSO2_AFTERIMAGE_COUNT; i++) {
            this->afterimageJointTable[this->afterimageIndex][i] = this->jointTable[i];
        }
    } else if (this->action != EN_JSO2_ACTION_INTRO_CUTSCENE) {
        this->afterimageCount = 0;
    }

    if ((this->action != EN_JSO2_ACTION_CIRCLE_PLAYER) && (this->action != EN_JSO2_ACTION_SPIN_BEFORE_ATTACK) &&
        (this->action != EN_JSO2_ACTION_DAMAGED) && (this->action != EN_JSO2_ACTION_SPIN_ATTACK) &&
        (this->action != EN_JSO2_ACTION_TELEPORT) && (this->action != EN_JSO2_ACTION_JUMP_BACK)) {
        this->actor.shape.rot.y = this->actor.world.rot.y;
    }

    this->actor.shape.rot.x = this->actor.world.rot.x;
    Collider_UpdateCylinder(&this->actor, &this->bodyCollider);

    if ((this->action != EN_JSO2_ACTION_INTRO_CUTSCENE) && (this->action != EN_JSO2_ACTION_SPIN_ATTACK) &&
        (this->action != EN_JSO2_ACTION_TELEPORT)) {
        CollisionCheck_SetOC(play, &play->colChkCtx, &this->bodyCollider.base);
        if ((this->action != EN_JSO2_ACTION_UNK_1) && (this->action != EN_JSO2_ACTION_SPIN_ATTACK)) {
            CollisionCheck_SetAC(play, &play->colChkCtx, &this->bodyCollider.base);
        }
    }

    if (((this->action == EN_JSO2_ACTION_SLASH) || (this->action == EN_JSO2_ACTION_FALL_FROM_TELEPORT) ||
         (this->action == EN_JSO2_ACTION_DASH_ATTACK) || (this->action == EN_JSO2_ACTION_SPIN_ATTACK)) &&
        !this->slashHitSomething && (this->swordState == EN_JSO2_SWORD_STATE_BOTH_DRAWN)) {
        CollisionCheck_SetAT(play, &play->colChkCtx, &this->rightSwordCollider.base);
        CollisionCheck_SetAT(play, &play->colChkCtx, &this->leftSwordCollider.base);
    }
}

s32 EnJso2_OverrideLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, Actor* thisx) {
    EnJso2* this = (EnJso2*)thisx;

    if (this->swordState == EN_JSO2_SWORD_STATE_NONE_DRAWN) {
        if ((limbIndex == GARO_MASTER_LIMB_LEFT_SWORD) && (this->action != EN_JSO2_ACTION_BLOW_UP)) {
            *dList = NULL;
        }

        if (limbIndex == GARO_MASTER_LIMB_RIGHT_SWORD) {
            *dList = NULL;
        }
    }

    return false;
}

void EnJso2_PostLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot, Actor* thisx) {
    static Vec3f sSwordTipOffset = { 1600.0f, 0.0f, 0.0f };
    static Vec3f sSwordBaseOffset = { 0.0f, 0.0f, 0.0f };
    static Vec3f sSwordTipQuadOffset = { 1700.0f, 0.0f, 0.0f };
    static Vec3f sSwordBaseQuadOffset = { 0.0f, 0.0f, 0.0f };
    EnJso2* this = (EnJso2*)thisx;
    Vec3f swordTipPos;
    Vec3f swordBasePos;
    Vec3f bombOffset = { 0.0f, 0.0f, 0.0f };

    if (limbIndex == GARO_MASTER_LIMB_LEFT_SWORD) {
        Matrix_Translate(0.0f, 0.0f, 0.0f, MTXMODE_APPLY);
        Math_Vec3f_Copy(&this->leftSwordCollider.dim.quad[3], &this->leftSwordCollider.dim.quad[1]);
        Math_Vec3f_Copy(&this->leftSwordCollider.dim.quad[2], &this->leftSwordCollider.dim.quad[0]);
        Matrix_MultVec3f(&sSwordTipQuadOffset, &this->leftSwordCollider.dim.quad[1]);
        Matrix_MultVec3f(&sSwordBaseQuadOffset, &this->leftSwordCollider.dim.quad[0]);
        Collider_SetQuadVertices(&this->leftSwordCollider, &this->leftSwordCollider.dim.quad[0],
                                 &this->leftSwordCollider.dim.quad[1], &this->leftSwordCollider.dim.quad[2],
                                 &this->leftSwordCollider.dim.quad[3]);
        Matrix_MultVec3f(&sSwordTipOffset, &swordTipPos);
        Matrix_MultVec3f(&sSwordBaseOffset, &swordBasePos);

        if ((this->action == EN_JSO2_ACTION_SLASH) || (this->action == EN_JSO2_ACTION_WAIT_AFTER_SLASH)) {
            Matrix_MultVec3f(&sSlashFlameOffsets[0], &this->flamePos[0]);
            Matrix_MultVec3f(&sSlashFlameOffsets[1], &this->flamePos[1]);
            Matrix_MultVec3f(&sSlashFlameOffsets[2], &this->flamePos[2]);
        } else {
            Matrix_MultVec3f(&sFlameOffsets[0], &this->flamePos[0]);
            Matrix_MultVec3f(&sFlameOffsets[1], &this->flamePos[1]);
            Matrix_MultVec3f(&sFlameOffsets[2], &this->flamePos[2]);
        }

        if (((this->action == EN_JSO2_ACTION_SLASH) || (this->action == EN_JSO2_ACTION_SPIN_ATTACK) ||
             (this->action == EN_JSO2_ACTION_DASH_ATTACK) || (this->action == EN_JSO2_ACTION_FALL_FROM_TELEPORT)) &&
            !this->disableBlure) {
            EffectBlure_AddVertex(Effect_GetByIndex(this->leftSwordBlureIndex), &swordTipPos, &swordBasePos);
        } else if (this->disableBlure == true) {
            EffectBlure_AddSpace(Effect_GetByIndex(this->leftSwordBlureIndex));
        }
    }

    if (limbIndex == GARO_MASTER_LIMB_RIGHT_SWORD) {
        Matrix_Translate(0.0f, 0.0f, 0.0f, MTXMODE_APPLY);
        Math_Vec3f_Copy(&this->rightSwordCollider.dim.quad[3], &this->rightSwordCollider.dim.quad[1]);
        Math_Vec3f_Copy(&this->rightSwordCollider.dim.quad[2], &this->rightSwordCollider.dim.quad[0]);
        Matrix_MultVec3f(&sSwordTipQuadOffset, &this->rightSwordCollider.dim.quad[1]);
        Matrix_MultVec3f(&sSwordBaseQuadOffset, &this->rightSwordCollider.dim.quad[0]);
        Collider_SetQuadVertices(&this->rightSwordCollider, &this->rightSwordCollider.dim.quad[0],
                                 &this->rightSwordCollider.dim.quad[1], &this->rightSwordCollider.dim.quad[2],
                                 &this->rightSwordCollider.dim.quad[3]);
        Matrix_MultVec3f(&sSwordTipOffset, &swordTipPos);
        Matrix_MultVec3f(&sSwordBaseOffset, &swordBasePos);

        if ((this->action == EN_JSO2_ACTION_SLASH) || (this->action == EN_JSO2_ACTION_WAIT_AFTER_SLASH)) {
            Matrix_MultVec3f(&sSlashFlameOffsets[3], &this->flamePos[3]);
            Matrix_MultVec3f(&sSlashFlameOffsets[4], &this->flamePos[4]);
            Matrix_MultVec3f(&sSlashFlameOffsets[5], &this->flamePos[5]);
        } else {
            Matrix_MultVec3f(&sFlameOffsets[3], &this->flamePos[3]);
            Matrix_MultVec3f(&sFlameOffsets[4], &this->flamePos[4]);
            Matrix_MultVec3f(&sFlameOffsets[5], &this->flamePos[5]);
        }

        if (((this->action == EN_JSO2_ACTION_SLASH) || (this->action == EN_JSO2_ACTION_SPIN_ATTACK) ||
             (this->action == EN_JSO2_ACTION_DASH_ATTACK) || (this->action == EN_JSO2_ACTION_FALL_FROM_TELEPORT)) &&
            !this->disableBlure) {
            EffectBlure_AddVertex(Effect_GetByIndex(this->rightSwordBlureIndex), &swordTipPos, &swordBasePos);
        } else if (this->disableBlure == true) {
            EffectBlure_AddSpace(Effect_GetByIndex(this->rightSwordBlureIndex));
            this->disableBlure = false;
        }
    }

    if (limbIndex == GARO_MASTER_LIMB_ROBE_RIGHT) {
        bombOffset.x = 900.0f;
        bombOffset.y = 50.0f;
        bombOffset.z = -330.0f;
        Matrix_MultVec3f(&bombOffset, &this->bombPos);
    }

    if ((this->action != EN_JSO2_ACTION_BLOW_UP) &&
        ((limbIndex == GARO_MASTER_LIMB_LEFT_SWORD) || (limbIndex == GARO_MASTER_LIMB_RIGHT_SWORD) ||
         (limbIndex == GARO_MASTER_LIMB_ROBE_TOP) || (limbIndex == GARO_MASTER_LIMB_ROBE_BACK) ||
         (limbIndex == GARO_MASTER_LIMB_ROBE_LEFT) || (limbIndex == GARO_MASTER_LIMB_ROBE_RIGHT) ||
         (limbIndex == GARO_MASTER_LIMB_ROBE_FRONT) || (limbIndex == GARO_MASTER_LIMB_HEAD) ||
         (limbIndex == GARO_MASTER_LIMB_RIGHT_THIGH) || (limbIndex == GARO_MASTER_LIMB_RIGHT_FOOT) ||
         (limbIndex == GARO_MASTER_LIMB_LEFT_THIGH) || (limbIndex == GARO_MASTER_LIMB_LEFT_FOOT))) {
        Matrix_MultZero(&this->bodyPartsPos[this->bodyPartIndex]);
        this->bodyPartIndex++;
        if (this->bodyPartIndex >= EN_JSO2_BODYPART_MAX) {
            this->bodyPartIndex = 0;
        }
    }

    if (limbIndex == GARO_MASTER_LIMB_HEAD) {
        Matrix_Push();
        Matrix_Scale(1.0f, 1.0f, 1.0f, MTXMODE_APPLY);

        OPEN_DISPS(play->state.gfxCtx);

        MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx);
        gSPDisplayList(POLY_OPA_DISP++, gGaroMasterEyesDL);

        CLOSE_DISPS(play->state.gfxCtx);

        Matrix_Pop();
    }
}

void EnJso2_Draw(Actor* thisx, PlayState* play2) {
    static s16 sAfterimageAlpha[EN_JSO2_AFTERIMAGE_COUNT] = {
        128, 0, 0, 0, 0, 128, 0, 0, 0, 0, 128, 0, 0, 0, 0, 128, 0, 0, 0, 0,
    };
    EnJso2* this = (EnJso2*)thisx;
    PlayState* play = play2;

    OPEN_DISPS(play->state.gfxCtx);

    Gfx_SetupDL25_Xlu(play->state.gfxCtx);
    Gfx_SetupDL25_Opa(play->state.gfxCtx);

    if (!this->isFadingAway) {
        Scene_SetRenderModeXlu(play, 0, 1);
        SkelAnime_DrawFlexOpa(play, this->skelAnime.skeleton, this->skelAnime.jointTable, this->skelAnime.dListCount,
                              EnJso2_OverrideLimbDraw, EnJso2_PostLimbDraw, &this->actor);
    } else {
        gDPPipeSync(POLY_XLU_DISP++);
        gDPSetEnvColor(POLY_XLU_DISP++, 0, 0, 0, this->alpha);
        Scene_SetRenderModeXlu(play, 1, 2);
        POLY_XLU_DISP = SkelAnime_DrawFlex(play, this->skelAnime.skeleton, this->skelAnime.jointTable,
                                           this->skelAnime.dListCount, NULL, NULL, &this->actor, POLY_XLU_DISP);
    }

    if (this->afterimageCount > 0) {
        s32 i;
        s32 index = this->afterimageIndex;

        for (i = 0; i < this->afterimageCount; i++) {
            if (sAfterimageAlpha[i] == 0) {
                continue;
            }

            Matrix_Translate(this->afterimagePos[index].x, this->afterimagePos[index].y, this->afterimagePos[index].z,
                             MTXMODE_NEW);
            Matrix_Scale(this->scale, this->scale, this->scale, MTXMODE_APPLY);
            Matrix_RotateYS(this->afterimageRot[index].y, MTXMODE_APPLY);
            Matrix_RotateXS(this->afterimageRot[index].x, MTXMODE_APPLY);
            Matrix_RotateZS(this->afterimageRot[index].z, MTXMODE_APPLY);

            gDPPipeSync(POLY_XLU_DISP++);
            gDPSetEnvColor(POLY_XLU_DISP++, 0, 0, 0, sAfterimageAlpha[i]);
            Scene_SetRenderModeXlu(play, 1, 2);
            POLY_XLU_DISP = SkelAnime_DrawFlex(play, this->skelAnime.skeleton, this->afterimageJointTable[index],
                                               this->skelAnime.dListCount, NULL, NULL, &this->actor, POLY_XLU_DISP);

            index--;
            if (index < 0) {
                index = EN_JSO2_AFTERIMAGE_COUNT - 1;
            }
        }
    }

    if ((this->action < EN_JSO2_ACTION_TELEPORT) && (this->swordState == EN_JSO2_SWORD_STATE_BOTH_DRAWN) &&
        (((this->drawDmgEffType != ACTOR_DRAW_DMGEFF_FROZEN_SFX) &&
          (this->drawDmgEffType != ACTOR_DRAW_DMGEFF_FROZEN_NO_SFX)) ||
         (((this->drawDmgEffType == ACTOR_DRAW_DMGEFF_FROZEN_SFX) ||
           (this->drawDmgEffType == ACTOR_DRAW_DMGEFF_FROZEN_NO_SFX)) &&
          (this->drawDmgEffAlpha == 0)))) {
        s32 i;

        for (i = 0; i < EN_JSO2_FLAME_COUNT; i++) {
            Matrix_Push();
            Matrix_Translate(this->flamePos[i].x, this->flamePos[i].y, this->flamePos[i].z, MTXMODE_NEW);
            Matrix_Scale(this->flameScale[i].x, this->flameScale[i].y, this->flameScale[i].z, MTXMODE_APPLY);
            gSPSegment(POLY_XLU_DISP++, 0x08,
                       Gfx_TwoTexScrollEx(play->state.gfxCtx, G_TX_RENDERTILE, 0, 0, 32, 64, 1, 0,
                                          ((this->flameScroll * 10) - (play->state.frames * 20)) & 0x1FF, 32, 128, 0, 0,
                                          0, -20));
            gDPPipeSync(POLY_XLU_DISP++);
            gDPSetPrimColor(POLY_XLU_DISP++, 0x80, 0x80, 255, 255, 170, 255);
            gDPSetEnvColor(POLY_XLU_DISP++, 255, 50, 0, 255);
            Matrix_Mult(&play->billboardMtxF, MTXMODE_APPLY);
            MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx);
            gSPDisplayList(POLY_XLU_DISP++, gEffFire1DL);
            Matrix_Pop();
        }
    }

    if (this->drawDmgEffAlpha != 0) {
        f32 drawDmgEffAlpha = this->drawDmgEffAlpha * 0.05f;

        if ((this->drawDmgEffType == ACTOR_DRAW_DMGEFF_FROZEN_SFX) ||
            (this->drawDmgEffType == ACTOR_DRAW_DMGEFF_FROZEN_NO_SFX)) {
            this->drawDmgEffScale += 0.3f;
            if (this->drawDmgEffScale > 0.5f) {
                this->drawDmgEffScale = 0.5f;
            }

            Math_ApproachF(&this->drawDmgEffFrozenSteamScale, this->drawDmgEffScale, 0.1f, 0.04f);
        }

        Actor_DrawDamageEffects(play, &this->actor, this->bodyPartsPos, this->bodyPartsCount, this->drawDmgEffScale,
                                this->drawDmgEffFrozenSteamScale, drawDmgEffAlpha, this->drawDmgEffType);
    }

    CLOSE_DISPS(play->state.gfxCtx);
}
