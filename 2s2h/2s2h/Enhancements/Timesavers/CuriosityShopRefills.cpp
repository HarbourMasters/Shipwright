#include <libultraship/bridge/consolevariablebridge.h>
#include "2s2h/GameInteractor/GameInteractor.h"
#include "2s2h/ShipInit.hpp"
#include "2s2h/CustomMessage/CustomMessage.h"
#include "2s2h/Rando/Rando.h"

extern "C" {
#include "overlays/actors/ovl_En_Fsn/z_en_fsn.h"
#include "overlays/actors/ovl_En_GirlA/z_en_girla.h"

void EnGirlA_SetupAction(EnGirlA* enGirlA, EnGirlAActionFunc action);
void EnGirlA_Update2(EnGirlA* enGirlA, PlayState* play);
void EnGirlA_DoNothing(EnGirlA* enGirlA, PlayState* play);
void EnGirlA_Bought(PlayState* play, EnGirlA* enGirlA);
void EnGirlA_Restock(PlayState* play, EnGirlA* enGirlA);
void EnGirlA_Draw(Actor* actor, PlayState* play);
void EnGirlA_BuyFanfare(PlayState* play, EnGirlA* enGirlA);
}

#define CVAR_NAME "gEnhancements.Shops.CuriosityShopRefills"
#define CVAR CVarGetInteger(CVAR_NAME, 0)

#define SHOP_DESC_TEXT_ID 0x0841
#define SHOP_CHOICE_TEXT_ID 0x0842

struct RefillItem {
    ItemId itemId;
    GetItemId gi;
    GetItemDrawId drawId;
    u16 weekEventFlag;
    RandoItemId randoItem;
    const char* name;
    u16 price;
};

static constexpr RefillItem sRefillItems[] = {
    { ITEM_SEAHORSE, GI_SEAHORSE_CAUGHT, GID_SEAHORSE_CAUGHT, WEEKEVENTREG_RECEIVED_SEAHORSE_HEART_PIECE, RI_NONE,
      "Seahorse", 100 },
    { ITEM_GOLD_DUST, GI_GOLD_DUST_2, GID_GOLD_DUST, WEEKEVENTREG_RECEIVED_GORON_RACE_BOTTLE, RI_BOTTLE_GOLD_DUST,
      "Gold Dust", 200 },
    { ITEM_CHATEAU, GI_CHATEAU, GID_CHATEAU, WEEKEVENTREG_57_08, RI_BOTTLE_CHATEAU_ROMANI, "Chateau Romani", 200 },
};

// Shop item positions
static const Vec3f sShopItemPositions[] = {
    { -5.0f, 35.0f, -95.0f },
    { 13.0f, 35.0f, -95.0f },
    { 31.0f, 35.0f, -95.0f },
};

static const RefillItem* GetRefillItem(s16 shopId) {
    if (shopId < 0 || shopId >= static_cast<s16>(std::size(sRefillItems))) {
        return nullptr;
    }
    return &sRefillItems[shopId];
}

static bool HasAccessToGreatBay() {
    if (gSaveContext.save.saveInfo.inventory.items[SLOT_OCARINA] == ITEM_NONE) {
        return false;
    }

    if (CHECK_QUEST_ITEM(QUEST_SONG_EPONA)) {
        return true;
    }

    if (CHECK_QUEST_ITEM(QUEST_SONG_SOARING)) {
        if (GET_OWL_STATUE_ACTIVATED(OWL_WARP_GREAT_BAY_COAST)) {
            return true;
        }

        if (GET_OWL_STATUE_ACTIVATED(OWL_WARP_ZORA_CAPE)) {
            return true;
        }
    }
    return false;
}

