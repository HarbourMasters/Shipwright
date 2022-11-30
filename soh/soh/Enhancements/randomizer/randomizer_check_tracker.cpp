#include "randomizer_check_tracker.h"
#include "../../OTRGlobals.h"
#include <ImGuiImpl.h>
#include "../../UIWidgets.hpp"

#include <string>
#include <vector>
#include <Cvar.h>
#include <Hooks.h>
#include "3drando/item_location.hpp"

extern "C" {
#include "variables.h"
#include "functions.h"
#include "macros.h"
extern PlayState* gPlayState;
}
extern "C" uint32_t ResourceMgr_IsSceneMasterQuest(s16 sceneNum);

void Teardown();
void InitializeChecks();
void UpdateChecks();
void UpdateInventoryChecks();
void DrawLocation(RandomizerCheckObject rcObj, RandomizerCheckShow* thisCheckStatus);
void BeginFloatWindows(std::string UniqueName, ImGuiWindowFlags flags = 0);
void EndFloatWindows();
void UpdateOrdering(bool init = false);
bool ShouldUpdateChecks();
bool CompareCheckObject(RandomizerCheckObject i, RandomizerCheckObject j);
bool HasItemBeenCollected(RandomizerCheckObject obj);
ImVec4 Color_RGBA8_to_ImVec4(Color_RGBA8& color);
Color_RGBA8 ImVec4_to_Color_RGBA8(ImVec4& color);
RandomizerCheckShow GetCheckStatus(RandomizerCheckObject rcObj, int idx);


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

ImVec4 Color_Background  = { 0, 0, 0, 0 };

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

// persistent during gameplay
bool initialized = false;
bool doInitialize = false;
std::map<RandomizerCheck, RandomizerCheckShow> checkStatusMap;
u32 areasFullyChecked = 0;
u32 areasSpoiled = 0;
bool showVOrMQ;
s8 areaChecksTotal[32];   //| For sorting and showing
s8 areaChecksGotten[32];  //|     "Kokiri Forest (4/9)"
std::vector<RandomizerCheckObject> checks;
bool optCollapseAll; // A bool that will collapse all checks once
bool optExpandAll;   // A bool that will expand all checks once
s32 lastSaveCount = -1;
RandomizerCheck lastLocationChecked = RC_UNKNOWN_CHECK;
RandomizerCheckArea previousArea = RCAREA_INVALID;
RandomizerCheckArea currentArea = RCAREA_INVALID;
OSContPad* trackerButtonsPressed;

std::vector<uint32_t> buttons = { BTN_A, BTN_B, BTN_CUP,   BTN_CDOWN, BTN_CLEFT, BTN_CRIGHT, BTN_L,
                                  BTN_Z, BTN_R, BTN_START, BTN_DUP,   BTN_DDOWN, BTN_DLEFT,  BTN_DRIGHT };

