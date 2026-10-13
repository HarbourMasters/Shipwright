/*
 * File: z_en_girla.c
 * Overlay: obj_En_Girla
 * Description: Shop Items
 */

#include "z_en_girla.h"

#define FLAGS (ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_FRIENDLY | ACTOR_FLAG_UPDATE_CULLING_DISABLED)

void EnGirlA_Init(Actor* thisx, PlayState* play);
void EnGirlA_Destroy(Actor* thisx, PlayState* play);
void EnGirlA_Update(Actor* thisx, PlayState* play);
void EnGirlA_Draw(Actor* thisx, PlayState* play);

void EnGirlA_InitialUpdate(EnGirlA* this, PlayState* play);
void EnGirlA_Update2(EnGirlA* this, PlayState* play);

s32 EnGirlA_CanBuyPotionRed(PlayState* play, EnGirlA* this);
s32 EnGirlA_CanBuyPotionGreen(PlayState* play, EnGirlA* this);
s32 EnGirlA_CanBuyPotionBlue(PlayState* play, EnGirlA* this);
s32 EnGirlA_CanBuyArrows(PlayState* play, EnGirlA* this);
s32 EnGirlA_CanBuyNuts(PlayState* play, EnGirlA* this);
s32 EnGirlA_CanBuyShieldHero(PlayState* play, EnGirlA* this);
s32 EnGirlA_CanBuyStick(PlayState* play, EnGirlA* this);
s32 EnGirlA_CanBuyMaskAllNight(PlayState* play, EnGirlA* this);
s32 EnGirlA_CanBuyBombBagCuriosityShop(PlayState* play, EnGirlA* this);
s32 EnGirlA_CanBuyBombBag20BombShop(PlayState* play, EnGirlA* this);
s32 EnGirlA_CanBuyBombBag30BombShop(PlayState* play, EnGirlA* this);
s32 EnGirlA_CanBuyBombchus(PlayState* play, EnGirlA* this);
s32 EnGirlA_CanBuyBombs(PlayState* play, EnGirlA* this);
s32 EnGirlA_CanBuyBottleStolen(PlayState* play, EnGirlA* this);
s32 EnGirlA_CanBuySword(PlayState* play, EnGirlA* this);
s32 EnGirlA_CanBuyShieldMirror(PlayState* play, EnGirlA* this);
s32 EnGirlA_CanBuyFairy(PlayState* play, EnGirlA* this);

void EnGirlA_BuyBottleItem(PlayState* play, EnGirlA* this);
void EnGirlA_BuyArrows(PlayState* play, EnGirlA* this);
void EnGirlA_BuyNuts(PlayState* play, EnGirlA* this);
void EnGirlA_BuyShieldHero(PlayState* play, EnGirlA* this);
void EnGirlA_BuyStick(PlayState* play, EnGirlA* this);
void EnGirlA_BuyMaskAllNight(PlayState* play, EnGirlA* this);
void EnGirlA_BuyBombBag(PlayState* play, EnGirlA* this);
void EnGirlA_BuyBombchus(PlayState* play, EnGirlA* this);
void EnGirlA_BuyBombs(PlayState* play, EnGirlA* this);
void EnGirlA_BuyBottle(PlayState* play, EnGirlA* this);
void EnGirlA_BuySword(PlayState* play, EnGirlA* this);
void EnGirlA_BuyShieldMirror(PlayState* play, EnGirlA* this);

void EnGirlA_BuyFanfare(PlayState* play, EnGirlA* this);

ActorProfile En_GirlA_Profile = {
    /**/ ACTOR_EN_GIRLA,
    /**/ ACTORCAT_PROP,
    /**/ FLAGS,
    /**/ GAMEPLAY_KEEP,
    /**/ sizeof(EnGirlA),
    /**/ EnGirlA_Init,
    /**/ EnGirlA_Destroy,
    /**/ EnGirlA_Update,
    /**/ NULL,
};

