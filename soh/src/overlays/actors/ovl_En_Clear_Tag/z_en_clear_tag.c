#include "z_en_clear_tag.h"

#include <string.h>

#include "soh/frame_interpolation.h"
#include "soh/Enhancements/game-interactor/GameInteractor_Hooks.h"

#define FLAGS (ACTOR_FLAG_TARGETABLE | ACTOR_FLAG_HOSTILE | ACTOR_FLAG_UPDATE_WHILE_CULLED | ACTOR_FLAG_DRAW_WHILE_CULLED)

void EnClearTag_Init(Actor* thisx, PlayState* play);
void EnClearTag_Destroy(Actor* thisx, PlayState* play);
void EnClearTag_Update(Actor* thisx, PlayState* play);
void EnClearTag_Draw(Actor* thisx, PlayState* play);
void EnClearTag_Reset(void);

void EnClearTag_UpdateEffects(PlayState* play);
void EnClearTag_DrawEffects(PlayState* play);

void EnClearTag_CreateDebrisEffect(PlayState* play, Vec3f* position, Vec3f* velocity, Vec3f* acceleration,
                                   f32 scale, f32 floorHeight);
void EnClearTag_CreateFireEffect(PlayState* play, Vec3f* pos, f32 scale);
void EnClearTag_CreateSmokeEffect(PlayState* play, Vec3f* position, f32 scale);
void EnClearTag_CreateFlashEffect(PlayState* play, Vec3f* position, f32 scale, f32 floorHeight,
                                  Vec3f* floorTangent);

void EnClearTag_CalculateFloorTangent(EnClearTag* this);

const ActorInit En_Clear_Tag_InitVars = {
    ACTOR_EN_CLEAR_TAG,
    ACTORCAT_BOSS,
    FLAGS,
    OBJECT_GAMEPLAY_KEEP,
    sizeof(EnClearTag),
    (ActorFunc)EnClearTag_Init,
    (ActorFunc)EnClearTag_Destroy,
    (ActorFunc)EnClearTag_Update,
    (ActorFunc)EnClearTag_Draw,
    (ActorResetFunc)EnClearTag_Reset,
};

u8 sClearTagIsEffectsInitialized = false;

static Vec3f sZeroVector = { 0.0f, 0.0f, 0.0f };

static ColliderCylinderInit sArwingCylinderInit = {
    {
        COLTYPE_HIT3,
        AT_ON | AT_TYPE_ENEMY,
        AC_ON | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_1,
        COLSHAPE_CYLINDER,
    },
    {
        ELEMTYPE_UNK0,
        { 0xFFCFFFFF, 0x00, 0x04 },
        { 0xFFDFFFFF, 0x00, 0x00 },
        TOUCH_ON | TOUCH_SFX_NORMAL,
        BUMP_ON,
        OCELEM_ON,
    },
    { 15, 30, 10, { 0, 0, 0 } },
};

static ColliderCylinderInit sLaserCylinderInit = {
    {
        COLTYPE_METAL,
        AT_ON | AT_TYPE_ENEMY,
        AC_ON | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_1,
        COLSHAPE_CYLINDER,
    },
    {
        ELEMTYPE_UNK0,
        { 0xFFCFFFFF, 0x00, 0x04 },
        { 0xFFDFFFFF, 0x00, 0x00 },
        TOUCH_ON | TOUCH_SFX_NORMAL,
        BUMP_ON,
        OCELEM_ON,
    },
    { 15, 30, 10, { 0, 0, 0 } },
};

//static UNK_TYPE4 D_809D5C98 = 0; // unused
//static UNK_TYPE4 D_809D5C9C = 0; // unused

EnClearTagEffect sClearTagEffects[CLEAR_TAG_EFFECT_MAX_COUNT];

#include "overlays/ovl_En_Clear_Tag/ovl_En_Clear_Tag.h"

/**
 * Creates a debris effect.
 * Debris effects are spawned when the Arwing dies. It spawns fire effects.
 */
void EnClearTag_CreateDebrisEffect(PlayState* play, Vec3f* position, Vec3f* velocity, Vec3f* acceleration,
                                   f32 scale, f32 floorHeight) {
    s16 i;
    EnClearTagEffect* effect = (EnClearTagEffect*)play->specialEffects;

    // Look for an available effect to allocate a Debris effect to.
    for (i = 0; i < CLEAR_TAG_EFFECT_MAX_COUNT; i++, effect++) {
        if (effect->type == CLEAR_TAG_EFFECT_AVAILABLE) {
            effect->type = CLEAR_TAG_EFFECT_DEBRIS;

            effect->position = *position;
            effect->velocity = *velocity;
            effect->acceleration = *acceleration;

            effect->scale = scale;

            // Set the debris effects to spawn in a circle.
            effect->rotationY = Rand_ZeroFloat(M_PI * 2);
            effect->rotationX = Rand_ZeroFloat(M_PI * 2);

            effect->timer = effect->bounces = 0;

            effect->floorHeight = floorHeight;

            effect->random = (s16)Rand_ZeroFloat(10.0f);

            return;
        }
    }
}

/**
 * Creates a fire effect.
 * Fire effects are spawned by debris effects. Fire effects spawn smoke effects
 */
void EnClearTag_CreateFireEffect(PlayState* play, Vec3f* pos, f32 scale) {
    s16 i;
    EnClearTagEffect* effect = (EnClearTagEffect*)play->specialEffects;

    // Look for an available effect to allocate a fire effect to.
    for (i = 0; i < CLEAR_TAG_EFFECT_MAX_COUNT; i++, effect++) {
        if (effect->type == CLEAR_TAG_EFFECT_AVAILABLE) {
            effect->random = (s16)Rand_ZeroFloat(100.0f);
            effect->type = CLEAR_TAG_EFFECT_FIRE;

            effect->position = *pos;
            effect->velocity = sZeroVector;
            effect->acceleration = sZeroVector;
            effect->acceleration.y = 0.15f;

            effect->scale = scale;

            effect->primColor.a = 200.0f;

            return;
        }
    }
}

/**
 * Creates a smoke effect.
 * Smoke effects are spawned by fire effects.
 */
