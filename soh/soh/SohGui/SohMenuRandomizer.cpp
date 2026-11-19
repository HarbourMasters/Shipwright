#include "soh/Enhancements/randomizer/TrackerLocalization.h"
#include "soh/Enhancements/randomizer/randomizerEnumStrings.h"
#include "soh/Enhancements/Lang/Lang.h"
#include <unordered_set>

#include <ship/Context.h>

#include "soh/Enhancements/game-interactor/GameInteractor.h"
#include "SohMenu.h"
#include "soh/Enhancements/enhancementTypes.h"
#include "soh/Enhancements/randomizer/randomizer_check_objects.h"
#include "soh/Enhancements/randomizer/randomizer.h"
#include "soh/Enhancements/randomizer/settings.h"
#include "soh/ShipUtils.h"
#include "soh/SohGui/SohGui.hpp"
#include "soh/SohGui/UIWidgets.hpp"

extern "C" {
#include "variables.h"
}

namespace SohGui {

extern std::shared_ptr<SohMenu> mSohMenu;
using namespace UIWidgets;

static const std::map<int32_t, const char*> skipGetItemAnimationOptions = {
    { SGIA_DISABLED, "Disabled" },
    { SGIA_JUNK, "Junk Items" },
    { SGIA_ALL, "All Items" },
};

static bool locationsDirty = true;
static bool tricksDirty = true;
static int32_t prevMQDungeonSetting;
static char seedString[MAX_SEED_STRING_SIZE];
static std::set<RandomizerCheck> excludedLocations;
static std::set<RandomizerTrick> enabledTricks;
static std::set<RandomizerTrick> enabledGlitches;

void SaveEnabledTricks() {
    std::string enabledTrickString = "";
    for (auto enabledTrickIt : enabledTricks) {
        enabledTrickString += Rando::Settings::GetInstance()->GetTrickSetting(enabledTrickIt).GetNameTag();
        enabledTrickString += ",";
    }
    if (enabledTricks.size() == 0) {
        CVarClear(CVAR_RANDOMIZER_SETTING("EnabledTricks"));
    } else {
        CVarSetString(CVAR_RANDOMIZER_SETTING("EnabledTricks"), enabledTrickString.c_str());
    }
    Ship::Context::GetRawInstance()->GetWindow()->GetGui()->SaveConsoleVariablesNextFrame();
    tricksDirty = false;
    return;
}

void SaveExcludedLocations() {
    std::string excludedLocationString = Rando::StaticData::SerializeExcludedLocations(excludedLocations);
    if (excludedLocationString == "") {
        CVarClear(CVAR_RANDOMIZER_SETTING("ExcludedLocations"));
    } else {
        CVarSetString(CVAR_RANDOMIZER_SETTING("ExcludedLocations"), excludedLocationString.c_str());
    }
    Ship::Context::GetRawInstance()->GetWindow()->GetGui()->SaveConsoleVariablesNextFrame();
    locationsDirty = true;
}

static const std::string& MenuCheckName(RandomizerCheck check) {
    return TrackerLocalization::Name("checks", EnumToString(check),
                                     Rando::StaticData::GetLocation(check)->GetShortName(), "name");
}
static bool MenuCheckMatches(ImGuiTextFilter& filter, RandomizerCheck check) {
    if (!filter.IsActive())
        return true;
    const std::string search = Rando::StaticData::GetLocation(check)->GetName() + " " + MenuCheckName(check);
    return filter.PassFilter(search.c_str());
}
static std::string MenuAreaLabel(RandomizerCheckArea area, const char* suffix) {
    const std::string key = "ui.trackers.areas." + std::string(EnumToString(area));
    const std::string canonical = RandomizerCheckObjects::GetRCAreaName(area) + suffix;
    return Lang::UILabel(key.c_str(), canonical.c_str());
}

void DrawLocationsMenu(WidgetInfo& info) {
    auto ctx = Rando::Context::GetInstance();
    int32_t currMQDungeonSetting = CVarGetInteger(CVAR_RANDOMIZER_SETTING("MQDungeons"), 0) |
                                   CVarGetInteger(CVAR_RANDOMIZER_SETTING("MQDungeonCount"), 0) << 8;
    static ImVec2 cellPadding(8.0f, 8.0f);
    bool generating = IsRandoGenerating();
    bool disableEditingRandoSettings = generating || CVarGetInteger(CVAR_GENERAL("OnFileSelectNameEntry"), 0);
    ImGui::BeginDisabled(CVarGetInteger(CVAR_SETTING("DisableChanges"), 0) || disableEditingRandoSettings);
    ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, cellPadding);
    if (locationsDirty || currMQDungeonSetting != prevMQDungeonSetting || GameInteractor::IsSaveLoaded()) {
        locationsDirty = false;
        prevMQDungeonSetting = currMQDungeonSetting;
        UpdateMenuLocations();
    } else {
        RandomizerCheckObjects::UpdateImGuiVisibility();
    }

    // Locations
    static std::unordered_set<RandomizerCheckArea> areaTreeIncluded{
        RCAREA_KOKIRI_FOREST,
        RCAREA_LOST_WOODS,
        RCAREA_SACRED_FOREST_MEADOW,
        RCAREA_HYRULE_FIELD,
        RCAREA_LAKE_HYLIA,
        RCAREA_GERUDO_VALLEY,
        RCAREA_GERUDO_FORTRESS,
        RCAREA_WASTELAND,
        RCAREA_DESERT_COLOSSUS,
        RCAREA_MARKET,
        RCAREA_HYRULE_CASTLE,
        RCAREA_KAKARIKO_VILLAGE,
        RCAREA_GRAVEYARD,
        RCAREA_DEATH_MOUNTAIN_TRAIL,
        RCAREA_GORON_CITY,
        RCAREA_DEATH_MOUNTAIN_CRATER,
        RCAREA_ZORAS_RIVER,
        RCAREA_ZORAS_DOMAIN,
        RCAREA_ZORAS_FOUNTAIN,
        RCAREA_LON_LON_RANCH,
        RCAREA_DEKU_TREE,
        RCAREA_DODONGOS_CAVERN,
        RCAREA_JABU_JABUS_BELLY,
        RCAREA_FOREST_TEMPLE,
        RCAREA_FIRE_TEMPLE,
        RCAREA_WATER_TEMPLE,
        RCAREA_SPIRIT_TEMPLE,
        RCAREA_SHADOW_TEMPLE,
        RCAREA_BOTTOM_OF_THE_WELL,
        RCAREA_ICE_CAVERN,
        RCAREA_GERUDO_TRAINING_GROUND,
        RCAREA_GANONS_CASTLE,
    };
    static std::unordered_set<RandomizerCheckArea> areaTreeExcluded{
        RCAREA_KOKIRI_FOREST,
        RCAREA_LOST_WOODS,
        RCAREA_SACRED_FOREST_MEADOW,
        RCAREA_HYRULE_FIELD,
        RCAREA_LAKE_HYLIA,
        RCAREA_GERUDO_VALLEY,
        RCAREA_GERUDO_FORTRESS,
        RCAREA_WASTELAND,
        RCAREA_DESERT_COLOSSUS,
        RCAREA_MARKET,
        RCAREA_HYRULE_CASTLE,
        RCAREA_KAKARIKO_VILLAGE,
        RCAREA_GRAVEYARD,
        RCAREA_DEATH_MOUNTAIN_TRAIL,
        RCAREA_GORON_CITY,
        RCAREA_DEATH_MOUNTAIN_CRATER,
        RCAREA_ZORAS_RIVER,
        RCAREA_ZORAS_DOMAIN,
        RCAREA_ZORAS_FOUNTAIN,
        RCAREA_LON_LON_RANCH,
        RCAREA_DEKU_TREE,
        RCAREA_DODONGOS_CAVERN,
        RCAREA_JABU_JABUS_BELLY,
        RCAREA_FOREST_TEMPLE,
        RCAREA_FIRE_TEMPLE,
        RCAREA_WATER_TEMPLE,
        RCAREA_SPIRIT_TEMPLE,
        RCAREA_SHADOW_TEMPLE,
        RCAREA_BOTTOM_OF_THE_WELL,
        RCAREA_ICE_CAVERN,
        RCAREA_GERUDO_TRAINING_GROUND,
        RCAREA_GANONS_CASTLE,
    };

