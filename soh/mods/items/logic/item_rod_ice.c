/**
 * item_rod_ice.c - Ice Rod from A Link Between Worlds
 *
 * Controls:
 *   B Button:  Swing rod (uses sword mechanics)
 *   C-UP:      Toggle first-person aiming mode
 *
 * Attack Types:
 *   - Slash: 3 iceballs spread at +30/0/-30 degrees
 *   - Stab: Single long-range iceball
 *   - Jump Slash: Ice wave cone (6 colliders)
 *   - Spin Attack: Expanding ice cylinder
 *
 * Special: Freezes enemies on hit, backfire freezes Link
 */

#include "item_rod_ice.h"
#include "../helpers/equip_helper.h"
#include "../helpers/combat_helper.h"
#include "../helpers/fx_helper.h"
#include "../helpers/camera_helper.h"

static ItemEquipState sIceEquipState = { 0 };
static s8 sIcePrevInvinc = 0;
static u8 sIceLastSwingType = 0;
static u8 sIceJumpEffectSpawned = 0;
static u8 sIceChargeButtonHeld = 0;
static s16 sIceChargeHoldCounter = 0;
static f32 sIceRodTargetScale = 2.0f;
static f32 sIceSpinExpandProgress = 0.0f;
static u8 sIceSpinColliderInited = 0;
static u8 sIceWaveCollidersInited = 0;
static u8 sIceMultiCollidersInited = 0;

static RodColor sIceRodColor = {
    ICE_ROD_PRIM_R, ICE_ROD_PRIM_G, ICE_ROD_PRIM_B, ICE_ROD_PRIM_A,
    ICE_ROD_ENV_R, ICE_ROD_ENV_G, ICE_ROD_ENV_B, ICE_ROD_ENV_A
};

extern int Player_IsZTargeting(Player* this);
extern void func_80837948(PlayState* play, Player* player, s32 meleeWeaponAnim);

// Aliases for multi-projectile system
#define iceRodProjPos2     gCustomItemState.iceRodProjPos2
#define iceRodProjPos3     gCustomItemState.iceRodProjPos3
#define iceRodProjVel      gCustomItemState.iceRodProjVel
#define iceRodProjCount    gCustomItemState.iceRodProjCount
#define iceRodCollider2    gCustomItemState.iceRodCollider2
#define iceRodCollider3    gCustomItemState.iceRodCollider3
#define iceRodWaveActive   gCustomItemState.iceRodWaveActive
#define iceRodWaveTimer    gCustomItemState.iceRodWaveTimer
#define iceRodWavePos      gCustomItemState.iceRodWavePos
#define iceRodWaveColliders gCustomItemState.iceRodWaveColliders

// =============================================================================
// BACKFIRE - Freezes Link when using ice rod without magic
// =============================================================================

// func_80837C0C: Handles player hit response including freeze
extern void func_80837C0C(PlayState* play, Player* this, s32 hitResponse, f32 damageSpeed, f32 damageRot, s16 damageRotType, s32 invincibility);

static void IceRod_Backfire(Player* p, PlayState* play) {
    Audio_PlayActorSound2(&p->actor, ICE_ROD_SFX_BACKFIRE_HIT);
    Audio_PlayActorSound2(&p->actor, NA_SE_VO_LI_FREEZE);

    // Freeze Link using the ice trap response (PLAYER_HIT_RESPONSE_ICE_TRAP = 3)
    func_80837C0C(play, p, 3, 0.0f, 0.0f, 0, 20);
}

static u8 IceRod_CheckBackfire(Player* p, PlayState* play, s16 magicCost, u8 backfireChance) {
    if (ItemMagic_HasEnough(play, magicCost)) return 0;

    u8 roll = (u8)(Rand_ZeroOne() * 100.0f);
    if (roll < backfireChance) {
        IceRod_Backfire(p, play);
        return 1;
    }

    Sfx_PlaySfxCentered(ICE_ROD_SFX_NO_MAGIC);
    return 1;
}

// =============================================================================
// MULTI-PROJECTILE SYSTEM - Supports 1-3 simultaneous iceballs
// =============================================================================

static void IceRod_InitColliders(Player* p, PlayState* play) {
    if (sIceMultiCollidersInited) return;

    Collider_InitCylinder(play, &iceRodCollider);
    Collider_SetCylinder(play, &iceRodCollider, &p->actor, &sIceRodProjColInit);
    Collider_InitCylinder(play, &iceRodCollider2);
    Collider_SetCylinder(play, &iceRodCollider2, &p->actor, &sIceRodProjColInit);
    Collider_InitCylinder(play, &iceRodCollider3);
    Collider_SetCylinder(play, &iceRodCollider3, &p->actor, &sIceRodProjColInit);
    sIceMultiCollidersInited = 1;
}

