#include <libultraship/bridge.h>
#include "soh/OTRGlobals.h"
#include "soh/ResourceManagerHelpers.h"
#include "soh/Enhancements/enhancementTypes.h"
#include "soh/Enhancements/custom-message/CustomMessageTypes.h"
#include "soh/Enhancements/item-tables/ItemTableManager.h"
#include "soh/Enhancements/randomizer/randomizerTypes.h"
#include "soh/Enhancements/randomizer/dungeon.h"
#include "soh/Enhancements/randomizer/fishsanity.h"
#include "soh/Enhancements/randomizer/static_data.h"
#include "soh/Enhancements/randomizer/ShufflePots.h"
#include "soh/Enhancements/randomizer/ShuffleFreestanding.h"
#include "soh/Enhancements/game-interactor/GameInteractor.h"
#include "soh/Enhancements/game-interactor/GameInteractor_Hooks.h"
#include "soh/SohGui/ImGuiUtils.h"
#include "soh/Notification/Notification.h"
#include "soh/SaveManager.h"
#include "soh/Enhancements/randomizer/ShuffleFairies.h"

extern "C" {
#include "macros.h"
#include "functions.h"
#include "variables.h"
#include "soh/Enhancements/randomizer/ShuffleTradeItems.h"
#include "soh/Enhancements/randomizer/randomizer_entrance.h"
#include "soh/Enhancements/randomizer/randomizer_grotto.h"
#include "src/overlays/actors/ovl_Bg_Treemouth/z_bg_treemouth.h"
#include "src/overlays/actors/ovl_En_Si/z_en_si.h"
#include "src/overlays/actors/ovl_En_Shopnuts/z_en_shopnuts.h"
#include "src/overlays/actors/ovl_En_Dns/z_en_dns.h"
#include "src/overlays/actors/ovl_En_Gb/z_en_gb.h"
#include "src/overlays/actors/ovl_Item_B_Heart/z_item_b_heart.h"
#include "src/overlays/actors/ovl_En_Ko/z_en_ko.h"
#include "src/overlays/actors/ovl_En_Mk/z_en_mk.h"
#include "src/overlays/actors/ovl_En_Niw_Lady/z_en_niw_lady.h"
#include "src/overlays/actors/ovl_En_Kz/z_en_kz.h"
#include "src/overlays/actors/ovl_En_Go2/z_en_go2.h"
#include "src/overlays/actors/ovl_En_Ms/z_en_ms.h"
#include "src/overlays/actors/ovl_En_Fr/z_en_fr.h"
#include "src/overlays/actors/ovl_En_Syateki_Man/z_en_syateki_man.h"
#include "src/overlays/actors/ovl_En_Sth/z_en_sth.h"
#include "src/overlays/actors/ovl_Item_Etcetera/z_item_etcetera.h"
#include "src/overlays/actors/ovl_En_Box/z_en_box.h"
#include "src/overlays/actors/ovl_En_Skj/z_en_skj.h"
#include "src/overlays/actors/ovl_En_Hy/z_en_hy.h"
#include "src/overlays/actors/ovl_Obj_Comb/z_obj_comb.h"
#include "src/overlays/actors/ovl_En_Bom_Bowl_Pit/z_en_bom_bowl_pit.h"
#include "src/overlays/actors/ovl_En_Ge1/z_en_ge1.h"
#include "src/overlays/actors/ovl_En_Ds/z_en_ds.h"
#include "src/overlays/actors/ovl_En_Gm/z_en_gm.h"
#include "src/overlays/actors/ovl_En_Js/z_en_js.h"
#include "src/overlays/actors/ovl_En_Door/z_en_door.h"
#include "src/overlays/actors/ovl_Door_Shutter/z_door_shutter.h"
#include "src/overlays/actors/ovl_Door_Gerudo/z_door_gerudo.h"
#include "src/overlays/actors/ovl_En_Xc/z_en_xc.h"
#include "src/overlays/actors/ovl_Fishing/z_fishing.h"
#include "src/overlays/actors/ovl_En_Mk/z_en_mk.h"
#include "src/overlays/actors/ovl_En_Ge1/z_en_ge1.h"
#include "draw.h"

extern SaveContext gSaveContext;
extern PlayState* gPlayState;
extern void func_8084DFAC(PlayState* play, Player* player);
extern void Player_SetupActionPreserveAnimMovement(PlayState* play, Player* player, PlayerActionFunc actionFunc,
                                                   s32 flags);
extern s32 Player_SetupWaitForPutAway(PlayState* play, Player* player, AfterPutAwayFunc func);
extern void Play_InitEnvironment(PlayState* play, s16 skyboxId);
extern void EnMk_Wait(EnMk* enMk, PlayState* play);
extern void func_80ABA778(EnNiwLady* enNiwLady, PlayState* play);
extern void EnGe1_Wait_Archery(EnGe1* enGe1, PlayState* play);
extern void EnGe1_SetAnimationIdle(EnGe1* enGe1);
}

#define RAND_GET_OPTION(option) Rando::Context::GetInstance()->GetOption(option).Get()

bool LocMatchesQuest(Rando::Location loc) {
    if (loc.GetQuest() == RCQUEST_BOTH) {
        return true;
    } else {
        auto dungeon = OTRGlobals::Instance->gRandoContext->GetDungeons()->GetDungeonFromScene(loc.GetScene());
        return (dungeon->IsMQ() && loc.GetQuest() == RCQUEST_MQ) ||
               (dungeon->IsVanilla() && loc.GetQuest() == RCQUEST_VANILLA);
    }
}

RandomizerCheck GetRandomizerCheckFromFlag(int16_t flagType, int16_t flag) {
    for (auto& loc : Rando::StaticData::GetLocationTable()) {
        if ((loc.GetCollectionCheck().flag == flag &&
                 ((flagType == FLAG_INF_TABLE && loc.GetCollectionCheck().type == SPOILER_CHK_INF_TABLE) ||
                  (flagType == FLAG_EVENT_CHECK_INF && loc.GetCollectionCheck().type == SPOILER_CHK_EVENT_CHK_INF) ||
                  (flagType == FLAG_ITEM_GET_INF && loc.GetCollectionCheck().type == SPOILER_CHK_ITEM_GET_INF) ||
                  (flagType == FLAG_RANDOMIZER_INF && loc.GetCollectionCheck().type == SPOILER_CHK_RANDOMIZER_INF)) ||
             (loc.GetActorParams() == flag && flagType == FLAG_GS_TOKEN &&
              loc.GetCollectionCheck().type == SPOILER_CHK_GOLD_SKULLTULA)) &&
            LocMatchesQuest(loc)) {
            return loc.GetRandomizerCheck();
        }
    }

    return RC_UNKNOWN_CHECK;
}

RandomizerCheck GetRandomizerCheckFromSceneFlag(int16_t sceneNum, int16_t flagType, int16_t flag) {
    for (auto& loc : Rando::StaticData::GetLocationTable()) {
        if (loc.GetCollectionCheck().scene == sceneNum && loc.GetCollectionCheck().flag == flag &&
            ((flagType == FLAG_SCENE_TREASURE && loc.GetCollectionCheck().type == SPOILER_CHK_CHEST) ||
             (flagType == FLAG_SCENE_COLLECTIBLE && loc.GetCollectionCheck().type == SPOILER_CHK_COLLECTABLE) ||
             (flagType == FLAG_GS_TOKEN && loc.GetCollectionCheck().type == SPOILER_CHK_GOLD_SKULLTULA)) &&
            LocMatchesQuest(loc)) {
            return loc.GetRandomizerCheck();
        }
    }

    return RC_UNKNOWN_CHECK;
}

bool MeetsLACSRequirements() {
    switch (RAND_GET_OPTION(RSK_GANONS_BOSS_KEY)) {
        case RO_GANON_BOSS_KEY_LACS_STONES:
            if ((CheckStoneCount() + CheckLACSRewardCount()) >= RAND_GET_OPTION(RSK_LACS_STONE_COUNT)) {
                return true;
            }
            break;
        case RO_GANON_BOSS_KEY_LACS_MEDALLIONS:
            if ((CheckMedallionCount() + CheckLACSRewardCount()) >= RAND_GET_OPTION(RSK_LACS_MEDALLION_COUNT)) {
                return true;
            }
            break;
        case RO_GANON_BOSS_KEY_LACS_REWARDS:
            if ((CheckMedallionCount() + CheckStoneCount() + CheckLACSRewardCount()) >=
                RAND_GET_OPTION(RSK_LACS_REWARD_COUNT)) {
                return true;
            }
            break;
        case RO_GANON_BOSS_KEY_LACS_DUNGEONS:
            if ((CheckDungeonCount() + CheckLACSRewardCount()) >= RAND_GET_OPTION(RSK_LACS_DUNGEON_COUNT)) {
                return true;
            }
            break;
        case RO_GANON_BOSS_KEY_LACS_TOKENS:
            if (gSaveContext.inventory.gsTokens >= RAND_GET_OPTION(RSK_LACS_TOKEN_COUNT)) {
                return true;
            }
            break;
        default:
            if (CHECK_QUEST_ITEM(QUEST_MEDALLION_SPIRIT) && CHECK_QUEST_ITEM(QUEST_MEDALLION_SHADOW)) {
                return true;
            }
            break;
    }

    return false;
}

bool CompletedAllTrials() {
    return Flags_GetEventChkInf(EVENTCHKINF_COMPLETED_WATER_TRIAL) &&
           Flags_GetEventChkInf(EVENTCHKINF_COMPLETED_LIGHT_TRIAL) &&
           Flags_GetEventChkInf(EVENTCHKINF_COMPLETED_FIRE_TRIAL) &&
           Flags_GetEventChkInf(EVENTCHKINF_COMPLETED_SHADOW_TRIAL) &&
           Flags_GetEventChkInf(EVENTCHKINF_COMPLETED_SPIRIT_TRIAL) &&
           Flags_GetEventChkInf(EVENTCHKINF_COMPLETED_FOREST_TRIAL);
}

bool MeetsRainbowBridgeRequirements() {
    switch (RAND_GET_OPTION(RSK_RAINBOW_BRIDGE)) {
        case RO_BRIDGE_VANILLA: {
            if (CHECK_QUEST_ITEM(QUEST_MEDALLION_SPIRIT) && CHECK_QUEST_ITEM(QUEST_MEDALLION_SHADOW) &&
                (INV_CONTENT(ITEM_ARROW_LIGHT) == ITEM_ARROW_LIGHT)) {
                return true;
            }
            break;
        }
        case RO_BRIDGE_STONES: {
            if ((CheckStoneCount() + CheckBridgeRewardCount()) >= RAND_GET_OPTION(RSK_RAINBOW_BRIDGE_STONE_COUNT)) {
                return true;
            }
            break;
        }
        case RO_BRIDGE_MEDALLIONS: {
            if ((CheckMedallionCount() + CheckBridgeRewardCount()) >=
                RAND_GET_OPTION(RSK_RAINBOW_BRIDGE_MEDALLION_COUNT)) {
                return true;
            }
            break;
        }
        case RO_BRIDGE_DUNGEON_REWARDS: {
            if ((CheckMedallionCount() + CheckStoneCount() + CheckBridgeRewardCount()) >=
                RAND_GET_OPTION(RSK_RAINBOW_BRIDGE_REWARD_COUNT)) {
                return true;
            }
            break;
        }
        case RO_BRIDGE_DUNGEONS: {
            if ((CheckDungeonCount() + CheckBridgeRewardCount()) >= RAND_GET_OPTION(RSK_RAINBOW_BRIDGE_DUNGEON_COUNT)) {
                return true;
            }
            break;
        }
        case RO_BRIDGE_TOKENS: {
            if (gSaveContext.inventory.gsTokens >= RAND_GET_OPTION(RSK_RAINBOW_BRIDGE_TOKEN_COUNT)) {
                return true;
            }
            break;
        }
        case RO_BRIDGE_GREG: {
            if (Flags_GetRandomizerInf(RAND_INF_GREG_FOUND)) {
                return true;
            }
            break;
        }
        case RO_BRIDGE_ALWAYS_OPEN: {
            return true;
        }
    }

    return false;
}

// Todo Move this to randomizer context, clear it out on save load etc
static std::queue<RandomizerCheck> randomizerQueuedChecks;
static RandomizerCheck randomizerQueuedCheck = RC_UNKNOWN_CHECK;
static GetItemEntry randomizerQueuedItemEntry = GET_ITEM_NONE;

void RandomizerOnFlagSetHandler(int16_t flagType, int16_t flag) {
    // Consume adult trade items
    if (RAND_GET_OPTION(RSK_SHUFFLE_ADULT_TRADE) && flagType == FLAG_RANDOMIZER_INF) {
        switch (flag) {
            case RAND_INF_ADULT_TRADES_DMT_TRADE_BROKEN_SWORD:
                Flags_UnsetRandomizerInf(RAND_INF_ADULT_TRADES_HAS_SWORD_BROKEN);
                Inventory_ReplaceItem(gPlayState, ITEM_SWORD_BROKEN, Randomizer_GetNextAdultTradeItem());
                break;
            case RAND_INF_ADULT_TRADES_DMT_TRADE_EYEDROPS:
                Flags_UnsetRandomizerInf(RAND_INF_ADULT_TRADES_HAS_EYEDROPS);
                Inventory_ReplaceItem(gPlayState, ITEM_EYEDROPS, Randomizer_GetNextAdultTradeItem());
                break;
        }
    }

    if (flagType == FLAG_EVENT_CHECK_INF && flag == EVENTCHKINF_TALON_WOKEN_IN_CASTLE) {
        // remove chicken as this is the only use for it
        Flags_UnsetRandomizerInf(RAND_INF_CHILD_TRADES_HAS_CHICKEN);
    }

    if (flagType == FLAG_EVENT_CHECK_INF && flag == EVENTCHKINF_OBTAINED_ZELDAS_LETTER) {
        Flags_SetRandomizerInf(RAND_INF_ZELDAS_LETTER);
    }

    RandomizerCheck rc = GetRandomizerCheckFromFlag(flagType, flag);
    if (rc == RC_UNKNOWN_CHECK)
        return;

    if (flagType == FLAG_GS_TOKEN &&
        Rando::Context::GetInstance()->GetOption(RSK_SHUFFLE_TOKENS).Is(RO_TOKENSANITY_OFF))
        return;
    auto loc = Rando::Context::GetInstance()->GetItemLocation(rc);
    if (loc == nullptr || loc->HasObtained() || loc->GetPlacedRandomizerGet() == RG_NONE)
        return;

    SPDLOG_INFO("Queuing RC: {}", static_cast<uint32_t>(rc));
    randomizerQueuedChecks.push(rc);
}

void RandomizerOnSceneFlagSetHandler(int16_t sceneNum, int16_t flagType, int16_t flag) {
    if (RAND_GET_OPTION(RSK_SHUFFLE_DUNGEON_ENTRANCES) != RO_DUNGEON_ENTRANCE_SHUFFLE_OFF &&
        sceneNum == SCENE_GERUDOS_FORTRESS && flagType == FLAG_SCENE_SWITCH && flag == 0x3A) {
        Flags_SetRandomizerInf(RAND_INF_GF_GTG_GATE_PERMANENTLY_OPEN);
    }

    RandomizerCheck rc = GetRandomizerCheckFromSceneFlag(sceneNum, flagType, flag);
    if (rc == RC_UNKNOWN_CHECK)
        return;

    auto loc = Rando::Context::GetInstance()->GetItemLocation(rc);
    if (loc == nullptr || loc->HasObtained() || loc->GetPlacedRandomizerGet() == RG_NONE)
        return;

    SPDLOG_INFO("Queuing RC: {}", static_cast<uint32_t>(rc));
    randomizerQueuedChecks.push(rc);
}

static Vec3f spawnPos = { 0.0f, -999.0f, 0.0f };

void RandomizerOnPlayerUpdateForRCQueueHandler() {
    // If we're already queued, don't queue again
    if (randomizerQueuedCheck != RC_UNKNOWN_CHECK)
        return;

    // If there's nothing to queue, don't queue
    if (randomizerQueuedChecks.size() < 1)
        return;

    // If we're in a cutscene, don't queue
    Player* player = GET_PLAYER(gPlayState);
    if (Player_InBlockingCsMode(gPlayState, player) || player->stateFlags1 & PLAYER_STATE1_IN_ITEM_CS ||
        player->stateFlags1 & PLAYER_STATE1_GETTING_ITEM || player->stateFlags1 & PLAYER_STATE1_CARRYING_ACTOR) {
        return;
    }

    RandomizerCheck rc = randomizerQueuedChecks.front();
    auto loc = Rando::Context::GetInstance()->GetItemLocation(rc);
    RandomizerGet vanillaRandomizerGet = Rando::StaticData::GetLocation(rc)->GetVanillaItem();
    GetItemID vanillaItem = (GetItemID)Rando::StaticData::RetrieveItem(vanillaRandomizerGet).GetItemID();
    GetItemEntry getItemEntry =
        Rando::Context::GetInstance()->GetFinalGIEntry(rc, true, (GetItemID)vanillaRandomizerGet);

    if (loc->HasObtained()) {
        SPDLOG_INFO("RC {} already obtained, skipping", static_cast<uint32_t>(rc));
    } else {
        iceTrapScale = 0.0f;
        randomizerQueuedCheck = rc;
        randomizerQueuedItemEntry = getItemEntry;
        SPDLOG_INFO("Queueing Item mod {} item {} from RC {}", getItemEntry.modIndex, getItemEntry.itemId,
                    static_cast<uint32_t>(rc));
        if (
            // Skipping ItemGet animation incompatible with checks that require closing a text box to finish
            rc != RC_HF_OCARINA_OF_TIME_ITEM && rc != RC_SPIRIT_TEMPLE_SILVER_GAUNTLETS_CHEST &&
            rc != RC_MARKET_BOMBCHU_BOWLING_FIRST_PRIZE && rc != RC_MARKET_BOMBCHU_BOWLING_SECOND_PRIZE &&
            // Always show ItemGet animation for ice traps
            !(getItemEntry.modIndex == MOD_RANDOMIZER && getItemEntry.getItemId == RG_ICE_TRAP) &&
            // Always show ItemGet animation outside of randomizer to keep behaviour consistent in vanilla
            IS_RANDO &&
            (CVarGetInteger(CVAR_RANDOMIZER_ENHANCEMENT("TimeSavers.SkipGetItemAnimation"), SGIA_JUNK) == SGIA_ALL ||
             (CVarGetInteger(CVAR_RANDOMIZER_ENHANCEMENT("TimeSavers.SkipGetItemAnimation"), SGIA_JUNK) == SGIA_JUNK &&
              (
                  // crude fix to ensure map hints are readable. Ideally replace with better hint tracking.
                  !(getItemEntry.getItemId >= RG_DEKU_TREE_MAP && getItemEntry.getItemId <= RG_ICE_CAVERN_MAP &&
                    getItemEntry.modIndex == MOD_RANDOMIZER) &&
                  (getItemEntry.getItemCategory == ITEM_CATEGORY_JUNK ||
                   getItemEntry.getItemCategory == ITEM_CATEGORY_SKULLTULA_TOKEN ||
                   getItemEntry.getItemCategory == ITEM_CATEGORY_LESSER))))) {
            Item_DropCollectible(gPlayState, &spawnPos, ITEM00_SOH_GIVE_ITEM_ENTRY | 0x8000);
        }
    }

    randomizerQueuedChecks.pop();
}

