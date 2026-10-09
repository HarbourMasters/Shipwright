#ifndef Z_OBJ_HAKAISI_H
#define Z_OBJ_HAKAISI_H

#include "global.h"

struct ObjHakaisi;

typedef void (*ObjHakaisiActionFunc)(struct ObjHakaisi*, PlayState*);

#define OBJHAKAISI_GET_FF(thisx) ((thisx)->params & 0xFF)
#define OBJHAKAISI_GET_SWITCH_FLAG(thisx) (((thisx)->params & 0xFF00) >> 8)

#define OBJHAKAISI_SWITCH_FLAG_NONE 0xFF

typedef struct ObjHakaisi {
    /* 0x000 */ DynaPolyActor dyna;
    /* 0x15C */ ObjHakaisiActionFunc actionFunc;
    /* 0x160 */ Vec3f unk_160[3];
    /* 0x184 */ Vec3f unk_184;
    /* 0x190 */ s32 switchFlag;
    /* 0x194 */ s16 unk_194;
    /* 0x196 */ s16 csId;
    /* 0x198 */ s16 unk_198;
    /* 0x19A */ s16 unk_19A;
    /* 0x19C */ s16 unk_19C;
    /* 0x19E */ u8 unk_19E;
} ObjHakaisi; // size = 0x1A0

#endif // Z_OBJ_HAKAISI_H
