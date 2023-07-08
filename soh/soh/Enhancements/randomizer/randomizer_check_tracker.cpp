
#ifdef __cplusplus
#include "randomizer_check_tracker.h"
#include "randomizer_entrance_tracker.h"
#include "randomizer_item_tracker.h"
#include "randomizerTypes.h"
#include "../../OTRGlobals.h"
#include "../../UIWidgets.hpp"

#include <string>
#include <vector>
#include <set>
#include <libultraship/libultraship.h>
#include "3drando/item_location.hpp"
#include "soh/Enhancements/game-interactor/GameInteractor.h"
#include "randomizerTypes.h"
#include "z64item.h"

extern "C" {
#include "variables.h"
#include "functions.h"
#include "macros.h"
extern PlayState* gPlayState;
}
extern "C" uint32_t ResourceMgr_IsSceneMasterQuest(s16 sceneNum);
extern "C" GetItemEntry ItemTable_RetrieveEntry(s16 modIndex, s16 getItemID);

extern std::vector<ItemTrackerItem> dungeonRewardStones;
extern std::vector<ItemTrackerItem> dungeonRewardMedallions;
extern std::vector<ItemTrackerItem> songItems;
extern std::vector<ItemTrackerItem> equipmentItems;

#define RCO_RAORU { RC_GIFT_FROM_SAGES, RCVORMQ_BOTH, RCTYPE_DUNGEON_REWARD, RCAREA_MARKET, ACTOR_ID_MAX, SCENE_ID_MAX, 0x00, GI_NONE, false, "Gift from Raoru", "Gift from Raoru", true };

using json = nlohmann::json;

void to_json(json& j, const RandomizerCheckTrackerData& rctd) {
    j = json{ rctd.status, rctd.skipped, rctd.hintItem };
}

void from_json(const json& j, RandomizerCheckTrackerData& rctd) {
    rctd.status = j[0];
    rctd.skipped = j[1];
    rctd.hintItem = j[2];
}

bool bypassRandoCheck = true;

