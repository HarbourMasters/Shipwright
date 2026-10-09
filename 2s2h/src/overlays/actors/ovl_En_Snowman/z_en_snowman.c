/*
 * File: z_en_snowman.c
 * Overlay: ovl_En_Snowman
 * Description: Eeno
 */

#include "z_en_snowman.h"
#include "overlays/actors/ovl_En_Clear_Tag/z_en_clear_tag.h"

#define FLAGS (ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_HOSTILE)

void EnSnowman_Init(Actor* thisx, PlayState* play);
void EnSnowman_Destroy(Actor* thisx, PlayState* play);
void EnSnowman_Update(Actor* thisx, PlayState* play);
void EnSnowman_Draw(Actor* thisx, PlayState* play);

void EnSnowman_SetupMoveSnowPile(EnSnowman* this);
void EnSnowman_MoveSnowPile(EnSnowman* this, PlayState* play);
void EnSnowman_SetupEmerge(EnSnowman* this, PlayState* play);
void EnSnowman_Emerge(EnSnowman* this, PlayState* play);
void EnSnowman_SetupReadySnowball(EnSnowman* this);
void EnSnowman_ReadySnowball(EnSnowman* this, PlayState* play);
void EnSnowman_SetupThrowSnowball(EnSnowman* this);
void EnSnowman_ThrowSnowball(EnSnowman* this, PlayState* play);
void EnSnowman_SetupIdle(EnSnowman* this);
void EnSnowman_Idle(EnSnowman* this, PlayState* play);
void EnSnowman_SetupSubmerge(EnSnowman* this, PlayState* play);
void EnSnowman_Submerge(EnSnowman* this, PlayState* play);
void EnSnowman_Melt(EnSnowman* this, PlayState* play);
void EnSnowman_Stun(EnSnowman* this, PlayState* play);
void EnSnowman_Damaged(EnSnowman* this, PlayState* play);
void EnSnowman_SetupDead(EnSnowman* this);
void EnSnowman_Dead(EnSnowman* this, PlayState* play);
void EnSnowman_SetupSplitDoNothing(EnSnowman* this);
void EnSnowman_SplitDoNothing(EnSnowman* this, PlayState* play);
void EnSnowman_SetupKill(EnSnowman* this);
void EnSnowman_Kill(EnSnowman* this, PlayState* play);
void EnSnowman_CreateSplitEeno(EnSnowman* this, Vec3f* basePos, s32 yRot);
void EnSnowman_SetupCombine(EnSnowman* this, PlayState* play, Vec3f* combinePos);
void EnSnowman_Combine(EnSnowman* this, PlayState* play);
void EnSnowman_UpdateSnowball(Actor* thisx, PlayState* play);
void EnSnowman_DrawSnowPile(Actor* thisx, PlayState* play);
void EnSnowman_DrawSnowball(Actor* thisx, PlayState* play);

typedef enum {
    // Indicates that this split Eeno is not currently trying to combine into a large Eeno.
    /* 0 */ EN_SNOWMAN_COMBINE_STATE_NONE,

    // Indicates that this split Eeno is actively trying to combine into a large Eeno again.
    // Eenos in this state can both absorb other Eenos and start being absorbed themselves.
    /* 1 */ EN_SNOWMAN_COMBINE_STATE_ACTIVE,

    // Indicates that this Eeno cannot absorb other Eenos to combine into a large Eeno.
    // It can still be absorbed by Eenos in the EN_SNOWMAN_COMBINE_STATE_ACTIVE state.
    /* 2 */ EN_SNOWMAN_COMBINE_STATE_NO_ABSORPTION,

    // Indicates that either this Eeno is shrinking having been absorbed by another Eeno, or that
    // this Eeno is a fully-combined large Eeno and is done trying to combine with other split
    // Eenos. In both cases, this Eeno cannot be absorbed by any other Eeno.
    /* 3 */ EN_SNOWMAN_COMBINE_STATE_BEING_ABSORBED_OR_DONE
} EnSnowmanCombineState;

ActorProfile En_Snowman_Profile = {
    /**/ ACTOR_EN_SNOWMAN,
    /**/ ACTORCAT_ENEMY,
    /**/ FLAGS,
    /**/ OBJECT_SNOWMAN,
    /**/ sizeof(EnSnowman),
    /**/ EnSnowman_Init,
    /**/ EnSnowman_Destroy,
    /**/ EnSnowman_Update,
    /**/ EnSnowman_Draw,
};

static ColliderCylinderInit sEenoCylinderInit = {
    {
        COL_MATERIAL_HIT4,
        AT_NONE,
        AC_ON | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_1,
        COLSHAPE_CYLINDER,
    },
    {
        ELEM_MATERIAL_UNK0,
        { 0.0f, 0x00, 0x00 },
        { 0xF7CFFFFF, 0x00, 0x00 },
        ATELEM_NONE | ATELEM_SFX_NORMAL,
        ACELEM_ON | ACELEM_HOOKABLE,
        OCELEM_ON,
    },
    { 60, 80, 0, { 0, 0, 0 } },
};

static ColliderCylinderInit sSnowballCylinderInit = {
    {
        COL_MATERIAL_NONE,
        AT_ON | AT_TYPE_ENEMY,
        AC_ON | AC_TYPE_PLAYER,
        OC1_NONE | OC1_TYPE_ALL,
        OC2_TYPE_1,
        COLSHAPE_CYLINDER,
    },
    {
        ELEM_MATERIAL_UNK0,
        { 0xF7CFFFFF, 0x00, 0x04 },
        { 0xF7CFFFFF, 0x00, 0x00 },
        ATELEM_ON | ATELEM_SFX_NONE,
        ACELEM_ON,
        OCELEM_ON,
    },
    { 60, 80, 0, { 0, 0, 0 } },
};

typedef enum {
    /* 0x0 */ EN_SNOWMAN_DMGEFF_NONE,
    /* 0x1 */ EN_SNOWMAN_DMGEFF_STUN,
    /* 0x2 */ EN_SNOWMAN_DMGEFF_MELT,
    /* 0x4 */ EN_SNOWMAN_DMGEFF_LIGHT_ORB = 0x4,
    /* 0x5 */ EN_SNOWMAN_DMGEFF_ELECTRIC_STUN,
    /* 0xF */ EN_SNOWMAN_DMGEFF_HOOKSHOT = 0xF // Damages small Eenos, pulls the player towards large Eenos
} EnTalkGibudDamageEffect;

static DamageTable sDamageTable = {
    /* Deku Nut       */ DMG_ENTRY(0, EN_SNOWMAN_DMGEFF_STUN),
    /* Deku Stick     */ DMG_ENTRY(1, EN_SNOWMAN_DMGEFF_NONE),
    /* Horse trample  */ DMG_ENTRY(1, EN_SNOWMAN_DMGEFF_NONE),
    /* Explosives     */ DMG_ENTRY(1, EN_SNOWMAN_DMGEFF_NONE),
    /* Zora boomerang */ DMG_ENTRY(1, EN_SNOWMAN_DMGEFF_NONE),
    /* Normal arrow   */ DMG_ENTRY(1, EN_SNOWMAN_DMGEFF_NONE),
    /* UNK_DMG_0x06   */ DMG_ENTRY(0, EN_SNOWMAN_DMGEFF_NONE),
    /* Hookshot       */ DMG_ENTRY(1, EN_SNOWMAN_DMGEFF_HOOKSHOT),
    /* Goron punch    */ DMG_ENTRY(2, EN_SNOWMAN_DMGEFF_NONE),
    /* Sword          */ DMG_ENTRY(1, EN_SNOWMAN_DMGEFF_NONE),
    /* Goron pound    */ DMG_ENTRY(1, EN_SNOWMAN_DMGEFF_NONE),
    /* Fire arrow     */ DMG_ENTRY(2, EN_SNOWMAN_DMGEFF_MELT),
    /* Ice arrow      */ DMG_ENTRY(1, EN_SNOWMAN_DMGEFF_NONE),
    /* Light arrow    */ DMG_ENTRY(2, EN_SNOWMAN_DMGEFF_LIGHT_ORB),
    /* Goron spikes   */ DMG_ENTRY(1, EN_SNOWMAN_DMGEFF_NONE),
    /* Deku spin      */ DMG_ENTRY(0, EN_SNOWMAN_DMGEFF_STUN),
    /* Deku bubble    */ DMG_ENTRY(1, EN_SNOWMAN_DMGEFF_NONE),
    /* Deku launch    */ DMG_ENTRY(2, EN_SNOWMAN_DMGEFF_NONE),
    /* UNK_DMG_0x12   */ DMG_ENTRY(0, EN_SNOWMAN_DMGEFF_STUN),
    /* Zora barrier   */ DMG_ENTRY(0, EN_SNOWMAN_DMGEFF_ELECTRIC_STUN),
    /* Normal shield  */ DMG_ENTRY(0, EN_SNOWMAN_DMGEFF_NONE),
    /* Light ray      */ DMG_ENTRY(0, EN_SNOWMAN_DMGEFF_NONE),
    /* Thrown object  */ DMG_ENTRY(1, EN_SNOWMAN_DMGEFF_NONE),
    /* Zora punch     */ DMG_ENTRY(1, EN_SNOWMAN_DMGEFF_NONE),
    /* Spin attack    */ DMG_ENTRY(1, EN_SNOWMAN_DMGEFF_NONE),
    /* Sword beam     */ DMG_ENTRY(0, EN_SNOWMAN_DMGEFF_NONE),
    /* Normal Roll    */ DMG_ENTRY(0, EN_SNOWMAN_DMGEFF_NONE),
    /* UNK_DMG_0x1B   */ DMG_ENTRY(0, EN_SNOWMAN_DMGEFF_NONE),
    /* UNK_DMG_0x1C   */ DMG_ENTRY(0, EN_SNOWMAN_DMGEFF_NONE),
    /* Unblockable    */ DMG_ENTRY(0, EN_SNOWMAN_DMGEFF_NONE),
    /* UNK_DMG_0x1E   */ DMG_ENTRY(0, EN_SNOWMAN_DMGEFF_NONE),
    /* Powder Keg     */ DMG_ENTRY(1, EN_SNOWMAN_DMGEFF_NONE),
};

