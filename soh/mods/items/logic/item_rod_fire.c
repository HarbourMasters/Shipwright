/**
 * item_rod_fire.c - Fire Rod from A Link Between Worlds
 *
 * Controls:
 *   B Button:  Swing rod (uses sword mechanics)
 *   C-UP:      Toggle first-person aiming mode
 *
 * Attack Types:
 *   - Slash: 3 fireballs spread at +30/0/-30 degrees
 *   - Stab: Single long-range fireball
 *   - Jump Slash: Flamethrower cone (6 colliders)
 *   - Spin Attack: Expanding fire cylinder
 *
 * Special: Burns enemies on hit, backfire burns Link
 */

#include "item_rod_fire.h"
#include "../helpers/equip_helper.h"
#include "../helpers/combat_helper.h"
#include "../helpers/fx_helper.h"
#include "../helpers/camera_helper.h"

static ItemEquipState sEquipState = { 0 };
static s8 sPrevInvinc = 0;
static u8 sLastSwingType = 0;
static u8 sJumpEffectSpawned = 0;
static u8 sChargeButtonHeld = 0;
static s16 sChargeHoldCounter = 0;
static f32 sFireRodTargetScale = 2.0f;
static f32 sSpinExpandProgress = 0.0f;
static u8 sSpinColliderInited = 0;
static u8 sFlameCollidersInited = 0;
static u8 sMultiCollidersInited = 0;

static RodColor sFireRodColor = {
    FIRE_ROD_PRIM_R, FIRE_ROD_PRIM_G, FIRE_ROD_PRIM_B, FIRE_ROD_PRIM_A,
    FIRE_ROD_ENV_R, FIRE_ROD_ENV_G, FIRE_ROD_ENV_B, FIRE_ROD_ENV_A
};

extern int Player_IsZTargeting(Player* this);
extern void func_80837948(PlayState* play, Player* player, s32 meleeWeaponAnim);

// Aliases for multi-projectile system
#define fireRodProjPos2     gCustomItemState.fireRodProjPos2
#define fireRodProjPos3     gCustomItemState.fireRodProjPos3
#define fireRodProjVel      gCustomItemState.fireRodProjVel
#define fireRodProjCount    gCustomItemState.fireRodProjCount
#define fireRodCollider2    gCustomItemState.fireRodCollider2
#define fireRodCollider3    gCustomItemState.fireRodCollider3
#define fireRodFlameActive  gCustomItemState.fireRodFlameActive
#define fireRodFlameTimer   gCustomItemState.fireRodFlameTimer
#define fireRodFlamePos     gCustomItemState.fireRodFlamePos
#define fireRodFlameColliders gCustomItemState.fireRodFlameColliders

// =============================================================================
// BACKFIRE - Sets Link on fire when using fire rod without magic
// =============================================================================

// func_8083821C: Sets Link's body on fire (initializes bodyFlameTimers and bodyIsBurning)
extern void func_8083821C(Player* this);

static void FireRod_Backfire(Player* p, PlayState* play) {
    Audio_PlayActorSound2(&p->actor, FIRE_ROD_SFX_BACKFIRE_HIT);
    Audio_PlayActorSound2(&p->actor, NA_SE_VO_LI_FALL_L);

    // Set Link on fire using the real burn system
    func_8083821C(p);

    // func_8002F6D4: Applies knockback (speed, direction, height, type)
    func_8002F6D4(play, &p->actor, 4.0f, p->actor.shape.rot.y + 0x8000, 6.0f, 0);
}

static u8 FireRod_CheckBackfire(Player* p, PlayState* play, s16 magicCost, u8 backfireChance) {
    if (ItemMagic_HasEnough(play, magicCost)) return 0;

    u8 roll = (u8)(Rand_ZeroOne() * 100.0f);
    if (roll < backfireChance) {
        FireRod_Backfire(p, play);
        return 1;
    }

    Sfx_PlaySfxCentered(FIRE_ROD_SFX_NO_MAGIC);
    return 1;
}

// =============================================================================
// MULTI-PROJECTILE SYSTEM - Supports 1-3 simultaneous fireballs
// =============================================================================

static void FireRod_InitColliders(Player* p, PlayState* play) {
    if (sMultiCollidersInited) return;

    Collider_InitCylinder(play, &fireRodCollider);
    Collider_SetCylinder(play, &fireRodCollider, &p->actor, &sFireRodProjColInit);
    Collider_InitCylinder(play, &fireRodCollider2);
    Collider_SetCylinder(play, &fireRodCollider2, &p->actor, &sFireRodProjColInit);
    Collider_InitCylinder(play, &fireRodCollider3);
    Collider_SetCylinder(play, &fireRodCollider3, &p->actor, &sFireRodProjColInit);
    sMultiCollidersInited = 1;
}

static void FireRod_CalcVelocity(Vec3f* outVel, s16 yaw, s16 pitch) {
    Vec3f localVel = { 0.0f, 0.0f, FIRE_ROD_PROJ_SPEED };
    Matrix_Push();
    Matrix_RotateY(BINANG_TO_RAD(yaw), MTXMODE_NEW);
    Matrix_RotateX(BINANG_TO_RAD(pitch), MTXMODE_APPLY);
    Matrix_MultVec3f(&localVel, outVel);
    Matrix_Pop();
}

