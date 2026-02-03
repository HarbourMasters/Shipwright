/**
 * item_rod_light.c - Light Rod (custom item)
 *
 * Controls:
 *   B Button:  Swing rod (uses sword mechanics)
 *   C-UP:      Toggle first-person aiming mode
 *
 * Attack Types:
 *   - Slash: 3 light balls spread at +30/0/-30 degrees
 *   - Stab: Single long-range light ball
 *   - Jump Slash: Light beam (6 balls, increasing size)
 *   - Spin Attack: Expanding light cylinder (area stun)
 *
 * Special: Stuns/paralyzes enemies, backfire electrocutes Link
 */

#include "item_rod_light.h"
#include "../helpers/equip_helper.h"
#include "../helpers/combat_helper.h"
#include "../helpers/fx_helper.h"
#include "../helpers/camera_helper.h"

static ItemEquipState sLightEquipState = { 0 };
static s8 sLightPrevInvinc = 0;
static u8 sLightLastSwingType = 0;
static u8 sLightJumpEffectSpawned = 0;
static u8 sLightChargeButtonHeld = 0;
static s16 sLightChargeHoldCounter = 0;
static f32 sLightRodTargetScale = 2.0f;
static f32 sLightSpinExpandProgress = 0.0f;
static u8 sLightSpinColliderInited = 0;
static u8 sLightBeamCollidersInited = 0;
static u8 sLightMultiCollidersInited = 0;

static RodColor sLightRodColor = {
    LIGHT_ROD_PRIM_R, LIGHT_ROD_PRIM_G, LIGHT_ROD_PRIM_B, LIGHT_ROD_PRIM_A,
    LIGHT_ROD_ENV_R, LIGHT_ROD_ENV_G, LIGHT_ROD_ENV_B, LIGHT_ROD_ENV_A
};

extern int Player_IsZTargeting(Player* this);
extern void func_80837948(PlayState* play, Player* player, s32 meleeWeaponAnim);

// Aliases for multi-projectile system
#define lightRodProjPos2     gCustomItemState.lightRodProjPos2
#define lightRodProjPos3     gCustomItemState.lightRodProjPos3
#define lightRodProjVel      gCustomItemState.lightRodProjVel
#define lightRodProjCount    gCustomItemState.lightRodProjCount
#define lightRodCollider2    gCustomItemState.lightRodCollider2
#define lightRodCollider3    gCustomItemState.lightRodCollider3
#define lightRodBeamActive   gCustomItemState.lightRodBeamActive
#define lightRodBeamTimer    gCustomItemState.lightRodBeamTimer
#define lightRodBeamPos      gCustomItemState.lightRodBeamPos
#define lightRodBeamColliders gCustomItemState.lightRodBeamColliders

// =============================================================================
// BACKFIRE - Electrocutes Link when using light rod without magic
// =============================================================================

// func_80837C0C: Handles player hit response including electric shock
extern void func_80837C0C(PlayState* play, Player* this, s32 hitResponse, f32 damageSpeed, f32 damageRot, s16 damageRotType, s32 invincibility);

// Helper function to spawn golden/yellow KiraKira sparkles (like Fire Rod but yellow)
static void LightRod_SpawnKiraKira(PlayState* play, Vec3f* pos, f32 spread, s32 count) {
    Color_RGBA8 primColor = { LIGHT_ROD_PRIM_R, LIGHT_ROD_PRIM_G, LIGHT_ROD_PRIM_B, LIGHT_ROD_PRIM_A };
    Color_RGBA8 envColor = { LIGHT_ROD_ENV_R, LIGHT_ROD_ENV_G, LIGHT_ROD_ENV_B, LIGHT_ROD_ENV_A };
    Vec3f vel = { 0.0f, 0.0f, 0.0f };
    Vec3f accel = { 0.0f, 0.0f, 0.0f };

    for (s32 i = 0; i < count; i++) {
        Vec3f sparkPos;
        sparkPos.x = pos->x + Rand_CenteredFloat(spread);
        sparkPos.y = pos->y + Rand_CenteredFloat(spread);
        sparkPos.z = pos->z + Rand_CenteredFloat(spread);
        EffectSsKiraKira_SpawnDispersed(play, &sparkPos, &vel, &accel, &primColor, &envColor, 1200, 12);
    }
}

static void LightRod_Backfire(Player* p, PlayState* play) {
    Audio_PlayActorSound2(&p->actor, LIGHT_ROD_SFX_BACKFIRE_HIT);
    Audio_PlayActorSound2(&p->actor, NA_SE_VO_LI_DAMAGE_S);

    // Electrocute Link using the electric shock response (PLAYER_HIT_RESPONSE_ELECTRIC_SHOCK = 4)
    func_80837C0C(play, p, 4, 0.0f, 0.0f, 0, 20);

    // Spawn golden sparkles on backfire
    Vec3f shockPos = p->actor.world.pos;
    shockPos.y += 50.0f;
    LightRod_SpawnKiraKira(play, &shockPos, 40.0f, 15);
}

static u8 LightRod_CheckBackfire(Player* p, PlayState* play, s16 magicCost, u8 backfireChance) {
    if (ItemMagic_HasEnough(play, magicCost)) return 0;

    u8 roll = (u8)(Rand_ZeroOne() * 100.0f);
    if (roll < backfireChance) {
        LightRod_Backfire(p, play);
        return 1;
    }

    Sfx_PlaySfxCentered(LIGHT_ROD_SFX_NO_MAGIC);
    return 1;
}