namespace CheckTracker {
// persistent during gameplay
bool initialized;
bool doAreaScroll;
bool doInitialize;
bool previousShowHidden = false;
bool hideShopRightChecks = true;

bool checkCollected = false;
int checkLoops = 0;
int checkCounter = 0;
u16 savedFrames = 0;
bool messageCloseCheck = false;
bool pendingSaleCheck = false;
bool transitionCheck = false;

//std::map<RandomizerCheck, RandomizerCheckShow> checkStatusMap;
std::map<RandomizerCheckArea, std::vector<RandomizerCheckObject>> checksByArea;
bool areasFullyChecked[RCAREA_INVALID];
u32 areasSpoiled = 0;
bool showVOrMQ;
s8 areaChecksGotten[32]; //|     "Kokiri Forest (4/9)"
bool optCollapseAll;     // A bool that will collapse all checks once
bool optExpandAll;       // A bool that will expand all checks once
RandomizerCheck lastItemGetCheck = RC_UNKNOWN_CHECK;
RandomizerCheck lastLocationChecked = RC_UNKNOWN_CHECK;
RandomizerCheckArea previousArea = RCAREA_INVALID;
RandomizerCheckArea currentArea = RCAREA_INVALID;
RandomizerCheckArea checkArea = RCAREA_INVALID;
OSContPad* trackerButtonsPressed;
SceneID checkScene = SCENE_ID_MAX;
SceneID lastScene = SCENE_ID_MAX;
SceneID currentScene = SCENE_ID_MAX;
bool newFileCheck = false;
bool tickCheck = false;
bool autoSaved = false;
int tickCounter = 0;

void BeginFloatWindows(std::string UniqueName, bool& open, ImGuiWindowFlags flags = 0);
bool CompareChecks(RandomizerCheckObject, RandomizerCheckObject);
bool CheckByArea(RandomizerCheckArea, GetItemEntry, bool);
void DrawLocation(RandomizerCheckObject);
void EndFloatWindows();
bool HasItemBeenCollected(RandomizerCheck);
void LoadSettings();
void RainbowTick();
void UpdateAreas(RandomizerCheckArea area);
void UpdateInventoryChecks();
void UpdateAllOrdering();
void UpdateOrdering(RandomizerCheckArea);
int sectionId;

SceneID DungeonSceneLookupByArea(RandomizerCheckArea area) {
    switch (area) {
        case RCAREA_DEKU_TREE:              return SCENE_YDAN;
        case RCAREA_DODONGOS_CAVERN:        return SCENE_DDAN;
        case RCAREA_JABU_JABUS_BELLY:       return SCENE_BDAN;
        case RCAREA_FOREST_TEMPLE:          return SCENE_BMORI1;
        case RCAREA_FIRE_TEMPLE:            return SCENE_HIDAN;
        case RCAREA_WATER_TEMPLE:           return SCENE_MIZUSIN;
        case RCAREA_SPIRIT_TEMPLE:          return SCENE_JYASINZOU;
        case RCAREA_SHADOW_TEMPLE:          return SCENE_HAKADAN;
        case RCAREA_BOTTOM_OF_THE_WELL:     return SCENE_HAKADANCH;
        case RCAREA_ICE_CAVERN:             return SCENE_ICE_DOUKUTO;
        case RCAREA_GERUDO_TRAINING_GROUND: return SCENE_MEN;
        case RCAREA_GANONS_CASTLE:          return SCENE_GANONTIKA;
        default:                            return SCENE_ID_MAX;
    }
}

Color_RGBA8 Color_Bg_Default                        = {   0,   0,   0, 255 };   // Black
Color_RGBA8 Color_Main_Default                      = { 255, 255, 255, 255 };   // White
Color_RGBA8 Color_Area_Incomplete_Extra_Default     = { 255, 255, 255, 255 };   // White
Color_RGBA8 Color_Area_Complete_Extra_Default       = { 255, 255, 255, 255 };   // White
Color_RGBA8 Color_Unchecked_Extra_Default           = { 255, 255, 255, 255 };   // White
Color_RGBA8 Color_Skipped_Main_Default              = { 160, 160, 160, 255 };   // Grey
Color_RGBA8 Color_Skipped_Extra_Default             = { 160, 160, 160, 255 };   // Grey
Color_RGBA8 Color_Seen_Extra_Default                = { 255, 255, 255, 255 };   // TODO
Color_RGBA8 Color_Hinted_Extra_Default              = { 255, 255, 255, 255 };   // TODO
Color_RGBA8 Color_Collected_Extra_Default           = { 242, 101,  34, 255 };   // Orange
Color_RGBA8 Color_Scummed_Extra_Default             = {   0, 174, 239, 255 };   // Blue
Color_RGBA8 Color_Saved_Extra_Default               = {   0, 185,   0, 255 };   // Green

Color_RGBA8 Color_Background = { 0, 0, 0, 255 };

Color_RGBA8 Color_Area_Incomplete_Main  = { 255, 255, 255, 255 }; // White
Color_RGBA8 Color_Area_Incomplete_Extra = { 255, 255, 255, 255 }; // White
Color_RGBA8 Color_Area_Complete_Main    = { 255, 255, 255, 255 }; // White
Color_RGBA8 Color_Area_Complete_Extra   = { 255, 255, 255, 255 }; // White
Color_RGBA8 Color_Unchecked_Main        = { 255, 255, 255, 255 }; // White
Color_RGBA8 Color_Unchecked_Extra       = { 255, 255, 255, 255 }; // Useless
Color_RGBA8 Color_Skipped_Main          = { 160, 160, 160, 255 }; // Grey
Color_RGBA8 Color_Skipped_Extra         = { 160, 160, 160, 255 }; // Grey
Color_RGBA8 Color_Seen_Main             = { 255, 255, 255, 255 }; // TODO
Color_RGBA8 Color_Seen_Extra            = { 160, 160, 160, 255 }; // TODO
Color_RGBA8 Color_Hinted_Main           = { 255, 255, 255, 255 }; // TODO
Color_RGBA8 Color_Hinted_Extra          = { 255, 255, 255, 255 }; // TODO
Color_RGBA8 Color_Collected_Main        = { 255, 255, 255, 255 }; // White
Color_RGBA8 Color_Collected_Extra       = { 242, 101,  34, 255 }; // Orange
Color_RGBA8 Color_Scummed_Main          = { 255, 255, 255, 255 }; // White
Color_RGBA8 Color_Scummed_Extra         = {   0, 174, 239, 255 }; // Blue
Color_RGBA8 Color_Saved_Main            = { 255, 255, 255, 255 }; // White
Color_RGBA8 Color_Saved_Extra           = {   0, 185,   0, 255 }; // Green

std::vector<uint32_t> buttons = { BTN_A, BTN_B, BTN_CUP,   BTN_CDOWN, BTN_CLEFT, BTN_CRIGHT, BTN_L,
                                  BTN_Z, BTN_R, BTN_START, BTN_DUP,   BTN_DDOWN, BTN_DLEFT,  BTN_DRIGHT };

std::map<GetItemID, Text> itemNames = {
    { GI_HOOKSHOT,          Text{ "Hookshot",            "Grappin",                  "Gancho" } },
    { GI_LONGSHOT,          Text{ "Longshot",            "Super-Grappin",            "Supergancho" } },
    { GI_OCARINA_FAIRY,     Text{ "Fairy Ocarina",       "Ocarina des fées",         "Ocarina de las Hadas" } },
    { GI_OCARINA_OOT,       Text{ "Ocarina of Time",     "Ocarina du Temps",         "Ocarina del Tiempo" } },
    { GI_BOMB_BAG_20,       Text{ "Bomb Bag",            "Sac de Bombes",            "Saco de bombas" } },
    { GI_BOMB_BAG_30,       Text{ "Bomb Bag Upgrade",    "Sac de Bombes améliorer",  "Saco de bombas mejora" } },
    { GI_BOMB_BAG_40,       Text{ "Bomb Bag Upgrade",    "Sac de Bombes améliorer",  "Saco de bombas mejora" } },
    { GI_BOW,               Text{ "Fairy Bow",           "Arc des Fées",             "Arco de las Hadas" } },
    { GI_QUIVER_40,         Text{ "Quiver Upgrade",      "Carquois améliorer",       "Carcaj grande" } },
    { GI_QUIVER_50,         Text{ "Quiver Upgrade",      "Carquois améliorer",       "Carcaj grande" } },
    { GI_SLINGSHOT,         Text{ "Fairy Slingshot",     "Lance-Pierre des Fées",    "Resortera de las hadas" } },
    { GI_BULLET_BAG_40,     Text{ "Bullet Bag Upgrade",  "Sac de graines améliorer", "Bolsa de semillas deku mejora" } },
    { GI_BULLET_BAG_50,     Text{ "Bullet Bag Upgrade",  "Sac de graines améliorer", "Bolsa de semillas deku mejora" } },
    { GI_BRACELET,          Text{ "Goron's Bracelet",    "Bracelet Goron",           "Brazalete de los Goron" } },
    { GI_GAUNTLETS_SILVER,  Text{ "Silver Gauntlets",    "Gantelets d'argent",       "Guantes de plata" } },
    { GI_GAUNTLETS_GOLD,    Text{ "Golden Gauntlets",    "Gantelets d'or",           "Guantes de oro" } },
    { GI_SCALE_SILVER,      Text{ "Silver Scale",        "Écaille d'argent",         "Escama de Plata" } },
    { GI_SCALE_GOLD,        Text{ "Golden Scale",        "Écaille d'or",             "Escama de Oro" } },
    { GI_WALLET_ADULT,      Text{ "Adult Wallet",        "Grande Bourse",            "Bolsa de adulto" } },
    { GI_WALLET_GIANT,      Text{ "Giant Wallet",        "Bourse de Géant",          "Bolsa gigante" } },
    { GI_NUT_UPGRADE_30,    Text{ "Deku Nut Capacity",   "Capacité de noix Mojo",    "Capacidad de nueces deku" } },
    { GI_NUT_UPGRADE_40,    Text{ "Deku Nut Capacity",   "Capacité de noix Mojo",    "Capacidad de nueces deku" } },
    { GI_STICK_UPGRADE_20,  Text{ "Deku Stick Capacity", "Capacité de Bâtons Mojo",  "Capacidad de palos deku" } },
    { GI_STICK_UPGRADE_30,  Text{ "Deku Stick Capacity", "Capacité de Bâtons Mojo",  "Capacidad de palos deku" } }
    //{GI_,                       Text{"Magic Meter",                     "Jauge de Magie",                   "Poder mágico"}},
    //{RG_ENHANCED_MAGIC_METER,              Text{"Enhanced Magic Meter",            "Jauge de Magie améliorée",         "Poder mágico mejorado"}},

};

void SetLastItemGetRC(RandomizerCheck rc) {
    lastItemGetCheck = rc;
}

void DefaultCheckData(RandomizerCheck rc) {
    gSaveContext.checkTrackerData[rc].status = RCSHOW_UNCHECKED;
    gSaveContext.checkTrackerData[rc].skipped = 0;
    gSaveContext.checkTrackerData[rc].hintItem = RC_UNKNOWN_CHECK;
}

void SongFromImpa() {
    if (gSaveContext.n64ddFlag) {
        if (OTRGlobals::Instance->gRandomizer->GetRandoSettingValue(RSK_SKIP_CHILD_ZELDA) == RO_GENERIC_ON && gSaveContext.n64ddFlag) {
            if (gSaveContext.checkTrackerData[RC_SONG_FROM_IMPA].status != RCSHOW_SAVED) {
                gSaveContext.checkTrackerData[RC_SONG_FROM_IMPA].status = RCSHOW_SAVED;
            }
        }
    }
}

void GiftFromSages() {
    if (!gSaveContext.n64ddFlag) {
        DefaultCheckData(RC_GIFT_FROM_SAGES);
    }
}

//std::map<RandomizerCheck, RandomizerCheckShow> checkStatusMap;
std::vector<RandomizerCheckObject> checks;

// Function for adding Link's Pocket check
void LinksPocket() {
    if (gSaveContext.n64ddFlag) {
        if (OTRGlobals::Instance->gRandomizer->GetRandoSettingValue(RSK_LINKS_POCKET) != RO_LINKS_POCKET_NOTHING ||
            OTRGlobals::Instance->gRandomizer->GetRandoSettingValue(RSK_SHUFFLE_DUNGEON_REWARDS) == RO_DUNGEON_REWARDS_END_OF_DUNGEON) {
            DefaultCheckData(RC_LINKS_POCKET);
            gSaveContext.checkTrackerData[RC_LINKS_POCKET].status = RCSHOW_SAVED;
        }
    }
}

bool IsGameRunning() {
    return gPlayState != nullptr && gSaveContext.fileNum < 10;
}

void TrySetAreas() {
    if (checksByArea.empty()) {
        for (int i = RCAREA_KOKIRI_FOREST; i < RCAREA_INVALID; i++) {
            checksByArea.emplace(static_cast<RandomizerCheckArea>(i), std::vector<RandomizerCheckObject>());
        }
    }
}

void SetCheckCollected(RandomizerCheck rc) {
    gSaveContext.checkTrackerData[rc].status = RCSHOW_COLLECTED;
    RandomizerCheckObject rcObj;
    if (rc == RC_GIFT_FROM_SAGES && !gSaveContext.n64ddFlag) {
        rcObj = RCO_RAORU;
    } else {
        rcObj = RandomizerCheckObjects::GetAllRCObjects().find(rc)->second;
    }
    if (!gSaveContext.checkTrackerData[rc].skipped) {
        areaChecksGotten[rcObj.rcArea]++;
    } else {
        gSaveContext.checkTrackerData[rc].skipped = false;
    }
    autoSaved = false;
    SaveManager::Instance->SaveSection(gSaveContext.fileNum, sectionId, true);

    doAreaScroll = true;
    UpdateOrdering(rcObj.rcArea);
    UpdateInventoryChecks();
}

bool IsAreaScene(SceneID sceneNum) {
    switch (sceneNum) {
        case SCENE_SPOT00:
        case SCENE_SPOT01:
        case SCENE_SPOT02:
        case SCENE_SPOT03:
        case SCENE_SPOT04:
        case SCENE_SPOT05:
        case SCENE_SPOT06:
        case SCENE_SPOT07:
        case SCENE_SPOT08:
        case SCENE_SPOT09:
        case SCENE_SPOT10:
        case SCENE_SPOT11:
        case SCENE_SPOT12:
        case SCENE_SPOT13:
        case SCENE_SPOT15:
        case SCENE_SPOT16:
        case SCENE_SPOT17:
        case SCENE_SPOT18:
        case SCENE_SPOT20:
        case SCENE_YDAN:
        case SCENE_DDAN:
        case SCENE_BDAN:
        case SCENE_BMORI1:
        case SCENE_HIDAN:
        case SCENE_MIZUSIN:
        case SCENE_JYASINZOU:
        case SCENE_HAKADAN:
        case SCENE_HAKADANCH:
        case SCENE_ICE_DOUKUTO:
        case SCENE_MEN:
        case SCENE_GANON:
        case SCENE_GANONTIKA:
        case SCENE_MARKET_ALLEY:
        case SCENE_MARKET_ALLEY_N:
        case SCENE_MARKET_DAY:
        case SCENE_MARKET_NIGHT:
        case SCENE_MARKET_RUINS:
            return true;
        default:
            return false;
    }
}

RandomizerCheckArea AreaFromEntranceGroup[] = {
    RCAREA_INVALID,
    RCAREA_KOKIRI_FOREST,
    RCAREA_LOST_WOODS,
    RCAREA_SACRED_FOREST_MEADOW,
    RCAREA_KAKARIKO_VILLAGE,
    RCAREA_GRAVEYARD,
    RCAREA_DEATH_MOUNTAIN_TRAIL,
    RCAREA_DEATH_MOUNTAIN_CRATER,
    RCAREA_GORON_CITY,
    RCAREA_ZORAS_RIVER,
    RCAREA_ZORAS_DOMAIN,
    RCAREA_ZORAS_FOUNTAIN,
    RCAREA_HYRULE_FIELD,
    RCAREA_LON_LON_RANCH,
    RCAREA_LAKE_HYLIA,
    RCAREA_GERUDO_VALLEY,
    RCAREA_WASTELAND,
    RCAREA_MARKET,
    RCAREA_HYRULE_CASTLE,
};

RandomizerCheckArea GetCheckArea() {
    auto scene = static_cast<SceneID>(gPlayState->sceneNum);
    bool grottoScene = (scene == SCENE_KAKUSIANA || scene == SCENE_YOUSEI_IZUMI_TATE);
    const EntranceData* ent = GetEntranceData(grottoScene ?
        ENTRANCE_RANDO_GROTTO_EXIT_START + GetCurrentGrottoId() : gSaveContext.entranceIndex);
    RandomizerCheckArea area = RCAREA_INVALID;
    if (ent != nullptr && !IsAreaScene(scene) && ent->type != ENTRANCE_TYPE_DUNGEON) {
        if (ent->source == "Desert Colossus" || ent->destination == "Desert Colossus") {
            area = RCAREA_DESERT_COLOSSUS;
        } else if (ent->source == "Gerudo Fortress" || ent->destination == "Gerudo Fortress") {
            area = RCAREA_GERUDO_FORTRESS;
        } else {
            area = AreaFromEntranceGroup[ent->dstGroup];
        }
    }
    if (area == RCAREA_INVALID) {
        if (grottoScene && (GetCurrentGrottoId() == -1) && (OTRGlobals::Instance->gRandomizer->GetRandoSettingValue(RSK_SHUFFLE_GROTTO_ENTRANCES) == RO_GENERIC_OFF)) {
            area = previousArea;
        } else {
            area = RandomizerCheckObjects::GetRCAreaBySceneID(scene);
        }
    }
    return area;
}

bool vector_contains_scene(std::vector<SceneID> vec, const int16_t scene) {
    return std::any_of(vec.begin(), vec.end(), [&](const auto& x) { return x == scene; });
}

std::vector<SceneID> skipScenes = {SCENE_GANON_BOSS, SCENE_GANON_FINAL, SCENE_GANON_DEMO, SCENE_GANONTIKA_SONOGO, SCENE_GANON_SONOGO};
std::vector<SceneID> scenesToCheck = {SCENE_YOUSEI_IZUMI_YOKO, SCENE_YOUSEI_IZUMI_TATE, SCENE_KAKUSIANA, SCENE_TOKINOMA, SCENE_SHOP1};

bool CheckChecks(GetItemEntry giEntry = GET_ITEM_NONE, bool recheck = false) {
    if (gPlayState == nullptr) {
        return false;
    }
    return CheckByArea(GetCheckArea(), giEntry, recheck);
}

bool EvaluateCheck(RandomizerCheckObject rco) {
    if (HasItemBeenCollected(rco.rc) && gSaveContext.checkTrackerData[rco.rc].status != RCSHOW_COLLECTED &&
            gSaveContext.checkTrackerData[rco.rc].status != RCSHOW_SAVED) {
        SetCheckCollected(rco.rc);
        return true;
    }
    return false;
}

std::map<RandomizerCheck, QuestItem> QuestItemFromCheck = {
    { RC_QUEEN_GOHMA, QUEST_KOKIRI_EMERALD },
    { RC_KING_DODONGO, QUEST_GORON_RUBY },
    { RC_BARINADE, QUEST_ZORA_SAPPHIRE },
    { RC_PHANTOM_GANON, QUEST_MEDALLION_FOREST },
    { RC_VOLVAGIA, QUEST_MEDALLION_FIRE },
    { RC_MORPHA, QUEST_MEDALLION_WATER },
    { RC_BONGO_BONGO, QUEST_MEDALLION_SHADOW },
    { RC_TWINROVA, QUEST_MEDALLION_SPIRIT },
    { RC_GIFT_FROM_SAGES, QUEST_MEDALLION_LIGHT },
    { RC_SONG_FROM_IMPA, QUEST_SONG_LULLABY },
    { RC_SONG_FROM_MALON, QUEST_SONG_EPONA },
    { RC_SONG_FROM_SARIA, QUEST_SONG_SARIA },
    { RC_SONG_FROM_ROYAL_FAMILYS_TOMB, QUEST_SONG_SUN },
    { RC_SONG_FROM_OCARINA_OF_TIME, QUEST_SONG_TIME },
    { RC_SONG_FROM_WINDMILL, QUEST_SONG_STORMS },
    { RC_SHEIK_IN_FOREST, QUEST_SONG_MINUET },
    { RC_SHEIK_IN_CRATER, QUEST_SONG_BOLERO },
    { RC_SHEIK_IN_ICE_CAVERN, QUEST_SONG_SERENADE },
    { RC_SHEIK_AT_COLOSSUS, QUEST_SONG_REQUIEM },
    { RC_SHEIK_IN_KAKARIKO, QUEST_SONG_NOCTURNE },
    { RC_SHEIK_AT_TEMPLE, QUEST_SONG_PRELUDE }
};

bool CheckByArea(RandomizerCheckArea area = RCAREA_INVALID, GetItemEntry giEntry = GET_ITEM_NONE, bool recheck = false) {
    if (area == RCAREA_INVALID) {
        area = GetCheckArea();
    }
    if (recheck) {
        for (auto rco : checksByArea.find(area)->second) {
            RandomizerCheckTrackerData rcData = gSaveContext.checkTrackerData[rco.rc];
            GetItemID giid = OTRGlobals::Instance->gRandomizer->GetItemIdFromRandomizerGet(
                OTRGlobals::Instance->gRandomizer->GetRandomizerGetDataFromKnownCheck(rco.rc).rgID, rco.ogItemId);
            if (rcData.status == RCSHOW_SCUMMED || rcData.status == RCSHOW_COLLECTED || rcData.status == RCSHOW_SAVED) {
                gSaveContext.checkTrackerData[rco.rc].status = RCSHOW_UNCHECKED;
                if (rcData.status != RCSHOW_SCUMMED) {
                    areaChecksGotten[rco.rcArea]--;
                }
            }
            bool collected = false;
            if (HasItemBeenCollected(rco.rc)) {
                collected = true;
            }
            if (!gSaveContext.n64ddFlag) {
                if (rco.rc == RC_KF_SHOP_ITEM_3) {
                    collected = HasEqItem(equipmentItems[6]);
                }else if (rco.rc >= RC_QUEEN_GOHMA && rco.rc <= RC_SHEIK_AT_TEMPLE) {
                    if (QuestItemFromCheck.contains(rco.rc)) {
                        if (rco.rc == RC_GIFT_FROM_SAGES) {
                            if (HasQuestItem(dungeonRewardMedallions[5])) {
                                collected = true;
                            }
                        } else if (rco.rc <= RC_BARINADE) {
                            if (HasQuestItem(dungeonRewardStones[QuestItemFromCheck.find(rco.rc)->second - QUEST_KOKIRI_EMERALD])) {
                                collected = true;
                            }
                        } else if (rco.rc <= RC_TWINROVA) {
                            if (HasQuestItem(dungeonRewardMedallions[QuestItemFromCheck.find(rco.rc)->second - QUEST_MEDALLION_FOREST])) {
                                collected = true;
                            }
                        } else if (rco.rc <= RC_SONG_FROM_WINDMILL) {
                            auto item = QuestItemFromCheck.find(rco.rc)->second;
                            auto titem = songItems[item - QUEST_SONG_LULLABY];
                            if (HasSong(titem)) {
                                collected = true;
                            }
                        } else if (rco.rc <= RC_SHEIK_AT_TEMPLE) {
                            auto item = QuestItemFromCheck.find(rco.rc)->second;
                            auto titem = songItems[item - QUEST_SONG_MINUET + 6];
                            if (HasSong(titem)) {
                                collected = true;
                            }
                        } else if (rco.rc == RC_GC_DARUNIAS_JOY) {
                            if (CUR_UPG_VALUE(UPG_STRENGTH) == 1) {
                                collected = true;
                            }
                        }
                    }
                }
            }
            if (collected) {
                gSaveContext.checkTrackerData[rco.rc].status = RCSHOW_SAVED;
                if (rcData.skipped) {
                    gSaveContext.checkTrackerData[rco.rc].skipped = false;
                } else {
                    areaChecksGotten[rco.rcArea]++;
                }
            }
        }
        return true;
    } else if (area != RCAREA_INVALID) {
        auto areaChecks = checksByArea.find(checkArea)->second;
        if (checkCounter >= areaChecks.size()) {
            checkCounter = 0;
            checkLoops++;
        }
        auto rco = areaChecks.at(checkCounter);
        return EvaluateCheck(rco);
    }
}

bool HasItemBeenCollected(RandomizerCheck rc) {
    if (gPlayState == nullptr) {
        return false;
    }
    ItemLocation* x = Location(rc);
    SpoilerCollectionCheck check = x->GetCollectionCheck();
    auto flag = check.flag;
    auto scene = check.scene;
    auto type = check.type;

    switch (type) {
    case SpoilerCollectionCheckType::SPOILER_CHK_ALWAYS_COLLECTED:
        return true;
    case SpoilerCollectionCheckType::SPOILER_CHK_BIGGORON:
        return gSaveContext.bgsFlag & flag;
    case SpoilerCollectionCheckType::SPOILER_CHK_CHEST:
        return (gPlayState->sceneNum == scene && gPlayState->actorCtx.flags.chest & (1 << flag)) || gSaveContext.sceneFlags[scene].chest & (1 << flag);
    case SpoilerCollectionCheckType::SPOILER_CHK_COLLECTABLE:
        return (gPlayState->sceneNum == scene && gPlayState->actorCtx.flags.collect & (1 << flag)) || gSaveContext.sceneFlags[scene].collect & (1 << flag);
    case SpoilerCollectionCheckType::SPOILER_CHK_MERCHANT:
    case SpoilerCollectionCheckType::SPOILER_CHK_SHOP_ITEM:
    case SpoilerCollectionCheckType::SPOILER_CHK_COW:
    case SpoilerCollectionCheckType::SPOILER_CHK_SCRUB:
    case SpoilerCollectionCheckType::SPOILER_CHK_RANDOMIZER_INF:
        return Flags_GetRandomizerInf(OTRGlobals::Instance->gRandomizer->GetRandomizerInfFromCheck(rc));
    case SpoilerCollectionCheckType::SPOILER_CHK_EVENT_CHK_INF:
        return gSaveContext.eventChkInf[flag / 16] & (0x01 << flag % 16);
    case SpoilerCollectionCheckType::SPOILER_CHK_GERUDO_MEMBERSHIP_CARD:
        return CHECK_FLAG_ALL(gSaveContext.eventChkInf[0x09], 0x0F);
    case SpoilerCollectionCheckType::SPOILER_CHK_GOLD_SKULLTULA:
        return GET_GS_FLAGS(scene) & flag;
    case SpoilerCollectionCheckType::SPOILER_CHK_INF_TABLE:
        return gSaveContext.infTable[scene] & INDEX_TO_16BIT_LITTLE_ENDIAN_BITMASK(flag);
    case SpoilerCollectionCheckType::SPOILER_CHK_ITEM_GET_INF:
        return gSaveContext.itemGetInf[flag / 16] & INDEX_TO_16BIT_LITTLE_ENDIAN_BITMASK(flag);
    case SpoilerCollectionCheckType::SPOILER_CHK_MAGIC_BEANS:
        return BEANS_BOUGHT >= 10;
    case SpoilerCollectionCheckType::SPOILER_CHK_MINIGAME:
        if (rc == RC_LH_CHILD_FISHING)
            return HIGH_SCORE(HS_FISHING) & 0x400;
        if (rc == RC_LH_ADULT_FISHING)
            return HIGH_SCORE(HS_FISHING) & 0x800;
    case SpoilerCollectionCheckType::SPOILER_CHK_NONE:
        return false;
    case SpoilerCollectionCheckType::SPOILER_CHK_POE_POINTS:
        return gSaveContext.highScores[HS_POE_POINTS] >= 1000;
    case SpoilerCollectionCheckType::SPOILER_CHK_GRAVEDIGGER:
        // Gravedigger has a fix in place that means one of two save locations. Check both.
        return (gSaveContext.itemGetInf[1] & 0x1000) || // vanilla flag
            ((gSaveContext.n64ddFlag || CVarGetInteger("gGravediggingTourFix", 0)) &&
                gSaveContext.sceneFlags[scene].collect & (1 << flag) || (gPlayState->actorCtx.flags.collect & (1 << flag))); // rando/fix flag
    default:
        return false;
    }
    return false;
}

void CheckTrackerDialogClosed() {
    if (messageCloseCheck) {
        messageCloseCheck = false;
        checkCollected = true;
    }
}

void CheckTrackerTransition(uint32_t sceneNum) {
    if (!IsGameRunning()) {
        return;
    }
    gSaveContext;
    if (transitionCheck) {
        transitionCheck = false;
        checkCollected = true;
    }
    doAreaScroll = true;
    previousArea = currentArea;
    currentArea = GetCheckArea();
}

void CheckTrackerFrame() {
    if (!IsGameRunning()) {
        return;
    }
    if (checkCollected) {
        for (int i = 0; i < 6; i++) {
            if (CheckChecks()) {
                checkCollected = false;
                checkCounter = 0;
                break;
            } else {
                checkCounter++;
            }
        }
        if (checkLoops > 15) {
            checkCollected = false;
            checkLoops = 0;
        }
    }
    if (savedFrames > 0 && !pendingSaleCheck) {
        savedFrames--;
    }
}

void CheckTrackerSaleEnd(GetItemEntry giEntry) {
    if (pendingSaleCheck) {
        pendingSaleCheck = false;
        checkCollected = true;
    }
}

void CheckTrackerItemReceive(GetItemEntry giEntry) {
    if (!IsGameRunning() || vector_contains_scene(skipScenes, gPlayState->sceneNum)) {
        return;
    }
    auto scene = static_cast<SceneID>(gPlayState->sceneNum);
    // Vanilla special item checks
    if (!gSaveContext.n64ddFlag) {
        if (scene == SCENE_KOKIRI_SHOP && giEntry.itemId == ITEM_SHIELD_DEKU) {
            SetCheckCollected(RC_KF_SHOP_ITEM_3);
            return;
        }
        if (giEntry.itemId == ITEM_KOKIRI_EMERALD) {
            SetCheckCollected(RC_QUEEN_GOHMA);
            return;
        } else if (giEntry.itemId == ITEM_GORON_RUBY) {
            SetCheckCollected(RC_KING_DODONGO);
            return;
        } else if (giEntry.itemId == ITEM_ZORA_SAPPHIRE) {
            SetCheckCollected(RC_BARINADE);
            return;
        } else if (giEntry.itemId == ITEM_MEDALLION_FOREST) {
            SetCheckCollected(RC_PHANTOM_GANON);
            return;
        } else if (giEntry.itemId == ITEM_MEDALLION_FIRE) {
            SetCheckCollected(RC_VOLVAGIA);
            return;
        } else if (giEntry.itemId == ITEM_MEDALLION_WATER) {
            SetCheckCollected(RC_MORPHA);
            return;
        } else if (giEntry.itemId == ITEM_MEDALLION_SHADOW) {
            SetCheckCollected(RC_BONGO_BONGO);
            return;
        } else if (giEntry.itemId == ITEM_MEDALLION_SPIRIT) {
            SetCheckCollected(RC_TWINROVA);
            return;
        } else if (giEntry.itemId == ITEM_MEDALLION_LIGHT) {
            SetCheckCollected(RC_GIFT_FROM_SAGES);
            return;
        } else if (giEntry.itemId == ITEM_SONG_LULLABY) {
            SetCheckCollected(RC_SONG_FROM_IMPA);
            return;
        } else if (giEntry.itemId == ITEM_SONG_EPONA) {
            SetCheckCollected(RC_SONG_FROM_MALON);
            return;
        } else if (giEntry.itemId == ITEM_SONG_SARIA) {
            SetCheckCollected(RC_SONG_FROM_SARIA);
            return;
        } else if (giEntry.itemId == ITEM_SONG_SUN) {
            SetCheckCollected(RC_SONG_FROM_ROYAL_FAMILYS_TOMB);
            return;
        } else if (giEntry.itemId == ITEM_SONG_TIME) {
            SetCheckCollected(RC_SONG_FROM_OCARINA_OF_TIME);
            return;
        } else if (giEntry.itemId == ITEM_SONG_STORMS) {
            SetCheckCollected(RC_SONG_FROM_WINDMILL);
            return;
        } else if (giEntry.itemId == ITEM_SONG_MINUET) {
            SetCheckCollected(RC_SHEIK_IN_FOREST);
            return;
        } else if (giEntry.itemId == ITEM_SONG_BOLERO) {
            SetCheckCollected(RC_SHEIK_IN_CRATER);
            return;
        } else if (giEntry.itemId == ITEM_SONG_SERENADE) {
            SetCheckCollected(RC_SHEIK_IN_ICE_CAVERN);
            return;
        } else if (giEntry.itemId == ITEM_SONG_NOCTURNE) {
            SetCheckCollected(RC_SHEIK_IN_KAKARIKO);
            return;
        } else if (giEntry.itemId == ITEM_SONG_REQUIEM) {
            SetCheckCollected(RC_SHEIK_AT_COLOSSUS);
            return;
        } else if (giEntry.itemId == ITEM_SONG_PRELUDE) {
            SetCheckCollected(RC_SHEIK_AT_TEMPLE);
            return;
        } else if (giEntry.itemId == ITEM_BRACELET) {
            SetCheckCollected(RC_GC_DARUNIAS_JOY);
            return;
        }
    }
    checkScene = scene;
    checkArea = GetCheckArea();
    if (gSaveContext.pendingSale != ITEM_NONE) {
        pendingSaleCheck = true;
        return;
    }
    if (scene == SCENE_YOUSEI_IZUMI_YOKO) {
        if (checkArea == RCAREA_HYRULE_CASTLE) {
            SetCheckCollected(RC_HC_GREAT_FAIRY_REWARD);
        } else if (checkArea == RCAREA_DESERT_COLOSSUS) {
            SetCheckCollected(RC_COLOSSUS_GREAT_FAIRY_REWARD);
        } else if (checkArea == RCAREA_ZORAS_FOUNTAIN) {
            SetCheckCollected(RC_ZF_GREAT_FAIRY_REWARD);
        }
        return;
    }
    if (scene == SCENE_DAIYOUSEI_IZUMI) {
        if (checkArea == RCAREA_HYRULE_CASTLE) {
            SetCheckCollected(RC_OGC_GREAT_FAIRY_REWARD);
        } else if (checkArea == RCAREA_DEATH_MOUNTAIN_TRAIL) {
            SetCheckCollected(RC_DMT_GREAT_FAIRY_REWARD);
        } else if (checkArea == RCAREA_DEATH_MOUNTAIN_CRATER) {
            SetCheckCollected(RC_DMC_GREAT_FAIRY_REWARD);
        }
        return;
    }
    if (scene == SCENE_SPOT11 && (gSaveContext.entranceIndex == 485 || gSaveContext.entranceIndex == 489)) {
        if (EvaluateCheck(RandomizerCheckObjects::GetAllRCObjects().find(RC_SPIRIT_TEMPLE_SILVER_GAUNTLETS_CHEST)->second) ||
            EvaluateCheck(RandomizerCheckObjects::GetAllRCObjects().find(RC_SPIRIT_TEMPLE_MIRROR_SHIELD_CHEST)->second)) {
            return;
        }
    }
    if (GET_PLAYER(gPlayState) == nullptr) {
        transitionCheck = true;
        return;
    }
    RandomizerCheck rc = RC_UNKNOWN_CHECK;
    if (gPlayState->lastCheck != nullptr) {
        rc = OTRGlobals::Instance->gRandomizer->GetCheckFromActor(gPlayState->lastCheck->id, scene, gPlayState->lastCheck->params);
    }
    if (gPlayState->msgCtx.msgMode != MSGMODE_NONE && rc != RC_UNKNOWN_CHECK) {
        RandomizerCheckObject rco = RandomizerCheckObjects::GetAllRCObjects().find(rc)->second;
        if (!(rco.rcType == RCTYPE_SKULL_TOKEN && giEntry.itemId == ITEM_SKULL_TOKEN)) {
            messageCloseCheck = true;
            return;
        }
    }
    if (gSaveContext.n64ddFlag || (!gSaveContext.n64ddFlag && giEntry.getItemCategory != ITEM_CATEGORY_JUNK)) {
        checkCollected = true;
    }
}

void InitTrackerData(bool isDebug) {
    TrySetAreas();
    for (auto& [rc, rco] : RandomizerCheckObjects::GetAllRCObjects()) {
        if (rc != RC_UNKNOWN_CHECK && rc != RC_MAX && rc != RC_LINKS_POCKET) {
            DefaultCheckData(rc);
        }
    }
    if (gSaveContext.n64ddFlag) {
        LinksPocket();
        SongFromImpa();
        // TODO: add debug checks?
    } else {
        GiftFromSages();
    }
    UpdateAllOrdering();
    UpdateInventoryChecks();
}

void SaveTrackerData(SaveContext* saveContext, int sectionID, bool gameSave) {
    SaveManager::Instance->SaveArray("checks", ARRAY_COUNT(saveContext->checkTrackerData), [&](size_t i) {
        if (saveContext->checkTrackerData[i].status == RCSHOW_COLLECTED) {
            if (gameSave || savedFrames > 0) {
                gSaveContext.checkTrackerData[i].status = saveContext->checkTrackerData[i].status = RCSHOW_SAVED;
                UpdateAllOrdering();
                UpdateInventoryChecks();
            } else {
                saveContext->checkTrackerData[i].status = RCSHOW_SCUMMED;
            }
        }
        SaveManager::Instance->SaveStruct("", [&]() {
            SaveManager::Instance->SaveData("status", saveContext->checkTrackerData[i].status);
            SaveManager::Instance->SaveData("skipped", saveContext->checkTrackerData[i].skipped);
            SaveManager::Instance->SaveData("hintItem", saveContext->checkTrackerData[i].hintItem);
        });
    });
}

void SaveFile(SaveContext* saveContext, int sectionID, bool fullSave) {
    SaveTrackerData(saveContext, sectionID, fullSave);
    if (fullSave) {
        savedFrames = 40;
    }
}

void LoadFile() {
    Teardown();
    LoadSettings();
    TrySetAreas();
    SaveManager::Instance->LoadArray("checks", RC_MAX, [](size_t i) {
        SaveManager::Instance->LoadStruct("", [&]() {
            SaveManager::Instance->LoadData("status", gSaveContext.checkTrackerData[i].status);
            SaveManager::Instance->LoadData("skipped", gSaveContext.checkTrackerData[i].skipped);
            SaveManager::Instance->LoadData("hintItem", gSaveContext.checkTrackerData[i].hintItem);
        });
        RandomizerCheckTrackerData entry = gSaveContext.checkTrackerData[i];
        RandomizerCheck rc = static_cast<RandomizerCheck>(i);
        if (rc == RC_UNKNOWN_CHECK || rc == RC_LINKS_POCKET || rc == RC_MAX ||
            !RandomizerCheckObjects::GetAllRCObjects().contains(rc))
            return;

        RandomizerCheckObject entry2;
        if (rc == RC_GIFT_FROM_SAGES && !gSaveContext.n64ddFlag) {
            entry2 = RCO_RAORU;
        } else {
            entry2 = RandomizerCheckObjects::GetAllRCObjects().find(rc)->second;
        }
        if (!IsVisibleInCheckTracker(entry2)) return;

        checksByArea.find(entry2.rcArea)->second.push_back(entry2);
        if (entry.status == RCSHOW_SAVED || entry.skipped) {
            areaChecksGotten[entry2.rcArea]++;
        }

        if (areaChecksGotten[entry2.rcArea] != 0 || RandomizerCheckObjects::AreaIsOverworld(entry2.rcArea)) {
            areasSpoiled |= (1 << entry2.rcArea);
        }
    });
    if (OTRGlobals::Instance->gRandomizer->GetRandoSettingValue(RSK_LINKS_POCKET) != RO_LINKS_POCKET_NOTHING && gSaveContext.n64ddFlag) {
        s8 startingAge = OTRGlobals::Instance->gRandomizer->GetRandoSettingValue(RSK_STARTING_AGE);
        RandomizerCheckArea startingArea;
        switch (startingAge) {
            case RO_AGE_CHILD:
                startingArea = RCAREA_KOKIRI_FOREST;
                break;
            case RO_AGE_ADULT:
                startingArea = RCAREA_MARKET;
                break;
            default:
                startingArea = RCAREA_KOKIRI_FOREST;
                break;
        }
        RandomizerCheckObject linksPocket = { RC_LINKS_POCKET, RCVORMQ_BOTH, RCTYPE_LINKS_POCKET, startingArea, ACTOR_ID_MAX, SCENE_ID_MAX, 0x00, GI_NONE, false, "Link's Pocket", "Link's Pocket" };
        
        checksByArea.find(startingArea)->second.push_back(linksPocket);
        areaChecksGotten[startingArea]++;
    }

    showVOrMQ = (OTRGlobals::Instance->gRandomizer->GetRandoSettingValue(RSK_MQ_DUNGEON_COUNT) > 0);
    LinksPocket();
    doInitialize = false;
    initialized = true;
    UpdateAllOrdering();
    UpdateInventoryChecks();
}

void Teardown() {
    initialized = false;
    for (auto& [rcArea, vec] : checksByArea) {
        vec.clear();
        areaChecksGotten[rcArea] = 0;
    }
    checksByArea.clear();
    areasSpoiled = 0;
    checkCollected = false;
    checkLoops = 0;

    lastLocationChecked = RC_UNKNOWN_CHECK;
}

void CheckTrackerWindow::DrawElement() {
    ImGui::SetNextWindowSize(ImVec2(400, 540), ImGuiCond_FirstUseEver);

    if (initialized && (gPlayState == nullptr || gSaveContext.fileNum < 0 || gSaveContext.fileNum > 2)) {
        return;
    }

    if (CVarGetInteger("gCheckTrackerWindowType", TRACKER_WINDOW_WINDOW) == TRACKER_WINDOW_FLOATING) {
        if (CVarGetInteger("gCheckTrackerShowOnlyPaused", 0) && (gPlayState == nullptr || gPlayState->pauseCtx.state == 0)) {
            return;
        }

        if (CVarGetInteger("gCheckTrackerDisplayType", TRACKER_DISPLAY_ALWAYS) == TRACKER_DISPLAY_COMBO_BUTTON) {
            int comboButton1Mask = buttons[CVarGetInteger("gCheckTrackerComboButton1", TRACKER_COMBO_BUTTON_L)];
            int comboButton2Mask = buttons[CVarGetInteger("gCheckTrackerComboButton2", TRACKER_COMBO_BUTTON_R)];
            OSContPad* trackerButtonsPressed = LUS::Context::GetInstance()->GetControlDeck()->GetPads();
            bool comboButtonsHeld = trackerButtonsPressed != nullptr &&
                                    trackerButtonsPressed[0].button & comboButton1Mask &&
                                    trackerButtonsPressed[0].button & comboButton2Mask;
            if (!comboButtonsHeld) {
                return;
            }
        }
    }

    BeginFloatWindows("Check Tracker", mIsVisible, ImGuiWindowFlags_NoScrollbar);

    if (!IsGameRunning()) {
        ImGui::Text("Waiting for file load..."); //TODO Language
        EndFloatWindows();
        return;
    }

    SceneID sceneId = SCENE_ID_MAX;
    if (gPlayState != nullptr) {
        sceneId = (SceneID)gPlayState->sceneNum;
    }

    //previousArea = currentArea;
    areasSpoiled |= (1 << currentArea);

    //Quick Options
#ifdef __WIIU__
    float headerHeight = 40.0f;
#else
    float headerHeight = 20.0f;
#endif
    ImVec2 size = ImGui::GetContentRegionMax();
    size.y -= headerHeight;
    if (!ImGui::BeginTable("Check Tracker", 1, 0, size)) {
        EndFloatWindows();
        return;
    }

    ImGui::TableNextRow(0, headerHeight);
    ImGui::TableNextColumn();
    UIWidgets::EnhancementCheckbox(
        "Show Hidden Items", "gCheckTrackerOptionShowHidden", false,
        "When active, items will show hidden checks by default when updated to this state.");
    UIWidgets::PaddedSeparator();
    if (ImGui::Button("Expand All")) {
        optCollapseAll = false;
        optExpandAll = true;
    }
    ImGui::SameLine();
    if (ImGui::Button("Collapse All")) {
        optExpandAll = false;
        optCollapseAll = true;
    }
    ImGui::SameLine();
    if (ImGui::Button("Recheck Area")) {
        CheckChecks(GET_ITEM_NONE, true);
        UpdateAllOrdering();
        UpdateInventoryChecks();
    }
    UIWidgets::PaddedSeparator();

    //Checks Section Lead-in
    ImGui::TableNextRow();
    ImGui::TableNextColumn();
    size = ImGui::GetContentRegionAvail();
    if (!ImGui::BeginTable("CheckTracker##Checks", 1, ImGuiTableFlags_ScrollY, size)) {
        ImGui::EndTable();
        EndFloatWindows();
        return;
    }
    ImGui::TableNextRow();
    ImGui::TableNextColumn();

    //Prep for loop
    RainbowTick();
    bool doDraw = false;
    bool thisAreaFullyChecked = false;
    bool showHidden = CVarGetInteger("gCheckTrackerOptionShowHidden", 0);
    bool hideIncomplete = CVarGetInteger("gCheckTrackerAreaIncompleteHide", 0);
    bool hideComplete = CVarGetInteger("gCheckTrackerAreaCompleteHide", 0);
    bool collapseLogic;
    bool doingCollapseOrExpand = optExpandAll || optCollapseAll;
    bool isThisAreaSpoiled;
    RandomizerCheckArea lastArea = RCAREA_INVALID;
    Color_RGBA8 areaCompleteColor = CVarGetColor("gCheckTrackerAreaMainCompleteColor", Color_Main_Default);
    Color_RGBA8 areaIncompleteColor = CVarGetColor("gCheckTrackerAreaMainIncompleteColor", Color_Main_Default);
    Color_RGBA8 extraCompleteColor = CVarGetColor("gCheckTrackerAreaExtraCompleteColor", Color_Area_Complete_Extra_Default);
    Color_RGBA8 extraIncompleteColor = CVarGetColor("gCheckTrackerAreaExtraIncompleteColor", Color_Area_Incomplete_Extra_Default);
    Color_RGBA8 mainColor;
    Color_RGBA8 extraColor;
    std::string stemp;
    s32 areaMask = 1;

    for (auto& [rcArea, objs] : checksByArea) {
        RandomizerCheckArea thisArea = currentArea;

        const int areaChecksTotal = static_cast<int>(objs.size());
        thisAreaFullyChecked = (areaChecksGotten[rcArea] == areaChecksTotal);
        //Last Area needs to be cleaned up
        if (lastArea != RCAREA_INVALID && doDraw) {
            UIWidgets::PaddedSeparator();
        }
        lastArea = rcArea;
        if (previousShowHidden != showHidden) {
            previousShowHidden = showHidden;
            doAreaScroll = true;
        }
        if (!showHidden && (
            hideComplete && thisAreaFullyChecked || 
            hideIncomplete && !thisAreaFullyChecked
        )) {
            doDraw = false;
        } else {
            //Get the colour for the area
            if (thisAreaFullyChecked) {
                mainColor = areaCompleteColor;
                extraColor = extraCompleteColor;
            } else {
                mainColor = areaIncompleteColor;
                extraColor = extraIncompleteColor;
            }

            //Draw the area
            collapseLogic = !thisAreaFullyChecked;
            if (doingCollapseOrExpand) {
                if (optExpandAll) {
                    collapseLogic = true;
                } else if (optCollapseAll) {
                    collapseLogic = false;
                }
            }
            stemp = RandomizerCheckObjects::GetRCAreaName(rcArea) + "##TreeNode";
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(mainColor.r / 255.0f, mainColor.g / 255.0f,
                                                        mainColor.b / 255.0f, mainColor.a / 255.0f));
            if (doingCollapseOrExpand)
                ImGui::SetNextItemOpen(collapseLogic, ImGuiCond_Always);
            else
                ImGui::SetNextItemOpen(!thisAreaFullyChecked, ImGuiCond_Once);
            doDraw = ImGui::TreeNode(stemp.c_str());
            ImGui::PopStyleColor();
            ImGui::SameLine();
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(extraColor.r / 255.0f, extraColor.g / 255.0f,
                                                        extraColor.b / 255.0f, extraColor.a / 255.0f));

            isThisAreaSpoiled = areasSpoiled & areaMask || CVarGetInteger("gCheckTrackerOptionMQSpoilers", 0);

            if (isThisAreaSpoiled) {
                if (showVOrMQ && RandomizerCheckObjects::AreaIsDungeon(rcArea)) {
                    if (OTRGlobals::Instance->gRandomizer->masterQuestDungeons.contains(DungeonSceneLookupByArea(rcArea)))
                        ImGui::Text("(%d/%d) - MQ", areaChecksGotten[rcArea], areaChecksTotal);
                    else
                        ImGui::Text("(%d/%d) - Vanilla", areaChecksGotten[rcArea], areaChecksTotal);
                } else {
                    ImGui::Text("(%d/%d)", areaChecksGotten[rcArea], areaChecksTotal);
                }
            } else {
                ImGui::Text("???");
            }

            ImGui::PopStyleColor();

            //Keep areas loaded between transitions
            if (thisArea == rcArea && doAreaScroll) {
                ImGui::SetScrollHereY(0.0f);
                doAreaScroll = false;
            }
            for (auto rco : objs) {
                if (doDraw && isThisAreaSpoiled && IsVisibleInCheckTracker(rco))
                    DrawLocation(rco);
            }
            if (doDraw)
                ImGui::TreePop();
        }
    }

    ImGui::EndTable(); //Checks Lead-out
    ImGui::EndTable(); //Quick Options Lead-out
    EndFloatWindows();
    if (doingCollapseOrExpand) {
        optCollapseAll = false;
        optExpandAll = false;
    }
}

