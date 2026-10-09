/*
 * File: z_en_bbfall.c
 * Overlay: ovl_En_Bbfall
 * Description: Red Bubble
 */

#include "z_en_bbfall.h"
#include "overlays/actors/ovl_En_Clear_Tag/z_en_clear_tag.h"
#include "objects/gameplay_keep/gameplay_keep.h"

#define FLAGS                                                                                 \
    (ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_HOSTILE | ACTOR_FLAG_UPDATE_CULLING_DISABLED | \
     ACTOR_FLAG_HOOKSHOT_PULLS_ACTOR)

void EnBbfall_Init(Actor* thisx, PlayState* play);
void EnBbfall_Destroy(Actor* thisx, PlayState* play);
void EnBbfall_Update(Actor* thisx, PlayState* play);
void EnBbfall_Draw(Actor* thisx, PlayState* play2);

void EnBbfall_SetupWaitForPlayer(EnBbfall* this);
void EnBbfall_WaitForPlayer(EnBbfall* this, PlayState* play);
void EnBbfall_SetupEmerge(EnBbfall* this);
void EnBbfall_Emerge(EnBbfall* this, PlayState* play);
void EnBbfall_SetupFly(EnBbfall* this);
void EnBbfall_Fly(EnBbfall* this, PlayState* play);
void EnBbfall_SetupSinkIntoLava(EnBbfall* this);
void EnBbfall_SinkIntoLava(EnBbfall* this, PlayState* play);
void EnBbfall_Down(EnBbfall* this, PlayState* play);
void EnBbfall_Dead(EnBbfall* this, PlayState* play);
void EnBbfall_Damage(EnBbfall* this, PlayState* play);
void EnBbfall_Frozen(EnBbfall* this, PlayState* play);

typedef enum {
    /* -1 */ BBFALL_BODY_PART_DRAW_STATUS_BROKEN = -1,
    /*  0 */ BBFALL_BODY_PART_DRAW_STATUS_ALIVE,
    /*  1 */ BBFALL_BODY_PART_DRAW_STATUS_DEAD
} EnBbfallBodyPartDrawStatus;

ActorProfile En_Bbfall_Profile = {
    /**/ ACTOR_EN_BBFALL,
    /**/ ACTORCAT_ENEMY,
    /**/ FLAGS,
    /**/ OBJECT_BB,
    /**/ sizeof(EnBbfall),
    /**/ EnBbfall_Init,
    /**/ EnBbfall_Destroy,
    /**/ EnBbfall_Update,
    /**/ EnBbfall_Draw,
};

static ColliderJntSphElementInit sJntSphElementsInit[3] = {
    {
        {
            ELEM_MATERIAL_UNK0,
            { 0xF7CFFFFF, 0x01, 0x08 },
            { 0xF7CFFFFF, 0x00, 0x00 },
            ATELEM_ON | ATELEM_SFX_HARD,
            ACELEM_ON | ACELEM_HOOKABLE,
            OCELEM_ON,
        },
        { 0, { { 0, 0, 0 }, 20 }, 100 },
    },
    {
        {
            ELEM_MATERIAL_UNK0,
            { 0xF7CFFFFF, 0x01, 0x08 },
            { 0xF7CFFFFF, 0x00, 0x00 },
            ATELEM_ON | ATELEM_SFX_HARD,
            ACELEM_NONE,
            OCELEM_NONE,
        },
        { 0, { { 0, 0, 0 }, 20 }, 100 },
    },
    {
        {
            ELEM_MATERIAL_UNK0,
            { 0xF7CFFFFF, 0x01, 0x08 },
            { 0xF7CFFFFF, 0x00, 0x00 },
            ATELEM_ON | ATELEM_SFX_HARD,
            ACELEM_NONE,
            OCELEM_NONE,
        },
        { 0, { { 0, 0, 0 }, 20 }, 100 },
    },
};

static ColliderJntSphInit sJntSphInit = {
    {
        COL_MATERIAL_HIT3,
        AT_NONE | AT_TYPE_ENEMY,
        AC_ON | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_1,
        COLSHAPE_JNTSPH,
    },
    ARRAY_COUNT(sJntSphElementsInit),
    sJntSphElementsInit,
};

typedef enum {
    /* 0x0 */ EN_BBFALL_DMGEFF_NONE,
    /* 0x1 */ EN_BBFALL_DMGEFF_STUN,
    /* 0x3 */ EN_BBFALL_DMGEFF_ICE_ARROW = 0x3,
    /* 0x4 */ EN_BBFALL_DMGEFF_LIGHT_ARROW,
    /* 0x5 */ EN_BBFALL_DMGEFF_ZORA_MAGIC,
    /* 0xE */ EN_BBFALL_DMGEFF_HOOKSHOT = 0xE
} EnBbfallDamageEffect;

