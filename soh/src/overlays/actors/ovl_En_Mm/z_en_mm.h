#ifndef Z_EN_MM_H
#define Z_EN_MM_H

#include "ultra64.h"
#include "global.h"

struct EnMm;

typedef void (*EnMmActionFunc)(struct EnMm*, PlayState*);

typedef struct EnMm {
    /* 0x0000 */ Actor actor;
    /* 0x014C */ SkelAnime skelAnime;
    /* 0x0190 */ EnMmActionFunc actionFunc;
    /* 0x0194 */ ColliderCylinder collider;
    /* 0x01E0 */ s32 unk_1E0;
    /* 0x01E4 */ s32 mouthTexIndex;
    /* 0x01E8 */ s32 unk_1E8;
    /* 0x01EC */ s32 path;
    /* 0x01F0 */ s32 unk_1F0;
    /* 0x01F4 */ s32 waypoint;
    /* 0x01F8 */ f32 yawToWaypoint;
    /* 0x01FC */ f32 distToWaypoint;
    /* 0x0200 */ f32 speedXZ;
    /* 0x0204 */ s32 unk_204;
    /* 0x0208 */ MtxF unk_208;
    /* 0x0248 */ Vec3s unk_248;
    /* 0x024E */ Vec3s unk_24E;
    /* 0x0254 */ s32 unk_254;
    /* 0x0258 */ s32 curAnimIndex;
    /* 0x025C */ s32 sitTimer;
    /* 0x0260 */ Vec3s jointTable[16];
    /* 0x02C0 */ Vec3s morphTable[16];
} EnMm; // size = 0x0320

#endif