void DrawCheckTracker(bool& open) {
    if (!open) {
        CVar_SetS32("gCheckTrackerEnabled", 0);
        return;
    }

    if (doInitialize)
        InitializeChecks();
    else if (gPlayState == nullptr || gSaveContext.fileNum < 0 || gSaveContext.fileNum > 2) {
        Teardown();
        return;
    }

    if (CVar_GetS32("gCheckTrackerWindowType", 1) == 0) {
        if (CVar_GetS32("gCheckTrackerShowOnlyPaused", 0) == 1)
            if (gPlayState == nullptr || gPlayState->pauseCtx.state == 0)
                return;

        if (CVar_GetS32("gCheckTrackerDisplayType", 0) == 1) {
            int comboButton1Mask = buttons[CVar_GetS32("gCheckTrackerComboButton1", 6)];
            int comboButton2Mask = buttons[CVar_GetS32("gCheckTrackerComboButton2", 8)];
            bool comboButtonsHeld = trackerButtonsPressed != nullptr &&
                                    trackerButtonsPressed[0].button & comboButton1Mask &&
                                    trackerButtonsPressed[0].button & comboButton2Mask;
            if (!comboButtonsHeld)
                return;
        }
    }

    BeginFloatWindows("Check Tracker", ImGuiWindowFlags_NoScrollbar);

    if (!initialized) {
        ImGui::Text("Waiting for file load...");
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

    //Only update the checks if something has changed
    if (ShouldUpdateChecks()) {
        UpdateChecks();
        UpdateInventoryChecks();
        UpdateOrdering();
    }


    //Quick Options
#ifdef __WIIU__
    float headerHeight = 40.0f;
#else
    float headerHeight = 20.0f;
#endif
    ImVec2 size = ImGui::GetContentRegionMax();
    size.y -= headerHeight;
    if (ImGui::BeginTable("Check Tracker", 1, 0, size)) {
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
    }

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
    bool doDraw = false;
    bool thisAreaFullyChecked = false;
    bool showHidden = CVar_GetS32("gCheckTrackerOptionShowHidden", 0);
    bool hideIncomplete = CVar_GetS32("gCheckTrackerAreaIncompleteHide", 0);
    bool hideComplete = CVar_GetS32("gCheckTrackerAreaCompleteHide", 0);
    bool collapseLogic;
    bool doingCollapseOrExpand = optExpandAll || optCollapseAll;
    bool isThisAreaSpoiled;
    RandomizerCheckArea lastArea = RCAREA_INVALID;
    Color_RGBA8 areaCompleteColor = CVar_GetRGBA("gCheckTrackerAreaMainCompleteColor", Color_Main_Default);
    Color_RGBA8 areaIncompleteColor = CVar_GetRGBA("gCheckTrackerAreaMainIncompleteColor", Color_Main_Default);
    Color_RGBA8 extraCompleteColor = CVar_GetRGBA("gCheckTrackerAreaExtraCompleteColor", Color_Area_Complete_Extra_Default);
    Color_RGBA8 extraIncompleteColor = CVar_GetRGBA("gCheckTrackerAreaExtraIncompleteColor", Color_Area_Incomplete_Extra_Default);
    Color_RGBA8 mainColor;
    Color_RGBA8 extraColor;
    std::string stemp;
    s32 areaMask = 1;

    // Logic for each check
    for (auto& obj : checks)
    {
        // Second Column mode
        //  if (twoColumnMode && areaChecksGotten[obj.rcArea] != areaChecksTotal[obj.rcArea]) {
        //      //TODO draw left column
        //      ImGui::TableNextColumn();
        //      //TODO draw right column
        //  }
        //

        //New Area to be drawn
        if (obj.rcArea != lastArea)
        {
            //Last Area needs to be cleaned up
            if (lastArea != RCAREA_INVALID && doDraw) {
                ImGui::TreePop();
                UIWidgets::PaddedSeparator();
            }
            lastArea = obj.rcArea;

            //Decide if we should skip because of hiding rules
            thisAreaFullyChecked = ((areasFullyChecked & areaMask) != 0);
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
                stemp = RandomizerCheckObjects::GetRCAreaName(obj.rcArea) + "##TreeNode";
                ImGui::PushStyleColor(ImGuiCol_Text, Color_RGBA8_to_ImVec4(mainColor));
                if (doingCollapseOrExpand)
                    ImGui::SetNextItemOpen(collapseLogic, ImGuiCond_Always);
                else
                    ImGui::SetNextItemOpen(!thisAreaFullyChecked, ImGuiCond_Once);
                doDraw = ImGui::TreeNode(stemp.c_str());
                ImGui::PopStyleColor();
                ImGui::SameLine();
                ImGui::PushStyleColor(ImGuiCol_Text, Color_RGBA8_to_ImVec4(extraColor));

                isThisAreaSpoiled = areasSpoiled & areaMask || CVar_GetS32("gCheckTrackerOptionMQSpoilers", 0);

                if (isThisAreaSpoiled) {
                    if (showVOrMQ && RandomizerCheckObjects::AreaIsDungeon(obj.rcArea)) {
                        if (OTRGlobals::Instance->gRandomizer->masterQuestDungeons.contains(obj.sceneId))
                            ImGui::Text("(%d/%d) - MQ", areaChecksGotten[obj.rcArea], areaChecksTotal[obj.rcArea]);
                        else
                            ImGui::Text("(%d/%d) - Vanilla", areaChecksGotten[obj.rcArea], areaChecksTotal[obj.rcArea]);
                    } else {
                        ImGui::Text("(%d/%d)", areaChecksGotten[obj.rcArea], areaChecksTotal[obj.rcArea]);
                    }
                } else {
                    ImGui::Text("???", areaChecksGotten[obj.rcArea], areaChecksTotal[obj.rcArea]);
                }

                ImGui::PopStyleColor();

                //Keep areas loaded between transitions
                if (currentArea == obj.rcArea && doAreaScroll)
                    ImGui::SetScrollHereY(0.0f);
            }

            areaMask <<= 1;
        }

        if (doDraw && isThisAreaSpoiled)
            DrawLocation(obj, &checkStatusMap.find(obj.rc)->second);
    }

    //Clean up last area
    if (doDraw)
        ImGui::TreePop();

    ImGui::EndTable(); //Checks Lead-out
    ImGui::EndTable(); //Quick Options Lead-out
    EndFloatWindows();
    if (doingCollapseOrExpand) {
        optCollapseAll = false;
        optExpandAll = false;
    }
}

// Windowing stuff
void BeginFloatWindows(std::string UniqueName, ImGuiWindowFlags flags) {
    ImGuiWindowFlags windowFlags = flags;

    if (windowFlags == 0) {
        windowFlags |=
            ImGuiWindowFlags_AlwaysVerticalScrollbar | ImGuiWindowFlags_NoFocusOnAppearing;
    }

    if (!CVar_GetS32("gCheckTrackerWindowType", 1)) {
        ImGui::SetNextWindowViewport(ImGui::GetMainViewport()->ID);
        windowFlags |= ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoTitleBar |
                       ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoScrollbar;

        if (!CVar_GetS32("gCheckTrackerHudEditMode", 0)) {
            windowFlags |= ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoMove;
        }
    }
    ImGui::PushStyleColor(ImGuiCol_WindowBg, Color_Background);
    ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0, 0, 0, 0));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 4.0f);
    ImGui::Begin(UniqueName.c_str(), nullptr, windowFlags);
}
void EndFloatWindows() {
    ImGui::PopStyleVar();
    ImGui::PopStyleColor();
    ImGui::PopStyleColor();
    ImGui::End();
}

