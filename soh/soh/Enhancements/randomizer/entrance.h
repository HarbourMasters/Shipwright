#pragma once
#ifdef __cplusplus

#include "randomizerTypes.h"
#include "3drando/location_access.hpp"

#include <nlohmann/json.hpp>

#define ENTRANCE_SHUFFLE_SUCCESS 0
#define ENTRANCE_SHUFFLE_FAILURE 1

namespace Rando {
enum class EntranceType {
    None,
    OwlDrop,
    Spawn,
    WarpSong,
    BlueWarp,
    Dungeon,
    GanonDungeon,
    DungeonReverse,
    Boss,
    BossReverse,
    ChildBoss,
    ChildBossReverse,
    AdultBoss,
    AdultBossReverse,
    Interior,
    InteriorReverse,
    SpecialInterior,
    GrottoGrave,
    GrottoGraveReverse,
    Overworld,
    Extra,
    Mixed,
    All,
};

class Entrance {
  public:
    Entrance(RandomizerRegion connectedRegion_, std::vector<ConditionFn> conditions_met_, bool spreadsAreasWithPriority_ = true);
    void SetCondition(ConditionFn newCondition);
    bool GetConditionsMet() const;
    std::string to_string() const;
    void SetName(std::string name_ = "");
    std::string GetName() const;
    void printAgeTimeAccess();
    bool ConditionsMet(bool allAgeTimes = false) const;
    uint32_t Getuint32_t() const;
    bool CheckConditionAtAgeTime(bool& age, bool& time, bool passAnyway = false) const;
    RandomizerRegion GetConnectedRegionKey() const;
    RandomizerRegion GetOriginalConnectedRegionKey() const;
    Region* GetConnectedRegion() const;
    void SetParentRegion(RandomizerRegion newParent);
    RandomizerRegion GetParentRegionKey() const;
    Region* GetParentRegion() const;
    void SetNewEntrance(RandomizerRegion newRegion);
    void SetAsShuffled();
    bool IsShuffled() const;
    bool IsAddedToPool() const;
    void AddToPool();
    void RemoveFromPool();
    void SetAsPrimary();
    bool IsPrimary() const;
    bool IsDecoupled() const;
    void SetDecoupled();
    int16_t GetIndex() const;
    void SetIndex(int16_t newIndex);
    Entrance* GetAssumed() const;
    void SetReplacement(Entrance* newReplacement);
    Entrance* GetReplacement() const;
    EntranceType GetType() const;
    void SetType(EntranceType newType);
    Entrance* GetReverse() const;
    void Connect(RandomizerRegion newConnectedRegion);
    RandomizerRegion Disconnect();
    void BindTwoWay(Entrance* otherEntrance);
    Entrance* GetNewTarget();
    Entrance* AssumeReachable();
    bool DoesSpreadAreas();

  private:
    RandomizerRegion parentRegion;
    RandomizerRegion connectedRegion;
    RandomizerRegion originalConnectedRegion;
    std::vector<ConditionFn> conditions_met;

    EntranceType type = EntranceType::None;
    Entrance* target = nullptr;
    Entrance* reverse = nullptr;
    Entrance* assumed = nullptr;
    Entrance* replacement = nullptr;
    int16_t index = 0xFFFF;
    bool shuffled = false;
    bool primary = false;
    bool addedToPool = false;
    bool decoupled = false;
    std::string name = "";
    //If this is false, areas only spread to interiors through this entrance if there is no other choice
    //Set to false for owl drops, the windmill path between dampe's grave and windmill and blue warps
    bool spreadsAreasWithPriority = true; 
};

typedef struct {
    EntranceType type;
    RandomizerRegion parentRegion;
    RandomizerRegion connectedRegion;
    int16_t index;
} EntranceLinkInfo;

typedef struct {
    std::list<RandomizerRegion> targetRegions;
    std::list<EntranceType> allowedTypes;
} PriorityEntrance;
// primary,          secondary
using EntranceInfoPair = std::pair<EntranceLinkInfo, EntranceLinkInfo>;
using EntrancePair = std::pair<Entrance*, Entrance*>;
using EntrancePools = std::map<EntranceType, std::vector<Entrance*>>;

class EntranceShuffler {
  public:
    EntranceShuffler();
    std::array<EntranceOverride, ENTRANCE_OVERRIDES_MAX_COUNT> entranceOverrides;
    std::vector<std::list<Entrance*>> playthroughEntrances;
    bool HasNoRandomEntrances();
    void SetNoRandomEntrances(bool noRandomEntrances);
    int ShuffleAllEntrances();
    void CreateEntranceOverrides();
    void UnshuffleAllEntrances();
    void ParseJson(nlohmann::json spoilerFileJson);
  private:
    std::vector<Entrance*> AssumeEntrancePool(std::vector<Entrance*>& entrancePool);
    bool ShuffleOneWayPriorityEntrances(std::map<std::string, PriorityEntrance>& oneWayPriorities,
                                        EntrancePools oneWayEntrancePools, EntrancePools oneWayTargetEntrancePools,
                                        int retryCount = 2);
    bool PlaceOneWayPriorityEntrance(std::string priorityName, std::list<RandomizerRegion>& allowedRegions,
                                     std::list<EntranceType>& allowedTypes, std::vector<EntrancePair>& rollbacks,
                                     EntrancePools oneWayEntrancePools, EntrancePools oneWayTargetEntrancePools);
    bool ReplaceEntrance(Entrance* entrance, Entrance* target, std::vector<EntrancePair>& rollbacks);
    void ShuffleEntrancePool(std::vector<Entrance*>& entrancePool, std::vector<Entrance*>& targetEntrances,
                             int retryCount = 20);
    bool ShuffleEntrances(std::vector<Entrance*>& entrances, std::vector<Entrance*>& targetEntrances,
                          std::vector<EntrancePair>& rollbacks);
    bool mNoRandomEntrances;
    int mTotalRandomizableEntrances = 0;
    int mCurNumRandomizedEntrances = 0;
    bool mEntranceShuffleFailure = false;
};
} // namespace Rando

extern "C" {
#endif
  EntranceOverride* Randomizer_GetEntranceOverrides();
#ifdef __cplusplus
}
#endif
