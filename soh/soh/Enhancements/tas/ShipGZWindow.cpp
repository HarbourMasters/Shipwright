/*
 * ShipGZ window: the SoH counterpart of gz's macro menu (gz_macro.c), virtual controller menu, command
 * binding menu (gz_settings.c) and on-screen pause / movie / input display (gz.c main_hook), plus a frame
 * by frame input editor.
 */

#include "ShipGZWindow.h"
#include "ShipGZCore.hpp"

#include <algorithm>
#include <chrono>
#include <cmath>
#include <cstring>
#include <filesystem>
#include <string>
#include <vector>

#include <imgui.h>
#include <ship/Context.h>
#include <ship/window/Window.h>
#include <libultraship/bridge/consolevariablebridge.h>

#include "soh/OTRGlobals.h"

extern "C" {
#include "z64.h"
#include "variables.h"
extern PlayState* gPlayState;
}

#define SHIPGZ_CVAR(name) "gShipGZ." name

using namespace ShipGZ;

namespace {

struct ButtonDef {
    uint32_t bit;
    const char* label;
    ImU32 color;
};

// Order and colours follow gz's input display (gz.c: input_button_color)
const ButtonDef kButtons[] = {
    { BTN_A, "A", IM_COL32(0x50, 0x50, 0xFF, 0xFF) },      { BTN_B, "B", IM_COL32(0x00, 0xC8, 0x00, 0xFF) },
    { BTN_Z, "Z", IM_COL32(0xC8, 0xC8, 0xC8, 0xFF) },      { BTN_START, "S", IM_COL32(0xC8, 0x00, 0x00, 0xFF) },
    { BTN_L, "L", IM_COL32(0xC8, 0xC8, 0xC8, 0xFF) },      { BTN_R, "R", IM_COL32(0xC8, 0xC8, 0xC8, 0xFF) },
    { BTN_CUP, "Cu", IM_COL32(0xFF, 0xFF, 0x00, 0xFF) },   { BTN_CDOWN, "Cd", IM_COL32(0xFF, 0xFF, 0x00, 0xFF) },
    { BTN_CLEFT, "Cl", IM_COL32(0xFF, 0xFF, 0x00, 0xFF) }, { BTN_CRIGHT, "Cr", IM_COL32(0xFF, 0xFF, 0x00, 0xFF) },
    { BTN_DUP, "Du", IM_COL32(0xC8, 0xC8, 0xC8, 0xFF) },   { BTN_DDOWN, "Dd", IM_COL32(0xC8, 0xC8, 0xC8, 0xFF) },
    { BTN_DLEFT, "Dl", IM_COL32(0xC8, 0xC8, 0xC8, 0xFF) }, { BTN_DRIGHT, "Dr", IM_COL32(0xC8, 0xC8, 0xC8, 0xFF) },
    { 0x0040, "M1", IM_COL32(0xA0, 0x80, 0xFF, 0xFF) },     { 0x0080, "M2", IM_COL32(0xA0, 0x80, 0xFF, 0xFF) },
};
constexpr int kNumButtons = sizeof(kButtons) / sizeof(*kButtons);

const ImVec4 kColRec = ImVec4(0.90f, 0.20f, 0.20f, 1.0f);
const ImVec4 kColPlay = ImVec4(0.25f, 0.80f, 0.35f, 1.0f);
const ImVec4 kColPause = ImVec4(0.95f, 0.75f, 0.20f, 1.0f);
const ImVec4 kColDim = ImVec4(0.60f, 0.60f, 0.60f, 1.0f);

std::string ComboName(uint32_t combo) {
    if (combo == 0) {
        return "unbound";
    }
    std::string s;
    for (const auto& b : kButtons) {
        if (combo & b.bit) {
            if (!s.empty()) {
                s += " + ";
            }
            s += b.label;
        }
    }
    return s.empty() ? "?" : s;
}

std::string KeyName(int key) {
    if (key <= ImGuiKey_None || key >= ImGuiKey_NamedKey_END) {
        return "unbound";
    }
    return ImGui::GetKeyName((ImGuiKey)key);
}

const char* StateName(MovieState s) {
    switch (s) {
        case MovieState::Recording:
            return "recording";
        case MovieState::Playing:
            return "playing";
        default:
            return "idle";
    }
}

bool CVarCheckbox(const char* label, const char* cvar, bool def, const char* tooltip = nullptr) {
    bool v = CVarGetInteger(cvar, def ? 1 : 0) != 0;
    bool changed = ImGui::Checkbox(label, &v);
    if (changed) {
        CVarSetInteger(cvar, v ? 1 : 0);
        Ship::Context::GetRawInstance()->GetWindow()->GetGui()->SaveConsoleVariablesNextFrame();
    }
    if (tooltip != nullptr && ImGui::IsItemHovered()) {
        ImGui::SetTooltip("%s", tooltip);
    }
    return changed;
}

// Binding capture state for the commands tab
int sCaptureKeyCmd = -1;
int sCapturePadCmd = -1;
uint32_t sCapturePadAccum = 0;
bool sCapturePadSawInput = false;

// File section state
char sPathBuf[512] = "";
std::string sFileError;
std::string sFileStatus;

// Input editor state
bool sFollowFrame = true;
int32_t sLastFollowedFrame = -1;
int sPaintColumn = -1;
bool sPaintValue = false;
int32_t sContextFrame = -1;
int sInsertCount = 1;

int sAdvanceCount = 10;
int sSeekFrame = 0;

std::string DefaultMoviePath() {
    return (std::filesystem::path(GetTasDirectory()) / "movie.sgzm").string();
}

void SetPath(const std::string& p) {
    snprintf(sPathBuf, sizeof(sPathBuf), "%s", p.c_str());
}

void HelpMarker(const char* text) {
    ImGui::SameLine();
    ImGui::TextDisabled("(?)");
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("%s", text);
    }
}

} // namespace

