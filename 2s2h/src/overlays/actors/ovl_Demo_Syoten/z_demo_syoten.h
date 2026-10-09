#ifndef Z_DEMO_SYOTEN_H
#define Z_DEMO_SYOTEN_H

#include "global.h"

struct DemoSyoten;

typedef void (*DemoSyotenActionFunc)(struct DemoSyoten*, PlayState*);

#define DEMOSYOTEN_GET_F(thisx) ((thisx)->params & 0xF)
#define DEMOSYOTEN_GET_PATH_INDEX(thisx) (((thisx)->params & 0x7E00) >> 9)

#define DEMOSYOTEN_PATH_INDEX_NONE 0x3F

typedef enum {
    /* 0 */ DEMOSYOTEN_F_0,
    /* 1 */ DEMOSYOTEN_F_1,
    /* 2 */ DEMOSYOTEN_F_2,
    /* 3 */ DEMOSYOTEN_F_3,
    /* 4 */ DEMOSYOTEN_F_4
} DemoSyotenParam;

typedef struct DemoSyoten {
    /* 0x000 */ Actor actor;
    /* 0x144 */ KFSkelAnimeFlex kfSkelAnime;
    /* 0x174 */ Vec3s jointTable[51];
    /* 0x2A6 */ Vec3s morphTable[51];
    /* 0x3D8 */ f32 unk_3D8;
    /* 0x3DC */ Gfx* unk_3DC;
    /* 0x3E0 */ AnimatedMaterial* unk_3E0;
    /* 0x3E4 */ u16 unk_3E4;
    /* 0x3E6 */ s16 unk_3E6;
    /* 0x3E8 */ Path* path;
    /* 0x3EC */ s32 unk_3EC;
    /* 0x3F0 */ u16 cueType;
    /* 0x3F2 */ u16 cueId;
    /* 0x3F4 */ DemoSyotenActionFunc actionFunc;
} DemoSyoten; // size = 0x3F8

#endif // Z_DEMO_SYOTEN_H
