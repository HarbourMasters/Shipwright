#pragma once

#include <unordered_map>
#include <string>
#include "../../../include/ultra64.h"
#include "../../../include/z64item.h"
#include <memory>
#include <soh/Enhancements/randomizer/randomizerTypes.h>
#include <soh/Enhancements/custom-message/CustomMessageManager.h>

#define NUM_NAVI_MESSAGES 19

class Randomizer {
  private:
    std::unordered_map<RandomizerCheck, RandomizerGet> itemLocations;
    std::unordered_map<RandomizerCheck, std::string> hintLocations;
    std::unordered_map<RandomizerInf, bool> trialsRequired;
    std::string childAltarText;
    std::string adultAltarText;
    std::string ganonHintText;
    std::string ganonText;
    std::unordered_map<RandomizerSettingKey, u8> randoSettings;
    std::unordered_map<RandomizerCheck, u16> randomizerMerchantPrices;
    void ParseRandomizerSettingsFile(const char* spoilerFileName);
    void ParseHintLocationsFile(const char* spoilerFileName);
    void ParseRequiredTrialsFile(const char* spoilerFileName);
    void ParseItemLocationsFile(const char* spoilerFileName, bool silent);
    bool IsItemVanilla(RandomizerGet randoGet);


  public:
    Randomizer();
    ~Randomizer();

    static const std::string getItemMessageTableID;
    static const std::string hintMessageTableID;
    static const std::string merchantMessageTableID;
    static const std::string rupeeMessageTableID;
    static const std::string NaviRandoMessageTableID;

    static Sprite* GetSeedTexture(uint8_t index);
    s16 GetItemModelFromId(s16 itemId);
    s32 GetItemIDFromGetItemID(s32 getItemId);
    bool SpoilerFileExists(const char* spoilerFileName);
    void LoadRandomizerSettings(const char* spoilerFileName);
    void LoadHintLocations(const char* spoilerFileName);
    void LoadRequiredTrials(const char* spoilerFileName);
    void LoadItemLocations(const char* spoilerFileName,bool silent);
    bool IsTrialRequired(RandomizerInf trial);
    u8 GetRandoSettingValue(RandomizerSettingKey randoSettingKey);
    RandomizerCheck GetCheckFromActor(s16 actorId, s16 sceneNum, s16 actorParams);
    RandomizerGet GetRandomizerGetFromActor(s16 actorId, s16 sceneNum, s16 actorParams);
    RandomizerGet GetRandomizerGetFromKnownCheck(RandomizerCheck randomizerCheck);
    std::string GetChildAltarText() const;
    std::string GetAdultAltarText() const;
    std::string GetGanonText() const;
    std::string GetGanonHintText() const;
    ScrubIdentity IdentifyScrub(s32 sceneNum, s32 actorParams, s32 respawnData);
    GetItemID GetItemIdFromKnownCheck(RandomizerCheck randomizerCheck, GetItemID ogItemId);
    GetItemID GetItemIdFromActor(s16 actorId, s16 sceneNum, s16 actorParams, GetItemID ogItemId);
    GetItemID GetItemIdFromRandomizerGet(RandomizerGet randoGet, GetItemID ogItemId);
    ItemObtainability GetItemObtainabilityFromRandomizerCheck(RandomizerCheck randomizerCheck);
    ItemObtainability GetItemObtainabilityFromRandomizerGet(RandomizerGet randomizerCheck);
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

