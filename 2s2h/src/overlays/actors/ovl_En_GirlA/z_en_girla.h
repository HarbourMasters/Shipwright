#ifndef Z_EN_GIRLA_H
#define Z_EN_GIRLA_H

#include "global.h"

struct EnGirlA;

typedef void (*EnGirlAActionFunc)(struct EnGirlA*, PlayState*);
typedef void (*EnGirlADrawFunc)(struct Actor*, PlayState*, s32);
typedef s32 (*EnGirlACanBuyFunc)(PlayState*, struct EnGirlA*);
typedef void (*EnGirlAShopActionFunc)(PlayState*, struct EnGirlA*); // Buying/Restocking

typedef struct ShopItemEntry {
    /* 0x00 */ s16 objectId;
    /* 0x02 */ s16 getItemDrawId;
    /* 0x04 */ EnGirlADrawFunc drawFunc;
    /* 0x08 */ s16 params;
    /* 0x0A */ u16 descriptionTextId;
    /* 0x0C */ u16 choiceTextId;
    /* 0x10 */ s32 getItemId;
    /* 0x14 */ EnGirlACanBuyFunc canBuyFunc;
    /* 0x18 */ EnGirlAShopActionFunc buyFunc;
    /* 0x1C */ EnGirlAShopActionFunc buyFanfareFunc;
} ShopItemEntry; // size = 0x20

typedef struct EnGirlA {
    /* 0x000 */ Actor actor;
    /* 0x144 */ UNK_TYPE1 unk144[0x44];
    /* 0x188 */ EnGirlAActionFunc actionFunc;
    /* 0x18C */ s8 objectSlot;
    /* 0x190 */ EnGirlAActionFunc mainActionFunc;
    /* 0x194 */ s32 isInitialized;
    /* 0x198 */ u16 choiceTextId;
    /* 0x19C */ s32 getItemId;
    /* 0x1A0 */ s16 isOutOfStock;
    /* 0x1A4 */ EnGirlAShopActionFunc boughtFunc;
    /* 0x1A8 */ EnGirlAShopActionFunc restockFunc;
    /* 0x1AC */ s16 isSelected;
    /* 0x1AE */ s16 initialRotY;
    /* 0x1B0 */ s16 rotY;
    /* 0x1B4 */ EnGirlACanBuyFunc canBuyFunc;
    /* 0x1B8 */ EnGirlAShopActionFunc buyFunc;
    /* 0x1BC */ EnGirlAShopActionFunc buyFanfareFunc;
    /* 0x1C0 */ s16 unk1C0;
    /* 0x1C2 */ s16 itemParams;
    /* 0x1C4 */ s16 getItemDrawId;
    /* 0x1C8 */ EnGirlADrawFunc drawFunc;
} EnGirlA; // size = 0x1CC

typedef enum {
    /* 0 */ CANBUY_RESULT_SUCCESS_1,
    /* 1 */ CANBUY_RESULT_SUCCESS_2,
    /* 2 */ CANBUY_RESULT_NO_ROOM,
    /* 3 */ CANBUY_RESULT_NEED_EMPTY_BOTTLE,
    /* 4 */ CANBUY_RESULT_NEED_RUPEES,
    /* 5 */ CANBUY_RESULT_CANNOT_GET_NOW,
    /* 6 */ CANBUY_RESULT_CANNOT_GET_NOW_2,
    /* 7 */ CANBUY_RESULT_NO_ROOM_2,
    /* 8 */ CANBUY_RESULT_ALREADY_HAVE,
    /* 9 */ CANBUY_RESULT_HAVE_BETTER
} EnGirlACanBuyResult;

typedef enum {
    /* 0x00 */ SI_POTION_RED_1,
    /* 0x01 */ SI_POTION_GREEN_1,
    /* 0x02 */ SI_POTION_BLUE,
    /* 0x03 */ SI_FAIRY_1,
    /* 0x04 */ SI_ARROWS_LARGE_1,
    /* 0x05 */ SI_POTION_GREEN_2,
    /* 0x06 */ SI_SHIELD_HERO_1,
    /* 0x07 */ SI_STICK_1,
    /* 0x08 */ SI_ARROWS_MEDIUM_1,
    /* 0x09 */ SI_NUTS_1,
    /* 0x0A */ SI_POTION_RED_2,
    /* 0x0B */ SI_FAIRY_2,
    /* 0x0C */ SI_ARROWS_MEDIUM_2,
    /* 0x0D */ SI_ARROWS_LARGE_2,
    /* 0x0E */ SI_POTION_GREEN_3,
    /* 0x0F */ SI_NUTS_2,
    /* 0x10 */ SI_STICK_2,
    /* 0x11 */ SI_SHIELD_HERO_2,
    /* 0x12 */ SI_POTION_RED_3,
    /* 0x13 */ SI_MASK_ALL_NIGHT,
    /* 0x14 */ SI_BOMB_BAG_20_1,
    /* 0x15 */ SI_BOMB_BAG_30_1,
    /* 0x16 */ SI_BOMB_BAG_40,
    /* 0x17 */ SI_BOMB_BAG_20_2,
    /* 0x18 */ SI_BOMB_BAG_30_2,
    /* 0x19 */ SI_BOMBCHU,
    /* 0x1A */ SI_BOMB_1,
    /* 0x1B */ SI_SHIELD_HERO_3,
    /* 0x1C */ SI_ARROWS_SMALL_1,
    /* 0x1D */ SI_POTION_RED_4,
    /* 0x1E */ SI_BOMB_2,
    /* 0x1F */ SI_ARROWS_SMALL_2,
    /* 0x20 */ SI_POTION_RED_5,
    /* 0x21 */ SI_BOMB_3,
    /* 0x22 */ SI_ARROWS_SMALL_3,
    /* 0x23 */ SI_POTION_RED_6,
    /* 0x24 */ SI_BOTTLE,
    /* 0x25 */ SI_SWORD_GREAT_FAIRY,
    /* 0x26 */ SI_SWORD_KOKIRI,
    /* 0x27 */ SI_SWORD_RAZOR,
    /* 0x28 */ SI_SWORD_GILDED,
    /* 0x29 */ SI_SHIELD_HERO_4,
    /* 0x2A */ SI_SHIELD_MIRROR,
    /* 0x2B */ SI_MAX
} EnGirlAShopItemId;

/* 
* Shopkeeper Common Defines 
*/
#define CURSOR_INVALID 0xFF
#define COL_CHAN_MIX(c1, c2, m) (c1 - (s32)(c2 * m)) & 0xFF

typedef struct {
    /* 0x00 */ Color_RGBAu32 stickColor;
    /* 0x10 */ f32 stickTexX;
    /* 0x14 */ f32 stickTexY;
    /* 0x18 */ Color_RGBAu32 arrowColor;
    /* 0x28 */ f32 arrowTexX;
    /* 0x2C */ f32 arrowTexY;
    /* 0x30 */ f32 texZ; // Used for both stickTexZ and arrowTexZ
    /* 0x34 */ u32 isEnabled;
} StickDirectionPrompt; // size = 0x38

typedef struct ShopItem {
    /* 0x0 */ s16 shopItemId;
    /* 0x2 */ Vec3s spawnPos;
} ShopItem; // size = 0x8

#endif // Z_EN_GIRLA_H