static void IceRod_CalcVelocity(Vec3f* outVel, s16 yaw, s16 pitch) {
    Vec3f localVel = { 0.0f, 0.0f, ICE_ROD_PROJ_SPEED };
    Matrix_Push();
    Matrix_RotateY(BINANG_TO_RAD(yaw), MTXMODE_NEW);
    Matrix_RotateX(BINANG_TO_RAD(pitch), MTXMODE_APPLY);
    Matrix_MultVec3f(&localVel, outVel);
    Matrix_Pop();
}

// Spawns single projectile (stab, first-person)
static void IceRod_InitSingleProjectile(Player* p, PlayState* play, Vec3f* startPos, s16 yaw, s16 pitch, f32 maxRange) {
    IceRod_InitColliders(p, play);

    sIceRodTargetScale = 2.0f;
    iceRodProjActive = 1;
    iceRodProjCount = 1;
    iceRodProjPos = *startPos;
    iceRodProjTimer = (s16)(maxRange / ICE_ROD_PROJ_SPEED);
    if (iceRodProjTimer < 10) iceRodProjTimer = 10;
    if (iceRodProjTimer > 30) iceRodProjTimer = 30;

    iceRodProjScale = 0.0f;
    iceRodProjRotZ = 0;
    iceRodProjTrailIdx = 0;
    for (s32 i = 0; i < 6; i++) iceRodProjTrail[i] = *startPos;

    iceRodProjYaw = yaw;
    iceRodProjPitch = pitch;
    IceRod_CalcVelocity(&iceRodProjVel[0], yaw, pitch);
}

// Spawns 3 iceballs spread at configurable angles (slash attack)
static void IceRod_InitTripleProjectile(Player* p, PlayState* play, Vec3f* startPos, s16 baseYaw, s16 pitch) {
    IceRod_InitColliders(p, play);

    sIceRodTargetScale = 2.0f;
    iceRodProjActive = 1;
    iceRodProjCount = 3;

    s16 spreadAngle = (s16)(ICE_ROD_SLASH_SPREAD * (0x10000 / 360));
    s16 timer = (s16)(ICE_ROD_SLASH_RANGE / ICE_ROD_PROJ_SPEED);
    if (timer < 10) timer = 10;
    iceRodProjTimer = timer;

    iceRodProjScale = 0.0f;
    iceRodProjRotZ = 0;
    iceRodProjTrailIdx = 0;

    // Center iceball
    iceRodProjPos = *startPos;
    iceRodProjYaw = baseYaw;
    iceRodProjPitch = pitch;
    IceRod_CalcVelocity(&iceRodProjVel[0], baseYaw, pitch);
    for (s32 i = 0; i < 6; i++) iceRodProjTrail[i] = *startPos;

    // Left iceball (-spread angle)
    iceRodProjPos2 = *startPos;
    IceRod_CalcVelocity(&iceRodProjVel[1], baseYaw - spreadAngle, pitch);

    // Right iceball (+spread angle)
    iceRodProjPos3 = *startPos;
    IceRod_CalcVelocity(&iceRodProjVel[2], baseYaw + spreadAngle, pitch);
}

static void IceRod_UpdateCollider(ColliderCylinder* col, Vec3f* pos, f32 scale, PlayState* play) {
    col->dim.radius = (s16)(scale * 1.5f + 2.0f);
    col->dim.height = (s16)(scale * 2.0f + 3.0f);
    col->dim.pos.x = (s16)pos->x;
    col->dim.pos.y = (s16)pos->y;
    col->dim.pos.z = (s16)pos->z;
    CollisionCheck_SetAT(play, &play->colChkCtx, &col->base);
}

static u8 IceRod_CheckHit(ColliderCylinder* col, Vec3f* pos, PlayState* play, Player* p) {
    if (col->base.atFlags & AT_HIT) {
        // Spawn ice burst effect on hit
        EffectSsIcePiece_SpawnBurst(play, pos, 1.0f);
        Audio_PlayActorSound2(&p->actor, ICE_ROD_SFX_ICE_EXPLODE);
        col->base.atFlags &= ~AT_HIT;
        return 1;
    }
    return 0;
}