// Spawns single projectile (stab, first-person)
static void FireRod_InitSingleProjectile(Player* p, PlayState* play, Vec3f* startPos, s16 yaw, s16 pitch, f32 maxRange) {
    FireRod_InitColliders(p, play);

    sFireRodTargetScale = 2.0f;
    fireRodProjActive = 1;
    fireRodProjCount = 1;
    fireRodProjPos = *startPos;
    fireRodProjTimer = (s16)(maxRange / FIRE_ROD_PROJ_SPEED);
    if (fireRodProjTimer < 10) fireRodProjTimer = 10;
    if (fireRodProjTimer > 30) fireRodProjTimer = 30;

    fireRodProjScale = 0.0f;
    fireRodProjRotZ = 0;
    fireRodProjTrailIdx = 0;
    for (s32 i = 0; i < 6; i++) fireRodProjTrail[i] = *startPos;

    fireRodProjYaw = yaw;
    fireRodProjPitch = pitch;
    FireRod_CalcVelocity(&fireRodProjVel[0], yaw, pitch);
}

// Spawns 3 fireballs spread at configurable angles (slash attack)
static void FireRod_InitTripleProjectile(Player* p, PlayState* play, Vec3f* startPos, s16 baseYaw, s16 pitch) {
    FireRod_InitColliders(p, play);

    sFireRodTargetScale = 2.0f;
    fireRodProjActive = 1;
    fireRodProjCount = 3;

    s16 spreadAngle = (s16)(FIRE_ROD_SLASH_SPREAD * (0x10000 / 360));
    s16 timer = (s16)(FIRE_ROD_SLASH_RANGE / FIRE_ROD_PROJ_SPEED);
    if (timer < 10) timer = 10;
    fireRodProjTimer = timer;

    fireRodProjScale = 0.0f;
    fireRodProjRotZ = 0;
    fireRodProjTrailIdx = 0;

    // Center fireball
    fireRodProjPos = *startPos;
    fireRodProjYaw = baseYaw;
    fireRodProjPitch = pitch;
    FireRod_CalcVelocity(&fireRodProjVel[0], baseYaw, pitch);
    for (s32 i = 0; i < 6; i++) fireRodProjTrail[i] = *startPos;

    // Left fireball (-spread angle)
    fireRodProjPos2 = *startPos;
    FireRod_CalcVelocity(&fireRodProjVel[1], baseYaw - spreadAngle, pitch);

    // Right fireball (+spread angle)
    fireRodProjPos3 = *startPos;
    FireRod_CalcVelocity(&fireRodProjVel[2], baseYaw + spreadAngle, pitch);
}

static void FireRod_UpdateCollider(ColliderCylinder* col, Vec3f* pos, f32 scale, PlayState* play) {
    // VFX uses: scale * 0.0015f * ~1000 (gEffFire1DL base) = scale * 1.5 visual radius
    // Collider should match: scale * 1.5 for radius, slightly taller for height
    col->dim.radius = (s16)(scale * 1.5f + 2.0f);
    col->dim.height = (s16)(scale * 2.0f + 3.0f);
    col->dim.pos.x = (s16)pos->x;
    col->dim.pos.y = (s16)pos->y;
    col->dim.pos.z = (s16)pos->z;
    CollisionCheck_SetAT(play, &play->colChkCtx, &col->base);
}

static u8 FireRod_CheckHit(ColliderCylinder* col, Vec3f* pos, PlayState* play, Player* p) {
    if (col->base.atFlags & AT_HIT) {
        Vec3f zero = { 0.0f, 0.0f, 0.0f };
        EffectSsBomb2_SpawnLayered(play, pos, &zero, &zero, 10, 5);
        Audio_PlayActorSound2(&p->actor, FIRE_ROD_SFX_FIRE_EXPLODE);
        col->base.atFlags &= ~AT_HIT;
        return 1;
    }
    return 0;
}

// EffectSsKiraKira_SpawnDispersed: Creates sparkle particles that disperse outward
static void FireRod_SpawnFireSparks(PlayState* play, Vec3f* pos, f32 scale) {
    Color_RGBA8 primColor = { FIRE_ROD_PRIM_R, FIRE_ROD_PRIM_G, FIRE_ROD_PRIM_B, FIRE_ROD_PRIM_A };
    Color_RGBA8 envColor = { FIRE_ROD_ENV_R, FIRE_ROD_ENV_G, FIRE_ROD_ENV_B, FIRE_ROD_ENV_A };
    Vec3f vel = { 0.0f, 0.0f, 0.0f };
    Vec3f accel = { 0.0f, 0.0f, 0.0f };

    for (s32 i = 0; i < 10; i++) {
        Vec3f sparkPos;
        sparkPos.x = pos->x + (Rand_ZeroOne() - 0.5f) * (scale * 20.0f);
        sparkPos.y = pos->y + (Rand_ZeroOne() - 0.5f) * (scale * 20.0f);
        sparkPos.z = pos->z;
        EffectSsKiraKira_SpawnDispersed(play, &sparkPos, &vel, &accel, &primColor, &envColor, 1000, 10);
    }
}

