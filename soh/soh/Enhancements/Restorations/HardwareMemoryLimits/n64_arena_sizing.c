#include "n64_arena_sizing.h"
#include "N64SizeData.hpp"
#include "soh/Enhancements/Restorations/HardwareMemoryLimits/HardwareMemoryLimits.hpp"

#include <libultraship/log/luslog.h>

#include "macros.h"
#include "variables.h"

// Declared in z_bgcheck.c but not exposed via header.
s32 BgCheck_IsSpotScene(PlayState* play);
s32 BgCheck_TryGetCustomMemsize(s32 sceneId, uint32_t* memSize);

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
#define N64_SIZEOF_GFX               8      // sizeof(Gfx) on N64: Two uint32_t words -- SoH is 16
#define N64_SIZEOF_VTX               0x10   // sizeof(Vtx) on N64: Same on both platforms
#define N64_SIZEOF_EFFECT_SS         0x60   // sizeof(EffectSs) on N64: No pointer members, constant across all N64 versions



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

#define N64_MATRIX_STACK_SIZE (20 * 0x40)   // sys_matrix.c: 20 * sizeof(MtxF)
#define N64_TEXT_BOX_SIZE      0x2200       // Message_Init: Constant
#define N64_DO_ACTION_SIZE     0x480        // z_construct.c: 3 * DO_ACTION_TEX_SIZE (48x16 IA4 = 0x180 each)
#define N64_ICON_ITEM_SIZE    (0x1000 * 4)  // z_construct.c: 4 * ITEM_ICON_SIZE (32x32 RGBA32)
#define N64_MAP_SEGMENT_SIZE   0x1000       // z_map_exp.c: DMA target buffer for minimap textures

// --------------------------------------------------------------------------------------------------------------------
// ovl_map_mark_data: N64-unique THA consumer for dungeon map marks
//
// On N64, the map mark data overlay (chest/boss/dungeon icons on the pause map) is loaded into THA
// via GAME_STATE_ALLOC in MapMark_Init.  SoH compiles this data in directly.
// VRAM size from decomp linker map: 0x8085D460 - 0x80856900 = 0x6B60.  Constant across OoT versions (dungeon map mark
// positions don't change).  Only loaded for the 10 main dungeons (Deku Tree through Ice Cavern) and their boss rooms.
// --------------------------------------------------------------------------------------------------------------------

#define N64_MAP_MARK_DATA_VRAM_SIZE 0x6B60

static uint32_t GetMapMarkDataOverlaySize(PlayState* play) {
    // Mirrors the condition in z_map_exp.c Map_Init: the dungeon case block's inner guard.
    //  Main dungeons: SCENE_DEKU_TREE (0x00) through SCENE_ICE_CAVERN (0x09)
    //  Boss rooms:    SCENE_DEKU_TREE_BOSS (0x11) through SCENE_SHADOW_TEMPLE_BOSS (0x18)
    if (play->sceneNum <= SCENE_ICE_CAVERN ||
        (play->sceneNum >= SCENE_DEKU_TREE_BOSS && play->sceneNum <= SCENE_SHADOW_TEMPLE_BOSS)) {
        return N64_MAP_MARK_DATA_VRAM_SIZE;
    }

    return 0;
}

// --------------------------------------------------------------------------------------------------------------------
// Skybox N64-unique THA consumers
//
// On N64, skybox textures and palettes are DMA'd into THA-allocated staticSegment buffers.  SoH loads from O2R via the
// ResourceManager, never touching THA.
//
// The allocation pattern depends on the skybox type:
//   SKYBOX_NORMAL_SKY / OVERCAST_SUNSET:   2 texture banks + 2 palettes (all banks are 0xC000/0x100)
//   SKYBOX_CUTSCENE_MAP:                   2 different tex files + 2 palettes
//   Indoor skyboxes:                       1 tex file + 1 palette file (sizes vary per skybox)
//   SKYBOX_NONE:                           Nothing
//
// The dList/vtx buffer pattern depends on drawType:
//   SKYBOX_DRAW_128 (outdoor):                dList=12×150×Gfx, vtx=5×32×Vtx (6×32 for CUTSCENE_MAP)
//   SKYBOX_DRAW_256 (indoor, 3- or 4-face):   dList=8×150×Gfx, vtx=8×32×Vtx
// --------------------------------------------------------------------------------------------------------------------

// DMA file name for a single-file indoor skybox (1 tex + 1 pal).
typedef struct {
    const char* texName;
    const char* palName;
} SkyboxDmaEntry;