// EffectSsEnIce_Spawn: Creates ice clump effects
static void IceRod_SpawnIceSparks(PlayState* play, Vec3f* pos, f32 scale) {
    Color_RGBA8 primColor = { ICE_ROD_PRIM_R, ICE_ROD_PRIM_G, ICE_ROD_PRIM_B, ICE_ROD_PRIM_A };
    Color_RGBA8 envColor = { ICE_ROD_ENV_R, ICE_ROD_ENV_G, ICE_ROD_ENV_B, ICE_ROD_ENV_A };
    Vec3f vel = { 0.0f, 0.0f, 0.0f };
    Vec3f accel = { 0.0f, -0.5f, 0.0f };

    for (s32 i = 0; i < 6; i++) {
        Vec3f sparkPos;
        sparkPos.x = pos->x + (Rand_ZeroOne() - 0.5f) * (scale * 15.0f);
        sparkPos.y = pos->y + (Rand_ZeroOne() - 0.5f) * (scale * 15.0f);
        sparkPos.z = pos->z + (Rand_ZeroOne() - 0.5f) * (scale * 15.0f);
        vel.x = (Rand_ZeroOne() - 0.5f) * 3.0f;
        vel.y = Rand_ZeroOne() * 2.0f;
        vel.z = (Rand_ZeroOne() - 0.5f) * 3.0f;
        EffectSsEnIce_Spawn(play, &sparkPos, scale * 0.3f, &vel, &accel, &primColor, &envColor, 15);
    }
}

static void IceRod_UpdateProjectile(Player* p, PlayState* play) {
    if (!iceRodProjActive) return;

    iceRodProjRotZ += 5000;

    if (iceRodProjTimer > 0) iceRodProjTimer--;
    if (iceRodProjTimer == 0) sIceRodTargetScale = 0.0f;

    Math_ApproachF(&iceRodProjScale, sIceRodTargetScale, 0.2f, 0.4f);

    if (iceRodProjTimer == 0 && iceRodProjScale < 0.1f) {
        iceRodProjActive = 0;
        sIceRodTargetScale = 2.0f;
        return;
    }

    // Update all active projectiles
    iceRodProjPos.x += iceRodProjVel[0].x;
    iceRodProjPos.y += iceRodProjVel[0].y;
    iceRodProjPos.z += iceRodProjVel[0].z;

    if (iceRodProjCount >= 2) {
        iceRodProjPos2.x += iceRodProjVel[1].x;
        iceRodProjPos2.y += iceRodProjVel[1].y;
        iceRodProjPos2.z += iceRodProjVel[1].z;
    }
    if (iceRodProjCount >= 3) {
        iceRodProjPos3.x += iceRodProjVel[2].x;
        iceRodProjPos3.y += iceRodProjVel[2].y;
        iceRodProjPos3.z += iceRodProjVel[2].z;
    }

    // Trail for center projectile
    iceRodProjTrail[0] = iceRodProjPos;
    for (s32 i = 4; i >= 0; i--) iceRodProjTrail[i + 1] = iceRodProjTrail[i];

    // Sparks and sound
    if (iceRodProjScale >= 0.4f) {
        IceRod_SpawnIceSparks(play, &iceRodProjPos, iceRodProjScale);
        if (iceRodProjCount >= 2) IceRod_SpawnIceSparks(play, &iceRodProjPos2, iceRodProjScale);
        if (iceRodProjCount >= 3) IceRod_SpawnIceSparks(play, &iceRodProjPos3, iceRodProjScale);
        Audio_PlayActorSound2(&p->actor, ICE_ROD_SFX_ICE_LOOP - SFX_FLAG);
    }

    // Colliders
    if (iceRodProjScale >= 0.6f) {
        IceRod_UpdateCollider(&iceRodCollider, &iceRodProjPos, iceRodProjScale, play);
        if (iceRodProjCount >= 2) IceRod_UpdateCollider(&iceRodCollider2, &iceRodProjPos2, iceRodProjScale, play);
        if (iceRodProjCount >= 3) IceRod_UpdateCollider(&iceRodCollider3, &iceRodProjPos3, iceRodProjScale, play);
    }

    // Hit detection
    u8 anyHit = IceRod_CheckHit(&iceRodCollider, &iceRodProjPos, play, p);
    if (iceRodProjCount >= 2) anyHit |= IceRod_CheckHit(&iceRodCollider2, &iceRodProjPos2, play, p);
    if (iceRodProjCount >= 3) anyHit |= IceRod_CheckHit(&iceRodCollider3, &iceRodProjPos3, play, p);

    if (anyHit) {
        iceRodProjVel[0].x = iceRodProjVel[0].y = iceRodProjVel[0].z = 0.0f;
        iceRodProjVel[1].x = iceRodProjVel[1].y = iceRodProjVel[1].z = 0.0f;
        iceRodProjVel[2].x = iceRodProjVel[2].y = iceRodProjVel[2].z = 0.0f;
        iceRodProjTimer = 0;
        sIceRodTargetScale = 0.0f;
    }
}

// =============================================================================
// ICE WAVE SYSTEM - 6 ice effects with individual colliders (jump slash)
// =============================================================================

static void IceRod_InitWaveColliders(Player* p, PlayState* play) {
    if (sIceWaveCollidersInited) return;

    for (s32 i = 0; i < ICE_ROD_WAVE_COUNT; i++) {
        Collider_InitCylinder(play, &iceRodWaveColliders[i]);
        Collider_SetCylinder(play, &iceRodWaveColliders[i], &p->actor, &sIceRodWaveColInit);
    }
    sIceWaveCollidersInited = 1;
}

