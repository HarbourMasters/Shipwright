/*
 * File: z_en_tanron5.c
 * Overlay: ovl_En_Tanron5
 * Description: Destructible ruins, fragments, and item drops in Twinmold's arena.
 *
 * This actor is responsible for three different interactive props in Twinmold's arena, all of which can be further
 * divided into more categories. The main thing this actor handles are the destructible ruins placed around the arena,
 * of which there are two kinds. There are pillars with Majora's Mask on them, and there are ruins in a roughly
 * pyramidal shape. While both types of ruin behave in roughly the same way, how they implement this behavior can
 * sometimes be quite different.
 *
 * When Twinmold or the player wearing the Giant's Mask hits one of these destructible ruins, various fragments
 * of the ruin fly off from the point of impact. These fragments are handled by this actor as well. The fragments
 * can be large or small, and the two sizes behave almost identically outside of two small differences. Large
 * fragments can damage the player and sink into the sand once they hit the ground, whereas small fragments
 * deal no damage and despawn upon touching the ground.
 *
 * Sometimes, item drops can also appear when a destructible ruin is hit; this actor is responsible for handling
 * these drops too. There are drops that give the player 10 arrows, and drops that give the player a big magic
 * jar, and these drops behave identically outside of what item they give when collected by the player.
 */

#include "z_en_tanron5.h"
#include "objects/gameplay_keep/gameplay_keep.h"
#include "objects/object_boss02/object_boss02.h"
#include "overlays/actors/ovl_Boss_02/z_boss_02.h"

#define FLAGS (ACTOR_FLAG_UPDATE_CULLING_DISABLED | ACTOR_FLAG_DRAW_CULLING_DISABLED)

void EnTanron5_Init(Actor* thisx, PlayState* play);
void EnTanron5_Destroy(Actor* thisx, PlayState* play);
void EnTanron5_Update(Actor* thisx, PlayState* play2);
void EnTanron5_Draw(Actor* thisx, PlayState* play);

void EnTanron5_RuinFragmentItemDrop_Update(Actor* thisx, PlayState* play2);
void EnTanron5_ItemDrop_Draw(Actor* thisx, PlayState* play);

typedef enum TwinmoldPropItemDropType {
    /* 0 */ TWINMOLD_PROP_ITEM_DROP_TYPE_10_ARROWS,
    /* 1 */ TWINMOLD_PROP_ITEM_DROP_TYPE_MAGIC_JAR_BIG
} TwinmoldPropItemDropType;

s32 sFragmentAndItemDropCount = 0;

ActorProfile En_Tanron5_Profile = {
    /**/ ACTOR_EN_TANRON5,
    /**/ ACTORCAT_BOSS,
    /**/ FLAGS,
    /**/ OBJECT_BOSS02,
    /**/ sizeof(EnTanron5),
    /**/ EnTanron5_Init,
    /**/ EnTanron5_Destroy,
    /**/ EnTanron5_Update,
    /**/ EnTanron5_Draw,
};

static ColliderCylinderInit sCylinderInit = {
    {
        COL_MATERIAL_NONE,
        AT_ON | AT_TYPE_ALL,
        AC_ON | AC_TYPE_PLAYER | AC_TYPE_ENEMY | AC_TYPE_OTHER,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_1,
        COLSHAPE_CYLINDER,
    },
    {
        ELEM_MATERIAL_UNK2,
        { 0xF7CFFFFF, 0x00, 0x00 },
        { 0xF7CFFFFF, 0x00, 0x00 },
        ATELEM_ON | ATELEM_SFX_NORMAL,
        ACELEM_ON,
        OCELEM_ON,
    },
    { 70, 450, 0, { 0, 0, 0 } },
};

/**
 * Multiplies the scale differently depending on whether the player is wearing the Giant's Mask or not.
 * When the player is wearing the Giant's Mask, this value is smaller to make the player seem larger.
 */
static f32 sGiantModeScaleFactor = 1.0f;

/**
 * Stores the X and Z spawn positions for all of the ruins. Their Y spawn position is determined by the
 * height of the floor, so there's no need to store it.
 */
