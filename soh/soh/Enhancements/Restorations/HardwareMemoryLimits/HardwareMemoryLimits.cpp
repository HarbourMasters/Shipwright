#include <libultraship/bridge/consolevariablebridge.h>

#include <spdlog/fmt/fmt.h>
#include <string>

#include "soh/Enhancements/game-interactor/GameInteractor.h"
#include "soh/ShipInit.hpp"
#include "soh/ActorDB.h"

extern "C" {
#include "HardwareMemoryLimits.hpp"
#include "N64SizeData.hpp"
#include "n64_shadow_arena.h"
#include "n64_arena_sizing.h"
#include "global.h"
}

#define CVAR_NAME CVAR_ENHANCEMENT("HardwareMemoryLimits")
#define CVAR_DEFAULT 0
#define CVAR_VALUE CVarGetInteger(CVAR_NAME, CVAR_DEFAULT)

// --------------------------------------------------------------------------------------------------------------------
// State
// --------------------------------------------------------------------------------------------------------------------

static s32 sIsActive = 0;
static ShadowArena sShadow;
static u32 sSohThaRemainder = 0;
static u8 sElfMsgNum = 0;

// Shadow offsets for actor overlays, keyed by actor ID.  SHADOW_NULL means no shadow allocation exists for that type.
static u32 sOverlayShadows[ACTOR_ID_MAX];

// Shadow offsets for effect overlays, keyed by effect type.
static u32 sEffectOverlayShadows[EFFECT_SS_TYPE_MAX];

// Shadow offset for the shared absolute-space overlay buffer.
static u32 sAbsoluteSpaceShadow = SHADOW_NULL;

// Maps real pointers (instances and subsidiaries) to their shadow offsets.
static std::unordered_map<void*, u32> sShadowMap;

// Maps real actor pointers to their original actor ID (before enemy randomizer substitution).  Used by FreeOverlay to
// free the correct overlay shadow entry.  Only populated for randomized actors.
static std::unordered_map<void*, s16> sActorOriginalIds;

// Block metadata for the heap viewer.  Keyed by shadow DATA offset (node offset + arena->nodeSize).
// Populated in each Alloc function, erased in each Free, cleared in N64Mem_Reset.
struct BlockMeta {
    u8 type;
    s16 actorId;
};

static std::unordered_map<u32, BlockMeta> sBlockMetaMap;

// When the enemy randomizer replaces an actor, this holds the ORIGINAL actor ID so that shadow allocations are charged
// at the original N64 size rather than the replacement's size.  Set by N64Mem_SetOriginalActorId before Actor_Spawn,
// cleared by N64Mem_ClearOriginalActorId after Actor_Spawn returns.  -1 means no override (use the passed actorId).
static s16 sOriginalActorId = -1;

// Set when AllocInstance consumes a non-negative sOriginalActorId, cleared after Actor_Init returns.  While set, ALL
// shadow allocations (overlay, instance, subsidiary) from child actors spawned during the replacement actor's init are
// skipped -- on N64 these children don't exist because the original actor never spawned them.
static s32 sInsideRandomizedInit = 0;

// Returns the actor ID to use for size lookups.  If an original actor ID override is set (enemy randomizer active),
// returns that; otherwise returns the passed actorId unchanged.
static u16 ResolveSizeActorId(s16 actorId) {
    return sOriginalActorId >= 0 ? static_cast<u16>(sOriginalActorId) : static_cast<u16>(actorId);
}

// --------------------------------------------------------------------------------------------------------------------
// Diagnostics
// --------------------------------------------------------------------------------------------------------------------

static s32 sTraceEnabled = 0;

static void LogShadowState(const char* context) {
    u32 maxFree = 0;
    u32 totalFree = 0;
    u32 totalAlloc = 0;

    ShadowArena_GetSizes(&sShadow, &maxFree, &totalFree, &totalAlloc);
    SPDLOG_INFO("[HardwareMemoryLimits] ({}): alloc=0x{:X}, free=0x{:X}, largest=0x{:X}", context, totalAlloc,
                totalFree,
                maxFree);
}

static void TraceAlloc(const char* tag, u32 id, u32 size) {
    if (sTraceEnabled) {
        u32 consumed = (size + 0xF & ~0xF) + sShadow.nodeSize;
        SPDLOG_TRACE("[N64Trace] +{} id=0x{:X} sz=0x{:X} cost=0x{:X}", tag, id, size, consumed);
    }
}

