#ifndef RANDOMIZER_H
#define RANDOMIZER_H

#include <unordered_map>
#include <string>
#include "../../include/ultra64.h"
#include "../../include/z64item.h"
#include <randomizerTypes.h>
#include "randomizer/keys.hpp"

class Randomizer {
  private:
    std::unordered_map<Key, Key> itemLocations;
    GetItemID GetItemFromGet(Key randoGet, GetItemID ogItemId);
    Key GetCheckFromActor(s16 actorId, s16 actorParams, s16 sceneNum);
    GetItemID GetItemFromActor(s16 actorId, s16 actorParams, s16 sceneNum, GetItemID ogItemId);

  public:
    Randomizer();
    ~Randomizer();

    s16 GetItemModelFromId(s16 itemId);
    void ParseItemLocations(SpoilerData spoilerData);
    GetItemID GetRandomizedItemIdFromKnownCheck(Key randomizerCheck, GetItemID ogId);
    GetItemID GetRandomizedItemId(GetItemID ogId, s16 actorId, s16 actorParams, s16 sceneNum);
};

#endif
