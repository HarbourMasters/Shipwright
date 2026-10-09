#ifndef Z64PRENMI_H
#define Z64PRENMI_H

#include "z64game.h"

typedef struct {
    /* 0x00 */ GameState state;
    /* 0xA4 */ u32 timer;
    /* 0xA8 */ s32 unk_A8;
} PreNMIState; // size = 0xAC

void PreNMI_Destroy(GameState* thisx);
void PreNMI_Init(GameState* thisx);

#endif
