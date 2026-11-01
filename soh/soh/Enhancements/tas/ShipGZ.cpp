/*
 * ShipGZ core: macro (movie) engine, pause / frame advance, determinism hooks, commands and file I/O.
 *
 * Behaviour is modelled on gz (src/gz/gz.c input_hook / state_main_hook / srand_hook / afx_rand_hook,
 * gz_movie.c, gz_macro.c). Differences from the N64 original are noted inline; the short version is
 * that SoH runs audio, input polling and room loading synchronously with the game thread, so the
 * "ocarina input", "ocarina sync" and "room load" sync hacks of gz are unnecessary here.
 */

#include "ShipGZCore.hpp"

#include <algorithm>
#include <chrono>
#include <cmath>
#include <cstdarg>
#include <cstdio>
#include <cstring>
#include <filesystem>
#include <fstream>

#include <imgui.h>
#include <spdlog/spdlog.h>
#include <ship/Context.h>
#include <ship/window/Window.h>
#include <libultraship/bridge/consolevariablebridge.h>

#include "soh/OTRGlobals.h"
#include "soh/util.h"
#include "soh/Enhancements/savestates.h"
#include "soh/Enhancements/game-interactor/GameInteractor.h"
#include "soh/Enhancements/game-interactor/GameInteractor_Hooks.h"

extern "C" {
#include "z64.h"
#include "macros.h"
#include "variables.h"
#include "functions.h"
extern PlayState* gPlayState;
void Play_LoadToLastEntrance(PlayState* play);
}

#define SHIPGZ_CVAR(name) "gShipGZ." name

