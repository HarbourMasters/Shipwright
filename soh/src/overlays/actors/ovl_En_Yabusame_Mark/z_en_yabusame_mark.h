#ifndef Z_EN_YABUSAME_MARK_H
#define Z_EN_YABUSAME_MARK_H

#include "ultra64.h"
#include "global.h"

struct EnYabusameMark;

typedef void (*EnYabusameMarkActionFunc)(struct EnYabusameMark*, PlayState*);

typedef struct EnYabusameMark {
    /* 0x0000 */ Actor actor;
    /* 0x014C */ EnYabusameMarkActionFunc actionFunc;
    /* 0x0150 */ s16 typeIndex;
    /* 0x0152 */ s16 subTypeIndex;
    /* 0x0154 */ Vec3f worldPos;
    /* 0x0160 */ Vec3f vertexA;
    /* 0x016C */ Vec3f vertexB;
    /* 0x0178 */ Vec3f vertexC;
    /* 0x0184 */ Vec3f vertexD;
    /* 0x0190 */ ColliderQuad collider;
} EnYabusameMark; // size = 0x0210

#endif
