#include "randomizer.h"
#include <nlohmann/json.hpp>
#include <fstream>
#include <variables.h>
#include <macros.h>
#include <objects/gameplay_keep/gameplay_keep.h>
#include <functions.h>
#include <libultraship/libultraship.h>
#include <textures/icon_item_static/icon_item_static.h>
#include <textures/icon_item_24_static/icon_item_24_static.h>
#include "3drando/menu.hpp"
#include "soh/ResourceManagerHelpers.h"
#include "soh/SohGui/SohGui.hpp"
#include <imgui.h>
#include <imgui_internal.h>
#include "../../../src/overlays/actors/ovl_En_GirlA/z_en_girla.h"
#include "randomizer_check_objects.h"
#include <sstream>
#include <tuple>
#include "draw.h"
#include "soh/OTRGlobals.h"
#include <ship/window/FileDropMgr.h>
#include "static_data.h"
#include "soh/Enhancements/game-interactor/GameInteractor.h"
#include "trial.h"
#include "settings.h"
#include "soh/util.h"
#include "randomizerTypes.h"
#include "soh/Notification/Notification.h"
#include "soh/ObjectExtension/ObjectExtension.h"
#include "soh/Enhancements/randomizer/RCToRandInf.h"

extern "C" {
#include "src/overlays/actors/ovl_Obj_Bean/z_obj_bean.h"

extern void func_80B8FE00(ObjBean*); // trigger planting
}

static ObjectExtension::Register<CheckIdentity> RegisterIdentity;

extern std::map<RandomizerCheckArea, std::string> rcAreaNames;

using json = nlohmann::json;
using namespace std::literals::string_literals;

std::unordered_map<std::string, RandomizerCheckArea> SpoilerfileAreaNameToEnum;
std::unordered_map<std::string, HintType> SpoilerfileHintTypeNameToEnum;
std::set<RandomizerCheck> excludedLocations;
std::set<RandomizerCheck> spoilerExcludedLocations;

bool generated;

bool Rando_HandleSpoilerDrop(char* filePath) {
    if (SohUtils::IsStringEmpty(filePath)) {
        return false;
    }

    try {
        std::ifstream stream(filePath);
        if (!stream) {
            return false;
        }

        nlohmann::json json;
        stream >> json;

        if (json.contains("version") && json.contains("finalSeed")) {
            CVarSetString(CVAR_GENERAL("RandomizerDroppedFile"), filePath);
            CVarSetInteger(CVAR_GENERAL("RandomizerNewFileDropped"), 1);
            return true;
        }
    } catch (std::exception& e) {}
    return false;
}

Randomizer::Randomizer() {
    Rando::StaticData::InitItemTable();
    Rando::StaticData::InitLocationTable();

    for (auto area : rcAreaNames) {
        SpoilerfileAreaNameToEnum[area.second] = area.first;
    }
    SpoilerfileAreaNameToEnum["Inside Ganon's Castle"] = RCAREA_GANONS_CASTLE;
    SpoilerfileAreaNameToEnum["the Lost Woods"] = RCAREA_LOST_WOODS;
    SpoilerfileAreaNameToEnum["the Market"] = RCAREA_MARKET;
    SpoilerfileAreaNameToEnum["the Graveyard"] = RCAREA_GRAVEYARD;
    SpoilerfileAreaNameToEnum["Haunted Wasteland"] = RCAREA_WASTELAND;
    SpoilerfileAreaNameToEnum["outside Ganon's Castle"] = RCAREA_HYRULE_CASTLE;
    for (size_t c = 0; c < Rando::StaticData::hintTypeNames.size(); c++) {
        SpoilerfileHintTypeNameToEnum[Rando::StaticData::hintTypeNames[(HintType)c].GetEnglish(MF_CLEAN)] = (HintType)c;
    }

    Ship::Context::GetRawInstance()->GetFileDropMgr()->RegisterDropHandler(Rando_HandleSpoilerDrop);
}

Randomizer::~Randomizer() {
}

std::unordered_map<std::string, SceneID> spoilerFileDungeonToScene = {
    { "Deku Tree", SCENE_DEKU_TREE },
    { "Dodongo's Cavern", SCENE_DODONGOS_CAVERN },
    { "Jabu Jabu's Belly", SCENE_JABU_JABU },
    { "Forest Temple", SCENE_FOREST_TEMPLE },
    { "Fire Temple", SCENE_FIRE_TEMPLE },
    { "Water Temple", SCENE_WATER_TEMPLE },
    { "Spirit Temple", SCENE_SPIRIT_TEMPLE },
    { "Shadow Temple", SCENE_SHADOW_TEMPLE },
    { "Bottom of the Well", SCENE_BOTTOM_OF_THE_WELL },
    { "Ice Cavern", SCENE_ICE_CAVERN },
    { "Gerudo Training Ground", SCENE_GERUDO_TRAINING_GROUND },
    { "Ganon's Castle", SCENE_INSIDE_GANONS_CASTLE }
};

// used for items that only set a rand inf when obtained
std::unordered_map<RandomizerGet, RandomizerInf> randomizerGetToRandInf = {
    { RG_FISHING_POLE, RAND_INF_FISHING_POLE_FOUND },
    { RG_BRONZE_SCALE, RAND_INF_CAN_SWIM },
    { RG_POWER_BRACELET, RAND_INF_CAN_GRAB },
    { RG_CLIMB, RAND_INF_CAN_CLIMB },
    { RG_CRAWL, RAND_INF_CAN_CRAWL },
    { RG_OPEN_CHEST, RAND_INF_CAN_OPEN_CHEST },
    { RG_SPEAK_DEKU, RAND_INF_CAN_SPEAK_DEKU },
    { RG_SPEAK_GERUDO, RAND_INF_CAN_SPEAK_GERUDO },
    { RG_SPEAK_GORON, RAND_INF_CAN_SPEAK_GORON },
    { RG_SPEAK_HYLIAN, RAND_INF_CAN_SPEAK_HYLIAN },
    { RG_SPEAK_KOKIRI, RAND_INF_CAN_SPEAK_KOKIRI },
    { RG_SPEAK_ZORA, RAND_INF_CAN_SPEAK_ZORA },
    { RG_QUIVER_INF, RAND_INF_HAS_INFINITE_QUIVER },
    { RG_BOMB_BAG_INF, RAND_INF_HAS_INFINITE_BOMB_BAG },
    { RG_BULLET_BAG_INF, RAND_INF_HAS_INFINITE_BULLET_BAG },
    { RG_STICK_UPGRADE_INF, RAND_INF_HAS_INFINITE_STICK_UPGRADE },
    { RG_NUT_UPGRADE_INF, RAND_INF_HAS_INFINITE_NUT_UPGRADE },
    { RG_MAGIC_INF, RAND_INF_HAS_INFINITE_MAGIC_METER },
    { RG_BOMBCHU_INF, RAND_INF_HAS_INFINITE_BOMBCHUS },
    { RG_WALLET_INF, RAND_INF_HAS_INFINITE_MONEY },
    { RG_OCARINA_A_BUTTON, RAND_INF_HAS_OCARINA_A },
    { RG_OCARINA_C_UP_BUTTON, RAND_INF_HAS_OCARINA_C_UP },
    { RG_OCARINA_C_DOWN_BUTTON, RAND_INF_HAS_OCARINA_C_DOWN },
    { RG_OCARINA_C_LEFT_BUTTON, RAND_INF_HAS_OCARINA_C_LEFT },
    { RG_OCARINA_C_RIGHT_BUTTON, RAND_INF_HAS_OCARINA_C_RIGHT },
    { RG_DEATH_MOUNTAIN_CRATER_BEAN_SOUL, RAND_INF_DEATH_MOUNTAIN_CRATER_BEAN_SOUL },
    { RG_DEATH_MOUNTAIN_TRAIL_BEAN_SOUL, RAND_INF_DEATH_MOUNTAIN_TRAIL_BEAN_SOUL },
    { RG_DESERT_COLOSSUS_BEAN_SOUL, RAND_INF_DESERT_COLOSSUS_BEAN_SOUL },
    { RG_GERUDO_VALLEY_BEAN_SOUL, RAND_INF_GERUDO_VALLEY_BEAN_SOUL },
    { RG_GRAVEYARD_BEAN_SOUL, RAND_INF_GRAVEYARD_BEAN_SOUL },
    { RG_KOKIRI_FOREST_BEAN_SOUL, RAND_INF_KOKIRI_FOREST_BEAN_SOUL },
    { RG_LAKE_HYLIA_BEAN_SOUL, RAND_INF_LAKE_HYLIA_BEAN_SOUL },
    { RG_LOST_WOODS_BRIDGE_BEAN_SOUL, RAND_INF_LOST_WOODS_BRIDGE_BEAN_SOUL },
    { RG_LOST_WOODS_BEAN_SOUL, RAND_INF_LOST_WOODS_BEAN_SOUL },
    { RG_ZORAS_RIVER_BEAN_SOUL, RAND_INF_ZORAS_RIVER_BEAN_SOUL },
    { RG_GOHMA_SOUL, RAND_INF_GOHMA_SOUL },
    { RG_KING_DODONGO_SOUL, RAND_INF_KING_DODONGO_SOUL },
    { RG_BARINADE_SOUL, RAND_INF_BARINADE_SOUL },
    { RG_PHANTOM_GANON_SOUL, RAND_INF_PHANTOM_GANON_SOUL },
    { RG_VOLVAGIA_SOUL, RAND_INF_VOLVAGIA_SOUL },
    { RG_MORPHA_SOUL, RAND_INF_MORPHA_SOUL },
    { RG_BONGO_BONGO_SOUL, RAND_INF_BONGO_BONGO_SOUL },
    { RG_TWINROVA_SOUL, RAND_INF_TWINROVA_SOUL },
    { RG_GANON_SOUL, RAND_INF_GANON_SOUL },
};