namespace ShipGZ {

// ---------------------------------------------------------------------------------------------------------------
// Engine state
// ---------------------------------------------------------------------------------------------------------------

static Engine sEngine;
static std::vector<int> sPendingCommands; // commands detected mid-frame, executed after the frame
static std::array<int32_t, CMD_COUNT> sHeldFrames{};
static std::array<int32_t, CMD_COUNT> sHeldPadFrames{};

Engine& Get() {
    return sEngine;
}

static double NowSeconds() {
    using namespace std::chrono;
    return duration<double>(steady_clock::now().time_since_epoch()).count();
}

void Log(const char* fmt, ...) {
    char buf[256];
    va_list ap;
    va_start(ap, fmt);
    vsnprintf(buf, sizeof(buf), fmt, ap);
    va_end(ap);
    SPDLOG_INFO("[ShipGZ] {}", buf);
    auto& log = sEngine.log;
    log.push_front({ buf, NowSeconds() });
    while (log.size() > kLogMax) {
        log.pop_back();
    }
}

// ---------------------------------------------------------------------------------------------------------------
// Commands
// ---------------------------------------------------------------------------------------------------------------

static const CommandInfo sCommands[CMD_COUNT] = {
    { "ToggleWindow", "show/hide ShipGZ", "Shows or hides the ShipGZ window.", ImGuiKey_F8, 0, false },
    { "Pause", "pause/unpause",
      "Freezes the game. While frozen, nothing advances: game logic, input polling and audio are all held.",
      ImGuiKey_P, 0, false },
    { "Advance", "frame advance",
      "Advances one frame while frozen; freezes the game if it is running. Hold to advance continuously.", ImGuiKey_O,
      0, false },
    { "Record", "record macro", "Start/stop recording controller 1 into the macro.", ImGuiKey_F3, 0, false },
    { "Play", "play macro", "Start/stop macro playback.", ImGuiKey_F4, 0, false },
    { "Rewind", "rewind macro", "Seek the macro to frame 0.", ImGuiKey_Home, 0, false },
    { "Trim", "trim macro", "Delete every recorded input after the current macro frame.", 0, 0, false },
    { "SaveState", "save state", "Save a state to the current slot (stores the macro frame).", ImGuiKey_K, 0, false },
    { "LoadState", "load state", "Load the state in the current slot (seeks the macro to its frame).", ImGuiKey_L, 0,
      false },
    { "PrevState", "previous state slot", "Select the previous savestate slot.", ImGuiKey_LeftBracket, 0, false },
    { "NextState", "next state slot", "Select the next savestate slot.", ImGuiKey_RightBracket, 0, false },
    { "QuickRecord", "quick record movie",
      "Rewind, start recording, save the start state to slot 0 and select slot 1 for rerecording.", 0, 0, false },
    { "QuickPlay", "quick play movie", "Rewind, start playback and load the start state in slot 0.", 0, 0, false },
    { "Reset", "reset", "Reset the game to the title screen (recorded into the macro while recording).", 0, 0,
      false },
    { "Levitate", "levitate", "The classic L to levitate.", 0, 0, true },
    { "Turbo", "turbo", "Sets Link's linear velocity to 27.", 0, 0, true },
    { "SavePos", "save position", "Save Link's position and facing to the current position slot.", 0, 0, false },
    { "LoadPos", "load position", "Teleport Link to the position in the current position slot.", 0, 0, false },
    { "PrevPos", "previous position slot", "Select the previous position slot.", 0, 0, false },
    { "NextPos", "next position slot", "Select the next position slot.", 0, 0, false },
    { "VoidOut", "void out", "Respawn as if Link voided out.", 0, 0, false },
    { "ReloadScene", "reload scene", "Reload the current scene from its last entrance.", 0, 0, false },
    { "Slower", "slower", "Decrease the emulation speed.", ImGuiKey_Minus, 0, false },
    { "Faster", "faster", "Increase the emulation speed.", ImGuiKey_Equal, 0, false },
};

const CommandInfo& GetCommandInfo(int cmd) {
    return sCommands[cmd];
}

static std::string KeyCVar(int cmd) {
    return std::string(SHIPGZ_CVAR("Key.")) + sCommands[cmd].id;
}
static std::string PadCVar(int cmd) {
    return std::string(SHIPGZ_CVAR("Pad.")) + sCommands[cmd].id;
}

int GetCommandKey(int cmd) {
    return CVarGetInteger(KeyCVar(cmd).c_str(), sCommands[cmd].defaultKey);
}
void SetCommandKey(int cmd, int key) {
    CVarSetInteger(KeyCVar(cmd).c_str(), key);
    if (auto* context = Ship::Context::GetRawInstance()) {
        if (auto window = context->GetWindow()) {
            if (auto gui = window->GetGui()) {
                gui->SaveConsoleVariablesNextFrame();
            }
        }
    }
}
uint32_t GetCommandPad(int cmd) {
    return (uint32_t)CVarGetInteger(PadCVar(cmd).c_str(), (int32_t)sCommands[cmd].defaultPad);
}
void SetCommandPad(int cmd, uint32_t pad) {
    CVarSetInteger(PadCVar(cmd).c_str(), (int32_t)pad);
    if (auto* context = Ship::Context::GetRawInstance()) {
        if (auto window = context->GetWindow()) {
            if (auto gui = window->GetGui()) {
                gui->SaveConsoleVariablesNextFrame();
            }
        }
    }
}

static const float sSpeeds[] = { 0.05f, 0.1f, 0.25f, 0.5f, 0.75f, 1.0f, 1.5f, 2.0f, 3.0f, 4.0f };

static void StepSpeed(int dir) {
    float cur = CVarGetFloat(SHIPGZ_CVAR("Speed"), 1.0f);
    int idx = 0;
    float best = 1e9f;
    for (int i = 0; i < (int)(sizeof(sSpeeds) / sizeof(*sSpeeds)); i++) {
        float d = std::fabs(sSpeeds[i] - cur);
        if (d < best) {
            best = d;
            idx = i;
        }
    }
    idx = std::clamp(idx + dir, 0, (int)(sizeof(sSpeeds) / sizeof(*sSpeeds)) - 1);
    CVarSetFloat(SHIPGZ_CVAR("Speed"), sSpeeds[idx]);
    Log("speed %d%%", (int)std::lround(sSpeeds[idx] * 100.0f));
}

static Player* GetPlayer() {
    if (gPlayState == nullptr) {
        return nullptr;
    }
    return GET_PLAYER(gPlayState);
}

void ExecuteCommand(int cmd) {
    Engine& e = sEngine;
    switch (cmd) {
        case CMD_TOGGLE_WINDOW: {
            auto window = Ship::Context::GetRawInstance()->GetWindow()->GetGui()->GetGuiWindow("ShipGZ");
            if (window != nullptr) {
                window->ToggleVisibility();
            }
            break;
        }
        case CMD_PAUSE:
            TogglePause();
            break;
        case CMD_ADVANCE:
            FrameAdvance();
            break;
        case CMD_RECORD:
            if (e.state == MovieState::Recording) {
                StopRecording();
            } else {
                StartRecording();
            }
            break;
        case CMD_PLAY:
            if (e.state == MovieState::Playing) {
                StopPlayback();
            } else {
                StartPlayback();
            }
            break;
        case CMD_REWIND:
            MovieRewind();
            Log("macro rewound");
            break;
        case CMD_TRIM:
            MovieTrim();
            break;
        case CMD_SAVE_STATE:
            SaveState(GetStateSlot());
            break;
        case CMD_LOAD_STATE:
            LoadState(GetStateSlot());
            break;
        case CMD_PREV_STATE:
            SetStateSlot((GetStateSlot() + kNumStateSlots - 1) % kNumStateSlots);
            break;
        case CMD_NEXT_STATE:
            SetStateSlot((GetStateSlot() + 1) % kNumStateSlots);
            break;
        case CMD_QUICK_RECORD:
            QuickRecord();
            break;
        case CMD_QUICK_PLAY:
            QuickPlay();
            break;
        case CMD_RESET:
            RequestReset();
            break;
        case CMD_LEVITATE:
            if (Player* p = GetPlayer()) {
                p->actor.velocity.y = 6.34375f;
            }
            break;
        case CMD_TURBO:
            if (Player* p = GetPlayer()) {
                p->linearVelocity = 27.0f;
            }
            break;
        case CMD_SAVE_POS:
            if (Player* p = GetPlayer()) {
                PositionSlot& s = e.positions[e.positionSlot];
                s.valid = true;
                s.x = p->actor.world.pos.x;
                s.y = p->actor.world.pos.y;
                s.z = p->actor.world.pos.z;
                s.yaw = p->actor.shape.rot.y;
                Log("saved position %d", e.positionSlot);
            }
            break;
        case CMD_LOAD_POS:
            if (Player* p = GetPlayer()) {
                PositionSlot& s = e.positions[e.positionSlot];
                if (!s.valid) {
                    Log("position %d empty", e.positionSlot);
                    break;
                }
                p->actor.home.pos = { s.x, s.y, s.z };
                p->actor.world.pos = { s.x, s.y, s.z };
                p->actor.prevPos = { s.x, s.y, s.z };
                p->actor.shape.rot.y = s.yaw;
                p->actor.world.rot.y = s.yaw;
                p->yaw = s.yaw;
                Log("loaded position %d", e.positionSlot);
            }
            break;
        case CMD_PREV_POS:
            e.positionSlot = (e.positionSlot + kNumPositionSlots - 1) % kNumPositionSlots;
            Log("position slot %d", e.positionSlot);
            break;
        case CMD_NEXT_POS:
            e.positionSlot = (e.positionSlot + 1) % kNumPositionSlots;
            Log("position slot %d", e.positionSlot);
            break;
        case CMD_VOID_OUT:
            if (gPlayState != nullptr) {
                Play_TriggerVoidOut(gPlayState);
            }
            break;
        case CMD_RELOAD_SCENE:
            if (gPlayState != nullptr) {
                Play_LoadToLastEntrance(gPlayState);
            }
            break;
        case CMD_SLOWER:
            StepSpeed(-1);
            break;
        case CMD_FASTER:
            StepSpeed(1);
            break;
        default:
            break;
    }
}

// Called once per host frame with whatever the raw controller-1 buttons are. Detects button combos
// (gz: input_bind_*), computes the mask of buttons to hide from the game, and fires commands.
// If `deferred` is set the commands are queued and run after the current game frame finishes, so
// that e.g. "quick play" never takes effect halfway through a frame's input processing.
static void HandlePadCombos(uint32_t raw, bool deferred) {
    Engine& e = sEngine;
    uint32_t pressed = raw & ~e.padHeldPrev;
    uint32_t mask = 0;
    for (int cmd = 0; cmd < CMD_COUNT; cmd++) {
        uint32_t combo = GetCommandPad(cmd);
        if (combo == 0) {
            continue;
        }
        if ((raw & combo) != combo) {
            continue;
        }
        mask |= combo;
        bool fire = false;
        if (sCommands[cmd].continuous) {
            fire = true;
        } else if (pressed & combo) {
            fire = true;
            sHeldPadFrames[cmd] = 0;
        } else if (cmd == CMD_ADVANCE && e.framesQueued >= 0) {
            // held frame advance repeats after a short delay
            if (++sHeldPadFrames[cmd] > 15 && e.framesQueued == 0) {
                e.framesQueued = 1;
            }
        }
        if (fire) {
            if (deferred) {
                sPendingCommands.push_back(cmd);
            } else {
                ExecuteCommand(cmd);
            }
        }
    }
    e.padHeldPrev = raw;
    e.padMask = mask;
}

void PollKeyboardCommands() {
    ImGuiIO& io = ImGui::GetIO();
    if (io.WantTextInput) {
        sHeldFrames.fill(0);
        return;
    }
    for (int cmd = 0; cmd < CMD_COUNT; cmd++) {
        int key = GetCommandKey(cmd);
        if (key <= ImGuiKey_None || key >= ImGuiKey_NamedKey_END) {
            continue;
        }
        if (!ImGui::IsKeyDown((ImGuiKey)key)) {
            sHeldFrames[cmd] = 0;
            continue;
        }
        // ignore keys pressed together with Ctrl/Alt so they never clash with menu shortcuts
        if (io.KeyCtrl || io.KeyAlt) {
            continue;
        }
        int32_t held = sHeldFrames[cmd]++;
        if (sCommands[cmd].continuous) {
            ExecuteCommand(cmd);
        } else if (held == 0) {
            ExecuteCommand(cmd);
        } else if (cmd == CMD_ADVANCE && held > 20 && sEngine.framesQueued == 0) {
            // hold to advance continuously, one game frame per host frame
            sEngine.framesQueued = 1;
        }
    }
}

// ---------------------------------------------------------------------------------------------------------------
// Movie operations (gz_movie.c / gz_macro.c)
// ---------------------------------------------------------------------------------------------------------------

void MovieRewind() {
    sEngine.frame = 0;
    sEngine.seedPos = 0;
}

void MovieSeek(int32_t frame) {
    Engine& e = sEngine;
    frame = std::clamp<int32_t>(frame, 0, (int32_t)e.movie.inputs.size());
    e.frame = frame;
    // Reseeds in SoH happen between frames (Play_Init), with the macro frame equal to the number of
    // inputs consumed so far. A state saved at macro frame F is saved before any reseed at time F,
    // so seek to the first seed with frame >= F.
    size_t pos = 0;
    while (pos < e.movie.seeds.size() && e.movie.seeds[pos].frame < frame) {
        pos++;
    }
    e.seedPos = pos;
}

void MovieTrim() {
    Engine& e = sEngine;
    e.movie.inputs.resize(std::min<size_t>(e.movie.inputs.size(), e.frame));
    e.movie.seeds.resize(std::min<size_t>(e.movie.seeds.size(), (size_t)e.seedPos));
    e.movie.inputs.shrink_to_fit();
    Log("macro trimmed to %d frames", (int)e.movie.inputs.size());
}

void MovieClear() {
    Engine& e = sEngine;
    e.state = MovieState::Idle;
    e.movie = Movie();
    e.frame = 0;
    e.seedPos = 0;
    e.desyncCount = 0;
    for (auto& m : e.stateMeta) {
        m.movieFrame = -1;
    }
    Log("macro cleared");
}

void StartRecording() {
    Engine& e = sEngine;
    if (e.state == MovieState::Playing) {
        // gz lets you take over a playback at any frame and continue recording from there
        Log("recording from frame %d", e.frame);
    } else {
        Log("recording");
    }
    e.state = MovieState::Recording;
}

void StopRecording() {
    sEngine.state = MovieState::Idle;
    Log("recording stopped at %d / %d", sEngine.frame, (int)sEngine.movie.inputs.size());
}

void StartPlayback() {
    Engine& e = sEngine;
    if (e.movie.inputs.empty()) {
        Log("no macro to play");
        return;
    }
    if (e.frame >= (int32_t)e.movie.inputs.size()) {
        MovieRewind();
    }
    e.state = MovieState::Playing;
    e.desyncCount = 0;
    Log("playing from frame %d", e.frame);
}

void StopPlayback() {
    sEngine.state = MovieState::Idle;
    Log("playback stopped at %d", sEngine.frame);
}

void TogglePause() {
    Engine& e = sEngine;
    if (e.framesQueued == -1) {
        e.framesQueued = 0;
    } else {
        e.framesQueued = -1;
        e.speedAccumulator = 0.0f;
    }
}

void FrameAdvance() {
    Engine& e = sEngine;
    if (e.framesQueued == -1) {
        e.framesQueued = 0;
    } else {
        e.framesQueued++;
    }
}

void QuickRecord() {
    Engine& e = sEngine;
    if (!InGameplay()) {
        Log("can not save here");
        return;
    }
    MovieRewind();
    e.state = MovieState::Recording;
    SetStateSlot(0);
    SaveState(0);
    SetStateSlot(1);
    SaveState(1);
    Log("quick record: start state in slot 0");
}

void QuickPlay() {
    Engine& e = sEngine;
    const StateMeta& m = e.stateMeta[0];
    if (!InGameplay()) {
        Log("can not load here");
        return;
    }
    if (!m.valid || m.movieFrame != 0 || e.movie.inputs.empty()) {
        Log("no movie recorded");
        return;
    }
    MovieRewind();
    e.state = MovieState::Playing;
    e.desyncCount = 0;
    LoadState(0);
}

void RequestReset() {
    sEngine.resetPending = true;
    Log("reset requested");
}

void InsertFrames(int32_t at, int32_t count) {
    Engine& e = sEngine;
    at = std::clamp<int32_t>(at, 0, (int32_t)e.movie.inputs.size());
    MovieInput blank;
    if (at > 0) {
        blank = e.movie.inputs[at - 1];
        blank.flags = 0;
    }
    e.movie.inputs.insert(e.movie.inputs.begin() + at, count, blank);
    for (auto& s : e.movie.seeds) {
        if (s.frame >= at) {
            s.frame += count;
        }
    }
    if (e.frame > at) {
        e.frame += count;
    }
    MovieSeek(e.frame);
}

void DeleteFrames(int32_t at, int32_t count) {
    Engine& e = sEngine;
    if (at < 0 || at >= (int32_t)e.movie.inputs.size()) {
        return;
    }
    count = std::min<int32_t>(count, (int32_t)e.movie.inputs.size() - at);
    e.movie.inputs.erase(e.movie.inputs.begin() + at, e.movie.inputs.begin() + at + count);
    for (auto& s : e.movie.seeds) {
        if (s.frame > at) {
            s.frame = std::max<int32_t>(at, s.frame - count);
        }
    }
    if (e.frame > at) {
        e.frame = std::max<int32_t>(at, e.frame - count);
    }
    MovieSeek(e.frame);
}

// ---------------------------------------------------------------------------------------------------------------
// Savestates
// ---------------------------------------------------------------------------------------------------------------

bool InGameplay() {
    return gPlayState != nullptr;
}

uint32_t GetStateSlot() {
    return OTRGlobals::Instance->gSaveStateMgr->GetCurrentSlot();
}

void SetStateSlot(uint32_t slot) {
    OTRGlobals::Instance->gSaveStateMgr->SetCurrentSlot(slot % kNumStateSlots);
}

bool SaveState(uint32_t slot) {
    return OTRGlobals::Instance->gSaveStateMgr->AddRequest({ slot, RequestType::SAVE }) == SaveStateReturn::SUCCESS;
}

bool LoadState(uint32_t slot) {
    return OTRGlobals::Instance->gSaveStateMgr->AddRequest({ slot, RequestType::LOAD }) == SaveStateReturn::SUCCESS;
}

// ---------------------------------------------------------------------------------------------------------------
// Pad conversion
// ---------------------------------------------------------------------------------------------------------------

MovieInput PadToInput(const OSContPad& pad) {
    MovieInput in;
    in.buttons = pad.button;
    in.stickX = pad.stick_x;
    in.stickY = pad.stick_y;
    in.rStickX = pad.right_stick_x;
    in.rStickY = pad.right_stick_y;
    in.gyroX = pad.gyro_x;
    in.gyroY = pad.gyro_y;
    return in;
}

void InputToPad(const MovieInput& in, OSContPad& pad) {
    pad.button = in.buttons;
    pad.stick_x = in.stickX;
    pad.stick_y = in.stickY;
    pad.right_stick_x = in.rStickX;
    pad.right_stick_y = in.rStickY;
    pad.gyro_x = in.gyroX;
    pad.gyro_y = in.gyroY;
    pad.err_no = 0;
}

const char* SceneName(int16_t sceneNum) {
    if (sceneNum < 0) {
        return "?";
    }
    return SohUtils::GetSceneName(sceneNum).c_str();
}

// ---------------------------------------------------------------------------------------------------------------
// File I/O
// ---------------------------------------------------------------------------------------------------------------

static const char kMagic[4] = { 'S', 'G', 'Z', 'M' };
static const uint32_t kVersion = 1;

namespace {
struct Writer {
    std::vector<uint8_t> buf;
    void bu8(uint8_t v) {
        buf.push_back(v);
    }
    void bu32(uint32_t v) {
        for (int i = 0; i < 4; i++) {
            buf.push_back((v >> (i * 8)) & 0xFF);
        }
    }
    void bi32(int32_t v) {
        bu32((uint32_t)v);
    }
    void bf32(float v) {
        uint32_t u;
        memcpy(&u, &v, 4);
        bu32(u);
    }
    void str(const std::string& s) {
        bu32((uint32_t)s.size());
        buf.insert(buf.end(), s.begin(), s.end());
    }
};

struct Reader {
    const std::vector<uint8_t>& buf;
    size_t pos = 0;
    bool bigEndian = false;
    bool ok = true;
    explicit Reader(const std::vector<uint8_t>& b) : buf(b) {
    }
    bool has(size_t n) const {
        return pos + n <= buf.size();
    }
    uint8_t bu8() {
        if (!has(1)) {
            ok = false;
            return 0;
        }
        return buf[pos++];
    }
    uint16_t bu16() {
        if (!has(2)) {
            ok = false;
            return 0;
        }
        uint16_t v = bigEndian ? (uint16_t)((buf[pos] << 8) | buf[pos + 1]) : (uint16_t)(buf[pos] | (buf[pos + 1] << 8));
        pos += 2;
        return v;
    }
    uint32_t bu32() {
        if (!has(4)) {
            ok = false;
            return 0;
        }
        uint32_t v = 0;
        for (int i = 0; i < 4; i++) {
            uint32_t b = buf[pos + i];
            v |= bigEndian ? (b << ((3 - i) * 8)) : (b << (i * 8));
        }
        pos += 4;
        return v;
    }
    int32_t bi32() {
        return (int32_t)bu32();
    }
    float bf32() {
        uint32_t u = bu32();
        float f;
        memcpy(&f, &u, 4);
        return f;
    }
    std::string str() {
        uint32_t n = bu32();
        if (!ok || !has(n) || n > (1u << 20)) {
            ok = false;
            return {};
        }
        std::string s(buf.begin() + pos, buf.begin() + pos + n);
        pos += n;
        return s;
    }
};

bool ReadFile(const std::string& path, std::vector<uint8_t>& out) {
    std::ifstream f(path, std::ios::binary);
    if (!f) {
        return false;
    }
    out.assign(std::istreambuf_iterator<char>(f), std::istreambuf_iterator<char>());
    return true;
}

std::string WriteFileAtomic(const std::string& path, const std::vector<uint8_t>& data) {
    std::error_code ec;
    std::filesystem::path p(path);
    if (p.has_parent_path()) {
        std::filesystem::create_directories(p.parent_path(), ec);
    }
    std::string tmp = path + ".tmp";
    {
        std::ofstream f(tmp, std::ios::binary | std::ios::trunc);
        if (!f) {
            return "could not open " + path + " for writing";
        }
        f.write((const char*)data.data(), (std::streamsize)data.size());
        if (!f) {
            return "write failed";
        }
    }
    std::filesystem::rename(tmp, path, ec);
    if (ec) {
        std::filesystem::remove(path, ec);
        std::filesystem::rename(tmp, path, ec);
        if (ec) {
            return "could not replace " + path + ": " + ec.message();
        }
    }
    return {};
}
} // namespace

std::string GetTasDirectory() {
    return Ship::Context::GetPathRelativeToAppDirectory("tas");
}

std::string ExportMovie(const std::string& path) {
    const Movie& m = sEngine.movie;
    Writer w;
    for (char c : kMagic) {
        w.bu8((uint8_t)c);
    }
    w.bu32(kVersion);
    w.bu32(1); // number of recorded ports
    w.bu32((uint32_t)m.inputs.size());
    w.bu32((uint32_t)m.seeds.size());
    w.bi32(m.rerecords);
    w.bi32(m.lastRecordedFrame);
    w.str(m.author);
    w.str(m.description);
    w.str(m.startNote);
    for (const MovieInput& in : m.inputs) {
        w.bu32(in.buttons);
        w.bu8((uint8_t)in.stickX);
        w.bu8((uint8_t)in.stickY);
        w.bu8((uint8_t)in.rStickX);
        w.bu8((uint8_t)in.rStickY);
        w.bf32(in.gyroX);
        w.bf32(in.gyroY);
        w.bu8(in.flags);
    }
    for (const MovieSeed& s : m.seeds) {
        w.bi32(s.frame);
        w.bu32(s.oldSeed);
        w.bu32(s.newSeed);
    }
    std::string err = WriteFileAtomic(path, w.buf);
    if (err.empty()) {
        Log("exported %d frames", (int)m.inputs.size());
    }
    return err;
}

std::string ImportMovie(const std::string& path) {
    std::vector<uint8_t> data;
    if (!ReadFile(path, data)) {
        return "could not open " + path;
    }
    if (data.size() >= 4 && memcmp(data.data(), kMagic, 4) != 0) {
        // not a ShipGZ macro; maybe it is a gz macro
        return ImportGzMacro(path);
    }
    Reader r(data);
    r.pos = 4;
    uint32_t version = r.bu32();
    if (!r.ok || version == 0 || version > kVersion) {
        return "unsupported macro version";
    }
    uint32_t ports = r.bu32();
    uint32_t nInputs = r.bu32();
    uint32_t nSeeds = r.bu32();
    Movie m;
    m.rerecords = r.bi32();
    m.lastRecordedFrame = r.bi32();
    m.author = r.str();
    m.description = r.str();
    m.startNote = r.str();
    if (!r.ok || ports < 1 || ports > (uint32_t)kNumPorts) {
        return "corrupt macro header";
    }
    const size_t inputSize = 17;
    if (!r.has((size_t)nInputs * inputSize * ports + (size_t)nSeeds * 12)) {
        return "unexpected end of file";
    }
    m.inputs.resize(nInputs);
    for (uint32_t i = 0; i < nInputs; i++) {
        for (uint32_t p = 0; p < ports; p++) {
            MovieInput in;
            in.buttons = r.bu32();
            in.stickX = (int8_t)r.bu8();
            in.stickY = (int8_t)r.bu8();
            in.rStickX = (int8_t)r.bu8();
            in.rStickY = (int8_t)r.bu8();
            in.gyroX = r.bf32();
            in.gyroY = r.bf32();
            in.flags = r.bu8();
            if (p == 0) {
                m.inputs[i] = in;
            }
        }
    }
    m.seeds.resize(nSeeds);
    for (uint32_t i = 0; i < nSeeds; i++) {
        m.seeds[i].frame = r.bi32();
        m.seeds[i].oldSeed = r.bu32();
        m.seeds[i].newSeed = r.bu32();
    }
    if (!r.ok) {
        return "unexpected end of file";
    }
    sEngine.state = MovieState::Idle;
    sEngine.movie = std::move(m);
    MovieRewind();
    Log("imported %d frames", (int)nInputs);
    return {};
}

// gz macro format (.gzm, gz_macro.c do_export_macro). Written by an N64, so big-endian with 32-bit size_t.
// Only inputs, reset flags and RNG seeds carry over; N64 lag / ocarina sync data has no SoH equivalent.
std::string ImportGzMacro(const std::string& path) {
    std::vector<uint8_t> data;
    if (!ReadFile(path, data)) {
        return "could not open " + path;
    }
    Reader r(data);
    r.bigEndian = true;
    uint32_t nInput = r.bu32();
    uint32_t nSeed = r.bu32();
    if (!r.ok || !r.has(4 + (size_t)nInput * 6 + (size_t)nSeed * 12)) {
        return "not a ShipGZ or gz macro";
    }
    r.pos += 4; // movie_input_start: raw pad before frame 0, not needed
    Movie m;
    m.inputs.resize(nInput);
    for (uint32_t i = 0; i < nInput; i++) {
        uint16_t pad = r.bu16();
        int8_t x = (int8_t)r.bu8();
        int8_t y = (int8_t)r.bu8();
        uint16_t delta = r.bu16();
        MovieInput& in = m.inputs[i];
        in.buttons = pad & 0xFF3F; // 0x0040/0x0080 are unused on N64 but are SoH modifier buttons
        in.stickX = x;
        in.stickY = y;
        in.flags = (delta & 0x0080) ? INPUT_FLAG_RESET : 0;
    }
    m.seeds.resize(nSeed);
    for (uint32_t i = 0; i < nSeed; i++) {
        m.seeds[i].frame = r.bi32();
        m.seeds[i].oldSeed = r.bu32();
        m.seeds[i].newSeed = r.bu32();
    }
    // skip ocarina input / sync / room load records if present, then read rerecords
    if (r.has(12)) {
        uint32_t nOcaInput = r.bu32();
        uint32_t nOcaSync = r.bu32();
        uint32_t nRoomLoad = r.bu32();
        size_t skip = (size_t)nOcaInput * 8 + (size_t)nOcaSync * 8 + (size_t)nRoomLoad * 4;
        if (r.has(skip)) {
            r.pos += skip;
            if (r.has(8)) {
                m.rerecords = r.bi32();
                m.lastRecordedFrame = r.bi32();
            }
        }
    }
    m.startNote = "imported from gz macro " + std::filesystem::path(path).filename().string();
    sEngine.state = MovieState::Idle;
    sEngine.movie = std::move(m);
    MovieRewind();
    Log("imported gz macro: %d frames", (int)nInput);
    return {};
}

std::string ExportInputCsv(const std::string& path) {
    static const struct {
        uint32_t bit;
        const char* name;
    } kButtons[] = {
        { BTN_A, "A" },      { BTN_B, "B" },      { BTN_Z, "Z" },          { BTN_START, "S" },
        { BTN_L, "L" },      { BTN_R, "R" },      { BTN_CUP, "Cu" },       { BTN_CDOWN, "Cd" },
        { BTN_CLEFT, "Cl" }, { BTN_CRIGHT, "Cr" }, { BTN_DUP, "Du" },      { BTN_DDOWN, "Dd" },
        { BTN_DLEFT, "Dl" }, { BTN_DRIGHT, "Dr" }, { 0x0040, "M1" },       { 0x0080, "M2" },
    };
    std::string out = "frame,buttons,stick_x,stick_y,rstick_x,rstick_y,gyro_x,gyro_y,reset\n";
    char line[256];
    const auto& inputs = sEngine.movie.inputs;
    for (size_t i = 0; i < inputs.size(); i++) {
        const MovieInput& in = inputs[i];
        std::string b;
        for (const auto& kb : kButtons) {
            if (in.buttons & kb.bit) {
                if (!b.empty()) {
                    b += ' ';
                }
                b += kb.name;
            }
        }
        snprintf(line, sizeof(line), "%zu,%s,%d,%d,%d,%d,%g,%g,%d\n", i, b.c_str(), in.stickX, in.stickY, in.rStickX,
                 in.rStickY, in.gyroX, in.gyroY, (in.flags & INPUT_FLAG_RESET) ? 1 : 0);
        out += line;
    }
    return WriteFileAtomic(path, std::vector<uint8_t>(out.begin(), out.end()));
}

} // namespace ShipGZ

