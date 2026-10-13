/*
 * File: z_en_slime.c
 * Overlay: ovl_En_Slime
 * Description: Chuchu
 */

#include "z_en_slime.h"
#include "overlays/actors/ovl_En_Clear_Tag/z_en_clear_tag.h"

#include "2s2h/Enhancements/FrameInterpolation/FrameInterpolation.h"
#include "2s2h/GameInteractor/GameInteractor.h"
#include "2s2h/BenGui/CosmeticEditor.h"

#define FLAGS                                                                                 \
    (ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_HOSTILE | ACTOR_FLAG_UPDATE_CULLING_DISABLED | \
     ACTOR_FLAG_HOOKSHOT_PULLS_ACTOR)

#define ICE_BLOCK_TIMER_MAX 254
#define ICE_BLOCK_UNUSED (ICE_BLOCK_TIMER_MAX + 1)

typedef enum EnSlimeEyeTexture {
    /* 0 */ EN_SLIME_EYETEX_OPEN,
    /* 1 */ EN_SLIME_EYETEX_HALF,
    /* 2 */ EN_SLIME_EYETEX_CLOSED,
    /* 3 */ EN_SLIME_EYETEX_HALF2,
    /* 4 */ EN_SLIME_EYETEX_MAX
} EnSlimeEyeTexture;

typedef enum EnSlimeDamageEffect {
    /* 0x0 */ EN_SLIME_DMGEFF_NORMAL,
    /* 0x1 */ EN_SLIME_DMGEFF_STUN,
    /* 0x3 */ EN_SLIME_DMGEFF_FREEZE = 0x3,
    /* 0x4 */ EN_SLIME_DMGEFF_LIGHT,
    /* 0x5 */ EN_SLIME_DMGEFF_ELECTRIC,
    /* 0xE */ EN_SLIME_DMGEFF_HOOKSHOT = 0xE,
    /* 0xF */ EN_SLIME_DMGEFF_BLUNT
} EnSlimeDamageEffect;

void EnSlime_Init(Actor* thisx, PlayState* play);
void EnSlime_Destroy(Actor* thisx, PlayState* play);
void EnSlime_Update(Actor* thisx, PlayState* play);
void EnSlime_Draw(Actor* thisx, PlayState* play);

void EnSlime_SetupInitializeIdle(EnSlime* this);
void EnSlime_InitializeIdle(EnSlime* this, PlayState* play);
void EnSlime_SetupIdle(EnSlime* this);
void EnSlime_Idle(EnSlime* this, PlayState* play);
void EnSlime_SetupMoveInDirection(EnSlime* this);
void EnSlime_MoveInDirection(EnSlime* this, PlayState* play);
void EnSlime_SetupMoveToHome(EnSlime* this);
void EnSlime_MoveToHome(EnSlime* this, PlayState* play);
void EnSlime_SetupTurnToPlayer(EnSlime* this);
void EnSlime_TurnToPlayer(EnSlime* this, PlayState* play);
void EnSlime_SetupJump(EnSlime* this);
void EnSlime_Jump(EnSlime* this, PlayState* play);
void EnSlime_SetupLand(EnSlime* this);
void EnSlime_Land(EnSlime* this, PlayState* play);
void EnSlime_ReactToBluntHit(EnSlime* this, PlayState* play);
void EnSlime_Damaged(EnSlime* this, PlayState* play);
void EnSlime_SetupDead(EnSlime* this);
void EnSlime_Dead(EnSlime* this, PlayState* play);
void EnSlime_SpawnIceBlock(EnSlime* this, PlayState* play);
void EnSlime_SetupIceBlock(EnSlime* this);
void EnSlime_IceBlock(EnSlime* this, PlayState* play);
void EnSlime_Stun(EnSlime* this, PlayState* play);
void EnSlime_SetupIceBlockThaw(EnSlime* this);
void EnSlime_IceBlockThaw(EnSlime* this, PlayState* play);
void EnSlime_SetupWaitForRevive(EnSlime* this);
void EnSlime_WaitForRevive(EnSlime* this, PlayState* play);
void EnSlime_SetupRevive(EnSlime* this);
void EnSlime_Revive(EnSlime* this, PlayState* play);

ActorProfile En_Slime_Profile = {
    /**/ ACTOR_EN_SLIME,
    /**/ ACTORCAT_ENEMY,
    /**/ FLAGS,
    /**/ OBJECT_SLIME,
    /**/ sizeof(EnSlime),
    /**/ EnSlime_Init,
    /**/ EnSlime_Destroy,
    /**/ EnSlime_Update,
    /**/ EnSlime_Draw,
};

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
        ELEM_MATERIAL_UNK0,
        { 0xF7CFFFFF, 0x00, 0x04 },
        { 0xF7CFFFFF, 0x00, 0x00 },
        ATELEM_ON | ATELEM_SFX_HARD,
        ACELEM_ON | ACELEM_HOOKABLE,
        OCELEM_ON,
    },
    { 22, 35, 0, { 0, 0, 0 } },
};

static DamageTable sDamageTable = {
    /* Deku Nut       */ DMG_ENTRY(0, EN_SLIME_DMGEFF_STUN),
    /* Deku Stick     */ DMG_ENTRY(3, EN_SLIME_DMGEFF_NORMAL),
    /* Horse trample  */ DMG_ENTRY(1, EN_SLIME_DMGEFF_NORMAL),
    /* Explosives     */ DMG_ENTRY(1, EN_SLIME_DMGEFF_NORMAL),
    /* Zora boomerang */ DMG_ENTRY(1, EN_SLIME_DMGEFF_NORMAL),
    /* Normal arrow   */ DMG_ENTRY(1, EN_SLIME_DMGEFF_NORMAL),
    /* UNK_DMG_0x06   */ DMG_ENTRY(0, EN_SLIME_DMGEFF_NORMAL),
    /* Hookshot       */ DMG_ENTRY(0, EN_SLIME_DMGEFF_HOOKSHOT),
    /* Goron punch    */ DMG_ENTRY(0, EN_SLIME_DMGEFF_BLUNT),
    /* Sword          */ DMG_ENTRY(1, EN_SLIME_DMGEFF_NORMAL),
    /* Goron pound    */ DMG_ENTRY(0, EN_SLIME_DMGEFF_BLUNT),
    /* Fire arrow     */ DMG_ENTRY(2, EN_SLIME_DMGEFF_NORMAL),
    /* Ice arrow      */ DMG_ENTRY(2, EN_SLIME_DMGEFF_FREEZE),
    /* Light arrow    */ DMG_ENTRY(2, EN_SLIME_DMGEFF_LIGHT),
    /* Goron spikes   */ DMG_ENTRY(1, EN_SLIME_DMGEFF_NORMAL),
    /* Deku spin      */ DMG_ENTRY(1, EN_SLIME_DMGEFF_NORMAL),
    /* Deku bubble    */ DMG_ENTRY(0, EN_SLIME_DMGEFF_BLUNT),
    /* Deku launch    */ DMG_ENTRY(2, EN_SLIME_DMGEFF_NORMAL),
    /* UNK_DMG_0x12   */ DMG_ENTRY(0, EN_SLIME_DMGEFF_STUN),
    /* Zora barrier   */ DMG_ENTRY(1, EN_SLIME_DMGEFF_ELECTRIC),
    /* Normal shield  */ DMG_ENTRY(0, EN_SLIME_DMGEFF_NORMAL),
    /* Light ray      */ DMG_ENTRY(0, EN_SLIME_DMGEFF_NORMAL),
    /* Thrown object  */ DMG_ENTRY(0, EN_SLIME_DMGEFF_BLUNT),
    /* Zora punch     */ DMG_ENTRY(1, EN_SLIME_DMGEFF_NORMAL),
    /* Spin attack    */ DMG_ENTRY(1, EN_SLIME_DMGEFF_NORMAL),
    /* Sword beam     */ DMG_ENTRY(0, EN_SLIME_DMGEFF_NORMAL),
    /* Normal Roll    */ DMG_ENTRY(0, EN_SLIME_DMGEFF_NORMAL),
    /* UNK_DMG_0x1B   */ DMG_ENTRY(0, EN_SLIME_DMGEFF_NORMAL),
    /* UNK_DMG_0x1C   */ DMG_ENTRY(0, EN_SLIME_DMGEFF_NORMAL),
    /* Unblockable    */ DMG_ENTRY(0, EN_SLIME_DMGEFF_NORMAL),
    /* UNK_DMG_0x1E   */ DMG_ENTRY(0, EN_SLIME_DMGEFF_NORMAL),
    /* Powder Keg     */ DMG_ENTRY(1, EN_SLIME_DMGEFF_NORMAL),
};

