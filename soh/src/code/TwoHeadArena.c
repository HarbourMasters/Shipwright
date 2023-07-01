#include "global.h"

#include <string.h>

void THGA_Ct(TwoHeadGfxArena* thga, Gfx* start, size_t size) {
    THA_Ct((TwoHeadArena*)thga, start, size);
}

void THGA_Dt(TwoHeadGfxArena* thga) {
    THA_Dt((TwoHeadArena*)thga);
}

u32 THGA_IsCrash(TwoHeadGfxArena* thga) {
    return THA_IsCrash((TwoHeadArena*)thga);
}

void THGA_Init(TwoHeadGfxArena* thga) {
    THA_Init((TwoHeadArena*)thga);
}

s32 THGA_GetSize(TwoHeadGfxArena* thga) {
    return THA_GetSize((TwoHeadArena*)thga);
}

Gfx* THGA_GetHead(TwoHeadGfxArena* thga) {
    return THA_GetHead((TwoHeadArena*)thga);
}

void THGA_SetHead(TwoHeadGfxArena* thga, Gfx* start) {
    THA_SetHead((TwoHeadArena*)thga, start);
}

Gfx* THGA_GetTail(TwoHeadGfxArena* thga) {
    return THA_GetTail((TwoHeadArena*)thga);
}

Gfx* THGA_AllocStartArray8(TwoHeadGfxArena* thga, u32 count) {
    return THA_AllocStart((TwoHeadArena*)thga, count * 8);
}

Gfx* THGA_AllocStart8(TwoHeadGfxArena* thga) {
    return THGA_AllocStartArray8(thga, 1);
}

Gfx* THGA_AllocStart8Wrapper(TwoHeadGfxArena* thga) {
    return THGA_AllocStart8(thga);
}

Gfx* THGA_AllocEnd(TwoHeadGfxArena* thga, size_t size) {
    return THA_AllocEnd((TwoHeadArena*)thga, size);
}

Gfx* THGA_AllocEndArray64(TwoHeadGfxArena* thga, u32 count) {
    return THGA_AllocEnd(thga, count * 0x40);
}

Gfx* THGA_AllocEnd64(TwoHeadGfxArena* thga) {
    return THGA_AllocEnd(thga, 0x40);
}

Gfx* THGA_AllocEndArray16(TwoHeadGfxArena* thga, u32 count) {
    return THGA_AllocEnd(thga, count * 0x10);
}

Gfx* THGA_AllocEnd16(TwoHeadGfxArena* thga) {
    return THGA_AllocEnd(thga, 0x10);
}

void* THA_GetHead(TwoHeadArena* tha) {
    return tha->head;
}

void THA_SetHead(TwoHeadArena* tha, void* start) {
    tha->head = start;
}

void* THA_GetTail(TwoHeadArena* tha) {
    return tha->tail;
}

void* THA_AllocStart(TwoHeadArena* tha, size_t size) {
    void* start = tha->head;

    tha->head = (uintptr_t)tha->head + size;
    return start;
}

void* THA_AllocStart1(TwoHeadArena* tha) {
    return THA_AllocStart(tha, 1);
}

void* THA_AllocEnd(TwoHeadArena* tha, size_t size) {
    size_t mask;

    if (size == 8) {
        mask = ~7;
    } else if (size == 4 || size == 12) {
        mask = ~3;
    } else if (size == 2 || size == 6 || size == 10 || size == 12 || size == 14) {
        mask = ~1;
    } else {
        mask = (size >= 0x10) ? ~0xF : 0;
    }

    tha->tail = (((uintptr_t)tha->tail & mask) - size) & mask;
    return tha->tail;
}

void* THA_AllocEndAlign16(TwoHeadArena* tha, size_t size) {
    size_t mask = ~0xF;

    tha->tail = (((uintptr_t)tha->tail & mask) - size) & mask;
    return tha->tail;
}

void* THA_AllocEndAlign(TwoHeadArena* tha, size_t size, size_t mask) {
    tha->tail = (((uintptr_t)tha->tail & mask) - size) & mask;
    return tha->tail;
}

s32 THA_GetSize(TwoHeadArena* tha) {
    return (uintptr_t)tha->tail - (uintptr_t)tha->head;
}

u32 THA_IsCrash(TwoHeadArena* tha) {
    return THA_GetSize(tha) < 0;
}

void THA_Init(TwoHeadArena* tha) {
    tha->head = tha->bufp;
    tha->tail = (uintptr_t)tha->bufp + tha->size;
}

void THA_Ct(TwoHeadArena* tha, void* ptr, size_t size) {
    tha->bufp = ptr;
    tha->size = size;
    THA_Init(tha);
}

void THA_Dt(TwoHeadArena* tha) {
    memset(tha, 0, sizeof(TwoHeadArena));
}