#ifdef _MSC_VER
#pragma optimize("", off)
#else
#pragma GCC push_options
#pragma GCC optimize("O0")
#endif
bool Randomizer::SpoilerFileExists(const char* spoilerFileName) {
    static std::unordered_map<std::string, bool> existsCache;
    static std::unordered_map<std::string, std::filesystem::file_time_type> lastModifiedCache;

    if (strcmp(spoilerFileName, "") == 0) {
        return false;
    }

    std::string sanitizedFileName = SohUtils::Sanitize(spoilerFileName);

    try {
        // Check if file exists and get last modified time
        std::filesystem::path filePath(sanitizedFileName);
        if (!std::filesystem::exists(filePath)) {
            // Cache and return false if file doesn't exist
            existsCache[sanitizedFileName] = false;
            lastModifiedCache.erase(sanitizedFileName);
            return false;
        }

        auto currentLastModified = std::filesystem::last_write_time(filePath);

        // Check cache first
        auto existsCacheIt = existsCache.find(sanitizedFileName);
        auto lastModifiedCacheIt = lastModifiedCache.find(sanitizedFileName);

        // If we have a valid cache entry and the file hasn't been modified
        if (existsCacheIt != existsCache.end() && lastModifiedCacheIt != lastModifiedCache.end() &&
            lastModifiedCacheIt->second == currentLastModified) {
            return existsCacheIt->second;
        }

        // Cache miss or file modified - need to check contents
        std::ifstream spoilerFileStream(sanitizedFileName);
        if (spoilerFileStream) {
            nlohmann::json contents;
            spoilerFileStream >> contents;
            spoilerFileStream.close();

            bool isValid = contents.contains("version") &&
                           strcmp(std::string(contents["version"]).c_str(), (char*)gBuildVersion) == 0;

            if (!isValid) {
                SohGui::RegisterPopup(
                    "Old Spoiler Version",
                    "The spoiler file located at\n" + std::string(spoilerFileName) +
                        "\nwas made by a version that doesn't match the currently running version.\n" +
                        "Loading for this file has been cancelled.");
                CVarClear(CVAR_GENERAL("SpoilerLog"));
                Ship::Context::GetRawInstance()->GetWindow()->GetGui()->SaveConsoleVariablesNextFrame();
            }

            // Update cache
            existsCache[sanitizedFileName] = isValid;
            lastModifiedCache[sanitizedFileName] = currentLastModified;
            return isValid;
        }

        // File couldn't be opened
        existsCache[sanitizedFileName] = false;
        lastModifiedCache.erase(sanitizedFileName);
        return false;

    } catch (const std::filesystem::filesystem_error&) {
        // Handle filesystem errors by invalidating cache
        existsCache[sanitizedFileName] = false;
        lastModifiedCache.erase(sanitizedFileName);
        return false;
    }
}
#ifdef _MSC_VER
#pragma optimize("", on)
#else
#pragma GCC pop_options
#endif

// Reference soh/src/overlays/actors/ovl_En_GirlA/z_en_girla.h
std::unordered_map<RandomizerGet, EnGirlAShopItem> randomizerGetToEnGirlShopItem = {
    { RG_BUY_DEKU_NUTS_5, SI_DEKU_NUTS_5 },
    { RG_BUY_ARROWS_30, SI_ARROWS_30 },
    { RG_BUY_ARROWS_50, SI_ARROWS_50 },
    { RG_BUY_BOMBS_525, SI_BOMBS_5_R25 },
    { RG_BUY_DEKU_NUTS_10, SI_DEKU_NUTS_10 },
    { RG_BUY_DEKU_STICK_1, SI_DEKU_STICK },
    { RG_BUY_BOMBS_10, SI_BOMBS_10 },
    { RG_BUY_FISH, SI_FISH },
    { RG_BUY_RED_POTION_30, SI_RED_POTION_R30 },
    { RG_BUY_GREEN_POTION, SI_GREEN_POTION },
    { RG_BUY_BLUE_POTION, SI_BLUE_POTION },
    { RG_BUY_HYLIAN_SHIELD, SI_HYLIAN_SHIELD },
    { RG_BUY_DEKU_SHIELD, SI_DEKU_SHIELD },
    { RG_BUY_GORON_TUNIC, SI_GORON_TUNIC },
    { RG_BUY_ZORA_TUNIC, SI_ZORA_TUNIC },
    { RG_BUY_HEART, SI_RECOVERY_HEART },
    { RG_BUY_BOMBCHUS_10, SI_BOMBCHU_10_1 },
    { RG_BUY_BOMBCHUS_20, SI_BOMBCHU_20_1 },
    { RG_BUY_DEKU_SEEDS_30, SI_DEKU_SEEDS_30 },
    { RG_BUY_BLUE_FIRE, SI_BLUE_FIRE },
    { RG_BUY_BOTTLE_BUG, SI_BUGS },
    { RG_BUY_POE, SI_POE },
    { RG_BUY_FAIRYS_SPIRIT, SI_FAIRY },
    { RG_BUY_ARROWS_10, SI_ARROWS_10 },
    { RG_BUY_BOMBS_20, SI_BOMBS_20 },
    { RG_BUY_BOMBS_30, SI_BOMBS_30 },
    { RG_BUY_BOMBS_535, SI_BOMBS_5_R35 },
    { RG_BUY_RED_POTION_40, SI_RED_POTION_R40 },
    { RG_BUY_RED_POTION_50, SI_RED_POTION_R50 },
};

std::map<s32, TrialKey> trialFlagToTrialKey = {
    { EVENTCHKINF_COMPLETED_LIGHT_TRIAL, TK_LIGHT_TRIAL },   { EVENTCHKINF_COMPLETED_FOREST_TRIAL, TK_FOREST_TRIAL },
    { EVENTCHKINF_COMPLETED_FIRE_TRIAL, TK_FIRE_TRIAL },     { EVENTCHKINF_COMPLETED_WATER_TRIAL, TK_WATER_TRIAL },
    { EVENTCHKINF_COMPLETED_SPIRIT_TRIAL, TK_SPIRIT_TRIAL }, { EVENTCHKINF_COMPLETED_SHADOW_TRIAL, TK_SHADOW_TRIAL },
};

bool Randomizer::IsTrialRequired(s32 trialFlag) {
    return Rando::Context::GetInstance()->GetTrial(trialFlagToTrialKey[trialFlag])->IsRequired();
}

GetItemEntry Randomizer::GetItemFromActor(s16 actorId, s16 sceneNum, s16 actorParams, GetItemID ogItemId,
                                          bool checkObtainability) {
    return Rando::Context::GetInstance()->GetFinalGIEntry(GetCheckFromActor(actorId, sceneNum, actorParams),
                                                          checkObtainability, ogItemId);
}

ItemObtainability Randomizer::GetItemObtainabilityFromRandomizerCheck(RandomizerCheck randomizerCheck) {
    return GetItemObtainabilityFromRandomizerGet(
        Rando::Context::GetInstance()->GetItemLocation(randomizerCheck)->GetPlacedRandomizerGet());
}