static CollisionCheckInfoInit sColChkInfoInit = { 1, 22, 35, 60 };

static TexturePtr sEyeTextures[] = {
    gChuchuEyeOpenTex,
    gChuchuEyeHalfTex,
    gChuchuEyeClosedTex,
    gChuchuEyeHalfTex,
};

static InitChainEntry sInitChain[] = {
    ICHAIN_F32(gravity, -2, ICHAIN_CONTINUE),
    ICHAIN_F32(lockOnArrowOffset, 6000, ICHAIN_STOP),
};

static s32 sTexturesDesegmented = false;

static Color_RGBA8 sBubblePrimColor = { 255, 255, 255, 255 };
static Color_RGBA8 sBubbleEnvColor = { 150, 150, 150, 0 };
static Vec3f sBubbleAccel = { 0.0f, -0.8f, 0.0f };

AnimatedMaterial* sSlimeTexAnim;

void EnSlime_Init(Actor* thisx, PlayState* play) {
    EnSlime* this = (EnSlime*)thisx;
    s32 reviveTimeSeconds;
    s32 i;

    Actor_ProcessInitChain(&this->actor, sInitChain);
    Collider_InitAndSetCylinder(play, &this->collider, &this->actor, &sCylinderInit);
    CollisionCheck_SetInfo(&this->actor.colChkInfo, &sDamageTable, &sColChkInfoInit);
    ActorShape_Init(&this->actor.shape, 0.0f, ActorShadow_DrawCircle, 38.0f);

    this->iceBlockTimer = ICE_BLOCK_UNUSED;
    this->eyeTexIndex = EN_SLIME_EYETEX_OPEN;
    this->wobbleRot.y = 0.0f;

    // Sets maximum distance chuchu will move from its home before turning around.
    if (this->actor.shape.rot.x <= 0) {
        this->distLimit = 30000.0f;
    } else {
        this->distLimit = this->actor.shape.rot.x * 40.0f;
    }

    this->actor.shape.rot.x = 0;
    reviveTimeSeconds = EN_SLIME_GET_REVIVE_TIME(&this->actor);
    this->actor.shape.rot.z = 0;
    this->actor.params &= 0xFF;
    if (reviveTimeSeconds == 0xFF) {
        reviveTimeSeconds = 0;
    }

    // Upper byte of actorParams at init is # of seconds past 10s to revive.
    this->reviveTime = (reviveTimeSeconds * 20) + 200;

    // Update addresses of texture assets (if another instance hasn't already)
    if (!sTexturesDesegmented) {
        for (i = 0; i < EN_SLIME_EYETEX_MAX; i++) {
            sEyeTextures[i] = Lib_SegmentedToVirtual(sEyeTextures[i]);
        }
        sSlimeTexAnim = Lib_SegmentedToVirtual(gChuchuSlimeFlowTexAnim);
        sTexturesDesegmented = true;
    }

    // Set hint IDs and drop item icon depending on chuchu color
    if (EN_SLIME_GET_TYPE(&this->actor) == EN_SLIME_TYPE_YELLOW) {
        this->itemDropTex = Lib_SegmentedToVirtual(gDropArrows1Tex);
        this->actor.hintId = TATL_HINT_ID_YELLOW_CHUCHU;
    } else if (EN_SLIME_GET_TYPE(&this->actor) == EN_SLIME_TYPE_GREEN) {
        this->itemDropTex = Lib_SegmentedToVirtual(gDropMagicSmallTex);
        this->actor.hintId = TATL_HINT_ID_GREEN_CHUCHU;
    } else if (EN_SLIME_GET_TYPE(&this->actor) == EN_SLIME_TYPE_RED) {
        this->itemDropTex = Lib_SegmentedToVirtual(gDropRecoveryHeartTex);
        this->actor.hintId = TATL_HINT_ID_RED_CHUCHU;
    } else {
        this->actor.hintId = TATL_HINT_ID_BLUE_CHUCHU;
    }

    // Sets actor scale and initial action func
    EnSlime_SetupInitializeIdle(this);
}

void EnSlime_Destroy(Actor* thisx, PlayState* play) {
    EnSlime* this = (EnSlime*)thisx;

    Collider_DestroyCylinder(play, &this->collider);
}

/**
 * Upon immobilization via ice arrow (for chuchu types outside of blue),
 * initialize ice visual effects before chuchu enters immobile state.
 */
void EnSlime_Freeze(EnSlime* this) {
    this->drawDmgEffType = ACTOR_DRAW_DMGEFF_FROZEN_NO_SFX;
    this->drawDmgEffScale = 0.4f;
    this->collider.base.colMaterial = COL_MATERIAL_HIT3;
    this->drawDmgEffFrozenSteamScale = 0.6f;
    this->drawDmgEffAlpha = 1.0f;
    this->timer = 80;
    this->actor.flags &= ~ACTOR_FLAG_HOOKSHOT_PULLS_PLAYER;
    Actor_SetColorFilter(&this->actor, COLORFILTER_COLORFLAG_RED, 255, COLORFILTER_BUFFLAG_XLU, 80);
}

/**
 * If stunned ice effect is active (ie. non-blue chuchu was frozen by ice arrow),
 * initialize thaw and spawn thawing effect.
 */
void EnSlime_Thaw(EnSlime* this, PlayState* play) {
    if (this->drawDmgEffType == ACTOR_DRAW_DMGEFF_FROZEN_NO_SFX) {
        this->drawDmgEffType = 0; // So it's not triggered again until Freeze has been called again.
        this->collider.base.colMaterial = COL_MATERIAL_NONE;
        this->drawDmgEffAlpha = 0.0f;
        Actor_SpawnIceEffects(play, &this->actor, this->bodyPartsPos, EN_SLIME_BODYPART_MAX, 2, 0.2f, 0.2f);
        this->actor.flags |= ACTOR_FLAG_HOOKSHOT_PULLS_ACTOR;
    }
}

/**
 * If blink animation is in progress, advance to next frame.
 * If eyes are open, randomly (5% chance) initialize new blink cycle.
 */
void EnSlime_Blink(EnSlime* this) {
    if (this->eyeTexIndex != EN_SLIME_EYETEX_OPEN) {
        this->eyeTexIndex++;
        if (this->eyeTexIndex == EN_SLIME_EYETEX_MAX) {
            this->eyeTexIndex = EN_SLIME_EYETEX_OPEN;
        }
    } else if (Rand_ZeroOne() < 0.05f) {
        this->eyeTexIndex = EN_SLIME_EYETEX_HALF;
    }
}

/**
 * Set the actor's initial scale and action function.
 */
void EnSlime_SetupInitializeIdle(EnSlime* this) {
    this->actor.scale.x = 0.008f;
    this->actor.scale.z = 0.008f;
    this->actor.scale.y = 0.011f;
    this->actionFunc = EnSlime_InitializeIdle;
}

/**
 * Once actor touches the ground, it starts its idle behavior.
 * This function is only called right after the actor is first initialized.
 * It is not set as the action function again.
 */
void EnSlime_InitializeIdle(EnSlime* this, PlayState* play) {
    if (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND) {
        this->actor.flags &= ~ACTOR_FLAG_UPDATE_CULLING_DISABLED;
        EnSlime_SetupIdle(this);
    }
}

/**
 * Set or restore idle action state.
 * Sets timer to 25 and cycles to SitIdle action.
 */
