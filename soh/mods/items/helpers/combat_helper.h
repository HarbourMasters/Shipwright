/**
 * combat_helper.h - Combat colliders and damage utilities
 */

#ifndef COMBAT_HELPER_H
#define COMBAT_HELPER_H

#include "z64.h"
#include "../custom_items.h"

#ifdef __cplusplus
extern "C" {
#endif

#define KNOCKBACK_SPEED_STRONG   8.0f   // units/frame
#define KNOCKBACK_HEIGHT_MEDIUM  6.0f   // units/frame
#define ANGLE_180_DEG  0x8000

/**
 * Combat collider configuration for AT collider setup.
 */
typedef struct {
    u32 dmgFlags;   // DMG_HAMMER, DMG_SWORD, etc.
    u8 damage;      // Damage amount (hearts = damage/2)
    u8 effect;      // Knockback, fire, ice, etc.
    f32 radius;     // Cylinder radius
    f32 height;     // Cylinder height
} CombatColliderConfig;

/**
 * Initialize a cylinder AT collider.
 * @param play PlayState instance
 * @param col Collider to initialize
 * @param owner Actor that owns this collider
 * @param cfg Combat configuration
 */
void Combat_InitCylinder(PlayState* play, ColliderCylinder* col, Actor* owner, CombatColliderConfig* cfg);

/**
 * Update collider position and config.
 * @param col Collider to update
 * @param pos New world position
 * @param cfg Updated configuration
 */
void Combat_UpdateCylinder(ColliderCylinder* col, Vec3f* pos, CombatColliderConfig* cfg);

/**
 * Register collider in AT collision system.
 * @param play PlayState instance
 * @param col Collider to register
 */
void Combat_RegisterCollider(PlayState* play, ColliderCylinder* col);

/**
 * Check if collider hit something this frame.
 * For performance: use (col->base.atFlags & AT_HIT) directly.
 * @param col Collider to check
 * @return 1 if hit, 0 otherwise
 */
u8 Combat_CheckHit(ColliderCylinder* col);

/**
 * Play hammer hit sound effect.
 * @param pos World position for 3D audio
 */
void Combat_PlayHitSFX(Vec3f* pos);

/**
 * Apply knockback using pre-calculated yawTowardsPlayer.
 * @param actor Target actor
 * @param knockbackSpeed XZ speed (units/frame)
 * @param knockbackHeight Y velocity (units/frame)
 */
void Combat_ApplyKnockback(Actor* actor, f32 knockbackSpeed, f32 knockbackHeight);

/**
 * Apply knockback from a specific point.
 * @param actor Target actor
 * @param sourcePos Origin point for knockback direction
 * @param knockbackSpeed XZ speed (units/frame)
 * @param knockbackHeight Y velocity (units/frame)
 */
void Combat_ApplyKnockbackFromPoint(Actor* actor, Vec3f* sourcePos, f32 knockbackSpeed, f32 knockbackHeight);

/**
 * Check if actor is within attack range.
 * @param actor Target actor
 * @param xzRange Horizontal distance threshold
 * @param yRange Vertical distance threshold
 * @return 1 if in range, 0 otherwise
 */
u8 Combat_IsActorInRange(Actor* actor, f32 xzRange, f32 yRange);

/**
 * Apply damage to actor via colChkInfo.
 * @param actor Target actor
 * @param damage Damage amount
 * @param effect Damage effect type
 */
void Combat_DamageActor(Actor* actor, u8 damage, u8 effect);

/**
 * Damage all enemies within radius.
 * @param play PlayState instance
 * @param center Origin point
 * @param radius Effect radius
 * @param element Damage element type
 * @param damage Damage amount
 */
void Combat_DamageEnemiesInRadius(PlayState* play, Vec3f* center, f32 radius, u8 element, s32 damage);

#ifdef __cplusplus
}
#endif

#endif // COMBAT_HELPER_H