// Note: IsRefillAvailable checks for bottle availibility. We don't need to do that here.
static bool HasSeahorseRequirements() {
    if (!HasAccessToGreatBay()) {
        return false;
    }

    if (gSaveContext.save.saveInfo.inventory.items[SLOT_MASK_ZORA] == ITEM_NONE) {
        return false;
    }

    if (gSaveContext.save.saveInfo.inventory.items[SLOT_PICTOGRAPH_BOX] == ITEM_NONE) {
        return false;
    }

    if (IS_RANDO && RANDO_SAVE_OPTIONS[RO_SHUFFLE_SWIM] && !Flags_GetRandoInf(RANDO_INF_OBTAINED_SWIM)) {
        return false;
    }

    return true;
}

static bool IsRefillAvailable(const RefillItem& item) {
    // If they got it in their inventory don't even offer the thing for sale
    if (Inventory_HasItemInBottle(item.itemId)) {
        return false;
    }

    if (item.itemId == ITEM_SEAHORSE) {
        return HasSeahorseRequirements();
    }

    if (IS_RANDO) {
        if (item.randoItem != RI_NONE) {
            RandoCheckId itemPlacement = Rando::FindItemPlacement(item.randoItem);
            return itemPlacement != RC_UNKNOWN && RANDO_SAVE_CHECKS[itemPlacement].obtained;
        }
    }

    // Vanilla: check the relevant quest flag
    return CHECK_WEEKEVENTREG(item.weekEventFlag);
}

static bool HasAvailableRefills() {
    for (const auto& item : sRefillItems) {
        if (IsRefillAvailable(item)) {
            return true;
        }
    }
    return false;
}

static s32 RefillItem_CanBuy(PlayState* play, EnGirlA* enGirlA) {
    const RefillItem* item = GetRefillItem(enGirlA->actor.world.rot.z);

    if (item == nullptr) {
        return CANBUY_RESULT_CANNOT_GET_NOW;
    }

    if (!Inventory_HasEmptyBottle()) {
        return CANBUY_RESULT_NEED_EMPTY_BOTTLE;
    }

    if (gSaveContext.save.saveInfo.playerData.rupees < item->price) {
        return CANBUY_RESULT_NEED_RUPEES;
    }

    return CANBUY_RESULT_SUCCESS_2;
}

static void RefillItem_Init(EnGirlA* enGirlA, PlayState* play) {
    const RefillItem* item = GetRefillItem(enGirlA->actor.world.rot.z);
    if (item == nullptr) {
        Actor_Kill(&enGirlA->actor);
        return;
    }

    // Mimic EnGirlA initialization
    enGirlA->actor.textId = SHOP_DESC_TEXT_ID;
    enGirlA->choiceTextId = SHOP_CHOICE_TEXT_ID;
    enGirlA->getItemId = item->gi;
    enGirlA->getItemDrawId = item->drawId;
    enGirlA->canBuyFunc = RefillItem_CanBuy;
    enGirlA->boughtFunc = EnGirlA_Bought;
    enGirlA->restockFunc = EnGirlA_Restock;
    enGirlA->buyFunc = (EnGirlAShopActionFunc)EnGirlA_DoNothing; // EnFsn handles the transfer
    enGirlA->buyFanfareFunc = EnGirlA_BuyFanfare;

    enGirlA->actor.flags &= ~ACTOR_FLAG_ATTENTION_ENABLED;
    Actor_SetScale(&enGirlA->actor, 0.25f);
    enGirlA->actor.shape.yOffset = 24.0f;
    enGirlA->actor.shape.shadowScale = 4.0f;
    enGirlA->actor.floorHeight = enGirlA->actor.home.pos.y;
    enGirlA->actor.gravity = 0.0f;

    EnGirlA_SetupAction(enGirlA, EnGirlA_DoNothing);
    enGirlA->isInitialized = true;
    enGirlA->mainActionFunc = EnGirlA_Update2;
    enGirlA->isSelected = false;
    enGirlA->rotY = 0;
    enGirlA->initialRotY = enGirlA->actor.shape.rot.y;
    enGirlA->isOutOfStock = false;
    enGirlA->actor.draw = EnGirlA_Draw;
    enGirlA->drawFunc = NULL;
}

