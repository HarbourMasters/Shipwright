#ifndef SLOWLY_H
#define SLOWLY_H

#include "ultra64.h"

#define SLOWLY_STATUS_DONE (1 << 0)
#define SLOWLY_STATUS_STARTED (1 << 1)

typedef void (*SlowlyCallbackZero)(void);
typedef void (*SlowlyCallbackOne)(void*);
typedef void (*SlowlyCallbackTwo)(void*, void*);

typedef union {
    SlowlyCallbackZero zero;
    SlowlyCallbackOne one;
    SlowlyCallbackTwo two;
} SlowlyCallback; // size = 0x4

typedef struct {
    /* 0x000 */ OSThread thread;
    /* 0x1B0 */ u8 argCount;
    /* 0x1B1 */ u8 status;
    /* 0x1B4 */ SlowlyCallback callback;
    /* 0x1B8 */ void* arg0;
    /* 0x1BC */ void* arg1;
} SlowlyMgr; // size = 0x1C0

void Slowly_Init(SlowlyMgr* slowly, void* stack, SlowlyCallbackTwo callback, void* arg0, void* arg1);
void Slowly_Destroy(SlowlyMgr* slowly);

#endif