uint8_t gRandomizeShopsanity;
uint8_t gRandomizeShuffleTokens;
uint8_t gRandomizeShuffleBeans;
uint8_t gRandomizeShuffleScrubs;
uint8_t gRandomizeShuffleMerchants;
uint8_t gRandomizeShuffleCows;
uint8_t gRandomizeShuffleAdultTrade;
uint8_t gRandomizeShuffleKokiriSword;
uint8_t gRandomizeShuffleWeirdEgg;
uint8_t gRandomizeShuffleGerudoCard;
uint8_t gRandomizeShuffleFrogSongRupees;
uint8_t gRandomizeStartingMapsCompasses;
uint8_t gRandomizeKeysanity;
uint8_t gRandomizeGerudoFortressKeys;
uint8_t gRandomizeBossKeysanity;
uint8_t gRandomizeShuffleGanonBossKey;
uint8_t gRandomizeGerudoFortress;

void SetVanillaSettings() {
    gRandomizeShopsanity = RO_SHOPSANITY_OFF;
    gRandomizeShuffleTokens = RO_TOKENSANITY_ALL;
    gRandomizeShuffleBeans = RO_GENERIC_YES;
    gRandomizeShuffleScrubs = RO_GENERIC_YES;
    gRandomizeShuffleMerchants = RO_GENERIC_YES;
    gRandomizeShuffleCows = RO_GENERIC_NO;
    gRandomizeShuffleAdultTrade = RO_GENERIC_YES;
    gRandomizeShuffleKokiriSword = RO_GENERIC_YES;
    gRandomizeShuffleWeirdEgg = RO_GENERIC_YES;
    gRandomizeShuffleGerudoCard = RO_GENERIC_YES;
    gRandomizeShuffleFrogSongRupees = RO_GENERIC_NO;
    gRandomizeStartingMapsCompasses = RO_DUNGEON_ITEM_LOC_ANYWHERE;
    gRandomizeKeysanity = RO_DUNGEON_ITEM_LOC_ANYWHERE;
    gRandomizeGerudoFortressKeys = RO_GERUDO_KEYS_VANILLA;
    gRandomizeBossKeysanity = RO_DUNGEON_ITEM_LOC_ANYWHERE;
    gRandomizeShuffleGanonBossKey = RO_GANON_BOSS_KEY_ANYWHERE;
    gRandomizeGerudoFortress = RO_GF_NORMAL;
}

void SetRandomizerSettings() {
    uint8_t gRandomizeShopsanity =              OTRGlobals::Instance->gRandomizer->GetRandoSettingValue(RSK_SHOPSANITY);
    uint8_t gRandomizeShuffleTokens =           OTRGlobals::Instance->gRandomizer->GetRandoSettingValue(RSK_SHUFFLE_TOKENS);
    uint8_t gRandomizeShuffleBeans =            OTRGlobals::Instance->gRandomizer->GetRandoSettingValue(RSK_SHUFFLE_MAGIC_BEANS);
    uint8_t gRandomizeShuffleScrubs =           OTRGlobals::Instance->gRandomizer->GetRandoSettingValue(RSK_SHUFFLE_SCRUBS);
    uint8_t gRandomizeShuffleMerchants =        OTRGlobals::Instance->gRandomizer->GetRandoSettingValue(RSK_SHUFFLE_MERCHANTS);
    uint8_t gRandomizeShuffleCows =             OTRGlobals::Instance->gRandomizer->GetRandoSettingValue(RSK_SHUFFLE_COWS);
    uint8_t gRandomizeShuffleAdultTrade =       OTRGlobals::Instance->gRandomizer->GetRandoSettingValue(RSK_SHUFFLE_ADULT_TRADE);
    uint8_t gRandomizeShuffleKokiriSword =      OTRGlobals::Instance->gRandomizer->GetRandoSettingValue(RSK_SHUFFLE_KOKIRI_SWORD);
    uint8_t gRandomizeShuffleWeirdEgg =         OTRGlobals::Instance->gRandomizer->GetRandoSettingValue(RSK_SHUFFLE_WEIRD_EGG);
    uint8_t gRandomizeShuffleGerudoCard =       OTRGlobals::Instance->gRandomizer->GetRandoSettingValue(RSK_SHUFFLE_GERUDO_MEMBERSHIP_CARD);
    uint8_t gRandomizeShuffleFrogSongRupees =   OTRGlobals::Instance->gRandomizer->GetRandoSettingValue(RSK_SHUFFLE_FROG_SONG_RUPEES);
    uint8_t gRandomizeStartingMapsCompasses =   OTRGlobals::Instance->gRandomizer->GetRandoSettingValue(RSK_STARTING_MAPS_COMPASSES);
    uint8_t gRandomizeKeysanity =               OTRGlobals::Instance->gRandomizer->GetRandoSettingValue(RSK_KEYSANITY);
    uint8_t gRandomizeGerudoFortressKeys =      OTRGlobals::Instance->gRandomizer->GetRandoSettingValue(RSK_GERUDO_KEYS);
    uint8_t gRandomizeBossKeysanity =           OTRGlobals::Instance->gRandomizer->GetRandoSettingValue(RSK_BOSS_KEYSANITY);
    uint8_t gRandomizeShuffleGanonBossKey =     OTRGlobals::Instance->gRandomizer->GetRandoSettingValue(RSK_GANONS_BOSS_KEY);
    uint8_t gRandomizeGerudoFortress =          OTRGlobals::Instance->gRandomizer->GetRandoSettingValue(RSK_GERUDO_FORTRESS);
}

