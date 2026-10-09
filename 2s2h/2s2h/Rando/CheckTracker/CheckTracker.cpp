
#include "CheckTracker.h"
#include "2s2h/Rando/Logic/Logic.h"
#include "2s2h/ShipUtils.h"
#include "2s2h/BenGui/UIWidgets.hpp"
#include "2s2h/Rando/StaticData/StaticData.h"
#include "2s2h/BenPort.h"
#include <cstring>

// Image Icons
#include "assets/2s2h_assets.h"
#include "interface/parameter_static/parameter_static.h"
#include "assets/archives/icon_item_static/icon_item_static_yar.h"
#include "assets/interface/icon_item_dungeon_static/icon_item_dungeon_static.h"
#include "assets/archives/icon_item_24_static/icon_item_24_static_yar.h"
#include "assets/archives/schedule_dma_static/schedule_dma_static_yar.h"
#include "assets/interface/icon_item_field_static/icon_item_field_static.h"
#include "assets/objects/gameplay_keep/gameplay_keep.h"
#include "assets/overlays/ovl_En_Syateki_Okuta/ovl_En_Syateki_Okuta.h"

extern "C" {
s16 Play_GetOriginalSceneId(s16 sceneId);
}

#include <fast/Fast3dGui.h>

namespace BenGui {
extern std::shared_ptr<Rando::CheckTracker::CheckTrackerWindow> mRandoCheckTrackerWindow;
}

#define WIDGET_COLOR UIWidgets::Colors(CVarGetInteger("gSettings.Menu.Theme", 5))

#define DEFINE_SCENE(_name, enumValue, _textId, _drawConfig, _restrictionFlags, _persistentCycleFlags, \
                     _entranceSceneId, betterMapSelectIndex, _humanName)                               \
    { enumValue, betterMapSelectIndex },
#define DEFINE_SCENE_UNSET(_enumValue)

static std::unordered_map<s32, s32> betterSceneIndex = {
#include "tables/scene_table.h"
};

#undef DEFINE_SCENE
#undef DEFINE_SCENE_UNSET

typedef enum {
    CHECK_MODE_NORMAL,
    CHECK_MODE_COLORED,
    CHECK_MODE_HIDDEN,
} CheckDisplayMode;

static std::unordered_map<int32_t, const char*> sCheckModes = {
    { CHECK_MODE_NORMAL, "Normal" },
    { CHECK_MODE_COLORED, "Colored" },
    { CHECK_MODE_HIDDEN, "Hidden" },
};

typedef enum {
    CHECK_TRACKER_VISIBILITY_MODE_ALWAYS,
    CHECK_TRACKER_VISIBILITY_MODE_ONLY_ON_PAUSE_MENU,
    CHECK_TRACKER_VISIBILITY_MODE_BUTTON_TOGGLE,
    CHECK_TRACKER_VISIBILITY_MODE_BUTTON_HOLD,
} CheckTrackerVisibilityMode;

static std::unordered_map<int32_t, const char*> sCheckVisibilityModes = {
    { CHECK_TRACKER_VISIBILITY_MODE_ALWAYS, "Always" },
    { CHECK_TRACKER_VISIBILITY_MODE_ONLY_ON_PAUSE_MENU, "Only on Pause Menu" },
    { CHECK_TRACKER_VISIBILITY_MODE_BUTTON_TOGGLE, "Button Toggle" },
    { CHECK_TRACKER_VISIBILITY_MODE_BUTTON_HOLD, "Button Hold" },
};

static bool sCheckTrackerBtnState = false;

