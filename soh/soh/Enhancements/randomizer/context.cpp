#include "context.h"
#include "static_data.h"
#include "soh/OTRGlobals.h"
#include "soh/Enhancements/item-tables/ItemTableManager.h"
#include "3drando/shops.hpp"
#include "dungeon.h"
#include "trial.h"
#include "entrance.h"
#include "settings.h"
#include "rando_hash.h"

#include <fstream>
#include <spdlog/spdlog.h>

namespace Rando {
std::weak_ptr<Context> Context::mContext;

Context::Context() {
    for (auto& location : StaticData::GetLocationTable()) {
        mSpoilerfileCheckNameToEnum[location.GetName()] = location.GetRandomizerCheck();
    }
    mSpoilerfileCheckNameToEnum["Invalid Location"] = RC_UNKNOWN_CHECK;
    mSpoilerfileCheckNameToEnum["Link's Pocket"] = RC_LINKS_POCKET;

    for (auto& item : StaticData::GetItemTable()) {
        // Easiest way to filter out all the empty values from the array, since we still technically want the 0/RG_NONE
        // entry
        if (item.GetName().english.empty())
            continue;
        mSpoilerfileGetNameToEnum[item.GetName().english] = item.GetRandomizerGet();
        mSpoilerfileGetNameToEnum[item.GetName().french] = item.GetRandomizerGet();
    }
    mSpoilerfileHintTypeNameToEnum = {
        { "Trial", HINT_TYPE_TRIAL },
        { "Always", HINT_TYPE_ALWAYS },
        { "WotH", HINT_TYPE_WOTH },
        { "Barren", HINT_TYPE_BARREN },
        { "Entrance", HINT_TYPE_ENTRANCE },
        { "Sometimes", HINT_TYPE_SOMETIMES },
        { "Random", HINT_TYPE_RANDOM },
        { "Static", HINT_TYPE_STATIC },
        { "Song", HINT_TYPE_SONG },
        { "Overworld", HINT_TYPE_OVERWORLD },
        { "Dungeon", HINT_TYPE_DUNGEON },
        { "Junk", HINT_TYPE_JUNK },
        { "Named Item", HINT_TYPE_NAMED_ITEM },
        { "Random", HINT_TYPE_RANDOM }
    };
    for (int i = 0; i < RC_MAX; i++) {
        itemLocationTable[i] = ItemLocation((RandomizerCheck)i);
    }
    mEntranceShuffler = std::make_shared<EntranceShuffler>();
    mDungeons = std::make_shared<Dungeons>();
    mTrials = std::make_shared<Trials>();
    mSettings = std::make_shared<Settings>();
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

Hint* Context::GetHint(RandomizerHintKey hintKey) {
    return &hintTable[hintKey];
}

void Context::AddHint(RandomizerHintKey hintId, Text text, RandomizerCheck hintedLocation, HintType hintType,
                      Text hintedRegion) {
    hintTable[hintId] = Hint(text, hintedLocation, hintType, hintedRegion);
    GetItemLocation(hintedLocation)->SetHintKey(hintId);
}

ItemLocation* Context::GetItemLocation(RandomizerCheck locKey) {
    return &(itemLocationTable[locKey]);
}

ItemLocation* Context::GetItemLocation(size_t locKey) {
    return &(itemLocationTable[static_cast<RandomizerCheck>(locKey)]);
}

void Context::PlaceItemInLocation(RandomizerCheck locKey, RandomizerGet item, bool applyEffectImmediately,
                                  bool setHidden) {
    auto loc = GetItemLocation(locKey);
    SPDLOG_DEBUG("\n");
    SPDLOG_DEBUG(StaticData::RetrieveItem(item).GetName().GetEnglish());
    SPDLOG_DEBUG(" placed at ");
    SPDLOG_DEBUG(StaticData::GetLocation(locKey)->GetName());
    SPDLOG_DEBUG("\n\n");

    if (applyEffectImmediately || mSettings->Setting(RSK_LOGIC_RULES).Is(RO_LOGIC_GLITCHLESS) || mSettings->Setting(RSK_LOGIC_RULES).Is(RO_LOGIC_VANILLA)) {
        Rando::StaticData::RetrieveItem(item).ApplyEffect();
    }

    // TODO? Show Progress

    // If we're placing a non-shop item in a shop location, we want to record it for custom messages
    if (StaticData::RetrieveItem(item).GetItemType() != ITEMTYPE_SHOP &&
        Rando::StaticData::GetLocation(locKey)->IsCategory(Category::cShop)) {
        int index = TransformShopIndex(GetShopIndex(locKey));
        NonShopItems[index].Name = Rando::StaticData::RetrieveItem(item).GetName();
        NonShopItems[index].Repurchaseable =
            Rando::StaticData::RetrieveItem(item).GetItemType() == ITEMTYPE_REFILL ||
            Rando::StaticData::RetrieveItem(item).GetHintKey() == RHT_PROGRESSIVE_BOMBCHUS;
    }

    loc->SetPlacedItem(item);
    if (setHidden) {
        loc->SetHidden(true);
    }
}

void Context::AddLocation(RandomizerCheck loc, std::vector<RandomizerCheck>* destination) {
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
    if (mSettings->Setting(RSK_TRIFORCE_HUNT)) {
        AddLocation(RC_TRIFORCE_COMPLETED);
    }
    AddLocations(StaticData::overworldLocations);

    for (auto dungeon : mDungeons->GetDungeonList()) {
        AddLocations(dungeon->GetDungeonLocations());
    }
}

void Context::AddExcludedOptions() {
    AddLocations(StaticData::overworldLocations, &everyPossibleLocation);
    for (auto dungeon : mDungeons->GetDungeonList()) {
        AddLocations(dungeon->GetEveryLocation(), &everyPossibleLocation);
    }
    for (RandomizerCheck rc : everyPossibleLocation) {
        GetItemLocation(rc)->AddExcludeOption();
    }
}

std::vector<RandomizerCheck> Context::GetLocations(const std::vector<RandomizerCheck>& locationPool,
                                                   Category categoryInclude, Category categoryExclude) {
    std::vector<RandomizerCheck> locationsInCategory;
    for (RandomizerCheck locKey : locationPool) {
        if (StaticData::GetLocation(locKey)->IsCategory(categoryInclude) &&
            !StaticData::GetLocation(locKey)->IsCategory(categoryExclude)) {
            locationsInCategory.push_back(locKey);
        }
    }
    return locationsInCategory;
}

void Context::ItemReset() {
    for (RandomizerCheck il : allLocations) {
        GetItemLocation(il)->ResetVariables();
    }

    for (RandomizerCheck il : Rando::StaticData::dungeonRewardLocations) {
        GetItemLocation(il)->ResetVariables();
    }
}

void Context::LocationReset() {
    for (RandomizerCheck il : allLocations) {
        GetItemLocation(il)->RemoveFromPool();
    }

    for (RandomizerCheck il : Rando::StaticData::dungeonRewardLocations) {
        GetItemLocation(il)->RemoveFromPool();
    }

    for (RandomizerCheck il : Rando::StaticData::gossipStoneLocations) {
        GetItemLocation(il)->RemoveFromPool();
    }

    GetItemLocation(RC_GANONDORF_HINT)->RemoveFromPool();
}

void Context::HintReset() {
    for (RandomizerCheck il : Rando::StaticData::gossipStoneLocations) {
        GetItemLocation(il)->ResetVariables();
        GetHint((RandomizerHintKey)(il - RC_COLOSSUS_GOSSIP_STONE + 1))->ResetVariables();
    }
}

void Context::CreateItemOverrides() {
    SPDLOG_DEBUG("NOW CREATING OVERRIDES\n\n");
    for (RandomizerCheck locKey : allLocations) {
        auto loc = Rando::StaticData::GetLocation(locKey);
        auto itemLoc = GetItemLocation(locKey);
        // If this is an ice trap, store the disguise model in iceTrapModels
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

bool Context::IsSeedGenerated() {
    return mSeedGenerated;
}

void Context::SetSeedGenerated(bool seedGenerated) {
    mSeedGenerated = seedGenerated;
}

bool Context::IsSpoilerLoaded() {
    return mSpoilerLoaded;
}

void Context::SetSpoilerLoaded(bool spoilerLoaded) {
    mSpoilerLoaded = spoilerLoaded;
}

bool Context::IsPlandoLoaded() {
    return mPlandoLoaded;
}

void Context::SetPlandoLoaded(bool plandoLoaded) {
    mPlandoLoaded = plandoLoaded;
}

GetItemEntry Context::GetFinalGIEntry(RandomizerCheck rc, bool checkObtainability, GetItemID ogItemId) {
    auto itemLoc = GetItemLocation(rc);
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
        auto fakeGiEntry = StaticData::RetrieveItem(overrides[rc].LooksLike()).GetGIEntry();
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
    stringValue.erase(std::remove_if(stringValue.begin(), stringValue.end(),
                                     [](char const c) { return '\n' == c || '\r' == c || '\0' == c || '\x1A' == c; }),
                      stringValue.end());

    return stringValue;
}

void Context::ParseSpoiler(const char* spoilerFileName, bool plandoMode) {
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
    } catch (std::exception& e) {
        throw e;
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
                    itemLocationTable[rc].SetPlacedItem(mSpoilerfileGetNameToEnum[itemit.value().template get<std::string>()]);
                } else if (itemit.key() == "price") {
                    itemLocationTable[rc].SetCustomPrice(itemit.value().template get<uint16_t>());
                } else if (itemit.key() == "model") {
                    overrides[rc] = ItemOverride(rc, mSpoilerfileGetNameToEnum[itemit.value().template get<std::string>()]);
                } else if (itemit.key() == "trickName") {
                    overrides[rc].SetTrickName(Text(itemit.value().template get<std::string>()));
                }
            }
        } else {
            itemLocationTable[rc].SetPlacedItem(mSpoilerfileGetNameToEnum[it.value().template get<std::string>()]);
        }
    }
}