bool IsVisibleInCheckTracker(RandomizerCheckObject rcObj) {
    return
        (rcObj.rcArea != RCAREA_INVALID) && // don't show Invalid locations
        (!RandomizerCheckObjects::AreaIsDungeon(rcObj.rcArea) || 
            rcObj.vOrMQ == RCVORMQ_BOTH ||
            rcObj.vOrMQ == RCVORMQ_MQ && OTRGlobals::Instance->gRandomizer->masterQuestDungeons.contains(rcObj.sceneId) ||
            rcObj.vOrMQ == RCVORMQ_VANILLA && !OTRGlobals::Instance->gRandomizer->masterQuestDungeons.contains(rcObj.sceneId)
        ) &&
        (rcObj.rcType != RCTYPE_SHOP || gRandomizeShopsanity > RO_SHOPSANITY_ZERO_ITEMS) &&
        (rcObj.rcType != RCTYPE_SCRUB || gRandomizeShuffleScrubs > RO_SCRUBS_OFF) &&
        (rcObj.rcType != RCTYPE_MERCHANT || gRandomizeShuffleMerchants > RO_SHUFFLE_MERCHANTS_OFF) &&
        (rcObj.rcType != RCTYPE_GOSSIP_STONE) && //TODO: Don't show hints until tracker supports them
        (rcObj.rcType != RCTYPE_CHEST_GAME) && // don't show non final reward chest game checks until we support shuffling them
        (rcObj.rcType != RCTYPE_SKULL_TOKEN ||
            (gRandomizeShuffleTokens == RO_TOKENSANITY_ALL) ||
            (gRandomizeShuffleTokens == RO_TOKENSANITY_OVERWORLD && RandomizerCheckObjects::AreaIsOverworld(rcObj.rcArea)) ||
            (gRandomizeShuffleTokens == RO_TOKENSANITY_DUNGEONS && RandomizerCheckObjects::AreaIsDungeon(rcObj.rcArea))
        ) &&
        (rcObj.rcType != RCTYPE_COW || gRandomizeShuffleCows == RO_GENERIC_YES) &&
        (rcObj.rcType != RCTYPE_ADULT_TRADE || gRandomizeShuffleAdultTrade == RO_GENERIC_YES) &&
        (rcObj.rc != RC_KF_KOKIRI_SWORD_CHEST || gRandomizeShuffleKokiriSword == RO_GENERIC_YES) &&
        (rcObj.rc != RC_ZR_MAGIC_BEAN_SALESMAN || gRandomizeShuffleBeans == RO_GENERIC_YES) &&
        (rcObj.rc != RC_HC_MALON_EGG || gRandomizeShuffleWeirdEgg == RO_GENERIC_YES) &&
        (rcObj.rc != RC_GF_GERUDO_MEMBERSHIP_CARD || gRandomizeShuffleGerudoCard == RO_GENERIC_YES) &&
        (rcObj.rcType != RCTYPE_FROG_SONG || gRandomizeShuffleFrogSongRupees == RO_GENERIC_YES) &&
        (rcObj.rcType != RCTYPE_MAP_COMPASS || gRandomizeStartingMapsCompasses != RO_DUNGEON_ITEM_LOC_VANILLA) &&
        (rcObj.rcType != RCTYPE_SMALL_KEY || gRandomizeKeysanity != RO_DUNGEON_ITEM_LOC_VANILLA) &&
           (rcObj.rcType != RCTYPE_GF_KEY || gRandomizeGerudoFortressKeys != RO_GERUDO_KEYS_VANILLA) &&
        (rcObj.rcType != RCTYPE_BOSS_KEY || gRandomizeBossKeysanity != RO_DUNGEON_ITEM_LOC_VANILLA) &&
        (rcObj.rcType != RCTYPE_GANON_BOSS_KEY || gRandomizeShuffleGanonBossKey != RO_GANON_BOSS_KEY_VANILLA) &&
        (!RC_IS_CARPENTER(rcObj.rc) && rcObj.rc != RC_GF_GERUDO_MEMBERSHIP_CARD ||
            (gRandomizeGerudoFortress == RO_GF_OPEN && !RC_IS_CARPENTER(rcObj.rc) && rcObj.rc != RC_GF_GERUDO_MEMBERSHIP_CARD) ||
            (gRandomizeGerudoFortress == RO_GF_FAST && (rcObj.rc == RC_GF_NORTH_F1_CARPENTER || rcObj.rc == RC_GF_GERUDO_MEMBERSHIP_CARD)) ||
            (gRandomizeGerudoFortress == RO_GF_NORMAL)
        );
}