// Windowing stuff
void BeginFloatWindows(std::string UniqueName, bool& open, ImGuiWindowFlags flags) {
    ImGuiWindowFlags windowFlags = flags;

    if (windowFlags == 0) {
        windowFlags |=
            ImGuiWindowFlags_AlwaysVerticalScrollbar | ImGuiWindowFlags_NoFocusOnAppearing;
    }

    if (CVarGetInteger("gCheckTrackerWindowType", TRACKER_WINDOW_WINDOW) == TRACKER_WINDOW_FLOATING) {
        ImGui::SetNextWindowViewport(ImGui::GetMainViewport()->ID);
        windowFlags |= ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoTitleBar |
                       ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoScrollbar;

        if (!CVarGetInteger("gCheckTrackerHudEditMode", 0)) {
            windowFlags |= ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoMove;
        }
    }
    ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(Color_Background.r / 255.0f, Color_Background.g / 255.0f,
                                                    Color_Background.b / 255.0f, Color_Background.a / 255.0f));
    ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0, 0, 0, 0));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 4.0f);
    ImGui::Begin(UniqueName.c_str(), &open, windowFlags);
}
void EndFloatWindows() {
    ImGui::PopStyleVar();
    ImGui::PopStyleColor();
    ImGui::PopStyleColor();
    ImGui::End();
}

