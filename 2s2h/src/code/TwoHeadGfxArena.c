/**
 * @file TwoHeadGfxArena.c
 *
 * This file implements a particular use of the double-ended stack allocator from TwoHeadArena.c for graphics data.
 *
 * Display list commands are allocated from the head while other graphics data such as matrices and vertices are
 * allocated from the tail end.
 *
 * @see TwoHeadArena.c
 */

#include "thga.h"
#include "alignment.h"
#include "functions.h"

void THGA_Init(TwoHeadGfxArena* thga, void* start, size_t size) {
    THA_Init(&thga->tha, start, size);
}

void THGA_Destroy(TwoHeadGfxArena* thga) {
    THA_Destroy(&thga->tha);
}

u32 THGA_IsCrash(TwoHeadGfxArena* thga) {
    return THA_IsCrash(&thga->tha);
}

void THGA_Reset(TwoHeadGfxArena* thga) {
    THA_Reset(&thga->tha);
}

s32 THGA_GetRemaining(TwoHeadGfxArena* thga) {
    return THA_GetRemaining(&thga->tha);
}

Gfx* THGA_GetHead(TwoHeadGfxArena* thga) {
    return THA_GetHead(&thga->tha);
}

void THGA_SetHead(TwoHeadGfxArena* thga, Gfx* newHead) {
    THA_SetHead(&thga->tha, newHead);
}

void* THGA_GetTail(TwoHeadGfxArena* thga) {
    return THA_GetTail(&thga->tha);
}

/**
 * Allocates a display list of `num` Gfx commands to the head of the Two Head Gfx Arena.
 */
Gfx* THGA_AllocDisplayList(TwoHeadGfxArena* thga, size_t num) {
    return THA_AllocHead(&thga->tha, num * sizeof(Gfx));
}

/**
 * Allocates a single Gfx command to the head of the Two Head Gfx Arena.
 */
Gfx* THGA_AllocGfx(TwoHeadGfxArena* thga) {
    return THGA_AllocDisplayList(thga, 1);
}

/**
 * Identical to `THGA_AllocGfx`
 *
 * @see THGA_AllocGfx
 */
Gfx* THGA_AllocGfx2(TwoHeadGfxArena* thga) {
    return THGA_AllocGfx(thga);
}

/**
 * Allocates to the end of the Two Head Gfx Arena. Intended for data complementary to the display lists such as
 * matrices and vertices that are only needed for a single graphics task.
 */
void* THGA_AllocTail(TwoHeadGfxArena* thga, size_t size) {
    return THA_AllocTail(&thga->tha, size);
}

/**
 * Allocates `num` matrices to the tail end of the Two Head Gfx Arena.
 */
Mtx* THGA_AllocMtxArray(TwoHeadGfxArena* thga, size_t num) {
    return THGA_AllocTail(thga, num * sizeof(Mtx));
}

/**
 * Allocates a matrix to the tail end of the Two Head Gfx Arena.
 */
Mtx* THGA_AllocMtx(TwoHeadGfxArena* thga) {
    return THGA_AllocTail(thga, sizeof(Mtx));
}

/**
 * Allocates `num` vertices to the tail end of the Two Head Gfx Arena.
 */
Vtx* THGA_AllocVtxArray(TwoHeadGfxArena* thga, size_t num) {
    return THGA_AllocTail(thga, num * sizeof(Vtx));
}

/**
 * Allocates a vertex to the tail end of the Two Head Gfx Arena.
 */
Vtx* THGA_AllocVtx(TwoHeadGfxArena* thga) {
    return THGA_AllocTail(thga, sizeof(Vtx));
}