    static ImGuiTextFilter locationSearch;
    UIWidgets::PushStyleInput(THEME_COLOR);
    locationSearch.Draw(Lang::UILabel("ui.text.sohgui.sohmenurandomizer.filter_inc_exc", "Filter (inc,-exc)"), 490.0f);
    UIWidgets::PopStyleInput();

    if (ImGui::BeginTable("tableRandoLocations", 2, ImGuiTableFlags_BordersH | ImGuiTableFlags_BordersV)) {
        ImGui::TableSetupColumn(Lang::UILabel("ui.text.sohgui.sohmenurandomizer.included", "Included"),
                                ImGuiTableColumnFlags_WidthStretch, 200.0f);
        ImGui::TableSetupColumn(Lang::UILabel("ui.text.sohgui.sohmenurandomizer.excluded", "Excluded"),
                                ImGuiTableColumnFlags_WidthStretch, 200.0f);
        ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
        ImGui::TableHeadersRow();
        ImGui::PopItemFlag();
        ImGui::TableNextRow();

        // COLUMN 1 - INCLUDED LOCATIONS
        ImGui::TableNextColumn();
        // window->DC.CurrLineTextBaseOffset = 0.0f;

        if (UIWidgets::Button(Lang::UILabel("ui.text.sohgui.sohmenurandomizer.collapse_all", "Collapse All##included"),
                              UIWidgets::ButtonOptions().Color(THEME_COLOR).Size(ImVec2(0.f, 0.f)))) {
            for (int i = 0; i < RCAREA_INVALID; i++) {
                areaTreeIncluded.erase(static_cast<RandomizerCheckArea>(i));
            }
        }
        ImGui::SameLine();
        if (UIWidgets::Button(Lang::UILabel("ui.text.sohgui.sohmenurandomizer.open_all", "Open All##included"),
                              UIWidgets::ButtonOptions().Color(THEME_COLOR).Size(ImVec2(0.f, 0.f)))) {
            for (int i = 0; i < RCAREA_INVALID; i++) {
                areaTreeIncluded.insert(static_cast<RandomizerCheckArea>(i));
            }
        }
        ImGui::SameLine();
        if (UIWidgets::Button(Lang::UILabel("ui.text.sohgui.sohmenurandomizer.exclude_visible", "Exclude Visible"),
                              UIWidgets::ButtonOptions().Color(THEME_COLOR).Size(ImVec2(0.f, 0.f)))) {
            for (auto& [rcArea, locations] : RandomizerCheckObjects::GetAllRCObjectsByArea()) {
                if (!areaTreeIncluded.contains(rcArea)) {
                    continue;
                }
                for (RandomizerCheck rc : locations) {
                    if (ctx->GetItemLocation(rc)->IsVisible() && !excludedLocations.count(rc) &&
                        MenuCheckMatches(locationSearch, rc)) {
                        excludedLocations.insert(rc);
                    }
                }
            }
            SaveExcludedLocations();
        }

        ImGui::BeginChild("ChildIncludedLocations", ImVec2(0, -8));
        for (auto& [rcArea, locations] : RandomizerCheckObjects::GetAllRCObjectsByArea()) {
            bool hasItems = false;
            for (RandomizerCheck rc : locations) {
                if (ctx->GetItemLocation(rc)->IsVisible() && !excludedLocations.count(rc) &&
                    MenuCheckMatches(locationSearch, rc)) {

                    hasItems = true;
                    break;
                }
            }

            if (hasItems) {
                std::string areaLabel = MenuAreaLabel(rcArea, "##included");
                ImGui::TreeNodeSetOpen(ImGui::GetID(areaLabel.c_str()), areaTreeIncluded.contains(rcArea));
                ImGui::SetNextItemOpen(true, ImGuiCond_Once);
                if (ImGui::TreeNode(areaLabel.c_str())) {
                    for (auto& location : locations) {
                        if (ctx->GetItemLocation(location)->IsVisible() && !excludedLocations.count(location) &&
                            MenuCheckMatches(locationSearch, location)) {
                            UIWidgets::PushStyleButton(THEME_COLOR, ImVec2(7.f, 5.f));
                            if (ImGui::ArrowButton(std::to_string(location).c_str(), ImGuiDir_Right)) {
                                excludedLocations.insert(location);
                                SaveExcludedLocations();
                            }
                            UIWidgets::PopStyleButton();
                            ImGui::SameLine();
                            ImGui::Text("%s", MenuCheckName(location).c_str());
                        }
                    }
                    areaTreeIncluded.insert(rcArea);
                    ImGui::TreePop();
                } else {
                    areaTreeIncluded.erase(rcArea);
                }
            }
        }
        ImGui::EndChild();

        // COLUMN 2 - EXCLUDED LOCATIONS
        ImGui::TableNextColumn();
        // window->DC.CurrLineTextBaseOffset = 0.0f;

        if (UIWidgets::Button(Lang::UILabel("ui.text.sohgui.sohmenurandomizer.collapse_all", "Collapse All##excluded"),
                              UIWidgets::ButtonOptions().Color(THEME_COLOR).Size(ImVec2(0.f, 0.f)))) {
            for (int i = 0; i < RCAREA_INVALID; i++) {
                areaTreeExcluded.erase(static_cast<RandomizerCheckArea>(i));
            }
        }
        ImGui::SameLine();
        if (UIWidgets::Button(Lang::UILabel("ui.text.sohgui.sohmenurandomizer.open_all", "Open All##excluded"),
                              UIWidgets::ButtonOptions().Color(THEME_COLOR).Size(ImVec2(0.f, 0.f)))) {
            for (int i = 0; i < RCAREA_INVALID; i++) {
                areaTreeExcluded.insert(static_cast<RandomizerCheckArea>(i));
            }
        }
        ImGui::SameLine();
        if (UIWidgets::Button(Lang::UILabel("ui.text.sohgui.sohmenurandomizer.include_visible", "Include Visible"),
                              UIWidgets::ButtonOptions().Color(THEME_COLOR).Size(ImVec2(0.f, 0.f)))) {
            for (auto& [rcArea, locations] : RandomizerCheckObjects::GetAllRCObjectsByArea()) {
                if (!areaTreeExcluded.contains(rcArea)) {
                    continue;
                }
                for (RandomizerCheck rc : locations) {
                    if (ctx->GetItemLocation(rc)->IsVisible() && excludedLocations.count(rc) &&
                        MenuCheckMatches(locationSearch, rc)) {
                        excludedLocations.erase(rc);
                    }
                }
            }
            SaveExcludedLocations();
        }

        ImGui::BeginChild("ChildExcludedLocations", ImVec2(0, -8));
        for (auto& [rcArea, locations] : RandomizerCheckObjects::GetAllRCObjectsByArea()) {
            bool hasItems = false;
            for (RandomizerCheck rc : locations) {
                if (ctx->GetItemLocation(rc)->IsVisible() && excludedLocations.count(rc) &&
                    MenuCheckMatches(locationSearch, rc)) {

                    hasItems = true;
                    break;
                }
            }

            if (hasItems) {
                std::string areaLabel = MenuAreaLabel(rcArea, "##excluded");
                ImGui::TreeNodeSetOpen(ImGui::GetID(areaLabel.c_str()), areaTreeExcluded.contains(rcArea));
                ImGui::SetNextItemOpen(true, ImGuiCond_Once);
                if (ImGui::TreeNode(areaLabel.c_str())) {
                    for (auto& location : locations) {
                        auto elfound = excludedLocations.find(location);
                        if (ctx->GetItemLocation(location)->IsVisible() && elfound != excludedLocations.end() &&
                            MenuCheckMatches(locationSearch, location)) {
                            UIWidgets::PushStyleButton(THEME_COLOR, ImVec2(7.f, 5.f));
                            if (ImGui::ArrowButton(std::to_string(location).c_str(), ImGuiDir_Left)) {
                                excludedLocations.erase(elfound);
                                SaveExcludedLocations();
                            }
                            UIWidgets::PopStyleButton();
                            ImGui::SameLine();
                            ImGui::Text("%s", MenuCheckName(location).c_str());
                        }
                    }
                    areaTreeExcluded.insert(rcArea);
                    ImGui::TreePop();
                } else {
                    areaTreeExcluded.erase(rcArea);
                }
            }
        }
        ImGui::EndChild();

        ImGui::EndTable();
    }
    ImGui::PopStyleVar(1);
    // ImGui::EndTabItem();
    ImGui::EndDisabled();
}

