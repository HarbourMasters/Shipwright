#include "global.h"
#include "BenPort.h"

#define COLD_RESET 0
#define NMI 1

typedef struct {
    /* 0x00 */ u32 resetting;
    /* 0x04 */ u32 resetCount;
    /* 0x08 */ OSTime duration;
    /* 0x10 */ OSTime resetTime;
} NmiBuff; // size >= 0x18

NmiBuff* gNMIBuffer;

void Nmi_Init(void) {
    gNMIBuffer = (NmiBuff*)osAppNMIBuffer;
    gNMIBuffer->resetting = false;

    if (osResetType == COLD_RESET) {
        gNMIBuffer->resetCount = 0;
        gNMIBuffer->duration = 0;
    } else {
        gNMIBuffer->resetCount++;
        gNMIBuffer->duration += gNMIBuffer->resetTime;
    }

    gNMIBuffer->resetTime = 0;
}

void Nmi_SetPrenmiStart(void) {
    gNMIBuffer->resetting = true;
    gNMIBuffer->resetTime = osGetTime();
}

u32 Nmi_GetPrenmiHasStarted(void) {
    return gNMIBuffer->resetting;
}
