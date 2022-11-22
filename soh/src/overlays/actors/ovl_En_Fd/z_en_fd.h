#ifndef Z_EN_FD_H
#define Z_EN_FD_H

#include "ultra64.h"
#include "global.h"

struct EnFd;

typedef void (*EnFdActionFunc)(struct EnFd* this, PlayState* play);

typedef enum {
    FD_EFFECT_NONE,
    FD_EFFECT_FLAME,
    FD_EFFECT_DOT
} FDEffectType;

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
} EnFdEffect; // size = 0x38

typedef struct EnFd {
    /* 0x0000 */ Actor actor;
    /* 0x014C */ SkelAnime skelAnime;
    /* 0x0190 */ EnFdActionFunc actionFunc;
    /* 0x0194 */ ColliderJntSph collider;
    /* 0x01B4 */ ColliderJntSphElement colSphs[12];
    /* 0x04B4 */ u8 coreActive;
    /* 0x04B6 */ s16 initYawToInitPos;
    /* 0x04B8 */ s16 curYawToInitPos;
    /* 0x04BA */ s16 runDir;
    /* 0x04BC */ s16 firstUpdateFlag;
    /* 0x04BE */ s16 spinTimer;
    /* 0x04C0 */ s16 circlesToComplete;
    /* 0x04C2 */ s16 invincibilityTimer;
    /* 0x04C4 */ s16 attackTimer;
    /* 0x04C8 */ f32 runRadius;
    /* 0x04CC */ f32 fadeAlpha;
    /* 0x04D0 */ Vec3f corePos;
    /* 0x04DC */ Vec3s jointTable[27];
    /* 0x057E */ Vec3s morphTable[27];
    /* 0x0620 */ EnFdEffect effects[200];
} EnFd; // size = 0x31E0

#endif
