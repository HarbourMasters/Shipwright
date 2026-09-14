#include "randomizer_entrance_tracker_graph.h"

#include <libultraship/bridge/consolevariablebridge.h>
#include "soh/SohGui/SohGui.hpp"
#include "entrance.h"
#include "location_access.h"
#include "randomizer_entrance_tracker.h"
#include "randomizerEnumStrings.h"
#include "randomizerTypes.h"
#include "randomizer_tracker_windows.h"

const float initialSize = 2500.0f;

void EntranceTrackerGraphWindow::Draw() {
    if (!IsVisible()) {
        return;
    }
    DrawElement();
    // Sync up the IsVisible flag if it was changed by ImGui
    SyncVisibilityConsoleVariable();
}

const GraphOptions defaultOptions = {
    {
        0.05f,
        5.0f,
        true,
        true,
    },
    {
        1.0f,
        10.0f,
    },
    {
        80.0f,
        10.0f,
        40.0f,
    },
    {
        20.0f,
        {
            { 3.0f, 2.0f },
            3.0f,
            IM_COL32(48, 48, 64, 220),
            true,
            0.75f,
            1.0f,
        },
    },
    {
        2.0f,
        {
            0.0f,
            { 3.0f, 2.0f },
            3.0f,
            IM_COL32(48, 48, 64, 220),
            true,
            2.0f,
            3.0f,
        },
    },
};

