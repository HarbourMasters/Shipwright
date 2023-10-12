#include "item.h"
#include "item_location.h"

#include "3drando/logic.hpp"
#include "3drando/random.hpp"
#include "3drando/item_pool.hpp"
#include "z64item.h"

namespace Rando {
Item::Item(RandomizerGet randomizerGet_, Text name_, ItemType type_, int16_t getItemId_, bool advancement_,
                     bool* logicVar_, RandomizerHintTextKey hintKey_, uint16_t itemId_, uint16_t objectId_, uint16_t gid_,
                     uint16_t textId_, uint16_t field_, int16_t chestAnimation_, GetItemCategory category_,
                     uint16_t modIndex_, bool progressive_, uint16_t price_)
    : randomizerGet(randomizerGet_), name(std::move(name_)), type(type_), getItemId(getItemId_),
      advancement(advancement_), logicVar(logicVar_), hintKey(hintKey_), progressive(progressive_), price(price_) {
    if (modIndex_ == MOD_RANDOMIZER) {
        giEntry = std::shared_ptr<GetItemEntry>(
            new GetItemEntry(GET_ITEM(itemId_, objectId_, gid_, textId_, field_, chestAnimation_, category_, modIndex_,
                                      (int16_t)randomizerGet_)));
    } else {
        giEntry = std::shared_ptr<GetItemEntry>(new GetItemEntry(
            GET_ITEM(itemId_, objectId_, gid_, textId_, field_, chestAnimation_, category_, modIndex_, getItemId_)));
    }
}

Item::Item(RandomizerGet randomizerGet_, Text name_, ItemType type_, int16_t getItemId_, bool advancement_,
                     uint8_t* logicVar_, RandomizerHintTextKey hintKey_, uint16_t itemId_, uint16_t objectId_, uint16_t gid_,
                     uint16_t textId_, uint16_t field_, int16_t chestAnimation_, GetItemCategory category_,
                     uint16_t modIndex_, bool progressive_, uint16_t price_)
    : randomizerGet(randomizerGet_), name(std::move(name_)), type(type_), getItemId(getItemId_),
      advancement(advancement_), logicVar(logicVar_), hintKey(hintKey_), progressive(progressive_), price(price_) {
    if (modIndex_ == MOD_RANDOMIZER) {
        giEntry = std::shared_ptr<GetItemEntry>(
            new GetItemEntry(GET_ITEM(itemId_, objectId_, gid_, textId_, field_, chestAnimation_, category_, modIndex_,
                                      (int16_t)randomizerGet_)));
    } else {
        giEntry = std::shared_ptr<GetItemEntry>(new GetItemEntry(
            GET_ITEM(itemId_, objectId_, gid_, textId_, field_, chestAnimation_, category_, modIndex_, getItemId_)));
    }
}

Item::Item(RandomizerGet randomizerGet_, Text name_, ItemType type_, int getItemId_, bool advancement_,
                     bool* logicVar_, RandomizerHintTextKey hintKey_, bool progressive_, uint16_t price_)
    : randomizerGet(randomizerGet_), name(std::move(name_)), type(type_), getItemId(getItemId_),
      advancement(advancement_), logicVar(logicVar_), hintKey(hintKey_), progressive(progressive_), price(price_) {
}

Item::Item(RandomizerGet randomizerGet_, Text name_, ItemType type_, int getItemId_, bool advancement_,
                     uint8_t* logicVar_, RandomizerHintTextKey hintKey_, bool progressive_, uint16_t price_)
    : randomizerGet(randomizerGet_), name(std::move(name_)), type(type_), getItemId(getItemId_),
      advancement(advancement_), logicVar(logicVar_), hintKey(hintKey_), progressive(progressive_), price(price_) {
}

