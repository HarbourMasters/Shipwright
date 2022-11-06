#ifndef Z_EN_GO_H
#define Z_EN_GO_H

#include "ultra64.h"
#include "global.h"

struct EnGo;

typedef void (*EnGoActionFunc)(struct EnGo*, PlayState*);
typedef u16 (*callback1_80A3ED24)(PlayState*, struct EnGo*);
typedef s16 (*callback2_80A3ED24)(PlayState*, struct EnGo*);

// WIP type docs
// /* 0x00 */ GORON1_CITY_LINK,
// /* 0x10 */ GORON1_FIRE_GENERIC,
// /* 0x20 */ GORON1_DMT_DC_ENTRANCE,
// /* 0x30 */ GORON1_DMT_ROLLING_SMALL,
// /* 0x40 */ GORON1_DMT_BOMB_FLOWER,
// /* 0x50 */ GORON1_CITY_ENTRANCE, 
// /* 0x60 */ GORON1_CITY_ISLAND,
// /* 0x70 */ GORON1_CITY_LOST_WOODS, 
// /* 0x80 */ // Not Used
// /* 0x90 */ GORON1_DMT_BIGGORON,


typedef struct {
    /* 0x0000 */ u8 type;
    /* 0x0001 */ u8 timer;
    /* 0x0002 */ u8 initialTimer;
    /* 0x0004 */ f32 scale;
    /* 0x0008 */ f32 scaleStep;
    /* 0x000C */ Color_RGBA8 color;
    /* 0x0010 */ char unk_10[4];
    /* 0x0014 */ Vec3f pos;
    /* 0x0020 */ Vec3f velocity;
    /* 0x002C */ Vec3f accel;
    u32 epoch;
} EnGoEffect; // size = 0x38

typedef struct EnGo {
    /* 0x0000 */ Actor actor;
    /* 0x014C */ SkelAnime skelAnime;
    /* 0x0190 */ EnGoActionFunc actionFunc;
    /* 0x0194 */ ColliderCylinder collider;
    /* 0x01E0 */ struct_80034A14_arg1 unk_1E0;
    /* 0x0208 */ char unk_208[0x4];
    /* 0x020C */ s16 unk_20C;
    /* 0x020E */ s16 unk_20E;
    /* 0x0210 */ s16 unk_210;
    /* 0x0212 */ s16 unk_212;
    /* 0x0214 */ s16 unk_214;
    /* 0x0216 */ s16 unk_216;
    /* 0x0218 */ s16 unk_218;
    /* 0x021A */ s16 unk_21A;
    /* 0x021C */ s16 unk_21C;
    /* 0x021E */ s16 unk_21E;
    /* 0x0220 */ s16 jointTable[18];
    /* 0x0244 */ s16 morphTable[18];
    /* 0x0268 */ EnGoEffect dustEffects[20];
} EnGo; // size = 0x06C8

#endif
