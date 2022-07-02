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
    std::unordered_map<RandomizerCheck, std::string> hintLocations;
    bool gettingBottledItem;
    std::string childAltarText;
    std::string adultAltarText;
    std::unordered_map<RandomizerSettingKey, u8> randoSettings;
    GetItemID GetItemFromGet(RandomizerGet randoGet, GetItemID ogItemId);
    GetItemID GetItemFromActor(s16 actorId, s16 actorParams, s16 sceneNum, GetItemID ogItemId);
    void ParseRandomizerSettingsFile(const char* spoilerFileName);
    void ParseHintLocationsFile(const char* spoilerFileName);
    void ParseItemLocationsFile(const char* spoilerFileName);

  public:
    Randomizer();
    ~Randomizer();

    static Sprite* GetSeedTexture(uint8_t index);
    s16 GetItemModelFromId(s16 itemId);
    s32 GetItemIDFromGetItemID(s32 getItemId);
    bool SpoilerFileExists(const char* spoilerFileName);
    void LoadRandomizerSettings(const char* spoilerFileName);
    void LoadHintLocations(const char* spoilerFileName);
    void LoadItemLocations(const char* spoilerFileName);
    u8 GetRandoSettingValue(RandomizerSettingKey randoSettingKey);
    RandomizerCheck GetCheckFromActor(s16 actorId, s16 actorParams, s16 sceneNum);
    bool GettingItemInBottle();
    std::string GetChildAltarText();
    std::string GetAdultAltarText();
    std::string GetHintFromCheck(RandomizerCheck check);
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
