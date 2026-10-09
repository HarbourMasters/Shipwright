#ifndef Z_BG_HAKUGIN_ELVPOLE_H
#define Z_BG_HAKUGIN_ELVPOLE_H

#include "global.h"

struct BgHakuginElvpole;

#define BGHAKUGINELVPOLE_GET_SWITCH_FLAG(thisx) ((thisx)->params & 0x7F)

typedef void (*BgHakuginElvpoleActionFunc)(struct BgHakuginElvpole*, PlayState*);

typedef struct BgHakuginElvpole {
    /* 0x000 */ DynaPolyActor dyna;
    /* 0x15C */ s16 unk_15C;
    /* 0x15E */ s16 unk_15E;
    /* 0x160 */ u16 unk_160;
    /* 0x164 */ BgHakuginElvpoleActionFunc actionFunc;
} BgHakuginElvpole; // size = 0x168

#endif // Z_BG_HAKUGIN_ELVPOLE_H