#define CVAR_NAME_SHOW_CHECK_TRACKER "gWindows.CheckTracker"
#define CVAR_NAME_VISIBILITY_MODE "gRando.CheckTracker.VisibilityMode"
#define CVAR_NAME_VISIBILITY_BTN "gRando.CheckTracker.VisibilityBtn"
#define CVAR_NAME_OUT_OF_LOGIC_MODE "gRando.CheckTracker.OutOfLogicMode"
#define CVAR_NAME_OUT_OF_LOGIC_COLOR "gRando.CheckTracker.OutOfLogicColor"
#define CVAR_NAME_COLLECTED_MODE "gRando.CheckTracker.CollectedMode"
#define CVAR_NAME_COLLECTED_COLOR "gRando.CheckTracker.CollectedColor"
#define CVAR_NAME_SKIPPED_MODE "gRando.CheckTracker.SkippedMode"
#define CVAR_NAME_SKIPPED_COLOR "gRando.CheckTracker.SkippedColor"
#define CVAR_NAME_SCROLL_TO_SCENE "gRando.CheckTracker.ScrollToCurrentScene"
#define CVAR_NAME_TRACKER_OPACITY "gRando.CheckTracker.Opacity"
#define CVAR_NAME_TRACKER_SCALE "gRando.CheckTracker.Scale"
#define CVAR_NAME_SHOW_CURRENT_SCENE "gRando.CheckTracker.ShowCurrentScene"
#define CVAR_NAME_SHOW_CHECK_TYPE_FILTER "gRando.CheckTracker.ShowCheckTypeFilter"
#define CVAR_NAME_SHOW_SEARCH "gRando.CheckTracker.ShowSearch"
#define CVAR_SHOW_CHECK_TRACKER CVarGetInteger(CVAR_NAME_SHOW_CHECK_TRACKER, 0)
#define CVAR_VISIBILITY_MODE CVarGetInteger(CVAR_NAME_VISIBILITY_MODE, CHECK_TRACKER_VISIBILITY_MODE_ALWAYS)
#define CVAR_VISIBILITY_BTN CVarGetInteger(CVAR_NAME_VISIBILITY_BTN, BTN_CUSTOM_MODIFIER1)
#define CVAR_OUT_OF_LOGIC_MODE CVarGetInteger(CVAR_NAME_OUT_OF_LOGIC_MODE, CHECK_MODE_COLORED)
#define CVAR_OUT_OF_LOGIC_COLOR CVarGetColor(CVAR_NAME_OUT_OF_LOGIC_COLOR ".Value", { 255, 255, 255, 100 })
#define CVAR_COLLECTED_MODE CVarGetInteger(CVAR_NAME_COLLECTED_MODE, CHECK_MODE_HIDDEN)
#define CVAR_COLLECTED_COLOR CVarGetColor(CVAR_NAME_COLLECTED_COLOR ".Value", { 100, 255, 100, 255 })
#define CVAR_SKIPPED_MODE CVarGetInteger(CVAR_NAME_SKIPPED_MODE, CHECK_MODE_HIDDEN)
#define CVAR_SKIPPED_COLOR CVarGetColor(CVAR_NAME_SKIPPED_COLOR ".Value", { 255, 100, 255, 255 })
#define CVAR_SCROLL_TO_SCENE CVarGetInteger(CVAR_NAME_SCROLL_TO_SCENE, 1)
#define CVAR_TRACKER_OPACITY CVarGetFloat(CVAR_NAME_TRACKER_OPACITY, 0.5f)
#define CVAR_TRACKER_SCALE CVarGetFloat(CVAR_NAME_TRACKER_SCALE, 1.0f)
#define CVAR_SHOW_CURRENT_SCENE CVarGetInteger(CVAR_NAME_SHOW_CURRENT_SCENE, 0)
#define CVAR_SHOW_CHECK_TYPE_FILTER CVarGetInteger(CVAR_NAME_SHOW_CHECK_TYPE_FILTER, 0)
#define CVAR_SHOW_SEARCH CVarGetInteger(CVAR_NAME_SHOW_SEARCH, 1)

static bool sExpandedHeadersToggle = true;
static bool sExpandedHeadersState = true;
static s32 sScrollToTargetScene = -1;
static s32 sScrollToTargetEntrance = -1;
static ImGuiTextFilter sCheckTrackerFilter;
float trackerScale = 1.0f;
float searchBoxPadding = 15.0f;
ImVec4 trackerBG = ImVec4{ 0, 0, 0, 0.5f };

std::map<SceneId, std::set<RandoCheckId>> sceneChecks;
std::vector<SceneId> sortedSceneIds;
std::unordered_map<RandoCheckId, std::string> accessLogicFuncs;
std::unordered_map<RandoRegionId, SceneId> regionParentSceneMap;
std::set<RandoCheckType> checkTypeFiltersAvailable;
std::set<RandoCheckType> checkTypeFilter;

std::vector<const char*> checkTypeIconList = {
    /*RCTYPE_UNKNOWN*/ gItemIconBombersNotebookTex,
    /*RCTYPE_BARREL*/ gBarrelTrackerIcon,
    /*RCTYPE_BUTTERFLY*/ gItemIconDekuStickTex,
    /*RCTYPE_CHEST*/ gChestTrackerIcon,
    /*RCTYPE_COW*/ gItemIconRomaniMaskTex,
    /*RCTYPE_CRATE*/ gCrateTrackerIcon,
    /*RCTYPE_ENEMY_DROP*/ gDungeonMapSkullTex,
    /*RCTYPE_FREESTANDING*/ gRupeeCounterIconTex,
    /*RCTYPE_FROG*/ gItemIconDonGeroMaskTex,
    /*RCTYPE_GRASS*/ gameplay_keep_Tex_053140,
    /*RCTYPE_HEART*/ gQuestIconPieceOfHeartTex,
    /*RCTYPE_BEEHIVE*/ gArcheryScoreIconTex,
    /*RCTYPE_MINIGAME*/ gArcheryScoreIconTex,
    /*RCTYPE_NPC*/ gItemIconBombersNotebookTex,
    /*RCTYPE_OWL*/ gWorldMapOwlFaceTex,
    /*RCTYPE_POT*/ gPotTrackerIcon,
    /*RCTYPE_REMAINS*/ gItemIconBombersNotebookTex,
    /*RCTYPE_SHOP*/ gItemIconAdultsWalletTex,
    /*RCTYPE_SKULL_TOKEN*/ gQuestIconGoldSkulltulaTex,
    /*RCTYPE_SNOWBALL*/ gMagicArrowEquipEffectTex,
    /*RCTYPE_SONG*/ gItemIconSongNoteTex,
    /*RCTYPE_STRAY_FAIRY*/ gStrayFairyGreatBayIconTex,
    /*RCTYPE_TINGLE_SHOP*/ gItemIconAdultsWalletTex,
    /*RCTYPE_TREE*/ gItemIconDekuStickTex,
    /*RCTYPE_WONDER_ITEM*/ gPauseUnusedCursorTex,
};