void RandomizerOnPlayerUpdateForItemQueueHandler() {
    if (randomizerQueuedCheck == RC_UNKNOWN_CHECK)
        return;

    Player* player = GET_PLAYER(gPlayState);
    if (player == NULL || Player_InBlockingCsMode(gPlayState, player) ||
        player->stateFlags1 & PLAYER_STATE1_IN_ITEM_CS || player->stateFlags1 & PLAYER_STATE1_GETTING_ITEM ||
        player->stateFlags1 & PLAYER_STATE1_CARRYING_ACTOR) {
        return;
    }

    SPDLOG_INFO("Attempting to give Item mod {} item {} from RC {}", randomizerQueuedItemEntry.modIndex,
                randomizerQueuedItemEntry.itemId, static_cast<uint32_t>(randomizerQueuedCheck));
    GiveItemEntryWithoutActor(gPlayState, randomizerQueuedItemEntry);
    if (player->stateFlags1 & PLAYER_STATE1_IN_WATER) {
        // Allow the player to receive the item while swimming
        player->stateFlags2 |= PLAYER_STATE2_UNDERWATER;
        Player_ActionHandler_2(player, gPlayState);
    }
}

void RandomizerOnItemReceiveHandler(GetItemEntry receivedItemEntry) {
    if (randomizerQueuedCheck == RC_UNKNOWN_CHECK)
        return;

    auto loc = Rando::Context::GetInstance()->GetItemLocation(randomizerQueuedCheck);
    if (randomizerQueuedItemEntry.modIndex == receivedItemEntry.modIndex &&
        randomizerQueuedItemEntry.itemId == receivedItemEntry.itemId) {
        SPDLOG_INFO("Item received mod {} item {} from RC {}", receivedItemEntry.modIndex, receivedItemEntry.itemId,
                    static_cast<uint32_t>(randomizerQueuedCheck));
        loc->SetCheckStatus(RCSHOW_COLLECTED);
        CheckTracker::SpoilAreaFromCheck(randomizerQueuedCheck);
        CheckTracker::RecalculateAllAreaTotals();
        CheckTracker::RecalculateAvailableChecks();
        SaveManager::Instance->SaveSection(gSaveContext.fileNum, SECTION_ID_TRACKER_DATA, true);
        randomizerQueuedCheck = RC_UNKNOWN_CHECK;
        randomizerQueuedItemEntry = GET_ITEM_NONE;
    }

    if (receivedItemEntry.modIndex == MOD_NONE &&
        (receivedItemEntry.itemId == ITEM_HEART_PIECE || receivedItemEntry.itemId == ITEM_HEART_PIECE_2 ||
         receivedItemEntry.itemId == ITEM_HEART_CONTAINER)) {
        gSaveContext.healthAccumulator = 0x140; // Refill 20 hearts
        if ((s32)(gSaveContext.inventory.questItems & 0xF0000000) == 0x40000000) {
            gSaveContext.inventory.questItems ^= 0x40000000;
            gSaveContext.healthCapacity += 0x10;
            gSaveContext.health += 0x10;
        }
    }

    if (loc->GetRandomizerCheck() == RC_SPIRIT_TEMPLE_SILVER_GAUNTLETS_CHEST &&
        !CVarGetInteger(CVAR_ENHANCEMENT("TimeSavers.SkipCutscene.Story"), IS_RANDO)) {
        static uint32_t updateHook;
        updateHook = GameInteractor::Instance->RegisterGameHook<GameInteractor::OnPlayerUpdate>([]() {
            Player* player = GET_PLAYER(gPlayState);
            if (player == NULL || Player_InBlockingCsMode(gPlayState, player) ||
                player->stateFlags1 & PLAYER_STATE1_IN_ITEM_CS || player->stateFlags1 & PLAYER_STATE1_GETTING_ITEM ||
                player->stateFlags1 & PLAYER_STATE1_CARRYING_ACTOR) {
                return;
            }

            gPlayState->nextEntranceIndex = ENTR_DESERT_COLOSSUS_EAST_EXIT;
            gPlayState->transitionTrigger = TRANS_TRIGGER_START;
            gSaveContext.nextCutsceneIndex = 0xFFF1;
            gPlayState->transitionType = TRANS_TYPE_SANDSTORM_END;
            GET_PLAYER(gPlayState)->stateFlags1 &= ~PLAYER_STATE1_IN_CUTSCENE;
            Player_TryCsAction(gPlayState, NULL, 8);
            GameInteractor::Instance->UnregisterGameHook<GameInteractor::OnPlayerUpdate>(updateHook);
        });
    }
}

void EnExItem_DrawRandomizedItem(EnExItem* enExItem, PlayState* play) {
    GetItemEntry randoGetItem = enExItem->sohItemEntry;
    if (CVarGetInteger(CVAR_RANDOMIZER_ENHANCEMENT("MysteriousShuffle"), 0)) {
        randoGetItem = GET_ITEM_MYSTERY;
    }
    func_8002EBCC(&enExItem->actor, play, 0);
    func_8002ED80(&enExItem->actor, play, 0);
    EnItem00_CustomItemsParticles(&enExItem->actor, play, randoGetItem);
    GetItemEntry_Draw(play, randoGetItem);
}

void EnExItem_WaitForObjectRandomized(EnExItem* enExItem, PlayState* play) {
    EnExItem_WaitForObject(enExItem, play);
    if (Object_IsLoaded(&play->objectCtx, enExItem->objectIdx)) {
        enExItem->actor.draw = (ActorFunc)EnExItem_DrawRandomizedItem;
        Actor_SetScale(&enExItem->actor, enExItem->scale);

        // for now we're just using this to not have items float
        // below the bowling counter, but it would be nice to use
        // this to not draw gigantic skull tokens etc.
        switch (enExItem->type) {
            case EXITEM_BOMB_BAG_COUNTER: {
                enExItem->actor.shape.yOffset = -10.0f;
                break;
            }
        }
    }
}

void EnItem00_DrawRandomizedItem(EnItem00* enItem00, PlayState* play) {
    f32 mtxScale = CVarGetFloat(CVAR_RANDOMIZER_ENHANCEMENT("TimeSavers.SkipGetItemAnimationScale"), 10.0f);
    Matrix_Scale(mtxScale, mtxScale, mtxScale, MTXMODE_APPLY);
    GetItemEntry randoItem = enItem00->itemEntry;
    if (CVarGetInteger(CVAR_RANDOMIZER_ENHANCEMENT("MysteriousShuffle"), 0) &&
        enItem00->actor.params != ITEM00_SOH_GIVE_ITEM_ENTRY) {
        randoItem = GET_ITEM_MYSTERY;
    }
    func_8002EBCC(&enItem00->actor, play, 0);
    func_8002ED80(&enItem00->actor, play, 0);
    EnItem00_CustomItemsParticles(&enItem00->actor, play, randoItem);
    GetItemEntry_Draw(play, randoItem);
}

void ItemBHeart_DrawRandomizedItem(ItemBHeart* itemBHeart, PlayState* play) {
    GetItemEntry randoItem = itemBHeart->sohItemEntry;
    if (CVarGetInteger(CVAR_RANDOMIZER_ENHANCEMENT("MysteriousShuffle"), 0)) {
        randoItem = GET_ITEM_MYSTERY;
    }
    func_8002EBCC(&itemBHeart->actor, play, 0);
    func_8002ED80(&itemBHeart->actor, play, 0);
    EnItem00_CustomItemsParticles(&itemBHeart->actor, play, randoItem);
    GetItemEntry_Draw(play, randoItem);
}

void ItemBHeart_UpdateRandomizedItem(Actor* actor, PlayState* play) {
    ItemBHeart* itemBHeart = (ItemBHeart*)actor;

    func_80B85264(itemBHeart, play);
    Actor_UpdateBgCheckInfo(play, &itemBHeart->actor, 0.0f, 0.0f, 0.0f, 4);
    if ((itemBHeart->actor.xzDistToPlayer < 30.0f) && (fabsf(itemBHeart->actor.yDistToPlayer) < 40.0f)) {
        Flags_SetCollectible(play, 0x1F);
        Actor_Kill(&itemBHeart->actor);
    }
}

void ItemEtcetera_DrawRandomizedItem(ItemEtcetera* itemEtcetera, PlayState* play) {
    GetItemEntry randoItem = itemEtcetera->sohItemEntry;
    if (CVarGetInteger(CVAR_RANDOMIZER_ENHANCEMENT("MysteriousShuffle"), 0)) {
        randoItem = GET_ITEM_MYSTERY;
    }
    EnItem00_CustomItemsParticles(&itemEtcetera->actor, play, randoItem);
    func_8002EBCC(&itemEtcetera->actor, play, 0);
    func_8002ED80(&itemEtcetera->actor, play, 0);
    GetItemEntry_Draw(play, randoItem);
}

void ItemEtcetera_DrawRandomizedItemThroughLens(ItemEtcetera* itemEtcetera, PlayState* play) {
    if (play->actorCtx.lensActive) { // todo [Rando] mysterious shuffle for chest minigame key shuffle
        ItemEtcetera_DrawRandomizedItem(itemEtcetera, play);
    }
}

void ItemEtcetera_func_80B858B4_Randomized(ItemEtcetera* itemEtcetera, PlayState* play) {
    if (itemEtcetera->actor.xzDistToPlayer < 30.0f && fabsf(itemEtcetera->actor.yDistToPlayer) < 50.0f) {
        if ((itemEtcetera->actor.params & 0xFF) == 1) {
            Flags_SetEventChkInf(EVENTCHKINF_OBTAINED_RUTOS_LETTER);
            Flags_SetSwitch(play, 0xB);
        }

        Actor_Kill(&itemEtcetera->actor);
    } else {
        if ((play->gameplayFrames & 0xD) == 0) {
            EffectSsBubble_Spawn(play, &itemEtcetera->actor.world.pos, 0.0f, 0.0f, 10.0f, 0.13f);
        }
    }
}

void ItemEtcetera_func_80B85824_Randomized(ItemEtcetera* itemEtcetera, PlayState* play) {
    if ((itemEtcetera->actor.params & 0xFF) != 7) {
        return;
    }

    if (itemEtcetera->actor.xzDistToPlayer < 30.0f && fabsf(itemEtcetera->actor.yDistToPlayer) < 50.0f) {

        Flags_SetTreasure(play, 0x1F);
        Actor_Kill(&itemEtcetera->actor);
    }
}

void ItemEtcetera_MoveRandomizedFireArrowDown(ItemEtcetera* itemEtcetera, PlayState* play) {
    Actor_UpdateBgCheckInfo(play, &itemEtcetera->actor, 10.0f, 10.0f, 0.0f, 5);
    Actor_MoveXZGravity(&itemEtcetera->actor);
    if (!(itemEtcetera->actor.bgCheckFlags & 1)) {
        ItemEtcetera_SpawnSparkles(itemEtcetera, play);
    }
    itemEtcetera->actor.shape.rot.y += 0x400;
    ItemEtcetera_func_80B85824_Randomized(itemEtcetera, play);
}

void ItemEtcetera_UpdateRandomizedFireArrow(ItemEtcetera* itemEtcetera, PlayState* play) {
    if ((play->csCtx.state != CS_STATE_IDLE) && (play->csCtx.npcActions[0] != NULL)) {
        if (play->csCtx.npcActions[0]->action == 2) {
            itemEtcetera->actor.draw = (ActorFunc)ItemEtcetera_DrawRandomizedItem;
            itemEtcetera->actor.gravity = -0.1f;
            itemEtcetera->actor.minVelocityY = -4.0f;
            itemEtcetera->actionFunc = ItemEtcetera_MoveRandomizedFireArrowDown;
        }
    } else {
        itemEtcetera->actor.gravity = -0.1f;
        itemEtcetera->actor.minVelocityY = -4.0f;
        itemEtcetera->actionFunc = ItemEtcetera_MoveRandomizedFireArrowDown;
    }
}

u8 EnDs_RandoCanGetGrannyItem() {
    return (RAND_GET_OPTION(RSK_SHUFFLE_MERCHANTS) == RO_SHUFFLE_MERCHANTS_ALL_BUT_BEANS ||
            RAND_GET_OPTION(RSK_SHUFFLE_MERCHANTS) == RO_SHUFFLE_MERCHANTS_ALL) &&
           !Flags_GetRandomizerInf(RAND_INF_MERCHANTS_GRANNYS_SHOP) &&
           // Traded odd mushroom when adult trade is on
           ((RAND_GET_OPTION(RSK_SHUFFLE_ADULT_TRADE) && Flags_GetItemGetInf(ITEMGETINF_30)) ||
            // Found claim check when adult trade is off
            (!RAND_GET_OPTION(RSK_SHUFFLE_ADULT_TRADE) && INV_CONTENT(ITEM_CLAIM_CHECK) == ITEM_CLAIM_CHECK));
}

u8 EnJs_RandoCanGetCarpetMerchantItem() {
    return (RAND_GET_OPTION(RSK_SHUFFLE_MERCHANTS) == RO_SHUFFLE_MERCHANTS_ALL ||
            RAND_GET_OPTION(RSK_SHUFFLE_MERCHANTS) == RO_SHUFFLE_MERCHANTS_ALL_BUT_BEANS) &&
           // If the rando check has already been awarded, use vanilla behavior.
           !Flags_GetRandomizerInf(RAND_INF_MERCHANTS_CARPET_SALESMAN);
}

u8 EnGm_RandoCanGetMedigoronItem() {
    return (RAND_GET_OPTION(RSK_SHUFFLE_MERCHANTS) == RO_SHUFFLE_MERCHANTS_ALL ||
            RAND_GET_OPTION(RSK_SHUFFLE_MERCHANTS) == RO_SHUFFLE_MERCHANTS_ALL_BUT_BEANS) &&
           // If the rando check has already been awarded, use vanilla behavior.
           !Flags_GetRandomizerInf(RAND_INF_MERCHANTS_MEDIGORON);
}

void RandomizerSetChestGameRandomizerInf(RandomizerCheck rc) {
    switch (rc) {
        case RC_MARKET_TREASURE_CHEST_GAME_ITEM_1:
            Flags_SetRandomizerInf(RAND_INF_MARKET_TREASURE_CHEST_GAME_ITEM_1);
            break;
        case RC_MARKET_TREASURE_CHEST_GAME_ITEM_2:
            Flags_SetRandomizerInf(RAND_INF_MARKET_TREASURE_CHEST_GAME_ITEM_2);
            break;
        case RC_MARKET_TREASURE_CHEST_GAME_ITEM_3:
            Flags_SetRandomizerInf(RAND_INF_MARKET_TREASURE_CHEST_GAME_ITEM_3);
            break;
        case RC_MARKET_TREASURE_CHEST_GAME_ITEM_4:
            Flags_SetRandomizerInf(RAND_INF_MARKET_TREASURE_CHEST_GAME_ITEM_4);
            break;
        case RC_MARKET_TREASURE_CHEST_GAME_ITEM_5:
            Flags_SetRandomizerInf(RAND_INF_MARKET_TREASURE_CHEST_GAME_ITEM_5);
            break;
        case RC_MARKET_TREASURE_CHEST_GAME_KEY_1:
            Flags_SetRandomizerInf(RAND_INF_MARKET_TREASURE_CHEST_GAME_KEY_1);
            break;
        case RC_MARKET_TREASURE_CHEST_GAME_KEY_2:
            Flags_SetRandomizerInf(RAND_INF_MARKET_TREASURE_CHEST_GAME_KEY_2);
            break;
        case RC_MARKET_TREASURE_CHEST_GAME_KEY_3:
            Flags_SetRandomizerInf(RAND_INF_MARKET_TREASURE_CHEST_GAME_KEY_3);
            break;
        case RC_MARKET_TREASURE_CHEST_GAME_KEY_4:
            Flags_SetRandomizerInf(RAND_INF_MARKET_TREASURE_CHEST_GAME_KEY_4);
            break;
        case RC_MARKET_TREASURE_CHEST_GAME_KEY_5:
            Flags_SetRandomizerInf(RAND_INF_MARKET_TREASURE_CHEST_GAME_KEY_5);
            break;
        default:
            break;
    }
}

void Player_Action_8084E6D4_override(Player* player, PlayState* play) {
    if (LinkAnimation_Update(play, &player->skelAnime)) {
        func_8084DFAC(play, player);
    }
}

