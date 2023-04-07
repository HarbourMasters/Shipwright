#include "item.h"

#include "3drando/logic.hpp"
#include "3drando/item_location.hpp"
#include "3drando/random.hpp"
#include "3drando/item_pool.hpp"
#include "z64item.h"

RandoItem::RandoItem(RandomizerGet randomizerGet_, Text name_, ItemType type_, int16_t getItemId_, bool advancement_,
                     bool* logicVar_, uint32_t hintKey_, uint16_t itemId_, uint16_t objectId_, uint16_t gid_,
                     uint16_t textId_, uint16_t field_, int16_t chestAnimation_, GetItemCategory category_,
                     uint16_t modIndex_, bool progressive_, uint16_t price_)
    : randomizerGet(randomizerGet_), name(std::move(name_)), type(type_), getItemId(getItemId_),
      advancement(advancement_), logicVar(logicVar_), hintKey(hintKey_), progressive(progressive_), price(price_) {
    giEntry = std::make_shared<GetItemEntry>(std::initializer_list<GetItemEntry>{
        GET_ITEM(itemId_, objectId_, gid_, textId_, field_, chestAnimation_, category_, modIndex_, getItemId_) });
}

RandoItem::RandoItem(RandomizerGet randomizerGet_, Text name_, ItemType type_, int16_t getItemId_, bool advancement_,
                     uint8_t* logicVar_, uint32_t hintKey_, uint16_t itemId_, uint16_t objectId_, uint16_t gid_,
                     uint16_t textId_, uint16_t field_, int16_t chestAnimation_, GetItemCategory category_,
                     uint16_t modIndex_, bool progressive_, uint16_t price_)
    : randomizerGet(randomizerGet_), name(std::move(name_)), type(type_), getItemId(getItemId_),
      advancement(advancement_), logicVar(logicVar_), hintKey(hintKey_), progressive(progressive_), price(price_) {
    giEntry = std::make_shared<GetItemEntry>(std::initializer_list<GetItemEntry>{
        GET_ITEM(itemId_, objectId_, gid_, textId_, field_, chestAnimation_, category_, modIndex_, getItemId_) });
}

RandoItem::RandoItem(RandomizerGet randomizerGet_, Text name_, ItemType type_, int getItemId_, bool advancement_,
                     bool* logicVar_, uint32_t hintKey_, bool progressive_, uint16_t price_)
    : randomizerGet(randomizerGet_), name(std::move(name_)), type(type_), getItemId(getItemId_),
      advancement(advancement_), logicVar(logicVar_), hintKey(hintKey_), progressive(progressive_), price(price_) {
}

RandoItem::RandoItem(RandomizerGet randomizerGet_, Text name_, ItemType type_, int getItemId_, bool advancement_,
                     uint8_t* logicVar_, uint32_t hintKey_, bool progressive_, uint16_t price_)
    : randomizerGet(randomizerGet_), name(std::move(name_)), type(type_), getItemId(getItemId_),
      advancement(advancement_), logicVar(logicVar_), hintKey(hintKey_), progressive(progressive_), price(price_) {
}

    RandoItem::~RandoItem() = default;

void RandoItem::ApplyEffect() {
    // If this is a key ring, logically add as many keys as we could need
    if (RG_FOREST_TEMPLE_KEY_RING <= hintKey && hintKey <= RG_GANONS_CASTLE_KEY_RING) {
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

void RandoItem::UndoEffect() {
    if (RG_FOREST_TEMPLE_KEY_RING <= hintKey && hintKey <= RG_GANONS_CASTLE_KEY_RING) {
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

ItemOverride_Value RandoItem::Value() const {
    ItemOverride_Value val;

    val.all = 0;
    val.itemId = getItemId;
    if (getItemId == RG_ICE_TRAP) {
        val.looksLikeItemId = RandomElement(IceTrapModels);
    }
    if (!Settings::ColoredBossKeys && (getItemId >= 0x95 && getItemId <= 0x9A)) { // Boss keys
        val.looksLikeItemId = GI_KEY_BOSS;
    }
    if (!Settings::ColoredKeys && (getItemId >= 0xAF && getItemId <= 0xB7)) { // Small keys
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

const Text& RandoItem::GetName() const {
    return name;
}

bool RandoItem::IsAdvancement() const {
    return advancement;
}

int RandoItem::GetItemID() const {
    return getItemId;
}

ItemType RandoItem::GetItemType() const {
    return type;
}

RandomizerGet RandoItem::GetRandomizerGet() {
    return randomizerGet;
}

uint16_t RandoItem::GetPrice() const {
    return price;
}

void RandoItem::SetPrice(uint16_t price_) {
    price = price_;
}

void RandoItem::SetAsPlaythrough() {
    playthrough = true;
}

bool RandoItem::IsPlaythrough() const {
    return playthrough;
}

bool RandoItem::IsBottleItem() const {
    return getItemId == 0x0F ||                      // Empty Bottle
           getItemId == 0X14 ||                      // Bottle with Milk
           (getItemId >= 0x8C && getItemId <= 0x94); // Rest of bottled contents
}

bool RandoItem::IsMajorItem() const {
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

    if (hintKey == RG_HEART_CONTAINER || hintKey == RG_PIECE_OF_HEART || hintKey == RG_TREASURE_GAME_HEART) {
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

const uint32_t RandoItem::GetHintKey() const {
    return hintKey;
}

const HintText& RandoItem::GetHint() const {
    return Hint(hintKey);
}

bool RandoItem::operator==(const RandoItem& right) const {
    return type == right.GetItemType() && getItemId == right.GetItemID();
}

bool RandoItem::operator!=(const RandoItem& right) const {
    return !operator==(right);
}