bool showShops;
bool showOverworldTokens;
bool showDungeonTokens;
bool showBeans;
bool showScrubs;
bool showMerchants;
bool showCows;
bool showAdultTrade;
bool showKokiriSword;
bool showWeirdEgg;
bool showGerudoCard;
bool showFrogSongRupees;
bool showStartingMapsCompasses;
bool showKeysanity;
bool showGerudoFortressKeys;
bool showBossKeysanity;
bool showGanonBossKey;
bool showOcarinas;
bool show100SkullReward;
bool fortressFast;
bool fortressNormal;

void LoadSettings() {
    //If in randomzer (n64ddFlag), then get the setting and check if in general we should be showing the settings
    //If in vanilla, _try_ to show items that at least are needed for 100%

    showShops = gSaveContext.n64ddFlag ? (
            OTRGlobals::Instance->gRandomizer->GetRandoSettingValue(RSK_SHOPSANITY) != RO_SHOPSANITY_OFF &&
            OTRGlobals::Instance->gRandomizer->GetRandoSettingValue(RSK_SHOPSANITY) != RO_SHOPSANITY_ZERO_ITEMS)
        : false;
    showBeans = gSaveContext.n64ddFlag ?
        OTRGlobals::Instance->gRandomizer->GetRandoSettingValue(RSK_SHUFFLE_MAGIC_BEANS) == RO_GENERIC_YES
        : true;
    showScrubs = gSaveContext.n64ddFlag ?
        OTRGlobals::Instance->gRandomizer->GetRandoSettingValue(RSK_SHUFFLE_SCRUBS) != RO_SCRUBS_OFF
        : false;
    showMerchants = gSaveContext.n64ddFlag ?
        OTRGlobals::Instance->gRandomizer->GetRandoSettingValue(RSK_SHUFFLE_MERCHANTS) != RO_SHUFFLE_MERCHANTS_OFF
        : true;
    showCows = gSaveContext.n64ddFlag ?
        OTRGlobals::Instance->gRandomizer->GetRandoSettingValue(RSK_SHUFFLE_COWS) == RO_GENERIC_YES
        : false;
    showAdultTrade = gSaveContext.n64ddFlag ?
        OTRGlobals::Instance->gRandomizer->GetRandoSettingValue(RSK_SHUFFLE_ADULT_TRADE) == RO_GENERIC_YES
        : true;
    showKokiriSword = gSaveContext.n64ddFlag ?
        OTRGlobals::Instance->gRandomizer->GetRandoSettingValue(RSK_SHUFFLE_KOKIRI_SWORD) == RO_GENERIC_YES
        : true;
    showWeirdEgg = gSaveContext.n64ddFlag ?
        OTRGlobals::Instance->gRandomizer->GetRandoSettingValue(RSK_SHUFFLE_WEIRD_EGG) == RO_GENERIC_YES
        : true;
    showGerudoCard = gSaveContext.n64ddFlag ?
        OTRGlobals::Instance->gRandomizer->GetRandoSettingValue(RSK_SHUFFLE_GERUDO_MEMBERSHIP_CARD) == RO_GENERIC_YES
        : true;
    showFrogSongRupees = gSaveContext.n64ddFlag ?
        OTRGlobals::Instance->gRandomizer->GetRandoSettingValue(RSK_SHUFFLE_FROG_SONG_RUPEES) == RO_GENERIC_YES
        : false;
    showStartingMapsCompasses = gSaveContext.n64ddFlag ?
        OTRGlobals::Instance->gRandomizer->GetRandoSettingValue(RSK_STARTING_MAPS_COMPASSES) != RO_DUNGEON_ITEM_LOC_VANILLA
        : false;
    showKeysanity = gSaveContext.n64ddFlag ?
        OTRGlobals::Instance->gRandomizer->GetRandoSettingValue(RSK_KEYSANITY) != RO_DUNGEON_ITEM_LOC_VANILLA
        : false;
    showBossKeysanity = gSaveContext.n64ddFlag ?
        OTRGlobals::Instance->gRandomizer->GetRandoSettingValue(RSK_BOSS_KEYSANITY) != RO_DUNGEON_ITEM_LOC_VANILLA
        : false;
    showGerudoFortressKeys = gSaveContext.n64ddFlag ?
        OTRGlobals::Instance->gRandomizer->GetRandoSettingValue(RSK_GERUDO_KEYS) != RO_GERUDO_KEYS_VANILLA
        : false;
    showGanonBossKey = gSaveContext.n64ddFlag ?
        OTRGlobals::Instance->gRandomizer->GetRandoSettingValue(RSK_GANONS_BOSS_KEY) != RO_GANON_BOSS_KEY_VANILLA
        : false;
    showOcarinas = gSaveContext.n64ddFlag ?
        OTRGlobals::Instance->gRandomizer->GetRandoSettingValue(RSK_SHUFFLE_OCARINA) == RO_GENERIC_YES
        : false;
    show100SkullReward = gSaveContext.n64ddFlag ?
        OTRGlobals::Instance->gRandomizer->GetRandoSettingValue(RSK_SHUFFLE_100_GS_REWARD) == RO_GENERIC_YES
        : false;
    hideShopRightChecks = gSaveContext.n64ddFlag ? CVarGetInteger("gCheckTrackerOptionHideRightShopChecks", 1) : false;

    if (gSaveContext.n64ddFlag) {
        switch (OTRGlobals::Instance->gRandomizer->GetRandoSettingValue(RSK_SHUFFLE_TOKENS)) {
            case RO_TOKENSANITY_ALL:
                showOverworldTokens = true;
                showDungeonTokens = true;
                break;
            case RO_TOKENSANITY_OVERWORLD:
                showOverworldTokens = true;
                showDungeonTokens = false;
                break;
            case RO_TOKENSANITY_DUNGEONS:
                showOverworldTokens = false;
                showDungeonTokens = true;
                break;
            default:
                showOverworldTokens = false;
                showDungeonTokens = false;
                break;
        }
    } else { // Vanilla
        showOverworldTokens = true;
        showDungeonTokens = true;
    }

    fortressFast = false;
    fortressNormal = false;
    switch (OTRGlobals::Instance->gRandomizer->GetRandoSettingValue(RSK_GERUDO_FORTRESS)) {
        case RO_GF_OPEN:
            showGerudoFortressKeys = false;
            showGerudoCard = false;
            break;
        case RO_GF_FAST:
            fortressFast = true;
            break;
        case RO_GF_NORMAL:
            fortressNormal = true;
            break;
    }
}

