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
    RandomizerCheck GetCheckFromSceneAndParams(s16 sceneNum, s16 actorParams);

  public:
    Randomizer();
    ~Randomizer();

    void LoadItemLocations();
    void ParseItemLocations(std::string spoilerfilename);
    GetItemID GetItemFromActor(s16 actorId, GetItemID ogItemId);
    GetItemID GetItemFromSceneAndParams(s16 sceneNum, s16 actorParams, GetItemID ogItemId);
};

#endif
