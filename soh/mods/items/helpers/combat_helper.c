/**
 * combat_helper.c - Combat colliders and damage utilities
 */

#include "combat_helper.h"
#include "macros.h"
#include "functions.h"
#include "variables.h"

void Combat_InitCylinder(PlayState* play, ColliderCylinder* col, Actor* owner, CombatColliderConfig* cfg) {
    Collider_InitCylinder(play, col);
    Collider_SetCylinder(play, col, owner,
                         &(ColliderCylinderInit){
                             { COLTYPE_NONE, AT_ON | AT_TYPE_PLAYER, AC_NONE, OC1_NONE, OC2_NONE, COLSHAPE_CYLINDER },
                             { ELEMTYPE_UNK2,
                               { cfg->dmgFlags, cfg->effect, cfg->damage },
                               { 0, 0, 0 },
                               TOUCH_ON | TOUCH_SFX_NORMAL,
                               BUMP_NONE,
                               OCELEM_NONE },
                             { (s16)cfg->radius, (s16)cfg->height, 0, { 0, 0, 0 } }
                         });
}

void Combat_UpdateCylinder(ColliderCylinder* col, Vec3f* pos, CombatColliderConfig* cfg) {
    col->dim.pos.x = (s16)pos->x;
    col->dim.pos.y = (s16)pos->y;
    col->dim.pos.z = (s16)pos->z;
    col->dim.radius = (s16)cfg->radius;
    col->dim.height = (s16)cfg->height;
    col->info.toucher.dmgFlags = cfg->dmgFlags;
    col->info.toucher.damage = cfg->damage;
    col->info.toucher.effect = cfg->effect;
    col->base.atFlags |= AT_ON | AT_TYPE_PLAYER;
}

void Combat_RegisterCollider(PlayState* play, ColliderCylinder* col) {
    CollisionCheck_SetAT(play, &play->colChkCtx, &col->base);
}

inline u8 Combat_CheckHit(ColliderCylinder* col) {
    return (col->base.atFlags & AT_HIT) ? 1 : 0;
}

void Combat_PlayHitSFX(Vec3f* pos) {
    Audio_PlaySoundGeneral(NA_SE_IT_HAMMER_HIT, pos, 4,
                           &gSfxDefaultFreqAndVolScale, &gSfxDefaultFreqAndVolScale, &gSfxDefaultReverb);
}

void Combat_ApplyKnockback(Actor* actor, f32 knockbackSpeed, f32 knockbackHeight) {
    if (actor == NULL || actor->update == NULL) return;

    s16 knockbackYaw = actor->yawTowardsPlayer + ANGLE_180_DEG;
    actor->world.rot.y = knockbackYaw;
    actor->speedXZ = knockbackSpeed;
    actor->velocity.y = knockbackHeight;
}

void Combat_ApplyKnockbackFromPoint(Actor* actor, Vec3f* sourcePos, f32 knockbackSpeed, f32 knockbackHeight) {
    if (actor == NULL || actor->update == NULL) return;

    s16 knockbackYaw = Math_Vec3f_Yaw(sourcePos, &actor->world.pos);
    actor->world.rot.y = knockbackYaw;
    actor->speedXZ = knockbackSpeed;
    actor->velocity.y = knockbackHeight;
}

u8 Combat_IsActorInRange(Actor* actor, f32 xzRange, f32 yRange) {
    if (actor == NULL) return 0;
    return (actor->xzDistToPlayer < xzRange) && (fabsf(actor->yDistToPlayer) < yRange);
}

void Combat_DamageActor(Actor* actor, u8 damage, u8 effect) {
    if (actor == NULL || actor->update == NULL) return;
    if (actor->colChkInfo.health <= 0) return;

    actor->colChkInfo.damage = damage;
    actor->colChkInfo.damageEffect = effect;
}

void Combat_DamageEnemiesInRadius(PlayState* play, Vec3f* center, f32 radius, u8 element, s32 damage) {
    f32 radiusSq = SQ(radius);
    Actor* actor = play->actorCtx.actorLists[ACTORCAT_ENEMY].head;

    while (actor != NULL) {
        Actor* next = actor->next;
        if (actor->update != NULL && actor->colChkInfo.health > 0) {
            f32 dx = actor->world.pos.x - center->x;
            f32 dy = actor->world.pos.y - center->y;
            f32 dz = actor->world.pos.z - center->z;
            f32 distSq = SQ(dx) + SQ(dy) + SQ(dz);

            if (distSq < radiusSq) {
                actor->colChkInfo.damage = damage;
                actor->colChkInfo.damageEffect = element;
                Combat_ApplyKnockbackFromPoint(actor, center, KNOCKBACK_SPEED_STRONG, KNOCKBACK_HEIGHT_MEDIUM);
            }
        }
        actor = next;
    }
}