// ---------------------------------------------------------------------------------------------------------------

void ShipGZWindow::InitElement() {
    SetPath(DefaultMoviePath());
}

void ShipGZWindow::UpdateElement() {
    ShipGZ_Update(sCaptureKeyCmd >= 0 || sCapturePadCmd >= 0);
    if (CVarGetInteger(SHIPGZ_CVAR("Osd"), 1)) {
        DrawOsd();
    }
}

// On-screen display: gz's pause / record / play icon with macro frame counter, the input display and the log.
void ShipGZWindow::DrawOsd() {
    Engine& e = Get();
    ImGuiViewport* vp = ImGui::GetMainViewport();
    ImDrawList* dl = ImGui::GetForegroundDrawList(vp);
    float scale = CVarGetFloat(SHIPGZ_CVAR("OsdScale"), 1.0f);
    ImFont* font = ImGui::GetFont();
    float fs = ImGui::GetFontSize() * scale;
    ImVec2 origin = ImVec2(vp->WorkPos.x + 12.0f, vp->WorkPos.y + 12.0f);
    ImVec2 p = origin;
    auto text = [&](ImU32 col, const char* s) {
        dl->AddText(font, fs, ImVec2(p.x + 1, p.y + 1), IM_COL32(0, 0, 0, 0xC0), s);
        dl->AddText(font, fs, p, col, s);
        p.x += font->CalcTextSizeA(fs, FLT_MAX, 0.0f, s).x;
    };
    auto newline = [&]() {
        p.x = origin.x;
        p.y += fs + 2.0f;
    };

    bool frozen = e.framesQueued != -1;
    char buf[128];
    bool any = false;
    if (e.state != MovieState::Idle) {
        ImU32 col = e.state == MovieState::Recording ? IM_COL32(0xE0, 0x30, 0x30, 0xFF) : IM_COL32(0x40, 0xD0, 0x50, 0xFF);
        snprintf(buf, sizeof(buf), "%s%s", e.state == MovieState::Recording ? "REC" : "PLAY", frozen ? " ||" : "");
        text(col, buf);
        snprintf(buf, sizeof(buf), "  %d / %d", e.frame, (int)e.movie.inputs.size());
        text(IM_COL32(0xC0, 0xC0, 0xC0, 0xFF), buf);
        if (e.desyncCount > 0) {
            snprintf(buf, sizeof(buf), "  desync x%d", e.desyncCount);
            text(IM_COL32(0xFF, 0x90, 0x30, 0xFF), buf);
        }
        any = true;
    } else if (frozen) {
        text(IM_COL32(0xC0, 0xC0, 0xC0, 0xFF), e.framesQueued == 0 ? "||" : "|>");
        any = true;
    }
    float speed = CVarGetFloat(SHIPGZ_CVAR("Speed"), 1.0f);
    if (std::fabs(speed - 1.0f) > 0.001f) {
        snprintf(buf, sizeof(buf), "%s%d%%", any ? "  " : "", (int)std::lround(speed * 100.0f));
        text(IM_COL32(0x80, 0xC0, 0xFF, 0xFF), buf);
        any = true;
    }
    if (any) {
        newline();
    }

    if (CVarGetInteger(SHIPGZ_CVAR("OsdInput"), 1)) {
        const MovieInput& in = e.lastGameInput;
        snprintf(buf, sizeof(buf), "%4d %4d ", in.stickX, in.stickY);
        text(IM_COL32(0xC0, 0xC0, 0xC0, 0xFF), buf);
        for (const auto& b : kButtons) {
            if (in.buttons & b.bit) {
                text(b.color, b.label);
                text(IM_COL32(0, 0, 0, 0), " ");
            }
        }
        newline();
    }

    if (CVarGetInteger(SHIPGZ_CVAR("OsdLog"), 1)) {
        auto& log = e.log;
        // entries fade out after a few seconds, like gz's log
        for (size_t i = 0; i < log.size() && i < 4; i++) {
            using namespace std::chrono;
            double age =
                duration<double>(steady_clock::now().time_since_epoch()).count() - log[i].time;
            if (age > 4.0) {
                continue;
            }
            int alpha = age < 3.0 ? 0xFF : (int)(0xFF * (4.0 - age));
            text(IM_COL32(0xC0, 0xC0, 0xC0, alpha), log[i].msg.c_str());
            newline();
        }
    }
}

// ---------------------------------------------------------------------------------------------------------------

void ShipGZWindow::DrawElement() {
    Engine& e = Get();

    // status line
    {
        ImVec4 col = e.state == MovieState::Recording ? kColRec : e.state == MovieState::Playing ? kColPlay : kColDim;
        ImGui::TextColored(col, "%s", StateName(e.state));
        ImGui::SameLine();
        ImGui::Text("| frame %d / %d | rerecords %d", e.frame, (int)e.movie.inputs.size(), e.movie.rerecords);
        ImGui::SameLine();
        if (e.framesQueued == -1) {
            ImGui::TextColored(kColDim, "| running");
        } else {
            ImGui::TextColored(kColPause, "| frozen%s", e.framesQueued > 0 ? " (advancing)" : "");
        }
        if (!InGameplay()) {
            ImGui::SameLine();
            ImGui::TextColored(kColDim, "| not in gameplay: states unavailable");
        }
    }

    if (ImGui::BeginTabBar("ShipGZTabs")) {
        if (ImGui::BeginTabItem("Macro")) {
            DrawMacroTab();
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("States")) {
            DrawStatesTab();
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Input Editor")) {
            DrawInputEditorTab();
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Virtual Controller")) {
            DrawVirtualControllerTab();
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Commands")) {
            DrawCommandsTab();
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Settings")) {
            DrawSettingsTab();
            ImGui::EndTabItem();
        }
        ImGui::EndTabBar();
    }
}

