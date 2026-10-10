/*
 * File: z_mir_ray.c
 * Overlay: ovl_Mir_Ray
 * Description: Reflectible light ray (unused, somewhat broken)
 * Note: This actor is unchanged from OoT, and will not work correctly
 */

#include "z_mir_ray.h"
#include "objects/object_mir_ray/object_mir_ray.h"

#define FLAGS (ACTOR_FLAG_UPDATE_CULLING_DISABLED | ACTOR_FLAG_DRAW_CULLING_DISABLED)

void MirRay_Init(Actor* thisx, PlayState* play);
void MirRay_Destroy(Actor* thisx, PlayState* play);
void MirRay_Update(Actor* thisx, PlayState* play);
void MirRay_Draw(Actor* thisx, PlayState* play);

s32 MirRay_CheckInFrustum(Vec3f* vecA, Vec3f* vecB, f32 pointx, f32 pointy, f32 pointz, s16 radiusA, s16 radiusB);

typedef struct {
    /* 0x00 */ Vec3f pos;
    /* 0x0C */ MtxF mtx;
    /* 0x4C */ CollisionPoly* reflectionPoly;
    /* 0x50 */ u8 opacity;
} MirRayShieldReflection; // size = 0x54

typedef struct {
    /* 0x00 */ Vec3s sourcePoint;
    /* 0x06 */ Vec3s poolPoint;     // point at center of light pool on floor for windows and BigMirror, same as source
                                    // point for Cobra Mirror
    /* 0x0C */ s16 sourceEndRadius; // Radius of beam frustum at the source end
    /* 0x0E */ s16 poolEndRadius;   // Radius of beam frustum at the pool end
    /* 0x10 */ f32 unk_10;          // placement of collider center along beam
    /* 0x14 */ s16 unk_14;          // collider radius before scaled
    /* 0x16 */ s16 lgtPtMaxRad;     // light point max radius
    /* 0x18 */ f32 unk_18; // placement of light point between source and reflection point (pool point for windows,
                           // player for mirrors)
    /* 0x1C */ Color_RGB8 color;
    /* 0x1F */ u8 params;
} MirRayDataEntry; // size = 0x20

ActorProfile Mir_Ray_Profile = {
    /**/ ACTOR_MIR_RAY,
    /**/ ACTORCAT_ITEMACTION,
    /**/ FLAGS,
    /**/ OBJECT_MIR_RAY,
    /**/ sizeof(MirRay),
    /**/ MirRay_Init,
    /**/ MirRay_Destroy,
    /**/ MirRay_Update,
    /**/ MirRay_Draw,
};

u8 D_808E3BF0 = false;

static ColliderQuadInit sQuadInit = {
    {
        COL_MATERIAL_NONE,
        AT_ON | AT_TYPE_PLAYER,
        AC_NONE,
        OC1_NONE,
        OC2_NONE,
        COLSHAPE_QUAD,
    },
    {
        ELEM_MATERIAL_UNK0,
        { 0x00200000, 0x00, 0x00 },
        { 0xF7CFFFFF, 0x00, 0x00 },
        ATELEM_ON | ATELEM_SFX_NORMAL,
        ACELEM_NONE,
        OCELEM_NONE,
    },
    { { { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } } },
};

static ColliderJntSphElementInit sJntSphElementsInit[1] = {
    {
        {
            ELEM_MATERIAL_UNK0,
            { 0x00200000, 0x00, 0x00 },
            { 0x00000000, 0x00, 0x00 },
            ATELEM_ON | ATELEM_SFX_NORMAL,
            ACELEM_NONE,
            OCELEM_NONE,
        },
        { 0, { { 0, 0, 0 }, 50 }, 100 },
    },
};

static ColliderJntSphInit sJntSphInit = {
    {
        COL_MATERIAL_NONE,
        AT_ON | AT_TYPE_PLAYER,
        AC_NONE,
        OC1_NONE,
        OC2_NONE,
        COLSHAPE_JNTSPH,
    },
    ARRAY_COUNT(sJntSphElementsInit),
    sJntSphElementsInit,
};

// Note: this data is unchanged from OoT and hardcoded for Spirit Temple, so the actor will not work at all without
// modification.