void EnClearTag_CreateSmokeEffect(PlayState* play, Vec3f* position, f32 scale) {
    s16 i;
    EnClearTagEffect* effect = (EnClearTagEffect*)play->specialEffects;

    // Look for an available effect to allocate a smoke effect to.
    for (i = 0; i < CLEAR_TAG_EFFECT_MAX_COUNT; i++, effect++) {
        if (effect->type == CLEAR_TAG_EFFECT_AVAILABLE) {
            effect->random = (s16)Rand_ZeroFloat(100.0f);
            effect->type = CLEAR_TAG_EFFECT_SMOKE;

            effect->position = *position;
            effect->velocity = sZeroVector;
            effect->acceleration = sZeroVector;

            effect->scale = scale;
            effect->maxScale = scale * 2.0f;

            effect->primColor.r = 200.0f;
            effect->primColor.g = 20.0f;
            effect->primColor.b = 0.0f;
            effect->primColor.a = 255.0f;
            effect->envColor.r = 255.0f;
            effect->envColor.g = 215.0f;
            effect->envColor.b = 255.0f;

            return;
        }
    }
}

/**
 * Creates a flash effect.
 * Flash effects are spawned when the Arwing dies.
 * Flash effects two components: 1) a billboard flash, and 2) a light effect on the ground.
 */
void EnClearTag_CreateFlashEffect(PlayState* play, Vec3f* position, f32 scale, f32 floorHeight,
                                  Vec3f* floorTangent) {
    s16 i;
    EnClearTagEffect* effect = (EnClearTagEffect*)play->specialEffects;

    // Look for an available effect to allocate a flash effect to.
    for (i = 0; i < CLEAR_TAG_EFFECT_MAX_COUNT; i++, effect++) {
        if (effect->type == CLEAR_TAG_EFFECT_AVAILABLE) {
            effect->type = CLEAR_TAG_EFFECT_FLASH;

            effect->position = *position;
            effect->velocity = sZeroVector;
            effect->acceleration = sZeroVector;

            effect->scale = 0.0f;
            effect->maxScale = scale * 2.0f;

            effect->floorHeight = floorHeight;
            effect->floorTangent = *floorTangent;

            effect->primColor.a = 180.0f;

            return;
        }
    }
}

/**
 * EnClear_Tag destructor.
 * This just destroys the collider.
 */
void EnClearTag_Destroy(Actor* thisx, PlayState* play) {
    EnClearTag* this = (EnClearTag*)thisx;

    Collider_DestroyCylinder(play, &this->collider);
}

/**
 * EnClear_Tag constructor.
 * This allocates a collider, initializes effects, and sets up ClearTag instance data.
 */
void EnClearTag_Init(Actor* thisx, PlayState* play) {
    EnClearTag* this = (EnClearTag*)thisx;
    s32 defaultCutsceneTimer = 100;
    s16 i;
    s16 j;

    Collider_InitCylinder(play, &this->collider);

    // Initialize the Arwing laser.
    if (this->actor.params == CLEAR_TAG_LASER) {
        this->state = CLEAR_TAG_STATE_LASER;
        this->timers[CLEAR_TAG_TIMER_LASER_DEATH] = 70;
        this->actor.speedXZ = 35.0f;
        func_8002D908(&this->actor);
        for (j = 0; j <= 0; j++) {
            func_8002D7EC(&this->actor);
        }
        this->actor.scale.x = 0.4f;
        this->actor.scale.y = 0.4f;
        this->actor.scale.z = 2.0f;
        this->actor.speedXZ = 70.0f;
        this->actor.shape.rot.x = -this->actor.shape.rot.x;

        func_8002D908(&this->actor);
        Collider_SetCylinder(play, &this->collider, &this->actor, &sLaserCylinderInit);
        Audio_PlayActorSound2(&this->actor, NA_SE_IT_SWORD_REFLECT_MG);
    } else { // Initialize the Arwing.

        // Change Arwing to regular enemy instead of boss with enemy randomizer and crowd control.
        // This way Arwings will be considered for "clear enemy" rooms properly.
        if (CVarGetInteger("gRandomizedEnemies", 0) || CVarGetInteger("gCrowdControl", 0)) {
            Actor_ChangeCategory(play, &play->actorCtx, thisx, ACTORCAT_ENEMY);
        }

        this->actor.flags |= ACTOR_FLAG_TARGETABLE;
        this->actor.targetMode = 5;
        Collider_SetCylinder(play, &this->collider, &this->actor, &sArwingCylinderInit);
        this->actor.colChkInfo.health = 3;

        // Update the Arwing to play the intro cutscene.
        if (this->actor.params == CLEAR_TAG_CUTSCENE_ARWING) {
            this->timers[CLEAR_TAG_TIMER_ARWING_UPDATE_STATE] = 70;
            this->timers[CLEAR_TAG_TIMER_ARWING_ENTER_LOCKED_ON] = 250;
            this->state = CLEAR_TAG_STATE_DEMO;
            this->actor.world.rot.x = 0x4000;
            this->cutsceneMode = CLEAR_TAG_CUTSCENE_MODE_SETUP;
            this->cutsceneTimer = defaultCutsceneTimer;
            this->timers[CLEAR_TAG_TIMER_ARWING_UPDATE_BG_INFO] = 20;
        }

        // Initialize all effects to available if effects have not been initialized.
        if (!sClearTagIsEffectsInitialized) {
            sClearTagIsEffectsInitialized = true;
            play->specialEffects = &sClearTagEffects[0];
            for (i = 0; i < CLEAR_TAG_EFFECT_MAX_COUNT; i++) {
                sClearTagEffects[i].type = CLEAR_TAG_EFFECT_AVAILABLE;
                sClearTagEffects[i].epoch++;
            }
            this->drawMode = CLEAR_TAG_DRAW_MODE_ALL;
        }
    }
}

/**
 * Calculate a floor tangent.
 * This is used for the ground flash display lists and Arwing shadow display lists to snap onto the floor.
 */
void EnClearTag_CalculateFloorTangent(EnClearTag* this) {
    // If there is a floor poly below the Arwing, calculate the floor tangent.
    if (this->actor.floorPoly != NULL) {
        f32 x = COLPOLY_GET_NORMAL(this->actor.floorPoly->normal.x);
        f32 y = COLPOLY_GET_NORMAL(this->actor.floorPoly->normal.y);
        f32 z = COLPOLY_GET_NORMAL(this->actor.floorPoly->normal.z);

        this->floorTangent.x = -Math_FAtan2F(-z * y, 1.0f);
        this->floorTangent.z = Math_FAtan2F(-x * y, 1.0f);
    }
}