// gz_macro.c: pause, frame advance, record, play, rewind, trim, macro frame, quick record / play, export / import
void ShipGZWindow::DrawMacroTab() {
    Engine& e = Get();

    ImGui::SeparatorText("Frame control");
    if (ImGui::Button(e.framesQueued == -1 ? "Pause" : "Resume")) {
        TogglePause();
    }
    ImGui::SameLine();
    ImGui::Button("Frame advance");
    if (ImGui::IsItemActivated()) {
        FrameAdvance();
    } else if (ImGui::IsItemActive() && ImGui::GetIO().MouseDownDuration[0] > 0.35f && e.framesQueued == 0) {
        e.framesQueued = 1; // hold to advance continuously
    }
    ImGui::SameLine();
    ImGui::SetNextItemWidth(80);
    ImGui::InputInt("##advn", &sAdvanceCount);
    sAdvanceCount = std::clamp(sAdvanceCount, 1, 100000);
    ImGui::SameLine();
    if (ImGui::Button("Advance N")) {
        if (e.framesQueued == -1) {
            e.framesQueued = 0;
        }
        e.framesQueued += sAdvanceCount;
    }

    float speed = CVarGetFloat(SHIPGZ_CVAR("Speed"), 1.0f);
    ImGui::SetNextItemWidth(220);
    if (ImGui::SliderFloat("speed", &speed, 0.05f, 4.0f, "%.2fx", ImGuiSliderFlags_Logarithmic)) {
        CVarSetFloat(SHIPGZ_CVAR("Speed"), speed);
    }
    ImGui::SameLine();
    if (ImGui::Button("1x")) {
        CVarSetFloat(SHIPGZ_CVAR("Speed"), 1.0f);
    }
    HelpMarker("Below 1x, the game holds each frame (slow motion). Above 1x, frame interpolation is turned off and "
               "the frame limiter is raised (fast forward; V-Sync may cap it).");

    ImGui::SeparatorText("Macro");
    {
        bool rec = e.state == MovieState::Recording;
        ImGui::PushStyleColor(ImGuiCol_Button, rec ? ImVec4(0.6f, 0.1f, 0.1f, 1.0f) : ImGui::GetStyleColorVec4(ImGuiCol_Button));
        if (ImGui::Button(rec ? "Stop recording" : "Record")) {
            ExecuteCommand(CMD_RECORD);
        }
        ImGui::PopStyleColor();
        ImGui::SameLine();
        bool play = e.state == MovieState::Playing;
        ImGui::PushStyleColor(ImGuiCol_Button, play ? ImVec4(0.1f, 0.5f, 0.15f, 1.0f) : ImGui::GetStyleColorVec4(ImGuiCol_Button));
        if (ImGui::Button(play ? "Stop playback" : "Play")) {
            ExecuteCommand(CMD_PLAY);
        }
        ImGui::PopStyleColor();
        ImGui::SameLine();
        if (ImGui::Button("Rewind")) {
            ExecuteCommand(CMD_REWIND);
        }
        ImGui::SameLine();
        if (ImGui::Button("Trim")) {
            MovieTrim();
        }
        if (ImGui::IsItemHovered()) {
            ImGui::SetTooltip("Delete all recorded inputs after the current macro frame.");
        }
        ImGui::SameLine();
        if (ImGui::Button("New macro")) {
            ImGui::OpenPopup("Clear macro?");
        }
        if (ImGui::BeginPopupModal("Clear macro?", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
            ImGui::Text("Discard all %d recorded frames?", (int)e.movie.inputs.size());
            if (ImGui::Button("Discard")) {
                MovieClear();
                ImGui::CloseCurrentPopup();
            }
            ImGui::SameLine();
            if (ImGui::Button("Cancel")) {
                ImGui::CloseCurrentPopup();
            }
            ImGui::EndPopup();
        }
    }

    // macro frame (gz: movie_pos_proc) - editing it seeks the macro
    int frame = e.frame;
    ImGui::SetNextItemWidth(140);
    // ImGui's InputInt asserts on ImGuiInputTextFlags_EnterReturnsTrue, so seek when editing finishes instead
    ImGui::InputInt("macro frame", &frame, 1, 10);
    if (ImGui::IsItemDeactivatedAfterEdit()) {
        MovieSeek(frame);
    }
    ImGui::SameLine();
    ImGui::Text("/ %d", (int)e.movie.inputs.size());
    ImGui::SameLine();
    ImGui::TextColored(kColDim, "rerecords: %d   rng seeds: %d", e.movie.rerecords, (int)e.movie.seeds.size());

    bool stopAt = e.stopAtFrame >= 0;
    if (ImGui::Checkbox("Pause at macro frame", &stopAt)) {
        e.stopAtFrame = stopAt ? std::max(sSeekFrame, 0) : -1;
    }
    ImGui::SameLine();
    ImGui::SetNextItemWidth(140);
    if (ImGui::InputInt("##stopat", &sSeekFrame)) {
        sSeekFrame = std::max(sSeekFrame, 0);
        if (stopAt) {
            e.stopAtFrame = sSeekFrame;
        }
    }
    HelpMarker("Freezes the game when the macro frame counter reaches this value while recording or playing. "
               "Handy for 'play from state 0 until frame N, then take over'.");

    ImGui::SeparatorText("Quick movie");
    if (ImGui::Button("Quick record movie")) {
        QuickRecord();
    }
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("Rewind, start recording, save the start state to slot 0 and switch to slot 1 for "
                          "rerecording.");
    }
    ImGui::SameLine();
    if (ImGui::Button("Quick play movie")) {
        QuickPlay();
    }
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("Rewind, start playback and load the start state in slot 0.");
    }
    ImGui::SameLine();
    if (ImGui::Button("Reset game")) {
        RequestReset();
    }
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("Reset to the title screen after the next frame. While recording, the reset is stored in the "
                          "macro and replayed.");
    }

    DrawFilesSection();

    if (!e.log.empty()) {
        ImGui::SeparatorText("Log");
        for (const auto& l : e.log) {
            ImGui::TextColored(kColDim, "%s", l.msg.c_str());
        }
    }
}