static DamageTable sDamageTable = {
    /* Deku Nut       */ DMG_ENTRY(0, EN_BBFALL_DMGEFF_STUN),
    /* Deku Stick     */ DMG_ENTRY(1, EN_BBFALL_DMGEFF_NONE),
    /* Horse trample  */ DMG_ENTRY(0, EN_BBFALL_DMGEFF_NONE),
    /* Explosives     */ DMG_ENTRY(1, EN_BBFALL_DMGEFF_NONE),
    /* Zora boomerang */ DMG_ENTRY(1, EN_BBFALL_DMGEFF_NONE),
    /* Normal arrow   */ DMG_ENTRY(1, EN_BBFALL_DMGEFF_NONE),
    /* UNK_DMG_0x06   */ DMG_ENTRY(0, EN_BBFALL_DMGEFF_NONE),
    /* Hookshot       */ DMG_ENTRY(0, EN_BBFALL_DMGEFF_HOOKSHOT),
    /* Goron punch    */ DMG_ENTRY(1, EN_BBFALL_DMGEFF_NONE),
    /* Sword          */ DMG_ENTRY(1, EN_BBFALL_DMGEFF_NONE),
    /* Goron pound    */ DMG_ENTRY(1, EN_BBFALL_DMGEFF_NONE),
    /* Fire arrow     */ DMG_ENTRY(1, EN_BBFALL_DMGEFF_NONE),
    /* Ice arrow      */ DMG_ENTRY(2, EN_BBFALL_DMGEFF_ICE_ARROW),
    /* Light arrow    */ DMG_ENTRY(2, EN_BBFALL_DMGEFF_LIGHT_ARROW),
    /* Goron spikes   */ DMG_ENTRY(1, EN_BBFALL_DMGEFF_NONE),
    /* Deku spin      */ DMG_ENTRY(1, EN_BBFALL_DMGEFF_NONE),
    /* Deku bubble    */ DMG_ENTRY(1, EN_BBFALL_DMGEFF_NONE),
    /* Deku launch    */ DMG_ENTRY(2, EN_BBFALL_DMGEFF_NONE),
    /* UNK_DMG_0x12   */ DMG_ENTRY(0, EN_BBFALL_DMGEFF_STUN),
    /* Zora barrier   */ DMG_ENTRY(0, EN_BBFALL_DMGEFF_ZORA_MAGIC),
    /* Normal shield  */ DMG_ENTRY(0, EN_BBFALL_DMGEFF_NONE),
    /* Light ray      */ DMG_ENTRY(0, EN_BBFALL_DMGEFF_NONE),
    /* Thrown object  */ DMG_ENTRY(1, EN_BBFALL_DMGEFF_NONE),
    /* Zora punch     */ DMG_ENTRY(1, EN_BBFALL_DMGEFF_NONE),
    /* Spin attack    */ DMG_ENTRY(1, EN_BBFALL_DMGEFF_NONE),
    /* Sword beam     */ DMG_ENTRY(0, EN_BBFALL_DMGEFF_NONE),
    /* Normal Roll    */ DMG_ENTRY(0, EN_BBFALL_DMGEFF_NONE),
    /* UNK_DMG_0x1B   */ DMG_ENTRY(0, EN_BBFALL_DMGEFF_NONE),
    /* UNK_DMG_0x1C   */ DMG_ENTRY(0, EN_BBFALL_DMGEFF_NONE),
    /* Unblockable    */ DMG_ENTRY(0, EN_BBFALL_DMGEFF_NONE),
    /* UNK_DMG_0x1E   */ DMG_ENTRY(0, EN_BBFALL_DMGEFF_NONE),
    /* Powder Keg     */ DMG_ENTRY(1, EN_BBFALL_DMGEFF_NONE),
};

static CollisionCheckInfoInit sColChkInfoInit = { 2, 20, 40, 50 };

static InitChainEntry sInitChain[] = {
    ICHAIN_S8(hintId, TATL_HINT_ID_RED_BUBBLE, ICHAIN_CONTINUE),
    ICHAIN_F32(lockOnArrowOffset, 10, ICHAIN_STOP),
};

void EnBbfall_Init(Actor* thisx, PlayState* play) {
    EnBbfall* this = (EnBbfall*)thisx;
    s32 i;

    Actor_ProcessInitChain(&this->actor, sInitChain);
    SkelAnime_Init(play, &this->skelAnime, &gBubbleSkel, &gBubbleFlyingAnim, this->jointTable, this->morphTable,
                   BUBBLE_LIMB_MAX);
    CollisionCheck_SetInfo(&this->actor.colChkInfo, &sDamageTable, &sColChkInfoInit);
    Collider_InitAndSetJntSph(play, &this->collider, &this->actor, &sJntSphInit, this->colliderElements);
    ActorShape_Init(&this->actor.shape, 1500.0f, ActorShadow_DrawCircle, 35.0f);
    this->timer = 0;
    EnBbfall_SetupWaitForPlayer(this);
    Actor_SetFocus(&this->actor, 0.0f);
    for (i = 0; i < ARRAY_COUNT(this->colliderElements); i++) {
        this->collider.elements[i].dim.worldSphere.radius = this->collider.elements[i].dim.modelSphere.radius;
    }
}

void EnBbfall_Destroy(Actor* thisx, PlayState* play) {
    EnBbfall* this = (EnBbfall*)thisx;

    Collider_DestroyJntSph(play, &this->collider);
}