MirRayDataEntry sMirRayData[] = {
    {
        { -0x488, 0x2AE, -0x370 },
        { -0x398, 0x1E0, -0x379 },
        30,
        50,
        1.0f,
        50,
        150,
        0.8f,
        { 255, 255, 255 },
        2,
    },
    {
        { -0x740, 0x444, -0xBE },
        { -0x6A7, 0x349, -0xBA },
        30,
        70,
        0.88f,
        0x36,
        150,
        0.8f,
        { 255, 255, 255 },
        2,
    },
    {
        { 0x557, 0x2E2, -0x35C },
        { 0x443, 0x1DC, -0x35C },
        30,
        0x55,
        0.0f,
        0,
        150,
        0.8f,
        { 255, 255, 255 },
        0,
    },
    {
        { 0x898, 0x44F, -0xDC },
        { 0x7F8, 0x34B, -0xDC },
        30,
        60,
        0.0f,
        0,
        150,
        0.8f,
        { 255, 255, 255 },
        1,
    },
    {
        { -0x230, 0x879, -0x136 },
        { -0x230, 0x6CF, -0x136 },
        30,
        70,
        0.0f,
        0,
        150,
        0.8f,
        { 255, 255, 255 },
        0,
    },
    {
        { 0x3C, 0x70A, -0x442 },
        { 0x3C, 0x3CD, -0x442 },
        30,
        70,
        0.0f,
        0,
        150,
        0.9f,
        { 255, 255, 255 },
        13,
    },
    {
        { 0x474, 0x1E0, -0x35C },
        { 0x474, 0x1E0, -0x35C },
        30,
        30,
        1.0f,
        10,
        100,
        0.9f,
        { 255, 255, 255 },
        14,
    },
    {
        { -0x230, 0x6CF, -0x136 },
        {
            -0x230,
            0x6CF,
            -0x136,
        },
        30,
        30,
        0.0f,
        0,
        100,
        0.94f,
        { 255, 255, 255 },
        12,
    },
    {
        { 0x3C, 0x6CF, -0x136 },
        { 0x3C, 0x6CF, -0x136 },
        30,
        30,
        0.0f,
        0,
        100,
        0.94f,
        { 255, 255, 255 },
        12,
    },
    {
        { -0x496, 0x1C0, 0x4AA },
        { -0x496, 0x94, 0x4AA },
        50,
        100,
        1.0f,
        50,
        150,
        0.8f,
        { 255, 255, 255 },
        3,
    },
};

static InitChainEntry sInitChain[] = {
    ICHAIN_VEC3F_DIV1000(scale, 0, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeDistance, 4000, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeScale, 1000, ICHAIN_CONTINUE),
    ICHAIN_F32(cullingVolumeDownward, 1000, ICHAIN_STOP),
};

const char D_808E3DD0[] = "反射光 発生失敗";

void MirRay_SetupCollider(MirRay* this) {
    MirRayDataEntry* dataEntry = &sMirRayData[MIRRAY_LOCATION(&this->actor)];
    f32 x = (this->poolPt.x - this->sourcePt.x) * dataEntry->unk_10;
    f32 y = (this->poolPt.y - this->sourcePt.y) * dataEntry->unk_10;
    f32 z = (this->poolPt.z - this->sourcePt.z) * dataEntry->unk_10;

    this->collider1.elements->dim.worldSphere.center.x = this->sourcePt.x + x;
    this->collider1.elements->dim.worldSphere.center.y = this->sourcePt.y + y;
    this->collider1.elements->dim.worldSphere.center.z = this->sourcePt.z + z;

    this->collider1.elements->dim.worldSphere.radius = dataEntry->unk_14 * this->collider1.elements->dim.scale;
}

