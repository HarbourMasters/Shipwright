#ifndef Z_EN_ENCOUNT3_H
#define Z_EN_ENCOUNT3_H

#include "global.h"

struct EnEncount3;

typedef void (*EnEncount3ActionFunc)(struct EnEncount3*, PlayState*);

#define ENCOUNT3_GET_SPAWN_INDEX(thisx) (((thisx)->params >> 0xC) & 0xF)
#define ENCOUNT3_GET_PARAM_F80(thisx) (((thisx)->params >> 7) & 0x1F)
#define ENCOUNT3_GET_SWITCH_FLAG(thisx) ((thisx)->params & 0x7F)

#define ENCOUNT3_GET_ROTZ(thisx) ((thisx)->world.rot.z)

#define ENCOUNT3_SWITCH_FLAG_NONE 0x7F

typedef struct EnEncount3 {
    /* 0x000 */ Actor actor;
    /* 0x144 */ EnEncount3ActionFunc actionFunc;
    /* 0x148 */ u8 unk148;
    /* 0x14A */ s16 unk14A;
    /* 0x14C */ UNK_TYPE1 pad14C[0x2];
    /* 0x14E */ s16 unk14E;
    /* 0x150 */ s16 unk150;
    /* 0x152 */ s16 unk152;
    /* 0x154 */ s16 childActorId;
    /* 0x156 */ s16 unk156;
    /* 0x158 */ s16 childParams;
    /* 0x15A */ s16 csId;
    /* 0x15C */ s16 switchFlag;
    /* 0x15E */ s16 timer;
    /* 0x160 */ f32 unk160;
    /* 0x164 */ f32 unk164;
    /* 0x168 */ f32 unk168;
    /* 0x16C */ f32 unk16C;
    /* 0x170 */ f32 unk170;
    /* 0x174 */ f32 unk174;
    /* 0x178 */ f32 unk178;
    /* 0x17C */ Actor* child;
    /* 0x180 */ UNK_TYPE1 unk180[0x4C];
} EnEncount3; // size = 0x1CC

#endif // Z_EN_ENCOUNT3_H