static constexpr ImVec4 tintColor = {};

std::string GetTotalCheckCount() {
    std::string totalChecks;
    uint32_t collected = 0;
    uint32_t totalShuffled = 0;
    for (auto& [_, randoStaticCheck] : Rando::StaticData::Checks) {
        if (RANDO_SAVE_CHECKS[randoStaticCheck.randoCheckId].obtained ||
            RANDO_SAVE_CHECKS[randoStaticCheck.randoCheckId].skipped) {
            collected++;
        }
        if (RANDO_SAVE_CHECKS[randoStaticCheck.randoCheckId].shuffled) {
            totalShuffled++;
        }
    }
    totalChecks = std::to_string(collected);
    totalChecks += "/";
    totalChecks += std::to_string(totalShuffled);
    return totalChecks;
}

void DrawCheckTypeIcon(RandoCheckId randoCheckId) {
    RandoCheckType checkType = Rando::StaticData::Checks[randoCheckId].randoCheckType;
    ImGui::Image(std::dynamic_pointer_cast<Fast::Fast3dGui>(Ship::Context::GetRawInstance()->GetWindow()->GetGui())
                     ->GetTextureByName(checkTypeIconList[checkType]),
                 checkType == RCTYPE_SONG  ? ImVec2(12.0f * trackerScale, 18.0f * trackerScale)
                 : checkType == RCTYPE_OWL ? ImVec2(18.0f * trackerScale, 9.0f * trackerScale)
                                           : ImVec2(18.0f * trackerScale, 18.0f * trackerScale),
                 ImVec2(0, 0), ImVec2(1, 1),
                 checkType == RCTYPE_FREESTANDING ? ImVec4(0.78f, 1, 0.39f, 1) : ImVec4(1, 1, 1, 1), tintColor);
}

static std::set<SceneId> scenesToCheckParent = {
    SCENE_KAKUSIANA, SCENE_TAKARAYA,    SCENE_BOWLING,      SCENE_SONCHONOIE, SCENE_SYATEKI_MIZU, SCENE_YADOYA,
    SCENE_MILK_BAR,  SCENE_AYASHIISHOP, SCENE_YOUSEI_IZUMI, SCENE_DOUJOU,     SCENE_8ITEMSHOP,    SCENE_BOMYA,
    SCENE_POSTHOUSE, SCENE_TAKARAKUJI,  SCENE_SYATEKI_MORI, SCENE_MAP_SHOP,   SCENE_WITCH_SHOP,   SCENE_F01C,
    SCENE_F01_B,     SCENE_OMOYA,       SCENE_GORONSHOP,    SCENE_KAJIYA,     SCENE_FISHERMAN,    SCENE_LABO,
    SCENE_BANDROOM,  SCENE_TOUGITES,    SCENE_MUSICHOUSE,
};

static std::unordered_map<SceneId, const char*> sCheckTrackerSceneNameOverrides = {
    { SCENE_20SICHITAI, "Southern Swamp" },   { SCENE_10YUKIYAMANOMURA, "Mountain Village" },
    { SCENE_11GORONNOSATO, "Goron Village" }, { SCENE_17SETUGEN, "Path to Goron Village" },
    { SCENE_KAKUSIANA, "Lone Peak Shrine" },
};

const char* GetCheckTrackerSceneName(SceneId sceneId) {
    auto it = sCheckTrackerSceneNameOverrides.find(sceneId);
    if (it != sCheckTrackerSceneNameOverrides.end()) {
        return it->second;
    }
    return Ship_GetSceneName(sceneId);
}

SceneId GetScrollTargetScene(s32 rawSceneId) {
    SceneId sceneId = (SceneId)Play_GetOriginalSceneId(rawSceneId);
    if (scenesToCheckParent.contains(sceneId)) {
        RandoRegionId regionId = Rando::Logic::GetRegionIdFromEntrance(gSaveContext.save.entrance);
        if (regionParentSceneMap.contains(regionId)) {
            return regionParentSceneMap[regionId];
        }
    }
    return sceneId;
}

void initializeSceneChecks() {
    sceneChecks.clear();
    regionParentSceneMap.clear();
    checkTypeFiltersAvailable.clear();
    checkTypeFiltersAvailable.insert(RCTYPE_UNKNOWN); // Always include the unknown type

    for (auto& [regionId, staticRegion] : Rando::Logic::Regions) {
        SceneId sceneId = (SceneId)Play_GetOriginalSceneId(staticRegion.sceneId);

        // For some scenes we want to show them in their parent instead
        if (scenesToCheckParent.contains(sceneId)) {
            RandoRegionId connectedRegionId = RR_MAX;
            if (regionId == RR_LONE_PEAK_SHRINE) {
                connectedRegionId = RR_LONE_PEAK_SHRINE;
            } else {
                for (auto& [entrance, _] : staticRegion.exits) {
                    connectedRegionId = Rando::Logic::GetRegionIdFromEntrance(entrance);
                }
                for (auto& [regionId, _] : staticRegion.connections) {
                    connectedRegionId = regionId;
                }
                if (connectedRegionId == RR_MAX) {
                    continue;
                }
            }
            sceneId = Rando::Logic::Regions[connectedRegionId].sceneId;
            regionParentSceneMap[regionId] = sceneId;
        }
        for (auto& [randoCheckId, _] : staticRegion.checks) {
            RandoSaveCheck& randoSaveCheck = RANDO_SAVE_CHECKS[randoCheckId];
            auto& randoStaticCheck = Rando::StaticData::Checks[randoCheckId];
            if (!randoSaveCheck.shuffled) {
                continue;
            }
            checkTypeFiltersAvailable.insert(randoStaticCheck.randoCheckType);
            sceneChecks[sceneId].insert(randoCheckId);
        }
    }

    sortedSceneIds.clear();
    for (auto& [sceneId, _] : sceneChecks) {
        sortedSceneIds.push_back(sceneId);
    }
    std::sort(sortedSceneIds.begin(), sortedSceneIds.end(),
              [](SceneId a, SceneId b) { return betterSceneIndex[a] < betterSceneIndex[b]; });
}