void EnSlime_SetupIdle(EnSlime* this) {
    this->timer = 25;
    this->actor.speed = 0.0f;
    this->actionFunc = EnSlime_Idle;
}

/**
 * Sit idle for one frame, updating rotation and scale to give
 * the actor an amorphous, oozing effect.
 * Chuchu will engage player if player is visible and
 * close enough to it.
 */
void EnSlime_Idle(EnSlime* this, PlayState* play) {
    f32 timerFactor;
    f32 scale;

    this->timer--;
    timerFactor = sqrtf(this->timer) * 0.2f;
    EnSlime_Blink(this);

    scale = ((Math_SinF(this->timer * (2.0f * M_PIf / 5.0f)) * (0.13f * timerFactor)) + 1.0f) * 0.01f;
    this->actor.scale.x = scale;
    this->actor.scale.z = scale;
    if (this->timer < 24) {
        this->actor.scale.y = ((Math_CosF(this->timer * (2.0f * M_PIf / 5.0f)) * (0.05f * timerFactor)) + 1.0f) * 0.01f;
    }

    this->actor.shape.rot.x = Rand_CenteredFloat(0x200) * timerFactor;
    this->actor.shape.rot.z = Rand_CenteredFloat(0x200) * timerFactor;

    // If player is not wearing the stone mask, is within 280 units of the chuchu,
    // and the chuchu is facing the player, initialize chase/attack cycle.
    if ((Player_GetMask(play) != PLAYER_MASK_STONE) && (this->actor.xzDistToPlayer < 280.0f) &&
        Actor_IsFacingPlayer(&this->actor, 0x5000)) {
        EnSlime_SetupTurnToPlayer(this);
    } else if (this->timer == 0) {
        // Start moving (remaining idle)
        EnSlime_SetupMoveInDirection(this);
    }
}

/**
 * Reset the action timer (to 120), then select a new direction for
 * chuchu to move.
 */
void EnSlime_SetupMoveInDirection(EnSlime* this) {
    this->timer = 120;

    // If actor is more than 120 units from its home, set the new direction to be towards its home.
    // Otherwise, the new direction to move is pretty much random.
    if (Actor_WorldDistXZToPoint(&this->actor, &this->actor.home.pos) > 120.0f) {
        this->idleRotY = Actor_WorldYawTowardPoint(&this->actor, &this->actor.home.pos);
    } else if (Rand_ZeroOne() < 0.7f) {
        this->idleRotY = (s32)Rand_CenteredFloat(0x4000) + this->actor.shape.rot.y;
    }
    this->actor.world.rot.y = this->actor.shape.rot.y;
    this->actionFunc = EnSlime_MoveInDirection;
}

/**
 * While moving idle, update rotation, speed, scale in a sinusoidal fashion to
 * give oozing effect.
 * If actor has hit a wall or is too far from home, turn around.
 * Chuchu will engage player during idle movement if player is visible and
 * close enough to it.
 */
void EnSlime_MoveInDirection(EnSlime* this, PlayState* play) {
    f32 sinFactor;

    EnSlime_Blink(this);
    this->timer--;
    Math_ApproachS(&this->actor.shape.rot.y, this->idleRotY, 4, 0x100);

    // If actor is touching a wall, set target rotation to match wall where it is touching.
    // If actor is more than 120 units from its home, turn around to face home.
    if (this->actor.bgCheckFlags & BGCHECKFLAG_WALL) {
        this->idleRotY = this->actor.wallYaw;
    } else if (Actor_WorldDistXZToPoint(&this->actor, &this->actor.home.pos) > 120.0f) {
        this->idleRotY = Actor_WorldYawTowardPoint(&this->actor, &this->actor.home.pos);
    }

    // Update actor scale, xz speed, and rotation to provide amorphous effect
    sinFactor = fabsf(Math_SinF(this->timer * (M_PIf / 24)));
    Math_StepToF(&this->actor.scale.z, ((0.15f * sinFactor) + 1.0f) * 0.01f, 0.0002f);
    Math_StepToF(&this->actor.scale.x, (1.0f - (0.2f * sinFactor)) * 0.01f, 0.0002f);
    Math_StepToF(&this->actor.scale.y, (1.0f - (0.1f * sinFactor)) * 0.01f, 0.0002f);

    this->actor.speed = (0.8f * sinFactor) + 0.2f;
    this->actor.shape.rot.x = 0x800 * sinFactor;
    this->actor.world.rot.y = this->actor.shape.rot.y;

    if (Actor_WorldDistXZToPoint(&this->actor, &this->actor.home.pos) > this->distLimit) {
        // If actor is too far from home, start return home cycle
        EnSlime_SetupMoveToHome(this);
    } else if ((Player_GetMask(play) != PLAYER_MASK_STONE) && (this->actor.xzDistToPlayer < 280.0f) &&
               Actor_IsFacingPlayer(&this->actor, 0x5000)) {
        // If player is close enough to chuchu while not wearing the stone mask, and the chuchu is facing player,
        //  initialize attack cycle
        EnSlime_SetupTurnToPlayer(this);
    } else if (this->timer == 0) {
        // If move timer runs out, restart idle cycle (sit in place for 25 frames)
        EnSlime_SetupIdle(this);
    }
}

/**
 * Reset the action timer (to 24), then set the target y angle to face home.
 */
void EnSlime_SetupMoveToHome(EnSlime* this) {
    this->timer = 24;
    this->idleRotY = Actor_WorldYawTowardPoint(&this->actor, &this->actor.home.pos);
    this->actor.world.rot.y = this->actor.shape.rot.y;
    this->actionFunc = EnSlime_MoveToHome;
}

/**
 * While moving towards home point, update rotation, speed, scale
 * in a sinusoidal fashion to give oozing effect.
 * If actor has hit a wall, turn around.
 * Once it is close enough to home point, return to idle cycle.
 * Chuchu will not engage player while the return home cycle is active.
 */
void EnSlime_MoveToHome(EnSlime* this, PlayState* play) {
    f32 timerFactor;

    EnSlime_Blink(this);
    this->timer--;
    Math_ApproachS(&this->actor.shape.rot.y, this->idleRotY, 4, 0x400);
    if (this->actor.bgCheckFlags & BGCHECKFLAG_WALL) {
        // If hit wall, turn around.
        this->idleRotY = this->actor.wallYaw;
    } else {
        this->idleRotY = Actor_WorldYawTowardPoint(&this->actor, &this->actor.home.pos);
    }

    timerFactor = fabsf(Math_SinF(this->timer * (M_PIf / 24)));
    Math_StepToF(&this->actor.scale.z, ((0.15f * timerFactor) + 1.0f) * 0.01f, 0.0002f);
    Math_StepToF(&this->actor.scale.x, (1.0f - (0.2f * timerFactor)) * 0.01f, 0.0002f);
    Math_StepToF(&this->actor.scale.y, (1.0f - (0.1f * timerFactor)) * 0.01f, 0.0002f);

    this->actor.speed = (0.8f * timerFactor) + 0.2f;
    this->actor.shape.rot.x = 0x800 * timerFactor;
    this->actor.world.rot.y = this->actor.shape.rot.y;

    // Reset timer for use calculating wobble/ooze amount for frame
    if (this->timer == 0) {
        this->timer = 24;
    }

    // Only return to idle once actor is close enough to home point.
    if (Actor_WorldDistXZToPoint(&this->actor, &this->actor.home.pos) < (this->distLimit * 0.8f)) {
        EnSlime_SetupIdle(this);
    }
}

void EnSlime_SetupTurnToPlayer(EnSlime* this) {
    this->actor.shape.rot.x = 0;
    this->actor.shape.rot.z = 0;
    this->timer = 8;
    this->actionFunc = EnSlime_TurnToPlayer;
    this->actor.speed = 0.0f;
}

/**
 * Update actor angle and ooze effect as chuchu initially
 * turns to face player.
 */
