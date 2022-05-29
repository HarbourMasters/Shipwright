#ifndef RANDOMIZER_H
#define RANDOMIZER_H

#include <unordered_map>
#include <string>
#include "../../include/ultra64.h"
#include "../../include/z64item.h"
#include <randomizerTypes.h>

class Randomizer {
  private:
    std::unordered_map<RandomizerCheck, RandomizerGet> itemLocations;
    GetItemID GetItemFromGet(RandomizerGet randoGet, GetItemID ogItemId);
    RandomizerCheck GetCheckFromActor(s16 actorId, GetItemID ogItemId);
    RandomizerCheck GetCheckFromSceneAndParams(s16 sceneNum, s16 actorParams, s32 homePosX, s32 homePosY, s32 homePosZ);
    GetItemID GetItemFromActor(s16 actorId, GetItemID ogItemId);
    GetItemID GetItemFromSceneParamsAndHomePos(s16 sceneNum, s16 actorParams, s32 homePosX, s32 homePosY, s32 homePosZ, GetItemID ogItemId);

  public:
    Randomizer();
    ~Randomizer();

    s16 GetItemModelFromId(s16 itemId);
    void LoadItemLocations(const char* spoilerFileName);
    void ParseItemLocations(const char* spoilerFileName);
    s32 GetRandomizedItemId(GetItemID ogId, s16 actorId = -1, s16 sceneNum = -1, s16 actorParams = -1, s32 homePosX = 0, s32 homePosY = 0, s32 homePosZ = 0);
};

#endif
