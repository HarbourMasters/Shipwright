#define MM_ACTOR_COMPAT_CPLUSPLUS_CHECK_BYPASS
#include "MMActorCompat.h"
#include "math.h"
#include "functions.h"
#include "variables.h"
#include "macros.h"
#include "soh/frame_interpolation.h"
#include <libultraship/bridge.h>

extern "C" MtxF* sCurrentMatrix;

extern "C" void Matrix_RotateXS(s16 x, MatrixMode mode) {
    MtxF* cmf;
    f32 sin;
    f32 cos;
    f32 tempY;
    f32 tempZ;

    if (mode == MTXMODE_APPLY) {
        if (x != 0) {
            cmf = sCurrentMatrix;

            sin = Math_SinS(x);
            cos = Math_CosS(x);

            tempY = cmf->xy;
            tempZ = cmf->xz;
            cmf->xy = tempY * cos + tempZ * sin;
            cmf->xz = tempZ * cos - tempY * sin;

            tempY = cmf->yy;
            tempZ = cmf->yz;
            cmf->yy = tempY * cos + tempZ * sin;
            cmf->yz = tempZ * cos - tempY * sin;

            tempY = cmf->zy;
            tempZ = cmf->zz;
            cmf->zy = tempY * cos + tempZ * sin;
            cmf->zz = tempZ * cos - tempY * sin;

            tempY = cmf->wy;
            tempZ = cmf->wz;
            cmf->wy = tempY * cos + tempZ * sin;
            cmf->wz = tempZ * cos - tempY * sin;
        }
    } else {
        cmf = sCurrentMatrix;

        if (x != 0) {
            sin = Math_SinS(x);
            cos = Math_CosS(x);
        } else {
            sin = 0.0f;
            cos = 1.0f;
        }

        cmf->yx = 0.0f;
        cmf->zx = 0.0f;
        cmf->wx = 0.0f;
        cmf->xy = 0.0f;
        cmf->wy = 0.0f;
        cmf->xz = 0.0f;
        cmf->wz = 0.0f;
        cmf->xw = 0.0f;
        cmf->yw = 0.0f;
        cmf->zw = 0.0f;
        cmf->xx = 1.0f;
        cmf->ww = 1.0f;
        cmf->yy = cos;
        cmf->zz = cos;
        cmf->zy = sin;
        cmf->yz = -sin;
    }
}

extern "C" void Matrix_RotateYS(s16 y, MatrixMode mode) {
    MtxF* cmf;
    f32 sin;
    f32 cos;
    f32 tempX;
    f32 tempZ;

    if (mode == MTXMODE_APPLY) {
        if (y != 0) {
            cmf = sCurrentMatrix;

            sin = Math_SinS(y);
            cos = Math_CosS(y);

            tempX = cmf->xx;
            tempZ = cmf->xz;
            cmf->xx = tempX * cos - tempZ * sin;
            cmf->xz = tempX * sin + tempZ * cos;

            tempX = cmf->yx;
            tempZ = cmf->yz;
            cmf->yx = tempX * cos - tempZ * sin;
            cmf->yz = tempX * sin + tempZ * cos;

            tempX = cmf->zx;
            tempZ = cmf->zz;
            cmf->zx = tempX * cos - tempZ * sin;
            cmf->zz = tempX * sin + tempZ * cos;

            tempX = cmf->wx;
            tempZ = cmf->wz;
            cmf->wx = tempX * cos - tempZ * sin;
            cmf->wz = tempX * sin + tempZ * cos;
        }
    } else {
        cmf = sCurrentMatrix;

        if (y != 0) {
            sin = Math_SinS(y);
            cos = Math_CosS(y);
        } else {
            sin = 0.0f;
            cos = 1.0f;
        }

        cmf->yx = 0.0f;
        cmf->wx = 0.0f;
        cmf->xy = 0.0f;
        cmf->zy = 0.0f;
        cmf->wy = 0.0f;
        cmf->yz = 0.0f;
        cmf->wz = 0.0f;
        cmf->xw = 0.0f;
        cmf->yw = 0.0f;
        cmf->zw = 0.0f;
        cmf->yy = 1.0f;
        cmf->ww = 1.0f;
        cmf->xx = cos;
        cmf->zz = cos;
        cmf->zx = -sin;
        cmf->xz = sin;
    }
}