// =============================================================================
// MULTI-PROJECTILE SYSTEM - Supports 1-3 simultaneous light balls
// =============================================================================

static void LightRod_InitColliders(Player* p, PlayState* play) {
    if (sLightMultiCollidersInited) return;

    Collider_InitCylinder(play, &lightRodCollider);
    Collider_SetCylinder(play, &lightRodCollider, &p->actor, &sLightRodProjColInit);
    Collider_InitCylinder(play, &lightRodCollider2);
    Collider_SetCylinder(play, &lightRodCollider2, &p->actor, &sLightRodProjColInit);
    Collider_InitCylinder(play, &lightRodCollider3);
    Collider_SetCylinder(play, &lightRodCollider3, &p->actor, &sLightRodProjColInit);
    sLightMultiCollidersInited = 1;
}

static void LightRod_CalcVelocity(Vec3f* outVel, s16 yaw, s16 pitch) {
    Vec3f localVel = { 0.0f, 0.0f, LIGHT_ROD_PROJ_SPEED };
    Matrix_Push();
    Matrix_RotateY(BINANG_TO_RAD(yaw), MTXMODE_NEW);
    Matrix_RotateX(BINANG_TO_RAD(pitch), MTXMODE_APPLY);
    Matrix_MultVec3f(&localVel, outVel);
    Matrix_Pop();
}

// Spawns single projectile (stab, first-person)
static void LightRod_InitSingleProjectile(Player* p, PlayState* play, Vec3f* startPos, s16 yaw, s16 pitch, f32 maxRange) {
    LightRod_InitColliders(p, play);

    sLightRodTargetScale = 2.0f;
    lightRodProjActive = 1;
    lightRodProjCount = 1;
    lightRodProjPos = *startPos;
    lightRodProjTimer = (s16)(maxRange / LIGHT_ROD_PROJ_SPEED);
    if (lightRodProjTimer < 10) lightRodProjTimer = 10;
    if (lightRodProjTimer > 25) lightRodProjTimer = 25;

    lightRodProjScale = 0.0f;
    lightRodProjRotZ = 0;
    lightRodProjTrailIdx = 0;
    for (s32 i = 0; i < 6; i++) lightRodProjTrail[i] = *startPos;

    lightRodProjYaw = yaw;
    lightRodProjPitch = pitch;
    LightRod_CalcVelocity(&lightRodProjVel[0], yaw, pitch);
}

// Spawns 3 light balls spread at configurable angles (slash attack)
static void LightRod_InitTripleProjectile(Player* p, PlayState* play, Vec3f* startPos, s16 baseYaw, s16 pitch) {
    LightRod_InitColliders(p, play);

    sLightRodTargetScale = 2.0f;
    lightRodProjActive = 1;
    lightRodProjCount = 3;

    s16 spreadAngle = (s16)(LIGHT_ROD_SLASH_SPREAD * (0x10000 / 360));
    s16 timer = (s16)(LIGHT_ROD_SLASH_RANGE / LIGHT_ROD_PROJ_SPEED);
    if (timer < 10) timer = 10;
    lightRodProjTimer = timer;

    lightRodProjScale = 0.0f;
    lightRodProjRotZ = 0;
    lightRodProjTrailIdx = 0;

    // Center light ball
    lightRodProjPos = *startPos;
    lightRodProjYaw = baseYaw;
    lightRodProjPitch = pitch;
    LightRod_CalcVelocity(&lightRodProjVel[0], baseYaw, pitch);
    for (s32 i = 0; i < 6; i++) lightRodProjTrail[i] = *startPos;

    // Left light ball (-spread angle)
    lightRodProjPos2 = *startPos;
    LightRod_CalcVelocity(&lightRodProjVel[1], baseYaw - spreadAngle, pitch);

    // Right light ball (+spread angle)
    lightRodProjPos3 = *startPos;
    LightRod_CalcVelocity(&lightRodProjVel[2], baseYaw + spreadAngle, pitch);
}

static void LightRod_UpdateCollider(ColliderCylinder* col, Vec3f* pos, f32 scale, PlayState* play) {
    col->dim.radius = (s16)(scale * 1.5f + 2.0f);
    col->dim.height = (s16)(scale * 2.0f + 3.0f);
    col->dim.pos.x = (s16)pos->x;
    col->dim.pos.y = (s16)pos->y;
    col->dim.pos.z = (s16)pos->z;
    CollisionCheck_SetAT(play, &play->colChkCtx, &col->base);
}

static u8 LightRod_CheckHit(ColliderCylinder* col, Vec3f* pos, PlayState* play, Player* p) {
    if (col->base.atFlags & AT_HIT) {
        // Spawn golden sparkle burst on hit
        LightRod_SpawnKiraKira(play, pos, 30.0f, 20);
        Audio_PlayActorSound2(&p->actor, LIGHT_ROD_SFX_LIGHT_EXPLODE);
        col->base.atFlags &= ~AT_HIT;
        return 1;
    }
    return 0;
}

