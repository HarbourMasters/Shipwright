#include <libultraship/bridge/consolevariablebridge.h>

#include "soh/Enhancements/game-interactor/GameInteractor.h"
#include "soh/ShipInit.hpp"

extern "C" {
#include "N64MemoryModel.hpp"
#include "ShadowArena/shadow_arena.h"
#include "ShadowArena/arena_sizing.h"
#include "ShadowArena/actor_overlay_sizes.h"
#include "ShadowArena/effect_overlay_sizes.h"
#include "ShadowArena/instance_sizes.h"
}

#define CVAR_NAME CVAR_ENHANCEMENT("N64MemoryModel")
#define CVAR_DEFAULT 0
#define CVAR_VALUE CVarGetInteger(CVAR_NAME, CVAR_DEFAULT)

// --------------------------------------------------------------------------------------------------------------------
// State
// --------------------------------------------------------------------------------------------------------------------

static s32 sIsActive = 0;
static ShadowArena sShadow;
static u32 sSohThaRemainder = 0;

// Shadow offsets for actor overlays, keyed by actor ID.  SHADOW_NULL means no shadow allocation exists for that type.
static u32 sOverlayShadows[ACTOR_ID_MAX];

// Shadow offsets for effect overlays, keyed by effect type.
static u32 sEffectOverlayShadows[EFFECT_SS_TYPE_MAX];

// Shadow offset for the shared absolute-space overlay buffer.
static u32 sAbsoluteSpaceShadow = SHADOW_NULL;

// Maps real pointers (instances and subsidiaries) to their shadow offsets.
static std::unordered_map<void*, u32> sShadowMap;

// --------------------------------------------------------------------------------------------------------------------
// Diagnostics
// --------------------------------------------------------------------------------------------------------------------

static void LogShadowState(const char* context)
{
    u32 maxFree = 0;
    u32 totalFree = 0;
    u32 totalAlloc = 0;

    ShadowArena_GetSizes(&sShadow, &maxFree, &totalFree, &totalAlloc);
    SPDLOG_INFO("[N64MemoryModel] ({}): alloc=0x{:X}, free=0x{:X}, largest=0x{:X}", context, totalAlloc, totalFree,
                maxFree);
}

// --------------------------------------------------------------------------------------------------------------------
// Lifecycle
// --------------------------------------------------------------------------------------------------------------------

void N64Mem_StoreThaRemainder(u32 sohRemainder)
{
    sSohThaRemainder = sohRemainder;
}

void N64Mem_Reset(PlayState* play)
{
    // Log shadow state before teardown for per-scene diagnostics.
    if (sIsActive && sShadow.buffer)
    {
        u32 maxFree = 0;
        u32 totalFree = 0;
        u32 totalAlloc = 0;

        ShadowArena_GetSizes(&sShadow, &maxFree, &totalFree, &totalAlloc);
        SPDLOG_INFO("[N64MemoryModel] Teardown: alloc=0x{:X}, free=0x{:X}, largest=0x{:X}, ptrs={}", totalAlloc,
                    totalFree, maxFree, sShadowMap.size());
    }

    // Tear down previous shadow state unconditionally -- the real ZeldaArena has already been reinitialized by
    // Play_Init.
    ShadowArena_Destroy(&sShadow);
    sShadowMap.clear();
    sAbsoluteSpaceShadow = SHADOW_NULL;

    for (u32& sOverlayShadow : sOverlayShadows)
    {
        sOverlayShadow = SHADOW_NULL;
    }

    for (u32& sEffectOverlayShadow : sEffectOverlayShadows)
    {
        sEffectOverlayShadow = SHADOW_NULL;
    }

    sIsActive = CVAR_VALUE;
    if (sIsActive && play != nullptr)
    {
        // Compute N64-equivalent arena size from first principles.
        // #TODO: Select version constants based on detected ROM version.
        u32 shadowArenaSize = ArenaSizing_ComputeN64ArenaSize(play, &gVersionConstantsNtsc12);
        if (shadowArenaSize == 0)
        {
            SPDLOG_ERROR("[N64MemoryModel] Arena sizing returned 0 -- THA budget exceeded, disabling.");
            sIsActive = 0;
            return;
        }

        SPDLOG_INFO("[N64MemoryModel] Shadow arena size=0x{:X} for scene 0x{:X}", shadowArenaSize, play->sceneNum);
        ShadowArena_Init(&sShadow, shadowArenaSize);
    }
}

s32 N64Mem_IsActive()
{
    return sIsActive;
}

ShadowArena* N64Mem_GetShadowArena()
{
    return &sShadow;
}

// --------------------------------------------------------------------------------------------------------------------
// Actor overlays
// --------------------------------------------------------------------------------------------------------------------

