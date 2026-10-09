#ifndef Z_EN_HORSE_GAME_CHECK_H
#define Z_EN_HORSE_GAME_CHECK_H

#include "global.h"
#include "overlays/actors/ovl_En_Horse/z_en_horse.h"

struct EnHorseGameCheck;

typedef s32 (*EnHorseGameCheckUnkFunc)(struct EnHorseGameCheck*, PlayState*);

#define ENHORSEGAMECHECK_GET_FF(thisx) ((thisx)->params & 0xFF)
#define ENHORSEGAMECHECK_GET_FF00(thisx) (((thisx)->params & 0xFF00) >> 8)

typedef enum {
    /*  0 */ ENHORSEGAMECHECK_FF_0,
    /*  1 */ ENHORSEGAMECHECK_FF_1,
    /*  2 */ ENHORSEGAMECHECK_FF_2,
    /*  3 */ ENHORSEGAMECHECK_FF_3,
    /*  4 */ ENHORSEGAMECHECK_FF_4,
    /*  5 */ ENHORSEGAMECHECK_FF_5,
    /*  6 */ ENHORSEGAMECHECK_FF_6,
    /*  7 */ ENHORSEGAMECHECK_FF_7,
    /*  8 */ ENHORSEGAMECHECK_FF_8,
    /*  9 */ ENHORSEGAMECHECK_FF_9,
    /* 10 */ ENHORSEGAMECHECK_FF_MAX
} EnHorseGameCheckParam;

typedef struct EnHorseGameCheck {
    /* 0x000 */ DynaPolyActor dyna;
    /* 0x15C */ s32 unk_15C;
    /* 0x160 */ s32 unk_160;
    /* 0x164 */ s32 unk_164;
    /* 0x168 */ s32 unk_168;
    /* 0x16C */ EnHorse* horse1;
    /* 0x170 */ EnHorse* horse2;
    /* 0x174 */ s32 unk_174;
    /* 0x178 */ UNK_TYPE1 pad178[0x4];
    /* 0x17C */ s32 unk_17C;
    /* 0x180 */ UNK_TYPE1 pad180[0x38];
} EnHorseGameCheck; // size = 0x1B8

#endif // Z_EN_HORSE_GAME_CHECK_H
