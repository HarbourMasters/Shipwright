#ifndef Z_EN_SEKIHI_H
#define Z_EN_SEKIHI_H

#include "global.h"
#include "overlays/actors/ovl_En_Si/z_en_si.h"


#define ENSIKIHI_GET_TYPE(thisx) ((thisx)->params & 0xF)

struct EnSekihi;

typedef void (*EnSekihiActionFunc)(struct EnSekihi*, PlayState*);

typedef struct EnSekihi {
    /* 0x000 */ DynaPolyActor dyna;
    /* 0x15C */ Gfx* opaDList;
    /* 0x160 */ Gfx* xluDList;
    /* 0x164 */ s8 objectSlot;
    /* 0x168 */ EnSekihiActionFunc actionFunc;
} EnSekihi; // size = 0x16C

typedef enum SekihiType {
    /* 0x0 */ SEKIHI_TYPE_0,
    /* 0x1 */ SEKIHI_TYPE_1,
    /* 0x2 */ SEKIHI_TYPE_2,
    /* 0x3 */ SEKIHI_TYPE_3,
    /* 0x4 */ SEKIHI_TYPE_4,
    /* 0x5 */ SEKIHI_TYPE_MAX
} SekihiType;

#endif // Z_EN_SEKIHI_H