// Set up a light point between source point and reflection point. Reflection point is the pool point (for windows) or
// at the player position (for mirrors)
void MirRay_MakeShieldLight(MirRay* this, PlayState* play) {
    Player* player = GET_PLAYER(play);
    MirRayDataEntry* dataEntry = &sMirRayData[MIRRAY_LOCATION(&this->actor)];
    Vec3f reflectionPt;
    Vec3s lightPt;

    if (MirRay_CheckInFrustum(&this->sourcePt, &this->poolPt, player->actor.world.pos.x,
                              player->actor.world.pos.y + 30.0f, player->actor.world.pos.z, this->sourceEndRad,
                              this->poolEndRad)) {
        if (dataEntry->params & 8) {
            Math_Vec3f_Diff(&player->actor.world.pos, &this->sourcePt, &reflectionPt);
        } else {
            Math_Vec3f_Diff(&this->poolPt, &this->sourcePt, &reflectionPt);
        }

        lightPt.x = (dataEntry->unk_18 * reflectionPt.x) + this->sourcePt.x;
        lightPt.y = (dataEntry->unk_18 * reflectionPt.y) + this->sourcePt.y;
        lightPt.z = (dataEntry->unk_18 * reflectionPt.z) + this->sourcePt.z;

        // Fade up
        Math_StepToS(&this->lightPointRad, dataEntry->lgtPtMaxRad, 6);
        Lights_PointNoGlowSetInfo(&this->lightInfo, lightPt.x, lightPt.y, lightPt.z, dataEntry->color.r,
                                  dataEntry->color.g, dataEntry->color.b, this->lightPointRad);
    } else {
        // Fade down
        Math_StepToS(&this->lightPointRad, 0, 6);
        Lights_PointSetColorAndRadius(&this->lightInfo, dataEntry->color.r, dataEntry->color.g, dataEntry->color.b,
                                      this->lightPointRad);
    }
}

void MirRay_Init(Actor* thisx, PlayState* play) {
    s32 pad;
    MirRay* this = (MirRay*)thisx;
    MirRayDataEntry* dataEntry = &sMirRayData[MIRRAY_LOCATION(&this->actor)];

    Actor_ProcessInitChain(&this->actor, sInitChain);
    ActorShape_Init(&this->actor.shape, 0.0f, NULL, 0.0f);

    if (MIRRAY_LOCATION(&this->actor) >= MIRRAY_MAX) {
        Actor_Kill(&this->actor);
    }

    this->sourcePt.x = dataEntry->sourcePoint.x;
    this->sourcePt.y = dataEntry->sourcePoint.y;
    this->sourcePt.z = dataEntry->sourcePoint.z;
    this->sourceEndRad = dataEntry->sourceEndRadius;

    this->poolPt.x = dataEntry->poolPoint.x;
    this->poolPt.y = dataEntry->poolPoint.y;
    this->poolPt.z = dataEntry->poolPoint.z;
    this->poolEndRad = dataEntry->poolEndRadius;

    Lights_PointNoGlowSetInfo(&this->lightInfo, this->sourcePt.x, this->sourcePt.y, this->sourcePt.z, 255, 255, 255,
                              100);
    this->lightNode = LightContext_InsertLight(play, &play->lightCtx, &this->lightInfo);

    this->shieldCorners[0].x = -536.0f;
    this->shieldCorners[0].y = -939.0f;

    this->shieldCorners[1].x = -1690.0f;
    this->shieldCorners[1].y = 0.0f;

    this->shieldCorners[2].x = -536.0f;
    this->shieldCorners[2].y = 938.0f;

    this->shieldCorners[3].x = 921.0f;
    this->shieldCorners[3].y = 0.0f;

    this->shieldCorners[4].x = 758.0f;
    this->shieldCorners[4].y = 800.0f;

    this->shieldCorners[5].x = 758.0f;
    this->shieldCorners[5].y = -800.0f;

    if (dataEntry->params & 2) {
        Collider_InitJntSph(play, &this->collider1);
        Collider_SetJntSph(play, &this->collider1, &this->actor, &sJntSphInit, this->collider1Elements);
        if (!(dataEntry->params & 4)) { // Beams not from mirrors
            MirRay_SetupCollider(this);
        }
    }

    Collider_InitQuad(play, &this->collider2);
    Collider_SetQuad(play, &this->collider2, &this->actor, &sQuadInit);

    if ((MIRRAY_LOCATION(&this->actor) == MIRRAY_SPIRIT_TOPROOM_CEILINGMIRROR) ||
        (MIRRAY_LOCATION(&this->actor) == MIRRAY_SPIRIT_TOPROOM_COBRA1) ||
        (MIRRAY_LOCATION(&this->actor) == MIRRAY_SPIRIT_TOPROOM_COBRA2)) {
        this->actor.room = -1;
    }
}

