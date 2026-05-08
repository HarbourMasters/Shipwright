#include "arena_sizing.h"

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

#define N64_SIZEOF_CHAR_PTR 4 // sizeof(char*) on N64 MIPS
#define N64_SIZEOF_COLLISION_CONTEXT 0x1464 // CollisionContext size = 0x1464 (from decomp header comment)

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
#define N64_DO_ACTION_SIZE (3 * N64_SIZEOF_CHAR_PTR) // z_construct.c: 3 * sizeof(char*)
#define N64_ICON_ITEM_SIZE (0x1000 * 4) // z_construct.c: 0x1000 * 4 (buttonItems on N64)
#define N64_MAP_SEGMENT_SIZE (2 * N64_SIZEOF_CHAR_PTR) // z_map_exp.c: 2 * sizeof(char*)

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
        *outDlistSize = 8 * 150 * sizeof(Gfx);
        *outVtxSize = 256 * sizeof(Vtx);
    }
    else if (skyboxId == SKYBOX_CUTSCENE_MAP)
    {
        *outDlistSize = 12 * 150 * sizeof(Gfx);
        *outVtxSize = 192 * sizeof(Vtx);
    }
    else
    {
        *outDlistSize = 12 * 150 * sizeof(Gfx);
        *outVtxSize = 160 * sizeof(Vtx);
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
// SpecialFiles command has cUpElfMsgNum != 0.
// --------------------------------------------------------------------------------------------------------------------

static u32 GetElfMessageSize(PlayState* play)
{
    if (play->cUpElfMsgs != NULL)
    {
        // elf_message_field is 0x70, elf_message_ydan is 0x10 on NTSC 1.2.
        // #TODO: Per-version data.
        return 0x80;
    }

    return 0;
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
    u32 total = 0;

    // Per-version constants (N64-unique THA consumers SoH skips)
    total += vc->kaleidoOverlayVramSize;
    total += vc->parameterStaticSize;

    // Fixed consumers
    total += N64_MATRIX_STACK_SIZE;
    total += 0x55 * vc->effectSsSize;
    total += N64_TEXT_BOX_SIZE;
    total += N64_DO_ACTION_SIZE;
    total += N64_ICON_ITEM_SIZE;
    total += N64_MAP_SEGMENT_SIZE;

    // Scene-dependent consumers
    total += GetObjectBankSize(play);
    total += play->loadedScene->sceneFile.vromEnd - play->loadedScene->sceneFile.vromStart;
    total += GetMaxRoomSize(play);

    // Skybox
    {
        u32 dListSize = 0;
        u32 vtxSize = 0;
        GetSkyboxDlistAndVtxSize(play->skyboxId, &dListSize, &vtxSize);
        total += dListSize;
        total += vtxSize;
        total += GetN64SkyboxTextureSize(play->skyboxId);
    }

    // BgCheck
    total += GetBgCheckThaTotal(play);

    // Elf message
    total += GetElfMessageSize(play);

    if (total >= N64_THA_BUDGET)
    {
        return 0;
    }

    return N64_THA_BUDGET - total;
}
