#include "randomizer_check_objects.h"
#include "static_data.h"
#include "context.h"
#include <map>
#include <string>
#include <libultraship/bridge.h>
#include "z64.h"
#include "fishsanity.h"

std::map<RandomizerCheckArea, std::string> rcAreaNames = {
    { RCAREA_KOKIRI_FOREST, "Kokiri Forest" },
    { RCAREA_LOST_WOODS, "Lost Woods" },
    { RCAREA_SACRED_FOREST_MEADOW, "Sacred Forest Meadow" },
    { RCAREA_HYRULE_FIELD, "Hyrule Field" },
    { RCAREA_LAKE_HYLIA, "Lake Hylia" },
    { RCAREA_GERUDO_VALLEY, "Gerudo Valley" },
    { RCAREA_GERUDO_FORTRESS, "Gerudo Fortress" },
    { RCAREA_WASTELAND, "Haunted Wasteland" },
    { RCAREA_DESERT_COLOSSUS, "Desert Colossus" },
    { RCAREA_MARKET, "Hyrule Market" },
    { RCAREA_HYRULE_CASTLE, "Hyrule Castle" },
    { RCAREA_KAKARIKO_VILLAGE, "Kakariko Village" },
    { RCAREA_GRAVEYARD, "Graveyard" },
    { RCAREA_DEATH_MOUNTAIN_TRAIL, "Death Mountain Trail" },
    { RCAREA_GORON_CITY, "Goron City" },
    { RCAREA_DEATH_MOUNTAIN_CRATER, "Death Mountain Crater" },
    { RCAREA_ZORAS_RIVER, "Zora's River" },
    { RCAREA_ZORAS_DOMAIN, "Zora's Domain" },
    { RCAREA_ZORAS_FOUNTAIN, "Zora's Fountain" },
    { RCAREA_LON_LON_RANCH, "Lon Lon Ranch" },
    { RCAREA_DEKU_TREE, "Deku Tree" },
    { RCAREA_DODONGOS_CAVERN, "Dodongo's Cavern" },
    { RCAREA_JABU_JABUS_BELLY, "Jabu Jabu's Belly" },
    { RCAREA_FOREST_TEMPLE, "Forest Temple" },
    { RCAREA_FIRE_TEMPLE, "Fire Temple" },
    { RCAREA_WATER_TEMPLE, "Water Temple" },
    { RCAREA_SPIRIT_TEMPLE, "Spirit Temple" },
    { RCAREA_SHADOW_TEMPLE, "Shadow Temple" },
    { RCAREA_BOTTOM_OF_THE_WELL, "Bottom of the Well" },
    { RCAREA_ICE_CAVERN, "Ice Cavern" },
    { RCAREA_GERUDO_TRAINING_GROUND, "Gerudo Training Grounds" },
    { RCAREA_GANONS_CASTLE, "Ganon's Castle" },
    { RCAREA_INVALID, "Invalid" },
};

std::map<RandomizerCheckArea, std::vector<RandomizerCheck>> rcObjectsByArea = {};

std::map<RandomizerCheckArea, std::vector<RandomizerCheck>> RandomizerCheckObjects::GetAllRCObjectsByArea() {
    if (rcObjectsByArea.size() == 0) {
        for (auto& location : Rando::StaticData::GetLocationTable()) {
            // There are some RCs that don't have checks implemented yet, prevent adding these to the map.
            if (location.GetRandomizerCheck() != RC_UNKNOWN_CHECK) {
                rcObjectsByArea[location.GetArea()].push_back(location.GetRandomizerCheck());
            }
        }
    }

    return rcObjectsByArea;
}

bool RandomizerCheckObjects::AreaIsDungeon(RandomizerCheckArea area) {
    return area == RCAREA_GANONS_CASTLE || area == RCAREA_GERUDO_TRAINING_GROUND || area == RCAREA_ICE_CAVERN ||
           area == RCAREA_BOTTOM_OF_THE_WELL || area == RCAREA_SHADOW_TEMPLE || area == RCAREA_SPIRIT_TEMPLE ||
           area == RCAREA_WATER_TEMPLE || area == RCAREA_FIRE_TEMPLE || area == RCAREA_FOREST_TEMPLE ||
           area == RCAREA_JABU_JABUS_BELLY || area == RCAREA_DODONGOS_CAVERN || area == RCAREA_DEKU_TREE;
}