extern "C" void Matrix_RotateZS(s16 z, MatrixMode mode) {
    MtxF* cmf;
    f32 sin;
    f32 cos;
    f32 tempX;
    f32 tempY;
    f32 zero = 0.0;
    f32 one = 1.0;

    if (mode == MTXMODE_APPLY) {
        if (z != 0) {
            cmf = sCurrentMatrix;

            sin = Math_SinS(z);
            cos = Math_CosS(z);

            tempX = cmf->xx;
            tempY = cmf->xy;
            cmf->xx = tempX * cos + tempY * sin;
            cmf->xy = tempY * cos - tempX * sin;

            tempX = cmf->yx;
            tempY = cmf->yy;
            cmf->yx = tempX * cos + tempY * sin;
            cmf->yy = tempY * cos - tempX * sin;

            tempX = cmf->zx;
            tempY = cmf->zy;
            cmf->zx = tempX * cos + tempY * sin;
            cmf->zy = tempY * cos - tempX * sin;

            tempX = cmf->wx;
            tempY = cmf->wy;
            cmf->wx = tempX * cos + tempY * sin;
            cmf->wy = tempY * cos - tempX * sin;
        }
    } else {
        cmf = sCurrentMatrix;

        if (z != 0) {
            sin = Math_SinS(z);
            cos = Math_CosS(z);
        } else {
            sin = zero;
            cos = one;
        }

        cmf->zx = zero;
        cmf->wx = zero;
        cmf->zy = zero;
        cmf->wy = zero;
        cmf->xz = zero;
        cmf->yz = zero;
        cmf->wz = zero;
        cmf->xw = zero;
        cmf->yw = zero;
        cmf->zw = zero;
        cmf->zz = one;
        cmf->ww = one;
        cmf->xx = cos;
        cmf->yy = cos;
        cmf->yx = sin;
        cmf->xy = -sin;
    }
}

extern "C" void Matrix_MultVecZ(f32 z, Vec3f* dest) {
    MtxF* cmf = sCurrentMatrix;

    dest->x = cmf->xw + cmf->xz * z;
    dest->y = cmf->yw + cmf->yz * z;
    dest->z = cmf->zw + cmf->zz * z;
}

extern "C" void Matrix_MultZero(Vec3f* dest) {
    MtxF* cmf = sCurrentMatrix;

    dest->x = cmf->xw;
    dest->y = cmf->yw;
    dest->z = cmf->zw;
}

extern "C" void Matrix_RotateZF(f32 z, MatrixMode mode) {
    FrameInterpolation_RecordMatrixRotate1Coord(2, z, mode);
    MtxF* cmf;
    f32 sin;
    f32 cos;
    f32 tempX;
    f32 tempY;

    if (mode == MTXMODE_APPLY) {
        if (z != 0) {
            cmf = sCurrentMatrix;

            sin = sinf(z);
            cos = cosf(z);

            tempX = cmf->xx;
            tempY = cmf->xy;
            cmf->xx = tempX * cos + tempY * sin;
            cmf->xy = tempY * cos - tempX * sin;

            tempX = cmf->yx;
            tempY = cmf->yy;
            cmf->yx = tempX * cos + tempY * sin;
            cmf->yy = tempY * cos - tempX * sin;

            tempX = cmf->zx;
            tempY = cmf->zy;
            cmf->zx = tempX * cos + tempY * sin;
            cmf->zy = tempY * cos - tempX * sin;

            tempX = cmf->wx;
            tempY = cmf->wy;
            cmf->wx = tempX * cos + tempY * sin;
            cmf->wy = tempY * cos - tempX * sin;
        }
    } else {
        cmf = sCurrentMatrix;

        if (z != 0) {
            sin = sinf(z);
            cos = cosf(z);
        } else {
            sin = 0.0f;
            cos = 1.0f;
        }

        cmf->zx = 0.0f;
        cmf->wx = 0.0f;
        cmf->zy = 0.0f;
        cmf->wy = 0.0f;
        cmf->xz = 0.0f;
        cmf->yz = 0.0f;
        cmf->wz = 0.0f;
        cmf->xw = 0.0f;
        cmf->yw = 0.0f;
        cmf->zw = 0.0f;
        cmf->zz = 1.0f;
        cmf->ww = 1.0f;
        cmf->xx = cos;
        cmf->yy = cos;
        cmf->yx = sin;
        cmf->xy = -sin;
    }
}