void ShipGZWindow::DrawFilesSection() {
    Engine& e = Get();
    ImGui::SeparatorText("Files");

    char author[128];
    snprintf(author, sizeof(author), "%s", e.movie.author.c_str());
    ImGui::SetNextItemWidth(200);
    if (ImGui::InputText("author", author, sizeof(author))) {
        e.movie.author = author;
    }
    ImGui::SameLine();
    char desc[256];
    snprintf(desc, sizeof(desc), "%s", e.movie.description.c_str());
    ImGui::SetNextItemWidth(-90);
    if (ImGui::InputText("description", desc, sizeof(desc))) {
        e.movie.description = desc;
    }
    if (!e.movie.startNote.empty()) {
        ImGui::TextColored(kColDim, "note: %s", e.movie.startNote.c_str());
    }

    ImGui::SetNextItemWidth(-1);
    ImGui::InputText("##path", sPathBuf, sizeof(sPathBuf));
    if (ImGui::Button("Export macro")) {
        sFileError = ExportMovie(sPathBuf);
        sFileStatus = sFileError.empty() ? "saved" : "";
    }
    ImGui::SameLine();
    if (ImGui::Button("Import macro")) {
        sFileError = ImportMovie(sPathBuf);
        sFileStatus = sFileError.empty() ? "loaded" : "";
    }
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("Loads a ShipGZ macro (.sgzm). gz macros (.gzm) are detected and converted.");
    }
    ImGui::SameLine();
    if (ImGui::Button("Export CSV")) {
        std::string p = std::filesystem::path(sPathBuf).replace_extension(".csv").string();
        sFileError = ExportInputCsv(p);
        sFileStatus = sFileError.empty() ? ("wrote " + p) : "";
    }
    ImGui::SameLine();
    if (ImGui::Button("Open folder list")) {
        ImGui::OpenPopup("ShipGZFiles");
    }
    if (!sFileError.empty()) {
        ImGui::TextColored(kColRec, "%s", sFileError.c_str());
    } else if (!sFileStatus.empty()) {
        ImGui::TextColored(kColPlay, "%s", sFileStatus.c_str());
    }

    if (ImGui::BeginPopup("ShipGZFiles")) {
        std::string dir = GetTasDirectory();
        ImGui::TextColored(kColDim, "%s", dir.c_str());
        std::error_code ec;
        std::filesystem::create_directories(dir, ec);
        bool anyFile = false;
        for (const auto& entry : std::filesystem::directory_iterator(dir, ec)) {
            if (!entry.is_regular_file()) {
                continue;
            }
            std::string ext = entry.path().extension().string();
            if (ext != ".sgzm" && ext != ".gzm") {
                continue;
            }
            anyFile = true;
            if (ImGui::Selectable(entry.path().filename().string().c_str())) {
                SetPath(entry.path().string());
            }
        }
        if (!anyFile) {
            ImGui::TextColored(kColDim, "no .sgzm / .gzm files yet");
        }
        ImGui::EndPopup();
    }
}

