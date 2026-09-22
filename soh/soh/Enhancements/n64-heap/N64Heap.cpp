// Connects the N64 actor-heap shadow (N64HeapCore) and the object-space model
// (N64ObjectSpace) to Ship of Harkinian.
//
// Inputs:  ZeldaArena_Init / MallocDebug / MallocRDebug / FreeDebug (z_malloc.c),
//          GameInteractor OnActorSpawn, OnSceneInit and OnGameFrameUpdate.
// Output:  the value NTSC 1.2 would hold in play->csCtx.script and a verdict
//          on the data at that address (for example after the "pause in
//          Link's house" fix), logged whenever they change; with "Fix Wrong
//          Warps" enabled, N64Heap_FilterCutsceneScript also plays stale
//          cutscene pointers the way the N64 would.

#include "N64Heap.h"
#include "N64HeapCore.h"
#include "N64HeapTables.h"
#include "N64ObjectSpace.h"

#include <cstring>
#include <spdlog/spdlog.h>

#include <libultraship/bridge/consolevariablebridge.h>
#include <libultraship/bridge/resourcebridge.h>

#include "soh/Enhancements/game-interactor/GameInteractor.h"
#include "soh/ResourceManagerHelpers.h"
#include "soh/ShipInit.hpp"
#include "soh/cvar_prefixes.h"

extern "C" {
#include "z64.h"
#include "variables.h"
extern PlayState* gPlayState;
}

