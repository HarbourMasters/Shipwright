#ifndef AUDIOMGR_H
#define AUDIOMGR_H

#include "ultra64.h"
#include "unk.h"
#include "irqmgr.h"
#include "scheduler.h"
#include "z64audio.h"

typedef struct {
    /* 0x000 */ IrqMgr* irqMgr;
    /* 0x004 */ SchedContext* sched;
    /* 0x008 */ OSScTask audioTask;
    /* 0x060 */ AudioTask* rspTask;
    /* 0x064 */ OSMesgQueue interruptQueue;
    /* 0x07C */ OSMesg interruptMsgBuf[30];
    /* 0x0F4 */ OSMesgQueue cmdQueue;
    /* 0x10C */ OSMesg cmdMsgBuf[1];
    /* 0x110 */ OSMesgQueue lockQueue;
    /* 0x128 */ OSMesg lockMsgBuf[1];
    /* 0x12C */ UNK_TYPE1 pad_12C[0x4];
    /* 0x130 */ OSThread thread;
} AudioMgr; // size = 0x2E0

void AudioMgr_StopAllSfxExceptSystem(void);
void AudioMgr_Unlock(AudioMgr* audioMgr);
void AudioMgr_Init(AudioMgr* audioMgr, void* stack, OSPri pri, OSId id, SchedContext* sched, IrqMgr* irqMgr);

#endif