static Vec2s sSpawnPosList[] = {
    { 1200, 2500 },   // TWINMOLD_PROP_TYPE_RUIN_PILLAR_1
    { -1200, 2500 },  // TWINMOLD_PROP_TYPE_RUIN_PILLAR_2
    { 1200, -2500 },  // TWINMOLD_PROP_TYPE_RUIN_PILLAR_3
    { -1200, -2500 }, // TWINMOLD_PROP_TYPE_RUIN_PILLAR_4
    { 2500, 1200 },   // TWINMOLD_PROP_TYPE_RUIN_PILLAR_5
    { -2500, 1200 },  // TWINMOLD_PROP_TYPE_RUIN_PILLAR_6
    { 2500, -1200 },  // TWINMOLD_PROP_TYPE_RUIN_PILLAR_7
    { -2500, -1200 }, // TWINMOLD_PROP_TYPE_RUIN_PILLAR_8
    { 1000, 1000 },   // TWINMOLD_PROP_TYPE_RUIN_PILLAR_9
    { -1000, 1000 },  // TWINMOLD_PROP_TYPE_RUIN_PILLAR_10
    { 1000, -1000 },  // TWINMOLD_PROP_TYPE_RUIN_PILLAR_11
    { -1000, -1000 }, // TWINMOLD_PROP_TYPE_RUIN_PILLAR_12
    { 0, -1000 },     // TWINMOLD_PROP_TYPE_RUIN_PYRAMID_1
    { 0, 1000 },      // TWINMOLD_PROP_TYPE_RUIN_PYRAMID_2
    { 1000, 0 },      // TWINMOLD_PROP_TYPE_RUIN_PYRAMID_3
    { -1000, 0 },     // TWINMOLD_PROP_TYPE_RUIN_PYRAMID_4
    { 0, -2000 },     // TWINMOLD_PROP_TYPE_RUIN_PYRAMID_5
    { 0, 2000 },      // TWINMOLD_PROP_TYPE_RUIN_PYRAMID_6
    { 2000, 0 },      // TWINMOLD_PROP_TYPE_RUIN_PYRAMID_7
    { -2000, 0 },     // TWINMOLD_PROP_TYPE_RUIN_PYRAMID_8
};

/**
 * Display lists for all ruins.
 */
static Gfx* sDLists[] = {
    gTwinmoldRuinPillarDL,  // TWINMOLD_PROP_TYPE_RUIN_PILLAR_1
    gTwinmoldRuinPillarDL,  // TWINMOLD_PROP_TYPE_RUIN_PILLAR_2
    gTwinmoldRuinPillarDL,  // TWINMOLD_PROP_TYPE_RUIN_PILLAR_3
    gTwinmoldRuinPillarDL,  // TWINMOLD_PROP_TYPE_RUIN_PILLAR_4
    gTwinmoldRuinPillarDL,  // TWINMOLD_PROP_TYPE_RUIN_PILLAR_5
    gTwinmoldRuinPillarDL,  // TWINMOLD_PROP_TYPE_RUIN_PILLAR_6
    gTwinmoldRuinPillarDL,  // TWINMOLD_PROP_TYPE_RUIN_PILLAR_7
    gTwinmoldRuinPillarDL,  // TWINMOLD_PROP_TYPE_RUIN_PILLAR_8
    gTwinmoldRuinPillarDL,  // TWINMOLD_PROP_TYPE_RUIN_PILLAR_9
    gTwinmoldRuinPillarDL,  // TWINMOLD_PROP_TYPE_RUIN_PILLAR_10
    gTwinmoldRuinPillarDL,  // TWINMOLD_PROP_TYPE_RUIN_PILLAR_11
    gTwinmoldRuinPillarDL,  // TWINMOLD_PROP_TYPE_RUIN_PILLAR_12
    gTwinmoldRuinPyramidDL, // TWINMOLD_PROP_TYPE_RUIN_PYRAMID_1
    gTwinmoldRuinPyramidDL, // TWINMOLD_PROP_TYPE_RUIN_PYRAMID_2
    gTwinmoldRuinPyramidDL, // TWINMOLD_PROP_TYPE_RUIN_PYRAMID_3
    gTwinmoldRuinPyramidDL, // TWINMOLD_PROP_TYPE_RUIN_PYRAMID_4
    gTwinmoldRuinPyramidDL, // TWINMOLD_PROP_TYPE_RUIN_PYRAMID_5
    gTwinmoldRuinPyramidDL, // TWINMOLD_PROP_TYPE_RUIN_PYRAMID_6
    gTwinmoldRuinPyramidDL, // TWINMOLD_PROP_TYPE_RUIN_PYRAMID_7
    gTwinmoldRuinPyramidDL, // TWINMOLD_PROP_TYPE_RUIN_PYRAMID_8
};

