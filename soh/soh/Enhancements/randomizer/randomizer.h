#pragma once

#include <unordered_map>
#include <unordered_set>
#include <string>
#include <vector>
#include <libultraship/libultra.h>
#include "z64item.h"
#include <memory>
#include "SeedContext.h"
#include <soh/Enhancements/randomizer/randomizerTypes.h>
#include "soh/Enhancements/randomizer/randomizer_check_objects.h"
#include "soh/Enhancements/randomizer/randomizer_check_tracker.h"
#include "soh/Enhancements/randomizer/tricks.h"
#include <soh/Enhancements/custom-message/CustomMessageManager.h>
#include "soh/Enhancements/item-tables/ItemTableTypes.h"
#include "../custom-message/CustomMessageTypes.h"
#include "soh/Enhancements/randomizer/fishsanity.h"

#define MAX_SEED_STRING_SIZE 1024

class Randomizer {
  private:
    std::unordered_map<RandomizerSettingKey, u8> randoSettings;

  public:
    Randomizer();
    ~Randomizer();
    static Sprite* GetSeedTexture(uint8_t index);
    bool SpoilerFileExists(const char* spoilerFileName);
    bool IsTrialRequired(s32 trialFlag);
    u8 GetRandoSettingValue(RandomizerSettingKey randoSettingKey);
    RandomizerCheck GetCheckFromRandomizerInf(RandomizerInf randomizerInf);
    RandomizerInf GetRandomizerInfFromCheck(RandomizerCheck rc);
    Rando::Location* GetCheckObjectFromActor(s16 actorId, s16 sceneNum, s32 actorParams);
    ScrubIdentity IdentifyScrub(s32 sceneNum, s32 actorParams, s32 respawnData);
    CheckIdentity IdentifyBeehive(s32 sceneNum, s16 xPosition, s32 respawnData);
    ShopItemIdentity IdentifyShopItem(s32 sceneNum, u8 slotIndex);
    CheckIdentity IdentifyCow(s32 sceneNum, s32 posX, s32 posZ);
    CheckIdentity IdentifyPot(s32 sceneNum, s32 posX, s32 posZ);
    CheckIdentity IdentifyFish(s32 sceneNum, s32 actorParams);
    CheckIdentity IdentifyGrass(s32 sceneNum, s32 posX, s32 posZ, s32 respawnData, s32 linkAge);
    CheckIdentity IdentifyCrate(s32 sceneNum, s32 posX, s32 posZ);
    CheckIdentity IdentifySmallCrate(s32 sceneNum, s32 posX, s32 posZ);
    CheckIdentity IdentifyTree(s32 sceneNum, s32 posX, s32 posZ);
    GetItemEntry GetItemFromKnownCheck(RandomizerCheck randomizerCheck, GetItemID ogItemId,
                                       bool checkObtainability = true);
    GetItemEntry GetItemFromActor(s16 actorId, s16 sceneNum, s16 actorParams, GetItemID ogItemId,
                                  bool checkObtainability = true);
    ItemObtainability GetItemObtainabilityFromRandomizerCheck(RandomizerCheck randomizerCheck);
    ItemObtainability GetItemObtainabilityFromRandomizerGet(RandomizerGet randomizerCheck);
    RandomizerCheck GetCheckFromActor(s16 actorId, s16 sceneNum, s16 actorParams);
};

#ifdef __cplusplus
extern "C" {
#endif

bool GenerateRandomizer(std::string seed = "");
void JoinRandoGenerationThread();

#ifdef __cplusplus
}
#endif
