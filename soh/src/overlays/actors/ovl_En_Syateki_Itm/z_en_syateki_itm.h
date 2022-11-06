#ifndef Z_EN_SYATEKI_ITM_H
#define Z_EN_SYATEKI_ITM_H

#include "ultra64.h"
#include "global.h"

struct EnSyatekiItm;

typedef void (*EnSyatekiItmActionFunc)(struct EnSyatekiItm*, PlayState*);

typedef enum {
    /* 0 */ ENSYATEKI_NONE,
    /* 1 */ ENSYATEKI_START,
    /* 2 */ ENSYATEKI_END,
    /* 3 */ ENSYATEKI_RESULTS
} EnSyatekiSignal;

typedef enum {
    /* 0 */ ENSYATEKIHIT_NONE,
    /* 1 */ ENSYATEKIHIT_MISS,
    /* 2 */ ENSYATEKIHIT_HIT
} EnSyatekiHitState;

typedef struct EnSyatekiItm {
    /* 0x0000 */ Actor actor;
    /* 0x014C */ EnSyatekiItmActionFunc actionFunc;
    /* 0x0150 */ s16 timer; // timer for next round
    /* 0x0152 */ s16 unkTimer; // unk timer
    /* 0x0154 */ s16 signal; // signal between this and shopkeeper
    /* 0x0156 */ s16 hitCount; // total rupees hit
    /* 0x0158 */ s16 roundNum; // current round
    /* 0x015A */ s16 roundFlags[6]; // flags for each round happening
    /* 0x0166 */ s16 targetState[2]; // current state of target rupees
    /* 0x016A */ s16 numTargets; // number of target rupees for this round
    /* 0x016C */ Vec3f targetHome[2]; // initial position of target rupees
    /* 0x0184 */ Vec3f targetFinal[2]; // target position of target rupees
    /* 0x019C */ struct EnExRuppy* markers[10]; // marker rupees for hits
    /* 0x01C4 */ struct EnGSwitch* targets[2]; // currently spawned target rupees
    /* 0x01CC */ struct EnSyatekiMan* man; // shopkeeper
    /* 0x01D0 */ struct EnExRuppy* curMarkers[2]; // marker rupees for the current round
} EnSyatekiItm; // size = 0x01D8

#endif