/**
 * The initial base scale for all ruins. In the final game, this array isn't very useful,
 * since they're all the same value, but this could be used to make some ruins larger or
 * smaller than the others.
 */
static f32 sBaseScales[] = {
    0.09f, // TWINMOLD_PROP_TYPE_RUIN_PILLAR_1
    0.09f, // TWINMOLD_PROP_TYPE_RUIN_PILLAR_2
    0.09f, // TWINMOLD_PROP_TYPE_RUIN_PILLAR_3
    0.09f, // TWINMOLD_PROP_TYPE_RUIN_PILLAR_4
    0.09f, // TWINMOLD_PROP_TYPE_RUIN_PILLAR_5
    0.09f, // TWINMOLD_PROP_TYPE_RUIN_PILLAR_6
    0.09f, // TWINMOLD_PROP_TYPE_RUIN_PILLAR_7
    0.09f, // TWINMOLD_PROP_TYPE_RUIN_PILLAR_8
    0.09f, // TWINMOLD_PROP_TYPE_RUIN_PILLAR_9
    0.09f, // TWINMOLD_PROP_TYPE_RUIN_PILLAR_10
    0.09f, // TWINMOLD_PROP_TYPE_RUIN_PILLAR_11
    0.09f, // TWINMOLD_PROP_TYPE_RUIN_PILLAR_12
    0.09f, // TWINMOLD_PROP_TYPE_RUIN_PYRAMID_1
    0.09f, // TWINMOLD_PROP_TYPE_RUIN_PYRAMID_2
    0.09f, // TWINMOLD_PROP_TYPE_RUIN_PYRAMID_3
    0.09f, // TWINMOLD_PROP_TYPE_RUIN_PYRAMID_4
    0.09f, // TWINMOLD_PROP_TYPE_RUIN_PYRAMID_5
    0.09f, // TWINMOLD_PROP_TYPE_RUIN_PYRAMID_6
    0.09f, // TWINMOLD_PROP_TYPE_RUIN_PYRAMID_7
    0.09f, // TWINMOLD_PROP_TYPE_RUIN_PYRAMID_8
};

/**
 * Spawns the sand effect that appears when a ruin fragment hits the ground.
 */
void EnTanron5_SpawnEffectSand(TwinmoldEffect* effect, Vec3f* pos, f32 scale) {
    s16 i;

    for (i = 0; i < TWINMOLD_EFFECT_COUNT; i++, effect++) {
        if (effect->type == TWINMOLD_EFFECT_NONE) {
            effect->type = TWINMOLD_EFFECT_SAND;
            effect->pos = *pos;
            effect->velocity.x = Rand_CenteredFloat(10.0f);
            effect->velocity.y = Rand_ZeroFloat(2.0f) + 3.0f;
            effect->velocity.z = Rand_CenteredFloat(10.0f);
            effect->accel.y = -0.15f;
            effect->accel.x = effect->accel.z = 0.0f;
            effect->alpha = Rand_ZeroFloat(100.0f) + 200.0f;
            effect->timer = 0;
            effect->scale = scale;
            effect->targetScale = 2.0f * scale;
            effect->epoch++;
            break;
        }
    }
}

/**
 * Spawns the black dust that appears whenever part of a ruin is destroyed.
 */
void EnTanron5_SpawnEffectBlackDust(TwinmoldEffect* effect, Vec3f* pos, f32 scale) {
    s16 i;

    for (i = 0; i < TWINMOLD_EFFECT_COUNT; i++, effect++) {
        if (effect->type == TWINMOLD_EFFECT_NONE) {
            effect->type = TWINMOLD_EFFECT_BLACK_DUST;
            effect->pos = *pos;
            effect->velocity.x = Rand_CenteredFloat(30.0f);
            effect->velocity.y = Rand_ZeroFloat(7.0f);
            effect->velocity.z = Rand_CenteredFloat(30.0f);
            effect->accel.y = -0.3f;
            effect->accel.x = effect->accel.z = 0.0f;
            effect->alpha = Rand_ZeroFloat(70.0f) + 150.0f;
            effect->timer = 0;
            effect->scale = scale;
            effect->targetScale = 2.0f * scale;
            effect->epoch++;
            break;
        }
    }
}

