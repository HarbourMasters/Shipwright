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
#include "../kaleido.h"

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
    mFishsanity = std::make_shared<Fishsanity>();
    VanillaLogicDefaults = {
        // RANDOTODO check what this does
        &mOptions[RSK_LINKS_POCKET],
        &mOptions[RSK_SHUFFLE_DUNGEON_REWARDS],
        &mOptions[RSK_SHUFFLE_SONGS],
        &mOptions[RSK_SHOPSANITY],
        &mOptions[RSK_SHOPSANITY_COUNT],
        &mOptions[RSK_SHOPSANITY_PRICES],
        &mOptions[RSK_SHOPSANITY_PRICES_AFFORDABLE],
        &mOptions[RSK_FISHSANITY],
        &mOptions[RSK_FISHSANITY_POND_COUNT],
        &mOptions[RSK_FISHSANITY_AGE_SPLIT],
        &mOptions[RSK_SHUFFLE_SCRUBS],
        &mOptions[RSK_SHUFFLE_BEEHIVES],
        &mOptions[RSK_SHUFFLE_COWS],
        &mOptions[RSK_SHUFFLE_POTS],
        &mOptions[RSK_SHUFFLE_FREESTANDING],
        &mOptions[RSK_SHUFFLE_MERCHANTS],
        &mOptions[RSK_SHUFFLE_FROG_SONG_RUPEES],
        &mOptions[RSK_SHUFFLE_ADULT_TRADE],
        &mOptions[RSK_SHUFFLE_100_GS_REWARD],
        &mOptions[RSK_SHUFFLE_FAIRIES],
        &mOptions[RSK_GOSSIP_STONE_HINTS],
    };
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