static void FireRod_UpdateProjectile(Player* p, PlayState* play) {
    if (!fireRodProjActive) return;

    fireRodProjRotZ += 5000;

    if (fireRodProjTimer > 0) fireRodProjTimer--;
    if (fireRodProjTimer == 0) sFireRodTargetScale = 0.0f;

    Math_ApproachF(&fireRodProjScale, sFireRodTargetScale, 0.2f, 0.4f);

    if (fireRodProjTimer == 0 && fireRodProjScale < 0.1f) {
        fireRodProjActive = 0;
        sFireRodTargetScale = 2.0f;
        return;
    }

    // Update all active projectiles
    fireRodProjPos.x += fireRodProjVel[0].x;
    fireRodProjPos.y += fireRodProjVel[0].y;
    fireRodProjPos.z += fireRodProjVel[0].z;

    if (fireRodProjCount >= 2) {
        fireRodProjPos2.x += fireRodProjVel[1].x;
        fireRodProjPos2.y += fireRodProjVel[1].y;
        fireRodProjPos2.z += fireRodProjVel[1].z;
    }
    if (fireRodProjCount >= 3) {
        fireRodProjPos3.x += fireRodProjVel[2].x;
        fireRodProjPos3.y += fireRodProjVel[2].y;
        fireRodProjPos3.z += fireRodProjVel[2].z;
    }

    // Trail for center projectile
    fireRodProjTrail[0] = fireRodProjPos;
    for (s32 i = 4; i >= 0; i--) fireRodProjTrail[i + 1] = fireRodProjTrail[i];

    // Sparks and sound
    if (fireRodProjScale >= 0.4f) {
        FireRod_SpawnFireSparks(play, &fireRodProjPos, fireRodProjScale);
        if (fireRodProjCount >= 2) FireRod_SpawnFireSparks(play, &fireRodProjPos2, fireRodProjScale);
        if (fireRodProjCount >= 3) FireRod_SpawnFireSparks(play, &fireRodProjPos3, fireRodProjScale);
        Audio_PlayActorSound2(&p->actor, FIRE_ROD_SFX_FIRE_LOOP - SFX_FLAG);
    }

    // Colliders
    if (fireRodProjScale >= 0.6f) {
        FireRod_UpdateCollider(&fireRodCollider, &fireRodProjPos, fireRodProjScale, play);
        if (fireRodProjCount >= 2) FireRod_UpdateCollider(&fireRodCollider2, &fireRodProjPos2, fireRodProjScale, play);
        if (fireRodProjCount >= 3) FireRod_UpdateCollider(&fireRodCollider3, &fireRodProjPos3, fireRodProjScale, play);
    }

    // Hit detection
    u8 anyHit = FireRod_CheckHit(&fireRodCollider, &fireRodProjPos, play, p);
    if (fireRodProjCount >= 2) anyHit |= FireRod_CheckHit(&fireRodCollider2, &fireRodProjPos2, play, p);
    if (fireRodProjCount >= 3) anyHit |= FireRod_CheckHit(&fireRodCollider3, &fireRodProjPos3, play, p);

    if (anyHit) {
        fireRodProjVel[0].x = fireRodProjVel[0].y = fireRodProjVel[0].z = 0.0f;
        fireRodProjVel[1].x = fireRodProjVel[1].y = fireRodProjVel[1].z = 0.0f;
        fireRodProjVel[2].x = fireRodProjVel[2].y = fireRodProjVel[2].z = 0.0f;
        fireRodProjTimer = 0;
        sFireRodTargetScale = 0.0f;
    }
}

// =============================================================================
// FLAMETHROWER SYSTEM - 6 flames with individual colliders (jump slash)
// =============================================================================

static void FireRod_InitFlameColliders(Player* p, PlayState* play) {
    if (sFlameCollidersInited) return;

    for (s32 i = 0; i < FIRE_ROD_FLAME_COUNT; i++) {
        Collider_InitCylinder(play, &fireRodFlameColliders[i]);
        Collider_SetCylinder(play, &fireRodFlameColliders[i], &p->actor, &sFireRodFlameColInit);
    }
    sFlameCollidersInited = 1;
}

