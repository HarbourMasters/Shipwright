/**
 * Grappling Helper Implementation
 *
 * Surface shape analysis for grapple-type items.
 * Uses neighbor polygon walking + bounding box to detect beam/bar geometry.
 */

#include "grappling_helper.h"
#include "z64.h"
#include "macros.h"
#include "functions.h"
#include <math.h>

// Forward declarations from z_bgcheck.c
extern CollisionHeader* BgCheck_GetCollisionHeader(CollisionContext* colCtx, s32 bgId);
extern u32 SurfaceType_IsHookshotSurface(CollisionContext* colCtx, CollisionPoly* poly, s32 bgId);
extern void CollisionPoly_GetNormalF(CollisionPoly* poly, f32* nx, f32* ny, f32* nz);

// =============================================================================
// Internal: sort 3 floats ascending
// =============================================================================
static void SortDims3(f32* a, f32* b, f32* c) {
    f32 tmp;
    if (*a > *b) { tmp = *a; *a = *b; *b = tmp; }
    if (*b > *c) { tmp = *b; *b = *c; *c = tmp; }
    if (*a > *b) { tmp = *a; *a = *b; *b = tmp; }
}

// =============================================================================
// Internal: expand bounding box with a Vec3s vertex
// =============================================================================
static void ExpandBBox(Vec3f* bMin, Vec3f* bMax, Vec3s* vtx) {
    if (vtx->x < bMin->x) bMin->x = vtx->x;
    if (vtx->y < bMin->y) bMin->y = vtx->y;
    if (vtx->z < bMin->z) bMin->z = vtx->z;
    if (vtx->x > bMax->x) bMax->x = vtx->x;
    if (vtx->y > bMax->y) bMax->y = vtx->y;
    if (vtx->z > bMax->z) bMax->z = vtx->z;
}

// =============================================================================
// Grapple_AnalyzeSurface
// =============================================================================
s32 Grapple_AnalyzeSurface(PlayState* play, CollisionPoly* poly, s32 bgId,
                           Vec3f* hitPos, GrappleTarget* outTarget) {
    CollisionHeader* colHeader;
    Vec3s* vtxList;
    u16 hitType;
    u16 idxA, idxB, idxC;
    Vec3f bMin, bMax;
    f32 dx, dy, dz;

    if (outTarget == NULL || poly == NULL) return 0;

    // Initialize output
    outTarget->poly = poly;
    outTarget->bgId = bgId;
    outTarget->isGraspable = 0;
    outTarget->isHookshottable = 0;

    if (hitPos != NULL) {
        outTarget->attachPoint = *hitPos;
    }

    // Get surface normal
    CollisionPoly_GetNormalF(poly, &outTarget->surfaceNormal.x,
                             &outTarget->surfaceNormal.y,
                             &outTarget->surfaceNormal.z);

    // Check hookshot flag
    outTarget->isHookshottable = SurfaceType_IsHookshotSurface(&play->colCtx, poly, bgId);

    // Get collision header
    colHeader = BgCheck_GetCollisionHeader(&play->colCtx, bgId);
    if (colHeader == NULL) return 0;

    // Get vertex list
    if (bgId == BGCHECK_SCENE) {
        vtxList = colHeader->vtxList;
    } else {
        vtxList = play->colCtx.dyna.vtxList;
    }
    if (vtxList == NULL) return 0;

    // Get hit poly vertex indices
    hitType = poly->type;
    idxA = COLPOLY_VTX_INDEX(poly->flags_vIA);
    idxB = COLPOLY_VTX_INDEX(poly->flags_vIB);
    idxC = poly->vIC;

    // Initialize bounding box from hit poly vertices
    bMin.x = bMin.y = bMin.z = 99999.0f;
    bMax.x = bMax.y = bMax.z = -99999.0f;

    ExpandBBox(&bMin, &bMax, &vtxList[idxA]);
    ExpandBBox(&bMin, &bMax, &vtxList[idxB]);
    ExpandBBox(&bMin, &bMax, &vtxList[idxC]);

    // Walk neighbor polygons: same surface type AND share at least 1 vertex
    for (u16 i = 0; i < colHeader->numPolygons; i++) {
        CollisionPoly* other = &colHeader->polyList[i];
        u16 oA, oB, oC;
        s32 shared;

        if (other->type != hitType) continue;
        if (other == poly) continue;

        oA = COLPOLY_VTX_INDEX(other->flags_vIA);
        oB = COLPOLY_VTX_INDEX(other->flags_vIB);
        oC = other->vIC;

        // Check if shares at least 1 vertex with hit poly
        shared = (oA == idxA || oA == idxB || oA == idxC ||
                  oB == idxA || oB == idxB || oB == idxC ||
                  oC == idxA || oC == idxB || oC == idxC);

        if (!shared) continue;

        ExpandBBox(&bMin, &bMax, &vtxList[oA]);
        ExpandBBox(&bMin, &bMax, &vtxList[oB]);
        ExpandBBox(&bMin, &bMax, &vtxList[oC]);
    }

    // Calculate dimensions
    dx = bMax.x - bMin.x;
    dy = bMax.y - bMin.y;
    dz = bMax.z - bMin.z;

    // Sort dimensions: smallest, middle, largest
    SortDims3(&dx, &dy, &dz);
    outTarget->dims[0] = dx; // smallest
    outTarget->dims[1] = dy; // middle
    outTarget->dims[2] = dz; // largest

    // Check graspable proportions
    outTarget->isGraspable =
        (dz >= GRAPPLE_MIN_LENGTH) &&
        (dx >= GRAPPLE_MIN_THICKNESS) &&
        (dx <= GRAPPLE_MAX_CROSS_SECTION) &&
        (dy <= GRAPPLE_MAX_CROSS_SECTION) &&
        (dx + dy <= GRAPPLE_MAX_CROSS_SUM);

    return outTarget->isGraspable;
}