namespace {

n64heap::Core& Shadow() {
    static n64heap::Core core(n64heap::HostSizes{ sizeof(ColliderJntSphElement), sizeof(ColliderTrisElement),
                                                  sizeof(SkinLimbVtx), sizeof(void*) });
    return core;
}

n64heap::ObjectSpace sObjectSpace;
void* sLastHostScript = nullptr;
uint32_t sCutscenePointer = 0;
std::string sLastVerdict;
n64heap::Stats sLastStats;
uint16_t sLastPauseState = 0;
bool sVerdictDirty = false;

// N64 bytes of the files the model reads (objects, scene files, pause-screen
// files). The extractor stores them in oot.o2r as "n64heap/<file>/data", from
// the asset ymls in soh/assets/yml/ntsc_1-2/n64heap, so they exist only when
// the assets were generated from an NTSC 1.2 ROM.
bool ReadRom(uint32_t vrom, uint8_t* out, uint32_t size) {
    const n64heap::ArchiveFile* files = n64heap::kArchiveFiles;
    int lo = 0;
    int hi = n64heap::kArchiveFileCount - 1;
    while (lo < hi) { // last file starting at or before vrom
        int mid = (lo + hi + 1) / 2;
        if (files[mid].vrom <= vrom) {
            lo = mid;
        } else {
            hi = mid - 1;
        }
    }
    const n64heap::ArchiveFile& file = files[lo];
    if (vrom < file.vrom || vrom + size > file.vrom + file.size) {
        return false;
    }
    static const uint8_t* sData[n64heap::kArchiveFileCount];
    static bool sLoaded[n64heap::kArchiveFileCount];
    if (!sLoaded[lo]) {
        sLoaded[lo] = true;
        if (ResourceMgr_FileExists(file.path) && ResourceGetSizeByName(file.path) == file.size) {
            sData[lo] = static_cast<const uint8_t*>(ResourceGetDataByName(file.path));
        }
    }
    if (sData[lo] == nullptr) {
        return false;
    }
    memcpy(out, sData[lo] + (vrom - file.vrom), size);
    return true;
}

void OnActorSpawn(void* actor) {
    Shadow().OnActorSpawn(actor, static_cast<Actor*>(actor)->id);
}

void OnSceneInit(int16_t sceneNum) {
    sObjectSpace.SceneInit(sceneNum);
    sLastPauseState = 0;
    sVerdictDirty = true;
}

void ReportStats() {
    const n64heap::Stats& stats = Shadow().GetStats();
    if (stats.arenaCorrupt != sLastStats.arenaCorrupt || stats.failedAllocs != sLastStats.failedAllocs ||
        stats.unresolvedSpawns != sLastStats.unresolvedSpawns || stats.unknownSites != sLastStats.unknownSites ||
        stats.unknownFrees != sLastStats.unknownFrees) {
        SPDLOG_WARN("[N64Heap] corrupt={} failedAllocs={} unresolvedSpawns={} unknownSites={} unknownFrees={} "
                    "approximate={}",
                    stats.arenaCorrupt, stats.failedAllocs, stats.unresolvedSpawns, stats.unknownSites,
                    stats.unknownFrees, stats.approximateSizes);
    }
    sLastStats = stats;
}

void SyncObjectSpace() {
    ObjectContext& objects = gPlayState->objectCtx;
    int16_t ids[OBJECT_EXCHANGE_BANK_MAX];
    int count = objects.num < OBJECT_EXCHANGE_BANK_MAX ? objects.num : OBJECT_EXCHANGE_BANK_MAX;
    for (int i = 0; i < count; i++) {
        ids[i] = objects.status[i].id;
    }
    sObjectSpace.SyncObjects(ids, count);

    // KaleidoScope loads its data over the object space in PAUSE_STATE_INIT (3)
    // and PAUSE_STATE_GAME_OVER_INIT (10); leaving the pause screen
    // (state -> 0) reloads every object (Object_ReloadAll).
    uint16_t state = gPlayState->pauseCtx.state;
    if (state != sLastPauseState) {
        bool japanese = gSaveContext.language == LANGUAGE_JPN;
        if (state >= 3 && state <= 7 && !(sLastPauseState >= 3 && sLastPauseState <= 7)) {
            sObjectSpace.PauseOpened(gSaveContext.linkAge, japanese, gSaveContext.worldMapArea);
            sVerdictDirty = true;
        } else if (state >= 10 && state <= 17 && !(sLastPauseState >= 10 && sLastPauseState <= 17)) {
            sObjectSpace.GameOverOpened(japanese);
            sVerdictDirty = true;
        } else if (state == 0) {
            sObjectSpace.PauseClosed();
            sVerdictDirty = true;
        }
        sLastPauseState = state;
    }
}

void UpdatePointer() {
    void* script = gPlayState->csCtx.segment;
    if (script == sLastHostScript) {
        return;
    }
    sLastHostScript = script;
    if (script == nullptr) {
        return; // the N64 value is never cleared; keep the last one
    }
    uint32_t value = 0;
    std::string what;
    bool resolved;
    if (memcmp(script, "__OTR__", 7) == 0) {
        // Entrance cutscenes (Cutscene_SetSegment) hold a resource path, not data.
        char path[256];
        strncpy(path, static_cast<const char*>(script), sizeof(path) - 1);
        path[sizeof(path) - 1] = '\0';
        resolved = Shadow().ResolvePath(path, &value, &what);
        if (!resolved) {
            SPDLOG_WARN("[N64Heap] unrecognised cutscene path {}", path);
        }
    } else {
        int32_t words[4];
        memcpy(words, script, sizeof(words));
        resolved = Shadow().ResolveScript(words, gPlayState->sceneNum, &value, &what);
        if (!resolved) {
            SPDLOG_WARN("[N64Heap] unrecognised cutscene script (header {}, {}) in scene {}", words[0], words[1],
                        gPlayState->sceneNum);
        }
    }
    sCutscenePointer = resolved ? value : 0;
    if (resolved) {
        SPDLOG_INFO("[N64Heap] csCtx.script ({:#010x}) = {:#010x} ({})", n64heap::kCsScriptAddress, value, what);
    }
    sVerdictDirty = true;
}

std::string DescribeOutcome(const n64heap::ScriptSimulation& sim) {
    using Outcome = n64heap::ScriptSimulation::Outcome;
    switch (sim.outcome) {
        case Outcome::EndsImmediately:
            return "cutscene ends immediately (safe)";
        case Outcome::NoCommands:
            return sim.frameCount == 0
                       ? "no commands, ends after 1 frame (safe)"
                       : "no commands, cutscene lasts " + std::to_string(sim.frameCount) + " frames then ends";
        case Outcome::Hang:
            return "N64 locks up in the cutscene parser: " + sim.detail;
        case Outcome::RunsCommands:
            return "runs cutscene commands (" + sim.detail + "); safe only if this is real cutscene data";
        default:
            return "parser reaches data that is not modelled (" + sim.detail + ")";
    }
}

void UpdateVerdict() {
    if (!sVerdictDirty || sCutscenePointer == 0) {
        return;
    }
    sVerdictDirty = false;
    n64heap::ScriptSimulation sim = sObjectSpace.Simulate(sCutscenePointer, ReadRom);
    std::string line = sim.headerKnown ? sim.source + ": header (" + std::to_string(sim.totalEntries) + ", " +
                                             std::to_string(sim.frameCount) + ") -> " + DescribeOutcome(sim)
                                       : sim.source + ": unknown (data is not deterministic or not modelled)";
    if (line != sLastVerdict) {
        sLastVerdict = line;
        SPDLOG_INFO("[N64Heap] data at {:#010x}: {}", sCutscenePointer, line);
    }
}

void OnGameFrameUpdate() {
    Shadow().Flush();
    ReportStats();
    if (gPlayState == nullptr) {
        return;
    }
    SyncObjectSpace();
    UpdatePointer();
    UpdateVerdict();
}

void RegisterN64Heap() {
    GameInteractor::Instance->RegisterGameHook<GameInteractor::OnActorSpawn>(OnActorSpawn);
    GameInteractor::Instance->RegisterGameHook<GameInteractor::OnSceneInit>(OnSceneInit);
    GameInteractor::Instance->RegisterGameHook<GameInteractor::OnGameFrameUpdate>(OnGameFrameUpdate);
}

RegisterShipInitFunc sInitFunc(RegisterN64Heap);

} // namespace