static ShopItemEntry sShopItemEntries[] = {
    { OBJECT_GI_LIQUID, GID_POTION_RED, func_800B8050, 1, 0x083F, 0x0840, GI_POTION_RED, EnGirlA_CanBuyPotionRed,
      EnGirlA_BuyBottleItem, EnGirlA_BuyFanfare },
    { OBJECT_GI_LIQUID, GID_POTION_GREEN, func_800B8050, 1, 0x0841, 0x0842, GI_POTION_GREEN, EnGirlA_CanBuyPotionGreen,
      EnGirlA_BuyBottleItem, EnGirlA_BuyFanfare },
    { OBJECT_GI_LIQUID, GID_POTION_BLUE, func_800B8050, 1, 0x0843, 0x0844, GI_POTION_BLUE, EnGirlA_CanBuyPotionBlue,
      EnGirlA_BuyBottleItem, EnGirlA_BuyFanfare },
    { OBJECT_GI_BOTTLE_04, GID_FAIRY, func_800B8050, 1, 0x06B6, 0x06B7, GI_FAIRY, EnGirlA_CanBuyFairy,
      EnGirlA_BuyBottleItem, EnGirlA_BuyFanfare },
    { OBJECT_GI_ARROW, GID_ARROWS_LARGE, func_800B8050, 50, 0x06BA, 0x06BB, GI_ARROWS_40, EnGirlA_CanBuyArrows,
      EnGirlA_BuyArrows, EnGirlA_BuyFanfare },
    { OBJECT_GI_LIQUID, GID_POTION_GREEN, func_800B8050, 1, 0x06B2, 0x06B3, GI_POTION_GREEN, EnGirlA_CanBuyPotionGreen,
      EnGirlA_BuyBottleItem, EnGirlA_BuyFanfare },
    { OBJECT_GI_SHIELD_2, GID_SHIELD_HERO, func_800B8050, 1, 0x06AE, 0x06AF, GI_SHIELD_HERO, EnGirlA_CanBuyShieldHero,
      EnGirlA_BuyShieldHero, EnGirlA_BuyFanfare },
    { OBJECT_GI_STICK, GID_DEKU_STICK, NULL, 1, 0x06B4, 0x06B5, GI_DEKU_STICKS_1, EnGirlA_CanBuyStick, EnGirlA_BuyStick,
      EnGirlA_BuyFanfare },
    { OBJECT_GI_ARROW, GID_ARROWS_MEDIUM, func_800B8050, 30, 0x06B8, 0x06B9, GI_ARROWS_30, EnGirlA_CanBuyArrows,
      EnGirlA_BuyArrows, EnGirlA_BuyFanfare },
    { OBJECT_GI_NUTS, GID_DEKU_NUTS, func_800B8118, 10, 0x06B0, 0x06B1, GI_DEKU_NUTS_10, EnGirlA_CanBuyNuts,
      EnGirlA_BuyNuts, EnGirlA_BuyFanfare },
    { OBJECT_GI_LIQUID, GID_POTION_RED, func_800B8050, 1, 0x06AC, 0x06AD, GI_POTION_RED, EnGirlA_CanBuyPotionRed,
      EnGirlA_BuyBottleItem, EnGirlA_BuyFanfare },
    { OBJECT_GI_BOTTLE_04, GID_FAIRY, func_800B8050, 1, 0x06D3, 0x06D4, GI_FAIRY, EnGirlA_CanBuyFairy,
      EnGirlA_BuyBottleItem, EnGirlA_BuyFanfare },
    { OBJECT_GI_ARROW, GID_ARROWS_MEDIUM, func_800B8050, 30, 0x06D5, 0x06D6, GI_ARROWS_30, EnGirlA_CanBuyArrows,
      EnGirlA_BuyArrows, EnGirlA_BuyFanfare },
    { OBJECT_GI_ARROW, GID_ARROWS_LARGE, func_800B8050, 50, 0x06D7, 0x06D8, GI_ARROWS_40, EnGirlA_CanBuyArrows,
      EnGirlA_BuyArrows, EnGirlA_BuyFanfare },
    { OBJECT_GI_LIQUID, GID_POTION_GREEN, func_800B8050, 1, 0x06CF, 0x06D0, GI_POTION_GREEN, EnGirlA_CanBuyPotionGreen,
      EnGirlA_BuyBottleItem, EnGirlA_BuyFanfare },
    { OBJECT_GI_NUTS, GID_DEKU_NUTS, func_800B8118, 10, 0x06CD, 0x06CE, GI_DEKU_NUTS_10, EnGirlA_CanBuyNuts,
      EnGirlA_BuyNuts, EnGirlA_BuyFanfare },
    { OBJECT_GI_STICK, GID_DEKU_STICK, NULL, 1, 0x06D1, 0x06D2, GI_DEKU_STICKS_1, EnGirlA_CanBuyStick, EnGirlA_BuyStick,
      EnGirlA_BuyFanfare },
    { OBJECT_GI_SHIELD_2, GID_SHIELD_HERO, func_800B8050, 1, 0x06CB, 0x06CC, GI_SHIELD_HERO, EnGirlA_CanBuyShieldHero,
      EnGirlA_BuyShieldHero, EnGirlA_BuyFanfare },
    { OBJECT_GI_LIQUID, GID_POTION_RED, func_800B8050, 1, 0x06C9, 0x06CA, GI_POTION_RED, EnGirlA_CanBuyPotionRed,
      EnGirlA_BuyBottleItem, EnGirlA_BuyFanfare },
    { OBJECT_GI_MASK06, GID_MASK_ALL_NIGHT, func_800B8050, 1, 0x29D9, 0x29DA, GI_MASK_ALL_NIGHT,
      EnGirlA_CanBuyMaskAllNight, EnGirlA_BuyMaskAllNight, EnGirlA_BuyFanfare },
    { OBJECT_GI_BOMBPOUCH, GID_BOMB_BAG_20, func_800B8050, 1, 0x29DB, 0x29DC, GI_BOMB_BAG_20,
      EnGirlA_CanBuyBombBagCuriosityShop, EnGirlA_BuyBombBag, EnGirlA_BuyFanfare },
    { OBJECT_GI_BOMBPOUCH, GID_BOMB_BAG_30, func_800B8050, 2, 0x29DB, 0x29DC, GI_BOMB_BAG_30,
      EnGirlA_CanBuyBombBagCuriosityShop, EnGirlA_BuyBombBag, EnGirlA_BuyFanfare },
    { OBJECT_GI_BOMBPOUCH, GID_BOMB_BAG_40, func_800B8050, 3, 0x29DB, 0x29DC, GI_BOMB_BAG_40,
      EnGirlA_CanBuyBombBagCuriosityShop, EnGirlA_BuyBombBag, EnGirlA_BuyFanfare },
    { OBJECT_GI_BOMBPOUCH, GID_BOMB_BAG_20, func_800B8050, 1, 0x0654, 0x0655, GI_BOMB_BAG_20,
      EnGirlA_CanBuyBombBag20BombShop, EnGirlA_BuyBombBag, EnGirlA_BuyFanfare },
    { OBJECT_GI_BOMBPOUCH, GID_BOMB_BAG_30, func_800B8050, 2, 0x0656, 0x0657, GI_BOMB_BAG_30,
      EnGirlA_CanBuyBombBag30BombShop, EnGirlA_BuyBombBag, EnGirlA_BuyFanfare },
    { OBJECT_GI_BOMB_2, GID_BOMBCHU, func_800B8050, 10, 0x0652, 0x0653, GI_BOMBCHUS_10, EnGirlA_CanBuyBombchus,
      EnGirlA_BuyBombchus, EnGirlA_BuyFanfare },
    { OBJECT_GI_BOMB_1, GID_BOMB, func_800B8050, 10, 0x0650, 0x0651, GI_BOMBS_10, EnGirlA_CanBuyBombs, EnGirlA_BuyBombs,
      EnGirlA_BuyFanfare },
    { OBJECT_GI_SHIELD_2, GID_SHIELD_HERO, func_800B8050, 1, 0x12DB, 0x12DC, GI_SHIELD_HERO, EnGirlA_CanBuyShieldHero,
      EnGirlA_BuyShieldHero, EnGirlA_BuyFanfare },
    { OBJECT_GI_ARROW, GID_ARROWS_SMALL, func_800B8050, 10, 0x12DD, 0x12DE, GI_ARROWS_10, EnGirlA_CanBuyArrows,
      EnGirlA_BuyArrows, EnGirlA_BuyFanfare },
    { OBJECT_GI_LIQUID, GID_POTION_RED, func_800B8050, 1, 0x12DF, 0x12E0, GI_POTION_RED, EnGirlA_CanBuyPotionRed,
      EnGirlA_BuyBottleItem, EnGirlA_BuyFanfare },
    { OBJECT_GI_BOMB_1, GID_BOMB, func_800B8050, 10, 0x0BC5, 0x0BC6, GI_BOMBS_10, EnGirlA_CanBuyBombs, EnGirlA_BuyBombs,
      EnGirlA_BuyFanfare },
    { OBJECT_GI_ARROW, GID_ARROWS_SMALL, func_800B8050, 10, 0x0BC7, 0x0BC8, GI_ARROWS_10, EnGirlA_CanBuyArrows,
      EnGirlA_BuyArrows, EnGirlA_BuyFanfare },
    { OBJECT_GI_LIQUID, GID_POTION_RED, func_800B8050, 1, 0x0BC9, 0x0BCA, GI_POTION_RED, EnGirlA_CanBuyPotionRed,
      EnGirlA_BuyBottleItem, EnGirlA_BuyFanfare },
    { OBJECT_GI_BOMB_1, GID_BOMB, func_800B8050, 10, 0x0BCB, 0x0BCC, GI_BOMBS_10, EnGirlA_CanBuyBombs, EnGirlA_BuyBombs,
      EnGirlA_BuyFanfare },
    { OBJECT_GI_ARROW, GID_ARROWS_SMALL, func_800B8050, 10, 0x0BCD, 0x0BCE, GI_ARROWS_10, EnGirlA_CanBuyArrows,
      EnGirlA_BuyArrows, EnGirlA_BuyFanfare },
    { OBJECT_GI_LIQUID, GID_POTION_RED, func_800B8050, 1, 0x0BCF, 0x0BD0, GI_POTION_RED, EnGirlA_CanBuyPotionRed,
      EnGirlA_BuyBottleItem, EnGirlA_BuyFanfare },
    { OBJECT_GI_BOTTLE, GID_BOTTLE, func_800B8050, 1, 0x29F8, 0x29F9, GI_BOTTLE_STOLEN, EnGirlA_CanBuyBottleStolen,
      EnGirlA_BuyBottle, EnGirlA_BuyFanfare },
    { OBJECT_GI_SWORD_4, GID_SWORD_GREAT_FAIRY, func_800B8050, 4, 0x29F2, 0x29F3, GI_SWORD_GREAT_FAIRY_STOLEN,
      EnGirlA_CanBuySword, EnGirlA_BuySword, EnGirlA_BuyFanfare },
    { OBJECT_GI_SWORD_1, GID_SWORD_KOKIRI, func_800B8050, 1, 0x29F4, 0x29F5, GI_SWORD_KOKIRI_STOLEN,
      EnGirlA_CanBuySword, EnGirlA_BuySword, EnGirlA_BuyFanfare },
    { OBJECT_GI_SWORD_2, GID_SWORD_RAZOR, func_800B8050, 2, 0x29F4, 0x29F5, GI_SWORD_RAZOR_STOLEN, EnGirlA_CanBuySword,
      EnGirlA_BuySword, EnGirlA_BuyFanfare },
    { OBJECT_GI_SWORD_3, GID_SWORD_GILDED, func_800B8050, 3, 0x29F4, 0x29F5, GI_SWORD_GILDED_STOLEN,
      EnGirlA_CanBuySword, EnGirlA_BuySword, EnGirlA_BuyFanfare },
    { OBJECT_GI_SHIELD_2, GID_SHIELD_HERO, func_800B8050, 1, 0x29F6, 0x29F7, GI_SHIELD_HERO_STOLEN,
      EnGirlA_CanBuyShieldHero, EnGirlA_BuyShieldHero, EnGirlA_BuyFanfare },
    { OBJECT_GI_SHIELD_3, GID_SHIELD_MIRROR, func_800B8050, 1, 0x29F6, 0x29F7, GI_SHIELD_MIRROR,
      EnGirlA_CanBuyShieldMirror, EnGirlA_BuyShieldMirror, EnGirlA_BuyFanfare },
};

