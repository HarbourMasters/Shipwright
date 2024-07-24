#include <libultraship/bridge.h>
#include "soh/OTRGlobals.h"
#include "soh/Enhancements/enhancementTypes.h"
#include "soh/Enhancements/custom-message/CustomMessageTypes.h"
#include "soh/Enhancements/randomizer/randomizerTypes.h"
#include "soh/Enhancements/randomizer/dungeon.h"
#include "soh/Enhancements/game-interactor/GameInteractor.h"
#include "soh/Enhancements/game-interactor/GameInteractor_Hooks.h"

extern "C" {
#include "macros.h"
#include "functions.h"
#include "variables.h"
#include "soh/Enhancements/randomizer/adult_trade_shuffle.h"
#include "src/overlays/actors/ovl_En_Si/z_en_si.h"
#include "src/overlays/actors/ovl_En_Cow/z_en_cow.h"
#include "src/overlays/actors/ovl_En_Shopnuts/z_en_shopnuts.h"
#include "src/overlays/actors/ovl_En_Dns/z_en_dns.h"
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
#include "adult_trade_shuffle.h"
#include "draw.h"

extern SaveContext gSaveContext;
extern PlayState* gPlayState;
extern void func_8084DFAC(PlayState* play, Player* player);
extern void func_80835DAC(PlayState* play, Player* player, PlayerActionFunc actionFunc, s32 flags);
extern s32 func_80836898(PlayState* play, Player* player, PlayerFuncA74 func);
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
    if (Player_InBlockingCsMode(gPlayState, player) || player->stateFlags1 & PLAYER_STATE1_IN_ITEM_CS || player->stateFlags1 & PLAYER_STATE1_GETTING_ITEM || player->stateFlags1 & PLAYER_STATE1_ITEM_OVER_HEAD) {
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
            rc != RC_MARKET_BOMBCHU_BOWLING_BOMBCHUS &&
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
    if (player == NULL || Player_InBlockingCsMode(gPlayState, player) || player->stateFlags1 & PLAYER_STATE1_IN_ITEM_CS || player->stateFlags1 & PLAYER_STATE1_GETTING_ITEM || player->stateFlags1 & PLAYER_STATE1_ITEM_OVER_HEAD) {
        return;
    }

    SPDLOG_INFO("Attempting to give Item mod {} item {} from RC {}", randomizerQueuedItemEntry.modIndex, randomizerQueuedItemEntry.itemId, static_cast<uint32_t>(randomizerQueuedCheck));
    GiveItemEntryWithoutActor(gPlayState, randomizerQueuedItemEntry);
    if (player->stateFlags1 & PLAYER_STATE1_IN_WATER) {
        // Allow the player to receive the item while swimming
        player->stateFlags2 |= PLAYER_STATE2_UNDERWATER;
        Player_ActionChange_2(player, gPlayState);
    }
}

void RandomizerOnItemReceiveHandler(GetItemEntry receivedItemEntry) {
    if (randomizerQueuedCheck == RC_UNKNOWN_CHECK) return;

    auto loc = Rando::Context::GetInstance()->GetItemLocation(randomizerQueuedCheck);
    if (randomizerQueuedItemEntry.modIndex == receivedItemEntry.modIndex && randomizerQueuedItemEntry.itemId == receivedItemEntry.itemId) {
        SPDLOG_INFO("Item received mod {} item {} from RC {}", receivedItemEntry.modIndex, receivedItemEntry.itemId, static_cast<uint32_t>(randomizerQueuedCheck));
        loc->MarkAsObtained();
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
            if (player == NULL || Player_InBlockingCsMode(gPlayState, player) || player->stateFlags1 & PLAYER_STATE1_IN_ITEM_CS || player->stateFlags1 & PLAYER_STATE1_GETTING_ITEM || player->stateFlags1 & PLAYER_STATE1_ITEM_OVER_HEAD) {
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
    return RAND_GET_OPTION(RSK_SHUFFLE_MERCHANTS) != RO_SHUFFLE_MERCHANTS_OFF &&
           !Flags_GetRandomizerInf(RAND_INF_MERCHANTS_GRANNYS_SHOP) &&
           // Traded odd mushroom when adult trade is on
           ((RAND_GET_OPTION(RSK_SHUFFLE_ADULT_TRADE) && Flags_GetItemGetInf(ITEMGETINF_30)) ||
            // Found claim check when adult trade is off
            (!RAND_GET_OPTION(RSK_SHUFFLE_ADULT_TRADE) &&
             INV_CONTENT(ITEM_CLAIM_CHECK) == ITEM_CLAIM_CHECK));
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
    }
}

void Player_Action_8084E6D4_override(Player* player, PlayState* play) {
    if (LinkAnimation_Update(play, &player->skelAnime)) {
        func_8084DFAC(play, player);
    }
}

void func_8083A434_override(PlayState* play, Player* player) {
    func_80835DAC(play, player, Player_Action_8084E6D4_override, 0);
    player->stateFlags1 |= PLAYER_STATE1_GETTING_ITEM | PLAYER_STATE1_IN_CUTSCENE;
}

void RandomizerOnVanillaBehaviorHandler(GIVanillaBehavior id, bool* should, void* optionalArg) {
    switch (id) {
        case VB_PLAY_SLOW_CHEST_CS: {
            // We force fast chests if SkipGetItemAnimation is enabled because the camera in the CS looks pretty wonky otherwise
            if (CVarGetInteger(CVAR_ENHANCEMENT("TimeSavers.SkipGetItemAnimation"), SGIA_DISABLED)) {
                *should = false;
            }
            break;
        }
        case VB_GIVE_ITEM_FROM_CHEST: {
            EnBox* chest = static_cast<EnBox*>(optionalArg);
            RandomizerCheck rc = OTRGlobals::Instance->gRandomizer->GetCheckFromActor(chest->dyna.actor.id, gPlayState->sceneNum, chest->dyna.actor.params);
            
            // if this is a treasure chest game chest then set the appropriate rando inf
            RandomizerSetChestGameRandomizerInf(rc);

            Player* player = GET_PLAYER(gPlayState);
            func_80836898(gPlayState, player, func_8083A434_override);

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
            *should = !Flags_GetEventChkInf(EVENTCHKINF_RETURNED_TO_TEMPLE_OF_TIME_WITH_ALL_MEDALLIONS) && MeetsLACSRequirements();
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
            if (RAND_GET_OPTION(RSK_SHUFFLE_MAGIC_BEANS)) {
                *should = gSaveContext.rupees >= 60;
            }
            break;
        }
        case VB_GIVE_ITEM_MASTER_SWORD:
            if (RAND_GET_OPTION(RSK_SHUFFLE_MASTER_SWORD)) {
                *should = false;
            } else {
                *should = true;
                Rando::Context::GetInstance()->GetItemLocation(RC_TOT_MASTER_SWORD)->MarkAsObtained();
            }
            break;
        case VB_ITEM00_DESPAWN: {
            EnItem00* item00 = static_cast<EnItem00*>(optionalArg);
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
        case VB_MALON_ALREADY_TAUGHT_EPONAS_SONG: {
            *should = Flags_GetRandomizerInf(RAND_INF_LEARNED_EPONA_SONG);
            break;
        }
        case VB_SET_CUCCO_COUNT: {
            EnNiwLady* enNiwLady = static_cast<EnNiwLady*>(optionalArg);
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
            EnItem00* item00 = static_cast<EnItem00*>(optionalArg);
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
            EnCow* enCow = static_cast<EnCow*>(optionalArg);
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
            // Only setting the inf if we've actually gotten the rando item and not the vanilla blue potion
            Flags_SetRandomizerInf(RAND_INF_MERCHANTS_GRANNYS_SHOP);
            *should = false;
            break;
        }
        case VB_GIVE_ITEM_FROM_THAWING_KING_ZORA: {
            EnKz* enKz = static_cast<EnKz*>(optionalArg);
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
        case VB_GIVE_ITEM_FROM_CARPET_SALESMAN: {
            *should = RAND_GET_OPTION(RSK_SHUFFLE_MERCHANTS) == RO_SHUFFLE_MERCHANTS_OFF ||
                      // If the rando check has already been awarded, use vanilla behavior.
                      Flags_GetRandomizerInf(RAND_INF_MERCHANTS_CARPET_SALESMAN);
            break;
        }
        case VB_GIVE_ITEM_FROM_MEDIGORON: {
            // fallthrough
        case VB_BE_ELIGIBLE_FOR_GIANTS_KNIFE_PURCHASE:
            if (RAND_GET_OPTION(RSK_SHUFFLE_MERCHANTS) != RO_SHUFFLE_MERCHANTS_OFF &&
                !Flags_GetRandomizerInf(RAND_INF_MERCHANTS_MEDIGORON)) {
                if (id == VB_GIVE_ITEM_FROM_MEDIGORON) {
                    Flags_SetInfTable(INFTABLE_B1);
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
            EnMs* enMs = static_cast<EnMs*>(optionalArg);
            if (RAND_GET_OPTION(RSK_SHUFFLE_MAGIC_BEANS)) {
                Rupees_ChangeBy(-60);
                BEANS_BOUGHT = 10;
                // Only set inf for buying rando check
                Flags_SetRandomizerInf(RAND_INF_MERCHANTS_MAGIC_BEAN_SALESMAN);
                enMs->actionFunc = (EnMsActionFunc)EnMs_Wait;
                *should = false;
            }
            break;
        }
        case VB_GIVE_ITEM_FROM_FROGS: {
            EnFr* enFr = static_cast<EnFr*>(optionalArg);

            // Skip GiveReward+SetIdle action func if the reward is an ice trap
            if (enFr->actionFunc == (EnFrActionFunc)EnFr_GiveReward) {
                RandomizerCheck rc = EnFr_RandomizerCheckFromSongIndex(enFr->songIndex);
                GetItemEntry gi = Rando::Context::GetInstance()->GetFinalGIEntry(rc, true, (GetItemID)Rando::StaticData::GetLocation(rc)->GetVanillaItem());
                if (gi.getItemId == RG_ICE_TRAP) {
                    enFr->actionFunc = (EnFrActionFunc)EnFr_Idle;
                }
            }

            *should = false;
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
            Randomizer_ConsumeAdultTradeItem(gPlayState, ITEM_ODD_MUSHROOM);
            // Trigger the reward now
            Flags_SetItemGetInf(ITEMGETINF_30);
            *should = false;
            break;
        }
        case VB_TRADE_ODD_POTION: {
            EnKo* enKo = static_cast<EnKo*>(optionalArg);
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
            EnKz* enKz = static_cast<EnKz*>(optionalArg);
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
            EnCow* enCow = static_cast<EnCow*>(optionalArg);
            // If this is a cow we have to move, then move it now.
            EnCow_MoveForRandomizer(enCow, gPlayState);
            break;
        }
        case VB_BUSINESS_SCRUB_DESPAWN: {
            EnShopnuts* enShopnuts = static_cast<EnShopnuts*>(optionalArg);
            s16 respawnData = gSaveContext.respawn[RESPAWN_MODE_RETURN].data & ((1 << 8) - 1);
            ScrubIdentity scrubIdentity = OTRGlobals::Instance->gRandomizer->IdentifyScrub(gPlayState->sceneNum, enShopnuts->actor.params, respawnData);

            if (scrubIdentity.isShuffled) {
                *should = Flags_GetRandomizerInf(scrubIdentity.randomizerInf);
            }
            break;
        }
        case VB_GIVE_ITEM_FROM_BUSINESS_SCRUB: {
            EnDns* enDns = static_cast<EnDns*>(optionalArg);
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
            EnMk* enMk = static_cast<EnMk*>(optionalArg);
            *should &= enMk->swimFlag == 0;
            break;
        }
        case VB_OFFER_BLUE_POTION: {
            // Always offer blue potion when adult trade is off
            *should |= RAND_GET_OPTION(RSK_SHUFFLE_ADULT_TRADE) == RO_GENERIC_OFF;
            break;
        }
        case VB_NEED_BOTTLE_FOR_GRANNYS_ITEM: {
            // Allow buying the rando item regardless of having a bottle
            *should &= !EnDs_RandoCanGetGrannyItem();
            break;
        }
        case VB_GIVE_ITEM_FROM_SHOOTING_GALLERY: {
            EnSyatekiMan* enSyatekiMan = static_cast<EnSyatekiMan*>(optionalArg);
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
            EnSth* enSth = static_cast<EnSth*>(optionalArg);
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
            EnSkj* enSkj = static_cast<EnSkj*>(optionalArg);
            Flags_SetItemGetInf(ITEMGETINF_17);
            enSkj->actionFunc = (EnSkjActionFunc)EnSkj_CleanupOcarinaGame;
            *should = false;
            break;
        }
        case VB_GIVE_ITEM_FROM_LOST_DOG: {
            EnHy* enHy = static_cast<EnHy*>(optionalArg);
            Flags_SetInfTable(INFTABLE_191);
            gSaveContext.dogParams = 0;
            gSaveContext.dogIsLost = false;
            enHy->actionFunc = func_80A7127C;
            *should = false;
            break;
        }
        case VB_GIVE_ITEM_FROM_BOMBCHU_BOWLING: {
            EnBomBowlPit* enBomBowlPit = static_cast<EnBomBowlPit*>(optionalArg);
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
            s16 item = *static_cast<s16*>(optionalArg);
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
    }
}

void RandomizerOnSceneInitHandler(int16_t sceneNum) {
    // Treasure Chest Game
    // todo: for now we're just unsetting all of them, we will
    //       probably need to do something different when we implement shuffle
    if (sceneNum == SCENE_TREASURE_BOX_SHOP) {
        Flags_UnsetRandomizerInf(RAND_INF_MARKET_TREASURE_CHEST_GAME_ITEM_1);
        Rando::Context::GetInstance()->GetItemLocation(RC_MARKET_TREASURE_CHEST_GAME_ITEM_1)->MarkAsNotObtained();
        Flags_UnsetRandomizerInf(RAND_INF_MARKET_TREASURE_CHEST_GAME_ITEM_2);
        Rando::Context::GetInstance()->GetItemLocation(RC_MARKET_TREASURE_CHEST_GAME_ITEM_2)->MarkAsNotObtained();
        Flags_UnsetRandomizerInf(RAND_INF_MARKET_TREASURE_CHEST_GAME_ITEM_3);
        Rando::Context::GetInstance()->GetItemLocation(RC_MARKET_TREASURE_CHEST_GAME_ITEM_3)->MarkAsNotObtained();
        Flags_UnsetRandomizerInf(RAND_INF_MARKET_TREASURE_CHEST_GAME_ITEM_4);
        Rando::Context::GetInstance()->GetItemLocation(RC_MARKET_TREASURE_CHEST_GAME_ITEM_4)->MarkAsNotObtained();
        Flags_UnsetRandomizerInf(RAND_INF_MARKET_TREASURE_CHEST_GAME_ITEM_5);
        Rando::Context::GetInstance()->GetItemLocation(RC_MARKET_TREASURE_CHEST_GAME_ITEM_5)->MarkAsNotObtained();
        Flags_UnsetRandomizerInf(RAND_INF_MARKET_TREASURE_CHEST_GAME_KEY_1);
        Rando::Context::GetInstance()->GetItemLocation(RC_MARKET_TREASURE_CHEST_GAME_KEY_1)->MarkAsNotObtained();
        Flags_UnsetRandomizerInf(RAND_INF_MARKET_TREASURE_CHEST_GAME_KEY_2);
        Rando::Context::GetInstance()->GetItemLocation(RC_MARKET_TREASURE_CHEST_GAME_KEY_2)->MarkAsNotObtained();
        Flags_UnsetRandomizerInf(RAND_INF_MARKET_TREASURE_CHEST_GAME_KEY_3);
        Rando::Context::GetInstance()->GetItemLocation(RC_MARKET_TREASURE_CHEST_GAME_KEY_3)->MarkAsNotObtained();
        Flags_UnsetRandomizerInf(RAND_INF_MARKET_TREASURE_CHEST_GAME_KEY_4);
        Rando::Context::GetInstance()->GetItemLocation(RC_MARKET_TREASURE_CHEST_GAME_KEY_4)->MarkAsNotObtained();
        Flags_UnsetRandomizerInf(RAND_INF_MARKET_TREASURE_CHEST_GAME_KEY_5);
        Rando::Context::GetInstance()->GetItemLocation(RC_MARKET_TREASURE_CHEST_GAME_KEY_5)->MarkAsNotObtained();
    }

    // LACs & Prelude checks
    static uint32_t updateHook = 0;

    if (updateHook) {
        GameInteractor::Instance->UnregisterGameHook<GameInteractor::OnPlayerUpdate>(updateHook);
        updateHook = 0;
    }

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

        if (!Flags_GetEventChkInf(EVENTCHKINF_RETURNED_TO_TEMPLE_OF_TIME_WITH_ALL_MEDALLIONS) && MeetsLACSRequirements()) {
            Flags_SetEventChkInf(EVENTCHKINF_RETURNED_TO_TEMPLE_OF_TIME_WITH_ALL_MEDALLIONS);
        }

        if (
            Flags_GetEventChkInf(EVENTCHKINF_LEARNED_PRELUDE_OF_LIGHT) &&
            Flags_GetEventChkInf(EVENTCHKINF_RETURNED_TO_TEMPLE_OF_TIME_WITH_ALL_MEDALLIONS)
        ) {
            GameInteractor::Instance->UnregisterGameHook<GameInteractor::OnPlayerUpdate>(updateHook);
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

    if (actor->id == ACTOR_ITEM_B_HEART) {
        ItemBHeart* itemBHeart = static_cast<ItemBHeart*>(actorRef);
        RandomizerCheck rc = OTRGlobals::Instance->gRandomizer->GetCheckFromActor(itemBHeart->actor.id, gPlayState->sceneNum, itemBHeart->actor.params);
        if (rc != RC_UNKNOWN_CHECK) {
            itemBHeart->sohItemEntry = Rando::Context::GetInstance()->GetFinalGIEntry(rc, true, (GetItemID)Rando::StaticData::GetLocation(rc)->GetVanillaItem());
            itemBHeart->actor.draw = (ActorFunc)ItemBHeart_DrawRandomizedItem;
            itemBHeart->actor.update = (ActorFunc)ItemBHeart_UpdateRandomizedItem;
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
                rc = RC_MARKET_BOMBCHU_BOWLING_BOMBCHUS;
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

        onFlagSetHook = 0;
        onSceneFlagSetHook = 0;
        onPlayerUpdateForRCQueueHook = 0;
        onPlayerUpdateForItemQueueHook = 0;
        onItemReceiveHook = 0;
        onVanillaBehaviorHook = 0;
        onSceneInitHook = 0;
        onActorInitHook = 0;

        if (!IS_RANDO) return;

        onFlagSetHook = GameInteractor::Instance->RegisterGameHook<GameInteractor::OnFlagSet>(RandomizerOnFlagSetHandler);
        onSceneFlagSetHook = GameInteractor::Instance->RegisterGameHook<GameInteractor::OnSceneFlagSet>(RandomizerOnSceneFlagSetHandler);
        onPlayerUpdateForRCQueueHook = GameInteractor::Instance->RegisterGameHook<GameInteractor::OnPlayerUpdate>(RandomizerOnPlayerUpdateForRCQueueHandler);
        onPlayerUpdateForItemQueueHook = GameInteractor::Instance->RegisterGameHook<GameInteractor::OnPlayerUpdate>(RandomizerOnPlayerUpdateForItemQueueHandler);
        onItemReceiveHook = GameInteractor::Instance->RegisterGameHook<GameInteractor::OnItemReceive>(RandomizerOnItemReceiveHandler);
        onVanillaBehaviorHook = GameInteractor::Instance->RegisterGameHook<GameInteractor::OnVanillaBehavior>(RandomizerOnVanillaBehaviorHandler);
        onSceneInitHook = GameInteractor::Instance->RegisterGameHook<GameInteractor::OnSceneInit>(RandomizerOnSceneInitHandler);
        onActorInitHook = GameInteractor::Instance->RegisterGameHook<GameInteractor::OnActorInit>(RandomizerOnActorInitHandler);
    });
}