// Indexed by skybox ID.  NULL texName means the ID isn't a single-file indoor skybox (handled separately).
static const SkyboxDmaEntry sSkyboxDmaTable[] = {
    [SKYBOX_NONE] = { NULL, NULL },
    [SKYBOX_NORMAL_SKY] = { NULL, NULL }, // gNormalSkyFiles, hardcoded
    [SKYBOX_BAZAAR] = { "vr_SP1a_static", "vr_SP1a_pal_static" },
    [SKYBOX_OVERCAST_SUNSET] = { NULL, NULL }, // same as NORMAL_SKY
    [SKYBOX_MARKET_ADULT] = { "vr_RUVR_static", "vr_RUVR_pal_static" },
    [SKYBOX_CUTSCENE_MAP] = { NULL, NULL }, // Two tex files, handled separately
    [SKYBOX_HOUSE_LINK] = { "vr_LHVR_static", "vr_LHVR_pal_static" },
    [SKYBOX_MARKET_CHILD_DAY] = { "vr_MDVR_static", "vr_MDVR_pal_static" },
    [SKYBOX_MARKET_CHILD_NIGHT] = { "vr_MNVR_static", "vr_MNVR_pal_static" },
    [SKYBOX_HAPPY_MASK_SHOP] = { "vr_FCVR_static", "vr_FCVR_pal_static" },
    [SKYBOX_HOUSE_KNOW_IT_ALL_BROTHERS] = { "vr_KHVR_static", "vr_KHVR_pal_static" },
    [SKYBOX_HOUSE_OF_TWINS] = { "vr_K3VR_static", "vr_K3VR_pal_static" },
    [SKYBOX_STABLES] = { "vr_MLVR_static", "vr_MLVR_pal_static" },
    [SKYBOX_HOUSE_KAKARIKO] = { "vr_KKRVR_static", "vr_KKRVR_pal_static" },
    [SKYBOX_KOKIRI_SHOP] = { "vr_KSVR_static", "vr_KSVR_pal_static" },
    [SKYBOX_GORON_SHOP] = { "vr_GLVR_static", "vr_GLVR_pal_static" },
    [SKYBOX_ZORA_SHOP] = { "vr_ZRVR_static", "vr_ZRVR_pal_static" },
    [SKYBOX_POTION_SHOP_KAKARIKO] = { "vr_DGVR_static", "vr_DGVR_pal_static" },
    [SKYBOX_POTION_SHOP_MARKET] = { "vr_ALVR_static", "vr_ALVR_pal_static" },
    [SKYBOX_BOMBCHU_SHOP] = { "vr_NSVR_static", "vr_NSVR_pal_static" },
    [SKYBOX_HOUSE_RICHARD] = { "vr_IPVR_static", "vr_IPVR_pal_static" },
    [SKYBOX_HOUSE_IMPA] = { "vr_LBVR_static", "vr_LBVR_pal_static" },
    [SKYBOX_TENT] = { "vr_TTVR_static", "vr_TTVR_pal_static" },
    [SKYBOX_HOUSE_MIDO] = { "vr_K4VR_static", "vr_K4VR_pal_static" },
    [SKYBOX_HOUSE_SARIA] = { "vr_K5VR_static", "vr_K5VR_pal_static" },
    [SKYBOX_HOUSE_ALLEY] = { "vr_KR3VR_static", "vr_KR3VR_pal_static" },
};

static uint32_t GetN64SkyboxTextureSize(int16_t skyboxId) {
    if (skyboxId == SKYBOX_NONE) {
        return 0;
    }

    // NORMAL_SKY and OVERCAST_SUNSET both load 2 texture banks + 2 palettes from the vr_fine/vr_cloud files.
    // All 16 banks are exactly 0xC000 and all 16 palettes are exactly 0x100, so the total is constant.
    if (skyboxId == SKYBOX_NORMAL_SKY || skyboxId == SKYBOX_OVERCAST_SUNSET) {
        return 2 * 0xC000 + 2 * 0x100;
    }

    // CUTSCENE_MAP loads two different texture files + 2 palette copies.
    if (skyboxId == SKYBOX_CUTSCENE_MAP) {
        const uint32_t tex0 = N64SizeData_GetDmaFileSize("vr_holy0_static");
        const uint32_t tex1 = N64SizeData_GetDmaFileSize("vr_holy1_static");
        const uint32_t pal = N64SizeData_GetDmaFileSize("vr_holy0_pal_static");
        return tex0 + tex1 + pal * 2;
    }

    // Indoor skyboxes: 1 texture + 1 palette, looked up from the DMA blob.
    if (skyboxId >= 0 && skyboxId < (int16_t)ARRAY_COUNT(sSkyboxDmaTable)) {
        const SkyboxDmaEntry* entry = &sSkyboxDmaTable[skyboxId];
        if (entry->texName != NULL) {
            const uint32_t tex = N64SizeData_GetDmaFileSize(entry->texName);
            const uint32_t pal = N64SizeData_GetDmaFileSize(entry->palName);
            return tex + pal;
        }
    }

    return 0;
}