void InitializeChecks() {
    if (gPlayState == nullptr || gSaveContext.fileNum < 0 || gSaveContext.fileNum > 2)
        return;

    if (gSaveContext.n64ddFlag)
        SetRandomizerSettings();
    else
        SetVanillaSettings();

    int count = 0;
    
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
        RandomizerCheckObject linksPocket = { RC_LINKS_POCKET, RCVORMQ_BOTH, RCTYPE_LINKS_POCKET, startingArea, ACTOR_ID_MAX, SCENE_ID_MAX, 0x00, GI_NONE, false, true, "Link's Pocket", "Link's Pocket" };

        checks.push_back(linksPocket);
        checkStatusMap.emplace(RC_LINKS_POCKET, RCSHOW_SAVED);
        count++;
        areaChecksTotal[startingArea]++;
    }

    for (auto& [rcCheck, rcObj] : RandomizerCheckObjects::GetAllRCObjects()) {
        if (!IsVisibleInCheckTracker(rcObj))
            continue;

        checks.push_back(rcObj);
        checkStatusMap.emplace(rcObj.rc, RCSHOW_UNCHECKED);
        count++;
        areaChecksTotal[rcObj.rcArea]++;

        if (areaChecksGotten[rcObj.rcArea] != 0 || RandomizerCheckObjects::AreaIsOverworld(rcObj.rcArea))
            areasSpoiled |= (1 << rcObj.rcArea);
    }

    showVOrMQ = (OTRGlobals::Instance->gRandomizer->GetRandoSettingValue(RSK_MQ_DUNGEON_COUNT) > 0);
    //Bug: the above will spoil that everything is vanilla if the random count rolled 0. 
    // Should use the below instead, but the setting isn't currently saved to the savefile
    //showVOrMQ = (OTRGlobals::Instance->gRandomizer->GetRandoSettingValue(RSK_RANDOM_MQ_DUNGEONS) != RO_GENERIC_OFF);

    UpdateChecks();
    UpdateInventoryChecks();
    UpdateOrdering(true);
    doInitialize = false;
    initialized = true;
}

void Teardown() {
    initialized = false;
    checkStatusMap.clear();
    areasFullyChecked = 0;
    checks.clear();
    lastSaveCount = -1;
    lastLocationChecked = RC_UNKNOWN_CHECK;
    for (int i = 0; i < sizeof(areaChecksTotal); i++) {
        areaChecksTotal[i] = 0;
        areaChecksGotten[i] = 0;
    }
    doInitialize = true;
}

int slowCheckIdx = 0;
// Checks only one check every call
bool SlowUpdateCheck() {
    bool ret = false;
    auto checkIt = checks.begin() + slowCheckIdx;
    if (checkIt == checks.end()) {
        slowCheckIdx = 0;
        return false;
    }

    RandomizerCheckObject rcObj = *checkIt;
    RandomizerCheckShow lastStatus = checkStatusMap.find(rcObj.rc)->second;
    if (lastStatus != GetCheckStatus(rcObj, slowCheckIdx))
        ret = true;

    slowCheckIdx++;
    return ret;
}

bool ShouldUpdateChecks() {
    // TODO eventually will need to be hooked into game elements rather than just save file
    // TODO, saveCount isn't actually a thing. If not allowed to add it, will need to iterate over save bits and see if any changed 
    // TODO, or enhance hooks, but that is a LUS change
    //return lastSaveCount != gSaveContext.sohStats.saveCount;


    if (CVar_GetS32("gCheckTrackerOptionPerformanceMode", 0))
        return SlowUpdateCheck();
    else
        return true;
}

void UpdateInventoryChecks() {
    //For all the areas with compasses, if you have one, spoil the area
    for (u8 i = RCAREA_DEKU_TREE; i <= RCAREA_ICE_CAVERN; i++)
        if (gSaveContext.inventory.dungeonItems[i - RCAREA_DEKU_TREE] & 0x02)
            areasSpoiled |= (1 << i);
}

void UpdateChecks() {
    int idx = 0;
    RandomizerCheckObject* lastCheck;
    RandomizerCheckShow lastStatus;
    for (auto& rcObj : checks) {
        RandomizerCheckShow* checkStatusPtr = &checkStatusMap.find(rcObj.rc)->second;
        lastStatus = *checkStatusPtr;
        *checkStatusPtr = GetCheckStatus(rcObj, idx);

        //Update areasFullyChecked 
        if (lastStatus != *checkStatusPtr) {
            if (lastStatus != RCSHOW_CHECKED && lastStatus != RCSHOW_SAVED && (*checkStatusPtr == RCSHOW_CHECKED || *checkStatusPtr == RCSHOW_SAVED))
                areaChecksGotten[rcObj.rcArea]++;
            else if ((lastStatus == RCSHOW_CHECKED || lastStatus == RCSHOW_SAVED) && *checkStatusPtr != RCSHOW_CHECKED && *checkStatusPtr != RCSHOW_SAVED)
                areaChecksGotten[rcObj.rcArea]--;

            if (areaChecksGotten[rcObj.rcArea] == areaChecksTotal[rcObj.rcArea])
                areasFullyChecked |= (1 << rcObj.rcArea);
            else
                areasFullyChecked &= (0xFFFFFFFF - (1 << rcObj.rcArea));

            if (areaChecksGotten[rcObj.rcArea] != 0 || RandomizerCheckObjects::AreaIsOverworld(rcObj.rcArea))
                areasSpoiled |= (1 << rcObj.rcArea);
        }

        lastCheck = &rcObj;

        idx++;
    }

    //TODO for when using saveCount as a basis
    //lastSaveCount = gSaveContext.sohStats.saveCount;
}

void UpdateOrdering(bool init) {
    // Sort the entire thing
    if (init) {
        std::sort(checks.begin(), checks.end(), CompareCheckObject);
        return;
    }
    //TODO benchmark the full sort over the individual sorts, it might just make sense to always sort all?
    
    //sort each area individually
    int startOffset = 0;
    int endOffset = 0;
    for (int x = 0; x < sizeof(areaChecksTotal); x++) {
        endOffset = startOffset + areaChecksTotal[x];
        std::sort(checks.begin() + startOffset, checks.begin() + endOffset, CompareCheckObject);
        startOffset += areaChecksTotal[x];
    }
}

