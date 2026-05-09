#include "arena_sizing.h"
#include "N64SizeData.hpp"
#include "soh/Enhancements/Restorations/N64MemoryModel/N64MemoryModel.hpp"

#include "global.h"

// Declared in z_bgcheck.c but not exposed via header.
s32 BgCheck_IsSpotScene(PlayState* play);
s32 BgCheck_TryGetCustomMemsize(s32 sceneId, u32* memSize);

// --------------------------------------------------------------------------------------------------------------------
// N64 THA budget
//
// GameState_Realloc is called with this value in Play_Init when the memory model CVar is active.  This is the total
// pool from which every THA consumer draws; whatever remains becomes ZeldaArena.
// --------------------------------------------------------------------------------------------------------------------

#define N64_THA_BUDGET 0x1D4790

// --------------------------------------------------------------------------------------------------------------------
// N64 struct sizes (32-bit, from decomp headers and linker map)
// --------------------------------------------------------------------------------------------------------------------

#define N64_SIZEOF_COLLISION_CONTEXT 0x1464 // CollisionContext size = 0x1464 (from decomp header comment)
#define N64_SIZEOF_GFX 8 // sizeof(Gfx) on N64: Two u32 words -- SoH is 16
#define N64_SIZEOF_VTX 0x10 // sizeof(Vtx) on N64: Same on both platforms

// Struct sizes that are identical on N64 and SoH (no pointer members):
//  sizeof(MtxF)            = 0x40
//  sizeof(GFx)             = 0x08
//  sizeof(Vtx)             = 0x10
//  sizeof(Vec3s)           = 0x06
//  sizeof(SSNode)          = 0x04
//  sizeof(CollisionPoly)   = 0x10
//  sizeof(StaticLookup)    = 0x06

// --------------------------------------------------------------------------------------------------------------------
// Fixed THA consumers (same value on every N64 scene)
// --------------------------------------------------------------------------------------------------------------------

#define N64_MATRIX_STACK_SIZE (20 * 0x40) // sys_matrix.c: 20 * sizeof(MtxF)
#define N64_TEXT_BOX_SIZE 0x2200 // Message_Init: Constant
#define N64_DO_ACTION_SIZE 0x480 // z_construct.c: 3 * DO_ACTION_TEX_SIZE (48x16 IA4 = 0x180 each)
#define N64_ICON_ITEM_SIZE (0x1000 * 4) // z_construct.c: 4 * ITEM_ICON_SIZE (32x32 RGBA32)
#define N64_MAP_SEGMENT_SIZE 0x1000 // z_map_exp.c: DMA target buffer for minimap textures

// --------------------------------------------------------------------------------------------------------------------
// ovl_map_mark_data: N64-unique THA consumer for dungeon map marks
//
// On N64, the map mark data overlay (chest/boss/dungeon icons on the pause map) is loaded into THA
// via GAME_STATE_ALLOC in MapMark_Init.  SoH compiles this data in directly.
// VRAM size from decomp linker map: 0x8085D460 - 0x80856900 = 0x6B60.
// Constant across OoT versions (dungeon map mark positions don't change).
// Only loaded for the 10 main dungeons (Deku Tree through Ice Cavern) and their boss rooms.
// --------------------------------------------------------------------------------------------------------------------

#define N64_MAP_MARK_DATA_VRAM_SIZE 0x6B60

static u32 GetMapMarkDataOverlaySize(PlayState* play)
{
    // Mirrors the condition in z_map_exp.c Map_Init: the dungeon case block's inner guard.
    // Main dungeons: SCENE_DEKU_TREE (0x00) through SCENE_ICE_CAVERN (0x09)
    // Boss rooms:    SCENE_DEKU_TREE_BOSS (0x11) through SCENE_SHADOW_TEMPLE_BOSS (0x18)
    if (play->sceneNum <= SCENE_ICE_CAVERN ||
        (play->sceneNum >= SCENE_DEKU_TREE_BOSS && play->sceneNum <= SCENE_SHADOW_TEMPLE_BOSS))
    {
        return N64_MAP_MARK_DATA_VRAM_SIZE;
    }

    return 0;
}