// -------------------------------------------------------------------------------------------------------------------
// C entry points
// -------------------------------------------------------------------------------------------------------------------

using namespace ShipGZ;

static void ShipGZ_BeginFrameHook(bool* runGameFrame, bool* slowMotion) {
    Engine& e = sEngine;
    if (!CVarGetInteger(SHIPGZ_CVAR("Enabled"), 1)) {
        e.enabled = false;
        e.framesQueued = -1;
        e.state = MovieState::Idle;
        e.resetPending = false;
        e.resetRequestedThisFrame = false;
        sPendingCommands.clear();
        *runGameFrame = true;
        *slowMotion = false;
        return;
    }
    e.enabled = true;

    if (!e.hasPresentedFrame) {
        // There is no previous display list to show yet, so the very first frame always runs.
        *runGameFrame = true;
        *slowMotion = false;
        return;
    }

    bool run = false;
    if (e.framesQueued > 0) {
        e.framesQueued--;
        run = true;
    } else if (e.framesQueued == -1) {
        float speed = GameInteractor_ExecuteOnShipGZGetSpeed(1.0f);
        if (speed >= 1.0f) {
            run = true;
        } else {
            // slow motion: run a game frame only every 1/speed host frames, show the last one in between
            e.speedAccumulator += speed;
            if (e.speedAccumulator >= 1.0f) {
                e.speedAccumulator -= 1.0f;
                run = true;
            }
        }
    }

    if (!run) {
        // PadMgr does not run this host frame, so poll controller 1 ourselves for command combos.
        // Static: LUS's ControlDeck keeps a pointer to the last array it filled (trackers read it).
        static OSContPad pads[4];
        osContGetReadData(pads);
        HandlePadCombos(pads[0].button, false);
        // a combo might have just queued a frame (e.g. frame advance)
        if (e.framesQueued > 0) {
            e.framesQueued--;
            *runGameFrame = true;
            *slowMotion = false;
            return;
        }
        *runGameFrame = false;
        *slowMotion = e.framesQueued == -1;
        return;
    }
    *runGameFrame = true;
    *slowMotion = false;
}