/**
 * EnClear_Tag update function.
 *
 * ClearTag has three variants:
 * CLEAR_TAG_CUTSCENE_ARWING which is the regular Arwing and plays a cutscene on spawn.
 * CLEAR_TAG_ARWING which is a regular Arwing.
 * CLEAR_TAG_LASER which are the lasers fired by the Arwing.
 *
 * This function controls the Arwing flying. A target position is set and the Arwing flies toward it based on
 * calculated directions from the current position.
 * This function fires Arwing lasers when the state is CLEAR_TAG_STATE_TARGET_LOCKED.
 * This function controls the cutscene that plays when the Arwing has params for
 * cutscene. The cutscene stops playing when the Arwing is a specified distance from the starting point.
 */
void EnClearTag_Update(Actor* thisx, PlayState* play2) {
    u8 hasAtHit = false;
    s16 i;
    s16 xRotationTarget;
    s16 rotationScale;
    PlayState* play = play2;
    EnClearTag* this = (EnClearTag*)thisx;
    Player* player = GET_PLAYER(play);

    this->frameCounter++;

    if (this->drawMode != CLEAR_TAG_DRAW_MODE_EFFECT) {
        for (i = 0; i < 3; i++) {
            if (this->timers[i] != 0) {
                this->timers[i]--;
            }
        }

        if (this->cutsceneTimer != 0) {
            this->cutsceneTimer--;
        }

        switch (this->state) {
            case CLEAR_TAG_STATE_DEMO:
            case CLEAR_TAG_STATE_TARGET_LOCKED:
            case CLEAR_TAG_STATE_FLYING: {
                f32 vectorToTargetX;
                f32 vectorToTargetY;
                f32 vectorToTargetZ;
                s16 worldRotationTargetX;
                s16 worldRotationTargetY;
                f32 loseTargetLockDistance;
                s16 worldRotationTargetZ;
                s32 pad;

                // Check if the Arwing should crash.
                if (this->collider.base.acFlags & AC_HIT) {

                    this->collider.base.acFlags &= ~AC_HIT;
                    this->crashingTimer = 20;
                    Actor_SetColorFilter(&this->actor, 0x4000, 255, 0, 5);
                    this->acceleration.x = Rand_CenteredFloat(15.0f);
                    this->acceleration.y = Rand_CenteredFloat(15.0f);
                    this->acceleration.z = Rand_CenteredFloat(15.0f);

                    Audio_PlayActorSound2(&this->actor, NA_SE_EN_FANTOM_THUNDER_GND);
                    this->actor.colChkInfo.health--;
                    if ((s8)this->actor.colChkInfo.health <= 0) {
                        this->state = CLEAR_TAG_STATE_CRASHING;
                        this->actor.velocity.y = 0.0f;
                        GameInteractor_ExecuteOnEnemyDefeat(&this->actor);
                        goto state_crashing;
                    }
                }
                Actor_SetScale(&this->actor, 0.2f);
                this->actor.speedXZ = 7.0f;

                if (this->timers[CLEAR_TAG_TIMER_ARWING_UPDATE_STATE] == 0) {
                    if (this->timers[CLEAR_TAG_TIMER_ARWING_ENTER_LOCKED_ON] == 0) {
                        this->state = CLEAR_TAG_STATE_TARGET_LOCKED;
                        this->timers[CLEAR_TAG_TIMER_ARWING_UPDATE_STATE] = 300;
                    } else {
                        this->state = CLEAR_TAG_STATE_FLYING;
                        this->timers[CLEAR_TAG_TIMER_ARWING_UPDATE_STATE] = (s16)Rand_ZeroFloat(50.0f) + 20;

                        if (this->actor.params == CLEAR_TAG_ARWING) {
                            // Set the Arwing to fly in a circle around the player.
                            f32 targetCircleX = Math_SinS(player->actor.shape.rot.y) * 400.0f;
                            f32 targetCircleZ = Math_CosS(player->actor.shape.rot.y) * 400.0f;

                            this->targetPosition.x =
                                Rand_CenteredFloat(700.0f) + (player->actor.world.pos.x + targetCircleX);
                            this->targetPosition.y = Rand_ZeroFloat(200.0f) + player->actor.world.pos.y + 150.0f;
                            this->targetPosition.z =
                                Rand_CenteredFloat(700.0f) + (player->actor.world.pos.z + targetCircleZ);
                        } else {
                            // Set the Arwing to fly to a random position.
                            this->targetPosition.x = Rand_CenteredFloat(700.0f);
                            this->targetPosition.y = Rand_ZeroFloat(200.0f) + 150.0f;
                            this->targetPosition.z = Rand_CenteredFloat(700.0f);
                        }
                    }

                    this->targetDirection.x = this->targetDirection.y = this->targetDirection.z = 0.0f;
                }

                rotationScale = 10;
                xRotationTarget = 0x800;
                loseTargetLockDistance = 100.0f;
                if (this->state == CLEAR_TAG_STATE_TARGET_LOCKED) {
                    // Set the Arwing to fly towards the player.
                    this->targetPosition.x = player->actor.world.pos.x;
                    this->targetPosition.y = player->actor.world.pos.y + 40.0f;
                    this->targetPosition.z = player->actor.world.pos.z;
                    rotationScale = 7;
                    xRotationTarget = 0x1000;
                    loseTargetLockDistance = 150.0f;
                } else if (this->state == CLEAR_TAG_STATE_DEMO) {
                    // Move the Arwing for the intro cutscene.

                    // Do a Barrel Roll!
                    this->roll += 0.5f;
                    if (this->roll > M_PI * 2) {
                        this->roll -= M_PI * 2;
                    }

                    // Set the Arwing to fly to a hardcoded position.
                    this->targetPosition.x = 0.0f;
                    this->targetPosition.y = 300.0f;
                    this->targetPosition.z = 0.0f;
                    loseTargetLockDistance = 100.0f;
                }

                // If the Arwing is not in cutscene state, smoothly update the roll to zero.
                // This will reset the Arwing to be right side up after the cutscene is done.
                // The cutscene will set the Arwing to do a barrel roll and doesn't end on right side up.
                if (this->state != CLEAR_TAG_STATE_DEMO) {
                    Math_ApproachZeroF(&this->roll, 0.1f, 0.2f);
                }

                // Calculate a vector towards the targetted position.
                vectorToTargetX = this->targetPosition.x - this->actor.world.pos.x;
                vectorToTargetY = this->targetPosition.y - this->actor.world.pos.y;
                vectorToTargetZ = this->targetPosition.z - this->actor.world.pos.z;

                // If the Arwing is within a certain distance to the target position, it will be updated to flymode
                if (sqrtf(SQ(vectorToTargetX) + SQ(vectorToTargetY) + SQ(vectorToTargetZ)) < loseTargetLockDistance) {
                    this->timers[CLEAR_TAG_TIMER_ARWING_UPDATE_STATE] = 0;
                    if (this->state == CLEAR_TAG_STATE_TARGET_LOCKED) {
                        this->timers[CLEAR_TAG_TIMER_ARWING_ENTER_LOCKED_ON] = (s16)Rand_ZeroFloat(100.0f) + 100;
                    }
                    this->state = CLEAR_TAG_STATE_FLYING;
                }

                // Calculate the direction for the Arwing to fly and the rotation for the Arwing
                // based on the Arwing's direction, and current rotation.
                worldRotationTargetY = Math_FAtan2F(vectorToTargetX, vectorToTargetZ) * (0x8000 / M_PI);
                worldRotationTargetX =
                    Math_FAtan2F(vectorToTargetY, sqrtf(SQ(vectorToTargetX) + SQ(vectorToTargetZ))) * (0x8000 / M_PI);
                if ((worldRotationTargetX < 0) && (this->actor.world.pos.y < this->actor.floorHeight + 20.0f)) {
                    worldRotationTargetX = 0;
                }
                Math_ApproachS(&this->actor.world.rot.x, worldRotationTargetX, rotationScale, this->targetDirection.x);
                worldRotationTargetZ = Math_SmoothStepToS(&this->actor.world.rot.y, worldRotationTargetY, rotationScale,
                                                          this->targetDirection.y, 0);
                Math_ApproachF(&this->targetDirection.x, xRotationTarget, 1.0f, 0x100);
                this->targetDirection.y = this->targetDirection.x;
                if (ABS(worldRotationTargetZ) < 0x1000) {
                    Math_ApproachS(&this->actor.world.rot.z, 0, 15, this->targetDirection.z);
                    Math_ApproachF(&this->targetDirection.z, 0x500, 1.0f, 0x100);

                    // Introduce a range requirement in Enemy Rando so Arwings don't shoot the player from
                    // across the map. Especially noticeable in big maps like Lake Hylia and Hyrule Field.
                    uint8_t enemyRandoShootLaser = !CVarGetInteger("gRandomizedEnemies", 0) || this->actor.xzDistToPlayer < 1000.0f;

                    // Check if the Arwing should fire its laser.
                    if ((this->frameCounter % 4) == 0 && (Rand_ZeroOne() < 0.75f) &&
                            (this->state == CLEAR_TAG_STATE_TARGET_LOCKED) && enemyRandoShootLaser) {
                        this->shouldShootLaser = true;
                    }
                } else {
                    worldRotationTargetZ = worldRotationTargetZ > 0 ? -0x2500 : 0x2500;
                    Math_ApproachS(&this->actor.world.rot.z, worldRotationTargetZ, rotationScale,
                                   this->targetDirection.z);
                    Math_ApproachF(&this->targetDirection.z, 0x1000, 1.0f, 0x200);
                }
                this->actor.shape.rot = this->actor.world.rot;
                this->actor.shape.rot.x = -this->actor.shape.rot.x;

                // Update the Arwing's velocity.
                func_8002D908(&this->actor);
                this->actor.velocity.x += this->acceleration.x;
                this->actor.velocity.y += this->acceleration.y;
                this->actor.velocity.z += this->acceleration.z;
                Math_ApproachZeroF(&this->acceleration.x, 1.0f, 1.0f);
                Math_ApproachZeroF(&this->acceleration.y, 1.0f, 1.0f);
                Math_ApproachZeroF(&this->acceleration.z, 1.0f, 1.0f);

                // Fire the Arwing laser.
                if (this->shouldShootLaser) {
                    this->shouldShootLaser = false;
                    Actor_Spawn(&play->actorCtx, play, ACTOR_EN_CLEAR_TAG, this->actor.world.pos.x,
                                this->actor.world.pos.y, this->actor.world.pos.z, this->actor.world.rot.x,
                                this->actor.world.rot.y, this->actor.world.rot.z, CLEAR_TAG_STATE_LASER, true);
                }
            }
            case CLEAR_TAG_STATE_CRASHING:
            state_crashing:
                if (this->crashingTimer != 0) {
                    this->crashingTimer--;
                }

                func_8002D7EC(&this->actor);

                Actor_SetFocus(&this->actor, 0.0f);

                // Update Arwing collider to better match a ground collision.
                this->collider.dim.radius = 20;
                this->collider.dim.height = 15;
                this->collider.dim.yShift = -5;
                Collider_UpdateCylinder(&this->actor, &this->collider);

                CollisionCheck_SetAC(play, &play->colChkCtx, &this->collider.base);
                CollisionCheck_SetAT(play, &play->colChkCtx, &this->collider.base);

                if (this->timers[CLEAR_TAG_TIMER_ARWING_UPDATE_BG_INFO] == 0) {
                    Actor_UpdateBgCheckInfo(play, &this->actor, 50.0f, 30.0f, 100.0f, 5);
                    EnClearTag_CalculateFloorTangent(this);
                }

                if (this->state == CLEAR_TAG_STATE_CRASHING) {
                    // Create fire effects while the Arwing crashes.
                    EnClearTag_CreateFireEffect(play, &this->actor.world.pos, 1.0f);

                    // Causes the Arwing to roll around seemingly randomly while crashing.
                    this->roll -= 0.5f;
                    this->actor.velocity.y -= 0.2f;
                    this->actor.shape.rot.x += 0x10;

                    Audio_PlayActorSound2(&this->actor, NA_SE_EN_DODO_K_BREATH - SFX_FLAG);

                    // Check if the Arwing has hit the ground.
                    if (this->actor.bgCheckFlags & 9) {
                        this->shouldExplode = true;

                        if (this->drawMode != CLEAR_TAG_DRAW_MODE_ARWING) {
                            this->drawMode = CLEAR_TAG_DRAW_MODE_EFFECT;
                            this->deathTimer = 70;
                            this->actor.flags &= ~ACTOR_FLAG_TARGETABLE;
                        } else {
                            Actor_Kill(&this->actor);
                        }
                    }
                }
                break;

            case CLEAR_TAG_STATE_LASER:
                func_8002D7EC(&this->actor);

                // Check if the laser has hit a target.
                if (this->collider.base.atFlags & AT_HIT) {
                    hasAtHit = true;
                }

                // Set laser collider properties.
                this->collider.dim.radius = 23;
                this->collider.dim.height = 25;
                this->collider.dim.yShift = -10;
                Collider_UpdateCylinder(&this->actor, &this->collider);
                CollisionCheck_SetAT(play, &play->colChkCtx, &this->collider.base);
                Actor_UpdateBgCheckInfo(play, &this->actor, 50.0f, 80.0f, 100.0f, 5);

                // Check if the laser has hit a target, timed out, or hit the ground.
                if (this->actor.bgCheckFlags & 9 || hasAtHit || this->timers[CLEAR_TAG_TIMER_LASER_DEATH] == 0) {
                    // Kill the laser.
                    Actor_Kill(&this->actor);
                    // Player laser sound effect if the laser did not time out.
                    if (this->timers[CLEAR_TAG_TIMER_LASER_DEATH] != 0) {
                        SoundSource_PlaySfxAtFixedWorldPos(play, &this->actor.world.pos, 20,
                                                           NA_SE_EN_FANTOM_THUNDER_GND);
                    }
                }
                break;
        }

        if (this->state < CLEAR_TAG_STATE_LASER) {
            // Play the Arwing cutscene.
            osSyncPrintf("DEMO_MODE %d\n", this->cutsceneMode);
            osSyncPrintf("CAMERA_NO %d\n", this->cameraId);

            if (this->cutsceneMode != CLEAR_TAG_CUTSCENE_MODE_NONE) {
                f32 cutsceneCameraCircleX;
                f32 cutsceneCameraCircleZ;
                s16 cutsceneTimer;
                Vec3f cutsceneCameraAtTarget;
                Vec3f cutsceneCameraEyeTarget;

                switch (this->cutsceneMode) {
                    case CLEAR_TAG_CUTSCENE_MODE_SETUP:
                        // Initializes Arwing cutscene camera data.
                        this->cutsceneMode = CLEAR_TAG_CUTSCENE_MODE_PLAY;
                        func_80064520(play, &play->csCtx);
                        this->cameraId = Play_CreateSubCamera(play);
                        Play_ChangeCameraStatus(play, MAIN_CAM, CAM_STAT_WAIT);
                        Play_ChangeCameraStatus(play, this->cameraId, CAM_STAT_ACTIVE);
                    case CLEAR_TAG_CUTSCENE_MODE_PLAY:
                        // Update the Arwing cutscene camera to spin around in a circle.
                        cutsceneTimer = this->frameCounter * 128;
                        cutsceneCameraCircleX = Math_SinS(cutsceneTimer) * 200.0f;
                        cutsceneCameraCircleZ = Math_CosS(cutsceneTimer) * 200.0f;
                        cutsceneCameraAtTarget.x = this->actor.world.pos.x + cutsceneCameraCircleX;
                        cutsceneCameraAtTarget.y = 200.0f;
                        cutsceneCameraAtTarget.z = this->actor.world.pos.z + cutsceneCameraCircleZ;
                        cutsceneCameraEyeTarget = this->actor.world.pos;
                        break;
                }

                // Make the Arwing cutscene camera approach the target.
                if (this->cameraId != SUBCAM_FREE) {
                    Math_ApproachF(&this->cutsceneCameraAt.x, cutsceneCameraAtTarget.x, 0.1f, 500.0f);
                    Math_ApproachF(&this->cutsceneCameraAt.y, cutsceneCameraAtTarget.y, 0.1f, 500.0f);
                    Math_ApproachF(&this->cutsceneCameraAt.z, cutsceneCameraAtTarget.z, 0.1f, 500.0f);
                    Math_ApproachF(&this->cutsceneCameraEye.x, cutsceneCameraEyeTarget.x, 0.2f, 500.0f);
                    Math_ApproachF(&this->cutsceneCameraEye.y, cutsceneCameraEyeTarget.y, 0.2f, 500.0f);
                    Math_ApproachF(&this->cutsceneCameraEye.z, cutsceneCameraEyeTarget.z, 0.2f, 500.0f);
                    Play_CameraSetAtEye(play, this->cameraId, &this->cutsceneCameraEye,
                                            &this->cutsceneCameraAt);
                }

                // Cutscene has finished.
                if (this->cutsceneTimer == 1) {
                    func_800C08AC(play, this->cameraId, 0);
                    this->cutsceneMode = this->cameraId = SUBCAM_FREE;
                    func_80064534(play, &play->csCtx);
                }
            }
        }
    }

    // Explode the Arwing
    if (this->shouldExplode) {
        Vec3f crashEffectLocation;
        Vec3f crashEffectVelocity;
        Vec3f debrisEffectAcceleration;

        this->shouldExplode = false;
        SoundSource_PlaySfxAtFixedWorldPos(play, &this->actor.world.pos, 40, NA_SE_IT_BOMB_EXPLOSION);

        // Spawn flash effect.
        crashEffectLocation.x = this->actor.world.pos.x;
        crashEffectLocation.y = (this->actor.world.pos.y + 40.0f) - 30.0f;
        crashEffectLocation.z = this->actor.world.pos.z;
        EnClearTag_CreateFlashEffect(play, &crashEffectLocation, 6.0f, this->actor.floorHeight,
                                     &this->floorTangent);

        // Spawn smoke effect.
        crashEffectLocation.y = (this->actor.world.pos.y + 30.0f) - 50.0f;
        EnClearTag_CreateSmokeEffect(play, &crashEffectLocation, 3.0f);
        crashEffectLocation.y = this->actor.world.pos.y;

        // Spawn debris effects.
        for (i = 0; i < 15; i++) {
            crashEffectVelocity.x = sinf(i * 1.65f) * i * 0.3f;
            crashEffectVelocity.z = cosf(i * 1.65f) * i * 0.3f;
            crashEffectVelocity.y = Rand_ZeroFloat(6.0f) + 5.0f;
            crashEffectVelocity.x += Rand_CenteredFloat(0.5f);
            crashEffectVelocity.z += Rand_CenteredFloat(0.5f);

            debrisEffectAcceleration.x = 0.0f;
            debrisEffectAcceleration.y = -1.0f;
            debrisEffectAcceleration.z = 0.0f;

            EnClearTag_CreateDebrisEffect(play, &crashEffectLocation, &crashEffectVelocity,
                                          &debrisEffectAcceleration, Rand_ZeroFloat(0.15f) + 0.075f,
                                          this->actor.floorHeight);
        }
    }

    if (this->drawMode != CLEAR_TAG_DRAW_MODE_ARWING) {
        // Check if the Arwing should be removed.
        if ((this->drawMode == CLEAR_TAG_DRAW_MODE_EFFECT) && (DECR(this->deathTimer) == 0)) {
            Actor_Kill(&this->actor);
        }

        EnClearTag_UpdateEffects(play);
    }
}