void EnSlime_TurnToPlayer(EnSlime* this, PlayState* play) {
    f32 factorY;
    f32 scaleXZ;

    this->timer--;
    EnSlime_Blink(this);
    Math_ApproachS(&this->actor.shape.rot.y, this->actor.yawTowardsPlayer, 4, 0x1000);
    if (this->timer >= 0) {
        factorY = 8 - this->timer;
        scaleXZ = ((factorY * 0.04f) + 1.0f) * 0.01f;
        this->actor.scale.x = scaleXZ;
        this->actor.scale.y = (1.0f - (factorY * 0.05f)) * 0.01f;
        this->actor.scale.z = scaleXZ;
    }
    if (this->timer < -2) {
        EnSlime_SetupJump(this);
    }
}

/**
 * If fewer than 120 units from the player, start a larger attack lunge.
 * Otherwise, hop forward.
 */
void EnSlime_SetupJump(EnSlime* this) {
    if (this->actor.xzDistToPlayer > 120.0f) {
        this->actor.velocity.y = 11.0f;
        this->actor.speed = 5.0f;
        this->actor.gravity = -2.0f;
    } else {
        this->actor.velocity.y = 18.0f;
        this->actor.speed = 7.0f;
        this->actor.gravity = -3.5f;
    }
    this->actor.world.rot.y = this->actor.shape.rot.y;
    this->timer = 12;
    this->eyeTexIndex = EN_SLIME_EYETEX_OPEN;
    Math_StepToF(&this->actor.scale.x, 0.008f, 0.0025f);
    Math_StepToF(&this->actor.scale.y, 0.011f, 0.0025f);
    this->actor.scale.z = this->actor.scale.x;
    Actor_PlaySfx(&this->actor, NA_SE_EN_SLIME_JUMP);
    this->actionFunc = EnSlime_Jump;
}

/**
 * Check for collision and whether actor has stopped falling.
 * Update actor scale for slime effect.
 * If collision, knockback actor. If actor has stopped moving
 * in Y direction, initialize landing action.
 */
void EnSlime_Jump(EnSlime* this, PlayState* play) {
    this->timer--;
    if (this->collider.base.atFlags & AT_HIT) {
        if (this->actor.speed > 0.0f) {
            this->actor.speed *= -1.2f;
            this->collider.base.atFlags &= ~AT_HIT;
        }
    }
    Math_StepToF(&this->actor.scale.x, 0.008f, 0.0025f);
    Math_StepToF(&this->actor.scale.y, 0.011f, 0.0025f);
    this->actor.scale.z = this->actor.scale.x;
    if ((this->actor.velocity.y < 0.0f) && (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND)) {
        EnSlime_SetupLand(this);
    }
}

void EnSlime_SetupLand(EnSlime* this) {
    this->timer = 15;
    this->actor.scale.x = 0.0132f;
    this->actor.scale.y = 0.0074399994f;
    this->actor.scale.z = 0.0132f;
    this->actor.speed = 0.0f;
    this->actionFunc = EnSlime_Land;
}

/**
 * Update scale/rot for ooze jiggle effect (XZ), update Y scale
 * to make it appear as though chuchu is compressing.
 * If > 12 frames have passed since jump start, check again if player
 * is in range and restart engage/jump cycle if so.
 * If too far from home, initialize return home cycle
 * (takes priority over player engage check).
 * Otherwise if timer is up, return to idle.
 */
void EnSlime_Land(EnSlime* this, PlayState* play) {
    f32 scaleY;
    f32 rotXZ;
    f32 scaleXZ;

    EnSlime_Blink(this);
    this->timer--;
    scaleY = ((this->timer / 5) + 1) * 1.6f;
    rotXZ = sqrtf(this->timer) * 0.2f;
    scaleXZ = ((Math_CosF(this->timer * (2.0f * M_PIf / 5.0f)) * (0.05f * scaleY)) + 1.0f) * 0.01f;
    this->actor.scale.x = scaleXZ;
    this->actor.scale.z = scaleXZ;
    if (this->timer < 15) {
        this->actor.scale.y = (1.0f - (Math_SinF(this->timer * (2.0f * M_PIf / 5.0f)) * (0.04f * scaleY))) * 0.01f;
    }
    this->actor.shape.rot.x = Rand_CenteredFloat(0x200) * rotXZ;
    this->actor.shape.rot.z = Rand_CenteredFloat(0x200) * rotXZ;

    if (Actor_WorldDistXZToPoint(&this->actor, &this->actor.home.pos) > this->distLimit) {
        EnSlime_SetupMoveToHome(this);
    } else if ((Player_GetMask(play) != PLAYER_MASK_STONE) && (this->actor.xzDistToPlayer < 280.0f) &&
               (this->timer < 12)) {
        EnSlime_SetupTurnToPlayer(this);
    } else if (this->timer == 0) {
        EnSlime_SetupIdle(this);
    }
}

/**
 * Initialize reaction to receiving a blunt hit (such as with Goron Punch etc.)
 * Chuchu does not take damage, but appears slide backwards.
 */
void EnSlime_SetupReactToBluntHit(EnSlime* this) {
    this->actor.speed = 10.0f;
    if (this->actor.velocity.y > 0.0f) {
        this->actor.velocity.y = 0.0f;
    }
    this->collider.base.acFlags &= ~AC_ON;
    this->timer = 30;
    Actor_PlaySfx(&this->actor, NA_SE_EN_SLIME_DEFENCE);
    if (this->collider.base.ac != NULL) {
        this->actor.world.rot.y = Actor_WorldYawTowardActor(&this->actor, this->collider.base.ac) + 0x8000;
    } else {
        this->actor.world.rot.y = this->actor.yawTowardsPlayer + 0x8000;
    }
    this->actionFunc = EnSlime_ReactToBluntHit;
}

/**
 * Update reaction to receiving a blunt hit (such as with Goron Punch etc.)
 * Update XZ speed and jiggle effect
 * AC_ON flag is off until 15 frames of reaction remain.
 * When timer is up, chuchu will choose whether to engage player, return home,
 * or return to idle.
 * If chuchu is too far from home, returning home will take precedence.
 */
void EnSlime_ReactToBluntHit(EnSlime* this, PlayState* play) {
    f32 timerFactor;
    f32 scale;

    EnSlime_Blink(this);
    this->timer--;
    Math_StepToF(&this->actor.speed, 0.0f, 1.0f);
    timerFactor = sqrtf(this->timer);
    if (this->timer < 30) {
        scale = ((Math_CosF(this->timer * (2.0f * M_PIf / 5.0f)) * (0.08f * timerFactor)) + 1.0f) * 0.01f;
        this->actor.scale.x = scale;
        this->actor.scale.z = scale;
    }
    if (this->timer == 15) {
        this->collider.base.acFlags |= AC_ON;
    }
    this->actor.scale.y =
        ((Math_SinF((f32)this->timer * (2.0f * M_PIf / 5.0f)) * (0.07f * timerFactor)) + 1.0f) * 0.01f;
    this->actor.shape.rot.x = Rand_CenteredFloat(0x200) * timerFactor;
    this->actor.shape.rot.z = Rand_CenteredFloat(0x200) * timerFactor;

    if (this->timer == 0) {
        if (Actor_WorldDistXZToPoint(&this->actor, &this->actor.home.pos) > this->distLimit) {
            EnSlime_SetupMoveToHome(this);
        } else if ((this->actor.xzDistToPlayer < 280.0f) && (Player_GetMask(play) != PLAYER_MASK_STONE)) {
            EnSlime_SetupTurnToPlayer(this);
        } else {
            EnSlime_SetupIdle(this);
        }
    }
}

/**
 * Spawn bubbles & splash, start actor rebuff wobble,
 * push actor, and play sfx.
 */
