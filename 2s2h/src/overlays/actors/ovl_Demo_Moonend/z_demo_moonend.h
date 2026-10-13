#ifndef Z_DEMO_MOONEND_H
#define Z_DEMO_MOONEND_H

#include "global.h"
#include "objects/object_moonend/object_moonend.h"

struct DemoMoonend;

typedef void (*DemoMoonendActionFunc)(struct DemoMoonend*, PlayState*);

#define DEMOMOONEND_GET_PARAM_F(thisx) (((thisx)->params) & 0xF)

typedef struct DemoMoonend {
    /* 0x000 */ Actor actor;
    /* 0x144 */ KFSkelAnimeFlex kfSkelAnime;
    /* 0x174 */ Vec3s jointTable[30];
    /* 0x228 */ Vec3s morphTable[30];
    /* 0x2DC */ u16 cueType;
    /* 0x2DE */ u16 cueId;
    /* 0x2E0 */ DemoMoonendActionFunc actionFunc;
} DemoMoonend; // size = 0x2E4

#endif // Z_DEMO_MOONEND_H