static CollisionCheckInfoInit sColChkInfoInit = { 2, 60, 80, 150 };

static Color_RGBA8 sDustPrimColor = { 250, 250, 250, 255 };

static Color_RGBA8 sDustEnvColor = { 180, 180, 180, 255 };

static Vec3f sDustVelocity = { 0.0f, 1.5f, 0.0f };

static Gfx* sSnowballDLs[] = {
    gEenoSmallSnowballDL,
    gEenoLargeSnowballDL,
    gEenoSmallSnowballDL,
};

static Gfx* sSnowballFragmentDLs[] = {
    gEenoSnowballFragment1DL,
    gEenoSnowballFragment2DL,
    gEenoSnowballFragment3DL,
};

static InitChainEntry sInitChain[] = {
    ICHAIN_S8(hintId, TATL_HINT_ID_EENO, ICHAIN_CONTINUE),
    ICHAIN_F32(lockOnArrowOffset, 3000, ICHAIN_CONTINUE),
    ICHAIN_F32_DIV1000(gravity, -1000, ICHAIN_STOP),
};

void EnSnowman_Init(Actor* thisx, PlayState* play) {
    s32 pad;
    EnSnowman* this = (EnSnowman*)thisx;
    s32 attackRange;

    Actor_ProcessInitChain(thisx, sInitChain);
    attackRange = EN_SNOWMAN_GET_ATTACK_RANGE(thisx);
    if (attackRange == 0xFF) {
        attackRange = 0;
    }

    thisx->params &= 7;
    if (EN_SNOWMAN_GET_TYPE(thisx) < EN_SNOWMAN_TYPE_SMALL_SNOWBALL) {
        SkelAnime_InitFlex(play, &this->skelAnime, &gEenoSkel, &gEenoEmergeAnim, this->jointTable, this->morphTable,
                           EENO_LIMB_MAX);
        SkelAnime_InitFlex(play, &this->snowPileSkelAnime, &gEenoSnowPileSkel, &gEenoSnowPileMoveAnim,
                           this->snowPileJointTable, this->snowPileMorphTable, EENO_SNOW_PILE_LIMB_MAX);
        CollisionCheck_SetInfo(&thisx->colChkInfo, &sDamageTable, &sColChkInfoInit);
        Collider_InitAndSetCylinder(play, &this->collider, thisx, &sEenoCylinderInit);
        if (EN_SNOWMAN_GET_TYPE(thisx) == EN_SNOWMAN_TYPE_LARGE) {
            thisx->flags |= ACTOR_FLAG_HOOKSHOT_PULLS_PLAYER;
            Actor_SpawnAsChild(&play->actorCtx, thisx, play, ACTOR_EN_SNOWMAN, thisx->world.pos.x, thisx->world.pos.y,
                               thisx->world.pos.z, 0, 0, 0, EN_SNOWMAN_TYPE_SPLIT);
            thisx->parent = Actor_SpawnAsChildAndCutscene(&play->actorCtx, play, ACTOR_EN_SNOWMAN, thisx->world.pos.x,
                                                          thisx->world.pos.y, thisx->world.pos.z, 0, 0, 0,
                                                          EN_SNOWMAN_TYPE_SPLIT, CS_ID_NONE, thisx->halfDaysBits, NULL);
            if ((thisx->child == NULL) || (thisx->parent == NULL)) {
                if (thisx->child != NULL) {
                    Actor_Kill(thisx->child);
                }

                if (thisx->parent != NULL) {
                    Actor_Kill(thisx->parent);
                }

                Actor_Kill(thisx);
                return;
            }

            // Makes each split Eeno the parent or child of other split Eenos
            thisx->parent->child = thisx;
            thisx->child->child = thisx->parent;
            thisx->parent->parent = thisx->child;

            //! FAKE:
            if (1) {}

            Actor_SetScale(thisx, 0.02f);
        }

        this->eenoScale = thisx->scale.x * 100.0f;
        this->attackRange = (240.0f * this->eenoScale) + (attackRange * 0.1f * 40.0f);
        if (EN_SNOWMAN_GET_TYPE(thisx) == EN_SNOWMAN_TYPE_SPLIT) {
            EnSnowman_SetupSplitDoNothing(this);
        } else {
            EnSnowman_SetupMoveSnowPile(this);
        }
    } else {
        Player* player = GET_PLAYER(play);

        thisx->flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
        Collider_InitAndSetCylinder(play, &this->collider, thisx, &sSnowballCylinderInit);
        thisx->world.rot.y = Actor_WorldYawTowardActor(thisx, &player->actor);
        thisx->velocity.y = (Actor_WorldDistXZToActor(thisx, &player->actor) * 0.035f) + -5.0f;
        thisx->velocity.y = CLAMP_MAX(thisx->velocity.y, 3.5f);
        if (EN_SNOWMAN_GET_TYPE(thisx) == EN_SNOWMAN_TYPE_SMALL_SNOWBALL) {
            thisx->speed = 15.0f;
        } else {
            thisx->speed = 22.5f;
            thisx->velocity.y *= 1.5f;
        }

        thisx->world.pos.x += thisx->speed * Math_SinS(thisx->world.rot.y);
        thisx->world.pos.y += thisx->velocity.y;
        thisx->world.pos.z += thisx->speed * Math_CosS(thisx->world.rot.y);

        if (EN_SNOWMAN_GET_TYPE(thisx) == EN_SNOWMAN_TYPE_SMALL_SNOWBALL) {
            this->collider.dim.radius = 8;
            this->collider.dim.height = 12;
            this->collider.dim.yShift = -6;
            ActorShape_Init(&thisx->shape, 0.0f, ActorShadow_DrawCircle, 10.0f);
        } else {
            this->collider.dim.radius = 50;
            this->collider.dim.height = 122;
            this->collider.dim.yShift = -8;
            this->collider.elem.atDmgInfo.damage = 16;
            thisx->world.pos.y -= 32.0f;
            Actor_SetScale(thisx, 0.006f);
            ActorShape_Init(&thisx->shape, 16000.0f / 3.0f, ActorShadow_DrawCircle, 170.0f);
            thisx->gravity = -1.5f;
        }

        thisx->flags |= ACTOR_FLAG_UPDATE_CULLING_DISABLED;
        thisx->update = EnSnowman_UpdateSnowball;
        thisx->draw = EnSnowman_DrawSnowball;
        this->work.timer = 5;
    }
}

void EnSnowman_Destroy(Actor* thisx, PlayState* play) {
    EnSnowman* this = (EnSnowman*)thisx;

    Collider_DestroyCylinder(play, &this->collider);
}

/**
 * Spawns a circle of snow-colored dust around the actor.
 */