void func_8083A434_override(PlayState* play, Player* player) {
    Player_SetupActionPreserveAnimMovement(play, player, Player_Action_8084E6D4_override, 0);
    player->stateFlags1 |= PLAYER_STATE1_GETTING_ITEM | PLAYER_STATE1_IN_CUTSCENE;
}

bool ShouldGiveFishingPrize(f32 sFishOnHandLength) {
    // RANDOTODO: update the enhancement sliders to not allow
    // values above rando fish weight values when rando'd
    if (LINK_IS_CHILD) {
        int32_t weight = CVarGetInteger(CVAR_ENHANCEMENT("CustomizeFishing"), 0)
                             ? CVarGetInteger(CVAR_ENHANCEMENT("MinimumFishWeightChild"), 10)
                             : 10;
        f32 score = sqrt(((f32)weight - 0.5f) / 0.0036f);
        return sFishOnHandLength >= score && (IS_RANDO ? !Flags_GetRandomizerInf(RAND_INF_CHILD_FISHING)
                                                       : !(HIGH_SCORE(HS_FISHING) & HS_FISH_PRIZE_CHILD));
    } else {
        int32_t weight = CVarGetInteger(CVAR_ENHANCEMENT("CustomizeFishing"), 0)
                             ? CVarGetInteger(CVAR_ENHANCEMENT("MinimumFishWeightAdult"), 13)
                             : 13;
        f32 score = sqrt(((f32)weight - 0.5f) / 0.0036f);
        return sFishOnHandLength >= score && (IS_RANDO ? !Flags_GetRandomizerInf(RAND_INF_ADULT_FISHING)
                                                       : !(HIGH_SCORE(HS_FISHING) & HS_FISH_PRIZE_ADULT));
    }
}

void RandomizerOnDialogMessageHandler() {
    MessageContext* msgCtx = &gPlayState->msgCtx;
    Actor* actor = msgCtx->talkActor;
    auto ctx = Rando::Context::GetInstance();
    bool revealMerchant = ctx->GetOption(RSK_MERCHANT_TEXT_HINT).Get() != RO_GENERIC_OFF;
    bool nonBeanMerchants = ctx->GetOption(RSK_SHUFFLE_MERCHANTS).Is(RO_SHUFFLE_MERCHANTS_ALL_BUT_BEANS) ||
                            ctx->GetOption(RSK_SHUFFLE_MERCHANTS).Is(RO_SHUFFLE_MERCHANTS_ALL);

    RandomizerCheck reveal = RC_UNKNOWN_CHECK;
    if (ctx->GetOption(RSK_CHICKENS_HINT) &&
        (msgCtx->textId >= TEXT_ANJU_PLEASE_BRING_MY_CUCCOS_BACK && msgCtx->textId <= TEXT_ANJU_PLEASE_BRING_1_CUCCO)) {
        reveal = RC_KAK_ANJU_AS_CHILD;
    } else {
        switch (msgCtx->textId) {
            case TEXT_SKULLTULA_PEOPLE_IM_CURSED:
                if (actor->params == 1 && ctx->GetOption(RSK_KAK_10_SKULLS_HINT)) {
                    reveal = RC_KAK_10_GOLD_SKULLTULA_REWARD;
                } else if (actor->params == 2 && ctx->GetOption(RSK_KAK_20_SKULLS_HINT)) {
                    reveal = RC_KAK_20_GOLD_SKULLTULA_REWARD;
                } else if (actor->params == 3 && ctx->GetOption(RSK_KAK_30_SKULLS_HINT)) {
                    reveal = RC_KAK_30_GOLD_SKULLTULA_REWARD;
                } else if (actor->params == 4 && ctx->GetOption(RSK_KAK_40_SKULLS_HINT)) {
                    reveal = RC_KAK_40_GOLD_SKULLTULA_REWARD;
                } else if (ctx->GetOption(RSK_KAK_50_SKULLS_HINT)) {
                    reveal = RC_KAK_50_GOLD_SKULLTULA_REWARD;
                }
                break;
            case TEXT_SKULLTULA_PEOPLE_MAKE_YOU_VERY_RICH:
                if (ctx->GetOption(RSK_KAK_100_SKULLS_HINT)) {
                    reveal = RC_KAK_100_GOLD_SKULLTULA_REWARD;
                }
                break;
            case TEXT_MASK_SHOP_SIGN:
                if (ctx->GetOption(RSK_MASK_SHOP_HINT)) {
                    auto itemSkull_loc = ctx->GetItemLocation(RC_DEKU_THEATER_SKULL_MASK);
                    if (itemSkull_loc->GetCheckStatus() == RCSHOW_UNCHECKED) {
                        itemSkull_loc->SetCheckStatus(RCSHOW_IDENTIFIED);
                    }
                    reveal = RC_DEKU_THEATER_MASK_OF_TRUTH;
                }
                break;
            case TEXT_GHOST_SHOP_EXPLAINATION:
            case TEXT_GHOST_SHOP_CARD_HAS_POINTS:
                if (ctx->GetOption(RSK_BIG_POES_HINT)) {
                    reveal = RC_MARKET_10_BIG_POES;
                }
                break;
            case TEXT_MALON_EVERYONE_TURNING_EVIL:
            case TEXT_MALON_I_SING_THIS_SONG:
            case TEXT_MALON_HOW_IS_EPONA_DOING:
            case TEXT_MALON_OBSTICLE_COURSE:
            case TEXT_MALON_INGO_MUST_HAVE_BEEN_TEMPTED:
                if (ctx->GetOption(RSK_MALON_HINT)) {
                    reveal = RC_KF_LINKS_HOUSE_COW;
                }
                break;
            case TEXT_FROGS_UNDERWATER:
                if (ctx->GetOption(RSK_FROGS_HINT)) {
                    reveal = RC_ZR_FROGS_OCARINA_GAME;
                }
                break;
            case TEXT_GF_HBA_SIGN:
            case TEXT_HBA_NOT_ON_HORSE:
            case TEXT_HBA_INITIAL_EXPLAINATION:
            case TEXT_HBA_ALREADY_HAVE_1000:
                if (ctx->GetOption(RSK_HBA_HINT)) {
                    auto item1000_loc = ctx->GetItemLocation(RC_GF_HBA_1000_POINTS);
                    if (item1000_loc->GetCheckStatus() == RCSHOW_UNCHECKED) {
                        item1000_loc->SetCheckStatus(RCSHOW_IDENTIFIED);
                    }
                    reveal = RC_GF_HBA_1500_POINTS;
                }
                break;
            case TEXT_SCRUB_RANDOM:
                if (ctx->GetOption(RSK_SCRUB_TEXT_HINT).Get() != RO_GENERIC_OFF) {
                    EnDns* enDns = (EnDns*)actor;
                    reveal = OTRGlobals::Instance->gRandomizer->GetCheckFromRandomizerInf(
                        (RandomizerInf)enDns->sohScrubIdentity.randomizerInf);
                }
                break;
            case TEXT_BEAN_SALESMAN_BUY_FOR_10:
                if (revealMerchant && (ctx->GetOption(RSK_SHUFFLE_MERCHANTS).Is(RO_SHUFFLE_MERCHANTS_BEANS_ONLY) ||
                                       ctx->GetOption(RSK_SHUFFLE_MERCHANTS).Is(RO_SHUFFLE_MERCHANTS_ALL))) {
                    reveal = RC_ZR_MAGIC_BEAN_SALESMAN;
                }
                break;
            case TEXT_GRANNYS_SHOP:
                if (revealMerchant && nonBeanMerchants &&
                    (ctx->GetOption(RSK_SHUFFLE_ADULT_TRADE) || INV_CONTENT(ITEM_CLAIM_CHECK) == ITEM_CLAIM_CHECK)) {
                    reveal = RC_KAK_GRANNYS_SHOP;
                }
                break;
            case TEXT_MEDIGORON:
                if (revealMerchant && nonBeanMerchants) {
                    reveal = RC_GC_MEDIGORON;
                }
                break;
            case TEXT_CARPET_SALESMAN_1:
                if (revealMerchant && nonBeanMerchants) {
                    reveal = RC_WASTELAND_BOMBCHU_SALESMAN;
                }
                break;
            case TEXT_BIGGORON_BETTER_AT_SMITHING:
            case TEXT_BIGGORON_WAITING_FOR_YOU:
            case TEXT_BIGGORON_RETURN_AFTER_A_FEW_DAYS:
            case TEXT_BIGGORON_I_MAAAADE_THISSSS:
                if (ctx->GetOption(RSK_BIGGORON_HINT)) {
                    reveal = RC_DMT_TRADE_CLAIM_CHECK;
                }
                break;
            case TEXT_SHEIK_NEED_HOOK:
            case TEXT_SHEIK_HAVE_HOOK:
                if (ctx->GetOption(RSK_OOT_HINT) && gPlayState->sceneNum == SCENE_TEMPLE_OF_TIME &&
                    !ctx->GetItemLocation(RC_SONG_FROM_OCARINA_OF_TIME)->HasObtained()) {
                    auto itemoot_loc = ctx->GetItemLocation(RC_HF_OCARINA_OF_TIME_ITEM);
                    if (itemoot_loc->GetCheckStatus() == RCSHOW_UNCHECKED) {
                        itemoot_loc->SetCheckStatus(RCSHOW_IDENTIFIED);
                    }
                    reveal = RC_SONG_FROM_OCARINA_OF_TIME;
                }
                break;
            case TEXT_FISHING_CLOUDY:
            case TEXT_FISHING_TRY_ANOTHER_LURE:
            case TEXT_FISHING_SECRETS:
            case TEXT_FISHING_GOOD_FISHERMAN:
            case TEXT_FISHING_DIFFERENT_POND:
            case TEXT_FISHING_SCRATCHING:
            case TEXT_FISHING_TRY_ANOTHER_LURE_WITH_SINKING_LURE:
                if (ctx->GetOption(RSK_LOACH_HINT)) {
                    reveal = RC_LH_HYRULE_LOACH;
                }
                break;
        }
    }

    if (reveal != RC_UNKNOWN_CHECK) {
        auto item_loc = ctx->GetItemLocation(reveal);
        if (item_loc->GetCheckStatus() == RCSHOW_UNCHECKED) {
            item_loc->SetCheckStatus(RCSHOW_IDENTIFIED);
        }
    }
}

