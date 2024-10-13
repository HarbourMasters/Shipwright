#pragma once
#include <libultraship/libultra.h>
#include "z64scene.h"
#include "z64.h"

typedef enum {
    SCENETYPE_OVERWORLD,
    SCENETYPE_DUNGEON,
    SCENETYPE_BOSS,
    SCENETYPE_OTHER
} SceneType;

typedef struct {
    FloorID id;
    f32 height;
    s8* rooms;
    u32 numRooms;
    const char* mapLeftTexture;
    const char* mapRightTexture;
    PauseMapMarkPoint* chestMarks;
    u32 numChestMarks;
    PauseMapMarkPoint* bossMarks;
    u32 numBossMarks;
} SceneDBFloor;

typedef struct {
    s16 compassOffsetX;
    s16 compassOffsetY;
    const char* minimapTexture;
    MapMarkPoint* chestMarks;
    u32 numChestMarks;
    MapMarkPoint* bossMarks;
    u32 numBossMarks;
} SceneDBRoom;

typedef struct {
    u8 fromRoom;
    u8 toRoom;
    u8 toFloor;
} SceneDBIntraRoomTransition;

typedef struct {
    const char* name;
    const char* desc;
    u32 valid;
    s32 id;
    const char* titleCard;
    SceneDrawConfig sceneDrawConfig;
    SceneType type;
    struct {
        u8 hGauge;
        u8 bButton;
        u8 aButton;
        u8 bottles;
        u8 tradeItems;
        u8 hookshot;
        u8 ocarina;
        u8 warpSongs;
        u8 sunsSong;
        u8 farores;
        u8 dinsNayrus;
        u8 all;
    } restrictions;
    struct {
        u32 allowed;
        Vec3s startingPos;
        s16 angle;
        Vec3s* spawnPos;
        u32 numSpawns;
    } epona;
    struct {
        s16 scaleX;
        s16 scaleY;
        s16 x;
        s16 y;
    } compassInfo;
    struct {
        s32 mapScene;
    } bossData;
    struct {
        s32 bossFloor;
        s16* palettes;
        u32 numPalettes;
        const char* nameEngTexture;
        const char* nameGerTexture;
        const char* nameFraTexture;
        SceneDBFloor floors[8];
        SceneDBRoom* rooms;
        u32 numRooms;
        SceneDBIntraRoomTransition* intraRoomTransitions;
        u32 numIntraRoomTransitions;
    } dungeonData;
    struct {
        s16 minimapX;
        s16 minimapY;
        s16 minimapWidth;
        s16 minimapHeight;
        s16 iconX;
        s16 iconY;
        u16 entranceFlag;
        const char* minimapTexture;
    } worldData;
} SceneDBEntry;

typedef struct {
    const char* name;
    const char* desc;
    u32 valid;
    s32 id;
    s32 sceneId;
    s32 spawn;
    s32 layer;
    u8 continueBgm;
    u8 displayTitleCard;
    u8 endTransition;
    u8 startTransition;
} EntranceDBEntry;

#ifdef __cplusplus

#include <string>
#include <unordered_map>
#include <vector>
#include <map>

// TODO move into class
struct SceneDBInit {
    struct FloorInit {
        u8 id = 0; // TODO FloorID?
        f32 height = 9999.0f;
        std::vector<s8> palettes;
        std::string mapLeftTexture;
        std::string mapRightTexture;
        std::vector<PauseMapMarkPoint> chestMarks;
        std::vector<PauseMapMarkPoint> bossMarks;
    };

    struct RoomInit {
        s16 compassOffsetX = 0;
        s16 compassOffsetY = 0;
        std::string minimapTexture;
        std::vector<MapMarkPoint> chestMarks;
        std::vector<MapMarkPoint> bossMarks;
    };

    struct IntraRoomTransitionInit {
        u8 fromRoom = 0xFF;
        u8 toRoom = 0xFF;
        u8 toFloor = 0xFF;
    };