void EnSnowman_SpawnCircularDustEffect(EnSnowman* this, PlayState* play) {
    s16 angle = 0;
    Vec3f pos;
    f32 offset;
    s32 i;

    pos.y = (Rand_ZeroFloat(10.0f) * this->eenoScale) + this->actor.world.pos.y;
    for (i = 0; i < 16; i++) {
        offset = (Rand_ZeroFloat(10.0f) + 20.0f) * this->eenoScale;
        pos.x = (Math_SinS(angle) * offset) + this->actor.world.pos.x;
        pos.z = (Math_CosS(angle) * offset) + this->actor.world.pos.z;
        func_800B0DE0(play, &pos, &sDustVelocity, &gZeroVec3f, &sDustPrimColor, &sDustEnvColor,
                      this->eenoScale * 400.0f, 10);
        angle += 0x1000;
    }
}

/**
 * Spawns a batch of large snowball fragments and some snow-colored dust at snowballPos.
 */
void EnSnowman_SpawnBigSnowballFragmentEffects(EnSnowman* this, PlayState* play) {
    static Vec3f sAccel = { 0.0f, -1.0f, 0.0f };
    s16 altitude;
    s16 azimuth;
    Vec3f pos;
    Vec3f velocity;
    f32 speed;
    s32 i;

    for (i = 0; i < 15; i++) {
        altitude = Rand_S16Offset(0x1800, 0x2800);
        azimuth = Rand_Next() >> 0x10;
        speed = Rand_ZeroFloat(3.0f) + 8.0f;
        velocity.x = (speed * Math_CosS(altitude)) * Math_SinS(azimuth);
        velocity.y = speed * Math_SinS(altitude);
        velocity.z = (speed * Math_CosS(altitude)) * Math_CosS(azimuth);
        pos.x = (Rand_ZeroFloat(10.0f) * velocity.x) + this->snowballPos.x;
        pos.y = (Rand_ZeroFloat(8.0f) * velocity.y) + this->snowballPos.y;
        pos.z = (Rand_ZeroFloat(10.0f) * velocity.z) + this->snowballPos.z;
        EffectSsHahen_Spawn(play, &pos, &velocity, &sAccel, 0,
                            Rand_S16Offset((((i % 3) * 50) + 50), (((i % 3) * 25) + 25)), OBJECT_SNOWMAN, 20,
                            sSnowballFragmentDLs[i % 3]);
    }

    func_800B0DE0(play, &this->snowballPos, &gZeroVec3f, &gZeroVec3f, &sDustPrimColor, &sDustEnvColor, 1000, 150);
}

void EnSnowman_SetupMoveSnowPile(EnSnowman* this) {
    Animation_PlayLoop(&this->snowPileSkelAnime, &gEenoSnowPileMoveAnim);
    this->actor.scale.y = this->actor.scale.x;
    this->actor.speed = 2.0f;
    this->actor.draw = EnSnowman_DrawSnowPile;
    this->work.timer = 40;
    this->turningOnSteepSlope = false;
    this->collider.dim.radius = this->eenoScale * 30.0f;
    this->collider.dim.height = this->eenoScale * 10.0f;
    this->actionFunc = EnSnowman_MoveSnowPile;
}

/**
 * Moves around beneath the snow as a snow pile.
 */
void EnSnowman_MoveSnowPile(EnSnowman* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    Vec3f combinePos;

    SkelAnime_Update(&this->snowPileSkelAnime);
    if (EN_SNOWMAN_GET_TYPE(&this->actor) == EN_SNOWMAN_TYPE_LARGE) {
        Actor_PlaySfx_Flagged(&this->actor, NA_SE_EN_YMAJIN_MOVE - SFX_FLAG);
    } else {
        Actor_PlaySfx_Flagged(&this->actor, NA_SE_EN_YMAJIN_MINI_MOVE - SFX_FLAG);
    }

    if (this->work.timer > 0) {
        this->work.timer--;
    }

    if (this->work.timer == 0) {
        this->collider.base.acFlags |= AC_ON;
    }

    if ((this->combineTimer == 0) && (EN_SNOWMAN_GET_TYPE(&this->actor) == EN_SNOWMAN_TYPE_SPLIT)) {
        // Sets the combinePos to be the average position of all living split Eenos.
        if ((this->actor.parent->colChkInfo.health != 0) && (this->actor.child->colChkInfo.health != 0)) {
            combinePos.x =
                (this->actor.parent->world.pos.x + this->actor.child->world.pos.x + this->actor.world.pos.x) *
                (1.0f / 3.0f);
            combinePos.z =
                (this->actor.parent->world.pos.z + this->actor.child->world.pos.z + this->actor.world.pos.z) *
                (1.0f / 3.0f);
        } else if (this->actor.parent->colChkInfo.health != 0) {
            combinePos.x = (this->actor.parent->world.pos.x + this->actor.world.pos.x) / 2.0f;
            combinePos.z = (this->actor.parent->world.pos.z + this->actor.world.pos.z) / 2.0f;
        } else if (this->actor.child->colChkInfo.health != 0) {
            combinePos.x = (this->actor.child->world.pos.x + this->actor.world.pos.x) / 2.0f;
            combinePos.z = (this->actor.child->world.pos.z + this->actor.world.pos.z) / 2.0f;
        } else {
            combinePos.x = this->actor.world.pos.x;
            combinePos.z = this->actor.world.pos.z;
        }

        combinePos.y = this->actor.world.pos.y;
        EnSnowman_SetupCombine((EnSnowman*)this->actor.parent, play, &combinePos);
        EnSnowman_SetupCombine((EnSnowman*)this->actor.child, play, &combinePos);
        EnSnowman_SetupCombine(this, play, &combinePos);
    } else if ((this->work.timer == 0) && (fabsf(this->actor.playerHeightRel) < 60.0f) &&
               (this->actor.xzDistToPlayer < this->attackRange) && (Player_GetMask(play) != PLAYER_MASK_STONE) &&
               !(player->stateFlags1 & PLAYER_STATE1_800000)) {
        EnSnowman_SetupEmerge(this, play);
    } else if (this->snowPileTargetRotY != this->actor.shape.rot.y) {
        if (Math_ScaledStepToS(&this->actor.shape.rot.y, this->snowPileTargetRotY, 0x100)) {
            this->turningOnSteepSlope = false;
        }

        this->actor.world.rot.y = this->actor.shape.rot.y;
    } else if (this->actor.bgCheckFlags & BGCHECKFLAG_WALL) {
        this->snowPileTargetRotY = this->actor.wallYaw;
    } else if (Actor_WorldDistXZToPoint(&this->actor, &this->actor.home.pos) > 200.0f) {
        this->snowPileTargetRotY =
            Actor_WorldYawTowardPoint(&this->actor, &this->actor.home.pos) + ((s32)Rand_Next() >> 0x14);
    } else if (Rand_ZeroOne() < 0.02f) {
        this->snowPileTargetRotY += (s16)(((Rand_Next() >> 0x13) + 0x1000) * ((Rand_ZeroOne() < 0.5f) ? -1 : 1));
    }
}

void EnSnowman_SetupEmerge(EnSnowman* this, PlayState* play) {
    Animation_PlayOnce(&this->skelAnime, &gEenoEmergeAnim);
    Actor_PlaySfx(&this->actor, NA_SE_EN_YMAJIN_SURFACE);
    this->collider.dim.radius = this->eenoScale * 40.0f;
    this->collider.dim.height = this->eenoScale * 25.0f;
    this->actor.draw = EnSnowman_Draw;
    this->actor.scale.y = this->actor.scale.x * 0.4f;
    this->actor.speed = 0.0f;
    this->actor.shape.rot.y = this->actor.yawTowardsPlayer;
    EnSnowman_SpawnCircularDustEffect(this, play);
    this->collider.base.acFlags &= ~AC_ON;
    this->actionFunc = EnSnowman_Emerge;
}

/**
 * Emerge from beneath the snow and face the player.
 */
void EnSnowman_Emerge(EnSnowman* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    Math_StepToF(&this->actor.scale.y, this->actor.scale.x,
                 (this->actor.scale.x * 0.6f) / Animation_GetLastFrame(&gEenoEmergeAnim));
    Math_ApproachS(&this->actor.shape.rot.y, this->actor.yawTowardsPlayer, 0xA, 0x1000);

    if (SkelAnime_Update(&this->skelAnime)) {
        if (this->combineState == EN_SNOWMAN_COMBINE_STATE_ACTIVE) {
            EnSnowman_SetupSubmerge(this, play);
        } else if (!(player->stateFlags1 & PLAYER_STATE1_800000) && (Player_GetMask(play) != PLAYER_MASK_STONE)) {
            this->collider.base.acFlags |= AC_ON;
            this->work.snowballsToThrowBeforeIdling = 3;
            EnSnowman_SetupReadySnowball(this);
        } else {
            this->collider.base.acFlags |= AC_ON;
            this->actor.scale.y = this->actor.scale.x;
            EnSnowman_SetupIdle(this);
        }
    }
}

