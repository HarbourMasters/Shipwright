#include <libultraship/bridge.h>
#include "soh/OTRGlobals.h"
#include "soh/Enhancements/enhancementTypes.h"
#include "soh/Enhancements/custom-message/CustomMessageTypes.h"
#include "soh/Enhancements/item-tables/ItemTableManager.h"
#include "soh/Enhancements/randomizer/randomizerTypes.h"
#include "soh/Enhancements/randomizer/dungeon.h"
#include "soh/Enhancements/randomizer/fishsanity.h"
#include "soh/Enhancements/game-interactor/GameInteractor.h"
#include "soh/Enhancements/game-interactor/GameInteractor_Hooks.h"
#include "soh/ImGuiUtils.h"
#include "soh/Notification/Notification.h"

extern "C" {
#include "macros.h"
#include "functions.h"
#include "variables.h"
#include "soh/Enhancements/randomizer/adult_trade_shuffle.h"
#include "src/overlays/actors/ovl_Bg_Treemouth/z_bg_treemouth.h"
#include "src/overlays/actors/ovl_En_Si/z_en_si.h"
#include "src/overlays/actors/ovl_En_Cow/z_en_cow.h"
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
#include "adult_trade_shuffle.h"
#include "draw.h"

extern SaveContext gSaveContext;
extern PlayState* gPlayState;
extern void func_8084DFAC(PlayState* play, Player* player);
extern void Player_SetupActionPreserveAnimMovement(PlayState* play, Player* player, PlayerActionFunc actionFunc, s32 flags);
extern s32 Player_SetupWaitForPutAway(PlayState* play, Player* player, AfterPutAwayFunc func);
}

#define RAND_GET_OPTION(option) Rando::Context::GetInstance()->GetOption(option).GetSelectedOptionIndex()

bool LocMatchesQuest(Rando::Location loc) {
    if (loc.GetQuest() == RCQUEST_BOTH) {
        return true;
    } else {
        auto dungeon = OTRGlobals::Instance->gRandoContext->GetDungeons()->GetDungeonFromScene(loc.GetScene());
        return (dungeon->IsMQ() && loc.GetQuest() == RCQUEST_MQ) || (dungeon->IsVanilla() && loc.GetQuest() == RCQUEST_VANILLA);
    }
}

RandomizerCheck GetRandomizerCheckFromFlag(int16_t flagType, int16_t flag) {
    for (auto& loc : Rando::StaticData::GetLocationTable()) {
        if ((loc.GetCollectionCheck().flag == flag && (
                (flagType == FLAG_INF_TABLE && loc.GetCollectionCheck().type == SPOILER_CHK_INF_TABLE) ||
                (flagType == FLAG_EVENT_CHECK_INF && loc.GetCollectionCheck().type == SPOILER_CHK_EVENT_CHK_INF) ||
                (flagType == FLAG_ITEM_GET_INF && loc.GetCollectionCheck().type == SPOILER_CHK_ITEM_GET_INF) ||
                (flagType == FLAG_RANDOMIZER_INF && loc.GetCollectionCheck().type == SPOILER_CHK_RANDOMIZER_INF)
            ) ||
            (loc.GetActorParams() == flag && flagType == FLAG_GS_TOKEN && loc.GetCollectionCheck().type == SPOILER_CHK_GOLD_SKULLTULA)
        ) && LocMatchesQuest(loc)) {
            return loc.GetRandomizerCheck();
        }
    }

    return RC_UNKNOWN_CHECK;
}