void EnGirlA_SetupAction(EnGirlA* this, EnGirlAActionFunc action) {
    this->actionFunc = action;
}

void EnGirlA_InitObjIndex(EnGirlA* this, PlayState* play) {
    s16 params = this->actor.params;

    if ((params >= SI_MAX) && (params < SI_POTION_RED_1)) {
        //! @bug: Impossible to reach, && should be an ||
        Actor_Kill(&this->actor);
        return;
    }

    this->objectSlot = Object_GetSlot(&play->objectCtx, sShopItemEntries[params].objectId);
    if (this->objectSlot <= OBJECT_SLOT_NONE) {
        Actor_Kill(&this->actor);
        return;
    }

    this->actor.params = params;
    this->mainActionFunc = EnGirlA_InitialUpdate;
}

void EnGirlA_Init(Actor* thisx, PlayState* play) {
    EnGirlA* this = (EnGirlA*)thisx;

    EnGirlA_InitObjIndex(this, play);
}

void EnGirlA_Destroy(Actor* thisx, PlayState* play) {
}

s32 EnGirlA_CanBuyPotionRed(PlayState* play, EnGirlA* this) {
    if (!Inventory_HasEmptyBottle()) {
        return CANBUY_RESULT_NEED_EMPTY_BOTTLE;
    }
    if (gSaveContext.save.saveInfo.playerData.rupees < play->msgCtx.unk1206C) {
        return CANBUY_RESULT_NEED_RUPEES;
    }
    return CANBUY_RESULT_SUCCESS_2;
}

