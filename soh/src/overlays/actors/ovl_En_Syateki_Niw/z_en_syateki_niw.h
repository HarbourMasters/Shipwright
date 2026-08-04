#ifndef Z_EN_SYATEKI_NIW_H
#define Z_EN_SYATEKI_NIW_H

#include <libultraship/libultra.h>
#include "global.h"

struct EnSyatekiNiw;

typedef void (*EnSyatekiNiwActionFunc)(struct EnSyatekiNiw*, PlayState*);

typedef struct {
    /* 0x00 */ u8    state;
    /* 0x0C */ Vec3f pos;
    /* 0x10 */ Vec3f vel;
    /* 0x1C */ Vec3f accel;
    /* 0x28 */ s16   lifespan;
    /* 0x2A */ s16   rotPulse;
    /* 0x2C */ f32   scale;
    /* 0x30 */ f32   rot;
    /* 0x34 */ u8    timer;
    u32 epoch;
} EnSyatekiNiw_1; // size = 0x38

typedef struct EnSyatekiNiw {
    /* 0x0000 */ Actor actor;
    /* 0x014C */ SkelAnime skelAnime;
    /* 0x0190 */ Vec3s jointTable[16];
    /* 0x01F0 */ Vec3s morphTable[16];
    /* 0x0250 */ EnSyatekiNiwActionFunc actionFunc;
    /* 0x0254 */ s16 peckTimer;
    /* 0x0256 */ s16 timer1;
    /* 0x0258 */ s16 flapTimer;
    /* 0x025A */ s16 archeryTimer;
    /* 0x025C */ s16 hopTimer;
    /* 0x025E */ s16 movementTimer;
    /* 0x0260 */ s16 sootTimer;
    /* 0x0262 */ s16 cluckTimer;
    /* 0x0264 */ f32 headRotXTarget;
    /* 0x0268 */ f32 rightWingRotXTarget;
    /* 0x026C */ f32 leftWingRotXTarget;
    /* 0x0270 */ char unk_270[0x8];
    /* 0x0278 */ f32 rightWingRotYTarget;
    /* 0x027C */ f32 rightWingRotZTarget;
    /* 0x0284 */ f32 leftWingRotYTarget;
    /* 0x0280 */ f32 leftWingRotZTarget;
    /* 0x0288 */ f32 unkArcheryFloat;
    /* 0x028C */ s16 lifetime;
    /* 0x028E */ s16 headRotXState;
    /* 0x0290 */ s16 unk_290;
    /* 0x0292 */ s16 wingsRotState;
    /* 0x0294 */ s16 targetPosTimer;
    /* 0x0296 */ s16 archeryAnimationType;
    /* 0x0298 */ s16 rotYFlip;
    /* 0x029C */ s16 archeryState;
    /* 0x029E */ s16 minigameType;
    /* 0x02A0 */ s16 spawnFeathers;
    /* 0x02A4 */ Vec3f leftWingRot;
    /* 0x02B0 */ Vec3f rightWingRot;
    /* 0x02BC */ Vec3f headRot;
    /* 0x02C8 */ Vec3f posRotStep;
    /* 0x02D4 */ f32 focusYOffset;
    /* 0x02D8 */ f32 removeStateYaw;
    /* 0x02DC */ Vec3f initPos;
    /* 0x02E8 */ Vec3f targetPos;
    /* 0x02F4 */ f32 scale;
    /* 0x02F8 */ u8 unkAlleyHitByte;
    /* 0x02FC */ ColliderCylinder collider;
    /* 0x0348 */ EnSyatekiNiw_1 effects[5];
} EnSyatekiNiw; // size = 0x0460

#endif
