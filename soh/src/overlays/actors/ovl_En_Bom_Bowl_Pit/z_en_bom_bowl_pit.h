#ifndef Z_EN_BOM_BOWL_PIT_H
#define Z_EN_BOM_BOWL_PIT_H

#include "ultra64.h"
#include "global.h"
#include "overlays/actors/ovl_En_Ex_Item/z_en_ex_item.h"

struct EnBomBowlPit;

typedef void (*EnBomBowlPitActionFunc)(struct EnBomBowlPit*, PlayState*);

typedef struct EnBomBowlPit {
    /* 0x0000 */ Actor actor;
    /* 0x014C */ EnBomBowlPitActionFunc actionFunc;
    /* 0x0150 */ u16 unk_150;
    /* 0x0152 */ s16 camId;
    /* 0x0154 */ s16 unk_154;
    /* 0x0156 */ s16 exItemDone; // Set by EnExItem?
    /* 0x0158 */ s16 timer;
    /* 0x015A */ s16 prizeIndex;
    /* 0x015C */ s16 start;
    /* 0x0160 */ s32 getItemId;
    /* 0x0164 */ u8 status;
    /* 0x0168 */ Vec3f unk_168; // set and not used?
    /* 0x0174 */ Vec3f unk_174; // set and not used?
    /* 0x0180 */ Vec3f unk_180; // camera at (start)
    /* 0x018C */ Vec3f unk_18C; // camera eye (start)
    /* 0x0198 */ Vec3f unk_198; // camera eye (end)
    /* 0x01A4 */ Vec3f unk_1A4; // camera eye (scales)
    /* 0x01B0 */ Vec3f unk_1B0; // camera eye (maxsteps)
    /* 0x01BC */ Vec3f unk_1BC; // camera at (end)
    /* 0x01C8 */ Vec3f unk_1C8; // camera at (scales)
    /* 0x01D4 */ Vec3f unk_1D4; // camera eye (maxsteps)
    /* 0x01E0 */ EnExItem* exItem;
    /* 0x01E4 */ char unk_1E4[0x3520];
    /*        */ GetItemEntry getItemEntry;
} EnBomBowlPit; // size = 0x3704

#endif