static void LightRod_SpawnLightSparks(PlayState* play, Vec3f* pos, f32 scale) {
    // Use KiraKira sparkles for projectile trail (like Fire Rod pattern)
    s32 count = (s32)(scale * 3.0f);
    if (count < 2) count = 2;
    if (count > 8) count = 8;
    LightRod_SpawnKiraKira(play, pos, scale * 10.0f, count);
}

static void LightRod_UpdateProjectile(Player* p, PlayState* play) {
    if (!lightRodProjActive) return;

    lightRodProjRotZ += 6000;

    if (lightRodProjTimer > 0) lightRodProjTimer--;
    if (lightRodProjTimer == 0) sLightRodTargetScale = 0.0f;

    Math_ApproachF(&lightRodProjScale, sLightRodTargetScale, 0.2f, 0.5f);

    if (lightRodProjTimer == 0 && lightRodProjScale < 0.1f) {
        lightRodProjActive = 0;
        sLightRodTargetScale = 2.0f;
        return;
    }

    // Update all active projectiles
    lightRodProjPos.x += lightRodProjVel[0].x;
    lightRodProjPos.y += lightRodProjVel[0].y;
    lightRodProjPos.z += lightRodProjVel[0].z;

    if (lightRodProjCount >= 2) {
        lightRodProjPos2.x += lightRodProjVel[1].x;
        lightRodProjPos2.y += lightRodProjVel[1].y;
        lightRodProjPos2.z += lightRodProjVel[1].z;
    }
    if (lightRodProjCount >= 3) {
        lightRodProjPos3.x += lightRodProjVel[2].x;
        lightRodProjPos3.y += lightRodProjVel[2].y;
        lightRodProjPos3.z += lightRodProjVel[2].z;
    }

    // Trail for center projectile
    lightRodProjTrail[0] = lightRodProjPos;
    for (s32 i = 4; i >= 0; i--) lightRodProjTrail[i + 1] = lightRodProjTrail[i];

    // Sparks and sound - throttle particle spawning like Dominion Rod does
    if (lightRodProjScale >= 0.4f) {
        // Only spawn particles every 3 frames to avoid overwhelming the effect system
        if ((play->gameplayFrames % 3) == 0) {
            LightRod_SpawnLightSparks(play, &lightRodProjPos, lightRodProjScale);
            if (lightRodProjCount >= 2) LightRod_SpawnLightSparks(play, &lightRodProjPos2, lightRodProjScale);
            if (lightRodProjCount >= 3) LightRod_SpawnLightSparks(play, &lightRodProjPos3, lightRodProjScale);
        }
        Audio_PlayActorSound2(&p->actor, LIGHT_ROD_SFX_LIGHT_LOOP - SFX_FLAG);
    }

    // Colliders
    if (lightRodProjScale >= 0.6f) {
        LightRod_UpdateCollider(&lightRodCollider, &lightRodProjPos, lightRodProjScale, play);
        if (lightRodProjCount >= 2) LightRod_UpdateCollider(&lightRodCollider2, &lightRodProjPos2, lightRodProjScale, play);
        if (lightRodProjCount >= 3) LightRod_UpdateCollider(&lightRodCollider3, &lightRodProjPos3, lightRodProjScale, play);
    }

    // Hit detection
    u8 anyHit = LightRod_CheckHit(&lightRodCollider, &lightRodProjPos, play, p);
    if (lightRodProjCount >= 2) anyHit |= LightRod_CheckHit(&lightRodCollider2, &lightRodProjPos2, play, p);
    if (lightRodProjCount >= 3) anyHit |= LightRod_CheckHit(&lightRodCollider3, &lightRodProjPos3, play, p);

    if (anyHit) {
        lightRodProjVel[0].x = lightRodProjVel[0].y = lightRodProjVel[0].z = 0.0f;
        lightRodProjVel[1].x = lightRodProjVel[1].y = lightRodProjVel[1].z = 0.0f;
        lightRodProjVel[2].x = lightRodProjVel[2].y = lightRodProjVel[2].z = 0.0f;
        lightRodProjTimer = 0;
        sLightRodTargetScale = 0.0f;
    }
}

// =============================================================================
// LIGHT BEAM SYSTEM - 6 light balls with individual colliders (jump slash)
// Uses KiraKira sparkles for visual effect (same as Fire Rod pattern)
// =============================================================================

// KiraKira sparkle effect for jump attack - golden/yellow light sparkles
static void LightRod_SpawnJumpSparkles(PlayState* play, Vec3f* pos, f32 scale) {
    Color_RGBA8 primColor = { LIGHT_ROD_PRIM_R, LIGHT_ROD_PRIM_G, LIGHT_ROD_PRIM_B, LIGHT_ROD_PRIM_A };
    Color_RGBA8 envColor = { LIGHT_ROD_ENV_R, LIGHT_ROD_ENV_G, LIGHT_ROD_ENV_B, LIGHT_ROD_ENV_A };
    Vec3f vel = { 0.0f, 1.0f, 0.0f };
    Vec3f accel = { 0.0f, -0.05f, 0.0f };

    s32 count = (s32)(scale * 0.05f);
    if (count < 3) count = 3;
    if (count > 15) count = 15;

    for (s32 i = 0; i < count; i++) {
        Vec3f sparkPos;
        sparkPos.x = pos->x + Rand_CenteredFloat(20.0f);
        sparkPos.y = pos->y + Rand_CenteredFloat(15.0f);
        sparkPos.z = pos->z + Rand_CenteredFloat(20.0f);

        Vec3f sparkVel = vel;
        sparkVel.x = Rand_CenteredFloat(2.0f);
        sparkVel.z = Rand_CenteredFloat(2.0f);

        EffectSsKiraKira_SpawnDispersed(play, &sparkPos, &sparkVel, &accel, &primColor, &envColor, 1500, 15);
    }
}