void EnBbfall_Freeze(EnBbfall* this) {
    this->drawDmgEffType = ACTOR_DRAW_DMGEFF_FROZEN_NO_SFX;
    this->drawDmgEffScale = 0.4f;
    this->drawDmgEffFrozenSteamScale = 0.6f;
    this->timer = 80;
    this->drawDmgEffAlpha = 1.0f;
    this->actor.flags &= ~ACTOR_FLAG_HOOKSHOT_PULLS_ACTOR;
    Actor_SetColorFilter(&this->actor, COLORFILTER_COLORFLAG_RED, 255, COLORFILTER_BUFFLAG_OPA, 80);
}

void EnBbfall_Thaw(EnBbfall* this, PlayState* play) {
    if (this->drawDmgEffType == ACTOR_DRAW_DMGEFF_FROZEN_NO_SFX) {
        this->drawDmgEffType = ACTOR_DRAW_DMGEFF_FIRE;
        this->drawDmgEffAlpha = 0.0f;
        Actor_SpawnIceEffects(play, &this->actor, this->bodyPartsPos, BUBBLE_BODYPART_MAX, 2, 0.2f, 0.15f);
        this->actor.flags |= ACTOR_FLAG_HOOKSHOT_PULLS_ACTOR;
    }
}

/**
 * Returns true if the Bubble is touching a floor that it should "sink into" (i.e., if it's touching lava).
 */
s32 EnBbfall_IsTouchingLava(EnBbfall* this, PlayState* play) {
    if (!SurfaceType_IsWallDamage(&play->colCtx, this->actor.floorPoly, this->actor.floorBgId)) {
        FloorType floorType = SurfaceType_GetFloorType(&play->colCtx, this->actor.floorPoly, this->actor.floorBgId);

        if ((floorType == FLOOR_TYPE_2) || (floorType == FLOOR_TYPE_3) || (floorType == FLOOR_TYPE_9)) {
            return true;
        }
    }

    return false;
}

void EnBbfall_PlaySfx(EnBbfall* this) {
    if (Animation_OnFrame(&this->skelAnime, 0.0f) || Animation_OnFrame(&this->skelAnime, 5.0f)) {
        Actor_PlaySfx(&this->actor, NA_SE_EN_BUBLE_MOUTH);
    }

    Actor_PlaySfx_Flagged(&this->actor, NA_SE_EN_BUBLEFALL_FIRE - SFX_FLAG);
}

/**
 * Checks to see if the Bubble is touching a wall. If it is, and if the
 * Bubble is facing directly "into" the wall, then rotate it away from
 * the wall.
 */
void EnBbfall_CheckForWall(EnBbfall* this) {
    s16 yawDiff;

    if (this->actor.bgCheckFlags & BGCHECKFLAG_WALL) {
        yawDiff = this->actor.shape.rot.y - this->actor.wallYaw;

        if (ABS_ALT(yawDiff) > 0x4000) {
            this->actor.shape.rot.y = ((this->actor.wallYaw * 2) - this->actor.shape.rot.y) - 0x8000;
        }

        this->actor.bgCheckFlags &= ~BGCHECKFLAG_WALL;
    }
}

void EnBbfall_EnableColliders(EnBbfall* this) {
    this->collider.elements[0].base.atDmgInfo.effect = ELEM_MATERIAL_UNK1; // Fire
    this->collider.elements[1].base.atElemFlags |= ATELEM_ON;
    this->collider.elements[2].base.atElemFlags |= ATELEM_ON;
}

void EnBbfall_DisableColliders(EnBbfall* this) {
    this->collider.elements[0].base.atDmgInfo.effect = ELEM_MATERIAL_UNK0; // Nothing
    this->collider.elements[1].base.atElemFlags &= ~ATELEM_ON;
    this->collider.elements[2].base.atElemFlags &= ~ATELEM_ON;
}

void EnBbfall_SetupWaitForPlayer(EnBbfall* this) {
    s32 i;

    Animation_PlayLoop(&this->skelAnime, &gBubbleAttackAnim);
    this->collider.base.atFlags &= ~AT_ON;
    this->collider.base.acFlags &= ~AC_ON;
    this->collider.base.ocFlags1 &= ~OC1_ON;
    this->flameScaleY = 0.8f;
    this->flameScaleX = 1.0f;
    this->flameOpacity = 255;
    this->actor.colChkInfo.health = sColChkInfoInit.health;
    this->actor.colorFilterTimer = 0;
    this->isBgCheckCollisionEnabled = false;
    this->actor.speed = 0.0f;
    this->actor.gravity = 0.0f;
    this->actor.velocity.y = 0.0f;
    Math_Vec3f_Copy(&this->actor.world.pos, &this->actor.home.pos);
    this->actor.world.pos.y -= 90.0f;
    for (i = 0; i < ARRAY_COUNT(this->flamePos); i++) {
        Math_Vec3f_Copy(&this->flamePos[i], &this->actor.world.pos);
        this->flamePos[i].y -= 47.0f;
    }

    this->actor.bgCheckFlags &= ~BGCHECKFLAG_GROUND;
    this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
    this->actionFunc = EnBbfall_WaitForPlayer;
}

