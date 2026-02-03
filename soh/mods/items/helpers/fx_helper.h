/**
 * fx_helper.h - Visual effects for custom items
 */

#ifndef FX_HELPER_H
#define FX_HELPER_H

#include "z64.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    u8 r, g, b, a;
} FX_Color;

typedef enum {
    FX_DUST,
    FX_SPARKLE,
    FX_FIRE,
    FX_ICE,
    FX_SHOCK,
    FX_EXPLOSION
} FX_Type;

/**
 * Rod effect colors (prim + env for two-color effects).
 */
typedef struct {
    u8 primR, primG, primB, primA;
    u8 envR, envG, envB, envA;
} RodColor;

/**
 * Spawn particles at position.
 * @param play PlayState instance
 * @param pos World position
 * @param type Effect type
 * @param count Number of particles
 */
void FX_SpawnParticles(PlayState* play, Vec3f* pos, FX_Type type, u8 count);

/**
 * Spawn dust particles.
 * @param play PlayState instance
 * @param pos World position
 * @param color Particle color
 * @param count Number of particles
 */
void FX_SpawnDust(PlayState* play, Vec3f* pos, FX_Color* color, u8 count);

/**
 * Spawn sparkle particles around player.
 * @param player Player instance
 * @param play PlayState instance
 */
void FX_SpawnSparkles(Player* player, PlayState* play);

/**
 * Spawn explosion effect.
 * @param play PlayState instance
 * @param pos World position
 * @param scale Effect scale multiplier
 */
void FX_SpawnExplosion(PlayState* play, Vec3f* pos, f32 scale);

/**
 * Spawn white shockwave.
 * @param play PlayState instance
 * @param pos World position
 */
void FX_SpawnShockwave(PlayState* play, Vec3f* pos);

/**
 * Spawn scaled white shockwave.
 * @param play PlayState instance
 * @param pos World position
 * @param scale Initial scale
 * @param scaleStep Scale increment per frame
 */
void FX_SpawnShockwaveSmall(PlayState* play, Vec3f* pos, s16 scale, s16 scaleStep);

/**
 * Spawn trail effect.
 * @param play PlayState instance
 * @param pos World position
 * @param type Effect type
 */
void FX_SpawnTrail(PlayState* play, Vec3f* pos, FX_Type type);

/**
 * Spawn dust in ring pattern.
 * @param play PlayState instance
 * @param center Center position
 * @param minRadius Inner radius
 * @param maxRadius Outer radius
 * @param count Number of particles (max 8)
 * @param color Particle color
 */
void FX_SpawnRadialDust(PlayState* play, Vec3f* center, f32 minRadius, f32 maxRadius, u8 count, FX_Color* color);

/**
 * Spawn explosions in ring pattern.
 * @param play PlayState instance
 * @param center Center position
 * @param radius Ring radius
 * @param count Number of explosions (max 12)
 * @param scale Explosion scale
 */
void FX_SpawnRadialExplosion(PlayState* play, Vec3f* center, f32 radius, u8 count, f32 scale);

/**
 * Spawn lightning bolt.
 * @param play PlayState instance
 * @param pos World position
 * @param red 1 for red lightning, 0 for blue
 * @param scale Effect scale
 */
void FX_SpawnLightning(PlayState* play, Vec3f* pos, u8 red, u8 scale);

/**
 * Spawn lightning in ring pattern.
 * @param play PlayState instance
 * @param center Center position
 * @param minRadius Inner radius
 * @param maxRadius Outer radius
 * @param count Number of bolts (max 6)
 * @param red 1 for red, 0 for blue
 * @param scale Effect scale
 */
void FX_SpawnLightningRing(PlayState* play, Vec3f* center, f32 minRadius, f32 maxRadius, u8 count, u8 red, u8 scale);

/**
 * Spawn suction particles flowing toward origin.
 * @param play PlayState instance
 * @param origin Suction point
 * @param yaw Direction angle
 * @param pitch Vertical angle
 */
void FX_SpawnSuction(PlayState* play, Vec3f* origin, s16 yaw, s16 pitch);

/**
 * Spawn wind particles flowing away from origin.
 * @param play PlayState instance
 * @param origin Wind source
 * @param yaw Direction angle
 * @param range Effect range
 */
void FX_SpawnWindBlow(PlayState* play, Vec3f* origin, s16 yaw, f32 range);

/**
 * Spawn projectile trail with particles.
 * @param play PlayState instance
 * @param pos World position
 * @param type Effect type
 */