extern "C" void Matrix_RotateXFApply(f32 x) {
    MtxF* cmf;
    f32 sin;
    f32 cos;
    f32 tempY;
    f32 tempZ;
    s32 pad;

    if (x != 0.0f) {
        cmf = sCurrentMatrix;

        sin = sins(RADF_TO_BINANG(x)) * SHT_MINV;
        cos = coss(RADF_TO_BINANG(x)) * SHT_MINV;

        tempY = cmf->xy;
        tempZ = cmf->xz;
        cmf->xy = (tempY * cos) + (tempZ * sin);
        cmf->xz = (tempZ * cos) - (tempY * sin);

        tempY = cmf->yy;
        tempZ = cmf->yz;
        cmf->yy = (tempY * cos) + (tempZ * sin);
        cmf->yz = (tempZ * cos) - (tempY * sin);

        tempY = cmf->zy;
        tempZ = cmf->zz;
        cmf->zy = (tempY * cos) + (tempZ * sin);
        cmf->zz = (tempZ * cos) - (tempY * sin);

        tempY = cmf->wy;
        tempZ = cmf->wz;
        cmf->wy = (tempY * cos) + (tempZ * sin);
        cmf->wz = (tempZ * cos) - (tempY * sin);
    }
}

extern "C" s16 Math_Atan2S_XY(f32 x, f32 y) {
    return Math_Atan2S(y, x);
}

extern "C" void Actor_UpdateVelocityWithGravity(Actor* actor) {
    actor->velocity.x = actor->speedXZ * Math_SinS(actor->world.rot.y);
    actor->velocity.z = actor->speedXZ * Math_CosS(actor->world.rot.y);

    actor->velocity.y += actor->gravity;
    if (actor->velocity.y < -20.0f /*actor->terminalVelocity*/) {
        actor->velocity.y = -20.0f /*actor->terminalVelocity*/;
    }
}

extern "C" void Actor_UpdateVelocityWithoutGravity(Actor* actor) {
    f32 speedXZ = Math_CosS(actor->world.rot.x) * actor->speedXZ;

    actor->velocity.x = Math_SinS(actor->world.rot.y) * speedXZ;
    actor->velocity.y = Math_SinS(actor->world.rot.x) * actor->speedXZ;
    actor->velocity.z = Math_CosS(actor->world.rot.y) * speedXZ;
}

extern "C" void Actor_MoveWithGravity(Actor* actor) {
    Actor_UpdateVelocityWithGravity(actor);
    Actor_UpdatePos(actor);
}

extern "C" void Actor_MoveWithoutGravity(Actor* actor) {
    Actor_UpdateVelocityWithoutGravity(actor);
    Actor_UpdatePos(actor);
}

extern "C" void Actor_SpawnIceEffects(PlayState* play, Actor* actor, Vec3f bodyPartsPos[], s32 bodyPartsCount,
                                      s32 effectsPerBodyPart, f32 scale, f32 scaleRange) {
    static Color_RGBA8 sPrimColor = { 170, 255, 255, 255 };
    static Color_RGBA8 sEnvColor = { 200, 200, 255, 255 };
    static Vec3f sAccel = { 0.0f, -1.0f, 0.0f };
    s32 i;
    s32 pad;
    Vec3f velocity;
    s16 randomYaw;
    s16 yaw;
    s32 j;

    SoundSource_PlaySfxAtFixedWorldPos(play, &actor->world.pos, 30, NA_SE_EV_ICE_BROKEN);

    for (i = 0; i < bodyPartsCount; i++) {
        yaw = Actor_WorldYawTowardPoint(actor, bodyPartsPos);

        for (j = 0; j < effectsPerBodyPart; j++) {
            randomYaw = ((s32)Rand_Next() >> 0x13) + yaw;

            velocity.z = Rand_ZeroFloat(5.0f);

            velocity.x = Math_SinS(randomYaw) * velocity.z;
            velocity.y = Rand_ZeroFloat(4.0f) + 8.0f;
            velocity.z *= Math_CosS(randomYaw);

            EffectSsEnIce_Spawn(play, bodyPartsPos, Rand_ZeroFloat(scaleRange) + scale, &velocity, &sAccel, &sPrimColor,
                                &sEnvColor, 30);
        }

        bodyPartsPos++;
    }
}

