#include "context.h"
#include "static_data.h"
#include "soh/OTRGlobals.h"
#include "soh/Enhancements/item-tables/ItemTableManager.h"
#include "3drando/shops.hpp"
#include "dungeon.h"
#include "logic.h"
#include "trial.h"
#include "entrance.h"
#include "settings.h"
#include "rando_hash.h"
#include "fishsanity.h"

#include <fstream>
#include <spdlog/spdlog.h>

#include "luslog.h"

namespace Rando {
std::weak_ptr<Context> Context::mContext;

Context::Context() {
    mSpoilerfileCheckNameToEnum["Invalid Location"] = RC_UNKNOWN_CHECK;
    mSpoilerfileCheckNameToEnum["Link's Pocket"] = RC_LINKS_POCKET;

    for (auto& item : StaticData::GetItemTable()) {
        // Easiest way to filter out all the empty values from the array, since we still technically want the 0/RG_NONE
        // entry
        if (item.GetName().english.empty()) {
            continue;
        }
        mSpoilerfileGetNameToEnum[item.GetName().english] = item.GetRandomizerGet();
        mSpoilerfileGetNameToEnum[item.GetName().french] = item.GetRandomizerGet();
    }

    mSpoilerfileHintTypeNameToEnum = {
        { "Static", HINT_TYPE_STATIC },
        { "Trial", HINT_TYPE_TRIAL },
        { "WotH", HINT_TYPE_WOTH },
        { "Barren", HINT_TYPE_BARREN },
        { "Entrance", HINT_TYPE_ENTRANCE },
        { "Item Area", HINT_TYPE_ITEM_AREA },
        { "Item Location", HINT_TYPE_ITEM_LOCATION },
        { "Junk", HINT_TYPE_JUNK },
    };

    mSpoilerfileAreaNameToEnum = {
        {"No Hint", RA_NONE},
        {"Link's Pocket", RA_LINKS_POCKET},
        {"Kokiri Forest", RA_KOKIRI_FOREST},
        {"The Lost Woods", RA_THE_LOST_WOODS},
        {"Sacred Forest Meadow", RA_SACRED_FOREST_MEADOW},
        {"Hyrule Field", RA_HYRULE_FIELD},
        {"Lake Hylia", RA_LAKE_HYLIA},
        {"Gerudo Valley", RA_GERUDO_VALLEY},
        {"Gerudo Fortress", RA_GERUDO_FORTRESS},
        {"Haunted Wasteland", RA_HAUNTED_WASTELAND},
        {"Desert Colossus", RA_DESERT_COLOSSUS},
        {"The Market", RA_THE_MARKET},
        {"Temple of Time", RA_TEMPLE_OF_TIME},
        {"Hyrule Castle", RA_HYRULE_CASTLE},
        {"Outside Ganon's Castle", RA_OUTSIDE_GANONS_CASTLE},
        {"Castle Grounds", RA_CASTLE_GROUNDS},
        {"Kakariko Village", RA_KAKARIKO_VILLAGE},
        {"the Graveyard", RA_THE_GRAVEYARD},
        {"Death Mountain Trail", RA_DEATH_MOUNTAIN_TRAIL},
        {"Goron City", RA_GORON_CITY},
        {"Death Mountain Crater", RA_DEATH_MOUNTAIN_CRATER},
        {"Zora's River", RA_ZORAS_RIVER},
        {"Zora's Domain", RA_ZORAS_DOMAIN},
        {"Zora's Fountain", RA_ZORAS_FOUNTAIN},
        {"Lon Lon Ranch", RA_LON_LON_RANCH},
        {"Deku Tree", RA_DEKU_TREE},
        {"Dodongo's Cavern", RA_DODONGOS_CAVERN},
        {"Jabu-Jabu's Belly", RA_JABU_JABUS_BELLY},
        {"Forest Temple", RA_FOREST_TEMPLE},
        {"Fire Temple", RA_FIRE_TEMPLE},
        {"Water Temple", RA_WATER_TEMPLE},
        {"Spirit Temple", RA_SPIRIT_TEMPLE},
        {"Shadow Temple", RA_SHADOW_TEMPLE},
        {"Bottom of the Well", RA_BOTTOM_OF_THE_WELL},
        {"Ice Cavern", RA_ICE_CAVERN},
        {"Gerudo training Grounds", RA_GERUDO_TRAINING_GROUND},
        {"Inside Ganon's Castle", RA_GANONS_CASTLE},
    };

    for (int i = 0; i < RC_MAX; i++) {
        itemLocationTable[i] = ItemLocation(static_cast<RandomizerCheck>(i));
    }
    mEntranceShuffler = std::make_shared<EntranceShuffler>();
    mDungeons = std::make_shared<Dungeons>();
    mLogic = std::make_shared<Logic>();
    mTrials = std::make_shared<Trials>();
    mSettings = std::make_shared<Settings>();
    mFishsanity = std::make_shared<Fishsanity>();
    for (auto& location : StaticData::GetLocationTable()) {
        mSpoilerfileCheckNameToEnum[location.GetName()] = location.GetRandomizerCheck();
    }
}

RandomizerArea Context::GetAreaFromString(std::string str) {
    return mSpoilerfileAreaNameToEnum[str];
}

void Context::InitStaticData() {
    StaticData::InitItemTable();
    StaticData::InitLocationTable();
}

std::shared_ptr<Context> Context::CreateInstance() {
    if (mContext.expired()) {
        auto instance = std::make_shared<Context>();
        mContext = instance;
        return instance;
    }
    return GetInstance();
}

std::shared_ptr<Context> Context::GetInstance() {
    return mContext.lock();
}

Hint* Context::GetHint(const RandomizerHintKey hintKey) {
    return &hintTable[hintKey];
}

void Context::AddHint(const RandomizerHintKey hintId, const Text& text, const RandomizerCheck hintedLocation, const HintType hintType,
                      std::string distributionName, RandomizerArea hintedArea) {
    hintTable[hintId] = Hint(text, hintedLocation, hintType, distributionName, hintedArea);
    GetItemLocation(hintedLocation)->AddHintedBy(hintId);
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
    for (int i = 0; i < itemLocationTable.size(); i++) {
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

    for (const RandomizerCheck il : StaticData::otherHintLocations) {
        GetItemLocation(il)->RemoveFromPool();
    }
}

void Context::HintReset() {
    for (const RandomizerCheck il : StaticData::gossipStoneLocations) {
        GetItemLocation(il)->ResetVariables();
        GetHint(static_cast<RandomizerHintKey>(il - RC_COLOSSUS_GOSSIP_STONE + 1))->ResetVariables();
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
        RandomizerCheck rc = mSpoilerfileCheckNameToEnum[it.key()];
        if (it->is_structured()) {
            nlohmann::json itemJson = *it;
            for (auto itemit = itemJson.begin(); itemit != itemJson.end(); ++itemit) {
                if (itemit.key() == "item") {
                    itemLocationTable[rc].SetPlacedItem(mSpoilerfileGetNameToEnum[itemit.value().get<std::string>()]);
                } else if (itemit.key() == "price") {
                    itemLocationTable[rc].SetCustomPrice(itemit.value().get<uint16_t>());
                } else if (itemit.key() == "model") {
                    overrides[rc] = ItemOverride(rc, mSpoilerfileGetNameToEnum[itemit.value().get<std::string>()]);
                } else if (itemit.key() == "trickName") {
                    overrides[rc].SetTrickName(Text(itemit.value().get<std::string>()));
                }
            }
        } else {
            itemLocationTable[rc].SetPlacedItem(mSpoilerfileGetNameToEnum[it.value().get<std::string>()]);
        }
    }
}

void Context::ParseHintJson(nlohmann::json spoilerFileJson) {
    // Child Altar
    std::string childAltarText = spoilerFileJson["childAltar"]["hintText"].get<std::string>();
    AddHint(RH_ALTAR_CHILD, Text(childAltarText), RC_UNKNOWN_CHECK, HINT_TYPE_STATIC, "Static", RA_NONE);
    mEmeraldLoc = mSpoilerfileCheckNameToEnum[spoilerFileJson["childAltar"]["rewards"]["emeraldLoc"]];
    mRubyLoc = mSpoilerfileCheckNameToEnum[spoilerFileJson["childAltar"]["rewards"]["rubyLoc"]];
    mSapphireLoc = mSpoilerfileCheckNameToEnum[spoilerFileJson["childAltar"]["rewards"]["sapphireLoc"]];

    // Adult Altar
    std::string adultAltarText = spoilerFileJson["adultAltar"]["hintText"].get<std::string>();
    AddHint(RH_ALTAR_ADULT, Text(adultAltarText), RC_UNKNOWN_CHECK, HINT_TYPE_STATIC, "Static", RA_NONE);
    mForestMedallionLoc = mSpoilerfileCheckNameToEnum[spoilerFileJson["adultAltar"]["rewards"]["forestMedallionLoc"].get<std::string>()];
    mFireMedallionLoc = mSpoilerfileCheckNameToEnum[spoilerFileJson["adultAltar"]["rewards"]["fireMedallionLoc"].get<std::string>()];
    mWaterMedallionLoc = mSpoilerfileCheckNameToEnum[spoilerFileJson["adultAltar"]["rewards"]["waterMedallionLoc"].get<std::string>()];
    mShadowMedallionLoc = mSpoilerfileCheckNameToEnum[spoilerFileJson["adultAltar"]["rewards"]["shadowMedallionLoc"].get<std::string>()];
    mSpiritMedallionLoc = mSpoilerfileCheckNameToEnum[spoilerFileJson["adultAltar"]["rewards"]["spiritMedallionLoc"].get<std::string>()];
    mLightMedallionLoc = mSpoilerfileCheckNameToEnum[spoilerFileJson["adultAltar"]["rewards"]["lightMedallionLoc"].get<std::string>()];

    // Ganondorf and Sheik Light Arrow Hints
    std::string ganonHintText = spoilerFileJson["ganonHintText"].get<std::string>();
    RandomizerCheck lightArrowLoc = mSpoilerfileCheckNameToEnum[spoilerFileJson["lightArrowHintLoc"].get<std::string>()];
    std::string lightArrowRegion = spoilerFileJson["lightArrowArea"].get<std::string>();
    AddHint(RH_GANONDORF_HINT, Text(ganonHintText), lightArrowLoc, HINT_TYPE_STATIC, "Static", mSpoilerfileAreaNameToEnum[lightArrowRegion]);
    if (spoilerFileJson.contains("sheikText")) {
        std::string sheikText = spoilerFileJson["sheikText"].get<std::string>();
        AddHint(RH_SHEIK_LIGHT_ARROWS, Text(sheikText), lightArrowLoc, HINT_TYPE_STATIC, lightArrowRegion);
    }
    std::string ganonText = spoilerFileJson["ganonText"].get<std::string>();
    AddHint(RH_GANONDORF_NOHINT, Text(ganonText), RC_UNKNOWN_CHECK, HINT_TYPE_JUNK, "Static", RA_GANONS_CASTLE);

    // Dampe Hookshot Hint
    if (spoilerFileJson.contains("dampeText")) {
        std::string dampeText = spoilerFileJson["dampeText"].get<std::string>();
        std::string dampeRegion = spoilerFileJson["dampeRegion"].get<std::string>();
        RandomizerCheck dampeHintLoc = mSpoilerfileCheckNameToEnum[spoilerFileJson["dampeHintLoc"].get<std::string>()];
        AddHint(RH_DAMPES_DIARY, Text(dampeText), dampeHintLoc, HINT_TYPE_STATIC, "Static", mSpoilerfileAreaNameToEnum[dampeRegion]);
    }

    // Greg Hint
    if (spoilerFileJson.contains("gregText")) {
        std::string gregText = spoilerFileJson["gregText"].get<std::string>();
        std::string gregRegion = spoilerFileJson["gregRegion"].get<std::string>();
        RandomizerCheck gregLoc = mSpoilerfileCheckNameToEnum[spoilerFileJson["gregLoc"].get<std::string>()];
        AddHint(RH_GREG_RUPEE, Text(gregText), gregLoc, HINT_TYPE_STATIC, "Static", mSpoilerfileAreaNameToEnum[gregRegion]);
    }

    // Saria Magic Hint
    if (spoilerFileJson.contains("sariaText")) {
        std::string sariaText = spoilerFileJson["sariaText"].get<std::string>();
        std::string sariaRegion = spoilerFileJson["sariaRegion"].get<std::string>();
        RandomizerCheck sariaHintLoc = mSpoilerfileCheckNameToEnum[spoilerFileJson["sariaHintLoc"].get<std::string>()];
        AddHint(RH_SARIA, Text(sariaText), sariaHintLoc, HINT_TYPE_STATIC, "Static", mSpoilerfileAreaNameToEnum[sariaRegion]);
    }

    // Fishing Pole Hint
    if(spoilerFileJson.contains("fishingPoleText")) {
        std::string fishingPoleText = spoilerFileJson["fishingPoleText"].get<std::string>();
        std::string fishingPoleRegion = spoilerFileJson["fishingPoleRegion"].get<std::string>();
        RandomizerCheck fishingPoleHintLoc = mSpoilerfileCheckNameToEnum[spoilerFileJson["fishingPoleHintLoc"].get<std::string>()];
        AddHint(RH_FISHING_POLE, Text(fishingPoleText), fishingPoleHintLoc, HINT_TYPE_STATIC, "Static", mSpoilerfileAreaNameToEnum[fishingPoleRegion]);
    }

    // Warp Songs
    if (spoilerFileJson.contains("warpMinuetText")) {
        std::string warpMinuetText = spoilerFileJson["warpMinuetText"].get<std::string>(); //RANDOTODO fall back for if location is used
        AddHint(RH_MINUET_WARP_LOC, Text(warpMinuetText), RC_UNKNOWN_CHECK, HINT_TYPE_STATIC, "Static", mSpoilerfileAreaNameToEnum[warpMinuetText]);
    }
    if (spoilerFileJson.contains("warpBoleroText")) {
        std::string warpBoleroText = spoilerFileJson["warpBoleroText"].get<std::string>();
        AddHint(RH_BOLERO_WARP_LOC, Text(warpBoleroText), RC_UNKNOWN_CHECK, HINT_TYPE_STATIC, "Static", mSpoilerfileAreaNameToEnum[warpBoleroText]);
    }
    if (spoilerFileJson.contains("warpSerenadeText")) {
        std::string warpSerenadeText = spoilerFileJson["warpSerenadeText"].get<std::string>();
        AddHint(RH_SERENADE_WARP_LOC, Text(warpSerenadeText), RC_UNKNOWN_CHECK, HINT_TYPE_STATIC, "Static", mSpoilerfileAreaNameToEnum[warpSerenadeText]);
    }
    if (spoilerFileJson.contains("warpRequiemText")) {
        std::string warpRequiemText = spoilerFileJson["warpRequiemText"].get<std::string>();
        AddHint(RH_REQUIEM_WARP_LOC, Text(warpRequiemText), RC_UNKNOWN_CHECK, HINT_TYPE_STATIC, "Static", mSpoilerfileAreaNameToEnum[warpRequiemText]);
    }
    if (spoilerFileJson.contains("warpNocturneText")) {
        std::string warpNocturneText = spoilerFileJson["warpNocturneText"].get<std::string>();
        AddHint(RH_NOCTURNE_WARP_LOC, Text(warpNocturneText), RC_UNKNOWN_CHECK, HINT_TYPE_STATIC, "Static", mSpoilerfileAreaNameToEnum[warpNocturneText]);
    }
    if (spoilerFileJson.contains("warpPreludeText")) {
        std::string warpPreludeText = spoilerFileJson["warpPreludeText"].get<std::string>();
        AddHint(RH_PRELUDE_WARP_LOC, Text(warpPreludeText), RC_UNKNOWN_CHECK, HINT_TYPE_STATIC, "Static", mSpoilerfileAreaNameToEnum[warpPreludeText]);
    }

    // Gossip Stones
    nlohmann::json hintsJson = spoilerFileJson["hints"];
    for (auto it = hintsJson.begin(); it != hintsJson.end(); ++it) {
        RandomizerCheck gossipStoneLoc = mSpoilerfileCheckNameToEnum[it.key()];
        nlohmann::json hintInfo = it.value();
        std::string hintText = hintInfo["hint"].get<std::string>();
        HintType hintType = mSpoilerfileHintTypeNameToEnum[hintInfo["type"].get<std::string>()];
        RandomizerCheck hintedLocation = hintInfo.contains("location") ? mSpoilerfileCheckNameToEnum[hintInfo["location"]] : RC_UNKNOWN_CHECK;
        RandomizerArea hintedArea = hintInfo.contains("area") ? mSpoilerfileAreaNameToEnum[hintInfo["area"].get<std::string>()] : RA_NONE;
        std::string distribution = hintInfo["distribution"].get<std::string>();
        AddHint(static_cast<RandomizerHintKey>(gossipStoneLoc - RC_COLOSSUS_GOSSIP_STONE + 1), Text(hintText), hintedLocation, hintType, distribution, hintedArea);
    }
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

void Context::ResetLogic() {
    mLogic->Reset();
}

std::shared_ptr<Trials> Context::GetTrials() {
    return mTrials;
}

TrialInfo* Context::GetTrial(size_t key) const {
    return mTrials->GetTrial(static_cast<TrialKey>(key));
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
} // namespace Rando