void FX_SpawnProjectileTrail(PlayState* play, Vec3f* pos, FX_Type type);

/**
 * Spawn rod fireball effect.
 * @param play PlayState instance
 * @param pos World position
 * @param yaw Direction angle
 * @param color Rod colors
 */
void FX_SpawnRodFireball(PlayState* play, Vec3f* pos, s16 yaw, RodColor* color);

/**
 * Spawn rod fire smoke burst (ground impact).
 * @param play PlayState instance
 * @param pos World position
 * @param color Rod colors
 */
void FX_SpawnRodFireSmoke(PlayState* play, Vec3f* pos, RodColor* color);

/**
 * Spawn rod energy ball with 3D direction.
 * @param play PlayState instance
 * @param pos World position
 * @param yaw Horizontal angle
 * @param pitch Vertical angle
 * @param color Rod colors
 */
void FX_SpawnRodEnergyBall(PlayState* play, Vec3f* pos, s16 yaw, s16 pitch, RodColor* color);

/**
 * Spawn fire sparks at weapon tip during swing.
 * @param play PlayState instance
 * @param tipPos Weapon tip position
 * @param color Rod colors
 */
void FX_SpawnRodSwingParticles(PlayState* play, Vec3f* tipPos, RodColor* color);

/**
 * Spawn radial fire burst for spin attack.
 * @param play PlayState instance
 * @param center Center position
 * @param color Rod colors
 */
void FX_SpawnRodSpinFire(PlayState* play, Vec3f* center, RodColor* color);

/**
 * Draw charge aura cylinder around player.
 * @param play PlayState instance
 * @param player Player instance
 * @param chargeLevel 0.0 to 1.0 charge progress
 * @param color Rod colors
 */
void FX_DrawChargeAura(PlayState* play, Player* player, f32 chargeLevel, RodColor* color);

/**
 * Spawn fire shockwave ring.
 * @param play PlayState instance
 * @param center Center position
 * @param color Rod colors
 * @param radius Effect radius
 */
void FX_SpawnFireShockwave(PlayState* play, Vec3f* center, RodColor* color, f32 radius);

/**
 * Spawn fire particles in radius.
 * @param play PlayState instance
 * @param center Center position
 * @param radius Spawn radius
 * @param color Rod colors
 * @param count Number of particles
 */
void FX_SpawnFireBurstInRadius(PlayState* play, Vec3f* center, f32 radius, RodColor* color, u8 count);

/**
 * Draw expanding spin fire cylinder.
 * @param play PlayState instance
 * @param player Player instance
 * @param radius Cylinder radius
 * @param isBigSpin 1 for big spin, 0 for small
 * @param color Rod colors
 */
void FX_DrawSpinFireCylinder(PlayState* play, Player* player, f32 radius, u8 isBigSpin, RodColor* color);

/**
 * Draw fireball sprite (EnBb-style).
 * @param play PlayState instance
 * @param pos World position
 * @param scale Sprite scale
 * @param color Rod colors
 * @param frame Animation frame
 */
void FX_DrawRodFireball(PlayState* play, Vec3f* pos, f32 scale, RodColor* color, u32 frame);

/**
 * Draw fire smoke cloud (multiple sprites).
 * @param play PlayState instance
 * @param pos World position
 * @param scale Effect scale
 * @param color Rod colors
 * @param frame Animation frame
 */
void FX_DrawRodFireSmoke(PlayState* play, Vec3f* pos, f32 scale, RodColor* color, u32 frame);

/**
 * Draw pulsing energy ball.
 * @param play PlayState instance
 * @param pos World position
 * @param scale Ball scale
 * @param color Rod colors
 * @param frame Animation frame
 */
void FX_DrawRodEnergyBall(PlayState* play, Vec3f* pos, f32 scale, RodColor* color, u32 frame);

/**
 * Initialize sword trail effect.
 * @param play PlayState instance
 * @param color Trail colors
 * @return Effect index or -1 on failure
 */
s32 FX_InitSwordTrail(PlayState* play, RodColor* color);

/**
 * Add vertex to sword trail.
 * @param blureIdx Effect index from FX_InitSwordTrail
 * @param base Handle position
 * @param tip Tip position
 */
void FX_AddSwordTrailVertex(s32 blureIdx, Vec3f* base, Vec3f* tip);

/**
 * Clean up sword trail effect.
 * @param play PlayState instance
 * @param blureIdx Effect index
 */
void FX_KillSwordTrail(PlayState* play, s32 blureIdx);

#ifdef __cplusplus
}
#endif

#endif // FX_HELPER_H
