#include "N64Heap.h"
#include "N64ActorMemory.h"
#include "N64HeapCore.h"
#include "N64HeapTables.h"
#include "N64ObjectSpace.h"

#include <cstring>
#include <memory>
#include <spdlog/spdlog.h>
#include <vector>

#include <libultraship/bridge/consolevariablebridge.h>
#include <ship/Context.h>
#include <ship/resource/ResourceManager.h>
#include <ship/resource/type/Blob.h>

#include "soh/Enhancements/game-interactor/GameInteractor.h"
#include "soh/ResourceManagerHelpers.h"
#include "soh/ShipInit.hpp"
#include "soh/cvar_prefixes.h"

extern "C" {
#include "z64.h"
#include "variables.h"
extern PlayState* gPlayState;
}

#define CVAR_FIX_N64_HEAP CVAR_ENHANCEMENT("FixN64Heap")

namespace {

n64heap::Core& Shadow() {
    static n64heap::Core core(n64heap::HostSizes{ sizeof(ColliderJntSphElement), sizeof(ColliderTrisElement),
                                                  sizeof(SkinLimbVtx), sizeof(void*) });
    return core;
}

// The last host script and N64 pointer a cutscene decision was made for
struct CutsceneDecision {
    void* script = nullptr;
    uint32_t pointer = 0;
    uint32_t memoryVersion = UINT32_MAX;
    void* result = nullptr;
    std::vector<int32_t> replacement;
};

// The rebuilt N64 bytes of one live actor, valid for the frame they were built on
struct ActorMemoryCache {
    const void* actor = nullptr;
    uint32_t frame = 0;
    std::unique_ptr<n64heap::ActorMemory> memory;
};

n64heap::ObjectSpace sObjectSpace;
void* sLastHostScript = nullptr;
uint32_t sCutscenePointer = 0;
std::string sLastVerdict;
n64heap::Stats sLastStats;
uint16_t sLastPauseState = 0;
bool sVerdictDirty = false;
CutsceneDecision sDecision;
ActorMemoryCache sActorMemory;

bool Enabled() {
    return CVarGetInteger(CVAR_FIX_N64_HEAP, 0) && N64Heap_HasN64Data();
}

// Held so the resource manager cannot unload the data while the model reads it
std::shared_ptr<Ship::Blob> sArchiveData[n64heap::kArchiveFileCount];
bool sArchiveLoaded[n64heap::kArchiveFileCount];

// Index of the last archive file starting at or before vrom
int FindArchiveFile(uint32_t vrom) {
    int low = 0;
    int high = n64heap::kArchiveFileCount - 1;
    while (low < high) {
        int middle = (low + high + 1) / 2;
        if (n64heap::kArchiveFiles[middle].vrom <= vrom) {
            low = middle;
        } else {
            high = middle - 1;
        }
    }
    return low;
}

// N64 file data stored in oot.o2r as "n64heap/<file>/data" when the assets come from an NTSC 1.2 ROM
bool ReadN64File(uint32_t vrom, uint8_t* out, uint32_t size) {
    int index = FindArchiveFile(vrom);
    const n64heap::ArchiveFile& file = n64heap::kArchiveFiles[index];
    if (vrom < file.vrom || vrom + size > file.vrom + file.size) {
        return false;
    }
    if (!sArchiveLoaded[index]) {
        sArchiveLoaded[index] = true;
        auto blob = std::dynamic_pointer_cast<Ship::Blob>(
            Ship::Context::GetRawInstance()->GetResourceManager()->LoadResource(file.path));
        // The blob loader pads the data with zeros
        if (blob == nullptr || blob->Data.size() < file.size) {
            SPDLOG_WARN("[N64Heap] {} is missing or too small ({:#x} bytes, expected {:#x})", file.path,
                        blob != nullptr ? blob->Data.size() : size_t(0), size_t(file.size));
        } else {
            sArchiveData[index] = blob;
        }
    }
    if (sArchiveData[index] == nullptr) {
        return false;
    }
    memcpy(out, sArchiveData[index]->Data.data() + (vrom - file.vrom), size);
    return true;
}

void OnSceneInit(int16_t sceneNum) {
    sObjectSpace.SceneInit(sceneNum, gSaveContext.sceneLayer);
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

// pauseCtx.state values while KaleidoScope loads and shows the pause and game over screens
bool InPauseScreen(uint16_t state) {
    return state >= 3 && state <= 7;
}

bool InGameOverScreen(uint16_t state) {
    return state >= 10 && state <= 17;
}

void SyncObjectSpace() {
    ObjectContext& objects = gPlayState->objectCtx;
    int16_t ids[OBJECT_EXCHANGE_BANK_MAX];
    int count = objects.num < OBJECT_EXCHANGE_BANK_MAX ? objects.num : OBJECT_EXCHANGE_BANK_MAX;
    for (int i = 0; i < count; i++) {
        ids[i] = objects.status[i].id;
    }
    sObjectSpace.SyncObjects(ids, count);

    // The pause and game over screens load their data over the object space, which is reloaded on close
    uint16_t state = gPlayState->pauseCtx.state;
    if (state != sLastPauseState) {
        bool japanese = gSaveContext.language == LANGUAGE_JPN;
        if (InPauseScreen(state) && !InPauseScreen(sLastPauseState)) {
            sObjectSpace.PauseOpened(gSaveContext.linkAge, japanese, gSaveContext.worldMapArea);
            sVerdictDirty = true;
        } else if (InGameOverScreen(state) && !InGameOverScreen(sLastPauseState)) {
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
        return; // The N64 never clears the pointer
    }
    uint32_t value = 0;
    std::string what;
    bool resolved;
    if (memcmp(script, "__OTR__", 7) == 0) {
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

bool InActorHeap(uint32_t address) {
    return address >= n64heap::kZeldaArenaStart && address < n64heap::kZeldaArenaStart + Shadow().ArenaSize();
}

n64heap::ActorMemory RebuildActor(const Actor* actor) {
    return n64heap::ActorMemory(actor, n64heap::kActors[actor->id].instanceSize,
                                [](const void* host) { return Shadow().AddressOf(host); });
}

const n64heap::ActorMemory& ActorMemoryFor(const Actor* actor) {
    if (sActorMemory.actor != actor || sActorMemory.frame != gPlayState->gameplayFrames || !sActorMemory.memory) {
        sActorMemory.actor = actor;
        sActorMemory.frame = gPlayState->gameplayFrames;
        sActorMemory.memory = std::make_unique<n64heap::ActorMemory>(RebuildActor(actor));
    }
    return *sActorMemory.memory;
}

// Reads N64 memory from the live actors in the shadow heap or from the object space model
bool ReadN64Word(uint32_t address, uint32_t* value, std::string* source) {
    if (!InActorHeap(address)) {
        return sObjectSpace.ReadWord(address, ReadN64File, value, source);
    }
    uint32_t instance = 0;
    const Actor* actor = static_cast<const Actor*>(Shadow().FindActorAt(address, &instance));
    if (actor == nullptr) {
        *source = "actor heap: " + Shadow().DescribeAddress(address);
        return Shadow().ReadLeftover(address, value);
    }
    char text[64];
    snprintf(text, sizeof(text), "actor 0x%03X instance at %08X +0x%X", actor->id, instance, address - instance);
    *source = text;
    if (!ActorMemoryFor(actor).ReadWord(address - instance, value)) {
        *source += " (not modelled)";
        return false;
    }
    return true;
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
            return "runs cutscene commands (" + sim.detail + ")";
        default:
            return "parser reaches data that is not modelled (" + sim.detail + ")";
    }
}

void UpdateVerdict() {
    if (!sVerdictDirty || sCutscenePointer == 0) {
        return;
    }
    sVerdictDirty = false;
    n64heap::ScriptSimulation sim = n64heap::SimulateCutscene(sCutscenePointer, ReadN64Word);
    std::string line = sim.headerKnown ? sim.source + ": header (" + std::to_string(sim.totalEntries) + ", " +
                                             std::to_string(sim.frameCount) + ") -> " + DescribeOutcome(sim)
                                       : sim.source + ": unknown (data is not deterministic or not modelled)";
    if (line != sLastVerdict) {
        sLastVerdict = line;
        SPDLOG_INFO("[N64Heap] data at {:#010x}: {}", sCutscenePointer, line);
    }
}

void OnGameFrameUpdate() {
    ReportStats();
    if (gPlayState == nullptr) {
        return;
    }
    SyncObjectSpace();
    UpdatePointer();
    UpdateVerdict();
}

void RegisterN64Heap() {
    GameInteractor::Instance->RegisterGameHook<GameInteractor::OnSceneInit>(OnSceneInit);
    GameInteractor::Instance->RegisterGameHook<GameInteractor::OnGameFrameUpdate>(OnGameFrameUpdate);
}

RegisterShipInitFunc sInitFunc(RegisterN64Heap);

} // namespace

extern "C" void N64Heap_OnArenaInit(void) {
    const n64heap::SceneLayout* layout = n64heap::FindSceneLayout(gPlayState->sceneNum, gSaveContext.sceneLayer);
    if (layout == nullptr) {
        SPDLOG_WARN("[N64Heap] no N64 layout for scene {:#x} layer {}", gPlayState->sceneNum, gSaveContext.sceneLayer);
    }
    Shadow().ArenaInit(layout != nullptr ? layout->zeldaArenaSize : 0, gSaveContext.nayrusLoveTimer == 2000);
}

extern "C" void N64Heap_OnAlloc(void* ptr, size_t size, const char* file, int reverse) {
    if (ptr != nullptr) {
        Shadow().OnAlloc(ptr, size, file, reverse != 0);
    }
}

extern "C" void N64Heap_OnFree(void* ptr) {
    if (ptr == nullptr) {
        return;
    }
    uint32_t address;
    int16_t actorId;
    if (Shadow().FindActor(ptr, &address, &actorId)) {
        n64heap::ActorMemory memory = RebuildActor(static_cast<const Actor*>(ptr));
        Shadow().RecordLeftover(address, memory.Bytes(), memory.Known(), actorId);
    }
    if (sActorMemory.actor == ptr) {
        sActorMemory = ActorMemoryCache{};
    }
    Shadow().OnFree(ptr);
}

extern "C" int N64Heap_ActorSpawn(int16_t actorId) {
    if (Shadow().ActorSpawn(actorId)) {
        return 1;
    }
    SPDLOG_INFO("[N64Heap] actor {:#x} does not fit in the N64 heap (largest free block {:#x})", actorId,
                Shadow().LargestFree());
    if (!Enabled()) {
        return 1;
    }
    Shadow().AbortSpawn();
    return 0;
}

extern "C" int N64Heap_EffectSpawn(int32_t type) {
    if (Shadow().EffectSpawn(type)) {
        return 1;
    }
    SPDLOG_INFO("[N64Heap] effect {:#x} overlay does not fit in the N64 heap (largest free block {:#x})", type,
                Shadow().LargestFree());
    return Enabled() ? 0 : 1;
}

extern "C" int N64Heap_HasN64Data(void) {
    return ResourceMgr_FileExists(n64heap::kArchiveFiles[0].path) ? 1 : 0;
}

extern "C" void* N64Heap_FilterCutsceneScript(void* script) {
    if (script == nullptr || gPlayState == nullptr || !Enabled()) {
        return script;
    }
    // The pointer may have been set earlier this frame
    UpdatePointer();
    if (sCutscenePointer == 0) {
        return script;
    }
    if (script == sDecision.script && sCutscenePointer == sDecision.pointer &&
        sObjectSpace.Version() == sDecision.memoryVersion) {
        return sDecision.result;
    }
    sDecision.script = script;
    sDecision.pointer = sCutscenePointer;
    sDecision.memoryVersion = sObjectSpace.Version();
    sDecision.result = script;

    n64heap::ScriptSimulation sim = n64heap::SimulateCutscene(sCutscenePointer, ReadN64Word);
    int32_t hostHeader[2];
    memcpy(hostHeader, script, sizeof(hostHeader));
    if (sim.outcome != n64heap::ScriptSimulation::Outcome::Unknown && sim.totalEntries == hostHeader[0] &&
        sim.frameCount == hostHeader[1]) {
        return script;
    }
    // Replacements that run no commands are a header (entry count, frame count) followed by the end of script command
    std::vector<int32_t>& replacement = sDecision.replacement;
    using Outcome = n64heap::ScriptSimulation::Outcome;
    switch (sim.outcome) {
        case Outcome::EndsImmediately:
            replacement = { sim.totalEntries, sim.frameCount, n64heap::kCsCmdEndOfScript, 0 };
            break;
        case Outcome::NoCommands:
            replacement = { 0, sim.frameCount, n64heap::kCsCmdEndOfScript, 0 };
            break;
        case Outcome::Hang:
            // The N64 locks up in the parser, so play an endless cutscene that does nothing instead
            replacement = { 0, INT32_MAX, n64heap::kCsCmdEndOfScript, 0 };
            break;
        case Outcome::RunsCommands:
            replacement = sim.script;
            break;
        default:
            SPDLOG_INFO("[N64Heap] cutscene pointer {:#010x} reaches data that is not modelled ({}); SoH runs its own "
                        "copy",
                        sCutscenePointer, sim.detail);
            return script;
    }
    sDecision.result = replacement.data();
    SPDLOG_INFO("[N64Heap] running N64 cutscene data at {:#010x} ({}): header ({}, {}) -> {}{}", sCutscenePointer,
                sim.source, sim.totalEntries, sim.frameCount, DescribeOutcome(sim),
                sim.outcome == Outcome::Hang ? " (emulated as an endless cutscene)" : "");
    return sDecision.result;
}