extern "C" void N64Heap_OnArenaInit(void) {
    Shadow().ArenaInit(gSaveContext.nayrusLoveTimer == 2000);
}

extern "C" void N64Heap_OnAlloc(void* ptr, size_t size, const char* file, int reverse) {
    if (ptr != nullptr) {
        Shadow().OnAlloc(ptr, size, file, reverse != 0);
    }
}

extern "C" void N64Heap_OnFree(void* ptr) {
    if (ptr != nullptr) {
        Shadow().OnFree(ptr);
    }
}

extern "C" int N64Heap_HasN64Data(void) {
    return ResourceMgr_FileExists(n64heap::kArchiveFiles[0].path) ? 1 : 0;
}

extern "C" void* N64Heap_FilterCutsceneScript(void* script) {
    // Off by default: this deliberately changes gameplay to match the N64.
    if (script == nullptr || gPlayState == nullptr || !CVarGetInteger(CVAR_ENHANCEMENT("FixWrongWarps"), 0) ||
        !N64Heap_HasN64Data()) {
        return script;
    }
    // The pointer may have been set earlier this frame (e.g. by an actor), so
    // bring the shadow heap and the resolved N64 pointer up to date first.
    Shadow().Flush();
    UpdatePointer();
    if (sCutscenePointer == 0) {
        return script;
    }
    // Decide once per (script, pointer, memory state); the result is reused
    // on every frame the cutscene runs.
    static void* sDecidedScript = nullptr;
    static uint32_t sDecidedPointer = 0;
    static uint32_t sDecidedVersion = 0xFFFFFFFF;
    static void* sDecision = nullptr;
    static int32_t sReplacement[4];
    if (script == sDecidedScript && sCutscenePointer == sDecidedPointer && sObjectSpace.Version() == sDecidedVersion) {
        return sDecision;
    }
    sDecidedScript = script;
    sDecidedPointer = sCutscenePointer;
    sDecidedVersion = sObjectSpace.Version();
    sDecision = script;

    n64heap::ScriptSimulation sim = sObjectSpace.Simulate(sCutscenePointer, ReadRom);
    int32_t hostHeader[2];
    memcpy(hostHeader, script, sizeof(hostHeader));
    if (sim.outcome != n64heap::ScriptSimulation::Outcome::Unknown && sim.totalEntries == hostHeader[0] &&
        sim.frameCount == hostHeader[1]) {
        return script; // N64 memory still holds this script: nothing to emulate
    }
    using Outcome = n64heap::ScriptSimulation::Outcome;
    switch (sim.outcome) {
        case Outcome::EndsImmediately:
            sReplacement[0] = sim.totalEntries;
            sReplacement[1] = sim.frameCount;
            break;
        case Outcome::NoCommands:
            sReplacement[0] = 0; // nothing the N64 recognises runs; same length
            sReplacement[1] = sim.frameCount;
            break;
        case Outcome::Hang:
            // The N64 locks up inside the parser. Closest safe equivalent: a
            // cutscene that never ends and does nothing (no control = softlock).
            sReplacement[0] = 0;
            sReplacement[1] = 0x7FFFFFFF;
            break;
        case Outcome::RunsCommands:
            SPDLOG_WARN("[N64Heap] N64 cutscene data at {:#010x} not emulated ({}); SoH runs its own copy",
                        sCutscenePointer, sim.detail);
            return script;
        default:
            return script; // not modelled (e.g. actor heap) or not deterministic: leave SoH alone
    }
    sReplacement[2] = n64heap::kCsCmdEndOfScript;
    sReplacement[3] = 0;
    sDecision = sReplacement;
    SPDLOG_INFO("[N64Heap] running N64 cutscene data at {:#010x} ({}): header ({}, {}) -> {}{}", sCutscenePointer,
                sim.source, sim.totalEntries, sim.frameCount, DescribeOutcome(sim),
                sim.outcome == Outcome::Hang ? " (emulated as an endless cutscene)" : "");
    return sDecision;
}
