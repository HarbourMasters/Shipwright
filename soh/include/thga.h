#ifndef THGA_H
#define THGA_H

#include "tha.h"

// TODO: Update the type to upstream
typedef struct TwoHeadGfxArena {
    /* 0x0000 */ u32    size;
    /* 0x0004 */ Gfx*   bufp;
    /* 0x0008 */ Gfx*   p;
    /* 0x000C */ Gfx*   d;
} TwoHeadGfxArena; // size = 0x10

EXTERN_C void THGA_Ct(TwoHeadGfxArena* thga, Gfx* start, size_t size);
EXTERN_C void THGA_Dt(TwoHeadGfxArena* thga);
EXTERN_C u32 THGA_IsCrash(TwoHeadGfxArena* thga);
EXTERN_C void THGA_Init(TwoHeadGfxArena* thga);
EXTERN_C s32 THGA_GetSize(TwoHeadGfxArena* thga);
EXTERN_C Gfx* THGA_GetHead(TwoHeadGfxArena* thga);
EXTERN_C void THGA_SetHead(TwoHeadGfxArena* thga, Gfx* start);
EXTERN_C Gfx* THGA_GetTail(TwoHeadGfxArena* thga);
EXTERN_C Gfx* THGA_AllocStartArray8(TwoHeadGfxArena* thga, u32 count);
EXTERN_C Gfx* THGA_AllocStart8(TwoHeadGfxArena* thga);
EXTERN_C Gfx* THGA_AllocStart8Wrapper(TwoHeadGfxArena* thga);
EXTERN_C Gfx* THGA_AllocEnd(TwoHeadGfxArena* thga, size_t size);
EXTERN_C Gfx* THGA_AllocEndArray64(TwoHeadGfxArena* thga, u32 count);
EXTERN_C Gfx* THGA_AllocEnd64(TwoHeadGfxArena* thga);
EXTERN_C Gfx* THGA_AllocEndArray16(TwoHeadGfxArena* thga, u32 count);
EXTERN_C Gfx* THGA_AllocEnd16(TwoHeadGfxArena* thga);

#endif