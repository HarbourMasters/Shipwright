#include "soh/Enhancements/game-interactor/GameInteractor_Hooks.h"
#include "soh/ShipInit.hpp"
#include "soh/Enhancements/randomizer/3drando/random.hpp"
#include "soh/Notification/Notification.h"
#include "soh/OTRGlobals.h"
#include "soh/SohGui/ImGuiUtils.h"
#include "soh/SaveManager.h"

extern "C" {
#include "variables.h"
#include "functions.h"
#include "macros.h"
extern PlayState* gPlayState;
GetItemEntry ItemTable_RetrieveEntry(s16 modIndex, s16 getItemID);
GetItemID RetrieveGetItemIDFromItemID(ItemID itemID);
RandomizerGet RetrieveRandomizerGetFromItemID(ItemID itemID);
}

#define CVAR_EXTRA_TRAPS_NAME CVAR_ENHANCEMENT("ExtraTraps.Enabled")
#define CVAR_EXTRA_TRAPS_DEFAULT 0
#define CVAR_EXTRA_TRAPS_VALUE CVarGetInteger(CVAR_EXTRA_TRAPS_NAME, CVAR_EXTRA_TRAPS_DEFAULT)

typedef enum {
    ADD_ICE_TRAP,
    ADD_BURN_TRAP,
    ADD_SHOCK_TRAP,
    ADD_KNOCK_TRAP,
    ADD_SPEED_TRAP,
    ADD_BOMB_TRAP,
    ADD_VOID_TRAP,
    ADD_AMMO_TRAP,
    ADD_KILL_TRAP,
    ADD_TELEPORT_TRAP,
    ADD_POCKET_TRAP,
    ADD_PERMADEATH_TRAP,
    ADD_TRAP_MAX
} AltTrapType;

static AltTrapType roll = ADD_TRAP_MAX;
static int statusTimer = -1;
static int eventTimer = -1;
static int permaDeathTimer = -1;
bool shouldFileDelete = false;

const char* altTrapTypeCvars[] = {
    CVAR_ENHANCEMENT("ExtraTraps.Ice"),    CVAR_ENHANCEMENT("ExtraTraps.Burn"),
    CVAR_ENHANCEMENT("ExtraTraps.Shock"),  CVAR_ENHANCEMENT("ExtraTraps.Knockback"),
    CVAR_ENHANCEMENT("ExtraTraps.Speed"),  CVAR_ENHANCEMENT("ExtraTraps.Bomb"),
    CVAR_ENHANCEMENT("ExtraTraps.Void"),   CVAR_ENHANCEMENT("ExtraTraps.Ammo"),
    CVAR_ENHANCEMENT("ExtraTraps.Kill"),   CVAR_ENHANCEMENT("ExtraTraps.Teleport"),
    CVAR_ENHANCEMENT("ExtraTraps.Pocket"), CVAR_ENHANCEMENT("ExtraTraps.Permadeath"),
};

static std::unordered_map<ItemID, QuestItem> itemToQuestMap = {
    { ITEM_MEDALLION_FOREST, QUEST_MEDALLION_FOREST },
    { ITEM_MEDALLION_FIRE, QUEST_MEDALLION_FIRE },
    { ITEM_MEDALLION_WATER, QUEST_MEDALLION_WATER },
    { ITEM_MEDALLION_SPIRIT, QUEST_MEDALLION_SPIRIT },
    { ITEM_MEDALLION_SHADOW, QUEST_MEDALLION_SHADOW },
    { ITEM_MEDALLION_LIGHT, QUEST_MEDALLION_LIGHT },
    { ITEM_SONG_MINUET, QUEST_SONG_MINUET },
    { ITEM_SONG_BOLERO, QUEST_SONG_BOLERO },
    { ITEM_SONG_SERENADE, QUEST_SONG_SERENADE },
    { ITEM_SONG_REQUIEM, QUEST_SONG_REQUIEM },
    { ITEM_SONG_NOCTURNE, QUEST_SONG_NOCTURNE },
    { ITEM_SONG_PRELUDE, QUEST_SONG_PRELUDE },
    { ITEM_SONG_LULLABY, QUEST_SONG_LULLABY },
    { ITEM_SONG_EPONA, QUEST_SONG_EPONA },
    { ITEM_SONG_SARIA, QUEST_SONG_SARIA },
    { ITEM_SONG_SUN, QUEST_SONG_SUN },
    { ITEM_SONG_TIME, QUEST_SONG_TIME },
    { ITEM_SONG_STORMS, QUEST_SONG_STORMS },
    { ITEM_KOKIRI_EMERALD, QUEST_KOKIRI_EMERALD },
    { ITEM_GORON_RUBY, QUEST_GORON_RUBY },
    { ITEM_ZORA_SAPPHIRE, QUEST_ZORA_SAPPHIRE },
    { ITEM_GERUDO_CARD, QUEST_GERUDO_CARD },
};