    Item::~Item() = default;

void Item::ApplyEffect() {
    // If this is a key ring, logically add as many keys as we could need
    if (RHT_FOREST_TEMPLE_KEY_RING <= hintKey && hintKey <= RHT_GANONS_CASTLE_KEY_RING) {
        *std::get<uint8_t*>(logicVar) += 10;
    } else {
        if (std::holds_alternative<bool*>(logicVar)) {
            *std::get<bool*>(logicVar) = true;
        } else {
            *std::get<uint8_t*>(logicVar) += 1;
        }
    }
    Logic::UpdateHelpers();
}

void Item::UndoEffect() {
    if (RHT_FOREST_TEMPLE_KEY_RING <= hintKey && hintKey <= RHT_GANONS_CASTLE_KEY_RING) {
        *std::get<uint8_t*>(logicVar) -= 10;
    } else {
        if (std::holds_alternative<bool*>(logicVar)) {
            *std::get<bool*>(logicVar) = false;
        } else {
            *std::get<uint8_t*>(logicVar) -= 1;
        }
    }
    Logic::UpdateHelpers();
}

Rando::ItemOverride_Value Item::Value() const {
    Rando::ItemOverride_Value val;

    val.all = 0;
    val.itemId = getItemId;
    if (getItemId == RG_ICE_TRAP) {
        val.looksLikeItemId = RandomElement(IceTrapModels);
    }
    if ((getItemId >= 0x95 && getItemId <= 0x9A)) { // Boss keys
        val.looksLikeItemId = GI_KEY_BOSS;
    }
    if ((getItemId >= 0xAF && getItemId <= 0xB7)) { // Small keys
        val.looksLikeItemId = GI_KEY_SMALL;
    }
    if (type == ITEMTYPE_SHOP) {
        // With the current shopsanity implementation, we need a way to detect
        // regular shop items. This method should have no unintended side effects
        // unless there was a multiworld with 256 players... so, it should be fine.
        val.player = 0xFF;
    }
    return val;
}

const Text& Item::GetName() const {
    return name;
}

bool Item::IsAdvancement() const {
    return advancement;
}

int Item::GetItemID() const {
    return getItemId;
}

ItemType Item::GetItemType() const {
    return type;
}

RandomizerGet Item::GetRandomizerGet() {
    return randomizerGet;
}

uint16_t Item::GetPrice() const {
    return price;
}

std::shared_ptr<GetItemEntry> Item::GetGIEntry() const {
    return giEntry;
}

GetItemEntry Item::GetGIEntry_Copy() {
    return *giEntry;
}

void Item::SetPrice(uint16_t price_) {
    price = price_;
}

void Item::SetAsPlaythrough() {
    playthrough = true;
}

void Item::SetCustomDrawFunc(CustomDrawFunc drawFunc) {
    giEntry->drawFunc = drawFunc;
}

bool Item::IsPlaythrough() const {
    return playthrough;
}

bool Item::IsBottleItem() const {
    return getItemId == 0x0F ||                      // Empty Bottle
           getItemId == 0X14 ||                      // Bottle with Milk
           (getItemId >= 0x8C && getItemId <= 0x94); // Rest of bottled contents
}

bool Item::IsMajorItem() const {
    using namespace Settings;
    if (type == ITEMTYPE_TOKEN) {
        return Bridge.Is(RAINBOWBRIDGE_TOKENS) || LACSCondition == LACSCONDITION_TOKENS;
    }

    if (type == ITEMTYPE_DROP || type == ITEMTYPE_EVENT || type == ITEMTYPE_SHOP || type == ITEMTYPE_MAP ||
        type == ITEMTYPE_COMPASS) {
        return false;
    }

    if (type == ITEMTYPE_DUNGEONREWARD && (ShuffleRewards.Is(REWARDSHUFFLE_END_OF_DUNGEON))) {
        return false;
    }

    if ((randomizerGet == RG_BOMBCHU_5 || randomizerGet == RG_BOMBCHU_10 || randomizerGet == RG_BOMBCHU_20) &&
        !BombchusInLogic) {
        return false;
    }

    if (randomizerGet == RG_HEART_CONTAINER || randomizerGet == RG_PIECE_OF_HEART ||
        randomizerGet == RG_TREASURE_GAME_HEART) {
        return false;
    }

    if (type == ITEMTYPE_SMALLKEY && (Keysanity.Is(KEYSANITY_VANILLA) || Keysanity.Is(KEYSANITY_OWN_DUNGEON))) {
        return false;
    }

    if (type == ITEMTYPE_FORTRESS_SMALLKEY && GerudoKeys.Is(GERUDOKEYS_VANILLA)) {
        return false;
    }

    if ((type == ITEMTYPE_BOSSKEY && getItemId != 0xAD) &&
        (BossKeysanity.Is(BOSSKEYSANITY_VANILLA) || BossKeysanity.Is(BOSSKEYSANITY_OWN_DUNGEON))) {
        return false;
    }
    // Ganons Castle Boss Key
    if (getItemId == 0xAD && (GanonsBossKey.Is(GANONSBOSSKEY_VANILLA) || GanonsBossKey.Is(GANONSBOSSKEY_OWN_DUNGEON))) {
        return false;
    }

    if (randomizerGet == RG_GREG_RUPEE) {
        return Bridge.Is(RAINBOWBRIDGE_GREG);
    }

    return IsAdvancement();
}

RandomizerHintTextKey Item::GetHintKey() const {
    return hintKey;
}

const HintText& Item::GetHint() const {
    return Hint(hintKey);
}

bool Item::operator==(const Item& right) const {
    return type == right.GetItemType() && getItemId == right.GetItemID();
}

bool Item::operator!=(const Item& right) const {
    return !operator==(right);
}
}