void MarkRandomizerMenusDirty() {
    locationsDirty = true;
    tricksDirty = true;
}

void UpdateMenuLocations() {
    RandomizerCheckObjects::UpdateImGuiVisibility();
    excludedLocations =
        Rando::StaticData::ParseExcludedLocations(CVarGetString(CVAR_RANDOMIZER_SETTING("ExcludedLocations"), ""));
}

void UpdateMenuTricks() {
    // RandomizerTricks::UpdateImGuiVisibility();
    //  todo: this efficiently when we build out cvar array support
    std::stringstream enabledTrickStringStream(CVarGetString(CVAR_RANDOMIZER_SETTING("EnabledTricks"), ""));
    std::string enabledTrickString;
    enabledTricks.clear();
    while (getline(enabledTrickStringStream, enabledTrickString, ',')) {
        if (Rando::StaticData::trickToEnum.contains(enabledTrickString)) {
            enabledTricks.insert(Rando::StaticData::trickToEnum[enabledTrickString]);
        }
    }
    std::stringstream enabledGlitchStringStream(CVarGetString(CVAR_RANDOMIZER_SETTING("EnabledGlitches"), ""));
    std::string enabledGlitchString;
    enabledGlitches.clear();
    while (getline(enabledGlitchStringStream, enabledGlitchString, ',')) {
        if (!enabledGlitchString.empty()) {
            enabledGlitches.insert((RandomizerTrick)std::stoi(enabledGlitchString));
        }
    }
}

