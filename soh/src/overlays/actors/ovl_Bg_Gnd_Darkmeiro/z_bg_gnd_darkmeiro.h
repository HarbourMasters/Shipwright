#ifndef Z_BG_GND_DARKMEIRO_H
#define Z_BG_GND_DARKMEIRO_H

#include "ultra64.h"
#include "global.h"

struct BgGndDarkmeiro;

typedef void (*BgGndDarkmeiroUpdateFunc)(struct BgGndDarkmeiro*, PlayState*);

typedef struct BgGndDarkmeiro {
    /* 0x0000 */ DynaPolyActor dyna;
    /* 0x0164 */ u16 actionFlags;       // 0x8 for timer 2, 0x4 for timer 1, 0x2 for blocks.
    /* 0x0166 */ s16 timer1;            // Vanish countdown for clear blocks
    /* 0x0168 */ s16 timer2;            // Transparency flicker for clear blocks. Does not work.
    /* 0x016C */ BgGndDarkmeiroUpdateFunc updateFunc;
} BgGndDarkmeiro; // size = 0x0170

typedef enum {
    /* 0 */ DARKMEIRO_INVISIBLE_PATH,   // Textures for the invisible path in shadow trial.
    
    /* 1 */ DARKMEIRO_CLEAR_BLOCK,      /* Clear blocks appear when their switch flag is set and
                                           disappear 64 frames after their switch flag is cleared.
                                           Clear blocks with flag 0x3F are always on.             */
                                      
    /* 2 */ DARKMEIRO_BLOCK_TIMER       /* A block timer with switch flag N reacts to switch flags N+1
                                           and N+2 being set, setting its own switch flag and a timer
                                           for 304 frames. There are separate timers for N+1 and N+2,
                                           and the timer sets flag N if either timer is above 64 frames. */
} DarkmeiroType;

#endif
