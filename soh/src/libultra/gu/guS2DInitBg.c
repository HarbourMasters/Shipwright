#include "global.h"

void guS2DInitBg(uObjBg* bg) {
    u32 size;
    s32 tmem = (bg->b.imageFmt == G_IM_FMT_CI) ? 0x100 : 0x200;
    u16 shift = (6 - bg->b.imageSiz);

    if (bg->b.imageLoad == G_BGLT_LOADBLOCK) {
        bg->b.tmemW = bg->b.imageW >> shift;
        bg->b.tmemH = (tmem / bg->b.tmemW) * 4;
        bg->b.tmemSizeW = bg->b.tmemW * 2;
        bg->b.tmemSize = bg->b.tmemH * bg->b.tmemSizeW;
        bg->b.tmemLoadSH = (bg->b.tmemSize >> 1) - 1;
        bg->b.tmemLoadTH = (0x7FF / bg->b.tmemW) + 1;
    } else { // G_BGLT_LOADTILE
        bg->b.tmemW = (bg->b.frameW >> shift) + 3;
        bg->b.tmemH = (tmem / bg->b.tmemW) * 4;
        bg->b.tmemSizeW = (bg->b.imageW >> shift) * 2;

        size = bg->b.tmemH * bg->b.tmemSizeW;
        bg->b.tmemSize = (size >> 16);
        bg->b.tmemLoadSH = (size >> 0) & 0xFFFF;
        bg->b.tmemLoadTH = bg->b.tmemH - 1;
    }
}
