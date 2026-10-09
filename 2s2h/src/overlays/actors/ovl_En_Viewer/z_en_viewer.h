#ifndef Z_EN_VIEWER_H
#define Z_EN_VIEWER_H

#include "global.h"

#define ENVIEWER_GET_3(thisx) ((thisx)->params & 3)
#define ENVIEWER_GET_FE00(thisx) ((thisx)->params >> 9)
#define ENVIEWER_GET_1FC(thisx) (((thisx)->params >> 2) & 0x7F)
#define ENVIEWER_GET_8000(thisx) (((thisx)->params & 0xFFFF) >> 9)

struct EnViewer;

typedef void (*EnViewerActionFunc)(struct EnViewer*, PlayState*);

typedef struct EnViewer {
    /* 0x000 */ Actor actor;
    /* 0x144 */ EnViewerActionFunc actionFunc;
    /* 0x148 */ f32 unk_148;
    /* 0x14C */ f32 unk_14C;
    /* 0x150 */ f32 unk_150;
    /* 0x154 */ u8 unk_154;
} EnViewer; // size = 0x158

typedef enum {
    /* 0x0 */ ENVIEWER_PARAM_0,
    /* 0x1 */ ENVIEWER_PARAM_1,
    /* 0x2 */ ENVIEWER_PARAM_2
} EnViewerParam;

#endif // Z_EN_VIEWER_H