void EnBbfall_WaitForPlayer(EnBbfall* this, PlayState* play) {
    if (this->timer != 0) {
        this->timer--;
    } else if ((Player_GetMask(play) != PLAYER_MASK_STONE) && (this->actor.xyzDistToPlayerSq <= SQ(250.0f))) {
        EnBbfall_SetupEmerge(this);
    }
}

void EnBbfall_SetupEmerge(EnBbfall* this) {
    this->actor.gravity = -1.0f;
    this->actor.world.rot.y = this->actor.yawTowardsPlayer;
    this->actor.shape.rot.y = this->actor.yawTowardsPlayer;
    this->collider.base.atFlags |= AT_ON;
    this->collider.base.acFlags |= AC_ON;
    this->collider.base.ocFlags1 |= OC1_ON;
    this->actor.velocity.y = 17.0f;
    EnBbfall_EnableColliders(this);
    this->actor.flags |= ACTOR_FLAG_ATTENTION_ENABLED;
    Actor_PlaySfx(&this->actor, NA_SE_EN_BUBLEFALL_APPEAR);
    this->actionFunc = EnBbfall_Emerge;
}

void EnBbfall_Emerge(EnBbfall* this, PlayState* play) {
    SkelAnime_Update(&this->skelAnime);
    if (this->actor.home.pos.y < this->actor.world.pos.y) {
        EnBbfall_SetupFly(this);
    }

    EnBbfall_PlaySfx(this);
}

void EnBbfall_SetupFly(EnBbfall* this) {
    this->flameOpacity = 255;
    this->isBgCheckCollisionEnabled = true;
    this->actor.bgCheckFlags &= ~BGCHECKFLAG_GROUND;
    this->actor.speed = 5.0f;
    this->actor.gravity = -1.0f;
    this->actionFunc = EnBbfall_Fly;
}

void EnBbfall_Fly(EnBbfall* this, PlayState* play) {
    SkelAnime_Update(&this->skelAnime);
    Math_StepToF(&this->flameScaleY, 0.8f, 0.1f);
    Math_StepToF(&this->flameScaleX, 1.0f, 0.1f);
    EnBbfall_CheckForWall(this);
    if (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND) {
        if (EnBbfall_IsTouchingLava(this, play)) {
            EnBbfall_SetupSinkIntoLava(this);
        } else {
            // Bounce upwards off the ground
            this->actor.velocity.y *= -1.2f;
            this->actor.velocity.y = CLAMP(this->actor.velocity.y, 8.0f, 12.0f);
            this->actor.shape.rot.y += TRUNCF_BINANG(Rand_CenteredFloat(0x12000));
        }

        this->actor.bgCheckFlags &= ~BGCHECKFLAG_GROUND;
    }

    this->actor.world.rot.y = this->actor.shape.rot.y;
    EnBbfall_PlaySfx(this);
}

void EnBbfall_SetupSinkIntoLava(EnBbfall* this) {
    this->isBgCheckCollisionEnabled = false;
    this->collider.base.acFlags &= ~AC_ON;
    this->actionFunc = EnBbfall_SinkIntoLava;
}

void EnBbfall_SinkIntoLava(EnBbfall* this, PlayState* play) {
    SkelAnime_Update(&this->skelAnime);
    if (this->actor.world.pos.y < (this->actor.floorHeight - 90.0f)) {
        this->timer = 10;
        EnBbfall_SetupWaitForPlayer(this);
    } else {
        EnBbfall_PlaySfx(this);
    }
}

void EnBbfall_SetupDown(EnBbfall* this) {
    Animation_PlayLoop(&this->skelAnime, &gBubbleFlyingAnim);
    this->collider.base.atFlags |= AT_ON;
    this->timer = 200;
    this->flameOpacity = 0;
    this->collider.base.acFlags |= AC_ON;
    this->actor.speed = 2.0f;
    this->flameScaleY = 0.0f;
    this->flameScaleX = 0.0f;
    this->actor.gravity = -2.0f;
    Actor_PlaySfx(&this->actor, NA_SE_EN_BUBLE_DOWN);
    this->actor.world.rot.y = this->actor.shape.rot.y;
    this->actionFunc = EnBbfall_Down;
}

void EnBbfall_Down(EnBbfall* this, PlayState* play) {
    SkelAnime_Update(&this->skelAnime);
    EnBbfall_CheckForWall(this);

    if (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND) {
        if (EnBbfall_IsTouchingLava(this, play)) {
            EnBbfall_SetupSinkIntoLava(this);
            return;
        }

        Actor_PlaySfx(&this->actor, NA_SE_EN_EYEGOLE_ATTACK);
        if (this->timer == 0) {
            Actor_PlaySfx(&this->actor, NA_SE_EN_BUBLE_UP);
            EnBbfall_EnableColliders(this);
            this->actor.velocity.y = 8.0f;
            EnBbfall_SetupFly(this);
            return;
        }

        if (this->actor.velocity.y < -14.0f) {
            this->actor.velocity.y *= -0.7f;
        } else {
            this->actor.velocity.y = 10.0f;
        }

        this->actor.bgCheckFlags &= ~BGCHECKFLAG_GROUND;
        Actor_SpawnFloorDustRing(play, &this->actor, &this->actor.world.pos, 7.0f, 2, 2.0f, 0, 0, false);
        Math_ScaledStepToS(&this->actor.shape.rot.y, BINANG_ADD(this->actor.yawTowardsPlayer, 0x8000), 0xBB8);
    }

    this->actor.world.rot.y = this->actor.shape.rot.y;
    if (Animation_OnFrame(&this->skelAnime, 5.0f)) {
        Actor_PlaySfx(&this->actor, NA_SE_EN_BUBLE_WING);
    }

    if (this->timer > 0) {
        this->timer--;
    }
}

