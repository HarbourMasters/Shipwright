#ifndef Z_EN_JJ_H
#define Z_EN_JJ_H

#include "ultra64.h"
#include "global.h"

struct EnJj;

typedef void (*EnJjActionFunc)(struct EnJj*, PlayState*);

typedef struct EnJj {
    /* 0x0000 */ DynaPolyActor dyna;
    /* 0x0164 */ SkelAnime skelAnime;
    /* 0x01A8 */ Vec3s jointTable[22];
    /* 0x022C */ Vec3s morphTable[22];
    /* 0x02B0 */ ColliderCylinder collider;
    /* 0x02FC */ EnJjActionFunc actionFunc;
    /* 0x0300 */ DynaPolyActor* bodyCollisionActor;
    /* 0x0304 */ Actor* dust;
    /* 0x0308 */ s16 mouthOpenAngle;
    /* 0x030A */ u16 unk_30A;
    /* 0x030C */ s16 cutsceneCountdownTimer;
    /* 0x030E */ u8 eyeIndex;
    /* 0x030F */ u8 blinkTimer;
    /* 0x0310 */ u8 extraBlinkCounter;
    /* 0x0311 */ u8 extraBlinkTotal;
} EnJj; // size = 0x0314

typedef enum {
    /* -1 */ JABUJABU_MAIN = -1, // Head, drawn body, handles updating
    /*  0 */ JABUJABU_COLLISION, // Static collision for body
    /*  1 */ JABUJABU_UNUSED_COLLISION // Shaped like a screen
} EnJjType;


#endif