    std::string name;
    std::string desc;
    std::string titleCard;
    SceneDrawConfig sceneDrawConfig = SDC_DEFAULT;
    SceneType type = SCENETYPE_OTHER;
    struct {
        bool hGauge = false;
        bool bButton = false;
        bool aButton = false;
        bool bottles = false;
        bool tradeItems = false;
        bool hookshot = false;
        bool ocarina = false;
        bool warpSongs = false;
        bool sunsSong = false;
        bool farores = false;
        bool dinsNayrus = false;
        bool all = false;
    } restrictions;
    struct {
        bool allowed = false;
        Vec3s startingPos = {0, 0, 0};
        s16 angle = 0;
        std::vector<Vec3s> spawnPos;
    } epona;
    struct {
        s16 scaleX = 0;
        s16 scaleY = 0;
        s16 x = 0;
        s16 y = 0;
    } compassInfo;
    struct {
        std::string mapScene;
    } bossData;
    struct {
        s32 bossFloor = -1;
        std::vector<s16> palettes;
        std::string nameEngTexture;
        std::string nameGerTexture;
        std::string nameFraTexture;
        std::vector<FloorInit> floors;
        std::vector<RoomInit> rooms;
        std::vector<IntraRoomTransitionInit> intraRoomTransitions;
    } dungeonData;
    struct {
        s16 minimapX = 0;
        s16 minimapY = 0;
        s16 minimapWidth = 0;
        s16 minimapHeight = 0;
        s16 iconX = 0;
        s16 iconY = 0;
        u16 entranceFlag = 0xFFFF;
        std::string minimapTexture;
    } worldData;
};

struct EntranceDBInit {
    std::string name;
    std::string desc;
    std::string scene;
    s32 spawn; // TODO string?
    bool continueBgm;
    bool displayTitleCard;
    u8 endTransition;
    u8 startTransition;
};

class SceneDB {
public:
    static SceneDB* Instance;

    SceneDB();

    // Wrapper around SceneDBEntry so we get C++isms for the entries
    // TODO structs for strings and vectors
    struct Entry {
        Entry();
        Entry(const Entry& other);
        Entry& operator=(const Entry& other);

        struct FloorInfo {
            std::vector<s8> rooms;
            std::string mapLeftTexture;
            std::string mapRightTexture;
            std::vector<PauseMapMarkPoint> chestMarks;
            std::vector<PauseMapMarkPoint> bossMarks;
        };

        struct RoomInfo {
            std::string minimapTexture;
            std::vector<MapMarkPoint> chestMarks;
            std::vector<MapMarkPoint> bossMarks;
        };

        void SetName(const std::string& newName);
        void SetDesc(const std::string& newDesc);
        void SetTitleCard(const std::string& newTitleCard);
        void SetEponaSpawnPos(const std::vector<Vec3s>& newSpawnPos);
        void SetDungeonPalettes(const std::vector<s16>& newDungeonPalettes);
        void SetDungeonNameTextures(const std::string& newNameEngTexture, const std::string& newNameGerTexture, const std::string& newNameFraTexture);
        void SetDungeonFloors(const std::vector<SceneDBInit::FloorInit>& newDungeonFloors);
        void SetDungeonFloors(const std::vector<SceneDBFloor>& newDungeonFloors, const std::vector<FloorInfo>& newDungeonFloorInfo);
        void SetDungeonRooms(const std::vector<SceneDBInit::RoomInit>& newDungeonRooms);
        void SetDungeonRooms(const std::vector<SceneDBRoom>& newDungeonRooms, const std::vector<RoomInfo>& newDungeonRoomInfo);
        void SetDungeonIntraRoomTransitions(const std::vector<SceneDBInit::IntraRoomTransitionInit>& newDungeonIntraRoomTransitions);
        void SetDungeonIntraRoomTransitions(const std::vector<SceneDBIntraRoomTransition>& newDungeonIntraRoomTransitions);
        void SetWorldMinimapTexture(const std::string& newWorldMinimapTexture);
        void SetMapMarkData(const bool isMQ);
        void SetPauseMapMarkData(const bool isMQ);