void EnBbfall_SetupDead(EnBbfall* this, PlayState* play) {
    Vec3f* bodyPartVelocity;
    Vec3f posDiff;
    f32 magnitude;
    s32 i;

    func_800BE5CC(&this->actor, &this->collider, 0);
    this->timer = 15;
    this->actor.shape.rot.x += 0x4E20;
    this->actor.speed = 0.0f;
    SoundSource_PlaySfxAtFixedWorldPos(play, &this->actor.world.pos, 40, NA_SE_EN_BUBLE_DEAD);
    Item_DropCollectibleRandom(play, &this->actor, &this->actor.world.pos, 0x70);
    this->actor.velocity.y = 0.0f;
    this->actor.speed = 0.0f;
    this->bodyPartDrawStatus = BBFALL_BODY_PART_DRAW_STATUS_DEAD;
    this->actor.gravity = -1.5f;

    bodyPartVelocity = &this->bodyPartsVelocity[0];
    for (i = 0; i < BUBBLE_BODYPART_MAX; i++, bodyPartVelocity++) {
        Math_Vec3f_Diff(&this->bodyPartsPos[i], &this->actor.world.pos, &posDiff);
        magnitude = Math3D_Vec3fMagnitude(&posDiff);
        if (magnitude > 1.0f) {
            magnitude = 2.5f / magnitude;
        }

        bodyPartVelocity->x = posDiff.x * magnitude;
        bodyPartVelocity->z = posDiff.z * magnitude;
        bodyPartVelocity->y = Rand_ZeroFloat(3.5f) + 10.0f;
    }

    this->actionFunc = EnBbfall_Dead;
}

void EnBbfall_Dead(EnBbfall* this, PlayState* play) {
    s32 i;

    this->timer--;
    Math_SmoothStepToS(&this->actor.world.rot.z, 0x4000, 4, 0x1000, 0x400);

    if (this->timer == 0) {
        for (i = 0; i < BUBBLE_BODYPART_MAX; i++) {
            func_800B3030(play, &this->bodyPartsPos[i], &gZeroVec3f, &gZeroVec3f, 40, 7, 2);
            SoundSource_PlaySfxAtFixedWorldPos(play, &this->bodyPartsPos[i], 11, NA_SE_EN_EXTINCT);
        }

        Actor_Kill(&this->actor);
        return;
    }

    for (i = 0; i < BUBBLE_BODYPART_MAX; i++) {
        Math_Vec3f_Sum(&this->bodyPartsPos[i], &this->bodyPartsVelocity[i], &this->bodyPartsPos[i]);
        this->bodyPartsVelocity[i].y += this->actor.gravity;
    }
}

void EnBbfall_SetupDamage(EnBbfall* this) {
    this->collider.base.acFlags &= ~AC_ON;
    Actor_PlaySfx(&this->actor, NA_SE_EN_BUBLE_DAMAGE);
    func_800BE5CC(&this->actor, &this->collider, 0);

    if (this->actor.colChkInfo.damageEffect == EN_BBFALL_DMGEFF_ZORA_MAGIC) {
        Actor_SetColorFilter(&this->actor, COLORFILTER_COLORFLAG_BLUE, 255, COLORFILTER_BUFFLAG_OPA, 40);
        this->drawDmgEffType = ACTOR_DRAW_DMGEFF_ELECTRIC_SPARKS_LARGE;
        this->drawDmgEffAlpha = 2.0f;
        this->drawDmgEffScale = 0.4f;
    } else if (this->actor.colChkInfo.damageEffect == EN_BBFALL_DMGEFF_STUN) {
        Actor_SetColorFilter(&this->actor, COLORFILTER_COLORFLAG_BLUE, 255, COLORFILTER_BUFFLAG_OPA, 20);
        this->actor.speed = 0.0f;
    } else if (this->actor.colChkInfo.damageEffect == EN_BBFALL_DMGEFF_HOOKSHOT) {
        this->actor.speed = 0.0f;
    } else {
        Actor_SetColorFilter(&this->actor, COLORFILTER_COLORFLAG_RED, 255, COLORFILTER_BUFFLAG_OPA, 20);
        this->actor.speed = 7.0f;
    }

    this->actionFunc = EnBbfall_Damage;
}

