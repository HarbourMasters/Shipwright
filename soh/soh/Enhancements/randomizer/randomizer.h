#ifndef RANDOMIZER_H
#define RANDOMIZER_H

#include <unordered_map>
#include <string>
#include "../../../include/ultra64.h"
#include "../../../include/z64item.h"
#include <randomizerTypes.h>

class Randomizer {
  private:
    std::unordered_map<RandomizerCheck, RandomizerGet> itemLocations;
    std::unordered_map<RandomizerSettingKey, u8> randoSettings;
    GetItemID GetItemFromGet(RandomizerGet randoGet, GetItemID ogItemId);
    RandomizerCheck GetCheckFromActor(s16 actorId, s16 actorParams, s16 sceneNum);
    GetItemID GetItemFromActor(s16 actorId, s16 actorParams, s16 sceneNum, GetItemID ogItemId);
    void ParseRandomizerSettingsFile(const char* spoilerFileName);
    void ParseItemLocationsFile(const char* spoilerFileName);

  public:
    Randomizer();
    ~Randomizer();

    static Sprite* GetSeedTexture(uint8_t index);
    s16 GetItemModelFromId(s16 itemId);
    void LoadRandomizerSettings(const char* spoilerFileName);
    void LoadItemLocations(const char* spoilerFileName);
    u8 GetRandoSettingValue(RandomizerSettingKey randoSettingKey);
    GetItemID GetRandomizedItemIdFromKnownCheck(RandomizerCheck randomizerCheck, GetItemID ogId);
    GetItemID GetRandomizedItemId(GetItemID ogId, s16 actorId, s16 actorParams, s16 sceneNum);
};

#ifdef __cplusplus
extern "C" {
#endif

void Rando_Init(void);

#ifdef __cplusplus
}
#endif


#endif