s32 EnGirlA_CanBuyPotionGreen(PlayState* play, EnGirlA* this) {
    if (!Inventory_HasEmptyBottle()) {
        return CANBUY_RESULT_NEED_EMPTY_BOTTLE;
    }
    if (gSaveContext.save.saveInfo.playerData.rupees < play->msgCtx.unk1206C) {
        return CANBUY_RESULT_NEED_RUPEES;
    }
    return CANBUY_RESULT_SUCCESS_2;
}

s32 EnGirlA_CanBuyPotionBlue(PlayState* play, EnGirlA* this) {
    if (!CHECK_WEEKEVENTREG(WEEKEVENTREG_GAVE_KOTAKE_MUSHROOM)) {
        return CANBUY_RESULT_CANNOT_GET_NOW;
    }
    if (!Inventory_HasEmptyBottle()) {
        return CANBUY_RESULT_NEED_EMPTY_BOTTLE;
    }
    if (!CHECK_WEEKEVENTREG(WEEKEVENTREG_RECEIVED_FREE_BLUE_POTION)) {
        return CANBUY_RESULT_SUCCESS_2;
    }
    if (gSaveContext.save.saveInfo.playerData.rupees < play->msgCtx.unk1206C) {
        return CANBUY_RESULT_NEED_RUPEES;
    }
    return CANBUY_RESULT_SUCCESS_2;
}