static void LightRod_InitBeamColliders(Player* p, PlayState* play) {
    if (sLightBeamCollidersInited) return;

    for (s32 i = 0; i < LIGHT_ROD_BEAM_COUNT; i++) {
        Collider_InitCylinder(play, &lightRodBeamColliders[i]);
        Collider_SetCylinder(play, &lightRodBeamColliders[i], &p->actor, &sLightRodBeamColInit);
    }
    sLightBeamCollidersInited = 1;
}

static void LightRod_StartLightBeam(Player* p, PlayState* play) {
    LightRod_InitBeamColliders(p, play);

    Vec3f impactPos = p->actor.world.pos;
    impactPos.y = p->actor.floorHeight + 5.0f;
    s16 playerYaw = p->actor.shape.rot.y;

    lightRodBeamActive = 1;
    lightRodBeamTimer = 30;

    // Position light balls in a line going forward from Link
    for (s32 i = 0; i < LIGHT_ROD_BEAM_COUNT; i++) {
        f32 dist = (i + 1) * LIGHT_ROD_BEAM_SPACING;
        lightRodBeamPos[i].x = impactPos.x + dist * Math_SinS(playerYaw);
        lightRodBeamPos[i].y = impactPos.y + 20.0f;
        lightRodBeamPos[i].z = impactPos.z + dist * Math_CosS(playerYaw);

        // Spawn KiraKira sparkles at each position (golden light effect)
        f32 scale = (LIGHT_ROD_BEAM_BASE_SCALE + (i * LIGHT_ROD_BEAM_SCALE_GROW));
        LightRod_SpawnJumpSparkles(play, &lightRodBeamPos[i], scale);
    }

    Audio_PlayActorSound2(&p->actor, LIGHT_ROD_SFX_LIGHTBEAM);
    Audio_PlayActorSound2(&p->actor, LIGHT_ROD_SFX_LIGHT_LOOP);
}

static void LightRod_UpdateLightBeam(Player* p, PlayState* play) {
    if (!lightRodBeamActive) return;

    if (lightRodBeamTimer > 0) {
        lightRodBeamTimer--;

        // Update colliders for all light balls
        for (s32 i = 0; i < LIGHT_ROD_BEAM_COUNT; i++) {
            f32 scale = LIGHT_ROD_BEAM_BASE_SCALE + (i * LIGHT_ROD_BEAM_SCALE_GROW);
            lightRodBeamColliders[i].dim.radius = (s16)(scale * 0.12f + 3.0f);
            lightRodBeamColliders[i].dim.height = (s16)(scale * 0.2f + 5.0f);
            lightRodBeamColliders[i].dim.pos.x = (s16)lightRodBeamPos[i].x;
            lightRodBeamColliders[i].dim.pos.y = (s16)lightRodBeamPos[i].y;
            lightRodBeamColliders[i].dim.pos.z = (s16)lightRodBeamPos[i].z;
            CollisionCheck_SetAT(play, &play->colChkCtx, &lightRodBeamColliders[i].base);
        }

        // Spawn KiraKira sparkles while beam is active (golden light effect)
        if ((play->gameplayFrames % 4) == 0) {
            for (s32 i = 0; i < LIGHT_ROD_BEAM_COUNT; i++) {
                f32 scale = (LIGHT_ROD_BEAM_BASE_SCALE + (i * LIGHT_ROD_BEAM_SCALE_GROW)) * 0.5f;
                LightRod_SpawnJumpSparkles(play, &lightRodBeamPos[i], scale);
            }
        }
    } else {
        lightRodBeamActive = 0;
    }
}

// =============================================================================
// ATTACK EFFECTS
// =============================================================================

// Slash: 3 light balls spread at short range
static void LightRod_SlashEffect(Player* p, PlayState* play) {
    if (LightRod_CheckBackfire(p, play, LIGHT_ROD_MAGIC_SLASH, LIGHT_ROD_BACKFIRE_SLASH)) return;
    ItemMagic_Consume(play, LIGHT_ROD_MAGIC_SLASH);

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

    LightRod_InitTripleProjectile(p, play, tipPos, baseYaw, pitch);
    Audio_PlayActorSound2(&p->actor, LIGHT_ROD_SFX_SWING);
}

// Stab: Single light ball at long range
static void LightRod_StabEffect(Player* p, PlayState* play) {
    if (LightRod_CheckBackfire(p, play, LIGHT_ROD_MAGIC_STAB, LIGHT_ROD_BACKFIRE_SLASH)) return;
    ItemMagic_Consume(play, LIGHT_ROD_MAGIC_STAB);

    Vec3f* tipPos = &p->meleeWeaponInfo[0].tip;
    Vec3f* basePos = &p->meleeWeaponInfo[0].base;
    s16 yaw, pitch;

    if (Player_IsZTargeting(p) && p->focusActor != NULL && p->focusActor->update != NULL) {
        Vec3f* targetPos = &p->focusActor->focus.pos;
        yaw = Math_Vec3f_Yaw(tipPos, targetPos);
        pitch = Math_Vec3f_Pitch(tipPos, targetPos);
    } else {
        yaw = Math_Vec3f_Yaw(basePos, tipPos);
        pitch = Math_Vec3f_Pitch(basePos, tipPos);
    }

    LightRod_InitSingleProjectile(p, play, tipPos, yaw, pitch, LIGHT_ROD_PROJ_LIFETIME * LIGHT_ROD_PROJ_SPEED);
    Audio_PlayActorSound2(&p->actor, LIGHT_ROD_SFX_SWING);
}