std::vector<AltTrapType> getEnabledAddTraps() {
    std::vector<AltTrapType> enabledAddTraps;
    for (int i = 0; i < ADD_TRAP_MAX; i++) {
        if (CVarGetInteger(altTrapTypeCvars[i], 0)) {
            if (gSaveContext.equips.buttonItems[0] == ITEM_FISHING_POLE &&
                (i == ADD_VOID_TRAP || i == ADD_TELEPORT_TRAP)) {
                continue; // don't add void or teleport if you're holding the fishing pole, as this causes issues
            }
            enabledAddTraps.push_back(static_cast<AltTrapType>(i));
        }
    }
    if (enabledAddTraps.size() == 0) {
        enabledAddTraps.push_back(ADD_ICE_TRAP);
    }
    return enabledAddTraps;
};

void TriggerVoidOut() {
    Player* player = GET_PLAYER(gPlayState);

    Audio_PlaySoundGeneral(NA_SE_EN_BUBLE_LAUGH, &gSfxDefaultPos, 4, &gSfxDefaultFreqAndVolScale,
                           &gSfxDefaultFreqAndVolScale, &gSfxDefaultReverb);

    // Hyrule Castle: Very likely to fall through floor, so we force a specific entrance
    if (gPlayState->sceneNum == SCENE_HYRULE_CASTLE || gPlayState->sceneNum == SCENE_OUTSIDE_GANONS_CASTLE) {
        gPlayState->nextEntranceIndex = ENTR_CASTLE_GROUNDS_SOUTH_EXIT;
    } else {
        gSaveContext.respawnFlag = 1;
        gPlayState->nextEntranceIndex = gSaveContext.entranceIndex;

        // Preserve the player's position and orientation
        gSaveContext.respawn[RESPAWN_MODE_DOWN].entranceIndex = gPlayState->nextEntranceIndex;
        gSaveContext.respawn[RESPAWN_MODE_DOWN].roomIndex = gPlayState->roomCtx.curRoom.num;
        gSaveContext.respawn[RESPAWN_MODE_DOWN].pos = player->actor.world.pos;
        gSaveContext.respawn[RESPAWN_MODE_DOWN].yaw = player->actor.shape.rot.y;

        if (gPlayState->roomCtx.curRoom.behaviorType2 < 4) {
            gSaveContext.respawn[RESPAWN_MODE_DOWN].playerParams = 0x0DFF;
        } else {
            // Scenes with static backgrounds use a special camera we need to preserve
            Camera* camera = GET_ACTIVE_CAM(gPlayState);
            s16 camId = camera->camDataIdx;
            gSaveContext.respawn[RESPAWN_MODE_DOWN].playerParams = 0x0D00 | camId;
        }
    }

    gPlayState->transitionTrigger = TRANS_TRIGGER_START;
    gPlayState->transitionType = TRANS_TYPE_INSTANT;
    gSaveContext.nextTransitionType = TRANS_TYPE_FADE_BLACK_FAST;
}