static void ShipGZ_EndFrameHook(bool* resetGame) {
    Engine& e = sEngine;
    e.hasPresentedFrame = true;
    e.totalGameFrames++;

    // commands that were triggered by controller combos during the frame
    std::vector<int> pending;
    pending.swap(sPendingCommands);
    for (int cmd : pending) {
        ExecuteCommand(cmd);
    }

    if (e.resetRequestedThisFrame) {
        e.resetRequestedThisFrame = false;
        *resetGame = true;
        return;
    }
    *resetGame = false;
}

static void ShipGZ_ProcessPadsHook(OSContPad* pads, int32_t numPads) {
    Engine& e = sEngine;
    if (!CVarGetInteger(SHIPGZ_CVAR("Enabled"), 1)) {
        e.enabled = false;
        e.framesQueued = -1;
        e.state = MovieState::Idle;
        e.resetPending = false;
        e.resetRequestedThisFrame = false;
        sPendingCommands.clear();
        return;
    }
    e.enabled = true;
    if (numPads <= 0) {
        return;
    }

    // 1. command combos on the raw controller, then hide combo buttons from the game (gz: mask_input)
    HandlePadCombos(pads[0].button, true);
    pads[0].button &= ~e.padMask;

    // 2. virtual controllers (gz: gz_vcont_get)
    for (int i = 0; i < std::min<int>(numPads, kNumPorts); i++) {
        const VirtualController& vc = e.vcont[i];
        if (!vc.enabled) {
            continue;
        }
        if (!vc.pluggedIn) {
            memset(&pads[i], 0, sizeof(OSContPad));
            pads[i].err_no = 8; // CONT_NO_RESPONSE_ERROR
        } else {
            InputToPad(vc.input, pads[i]);
        }
    }

    MovieInput live = PadToInput(pads[0]);
    e.lastLiveInput = live;

    // 3. macro (gz: input_hook)
    if (e.state == MovieState::Recording) {
        Movie& m = e.movie;
        // clear rerecords for empty movies
        if (e.frame == 0 && m.inputs.empty()) {
            m.lastRecordedFrame = -1;
            m.rerecords = 0;
        }
        // Recording over old data: any seed still ahead of us at or before this frame belongs to the
        // previous take (seeds of this take were inserted before seedPos), so drop it.
        while (e.seedPos < m.seeds.size() && m.seeds[e.seedPos].frame <= e.frame) {
            m.seeds.erase(m.seeds.begin() + e.seedPos);
        }
        if (e.frame >= (int32_t)m.inputs.size()) {
            m.inputs.resize(e.frame + 1);
        }
        if (m.lastRecordedFrame >= e.frame) {
            m.rerecords++;
        }
        m.lastRecordedFrame = e.frame;
        MovieInput rec = live;
        rec.flags = 0;
        if (e.resetPending) {
            rec.flags |= INPUT_FLAG_RESET;
            e.resetPending = false;
            e.resetRequestedThisFrame = true;
        }
        m.inputs[e.frame++] = rec;
    } else if (e.state == MovieState::Playing) {
        Movie& m = e.movie;
        if (e.frame >= (int32_t)m.inputs.size()) {
            if (CVarGetInteger(SHIPGZ_CVAR("LoopPlayback"), 0) && !m.inputs.empty()) {
                MovieRewind();
            } else {
                e.state = MovieState::Idle;
            }
        }
        if (e.state == MovieState::Playing) {
            MovieInput in = m.inputs[e.frame++];
            if (CVarGetInteger(SHIPGZ_CVAR("MacroInput"), 0)) {
                // gz "macro input" setting: live input is combined with the macro
                in.buttons |= live.buttons;
                if (std::abs(in.stickX) < 8) {
                    in.stickX = live.stickX;
                }
                if (std::abs(in.stickY) < 8) {
                    in.stickY = live.stickY;
                }
                if (std::abs(in.rStickX) < 8) {
                    in.rStickX = live.rStickX;
                }
                if (std::abs(in.rStickY) < 8) {
                    in.rStickY = live.rStickY;
                }
            }
            InputToPad(in, pads[0]);
            if (in.flags & INPUT_FLAG_RESET) {
                e.resetRequestedThisFrame = true;
            }
            if (e.frame >= (int32_t)m.inputs.size() && !CVarGetInteger(SHIPGZ_CVAR("LoopPlayback"), 0)) {
                e.state = MovieState::Idle;
                Log("playback finished (%d frames)", (int)m.inputs.size());
                if (CVarGetInteger(SHIPGZ_CVAR("PauseAtEnd"), 1)) {
                    e.framesQueued = 0;
                }
            }
        }
    }
    if (e.state == MovieState::Idle && e.resetPending) {
        e.resetPending = false;
        e.resetRequestedThisFrame = true;
    }

    if (e.stopAtFrame >= 0 && e.state != MovieState::Idle && e.frame == e.stopAtFrame) {
        e.framesQueued = 0;
        Log("reached frame %d", e.stopAtFrame);
        e.stopAtFrame = -1;
    }

    e.lastGameInput = PadToInput(pads[0]);
}

