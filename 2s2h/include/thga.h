#ifndef THGA_H
#define THGA_H

#include "tha.h"

typedef union TwoHeadGfxArena {
    struct {       // Same as TwoHeadArena, with different types and field names for the head and tail pointers
        /* 0x0 */ size_t size;
        /* 0x4 */ void* start;
        /* 0x8 */ Gfx* p;
        /* 0xC */ void* d;
    };
    /* 0x0 */ TwoHeadArena tha;
} TwoHeadGfxArena; // size = 0x10

void THGA_Init(TwoHeadGfxArena* thga, void* start, size_t size);
void THGA_Destroy(TwoHeadGfxArena* thga);
u32 THGA_IsCrash(TwoHeadGfxArena* thga);
void THGA_Reset(TwoHeadGfxArena* thga);
s32 THGA_GetRemaining(TwoHeadGfxArena* thga);
Gfx* THGA_GetHead(TwoHeadGfxArena* thga);
void THGA_SetHead(TwoHeadGfxArena* thga, Gfx* newHead);
void* THGA_GetTail(TwoHeadGfxArena* thga);
Gfx* THGA_AllocDisplayList(TwoHeadGfxArena* thga, size_t num);
Gfx* THGA_AllocGfx(TwoHeadGfxArena* thga);
Gfx* THGA_AllocGfx2(TwoHeadGfxArena* thga);
void* THGA_AllocTail(TwoHeadGfxArena* thga, size_t size);
Mtx* THGA_AllocMtxArray(TwoHeadGfxArena* thga, size_t num);
Mtx* THGA_AllocMtx(TwoHeadGfxArena* thga);
Vtx* THGA_AllocVtxArray(TwoHeadGfxArena* thga, size_t num);
Vtx* THGA_AllocVtx(TwoHeadGfxArena* thga);

#endif