// Savestate slots shared with SoH's F5 / F6 / F7 savestates
void ShipGZWindow::DrawStatesTab() {
    Engine& e = Get();
    uint32_t cur = GetStateSlot();

    if (ImGui::ArrowButton("##prevslot", ImGuiDir_Left)) {
        ExecuteCommand(CMD_PREV_STATE);
    }
    ImGui::SameLine();
    ImGui::Text("slot %u", cur);
    ImGui::SameLine();
    if (ImGui::ArrowButton("##nextslot", ImGuiDir_Right)) {
        ExecuteCommand(CMD_NEXT_STATE);
    }
    ImGui::SameLine();
    ImGui::BeginDisabled(!InGameplay());
    if (ImGui::Button("Save state")) {
        SaveState(cur);
    }
    ImGui::SameLine();
    if (ImGui::Button("Load state")) {
        LoadState(cur);
    }
    ImGui::EndDisabled();

    ImGui::TextColored(kColDim, "States carry the macro frame they were saved on. Loading one while recording seeks "
                                "the macro there, so you can rerecord from that point.");

    if (ImGui::BeginTable("ShipGZStates", 6, ImGuiTableFlags_RowBg | ImGuiTableFlags_BordersInnerV)) {
        ImGui::TableSetupColumn("slot", ImGuiTableColumnFlags_WidthFixed, 40);
        ImGui::TableSetupColumn("scene");
        ImGui::TableSetupColumn("room", ImGuiTableColumnFlags_WidthFixed, 40);
        ImGui::TableSetupColumn("macro frame", ImGuiTableColumnFlags_WidthFixed, 90);
        ImGui::TableSetupColumn("rng", ImGuiTableColumnFlags_WidthFixed, 80);
        ImGui::TableSetupColumn("", ImGuiTableColumnFlags_WidthFixed, 110);
        ImGui::TableHeadersRow();
        for (int i = 0; i < kNumStateSlots; i++) {
            const StateMeta& m = e.stateMeta[i];
            ImGui::PushID(i);
            ImGui::TableNextRow();
            ImGui::TableNextColumn();
            if (ImGui::Selectable(std::to_string(i).c_str(), cur == (uint32_t)i, ImGuiSelectableFlags_SpanAllColumns |
                                                                                 ImGuiSelectableFlags_AllowOverlap)) {
                SetStateSlot(i);
            }
            ImGui::TableNextColumn();
            if (!m.valid) {
                ImGui::TextColored(kColDim, "no state");
            } else {
                ImGui::TextUnformatted(SceneName(m.sceneNum));
            }
            ImGui::TableNextColumn();
            if (m.valid) {
                ImGui::Text("%d", m.roomNum);
            }
            ImGui::TableNextColumn();
            if (m.valid) {
                if (m.movieFrame >= 0) {
                    ImGui::TextColored(kColRec, "%d", m.movieFrame);
                } else {
                    ImGui::TextColored(kColDim, "-");
                }
            }
            ImGui::TableNextColumn();
            if (m.valid) {
                ImGui::TextColored(kColDim, "%08X", m.rng);
            }
            ImGui::TableNextColumn();
            ImGui::BeginDisabled(!InGameplay());
            if (ImGui::SmallButton("save")) {
                SetStateSlot(i);
                SaveState(i);
            }
            ImGui::SameLine();
            ImGui::BeginDisabled(!m.valid);
            if (ImGui::SmallButton("load")) {
                SetStateSlot(i);
                LoadState(i);
            }
            ImGui::EndDisabled();
            ImGui::EndDisabled();
            ImGui::PopID();
        }
        ImGui::EndTable();
    }

    ImGui::SeparatorText("Positions");
    if (ImGui::ArrowButton("##prevpos", ImGuiDir_Left)) {
        ExecuteCommand(CMD_PREV_POS);
    }
    ImGui::SameLine();
    const PositionSlot& ps = e.positions[e.positionSlot];
    if (ps.valid) {
        ImGui::Text("position %d: %.1f %.1f %.1f", e.positionSlot, ps.x, ps.y, ps.z);
    } else {
        ImGui::Text("position %d: empty", e.positionSlot);
    }
    ImGui::SameLine();
    if (ImGui::ArrowButton("##nextpos", ImGuiDir_Right)) {
        ExecuteCommand(CMD_NEXT_POS);
    }
    ImGui::BeginDisabled(!InGameplay());
    if (ImGui::Button("Save position")) {
        ExecuteCommand(CMD_SAVE_POS);
    }
    ImGui::SameLine();
    if (ImGui::Button("Load position")) {
        ExecuteCommand(CMD_LOAD_POS);
    }
    ImGui::SameLine();
    if (ImGui::Button("Void out")) {
        ExecuteCommand(CMD_VOID_OUT);
    }
    ImGui::SameLine();
    if (ImGui::Button("Reload scene")) {
        ExecuteCommand(CMD_RELOAD_SCENE);
    }
    ImGui::EndDisabled();
}