bool CompareCheckObject(RandomizerCheckObject i, RandomizerCheckObject j) {
    if (i.rcArea < j.rcArea)
        return true;
    else if (i.rcArea > j.rcArea)
        return false;

    if (checkStatusMap.find(i.rc)->second < checkStatusMap.find(j.rc)->second)
        return true;
    else if (checkStatusMap.find(i.rc)->second > checkStatusMap.find(j.rc)->second)
        return false;

    if (i.rcShortName < j.rcShortName)
        return true;
    if (i.rcShortName > j.rcShortName)
        return false;

    return false;
}

RandomizerCheckShow GetCheckStatus(RandomizerCheckObject rcObj, int idx) {
    if (HasItemBeenCollected(rcObj))
        return RCSHOW_SAVED; // TODO: use SAVED until we hook into game elements without requiring a save

    //If the status hasn't updated, keep showing as skipped
    if (checkStatusMap.find(rcObj.rc)->second == RCSHOW_SKIPPED)
        return RCSHOW_SKIPPED;

    return RCSHOW_UNCHECKED;

    // TODO Seen, Hinted, Scummed, saved/checked
}


bool HasItemBeenCollected(RandomizerCheckObject obj) {
    // TODO doesn't consider vanilla/MQ?

    // TODO move all the code to a static function in item_location?
    // return Location(obj.rc)->GetCollectionCheck().IsChecked(gSaveContext);

    ItemLocation* x = Location(obj.rc);
    SpoilerCollectionCheck check = x->GetCollectionCheck();
    auto flag = check.flag;
    auto scene = check.scene;
    auto type = check.type;

    int shift;
    int mask;

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
            // Magic to flip an index `flag` to a lookup for 16bit big endian integers. Probably an easier way.....
            shift = 7 - (flag % 8) + ((flag % 16) / 8) * 8;
            mask = 0x8000 >> shift;
            return gSaveContext.infTable[scene] & mask;
        case SpoilerCollectionCheckType::SPOILER_CHK_ITEM_GET_INF:
            // Magic to flip an index `flag` to a lookup for 16bit big endian integers. Probably an easier way.....
            shift = 7 - (flag % 8) + ((flag % 16) / 8) * 8;
            mask = 0x8000 >> shift;
            return gSaveContext.itemGetInf[flag / 16] & mask;
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
            return (gSaveContext.itemGetInf[1] & 0x1000) ||
                   CVar_GetS32("gGravediggingTourFix", 0) && gSaveContext.sceneFlags[scene].collect & (1 << flag);
        default:
            return false;
    }
    return false;
}

void DrawLocation(RandomizerCheckObject rcObj, RandomizerCheckShow* thisCheckStatus) {
    Color_RGBA8 mainColor;
    Color_RGBA8 extraColor;
    std::string txt;
    bool showHidden = CVar_GetS32("gCheckTrackerOptionShowHidden", 0);

    if (*thisCheckStatus == RCSHOW_UNCHECKED) {
        if (!showHidden && CVar_GetS32("gCheckTrackerUncheckedHide", 0))
            return;
        mainColor = CVar_GetRGBA("gCheckTrackerUncheckedMainColor", Color_Main_Default);
        extraColor = CVar_GetRGBA("gCheckTrackerUncheckedExtraColor",  Color_Unchecked_Extra_Default);
    } else if (*thisCheckStatus == RCSHOW_SKIPPED) {
        if (!showHidden && CVar_GetS32("gCheckTrackerSkippedHide", 0))
            return;
        mainColor = CVar_GetRGBA("gCheckTrackerSkippedMainColor", Color_Main_Default);
        extraColor = CVar_GetRGBA("gCheckTrackerSkippedExtraColor", Color_Skipped_Extra_Default);
    } else if (*thisCheckStatus == RCSHOW_SEEN) {
        if (!showHidden && CVar_GetS32("gCheckTrackerSeenHide", 0))
            return;
        mainColor = CVar_GetRGBA("gCheckTrackerSeenMainColor", Color_Main_Default);
        extraColor = CVar_GetRGBA("gCheckTrackerSeenExtraColor", Color_Seen_Extra_Default);
    } else if (*thisCheckStatus == RCSHOW_HINTED) {
        if (!showHidden && CVar_GetS32("gCheckTrackerHintedHide", 0))
            return;
        mainColor = CVar_GetRGBA("gCheckTrackerHintedMainColor", Color_Main_Default);
        extraColor = CVar_GetRGBA("gCheckTrackerHintedExtraColor", Color_Hinted_Extra_Default);
    } else if (*thisCheckStatus == RCSHOW_CHECKED) {
        if (!showHidden && CVar_GetS32("gCheckTrackerCheckedHide", 0))
            return;
        mainColor = CVar_GetRGBA("gCheckTrackerCheckedMainColor", Color_Main_Default);
        extraColor = CVar_GetRGBA("gCheckTrackerCheckedExtraColor", Color_Checked_Extra_Default);
    } else if (*thisCheckStatus == RCSHOW_SCUMMED) {
        if (!showHidden && CVar_GetS32("gCheckTrackerScummedHide", 0))
            return;
        mainColor = CVar_GetRGBA("gCheckTrackerScummedMainColor", Color_Main_Default);
        extraColor = CVar_GetRGBA("gCheckTrackerScummedExtraColor", Color_Scummed_Extra_Default);
    } else if (*thisCheckStatus == RCSHOW_SAVED) {
        if (!showHidden && CVar_GetS32("gCheckTrackerSavedHide", 0))
            return;
        mainColor = CVar_GetRGBA("gCheckTrackerSavedMainColor", Color_Main_Default);
        extraColor = CVar_GetRGBA("gCheckTrackerSavedExtraColor", Color_Saved_Extra_Default);
    }
    
    //Main Text
    txt = rcObj.rcShortName;
    if (lastLocationChecked == rcObj.rc)
        txt = "* " + txt;
    
    //Second Column mode
    //TODO
    // if (twoColumnMode) ImGui::TableNextColumn();

    // Draw button - for Skipped/Unchecked only
    if (*thisCheckStatus == RCSHOW_UNCHECKED || *thisCheckStatus == RCSHOW_SKIPPED) {
        bool skipped = (*thisCheckStatus == RCSHOW_SKIPPED);
        if (ImGui::ArrowButton(std::to_string(rcObj.rc).c_str(), skipped ? ImGuiDir_Left : ImGuiDir_Right)) {
            if (skipped)
                *thisCheckStatus = RCSHOW_UNCHECKED;
            else
                *thisCheckStatus = RCSHOW_SKIPPED;
        }
    } else {
        ImGui::InvisibleButton("", ImVec2(20.0f, 10.0f));
    }
    ImGui::SameLine();

    //Draw
    ImGui::PushStyleColor(ImGuiCol_Text, Color_RGBA8_to_ImVec4(mainColor));
    ImGui::Text("%s", txt.c_str());
    ImGui::PopStyleColor();

    //Draw the extra info
    if (*thisCheckStatus != RCSHOW_UNCHECKED) {
        switch (*thisCheckStatus) {
            case RCSHOW_SAVED:
            case RCSHOW_CHECKED:
            case RCSHOW_SCUMMED:
                if (gSaveContext.n64ddFlag)
                    txt = OTRGlobals::Instance->gRandomizer
                        ->EnumToSpoilerfileGetName[gSaveContext.itemLocations[rcObj.rc].get.rgID][gSaveContext.language];
                else if (gSaveContext.language == LANGUAGE_ENG)
                    txt = ItemFromGIID(rcObj.ogItemId).GetName().english;
                else if (gSaveContext.language == LANGUAGE_FRA)
                    txt = ItemFromGIID(rcObj.ogItemId).GetName().french;
                break;
            case RCSHOW_SKIPPED:
                txt = "Skipped"; //TODO language
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
            case RCSHOW_HINTED:
                txt = "Hints are WIP"; // TODO language
                break;
        }

        ImGui::PushStyleColor(ImGuiCol_Text, Color_RGBA8_to_ImVec4(extraColor));
        ImGui::SameLine();
        ImGui::Text(" (%s)", txt.c_str());
        ImGui::PopStyleColor();
    }

}


