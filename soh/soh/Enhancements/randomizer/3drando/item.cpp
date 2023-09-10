#include "item.hpp"

#include <array>

#include "item_location.hpp"
#include "logic.hpp"
#include "random.hpp"
#include "item_pool.hpp"
#include "settings.hpp"
#include "z64item.h"

Item::Item(RandomizerGet randomizerGet_, Text name_, ItemType type_, int getItemId_, bool advancement_, bool* logicVar_, uint32_t hintKey_,
           uint16_t price_)
    : randomizerGet(randomizerGet_),
      name(std::move(name_)),
      type(type_),
      getItemId(getItemId_),
      advancement(advancement_),
      logicVar(logicVar_),
      hintKey(hintKey_),
      price(price_) {}

Item::Item(RandomizerGet randomizerGet_, Text name_, ItemType type_, int getItemId_, bool advancement_, uint8_t* logicVar_, uint32_t hintKey_,
           uint16_t price_)
    : randomizerGet(randomizerGet_),
      name(std::move(name_)),
      type(type_),
      getItemId(getItemId_),
      advancement(advancement_),
      logicVar(logicVar_),
      hintKey(hintKey_),
      price(price_) {}

Item::~Item() = default;

void Item::ApplyEffect() {
    //If this is a key ring, logically add as many keys as we could need
    if (FOREST_TEMPLE_KEY_RING <= hintKey && hintKey <= GANONS_CASTLE_KEY_RING) {
        *std::get<uint8_t*>(logicVar) += 10;
    }
    else {
        if (std::holds_alternative<bool*>(logicVar)) {
            *std::get<bool*>(logicVar) = true;
        } else {
            *std::get<uint8_t*>(logicVar) += 1;
        }
    }
    Logic::UpdateHelpers();
}

void Item::UndoEffect() {
    if (FOREST_TEMPLE_KEY_RING <= hintKey && hintKey <= GANONS_CASTLE_KEY_RING) {
        *std::get<uint8_t*>(logicVar) -= 10;
    }
    else {
        if (std::holds_alternative<bool*>(logicVar)) {
            *std::get<bool*>(logicVar) = false;
        } else {
            *std::get<uint8_t*>(logicVar) -= 1;
        }
    }
    Logic::UpdateHelpers();
}

ItemOverride_Value Item::Value() const {
    ItemOverride_Value val;

    val.all = 0;
    val.itemId = getItemId;
    if (getItemId == GI_ICE_TRAP) {
        val.looksLikeItemId = RandomElement(IceTrapModels);
    }
    if (getItemId >= 0x95 && getItemId <= 0x9A) { //Boss keys
        val.looksLikeItemId = GI_KEY_BOSS;
    }
    if (getItemId >= 0xAF && getItemId <= 0xB7) { //Small keys
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