static void FireRod_StartFlamethrower(Player* p, PlayState* play) {
    FireRod_InitFlameColliders(p, play);

    Vec3f impactPos = p->actor.world.pos;
    impactPos.y = p->actor.floorHeight + 5.0f;
    s16 playerYaw = p->actor.shape.rot.y;

    fireRodFlameActive = 1;
    fireRodFlameTimer = 30;

    // Position flames in a line going forward from Link
    for (s32 i = 0; i < FIRE_ROD_FLAME_COUNT; i++) {
        f32 dist = (i + 1) * FIRE_ROD_FLAME_SPACING;
        fireRodFlamePos[i].x = impactPos.x + dist * Math_SinS(playerYaw);
        fireRodFlamePos[i].y = impactPos.y + 10.0f;
        fireRodFlamePos[i].z = impactPos.z + dist * Math_CosS(playerYaw);

        // EffectSsEnFire_SpawnVec3f: Spawns torch-style fire at position (scale, flags, bodyPart)
        f32 scale = FIRE_ROD_FLAME_BASE_SCALE + (i * FIRE_ROD_FLAME_SCALE_GROW);
        EffectSsEnFire_SpawnVec3f(play, &p->actor, &fireRodFlamePos[i], (s16)scale, 0, 0, -1);
    }

    Audio_PlayActorSound2(&p->actor, FIRE_ROD_SFX_FLAMETHROWER);
    Audio_PlayActorSound2(&p->actor, FIRE_ROD_SFX_FIRE_LOOP);
}

static void FireRod_UpdateFlamethrower(Player* p, PlayState* play) {
    if (!fireRodFlameActive) return;

    if (fireRodFlameTimer > 0) {
        fireRodFlameTimer--;

        // Update colliders for all flames
        // EffectSsEnFire visual radius is roughly scale * 0.12, height ~scale * 0.2
        for (s32 i = 0; i < FIRE_ROD_FLAME_COUNT; i++) {
            f32 scale = FIRE_ROD_FLAME_BASE_SCALE + (i * FIRE_ROD_FLAME_SCALE_GROW);
            fireRodFlameColliders[i].dim.radius = (s16)(scale * 0.12f + 3.0f);
            fireRodFlameColliders[i].dim.height = (s16)(scale * 0.2f + 5.0f);
            fireRodFlameColliders[i].dim.pos.x = (s16)fireRodFlamePos[i].x;
            fireRodFlameColliders[i].dim.pos.y = (s16)fireRodFlamePos[i].y;
            fireRodFlameColliders[i].dim.pos.z = (s16)fireRodFlamePos[i].z;
            CollisionCheck_SetAT(play, &play->colChkCtx, &fireRodFlameColliders[i].base);
        }
    } else {
        fireRodFlameActive = 0;
    }
}

// =============================================================================
// ATTACK EFFECTS
// =============================================================================

// Slash: 3 fireballs spread at short range
static void FireRod_SlashEffect(Player* p, PlayState* play) {
    if (FireRod_CheckBackfire(p, play, FIRE_ROD_MAGIC_SLASH, FIRE_ROD_BACKFIRE_SLASH)) return;
    ItemMagic_Consume(play, FIRE_ROD_MAGIC_SLASH);

    Vec3f* tipPos = &p->meleeWeaponInfo[0].tip;
    s16 baseYaw, pitch;

    if (Player_IsZTargeting(p) && p->focusActor != NULL && p->focusActor->update != NULL) {
        Vec3f* targetPos = &p->focusActor->focus.pos;
        baseYaw = Math_Vec3f_Yaw(tipPos, targetPos);
        pitch = Math_Vec3f_Pitch(tipPos, targetPos);
    } else {
        baseYaw = p->actor.shape.rot.y;
        pitch = 0;
    }

    FireRod_InitTripleProjectile(p, play, tipPos, baseYaw, pitch);
    Audio_PlayActorSound2(&p->actor, FIRE_ROD_SFX_SWING);
}

// Stab: Single fireball at long range - uses weapon direction from base to tip
static void FireRod_StabEffect(Player* p, PlayState* play) {
    if (FireRod_CheckBackfire(p, play, FIRE_ROD_MAGIC_STAB, FIRE_ROD_BACKFIRE_SLASH)) return;
    ItemMagic_Consume(play, FIRE_ROD_MAGIC_STAB);

    Vec3f* tipPos = &p->meleeWeaponInfo[0].tip;
    Vec3f* basePos = &p->meleeWeaponInfo[0].base;
    s16 yaw, pitch;

    if (Player_IsZTargeting(p) && p->focusActor != NULL && p->focusActor->update != NULL) {
        Vec3f* targetPos = &p->focusActor->focus.pos;
        yaw = Math_Vec3f_Yaw(tipPos, targetPos);
        pitch = Math_Vec3f_Pitch(tipPos, targetPos);
    } else {
        // Use weapon direction (base to tip) for stab direction
        yaw = Math_Vec3f_Yaw(basePos, tipPos);
        pitch = Math_Vec3f_Pitch(basePos, tipPos);
    }

    FireRod_InitSingleProjectile(p, play, tipPos, yaw, pitch, FIRE_ROD_PROJ_LIFETIME * FIRE_ROD_PROJ_SPEED);
    Audio_PlayActorSound2(&p->actor, FIRE_ROD_SFX_SWING);
}

// Jump Slash: Flamethrower cone
static void FireRod_JumpEffect(Player* p, PlayState* play) {
    if (FireRod_CheckBackfire(p, play, FIRE_ROD_MAGIC_JUMP, FIRE_ROD_BACKFIRE_JUMP)) return;
    ItemMagic_Consume(play, FIRE_ROD_MAGIC_JUMP);
    FireRod_StartFlamethrower(p, play);
}