static void TraceFree(const char* tag, u32 id) {
    if (sTraceEnabled) {
        SPDLOG_TRACE("[N64Trace] -{} id=0x{:X}", tag, id);
    }
}

// --------------------------------------------------------------------------------------------------------------------
// Graveyard benchmark
// --------------------------------------------------------------------------------------------------------------------

static s32 sGraveyardTransitionCount = 0;

void N64Mem_BenchmarkTransition(PlayState* play) {
    if (!sIsActive || play->sceneNum != SCENE_GRAVEYARD) {
        return;
    }

    sGraveyardTransitionCount++;

    u32 maxFree = 0;
    u32 totalFree = 0;
    u32 totalAlloc = 0;
    ShadowArena_GetSizes(&sShadow, &maxFree, &totalFree, &totalAlloc);

    SPDLOG_INFO("[N64Benchmark] transition={}, largest_free=0x{:X}, total_free=0x{:X}",
                sGraveyardTransitionCount, maxFree, totalFree);
}

// --------------------------------------------------------------------------------------------------------------------
// Lifecycle
// --------------------------------------------------------------------------------------------------------------------

void N64Mem_StoreThaRemainder(u32 sohRemainder) {
    sSohThaRemainder = sohRemainder;
}

void N64Mem_StoreElfMsgNum(u8 num) {
    sElfMsgNum = num;
}

u8 N64Mem_GetElfMsgNum() {
    return sElfMsgNum;
}

void N64Mem_Reset(PlayState* play) {
    // Log shadow state before teardown for per-scene diagnostics.
    if (sIsActive && sShadow.buffer) {
        u32 maxFree = 0;
        u32 totalFree = 0;
        u32 totalAlloc = 0;

        ShadowArena_GetSizes(&sShadow, &maxFree, &totalFree, &totalAlloc);
        SPDLOG_INFO("[HardwareMemoryLimits] Teardown: alloc=0x{:X}, free=0x{:X}, largest=0x{:X}, ptrs={}", totalAlloc,
                    totalFree, maxFree, sShadowMap.size());

        if (sGraveyardTransitionCount > 0) {
            SPDLOG_INFO("[N64Benchmark] RESULT transitions={}", sGraveyardTransitionCount);
        }
    }

    sGraveyardTransitionCount = 0;

    // Tear down previous shadow state unconditionally -- the real ZeldaArena has already been reinitialized by
    // Play_Init.
    ShadowArena_Destroy(&sShadow);
    sShadowMap.clear();
    sActorOriginalIds.clear();
    sBlockMetaMap.clear();
    sAbsoluteSpaceShadow = SHADOW_NULL;
    sOriginalActorId = -1;
    sInsideRandomizedInit = 0;

    for (u32& sOverlayShadow : sOverlayShadows) {
        sOverlayShadow = SHADOW_NULL;
    }

    for (u32& sEffectOverlayShadow : sEffectOverlayShadows) {
        sEffectOverlayShadow = SHADOW_NULL;
    }

    sIsActive = CVAR_VALUE;
    if (sIsActive && play != nullptr) {
        // Compute N64-equivalent arena size from first principles.  All per-version constants are derived from the OTR
        // blob, which was extracted from the user's specific ROM version.
        u32 shadowArenaSize = ArenaSizing_ComputeN64ArenaSize(play);
        if (shadowArenaSize == 0) {
            SPDLOG_ERROR("[HardwareMemoryLimits] Arena sizing returned 0 -- THA budget exceeded, disabling.");
            sIsActive = 0;
            return;
        }

        SPDLOG_INFO("[HardwareMemoryLimits] Shadow arena size=0x{:X} for scene 0x{:X}", shadowArenaSize,
                    play->sceneNum);
        ShadowArena_Init(&sShadow, shadowArenaSize, N64SizeData_GetArenaNodeSize());

        sTraceEnabled = play->sceneNum == SCENE_GRAVEYARD;
    }
}

s32 N64Mem_IsActive() {
    return sIsActive;
}

ShadowArena* N64Mem_GetShadowArena() {
    return &sShadow;
}

void N64Mem_LogState(const char* context) {
    if (sIsActive) {
        LogShadowState(context);
    }
}

