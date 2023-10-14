#pragma once

#include <unordered_map>
#include <unordered_set>
#include <string>
#include <vector>
#include <libultraship/libultra.h>
#include "z64item.h"
#include <memory>
#include <soh/Enhancements/randomizer/randomizerTypes.h>
#include "soh/Enhancements/randomizer/randomizer_check_objects.h"
#include "soh/Enhancements/randomizer/randomizer_tricks.h"
#include <soh/Enhancements/custom-message/CustomMessageManager.h>
#include "soh/Enhancements/item-tables/ItemTableTypes.h"

#define MAX_SEED_STRING_SIZE 1024
#define NUM_TRIFORCE_PIECE_MESSAGES 6
#define NUM_NAVI_MESSAGES 19
#define NUM_ICE_TRAP_MESSAGES 23
#define NUM_GORON_MESSAGES 9

class Randomizer {
  private:
    std::unordered_map<RandomizerCheck, RandomizerGetData> itemLocations;
    std::unordered_map<RandomizerCheck, std::string> hintLocations;
    std::string childAltarText;
    std::string adultAltarText;
    std::string ganonHintText;
    std::string ganonText;
    std::string dampeText;
    std::string sheikText;
    std::string sariaText;
    std::unordered_map<RandomizerSettingKey, u8> randoSettings;
    void ParseRandomizerSettingsFile(const char* spoilerFileName);
    void ParseHintLocationsFile(const char* spoilerFileName);
    void ParseRequiredTrialsFile(const char* spoilerFileName);
    void ParseMasterQuestDungeonsFile(const char* spoilerFileName);
    void ParseItemLocationsFile(const char* spoilerFileName, bool silent);
    void ParseEntranceDataFile(const char* spoilerFileName, bool silent);
    bool IsItemVanilla(RandomizerGet randoGet);
    int16_t GetVanillaMerchantPrice(RandomizerCheck check);

  public:
    Randomizer();
    ~Randomizer();

    static const std::string getItemMessageTableID;
    static const std::string hintMessageTableID;
    static const std::string merchantMessageTableID;
    static const std::string rupeeMessageTableID;
    static const std::string triforcePieceMessageTableID;
    static const std::string NaviRandoMessageTableID;
    static const std::string IceTrapRandoMessageTableID;
    static const std::string randoMiscHintsTableID;

    // Public for now to be accessed by SaveManager, will be made private again soon :tm:
    std::unordered_map<RandomizerInf, bool> trialsRequired;
    std::unordered_set<uint16_t> masterQuestDungeons;
    std::unordered_map<RandomizerCheck, u16> merchantPrices;
    std::unordered_map<RandomizerGet, std::array<std::string, 3>> EnumToSpoilerfileGetName;

    static Sprite* GetSeedTexture(uint8_t index);
    s16 GetItemModelFromId(s16 itemId);
    s32 GetItemIDFromGetItemID(s32 getItemId);
    bool SpoilerFileExists(const char* spoilerFileName);
    void LoadRandomizerSettings(const char* spoilerFileName);
    void LoadHintLocations(const char* spoilerFileName);
    void LoadMerchantMessages(const char* spoilerFileName);
    void LoadItemLocations(const char* spoilerFileName, bool silent);
    void LoadRequiredTrials(const char* spoilerFileName);
    void LoadMasterQuestDungeons(const char* spoilerFileName);
    bool IsTrialRequired(RandomizerInf trial);
    void LoadEntranceOverrides(const char* spoilerFileName, bool silent);
    u8 GetRandoSettingValue(RandomizerSettingKey randoSettingKey);
    RandomizerCheck GetCheckFromActor(s16 actorId, s16 sceneNum, s16 actorParams);
    RandomizerCheck GetCheckFromRandomizerInf(RandomizerInf randomizerInf);
    RandomizerInf GetRandomizerInfFromCheck(RandomizerCheck rc);
    RandomizerGetData GetRandomizerGetDataFromActor(s16 actorId, s16 sceneNum, s16 actorParams);
    RandomizerGetData GetRandomizerGetDataFromKnownCheck(RandomizerCheck randomizerCheck);
    GetItemEntry GetItemEntryFromRGData(RandomizerGetData rgData, GetItemID ogItemId, bool checkObtainability = true);
    std::string GetChildAltarText() const;
    std::string GetAdultAltarText() const;
    std::string GetGanonText() const;
    std::string GetGanonHintText() const;
    std::string GetDampeText() const;
    std::string GetSheikText() const;
    std::string GetSariaText() const;
    RandomizerCheckObject GetCheckObjectFromActor(s16 actorId, s16 sceneNum, s32 actorParams);
    ScrubIdentity IdentifyScrub(s32 sceneNum, s32 actorParams, s32 respawnData);
    ShopItemIdentity IdentifyShopItem(s32 sceneNum, u8 slotIndex);
    CowIdentity IdentifyCow(s32 sceneNum, s32 posX, s32 posZ);
    GetItemEntry GetItemFromKnownCheck(RandomizerCheck randomizerCheck, GetItemID ogItemId, bool checkObtainability = true);
    GetItemEntry GetItemFromActor(s16 actorId, s16 sceneNum, s16 actorParams, GetItemID ogItemId, bool checkObtainability = true);
    GetItemID GetItemIdFromRandomizerGet(RandomizerGet randoGet, GetItemID ogItemId);
    ItemObtainability GetItemObtainabilityFromRandomizerCheck(RandomizerCheck randomizerCheck);
    ItemObtainability GetItemObtainabilityFromRandomizerGet(RandomizerGet randomizerCheck);
    CustomMessage GetWarpSongMessage(u16 textId, bool mysterious = false);
    CustomMessage GetSheikMessage(s16 scene, u16 originalTextId);
    CustomMessage GetFrogsMessage(u16 originalTextId);
    CustomMessage GetSariaMessage(u16 originalTextId);
    CustomMessage GetMerchantMessage(RandomizerInf randomizerInf, u16 textId, bool mysterious = false);
    CustomMessage GetCursedSkullMessage(s16 params);
    CustomMessage GetGoronMessage(u16 index);
    CustomMessage GetMapGetItemMessageWithHint(GetItemEntry itemEntry);
    static void CreateCustomMessages();
    static CustomMessage GetRupeeMessage(u16 rupeeTextId);
    static CustomMessage GetTriforcePieceMessage();
    bool CheckContainsVanillaItem(RandomizerCheck randoCheck);
};

#ifdef __cplusplus
extern "C" {
#endif

bool GenerateRandomizer(std::string seed = "");

#ifdef __cplusplus
}
#endif