bool RandomizerCheckObjects::AreaIsOverworld(RandomizerCheckArea area) {
    return !AreaIsDungeon(area);
}

std::string RandomizerCheckObjects::GetRCAreaName(RandomizerCheckArea area) {
    return rcAreaNames[area];
}

std::map<SceneID, RandomizerCheckArea> rcAreaBySceneID = {};
std::map<SceneID, RandomizerCheckArea> RandomizerCheckObjects::GetAllRCAreaBySceneID() {
    // memoize on first request
    if (rcAreaBySceneID.size() == 0) {
        for (auto& location : Rando::StaticData::GetLocationTable()) {
            // There are some RCs that don't have checks implemented yet, prevent adding these to the map.
            if (location.GetRandomizerCheck() != RC_UNKNOWN_CHECK) {
                rcAreaBySceneID[location.GetScene()] = location.GetArea();
            }
        }
        // Add checkless Hyrule Market areas to the area return
        for (int id = (int)SCENE_MARKET_ENTRANCE_DAY; id <= (int)SCENE_MARKET_RUINS; id++) {
            rcAreaBySceneID[(SceneID)id] = RCAREA_MARKET;
        }
        rcAreaBySceneID[SCENE_TEMPLE_OF_TIME] = RCAREA_MARKET;
        rcAreaBySceneID[SCENE_CASTLE_COURTYARD_GUARDS_DAY] = RCAREA_HYRULE_CASTLE;
        rcAreaBySceneID[SCENE_CASTLE_COURTYARD_GUARDS_NIGHT] = RCAREA_HYRULE_CASTLE;
    }
    return rcAreaBySceneID;
}

RandomizerCheckArea RandomizerCheckObjects::GetRCAreaBySceneID(SceneID sceneId) {
    std::map<SceneID, RandomizerCheckArea> areas = GetAllRCAreaBySceneID();
    auto areaIt = areas.find(sceneId);
    if (areaIt == areas.end())
        return RCAREA_INVALID;
    else
        return areaIt->second;
}