void MirRay_Destroy(Actor* thisx, PlayState* play) {
    MirRay* this = (MirRay*)thisx;

    LightContext_RemoveLight(play, &play->lightCtx, this->lightNode);

    if (sMirRayData[MIRRAY_LOCATION(&this->actor)].params & 2) {
        Collider_DestroyJntSph(play, &this->collider1);
    }

    Collider_DestroyQuad(play, &this->collider2);
}

void MirRay_Update(Actor* thisx, PlayState* play) {
    s32 pad;
    MirRay* this = (MirRay*)thisx;
    Player* player = GET_PLAYER(play);

    D_808E3BF0 = false;

    if (!this->unLit) {
        if (sMirRayData[MIRRAY_LOCATION(&this->actor)].params & 2) {
            if (sMirRayData[MIRRAY_LOCATION(&this->actor)].params & 4) {
                MirRay_SetupCollider(this);
            }
            CollisionCheck_SetAT(play, &play->colChkCtx, &this->collider1.base);
        }

        if (this->reflectIntensity > 0.0f) {
            CollisionCheck_SetAT(play, &play->colChkCtx, &this->collider2.base);
        }

        MirRay_MakeShieldLight(this, play);

        if (this->reflectIntensity > 0.0f) {
            Actor_PlaySfx_Flagged2(&player->actor, NA_SE_IT_SHIELD_BEAM - SFX_FLAG);
        }
    }
}

void MirRay_SetIntensity(MirRay* this, PlayState* play) {
    f32 sp4C[3];
    s32 pad[4];
    Player* player = GET_PLAYER(play);
    MtxF* shieldMtx = &player->shieldMf;
    f32 temp_f0;
    f32 temp_f0_2;
    f32 temp_f2;

    this->reflectIntensity = 0.0f;

    if (MirRay_CheckInFrustum(&this->sourcePt, &this->poolPt, shieldMtx->xw, shieldMtx->yw, shieldMtx->zw,
                              this->sourceEndRad, this->poolEndRad)) {
        temp_f0 = sqrtf(SQ(shieldMtx->zz) + (SQ(shieldMtx->xz) + SQ(shieldMtx->yz)));
        if (temp_f0 == 0.0f) {
            this->reflectRange = 1.0f;
        } else {
            this->reflectRange = 1.0f / temp_f0;
        }

        if (sMirRayData[MIRRAY_LOCATION(&this->actor)].params & 1) {
            this->reflectIntensity = 1.0f;
        } else {
            sp4C[0] = this->poolPt.x - this->sourcePt.x;
            sp4C[1] = this->poolPt.y - this->sourcePt.y;
            sp4C[2] = this->poolPt.z - this->sourcePt.z;

            temp_f2 = (-shieldMtx->xz * sp4C[0]) - (shieldMtx->yz * sp4C[1]) - (shieldMtx->zz * sp4C[2]);

            if (temp_f2 < 0.0f) {
                temp_f0_2 = sqrtf(SQ(sp4C[0]) + SQ(sp4C[1]) + SQ(sp4C[2]));
                if ((temp_f0 != 0.0f) && (temp_f0_2 != 0.0f)) {
                    this->reflectIntensity = -temp_f2 / (temp_f0 * temp_f0_2);
                }
            }
        }
    }
}

// Draws six images, one for each corner of the shield, by finding the intersection of a line segment from the corner
// perpendicular to the shield with the nearest collision (if any).
void MirRay_SetupReflectionPolys(MirRay* this, PlayState* play, MirRayShieldReflection* reflection) {
    s32 i;
    Player* player = GET_PLAYER(play);
    MtxF* shieldMtx = &player->shieldMf;
    Vec3f posA;
    Vec3f posB;
    Vec3f sp70;
    CollisionPoly* outPoly;
    f32 sp60[3];

    sp60[0] = -(shieldMtx->xz * this->reflectRange) * this->reflectIntensity * 400.0f;
    sp60[1] = -(shieldMtx->yz * this->reflectRange) * this->reflectIntensity * 400.0f;
    sp60[2] = -(shieldMtx->zz * this->reflectRange) * this->reflectIntensity * 400.0f;

    for (i = 0; i < ARRAY_COUNT(this->shieldCorners); i++) {
        posA.x =
            (shieldMtx->xw + (this->shieldCorners[i].x * shieldMtx->xx)) + (this->shieldCorners[i].y * (*shieldMtx).xy);
        posA.y =
            (shieldMtx->yw + (this->shieldCorners[i].x * shieldMtx->yx)) + (this->shieldCorners[i].y * (*shieldMtx).yy);
        posA.z =
            (shieldMtx->zw + (this->shieldCorners[i].x * shieldMtx->zx)) + (this->shieldCorners[i].y * (*shieldMtx).zy);

        posB.x = sp60[0] + posA.x;
        posB.y = sp60[1] + posA.y;
        posB.z = sp60[2] + posA.z;

        if (BgCheck_AnyLineTest1(&play->colCtx, &posA, &posB, &sp70, &outPoly, true)) {
            reflection[i].reflectionPoly = outPoly;
        } else {
            reflection[i].reflectionPoly = NULL;
        }
    }
}

