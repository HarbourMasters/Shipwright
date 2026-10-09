#ifndef Z_EN_TAB_H
#define Z_EN_TAB_H

#include "global.h"
#include "overlays/actors/ovl_En_Gm/z_en_gm.h"
#include "assets/objects/object_tab/object_tab.h"

struct EnTab;

typedef void (*EnTabActionFunc)(struct EnTab*, PlayState*);

typedef struct EnTab {
    /* 0x000 */ Actor actor;
    /* 0x144 */ SkelAnime skelAnime;
    /* 0x188 */ EnTabActionFunc actionFunc;
    /* 0x18C */ ColliderCylinder collider;
    /* 0x1D8 */ u8 scheduleResult;
    /* 0x1DC */ s32 msgScriptPos;
    /* 0x1E0 */ Actor* unk_1E0;
    /* 0x1E4 */ EnGm* unk_1E4;
    /* 0x1E8 */ Vec3f unk_1E8[2];
    /* 0x200 */ Vec3s unk_200[2];
    /* 0x20C */ Vec3s jointTable[BARTEN_LIMB_MAX];
    /* 0x284 */ Vec3s morphTable[BARTEN_LIMB_MAX];
    /* 0x2FC */ u16 unk_2FC;
    /* 0x300 */ f32 animPlaySpeed;
    /* 0x304 */ f32 unk_304;
    /* 0x308 */ f32 unk_308;
    /* 0x30C */ s16 unk_30C;
    /* 0x30E */ UNK_TYPE1 unk30E[4];
    /* 0x312 */ s16 unk_312;
    /* 0x314 */ s16 unk_314;
    /* 0x316 */ s16 unk_316;
    /* 0x318 */ s16 unk_318;
    /* 0x31A */ s16 unk_31A;
    /* 0x31C */ s16 unk_31C;
    /* 0x31E */ s16 unk_31E;
    /* 0x320 */ s16 unk_320;
    /* 0x322 */ s16 unk_322;
    /* 0x324 */ s16 unk_324;
    /* 0x328 */ MsgScriptCallback msgScriptCallback;
    /* 0x32C */ s32 animIndex;
    /* 0x330 */ UNK_TYPE1 unk330[4];
    /* 0x334 */ s32 prevTalkState;
    /* 0x338 */ s32 unk_338;
} EnTab; // size = 0x33C

#endif // Z_EN_TAB_H