static void IceRod_StartIceWave(Player* p, PlayState* play) {
    IceRod_InitWaveColliders(p, play);

    Vec3f impactPos = p->actor.world.pos;
    impactPos.y = p->actor.floorHeight + 5.0f;
    s16 playerYaw = p->actor.shape.rot.y;

    iceRodWaveActive = 1;
    iceRodWaveTimer = 30;

    // Position ice effects in a line going forward from Link
    for (s32 i = 0; i < ICE_ROD_WAVE_COUNT; i++) {
        f32 dist = (i + 1) * ICE_ROD_WAVE_SPACING;
        iceRodWavePos[i].x = impactPos.x + dist * Math_SinS(playerYaw);
        iceRodWavePos[i].y = impactPos.y + 10.0f;
        iceRodWavePos[i].z = impactPos.z + dist * Math_CosS(playerYaw);

        // EffectSsIcePiece_SpawnBurst: Spawns ice burst at position
        f32 scale = (ICE_ROD_WAVE_BASE_SCALE + (i * ICE_ROD_WAVE_SCALE_GROW)) / 100.0f;
        EffectSsIcePiece_SpawnBurst(play, &iceRodWavePos[i], scale);
    }

    Audio_PlayActorSound2(&p->actor, ICE_ROD_SFX_ICEWAVE);
    Audio_PlayActorSound2(&p->actor, ICE_ROD_SFX_ICE_LOOP);
}

static void IceRod_UpdateIceWave(Player* p, PlayState* play) {
    if (!iceRodWaveActive) return;

    if (iceRodWaveTimer > 0) {
        iceRodWaveTimer--;

        // Update colliders for all ice effects
        for (s32 i = 0; i < ICE_ROD_WAVE_COUNT; i++) {
            f32 scale = ICE_ROD_WAVE_BASE_SCALE + (i * ICE_ROD_WAVE_SCALE_GROW);
            iceRodWaveColliders[i].dim.radius = (s16)(scale * 0.12f + 3.0f);
            iceRodWaveColliders[i].dim.height = (s16)(scale * 0.2f + 5.0f);
            iceRodWaveColliders[i].dim.pos.x = (s16)iceRodWavePos[i].x;
            iceRodWaveColliders[i].dim.pos.y = (s16)iceRodWavePos[i].y;
            iceRodWaveColliders[i].dim.pos.z = (s16)iceRodWavePos[i].z;
            CollisionCheck_SetAT(play, &play->colChkCtx, &iceRodWaveColliders[i].base);
        }
    } else {
        iceRodWaveActive = 0;
    }
}

// =============================================================================
// ATTACK EFFECTS
// =============================================================================

// Slash: 3 iceballs spread at short range
static void IceRod_SlashEffect(Player* p, PlayState* play) {
    if (IceRod_CheckBackfire(p, play, ICE_ROD_MAGIC_SLASH, ICE_ROD_BACKFIRE_SLASH)) return;
    ItemMagic_Consume(play, ICE_ROD_MAGIC_SLASH);

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

    IceRod_InitTripleProjectile(p, play, tipPos, baseYaw, pitch);
    Audio_PlayActorSound2(&p->actor, ICE_ROD_SFX_SWING);
}

// Stab: Single iceball at long range - uses weapon direction from base to tip
static void IceRod_StabEffect(Player* p, PlayState* play) {
    if (IceRod_CheckBackfire(p, play, ICE_ROD_MAGIC_STAB, ICE_ROD_BACKFIRE_SLASH)) return;
    ItemMagic_Consume(play, ICE_ROD_MAGIC_STAB);

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

    IceRod_InitSingleProjectile(p, play, tipPos, yaw, pitch, ICE_ROD_PROJ_LIFETIME * ICE_ROD_PROJ_SPEED);
    Audio_PlayActorSound2(&p->actor, ICE_ROD_SFX_SWING);
}

// Jump Slash: Ice wave cone
static void IceRod_JumpEffect(Player* p, PlayState* play) {
    if (IceRod_CheckBackfire(p, play, ICE_ROD_MAGIC_JUMP, ICE_ROD_BACKFIRE_JUMP)) return;
    ItemMagic_Consume(play, ICE_ROD_MAGIC_JUMP);
    IceRod_StartIceWave(p, play);
}

