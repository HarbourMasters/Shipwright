#ifndef Z_OBJ_HUNSUI_H
#define Z_OBJ_HUNSUI_H

#include "global.h"
#include "overlays/actors/ovl_Bg_Dblue_Movebg/z_bg_dblue_movebg.h"

struct ObjHunsui;

typedef void (*ObjHunsuiActionFunc)(struct ObjHunsui*, PlayState*);

#define OBJHUNSUI_GET_SWITCH_FLAG(thisx) ((thisx)->params & 0x7F)
#define OBJHUNSUI_GET_F80(thisx) (((thisx)->params >> 7) & 0x1F)
#define OBJHUNSUI_GET_F000(thisx) (((thisx)->params >> 0xC) & 0xF)

typedef enum {
    /* 0 */ OBJHUNSUI_F000_0,
    /* 1 */ OBJHUNSUI_F000_1,
    /* 2 */ OBJHUNSUI_F000_2,
    /* 3 */ OBJHUNSUI_F000_3,
    /* 4 */ OBJHUNSUI_F000_4,
    /* 5 */ OBJHUNSUI_F000_5,
    /* 6 */ OBJHUNSUI_F000_6
} ObjHunsuiParam;

typedef struct ObjHunsui {
    /* 0x000 */ DynaPolyActor dyna;
    /* 0x15C */ ObjHunsuiActionFunc actionFunc;
    /* 0x160 */ s16 unk_160;
    /* 0x164 */ s32 unk_164;
    /* 0x168 */ s32 switchFlag;
    /* 0x16C */ s8 unk_16C;
    /* 0x16D */ s8 unk_16D;
    /* 0x16E */ s8 unk_16E;
    /* 0x170 */ s16 csIdList[1];
    /* 0x172 */ u16 unk_172;
    /* 0x174 */ f32 unk_174;
    /* 0x178 */ f32 unk_178;
    /* 0x17C */ s16 csId;
    /* 0x180 */ s32 unk_180;
    /* 0x184 */ s16 unk_184;
    /* 0x186 */ s16 unk_186;
    /* 0x188 */ s16 unk_188;
    /* 0x18A */ s16 unk_18A;
    /* 0x18C */ s16 unk_18C;
    /* 0x190 */ f32 unk_190;
    /* 0x194 */ f32 unk_194;
    /* 0x198 */ f32 unk_198;
    /* 0x19C */ f32 unk_19C;
    /* 0x1A0 */ f32 unk_1A0;
    /* 0x1A4 */ s16 unk_1A4;
    /* 0x196 */ UNK_TYPE1 unk1A6[0x6];
    /* 0x1AC */ f32 unk_1AC;
    /* 0x1B0 */ f32 unk_1B0;
    /* 0x1B4 */ BgDblueMovebg* unk_1B4;
} ObjHunsui; // size = 0x1B8

#endif // Z_OBJ_HUNSUI_H
