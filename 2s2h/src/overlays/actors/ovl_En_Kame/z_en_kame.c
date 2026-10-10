/*
 * File: z_en_kame.c
 * Overlay: ovl_En_Kame
 * Description: Snapper
 */

#include "z_en_kame.h"
#include "overlays/actors/ovl_En_Clear_Tag/z_en_clear_tag.h"

#define FLAGS (ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_HOSTILE | ACTOR_FLAG_HOOKSHOT_PULLS_PLAYER)

void EnKame_Init(Actor* thisx, PlayState* play);
void EnKame_Destroy(Actor* thisx, PlayState* play);
void EnKame_Update(Actor* thisx, PlayState* play);
void EnKame_Draw(Actor* thisx, PlayState* play);

void EnKame_SetupIdle(EnKame* this);
void EnKame_Idle(EnKame* this, PlayState* play);
void EnKame_SetupWalk(EnKame* this);
void EnKame_Walk(EnKame* this, PlayState* play);
void EnKame_SetupRetreatIntoShell(EnKame* this);
void EnKame_RetreatIntoShell(EnKame* this, PlayState* play);
void EnKame_SetupPrepareToAttack(EnKame* this);
void EnKame_PrepareToAttack(EnKame* this, PlayState* play);
void EnKame_SetupAttack(EnKame* this);
void EnKame_Attack(EnKame* this, PlayState* play);
void EnKame_SetupEmergeFromShell(EnKame* this);
void EnKame_EmergeFromShell(EnKame* this, PlayState* play);
void EnKame_Flip(EnKame* this, PlayState* play);
void EnKame_SetupStruggle(EnKame* this);
void EnKame_Struggle(EnKame* this, PlayState* play);
void EnKame_SetupFlipUpright(EnKame* this);
void EnKame_FlipUpright(EnKame* this, PlayState* play);
void EnKame_Stunned(EnKame* this, PlayState* play);
void EnKame_Damaged(EnKame* this, PlayState* play);
void EnKame_SetupDead(EnKame* this, PlayState* play);
void EnKame_Dead(EnKame* this, PlayState* play);
void EnKame_SetupBurstIntoFlames(EnKame* this);
void EnKame_BurstIntoFlames(EnKame* this, PlayState* play);
void EnKame_DrawSpikedSnapper(Actor* thisx, PlayState* play);

// This enum is used for when the Snapper is forcibly flipped
// by one of the player's attacks. It is *not* used for when
// the Snapper flips itself upright.
typedef enum {
    /* 0 */ EN_KAME_FLIP_TYPE_UPSIDE_DOWN,
    /* 1 */ EN_KAME_FLIP_TYPE_RIGHTSIDE_UP
} EnKameFlipType;

typedef enum {
    /* 0 */ EN_KAME_EYE_OPEN,
    /* 1 */ EN_KAME_EYE_HALF_CLOSING,
    /* 2 */ EN_KAME_EYE_CLOSED,
    /* 3 */ EN_KAME_EYE_HALF_OPENING,
    /* 4 */ EN_KAME_EYE_MAX
} EnKameEyeTexture;

ActorProfile En_Kame_Profile = {
    /**/ ACTOR_EN_KAME,
    /**/ ACTORCAT_ENEMY,
    /**/ FLAGS,
    /**/ OBJECT_TL,
    /**/ sizeof(EnKame),
    /**/ EnKame_Init,
    /**/ EnKame_Destroy,
    /**/ EnKame_Update,
    /**/ EnKame_Draw,
};

static ColliderCylinderInit sCylinderInit = {
    {
        COL_MATERIAL_HARD,
        AT_NONE | AT_TYPE_ENEMY,
        AC_ON | AC_HARD | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_1,
        COLSHAPE_CYLINDER,
    },
    {
        ELEM_MATERIAL_UNK0,
        { 0xF7CFFFFF, 0x00, 0x04 },
        { 0xF7CF7FFF, 0x00, 0x00 },
        ATELEM_ON | ATELEM_SFX_NORMAL,
        ACELEM_ON | ACELEM_HOOKABLE,
        OCELEM_ON,
    },
    { 35, 40, 0, { 0, 0, 0 } },
};

static CollisionCheckInfoInit sColChkInfoInit = { 3, 15, 30, 80 };

typedef enum {
    /* 0x0 */ EN_KAME_DMGEFF_NONE,           // Has no special effect
    /* 0x1 */ EN_KAME_DMGEFF_STUN,           // Cannot damage the Snapper, but stuns it for 40 frames
    /* 0x2 */ EN_KAME_DMGEFF_FIRE,           // Sets the Snapper on fire
    /* 0x3 */ EN_KAME_DMGEFF_FREEZE,         // Stuns the Snapper for 80 frames and surrounds it with ice
    /* 0x4 */ EN_KAME_DMGEFF_LIGHT_ORB,      // Surrounds the snapper with light orbs
    /* 0x5 */ EN_KAME_DMGEFF_ELECTRIC_STUN,  // Similar to EN_KAME_DMGEFF_STUN, but also applies an electric effect
    /* 0xD */ EN_KAME_DMGEFF_HOOKSHOT = 0xD, // Cannot damage the Snapper, but pulls the player towards it
    /* 0xE */ EN_KAME_DMGEFF_INSTANT_KILL,   // Instantly kills the Snapper if it hits, even when it is rightside-up
    /* 0xF */ EN_KAME_DMGEFF_FLIP_OVER       // Flips over Snappers if they are rightside-up
} EnKameDamageEffect;