// Remove reflections that are in the same position and are sufficiently near to the same plane
void MirRay_RemoveSimilarReflections(MirRayShieldReflection* reflection) {
    s32 i;
    s32 j;

    for (i = 0; i < 6; i++) {
        for (j = i + 1; j < 6; j++) {
            if ((reflection[i].reflectionPoly != NULL) && (reflection[j].reflectionPoly != NULL)) {
                if ((ABS(reflection[i].reflectionPoly->normal.x - reflection[j].reflectionPoly->normal.x) < 100) &&
                    (ABS(reflection[i].reflectionPoly->normal.y - reflection[j].reflectionPoly->normal.y) < 100) &&
                    (ABS(reflection[i].reflectionPoly->normal.z - reflection[j].reflectionPoly->normal.z) < 100) &&
                    (reflection[i].reflectionPoly->dist == reflection[j].reflectionPoly->dist)) {
                    reflection[j].reflectionPoly = NULL;
                }
            }
        }
    }
}

// Creates the reflected beam's collider (to interact with objects) and places and orients the shield images
void MirRay_ReflectedBeam(MirRay* this, PlayState* play, MirRayShieldReflection* reflection) {
    Player* player = GET_PLAYER(play);
    s32 i;
    f32 temp_f0;
    Vec3f vecB;
    Vec3f vecD;
    Vec3f sp118;
    Vec3f sp10C;
    Vec3f sp100;
    Vec3f intersection;
    f32 spE8[3];
    f32 normalVec[3];
    MtxF* shieldMtx = &player->shieldMf;

    spE8[0] = -(shieldMtx->xz * this->reflectRange) * this->reflectIntensity * 400.0f;
    spE8[1] = -(shieldMtx->yz * this->reflectRange) * this->reflectIntensity * 400.0f;
    spE8[2] = -(shieldMtx->zz * this->reflectRange) * this->reflectIntensity * 400.0f;

    vecB.x = shieldMtx->xw;
    vecB.y = shieldMtx->yw;
    vecB.z = shieldMtx->zw;

    vecD.x = spE8[0] + vecB.x;
    vecD.y = spE8[1] + vecB.y;
    vecD.z = spE8[2] + vecB.z;

    {
        Vec3f vecA;
        Vec3f vecC;

        vecA.x = vecB.x + (shieldMtx->xx * 300.0f);
        vecA.y = vecB.y + (shieldMtx->yx * 300.0f);
        vecA.z = vecB.z + (shieldMtx->zx * 300.0f);

        vecC.x = vecD.x + (shieldMtx->xx * 300.0f);
        vecC.y = vecD.y + (shieldMtx->yx * 300.0f);
        vecC.z = vecD.z + (shieldMtx->zx * 300.0f);

        Collider_SetQuadVertices(&this->collider2, &vecA, &vecB, &vecC, &vecD);

        for (i = 0; i < 6; i++) {
            if (reflection[i].reflectionPoly != NULL) {
                normalVec[0] = COLPOLY_GET_NORMAL(reflection[i].reflectionPoly->normal.x);
                normalVec[1] = COLPOLY_GET_NORMAL(reflection[i].reflectionPoly->normal.y);
                normalVec[2] = COLPOLY_GET_NORMAL(reflection[i].reflectionPoly->normal.z);

                if (Math3D_LineSegVsPlane(normalVec[0], normalVec[1], normalVec[2], reflection[i].reflectionPoly->dist,
                                          &vecB, &vecD, &sp118, 1)) {

                    reflection[i].pos.x = sp118.x;
                    reflection[i].pos.y = sp118.y;
                    reflection[i].pos.z = sp118.z;

                    temp_f0 = sqrtf(SQ(sp118.x - vecB.x) + SQ(sp118.y - vecB.y) + SQ(sp118.z - vecB.z));

                    if (temp_f0 < (this->reflectIntensity * 600.0f)) {
                        reflection[i].opacity = 200;
                    } else {
                        reflection[i].opacity = (s32)(800.0f - temp_f0);
                    }

                    reflection[i].opacity = (s32)(reflection[i].opacity * 1.275f);

                    sp10C.x = (shieldMtx->xx * 100.0f) + vecB.x;
                    sp10C.y = (shieldMtx->yx * 100.0f) + vecB.y;
                    sp10C.z = (shieldMtx->zx * 100.0f) + vecB.z;

                    sp100.x = (spE8[0] * 4.0f) + sp10C.x;
                    sp100.y = (spE8[1] * 4.0f) + sp10C.y;
                    sp100.z = (spE8[2] * 4.0f) + sp10C.z;

                    reflection[i].mtx.xx = reflection[i].mtx.yy = reflection[i].mtx.zz = reflection[i].mtx.ww = 1.0f;

                    reflection[i].mtx.yx = reflection[i].mtx.zx = reflection[i].mtx.wx = reflection[i].mtx.xy =
                        reflection[i].mtx.zy = reflection[i].mtx.wy = reflection[i].mtx.xz = reflection[i].mtx.yz =
                            reflection[i].mtx.wz = reflection[i].mtx.xw = reflection[i].mtx.yw = reflection[i].mtx.zw =
                                0.0f;

                    if (Math3D_LineSegVsPlane(normalVec[0], normalVec[1], normalVec[2],
                                              reflection[i].reflectionPoly->dist, &sp10C, &sp100, &intersection, 1)) {
                        reflection[i].mtx.xx = intersection.x - sp118.x;
                        reflection[i].mtx.yx = intersection.y - sp118.y;
                        reflection[i].mtx.zx = intersection.z - sp118.z;
                    }

                    sp10C.x = (shieldMtx->xy * 100.0f) + vecB.x;
                    sp10C.y = (shieldMtx->yy * 100.0f) + vecB.y;
                    sp10C.z = (shieldMtx->zy * 100.0f) + vecB.z;

                    sp100.x = (spE8[0] * 4.0f) + sp10C.x;
                    sp100.y = (spE8[1] * 4.0f) + sp10C.y;
                    sp100.z = (spE8[2] * 4.0f) + sp10C.z;

                    if (Math3D_LineSegVsPlane(normalVec[0], normalVec[1], normalVec[2],
                                              reflection[i].reflectionPoly->dist, &sp10C, &sp100, &intersection, 1)) {
                        reflection[i].mtx.xy = intersection.x - sp118.x;
                        reflection[i].mtx.yy = intersection.y - sp118.y;
                        reflection[i].mtx.zy = intersection.z - sp118.z;
                    }
                } else {
                    reflection[i].reflectionPoly = NULL;
                }
            }
        }
    }
}