// --------------------------------------------------------------------------------------------------------------------
// Skybox dListBuf and roomVtx
//
// Allocation sizes depend on the drawType, which is determined by the skybox ID:
//   SKYBOX_DRAW_128  (NORMAL_SKY, OVERCAST_SUNSET, CUTSCENE_MAP): 12-face dList, 5- or 6-face vtx
//   SKYBOX_DRAW_256  (all indoor skyboxes):                       8-face dList, 8-face vtx

// Gfx is 8 bytes on N64, Vtx is 0x10.
// --------------------------------------------------------------------------------------------------------------------

static void GetSkyboxDlistAndVtxSize(int16_t skyboxId, uint32_t* outDlistSize, uint32_t* outVtxSize) {
    if (skyboxId == SKYBOX_NONE) {
        *outDlistSize = 0;
        *outVtxSize = 0;
        return;
    }

    if (skyboxId == SKYBOX_NORMAL_SKY || skyboxId == SKYBOX_OVERCAST_SUNSET) {
        *outDlistSize = 12 * 150 * N64_SIZEOF_GFX;
        *outVtxSize = 5 * 32 * N64_SIZEOF_VTX;
    } else if (skyboxId == SKYBOX_CUTSCENE_MAP) {
        *outDlistSize = 12 * 150 * N64_SIZEOF_GFX;
        *outVtxSize = 6 * 32 * N64_SIZEOF_VTX;
    } else {
        // Indoor skyboxes: SKYBOX_DRAW_256_4FACE or SKYBOX_DRAW_256_3FACE, both use the same allocation.
        *outDlistSize = 8 * 150 * N64_SIZEOF_GFX;
        *outVtxSize = 8 * 32 * N64_SIZEOF_VTX;
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

static uint32_t GetBgCheckMemSize(PlayState* play) {
    const int16_t sceneNum = play->sceneNum;

    if (YREG(15) == 0x10 || YREG(15) == 0x20 || YREG(15) == 0x30 || YREG(15) == 0x40) {
        return sceneNum == SCENE_STABLE ? 0x3520 : 0x4E20;
    }

    if (BgCheck_IsSpotScene(play)) {
        return 0xF000;
    }

    uint32_t customMemSize = 0;
    if (BgCheck_TryGetCustomMemsize(sceneNum, &customMemSize)) {
        return customMemSize;
    }

    return 0x1CC00;
}

static uint32_t GetBgCheckThaTotal(PlayState* play) {
    return GetBgCheckMemSize(play) - N64_SIZEOF_COLLISION_CONTEXT;
}

// --------------------------------------------------------------------------------------------------------------------
// Object bank size (mirrors z_scene.c Object_InitBlank)
// --------------------------------------------------------------------------------------------------------------------

static uint32_t GetObjectBankSize(PlayState* play) {
    const int16_t sceneNum = play->sceneNum;
    if (sceneNum == SCENE_GANON_BOSS && gSaveContext.sceneSetupIndex == 4) {
        return 1177600;
    }

    if (sceneNum == SCENE_SPIRIT_TEMPLE_BOSS || sceneNum == SCENE_CHAMBER_OF_THE_SAGES ||
        sceneNum == SCENE_GANONDORF_BOSS) {
        return 1075200;
    }

    return 1024000;
}

// --------------------------------------------------------------------------------------------------------------------
// Elf message size
//
// On N64, loaded via Play_LoadFile into THA.  SoH loads from O2R via ResourceManager.  Only present if the scene's
// SpecialFiles command has cUpElfMsgNum != 0.  The file name is determined by cUpElfMsgNum (1-indexed).
// --------------------------------------------------------------------------------------------------------------------

static const char* sElfMsgDmaNames[] = {
    "elf_message_field",
    "elf_message_ydan",
};

static uint32_t GetElfMessageSize(PlayState* play) {
    if (play->cUpElfMsgs == NULL) {
        return 0;
    }

    const uint8_t elfMsgNum = N64Mem_GetElfMsgNum();
    if (elfMsgNum == 0 || elfMsgNum > ARRAY_COUNT(sElfMsgDmaNames)) {
        LUSLOG_WARN("[ArenaSizing] elfMsg: cUpElfMsgs non-NULL but elfMsgNum=%d out of range", elfMsgNum);
        return 0;
    }

    return N64SizeData_GetDmaFileSize(sElfMsgDmaNames[elfMsgNum - 1]);
}

// --------------------------------------------------------------------------------------------------------------------
// Room buffer max size (mirrors func_80096FE8 logic)
// --------------------------------------------------------------------------------------------------------------------

static uintptr_t GetMaxRoomSize(PlayState* play) {
    uintptr_t maxRoomSize = 0;

    for (size_t i = 0; i < play->numRooms; ++i) {
        const uintptr_t roomSize = play->roomList[i].vromEnd - play->roomList[i].vromStart;
        if (roomSize > maxRoomSize) {
            maxRoomSize = roomSize;
        }
    }

    if (play->transiActorCtx.numActors != 0) {
        const TransitionActorEntry* transitionActor = &play->transiActorCtx.list[0];

        for (size_t j = 0; j < play->transiActorCtx.numActors; ++j) {
            const int8_t frontRoom = transitionActor->sides[0].room;
            const int8_t backRoom = transitionActor->sides[1].room;
            const uintptr_t frontSize = frontRoom < 0
                                            ? 0
                                            : play->roomList[frontRoom].vromEnd - play->roomList[frontRoom].vromStart;
            const uintptr_t backSize = backRoom < 0
                                           ? 0
                                           : play->roomList[backRoom].vromEnd - play->roomList[backRoom].vromStart;

            const uintptr_t cumulSize = frontRoom != backRoom ? frontSize + backSize : frontSize;
            if (cumulSize > maxRoomSize) {
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

uint32_t ArenaSizing_ComputeN64ArenaSize(PlayState* play) {
    // Each THA consumer is allocated via GAME_STATE_ALLOC -> THA_AllocTailAlign16, which consumes ALIGN16(size) bytes.
    // We must align each consumer individually before summing; aligning the sum would under-count when individual
    // sizes are not 16-byte aligned (common for DMA file sizes).  BgCheck is the exception -- its internal allocations
    // use mixed alignment, but the tblMax computation absorbs the internal waste, so the simplified total
    // (memSize - sizeof(CollisionContext)) is used as-is.

    uint32_t total = 0;

    // Kaleido overlay buffer: max(ovl_kaleido_scope, ovl_player_actor) VRAM span, from O2R blob.
    const uint32_t kaleidoVramSize = N64SizeData_GetKaleidoVramSize();
    total += ALIGN16(kaleidoVramSize);

    // parameter_static: DMA file size, looked up from the O2R blob.
    const uint32_t parameterStaticSize = N64SizeData_GetDmaFileSize("parameter_static");
    total += ALIGN16(parameterStaticSize);

    // Fixed consumers
    {
        total += ALIGN16(N64_MATRIX_STACK_SIZE);
        total += ALIGN16(0x55 * N64_SIZEOF_EFFECT_SS);
        total += ALIGN16(N64_TEXT_BOX_SIZE);
        total += ALIGN16(N64_DO_ACTION_SIZE);
        total += ALIGN16(N64_ICON_ITEM_SIZE);
        total += ALIGN16(N64_MAP_SEGMENT_SIZE);

        const uint32_t fixed = total;
        LUSLOG_INFO("[ArenaSizing] fixed=0x%X (kaleido=0x%X, param=0x%X)", fixed, kaleidoVramSize,
                    parameterStaticSize);
    }

    // Scene-dependent consumers
    {
        const uint32_t objBank = GetObjectBankSize(play);
        const uint32_t sceneFile = N64SizeData_GetDmaFileSize(play->loadedScene->sceneFile.fileName);
        const uintptr_t roomBuf = GetMaxRoomSize(play);
        total += ALIGN16(objBank);
        total += ALIGN16(sceneFile);
        total += ALIGN16(roomBuf);
        LUSLOG_INFO("[ArenaSizing] objBank=0x%X, sceneFile=0x%X, roomBuf=0x%x", (uint32_t)objBank, (uint32_t)sceneFile,
                    (uint32_t)roomBuf);
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
        uint32_t dListSize = 0;
        uint32_t vtxSize = 0;
        uint32_t texSize = 0;
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
        const uint32_t bgCheck = GetBgCheckThaTotal(play);
        total += bgCheck;
        LUSLOG_INFO("[ArenaSizing] bgCheck=0x%X (memSize=0x%X)", bgCheck, GetBgCheckMemSize(play));
    }

    // Elf message
    {
        const uint32_t elfMsg = GetElfMessageSize(play);
        total += ALIGN16(elfMsg);
        LUSLOG_INFO("[ArenaSizing] elfMsg=0x%X", elfMsg);
    }

    // Map mark data overlay (dungeons only)
    {
        const uint32_t mapMarkData = GetMapMarkDataOverlaySize(play);
        total += ALIGN16(mapMarkData);
    }

    LUSLOG_INFO("[ArenaSizing] total=0x%X, arena=0x%X (budget=0x%X)", total, N64_THA_BUDGET - total,
                (uint32_t)N64_THA_BUDGET);

    if (total >= N64_THA_BUDGET) {
        return 0;
    }

    return N64_THA_BUDGET - total;
}