bool IsVisibleInCheckTracker(RandomizerCheckObject rcObj) {
    if (gSaveContext.n64ddFlag) {
        return
            (rcObj.rcArea != RCAREA_INVALID) &&         // don't show Invalid locations
            (rcObj.rcType != RCTYPE_GOSSIP_STONE) &&    //TODO: Don't show hints until tracker supports them
            (rcObj.rcType != RCTYPE_CHEST_GAME) &&      // don't show non final reward chest game checks until we support shuffling them
            (rcObj.rc != RC_HC_ZELDAS_LETTER) &&        // don't show zeldas letter until we support shuffling it
            (!RandomizerCheckObjects::AreaIsDungeon(rcObj.rcArea) ||
                rcObj.vOrMQ == RCVORMQ_BOTH ||
                rcObj.vOrMQ == RCVORMQ_MQ && OTRGlobals::Instance->gRandomizer->masterQuestDungeons.contains(rcObj.sceneId) ||
                rcObj.vOrMQ == RCVORMQ_VANILLA && !OTRGlobals::Instance->gRandomizer->masterQuestDungeons.contains(rcObj.sceneId)
                ) &&
            (rcObj.rcType != RCTYPE_SHOP || (showShops && (!hideShopRightChecks || hideShopRightChecks && rcObj.actorParams > 0x03))) &&
            (rcObj.rcType != RCTYPE_SCRUB ||
                showScrubs ||
                rcObj.rc == RC_LW_DEKU_SCRUB_NEAR_BRIDGE || // The 3 scrubs that are always randomized
                rcObj.rc == RC_HF_DEKU_SCRUB_GROTTO ||
                rcObj.rc == RC_LW_DEKU_SCRUB_GROTTO_FRONT
                ) &&
            (rcObj.rcType != RCTYPE_MERCHANT || showMerchants) &&
            (rcObj.rcType != RCTYPE_OCARINA || showOcarinas) &&
            (rcObj.rcType != RCTYPE_SKULL_TOKEN ||
                (showOverworldTokens && RandomizerCheckObjects::AreaIsOverworld(rcObj.rcArea)) ||
                (showDungeonTokens && RandomizerCheckObjects::AreaIsDungeon(rcObj.rcArea))
                ) &&
            (rcObj.rcType != RCTYPE_COW || showCows) &&
            (rcObj.rcType != RCTYPE_ADULT_TRADE ||
                showAdultTrade ||
                rcObj.rc == RC_KAK_ANJU_AS_ADULT ||  // adult trade checks that are always shuffled
                rcObj.rc == RC_DMT_TRADE_CLAIM_CHECK // even when shuffle adult trade is off
                ) &&
            (rcObj.rc != RC_KF_KOKIRI_SWORD_CHEST || showKokiriSword) &&
            (rcObj.rc != RC_ZR_MAGIC_BEAN_SALESMAN || showBeans) &&
            (rcObj.rc != RC_HC_MALON_EGG || showWeirdEgg) &&
            (rcObj.rcType != RCTYPE_FROG_SONG || showFrogSongRupees) &&
            (rcObj.rcType != RCTYPE_MAP_COMPASS || showStartingMapsCompasses) &&
            (rcObj.rcType != RCTYPE_SMALL_KEY || showKeysanity) &&
            (rcObj.rcType != RCTYPE_BOSS_KEY || showBossKeysanity) &&
            (rcObj.rcType != RCTYPE_GANON_BOSS_KEY || showGanonBossKey) &&
            (rcObj.rc != RC_KAK_100_GOLD_SKULLTULA_REWARD || show100SkullReward) &&
            (rcObj.rcType != RCTYPE_GF_KEY && rcObj.rc != RC_GF_GERUDO_MEMBERSHIP_CARD ||
                (showGerudoCard && rcObj.rc == RC_GF_GERUDO_MEMBERSHIP_CARD) ||
                (fortressNormal && showGerudoFortressKeys && rcObj.rcType == RCTYPE_GF_KEY) ||
                (fortressFast && showGerudoFortressKeys && rcObj.rc == RC_GF_NORTH_F1_CARPENTER)
                );
    }
    else if (rcObj.vanillaCheck) {
        return (rcObj.vOrMQ == RCVORMQ_BOTH ||
            rcObj.vOrMQ == RCVORMQ_MQ && OTRGlobals::Instance->gRandomizer->masterQuestDungeons.contains(rcObj.sceneId) ||
            rcObj.vOrMQ == RCVORMQ_VANILLA && !OTRGlobals::Instance->gRandomizer->masterQuestDungeons.contains(rcObj.sceneId) ||
            rcObj.rc == RC_GIFT_FROM_SAGES) && rcObj.rc != RC_LINKS_POCKET;
    }
    return false;
}

