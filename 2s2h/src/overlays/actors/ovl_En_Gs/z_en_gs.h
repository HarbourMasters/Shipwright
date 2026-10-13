#ifndef Z_EN_GS_H
#define Z_EN_GS_H

#include "global.h"

struct EnGs;

typedef void (*EnGsActionFunc)(struct EnGs*, PlayState*);

#define ENGS_GET_1F(thisx) ((thisx)->params & 0x1F)
#define ENGS_GET_SWITCH_FLAG(thisx) (((thisx)->params >> 5) & 0x7F)
#define ENGS_GET_F000(thisx) (((thisx)->params >> 0xC) & 0xF)

typedef enum {
    /* 0 */ ENGS_0,
    /* 1 */ ENGS_1,
    /* 2 */ ENGS_2,
    /* 3 */ ENGS_3
} EnGsParam;

typedef struct EnGs {
    /* 0x000 */ Actor actor;
    /* 0x144 */ ColliderCylinder collider;
    /* 0x190 */ EnGsActionFunc actionFunc;
    /* 0x194 */ s8 unk_194;
    /* 0x195 */ u8 unk_195;
    /* 0x196 */ u8 switchFlag;
    /* 0x197 */ s8 unk_197;
    /* 0x198 */ s16 unk_198;
    /* 0x19A */ s16 unk_19A;
    /* 0x19C */ s8 unk_19C;
    /* 0x19D */ u8 unk_19D;
    /* 0x19E */ Vec3s unk_19E[3];
    /* 0x1B0 */ Vec3f unk_1B0[2];
    /* 0x1C8 */ UNK_TYPE1 unk1C8[0xC];
    /* 0x1D4 */ s16 unk_1D4;
    /* 0x1D6 */ UNK_TYPE1 unk1D6[0x6];
    /* 0x1DC */ f32 unk_1DC;
    /* 0x1E0 */ f32 unk_1E0;
    /* 0x1E4 */ f32 unk_1E4;
    /* 0x1E8 */ f32 unk_1E8;
    /* 0x1EC */ f32 unk_1EC;
    /* 0x1F0 */ f32 unk_1F0;
    /* 0x1F4 */ Color_RGB8 unk_1F4;
    /* 0x1F7 */ Color_RGB8 unk_1F7;
    /* 0x1FA */ Color_RGB8 unk_1FA;
    /* 0x200 */ f32 unk_200;
    /* 0x204 */ s32 unk_204;
    /* 0x208 */ s32 unk_208;
    /* 0x20C */ s32 getItemId;
    /* 0x210 */ u16 unk_210;
    /* 0x212 */ s16 csIdList[2];
    /* 0x216 */ s16 unk_216;
    /* 0x218 */ s16 unk_218;
    /* 0x21A */ s16 unk_21A;
    /* 0x21C */ s16 quakeY;
    /* 0x21E */ s16 quakeDuration;
} EnGs; // size = 0x220

#endif // Z_EN_GS_H