ItemObtainability Randomizer::GetItemObtainabilityFromRandomizerGet(RandomizerGet randoGet) {
    if (randomizerGetToRandInf.find(randoGet) != randomizerGetToRandInf.end()) {
        return Flags_GetRandomizerInf(randomizerGetToRandInf.find(randoGet)->second) ? CANT_OBTAIN_ALREADY_HAVE
                                                                                     : CAN_OBTAIN;
    }

    // This is needed since Plentiful item pool also adds a third progressive wallet
    // but we should not get Tycoon's Wallet from it if it is off.
    bool tycoonWallet = GetRandoSettingValue(RSK_INCLUDE_TYCOON_WALLET);

    // Same thing with the infinite upgrades, if we're not shuffling them
    // and we're using the Plentiful item pool, we should prevent the infinite
    // upgrades from being gotten
    u8 infiniteUpgrades = GetRandoSettingValue(RSK_INFINITE_UPGRADES);

    u8 numWallets = 2 + (u8)tycoonWallet + (infiniteUpgrades != RO_INF_UPGRADES_OFF ? 1 : 0);
    switch (randoGet) {
        case RG_NONE:
        case RG_TRIFORCE:
        case RG_HINT:
        case RG_MAX:
        case RG_SOLD_OUT:
            return CANT_OBTAIN_MISC;

        // Equipment
        case RG_KOKIRI_SWORD:
            return !CHECK_OWNED_EQUIP(EQUIP_TYPE_SWORD, EQUIP_INV_SWORD_KOKIRI) ? CAN_OBTAIN : CANT_OBTAIN_ALREADY_HAVE;
        case RG_MASTER_SWORD:
            return !CHECK_OWNED_EQUIP(EQUIP_TYPE_SWORD, EQUIP_INV_SWORD_MASTER) ? CAN_OBTAIN : CANT_OBTAIN_ALREADY_HAVE;
        case RG_BIGGORON_SWORD:
            return !gSaveContext.bgsFlag ? CAN_OBTAIN : CANT_OBTAIN_ALREADY_HAVE;
        case RG_DEKU_SHIELD:
        case RG_BUY_DEKU_SHIELD:
            return !CHECK_OWNED_EQUIP(EQUIP_TYPE_SHIELD, EQUIP_INV_SHIELD_DEKU) ? CAN_OBTAIN : CANT_OBTAIN_ALREADY_HAVE;
        case RG_HYLIAN_SHIELD:
        case RG_BUY_HYLIAN_SHIELD:
            return !CHECK_OWNED_EQUIP(EQUIP_TYPE_SHIELD, EQUIP_INV_SHIELD_HYLIAN) ? CAN_OBTAIN
                                                                                  : CANT_OBTAIN_ALREADY_HAVE;
        case RG_MIRROR_SHIELD:
            return !CHECK_OWNED_EQUIP(EQUIP_TYPE_SHIELD, EQUIP_INV_SHIELD_MIRROR) ? CAN_OBTAIN
                                                                                  : CANT_OBTAIN_ALREADY_HAVE;
        case RG_GORON_TUNIC:
        case RG_BUY_GORON_TUNIC:
            return !CHECK_OWNED_EQUIP(EQUIP_TYPE_TUNIC, EQUIP_INV_TUNIC_GORON) ? CAN_OBTAIN : CANT_OBTAIN_ALREADY_HAVE;
        case RG_ZORA_TUNIC:
        case RG_BUY_ZORA_TUNIC:
            return !CHECK_OWNED_EQUIP(EQUIP_TYPE_TUNIC, EQUIP_INV_TUNIC_ZORA) ? CAN_OBTAIN : CANT_OBTAIN_ALREADY_HAVE;
        case RG_IRON_BOOTS:
            return !CHECK_OWNED_EQUIP(EQUIP_TYPE_BOOTS, EQUIP_INV_BOOTS_IRON) ? CAN_OBTAIN : CANT_OBTAIN_ALREADY_HAVE;
        case RG_HOVER_BOOTS:
            return !CHECK_OWNED_EQUIP(EQUIP_TYPE_BOOTS, EQUIP_INV_BOOTS_HOVER) ? CAN_OBTAIN : CANT_OBTAIN_ALREADY_HAVE;

        // Inventory Items
        case RG_PROGRESSIVE_STICK_UPGRADE:
            return infiniteUpgrades != RO_INF_UPGRADES_OFF
                       ? (Flags_GetRandomizerInf(RAND_INF_HAS_INFINITE_STICK_UPGRADE) ? CANT_OBTAIN_ALREADY_HAVE
                                                                                      : CAN_OBTAIN)
                       : (CUR_UPG_VALUE(UPG_STICKS) < 3 ? CAN_OBTAIN : CANT_OBTAIN_ALREADY_HAVE);
        case RG_DEKU_STICK_1:
        case RG_BUY_DEKU_STICK_1:
            return CUR_UPG_VALUE(UPG_STICKS) ||
                           !OTRGlobals::Instance->gRandoContext->GetOption(RSK_SHUFFLE_DEKU_STICK_BAG).Get()
                       ? CAN_OBTAIN
                       : CANT_OBTAIN_NEED_UPGRADE;
        case RG_PROGRESSIVE_NUT_UPGRADE:
            return infiniteUpgrades != RO_INF_UPGRADES_OFF
                       ? (Flags_GetRandomizerInf(RAND_INF_HAS_INFINITE_NUT_UPGRADE) ? CANT_OBTAIN_ALREADY_HAVE
                                                                                    : CAN_OBTAIN)
                       : (CUR_UPG_VALUE(UPG_NUTS) < 3 ? CAN_OBTAIN : CANT_OBTAIN_ALREADY_HAVE);
        case RG_DEKU_NUTS_5:
        case RG_DEKU_NUTS_10:
        case RG_BUY_DEKU_NUTS_5:
        case RG_BUY_DEKU_NUTS_10:
            return CUR_UPG_VALUE(UPG_NUTS) ||
                           !OTRGlobals::Instance->gRandoContext->GetOption(RSK_SHUFFLE_DEKU_NUT_BAG).Get()
                       ? CAN_OBTAIN
                       : CANT_OBTAIN_NEED_UPGRADE;
        case RG_PROGRESSIVE_BOMB_BAG:
            return infiniteUpgrades != RO_INF_UPGRADES_OFF
                       ? (Flags_GetRandomizerInf(RAND_INF_HAS_INFINITE_BOMB_BAG) ? CANT_OBTAIN_ALREADY_HAVE
                                                                                 : CAN_OBTAIN)
                       : (CUR_UPG_VALUE(UPG_BOMB_BAG) < 3 ? CAN_OBTAIN : CANT_OBTAIN_ALREADY_HAVE);
        case RG_BOMBS_5:
        case RG_BOMBS_10:
        case RG_BOMBS_20:
        case RG_BUY_BOMBS_525:
        case RG_BUY_BOMBS_535:
        case RG_BUY_BOMBS_10:
        case RG_BUY_BOMBS_20:
        case RG_BUY_BOMBS_30:
            return CUR_UPG_VALUE(UPG_BOMB_BAG) ? CAN_OBTAIN : CANT_OBTAIN_NEED_UPGRADE;
        case RG_PROGRESSIVE_BOW:
            return infiniteUpgrades != RO_INF_UPGRADES_OFF
                       ? (Flags_GetRandomizerInf(RAND_INF_HAS_INFINITE_QUIVER) ? CANT_OBTAIN_ALREADY_HAVE : CAN_OBTAIN)
                       : (CUR_UPG_VALUE(UPG_QUIVER) < 3 ? CAN_OBTAIN : CANT_OBTAIN_ALREADY_HAVE);
        case RG_ARROWS_5:
        case RG_ARROWS_10:
        case RG_ARROWS_30:
        case RG_BUY_ARROWS_10:
        case RG_BUY_ARROWS_30:
        case RG_BUY_ARROWS_50:
            return CUR_UPG_VALUE(UPG_QUIVER) ? CAN_OBTAIN : CANT_OBTAIN_NEED_UPGRADE;
        case RG_PROGRESSIVE_SLINGSHOT:
            return infiniteUpgrades != RO_INF_UPGRADES_OFF
                       ? (Flags_GetRandomizerInf(RAND_INF_HAS_INFINITE_BULLET_BAG) ? CANT_OBTAIN_ALREADY_HAVE
                                                                                   : CAN_OBTAIN)
                       : (CUR_UPG_VALUE(UPG_BULLET_BAG) < 3 ? CAN_OBTAIN : CANT_OBTAIN_ALREADY_HAVE);
        case RG_DEKU_SEEDS_30:
        case RG_BUY_DEKU_SEEDS_30:
            return CUR_UPG_VALUE(UPG_BULLET_BAG) ? CAN_OBTAIN : CANT_OBTAIN_NEED_UPGRADE;
        case RG_PROGRESSIVE_OCARINA:
            switch (INV_CONTENT(ITEM_OCARINA_FAIRY)) {
                case ITEM_NONE:
                case ITEM_OCARINA_FAIRY:
                    return CAN_OBTAIN;
                case ITEM_OCARINA_TIME:
                default:
                    return CANT_OBTAIN_ALREADY_HAVE;
            }
        case RG_BOMBCHU_5:
        case RG_BOMBCHU_10:
        case RG_BOMBCHU_20:
        case RG_BUY_BOMBCHUS_10:
        case RG_BUY_BOMBCHUS_20:
            return OTRGlobals::Instance->gRandoContext->GetOption(RSK_BOMBCHU_BAG).Is(RO_BOMBCHU_BAG_NONE)
                       ? CAN_OBTAIN
                       : (INV_CONTENT(ITEM_BOMBCHU) != ITEM_NONE ? CAN_OBTAIN : CANT_OBTAIN_NEED_UPGRADE);
        case RG_PROGRESSIVE_BOMBCHU_BAG: // RANDOTODO Do we want bombchu refills to exist separately from bombchu bags?
                                         // If so, this needs changing.
            switch (OTRGlobals::Instance->gRandoContext->GetOption(RSK_BOMBCHU_BAG).Get()) {
                case RO_BOMBCHU_BAG_NONE:
                    return CANT_OBTAIN_MISC;
                case RO_BOMBCHU_BAG_SINGLE:
                    return CAN_OBTAIN;
                case RO_BOMBCHU_BAG_PROGRESSIVE:
                    if (Flags_GetRandomizerInf(RAND_INF_HAS_INFINITE_BOMBCHUS)) {
                        return CANT_OBTAIN_ALREADY_HAVE;
                    } else {
                        switch (gSaveContext.ship.quest.data.randomizer.bombchuUpgradeLevel) {
                            case 0:
                            case 1:
                                return CAN_OBTAIN;
                            case 2:
                                return infiniteUpgrades == RO_INF_UPGRADES_CONDENSED_PROGRESSIVE
                                           ? CANT_OBTAIN_ALREADY_HAVE
                                           : CAN_OBTAIN;
                            case 3:
                                return infiniteUpgrades == RO_INF_UPGRADES_PROGRESSIVE ? CAN_OBTAIN
                                                                                       : CANT_OBTAIN_ALREADY_HAVE;
                        }
                    }
            }
            assert(false);
            return CAN_OBTAIN;
        case RG_PROGRESSIVE_HOOKSHOT:
            switch (INV_CONTENT(ITEM_HOOKSHOT)) {
                case ITEM_NONE:
                case ITEM_HOOKSHOT:
                    return CAN_OBTAIN;
                case ITEM_LONGSHOT:
                default:
                    return CANT_OBTAIN_ALREADY_HAVE;
            }
        case RG_BOOMERANG:
            return INV_CONTENT(ITEM_BOOMERANG) == ITEM_NONE ? CAN_OBTAIN : CANT_OBTAIN_ALREADY_HAVE;
        case RG_LENS_OF_TRUTH:
            return INV_CONTENT(ITEM_LENS) == ITEM_NONE ? CAN_OBTAIN : CANT_OBTAIN_ALREADY_HAVE;
        case RG_MAGIC_BEAN:
        case RG_MAGIC_BEAN_PACK:
            return AMMO(ITEM_BEAN) < 10 ? CAN_OBTAIN : CANT_OBTAIN_ALREADY_HAVE;
        case RG_MEGATON_HAMMER:
            return INV_CONTENT(ITEM_HAMMER) == ITEM_NONE ? CAN_OBTAIN : CANT_OBTAIN_ALREADY_HAVE;
        case RG_FIRE_ARROWS:
            return INV_CONTENT(ITEM_ARROW_FIRE) == ITEM_NONE ? CAN_OBTAIN : CANT_OBTAIN_ALREADY_HAVE;
        case RG_ICE_ARROWS:
            return INV_CONTENT(ITEM_ARROW_ICE) == ITEM_NONE ? CAN_OBTAIN : CANT_OBTAIN_ALREADY_HAVE;
        case RG_LIGHT_ARROWS:
            return INV_CONTENT(ITEM_ARROW_LIGHT) == ITEM_NONE ? CAN_OBTAIN : CANT_OBTAIN_ALREADY_HAVE;
        case RG_DINS_FIRE:
            return INV_CONTENT(ITEM_DINS_FIRE) == ITEM_NONE ? CAN_OBTAIN : CANT_OBTAIN_ALREADY_HAVE;
        case RG_FARORES_WIND:
            return INV_CONTENT(ITEM_FARORES_WIND) == ITEM_NONE ? CAN_OBTAIN : CANT_OBTAIN_ALREADY_HAVE;
        case RG_NAYRUS_LOVE:
            if (!GetRandoSettingValue(RSK_ROCS_FEATHER)) {
                return INV_CONTENT(ITEM_NAYRUS_LOVE) == ITEM_NONE ? CAN_OBTAIN : CANT_OBTAIN_ALREADY_HAVE;
            } else {
                return Flags_GetRandomizerInf(RAND_INF_OBTAINED_NAYRUS_LOVE) ? CANT_OBTAIN_ALREADY_HAVE : CAN_OBTAIN;
            }
        case RG_ROCS_FEATHER:
            return Flags_GetRandomizerInf(RAND_INF_OBTAINED_ROCS_FEATHER) ? CANT_OBTAIN_ALREADY_HAVE : CAN_OBTAIN;

        // Bottles
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
        case RG_RUTOS_LETTER:
        case RG_BOTTLE_WITH_BIG_POE:
            return Inventory_HasEmptyBottleSlot() ? CAN_OBTAIN : CANT_OBTAIN_ALREADY_HAVE;

        // Bottle Refills
        case RG_MILK:
        case RG_FISH:
        case RG_RED_POTION_REFILL:
        case RG_GREEN_POTION_REFILL:
        case RG_BLUE_POTION_REFILL:
        case RG_BUY_FISH:
        case RG_BUY_RED_POTION_30:
        case RG_BUY_GREEN_POTION:
        case RG_BUY_BLUE_POTION:
        case RG_BUY_BLUE_FIRE:
        case RG_BUY_BOTTLE_BUG:
        case RG_BUY_POE:
        case RG_BUY_FAIRYS_SPIRIT:
        case RG_BUY_RED_POTION_40:
        case RG_BUY_RED_POTION_50:
            return Inventory_HasEmptyBottle() ? CAN_OBTAIN : CANT_OBTAIN_NEED_EMPTY_BOTTLE;

        // Trade Items
        // TODO: Do we want to be strict about any of this?
        // case RG_WEIRD_EGG:
        // case RG_ZELDAS_LETTER:
        // case RG_POCKET_EGG:
        // case RG_COJIRO:
        // case RG_ODD_MUSHROOM:
        // case RG_ODD_POTION:
        // case RG_POACHERS_SAW:
        // case RG_BROKEN_SWORD:
        // case RG_PRESCRIPTION:
        // case RG_EYEBALL_FROG:
        // case RG_EYEDROPS:
        // case RG_CLAIM_CHECK:
        // case RG_PROGRESSIVE_GORONSWORD:
        // case RG_GIANTS_KNIFE:

        // Misc Items
        case RG_STONE_OF_AGONY:
            return !CHECK_QUEST_ITEM(QUEST_STONE_OF_AGONY) ? CAN_OBTAIN : CANT_OBTAIN_ALREADY_HAVE;
        case RG_GERUDO_MEMBERSHIP_CARD:
            return !CHECK_QUEST_ITEM(QUEST_GERUDO_CARD) ? CAN_OBTAIN : CANT_OBTAIN_ALREADY_HAVE;
        case RG_DOUBLE_DEFENSE:
            return !gSaveContext.isDoubleDefenseAcquired ? CAN_OBTAIN : CANT_OBTAIN_ALREADY_HAVE;
        case RG_GOLD_SKULLTULA_TOKEN:
            return gSaveContext.inventory.gsTokens < 100 ? CAN_OBTAIN : CANT_OBTAIN_ALREADY_HAVE;
        case RG_PROGRESSIVE_STRENGTH:
            return CUR_UPG_VALUE(UPG_STRENGTH) < 3 ? CAN_OBTAIN : CANT_OBTAIN_ALREADY_HAVE;
        case RG_PROGRESSIVE_WALLET:
            return CUR_UPG_VALUE(UPG_WALLET) < numWallets ? CAN_OBTAIN : CANT_OBTAIN_ALREADY_HAVE;
        case RG_PROGRESSIVE_SCALE:
            return CUR_UPG_VALUE(UPG_SCALE) < 2 ? CAN_OBTAIN : CANT_OBTAIN_ALREADY_HAVE;
        case RG_PROGRESSIVE_MAGIC_METER:
        case RG_MAGIC_SINGLE:
        case RG_MAGIC_DOUBLE:
            return infiniteUpgrades != RO_INF_UPGRADES_OFF
                       ? (Flags_GetRandomizerInf(RAND_INF_HAS_INFINITE_MAGIC_METER) ? CANT_OBTAIN_ALREADY_HAVE
                                                                                    : CAN_OBTAIN)
                       : (gSaveContext.magicLevel < 2 ? CAN_OBTAIN : CANT_OBTAIN_ALREADY_HAVE);
        case RG_FISHING_POLE:
            return !Flags_GetRandomizerInf(RAND_INF_FISHING_POLE_FOUND) ? CAN_OBTAIN : CANT_OBTAIN_ALREADY_HAVE;

        // Songs
        case RG_ZELDAS_LULLABY:
            return !CHECK_QUEST_ITEM(QUEST_SONG_LULLABY) ? CAN_OBTAIN : CANT_OBTAIN_ALREADY_HAVE;
        case RG_EPONAS_SONG:
            return !CHECK_QUEST_ITEM(QUEST_SONG_EPONA) ? CAN_OBTAIN : CANT_OBTAIN_ALREADY_HAVE;
        case RG_SARIAS_SONG:
            return !CHECK_QUEST_ITEM(QUEST_SONG_SARIA) ? CAN_OBTAIN : CANT_OBTAIN_ALREADY_HAVE;
        case RG_SUNS_SONG:
            return !CHECK_QUEST_ITEM(QUEST_SONG_SUN) ? CAN_OBTAIN : CANT_OBTAIN_ALREADY_HAVE;
        case RG_SONG_OF_TIME:
            return !CHECK_QUEST_ITEM(QUEST_SONG_TIME) ? CAN_OBTAIN : CANT_OBTAIN_ALREADY_HAVE;
        case RG_SONG_OF_STORMS:
            return !CHECK_QUEST_ITEM(QUEST_SONG_STORMS) ? CAN_OBTAIN : CANT_OBTAIN_ALREADY_HAVE;
        case RG_MINUET_OF_FOREST:
            return !CHECK_QUEST_ITEM(QUEST_SONG_MINUET) ? CAN_OBTAIN : CANT_OBTAIN_ALREADY_HAVE;
        case RG_BOLERO_OF_FIRE:
            return !CHECK_QUEST_ITEM(QUEST_SONG_BOLERO) ? CAN_OBTAIN : CANT_OBTAIN_ALREADY_HAVE;
        case RG_SERENADE_OF_WATER:
            return !CHECK_QUEST_ITEM(QUEST_SONG_SERENADE) ? CAN_OBTAIN : CANT_OBTAIN_ALREADY_HAVE;
        case RG_REQUIEM_OF_SPIRIT:
            return !CHECK_QUEST_ITEM(QUEST_SONG_REQUIEM) ? CAN_OBTAIN : CANT_OBTAIN_ALREADY_HAVE;
        case RG_NOCTURNE_OF_SHADOW:
            return !CHECK_QUEST_ITEM(QUEST_SONG_NOCTURNE) ? CAN_OBTAIN : CANT_OBTAIN_ALREADY_HAVE;
        case RG_PRELUDE_OF_LIGHT:
            return !CHECK_QUEST_ITEM(QUEST_SONG_PRELUDE) ? CAN_OBTAIN : CANT_OBTAIN_ALREADY_HAVE;

        // Dungeon Items
        case RG_DEKU_TREE_MAP:
            return !CHECK_DUNGEON_ITEM(DUNGEON_MAP, SCENE_DEKU_TREE) ? CAN_OBTAIN : CANT_OBTAIN_ALREADY_HAVE;
        case RG_DODONGOS_CAVERN_MAP:
            return !CHECK_DUNGEON_ITEM(DUNGEON_MAP, SCENE_DODONGOS_CAVERN) ? CAN_OBTAIN : CANT_OBTAIN_ALREADY_HAVE;
        case RG_JABU_JABUS_BELLY_MAP:
            return !CHECK_DUNGEON_ITEM(DUNGEON_MAP, SCENE_JABU_JABU) ? CAN_OBTAIN : CANT_OBTAIN_ALREADY_HAVE;
        case RG_FOREST_TEMPLE_MAP:
            return !CHECK_DUNGEON_ITEM(DUNGEON_MAP, SCENE_FOREST_TEMPLE) ? CAN_OBTAIN : CANT_OBTAIN_ALREADY_HAVE;
        case RG_FIRE_TEMPLE_MAP:
            return !CHECK_DUNGEON_ITEM(DUNGEON_MAP, SCENE_FIRE_TEMPLE) ? CAN_OBTAIN : CANT_OBTAIN_ALREADY_HAVE;
        case RG_WATER_TEMPLE_MAP:
            return !CHECK_DUNGEON_ITEM(DUNGEON_MAP, SCENE_WATER_TEMPLE) ? CAN_OBTAIN : CANT_OBTAIN_ALREADY_HAVE;
        case RG_SPIRIT_TEMPLE_MAP:
            return !CHECK_DUNGEON_ITEM(DUNGEON_MAP, SCENE_SPIRIT_TEMPLE) ? CAN_OBTAIN : CANT_OBTAIN_ALREADY_HAVE;
        case RG_SHADOW_TEMPLE_MAP:
            return !CHECK_DUNGEON_ITEM(DUNGEON_MAP, SCENE_SHADOW_TEMPLE) ? CAN_OBTAIN : CANT_OBTAIN_ALREADY_HAVE;
        case RG_BOTTOM_OF_THE_WELL_MAP:
            return !CHECK_DUNGEON_ITEM(DUNGEON_MAP, SCENE_BOTTOM_OF_THE_WELL) ? CAN_OBTAIN : CANT_OBTAIN_ALREADY_HAVE;
        case RG_ICE_CAVERN_MAP:
            return !CHECK_DUNGEON_ITEM(DUNGEON_MAP, SCENE_ICE_CAVERN) ? CAN_OBTAIN : CANT_OBTAIN_ALREADY_HAVE;
        case RG_DEKU_TREE_COMPASS:
            return !CHECK_DUNGEON_ITEM(DUNGEON_COMPASS, SCENE_DEKU_TREE) ? CAN_OBTAIN : CANT_OBTAIN_ALREADY_HAVE;
        case RG_DODONGOS_CAVERN_COMPASS:
            return !CHECK_DUNGEON_ITEM(DUNGEON_COMPASS, SCENE_DODONGOS_CAVERN) ? CAN_OBTAIN : CANT_OBTAIN_ALREADY_HAVE;
        case RG_JABU_JABUS_BELLY_COMPASS:
            return !CHECK_DUNGEON_ITEM(DUNGEON_COMPASS, SCENE_JABU_JABU) ? CAN_OBTAIN : CANT_OBTAIN_ALREADY_HAVE;
        case RG_FOREST_TEMPLE_COMPASS:
            return !CHECK_DUNGEON_ITEM(DUNGEON_COMPASS, SCENE_FOREST_TEMPLE) ? CAN_OBTAIN : CANT_OBTAIN_ALREADY_HAVE;
        case RG_FIRE_TEMPLE_COMPASS:
            return !CHECK_DUNGEON_ITEM(DUNGEON_COMPASS, SCENE_FIRE_TEMPLE) ? CAN_OBTAIN : CANT_OBTAIN_ALREADY_HAVE;
        case RG_WATER_TEMPLE_COMPASS:
            return !CHECK_DUNGEON_ITEM(DUNGEON_COMPASS, SCENE_WATER_TEMPLE) ? CAN_OBTAIN : CANT_OBTAIN_ALREADY_HAVE;
        case RG_SPIRIT_TEMPLE_COMPASS:
            return !CHECK_DUNGEON_ITEM(DUNGEON_COMPASS, SCENE_SPIRIT_TEMPLE) ? CAN_OBTAIN : CANT_OBTAIN_ALREADY_HAVE;
        case RG_SHADOW_TEMPLE_COMPASS:
            return !CHECK_DUNGEON_ITEM(DUNGEON_COMPASS, SCENE_SHADOW_TEMPLE) ? CAN_OBTAIN : CANT_OBTAIN_ALREADY_HAVE;
        case RG_BOTTOM_OF_THE_WELL_COMPASS:
            return !CHECK_DUNGEON_ITEM(DUNGEON_COMPASS, SCENE_BOTTOM_OF_THE_WELL) ? CAN_OBTAIN
                                                                                  : CANT_OBTAIN_ALREADY_HAVE;
        case RG_ICE_CAVERN_COMPASS:
            return !CHECK_DUNGEON_ITEM(DUNGEON_COMPASS, SCENE_ICE_CAVERN) ? CAN_OBTAIN : CANT_OBTAIN_ALREADY_HAVE;
        case RG_FOREST_TEMPLE_BOSS_KEY:
            return !CHECK_DUNGEON_ITEM(DUNGEON_KEY_BOSS, SCENE_FOREST_TEMPLE) ? CAN_OBTAIN : CANT_OBTAIN_ALREADY_HAVE;
        case RG_FIRE_TEMPLE_BOSS_KEY:
            return !CHECK_DUNGEON_ITEM(DUNGEON_KEY_BOSS, SCENE_FIRE_TEMPLE) ? CAN_OBTAIN : CANT_OBTAIN_ALREADY_HAVE;
        case RG_WATER_TEMPLE_BOSS_KEY:
            return !CHECK_DUNGEON_ITEM(DUNGEON_KEY_BOSS, SCENE_WATER_TEMPLE) ? CAN_OBTAIN : CANT_OBTAIN_ALREADY_HAVE;
        case RG_SPIRIT_TEMPLE_BOSS_KEY:
            return !CHECK_DUNGEON_ITEM(DUNGEON_KEY_BOSS, SCENE_SPIRIT_TEMPLE) ? CAN_OBTAIN : CANT_OBTAIN_ALREADY_HAVE;
        case RG_SHADOW_TEMPLE_BOSS_KEY:
            return !CHECK_DUNGEON_ITEM(DUNGEON_KEY_BOSS, SCENE_SHADOW_TEMPLE) ? CAN_OBTAIN : CANT_OBTAIN_ALREADY_HAVE;
        case RG_GANONS_CASTLE_BOSS_KEY:
            return !CHECK_DUNGEON_ITEM(DUNGEON_KEY_BOSS, SCENE_GANONS_TOWER) ? CAN_OBTAIN : CANT_OBTAIN_ALREADY_HAVE;
        case RG_FOREST_TEMPLE_SMALL_KEY:
            return gSaveContext.inventory.dungeonKeys[SCENE_FOREST_TEMPLE] < FOREST_TEMPLE_SMALL_KEY_MAX
                       ? CAN_OBTAIN
                       : CANT_OBTAIN_ALREADY_HAVE;
        case RG_FIRE_TEMPLE_SMALL_KEY:
            return gSaveContext.inventory.dungeonKeys[SCENE_FIRE_TEMPLE] < FIRE_TEMPLE_SMALL_KEY_MAX
                       ? CAN_OBTAIN
                       : CANT_OBTAIN_ALREADY_HAVE;
        case RG_WATER_TEMPLE_SMALL_KEY:
            return gSaveContext.inventory.dungeonKeys[SCENE_WATER_TEMPLE] < WATER_TEMPLE_SMALL_KEY_MAX
                       ? CAN_OBTAIN
                       : CANT_OBTAIN_ALREADY_HAVE;
        case RG_SPIRIT_TEMPLE_SMALL_KEY:
            return gSaveContext.inventory.dungeonKeys[SCENE_SPIRIT_TEMPLE] < SPIRIT_TEMPLE_SMALL_KEY_MAX
                       ? CAN_OBTAIN
                       : CANT_OBTAIN_ALREADY_HAVE;
        case RG_SHADOW_TEMPLE_SMALL_KEY:
            return gSaveContext.inventory.dungeonKeys[SCENE_SHADOW_TEMPLE] < SHADOW_TEMPLE_SMALL_KEY_MAX
                       ? CAN_OBTAIN
                       : CANT_OBTAIN_ALREADY_HAVE;
        case RG_BOTTOM_OF_THE_WELL_SMALL_KEY:
            return gSaveContext.inventory.dungeonKeys[SCENE_BOTTOM_OF_THE_WELL] < BOTTOM_OF_THE_WELL_SMALL_KEY_MAX
                       ? CAN_OBTAIN
                       : CANT_OBTAIN_ALREADY_HAVE;
        case RG_GERUDO_TRAINING_GROUND_SMALL_KEY:
            return gSaveContext.inventory.dungeonKeys[SCENE_GERUDO_TRAINING_GROUND] <
                           GERUDO_TRAINING_GROUND_SMALL_KEY_MAX
                       ? CAN_OBTAIN
                       : CANT_OBTAIN_ALREADY_HAVE;
        case RG_GERUDO_FORTRESS_SMALL_KEY:
            return gSaveContext.inventory.dungeonKeys[SCENE_THIEVES_HIDEOUT] < GERUDO_FORTRESS_SMALL_KEY_MAX
                       ? CAN_OBTAIN
                       : CANT_OBTAIN_ALREADY_HAVE;
        case RG_GANONS_CASTLE_SMALL_KEY:
            return gSaveContext.inventory.dungeonKeys[SCENE_INSIDE_GANONS_CASTLE] < GANONS_CASTLE_SMALL_KEY_MAX
                       ? CAN_OBTAIN
                       : CANT_OBTAIN_ALREADY_HAVE;
        case RG_TREASURE_GAME_SMALL_KEY:
            return gSaveContext.inventory.dungeonKeys[SCENE_TREASURE_BOX_SHOP] < TREASURE_GAME_SMALL_KEY_MAX
                       ? CAN_OBTAIN
                       : CANT_OBTAIN_ALREADY_HAVE;

        // Dungeon Rewards
        case RG_KOKIRI_EMERALD:
            return !CHECK_QUEST_ITEM(QUEST_KOKIRI_EMERALD) ? CAN_OBTAIN : CANT_OBTAIN_ALREADY_HAVE;
        case RG_GORON_RUBY:
            return !CHECK_QUEST_ITEM(QUEST_GORON_RUBY) ? CAN_OBTAIN : CANT_OBTAIN_ALREADY_HAVE;
        case RG_ZORA_SAPPHIRE:
            return !CHECK_QUEST_ITEM(QUEST_ZORA_SAPPHIRE) ? CAN_OBTAIN : CANT_OBTAIN_ALREADY_HAVE;
        case RG_FOREST_MEDALLION:
            return !CHECK_QUEST_ITEM(QUEST_MEDALLION_FOREST) ? CAN_OBTAIN : CANT_OBTAIN_ALREADY_HAVE;
        case RG_FIRE_MEDALLION:
            return !CHECK_QUEST_ITEM(QUEST_MEDALLION_FIRE) ? CAN_OBTAIN : CANT_OBTAIN_ALREADY_HAVE;
        case RG_WATER_MEDALLION:
            return !CHECK_QUEST_ITEM(QUEST_MEDALLION_WATER) ? CAN_OBTAIN : CANT_OBTAIN_ALREADY_HAVE;
        case RG_SPIRIT_MEDALLION:
            return !CHECK_QUEST_ITEM(QUEST_MEDALLION_SPIRIT) ? CAN_OBTAIN : CANT_OBTAIN_ALREADY_HAVE;
        case RG_SHADOW_MEDALLION:
            return !CHECK_QUEST_ITEM(QUEST_MEDALLION_SHADOW) ? CAN_OBTAIN : CANT_OBTAIN_ALREADY_HAVE;
        case RG_LIGHT_MEDALLION:
            return !CHECK_QUEST_ITEM(QUEST_MEDALLION_LIGHT) ? CAN_OBTAIN : CANT_OBTAIN_ALREADY_HAVE;

        case RG_RECOVERY_HEART:
        case RG_GREEN_RUPEE:
        case RG_GREG_RUPEE:
        case RG_BLUE_RUPEE:
        case RG_RED_RUPEE:
        case RG_PURPLE_RUPEE:
        case RG_HUGE_RUPEE:
        case RG_PIECE_OF_HEART:
        case RG_HEART_CONTAINER:
        case RG_ICE_TRAP:
        case RG_TREASURE_GAME_HEART:
        case RG_TREASURE_GAME_GREEN_RUPEE:
        case RG_BUY_HEART:
        case RG_TRIFORCE_PIECE:
        default:
            return CAN_OBTAIN;
    }
}