// =============================================================================
// Grapple_FindTarget
// =============================================================================
s32 Grapple_FindTarget(PlayState* play, Player* player, f32 maxRange, GrappleTarget* outTarget) {
    Vec3f rayStart, rayEnd;
    Vec3f hitPos;
    CollisionPoly* hitPoly = NULL;
    s32 bgId = BGCHECK_SCENE;
    s16 aimYaw, aimPitch;
    f32 cosP, sinP, cosY, sinY;

    if (outTarget == NULL || player == NULL) return 0;

    // Determine aim direction
    if (Player_IsZTargeting(player) && player->focusActor != NULL) {
        // Z-target: aim at focus actor
        Vec3f targetPos = player->focusActor->focus.pos;
        f32 dx = targetPos.x - player->actor.world.pos.x;
        f32 dy = targetPos.y - (player->actor.world.pos.y + 50.0f);
        f32 dz = targetPos.z - player->actor.world.pos.z;
        f32 hDist = sqrtf(dx * dx + dz * dz);
        aimYaw = Math_Atan2S(dx, dz);
        aimPitch = Math_Atan2S(-dy, hDist);
    } else {
        // Free aim: use player facing direction
        aimYaw = player->actor.shape.rot.y;
        aimPitch = 0;
    }

    // Calculate ray start (player eye position)
    rayStart.x = player->actor.world.pos.x;
    rayStart.y = player->actor.world.pos.y + 50.0f; // eye height
    rayStart.z = player->actor.world.pos.z;

    // Calculate ray end
    cosP = Math_CosS(aimPitch);
    sinP = Math_SinS(aimPitch);
    cosY = Math_CosS(aimYaw);
    sinY = Math_SinS(aimYaw);

    rayEnd.x = rayStart.x + sinY * cosP * maxRange;
    rayEnd.y = rayStart.y - sinP * maxRange;
    rayEnd.z = rayStart.z + cosY * cosP * maxRange;

    // Cast line test
    if (!BgCheck_EntityLineTest1(&play->colCtx, &rayStart, &rayEnd, &hitPos,
                                  &hitPoly, true, true, true, true, &bgId)) {
        return 0; // Nothing hit
    }

    // Analyze the surface
    Grapple_AnalyzeSurface(play, hitPoly, bgId, &hitPos, outTarget);

    return 1;
}