void EnBbfall_Damage(EnBbfall* this, PlayState* play) {
    Math_SmoothStepToF(&this->actor.speed, 0.0f, 1.0f, 0.5f, 0.0f);
    if ((this->actor.bgCheckFlags & BGCHECKFLAG_GROUND) && (this->actor.speed < 0.1f)) {
        if (EnBbfall_IsTouchingLava(this, play)) {
            EnBbfall_SetupSinkIntoLava(this);
        } else {
            EnBbfall_SetupDown(this);
        }
    }
}

void EnBbfall_SetupFrozen(EnBbfall* this) {
    this->actor.speed = 0.0f;
    if (this->actor.velocity.y > 0.0f) {
        this->actor.velocity.y = 0.0f;
    }

    this->actor.gravity = -2.0f;
    this->actionFunc = EnBbfall_Frozen;
}

void EnBbfall_Frozen(EnBbfall* this, PlayState* play) {
    DECR(this->timer);

    if (this->timer == 0) {
        EnBbfall_Thaw(this, play);
        if (this->actor.colChkInfo.health == 0) {
            EnBbfall_SetupDead(this, play);
        } else {
            EnBbfall_SetupDown(this);
        }
    }
}

void EnBbfall_UpdateDamage(EnBbfall* this, PlayState* play) {
    if (this->collider.base.acFlags & AC_HIT) {
        this->collider.base.acFlags &= ~AC_HIT;
        this->collider.base.atFlags &= ~(AT_HIT | AT_BOUNCED);
        this->collider.base.atFlags &= ~AT_ON;
        if ((this->drawDmgEffType != ACTOR_DRAW_DMGEFF_FROZEN_NO_SFX) ||
            !(this->collider.elements[0].base.acHitElem->atDmgInfo.dmgFlags & 0xDB0B3)) {
            Actor_SetDropFlagJntSph(&this->actor, &this->collider);
            this->flameOpacity = 0;
            this->flameScaleY = 0.0f;
            this->flameScaleX = 0.0f;
            EnBbfall_DisableColliders(this);
            EnBbfall_Thaw(this, play);

            if (Actor_ApplyDamage(&this->actor) == 0) {
                Enemy_StartFinishingBlow(play, &this->actor);
            }

            if (this->actor.colChkInfo.damageEffect == EN_BBFALL_DMGEFF_ICE_ARROW) {
                EnBbfall_Freeze(this);
                if (this->actor.colChkInfo.health == 0) {
                    this->timer = 3;
                    this->collider.base.acFlags &= ~AC_ON;
                }

                EnBbfall_SetupFrozen(this);
            } else if (this->actor.colChkInfo.health == 0) {
                EnBbfall_SetupDead(this, play);
            } else {
                EnBbfall_SetupDamage(this);
            }

            if (this->actor.colChkInfo.damageEffect == EN_BBFALL_DMGEFF_LIGHT_ARROW) {
                this->drawDmgEffAlpha = 4.0f;
                this->drawDmgEffScale = 0.4f;
                this->drawDmgEffType = ACTOR_DRAW_DMGEFF_LIGHT_ORBS;
                Actor_Spawn(&play->actorCtx, play, ACTOR_EN_CLEAR_TAG,
                            this->collider.elements[0].base.acDmgInfo.hitPos.x,
                            this->collider.elements[0].base.acDmgInfo.hitPos.y,
                            this->collider.elements[0].base.acDmgInfo.hitPos.z, 0, 0, 0,
                            CLEAR_TAG_PARAMS(CLEAR_TAG_SMALL_LIGHT_RAYS));
            }
        }
    } else {
        if (this->collider.base.atFlags & AT_BOUNCED) {
            this->collider.base.atFlags &= ~(AT_HIT | AT_BOUNCED);
            EnBbfall_DisableColliders(this);
            if (this->actionFunc != EnBbfall_Down) {
                this->actor.world.rot.y = this->actor.yawTowardsPlayer + 0x8000;
                this->actor.shape.rot.y = this->actor.world.rot.y;
                EnBbfall_SetupDown(this);
            }
        }
    }
}