s32 N64Mem_AllocOverlay(s16 actorId, u16 allocType)
{
    if (!sIsActive)
    {
        return 1;
    }

    if (actorId < 0 || actorId >= ACTOR_ID_MAX)
    {
        return 1;
    }

    const u32 overlaySize = gN64ActorOverlaySizes[actorId];
    if (overlaySize == 0)
    {
        return 1;
    }


    // ABSOLUTE: Shared fixed-size buffer, allocated once via MallocR.
    if (allocType & ALLOCTYPE_ABSOLUTE)
    {
        if (sAbsoluteSpaceShadow == SHADOW_NULL)
        {
            sAbsoluteSpaceShadow = ShadowArena_MallocR(&sShadow, AM_FIELD_SIZE);
            if (sAbsoluteSpaceShadow == SHADOW_NULL)
            {
                SPDLOG_ERROR("[N64MemoryModel] Shadow absolute space failed (need 0x{:X})", AM_FIELD_SIZE);
                return 0;
            }
        }

        return 1;
    }

    // Already shadowed for this type.
    if (sOverlayShadows[actorId] != SHADOW_NULL)
    {
        return 1;
    }

    u32 shadow = SHADOW_NULL;
    if (allocType & ALLOCTYPE_PERMANENT)
    {
        shadow = ShadowArena_MallocR(&sShadow, overlaySize);
    }
    else
    {
        shadow = ShadowArena_Malloc(&sShadow, overlaySize);
    }

    if (shadow == SHADOW_NULL)
    {
        SPDLOG_ERROR("[N64MemoryModel] Shadow overlay failed for actor 0x{:04X} (need 0x{:X})", actorId, overlaySize);
        return 0;
    }

    sOverlayShadows[actorId] = shadow;
    return 1;
}

void N64Mem_FreeOverlay(s16 actorId, u16 allocType)
{
    if (!sIsActive)
    {
        return;
    }

    if (actorId < 0 || actorId >= ACTOR_ID_MAX)
    {
        return;
    }

    // PERMANENT: Overlays that are never freed.
    if (allocType & ALLOCTYPE_PERMANENT)
    {
        return;
    }

    ShadowArena_Free(&sShadow, sOverlayShadows[actorId]);
    sOverlayShadows[actorId] = SHADOW_NULL;
}

// --------------------------------------------------------------------------------------------------------------------
// Actor instances
// --------------------------------------------------------------------------------------------------------------------

s32 N64Mem_AllocInstance(s16 actorId, void* realPtr)
{
    if (!sIsActive)
    {
        return 1;
    }

    if (actorId < 0 || actorId >= ACTOR_ID_MAX)
    {
        return 1;
    }

    const u32 instanceSize = gN64InstanceSizes[actorId];
    if (instanceSize == 0)
    {
        return 1;
    }

    const u32 shadow = ShadowArena_Malloc(&sShadow, instanceSize);
    if (shadow == SHADOW_NULL)
    {
        SPDLOG_ERROR("[N64MemoryModel] Shadow instance failed for actor 0x{:04X} (need 0x{:X})", actorId, instanceSize);
        return 0;
    }

    sShadowMap[realPtr] = shadow;
    return 1;
}

void N64Mem_FreeInstance(void* realPtr)
{
    if (!sIsActive || !realPtr)
    {
        return;
    }

    const auto i = sShadowMap.find(realPtr);
    if (i == sShadowMap.end())
    {
        return;
    }

    ShadowArena_Free(&sShadow, i->second);
    sShadowMap.erase(i);
}

// --------------------------------------------------------------------------------------------------------------------
// Subsidiaries
// --------------------------------------------------------------------------------------------------------------------

s32 N64Mem_AllocSubsidiary(void* realPtr, u32 n64Size)
{
    if (!sIsActive)
    {
        return 1;
    }

    const u32 shadow = ShadowArena_Malloc(&sShadow, n64Size);
    if (shadow == SHADOW_NULL)
    {
        SPDLOG_ERROR("[N64MemoryModel] Shadow subsidiary failed (need 0x{:X})", n64Size);
        return 0;
    }

    sShadowMap[realPtr] = shadow;
    return 1;
}

void N64Mem_FreeSubsidiary(void* realPtr)
{
    if (!sIsActive || !realPtr)
    {
        return;
    }

    const auto i = sShadowMap.find(realPtr);
    if (i == sShadowMap.end())
    {
        return;
    }

    ShadowArena_Free(&sShadow, i->second);
    sShadowMap.erase(i);
}


// --------------------------------------------------------------------------------------------------------------------
// Effect overlays
// --------------------------------------------------------------------------------------------------------------------

s32 N64Mem_AllocEffectOverlay(s32 type)
{
    if (!sIsActive)
    {
        return 1;
    }

    if (type < 0 || type >= EFFECT_SS_TYPE_MAX)
    {
        return 1;
    }

    if (sEffectOverlayShadows[type] != SHADOW_NULL)
    {
        return 1;
    }

    u32 overlaySize = gN64EffectOverlaySizes[type];
    if (overlaySize == 0)
    {
        return 1;
    }

    const u32 shadow = ShadowArena_MallocR(&sShadow, overlaySize);
    if (shadow == SHADOW_NULL)
    {
        SPDLOG_ERROR("[N64MemoryModel] Shadow effect overlay failed for type 0x{:02X} (need 0x{:X})", type,
                     overlaySize);
        return 0;
    }

    sEffectOverlayShadows[type] = shadow;
    return 1;
}


// --------------------------------------------------------------------------------------------------------------------
// Registration
// --------------------------------------------------------------------------------------------------------------------

void RegisterN64MemoryModel()
{
    // #TODO: Shadow arena initialization
}

static RegisterShipInitFunc initFunc(RegisterN64MemoryModel, {CVAR_NAME});