// Frame-by-frame macro editor ("piano roll")
void ShipGZWindow::DrawInputEditorTab() {
    Engine& e = Get();
    auto& inputs = e.movie.inputs;

    ImGui::Checkbox("Follow current frame", &sFollowFrame);
    ImGui::SameLine();
    ImGui::SetNextItemWidth(80);
    ImGui::InputInt("##inscount", &sInsertCount);
    sInsertCount = std::clamp(sInsertCount, 1, 10000);
    ImGui::SameLine();
    if (ImGui::Button("Append frames")) {
        InsertFrames((int32_t)inputs.size(), sInsertCount);
    }
    HelpMarker("Click a button cell to toggle it, drag to paint a column. Drag stick values or ctrl+click to type. "
               "Right-click a frame number for insert / delete / seek. The highlighted row is the next frame the macro "
               "will record or play. 'rng' marks frames where an RNG reseed was recorded.");

    const int numCols = 1 + kNumButtons + 4 + 2;
    ImGuiTableFlags flags = ImGuiTableFlags_ScrollY | ImGuiTableFlags_ScrollX | ImGuiTableFlags_RowBg |
                            ImGuiTableFlags_BordersInnerV | ImGuiTableFlags_SizingFixedFit;
    if (!ImGui::BeginTable("ShipGZPianoRoll", numCols, flags, ImVec2(0, -1))) {
        return;
    }
    ImGui::TableSetupScrollFreeze(1, 1);
    ImGui::TableSetupColumn("frame", ImGuiTableColumnFlags_WidthFixed, 64);
    for (const auto& b : kButtons) {
        ImGui::TableSetupColumn(b.label, ImGuiTableColumnFlags_WidthFixed, 22);
    }
    ImGui::TableSetupColumn("x", ImGuiTableColumnFlags_WidthFixed, 44);
    ImGui::TableSetupColumn("y", ImGuiTableColumnFlags_WidthFixed, 44);
    ImGui::TableSetupColumn("rx", ImGuiTableColumnFlags_WidthFixed, 44);
    ImGui::TableSetupColumn("ry", ImGuiTableColumnFlags_WidthFixed, 44);
    ImGui::TableSetupColumn("rst", ImGuiTableColumnFlags_WidthFixed, 26);
    ImGui::TableSetupColumn("rng", ImGuiTableColumnFlags_WidthFixed, 30);
    ImGui::TableHeadersRow();

    float rowHeight = ImGui::GetFrameHeight();
    if (sFollowFrame && sLastFollowedFrame != e.frame) {
        sLastFollowedFrame = e.frame;
        float visible = ImGui::GetWindowHeight();
        ImGui::SetScrollY(std::max(0.0f, e.frame * (rowHeight + ImGui::GetStyle().CellPadding.y * 2) - visible * 0.5f));
    }

    if (!ImGui::IsMouseDown(ImGuiMouseButton_Left)) {
        sPaintColumn = -1;
    }

    // seeds lookup: frames with a reseed (seeds are sorted by frame)
    const auto& seeds = e.movie.seeds;

    ImGuiListClipper clipper;
    clipper.Begin((int)inputs.size(), rowHeight);
    while (clipper.Step()) {
        for (int row = clipper.DisplayStart; row < clipper.DisplayEnd; row++) {
            ImGui::TableNextRow(ImGuiTableRowFlags_None, rowHeight);
            if (row == e.frame) {
                ImGui::TableSetBgColor(ImGuiTableBgTarget_RowBg0, IM_COL32(0x30, 0x60, 0xA0, 0xA0));
            } else if (row < e.frame) {
                ImGui::TableSetBgColor(ImGuiTableBgTarget_RowBg0, IM_COL32(0x30, 0x30, 0x30, 0x60));
            }
            ImGui::PushID(row);

            ImGui::TableNextColumn();
            ImGui::AlignTextToFramePadding();
            ImGui::Selectable(std::to_string(row).c_str(), false);
            if (ImGui::IsItemClicked(ImGuiMouseButton_Right)) {
                sContextFrame = row;
                ImGui::OpenPopup("rowctx");
            }
            if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left)) {
                MovieSeek(row);
            }
            if (ImGui::BeginPopup("rowctx")) {
                ImGui::TextColored(kColDim, "frame %d", sContextFrame);
                if (ImGui::MenuItem("Seek here")) {
                    MovieSeek(sContextFrame);
                }
                if (ImGui::MenuItem("Pause when reaching this frame")) {
                    e.stopAtFrame = sContextFrame;
                    sSeekFrame = sContextFrame;
                }
                ImGui::Separator();
                if (ImGui::MenuItem("Insert frame before")) {
                    InsertFrames(sContextFrame, 1);
                }
                if (ImGui::MenuItem("Insert frame after")) {
                    InsertFrames(sContextFrame + 1, 1);
                }
                char label[64];
                snprintf(label, sizeof(label), "Insert %d frames after", sInsertCount);
                if (ImGui::MenuItem(label)) {
                    InsertFrames(sContextFrame + 1, sInsertCount);
                }
                if (ImGui::MenuItem("Delete frame")) {
                    DeleteFrames(sContextFrame, 1);
                }
                if (ImGui::MenuItem("Clear input")) {
                    if (sContextFrame >= 0 && sContextFrame < (int32_t)inputs.size()) {
                        inputs[sContextFrame] = MovieInput();
                    }
                }
                ImGui::EndPopup();
            }
            if (row >= (int)inputs.size()) {
                // the popup may have deleted rows
                ImGui::PopID();
                break;
            }
            // fetched after the context menu, which may have inserted / deleted frames
            MovieInput& in = inputs[row];

            for (int b = 0; b < kNumButtons; b++) {
                ImGui::TableNextColumn();
                bool on = (in.buttons & kButtons[b].bit) != 0;
                ImVec2 pos = ImGui::GetCursorScreenPos();
                ImVec2 size(20, rowHeight);
                ImGui::PushID(b);
                ImGui::InvisibleButton("##btn", size);
                ImGui::PopID();
                bool hovered = ImGui::IsItemHovered();
                if (ImGui::IsItemActivated()) {
                    sPaintColumn = b;
                    sPaintValue = !on;
                }
                if (sPaintColumn == b && hovered && ImGui::IsMouseDown(ImGuiMouseButton_Left)) {
                    on = sPaintValue;
                    if (on) {
                        in.buttons |= kButtons[b].bit;
                    } else {
                        in.buttons &= ~kButtons[b].bit;
                    }
                }
                ImDrawList* dl = ImGui::GetWindowDrawList();
                if (on) {
                    dl->AddRectFilled(ImVec2(pos.x + 2, pos.y + 2), ImVec2(pos.x + size.x - 2, pos.y + size.y - 2),
                                      kButtons[b].color, 3.0f);
                    ImVec2 ts = ImGui::CalcTextSize(kButtons[b].label);
                    dl->AddText(ImVec2(pos.x + (size.x - ts.x) * 0.5f, pos.y + (size.y - ts.y) * 0.5f),
                                IM_COL32(0, 0, 0, 0xFF), kButtons[b].label);
                } else if (hovered) {
                    dl->AddRect(ImVec2(pos.x + 2, pos.y + 2), ImVec2(pos.x + size.x - 2, pos.y + size.y - 2),
                                IM_COL32(0x80, 0x80, 0x80, 0xFF), 3.0f);
                }
            }

            int8_t* sticks[4] = { &in.stickX, &in.stickY, &in.rStickX, &in.rStickY };
            for (int s = 0; s < 4; s++) {
                ImGui::TableNextColumn();
                int v = *sticks[s];
                ImGui::SetNextItemWidth(-FLT_MIN);
                char id[8];
                snprintf(id, sizeof(id), "##s%d", s);
                if (ImGui::DragInt(id, &v, 0.5f, -128, 127)) {
                    *sticks[s] = (int8_t)std::clamp(v, -128, 127);
                }
            }

            ImGui::TableNextColumn();
            bool rst = (in.flags & INPUT_FLAG_RESET) != 0;
            if (ImGui::Checkbox("##rst", &rst)) {
                in.flags = rst ? (in.flags | INPUT_FLAG_RESET) : (in.flags & ~INPUT_FLAG_RESET);
            }

            ImGui::TableNextColumn();
            auto it = std::lower_bound(seeds.begin(), seeds.end(), row,
                                       [](const MovieSeed& s, int32_t f) { return s.frame < f; });
            if (it != seeds.end() && it->frame == row) {
                ImGui::TextColored(kColPause, "rng");
                if (ImGui::IsItemHovered()) {
                    ImGui::SetTooltip("reseed before this frame\nold %08X\nnew %08X", it->oldSeed, it->newSeed);
                }
            }

            ImGui::PopID();
        }
    }
    ImGui::EndTable();
}

