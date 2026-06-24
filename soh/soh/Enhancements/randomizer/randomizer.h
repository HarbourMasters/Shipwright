#pragma once

#include <unordered_map>
#include <string>
#include <libultraship/libultra.h>
#include "z64item.h"
#include "SeedContext.h"
#include <soh/Enhancements/randomizer/randomizerTypes.h>
#include "soh/Enhancements/randomizer/randomizer_check_objects.h"
#include "soh/Enhancements/randomizer/tricks.h"
#include <soh/Enhancements/custom-message/CustomMessageManager.h>
#include "soh/Enhancements/item-tables/ItemTableTypes.h"
#include "../custom-message/CustomMessageTypes.h"

#define MAX_SEED_STRING_SIZE 1024
#define FOREST_TEMPLE_SMALL_KEY_MAX (ResourceMgr_IsSceneMasterQuest(SCENE_FOREST_TEMPLE) ? 6 : 5)
#define FIRE_TEMPLE_SMALL_KEY_MAX (ResourceMgr_IsSceneMasterQuest(SCENE_FIRE_TEMPLE) ? 5 : 8)
#define WATER_TEMPLE_SMALL_KEY_MAX (ResourceMgr_IsSceneMasterQuest(SCENE_WATER_TEMPLE) ? 2 : 6)
#define SPIRIT_TEMPLE_SMALL_KEY_MAX (ResourceMgr_IsSceneMasterQuest(SCENE_SPIRIT_TEMPLE) ? 7 : 5)
#define SHADOW_TEMPLE_SMALL_KEY_MAX (ResourceMgr_IsSceneMasterQuest(SCENE_SHADOW_TEMPLE) ? 6 : 5)
#define BOTTOM_OF_THE_WELL_SMALL_KEY_MAX (ResourceMgr_IsSceneMasterQuest(SCENE_BOTTOM_OF_THE_WELL) ? 2 : 3)
#define GERUDO_TRAINING_GROUND_SMALL_KEY_MAX (ResourceMgr_IsSceneMasterQuest(SCENE_GERUDO_TRAINING_GROUND) ? 3 : 9)
#define GERUDO_FORTRESS_SMALL_KEY_MAX                                                                    \
    (OTRGlobals::Instance->gRandoContext->GetOption(RSK_GERUDO_FORTRESS).Is(RO_GF_CARPENTERS_FAST)   ? 1 \
     : OTRGlobals::Instance->gRandoContext->GetOption(RSK_GERUDO_FORTRESS).Is(RO_GF_CARPENTERS_FREE) ? 0 \
                                                                                                     : 4)
#define THIEVES_HIDEOUT_DOOR_FLAGS                                                                   \
    (OTRGlobals::Instance->gRandoContext->GetOption(RSK_GERUDO_FORTRESS).Is(RO_GF_CARPENTERS_FAST)   \
         ? std::vector<uint8_t>{ 1 }                                                                 \
     : OTRGlobals::Instance->gRandoContext->GetOption(RSK_GERUDO_FORTRESS).Is(RO_GF_CARPENTERS_FREE) \
         ? std::vector<uint8_t>{}                                                                    \
         : std::vector<uint8_t>{ 1, 2, 3, 4 })
#define GANONS_CASTLE_SMALL_KEY_MAX (ResourceMgr_IsSceneMasterQuest(SCENE_INSIDE_GANONS_CASTLE) ? 3 : 2)
#define TREASURE_GAME_SMALL_KEY_MAX 6

class Randomizer {
  private:
    std::unordered_map<RandomizerSettingKey, u8> randoSettings;

  public:
    Randomizer();
    ~Randomizer();
    bool SpoilerFileExists(const char* spoilerFileName);
    bool IsTrialRequired(s32 trialFlag);
    u8 GetRandoSettingValue(RandomizerSettingKey randoSettingKey);
    RandomizerCheck GetCheckFromRandomizerInf(RandomizerInf randomizerInf);
    RandomizerInf GetRandomizerInfFromCheck(RandomizerCheck rc);
    Rando::Location* GetCheckObjectFromActor(s16 actorId, s16 sceneNum, s32 actorParams);
    ShopItemIdentity IdentifyShopItem(s32 sceneNum, u8 slotIndex);
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