void EnSnowman_SetupReadySnowball(EnSnowman* this) {
    this->actor.scale.y = this->actor.scale.x;
    if (EN_SNOWMAN_GET_TYPE(&this->actor) == EN_SNOWMAN_TYPE_LARGE) {
        this->fwork.frameToStartHoldingSnowball = 15.0f;
        Animation_PlayOnce(&this->skelAnime, &gEenoLargeSnowballCreateAnim);
    } else {
        this->fwork.frameToStartHoldingSnowball = 6.0f;
        Animation_PlayOnce(&this->skelAnime, &gEenoSmallSnowballCreateAnim);
    }

    this->actionFunc = EnSnowman_ReadySnowball;
}

/**
 * Plays the animation of the Eeno creating a snowball and spawns some snow-colored dust
 * near the snowball at the appropriate part of their animation.
 */
void EnSnowman_ReadySnowball(EnSnowman* this, PlayState* play) {
    Vec3f pos;

    Math_ApproachS(&this->actor.shape.rot.y, this->actor.yawTowardsPlayer, 0xA, 0x1000);
    if ((EN_SNOWMAN_GET_TYPE(&this->actor) != EN_SNOWMAN_TYPE_LARGE) && this->isHoldingSnowball &&
        ((play->gameplayFrames % 2) != 0)) {
        pos.x = Rand_CenteredFloat(10.0f) + this->snowballPos.x;
        pos.y = Rand_CenteredFloat(10.0f) + this->snowballPos.y;
        pos.z = Rand_CenteredFloat(10.0f) + this->snowballPos.z;
        func_800B0DE0(play, &pos, &sDustVelocity, &gZeroVec3f, &sDustPrimColor, &sDustEnvColor, 500, 30);
    } else if (EN_SNOWMAN_GET_TYPE(&this->actor) == EN_SNOWMAN_TYPE_LARGE) {
        if ((this->skelAnime.curFrame > 3.0f) && (this->skelAnime.curFrame < 14.0f) &&
            ((play->gameplayFrames % 2) != 0)) {
            pos.x =
                (this->actor.world.pos.x + (70.0f * Math_SinS(this->actor.shape.rot.y))) + Rand_CenteredFloat(40.0f);
            pos.y = this->actor.world.pos.y + Rand_CenteredFloat(20.0f);
            pos.z =
                (this->actor.world.pos.z + (70.0f * Math_CosS(this->actor.shape.rot.y))) + Rand_CenteredFloat(40.0f);
            func_800B0DE0(play, &pos, &sDustVelocity, &gZeroVec3f, &sDustPrimColor, &sDustEnvColor, 1000, 150);
        }
    }

    if (SkelAnime_Update(&this->skelAnime)) {
        EnSnowman_SetupThrowSnowball(this);
    } else if (Animation_OnFrame(&this->skelAnime, this->fwork.frameToStartHoldingSnowball)) {
        if (EN_SNOWMAN_GET_TYPE(&this->actor) == EN_SNOWMAN_TYPE_LARGE) {
            Actor_PlaySfx(&this->actor, NA_SE_EN_YMAJIN_HOLD_SNOW);
        } else {
            Actor_PlaySfx(&this->actor, NA_SE_EN_YMAJIN_MINI_HOLD);
        }

        this->isHoldingSnowball = true;
    }
}

void EnSnowman_SetupThrowSnowball(EnSnowman* this) {
    if (EN_SNOWMAN_GET_TYPE(&this->actor) == EN_SNOWMAN_TYPE_LARGE) {
        Animation_PlayOnce(&this->skelAnime, &gEenoLargeSnowballThrowAnim);
        this->fwork.frameToThrowSnowball = 17.0f;
    } else {
        Animation_PlayOnce(&this->skelAnime, &gEenoSmallSnowballThrowAnim);
        this->fwork.frameToThrowSnowball = 15.0f;
    }

    this->work.snowballsToThrowBeforeIdling--;
    this->actionFunc = EnSnowman_ThrowSnowball;
}

/**
 * Throws the snowball the Eeno is holding at the player. If there are still more snowballs
 * to throw before idling, this function is responsible for calling EnSnowman_SetupReadySnowball
 * so it can eventually throw another one. Otherwise, this function will call EnSnowman_SetupIdle
 * to transition the Eeno into an idle state.
 */
void EnSnowman_ThrowSnowball(EnSnowman* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    s32 params;

    Math_ApproachS(&this->actor.shape.rot.y, this->actor.yawTowardsPlayer, 0xA, 0x1000);
    if (SkelAnime_Update(&this->skelAnime)) {
        if ((this->work.snowballsToThrowBeforeIdling != 0) && (Player_GetMask(play) != PLAYER_MASK_STONE) &&
            !(player->stateFlags1 & PLAYER_STATE1_800000)) {
            EnSnowman_SetupReadySnowball(this);
        } else {
            this->work.snowballsToThrowBeforeIdling = 0;
            EnSnowman_SetupIdle(this);
        }
    } else if (Animation_OnFrame(&this->skelAnime, this->fwork.frameToThrowSnowball)) {
        this->isHoldingSnowball = false;
        if (EN_SNOWMAN_GET_TYPE(&this->actor) == EN_SNOWMAN_TYPE_LARGE) {
            params = EN_SNOWMAN_TYPE_LARGE_SNOWBALL;
            Actor_PlaySfx(&this->actor, NA_SE_EN_YMAJIN_THROW);
        } else {
            params = EN_SNOWMAN_TYPE_SMALL_SNOWBALL;
            Actor_PlaySfx(&this->actor, NA_SE_EN_YMAJIN_MINI_THROW);
        }

        Actor_Spawn(&play->actorCtx, play, ACTOR_EN_SNOWMAN, this->snowballPos.x, this->snowballPos.y,
                    this->snowballPos.z, 0, this->actor.yawTowardsPlayer, 0, params);
    }
}

void EnSnowman_SetupIdle(EnSnowman* this) {
    Animation_PlayLoop(&this->skelAnime, &gEenoIdleAnim);
    this->work.timer = 60;
    this->actionFunc = EnSnowman_Idle;
}

/**
 * Simply makes the Eeno wait around for a bit before submerging back into the snow.
 */
void EnSnowman_Idle(EnSnowman* this, PlayState* play) {
    SkelAnime_Update(&this->skelAnime);
    if (this->work.timer > 0) {
        this->work.timer--;
    } else {
        EnSnowman_SetupSubmerge(this, play);
    }
}

void EnSnowman_SetupSubmerge(EnSnowman* this, PlayState* play) {
    Animation_Change(&this->skelAnime, &gEenoEmergeAnim, -1.0f, Animation_GetLastFrame(&gEenoEmergeAnim), 0.0f,
                     ANIMMODE_ONCE, -3.0f);
    EnSnowman_SpawnCircularDustEffect(this, play);
    Actor_PlaySfx(&this->actor, NA_SE_EN_YMAJIN_HIDE);
    this->actionFunc = EnSnowman_Submerge;
}

/**
 * Submerge beneath the snow and become a snow pile.
 */
void EnSnowman_Submerge(EnSnowman* this, PlayState* play) {
    Math_StepToF(&this->actor.scale.y, this->actor.scale.x * 0.4f,
                 (this->actor.scale.x * 0.6f) / Animation_GetLastFrame(&gEenoEmergeAnim));

    if (SkelAnime_Update(&this->skelAnime)) {
        if (this->combineState == EN_SNOWMAN_COMBINE_STATE_ACTIVE) {
            this->actor.draw = EnSnowman_DrawSnowPile;
            this->collider.base.acFlags |= AC_ON;

            // Calling EnSnowman_SetupCombine while EnSnowman_Submerge is our actionFunc will result
            // in the broken target scale bug described in EnSnowman_SetupCombine.
            EnSnowman_SetupCombine(this, play, &this->combinePos);
        } else {
            EnSnowman_SetupMoveSnowPile(this);
        }
    }
}