bool CheckTrackerIsFiltered(RandoCheckId randoCheckId) {
    bool filterCheck = false;
    if (checkTypeFilter.empty()) {
        return false;
    }
    if (!checkTypeFilter.contains(Rando::StaticData::Checks[randoCheckId].randoCheckType)) {
        filterCheck = true;
    }
    return filterCheck;
}

std::unordered_map<RandoCheckId, bool> checksInLogic;
static u32 lastFrame = 0;

void RefreshChecksInLogic() {
    if (gGameState == NULL || gGameState->frames - lastFrame < 20 || CVAR_OUT_OF_LOGIC_MODE == CHECK_MODE_NORMAL) {
        return;
    }

    lastFrame = gGameState->frames;
    checksInLogic.clear();

    // Clear all events so they're re-evaluated fresh each refresh
    for (int i = 0; i < RE_MAX; i++) {
        RANDO_EVENTS[i] = 0;
    }

    std::set<RandoRegionId> reachableRegions = {
        RR_MAX,
        Rando::Logic::GetRegionIdFromEntrance(gSaveContext.save.entrance),
    };
    // Initialize time states using shared function
    std::unordered_map<RandoRegionId, Rando::Logic::RegionTimeState> regionTimeStates =
        Rando::Logic::InitializeRegionTimeStates(RR_MAX);

    std::set<std::pair<RandoEvent, std::function<bool()>>*> appliedEvents;

    // Iteratively explore until no new regions/events discovered
    bool changed = true;
    while (changed) {
        changed = false;
        auto prevSize = reachableRegions.size();

        // Explore from all currently reachable regions
        std::set<RandoRegionId> regionsToExplore = reachableRegions;
        for (RandoRegionId regionId : regionsToExplore) {
            Rando::Logic::FindReachableRegions(regionId, reachableRegions, regionTimeStates);
        }

        // Trigger events for newly discovered regions
        for (RandoRegionId regionId : reachableRegions) {
            auto& randoRegion = Rando::Logic::Regions[regionId];
            Rando::Logic::SetCurrentRegionTime(regionTimeStates, regionId);

            for (auto& event : randoRegion.events) {
                if (!appliedEvents.contains(&event) && event.second()) {
                    RANDO_EVENTS[event.first]++;
                    appliedEvents.insert(&event);
                    changed = true;
                }
            }
        }

        if (reachableRegions.size() != prevSize) {
            changed = true;
        }
    }

    // Evaluate checks for all reachable regions
    for (RandoRegionId regionId : reachableRegions) {
        auto& randoRegion = Rando::Logic::Regions[regionId];
        Rando::Logic::SetCurrentRegionTime(regionTimeStates, regionId);

        for (auto& [randoCheckId, accessLogicFunc] : randoRegion.checks) {
            auto& randoStaticCheck = Rando::StaticData::Checks[randoCheckId];
            auto& randoSaveCheck = RANDO_SAVE_CHECKS[randoCheckId];
            if (randoSaveCheck.shuffled && !randoSaveCheck.obtained && accessLogicFunc.first()) {
                checksInLogic.insert({ randoCheckId, true });
            }
        }
    }
}