// First Person: Fires stab in aimed direction
static void IceRod_FirstPersonFire(Player* p, PlayState* play) {
    if (IceRod_CheckBackfire(p, play, ICE_ROD_MAGIC_STAB, ICE_ROD_BACKFIRE_SLASH)) return;
    ItemMagic_Consume(play, ICE_ROD_MAGIC_STAB);

    s16 aimYaw = FirstPerson_GetAimYaw(p);
    s16 aimPitch = FirstPerson_GetAimPitch(p);

    Vec3f startPos;
    startPos.x = p->actor.world.pos.x + 30.0f * Math_SinS(aimYaw);
    startPos.y = p->actor.world.pos.y + 40.0f;
    startPos.z = p->actor.world.pos.z + 30.0f * Math_CosS(aimYaw);

    IceRod_InitSingleProjectile(p, play, &startPos, aimYaw, aimPitch, ICE_ROD_PROJ_LIFETIME * ICE_ROD_PROJ_SPEED);
    Audio_PlayActorSound2(&p->actor, ICE_ROD_SFX_SWING);
}

// =============================================================================
// SWING PROCESSING
// =============================================================================

static void IceRod_SwingParticles(Player* p, PlayState* play) {
    Vec3f* tipPos = &p->meleeWeaponInfo[0].tip;
    Vec3f* basePos = &p->meleeWeaponInfo[0].base;

    if ((play->gameplayFrames % 2) == 0) {
        FX_SpawnRodSwingParticles(play, tipPos, &sIceRodColor);
    }

    if (iceRodBlureIdx >= 0) {
        FX_AddSwordTrailVertex(iceRodBlureIdx, basePos, tipPos);
    }

    if ((play->gameplayFrames % 8) == 0) {
        Audio_PlayActorSound2(&p->actor, ICE_ROD_SFX_ICE_IGNITE);
    }
}

static u8 IceRod_IsSpinAttack(u8 mwa) {
    return (mwa == PLAYER_MWA_SPIN_ATTACK_1H || mwa == PLAYER_MWA_SPIN_ATTACK_2H ||
            mwa == PLAYER_MWA_BIG_SPIN_1H || mwa == PLAYER_MWA_BIG_SPIN_2H);
}

// =============================================================================
// SPIN ICE CYLINDER
// =============================================================================

static void IceRod_StartSpinIce(Player* p, PlayState* play, u8 isBigSpin) {
    if (!sIceSpinColliderInited) {
        Collider_InitCylinder(play, &iceRodSpinCollider);
        Collider_SetCylinder(play, &iceRodSpinCollider, &p->actor, &sIceRodSpinColInit);
        sIceSpinColliderInited = 1;
    }

    iceRodSpinActive = 1;
    iceRodSpinIsBig = isBigSpin;
    iceRodSpinRadius = 50.0f;
    iceRodSpinMaxRadius = isBigSpin ? ICE_ROD_SPIN_BIG_RADIUS : ICE_ROD_SPIN_SMALL_RADIUS;
    sIceSpinExpandProgress = 0.0f;
    Audio_PlayActorSound2(&p->actor, ICE_ROD_SFX_ICE_CAST);
}

static void IceRod_UpdateSpinIce(Player* p, PlayState* play) {
    if (!iceRodSpinActive) return;

    f32 expandSpeed = iceRodSpinIsBig ? 30.0f : 15.0f;
    iceRodSpinRadius += expandSpeed;
    if (iceRodSpinRadius >= iceRodSpinMaxRadius) iceRodSpinRadius = iceRodSpinMaxRadius;

    sIceSpinExpandProgress = iceRodSpinRadius / iceRodSpinMaxRadius;
    if (sIceSpinExpandProgress > 1.0f) sIceSpinExpandProgress = 1.0f;

    iceRodSpinCollider.dim.radius = (s16)iceRodSpinRadius;
    iceRodSpinCollider.dim.height = 80;
    iceRodSpinCollider.dim.pos.x = (s16)p->actor.world.pos.x;
    iceRodSpinCollider.dim.pos.y = (s16)p->actor.world.pos.y;
    iceRodSpinCollider.dim.pos.z = (s16)p->actor.world.pos.z;

    CollisionCheck_SetAT(play, &play->colChkCtx, &iceRodSpinCollider.base);

    // Draw ice cylinder (blue to white based on progress)
    FX_DrawSpinFireCylinder(play, p, iceRodSpinRadius, iceRodSpinIsBig, &sIceRodColor);

    if ((play->gameplayFrames % 6) == 0) {
        Audio_PlayActorSound2(&p->actor, ICE_ROD_SFX_ICE_IGNITE);
    }
}

static void IceRod_StopSpinIce(void) {
    iceRodSpinActive = 0;
    iceRodSpinRadius = 0.0f;
    sIceSpinExpandProgress = 0.0f;
}

