#ifndef Z_MIR_RAY_H
#define Z_MIR_RAY_H

#include "ultra64.h"
#include "global.h"

struct MirRay;

typedef struct {
    /* 0x00 */ Vec3s sourcePoint;
    /* 0x06 */ Vec3s poolPoint; // point at center of light pool on floor for windows and BigMirror, same as source point for Cobra Mirror
    /* 0x0C */ s16 sourceEndRadius; // Radius of beam frustum at the source end
    /* 0x0E */ s16 poolEndRadius; // Radius of beam frustum at the pool end
    /* 0x10 */ f32 unk_10; // placement of collider center along beam
    /* 0x14 */ s16 unk_14; // collider radius before scaled
    /* 0x16 */ s16 lgtPtMaxRad; // light point max radius
    /* 0x18 */ f32 unk_18; // placement of light point between source and reflection point (pool point for windows, player for mirrors)
    /* 0x1C */ Color_RGB8 color;
    /* 0x1F */ u8 params;
} MirRayDataEntry; // size = 0x20

typedef struct {
    /* 0x00 */ Vec3f pos;
    /* 0x0C */ MtxF mtx;
    /* 0x4C */ CollisionPoly* reflectionPoly;
    /* 0x50 */ u8 opacity;
} MirRayShieldReflection; // size = 0x54

typedef struct MirRay {
    /* 0x0000 */ Actor actor;
    /* 0x014C */ ColliderJntSph colliderSph;
    /* 0x016C */ ColliderJntSphElement colliderSphItem;
    /* 0x01AC */ ColliderQuad shieldRay;
    /* 0x022C */ f32 reflectIntensity; // Reflection occurs if it is positive, brightness depends on it
    /* 0x0230 */ Vec3f shieldCorners[6];
    /* 0x0278 */ f32 reflectRange;
    /* 0x027C */ Vec3f sourcePt;
    /* 0x0288 */ Vec3f poolPt;
    /* 0x0294 */ s16 sourceEndRad;
    /* 0x0296 */ s16 poolEndRad;
    /* 0x0298 */ s16 lightPointRad;
    /* 0x029C */ LightNode* lightNode;
    /* 0x02A0 */ LightInfo lightInfo;
    /* 0x02AE */ u8 unLit; // Conditioned on. set in Cobra?
} MirRay; // size = 0x02B0

#endif