int slowCheckIdx = 0;

void UpdateInventoryChecks() {
    //For all the areas with compasses, if you have one, spoil the area
    for (u8 i = RCAREA_DEKU_TREE; i <= RCAREA_ICE_CAVERN; i++)
        if (gSaveContext.inventory.dungeonItems[i - RCAREA_DEKU_TREE] & 0x02)
            areasSpoiled |= (1 << i);
}

void UpdateAreaFullyChecked(RandomizerCheckArea area) {
}

void UpdateAreas(RandomizerCheckArea area) {
    areasFullyChecked[area] = areaChecksGotten[area] == checksByArea.find(area)->second.size();
    if (areaChecksGotten[area] != 0 || RandomizerCheckObjects::AreaIsOverworld(area))
        areasSpoiled |= (1 << area);
}

void UpdateAllOrdering() {
    // Sort the entire thing
    for (int i = 0; i < RCAREA_INVALID; i++) {
        UpdateOrdering(static_cast<RandomizerCheckArea>(i));
    }
}

void UpdateOrdering(RandomizerCheckArea rcArea) {
    // Sort a single area
    if(checksByArea.contains(rcArea)) {
        std::sort(checksByArea.find(rcArea)->second.begin(), checksByArea.find(rcArea)->second.end(), CompareChecks);
    }
}

bool IsEoDCheck(RandomizerCheckType type) {
    return type == RCTYPE_BOSS_HEART_OR_OTHER_REWARD || type == RCTYPE_DUNGEON_REWARD;
}

bool CompareChecks(RandomizerCheckObject i, RandomizerCheckObject j) {
    RandomizerCheckTrackerData iShow = gSaveContext.checkTrackerData[i.rc];
    RandomizerCheckTrackerData jShow = gSaveContext.checkTrackerData[j.rc];
    bool iCollected = iShow.status == RCSHOW_COLLECTED || iShow.status == RCSHOW_SAVED;
    bool iSaved = iShow.status == RCSHOW_SAVED;
    bool jCollected = jShow.status == RCSHOW_COLLECTED || jShow.status == RCSHOW_SAVED;
    bool jSaved = jShow.status == RCSHOW_SAVED;
    if (!iCollected && jCollected)
        return true;
    else if (iCollected && !jCollected)
        return false;

    if (!iSaved && jSaved)
        return true;
    else if (iSaved && !jSaved)
        return false;

    if (!iShow.skipped && jShow.skipped)
        return true;
    else if (iShow.skipped && !jShow.skipped)
        return false;

    if (!IsEoDCheck(i.rcType) && IsEoDCheck(j.rcType))
        return true;
    else if (IsEoDCheck(i.rcType) && !IsEoDCheck(j.rcType))
        return false;

    if (i.rc < j.rc)
        return true;
    else if (i.rc > j.rc)
        return false;

    return false;
}