static void IceRod_ProcessSwing(Player* p, PlayState* play) {
    u8 mwa = p->meleeWeaponAnimation;

    IceRod_SwingParticles(p, play);

    if (IceRod_IsSpinAttack(mwa)) {
        if (!iceRodSpinActive) {
            u8 isBigSpin = (mwa == PLAYER_MWA_BIG_SPIN_1H || mwa == PLAYER_MWA_BIG_SPIN_2H);
            IceRod_StartSpinIce(p, play, isBigSpin);
        }
        IceRod_UpdateSpinIce(p, play);
    } else {
        if (iceRodSpinActive) IceRod_StopSpinIce();
    }

    if (mwa == sIceLastSwingType) return;
    sIceLastSwingType = mwa;

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
            IceRod_SlashEffect(p, play);
            break;

        case PLAYER_MWA_STAB_1H:
        case PLAYER_MWA_STAB_2H:
        case PLAYER_MWA_STAB_COMBO_1H:
        case PLAYER_MWA_STAB_COMBO_2H:
            IceRod_StabEffect(p, play);
            break;

        case PLAYER_MWA_FLIPSLASH_START:
        case PLAYER_MWA_JUMPSLASH_START:
            sIceJumpEffectSpawned = 0;
            break;

        case PLAYER_MWA_FLIPSLASH_FINISH:
        case PLAYER_MWA_JUMPSLASH_FINISH:
            if (!sIceJumpEffectSpawned) {
                IceRod_JumpEffect(p, play);
                sIceJumpEffectSpawned = 1;
            }
            break;

        default:
            break;
    }
}

// =============================================================================
// CHARGE ATTACK
// =============================================================================

static u8 IceRod_CanCharge(Player* p, PlayState* play) {
    if (p->meleeWeaponState > 0) return 0;
    if (p->stateFlags1 & (PLAYER_STATE1_DEAD | PLAYER_STATE1_IN_CUTSCENE |
                          PLAYER_STATE1_DAMAGED | PLAYER_STATE1_LOADING |
                          PLAYER_STATE1_HOOKSHOT_FALLING)) return 0;
    if (!(p->actor.bgCheckFlags & 1)) return 0;
    if (p->stateFlags2 & PLAYER_STATE2_HOPPING) return 0;
    return 1;
}

static void IceRod_StartCharge(Player* p, PlayState* play) {
    iceRodCharging = 1;
    iceRodChargeLevel = 0.0f;
    iceRodChargeReady = 0;
    iceRodChargeTimer = 0;
    iceRodState = ICE_ROD_STATE_CHARGING;
    Audio_PlayActorSound2(&p->actor, ICE_ROD_SFX_CHARGE);
}

static void IceRod_UpdateCharge(Player* p, PlayState* play) {
    if (!iceRodCharging) return;

    iceRodChargeTimer++;

    if (iceRodChargeLevel < 1.0f) {
        iceRodChargeLevel += ICE_ROD_CHARGE_RATE;
        if (iceRodChargeLevel > 1.0f) iceRodChargeLevel = 1.0f;
    }

    if (!iceRodChargeReady && iceRodChargeLevel >= ICE_ROD_CHARGE_MIN) {
        iceRodChargeReady = 1;
        Audio_PlayActorSound2(&p->actor, ICE_ROD_SFX_CHARGE);
    }

    if (iceRodChargeLevel >= ICE_ROD_CHARGE_BIG &&
        iceRodChargeTimer == (s16)(ICE_ROD_CHARGE_BIG / ICE_ROD_CHARGE_RATE)) {
        Audio_PlayActorSound2(&p->actor, ICE_ROD_SFX_CHARGE);
    }

    FX_DrawChargeAura(play, p, iceRodChargeLevel, &sIceRodColor);

    if ((play->gameplayFrames % 3) == 0) {
        Vec3f* tipPos = &p->meleeWeaponInfo[0].tip;
        FX_SpawnRodSwingParticles(play, tipPos, &sIceRodColor);
    }

    if ((play->gameplayFrames % 12) == 0) {
        Audio_PlayActorSound2(&p->actor, ICE_ROD_SFX_ICE_IGNITE);
    }
}

static void IceRod_ReleaseCharge(Player* p, PlayState* play) {
    if (!iceRodCharging) return;

    s32 spinType;
    u8 isBigSpin = 0;
    s16 magicCost;

    if (iceRodChargeLevel >= ICE_ROD_CHARGE_BIG) {
        spinType = PLAYER_MWA_BIG_SPIN_1H;
        magicCost = ICE_ROD_MAGIC_SPIN_BIG;
        isBigSpin = 1;
    } else if (iceRodChargeLevel >= ICE_ROD_CHARGE_MIN) {
        spinType = PLAYER_MWA_SPIN_ATTACK_1H;
        magicCost = ICE_ROD_MAGIC_SPIN_SMALL;
    } else {
        iceRodCharging = 0;
        iceRodChargeLevel = 0.0f;
        iceRodChargeReady = 0;
        iceRodState = ICE_ROD_STATE_EQUIPPED;
        return;
    }

    if (IceRod_CheckBackfire(p, play, magicCost, ICE_ROD_BACKFIRE_SPIN)) {
        iceRodCharging = 0;
        iceRodChargeLevel = 0.0f;
        iceRodChargeReady = 0;
        iceRodState = ICE_ROD_STATE_EQUIPPED;
        return;
    }

    ItemMagic_Consume(play, magicCost);
    func_80837948(play, p, spinType);

    iceRodCharging = 0;
    iceRodChargeLevel = 0.0f;
    iceRodChargeReady = 0;
    iceRodState = ICE_ROD_STATE_SWINGING;
    Audio_PlayActorSound2(&p->actor, ICE_ROD_SFX_SWING);
}

