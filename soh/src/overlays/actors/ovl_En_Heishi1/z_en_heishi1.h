#ifndef Z_EN_HEISHI1_H
#define Z_EN_HEISHI1_H

#include "ultra64.h"
#include "global.h"

struct EnHeishi1;

typedef void (*EnHeishi1ActionFunc)(struct EnHeishi1*, PlayState*);

typedef struct EnHeishi1 {
    /* 0x0000 */ Actor actor;
    /* 0x014C */ SkelAnime skelAnime;
    /* 0x0190 */ Vec3s jointTable[17];
    /* 0x01F6 */ Vec3s morphTable[17];
    /* 0x025C */ EnHeishi1ActionFunc actionFunc;
    /* 0x0260 */ s16 activeTimer; 
    /* 0x0262 */ s16 headDirection;
    /* 0x0264 */ s16 headBehaviorDecided;
    /* 0x0266 */ s16 linkDetected;
    /* 0x0268 */ s16 loadStarted;
    /* 0x026A */ s16 type;
    /* 0x026C */ s16 path;
    /* 0x026E */ s16 unused1;
    /* 0x0270 */ s16 waypointTimer;
    /* 0x0272 */ s16 unused2;
    /* 0x0274 */ f32 moveSpeed;
    /* 0x0278 */ f32 bodyTurnSpeed;
    /* 0x027C */ f32 headAngle;
    /* 0x0280 */ f32 headAngleTarget;
    union {
        struct {
            /* 0x0284 */ f32 animSpeed;
            /* 0x0288 */ f32 transitionRate;
            /* 0x028C */ f32 moveSpeedTarget;
            /* 0x0290 */ f32 moveSpeedMax;
            /* 0x0294 */ f32 bodyTurnSpeedTarget;
            /* 0x0298 */ f32 bodyTurnSpeedMax;
            /* 0x029C */ f32 headTurnSpeedScale;
            /* 0x02A0 */ f32 headTurnSpeedMax;
        };
        f32 animParams[8];
    };
    union {
        struct {
            /* 0x02A4 */ s16 headTimer;
            /* 0x02A6 */ s16 waitTimer;
            /* 0x02A8 */ s16 kickTimer;
        };
        s16 timers[3];
    };
    /* 0x02AA */ s16 waypoint;
} EnHeishi1; // size = 0x02AC

#endif