// First Person: Fires stab in aimed direction
static void FireRod_FirstPersonFire(Player* p, PlayState* play) {
    if (FireRod_CheckBackfire(p, play, FIRE_ROD_MAGIC_STAB, FIRE_ROD_BACKFIRE_SLASH)) return;
    ItemMagic_Consume(play, FIRE_ROD_MAGIC_STAB);

    s16 aimYaw = FirstPerson_GetAimYaw(p);
    s16 aimPitch = FirstPerson_GetAimPitch(p);

    Vec3f startPos;
    startPos.x = p->actor.world.pos.x + 30.0f * Math_SinS(aimYaw);
    startPos.y = p->actor.world.pos.y + 40.0f;
    startPos.z = p->actor.world.pos.z + 30.0f * Math_CosS(aimYaw);

    FireRod_InitSingleProjectile(p, play, &startPos, aimYaw, aimPitch, FIRE_ROD_PROJ_LIFETIME * FIRE_ROD_PROJ_SPEED);
    Audio_PlayActorSound2(&p->actor, FIRE_ROD_SFX_SWING);
}

// =============================================================================
// SWING PROCESSING
// =============================================================================

static void FireRod_SwingParticles(Player* p, PlayState* play) {
    Vec3f* tipPos = &p->meleeWeaponInfo[0].tip;
    Vec3f* basePos = &p->meleeWeaponInfo[0].base;

    if ((play->gameplayFrames % 2) == 0) {
        FX_SpawnRodSwingParticles(play, tipPos, &sFireRodColor);
    }

    if (fireRodBlureIdx >= 0) {
        FX_AddSwordTrailVertex(fireRodBlureIdx, basePos, tipPos);
    }

    if ((play->gameplayFrames % 8) == 0) {
        Audio_PlayActorSound2(&p->actor, FIRE_ROD_SFX_FIRE_IGNITE);
    }
}

static u8 FireRod_IsSpinAttack(u8 mwa) {
    return (mwa == PLAYER_MWA_SPIN_ATTACK_1H || mwa == PLAYER_MWA_SPIN_ATTACK_2H ||
            mwa == PLAYER_MWA_BIG_SPIN_1H || mwa == PLAYER_MWA_BIG_SPIN_2H);
}

// =============================================================================
// SPIN FIRE CYLINDER
// =============================================================================

static void FireRod_StartSpinFire(Player* p, PlayState* play, u8 isBigSpin) {
    if (!sSpinColliderInited) {
        Collider_InitCylinder(play, &fireRodSpinCollider);
        Collider_SetCylinder(play, &fireRodSpinCollider, &p->actor, &sFireRodSpinColInit);
        sSpinColliderInited = 1;
    }

    fireRodSpinActive = 1;
    fireRodSpinIsBig = isBigSpin;
    fireRodSpinRadius = 50.0f;
    fireRodSpinMaxRadius = isBigSpin ? FIRE_ROD_SPIN_BIG_RADIUS : FIRE_ROD_SPIN_SMALL_RADIUS;
    sSpinExpandProgress = 0.0f;
    Audio_PlayActorSound2(&p->actor, FIRE_ROD_SFX_FIRE_CAST);
}

static void FireRod_UpdateSpinFire(Player* p, PlayState* play) {
    if (!fireRodSpinActive) return;

    f32 expandSpeed = fireRodSpinIsBig ? 30.0f : 15.0f;
    fireRodSpinRadius += expandSpeed;
    if (fireRodSpinRadius >= fireRodSpinMaxRadius) fireRodSpinRadius = fireRodSpinMaxRadius;

    sSpinExpandProgress = fireRodSpinRadius / fireRodSpinMaxRadius;
    if (sSpinExpandProgress > 1.0f) sSpinExpandProgress = 1.0f;

    fireRodSpinCollider.dim.radius = (s16)fireRodSpinRadius;
    fireRodSpinCollider.dim.height = 80;
    fireRodSpinCollider.dim.pos.x = (s16)p->actor.world.pos.x;
    fireRodSpinCollider.dim.pos.y = (s16)p->actor.world.pos.y;
    fireRodSpinCollider.dim.pos.z = (s16)p->actor.world.pos.z;

    CollisionCheck_SetAT(play, &play->colChkCtx, &fireRodSpinCollider.base);
    FX_DrawSpinFireCylinder(play, p, fireRodSpinRadius, fireRodSpinIsBig, &sFireRodColor);

    if ((play->gameplayFrames % 6) == 0) {
        Audio_PlayActorSound2(&p->actor, FIRE_ROD_SFX_FIRE_IGNITE);
    }
}

static void FireRod_StopSpinFire(void) {
    fireRodSpinActive = 0;
    fireRodSpinRadius = 0.0f;
    sSpinExpandProgress = 0.0f;
}

