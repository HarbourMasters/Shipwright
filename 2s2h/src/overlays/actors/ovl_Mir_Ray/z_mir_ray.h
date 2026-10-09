#ifndef Z_MIR_RAY_H
#define Z_MIR_RAY_H

#include "global.h"

struct MirRay;

#define MIRRAY_LOCATION(thisx) ((thisx)->params)

// Locations of light beams in sMirRayData
typedef enum {
    /* 0x0 */ MIRRAY_SPIRIT_BOMBCHUIWAROOM_DOWNLIGHT,
    /* 0x1 */ MIRRAY_SPIRIT_SUNBLOCKROOM_DOWNLIGHT,
    /* 0x2 */ MIRRAY_SPIRIT_SINGLECOBRAROOM_DOWNLIGHT,
    /* 0x3 */ MIRRAY_SPIRIT_ARMOSROOM_DOWNLIGHT,
    /* 0x4 */ MIRRAY_SPIRIT_TOPROOM_DOWNLIGHT,
    /* 0x5 */ MIRRAY_SPIRIT_TOPROOM_CEILINGMIRROR,
    /* 0x6 */ MIRRAY_SPIRIT_SINGLECOBRAROOM_COBRA,
    /* 0x7 */ MIRRAY_SPIRIT_TOPROOM_COBRA1,
    /* 0x8 */ MIRRAY_SPIRIT_TOPROOM_COBRA2,
    /* 0x9 */ MIRRAY_GANONSCASTLE_SPIRITTRIAL_DOWNLIGHT,
    /* 0xA */ MIRRAY_MAX
} MirRayBeamLocation;

typedef struct MirRay {
    /* 0x000 */ Actor actor;
    /* 0x144 */ ColliderJntSph collider1;
    /* 0x164 */ ColliderJntSphElement collider1Elements[1];
    /* 0x1A4 */ ColliderQuad collider2;
    /* 0x224 */ f32 reflectIntensity; // Reflection occurs if it is positive, brightness depends on it
    /* 0x228 */ Vec3f shieldCorners[6];
    /* 0x270 */ f32 reflectRange;
    /* 0x274 */ Vec3f sourcePt;
    /* 0x280 */ Vec3f poolPt;
    /* 0x28C */ s16 sourceEndRad;
    /* 0x28E */ s16 poolEndRad;
    /* 0x290 */ s16 lightPointRad;
    /* 0x294 */ LightNode* lightNode;
    /* 0x298 */ LightInfo lightInfo;
    /* 0x2A6 */ u8 unLit; // Conditioned on. set in Cobra?
} MirRay; // size = 0x2A8

#endif // Z_MIR_RAY_H