s32 EnGirlA_CanBuyArrows(PlayState* play, EnGirlA* this) {
    if (GET_CUR_UPG_VALUE(UPG_QUIVER) == 0) {
        return CANBUY_RESULT_CANNOT_GET_NOW_2;
    }
    if (AMMO(ITEM_BOW) >= CUR_CAPACITY(UPG_QUIVER)) {
        return CANBUY_RESULT_NO_ROOM_2;
    }
    if (gSaveContext.save.saveInfo.playerData.rupees < play->msgCtx.unk1206C) {
        return CANBUY_RESULT_NEED_RUPEES;
    }
    return CANBUY_RESULT_SUCCESS_2;
}

s32 EnGirlA_CanBuyNuts(PlayState* play, EnGirlA* this) {
    if ((CUR_CAPACITY(UPG_DEKU_NUTS) != 0) && (CUR_CAPACITY(UPG_DEKU_NUTS) <= AMMO(ITEM_DEKU_NUT))) {
        return CANBUY_RESULT_NO_ROOM;
    }
    if (gSaveContext.save.saveInfo.playerData.rupees < play->msgCtx.unk1206C) {
        return CANBUY_RESULT_NEED_RUPEES;
    }
    if (Item_CheckObtainability(ITEM_DEKU_NUT) == ITEM_NONE) {
        return CANBUY_RESULT_SUCCESS_1;
    }
    return CANBUY_RESULT_SUCCESS_2;
}

s32 EnGirlA_CanBuyShieldHero(PlayState* play, EnGirlA* this) {
    if (GET_CUR_EQUIP_VALUE(EQUIP_TYPE_SHIELD) != EQUIP_VALUE_SHIELD_NONE) {
        return CANBUY_RESULT_NO_ROOM;
    }
    if (gSaveContext.save.saveInfo.playerData.rupees < play->msgCtx.unk1206C) {
        return CANBUY_RESULT_NEED_RUPEES;
    }
    return CANBUY_RESULT_SUCCESS_1;
}

s32 EnGirlA_CanBuyStick(PlayState* play, EnGirlA* this) {
    if ((CUR_CAPACITY(UPG_DEKU_STICKS) != 0) && (AMMO(ITEM_DEKU_STICK) >= CUR_CAPACITY(UPG_DEKU_STICKS))) {
        return CANBUY_RESULT_NO_ROOM;
    }
    if (gSaveContext.save.saveInfo.playerData.rupees < play->msgCtx.unk1206C) {
        return CANBUY_RESULT_NEED_RUPEES;
    }
    if (Item_CheckObtainability(ITEM_DEKU_STICK) == ITEM_NONE) {
        return CANBUY_RESULT_SUCCESS_1;
    }
    return CANBUY_RESULT_SUCCESS_2;
}

s32 EnGirlA_CanBuyMaskAllNight(PlayState* play, EnGirlA* this) {
    if (gSaveContext.save.saveInfo.playerData.rupees < play->msgCtx.unk1206C) {
        return CANBUY_RESULT_NEED_RUPEES;
    }
    return CANBUY_RESULT_SUCCESS_2;
}