// Jump Slash: Light beam
static void LightRod_JumpEffect(Player* p, PlayState* play) {
    if (LightRod_CheckBackfire(p, play, LIGHT_ROD_MAGIC_JUMP, LIGHT_ROD_BACKFIRE_JUMP)) return;
    ItemMagic_Consume(play, LIGHT_ROD_MAGIC_JUMP);
    LightRod_StartLightBeam(p, play);
}

// First Person: Fires stab in aimed direction
static void LightRod_FirstPersonFire(Player* p, PlayState* play) {
    if (LightRod_CheckBackfire(p, play, LIGHT_ROD_MAGIC_STAB, LIGHT_ROD_BACKFIRE_SLASH)) return;
    ItemMagic_Consume(play, LIGHT_ROD_MAGIC_STAB);

    s16 aimYaw = FirstPerson_GetAimYaw(p);
    s16 aimPitch = FirstPerson_GetAimPitch(p);

    Vec3f startPos;
    startPos.x = p->actor.world.pos.x + 30.0f * Math_SinS(aimYaw);
    startPos.y = p->actor.world.pos.y + 40.0f;
    startPos.z = p->actor.world.pos.z + 30.0f * Math_CosS(aimYaw);

    LightRod_InitSingleProjectile(p, play, &startPos, aimYaw, aimPitch, LIGHT_ROD_PROJ_LIFETIME * LIGHT_ROD_PROJ_SPEED);
    Audio_PlayActorSound2(&p->actor, LIGHT_ROD_SFX_SWING);
}

// =============================================================================
// SWING PROCESSING
// =============================================================================

static void LightRod_SwingParticles(Player* p, PlayState* play) {
    Vec3f* tipPos = &p->meleeWeaponInfo[0].tip;
    Vec3f* basePos = &p->meleeWeaponInfo[0].base;

    if ((play->gameplayFrames % 2) == 0) {
        FX_SpawnRodSwingParticles(play, tipPos, &sLightRodColor);
    }

    if (lightRodBlureIdx >= 0) {
        FX_AddSwordTrailVertex(lightRodBlureIdx, basePos, tipPos);
    }

    if ((play->gameplayFrames % 8) == 0) {
        Audio_PlayActorSound2(&p->actor, LIGHT_ROD_SFX_LIGHT_IGNITE);
    }
}

static u8 LightRod_IsSpinAttack(u8 mwa) {
    return (mwa == PLAYER_MWA_SPIN_ATTACK_1H || mwa == PLAYER_MWA_SPIN_ATTACK_2H ||
            mwa == PLAYER_MWA_BIG_SPIN_1H || mwa == PLAYER_MWA_BIG_SPIN_2H);
}

// =============================================================================
// SPIN LIGHT CYLINDER (Paralyzes all enemies)
// =============================================================================

static void LightRod_StartSpinLight(Player* p, PlayState* play, u8 isBigSpin) {
    if (!sLightSpinColliderInited) {
        Collider_InitCylinder(play, &lightRodSpinCollider);
        Collider_SetCylinder(play, &lightRodSpinCollider, &p->actor, &sLightRodSpinColInit);
        sLightSpinColliderInited = 1;
    }

    lightRodSpinActive = 1;
    lightRodSpinIsBig = isBigSpin;
    lightRodSpinRadius = 50.0f;
    lightRodSpinMaxRadius = isBigSpin ? LIGHT_ROD_SPIN_BIG_RADIUS : LIGHT_ROD_SPIN_SMALL_RADIUS;
    sLightSpinExpandProgress = 0.0f;
    Audio_PlayActorSound2(&p->actor, LIGHT_ROD_SFX_LIGHT_CAST);
}

static void LightRod_UpdateSpinLight(Player* p, PlayState* play) {
    if (!lightRodSpinActive) return;

    f32 expandSpeed = lightRodSpinIsBig ? 30.0f : 15.0f;
    lightRodSpinRadius += expandSpeed;
    if (lightRodSpinRadius >= lightRodSpinMaxRadius) lightRodSpinRadius = lightRodSpinMaxRadius;

    sLightSpinExpandProgress = lightRodSpinRadius / lightRodSpinMaxRadius;
    if (sLightSpinExpandProgress > 1.0f) sLightSpinExpandProgress = 1.0f;

    lightRodSpinCollider.dim.radius = (s16)lightRodSpinRadius;
    lightRodSpinCollider.dim.height = 80;
    lightRodSpinCollider.dim.pos.x = (s16)p->actor.world.pos.x;
    lightRodSpinCollider.dim.pos.y = (s16)p->actor.world.pos.y;
    lightRodSpinCollider.dim.pos.z = (s16)p->actor.world.pos.z;

    CollisionCheck_SetAT(play, &play->colChkCtx, &lightRodSpinCollider.base);
    FX_DrawSpinFireCylinder(play, p, lightRodSpinRadius, lightRodSpinIsBig, &sLightRodColor);

    if ((play->gameplayFrames % 6) == 0) {
        Audio_PlayActorSound2(&p->actor, LIGHT_ROD_SFX_LIGHT_IGNITE);
    }
}