void RandomizerOnVanillaBehaviorHandler(GIVanillaBehavior id, bool* should, va_list originalArgs) {
    va_list args;
    va_copy(args, originalArgs);

    switch (id) {
        case VB_ALLOW_ENTRANCE_CS_FOR_EITHER_AGE: {
            s32 entranceIndex = va_arg(args, s32);

            // Allow Nabooru fight cutscene to play for child in rando
            if (entranceIndex == ENTR_SPIRIT_TEMPLE_BOSS_ENTRANCE) {
                *should = true;
            }
            break;
        }
        case VB_PLAY_SLOW_CHEST_CS: {
            // We force fast chests if SkipGetItemAnimation is enabled because the camera in the CS looks pretty wonky
            // otherwise
            if (CVarGetInteger(CVAR_RANDOMIZER_ENHANCEMENT("TimeSavers.SkipGetItemAnimation"), SGIA_JUNK)) {
                *should = false;
            }
            break;
        }
        case VB_GIVE_ITEM_FROM_CHEST: {
            EnBox* chest = va_arg(args, EnBox*);
            RandomizerCheck rc = OTRGlobals::Instance->gRandomizer->GetCheckFromActor(
                chest->dyna.actor.id, gPlayState->sceneNum, chest->dyna.actor.params);
            if (!OTRGlobals::Instance->gRandoContext->IsLocationShuffled(rc)) {
                break;
            }

            // if this is a treasure chest game chest then set the appropriate rando inf
            RandomizerSetChestGameRandomizerInf(rc);

            Player* player = GET_PLAYER(gPlayState);
            Player_SetupWaitForPutAway(gPlayState, player, func_8083A434_override);

            *should = false;
            break;
        }
        case VB_SPAWN_FIRE_ARROW:
            *should = !Flags_GetTreasure(gPlayState, 0x1F);
            break;
        case VB_PLAY_NABOORU_CAPTURED_CS:
            // This behavior is replicated for randomizer in RandomizerOnItemReceiveHandler
            *should = false;
            break;
        case VB_SHIEK_PREPARE_TO_GIVE_SERENADE_OF_WATER: {
            *should =
                !Flags_GetEventChkInf(EVENTCHKINF_LEARNED_SERENADE_OF_WATER) && !Flags_GetTreasure(gPlayState, 0x2);
            break;
        }
        case VB_BE_ELIGIBLE_FOR_SERENADE_OF_WATER:
            *should =
                !Flags_GetEventChkInf(EVENTCHKINF_LEARNED_SERENADE_OF_WATER) && Flags_GetTreasure(gPlayState, 0x2);
            break;
        case VB_BE_ELIGIBLE_FOR_PRELUDE_OF_LIGHT:
            *should =
                !Flags_GetEventChkInf(EVENTCHKINF_LEARNED_PRELUDE_OF_LIGHT) && CHECK_QUEST_ITEM(QUEST_MEDALLION_FOREST);
            break;
        case VB_MIDO_SPAWN:
            if (RAND_GET_OPTION(RSK_FOREST) != RO_CLOSED_FOREST_OFF &&
                !Flags_GetEventChkInf(EVENTCHKINF_SHOWED_MIDO_SWORD_SHIELD)) {
                *should = true;
            }
            break;
        case VB_MOVE_MIDO_IN_KOKIRI_FOREST:
            if (RAND_GET_OPTION(RSK_FOREST) == RO_CLOSED_FOREST_OFF && gSaveContext.cutsceneIndex == 0) {
                *should = true;
            }
            break;
        case VB_MIDO_CONSIDER_DEKU_TREE_DEAD:
            *should = Flags_GetEventChkInf(EVENTCHKINF_OBTAINED_KOKIRI_EMERALD_DEKU_TREE_DEAD);
            break;
        case VB_OPEN_KOKIRI_FOREST:
            *should = Flags_GetEventChkInf(EVENTCHKINF_OBTAINED_KOKIRI_EMERALD_DEKU_TREE_DEAD) ||
                      RAND_GET_OPTION(RSK_FOREST) != RO_CLOSED_FOREST_ON;
            break;
        case VB_BE_ELIGIBLE_FOR_DARUNIAS_JOY_REWARD:
            *should = !Flags_GetRandomizerInf(RAND_INF_DARUNIAS_JOY);
            break;
        case VB_BE_ELIGIBLE_FOR_LIGHT_ARROWS:
            *should = LINK_IS_ADULT && (gEntranceTable[gSaveContext.entranceIndex].scene == SCENE_TEMPLE_OF_TIME) &&
                      !Flags_GetEventChkInf(EVENTCHKINF_RETURNED_TO_TEMPLE_OF_TIME_WITH_ALL_MEDALLIONS) &&
                      MeetsLACSRequirements();
            break;
        case VB_BE_ELIGIBLE_FOR_NOCTURNE_OF_SHADOW:
            *should = !Flags_GetEventChkInf(EVENTCHKINF_BONGO_BONGO_ESCAPED_FROM_WELL) && LINK_IS_ADULT &&
                      gEntranceTable[((void)0, gSaveContext.entranceIndex)].scene == SCENE_KAKARIKO_VILLAGE &&
                      CHECK_QUEST_ITEM(QUEST_MEDALLION_FOREST) && CHECK_QUEST_ITEM(QUEST_MEDALLION_FIRE) &&
                      CHECK_QUEST_ITEM(QUEST_MEDALLION_WATER);
            break;
        case VB_BE_ELIGIBLE_FOR_CHILD_ROLLING_GORON_REWARD: {
            // Don't require a bomb bag to get prize in rando
            *should = true;
            break;
        }
        case VB_BE_ELIGIBLE_FOR_MAGIC_BEANS_PURCHASE: {
            if (RAND_GET_OPTION(RSK_SHUFFLE_MERCHANTS) == RO_SHUFFLE_MERCHANTS_BEANS_ONLY ||
                RAND_GET_OPTION(RSK_SHUFFLE_MERCHANTS) == RO_SHUFFLE_MERCHANTS_ALL) {
                *should = gSaveContext.rupees >=
                          OTRGlobals::Instance->gRandoContext->GetItemLocation(RC_ZR_MAGIC_BEAN_SALESMAN)->GetPrice();
            }
            break;
        }
        case VB_GIVE_ITEM_MASTER_SWORD:
            if (RAND_GET_OPTION(RSK_SHUFFLE_MASTER_SWORD)) {
                *should = false;
            } else {
                *should = true;
                Rando::Context::GetInstance()->GetItemLocation(RC_TOT_MASTER_SWORD)->SetCheckStatus(RCSHOW_COLLECTED);
                CheckTracker::RecalculateAllAreaTotals();
            }
            break;
        case VB_ITEM00_DESPAWN: {
            EnItem00* item00 = va_arg(args, EnItem00*);
            if (item00->actor.params == ITEM00_HEART_PIECE || item00->actor.params == ITEM00_SMALL_KEY) {
                RandomizerCheck rc = OTRGlobals::Instance->gRandomizer->GetCheckFromActor(
                    item00->actor.id, gPlayState->sceneNum, item00->ogParams);
                if (rc != RC_UNKNOWN_CHECK) {
                    item00->randoInf = RAND_INF_MAX;
                    item00->actor.params = ITEM00_SOH_DUMMY;
                    item00->itemEntry = Rando::Context::GetInstance()->GetFinalGIEntry(
                        rc, true, (GetItemID)Rando::StaticData::GetLocation(rc)->GetVanillaItem());
                    item00->actor.draw = (ActorFunc)EnItem00_DrawRandomizedItem;
                    *should = Rando::Context::GetInstance()->GetItemLocation(rc)->HasObtained();
                }
            } else if (item00->actor.params == ITEM00_SOH_GIVE_ITEM_ENTRY ||
                       item00->actor.params == ITEM00_SOH_GIVE_ITEM_ENTRY_GI) {
                GetItemEntry itemEntry = randomizerQueuedItemEntry;
                item00->itemEntry = itemEntry;
                item00->actor.draw = (ActorFunc)EnItem00_DrawRandomizedItem;
            }
            break;
        }
        case VB_ITEM_B_HEART_DESPAWN: {
            ItemBHeart* itemBHeart = va_arg(args, ItemBHeart*);
            RandomizerCheck rc = OTRGlobals::Instance->gRandomizer->GetCheckFromActor(
                itemBHeart->actor.id, gPlayState->sceneNum, itemBHeart->actor.params);
            if (rc != RC_UNKNOWN_CHECK) {
                itemBHeart->sohItemEntry = Rando::Context::GetInstance()->GetFinalGIEntry(
                    rc, true, (GetItemID)Rando::StaticData::GetLocation(rc)->GetVanillaItem());
                itemBHeart->actor.draw = (ActorFunc)ItemBHeart_DrawRandomizedItem;
                itemBHeart->actor.update = (ActorFunc)ItemBHeart_UpdateRandomizedItem;
                *should = Rando::Context::GetInstance()->GetItemLocation(rc)->HasObtained();
            }
            break;
        }
        case VB_MALON_ALREADY_TAUGHT_EPONAS_SONG: {
            *should = Flags_GetRandomizerInf(RAND_INF_LEARNED_EPONA_SONG);
            break;
        }
        case VB_SET_CUCCO_COUNT: {
            EnNiwLady* enNiwLady = va_arg(args, EnNiwLady*);
            // Override starting Cucco count using setting value
            enNiwLady->cuccosInPen = 7 - RAND_GET_OPTION(RSK_CUCCO_COUNT);
            *should = false;
            break;
        }
        case VB_KING_ZORA_THANK_CHILD: {
            // Allow turning in Ruto's letter even if you have already rescued her
            if (!Flags_GetEventChkInf(EVENTCHKINF_KING_ZORA_MOVED)) {
                GET_PLAYER(gPlayState)->exchangeItemId = EXCH_ITEM_LETTER_RUTO;
            }
            *should = Flags_GetEventChkInf(EVENTCHKINF_USED_JABU_JABUS_BELLY_BLUE_WARP);
            break;
        }
        case VB_BE_ABLE_TO_EXCHANGE_RUTOS_LETTER: {
            *should = LINK_IS_CHILD;
            break;
        }
        case VB_KING_ZORA_BE_MOVED: {
            *should = false;
            switch (RAND_GET_OPTION(RSK_ZORAS_FOUNTAIN)) {
                case RO_ZF_CLOSED:
                    if (Flags_GetEventChkInf(EVENTCHKINF_KING_ZORA_MOVED)) {
                        *should = true;
                    }
                    break;
                case RO_ZF_CLOSED_CHILD:
                    if (LINK_IS_ADULT) {
                        *should = true;
                    } else if (Flags_GetEventChkInf(EVENTCHKINF_KING_ZORA_MOVED)) {
                        *should = true;
                    }
                    break;
                case RO_ZF_OPEN:
                    *should = true;
                    break;
            }
            break;
        }
        case VB_KING_ZORA_TUNIC_CHECK: {
            if (!Flags_GetRandomizerInf(RAND_INF_KING_ZORA_THAWED)) {
                *should = false;
            }
            break;
        }
        case VB_BIGGORON_CONSIDER_SWORD_COLLECTED: {
            *should = Flags_GetRandomizerInf(RAND_INF_ADULT_TRADES_DMT_TRADE_CLAIM_CHECK);
            break;
        }
        case VB_BIGGORON_CONSIDER_TRADE_COMPLETE: {
            // This being true will prevent other biggoron trades, there are already safegaurds in place to prevent
            // claim check from being traded multiple times, so we don't really need the quest to ever be considered
            // "complete"
            *should = false;
            break;
        }
        case VB_GORONS_CONSIDER_FIRE_TEMPLE_FINISHED: {
            *should = Flags_GetEventChkInf(EVENTCHKINF_USED_FIRE_TEMPLE_BLUE_WARP);
            break;
        }
        case VB_GORONS_CONSIDER_DODONGOS_CAVERN_FINISHED: {
            *should = Flags_GetEventChkInf(EVENTCHKINF_USED_DODONGOS_CAVERN_BLUE_WARP);
            break;
        }
        case VB_GORONS_CONSIDER_TUNIC_COLLECTED: {
            *should = Flags_GetInfTable(INFTABLE_GORON_CITY_DOORS_UNLOCKED);
            break;
        }
        case VB_GIVE_ITEM_FROM_ITEM_00: {
            EnItem00* item00 = va_arg(args, EnItem00*);
            if (item00->actor.params == ITEM00_SOH_DUMMY) {
                if (item00->randoInf != RAND_INF_MAX) {
                    Flags_SetRandomizerInf(item00->randoInf);
                } else {
                    Flags_SetCollectible(gPlayState, item00->collectibleFlag);
                }
                Actor_Kill(&item00->actor);
                *should = false;
            } else if (item00->actor.params == ITEM00_SOH_GIVE_ITEM_ENTRY) {
                Audio_PlaySoundGeneral(NA_SE_SY_GET_ITEM, &gSfxDefaultPos, 4, &gSfxDefaultFreqAndVolScale,
                                       &gSfxDefaultFreqAndVolScale, &gSfxDefaultReverb);
                if (item00->itemEntry.modIndex == MOD_NONE) {
                    if (item00->itemEntry.getItemId == GI_SWORD_BGS) {
                        gSaveContext.bgsFlag = true;
                    }
                    Item_Give(gPlayState, item00->itemEntry.itemId);
                } else if (item00->itemEntry.modIndex == MOD_RANDOMIZER) {
                    if (item00->itemEntry.getItemId == RG_ICE_TRAP) {
                        gSaveContext.ship.pendingIceTrapCount++;
                    } else {
                        Randomizer_Item_Give(gPlayState, item00->itemEntry);
                    }
                }

                if (item00->itemEntry.modIndex == MOD_NONE) {
                    Notification::Emit({
                        .itemIcon = GetTextureForItemId(item00->itemEntry.itemId),
                        .message = "You found ",
                        .suffix = SohUtils::GetItemName(item00->itemEntry.itemId),
                    });
                } else if (item00->itemEntry.modIndex == MOD_RANDOMIZER) {
                    Notification::Emit({
                        .message = "You found ",
                        .suffix = Rando::StaticData::RetrieveItem((RandomizerGet)item00->itemEntry.getItemId)
                                      .GetName()
                                      .english,
                    });
                }

                // This is typically called when you close the text box after getting an item, in case a previous
                // function hid the interface.
                gSaveContext.unk_13EA = 0;
                Interface_ChangeAlpha(0x32);
                // EnItem00_SetupAction(item00, func_8001E5C8);
                // *should = false;
            } else if (item00->actor.params == ITEM00_SOH_GIVE_ITEM_ENTRY_GI) {
                if (!Actor_HasParent(&item00->actor, gPlayState)) {
                    GiveItemEntryFromActorWithFixedRange(&item00->actor, gPlayState, item00->itemEntry);
                }
                EnItem00_SetupAction(item00, func_8001E5C8);
                *should = false;
            }
            break;
        }
        case VB_BE_ELIGIBLE_FOR_SARIAS_SONG: {
            *should = !Flags_GetEventChkInf(EVENTCHKINF_LEARNED_SARIAS_SONG);
            break;
        }
        case VB_GIVE_ITEM_FROM_GRANNYS_SHOP: {
            if (!EnDs_RandoCanGetGrannyItem()) {
                break;
            }
            EnDs* granny = va_arg(args, EnDs*);
            // Only setting the inf if we've actually gotten the rando item and not the vanilla blue potion
            Flags_SetRandomizerInf(RAND_INF_MERCHANTS_GRANNYS_SHOP);
            granny->actor.parent = NULL;
            granny->actionFunc = EnDs_Talk;
            *should = false;
            break;
        }
        case VB_GIVE_ITEM_FROM_ANJU_AS_CHILD: {
            Flags_SetItemGetInf(ITEMGETINF_0C);
            *should = false;
            break;
        }
        case VB_GIVE_ITEM_FROM_ANJU_AS_ADULT: {
            EnNiwLady* enNiwLady = va_arg(args, EnNiwLady*);
            Flags_SetItemGetInf(ITEMGETINF_2C);
            enNiwLady->actionFunc = func_80ABA778;
            *should = false;
            break;
        }
        case VB_GIVE_ITEM_FROM_POE_COLLECTOR: {
            EnGb* enGb = va_arg(args, EnGb*);
            if (!Flags_GetRandomizerInf(RAND_INF_10_BIG_POES)) {
                Flags_SetRandomizerInf(RAND_INF_10_BIG_POES);
                enGb->dyna.actor.parent = NULL;
                enGb->actionFunc = func_80A2FC0C;
                *should = false;
            }
            break;
        }
        case VB_CHECK_RANDO_PRICE_OF_CARPET_SALESMAN: {
            if (EnJs_RandoCanGetCarpetMerchantItem()) {
                *should =
                    gSaveContext.rupees <
                    OTRGlobals::Instance->gRandoContext->GetItemLocation(RC_WASTELAND_BOMBCHU_SALESMAN)->GetPrice();
            }
            break;
        }
        case VB_GIVE_ITEM_FROM_CARPET_SALESMAN: {
            EnJs* enJs = va_arg(args, EnJs*);
            if (EnJs_RandoCanGetCarpetMerchantItem()) {
                Rupees_ChangeBy(
                    OTRGlobals::Instance->gRandoContext->GetItemLocation(RC_WASTELAND_BOMBCHU_SALESMAN)->GetPrice() *
                    -1);
                enJs->actor.parent = NULL;
                enJs->actor.textId = TEXT_CARPET_SALESMAN_ARMS_DEALER;
                enJs->actionFunc = (EnJsActionFunc)func_80A890C0;
                enJs->actor.flags |= ACTOR_FLAG_TALK_OFFER_AUTO_ACCEPTED;
                Flags_SetRandomizerInf(RAND_INF_MERCHANTS_CARPET_SALESMAN);
                *should = true;
            }
            break;
        }
        case VB_GIVE_BOMBCHUS_FROM_CARPET_SALESMAN: {
            *should = RAND_GET_OPTION(RSK_BOMBCHU_BAG) == false || INV_CONTENT(ITEM_BOMBCHU) == ITEM_BOMBCHU;
            break;
        }
        case VB_CHECK_RANDO_PRICE_OF_MEDIGORON: {
            if (EnGm_RandoCanGetMedigoronItem()) {
                *should = gSaveContext.rupees <
                          OTRGlobals::Instance->gRandoContext->GetItemLocation(RC_GC_MEDIGORON)->GetPrice();
            }
            break;
        }

        case VB_GIVE_ITEM_FROM_MEDIGORON:
        case VB_BE_ELIGIBLE_FOR_GIANTS_KNIFE_PURCHASE: {
            if (EnGm_RandoCanGetMedigoronItem()) {
                if (id == VB_GIVE_ITEM_FROM_MEDIGORON) {
                    EnGm* enGm = va_arg(args, EnGm*);
                    Flags_SetInfTable(INFTABLE_B1);
                    Flags_SetRandomizerInf(RAND_INF_MERCHANTS_MEDIGORON);
                    enGm->actor.parent = NULL;
                    enGm->actionFunc = (EnGmActionFunc)func_80A3DC44;
                    Rupees_ChangeBy(OTRGlobals::Instance->gRandoContext->GetItemLocation(RC_GC_MEDIGORON)->GetPrice() *
                                    -1);
                    *should = false;
                } else {
                    // Resets "Talked to Medigoron" flag in infTable to restore initial conversation state
                    Flags_UnsetInfTable(INFTABLE_B1);
                    *should = true;
                }
            }
            break;
        }
        case VB_GIVE_ITEM_FROM_MAGIC_BEAN_SALESMAN: {
            EnMs* enMs = va_arg(args, EnMs*);
            if (RAND_GET_OPTION(RSK_SHUFFLE_MERCHANTS) == RO_SHUFFLE_MERCHANTS_BEANS_ONLY ||
                RAND_GET_OPTION(RSK_SHUFFLE_MERCHANTS) == RO_SHUFFLE_MERCHANTS_ALL) {
                Rupees_ChangeBy(
                    OTRGlobals::Instance->gRandoContext->GetItemLocation(RC_ZR_MAGIC_BEAN_SALESMAN)->GetPrice() * -1);
                BEANS_BOUGHT = 10;
                // Only set inf for buying rando check
                Flags_SetRandomizerInf(RAND_INF_MERCHANTS_MAGIC_BEAN_SALESMAN);
                enMs->actionFunc = (EnMsActionFunc)EnMs_Wait;
                *should = false;
            }
            break;
        }
        case VB_FROGS_GO_TO_IDLE: {
            EnFr* enFr = va_arg(args, EnFr*);

            if ((enFr->songIndex >= FROG_STORMS && enFr->reward == GI_HEART_PIECE) ||
                (enFr->songIndex < FROG_STORMS && enFr->reward == GI_RUPEE_PURPLE)) {
                *should = true;
            }
            break;
        }
        case VB_TRADE_POCKET_CUCCO: {
            EnNiwLady* enNiwLady = va_arg(args, EnNiwLady*);
            Flags_UnsetRandomizerInf(RAND_INF_ADULT_TRADES_HAS_POCKET_CUCCO);
            Inventory_ReplaceItem(gPlayState, ITEM_POCKET_CUCCO, Randomizer_GetNextAdultTradeItem());
            // Trigger the reward now
            Flags_SetItemGetInf(ITEMGETINF_2E);
            enNiwLady->actionFunc = func_80ABA778;

            *should = false;
            break;
        }
        case VB_TRADE_COJIRO: {
            Flags_UnsetRandomizerInf(RAND_INF_ADULT_TRADES_HAS_COJIRO);
            Inventory_ReplaceItem(gPlayState, ITEM_COJIRO, Randomizer_GetNextAdultTradeItem());
            *should = false;
            break;
        }
        case VB_TRADE_ODD_MUSHROOM: {
            EnDs* granny = va_arg(args, EnDs*);
            Flags_UnsetRandomizerInf(RAND_INF_ADULT_TRADES_HAS_ODD_MUSHROOM);
            Inventory_ReplaceItem(gPlayState, ITEM_ODD_MUSHROOM, Randomizer_GetNextAdultTradeItem());
            // Trigger the reward now
            Flags_SetItemGetInf(ITEMGETINF_30);
            granny->actor.textId = 0x504F;
            granny->actionFunc = (EnDsActionFunc)EnDs_TalkAfterGiveOddPotion;
            granny->actor.flags &= ~ACTOR_FLAG_TALK;
            *should = false;
            break;
        }
        case VB_TRADE_ODD_POTION: {
            EnKo* enKo = va_arg(args, EnKo*);
            Flags_UnsetRandomizerInf(RAND_INF_ADULT_TRADES_HAS_ODD_POTION);
            Inventory_ReplaceItem(gPlayState, ITEM_ODD_POTION, Randomizer_GetNextAdultTradeItem());
            // Trigger the reward now
            Flags_SetItemGetInf(ITEMGETINF_31);
            *should = false;
            break;
        }
        case VB_TRADE_SAW: {
            Flags_UnsetRandomizerInf(RAND_INF_ADULT_TRADES_HAS_SAW);
            Inventory_ReplaceItem(gPlayState, ITEM_SAW, Randomizer_GetNextAdultTradeItem());
            *should = false;
            break;
        }
        case VB_ADULT_KING_ZORA_ITEM_GIVE: {
            EnKz* enKz = va_arg(args, EnKz*);
            Input input = gPlayState->state.input[0];

            if (CVarGetInteger(CVAR_ENHANCEMENT("EarlyEyeballFrog"), 0)) {
                // For early eyeball frog hook override, simulate collection delay behavior by just checking for the R
                // button being held while wearing a shield, and a trade item lower than frog in inventory
                bool hasShieldHoldingR = (CHECK_BTN_ANY(input.cur.button, BTN_R) &&
                                          CUR_EQUIP_VALUE(EQUIP_TYPE_SHIELD) > EQUIP_VALUE_SHIELD_NONE);

                if (func_8002F368(gPlayState) == EXCH_ITEM_PRESCRIPTION ||
                    (hasShieldHoldingR && INV_CONTENT(ITEM_TRADE_ADULT) < ITEM_FROG)) {
                    Flags_SetRandomizerInf(RAND_INF_ADULT_TRADES_ZD_TRADE_PRESCRIPTION);
                    Flags_UnsetRandomizerInf(RAND_INF_ADULT_TRADES_HAS_PRESCRIPTION);
                    Inventory_ReplaceItem(gPlayState, ITEM_PRESCRIPTION, Randomizer_GetNextAdultTradeItem());
                } else {
                    Flags_SetRandomizerInf(RAND_INF_KING_ZORA_THAWED);
                }
            } else {
                if (enKz->isTrading) {
                    Flags_SetRandomizerInf(RAND_INF_ADULT_TRADES_ZD_TRADE_PRESCRIPTION);
                    Flags_UnsetRandomizerInf(RAND_INF_ADULT_TRADES_HAS_PRESCRIPTION);
                    Inventory_ReplaceItem(gPlayState, ITEM_PRESCRIPTION, Randomizer_GetNextAdultTradeItem());
                } else {
                    Flags_SetRandomizerInf(RAND_INF_KING_ZORA_THAWED);
                }
            }
            *should = false;
            break;
        }
        case VB_TRADE_FROG: {
            Flags_UnsetRandomizerInf(RAND_INF_ADULT_TRADES_HAS_FROG);
            Inventory_ReplaceItem(gPlayState, ITEM_FROG, Randomizer_GetNextAdultTradeItem());
            *should = false;
            break;
        }
        case VB_BUSINESS_SCRUB_DESPAWN: {
            EnShopnuts* enShopnuts = va_arg(args, EnShopnuts*);
            s16 respawnData = gSaveContext.respawn[RESPAWN_MODE_RETURN].data & ((1 << 8) - 1);
            ScrubIdentity scrubIdentity = OTRGlobals::Instance->gRandomizer->IdentifyScrub(
                gPlayState->sceneNum, enShopnuts->actor.params, respawnData);

            if (scrubIdentity.isShuffled) {
                *should = Flags_GetRandomizerInf(scrubIdentity.randomizerInf);
            }
            break;
        }
        case VB_GIVE_ITEM_FROM_BUSINESS_SCRUB: {
            EnDns* enDns = va_arg(args, EnDns*);
            *should = !enDns->sohScrubIdentity.isShuffled;
            break;
        }
        // To explain the logic because Fado and Grog are linked:
        // - If you have Cojiro, then spawn Grog and not Fado.
        // - If you don't have Cojiro but do have Odd Potion, spawn Fado and not Grog.
        // - If you don't have either, spawn Grog if you haven't traded the Odd Mushroom.
        // - If you don't have either but have traded the mushroom, don't spawn either.
        case VB_DESPAWN_GROG: {
            if (!RAND_GET_OPTION(RSK_SHUFFLE_ADULT_TRADE)) {
                break;
            }
            if (Flags_GetRandomizerInf(RAND_INF_ADULT_TRADES_HAS_COJIRO)) {
                *should = false;
            } else if (Flags_GetRandomizerInf(RAND_INF_ADULT_TRADES_HAS_ODD_POTION)) {
                *should = true;
            } else {
                *should = Flags_GetItemGetInf(ITEMGETINF_30); // Traded odd mushroom
            }
            break;
        }
        case VB_SPAWN_LW_FADO: {
            if (!RAND_GET_OPTION(RSK_SHUFFLE_ADULT_TRADE)) {
                break;
            }

            if (Flags_GetRandomizerInf(RAND_INF_ADULT_TRADES_HAS_COJIRO)) {
                *should = false;
            } else {
                *should = Flags_GetRandomizerInf(RAND_INF_ADULT_TRADES_HAS_ODD_POTION);
            }

            break;
        }
        case VB_USE_EYEDROP_DIALOGUE: {
            // Skip eye drop text on rando if Link went in the water, so you can still receive the dive check
            EnMk* enMk = va_arg(args, EnMk*);
            *should &= enMk->swimFlag == 0;
            break;
        }
        case VB_OFFER_BLUE_POTION: {
            // Always offer blue potion when adult trade is off
            *should |= RAND_GET_OPTION(RSK_SHUFFLE_ADULT_TRADE) == RO_GENERIC_OFF;
            break;
        }
        case VB_GRANNY_SAY_INSUFFICIENT_RUPEES: {
            if (EnDs_RandoCanGetGrannyItem()) {
                *should = gSaveContext.rupees <
                          OTRGlobals::Instance->gRandoContext->GetItemLocation(RC_KAK_GRANNYS_SHOP)->GetPrice();
            }
            break;
        }
        case VB_GRANNY_TAKE_MONEY: {
            if (EnDs_RandoCanGetGrannyItem()) {
                *should = false;
                Rupees_ChangeBy(OTRGlobals::Instance->gRandoContext->GetItemLocation(RC_KAK_GRANNYS_SHOP)->GetPrice() *
                                -1);
            }
            break;
        }
        case VB_NEED_BOTTLE_FOR_GRANNYS_ITEM: {
            // Allow buying the rando item regardless of having a bottle
            *should &= !EnDs_RandoCanGetGrannyItem();
            break;
        }
        case VB_GIVE_ITEM_FROM_SHOOTING_GALLERY: {
            EnSyatekiMan* enSyatekiMan = va_arg(args, EnSyatekiMan*);
            enSyatekiMan->getItemId = GI_RUPEE_PURPLE;
            if (LINK_IS_ADULT) {
                // Give purple rupee if we've already obtained the reward OR we don't have a bow
                *should = Flags_GetItemGetInf(ITEMGETINF_0E) || CUR_UPG_VALUE(UPG_QUIVER) == 0;
            } else {
                // Give purple rupee if we've already obtained the reward
                *should = Flags_GetItemGetInf(ITEMGETINF_0D);
            }
            break;
        }
        case VB_BE_ELIGIBLE_FOR_ADULT_SHOOTING_GAME_REWARD: {
            *should = CUR_UPG_VALUE(UPG_QUIVER) > 0;
            if (!*should) {
                // In Rando without a quiver, display a message reminding the player to come back with a bow
                Message_StartTextbox(gPlayState, TEXT_SHOOTING_GALLERY_MAN_COME_BACK_WITH_BOW, NULL);
            }
            break;
        }
        case VB_BE_ELIGIBLE_TO_OPEN_DOT: {
            bool eligible =
                RAND_GET_OPTION(RSK_DOOR_OF_TIME) != RO_DOOROFTIME_CLOSED ||
                (INV_CONTENT(ITEM_OCARINA_FAIRY) == ITEM_OCARINA_TIME && CHECK_QUEST_ITEM(QUEST_KOKIRI_EMERALD) &&
                 CHECK_QUEST_ITEM(QUEST_GORON_RUBY) && CHECK_QUEST_ITEM(QUEST_ZORA_SAPPHIRE));
            *should = eligible;
            break;
        }
        case VB_GIVE_ITEM_FROM_HORSEBACK_ARCHERY: {
            EnGe1* enGe1 = va_arg(args, EnGe1*);
            // give both rewards at the same time
            if (gSaveContext.minigameScore >= 1000) {
                Flags_SetInfTable(INFTABLE_190);
            }
            if (gSaveContext.minigameScore >= 1500) {
                Flags_SetItemGetInf(ITEMGETINF_0F);
            }
            // move gerudo actor onto her wait loop
            enGe1->actionFunc = EnGe1_Wait_Archery;
            EnGe1_SetAnimationIdle(enGe1);
            // skip the vanilla gives.
            *should = false;
            break;
        }
        case VB_GIVE_ITEM_FROM_SKULLTULA_REWARD: {
            // In z_en_sth.c the rewards are stored in sGetItemIds, the first entry
            // in that array is GI_RUPEE_GOLD, and the reward is picked in EnSth_GivePlayerItem
            // via sGetItemIds[this->actor.params]. This means if actor.params == 0 we're looking
            // at the 100 GS reward
            EnSth* enSth = va_arg(args, EnSth*);
            if (enSth->actor.params == 0) {
                // if nothing is shuffled onto 100 GS,
                // or we already got the 100 GS reward,
                // let the player farm
                if (!RAND_GET_OPTION(RSK_SHUFFLE_100_GS_REWARD) ||
                    Flags_GetRandomizerInf(RAND_INF_KAK_100_GOLD_SKULLTULA_REWARD)) {
                    *should = true;
                    break;
                }

                // we're giving the 100 GS rando reward! set the rando inf
                Flags_SetRandomizerInf(RAND_INF_KAK_100_GOLD_SKULLTULA_REWARD);

                // also set the actionfunc so this doesn't immediately get
                // called again (and lead to a vanilla+rando item give
                // because the flag check will pass next time)
                enSth->actionFunc = (EnSthActionFunc)EnSth_RewardObtainedTalk;
            }
            *should = false;
            break;
        }
        case VB_GIVE_ITEM_FROM_OCARINA_MEMORY_GAME: {
            EnSkj* enSkj = va_arg(args, EnSkj*);
            Flags_SetItemGetInf(ITEMGETINF_17);
            enSkj->actionFunc = (EnSkjActionFunc)EnSkj_CleanupOcarinaGame;
            *should = false;
            break;
        }
        case VB_GIVE_ITEM_FROM_LOST_DOG: {
            EnHy* enHy = va_arg(args, EnHy*);
            Flags_SetInfTable(INFTABLE_191);
            gSaveContext.dogParams = 0;
            gSaveContext.dogIsLost = false;
            enHy->actionFunc = func_80A7127C;
            *should = false;
            break;
        }
        case VB_GIVE_ITEM_FROM_BOMBCHU_BOWLING: {
            EnBomBowlPit* enBomBowlPit = va_arg(args, EnBomBowlPit*);
            if (enBomBowlPit->prizeIndex == EXITEM_BOMB_BAG_BOWLING ||
                enBomBowlPit->prizeIndex == EXITEM_HEART_PIECE_BOWLING) {
                *should = false;
            }
            break;
        }
        case VB_GERUDOS_BE_FRIENDLY: {
            *should = CHECK_QUEST_ITEM(QUEST_GERUDO_CARD);
            break;
        }
        case VB_GTG_GATE_BE_OPEN: {
            if (Flags_GetRandomizerInf(RAND_INF_GF_GTG_GATE_PERMANENTLY_OPEN)) {
                *should = true;
            }
            break;
        }
        case VB_GIVE_ITEM_GERUDO_MEMBERSHIP_CARD: {
            Flags_SetRandomizerInf(RAND_INF_GF_ITEM_FROM_LEADER_OF_FORTRESS);
            *should = false;
            break;
        }
        case VB_BE_ELIGIBLE_FOR_RAINBOW_BRIDGE: {
            *should = MeetsRainbowBridgeRequirements();
            break;
        }
        case VB_PLAY_BLUE_WARP_CS: {
            // We need to override just these two temples because they check medallions instead of flags
            if (gPlayState->sceneNum == SCENE_SPIRIT_TEMPLE_BOSS) {
                *should = !Flags_GetRandomizerInf(RAND_INF_DUNGEONS_DONE_SPIRIT_TEMPLE);
            } else if (gPlayState->sceneNum == SCENE_SHADOW_TEMPLE_BOSS) {
                *should = !Flags_GetRandomizerInf(RAND_INF_DUNGEONS_DONE_SHADOW_TEMPLE);
            }
            break;
        }
        case VB_DRAW_AMMO_COUNT: {
            s16 item = *va_arg(args, s16*);
            // don't draw ammo count if you have the infinite upgrade
            if ((item == ITEM_NUT && Flags_GetRandomizerInf(RAND_INF_HAS_INFINITE_NUT_UPGRADE)) ||
                (item == ITEM_STICK && Flags_GetRandomizerInf(RAND_INF_HAS_INFINITE_STICK_UPGRADE)) ||
                (item == ITEM_BOMB && Flags_GetRandomizerInf(RAND_INF_HAS_INFINITE_BOMB_BAG)) ||
                ((item == ITEM_BOW || item == ITEM_BOW_ARROW_FIRE || item == ITEM_BOW_ARROW_ICE ||
                  item == ITEM_BOW_ARROW_LIGHT) &&
                 Flags_GetRandomizerInf(RAND_INF_HAS_INFINITE_QUIVER) && gPlayState->shootingGalleryStatus < 2 &&
                 gSaveContext.minigameState != 1) ||
                (item == ITEM_SLINGSHOT && Flags_GetRandomizerInf(RAND_INF_HAS_INFINITE_BULLET_BAG) &&
                 gPlayState->shootingGalleryStatus < 2) ||
                (item == ITEM_BOMBCHU && Flags_GetRandomizerInf(RAND_INF_HAS_INFINITE_BOMBCHUS) &&
                 gPlayState->bombchuBowlingStatus < 1)) {
                *should = false;
            }
            break;
        }
        case VB_HAVE_OCARINA_NOTE_A4: {
            if (!Flags_GetRandomizerInf(RAND_INF_HAS_OCARINA_C_RIGHT)) {
                *should = false;
            }
            break;
        }
        case VB_HAVE_OCARINA_NOTE_B4: {
            if (!Flags_GetRandomizerInf(RAND_INF_HAS_OCARINA_C_LEFT)) {
                *should = false;
            }
            break;
        }
        case VB_HAVE_OCARINA_NOTE_D4: {
            if (!Flags_GetRandomizerInf(RAND_INF_HAS_OCARINA_A)) {
                *should = false;
            }
            break;
        }
        case VB_HAVE_OCARINA_NOTE_D5: {
            if (!Flags_GetRandomizerInf(RAND_INF_HAS_OCARINA_C_UP)) {
                *should = false;
            }
            break;
        }
        case VB_HAVE_OCARINA_NOTE_F4: {
            if (!Flags_GetRandomizerInf(RAND_INF_HAS_OCARINA_C_DOWN)) {
                *should = false;
            }
            break;
        }
        case VB_SKIP_SCARECROWS_SONG: {
            int ocarinaButtonCount = 0;
            for (int i = VB_HAVE_OCARINA_NOTE_A4; i <= VB_HAVE_OCARINA_NOTE_F4; i++) {
                if (GameInteractor_Should((GIVanillaBehavior)i, true)) {
                    ocarinaButtonCount++;
                }
            }

            if (ocarinaButtonCount < 2) {
                *should = false;
                break;
            }

            if (gPlayState->msgCtx.msgMode == MSGMODE_OCARINA_PLAYING && RAND_GET_OPTION(RSK_SKIP_SCARECROWS_SONG)) {
                *should = true;
                break;
            }
            break;
        }
        case VB_RENDER_KEY_COUNTER: {
            if (Flags_GetRandomizerInf(RAND_INF_HAS_SKELETON_KEY)) {
                *should = false;
            }
            break;
        }
        case VB_RENDER_RUPEE_COUNTER: {
            if (!Flags_GetRandomizerInf(RAND_INF_HAS_WALLET) || Flags_GetRandomizerInf(RAND_INF_HAS_INFINITE_MONEY)) {
                *should = false;
            }
            break;
        }
        case VB_REVERT_SPOILING_ITEMS: {
            if (RAND_GET_OPTION(RSK_SHUFFLE_ADULT_TRADE)) {
                *should = false;
            }
            break;
        }
        case VB_BE_ABLE_TO_PLAY_BOMBCHU_BOWLING: {
            // Only check for bomb bag when bombchus aren't in logic
            // and only check for bombchus when bombchus are in logic
            *should = INV_CONTENT((RAND_GET_OPTION(RSK_BOMBCHU_BAG) ? ITEM_BOMBCHU : ITEM_BOMB)) != ITEM_NONE;
            break;
        }
        case VB_SHOULD_CHECK_FOR_FISHING_RECORD: {
            f32 sFishOnHandLength = *va_arg(args, f32*);
            *should = *should || ShouldGiveFishingPrize(sFishOnHandLength);
            break;
        }
        case VB_SHOULD_SET_FISHING_RECORD: {
            VBFishingData* fishData = va_arg(args, VBFishingData*);
            *should = (s16)fishData->sFishingRecordLength < (s16)fishData->fishWeight;
            if (!*should) {
                *fishData->sFishOnHandLength = 0.0f;
            }
            break;
        }
        case VB_SHOULD_GIVE_VANILLA_FISHING_PRIZE: {
            VBFishingData* fishData = va_arg(args, VBFishingData*);
            *should = !IS_RANDO && ShouldGiveFishingPrize(fishData->fishWeight);
            break;
        }
        case VB_GIVE_RANDO_FISHING_PRIZE: {
            if (IS_RANDO) {
                VBFishingData* fishData = va_arg(args, VBFishingData*);
                if (*fishData->sFishOnHandIsLoach) {
                    if (!Flags_GetRandomizerInf(RAND_INF_CAUGHT_LOACH) &&
                        OTRGlobals::Instance->gRandomizer->GetRandoSettingValue(RSK_FISHSANITY) ==
                            RO_FISHSANITY_HYRULE_LOACH) {
                        Flags_SetRandomizerInf(RAND_INF_CAUGHT_LOACH);
                        Message_StartTextbox(gPlayState, TEXT_FISHING_RELEASE_THIS_ONE, NULL);
                        *should = true;
                        fishData->actor->stateAndTimer = 20;
                    }
                } else {
                    if (ShouldGiveFishingPrize(fishData->fishWeight)) {
                        if (LINK_IS_CHILD) {
                            Flags_SetRandomizerInf(RAND_INF_CHILD_FISHING);
                            HIGH_SCORE(HS_FISHING) |= HS_FISH_PRIZE_CHILD;
                        } else {
                            Flags_SetRandomizerInf(RAND_INF_ADULT_FISHING);
                            HIGH_SCORE(HS_FISHING) |= HS_FISH_PRIZE_ADULT;
                        }
                        *should = true;
                        *fishData->sSinkingLureLocation = (u8)Rand_ZeroFloat(3.999f) + 1;
                        fishData->actor->stateAndTimer = 0;
                    }
                }
            }
            break;
        }
        case VB_GIVE_RANDO_GLITCH_FISHING_PRIZE: {
            if (IS_RANDO) {
                Fishing* fishing = va_arg(args, Fishing*);
                if (!Flags_GetRandomizerInf(RAND_INF_ADULT_FISHING)) {
                    Flags_SetRandomizerInf(RAND_INF_ADULT_FISHING);
                }
                *should = true;
                fishing->stateAndTimer = 0;
            }
            break;
        }
        case VB_TRADE_TIMER_EYEDROPS: {
            EnMk* enMk = va_arg(args, EnMk*);
            Flags_SetRandomizerInf(RAND_INF_ADULT_TRADES_LH_TRADE_FROG);
            enMk->actor.flags &= ~ACTOR_FLAG_TALK_OFFER_AUTO_ACCEPTED;
            enMk->actionFunc = EnMk_Wait;
            enMk->flags |= 1;
            *should = false;
            break;
        }
        // We need to override the vanilla behavior here because the player might sequence break and get Ruto kidnapped
        // before accessing other checks that require Ruto. So if she's kidnapped we allow her to spawn again
        case VB_RUTO_BE_CONSIDERED_NOT_KIDNAPPED: {
            *should = !Flags_GetInfTable(INFTABLE_145) || Flags_GetInfTable(INFTABLE_146);
            break;
        }
        case VB_SET_VOIDOUT_FROM_SURFACE: {
            // ENTRTODO: Move all entrance rando handling to a dedicated file
            std::vector<s16> entrPersistTempFlags = {
                ENTR_DEKU_TREE_BOSS_ENTRANCE,     ENTR_DEKU_TREE_BOSS_DOOR,        ENTR_DODONGOS_CAVERN_BOSS_ENTRANCE,
                ENTR_DODONGOS_CAVERN_BOSS_DOOR,   ENTR_JABU_JABU_BOSS_ENTRANCE,    ENTR_JABU_JABU_BOSS_DOOR,
                ENTR_FOREST_TEMPLE_BOSS_ENTRANCE, ENTR_FOREST_TEMPLE_BOSS_DOOR,    ENTR_FIRE_TEMPLE_BOSS_ENTRANCE,
                ENTR_FIRE_TEMPLE_BOSS_DOOR,       ENTR_WATER_TEMPLE_BOSS_ENTRANCE, ENTR_WATER_TEMPLE_BOSS_DOOR,
                ENTR_SPIRIT_TEMPLE_BOSS_ENTRANCE, ENTR_SPIRIT_TEMPLE_BOSS_DOOR,    ENTR_SHADOW_TEMPLE_BOSS_ENTRANCE,
                ENTR_SHADOW_TEMPLE_BOSS_DOOR,     ENTR_SPIRIT_TEMPLE_ENTRANCE,
            };

            s16 originalEntrance = (s16)va_arg(args, int);

            // In Entrance rando, if our respawnFlag is set for a grotto return, we don't want the void out to happen
            if (*should == true && RAND_GET_OPTION(RSK_SHUFFLE_ENTRANCES)) {
                // Check for dungeon special entrances that are randomized to a new location
                if (std::find(entrPersistTempFlags.begin(), entrPersistTempFlags.end(), originalEntrance) !=
                        entrPersistTempFlags.end() &&
                    originalEntrance != gPlayState->nextEntranceIndex) {
                    // Normally dungeons use a special voidout between scenes so that entering/exiting a boss room,
                    // or leaving via Spirit Hands and going back in persist temp flags across scenes.
                    // For ER, the temp flags should be wiped out so that they aren't transferred to the new location.
                    gPlayState->actorCtx.flags.tempSwch = 0;
                    gPlayState->actorCtx.flags.tempCollect = 0;

                    // If the respawnFlag is set for a grotto return, we don't want the void out to happen.
                    // Set the data flag to one to prevent the respawn point from being overridden by dungeon doors.
                    if (gSaveContext.respawnFlag == 2) {
                        gSaveContext.respawn[RESPAWN_MODE_DOWN].data = 1;
                        *should = false;
                    }
                }
            }
            break;
        }
        case VB_HEALTH_METER_BE_CRITICAL: {
            if (gSaveContext.health == gSaveContext.healthCapacity) {
                *should = false;
            }
            break;
        }
        case VB_HEISHI2_ACCEPT_ITEM_AS_ZELDAS_LETTER: {
            if (*should) {
                // remove zelda's letter as this is the only use for it
                Flags_UnsetRandomizerInf(RAND_INF_CHILD_TRADES_HAS_LETTER_ZELDA);
            }
            break;
        }
        case VB_FREEZE_ON_SKULL_TOKEN:
        case VB_TRADE_TIMER_ODD_MUSHROOM:
        case VB_TRADE_TIMER_FROG:
        case VB_GIVE_ITEM_FROM_TARGET_IN_WOODS:
        case VB_GIVE_ITEM_FROM_TALONS_CHICKENS:
        case VB_GIVE_ITEM_FROM_DIVING_MINIGAME:
        case VB_GIVE_ITEM_FROM_GORON:
        case VB_GIVE_ITEM_FROM_LAB_DIVE:
        case VB_GIVE_ITEM_FROM_SKULL_KID_SARIAS_SONG:
        case VB_GIVE_ITEM_FROM_MAN_ON_ROOF:
        case VB_GIVE_ITEM_FROM_BLUE_WARP:
        case VB_GIVE_ITEM_FAIRY_OCARINA:
        case VB_GIVE_ITEM_WEIRD_EGG:
        case VB_GIVE_ITEM_LIGHT_ARROW:
        case VB_GIVE_ITEM_STRENGTH_1:
        case VB_GIVE_ITEM_ZELDAS_LETTER:
        case VB_GIVE_ITEM_OCARINA_OF_TIME:
        case VB_GIVE_ITEM_LIGHT_MEDALLION:
        case VB_GIVE_ITEM_FOREST_MEDALLION:
        case VB_GIVE_ITEM_FIRE_MEDALLION:
        case VB_GIVE_ITEM_WATER_MEDALLION:
        case VB_GIVE_ITEM_SPIRIT_MEDALLION:
        case VB_GIVE_ITEM_SHADOW_MEDALLION:
            *should = false;
            break;
        case VB_GIVE_ITEM_SKULL_TOKEN:
            *should = (Rando::Context::GetInstance()->GetOption(RSK_SHUFFLE_TOKENS).Is(RO_TOKENSANITY_OFF));
            break;
        default:
            break;
    }

    va_end(args);
}