void EnSlime_SetupDamaged(EnSlime* this, PlayState* play, s32 arg2) {
    s32 i;
    Vec3f effectVelocity;
    Vec3f effectPos;
    f32 ySin;
    f32 yCos;

    this->collider.base.acFlags &= ~AC_ON;
    if (this->actor.velocity.y > 0.0f) {
        this->actor.velocity.y = 0.0f;
    }
    Actor_SetColorFilter(&this->actor, COLORFILTER_COLORFLAG_RED, 255, COLORFILTER_BUFFLAG_XLU, 20);
    this->timer = 20;
    this->actor.speed = 10.0f;
    if (arg2 == true) {
        // This function changes actor's world Y rotation?
        func_800BE504(&this->actor, &this->collider);
    }

    this->eyeTexIndex = EN_SLIME_EYETEX_OPEN;
    Actor_SetScale(&this->actor, 0.01f);
    this->wobbleRot.x = Rand_ZeroOne() * (M_PIf * 2.0f);
    this->wobbleRot.z = Rand_ZeroOne() * (M_PIf * 2.0f);
    ySin = Math_SinS(this->actor.world.rot.y) * 10.0f;
    yCos = Math_CosS(this->actor.world.rot.y) * 10.0f;
    effectPos.x = this->actor.world.pos.x + ySin;
    effectPos.y = this->actor.world.pos.y + 20.0f;
    effectPos.z = this->actor.world.pos.z + yCos;
    EffectSsGSplash_Spawn(play, &effectPos, NULL, NULL, 1, 550);

    for (i = 0; i < 10; i++) {
        effectPos.x = Rand_CenteredFloat(40.0f) + this->actor.world.pos.x + ySin;
        effectPos.y = Rand_CenteredFloat(10.0f) + this->actor.world.pos.y + 40.0f;
        effectPos.z = Rand_CenteredFloat(40.0f) + this->actor.world.pos.z + yCos;

        effectVelocity.x = ((Rand_ZeroOne() * 3.5f) + 1.0f) * -Math_SinS(this->actor.world.rot.y);
        effectVelocity.z = ((Rand_ZeroOne() * 3.5f) + 1.0f) * -Math_CosS(this->actor.world.rot.y);

        effectVelocity.y = (Rand_ZeroOne() * 6.0f) + 2.0f;
        EffectSsDtBubble_SpawnCustomColor(play, &effectPos, &effectVelocity, &sBubbleAccel, &sBubblePrimColor,
                                          &sBubbleEnvColor, Rand_S16Offset(40, 20), 20, 0);
    }

    if (this->actor.colChkInfo.health == 0) {
        Actor_PlaySfx(&this->actor, NA_SE_EN_SLIME_DEAD);
    } else {
        Actor_PlaySfx(&this->actor, NA_SE_EN_SLIME_DAMAGE);
    }
    this->actionFunc = EnSlime_Damaged;
}

/**
 * Update reaction to taking damage.
 * Updates the rebuff wobble.
 * Once timer drops to 0, actor initializes death cycle, re-engages player, or returns to idle.
 */
void EnSlime_Damaged(EnSlime* this, PlayState* play) {
    this->timer--;
    Math_StepToF(&this->actor.speed, 0.0f, 1.0f);
    if ((this->timer % 5) == 0) {
        this->wobbleRot.x = Rand_ZeroOne() * (M_PIf * 2.0f);
        this->wobbleRot.z = Rand_ZeroOne() * (M_PIf * 2.0f);
    }
    if (this->timer == 0) {
        if (this->actor.colChkInfo.health != 0) {
            this->collider.base.acFlags |= AC_ON;
            if ((this->actor.xzDistToPlayer < 280.0f) && (Player_GetMask(play) != PLAYER_MASK_STONE)) {
                EnSlime_SetupTurnToPlayer(this);
            } else {
                EnSlime_SetupIdle(this);
            }
        } else {
            SoundSource_PlaySfxAtFixedWorldPos(play, &this->actor.world.pos, 40, NA_SE_EN_SLIME_BREAK);
            EnSlime_SetupDead(this);
        }
    }
}

/**
 * Disable AC_ON flag, make actor stop moving, and shrink the actor.
 */
void EnSlime_SetupDead(EnSlime* this) {
    this->collider.base.acFlags &= ~AC_ON;
    if (this->actor.velocity.y > 0.0f) {
        this->actor.velocity.y = 0.0f;
    }
    Actor_SetScale(&this->actor, 0.01f);
    this->actor.speed = 0.0f;
    this->actionFunc = EnSlime_Dead;
}

/**
 * Spawn random effects (bubbles, splash) for death sequence.
 * If chuchu is red, green, or yellow, spawn item drop.
 * Shrink actor scale to 0.
 * Call SetupWaitForRevive function for cleanup.
 */
void EnSlime_Dead(EnSlime* this, PlayState* play) {
    s32 i;
    Vec3f velocity;
    Vec3f effectPos;
    f32 randFloat;
    s32 randSign;

    if (Math_StepToF(&this->actor.scale.x, 0.0f, 0.002f)) {
        effectPos.x = this->actor.world.pos.x;
        effectPos.y = this->actor.world.pos.y + 20.0f;
        effectPos.z = this->actor.world.pos.z;
        EffectSsGSplash_Spawn(play, &effectPos, NULL, NULL, 1, 800);

        for (i = 0; i < 15; i++) {
            randFloat = Rand_ZeroOne();
            randSign = Rand_ZeroOne() < 0.5f ? -1 : 1;
            velocity.x = randSign * ((randFloat * 2.5f) + 2.0f);

            randFloat = Rand_ZeroOne();
            randSign = Rand_ZeroOne() < 0.5f ? -1 : 1;
            velocity.z = randSign * ((randFloat * 2.5f) + 2.0f);

            velocity.y = (Rand_ZeroOne() * 6.0f) + 2.0f;
            EffectSsDtBubble_SpawnCustomColor(play, &effectPos, &velocity, &sBubbleAccel, &sBubblePrimColor,
                                              &sBubbleEnvColor, Rand_S16Offset(40, 20), 20, 0);
        }

        if (this->actor.params == EN_SLIME_TYPE_YELLOW) {
            Item_DropCollectible(play, &this->actor.world.pos, ITEM00_ARROWS_10);
        } else if (this->actor.params == EN_SLIME_TYPE_GREEN) {
            Item_DropCollectible(play, &this->actor.world.pos, ITEM00_MAGIC_JAR_SMALL);
        } else if (this->actor.params == EN_SLIME_TYPE_RED) {
            Item_DropCollectible(play, &this->actor.world.pos, ITEM00_RECOVERY_HEART);
        }
        EnSlime_SetupWaitForRevive(this);
    }
    this->actor.scale.y = this->actor.scale.x;
    this->actor.scale.z = this->actor.scale.x;
}

/**
 * Calculate a new "current" position value that is a multiple of 60
 * away from the "home" position.
 * This is for use by blue chuchus frozen into an ice block so that
 * their position is a multiple of the ice block size.
 */
f32 EnSlime_SnapIceBlockPosition(f32 currentPosition, f32 homePosition) {
    s32 dist = currentPosition - homePosition;

    if (dist > 0) {
        dist += 30;
    } else {
        dist -= 30;
    }

    return ((dist / 60) * 60.0f) + homePosition;
}

/**
 * Prepare actor for ice block to spawn around it.
 * The actor will stop moving, have AC_ON cleared,
 * and target position will be snapped to
 * a multiple of the size of an ice block.
 */
void EnSlime_SetupSpawnIceBlock(EnSlime* this) {
    this->collider.base.acFlags &= ~AC_ON;
    this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
    this->drawDmgEffAlpha = 0.0f;
    this->actor.speed = 0.0f;
    this->actor.velocity.y = 0.0f;
    this->actor.gravity = 0.0f;
    this->iceBlockSnapPos.x = EnSlime_SnapIceBlockPosition(this->actor.world.pos.x, this->actor.home.pos.x);
    this->iceBlockSnapPos.z = EnSlime_SnapIceBlockPosition(this->actor.world.pos.z, this->actor.home.pos.z);
    Actor_SetColorFilter(&this->actor, COLORFILTER_COLORFLAG_GRAY, COLORFILTER_INTENSITY_FLAG | 255,
                         COLORFILTER_BUFFLAG_XLU, 10);
    this->eyeTexIndex = EN_SLIME_EYETEX_OPEN;
    this->iceBlockTimer = ICE_BLOCK_TIMER_MAX;
    this->actionFunc = EnSlime_SpawnIceBlock;
}