        bool isBoss();
        bool isDungeon();
        bool isOverworld();

        std::string name;
        std::string desc;
        std::string titleCard;
        std::vector<Vec3s> eponaSpawnPos;
        std::vector<s16> dungeonPalettes;
        std::string nameEngTexture;
        std::string nameGerTexture;
        std::string nameFraTexture;
        std::vector<FloorInfo> dungeonFloorInfo;
        std::vector<SceneDBFloor> dungeonFloors;
        std::vector<RoomInfo> dungeonRoomInfo;
        std::vector<SceneDBRoom> dungeonRooms;
        std::vector<SceneDBIntraRoomTransition> dungeonIntraRoomTransitions;
        std::string worldMinimapTexture;
        SceneDBEntry entry;
    };
    Entry& AddEntry(const SceneDBInit& init);

    Entry& RetrieveEntry(const int id);
    int RetrieveId(const std::string& name);

    size_t GetNumEntries();

private:
    Entry& AddEntry(const std::string& name, const std::string& desc, size_t index);

    std::vector<Entry> db;
    std::unordered_map<std::string, int> nameTable;
    size_t nextFreeId = 0;
};

class EntranceDB {
public:
    static EntranceDB* Instance;

    EntranceDB();

    // Wrapper around EntranceDBEntry so we get C++isms for the entries
    struct Entry {
        Entry();
        Entry(const Entry& other);
        Entry& operator=(const Entry& other);

        void SetName(const std::string& newName);
        void SetDesc(const std::string& newDesc);

        std::string name;
        std::string desc;
        EntranceDBEntry entry;
    };
    Entry& AddEntry(const EntranceDBInit& init);

    Entry& RetrieveEntry(const int id);
    Entry& RetrieveEntry(const int id, const int layer);
    int RetrieveId(const std::string& name);

    size_t GetNumEntries();

    s32 CalcId(s32 sceneId, s32 spawn, s32 layer);
    s32 CalcId(s32 entrance, s32 newLayer);
private:
    Entry& AddEntry(const std::string& name, const std::string& desc, size_t index);

    std::vector<Entry> db;
    std::unordered_map<std::string, int> nameTable;
    size_t nextFreeId = 0;

    struct IdLookupKey {
        s32 sceneId;
        s32 spawn;
        s32 layer;
        auto operator<=>(const IdLookupKey&) const = default;
    };
    std::map<IdLookupKey, s32> idLookupTable;

    struct NextLayerLookupKey {
        s32 sceneId;
        s32 spawn;
        auto operator<=>(const NextLayerLookupKey&) const = default;
    };
    std::map<NextLayerLookupKey, s32> nextLayerLookupTable;
};

#else

#define SCENEDB_ISBOSS(entry) ((entry)->bossData.mapScene != -1)
#define SCENEDB_ISDUNGEON(entry) ((entry)->dungeonData.numRooms != 0)
// TODO is this a good one?
#define SCENEDB_ISOVERWORLD(entry) ((entry)->worldData.minimapTexture != NULL)

SceneDBEntry* SceneDB_Retrieve(const int id);
int SceneDB_RetrieveId(const char* name);
int SceneDB_IsBoss(const int id);
int SceneDB_IsDungeon(const int id);
int SceneDB_IsOverworld(const int id);
void SceneDB_SetMapMarkData(const int id, const int isMQ);
void SceneDB_SetPauseMapMarkData(const int id, const int isMQ);

EntranceDBEntry* EntranceDB_Retrieve(const int id);
EntranceDBEntry* EntranceDB_RetrieveLayer(const int id, const int layer);
int EntranceDB_RetrieveId(const char* name);
int EntranceDB_CalcId(const int sceneId, const int spawn, const int layer);
int EntranceDB_CalcIdWithEntrance(const int entrance, const int newLayer);

#endif