static const RefillItem* GetActiveRefillItem() {
    if (gPlayState->msgCtx.talkActor == nullptr || gPlayState->msgCtx.talkActor->id != ACTOR_EN_FSN) {
        return nullptr;
    }

    auto* enFsn = (EnFsn*)gPlayState->msgCtx.talkActor;
    if (enFsn->cursorIndex < 0 || enFsn->cursorIndex >= ARRAY_COUNT(sShopItemPositions)) {
        return nullptr;
    }

    auto* itemActor = enFsn->items[enFsn->cursorIndex];
    if (itemActor == nullptr) {
        return nullptr;
    }

    return GetRefillItem(itemActor->actor.world.rot.z);
}

static void RegisterCuriosityShopRefills() {
    // Spawn refills on Shopkeeper Init
    COND_ID_HOOK(OnActorInit, ACTOR_EN_FSN, CVAR, [](Actor* actor) {
        EnFsn* enFsn = (EnFsn*)actor;

        if (!ENFSN_IS_SHOP(&enFsn->actor) || gPlayState->sceneId != SCENE_AYASHIISHOP) {
            return;
        }

        s32 slotIndex = enFsn->totalSellingItems;

        for (size_t i = 0; i < std::size(sRefillItems) && slotIndex < ARRAY_COUNT(sShopItemPositions); ++i) {
            const RefillItem& refill = sRefillItems[i];
            if (!IsRefillAvailable(refill)) {
                continue;
            }

            const Vec3f& pos = sShopItemPositions[slotIndex];
            Actor* spawnedActor =
                Actor_Spawn(&gPlayState->actorCtx, gPlayState, ACTOR_EN_GIRLA, pos.x, pos.y, pos.z, 0, 0, 0, SI_BOTTLE);
            if (spawnedActor != nullptr) {
                EnGirlA* enGirlA = (EnGirlA*)spawnedActor;
                enGirlA->actor.world.rot.z = (s16)i;
                enGirlA->mainActionFunc = (EnGirlAActionFunc)RefillItem_Init; // Hijack update to run correct init

                enFsn->items[slotIndex] = enGirlA;
                enFsn->itemIds[slotIndex] = SI_BOTTLE;
                enFsn->totalSellingItems++;
                enFsn->numSellingItems++;
                slotIndex++;
            }
        }
    });

    // Hook: Notify shopkeeper of available items
    COND_VB_SHOULD(VB_EN_FSN_HAS_ITEMS, CVAR, {
        if (gPlayState->sceneId == SCENE_AYASHIISHOP) {
            *should = HasAvailableRefills();
        }
    });

    // Custom text descriptions
    COND_ID_HOOK(OnOpenText, SHOP_DESC_TEXT_ID, CVAR, [](u16* textId, bool* loadFromMessageTable) {
        const RefillItem* item = GetActiveRefillItem();
        if (item == nullptr) {
            return;
        }

        CustomMessage::Entry entry = CustomMessage::LoadVanillaMessageTableEntry(SHOP_DESC_TEXT_ID);
        entry.msg =
            "%r" + std::string(item->name) + ": " + std::to_string(item->price) + " Rupees\n" + "%wA rare refill!\x1A";

        CustomMessage::LoadCustomMessageIntoFont(entry);
        *loadFromMessageTable = false;
    });

    COND_ID_HOOK(OnOpenText, SHOP_CHOICE_TEXT_ID, CVAR, [](u16* textId, bool* loadFromMessageTable) {
        const RefillItem* item = GetActiveRefillItem();
        if (item == nullptr) {
            return;
        }

        CustomMessage::Entry entry = CustomMessage::LoadVanillaMessageTableEntry(SHOP_CHOICE_TEXT_ID);
        entry.firstItemCost = item->price;
        entry.msg = "%r" + std::string(item->name) + ": " + std::to_string(item->price) + " Rupees\n" +
                    "%g\xC2I'll buy it\nNo thanks";

        CustomMessage::LoadCustomMessageIntoFont(entry);
        *loadFromMessageTable = false;
    });
}

static RegisterShipInitFunc initFunc(RegisterCuriosityShopRefills, { CVAR_NAME });