// gz_macro.c virtual controller menu
void ShipGZWindow::DrawVirtualControllerTab() {
    Engine& e = Get();
    ImGui::TextColored(kColDim, "A virtual controller replaces a port's input. Controller 1 is what macros record.");
    for (int i = 0; i < kNumPorts; i++) {
        VirtualController& vc = e.vcont[i];
        ImGui::PushID(i);
        char label[32];
        snprintf(label, sizeof(label), "controller %d", i + 1);
        ImGui::SeparatorText(label);
        ImGui::Checkbox("override", &vc.enabled);
        ImGui::BeginDisabled(!vc.enabled);
        ImGui::SameLine();
        ImGui::Checkbox("plugged in", &vc.pluggedIn);
        int x = vc.input.stickX;
        int y = vc.input.stickY;
        ImGui::SetNextItemWidth(200);
        if (ImGui::SliderInt("stick x", &x, -128, 127)) {
            vc.input.stickX = (int8_t)x;
        }
        ImGui::SameLine();
        ImGui::SetNextItemWidth(200);
        if (ImGui::SliderInt("stick y", &y, -128, 127)) {
            vc.input.stickY = (int8_t)y;
        }
        for (int b = 0; b < kNumButtons; b++) {
            bool on = (vc.input.buttons & kButtons[b].bit) != 0;
            if (b > 0) {
                ImGui::SameLine();
            }
            if (ImGui::Checkbox(kButtons[b].label, &on)) {
                vc.input.buttons = on ? (vc.input.buttons | kButtons[b].bit) : (vc.input.buttons & ~kButtons[b].bit);
            }
        }
        ImGui::EndDisabled();
        ImGui::PopID();
    }
}

// gz settings > commands: every command can be run from here, bound to a key and to a controller combo
void ShipGZWindow::DrawCommandsTab() {
    ImGui::TextColored(kColDim, "Click a key or combo to rebind it. Press Escape to cancel, Backspace to unbind. "
                                "Controller combos: hold the buttons, then release. Buttons of an active combo are "
                                "hidden from the game.");

    // key capture
    if (sCaptureKeyCmd >= 0) {
        for (int k = ImGuiKey_NamedKey_BEGIN; k < ImGuiKey_NamedKey_END; k++) {
            if (!ImGui::IsKeyPressed((ImGuiKey)k, false)) {
                continue;
            }
            if (k == ImGuiKey_MouseLeft || k == ImGuiKey_MouseRight || k == ImGuiKey_MouseMiddle ||
                (k >= ImGuiKey_GamepadStart && k <= ImGuiKey_GamepadRStickDown) || k == ImGuiKey_ReservedForModCtrl ||
                k == ImGuiKey_ReservedForModShift || k == ImGuiKey_ReservedForModAlt ||
                k == ImGuiKey_ReservedForModSuper) {
                continue;
            }
            if (k == ImGuiKey_Escape) {
                sCaptureKeyCmd = -1;
                break;
            }
            SetCommandKey(sCaptureKeyCmd, k == ImGuiKey_Backspace ? 0 : k);
            sCaptureKeyCmd = -1;
            break;
        }
    }
    // controller combo capture (raw controller 1)
    if (sCapturePadCmd >= 0) {
        static OSContPad pads[4]; // static: LUS keeps a pointer to the last array it filled
        osContGetReadData(pads);
        uint32_t held = pads[0].button;
        if (held != 0) {
            sCapturePadAccum |= held;
            sCapturePadSawInput = true;
        } else if (sCapturePadSawInput) {
            SetCommandPad(sCapturePadCmd, sCapturePadAccum);
            sCapturePadCmd = -1;
        }
        if (ImGui::IsKeyPressed(ImGuiKey_Escape, false)) {
            sCapturePadCmd = -1;
        } else if (ImGui::IsKeyPressed(ImGuiKey_Backspace, false)) {
            SetCommandPad(sCapturePadCmd, 0);
            sCapturePadCmd = -1;
        }
    }

    if (ImGui::BeginTable("ShipGZCommands", 3, ImGuiTableFlags_RowBg | ImGuiTableFlags_BordersInnerV |
                                                   ImGuiTableFlags_ScrollY,
                          ImVec2(0, -ImGui::GetFrameHeightWithSpacing()))) {
        ImGui::TableSetupScrollFreeze(0, 1);
        ImGui::TableSetupColumn("command (click to run)");
        ImGui::TableSetupColumn("key", ImGuiTableColumnFlags_WidthFixed, 130);
        ImGui::TableSetupColumn("controller", ImGuiTableColumnFlags_WidthFixed, 170);
        ImGui::TableHeadersRow();
        for (int cmd = 0; cmd < CMD_COUNT; cmd++) {
            const CommandInfo& info = GetCommandInfo(cmd);
            ImGui::PushID(cmd);
            ImGui::TableNextRow();
            ImGui::TableNextColumn();
            if (ImGui::Selectable(info.name)) {
                ExecuteCommand(cmd);
            }
            if (ImGui::IsItemHovered()) {
                ImGui::SetTooltip("%s", info.tooltip);
            }
            ImGui::TableNextColumn();
            std::string key = sCaptureKeyCmd == cmd ? "press a key..." : KeyName(GetCommandKey(cmd));
            if (ImGui::Button((key + "##key").c_str(), ImVec2(-FLT_MIN, 0))) {
                sCaptureKeyCmd = cmd;
                sCapturePadCmd = -1;
            }
            ImGui::TableNextColumn();
            std::string pad = sCapturePadCmd == cmd ? (sCapturePadSawInput ? ComboName(sCapturePadAccum) : "hold buttons...")
                                                    : ComboName(GetCommandPad(cmd));
            if (ImGui::Button((pad + "##pad").c_str(), ImVec2(-FLT_MIN, 0))) {
                sCapturePadCmd = cmd;
                sCaptureKeyCmd = -1;
                sCapturePadAccum = 0;
                sCapturePadSawInput = false;
            }
            ImGui::PopID();
        }
        ImGui::EndTable();
    }
    if (ImGui::Button("Restore default bindings")) {
        for (int cmd = 0; cmd < CMD_COUNT; cmd++) {
            SetCommandKey(cmd, GetCommandInfo(cmd).defaultKey);
            SetCommandPad(cmd, GetCommandInfo(cmd).defaultPad);
        }
    }
}

