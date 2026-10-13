#ifndef Z_BG_DBLUE_BALANCE_H
#define Z_BG_DBLUE_BALANCE_H

#include "global.h"

struct BgDblueBalance;

typedef void (*BgDblueBalanceActionFunc)(struct BgDblueBalance*, PlayState*);

#define BGDBLUEBALANCE_GET_SWITCH_FLAG(thisx) ((thisx)->params & 0x7F)
#define BGDBLUEBALANCE_GET_300(thisx) (((thisx)->params >> 8) & 3)

typedef struct {
    /* 0x00 */ Vec3f unk_00;
    /* 0x0C */ s16 unk_0C;
    /* 0x0E */ u8 unk_0E;
    /* 0x10 */ f32 unk_10;
} BgDblueBalanceStruct; // size = 0x14

typedef struct BgDblueBalance {
    /* 0x000 */ DynaPolyActor dyna;
    /* 0x15C */ struct BgDblueBalance* unk_15C;
    /* 0x160 */ struct BgDblueBalance* unk_160;
    /* 0x164 */ struct BgDblueBalance* unk_164;
    /* 0x168 */ BgDblueBalanceActionFunc actionFunc;
    /* 0x16C */ s32 isSwitchFlagSet2;
    /* 0x170 */ s32 isSwitchFlagSet;
    /* 0x174 */ s16 unk_174;
    /* 0x176 */ s16 unk_176;
    /* 0x178 */ s16 unk_178;
    /* 0x17A */ s16 unk_17A;
    /* 0x17C */ s8 unk_17C;
    /* 0x17D */ s8 unk_17D;
    /* 0x17E */ s8 unk_17E;
    /* 0x17F */ s8 unk_17F;
    /* 0x180 */ s8 isSwitchPressed;
    /* 0x181 */ s8 isHeavySwitchPressed;
    /* 0x182 */ s8 unk_182;
    /* 0x183 */ u8 unk_183;
    /* 0x184 */ s16 unk_184;
    /* 0x186 */ s8 unk_186;
    /* 0x187 */ s8 unk_187;
    /* 0x188 */ BgDblueBalanceStruct unk_188[8];
    /* 0x228 */ f32 unk_228;
} BgDblueBalance; // size = 0x22C

#endif // Z_BG_DBLUE_BALANCE_H