void N64Mem_DumpArena(const char* tag) {
    if (!sIsActive || sShadow.buffer == nullptr) {
        return;
    }

    std::string out = fmt::format("\n[N64HeapDump] === {} ===\n", tag);

    u32 offset = ShadowArena_GetHead(&sShadow);
    u32 freeCount = 0;
    u32 allocCount = 0;
    u32 freeTotal = 0;
    u32 allocTotal = 0;

    while (offset != SHADOW_NULL) {
        s32 isFree = 0;
        u32 size = 0;
        u32 next = 0;
        if (!ShadowArena_GetNodeInfo(&sShadow, offset, &isFree, &size, &next)) {
            out += fmt::format("  +0x{:06X} <invalid node>\n", offset);
            break;
        }

        const u32 dataOff = offset + sShadow.nodeSize;
        if (isFree) {
            out += fmt::format("  +0x{:06X} 0x{:06X} FREE\n", offset, size);
            freeTotal += size;
            freeCount++;
        } else {
            u8 type = 0;
            s16 actorId = -1;
            const char* typeName = "???";
            if (N64Mem_GetBlockInfo(dataOff, &type, &actorId)) {
                switch (type) {
                    case N64MEM_BLOCK_INSTANCE:
                        typeName = "inst";
                        break;
                    case N64MEM_BLOCK_OVERLAY:
                        typeName = "ovl ";
                        break;
                    case N64MEM_BLOCK_SUBSIDIARY:
                        typeName = "sub ";
                        break;
                    case N64MEM_BLOCK_EFFECT:
                        typeName = "efx ";
                        break;
                    case N64MEM_BLOCK_ABSOLUTE:
                        typeName = "abs ";
                        break;
                    default:
                        break;
                }
            }

            if (actorId >= 0) {
                out += fmt::format("  +0x{:06X} 0x{:06X} {} actor=0x{:04X}\n", offset, size, typeName,
                                   static_cast<u16>(actorId));
            } else {
                out += fmt::format("  +0x{:06X} 0x{:06X} {}\n", offset, size, typeName);
            }
            allocTotal += size;
            allocCount++;
        }

        offset = next;
    }

    out += fmt::format("[N64HeapDump] {} blocks: {} alloc (0x{:X}B), {} free (0x{:X}B)", freeCount + allocCount,
                       allocCount, allocTotal, freeCount, freeTotal);

    SPDLOG_INFO("{}", out);
}

void N64Mem_SetOriginalActorId(s16 actorId) {
    sOriginalActorId = actorId;
}

void N64Mem_ClearOriginalActorId() {
    sOriginalActorId = -1;
}

s32 N64Mem_GetRandomizedInit() {
    return sInsideRandomizedInit;
}

void N64Mem_SetRandomizedInit(s32 value) {
    sInsideRandomizedInit = value;
}

// --------------------------------------------------------------------------------------------------------------------
// Actor overlays
// --------------------------------------------------------------------------------------------------------------------

s32 N64Mem_AllocOverlay(s16 actorId, u16 allocType) {
    if (!sIsActive) {
        return 1;
    }

    // Child actors spawned during a randomized enemy's init don't exist on N64 -- skip shadow tracking.
    if (sInsideRandomizedInit) {
        return 1;
    }

    if (actorId < 0 || actorId >= ACTOR_ID_MAX) {
        return 1;
    }

    const u32 overlaySize = N64SizeData_GetActorOverlaySize(ResolveSizeActorId(actorId));
    if (overlaySize == 0) {
        return 1;
    }

    // When the enemy randomizer is active, use the original actor's allocType so the shadow takes the same path
    // N64 would have taken for the unsubstituted actor (i.e., a regular enemy replaced by an ABSOLUTE mini-boss
    // should still shadow as ALLOCTYPE_NORMAL).
    if (sOriginalActorId >= 0) {
        const auto& entry = ActorDB::Instance->RetrieveEntry(sOriginalActorId);
        allocType = entry.entry.allocType;
    }

    // ABSOLUTE: Shared fixed-size buffer, allocated once via MallocR.
    if (allocType & ALLOCTYPE_ABSOLUTE) {
        if (sAbsoluteSpaceShadow == SHADOW_NULL) {
            sAbsoluteSpaceShadow = ShadowArena_MallocR(&sShadow, AM_FIELD_SIZE);
            if (sAbsoluteSpaceShadow == SHADOW_NULL) {
                SPDLOG_ERROR("[HardwareMemoryLimits] Shadow absolute space failed (need 0x{:X})", AM_FIELD_SIZE);
                return 0;
            }
            sBlockMetaMap[sAbsoluteSpaceShadow] = { N64MEM_BLOCK_ABSOLUTE, -1 };
        }

        return 1;
    }

    // Resolve the actor ID for overlay tracking.  When the enemy randomizer is active, overlays are tracked by
    // the ORIGINAL actor ID so that multiple replacements of the same original type share one overlay shadow.
    const u16 overlayTrackId = ResolveSizeActorId(actorId);

    // Already shadowed for this type.
    if (sOverlayShadows[overlayTrackId] != SHADOW_NULL) {
        return 1;
    }

    // Match Actor_LoadOverlay's branching: PERSISTENT/PERMANENT overlays go through MallocR (arena top), default
    // (NORMAL) overlays go through forward Malloc (arena bottom).  ABSOLUTE is handled above.  NORMAL overlays being
    // interleaved with instances at the bottom is the authentic N64 fragmentation pattern -- the simulation must
    // reproduce it, not paper over it.
    const u32 shadow = (allocType & ALLOCTYPE_PERMANENT)
                           ? ShadowArena_MallocR(&sShadow, overlaySize)
                           : ShadowArena_Malloc(&sShadow, overlaySize);
    if (shadow == SHADOW_NULL) {
        SPDLOG_ERROR("[HardwareMemoryLimits] Shadow overlay failed for actor 0x{:04X} (need 0x{:X})", actorId,
                     overlaySize);
        return 0;
    }

    sOverlayShadows[overlayTrackId] = shadow;
    sBlockMetaMap[shadow] = { N64MEM_BLOCK_OVERLAY, static_cast<s16>(overlayTrackId) };
    TraceAlloc("ovl", overlayTrackId, overlaySize);
    return 1;
}