bool Context::IsLocationShuffled(const RandomizerCheck locKey) {
    return itemLocationTable[locKey].GetPlacedRandomizerGet() != RG_NONE;
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
    SPDLOG_DEBUG(StaticData::RetrieveItem(item).GetName().GetEnglish() + " placed at " + StaticData::GetLocation(locKey)->GetName() + "\n");
    
    if (applyEffectImmediately || mOptions[RSK_LOGIC_RULES].Is(RO_LOGIC_GLITCHLESS) || mOptions[RSK_LOGIC_RULES].Is(RO_LOGIC_VANILLA)) {
        StaticData::RetrieveItem(item).ApplyEffect();
    }

    // TODO? Show Progress

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

bool Context::IsQuestOfLocationActive(RandomizerCheck rc) {
    const auto loc = Rando::StaticData::GetLocation(rc);
    return loc->GetQuest() == RCQUEST_BOTH ||
        loc->GetQuest() == RCQUEST_MQ && mDungeons->GetDungeonFromScene(loc->GetScene())->IsMQ() ||
        loc->GetQuest() == RCQUEST_VANILLA && mDungeons->GetDungeonFromScene(loc->GetScene())->IsVanilla();
}

void Context::GenerateLocationPool() {
    allLocations.clear();
    if (mOptions[RSK_TRIFORCE_HUNT]) {
        AddLocation(RC_TRIFORCE_COMPLETED);
    }
    for (Location& location : StaticData::GetLocationTable()) {
        // skip RCs that shouldn't be in the pool for any reason (i.e. settings, unsupported check type, etc.)
        // TODO: Exclude checks for some of the older shuffles from the pool too i.e. Frog Songs, Scrubs, etc.)
        if (location.GetRandomizerCheck() == RC_UNKNOWN_CHECK ||
            location.GetRandomizerCheck() == RC_TRIFORCE_COMPLETED || // already in pool
            location.GetRCType() == RCTYPE_CHEST_GAME ||              // not supported yet
            location.GetRCType() == RCTYPE_STATIC_HINT ||             // can't have items
            location.GetRCType() == RCTYPE_GOSSIP_STONE ||            // can't have items
            (location.GetRCType() == RCTYPE_SCRUB && mOptions[RSK_SHUFFLE_SCRUBS].Is(RO_SCRUBS_OFF)) ||
            (location.GetRCType() == RCTYPE_SCRUB && mOptions[RSK_SHUFFLE_SCRUBS].Is(RO_SCRUBS_ONE_TIME_ONLY) && !(
                location.GetRandomizerCheck() == RC_LW_DEKU_SCRUB_GROTTO_FRONT ||
                location.GetRandomizerCheck() == RC_LW_DEKU_SCRUB_NEAR_BRIDGE ||
                location.GetRandomizerCheck() == RC_HF_DEKU_SCRUB_GROTTO
            )) ||
            (location.GetRCType() == RCTYPE_COW && mOptions[RSK_SHUFFLE_COWS].Is(RO_GENERIC_OFF)) ||
            (location.GetRCType() == RCTYPE_FISH && mOptions[RSK_FISHSANITY].Is(RO_FISHSANITY_OFF)) ||
            (location.GetRCType() == RCTYPE_POT && mOptions[RSK_SHUFFLE_POTS].Is(RO_SHUFFLE_POTS_OFF)) ||
            (location.GetRCType() == RCTYPE_FAIRY && !mOptions[RSK_SHUFFLE_FAIRIES]) ||
            (location.GetRCType() == RCTYPE_FREESTANDING &&
             mOptions[RSK_SHUFFLE_FREESTANDING].Is(RO_SHUFFLE_FREESTANDING_OFF)) ||
            (location.GetRCType() == RCTYPE_BEEHIVE && !mOptions[RSK_SHUFFLE_BEEHIVES])) {
            continue;
        }
        if (location.IsOverworld()) {
            // Skip stuff that is shuffled to dungeon only, i.e. tokens, pots, etc., or other checks that
            // should not have a shuffled item.
            if ((location.GetRCType() == RCTYPE_FREESTANDING && mOptions[RSK_SHUFFLE_FREESTANDING].Is(RO_SHUFFLE_FREESTANDING_DUNGEONS)) ||
                (location.GetRCType() == RCTYPE_POT && mOptions[RSK_SHUFFLE_POTS].Is(RO_SHUFFLE_POTS_DUNGEONS))) {
                continue;
            }
            // If we've gotten past all the conditions where an overworld location should not be
            // shuffled, add it to the pool.
            AddLocation(location.GetRandomizerCheck());
        } else { // is a dungeon check
            auto* dungeon = GetDungeon(location.GetArea() - RCAREA_DEKU_TREE);
            if (location.GetQuest() == RCQUEST_BOTH || (location.GetQuest() == RCQUEST_MQ) == dungeon->IsMQ()) {
                if ((location.GetRCType() == RCTYPE_FREESTANDING &&
                     mOptions[RSK_SHUFFLE_FREESTANDING].Is(RO_SHUFFLE_FREESTANDING_OVERWORLD)) ||
                    (location.GetRCType() == RCTYPE_POT && mOptions[RSK_SHUFFLE_POTS].Is(RO_SHUFFLE_POTS_OVERWORLD))) {
                    continue;
                }
                // also add to that dungeon's location list.
                AddLocation(location.GetRandomizerCheck(), &dungeon->locations);
                AddLocation(location.GetRandomizerCheck());
            }
        }
    }
}

void Context::AddExcludedOptions() {
    for (auto& loc : StaticData::GetLocationTable()) {
        // Checks of these types don't have items, skip them.
        if (loc.GetRandomizerCheck() == RC_UNKNOWN_CHECK ||
            loc.GetRandomizerCheck() == RC_TRIFORCE_COMPLETED || loc.GetRCType() == RCTYPE_CHEST_GAME ||
            loc.GetRCType() == RCTYPE_STATIC_HINT || loc.GetRCType() == RCTYPE_GOSSIP_STONE) {
            continue;
        }
        AddLocation(loc.GetRandomizerCheck(), &everyPossibleLocation);
        bool alreadyAdded = false;
        for (Option* location : Rando::Settings::GetInstance()->GetExcludeOptionsForArea(loc.GetArea()))
        {
            if (location->GetName() == loc.GetExcludedOption()->GetName()) {
                alreadyAdded = true;
            }
        }
        if (!alreadyAdded) {
            Rando::Settings::GetInstance()->GetExcludeOptionsForArea(loc.GetArea()).push_back(loc.GetExcludedOption());
        }
    }
}

std::vector<RandomizerCheck> Context::GetLocations(const std::vector<RandomizerCheck>& locationPool, const RandomizerCheckType checkType) {
    std::vector<RandomizerCheck> locationsOfType;
    for (RandomizerCheck locKey : locationPool) {
        if (StaticData::GetLocation(locKey)->GetRCType() == checkType) {
            locationsOfType.push_back(locKey);
        }
    }
    return locationsOfType;
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

    GetItemLocation(RC_GIFT_FROM_RAURU)->ResetVariables();
    GetItemLocation(RC_LINKS_POCKET)->ResetVariables();
}

void Context::LocationReset() {
    for (auto& il : itemLocationTable) {
        il.RemoveFromPool();
    }
}

void Context::HintReset() {
    for (const RandomizerCheck il : StaticData::GetGossipStoneLocations()) {
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

void Context::ParseSpoiler(const char* spoilerFileName) {
    std::ifstream spoilerFileStream(sanitize(spoilerFileName));
    if (!spoilerFileStream) {
        return;
    }
    mSeedGenerated = false;
    mSpoilerLoaded = false;
    try {
        nlohmann::json spoilerFileJson;
        spoilerFileStream >> spoilerFileJson;
        ParseHashIconIndexesJson(spoilerFileJson);
        Rando::Settings::GetInstance()->ParseJson(spoilerFileJson);
        ParseItemLocationsJson(spoilerFileJson);
        ParseHintJson(spoilerFileJson);
        mEntranceShuffler->ParseJson(spoilerFileJson);
        mDungeons->ParseJson(spoilerFileJson);
        mTrials->ParseJson(spoilerFileJson);
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

OptionValue& Context::GetOption(const RandomizerSettingKey key) {
    return mOptions[key];
}

OptionValue& Context::GetTrickOption(const RandomizerTrick key) {
    return mTrickOptions[key];
}

OptionValue& Context::GetLocationOption(const RandomizerCheck key) {
    return itemLocationTable[key].GetExcludedOption();
}

RandoOptionLACSCondition Context::LACSCondition() const {
    return mLACSCondition;
}

std::shared_ptr<Kaleido> Context::GetKaleido() {
    if (mKaleido == nullptr) {
        mKaleido = std::make_shared<Kaleido>();
    }
    return mKaleido;
}

std::string Context::GetHash() const {
    return mHash;
}

void Context::SetHash(std::string hash) {
    mHash = std::move(hash);
}

const std::string& Context::GetSeedString() const {
    return mSeedString;
}

void Context::SetSeedString(std::string seedString) {
    mSeedString = std::move(seedString);
}

uint32_t Context::GetSeed() const {
    return mFinalSeed;
}

void Context::SetSeed(const uint32_t seed) {
    mFinalSeed = seed;
}
} // namespace Rando