static void LightRod_StopSpinLight(void) {
    lightRodSpinActive = 0;
    lightRodSpinRadius = 0.0f;
    sLightSpinExpandProgress = 0.0f;
}

static void LightRod_ProcessSwing(Player* p, PlayState* play) {
    u8 mwa = p->meleeWeaponAnimation;

    LightRod_SwingParticles(p, play);

    if (LightRod_IsSpinAttack(mwa)) {
        if (!lightRodSpinActive) {
            u8 isBigSpin = (mwa == PLAYER_MWA_BIG_SPIN_1H || mwa == PLAYER_MWA_BIG_SPIN_2H);
            LightRod_StartSpinLight(p, play, isBigSpin);
        }
        LightRod_UpdateSpinLight(p, play);
    } else {
        if (lightRodSpinActive) LightRod_StopSpinLight();
    }

    if (mwa == sLightLastSwingType) return;
    sLightLastSwingType = mwa;

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
            LightRod_SlashEffect(p, play);
            break;

        case PLAYER_MWA_STAB_1H:
        case PLAYER_MWA_STAB_2H:
        case PLAYER_MWA_STAB_COMBO_1H:
        case PLAYER_MWA_STAB_COMBO_2H:
            LightRod_StabEffect(p, play);
            break;

        case PLAYER_MWA_FLIPSLASH_START:
        case PLAYER_MWA_JUMPSLASH_START:
            sLightJumpEffectSpawned = 0;
            break;

        case PLAYER_MWA_FLIPSLASH_FINISH:
        case PLAYER_MWA_JUMPSLASH_FINISH:
            if (!sLightJumpEffectSpawned) {
                LightRod_JumpEffect(p, play);
                sLightJumpEffectSpawned = 1;
            }
            break;

        default:
            break;
    }
}

// =============================================================================
// CHARGE ATTACK
// =============================================================================

static u8 LightRod_CanCharge(Player* p, PlayState* play) {
    if (p->meleeWeaponState > 0) return 0;
    if (p->stateFlags1 & (PLAYER_STATE1_DEAD | PLAYER_STATE1_IN_CUTSCENE |
                          PLAYER_STATE1_DAMAGED | PLAYER_STATE1_LOADING |
                          PLAYER_STATE1_HOOKSHOT_FALLING)) return 0;
    if (!(p->actor.bgCheckFlags & 1)) return 0;
    if (p->stateFlags2 & PLAYER_STATE2_HOPPING) return 0;
    return 1;
}

static void LightRod_StartCharge(Player* p, PlayState* play) {
    lightRodCharging = 1;
    lightRodChargeLevel = 0.0f;
    lightRodChargeReady = 0;
    lightRodChargeTimer = 0;
    lightRodState = LIGHT_ROD_STATE_CHARGING;
    Audio_PlayActorSound2(&p->actor, LIGHT_ROD_SFX_CHARGE);
}

static void LightRod_UpdateCharge(Player* p, PlayState* play) {
    if (!lightRodCharging) return;

    lightRodChargeTimer++;

    if (lightRodChargeLevel < 1.0f) {
        lightRodChargeLevel += LIGHT_ROD_CHARGE_RATE;
        if (lightRodChargeLevel > 1.0f) lightRodChargeLevel = 1.0f;
    }

    if (!lightRodChargeReady && lightRodChargeLevel >= LIGHT_ROD_CHARGE_MIN) {
        lightRodChargeReady = 1;
        Audio_PlayActorSound2(&p->actor, LIGHT_ROD_SFX_CHARGE);
    }

    if (lightRodChargeLevel >= LIGHT_ROD_CHARGE_BIG &&
        lightRodChargeTimer == (s16)(LIGHT_ROD_CHARGE_BIG / LIGHT_ROD_CHARGE_RATE)) {
        Audio_PlayActorSound2(&p->actor, LIGHT_ROD_SFX_CHARGE);
    }

    // Use bright yellow (255, 255, 0) when at max charge, otherwise use default color
    RodColor chargeColor;
    if (lightRodChargeLevel >= LIGHT_ROD_CHARGE_BIG) {
        // Bright yellow for max charge - intense light magic
        chargeColor.primR = 255; chargeColor.primG = 255; chargeColor.primB = 0; chargeColor.primA = 255;
        chargeColor.envR = 255; chargeColor.envG = 255; chargeColor.envB = 100; chargeColor.envA = 255;
    } else {
        chargeColor = sLightRodColor;
    }
    FX_DrawChargeAura(play, p, lightRodChargeLevel, &chargeColor);

    if ((play->gameplayFrames % 3) == 0) {
        Vec3f* tipPos = &p->meleeWeaponInfo[0].tip;
        FX_SpawnRodSwingParticles(play, tipPos, &sLightRodColor);
    }

    if ((play->gameplayFrames % 12) == 0) {
        Audio_PlayActorSound2(&p->actor, LIGHT_ROD_SFX_LIGHT_IGNITE);
    }
}