RandomizerCheck GetRandomizerCheckFromSceneFlag(int16_t sceneNum, int16_t flagType, int16_t flag) {
    for (auto& loc : Rando::StaticData::GetLocationTable()) {
        if (loc.GetCollectionCheck().scene == sceneNum && loc.GetCollectionCheck().flag == flag && (
            (flagType == FLAG_SCENE_TREASURE && loc.GetCollectionCheck().type == SPOILER_CHK_CHEST) ||
            (flagType == FLAG_SCENE_COLLECTIBLE && loc.GetCollectionCheck().type == SPOILER_CHK_COLLECTABLE) ||
            (flagType == FLAG_GS_TOKEN && loc.GetCollectionCheck().type == SPOILER_CHK_GOLD_SKULLTULA)
        ) && LocMatchesQuest(loc)) {
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
            if ((CheckMedallionCount() + CheckStoneCount() + CheckLACSRewardCount()) >= RAND_GET_OPTION(RSK_LACS_REWARD_COUNT)) {
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
            if ((CheckMedallionCount() + CheckBridgeRewardCount()) >= RAND_GET_OPTION(RSK_RAINBOW_BRIDGE_MEDALLION_COUNT)) {
                return true;
            }
            break;
        }
        case RO_BRIDGE_DUNGEON_REWARDS: {
            if ((CheckMedallionCount() + CheckStoneCount() + CheckBridgeRewardCount()) >= RAND_GET_OPTION(RSK_RAINBOW_BRIDGE_REWARD_COUNT)) {
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
                Randomizer_ConsumeAdultTradeItem(gPlayState, ITEM_SWORD_BROKEN);
                break;
            case RAND_INF_ADULT_TRADES_DMT_TRADE_EYEDROPS:
                Randomizer_ConsumeAdultTradeItem(gPlayState, ITEM_EYEDROPS);
                break;
        }
    }

    RandomizerCheck rc = GetRandomizerCheckFromFlag(flagType, flag);
    if (rc == RC_UNKNOWN_CHECK) return;

    auto loc = Rando::Context::GetInstance()->GetItemLocation(rc);
    if (loc == nullptr || loc->HasObtained()) return;

    SPDLOG_INFO("Queuing RC: {}", static_cast<uint32_t>(rc));
    randomizerQueuedChecks.push(rc);
}

void RandomizerOnSceneFlagSetHandler(int16_t sceneNum, int16_t flagType, int16_t flag) {
    if (RAND_GET_OPTION(RSK_SHUFFLE_DUNGEON_ENTRANCES) != RO_DUNGEON_ENTRANCE_SHUFFLE_OFF &&
        sceneNum == SCENE_GERUDOS_FORTRESS &&
        flagType == FLAG_SCENE_SWITCH &&
        flag == 0x3A) {
        Flags_SetRandomizerInf(RAND_INF_GF_GTG_GATE_PERMANENTLY_OPEN);
    }

    RandomizerCheck rc = GetRandomizerCheckFromSceneFlag(sceneNum, flagType, flag);
    if (rc == RC_UNKNOWN_CHECK) return;

    auto loc = Rando::Context::GetInstance()->GetItemLocation(rc);
    if (loc == nullptr || loc->HasObtained()) return;

    SPDLOG_INFO("Queuing RC: {}", static_cast<uint32_t>(rc));
    randomizerQueuedChecks.push(rc);
}

static Vec3f spawnPos = { 0.0f, -999.0f, 0.0f };

void RandomizerOnPlayerUpdateForRCQueueHandler() {
    // If we're already queued, don't queue again
    if (randomizerQueuedCheck != RC_UNKNOWN_CHECK) return;

    // If there's nothing to queue, don't queue
    if (randomizerQueuedChecks.size() < 1) return;

    // If we're in a cutscene, don't queue
    Player* player = GET_PLAYER(gPlayState);
    if (Player_InBlockingCsMode(gPlayState, player) || player->stateFlags1 & PLAYER_STATE1_IN_ITEM_CS || player->stateFlags1 & PLAYER_STATE1_GETTING_ITEM || player->stateFlags1 & PLAYER_STATE1_CARRYING_ACTOR) {
        return;
    }

    RandomizerCheck rc = randomizerQueuedChecks.front();
    auto loc = Rando::Context::GetInstance()->GetItemLocation(rc);
    GetItemEntry getItemEntry = Rando::Context::GetInstance()->GetFinalGIEntry(rc, true, (GetItemID)Rando::StaticData::GetLocation(rc)->GetVanillaItem());

    if (loc->HasObtained()) {
        SPDLOG_INFO("RC {} already obtained, skipping", static_cast<uint32_t>(rc));
    } else {
        randomizerQueuedCheck = rc;
        randomizerQueuedItemEntry = getItemEntry;
        SPDLOG_INFO("Queueing Item mod {} item {} from RC {}", getItemEntry.modIndex, getItemEntry.itemId, static_cast<uint32_t>(rc));
        if (
            // Skipping ItemGet animation incompatible with checks that require closing a text box to finish
            rc != RC_HF_OCARINA_OF_TIME_ITEM &&
            rc != RC_SPIRIT_TEMPLE_SILVER_GAUNTLETS_CHEST &&
            rc != RC_MARKET_BOMBCHU_BOWLING_FIRST_PRIZE &&
            rc != RC_MARKET_BOMBCHU_BOWLING_SECOND_PRIZE &&
            // Always show ItemGet animation for ice traps
            !(getItemEntry.modIndex == MOD_RANDOMIZER && getItemEntry.getItemId == RG_ICE_TRAP) &&
            (
                CVarGetInteger(CVAR_ENHANCEMENT("TimeSavers.SkipGetItemAnimation"), SGIA_DISABLED) == SGIA_ALL ||
                (
                    CVarGetInteger(CVAR_ENHANCEMENT("TimeSavers.SkipGetItemAnimation"), SGIA_DISABLED) == SGIA_JUNK &&
                    (
                        getItemEntry.getItemCategory == ITEM_CATEGORY_JUNK ||
                        getItemEntry.getItemCategory == ITEM_CATEGORY_SKULLTULA_TOKEN ||
                        getItemEntry.getItemCategory == ITEM_CATEGORY_LESSER
                    )
                )
            )
        ) {
            Item_DropCollectible(gPlayState, &spawnPos, ITEM00_SOH_GIVE_ITEM_ENTRY | 0x8000);
        }
    }

    randomizerQueuedChecks.pop();
}

void RandomizerOnPlayerUpdateForItemQueueHandler() {
    if (randomizerQueuedCheck == RC_UNKNOWN_CHECK) return;

    Player* player = GET_PLAYER(gPlayState);
    if (player == NULL || Player_InBlockingCsMode(gPlayState, player) || player->stateFlags1 & PLAYER_STATE1_IN_ITEM_CS || player->stateFlags1 & PLAYER_STATE1_GETTING_ITEM || player->stateFlags1 & PLAYER_STATE1_CARRYING_ACTOR) {
        return;
    }

    SPDLOG_INFO("Attempting to give Item mod {} item {} from RC {}", randomizerQueuedItemEntry.modIndex, randomizerQueuedItemEntry.itemId, static_cast<uint32_t>(randomizerQueuedCheck));
    GiveItemEntryWithoutActor(gPlayState, randomizerQueuedItemEntry);
    if (player->stateFlags1 & PLAYER_STATE1_IN_WATER) {
        // Allow the player to receive the item while swimming
        player->stateFlags2 |= PLAYER_STATE2_UNDERWATER;
        Player_ActionHandler_2(player, gPlayState);
    }
}

void RandomizerOnItemReceiveHandler(GetItemEntry receivedItemEntry) {
    if (randomizerQueuedCheck == RC_UNKNOWN_CHECK) return;

    auto loc = Rando::Context::GetInstance()->GetItemLocation(randomizerQueuedCheck);
    if (randomizerQueuedItemEntry.modIndex == receivedItemEntry.modIndex && randomizerQueuedItemEntry.itemId == receivedItemEntry.itemId) {
        SPDLOG_INFO("Item received mod {} item {} from RC {}", receivedItemEntry.modIndex, receivedItemEntry.itemId, static_cast<uint32_t>(randomizerQueuedCheck));
        loc->SetCheckStatus(RCSHOW_COLLECTED);
        CheckTracker::RecalculateAllAreaTotals();
        SaveManager::Instance->SaveSection(gSaveContext.fileNum, SECTION_ID_TRACKER_DATA, true);
        randomizerQueuedCheck = RC_UNKNOWN_CHECK;
        randomizerQueuedItemEntry = GET_ITEM_NONE;
    }

    if (
        receivedItemEntry.modIndex == MOD_NONE && (
            receivedItemEntry.itemId == ITEM_HEART_PIECE ||
            receivedItemEntry.itemId == ITEM_HEART_PIECE_2 ||
            receivedItemEntry.itemId == ITEM_HEART_CONTAINER
        )
    ) {
        gSaveContext.healthAccumulator = 0x140; // Refill 20 hearts
        if ((s32)(gSaveContext.inventory.questItems & 0xF0000000) == 0x40000000) {
            gSaveContext.inventory.questItems ^= 0x40000000;
            gSaveContext.healthCapacity += 0x10;
            gSaveContext.health += 0x10;
        }
    }

    if (loc->GetRandomizerCheck() == RC_SPIRIT_TEMPLE_SILVER_GAUNTLETS_CHEST && !CVarGetInteger(CVAR_ENHANCEMENT("TimeSavers.SkipCutscene.Story"), IS_RANDO)) {
        static uint32_t updateHook;
        updateHook = GameInteractor::Instance->RegisterGameHook<GameInteractor::OnPlayerUpdate>([]() {
            Player* player = GET_PLAYER(gPlayState);
            if (player == NULL || Player_InBlockingCsMode(gPlayState, player) || player->stateFlags1 & PLAYER_STATE1_IN_ITEM_CS || player->stateFlags1 & PLAYER_STATE1_GETTING_ITEM || player->stateFlags1 & PLAYER_STATE1_CARRYING_ACTOR) {
                return;
            }

            gPlayState->nextEntranceIndex = ENTR_DESERT_COLOSSUS_0;
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
    f32 mtxScale = CVarGetFloat(CVAR_ENHANCEMENT("TimeSavers.SkipGetItemAnimationScale"), 10.0f);
    Matrix_Scale(mtxScale, mtxScale, mtxScale, MTXMODE_APPLY);
    GetItemEntry randoItem = enItem00->itemEntry;
    if (CVarGetInteger(CVAR_RANDOMIZER_ENHANCEMENT("MysteriousShuffle"), 0)) {
        randoItem = GET_ITEM_MYSTERY;
    }
    EnItem00_CustomItemsParticles(&enItem00->actor, play, randoItem);
    GetItemEntry_Draw(play, randoItem);
}

void ItemBHeart_DrawRandomizedItem(ItemBHeart* itemBHeart, PlayState* play) {
    GetItemEntry randoItem = itemBHeart->sohItemEntry;
    if (CVarGetInteger(CVAR_RANDOMIZER_ENHANCEMENT("MysteriousShuffle"), 0)) {
        randoItem = GET_ITEM_MYSTERY;
    }
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
    if (itemEtcetera->actor.xzDistToPlayer < 30.0f &&
        fabsf(itemEtcetera->actor.yDistToPlayer) < 50.0f) {
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

    if (itemEtcetera->actor.xzDistToPlayer < 30.0f &&
        fabsf(itemEtcetera->actor.yDistToPlayer) < 50.0f) {

        Flags_SetTreasure(play, 0x1F);
        Actor_Kill(&itemEtcetera->actor);
    }
}

void ItemEtcetera_MoveRandomizedFireArrowDown(ItemEtcetera* itemEtcetera, PlayState* play) {
    Actor_UpdateBgCheckInfo(play, &itemEtcetera->actor, 10.0f, 10.0f, 0.0f, 5);
    Actor_MoveForward(&itemEtcetera->actor);
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

void EnCow_MoveForRandomizer(EnCow* enCow, PlayState* play) {
    bool moved = false;

    // Don't reposition the tail
    if (enCow->actor.params != 0) {
        return;
    }

    // Move left cow in lon lon tower
    if (play->sceneNum == SCENE_LON_LON_BUILDINGS && enCow->actor.world.pos.x == -108 &&
        enCow->actor.world.pos.z == -65) {
        enCow->actor.world.pos.x = -229.0f;
        enCow->actor.world.pos.z = 157.0f;
        enCow->actor.shape.rot.y = 15783.0f;
        moved = true;
        // Move right cow in lon lon stable
    } else if (play->sceneNum == SCENE_STABLE && enCow->actor.world.pos.x == -3 && enCow->actor.world.pos.z == -254) {
        enCow->actor.world.pos.x += 119.0f;
        moved = true;
    }

    if (moved) {
        // Reposition collider
        func_809DEE9C(enCow);
    }
}

u8 EnDs_RandoCanGetGrannyItem() {
    return (RAND_GET_OPTION(RSK_SHUFFLE_MERCHANTS) == RO_SHUFFLE_MERCHANTS_ALL_BUT_BEANS ||
            RAND_GET_OPTION(RSK_SHUFFLE_MERCHANTS) == RO_SHUFFLE_MERCHANTS_ALL) &&
           !Flags_GetRandomizerInf(RAND_INF_MERCHANTS_GRANNYS_SHOP) &&
           // Traded odd mushroom when adult trade is on
           ((RAND_GET_OPTION(RSK_SHUFFLE_ADULT_TRADE) && Flags_GetItemGetInf(ITEMGETINF_30)) ||
            // Found claim check when adult trade is off
            (!RAND_GET_OPTION(RSK_SHUFFLE_ADULT_TRADE) &&
             INV_CONTENT(ITEM_CLAIM_CHECK) == ITEM_CLAIM_CHECK));
}

u8 EnJs_RandoCanGetCarpetMerchantItem() {
    return  (RAND_GET_OPTION(RSK_SHUFFLE_MERCHANTS) == RO_SHUFFLE_MERCHANTS_ALL ||
             RAND_GET_OPTION(RSK_SHUFFLE_MERCHANTS) == RO_SHUFFLE_MERCHANTS_ALL_BUT_BEANS) &&
            // If the rando check has already been awarded, use vanilla behavior.
            !Flags_GetRandomizerInf(RAND_INF_MERCHANTS_CARPET_SALESMAN);
}

u8 EnGm_RandoCanGetMedigoronItem() {
    return  (RAND_GET_OPTION(RSK_SHUFFLE_MERCHANTS) == RO_SHUFFLE_MERCHANTS_ALL ||
             RAND_GET_OPTION(RSK_SHUFFLE_MERCHANTS) == RO_SHUFFLE_MERCHANTS_ALL_BUT_BEANS) &&
            // If the rando check has already been awarded, use vanilla behavior.
            !Flags_GetRandomizerInf(RAND_INF_MERCHANTS_MEDIGORON);
}

RandomizerCheck EnFr_RandomizerCheckFromSongIndex(u16 songIndex) {
    switch (songIndex) {
        case FROG_ZL:
            return RC_ZR_FROGS_ZELDAS_LULLABY;
        case FROG_EPONA:
            return RC_ZR_FROGS_EPONAS_SONG;
        case FROG_SARIA:
            return RC_ZR_FROGS_SARIAS_SONG;
        case FROG_SUNS:
            return RC_ZR_FROGS_SUNS_SONG;
        case FROG_SOT:
            return RC_ZR_FROGS_SONG_OF_TIME;
        case FROG_STORMS:
            return RC_ZR_FROGS_IN_THE_RAIN;
        case FROG_CHOIR_SONG:
            return RC_ZR_FROGS_OCARINA_GAME;
        default:
            return RC_UNKNOWN_CHECK;
    }
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

bool ShouldGiveFishingPrize(f32 sFishOnHandLength){
    // RANDOTODO: update the enhancement sliders to not allow
    // values above rando fish weight values when rando'd
    if(LINK_IS_CHILD) { 
        int32_t weight = CVarGetInteger(CVAR_ENHANCEMENT("CustomizeFishing"), 0) ? CVarGetInteger(CVAR_ENHANCEMENT("MinimumFishWeightChild"), 10) : 10;
        f32 score = sqrt(((f32)weight - 0.5f) / 0.0036f);
        return sFishOnHandLength >= score && (IS_RANDO ? !Flags_GetRandomizerInf(RAND_INF_CHILD_FISHING) : !(HIGH_SCORE(HS_FISHING) & HS_FISH_PRIZE_CHILD));
    } else 
    {
        int32_t weight = CVarGetInteger(CVAR_ENHANCEMENT("CustomizeFishing"), 0) ? CVarGetInteger(CVAR_ENHANCEMENT("MinimumFishWeightAdult"), 13) : 13;
        f32 score = sqrt(((f32)weight - 0.5f) / 0.0036f);
        return sFishOnHandLength >= score && (IS_RANDO ? !Flags_GetRandomizerInf(RAND_INF_ADULT_FISHING) : !(HIGH_SCORE(HS_FISHING) & HS_FISH_PRIZE_ADULT));
    }
}

void RandomizerOnVanillaBehaviorHandler(GIVanillaBehavior id, bool* should, va_list originalArgs) {
    va_list args;
    va_copy(args, originalArgs);

    switch (id) {
        case VB_PLAY_SLOW_CHEST_CS: {
            // We force fast chests if SkipGetItemAnimation is enabled because the camera in the CS looks pretty wonky otherwise
            if (CVarGetInteger(CVAR_ENHANCEMENT("TimeSavers.SkipGetItemAnimation"), SGIA_DISABLED)) {
                *should = false;
            }
            break;
        }
        case VB_GIVE_ITEM_FROM_CHEST: {
            EnBox* chest = va_arg(args, EnBox*);
            RandomizerCheck rc = OTRGlobals::Instance->gRandomizer->GetCheckFromActor(chest->dyna.actor.id, gPlayState->sceneNum, chest->dyna.actor.params);
            
            // if this is a treasure chest game chest then set the appropriate rando inf
            RandomizerSetChestGameRandomizerInf(rc);

            Player* player = GET_PLAYER(gPlayState);
            Player_SetupWaitForPutAway(gPlayState, player, func_8083A434_override);

            *should = false;
            break;
        }
        case VB_PLAY_NABOORU_CAPTURED_CS:
            // This behavior is replicated for randomizer in RandomizerOnItemReceiveHandler
            *should = false;
            break;
        case VB_SHIEK_PREPARE_TO_GIVE_SERENADE_OF_WATER: {
            *should = !Flags_GetEventChkInf(EVENTCHKINF_LEARNED_SERENADE_OF_WATER) && !Flags_GetTreasure(gPlayState, 0x2);
            break;
        }
        case VB_BE_ELIGIBLE_FOR_SERENADE_OF_WATER:
            *should = !Flags_GetEventChkInf(EVENTCHKINF_LEARNED_SERENADE_OF_WATER) && Flags_GetTreasure(gPlayState, 0x2);
            break;
        case VB_BE_ELIGIBLE_FOR_PRELUDE_OF_LIGHT:
            *should = !Flags_GetEventChkInf(EVENTCHKINF_LEARNED_PRELUDE_OF_LIGHT) && CHECK_QUEST_ITEM(QUEST_MEDALLION_FOREST);
            break;
        case VB_MIDO_SPAWN:
            if (RAND_GET_OPTION(RSK_FOREST) != RO_FOREST_OPEN && !Flags_GetEventChkInf(EVENTCHKINF_SHOWED_MIDO_SWORD_SHIELD)) {
                *should = true;
            }
            break;
        case VB_MOVE_MIDO_IN_KOKIRI_FOREST:
            if (RAND_GET_OPTION(RSK_FOREST) == RO_FOREST_OPEN) {
                *should = true;
            }
            break;
        case VB_MIDO_CONSIDER_DEKU_TREE_DEAD:
            *should = Flags_GetEventChkInf(EVENTCHKINF_OBTAINED_KOKIRI_EMERALD_DEKU_TREE_DEAD);
            break;
        case VB_OPEN_KOKIRI_FOREST:
            *should = Flags_GetEventChkInf(EVENTCHKINF_OBTAINED_KOKIRI_EMERALD_DEKU_TREE_DEAD) || RAND_GET_OPTION(RSK_FOREST) != RO_FOREST_CLOSED;
            break;
        case VB_BE_ELIGIBLE_FOR_DARUNIAS_JOY_REWARD:
            *should = !Flags_GetRandomizerInf(RAND_INF_DARUNIAS_JOY);
            break;
        case VB_BE_ELIGIBLE_FOR_LIGHT_ARROWS:
            *should =
                LINK_IS_ADULT &&
                (gEntranceTable[gSaveContext.entranceIndex].scene == SCENE_TEMPLE_OF_TIME) &&
                !Flags_GetEventChkInf(EVENTCHKINF_RETURNED_TO_TEMPLE_OF_TIME_WITH_ALL_MEDALLIONS) &&
                MeetsLACSRequirements();
            break;
        case VB_BE_ELIGIBLE_FOR_NOCTURNE_OF_SHADOW:
            *should =
                    !Flags_GetEventChkInf(EVENTCHKINF_BONGO_BONGO_ESCAPED_FROM_WELL) &&
                    LINK_IS_ADULT &&
                    gEntranceTable[((void)0, gSaveContext.entranceIndex)].scene == SCENE_KAKARIKO_VILLAGE &&
                    CHECK_QUEST_ITEM(QUEST_MEDALLION_FOREST) &&
                    CHECK_QUEST_ITEM(QUEST_MEDALLION_FIRE) &&
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
                *should = gSaveContext.rupees >= OTRGlobals::Instance->gRandoContext->GetItemLocation(RC_ZR_MAGIC_BEAN_SALESMAN)->GetPrice();
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
                RandomizerCheck rc = OTRGlobals::Instance->gRandomizer->GetCheckFromActor(item00->actor.id, gPlayState->sceneNum, item00->ogParams);
                if (rc != RC_UNKNOWN_CHECK) {
                    item00->actor.params = ITEM00_SOH_DUMMY;
                    item00->itemEntry = Rando::Context::GetInstance()->GetFinalGIEntry(rc, true, (GetItemID)Rando::StaticData::GetLocation(rc)->GetVanillaItem());
                    item00->actor.draw = (ActorFunc)EnItem00_DrawRandomizedItem;
                    *should = Rando::Context::GetInstance()->GetItemLocation(rc)->HasObtained();
                }
            } else if (item00->actor.params == ITEM00_SOH_GIVE_ITEM_ENTRY || item00->actor.params == ITEM00_SOH_GIVE_ITEM_ENTRY_GI) {
                GetItemEntry itemEntry = randomizerQueuedItemEntry;
                item00->itemEntry = itemEntry;
                item00->actor.draw = (ActorFunc)EnItem00_DrawRandomizedItem;
            }
            break;
        }
        case VB_ITEM_B_HEART_DESPAWN: {
            ItemBHeart* itemBHeart = va_arg(args, ItemBHeart*);
            RandomizerCheck rc = OTRGlobals::Instance->gRandomizer->GetCheckFromActor(itemBHeart->actor.id, gPlayState->sceneNum, itemBHeart->actor.params);
            if (rc != RC_UNKNOWN_CHECK) {
                itemBHeart->sohItemEntry = Rando::Context::GetInstance()->GetFinalGIEntry(rc, true, (GetItemID)Rando::StaticData::GetLocation(rc)->GetVanillaItem());
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
        case VB_BIGGORON_CONSIDER_SWORD_COLLECTED: {
            *should = Flags_GetRandomizerInf(RAND_INF_ADULT_TRADES_DMT_TRADE_CLAIM_CHECK);
            break;
        }
        case VB_BIGGORON_CONSIDER_TRADE_COMPLETE: {
            // This being true will prevent other biggoron trades, there are already safegaurds in place to prevent
            // claim check from being traded multiple times, so we don't really need the quest to ever be considered "complete"
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
                Audio_PlaySoundGeneral(NA_SE_SY_GET_ITEM, &D_801333D4, 4, &D_801333E0, &D_801333E0, &D_801333E8);
                if (item00->itemEntry.modIndex == MOD_NONE) {
                    if (item00->itemEntry.getItemId == GI_SWORD_BGS) {
                        gSaveContext.bgsFlag = true;
                    }
                    Item_Give(gPlayState, item00->itemEntry.itemId);
                } else if (item00->itemEntry.modIndex == MOD_RANDOMIZER) {
                    if (item00->itemEntry.getItemId == RG_ICE_TRAP) {
                        gSaveContext.pendingIceTrapCount++;
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
                        .suffix = Rando::StaticData::RetrieveItem((RandomizerGet)item00->itemEntry.getItemId).GetName().english,
                    });
                }

                // This is typically called when you close the text box after getting an item, in case a previous
                // function hid the interface.
                Interface_ChangeAlpha(gSaveContext.unk_13EE);
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
        case VB_GIVE_ITEM_FROM_COW: {
            if (!RAND_GET_OPTION(RSK_SHUFFLE_COWS)) {
                break;
            }
            EnCow* enCow = va_arg(args, EnCow*);
            CowIdentity cowIdentity = OTRGlobals::Instance->gRandomizer->IdentifyCow(gPlayState->sceneNum, enCow->actor.world.pos.x, enCow->actor.world.pos.z);
            // Has this cow already rewarded an item?
            if (Flags_GetRandomizerInf(cowIdentity.randomizerInf)) {
                break;
            }
            Flags_SetRandomizerInf(cowIdentity.randomizerInf);
            // setting the ocarina mode here prevents intermittent issues
            // with the item get not triggering until walking away
            gPlayState->msgCtx.ocarinaMode = OCARINA_MODE_00;
            *should = false;
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
        case VB_GIVE_ITEM_FROM_THAWING_KING_ZORA: {
            EnKz* enKz = va_arg(args, EnKz*);
            // If we aren't setting up the item offer, then we're just checking if it should be possible.
            if (enKz->actionFunc != (EnKzActionFunc)EnKz_SetupGetItem) {
                // Always give the reward in rando
                *should = true;
                break;
            }
            *should = false;
            break;
        }
        case VB_GIVE_ITEM_FROM_ANJU_AS_CHILD: {
            Flags_SetItemGetInf(ITEMGETINF_0C);
            *should = false;
            break;
        }
        case VB_GIVE_ITEM_FROM_ANJU_AS_ADULT: {
            Flags_SetItemGetInf(ITEMGETINF_2C);
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
            if (EnJs_RandoCanGetCarpetMerchantItem()){
                *should = gSaveContext.rupees < OTRGlobals::Instance->gRandoContext->GetItemLocation(RC_WASTELAND_BOMBCHU_SALESMAN)->GetPrice();
            }
            break;
        }
        case VB_GIVE_ITEM_FROM_CARPET_SALESMAN: {
            EnJs* enJs = va_arg(args, EnJs*);
            if (EnJs_RandoCanGetCarpetMerchantItem()){
                Rupees_ChangeBy(OTRGlobals::Instance->gRandoContext->GetItemLocation(RC_WASTELAND_BOMBCHU_SALESMAN)->GetPrice() * -1);
                enJs->actor.parent = NULL;
                enJs->actor.textId = TEXT_CARPET_SALESMAN_ARMS_DEALER;
                enJs->actionFunc = (EnJsActionFunc)func_80A890C0;
                enJs->actor.flags |= ACTOR_FLAG_WILL_TALK;
                Flags_SetRandomizerInf(RAND_INF_MERCHANTS_CARPET_SALESMAN);
                *should = true;
            }
            break;
        }
        case VB_GIVE_BOMBCHUS_FROM_CARPET_SALESMAN: {
            *should = RAND_GET_OPTION(RSK_BOMBCHUS_IN_LOGIC) == false || INV_CONTENT(ITEM_BOMBCHU) == ITEM_BOMBCHU;
            break;
        }
        case VB_CHECK_RANDO_PRICE_OF_MEDIGORON: {
            if (EnGm_RandoCanGetMedigoronItem()){
                *should = gSaveContext.rupees < OTRGlobals::Instance->gRandoContext->GetItemLocation(RC_GC_MEDIGORON)->GetPrice();
            }
            break;
        }

        case VB_GIVE_ITEM_FROM_MEDIGORON: {
            // fallthrough
        case VB_BE_ELIGIBLE_FOR_GIANTS_KNIFE_PURCHASE:
            if (EnGm_RandoCanGetMedigoronItem()) {
                if (id == VB_GIVE_ITEM_FROM_MEDIGORON) {
                    EnGm* enGm = va_arg(args, EnGm*);
                    Flags_SetInfTable(INFTABLE_B1);
                    Flags_SetRandomizerInf(RAND_INF_MERCHANTS_MEDIGORON);
                    enGm->actor.parent = NULL;
                    enGm->actionFunc = (EnGmActionFunc)func_80A3DC44;
                    Rupees_ChangeBy(OTRGlobals::Instance->gRandoContext->GetItemLocation(RC_GC_MEDIGORON)->GetPrice() * -1);
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
                Rupees_ChangeBy(OTRGlobals::Instance->gRandoContext->GetItemLocation(RC_ZR_MAGIC_BEAN_SALESMAN)->GetPrice() * -1);
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

            if (
                (enFr->songIndex >= FROG_STORMS && enFr->reward == GI_HEART_PIECE) || 
                (enFr->songIndex < FROG_STORMS && enFr->reward == GI_RUPEE_PURPLE)
            ) {
                *should = true;
            }
            break;
        }
        case VB_TRADE_POCKET_CUCCO: {
            Randomizer_ConsumeAdultTradeItem(gPlayState, ITEM_POCKET_CUCCO);
            // Trigger the reward now
            Flags_SetItemGetInf(ITEMGETINF_2E);
            *should = false;
            break;
        }
        case VB_TRADE_COJIRO: {
            Randomizer_ConsumeAdultTradeItem(gPlayState, ITEM_COJIRO);
            *should = false;
            break;
        }
        case VB_TRADE_ODD_MUSHROOM: {
            EnDs* granny = va_arg(args, EnDs*);
            Randomizer_ConsumeAdultTradeItem(gPlayState, ITEM_ODD_MUSHROOM);
            // Trigger the reward now
            Flags_SetItemGetInf(ITEMGETINF_30);
            granny->actor.textId = 0x504F;
            granny->actionFunc = (EnDsActionFunc)EnDs_TalkAfterGiveOddPotion;
            granny->actor.flags &= ~ACTOR_FLAG_PLAYER_TALKED_TO;
            *should = false;
            break;
        }
        case VB_TRADE_ODD_POTION: {
            EnKo* enKo = va_arg(args, EnKo*);
            Randomizer_ConsumeAdultTradeItem(gPlayState, ITEM_ODD_POTION);
            // Trigger the reward now
            Flags_SetItemGetInf(ITEMGETINF_31);
            *should = false;
            break;
        }
        case VB_TRADE_SAW: {
            Randomizer_ConsumeAdultTradeItem(gPlayState, ITEM_SAW);
            *should = false;
            break;
        }
        case VB_TRADE_PRESCRIPTION: {
            EnKz* enKz = va_arg(args, EnKz*);
            // If we aren't setting up the item offer, then we're just checking if it should be possible.
            if (enKz->actionFunc != (EnKzActionFunc)EnKz_SetupGetItem) {
                *should = !Flags_GetRandomizerInf(RAND_INF_ADULT_TRADES_ZD_TRADE_PRESCRIPTION);
                break;
            }
            Randomizer_ConsumeAdultTradeItem(gPlayState, ITEM_PRESCRIPTION);
            *should = false;
            break;
        }
        case VB_TRADE_FROG: {
            Randomizer_ConsumeAdultTradeItem(gPlayState, ITEM_FROG);
            *should = false;
            break;
        }
        case VB_DESPAWN_HORSE_RACE_COW: {
            if (!RAND_GET_OPTION(RSK_SHUFFLE_COWS)) {
                break;
            }
            EnCow* enCow = va_arg(args, EnCow*);
            // If this is a cow we have to move, then move it now.
            EnCow_MoveForRandomizer(enCow, gPlayState);
            break;
        }
        case VB_BUSINESS_SCRUB_DESPAWN: {
            EnShopnuts* enShopnuts = va_arg(args, EnShopnuts*);
            s16 respawnData = gSaveContext.respawn[RESPAWN_MODE_RETURN].data & ((1 << 8) - 1);
            ScrubIdentity scrubIdentity = OTRGlobals::Instance->gRandomizer->IdentifyScrub(gPlayState->sceneNum, enShopnuts->actor.params, respawnData);

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
            if (PLAYER_HAS_SHUFFLED_ADULT_TRADE_ITEM(ITEM_COJIRO)) {
                *should = false;
            } else if (PLAYER_HAS_SHUFFLED_ADULT_TRADE_ITEM(ITEM_ODD_POTION)) {
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

            if (PLAYER_HAS_SHUFFLED_ADULT_TRADE_ITEM(ITEM_COJIRO)) {
                *should = false;
            } else {
                *should = PLAYER_HAS_SHUFFLED_ADULT_TRADE_ITEM(ITEM_ODD_POTION);
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
            if (EnDs_RandoCanGetGrannyItem()){
                *should = gSaveContext.rupees < OTRGlobals::Instance->gRandoContext->GetItemLocation(RC_KAK_GRANNYS_SHOP)->GetPrice();
            }
            break;
        }
        case VB_GRANNY_TAKE_MONEY: {
            if (EnDs_RandoCanGetGrannyItem()){
                    *should = false;
                    Rupees_ChangeBy(OTRGlobals::Instance->gRandoContext->GetItemLocation(RC_KAK_GRANNYS_SHOP)->GetPrice() * -1);
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
            bool eligible = RAND_GET_OPTION(RSK_DOOR_OF_TIME) != RO_DOOROFTIME_CLOSED || (
                INV_CONTENT(ITEM_OCARINA_FAIRY) == ITEM_OCARINA_TIME &&
                CHECK_QUEST_ITEM(QUEST_KOKIRI_EMERALD) &&
                CHECK_QUEST_ITEM(QUEST_GORON_RUBY) &&
                CHECK_QUEST_ITEM(QUEST_ZORA_SAPPHIRE)
            );
            *should = eligible;
            break;
        }
        case VB_GIVE_ITEM_FROM_HORSEBACK_ARCHERY: {
            // give both rewards at the same time
            if (gSaveContext.minigameScore >= 1500) {
                Flags_SetItemGetInf(ITEMGETINF_0F);
            }
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
            if (enBomBowlPit->prizeIndex == EXITEM_BOMB_BAG_BOWLING || enBomBowlPit->prizeIndex == EXITEM_HEART_PIECE_BOWLING) {
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
            if (
                (item == ITEM_NUT && Flags_GetRandomizerInf(RAND_INF_HAS_INFINITE_NUT_UPGRADE)) ||
                (item == ITEM_STICK && Flags_GetRandomizerInf(RAND_INF_HAS_INFINITE_STICK_UPGRADE)) ||
                (item == ITEM_BOMB && Flags_GetRandomizerInf(RAND_INF_HAS_INFINITE_BOMB_BAG)) ||
                (
                    (item == ITEM_BOW || item == ITEM_BOW_ARROW_FIRE || item == ITEM_BOW_ARROW_ICE || item == ITEM_BOW_ARROW_LIGHT) &&
                    Flags_GetRandomizerInf(RAND_INF_HAS_INFINITE_QUIVER)
                ) ||
                (item == ITEM_SLINGSHOT && Flags_GetRandomizerInf(RAND_INF_HAS_INFINITE_BULLET_BAG)) ||
                (item == ITEM_BOMBCHU && Flags_GetRandomizerInf(RAND_INF_HAS_INFINITE_BOMBCHUS))
            ) {
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
        case VB_HAVE_OCARINA_NOTE_B4: {
            if (!Flags_GetRandomizerInf(RAND_INF_HAS_OCARINA_C_LEFT)) {
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
        case VB_SKIP_SCARECROWS_SONG: {
            int ocarinaButtonCount = 0;
            for (int i = VB_HAVE_OCARINA_NOTE_D4; i <= VB_HAVE_OCARINA_NOTE_A4; i++) {
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
            *should = INV_CONTENT((RAND_GET_OPTION(RSK_BOMBCHUS_IN_LOGIC) ? ITEM_BOMBCHU : ITEM_BOMB)) != ITEM_NONE;
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
            if (!*should){
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
            if (IS_RANDO){
                VBFishingData* fishData = va_arg(args, VBFishingData*);
                if (*fishData->sFishOnHandIsLoach) {
                    if (!Flags_GetRandomizerInf(RAND_INF_CAUGHT_LOACH) &&
                        OTRGlobals::Instance->gRandomizer->GetRandoSettingValue(RSK_FISHSANITY) == RO_FISHSANITY_HYRULE_LOACH){
                        Flags_SetRandomizerInf(RAND_INF_CAUGHT_LOACH);
                        Message_StartTextbox(gPlayState, TEXT_FISHING_RELEASE_THIS_ONE, NULL);
                        *should = true;
                        fishData->actor->stateAndTimer = 20;
                    }
                } else {
                    if (ShouldGiveFishingPrize(fishData->fishWeight)){
                        if (LINK_IS_CHILD){
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
        case VB_TRADE_TIMER_ODD_MUSHROOM:
        case VB_TRADE_TIMER_EYEDROPS:
        case VB_TRADE_TIMER_FROG:
        case VB_ANJU_SET_OBTAINED_TRADE_ITEM:
        case VB_GIVE_ITEM_FROM_TARGET_IN_WOODS:
        case VB_GIVE_ITEM_FROM_TALONS_CHICKENS:
        case VB_GIVE_ITEM_FROM_DIVING_MINIGAME:
        case VB_GIVE_ITEM_FROM_GORON:
        case VB_GIVE_ITEM_FROM_LAB_DIVE:
        case VB_GIVE_ITEM_FROM_SKULL_KID_SARIAS_SONG:
        case VB_GIVE_ITEM_FROM_MAN_ON_ROOF:
        case VB_GIVE_ITEM_SKULL_TOKEN:
        case VB_GIVE_ITEM_FROM_BLUE_WARP:
        case VB_GIVE_ITEM_FAIRY_OCARINA:
        case VB_GIVE_ITEM_WEIRD_EGG:
        case VB_GIVE_ITEM_LIGHT_ARROW:
        case VB_GIVE_ITEM_STRENGTH_1:
        case VB_GIVE_ITEM_ZELDAS_LETTER:
        case VB_GIVE_ITEM_OCARINA_OF_TIME:
        case VB_GIVE_ITEM_KOKIRI_EMERALD:
        case VB_GIVE_ITEM_GORON_RUBY:
        case VB_GIVE_ITEM_ZORA_SAPPHIRE:
        case VB_GIVE_ITEM_LIGHT_MEDALLION:
        case VB_GIVE_ITEM_FOREST_MEDALLION:
        case VB_GIVE_ITEM_FIRE_MEDALLION:
        case VB_GIVE_ITEM_WATER_MEDALLION:
        case VB_GIVE_ITEM_SPIRIT_MEDALLION:
        case VB_GIVE_ITEM_SHADOW_MEDALLION:
            *should = false;
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
        Rando::Context::GetInstance()->GetItemLocation(RC_MARKET_TREASURE_CHEST_GAME_ITEM_1)->SetCheckStatus(RCSHOW_UNCHECKED);
        Flags_UnsetRandomizerInf(RAND_INF_MARKET_TREASURE_CHEST_GAME_ITEM_2);
        Rando::Context::GetInstance()->GetItemLocation(RC_MARKET_TREASURE_CHEST_GAME_ITEM_2)->SetCheckStatus(RCSHOW_UNCHECKED);
        Flags_UnsetRandomizerInf(RAND_INF_MARKET_TREASURE_CHEST_GAME_ITEM_3);
        Rando::Context::GetInstance()->GetItemLocation(RC_MARKET_TREASURE_CHEST_GAME_ITEM_3)->SetCheckStatus(RCSHOW_UNCHECKED);
        Flags_UnsetRandomizerInf(RAND_INF_MARKET_TREASURE_CHEST_GAME_ITEM_4);
        Rando::Context::GetInstance()->GetItemLocation(RC_MARKET_TREASURE_CHEST_GAME_ITEM_4)->SetCheckStatus(RCSHOW_UNCHECKED);
        Flags_UnsetRandomizerInf(RAND_INF_MARKET_TREASURE_CHEST_GAME_ITEM_5);
        Rando::Context::GetInstance()->GetItemLocation(RC_MARKET_TREASURE_CHEST_GAME_ITEM_5)->SetCheckStatus(RCSHOW_UNCHECKED);
        Flags_UnsetRandomizerInf(RAND_INF_MARKET_TREASURE_CHEST_GAME_KEY_1);
        Rando::Context::GetInstance()->GetItemLocation(RC_MARKET_TREASURE_CHEST_GAME_KEY_1)->SetCheckStatus(RCSHOW_UNCHECKED);
        Flags_UnsetRandomizerInf(RAND_INF_MARKET_TREASURE_CHEST_GAME_KEY_2);
        Rando::Context::GetInstance()->GetItemLocation(RC_MARKET_TREASURE_CHEST_GAME_KEY_2)->SetCheckStatus(RCSHOW_UNCHECKED);
        Flags_UnsetRandomizerInf(RAND_INF_MARKET_TREASURE_CHEST_GAME_KEY_3);
        Rando::Context::GetInstance()->GetItemLocation(RC_MARKET_TREASURE_CHEST_GAME_KEY_3)->SetCheckStatus(RCSHOW_UNCHECKED);
        Flags_UnsetRandomizerInf(RAND_INF_MARKET_TREASURE_CHEST_GAME_KEY_4);
        Rando::Context::GetInstance()->GetItemLocation(RC_MARKET_TREASURE_CHEST_GAME_KEY_4)->SetCheckStatus(RCSHOW_UNCHECKED);
        Flags_UnsetRandomizerInf(RAND_INF_MARKET_TREASURE_CHEST_GAME_KEY_5);
        Rando::Context::GetInstance()->GetItemLocation(RC_MARKET_TREASURE_CHEST_GAME_KEY_5)->SetCheckStatus(RCSHOW_UNCHECKED);
        CheckTracker::RecalculateAllAreaTotals();
    }

    if (RAND_GET_OPTION(RSK_SHUFFLE_ENTRANCES)) {
        Entrance_OverrideWeatherState();
    }

    // LACs & Prelude checks
    static uint32_t updateHook = 0;

    if (updateHook) {
        GameInteractor::Instance->UnregisterGameHook<GameInteractor::OnPlayerUpdate>(updateHook);
        updateHook = 0;
    }

    // If we're not in the Temple of Time or we've already learned the Prelude of Light and received LACs, we don't need to do anything
    if (
        sceneNum != SCENE_TEMPLE_OF_TIME || 
        (
            Flags_GetEventChkInf(EVENTCHKINF_LEARNED_PRELUDE_OF_LIGHT) &&
            Flags_GetEventChkInf(EVENTCHKINF_RETURNED_TO_TEMPLE_OF_TIME_WITH_ALL_MEDALLIONS)
        )
    ) return;

    updateHook = GameInteractor::Instance->RegisterGameHook<GameInteractor::OnPlayerUpdate>([]() {
        if (!Flags_GetEventChkInf(EVENTCHKINF_LEARNED_PRELUDE_OF_LIGHT) && LINK_IS_ADULT && CHECK_QUEST_ITEM(QUEST_MEDALLION_FOREST) && gPlayState->roomCtx.curRoom.num == 0) {
            Flags_SetEventChkInf(EVENTCHKINF_LEARNED_PRELUDE_OF_LIGHT);
        }

        // We're always in rando here, and rando always overrides this should so we can just pass false
        if (GameInteractor_Should(VB_BE_ELIGIBLE_FOR_LIGHT_ARROWS, false)) {
            Flags_SetEventChkInf(EVENTCHKINF_RETURNED_TO_TEMPLE_OF_TIME_WITH_ALL_MEDALLIONS);
        }

        // If both awards have been given, we can unregister the hook, otherwise it will get unregistered when the player leaves the area
        if (
            Flags_GetEventChkInf(EVENTCHKINF_LEARNED_PRELUDE_OF_LIGHT) &&
            Flags_GetEventChkInf(EVENTCHKINF_RETURNED_TO_TEMPLE_OF_TIME_WITH_ALL_MEDALLIONS)
        ) {
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

    if (
        RAND_GET_OPTION(RSK_SHUFFLE_BEEHIVES) &&
        !Flags_GetRandomizerInf(objComb->beehiveIdentity.randomizerInf)
    ) {
        EnItem00* item00 = (EnItem00*)Item_DropCollectible2(play, &objComb->actor.world.pos, ITEM00_SOH_DUMMY);
        item00->randoInf = objComb->beehiveIdentity.randomizerInf;
        item00->itemEntry = OTRGlobals::Instance->gRandomizer->GetItemFromKnownCheck(objComb->beehiveIdentity.randomizerCheck, GI_NONE);
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
    if (
        RAND_GET_OPTION(RSK_SHUFFLE_BEEHIVES) &&
        !Flags_GetRandomizerInf(objComb->beehiveIdentity.randomizerInf)
    ) {
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
        RandomizerCheck rc = OTRGlobals::Instance->gRandomizer->GetCheckFromActor(actor->id, gPlayState->sceneNum, actor->params);
        if (rc != RC_UNKNOWN_CHECK) {
            EnSi* enSi = static_cast<EnSi*>(actorRef);
            enSi->sohGetItemEntry = Rando::Context::GetInstance()->GetFinalGIEntry(rc, true, (GetItemID)Rando::StaticData::GetLocation(rc)->GetVanillaItem());
            actor->draw = (ActorFunc)EnSi_DrawRandomizedItem;
        }
    }

    if (actor->id == ACTOR_EN_DNS) {
        EnDns* enDns = static_cast<EnDns*>(actorRef);
        s16 respawnData = gSaveContext.respawn[RESPAWN_MODE_RETURN].data & ((1 << 8) - 1);
        enDns->sohScrubIdentity = OTRGlobals::Instance->gRandomizer->IdentifyScrub(gPlayState->sceneNum, enDns->actor.params, respawnData);

        if (enDns->sohScrubIdentity.isShuffled) {
            // DNS uses pointers so we're creating our own entry instead of modifying the original
            enDns->sohDnsItemEntry = {
                enDns->dnsItemEntry->itemPrice,
                1,
                enDns->sohScrubIdentity.getItemId,
                EnDns_RandomizerPurchaseableCheck,
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
                enDnsUpdateHook = GameInteractor::Instance->RegisterGameHook<GameInteractor::OnActorUpdate>([](void* innerActorRef) {
                    Actor* innerActor = static_cast<Actor*>(innerActorRef);
                    if (innerActor->id == ACTOR_EN_DNS) {
                        EnDns* innerEnDns = static_cast<EnDns*>(innerActorRef);
                        if (innerEnDns->sohScrubIdentity.isShuffled) {
                            innerActor->textId = TEXT_SCRUB_RANDOM;
                        }
                    }
                });
                enDnsKillHook = GameInteractor::Instance->RegisterGameHook<GameInteractor::OnSceneInit>([](int16_t sceneNum) {
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
        RandomizerCheck rc = OTRGlobals::Instance->gRandomizer->GetCheckFromActor(itemEtcetera->actor.id, gPlayState->sceneNum, itemEtcetera->actor.params);
        if (rc != RC_UNKNOWN_CHECK) {
            itemEtcetera->sohItemEntry = Rando::Context::GetInstance()->GetFinalGIEntry(rc, true, (GetItemID)Rando::StaticData::GetLocation(rc)->GetVanillaItem());
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
        objComb->beehiveIdentity = OTRGlobals::Instance->gRandomizer->IdentifyBeehive(gPlayState->sceneNum, (s16)actor->world.pos.x, respawnData);
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
                //RC_MARKET_BOMBCHU_BOWLING_BOMBCHUS was removed as a 3DS holdover not in anyones near term plans due to being pretty useless.
                break;
            case EXITEM_BULLET_BAG:
                rc = RC_LW_TARGET_IN_WOODS;
                break;
        }
        if (rc != RC_UNKNOWN_CHECK) {
            enExItem->sohItemEntry = Rando::Context::GetInstance()->GetFinalGIEntry(rc, true, (GetItemID)Rando::StaticData::GetLocation(rc)->GetVanillaItem());
            enExItem->actionFunc = (EnExItemActionFunc)EnExItem_WaitForObjectRandomized;
        }
    }

    if (actor->id == ACTOR_EN_GE1) {
        EnGe1* enGe1 = static_cast<EnGe1*>(actorRef);
        auto ge1Type = enGe1->actor.params & 0xFF;
        if (ge1Type == GE1_TYPE_TRAINING_GROUNDS_GUARD &&
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
        (RAND_GET_OPTION(RSK_FOREST) == RO_FOREST_OPEN ||
            Flags_GetEventChkInf(EVENTCHKINF_SHOWED_MIDO_SWORD_SHIELD))) {
        BgTreemouth* bgTreemouth = static_cast<BgTreemouth*>(actorRef);
        bgTreemouth->unk_168 = 1.0f;
    }

    //consumable bags
    if (
        actor->id == ACTOR_EN_ITEM00 &&
        (
            (
                RAND_GET_OPTION(RSK_SHUFFLE_DEKU_STICK_BAG) &&
                CUR_UPG_VALUE(UPG_STICKS) == 0 &&
                actor->params == ITEM00_STICK
            ) ||
            (
                RAND_GET_OPTION(RSK_SHUFFLE_DEKU_NUT_BAG) &&
                CUR_UPG_VALUE(UPG_NUTS) == 0 &&
                actor->params == ITEM00_NUTS
            )
        )
    ) {
        Actor_Kill(actor);
    }

    if (RAND_GET_OPTION(RSK_SHUFFLE_BOSS_SOULS)) {
        //Boss souls require an additional item (represented by a RAND_INF) to spawn a boss in a particular lair
        RandomizerInf currentBossSoulRandInf = RAND_INF_MAX;
        switch (gPlayState->sceneNum){
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

        //Deletes all actors in the boss category if the soul isn't found.
        //Some actors, like Dark Link, Arwings, and Zora's Sapphire...?, are in this category despite not being actual bosses,
        //so ignore any "boss" if `currentBossSoulRandInf` doesn't change from RAND_INF_MAX.
        if (currentBossSoulRandInf != RAND_INF_MAX) {
            if (!Flags_GetRandomizerInf(currentBossSoulRandInf) && actor->category == ACTORCAT_BOSS) {
                Actor_Delete(&gPlayState->actorCtx, actor, gPlayState);
            }
            //Special case for Phantom Ganon's horse (and fake), as they're considered "background actors",
            //but still control the boss fight flow.
            if (!Flags_GetRandomizerInf(RAND_INF_PHANTOM_GANON_SOUL) && actor->id == ACTOR_EN_FHG) {
                Actor_Delete(&gPlayState->actorCtx, actor, gPlayState);
            }
        }
    }

    // In MQ Spirit, remove the large silver block in the hole as child so the chest in the silver block hallway
    // can be guaranteed accessible
    if (
        actor->id == ACTOR_OBJ_OSHIHIKI &&
        LINK_IS_CHILD &&
        IsGameMasterQuest() &&
        gPlayState->sceneNum == SCENE_SPIRIT_TEMPLE && actor->room == 6 && // Spirit Temple silver block hallway
        actor->params == 0x9C7 // Silver block that is marked as in the hole
    ) {
        Actor_Kill(actor);
        return;
    }

    if (
        // If child is in the adult shooting gallery or adult in the child shooting gallery, then despawn the shooting gallery man
        actor->id == ACTOR_EN_SYATEKI_MAN &&
        RAND_GET_OPTION(RSK_SHUFFLE_INTERIOR_ENTRANCES) &&
        (
            (LINK_IS_CHILD && Entrance_SceneAndSpawnAre(SCENE_SHOOTING_GALLERY, 0x00)) || //Kakariko Village -> Adult Shooting Gallery, index 003B in the entrance table
            (LINK_IS_ADULT && Entrance_SceneAndSpawnAre(SCENE_SHOOTING_GALLERY, 0x01)) //Market -> Child Shooting Gallery,              index 016D in the entrance table
        )
    ) {
        Actor_Kill(actor);
        return;
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
    if (RAND_GET_OPTION(RSK_SHUFFLE_ENTRANCES) && actor->id == ACTOR_DEMO_KANKYO && actor->params == 0x000F) { // Warp Song particles
        Entrance_SetWarpSongEntrance();
    }

    if (actor->id == ACTOR_OBJ_COMB) {
        ObjComb* combActor = reinterpret_cast<ObjComb*>(actor);
        combActor->actor.shape.rot.x = Math_SinS(combActor->unk_1B2) * CLAMP_MIN(combActor->unk_1B0, 0) + combActor->actor.home.rot.x;
    }
}

//from z_player.c
typedef struct {
    /* 0x00 */ Vec3f pos;
    /* 0x0C */ s16 yaw;
} SpecialRespawnInfo; // size = 0x10

//special respawns used when voided out without swim to prevent infinite loops
std::map<s32, SpecialRespawnInfo> swimSpecialRespawnInfo = {
    {
        ENTR_ZORAS_RIVER_3,//hf to zr in water
        { { -1455.443, -20, 1384.826 }, 28761 }
    },
    {
        ENTR_HYRULE_FIELD_14,//zr to hf in water
        { { 5730.209, -20, 3725.911 }, -20025 }
    },
    {
        ENTR_LOST_WOODS_7,//zr to lw
        { { 1978.718, -36.908, -855 }, -16384 }
    },
    {
        ENTR_ZORAS_RIVER_4,//lw to zr
        { { 4082.366, 860.442, -1018.949 }, -32768 }
    },
    {
        ENTR_LAKE_HYLIA_1,//gv to lh
        { { -3276.416, -1033, 2908.421 }, 11228 }
    },
    {
        ENTR_WATER_TEMPLE_0,//lh to water temple
        { { -182, 780, 759.5 }, -32768 }
    },
    {
        ENTR_LAKE_HYLIA_2,//water temple to lh
        { { -955.028, -1306.9, 6768.954 }, -32768 }
    },
    {
        ENTR_ZORAS_DOMAIN_4,//lh to zd
        { { -109.86, 11.396, -9.933 }, -29131 }
    },
    {
        ENTR_LAKE_HYLIA_7,//zd to lh
        { { -912, -1326.967, 3391 }, 0 }
    },
    {
        ENTR_GERUDO_VALLEY_1,//caught by gerudos as child
        { { -424, -2051, -74 }, 16384 }
    },
    {
        ENTR_HYRULE_FIELD_7,//mk to hf (can be a problem when it then turns night)
        { { 0, 0, 1100 }, 0 }
    },
    {
        ENTR_ZORAS_FOUNTAIN_0,//jabu blue warp to zf
        { { -1580, 150, 1670 }, 8000 }
    },
};

f32 triforcePieceScale;

void RandomizerOnPlayerUpdateHandler() {
    if (
        (GET_PLAYER(gPlayState)->stateFlags1 & PLAYER_STATE1_IN_WATER) &&
        !Flags_GetRandomizerInf(RAND_INF_CAN_SWIM) &&
        CUR_EQUIP_VALUE(EQUIP_TYPE_BOOTS) != EQUIP_VALUE_BOOTS_IRON
    ) {
        //if you void out in water temple without swim you get instantly kicked out to prevent softlocks
        if (gPlayState->sceneNum == SCENE_WATER_TEMPLE) {
            GameInteractor::RawAction::TeleportPlayer(Entrance_OverrideNextIndex(ENTR_LAKE_HYLIA_2));//lake hylia from water temple
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
    if (
        !GameInteractor::IsGameplayPaused() &&
        Flags_GetRandomizerInf(RAND_INF_GRANT_GANONS_BOSSKEY) &&
        gPlayState->transitionTrigger != TRANS_TRIGGER_START &&
        (1 << 0 & gSaveContext.inventory.dungeonItems[SCENE_GANONS_TOWER]) == 0
    ) {
        GiveItemEntryWithoutActor(gPlayState, ItemTableManager::Instance->RetrieveItemEntry(MOD_RANDOMIZER, RG_GANONS_CASTLE_BOSS_KEY));
    }

    if (
        !GameInteractor::IsGameplayPaused() &&
        RAND_GET_OPTION(RSK_TRIFORCE_HUNT)
    ) {
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
                    Actor_Spawn(&gPlayState->actorCtx, gPlayState, ACTOR_EN_XC, -104, -40, 2382, 0, 0x8000, 0, SHEIK_TYPE_RANDO, false);
                }
                break;
            case SCENE_INSIDE_GANONS_CASTLE:
                if (gPlayState->roomCtx.curRoom.num == 1) {
                    Actor_Spawn(&gPlayState->actorCtx, gPlayState, ACTOR_EN_XC, 101, 150, 137, 0, 0, 0, SHEIK_TYPE_RANDO, false);
                }
                break;
            default:
                break;
        }
    }
}

void RandomizerRegisterHooks() {
    static uint32_t onFlagSetHook = 0;
    static uint32_t onSceneFlagSetHook = 0;
    static uint32_t onPlayerUpdateForRCQueueHook = 0;
    static uint32_t onPlayerUpdateForItemQueueHook = 0;
    static uint32_t onItemReceiveHook = 0;
    static uint32_t onVanillaBehaviorHook = 0;
    static uint32_t onSceneInitHook = 0;
    static uint32_t onActorInitHook = 0;
    static uint32_t onActorUpdateHook = 0;
    static uint32_t onPlayerUpdateHook = 0;
    static uint32_t onGameFrameUpdateHook = 0;
    static uint32_t onSceneSpawnActorsHook = 0;

    static uint32_t fishsanityOnActorInitHook = 0;
    static uint32_t fishsanityOnFlagSetHook = 0;
    static uint32_t fishsanityOnActorUpdateHook = 0;
    static uint32_t fishsanityOnSceneInitHook = 0;
    static uint32_t fishsanityOnVanillaBehaviorHook = 0;

    GameInteractor::Instance->RegisterGameHook<GameInteractor::OnLoadGame>([](int32_t fileNum) {
        randomizerQueuedChecks = std::queue<RandomizerCheck>();
        randomizerQueuedCheck = RC_UNKNOWN_CHECK;
        randomizerQueuedItemEntry = GET_ITEM_NONE;

        GameInteractor::Instance->UnregisterGameHook<GameInteractor::OnFlagSet>(onFlagSetHook);
        GameInteractor::Instance->UnregisterGameHook<GameInteractor::OnSceneFlagSet>(onSceneFlagSetHook);
        GameInteractor::Instance->UnregisterGameHook<GameInteractor::OnPlayerUpdate>(onPlayerUpdateForRCQueueHook);
        GameInteractor::Instance->UnregisterGameHook<GameInteractor::OnPlayerUpdate>(onPlayerUpdateForItemQueueHook);
        GameInteractor::Instance->UnregisterGameHook<GameInteractor::OnItemReceive>(onItemReceiveHook);
        GameInteractor::Instance->UnregisterGameHook<GameInteractor::OnVanillaBehavior>(onVanillaBehaviorHook);
        GameInteractor::Instance->UnregisterGameHook<GameInteractor::OnSceneInit>(onSceneInitHook);
        GameInteractor::Instance->UnregisterGameHook<GameInteractor::OnActorInit>(onActorInitHook);
        GameInteractor::Instance->UnregisterGameHook<GameInteractor::OnActorUpdate>(onActorUpdateHook);
        GameInteractor::Instance->UnregisterGameHook<GameInteractor::OnPlayerUpdate>(onPlayerUpdateHook);
        GameInteractor::Instance->UnregisterGameHook<GameInteractor::OnGameFrameUpdate>(onGameFrameUpdateHook);
        GameInteractor::Instance->UnregisterGameHook<GameInteractor::OnSceneSpawnActors>(onSceneSpawnActorsHook);

        GameInteractor::Instance->UnregisterGameHook<GameInteractor::OnActorInit>(fishsanityOnActorInitHook);
        GameInteractor::Instance->UnregisterGameHook<GameInteractor::OnFlagSet>(fishsanityOnFlagSetHook);
        GameInteractor::Instance->UnregisterGameHook<GameInteractor::OnActorUpdate>(fishsanityOnActorUpdateHook);
        GameInteractor::Instance->UnregisterGameHook<GameInteractor::OnSceneInit>(fishsanityOnSceneInitHook);
        GameInteractor::Instance->UnregisterGameHook<GameInteractor::OnVanillaBehavior>(fishsanityOnVanillaBehaviorHook);

        onFlagSetHook = 0;
        onSceneFlagSetHook = 0;
        onPlayerUpdateForRCQueueHook = 0;
        onPlayerUpdateForItemQueueHook = 0;
        onItemReceiveHook = 0;
        onVanillaBehaviorHook = 0;
        onSceneInitHook = 0;
        onActorInitHook = 0;
        onActorUpdateHook = 0;
        onPlayerUpdateHook = 0;
        onGameFrameUpdateHook = 0;
        onSceneSpawnActorsHook = 0;

        fishsanityOnActorInitHook = 0;
        fishsanityOnFlagSetHook = 0;
        fishsanityOnActorUpdateHook = 0;
        fishsanityOnSceneInitHook = 0;
        fishsanityOnVanillaBehaviorHook = 0;

        if (!IS_RANDO) return;

        // Setup the modified entrance table and entrance shuffle table for rando
        Entrance_Init();

        // Handle randomized spawn positions after the save context has been setup from load
        if (RAND_GET_OPTION(RSK_SHUFFLE_ENTRANCES)) {
            Entrance_SetSavewarpEntrance();
        }

        onFlagSetHook = GameInteractor::Instance->RegisterGameHook<GameInteractor::OnFlagSet>(RandomizerOnFlagSetHandler);
        onSceneFlagSetHook = GameInteractor::Instance->RegisterGameHook<GameInteractor::OnSceneFlagSet>(RandomizerOnSceneFlagSetHandler);
        onPlayerUpdateForRCQueueHook = GameInteractor::Instance->RegisterGameHook<GameInteractor::OnPlayerUpdate>(RandomizerOnPlayerUpdateForRCQueueHandler);
        onPlayerUpdateForItemQueueHook = GameInteractor::Instance->RegisterGameHook<GameInteractor::OnPlayerUpdate>(RandomizerOnPlayerUpdateForItemQueueHandler);
        onItemReceiveHook = GameInteractor::Instance->RegisterGameHook<GameInteractor::OnItemReceive>(RandomizerOnItemReceiveHandler);
        onVanillaBehaviorHook = GameInteractor::Instance->RegisterGameHook<GameInteractor::OnVanillaBehavior>(RandomizerOnVanillaBehaviorHandler);
        onSceneInitHook = GameInteractor::Instance->RegisterGameHook<GameInteractor::OnSceneInit>(RandomizerOnSceneInitHandler);
        onActorInitHook = GameInteractor::Instance->RegisterGameHook<GameInteractor::OnActorInit>(RandomizerOnActorInitHandler);
        onActorUpdateHook = GameInteractor::Instance->RegisterGameHook<GameInteractor::OnActorUpdate>(RandomizerOnActorUpdateHandler);
        onPlayerUpdateHook = GameInteractor::Instance->RegisterGameHook<GameInteractor::OnPlayerUpdate>(RandomizerOnPlayerUpdateHandler);
        onGameFrameUpdateHook = GameInteractor::Instance->RegisterGameHook<GameInteractor::OnGameFrameUpdate>(RandomizerOnGameFrameUpdateHandler);
        onSceneSpawnActorsHook = GameInteractor::Instance->RegisterGameHook<GameInteractor::OnSceneSpawnActors>(RandomizerOnSceneSpawnActorsHandler);

        if (RAND_GET_OPTION(RSK_FISHSANITY) != RO_FISHSANITY_OFF) {
            OTRGlobals::Instance->gRandoContext->GetFishsanity()->InitializeFromSave();

            fishsanityOnActorInitHook = GameInteractor::Instance->RegisterGameHook<GameInteractor::OnActorInit>(Rando::Fishsanity::OnActorInitHandler);
            fishsanityOnFlagSetHook = GameInteractor::Instance->RegisterGameHook<GameInteractor::OnFlagSet>(Rando::Fishsanity::OnFlagSetHandler);
            fishsanityOnActorUpdateHook = GameInteractor::Instance->RegisterGameHook<GameInteractor::OnActorUpdate>(Rando::Fishsanity::OnActorUpdateHandler);
            fishsanityOnSceneInitHook = GameInteractor::Instance->RegisterGameHook<GameInteractor::OnSceneInit>(Rando::Fishsanity::OnSceneInitHandler);
            fishsanityOnVanillaBehaviorHook = GameInteractor::Instance->RegisterGameHook<GameInteractor::OnVanillaBehavior>(Rando::Fishsanity::OnVanillaBehaviorHandler);
        }
    });
}