static void ShipGZ_FilterRandSeedHook(uint32_t naturalSeed, uint32_t* seed) {
    *seed = naturalSeed;
    if (!CVarGetInteger(SHIPGZ_CVAR("Enabled"), 1)) {
        return;
    }
    Engine& e = sEngine;
    uint32_t old = Rand_GetState();

    if (e.state == MovieState::Recording) {
        // gz srand_hook: insert a recorded seed
        MovieSeed s;
        s.frame = e.frame;
        s.oldSeed = old;
        s.newSeed = naturalSeed;
        e.movie.seeds.insert(e.movie.seeds.begin() + e.seedPos, s);
        e.seedPos++;
        return;
    }

    if (e.state == MovieState::Playing) {
        auto& seeds = e.movie.seeds;
        // skip seeds we have already passed (e.g. after editing the input timeline)
        while (e.seedPos < seeds.size() && seeds[e.seedPos].frame < e.frame) {
            e.seedPos++;
        }
        if (e.seedPos < seeds.size() && seeds[e.seedPos].frame == e.frame) {
            const MovieSeed& s = seeds[e.seedPos++];
            if (s.oldSeed == old || CVarGetInteger(SHIPGZ_CVAR("ForceSeeds"), 0)) {
                *seed = s.newSeed;
                return;
            }
            e.desyncCount++;
            Log("rng desync detected at frame %d", e.frame);
        } else {
            e.desyncCount++;
            Log("unrecorded rng reseed at frame %d", e.frame);
        }
    }

    if (e.state == MovieState::Idle && CVarGetInteger(SHIPGZ_CVAR("FixedSeed"), 0)) {
        *seed = (uint32_t)CVarGetInteger(SHIPGZ_CVAR("FixedSeedValue"), 0);
    }
}

