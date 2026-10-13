#include "Logic.h"
#include "Rando/StaticData/StaticData.h"

namespace Rando {

namespace Logic {

// if we keep adding more groups we should probably make this an enum or somthing
static constexpr int CONFINEMENT_GROUP_SONGS = DUNGEON_SCENE_INDEX_STONE_TOWER_TEMPLE_BOSS + 1;

int SceneIdToDungeon(SceneId sceneId) {
    switch (sceneId) {
        case SCENE_MITURIN:
        case SCENE_MITURIN_BS:
            return DUNGEON_SCENE_INDEX_WOODFALL_TEMPLE;
        case SCENE_HAKUGIN:
        case SCENE_HAKUGIN_BS:
            return DUNGEON_SCENE_INDEX_SNOWHEAD_TEMPLE;
        case SCENE_SEA:
        case SCENE_SEA_BS:
            return DUNGEON_SCENE_INDEX_GREAT_BAY_TEMPLE;
        case SCENE_INISIE_N:
        case SCENE_INISIE_R:
        case SCENE_INISIE_BS:
            return DUNGEON_SCENE_INDEX_STONE_TOWER_TEMPLE;
        default:
            return -1;
    }
}

int DungeonItemToDungeon(RandoItemId itemId) {
    int dungeon = -1;
    switch (itemId) {
        case RI_WOODFALL_SMALL_KEY:
        case RI_WOODFALL_BOSS_KEY:
        case RI_WOODFALL_STRAY_FAIRY:
        case RI_REMAINS_ODOLWA:
            dungeon = DUNGEON_SCENE_INDEX_WOODFALL_TEMPLE;
            break;
        case RI_SNOWHEAD_SMALL_KEY:
        case RI_SNOWHEAD_BOSS_KEY:
        case RI_SNOWHEAD_STRAY_FAIRY:
        case RI_REMAINS_GOHT:
            dungeon = DUNGEON_SCENE_INDEX_SNOWHEAD_TEMPLE;
            break;
        case RI_GREAT_BAY_SMALL_KEY:
        case RI_GREAT_BAY_BOSS_KEY:
        case RI_GREAT_BAY_STRAY_FAIRY:
        case RI_REMAINS_GYORG:
            dungeon = DUNGEON_SCENE_INDEX_GREAT_BAY_TEMPLE;
            break;
        case RI_STONE_TOWER_SMALL_KEY:
        case RI_STONE_TOWER_BOSS_KEY:
        case RI_STONE_TOWER_STRAY_FAIRY:
        case RI_REMAINS_TWINMOLD:
            dungeon = DUNGEON_SCENE_INDEX_STONE_TOWER_TEMPLE;
            break;
        default:
            return -1;
    }
    return dungeon;
}

RandoOptionId DungeonItemPlacementOption(RandoItemId itemId) {
    if (DungeonItemToDungeon(itemId) < 0 && itemId != RI_CLOCK_TOWN_STRAY_FAIRY) {
        return RO_MAX;
    }
    switch (Rando::StaticData::Items[itemId].randoItemType) {
        case RITYPE_SMALL_KEY:
            return RO_PLACEMENT_SMALL_KEYS;
        case RITYPE_BOSS_KEY:
            return RO_PLACEMENT_BOSS_KEYS;
        case RITYPE_STRAY_FAIRY:
            return RO_PLACEMENT_STRAY_FAIRIES;
        default:
            return RO_MAX;
    }
}

bool StaysAtVanillaCheck(RandoItemId itemId, const RandoSaveInfo& saveInfo) {
    RandoOptionId placementOption = DungeonItemPlacementOption(itemId);
    if (placementOption == RO_MAX) {
        return false;
    }
    switch (saveInfo.randoSaveOptions[placementOption]) {
        case RO_DUNGEON_ITEM_VANILLA:
            return true;
        case RO_DUNGEON_ITEM_OWN_DUNGEON:
            return DungeonItemToDungeon(itemId) < 0;
        default:
            return false;
    }
}

int RandoItemIdToDungeon(RandoItemId itemId) {
    switch (itemId) {
        case RI_REMAINS_ODOLWA:
        case RI_REMAINS_GOHT:
        case RI_REMAINS_GYORG:
        case RI_REMAINS_TWINMOLD:
            return RANDO_SAVE_OPTIONS[RO_SHUFFLE_BOSS_REMAINS] == RO_REMAINS_SHUFFLE_OWN_DUNGEON
                       ? DungeonItemToDungeon(itemId)
                       : -1;
        default:
            break;
    }

    RandoOptionId placementOption = DungeonItemPlacementOption(itemId);
    if (placementOption == RO_MAX || RANDO_SAVE_OPTIONS[placementOption] != RO_DUNGEON_ITEM_OWN_DUNGEON) {
        return -1;
    }
    return DungeonItemToDungeon(itemId);
}

bool IsSongLocationItem(RandoItemId itemId) {
    static const std::array<bool, RI_MAX> songLocationItems = [] {
        std::array<bool, RI_MAX> items{};
        for (auto& [randoCheckId, randoStaticCheck] : Rando::StaticData::Checks) {
            if (randoStaticCheck.randoCheckType == RCTYPE_SONG) {
                items[randoStaticCheck.randoItemId] = true;
            }
        }
        return items;
    }();
    return itemId < RI_MAX && songLocationItems[itemId];
}

int CheckIdToConfinementGroup(RandoCheckId checkId) {
    auto& randoStaticCheck = Rando::StaticData::Checks[checkId];
    if (RANDO_SAVE_OPTIONS[RO_SHUFFLE_SONGS] == RO_SONG_SHUFFLE_SONG_LOCATIONS &&
        randoStaticCheck.randoCheckType == RCTYPE_SONG) {
        return CONFINEMENT_GROUP_SONGS;
    }
    return SceneIdToDungeon(randoStaticCheck.sceneId);
}

int RandoItemIdToConfinementGroup(RandoItemId itemId) {
    if (RANDO_SAVE_OPTIONS[RO_SHUFFLE_SONGS] == RO_SONG_SHUFFLE_SONG_LOCATIONS && IsSongLocationItem(itemId)) {
        return CONFINEMENT_GROUP_SONGS;
    }
    return RandoItemIdToDungeon(itemId);
}

bool IsItemAllowedAtCheck(RandoItemId itemId, RandoCheckId checkId) {
    int confinedGroup = RandoItemIdToConfinementGroup(itemId);
    return confinedGroup < 0 || confinedGroup == CheckIdToConfinementGroup(checkId);
}

size_t SelectItemForCheck(const std::vector<RandoItemId>& itemPool, const std::vector<RandoCheckId>& checkPool,
                          RandoCheckId checkId) {
    int group = CheckIdToConfinementGroup(checkId);
    if (group >= 0) {
        int confinedItems = 0;
        for (RandoItemId itemId : itemPool) {
            if (RandoItemIdToConfinementGroup(itemId) == group) {
                confinedItems++;
            }
        }
        if (confinedItems > 0) {
            int groupChecks = 1; // this check was already removed from checkPool
            for (RandoCheckId other : checkPool) {
                if (CheckIdToConfinementGroup(other) == group) {
                    groupChecks++;
                }
            }
            if (confinedItems >= groupChecks) {
                for (size_t i = itemPool.size(); i-- > 0;) {
                    if (RandoItemIdToConfinementGroup(itemPool[i]) == group) {
                        return i;
                    }
                }
            }
        }
    }

    for (size_t i = itemPool.size(); i-- > 0;) {
        if (IsItemAllowedAtCheck(itemPool[i], checkId)) {
            return i;
        }
    }
    return itemPool.size();
}

void PreplaceConfinedItems(std::vector<RandoCheckId>& checkPool, std::vector<RandoItemId>& itemPool) {
    std::map<int, std::vector<RandoCheckId>> groupChecks;
    for (RandoCheckId checkId : checkPool) {
        int group = CheckIdToConfinementGroup(checkId);
        if (group >= 0) {
            groupChecks[group].push_back(checkId);
        }
    }

    std::set<RandoCheckId> placedChecks;
    std::set<size_t> placedItems;
    for (size_t i = 0; i < itemPool.size(); i++) {
        int group = RandoItemIdToConfinementGroup(itemPool[i]);
        if (group < 0) {
            continue; // not confined to a dungeon
        }
        auto& checks = groupChecks[group];
        if (checks.empty()) {
            continue; // dungeon out of room: leave it for the general pool
        }
        size_t pick = Ship_Random(0, checks.size());
        RandoCheckId checkId = checks[pick];
        checks.erase(checks.begin() + pick);

        RANDO_SAVE_CHECKS[checkId].shuffled = true;
        RANDO_SAVE_CHECKS[checkId].randoItemId = itemPool[i];
        placedChecks.insert(checkId);
        placedItems.insert(i);
    }

    // Remove the placed items and checks from the pools.
    std::vector<RandoItemId> remainingItems;
    for (size_t i = 0; i < itemPool.size(); i++) {
        if (placedItems.find(i) == placedItems.end()) {
            remainingItems.push_back(itemPool[i]);
        }
    }
    itemPool = std::move(remainingItems);

    std::vector<RandoCheckId> remainingChecks;
    for (RandoCheckId checkId : checkPool) {
        if (placedChecks.find(checkId) == placedChecks.end()) {
            remainingChecks.push_back(checkId);
        }
    }
    checkPool = std::move(remainingChecks);
}

} // namespace Logic

} // namespace Rando
