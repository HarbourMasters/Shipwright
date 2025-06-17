#include "SceneDB.h"

#include <assert.h>
#include <algorithm>
#include <vector>

#include "variables.h"

SceneDB* SceneDB::Instance;
EntranceDB* EntranceDB::Instance;

extern const std::vector<SceneDB::Init> sceneDBInit;

// We need a different struct than EntranceDB::Init (scene is a int not a string) because tables/entrance_table.h has
// the scene as an enum, which we do not have a compile-time way to convert to a string
struct EntranceTableInit {
    std::string name;
    std::string desc;
    s32 scene;
    s32 spawn;
    bool continueBgm;
    bool displayTitleCard;
    u8 endTransition;
    u8 startTransition;
};

#define DEFINE_ENTRANCE(enumName, sceneId, spawn, continueBgm, displayTitleCard, endTransType, startTransType) \
    { #enumName, "", sceneId, spawn, continueBgm, displayTitleCard, endTransType, startTransType },

static const EntranceTableInit entranceDBInit[] = {
#include "tables/entrance_table.h"
};

#undef DEFINE_ENTRANCE

SceneDB::SceneDB() {
    db.reserve(SCENE_ID_MAX); // reserve size for all initial entries so we don't do it for each
    for (const Init& init : sceneDBInit) {
        AddEntry(init);
    }

    SceneDB::Entry& entry = RetrieveEntry(SCENE_BOTTOM_OF_THE_WELL);
    entry.entry.epona.allowed = true;
    entry.SetEponaSpawnPos({ { -630, 0, 53 } });
}

// Adds an scene at the given index. The name must be unique.
SceneDB::Entry& SceneDB::AddEntry(const std::string& name, const std::string& desc, size_t index) {
    assert(!nameTable.contains(name)); // TODO this should maybe throw instead. We'll need to think about error handling
                                       // for mods that try to declare the same scene.

    if (db.size() < (index + 1)) {
        db.resize(index + 1);
    }
    Entry& newEntry = db.at(index);
    newEntry.entry.id = index;

    assert(!newEntry.entry.valid);

    nextFreeId = std::max(nextFreeId, index + 1);

    nameTable[name] = newEntry.entry.id;
    newEntry.SetName(name);
    newEntry.SetDesc(desc);

    newEntry.entry.valid = true;

    return newEntry;
}

// Adds an scene with the new SceneDBInit struct. The id assigned to the scene is dynamic. Use the returned Entry or
// RetrieveId to get it.
SceneDB::Entry& SceneDB::AddEntry(const Init& init) {
    Entry& entry = AddEntry(init.name, init.desc, nextFreeId);

    entry.SetTitleCard(init.titleCard);

    entry.entry.sceneDrawConfig = init.sceneDrawConfig;
    entry.entry.restrictions.hGauge = init.restrictions.hGauge;
    entry.entry.restrictions.bButton = init.restrictions.bButton;
    entry.entry.restrictions.aButton = init.restrictions.aButton;
    entry.entry.restrictions.bottles = init.restrictions.bottles;
    entry.entry.restrictions.tradeItems = init.restrictions.tradeItems;
    entry.entry.restrictions.hookshot = init.restrictions.hookshot;
    entry.entry.restrictions.ocarina = init.restrictions.ocarina;
    entry.entry.restrictions.warpSongs = init.restrictions.warpSongs;
    entry.entry.restrictions.sunsSong = init.restrictions.sunsSong;
    entry.entry.restrictions.farores = init.restrictions.farores;
    entry.entry.restrictions.dinsNayrus = init.restrictions.dinsNayrus;
    entry.entry.restrictions.all = init.restrictions.all;
    entry.entry.epona.allowed = init.epona.allowed;
    entry.entry.epona.startingPos = init.epona.startingPos;
    entry.entry.epona.angle = init.epona.angle;
    entry.SetEponaSpawnPos(init.epona.spawnPos);

    entry.entry.compassInfo.scaleX = init.compassInfo.scaleX;
    entry.entry.compassInfo.scaleY = init.compassInfo.scaleY;
    entry.entry.compassInfo.x = init.compassInfo.x;
    entry.entry.compassInfo.y = init.compassInfo.y;

    if (init.bossData.mapScene != "") {
        int id = RetrieveId(init.bossData.mapScene);
        assert(id != -1); // TODO error of some kind for invalid mods
        entry.entry.bossData.mapScene = id;
    } else {
        entry.entry.bossData.mapScene = -1;
    }
    entry.entry.dungeonData.bossFloor = init.dungeonData.bossFloor;
    entry.SetDungeonPalettes(init.dungeonData.palettes);
    entry.SetDungeonNameTextures(init.dungeonData.nameEngTexture, init.dungeonData.nameGerTexture,
                                 init.dungeonData.nameFraTexture);
    entry.SetDungeonFloors(init.dungeonData.floors);
    entry.SetDungeonRooms(init.dungeonData.rooms);
    entry.SetDungeonIntraRoomTransitions(init.dungeonData.intraRoomTransitions);

    entry.entry.worldData.minimapX = init.worldData.minimapX;
    entry.entry.worldData.minimapY = init.worldData.minimapY;
    entry.entry.worldData.minimapWidth = init.worldData.minimapWidth;
    entry.entry.worldData.minimapHeight = init.worldData.minimapHeight;
    entry.entry.worldData.iconX = init.worldData.iconX;
    entry.entry.worldData.iconY = init.worldData.iconY;
    entry.entry.worldData.entranceFlag = init.worldData.entranceFlag;
    entry.SetWorldMinimapTexture(init.worldData.minimapTexture);

    return entry;
}

// Get the SceneDB::Entry for the given scene id.
SceneDB::Entry& SceneDB::RetrieveEntry(const int id) {
    static Entry invalid;
    if ((id < 0) || (id >= db.size())) {
        return invalid;
    }
    return db[id];
}

// Get the id for a given scene by name.
int SceneDB::RetrieveId(const std::string& name) {
    auto entry = nameTable.find(name);
    if (entry == nameTable.end()) {
        return -1;
    }

    return entry->second;
}

// Get the number of scene entries
size_t SceneDB::GetNumEntries() {
    return db.size();
}

SceneDB::Entry::Entry() {
    entry.name = nullptr;
    entry.desc = nullptr;
    entry.valid = false;
    entry.id = 0;
    entry.titleCard = nullptr;
    entry.sceneDrawConfig = SDC_DEFAULT;
    entry.restrictions.hGauge = false;
    entry.restrictions.bButton = false;
    entry.restrictions.aButton = false;
    entry.restrictions.bottles = false;
    entry.restrictions.tradeItems = false;
    entry.restrictions.hookshot = false;
    entry.restrictions.ocarina = false;
    entry.restrictions.warpSongs = false;
    entry.restrictions.sunsSong = false;
    entry.restrictions.farores = false;
    entry.restrictions.dinsNayrus = false;
    entry.restrictions.all = false;
    entry.epona.allowed = false;
    entry.epona.startingPos = { 0, 0, 0 };
    entry.epona.spawnPos = {};
    entry.epona.angle = 0;
    entry.compassInfo.scaleX = 0;
    entry.compassInfo.scaleY = 0;
    entry.compassInfo.x = 0;
    entry.compassInfo.y = 0;
    entry.bossData.mapScene = -1;
    entry.dungeonData.bossFloor = -1;
    entry.dungeonData.palettes = nullptr;
    entry.dungeonData.numPalettes = 0;
    for (size_t i = 0; i < 8; i++) {
        entry.dungeonData.floors[i].id = static_cast<FloorID>(0);
        entry.dungeonData.floors[i].height = 0.0f;
        entry.dungeonData.floors[i].rooms = nullptr;
        entry.dungeonData.floors[i].numRooms = 0;
        entry.dungeonData.floors[i].mapLeftTexture = nullptr;
        entry.dungeonData.floors[i].mapRightTexture = nullptr;
    }
    entry.dungeonData.rooms = nullptr;
    entry.dungeonData.numRooms = 0;
    entry.dungeonData.intraRoomTransitions = nullptr;
    entry.dungeonData.numIntraRoomTransitions = 0;
    entry.worldData.minimapX = 0;
    entry.worldData.minimapY = 0;
    entry.worldData.minimapWidth = 0;
    entry.worldData.minimapHeight = 0;
    entry.worldData.iconX = 0;
    entry.worldData.iconY = 0;
    entry.worldData.entranceFlag = 0;
    entry.worldData.minimapTexture = nullptr;
}

SceneDB::Entry::Entry(const Entry& other) {
    entry = other.entry;
    SetName(other.name);
    SetDesc(other.desc);
    SetTitleCard(other.titleCard);
    SetEponaSpawnPos(other.eponaSpawnPos);
    SetDungeonPalettes(other.dungeonPalettes);
    SetDungeonNameTextures(other.nameEngTexture, other.nameGerTexture, other.nameFraTexture);
    SetDungeonFloors(other.dungeonFloors, other.dungeonFloorInfo);
    SetDungeonRooms(other.dungeonRooms, other.dungeonRoomInfo);
    SetDungeonIntraRoomTransitions(other.dungeonIntraRoomTransitions);
    SetWorldMinimapTexture(other.worldMinimapTexture);
}

SceneDB::Entry& SceneDB::Entry::operator=(const Entry& other) {
    entry = other.entry;
    SetName(other.name);
    SetDesc(other.desc);
    SetTitleCard(other.titleCard);
    SetEponaSpawnPos(other.eponaSpawnPos);
    SetDungeonPalettes(other.dungeonPalettes);
    SetDungeonNameTextures(other.nameEngTexture, other.nameGerTexture, other.nameFraTexture);
    SetDungeonFloors(other.dungeonFloors, other.dungeonFloorInfo);
    SetDungeonRooms(other.dungeonRooms, other.dungeonRoomInfo);
    SetDungeonIntraRoomTransitions(other.dungeonIntraRoomTransitions);
    SetWorldMinimapTexture(other.worldMinimapTexture);
    return *this;
}

void SceneDB::Entry::SetName(const std::string& newName) {
    name = newName;
    entry.name = name.c_str();
}

void SceneDB::Entry::SetDesc(const std::string& newDesc) {
    desc = newDesc;
    entry.desc = desc.c_str();
}

void SceneDB::Entry::SetTitleCard(const std::string& newTitleCard) {
    titleCard = newTitleCard;
    entry.titleCard = titleCard.c_str();
}

void SceneDB::Entry::SetEponaSpawnPos(const std::vector<Vec3s>& newSpawnPos) {
    eponaSpawnPos = newSpawnPos;
    entry.epona.spawnPos = eponaSpawnPos.data();
    entry.epona.numSpawns = eponaSpawnPos.size();
}

void SceneDB::Entry::SetDungeonPalettes(const std::vector<s16>& newDungeonPalettes) {
    dungeonPalettes = newDungeonPalettes;
    entry.dungeonData.palettes = dungeonPalettes.data();
    entry.dungeonData.numPalettes = dungeonPalettes.size();
}

void SceneDB::Entry::SetDungeonNameTextures(const std::string& newNameEngTexture, const std::string& newNameGerTexture,
                                            const std::string& newNameFraTexture) {
    nameEngTexture = newNameEngTexture;
    nameGerTexture = newNameGerTexture;
    nameFraTexture = newNameFraTexture;
    entry.dungeonData.nameEngTexture = nameEngTexture.c_str();
    entry.dungeonData.nameGerTexture = nameGerTexture.c_str();
    entry.dungeonData.nameFraTexture = nameFraTexture.c_str();
}

void SceneDB::Entry::SetDungeonFloors(const std::vector<Init::FloorInit>& newDungeonFloors) {
    assert(newDungeonFloors.size() == 8 || newDungeonFloors.size() == 0);

    dungeonFloors.clear();
    dungeonFloorInfo.clear();

    dungeonFloors.reserve(newDungeonFloors.size());
    dungeonFloorInfo.reserve(newDungeonFloors.size());

    for (size_t i = 0; i < newDungeonFloors.size(); i++) {
        dungeonFloors.push_back(SceneDBFloor{ static_cast<FloorID>(newDungeonFloors[i].id), newDungeonFloors[i].height,
                                              nullptr, 0, nullptr, nullptr, nullptr, 0, nullptr, 0 });
        dungeonFloorInfo.push_back(FloorInfo{ newDungeonFloors[i].palettes, newDungeonFloors[i].mapLeftTexture,
                                              newDungeonFloors[i].mapRightTexture, newDungeonFloors[i].chestMarks,
                                              newDungeonFloors[i].bossMarks });
    }

    for (size_t i = 0; i < dungeonFloors.size(); i++) {
        dungeonFloors[i].rooms = dungeonFloorInfo[i].rooms.data();
        dungeonFloors[i].numRooms = dungeonFloorInfo[i].rooms.size();
        dungeonFloors[i].mapLeftTexture = dungeonFloorInfo[i].mapLeftTexture.c_str();
        dungeonFloors[i].mapRightTexture = dungeonFloorInfo[i].mapRightTexture.c_str();
        dungeonFloors[i].numChestMarks = dungeonFloorInfo[i].chestMarks.size();
        dungeonFloors[i].chestMarks = dungeonFloorInfo[i].chestMarks.data();
        dungeonFloors[i].numBossMarks = dungeonFloorInfo[i].bossMarks.size();
        dungeonFloors[i].bossMarks = dungeonFloorInfo[i].bossMarks.data();
    }
    std::copy(dungeonFloors.begin(), dungeonFloors.end(), entry.dungeonData.floors);
}

void SceneDB::Entry::SetDungeonFloors(const std::vector<SceneDBFloor>& newDungeonFloors,
                                      const std::vector<FloorInfo>& newDungeonFloorInfo) {
    assert(newDungeonFloors.size() == 8 || newDungeonFloors.size() == 0);
    assert(newDungeonFloorInfo.size() == 8 || newDungeonFloorInfo.size() == 0);
    assert(newDungeonFloors.size() == newDungeonFloorInfo.size());
    dungeonFloors = newDungeonFloors;
    dungeonFloorInfo = newDungeonFloorInfo;
    for (size_t i = 0; i < dungeonFloors.size(); i++) {
        dungeonFloors[i].rooms = dungeonFloorInfo[i].rooms.data();
        dungeonFloors[i].numRooms = dungeonFloorInfo[i].rooms.size();
        dungeonFloors[i].mapLeftTexture = dungeonFloorInfo[i].mapLeftTexture.c_str();
        dungeonFloors[i].mapRightTexture = dungeonFloorInfo[i].mapRightTexture.c_str();
        dungeonFloors[i].numChestMarks = dungeonFloorInfo[i].chestMarks.size();
        dungeonFloors[i].chestMarks = dungeonFloorInfo[i].chestMarks.data();
        dungeonFloors[i].numBossMarks = dungeonFloorInfo[i].bossMarks.size();
        dungeonFloors[i].bossMarks = dungeonFloorInfo[i].bossMarks.data();
    }
    std::copy(dungeonFloors.begin(), dungeonFloors.end(), entry.dungeonData.floors);
}

void SceneDB::Entry::SetDungeonRooms(const std::vector<Init::RoomInit>& newDungeonRooms) {
    dungeonRooms.clear();
    dungeonRoomInfo.clear();

    dungeonRooms.reserve(newDungeonRooms.size());
    dungeonRoomInfo.reserve(newDungeonRooms.size());

    for (size_t i = 0; i < newDungeonRooms.size(); i++) {
        dungeonRooms.push_back(SceneDBRoom{ newDungeonRooms[i].compassOffsetX, newDungeonRooms[i].compassOffsetY,
                                            nullptr, nullptr, 0, nullptr, 0 });
        dungeonRoomInfo.push_back(
            RoomInfo{ newDungeonRooms[i].minimapTexture, newDungeonRooms[i].chestMarks, newDungeonRooms[i].bossMarks });
    }

    for (size_t i = 0; i < dungeonRooms.size(); i++) {
        dungeonRooms[i].minimapTexture = dungeonRoomInfo[i].minimapTexture.c_str();
        dungeonRooms[i].chestMarks = dungeonRoomInfo[i].chestMarks.data();
        dungeonRooms[i].numChestMarks = dungeonRoomInfo[i].chestMarks.size();
        dungeonRooms[i].bossMarks = dungeonRoomInfo[i].bossMarks.data();
        dungeonRooms[i].numBossMarks = dungeonRoomInfo[i].bossMarks.size();
    }
    entry.dungeonData.rooms = dungeonRooms.data();
    entry.dungeonData.numRooms = dungeonRooms.size();
}

void SceneDB::Entry::SetDungeonRooms(const std::vector<SceneDBRoom>& newDungeonRooms,
                                     const std::vector<RoomInfo>& newDungeonRoomInfo) {
    dungeonRooms = newDungeonRooms;
    dungeonRoomInfo = newDungeonRoomInfo;
    for (size_t i = 0; i < dungeonRooms.size(); i++) {
        dungeonRooms[i].minimapTexture = dungeonRoomInfo[i].minimapTexture.c_str();
        dungeonRooms[i].chestMarks = dungeonRoomInfo[i].chestMarks.data();
        dungeonRooms[i].numChestMarks = dungeonRoomInfo[i].chestMarks.size();
        dungeonRooms[i].bossMarks = dungeonRoomInfo[i].bossMarks.data();
        dungeonRooms[i].numBossMarks = dungeonRoomInfo[i].bossMarks.size();
    }
    entry.dungeonData.rooms = dungeonRooms.data();
    entry.dungeonData.numRooms = dungeonRooms.size();
}

void SceneDB::Entry::SetDungeonIntraRoomTransitions(
    const std::vector<Init::IntraRoomTransitionInit>& newDungeonIntraRoomTransitions) {
    dungeonIntraRoomTransitions.clear();

    dungeonIntraRoomTransitions.reserve(newDungeonIntraRoomTransitions.size());

    for (size_t i = 0; i < newDungeonIntraRoomTransitions.size(); i++) {
        dungeonIntraRoomTransitions.push_back(SceneDBIntraRoomTransition{ newDungeonIntraRoomTransitions[i].fromRoom,
                                                                          newDungeonIntraRoomTransitions[i].toRoom,
                                                                          newDungeonIntraRoomTransitions[i].toFloor });
    }
    entry.dungeonData.intraRoomTransitions = dungeonIntraRoomTransitions.data();
    entry.dungeonData.numIntraRoomTransitions = dungeonIntraRoomTransitions.size();
}

void SceneDB::Entry::SetDungeonIntraRoomTransitions(
    const std::vector<SceneDBIntraRoomTransition>& newDungeonIntraRoomTransitions) {
    dungeonIntraRoomTransitions = newDungeonIntraRoomTransitions;
    entry.dungeonData.intraRoomTransitions = dungeonIntraRoomTransitions.data();
    entry.dungeonData.numIntraRoomTransitions = dungeonIntraRoomTransitions.size();
}

void SceneDB::Entry::SetWorldMinimapTexture(const std::string& newWorldMinimapTexture) {
    worldMinimapTexture = newWorldMinimapTexture;
    entry.worldData.minimapTexture = worldMinimapTexture.empty() ? nullptr : worldMinimapTexture.c_str();
}

struct MqMapMarkInit {
    struct Room {
        std::vector<MapMarkPoint> chestMarks;
        std::vector<MapMarkPoint> bossMarks;
    };

    std::vector<Room> rooms;
};

std::vector<MqMapMarkInit> mqMapMarkInit = {
    // Deku Tree
    {
        { { { { 3, 71, 50 } }, {} },
          { {}, {} },
          { { { 1, 64, 62 } }, {} },
          { { { 4, 76, 37 } }, {} },
          { {}, {} },
          { { { 0, 46, 50 }, { 5, 76, 52 } }, {} },
          { {}, {} },
          { {}, {} },
          { {}, {} },
          { {}, { { -1, 50, 23 } } },
          { { { 2, 46, 50 }, { 6, 58, 60 } }, {} },
          { {}, {} },
          { {}, {} } },
    },
    // Dodongo's Cavern
    { {
        { { { 0, 69, 14 }, { 4, 69, 30 } }, {} },
        { {}, {} },
        { {}, {} },
        { {}, {} },
        { {}, {} },
        { { { 5, 54, 54 } }, {} },
        { { { 2, 69, 54 } }, {} },
        { {}, { { -1, 37, 49 } } },
        { {}, {} },
        { { { 3, 59, 53 } }, {} },
        { {}, {} },
        { {}, {} },
        { {}, {} },
        { {}, {} },
        { { { 1, 68, 54 } }, {} },
        { {}, {} },
        { {}, {} },
        { {}, {} },
        { {}, {} },
    } },
    // Jabu-Jabu's Belly
    { {
        { { { 3, 66, 50 }, { 5, 72, 47 } }, {} },
        { { { 7, 72, 54 } }, {} },
        { {}, {} },
        { { { 4, 64, 62 }, { 8, 79, 38 } }, {} },
        { {}, {} },
        { { { 10, 64, 45 } }, { { -1, 67, 32 } } },
        { {}, {} },
        { {}, {} },
        { {}, {} },
        { {}, {} },
        { {}, {} },
        { { { 9, 68, 45 } }, {} },
        { {}, {} },
        { {}, {} },
        { { { 1, 79, 33 }, { 6, 61, 41 } }, {} },
        { { { 0, 48, 57 }, { 2, 77, 55 } }, {} },
        // Jabu-Jabu's Belly minimap 16
        // SoH [General] - This entry corresponds to Big Octorok's room and is missing in the MQ game
        // N64 hardware does an OoB read and lands on MQ Forest Temple room 0
        // To avoid UB with OoB for SoH, the correct entry is now added below
        { {}, {} },
    } },
    // Forest Temple
    { {
        { {}, {} },
        { {}, {} },
        { {}, {} },
        { {}, {} },
        { {}, {} },
        { {}, {} },
        { { { 0, 72, 57 } }, {} },
        { { { 1, 69, 39 }, { 9, 62, 65 } }, {} },
        { {}, {} },
        { { { 9, 71, 59 } }, {} },
        { {}, {} },
        { {}, {} },
        { { { 13, 80, 53 } }, {} },
        { { { 15, 49, 50 } }, {} },
        { {}, {} },
        { { { 6, 65, 53 } }, {} },
        { {}, {} },
        { { { 11, 39, 35 } }, { { -1, 53, 5 } } },
        { { { 2, 65, 54 } }, {} },
        { { { 14, 64, 31 } }, {} },
        { {}, {} },
        { {}, {} },
        { { { 3, 75, 53 } }, {} },
        { { { 12, 69, 52 } }, {} },
        { { { 5, 58, 27 } }, {} },
        { {}, {} },
        { {}, {} },
    } },
    // Fire Temple
    { {
        { {}, {} },
        { {}, {} },
        { { { 7, 53, 70 } }, { { -1, 40, 47 } } },
        { {}, {} },
        { { { 11, 57, 48 } }, {} },
        { { { 3, 67, 73 }, { 6, 58, 76 } }, {} },
        { {}, {} },
        { {}, {} },
        { {}, {} },
        { {}, {} },
        { {}, {} },
        { {}, {} },
        { {}, {} },
        { {}, {} },
        { {}, {} },
        { {}, {} },
        { {}, {} },
        { { { 2, 78, 62 }, { 12, 77, 58 } }, {} },
        { {}, {} },
        { { { 4, 60, 54 } }, {} },
        { {}, {} },
        { {}, {} },
        { { { 1, 72, 68 } }, {} },
        { { { 8, 66, 57 } }, {} },
        { {}, {} },
        { {}, {} },
        { {}, {} },
        { {}, {} },
        { {}, {} },
        { {}, {} },
        { {}, {} },
        { {}, {} },
        { {}, {} },
        { {}, {} },
        { {}, {} },
        { { { 5, 51, 61 } }, {} },
        { {}, {} },
        { {}, {} },
    } },
    // Water Temple
    { {
        { {}, {} },
        { {}, {} },
        { { { 6, 81, 68 } }, {} },
        { {}, {} },
        { {}, {} },
        { {}, {} },
        { {}, {} },
        { {}, {} },
        { {}, {} },
        { { { 5, 75, 55 } }, {} },
        { {}, {} },
        { {}, { { -1, 77, 40 } } },
        { {}, {} },
        { {}, {} },
        { {}, {} },
        { {}, {} },
        { {}, {} },
        { {}, {} },
        { { { 1, 74, 61 } }, {} },
        { { { 2, 73, 65 } }, {} },
        { {}, {} },
        { {}, {} },
        { {}, {} },
        { {}, {} },
        { {}, {} },
        { {}, {} },
        { {}, {} },
        { {}, {} },
        { {}, {} },
        { {}, {} },
        { {}, {} },
        { {}, {} },
        { {}, {} },
        { {}, {} },
        { {}, {} },
        { {}, {} },
        { {}, {} },
        { {}, {} },
        { {}, {} },
        { { { 0, 73, 63 } }, {} },
        { {}, {} },
        { {}, {} },
        { {}, {} },
        { {}, {} },
    } },
    // Spirit Temple
    { {
        { { { 26, 27, 35 }, { 27, 36, 35 }, { 30, 27, 28 }, { 31, 36, 28 } }, {} },
        { { { 29, 67, 63 } }, {} },
        { {}, {} },
        { { { 0, 71, 62 }, { 8, 71, 48 } }, {} },
        { {}, {} },
        { { { 3, 56, 54 }, { 15, 69, 42 } }, {} },
        { { { 28, 60, 54 } }, {} },
        { {}, {} },
        { { { 1, 76, 40 } }, {} },
        { {}, {} },
        { {}, {} },
        { {}, {} },
        { { { 7, 70, 53 } }, {} },
        { {}, {} },
        { { { 4, 68, 42 } }, {} },
        { {}, {} },
        { {}, {} },
        { { { 25, 78, 58 } }, {} },
        { { { 24, 78, 58 } }, {} },
        { {}, {} },
        { {}, {} },
        { { { 5, 71, 55 } }, {} },
        { {}, {} },
        { {}, {} },
        { {}, {} },
        { {}, {} },
        { { { 18, 75, 54 } }, {} },
        { {}, {} },
        { { { 6, 78, 55 }, { 12, 70, 70 } }, {} },
        { { { 2, 76, 37 } }, { { -1, 57, 23 } } },
        { {}, {} },
        { {}, {} },
        { {}, {} },
    } },
    // Shadow Temple
    { {
        { {}, {} },
        { { { 1, 77, 64 } }, {} },
        { {}, {} },
        { {}, { { -1, 77, 76 } } },
        { { { 7, 76, 65 } }, {} },
        { {}, {} },
        { { { 2, 83, 67 }, { 14, 84, 59 } }, {} },
        { { { 3, 76, 67 } }, {} },
        { {}, {} },
        { {}, {} },
        { { { 4, 78, 62 }, { 5, 74, 62 }, { 6, 71, 68 } }, {} },
        { { { 9, 77, 64 } }, {} },
        { {}, {} },
        { { { 10, 71, 65 }, { 11, 80, 65 } }, {} },
        { { { 16, 73, 64 } }, {} },
        { {}, {} },
        { { { 12, 87, 64 }, { 22, 87, 68 } }, {} },
        { { { 13, 77, 66 } }, {} },
        { {}, {} },
        { { { 21, 78, 66 } }, {} },
        { { { 8, 76, 66 }, { 20, 78, 68 } }, {} },
        { { { 14, 77, 62 } }, {} },
        { {}, {} },
        { { { 15, 56, 67 } }, {} },
        { {}, {} },
        { { { 16, 73, 64 } }, {} },
        { { { 14, 77, 62 } }, {} },
    } },
    // Bottom of the Well
    { {
        { { { 3, 60, 18 } }, {} },
        { {}, {} },
        { {}, {} },
        { {}, {} },
        { { { 2, 73, 61 } }, {} },
        { {}, {} },
        { {}, {} },
        { {}, {} },
        { {}, {} },
        { { { 1, 74, 66 } }, {} },
    } },
    // Ice Cavern
    { {
        { {}, {} },
        { {}, {} },
        { {}, {} },
        { {}, {} },
        { {}, {} },
        { {}, {} },
        { {}, {} },
        { { { 2, 71, 59 } }, {} },
        { {}, {} },
        { { { 0, 48, 36 } }, {} },
        { {}, {} },
        { { { 1, 73, 67 } }, {} },
    } },
};

// Sets a scene's dungeon map marks based on whether it should be MQ or not
void SceneDB::Entry::SetMapMarkData(const bool isMQ) {
    if (isMQ) {
        if (this->entry.id >= SCENE_DEKU_TREE && this->entry.id <= SCENE_ICE_CAVERN) {
            s32 dungeonNum = this->entry.id - SCENE_DEKU_TREE;
            for (size_t i = 0; i < mqMapMarkInit[dungeonNum].rooms.size(); i++) {
                dungeonRoomInfo[i].chestMarks = mqMapMarkInit[dungeonNum].rooms[i].chestMarks;
                dungeonRoomInfo[i].bossMarks = mqMapMarkInit[dungeonNum].rooms[i].bossMarks;

                dungeonRooms[i].chestMarks = dungeonRoomInfo[i].chestMarks.data();
                dungeonRooms[i].numChestMarks = dungeonRoomInfo[i].chestMarks.size();
                dungeonRooms[i].bossMarks = dungeonRoomInfo[i].bossMarks.data();
                dungeonRooms[i].numBossMarks = dungeonRoomInfo[i].bossMarks.size();
            }
        }
    } else {
        SetDungeonRooms(sceneDBInit[this->entry.id].dungeonData.rooms);
    }
}

struct MqPauseMapMarkInit {
    struct Floor {
        std::vector<PauseMapMarkPoint> chestMarks;
        std::vector<PauseMapMarkPoint> bossMarks;
    };

    Floor floors[8];
};

std::vector<MqPauseMapMarkInit> mqPauseMapMarkInit = {
    // Deku Tree
    { {
        { {}, {} },
        { {}, {} },
        { {}, {} },
        { { { 2, 40.0f, -33.0f }, { 6, 49.0f, -42.0f } }, {} },                       // 3F
        { { { 1, 48.0f, -63.0f } }, {} },                                             // 2F
        { { { 3, 84.0f, -39.0f } }, {} },                                             // 1F
        { { { 0, 46.0f, -59.0f }, { 4, 77.0f, -26.0f }, { 5, 65.0f, -61.0f } }, {} }, // B1
        { {}, { { -1, 55.0f, 0.0f } } },                                              // B2
    } },
    // Dodongo's Cavern
    { {
        { {}, {} },
        { {}, {} },
        { {}, {} },
        { {}, {} },
        { {}, {} },
        { {}, {} },
        { { { 2, 55.0f, -36.0f }, { 3, 54.0f, -51.0f }, { 5, 13.0f, -61.0f } }, {} },                       // 2F
        { { { 0, 47.0f, -40.0f }, { 1, 51.0f, -3.0f }, { 4, 47.0f, -47.0f } }, { { -1, 23.0f, -25.0f } } }, // 1F
    } },
    // Jabu-Jabu's Belly
    { {
        { {}, {} },
        { {}, {} },
        { {}, {} },
        { {}, {} },
        { {}, {} },
        { {}, {} },
        { { { 3, 48.0f, -68.0f },
            { 5, 50.0f, -66.0f },
            { 7, 55.0f, -50.0f },
            { 9, 58.0f, 1.0f },
            { 10, 62.0f, -45.0f } },
          { { -1, 65.0f, -37.0f } } }, // 1F
        { { { 0, 37.0f, -49.0f },
            { 1, 65.0f, -38.0f },
            { 2, 52.0f, -48.0f },
            { 4, 46.0f, -36.0f },
            { 6, 59.0f, -41.0f },
            { 8, 52.0f, -26.0f } },
          {} }, // B1
    } },
    // Forest Temple
    { {
        { {}, {} },
        { {}, {} },
        { {}, {} },
        { {}, {} },
        { { { 3, 53.0f, -64.0f },
            { 5, 65.0f, -9.0f },
            { 12, 49.0f, -1.0f },
            { 13, 40.0f, 0.0f },
            { 14, 18.0f, -2.0f },
            { 15, 59.0f, 0.0f } },
          {} },                                                                                            // 2F
        { { { 0, 49.0f, -1.0f }, { 1, 71.0f, -13.0f }, { 2, 11.0f, -25.0f }, { 6, 84.0f, -16.0f } }, {} }, // 1F
        { { { 9, 65.0f, -30.0f } }, {} },                                                                  // B1
        { { { 11, 41.0f, -24.0f } }, { { -1, 50.0f, -11.0f } } },                                          // B2
    } },
    // Fire Temple
    { {
        { {}, {} },
        { {}, {} },
        { {}, {} },
        { { { 5, 24.0f, -40.0f } }, {} },                                             // 5F
        { {}, {} },                                                                   // 4F
        { { { 3, 75.0f, -47.0f }, { 6, 72.0f, -51.0f }, { 8, 65.0f, -12.0f } }, {} }, // 3F
        { { { 11, 78.0f, -35.0f } }, {} },                                            // 2F
        { { { 1, 67.0f, -58.0f },
            { 2, 48.0f, -30.0f },
            { 4, 63.0f, -14.0f },
            { 7, 36.0f, -45.0f },
            { 12, 47.0f, -26.0f } },
          { { -1, 26.0f, -34.0f } } }, // 1F
    } },
    // Water Temple
    { {
        { {}, {} },
        { {}, {} },
        { {}, {} },
        { {}, {} },
        { { { 2, 88.0f, -60.0f } }, { { -1, 62.0f, -23.0f } } }, // 3F
        { { { 0, 88.0f, -60.0f } }, {} },                        // 2F
        { { { 1, 88.0f, -60.0f }, { 5, 49.0f, -43.0f } }, {} },  // 1F
        { { { 6, 75.0f, -65.0f } }, {} },                        // B1
    } },
    // Spirit Temple
    { {
        { {}, {} },
        { {}, {} },
        { {}, {} },
        { {}, {} },
        { { { 18, 46.0f, -30.0f } }, {} }, // 4F
        { { { 1, 23.0f, -33.0f },
            { 2, 56.0f, -11.0f },
            { 5, 83.0f, -25.0f },
            { 24, 84.0f, -39.0f },
            { 25, 74.0f, -37.0f } },
          { { -1, 47.0f, 0.0f } } }, // 3F
        { { { 3, 46.0f, -20.0f },
            { 6, 28.0f, -19.0f },
            { 12, 25.0f, -25.0f },
            { 15, 50.0f, -13.0f },
            { 28, 48.0f, -29.0f } },
          {} }, // 2F
        { { { 0, 14.0f, -24.0f },
            { 4, 55.0f, -14.0f },
            { 7, 78.0f, -2.0f },
            { 8, 14.0f, -16.0f },
            { 26, 42.0f, -43.0f },
            { 27, 50.0f, -43.0f },
            { 29, 25.0f, -35.0f },
            { 30, 42.0f, -36.0f },
            { 31, 50.0f, -36.0f } },
          {} }, // 1F
    } },
    // Shadow Temple
    { {
        { {}, {} },
        { {}, {} },
        { {}, {} },
        { {}, {} },
        { { { 1, 41.0f, -17.0f }, { 7, 27.0f, -24.0f } }, {} },                          // B1
        { { { 2, 81.0f, -20.0f }, { 3, 74.0f, -37.0f } }, {} },                          // B2
        { { { 12, 96.0f, -51.0f }, { 16, 46.0f, -42.0f }, { 22, 96.0f, -55.0f } }, {} }, // B3
        { { { 4, 43.0f, -66.0f },
            { 5, 37.0f, -66.0f },
            { 6, 33.0f, -72.0f },
            { 8, 85.0f, -18.0f },
            { 9, 61.0f, -42.0f },
            { 10, 15.0f, -4.0f },
            { 11, 25.0f, -4.0f },
            { 13, 19.0f, -29.0f },
            { 14, 78.0f, -15.0f },
            { 15, 60.0f, -70.0f },
            { 21, 92.0f, -29.0f },
            { 20, 87.0f, -20.0f } },
          { { -1, 31.0f, -45.0f } } }, // B4
    } },
    // Bottom of the Well
    { {
        { {}, {} },
        { {}, {} },
        { {}, {} },
        { {}, {} },
        { {}, {} },
        { { { 2, 84.0f, -38.0f }, { 3, 57.0f, -18.0f } }, {} }, // B1
        { {}, {} },                                             // B2
        { { { 1, 72.0f, -32.0f } }, {} },                       // B3
    } },
    // Ice Cavern
    { {
        { {}, {} },
        { {}, {} },
        { {}, {} },
        { {}, {} },
        { {}, {} },
        { {}, {} },
        { {}, {} },
        { { { 0, 66.0f, -2.0f }, { 1, 77.0f, -46.0f }, { 2, 27.0f, -45.0f } }, {} }, // 1F
    } },
};

// Sets a scene's pause map marks based on whether it should be MQ or not
void SceneDB::Entry::SetPauseMapMarkData(const bool isMQ) {
    if (isMQ) {
        if (this->entry.id >= SCENE_DEKU_TREE && this->entry.id <= SCENE_ICE_CAVERN) {
            s32 dungeonNum = this->entry.id - SCENE_DEKU_TREE;
            for (size_t i = 0; i < 8; i++) {
                dungeonFloorInfo[i].chestMarks = mqPauseMapMarkInit[dungeonNum].floors[i].chestMarks;
                dungeonFloorInfo[i].bossMarks = mqPauseMapMarkInit[dungeonNum].floors[i].bossMarks;

                dungeonFloors[i].numChestMarks = dungeonFloorInfo[i].chestMarks.size();
                dungeonFloors[i].chestMarks = dungeonFloorInfo[i].chestMarks.data();
                dungeonFloors[i].numBossMarks = dungeonFloorInfo[i].bossMarks.size();
                dungeonFloors[i].bossMarks = dungeonFloorInfo[i].bossMarks.data();
            }
        }
    } else {
        SetDungeonFloors(sceneDBInit[this->entry.id].dungeonData.floors);
    }
}

// Returns is a scene is a boss scene
// A scene is a boss scene if it has the scene in the bossData is != -1
bool SceneDB::Entry::isBoss() {
    return SCENEDB_ISBOSS(&this->entry);
}

// Returns is a scene is a boss dungeon
// A scene is a dungeon scene if it's number of rooms is != 0
bool SceneDB::Entry::isDungeon() {
    return SCENEDB_ISDUNGEON(&this->entry);
}

// Returns is a scene is a overworld scene
// A scene is a world scene if it has a texture for a world minimap
bool SceneDB::Entry::isOverworld() {
    return SCENEDB_ISOVERWORLD(&this->entry);
}

EntranceDB::EntranceDB() {
    db.reserve(ENTR_MAX); // reserve size for all initial entries so we don't do it for each
    for (const EntranceTableInit& init : entranceDBInit) {
        Init nativeInit{ init.name,          init.desc,           SceneDB::Instance->RetrieveEntry(init.scene).name,
                         init.spawn,         init.continueBgm,    init.displayTitleCard,
                         init.endTransition, init.startTransition };

        AddEntry(nativeInit);
    }
}

EntranceDB::Entry& EntranceDB::AddEntry(const Init& init) {
    Entry& entry = AddEntry(init.name, init.desc, nextFreeId);
    // TODO handle errors
    entry.entry.sceneId = SceneDB::Instance->RetrieveEntry(SceneDB::Instance->RetrieveId(init.scene)).entry.id;
    entry.entry.spawn = init.spawn;
    entry.entry.continueBgm = init.continueBgm;
    entry.entry.displayTitleCard = init.displayTitleCard;
    entry.entry.endTransition = init.endTransition;
    entry.entry.startTransition = init.startTransition;

    // Add this entrance to the list of layers for the scene and spawn
    NextLayerLookupKey nextLayerKey{ entry.entry.sceneId, entry.entry.spawn };
    auto nextLayer = nextLayerLookupTable.find(nextLayerKey);
    if (nextLayer == nextLayerLookupTable.end()) {
        entry.entry.layer = 0;
        nextLayerLookupTable.insert({ nextLayerKey, 1 });
    } else {
        entry.entry.layer = nextLayer->second++;
    }

    // Add the entrance to the lookup of scene, spawn, and layer -> base scene
    idLookupTable.insert({ { entry.entry.sceneId, entry.entry.spawn, entry.entry.layer }, entry.entry.id });

    return entry;
}

EntranceDB::Entry& EntranceDB::RetrieveEntry(const int id) {
    static Entry invalid;
    if ((id < 0) || (id >= db.size())) {
        return invalid;
    }
    return db[id];
}

EntranceDB::Entry& EntranceDB::RetrieveEntry(const int id, const int layer) {
    Entry& entry = RetrieveEntry(id);
    Entry& ret = RetrieveEntry(CalcId(entry.entry.sceneId, entry.entry.spawn, layer));
    assert(ret.entry.valid);
    return ret;
}

int EntranceDB::RetrieveId(const std::string& name) {
    auto entry = nameTable.find(name);
    if (entry == nameTable.end()) {
        return -1;
    }

    return entry->second;
}

size_t EntranceDB::GetNumEntries() {
    return db.size();
}

s32 EntranceDB::CalcId(const s32 sceneId, const s32 spawn, const s32 layer) {
    auto entrance = idLookupTable.find({ sceneId, spawn, layer });

    // If the entrance does not exist, assume it is a wrong warp
    // TODO WW CVar to force error for mod debugging
    if (entrance == idLookupTable.end()) {
        auto wwEntrance = idLookupTable.find({ sceneId, spawn, 0 });
        // We need to make sure that at least the scene and spawn are correct, it's just the layer which is an OOB
        assert(wwEntrance != idLookupTable.end());
        return wwEntrance->second + layer;
    }

    return entrance->second;
}

s32 EntranceDB::CalcId(const s32 entrance, const s32 newLayer) {
    Entry& entry = RetrieveEntry(entrance);
    auto newEntrance = idLookupTable.find({ entry.entry.sceneId, entry.entry.spawn, newLayer });

    // If the entrance does not exist, assume it is a wrong warp
    // TODO WW CVar to force error for mod debugging
    if (newEntrance == idLookupTable.end()) {
        auto wwEntrance = idLookupTable.find({ entry.entry.sceneId, entry.entry.spawn, 0 });
        // We need to make sure that at least the scene and spawn are correct, it's just the layer which is an OOB
        assert(wwEntrance != idLookupTable.end());
        return wwEntrance->second + newLayer;
    }

    return newEntrance->second;
}

// Resets all vanilla entrances (mostly to restore all modifications rando makes)
void EntranceDB::ResetVanillaEntrances() {
    // Erase all instances of vanilla entrances in the lookup tables
    for (size_t i = 0; i < ENTR_MAX; i++) {
        // For this entrance, reset the next layer. This will cause problems with mods that add a new layer (like for a
        // cutscene) However, we will treat this as fine for now because we don't have such mods A more robust solution
        // is to use a local nextLayerLookupTable to reestablish the vanilla layers
        NextLayerLookupKey nextLayerKey{ entranceDBInit[i].scene, entranceDBInit[i].spawn };
        nextLayerLookupTable.erase(nextLayerKey);

        for (auto iter = idLookupTable.begin(); iter != idLookupTable.end();) {
            if (iter->second == i) {
                idLookupTable.erase(iter++);
            } else {
                iter++;
            }
        }
    }

    for (size_t i = 0; i < ENTR_MAX; i++) {
        // This entrance should already exist. Look it up and set its values to vanilla
        Entry& entry = RetrieveEntry(i);
        const EntranceTableInit& init = entranceDBInit[i];

        entry.entry.sceneId = SceneDB::Instance->RetrieveEntry(init.scene).entry.id;
        entry.entry.spawn = init.spawn;
        entry.entry.continueBgm = init.continueBgm;
        entry.entry.displayTitleCard = init.displayTitleCard;
        entry.entry.endTransition = init.endTransition;
        entry.entry.startTransition = init.startTransition;

        // Add this entrance to the list of layers for the scene and spawn
        NextLayerLookupKey nextLayerKey{ entry.entry.sceneId, entry.entry.spawn };
        auto nextLayer = nextLayerLookupTable.find(nextLayerKey);
        if (nextLayer == nextLayerLookupTable.end()) {
            entry.entry.layer = 0;
            nextLayerLookupTable.insert({ nextLayerKey, 1 });
        } else {
            entry.entry.layer = nextLayer->second++;
        }

        // Add the entrance to the lookup
        idLookupTable.insert({ { entry.entry.sceneId, entry.entry.spawn, entry.entry.layer }, entry.entry.id });
    }
}

// Copies an entrance. Needed by rando when making changes to the scene, spawn, or layer
void EntranceDB::Copy(s32 from, s32 to) {
    Entry& entryFrom = RetrieveEntry(from);
    Entry& entryTo = RetrieveEntry(to);

    // Remove entrance that is about to be overridden
    for (auto iter = idLookupTable.begin(); iter != idLookupTable.end();) {
        if (iter->second == to) {
            idLookupTable.erase(iter++);
        } else {
            iter++;
        }
    }

    // Add a lookup so that the overridden entrance returns the new one
    idLookupTable.insert({ { entryTo.entry.sceneId, entryTo.entry.spawn, entryTo.entry.layer }, entryFrom.entry.id });

    // Replace the entrance
    entryTo.entry.sceneId = entryFrom.entry.sceneId;
    entryTo.entry.spawn = entryFrom.entry.spawn;
    entryTo.entry.layer = entryFrom.entry.layer;
    entryTo.entry.continueBgm = entryFrom.entry.continueBgm;
    entryTo.entry.displayTitleCard = entryFrom.entry.displayTitleCard;
    entryTo.entry.endTransition = entryFrom.entry.endTransition;
    entryTo.entry.startTransition = entryFrom.entry.startTransition;
}

EntranceDB::Entry& EntranceDB::AddEntry(const std::string& name, const std::string& desc, size_t index) {
    assert(!nameTable.contains(name)); // TODO this should maybe throw instead. We'll need to think about error handling
                                       // for mods that try to declare the same entrance.

    if (db.size() < (index + 1)) {
        db.resize(index + 1);
    }
    Entry& newEntry = db.at(index);
    newEntry.entry.id = index;

    assert(!newEntry.entry.valid);

    nextFreeId = std::max(nextFreeId, index + 1);

    nameTable[name] = newEntry.entry.id;
    newEntry.SetName(name);
    newEntry.SetDesc(desc);

    newEntry.entry.valid = true;

    return newEntry;
}

EntranceDB::Entry::Entry() {
    entry.name = nullptr;
    entry.desc = nullptr;
    entry.valid = false;
    entry.id = 0;
    entry.sceneId = 0;
    entry.spawn = 0;
    entry.layer = 0;
    entry.continueBgm = false;
    entry.displayTitleCard = false;
    entry.endTransition = 0;
    entry.startTransition = 0;
}

EntranceDB::Entry::Entry(const Entry& other) {
    entry = other.entry;
    SetName(other.name);
    SetDesc(other.desc);
}

EntranceDB::Entry& EntranceDB::Entry::operator=(const Entry& other) {
    entry = other.entry;
    SetName(other.name);
    SetDesc(other.desc);
    return *this;
}

void EntranceDB::Entry::SetName(const std::string& newName) {
    name = newName;
    entry.name = name.c_str();
}

void EntranceDB::Entry::SetDesc(const std::string& newDesc) {
    desc = newDesc;
    entry.desc = desc.c_str();
}

extern "C" SceneDBEntry* SceneDB_Retrieve(const int id) {
    return &SceneDB::Instance->RetrieveEntry(id).entry;
}

extern "C" int SceneDB_RetrieveId(const char* name) {
    return SceneDB::Instance->RetrieveId(name);
}

extern "C" int SceneDB_IsBoss(const int id) {
    return SceneDB::Instance->RetrieveEntry(id).isBoss();
}

extern "C" int SceneDB_IsDungeon(const int id) {
    return SceneDB::Instance->RetrieveEntry(id).isDungeon();
}

extern "C" int SceneDB_IsOverworld(const int id) {
    return SceneDB::Instance->RetrieveEntry(id).isOverworld();
}

extern "C" void SceneDB_SetMapMarkData(const int id, const int isMQ) {
    SceneDB::Instance->RetrieveEntry(id).SetMapMarkData(isMQ);
}

extern "C" void SceneDB_SetPauseMapMarkData(const int id, const int isMQ) {
    SceneDB::Instance->RetrieveEntry(id).SetPauseMapMarkData(isMQ);
}

extern "C" EntranceDBEntry* EntranceDB_Retrieve(const int id) {
    return &EntranceDB::Instance->RetrieveEntry(id).entry;
}

extern "C" EntranceDBEntry* EntranceDB_RetrieveLayer(const int id, const int layer) {
    return &EntranceDB::Instance->RetrieveEntry(id, layer).entry;
}

extern "C" int EntranceDB_RetrieveId(const char* name) {
    return EntranceDB::Instance->RetrieveId(name);
}

extern "C" int EntranceDB_CalcId(const int sceneId, const int spawn, const int layer) {
    return EntranceDB::Instance->CalcId(sceneId, spawn, layer);
}

extern "C" int EntranceDB_CalcIdWithEntrance(const int entrance, const int newLayer) {
    return EntranceDB::Instance->CalcId(entrance, newLayer);
}

extern "C" void EntranceDB_ResetVanillaEntrances(void) {
    return EntranceDB::Instance->ResetVanillaEntrances();
}

extern "C" void EntranceDB_Copy(const int from, const int to) {
    return EntranceDB::Instance->Copy(from, to);
}
