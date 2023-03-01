#include "randomizer_check_tracker.h"
#include "randomizerTypes.h"
#include "../../OTRGlobals.h"
#include <ImGuiImpl.h>
#include "../../UIWidgets.hpp"

#include <string>
#include <vector>
#include <set>
#include <libultraship/bridge.h>
#include <Hooks.h>
#include "3drando/item_location.hpp"
#include "soh/Enhancements/game-interactor/GameInteractor.h"

extern "C" {
#include "variables.h"
#include "functions.h"
#include "macros.h"
extern PlayState* gPlayState;
}
extern "C" uint32_t ResourceMgr_IsSceneMasterQuest(s16 sceneNum);

using json = nlohmann::json;

namespace CheckTracker {

void Teardown();
void InitializeChecks();
void UpdateChecks();
void UpdateInventoryChecks();
void DrawLocation(RandomizerCheckObject rcObj);
void BeginFloatWindows(std::string UniqueName, bool& open, ImGuiWindowFlags flags = 0);
void EndFloatWindows();
void UpdateOrdering();
void UpdateOrdering(RandomizerCheckArea rcArea);
bool ShouldUpdateChecks();
bool CompareCheckObject(RandomizerCheckObject i, RandomizerCheckObject j);
bool HasItemBeenCollected(RandomizerCheckObject obj);
void RainbowTick();


Color_RGBA8 Color_Bg_Default                        = {   0,   0,   0, 255 };   // Black
Color_RGBA8 Color_Main_Default                      = { 255, 255, 255, 255 };   // White
Color_RGBA8 Color_Area_Incomplete_Extra_Default     = { 255, 255, 255, 255 };   // White
Color_RGBA8 Color_Area_Complete_Extra_Default       = { 255, 255, 255, 255 };   // White
Color_RGBA8 Color_Unchecked_Extra_Default           = { 255, 255, 255, 255 };   // White
Color_RGBA8 Color_Skipped_Main_Default              = { 160, 160, 160, 255 };   // Grey
Color_RGBA8 Color_Skipped_Extra_Default             = { 160, 160, 160, 255 };   // Grey
Color_RGBA8 Color_Seen_Extra_Default                = { 255, 255, 255, 255 };   // TODO
Color_RGBA8 Color_Hinted_Extra_Default              = { 255, 255, 255, 255 };   // TODO
Color_RGBA8 Color_Checked_Extra_Default             = { 255, 255, 255, 255 };   // TODO
Color_RGBA8 Color_Scummed_Extra_Default             = { 255, 255, 255, 255 };   // TODO
Color_RGBA8 Color_Saved_Extra_Default               = {   0, 185,   0, 255 };   // Green

Color_RGBA8 Color_Background = { 0, 0, 0, 255 };

Color_RGBA8 Color_Area_Incomplete_Main  = { 255, 255, 255, 255 }; //White
Color_RGBA8 Color_Area_Incomplete_Extra = { 255, 255, 255, 255 }; //White
Color_RGBA8 Color_Area_Complete_Main    = { 255, 255, 255, 255 }; // White
Color_RGBA8 Color_Area_Complete_Extra   = { 255, 255, 255, 255 }; // White
Color_RGBA8 Color_Unchecked_Main     = { 255, 255, 255, 255 }; //White
Color_RGBA8 Color_Unchecked_Extra    = { 255, 255, 255, 255 }; //Useless
Color_RGBA8 Color_Skipped_Main       = { 255, 255, 255, 255 }; //Grey
Color_RGBA8 Color_Skipped_Extra      = { 255, 255, 255, 255 }; //Grey
Color_RGBA8 Color_Seen_Main          = { 255, 255, 255, 255 }; //TODO
Color_RGBA8 Color_Seen_Extra         = { 255, 255, 255, 255 }; //TODO
Color_RGBA8 Color_Hinted_Main        = { 255, 255, 255, 255 }; //TODO
Color_RGBA8 Color_Hinted_Extra       = { 255, 255, 255, 255 }; //TODO
Color_RGBA8 Color_Checked_Main       = { 255, 255, 255, 255 }; //TODO
Color_RGBA8 Color_Checked_Extra      = { 255, 255, 255, 255 }; //TODO
Color_RGBA8 Color_Scummed_Main       = { 255, 255, 255, 255 }; //TODO
Color_RGBA8 Color_Scummed_Extra      = { 255, 255, 255, 255 }; //TODO
Color_RGBA8 Color_Saved_Main         = { 255, 255, 255, 255 }; //White
Color_RGBA8 Color_Saved_Extra        = {   0, 185,   0, 255 }; //Green

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

// persistent during gameplay
bool initialized = false;
bool doInitialize = false;
std::map<RandomizerCheck, RandomizerCheckShow> checkStatusMap;
std::map<RandomizerCheck, RandomizerCheckTrackerData> checkTrackerData;
std::map<RandomizerCheckArea, std::vector<RandomizerCheckObject>> checkObjectsByArea;
bool areasFullyChecked[RCAREA_INVALID];
u32 areasSpoiled = 0;
bool showVOrMQ;
s8 areaChecksGotten[32];  //|     "Kokiri Forest (4/9)"
std::vector<RandomizerCheckObject> checks;
bool optCollapseAll; // A bool that will collapse all checks once
bool optExpandAll;   // A bool that will expand all checks once
RandomizerCheck lastLocationChecked = RC_UNKNOWN_CHECK;
RandomizerCheckArea previousArea = RCAREA_INVALID;
RandomizerCheckArea currentArea = RCAREA_INVALID;
OSContPad* trackerButtonsPressed;

std::vector<uint32_t> buttons = { BTN_A, BTN_B, BTN_CUP,   BTN_CDOWN, BTN_CLEFT, BTN_CRIGHT, BTN_L,
                                  BTN_Z, BTN_R, BTN_START, BTN_DUP,   BTN_DDOWN, BTN_DLEFT,  BTN_DRIGHT };

std::map<RandomizerCheck, RandomizerCheckTrackerData> *GetCheckTrackerData() {
    return &checkTrackerData;
}

void PushDefaultCheckData(RandomizerCheck rc) {
    RandomizerCheckTrackerData newData;
    newData.rc = rc;
    newData.status = RCSHOW_UNCHECKED;
    newData.skipped = false;
    newData.hintItem = RC_UNKNOWN_CHECK;

    checkTrackerData.emplace(rc, newData);
}

// Function for adding Link's Pocket check
void LinksPocket() {
    PushDefaultCheckData(RC_LINKS_POCKET);
    checkTrackerData.find(RC_LINKS_POCKET)->second.status = RCSHOW_SAVED;
    areaChecksGotten
}

void TrySetAreas() {
    if (checkObjectsByArea.empty()) {
        for (int i = RCAREA_KOKIRI_FOREST; i < RCAREA_INVALID; i++) {
            checkObjectsByArea.emplace(static_cast<RandomizerCheckArea>(i), std::vector<RandomizerCheckObject>());
        }
    }
}

void CreateTrackerData() {
    TrySetAreas();
    for (auto& [rc, rco] : RandomizerCheckObjects::GetAllRCObjects()) {
        if (rc != RC_UNKNOWN_CHECK && rc != RC_MAX && rc != RC_LINKS_POCKET) {
            PushDefaultCheckData(rc);
        }
    }
    LinksPocket();
    UpdateOrdering();
    UpdateInventoryChecks();
}

void LoadCheckTrackerData(json checks) {
    TrySetAreas();
    InitializeChecks();
    for (auto& item : checks) {
        if (item["rc"] == RC_UNKNOWN_CHECK || item["rc"] == RC_MAX || item["rc"] == RC_LINKS_POCKET)
            continue;
        RandomizerCheckTrackerData entry;
        entry.rc = item["rc"];
        entry.status = item["status"];
        entry.skipped = item["skipped"];
        entry.hintItem = item["hintItem"];
        RandomizerCheckObject entry2 = RandomizerCheckObjects::GetAllRCObjects().find(entry.rc)->second;
        checkTrackerData.emplace(entry.rc, entry);
        checkObjectsByArea.find(entry2.rcArea)->second.push_back(entry2);
        if (entry.status == RCSHOW_SAVED)
            areaChecksGotten[entry2.rcArea]++;
    }
    LinksPocket();
    UpdateOrdering();
    UpdateInventoryChecks();
}

void DrawCheckTracker(bool& open) {
    if (!open) {
        CVarSetInteger("gCheckTrackerEnabled", 0);
        return;
    }

    ImGui::SetNextWindowSize(ImVec2(400, 540), ImGuiCond_FirstUseEver);

    if (doInitialize) {
        Teardown();
        InitializeChecks();
        UpdateOrdering();
    } else if (initialized && (gPlayState == nullptr || gSaveContext.fileNum < 0 || gSaveContext.fileNum > 2)) {
        Teardown();
        return;
    }

    if (CVarGetInteger("gCheckTrackerWindowType", 1) == 0) {
        if (CVarGetInteger("gCheckTrackerShowOnlyPaused", 0) == 1)
            if (gPlayState == nullptr || gPlayState->pauseCtx.state == 0)
                return;

        if (CVarGetInteger("gCheckTrackerDisplayType", 0) == 1) {
            int comboButton1Mask = buttons[CVarGetInteger("gCheckTrackerComboButton1", 6)];
            int comboButton2Mask = buttons[CVarGetInteger("gCheckTrackerComboButton2", 8)];
            bool comboButtonsHeld = trackerButtonsPressed != nullptr &&
                                    trackerButtonsPressed[0].button & comboButton1Mask &&
                                    trackerButtonsPressed[0].button & comboButton2Mask;
            if (!comboButtonsHeld)
                return;
        }
    }

    BeginFloatWindows("Check Tracker", open, ImGuiWindowFlags_NoScrollbar);

    if (!initialized) {
        ImGui::Text("Waiting for file load..."); //TODO Language
        EndFloatWindows();
        return;
    }

    SceneID sceneId = SCENE_ID_MAX;
    if (gPlayState != nullptr) {
        sceneId = (SceneID)gPlayState->sceneNum;
        currentArea = RandomizerCheckObjects::GetRCAreaBySceneID(sceneId);
    }

    bool doAreaScroll =
     (currentArea != RCAREA_INVALID && currentArea != previousArea &&
         sceneId != SCENE_KAKUSIANA && // Don't move for grottos
         sceneId != SCENE_YOUSEI_IZUMI_TATE && sceneId != SCENE_YOUSEI_IZUMI_YOKO && sceneId != SCENE_DAIYOUSEI_IZUMI  && // Don't move for fairy fountains
         sceneId != SCENE_SHOP1 && sceneId != SCENE_SYATEKIJYOU // Don't move for Bazaar/Gallery, as it moves between Kak and Market
         );
    previousArea = currentArea;
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
    ImGui::SameLine();
    if (ImGui::Button("Expand All")) {
        optCollapseAll = false;
        optExpandAll = true;
    }
    ImGui::SameLine();
    if (ImGui::Button("Collapse All")) {
        optExpandAll = false;
        optCollapseAll = true;
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

    for (auto& [rcArea, rcObjects] : checkObjectsByArea) {
        const int areaChecksTotal = static_cast<int>(rcObjects.size());
        thisAreaFullyChecked = (areaChecksGotten[rcArea] == areaChecksTotal);
        //Last Area needs to be cleaned up
        if (lastArea != RCAREA_INVALID && doDraw) {
            UIWidgets::PaddedSeparator();
        }
        lastArea = rcArea;
        if (!showHidden && (
            hideComplete && thisAreaFullyChecked || 
            hideIncomplete && !thisAreaFullyChecked
        )) {
            doDraw = false;
        }
        else {
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
            if (currentArea == rcArea && doAreaScroll)
                ImGui::SetScrollHereY(0.0f);
            for (auto& obj : rcObjects) {
                if (doDraw && isThisAreaSpoiled)
                    DrawLocation(obj);
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

    if (!CVarGetInteger("gCheckTrackerWindowType", 1)) {
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
        (rcObj.rcType != RCTYPE_SHOP                || showShops) &&
        (rcObj.rcType != RCTYPE_SCRUB ||
            showScrubs || 
            rcObj.rc == RC_LW_DEKU_SCRUB_NEAR_BRIDGE || // The 3 scrubs that are always randomized
            rcObj.rc == RC_HF_DEKU_SCRUB_GROTTO ||
            rcObj.rc == RC_LW_DEKU_SCRUB_GROTTO_FRONT
        ) &&
        (rcObj.rcType != RCTYPE_MERCHANT            || showMerchants) &&
        (rcObj.rcType != RCTYPE_OCARINA             || showOcarinas) &&
        (rcObj.rcType != RCTYPE_SKULL_TOKEN ||
            (showOverworldTokens && RandomizerCheckObjects::AreaIsOverworld(rcObj.rcArea)) ||
            (showDungeonTokens && RandomizerCheckObjects::AreaIsDungeon(rcObj.rcArea))
        ) &&
        (rcObj.rcType != RCTYPE_COW                 || showCows) &&
        (rcObj.rcType != RCTYPE_ADULT_TRADE ||
            showAdultTrade ||
            rcObj.rc == RC_KAK_ANJU_AS_ADULT ||  // adult trade checks that are always shuffled
            rcObj.rc == RC_DMT_TRADE_CLAIM_CHECK // even when shuffle adult trade is off
        ) &&
        (rcObj.rc != RC_KF_KOKIRI_SWORD_CHEST         || showKokiriSword) &&
        (rcObj.rc != RC_ZR_MAGIC_BEAN_SALESMAN        || showBeans) &&
        (rcObj.rc != RC_HC_MALON_EGG                  || showWeirdEgg) &&
        (rcObj.rcType != RCTYPE_FROG_SONG             || showFrogSongRupees) &&
        (rcObj.rcType != RCTYPE_MAP_COMPASS           || showStartingMapsCompasses) &&
        (rcObj.rcType != RCTYPE_SMALL_KEY             || showKeysanity) &&
        (rcObj.rcType != RCTYPE_BOSS_KEY              || showBossKeysanity) &&
        (rcObj.rcType != RCTYPE_GANON_BOSS_KEY        || showGanonBossKey) &&
        (rcObj.rc != RC_KAK_100_GOLD_SKULLTULA_REWARD || show100SkullReward) &&
        (rcObj.rcType != RCTYPE_GF_KEY && rcObj.rc != RC_GF_GERUDO_MEMBERSHIP_CARD ||
            (showGerudoCard && rcObj.rc == RC_GF_GERUDO_MEMBERSHIP_CARD) ||
            (fortressNormal && showGerudoFortressKeys && rcObj.rcType == RCTYPE_GF_KEY) ||
            (fortressFast && showGerudoFortressKeys && rcObj.rc == RC_GF_NORTH_F1_CARPENTER)
        );
}

void InitializeChecks() {
    if (gPlayState == nullptr || gSaveContext.fileNum < 0 || gSaveContext.fileNum > 2)
        return;

    LoadSettings();
    for (auto& [rcCheck, rcObj] : RandomizerCheckObjects::GetAllRCObjects()) {
        if (!IsVisibleInCheckTracker(rcObj))
            continue;

        checks.push_back(rcObj);
        checkObjectsByArea.find(rcObj.rcArea)->second.push_back(rcObj);

        if (areaChecksGotten[rcObj.rcArea] != 0 || RandomizerCheckObjects::AreaIsOverworld(rcObj.rcArea))
            areasSpoiled |= (1 << rcObj.rcArea);
    }
    //Link's Pocket
    if (OTRGlobals::Instance->gRandomizer->GetRandoSettingValue(RSK_LINKS_POCKET) != RO_LINKS_POCKET_NOTHING) {
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

        checks.push_back(linksPocket);
        checkObjectsByArea.find(startingArea)->second.push_back(linksPocket);
        areaChecksGotten[startingArea]++;
    }

    showVOrMQ = (OTRGlobals::Instance->gRandomizer->GetRandoSettingValue(RSK_MQ_DUNGEON_COUNT) > 0);
    //Bug: the above will spoil that everything is vanilla if the random count rolled 0.
    // Should use the below instead, but the setting isn't currently saved to the savefile
    //showVOrMQ = (OTRGlobals::Instance->gRandomizer->GetRandoSettingValue(RSK_RANDOM_MQ_DUNGEONS) != RO_GENERIC_OFF);
    
    UpdateInventoryChecks();
    doInitialize = false;
    initialized = true;
}

void Teardown() {
    initialized = false;
    checkStatusMap.clear();
    areasSpoiled = 0;
    checks.clear();
    lastLocationChecked = RC_UNKNOWN_CHECK;
    for (auto& [rcArea, vec] : checkObjectsByArea) {
        vec.clear();
        areaChecksGotten[rcArea] = 0;
    }
    doInitialize = true;
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

void UpdateChecks(RandomizerCheckArea area) {
    areasFullyChecked[area] = areaChecksGotten[area] == checkObjectsByArea.find(area)->second.size();
    if (areaChecksGotten[area] != 0 || RandomizerCheckObjects::AreaIsOverworld(area))
        areasSpoiled |= (1 << area);
}

void UpdateOrdering() {
    // Sort the entire thing
    for (int i = 0; i < RCAREA_INVALID; i++) {
        UpdateOrdering(static_cast<RandomizerCheckArea>(i));
    }
}

void UpdateOrdering(RandomizerCheckArea rcArea) {
    // Sort a single area
    std::sort(checkObjectsByArea.find(rcArea)->second.begin(), checkObjectsByArea.find(rcArea)->second.end(), CompareCheckObject);
}

bool CompareCheckObject(RandomizerCheckObject i, RandomizerCheckObject j) {
    RandomizerCheckTrackerData iShow = checkTrackerData.find(i.rc)->second;
    RandomizerCheckTrackerData jShow = checkTrackerData.find(j.rc)->second;
    bool iCollected = iShow.status == RCSHOW_COLLECTED || iShow.status == RCSHOW_SAVED;
    bool jCollected = jShow.status == RCSHOW_COLLECTED || jShow.status == RCSHOW_SAVED;
    if (!iCollected && jCollected)
        return true;
    else if (iCollected && !jCollected)
        return false;

    if (!iShow.skipped && jShow.skipped)
        return true;
    else if (iShow.skipped && !jShow.skipped)
        return false;

    if (i.rc < j.rc)
        return true;
    else if (i.rc > j.rc)
        return false;

    return false;
}

bool HasItemBeenCollected(RandomizerCheckObject obj) {
    ItemLocation* x = Location(obj.rc);
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
            return gSaveContext.sceneFlags[scene].chest & (1 << flag);
        case SpoilerCollectionCheckType::SPOILER_CHK_COLLECTABLE:
            return gSaveContext.sceneFlags[scene].collect & (1 << flag);
        case SpoilerCollectionCheckType::SPOILER_CHK_MERCHANT:
        case SpoilerCollectionCheckType::SPOILER_CHK_SHOP_ITEM:
        case SpoilerCollectionCheckType::SPOILER_CHK_COW:
        case SpoilerCollectionCheckType::SPOILER_CHK_SCRUB:
        case SpoilerCollectionCheckType::SPOILER_CHK_RANDOMIZER_INF:
            return Flags_GetRandomizerInf(OTRGlobals::Instance->gRandomizer->GetRandomizerInfFromCheck(obj.rc));
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
            if (obj.rc == RC_LH_CHILD_FISHING)
                return HIGH_SCORE(HS_FISHING) & 0x400;
            if (obj.rc == RC_LH_ADULT_FISHING)
                return HIGH_SCORE(HS_FISHING) & 0x800;
        case SpoilerCollectionCheckType::SPOILER_CHK_NONE:
            return false;
        case SpoilerCollectionCheckType::SPOILER_CHK_POE_POINTS:
            return gSaveContext.highScores[HS_POE_POINTS] >= 1000;
        case SpoilerCollectionCheckType::SPOILER_CHK_GRAVEDIGGER:
            // Gravedigger has a fix in place that means one of two save locations. Check both.
            return (gSaveContext.itemGetInf[1] & 0x1000) || // vanilla flag
                   ((gSaveContext.n64ddFlag || CVarGetInteger("gGravediggingTourFix", 0)) &&
                        gSaveContext.sceneFlags[scene].collect & (1 << flag)); // rando/fix flag
        default:
            return false;
    }
    return false;
}

void DrawLocation(RandomizerCheckObject rcObj) {
    Color_RGBA8 mainColor;
    Color_RGBA8 extraColor;
    std::string txt;
    bool showHidden = CVarGetInteger("gCheckTrackerOptionShowHidden", 0);
    if(!checkTrackerData.contains(rcObj.rc)) {
        PushDefaultCheckData(rcObj.rc);
    }
    RandomizerCheckTrackerData checkData = checkTrackerData.find(rcObj.rc)->second;
    RandomizerCheckShow status = checkData.status;
    bool skipped = checkData.skipped;
    if (skipped) {
        if (!showHidden && CVarGetInteger("gCheckTrackerSkippedHide", 0))
            return;
        mainColor = CVarGetColor("gCheckTrackerSkippedMainColor", Color_Skipped_Main_Default);
        extraColor = CVarGetColor("gCheckTrackerSkippedExtraColor", Color_Skipped_Extra_Default);
    } else if (status == RCSHOW_UNCHECKED) {
        if (!showHidden && CVarGetInteger("gCheckTrackerUncheckedHide", 0))
            return;
        mainColor = CVarGetColor("gCheckTrackerUncheckedMainColor", Color_Main_Default);
        extraColor = CVarGetColor("gCheckTrackerUncheckedExtraColor",  Color_Unchecked_Extra_Default);
    } else if (status == RCSHOW_SEEN) {
        if (!showHidden && CVarGetInteger("gCheckTrackerSeenHide", 0))
            return;
        mainColor = CVarGetColor("gCheckTrackerSeenMainColor", Color_Main_Default);
        extraColor = CVarGetColor("gCheckTrackerSeenExtraColor", Color_Seen_Extra_Default);
    } else if (status == RCSHOW_COLLECTED) {
        if (!showHidden && CVarGetInteger("gCheckTrackerCheckedHide", 0))
            return;
        mainColor = CVarGetColor("gCheckTrackerCheckedMainColor", Color_Main_Default);
        extraColor = CVarGetColor("gCheckTrackerCheckedExtraColor", Color_Checked_Extra_Default);
    } else if (status == RCSHOW_SAVED) {
        if (!showHidden && CVarGetInteger("gCheckTrackerSavedHide", 0))
            return;
        mainColor = CVarGetColor("gCheckTrackerSavedMainColor", Color_Main_Default);
        extraColor = CVarGetColor("gCheckTrackerSavedExtraColor", Color_Saved_Extra_Default);
    }
 
    //Main Text
    txt = rcObj.rcShortName;
    if (lastLocationChecked == rcObj.rc)
        txt = "* " + txt;
 
    // Draw button - for Skipped/Unchecked only
    if (status == RCSHOW_UNCHECKED || skipped) {
        if (ImGui::ArrowButton(std::to_string(rcObj.rc).c_str(), skipped ? ImGuiDir_Left : ImGuiDir_Right)) {
            if (skipped) {
                checkTrackerData.find(rcObj.rc)->second.skipped = false;
                areaChecksGotten[rcObj.rcArea]--;
            } else {
                checkTrackerData.find(rcObj.rc)->second.skipped = true;
                areaChecksGotten[rcObj.rcArea]++;
            }
            UpdateOrdering();
            UpdateInventoryChecks();
            SaveTrackerData(gSaveContext.fileNum, true);
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
    if (skipped) {
        txt = "Skipped"; //TODO language
    } else if (checkData.hintItem != 0) {
        // TODO hints
    } else if (status != RCSHOW_UNCHECKED) {
        switch (status) {
            case RCSHOW_SAVED:
            case RCSHOW_COLLECTED:
                if (gSaveContext.n64ddFlag)
                    txt = OTRGlobals::Instance->gRandomizer
                        ->EnumToSpoilerfileGetName[gSaveContext.itemLocations[rcObj.rc].get.rgID][gSaveContext.language];
                else if (gSaveContext.language == LANGUAGE_ENG)
                    txt = ItemFromGIID(rcObj.ogItemId).GetName().english;
                else if (gSaveContext.language == LANGUAGE_FRA)
                    txt = ItemFromGIID(rcObj.ogItemId).GetName().french;
                break;
            case RCSHOW_SEEN:
                if (gSaveContext.n64ddFlag)
                    txt = OTRGlobals::Instance->gRandomizer
                        ->EnumToSpoilerfileGetName[gSaveContext.itemLocations[rcObj.rc].get.fakeRgID][gSaveContext.language];
                else if (gSaveContext.language == LANGUAGE_ENG)
                    txt = ItemFromGIID(rcObj.ogItemId).GetName().english;
                else if (gSaveContext.language == LANGUAGE_FRA)
                    txt = ItemFromGIID(rcObj.ogItemId).GetName().french;
                break;
        }
    }
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
    "gCheckTrackerCheckedMainColor",        "gCheckTrackerCheckedExtraColor",
    "gCheckTrackerScummedMainColor",        "gCheckTrackerScummedExtraColor",
    "gCheckTrackerSavedMainColor",          "gCheckTrackerSavedExtraColor",
};

int hue = 0;
void RainbowTick() {
    float freqHue = hue * 2 * M_PI / (360 * CVarGetFloat("gCosmetics.RainbowSpeed", 0.6f));
    for (auto cvar : rainbowCVars) {
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



const char* windowType[] = { "Floating", "Window" };
const char* displayType[] = { "Always", "Combo Button Hold" };
const char* buttonStrings[] = { "A Button", "B Button", "C-Up",  "C-Down", "C-Left", "C-Right", "L Button",
                                "Z Button", "R Button", "Start", "D-Up",   "D-Down", "D-Left",  "D-Right" };
void DrawCheckTrackerOptions(bool& open) {
    if (!open) {
        CVarSetInteger("gCheckTrackerSettingsEnabled", 0);
        return;
    }

    ImGui::SetNextWindowSize(ImVec2(600, 375), ImGuiCond_FirstUseEver);

    if (!ImGui::Begin("Check Tracker Settings", &open, ImGuiWindowFlags_NoFocusOnAppearing)) {
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

    ImGui::Text("Window Type");
    ImGui::SameLine();
    UIWidgets::EnhancementCombobox("gCheckTrackerWindowType", windowType, 2, 1);

    if (CVarGetInteger("gCheckTrackerWindowType", 1) == 0) {
        UIWidgets::EnhancementCheckbox("Enable Dragging", "gCheckTrackerHudEditMode");
        UIWidgets::EnhancementCheckbox("Only enable while paused", "gCheckTrackerShowOnlyPaused");
        ImGui::Text("Display Mode");
        ImGui::SameLine();
        UIWidgets::EnhancementCombobox("gCheckTrackerDisplayType", displayType, 2, 0);
        if (CVarGetInteger("gCheckTrackerDisplayType", 0) > 0) {
            ImGui::Text("Combo Button 1");
            ImGui::SameLine();
            UIWidgets::EnhancementCombobox("gCheckTrackerComboButton1", buttonStrings, 14, 6);
            ImGui::Text("Combo Button 2");
            ImGui::SameLine();
            UIWidgets::EnhancementCombobox("gCheckTrackerComboButton2", buttonStrings, 14, 8);
        }
    }
    UIWidgets::EnhancementCheckbox("Performance mode", "gCheckTrackerOptionPerformanceMode");
    UIWidgets::Tooltip("Slows down checking for updates to 1 check per frame. Only required if experiencing poor performance when using Check Tracker.");
    UIWidgets::EnhancementCheckbox("Vanilla/MQ Dungeon Spoilers", "gCheckTrackerOptionMQSpoilers");
    UIWidgets::Tooltip("If enabled, Vanilla/MQ dungeons will show on the tracker immediately. Otherwise, Vanilla/MQ dungeon locations must be unlocked. ");

    ImGui::TableNextColumn();

    ImGuiDrawTwoColorPickerSection("Area Incomplete",  "gCheckTrackerAreaMainIncompleteColor",   "gCheckTrackerAreaExtraIncompleteColor",  Color_Area_Incomplete_Main,   Color_Area_Incomplete_Extra,  Color_Main_Default, Color_Area_Incomplete_Extra_Default, "gCheckTrackerAreaIncompleteHide" );
    ImGuiDrawTwoColorPickerSection("Area Complete",    "gCheckTrackerAreaMainCompleteColor",     "gCheckTrackerAreaExtraCompleteColor",    Color_Area_Complete_Main,     Color_Area_Complete_Extra,    Color_Main_Default, Color_Area_Complete_Extra_Default,   "gCheckTrackerAreaCompleteHide"   );
    ImGuiDrawTwoColorPickerSection("Unchecked",        "gCheckTrackerUncheckedMainColor",        "gCheckTrackerUncheckedExtraColor",       Color_Unchecked_Main,         Color_Unchecked_Extra,        Color_Main_Default, Color_Unchecked_Extra_Default,       "gCheckTrackerUncheckedHide"      );
    ImGuiDrawTwoColorPickerSection("Skipped",          "gCheckTrackerSkippedMainColor",          "gCheckTrackerSkippedExtraColor",         Color_Skipped_Main,           Color_Skipped_Extra,          Color_Main_Default, Color_Skipped_Extra_Default,         "gCheckTrackerSkippedHide"        );
    ImGuiDrawTwoColorPickerSection("Seen (WIP)",       "gCheckTrackerSeenMainColor",             "gCheckTrackerSeenExtraColor",            Color_Seen_Main,              Color_Seen_Extra,             Color_Main_Default, Color_Seen_Extra_Default,            "gCheckTrackerSeenHide"           );
    ImGuiDrawTwoColorPickerSection("Hinted (WIP)",     "gCheckTrackerHintedMainColor",           "gCheckTrackerHintedExtraColor",          Color_Hinted_Main,            Color_Hinted_Extra,           Color_Main_Default, Color_Hinted_Extra_Default,          "gCheckTrackerHintedHide"         );
    ImGuiDrawTwoColorPickerSection("Checked (WIP)",    "gCheckTrackerCheckedMainColor",          "gCheckTrackerCheckedExtraColor",         Color_Checked_Main,           Color_Checked_Extra,          Color_Main_Default, Color_Checked_Extra_Default,         "gCheckTrackerCheckedHide"        );
    ImGuiDrawTwoColorPickerSection("Scummed (WIP)",    "gCheckTrackerScummedMainColor",          "gCheckTrackerScummedExtraColor",         Color_Scummed_Main,           Color_Scummed_Extra,          Color_Main_Default, Color_Scummed_Extra_Default,         "gCheckTrackerScummedHide"        );
    ImGuiDrawTwoColorPickerSection("Saved",            "gCheckTrackerSavedMainColor",            "gCheckTrackerSavedExtraColor",           Color_Saved_Main,             Color_Saved_Extra,            Color_Main_Default, Color_Saved_Extra_Default,           "gCheckTrackerSavedHide"          );

    ImGui::PopStyleVar(1);
    ImGui::EndTable();
    ImGui::End();
}

void CheckTrackerItemReceive(uint8_t item) {
    
}

void InitCheckTracker() {
    SohImGui::AddWindow("Randomizer", "Check Tracker", DrawCheckTracker, CVarGetInteger("gCheckTrackerEnabled", 0) == 1);
    SohImGui::AddWindow("Randomizer", "Check Tracker Settings", DrawCheckTrackerOptions);
    Color_Background = CVarGetColor("gCheckTrackerBgColor", Color_Bg_Default);
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
    Color_Checked_Main          = CVarGetColor("gCheckTrackerCheckedMainColor",           Color_Main_Default);
    Color_Checked_Extra         = CVarGetColor("gCheckTrackerCheckedExtraColor",          Color_Checked_Extra_Default);
    Color_Scummed_Main          = CVarGetColor("gCheckTrackerScummedMainColor",           Color_Main_Default);
    Color_Scummed_Extra         = CVarGetColor("gCheckTrackerScummedExtraColor",          Color_Scummed_Extra_Default);
    Color_Saved_Main            = CVarGetColor("gCheckTrackerSavedMainColor",             Color_Main_Default);
    Color_Saved_Extra           = CVarGetColor("gCheckTrackerSavedExtraColor",            Color_Saved_Extra_Default);

    Ship::RegisterHook<Ship::ControllerRead>([](OSContPad* cont_pad) {
        trackerButtonsPressed = cont_pad;
    });
    Ship::RegisterHook<Ship::LoadFile>([](uint32_t fileNum) {
        doInitialize = true;
    });
    Ship::RegisterHook<Ship::DeleteFile>([](uint32_t fileNum) {
        Teardown();
    });
    GameInteractor::Instance->RegisterGameHook<GameInteractor::OnReceiveItem>(CheckTrackerItemReceive);

    LocationTable_Init();
}

} // namespace CheckTracker