// --------------------------------------------------------------------------------------------------------------------
// Skybox N64-unique THA consumers
//
// On N64, skybox textures and palettes are DMA'd into THA-allocated staticSegment buffers.  SoH loads from OTR via the
// ResourceManager, never touching THA.
//
//  SKYBOX_NORMAL_SKY:  2 texture banks * 0xC000 * 2 palettes * 0x100
//  Indoor skyboxes:    Varying sizes per skybox type
//  SKYBOX_NONE:        0
//
// #TODO: Other skybox types need the same treatment from decomp map's vr_*_static segments.
// --------------------------------------------------------------------------------------------------------------------

static u32 GetN64SkyboxTextureSize(s16 skyboxId)
{
    switch (skyboxId)
    {
    case SKYBOX_NORMAL_SKY:
    // Fall-through
    case SKYBOX_OVERCAST_SUNSET:
        return 2 * 0xC000 + 2 * 0x100;

    case SKYBOX_NONE:
        return 0;

    default:
        // Indoor skyboxes: Conservative estimate.
        // #TODO: Populate exact sizes per skybox ID from decomp map.
        return 3 * 0x8000 + 3 * 0x200;
    }
}

// --------------------------------------------------------------------------------------------------------------------
// Skybox dListBuf and roomVtx (shared -- same size on N64 and SoH, no pointer members)
// --------------------------------------------------------------------------------------------------------------------

static void GetSkyboxDlistAndVtxSize(s16 skyboxId, u32* outDlistSize, u32* outVtxSize)
{
    if (skyboxId == SKYBOX_NONE)
    {
        *outDlistSize = 0;
        *outVtxSize = 0;
        return;
    }

    // unk_140 != 0 means indoor/single-room skybox.
    // #TODO: Determine unk_140 from ID more precisely.
    const bool isIndoor = skyboxId != SKYBOX_NORMAL_SKY && skyboxId != SKYBOX_OVERCAST_SUNSET && skyboxId !=
        SKYBOX_CUTSCENE_MAP;
    if (isIndoor)
    {
        *outDlistSize = 8 * 150 * N64_SIZEOF_GFX;
        *outVtxSize = 256 * N64_SIZEOF_VTX;
    }
    else if (skyboxId == SKYBOX_CUTSCENE_MAP)
    {
        *outDlistSize = 12 * 150 * N64_SIZEOF_GFX;
        *outVtxSize = 192 * N64_SIZEOF_VTX;
    }
    else
    {
        *outDlistSize = 12 * 150 * N64_SIZEOF_GFX;
        *outVtxSize = 160 * N64_SIZEOF_VTX;
    }
}

// --------------------------------------------------------------------------------------------------------------------
// BgCheck THA Total
//
// The BgCheck system allocates 6 items from THA: lookupTbl, SSNode tbl, polyCheckTbl, dyna polyList, dyna vtxList,
// dyna polyNodes.
//
// By algebra, the total simplifies to: bgcheck_memSize - sizeof(CollisionContext)
//
// This is because the tblMax formula (z_bgcheck.c:1628) is defined as:
//  tblMax = (memSize - overhead) / sizeof(SSNode)
//
// where overhead includes all the other consumers.  When you sum all 6 THA allocations, every term cancels except
// bgcheck_memSize and sizeof(CollisionContext).
// --------------------------------------------------------------------------------------------------------------------

static u32 GetBgCheckMemSize(PlayState* play)
{
    const s16 sceneNum = play->sceneNum;

    if (YREG(15) == 0x10 || YREG(15) == 0x20 || YREG(15) == 0x30 || YREG(15) == 0x40)
    {
        return sceneNum == SCENE_STABLE ? 0x3520 : 0x4E20;
    }

    if (BgCheck_IsSpotScene(play))
    {
        return 0xF000;
    }

    u32 customMemSize = 0;
    if (BgCheck_TryGetCustomMemsize(sceneNum, &customMemSize))
    {
        return customMemSize;
    }

    return 0x1CC00;
}

static u32 GetBgCheckThaTotal(PlayState* play)
{
    return GetBgCheckMemSize(play) - N64_SIZEOF_COLLISION_CONTEXT;
}

// --------------------------------------------------------------------------------------------------------------------
// Object bank size (mirrors z_scene.c Object_InitBlank)
// --------------------------------------------------------------------------------------------------------------------