void ExecutePocketTrap() {
    std::vector<uint32_t> currentLoadout;

    for (auto& items : itemMapping) {
        if (INV_CONTENT(items.second.id) == items.second.id) {
            currentLoadout.push_back(items.second.id);
            continue;
        }
        auto questItem = itemToQuestMap.find(
            (ItemID)items.second.id); // std::find(itemToQuestMap.begin(), itemToQuestMap.end(), items.second.id);
        if (questItem == itemToQuestMap.end()) {
            continue;
        }
        if (CHECK_QUEST_ITEM(questItem->second)) {
            currentLoadout.push_back(items.second.id);
            continue;
        }
    }

    if (currentLoadout.size() == 0) {
        GameInteractor::RawAction::FreezePlayer();
        return;
    }

    RandomizerGet rgItem = RG_NONE;

    uint32_t roll = Random(0, currentLoadout.size() - 1);
    if (currentLoadout[roll] >= ITEM_SONG_MINUET) {
        rgItem = RetrieveRandomizerGetFromItemID((ItemID)currentLoadout[roll]);
    } else {
        GetItemID getItemId = RetrieveGetItemIDFromItemID((ItemID)currentLoadout[roll]);
        for (auto& randoGet : Rando::StaticData::GetItemTable()) {
            if (randoGet.GetItemID() == getItemId) {
                rgItem = randoGet.GetRandomizerGet();
                break;
            }
        }
    }

    if (rgItem == RG_NONE) {
        GameInteractor::RawAction::FreezePlayer();
        return;
    }

    for (auto& check : Rando::StaticData::GetLocationTable()) {
        if (Rando::Context::GetInstance()->GetItemLocation(check.GetRandomizerCheck())->GetPlacedRandomizerGet() ==
            rgItem) {
            if (check.GetRandomizerCheck() == RC_UNKNOWN_CHECK || check.GetRandomizerCheck() == RC_LINKS_POCKET) {
                GameInteractor::RawAction::FreezePlayer();
                return;
            }
            switch (check.GetActorID()) {
                case ACTOR_EN_BOX:
                    GameInteractor::RawAction::UnsetSceneFlag(check.GetScene(), FLAG_SCENE_TREASURE,
                                                              check.GetActorParams() & 0x1F);
                    break;
                default:
                    if (check.GetCollectionCheck().type != SPOILER_CHK_NONE) {
                        switch (check.GetCollectionCheck().type) {
                            case SPOILER_CHK_ITEM_GET_INF:
                                Flags_UnsetItemGetInf(check.GetCollectionCheck().flag);
                                break;
                            case SPOILER_CHK_RANDOMIZER_INF:
                                Flags_UnsetRandomizerInf((RandomizerInf)check.GetCollectionCheck().flag);
                                break;
                            case SPOILER_CHK_EVENT_CHK_INF:
                                Flags_UnsetEventInf(check.GetCollectionCheck().flag);
                                break;
                            case SPOILER_CHK_CHEST:
                                GameInteractor::RawAction::UnsetSceneFlag(check.GetScene(), FLAG_SCENE_TREASURE,
                                                                          check.GetCollectionCheck().flag);
                                break;
                            case SPOILER_CHK_COLLECTABLE:
                                GameInteractor::RawAction::UnsetSceneFlag(check.GetScene(), FLAG_SCENE_COLLECTIBLE,
                                                                          check.GetCollectionCheck().flag);
                                break;
                            case SPOILER_CHK_INF_TABLE:
                                Flags_UnsetInfTable(check.GetCollectionCheck().flag);
                                break;
                            default:
                                GameInteractor::RawAction::FreezePlayer();
                                return;
                        }
                    }
                    break;
            }

            Rando::Context::GetInstance()->GetItemLocation(check.GetRandomizerCheck())->SetCheckStatus(RCSHOW_SEEN);
            INV_CONTENT(currentLoadout[roll]) = ITEM_NONE;
            break;
        }
    }

    Notification::Emit({ .itemIcon = (const char*)gItemIcons[currentLoadout[roll]],
                         .prefix = "You've lost your",
                         .prefixColor = UIWidgets::ColorValues.at(UIWidgets::Colors::White),
                         .message = Rando::StaticData::RetrieveItem(rgItem).GetName().english.c_str(),
                         .messageColor = UIWidgets::ColorValues.at(UIWidgets::Colors::Green),
                         .suffix = ". I remember seeing it somewhere...",
                         .suffixColor = UIWidgets::ColorValues.at(UIWidgets::Colors::White) });

    TriggerVoidOut();
}