extern "C" s32 Collider_InitAndSetJntSph(PlayState* play, ColliderJntSph* sphereGroup, Actor* actor,
                                         ColliderJntSphInit* src, ColliderJntSphElement* elements) {
    Collider_InitJntSph(play, sphereGroup);
    Collider_SetJntSph(play, sphereGroup, actor, src, elements);
    return 0;
}

extern "C" s32 Collider_InitAndSetCylinder(PlayState* play, ColliderCylinder* collider, Actor* actor,
                                           ColliderCylinderInit* src) {
    Collider_InitCylinder(play, collider);
    Collider_SetCylinder(play, collider, actor, src);
    return 1;
}

/**
 * Fully initializes a ColliderQuad using the values in src.
 */
extern "C" s32 Collider_InitAndSetQuad(PlayState* play, ColliderQuad* collider, Actor* actor, ColliderQuadInit* src) {
    Collider_InitQuad(play, collider);
    Collider_SetQuad(play, collider, actor, src);
    return 0;
}

void func_800B4AEC(PlayState* play, Actor* actor, f32 y) {
    s32 floorBgId;
    f32 yPos = actor->world.pos.y;

    actor->world.pos.y += y;
    actor->floorHeight =
        BgCheck_EntityRaycastFloor5(play, &play->colCtx, &actor->floorPoly, &floorBgId, actor, &actor->world.pos);
    actor->floorBgId = floorBgId;
    actor->world.pos.y = yPos;
}

extern "C" void ActorShadow_Draw(Actor* actor, Lights* lights, PlayState* play, Gfx* dlist, Color_RGBA8* color);

#define dgSquareShadowDL "__OTR__objects/gameplay_keep/gSquareShadowDL"
static const ALIGN_ASSET(2) char gSquareShadowDL[] = dgSquareShadowDL;

extern "C" void ActorShadow_DrawSquare(Actor* actor, Lights* lights, PlayState* play) {
    if (actor->bgCheckFlags & (1 << 10)) {
        func_800B4AEC(play, actor, 50.0f);
    }

    ActorShadow_Draw(actor, lights, play, (Gfx*)gSquareShadowDL, NULL);
}

extern "C" u32 SurfaceType_GetData(CollisionContext* colCtx, CollisionPoly* poly, s32 bgId, s32 dataIdx);

extern "C" FloorProperty SurfaceType_GetFloorProperty2(CollisionContext* colCtx, CollisionPoly* poly, s32 bgId) {
    return (FloorProperty)(SurfaceType_GetData(colCtx, poly, bgId, 0) >> 26 & 0xF);
}

extern "C" void func_800B3030(PlayState* play, Vec3f* pos, Vec3f* velocity, Vec3f* accel, s16 scale, s16 scaleStep,
                              s32 colorIndex) {
    static Color_RGBA8 primColor = { 255, 255, 255, 255 };
    static Color_RGBA8 envColors[] = {
        { 255, 0, 0, 255 },
        { 0, 255, 0, 255 },
        { 0, 0, 255, 255 },
        { 150, 150, 150, 255 },
    };

    EffectSsDeadDb_Spawn(
        /* play      */ play,
        /* pos       */ pos,
        /* velocity  */ velocity,
        /* accel     */ accel,
        /* scale     */ scale,
        /* scaleStep */ scaleStep,
        /* primR     */ primColor.r,
        /* primG     */ primColor.g,
        /* primB     */ primColor.b,
        /* primA     */ primColor.a,
        /* envR      */ envColors[colorIndex].r,
        /* envG      */ envColors[colorIndex].g,
        /* envB      */ envColors[colorIndex].b,
        /* unused    */ 1,
        /* arg14     */ 9,
        /* playSound */ false);
}

extern "C" void func_800AE5A0(PlayState* play) {
    OPEN_DISPS(play->state.gfxCtx);

    gDPPipeSync(POLY_OPA_DISP++);
    POLY_OPA_DISP = Play_SetFog(play, POLY_OPA_DISP);

    CLOSE_DISPS(play->state.gfxCtx);
}