void CheckTrackerDrawNonLogicalList() {
    RefreshChecksInLogic();

    for (auto& sceneId : sortedSceneIds) {
        if (sceneId == SCENE_MAX) {
            continue;
        }

        auto& unfilteredChecks = sceneChecks[sceneId];
        std::vector<RandoCheckId> checks;
        uint32_t obtainedCheckSum = 0;

        for (auto& checkId : unfilteredChecks) {
            if (RANDO_SAVE_CHECKS[checkId].obtained) {
                obtainedCheckSum++;
                if (CVAR_COLLECTED_MODE == CHECK_MODE_HIDDEN) {
                    continue;
                }
            } else if (RANDO_SAVE_CHECKS[checkId].skipped) {
                obtainedCheckSum++;
                if (CVAR_SKIPPED_MODE == CHECK_MODE_HIDDEN) {
                    continue;
                }
            } else if (CVAR_OUT_OF_LOGIC_MODE == CHECK_MODE_HIDDEN && !checksInLogic.contains(checkId)) {
                continue;
            }

            if (!sCheckTrackerFilter.PassFilter(Rando::StaticData::CheckNames[checkId].c_str())) {
                continue;
            }

            if (CVAR_SHOW_CURRENT_SCENE && sceneId != GetScrollTargetScene(gPlayState->sceneId)) {
                continue;
            }

            if (CheckTrackerIsFiltered(checkId)) {
                continue;
            }

            checks.push_back(checkId);
        }

        if (checks.size() == 0) {
            continue;
        }

        ImGui::PushID(sceneId);
        ImGui::Separator();

        // Auto-scroll to current scene - called after Separator to ensure stable positioning
        if (!CVAR_SHOW_CURRENT_SCENE && sScrollToTargetScene != -1 && sScrollToTargetScene == sceneId) {
            ImGui::SetScrollHereY(0.0f);
            sScrollToTargetScene = -1;
            sScrollToTargetEntrance = -1;
        }
        ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(0, 0, 0, 0));
        std::string headerText = GetCheckTrackerSceneName(sceneId);
        headerText += " (" + std::to_string(obtainedCheckSum) + "/" + std::to_string(unfilteredChecks.size()) + ")";

        ImGui::PushStyleColor(ImGuiCol_Text, obtainedCheckSum == unfilteredChecks.size()
                                                 ? UIWidgets::ColorValues.at(UIWidgets::Colors::Green)
                                                 : UIWidgets::ColorValues.at(UIWidgets::Colors::White));

        if (sExpandedHeadersState != sExpandedHeadersToggle) {
            ImGui::SetNextItemOpen(sExpandedHeadersToggle);
        }
        if (ImGui::CollapsingHeader(headerText.c_str(), ImGuiTreeNodeFlags_DefaultOpen)) {
            ImGui::Indent(20.0f);
            if (ImGui::BeginTable("Check Tracker", 2)) {
                ImGui::TableSetupColumn("Type", ImGuiTableColumnFlags_WidthFixed, (16.0f * trackerScale));
                ImGui::TableSetupColumn("Check");
                ImGui::TableNextColumn();
                for (auto& randoCheckId : checks) {
                    Rando::StaticData::RandoStaticCheck& randoStaticCheck = Rando::StaticData::Checks[randoCheckId];
                    RandoSaveCheck& randoSaveCheck = RANDO_SAVE_CHECKS[randoCheckId];
                    ImVec4 textColor = UIWidgets::ColorValues.at(UIWidgets::Colors::White);
                    if (randoSaveCheck.obtained) {
                        if (CVAR_COLLECTED_MODE == CHECK_MODE_COLORED) {
                            textColor = VecFromRGBA8(CVAR_COLLECTED_COLOR);
                        }
                    } else if (randoSaveCheck.skipped) {
                        if (CVAR_SKIPPED_MODE == CHECK_MODE_COLORED) {
                            textColor = VecFromRGBA8(CVAR_SKIPPED_COLOR);
                        }
                    } else if (!checksInLogic.contains(randoCheckId)) {
                        if (CVAR_OUT_OF_LOGIC_MODE == CHECK_MODE_COLORED) {
                            textColor = VecFromRGBA8(CVAR_OUT_OF_LOGIC_COLOR);
                        }
                    }

                    ImGui::PushStyleColor(ImGuiCol_Text, textColor);
                    ImGui::BeginGroup();
                    float cursorPosY = ImGui::GetCursorPosY();
                    if (Rando::StaticData::Checks[randoCheckId].randoCheckType == RCTYPE_OWL) {
                        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 3.5f);
                    }
                    DrawCheckTypeIcon(randoCheckId);
                    ImGui::TableNextColumn();

                    ImGui::SetCursorPosY(cursorPosY);
                    ImGui::Text("%s", Rando::StaticData::CheckNames[randoCheckId].c_str());
                    if (randoSaveCheck.obtained) {
                        ImGui::SameLine();
                        ImGui::Text("(%s)", Rando::StaticData::Items[randoSaveCheck.randoItemId].name);
                    } else if (randoSaveCheck.skipped) {
                        ImGui::SameLine();
                        ImGui::Text("(Skipped)");
                    }
                    ImGui::SameLine();
                    ImGui::Dummy(ImVec2(ImGui::GetContentRegionAvail().x, 0));
                    ImGui::EndGroup();
                    ImGui::PopStyleColor();
                    std::string accessLogicString = accessLogicFuncs.find(randoCheckId) != accessLogicFuncs.end()
                                                        ? accessLogicFuncs[randoCheckId]
                                                        : "";
                    /*
                     * Enemy drop checks are multiple in number and may have unique conditions per location. This
                     * can result in arbitrary particular instances' conditions being displayed for the general
                     * check. Since the basic requirement of defeating the enemy is self-explanatory and the
                     * minimum, we'll omit the logic tooltip for them in particular.
                     */
                    if (accessLogicString != "" &&
                        !(randoCheckId >= RC_ENEMY_DROP_ALIEN && randoCheckId <= RC_ENEMY_DROP_WOLFOS)) {
                        UIWidgets::Tooltip(accessLogicString.c_str());
                    }
                    ImGui::TableSetBgColor(ImGuiTableBgTarget_RowBg0, ImGui::IsItemHovered()
                                                                          ? IM_COL32(255, 255, 0, 128)
                                                                          : IM_COL32(255, 255, 255, 0));
                    if (ImGui::IsItemClicked()) {
                        randoSaveCheck.skipped = !randoSaveCheck.skipped;
                    }
                    ImGui::TableNextColumn();
                }
                ImGui::EndTable();
            }
            ImGui::Unindent(20.0f);
        }
        ImGui::PopStyleColor(2);
        ImGui::PopID();
    }
}