static void RollRandomTrap(uint32_t seed) {
    uint32_t finalSeed = seed + (IS_RANDO ? Rando::Context::GetInstance()->GetSeed()
                                          : static_cast<uint32_t>(gSaveContext.ship.stats.fileCreatedAt));
    Random_Init(finalSeed);
    // roll = ADD_PERMADEATH_TRAP;
    roll = RandomElement(getEnabledAddTraps());
    switch (roll) {
        case ADD_ICE_TRAP:
            GameInteractor::RawAction::FreezePlayer();
            break;
        case ADD_BURN_TRAP:
            GameInteractor::RawAction::BurnPlayer();
            break;
        case ADD_SHOCK_TRAP:
            GameInteractor::RawAction::ElectrocutePlayer();
            break;
        case ADD_KNOCK_TRAP:
            eventTimer = 3;
            break;
        case ADD_SPEED_TRAP:
            Audio_PlaySoundGeneral(NA_SE_VO_KZ_MOVE, &gSfxDefaultPos, 4, &gSfxDefaultFreqAndVolScale,
                                   &gSfxDefaultFreqAndVolScale, &gSfxDefaultReverb);
            GameInteractor::State::MovementSpeedMultiplier = 0.5f;
            statusTimer = 200;
            Notification::Emit({ .message = "Speed Decreased!" });
            break;
        case ADD_BOMB_TRAP:
            eventTimer = 3;
            break;
        case ADD_VOID_TRAP:
            Audio_PlaySoundGeneral(NA_SE_EN_GANON_LAUGH, &gSfxDefaultPos, 4, &gSfxDefaultFreqAndVolScale,
                                   &gSfxDefaultFreqAndVolScale, &gSfxDefaultReverb);
            eventTimer = 3;
            break;
        case ADD_AMMO_TRAP:
            eventTimer = 3;
            Notification::Emit({ .message = "Ammo Halved!" });
            break;
        case ADD_KILL_TRAP:
            GameInteractor::RawAction::SetPlayerHealth(0);
            break;
        case ADD_TELEPORT_TRAP:
            eventTimer = 3;
            break;
        case ADD_POCKET_TRAP:
            ExecutePocketTrap();
            break;
        case ADD_PERMADEATH_TRAP:
            permaDeathTimer = 180;
            shouldFileDelete = true;
            Notification::Emit({ .itemIcon = (const char*)gItemIcons[ITEM_BOMB],
                                 .message = "Collect a Check or Perma Death executes in ",
                                 .messageColor = UIWidgets::ColorValues.at(UIWidgets::Colors::White),
                                 .suffix = "60 seconds.",
                                 .suffixColor = UIWidgets::ColorValues.at(UIWidgets::Colors::Red) });
            break;
        default:
            break;
    }
}