/**
 * EnClear_Tag draw function.
 * Laser clear tag type will draw two lasers.
 * Arwing clear tage types will draw the Arwing, the backfire, and a shadow.
 */
void EnClearTag_Draw(Actor* thisx, PlayState* play) {
    s32 pad;
    EnClearTag* this = (EnClearTag*)thisx;

    OPEN_DISPS(play->state.gfxCtx);
    if (this->drawMode != CLEAR_TAG_DRAW_MODE_EFFECT) {
        Gfx_SetupDL_25Xlu(play->state.gfxCtx);

        if (this->state >= CLEAR_TAG_STATE_LASER) {
            // Draw Arwing lasers.
            gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 0, 255, 0, 255);

            Matrix_Translate(25.0f, 0.0f, 0.0f, MTXMODE_APPLY);
            gSPMatrix(POLY_XLU_DISP++, MATRIX_NEWMTX(play->state.gfxCtx),
                      G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
            gSPDisplayList(POLY_XLU_DISP++, gArwingLaserDL);

            Matrix_Translate(-50.0f, 0.0f, 0.0f, MTXMODE_APPLY);
            gSPMatrix(POLY_XLU_DISP++, MATRIX_NEWMTX(play->state.gfxCtx),
                      G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
            gSPDisplayList(POLY_XLU_DISP++, gArwingLaserDL);
        } else {
            // Draw the Arwing itself.
            Gfx_SetupDL_25Opa(play->state.gfxCtx);
            gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 255, 255, 255, 255);
            if (this->crashingTimer != 0) {
                f32 xRotation;
                f32 yRotation;
                f32 scaledCrashingTimer = this->crashingTimer * 0.05f;

                xRotation = Math_SinS(this->frameCounter * 0x3000) * scaledCrashingTimer;
                yRotation = Math_SinS(this->frameCounter * 0x3700) * scaledCrashingTimer;
                Matrix_RotateX(xRotation, MTXMODE_APPLY);
                Matrix_RotateY(yRotation, MTXMODE_APPLY);
            }
            Matrix_RotateZ(this->roll, MTXMODE_APPLY);
            gSPMatrix(POLY_OPA_DISP++, MATRIX_NEWMTX(play->state.gfxCtx),
                      G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
            gSPDisplayList(POLY_OPA_DISP++, gArwingDL);

            // Draw the Arwing Backfire
            Matrix_Translate(0.0f, 0.0f, -60.0f, MTXMODE_APPLY);
            Matrix_ReplaceRotation(&play->billboardMtxF);
            Matrix_Scale(2.5f, 1.3f, 0.0f, MTXMODE_APPLY);
            if ((this->frameCounter % 2) != 0) {
                Matrix_Scale(1.15f, 1.15f, 1.15f, MTXMODE_APPLY);
            }
            gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 255, 255, 200, 155);
            gDPPipeSync(POLY_XLU_DISP++);
            gDPSetEnvColor(POLY_XLU_DISP++, 255, 50, 0, 0);
            gSPMatrix(POLY_XLU_DISP++, MATRIX_NEWMTX(play->state.gfxCtx),
                      G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
            gSPDisplayList(POLY_XLU_DISP++, gArwingBackfireDL);

            // Draw the Arwing shadow.
            gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 0, 0, 0, 130);
            Matrix_Translate(this->actor.world.pos.x, this->actor.floorHeight, this->actor.world.pos.z, MTXMODE_NEW);
            Matrix_RotateX(this->floorTangent.x, MTXMODE_APPLY);
            Matrix_RotateZ(this->floorTangent.z, MTXMODE_APPLY);
            Matrix_Scale(this->actor.scale.x + 0.35f, 0.0f, this->actor.scale.z + 0.35f, MTXMODE_APPLY);
            Matrix_RotateY((this->actor.shape.rot.y / 32768.0f) * M_PI, MTXMODE_APPLY);
            Matrix_RotateX((this->actor.shape.rot.x / 32768.0f) * M_PI, MTXMODE_APPLY);
            Matrix_RotateZ((this->actor.shape.rot.z / 32768.0f) * M_PI, MTXMODE_APPLY);
            if (this->crashingTimer != 0) {
                f32 xRotation;
                f32 yRotation;
                f32 scaledCrashingTimer = this->crashingTimer * 0.05f;

                xRotation = Math_SinS(this->frameCounter * 0x3000) * scaledCrashingTimer;
                yRotation = Math_SinS(this->frameCounter * 0x3700) * scaledCrashingTimer;
                Matrix_RotateX(xRotation, MTXMODE_APPLY);
                Matrix_RotateY(yRotation, MTXMODE_APPLY);
            }
            Matrix_RotateZ(this->roll, MTXMODE_APPLY);
            gSPMatrix(POLY_XLU_DISP++, MATRIX_NEWMTX(play->state.gfxCtx),
                      G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
            gSPDisplayList(POLY_XLU_DISP++, gArwingShadowDL);
        }
    }

    if (this->drawMode != CLEAR_TAG_DRAW_MODE_ARWING) {
        EnClearTag_DrawEffects(play);
    }

    CLOSE_DISPS(play->state.gfxCtx);
}

/**
 * Updates the Arwing effects.
 * Performs effect physics.
 * Moves and bounces debris effects.
 * Fades most effects out of view. When effects are completely faded away they are removed.
 */
void EnClearTag_UpdateEffects(PlayState* play) {
    EnClearTagEffect* effect = (EnClearTagEffect*)play->specialEffects;
    s16 i;
    f32 originalYPosition;
    Vec3f sphereCenter;

    for (i = 0; i < CLEAR_TAG_EFFECT_MAX_COUNT; i++, effect++) {
        if (effect->type != CLEAR_TAG_EFFECT_AVAILABLE) {
            effect->random++;

            // Perform effect physics.
            effect->position.x += effect->velocity.x;
            originalYPosition = effect->position.y;
            effect->position.y += effect->velocity.y;
            effect->position.z += effect->velocity.z;
            effect->velocity.x += effect->acceleration.x;
            effect->velocity.y += effect->acceleration.y;
            effect->velocity.z += effect->acceleration.z;

            if (effect->type == CLEAR_TAG_EFFECT_DEBRIS) {
                // Clamp the velocity to -5.0
                if (effect->velocity.y < -5.0f) {
                    effect->velocity.y = -5.0f;
                }

                // While the effect is falling check if it has hit the ground.
                if (effect->velocity.y < 0.0f) {
                    sphereCenter = effect->position;
                    sphereCenter.y += 5.0f;

                    // Check if the debris has hit the ground.
                    if (BgCheck_SphVsFirstPoly(&play->colCtx, &sphereCenter, 11.0f)) {
                        effect->position.y = originalYPosition;

                        // Bounce the debris effect.
                        if (effect->bounces <= 0) {
                            effect->bounces++;
                            effect->velocity.y *= -0.5f;
                            effect->timer = ((s16)Rand_ZeroFloat(20)) + 25;
                        } else {
                            // The Debris effect is done bouncing. Set its velocity and acceleration to 0.
                            effect->velocity.x = effect->velocity.z = effect->acceleration.y = effect->velocity.y =
                                0.0f;
                        }
                    }
                }

                // Rotate the debris effect.
                if (effect->acceleration.y != 0.0f) {
                    effect->rotationY += 0.5f;
                    effect->rotationX += 0.35f;
                }

                // If the timer is completed, unload the debris effect.
                if (effect->timer == 1) {
                    effect->type = CLEAR_TAG_EFFECT_AVAILABLE;
                }

                // Spawn a fire effect every 3 frames.
                if (effect->random >= 3) {
                    effect->random = 0;
                    EnClearTag_CreateFireEffect(play, &effect->position, effect->scale * 8.0f);
                }
            } else if (effect->type == CLEAR_TAG_EFFECT_FIRE) {
                // Fade the fire effect.
                Math_ApproachZeroF(&effect->primColor.a, 1.0f, 15.0f);
                // If the fire effect is fully faded, unload it.
                if (effect->primColor.a <= 0.0f) {
                    effect->type = CLEAR_TAG_EFFECT_AVAILABLE;
                }
            } else if (effect->type == CLEAR_TAG_EFFECT_SMOKE) {
                // Fade the smoke effects.
                Math_ApproachZeroF(&effect->primColor.r, 1.0f, 20.0f);
                Math_ApproachZeroF(&effect->primColor.g, 1.0f, 2.0f);
                Math_ApproachZeroF(&effect->envColor.r, 1.0f, 25.5f);
                Math_ApproachZeroF(&effect->envColor.g, 1.0f, 21.5f);
                Math_ApproachZeroF(&effect->envColor.b, 1.0f, 25.5f);

                // Smooth scale the smoke effects.
                Math_ApproachF(&effect->scale, effect->maxScale, 0.05f, 0.1f);

                if (effect->primColor.r == 0.0f) {
                    // Fade the smoke effects.
                    Math_ApproachZeroF(&effect->primColor.a, 1.0f, 3.0f);

                    // If the smoke effect has fully faded, unload it.
                    if (effect->primColor.a <= 0.0f) {
                        effect->type = CLEAR_TAG_EFFECT_AVAILABLE;
                    }
                }
            } else if (effect->type == CLEAR_TAG_EFFECT_FLASH) {
                // Smooth scale the flash effects.
                Math_ApproachF(&effect->scale, effect->maxScale, 1.0f, 3.0f);
                // Fade the flash effects.
                Math_ApproachZeroF(&effect->primColor.a, 1.0f, 10.0f);

                // If the flash effect has fully faded, unload it.
                if (effect->primColor.a <= 0.0f) {
                    effect->type = CLEAR_TAG_EFFECT_AVAILABLE;
                }
            }

            if (effect->timer != 0) {
                effect->timer--;
            }
        }
    }
}

/**
 * Draws the Arwing effects.
 * Each effect type is drawn before the next. The function will apply a material that applies to all effects of that
 * type while drawing the first effect of that type.
 */
void EnClearTag_DrawEffects(PlayState* play) {
    s16 i;
    GraphicsContext* gfxCtx = play->state.gfxCtx;
    u8 isMaterialApplied = false;
    EnClearTagEffect* effect = (EnClearTagEffect*)play->specialEffects;
    EnClearTagEffect* firstEffect = effect;

    OPEN_DISPS(gfxCtx);
    Gfx_SetupDL_25Opa(play->state.gfxCtx);
    Gfx_SetupDL_25Xlu(play->state.gfxCtx);

    // Draw all Debris effects.
    for (i = 0; i < CLEAR_TAG_EFFECT_MAX_COUNT; i++, effect++) {
        FrameInterpolation_RecordOpenChild(effect, effect->epoch);

        if (effect->type == CLEAR_TAG_EFFECT_DEBRIS) {
            // Apply the debris effect material if it has not already been applied.
            if (!isMaterialApplied) {
                isMaterialApplied++;
                gSPDisplayList(POLY_OPA_DISP++, gArwingDebrisEffectMaterialDL);
            }

            // Draw the debris effect.
            Matrix_Translate(effect->position.x, effect->position.y, effect->position.z, MTXMODE_NEW);
            Matrix_Scale(effect->scale, effect->scale, effect->scale, MTXMODE_APPLY);
            Matrix_RotateY(effect->rotationY, MTXMODE_APPLY);
            Matrix_RotateX(effect->rotationX, MTXMODE_APPLY);
            gSPMatrix(POLY_OPA_DISP++, MATRIX_NEWMTX(gfxCtx),
                      G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
            gSPDisplayList(POLY_OPA_DISP++, gArwingDebrisEffectDL);
        }

        FrameInterpolation_RecordCloseChild();
    }

    // Draw all ground flash effects.
    effect = firstEffect;
    isMaterialApplied = false;
    for (i = 0; i < CLEAR_TAG_EFFECT_MAX_COUNT; i++, effect++) {
        if (effect->type == CLEAR_TAG_EFFECT_FLASH) {
            FrameInterpolation_RecordOpenChild(effect, effect->epoch);

            // Apply the flash ground effect material if it has not already been applied.
            if (!isMaterialApplied) {
                gDPPipeSync(POLY_XLU_DISP++);
                gDPSetEnvColor(POLY_XLU_DISP++, 255, 255, 200, 0);
                isMaterialApplied++;
            }

            // Draw the ground flash effect.
            gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 255, 255, 200, (s8)effect->primColor.a);
            Matrix_Translate(effect->position.x, effect->floorHeight, effect->position.z, MTXMODE_NEW);
            Matrix_RotateX(effect->floorTangent.x, MTXMODE_APPLY);
            Matrix_RotateZ(effect->floorTangent.z, MTXMODE_APPLY);
            Matrix_Scale(effect->scale + effect->scale, 1.0f, effect->scale * 2.0f, MTXMODE_APPLY);
            gSPMatrix(POLY_XLU_DISP++, MATRIX_NEWMTX(gfxCtx),
                      G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
            gSPDisplayList(POLY_XLU_DISP++, gArwingFlashEffectGroundDL);

            FrameInterpolation_RecordCloseChild();
        }
    }

    // Draw all smoke effects.
    effect = firstEffect;
    isMaterialApplied = false;
    for (i = 0; i < CLEAR_TAG_EFFECT_MAX_COUNT; i++, effect++) {
        if (effect->type == CLEAR_TAG_EFFECT_SMOKE) {
            FrameInterpolation_RecordOpenChild(effect, effect->epoch);

            // Apply the smoke effect material if it has not already been applied.
            if (!isMaterialApplied) {
                gSPDisplayList(POLY_XLU_DISP++, gArwingFireEffectMaterialDL);
                isMaterialApplied++;
            }

            // Draw the smoke effect.
            gDPPipeSync(POLY_XLU_DISP++);
            gDPSetEnvColor(POLY_XLU_DISP++, (s8)effect->envColor.r, (s8)effect->envColor.g, (s8)effect->envColor.b,
                           128);
            gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, (s8)effect->primColor.r, (s8)effect->primColor.g,
                            (s8)effect->primColor.b, (s8)effect->primColor.a);
            gSPSegment(POLY_XLU_DISP++, 8,
                       Gfx_TwoTexScroll(play->state.gfxCtx, 0, 0, effect->random * -5, 32, 64, 1, 0, 0, 32, 32));
            Matrix_Translate(effect->position.x, effect->position.y, effect->position.z, MTXMODE_NEW);
            Matrix_ReplaceRotation(&play->billboardMtxF);
            Matrix_Scale(effect->scale, effect->scale, 1.0f, MTXMODE_APPLY);
            Matrix_Translate(0.0f, 20.0f, 0.0f, MTXMODE_APPLY);
            gSPMatrix(POLY_XLU_DISP++, MATRIX_NEWMTX(gfxCtx),
                      G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
            gSPDisplayList(POLY_XLU_DISP++, gArwingFireEffectDL);
        
            FrameInterpolation_RecordCloseChild();
        }
    }

    // Draw all fire effects.
    effect = firstEffect;
    isMaterialApplied = false;
    for (i = 0; i < CLEAR_TAG_EFFECT_MAX_COUNT; i++, effect++) {
        if (effect->type == CLEAR_TAG_EFFECT_FIRE) {
            FrameInterpolation_RecordOpenChild(effect, effect->epoch);

            // Apply the fire effect material if it has not already been applied.
            if (!isMaterialApplied) {
                gSPDisplayList(POLY_XLU_DISP++, gArwingFireEffectMaterialDL);
                gDPSetEnvColor(POLY_XLU_DISP++, 255, 215, 255, 128);
                isMaterialApplied++;
            }

            // Draw the fire effect.
            gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 200, 20, 0, (s8)effect->primColor.a);
            gSPSegment(POLY_XLU_DISP++, 8,
                       Gfx_TwoTexScroll(play->state.gfxCtx, 0, 0, (effect->random * -15) & 0xFF, 32, 64, 1, 0, 0,
                                        32, 32));
            Matrix_Translate(effect->position.x, effect->position.y, effect->position.z, MTXMODE_NEW);
            Matrix_ReplaceRotation(&play->billboardMtxF);
            Matrix_Scale(effect->scale, effect->scale, 1.0f, MTXMODE_APPLY);
            gSPMatrix(POLY_XLU_DISP++, MATRIX_NEWMTX(gfxCtx),
                      G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
            gSPDisplayList(POLY_XLU_DISP++, gArwingFireEffectDL);

            FrameInterpolation_RecordCloseChild();
        }
    }

    // Draw all flash billboard effects.
    effect = firstEffect;
    isMaterialApplied = false;
    for (i = 0; i < CLEAR_TAG_EFFECT_MAX_COUNT; i++, effect++) {
        if (effect->type == CLEAR_TAG_EFFECT_FLASH) {
            FrameInterpolation_RecordOpenChild(effect, effect->epoch);

            // Apply the flash billboard effect material if it has not already been applied.
            if (!isMaterialApplied) {
                gDPPipeSync(POLY_XLU_DISP++);
                gDPSetEnvColor(POLY_XLU_DISP++, 255, 255, 200, 0);
                isMaterialApplied++;
            }

            // Draw the flash billboard effect.
            gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 255, 255, 200, (s8)effect->primColor.a);
            Matrix_Translate(effect->position.x, effect->position.y, effect->position.z, MTXMODE_NEW);
            Matrix_ReplaceRotation(&play->billboardMtxF);
            Matrix_Scale(effect->scale, effect->scale, 1.0f, MTXMODE_APPLY);
            gSPMatrix(POLY_XLU_DISP++, MATRIX_NEWMTX(gfxCtx),
                      G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
            gSPDisplayList(POLY_XLU_DISP++, gArwingFlashEffectDL);

            FrameInterpolation_RecordCloseChild();
        }
    }

    CLOSE_DISPS(gfxCtx);
}

void EnClearTag_Reset(void) {
    memset(sClearTagEffects, 0, sizeof(sClearTagEffects));
    sClearTagIsEffectsInitialized = false;
}