Rando::Location* Randomizer::GetCheckObjectFromActor(s16 actorId, s16 sceneNum, s32 actorParams = 0x00) {
    RandomizerCheck specialRc = RC_UNKNOWN_CHECK;
    // TODO: Migrate these special cases into table, or at least document why they are special
    switch (sceneNum) {
        case SCENE_TREASURE_BOX_SHOP: {
            if ((actorId == ACTOR_EN_BOX && actorParams == 20170) ||
                (actorId == ACTOR_ITEM_ETCETERA && actorParams == 2572)) {
                specialRc = RC_MARKET_TREASURE_CHEST_GAME_REWARD;
            }

            // todo: handle the itemetc part of this so drawing works when we implement shuffle
            if (actorId == ACTOR_EN_BOX) {
                bool isAKey = (actorParams & 0x60) == 0x20;
                if ((actorParams & 0xF) < 2) {
                    specialRc = isAKey ? RC_MARKET_TREASURE_CHEST_GAME_KEY_1 : RC_MARKET_TREASURE_CHEST_GAME_ITEM_1;
                } else if ((actorParams & 0xF) < 4) {
                    specialRc = isAKey ? RC_MARKET_TREASURE_CHEST_GAME_KEY_2 : RC_MARKET_TREASURE_CHEST_GAME_ITEM_2;
                } else if ((actorParams & 0xF) < 6) {
                    specialRc = isAKey ? RC_MARKET_TREASURE_CHEST_GAME_KEY_3 : RC_MARKET_TREASURE_CHEST_GAME_ITEM_3;
                } else if ((actorParams & 0xF) < 8) {
                    specialRc = isAKey ? RC_MARKET_TREASURE_CHEST_GAME_KEY_4 : RC_MARKET_TREASURE_CHEST_GAME_ITEM_4;
                } else if ((actorParams & 0xF) < 10) {
                    specialRc = isAKey ? RC_MARKET_TREASURE_CHEST_GAME_KEY_5 : RC_MARKET_TREASURE_CHEST_GAME_ITEM_5;
                }
            }
            break;
        }
        case SCENE_SACRED_FOREST_MEADOW:
            if (actorId == ACTOR_EN_SA) {
                specialRc = RC_SONG_FROM_SARIA;
            }
            break;
        case SCENE_TEMPLE_OF_TIME_EXTERIOR_DAY:
        case SCENE_TEMPLE_OF_TIME_EXTERIOR_NIGHT:
        case SCENE_TEMPLE_OF_TIME_EXTERIOR_RUINS:
            switch (actorParams) {
                case 14342:
                    specialRc = RC_TOT_LEFTMOST_GOSSIP_STONE;
                    break;
                case 14599:
                    specialRc = RC_TOT_LEFT_CENTER_GOSSIP_STONE;
                    break;
                case 14862:
                    specialRc = RC_TOT_RIGHT_CENTER_GOSSIP_STONE;
                    break;
                case 15120:
                    specialRc = RC_TOT_RIGHTMOST_GOSSIP_STONE;
                    break;
            }
            break;
        case SCENE_HOUSE_OF_SKULLTULA:
            if (actorId == ACTOR_EN_SSH) {
                switch (actorParams) { // actor params are used to differentiate between textboxes
                    case 1:
                        specialRc = RC_KAK_10_GOLD_SKULLTULA_REWARD;
                        break;
                    case 2:
                        specialRc = RC_KAK_20_GOLD_SKULLTULA_REWARD;
                        break;
                    case 3:
                        specialRc = RC_KAK_30_GOLD_SKULLTULA_REWARD;
                        break;
                    case 4:
                        specialRc = RC_KAK_40_GOLD_SKULLTULA_REWARD;
                        break;
                    case 5:
                        specialRc = RC_KAK_50_GOLD_SKULLTULA_REWARD;
                        break;
                }
            }
            break;
        case SCENE_KAKARIKO_VILLAGE:
            switch (actorId) {
                case ACTOR_EN_NIW_LADY:
                    if (LINK_IS_ADULT) {
                        specialRc = RC_KAK_ANJU_AS_ADULT;
                    } else {
                        specialRc = RC_KAK_ANJU_AS_CHILD;
                    }
            }
            break;
        case SCENE_LAKE_HYLIA:
            switch (actorId) {
                case ACTOR_ITEM_ETCETERA:
                    if (LINK_IS_ADULT) {
                        specialRc = RC_LH_SUN;
                    } else {
                        specialRc = RC_LH_UNDERWATER_ITEM;
                    }
            }
            break;
        case SCENE_ZORAS_FOUNTAIN:
            switch (actorParams) {
                case 15362:
                case 14594:
                    specialRc = RC_ZF_JABU_GOSSIP_STONE;
                    break;
                case 14849:
                case 14337:
                    specialRc = RC_ZF_FAIRY_GOSSIP_STONE;
                    break;
            }
            break;
        case SCENE_GERUDOS_FORTRESS:
            // GF chest as child has different params and gives odd mushroom
            // set it to the GF chest check for both ages
            if (actorId == ACTOR_EN_BOX) {
                specialRc = RC_GF_CHEST;
            }
            break;
        case SCENE_DODONGOS_CAVERN:
            // special case for MQ DC Gossip Stone
            if (actorId == ACTOR_EN_GS && actorParams == 15892 && ResourceMgr_IsGameMasterQuest()) {
                specialRc = RC_DODONGOS_CAVERN_GOSSIP_STONE;
            }
            break;
        case SCENE_SHOOTING_GALLERY:
            // special case for shooting gallery sign
            if (actorId == ACTOR_EN_KANBAN) {
                if (LINK_IS_ADULT) {
                    specialRc = RC_KAK_SHOOTING_GALLERY_RECTANGLE_SIGN;
                } else {
                    specialRc = RC_MK_SHOOTING_GALLERY_RECTANGLE_SIGN;
                }
            }
            break;
    }

    if (specialRc != RC_UNKNOWN_CHECK) {
        return Rando::StaticData::GetLocation(specialRc);
    }

    auto range = Rando::StaticData::CheckFromActorMultimap.equal_range(std::make_tuple(actorId, sceneNum, actorParams));

    for (auto it = range.first; it != range.second; ++it) {
        if (Rando::StaticData::GetLocation(it->second)->GetQuest() == RCQUEST_BOTH ||
            (Rando::StaticData::GetLocation(it->second)->GetQuest() == RCQUEST_VANILLA &&
             !ResourceMgr_IsGameMasterQuest()) ||
            (Rando::StaticData::GetLocation(it->second)->GetQuest() == RCQUEST_MQ && ResourceMgr_IsGameMasterQuest())) {
            return Rando::StaticData::GetLocation(it->second);
        }
    }

    return Rando::StaticData::GetLocation(RC_UNKNOWN_CHECK);
}