void EnTanron5_Init(Actor* thisx, PlayState* play) {
    EnTanron5* this = (EnTanron5*)thisx;

    if (TWINMOLD_PROP_GET_TYPE(&this->actor) >= TWINMOLD_PROP_TYPE_FRAGMENT_LARGE_1) {
        // This is a ruin fragment or item drop; if there are more than 60 fragments or drops
        // already spawned, immediately kill this one. Otherwise, set up the fragment or drop
        // to fly off while spinning randomly.
        sFragmentAndItemDropCount++;
        if (sFragmentAndItemDropCount > 60) {
            Actor_Kill(&this->actor);
            return;
        }

        // fragmentAngularVelocityX is in a union with itemDropRotZ, so for item drops, this code
        // will initialize its z-rotation to a random value.
        this->fragmentAngularVelocityX = Rand_CenteredFloat(0x2000);
        this->fragmentAngularVelocityY = Rand_CenteredFloat(0x2000);

        if (TWINMOLD_PROP_GET_TYPE(&this->actor) <= TWINMOLD_PROP_TYPE_FRAGMENT_LARGE_7) {
            Actor_SetScale(&this->actor, (Rand_ZeroFloat(0.025f) + 0.085f) * sGiantModeScaleFactor);
        } else {
            Actor_SetScale(&this->actor, (Rand_ZeroFloat(0.015f) + 0.01f) * sGiantModeScaleFactor);
        }

        this->actor.speed = (Rand_ZeroFloat(10.0f) + 10.0f) * sGiantModeScaleFactor;
        this->actor.velocity.y = (Rand_ZeroFloat(10.0f) + 15.0f) * sGiantModeScaleFactor;
        this->actor.gravity = -2.5f * sGiantModeScaleFactor;
        this->actor.terminalVelocity = -1000.0f * sGiantModeScaleFactor;
        this->actor.update = EnTanron5_RuinFragmentItemDrop_Update;

        if (TWINMOLD_PROP_GET_TYPE(&this->actor) >= TWINMOLD_PROP_TYPE_ITEM_DROP_1) {
            this->actor.draw = EnTanron5_ItemDrop_Draw;
            this->itemDropType = Rand_ZeroFloat(1.999f);
            Actor_SetScale(&this->actor, sGiantModeScaleFactor * 0.03f);
            this->timer = 250;
            this->actor.shape.rot.x = this->actor.shape.rot.y = this->actor.shape.rot.z = 0;
        } else {
            this->dList = gRuinFragmentDL;
            this->timer = 150;
        }
    } else if (TWINMOLD_PROP_GET_TYPE(&this->actor) == TWINMOLD_PROP_TYPE_SPAWN_HANDLER) {
        EnTanron5* child;
        s32 i;

        // Spawns all of the ruins in the right places. Gets killed after everything is spawned.
        for (i = 0; i < ARRAY_COUNT(sSpawnPosList); i++) {
            child = (EnTanron5*)Actor_Spawn(&play->actorCtx, play, ACTOR_EN_TANRON5, sSpawnPosList[i].x,
                                            this->actor.world.pos.y, sSpawnPosList[i].z, 0, Rand_ZeroFloat(0x10000), 0,
                                            TWINMOLD_PROP_PARAMS(TWINMOLD_PROP_TYPE_RUIN_PILLAR_1 + i));

            child->actor.parent = this->actor.parent;
            child->baseScale = sBaseScales[i];

            Actor_SetScale(&child->actor, child->baseScale);

            child->dList = sDLists[i];
            if (child->dList == gTwinmoldRuinPyramidDL) {
                child->actor.shape.rot.y = 0;
            }

            Collider_InitAndSetCylinder(play, &child->collider, &child->actor, &sCylinderInit);
        }

        Actor_Kill(&this->actor);
    } else {
        // This is a ruin; update its y-position to be just below the floor, so it looks like it's
        // buried in the sand.
        Actor_UpdateBgCheckInfo(play, &this->actor, 50.0f, 150.0f, 100.0f, UPDBGCHECKINFO_FLAG_4);
        this->actor.world.pos.y = this->actor.floorHeight + -20.0f;
    }
}

void EnTanron5_Destroy(Actor* thisx, PlayState* play) {
    EnTanron5* this = (EnTanron5*)thisx;

    if (TWINMOLD_PROP_GET_TYPE(&this->actor) >= TWINMOLD_PROP_TYPE_FRAGMENT_LARGE_1) {
        sFragmentAndItemDropCount--;
    }
}