static void IceRod_CancelCharge(Player* p) {
    iceRodCharging = 0;
    iceRodChargeLevel = 0.0f;
    iceRodChargeReady = 0;
    iceRodState = ICE_ROD_STATE_EQUIPPED;
    sIceChargeButtonHeld = 0;
    sIceChargeHoldCounter = 0;
}

// =============================================================================
// FIRST PERSON MODE - Toggle with C-UP, exit on other buttons
// =============================================================================

static void IceRod_EnterFirstPerson(Player* p, PlayState* play) {
    FirstPerson_Init(p, play);
    iceRodFirstPerson = 1;
    iceRodState = ICE_ROD_STATE_AIMING;
}

static void IceRod_ExitFirstPerson(Player* p, PlayState* play) {
    FirstPerson_Exit(p, play);
    iceRodFirstPerson = 0;
    iceRodState = ICE_ROD_STATE_EQUIPPED;
}

static void IceRod_UpdateFirstPerson(Player* p, PlayState* play, ItemInputState* in) {
    FirstPerson_Update(p, play);

    // Fire on equipped C-button press
    if (in->isPressed) {
        IceRod_FirstPersonFire(p, play);
    }

    // Toggle off with C-UP
    if (CHECK_BTN_ALL(play->state.input[0].press.button, BTN_CUP)) {
        IceRod_ExitFirstPerson(p, play);
        return;
    }

    // Exit on B, other C-buttons, or damage/cutscene
    u16 exitButtons = BTN_B | BTN_CLEFT | BTN_CRIGHT | BTN_CDOWN;
    if (in->equippedButton) exitButtons &= ~in->equippedButton;

    if (CHECK_BTN_ANY(play->state.input[0].press.button, exitButtons) ||
        (p->stateFlags1 & (PLAYER_STATE1_DEAD | PLAYER_STATE1_IN_CUTSCENE | PLAYER_STATE1_DAMAGED))) {
        IceRod_ExitFirstPerson(p, play);
    }
}

// =============================================================================
// EQUIP/UNEQUIP
// =============================================================================

static void IceRod_OnEquip(PlayState* play, Player* p) {
    iceRodActive = 1;
    iceRodState = ICE_ROD_STATE_EQUIPPED;
    sIceLastSwingType = 0;
    sIceJumpEffectSpawned = 0;
    iceRodProjActive = 0;
    iceRodProjCount = 0;
    iceRodFirstPerson = 0;
    iceRodWaveActive = 0;

    iceRodCharging = 0;
    iceRodChargeLevel = 0.0f;
    iceRodChargeReady = 0;
    iceRodChargeTimer = 0;
    sIceChargeButtonHeld = 0;
    sIceChargeHoldCounter = 0;

    iceRodBlureIdx = FX_InitSwordTrail(play, &sIceRodColor);
    ItemEquip_PlayEquipSFX(play, p);
}

static void IceRod_OnUnequip(PlayState* play, Player* p) {
    if (iceRodFirstPerson) IceRod_ExitFirstPerson(p, play);

    iceRodActive = 0;
    iceRodState = ICE_ROD_STATE_INACTIVE;
    sIceLastSwingType = 0;
    iceRodProjActive = 0;
    iceRodProjCount = 0;
    iceRodWaveActive = 0;

    iceRodCharging = 0;
    iceRodChargeLevel = 0.0f;
    iceRodChargeReady = 0;
    iceRodChargeTimer = 0;
    sIceChargeButtonHeld = 0;
    sIceChargeHoldCounter = 0;

    if (iceRodBlureIdx >= 0) {
        FX_KillSwordTrail(play, iceRodBlureIdx);
        iceRodBlureIdx = -1;
    }

    if (iceRodSpinActive) IceRod_StopSpinIce();
    ItemEquip_PlayUnequipSFX(play, p);
}

// =============================================================================
// MAIN HANDLER
// =============================================================================