// RANDOTODO: Move all Shopsanity stuff to a ShuffleShops.cpp
ShopItemIdentity Randomizer::IdentifyShopItem(s32 sceneNum, u8 slotIndex) {
    ShopItemIdentity shopItemIdentity;

    shopItemIdentity.identity.randomizerInf = RAND_INF_MAX;
    shopItemIdentity.identity.randomizerCheck = RC_UNKNOWN_CHECK;
    shopItemIdentity.ogItemId = GI_NONE;
    shopItemIdentity.itemPrice = -1;
    shopItemIdentity.enGirlAShopItem = 0x32;

    if (slotIndex == 0) {
        return shopItemIdentity;
    }

    Rando::Location* location = GetCheckObjectFromActor(
        ACTOR_EN_GIRLA,
        // Bazaar (SHOP1) scene is reused, so if entering from Kak use debug scene to identify
        (sceneNum == SCENE_BAZAAR && gSaveContext.entranceIndex == ENTR_BAZAAR_0) ? SCENE_TEST01 : sceneNum,
        slotIndex - 1);

    if (location->GetRandomizerCheck() != RC_UNKNOWN_CHECK) {
        shopItemIdentity.identity.randomizerInf = rcToRandomizerInf[location->GetRandomizerCheck()];
        shopItemIdentity.identity.randomizerCheck = location->GetRandomizerCheck();
        shopItemIdentity.ogItemId = (GetItemID)Rando::StaticData::RetrieveItem(location->GetVanillaItem()).GetItemID();

        RandomizerGet randoGet = Rando::Context::GetInstance()
                                     ->GetItemLocation(shopItemIdentity.identity.randomizerCheck)
                                     ->GetPlacedRandomizerGet();
        if (randomizerGetToEnGirlShopItem.find(randoGet) != randomizerGetToEnGirlShopItem.end()) {
            shopItemIdentity.enGirlAShopItem = randomizerGetToEnGirlShopItem[randoGet];
        }

        shopItemIdentity.itemPrice =
            OTRGlobals::Instance->gRandoContext->GetItemLocation(shopItemIdentity.identity.randomizerCheck)->GetPrice();
    }

    return shopItemIdentity;
}

