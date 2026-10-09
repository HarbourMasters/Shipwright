#ifndef Z_OBJ_BEAN_H
#define Z_OBJ_BEAN_H

#include "global.h"

struct ObjBean;

typedef void (*ObjBeanActionFunc)(struct ObjBean*, PlayState*);
typedef void (*ObjBeanUnkFunc)(struct ObjBean*);

#define OBJBEAN_GET_SWITCH_FLAG_1(thisx, offset) (((thisx)->params + (offset)) & 0x7F)
#define OBJBEAN_GET_80(thisx) (((thisx)->params >> 7) & 1)
#define OBJBEAN_GET_PATH_INDEX(thisx) (((thisx)->params >> 8) & 0x3F)
#define OBJBEAN_GET_SWITCH_FLAG_2(thisx, offset) ((((thisx)->params >> 7) + (offset)) & 0x7F)
#define OBJBEAN_GET_C000(thisx) (((thisx)->params >> 0xE) & 3)

#define OBJBEAN_GET_3(thisx) ((thisx)->home.rot.z & 3)

typedef enum {
    /* 0 */ ENOBJBEAN_GET_C000_0,
    /* 1 */ ENOBJBEAN_GET_C000_1,
    /* 2 */ ENOBJBEAN_GET_C000_2
} ObjBeanParam;

typedef struct ObjBean {
    /* 0x000 */ DynaPolyActor dyna;
    /* 0x15C */ ObjBeanActionFunc actionFunc;
    /* 0x160 */ ColliderCylinder collider;
    /* 0x1AC */ s16 unk_1AC;
    /* 0x1AE */ s16 unk_1AE;
    /* 0x1B0 */ s16 unk_1B0;
    /* 0x1B2 */ s16 unk_1B2;
    /* 0x1B4 */ s8 unk_1B4;
    /* 0x1B8 */ f32 unk_1B8;
    /* 0x1BC */ Vec3f unk_1BC;
    /* 0x1C8 */ f32 unk_1C8;
    /* 0x1CC */ f32 unk_1CC;
    /* 0x1D0 */ f32 unk_1D0;
    /* 0x1D4 */ Vec3s* pathPoints;
    /* 0x1D8 */ s16 unk_1D8;
    /* 0x1DA */ s16 unk_1DA;
    /* 0x1DC */ s16 unk_1DC;
    /* 0x1DE */ s8 unk_1DE;
    /* 0x1DF */ s8 unk_1DF;
    /* 0x1E0 */ s8 unk_1E0;
    /* 0x1E4 */ s32 unk_1E4;
    /* 0x1E8 */ ObjBeanUnkFunc unk_1E8;
    /* 0x1EC */ s16 unk_1EC;
    /* 0x1EE */ s16 unk_1EE;
    /* 0x1F0 */ s16 unk_1F0;
    /* 0x1F2 */ s16 unk_1F2;
    /* 0x1F4 */ s16 unk_1F4;
    /* 0x1F6 */ s16 unk_1F6;
    /* 0x1F8 */ s16 unk_1F8;
    /* 0x1FA */ s16 unk_1FA;
    /* 0x1FC */ s16 unk_1FC;
    /* 0x1FE */ u8 unk_1FE;
    /* 0x1FF */ u8 unk_1FF;
    /* 0x200 */ u8 unk_200;
} ObjBean; // size = 0x204

#endif // Z_OBJ_BEAN_H