void Handle_IceRod(Player* p, PlayState* play) {
    IceRod_UpdateProjectile(p, play);
    IceRod_UpdateIceWave(p, play);

    ItemInputState in;
    ItemInput_Update(&in, ITEM_ROD_ICE, p, play);
    iceRodButtonMask = in.equippedButton;

    if (!in.wasEquipped) {
        if (iceRodActive) IceRod_OnUnequip(play, p);
        sIceEquipState.isEquipped = 0;
        return;
    }

    // C-UP toggles first person mode
    if (iceRodActive && !iceRodFirstPerson && !iceRodCharging && p->meleeWeaponState == 0) {
        if (CHECK_BTN_ALL(play->state.input[0].press.button, BTN_CUP)) {
            IceRod_EnterFirstPerson(p, play);
            return;
        }
    }

    if (iceRodFirstPerson) {
        IceRod_UpdateFirstPerson(p, play, &in);
        return;
    }

    if (!iceRodActive) {
        if (ItemInput_IsBlockedEx(p, play, 1)) return;
    } else {
        u32 criticalBlocks = (PLAYER_STATE1_DEAD | PLAYER_STATE1_IN_CUTSCENE | PLAYER_STATE1_LOADING |
            PLAYER_STATE1_IN_ITEM_CS | PLAYER_STATE1_TALKING | PLAYER_STATE1_GETTING_ITEM |
            PLAYER_STATE1_DAMAGED | PLAYER_STATE1_HANGING_OFF_LEDGE | PLAYER_STATE1_CLIMBING_LEDGE |
            PLAYER_STATE1_CLIMBING_LADDER | PLAYER_STATE1_ON_HORSE | PLAYER_STATE1_HOOKSHOT_FALLING);
        if (p->stateFlags1 & criticalBlocks) {
            if (iceRodCharging) IceRod_CancelCharge(p);
            return;
        }
    }

    if (ItemInput_CheckDamage(p, &sIcePrevInvinc)) {
        if (iceRodCharging) IceRod_CancelCharge(p);
        if (iceRodActive) IceRod_OnUnequip(play, p);
        sIceEquipState.isEquipped = 0;
        return;
    }

    ItemEquip_Update(&sIceEquipState, &in, IceRod_OnEquip, IceRod_OnUnequip, p, play);

    if (!iceRodActive) return;

    if (iceRodCharging) {
        if (in.isHeld) IceRod_UpdateCharge(p, play);
        else IceRod_ReleaseCharge(p, play);
    } else if (in.isHeld && IceRod_CanCharge(p, play)) {
        if (!sIceChargeButtonHeld) {
            sIceChargeButtonHeld = 1;
            sIceChargeHoldCounter = 0;
        }
        sIceChargeHoldCounter++;
        if (sIceChargeHoldCounter >= ICE_ROD_CHARGE_HOLD_FRAMES) {
            IceRod_StartCharge(p, play);
        }
    } else {
        sIceChargeButtonHeld = 0;
        sIceChargeHoldCounter = 0;
    }

    if (p->meleeWeaponState > 0) {
        iceRodState = ICE_ROD_STATE_SWINGING;
        IceRod_ProcessSwing(p, play);
        if (iceRodCharging) IceRod_CancelCharge(p);
    } else {
        if (!iceRodCharging) {
            iceRodState = ICE_ROD_STATE_EQUIPPED;
            sIceLastSwingType = 0;
        }
        if (iceRodSpinActive) IceRod_StopSpinIce();
    }
}

// =============================================================================
// INIT
// =============================================================================

void Player_InitIceRodIA(PlayState* play, Player* p) {
    iceRodActive = 1;
    iceRodState = ICE_ROD_STATE_EQUIPPED;
    sIceLastSwingType = 0;
    sIceJumpEffectSpawned = 0;
    iceRodProjActive = 0;
    iceRodProjCount = 0;
    iceRodBlureIdx = -1;
    iceRodFirstPerson = 0;
    iceRodButtonMask = 0;
    iceRodWaveActive = 0;

    iceRodCharging = 0;
    iceRodChargeLevel = 0.0f;
    iceRodChargeReady = 0;
    iceRodChargeTimer = 0;
    sIceChargeButtonHeld = 0;
    sIceChargeHoldCounter = 0;

    IceRod_InitColliders(p, play);
    IceRod_InitWaveColliders(p, play);

    if (!sIceSpinColliderInited) {
        Collider_InitCylinder(play, &iceRodSpinCollider);
        Collider_SetCylinder(play, &iceRodSpinCollider, &p->actor, &sIceRodSpinColInit);
        sIceSpinColliderInited = 1;
    }

    iceRodSpinActive = 0;
    iceRodSpinRadius = 0.0f;
    sIceSpinExpandProgress = 0.0f;

    iceRodBlureIdx = FX_InitSwordTrail(play, &sIceRodColor);
}

void CustomItems_DrawIceRodReticle(Player* p, PlayState* play) {
    if (!iceRodFirstPerson || iceRodState != ICE_ROD_STATE_AIMING) return;
    FirstPerson_DrawReticle(p, play, 0.0f, ICE_ROD_RETICLE_R, ICE_ROD_RETICLE_G, ICE_ROD_RETICLE_B);
}
