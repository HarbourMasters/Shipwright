#ifndef Z_EN_PAPER_H
#define Z_EN_PAPER_H

#include "global.h"

struct EnPaper;

typedef void (*EnPaperActionFunc)(struct EnPaper*, PlayState*);

#define ENPAPER_PIECES_COUNT 60

typedef struct {
    /* 0x00 */ Vec3f normal; // normal to the plane of the confetti piece
    /* 0x0C */ Vec3f rotAxis; // axis to spin around
    /* 0x18 */ Vec3f pos;
    /* 0x24 */ Vec3f velocity;
    /* 0x30 */ s16 angle;  // angle to rotate around `rotAxis`
    /* 0x32 */ s16 angVel; // angular velocity of `angle`
} EnPaperConfetto; // size = 0x34

typedef struct EnPaper {
    /* 0x000 */ Actor actor;
    /* 0x144 */ EnPaperActionFunc actionFunc;
    /* 0x148 */ EnPaperConfetto pieces[ENPAPER_PIECES_COUNT];
    /* 0xD78 */ Vec3f windForce;
    /* 0xD84 */ s16 timer;
} EnPaper; // size = 0xD88

#endif // Z_EN_PAPER_H