void DrawTricksMenu(WidgetInfo& info) {
    auto ctx = Rando::Context::GetInstance();
    auto randoSettings = Rando::Settings::GetInstance();
    static ImVec2 cellPadding(8.0f, 8.0f);
    bool generating = IsRandoGenerating();
    bool disableEditingRandoSettings = generating || CVarGetInteger(CVAR_GENERAL("OnFileSelectNameEntry"), 0);
    if (tricksDirty) {
        tricksDirty = false;
        UpdateMenuTricks();
    }

    ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, cellPadding);
    ImGui::BeginDisabled(CVarGetInteger(CVAR_SETTING("DisableChanges"), 0) || disableEditingRandoSettings);

    // Tricks
    static std::unordered_set<RandomizerArea> areaTreeDisabled{
        RA_NONE,
        RA_KOKIRI_FOREST,
        RA_THE_LOST_WOODS,
        RA_SACRED_FOREST_MEADOW,
        RA_HYRULE_FIELD,
        RA_LAKE_HYLIA,
        RA_GERUDO_VALLEY,
        RA_GERUDO_FORTRESS,
        RA_HAUNTED_WASTELAND,
        RA_DESERT_COLOSSUS,
        RA_THE_MARKET,
        RA_HYRULE_CASTLE,
        RA_KAKARIKO_VILLAGE,
        RA_THE_GRAVEYARD,
        RA_DEATH_MOUNTAIN_TRAIL,
        RA_GORON_CITY,
        RA_DEATH_MOUNTAIN_CRATER,
        RA_ZORAS_RIVER,
        RA_ZORAS_DOMAIN,
        RA_ZORAS_FOUNTAIN,
        RA_LON_LON_RANCH,
        RA_DEKU_TREE,
        RA_DODONGOS_CAVERN,
        RA_JABU_JABUS_BELLY,
        RA_FOREST_TEMPLE,
        RA_FIRE_TEMPLE,
        RA_WATER_TEMPLE,
        RA_SPIRIT_TEMPLE,
        RA_SHADOW_TEMPLE,
        RA_BOTTOM_OF_THE_WELL,
        RA_ICE_CAVERN,
        RA_GERUDO_TRAINING_GROUND,
        RA_GANONS_CASTLE,
    };
    static std::unordered_set<RandomizerArea> areaTreeEnabled{
        RA_NONE,
        RA_KOKIRI_FOREST,
        RA_THE_LOST_WOODS,
        RA_SACRED_FOREST_MEADOW,
        RA_HYRULE_FIELD,
        RA_LAKE_HYLIA,
        RA_GERUDO_VALLEY,
        RA_GERUDO_FORTRESS,
        RA_HAUNTED_WASTELAND,
        RA_DESERT_COLOSSUS,
        RA_THE_MARKET,
        RA_HYRULE_CASTLE,
        RA_KAKARIKO_VILLAGE,
        RA_THE_GRAVEYARD,
        RA_DEATH_MOUNTAIN_TRAIL,
        RA_GORON_CITY,
        RA_DEATH_MOUNTAIN_CRATER,
        RA_ZORAS_RIVER,
        RA_ZORAS_DOMAIN,
        RA_ZORAS_FOUNTAIN,
        RA_LON_LON_RANCH,
        RA_DEKU_TREE,
        RA_DODONGOS_CAVERN,
        RA_JABU_JABUS_BELLY,
        RA_FOREST_TEMPLE,
        RA_FIRE_TEMPLE,
        RA_WATER_TEMPLE,
        RA_SPIRIT_TEMPLE,
        RA_SHADOW_TEMPLE,
        RA_BOTTOM_OF_THE_WELL,
        RA_ICE_CAVERN,
        RA_GERUDO_TRAINING_GROUND,
        RA_GANONS_CASTLE,
    };

    static std::map<Rando::Tricks::Tag, bool> showTag{
        { Rando::Tricks::Tag::NOVICE, true },   { Rando::Tricks::Tag::INTERMEDIATE, true },
        { Rando::Tricks::Tag::ADVANCED, true }, { Rando::Tricks::Tag::EXPERT, true },
        { Rando::Tricks::Tag::EXTREME, true },  { Rando::Tricks::Tag::EXPERIMENTAL, true },
        { Rando::Tricks::Tag::GLITCH, false },
    };

    static ImGuiTextFilter trickSearch;
    UIWidgets::PushStyleInput(THEME_COLOR);
    trickSearch.Draw(Lang::UILabel("ui.text.sohgui.sohmenurandomizer.filter_inc_exc", "Filter (inc,-exc)"), 490.0f);
    UIWidgets::PopStyleInput();
    if (CVarGetInteger(CVAR_RANDOMIZER_SETTING("NoLogic"), RO_GENERIC_OFF) == RO_GENERIC_OFF) {
        ImGui::SameLine();
        if (UIWidgets::Button(Lang::UILabel("ui.text.sohgui.sohmenurandomizer.disable_all", "Disable All"),
                              UIWidgets::ButtonOptions().Color(THEME_COLOR).Size(ImVec2(250.f, 0.f)))) {
            for (int i = 0; i < RT_MAX; i++) {
                auto etfound = enabledTricks.find(static_cast<RandomizerTrick>(i));
                if (etfound != enabledTricks.end()) {
                    enabledTricks.erase(etfound);
                }
            }
            SaveEnabledTricks();
        }
        ImGui::SameLine();
        if (UIWidgets::Button(Lang::UILabel("ui.text.sohgui.sohmenurandomizer.enable_all", "Enable All"),
                              UIWidgets::ButtonOptions().Color(THEME_COLOR).Size(ImVec2(250.f, 0.f)))) {
            for (int i = 0; i < RT_MAX; i++) {
                if (!enabledTricks.count(static_cast<RandomizerTrick>(i))) {
                    enabledTricks.insert(static_cast<RandomizerTrick>(i));
                }
            }
            SaveEnabledTricks();
        }
    }
    if (ImGui::BeginTable("trickTags", static_cast<int>(showTag.size()),
                          ImGuiTableFlags_Resizable | ImGuiTableFlags_NoSavedSettings | ImGuiTableFlags_Borders)) {
        for (auto [rtTag, isShown] : showTag) {
            ImGui::TableNextColumn();
            if (isShown) {
                ImGui::PushStyleColor(ImGuiCol_Text, Rando::Tricks::GetTextColor(rtTag));
            } else {
                ImGui::PushStyleColor(ImGuiCol_Text, { 1.0f, 1.0f, 1.0f, 1.0f });
            }
            ImGui::PushStyleColor(ImGuiCol_Header, Rando::Tricks::GetTagColor(rtTag));
            ImGui::Selectable(Rando::Tricks::GetTagName(rtTag).c_str(), &showTag[rtTag]);
            ImGui::PopStyleColor(2);
        }
        ImGui::EndTable();
    }

    if (ImGui::BeginTable("tableRandoTricks", 2, ImGuiTableFlags_BordersH | ImGuiTableFlags_BordersV)) {
        ImGui::TableSetupColumn(Lang::UILabel("ui.text.sohgui.sohmenurandomizer.disabled_tricks", "Disabled Tricks"),
                                ImGuiTableColumnFlags_WidthStretch, 200.0f);
        ImGui::TableSetupColumn(Lang::UILabel("ui.text.sohgui.sohmenurandomizer.enabled_tricks", "Enabled Tricks"),
                                ImGuiTableColumnFlags_WidthStretch, 200.0f);
        ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
        ImGui::TableHeadersRow();
        ImGui::PopItemFlag();
        ImGui::TableNextRow();

        if (CVarGetInteger(CVAR_RANDOMIZER_SETTING("NoLogic"), RO_GENERIC_OFF) == RO_GENERIC_OFF) {
            // COLUMN 1 - DISABLED TRICKS
            ImGui::TableNextColumn();
            // window->DC.CurrLineTextBaseOffset = 0.0f;

            if (UIWidgets::Button(
                    Lang::UILabel("ui.text.sohgui.sohmenurandomizer.collapse_all", "Collapse All##disabled"),
                    UIWidgets::ButtonOptions().Color(THEME_COLOR).Size(ImVec2(0.f, 0.f)))) {
                for (int i = 0; i < RA_MAX; i++) {
                    areaTreeDisabled.erase(static_cast<RandomizerArea>(i));
                }
            }
            ImGui::SameLine();
            if (UIWidgets::Button(Lang::UILabel("ui.text.sohgui.sohmenurandomizer.open_all", "Open All##disabled"),
                                  UIWidgets::ButtonOptions().Color(THEME_COLOR).Size(ImVec2(0.f, 0.f)))) {
                for (int i = 0; i < RA_MAX; i++) {
                    areaTreeDisabled.insert(static_cast<RandomizerArea>(i));
                }
            }
            ImGui::SameLine();
            if (UIWidgets::Button(Lang::UILabel("ui.text.sohgui.sohmenurandomizer.enable_visible", "Enable Visible"),
                                  UIWidgets::ButtonOptions().Color(THEME_COLOR).Size(ImVec2(0.f, 0.f)))) {
                for (int i = 0; i < RT_MAX; i++) {
                    auto option = randoSettings->GetTrickSetting(static_cast<RandomizerTrick>(i));
                    if (!enabledTricks.count(static_cast<RandomizerTrick>(i)) &&
                        trickSearch.PassFilter(option.GetName().c_str()) &&
                        areaTreeDisabled.contains(option.GetArea()) &&
                        Rando::Tricks::CheckTags(showTag, option.GetTags())) {
                        enabledTricks.insert(static_cast<RandomizerTrick>(i));
                    }
                }
                SaveEnabledTricks();
            }

            ImGui::BeginChild("ChildTricksDisabled", ImVec2(0, -8), false, ImGuiWindowFlags_HorizontalScrollbar);

            for (auto [area, trickIds] : randoSettings->mTricksByArea) {
                bool hasTricks = false;
                for (auto rt : trickIds) {
                    auto option = randoSettings->GetTrickSetting(rt);
                    if (!option.IsHidden() && trickSearch.PassFilter(option.GetName().c_str()) &&
                        !enabledTricks.count(rt) && Rando::Tricks::CheckTags(showTag, option.GetTags())) {
                        hasTricks = true;
                        break;
                    }
                }
                if (hasTricks) {
                    std::string areaLabel = Rando::Tricks::GetAreaName(area) + "##disabled";
                    ImGui::TreeNodeSetOpen(ImGui::GetID(areaLabel.c_str()), areaTreeDisabled.contains(area));
                    ImGui::SetNextItemOpen(true, ImGuiCond_Once);
                    if (ImGui::TreeNode(areaLabel.c_str())) {
                        for (auto rt : trickIds) {
                            auto option = randoSettings->GetTrickSetting(rt);
                            if (!option.IsHidden() && trickSearch.PassFilter(option.GetName().c_str()) &&
                                !enabledTricks.count(rt) && Rando::Tricks::CheckTags(showTag, option.GetTags())) {
                                UIWidgets::PushStyleButton(THEME_COLOR, ImVec2(7.f, 5.f));
                                if (ImGui::ArrowButton(std::to_string(rt).c_str(), ImGuiDir_Right)) {
                                    enabledTricks.insert(rt);
                                    SaveEnabledTricks();
                                }
                                UIWidgets::PopStyleButton();
                                Rando::Tricks::DrawTagChips(option.GetTags(), option.GetName());
                                ImGui::SameLine();
                                ImGui::Text("%s", option.GetName().c_str());
                                UIWidgets::Tooltip(option.GetDescription());
                            }
                        }
                        areaTreeDisabled.insert(area);
                        ImGui::TreePop();
                    } else {
                        areaTreeDisabled.erase(area);
                    }
                }
            }
            ImGui::EndChild();

            // COLUMN 2 - ENABLED TRICKS
            ImGui::TableNextColumn();
            // window->DC.CurrLineTextBaseOffset = 0.0f;

            if (UIWidgets::Button(
                    Lang::UILabel("ui.text.sohgui.sohmenurandomizer.collapse_all", "Collapse All##enabled"),
                    UIWidgets::ButtonOptions().Color(THEME_COLOR).Size(ImVec2(0.f, 0.f)))) {
                for (int i = 0; i < RA_MAX; i++) {
                    areaTreeEnabled.erase(static_cast<RandomizerArea>(i));
                }
            }
            ImGui::SameLine();
            if (UIWidgets::Button(Lang::UILabel("ui.text.sohgui.sohmenurandomizer.open_all", "Open All##enabled"),
                                  UIWidgets::ButtonOptions().Color(THEME_COLOR).Size(ImVec2(0.f, 0.f)))) {
                for (int i = 0; i < RA_MAX; i++) {
                    areaTreeEnabled.insert(static_cast<RandomizerArea>(i));
                }
            }
            ImGui::SameLine();
            if (UIWidgets::Button(Lang::UILabel("ui.text.sohgui.sohmenurandomizer.disable_visible", "Disable Visible"),
                                  UIWidgets::ButtonOptions().Color(THEME_COLOR).Size(ImVec2(0.f, 0.f)))) {
                for (int i = 0; i < RT_MAX; i++) {
                    auto option = randoSettings->GetTrickSetting(static_cast<RandomizerTrick>(i));
                    if (enabledTricks.count(static_cast<RandomizerTrick>(i)) &&
                        trickSearch.PassFilter(option.GetName().c_str()) &&
                        areaTreeEnabled.contains(option.GetArea()) &&
                        Rando::Tricks::CheckTags(showTag, option.GetTags())) {
                        enabledTricks.erase(static_cast<RandomizerTrick>(i));
                    }
                }
                SaveEnabledTricks();
            }

            ImGui::BeginChild("ChildTricksEnabled", ImVec2(0, -8), false, ImGuiWindowFlags_HorizontalScrollbar);

            for (auto [area, trickIds] : randoSettings->mTricksByArea) {
                bool hasTricks = false;
                for (auto rt : trickIds) {
                    auto option = randoSettings->GetTrickSetting(rt);
                    if (!option.IsHidden() && trickSearch.PassFilter(option.GetName().c_str()) &&
                        enabledTricks.count(rt) && Rando::Tricks::CheckTags(showTag, option.GetTags())) {
                        hasTricks = true;
                        break;
                    }
                }
                if (hasTricks) {
                    std::string areaLabel = Rando::Tricks::GetAreaName(area) + "##enabled";
                    ImGui::TreeNodeSetOpen(ImGui::GetID(areaLabel.c_str()), areaTreeEnabled.contains(area));
                    ImGui::SetNextItemOpen(true, ImGuiCond_Once);
                    if (ImGui::TreeNode(areaLabel.c_str())) {
                        for (auto rt : trickIds) {
                            auto option = randoSettings->GetTrickSetting(rt);
                            if (!option.IsHidden() && trickSearch.PassFilter(option.GetName().c_str()) &&
                                enabledTricks.count(rt) && Rando::Tricks::CheckTags(showTag, option.GetTags())) {
                                UIWidgets::PushStyleButton(THEME_COLOR, ImVec2(7.f, 5.f));
                                if (ImGui::ArrowButton(std::to_string(rt).c_str(), ImGuiDir_Left)) {
                                    enabledTricks.erase(rt);
                                    SaveEnabledTricks();
                                }
                                UIWidgets::PopStyleButton();
                                Rando::Tricks::DrawTagChips(option.GetTags(), option.GetName());
                                ImGui::SameLine();
                                ImGui::Text("%s", option.GetName().c_str());
                                UIWidgets::Tooltip(option.GetDescription());
                            }
                        }
                        areaTreeEnabled.insert(area);
                        ImGui::TreePop();
                    } else {
                        areaTreeEnabled.erase(area);
                    }
                }
            }

            ImGui::EndChild();
        } else {
            ImGui::TableNextColumn();
            ImGui::BeginChild("ChildTricksDisabled", ImVec2(0, -8));
            ImGui::TextUnformatted(
                Lang::UIString("ui.text.sohgui.sohmenurandomizer.disabled_for_no_logic", "Disabled for No Logic."));
            ImGui::EndChild();
            ImGui::TableNextColumn();
            ImGui::BeginChild("ChildTricksEnabled", ImVec2(0, -8));
            ImGui::TextUnformatted(
                Lang::UIString("ui.text.sohgui.sohmenurandomizer.disabled_for_no_logic", "Disabled for No Logic."));
            ImGui::EndChild();
        }
        ImGui::EndTable();
    }
    ImGui::EndDisabled();
    ImGui::PopStyleVar(1);
}