void EnBbfall_Update(Actor* thisx, PlayState* play) {
    EnBbfall* this = (EnBbfall*)thisx;
    Sphere16* sphere;
    Vec3f diff;
    s32 i;
    f32 scale;
    s32 pad[2];

    EnBbfall_UpdateDamage(this, play);
    this->actionFunc(this, play);
    if (this->actionFunc != EnBbfall_Dead) {
        Actor_MoveWithGravity(&this->actor);
        if (this->isBgCheckCollisionEnabled) {
            Actor_UpdateBgCheckInfo(play, &this->actor, 30.0f, 25.0f, 20.0f,
                                    UPDBGCHECKINFO_FLAG_1 | UPDBGCHECKINFO_FLAG_2 | UPDBGCHECKINFO_FLAG_4);
        }

        for (i = ARRAY_COUNT(this->flamePos) - 1; i >= 2; i--) {
            Math_Vec3f_Diff(&this->flamePos[i - 2], &this->flamePos[i - 1], &diff);
            Math_Vec3f_Scale(&diff, (i - 1) * 0.1f);
            Math_Vec3f_Copy(&this->flamePos[i], &this->flamePos[i - 1]);
            Math_Vec3f_Sum(&this->flamePos[i], &diff, &this->flamePos[i]);
        }

        Math_Vec3f_Copy(&this->flamePos[1], &this->flamePos[0]);
        Math_Vec3f_Copy(&this->flamePos[0], &this->actor.world.pos);
        this->flamePos[0].y += 15.0f;
        this->flamePos[0].y -= 47.0f * this->flameScaleY;

        for (i = 0, scale = this->flameScaleX; i < ARRAY_COUNT(this->colliderElements); i++, scale *= 0.7569f) {
            sphere = &this->collider.elements[i].dim.worldSphere;
            sphere->radius = 30.0f * scale;
            sphere->center.x = this->flamePos[2 * i].x;
            sphere->center.y = this->flamePos[2 * i].y + (47.0f * scale);
            sphere->center.z = this->flamePos[2 * i].z;
        }

        this->collider.elements[0].dim.worldSphere.radius =
            CLAMP_MIN(this->collider.elements[0].dim.worldSphere.radius, 20);

        Math_Vec3s_ToVec3f(&this->actor.focus.pos, &this->collider.elements[0].dim.worldSphere.center);

        if (this->collider.base.atFlags & AT_ON) {
            this->actor.flags |= ACTOR_FLAG_SFX_FOR_PLAYER_BODY_HIT;
            CollisionCheck_SetAT(play, &play->colChkCtx, &this->collider.base);
        }

        if (this->collider.base.acFlags & AC_ON) {
            CollisionCheck_SetAC(play, &play->colChkCtx, &this->collider.base);
        }

        if (this->collider.base.ocFlags1 & OC1_ON) {
            CollisionCheck_SetOC(play, &play->colChkCtx, &this->collider.base);
        }

        if (this->drawDmgEffAlpha > 0.0f) {
            if (this->drawDmgEffType != ACTOR_DRAW_DMGEFF_FROZEN_NO_SFX) {
                Math_StepToF(&this->drawDmgEffAlpha, 0.0f, 0.05f);
                this->drawDmgEffScale = (this->drawDmgEffAlpha + 1.0f) * 0.2f;
                this->drawDmgEffScale = CLAMP_MAX(this->drawDmgEffScale, 0.4f);
            } else if (!Math_StepToF(&this->drawDmgEffFrozenSteamScale, 0.4f, 0.01f)) {
                Actor_PlaySfx_Flagged(&this->actor, NA_SE_EV_ICE_FREEZE - SFX_FLAG);
            }
        }
    }
}

s32 EnBbfall_OverrideLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, Actor* thisx) {
    EnBbfall* this = (EnBbfall*)thisx;

    if (this->bodyPartDrawStatus == BBFALL_BODY_PART_DRAW_STATUS_BROKEN) {
        this->limbDList = *dList;
        *dList = NULL;
    }

    return false;
}

/**
 * This maps a given limb based on its limbIndex to its appropriate index in the `bodyPartsPos/Velocity` arrays.
 */
static s8 sLimbToBodyParts[BUBBLE_LIMB_MAX] = {
    BODYPART_NONE,                      // BUBBLE_LIMB_NONE
    BODYPART_NONE,                      // BUBBLE_LIMB_ROOT
    BODYPART_NONE,                      // BUBBLE_LIMB_CRANIUM_ROOT
    BODYPART_NONE,                      // BUBBLE_LIMB_JAW_ROOT
    BUBBLE_BODYPART_JAW,                // BUBBLE_LIMB_JAW
    BODYPART_NONE,                      // BUBBLE_LIMB_LEFT_WING_ROOT
    BODYPART_NONE,                      // BUBBLE_LIMB_LEFT_WING_WRAPPER
    BODYPART_NONE,                      // BUBBLE_LIMB_LEFT_WING_WEBBING_ROOT
    BUBBLE_BODYPART_LEFT_WING_WEBBING,  // BUBBLE_LIMB_LEFT_WING_WEBBING
    BODYPART_NONE,                      // BUBBLE_LIMB_LEFT_WING_BONE
    BODYPART_NONE,                      // BUBBLE_LIMB_RIGHT_WING_ROOT
    BODYPART_NONE,                      // BUBBLE_LIMB_RIGHT_WING_WRAPPER
    BODYPART_NONE,                      // BUBBLE_LIMB_RIGHT_WING_WEBBING_ROOT
    BUBBLE_BODYPART_RIGHT_WING_WEBBING, // BUBBLE_LIMB_RIGHT_WING_WEBBING
    BODYPART_NONE,                      // BUBBLE_LIMB_RIGHT_WING_BONE
    BUBBLE_BODYPART_CRANIUM,            // BUBBLE_LIMB_CRANIUM
};

/**
 * The last element of the `bodyParts` arrays is not tied to any particular limb and is instead used to control the
 * placement of effects. The positions of these effects are offset by a certain amount from the Bubble's cranium limb.
 */
static Vec3f sEffectsBodyPartOffset = { 1000.0f, -700.0f, 0.0f };