/**
 * This is the update function for the destructible ruins (both the pillar and pyarmid ruins).
 */
void EnTanron5_Update(Actor* thisx, PlayState* play2) {
    PlayState* play = play2;
    EnTanron5* this = (EnTanron5*)thisx;
    Boss02* boss02 = (Boss02*)this->actor.parent;
    Player* player = GET_PLAYER(play2);
    s32 i;
    s32 yawDiff;
    s32 fragmentAndItemCount;
    Vec3f pos;

    // When a ruin is destroyed (i.e., it is hit three times), it will reduce its scale to 0.0f (making
    // it effectively invisible and intangible), then wait an additional 37 frames before actually
    // calling Actor_Kill to despawn. The reason for this extra waiting period is unknown.
    if (this->hitCount >= 3) {
        this->hitCount++;
        Actor_SetScale(&this->actor, 0.0f);
        if (this->hitCount >= 40) {
            Actor_Kill(&this->actor);
        }
        return;
    } else {
        // required
    }

    DECR(this->timer);

    //! @bug This code will keep sGiantModeScaleFactor up-to-date so long as at least one ruin is still active.
    //! However, once the last ruin is destroyed, this code will no longer run, so sGiantModeScaleFactor will
    //! get "stuck" at whatever its current value is. This is a problem, because other instances of EnTanron5,
    //! like the item drops, rely on this variable being updated to function properly. Getting in this "stuck"
    //! state can result in odd behavior for these other instances, like item drops not being obtainable when
    //! the player is normal-sized.
    //!
    //! The strange waiting period before despawning seen above may be an attempt to mitigate this, but it
    //! doesn't work. It doesn't update sGiantModeScaleFactor, and even if it did, waiting 37 frames before
    //! despawning is far too short of a time to wait, since item drops take 250 frames to despawn.
    if (boss02->actor.update != NULL) {
        sGiantModeScaleFactor = boss02->giantModeScaleFactor;
    } else {
        sGiantModeScaleFactor = 1.0f;
    }

    Actor_SetScale(&this->actor, this->baseScale * sGiantModeScaleFactor);

    if (this->dList == gTwinmoldRuinPillarDL) {
        this->collider.dim.radius = 65.0f * sGiantModeScaleFactor;
        this->collider.dim.height = 380.0f * sGiantModeScaleFactor;
    } else if (this->hitCount == 0) {
        this->collider.dim.radius = 85.0f * sGiantModeScaleFactor;
        this->collider.dim.height = 200.0f * sGiantModeScaleFactor;
    } else if (this->hitCount == 1) {
        this->collider.dim.radius = 95.0f * sGiantModeScaleFactor;
        this->collider.dim.height = 100.0f * sGiantModeScaleFactor;
    } else if (this->hitCount == 2) {
        this->collider.dim.radius = 95.0f * sGiantModeScaleFactor;
        this->collider.dim.height = 30.0f * sGiantModeScaleFactor;
    }

    if (this->timer == 0) {
        if (this->collider.base.acFlags & AC_HIT) {
            ColliderElement* acHitElem = this->collider.elem.acHitElem;
            Actor* ac = this->collider.base.ac;

            this->collider.base.acFlags &= ~AC_HIT;
            fragmentAndItemCount = 10;

            if (Play_InCsMode(play)) {
                // In Twinmold's opening cutscene, it emerges from the sand beneath a ruin and destroys it.
                // Setting the timer to 1 here allows Twinmold to hit the ruin every single frame during the
                // cutscene, allowing it to destory the ruin in only 3 frames.
                this->timer = 1;
            } else {
                this->timer = 5;
                fragmentAndItemCount = (s32)Rand_ZeroFloat(2.99f) + 10;
            }

            if ((KREG(19) != 0) || ((acHitElem->atDmgInfo.dmgFlags & 0x05000202) && (sGiantModeScaleFactor < 0.5f)) ||
                (ac->id == ACTOR_BOSS_02)) {
                if (this->dList == gTwinmoldRuinPillarDL) {
                    // To create the appearance of the pillar shrinking after being hit, push it further into the floor,
                    // spawn some ruin fragments, and also spawn some black dust effects.
                    Math_Vec3f_Copy(&pos, &this->actor.world.pos);
                    pos.y += sGiantModeScaleFactor * 300.0f;

                    // This will spawn four normal-sized ruin fragments, three small ruin fragments, and
                    // zero, one, or two item drops, depending on the result of Rand_ZeroFloat above.
                    for (i = 3; i < fragmentAndItemCount; i++) {
                        Actor_Spawn(&play->actorCtx, play, ACTOR_EN_TANRON5, pos.x, pos.y, pos.z,
                                    Rand_ZeroFloat(0x10000), Rand_ZeroFloat(0x10000), 0,
                                    TWINMOLD_PROP_PARAMS(TWINMOLD_PROP_TYPE_FRAGMENT_LARGE_1 + i));
                    }

                    for (i = 0; i < 6; i++) {
                        EnTanron5_SpawnEffectBlackDust(play->specialEffects, &pos, Rand_ZeroFloat(3.0f) + 6.0f);
                    }

                    this->actor.world.pos.y -= sGiantModeScaleFactor * 130.0f;
                } else {
                    f32 yFactor;
                    f32 xzFactor;
                    Vec3f fragmentAndDustPos;

                    // Check the number of times this pyramid ruin has been hit to scale it accordingly.
                    if (this->hitCount == 0) {
                        yFactor = 180.0f;
                        this->baseScale *= 1.4f;
                    } else if (this->hitCount == 1) {
                        yFactor = 230.0f;
                        this->baseScale *= 1.37f;
                    } else if (this->hitCount == 2) {
                        yFactor = 780.0f;
                        this->baseScale *= 1.5f;
                    }

                    this->actor.world.pos.y -= sGiantModeScaleFactor * yFactor;
                    Actor_SetScale(&this->actor, this->baseScale * sGiantModeScaleFactor);
                    Math_Vec3f_Copy(&pos, &this->actor.world.pos);

                    for (i = 0; i < fragmentAndItemCount; i++) {
                        if (this->hitCount == 0) {
                            xzFactor = 100.0f;
                            yFactor = 180.0f;
                        } else if (this->hitCount == 1) {
                            xzFactor = 200.0f;
                            yFactor = 100.0f;
                        } else if (this->hitCount == 2) {
                            xzFactor = 250.0f;
                            yFactor = 50.0f;
                        }

                        fragmentAndDustPos.x = pos.x + (Rand_CenteredFloat(xzFactor) * sGiantModeScaleFactor);
                        fragmentAndDustPos.z = pos.z + (Rand_CenteredFloat(xzFactor) * sGiantModeScaleFactor);
                        fragmentAndDustPos.y = this->actor.floorHeight + (yFactor * sGiantModeScaleFactor);

                        // This will spawn seven normal-sized ruin fragments, three small ruin fragments, and
                        // zero, one, or two item drops, depending on the result of Rand_ZeroFloat above.
                        Actor_Spawn(&play->actorCtx, play, ACTOR_EN_TANRON5, fragmentAndDustPos.x, fragmentAndDustPos.y,
                                    fragmentAndDustPos.z, Rand_ZeroFloat(0x10000), Rand_ZeroFloat(0x10000), 0,
                                    TWINMOLD_PROP_PARAMS(TWINMOLD_PROP_TYPE_FRAGMENT_LARGE_1 + i));

                        if (i < 8) {
                            EnTanron5_SpawnEffectBlackDust(play->specialEffects, &fragmentAndDustPos,
                                                           Rand_ZeroFloat(3.0f) + 6.0f);
                        }
                    }
                }

                // To better sell the illusion of the ruin being partially destroyed when it's hit
                // rather than just being pushed into the ground (which is what actually happens),
                // this code will rotate the ruin in a somewhat-random way.
                if (Rand_ZeroOne() < 0.333f) {
                    yawDiff = 0x4000;
                } else if (Rand_ZeroOne() < 0.5f) {
                    yawDiff = -0x8000;
                } else {
                    yawDiff = -0x4000;
                }

                this->actor.shape.rot.y += yawDiff;
                Actor_PlaySfx(&this->actor, NA_SE_IT_BIG_BOMB_EXPLOSION);
                Actor_RequestQuakeAndRumble(&this->actor, play, 4, 4);
                this->hitCount++;
            } else {
                // Something hit the ruin, but it wasn't Twinmold, and it wasn't the player while in giant
                // mode. Play the reflect sound effect and spawn some sparks instead of breaking.
                Vec3f hitPos;
                ColliderElement* acHitElem = this->collider.elem.acHitElem;

                hitPos.x = acHitElem->acDmgInfo.hitPos.x;
                hitPos.y = acHitElem->acDmgInfo.hitPos.y;
                hitPos.z = acHitElem->acDmgInfo.hitPos.z;

                Actor_PlaySfx(&this->actor, NA_SE_IT_SHIELD_REFLECT_SW);
                CollisionCheck_SpawnShieldParticlesMetal(play, &hitPos);
            }
        }
    }

    Collider_UpdateCylinder(&this->actor, &this->collider);
    if (this->dList == gTwinmoldRuinPyramidDL) {
        this->collider.dim.pos.y = this->actor.floorHeight;
    }

    if ((this->dList == gTwinmoldRuinPillarDL) || (sGiantModeScaleFactor < 0.5f)) {
        CollisionCheck_SetOC(play, &play->colChkCtx, &this->collider.base);
    } else {
        // The collider cylinder used for the pyramid ruin is smaller than the ruin's visual appearance. When
        // the player is wearing the Giant's Mask, it works fine, but when the player is normal-sized, they can
        // walk through the sides of the ruin and wander around inside it without the collider pushing them out.
        // The below code prevents this from happening by manually updating the player's position if they get
        // close enough to the ruin, effectively creating a collision "box" around it that pushes players out.
        f32 xDiff = player->actor.world.pos.x - this->actor.world.pos.x;
        f32 zDiff = player->actor.world.pos.z - this->actor.world.pos.z;

        if ((fabsf(xDiff) < 120.0f) && (fabsf(zDiff) < 120.0f)) {
            if (fabsf(zDiff) < fabsf(xDiff)) {
                if (xDiff > 0.0f) {
                    player->actor.prevPos.x = player->actor.world.pos.x = this->actor.world.pos.x + 120.0f;
                } else {
                    player->actor.prevPos.x = player->actor.world.pos.x = this->actor.world.pos.x - 120.0f;
                }
            } else if (zDiff > 0.0f) {
                player->actor.prevPos.z = player->actor.world.pos.z = this->actor.world.pos.z + 120.0f;
            } else {
                player->actor.prevPos.z = player->actor.world.pos.z = this->actor.world.pos.z - 120.0f;
            }
        }
    }

    CollisionCheck_SetAC(play, &play->colChkCtx, &this->collider.base);
}

