#ifndef Z_OBJ_OSHIHIKI_H
#define Z_OBJ_OSHIHIKI_H

#include "global.h"

struct ObjOshihiki;

typedef void (*ObjOshihikiActionFunc)(struct ObjOshihiki*, PlayState*);

#define OBJOSHIHIKI_GET_F(thisx) ((thisx)->params & 0xF)
#define OBJOSHIHIKI_GET_F0(thisx) (((thisx)->params >> 4) & 0xF)
#define OBJOSHIHIKI_GET_SWITCH_FLAG(thisx) (((thisx)->params >> 8) & 0x7F)
#define OBJOSHIHIKI_GET_FF00(thisx) (((thisx)->params >> 8) & 0xFF)

typedef enum {
    /* 0 */ OBJOSHIHIKI_F_0,
    /* 1 */ OBJOSHIHIKI_F_1,
    /* 2 */ OBJOSHIHIKI_F_2,
    /* 3 */ OBJOSHIHIKI_F_3,
    /* 4 */ OBJOSHIHIKI_F_4,
    /* 5 */ OBJOSHIHIKI_F_5,
    /* 6 */ OBJOSHIHIKI_F_6,
    /* 7 */ OBJOSHIHIKI_F_7,
    /* 8 */ OBJOSHIHIKI_F_8
} ObjOshihikiParam;

#define OBJOSHIHIKI_FF00_0 0
#define OBJOSHIHIKI_FF00_80 0x80

#define PUSHBLOCK_ON_SCENE       (1 << 0)
#define PUSHBLOCK_SETUP_ON_SCENE (1 << 1)
#define PUSHBLOCK_ON_ACTOR       (1 << 2)
#define PUSHBLOCK_SETUP_ON_ACTOR (1 << 3)
#define PUSHBLOCK_PUSH           (1 << 4)
#define PUSHBLOCK_SETUP_PUSH     (1 << 5)
#define PUSHBLOCK_FALL           (1 << 6)
#define PUSHBLOCK_SETUP_FALL     (1 << 7)
#define PUSHBLOCK_MOVE_UNDER     (1 << 8)

typedef struct ObjOshihiki {
    /* 0x000 */ DynaPolyActor dyna;
    /* 0x15C */ ObjOshihikiActionFunc actionFunc;
    /* 0x160 */ u16 stateFlags;
    /* 0x162 */ s16 timer;
    /* 0x164 */ f32 yawSin;
    /* 0x168 */ f32 yawCos;
    /* 0x16C */ f32 pushSpeed;
    /* 0x170 */ f32 pushDist;
    /* 0x174 */ f32 direction;
    /* 0x178 */ s32 floorBgIds[5];
    /* 0x18C */ CollisionPoly* floorPolys[5];
    /* 0x1A0 */ f32 floorHeights[5];
    /* 0x1B4 */ s16 highestFloor;
    /* 0x1B6 */ u8 cantMove;
    /* 0x1B8 */ struct ObjOshihiki* blockUnder;
    /* 0x1BC */ f32 underDistX;
    /* 0x1C0 */ f32 underDistZ;
    /* 0x1C4 */ s8 textureStep;
    /* 0x1C5 */ Color_RGB8 color;
    /* 0x1C8 */ AnimatedMaterial* texture;
} ObjOshihiki; // size = 0x1CC

#endif // Z_OBJ_OSHIHIKI_H
