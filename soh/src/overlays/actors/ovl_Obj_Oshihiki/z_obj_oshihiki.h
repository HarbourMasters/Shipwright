#ifndef Z_OBJ_OSHIHIKI_H
#define Z_OBJ_OSHIHIKI_H

#include "ultra64.h"
#include "global.h"

struct ObjOshihiki;

typedef enum {
    /* 0 */ PUSHBLOCK_SMALL_START_ON,
    /* 1 */ PUSHBLOCK_MEDIUM_START_ON,
    /* 2 */ PUSHBLOCK_LARGE_START_ON,
    /* 3 */ PUSHBLOCK_HUGE_START_ON,
    /* 4 */ PUSHBLOCK_SMALL_START_OFF,
    /* 5 */ PUSHBLOCK_MEDIUM_START_OFF,
    /* 6 */ PUSHBLOCK_LARGE_START_OFF,
    /* 7 */ PUSHBLOCK_HUGE_START_OFF
} PushBlockType;

#define PUSHBLOCK_ON_SCENE       (1 << 0)
#define PUSHBLOCK_SETUP_ON_SCENE (1 << 1)
#define PUSHBLOCK_ON_ACTOR       (1 << 2)
#define PUSHBLOCK_SETUP_ON_ACTOR (1 << 3)
#define PUSHBLOCK_PUSH           (1 << 4)
#define PUSHBLOCK_SETUP_PUSH     (1 << 5)
#define PUSHBLOCK_FALL           (1 << 6)
#define PUSHBLOCK_SETUP_FALL     (1 << 7)
#define PUSHBLOCK_MOVE_UNDER     (1 << 8)

typedef void (*ObjOshihikiActionFunc)(struct ObjOshihiki*, PlayState*);

typedef struct ObjOshihiki {
    /* 0x0000 */ DynaPolyActor dyna;
    /* 0x0164 */ ObjOshihikiActionFunc actionFunc;
    /* 0x0168 */ u16 stateFlags;
    /* 0x016A */ s16 timer;
    /* 0x016C */ f32 yawSin;
    /* 0x0170 */ f32 yawCos;
    /* 0x0174 */ f32 pushSpeed;
    /* 0x0178 */ f32 pushDist;
    /* 0x017C */ f32 direction;
    /* 0x0180 */ s32 floorBgIds[5];
    /* 0x0194 */ CollisionPoly* floorPolys[5];
    /* 0x01A8 */ f32 floorHeights[5];
    /* 0x01BC */ s16 highestFloor;
    /* 0x01BE */ u8 cantMove;
    /* 0x01C0 */ struct ObjOshihiki* blockUnder;
    /* 0x01C4 */ f32 underDistX;
    /* 0x01C8 */ f32 underDistZ;
    /* 0x01CC */ void* texture;
    /* 0x01D0 */ Color_RGB8 color;
} ObjOshihiki; // size = 0x01D4

#endif