static void LightRod_ReleaseCharge(Player* p, PlayState* play) {
    if (!lightRodCharging) return;

    s32 spinType;
    u8 isBigSpin = 0;
    s16 magicCost;

    if (lightRodChargeLevel >= LIGHT_ROD_CHARGE_BIG) {
        spinType = PLAYER_MWA_BIG_SPIN_1H;
        magicCost = LIGHT_ROD_MAGIC_SPIN_BIG;
        isBigSpin = 1;
    } else if (lightRodChargeLevel >= LIGHT_ROD_CHARGE_MIN) {
        spinType = PLAYER_MWA_SPIN_ATTACK_1H;
        magicCost = LIGHT_ROD_MAGIC_SPIN_SMALL;
    } else {
        lightRodCharging = 0;
        lightRodChargeLevel = 0.0f;
        lightRodChargeReady = 0;
        lightRodState = LIGHT_ROD_STATE_EQUIPPED;
        return;
    }

    if (LightRod_CheckBackfire(p, play, magicCost, LIGHT_ROD_BACKFIRE_SPIN)) {
        lightRodCharging = 0;
        lightRodChargeLevel = 0.0f;
        lightRodChargeReady = 0;
        lightRodState = LIGHT_ROD_STATE_EQUIPPED;
        return;
    }

    ItemMagic_Consume(play, magicCost);
    func_80837948(play, p, spinType);

    lightRodCharging = 0;
    lightRodChargeLevel = 0.0f;
    lightRodChargeReady = 0;
    lightRodState = LIGHT_ROD_STATE_SWINGING;
    Audio_PlayActorSound2(&p->actor, LIGHT_ROD_SFX_SWING);
}

static void LightRod_CancelCharge(Player* p) {
    lightRodCharging = 0;
    lightRodChargeLevel = 0.0f;
    lightRodChargeReady = 0;
    lightRodState = LIGHT_ROD_STATE_EQUIPPED;
    sLightChargeButtonHeld = 0;
    sLightChargeHoldCounter = 0;
}

// =============================================================================
// FIRST PERSON MODE
// =============================================================================

static void LightRod_EnterFirstPerson(Player* p, PlayState* play) {
    FirstPerson_Init(p, play);
    lightRodFirstPerson = 1;
    lightRodState = LIGHT_ROD_STATE_AIMING;
}

static void LightRod_ExitFirstPerson(Player* p, PlayState* play) {
    FirstPerson_Exit(p, play);
    lightRodFirstPerson = 0;
    lightRodState = LIGHT_ROD_STATE_EQUIPPED;
}

static void LightRod_UpdateFirstPerson(Player* p, PlayState* play, ItemInputState* in) {
    FirstPerson_Update(p, play);

    if (in->isPressed) {
        LightRod_FirstPersonFire(p, play);
    }

    if (CHECK_BTN_ALL(play->state.input[0].press.button, BTN_CUP)) {
        LightRod_ExitFirstPerson(p, play);
        return;
    }

    u16 exitButtons = BTN_B | BTN_CLEFT | BTN_CRIGHT | BTN_CDOWN;
    if (in->equippedButton) exitButtons &= ~in->equippedButton;

    if (CHECK_BTN_ANY(play->state.input[0].press.button, exitButtons) ||
        (p->stateFlags1 & (PLAYER_STATE1_DEAD | PLAYER_STATE1_IN_CUTSCENE | PLAYER_STATE1_DAMAGED))) {
        LightRod_ExitFirstPerson(p, play);
    }
}

// =============================================================================
// EQUIP/UNEQUIP
// =============================================================================

static void LightRod_OnEquip(PlayState* play, Player* p) {
    lightRodActive = 1;
    lightRodState = LIGHT_ROD_STATE_EQUIPPED;
    sLightLastSwingType = 0;
    sLightJumpEffectSpawned = 0;
    lightRodProjActive = 0;
    lightRodProjCount = 0;
    lightRodFirstPerson = 0;
    lightRodBeamActive = 0;

    lightRodCharging = 0;
    lightRodChargeLevel = 0.0f;
    lightRodChargeReady = 0;
    lightRodChargeTimer = 0;
    sLightChargeButtonHeld = 0;
    sLightChargeHoldCounter = 0;

    lightRodBlureIdx = FX_InitSwordTrail(play, &sLightRodColor);
    ItemEquip_PlayEquipSFX(play, p);
}

static void LightRod_OnUnequip(PlayState* play, Player* p) {
    if (lightRodFirstPerson) LightRod_ExitFirstPerson(p, play);

    lightRodActive = 0;
    lightRodState = LIGHT_ROD_STATE_INACTIVE;
    sLightLastSwingType = 0;
    lightRodProjActive = 0;
    lightRodProjCount = 0;
    lightRodBeamActive = 0;

    lightRodCharging = 0;
    lightRodChargeLevel = 0.0f;
    lightRodChargeReady = 0;
    lightRodChargeTimer = 0;
    sLightChargeButtonHeld = 0;
    sLightChargeHoldCounter = 0;

    if (lightRodBlureIdx >= 0) {
        FX_KillSwordTrail(play, lightRodBlureIdx);
        lightRodBlureIdx = -1;
    }

    if (lightRodSpinActive) LightRod_StopSpinLight();
    ItemEquip_PlayUnequipSFX(play, p);
}

