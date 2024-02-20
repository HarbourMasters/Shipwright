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
#include <soh/Enhancements/custom-message/CustomMessageManager.h>
#include "soh/Enhancements/item-tables/ItemTableTypes.h"
#include "../custom-message/CustomMessageTypes.h"
#include "soh/Enhancements/randomizer/fishsanity.h"

#define MAX_SEED_STRING_SIZE 1024
#define NUM_TRIFORCE_PIECE_MESSAGES 6
#define NUM_NAVI_MESSAGES 19
#define NUM_ICE_TRAP_MESSAGES 23
#define NUM_GORON_MESSAGES 9

class Randomizer {
  private:
    std::unordered_map<RandomizerSettingKey, u8> randoSettings;
    bool IsItemVanilla(RandomizerGet randoGet);

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
    std::unordered_map<RandomizerGet, std::array<std::string, 3>> EnumToSpoilerfileGetName;

    static Sprite* GetSeedTexture(uint8_t index);
    bool SpoilerFileExists(const char* spoilerFileName);
    void LoadMerchantMessages();
    void LoadHintMessages();
    bool IsTrialRequired(RandomizerInf trial);
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
    GetItemID GetItemIdFromRandomizerGet(RandomizerGet randoGet, GetItemID ogItemId);
    ItemObtainability GetItemObtainabilityFromRandomizerCheck(RandomizerCheck randomizerCheck);
    ItemObtainability GetItemObtainabilityFromRandomizerGet(RandomizerGet randomizerCheck);
    CustomMessage GetSheikMessage(s16 scene, u16 originalTextId);
    CustomMessage ReplaceWithItemName(CustomMessage message, std::string&& toReplace, RandomizerCheck hintedCheck);
    CustomMessage GetMiscHintMessage(TextIDs textToGet, RandomizerCheck hintedCheck, RandomizerCheck otherCheck = RC_UNKNOWN_CHECK);
    CustomMessage GetSariaMessage(u16 originalTextId);
    CustomMessage GetFishingPondOwnerMessage(u16 originalTextId);
    CustomMessage GetMerchantMessage(RandomizerInf randomizerInf, u16 textId, bool mysterious = false);
    RandomizerCheck GetCheckFromActor(s16 actorId, s16 sceneNum, s16 actorParams);
    CustomMessage GetCursedSkullMessage(s16 params, RandomizerCheck hintedCheck);
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

