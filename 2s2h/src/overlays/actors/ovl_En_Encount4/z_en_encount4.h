#ifndef Z_EN_ENCOUNT4_H
#define Z_EN_ENCOUNT4_H

#include "global.h"
#include "overlays/actors/ovl_Bg_Fire_Wall/z_bg_fire_wall.h"
#include "overlays/actors/ovl_En_Bsb/z_en_bsb.h"
#include "overlays/actors/ovl_En_Skb/z_en_skb.h"

struct EnEncount4;

typedef void (*EnEncount4ActionFunc)(struct EnEncount4*, PlayState*);

#define ENCOUNT4_GET_SWITCH_FLAG(thisx) ((thisx)->params & 0x7F)
#define ENCOUNT4_GET_F000(thisx) (((thisx)->params >> 0xC) & 0xF)

#define ENCOUNT4_SWITCH_FLAG_NONE 0x7F

typedef struct EnEncount4 {
    /* 0x000 */ Actor actor;
    /* 0x144 */ EnEncount4ActionFunc actionFunc;
    /* 0x148 */ s16 unk_148;
    /* 0x14A */ s16 switchFlag;
    /* 0x14C */ s16 unk_14C;
    /* 0x14E */ s16 unk_14E;
    /* 0x150 */ s16 timer;
    /* 0x154 */ EnBsb* captainKeeta;
} EnEncount4; // size = 0x158

#endif // Z_EN_ENCOUNT4_H