static DamageTable sDamageTable = {
    /* Deku Nut       */ DMG_ENTRY(0, EN_KAME_DMGEFF_STUN),
    /* Deku Stick     */ DMG_ENTRY(1, EN_KAME_DMGEFF_NONE),
    /* Horse trample  */ DMG_ENTRY(1, EN_KAME_DMGEFF_NONE),
    /* Explosives     */ DMG_ENTRY(1, EN_KAME_DMGEFF_FLIP_OVER),
    /* Zora boomerang */ DMG_ENTRY(1, EN_KAME_DMGEFF_NONE),
    /* Normal arrow   */ DMG_ENTRY(1, EN_KAME_DMGEFF_NONE),
    /* UNK_DMG_0x06   */ DMG_ENTRY(0, EN_KAME_DMGEFF_NONE),
    /* Hookshot       */ DMG_ENTRY(0, EN_KAME_DMGEFF_HOOKSHOT),
    /* Goron punch    */ DMG_ENTRY(1, EN_KAME_DMGEFF_NONE),
    /* Sword          */ DMG_ENTRY(1, EN_KAME_DMGEFF_NONE),
    /* Goron pound    */ DMG_ENTRY(0, EN_KAME_DMGEFF_FLIP_OVER),
    /* Fire arrow     */ DMG_ENTRY(2, EN_KAME_DMGEFF_FIRE),
    /* Ice arrow      */ DMG_ENTRY(2, EN_KAME_DMGEFF_FREEZE),
    /* Light arrow    */ DMG_ENTRY(2, EN_KAME_DMGEFF_LIGHT_ORB),
    /* Goron spikes   */ DMG_ENTRY(1, EN_KAME_DMGEFF_NONE),
    /* Deku spin      */ DMG_ENTRY(0, EN_KAME_DMGEFF_STUN),
    /* Deku bubble    */ DMG_ENTRY(1, EN_KAME_DMGEFF_NONE),
    /* Deku launch    */ DMG_ENTRY(2, EN_KAME_DMGEFF_INSTANT_KILL),
    /* UNK_DMG_0x12   */ DMG_ENTRY(0, EN_KAME_DMGEFF_STUN),
    /* Zora barrier   */ DMG_ENTRY(0, EN_KAME_DMGEFF_ELECTRIC_STUN),
    /* Normal shield  */ DMG_ENTRY(0, EN_KAME_DMGEFF_NONE),
    /* Light ray      */ DMG_ENTRY(0, EN_KAME_DMGEFF_NONE),
    /* Thrown object  */ DMG_ENTRY(1, EN_KAME_DMGEFF_NONE),
    /* Zora punch     */ DMG_ENTRY(1, EN_KAME_DMGEFF_NONE),
    /* Spin attack    */ DMG_ENTRY(1, EN_KAME_DMGEFF_NONE),
    /* Sword beam     */ DMG_ENTRY(0, EN_KAME_DMGEFF_NONE),
    /* Normal Roll    */ DMG_ENTRY(0, EN_KAME_DMGEFF_NONE),
    /* UNK_DMG_0x1B   */ DMG_ENTRY(0, EN_KAME_DMGEFF_NONE),
    /* UNK_DMG_0x1C   */ DMG_ENTRY(0, EN_KAME_DMGEFF_NONE),
    /* Unblockable    */ DMG_ENTRY(0, EN_KAME_DMGEFF_NONE),
    /* UNK_DMG_0x1E   */ DMG_ENTRY(0, EN_KAME_DMGEFF_NONE),
    /* Powder Keg     */ DMG_ENTRY(1, EN_KAME_DMGEFF_FLIP_OVER),
};

static TexturePtr sEyeTextures[] = { gSnapperEyeOpenTex, gSnapperEyeHalfTex, gSnapperEyeClosedTex, gSnapperEyeHalfTex };

static InitChainEntry sInitChain[] = {
    ICHAIN_S8(hintId, TATL_HINT_ID_SNAPPER, ICHAIN_CONTINUE),
    ICHAIN_F32(gravity, -1, ICHAIN_CONTINUE),
    ICHAIN_F32(lockOnArrowOffset, 3500, ICHAIN_STOP),
};

static s32 sTexturesDesegmented = false;

void EnKame_Init(Actor* thisx, PlayState* play) {
    EnKame* this = (EnKame*)thisx;

    Actor_ProcessInitChain(&this->actor, sInitChain);
    SkelAnime_InitFlex(play, &this->snapperSkelAnime, &gSnapperSkel, &gSnapperIdleAnim, this->snapperJointTable,
                       this->snapperMorphTable, SNAPPER_LIMB_MAX);
    SkelAnime_InitFlex(play, &this->spikedSnapperSkelAnime, &gSpikedSnapperSkel, &gSpikedSnapperIdleAnim,
                       this->spikedSnapperJointTable, this->spikedSnapperMorphTable, SPIKED_SNAPPER_LIMB_MAX);
    ActorShape_Init(&this->actor.shape, 0.0f, ActorShadow_DrawCircle, 55.0f);
    Collider_InitAndSetCylinder(play, &this->collider, &this->actor, &sCylinderInit);
    CollisionCheck_SetInfo(&this->actor.colChkInfo, &sDamageTable, &sColChkInfoInit);

    if (!sTexturesDesegmented) {
        s32 i;

        for (i = 0; i < ARRAY_COUNT(sEyeTextures); i++) {
            sEyeTextures[i] = Lib_SegmentedToVirtual(sEyeTextures[i]);
        }

        sTexturesDesegmented = true;
    }

    EnKame_SetupIdle(this);
}

void EnKame_Destroy(Actor* thisx, PlayState* play) {
    EnKame* this = (EnKame*)thisx;

    Collider_DestroyCylinder(play, &this->collider);
}

void EnKame_Blink(EnKame* this) {
    if (this->eyeIndex != EN_KAME_EYE_OPEN) {
        this->eyeIndex++;
        if (this->eyeIndex == EN_KAME_EYE_MAX) {
            this->eyeIndex = EN_KAME_EYE_OPEN;
        }
    } else if (Rand_ZeroOne() < 0.05f) {
        this->eyeIndex = EN_KAME_EYE_HALF_CLOSING;
    }
}

void EnKame_Freeze(EnKame* this) {
    this->drawDmgEffType = ACTOR_DRAW_DMGEFF_FROZEN_NO_SFX;
    this->drawDmgEffScale = 0.6f;
    this->drawDmgEffFrozenSteamScale = 900.0f * 0.001f;
    this->drawDmgEffAlpha = 1.0f;
    this->collider.base.colMaterial = COL_MATERIAL_HIT3;
    this->stunTimer = 80;
    this->actor.flags &= ~ACTOR_FLAG_HOOKSHOT_PULLS_PLAYER;
    Actor_SetColorFilter(&this->actor, COLORFILTER_COLORFLAG_RED, 255, COLORFILTER_BUFFLAG_OPA, 80);
}

void EnKame_Thaw(EnKame* this, PlayState* play) {
    if (this->drawDmgEffType == ACTOR_DRAW_DMGEFF_FROZEN_NO_SFX) {
        this->drawDmgEffType = ACTOR_DRAW_DMGEFF_FIRE;
        this->collider.base.colMaterial = COL_MATERIAL_HIT6;
        this->drawDmgEffAlpha = 0.0f;
        Actor_SpawnIceEffects(play, &this->actor, this->bodyPartsPos, SNAPPER_BODYPART_MAX, 2, 0.3f, 0.2f);
        this->actor.flags |= ACTOR_FLAG_HOOKSHOT_PULLS_PLAYER;
    }
}

void EnKame_SetupIdle(EnKame* this) {
    Animation_MorphToPlayOnce(&this->snapperSkelAnime, &gSnapperIdleAnim, -5.0f);
    this->actor.speed = 0.0f;
    this->actionFunc = EnKame_Idle;
}

/**
 * Plays the idle animation, then starts walking once the animation is over. If the player gets close enough to
 * the Snapper without the Stone Mask equipped, it will retreat into its shell to begin attacking them.
 */
