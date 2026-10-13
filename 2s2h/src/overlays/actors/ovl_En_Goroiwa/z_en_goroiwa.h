#ifndef Z_EN_GOROIWA_H
#define Z_EN_GOROIWA_H

#include "global.h"

struct EnGoroiwa;

typedef void (*EnGoroiwaActionFunc)(struct EnGoroiwa*, PlayState*);
typedef s32 (*EnGoroiwaUnkFunc)(struct EnGoroiwa*);

#define ENGOROIWA_GET_C000(thisx) (((thisx)->params >> 0xE) & 3)
#define ENGOROIWA_GET_3000(thisx) (((thisx)->params >> 0xC) & 3)
#define ENGOROIWA_GET_400(thisx) (((thisx)->params >> 0xA) & 1)
#define ENGOROIWA_GET_300(thisx) (((thisx)->params >> 0x8) & 3)
#define ENGOROIWA_GET_PATH_INDEX(thisx) ((thisx)->params & 0xFF)

#define ENGOROIWA_PATH_INDEX_NONE 0xFF

typedef enum {
    /* 0 */ ENGOROIWA_300_0,
    /* 1 */ ENGOROIWA_300_1,
    /* 3 */ ENGOROIWA_300_3 = 3
} EnGoroiwaParam300;

typedef enum {
    /* 0 */ ENGOROIWA_400_0,
    /* 1 */ ENGOROIWA_400_1
} EnGoroiwaParam400;

typedef enum {
    /* 0 */ ENGOROIWA_3000_0,
    /* 1 */ ENGOROIWA_3000_1,
    /* 2 */ ENGOROIWA_3000_2
} EnGoroiwaParam3000;

typedef enum {
    /* 1 */ ENGOROIWA_C000_1 = 1,
    /* 2 */ ENGOROIWA_C000_2
} EnGoroiwaParamC000;

typedef struct {
    /* 0x00 */ Vec3f unk_00;
    /* 0x0C */ f32 unk_0C;
    /* 0x10 */ f32 unk_10;
    /* 0x14 */ f32 unk_14;
    /* 0x18 */ f32 unk_18;
    /* 0x1C */ s16 unk_1C;
    /* 0x1E */ s16 unk_1E;
    /* 0x20 */ s16 unk_20;
    /* 0x22 */ s16 unk_22;
    /* 0x24 */ s16 unk_24;
    /* 0x26 */ s16 unk_26;
    /* 0x28 */ CollisionPoly* unk_28;
    /* 0x2C */ u8 unk_2C;
    /* 0x2D */ u8 unk_2D;
} EnGoroiwaStruct; // size = 0x30

typedef struct EnGoroiwa {
    /* 0x000 */ Actor actor;
    /* 0x144 */ ColliderJntSph collider;
    /* 0x164 */ ColliderJntSphElement colliderElements[1];
    /* 0x1A4 */ EnGoroiwaActionFunc actionFunc;
    /* 0x1A8 */ Vec3f unk_1A8;
    /* 0x1B4 */ Vec3f unk_1B4;
    /* 0x1C0 */ f32 unk_1C0;
    /* 0x1C4 */ f32 unk_1C4;
    /* 0x1C8 */ s16 unk_1C8;
    /* 0x1CA */ s16 unk_1CA;
    /* 0x1CC */ s16 unk_1CC;
    /* 0x1CE */ s16 unk_1CE;
    /* 0x1D0 */ Vec3s* pathPoints;
    /* 0x1D4 */ s16 unk_1D4;
    /* 0x1D6 */ s16 unk_1D6;
    /* 0x1D8 */ s16 unk_1D8;
    /* 0x1DA */ s16 unk_1DA;
    /* 0x1DC */ f32 unk_1DC;
    /* 0x1E0 */ f32 unk_1E0;
    /* 0x1E4 */ s8 unk_1E4;
    /* 0x1E5 */ u8 unk_1E5;
    /* 0x1E6 */ s8 unk_1E6;
    /* 0x1E7 */ s8 unk_1E7;
    /* 0x1E8 */ EnGoroiwaStruct unk_1E8[2];
    /* 0x248 */ s32 unk_248;
} EnGoroiwa; // size = 0x24C

#endif // Z_EN_GOROIWA_H