void MirRay_Draw(Actor* thisx, PlayState* play) {
    s32 pad;
    MirRay* this = (MirRay*)thisx;
    Player* player = GET_PLAYER(play);
    MirRayShieldReflection reflection[6];
    s32 i;

    this->reflectIntensity = 0.0f;

    if (!D_808E3BF0 && !this->unLit && Player_IsHoldingMirrorShield(play)) {
        Matrix_Mult(&player->shieldMf, MTXMODE_NEW);
        MirRay_SetIntensity(this, play);

        if (this->reflectIntensity <= 0.0f) {
            return;
        }

        OPEN_DISPS(play->state.gfxCtx);

        Gfx_SetupDL25_Xlu(play->state.gfxCtx);
        Matrix_Scale(1.0f, 1.0f, this->reflectIntensity, MTXMODE_APPLY);

        MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx);
        gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 255, 255, 255, (u8)(s8)(this->reflectIntensity * 100.0f));

        AnimatedMat_Draw(play, Lib_SegmentedToVirtual(object_mir_ray_Matanimheader_0003F8));

        gSPDisplayList(POLY_XLU_DISP++, object_mir_ray_DL_000168);

        MirRay_SetupReflectionPolys(this, play, reflection);
        MirRay_RemoveSimilarReflections(reflection);
        MirRay_ReflectedBeam(this, play, reflection);

        if (reflection[0].reflectionPoly == NULL) {
            reflection[0].opacity = 0;
        }

        for (i = 1; i < ARRAY_COUNT(reflection); i++) {
            if (reflection[i].reflectionPoly != NULL) {
                if (reflection[0].opacity < reflection[i].opacity) {
                    reflection[0].opacity = reflection[i].opacity;
                }
            }
        }

        for (i = 0; i < ARRAY_COUNT(reflection); i++) {
            if (reflection[i].reflectionPoly != NULL) {
                Matrix_Translate(reflection[i].pos.x, reflection[i].pos.y, reflection[i].pos.z, MTXMODE_NEW);
                Matrix_Scale(0.01f, 0.01f, 0.01f, MTXMODE_APPLY);
                Matrix_Mult(&reflection[i].mtx, MTXMODE_APPLY);

                MATRIX_FINALIZE_AND_LOAD(POLY_XLU_DISP++, play->state.gfxCtx);
                gDPSetRenderMode(POLY_XLU_DISP++, G_RM_FOG_SHADE_A, G_RM_AA_ZB_XLU_DECAL2);
                gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 255, 255, 255, reflection[0].opacity);
                gSPDisplayList(POLY_XLU_DISP++, object_mir_ray_DL_0004B0);
            }
        }

        D_808E3BF0 = true;

        CLOSE_DISPS(play->state.gfxCtx);
    }
}