static void FireRod_ProcessSwing(Player* p, PlayState* play) {
    u8 mwa = p->meleeWeaponAnimation;

    FireRod_SwingParticles(p, play);

    if (FireRod_IsSpinAttack(mwa)) {
        if (!fireRodSpinActive) {
            u8 isBigSpin = (mwa == PLAYER_MWA_BIG_SPIN_1H || mwa == PLAYER_MWA_BIG_SPIN_2H);
            FireRod_StartSpinFire(p, play, isBigSpin);
        }
        FireRod_UpdateSpinFire(p, play);
    } else {
        if (fireRodSpinActive) FireRod_StopSpinFire();
    }

    if (mwa == sLastSwingType) return;
    sLastSwingType = mwa;

    switch (mwa) {
        case PLAYER_MWA_FORWARD_SLASH_1H:
        case PLAYER_MWA_FORWARD_SLASH_2H:
        case PLAYER_MWA_FORWARD_COMBO_1H:
        case PLAYER_MWA_FORWARD_COMBO_2H:
        case PLAYER_MWA_RIGHT_SLASH_1H:
        case PLAYER_MWA_RIGHT_SLASH_2H:
        case PLAYER_MWA_RIGHT_COMBO_1H:
        case PLAYER_MWA_RIGHT_COMBO_2H:
        case PLAYER_MWA_LEFT_SLASH_1H:
        case PLAYER_MWA_LEFT_SLASH_2H:
        case PLAYER_MWA_LEFT_COMBO_1H:
        case PLAYER_MWA_LEFT_COMBO_2H:
            FireRod_SlashEffect(p, play);
            break;

        case PLAYER_MWA_STAB_1H:
        case PLAYER_MWA_STAB_2H:
        case PLAYER_MWA_STAB_COMBO_1H:
        case PLAYER_MWA_STAB_COMBO_2H:
            FireRod_StabEffect(p, play);
            break;

        case PLAYER_MWA_FLIPSLASH_START:
        case PLAYER_MWA_JUMPSLASH_START:
            sJumpEffectSpawned = 0;
            break;

        case PLAYER_MWA_FLIPSLASH_FINISH:
        case PLAYER_MWA_JUMPSLASH_FINISH:
            if (!sJumpEffectSpawned) {
                FireRod_JumpEffect(p, play);
                sJumpEffectSpawned = 1;
            }
            break;

        default:
            break;
    }
}

// =============================================================================
// CHARGE ATTACK
// =============================================================================

static u8 FireRod_CanCharge(Player* p, PlayState* play) {
    if (p->meleeWeaponState > 0) return 0;
    if (p->stateFlags1 & (PLAYER_STATE1_DEAD | PLAYER_STATE1_IN_CUTSCENE |
                          PLAYER_STATE1_DAMAGED | PLAYER_STATE1_LOADING |
                          PLAYER_STATE1_HOOKSHOT_FALLING)) return 0;
    if (!(p->actor.bgCheckFlags & 1)) return 0;
    if (p->stateFlags2 & PLAYER_STATE2_HOPPING) return 0;
    return 1;
}

static void FireRod_StartCharge(Player* p, PlayState* play) {
    fireRodCharging = 1;
    fireRodChargeLevel = 0.0f;
    fireRodChargeReady = 0;
    fireRodChargeTimer = 0;
    fireRodState = FIRE_ROD_STATE_CHARGING;
    Audio_PlayActorSound2(&p->actor, FIRE_ROD_SFX_CHARGE);
}

static void FireRod_UpdateCharge(Player* p, PlayState* play) {
    if (!fireRodCharging) return;

    fireRodChargeTimer++;

    if (fireRodChargeLevel < 1.0f) {
        fireRodChargeLevel += FIRE_ROD_CHARGE_RATE;
        if (fireRodChargeLevel > 1.0f) fireRodChargeLevel = 1.0f;
    }

    if (!fireRodChargeReady && fireRodChargeLevel >= FIRE_ROD_CHARGE_MIN) {
        fireRodChargeReady = 1;
        Audio_PlayActorSound2(&p->actor, FIRE_ROD_SFX_CHARGE);
    }

    if (fireRodChargeLevel >= FIRE_ROD_CHARGE_BIG &&
        fireRodChargeTimer == (s16)(FIRE_ROD_CHARGE_BIG / FIRE_ROD_CHARGE_RATE)) {
        Audio_PlayActorSound2(&p->actor, FIRE_ROD_SFX_CHARGE);
    }

    FX_DrawChargeAura(play, p, fireRodChargeLevel, &sFireRodColor);

    if ((play->gameplayFrames % 3) == 0) {
        Vec3f* tipPos = &p->meleeWeaponInfo[0].tip;
        FX_SpawnRodSwingParticles(play, tipPos, &sFireRodColor);
    }

    if ((play->gameplayFrames % 12) == 0) {
        Audio_PlayActorSound2(&p->actor, FIRE_ROD_SFX_FIRE_IGNITE);
    }
}

