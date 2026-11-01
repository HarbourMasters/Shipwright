#pragma once
/*
 * ShipGZ engine internals shared between the core (ShipGZ.cpp) and the UI (ShipGZWindow.cpp).
 */

#include <array>
#include <cstdint>
#include <deque>
#include <string>
#include <vector>

#include "ShipGZ.h"

namespace ShipGZ {

constexpr int kNumPorts = 4;
constexpr int kNumStateSlots = 10;
constexpr int kNumPositionSlots = 10;
constexpr int kLogMax = 8;

enum class MovieState {
    Idle,
    Recording,
    Playing,
};

enum MovieInputFlags : uint8_t {
    INPUT_FLAG_RESET = 1 << 0, // a game reset is executed after this frame (gz: pad_delta bit 0x80)
};

// One frame of controller 1 input. Serialized field by field, so the in-memory layout is free.
struct MovieInput {
    uint32_t buttons = 0;
    int8_t stickX = 0;
    int8_t stickY = 0;
    int8_t rStickX = 0;
    int8_t rStickY = 0;
    float gyroX = 0.0f;
    float gyroY = 0.0f;
    uint8_t flags = 0;

    bool operator==(const MovieInput& o) const {
        return buttons == o.buttons && stickX == o.stickX && stickY == o.stickY && rStickX == o.rStickX &&
               rStickY == o.rStickY && gyroX == o.gyroX && gyroY == o.gyroY && flags == o.flags;
    }
};

// An RNG reseed observed while recording (gz: struct movie_seed).
// frame = macro frame counter at the time of the reseed (number of inputs consumed so far).
struct MovieSeed {
    int32_t frame = 0;
    uint32_t oldSeed = 0; // RNG value right before the reseed; used to verify sync
    uint32_t newSeed = 0;
};

struct Movie {
    std::vector<MovieInput> inputs;
    std::vector<MovieSeed> seeds;
    int32_t rerecords = 0;
    int32_t lastRecordedFrame = -1;
    std::string author;
    std::string description;
    // Where playback is expected to begin. Purely informational for the user.
    std::string startNote;
};

struct StateMeta {
    bool valid = false;
    int32_t movieFrame = -1; // -1 = state was saved while no macro was active
    int16_t sceneNum = -1;
    int16_t roomNum = -1;
    uint32_t rng = 0;
};

struct VirtualController {
    bool enabled = false;
    bool pluggedIn = true;
    MovieInput input;
};

struct PositionSlot {
    bool valid = false;
    float x = 0, y = 0, z = 0;
    int16_t yaw = 0;
};

enum Command : int {
    CMD_TOGGLE_WINDOW,
    CMD_PAUSE,
    CMD_ADVANCE,
    CMD_RECORD,
    CMD_PLAY,
    CMD_REWIND,
    CMD_TRIM,
    CMD_SAVE_STATE,
    CMD_LOAD_STATE,
    CMD_PREV_STATE,
    CMD_NEXT_STATE,
    CMD_QUICK_RECORD,
    CMD_QUICK_PLAY,
    CMD_RESET,
    CMD_LEVITATE,
    CMD_TURBO,
    CMD_SAVE_POS,
    CMD_LOAD_POS,
    CMD_PREV_POS,
    CMD_NEXT_POS,
    CMD_VOID_OUT,
    CMD_RELOAD_SCENE,
    CMD_SLOWER,
    CMD_FASTER,
    CMD_COUNT,
};

struct CommandInfo {
    const char* id;      // CVar suffix
    const char* name;    // display name
    const char* tooltip; // description
    int defaultKey;      // ImGuiKey (stored as int to keep this header ImGui free), 0 = unbound
    uint32_t defaultPad; // controller combo, 0 = unbound
    bool continuous;     // fires every host frame while held (e.g. levitate)
};

const CommandInfo& GetCommandInfo(int cmd);
int GetCommandKey(int cmd);
void SetCommandKey(int cmd, int key);
uint32_t GetCommandPad(int cmd);
void SetCommandPad(int cmd, uint32_t pad);
// Invoke a command (from UI or hotkey). Safe to call between frames on the game thread.
void ExecuteCommand(int cmd);

struct LogEntry {
    std::string msg;
    double time;
};

// Global engine state. Everything is touched from the game thread only (ImGui runs on it too).
struct Engine {
    // pause / frame advance: -1 = running, 0 = paused, >0 = frames queued (gz semantics)
    int32_t framesQueued = -1;
    bool advanceHeld = false;
    int32_t advanceHoldFrames = 0;
    float speedAccumulator = 0.0f;
    bool hasPresentedFrame = false;
    bool enabled = true;

    MovieState state = MovieState::Idle;
    Movie movie;
    int32_t frame = 0;        // macro frame: index of the next input to record / play
    size_t seedPos = 0;       // index of the next seed event
    int32_t stopAtFrame = -1; // pause automatically when the macro frame reaches this value
    bool resetPending = false;
    bool resetRequestedThisFrame = false;
    bool desyncWarned = false;
    int32_t desyncCount = 0;

    // live / game-side input of the most recent game frame, for the input display
    MovieInput lastGameInput;
    MovieInput lastLiveInput;

    std::array<VirtualController, kNumPorts> vcont;
    std::array<StateMeta, kNumStateSlots> stateMeta;
    std::array<PositionSlot, kNumPositionSlots> positions;
    int positionSlot = 0;

    // controller combos
    uint32_t padHeldPrev = 0;
    uint32_t padMask = 0;

    std::deque<LogEntry> log;

    uint64_t totalGameFrames = 0;
};

Engine& Get();

// Logging shown in the on-screen display and the window.
void Log(const char* fmt, ...);

// Movie operations (gz_movie.c equivalents)
void MovieRewind();
void MovieSeek(int32_t frame);
void MovieTrim();
void MovieClear();
void StartRecording();
void StopRecording();
void StartPlayback();
void StopPlayback();
void TogglePause();
void FrameAdvance();
void QuickRecord();
void QuickPlay();
void RequestReset();

// Input editing helpers (piano roll)
void InsertFrames(int32_t at, int32_t count);
void DeleteFrames(int32_t at, int32_t count);

// Savestates (shared with SoH's SaveStateMgr slots)
bool SaveState(uint32_t slot);
bool LoadState(uint32_t slot);
uint32_t GetStateSlot();
void SetStateSlot(uint32_t slot);
bool InGameplay();

// File I/O. Returns empty string on success, error message otherwise.
std::string ExportMovie(const std::string& path);
std::string ImportMovie(const std::string& path);
std::string ImportGzMacro(const std::string& path);
std::string ExportInputCsv(const std::string& path);
std::string GetTasDirectory();

// Command processing from the UI layer once per host frame (keyboard via ImGui).
void PollKeyboardCommands();

// Utilities
MovieInput PadToInput(const OSContPad& pad);
void InputToPad(const MovieInput& in, OSContPad& pad);
const char* SceneName(int16_t sceneNum);

} // namespace ShipGZ