void EnSnowman_SetupMelt(EnSnowman* this) {
    Actor_SetColorFilter(&this->actor, COLORFILTER_COLORFLAG_RED, 255, COLORFILTER_BUFFLAG_OPA, 50);
    this->collider.base.acFlags &= ~AC_ON;
    this->work.timer = 50;
    this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
    this->actor.flags |= ACTOR_FLAG_UPDATE_CULLING_DISABLED;
    this->actor.scale.y = this->actor.scale.x;
    this->actor.speed = 0.0f;
    this->actionFunc = EnSnowman_Melt;
}

/**
 * Spawns smoke and shrinks the Eeno down until the timer runs out, then kills the Eeno.
 */
void EnSnowman_Melt(EnSnowman* this, PlayState* play) {
    Vec3f smokeVelocity;
    Vec3f smokePos;

    this->work.timer--;
    if ((this->work.timer >= 38) && !(this->work.timer & 1)) {
        smokeVelocity.y = (this->work.timer - 38) * (1.0f / 12.0f);
        smokeVelocity.x = Rand_CenteredFloat(1.5f) * smokeVelocity.y;
        smokeVelocity.z = Rand_CenteredFloat(1.5f) * smokeVelocity.y;
        smokeVelocity.y += 0.8f;
        smokePos.x = ((smokeVelocity.x >= 0.0f ? 1.0f : -1.0f) * Rand_ZeroFloat(20.0f) * this->eenoScale) +
                     this->actor.world.pos.x;
        smokePos.z = ((smokeVelocity.z >= 0.0f ? 1.0f : -1.0f) * Rand_ZeroFloat(20.0f) * this->eenoScale) +
                     this->actor.world.pos.z;
        smokePos.y = this->actor.world.pos.y + 3.0f;
        EffectSsIceSmoke_Spawn(play, &smokePos, &smokeVelocity, &gZeroVec3f, this->eenoScale * 300.0f);
    }

    if (this->work.timer == 0) {
        Item_DropCollectibleRandom(play, &this->actor, &this->actor.world.pos, 0x60);
        if (EN_SNOWMAN_GET_TYPE(&this->actor) == EN_SNOWMAN_TYPE_SPLIT) {
            EnSnowman_SetupSplitDoNothing(this);
        } else if (EN_SNOWMAN_GET_TYPE(&this->actor) == EN_SNOWMAN_TYPE_LARGE) {
            Actor_Kill(this->actor.parent);
            Actor_Kill(this->actor.child);
            Actor_Kill(&this->actor);
        } else {
            Actor_Kill(&this->actor);
        }
    }

    this->actor.scale.y = this->work.timer * 0.0002f * this->eenoScale;
    this->actor.scale.x = (this->eenoScale * (1.4f * 0.01f)) - (0.4f * this->actor.scale.y);
    this->actor.scale.z = (this->eenoScale * (1.4f * 0.01f)) - (0.4f * this->actor.scale.y);
}

void EnSnowman_SetupStun(EnSnowman* this) {
    if (this->actionFunc != EnSnowman_Stun) {
        this->prevActionFunc = this->actionFunc;
    }

    this->actionFunc = EnSnowman_Stun;
}

/**
 * Simply makes the Eeno do absolutely nothing until the stun is over.
 * Once the stun is over, this swaps the actionFunc back to what it was before the stun.
 */
void EnSnowman_Stun(EnSnowman* this, PlayState* play) {
    if (this->actor.colorFilterTimer == 0) {
        this->actionFunc = this->prevActionFunc;
    }
}

void EnSnowman_SetupDamaged(EnSnowman* this) {
    Animation_PlayLoop(&this->skelAnime, &gEenoDamageAnim);
    Actor_SetColorFilter(&this->actor, COLORFILTER_COLORFLAG_RED, 255, COLORFILTER_BUFFLAG_OPA, 20);
    this->collider.base.acFlags &= ~AC_ON;
    this->work.timer = 20;
    this->actor.draw = EnSnowman_Draw;
    this->actor.scale.y = this->actor.scale.x;
    this->actor.speed = 10.0f;
    func_800BE504(&this->actor, &this->collider);

    if (EN_SNOWMAN_GET_TYPE(&this->actor) == EN_SNOWMAN_TYPE_LARGE) {
        Actor_PlaySfx(&this->actor, NA_SE_EN_YMAJIN_DAMAGE);
    } else {
        Actor_PlaySfx(&this->actor, NA_SE_EN_YMAJIN_MINI_DAMAGE);
    }

    this->actionFunc = EnSnowman_Damaged;
}

/**
 * Plays the damage animation and spins the Eeno around.
 * Once it's done spinning, it will spawn three small split-off Eenos if the Eeno is a large one.
 * Otherwise, it will either submerge underground (if it has health remaining) or die.
 */
void EnSnowman_Damaged(EnSnowman* this, PlayState* play) {
    s32 angularVelocityScale;

    SkelAnime_Update(&this->skelAnime);
    angularVelocityScale = CLAMP_MAX(this->work.timer, 10);
    this->actor.shape.rot.y += angularVelocityScale * 0x300;
    Math_StepToF(&this->actor.speed, 0.0f, 0.5f);

    if (EN_SNOWMAN_GET_TYPE(&this->actor) == EN_SNOWMAN_TYPE_LARGE) {
        Math_StepToF(&this->actor.scale.y, 1.3f * 0.01f, 0.7f * 0.001f);
        this->actor.scale.x = this->actor.scale.y;
        this->actor.scale.z = this->actor.scale.y;
    }

    if (this->work.timer > 0) {
        this->work.timer--;
    } else if (EN_SNOWMAN_GET_TYPE(&this->actor) == EN_SNOWMAN_TYPE_LARGE) {
        Actor_PlaySfx(&this->actor, NA_SE_EN_YMAJIN_SPLIT);
        EnSnowman_SpawnCircularDustEffect(this, play);
        this->drawDmgEffAlpha = 0.0f;
        EnSnowman_CreateSplitEeno((EnSnowman*)this->actor.parent, &this->actor.world.pos,
                                  this->actor.shape.rot.y + 0x10000 / 3);
        EnSnowman_CreateSplitEeno((EnSnowman*)this->actor.child, &this->actor.world.pos,
                                  this->actor.shape.rot.y - 0x10000 / 3);
        EnSnowman_CreateSplitEeno(this, &this->actor.world.pos, this->actor.shape.rot.y);

        // This is a hack to reuse EnSnowman_SpawnBigSnowballFragmentEffects, since it
        // draws the fragments at snowballPos.
        Math_Vec3f_Copy(&this->snowballPos, &this->actor.world.pos);
        EnSnowman_SpawnBigSnowballFragmentEffects(this, play);
    } else if (this->actor.colChkInfo.health != 0) {
        this->collider.base.acFlags |= AC_ON;
        EnSnowman_SetupSubmerge(this, play);
    } else {
        EnSnowman_SetupDead(this);
    }
}

void EnSnowman_SetupDead(EnSnowman* this) {
    this->drawDmgEffAlpha = 0.0f;
    this->actionFunc = EnSnowman_Dead;
}

/**
 * Spawns snow-colored dust and snowball fragments and kills the Eeno.
 */
void EnSnowman_Dead(EnSnowman* this, PlayState* play) {
    static Vec3f sAccel = { 0.0f, -0.5f, 0.0f };
    Vec3f velocity;
    Vec3f pos;
    s16 altitude;
    s16 azimuth;
    f32 speed;
    s32 i;

    for (i = 0; i < 15; i++) {
        altitude = Rand_S16Offset(0x1000, 0x3000);
        azimuth = (Rand_Next() >> 0x10);
        speed = Rand_ZeroFloat(2.0f) + 4.0f;
        velocity.x = (speed * Math_CosS(altitude)) * Math_SinS(azimuth);
        velocity.y = speed * Math_SinS(altitude);
        velocity.z = (speed * Math_CosS(altitude)) * Math_CosS(azimuth);
        pos.x = (Rand_ZeroFloat(6.0f) * velocity.x) + this->actor.world.pos.x;
        pos.y = (Rand_ZeroFloat(3.0f) * velocity.y) + this->actor.world.pos.y;
        pos.z = (Rand_ZeroFloat(6.0f) * velocity.z) + this->actor.world.pos.z;
        EffectSsHahen_Spawn(play, &pos, &velocity, &sAccel, 0,
                            Rand_S16Offset((((i % 3) * 20) + 20), (((i % 3) * 10) + 10)), OBJECT_SNOWMAN, 20,
                            sSnowballFragmentDLs[i % 3]);
    }

    func_800B0DE0(play, &this->actor.world.pos, &gZeroVec3f, &gZeroVec3f, &sDustPrimColor, &sDustEnvColor, 500, 30);
    SoundSource_PlaySfxAtFixedWorldPos(play, &this->actor.world.pos, 30, NA_SE_EN_YMAJIN_DEAD_BREAK);
    Item_DropCollectibleRandom(play, &this->actor, &this->actor.world.pos, 0x60);
    if (EN_SNOWMAN_GET_TYPE(&this->actor) == EN_SNOWMAN_TYPE_SPLIT) {
        EnSnowman_SetupSplitDoNothing(this);
    } else {
        Actor_Kill(&this->actor);
    }
}