static u32 GetObjectBankSize(PlayState* play)
{
    const s16 sceneNum = play->sceneNum;
    if (sceneNum == SCENE_GANON_BOSS && gSaveContext.sceneSetupIndex == 4)
    {
        return 1177600;
    }

    if (sceneNum == SCENE_SPIRIT_TEMPLE_BOSS || sceneNum == SCENE_CHAMBER_OF_THE_SAGES ||
        sceneNum == SCENE_GANONDORF_BOSS)
    {
        return 1075200;
    }

    return 1024000;
}

// --------------------------------------------------------------------------------------------------------------------
// Elf message size
//
// On N64, loaded via Play_LoadFile into THA.  SoH loads from OTR via ResourceManager.  Only present if the scene's
// SpecialFiles command has cUpElfMsgNum != 0.  The file name is determined by cUpElfMsgNum (1-indexed).
// --------------------------------------------------------------------------------------------------------------------

static const char* sElfMsgDmaNames[] = {
    "elf_message_field",
    "elf_message_ydan",
};

static u32 GetElfMessageSize(PlayState* play)
{
    if (play->cUpElfMsgs == NULL)
    {
        return 0;
    }

    const u8 elfMsgNum = N64Mem_GetElfMsgNum();
    if (elfMsgNum == 0 || elfMsgNum > ARRAY_COUNT(sElfMsgDmaNames))
    {
        LUSLOG_WARN("[ArenaSizing] elfMsg: cUpElfMsgs non-NULL but elfMsgNum=%d out of range", elfMsgNum);
        return 0;
    }

    return N64SizeData_GetDmaFileSize(sElfMsgDmaNames[elfMsgNum - 1]);
}

// --------------------------------------------------------------------------------------------------------------------
// Room buffer max size (mirrors func_80096FE8 logic)
// --------------------------------------------------------------------------------------------------------------------

static u32 GetMaxRoomSize(PlayState* play)
{
    u32 maxRoomSize = 0;

    for (size_t i = 0; i < play->numRooms; ++i)
    {
        const u32 roomSize = play->roomList[i].vromEnd - play->roomList[i].vromStart;
        if (roomSize > maxRoomSize)
        {
            maxRoomSize = roomSize;
        }
    }

    if (play->transiActorCtx.numActors != 0)
    {
        const TransitionActorEntry* transitionActor = &play->transiActorCtx.list[0];

        for (size_t j = 0; j < play->transiActorCtx.numActors; ++j)
        {
            const s8 frontRoom = transitionActor->sides[0].room;
            const s8 backRoom = transitionActor->sides[1].room;
            const size_t frontSize = frontRoom < 0
                                         ? 0
                                         : play->roomList[frontRoom].vromEnd - play->roomList[frontRoom].vromStart;
            const u32 backSize = backRoom < 0
                                     ? 0
                                     : play->roomList[backRoom].vromEnd - play->roomList[backRoom].vromStart;

            const u32 cumulSize = frontRoom != backRoom ? frontSize + backSize : frontSize;
            if (cumulSize > maxRoomSize)
            {
                maxRoomSize = cumulSize;
            }

            transitionActor++;
        }
    }

    return maxRoomSize;
}

// --------------------------------------------------------------------------------------------------------------------
// Main computation
// --------------------------------------------------------------------------------------------------------------------

const VersionConstants gVersionConstantsNtsc12 = {
    0x26740, // kaleidoOverlayVramSize: max(kaleido_scope = 0x1CA00, player_actor = 0x26740)
    0x3B00, // parameterStaticSize
    0x60, // effectSsSize: N64 sizeof(EffectSs)
};