void EnKame_Idle(EnKame* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    if (Animation_OnFrame(&this->snapperSkelAnime, 10.0f)) {
        Actor_PlaySfx(&this->actor, NA_SE_EN_PAMET_VOICE);
        this->voiceTimer = 40;
    }

    if ((Player_GetMask(play) != PLAYER_MASK_STONE) && !(player->stateFlags1 & PLAYER_STATE1_800000) &&
        (this->actor.xzDistToPlayer < 240.0f)) {
        EnKame_SetupRetreatIntoShell(this);
    } else if (SkelAnime_Update(&this->snapperSkelAnime)) {
        EnKame_SetupWalk(this);
    }
}

void EnKame_SetupWalk(EnKame* this) {
    Animation_MorphToLoop(&this->snapperSkelAnime, &gSnapperWalkAnim, -5.0f);
    this->actor.speed = 0.5f;
    this->timer = Animation_GetLastFrame(&gSnapperWalkAnim) * ((s32)Rand_ZeroFloat(5.0f) + 3);
    this->targetRotY = this->actor.shape.rot.y;
    this->collider.base.acFlags |= (AC_HARD | AC_ON);
    this->collider.base.colMaterial = COL_MATERIAL_HARD;
    this->actionFunc = EnKame_Walk;
}

/**
 * Walk semi-randomly around the Snapper's home until the timer reaches 0, then transition back to being idle.
 * If the player gets close enough to the Snapper without the Stone Mask equipped, it will retreat into its
 * shell to begin attacking them.
 */
void EnKame_Walk(EnKame* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    if ((Player_GetMask(play) != PLAYER_MASK_STONE) && !(player->stateFlags1 & PLAYER_STATE1_800000) &&
        (this->actor.xzDistToPlayer < 240.0f)) {
        EnKame_SetupRetreatIntoShell(this);
        return;
    }

    SkelAnime_Update(&this->snapperSkelAnime);

    if (this->targetRotY != this->actor.shape.rot.y) {
        Math_ScaledStepToS(&this->actor.shape.rot.y, this->targetRotY, 0x100);
        this->actor.world.rot.y = this->actor.shape.rot.y;
    } else if (Actor_WorldDistXZToPoint(&this->actor, &this->actor.home.pos) > 40.0f) {
        this->targetRotY = Actor_WorldYawTowardPoint(&this->actor, &this->actor.home.pos) + ((s32)Rand_Next() >> 0x14);
    }

    this->timer--;
    if (this->timer == 0) {
        EnKame_SetupIdle(this);
    } else if (Animation_OnFrame(&this->snapperSkelAnime, 0.0f) || Animation_OnFrame(&this->snapperSkelAnime, 15.0f)) {
        Actor_PlaySfx(&this->actor, NA_SE_EN_PAMET_WALK);
    }
}

void EnKame_SetupRetreatIntoShell(EnKame* this) {
    Animation_MorphToPlayOnce(&this->snapperSkelAnime, &gSnapperRetreatIntoShellAnim, -3.0f);
    this->timer = 0;
    this->limbScaleY = 1.0f;
    this->limbScaleXZ = 1.0f;
    this->actor.speed = 0.0f;

    // If the Snapper recently played this same sound effect while it was idle, don't play it again.
    if (this->voiceTimer == 0) {
        Actor_PlaySfx(&this->actor, NA_SE_EN_PAMET_VOICE);
    }

    this->actionFunc = EnKame_RetreatIntoShell;
}

/**
 * Plays the animation of the Snapper retreating its shell, and scales its limbs appropriately to
 * make it appear like its head and legs are going into the shell. Once the animation is over, the
 * Snapper will either prepare for an attack (if the player is close enough and not wearing the
 * Stone Mask), or it will emerge from its shell.
 */
void EnKame_RetreatIntoShell(EnKame* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    if (SkelAnime_Update(&this->snapperSkelAnime)) {
        if ((Player_GetMask(play) != PLAYER_MASK_STONE) && !(player->stateFlags1 & PLAYER_STATE1_800000) &&
            ((this->timer == 0) || (this->actor.xzDistToPlayer < 120.0f))) {
            EnKame_SetupPrepareToAttack(this);
        } else {
            this->timer--;
            if (this->timer == 0) {
                EnKame_SetupEmergeFromShell(this);
            }
        }
    } else if (this->snapperSkelAnime.curFrame > 2.0f) {
        this->limbScaleY = 1.5f - ((this->snapperSkelAnime.curFrame - 2.0f) * (7.0f / 30));
        this->limbScaleXZ = 1.5f - ((this->snapperSkelAnime.curFrame - 2.0f) * (1.0f / 12));
    } else {
        f32 curFrame = this->snapperSkelAnime.curFrame;

        this->limbScaleY = (0.25f * curFrame) + 1.0f;
        this->limbScaleXZ = (0.25f * curFrame) + 1.0f;
    }
}

/**
 * Sets the Snapper's speed and z-rotation based on its angular velocity. When the Snapper is done with the first
 * part of its attack, it will slowly "wind up" before moving back to its home; by setting the speed to be a multiple
 * of its angular velocity, the Snapper will smootly ramp up its speed during the "wind up". As for the z-rotation,
 * setting it in this manner gives the Snapper a subtle, almost-imperceptible tilt as it spins.
 */
void EnKame_SetAttackSpeed(EnKame* this) {
    this->actor.speed = this->angularVelocityY * (5.0f / 0x1D80);
    this->actor.shape.rot.z = this->angularVelocityY * (13.0f / 118.0f);
}

/**
 * If the Snapper is moving quickly enough over certain surface materials, this function will spawn dirt or snow-colored
 * dust on even-numbered frames. Additionally, this function is also responsible for playing the Snapper's spinning
 * sound effect if its angular velocity is large enough.
 */
void EnKame_ProcessAttackEffects(EnKame* this, PlayState* play) {
    static Color_RGBA8 sSnowPrimColor = { 250, 250, 250, 255 };
    static Color_RGBA8 sSnowEnvColor = { 180, 180, 180, 255 };
    static Vec3f sVelocity = { 0.0f, 0.75f, 0.0f };

    if ((this->actor.bgCheckFlags & BGCHECKFLAG_GROUND) && (this->actor.speed >= 3.0f)) {
        if ((play->gameplayFrames % 2) == 0) {
            SurfaceMaterial surfaceMaterial =
                SurfaceType_GetMaterial(&play->colCtx, this->actor.floorPoly, this->actor.floorBgId);

            if ((surfaceMaterial == SURFACE_MATERIAL_DIRT) || (surfaceMaterial == SURFACE_MATERIAL_SAND)) {
                func_800B1210(play, &this->actor.world.pos, &sVelocity, &gZeroVec3f, 550, 100);
            } else if (surfaceMaterial == SURFACE_MATERIAL_SNOW) {
                func_800B0DE0(play, &this->actor.world.pos, &sVelocity, &gZeroVec3f, &sSnowPrimColor, &sSnowEnvColor,
                              550, 100);
            }
        }
    }

    if (this->angularVelocityY > 0x1200) {
        Actor_PlaySfx_Flagged(&this->actor, NA_SE_EN_PAMET_ROLL - SFX_FLAG);
    }
}

