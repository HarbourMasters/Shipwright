#ifndef Z_EN_FISHING_H
#define Z_EN_FISHING_H

#include "global.h"

struct EnFishing;

typedef void (*EnFishingActionFunc)(struct EnFishing*, PlayState*);

typedef struct EnFishing {
    /* 0x000 */ Actor actor;
    /* 0x144 */ UNK_TYPE1 unk_144[0x004];
    /* 0x148 */ u8 unk_148;
    /* 0x149 */ u8 unk_149;
    /* 0x14A */ u8 unk_14A;
    /* 0x14C */ s16 unk_14C;
    /* 0x14E */ u8 unk_14E;
    /* 0x14F */ u8 unk_14F;
    /* 0x150 */ s16 unk_150;
    /* 0x152 */ s16 unk_152;
    /* 0x154 */ s16 unk_154;
    /* 0x156 */ s16 unk_156;
    /* 0x158 */ s16 unk_158;
    /* 0x15A */ s16 unk_15A;
    /* 0x15C */ s16 unk_15C;
    /* 0x15E */ s16 unk_15E;
    /* 0x160 */ s16 unk_160;
    /* 0x162 */ s16 unk_162;
    /* 0x164 */ s16 unk_164;
    /* 0x166 */ s16 unk_166;
    /* 0x168 */ s16 unk_168;
    /* 0x16A */ s16 unk_16A;
    /* 0x16C */ s16 unk_16C;
    /* 0x16E */ s16 unk_16E;
    /* 0x170 */ s16 unk_170;
    /* 0x172 */ s16 unk_172[4];
    /* 0x17C */ f32 unk_17C;
    /* 0x180 */ f32 unk_180;
    /* 0x184 */ f32 unk_184;
    /* 0x188 */ f32 unk_188;
    /* 0x18C */ f32 unk_18C;
    /* 0x190 */ f32 unk_190;
    /* 0x194 */ f32 unk_194;
    /* 0x198 */ s16 unk_198;
    /* 0x19A */ s16 unk_19A;
    /* 0x19C */ s16 unk_19C;
    /* 0x1A0 */ f32 unk_1A0;
    /* 0x1A4 */ f32 unk_1A4;
    /* 0x1A8 */ f32 unk_1A8;
    /* 0x1AC */ Vec3f unk_1AC;
    /* 0x1B8 */ Vec3f fishMouthPos;
    /* 0x1C4 */ s16 unk_1C4[3];
    /* 0x1CA */ u8 unk_1CA;
    /* 0x1CB */ u8 unk_1CB;
    /* 0x1CC */ u8 unk_1CC;
    /* 0x1CD */ u8 unk_1CD;
    /* 0x1D0 */ SkelAnime skelAnime;
    /* 0x214 */ LightNode* lightNode;
    /* 0x218 */ LightInfo lightInfo;
    /* 0x228 */ ColliderJntSph collider;
    /* 0x248 */ ColliderJntSphElement colliderElements[12];
} EnFishing; // size = 0x548

#endif // Z_EN_FISHING_H