bool IsHeartPiece(GetItemID giid) {
    return giid == GI_HEART_PIECE || giid == GI_HEART_PIECE_WIN;
}

void DrawLocation(RandomizerCheckObject rcObj) {
    Color_RGBA8 mainColor; 
    Color_RGBA8 extraColor;
    std::string txt;
    bool showHidden = CVarGetInteger("gCheckTrackerOptionShowHidden", 0);
    RandomizerCheckTrackerData checkData = gSaveContext.checkTrackerData[rcObj.rc];
    RandomizerCheckShow status = checkData.status;
    bool skipped = checkData.skipped;
    if (status == RCSHOW_COLLECTED) {
        if (!showHidden && CVarGetInteger("gCheckTrackerCollectedHide", 0))
            return;
        mainColor = !IsHeartPiece(rcObj.ogItemId) && !gSaveContext.n64ddFlag ? CVarGetColor("gCheckTrackerCollectedExtraColor", Color_Collected_Extra_Default) :
                  CVarGetColor("gCheckTrackerCollectedMainColor", Color_Main_Default);
        extraColor = CVarGetColor("gCheckTrackerCollectedExtraColor", Color_Collected_Extra_Default);
    } else if (status == RCSHOW_SAVED) {
        if (!showHidden && CVarGetInteger("gCheckTrackerSavedHide", 0))
            return;
         mainColor = !IsHeartPiece(rcObj.ogItemId) && !gSaveContext.n64ddFlag ? CVarGetColor("gCheckTrackerSavedExtraColor", Color_Saved_Extra_Default) :
                  CVarGetColor("gCheckTrackerSavedMainColor", Color_Main_Default);
        extraColor = CVarGetColor("gCheckTrackerSavedExtraColor", Color_Saved_Extra_Default);
    } else if (skipped) {
        if (!showHidden && CVarGetInteger("gCheckTrackerSkippedHide", 0))
            return;
         mainColor = !IsHeartPiece(rcObj.ogItemId) && !gSaveContext.n64ddFlag ? CVarGetColor("gCheckTrackerSkippedExtraColor", Color_Skipped_Extra_Default) :
                  CVarGetColor("gCheckTrackerSkippedMainColor", Color_Main_Default);
        extraColor = CVarGetColor("gCheckTrackerSkippedExtraColor", Color_Skipped_Extra_Default);
    } else if (status == RCSHOW_SEEN) {
        if (!showHidden && CVarGetInteger("gCheckTrackerSeenHide", 0))
            return;
         mainColor = !IsHeartPiece(rcObj.ogItemId) && !gSaveContext.n64ddFlag ? CVarGetColor("gCheckTrackerSeenExtraColor", Color_Seen_Extra_Default) :
                  CVarGetColor("gCheckTrackerSeenMainColor", Color_Main_Default);
        extraColor = CVarGetColor("gCheckTrackerSeenExtraColor", Color_Seen_Extra_Default);
    } else if (status == RCSHOW_SCUMMED) {
        if (!showHidden && CVarGetInteger("gCheckTrackerKnownHide", 0))
            return;
         mainColor = !IsHeartPiece(rcObj.ogItemId) && !gSaveContext.n64ddFlag ? CVarGetColor("gCheckTrackerScummedExtraColor", Color_Scummed_Extra_Default) :
                  CVarGetColor("gCheckTrackerScummedMainColor", Color_Main_Default);
        extraColor = CVarGetColor("gCheckTrackerScummedExtraColor", Color_Scummed_Extra_Default);
    } else if (status == RCSHOW_UNCHECKED) {
        if (!showHidden && CVarGetInteger("gCheckTrackerUncheckedHide", 0))
            return;
         mainColor = !IsHeartPiece(rcObj.ogItemId) && !gSaveContext.n64ddFlag ? CVarGetColor("gCheckTrackerUncheckedExtraColor", Color_Unchecked_Extra_Default) :
                  CVarGetColor("gCheckTrackerUncheckedMainColor", Color_Main_Default);
        extraColor = CVarGetColor("gCheckTrackerUncheckedExtraColor",  Color_Unchecked_Extra_Default);
    }
 
    //Main Text
    txt = rcObj.rcShortName;
    if (lastLocationChecked == rcObj.rc)
        txt = "* " + txt;
 
    // Draw button - for Skipped/Seen/Scummed/Unchecked only
    if (status == RCSHOW_UNCHECKED || status == RCSHOW_SEEN || status == RCSHOW_SCUMMED || skipped) {
        if (UIWidgets::StateButton(std::to_string(rcObj.rc).c_str(), skipped ? ICON_FA_PLUS : ICON_FA_TIMES)) {
            if (skipped) {
                gSaveContext.checkTrackerData[rcObj.rc].skipped = false;
                areaChecksGotten[rcObj.rcArea]--;
            } else {
                gSaveContext.checkTrackerData[rcObj.rc].skipped = true;
                areaChecksGotten[rcObj.rcArea]++;
            }
            UpdateOrdering(rcObj.rcArea);
            UpdateInventoryChecks();
            SaveManager::Instance->SaveSection(gSaveContext.fileNum, sectionId, true);
        }
    } else {
        ImGui::InvisibleButton("", ImVec2(20.0f, 10.0f));
    }
    ImGui::SameLine();

    //Draw
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(mainColor.r / 255.0f, mainColor.g / 255.0f, mainColor.b / 255.0f, mainColor.a / 255.0f));
    ImGui::Text("%s", txt.c_str());
    ImGui::PopStyleColor();

    //Draw the extra info
    txt = "";

    if (checkData.hintItem != 0) {
        // TODO hints
    } else if (status != RCSHOW_UNCHECKED) {
        switch (status) {
            case RCSHOW_SAVED:
            case RCSHOW_COLLECTED:
            case RCSHOW_SCUMMED:
                if (gSaveContext.n64ddFlag) {
                    txt = OTRGlobals::Instance->gRandomizer->EnumToSpoilerfileGetName[gSaveContext.itemLocations[rcObj.rc].get.rgID][gSaveContext.language];
                } else {
                    if (IsHeartPiece(rcObj.ogItemId)) {
                        if (gSaveContext.language == LANGUAGE_ENG || gSaveContext.language == LANGUAGE_GER) {
                            txt = ItemFromGIID(rcObj.ogItemId).GetName().english;
                        } else if (gSaveContext.language == LANGUAGE_FRA) {
                            txt = ItemFromGIID(rcObj.ogItemId).GetName().french;
                        }
                    }
                }
                break;
            case RCSHOW_SEEN:
                if (gSaveContext.n64ddFlag) {
                    txt = OTRGlobals::Instance->gRandomizer->EnumToSpoilerfileGetName[gSaveContext.itemLocations[rcObj.rc].get.fakeRgID][gSaveContext.language];
                } else {
                    if (IsHeartPiece(rcObj.ogItemId)) {
                        if (gSaveContext.language == LANGUAGE_ENG || gSaveContext.language == LANGUAGE_GER) {
                            txt = ItemFromGIID(rcObj.ogItemId).GetName().english;
                        } else if (gSaveContext.language == LANGUAGE_FRA) {
                            txt = ItemFromGIID(rcObj.ogItemId).GetName().french;
                        }
                    }
                }
            break;
        }
    }
    if (txt == "" && skipped)
        txt = "Skipped"; //TODO language

    if (txt != "") {
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(extraColor.r / 255.0f, extraColor.g / 255.0f, extraColor.b / 255.0f, extraColor.a / 255.0f));
        ImGui::SameLine();
        ImGui::Text(" (%s)", txt.c_str());
        ImGui::PopStyleColor();
    }

}

static std::set<std::string> rainbowCVars = {
    "gCheckTrackerAreaMainIncompleteColor", "gCheckTrackerAreaExtraIncompleteColor",
    "gCheckTrackerAreaMainCompleteColor",   "gCheckTrackerAreaExtraCompleteColor",
    "gCheckTrackerUncheckedMainColor",      "gCheckTrackerUncheckedExtraColor",
    "gCheckTrackerSkippedMainColor",        "gCheckTrackerSkippedExtraColor",
    "gCheckTrackerSeenMainColor",           "gCheckTrackerSeenExtraColor",
    "gCheckTrackerHintedMainColor",         "gCheckTrackerHintedExtraColor",
    "gCheckTrackerCollectedMainColor",      "gCheckTrackerCollectedExtraColor",
    "gCheckTrackerScummedMainColor",        "gCheckTrackerScummedExtraColor",
    "gCheckTrackerSavedMainColor",          "gCheckTrackerSavedExtraColor",
};

int hue = 0;
void RainbowTick() {
    float freqHue = hue * 2 * M_PI / (360 * CVarGetFloat("gCosmetics.RainbowSpeed", 0.6f));
    for (auto& cvar : rainbowCVars) {
        if (CVarGetInteger((cvar + "RBM").c_str(), 0) == 0)
            continue;
     
        Color_RGBA8 newColor;
        newColor.r = sin(freqHue +              0) * 127 + 128;
        newColor.g = sin(freqHue + (2 * M_PI / 3)) * 127 + 128;
        newColor.b = sin(freqHue + (4 * M_PI / 3)) * 127 + 128;
        newColor.a = 255;
         
        CVarSetColor(cvar.c_str(), newColor);
    }

    hue++;
    hue %= 360;
}

void ImGuiDrawTwoColorPickerSection(const char* text, const char* cvarMainName, const char* cvarExtraName,
                                 Color_RGBA8& main_color, Color_RGBA8& extra_color, Color_RGBA8& main_default_color,
                                 Color_RGBA8& extra_default_color, const char* cvarHideName) {
    Color_RGBA8 cvarMainColor = CVarGetColor(cvarMainName, main_default_color);
    Color_RGBA8 cvarExtraColor = CVarGetColor(cvarExtraName, extra_default_color);
    main_color = cvarMainColor;
    extra_color = cvarExtraColor;

    if (ImGui::CollapsingHeader(text)) {
        if (*cvarHideName != '\0') {
            std::string label = cvarHideName;
            label += "##Hidden";
            ImGui::PushID(label.c_str());
            UIWidgets::EnhancementCheckbox("Hidden", cvarHideName, false,
                                           "When active, checks will hide by default when updated to this state. Can "
                                           "be overriden with the \"Show Hidden Items\" option.");
            ImGui::PopID();
        }
        if (ImGui::BeginTable(text, 2, ImGuiTableFlags_BordersH | ImGuiTableFlags_BordersV | ImGuiTableFlags_Hideable)) {
            ImGui::TableNextRow();
            ImGui::TableNextColumn();
            ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x);
            if (UIWidgets::EnhancementColor("Check", cvarMainName,
                ImVec4(main_color.r, main_color.g, main_color.b, main_color.a),
                ImVec4(main_default_color.r, main_default_color.g, main_default_color.b, main_default_color.a)))
            {
                main_color = CVarGetColor(cvarMainName, main_default_color);
            };
            ImGui::PopItemWidth();
         
            ImGui::TableNextColumn();
            ImGui::AlignTextToFramePadding();
            ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x);
            if (UIWidgets::EnhancementColor("Details", cvarExtraName,
                ImVec4(extra_color.r, extra_color.g, extra_color.b, extra_color.a),
                ImVec4(extra_default_color.r, extra_default_color.g, extra_default_color.b, extra_default_color.a)))
            {
                extra_color = CVarGetColor(cvarExtraName, extra_default_color);
            }
            ImGui::PopItemWidth();

            ImGui::EndTable();
        }
    }
}