static void ShipGZ_FilterAudioRandomHook(uint32_t naturalValue, uint32_t* value) {
    *value = naturalValue;
    if (!CVarGetInteger(SHIPGZ_CVAR("Enabled"), 1)) {
        return;
    }
    Engine& e = sEngine;
    if (e.state == MovieState::Idle) {
        *value = naturalValue;
        return;
    }
    // gz afx_rand_hook: produce a number that is deterministic within macros. The audio RNG is
    // seeded from the CPU counter, so derive the value from the game RNG and the macro frame instead.
    uint32_t v = Rand_GetState() + (uint32_t)e.frame * 0x9E3779B9u;
    for (int i = 0; i < 3; i++) {
        v = v * 0x0019660D + 0x3C6EF35F;
    }
    *value = v;
}

static void ShipGZ_GetSpeedHook(float* speed) {
    if (!CVarGetInteger(SHIPGZ_CVAR("Enabled"), 1)) {
        *speed = 1.0f;
        return;
    }
    float configuredSpeed = CVarGetFloat(SHIPGZ_CVAR("Speed"), 1.0f);
    if (!(configuredSpeed > 0.0f)) {
        *speed = 1.0f;
        return;
    }
    *speed = std::clamp(configuredSpeed, 0.01f, 8.0f);
}