void EnKame_SetupPrepareToAttack(EnKame* this) {
    // The Snapper's attack can be divided into roughly two phases:
    // - Charging towards the player
    // - Going back to their home
    // This if-statement is checking which of the two is the current phase. If the Snapper is in the first
    // phase of the attack, then it just finished retreating into its shell, and it hasn't switched to its
    // "spiked" form yet. Thus, we check the draw function here to determine which phase the Snapper is in.
    if (this->actor.draw == EnKame_Draw) {
        this->actor.draw = EnKame_DrawSpikedSnapper;
        this->angularVelocityY = 0x3B00;
        this->spikesScale = 0.5f;
        EnKame_SetAttackSpeed(this);
        this->timer = 15;
        this->actor.speed = 0.0f;
        Actor_PlaySfx(&this->actor, NA_SE_EN_PAMET_CUTTER_ON);
        // As stated above, we could tell this was the first phase of the Snapper's attack because it hadn't
        // changed its draw function to the "spiked" version yet. However, we won't be able to check the draw
        // function in the future, because we just changed it; we need some other way to tell which phase of
        // the attack the Snapper is in. Setting the targetPos's y-position here to something less than the
        // home's y-position will be used as a signal in future functions to indicate this is the first phase
        // of the attack. Nothing looks at the y-position of the targetPos, so we can use it however we want,
        // and during the second phase of the attack, the Snapper's home position is copied into targetPos,
        // including the y-position; the targetPos's y-position will *only* differ from the home's y-position
        // during the first phase of the attack due to the code below.
        this->targetPos.y = this->actor.home.pos.y - 100.0f;
    } else {
        this->actor.world.rot.y = Actor_WorldYawTowardPoint(&this->actor, &this->actor.home.pos);
        Math_Vec3f_Copy(&this->targetPos, &this->actor.home.pos);
        this->timer = 0;
    }

    this->actor.flags |= ACTOR_FLAG_UPDATE_CULLING_DISABLED;
    this->actionFunc = EnKame_PrepareToAttack;
}

/**
 * This function gradually increases the angular velocity to 0x3B00 and does a few other miscellaneous tasks (e.g.,
 * scaling the spikes up and making the Snapper face the player for the first phase of the attack) before attacking.
 */
void EnKame_PrepareToAttack(EnKame* this, PlayState* play) {
    // In EnKame_SetupPrepareToAttack, the timer is set to 15 if it was the first phase of the attack, so this is yet
    // another way of checking the current phase. Gradually scale the spikes up, and once they reach their full size,
    // enable AT collisions so the Snapper can damage the player.
    if (this->timer == 15) {
        this->spikesScale += 0.2f;
        if (this->spikesScale > 1.1f) {
            this->timer--;
            this->collider.base.atFlags |= AT_ON;
            this->spikesScale = 1.0f;
        }
    } else if (this->timer > 0) {
        this->timer--;
    } else if (Math_ScaledStepToS(&this->angularVelocityY, 0x3B00, (s32)(this->angularVelocityY * 0.09f) + 45)) {
        // If it's the first phase of the attack, rotate the Snapper to face the player and set the targetPos to a
        // fixed position in that direction. This will make them charge towards the player once they start moving.
        if (this->targetPos.y < this->actor.home.pos.y) {
            this->actor.world.rot.y = this->actor.yawTowardsPlayer;
            this->targetPos.x = (Math_SinS(this->actor.world.rot.y) * 360.0f) + this->actor.world.pos.x;
            this->targetPos.z = (Math_CosS(this->actor.world.rot.y) * 360.0f) + this->actor.world.pos.z;
        }

        EnKame_SetupAttack(this);
    }

    this->actor.shape.rot.y += this->angularVelocityY;

    // Calling EnKame_SetAttackSpeed will set the Snapper's speed based on its current angular velocity; since we are
    // gradually scaling up the angular velocity, this would gradually scale up the speed as well. However, during the
    // first phase of the attack, the Snapper should wait in place until it reaches its maximum angular velocity. This
    // check will prevent us from changing the Snapper's speed during the first phase until we're just about to attack.
    if (!(this->targetPos.y < this->actor.home.pos.y) || (this->actionFunc != EnKame_PrepareToAttack)) {
        EnKame_SetAttackSpeed(this);
    }

    EnKame_ProcessAttackEffects(this, play);
}

void EnKame_SetupAttack(EnKame* this) {
    this->targetAngularVelocityY = -1;
    this->actionFunc = EnKame_Attack;
}

/**
 * Moves the Snapper towards its target while spinning until it hits a wall or reaches the target. Once that happens,
 * the Snapper will either reduce its angular velocity to 0x300 and prepare for the second phase of the attack (if the
 * Snapper is currently in the first phase), or it will reduce its angular velocity to 0 and emerge from its shell (if
 * the Snapper is currently in the second phase).
 */
void EnKame_Attack(EnKame* this, PlayState* play) {
    this->actor.shape.rot.y += this->angularVelocityY;
    EnKame_ProcessAttackEffects(this, play);

    if (this->targetAngularVelocityY == -1) {
        s32 absYawToTarget;
        s16 yawToTarget = Actor_WorldYawTowardPoint(&this->actor, &this->targetPos) - this->actor.world.rot.y;

        absYawToTarget = ABS_ALT(yawToTarget);

        // If the yaw toward the targetPos is too large, that indicates the Snapper probably moved
        // beyond its target (in other words, the target is now "behind" the Snapper).
        if ((this->actor.bgCheckFlags & BGCHECKFLAG_WALL) || (absYawToTarget > 0x3000) ||
            (Actor_WorldDistXZToPoint(&this->actor, &this->targetPos) < 50.0f)) {
            s8 pad;

            if (this->targetPos.y < this->actor.home.pos.y) {
                this->targetAngularVelocityY = 0x300;
            } else {
                this->targetAngularVelocityY = 0;
            }
        }
        return;
    }

    if (Math_ScaledStepToS(&this->angularVelocityY, this->targetAngularVelocityY,
                           (s32)(this->angularVelocityY * 0.09f) + 45)) {
        if (this->targetAngularVelocityY == 0) {
            if (this->spikesScale >= 1.0f) {
                Actor_PlaySfx(&this->actor, NA_SE_EN_PAMET_CUTTER_OFF);
            }

            this->spikesScale -= 0.1f;
            this->collider.base.atFlags &= ~AT_ON;
            if (this->spikesScale < 0.5f) {
                this->actor.flags &= ~ACTOR_FLAG_UPDATE_CULLING_DISABLED;
                EnKame_SetupEmergeFromShell(this);
            }
        } else {
            EnKame_SetupPrepareToAttack(this);
        }
    }

    EnKame_SetAttackSpeed(this);
}