s32 EnGirlA_CanBuyBombBagCuriosityShop(PlayState* play, EnGirlA* this) {
    if (GET_CUR_UPG_VALUE(UPG_BOMB_BAG) >= 2) {
        return CANBUY_RESULT_CANNOT_GET_NOW;
    }
    if (gSaveContext.save.saveInfo.playerData.rupees < play->msgCtx.unk1206C) {
        return CANBUY_RESULT_NEED_RUPEES;
    }
    return CANBUY_RESULT_SUCCESS_2;
}

s32 EnGirlA_CanBuyBombBag20BombShop(PlayState* play, EnGirlA* this) {
    if (GET_CUR_UPG_VALUE(UPG_BOMB_BAG) == 1) {
        return CANBUY_RESULT_ALREADY_HAVE;
    }
    if (GET_CUR_UPG_VALUE(UPG_BOMB_BAG) >= 2) {
        return CANBUY_RESULT_HAVE_BETTER;
    }
    if (gSaveContext.save.saveInfo.playerData.rupees < play->msgCtx.unk1206C) {
        return CANBUY_RESULT_NEED_RUPEES;
    }
    return CANBUY_RESULT_SUCCESS_1;
}

s32 EnGirlA_CanBuyBombBag30BombShop(PlayState* play, EnGirlA* this) {
    if (GET_CUR_UPG_VALUE(UPG_BOMB_BAG) == 2) {
        return CANBUY_RESULT_ALREADY_HAVE;
    }
    if (GET_CUR_UPG_VALUE(UPG_BOMB_BAG) == 3) {
        return CANBUY_RESULT_HAVE_BETTER;
    }
    if (gSaveContext.save.saveInfo.playerData.rupees < play->msgCtx.unk1206C) {
        return CANBUY_RESULT_NEED_RUPEES;
    }
    return CANBUY_RESULT_SUCCESS_1;
}

s32 EnGirlA_CanBuyBombchus(PlayState* play, EnGirlA* this) {
    if (GET_CUR_UPG_VALUE(UPG_BOMB_BAG) == 0) {
        return CANBUY_RESULT_CANNOT_GET_NOW;
    }
    if (AMMO(ITEM_BOMBCHU) >= CUR_CAPACITY(UPG_BOMB_BAG)) {
        return CANBUY_RESULT_NO_ROOM;
    }
    if (gSaveContext.save.saveInfo.playerData.rupees < play->msgCtx.unk1206C) {
        return CANBUY_RESULT_NEED_RUPEES;
    }
    if (Item_CheckObtainability(ITEM_BOMBCHU) == ITEM_NONE) {
        return CANBUY_RESULT_SUCCESS_1;
    }
    return CANBUY_RESULT_SUCCESS_2;
}

s32 EnGirlA_CanBuyBombs(PlayState* play, EnGirlA* this) {
    if (GET_CUR_UPG_VALUE(UPG_BOMB_BAG) == 0) {
        return CANBUY_RESULT_CANNOT_GET_NOW;
    }
    if (AMMO(ITEM_BOMB) >= CUR_CAPACITY(UPG_BOMB_BAG)) {
        return CANBUY_RESULT_NO_ROOM;
    }
    if (gSaveContext.save.saveInfo.playerData.rupees < play->msgCtx.unk1206C) {
        return CANBUY_RESULT_NEED_RUPEES;
    }
    return CANBUY_RESULT_SUCCESS_2;
}

s32 EnGirlA_CanBuyBottleStolen(PlayState* play, EnGirlA* this) {
    if (gSaveContext.save.saveInfo.playerData.rupees < play->msgCtx.unk1206C) {
        return CANBUY_RESULT_NEED_RUPEES;
    }
    return CANBUY_RESULT_SUCCESS_1;
}

s32 EnGirlA_CanBuySword(PlayState* play, EnGirlA* this) {
    if (gSaveContext.save.saveInfo.playerData.rupees < play->msgCtx.unk1206C) {
        return CANBUY_RESULT_NEED_RUPEES;
    }
    return CANBUY_RESULT_SUCCESS_1;
}

s32 EnGirlA_CanBuyShieldMirror(PlayState* play, EnGirlA* this) {
    if (GET_CUR_EQUIP_VALUE(EQUIP_TYPE_SHIELD) != EQUIP_VALUE_SHIELD_NONE) {
        return CANBUY_RESULT_NO_ROOM;
    }
    if (gSaveContext.save.saveInfo.playerData.rupees < play->msgCtx.unk1206C) {
        return CANBUY_RESULT_NEED_RUPEES;
    }
    return CANBUY_RESULT_SUCCESS_1;
}

