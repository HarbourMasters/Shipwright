#ifndef Z64DMA_H
#define Z64DMA_H

#include "ultra64.h"

typedef struct {
    /* 0x00 */ uintptr_t    vromAddr;    // VROM address (source)
    /* 0x04 */ void*        dramAddr;    // DRAM address (destination)
    /* 0x08 */ u32          size;        // File Transfer size
    /* 0x0C */ char*        filename;    // Filename for debugging
    /* 0x10 */ s32          line;        // Line for debugging
    /* 0x14 */ s32          unk14;
    /* 0x18 */ OSMesgQueue* notifyQueue; // Message queue for the notification message
    /* 0x1C */ OSMesg       notifyMsg;   // Completion notification message
} DmaRequest; // size = 0x20

typedef struct {
    /* 0x0 */ uintptr_t vromStart;
    /* 0x4 */ uintptr_t vromEnd;
    /* 0x8 */ uintptr_t romStart;
    /* 0xC */ uintptr_t romEnd;
} DmaEntry; // size = 0x10

#endif