void EnKame_SetupEmergeFromShell(EnKame* this) {
    this->actor.draw = EnKame_Draw;
    Animation_MorphToPlayOnce(&this->snapperSkelAnime, &gSnapperEmergeFromShellAnim, -3.0f);
    this->actor.speed = 0.0f;
    this->limbScaleY = 0.1f;
    this->limbScaleXZ = 1.0f;
    this->actor.world.rot.y = this->actor.shape.rot.y;
    this->actionFunc = EnKame_EmergeFromShell;
}

/**
 * Plays the animation of the Snapper emerging from its shell, and scales its limbs appropriately to
 * make it appear like its head and legs are coming out of the shell. Once the animation is over, this
 * will make the Snapper start walking again.
 */
void EnKame_EmergeFromShell(EnKame* this, PlayState* play) {
    if (SkelAnime_Update(&this->snapperSkelAnime)) {
        EnKame_SetupWalk(this);
    } else if (this->snapperSkelAnime.curFrame > 7.0f) {
        this->limbScaleY = 1.5f - ((this->snapperSkelAnime.curFrame - 7.0f) * (1.0f / 6));
        this->limbScaleXZ = 1.5f - ((this->snapperSkelAnime.curFrame - 7.0f) * (1.0f / 6));
    } else {
        f32 curFrame = this->snapperSkelAnime.curFrame;

        this->limbScaleY = (0.2f * curFrame) + 0.1f;
        this->limbScaleXZ = ((1.0f / 14) * curFrame) + 1.0f;
    }
}

void EnKame_SetupFlip(EnKame* this) {
    if (this->actionFunc == EnKame_Struggle) {
        Animation_MorphToPlayOnce(&this->snapperSkelAnime, &gSnapperBouncedUprightAnim, -3.0f);
        this->flipType = EN_KAME_FLIP_TYPE_RIGHTSIDE_UP;
        this->collider.elem.acDmgInfo.dmgFlags &= ~0x8000;
    } else {
        Animation_MorphToPlayOnce(&this->snapperSkelAnime, &gSnapperFlipOverAnim, -3.0f);
        this->flipType = EN_KAME_FLIP_TYPE_UPSIDE_DOWN;
        this->collider.elem.acDmgInfo.dmgFlags |= 0x8000;
    }

    this->actor.draw = EnKame_Draw;
    this->actor.speed = 0.0f;
    this->collider.base.acFlags &= ~AC_ON;
    this->collider.base.atFlags &= ~AT_ON;
    this->collider.base.atFlags &= ~(AT_BOUNCED | AT_HIT);
    this->actor.flags &= ~ACTOR_FLAG_UPDATE_CULLING_DISABLED;
    this->actor.shape.rot.z = 0;
    Actor_PlaySfx(&this->actor, NA_SE_EN_PAMET_REVERSE);
    this->actionFunc = EnKame_Flip;
}

/**
 * If the Snapper is flipped upside-down, this will make it struggle for 200 frames once the flip animation ends.
 * If the Snapper is flipped rightside-up, this will make it start walking once the flip animation ends.
 */
void EnKame_Flip(EnKame* this, PlayState* play) {
    if (SkelAnime_Update(&this->snapperSkelAnime)) {
        if (this->flipType == EN_KAME_FLIP_TYPE_RIGHTSIDE_UP) {
            EnKame_SetupWalk(this);
        } else {
            this->timer = 200;
            EnKame_SetupStruggle(this);
        }
    }
}

void EnKame_SetupStruggle(EnKame* this) {
    Animation_MorphToPlayOnce(&this->snapperSkelAnime, &gSnapperWiggleLegsAnim, -3.0f);
    this->collider.base.acFlags |= AC_ON;
    this->collider.base.acFlags &= ~AC_HARD;
    this->collider.base.colMaterial = COL_MATERIAL_HIT6;
    this->actor.speed = 0.0f;
    this->actionFunc = EnKame_Struggle;
}

/**
 * Plays various struggling animations until the timer reaches 0, then it flips itself upright.
 */
void EnKame_Struggle(EnKame* this, PlayState* play) {
    if (this->timer > 0) {
        this->timer--;
        if (SkelAnime_Update(&this->snapperSkelAnime)) {
            if (Rand_ZeroOne() > 0.5f) {
                Animation_PlayOnce(&this->snapperSkelAnime, &gSnapperWiggleLegsAnim);
            } else {
                Animation_PlayOnce(&this->snapperSkelAnime, &gSnapperFailToFlipUprightAnim);
                Actor_PlaySfx(&this->actor, NA_SE_EN_PAMET_ROAR);
            }
        }
    } else {
        EnKame_SetupFlipUpright(this);
    }
}

void EnKame_SetupFlipUpright(EnKame* this) {
    Animation_MorphToPlayOnce(&this->snapperSkelAnime, &gSnapperFlipUprightAnim, -3.0f);
    Actor_PlaySfx(&this->actor, NA_SE_EN_PAMET_WAKEUP);
    this->actionFunc = EnKame_FlipUpright;
}

/**
 * Plays the animation of the Snapper flipping itself upright, then transitions back to walking.
 */
void EnKame_FlipUpright(EnKame* this, PlayState* play) {
    if (SkelAnime_Update(&this->snapperSkelAnime)) {
        this->actor.shape.shadowDraw = ActorShadow_DrawCircle;
        EnKame_SetupWalk(this);
    } else if (this->snapperSkelAnime.curFrame >= 10.0f) {
        // This is the part of the animation where the Snapper starts to flip itself
        // upright. During this flip, this actor needs to manually control when
        // ActorShadow_DrawCircle is called, so the shadowDraw pointer is set to NULL.
        // See EnKame_PostLimbDraw and EnKame_Draw for more information.
        this->actor.shape.shadowDraw = NULL;
        this->collider.base.acFlags &= ~AC_ON;
        this->collider.elem.acDmgInfo.dmgFlags &= ~0x8000;
    }
}

void EnKame_SetupStunned(EnKame* this) {
    this->actor.speed = 0.0f;
    if (this->actor.velocity.y > 0.0f) {
        this->actor.velocity.y = 0.0f;
    }

    func_800BE504(&this->actor, &this->collider);
    this->actionFunc = EnKame_Stunned;
}

/**
 * Stops the Snapper's animation and locks it in place until the stun timer reaches zero. After that, the
 * Snapper either dies (if the attack that stunned it reduced its health to 0) or goes back to struggling.
 * If the Snapper dies in this state, it skips the "finishing blow" screen flash and game freeze.
 */
void EnKame_Stunned(EnKame* this, PlayState* play) {
    if (this->stunTimer != 0) {
        this->stunTimer--;
    }

    if (this->stunTimer == 0) {
        EnKame_Thaw(this, play);
        if (this->actor.colChkInfo.health == 0) {
            EnKame_SetupDead(this, NULL);
        } else {
            this->actor.world.rot.y = this->actor.shape.rot.y;
            EnKame_SetupStruggle(this);
        }
    }
}

