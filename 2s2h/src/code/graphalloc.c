#include "global.h"

Gfx* Graph_GfxPlusOne(Gfx* gfx) {
    return &gfx[1];
}

Gfx* Graph_BranchDlist(Gfx* gfx, Gfx* dst) {
    gSPBranchList(gfx, dst);
    return dst;
}

/**
 * Allocates a structure of `size` into the display list buffer`gfx`,
 * returning a pointer to the start of the buffer.
 * Since the alloc may not itself be display list commands, a BranchList
 * command is used to step over this region.
 */
void* Graph_DlistAlloc(Gfx** gfx, size_t size) {
    u8* start;
    Gfx* end;

    size = ALIGN8(size);
    start = (u8*)&(*gfx)[1];
    end = (Gfx*)(start + size);
    gSPBranchList(*gfx, end);

    *gfx = end;
    return start;
}