u32 ArenaSizing_ComputeN64ArenaSize(PlayState* play, const VersionConstants* vc)
{
    // Each THA consumer is allocated via GAME_STATE_ALLOC -> THA_AllocTailAlign16, which consumes ALIGN16(size) bytes.
    // We must align each consumer individually before summing; aligning the sum would under-count when individual
    // sizes are not 16-byte aligned (common for DMA file sizes).  BgCheck is the exception -- its internal allocations
    // use mixed alignment, but the tblMax computation absorbs the internal waste, so the simplified total
    // (memSize - sizeof(CollisionContext)) is used as-is.

    u32 total = 0;

    // Per-version constants (N64-unique THA consumers SoH skips)
    total += ALIGN16(vc->kaleidoOverlayVramSize);
    total += ALIGN16(vc->parameterStaticSize);

    // Fixed consumers
    total += ALIGN16(N64_MATRIX_STACK_SIZE);
    total += ALIGN16(0x55 * vc->effectSsSize);
    total += ALIGN16(N64_TEXT_BOX_SIZE);
    total += ALIGN16(N64_DO_ACTION_SIZE);
    total += ALIGN16(N64_ICON_ITEM_SIZE);
    total += ALIGN16(N64_MAP_SEGMENT_SIZE);

    const u32 fixed = total;
    LUSLOG_INFO("[ArenaSizing] fixed=0x%X (kaleido=0x%X, param=0x%X)", fixed, vc->kaleidoOverlayVramSize,
                vc->parameterStaticSize);

    // Scene-dependent consumers
    {
        const u32 objBank = GetObjectBankSize(play);
        const u32 sceneFile = N64SizeData_GetDmaFileSize(play->loadedScene->sceneFile.fileName);
        const u32 roomBuf = GetMaxRoomSize(play);
        total += ALIGN16(objBank);
        total += ALIGN16(sceneFile);
        total += ALIGN16(roomBuf);
        LUSLOG_INFO("[ArenaSizing] objBank=0x%X, sceneFile=0x%X, roomBuf=0x%x", (u32)objBank, (u32)sceneFile,
                    (u32)roomBuf);
    }

    // ----------------------------------------------------------------------------------------------------------------
    // Skybox
    //
    // On N64 these are 3-5 separate GAME_STATE_ALLOC calls (tex0, tex1, palette, dList, vtx), each independently
    // aligned.  GetN64SkyboxTextureSize returns the combined raw size of the texture + palette allocations.
    // For NORMAL_SKY this is 2×0xC000 + 2×0x100, all already 16-aligned, so ALIGN16 is a no-op here.  The dList and
    // vtx are separate allocations.
    // ----------------------------------------------------------------------------------------------------------------
    {
        u32 dListSize = 0;
        u32 vtxSize = 0;
        u32 texSize = 0;
        GetSkyboxDlistAndVtxSize(play->skyboxId, &dListSize, &vtxSize);
        texSize = GetN64SkyboxTextureSize(play->skyboxId);
        total += ALIGN16(dListSize);
        total += ALIGN16(vtxSize);
        total += ALIGN16(texSize);
        LUSLOG_INFO("[ArenaSizing] skyboxId=%d, dList=0x%X, vtx=0x%X, tex=0x%X", play->skyboxId, dListSize, vtxSize,
                    texSize);
    }

    // ----------------------------------------------------------------------------------------------------------------
    // BgCheck
    //
    // Uses the algebraic simplification (memSize - sizeof(CollisionContext)).  BgCheck's internal allocations use
    // THA_AllocTailAlign with 2-byte alignment, and the tblMax formula absorbs internal alignment waste.  No separate
    // ALIGN16 needed here.
    // ----------------------------------------------------------------------------------------------------------------
    {
        const u32 bgCheck = GetBgCheckThaTotal(play);
        total += bgCheck;
        LUSLOG_INFO("[ArenaSizing] bgCheck=0x%X (memSize=0x%X)", bgCheck, GetBgCheckMemSize(play));
    }

    // Elf message
    {
        const u32 elfMsg = GetElfMessageSize(play);
        total += ALIGN16(elfMsg);
        LUSLOG_INFO("[ArenaSizing] elfMsg=0x%X", elfMsg);
    }

    // Map mark data overlay (dungeons only)
    {
        const u32 mapMarkData = GetMapMarkDataOverlaySize(play);
        total += ALIGN16(mapMarkData);
    }

    LUSLOG_INFO("[ArenaSizing] total=0x%X, arena=0x%X (budget=0x%X)", total, N64_THA_BUDGET - total,
                (u32)N64_THA_BUDGET);

    if (total >= N64_THA_BUDGET)
    {
        return 0;
    }

    return N64_THA_BUDGET - total;
}
