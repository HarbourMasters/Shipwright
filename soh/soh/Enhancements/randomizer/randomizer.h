#pragma once

#include <unordered_map>
#include <unordered_set>
#include <string>
#include <vector>
#include <libultraship/libultra.h>
#include "z64item.h"
#include <memory>
#include "context.h"
#include <soh/Enhancements/randomizer/randomizerTypes.h>
#include "soh/Enhancements/randomizer/randomizer_check_objects.h"
#include "soh/Enhancements/randomizer/randomizer_check_tracker.h"
#include "soh/Enhancements/randomizer/tricks.h"
#include <soh/Enhancements/custom-message/CustomMessageManager.h>
#include "soh/Enhancements/item-tables/ItemTableTypes.h"
#include "../custom-message/CustomMessageTypes.h"
#include "soh/Enhancements/randomizer/fishsanity.h"

#define MAX_SEED_STRING_SIZE 1024
#define NUM_TRIFORCE_PIECE_MESSAGES 6
#define NUM_NAVI_MESSAGES 18
#define NUM_GORON_MESSAGES 9

class Randomizer {
  private:
    std::unordered_map<RandomizerSettingKey, u8> randoSettings;

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

    static Sprite* GetSeedTexture(uint8_t index);
    bool SpoilerFileExists(const char* spoilerFileName);
    void LoadMerchantMessages();
    void LoadHintMessages();
    bool IsTrialRequired(s32 trialFlag);
    u8 GetRandoSettingValue(RandomizerSettingKey randoSettingKey);
    RandomizerCheck GetCheckFromRandomizerInf(RandomizerInf randomizerInf);
    RandomizerInf GetRandomizerInfFromCheck(RandomizerCheck rc);
    Rando::Location* GetCheckObjectFromActor(s16 actorId, s16 sceneNum, s32 actorParams);
    ScrubIdentity IdentifyScrub(s32 sceneNum, s32 actorParams, s32 respawnData);
    BeehiveIdentity IdentifyBeehive(s32 sceneNum, s16 xPosition, s32 respawnData);
    ShopItemIdentity IdentifyShopItem(s32 sceneNum, u8 slotIndex);
    CowIdentity IdentifyCow(s32 sceneNum, s32 posX, s32 posZ);
    FishIdentity IdentifyFish(s32 sceneNum, s32 actorParams);
    GetItemEntry GetItemFromKnownCheck(RandomizerCheck randomizerCheck, GetItemID ogItemId, bool checkObtainability = true);
    GetItemEntry GetItemFromActor(s16 actorId, s16 sceneNum, s16 actorParams, GetItemID ogItemId, bool checkObtainability = true);
    ItemObtainability GetItemObtainabilityFromRandomizerCheck(RandomizerCheck randomizerCheck);
    ItemObtainability GetItemObtainabilityFromRandomizerGet(RandomizerGet randomizerCheck);
    CustomMessage GetSheikMessage(s16 scene, u16 originalTextId);
    CustomMessage GetFishingPondOwnerMessage(u16 originalTextId);
    CustomMessage GetMerchantMessage(RandomizerCheck rc, TextIDs textId, TextIDs freeTextId = TEXT_NONE, bool mysterious = false);
    RandomizerCheck GetCheckFromActor(s16 actorId, s16 sceneNum, s16 actorParams);
    CustomMessage GetGoronMessage(u16 index);
    CustomMessage GetMapGetItemMessageWithHint(GetItemEntry itemEntry);
    static void CreateCustomMessages();
    static CustomMessage GetRupeeMessage(u16 rupeeTextId);
    static CustomMessage GetIceTrapMessage();
    static CustomMessage GetTriforcePieceMessage();
};

#ifdef __cplusplus
extern "C" {
#endif

bool GenerateRandomizer(std::string seed = "");

#ifdef __cplusplus
}
#endif

