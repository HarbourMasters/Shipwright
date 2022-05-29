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
    RandomizerCheck GetCheckFromActor(s16 actorId, s16 actorParams, s16 sceneNum);
    GetItemID GetItemFromActor(s16 actorId, s16 actorParams, s16 sceneNum, GetItemID ogItemId);

  public:
    Randomizer();
    ~Randomizer();

    s16 GetItemModelFromId(s16 itemId);
    void LoadItemLocations(const char* spoilerFileName);
    void ParseItemLocations(const char* spoilerFileName);
    s32 GetRandomizedItemId(GetItemID ogId, s16 actorId, s16 actorParams, s16 sceneNum);
};

#endif