void EntranceTrackerGraphWindow::DrawElement() {
    if (!this->graph.has_value()) {
        LUSLOG_ERROR("[EntranceTrackerGraphWindow::DrawElement] this->graph is std::nullopt");
        return;
    }

    Color_RGBA8 bgColor = { 0, 0, 0,
                            static_cast<uint8_t>(
                                CVarGetFloat(CVAR_TRACKER_ENTRANCE("Graph.backgroundTransparency"), 1.0f) * 255.0f) };
    if (Trackers::BeginFloatWindows("Entrance Tracker Graph", &mIsVisible, bgColor, TRACKER_WINDOW_WINDOW, true,
                                    ImGuiWindowFlags_NoScrollbar)) {
        ImVec2 canvasPos = ImGui::GetCursorScreenPos();
        ImVec2 canvasSize = ImGui::GetContentRegionAvail();

        if (!this->sufficientlyStabilized) {
            float displacement = 0;
            displacement += this->graph.value().StabilizeStep(initialSize * 2.0f, initialSize * 2.0f, 10.0f);
            displacement += this->graph.value().StabilizeStep(initialSize * 2.0f, initialSize * 2.0f, 10.0f);

            if (displacement < 29000) {
                this->sufficientlyStabilized = true;
            }
        }

        ImGui::SetNextItemAllowOverlap();

        if (canvasSize.x != 0.0f && canvasSize.y != 0.0f) {
            this->graph.value().Draw(canvasSize, canvasPos);
        }

        ImGui::SetCursorScreenPos(canvasPos);

        ImGui::SetNextItemAllowOverlap();

        if (menuOpen) {
            ImVec2 panelSize = { 325, 400 };

            ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 12.0f);
            ImGui::PushStyleColor(ImGuiCol_ChildBg, IM_COL32(40, 40, 40, 255));

            if (ImGui::BeginChild("ControlsPanel", panelSize, ImGuiChildFlags_Borders,
                                  ImGuiWindowFlags_AlwaysVerticalScrollbar)) {
                if (UIWidgets::Button(ICON_FA_COG,
                                      UIWidgets::ButtonOptions().Color(THEME_COLOR).Size({ 40.0f, 40.0f }))) {
                    menuOpen = false;
                }

                if (UIWidgets::Button("Reset View", UIWidgets::ButtonOptions().Color(THEME_COLOR))) {
                    this->graph.value().ResetView();
                }

                if (this->sufficientlyStabilized) {
                    if (UIWidgets::Button("Continue Stabilization",
                                          UIWidgets::ButtonOptions().Color(UIWidgets::Colors::Green))) {
                        this->sufficientlyStabilized = false;
                    }
                } else {
                    if (UIWidgets::Button("Stop Stabilization",
                                          UIWidgets::ButtonOptions().Color(UIWidgets::Colors::Red))) {
                        this->sufficientlyStabilized = true;
                    }
                }

                if (UIWidgets::Button(
                        "Stabilize Step",
                        UIWidgets::ButtonOptions().Color(THEME_COLOR).Disabled(!sufficientlyStabilized))) {
                    this->graph.value().StabilizeStep(initialSize * 2.0f, initialSize * 2.0f, 10.0f);
                    this->graph.value().StabilizeStep(initialSize * 2.0f, initialSize * 2.0f, 10.0f);
                }

#define CONFIG_INPUT_BOOL(option_path, text)                                                                      \
    if (CVarCheckbox(text, CVAR_TRACKER_ENTRANCE("Graph." #option_path),                                          \
                     UIWidgets::CheckboxOptions().DefaultValue(defaultOptions.option_path).Color(THEME_COLOR))) { \
        this->UpdateGraphOptions();                                                                               \
    }

#define CONFIG_INPUT_INT(option_path, text, min, max)                     \
    if (CVarSliderInt(text, CVAR_TRACKER_ENTRANCE("Graph." #option_path), \
                      UIWidgets::IntegerSliderOptions()                   \
                          .Min(min)                                       \
                          .Max(max)                                       \
                          .DefaultValue(defaultOptions.option_path)       \
                          .Format("%.1f")                                 \
                          .Size({ 300.0f, 0.0f })                         \
                          .Color(THEME_COLOR))) {                         \
        this->UpdateGraphOptions();                                       \
    }

#define CONFIG_INPUT_FLOAT(option_path, text, min, max, step, format)       \
    if (CVarSliderFloat(text, CVAR_TRACKER_ENTRANCE("Graph." #option_path), \
                        UIWidgets::FloatSliderOptions()                     \
                            .Min(min)                                       \
                            .Max(max)                                       \
                            .DefaultValue(defaultOptions.option_path)       \
                            .Format(format)                                 \
                            .Size({ 300.0f, 0.0f })                         \
                            .Step(step)                                     \
                            .Color(THEME_COLOR))) {                         \
        this->UpdateGraphOptions();                                         \
    }

                UIWidgets::Separator();

                CVarSliderFloat("Background Transparency", CVAR_TRACKER_ENTRANCE("Graph.backgroundTransparency"),
                                UIWidgets::FloatSliderOptions()
                                    .IsPercentage()
                                    .DefaultValue(1.0f)
                                    .Size({ 300.0f, 0.0f })
                                    .Step(0.01f)
                                    .Color(THEME_COLOR));

                UIWidgets::Separator();

                CONFIG_INPUT_FLOAT(zoom.min, "Minimum Zoom", 0.01f, 10.0f, 0.01f, "%.2f");
                CONFIG_INPUT_FLOAT(zoom.max, "Maximum Zoom", 0.01f, 10.0f, 0.1f, "%.1f");

                UIWidgets::Separator();

                CONFIG_INPUT_FLOAT(forceMultipliers.repulsion, "Repulsion Force", 1.0f, 50.0f, 0.1f, "%.1f");
                CONFIG_INPUT_FLOAT(forceMultipliers.attraction, "Attraction Force", 1.0f, 50.0f, 0.1f, "%.1f");

                UIWidgets::Separator();

                CONFIG_INPUT_FLOAT(temperature.starting, "Initial Starting Temperature", 10.0f, 1000.0f, 0.1f, "%.1f");
                CONFIG_INPUT_FLOAT(temperature.decreasePerIteration, "Initial Temperature Decrease Per Iteration",
                                   0.01f, 10.0f, 0.1f, "%.1f");
                CONFIG_INPUT_FLOAT(temperature.ending, "Initial Ending Temperature", 0.0f, 1000.0f, 0.1f, "%.1f");

                UIWidgets::Separator();

                CONFIG_INPUT_BOOL(zoom.nodesScaleWithZoom, "Nodes Scale With Zoom");
                CONFIG_INPUT_FLOAT(nodes.baseSize, "Node Base Size", 1.0f, 10.0f, 0.1f, "%.1f");
                // CONFIG_INPUT_IMVEC2(nodes.label.padding, "Node Label Padding", 0.0f, 20.0f);
                CONFIG_INPUT_FLOAT(nodes.label.rounding, "Node Label Rounding", 1.0f, 10.0f, 0.1f, "%.1f");
                // CONFIG_INPUT_COLOR(nodes.label.backgroundColor, "Node Label Rounding");
                CONFIG_INPUT_BOOL(nodes.label.fadeout, "Node Label Fades out");
                CONFIG_INPUT_FLOAT(nodes.label.fadeoutCutoffLower, "Node Label Fadeout Cutoff Lower", 0.1f, 5.0f, 0.1f,
                                   "%.1f");
                CONFIG_INPUT_FLOAT(nodes.label.fadeoutCutoffUpper, "Node Label Fadeout Cutoff Upper", 0.1f, 5.0f, 0.1f,
                                   "%.1f");

                UIWidgets::Separator();

                CONFIG_INPUT_BOOL(zoom.edgesScaleWithZoom, "Edges Scale With Zoom");
                CONFIG_INPUT_FLOAT(edges.thickness, "Edge Thickness", 1.0f, 10.0f, 0.1f, "%.1f");
                CONFIG_INPUT_FLOAT(edges.label.separation, "Edge Label Separation", 0.0f, 20.0f, 0.1f, "%.1f");
                // CONFIG_INPUT_IMVEC2(edges.label.padding, "Edge Label Padding", 0.0f, 20.0f);
                CONFIG_INPUT_FLOAT(edges.label.rounding, "Edge Label Rounding", 1.0f, 10.0f, 0.1f, "%.1f");
                // CONFIG_INPUT_COLOR(edges.label.backgroundColor, "Edge Label Rounding");
                CONFIG_INPUT_BOOL(edges.label.fadeout, "Edge Label Fades out");
                CONFIG_INPUT_FLOAT(edges.label.fadeoutCutoffLower, "Edge Label Fadeout Cutoff Lower", 0.1f, 5.0f, 0.1f,
                                   "%.1f");
                CONFIG_INPUT_FLOAT(edges.label.fadeoutCutoffUpper, "Edge Label Fadeout Cutoff Upper", 0.1f, 5.0f, 0.1f,
                                   "%.1f");

#undef CONFIG_INPUT_BOOL
#undef CONFIG_INPUT_INT
#undef CONFIG_INPUT_FLOAT
            }

            ImGui::PopStyleColor();
            ImGui::PopStyleVar();

            ImGui::EndChild();
        } else {
            if (UIWidgets::Button(ICON_FA_COG, UIWidgets::ButtonOptions().Color(THEME_COLOR).Size({ 40.0f, 40.0f }))) {
                menuOpen = true;
            }
        }
    }
    Trackers::EndFloatWindows();
}

ImU32 GetColorForArea(RandomizerArea area) {
    switch (area) {
        case RA_NONE:
        case RA_LINKS_POCKET:
            return IM_COL32(0xD8, 0xD2, 0xE8, 0xFF);
        case RA_KOKIRI_FOREST:
            return IM_COL32(0xB8, 0xE3, 0xC2, 0xFF);
        case RA_THE_LOST_WOODS:
            return IM_COL32(0xAF, 0xCF, 0xA8, 0xFF);
        case RA_SACRED_FOREST_MEADOW:
            return IM_COL32(0xCB, 0xE8, 0xB5, 0xFF);
        case RA_HYRULE_FIELD:
            return IM_COL32(0xDD, 0xE8, 0xA8, 0xFF);
        case RA_LAKE_HYLIA:
            return IM_COL32(0xA9, 0xDC, 0xE3, 0xFF);
        case RA_GERUDO_VALLEY:
            return IM_COL32(0xF2, 0xC6, 0xA0, 0xFF);
        case RA_GERUDO_FORTRESS:
            return IM_COL32(0xE8, 0xB3, 0xA7, 0xFF);
        case RA_HAUNTED_WASTELAND:
            return IM_COL32(0xC9, 0xB2, 0xC8, 0xFF);
        case RA_DESERT_COLOSSUS:
            return IM_COL32(0xEB, 0xCB, 0x8B, 0xFF);
        case RA_THE_MARKET:
            return IM_COL32(0xF4, 0xB7, 0xC5, 0xFF);
        case RA_TEMPLE_OF_TIME:
            return IM_COL32(0xED, 0xE2, 0xC6, 0xFF);
        case RA_HYRULE_CASTLE:
            return IM_COL32(0xB9, 0xC1, 0xE8, 0xFF);
        case RA_OUTSIDE_GANONS_CASTLE:
            return IM_COL32(0xA8, 0x9B, 0xBE, 0xFF);
        case RA_CASTLE_GROUNDS:
            return IM_COL32(0xB8, 0xCB, 0xE8, 0xFF);
        case RA_KAKARIKO_VILLAGE:
            return IM_COL32(0xE7, 0xB2, 0x8D, 0xFF);
        case RA_THE_GRAVEYARD:
            return IM_COL32(0xAA, 0xA0, 0xBA, 0xFF);
        case RA_DEATH_MOUNTAIN_TRAIL:
            return IM_COL32(0xE8, 0xA9, 0x8F, 0xFF);
        case RA_GORON_CITY:
            return IM_COL32(0xF0, 0x8F, 0x78, 0xFF);
        case RA_DEATH_MOUNTAIN_CRATER:
            return IM_COL32(0xF4, 0xB0, 0x8A, 0xFF);
        case RA_ZORAS_RIVER:
            return IM_COL32(0x9F, 0xD8, 0xD2, 0xFF);
        case RA_ZORAS_DOMAIN:
            return IM_COL32(0xA8, 0xD7, 0xE8, 0xFF);
        case RA_ZORAS_FOUNTAIN:
            return IM_COL32(0xC1, 0xE8, 0xE5, 0xFF);
        case RA_LON_LON_RANCH:
            return IM_COL32(0xF4, 0xE1, 0xA1, 0xFF);
        case RA_DEKU_TREE:
            return IM_COL32(0xA9, 0xD6, 0xA0, 0xFF);
        case RA_DODONGOS_CAVERN:
            return IM_COL32(0xC9, 0x9F, 0x88, 0xFF);
        case RA_JABU_JABUS_BELLY:
            return IM_COL32(0xEF, 0xAF, 0xC0, 0xFF);
        case RA_FOREST_TEMPLE:
            return IM_COL32(0xA9, 0xC8, 0xB5, 0xFF);
        case RA_FIRE_TEMPLE:
            return IM_COL32(0xE9, 0x96, 0x7A, 0xFF);
        case RA_WATER_TEMPLE:
            return IM_COL32(0x91, 0xC8, 0xD8, 0xFF);
        case RA_SPIRIT_TEMPLE:
            return IM_COL32(0xE8, 0xC2, 0x9A, 0xFF);
        case RA_SHADOW_TEMPLE:
            return IM_COL32(0xAA, 0xA6, 0xB8, 0xFF);
        case RA_BOTTOM_OF_THE_WELL:
            return IM_COL32(0xB5, 0xB0, 0xA8, 0xFF);
        case RA_ICE_CAVERN:
            return IM_COL32(0xB8, 0xDF, 0xF0, 0xFF);
        case RA_GERUDO_TRAINING_GROUND:
            return IM_COL32(0xD7, 0xA0, 0x8D, 0xFF);
        case RA_GANONS_CASTLE:
            return IM_COL32(0x92, 0x7C, 0x9F, 0xFF);
        default:
            LUSLOG_ERROR("[GetColorForArea] Invalid area (%d)", static_cast<int>(area));
            assert(false);
            return IM_COL32(0xD8, 0xD2, 0xE8, 0xFF);
    }
}

ImU32 GetColorForAreas(std::set<RandomizerArea> areas) {
    switch (areas.size()) {
        case 0:
            return IM_COL32(0xD8, 0xD2, 0xE8, 0xFF);
        case 1:
            return GetColorForArea(*areas.begin());
        default:
            // for now we just pick a random one (sets aren't ordered)
            return GetColorForArea(*areas.begin());
    }
}

uint64_t randoState = 0;

void EntranceTrackerGraphWindow::InitElement() {
    RegionTable_Init();

    this->InitGraph(true);
}

const ImU32 labelColor = IM_COL32_WHITE;
const ImU32 edgeColor = IM_COL32_WHITE;

void EntranceTrackerGraphWindow::InitGraph(bool initialStabilization) {
    std::vector<Node> nodes = {};

    std::vector<Edge> edges = {};

    for (const auto& region : areaTable) {
        if (region.randomizerRegionKey == RR_NONE) {
            continue;
        }

        std::optional<std::string_view> regionString = EnumToString(region.randomizerRegionKey);

        if (!regionString.has_value()) {
            regionString = "???";
        }

        // random starting positions to allow the forces to move them
        nodes.push_back(Node::New(
            { static_cast<float>(ShipUtils::next32(&randoState)) / INT32_MAX * initialSize - initialSize / 2,
              static_cast<float>(ShipUtils::next32(&randoState)) / INT32_MAX * initialSize - initialSize / 2 },
            std::string(regionString.value()), GetColorForAreas(region.areas), labelColor));
    }

    for (const auto& region : areaTable) {
        for (const auto& exit : region.exits) {
            // -1 due to skipping RR_NONE
            edges.push_back(Edge::New(exit.GetParentRegionKey() - 1, exit.GetConnectedRegionKey() - 1,
                                      exit.GetConditionStr(), edgeColor, labelColor));
        }
    }

    this->graph = Graph::New(nodes, edges, defaultOptions);

    this->UpdateGraphOptions();

    if (initialStabilization) {
        this->graph.value().Stabilize(initialSize * 2.0f, initialSize * 2.0f);
    }

    this->sufficientlyStabilized = false;
}

void EntranceTrackerGraphWindow::UpdateGraphOptions() {
    if (!this->graph.has_value()) {
        return;
    }

    GraphOptions& currentGraphOptions = this->graph.value().GetOptions();

#define UPDATE_OPTION_PATH_SIMPLE(option_path, type) \
    currentGraphOptions.option_path =                \
        CVarGet##type(CVAR_TRACKER_ENTRANCE("Graph." #option_path), defaultOptions.option_path)
#define UPDATE_OPTION_PATH_INT(option_path) UPDATE_OPTION_PATH_SIMPLE(option_path, Integer)
#define UPDATE_OPTION_PATH_FLOAT(option_path) UPDATE_OPTION_PATH_SIMPLE(option_path, Float)
#define UPDATE_OPTION_PATH_IMVEC2(option_path)                                                         \
    currentGraphOptions.option_path = {                                                                \
        CVarGetFloat(CVAR_TRACKER_ENTRANCE("Graph." #option_path ".x"), defaultOptions.option_path.x), \
        CVarGetFloat(CVAR_TRACKER_ENTRANCE("Graph." #option_path ".y"), defaultOptions.option_path.y), \
    }
#define UPDATE_OPTION_PATH_IMU32(option_path) \
    currentGraphOptions.option_path =         \
        static_cast<ImU32>(CVarGetInteger(CVAR_TRACKER_ENTRANCE("Graph." #option_path), defaultOptions.option_path))

    UPDATE_OPTION_PATH_FLOAT(zoom.min);
    UPDATE_OPTION_PATH_FLOAT(zoom.max);
    UPDATE_OPTION_PATH_INT(zoom.nodesScaleWithZoom);
    UPDATE_OPTION_PATH_INT(zoom.edgesScaleWithZoom);

    UPDATE_OPTION_PATH_FLOAT(forceMultipliers.repulsion);
    UPDATE_OPTION_PATH_FLOAT(forceMultipliers.attraction);

    UPDATE_OPTION_PATH_FLOAT(temperature.starting);
    UPDATE_OPTION_PATH_FLOAT(temperature.decreasePerIteration);
    UPDATE_OPTION_PATH_FLOAT(temperature.ending);

    UPDATE_OPTION_PATH_FLOAT(nodes.baseSize);
    UPDATE_OPTION_PATH_IMVEC2(nodes.label.padding);
    UPDATE_OPTION_PATH_FLOAT(nodes.label.rounding);
    UPDATE_OPTION_PATH_IMU32(nodes.label.backgroundColor);
    UPDATE_OPTION_PATH_INT(nodes.label.fadeout);
    UPDATE_OPTION_PATH_FLOAT(nodes.label.fadeoutCutoffLower);
    UPDATE_OPTION_PATH_FLOAT(nodes.label.fadeoutCutoffUpper);

    UPDATE_OPTION_PATH_FLOAT(edges.thickness);
    UPDATE_OPTION_PATH_FLOAT(edges.label.separation);
    UPDATE_OPTION_PATH_IMVEC2(edges.label.padding);
    UPDATE_OPTION_PATH_FLOAT(edges.label.rounding);
    UPDATE_OPTION_PATH_IMU32(edges.label.backgroundColor);
    UPDATE_OPTION_PATH_INT(edges.label.fadeout);
    UPDATE_OPTION_PATH_FLOAT(edges.label.fadeoutCutoffLower);
    UPDATE_OPTION_PATH_FLOAT(edges.label.fadeoutCutoffUpper);

#undef UPDATE_OPTION_PATH_SIMPLE
#undef UPDATE_OPTION_PATH_INT
#undef UPDATE_OPTION_PATH_FLOAT
#undef UPDATE_OPTION_PATH_IMVEC2
#undef UPDATE_OPTION_PATH_IMU32

    this->sufficientlyStabilized = false;
}

void EntranceTrackerGraphWindow::UpdateEdges() {
    if (!this->graph.has_value()) {
        return;
    }

    std::vector<Edge> edges = {};

    for (const auto& region : areaTable) {
        for (const auto& exit : region.exits) {
            // -1 due to skipping RR_NONE
            edges.push_back(Edge::New(exit.GetParentRegionKey() - 1, exit.GetConnectedRegionKey() - 1,
                                      exit.GetConditionStr(), edgeColor, labelColor));
        }
    }

    this->graph.value().ReplaceEdges(edges);

    this->UpdateGraphOptions();
}

void ReInitGraph() {
    std::dynamic_pointer_cast<EntranceTrackerGraphWindow>(
        Ship::Context::GetRawInstance()->GetWindow()->GetGui()->GetGuiWindow("Entrance Tracker Graph"))
        ->UpdateEdges();
}

static RegisterShipInitFunc initFunc(ReInitGraph, { "IS_RANDO" });