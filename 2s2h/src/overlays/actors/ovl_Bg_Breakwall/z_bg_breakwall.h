#ifndef Z_BG_BREAKWALL_H
#define Z_BG_BREAKWALL_H

#include "global.h"

struct BgBreakwall;

typedef void (*BgBreakwallActionFunc)(struct BgBreakwall*, PlayState*);
typedef bool (*BgBreakwallUnkFunc)(struct BgBreakwall*, PlayState*);
typedef void (*BgBreakwallUnkFunc2)(struct BgBreakwall*, PlayState*);

#define BGBREAKWALL_GET_F(thisx) ((thisx)-> params & 0xF)
#define BGBREAKWALL_SWITCH_FLAG(thisx) (((thisx)-> params & 0xFE00) >> 9)

typedef enum {
    /*  7 */ BGBREAKWALL_F_7 = 7,
    /*  9 */ BGBREAKWALL_F_9 = 9,
    /* 11 */ BGBREAKWALL_F_11 = 11
} BgBreakwallParam;

typedef struct BgBreakwall {
    /* 0x000 */ DynaPolyActor dyna;
    /* 0x15C */ s8 objectSlot;
    /* 0x15D */ u8 switchFlag;
    /* 0x15E */ u8 unk_15E;
    /* 0x160 */ BgBreakwallActionFunc actionFunc;
} BgBreakwall; // size = 0x164

#endif // Z_BG_BREAKWALL_H