/**
 * This is the update function for the fragments and the item drops that fly off from a destructible ruin.
 */
void EnTanron5_RuinFragmentItemDrop_Update(Actor* thisx, PlayState* play2) {
    f32 interactionDistSq;
    s32 i;
    Vec3f pos;
    EnTanron5* this = (EnTanron5*)thisx;
    PlayState* play = play2;

    // When a ruin fragment hits the floor, it will slowly sink into the sand. After sinking for 38 frames,
    // the ruin fragment will despawn.
    if ((TWINMOLD_PROP_GET_TYPE(&this->actor) < TWINMOLD_PROP_TYPE_ITEM_DROP_1) && (this->sinkTimer != 0)) {
        this->sinkTimer++;
        this->actor.world.pos.y -= 2.0f * sGiantModeScaleFactor;
        if (this->sinkTimer == 40) {
            Actor_Kill(&this->actor);
        }

        return;
    }

    if (DECR(this->timer) == 0) {
        Actor_Kill(&this->actor);
    }

    if (this->actor.speed > 0.02f) {
        Actor_MoveWithGravity(&this->actor);
        Actor_UpdateBgCheckInfo(play, &this->actor, 50.0f, 150.0f, 100.0f, UPDBGCHECKINFO_FLAG_4);
    }

    if (TWINMOLD_PROP_GET_TYPE(&this->actor) < TWINMOLD_PROP_TYPE_ITEM_DROP_1) {
        this->actor.shape.rot.x += this->fragmentAngularVelocityX;
        this->actor.shape.rot.y += this->fragmentAngularVelocityY;
        interactionDistSq = SQ(35.0f);

        if (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND) {
            if (TWINMOLD_PROP_GET_TYPE(&this->actor) <= TWINMOLD_PROP_TYPE_FRAGMENT_SMALL_1) {
                Math_Vec3f_Copy(&pos, &this->actor.world.pos);
                pos.y = this->actor.floorHeight;

                for (i = 0; i < 4; i++) {
                    EnTanron5_SpawnEffectSand(play->specialEffects, &pos, Rand_ZeroFloat(1.0f) + 2.0f);
                }

                // Set the sinkTimer to a non-zero value so that this fragment will start sinking on the next update.
                this->sinkTimer++;
            } else {
                // Small ruin fragments don't sink into the sand; they just immediately despawn.
                Actor_Kill(&this->actor);
            }
        }
    } else {
        interactionDistSq = SQ(20.0f);
        this->itemDropRotZ += 0x2000;
        if (this->actor.bgCheckFlags & BGCHECKFLAG_GROUND) {
            this->itemDropRotZ = 0;
            this->actor.speed = 0.0f;
        }
    }

    if (this->hitTimer == 0) {
        if ((sGiantModeScaleFactor > 0.5f) &&
            ((TWINMOLD_PROP_GET_TYPE(&this->actor) <= TWINMOLD_PROP_TYPE_FRAGMENT_SMALL_1) ||
             (TWINMOLD_PROP_GET_TYPE(&this->actor) >= TWINMOLD_PROP_TYPE_ITEM_DROP_1))) {
            Player* player = GET_PLAYER(play);
            Vec3f pos;

            pos.x = player->actor.world.pos.x - this->actor.world.pos.x;
            pos.y = (player->actor.world.pos.y + 10.0f) - this->actor.world.pos.y;
            pos.z = player->actor.world.pos.z - this->actor.world.pos.z;

            if (SQXYZ(pos) < interactionDistSq) {
                if (TWINMOLD_PROP_GET_TYPE(&this->actor) >= TWINMOLD_PROP_TYPE_ITEM_DROP_1) {
                    if (this->itemDropType == TWINMOLD_PROP_ITEM_DROP_TYPE_10_ARROWS) {
                        Item_Give(play, ITEM_ARROWS_10);
                    } else {
                        Item_Give(play, ITEM_MAGIC_JAR_BIG);
                    }

                    Actor_Kill(&this->actor);
                    Audio_PlaySfx(NA_SE_SY_GET_ITEM);
                } else {
                    // Damages the player and knocks them back. Starts a 20-frame timer to prevent
                    // this same ruin fragment from damaging the player again too quickly.
                    this->hitTimer = 20;
                    func_800B8D50(play, NULL, 5.0f, this->actor.world.rot.y, 0.0f, 8);
                }
            }
        }
    } else {
        this->hitTimer--;
    }
}