u8 Randomizer::GetRandoSettingValue(RandomizerSettingKey randoSettingKey) {
    return Rando::Context::GetInstance()->GetOption(randoSettingKey).Get();
}

GetItemEntry Randomizer::GetItemFromKnownCheck(RandomizerCheck randomizerCheck, GetItemID ogItemId,
                                               bool checkObtainability) {
    return Rando::Context::GetInstance()->GetFinalGIEntry(randomizerCheck, checkObtainability);
}

RandomizerCheck Randomizer::GetCheckFromActor(s16 actorId, s16 sceneNum, s16 actorParams) {
    return GetCheckObjectFromActor(actorId, sceneNum, actorParams)->GetRandomizerCheck();
}

RandomizerInf Randomizer::GetRandomizerInfFromCheck(RandomizerCheck rc) {
    auto rcIt = rcToRandomizerInf.find(rc);
    if (rcIt == rcToRandomizerInf.end())
        return RAND_INF_MAX;

    return rcIt->second;
}

RandomizerCheck Randomizer::GetCheckFromRandomizerInf(RandomizerInf randomizerInf) {
    for (auto const& [key, value] : rcToRandomizerInf) {
        if (value == randomizerInf)
            return key;
    }

    return RC_UNKNOWN_CHECK;
}

std::thread randoThread;

void GenerateRandomizerImgui(std::string seed = "") {
    CVarSetInteger(CVAR_GENERAL("RandoGenerating"), 1);
    Ship::Context::GetRawInstance()->GetWindow()->GetGui()->SaveConsoleVariablesNextFrame();
    auto ctx = Rando::Context::GetInstance();
    // RANDOTODO proper UI for selecting if a spoiler loaded should be used for settings
    Rando::Settings::GetInstance()->SetAllToContext();

    // todo: this efficiently when we build out cvar array support
    std::set<RandomizerCheck> excludedLocations;
    std::stringstream excludedLocationStringStream(CVarGetString(CVAR_RANDOMIZER_SETTING("ExcludedLocations"), ""));
    std::string excludedLocationString;
    while (getline(excludedLocationStringStream, excludedLocationString, ',')) {
        excludedLocations.insert((RandomizerCheck)std::stoi(excludedLocationString));
    }

    // todo: better way to sort out linking tricks rather than name

    std::set<RandomizerTrick> enabledTricks;
    std::stringstream enabledTrickStringStream(CVarGetString(CVAR_RANDOMIZER_SETTING("EnabledTricks"), ""));
    std::string enabledTrickString;
    while (getline(enabledTrickStringStream, enabledTrickString, ',')) {
        if (Rando::StaticData::trickToEnum.contains(enabledTrickString)) {
            enabledTricks.insert(Rando::StaticData::trickToEnum[enabledTrickString]);
        }
    }

    // Update the visibilitiy before removing conflicting excludes (in case the locations tab wasn't viewed)
    RandomizerCheckObjects::UpdateImGuiVisibility();

    // Remove excludes for locations that are no longer allowed to be excluded
    for (auto& location : Rando::StaticData::GetLocationTable()) {
        auto elfound = excludedLocations.find(location.GetRandomizerCheck());
        if (!ctx->GetItemLocation(location.GetRandomizerCheck())->IsVisible() && elfound != excludedLocations.end()) {
            excludedLocations.erase(elfound);
        }
    }

    Rando::Context::GetInstance()->SetSeedGenerated(GenerateRandomizer(excludedLocations, enabledTricks, seed));
    CVarSetInteger(CVAR_GENERAL("RandoGenerating"), 0);
    Ship::Context::GetRawInstance()->GetWindow()->GetGui()->SaveConsoleVariablesNextFrame();

    generated = true;

    GameInteractor::Instance->ExecuteHooks<GameInteractor::OnGenerationCompletion>();
}

bool GenerateRandomizer(std::string seed /*= ""*/) {
    if (generated) {
        generated = false;
        randoThread.join();
    }
    if (CVarGetInteger(CVAR_GENERAL("RandoGenerating"), 0) == 0) {
        randoThread = std::thread(&GenerateRandomizerImgui, seed);
        return true;
    }
    return false;
}

static bool locationsTabOpen = false;
static bool tricksTabOpen = false;

void JoinRandoGenerationThread() {
    if (generated) {
        generated = false;
        randoThread.join();
    }
}

class ExtendedVanillaTableInvalidItemIdException : public std::exception {
  private:
    s16 itemID;

  public:
    ExtendedVanillaTableInvalidItemIdException(s16 itemID) : itemID(itemID) {
    }
    std::string what() {
        return itemID + " is not a valid ItemID for the extendedVanillaGetItemTable. If you are adding a new"
                        "item, try adding it to randoGetItemTable instead.";
    }
};