void RandomizerOnSceneInitHandler(int16_t sceneNum) {
    // Treasure Chest Game
    // todo: for now we're just unsetting all of them, we will
    //       probably need to do something different when we implement shuffle
    if (sceneNum == SCENE_TREASURE_BOX_SHOP) {
        Flags_UnsetRandomizerInf(RAND_INF_MARKET_TREASURE_CHEST_GAME_ITEM_1);
        Rando::Context::GetInstance()
            ->GetItemLocation(RC_MARKET_TREASURE_CHEST_GAME_ITEM_1)
            ->SetCheckStatus(RCSHOW_UNCHECKED);
        Flags_UnsetRandomizerInf(RAND_INF_MARKET_TREASURE_CHEST_GAME_ITEM_2);
        Rando::Context::GetInstance()
            ->GetItemLocation(RC_MARKET_TREASURE_CHEST_GAME_ITEM_2)
            ->SetCheckStatus(RCSHOW_UNCHECKED);
        Flags_UnsetRandomizerInf(RAND_INF_MARKET_TREASURE_CHEST_GAME_ITEM_3);
        Rando::Context::GetInstance()
            ->GetItemLocation(RC_MARKET_TREASURE_CHEST_GAME_ITEM_3)
            ->SetCheckStatus(RCSHOW_UNCHECKED);
        Flags_UnsetRandomizerInf(RAND_INF_MARKET_TREASURE_CHEST_GAME_ITEM_4);
        Rando::Context::GetInstance()
            ->GetItemLocation(RC_MARKET_TREASURE_CHEST_GAME_ITEM_4)
            ->SetCheckStatus(RCSHOW_UNCHECKED);
        Flags_UnsetRandomizerInf(RAND_INF_MARKET_TREASURE_CHEST_GAME_ITEM_5);
        Rando::Context::GetInstance()
            ->GetItemLocation(RC_MARKET_TREASURE_CHEST_GAME_ITEM_5)
            ->SetCheckStatus(RCSHOW_UNCHECKED);
        Flags_UnsetRandomizerInf(RAND_INF_MARKET_TREASURE_CHEST_GAME_KEY_1);
        Rando::Context::GetInstance()
            ->GetItemLocation(RC_MARKET_TREASURE_CHEST_GAME_KEY_1)
            ->SetCheckStatus(RCSHOW_UNCHECKED);
        Flags_UnsetRandomizerInf(RAND_INF_MARKET_TREASURE_CHEST_GAME_KEY_2);
        Rando::Context::GetInstance()
            ->GetItemLocation(RC_MARKET_TREASURE_CHEST_GAME_KEY_2)
            ->SetCheckStatus(RCSHOW_UNCHECKED);
        Flags_UnsetRandomizerInf(RAND_INF_MARKET_TREASURE_CHEST_GAME_KEY_3);
        Rando::Context::GetInstance()
            ->GetItemLocation(RC_MARKET_TREASURE_CHEST_GAME_KEY_3)
            ->SetCheckStatus(RCSHOW_UNCHECKED);
        Flags_UnsetRandomizerInf(RAND_INF_MARKET_TREASURE_CHEST_GAME_KEY_4);
        Rando::Context::GetInstance()
            ->GetItemLocation(RC_MARKET_TREASURE_CHEST_GAME_KEY_4)
            ->SetCheckStatus(RCSHOW_UNCHECKED);
        Flags_UnsetRandomizerInf(RAND_INF_MARKET_TREASURE_CHEST_GAME_KEY_5);
        Rando::Context::GetInstance()
            ->GetItemLocation(RC_MARKET_TREASURE_CHEST_GAME_KEY_5)
            ->SetCheckStatus(RCSHOW_UNCHECKED);
        CheckTracker::RecalculateAllAreaTotals();
    }

    // ENTRTODO: Move all entrance rando handling to a dedicated file
    if (RAND_GET_OPTION(RSK_SHUFFLE_ENTRANCES)) {
        // In ER, override roomNum to load based on scene and spawn during scene init
        if (gSaveContext.respawnFlag <= 0) {
            s8 origRoom = gPlayState->roomCtx.curRoom.num;
            s8 replacedRoom = Entrance_OverrideSpawnSceneRoom(gPlayState->sceneNum, gPlayState->curSpawn, origRoom);

            if (origRoom != replacedRoom) {
                // Reset room ctx back to prev room and then load the new room
                gPlayState->roomCtx.status = 0;
                gPlayState->roomCtx.curRoom = gPlayState->roomCtx.prevRoom;
                func_8009728C(gPlayState, &gPlayState->roomCtx, replacedRoom);
            }
        }

        // Handle updated link spawn positions
        Entrance_OverrideSpawnScene(sceneNum, gPlayState->curSpawn);

        Entrance_OverrideWeatherState();
        // Need to reinitialize the environment after replacing the weather mode
        Play_InitEnvironment(gPlayState, gPlayState->skyboxId);
    }

    // LACs & Prelude checks
    static uint32_t updateHook = 0;

    if (updateHook) {
        GameInteractor::Instance->UnregisterGameHook<GameInteractor::OnPlayerUpdate>(updateHook);
        updateHook = 0;
    }

    // If we're not in the Temple of Time or we've already learned the Prelude of Light and received LACs, we don't need
    // to do anything
    if (sceneNum != SCENE_TEMPLE_OF_TIME ||
        (Flags_GetEventChkInf(EVENTCHKINF_LEARNED_PRELUDE_OF_LIGHT) &&
         Flags_GetEventChkInf(EVENTCHKINF_RETURNED_TO_TEMPLE_OF_TIME_WITH_ALL_MEDALLIONS)))
        return;

    updateHook = GameInteractor::Instance->RegisterGameHook<GameInteractor::OnPlayerUpdate>([]() {
        if (!Flags_GetEventChkInf(EVENTCHKINF_LEARNED_PRELUDE_OF_LIGHT) && LINK_IS_ADULT &&
            CHECK_QUEST_ITEM(QUEST_MEDALLION_FOREST) && gPlayState->roomCtx.curRoom.num == 0) {
            Flags_SetEventChkInf(EVENTCHKINF_LEARNED_PRELUDE_OF_LIGHT);
        }

        // We're always in rando here, and rando always overrides this should so we can just pass false
        if (GameInteractor_Should(VB_BE_ELIGIBLE_FOR_LIGHT_ARROWS, false)) {
            Flags_SetEventChkInf(EVENTCHKINF_RETURNED_TO_TEMPLE_OF_TIME_WITH_ALL_MEDALLIONS);
        }

        // If both awards have been given, we can unregister the hook, otherwise it will get unregistered when the
        // player leaves the area
        if (Flags_GetEventChkInf(EVENTCHKINF_LEARNED_PRELUDE_OF_LIGHT) &&
            Flags_GetEventChkInf(EVENTCHKINF_RETURNED_TO_TEMPLE_OF_TIME_WITH_ALL_MEDALLIONS)) {
            GameInteractor::Instance->UnregisterGameHook<GameInteractor::OnPlayerUpdate>(updateHook);
            updateHook = 0;
        }
    });
}

