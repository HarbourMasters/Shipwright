#include "item.h"
#include "item_location.h"

#include "context.h"
#include "logic.h"
#include "3drando/item_pool.hpp"
#include "z64item.h"
#include "variables.h"
#include "macros.h"
#include "functions.h"
#include "../../OTRGlobals.h"

namespace Rando {
Item::Item() : randomizerGet(RG_NONE), type(ITEMTYPE_ITEM), getItemId(GI_NONE), advancement(false), hintKey(RHT_NONE),
               progressive(false), price(0) {}
Item::Item(const RandomizerGet randomizerGet_, Text name_, const ItemType type_, const int16_t getItemId_,
    const bool advancement_, LogicVal logicVal_, const RandomizerHintTextKey hintKey_, const uint16_t itemId_,
    const uint16_t objectId_, const uint16_t gid_, const uint16_t textId_, const uint16_t field_,
    const int16_t chestAnimation_, const GetItemCategory category_, const uint16_t modIndex_,
    const bool progressive_, const uint16_t price_)
    : randomizerGet(randomizerGet_), name(std::move(name_)), type(type_), getItemId(getItemId_),
    advancement(advancement_), logicVal(logicVal_), hintKey(hintKey_), progressive(progressive_), price(price_) {
    if (modIndex_ == MOD_RANDOMIZER || getItemId > 0x7D) {
        giEntry = std::make_shared<GetItemEntry>(GetItemEntry{ itemId_, field_, static_cast<int16_t>((chestAnimation_ != CHEST_ANIM_SHORT ? 1 : -1) * (gid_ + 1)), textId_, objectId_, modIndex_, TABLE_RANDOMIZER, static_cast<int16_t>(randomizerGet_), gid_, true, ITEM_FROM_NPC, category_, static_cast<uint16_t>(randomizerGet_), MOD_RANDOMIZER, NULL });
    }
    else {
        giEntry = std::make_shared<GetItemEntry>(GetItemEntry{ itemId_, field_, static_cast<int16_t>((chestAnimation_ != CHEST_ANIM_SHORT ? 1 : -1) * (gid_ + 1)), textId_, objectId_, modIndex_, TABLE_VANILLA, getItemId_, gid_, true, ITEM_FROM_NPC, category_, itemId_, modIndex_, NULL });
    }
}

Item::Item(const RandomizerGet randomizerGet_, Text name_, const ItemType type_, const int16_t getItemId_,
    const bool advancement_, LogicVal logicVal_, const RandomizerHintTextKey hintKey_, const bool progressive_,
    const uint16_t price_)
    : randomizerGet(randomizerGet_), name(std::move(name_)), type(type_), getItemId(getItemId_),
    advancement(advancement_), logicVal(logicVal_), hintKey(hintKey_), progressive(progressive_), price(price_) {
}