/**
 * Attempt to move the chuchu to snapped position and
 * spawn an ice block actor as a child of the chuchu actor.
 * If ice block spawn fails, chuchu returns to idle.
 */
void EnSlime_SpawnIceBlock(EnSlime* this, PlayState* play) {
    s32 stepCheck;

    stepCheck = Math_StepToF(&this->actor.world.pos.x, this->iceBlockSnapPos.x, 10.0f);
    stepCheck &= Math_StepToF(&this->actor.world.pos.z, this->iceBlockSnapPos.z, 10.0f);

    this->actor.colorFilterTimer = 10;
    if (stepCheck) {
        this->actor.child =
            Actor_SpawnAsChild(&play->actorCtx, &this->actor, play, ACTOR_OBJ_ICEBLOCK, this->actor.world.pos.x,
                               this->actor.world.pos.y + 30.0f, this->actor.world.pos.z, 0, 0, 0, 0);
        if (this->actor.child != NULL) {
            this->actor.child->csId = this->actor.csId;
            EnSlime_SetupIceBlock(this);
        } else {
            this->actor.colorFilterTimer = 0;
            this->collider.base.acFlags |= AC_ON;
            this->iceBlockTimer = ICE_BLOCK_UNUSED;
            this->actor.flags |= ACTOR_FLAG_ATTENTION_ENABLED;
            this->actor.gravity = -2.0f;
            EnSlime_SetupIdle(this);
        }
    }
}

void EnSlime_SetupIceBlock(EnSlime* this) {
    this->actor.flags |= ACTOR_FLAG_UPDATE_CULLING_DISABLED;
    this->actionFunc = EnSlime_IceBlock;
}

/**
 * Decrement the ice block timer, or if it has hit zero,
 * check to see if the ice block child actor link has been broken
 * or if the block has shrunk sufficiently.
 * If timer is up and ice block is sufficiently "gone", init thaw.
 */
void EnSlime_IceBlock(EnSlime* this, PlayState* play) {
    if (this->iceBlockTimer == 0) {
        if ((this->actor.child == NULL) || (this->actor.child->update == NULL) ||
            !(this->actor.child->scale.y >= 0.1f)) {
            EnSlime_SetupIceBlockThaw(this);
        }
    } else {
        this->actor.colorFilterTimer = 10;
        this->iceBlockTimer = CLAMP_MIN(this->iceBlockTimer - 5, 0);
    }
}

void EnSlime_SetupStun(EnSlime* this) {
    this->actor.speed = 0.0f;
    func_800BE504(&this->actor, &this->collider);
    if (this->actor.velocity.y > 0.0f) {
        this->actor.velocity.y = 0.0f;
    }
    this->actionFunc = EnSlime_Stun;
}

/**
 * Check the actor's timer to see whether stun has worn off.
 * If so, either react to damage and initialize death, or start
 * idly wobbling in a new direction.
 */
void EnSlime_Stun(EnSlime* this, PlayState* play) {
    this->timer--;
    if (this->timer == 0) {
        EnSlime_Thaw(this, play);
        if (this->actor.colChkInfo.health == 0) {
            EnSlime_SetupDamaged(this, play, false);
        } else {
            this->actor.world.rot.y = this->actor.shape.rot.y;
            EnSlime_SetupMoveInDirection(this);
        }
    }
}

void EnSlime_SetupIceBlockThaw(EnSlime* this) {
    this->actor.colorFilterTimer = 0;
    this->actor.gravity = -2.0f;
    Actor_SetColorFilter(&this->actor, COLORFILTER_COLORFLAG_GRAY, COLORFILTER_INTENSITY_FLAG | 255,
                         COLORFILTER_BUFFLAG_XLU, 10);
    this->actionFunc = EnSlime_IceBlockThaw;
}

/**
 * Update the actor's ice block timer to match state of ice block.
 * When it reaches the "unset" value (255), return actor to idle.
 */
void EnSlime_IceBlockThaw(EnSlime* this, PlayState* play) {
    s32 iceBlockTimerAdj;

    this->actor.colorFilterTimer = 10;
    if ((this->actor.child != NULL) && (this->actor.child->update != NULL)) {
        iceBlockTimerAdj = (0.1f - this->actor.child->scale.y) * 10.0f * 255.0f;
        this->iceBlockTimer = CLAMP(iceBlockTimerAdj, 0, ICE_BLOCK_UNUSED);
    } else {
        this->actor.child = NULL;
        iceBlockTimerAdj = this->iceBlockTimer + 10;
        this->iceBlockTimer = CLAMP_MAX(iceBlockTimerAdj, ICE_BLOCK_UNUSED);
    }

    if (this->iceBlockTimer == ICE_BLOCK_UNUSED) {
        this->collider.base.acFlags |= AC_ON;
        this->actor.flags |= ACTOR_FLAG_ATTENTION_ENABLED;
        this->actor.flags &= ~ACTOR_FLAG_UPDATE_CULLING_DISABLED;
        EnSlime_SetupIdle(this);
    }
}

/**
 * Set speed to zero, and make undrawable.
 * Initialize revive timer.
 */
void EnSlime_SetupWaitForRevive(EnSlime* this) {
    this->actor.draw = NULL;
    this->actor.flags |= ACTOR_FLAG_UPDATE_CULLING_DISABLED;
    this->drawDmgEffAlpha = 0.0f;
    this->actor.gravity = 0.0f;
    this->actor.velocity.y = 0.0f;
    this->actor.speed = 0.0f;
    this->collider.base.ocFlags1 &= ~OC1_ON;
    this->timer = this->reviveTime;
    this->actionFunc = EnSlime_WaitForRevive;
}

/**
 * Decrement the revival timer, or if it is zero,
 * initialize revive.
 */
void EnSlime_WaitForRevive(EnSlime* this, PlayState* play) {
    if (this->timer == 0) {
        EnSlime_SetupRevive(this);
    } else {
        this->timer--;
    }
}

/**
 * Re-enable drawing, reset the actor's health and home,
 * and setup and initialize revive action.
 */
void EnSlime_SetupRevive(EnSlime* this) {
    this->actor.draw = EnSlime_Draw;
    this->actor.colChkInfo.health = sColChkInfoInit.health;
    this->iceBlockTimer = ICE_BLOCK_UNUSED;
    Math_Vec3f_Copy(&this->actor.world.pos, &this->actor.home.pos);
    this->timer = 0;
    this->actor.colorFilterTimer = 0;
    this->actor.home.rot.y = this->actor.yawTowardsPlayer;
    Actor_PlaySfx(&this->actor, NA_SE_EN_SLIME_SURFACE);
    this->actor.gravity = -2.0f;
    this->actionFunc = EnSlime_Revive;
}

/**
 * Update bubbling back up effect. After 21 frames, OC1_ON is set.
 * After 28 frames, AC_ON is set and actor returns to idle movement.
 */