void EnTanron5_Draw(Actor* thisx, PlayState* play) {
    EnTanron5* this = (EnTanron5*)thisx;

    if ((-500.0f * sGiantModeScaleFactor) < this->actor.projectedPos.z) {
        OPEN_DISPS(play->state.gfxCtx);

        Gfx_SetupDL25_Opa(play->state.gfxCtx);

        MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx);
        gSPDisplayList(POLY_OPA_DISP++, this->dList);

        CLOSE_DISPS(play->state.gfxCtx);
    }
}

void EnTanron5_ItemDrop_Draw(Actor* thisx, PlayState* play) {
    EnTanron5* this = (EnTanron5*)thisx;
    TexturePtr texture;
    s32 shouldDraw;

    // This makes the item drop flicker when it's close to despawning.
    if ((this->timer > 50) || (this->timer & 1)) {
        shouldDraw = true;
    } else {
        shouldDraw = false;
    }

    if (((-500.0f * sGiantModeScaleFactor) < this->actor.projectedPos.z) && shouldDraw) {
        OPEN_DISPS(play->state.gfxCtx);

        Gfx_SetupDL25_Opa(play->state.gfxCtx);
        if (this->itemDropType == TWINMOLD_PROP_ITEM_DROP_TYPE_10_ARROWS) {
            texture = gDropArrows1Tex;
        } else {
            texture = gDropMagicLargeTex;
        }

        POLY_OPA_DISP = Gfx_SetupDL66(POLY_OPA_DISP);

        gSPSegment(POLY_OPA_DISP++, 0x08, Lib_SegmentedToVirtual(texture));

        Matrix_Translate(0.0f, 200.0f, 0.0f, MTXMODE_APPLY);
        Matrix_RotateZS(this->itemDropRotZ, MTXMODE_APPLY);

        MATRIX_FINALIZE_AND_LOAD(POLY_OPA_DISP++, play->state.gfxCtx);
        gSPDisplayList(POLY_OPA_DISP++, gItemDropDL);

        CLOSE_DISPS(play->state.gfxCtx);
    }
}
