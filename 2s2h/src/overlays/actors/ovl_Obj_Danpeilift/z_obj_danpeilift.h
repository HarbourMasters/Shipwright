#ifndef Z_OBJ_DANPEILIFT_H
#define Z_OBJ_DANPEILIFT_H

#include "global.h"
#include "objects/object_obj_danpeilift/object_obj_danpeilift.h"

struct ObjDanpeilift;

typedef void (*ObjDanpeiliftActionFunc)(struct ObjDanpeilift*, PlayState*);

#define OBJDANPEILIFT_GET_TYPE(thisx) (((thisx)->params >> 0xF) & 1)
#define OBJDANPEILIFT_GET_STARTING_POINT(thisx) (((thisx)->params >> 7) & 0x1F)
#define OBJDANPEILIFT_GET_PATH_INDEX(thisx) ((thisx)->params & 0x7F)
#define OBJDANPEILIFT_SHOULD_TELEPORT(thisx) (((thisx)->params >> 0xC) & 1)
#define OBJDANPEILIFT_GET_SPEED(thisx) ((thisx)->home.rot.z * 0.1f)
#define OBJDANPEILIFT_REACT_TO_PLAYER_ON_TOP(thisx) (((thisx)->params >> 0xE) & 1)

typedef struct ObjDanpeilift {
    /* 0x000 */ DynaPolyActor dyna;
    /* 0x15C */ ObjDanpeiliftActionFunc actionFunc;
    /* 0x160 */ f32 speed;
    /* 0x164 */ s32 endPoint;
    /* 0x168 */ s32 curPoint;
    /* 0x16C */ s32 direction;
    /* 0x170 */ Vec3s* pathPoints;
    /* 0x174 */ s32 isPlayerOnTop;
    /* 0x178 */ s32 isPlayerOnTopPrev;
    /* 0x17C */ f32 cycleSpeed;
    /* 0x180 */ f32 maxHeight;
    /* 0x184 */ s16 cycle;
    /* 0x186 */ s16 waitTimer;
    /* 0x188 */ s16 cutsceneTimer;
} ObjDanpeilift; // size = 0x18C

#endif // Z_OBJ_DANPEILIFT_H
