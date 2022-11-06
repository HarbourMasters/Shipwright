#ifndef Z_EN_ZO_H
#define Z_EN_ZO_H

#include "ultra64.h"
#include "global.h"

struct EnZo;

typedef struct {
    /* 0x00 */ u8 type;
    /* 0x04 */ f32 scale;
    /* 0x08 */ f32 targetScale;
    /* 0x0C */ Color_RGBA8 color;
    /* 0x10 */ u32 pad;
    /* 0x14 */ Vec3f pos;
    /* 0x20 */ Vec3f vel;
    /* 0x2C */ Vec3f vec; // Usage specific
    u32 epoch;
} EnZoEffect; // size = 0x38

typedef void (*EnZoActionFunc)(struct EnZo*, PlayState*);

typedef struct EnZo {
    /* 0x0000 */ Actor actor;
    /* 0x014C */ SkelAnime skelAnime;
    /* 0x0190 */ EnZoActionFunc actionFunc;
    /* 0x0194 */ struct_80034A14_arg1 unk_194;
    /* 0x01BC */ ColliderCylinder collider;
    /* 0x0208 */ u8 canSpeak;
    /* 0x020A */ Vec3s jointTable[20];
    /* 0x0282 */ Vec3s morphTable[20];
    /* 0x02FC */ EnZoEffect effects[15];
    /* 0x0644 */ f32 dialogRadius;
    /* 0x0648 */ f32 alpha;
    /* 0x064C */ s16 unk_64C;
    /* 0x064E */ s16 rippleTimer;
    /* 0x0650 */ s16 timeToDive;
    /* 0x0652 */ s16 blinkTimer;
    /* 0x0654 */ s16 eyeTexture;
    /* 0x0656 */ s16 unk_656[20];
    /* 0x067E */ s16 unk_67E[20];
} EnZo; // size = 0x06A8

#endif