void EnSlime_Revive(EnSlime* this, PlayState* play) {
    f32 rescaleFactor1;
    f32 rescaleFactor2;

    this->timer++;
    if (this->timer == 28) {
        this->actor.flags &= ~ACTOR_FLAG_UPDATE_CULLING_DISABLED;
        this->actor.flags |= ACTOR_FLAG_ATTENTION_ENABLED;
        this->collider.base.acFlags |= AC_ON;
        this->actor.shape.rot.y = this->actor.home.rot.y;
        EnSlime_SetupMoveInDirection(this);
    } else {
        if (this->timer < 12) {
            rescaleFactor1 = this->timer * 0.0008333333f; // This is about 1/1200, but (1.0f/1200.0f) does not match
            this->reviveScale.x = rescaleFactor1;
            this->reviveScale.z = rescaleFactor1;
            this->reviveScale.y = 2.0f * rescaleFactor1;
            this->reviveRotY = this->timer * (0x4000 / 10.0f);
        } else if (this->timer < 20) {
            rescaleFactor1 = (this->timer - 12) * (1.0f / 1600.0f);
            rescaleFactor2 = 0.01f + rescaleFactor1;
            this->reviveScale.x = rescaleFactor2;
            this->reviveScale.z = rescaleFactor2;
            this->reviveScale.y = 2.0f * (0.01f - rescaleFactor1);
            this->reviveRotY = this->timer * (0x4000 / 10.0f);
        } else if (this->timer < 24) {
            rescaleFactor1 = (this->timer - 20) * 0.0033333332f; // This is about 1/300
            rescaleFactor2 = 0.015f - rescaleFactor1;
            this->reviveScale.x = rescaleFactor2;
            this->reviveScale.z = rescaleFactor2;
            this->reviveScale.y = (2.0f * rescaleFactor1) + 0.01f;
            Actor_SetScale(&this->actor, 1.5f * rescaleFactor1);
            this->reviveRotY = ((23 - this->timer) * 7281.778f) + 10922.667f;
            this->actor.shape.rot.y = (28 - this->timer) * (f32)0x1000;
        } else {
            Actor_SetScale(&this->actor, (((28 - this->timer) * 0.1f) + 1.0f) * 0.01f);
            Math_Vec3f_Copy(&this->reviveScale, &gZeroVec3f);
            this->actor.shape.rot.y = (28 - this->timer) * (f32)0x1000;
        }

        if (this->timer == 21) {
            this->collider.base.ocFlags1 |= OC1_ON;
        }

        this->reviveRotY += this->actor.home.rot.y;
        this->actor.shape.rot.y += this->actor.home.rot.y;
    }
}

/**
 * Check actor AC hit since last update. If hit, initialize appropriate action cycle.
 */
void EnSlime_UpdateDamage(EnSlime* this, PlayState* play) {
    if (this->collider.base.acFlags & AC_HIT) {
        this->collider.base.acFlags &= ~AC_HIT;

        if ((this->drawDmgEffType != ACTOR_DRAW_DMGEFF_FROZEN_NO_SFX) ||
            !(this->collider.elem.acHitElem->atDmgInfo.dmgFlags & 0xDB0B3)) {

            EnSlime_Thaw(this, play);
            if ((this->actor.params == EN_SLIME_TYPE_BLUE) &&
                (this->actor.colChkInfo.damageEffect == EN_SLIME_DMGEFF_FREEZE)) {
                // Blue chuchus take no damage from ice arrows.
                this->actor.colChkInfo.damage = 0;
            }

            if (Actor_ApplyDamage(&this->actor) == 0) {
                Actor_SetDropFlag(&this->actor, &this->collider.elem);
                Enemy_StartFinishingBlow(play, &this->actor);
                this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
            }

            if (this->actor.colChkInfo.damageEffect == EN_SLIME_DMGEFF_BLUNT) {
                EnSlime_SetupReactToBluntHit(this);
            } else if (this->actor.colChkInfo.damageEffect == EN_SLIME_DMGEFF_STUN) {
                this->timer = 40;
                Actor_SetColorFilter(&this->actor, COLORFILTER_COLORFLAG_BLUE, 255, COLORFILTER_BUFFLAG_XLU, 40);
                Actor_PlaySfx(&this->actor, NA_SE_EN_COMMON_FREEZE);
                EnSlime_SetupStun(this);
            } else if (this->actor.colChkInfo.damageEffect != EN_SLIME_DMGEFF_HOOKSHOT) {
                if (this->actor.colChkInfo.damageEffect == EN_SLIME_DMGEFF_FREEZE) {
                    if (this->actor.params == EN_SLIME_TYPE_BLUE) {
                        EnSlime_SetupSpawnIceBlock(this);
                    } else {
                        EnSlime_Freeze(this);
                        if (this->actor.colChkInfo.health == 0) {
                            this->timer = 3;
                            this->collider.base.acFlags &= ~AC_ON;
                        }
                        EnSlime_SetupStun(this);
                    }
                } else {
                    if (this->actor.colChkInfo.damageEffect == EN_SLIME_DMGEFF_LIGHT) {
                        this->drawDmgEffAlpha = 4.0f;
                        this->drawDmgEffScale = 0.4f;
                        this->drawDmgEffType = ACTOR_DRAW_DMGEFF_LIGHT_ORBS;
                        Actor_Spawn(&play->actorCtx, play, ACTOR_EN_CLEAR_TAG, this->collider.elem.acDmgInfo.hitPos.x,
                                    this->collider.elem.acDmgInfo.hitPos.y, this->collider.elem.acDmgInfo.hitPos.z, 0,
                                    0, 0, CLEAR_TAG_PARAMS(CLEAR_TAG_LARGE_LIGHT_RAYS));
                    } else if (this->actor.colChkInfo.damageEffect == EN_SLIME_DMGEFF_ELECTRIC) {
                        this->drawDmgEffType = ACTOR_DRAW_DMGEFF_ELECTRIC_SPARKS_LARGE;
                        this->drawDmgEffAlpha = 4.0f;
                        this->drawDmgEffScale = 0.4f;
                    }
                    EnSlime_SetupDamaged(this, play, true);
                }
            }
        }
    }
}

void EnSlime_Update(Actor* thisx, PlayState* play) {
    EnSlime* this = (EnSlime*)thisx;
    s32 pad;
    Player* player = GET_PLAYER(play);

    EnSlime_UpdateDamage(this, play);
    this->actionFunc(this, play);

    // Note: Does not match if you use this->actor instead of thisx
    thisx->shape.shadowAlpha = this->iceBlockTimer;
    if (this->iceBlockTimer == ICE_BLOCK_UNUSED) {
        if (thisx->scale.y > 0.0001f) {
            thisx->lockOnArrowOffset = 60.0f / thisx->scale.y;
        }

        if (this->collider.base.ocFlags1 & OC1_ON) {
            Actor_MoveWithGravity(thisx);
            Actor_UpdateBgCheckInfo(play, thisx, 20.0f, 35.0f, 40.0f,
                                    UPDBGCHECKINFO_FLAG_1 | UPDBGCHECKINFO_FLAG_4 | UPDBGCHECKINFO_FLAG_8 |
                                        UPDBGCHECKINFO_FLAG_10);
        }

        Collider_UpdateCylinder(thisx, &this->collider);
        this->collider.dim.radius = sCylinderInit.dim.radius * (100.0f * thisx->scale.z);
        this->collider.dim.height = sCylinderInit.dim.height * (100.0f * thisx->scale.y);

        if (this->actionFunc == EnSlime_Jump) {
            CollisionCheck_SetAT(play, &play->colChkCtx, &this->collider.base);
        }

        if (this->collider.base.acFlags & AC_ON) {
            CollisionCheck_SetAC(play, &play->colChkCtx, &this->collider.base);
        }

        if (player->stateFlags3 & PLAYER_STATE3_100) {
            this->collider.base.ocFlags1 &= ~OC1_TYPE_PLAYER;
        } else {
            this->collider.base.ocFlags1 |= OC1_TYPE_PLAYER;
        }

        if (this->collider.base.ocFlags1 & OC1_ON) {
            CollisionCheck_SetOC(play, &play->colChkCtx, &this->collider.base);
        }

        Actor_SetFocus(thisx, 15.0f);
        if (this->drawDmgEffAlpha > 0.0f) {
            if (this->drawDmgEffType != ACTOR_DRAW_DMGEFF_FROZEN_NO_SFX) {
                Math_StepToF(&this->drawDmgEffAlpha, 0.0f, 0.05f);
                this->drawDmgEffScale = (this->drawDmgEffAlpha + 1.0f) * 0.2f;
                this->drawDmgEffScale = CLAMP_MAX(this->drawDmgEffScale, 0.4f);
            } else if (!Math_StepToF(&this->drawDmgEffFrozenSteamScale, 0.4f, 0.01f)) {
                Actor_PlaySfx_Flagged(thisx, NA_SE_EV_ICE_FREEZE - SFX_FLAG);
            }
        }
    }
}