s32 EnGirlA_CanBuyFairy(PlayState* play, EnGirlA* this) {
    if (!Inventory_HasEmptyBottle()) {
        return CANBUY_RESULT_NEED_EMPTY_BOTTLE;
    }
    if (gSaveContext.save.saveInfo.playerData.rupees < play->msgCtx.unk1206C) {
        return CANBUY_RESULT_NEED_RUPEES;
    }
    return CANBUY_RESULT_SUCCESS_2;
}

void EnGirlA_BuyBottleItem(PlayState* play, EnGirlA* this) {
    switch (this->actor.params) {
        case SI_POTION_RED_1:
        case SI_POTION_RED_2:
        case SI_POTION_RED_3:
        case SI_POTION_RED_4:
        case SI_POTION_RED_5:
        case SI_POTION_RED_6:
            Item_Give(play, ITEM_POTION_RED);
            break;

        case SI_POTION_GREEN_1:
        case SI_POTION_GREEN_2:
        case SI_POTION_GREEN_3:
            Item_Give(play, ITEM_POTION_GREEN);
            break;

        case SI_POTION_BLUE:
            Item_Give(play, ITEM_POTION_BLUE);
            break;

        case SI_FAIRY_1:
        case SI_FAIRY_2:
            Item_Give(play, ITEM_FAIRY);
            break;

        default:
            break;
    }
    Rupees_ChangeBy(-play->msgCtx.unk1206C);
}

void EnGirlA_BuyArrows(PlayState* play, EnGirlA* this) {
    Inventory_ChangeAmmo(ITEM_BOW, this->itemParams);
    Rupees_ChangeBy(-play->msgCtx.unk1206C);
}

void EnGirlA_BuyNuts(PlayState* play, EnGirlA* this) {
    switch (this->itemParams) {
        case 5:
            Item_Give(play, ITEM_DEKU_NUTS_5);
            break;

        case 10:
            Item_Give(play, ITEM_DEKU_NUTS_10);
            break;

        default:
            break;
    }
    Rupees_ChangeBy(-play->msgCtx.unk1206C);
}

void EnGirlA_BuyShieldHero(PlayState* play, EnGirlA* this) {
    Item_Give(play, ITEM_SHIELD_HERO);
    Rupees_ChangeBy(-play->msgCtx.unk1206C);
}

void EnGirlA_BuyStick(PlayState* play, EnGirlA* this) {
    Item_Give(play, ITEM_DEKU_STICK);
    Rupees_ChangeBy(-play->msgCtx.unk1206C);
}

void EnGirlA_BuyMaskAllNight(PlayState* play, EnGirlA* this) {
    Item_Give(play, ITEM_MASK_ALL_NIGHT);
    Rupees_ChangeBy(-play->msgCtx.unk1206C);
}

void EnGirlA_BuyBombBag(PlayState* play, EnGirlA* this) {
    //! @bug: Bomb Bag parameters in sShopItemEntries are 1 2 3, not 20 21 22
    switch (this->itemParams) {
        case 20:
            Item_Give(play, ITEM_BOMB_BAG_20);
            break;

        case 21:
            Item_Give(play, ITEM_BOMB_BAG_30);
            break;

        case 22:
            Item_Give(play, ITEM_BOMB_BAG_40);
            break;

        default:
            break;
    }
    Rupees_ChangeBy(-play->msgCtx.unk1206C);
}

void EnGirlA_BuyBombchus(PlayState* play, EnGirlA* this) {
    if (this->itemParams == 10) {
        Item_Give(play, ITEM_BOMBCHUS_10);
    }
    Rupees_ChangeBy(-play->msgCtx.unk1206C);
}

void EnGirlA_BuyBombs(PlayState* play, EnGirlA* this) {
    switch (this->itemParams) {
        case 5:
            Item_Give(play, ITEM_BOMBS_5);
            break;

        case 10:
            Item_Give(play, ITEM_BOMBS_10);
            break;

        case 20:
            Item_Give(play, ITEM_BOMBS_20);
            break;

        case 30:
            Item_Give(play, ITEM_BOMBS_30);
            break;

        default:
            break;
    }
    Rupees_ChangeBy(-play->msgCtx.unk1206C);
}

void EnGirlA_BuyBottle(PlayState* play, EnGirlA* this) {
    Item_Give(play, ITEM_BOTTLE);
    Rupees_ChangeBy(-play->msgCtx.unk1206C);
}

void EnGirlA_BuySword(PlayState* play, EnGirlA* this) {
    switch (this->itemParams) {
        case 1:
            Item_Give(play, ITEM_SWORD_KOKIRI);
            break;

        case 2:
            Item_Give(play, ITEM_SWORD_RAZOR);
            break;

        case 3:
            Item_Give(play, ITEM_SWORD_GILDED);
            break;

        case 4:
            Item_Give(play, ITEM_SWORD_GREAT_FAIRY);
            break;

        default:
            break;
    }
    Rupees_ChangeBy(-play->msgCtx.unk1206C);
}