void EnKame_SetupDamaged(EnKame* this) {
    s16 endFrame = Animation_GetLastFrame(&gSnapperDamageAnim);

    Animation_Change(&this->snapperSkelAnime, &gSnapperDamageAnim, 1.0f, 0.0f, endFrame, ANIMMODE_ONCE, -3.0f);
    Actor_SetColorFilter(&this->actor, COLORFILTER_COLORFLAG_RED, 255, COLORFILTER_BUFFLAG_OPA, endFrame);
    Actor_PlaySfx(&this->actor, NA_SE_EN_PAMET_DAMAGE);
    this->collider.base.acFlags &= ~AC_ON;
    this->actionFunc = EnKame_Damaged;
}

/**
 * Plays the damaged animation, then goes back to struggling to flip itself upright.
 */
void EnKame_Damaged(EnKame* this, PlayState* play) {
    if (SkelAnime_Update(&this->snapperSkelAnime)) {
        EnKame_SetupStruggle(this);
    }
}

void EnKame_SetupDead(EnKame* this, PlayState* play) {
    Animation_PlayLoop(&this->snapperSkelAnime, &gSnapperDeathAnim);
    Actor_SetColorFilter(&this->actor, COLORFILTER_COLORFLAG_RED, 255, COLORFILTER_BUFFLAG_OPA, 20);
    this->collider.base.acFlags &= ~AC_ON;
    this->collider.base.atFlags &= ~AT_ON;
    this->collider.base.atFlags &= ~(AC_HARD | AC_HIT);
    this->actor.velocity.y = 15.0f;
    this->actor.speed = 1.5f;

    // This allows callers of this function to skip the "finishing blow" screen flash and game freeze as well
    // as the rotation that happens if the Snapper is killed by a projectile. Only EnKame_Stunned calls this
    // while passing NULL instead of play; in practice, this means that the only way to skip this code is to
    // kill the Snapper with an Ice Arrow, since that's the only attack that both stuns it and deals damage.
    if (play != NULL) {
        Enemy_StartFinishingBlow(play, &this->actor);
        if (this->actor.draw == EnKame_DrawSpikedSnapper) {
            this->actor.draw = EnKame_Draw;
        } else {
            func_800BE504(&this->actor, &this->collider);
        }
    }

    this->actor.bgCheckFlags &= ~BGCHECKFLAG_GROUND;
    this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
    this->actor.flags |= ACTOR_FLAG_UPDATE_CULLING_DISABLED;
    Actor_PlaySfx(&this->actor, NA_SE_EN_PAMET_DEAD);
    this->timer = 0;
    this->actionFunc = EnKame_Dead;
}

/**
 * Launches the Snapper into the air and spins it around wildly. Makes the Snapper
 * burst into flames once it touches the ground.
 */
void EnKame_Dead(EnKame* this, PlayState* play) {
    SkelAnime_Update(&this->snapperSkelAnime);
    if ((this->actor.bgCheckFlags & BGCHECKFLAG_GROUND) && (this->actor.velocity.y < 0.0f)) {
        Actor_PlaySfx(&this->actor, NA_SE_EN_HIPLOOP_LAND);
        EnKame_SetupBurstIntoFlames(this);
    } else {
        //! @bug unreachable code: EnKame_SetupDead sets this variable to 0, and no other code that runs
        //! while the Snapper is in this state will change it to anything else.
        if (this->timer == 1) {
            this->actor.colorFilterTimer = 100;
        } else if (this->actor.colorFilterTimer == 0) {
            Actor_SetColorFilter(&this->actor, COLORFILTER_COLORFLAG_NONE, 255, COLORFILTER_BUFFLAG_OPA, 100);
        }

        this->actor.shape.rot.x += Rand_S16Offset(0x700, 0x1400);
        this->actor.shape.rot.y += TRUNCF_BINANG(Rand_ZeroFloat(0x1400));
        this->actor.shape.rot.z += Rand_S16Offset(0x700, 0x1400);
    }
}

void EnKame_SetupBurstIntoFlames(EnKame* this) {
    this->timer = 20;
    this->actor.speed = 0.0f;
    this->actionFunc = EnKame_BurstIntoFlames;
}

/**
 * After waiting for the timer to reach 0, this function gradually shrinks the actor
 * and spawns blue flames around it. Once the Snapper shrinks enough, this drops a
 * random item and kills the actor.
 */
void EnKame_BurstIntoFlames(EnKame* this, PlayState* play) {
    Vec3f flamePos;

    SkelAnime_Update(&this->snapperSkelAnime);
    this->actor.colorFilterTimer = 100;
    if (this->timer > 0) {
        this->timer--;
        if (this->timer == 0) {
            SoundSource_PlaySfxEachFrameAtFixedWorldPos(play, &this->actor.world.pos, 21,
                                                        NA_SE_EN_COMMON_EXTINCT_LEV - SFX_FLAG);
        }
    } else {
        this->actor.scale.x -= 0.001f;
        if (this->actor.scale.x <= 0.0f) {
            Item_DropCollectibleRandom(play, &this->actor, &this->actor.world.pos, 0x60);
            Actor_Kill(&this->actor);
        } else {
            this->actor.scale.y = this->actor.scale.x;
            this->actor.scale.z = this->actor.scale.x;
        }

        flamePos.x = Rand_CenteredFloat(40.0f) + this->actor.world.pos.x;
        flamePos.y = this->actor.world.pos.y + 15.0f;
        flamePos.z = Rand_CenteredFloat(40.0f) + this->actor.world.pos.z;
        func_800B3030(play, &flamePos, &gZeroVec3f, &gZeroVec3f, 100, 0, 2);
    }
}

