#pragma once

#include <unordered_map>
#include <string>
#include <libultraship/libultra.h>
#include "z64item.h"
#include "SeedContext.h"
#include <soh/Enhancements/randomizer/randomizerTypes.h>
#include <soh/Enhancements/custom-message/CustomMessageManager.h>
#include "soh/Enhancements/item-tables/ItemTableTypes.h"
#include "../custom-message/CustomMessageTypes.h"

#define MAX_SEED_STRING_SIZE 1024

class Randomizer {
  private:
    std::unordered_map<RandomizerSettingKey, u8> randoSettings;

  public:
    Randomizer();
    ~Randomizer();
    bool SpoilerFileExists(const char* spoilerFileName);
    bool IsTrialRequired(s32 trialFlag);
    static s8 SilverTotal(RandomizerGet rg);
    static s8* SilverFieldFromSaveContext(SaveContext* saveContext, RandomizerGet rg);
    u8 GetRandoSettingValue(RandomizerSettingKey randoSettingKey);
    u8 GetTriforcePiecesRequired();
    RandomizerCheck GetCheckFromRandomizerInf(RandomizerInf randomizerInf);
    RandomizerInf GetRandomizerInfFromCheck(RandomizerCheck rc);
    Rando::Location* GetCheckObjectFromActor(s16 actorId, s16 sceneNum, s32 actorParams);
    ShopItemIdentity IdentifyShopItem(s32 sceneNum, u8 slotIndex);
    CheckIdentity IdentifySilver(s32 sceneNum, Vec3f pos);
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