/**
 * There are some cases where a split Eeno shouldn't do anything, but we don't want to mark it for death.
 * This is because we want to keep it around until *all* split Eenos are killed.
 *
 * This function is called whenever we want to put a split Eeno into this "do nothing" state. It disables
 * collision and turns off drawing for the Eeno. If all three split Eenos are killed, this function is
 * responsible for marking all of them for death.
 */
void EnSnowman_SetupSplitDoNothing(EnSnowman* this) {
    this->collider.base.acFlags &= ~AC_HIT;
    this->collider.base.acFlags &= ~AC_HIT;
    this->actor.draw = NULL;
    if (this->combineState == EN_SNOWMAN_COMBINE_STATE_ACTIVE) {
        this->combineState = EN_SNOWMAN_COMBINE_STATE_NO_ABSORPTION;
    }

    this->actor.flags &= ~(ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_UPDATE_CULLING_DISABLED);
    if ((this->actor.parent != NULL) && (((EnSnowman*)this->actor.parent)->actionFunc == EnSnowman_SplitDoNothing)) {
        if ((this->actor.child != NULL) && (((EnSnowman*)this->actor.child)->actionFunc == EnSnowman_SplitDoNothing)) {
            EnSnowman_SetupKill((EnSnowman*)this->actor.parent);
            EnSnowman_SetupKill((EnSnowman*)this->actor.child);
            Actor_Kill(&this->actor);
        }
    }

    this->actionFunc = EnSnowman_SplitDoNothing;
}

void EnSnowman_SplitDoNothing(EnSnowman* this, PlayState* play) {
}

/**
 * Turns off collision on the Eeno before calling Actor_Kill on it.
 */
void EnSnowman_SetupKill(EnSnowman* this) {
    this->collider.base.acFlags &= ~(AC_ON | AC_HIT);
    this->collider.base.ocFlags1 &= ~(OC1_ON | OC1_HIT);
    this->actionFunc = EnSnowman_Kill;
}

/**
 * Simply calls Actor_Kill on the Eeno.
 */
void EnSnowman_Kill(EnSnowman* this, PlayState* play) {
    Actor_Kill(&this->actor);
}

/**
 * Creates a small split Eeno with a certain Y-rotation and at a certain offset from basePos.
 */
void EnSnowman_CreateSplitEeno(EnSnowman* this, Vec3f* basePos, s32 yRot) {
    this->actor.flags |= ACTOR_FLAG_ATTENTION_ENABLED;
    Actor_SetScale(&this->actor, 0.01f);
    this->actor.shape.rot.y = yRot;
    this->actor.world.rot.y = this->actor.shape.rot.y;
    this->combineState = EN_SNOWMAN_COMBINE_STATE_NONE;
    this->actor.colChkInfo.health = 2;
    this->eenoScale = 1.0f;
    this->actor.world.pos.x = (Math_SinS(yRot) * 40.0f) + basePos->x;
    this->actor.world.pos.y = basePos->y;
    this->actor.world.pos.z = (Math_CosS(yRot) * 40.0f) + basePos->z;
    this->combineTimer = 600;
    this->actor.params = EN_SNOWMAN_TYPE_SPLIT;
    this->actor.flags &= ~ACTOR_FLAG_HOOKSHOT_PULLS_PLAYER;
    this->collider.base.ocFlags1 |= OC1_ON;
    this->collider.base.acFlags &= ~AC_ON;
    EnSnowman_SetupMoveSnowPile(this);
}

/**
 * Absorbs the smaller Eeno into the larger one. The smaller Eeno's target scale is set to
 * zero so it gradually disappears, and its collision is disabled, while the larger Eeno's
 * target scale increases.
 */
void EnSnowman_AbsorbEeno(EnSnowman* smallerEeno, EnSnowman* largerEeno) {
    Actor_PlaySfx(&largerEeno->actor, NA_SE_EN_YMAJIN_UNITE);
    largerEeno->fwork.targetScaleDuringCombine += 0.005f;
    smallerEeno->combineState = EN_SNOWMAN_COMBINE_STATE_BEING_ABSORBED_OR_DONE;
    smallerEeno->collider.base.ocFlags1 &= ~OC1_HIT;
    smallerEeno->collider.base.acFlags &= ~AC_HIT;
    smallerEeno->collider.base.ocFlags1 &= ~OC1_ON;
    smallerEeno->collider.base.acFlags &= ~AC_ON;
    smallerEeno->fwork.targetScaleDuringCombine = 0.0f;
}

void EnSnowman_SetupCombine(EnSnowman* this, PlayState* play, Vec3f* combinePos) {
    if (this->actor.colChkInfo.health == 0) {
        this->combineState = EN_SNOWMAN_COMBINE_STATE_NO_ABSORPTION;
    } else {
        this->actor.flags |= ACTOR_FLAG_UPDATE_CULLING_DISABLED;
        Math_Vec3f_Copy(&this->combinePos, combinePos);
        this->combineState = EN_SNOWMAN_COMBINE_STATE_ACTIVE;

        if (this->actionFunc != EnSnowman_Submerge) {
            this->combineTimer = 400;

            //! @bug: Skipping this based the current actionFunc results in whatever is currently in fwork
            //! being treated as the target scale when this function is called from EnSnowman_Submerge.
            //! Since the only other things that get put in fwork are animation frame numbers, this results
            //! in an enormous target scale, potentially creating the so-called "Mega Eeno" glitch.
            //! It can still fail to become a Mega Eeno if it gets absorbed before it gets a chance to call
            //! EnSnowman_Combine, though.
            this->fwork.targetScaleDuringCombine = 0.01f;
        }

        //! @bug: If an Eeno is in the middle of submerging, its draw function will still be EnSnowman_Draw.
        //! It will call EnSnowman_SetupSubmerge again, resulting in the submerge animation playing twice.
        if (this->actor.draw == EnSnowman_DrawSnowPile) {
            this->actor.speed = 3.0f;
            this->actionFunc = EnSnowman_Combine;
        } else {
            this->isHoldingSnowball = false;
            this->actor.speed = 0.0f;

            // At this point, the combineState is EN_SNOWMAN_COMBINE_STATE_ACTIVE, and the actionFunc
            // will be set to EnSnowman_Submerge. When the submerging animation is complete with this
            // combineState, EnSnowman_Submerge will call EnSnowman_SetupCombine, causing the broken
            // target scale bug described above.
            EnSnowman_SetupSubmerge(this, play);
        }
    }
}

/**
 * Makes the small, split-off Eenos try to combine into a large Eeno by absorbing each other.
 */
