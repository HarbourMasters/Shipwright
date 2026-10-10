#ifndef Z64_DAYTELOP_H
#define Z64_DAYTELOP_H

#include "z64game.h"
#include "z64view.h"

typedef struct {
    /* 0x000 */ GameState state;
    /* 0x0A4 */ void* daytelopStaticFile;
    /* 0x0A8 */ void* gameoverStaticFile;
    /* 0x0B0 */ View view;
    /* 0x218 */ UNK_TYPE1 unk_218[0x28];
    /* 0x240 */ s16 transitionCountdown;
    /* 0x242 */ s16 fadeInState;
    /* 0x244 */ s16 alpha;
} DayTelopState; // size = 0x248

typedef enum {
    /* 0 */ DAYTELOP_HOURSTEXT_OFF,
    /* 1 */ DAYTELOP_HOURSTEXT_FADEIN,
    /* 2 */ DAYTELOP_HOURSTEXT_ON
} DaytelopFadeState;

void DayTelop_Init(GameState* thisx);
void DayTelop_Destroy(GameState* thisx);

#endif