void N64Mem_FreeOverlay(s16 actorId, u16 allocType) {
    if (!sIsActive) {
        return;
    }

    if (actorId < 0 || actorId >= ACTOR_ID_MAX) {
        return;
    }

    // PERMANENT: Overlays that are never freed.
    if (allocType & ALLOCTYPE_PERMANENT) {
        return;
    }

    sBlockMetaMap.erase(sOverlayShadows[actorId]);
    ShadowArena_Free(&sShadow, sOverlayShadows[actorId]);
    sOverlayShadows[actorId] = SHADOW_NULL;
    TraceFree("ovl", actorId);
}

// --------------------------------------------------------------------------------------------------------------------
// Actor instances
// --------------------------------------------------------------------------------------------------------------------

s32 N64Mem_AllocInstance(s16 actorId, s16 params, void* realPtr) {
    if (!sIsActive) {
        sOriginalActorId = -1;
        return 1;
    }

    // Child actors spawned during a randomized enemy's init don't exist on N64 -- skip shadow tracking.
    if (sInsideRandomizedInit) {
        return 1;
    }

    const u16 sizeId = ResolveSizeActorId(actorId);

    // Consume the override and enter the randomized-init phase.  Subsidiaries allocated during Actor_Init will be
    // skipped (they belong to the replacement, not the original).  Child Actor_Spawn calls during init will also
    // be skipped via the sInsideRandomizedInit check above.
    if (sOriginalActorId >= 0) {
        sOriginalActorId = -1;
        sInsideRandomizedInit = 1;
    }

    if (actorId < 0 || actorId >= ACTOR_ID_MAX) {
        return 1;
    }

    const u32 instanceSize = N64SizeData_GetActorInstanceSize(sizeId);
    if (instanceSize == 0) {
        return 1;
    }

    if (instanceSize == 0x1A0) {
        SPDLOG_INFO("[HardwareMemoryLimits] 0x1A0 instance: actorId=0x{:X}", actorId);
    }

    const u32 shadow = ShadowArena_Malloc(&sShadow, instanceSize);
    if (shadow == SHADOW_NULL) {
        SPDLOG_ERROR("[HardwareMemoryLimits] Shadow instance failed for actor 0x{:04X} params=0x{:04X} (need 0x{:X})",
                     actorId, static_cast<u16>(params), instanceSize);
        N64Mem_DumpArena(fmt::format("instance failure: actor=0x{:04X} params=0x{:04X}", actorId,
                                     static_cast<u16>(params))
            .c_str());
        return 0;
    }

    sShadowMap[realPtr] = shadow;
    sActorOriginalIds[realPtr] = static_cast<s16>(sizeId);
    sBlockMetaMap[shadow] = { N64MEM_BLOCK_INSTANCE, static_cast<s16>(sizeId) };
    TraceAlloc("inst", actorId, instanceSize);
    return 1;
}

