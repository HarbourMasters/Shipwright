#ifndef Z_BG_INIBS_MOVEBG_H
#define Z_BG_INIBS_MOVEBG_H

#include "global.h"

struct BgInibsMovebg;

#define BG_INIBS_MOVEBG_GET_MODE(thisx) ((u16)((thisx)->params) & 0xF)

typedef struct BgInibsMovebg {
    /* 0x000 */ DynaPolyActor dyna;
    /* 0x15C */ Gfx* opaDList;
    /* 0x160 */ Gfx* xluDList;
    /* 0x164 */ AnimatedMaterial* sandTexAnim;
} BgInibsMovebg; // size = 0x168

#endif // Z_BG_INIBS_MOVEBG_H