static const char* windowType[] = { "Floating", "Window" };
static const char* displayType[] = { "Always", "Combo Button Hold" };
static const char* buttonStrings[] = { "A Button", "B Button", "C-Up",  "C-Down", "C-Left", "C-Right", "L Button",
                                       "Z Button", "R Button", "Start", "D-Up",   "D-Down", "D-Left",  "D-Right" };
void CheckTrackerSettingsWindow::DrawElement() {
    ImGui::SetNextWindowSize(ImVec2(600, 375), ImGuiCond_FirstUseEver);

    if (!ImGui::Begin("Check Tracker Settings", &mIsVisible, ImGuiWindowFlags_NoFocusOnAppearing)) {
        ImGui::End();
        return;
    }

    ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, { 8.0f, 8.0f });
    ImGui::BeginTable("CheckTrackerSettingsTable", 2, ImGuiTableFlags_BordersH | ImGuiTableFlags_BordersV);
    ImGui::TableSetupColumn("General settings", ImGuiTableColumnFlags_WidthStretch, 200.0f);
    ImGui::TableSetupColumn("Section settings", ImGuiTableColumnFlags_WidthStretch, 200.0f);
    ImGui::TableHeadersRow();
    ImGui::TableNextRow();
    ImGui::TableNextColumn();
    ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x);
    if (UIWidgets::EnhancementColor("BG Color", "gCheckTrackerBgColor",
        ImVec4(Color_Background.r, Color_Background.g, Color_Background.b, Color_Background.a),
        ImVec4(Color_Bg_Default.r, Color_Bg_Default.g, Color_Bg_Default.b, Color_Bg_Default.a),
        false, true))
    {
        Color_Background = CVarGetColor("gCheckTrackerBgColor", Color_Bg_Default);
    }
    ImGui::PopItemWidth();

    UIWidgets::LabeledRightAlignedEnhancementCombobox("Window Type", "gCheckTrackerWindowType", windowType, TRACKER_WINDOW_WINDOW);
    if (CVarGetInteger("gCheckTrackerWindowType", TRACKER_WINDOW_WINDOW) == TRACKER_WINDOW_FLOATING) {
        UIWidgets::EnhancementCheckbox("Enable Dragging", "gCheckTrackerHudEditMode");
        UIWidgets::EnhancementCheckbox("Only enable while paused", "gCheckTrackerShowOnlyPaused");
        UIWidgets::LabeledRightAlignedEnhancementCombobox("Display Mode", "gCheckTrackerDisplayType", displayType, 0);
        if (CVarGetInteger("gCheckTrackerDisplayType", TRACKER_DISPLAY_ALWAYS) == TRACKER_DISPLAY_COMBO_BUTTON) {
            UIWidgets::LabeledRightAlignedEnhancementCombobox("Combo Button 1", "gCheckTrackerComboButton1", buttonStrings, TRACKER_COMBO_BUTTON_L);
            UIWidgets::LabeledRightAlignedEnhancementCombobox("Combo Button 2", "gCheckTrackerComboButton2", buttonStrings, TRACKER_COMBO_BUTTON_R);
        }
    }
    UIWidgets::EnhancementCheckbox("Vanilla/MQ Dungeon Spoilers", "gCheckTrackerOptionMQSpoilers");
    UIWidgets::Tooltip("If enabled, Vanilla/MQ dungeons will show on the tracker immediately. Otherwise, Vanilla/MQ dungeon locations must be unlocked.");
    UIWidgets::EnhancementCheckbox("Hide right-side shop item checks", "gCheckTrackerOptionHideRightShopChecks");
    UIWidgets::Tooltip("If enabled, will prevent the tracker from displaying slots 1-4 in all shops. Requires save reload.");

    ImGui::TableNextColumn();

    ImGuiDrawTwoColorPickerSection("Area Incomplete",  "gCheckTrackerAreaMainIncompleteColor",   "gCheckTrackerAreaExtraIncompleteColor",  Color_Area_Incomplete_Main,   Color_Area_Incomplete_Extra,  Color_Main_Default, Color_Area_Incomplete_Extra_Default, "gCheckTrackerAreaIncompleteHide" );
    ImGuiDrawTwoColorPickerSection("Area Complete",    "gCheckTrackerAreaMainCompleteColor",     "gCheckTrackerAreaExtraCompleteColor",    Color_Area_Complete_Main,     Color_Area_Complete_Extra,    Color_Main_Default, Color_Area_Complete_Extra_Default,   "gCheckTrackerAreaCompleteHide"   );
    ImGuiDrawTwoColorPickerSection("Unchecked",        "gCheckTrackerUncheckedMainColor",        "gCheckTrackerUncheckedExtraColor",       Color_Unchecked_Main,         Color_Unchecked_Extra,        Color_Main_Default, Color_Unchecked_Extra_Default,       "gCheckTrackerUncheckedHide"      );
    ImGuiDrawTwoColorPickerSection("Skipped",          "gCheckTrackerSkippedMainColor",          "gCheckTrackerSkippedExtraColor",         Color_Skipped_Main,           Color_Skipped_Extra,          Color_Main_Default, Color_Skipped_Extra_Default,         "gCheckTrackerSkippedHide"        );
    ImGuiDrawTwoColorPickerSection("Seen (WIP)",       "gCheckTrackerSeenMainColor",             "gCheckTrackerSeenExtraColor",            Color_Seen_Main,              Color_Seen_Extra,             Color_Main_Default, Color_Seen_Extra_Default,            "gCheckTrackerSeenHide"           );
    ImGuiDrawTwoColorPickerSection("Scummed",          "gCheckTrackerScummedMainColor",          "gCheckTrackerScummedExtraColor",         Color_Scummed_Main,           Color_Scummed_Extra,          Color_Main_Default, Color_Scummed_Extra_Default,         "gCheckTrackerScummedHide"        );
    ImGuiDrawTwoColorPickerSection("Hinted (WIP)",     "gCheckTrackerHintedMainColor",           "gCheckTrackerHintedExtraColor",          Color_Hinted_Main,            Color_Hinted_Extra,           Color_Main_Default, Color_Hinted_Extra_Default,          "gCheckTrackerHintedHide"         );
    ImGuiDrawTwoColorPickerSection("Collected",        "gCheckTrackerCollectedMainColor",        "gCheckTrackerCollectedExtraColor",       Color_Collected_Main,         Color_Collected_Extra,        Color_Main_Default, Color_Collected_Extra_Default,       "gCheckTrackerCollectedHide"      );
    ImGuiDrawTwoColorPickerSection("Saved",            "gCheckTrackerSavedMainColor",            "gCheckTrackerSavedExtraColor",           Color_Saved_Main,             Color_Saved_Extra,            Color_Main_Default, Color_Saved_Extra_Default,           "gCheckTrackerSavedHide"          );

    ImGui::PopStyleVar(1);
    ImGui::EndTable();
    ImGui::End();
}

void CheckTrackerWindow::InitElement() {
    Color_Background            = CVarGetColor("gCheckTrackerBgColor",                    Color_Bg_Default);
    Color_Area_Incomplete_Main  = CVarGetColor("gCheckTrackerAreaMainIncompleteColor",    Color_Main_Default);
    Color_Area_Incomplete_Extra = CVarGetColor("gCheckTrackerAreaExtraIncompleteColor",   Color_Area_Incomplete_Extra_Default);
    Color_Area_Complete_Main    = CVarGetColor("gCheckTrackerAreaMainCompleteColor",      Color_Main_Default);
    Color_Area_Complete_Extra   = CVarGetColor("gCheckTrackerAreaExtraCompleteColor",     Color_Area_Complete_Extra_Default);
    Color_Unchecked_Main        = CVarGetColor("gCheckTrackerUncheckedMainColor",         Color_Main_Default);
    Color_Unchecked_Extra       = CVarGetColor("gCheckTrackerUncheckedExtraColor",        Color_Unchecked_Extra_Default);
    Color_Skipped_Main          = CVarGetColor("gCheckTrackerSkippedMainColor",           Color_Main_Default);
    Color_Skipped_Extra         = CVarGetColor("gCheckTrackerSkippedExtraColor",          Color_Skipped_Extra_Default);
    Color_Seen_Main             = CVarGetColor("gCheckTrackerSeenMainColor",              Color_Main_Default);
    Color_Seen_Extra            = CVarGetColor("gCheckTrackerSeenExtraColor",             Color_Seen_Extra_Default);
    Color_Hinted_Main           = CVarGetColor("gCheckTrackerHintedMainColor",            Color_Main_Default);
    Color_Hinted_Extra          = CVarGetColor("gCheckTrackerHintedExtraColor",           Color_Hinted_Extra_Default);
    Color_Collected_Main        = CVarGetColor("gCheckTrackerCollectedMainColor",         Color_Main_Default);
    Color_Collected_Extra       = CVarGetColor("gCheckTrackerCollectedExtraColor",        Color_Collected_Extra_Default);
    Color_Scummed_Main          = CVarGetColor("gCheckTrackerScummedMainColor",           Color_Main_Default);
    Color_Scummed_Extra         = CVarGetColor("gCheckTrackerScummedExtraColor",          Color_Scummed_Extra_Default);
    Color_Saved_Main            = CVarGetColor("gCheckTrackerSavedMainColor",             Color_Main_Default);
    Color_Saved_Extra           = CVarGetColor("gCheckTrackerSavedExtraColor",            Color_Saved_Extra_Default);

    SaveManager::Instance->AddInitFunction(InitTrackerData);
    sectionId = SaveManager::Instance->AddSaveFunction("trackerData", 1, SaveFile, true, -1);
    SaveManager::Instance->AddLoadFunction("trackerData", 1, LoadFile);
    GameInteractor::Instance->RegisterGameHook<GameInteractor::OnLoadFile>([](uint32_t fileNum) {
        doInitialize = true;
    });
    GameInteractor::Instance->RegisterGameHook<GameInteractor::OnDeleteFile>([](uint32_t fileNum) { Teardown(); });
    GameInteractor::Instance->RegisterGameHook<GameInteractor::OnItemReceive>(CheckTrackerItemReceive);
    GameInteractor::Instance->RegisterGameHook<GameInteractor::OnSaleEnd>(CheckTrackerSaleEnd);
    GameInteractor::Instance->RegisterGameHook<GameInteractor::OnGameFrameUpdate>(CheckTrackerFrame);
    GameInteractor::Instance->RegisterGameHook<GameInteractor::OnTransitionEnd>(CheckTrackerTransition);

    LocationTable_Init();
}

} // namespace CheckTracker

#endif