static void OnPlayerUpdate() {
    Player* player = GET_PLAYER(gPlayState);
    if (statusTimer == 0) {
        GameInteractor::State::MovementSpeedMultiplier = 1.0f;
    }
    if (eventTimer == 0) {
        switch (roll) {
            case ADD_KNOCK_TRAP:
                GameInteractor::RawAction::KnockbackPlayer(1);
                break;
            case ADD_BOMB_TRAP:
                GameInteractor::RawAction::SpawnActor(ACTOR_EN_BOM, 1);
                break;
            case ADD_VOID_TRAP:
                Play_TriggerRespawn(gPlayState);
                break;
            case ADD_AMMO_TRAP:
                AMMO(ITEM_STICK) = static_cast<int8_t>(floor(AMMO(ITEM_STICK) * 0.5f));
                AMMO(ITEM_NUT) = static_cast<int8_t>(floor(AMMO(ITEM_NUT) * 0.5f));
                AMMO(ITEM_SLINGSHOT) = static_cast<int8_t>(floor(AMMO(ITEM_SLINGSHOT) * 0.5f));
                AMMO(ITEM_BOW) = static_cast<int8_t>(floor(AMMO(ITEM_BOW) * 0.5f));
                AMMO(ITEM_BOMB) = static_cast<int8_t>(floor(AMMO(ITEM_BOMB) * 0.5f));
                AMMO(ITEM_BOMBCHU) = static_cast<int8_t>(floor(AMMO(ITEM_BOMBCHU) * 0.5f));
                Audio_PlaySoundGeneral(NA_SE_VO_FR_SMILE_0, &gSfxDefaultPos, 4, &gSfxDefaultFreqAndVolScale,
                                       &gSfxDefaultFreqAndVolScale, &gSfxDefaultReverb);
                break;
            case ADD_TELEPORT_TRAP: {
                int entrance;
                int index = Random(0, 7);
                switch (index) {
                    case 0:
                        entrance = GI_TP_DEST_SERENADE;
                        break;
                    case 1:
                        entrance = GI_TP_DEST_REQUIEM;
                        break;
                    case 2:
                        entrance = GI_TP_DEST_BOLERO;
                        break;
                    case 3:
                        entrance = GI_TP_DEST_MINUET;
                        break;
                    case 4:
                        entrance = GI_TP_DEST_NOCTURNE;
                        break;
                    case 5:
                        entrance = GI_TP_DEST_PRELUDE;
                        break;
                    default:
                        entrance = GI_TP_DEST_LINKSHOUSE;
                        break;
                }
                GameInteractor::RawAction::TeleportPlayer(entrance);
                break;
            }
            default:
                break;
        }
    }
    if (permaDeathTimer == 0) {
        if (shouldFileDelete) {
            SaveManager::Instance->DeleteZeldaFile(gSaveContext.fileNum);
            std::reinterpret_pointer_cast<Ship::ConsoleWindow>(
                Ship::Context::GetInstance()->GetWindow()->GetGui()->GetGuiWindow("Console"))
                ->Dispatch("reset");
        }
    }
    if (permaDeathTimer >= 0) {
        permaDeathTimer--;
    }
    if (statusTimer >= 0) {
        statusTimer--;
    }
    if (eventTimer >= 0) {
        eventTimer--;
    }
}

void RegisterExtraTraps() {
    COND_HOOK(OnPlayerUpdate, CVAR_EXTRA_TRAPS_VALUE, OnPlayerUpdate);

    COND_VB_SHOULD(VB_SHORT_CIRCUIT_GIVE_ITEM_PROCESS, true, {
        if (!gSaveContext.ship.pendingIceTrapCount) {
            return;
        }

        Player* player = GET_PLAYER(gPlayState);

        *should = true;
        gSaveContext.ship.pendingIceTrapCount--;
        gSaveContext.ship.stats.count[COUNT_ICE_TRAPS]++;
        GameInteractor_ExecuteOnItemReceiveHooks(ItemTable_RetrieveEntry(MOD_RANDOMIZER, RG_ICE_TRAP));
        if (CVAR_EXTRA_TRAPS_VALUE) {
            RollRandomTrap(gPlayState->sceneNum + player->getItemEntry.drawItemId);
        } else {
            GameInteractor::RawAction::FreezePlayer();
        }
    });

    COND_HOOK(OnFlagSet, CVAR_EXTRA_TRAPS_NAME, [](int16_t flagType, int16_t flag) {
        SPDLOG_INFO("Flag Set Here {}", std::to_string(flagType).c_str());
        if (flagType != FLAG_SCENE_CLEAR) {
            shouldFileDelete = false;
        }
    });

    COND_HOOK(OnSceneFlagSet, CVAR_EXTRA_TRAPS_NAME, [](int16_t sceneNum, int16_t flagType, int16_t flag) {
        SPDLOG_INFO("Scene Flag Set Here {}", std::to_string(flagType).c_str());
        if (flagType != FLAG_SCENE_CLEAR) {
            shouldFileDelete = false;
        }
    });
}

static RegisterShipInitFunc initFunc(RegisterExtraTraps, { CVAR_EXTRA_TRAPS_NAME });