void RandomizerCheckObjects::UpdateImGuiVisibility() {
    auto ctx = Rando::Context::GetInstance();
    for (auto& location : Rando::StaticData::GetLocationTable()) {
        auto itemLoc = ctx->GetItemLocation(location.GetRandomizerCheck());
        itemLoc->SetVisible(
            (location.GetRandomizerCheck() != RC_UNKNOWN_CHECK) &&
            (!RandomizerCheckObjects::AreaIsDungeon(location.GetArea()) || location.GetQuest() == RCQUEST_BOTH ||
             location.GetQuest() == RCQUEST_MQ &&
                 ((CVarGetInteger("gRandomizeMqDungeons", RO_MQ_DUNGEONS_NONE) == RO_MQ_DUNGEONS_SET_NUMBER &&
                       (CVarGetInteger("gRandomizeMqDungeonCount", 12) > 0) || // at least one MQ dungeon
                   CVarGetInteger("gRandomizeMqDungeons", RO_MQ_DUNGEONS_NONE) == RO_MQ_DUNGEONS_RANDOM_NUMBER)) ||
             location.GetQuest() == RCQUEST_VANILLA &&
                 (CVarGetInteger("gRandomizeMqDungeons", RO_MQ_DUNGEONS_NONE) != RO_MQ_DUNGEONS_SET_NUMBER ||
                  CVarGetInteger("gRandomizeMqDungeonCount", 12) < 12) // at least one vanilla dungeon
             ) &&
            (location.GetRCType() != RCTYPE_SHOP ||
             CVarGetInteger("gRandomizeShopsanity", RO_SHOPSANITY_OFF) > RO_SHOPSANITY_ZERO_ITEMS) &&
            (location.GetRCType() != RCTYPE_SCRUB ||
             CVarGetInteger("gRandomizeShuffleScrubs", RO_SCRUBS_OFF) != RO_SCRUBS_OFF ||
             location.GetRandomizerCheck() == RC_HF_DEKU_SCRUB_GROTTO ||
             location.GetRandomizerCheck() == RC_LW_DEKU_SCRUB_GROTTO_FRONT ||
             location.GetRandomizerCheck() ==
                 RC_LW_DEKU_SCRUB_NEAR_BRIDGE) && // The 3 scrubs that are always randomized
            (location.GetRCType() != RCTYPE_MERCHANT ||
             CVarGetInteger("gRandomizeShuffleMerchants", RO_SHUFFLE_MERCHANTS_OFF) != RO_SHUFFLE_MERCHANTS_OFF) &&
            (location.GetRCType() != RCTYPE_SONG_LOCATION ||
             CVarGetInteger("gRandomizeShuffleSongs", RO_SONG_SHUFFLE_SONG_LOCATIONS) !=
                 RO_SONG_SHUFFLE_SONG_LOCATIONS) && // song locations
            ((location.GetRCType() != RCTYPE_BOSS_HEART_OR_OTHER_REWARD &&
              location.GetRandomizerCheck() != RC_SONG_FROM_IMPA &&
              location.GetRandomizerCheck() != RC_SHEIK_IN_ICE_CAVERN) ||
             CVarGetInteger("gRandomizeShuffleSongs", RO_SONG_SHUFFLE_SONG_LOCATIONS) !=
                 RO_SONG_SHUFFLE_DUNGEON_REWARDS) && // song dungeon rewards
            (location.GetRCType() != RCTYPE_DUNGEON_REWARD ||
             CVarGetInteger("gRandomizeShuffleDungeonReward", RO_DUNGEON_REWARDS_END_OF_DUNGEON) !=
                 RO_DUNGEON_REWARDS_END_OF_DUNGEON) && // dungeon rewards end of dungeons
            (location.GetRCType() != RCTYPE_OCARINA ||
             CVarGetInteger("gRandomizeShuffleOcarinas", RO_GENERIC_NO)) && // ocarina locations
            (location.GetRandomizerCheck() != RC_HC_ZELDAS_LETTER) && // don't show until we support shuffling letter
            (location.GetRCType() !=
             RCTYPE_GOSSIP_STONE) && // don't show gossip stones (maybe gossipsanity will be a thing eventually?)
            (location.GetRCType() != RCTYPE_LINKS_POCKET) && // links pocket can be set to nothing if needed
            (location.GetRCType() !=
             RCTYPE_CHEST_GAME) && // don't show non final reward chest game checks until we support shuffling them
            (location.GetRCType() != RCTYPE_SKULL_TOKEN ||
             (CVarGetInteger("gRandomizeShuffleTokens", RO_TOKENSANITY_OFF) == RO_TOKENSANITY_ALL) ||
             ((CVarGetInteger("gRandomizeShuffleTokens", RO_TOKENSANITY_OFF) == RO_TOKENSANITY_OVERWORLD) &&
              RandomizerCheckObjects::AreaIsOverworld(location.GetArea())) ||
             ((CVarGetInteger("gRandomizeShuffleTokens", RO_TOKENSANITY_OFF) == RO_TOKENSANITY_DUNGEONS) &&
              RandomizerCheckObjects::AreaIsDungeon(location.GetArea()))) &&
            (location.GetRCType() != RCTYPE_COW || CVarGetInteger("gRandomizeShuffleCows", RO_GENERIC_NO)) &&
            (location.GetRCType() != RCTYPE_FISH || ctx->GetFishsanity()->GetFishLocationIncluded(&location, FSO_SOURCE_CVARS)) &&
            (location.GetRCType() != RCTYPE_ADULT_TRADE ||
             CVarGetInteger("gRandomizeShuffleAdultTrade", RO_GENERIC_NO)) &&
            (location.GetRandomizerCheck() != RC_KF_KOKIRI_SWORD_CHEST ||
             CVarGetInteger("gRandomizeShuffleKokiriSword", RO_GENERIC_NO)) &&
            (location.GetRandomizerCheck() != RC_ZR_MAGIC_BEAN_SALESMAN ||
             CVarGetInteger("gRandomizeShuffleBeans", RO_GENERIC_NO)) &&
            (location.GetRandomizerCheck() != RC_HC_MALON_EGG ||
             CVarGetInteger("gRandomizeShuffleWeirdEgg", RO_GENERIC_NO)) &&
            (location.GetRCType() != RCTYPE_FROG_SONG ||
             CVarGetInteger("gRandomizeShuffleFrogSongRupees", RO_GENERIC_NO)) &&
            (location.GetRCType() != RCTYPE_MAP_COMPASS ||
             CVarGetInteger("gRandomizeStartingMapsCompasses", RO_DUNGEON_ITEM_LOC_OWN_DUNGEON) !=
                 RO_DUNGEON_ITEM_LOC_VANILLA) &&
            (location.GetRCType() != RCTYPE_SMALL_KEY ||
             CVarGetInteger("gRandomizeKeysanity", RO_DUNGEON_ITEM_LOC_OWN_DUNGEON) != RO_DUNGEON_ITEM_LOC_VANILLA) &&
            (location.GetRCType() != RCTYPE_BOSS_KEY ||
             CVarGetInteger("gRandomizeBossKeysanity", RO_DUNGEON_ITEM_LOC_OWN_DUNGEON) !=
                 RO_DUNGEON_ITEM_LOC_VANILLA) &&
            (location.GetRCType() != RCTYPE_GANON_BOSS_KEY ||
             CVarGetInteger("gRandomizeShuffleGanonBossKey", RO_GANON_BOSS_KEY_VANILLA) !=
                 RO_GANON_BOSS_KEY_VANILLA) && // vanilla ganon boss key
            (location.GetRandomizerCheck() != RC_TOT_LIGHT_ARROWS_CUTSCENE ||
             (CVarGetInteger("gRandomizeShuffleGanonBossKey", RO_GANON_BOSS_KEY_VANILLA) !=
                  RO_GANON_BOSS_KEY_LACS_DUNGEONS &&
              CVarGetInteger("gRandomizeShuffleGanonBossKey", RO_GANON_BOSS_KEY_VANILLA) !=
                  RO_GANON_BOSS_KEY_LACS_MEDALLIONS &&
              CVarGetInteger("gRandomizeShuffleGanonBossKey", RO_GANON_BOSS_KEY_VANILLA) !=
                  RO_GANON_BOSS_KEY_LACS_REWARDS &&
              CVarGetInteger("gRandomizeShuffleGanonBossKey", RO_GANON_BOSS_KEY_VANILLA) !=
                  RO_GANON_BOSS_KEY_LACS_STONES &&
              CVarGetInteger("gRandomizeShuffleGanonBossKey", RO_GANON_BOSS_KEY_VANILLA) !=
                  RO_GANON_BOSS_KEY_LACS_TOKENS &&
              CVarGetInteger("gRandomizeShuffleGanonBossKey", RO_GANON_BOSS_KEY_VANILLA) !=
                  RO_GANON_BOSS_KEY_LACS_VANILLA)) && // LACS ganon boss key
            (location.GetRandomizerCheck() != RC_KAK_100_GOLD_SKULLTULA_REWARD ||
             CVarGetInteger("gRandomizeShuffleGanonBossKey", RO_GANON_BOSS_KEY_VANILLA) !=
                 RO_GANON_BOSS_KEY_KAK_TOKENS) && // 100 skull reward ganon boss key
            (location.GetRCType() != RCTYPE_GF_KEY && location.GetRandomizerCheck() != RC_GF_GERUDO_MEMBERSHIP_CARD ||
             (CVarGetInteger("gRandomizeGerudoFortress", RO_GF_NORMAL) == RO_GF_OPEN &&
              location.GetRCType() != RCTYPE_GF_KEY && location.GetRandomizerCheck() != RC_GF_GERUDO_MEMBERSHIP_CARD) ||
             (CVarGetInteger("gRandomizeGerudoFortress", RO_GF_NORMAL) == RO_GF_FAST &&
              ((location.GetRandomizerCheck() == RC_GF_GERUDO_MEMBERSHIP_CARD &&
                CVarGetInteger("gRandomizeShuffleGerudoToken", RO_GENERIC_NO) == RO_GENERIC_YES) ||
               (location.GetRandomizerCheck() == RC_GF_NORTH_F1_CARPENTER &&
                CVarGetInteger("gRandomizeGerudoKeys", RO_GERUDO_KEYS_VANILLA) != RO_GERUDO_KEYS_VANILLA))) ||
             (CVarGetInteger("gRandomizeGerudoFortress", RO_GF_NORMAL) == RO_GF_NORMAL &&
              ((location.GetRandomizerCheck() == RC_GF_GERUDO_MEMBERSHIP_CARD &&
                CVarGetInteger("gRandomizeShuffleGerudoToken", RO_GENERIC_NO) == RO_GENERIC_YES) ||
               (location.GetRCType() == RCTYPE_GF_KEY &&
                CVarGetInteger("gRandomizeGerudoKeys", RO_GERUDO_KEYS_VANILLA) != RO_GERUDO_KEYS_VANILLA)))));
    }
}