void EnKame_UpdateDamage(EnKame* this, PlayState* play) {
    if (this->collider.base.acFlags & AC_HIT) {
        this->collider.base.acFlags &= ~AC_HIT;

        Actor_SetDropFlag(&this->actor, &this->collider.elem);
        if ((this->drawDmgEffType == ACTOR_DRAW_DMGEFF_FROZEN_NO_SFX) &&
            (this->collider.elem.acHitElem->atDmgInfo.dmgFlags & 0xDB0B3)) {
            return;
        }

        EnKame_Thaw(this, play);
        if (this->actor.colChkInfo.damageEffect == EN_KAME_DMGEFF_HOOKSHOT) {
            return;
        }

        if (this->actor.colChkInfo.damageEffect == EN_KAME_DMGEFF_INSTANT_KILL) {
            EnKame_SetupDead(this, play);
        } else if (this->actor.colChkInfo.damageEffect == EN_KAME_DMGEFF_FLIP_OVER) {
            if (this->collider.base.acFlags & AC_HARD) {
                EnKame_SetupFlip(this);
            } else if (!Actor_ApplyDamage(&this->actor)) {
                EnKame_SetupDead(this, play);
            } else {
                EnKame_SetupDamaged(this);
            }
        } else if ((this->actionFunc == EnKame_Idle) || (this->actionFunc == EnKame_Walk)) {
            EnKame_SetupRetreatIntoShell(this);
            this->timer = 21;
        } else if (!(this->collider.base.acFlags & AC_HARD)) {
            if (this->actor.colChkInfo.damageEffect == EN_KAME_DMGEFF_ELECTRIC_STUN) {
                this->stunTimer = 40;
                Actor_SetColorFilter(&this->actor, COLORFILTER_COLORFLAG_BLUE, 255, COLORFILTER_BUFFLAG_OPA, 40);
                Actor_PlaySfx(&this->actor, NA_SE_EN_COMMON_FREEZE);
                this->drawDmgEffScale = 0.6f;
                this->drawDmgEffAlpha = 2.0f;
                this->drawDmgEffType = ACTOR_DRAW_DMGEFF_ELECTRIC_SPARKS_SMALL;
                EnKame_SetupStunned(this);
            } else if (this->actor.colChkInfo.damageEffect == EN_KAME_DMGEFF_STUN) {
                this->stunTimer = 40;
                Actor_SetColorFilter(&this->actor, COLORFILTER_COLORFLAG_BLUE, 255, COLORFILTER_BUFFLAG_OPA, 40);
                Actor_PlaySfx(&this->actor, NA_SE_EN_COMMON_FREEZE);
                EnKame_SetupStunned(this);
            } else if (this->actor.colChkInfo.damageEffect == EN_KAME_DMGEFF_FREEZE) {
                EnKame_Freeze(this);
                if (!Actor_ApplyDamage(&this->actor)) {
                    this->stunTimer = 3;
                    this->collider.base.acFlags &= ~AC_ON;
                }

                EnKame_SetupStunned(this);
            } else {
                if (this->actor.colChkInfo.damageEffect == EN_KAME_DMGEFF_FIRE) {
                    this->drawDmgEffScale = 0.6f;
                    this->drawDmgEffAlpha = 4.0f;
                    this->drawDmgEffType = ACTOR_DRAW_DMGEFF_FIRE;
                } else if (this->actor.colChkInfo.damageEffect == EN_KAME_DMGEFF_LIGHT_ORB) {
                    this->drawDmgEffScale = 0.6f;
                    this->drawDmgEffAlpha = 4.0f;
                    this->drawDmgEffType = ACTOR_DRAW_DMGEFF_LIGHT_ORBS;
                    Actor_Spawn(&play->actorCtx, play, ACTOR_EN_CLEAR_TAG, this->collider.elem.acDmgInfo.hitPos.x,
                                this->collider.elem.acDmgInfo.hitPos.y, this->collider.elem.acDmgInfo.hitPos.z, 0, 0, 0,
                                CLEAR_TAG_PARAMS(CLEAR_TAG_LARGE_LIGHT_RAYS));
                }

                if (!Actor_ApplyDamage(&this->actor)) {
                    EnKame_SetupDead(this, play);
                } else {
                    EnKame_SetupDamaged(this);
                }
            }
        }
    }

    if ((play->actorCtx.unk2 != 0) && (this->actor.xyzDistToPlayerSq < SQ(200.0f)) &&
        (this->collider.base.acFlags & AC_ON)) {
        EnKame_Thaw(this, play);
        EnKame_SetupFlip(this);
    }
}

void EnKame_Update(Actor* thisx, PlayState* play) {
    s32 pad;
    EnKame* this = (EnKame*)thisx;

    EnKame_Blink(this);

    DECR(this->voiceTimer);

    EnKame_UpdateDamage(this, play);

    if ((this->collider.base.atFlags & AT_HIT) && (this->collider.base.atFlags & AT_BOUNCED)) {
        this->collider.base.atFlags &= ~(AT_BOUNCED | AT_HIT);
        EnKame_SetupPrepareToAttack(this);
        if (Actor_WorldDistXZToPoint(&this->actor, &this->targetPos) < 50.0f) {
            this->collider.base.atFlags &= ~AT_ON;
        }

        this->angularVelocityY = 0x3B00;
        EnKame_SetAttackSpeed(this);
    }

    this->actionFunc(this, play);

    Actor_MoveWithGravity(&this->actor);
    Actor_UpdateBgCheckInfo(play, &this->actor, 40.0f, 60.0f, 40.0f,
                            UPDBGCHECKINFO_FLAG_1 | UPDBGCHECKINFO_FLAG_2 | UPDBGCHECKINFO_FLAG_4 |
                                UPDBGCHECKINFO_FLAG_8 | UPDBGCHECKINFO_FLAG_10);

    // When the Snapper is in the middle of the animation where it's flipping upright,
    // setting the focus and updating the collider is handled in EnKame_Draw instead.
    if (this->actor.shape.shadowDraw != NULL) {
        Actor_SetFocus(&this->actor, 25.0f);
        Collider_UpdateCylinder(&this->actor, &this->collider);
    }

    if (this->collider.base.atFlags & AT_ON) {
        CollisionCheck_SetAT(play, &play->colChkCtx, &this->collider.base);
    }

    if (this->collider.base.acFlags & AC_ON) {
        CollisionCheck_SetAC(play, &play->colChkCtx, &this->collider.base);
    }

    CollisionCheck_SetOC(play, &play->colChkCtx, &this->collider.base);

    if (this->drawDmgEffAlpha > 0.0f) {
        if (this->drawDmgEffType != ACTOR_DRAW_DMGEFF_FROZEN_NO_SFX) {
            Math_StepToF(&this->drawDmgEffAlpha, 0.0f, 0.05f);
            this->drawDmgEffScale = (this->drawDmgEffAlpha + 1.0f) * 0.3f;
            this->drawDmgEffScale = CLAMP_MAX(this->drawDmgEffScale, 0.6f);
        } else if (!Math_StepToF(&this->drawDmgEffFrozenSteamScale, 0.6f, 15.0f * 0.001f)) {
            Actor_PlaySfx_Flagged(&this->actor, NA_SE_EV_ICE_FREEZE - SFX_FLAG);
        }
    }
}

s32 EnKame_OverrideLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, Actor* thisx) {
    EnKame* this = (EnKame*)thisx;

    if ((this->actionFunc == EnKame_RetreatIntoShell) || (this->actionFunc == EnKame_EmergeFromShell)) {
        if (limbIndex == SNAPPER_LIMB_HEAD) {
            Matrix_Scale(this->limbScaleXZ, this->limbScaleY, this->limbScaleXZ, MTXMODE_APPLY);
        } else if ((limbIndex == SNAPPER_LIMB_BACK_RIGHT_LEG) || (limbIndex == SNAPPER_LIMB_BACK_LEFT_LEG) ||
                   (limbIndex == SNAPPER_LIMB_FRONT_RIGHT_LEG) || (limbIndex == SNAPPER_LIMB_FRONT_LEFT_LEG)) {
            Matrix_Scale(this->limbScaleXZ, this->limbScaleY, this->limbScaleY, MTXMODE_APPLY);
        }
    }

    return false;
}

/**
 * Five of the entries in the body parts array are used for rendering
 * multiple effects on the Snapper's body limb. The positions of these
 * effects are offset from the body limb using the values in this array.
 */
