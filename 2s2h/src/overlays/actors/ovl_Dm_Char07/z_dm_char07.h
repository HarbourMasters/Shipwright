#ifndef Z_DM_CHAR07_H
#define Z_DM_CHAR07_H

#include "global.h"

struct DmChar07;

typedef void (*DmChar07ActionFunc)(struct DmChar07*, PlayState*);

#define DMCHAR07_GET_SPOTLIGHTFLAGS(thisx) (((thisx)->params >> 8) & 0x1F)
#define DMCHAR07_GET_TYPE(thisx) ((thisx)->params & 0xFF)

#define DMCHAR07_STAGE_RIGHT_SPOTLIGHT (1 << 0)   // Deku spotlight
#define DMCHAR07_UPSTAGE_RIGHT_SPOTLIGHT (1 << 1) // Goron spotlight
#define DMCHAR07_STAGE_CENTER_SPOTLIGHT (1 << 2)  // Human spotlight
#define DMCHAR07_UPSTAGE_LEFT_SPOTLIGHT (1 << 3)  // Zora spotlight
#define DMCHAR07_DOWNSTAGE_CENTER_SPOTLIGHT (1 << 4)

typedef enum {
    /* 0 */ DMCHAR07_STAGE,
    /* 1 */ DMCHAR07_CREDITS_STAGE,
    /* 2 */ DMCHAR07_SPOTLIGHTS,
    /* 3 */ DMCHAR07_DISHES
} DmChar07Type;

typedef struct DmChar07 {
    /* 0x000 */ DynaPolyActor dyna;
    /* 0x15C */ UNK_TYPE1 pad15C[0x14C];
    /* 0x2A8 */ DmChar07ActionFunc actionFunc;
    /* 0x2AC */ UNK_TYPE1 pad2AC[0xD];
    /* 0x2B9 */ u8 spotlightFlags;
    /* 0x2BA */ u8 isStage;
} DmChar07; // size = 0x2BC

#endif // Z_DM_CHAR07_H