static std::unordered_map<RandomizerGet, GameplayStatTimestamp> randomizerGetToStatsTimeStamp = {
    { RG_GOHMA_SOUL, TIMESTAMP_FOUND_GOHMA_SOUL },
    { RG_KING_DODONGO_SOUL, TIMESTAMP_FOUND_KING_DODONGO_SOUL },
    { RG_BARINADE_SOUL, TIMESTAMP_FOUND_BARINADE_SOUL },
    { RG_PHANTOM_GANON_SOUL, TIMESTAMP_FOUND_PHANTOM_GANON_SOUL },
    { RG_VOLVAGIA_SOUL, TIMESTAMP_FOUND_VOLVAGIA_SOUL },
    { RG_MORPHA_SOUL, TIMESTAMP_FOUND_MORPHA_SOUL },
    { RG_BONGO_BONGO_SOUL, TIMESTAMP_FOUND_BONGO_BONGO_SOUL },
    { RG_TWINROVA_SOUL, TIMESTAMP_FOUND_TWINROVA_SOUL },
    { RG_GANON_SOUL, TIMESTAMP_FOUND_GANON_SOUL },

    { RG_BRONZE_SCALE, TIMESTAMP_FOUND_BRONZE_SCALE },

    { RG_OCARINA_A_BUTTON, TIMESTAMP_FOUND_OCARINA_A_BUTTON },
    { RG_OCARINA_C_UP_BUTTON, TIMESTAMP_FOUND_OCARINA_C_UP_BUTTON },
    { RG_OCARINA_C_DOWN_BUTTON, TIMESTAMP_FOUND_OCARINA_C_DOWN_BUTTON },
    { RG_OCARINA_C_LEFT_BUTTON, TIMESTAMP_FOUND_OCARINA_C_LEFT_BUTTON },
    { RG_OCARINA_C_RIGHT_BUTTON, TIMESTAMP_FOUND_OCARINA_C_RIGHT_BUTTON },

    { RG_FISHING_POLE, TIMESTAMP_FOUND_FISHING_POLE },

    { RG_GUARD_HOUSE_KEY, TIMESTAMP_FOUND_GUARD_HOUSE_KEY },
    { RG_MARKET_BAZAAR_KEY, TIMESTAMP_FOUND_MARKET_BAZAAR_KEY },
    { RG_MARKET_POTION_SHOP_KEY, TIMESTAMP_FOUND_MARKET_POTION_SHOP_KEY },
    { RG_MASK_SHOP_KEY, TIMESTAMP_FOUND_MASK_SHOP_KEY },
    { RG_MARKET_SHOOTING_GALLERY_KEY, TIMESTAMP_FOUND_MARKET_SHOOTING_GALLERY_KEY },
    { RG_BOMBCHU_BOWLING_KEY, TIMESTAMP_FOUND_BOMBCHU_BOWLING_KEY },
    { RG_TREASURE_CHEST_GAME_BUILDING_KEY, TIMESTAMP_FOUND_TREASURE_CHEST_GAME_BUILDING_KEY },
    { RG_BOMBCHU_SHOP_KEY, TIMESTAMP_FOUND_BOMBCHU_SHOP_KEY },
    { RG_RICHARDS_HOUSE_KEY, TIMESTAMP_FOUND_RICHARDS_HOUSE_KEY },
    { RG_ALLEY_HOUSE_KEY, TIMESTAMP_FOUND_ALLEY_HOUSE_KEY },
    { RG_KAK_BAZAAR_KEY, TIMESTAMP_FOUND_KAK_BAZAAR_KEY },
    { RG_KAK_POTION_SHOP_KEY, TIMESTAMP_FOUND_KAK_POTION_SHOP_KEY },
    { RG_BOSS_HOUSE_KEY, TIMESTAMP_FOUND_BOSS_HOUSE_KEY },
    { RG_GRANNYS_POTION_SHOP_KEY, TIMESTAMP_FOUND_GRANNYS_POTION_SHOP_KEY },
    { RG_SKULLTULA_HOUSE_KEY, TIMESTAMP_FOUND_SKULLTULA_HOUSE_KEY },
    { RG_IMPAS_HOUSE_KEY, TIMESTAMP_FOUND_IMPAS_HOUSE_KEY },
    { RG_WINDMILL_KEY, TIMESTAMP_FOUND_WINDMILL_KEY },
    { RG_KAK_SHOOTING_GALLERY_KEY, TIMESTAMP_FOUND_KAK_SHOOTING_GALLERY_KEY },
    { RG_DAMPES_HUT_KEY, TIMESTAMP_FOUND_DAMPES_HUT_KEY },
    { RG_TALONS_HOUSE_KEY, TIMESTAMP_FOUND_TALONS_HOUSE_KEY },
    { RG_STABLES_KEY, TIMESTAMP_FOUND_STABLES_KEY },
    { RG_BACK_TOWER_KEY, TIMESTAMP_FOUND_BACK_TOWER_KEY },
    { RG_HYLIA_LAB_KEY, TIMESTAMP_FOUND_HYLIA_LAB_KEY },
    { RG_FISHING_HOLE_KEY, TIMESTAMP_FOUND_FISHING_HOLE_KEY },
};

// Gameplay stat tracking: Update time the item was acquired
// (special cases for rando items)
void Randomizer_GameplayStats_SetTimestamp(uint16_t item) {
    u32 time = static_cast<u32>(GAMEPLAYSTAT_TOTAL_TIME);
    // Have items in Link's pocket shown as being obtained at 0.1 seconds
    if (time == 0) {
        time = 1;
    }

    if (gSaveContext.ship.stats.itemTimestamp[item] == 0) {
        if (item == RG_GANONS_CASTLE_BOSS_KEY) {
            gSaveContext.ship.stats.itemTimestamp[ITEM_KEY_BOSS] = time;
        } else if (item == RG_MASTER_SWORD) {
            gSaveContext.ship.stats.itemTimestamp[ITEM_SWORD_MASTER] = time;
        } else if (randomizerGetToStatsTimeStamp.contains((RandomizerGet)item)) {
            gSaveContext.ship.stats.itemTimestamp[randomizerGetToStatsTimeStamp[(RandomizerGet)item]] = time;
        } else if (item >= RG_EMPTY_BOTTLE && item <= RG_BOTTLE_WITH_BIG_POE) {
            gSaveContext.ship.stats.itemTimestamp[ITEM_BOTTLE] = time;
        } else if ((item >= RG_BOMBCHU_5 && item <= RG_BOMBCHU_20) || item == RG_PROGRESSIVE_BOMBCHU_BAG) {
            gSaveContext.ship.stats.itemTimestamp[ITEM_BOMBCHU] = time;
        } else if (item == RG_MAGIC_SINGLE) {
            gSaveContext.ship.stats.itemTimestamp[ITEM_SINGLE_MAGIC] = time;
        } else if (item == RG_DOUBLE_DEFENSE) {
            gSaveContext.ship.stats.itemTimestamp[ITEM_DOUBLE_DEFENSE] = time;
        }
    }
}

extern "C" u8 Return_Item_Entry(GetItemEntry itemEntry, u8 returnItem);

