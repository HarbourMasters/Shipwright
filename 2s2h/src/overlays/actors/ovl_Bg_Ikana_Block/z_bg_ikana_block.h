#ifndef Z_BG_IKANA_BLOCK_H
#define Z_BG_IKANA_BLOCK_H

#include "global.h"

struct BgIkanaBlock;

typedef void (*BgIkanaBlockActionFunc)(struct BgIkanaBlock*, PlayState*);

typedef struct BgIkanaBlock {
    /* 0x000 */ DynaPolyActor dyna;
    /* 0x15C */ AnimatedMaterial* unk_15C;
    /* 0x160 */ BgIkanaBlockActionFunc actionFunc;
    /* 0x164 */ f32* unk_164;
    /* 0x168 */ f32 unk_168;
    /* 0x16C */ f32 unk_16C;
    /* 0x170 */ f32 unk_170;
    /* 0x174 */ Vec3s unk_174;
    /* 0x17A */ u8 unk_17A;
    /* 0x17B */ s8 unk_17B;
    /* 0x17C */ u8 unk_17C;
    /* 0x17D */ s8 unk_17D;
    /* 0x17E */ s8 unk_17E;
    /* 0x17F */ s8 unk_17F;
} BgIkanaBlock; // size = 0x180

#endif // Z_BG_IKANA_BLOCK_H