static void FireRod_ReleaseCharge(Player* p, PlayState* play) {
    if (!fireRodCharging) return;

    s32 spinType;
    u8 isBigSpin = 0;
    s16 magicCost;

    if (fireRodChargeLevel >= FIRE_ROD_CHARGE_BIG) {
        spinType = PLAYER_MWA_BIG_SPIN_1H;
        magicCost = FIRE_ROD_MAGIC_SPIN_BIG;
        isBigSpin = 1;
    } else if (fireRodChargeLevel >= FIRE_ROD_CHARGE_MIN) {
        spinType = PLAYER_MWA_SPIN_ATTACK_1H;
        magicCost = FIRE_ROD_MAGIC_SPIN_SMALL;
    } else {
        fireRodCharging = 0;
        fireRodChargeLevel = 0.0f;
        fireRodChargeReady = 0;
        fireRodState = FIRE_ROD_STATE_EQUIPPED;
        return;
    }

    if (FireRod_CheckBackfire(p, play, magicCost, FIRE_ROD_BACKFIRE_SPIN)) {
        fireRodCharging = 0;
        fireRodChargeLevel = 0.0f;
        fireRodChargeReady = 0;
        fireRodState = FIRE_ROD_STATE_EQUIPPED;
        return;
    }

    ItemMagic_Consume(play, magicCost);
    func_80837948(play, p, spinType);

    fireRodCharging = 0;
    fireRodChargeLevel = 0.0f;
    fireRodChargeReady = 0;
    fireRodState = FIRE_ROD_STATE_SWINGING;
    Audio_PlayActorSound2(&p->actor, FIRE_ROD_SFX_SWING);
}

static void FireRod_CancelCharge(Player* p) {
    fireRodCharging = 0;
    fireRodChargeLevel = 0.0f;
    fireRodChargeReady = 0;
    fireRodState = FIRE_ROD_STATE_EQUIPPED;
    sChargeButtonHeld = 0;
    sChargeHoldCounter = 0;
}

// =============================================================================
// FIRST PERSON MODE - Toggle with C-UP, exit on other buttons
// =============================================================================

static void FireRod_EnterFirstPerson(Player* p, PlayState* play) {
    FirstPerson_Init(p, play);
    fireRodFirstPerson = 1;
    fireRodState = FIRE_ROD_STATE_AIMING;
}

static void FireRod_ExitFirstPerson(Player* p, PlayState* play) {
    FirstPerson_Exit(p, play);
    fireRodFirstPerson = 0;
    fireRodState = FIRE_ROD_STATE_EQUIPPED;
}

static void FireRod_UpdateFirstPerson(Player* p, PlayState* play, ItemInputState* in) {
    FirstPerson_Update(p, play);

    // Fire on equipped C-button press
    if (in->isPressed) {
        FireRod_FirstPersonFire(p, play);
    }

    // Toggle off with C-UP
    if (CHECK_BTN_ALL(play->state.input[0].press.button, BTN_CUP)) {
        FireRod_ExitFirstPerson(p, play);
        return;
    }

    // Exit on B, other C-buttons, or damage/cutscene
    u16 exitButtons = BTN_B | BTN_CLEFT | BTN_CRIGHT | BTN_CDOWN;
    if (in->equippedButton) exitButtons &= ~in->equippedButton;

    if (CHECK_BTN_ANY(play->state.input[0].press.button, exitButtons) ||
        (p->stateFlags1 & (PLAYER_STATE1_DEAD | PLAYER_STATE1_IN_CUTSCENE | PLAYER_STATE1_DAMAGED))) {
        FireRod_ExitFirstPerson(p, play);
    }
}

// =============================================================================
// EQUIP/UNEQUIP
// =============================================================================

static void FireRod_OnEquip(PlayState* play, Player* p) {
    fireRodActive = 1;
    fireRodState = FIRE_ROD_STATE_EQUIPPED;
    sLastSwingType = 0;
    sJumpEffectSpawned = 0;
    fireRodProjActive = 0;
    fireRodProjCount = 0;
    fireRodFirstPerson = 0;
    fireRodFlameActive = 0;

    fireRodCharging = 0;
    fireRodChargeLevel = 0.0f;
    fireRodChargeReady = 0;
    fireRodChargeTimer = 0;
    sChargeButtonHeld = 0;
    sChargeHoldCounter = 0;

    fireRodBlureIdx = FX_InitSwordTrail(play, &sFireRodColor);
    ItemEquip_PlayEquipSFX(play, p);
}

static void FireRod_OnUnequip(PlayState* play, Player* p) {
    if (fireRodFirstPerson) FireRod_ExitFirstPerson(p, play);

    fireRodActive = 0;
    fireRodState = FIRE_ROD_STATE_INACTIVE;
    sLastSwingType = 0;
    fireRodProjActive = 0;
    fireRodProjCount = 0;
    fireRodFlameActive = 0;

    fireRodCharging = 0;
    fireRodChargeLevel = 0.0f;
    fireRodChargeReady = 0;
    fireRodChargeTimer = 0;
    sChargeButtonHeld = 0;
    sChargeHoldCounter = 0;

    if (fireRodBlureIdx >= 0) {
        FX_KillSwordTrail(play, fireRodBlureIdx);
        fireRodBlureIdx = -1;
    }

    if (fireRodSpinActive) FireRod_StopSpinFire();
    ItemEquip_PlayUnequipSFX(play, p);
}

// =============================================================================
// MAIN HANDLER
// =============================================================================