namespace Rando {

namespace CheckTracker {

void CheckTrackerWindow::Draw() {
    if (!CVAR_SHOW_CHECK_TRACKER) {
        return;
    }

    if (CVAR_VISIBILITY_MODE == CHECK_TRACKER_VISIBILITY_MODE_ONLY_ON_PAUSE_MENU &&
        (!gPlayState || !gPlayState->pauseCtx.state)) {
        return;
    }

    if ((CVAR_VISIBILITY_MODE == CHECK_TRACKER_VISIBILITY_MODE_BUTTON_TOGGLE ||
         CVAR_VISIBILITY_MODE == CHECK_TRACKER_VISIBILITY_MODE_BUTTON_HOLD) &&
        !sCheckTrackerBtnState) {
        return;
    }

    ImGui::PushStyleColor(ImGuiCol_TitleBgActive, trackerBG);
    ImGui::PushStyleColor(ImGuiCol_TitleBg, trackerBG);
    ImGui::PushStyleColor(ImGuiCol_WindowBg, trackerBG);
    ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0, 0, 0, 0));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 4.0f);

    ImGui::SetNextWindowSize(ImVec2(485.0f, 500.0f), ImGuiCond_FirstUseEver);

    ImGui::Begin("Check Tracker", nullptr, ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoFocusOnAppearing);

    trackerBG.w = ImGui::IsWindowDocked() ? 1.0f : CVAR_TRACKER_OPACITY;
    ImGui::SetWindowFontScale(trackerScale);

    if (!gPlayState || !IS_RANDO) {
        ImGui::SetCursorPosX((ImGui::GetWindowWidth() - ImGui::CalcTextSize("No Rando Save Loaded").x) / 2);
        ImGui::SetCursorPosY(ImGui::GetWindowHeight() / 2 - 10.0f);
        ImGui::TextColored(UIWidgets::ColorValues.at(UIWidgets::Colors::Gray), "No Rando Save Loaded");
        ImGui::End();
        ImGui::PopStyleColor(4);
        ImGui::PopStyleVar(1);
        return;
    }

    if (CVAR_SHOW_SEARCH) {
        bool sameLine = !(ImGui::GetContentRegionAvail().x <= 300.0f * trackerScale);
        auto totalCheckCount = GetTotalCheckCount();

        UIWidgets::PushStyleInput();
        sCheckTrackerFilter.Draw("##filter", (ImGui::GetContentRegionAvail().x -
                                              (sameLine ? (ImGui::CalcTextSize("Total: ").x +
                                                           ImGui::CalcTextSize(totalCheckCount.c_str()).x + 60.0f)
                                                        : 40.0f)));
        UIWidgets::PopStyleInput();

        ImGui::SameLine();
        if (!sCheckTrackerFilter.IsActive()) {
            if (UIWidgets::Button(ICON_FA_ARROWS_V, UIWidgets::ButtonOptions().Size(UIWidgets::Sizes::Inline))) {
                if (gPlayState) {
                    sScrollToTargetScene = GetScrollTargetScene(gPlayState->sceneId);
                    sScrollToTargetEntrance = gSaveContext.save.entrance;
                }
            }
            UIWidgets::Tooltip("Scroll to Current Scene");
        } else {
            if (UIWidgets::Button(ICON_FA_TIMES, UIWidgets::ButtonOptions().Size(UIWidgets::Sizes::Inline))) {
                sCheckTrackerFilter.Clear();
                if (gPlayState) {
                    sScrollToTargetScene = GetScrollTargetScene(gPlayState->sceneId);
                    sScrollToTargetEntrance = gSaveContext.save.entrance;
                }
            }
        }

        if (!sCheckTrackerFilter.IsActive()) {
            ImGui::SameLine(18.0f);
            ImGui::Text("Search");
        }

        if (sameLine) {
            ImGui::SameLine(ImGui::GetContentRegionAvail().x -
                            (ImGui::CalcTextSize("Total: ").x + ImGui::CalcTextSize(totalCheckCount.c_str()).x));
        }
        ImGui::Text("Total: %s", totalCheckCount.c_str());
    }

    if (CVAR_SHOW_CHECK_TYPE_FILTER) {
        auto columns = ImGui::GetContentRegionAvail().x / (24.0f * trackerScale + 15.0f);

        if (ImGui::BeginTable("Type Filter", columns)) {
            for (auto randoCheckType : checkTypeFiltersAvailable) {
                ImGui::TableNextColumn();
                ImVec4 buttonCol = checkTypeFilter.contains(randoCheckType)
                                       ? UIWidgets::ColorValues.at(UIWidgets::Colors::Gray)
                                       : UIWidgets::ColorValues.at(UIWidgets::Colors::NoColor);
                ImGui::PushStyleColor(ImGuiCol_Button, buttonCol);
                ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(1.0f, 1.0f, 1.0f, 0.2f));
                ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(1.0f, 1.0f, 1.0f, 0.1f));
                TexturePtr textureId =
                    std::dynamic_pointer_cast<Fast::Fast3dGui>(Ship::Context::GetRawInstance()->GetWindow()->GetGui())
                        ->GetTextureByName(randoCheckType == RCTYPE_UNKNOWN
                                               ? (const char*)gShootingGalleryOctorokCrossTex
                                               : checkTypeIconList[randoCheckType]);
                if (randoCheckType == RCTYPE_OWL) {
                    ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 4.0f);
                } else if (randoCheckType == RCTYPE_SONG) {
                    ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 4.0f);
                }

                ImVec2 size = ImVec2(24.0f * trackerScale, 24.0f * trackerScale);
                if (randoCheckType == RCTYPE_SONG) {
                    size.x = 18.0f * trackerScale;
                } else if (randoCheckType == RCTYPE_OWL) {
                    size.y = 12.0f * trackerScale;
                }
                ImVec4 tint = ImVec4(1, 1, 1, 1);
                if (randoCheckType == RCTYPE_FREESTANDING) {
                    tint = ImVec4(0.78f, 1, 0.39f, 1);
                } else if (randoCheckType == RCTYPE_UNKNOWN) {
                    tint = ImVec4(1, 0, 0, 1);
                }

                if (ImGui::ImageButton(std::to_string(randoCheckType).c_str(), textureId, size, ImVec2(0, 0),
                                       ImVec2(1, 1), ImVec4(0, 0, 0, 0), tint)) {
                    if (randoCheckType == RCTYPE_UNKNOWN) {
                        checkTypeFilter.clear();
                    } else {
                        if (checkTypeFilter.contains(randoCheckType)) {
                            checkTypeFilter.erase(randoCheckType);
                        } else {
                            checkTypeFilter.insert(randoCheckType);
                        }
                    }
                }
                ImGui::PopStyleColor(3);
            }
            ImGui::EndTable();
        }
    }

    ImGui::BeginChild("Checks");
    CheckTrackerDrawNonLogicalList();
    sExpandedHeadersState = sExpandedHeadersToggle;
    ImGui::EndChild();

    ImGui::End();

    ImGui::PopStyleColor(4);
    ImGui::PopStyleVar(1);
}