static Vec3f sBodyLimbBodyPartOffsets[] = {
    { 1500.0f, 0.0f, -2000.0f }, // SNAPPER_BODYPART_BODY_1
    { 1500.0f, 0.0f, 2000.0f },  // SNAPPER_BODYPART_BODY_2
    { 1500.0f, 2000.0f, 0.0f },  // SNAPPER_BODYPART_BODY_3
    { 1500.0f, -2000.0f, 0.0f }, // SNAPPER_BODYPART_BODY_4
    { 2500.0f, 0.0f, 0.0f },     // SNAPPER_BODYPART_BODY_5
};

static s8 sLimbToBodyParts[SNAPPER_LIMB_MAX] = {
    BODYPART_NONE,                     // SNAPPER_LIMB_NONE
    BODYPART_NONE,                     // SNAPPER_LIMB_BODY
    BODYPART_NONE,                     // SNAPPER_LIMB_HEAD
    SNAPPER_BODYPART_JAW,              // SNAPPER_LIMB_JAW
    BODYPART_NONE,                     // SNAPPER_LIMB_EYES
    BODYPART_NONE,                     // SNAPPER_LIMB_FRONT_LEFT_LEG
    SNAPPER_BODYPART_FRONT_LEFT_FOOT,  // SNAPPER_LIMB_FRONT_LEFT_FOOT
    BODYPART_NONE,                     // SNAPPER_LIMB_FRONT_RIGHT_LEG
    SNAPPER_BODYPART_FRONT_RIGHT_FOOT, // SNAPPER_LIMB_FRONT_RIGHT_FOOT
    BODYPART_NONE,                     // SNAPPER_LIMB_BACK_LEFT_LEG
    SNAPPER_BODYPART_BACK_LEFT_FOOT,   // SNAPPER_LIMB_BACK_LEFT_FOOT
    BODYPART_NONE,                     // SNAPPER_LIMB_BACK_RIGHT_LEG
    SNAPPER_BODYPART_BACK_RIGHT_FOOT,  // SNAPPER_LIMB_BACK_RIGHT_FOOT
};

void EnKame_PostLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot, Actor* thisx) {
    EnKame* this = (EnKame*)thisx;

    if (sLimbToBodyParts[limbIndex] != BODYPART_NONE) {
        Matrix_MultZero(&this->bodyPartsPos[sLimbToBodyParts[limbIndex]]);
    }

    if (limbIndex == SNAPPER_LIMB_BODY) {
        s32 i;
        Vec3f* bodyPartsPosPtr;
        Vec3f* bodyLimbOffsetsPtr;

        // When the Snapper is flipping itself upright, the animation will make it appear
        // as if it is jumping off the ground. This is purely visual, though, so if we do
        // nothing, the actor's position won't actually change during this apparent "jump",
        // which causes certain things like the circle shadow and the actor's focus to look
        // incorrect. To fix this, we temporarily adjust the actor's position during this
        // animation so that everything looks correct. This position change is overridden
        // at the end of EnKame_Draw, so from the perspective of most other functions,
        // nothing really changes.
        if (this->actor.shape.shadowDraw == NULL) {
            Matrix_MultZero(&this->actor.world.pos);
        }

        bodyLimbOffsetsPtr = sBodyLimbBodyPartOffsets;
        bodyPartsPosPtr = &this->bodyPartsPos[SNAPPER_BODYPART_BODY_1];
        for (i = 0; i < ARRAY_COUNT(sBodyLimbBodyPartOffsets); i++) {
            Matrix_MultVec3f(bodyLimbOffsetsPtr, bodyPartsPosPtr);
            bodyLimbOffsetsPtr++;
            bodyPartsPosPtr++;
        }
    }
}

void EnKame_Draw(Actor* thisx, PlayState* play) {
    EnKame* this = (EnKame*)thisx;
    Vec3f originalPos;

    // If the Snapper is flipping itself upright, we'll update its position inside EnKame_PostLimbDraw
    // to make it better match the "jump" that it does in the animation. We want to make sure that its
    // original position is restored before we exit this function, however, so we save it here before
    // we ever call EnKame_PostLimbDraw.
    if (this->actor.shape.shadowDraw == NULL) {
        Math_Vec3f_Copy(&originalPos, &this->actor.world.pos);
    }

    OPEN_DISPS(play->state.gfxCtx);

    Gfx_SetupDL25_Opa(play->state.gfxCtx);

    gSPSegment(POLY_OPA_DISP++, 0x08, sEyeTextures[this->eyeIndex]);

    SkelAnime_DrawFlexOpa(play, this->snapperSkelAnime.skeleton, this->snapperSkelAnime.jointTable,
                          this->snapperSkelAnime.dListCount, EnKame_OverrideLimbDraw, EnKame_PostLimbDraw,
                          &this->actor);
    Actor_DrawDamageEffects(play, &this->actor, this->bodyPartsPos, SNAPPER_BODYPART_MAX, this->drawDmgEffScale,
                            this->drawDmgEffFrozenSteamScale, this->drawDmgEffAlpha, this->drawDmgEffType);

    // The Snapper is in the middle of the animation where it's flipping upright, and we updated its position
    // in EnKame_PostLimbDraw as explained earlier. We draw the circle shadow, set the focus, and update its
    // collider; all of these rely on the Snapper's position being updated earlier to match the "jump" that it
    // does in its animation. Once we're done, we restore the Snappper's position back to what it was before
    // because we want other functions to act like the position never changed.
    if (this->actor.shape.shadowDraw == NULL) {
        ActorShadow_DrawCircle(&this->actor, NULL, play);
        Actor_SetFocus(&this->actor, 25.0f);
        Collider_UpdateCylinder(&this->actor, &this->collider);
        Math_Vec3f_Copy(&this->actor.world.pos, &originalPos);
    }

    CLOSE_DISPS(play->state.gfxCtx);
}

s32 EnKame_SpikedSnapperOverrideLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot,
                                         Actor* thisx) {
    EnKame* this = (EnKame*)thisx;

    if (limbIndex == SPIKED_SNAPPER_LIMB_BODY) {
        pos->y -= 700.0f;
    }

    if ((this->spikesScale != 1.0f) && (limbIndex == SPIKED_SNAPPER_LIMB_SPIKES)) {
        Matrix_Scale(1.0f, this->spikesScale, this->spikesScale, MTXMODE_APPLY);
    }

    return false;
}

void EnKame_DrawSpikedSnapper(Actor* thisx, PlayState* play) {
    EnKame* this = (EnKame*)thisx;

    Gfx_SetupDL25_Opa(play->state.gfxCtx);
    SkelAnime_DrawFlexOpa(play, this->spikedSnapperSkelAnime.skeleton, this->spikedSnapperSkelAnime.jointTable,
                          this->spikedSnapperSkelAnime.dListCount, EnKame_SpikedSnapperOverrideLimbDraw, NULL,
                          &this->actor);
}