void Handle_FireRod(Player* p, PlayState* play) {
    FireRod_UpdateProjectile(p, play);
    FireRod_UpdateFlamethrower(p, play);

    ItemInputState in;
    ItemInput_Update(&in, ITEM_ROD_FIRE, p, play);
    fireRodButtonMask = in.equippedButton;

    if (!in.wasEquipped) {
        if (fireRodActive) FireRod_OnUnequip(play, p);
        sEquipState.isEquipped = 0;
        return;
    }

    // C-UP toggles first person mode
    if (fireRodActive && !fireRodFirstPerson && !fireRodCharging && p->meleeWeaponState == 0) {
        if (CHECK_BTN_ALL(play->state.input[0].press.button, BTN_CUP)) {
            FireRod_EnterFirstPerson(p, play);
            return;
        }
    }

    if (fireRodFirstPerson) {
        FireRod_UpdateFirstPerson(p, play, &in);
        return;
    }

    if (!fireRodActive) {
        if (ItemInput_IsBlockedEx(p, play, 1)) return;
    } else {
        u32 criticalBlocks = (PLAYER_STATE1_DEAD | PLAYER_STATE1_IN_CUTSCENE | PLAYER_STATE1_LOADING |
            PLAYER_STATE1_IN_ITEM_CS | PLAYER_STATE1_TALKING | PLAYER_STATE1_GETTING_ITEM |
            PLAYER_STATE1_DAMAGED | PLAYER_STATE1_HANGING_OFF_LEDGE | PLAYER_STATE1_CLIMBING_LEDGE |
            PLAYER_STATE1_CLIMBING_LADDER | PLAYER_STATE1_ON_HORSE | PLAYER_STATE1_HOOKSHOT_FALLING);
        if (p->stateFlags1 & criticalBlocks) {
            if (fireRodCharging) FireRod_CancelCharge(p);
            return;
        }
    }

    if (ItemInput_CheckDamage(p, &sPrevInvinc)) {
        if (fireRodCharging) FireRod_CancelCharge(p);
        if (fireRodActive) FireRod_OnUnequip(play, p);
        sEquipState.isEquipped = 0;
        return;
    }

    ItemEquip_Update(&sEquipState, &in, FireRod_OnEquip, FireRod_OnUnequip, p, play);

    if (!fireRodActive) return;

    if (fireRodCharging) {
        if (in.isHeld) FireRod_UpdateCharge(p, play);
        else FireRod_ReleaseCharge(p, play);
    } else if (in.isHeld && FireRod_CanCharge(p, play)) {
        if (!sChargeButtonHeld) {
            sChargeButtonHeld = 1;
            sChargeHoldCounter = 0;
        }
        sChargeHoldCounter++;
        if (sChargeHoldCounter >= FIRE_ROD_CHARGE_HOLD_FRAMES) {
            FireRod_StartCharge(p, play);
        }
    } else {
        sChargeButtonHeld = 0;
        sChargeHoldCounter = 0;
    }

    if (p->meleeWeaponState > 0) {
        fireRodState = FIRE_ROD_STATE_SWINGING;
        FireRod_ProcessSwing(p, play);
        if (fireRodCharging) FireRod_CancelCharge(p);
    } else {
        if (!fireRodCharging) {
            fireRodState = FIRE_ROD_STATE_EQUIPPED;
            sLastSwingType = 0;
        }
        if (fireRodSpinActive) FireRod_StopSpinFire();
    }
}

// =============================================================================
// INIT
// =============================================================================

void Player_InitFireRodIA(PlayState* play, Player* p) {
    fireRodActive = 1;
    fireRodState = FIRE_ROD_STATE_EQUIPPED;
    sLastSwingType = 0;
    sJumpEffectSpawned = 0;
    fireRodProjActive = 0;
    fireRodProjCount = 0;
    fireRodBlureIdx = -1;
    fireRodFirstPerson = 0;
    fireRodButtonMask = 0;
    fireRodFlameActive = 0;

    fireRodCharging = 0;
    fireRodChargeLevel = 0.0f;
    fireRodChargeReady = 0;
    fireRodChargeTimer = 0;
    sChargeButtonHeld = 0;
    sChargeHoldCounter = 0;

    FireRod_InitColliders(p, play);
    FireRod_InitFlameColliders(p, play);

    if (!sSpinColliderInited) {
        Collider_InitCylinder(play, &fireRodSpinCollider);
        Collider_SetCylinder(play, &fireRodSpinCollider, &p->actor, &sFireRodSpinColInit);
        sSpinColliderInited = 1;
    }

    fireRodSpinActive = 0;
    fireRodSpinRadius = 0.0f;
    sSpinExpandProgress = 0.0f;

    fireRodBlureIdx = FX_InitSwordTrail(play, &sFireRodColor);
}

void CustomItems_DrawFireRodReticle(Player* p, PlayState* play) {
    if (!fireRodFirstPerson || fireRodState != FIRE_ROD_STATE_AIMING) return;
    FirstPerson_DrawReticle(p, play, 0.0f, FIRE_ROD_RETICLE_R, FIRE_ROD_RETICLE_G, FIRE_ROD_RETICLE_B);
}