void SettingsWindow::DrawElement() {
    if (CVarGetInteger("gWindows.CheckTracker", 0)) {
        UIWidgets::WindowButton("Disable Check Tracker", "gWindows.CheckTracker", BenGui::mRandoCheckTrackerWindow,
                                { .size = UIWidgets::Sizes::Inline, .color = UIWidgets::Colors::Red });
    } else {
        UIWidgets::WindowButton("Enable Check Tracker", "gWindows.CheckTracker", BenGui::mRandoCheckTrackerWindow,
                                { .size = UIWidgets::Sizes::Inline, .color = UIWidgets::Colors::Green });
    }
    if (ImGui::BeginTable("Settings Table", 2)) {
        ImGui::TableSetupColumn("col1", ImGuiTableColumnFlags_WidthStretch);
        ImGui::TableSetupColumn("col2", ImGuiTableColumnFlags_WidthStretch);
        ImGui::TableNextColumn();
        ImGui::SeparatorText("Check Settings");
        // UIWidgets::CVarCheckbox("Dim Out of Logic Checks", CVAR_NAME_OUT_OF_LOGIC_MODE);
        UIWidgets::CVarCombobox("Out of Logic Checks", CVAR_NAME_OUT_OF_LOGIC_MODE, &sCheckModes,
                                UIWidgets::ComboboxOptions()
                                    .DefaultIndex(1)
                                    .ComponentAlignment(UIWidgets::ComponentAlignment::Right)
                                    .LabelPosition(UIWidgets::LabelPosition::Far));
        if (CVAR_OUT_OF_LOGIC_MODE == 1) {
            ImGui::SameLine(ImGui::GetContentRegionAvail().x - 155.0f);
            UIWidgets::CVarColorPicker("##OutOfLogicColor", CVAR_NAME_OUT_OF_LOGIC_COLOR ".Value",
                                       { 255, 255, 255, 100 }, true);
        }
        UIWidgets::CVarCombobox("Collected Checks", CVAR_NAME_COLLECTED_MODE, &sCheckModes,
                                UIWidgets::ComboboxOptions()
                                    .DefaultIndex(2)
                                    .ComponentAlignment(UIWidgets::ComponentAlignment::Right)
                                    .LabelPosition(UIWidgets::LabelPosition::Far));
        if (CVAR_COLLECTED_MODE == 1) {
            ImGui::SameLine(ImGui::GetContentRegionAvail().x - 155.0f);
            UIWidgets::CVarColorPicker("##CollectedColor", CVAR_NAME_COLLECTED_COLOR ".Value", { 100, 255, 100, 255 },
                                       true);
        }
        UIWidgets::CVarCombobox("Skipped Checks", CVAR_NAME_SKIPPED_MODE, &sCheckModes,
                                UIWidgets::ComboboxOptions()
                                    .DefaultIndex(2)
                                    .ComponentAlignment(UIWidgets::ComponentAlignment::Right)
                                    .LabelPosition(UIWidgets::LabelPosition::Far));
        if (CVAR_SKIPPED_MODE == 1) {
            ImGui::SameLine(ImGui::GetContentRegionAvail().x - 155.0f);
            UIWidgets::CVarColorPicker("##SkippedColor", CVAR_NAME_SKIPPED_COLOR ".Value", { 255, 100, 255, 255 },
                                       true);
        }
        UIWidgets::CVarCheckbox("Only Show Current Scene", CVAR_NAME_SHOW_CURRENT_SCENE);
        UIWidgets::CVarCheckbox("Auto Scroll To Current Scene", CVAR_NAME_SCROLL_TO_SCENE,
                                UIWidgets::CheckboxOptions().DefaultValue(true));
        if (UIWidgets::Button("Expand/Collapse All Scenes")) {
            sExpandedHeadersToggle = !sExpandedHeadersToggle;
        }

        ImGui::TableNextColumn();
        ImGui::SeparatorText("Window Settings");
        UIWidgets::CVarCombobox("Visibility", CVAR_NAME_VISIBILITY_MODE, &sCheckVisibilityModes,
                                UIWidgets::ComboboxOptions()
                                    .DefaultIndex(CHECK_TRACKER_VISIBILITY_MODE_ALWAYS)
                                    .ComponentAlignment(UIWidgets::ComponentAlignment::Right)
                                    .LabelPosition(UIWidgets::LabelPosition::Far));
        if (CVAR_VISIBILITY_MODE == CHECK_TRACKER_VISIBILITY_MODE_BUTTON_TOGGLE ||
            CVAR_VISIBILITY_MODE == CHECK_TRACKER_VISIBILITY_MODE_BUTTON_HOLD) {
            UIWidgets::CVarBtnSelector("Button Combination:", CVAR_NAME_VISIBILITY_BTN,
                                       UIWidgets::BtnSelectorOptions().DefaultValue(BTN_CUSTOM_MODIFIER1));
        }
        UIWidgets::CVarCheckbox("Show Search", CVAR_NAME_SHOW_SEARCH, UIWidgets::CheckboxOptions().DefaultValue(true));
        UIWidgets::CVarCheckbox("Show Check Type Filters", CVAR_NAME_SHOW_CHECK_TYPE_FILTER);

        if (UIWidgets::CVarSliderFloat("Opacity", CVAR_NAME_TRACKER_OPACITY,
                                       {
                                           .format = "Opacity: %.1f",
                                           .step = 0.10f,
                                           .min = 0.0f,
                                           .max = 1.0f,
                                           .defaultValue = 0.5f,
                                           .labelPosition = UIWidgets::LabelPosition::None,
                                       })) {
            trackerBG.w = CVAR_TRACKER_OPACITY;
        }
        if (UIWidgets::CVarSliderFloat("Scale", CVAR_NAME_TRACKER_SCALE,
                                       {
                                           .format = "Scale: %.1f",
                                           .step = 0.10f,
                                           .min = 0.7f,
                                           .max = 2.5f,
                                           .defaultValue = 1.0f,
                                           .labelPosition = UIWidgets::LabelPosition::None,
                                       })) {
            trackerScale = CVAR_TRACKER_SCALE;
        }

        ImGui::EndTable();
    }
}