std::string AltarIconString(char iconChar) {
    std::string iconString = "";
    switch (iconChar) {
        case '0':
            // Kokiri Emerald
            iconString += 0x13;
            iconString += 0x6C;
            break;
        case '1':
            // Goron Ruby
            iconString += 0x13;
            iconString += 0x6D;
            break;
        case '2':
            // Zora Sapphire
            iconString += 0x13;
            iconString += 0x6E;
            break;
        case '3':
            // Forest Medallion
            iconString += 0x13;
            iconString += 0x66;
            break;
        case '4':
            // Fire Medallion
            iconString += 0x13;
            iconString += 0x67;
            break;
        case '5':
            // Water Medallion
            iconString += 0x13;
            iconString += 0x68;
            break;
        case '6':
            // Spirit Medallion
            iconString += 0x13;
            iconString += 0x69;
            break;
        case '7':
            // Shadow Medallion
            iconString += 0x13;
            iconString += 0x6A;
            break;
        case '8':
            // Light Medallion
            iconString += 0x13;
            iconString += 0x6B;
            break;
        case 'o':
            // Open DOT (master sword)
            iconString += 0x13;
            iconString += 0x3C;
            break;
        case 'c':
            // Closed DOT (fairy ocarina)
            iconString += 0x13;
            iconString += 0x07;
            break;
        case 'i':
            // Intended DOT (oot)
            iconString += 0x13;
            iconString += 0x08;
            break;
        case 'l':
            // Light Arrow (for bridge reqs)
            iconString += 0x13;
            iconString += 0x12;
            break;
        case 'b':
            // Boss Key (ganon boss key location)
            iconString += 0x13;
            iconString += 0x74;
            break;
        case 'L':
            // Bow with Light Arrow
            iconString += 0x13;
            iconString += 0x3A;
            break;
        case 'k':
            // Kokiri Tunic
            iconString += 0x13;
            iconString += 0x41;
            break;
    }
    return iconString;
}

