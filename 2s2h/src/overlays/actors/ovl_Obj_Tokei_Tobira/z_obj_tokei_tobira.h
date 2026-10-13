#ifndef Z_OBJ_TOKEI_TOBIRA_H
#define Z_OBJ_TOKEI_TOBIRA_H

#include "global.h"

#define OBJTOKEITOBIRA_GET_TYPE(thisx) ((thisx)->params & 1)
#define OBJTOKEITOBIRA_PARAMS(type) (type)


struct ObjTokeiTobira;

typedef void (*ObjTokeiTobiraActionFunc)(struct ObjTokeiTobira*);


typedef enum ObjTokeiTobiraType {
    /* 0 */ OBJTOKEITOBIRA_TYPE_0,
    /* 1 */ OBJTOKEITOBIRA_TYPE_1
} ObjTokeiTobiraType;

typedef struct ObjTokeiTobira {
    /* 0x000 */ DynaPolyActor dyna;
    /* 0x15C */ ObjTokeiTobiraActionFunc actionFunc;
    /* 0x160 */ f32 unk160;
    /* 0x164 */ f32 unk164;
    /* 0x168 */ s32 unk168;
    /* 0x16C */ s32 unk16C;
} ObjTokeiTobira; // size = 0x170 

#endif // Z_OBJ_TOKEI_TOBIRA_H
