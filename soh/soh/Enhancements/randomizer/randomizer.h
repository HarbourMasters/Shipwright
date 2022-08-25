#pragma once

#include <unordered_map>
#include <string>
#include "../../../include/ultra64.h"
#include "../../../include/z64item.h"
#include <memory>
#include <soh/Enhancements/randomizer/randomizerTypes.h>
#include <soh/Enhancements/custom-message/CustomMessageManager.h>

#define NUM_NAVI_MESSAGES 15

class Randomizer {
  private:
    std::unordered_map<RandomizerCheck, RandomizerGet> itemLocations;
    std::unordered_map<RandomizerCheck, std::string> hintLocations;
    std::unordered_map<s32, s32> entranceOverrides;
    std::string childAltarText;
    std::string adultAltarText;
    std::string ganonHintText;
    std::string ganonText;
    std::unordered_map<RandomizerSettingKey, u8> randoSettings;
    s16 GetItemFromGet(RandomizerGet randoGet, GetItemID ogItemId);
    s16 GetItemFromActor(s16 actorId, s16 actorParams, s16 sceneNum, GetItemID ogItemId);
    void ParseRandomizerSettingsFile(const char* spoilerFileName);
    void ParseHintLocationsFile(const char* spoilerFileName);
    void ParseItemLocationsFile(const char* spoilerFileName, bool silent);
    void ParseEntranceDataFile(const char* spoilerFileName, bool silent);
    bool IsItemVanilla(RandomizerGet randoGet);


  public:
    Randomizer();
    ~Randomizer();

    static const std::string getItemMessageTableID;
    static const std::string hintMessageTableID;
    static const std::string scrubMessageTableID;
    static const std::string rupeeMessageTableID;
    static const std::string NaviRandoMessageTableID;

    static Sprite* GetSeedTexture(uint8_t index);
    s16 GetItemModelFromId(s16 itemId);
    s32 GetItemIDFromGetItemID(s32 getItemId);
    bool SpoilerFileExists(const char* spoilerFileName);
    void LoadRandomizerSettings(const char* spoilerFileName);
    void LoadHintLocations(const char* spoilerFileName);
    void LoadItemLocations(const char* spoilerFileName, bool silent);
    void LoadEntranceOverrides(const char* spoilerFileName, bool silent);
    u8 GetRandoSettingValue(RandomizerSettingKey randoSettingKey);
    RandomizerCheck GetCheckFromActor(s16 sceneNum, s16 actorId, s16 actorParams);
    std::string GetChildAltarText() const;
    std::string GetAdultAltarText() const;
    std::string GetGanonText() const;
    std::string GetGanonHintText() const;
    s16 GetRandomizedItemIdFromKnownCheck(RandomizerCheck randomizerCheck, GetItemID ogId);
    s16 GetRandomizedItemId(GetItemID ogId, s16 actorId, s16 actorParams, s16 sceneNum);
    static void CreateCustomMessages();
    static std::string RandomizeRupeeName(std::string message, int language);
    static CustomMessageEntry GetRupeeMessage(u16 rupeeTextId);
    bool CheckContainsVanillaItem(RandomizerCheck randoCheck);
};

#ifdef __cplusplus
extern "C" {
#endif

void Rando_Init(void);

#ifdef __cplusplus
}
#endif