void EnBbfall_PostLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot, Actor* thisx) {
    s32 pad;
    EnBbfall* this = (EnBbfall*)thisx;
    MtxF* currentMatrixState;

    if (this->bodyPartDrawStatus == BBFALL_BODY_PART_DRAW_STATUS_ALIVE) {
        if (sLimbToBodyParts[limbIndex] != BODYPART_NONE) {
            if (sLimbToBodyParts[limbIndex] == BUBBLE_BODYPART_JAW) {
                Matrix_MultVecX(1000.0f, &this->bodyPartsPos[BUBBLE_BODYPART_JAW]);
            } else if (sLimbToBodyParts[limbIndex] == BUBBLE_BODYPART_CRANIUM) {
                Matrix_MultVecX(-1000.0f, &this->bodyPartsPos[BUBBLE_BODYPART_CRANIUM]);
                Matrix_MultVec3f(&sEffectsBodyPartOffset, &this->bodyPartsPos[BUBBLE_BODYPART_EFFECTS]);
            } else {
                Matrix_MultZero(&this->bodyPartsPos[sLimbToBodyParts[limbIndex]]);
            }
        }
    } else if (this->bodyPartDrawStatus > BBFALL_BODY_PART_DRAW_STATUS_ALIVE) {
        if (sLimbToBodyParts[limbIndex] != BODYPART_NONE) {
            Matrix_MultZero(&this->bodyPartsPos[sLimbToBodyParts[limbIndex]]);
        }

        if (limbIndex == BUBBLE_LIMB_CRANIUM) {
            this->bodyPartDrawStatus = BBFALL_BODY_PART_DRAW_STATUS_BROKEN;
        }
    } else {
        if (sLimbToBodyParts[limbIndex] != BODYPART_NONE) {
            OPEN_DISPS(play->state.gfxCtx);

            currentMatrixState = Matrix_GetCurrent();
            currentMatrixState->mf[3][0] = this->bodyPartsPos[sLimbToBodyParts[limbIndex]].x;
            currentMatrixState->mf[3][1] = this->bodyPartsPos[sLimbToBodyParts[limbIndex]].y;
            currentMatrixState->mf[3][2] = this->bodyPartsPos[sLimbToBodyParts[limbIndex]].z;
            Matrix_RotateZS(thisx->world.rot.z, MTXMODE_APPLY);
            MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx);
            gSPDisplayList(POLY_OPA_DISP++, this->limbDList);

            CLOSE_DISPS(play->state.gfxCtx);
        }
    }
}

void EnBbfall_Draw(Actor* thisx, PlayState* play2) {
    PlayState* play = play2;
    EnBbfall* this = (EnBbfall*)thisx;
    MtxF* currentMatrixState;
    Gfx* gfx;
    s32 opacity;
    Vec3f* pos;
    s32 i;

    OPEN_DISPS(play->state.gfxCtx);

    gfx = POLY_OPA_DISP;
    gSPDisplayList(&gfx[0], gSetupDLs[SETUPDL_25]);
    POLY_OPA_DISP = &gfx[1];
    SkelAnime_DrawOpa(play, this->skelAnime.skeleton, this->skelAnime.jointTable, EnBbfall_OverrideLimbDraw,
                      EnBbfall_PostLimbDraw, &this->actor);

    if (this->flameOpacity > 0) {
        Gfx_SetupDL25_Xlu(play->state.gfxCtx);
        Matrix_RotateYS(((Camera_GetCamDirYaw(GET_ACTIVE_CAM(play)) - this->actor.shape.rot.y) + 0x8000),
                        MTXMODE_APPLY);
        Matrix_Scale(this->flameScaleX, this->flameScaleY, 1.0f, MTXMODE_APPLY);
        gDPSetEnvColor(POLY_XLU_DISP++, 255, 0, 0, 0);
        currentMatrixState = Matrix_GetCurrent();

        opacity = this->flameOpacity;
        pos = &this->flamePos[0];

        for (i = 0; i < ARRAY_COUNT(this->flamePos); i++, pos++) {
            gSPSegment(POLY_XLU_DISP++, 0x08,
                       Gfx_TwoTexScrollEx(play->state.gfxCtx, 0, 0, 0, 32, 64, 1, 0,
                                          ((play->gameplayFrames + (i * 10)) * (-20 + i * 2)) & 0x1FF, 32, 128, 0, 0, 0,
                                          -20 + i * 2));
            gDPSetPrimColor(POLY_XLU_DISP++, 0x80, 0x80, 255, 255, 0, opacity);
            currentMatrixState->mf[3][0] = pos->x;
            currentMatrixState->mf[3][1] = pos->y;
            currentMatrixState->mf[3][2] = pos->z;
            MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx);
            gSPDisplayList(POLY_XLU_DISP++, gEffFire1DL);

            opacity -= 35;
            if (opacity < 0) {
                break;
            }

            Matrix_Scale(0.87f, 0.87f, 1.0f, MTXMODE_APPLY);
        }
    }

    Actor_DrawDamageEffects(play, &this->actor, this->bodyPartsPos, BUBBLE_BODYPART_MAX, this->drawDmgEffScale,
                            this->drawDmgEffFrozenSteamScale, this->drawDmgEffAlpha, this->drawDmgEffType);

    CLOSE_DISPS(play->state.gfxCtx);
}