void ShipGZWindow::DrawSettingsTab() {
    ImGui::SeparatorText("Macro");
    CVarCheckbox("macro input", SHIPGZ_CVAR("MacroInput"), false,
                 "gz 'macro input': while a macro plays, live controller input is combined with it.");
    CVarCheckbox("pause at end of playback", SHIPGZ_CVAR("PauseAtEnd"), true,
                 "Freeze the game right after the last macro frame, so you can take over with frame advance.");
    CVarCheckbox("loop playback", SHIPGZ_CVAR("LoopPlayback"), false, "Restart the input stream when it ends.");
    CVarCheckbox("force recorded rng seeds", SHIPGZ_CVAR("ForceSeeds"), false,
                 "During playback, use recorded reseeds even when the RNG state before the reseed does not match "
                 "(gz only applies a seed when the RNG is still in sync).");

    ImGui::SeparatorText("Savestates");
    CVarCheckbox("ignore state's rng", SHIPGZ_CVAR("IgnoreStateRng"), false,
                 "gz 'ignore state's rng': keep the current RNG value when loading a state, e.g. to try different "
                 "RNG from the same position.");

    ImGui::SeparatorText("RNG");
    CVarCheckbox("fixed seed on scene load (no macro)", SHIPGZ_CVAR("FixedSeed"), false,
                 "While no macro is active, every scene load seeds the RNG with the value below instead of the clock. "
                 "Useful for practicing RNG-dependent setups.");
    int seed = CVarGetInteger(SHIPGZ_CVAR("FixedSeedValue"), 0);
    ImGui::SetNextItemWidth(160);
    if (ImGui::InputInt("seed value", &seed, 1, 100, ImGuiInputTextFlags_CharsHexadecimal)) {
        CVarSetInteger(SHIPGZ_CVAR("FixedSeedValue"), seed);
    }

    ImGui::SeparatorText("On-screen display");
    CVarCheckbox("show on-screen display", SHIPGZ_CVAR("Osd"), true, "Macro / pause status in the top-left corner.");
    CVarCheckbox("input display", SHIPGZ_CVAR("OsdInput"), true, "Show the input the game received this frame.");
    CVarCheckbox("log", SHIPGZ_CVAR("OsdLog"), true, "Show ShipGZ messages on screen.");
    float scale = CVarGetFloat(SHIPGZ_CVAR("OsdScale"), 1.0f);
    ImGui::SetNextItemWidth(160);
    if (ImGui::SliderFloat("display scale", &scale, 0.5f, 3.0f, "%.1f")) {
        CVarSetFloat(SHIPGZ_CVAR("OsdScale"), scale);
    }

    ImGui::SeparatorText("Determinism notes");
    ImGui::TextWrapped(
        "Recorded: controller 1 (buttons, both sticks, gyro), resets, and every RNG reseed. "
        "SoH polls input, loads rooms and runs audio in lock-step with the game, so the ocarina / room-load sync "
        "hacks gz needs on N64 are unnecessary. Things outside the macro still break sync: mouse aiming, "
        "Crowd Control / multiplayer, enhancements that use their own randomness, and changing settings that alter "
        "gameplay between recording and playback. Savestates live in memory only and do not survive a restart, so "
        "start long movies from a reset or a known save file.");
}