void EnGirlA_BuyShieldMirror(PlayState* play, EnGirlA* this) {
    Item_Give(play, ITEM_SHIELD_MIRROR);
    Rupees_ChangeBy(-play->msgCtx.unk1206C);
}

// Fanfare is handled by the shopkeeper
void EnGirlA_BuyFanfare(PlayState* play, EnGirlA* this) {
    Rupees_ChangeBy(-play->msgCtx.unk1206C);
}

void EnGirlA_DoNothing(EnGirlA* this, PlayState* play) {
}

void EnGirlA_InitItem(PlayState* play, EnGirlA* this) {
    ShopItemEntry* shopItem = &sShopItemEntries[this->actor.params];

    this->actor.textId = shopItem->descriptionTextId;
    this->isOutOfStock = false;
    this->actor.draw = EnGirlA_Draw;
}

void EnGirlA_Bought(PlayState* play, EnGirlA* this) {
    this->isOutOfStock = true;
    this->actor.draw = NULL;
}

void EnGirlA_Restock(PlayState* play, EnGirlA* this) {
    this->isOutOfStock = false;
    this->actor.draw = EnGirlA_Draw;
}

// Left over from OOT
s32 EnGirlA_TrySetMaskItemDescription(EnGirlA* this, PlayState* play) {
    return false;
}

void EnGirlA_InitialUpdate(EnGirlA* this, PlayState* play) {
    s16 params = this->actor.params;
    ShopItemEntry* shopItem = &sShopItemEntries[params];

    if (Object_IsLoaded(&play->objectCtx, this->objectSlot)) {
        this->actor.flags &= ~ACTOR_FLAG_UPDATE_CULLING_DISABLED;
        this->actor.objectSlot = this->objectSlot;
        this->actor.textId = shopItem->descriptionTextId;
        this->choiceTextId = shopItem->choiceTextId;

        // EnGirlA_TrySetMaskItemDescription always returns false
        if (!EnGirlA_TrySetMaskItemDescription(this, play)) {
            EnGirlA_InitItem(play, this);
        }

        this->boughtFunc = EnGirlA_Bought;
        this->restockFunc = EnGirlA_Restock;
        this->getItemId = shopItem->getItemId;
        this->canBuyFunc = shopItem->canBuyFunc;
        this->buyFunc = shopItem->buyFunc;
        this->buyFanfareFunc = shopItem->buyFanfareFunc;
        this->unk1C0 = 0;
        this->itemParams = shopItem->params;
        this->drawFunc = shopItem->drawFunc;
        this->getItemDrawId = shopItem->getItemDrawId;
        this->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
        Actor_SetScale(&this->actor, 0.25f);
        this->actor.shape.yOffset = 24.0f;
        this->actor.shape.shadowScale = 4.0f;
        this->actor.floorHeight = this->actor.home.pos.y;
        this->actor.gravity = 0.0f;
        EnGirlA_SetupAction(this, EnGirlA_DoNothing);
        this->isInitialized = true;
        this->mainActionFunc = EnGirlA_Update2;
        this->isSelected = false;
        this->rotY = 0;
        this->initialRotY = this->actor.shape.rot.y;
    }
}

void EnGirlA_Update2(EnGirlA* this, PlayState* play) {
    Actor_SetScale(&this->actor, 0.25f);
    this->actor.shape.yOffset = 24.0f;
    this->actor.shape.shadowScale = 4.0f;
    EnGirlA_TrySetMaskItemDescription(this, play);
    this->actionFunc(this, play);
    Actor_SetFocus(&this->actor, 5.0f);
    this->actor.shape.rot.x = 0;
    if (this->isSelected) {
        this->rotY += 0x1F4;
    } else {
        Math_SmoothStepToS(&this->rotY, 0, 0xA, 0x7D0, 0);
    }
}

void EnGirlA_Update(Actor* thisx, PlayState* play) {
    EnGirlA* this = (EnGirlA*)thisx;

    this->mainActionFunc(this, play);
}

void EnGirlA_Draw(Actor* thisx, PlayState* play) {
    EnGirlA* this = (EnGirlA*)thisx;

    Matrix_RotateYS(this->rotY, MTXMODE_APPLY);
    if (this->drawFunc != NULL) {
        this->drawFunc(&this->actor, play, 0);
    }
    GetItem_Draw(play, this->getItemDrawId);
}
