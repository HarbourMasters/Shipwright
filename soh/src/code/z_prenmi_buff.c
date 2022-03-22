#include "global.h"

#define COLD_RESET 0
#define NMI 1

void PreNmiBuff_Init(PreNmiBuff* this) {
    this->resetting = false;

    if (osResetType == COLD_RESET) {
        this->resetCount = 0;
        this->duration = 0;
    } else {
        this->resetCount++;
        this->duration += this->resetTime;
    }

    this->resetTime = 0;
}

void PreNmiBuff_SetReset(PreNmiBuff* this) {
    this->resetting = true;
    this->resetTime = osGetTime();
}

u32 PreNmiBuff_IsResetting(PreNmiBuff* this) {
    return this->resetting;
}