// Computes if the Point (pointx, pointy, pointz) lies within the right conical frustum with one end centred at vecA
// with radius radiusA, the other at vecB with radius radiusB
s32 MirRay_CheckInFrustum(Vec3f* vecA, Vec3f* vecB, f32 pointx, f32 pointy, f32 pointz, s16 radiusA, s16 radiusB) {
    f32 coneRadius;
    f32 closestPtx;
    f32 closestPty;
    f32 closestPtz;
    Vec3f vecdiff;
    f32 dist;
    Vec3f sp5C;
    Vec3f sp50;
    Vec3f sp44;

    vecdiff.x = vecB->x - vecA->x;
    vecdiff.y = vecB->y - vecA->y;
    vecdiff.z = vecB->z - vecA->z;
    if (1) {}

    dist = SQ(vecdiff.x) + SQ(vecdiff.y) + SQ(vecdiff.z);

    if (dist == 0.0f) {
        return false;
    }

    dist =
        (((pointx - vecA->x) * vecdiff.x) + ((pointy - vecA->y) * vecdiff.y) + ((pointz - vecA->z) * vecdiff.z)) / dist;

    // Closest point on line A-B to Point
    closestPtx = (vecdiff.x * dist) + vecA->x;
    closestPty = (vecdiff.y * dist) + vecA->y;
    closestPtz = (vecdiff.z * dist) + vecA->z;

    // Diameter of the double cone on the perpendicular plane through the closest point
    coneRadius = ((radiusB - radiusA) * dist) + radiusA;

    // If the Point is within the bounding double cone, check if it is in the frustum by checking whether it is between
    // the bounding planes
    if ((SQ(closestPtx - pointx) + SQ(closestPty - pointy) + SQ(closestPtz - pointz)) <= SQ(coneRadius)) {
        if (1) {}

        // Stores the vector difference again
        Math_Vec3f_Diff(vecB, vecA, &sp5C);

        sp50.x = pointx - vecA->x;
        sp50.y = pointy - vecA->y;
        sp50.z = pointz - vecA->z;

        if (Math3D_Cos(&sp5C, &sp50) < 0.0f) {
            return false;
        }

        sp44.x = pointx - vecB->x;
        sp44.y = pointy - vecB->y;
        sp44.z = pointz - vecB->z;

        if (Math3D_Cos(&sp5C, &sp44) > 0.0f) {
            return false;
        }
        return true;
    }
    return false;
}