    Item::~Item() = default;

void Item::ApplyEffect() const {
    auto ctx = Rando::Context::GetInstance();
    ctx->GetLogic()->ApplyItemEffect(StaticData::RetrieveItem(randomizerGet), true);
    ctx->GetLogic()->SetInLogic(logicVal, true);
}

void Item::UndoEffect() const {
    auto ctx = Rando::Context::GetInstance();
    ctx->GetLogic()->ApplyItemEffect(StaticData::RetrieveItem(randomizerGet), false);
    ctx->GetLogic()->SetInLogic(logicVal, false);
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

LogicVal Item::GetLogicVal() const {
    return logicVal;
}

RandomizerGet Item::GetRandomizerGet() const {
    return randomizerGet;
}

uint16_t Item::GetPrice() const {
    return price;
}

std::shared_ptr<GetItemEntry> Item::GetGIEntry() const { // NOLINT(*-no-recursion)
    if (giEntry != nullptr) {
        return giEntry;
    }
    std::shared_ptr<Rando::Context> ctx = Rando::Context::GetInstance();
    auto logic = ctx->GetLogic();
    RandomizerGet actual = RG_NONE;
    const bool tycoonWallet = OTRGlobals::Instance->gRandomizer->GetRandoSettingValue(RSK_INCLUDE_TYCOON_WALLET);
    const u8 infiniteUpgrades = OTRGlobals::Instance->gRandomizer->GetRandoSettingValue(RSK_INFINITE_UPGRADES);
    switch (randomizerGet) {
        case RG_PROGRESSIVE_STICK_UPGRADE:
            switch (logic->CurrentUpgrade(UPG_STICKS)) {
                case 0:
                    if (ctx->GetOption(RSK_SHUFFLE_DEKU_STICK_BAG)) {
                        actual = RG_DEKU_STICK_BAG;
                        break;
                    }
                case 1:
                    if (infiniteUpgrades == RO_INF_UPGRADES_CONDENSED_PROGRESSIVE) {
                        actual = RG_STICK_UPGRADE_INF;
                    } else {
                        actual = RG_DEKU_STICK_CAPACITY_20;
                    }
                    break;
                case 2:
                    actual = RG_DEKU_STICK_CAPACITY_30;
                    break;
                case 3:
                case 4:
                    if (infiniteUpgrades == RO_INF_UPGRADES_PROGRESSIVE) {
                        actual = RG_STICK_UPGRADE_INF;
                    } else {
                        actual = RG_DEKU_STICK_CAPACITY_30;
                    }
                    break;
                default:
                    break;
            }
            break;
        case RG_PROGRESSIVE_NUT_UPGRADE:
            switch (logic->CurrentUpgrade(UPG_NUTS)) {
                case 0:
                    if (ctx->GetOption(RSK_SHUFFLE_DEKU_NUT_BAG)) {
                        actual = RG_DEKU_NUT_BAG;
                        break;
                    }
                case 1:
                    if (infiniteUpgrades == RO_INF_UPGRADES_CONDENSED_PROGRESSIVE) {
                        actual = RG_NUT_UPGRADE_INF;
                    } else {
                        actual = RG_DEKU_NUT_CAPACITY_30;
                    }
                    break;
                case 2:
                    actual = RG_DEKU_NUT_CAPACITY_40;
                    break;
                case 3:
                case 4:
                    if (infiniteUpgrades == RO_INF_UPGRADES_PROGRESSIVE) {
                        actual = RG_NUT_UPGRADE_INF;
                    } else {
                        actual = RG_DEKU_NUT_CAPACITY_40;
                    }
                    break;
                default:
                    break;
            }
            break;
        case RG_PROGRESSIVE_BOMB_BAG:
            switch (logic->CurrentUpgrade(UPG_BOMB_BAG)) {
                case 0:
                    actual = RG_BOMB_BAG;
                    break;
                case 1:
                    if (infiniteUpgrades == RO_INF_UPGRADES_CONDENSED_PROGRESSIVE) {
                        actual = RG_BOMB_BAG_INF;
                    } else {
                        actual = RG_BIG_BOMB_BAG;
                    }
                    break;
                case 2:
                    actual = RG_BIGGEST_BOMB_BAG;
                    break;
                case 3:
                case 4:
                    if (infiniteUpgrades == RO_INF_UPGRADES_PROGRESSIVE) {
                        actual = RG_BOMB_BAG_INF;
                    } else {
                        actual = RG_BIGGEST_BOMB_BAG;
                    }
                    break;
                default:
                    break;
            }
            break;
        case RG_PROGRESSIVE_BOW:
            switch (logic->CurrentUpgrade(UPG_QUIVER)) {
                case 0:
                    actual = RG_FAIRY_BOW;
                    break;
                case 1:
                    if (infiniteUpgrades == RO_INF_UPGRADES_CONDENSED_PROGRESSIVE) {
                        actual = RG_QUIVER_INF;
                    } else {
                        actual = RG_BIG_QUIVER;
                    }
                    break;
                case 2:
                    actual = RG_BIGGEST_QUIVER;
                    break;
                case 3:
                case 4:
                    if (infiniteUpgrades == RO_INF_UPGRADES_PROGRESSIVE) {
                        actual = RG_QUIVER_INF;
                    } else {
                        actual = RG_BIGGEST_QUIVER;
                    }
                    break;
                default:
                    break;
            }
            break;
        case RG_PROGRESSIVE_SLINGSHOT:
            switch (logic->CurrentUpgrade(UPG_BULLET_BAG)) {
                case 0:
                    actual = RG_FAIRY_SLINGSHOT;
                    break;
                case 1:
                    if (infiniteUpgrades == RO_INF_UPGRADES_CONDENSED_PROGRESSIVE) {
                        actual = RG_BULLET_BAG_INF;
                    } else {
                        actual = RG_BIG_BULLET_BAG;
                    }
                    break;
                case 2:
                    actual = RG_BIGGEST_BULLET_BAG;
                    break;
                case 3:
                case 4:
                    if (infiniteUpgrades == RO_INF_UPGRADES_PROGRESSIVE) {
                        actual = RG_BULLET_BAG_INF;
                    } else {
                        actual = RG_BIGGEST_BULLET_BAG;
                    }
                    break;
                default:
                    break;
            }
            break;
        case RG_PROGRESSIVE_OCARINA:
            switch (logic->CurrentInventory(ITEM_OCARINA_FAIRY)) {
                case ITEM_NONE:
                    actual = RG_FAIRY_OCARINA;
                    break;
                case ITEM_OCARINA_FAIRY:
                case ITEM_OCARINA_TIME:
                    actual = RG_OCARINA_OF_TIME;
                    break;
                default:
                    break;
            }
            break;
        case RG_PROGRESSIVE_HOOKSHOT:
            switch (logic->CurrentInventory(ITEM_HOOKSHOT)) {
                case ITEM_NONE:
                    actual = RG_HOOKSHOT;
                    break;
                case ITEM_HOOKSHOT:
                case ITEM_LONGSHOT:
                    actual = RG_LONGSHOT;
                    break;
                default:
                    break;
            }
            break;
        case RG_PROGRESSIVE_STRENGTH:
            switch (logic->CurrentUpgrade(UPG_STRENGTH)) {
                case 0:
                    actual = RG_GORONS_BRACELET;
                    break;
                case 1:
                    actual = RG_SILVER_GAUNTLETS;
                    break;
                case 2:
                case 3:
                    actual = RG_GOLDEN_GAUNTLETS;
                    break;
                default:
                    break;
            }
            break;
        case RG_PROGRESSIVE_WALLET:
            if (!logic->CheckRandoInf(RAND_INF_HAS_WALLET)) {
                actual = RG_CHILD_WALLET;
                break;
            }
            switch (logic->CurrentUpgrade(UPG_WALLET)) {
                case 0:
                    actual = RG_ADULT_WALLET;
                    break;
                case 1:
                    actual = RG_GIANT_WALLET;
                    break;
                case 2:
                    if(tycoonWallet){
                        actual = RG_TYCOON_WALLET;
                        break;
                    }
                    //fallthrough
                case 3:
                case 4:
                    if (infiniteUpgrades != RO_INF_UPGRADES_OFF) {
                        actual = RG_WALLET_INF;
                    } else {
                        actual = tycoonWallet ? RG_TYCOON_WALLET : RG_GIANT_WALLET;
                    }
                    break;
                default:
                    break;
            }
            break;
        case RG_PROGRESSIVE_SCALE:
            if (!logic->CheckRandoInf(RAND_INF_CAN_SWIM)) {
                actual = RG_BRONZE_SCALE;
                break;
            }
            switch (logic->CurrentUpgrade(UPG_SCALE)) {
                case 0:
                    actual = RG_SILVER_SCALE;
                    break;
                case 1:
                case 2:
                    actual = RG_GOLDEN_SCALE;
                    break;
                default:
                    break;
            }
            break;
        case RG_PROGRESSIVE_MAGIC_METER:
            switch (logic->GetSaveContext()->magicLevel) {
                case 0:
                    actual = RG_MAGIC_SINGLE;
                    break;
                case 1:
                    actual = RG_MAGIC_DOUBLE;
                    break;
                case 2:
                case 3:
                    if (infiniteUpgrades != RO_INF_UPGRADES_OFF) {
                        actual = RG_MAGIC_INF;
                    } else {
                        actual = RG_MAGIC_DOUBLE;
                    }
                    break;
                default:
                    break;
            }
            break;
        case RG_PROGRESSIVE_GORONSWORD: // todo progressive?
           actual = RG_BIGGORON_SWORD;
           break;
        case RG_PROGRESSIVE_BOMBCHUS:
            if (logic->CurrentInventory(ITEM_BOMBCHU) == ITEM_NONE) {
                actual = RG_BOMBCHU_20;
            } else if (infiniteUpgrades != RO_INF_UPGRADES_OFF) {
                actual = RG_BOMBCHU_INF;
            } else if (logic->GetAmmo(ITEM_BOMBCHU) < 5) {
                actual = RG_BOMBCHU_10;
            } else {
                actual = RG_BOMBCHU_5;
            }
            break;
        default:
            actual = RG_NONE;
            break;
    }
    return StaticData::RetrieveItem(actual).GetGIEntry();
}

GetItemEntry Item::GetGIEntry_Copy() const {
    return *GetGIEntry();
}

void Item::SetPrice(const uint16_t price_) {
    price = price_;
}

void Item::SetAsPlaythrough() {
    playthrough = true;
}

void Item::SetCustomDrawFunc(const CustomDrawFunc drawFunc) const {
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
    const auto ctx = Context::GetInstance();
    if (type == ITEMTYPE_TOKEN) {
        return ctx->GetOption(RSK_RAINBOW_BRIDGE).Is(RO_BRIDGE_TOKENS) || ctx->GetSettings()->LACSCondition() == RO_LACS_TOKENS;
    }

    if (type == ITEMTYPE_DROP || type == ITEMTYPE_EVENT || type == ITEMTYPE_SHOP || type == ITEMTYPE_MAP ||
        type == ITEMTYPE_COMPASS) {
        return false;
    }

    if (type == ITEMTYPE_DUNGEONREWARD && ctx->GetOption(RSK_SHUFFLE_DUNGEON_REWARDS).Is(RO_DUNGEON_REWARDS_END_OF_DUNGEON)) {
        return false;
    }

    if ((randomizerGet == RG_BOMBCHU_5 || randomizerGet == RG_BOMBCHU_10 || randomizerGet == RG_BOMBCHU_20) &&
        !ctx->GetOption(RSK_BOMBCHUS_IN_LOGIC)) {
        return false;
    }

    if (randomizerGet == RG_HEART_CONTAINER || randomizerGet == RG_PIECE_OF_HEART ||
        randomizerGet == RG_TREASURE_GAME_HEART) {
        return false;
    }

    if (type == ITEMTYPE_SMALLKEY && (ctx->GetOption(RSK_KEYSANITY).Is(RO_DUNGEON_ITEM_LOC_VANILLA) || ctx->GetOption(RSK_KEYSANITY).Is(RO_DUNGEON_ITEM_LOC_OWN_DUNGEON))) {
        return false;
    }

    if (type == ITEMTYPE_FORTRESS_SMALLKEY && ctx->GetOption(RSK_GERUDO_KEYS).Is(RO_GERUDO_KEYS_VANILLA)) {
        return false;
    }

    if (type == ITEMTYPE_BOSSKEY && getItemId != 0xAD &&
        (ctx->GetOption(RSK_BOSS_KEYSANITY).Is(RO_DUNGEON_ITEM_LOC_VANILLA) || ctx->GetOption(RSK_BOSS_KEYSANITY).Is(RO_DUNGEON_ITEM_LOC_OWN_DUNGEON))) {
        return false;
    }
    // Ganons Castle Boss Key
    if (getItemId == 0xAD && (ctx->GetOption(RSK_GANONS_BOSS_KEY).Is(RO_GANON_BOSS_KEY_VANILLA) || ctx->GetOption(RSK_GANONS_BOSS_KEY).Is(RO_GANON_BOSS_KEY_OWN_DUNGEON))) {
        return false;
    }

    if (randomizerGet == RG_GREG_RUPEE) {
        return ctx->GetOption(RSK_RAINBOW_BRIDGE).Is(RO_BRIDGE_GREG);
    }

    return IsAdvancement();
}

RandomizerHintTextKey Item::GetHintKey() const {
    return hintKey;
}

const HintText& Item::GetHint() const {
    return StaticData::hintTextTable[hintKey];
}

bool Item::operator==(const Item& right) const {
    return type == right.GetItemType() && getItemId == right.GetItemID();
}

bool Item::operator!=(const Item& right) const {
    return !operator==(right);
}
}
