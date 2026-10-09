#ifndef Z_EN_FSN_H
#define Z_EN_FSN_H

#include "global.h"
#include "overlays/actors/ovl_En_GirlA/z_en_girla.h"
#include "objects/object_fsn/object_fsn.h"

#define ENFSN_IS_BACKROOM(thisx) ((thisx)->params & 1)
#define ENFSN_IS_SHOP(thisx) (!((thisx)->params & 1))

#define ENFSN_OFFER_FINAL_PRICE (1 << 6)
#define ENFSN_HAGGLE (1 << 7)
#define ENFSN_ANGRY (1 << 8)
#define ENFSN_CALM_DOWN (1 << 9)


//#region 2S2H [Port] This originally had a +1 because the game incorrectly thought the skel had 18 limbs when it only has 17.
#define ENFSN_LIMB_MAX FSN_LIMB_MAX

struct EnFsn;

typedef void (*EnFsnActionFunc)(struct EnFsn*, PlayState*);

typedef struct EnFsn {
    /* 0x000 */ Actor actor;
    /* 0x144 */ UNK_TYPE1 pad144[0x4C];
    /* 0x190 */ SkelAnime skelAnime;
    /* 0x1D4 */ EnFsnActionFunc actionFunc;
    /* 0x1D8 */ EnFsnActionFunc prevActionFunc; // Used to return to correct browsing function
    /* 0x1DC */ ColliderCylinder collider;
    /* 0x228 */ s16 fidgetTableY[ENFSN_LIMB_MAX];
    /* 0x24E */ s16 fidgetTableZ[ENFSN_LIMB_MAX];
    /* 0x274 */ Vec3s headRot;
    /* 0x27A */ Vec3s torsoRot; // Set but never used
    /* 0x280 */ Vec3s jointTable[ENFSN_LIMB_MAX];
    /* 0x2F2 */ Vec3s morphTable[ENFSN_LIMB_MAX];
    /* 0x364 */ s16 eyeTexIndex;
    /* 0x366 */ s16 blinkTimer;
    /* 0x368 */ s16 cutsceneState;
    /* 0x36A */ s16 csId;
    /* 0x36C */ s16 lookToShopkeeperCsId;
    /* 0x36E */ s16 lookToShelfCsId;
    /* 0x370 */ s16 lookToShopkeeperFromShelfCsId;
    /* 0x372 */ s16 lookToShopkeeperBuyingCsId;
    /* 0x374 */ s16 price;
    /* 0x376 */ u16 textId;
    /* 0x378 */ u8 isSelling;
    /* 0x379 */ u8 cursorIndex;
    /* 0x37C */ s32 getItemId;
    /* 0x380 */ s16 stolenItem1;
    /* 0x382 */ s16 stolenItem2;
    /* 0x384 */ s16 itemIds[3];
    /* 0x38A */ s16 totalSellingItems;
    /* 0x38C */ s16 numSellingItems;
    /* 0x390 */ EnGirlA* items[3];
    /* 0x39C */ s16 delayTimer;
    /* 0x3A0 */ s32 stickAccumX;
    /* 0x3A4 */ s32 stickAccumY;
    /* 0x3A8 */ Vec3f cursorPos;
    /* 0x3B4 */ Color_RGBAu32 cursorColor;
    /* 0x3C4 */ f32 cursorAnimTween;
    /* 0x3C8 */ u8 cursorAnimState;
    /* 0x3C9 */ u8 drawCursor;
    /* 0x3CC */ StickDirectionPrompt stickLeftPrompt;
    /* 0x404 */ StickDirectionPrompt stickRightPrompt;
    /* 0x43C */ f32 arrowAnimTween;
    /* 0x440 */ f32 stickAnimTween;
    /* 0x444 */ u8 arrowAnimState;
    /* 0x445 */ u8 stickAnimState;
    /* 0x448 */ f32 shopItemSelectedTween;
    /* 0x44C */ s16 animIndex;
    /* 0x44E */ u16 flags;
} EnFsn; // size = 0x450

#endif // Z_EN_FSN_H
