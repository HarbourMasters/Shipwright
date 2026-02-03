/**
 * grappling_helper.h - Surface analysis for grapple-type items
 * Used by: Whip, Switch Hook (future)
 *
 * Detects beam/bar shaped hookshottable surfaces and graspable actors.
 * Uses raycasting and collision polygon analysis.
 */

#ifndef GRAPPLING_HELPER_H
#define GRAPPLING_HELPER_H

#include "z64.h"

// =============================================================================
// Thresholds for beam/bar shape detection (OoT world units)
// =============================================================================
// Adult Link height = 68 units for reference
#define GRAPPLE_MIN_LENGTH        40.0f   // Min beam length (0.6x Link)
#define GRAPPLE_MAX_CROSS_SECTION 120.0f  // Max cross-section dim (1.8x Link)
#define GRAPPLE_MIN_THICKNESS     3.0f    // Min thickness (avoid degenerate polys)
#define GRAPPLE_MAX_CROSS_SUM     220.0f  // Sum of 2 smallest dims

// =============================================================================
// GrappleTarget: result of surface analysis
// =============================================================================
typedef struct {
    Vec3f attachPoint;     // World-space intersection point
    Vec3f surfaceNormal;   // Normal of hit surface (float)
    f32   dims[3];         // Sorted bounding box dimensions [smallest, middle, largest]
    s32   bgId;            // Background ID (BGCHECK_SCENE or dynamic actor index)
    CollisionPoly* poly;   // Hit collision polygon pointer
    s32   isGraspable;     // 1 if meets beam/bar proportions for swing
    s32   isHookshottable; // 1 if surface has hookshot flag set
} GrappleTarget;

// =============================================================================
// API
// =============================================================================

/**
 * Cast a ray from the player's eye position along their facing direction.
 * If it hits a collision surface within maxRange, analyzes the surface shape.
 * Supports Z-targeting (aims at focus actor) and free-aim.
 *
 * @param play       PlayState
 * @param player     Player pointer
 * @param maxRange   Maximum ray distance (e.g. 520.0f for longshot range)
 * @param outTarget  Output: filled with analysis results
 * @return           1 if a surface was hit, 0 if nothing hit
 */
s32 Grapple_FindTarget(PlayState* play, Player* player, f32 maxRange, GrappleTarget* outTarget);

/**
 * Analyze the shape of a collision surface at a given polygon.
 * Walks neighbor polygons (same surface type, shared vertices) to build
 * a bounding box and determine if the surface is beam/bar shaped.
 *
 * @param play      PlayState
 * @param poly      The collision polygon to analyze
 * @param bgId      Background ID from the line test
 * @param hitPos    World position of the hit point
 * @param outTarget Output: filled with analysis results
 * @return          1 if surface is graspable, 0 if not
 */
s32 Grapple_AnalyzeSurface(PlayState* play, CollisionPoly* poly, s32 bgId,
                           Vec3f* hitPos, GrappleTarget* outTarget);

#endif // GRAPPLING_HELPER_H
