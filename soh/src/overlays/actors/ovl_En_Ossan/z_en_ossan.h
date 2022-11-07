#ifndef Z_EN_OSSAN_H
#define Z_EN_OSSAN_H

#include "ultra64.h"
#include "global.h"
#include "overlays/actors/ovl_En_Tana/z_en_tana.h"
#include "overlays/actors/ovl_En_GirlA/z_en_girla.h"

struct EnOssan;

typedef void (*EnOssanActionFunc)(struct EnOssan*, PlayState*);
typedef void (*EnOssanTalkOwnerFunc)(PlayState*);
typedef void (*EnOssanInitFunc)(struct EnOssan*, PlayState*);
typedef s16 (*EnOssanGetGirlAParamsFunc)(s16);
typedef void (*EnOssanStateFunc)(struct EnOssan*, PlayState*, Player*);

#define ColChanMix(c1, c2, m) (c1 - (s32)(c2 * m)) & 0xFF

typedef struct EnOssan {
    /* 0x0000 */ Actor actor;
    /* 0x014C */ SkelAnime skelAnime;
    /* 0x0190 */ EnOssanActionFunc actionFunc;
    /* 0x0194 */ void (*obj3ToSeg6Func)(struct EnOssan*, PlayState*);
    /* 0x0198 */ ColliderCylinder collider; // unused
    /* 0x01E4 */ s16 timer;
    /* 0x01E6 */ s16 delayTimer;
    /* 0x01E8 */ s8 objBankIndex1;
    /* 0x01E9 */ s8 objBankIndex2;
    /* 0x01EA */ s8 objBankIndex3;
    /* 0x01EB */ u8 happyMaskShopState;
    /* 0x01EC */ u8 happyMaskShopkeeperEyeIdx;
    /* 0x01EE */ s16 headRot;
    /* 0x01F0 */ s16 headTargetRot;
    /* 0x01F2 */ s16 eyeTextureIdx;
    /* 0x01F4 */ s16 blinkTimer;
    /* 0x01F8 */ void (*blinkFunc)(struct EnOssan*);
    /* 0x01FC */ s16 stateFlag;
    /* 0x01FE */ s16 tempStateFlag;
    /* 0x0200 */ EnGirlA* shelfSlots[8];
    // Shelves are indexed as such:
    /* 7 5  3 1 */
    /* 6 4  2 0 */
    /* 0x0220 */ EnTana* shelves;
    /* 0x0224 */ s32 stickAccumX;
    /* 0x0228 */ s32 stickAccumY;
    /* 0x022C */ u8 moveHorizontal;
    /* 0x022D */ u8 moveVertical;
    /* 0x0230 */ f32 cursorX;
    /* 0x0234 */ f32 cursorY;
    /* 0x0238 */ f32 cursorZ;
    /* 0x023C */ u32 cursorColorR;
    /* 0x0240 */ u32 cursorColorG;
    /* 0x0244 */ u32 cursorColorB;
    /* 0x0248 */ u32 cursorColorA;
    /* 0x024C */ f32 cursorAnimTween;
    /* 0x0250 */ u8 cursorAnimState;
    /* 0x0251 */ u8 drawCursor;
    /* 0x0252 */ u8 cursorIndex;
    /* 0x0254 */ StickDirectionPrompt stickLeftPrompt;
    /* 0x028C */ StickDirectionPrompt stickRightPrompt;
    /* 0x02C4 */ f32 arrowAnimTween;
    /* 0x02C4 */ f32 stickAnimTween;
    /* 0x02CC */ u8 arrowAnimState;
    /* 0x02CD */ u8 stickAnimState;
    /* 0x02D0 */ f32 shopItemSelectedTween;
    /* 0x02D4 */ f32 cameraFaceAngle; // stored in degrees
} EnOssan; // size = 0x02D8

