#include "context.h"
#include "static_data.h"
#include "soh/OTRGlobals.h"
#include "soh/Enhancements/item-tables/ItemTableManager.h"
#include "3drando/shops.hpp"
#include "dungeon.h"
#include "logic.h"
#include "entrance.h"
#include "settings.h"
#include "rando_hash.h"
#include "fishsanity.h"
#include "macros.h"
#include "3drando/hints.hpp"

#include <fstream>
#include <spdlog/spdlog.h>

namespace Rando {
std::weak_ptr<Context> Context::mContext;

Context::Context() {

    for (int i = 0; i < RC_MAX; i++) {
        itemLocationTable[i] = ItemLocation(static_cast<RandomizerCheck>(i));
    }
    mEntranceShuffler = std::make_shared<EntranceShuffler>();
    mDungeons = std::make_shared<Dungeons>();
    mLogic = std::make_shared<Logic>();
    mTrials = std::make_shared<Trials>();
    mSettings = std::make_shared<Settings>();
    mFishsanity = std::make_shared<Fishsanity>();
}

RandomizerArea Context::GetAreaFromString(std::string str) {
    return (RandomizerArea)StaticData::areaNameToEnum[str];
}

void Context::InitStaticData() {
    StaticData::HintTable_Init();
    StaticData::trialNameToEnum = StaticData::PopulateTranslationMap(StaticData::trialData);
    StaticData::hintNameToEnum = StaticData::PopulateTranslationMap(StaticData::hintNames);
    StaticData::hintTypeNameToEnum = StaticData::PopulateTranslationMap(StaticData::hintTypeNames);
    StaticData::areaNameToEnum = StaticData::PopulateTranslationMap(StaticData::areaNames);
    StaticData::InitLocationTable();
}

std::shared_ptr<Context> Context::CreateInstance() {
    if (mContext.expired()) {
        auto instance = std::make_shared<Context>();
        mContext = instance;
        GetInstance()->GetLogic()->SetContext(GetInstance());
        return instance;
    }
    return GetInstance();
}

std::shared_ptr<Context> Context::GetInstance() {
    return mContext.lock();
}

Hint* Context::GetHint(const RandomizerHint hintKey) {
    return &hintTable[hintKey];
}

void Context::AddHint(const RandomizerHint hintId, const Hint hint) {
    hintTable[hintId] = hint; //RANDOTODO this should probably be an rvalue
}

ItemLocation* Context::GetItemLocation(const RandomizerCheck locKey) {
    return &itemLocationTable[locKey];
}

ItemLocation* Context::GetItemLocation(size_t locKey) {
    return &itemLocationTable[static_cast<RandomizerCheck>(locKey)];
}

ItemOverride& Context::GetItemOverride(RandomizerCheck locKey) {
    if (!overrides.contains(locKey)) {
        overrides.emplace(locKey, ItemOverride());
    }
    return overrides.at(locKey);
}

ItemOverride& Context::GetItemOverride(size_t locKey) {
    if (!overrides.contains(static_cast<RandomizerCheck>(locKey))) {
        overrides.emplace(static_cast<RandomizerCheck>(locKey), ItemOverride());
    }
    return overrides.at(static_cast<RandomizerCheck>(locKey));
}

void Context::PlaceItemInLocation(const RandomizerCheck locKey, const RandomizerGet item, const bool applyEffectImmediately,
                                  const bool setHidden) {
    const auto loc = GetItemLocation(locKey);
    SPDLOG_DEBUG("\n");
    SPDLOG_DEBUG(StaticData::RetrieveItem(item).GetName().GetEnglish());
    SPDLOG_DEBUG(" placed at ");
    SPDLOG_DEBUG(StaticData::GetLocation(locKey)->GetName());
    SPDLOG_DEBUG("\n\n");

    if (applyEffectImmediately || mSettings->GetOption(RSK_LOGIC_RULES).Is(RO_LOGIC_GLITCHLESS) || mSettings->GetOption(RSK_LOGIC_RULES).Is(RO_LOGIC_VANILLA)) {
        StaticData::RetrieveItem(item).ApplyEffect();
    }

    // TODO? Show Progress

    // If we're placing a non-shop item in a shop location, we want to record it for custom messages
    if (StaticData::RetrieveItem(item).GetItemType() != ITEMTYPE_SHOP &&
        StaticData::GetLocation(locKey)->IsCategory(Category::cShop)) {
        const int index = TransformShopIndex(GetShopIndex(locKey));
        NonShopItems[index].Name = StaticData::RetrieveItem(item).GetName();
        NonShopItems[index].Repurchaseable =
            StaticData::RetrieveItem(item).GetItemType() == ITEMTYPE_REFILL ||
            StaticData::RetrieveItem(item).GetHintKey() == RHT_PROGRESSIVE_BOMBCHUS;
    }

    loc->SetPlacedItem(item);
    if (setHidden) {
        loc->SetHidden(true);
    }
}

void Context::AddLocation(const RandomizerCheck loc, std::vector<RandomizerCheck>* destination) {
    if (destination == nullptr) {
        destination = &allLocations;
    }
    destination->push_back(loc);
}

template <typename Container>
void Context::AddLocations(const Container& locations, std::vector<RandomizerCheck>* destination) {
    if (destination == nullptr) {
        destination = &allLocations;
    }
    destination->insert(destination->end(), std::cbegin(locations), std::cend(locations));
}

void Context::GenerateLocationPool() {
    allLocations.clear();
    AddLocation(RC_LINKS_POCKET);
    if (mSettings->GetOption(RSK_TRIFORCE_HUNT)) {
        AddLocation(RC_TRIFORCE_COMPLETED);
    }
    AddLocations(StaticData::overworldLocations);

    if (mSettings->GetOption(RSK_FISHSANITY).IsNot(RO_FISHSANITY_OFF)) {
        AddLocations(mFishsanity->GetFishsanityLocations().first);
    }

    for (const auto dungeon : mDungeons->GetDungeonList()) {
        AddLocations(dungeon->GetDungeonLocations());
    }
}

void Context::AddExcludedOptions() {
    AddLocations(StaticData::overworldLocations, &everyPossibleLocation);
    for (const auto dungeon : mDungeons->GetDungeonList()) {
        AddLocations(dungeon->GetEveryLocation(), &everyPossibleLocation);
    }
    for (const RandomizerCheck rc : everyPossibleLocation) {
        GetItemLocation(rc)->AddExcludeOption();
    }
}

std::vector<RandomizerCheck> Context::GetLocations(const std::vector<RandomizerCheck>& locationPool,
                                                   const Category categoryInclude, const Category categoryExclude) {
    std::vector<RandomizerCheck> locationsInCategory;
    for (RandomizerCheck locKey : locationPool) {
        if (StaticData::GetLocation(locKey)->IsCategory(categoryInclude) &&
            !StaticData::GetLocation(locKey)->IsCategory(categoryExclude)) {
            locationsInCategory.push_back(locKey);
        }
    }
    return locationsInCategory;
}

void Context::ClearItemLocations() {
    for (size_t i = 0; i < itemLocationTable.size(); i++) {
        GetItemLocation(static_cast<RandomizerCheck>(i))->ResetVariables();
    }
}

void Context::ItemReset() {
    for (const RandomizerCheck il : allLocations) {
        GetItemLocation(il)->ResetVariables();
    }

    for (const RandomizerCheck il : StaticData::dungeonRewardLocations) {
        GetItemLocation(il)->ResetVariables();
    }
}

void Context::LocationReset() {
    for (const RandomizerCheck il : allLocations) {
        GetItemLocation(il)->RemoveFromPool();
    }

    for (const RandomizerCheck il : StaticData::dungeonRewardLocations) {
        GetItemLocation(il)->RemoveFromPool();
    }

    for (const RandomizerCheck il : StaticData::gossipStoneLocations) {
        GetItemLocation(il)->RemoveFromPool();
    }

    for (const RandomizerCheck il : StaticData::staticHintLocations) {
        GetItemLocation(il)->RemoveFromPool();
    }
}

void Context::HintReset() {
    for (const RandomizerCheck il : StaticData::gossipStoneLocations) {
        GetItemLocation(il)->ResetVariables();
    }
    for (Hint& hint : hintTable){
        hint.ResetVariables();
    }
}

void Context::CreateItemOverrides() {
    SPDLOG_DEBUG("NOW CREATING OVERRIDES\n\n");
    for (RandomizerCheck locKey : allLocations) {
        const auto loc = StaticData::GetLocation(locKey);
        // If this is an ice trap, store the disguise model in iceTrapModels
        const auto itemLoc = GetItemLocation(locKey);
        if (itemLoc->GetPlacedRandomizerGet() == RG_ICE_TRAP) {
            ItemOverride val(locKey, RandomElement(possibleIceTrapModels));
            iceTrapModels[locKey] = val.LooksLike();
            val.SetTrickName(GetIceTrapName(val.LooksLike()));
            // If this is ice trap is in a shop, change the name based on what the model will look like
            if (loc->IsCategory(Category::cShop)) {
                NonShopItems[TransformShopIndex(GetShopIndex(locKey))].Name = val.GetTrickName();
            }
            overrides[locKey] = val;
        }
        SPDLOG_DEBUG(loc->GetName());
        SPDLOG_DEBUG(": ");
        SPDLOG_DEBUG(itemLoc->GetPlacedItemName().GetEnglish());
        SPDLOG_DEBUG("\n");
    }
    SPDLOG_DEBUG("Overrides Created: ");
    SPDLOG_DEBUG(std::to_string(overrides.size()));
}

bool Context::IsSeedGenerated() const {
    return mSeedGenerated;
}

void Context::SetSeedGenerated(const bool seedGenerated) {
    mSeedGenerated = seedGenerated;
}

bool Context::IsSpoilerLoaded() const {
    return mSpoilerLoaded;
}

void Context::SetSpoilerLoaded(const bool spoilerLoaded) {
    mSpoilerLoaded = spoilerLoaded;
}

bool Context::IsPlandoLoaded() const {
    return mPlandoLoaded;
}

void Context::SetPlandoLoaded(const bool plandoLoaded) {
    mPlandoLoaded = plandoLoaded;
}

GetItemEntry Context::GetFinalGIEntry(const RandomizerCheck rc, const bool checkObtainability, const GetItemID ogItemId) {
    const auto itemLoc = GetItemLocation(rc);
    if (itemLoc->GetPlacedRandomizerGet() == RG_NONE) {
        if (ogItemId != GI_NONE) {
            return ItemTableManager::Instance->RetrieveItemEntry(MOD_NONE, ogItemId);
        }
        return ItemTableManager::Instance->RetrieveItemEntry(
            MOD_NONE, StaticData::RetrieveItem(StaticData::GetLocation(rc)->GetVanillaItem()).GetItemID());
    }
    if (checkObtainability && OTRGlobals::Instance->gRandomizer->GetItemObtainabilityFromRandomizerGet(
                                  itemLoc->GetPlacedRandomizerGet()) != CAN_OBTAIN) {
        return ItemTableManager::Instance->RetrieveItemEntry(MOD_NONE, GI_RUPEE_BLUE);
    }
    GetItemEntry giEntry = itemLoc->GetPlacedItem().GetGIEntry_Copy();
    if (overrides.contains(rc)) {
        const auto fakeGiEntry = StaticData::RetrieveItem(overrides[rc].LooksLike()).GetGIEntry();
        giEntry.gid = fakeGiEntry->gid;
        giEntry.gi = fakeGiEntry->gi;
        giEntry.drawItemId = fakeGiEntry->drawItemId;
        giEntry.drawModIndex = fakeGiEntry->drawModIndex;
        giEntry.drawFunc = fakeGiEntry->drawFunc;
    }
    return giEntry;
}

std::string sanitize(std::string stringValue) {
    // Add backslashes.
    for (auto i = stringValue.begin();;) {
        auto const pos =
            std::find_if(i, stringValue.end(), [](char const c) { return '\\' == c || '\'' == c || '"' == c; });
        if (pos == stringValue.end()) {
            break;
        }
        i = std::next(stringValue.insert(pos, '\\'), 2);
    }

    // Removes others.
    std::erase_if(stringValue, [](char const c) { return '\n' == c || '\r' == c || '\0' == c || '\x1A' == c; });

    return stringValue;
}

void Context::ParseSpoiler(const char* spoilerFileName, const bool plandoMode) {
    std::ifstream spoilerFileStream(sanitize(spoilerFileName));
    if (!spoilerFileStream) {
        return;
    }
    mSeedGenerated = false;
    mSpoilerLoaded = false;
    mPlandoLoaded = false;
    try {
        nlohmann::json spoilerFileJson;
        spoilerFileStream >> spoilerFileJson;
        ParseHashIconIndexesJson(spoilerFileJson);
        mSettings->ParseJson(spoilerFileJson);
        if (plandoMode) {
            ParseItemLocationsJson(spoilerFileJson);
            ParseHintJson(spoilerFileJson);
            mEntranceShuffler->ParseJson(spoilerFileJson);
            mDungeons->ParseJson(spoilerFileJson);
            mTrials->ParseJson(spoilerFileJson);
            mPlandoLoaded = true;
        }
        mSpoilerLoaded = true;
        mSeedGenerated = false;
    } catch (...) {
        LUSLOG_ERROR("Failed to load Spoiler File: %s", spoilerFileName);
    }
}

void Context::ParseHashIconIndexesJson(nlohmann::json spoilerFileJson) {
    nlohmann::json hashJson = spoilerFileJson["file_hash"];
    int index = 0;
    for (auto it = hashJson.begin(); it != hashJson.end(); ++it) {
        hashIconIndexes[index] = gSeedTextures[it.value()].id;
        index++;
    }
}

void Context::ParseItemLocationsJson(nlohmann::json spoilerFileJson) {
    nlohmann::json locationsJson = spoilerFileJson["locations"];
    for (auto it = locationsJson.begin(); it != locationsJson.end(); ++it) {
        RandomizerCheck rc = StaticData::locationNameToEnum[it.key()];
        if (it->is_structured()) {
            nlohmann::json itemJson = *it;
            for (auto itemit = itemJson.begin(); itemit != itemJson.end(); ++itemit) {
                if (itemit.key() == "item") {
                    itemLocationTable[rc].SetPlacedItem(StaticData::itemNameToEnum[itemit.value().get<std::string>()]);
                } else if (itemit.key() == "price") {
                    itemLocationTable[rc].SetCustomPrice(itemit.value().get<uint16_t>());
                } else if (itemit.key() == "model") {
                    overrides[rc] = ItemOverride(rc, StaticData::itemNameToEnum[itemit.value().get<std::string>()]);
                } else if (itemit.key() == "trickName") {
                    overrides[rc].SetTrickName(Text(itemit.value().get<std::string>()));
                }
            }
        } else {
            itemLocationTable[rc].SetPlacedItem(StaticData::itemNameToEnum[it.value().get<std::string>()]);
        }
    }
}

void Context::WriteHintJson(nlohmann::ordered_json& spoilerFileJson){
    for (Hint hint: hintTable){
        hint.logHint(spoilerFileJson);
    }
}

nlohmann::json getValueForMessage(std::unordered_map<std::string, nlohmann::json> map, CustomMessage message){
    std::vector<std::string> strings = message.GetAllMessages();
    for (uint8_t language = 0; language < LANGUAGE_MAX; language++){
        if (map.contains(strings[language])){
            return strings[language];
        }
    }
    return {};
}

void Context::ParseHintJson(nlohmann::json spoilerFileJson) {
    for (auto hintData : spoilerFileJson["Gossip Stone Hints"].items()){
        RandomizerHint hint = (RandomizerHint)StaticData::hintNameToEnum[hintData.key()];
        AddHint(hint, Hint(hint, hintData.value()));
    }
    for (auto hintData : spoilerFileJson["Static Hints"].items()){
        RandomizerHint hint = (RandomizerHint)StaticData::hintNameToEnum[hintData.key()];
        AddHint(hint, Hint(hint, hintData.value()));
    }
    CreateStaticHints();
}

std::map<RandomizerGet, uint32_t> Context::RandoGetToEquipFlag = {
    { RG_KOKIRI_SWORD,           EQUIP_FLAG_SWORD_KOKIRI },
    { RG_MASTER_SWORD,           EQUIP_FLAG_SWORD_MASTER },
    { RG_BIGGORON_SWORD,         EQUIP_FLAG_SWORD_BGS },
    { RG_DEKU_SHIELD,            EQUIP_FLAG_SHIELD_DEKU },
    { RG_HYLIAN_SHIELD,          EQUIP_FLAG_SHIELD_HYLIAN },
    { RG_MIRROR_SHIELD,          EQUIP_FLAG_SHIELD_MIRROR },
    { RG_GORON_TUNIC,            EQUIP_FLAG_TUNIC_GORON },
    { RG_ZORA_TUNIC,             EQUIP_FLAG_TUNIC_ZORA },
    { RG_BUY_DEKU_SHIELD,        EQUIP_FLAG_SHIELD_DEKU },
    { RG_BUY_HYLIAN_SHIELD,      EQUIP_FLAG_SHIELD_HYLIAN },
    { RG_BUY_GORON_TUNIC,        EQUIP_FLAG_TUNIC_GORON },
    { RG_BUY_ZORA_TUNIC,         EQUIP_FLAG_TUNIC_ZORA },
    { RG_IRON_BOOTS,             EQUIP_FLAG_BOOTS_IRON },
    { RG_HOVER_BOOTS,            EQUIP_FLAG_BOOTS_HOVER }
};

std::map<RandomizerGet, uint32_t> Context::RandoGetToRandInf = {
    { RG_ZELDAS_LETTER,          RAND_INF_ZELDAS_LETTER },
    { RG_WEIRD_EGG,              RAND_INF_WEIRD_EGG },
    { RG_GOHMA_SOUL,             RAND_INF_GOHMA_SOUL },
    { RG_KING_DODONGO_SOUL,      RAND_INF_KING_DODONGO_SOUL },
    { RG_BARINADE_SOUL,          RAND_INF_BARINADE_SOUL },
    { RG_PHANTOM_GANON_SOUL,     RAND_INF_PHANTOM_GANON_SOUL },
    { RG_VOLVAGIA_SOUL,          RAND_INF_VOLVAGIA_SOUL },
    { RG_MORPHA_SOUL,            RAND_INF_MORPHA_SOUL },
    { RG_BONGO_BONGO_SOUL,       RAND_INF_BONGO_BONGO_SOUL },
    { RG_TWINROVA_SOUL,          RAND_INF_TWINROVA_SOUL },
    { RG_GANON_SOUL,             RAND_INF_GANON_SOUL },
    { RG_OCARINA_A_BUTTON,       RAND_INF_HAS_OCARINA_A },
    { RG_OCARINA_C_UP_BUTTON,    RAND_INF_HAS_OCARINA_C_UP },
    { RG_OCARINA_C_DOWN_BUTTON,  RAND_INF_HAS_OCARINA_C_DOWN },
    { RG_OCARINA_C_LEFT_BUTTON,  RAND_INF_HAS_OCARINA_C_LEFT },
    { RG_OCARINA_C_RIGHT_BUTTON, RAND_INF_HAS_OCARINA_C_RIGHT },
    { RG_SKELETON_KEY,           RAND_INF_HAS_SKELETON_KEY },
    { RG_GREG_RUPEE,             RAND_INF_GREG_FOUND },
    { RG_FISHING_POLE,           RAND_INF_FISHING_POLE_FOUND }
};

std::map<uint32_t, uint32_t> Context::RandoGetToDungeonScene = {
    { RG_FOREST_TEMPLE_SMALL_KEY,           SCENE_FOREST_TEMPLE },
    { RG_FIRE_TEMPLE_SMALL_KEY,             SCENE_FIRE_TEMPLE },
    { RG_WATER_TEMPLE_SMALL_KEY,            SCENE_WATER_TEMPLE },
    { RG_SPIRIT_TEMPLE_SMALL_KEY,           SCENE_SPIRIT_TEMPLE },
    { RG_SHADOW_TEMPLE_SMALL_KEY,           SCENE_SHADOW_TEMPLE },
    { RG_BOTTOM_OF_THE_WELL_SMALL_KEY,      SCENE_BOTTOM_OF_THE_WELL },
    { RG_GERUDO_TRAINING_GROUNDS_SMALL_KEY, SCENE_GERUDO_TRAINING_GROUND },
    { RG_GERUDO_FORTRESS_SMALL_KEY,         SCENE_THIEVES_HIDEOUT },
    { RG_GANONS_CASTLE_SMALL_KEY,           SCENE_INSIDE_GANONS_CASTLE },
    { RG_FOREST_TEMPLE_KEY_RING,            SCENE_FOREST_TEMPLE },
    { RG_FIRE_TEMPLE_KEY_RING,              SCENE_FIRE_TEMPLE },
    { RG_WATER_TEMPLE_KEY_RING,             SCENE_WATER_TEMPLE },
    { RG_SPIRIT_TEMPLE_KEY_RING,            SCENE_SPIRIT_TEMPLE },
    { RG_SHADOW_TEMPLE_KEY_RING,            SCENE_SHADOW_TEMPLE },
    { RG_BOTTOM_OF_THE_WELL_KEY_RING,       SCENE_BOTTOM_OF_THE_WELL },
    { RG_GERUDO_TRAINING_GROUNDS_KEY_RING,  SCENE_GERUDO_TRAINING_GROUND },
    { RG_GERUDO_FORTRESS_KEY_RING,          SCENE_THIEVES_HIDEOUT },
    { RG_GANONS_CASTLE_KEY_RING,            SCENE_INSIDE_GANONS_CASTLE },
    { RG_FOREST_TEMPLE_BOSS_KEY,            SCENE_FOREST_TEMPLE },
    { RG_FIRE_TEMPLE_BOSS_KEY,              SCENE_FIRE_TEMPLE },
    { RG_WATER_TEMPLE_BOSS_KEY,             SCENE_WATER_TEMPLE },
    { RG_SPIRIT_TEMPLE_BOSS_KEY,            SCENE_SPIRIT_TEMPLE },
    { RG_SHADOW_TEMPLE_BOSS_KEY,            SCENE_SHADOW_TEMPLE },
    { RG_GANONS_CASTLE_BOSS_KEY,            SCENE_INSIDE_GANONS_CASTLE },
    { RG_DEKU_TREE_MAP,                     SCENE_DEKU_TREE },
    { RG_DODONGOS_CAVERN_MAP,               SCENE_DODONGOS_CAVERN },
    { RG_JABU_JABUS_BELLY_MAP,              SCENE_JABU_JABU },
    { RG_FOREST_TEMPLE_MAP,                 SCENE_FOREST_TEMPLE },
    { RG_FIRE_TEMPLE_MAP,                   SCENE_FIRE_TEMPLE },
    { RG_WATER_TEMPLE_MAP,                  SCENE_WATER_TEMPLE },
    { RG_SPIRIT_TEMPLE_MAP,                 SCENE_SPIRIT_TEMPLE },
    { RG_SHADOW_TEMPLE_MAP,                 SCENE_SHADOW_TEMPLE },
    { RG_BOTTOM_OF_THE_WELL_MAP,            SCENE_BOTTOM_OF_THE_WELL },
    { RG_ICE_CAVERN_MAP,                    SCENE_ICE_CAVERN },
    { RG_DEKU_TREE_COMPASS,                 SCENE_DEKU_TREE },
    { RG_DODONGOS_CAVERN_COMPASS,           SCENE_DODONGOS_CAVERN },
    { RG_JABU_JABUS_BELLY_COMPASS,          SCENE_JABU_JABU },
    { RG_FOREST_TEMPLE_COMPASS,             SCENE_FOREST_TEMPLE },
    { RG_FIRE_TEMPLE_COMPASS,               SCENE_FIRE_TEMPLE },
    { RG_WATER_TEMPLE_COMPASS,              SCENE_WATER_TEMPLE },
    { RG_SPIRIT_TEMPLE_COMPASS,             SCENE_SPIRIT_TEMPLE },
    { RG_SHADOW_TEMPLE_COMPASS,             SCENE_SHADOW_TEMPLE },
    { RG_BOTTOM_OF_THE_WELL_COMPASS,        SCENE_BOTTOM_OF_THE_WELL },
    { RG_ICE_CAVERN_COMPASS,                SCENE_ICE_CAVERN },
    { RG_TREASURE_GAME_SMALL_KEY,           SCENE_TREASURE_BOX_SHOP }
};

std::map<uint32_t, uint32_t> Context::RandoGetToQuestItem = {
    { RG_FOREST_MEDALLION,       QUEST_MEDALLION_FOREST },
    { RG_FIRE_MEDALLION,         QUEST_MEDALLION_FIRE },
    { RG_WATER_MEDALLION,        QUEST_MEDALLION_WATER },
    { RG_SPIRIT_MEDALLION,       QUEST_MEDALLION_SPIRIT },
    { RG_SHADOW_MEDALLION,       QUEST_MEDALLION_SHADOW },
    { RG_LIGHT_MEDALLION,        QUEST_MEDALLION_LIGHT },
    { RG_MINUET_OF_FOREST,       QUEST_SONG_MINUET },
    { RG_BOLERO_OF_FIRE,         QUEST_SONG_BOLERO },
    { RG_SERENADE_OF_WATER,      QUEST_SONG_SERENADE },
    { RG_REQUIEM_OF_SPIRIT,      QUEST_SONG_REQUIEM },
    { RG_NOCTURNE_OF_SHADOW,     QUEST_SONG_NOCTURNE },
    { RG_PRELUDE_OF_LIGHT,       QUEST_SONG_PRELUDE },
    { RG_ZELDAS_LULLABY,         QUEST_SONG_LULLABY },
    { RG_EPONAS_SONG,            QUEST_SONG_EPONA },
    { RG_SARIAS_SONG,            QUEST_SONG_SARIA },
    { RG_SUNS_SONG,              QUEST_SONG_SUN },
    { RG_SONG_OF_TIME,           QUEST_SONG_TIME },
    { RG_SONG_OF_STORMS,         QUEST_SONG_STORMS },
    { RG_KOKIRI_EMERALD,         QUEST_KOKIRI_EMERALD },
    { RG_GORON_RUBY,             QUEST_GORON_RUBY },
    { RG_ZORA_SAPPHIRE,          QUEST_ZORA_SAPPHIRE },
    { RG_STONE_OF_AGONY,         QUEST_STONE_OF_AGONY },
    { RG_GERUDO_MEMBERSHIP_CARD, QUEST_GERUDO_CARD },
};

uint32_t HookshotLookup[3] = { ITEM_NONE, ITEM_HOOKSHOT, ITEM_LONGSHOT };
uint32_t OcarinaLookup[3] = { ITEM_NONE, ITEM_OCARINA_FAIRY, ITEM_OCARINA_TIME };

void Context::ApplyItemEffect(Item& item, bool state) {
    auto randoGet = item.GetRandomizerGet();
    if (item.GetGIEntry()->objectId == OBJECT_GI_STICK) {
        SetInventory(ITEM_STICK, (!state ? ITEM_NONE : ITEM_STICK));
    }
    if (item.GetGIEntry()->objectId == OBJECT_GI_NUTS) {
        SetInventory(ITEM_NUT, (!state ? ITEM_NONE : ITEM_NUT));
    }
    switch (item.GetItemType()) {
    case ITEMTYPE_ITEM:
    {
        switch (randoGet) {
            case RG_STONE_OF_AGONY:
            case RG_GERUDO_MEMBERSHIP_CARD:
                SetQuestItem(RandoGetToQuestItem.at(randoGet), state);
                break;
            case RG_WEIRD_EGG:
                SetRandoInf(RAND_INF_WEIRD_EGG, state);
                break;
            case RG_ZELDAS_LETTER:
                SetRandoInf(RAND_INF_ZELDAS_LETTER, state);
                break;
            case RG_DOUBLE_DEFENSE:
                mSaveContext->isDoubleDefenseAcquired = state;
                break;
            case RG_POCKET_EGG:
            case RG_COJIRO:
            case RG_ODD_MUSHROOM:
            case RG_ODD_POTION:
            case RG_POACHERS_SAW:
            case RG_BROKEN_SWORD:
            case RG_PRESCRIPTION:
            case RG_EYEBALL_FROG:
            case RG_EYEDROPS:
            case RG_CLAIM_CHECK:
                SetAdultTrade(item.GetGIEntry()->itemId, state);
                break;
            case RG_PROGRESSIVE_HOOKSHOT:
            {
                uint8_t i;
                for (i = 0; i < 3; i++) {
                    if (CurrentInventory(ITEM_HOOKSHOT) == HookshotLookup[i]) {
                        break;
                    }
                }
                auto newItem = i + (!state ? -1 : 1);
                if (newItem < 0) {
                    newItem = 0;
                }
                else if (newItem > 2) {
                    newItem = 2;
                }
                SetInventory(ITEM_HOOKSHOT, HookshotLookup[newItem]);
            }   break;
            case RG_PROGRESSIVE_STRENGTH:
            {
                auto currentLevel = CurrentUpgrade(UPG_STRENGTH);
                auto newLevel = currentLevel + (!state ? -1 : 1);
                SetUpgrade(UPG_STRENGTH, newLevel);
            }   break;
            case RG_PROGRESSIVE_BOMB_BAG:
            {
                auto realGI = item.GetGIEntry();
                if (realGI->itemId == RG_BOMB_BAG_INF && realGI->modIndex == MOD_RANDOMIZER) {
                    SetRandoInf(RAND_INF_HAS_INFINITE_BOMB_BAG, true);
                    break;
                }
                auto currentLevel = CurrentUpgrade(UPG_BOMB_BAG);
                auto newLevel = currentLevel + (!state ? -1 : 1);
                if (currentLevel == 0 && state || currentLevel == 1 && !state) {
                    SetInventory(ITEM_BOMB, (!state ? ITEM_NONE : ITEM_BOMB));
                }
                SetUpgrade(UPG_BOMB_BAG, newLevel);
            }   break;
            case RG_PROGRESSIVE_BOW:
            {
                auto realGI = item.GetGIEntry();
                if (realGI->itemId == RG_QUIVER_INF && realGI->modIndex == MOD_RANDOMIZER) {
                    SetRandoInf(RAND_INF_HAS_INFINITE_QUIVER, true);
                    break;
                }
                auto currentLevel = CurrentUpgrade(UPG_QUIVER);
                auto newLevel = currentLevel + (!state ? -1 : 1);
                if (currentLevel == 0 && state || currentLevel == 1 && !state) {
                    SetInventory(ITEM_BOW, (!state ? ITEM_NONE : ITEM_BOW));
                }
                SetUpgrade(UPG_QUIVER, newLevel);
            }   break;
            case RG_PROGRESSIVE_SLINGSHOT:
            {
                auto realGI = item.GetGIEntry();
                if (realGI->itemId == RG_BULLET_BAG_INF && realGI->modIndex == MOD_RANDOMIZER) {
                    SetRandoInf(RAND_INF_HAS_INFINITE_BULLET_BAG, true);
                    break;
                }
                auto currentLevel = CurrentUpgrade(UPG_BULLET_BAG);
                auto newLevel = currentLevel + (!state ? -1 : 1);
                if (currentLevel == 0 && state || currentLevel == 1 && !state) {
                    SetInventory(ITEM_SLINGSHOT, (!state ? ITEM_NONE : ITEM_SLINGSHOT));
                }
                SetUpgrade(UPG_BULLET_BAG, newLevel);
            }   break;
            case RG_PROGRESSIVE_WALLET:
            {
                auto realGI = item.GetGIEntry();
                if (realGI->itemId == RG_WALLET_INF && realGI->modIndex == MOD_RANDOMIZER) {
                    SetRandoInf(RAND_INF_HAS_INFINITE_MONEY, true);
                    break;
                }
                auto currentLevel = CurrentUpgrade(UPG_WALLET);
                if (!CheckRandoInf(RAND_INF_HAS_WALLET) && state) {
                    SetRandoInf(RAND_INF_HAS_WALLET, true);
                }
                else if (currentLevel == 0 && !state) {
                    SetRandoInf(RAND_INF_HAS_WALLET, false);
                }
                else {
                    auto newLevel = currentLevel + (!state ? -1 : 1);
                    SetUpgrade(UPG_WALLET, newLevel);
                }
            }   break;
            case RG_PROGRESSIVE_SCALE:
            {
                auto currentLevel = CurrentUpgrade(UPG_SCALE);
                if (!CheckRandoInf(RAND_INF_CAN_SWIM) && state) {
                    SetRandoInf(RAND_INF_CAN_SWIM, true);
                }
                else if (currentLevel == 0 && !state) {
                    SetRandoInf(RAND_INF_CAN_SWIM, false);
                }
                else {
                    auto newLevel = currentLevel + (!state ? -1 : 1);
                    SetUpgrade(UPG_SCALE, newLevel);
                }
            }   break;
            case RG_PROGRESSIVE_NUT_UPGRADE:
            {
                auto realGI = item.GetGIEntry();
                if (realGI->itemId == RG_NUT_UPGRADE_INF && realGI->modIndex == MOD_RANDOMIZER) {
                    SetRandoInf(RAND_INF_HAS_INFINITE_NUT_UPGRADE, true);
                    break;
                }
                auto currentLevel = CurrentUpgrade(UPG_NUTS);
                auto newLevel = currentLevel + (!state ? -1 : 1);
                if (currentLevel == 0 && state || currentLevel == 1 && !state) {
                    SetInventory(ITEM_NUT, (!state ? ITEM_NONE : ITEM_NUT));
                }
                SetUpgrade(UPG_NUTS, newLevel);
            }   break;
            case RG_PROGRESSIVE_STICK_UPGRADE:
            {
                auto realGI = item.GetGIEntry();
                if (realGI->itemId == RG_STICK_UPGRADE_INF && realGI->modIndex == MOD_RANDOMIZER) {
                    SetRandoInf(RAND_INF_HAS_INFINITE_STICK_UPGRADE, true);
                    break;
                }
                auto currentLevel = CurrentUpgrade(UPG_STICKS);
                auto newLevel = currentLevel + (!state ? -1 : 1);
                if (currentLevel == 0 && state || currentLevel == 1 && !state) {
                    SetInventory(ITEM_STICK, (!state ? ITEM_NONE : ITEM_STICK));
                }
                SetUpgrade(UPG_STICKS, newLevel);
            }   break;
            case RG_PROGRESSIVE_BOMBCHUS:
            {
                auto realGI = item.GetGIEntry();
                if (realGI->itemId == RG_BOMBCHU_INF && realGI->modIndex == MOD_RANDOMIZER) {
                    SetRandoInf(RAND_INF_HAS_INFINITE_BOMBCHUS, true);
                    break;
                }
                SetInventory(ITEM_BOMBCHU, (!state ? ITEM_NONE : ITEM_BOMBCHU));
            } break;
            case RG_PROGRESSIVE_MAGIC_METER:
            {
                auto realGI = item.GetGIEntry();
                if (realGI->itemId == RG_MAGIC_INF && realGI->modIndex == MOD_RANDOMIZER) {
                    SetRandoInf(RAND_INF_HAS_INFINITE_MAGIC_METER, true);
                    break;
                }
                mSaveContext->magicLevel += (!state ? -1 : 1);
            } break;
            case RG_PROGRESSIVE_OCARINA:
            {
                uint8_t i;
                for (i = 0; i < 3; i++) {
                    if (CurrentInventory(ITEM_OCARINA_FAIRY) == OcarinaLookup[i]) {
                        break;
                    }
                }
                i += (!state ? -1 : 1);
                if (i < 0) {
                    i = 0;
                }
                else if (i > 2) {
                    i = 2;
                }
                SetInventory(ITEM_OCARINA_FAIRY, OcarinaLookup[i]);
            }   break;
            case RG_HEART_CONTAINER:
                mSaveContext->healthCapacity += (!state ? -16 : 16);
                break;
            case RG_PIECE_OF_HEART:
                mSaveContext->healthCapacity += (!state ? -4 : 4);
                break;
            case RG_BOOMERANG:
            case RG_LENS_OF_TRUTH:
            case RG_MEGATON_HAMMER:
            case RG_DINS_FIRE:
            case RG_FARORES_WIND:
            case RG_NAYRUS_LOVE:
            case RG_FIRE_ARROWS:
            case RG_ICE_ARROWS:
            case RG_LIGHT_ARROWS:
                SetInventory(item.GetGIEntry()->itemId, (!state ? ITEM_NONE : item.GetGIEntry()->itemId));
                break;
            case RG_MAGIC_BEAN:
            case RG_MAGIC_BEAN_PACK:
            {
                auto change = (item.GetRandomizerGet() == RG_MAGIC_BEAN ? 1 : 10);
                auto current = GetAmmo(ITEM_BEAN);
                SetAmmo(ITEM_BEAN, current + (!state ? -change : change));
            }   break;
            case RG_EMPTY_BOTTLE:
            case RG_BOTTLE_WITH_MILK:
            case RG_BOTTLE_WITH_RED_POTION:
            case RG_BOTTLE_WITH_GREEN_POTION:
            case RG_BOTTLE_WITH_BLUE_POTION:
            case RG_BOTTLE_WITH_FAIRY:
            case RG_BOTTLE_WITH_FISH:
            case RG_BOTTLE_WITH_BLUE_FIRE:
            case RG_BOTTLE_WITH_BUGS:
            case RG_BOTTLE_WITH_POE:
            case RG_BOTTLE_WITH_BIG_POE:
            {
                uint8_t slot = SLOT_BOTTLE_1;
                while (slot != SLOT_BOTTLE_4) {
                    if (mSaveContext->inventory.items[slot] == ITEM_NONE) {
                        break;
                    }
                    slot++;
                }
                mSaveContext->inventory.items[slot] = item.GetGIEntry()->itemId;
            }   break;
            case RG_RUTOS_LETTER:
                SetEventChkInf(EVENTCHKINF_OBTAINED_RUTOS_LETTER, state);
                break;
            case RG_GOHMA_SOUL:
            case RG_KING_DODONGO_SOUL:
            case RG_BARINADE_SOUL:
            case RG_PHANTOM_GANON_SOUL:
            case RG_VOLVAGIA_SOUL:
            case RG_MORPHA_SOUL:
            case RG_BONGO_BONGO_SOUL:
            case RG_TWINROVA_SOUL:
            case RG_GANON_SOUL:
            case RG_OCARINA_A_BUTTON:
            case RG_OCARINA_C_UP_BUTTON:
            case RG_OCARINA_C_DOWN_BUTTON:
            case RG_OCARINA_C_LEFT_BUTTON:
            case RG_OCARINA_C_RIGHT_BUTTON:
            case RG_GREG_RUPEE:
            case RG_FISHING_POLE:
                SetRandoInf(RandoGetToRandInf.at(randoGet), state);
                break;
            case RG_TRIFORCE_PIECE:
                mSaveContext->triforcePiecesCollected += (!state ? -1 : 1);
                break;
            case RG_BOMBCHU_5:
            case RG_BOMBCHU_10:
            case RG_BOMBCHU_20:
                SetInventory(ITEM_BOMBCHU, (!state ? ITEM_NONE : ITEM_BOMBCHU));
                break;
        }
    }
    break;
    case ITEMTYPE_EQUIP:
    {
        RandomizerGet itemRG = item.GetRandomizerGet();
        if (itemRG == RG_GIANTS_KNIFE) {
            return;
        }
        uint32_t equipId = RandoGetToEquipFlag.find(itemRG)->second;
        if (!state) {
            mSaveContext->inventory.equipment &= ~equipId;
            if (equipId == EQUIP_FLAG_SWORD_BGS) {
                mSaveContext->bgsFlag = false;
            }
        }
        else {
            mSaveContext->inventory.equipment |= equipId;
            if (equipId == EQUIP_FLAG_SWORD_BGS) {
                mSaveContext->bgsFlag = true;
            }
        }
    }
    break;
    case ITEMTYPE_DUNGEONREWARD:
    case ITEMTYPE_SONG:
        SetQuestItem(RandoGetToQuestItem.find(item.GetRandomizerGet())->second, state);
        break;
    case ITEMTYPE_MAP:
        SetDungeonItem(DUNGEON_MAP, RandoGetToDungeonScene.find(item.GetRandomizerGet())->second, state);
        break;
    case ITEMTYPE_COMPASS:
        SetDungeonItem(DUNGEON_COMPASS, RandoGetToDungeonScene.find(item.GetRandomizerGet())->second, state);
        break;
    case ITEMTYPE_BOSSKEY:
        SetDungeonItem(DUNGEON_KEY_BOSS, RandoGetToDungeonScene.find(item.GetRandomizerGet())->second, state);
        break;
    case ITEMTYPE_FORTRESS_SMALLKEY:
    case ITEMTYPE_SMALLKEY:
    {
        auto randoGet = item.GetRandomizerGet();
        auto keyring = randoGet >= RG_FOREST_TEMPLE_KEY_RING && randoGet <= RG_GANONS_CASTLE_KEY_RING;
        auto dungeonIndex = RandoGetToDungeonScene.find(randoGet)->second;
        auto count = GetSmallKeyCount(dungeonIndex);
        if (!state) {
            if (keyring) {
                count = 0;
            }
            else {
                count -= 1;
            }
        }
        else {
            if (keyring) {
                count = 10;
            }
            else {
                count += 1;
            }
        }
        SetSmallKeyCount(dungeonIndex, count);
    } break;
    case ITEMTYPE_TOKEN:
        mSaveContext->inventory.gsTokens += (!state ? -1 : 1);
        break;
    case ITEMTYPE_EVENT:
        break;
    case ITEMTYPE_DROP:
    case ITEMTYPE_REFILL:
    case ITEMTYPE_SHOP:
    {
        RandomizerGet itemRG = item.GetRandomizerGet();
        if (itemRG == RG_BUY_HYLIAN_SHIELD || itemRG == RG_BUY_DEKU_SHIELD || itemRG == RG_BUY_GORON_TUNIC || itemRG == RG_BUY_ZORA_TUNIC) {
            uint32_t equipId = RandoGetToEquipFlag.find(itemRG)->second;
            if (!state) {
                mSaveContext->inventory.equipment &= ~equipId;
            }
            else {
                mSaveContext->inventory.equipment |= equipId;
            }
        }
        switch (itemRG) {
        case RG_DEKU_NUTS_5:
        case RG_DEKU_NUTS_10:
        case RG_BUY_DEKU_NUTS_5:
        case RG_BUY_DEKU_NUTS_10:
            SetInventory(ITEM_NUT, (!state ? ITEM_NONE : ITEM_NUT));
            break;
        case RG_DEKU_STICK_1:
        case RG_BUY_DEKU_STICK_1:
        case RG_STICKS:
            SetInventory(ITEM_STICK, (!state ? ITEM_NONE : ITEM_STICK));
            break;
        case RG_BOMBCHU_5:
        case RG_BOMBCHU_10:
        case RG_BOMBCHU_20:
            SetInventory(ITEM_BOMBCHU, (!state ? ITEM_NONE : ITEM_BOMBCHU));
            break;
        }
    } break;
    }
    mLogic->UpdateHelpers();
}

std::shared_ptr<Settings> Context::GetSettings() {
    return mSettings;
}

std::shared_ptr<EntranceShuffler> Context::GetEntranceShuffler() {
    return mEntranceShuffler;
}

std::shared_ptr<Dungeons> Context::GetDungeons() {
    return mDungeons;
}

std::shared_ptr<Fishsanity> Context::GetFishsanity() {
    return mFishsanity;
}

DungeonInfo* Context::GetDungeon(size_t key) const {
    return mDungeons->GetDungeon(static_cast<DungeonKey>(key));
}

std::shared_ptr<Logic> Context::GetLogic() {
    if (mLogic.get() == nullptr) {
        mLogic = std::make_shared<Logic>();
    }
    return mLogic;
}

SaveContext* Context::GetSaveContext() {
    if (mSaveContext == nullptr) {
        NewSaveContext();
    }
    return mSaveContext;
}

void Context::SetSaveContext(SaveContext* context) {
    mSaveContext = context;
}

void Context::InitSaveContext() {
    mSaveContext->totalDays = 0;
    mSaveContext->bgsDayCount = 0;

    mSaveContext->deaths = 0;
    for (int i = 0; i < ARRAY_COUNT(mSaveContext->playerName); i++) {
        mSaveContext->playerName[i] = 0x3E;
    }
    mSaveContext->n64ddFlag = 0;
    mSaveContext->healthCapacity = 0x30;
    mSaveContext->health = 0x30;
    mSaveContext->magicLevel = 0;
    mSaveContext->magic = 0x30;
    mSaveContext->rupees = 0;
    mSaveContext->swordHealth = 0;
    mSaveContext->naviTimer = 0;
    mSaveContext->isMagicAcquired = 0;
    mSaveContext->isDoubleMagicAcquired = 0;
    mSaveContext->isDoubleDefenseAcquired = 0;
    mSaveContext->bgsFlag = 0;
    mSaveContext->ocarinaGameRoundNum = 0;
    for (int button = 0; button < ARRAY_COUNT(mSaveContext->childEquips.buttonItems); button++) {
        mSaveContext->childEquips.buttonItems[button] = ITEM_NONE;
    }
    for (int button = 0; button < ARRAY_COUNT(mSaveContext->childEquips.cButtonSlots); button++) {
        mSaveContext->childEquips.cButtonSlots[button] = SLOT_NONE;
    }
    mSaveContext->childEquips.equipment = 0;
    for (int button = 0; button < ARRAY_COUNT(mSaveContext->adultEquips.buttonItems); button++) {
        mSaveContext->adultEquips.buttonItems[button] = ITEM_NONE;
    }
    for (int button = 0; button < ARRAY_COUNT(mSaveContext->adultEquips.cButtonSlots); button++) {
        mSaveContext->adultEquips.cButtonSlots[button] = SLOT_NONE;
    }
    mSaveContext->adultEquips.equipment = 0;
    mSaveContext->unk_54 = 0;
    mSaveContext->savedSceneNum = SCENE_LINKS_HOUSE;

    // Equipment
    for (int button = 0; button < ARRAY_COUNT(mSaveContext->equips.buttonItems); button++) {
        mSaveContext->equips.buttonItems[button] = ITEM_NONE;
    }
    for (int button = 0; button < ARRAY_COUNT(mSaveContext->equips.cButtonSlots); button++) {
        mSaveContext->equips.cButtonSlots[button] = SLOT_NONE;
    }
    mSaveContext->equips.equipment = 0;

    // Inventory
    for (int item = 0; item < ARRAY_COUNT(mSaveContext->inventory.items); item++) {
        mSaveContext->inventory.items[item] = ITEM_NONE;
    }
    for (int ammo = 0; ammo < ARRAY_COUNT(mSaveContext->inventory.ammo); ammo++) {
        mSaveContext->inventory.ammo[ammo] = 0;
    }
    mSaveContext->inventory.equipment = 0;
    mSaveContext->inventory.upgrades = 0;
    mSaveContext->inventory.questItems = 0;
    for (int dungeon = 0; dungeon < ARRAY_COUNT(mSaveContext->inventory.dungeonItems); dungeon++) {
        mSaveContext->inventory.dungeonItems[dungeon] = 0;
    }
    for (int dungeon = 0; dungeon < ARRAY_COUNT(mSaveContext->inventory.dungeonKeys); dungeon++) {
        mSaveContext->inventory.dungeonKeys[dungeon] = 0x0;
    }
    mSaveContext->inventory.defenseHearts = 0;
    mSaveContext->inventory.gsTokens = 0;
    for (int scene = 0; scene < ARRAY_COUNT(mSaveContext->sceneFlags); scene++) {
        mSaveContext->sceneFlags[scene].chest = 0;
        mSaveContext->sceneFlags[scene].swch = 0;
        mSaveContext->sceneFlags[scene].clear = 0;
        mSaveContext->sceneFlags[scene].collect = 0;
        mSaveContext->sceneFlags[scene].unk = 0;
        mSaveContext->sceneFlags[scene].rooms = 0;
        mSaveContext->sceneFlags[scene].floors = 0;
    }
    mSaveContext->fw.pos.x = 0;
    mSaveContext->fw.pos.y = 0;
    mSaveContext->fw.pos.z = 0;
    mSaveContext->fw.yaw = 0;
    mSaveContext->fw.playerParams = 0;
    mSaveContext->fw.entranceIndex = 0;
    mSaveContext->fw.roomIndex = 0;
    mSaveContext->fw.set = 0;
    mSaveContext->fw.tempSwchFlags = 0;
    mSaveContext->fw.tempCollectFlags = 0;
    for (int flag = 0; flag < ARRAY_COUNT(mSaveContext->gsFlags); flag++) {
        mSaveContext->gsFlags[flag] = 0;
    }
    for (int highscore = 0; highscore < ARRAY_COUNT(mSaveContext->highScores); highscore++) {
        mSaveContext->highScores[highscore] = 0;
    }
    for (int flag = 0; flag < ARRAY_COUNT(mSaveContext->eventChkInf); flag++) {
        mSaveContext->eventChkInf[flag] = 0;
    }
    for (int flag = 0; flag < ARRAY_COUNT(mSaveContext->itemGetInf); flag++) {
        mSaveContext->itemGetInf[flag] = 0;
    }
    for (int flag = 0; flag < ARRAY_COUNT(mSaveContext->infTable); flag++) {
        mSaveContext->infTable[flag] = 0;
    }
    mSaveContext->worldMapAreaData = 0;
    mSaveContext->scarecrowLongSongSet = 0;
    for (int i = 0; i < ARRAY_COUNT(mSaveContext->scarecrowLongSong); i++) {
        mSaveContext->scarecrowLongSong[i].noteIdx = 0;
        mSaveContext->scarecrowLongSong[i].unk_01 = 0;
        mSaveContext->scarecrowLongSong[i].unk_02 = 0;
        mSaveContext->scarecrowLongSong[i].volume = 0;
        mSaveContext->scarecrowLongSong[i].vibrato = 0;
        mSaveContext->scarecrowLongSong[i].tone = 0;
        mSaveContext->scarecrowLongSong[i].semitone = 0;
    }
    mSaveContext->scarecrowSpawnSongSet = 0;
    for (int i = 0; i < ARRAY_COUNT(mSaveContext->scarecrowSpawnSong); i++) {
        mSaveContext->scarecrowSpawnSong[i].noteIdx = 0;
        mSaveContext->scarecrowSpawnSong[i].unk_01 = 0;
        mSaveContext->scarecrowSpawnSong[i].unk_02 = 0;
        mSaveContext->scarecrowSpawnSong[i].volume = 0;
        mSaveContext->scarecrowSpawnSong[i].vibrato = 0;
        mSaveContext->scarecrowSpawnSong[i].tone = 0;
        mSaveContext->scarecrowSpawnSong[i].semitone = 0;
    }

    mSaveContext->horseData.scene = SCENE_HYRULE_FIELD;
    mSaveContext->horseData.pos.x = -1840;
    mSaveContext->horseData.pos.y = 72;
    mSaveContext->horseData.pos.z = 5497;
    mSaveContext->horseData.angle = -0x6AD9;
    mSaveContext->magicLevel = 0;
    mSaveContext->infTable[29] = 1;
    mSaveContext->sceneFlags[5].swch = 0x40000000;

    // SoH specific
    mSaveContext->backupFW = mSaveContext->fw;
    mSaveContext->pendingSale = ITEM_NONE;
    mSaveContext->pendingSaleMod = MOD_NONE;
    mSaveContext->isBossRushPaused = 0;
    mSaveContext->pendingIceTrapCount = 0;

    // Init with normal quest unless only an MQ rom is provided
    mSaveContext->questId = OTRGlobals::Instance->HasOriginal() ? QUEST_NORMAL : QUEST_MASTER;

    //RANDOTODO (ADD ITEMLOCATIONS TO GSAVECONTEXT)
}

void Context::NewSaveContext() {
    if (mSaveContext != nullptr && mSaveContext != &gSaveContext) {
        free(mSaveContext);
    }
    mSaveContext = new SaveContext();
    InitSaveContext();
}

void Context::ResetLogic() {
    NewSaveContext();
    mLogic->Reset();
}

std::shared_ptr<Trials> Context::GetTrials() {
    return mTrials;
}

TrialInfo* Context::GetTrial(size_t key) const {
    return mTrials->GetTrial(static_cast<TrialKey>(key));
}

TrialInfo* Context::GetTrial(TrialKey key) const {
    return mTrials->GetTrial(key);
}

Sprite* Context::GetSeedTexture(const uint8_t index) {
    return &gSeedTextures[index];
}

Option& Context::GetOption(const RandomizerSettingKey key) const {
    return mSettings->GetOption(key);
}

TrickOption& Context::GetTrickOption(const RandomizerTrick key) const {
    return mSettings->GetTrickOption(key);
}

uint8_t Context::InventorySlot(uint32_t item) {
    return gItemSlots[item];
}

uint32_t Context::CurrentUpgrade(uint32_t upgrade) {
    return (mSaveContext->inventory.upgrades & gUpgradeMasks[upgrade]) >> gUpgradeShifts[upgrade];
}

uint32_t Context::CurrentInventory(uint32_t item) {
    return mSaveContext->inventory.items[InventorySlot(item)];
}

void Context::SetUpgrade(uint32_t upgrade, uint8_t level) {
    mSaveContext->inventory.upgrades &= gUpgradeNegMasks[upgrade];
    mSaveContext->inventory.upgrades |= level << gUpgradeShifts[upgrade];
}

bool Context::CheckInventory(uint32_t item, bool exact) {
    auto current = mSaveContext->inventory.items[InventorySlot(item)];
    return exact ? (current == item) : (current != ITEM_NONE);
}

void Context::SetInventory(uint32_t itemSlot, uint32_t item) {
    mSaveContext->inventory.items[InventorySlot(itemSlot)] = item;
}

bool Context::CheckEquipment(uint32_t equipFlag) {
    return (equipFlag & mSaveContext->inventory.equipment);
}

bool Context::CheckQuestItem(uint32_t item) {
    return ((1 << item) & mSaveContext->inventory.questItems);
}

bool Context::HasAdultTrade(uint32_t itemID) {
    int tradeIndex = itemID - ITEM_POCKET_EGG;
    return mSaveContext->adultTradeItems & (1 << tradeIndex);
}

void Context::SetAdultTrade(uint32_t itemID, bool state) {
    int tradeIndex = itemID - ITEM_POCKET_EGG;
    if (!state) {
        mSaveContext->adultTradeItems &= ~(1 << tradeIndex);
    }
    else {
        mSaveContext->adultTradeItems |= (1 << tradeIndex);
    }
}

void Context::SetQuestItem(uint32_t item, bool state) {
    if (!state) {
        mSaveContext->inventory.questItems &= ~(1 << item);
    }
    else {
        mSaveContext->inventory.questItems |= (1 << item);
    }
}

uint8_t Context::GetSmallKeyCount(uint32_t dungeonIndex) {
    return mSaveContext->inventory.dungeonKeys[dungeonIndex];
}

void Context::SetSmallKeyCount(uint32_t dungeonIndex, uint8_t count) {
    mSaveContext->inventory.dungeonKeys[dungeonIndex] = count;
}

bool Context::CheckDungeonItem(uint32_t item, uint32_t dungeonIndex) {
    return mSaveContext->inventory.dungeonItems[dungeonIndex] & gBitFlags[item];
}

void Context::SetDungeonItem(uint32_t item, uint32_t dungeonIndex, bool state) {
    if (!state) {
        mSaveContext->inventory.dungeonItems[dungeonIndex] &= ~gBitFlags[item];
    }
    else {
        mSaveContext->inventory.dungeonItems[dungeonIndex] |= gBitFlags[item];
    }
}

bool Context::CheckRandoInf(uint32_t flag) {
    return mSaveContext->randomizerInf[flag >> 4] & (1 << (flag & 0xF));
}

void Context::SetRandoInf(uint32_t flag, bool state) {
    if (!state) {
        mSaveContext->randomizerInf[flag >> 4] &= ~(1 << (flag & 0xF));
    }
    else {
        mSaveContext->randomizerInf[flag >> 4] |= (1 << (flag & 0xF));
    }
}

bool Context::CheckEventChkInf(int32_t flag) {
    return mSaveContext->eventChkInf[flag >> 4] & (1 << (flag & 0xF));
}

void Context::SetEventChkInf(int32_t flag, bool state) {
    if (!state) {
        mSaveContext->eventChkInf[flag >> 4] &= ~(1 << (flag & 0xF));
    }
    else {
        mSaveContext->eventChkInf[flag >> 4] |= (1 << (flag & 0xF));
    }
}

uint8_t Context::GetGSCount() {
    return mSaveContext->inventory.gsTokens;
}

uint8_t Context::GetAmmo(uint32_t item) {
    return mSaveContext->inventory.ammo[gItemSlots[item]];
}

void Context::SetAmmo(uint32_t item, uint8_t count) {
    mSaveContext->inventory.ammo[gItemSlots[item]] = count;
}
} // namespace Rando