void EnSnowman_Combine(EnSnowman* this, PlayState* play) {
    EnSnowman* parent;
    EnSnowman* child;

    SkelAnime_Update(&this->snowPileSkelAnime);
    parent = (EnSnowman*)this->actor.parent;
    child = (EnSnowman*)this->actor.child;
    Math_ScaledStepToS(&this->actor.shape.rot.y, Actor_WorldYawTowardPoint(&this->actor, &this->combinePos), 0x1000);
    this->actor.world.rot.y = this->actor.shape.rot.y;

    if (this->combineState == EN_SNOWMAN_COMBINE_STATE_ACTIVE) {
        if (this->collider.base.ocFlags1 & OC1_HIT) {
            if ((this->collider.base.oc == this->actor.parent) &&
                (parent->combineState == EN_SNOWMAN_COMBINE_STATE_ACTIVE)) {
                if (this->actor.scale.x < this->actor.parent->scale.x) {
                    EnSnowman_AbsorbEeno(this, parent);
                } else {
                    EnSnowman_AbsorbEeno(parent, this);
                }
            } else if ((this->collider.base.oc == this->actor.child) &&
                       (child->combineState == EN_SNOWMAN_COMBINE_STATE_ACTIVE)) {
                if (this->actor.scale.x < this->actor.child->scale.x) {
                    EnSnowman_AbsorbEeno(this, child);
                } else {
                    EnSnowman_AbsorbEeno(child, this);
                }
            }
        }

        if (parent->combineState == EN_SNOWMAN_COMBINE_STATE_NO_ABSORPTION) {
            EnSnowman_AbsorbEeno(parent, this);
        }

        if (child->combineState == EN_SNOWMAN_COMBINE_STATE_NO_ABSORPTION) {
            EnSnowman_AbsorbEeno(child, this);
        }
    }

    if ((this->combineTimer == 0) && (parent->fwork.targetScaleDuringCombine > 0.0f) &&
        (child->fwork.targetScaleDuringCombine > 0.0f) && (this->fwork.targetScaleDuringCombine < 0.011f) &&
        (this->combineState != EN_SNOWMAN_COMBINE_STATE_BEING_ABSORBED_OR_DONE)) {
        this->combineState = EN_SNOWMAN_COMBINE_STATE_NO_ABSORPTION;
        this->fwork.targetScaleDuringCombine = 0.0f;
    }

    if (Actor_WorldDistXZToPoint(&this->actor, &this->combinePos) < 20.0f) {
        this->actor.speed = 0.0f;
    }

    if (Math_StepToF(&this->actor.scale.x, this->fwork.targetScaleDuringCombine, 0.0005f)) {
        if (this->fwork.targetScaleDuringCombine < 0.01f) {
            EnSnowman_SetupSplitDoNothing(this);
        } else if (this->fwork.targetScaleDuringCombine > 0.018f) {
            Actor_SetScale(&this->actor, 0.02f);
            this->actor.params = EN_SNOWMAN_TYPE_LARGE;
            this->actor.flags |= ACTOR_FLAG_HOOKSHOT_PULLS_PLAYER;
            this->collider.base.ocFlags1 |= OC1_ON;
            this->combineState = EN_SNOWMAN_COMBINE_STATE_BEING_ABSORBED_OR_DONE;
            this->eenoScale = 2.0f;
            EnSnowman_SetupMoveSnowPile(this);
        }
    }

    this->actor.scale.y = this->actor.scale.x;
    this->actor.scale.z = this->actor.scale.x;
}

void EnSnowman_UpdateDamage(EnSnowman* this, PlayState* play) {
    if (this->collider.base.acFlags & AC_HIT) {
        this->collider.base.acFlags &= ~AC_HIT;
        Actor_SetDropFlag(&this->actor, &this->collider.elem);
        if ((this->actor.colChkInfo.damageEffect != EN_SNOWMAN_DMGEFF_HOOKSHOT) ||
            (EN_SNOWMAN_GET_TYPE(&this->actor) != EN_SNOWMAN_TYPE_LARGE)) {
            if (this->actor.colChkInfo.damageEffect == EN_SNOWMAN_DMGEFF_MELT) {
                Enemy_StartFinishingBlow(play, &this->actor);
                Actor_ApplyDamage(&this->actor);
                EnSnowman_SetupMelt(this);
            } else {
                if ((this->actionFunc == EnSnowman_MoveSnowPile) || (this->actionFunc == EnSnowman_Combine)) {
                    EnSnowman_SetupEmerge(this, play);
                } else if (this->actor.colChkInfo.damageEffect == EN_SNOWMAN_DMGEFF_STUN) {
                    Actor_SetColorFilter(&this->actor, COLORFILTER_COLORFLAG_BLUE, 255, COLORFILTER_BUFFLAG_OPA, 40);
                    Actor_PlaySfx(&this->actor, NA_SE_EN_COMMON_FREEZE);
                    EnSnowman_SetupStun(this);
                } else if (this->actor.colChkInfo.damageEffect == EN_SNOWMAN_DMGEFF_ELECTRIC_STUN) {
                    Actor_SetColorFilter(&this->actor, COLORFILTER_COLORFLAG_BLUE, 255, COLORFILTER_BUFFLAG_OPA, 40);
                    this->drawDmgEffScale = 0.55f;
                    this->drawDmgEffAlpha = 2.0f;
                    this->drawDmgEffType = ACTOR_DRAW_DMGEFF_ELECTRIC_SPARKS_LARGE;
                    Actor_PlaySfx(&this->actor, NA_SE_EN_COMMON_FREEZE);
                    EnSnowman_SetupStun(this);
                } else if (EN_SNOWMAN_GET_TYPE(&this->actor) == EN_SNOWMAN_TYPE_LARGE) {
                    if (this->isHoldingSnowball == true) {
                        this->isHoldingSnowball = false;
                        EnSnowman_SpawnBigSnowballFragmentEffects(this, play);
                    }

                    EnSnowman_SetupDamaged(this);
                } else {
                    if (Actor_ApplyDamage(&this->actor) == 0) {
                        Enemy_StartFinishingBlow(play, &this->actor);
                    }

                    EnSnowman_SetupDamaged(this);
                }
            }

            if (this->actor.colChkInfo.damageEffect == EN_SNOWMAN_DMGEFF_LIGHT_ORB) {
                this->drawDmgEffScale = 0.55f;
                this->drawDmgEffAlpha = 4.0f;
                this->drawDmgEffType = ACTOR_DRAW_DMGEFF_LIGHT_ORBS;
                Actor_Spawn(&play->actorCtx, play, ACTOR_EN_CLEAR_TAG, this->collider.elem.acDmgInfo.hitPos.x,
                            this->collider.elem.acDmgInfo.hitPos.y, this->collider.elem.acDmgInfo.hitPos.z, 0, 0, 0,
                            CLEAR_TAG_PARAMS(CLEAR_TAG_LARGE_LIGHT_RAYS));
            }
        }
    }
}

void EnSnowman_Update(Actor* thisx, PlayState* play) {
    s32 pad;
    EnSnowman* this = (EnSnowman*)thisx;
    f32 wallCheckRadius;

    if (this->actionFunc != EnSnowman_SplitDoNothing) {
        DECR(this->combineTimer);

        EnSnowman_UpdateDamage(this, play);
        this->actionFunc(this, play);

        if (this->actionFunc != EnSnowman_Kill) {
            Actor_MoveWithGravity(&this->actor);
            if ((EN_SNOWMAN_GET_TYPE(&this->actor) == EN_SNOWMAN_TYPE_LARGE) &&
                (this->actionFunc == EnSnowman_ReadySnowball)) {
                wallCheckRadius = (this->skelAnime.curFrame * (1.0f / 60.0f)) + 1.0f;
                wallCheckRadius = CLAMP_MAX(wallCheckRadius, 1.3f);
                wallCheckRadius *= this->collider.dim.radius;
            } else {
                wallCheckRadius = this->collider.dim.radius;
            }

            Actor_UpdateBgCheckInfo(play, &this->actor, 30.0f, wallCheckRadius, 0.0f,
                                    UPDBGCHECKINFO_FLAG_1 | UPDBGCHECKINFO_FLAG_4 | UPDBGCHECKINFO_FLAG_8 |
                                        UPDBGCHECKINFO_FLAG_10);
            if ((this->actor.floorPoly != NULL) && ((this->actor.floorPoly->normal.y * SHT_MINV) < 0.7f)) {
                Math_Vec3f_Copy(&this->actor.world.pos, &this->actor.prevPos);
                if (!this->turningOnSteepSlope) {
                    this->snowPileTargetRotY = Math_Atan2S_XY(this->actor.floorPoly->normal.z * SHT_MINV,
                                                              this->actor.floorPoly->normal.x * SHT_MINV);
                    this->turningOnSteepSlope = true;
                }
            } else {
                func_800BE3D0(&this->actor, this->actor.shape.rot.y, &this->actor.shape.rot);
            }

            Collider_UpdateCylinder(&this->actor, &this->collider);
            if (this->collider.base.acFlags & AC_ON) {
                CollisionCheck_SetAC(play, &play->colChkCtx, &this->collider.base);
            }
            CollisionCheck_SetOC(play, &play->colChkCtx, &this->collider.base);

            if (this->actor.draw == EnSnowman_Draw) {
                Actor_SetFocus(&this->actor, this->actor.scale.y * 1800.0f);
            } else {
                Actor_SetFocus(&this->actor, this->actor.scale.y * 720.0f);
            }

            if (this->drawDmgEffAlpha > 0.0f) {
                Math_StepToF(&this->drawDmgEffAlpha, 0.0f, 0.05f);
                this->drawDmgEffScale = (this->drawDmgEffAlpha + 1.0f) * 0.275f;
                this->drawDmgEffScale = CLAMP_MAX(this->drawDmgEffScale, 0.55f);
            }
        }
    }
}