extern "C" u16 Randomizer_Item_Give(PlayState* play, GetItemEntry giEntry) {
    if (giEntry.modIndex != MOD_RANDOMIZER) {
        LUSLOG_WARN(
            "Randomizer_Item_Give was called with a GetItemEntry with a mod index different from MOD_RANDOMIZER (%d)",
            giEntry.modIndex);
        assert(false);
        return -1;
    }

    RandomizerGet item = (RandomizerGet)giEntry.getItemId;

    // Gameplay stats: Update the time the item was obtained
    Randomizer_GameplayStats_SetTimestamp(item);

    // if it's an item that just sets a randomizerInf, set it
    if (randomizerGetToRandInf.find(item) != randomizerGetToRandInf.end()) {
        Flags_SetRandomizerInf(randomizerGetToRandInf.find(item)->second);
        return Return_Item_Entry(giEntry, RG_NONE);
    }

    // bottle items
    if (item >= RG_BOTTLE_WITH_RED_POTION && item <= RG_BOTTLE_WITH_BIG_POE) {
        for (u16 i = 0; i < 4; i++) {
            if (gSaveContext.inventory.items[SLOT_BOTTLE_1 + i] == ITEM_NONE) {
                ItemID bottleItem = ITEM_NONE;
                switch (item) {
                    case RG_BOTTLE_WITH_RED_POTION:
                        bottleItem = ITEM_POTION_RED;
                        break;
                    case RG_BOTTLE_WITH_GREEN_POTION:
                        bottleItem = ITEM_POTION_GREEN;
                        break;
                    case RG_BOTTLE_WITH_BLUE_POTION:
                        bottleItem = ITEM_POTION_BLUE;
                        break;
                    case RG_BOTTLE_WITH_FAIRY:
                        bottleItem = ITEM_FAIRY;
                        break;
                    case RG_BOTTLE_WITH_FISH:
                        bottleItem = ITEM_FISH;
                        break;
                    case RG_BOTTLE_WITH_BLUE_FIRE:
                        bottleItem = ITEM_BLUE_FIRE;
                        break;
                    case RG_BOTTLE_WITH_BUGS:
                        bottleItem = ITEM_BUG;
                        break;
                    case RG_BOTTLE_WITH_POE:
                        bottleItem = ITEM_POE;
                        break;
                    case RG_BOTTLE_WITH_BIG_POE:
                        bottleItem = ITEM_BIG_POE;
                        break;
                    default:
                        break;
                }

                gSaveContext.inventory.items[SLOT_BOTTLE_1 + i] = bottleItem;
                return Return_Item_Entry(giEntry, RG_NONE);
            }
        }
    }

    // dungeon items
    if ((item >= RG_FOREST_TEMPLE_SMALL_KEY && item <= RG_GANONS_CASTLE_SMALL_KEY) ||
        (item >= RG_FOREST_TEMPLE_KEY_RING && item <= RG_GANONS_CASTLE_KEY_RING) ||
        (item >= RG_FOREST_TEMPLE_BOSS_KEY && item <= RG_GANONS_CASTLE_BOSS_KEY) ||
        (item >= RG_DEKU_TREE_MAP && item <= RG_ICE_CAVERN_MAP) ||
        (item >= RG_DEKU_TREE_COMPASS && item <= RG_ICE_CAVERN_COMPASS)) {
        u16 mapIndex = gSaveContext.mapIndex;
        u8 numOfKeysOnKeyring = 0;
        switch (item) {
            case RG_DEKU_TREE_MAP:
            case RG_DEKU_TREE_COMPASS:
                mapIndex = SCENE_DEKU_TREE;
                break;
            case RG_DODONGOS_CAVERN_MAP:
            case RG_DODONGOS_CAVERN_COMPASS:
                mapIndex = SCENE_DODONGOS_CAVERN;
                break;
            case RG_JABU_JABUS_BELLY_MAP:
            case RG_JABU_JABUS_BELLY_COMPASS:
                mapIndex = SCENE_JABU_JABU;
                break;
            case RG_FOREST_TEMPLE_MAP:
            case RG_FOREST_TEMPLE_COMPASS:
            case RG_FOREST_TEMPLE_SMALL_KEY:
            case RG_FOREST_TEMPLE_KEY_RING:
            case RG_FOREST_TEMPLE_BOSS_KEY:
                mapIndex = SCENE_FOREST_TEMPLE;
                numOfKeysOnKeyring = FOREST_TEMPLE_SMALL_KEY_MAX;
                break;
            case RG_FIRE_TEMPLE_MAP:
            case RG_FIRE_TEMPLE_COMPASS:
            case RG_FIRE_TEMPLE_SMALL_KEY:
            case RG_FIRE_TEMPLE_KEY_RING:
            case RG_FIRE_TEMPLE_BOSS_KEY:
                mapIndex = SCENE_FIRE_TEMPLE;
                numOfKeysOnKeyring = FIRE_TEMPLE_SMALL_KEY_MAX;
                break;
            case RG_WATER_TEMPLE_MAP:
            case RG_WATER_TEMPLE_COMPASS:
            case RG_WATER_TEMPLE_SMALL_KEY:
            case RG_WATER_TEMPLE_KEY_RING:
            case RG_WATER_TEMPLE_BOSS_KEY:
                mapIndex = SCENE_WATER_TEMPLE;
                numOfKeysOnKeyring = WATER_TEMPLE_SMALL_KEY_MAX;
                break;
            case RG_SPIRIT_TEMPLE_MAP:
            case RG_SPIRIT_TEMPLE_COMPASS:
            case RG_SPIRIT_TEMPLE_SMALL_KEY:
            case RG_SPIRIT_TEMPLE_KEY_RING:
            case RG_SPIRIT_TEMPLE_BOSS_KEY:
                mapIndex = SCENE_SPIRIT_TEMPLE;
                numOfKeysOnKeyring = SPIRIT_TEMPLE_SMALL_KEY_MAX;
                break;
            case RG_SHADOW_TEMPLE_MAP:
            case RG_SHADOW_TEMPLE_COMPASS:
            case RG_SHADOW_TEMPLE_SMALL_KEY:
            case RG_SHADOW_TEMPLE_KEY_RING:
            case RG_SHADOW_TEMPLE_BOSS_KEY:
                mapIndex = SCENE_SHADOW_TEMPLE;
                numOfKeysOnKeyring = SHADOW_TEMPLE_SMALL_KEY_MAX;
                break;
            case RG_BOTTOM_OF_THE_WELL_MAP:
            case RG_BOTTOM_OF_THE_WELL_COMPASS:
            case RG_BOTTOM_OF_THE_WELL_SMALL_KEY:
            case RG_BOTTOM_OF_THE_WELL_KEY_RING:
                mapIndex = SCENE_BOTTOM_OF_THE_WELL;
                numOfKeysOnKeyring = BOTTOM_OF_THE_WELL_SMALL_KEY_MAX;
                break;
            case RG_ICE_CAVERN_MAP:
            case RG_ICE_CAVERN_COMPASS:
                mapIndex = SCENE_ICE_CAVERN;
                break;
            case RG_GANONS_CASTLE_BOSS_KEY:
                mapIndex = SCENE_GANONS_TOWER;
                break;
            case RG_GERUDO_TRAINING_GROUND_SMALL_KEY:
            case RG_GERUDO_TRAINING_GROUND_KEY_RING:
                mapIndex = SCENE_GERUDO_TRAINING_GROUND;
                numOfKeysOnKeyring = GERUDO_TRAINING_GROUND_SMALL_KEY_MAX;
                break;
            case RG_GERUDO_FORTRESS_SMALL_KEY:
            case RG_GERUDO_FORTRESS_KEY_RING:
                mapIndex = SCENE_THIEVES_HIDEOUT;
                numOfKeysOnKeyring = GERUDO_FORTRESS_SMALL_KEY_MAX;
                break;
            case RG_GANONS_CASTLE_SMALL_KEY:
            case RG_GANONS_CASTLE_KEY_RING:
                mapIndex = SCENE_INSIDE_GANONS_CASTLE;
                numOfKeysOnKeyring = GANONS_CASTLE_SMALL_KEY_MAX;
                break;
            default:
                break;
        }

        if ((item >= RG_FOREST_TEMPLE_SMALL_KEY) && (item <= RG_GANONS_CASTLE_SMALL_KEY)) {
            gSaveContext.ship.stats.dungeonKeys[mapIndex]++;
            if (gSaveContext.inventory.dungeonKeys[mapIndex] < 0) {
                gSaveContext.inventory.dungeonKeys[mapIndex] = 1;
            } else {
                gSaveContext.inventory.dungeonKeys[mapIndex]++;
            }
            return Return_Item_Entry(giEntry, RG_NONE);
        }

        if ((item >= RG_FOREST_TEMPLE_KEY_RING) && (item <= RG_GANONS_CASTLE_KEY_RING)) {
            gSaveContext.ship.stats.dungeonKeys[mapIndex] = numOfKeysOnKeyring;
            gSaveContext.inventory.dungeonKeys[mapIndex] = numOfKeysOnKeyring;
            return Return_Item_Entry(giEntry, RG_NONE);
        }

        u32 bitmask;
        if ((item >= RG_DEKU_TREE_MAP) && (item <= RG_ICE_CAVERN_MAP)) {
            bitmask = gBitFlags[2];
        } else if ((item >= RG_DEKU_TREE_COMPASS) && (item <= RG_ICE_CAVERN_COMPASS)) {
            bitmask = gBitFlags[1];
        } else {
            bitmask = gBitFlags[0];
        }

        gSaveContext.inventory.dungeonItems[mapIndex] |= bitmask;
        return Return_Item_Entry(giEntry, RG_NONE);
    } else if (item == RG_SKELETON_KEY) {
        Flags_SetRandomizerInf(RAND_INF_HAS_SKELETON_KEY);
        // This isn't technically necessary, because keys will no longer be consumed,
        // but for the player's sanity we display that they _have_ keys.
        gSaveContext.inventory.dungeonKeys[SCENE_FOREST_TEMPLE] = FOREST_TEMPLE_SMALL_KEY_MAX;
        gSaveContext.inventory.dungeonKeys[SCENE_FIRE_TEMPLE] = FIRE_TEMPLE_SMALL_KEY_MAX;
        gSaveContext.inventory.dungeonKeys[SCENE_WATER_TEMPLE] = WATER_TEMPLE_SMALL_KEY_MAX;
        gSaveContext.inventory.dungeonKeys[SCENE_SPIRIT_TEMPLE] = SPIRIT_TEMPLE_SMALL_KEY_MAX;
        gSaveContext.inventory.dungeonKeys[SCENE_SHADOW_TEMPLE] = SHADOW_TEMPLE_SMALL_KEY_MAX;
        gSaveContext.inventory.dungeonKeys[SCENE_BOTTOM_OF_THE_WELL] = BOTTOM_OF_THE_WELL_SMALL_KEY_MAX;
        gSaveContext.inventory.dungeonKeys[SCENE_GERUDO_TRAINING_GROUND] = GERUDO_TRAINING_GROUND_SMALL_KEY_MAX;
        gSaveContext.inventory.dungeonKeys[SCENE_THIEVES_HIDEOUT] = GERUDO_FORTRESS_SMALL_KEY_MAX;
        gSaveContext.inventory.dungeonKeys[SCENE_INSIDE_GANONS_CASTLE] = GANONS_CASTLE_SMALL_KEY_MAX;

        return Return_Item_Entry(giEntry, RG_NONE);
    } else if (item >= RG_GUARD_HOUSE_KEY && item <= RG_FISHING_HOLE_KEY) {
        Flags_SetRandomizerInf(
            (RandomizerInf)((int)RAND_INF_GUARD_HOUSE_UNLOCKED + ((item - RG_GUARD_HOUSE_KEY) * 2) + 1));
        return Return_Item_Entry(giEntry, RG_NONE);
    } else if (item >= RG_KEATON_MASK && item <= RG_MASK_OF_TRUTH) {
        Flags_SetRandomizerInf((RandomizerInf)((int)RAND_INF_CHILD_TRADES_HAS_MASK_KEATON + (item - RG_KEATON_MASK)));
        if (INV_CONTENT(ITEM_TRADE_CHILD) == ITEM_NONE) {
            INV_CONTENT(ITEM_TRADE_CHILD) = (int)ITEM_MASK_KEATON + (item - RG_KEATON_MASK);
        }
        return Return_Item_Entry(giEntry, RG_NONE);
    }

    switch (item) {
        case RG_MAGIC_SINGLE:
            gSaveContext.isMagicAcquired = true;
            gSaveContext.magicFillTarget = MAGIC_NORMAL_METER;
            Magic_Fill(play);
            break;
        case RG_MAGIC_DOUBLE:
            if (!gSaveContext.isMagicAcquired) {
                gSaveContext.isMagicAcquired = true;
            }
            gSaveContext.isDoubleMagicAcquired = true;
            gSaveContext.magicFillTarget = MAGIC_DOUBLE_METER;
            gSaveContext.magicLevel = 0;
            Magic_Fill(play);
            break;
        case RG_MAGIC_BEAN_PACK:
            if (INV_CONTENT(ITEM_BEAN) == ITEM_NONE) {
                INV_CONTENT(ITEM_BEAN) = ITEM_BEAN;
                AMMO(ITEM_BEAN) = 10;
            }
            break;
        case RG_DOUBLE_DEFENSE:
            gSaveContext.isDoubleDefenseAcquired = true;
            gSaveContext.inventory.defenseHearts = 20;
            gSaveContext.healthAccumulator = MAX_HEALTH;
            break;
        case RG_TYCOON_WALLET:
            Inventory_ChangeUpgrade(UPG_WALLET, 3);
            if (OTRGlobals::Instance->gRandomizer->GetRandoSettingValue(RSK_FULL_WALLETS)) {
                Rupees_ChangeBy(999);
            }
            break;
        case RG_CHILD_WALLET:
            Flags_SetRandomizerInf(RAND_INF_HAS_WALLET);
            if (OTRGlobals::Instance->gRandomizer->GetRandoSettingValue(RSK_FULL_WALLETS)) {
                Rupees_ChangeBy(99);
            }
            break;
        case RG_GREG_RUPEE:
            Rupees_ChangeBy(1);
            Flags_SetRandomizerInf(RAND_INF_GREG_FOUND);
            gSaveContext.ship.stats.itemTimestamp[TIMESTAMP_FOUND_GREG] = static_cast<u32>(GAMEPLAYSTAT_TOTAL_TIME);
            break;
        case RG_TRIFORCE_PIECE:
            gSaveContext.ship.quest.data.randomizer.triforcePiecesCollected++;
            GameInteractor_SetTriforceHuntPieceGiven(true);

            // Give Ganon's Boss Key and teleport to credits if set to Win when goal is reached.
            if (gSaveContext.ship.quest.data.randomizer.triforcePiecesCollected ==
                (OTRGlobals::Instance->gRandomizer->GetRandoSettingValue(RSK_TRIFORCE_HUNT_PIECES_REQUIRED) + 1)) {
                Flags_SetRandomizerInf(RAND_INF_GRANT_GANONS_BOSSKEY);

                if (OTRGlobals::Instance->gRandomizer->GetRandoSettingValue(RSK_TRIFORCE_HUNT) ==
                    RO_TRIFORCE_HUNT_WIN) {
                    // Save and warp are deferred until item queue drains
                    GameInteractor_SetTriforceHuntCreditsWarpActive(true);
                }
            }

            break;
        case RG_PROGRESSIVE_BOMBCHU_BAG:
            OTRGlobals::Instance->gRandoContext->HandleGetBombchuBag();
            break;
        case RG_MASTER_SWORD:
            if (!CHECK_OWNED_EQUIP(EQUIP_TYPE_SWORD, EQUIP_INV_SWORD_MASTER)) {
                gSaveContext.inventory.equipment |= gBitFlags[1] << gEquipShifts[EQUIP_TYPE_SWORD];
            }
            break;
        case RG_DEKU_STICK_BAG:
            Inventory_ChangeUpgrade(UPG_STICKS, 1);
            INV_CONTENT(ITEM_STICK) = ITEM_STICK;
            AMMO(ITEM_STICK) = static_cast<int8_t>(CUR_CAPACITY(UPG_STICKS));
            break;
        case RG_DEKU_NUT_BAG:
            Inventory_ChangeUpgrade(UPG_NUTS, 1);
            INV_CONTENT(ITEM_NUT) = ITEM_NUT;
            AMMO(ITEM_NUT) = static_cast<int8_t>(CUR_CAPACITY(UPG_NUTS));
            break;
        case RG_ROCS_FEATHER:
            Flags_SetRandomizerInf(RAND_INF_OBTAINED_ROCS_FEATHER);
            if (INV_CONTENT(ITEM_NAYRUS_LOVE) == ITEM_NONE) {
                INV_CONTENT(ITEM_NAYRUS_LOVE) = ITEM_ROCS_FEATHER;
            }
            break;
        default:
            LUSLOG_WARN("Randomizer_Item_Give didn't have behaviour specified for getItemId=%d", item);
            assert(false);
            return -1;
    }

    return Return_Item_Entry(giEntry, RG_NONE);
}