static Color_RGBA8 sPrimColors[EN_SLIME_TYPE_MAX] = {
    { 255, 255, 255, 255 }, // EN_SLIME_TYPE_BLUE
    { 255, 255, 0, 255 },   // EN_SLIME_TYPE_GREEN
    { 255, 255, 200, 255 }, // EN_SLIME_TYPE_YELLOW
    { 225, 200, 255, 255 }, // EN_SLIME_TYPE_RED
};

static Color_RGBA8 sEnvColors[EN_SLIME_TYPE_MAX] = {
    { 140, 255, 195, 255 }, // EN_SLIME_TYPE_BLUE
    { 50, 255, 0, 255 },    // EN_SLIME_TYPE_GREEN
    { 255, 180, 0, 255 },   // EN_SLIME_TYPE_YELLOW
    { 255, 50, 155, 255 },  // EN_SLIME_TYPE_RED
};

static Vec3f sBodyPartPosOffsets[EN_SLIME_BODYPART_MAX] = {
    { 2000.0f, 2000.0f, 0.0f },     // EN_SLIME_BODYPART_0
    { -1500.0f, 2500.0f, -500.0f }, // EN_SLIME_BODYPART_1
    { -500.0f, 1000.0f, 2500.0f },  // EN_SLIME_BODYPART_2
    { 0.0f, 4000.0f, 0.0f },        // EN_SLIME_BODYPART_3
    { 0.0f, 2000.0f, -2000.0f },    // EN_SLIME_BODYPART_4
};

void EnSlime_Draw(Actor* thisx, PlayState* play) {
    s32 i;
    EnSlime* this = (EnSlime*)thisx;
    Vec3f wobbleScale;
    Color_RGBA8* primColor;
    Color_RGBA8* envColor;
    s32 pad;

    OPEN_DISPS(play->state.gfxCtx);

    Gfx_SetupDL25_Opa(play->state.gfxCtx);
    Gfx_SetupDL25_Xlu(play->state.gfxCtx);
    func_800B8118(&this->actor, play, 0);
    if (this->iceBlockTimer != ICE_BLOCK_UNUSED) {
        gSPSegment(POLY_XLU_DISP++, 10, D_801AEFA0);
        gDPSetPrimColor(POLY_XLU_DISP++, 0, 170, 255, 255, 255, 255);
        gDPSetEnvColor(POLY_XLU_DISP++, 150, 255, 255, this->iceBlockTimer);
    } else {
        primColor = &sPrimColors[this->actor.params];
        envColor = &sEnvColors[this->actor.params];
        AnimatedMat_Draw(play, sSlimeTexAnim);
        switch (this->actor.params) {
            case EN_SLIME_TYPE_GREEN:
                gDPSetPrimColorOverride(POLY_XLU_DISP++, 0, 100, primColor->r, primColor->g, primColor->b, primColor->a,
                                        COSMETIC_ID("HUD.Magic"));
                gDPSetEnvColorOverride(POLY_XLU_DISP++, envColor->r, envColor->g, envColor->b, 255,
                                       COSMETIC_ID("HUD.Magic"));
                break;
            case EN_SLIME_TYPE_RED:
                gDPSetPrimColorOverride(POLY_XLU_DISP++, 0, 100, primColor->r, primColor->g, primColor->b, primColor->a,
                                        COSMETIC_ID("HUD.Hearts"));
                gDPSetEnvColorOverride(POLY_XLU_DISP++, envColor->r, envColor->g, envColor->b, 255,
                                       COSMETIC_ID("HUD.Hearts"));
                break;
            default:
                gDPSetPrimColor(POLY_XLU_DISP++, 0, 100, primColor->r, primColor->g, primColor->b, primColor->a);
                gDPSetEnvColor(POLY_XLU_DISP++, envColor->r, envColor->g, envColor->b, 255);
                break;
        }
    }

    if (this->actionFunc == EnSlime_Damaged) {
        wobbleScale.x = 1.0f - (Math_SinF((f32)this->timer * (M_PIf / 2.0f)) * 0.3f);
        wobbleScale.y = (Math_SinF((f32)this->timer * (M_PIf / 2.0f)) * 0.3f) + 1.0f;
        wobbleScale.z = 1.0f - (Math_CosF((f32)this->timer * (M_PIf / 2.0f)) * 0.3f);

        Matrix_RotateXFApply(this->wobbleRot.x);
        Matrix_RotateZF(this->wobbleRot.z, MTXMODE_APPLY);
        Matrix_Scale(wobbleScale.x, wobbleScale.y, wobbleScale.z, MTXMODE_APPLY);
        Matrix_RotateZF(-this->wobbleRot.z, MTXMODE_APPLY);
        Matrix_RotateXFApply(-this->wobbleRot.x);
    }

    MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx);
    gSPDisplayList(POLY_XLU_DISP++, gChuchuBodyDL);

    if (this->iceBlockTimer == ICE_BLOCK_UNUSED) {
        // Ice block is not active
        Scene_SetRenderModeXlu(play, 0, 1);

        gSPSegment(POLY_OPA_DISP++, 0x09, sEyeTextures[this->eyeTexIndex]);
        gDPSetEnvColor(POLY_OPA_DISP++, 0, 30, 70, 255);
        MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx);
        gSPDisplayList(POLY_OPA_DISP++, gChuchuEyesDL);

    } else {
        Scene_SetRenderModeXlu(play, 1, 2);
        gSPSegment(POLY_XLU_DISP++, 0x09, sEyeTextures[this->eyeTexIndex]);
        MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx);
        gSPDisplayList(POLY_XLU_DISP++, gChuchuEyesDL);
    }

    for (i = 0; i < EN_SLIME_BODYPART_MAX; i++) {
        Matrix_MultVec3f(&sBodyPartPosOffsets[i], &this->bodyPartsPos[i]);
    }

    if (this->actionFunc == EnSlime_Revive) {
        FrameInterpolation_RecordOpenChild(this, (uintptr_t)EnSlime_Revive);
        FrameInterpolation_IgnoreActorMtx();

        Matrix_Translate(this->actor.world.pos.x, this->actor.world.pos.y, this->actor.world.pos.z, MTXMODE_NEW);
        Matrix_RotateYS(this->reviveRotY, MTXMODE_APPLY);
        Matrix_Scale(this->reviveScale.x, this->reviveScale.y, this->reviveScale.z, MTXMODE_APPLY);

        MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx);
        gSPDisplayList(POLY_XLU_DISP++, gChuchuPuddleDL);

        FrameInterpolation_RecordCloseChild();
    }

    if ((this->actor.params != EN_SLIME_TYPE_BLUE) && (this->actor.scale.x > 0.0f)) {
        POLY_OPA_DISP = Play_SetFog(play, POLY_OPA_DISP);
        POLY_OPA_DISP = Gfx_SetupDL66(POLY_OPA_DISP);

        Matrix_Translate(this->actor.world.pos.x, this->actor.world.pos.y + (2000.0f * this->actor.scale.y),
                         this->actor.world.pos.z, MTXMODE_NEW);

        if (GameInteractor_Should(VB_DRAW_SLIME_RANDO_ITEM, true, this)) {
            if (GameInteractor_Should(VB_DRAW_SLIME_BODY_ITEM, true, this)) {
                Matrix_Scale(0.03f, 0.03f, 0.03f, MTXMODE_APPLY);

                gSPSegment(POLY_OPA_DISP++, 8, (uintptr_t)this->itemDropTex);
                MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx);
                gSPDisplayList(POLY_OPA_DISP++, gItemDropDL);
            }
        }
    }

    Actor_DrawDamageEffects(play, &this->actor, this->bodyPartsPos, EN_SLIME_BODYPART_MAX, this->drawDmgEffScale,
                            this->drawDmgEffFrozenSteamScale, this->drawDmgEffAlpha, this->drawDmgEffType);

    CLOSE_DISPS(play->state.gfxCtx);
}