void EnSnowman_UpdateSnowball(Actor* thisx, PlayState* play) {
    EnSnowman* this = (EnSnowman*)thisx;
    s16 scale;
    s32 i;

    if (this->work.timer > 0) {
        this->work.timer--;
    } else {
        this->collider.base.ocFlags1 |= OC1_ON;
    }

    if ((this->actor.bgCheckFlags & BGCHECKFLAG_WALL) || (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND) ||
        (this->actor.bgCheckFlags & BGCHECKFLAG_CEILING) || (this->collider.base.atFlags & AT_HIT) ||
        (this->collider.base.acFlags & AC_HIT) || (this->collider.base.ocFlags1 & OC1_HIT)) {
        if (EN_SNOWMAN_GET_TYPE(&this->actor) == EN_SNOWMAN_TYPE_SMALL_SNOWBALL) {
            scale = 10;
            for (i = 0; i < 3; i++) {
                EffectSsHahen_SpawnBurst(play, &thisx->world.pos, 5.0f, 0, scale, scale >> 1, 3, OBJECT_SNOWMAN, 20,
                                         sSnowballFragmentDLs[i]);
                scale *= 2;
            }

            func_800B0DE0(play, &this->actor.world.pos, &gZeroVec3f, &gZeroVec3f, &sDustPrimColor, &sDustEnvColor, 500,
                          30);
            SoundSource_PlaySfxAtFixedWorldPos(play, &this->actor.world.pos, 20, NA_SE_EV_SMALL_SNOWBALL_BROKEN);
        } else {
            // EnSnowman_SpawnBigSnowballFragmentEffects only works with snowballPos, so we need
            // to copy the snowball actor's current position into snowballPos to make this work.
            Math_Vec3f_Copy(&this->snowballPos, &this->actor.world.pos);
            EnSnowman_SpawnBigSnowballFragmentEffects(this, play);
            SoundSource_PlaySfxAtFixedWorldPos(play, &this->actor.world.pos, 20, NA_SE_EV_SNOWBALL_BROKEN);
        }

        SoundSource_PlaySfxAtFixedWorldPos(play, &this->actor.world.pos, 20, NA_SE_IT_REFLECTION_WOOD);
        Actor_Kill(&this->actor);
        return;
    }

    this->actor.shape.rot.x += 0xF00;
    Actor_MoveWithGravity(&this->actor);
    Actor_UpdateBgCheckInfo(play, &this->actor, 30.0f, this->collider.dim.radius * 0.6f,
                            this->collider.dim.height - this->collider.dim.yShift,
                            UPDBGCHECKINFO_FLAG_1 | UPDBGCHECKINFO_FLAG_2 | UPDBGCHECKINFO_FLAG_4 |
                                UPDBGCHECKINFO_FLAG_8 | UPDBGCHECKINFO_FLAG_10);
    Collider_UpdateCylinder(&this->actor, &this->collider);
    CollisionCheck_SetAT(play, &play->colChkCtx, &this->collider.base);
    CollisionCheck_SetAC(play, &play->colChkCtx, &this->collider.base);
    CollisionCheck_SetOC(play, &play->colChkCtx, &this->collider.base);
}

/**
 * This maps a given limb based on its limbIndex to its appropriate index
 * in the bodyPartsPos array.
 */
static s8 sLimbToBodyParts[EENO_LIMB_MAX] = {
    BODYPART_NONE,         // EENO_LIMB_NONE
    BODYPART_NONE,         // EENO_LIMB_ROOT
    BODYPART_NONE,         // EENO_LIMB_BODY_TOP
    BODYPART_NONE,         // EENO_LIMB_LEFT_EYE
    BODYPART_NONE,         // EENO_LIMB_RIGHT_EYE
    BODYPART_NONE,         // EENO_LIMB_LEFT_UPPER_ARM
    EN_SNOWMAN_BODYPART_0, // EENO_LIMB_LEFT_FOREARM
    EN_SNOWMAN_BODYPART_1, // EENO_LIMB_LEFT_HAND
    BODYPART_NONE,         // EENO_LIMB_RIGHT_UPPER_ARM
    EN_SNOWMAN_BODYPART_2, // EENO_LIMB_RIGHT_FOREARM
    EN_SNOWMAN_BODYPART_3, // EENO_LIMB_RIGHT_HAND
    EN_SNOWMAN_BODYPART_4, // EENO_LIMB_BODY_BOTTOM
};

/**
 * The last five elements of the bodyPartsPos array are duplicates of the body
 * bottom limb, each offset by a certain certain amount.
 */
static Vec3f sBodyBottomBodyPartOffsets[] = {
    { 2000.0f, 3000.0f, 0.0f },  // EN_SNOWMAN_BODYPART_4
    { 2000.0f, -2000.0f, 0.0f }, // EN_SNOWMAN_BODYPART_5
    { 3000.0f, 0.0f, 0.0f },     // EN_SNOWMAN_BODYPART_6
    { 1000.0f, 0.0f, 3000.0f },  // EN_SNOWMAN_BODYPART_7
    { 1000.0f, 0.0f, -3000.0f }, // EN_SNOWMAN_BODYPART_8
};

void EnSnowman_PostLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot, Actor* thisx) {
    s32 pad;
    EnSnowman* this = (EnSnowman*)thisx;
    Gfx* gfx;
    s32 i;

    if (sLimbToBodyParts[limbIndex] != BODYPART_NONE) {
        if (sLimbToBodyParts[limbIndex] == EN_SNOWMAN_BODYPART_4) {
            for (i = 0; i < ARRAY_COUNT(sBodyBottomBodyPartOffsets); i++) {
                Matrix_MultVec3f(&sBodyBottomBodyPartOffsets[i], &this->bodyPartsPos[EN_SNOWMAN_BODYPART_4 + i]);
            }
        } else {
            Matrix_MultZero(&this->bodyPartsPos[sLimbToBodyParts[limbIndex]]);
        }
    }

    if ((limbIndex == EENO_LIMB_RIGHT_HAND) && (this->isHoldingSnowball == true)) {
        OPEN_DISPS(play->state.gfxCtx);

        gfx = POLY_OPA_DISP;

        if (EN_SNOWMAN_GET_TYPE(thisx) != EN_SNOWMAN_TYPE_LARGE) {
            Matrix_Translate(800.0f, -600.0f, 0.0f, MTXMODE_APPLY);
        } else {
            Matrix_Translate(300.0f, -2300.0f, -1900.0f, MTXMODE_APPLY);
            Matrix_Scale(0.3f, 0.3f, 0.3f, MTXMODE_APPLY);
        }

        MATRIX_FINALIZE_AND_LOAD(&gfx[0], play->state.gfxCtx);
        gSPDisplayList(&gfx[1], sSnowballDLs[EN_SNOWMAN_GET_TYPE(&this->actor)]);

        POLY_OPA_DISP = &gfx[2];

        CLOSE_DISPS(play->state.gfxCtx);

        Matrix_MultZero(&this->snowballPos);
    }
}

void EnSnowman_Draw(Actor* thisx, PlayState* play) {
    EnSnowman* this = (EnSnowman*)thisx;

    Gfx_SetupDL25_Opa(play->state.gfxCtx);
    SkelAnime_DrawFlexOpa(play, this->skelAnime.skeleton, this->skelAnime.jointTable, this->skelAnime.dListCount, NULL,
                          EnSnowman_PostLimbDraw, &this->actor);
    Actor_DrawDamageEffects(play, &this->actor, this->bodyPartsPos, EN_SNOWMAN_BODYPART_MAX,
                            this->drawDmgEffScale * this->eenoScale, 0.0f, this->drawDmgEffAlpha, this->drawDmgEffType);
}

void EnSnowman_DrawSnowPile(Actor* thisx, PlayState* play) {
    EnSnowman* this = (EnSnowman*)thisx;

    Gfx_SetupDL25_Opa(play->state.gfxCtx);
    SkelAnime_DrawFlexOpa(play, this->snowPileSkelAnime.skeleton, this->snowPileSkelAnime.jointTable,
                          this->snowPileSkelAnime.dListCount, NULL, NULL, &this->actor);
}

void EnSnowman_DrawSnowball(Actor* thisx, PlayState* play) {
    Gfx_DrawDListOpa(play, sSnowballDLs[EN_SNOWMAN_GET_TYPE(thisx) - EN_SNOWMAN_TYPE_SMALL_SNOWBALL]);
}