// =============================================================================
// MAIN HANDLER
// =============================================================================

void Handle_LightRod(Player* p, PlayState* play) {
    LightRod_UpdateProjectile(p, play);
    LightRod_UpdateLightBeam(p, play);

    ItemInputState in;
    ItemInput_Update(&in, ITEM_ROD_LIGHT, p, play);
    lightRodButtonMask = in.equippedButton;

    if (!in.wasEquipped) {
        if (lightRodActive) LightRod_OnUnequip(play, p);
        sLightEquipState.isEquipped = 0;
        return;
    }

    // C-UP toggles first person mode
    if (lightRodActive && !lightRodFirstPerson && !lightRodCharging && p->meleeWeaponState == 0) {
        if (CHECK_BTN_ALL(play->state.input[0].press.button, BTN_CUP)) {
            LightRod_EnterFirstPerson(p, play);
            return;
        }
    }

    if (lightRodFirstPerson) {
        LightRod_UpdateFirstPerson(p, play, &in);
        return;
    }

    if (!lightRodActive) {
        if (ItemInput_IsBlockedEx(p, play, 1)) return;
    } else {
        u32 criticalBlocks = (PLAYER_STATE1_DEAD | PLAYER_STATE1_IN_CUTSCENE | PLAYER_STATE1_LOADING |
            PLAYER_STATE1_IN_ITEM_CS | PLAYER_STATE1_TALKING | PLAYER_STATE1_GETTING_ITEM |
            PLAYER_STATE1_DAMAGED | PLAYER_STATE1_HANGING_OFF_LEDGE | PLAYER_STATE1_CLIMBING_LEDGE |
            PLAYER_STATE1_CLIMBING_LADDER | PLAYER_STATE1_ON_HORSE | PLAYER_STATE1_HOOKSHOT_FALLING);
        if (p->stateFlags1 & criticalBlocks) {
            if (lightRodCharging) LightRod_CancelCharge(p);
            return;
        }
    }

    if (ItemInput_CheckDamage(p, &sLightPrevInvinc)) {
        if (lightRodCharging) LightRod_CancelCharge(p);
        if (lightRodActive) LightRod_OnUnequip(play, p);
        sLightEquipState.isEquipped = 0;
        return;
    }

    ItemEquip_Update(&sLightEquipState, &in, LightRod_OnEquip, LightRod_OnUnequip, p, play);

    if (!lightRodActive) return;

    if (lightRodCharging) {
        if (in.isHeld) LightRod_UpdateCharge(p, play);
        else LightRod_ReleaseCharge(p, play);
    } else if (in.isHeld && LightRod_CanCharge(p, play)) {
        if (!sLightChargeButtonHeld) {
            sLightChargeButtonHeld = 1;
            sLightChargeHoldCounter = 0;
        }
        sLightChargeHoldCounter++;
        if (sLightChargeHoldCounter >= LIGHT_ROD_CHARGE_HOLD_FRAMES) {
            LightRod_StartCharge(p, play);
        }
    } else {
        sLightChargeButtonHeld = 0;
        sLightChargeHoldCounter = 0;
    }

    if (p->meleeWeaponState > 0) {
        lightRodState = LIGHT_ROD_STATE_SWINGING;
        LightRod_ProcessSwing(p, play);
        if (lightRodCharging) LightRod_CancelCharge(p);
    } else {
        if (!lightRodCharging) {
            lightRodState = LIGHT_ROD_STATE_EQUIPPED;
            sLightLastSwingType = 0;
        }
        if (lightRodSpinActive) LightRod_StopSpinLight();
    }
}

// =============================================================================
// INIT
// =============================================================================

void Player_InitLightRodIA(PlayState* play, Player* p) {
    lightRodActive = 1;
    lightRodState = LIGHT_ROD_STATE_EQUIPPED;
    sLightLastSwingType = 0;
    sLightJumpEffectSpawned = 0;
    lightRodProjActive = 0;
    lightRodProjCount = 0;
    lightRodBlureIdx = -1;
    lightRodFirstPerson = 0;
    lightRodButtonMask = 0;
    lightRodBeamActive = 0;

    lightRodCharging = 0;
    lightRodChargeLevel = 0.0f;
    lightRodChargeReady = 0;
    lightRodChargeTimer = 0;
    sLightChargeButtonHeld = 0;
    sLightChargeHoldCounter = 0;

    LightRod_InitColliders(p, play);
    LightRod_InitBeamColliders(p, play);

    if (!sLightSpinColliderInited) {
        Collider_InitCylinder(play, &lightRodSpinCollider);
        Collider_SetCylinder(play, &lightRodSpinCollider, &p->actor, &sLightRodSpinColInit);
        sLightSpinColliderInited = 1;
    }

    lightRodSpinActive = 0;
    lightRodSpinRadius = 0.0f;
    sLightSpinExpandProgress = 0.0f;

    lightRodBlureIdx = FX_InitSwordTrail(play, &sLightRodColor);
}

void CustomItems_DrawLightRodReticle(Player* p, PlayState* play) {
    if (!lightRodFirstPerson || lightRodState != LIGHT_ROD_STATE_AIMING) return;
    FirstPerson_DrawReticle(p, play, 0.0f, LIGHT_ROD_RETICLE_R, LIGHT_ROD_RETICLE_G, LIGHT_ROD_RETICLE_B);
}
