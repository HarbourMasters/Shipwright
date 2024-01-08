#include <libultraship/bridge.h>
#include "soh/OTRGlobals.h"
#include "soh/Enhancements/enhancementTypes.h"
#include "soh/Enhancements/custom-message/CustomMessageTypes.h"
#include "soh/Enhancements/randomizer/randomizerTypes.h"
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
#include "adult_trade_shuffle.h"
extern SaveContext gSaveContext;
extern PlayState* gPlayState;
}

#define RAND_GET_OPTION(option) Rando::Context::GetInstance()->GetOption(option).GetSelectedOptionIndex()

RandomizerCheck GetRandomizerCheckFromFlag(int16_t flagType, int16_t flag) {
    for (auto& loc : Rando::StaticData::GetLocationTable()) {
        if (loc.GetCollectionCheck().flag == flag && (
                (flagType == FLAG_EVENT_CHECK_INF && loc.GetCollectionCheck().type == SPOILER_CHK_EVENT_CHK_INF) ||
                (flagType == FLAG_ITEM_GET_INF && loc.GetCollectionCheck().type == SPOILER_CHK_ITEM_GET_INF) ||
                (flagType == FLAG_RANDOMIZER_INF && loc.GetCollectionCheck().type == SPOILER_CHK_RANDOMIZER_INF)
            ) ||
            (loc.GetActorParams() == flag && flagType == FLAG_GS_TOKEN && loc.GetCollectionCheck().type == SPOILER_CHK_GOLD_SKULLTULA)
        ) {
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
        )) {
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

// Todo Move this to randomizer context, clear it out on save load etc
static std::queue<RandomizerCheck> randomizerQueuedChecks;
static RandomizerCheck randomizerQueuedCheck = RC_UNKNOWN_CHECK;
static GetItemEntry randomizerQueuedItemEntry = GET_ITEM_NONE;

void RandomizerOnFlagSetHandler(int16_t flagType, int16_t flag) {
    RandomizerCheck rc = GetRandomizerCheckFromFlag(flagType, flag);
    if (rc == RC_UNKNOWN_CHECK) return;

    auto loc = Rando::Context::GetInstance()->GetItemLocation(rc);
    if (loc == nullptr || loc->HasObtained()) return;

    SPDLOG_INFO("Queuing RC: {}", rc);
    randomizerQueuedChecks.push(rc);
}

void RandomizerOnSceneFlagSetHandler(int16_t sceneNum, int16_t flagType, int16_t flag) {
    RandomizerCheck rc = GetRandomizerCheckFromSceneFlag(sceneNum, flagType, flag);
    if (rc == RC_UNKNOWN_CHECK) return;

    auto loc = Rando::Context::GetInstance()->GetItemLocation(rc);
    if (loc == nullptr || loc->HasObtained()) return;

    SPDLOG_INFO("Queuing RC: {}", rc);
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
        SPDLOG_INFO("RC {} already obtained, skipping", rc);
    } else {
        randomizerQueuedCheck = rc;
        randomizerQueuedItemEntry = getItemEntry;
        SPDLOG_INFO("Queueing Item mod {} item {} from RC {}", getItemEntry.modIndex, getItemEntry.itemId, rc);
        if (
            // Skipping ItemGet animation incompatible with checks that require closing a text box to finish
            rc != RC_HF_OCARINA_OF_TIME_ITEM &&
            rc != RC_SPIRIT_TEMPLE_SILVER_GAUNTLETS_CHEST &&
            // Always show ItemGet animation for ice traps
            !(getItemEntry.modIndex == MOD_RANDOMIZER && getItemEntry.getItemId == RG_ICE_TRAP) &&
            (
                CVarGetInteger("gTimeSavers.SkipGetItemAnimation", SGIA_DISABLED) == SGIA_ALL ||
                (
                    CVarGetInteger("gTimeSavers.SkipGetItemAnimation", SGIA_DISABLED) == SGIA_JUNK &&
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

    SPDLOG_INFO("Attempting to give Item mod {} item {} from RC {}", randomizerQueuedItemEntry.modIndex, randomizerQueuedItemEntry.itemId, randomizerQueuedCheck);
    GiveItemEntryWithoutActor(gPlayState, randomizerQueuedItemEntry);
    if (player->stateFlags1 & PLAYER_STATE1_IN_WATER) {
        // Allow the player to receive the item while swimming
        player->stateFlags2 |= PLAYER_STATE2_UNDERWATER;
        func_8083E5A8(player, gPlayState);
    }
}

void RandomizerOnItemReceiveHandler(GetItemEntry receivedItemEntry) {
    if (randomizerQueuedCheck == RC_UNKNOWN_CHECK) return;

    auto loc = Rando::Context::GetInstance()->GetItemLocation(randomizerQueuedCheck);
    if (randomizerQueuedItemEntry.modIndex == receivedItemEntry.modIndex && randomizerQueuedItemEntry.itemId == receivedItemEntry.itemId) {
        SPDLOG_INFO("Item received mod {} item {} from RC {}", receivedItemEntry.modIndex, receivedItemEntry.itemId, randomizerQueuedCheck);
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

    if (loc->GetRandomizerCheck() == RC_SPIRIT_TEMPLE_SILVER_GAUNTLETS_CHEST && !CVarGetInteger("gTimeSavers.SkipCutscene.Story", 0)) {
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

void EnItem00_DrawRandomizedItem(EnItem00* enItem00, PlayState* play) {
    f32 mtxScale = CVarGetFloat("gTimeSavers.SkipGetItemAnimationScale", 10.0f);
    Matrix_Scale(mtxScale, mtxScale, mtxScale, MTXMODE_APPLY);
    EnItem00_CustomItemsParticles(&enItem00->actor, play, enItem00->itemEntry);
    GetItemEntry_Draw(play, enItem00->itemEntry);
}

void ItemBHeart_DrawRandomizedItem(ItemBHeart* itemBHeart, PlayState* play) {
    EnItem00_CustomItemsParticles(&itemBHeart->actor, play, itemBHeart->sohItemEntry);
    GetItemEntry_Draw(play, itemBHeart->sohItemEntry);
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

void RandomizerOnVanillaBehaviorHandler(GIVanillaBehavior id, bool* should, void* optionalArg) {
    switch (id) {
        case GI_VB_GIVE_ITEM_FROM_CHEST: {
            Player* player = GET_PLAYER(gPlayState);
            player->unk_850 = 1;
            player->getItemId = GI_NONE;
            player->getItemEntry = GetItemEntry(GET_ITEM_NONE);
            *should = false;
            break;
        }
        case GI_VB_PLAY_NABOORU_CAPTURED_CS:
            // This behavior is replicated for randomizer in RandomizerOnItemReceiveHandler
            *should = false;
            break;
        case GI_VB_SHIEK_PREPARE_TO_GIVE_SERENADE_OF_WATER: {
            *should = !Flags_GetEventChkInf(EVENTCHKINF_LEARNED_SERENADE_OF_WATER) && !Flags_GetTreasure(gPlayState, 0x2);
            break;
        }
        case GI_VB_BE_ELIGIBLE_FOR_SERENADE_OF_WATER:
            *should = !Flags_GetEventChkInf(EVENTCHKINF_LEARNED_SERENADE_OF_WATER) && Flags_GetTreasure(gPlayState, 0x2);
            break;
        case GI_VB_BE_ELIGIBLE_FOR_PRELUDE_OF_LIGHT:
            *should = !Flags_GetEventChkInf(EVENTCHKINF_LEARNED_PRELUDE_OF_LIGHT) && CHECK_QUEST_ITEM(QUEST_MEDALLION_FOREST);
            break;
        case GI_VB_MOVE_MIDO_IN_KOKIRI_FOREST:
            if (RAND_GET_OPTION(RSK_FOREST) == RO_FOREST_OPEN) {
                *should = true;
            }
            break;
        case GI_VB_MIDO_CONSIDER_DEKU_TREE_DEAD:
            *should = Flags_GetEventChkInf(EVENTCHKINF_OBTAINED_KOKIRI_EMERALD_DEKU_TREE_DEAD);
            break;
        case GI_VB_OPEN_KOKIRI_FOREST:
            *should = Flags_GetEventChkInf(EVENTCHKINF_OBTAINED_KOKIRI_EMERALD_DEKU_TREE_DEAD) || RAND_GET_OPTION(RSK_FOREST) != RO_FOREST_CLOSED;
            break;
        case GI_VB_BE_ELIGIBLE_FOR_DARUNIAS_JOY_REWARD:
            *should = !Flags_GetRandomizerInf(RAND_INF_DARUNIAS_JOY);
            break;
        case GI_VB_BE_ELIGIBLE_FOR_LIGHT_ARROWS:
            *should = !Flags_GetEventChkInf(EVENTCHKINF_RETURNED_TO_TEMPLE_OF_TIME_WITH_ALL_MEDALLIONS) && MeetsLACSRequirements();
            break;
        case GI_VB_BE_ELIGIBLE_FOR_NOCTURNE_OF_SHADOW:
            *should =
                    !Flags_GetEventChkInf(EVENTCHKINF_BONGO_BONGO_ESCAPED_FROM_WELL) &&
                    LINK_IS_ADULT &&
                    gEntranceTable[((void)0, gSaveContext.entranceIndex)].scene == SCENE_KAKARIKO_VILLAGE &&
                    CHECK_QUEST_ITEM(QUEST_MEDALLION_FOREST) &&
                    CHECK_QUEST_ITEM(QUEST_MEDALLION_FIRE) &&
                    CHECK_QUEST_ITEM(QUEST_MEDALLION_WATER);
            break;
        case GI_VB_BE_ELIGIBLE_FOR_CHILD_ROLLING_GORON_REWARD: {
            // Don't require a bomb bag to get prize in rando
            *should = true;
            break;
        }
        case GI_VB_BE_ELIGIBLE_FOR_MAGIC_BEANS_PURCHASE: {
            if (RAND_GET_OPTION(RSK_SHUFFLE_MAGIC_BEANS)) {
                *should = gSaveContext.rupees >= 60;
            }
            break;
        }
        case GI_VB_GIVE_ITEM_MASTER_SWORD:
            if (RAND_GET_OPTION(RSK_SHUFFLE_MASTER_SWORD)) {
                *should = false;
            } else {
                *should = true;
                Rando::Context::GetInstance()->GetItemLocation(RC_TOT_MASTER_SWORD)->MarkAsObtained();
            }
            break;
        case GI_VB_ITEM00_DESPAWN: {
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
        case GI_VB_MALON_ALREADY_TAUGHT_EPONAS_SONG: {
            *should = Flags_GetRandomizerInf(RAND_INF_LEARNED_EPONA_SONG);
            break;
        }
        case GI_VB_SET_CUCCO_COUNT: {
            EnNiwLady* enNiwLady = static_cast<EnNiwLady*>(optionalArg);
            // Override starting Cucco count using setting value
            enNiwLady->cuccosInPen = 7 - RAND_GET_OPTION(RSK_CUCCO_COUNT);
            *should = false;
            break;
        }
        case GI_VB_KING_ZORA_THANK_CHILD: {
            // Allow turning in Ruto's letter even if you have already rescued her
            if (!Flags_GetEventChkInf(EVENTCHKINF_KING_ZORA_MOVED)) {
                GET_PLAYER(gPlayState)->exchangeItemId = EXCH_ITEM_LETTER_RUTO;
            }
            *should = Flags_GetRandomizerInf(RAND_INF_DUNGEONS_DONE_JABU_JABUS_BELLY);
            break;
        }
        case GI_VB_BE_ABLE_TO_EXCHANGE_RUTOS_LETTER: {
            *should = LINK_IS_CHILD;
            break;
        }
        case GI_VB_KING_ZORA_BE_MOVED: {
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
        case GI_VB_BIGGORON_CONSIDER_SWORD_COLLECTED: {
            *should = Flags_GetRandomizerInf(RAND_INF_ADULT_TRADES_DMT_TRADE_CLAIM_CHECK);
            break;
        }
        case GI_VB_BIGGORON_CONSIDER_TRADE_COMPLETE: {

            *should = INV_CONTENT(ITEM_TRADE_ADULT) == ITEM_CLAIM_CHECK && Flags_GetRandomizerInf(RAND_INF_ADULT_TRADES_DMT_TRADE_CLAIM_CHECK);
            break;
        }
        case GI_VB_GORONS_CONSIDER_FIRE_TEMPLE_FINISHED: {
            *should = Flags_GetEventChkInf(EVENTCHKINF_USED_FIRE_TEMPLE_BLUE_WARP);
            break;
        }
        case GI_VB_GORONS_CONSIDER_DODONGOS_CAVERN_FINISHED: {
            *should = Flags_GetEventChkInf(EVENTCHKINF_USED_DODONGOS_CAVERN_BLUE_WARP);
            break;
        }
        case GI_VB_OVERRIDE_LINK_THE_GORON_DIALOGUE: {
            u16* textId = static_cast<u16*>(optionalArg);
            *should = true;

            // For rando, prioritize opening the doors in GC when Link the goron has been stopped when
            // the doors are not opened, otherwise let him talk about the DMC exit or that gorons are saved
            if (!Flags_GetInfTable(INFTABLE_STOPPED_GORON_LINKS_ROLLING)) {
                *textId = 0x3030;
            } else if (!Flags_GetInfTable(INFTABLE_GORON_CITY_DOORS_UNLOCKED)) {
                *textId = 0x3036;
            } else if (Flags_GetEventChkInf(EVENTCHKINF_USED_FIRE_TEMPLE_BLUE_WARP)) {
                *textId = 0x3041;
            } else {
                *textId = Flags_GetInfTable(INFTABLE_SPOKE_TO_GORON_LINK) ? 0x3038 : 0x3037;
            }
            break;
        }
        case GI_VB_EN_GO2_RESET_AFTER_GET_ITEM: {
            EnGo2* enGo2 = static_cast<EnGo2*>(optionalArg);
            // For randomizer, handle updating the states for the gorons after receiving the item based on
            // the goron type rather then the item being received
            switch (enGo2->actor.params & 0x1F) {
                case GORON_DMT_BIGGORON:
                    // Resolves #1301. unk_13EE is used to set the opacity of the HUD. The trade sequence discussion
                    // with Biggoron sets the HUD to transparent, and it is restored at z_message_PAL:3549, but by
                    // specifically watching for trade sequence items, this leaves it transparent for non-trade sequence
                    // items (in rando) so we fix that here
                    gSaveContext.unk_13EE = 0x32;
                    break;
                case GORON_CITY_LINK:
                    EnGo2_GetItemAnimation(enGo2, gPlayState);
                    break;
                case GORON_CITY_ROLLING_BIG:
                    EnGo2_RollingAnimation(enGo2, gPlayState);
                    enGo2->actionFunc = (EnGo2ActionFunc)EnGo2_GoronRollingBigContinueRolling;
                    break;
                default:
                    enGo2->actionFunc = func_80A46B40;
                    break;
            }
            break;
        }
        case GI_VB_GIVE_ITEM_FROM_ITEM_00: {
            EnItem00* item00 = static_cast<EnItem00*>(optionalArg);
            if (item00->actor.params == ITEM00_SOH_DUMMY) {
                Flags_SetCollectible(gPlayState, item00->collectibleFlag);
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
        case GI_VB_BE_ELIGIBLE_FOR_SARIAS_SONG: {
            *should = !Flags_GetEventChkInf(EVENTCHKINF_LEARNED_SARIAS_SONG);
            break;
        }
        case GI_VB_GIVE_ITEM_FROM_COW: {
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
        case GI_VB_GIVE_ITEM_FROM_GRANNYS_SHOP: {
            if (!EnDs_RandoCanGetGrannyItem()) {
                break;
            }
            // Only setting the inf if we've actually gotten the rando item and not the vanilla blue potion
            Flags_SetRandomizerInf(RAND_INF_MERCHANTS_GRANNYS_SHOP);
            *should = false;
            break;
        }
        case GI_VB_GIVE_ITEM_FROM_THAWING_KING_ZORA: {
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
        case GI_VB_GIVE_ITEM_FROM_ANJU_AS_CHILD: {
            Flags_SetItemGetInf(ITEMGETINF_0C);
            *should = false;
            break;
        }
        case GI_VB_GIVE_ITEM_FROM_ANJU_AS_ADULT: {
            Flags_SetItemGetInf(ITEMGETINF_2C);
            *should = false;
            break;
        }
        case GI_VB_GIVE_ITEM_FROM_ROLLING_GORON_AS_ADULT: {
            EnGo2* enGo2 = static_cast<EnGo2*>(optionalArg);
            *should = false;
            if (!Flags_GetRandomizerInf(RAND_INF_ROLLING_GORON_AS_ADULT)) {
                Flags_SetInfTable(INFTABLE_GORON_CITY_DOORS_UNLOCKED);
                enGo2->interactInfo.talkState = NPC_TALK_STATE_ACTION;
            }
            break;
        }
        case GI_VB_GIVE_ITEM_FROM_CARPET_SALESMAN: {
            *should = RAND_GET_OPTION(RSK_SHUFFLE_MERCHANTS) == RO_SHUFFLE_MERCHANTS_OFF ||
                      // If the rando check has already been awarded, use vanilla behavior.
                      Flags_GetRandomizerInf(RAND_INF_MERCHANTS_CARPET_SALESMAN);
            break;
        }
        case GI_VB_GIVE_ITEM_FROM_MEDIGORON: {
            // fallthrough
        case GI_VB_BE_ELIGIBLE_FOR_GIANTS_KNIFE_PURCHASE:
            if (RAND_GET_OPTION(RSK_SHUFFLE_MERCHANTS) != RO_SHUFFLE_MERCHANTS_OFF &&
                !Flags_GetRandomizerInf(RAND_INF_MERCHANTS_MEDIGORON)) {
                if (id == GI_VB_GIVE_ITEM_FROM_MEDIGORON) {
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
        case GI_VB_GIVE_ITEM_FROM_MAGIC_BEAN_SALESMAN: {
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
        case GI_VB_GIVE_ITEM_FROM_FROGS: {
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
        case GI_VB_TRADE_POCKET_CUCCO: {
            Randomizer_ConsumeAdultTradeItem(gPlayState, ITEM_POCKET_CUCCO);
            // Trigger the reward now
            Flags_SetItemGetInf(ITEMGETINF_2E);
            *should = false;
            break;
        }
        case GI_VB_TRADE_COJIRO: {
            Randomizer_ConsumeAdultTradeItem(gPlayState, ITEM_COJIRO);
            *should = false;
            break;
        }
        case GI_VB_TRADE_ODD_MUSHROOM: {
            Randomizer_ConsumeAdultTradeItem(gPlayState, ITEM_ODD_MUSHROOM);
            // Trigger the reward now
            Flags_SetItemGetInf(ITEMGETINF_30);
            *should = false;
            break;
        }
        case GI_VB_TRADE_ODD_POTION: {
            EnKo* enKo = static_cast<EnKo*>(optionalArg);
            Randomizer_ConsumeAdultTradeItem(gPlayState, ITEM_ODD_POTION);
            // Trigger the reward now
            Flags_SetItemGetInf(ITEMGETINF_31);
            *should = false;
            break;
        }
        case GI_VB_TRADE_SAW: {
            Randomizer_ConsumeAdultTradeItem(gPlayState, ITEM_SAW);
            *should = false;
            break;
        }
        case GI_VB_TRADE_PRESCRIPTION: {
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
        case GI_VB_TRADE_FROG: {
            Randomizer_ConsumeAdultTradeItem(gPlayState, ITEM_FROG);
            *should = false;
            break;
        }
        case GI_VB_TRADE_BROKEN_SWORD: {
            Randomizer_ConsumeAdultTradeItem(gPlayState, ITEM_SWORD_BROKEN);
            *should = false;
            break;
        }
        case GI_VB_TRADE_EYEDROPS: {
            Randomizer_ConsumeAdultTradeItem(gPlayState, ITEM_EYEDROPS);
            *should = false;
            break;
        }
        case GI_VB_DESPAWN_HORSE_RACE_COW: {
            if (!RAND_GET_OPTION(RSK_SHUFFLE_COWS)) {
                break;
            }
            EnCow* enCow = static_cast<EnCow*>(optionalArg);
            // If this is a cow we have to move, then move it now.
            EnCow_MoveForRandomizer(enCow, gPlayState);
            break;
        }
        case GI_VB_BUSINESS_SCRUB_DESPAWN: {
            EnShopnuts* enShopnuts = static_cast<EnShopnuts*>(optionalArg);
            s16 respawnData = gSaveContext.respawn[RESPAWN_MODE_RETURN].data & ((1 << 8) - 1);
            ScrubIdentity scrubIdentity = OTRGlobals::Instance->gRandomizer->IdentifyScrub(gPlayState->sceneNum, enShopnuts->actor.params, respawnData);

            if (scrubIdentity.isShuffled) {
                *should = Flags_GetRandomizerInf(scrubIdentity.randomizerInf);
            }
            break;
        }
        case GI_VB_GIVE_ITEM_FROM_BUSINESS_SCRUB: {
            EnDns* enDns = static_cast<EnDns*>(optionalArg);
            *should = !enDns->sohScrubIdentity.isShuffled;
            break;
        }
        // To explain the logic because Fado and Grog are linked:
        // - If you have Cojiro, then spawn Grog and not Fado.
        // - If you don't have Cojiro but do have Odd Potion, spawn Fado and not Grog.
        // - If you don't have either, spawn Grog if you haven't traded the Odd Mushroom.
        // - If you don't have either but have traded the mushroom, don't spawn either.
        case GI_VB_DESPAWN_GROG: {
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
        case GI_VB_SPAWN_LW_FADO: {
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
        case GI_VB_USE_EYEDROP_DIALOGUE: {
            // Skip eye drop text on rando if Link went in the water, so you can still receive the dive check
            EnMk* enMk = static_cast<EnMk*>(optionalArg);
            *should &= enMk->swimFlag == 0;
            break;
        }
        case GI_VB_OFFER_BLUE_POTION: {
            // Always offer blue potion when adult trade is off
            *should |= RAND_GET_OPTION(RSK_SHUFFLE_ADULT_TRADE) == RO_GENERIC_OFF;
            break;
        }
        case GI_VB_NEED_BOTTLE_FOR_GRANNYS_ITEM: {
            // Allow buying the rando item regardless of having a bottle
            *should &= !EnDs_RandoCanGetGrannyItem();
            break;
        }
        case GI_VB_TRADE_CLAIM_CHECK:
        case GI_VB_TRADE_TIMER_ODD_MUSHROOM:
        case GI_VB_TRADE_TIMER_EYEDROPS:
        case GI_VB_TRADE_TIMER_FROG:
        case GI_VB_ANJU_SET_OBTAINED_TRADE_ITEM:
        case GI_VB_GIVE_ITEM_FROM_ROLLING_GORON_AS_CHILD:
        case GI_VB_GIVE_ITEM_FROM_LAB_DIVE:
        case GI_VB_GIVE_ITEM_SKULL_TOKEN:
        case GI_VB_GIVE_ITEM_FROM_BLUE_WARP:
        case GI_VB_GIVE_ITEM_FAIRY_OCARINA:
        case GI_VB_GIVE_ITEM_WEIRD_EGG:
        case GI_VB_GIVE_ITEM_LIGHT_ARROW:
        case GI_VB_GIVE_ITEM_STRENGTH_1:
        case GI_VB_GIVE_ITEM_ZELDAS_LETTER:
        case GI_VB_GIVE_ITEM_OCARINA_OF_TIME:
        case GI_VB_GIVE_ITEM_KOKIRI_EMERALD:
        case GI_VB_GIVE_ITEM_GORON_RUBY:
        case GI_VB_GIVE_ITEM_ZORA_SAPPHIRE:
        case GI_VB_GIVE_ITEM_LIGHT_MEDALLION:
        case GI_VB_GIVE_ITEM_FOREST_MEDALLION:
        case GI_VB_GIVE_ITEM_FIRE_MEDALLION:
        case GI_VB_GIVE_ITEM_WATER_MEDALLION:
        case GI_VB_GIVE_ITEM_SPIRIT_MEDALLION:
        case GI_VB_GIVE_ITEM_SHADOW_MEDALLION:
            *should = false;
            break;
    }
}

void RandomizerOnSceneInitHandler(int16_t sceneNum) {
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
    func_8002ED80(&enSi->actor, play, 0);
    func_8002EBCC(&enSi->actor, play, 0);
    EnItem00_CustomItemsParticles(&enSi->actor, play, enSi->sohGetItemEntry);
    GetItemEntry_Draw(play, enSi->sohGetItemEntry);
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
