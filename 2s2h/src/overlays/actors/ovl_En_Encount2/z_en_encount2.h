#ifndef Z_EN_ENCOUNT2_H
#define Z_EN_ENCOUNT2_H

#include "global.h"

struct EnEncount2;

typedef void (*EnEncount2ActionFunc)(struct EnEncount2*, PlayState*);

#define ENCOUNT2_GET_SWITCH_FLAG(thisx) ((thisx)->params & 0x7F)

#define ENCOUNT2_SWITCH_FLAG_NONE 0x7F

typedef struct EnEncount2Effect{
   /* 0x00 */ u8 isEnabled;
   /* 0x04 */ Vec3f pos;
   /* 0x10 */ UNK_TYPE1 unk10[0x4];
   /* 0x14 */ s16 alpha;
   /* 0x16 */ s16 alphaFadeDelay; // frame count before alpha fade starts
   /* 0x18 */ Vec3f velocity;
   /* 0x24 */ Vec3f accel;
   /* 0x30 */ f32 scale;
} EnEncount2Effect; // size = 0x34

#define EN_ENCOUNT2_EFFECT_COUNT 200

typedef struct EnEncount2 {
    /* 0x0000 */ DynaPolyActor dyna;
    /* 0x015C */ EnEncount2ActionFunc actionFunc;
    /* 0x0160 */ s16 deathTimer;
    /* 0x0162 */ s16 isPopped;
    /* 0x0164 */ s16 switchFlag;
    /* 0x0168 */ f32 scale;
    /* 0x016C */ f32 oscillationAngle;
    /* 0x0170 */ ColliderJntSph collider;
    /* 0x0190 */ ColliderJntSphElement colElement;
    /* 0x01D0 */ EnEncount2Effect effects[EN_ENCOUNT2_EFFECT_COUNT];
} EnEncount2; // size = 0x2A70

#endif // Z_EN_ENCOUNT2_H
