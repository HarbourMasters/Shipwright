#pragma once

#include <string>
#include <vector>
#include <cstdint>

#include <libultraship/libultraship.h>

typedef enum {
    // ENTRANCE_GROUP_NO_GROUP,

    ENTRANCE_GROUP_ONE_WAY,
    ENTRANCE_GROUP_KOKIRI_FOREST,
    ENTRANCE_GROUP_LOST_WOODS,
    ENTRANCE_GROUP_SFM,
    ENTRANCE_GROUP_KAKARIKO,
    ENTRANCE_GROUP_GRAVEYARD,
    ENTRANCE_GROUP_DEATH_MOUNTAIN_TRAIL,
    ENTRANCE_GROUP_DEATH_MOUNTAIN_CRATER,
    ENTRANCE_GROUP_GORON_CITY,
    ENTRANCE_GROUP_ZORAS_RIVER,
    ENTRANCE_GROUP_ZORAS_DOMAIN,
    ENTRANCE_GROUP_ZORAS_FOUNTAIN,
    ENTRANCE_GROUP_HYRULE_FIELD,
    ENTRANCE_GROUP_LON_LON_RANCH,
    ENTRANCE_GROUP_LAKE_HYLIA,
    ENTRANCE_GROUP_GERUDO_VALLEY,
    ENTRANCE_GROUP_HAUNTED_WASTELAND,
    ENTRANCE_GROUP_MARKET,
    ENTRANCE_GROUP_HYRULE_CASTLE,
    SPOILER_ENTRANCE_GROUP_COUNT,
} SpoilerEntranceGroup;

typedef enum {
    ENTRANCE_TYPE_ONE_WAY,
    ENTRANCE_TYPE_OVERWORLD,
    ENTRANCE_TYPE_INTERIOR,
    ENTRANCE_TYPE_GROTTO,
    ENTRANCE_TYPE_DUNGEON,
    ENTRANCE_TYPE_COUNT,
} TrackerEntranceType;

typedef struct {
    int8_t scene;
    int8_t spawn;
} EntranceDataSceneAndSpawn;

typedef struct {
    int16_t index;
    int16_t reverseIndex;
    std::vector<EntranceDataSceneAndSpawn> scenes;
    std::string source;
    std::string destination;
    SpoilerEntranceGroup srcGroup;
    SpoilerEntranceGroup dstGroup;
    TrackerEntranceType type;
    std::string metaTag;
    uint8_t oneExit;
} EntranceData;

typedef enum {
    ENTRANCE_SOURCE_AREA,
    ENTRANCE_DESTINATION_AREA,
    ENTRANCE_SOURCE_TYPE,
    ENTRANCE_DESTINATION_TYPE,
    TRACKER_GROUP_TYPE_COUNT,
} TrackerEntranceGroupingType;

typedef struct {
    uint8_t EntranceCount;
    uint16_t GroupEntranceCounts[TRACKER_GROUP_TYPE_COUNT][SPOILER_ENTRANCE_GROUP_COUNT];
    uint16_t GroupOffsets[TRACKER_GROUP_TYPE_COUNT][SPOILER_ENTRANCE_GROUP_COUNT];
} EntranceTrackingData;

extern EntranceTrackingData gEntranceTrackingData;

#define SINGLE_SCENE_INFO(scene) {{ scene, -1 }}
#define SCENE_NO_SPAWN(scene) { scene, -1 }

void SetCurrentGrottoIDForTracker(int16_t entranceIndex);
void SetLastEntranceOverrideForTracker(int16_t entranceIndex);
void ClearEntranceTrackingData();
void InitEntranceTrackingData();
s16 GetLastEntranceOverride();
s16 GetCurrentGrottoId();
const EntranceData* GetEntranceData(s16);

class EntranceTrackerWindow : public LUS::GuiWindow {
  public:
    using GuiWindow::GuiWindow;

    void InitElement() override;
    void DrawElement() override;
    void UpdateElement() override {};
};
