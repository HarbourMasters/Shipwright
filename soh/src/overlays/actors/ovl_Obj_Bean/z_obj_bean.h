#ifndef Z_OBJ_BEAN_H
#define Z_OBJ_BEAN_H

#include "ultra64.h"
#include "global.h"

struct ObjBean;

typedef void (*ObjBeanActionFunc)(struct ObjBean*, PlayState*);
typedef void (*ObjBeanTransformFunc)(struct ObjBean*);

typedef struct ObjBean {
    /* 0x0000 */ DynaPolyActor dyna;
    /* 0x0164 */ ObjBeanActionFunc actionFunc;
    /* 0x0168 */ ColliderCylinder collider;
    /* 0x01B4 */ s16 timer;
    /* 0x01B6 */ Vec3s unk_1B6;
    /* 0x01BC */ ObjBeanTransformFunc transformFunc;
    /* 0x01C0 */ s16 unk_1C0;
    /* 0x01C2 */ s16 unk_1C2;
    /* 0x01C4 */ s16 unk_1C4;
    /* 0x01C6 */ s16 unk_1C6;
    /* 0x01C8 */ s16 unk_1C8;
    /* 0x01CA */ s16 unk_1CA;
    /* 0x01CC */ s16 unk_1CC;
    /* 0x01CE */ s16 unk_1CE;
    /* 0x01D0 */ s16 leafRotFactor;
    /* 0x01D2 */ u16 unk_1D2;
    /* 0x01D4 */ f32 stalkSizeMultiplier;
    /* 0x01D8 */ Vec3f pathPoints;
    /* 0x01E4 */ f32 unk_1E4;
    /* 0x01E8 */ f32 posOffsetX;
    /* 0x01EC */ f32 posOffsetZ;
    /* 0x01F0 */ s16 pathCount;
    /* 0x01F2 */ s16 currentPointIndex;
    /* 0x01F4 */ s16 nextPointIndex;
    /* 0x01F6 */ u8 unk_1F6;
    /* 0x01F7 */ u8 stateFlags;
} ObjBean; // size = 0x01F8

#endif
