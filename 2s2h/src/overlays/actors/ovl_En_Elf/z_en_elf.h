#ifndef Z_EN_ELF_H
#define Z_EN_ELF_H

#include "global.h"
#include "objects/gameplay_keep/gameplay_keep.h"

struct EnElf;

typedef void (*EnElfActionFunc)(struct EnElf*, PlayState*);
typedef void (*EnElfUnkFunc)(struct EnElf*, PlayState*);

#define FAIRY_GET_TYPE(thisx) ((thisx)->params & 0xF)
#define FAIRY_GET_BOOL_PARAM(thisx) ((thisx)->params & 0x100)
#define FAIRY_GET_COLLECTIBLE_FLAG(thisx) (((thisx)->params & 0xFE00) >> 9)

#define FAIRY_PARAMS(type, boolParam, collectibleFlag) (((type) /* & 0xF */) | (((boolParam) & 0x1) << 8) | ((((collectibleFlag) & 0x7F) << 9) & 0xFE00))

typedef enum {
    /* 0x0 */ FAIRY_TYPE_0,
    /* 0x1 */ FAIRY_TYPE_1,
    /* 0x2 */ FAIRY_TYPE_2,
    /* 0x3 */ FAIRY_TYPE_3,
    /* 0x4 */ FAIRY_TYPE_4,
    /* 0x5 */ FAIRY_TYPE_5,
    /* 0x6 */ FAIRY_TYPE_6,
    /* 0x7 */ FAIRY_TYPE_7,
    /* 0x8 */ FAIRY_TYPE_8,
    /* 0x9 */ FAIRY_TYPE_9,
    /* 0xA */ FAIRY_TYPE_10
} FairyType;

typedef struct EnElf {
    /* 0x000 */ Actor actor;
    /* 0x144 */ SkelAnime skelAnime;
    /* 0x188 */ Vec3s jointTable[FAIRY_LIMB_MAX];
    /* 0x1B2 */ Vec3s morphTable[FAIRY_LIMB_MAX];
    /* 0x1DC */ Color_RGBAf innerColor;
    /* 0x1EC */ Color_RGBAf outerColor;
    /* 0x1FC */ LightInfo lightInfoGlow;
    /* 0x20C */ LightNode* lightNodeGlow;
    /* 0x210 */ LightInfo lightInfoNoGlow;
    /* 0x220 */ LightNode* lightNodeNoGlow;
    /* 0x224 */ Vec3f unk_224;
    /* 0x230 */ Actor* elfMsg;
    /* 0x234 */ Actor* unk_234;
    /* 0x238 */ f32 unk_238;
    /* 0x23C */ f32 unk_23C;
    /* 0x240 */ f32 unk_240;
    /* 0x244 */ s16 unk_244;
    /* 0x246 */ s16 unk_246;
    /* 0x248 */ s16 unk_248;
    /* 0x24A */ s16 unk_24A;
    /* 0x24C */ s16 unk_24C;
    /* 0x250 */ f32 unk_250;
    /* 0x254 */ f32 unk_254;
    /* 0x258 */ s16 unk_258;
    /* 0x25A */ u16 timer;
    /* 0x25C */ s16 unk_25C;
    /* 0x25E */ s16 disappearTimer;
    /* 0x260 */ s16 collectibleFlag;
    /* 0x262 */ u16 fairyFlags;
    /* 0x264 */ u16 unk_264;
    /* 0x266 */ u16 unk_266;
    /* 0x268 */ u8 unk_268;
    /* 0x269 */ u8 unk_269;
    /* 0x26C */ EnElfUnkFunc unk_26C;
    /* 0x270 */ EnElfActionFunc actionFunc;
} EnElf; // size = 0x274

#endif // Z_EN_ELF_H
