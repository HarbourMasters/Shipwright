#ifndef Z_EN_NIW_LADY_H
#define Z_EN_NIW_LADY_H

#include "ultra64.h"
#include "global.h"

struct EnNiwLady;

typedef void (*EnNiwLadyActionFunc)(struct EnNiwLady*, PlayState*);

typedef struct EnNiwLady {
    /* 0x0000 */ Actor actor;
    /* 0x014C */ SkelAnime skelAnime;
    /* 0x0190 */ Vec3s jointTable[16];
    /* 0x01F0 */ Vec3s morphTable[16];
    /* 0x0250 */ EnNiwLadyActionFunc actionFunc;
    /* 0x0254 */ Vec3s unk_254;
    /* 0x025A */ Vec3s unk_25A;
    /* 0x0260 */ s16 unusedTimer;
    /* 0x0262 */ s16 unk_262; // "message_end_code"
    /* 0x0264 */ s16 unusedTimer2;
    /* 0x0266 */ s16 unusedRandomTimer;
    /* 0x0268 */ s16 cuccosInPen;
    /* 0x026A */ s16 unk_26A;
    /* 0x026C */ s16 unk_26C;
    /* 0x026E */ s16 unk_26E;
    /* 0x0270 */ s16 unk_270;
    /* 0x0272 */ u8 unk_272;
    /* 0x0273 */ u8 unk_273;
    /* 0x0274 */ u8 unk_274;
    /* 0x0275 */ u8 unk_275;
    /* 0x0276 */ u8 unk_276;
    /* 0x0277 */ u8 unk_277;
    /* 0x0278 */ s16 unk_278;
    /* 0x027A */ s16 unk_27A;
    /* 0x027C */ s16 faceState;
    /* 0x027E */ s16 unk_27E;
    /* 0x0280 */ s8 objectAneIndex;
    /* 0x0281 */ s8 objectOsAnimeIndex;
    /* 0x0284 */ s32 getItemId;
    /* 0x0288 */ struct_80034A14_arg1 unk_288;
    /* 0x02B0 */ ColliderCylinder collider;
    /* 0x02FC */ GetItemEntry getItemEntry;
} EnNiwLady; // size = 0x02FC

#endif
