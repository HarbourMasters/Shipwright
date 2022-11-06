#ifndef Z_EN_PO_FIELD_H
#define Z_EN_PO_FIELD_H

#include "ultra64.h"
#include "global.h"

struct EnPoField;

typedef void (*EnPoFieldActionFunc)(struct EnPoField*, PlayState*);

typedef enum {
    EN_PO_FIELD_SMALL,
    EN_PO_FIELD_BIG
} EnPoFieldSize;

typedef struct {
    /* 0x0000 */ Color_RGB8 primColor;
    /* 0x0003 */ Color_RGB8 lightColor;
    /* 0x0006 */ Color_RGB8 envColor;
    /* 0x0009 */ s8 unk_9;
    /* 0x000C */ void* soulTexture;
} EnPoFieldInfo; // size = 0x10

typedef struct EnPoField {
    /* 0x0000 */ Actor actor;
    /* 0x014C */ SkelAnime skelAnime;
    /* 0x0190 */ EnPoFieldActionFunc actionFunc;
    /* 0x0194 */ u8 unk_194;
    /* 0x0195 */ u8 spawnFlagIndex;
    /* 0x0196 */ s16 actionTimer;
    /* 0x0198 */ s16 flameRotation;
    /* 0x019A */ s16 flameTimer;
    /* 0x019C */ Vec3s jointTable[10];
    /* 0x01D8 */ Vec3s morphTable[10];
    /* 0x0214 */ Color_RGBA8 lightColor;
    /* 0x0214 */ Color_RGBA8 soulColor;
    /* 0x021C */ f32 scaleModifier;
    /* 0x0220 */ f32 flameScale;
    /* 0x0224 */ Vec3f flamePosition;
    /* 0x0230 */ LightNode* lightNode;
    /* 0x0234 */ LightInfo lightInfo;
    /* 0x0244 */ ColliderCylinder collider;
    /* 0x0290 */ ColliderCylinder flameCollider;
} EnPoField; // size = 0x02DC

#endif