inline void SaveColorToCVar(std::string cvarname, ImVec4 color) {
    CVar_SetFloat((cvarname + "R").c_str(), color.x);
    CVar_SetFloat((cvarname + "G").c_str(), color.y);
    CVar_SetFloat((cvarname + "B").c_str(), color.z);
    CVar_SetFloat((cvarname + "A").c_str(), color.w);
    SohImGui::RequestCvarSaveOnNextTick();
}
inline ImVec4 GetColorFromCVar(std::string cvarname) {
    float R = CVar_GetFloat((cvarname + "R").c_str(), 255);
    float G = CVar_GetFloat((cvarname + "G").c_str(), 255);
    float B = CVar_GetFloat((cvarname + "B").c_str(), 255);
    float A = CVar_GetFloat((cvarname + "A").c_str(), 255);
    return ImVec4(R, G, B, A);
}
inline Color_RGBA8 ImVec4_to_Color_RGBA8(ImVec4& color) {
    Color_RGBA8 ret;
    ret.r = color.x * 255.0f;
    ret.g = color.y * 255.0f;
    ret.b = color.z * 255.0f;
    ret.a = color.w * 255.0f;
    return ret;
}
inline ImVec4 Color_RGBA8_to_ImVec4(Color_RGBA8& color) {
    ImVec4 ret;
    ret.x = color.r / 255.0f;
    ret.y = color.g / 255.0f;
    ret.z = color.b / 255.0f;
    ret.w = color.a;
    return ret;
}

