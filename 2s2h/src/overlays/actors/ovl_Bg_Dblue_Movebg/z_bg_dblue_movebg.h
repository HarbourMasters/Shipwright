#ifndef Z_BG_DBLUE_MOVEBG_H
#define Z_BG_DBLUE_MOVEBG_H

#include "global.h"

struct BgDblueMovebg;

typedef void (*BgDblueMovebgActionFunc)(struct BgDblueMovebg*, PlayState*);

#define BGDBLUEMOVEBG_GET_F(thisx) ((thisx)->params & 0xF)
#define BGDBLUEMOVEBG_GET_SWITCH_FLAG(thisx) (((thisx)->params >> 4) & 0xFF)
#define BGDBLUEMOVEBG_GET_F000(thisx) (((thisx)->params >> 0xC) & 0xF)

#define BGDBLUEMOVEBG_F_8 8

typedef struct BgDblueMovebg {
    /* 0x000 */ DynaPolyActor dyna;
    /* 0x15C */ BgDblueMovebgActionFunc actionFunc;
    /* 0x160 */ s32 unk_160;
    /* 0x164 */ Gfx* opaDList;
    /* 0x168 */ Gfx* xluDList;
    /* 0x16C */ AnimatedMaterial* texAnim;
    /* 0x170 */ s8 unk_170;
    /* 0x171 */ s8 unk_171;
    /* 0x172 */ u16 unk_172;
    /* 0x174 */ u16 unk_174;
    /* 0x178 */ s32 unk_178;
    /* 0x17C */ UNK_TYPE1 unk17C[2];
    /* 0x17E */ s16 unk_17E;
    /* 0x180 */ s16 unk_180;
    /* 0x184 */ f32 unk_184;
    /* 0x188 */ s16 unk_188;
    /* 0x18A */ s16 unk_18A;
    /* 0x18C */ s16 unk_18C;
    /* 0x18E */ s16 unk_18E;
    /* 0x190 */ Vec3f unk_190;
    /* 0x19C */ Vec3f unk_19C;
    /* 0x1A8 */ Vec3f unk_1A8;
    /* 0x1B4 */ UNK_TYPE1 unk1B4[2];
    /* 0x1B6 */ s16 csIdList[2];
    /* 0x1BC */ s32 unk_1BC;
    /* 0x1C0 */ s32 switchFlag;
    /* 0x1C4 */ s32 unk_1C4;
    /* 0x1C8 */ s32 unk_1C8;
    /* 0x1CC */ s16 unk_1CC;
    /* 0x1CE */ s16 unk_1CE;
    /* 0x1D0 */ s16 unk_1D0;
    /* 0x1D2 */ s16 csId;
    /* 0x1D4 */ f32 unk_1D4;
    /* 0x1D8 */ s16 unk_1D8[2][8];
    /* 0x1F8 */ f32 unk_1F8[2][8];
    /* 0x238 */ Vec3f unk_238[2][8];
    /* 0x2F8 */ Actor* unk_2F8[2];
    /* 0x300 */ u8 unk_300[2][8];
    /* 0x310 */ UNK_TYPE1 unk310[0x60];
} BgDblueMovebg; // size = 0x370

#endif // Z_BG_DBLUE_MOVEBG_H