void SohMenu::AddMenuRandomizer() {
    // Add Randomizer Menu
    AddMenuEntry("Randomizer", CVAR_SETTING("Menu.RandomizerSidebarSection"));

    // Seed Settings
    WidgetPath path = { "Randomizer", "General", SECTION_COLUMN_1 };
    AddSidebarEntry("Randomizer", path.sidebarName, 2);
    AddWidget(path,
              "Be sure to explore the Presets and Enhancements Menus for various Speedups and Quality of Life changes!",
              WIDGET_TEXT)
        .Translation("ui.panels.sohmenurandomizer.be_sure_to_explore_the_presets_and_enhancements_menus_for_various_"
                     "speedups_and_quality_of_life_chang")
        .Options(TextOptions().Color(UIWidgets::Colors::Gray));
    AddWidget(path, "Seed Entry", WIDGET_SEPARATOR_TEXT).Translation("ui.panels.sohmenurandomizer.seed_entry");
    AddWidget(path, "Manual seed entry", WIDGET_CVAR_CHECKBOX)
        .CVar(CVAR_RANDOMIZER_SETTING("ManualSeedEntry"))
        .Options(CheckboxOptions().DefaultValue(true));
    AddWidget(path, "Seed", WIDGET_CUSTOM)
        .Translation("ui.panels.sohmenurandomizer.seed")
        .CustomFunction([](WidgetInfo& info) {
            if (CVarGetInteger(CVAR_RANDOMIZER_SETTING("ManualSeedEntry"), 0)) {
                UIWidgets::PushStyleInput(THEME_COLOR);
                ImGui::InputText("##RandomizerSeed", seedString, MAX_SEED_STRING_SIZE,
                                 ImGuiInputTextFlags_CallbackCharFilter, UIWidgets::TextFilters::FilterAlphaNum);
                UIWidgets::Tooltip(Lang::UIString("ui.panels.sohmenurandomizer.seed.description",
                                                  "Characters from a-z, A-Z, and 0-9 are supported.\n"
                                                  "Character limit is 1023, after which the seed will be truncated."));
                ImGui::SameLine();
                if (UIWidgets::Button(
                        ICON_FA_RANDOM,
                        UIWidgets::ButtonOptions()
                            .Size(UIWidgets::Sizes::Inline)
                            .Color(THEME_COLOR)
                            .Padding(ImVec2(10.f, 6.f))
                            .Tooltip(Lang::UIString(
                                "ui.text.sohgui.sohmenurandomizer.creates_a_new_random_seed_value_to_be_used_when_"
                                "generating_a_randomizer",
                                "Creates a new random seed value to be used when generating a randomizer")))) {
                    for (size_t i = 0; i < 10; i++) {
                        seedString[i] = '0' + ShipUtils::Random(0, 10);
                    }
                    seedString[10] = '\0';
                }
                ImGui::SameLine();
                if (UIWidgets::Button(ICON_FA_ERASER, UIWidgets::ButtonOptions()
                                                          .Size(UIWidgets::Sizes::Inline)
                                                          .Color(THEME_COLOR)
                                                          .Padding(ImVec2(10.f, 6.f)))) {
                    seedString[0] = 0;
                }
                if (seedString[0] == 0) {
                    ImGui::SameLine(17.0f);
                    ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 0.4f), "%s",
                                       Lang::UIString("ui.text.sohgui.sohmenurandomizer.leave_blank_for_random_seed",
                                                      "Leave blank for random seed"));
                }
                UIWidgets::PopStyleInput();
            }
        });
    AddWidget(path, "Generate Randomizer", WIDGET_BUTTON)
        .Translation("ui.panels.sohmenurandomizer.generate_randomizer")
        .Callback([](WidgetInfo& info) {
            GenerateRandomizer(CVarGetInteger(CVAR_RANDOMIZER_SETTING("ManualSeedEntry"), 0) ? seedString : "");
        })
        .PreFunc([](WidgetInfo& info) {
            info.options->disabled = (gSaveContext.gameMode != GAMEMODE_FILE_SELECT) || GameInteractor::IsSaveLoaded();
        })
        .Options(ButtonOptions()
                     .Size(ImVec2(250.f, 0.f))
                     .DisabledTooltip(
                         Lang::UIString("ui.panels.sohmenurandomizer.generate_randomizer.disabled_messages.default",
                                        "Must be on File Select to generate a randomizer seed.")));
    AddWidget(path, "Randomize All Settings", WIDGET_BUTTON)
        .Translation("ui.panels.sohmenurandomizer.randomize_all_settings")
        .Callback([](WidgetInfo& info) { Rando::Settings::GetInstance()->RandomizeAllSettings(); })
        .PreFunc([](WidgetInfo& info) {
            info.options->disabled = IsRandoGenerating() || CVarGetInteger(CVAR_GENERAL("OnFileSelectNameEntry"), 0);
        })
        .Options(ButtonOptions()
                     .Size(ImVec2(250.f, 0.f))
                     .Tooltip(Lang::UIString(
                         "ui.panels.sohmenurandomizer.randomize_all_settings.description",
                         "Randomizes all randomizer settings to random valid values (excludes tricks).")))
        .SameLine(true);
    AddWidget(path, "Spoiler File", WIDGET_CUSTOM)
        .Translation("ui.panels.sohmenurandomizer.spoiler_file")
        .CustomFunction([](WidgetInfo& info) {
            if (!CVarGetInteger(CVAR_RANDOMIZER_SETTING("DontGenerateSpoiler"), 0)) {
                ImGui::Text(Lang::UIFormat("ui.text.sohgui.sohmenurandomizer.spoiler_file_s", "Spoiler File: %s"),
                            CVarGetString(CVAR_GENERAL("SpoilerLog"), ""));
            }
        });

    // Enhancements
    AddWidget(path, "Enhancements", WIDGET_SEPARATOR_TEXT).Translation("ui.panels.sohmenurandomizer.enhancements");
    AddWidget(path, "These enhancements are only useful in the Randomizer mode but do not affect the randomizer logic.",
              WIDGET_TEXT)
        .Translation("ui.panels.sohmenurandomizer.these_enhancements_are_only_useful_in_the_randomizer_mode_but_do_not_"
                     "affect_the_randomizer_logic_label")
        .Options(TextOptions().Color(UIWidgets::Colors::Gray));
    AddWidget(path, "Rando-Relevant Navi Hints", WIDGET_CVAR_CHECKBOX)
        .CVar(CVAR_RANDOMIZER_ENHANCEMENT("RandoRelevantNavi"))
        .Options(CheckboxOptions()
                     .Tooltip(Lang::UIString("ui.widgets.gRandoEnhancements.RandoRelevantNavi.description",
                                             "Replace Navi's overworld quest hints with rando-related gameplay hints."))
                     .DefaultValue(true));
    AddWidget(path, "Random Rupee Names", WIDGET_CVAR_CHECKBOX)
        .CVar(CVAR_RANDOMIZER_ENHANCEMENT("RandomizeRupeeNames"))
        .RaceDisable(false)
        .Options(
            CheckboxOptions()
                .Tooltip(Lang::UIString("ui.widgets.gRandoEnhancements.RandomizeRupeeNames.description",
                                        "When obtaining Rupees, randomize what the Rupee is called in the textbox."))
                .DefaultValue(true));
    AddWidget(path, "Map & Compass Colors Match Dungeon", WIDGET_CVAR_CHECKBOX)
        .CVar(CVAR_RANDOMIZER_ENHANCEMENT("ColoredMapsAndCompasses"))
        .Options(CheckboxOptions()
                     .Tooltip(Lang::UIString(
                         "ui.widgets.gRandoEnhancements.ColoredMapsAndCompasses.description",
                         "Matches the color of maps & compasses to the dungeon they belong to. "
                         "This helps identify maps & compasses from afar and adds a little bit of flair.\n\nThis only "
                         "applies to seeds with maps & compasses shuffled to \"Any Dungeon\", \"Overworld\", or "
                         "\"Anywhere\"."))
                     .DefaultValue(true));
    AddWidget(path, "Jabber Nut Colors Match Kind", WIDGET_CVAR_CHECKBOX)
        .CVar(CVAR_RANDOMIZER_ENHANCEMENT("GenericJabberNutModel"))
        .RaceDisable(false)
        .Options(CheckboxOptions()
                     .Tooltip(Lang::UIString("ui.widgets.gRandoEnhancements.GenericJabberNutModel.description",
                                             "With Shuffle Speak, the jabber nut model & color will be generic."))
                     .DefaultValue(true));
    AddWidget(path, "Quest Item Fanfares", WIDGET_CVAR_CHECKBOX)
        .CVar(CVAR_RANDOMIZER_ENHANCEMENT("QuestItemFanfares"))
        .RaceDisable(false)
        .Options(CheckboxOptions().Tooltip(
            Lang::UIString("ui.widgets.gRandoEnhancements.QuestItemFanfares.description",
                           "Play unique fanfares when obtaining quest items (medallions/stones/songs). Note that these "
                           "fanfares can be longer than usual.")));
    AddWidget(path, "Mysterious Shuffled Items", WIDGET_CVAR_CHECKBOX)
        .CVar(CVAR_RANDOMIZER_ENHANCEMENT("MysteriousShuffle"))
        .Options(CheckboxOptions().Tooltip(Lang::UIString(
            "ui.widgets.gRandoEnhancements.MysteriousShuffle.description",
            "Displays a \"Mystery Item\" model in place of any freestanding/GS/shop items that were shuffled, "
            "and replaces item names for them and scrubs and merchants, regardless of hint settings, "
            "so you never know what you're getting.")));
    AddWidget(path, "Simpler Boss Soul Models", WIDGET_CVAR_CHECKBOX)
        .CVar(CVAR_RANDOMIZER_ENHANCEMENT("SimplerBossSoulModels"))
        .RaceDisable(false)
        .Options(CheckboxOptions().Tooltip(Lang::UIString(
            "ui.widgets.gRandoEnhancements.SimplerBossSoulModels.description",
            "When shuffling boss souls, they'll appear as a simpler model instead of showing the bosses' models. "
            "This might make boss souls more distinguishable from a distance, and can help with performance.")));
    AddWidget(path, "Skip Get Item Animations", WIDGET_CVAR_COMBOBOX)
        .CVar(CVAR_RANDOMIZER_ENHANCEMENT("TimeSavers.SkipGetItemAnimation"))
        .Options(ComboboxOptions().ComboMap(skipGetItemAnimationOptions).DefaultIndex(SGIA_JUNK));
    AddWidget(path, "Item Scale: %.2f", WIDGET_CVAR_SLIDER_FLOAT)
        .CVar(CVAR_RANDOMIZER_ENHANCEMENT("TimeSavers.SkipGetItemAnimationScale"))
        .PreFunc([](WidgetInfo& info) {
            info.options->disabled =
                !CVarGetInteger(CVAR_RANDOMIZER_ENHANCEMENT("TimeSavers.SkipGetItemAnimation"), SGIA_JUNK);
            info.options->disabledTooltip =
                "This slider only applies when using the \"Skip Get Item Animations\" option.";
        })
        .Options(FloatSliderOptions().Min(5.0f).Max(15.0f).Format("%.2f").DefaultValue(10.0f).Tooltip(
            Lang::UIString("ui.widgets.gRandoEnhancements.TimeSavers.SkipGetItemAnimationScale.description",
                           "The size of the item when it is picked up.")));
    AddWidget(path, "Signs Hint Entrances", WIDGET_CVAR_CHECKBOX)
        .CVar(CVAR_RANDOMIZER_ENHANCEMENT("EntrancesOnSigns"))
        .Options(CheckboxOptions().Tooltip(
            Lang::UIString("ui.widgets.gRandoEnhancements.EntrancesOnSigns.description",
                           "If enabled, signs near loading zones will tell you where they lead.")));

    auto randoSettings = Rando::Settings::GetInstance();
    randoSettings->CreateOptions();
    randoSettings->GetOptionGroup(RSG_MENU_SIDEBAR_LOGIC_ACCESS).AddWidgets(path);
    randoSettings->GetOptionGroup(RSG_MENU_SIDEBAR_DUNGEONS).AddWidgets(path);
    randoSettings->GetOptionGroup(RSG_MENU_SIDEBAR_SHUFFLES).AddWidgets(path);
    randoSettings->GetOptionGroup(RSG_MENU_SIDEBAR_HINTS_TRAPS).AddWidgets(path);
    path.sidebarName = "Starting Items";
    AddSidebarEntry("Randomizer", path.sidebarName, 1);
    AddWidget(path, "Starting Items", WIDGET_CUSTOM)
        .Translation("ui.panels.sohmenurandomizer.starting_items")
        .CustomFunction(DrawStartingItemsMenu);
    path.sidebarName = "Check Locations";
    AddSidebarEntry("Randomizer", path.sidebarName, 1);
    AddWidget(path, "Excluded Locations", WIDGET_CUSTOM)
        .Translation("ui.panels.sohmenurandomizer.excluded_locations")
        .CustomFunction(DrawLocationsMenu);
    path.sidebarName = "Tricks/Glitches";
    AddSidebarEntry("Randomizer", path.sidebarName, 1);
    AddWidget(path, "Tricks/Glitches", WIDGET_CUSTOM)
        .Translation("ui.panels.sohmenurandomizer.tricks_glitches")
        .CustomFunction(DrawTricksMenu);

    // Plandomizer
    path.sidebarName = "Plandomizer";
    AddSidebarEntry("Randomizer", path.sidebarName, 1);
    AddWidget(path, "Popout Plandomizer Window", WIDGET_WINDOW_BUTTON)
        .CVar(CVAR_WINDOW("PlandomizerEditor"))
        .RaceDisable(false)
        .WindowName("Plandomizer Editor")
        .HideInSearch(true)
        .Options(WindowButtonOptions().Tooltip(Lang::UIString("ui.widgets.gOpenWindows.PlandomizerEditor.description",
                                                              "Enables the separate Randomizer Settings Window.")));

    // Item Tracker
    path.sidebarName = "Item Tracker";
    AddSidebarEntry("Randomizer", path.sidebarName, 1);

    AddWidget(path, "Item Tracker", WIDGET_SEPARATOR_TEXT).Translation("ui.panels.sohmenurandomizer.item_tracker");
    AddWidget(path, "Toggle Item Tracker", WIDGET_WINDOW_BUTTON)
        .CVar(CVAR_WINDOW("ItemTracker"))
        .RaceDisable(false)
        .WindowName("Item Tracker")
        .HideInSearch(true)
        .Options(
            WindowButtonOptions()
                .Tooltip(Lang::UIString("ui.widgets.gOpenWindows.ItemTracker.description", "Toggles the Item Tracker."))
                .EmbedWindow(false));

    AddWidget(path, "Item Tracker Settings", WIDGET_SEPARATOR_TEXT)
        .Translation("ui.panels.sohmenurandomizer.item_tracker_settings");
    AddWidget(path, "Popout Item Tracker Settings", WIDGET_WINDOW_BUTTON)
        .CVar(CVAR_WINDOW("ItemTrackerSettings"))
        .RaceDisable(false)
        .WindowName("Item Tracker Settings")
        .HideInSearch(true)
        .Options(WindowButtonOptions().Tooltip(Lang::UIString("ui.widgets.gOpenWindows.ItemTrackerSettings.description",
                                                              "Enables the separate Item Tracker Settings Window.")));

    // Entrance Tracker
    path.sidebarName = "Entrance Tracker";
    AddSidebarEntry("Randomizer", path.sidebarName, 1);

    AddWidget(path, "Entrance Tracker", WIDGET_SEPARATOR_TEXT)
        .Translation("ui.panels.sohmenurandomizer.entrance_tracker");
    AddWidget(path, "Toggle Entrance Tracker", WIDGET_WINDOW_BUTTON)
        .CVar(CVAR_WINDOW("EntranceTracker"))
        .RaceDisable(false)
        .WindowName("Entrance Tracker")
        .HideInSearch(true)
        .Options(WindowButtonOptions()
                     .Tooltip(Lang::UIString("ui.widgets.gOpenWindows.EntranceTracker.description",
                                             "Toggles the Entrance Tracker."))
                     .EmbedWindow(false));

    AddWidget(path, "Entrance Tracker Settings", WIDGET_SEPARATOR_TEXT)
        .Translation("ui.panels.sohmenurandomizer.entrance_tracker_settings");
    AddWidget(path, "Popout Entrance Tracker Settings", WIDGET_WINDOW_BUTTON)
        .CVar(CVAR_WINDOW("EntranceTrackerSettings"))
        .RaceDisable(false)
        .WindowName("Entrance Tracker Settings")
        .HideInSearch(true)
        .Options(
            WindowButtonOptions().Tooltip(Lang::UIString("ui.widgets.gOpenWindows.EntranceTrackerSettings.description",
                                                         "Enables the separate Entrance Tracker Settings Window.")));

    // Check Tracker
    path.sidebarName = "Check Tracker";
    AddSidebarEntry("Randomizer", path.sidebarName, 1);

    AddWidget(path, "Check Tracker", WIDGET_SEPARATOR_TEXT).Translation("ui.panels.sohmenurandomizer.check_tracker");
    AddWidget(path, "Toggle Check Tracker", WIDGET_WINDOW_BUTTON)
        .CVar(CVAR_WINDOW("CheckTracker"))
        .RaceDisable(false)
        .WindowName("Check Tracker")
        .HideInSearch(true)
        .Options(WindowButtonOptions()
                     .Tooltip(Lang::UIString("ui.widgets.gOpenWindows.CheckTracker.description",
                                             "Toggles the Check Tracker."))
                     .EmbedWindow(false));

    AddWidget(path, "Check Tracker Settings", WIDGET_SEPARATOR_TEXT)
        .Translation("ui.panels.sohmenurandomizer.check_tracker_settings");
    AddWidget(path, "Popout Check Tracker Settings", WIDGET_WINDOW_BUTTON)
        .CVar(CVAR_WINDOW("CheckTrackerSettings"))
        .RaceDisable(false)
        .WindowName("Check Tracker Settings")
        .HideInSearch(true)
        .Options(
            WindowButtonOptions().Tooltip(Lang::UIString("ui.widgets.gOpenWindows.CheckTrackerSettings.description",
                                                         "Enables the separate Check Tracker Settings Window.")));

    // Hint Tracker
    path.sidebarName = "Hint Tracker";
    AddSidebarEntry("Randomizer", path.sidebarName, 1);

    AddWidget(path, "Hint Tracker", WIDGET_SEPARATOR_TEXT).Translation("ui.panels.sohmenurandomizer.hint_tracker");
    AddWidget(path, "Toggle Hint Tracker", WIDGET_WINDOW_BUTTON)
        .CVar(CVAR_WINDOW("HintTracker"))
        .RaceDisable(false)
        .WindowName("Hint Tracker")
        .HideInSearch(true)
        .Options(
            WindowButtonOptions()
                .Tooltip(Lang::UIString("ui.widgets.gOpenWindows.HintTracker.description", "Toggles the Hint Tracker."))
                .EmbedWindow(false));

    AddWidget(path, "Hint Tracker Settings", WIDGET_SEPARATOR_TEXT)
        .Translation("ui.panels.sohmenurandomizer.hint_tracker_settings");
    AddWidget(path, "Popout Hint Tracker Settings", WIDGET_WINDOW_BUTTON)
        .CVar(CVAR_WINDOW("HintTrackerSettings"))
        .RaceDisable(false)
        .WindowName("Hint Tracker Settings")
        .HideInSearch(true)
        .Options(WindowButtonOptions().Tooltip(Lang::UIString("ui.widgets.gOpenWindows.HintTrackerSettings.description",
                                                              "Enables the separate Hint Tracker Settings Window.")));
}

} // namespace SohGui