void ImGuiDrawTwoColorPickerSection(const char* text, const char* cvarMainName, const char* cvarExtraName,
                                 Color_RGBA8& main_color, Color_RGBA8& extra_color, Color_RGBA8& main_default_color,
                                 Color_RGBA8& extra_default_color, const char* cvarHideName) {
    Color_RGBA8 cvarMainColor = CVar_GetRGBA(cvarMainName, main_default_color);
    Color_RGBA8 cvarExtraColor = CVar_GetRGBA(cvarExtraName, extra_default_color);
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
            if (UIWidgets::EnhancementColor("Check", cvarMainName, Color_RGBA8_to_ImVec4(main_color), Color_RGBA8_to_ImVec4(main_default_color))) {
                main_color = CVar_GetRGBA(cvarMainName, main_default_color);
            };
            ImGui::PopItemWidth();
            
            ImGui::TableNextColumn();
            ImGui::AlignTextToFramePadding();
            ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x);
            if (UIWidgets::EnhancementColor("Details", cvarExtraName, Color_RGBA8_to_ImVec4(extra_color),
                                            Color_RGBA8_to_ImVec4(extra_default_color))) {
                main_color = CVar_GetRGBA(cvarExtraName, extra_default_color);
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
        CVar_SetS32("gCheckTrackerSettingsEnabled", 0);
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
    ImGui::Text("BG Color");
    ImGui::SameLine();
    ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x);
    if (ImGui::ColorEdit4("BG Color##gCheckTrackerBgColor", (float*)&Color_Background, ImGuiColorEditFlags_AlphaPreview | ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_NoLabel)) {
        CVar_SetFloat("gCheckTrackerBgColorR", Color_Background.x);
        CVar_SetFloat("gCheckTrackerBgColorG", Color_Background.y);
        CVar_SetFloat("gCheckTrackerBgColorB", Color_Background.z);
        CVar_SetFloat("gCheckTrackerBgColorA", Color_Background.w);
        SohImGui::RequestCvarSaveOnNextTick();
    }
    ImGui::PopItemWidth();

    ImGui::Text("Window Type");
    ImGui::SameLine();
    UIWidgets::EnhancementCombobox("gCheckTrackerWindowType", windowType, 2, 1);

    if (CVar_GetS32("gCheckTrackerWindowType", 1) == 0) {
        UIWidgets::EnhancementCheckbox("Enable Dragging", "gCheckTrackerHudEditMode");
        UIWidgets::EnhancementCheckbox("Only enable while paused", "gCheckTrackerShowOnlyPaused");
        ImGui::Text("Display Mode");
        ImGui::SameLine();
        UIWidgets::EnhancementCombobox("gCheckTrackerDisplayType", displayType, 2, 0);
        if (CVar_GetS32("gCheckTrackerDisplayType", 0) > 0) {
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

void InitCheckTracker() {
    SohImGui::AddWindow("Randomizer", "Check Tracker", DrawCheckTracker, CVar_GetS32("gCheckTrackerEnabled", 0) == 1);
    SohImGui::AddWindow("Randomizer", "Check Tracker Settings", DrawCheckTrackerOptions);
    float trackerBgR = CVar_GetFloat("gCheckTrackerBgColorR", 0);
    float trackerBgG = CVar_GetFloat("gCheckTrackerBgColorG", 0);
    float trackerBgB = CVar_GetFloat("gCheckTrackerBgColorB", 0);
    float trackerBgA = CVar_GetFloat("gCheckTrackerBgColorA", 1);
    Color_Background = { trackerBgR, trackerBgG, trackerBgB, trackerBgA }; // Float value, 1 = 255 in rgb value.
    Color_Area_Incomplete_Main  = CVar_GetRGBA("gCheckTrackerAreaMainIncompleteColor",    Color_Main_Default);
    Color_Area_Incomplete_Extra = CVar_GetRGBA("gCheckTrackerAreaExtraIncompleteColor",   Color_Area_Incomplete_Extra_Default);
    Color_Area_Complete_Main    = CVar_GetRGBA("gCheckTrackerAreaMainCompleteColor",      Color_Main_Default);
    Color_Area_Complete_Extra   = CVar_GetRGBA("gCheckTrackerAreaExtraCompleteColor",     Color_Area_Complete_Extra_Default);
    Color_Unchecked_Main        = CVar_GetRGBA("gCheckTrackerUncheckedMainColor",         Color_Main_Default);
    Color_Unchecked_Extra       = CVar_GetRGBA("gCheckTrackerUncheckedExtraColor",        Color_Unchecked_Extra_Default);
    Color_Skipped_Main          = CVar_GetRGBA("gCheckTrackerSkippedMainColor",           Color_Main_Default);
    Color_Skipped_Extra         = CVar_GetRGBA("gCheckTrackerSkippedExtraColor",          Color_Skipped_Extra_Default);
    Color_Seen_Main             = CVar_GetRGBA("gCheckTrackerSeenMainColor",              Color_Main_Default);
    Color_Seen_Extra            = CVar_GetRGBA("gCheckTrackerSeenExtraColor",             Color_Seen_Extra_Default);
    Color_Hinted_Main           = CVar_GetRGBA("gCheckTrackerHintedMainColor",            Color_Main_Default);
    Color_Hinted_Extra          = CVar_GetRGBA("gCheckTrackerHintedExtraColor",           Color_Hinted_Extra_Default);
    Color_Checked_Main          = CVar_GetRGBA("gCheckTrackerCheckedMainColor",           Color_Main_Default);
    Color_Checked_Extra         = CVar_GetRGBA("gCheckTrackerCheckedExtraColor",          Color_Checked_Extra_Default);
    Color_Scummed_Main          = CVar_GetRGBA("gCheckTrackerScummedMainColor",           Color_Main_Default);
    Color_Scummed_Extra         = CVar_GetRGBA("gCheckTrackerScummedExtraColor",          Color_Scummed_Extra_Default);
    Color_Saved_Main            = CVar_GetRGBA("gCheckTrackerSavedMainColor",             Color_Main_Default);
    Color_Saved_Extra           = CVar_GetRGBA("gCheckTrackerSavedExtraColor",            Color_Saved_Extra_Default);

    Ship::RegisterHook<Ship::ControllerRead>([](OSContPad* cont_pad) {
        trackerButtonsPressed = cont_pad;
    });
    Ship::RegisterHook<Ship::LoadFile>([](uint32_t fileNum) {
        doInitialize = true;
    });
    Ship::RegisterHook<Ship::DeleteFile>([](uint32_t fileNum) {
        Teardown();
    });
    LocationTable_Init();
}