typedef enum {
    /* 00 */ OSSAN_TYPE_KOKIRI,
    /* 01 */ OSSAN_TYPE_KAKARIKO_POTION,
    /* 02 */ OSSAN_TYPE_BOMBCHUS,
    /* 03 */ OSSAN_TYPE_MARKET_POTION,
    /* 04 */ OSSAN_TYPE_BAZAAR,
    /* 05 */ OSSAN_TYPE_ADULT,
    /* 06 */ OSSAN_TYPE_TALON,
    /* 07 */ OSSAN_TYPE_ZORA,
    /* 08 */ OSSAN_TYPE_GORON,
    /* 09 */ OSSAN_TYPE_INGO,
    /* 10 */ OSSAN_TYPE_MASK
} OssanType;

typedef enum {
    /* 00 */ OSSAN_STATE_IDLE,
    /* 01 */ OSSAN_STATE_START_CONVERSATION,
    /* 02 */ OSSAN_STATE_FACING_SHOPKEEPER,
    /* 03 */ OSSAN_STATE_TALKING_TO_SHOPKEEPER,
    /* 04 */ OSSAN_STATE_LOOK_SHELF_LEFT,
    /* 05 */ OSSAN_STATE_LOOK_SHELF_RIGHT,
    /* 06 */ OSSAN_STATE_BROWSE_LEFT_SHELF,
    /* 07 */ OSSAN_STATE_BROWSE_RIGHT_SHELF,
    /* 08 */ OSSAN_STATE_LOOK_SHOPKEEPER, // From looking at shelf
    /* 09 */ OSSAN_STATE_SELECT_ITEM,     // Select most items
    /* 10 */ OSSAN_STATE_SELECT_ITEM_MILK_BOTTLE,
    /* 11 */ OSSAN_STATE_SELECT_ITEM_WEIRD_EGG,
    /* 12 */ OSSAN_STATE_SELECT_ITEM_UNIMPLEMENTED, // Handles two unfinished shop items
    /* 13 */ OSSAN_STATE_SELECT_ITEM_BOMBS,
    /* 14 */ OSSAN_STATE_CANT_GET_ITEM,
    /* 15 */ OSSAN_STATE_GIVE_ITEM_FANFARE, // Give Item, hold it up with fanfare
    /* 16 */ OSSAN_STATE_ITEM_PURCHASED,
    /* 17 */ OSSAN_STATE_CONTINUE_SHOPPING_PROMPT,
    /* 18 */ OSSAN_STATE_GIVE_LON_LON_MILK,
    /* 19 */ OSSAN_STATE_DISPLAY_ONLY_BOMB_DIALOG,          // Turn to shopkeeper, talk about fake bombs
    /* 20 */ OSSAN_STATE_WAIT_FOR_DISPLAY_ONLY_BOMB_DIALOG, // Can't Get Goron City Bombs
    /* 21 */ OSSAN_STATE_21,                                // Unused
    /* 22 */ OSSAN_STATE_22,                                // Follows OSSAN_STATE_21
    /* 23 */ OSSAN_STATE_QUICK_BUY,
    /* 24 */ OSSAN_STATE_SELECT_ITEM_MASK,
    /* 25 */ OSSAN_STATE_LEND_MASK_OF_TRUTH, // First time all masks are sold
    /* 26 */ OSSAN_STATE_DISCOUNT_DIALOG     // Hylian Shield Discount
} EnOssanState;

typedef enum {
    OSSAN_HAPPY_STATE_REQUEST_PAYMENT_KEATON_MASK,
    OSSAN_HAPPY_STATE_REQUEST_PAYMENT_SPOOKY_MASK,
    OSSAN_HAPPY_STATE_REQUEST_PAYMENT_SKULL_MASK,
    OSSAN_HAPPY_STATE_REQUEST_PAYMENT_BUNNY_HOOD,
    OSSAN_HAPPY_STATE_BORROWED_FIRST_MASK,
    OSSAN_HAPPY_STATE_ANGRY,          // Give me my money man!
    OSSAN_HAPPY_STATE_ALL_MASKS_SOLD, // All masks have been sold
    OSSAN_HAPPY_STATE_NONE = 8        // No Action / Payment received!
} EnOssanHappyMaskState;

#endif