void Init() {
    for (auto& [randoRegionId, randoRegion] : Rando::Logic::Regions) {
        for (auto& [randoCheckId, accessLogicFunc] : randoRegion.checks) {
            accessLogicFuncs[randoCheckId] = accessLogicFunc.second;
        }
    }
    GameInteractor::Instance->RegisterGameHook<GameInteractor::OnSceneInit>([](s8 sceneId, s8 spawnNum) {
        if (CVAR_SCROLL_TO_SCENE) {
            sScrollToTargetScene = GetScrollTargetScene(sceneId);
            sScrollToTargetEntrance = gSaveContext.save.entrance;
        }
    });

    trackerBG = { 0, 0, 0, CVAR_TRACKER_OPACITY };
    trackerScale = CVAR_TRACKER_SCALE;
}

static RegisterShipInitFunc initFunc(
    []() {
        COND_HOOK(OnGameStateMainStart, CVAR_VISIBILITY_MODE >= CHECK_TRACKER_VISIBILITY_MODE_BUTTON_TOGGLE, []() {
            Input* input = CONTROLLER1(gGameState);

            if (CVAR_VISIBILITY_MODE == CHECK_TRACKER_VISIBILITY_MODE_BUTTON_HOLD) {
                if (CHECK_BTN_ALL(input->cur.button, CVAR_VISIBILITY_BTN)) {
                    sCheckTrackerBtnState = true;
                } else {
                    sCheckTrackerBtnState = false;
                }
            } else {
                if (CHECK_BTN_ALL(input->cur.button, CVAR_VISIBILITY_BTN) &&
                    CHECK_BTN_ANY(input->press.button, CVAR_VISIBILITY_BTN)) {
                    sCheckTrackerBtnState = !sCheckTrackerBtnState;
                }
            }
        });
    },
    { CVAR_NAME_VISIBILITY_MODE });

void OnFileLoad() {
    if (!IS_RANDO) {
        return;
    }

    initializeSceneChecks();
}

} // namespace CheckTracker

} // namespace Rando
