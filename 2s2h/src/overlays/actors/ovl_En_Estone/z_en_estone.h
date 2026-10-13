#ifndef Z_EN_ESTONE_H
#define Z_EN_ESTONE_H

#include "global.h"

struct EnEstone;

typedef void (*EnEstoneActionFunc)(struct EnEstone*, PlayState*);

#define ENESTONE_EFFECT_COUNT 200

typedef enum {
    /* 0 */ ENESTONE_TYPE_LARGE,
    /* 1 */ ENESTONE_TYPE_SMALL
} EnEstoneType;

typedef struct {
    /* 0x00 */ u8 isActive;
    /* 0x04 */ Vec3f pos;
    /* 0x10 */ Vec3f velocity;
    /* 0x1C */ Vec3f accel;
    /* 0x28 */ Vec3s rot;
    /* 0x30 */ f32 scale;
    /* 0x34 */ s16 timer;
} EnEstoneEffect; // size = 0x38

typedef struct EnEstone {
    /* 0x0000 */ Actor actor;
    /* 0x0144 */ EnEstoneActionFunc actionFunc;
    /* 0x0148 */ f32 scale;
    /* 0x014C */ Vec3f rot;
    /* 0x0158 */ Vec3f rotVel;
    /* 0x0164 */ s16 timer;
    /* 0x0166 */ s16 inactive;
    /* 0x0168 */ ColliderCylinder collider;
    /* 0x01B4 */ EnEstoneEffect effects[ENESTONE_EFFECT_COUNT];
} EnEstone; // size = 0x2D74

#endif // Z_EN_ESTONE_H
