#ifndef Z64ANIMATION_LEGACY_H
#define Z64ANIMATION_LEGACY_H

#include "ultra64.h"
#include "z64math.h"

typedef struct LegacyLimb {
    /* 0x00 */ Gfx* dList;
    /* 0x04 */ Vec3f trans;
    /* 0x10 */ Vec3s rot;
    /* 0x18 */ struct LegacyLimb* sibling;
    /* 0x1C */ struct LegacyLimb* child;
} LegacyLimb; // size = 0x20

typedef struct {
    /* 0x00 */ s16 xMax;
    /* 0x02 */ s16 x;
    /* 0x04 */ s16 yMax;
    /* 0x06 */ s16 y;
    /* 0x08 */ s16 zMax;
    /* 0x10 */ s16 z;
} LegacyJointKey; // size = 0x12

typedef struct {
    /* 0x0 */ s16 frameCount;
    /* 0x2 */ s16 limbCount;
    /* 0x4 */ s16* frameData;
    /* 0x8 */ LegacyJointKey* jointKey;
} LegacyAnimationHeader; // size = 0xC

s16 SkelAnime_GetFrameDataLegacy(LegacyAnimationHeader* animation, s32 frame, Vec3s* frameTable);
s16 Animation_GetLimbCountLegacy(LegacyAnimationHeader* animation);
s16 Animation_GetLengthLegacy(LegacyAnimationHeader* animation);
s16 Animation_GetLastFrameLegacy(LegacyAnimationHeader* animation);

#endif