static void ShipGZ_StateSavedHook(uint32_t slot) {
    if (!CVarGetInteger(SHIPGZ_CVAR("Enabled"), 1) || slot >= (uint32_t)kNumStateSlots) {
        return;
    }
    Engine& e = sEngine;
    StateMeta& m = e.stateMeta[slot];
    m.valid = true;
    m.movieFrame = (e.state != MovieState::Idle) ? e.frame : -1;
    m.sceneNum = gPlayState != nullptr ? gPlayState->sceneNum : -1;
    m.roomNum = gPlayState != nullptr ? gPlayState->roomCtx.curRoom.num : -1;
    m.rng = Rand_GetState();
}

static void ShipGZ_StateLoadedHook(uint32_t slot) {
    if (!CVarGetInteger(SHIPGZ_CVAR("Enabled"), 1) || slot >= (uint32_t)kNumStateSlots) {
        return;
    }
    Engine& e = sEngine;
    const StateMeta& m = e.stateMeta[slot];
    // Loading a state saved during a macro restores that point in the movie. In recording mode,
    // discard the abandoned branch so later frames extend from the restored point rather than
    // leaving the previous take's future frames attached to this one.
    if (m.valid && m.movieFrame >= 0 && e.state != MovieState::Idle) {
        MovieSeek(m.movieFrame);
        if (e.state == MovieState::Recording) {
            MovieTrim();
        }
    }
    e.padHeldPrev = 0;
}