void EnSi_DrawRandomizedItem(EnSi* enSi, PlayState* play) {
    GetItemEntry randoItem = enSi->sohGetItemEntry;
    if (CVarGetInteger(CVAR_RANDOMIZER_ENHANCEMENT("MysteriousShuffle"), 0)) {
        randoItem = GET_ITEM_MYSTERY;
    }
    func_8002ED80(&enSi->actor, play, 0);
    func_8002EBCC(&enSi->actor, play, 0);
    EnItem00_CustomItemsParticles(&enSi->actor, play, randoItem);
    GetItemEntry_Draw(play, randoItem);
}

u32 EnDns_RandomizerPurchaseableCheck(EnDns* enDns) {
    if (Flags_GetRandomizerInf(enDns->sohScrubIdentity.randomizerInf)) {
        return 3; // Can't get this now
    }
    if (gSaveContext.rupees < enDns->dnsItemEntry->itemPrice) {
        return 0; // Not enough rupees
    }
    return 4;
}

void EnDns_RandomizerPurchase(EnDns* enDns) {
    Rupees_ChangeBy(-enDns->dnsItemEntry->itemPrice);
    Flags_SetRandomizerInf(enDns->sohScrubIdentity.randomizerInf);
}

void ObjComb_RandomizerChooseItemDrop(ObjComb* objComb, PlayState* play) {
    s16 params = objComb->actor.params & 0x1F;

    if (RAND_GET_OPTION(RSK_SHUFFLE_BEEHIVES) && !Flags_GetRandomizerInf(objComb->beehiveIdentity.randomizerInf)) {
        EnItem00* item00 = (EnItem00*)Item_DropCollectible2(play, &objComb->actor.world.pos, ITEM00_SOH_DUMMY);
        item00->randoInf = objComb->beehiveIdentity.randomizerInf;
        item00->itemEntry =
            OTRGlobals::Instance->gRandomizer->GetItemFromKnownCheck(objComb->beehiveIdentity.randomizerCheck, GI_NONE);
        item00->actor.draw = (ActorFunc)EnItem00_DrawRandomizedItem;
        return;
    }

    if ((params > 0) || (params < 0x1A)) {
        if (params == 6) {
            if (Flags_GetCollectible(play, (objComb->actor.params >> 8) & 0x3F)) {
                params = -1;
            } else {
                params = (params | (((objComb->actor.params >> 8) & 0x3F) << 8));
            }
        } else if (Rand_ZeroOne() < 0.5f) {
            params = -1;
        }
        if (params >= 0 && !CVarGetInteger(CVAR_ENHANCEMENT("NoRandomDrops"), 0)) {
            Item_DropCollectible(play, &objComb->actor.world.pos, params);
        }
    }
}

void ObjComb_RandomizerWait(ObjComb* objComb, PlayState* play) {
    s32 dmgFlags;

    objComb->unk_1B0 -= 50;
    if (RAND_GET_OPTION(RSK_SHUFFLE_BEEHIVES) && !Flags_GetRandomizerInf(objComb->beehiveIdentity.randomizerInf)) {
        if (objComb->unk_1B0 <= -5000) {
            objComb->unk_1B0 = 1500;
        }
    } else if (objComb->unk_1B0 < 0) {
        objComb->unk_1B0 = 0;
    }

    if ((objComb->collider.base.acFlags & AC_HIT) != 0) {
        objComb->collider.base.acFlags &= ~AC_HIT;
        dmgFlags = objComb->collider.elements[0].info.acHitInfo->toucher.dmgFlags;
        if (dmgFlags & 0x4001F866) {
            objComb->unk_1B0 = 1500;
        } else {
            ObjComb_Break(objComb, play);
            ObjComb_RandomizerChooseItemDrop(objComb, play);
            Actor_Kill(&objComb->actor);
        }
    } else {
        CollisionCheck_SetAC(play, &play->colChkCtx, &objComb->collider.base);
    }

    if (objComb->actor.update != NULL) {
        CollisionCheck_SetOC(play, &play->colChkCtx, &objComb->collider.base);
    }
}