s16 N64Mem_FreeInstance(void* realPtr) {
    if (!sIsActive || !realPtr) {
        return -1;
    }

    const auto i = sShadowMap.find(realPtr);
    if (i == sShadowMap.end()) {
        return -1;
    }

    // Retrieve the stored original actor ID before erasing.  Used by Actor_Delete to free the correct overlay shadow.
    s16 originalId = -1;
    if (const auto j = sActorOriginalIds.find(realPtr); j != sActorOriginalIds.end()) {
        originalId = j->second;
        sActorOriginalIds.erase(j);
    }

    if (sTraceEnabled) {
        const auto* actor = static_cast<Actor*>(realPtr);
        TraceFree("inst", actor->id);
    }

    sBlockMetaMap.erase(i->second);
    ShadowArena_Free(&sShadow, i->second);
    sShadowMap.erase(i);
    return originalId;
}

// --------------------------------------------------------------------------------------------------------------------
// Subsidiaries
// --------------------------------------------------------------------------------------------------------------------

s32 N64Mem_AllocSubsidiary(void* realPtr, u32 n64Size) {
    if (!sIsActive) {
        return 1;
    }

    // When inside a randomized enemy's init, the replacement actor's subsidiaries (colliders, skeleton tables, skin
    // buffers) have different counts than the original's.  Rather than charge the wrong sizes, skip subsidiary
    // tracking entirely -- instance and overlay sizes from the original are already correct and dominate heap pressure.
    if (sInsideRandomizedInit) {
        return 1;
    }

    const u32 shadow = ShadowArena_Malloc(&sShadow, n64Size);
    if (shadow == SHADOW_NULL) {
        SPDLOG_ERROR("[HardwareMemoryLimits] Shadow subsidiary failed (need 0x{:X})", n64Size);
        return 0;
    }

    sShadowMap[realPtr] = shadow;
    sBlockMetaMap[shadow] = { N64MEM_BLOCK_SUBSIDIARY, -1 };
    TraceAlloc("sub", 0, n64Size);
    return 1;
}

void N64Mem_FreeSubsidiary(void* realPtr) {
    if (!sIsActive || !realPtr) {
        return;
    }

    const auto i = sShadowMap.find(realPtr);
    if (i == sShadowMap.end()) {
        return;
    }

    sBlockMetaMap.erase(i->second);
    ShadowArena_Free(&sShadow, i->second);
    sShadowMap.erase(i);
}


// --------------------------------------------------------------------------------------------------------------------
// Effect overlays
// --------------------------------------------------------------------------------------------------------------------

s32 N64Mem_AllocEffectOverlay(s32 type) {
    if (!sIsActive) {
        return 1;
    }

    if (type < 0 || type >= EFFECT_SS_TYPE_MAX) {
        return 1;
    }

    if (sEffectOverlayShadows[type] != SHADOW_NULL) {
        return 1;
    }

    u32 overlaySize = N64SizeData_GetEffectOverlaySize(type);
    if (overlaySize == 0) {
        return 1;
    }

    const u32 shadow = ShadowArena_MallocR(&sShadow, overlaySize);
    if (shadow == SHADOW_NULL) {
        SPDLOG_ERROR("[HardwareMemoryLimits] Shadow effect overlay failed for type 0x{:02X} (need 0x{:X})", type,
                     overlaySize);
        return 0;
    }

    sEffectOverlayShadows[type] = shadow;
    sBlockMetaMap[shadow] = { N64MEM_BLOCK_EFFECT, static_cast<s16>(type) };
    TraceAlloc("efx", type, overlaySize);
    return 1;
}


// --------------------------------------------------------------------------------------------------------------------
// Heap viewer metadata
// --------------------------------------------------------------------------------------------------------------------

s32 N64Mem_GetBlockInfo(u32 dataOffset, u8* outType, s16* outActorId) {
    const auto i = sBlockMetaMap.find(dataOffset);
    if (i == sBlockMetaMap.end()) {
        return 0;
    }

    if (outType) {
        *outType = i->second.type;
    }

    if (outActorId) {
        *outActorId = i->second.actorId;
    }

    return 1;
}

// --------------------------------------------------------------------------------------------------------------------
// Registration
// --------------------------------------------------------------------------------------------------------------------

void RegisterN64MemoryModel() {
    // #TODO: Shadow arena initialization
}

static RegisterShipInitFunc initFunc(RegisterN64MemoryModel, { CVAR_NAME });