static void ShipGZ_BeforeStateSaveHook(uint32_t* rngSeed, void* padInputs, size_t numPadInputs) {
    // Preserve the existing savestate RNG/input snapshot regardless of whether ShipGZ is enabled.
    if (rngSeed == nullptr || padInputs == nullptr) {
        return;
    }
    *rngSeed = Rand_GetState();
    memcpy(padInputs, gPadMgr.inputs, std::min<size_t>(numPadInputs, ARRAY_COUNT(gPadMgr.inputs)) * sizeof(Input));
}

static void ShipGZ_AfterStateLoadHook(uint32_t rngSeed, void* padInputs, size_t numPadInputs) {
    // Restore the existing savestate RNG/input snapshot regardless of whether ShipGZ is enabled.
    if (padInputs == nullptr) {
        return;
    }
    if (!CVarGetInteger(SHIPGZ_CVAR("IgnoreStateRng"), 0)) {
        Rand_SetState(rngSeed);
    }
    memcpy(gPadMgr.inputs, padInputs, std::min<size_t>(numPadInputs, ARRAY_COUNT(gPadMgr.inputs)) * sizeof(Input));
}

extern "C" void ShipGZ_InitHooks(void) {
    static bool initialized = false;
    if (initialized || GameInteractor::Instance == nullptr) {
        return;
    }
    GameInteractor::Instance->RegisterGameHook<GameInteractor::OnShipGZBeginFrame>(ShipGZ_BeginFrameHook);
    GameInteractor::Instance->RegisterGameHook<GameInteractor::OnShipGZEndFrame>(ShipGZ_EndFrameHook);
    GameInteractor::Instance->RegisterGameHook<GameInteractor::OnShipGZProcessPads>(ShipGZ_ProcessPadsHook);
    GameInteractor::Instance->RegisterGameHook<GameInteractor::OnShipGZFilterRandSeed>(ShipGZ_FilterRandSeedHook);
    GameInteractor::Instance->RegisterGameHook<GameInteractor::OnShipGZFilterAudioRandom>(ShipGZ_FilterAudioRandomHook);
    GameInteractor::Instance->RegisterGameHook<GameInteractor::OnShipGZGetSpeed>(ShipGZ_GetSpeedHook);
    GameInteractor::Instance->RegisterGameHook<GameInteractor::OnShipGZStateSaved>(ShipGZ_StateSavedHook);
    GameInteractor::Instance->RegisterGameHook<GameInteractor::OnShipGZStateLoaded>(ShipGZ_StateLoadedHook);
    GameInteractor::Instance->RegisterGameHook<GameInteractor::OnShipGZBeforeStateSave>(ShipGZ_BeforeStateSaveHook);
    GameInteractor::Instance->RegisterGameHook<GameInteractor::OnShipGZAfterStateLoad>(ShipGZ_AfterStateLoadHook);
    initialized = true;
}

extern "C" int32_t ShipGZ_IsFrozen(void) {
    return sEngine.enabled && sEngine.framesQueued != -1;
}

extern "C" float ShipGZ_GetSpeed(void) {
    return GameInteractor_ExecuteOnShipGZGetSpeed(1.0f);
}

void ShipGZ_Update(int32_t commandCaptureActive) {
    if (!commandCaptureActive) {
        PollKeyboardCommands();
    }
}
