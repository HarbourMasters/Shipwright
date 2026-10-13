#ifndef Z_OBJ_RAILLIFT_H
#define Z_OBJ_RAILLIFT_H

#include "global.h"

struct ObjRaillift;

typedef void (*ObjRailliftActionFunc)(struct ObjRaillift*, PlayState*);

#define OBJRAILLIFT_GET_TYPE(thisx) (((thisx)->params >> 0xF) & 1)
#define OBJRAILLIFT_HAS_FLAG(thisx) (((thisx)->params >> 0xD) & 1)
#define OBJRAILLIFT_GET_SWITCH_FLAG(thisx) ((thisx)->home.rot.x & 0x7F)
#define OBJRAILLIFT_GET_PATH_INDEX(thisx) ((thisx)->params & 0x7F)
#define OBJRAILLIFT_GET_STARTING_POINT(thisx) (((thisx)->params >> 7) & 0x1F)
#define OBJRAILLIFT_GET_SPEED(thisx) ((thisx)->home.rot.z * 0.1f)
#define OBJRAILLIFT_SHOULD_TELEPORT(thisx) (((thisx)->params >> 0xC) & 1)
#define OBJRAILLIFT_REACT_TO_PLAYER_ON_TOP(thisx) (((thisx)->params >> 0xE) & 1)

typedef enum {
    /* 0 */ OOT_WATER_TEMPLE_WATERFALL_PLATFORM,
    /* 1 */ DEKU_FLOWER_PLATFORM
} OBJRAILLIFT_TYPE;

typedef struct ObjRaillift {
    /* 0x000 */ DynaPolyActor dyna;
    /* 0x15C */ ObjRailliftActionFunc actionFunc;
    /* 0x160 */ f32 speed;
    /* 0x164 */ s32 endPoint;
    /* 0x168 */ s32 curPoint;
    /* 0x16C */ s32 direction; // +1 for forward, -1 for backward
    /* 0x170 */ Vec3s* pathPoints;
    /* 0x174 */ s32 isPlayerOnTop;
    /* 0x178 */ s32 isPlayerOnTopPrev;
    /* 0x17C */ f32 cycleSpeed;
    /* 0x180 */ f32 maxHeight;
    /* 0x184 */ s16 cycle;
    /* 0x186 */ s16 waitTimer;
    /* 0x188 */ s16 cutsceneTimer;
} ObjRaillift; // size = 0x18C

#endif // Z_OBJ_RAILLIFT_H