std::string FormatJsonHintText(std::string jsonHint) {
    std::string formattedHintMessage = jsonHint;

    // add icons to altar text
    for (char iconChar : { '0', '1', '2', '3', '4', '5', '6', '7', '8', 'o', 'c', 'i', 'l', 'b', 'L', 'k' }) {
        std::string textToReplace = "$";
        textToReplace += iconChar;
        size_t start_pos = formattedHintMessage.find(textToReplace);
        if (!(start_pos == std::string::npos)) {
            std::string iconString = AltarIconString(iconChar);
            formattedHintMessage.replace(start_pos, textToReplace.length(), iconString);
        }
    }
    return formattedHintMessage;
}

void Context::ParseHintJson(nlohmann::json spoilerFileJson) {
    // Child Altar
    std::string childAltarJsonText = spoilerFileJson["childAltar"]["hintText"].template get<std::string>();
    std::string formattedChildAltarText = FormatJsonHintText(childAltarJsonText);
    AddHint(RH_ALTAR_CHILD, Text(formattedChildAltarText), RC_UNKNOWN_CHECK, HINT_TYPE_STATIC, Text());
    mEmeraldLoc = mSpoilerfileCheckNameToEnum[spoilerFileJson["childAltar"]["rewards"]["emeraldLoc"]];
    mRubyLoc = mSpoilerfileCheckNameToEnum[spoilerFileJson["childAltar"]["rewards"]["rubyLoc"]];
    mSapphireLoc = mSpoilerfileCheckNameToEnum[spoilerFileJson["childAltar"]["rewards"]["sapphireLoc"]];
    // Adult Altar
    std::string adultAltarJsonText = spoilerFileJson["adultAltar"]["hintText"].template get<std::string>();
    std::string formattedAdultAltarText = FormatJsonHintText(adultAltarJsonText);
    AddHint(RH_ALTAR_ADULT, Text(formattedAdultAltarText), RC_UNKNOWN_CHECK, HINT_TYPE_STATIC, Text());
    mForestMedallionLoc = mSpoilerfileCheckNameToEnum[spoilerFileJson["adultAltar"]["rewards"]["forestMedallionLoc"].template get<std::string>()];
    mFireMedallionLoc = mSpoilerfileCheckNameToEnum[spoilerFileJson["adultAltar"]["rewards"]["fireMedallionLoc"].template get<std::string>()];
    mWaterMedallionLoc = mSpoilerfileCheckNameToEnum[spoilerFileJson["adultAltar"]["rewards"]["waterMedallionLoc"].template get<std::string>()];
    mShadowMedallionLoc = mSpoilerfileCheckNameToEnum[spoilerFileJson["adultAltar"]["rewards"]["shadowMedallionLoc"].template get<std::string>()];
    mSpiritMedallionLoc = mSpoilerfileCheckNameToEnum[spoilerFileJson["adultAltar"]["rewards"]["spiritMedallionLoc"].template get<std::string>()];
    mLightMedallionLoc = mSpoilerfileCheckNameToEnum[spoilerFileJson["adultAltar"]["rewards"]["lightMedallionLoc"].template get<std::string>()];
    // Ganondorf and Sheik Light Arrow Hints
    std::string ganonHintText = FormatJsonHintText(spoilerFileJson["ganonHintText"].template get<std::string>());
    RandomizerCheck lightArrowLoc = mSpoilerfileCheckNameToEnum[spoilerFileJson["lightArrowHintLoc"].template get<std::string>()];
    std::string lightArrowRegion = spoilerFileJson["lightArrowHintRegion"].template get<std::string>();
    AddHint(RH_GANONDORF_HINT, Text(ganonHintText), lightArrowLoc, HINT_TYPE_STATIC, Text(lightArrowRegion));
    std::string sheikText = FormatJsonHintText(spoilerFileJson["sheikText"].template get<std::string>());
    AddHint(RH_SHEIK_LIGHT_ARROWS, Text(sheikText), lightArrowLoc, HINT_TYPE_STATIC, lightArrowRegion);
    std::string ganonText = FormatJsonHintText(spoilerFileJson["ganonText"].template get<std::string>());
    AddHint(RH_GANONDORF_NOHINT, Text(ganonText), RC_UNKNOWN_CHECK, HINT_TYPE_JUNK, Text());
    // Dampe Hookshot Hint
    std::string dampeText = FormatJsonHintText(spoilerFileJson["dampeText"].template get<std::string>());
    std::string dampeRegion = spoilerFileJson["dampeRegion"].template get<std::string>();
    RandomizerCheck dampeHintLoc = mSpoilerfileCheckNameToEnum[spoilerFileJson["dampeHintLoc"].template get<std::string>()];
    AddHint(RH_DAMPES_DIARY, Text(dampeText), dampeHintLoc, HINT_TYPE_STATIC, Text(dampeRegion));
    // Greg Hint
    std::string gregText = FormatJsonHintText(spoilerFileJson["gregText"].template get<std::string>());
    std::string gregRegion = spoilerFileJson["gregRegion"].template get<std::string>();
    RandomizerCheck gregLoc = mSpoilerfileCheckNameToEnum[spoilerFileJson["gregLoc"].template get<std::string>()];
    AddHint(RH_GREG_RUPEE, Text(gregText), gregLoc, HINT_TYPE_STATIC, Text(gregRegion));
    // Saria Magic Hint
    std::string sariaText = FormatJsonHintText(spoilerFileJson["sariaText"].template get<std::string>());
    std::string sariaRegion = spoilerFileJson["sariaRegion"].template get<std::string>();
    RandomizerCheck sariaHintLoc = mSpoilerfileCheckNameToEnum[spoilerFileJson["sariaHintLoc"].template get<std::string>()];
    AddHint(RH_SARIA, Text(sariaText), sariaHintLoc, HINT_TYPE_STATIC, Text(sariaRegion));
    // Warp Songs
    std::string warpMinuetText = FormatJsonHintText(spoilerFileJson["warpMinuetText"].template get<std::string>());
    AddHint(RH_MINUET_WARP_LOC, Text(warpMinuetText), RC_UNKNOWN_CHECK, HINT_TYPE_STATIC, Text(warpMinuetText));
    std::string warpBoleroText = FormatJsonHintText(spoilerFileJson["warpBoleroText"].template get<std::string>());
    AddHint(RH_BOLERO_WARP_LOC, Text(warpBoleroText), RC_UNKNOWN_CHECK, HINT_TYPE_STATIC, Text(warpBoleroText));
    std::string warpSerenadeText = FormatJsonHintText(spoilerFileJson["warpSerenadeText"].template get<std::string>());
    AddHint(RH_SERENADE_WARP_LOC, Text(warpSerenadeText), RC_UNKNOWN_CHECK, HINT_TYPE_STATIC, Text(warpSerenadeText));
    std::string warpRequiemText = FormatJsonHintText(spoilerFileJson["warpRequiemText"].template get<std::string>());
    AddHint(RH_REQUIEM_WARP_LOC, Text(warpRequiemText), RC_UNKNOWN_CHECK, HINT_TYPE_STATIC, Text(warpRequiemText));
    std::string warpNocturneText = FormatJsonHintText(spoilerFileJson["warpNocturneText"].template get<std::string>());
    AddHint(RH_NOCTURNE_WARP_LOC, Text(warpNocturneText), RC_UNKNOWN_CHECK, HINT_TYPE_STATIC, Text(warpNocturneText));
    std::string warpPreludeText = FormatJsonHintText(spoilerFileJson["warpPreludeText"].template get<std::string>());
    AddHint(RH_PRELUDE_WARP_LOC, Text(warpPreludeText), RC_UNKNOWN_CHECK, HINT_TYPE_STATIC, Text(warpPreludeText));
    // Gossip Stones
    nlohmann::json hintsJson = spoilerFileJson["hints"];
    for (auto it = hintsJson.begin(); it != hintsJson.end(); it++) {
        RandomizerCheck gossipStoneLoc = mSpoilerfileCheckNameToEnum[it.key()];
        nlohmann::json hintInfo = it.value();
        std::string hintText = FormatJsonHintText(hintInfo["hint"].template get<std::string>());
        HintType hintType = mSpoilerfileHintTypeNameToEnum[hintInfo["type"].template get<std::string>()];
        RandomizerCheck hintedLocation = mSpoilerfileCheckNameToEnum[hintInfo["location"]];
        std::string hintedArea = hintInfo["area"].template get<std::string>();
        AddHint(RandomizerHintKey(gossipStoneLoc - RC_COLOSSUS_GOSSIP_STONE + 1), Text(hintText), hintedLocation, hintType, hintedArea);
    }
}

std::shared_ptr<Settings> Context::GetSettings() {
    return mSettings;
}

const std::shared_ptr<EntranceShuffler> Context::GetEntranceShuffler() {
    return mEntranceShuffler;
}

std::shared_ptr<Dungeons> Context::GetDungeons() {
    return mDungeons;
}

DungeonInfo* Context::GetDungeon(size_t key) {
    return mDungeons->GetDungeon(DungeonKey(key));
}

std::shared_ptr<Trials> Context::GetTrials() {
    return mTrials;
}

TrialInfo* Context::GetTrial(size_t key) {
    return mTrials->GetTrial(TrialKey(key));
}

Sprite* Context::GetSeedTexture(uint8_t index) {
    return &gSeedTextures[index];
}

Rando::Option& Context::GetOption(RandomizerSettingKey key) {
    return mSettings->Setting(key);
}

Rando::Option& Context::GetTrickOption(RandomizerTrick key) {
    return mSettings->GetTrickOption(key);
}
} // namespace Rando