void RandomizerOnActorInitHandler(void* actorRef) {
    Actor* actor = static_cast<Actor*>(actorRef);

    if (actor->id == ACTOR_EN_SI) {
        RandomizerCheck rc =
            OTRGlobals::Instance->gRandomizer->GetCheckFromActor(actor->id, gPlayState->sceneNum, actor->params);
        if (rc != RC_UNKNOWN_CHECK) {
            EnSi* enSi = static_cast<EnSi*>(actorRef);
            enSi->sohGetItemEntry = Rando::Context::GetInstance()->GetFinalGIEntry(
                rc, true, (GetItemID)Rando::StaticData::GetLocation(rc)->GetVanillaItem());
            actor->draw = (ActorFunc)EnSi_DrawRandomizedItem;
        }
    }

    if (actor->id == ACTOR_EN_DNS) {
        EnDns* enDns = static_cast<EnDns*>(actorRef);
        s16 respawnData = gSaveContext.respawn[RESPAWN_MODE_RETURN].data & ((1 << 8) - 1);
        enDns->sohScrubIdentity =
            OTRGlobals::Instance->gRandomizer->IdentifyScrub(gPlayState->sceneNum, enDns->actor.params, respawnData);

        if (enDns->sohScrubIdentity.isShuffled) {
            // DNS uses pointers so we're creating our own entry instead of modifying the original
            enDns->sohDnsItemEntry = {
                enDns->dnsItemEntry->itemPrice, 1, enDns->sohScrubIdentity.getItemId, EnDns_RandomizerPurchaseableCheck,
                EnDns_RandomizerPurchase,
            };
            enDns->dnsItemEntry = &enDns->sohDnsItemEntry;

            if (enDns->sohScrubIdentity.itemPrice != -1) {
                enDns->dnsItemEntry->itemPrice = enDns->sohScrubIdentity.itemPrice;
            }

            enDns->actor.textId = TEXT_SCRUB_RANDOM;

            static uint32_t enDnsUpdateHook = 0;
            static uint32_t enDnsKillHook = 0;
            if (!enDnsUpdateHook) {
                enDnsUpdateHook =
                    GameInteractor::Instance->RegisterGameHook<GameInteractor::OnActorUpdate>([](void* innerActorRef) {
                        Actor* innerActor = static_cast<Actor*>(innerActorRef);
                        if (innerActor->id == ACTOR_EN_DNS) {
                            EnDns* innerEnDns = static_cast<EnDns*>(innerActorRef);
                            if (innerEnDns->sohScrubIdentity.isShuffled) {
                                innerActor->textId = TEXT_SCRUB_RANDOM;
                            }
                        }
                    });
                enDnsKillHook =
                    GameInteractor::Instance->RegisterGameHook<GameInteractor::OnSceneInit>([](int16_t sceneNum) {
                        GameInteractor::Instance->UnregisterGameHook<GameInteractor::OnActorUpdate>(enDnsUpdateHook);
                        GameInteractor::Instance->UnregisterGameHook<GameInteractor::OnSceneInit>(enDnsKillHook);
                        enDnsUpdateHook = 0;
                        enDnsKillHook = 0;
                    });
            }
        }
    }

    if (actor->id == ACTOR_ITEM_ETCETERA) {
        ItemEtcetera* itemEtcetera = static_cast<ItemEtcetera*>(actorRef);
        RandomizerCheck rc = OTRGlobals::Instance->gRandomizer->GetCheckFromActor(
            itemEtcetera->actor.id, gPlayState->sceneNum, itemEtcetera->actor.params);
        if (rc != RC_UNKNOWN_CHECK) {
            itemEtcetera->sohItemEntry = Rando::Context::GetInstance()->GetFinalGIEntry(
                rc, true, (GetItemID)Rando::StaticData::GetLocation(rc)->GetVanillaItem());
            itemEtcetera->drawFunc = (ActorFunc)ItemEtcetera_DrawRandomizedItem;
        }

        int32_t type = itemEtcetera->actor.params & 0xFF;
        switch (type) {
            case ITEM_ETC_LETTER: {
                itemEtcetera->futureActionFunc = (ItemEtceteraActionFunc)ItemEtcetera_func_80B858B4_Randomized;
                break;
            }
            case ITEM_ETC_ARROW_FIRE: {
                itemEtcetera->futureActionFunc = (ItemEtceteraActionFunc)ItemEtcetera_UpdateRandomizedFireArrow;
                break;
            }
            case ITEM_ETC_RUPEE_GREEN_CHEST_GAME:
            case ITEM_ETC_RUPEE_BLUE_CHEST_GAME:
            case ITEM_ETC_RUPEE_RED_CHEST_GAME:
            case ITEM_ETC_RUPEE_PURPLE_CHEST_GAME:
            case ITEM_ETC_HEART_PIECE_CHEST_GAME:
            case ITEM_ETC_KEY_SMALL_CHEST_GAME: {
                if (rc != RC_UNKNOWN_CHECK) {
                    itemEtcetera->drawFunc = (ActorFunc)ItemEtcetera_DrawRandomizedItemThroughLens;
                }
                break;
            }
        }
    }

    if (actor->id == ACTOR_OBJ_COMB) {
        ObjComb* objComb = static_cast<ObjComb*>(actorRef);
        s16 respawnData = gSaveContext.respawn[RESPAWN_MODE_RETURN].data & ((1 << 8) - 1);
        objComb->beehiveIdentity = OTRGlobals::Instance->gRandomizer->IdentifyBeehive(
            gPlayState->sceneNum, (s16)actor->world.pos.x, respawnData);
        objComb->actionFunc = (ObjCombActionFunc)ObjComb_RandomizerWait;
    }

    if (actor->id == ACTOR_EN_EX_ITEM) {
        EnExItem* enExItem = static_cast<EnExItem*>(actorRef);

        RandomizerCheck rc = RC_UNKNOWN_CHECK;
        switch (enExItem->type) {
            case EXITEM_BOMB_BAG_COUNTER:
            case EXITEM_BOMB_BAG_BOWLING:
                rc = RC_MARKET_BOMBCHU_BOWLING_FIRST_PRIZE;
                break;
            case EXITEM_HEART_PIECE_COUNTER:
            case EXITEM_HEART_PIECE_BOWLING:
                rc = RC_MARKET_BOMBCHU_BOWLING_SECOND_PRIZE;
                break;
            case EXITEM_BOMBCHUS_COUNTER:
            case EXITEM_BOMBCHUS_BOWLING:
                // RC_MARKET_BOMBCHU_BOWLING_BOMBCHUS was removed as a 3DS holdover not in anyones near term plans due
                // to being pretty useless.
                break;
            case EXITEM_BULLET_BAG:
                rc = RC_LW_TARGET_IN_WOODS;
                break;
        }
        if (rc != RC_UNKNOWN_CHECK) {
            enExItem->sohItemEntry = Rando::Context::GetInstance()->GetFinalGIEntry(
                rc, true, (GetItemID)Rando::StaticData::GetLocation(rc)->GetVanillaItem());
            enExItem->actionFunc = (EnExItemActionFunc)EnExItem_WaitForObjectRandomized;
        }
    }

    if (actor->id == ACTOR_EN_GE1) {
        EnGe1* enGe1 = static_cast<EnGe1*>(actorRef);
        auto ge1Type = enGe1->actor.params & 0xFF;
        if (ge1Type == GE1_TYPE_TRAINING_GROUND_GUARD &&
            Flags_GetRandomizerInf(RAND_INF_GF_GTG_GATE_PERMANENTLY_OPEN)) {
            enGe1->actionFunc = (EnGe1ActionFunc)EnGe1_SetNormalText;
        }
    }

    if (actor->id == ACTOR_DEMO_KEKKAI && actor->params == 0) { // 0 == KEKKAI_TOWER
        if (CompletedAllTrials()) {
            Actor_Kill(actor);
        }
    }

    if (actor->id == ACTOR_BG_TREEMOUTH && LINK_IS_ADULT &&
        RAND_GET_OPTION(RSK_SHUFFLE_DUNGEON_ENTRANCES) != RO_DUNGEON_ENTRANCE_SHUFFLE_OFF &&
        (RAND_GET_OPTION(RSK_FOREST) == RO_CLOSED_FOREST_OFF ||
         Flags_GetEventChkInf(EVENTCHKINF_SHOWED_MIDO_SWORD_SHIELD))) {
        BgTreemouth* bgTreemouth = static_cast<BgTreemouth*>(actorRef);
        bgTreemouth->unk_168 = 1.0f;
    }

    // consumable bags
    if (actor->id == ACTOR_EN_ITEM00 &&
        ((RAND_GET_OPTION(RSK_SHUFFLE_DEKU_STICK_BAG) && CUR_UPG_VALUE(UPG_STICKS) == 0 &&
          actor->params == ITEM00_STICK) ||
         (RAND_GET_OPTION(RSK_SHUFFLE_DEKU_NUT_BAG) && CUR_UPG_VALUE(UPG_NUTS) == 0 && actor->params == ITEM00_NUTS))) {
        Actor_Kill(actor);
    }

    if (RAND_GET_OPTION(RSK_SHUFFLE_BOSS_SOULS)) {
        // Boss souls require an additional item (represented by a RAND_INF) to spawn a boss in a particular lair
        RandomizerInf currentBossSoulRandInf = RAND_INF_MAX;
        switch (gPlayState->sceneNum) {
            case SCENE_DEKU_TREE_BOSS:
                currentBossSoulRandInf = RAND_INF_GOHMA_SOUL;
                break;
            case SCENE_DODONGOS_CAVERN_BOSS:
                currentBossSoulRandInf = RAND_INF_KING_DODONGO_SOUL;
                break;
            case SCENE_JABU_JABU_BOSS:
                currentBossSoulRandInf = RAND_INF_BARINADE_SOUL;
                break;
            case SCENE_FOREST_TEMPLE_BOSS:
                currentBossSoulRandInf = RAND_INF_PHANTOM_GANON_SOUL;
                break;
            case SCENE_FIRE_TEMPLE_BOSS:
                currentBossSoulRandInf = RAND_INF_VOLVAGIA_SOUL;
                break;
            case SCENE_WATER_TEMPLE_BOSS:
                currentBossSoulRandInf = RAND_INF_MORPHA_SOUL;
                break;
            case SCENE_SHADOW_TEMPLE_BOSS:
                currentBossSoulRandInf = RAND_INF_BONGO_BONGO_SOUL;
                break;
            case SCENE_SPIRIT_TEMPLE_BOSS:
                currentBossSoulRandInf = RAND_INF_TWINROVA_SOUL;
                break;
            case SCENE_GANONDORF_BOSS:
            case SCENE_GANON_BOSS:
                if (RAND_GET_OPTION(RSK_SHUFFLE_BOSS_SOULS) == RO_BOSS_SOULS_ON_PLUS_GANON) {
                    currentBossSoulRandInf = RAND_INF_GANON_SOUL;
                }
                break;
            default:
                break;
        }

        // Deletes all actors in the boss category if the soul isn't found.
        // Some actors, like Dark Link, Arwings, and Zora's Sapphire...?, are in this category despite not being actual
        // bosses, so ignore any "boss" if `currentBossSoulRandInf` doesn't change from RAND_INF_MAX. Iron Knuckle
        // (Nabooru) in Twinrova's room is a special exception, so exclude knuckles too.
        if (currentBossSoulRandInf != RAND_INF_MAX) {
            if (!Flags_GetRandomizerInf(currentBossSoulRandInf) && actor->category == ACTORCAT_BOSS &&
                actor->id != ACTOR_EN_IK) {
                Actor_Delete(&gPlayState->actorCtx, actor, gPlayState);
            }
            // Special case for Phantom Ganon's horse (and fake), as they're considered "background actors",
            // but still control the boss fight flow.
            if (!Flags_GetRandomizerInf(RAND_INF_PHANTOM_GANON_SOUL) && actor->id == ACTOR_EN_FHG) {
                Actor_Delete(&gPlayState->actorCtx, actor, gPlayState);
            }
        }
    }

    // In MQ Spirit, remove the large silver block in the hole as child so the chest in the silver block hallway
    // can be guaranteed accessible
    if (actor->id == ACTOR_OBJ_OSHIHIKI && LINK_IS_CHILD && ResourceMgr_IsGameMasterQuest() &&
        gPlayState->sceneNum == SCENE_SPIRIT_TEMPLE && actor->room == 6 && // Spirit Temple silver block hallway
        actor->params == 0x9C7                                             // Silver block that is marked as in the hole
    ) {
        Actor_Kill(actor);
        return;
    }

    // If child is in the adult shooting gallery or adult in the child shooting gallery, then despawn the shooting
    // gallery man
    if (actor->id == ACTOR_EN_SYATEKI_MAN && RAND_GET_OPTION(RSK_SHUFFLE_INTERIOR_ENTRANCES) &&
        ((LINK_IS_CHILD &&
          // Kakariko Village -> Adult Shooting Gallery, index 003B in the entrance table
          Entrance_SceneAndSpawnAre(SCENE_SHOOTING_GALLERY, 0x00)) ||
         (LINK_IS_ADULT &&
          // Market -> Child Shooting Gallery,           index 016D in the entrance table
          Entrance_SceneAndSpawnAre(SCENE_SHOOTING_GALLERY, 0x01)))) {
        Actor_Kill(actor);
        return;
    }

    // In ER, once Link has spawned we know the scene has loaded, so we can sanitize the last known entrance type
    if (actor->id == ACTOR_PLAYER && RAND_GET_OPTION(RSK_SHUFFLE_ENTRANCES)) {
        Grotto_SanitizeEntranceType();
    }
}

void RandomizerOnGameFrameUpdateHandler() {
    if (Flags_GetRandomizerInf(RAND_INF_HAS_INFINITE_QUIVER)) {
        AMMO(ITEM_BOW) = CUR_CAPACITY(UPG_QUIVER);
    }

    if (Flags_GetRandomizerInf(RAND_INF_HAS_INFINITE_BOMB_BAG)) {
        AMMO(ITEM_BOMB) = CUR_CAPACITY(UPG_BOMB_BAG);
    }

    if (Flags_GetRandomizerInf(RAND_INF_HAS_INFINITE_BULLET_BAG)) {
        AMMO(ITEM_SLINGSHOT) = CUR_CAPACITY(UPG_BULLET_BAG);
    }

    if (Flags_GetRandomizerInf(RAND_INF_HAS_INFINITE_STICK_UPGRADE)) {
        AMMO(ITEM_STICK) = CUR_CAPACITY(UPG_STICKS);
    }

    if (Flags_GetRandomizerInf(RAND_INF_HAS_INFINITE_NUT_UPGRADE)) {
        AMMO(ITEM_NUT) = CUR_CAPACITY(UPG_NUTS);
    }

    if (Flags_GetRandomizerInf(RAND_INF_HAS_INFINITE_MAGIC_METER)) {
        gSaveContext.magic = gSaveContext.magicCapacity;
    }

    if (Flags_GetRandomizerInf(RAND_INF_HAS_INFINITE_BOMBCHUS)) {
        AMMO(ITEM_BOMBCHU) = 50;
    }

    if (Flags_GetRandomizerInf(RAND_INF_HAS_INFINITE_MONEY)) {
        gSaveContext.rupees = CUR_CAPACITY(UPG_WALLET);
    }

    if (!Flags_GetRandomizerInf(RAND_INF_HAS_WALLET)) {
        gSaveContext.rupees = 0;
    }
}

extern "C" void func_8099485C(DoorGerudo* gerudoDoor, PlayState* play);

void RandomizerOnActorUpdateHandler(void* refActor) {
    Actor* actor = static_cast<Actor*>(refActor);

    if (Flags_GetRandomizerInf(RAND_INF_HAS_SKELETON_KEY)) {
        if (actor->id == ACTOR_EN_DOOR) {
            EnDoor* door = reinterpret_cast<EnDoor*>(actor);
            door->lockTimer = 0;
        } else if (actor->id == ACTOR_DOOR_SHUTTER) {
            DoorShutter* shutterDoor = reinterpret_cast<DoorShutter*>(actor);
            if (shutterDoor->doorType == SHUTTER_KEY_LOCKED) {
                shutterDoor->unk_16E = 0;
            }
        } else if (actor->id == ACTOR_DOOR_GERUDO) {
            DoorGerudo* gerudoDoor = (DoorGerudo*)actor;
            gerudoDoor->actionFunc = func_8099485C;
            gerudoDoor->dyna.actor.world.pos.y = gerudoDoor->dyna.actor.home.pos.y + 200.0f;
        }
    }

    // In ER, override the warp song locations. Also removes the warp song cutscene
    if (RAND_GET_OPTION(RSK_SHUFFLE_ENTRANCES) && actor->id == ACTOR_DEMO_KANKYO &&
        actor->params == 0x000F) { // Warp Song particles
        Entrance_SetWarpSongEntrance();
    }

    if (actor->id == ACTOR_OBJ_COMB) {
        ObjComb* combActor = reinterpret_cast<ObjComb*>(actor);
        combActor->actor.shape.rot.x =
            Math_SinS(combActor->unk_1B2) * CLAMP_MIN(combActor->unk_1B0, 0) + combActor->actor.home.rot.x;
    }
}

// from z_player.c
typedef struct {
    /* 0x00 */ Vec3f pos;
    /* 0x0C */ s16 yaw;
} SpecialRespawnInfo; // size = 0x10

// special respawns used when voided out without swim to prevent infinite loops
std::map<s32, SpecialRespawnInfo> swimSpecialRespawnInfo = {
    { ENTR_ZORAS_RIVER_3, // hf to zr in water
      { { -1455.443, -20, 1384.826 }, 28761 } },
    { ENTR_HYRULE_FIELD_14, // zr to hf in water
      { { 5730.209, -20, 3725.911 }, -20025 } },
    { ENTR_LOST_WOODS_UNDERWATER_SHORTCUT, // zr to lw
      { { 1978.718, -36.908, -855 }, -16384 } },
    { ENTR_ZORAS_RIVER_UNDERWATER_SHORTCUT, // lw to zr
      { { 4082.366, 860.442, -1018.949 }, -32768 } },
    { ENTR_LAKE_HYLIA_RIVER_EXIT, // gv to lh
      { { -3276.416, -1033, 2908.421 }, 11228 } },
    { ENTR_WATER_TEMPLE_ENTRANCE, // lh to water temple
      { { -182, 780, 759.5 }, -32768 } },
    { ENTR_LAKE_HYLIA_OUTSIDE_TEMPLE, // water temple to lh
      { { -955.028, -1306.9, 6768.954 }, -32768 } },
    { ENTR_ZORAS_DOMAIN_UNDERWATER_SHORTCUT, // lh to zd
      { { -109.86, 11.396, -9.933 }, -29131 } },
    { ENTR_LAKE_HYLIA_UNDERWATER_SHORTCUT, // zd to lh
      { { -912, -1326.967, 3391 }, 0 } },
    { ENTR_GERUDO_VALLEY_1, // caught by gerudos as child
      { { -424, -2051, -74 }, 16384 } },
    { ENTR_HYRULE_FIELD_ON_BRIDGE_SPAWN, // mk to hf (can be a problem when it then turns night)
      { { 0, 0, 1100 }, 0 } },
    { ENTR_ZORAS_FOUNTAIN_JABU_JABU_BLUE_WARP, // jabu blue warp to zf
      { { -1580, 150, 1670 }, 8000 } },
};

f32 triforcePieceScale;

void RandomizerOnPlayerUpdateHandler() {
    if ((GET_PLAYER(gPlayState)->stateFlags1 & PLAYER_STATE1_IN_WATER) && !Flags_GetRandomizerInf(RAND_INF_CAN_SWIM) &&
        CUR_EQUIP_VALUE(EQUIP_TYPE_BOOTS) != EQUIP_VALUE_BOOTS_IRON) {
        // if you void out in water temple without swim you get instantly kicked out to prevent softlocks
        if (gPlayState->sceneNum == SCENE_WATER_TEMPLE) {
            GameInteractor::RawAction::TeleportPlayer(
                Entrance_OverrideNextIndex(ENTR_LAKE_HYLIA_OUTSIDE_TEMPLE)); // lake hylia from water temple
        } else {
            if (swimSpecialRespawnInfo.find(gSaveContext.entranceIndex) != swimSpecialRespawnInfo.end()) {
                SpecialRespawnInfo* respawnInfo = &swimSpecialRespawnInfo.at(gSaveContext.entranceIndex);

                Play_SetupRespawnPoint(gPlayState, RESPAWN_MODE_DOWN, 0xDFF);
                gSaveContext.respawn[RESPAWN_MODE_DOWN].pos = respawnInfo->pos;
                gSaveContext.respawn[RESPAWN_MODE_DOWN].yaw = respawnInfo->yaw;
            }

            Play_TriggerVoidOut(gPlayState);
        }
    }

    // Triforce Hunt needs the check if the player isn't being teleported to the credits scene.
    if (!GameInteractor::IsGameplayPaused() && Flags_GetRandomizerInf(RAND_INF_GRANT_GANONS_BOSSKEY) &&
        gPlayState->transitionTrigger != TRANS_TRIGGER_START &&
        (1 << 0 & gSaveContext.inventory.dungeonItems[SCENE_GANONS_TOWER]) == 0) {
        GiveItemEntryWithoutActor(gPlayState,
                                  *Rando::StaticData::GetItemTable().at(RG_GANONS_CASTLE_BOSS_KEY).GetGIEntry());
    }

    if (!GameInteractor::IsGameplayPaused() && RAND_GET_OPTION(RSK_TRIFORCE_HUNT)) {
        // Warp to credits
        if (GameInteractor::State::TriforceHuntCreditsWarpActive) {
            gPlayState->nextEntranceIndex = ENTR_CHAMBER_OF_THE_SAGES_0;
            gSaveContext.nextCutsceneIndex = 0xFFF2;
            gPlayState->transitionTrigger = TRANS_TRIGGER_START;
            gPlayState->transitionType = TRANS_TYPE_FADE_WHITE;
            GameInteractor::State::TriforceHuntCreditsWarpActive = 0;
        }

        // Reset Triforce Piece scale for GI animation. Triforce Hunt allows for multiple triforce models,
        // and cycles through them based on the amount of triforce pieces collected. It takes a little while
        // for the count to increase during the GI animation, so the model is entirely hidden until that piece
        // has been added. That scale has to be reset after the textbox is closed, and this is the best way
        // to ensure it's done at that point in time specifically.
        if (GameInteractor::State::TriforceHuntPieceGiven) {
            triforcePieceScale = 0.0f;
            GameInteractor::State::TriforceHuntPieceGiven = 0;
        }
    }
}

void RandomizerOnSceneSpawnActorsHandler() {
    if (LINK_IS_ADULT && RAND_GET_OPTION(RSK_SHEIK_LA_HINT)) {
        switch (gPlayState->sceneNum) {
            case SCENE_TEMPLE_OF_TIME:
                if (gPlayState->roomCtx.curRoom.num == 1) {
                    Actor_Spawn(&gPlayState->actorCtx, gPlayState, ACTOR_EN_XC, -104, -40, 2382, 0, 0x8000, 0,
                                SHEIK_TYPE_RANDO, false);
                }
                break;
            case SCENE_INSIDE_GANONS_CASTLE:
                if (gPlayState->roomCtx.curRoom.num == 1) {
                    Actor_Spawn(&gPlayState->actorCtx, gPlayState, ACTOR_EN_XC, 101, 150, 137, 0, 0, 0,
                                SHEIK_TYPE_RANDO, false);
                }
                break;
            default:
                break;
        }
    }
}

void RandomizerOnPlayDestroyHandler() {
    // In ER, remove link from epona when entering somewhere that doesn't support epona
    if (RAND_GET_OPTION(RSK_SHUFFLE_OVERWORLD_ENTRANCES)) {
        Entrance_HandleEponaState();
    }
}

void RandomizerOnExitGameHandler(int32_t fileNum) {
    // When going from a rando save to a vanilla save within the same game instance
    // we need to reset the entrance table back to its vanilla state
    Entrance_ResetEntranceTable();
}

void RandomizerOnKaleidoscopeUpdateHandler(int16_t inDungeonScene) {
    static uint16_t prevKaleidoState = 0;

    // In ER, handle overriding the game over respawn entrance and dealing with death warp to from grottos
    if (RAND_GET_OPTION(RSK_SHUFFLE_ENTRANCES)) {
        if (prevKaleidoState == 0x10 && gPlayState->pauseCtx.state == 0x11 && gPlayState->pauseCtx.promptChoice == 0) {
            // Needs to be called before Play_TriggerRespawn when transitioning from state 0x10 to 0x11
            Entrance_SetGameOverEntrance();
        }
        if (prevKaleidoState == 0x11 && gPlayState->pauseCtx.state == 0 && gPlayState->pauseCtx.promptChoice == 0) {
            // Needs to be called after Play_TriggerRespawn when transitioning from state 0x11 to 0
            Grotto_ForceGrottoReturn();
        }
    }

    prevKaleidoState = gPlayState->pauseCtx.state;
}

void RandomizerOnCuccoOrChickenHatch() {
    if (LINK_IS_CHILD) {
        Flags_UnsetRandomizerInf(RAND_INF_CHILD_TRADES_HAS_WEIRD_EGG);
        Flags_SetRandomizerInf(RAND_INF_CHILD_TRADES_HAS_CHICKEN);
    }
}

void RandomizerRegisterHooks() {
    static uint32_t onFlagSetHook = 0;
    static uint32_t onSceneFlagSetHook = 0;
    static uint32_t onPlayerUpdateForRCQueueHook = 0;
    static uint32_t onPlayerUpdateForItemQueueHook = 0;
    static uint32_t onItemReceiveHook = 0;
    static uint32_t onDialogMessageHook = 0;
    static uint32_t onVanillaBehaviorHook = 0;
    static uint32_t onSceneInitHook = 0;
    static uint32_t onActorInitHook = 0;
    static uint32_t onActorUpdateHook = 0;
    static uint32_t onPlayerUpdateHook = 0;
    static uint32_t onGameFrameUpdateHook = 0;
    static uint32_t onSceneSpawnActorsHook = 0;
    static uint32_t onPlayDestroyHook = 0;
    static uint32_t onExitGameHook = 0;
    static uint32_t onKaleidoUpdateHook = 0;
    static uint32_t onCuccoOrChickenHatchHook = 0;

    static uint32_t fishsanityOnActorInitHook = 0;
    static uint32_t fishsanityOnActorUpdateHook = 0;
    static uint32_t fishsanityOnSceneInitHook = 0;
    static uint32_t fishsanityOnVanillaBehaviorHook = 0;
    static uint32_t fishsanityOnItemReceiveHook = 0;

    static uint32_t shufflePotsOnActorInitHook = 0;
    static uint32_t shufflePotsOnVanillaBehaviorHook = 0;

    static uint32_t shuffleFreestandingOnVanillaBehaviorHook = 0;

    GameInteractor::Instance->RegisterGameHook<GameInteractor::OnLoadGame>([](int32_t fileNum) {
        ShipInit::Init("IS_RANDO");

        randomizerQueuedChecks = std::queue<RandomizerCheck>();
        randomizerQueuedCheck = RC_UNKNOWN_CHECK;
        randomizerQueuedItemEntry = GET_ITEM_NONE;

        GameInteractor::Instance->UnregisterGameHook<GameInteractor::OnFlagSet>(onFlagSetHook);
        GameInteractor::Instance->UnregisterGameHook<GameInteractor::OnSceneFlagSet>(onSceneFlagSetHook);
        GameInteractor::Instance->UnregisterGameHook<GameInteractor::OnPlayerUpdate>(onPlayerUpdateForRCQueueHook);
        GameInteractor::Instance->UnregisterGameHook<GameInteractor::OnPlayerUpdate>(onPlayerUpdateForItemQueueHook);
        GameInteractor::Instance->UnregisterGameHook<GameInteractor::OnItemReceive>(onItemReceiveHook);
        GameInteractor::Instance->UnregisterGameHook<GameInteractor::OnItemReceive>(onDialogMessageHook);
        GameInteractor::Instance->UnregisterGameHook<GameInteractor::OnVanillaBehavior>(onVanillaBehaviorHook);
        GameInteractor::Instance->UnregisterGameHook<GameInteractor::OnSceneInit>(onSceneInitHook);
        GameInteractor::Instance->UnregisterGameHook<GameInteractor::OnActorInit>(onActorInitHook);
        GameInteractor::Instance->UnregisterGameHook<GameInteractor::OnActorUpdate>(onActorUpdateHook);
        GameInteractor::Instance->UnregisterGameHook<GameInteractor::OnPlayerUpdate>(onPlayerUpdateHook);
        GameInteractor::Instance->UnregisterGameHook<GameInteractor::OnGameFrameUpdate>(onGameFrameUpdateHook);
        GameInteractor::Instance->UnregisterGameHook<GameInteractor::OnSceneSpawnActors>(onSceneSpawnActorsHook);
        GameInteractor::Instance->UnregisterGameHook<GameInteractor::OnPlayDestroy>(onPlayDestroyHook);
        GameInteractor::Instance->UnregisterGameHook<GameInteractor::OnExitGame>(onExitGameHook);
        GameInteractor::Instance->UnregisterGameHook<GameInteractor::OnKaleidoscopeUpdate>(onKaleidoUpdateHook);
        GameInteractor::Instance->UnregisterGameHook<GameInteractor::OnCuccoOrChickenHatch>(onCuccoOrChickenHatchHook);

        GameInteractor::Instance->UnregisterGameHook<GameInteractor::OnActorInit>(fishsanityOnActorInitHook);
        GameInteractor::Instance->UnregisterGameHook<GameInteractor::OnActorUpdate>(fishsanityOnActorUpdateHook);
        GameInteractor::Instance->UnregisterGameHook<GameInteractor::OnSceneInit>(fishsanityOnSceneInitHook);
        GameInteractor::Instance->UnregisterGameHook<GameInteractor::OnVanillaBehavior>(
            fishsanityOnVanillaBehaviorHook);
        GameInteractor::Instance->UnregisterGameHook<GameInteractor::OnItemReceive>(fishsanityOnItemReceiveHook);

        GameInteractor::Instance->UnregisterGameHookForID<GameInteractor::OnActorInit>(shufflePotsOnActorInitHook);
        GameInteractor::Instance->UnregisterGameHook<GameInteractor::OnVanillaBehavior>(
            shufflePotsOnVanillaBehaviorHook);

        GameInteractor::Instance->UnregisterGameHook<GameInteractor::OnVanillaBehavior>(
            shuffleFreestandingOnVanillaBehaviorHook);

        onFlagSetHook = 0;
        onSceneFlagSetHook = 0;
        onPlayerUpdateForRCQueueHook = 0;
        onPlayerUpdateForItemQueueHook = 0;
        onItemReceiveHook = 0;
        onDialogMessageHook = 0;
        onVanillaBehaviorHook = 0;
        onSceneInitHook = 0;
        onActorInitHook = 0;
        onActorUpdateHook = 0;
        onPlayerUpdateHook = 0;
        onGameFrameUpdateHook = 0;
        onSceneSpawnActorsHook = 0;
        onPlayDestroyHook = 0;
        onExitGameHook = 0;
        onKaleidoUpdateHook = 0;
        onCuccoOrChickenHatchHook = 0;

        fishsanityOnActorInitHook = 0;
        fishsanityOnActorUpdateHook = 0;
        fishsanityOnSceneInitHook = 0;
        fishsanityOnVanillaBehaviorHook = 0;
        fishsanityOnItemReceiveHook = 0;

        shufflePotsOnActorInitHook = 0;
        shufflePotsOnVanillaBehaviorHook = 0;

        shuffleFreestandingOnVanillaBehaviorHook = 0;

        ShuffleFairies_UnregisterHooks();

        if (!IS_RANDO)
            return;

        // ENTRTODO: Move all entrance rando handling to a dedicated file
        // Setup the modified entrance table and entrance shuffle table for rando
        Entrance_Init();

        // Handle randomized spawn positions after the save context has been setup from load
        if (RAND_GET_OPTION(RSK_SHUFFLE_ENTRANCES)) {
            Entrance_SetSavewarpEntrance();
        }

        onFlagSetHook =
            GameInteractor::Instance->RegisterGameHook<GameInteractor::OnFlagSet>(RandomizerOnFlagSetHandler);
        onSceneFlagSetHook =
            GameInteractor::Instance->RegisterGameHook<GameInteractor::OnSceneFlagSet>(RandomizerOnSceneFlagSetHandler);
        onPlayerUpdateForRCQueueHook = GameInteractor::Instance->RegisterGameHook<GameInteractor::OnPlayerUpdate>(
            RandomizerOnPlayerUpdateForRCQueueHandler);
        onPlayerUpdateForItemQueueHook = GameInteractor::Instance->RegisterGameHook<GameInteractor::OnPlayerUpdate>(
            RandomizerOnPlayerUpdateForItemQueueHandler);
        onItemReceiveHook =
            GameInteractor::Instance->RegisterGameHook<GameInteractor::OnItemReceive>(RandomizerOnItemReceiveHandler);
        onDialogMessageHook = GameInteractor::Instance->RegisterGameHook<GameInteractor::OnDialogMessage>(
            RandomizerOnDialogMessageHandler);
        onVanillaBehaviorHook = GameInteractor::Instance->RegisterGameHook<GameInteractor::OnVanillaBehavior>(
            RandomizerOnVanillaBehaviorHandler);
        onSceneInitHook =
            GameInteractor::Instance->RegisterGameHook<GameInteractor::OnSceneInit>(RandomizerOnSceneInitHandler);
        onActorInitHook =
            GameInteractor::Instance->RegisterGameHook<GameInteractor::OnActorInit>(RandomizerOnActorInitHandler);
        onActorUpdateHook =
            GameInteractor::Instance->RegisterGameHook<GameInteractor::OnActorUpdate>(RandomizerOnActorUpdateHandler);
        onPlayerUpdateHook =
            GameInteractor::Instance->RegisterGameHook<GameInteractor::OnPlayerUpdate>(RandomizerOnPlayerUpdateHandler);
        onGameFrameUpdateHook = GameInteractor::Instance->RegisterGameHook<GameInteractor::OnGameFrameUpdate>(
            RandomizerOnGameFrameUpdateHandler);
        onSceneSpawnActorsHook = GameInteractor::Instance->RegisterGameHook<GameInteractor::OnSceneSpawnActors>(
            RandomizerOnSceneSpawnActorsHandler);
        onPlayDestroyHook =
            GameInteractor::Instance->RegisterGameHook<GameInteractor::OnPlayDestroy>(RandomizerOnPlayDestroyHandler);
        onExitGameHook =
            GameInteractor::Instance->RegisterGameHook<GameInteractor::OnExitGame>(RandomizerOnExitGameHandler);
        onKaleidoUpdateHook = GameInteractor::Instance->RegisterGameHook<GameInteractor::OnKaleidoscopeUpdate>(
            RandomizerOnKaleidoscopeUpdateHandler);
        onCuccoOrChickenHatchHook = GameInteractor::Instance->RegisterGameHook<GameInteractor::OnCuccoOrChickenHatch>(
            RandomizerOnCuccoOrChickenHatch);

        if (RAND_GET_OPTION(RSK_FISHSANITY) != RO_FISHSANITY_OFF) {
            OTRGlobals::Instance->gRandoContext->GetFishsanity()->InitializeFromSave();

            fishsanityOnActorInitHook = GameInteractor::Instance->RegisterGameHook<GameInteractor::OnActorInit>(
                Rando::Fishsanity::OnActorInitHandler);
            fishsanityOnActorUpdateHook = GameInteractor::Instance->RegisterGameHook<GameInteractor::OnActorUpdate>(
                Rando::Fishsanity::OnActorUpdateHandler);
            fishsanityOnSceneInitHook = GameInteractor::Instance->RegisterGameHook<GameInteractor::OnSceneInit>(
                Rando::Fishsanity::OnSceneInitHandler);
            fishsanityOnVanillaBehaviorHook =
                GameInteractor::Instance->RegisterGameHook<GameInteractor::OnVanillaBehavior>(
                    Rando::Fishsanity::OnVanillaBehaviorHandler);
            fishsanityOnItemReceiveHook = GameInteractor::Instance->RegisterGameHook<GameInteractor::OnItemReceive>(
                Rando::Fishsanity::OnItemReceiveHandler);
        }

        if (RAND_GET_OPTION(RSK_SHUFFLE_POTS) != RO_SHUFFLE_POTS_OFF) {
            shufflePotsOnActorInitHook = GameInteractor::Instance->RegisterGameHookForID<GameInteractor::OnActorInit>(
                ACTOR_OBJ_TSUBO, ObjTsubo_RandomizerInit);
            shufflePotsOnVanillaBehaviorHook =
                GameInteractor::Instance->RegisterGameHook<GameInteractor::OnVanillaBehavior>(
                    ShufflePots_OnVanillaBehaviorHandler);
        }

        if (RAND_GET_OPTION(RSK_SHUFFLE_FREESTANDING) != RO_SHUFFLE_FREESTANDING_OFF) {
            shuffleFreestandingOnVanillaBehaviorHook =
                GameInteractor::Instance->RegisterGameHook<GameInteractor::OnVanillaBehavior>(
                    ShuffleFreestanding_OnVanillaBehaviorHandler);
        }

        if (RAND_GET_OPTION(RSK_SHUFFLE_FAIRIES)) {
            ShuffleFairies_RegisterHooks();
        }
    });
}
