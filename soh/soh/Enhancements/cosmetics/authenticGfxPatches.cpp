#include <libultraship/bridge.h>
#include <string>

extern "C" {
#include <libultraship/libultra.h>
#include "objects/gameplay_keep/gameplay_keep.h"
#include "objects/object_fz/object_fz.h"
#include "objects/object_gi_soldout/object_gi_soldout.h"
#include "objects/object_ik/object_ik.h"
#include "objects/object_link_child/object_link_child.h"

uint32_t ResourceMgr_GameHasMasterQuest();
uint32_t ResourceMgr_GameHasOriginal();
void ResourceMgr_PatchGfxByName(const char* path, const char* patchName, int index, Gfx instruction);
void ResourceMgr_UnpatchGfxByName(const char* path, const char* patchName);
}

typedef struct {
    const char* dlist;
    int startInstruction;
} DListPatchInfo;

static DListPatchInfo freezardEffectDListPatchInfos[] = {
    { gFreezardIntactDL, 5 },
    { gFreezardTopRightHornChippedDL, 5 },
    { gFreezardHeadChippedDL, 5 },
    { gFreezardIceTriangleDL, 5 },
    { gFreezardIceRockDL, 5 },
};

static DListPatchInfo ironKnuckleDListPatchInfos[] = {
    { object_ik_DL_01BE98, 39 },
    { object_ik_DL_01BE98, 59 },

    { object_ik_DL_01C130, 38 },

    { object_ik_DL_01C2B8, 39 },
    { object_ik_DL_01C2B8, 59 },

    { object_ik_DL_01C550, 38 },

    { object_ik_DL_01C7B8, 8 },
    { object_ik_DL_01C7B8, 28 },

    { object_ik_DL_01CB58, 8 },
    { object_ik_DL_01CB58, 31 },

    { object_ik_DL_01CCA0, 15 },
    { object_ik_DL_01CCA0, 37 },
    { object_ik_DL_01CCA0, 52 },
    { object_ik_DL_01CCA0, 68 },

    { object_ik_DL_01CEE0, 27 },
    { object_ik_DL_01CEE0, 46 },
    { object_ik_DL_01CEE0, 125 },

    { object_ik_DL_01D2B0, 8 },
    { object_ik_DL_01D2B0, 32 },
    
    { object_ik_DL_01D3F8, 15 },
    { object_ik_DL_01D3F8, 37 },
    { object_ik_DL_01D3F8, 52 },
    { object_ik_DL_01D3F8, 68 },

    { object_ik_DL_01D638, 23 },
    { object_ik_DL_01D638, 42 },
    { object_ik_DL_01D638, 110 },
};

void PatchDekuStickTextureOverflow() {
    // Custom texture for holding Deku Stick that accounts for overflow texture reading
    Gfx gDekuStickOverflowTexFix = gsDPSetTextureImage(G_IM_FMT_I, G_IM_SIZ_8b, 1, gDekuStickOverflowTex);

    // Gfx instructions to fix authentic vanilla bug where the Deku Stick texture is read as the wrong size
    Gfx gDekuStickTexFix[] = {
        gsDPLoadTextureBlock(gDekuStickTex, G_IM_FMT_I, G_IM_SIZ_8b, 8, 8, 0, G_TX_NOMIRROR | G_TX_WRAP,
                             G_TX_NOMIRROR | G_TX_WRAP, 4, 4, G_TX_NOLOD, G_TX_NOLOD)
    };

    const char* dlist = gLinkChildLinkDekuStickDL;
    int start = 5;

    if (!CVarGetInteger("gFixTexturesOOB", 0)) {
        // Unpatch the other texture fix
        for (size_t i = 0; i < 7; i++) {
            int instruction = start + (i == 0 ? 0 : i + 1);
            std::string unpatchName = "DekuStickFix" + std::to_string(instruction);
            ResourceMgr_UnpatchGfxByName(dlist, unpatchName.c_str());
        }

        std::string patchName = "DekuStickOverflow" + std::to_string(start);
        ResourceMgr_PatchGfxByName(dlist, patchName.c_str(), start, gDekuStickOverflowTexFix);
    } else {
        // Unpatch the other texture fix
        std::string unpatchName = "DekuStickOverflow" + std::to_string(start);
        ResourceMgr_UnpatchGfxByName(dlist, unpatchName.c_str());
        
        for (size_t i = 0; i < 7; i++) {
            int instruction = start + (i == 0 ? 0 : i + 1);
            std::string patchName = "DekuStickFix" + std::to_string(instruction);
            ResourceMgr_PatchGfxByName(dlist, patchName.c_str(), instruction, gDekuStickTexFix[i]);
        }
    }
}

void PatchFreezardTextureOverflow() {
    // Custom texture for Freezard effect that accounts for overflow texture reading
    Gfx gEffUnknown12OverflowTextFix = gsDPSetTextureImage(G_IM_FMT_IA, G_IM_SIZ_16b, 1, gEffUnknown12OverflowTex);

    // Gfx instructions to fix authentic vanilla bug where the Freezard effect texture is read as the wrong format
    Gfx gEffUnknown12TexFix[] = {
        gsDPLoadTextureBlock(gEffUnknown12Tex, G_IM_FMT_I, G_IM_SIZ_8b, 32, 32, 0, G_TX_NOMIRROR |
                             G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP, 5, 5, G_TX_NOLOD, G_TX_NOLOD)
    };

    for (const auto& patchInfo : freezardEffectDListPatchInfos) {
        const char* dlist = patchInfo.dlist;
        int start = patchInfo.startInstruction;

        char patchNameBuf[24];

        // Patch using custom overflowed texture
        if (!CVarGetInteger("gFixTexturesOOB", 0)) {
            // Unpatch the other texture fix
            for (size_t i = 0; i < 7; i++) {
                int instruction = start + (i == 0 ? 0 : i + 1);
                std::string unpatchName = "gEffUnknown12Fix" + std::to_string(instruction);
                ResourceMgr_UnpatchGfxByName(dlist, unpatchName.c_str());
            }

            std::string patchName = "gEffUnknown12Overflow" + std::to_string(start);
            ResourceMgr_PatchGfxByName(dlist, patchName.c_str(), start, gEffUnknown12OverflowTextFix);
        } else { // Patch texture to use correct image size
            // Unpatch the other texture fix
            std::string unpatchName = "gEffUnknown12Overflow" + std::to_string(start);
            ResourceMgr_UnpatchGfxByName(dlist, unpatchName.c_str());

            for (size_t i = 0; i < 7; i++) {
                int instruction = start + (i == 0 ? 0 : i + 1);
                std::string patchName = "gEffUnknown12Fix" + std::to_string(instruction);
                ResourceMgr_PatchGfxByName(dlist, patchName.c_str(), instruction, gEffUnknown12TexFix[i]);
            }
        }
    }
}

void PatchIronKnuckleTextureOverflow() {
    // Custom texture for Iron Knuckle that accounts for overflow texture reading
    Gfx gIronKnuckleMetalOverflowTexFix = gsDPSetTextureImage(G_IM_FMT_I, G_IM_SIZ_8b, 1, gIronKnuckleMetalOverflowTex);

    // Gfx instructions to fix authentic vanilla bug where the Iron Knuckle texture is read as the wrong format
    Gfx gIronKnuckleMetalTexFix[] = {
        gsDPLoadTextureBlock(gIronKnuckleMetalTex, G_IM_FMT_I, G_IM_SIZ_4b, 32, 64, 0, G_TX_MIRROR | G_TX_WRAP,
                            G_TX_MIRROR | G_TX_WRAP, 5, 6, G_TX_NOLOD, G_TX_NOLOD)
    };

    for (const auto& patchInfo : ironKnuckleDListPatchInfos) {
        const char* dlist = patchInfo.dlist;
        int start = patchInfo.startInstruction;

        // OTRTODO: Patching to use the correct size format for Iron Knuckle causes a tile size failure
        // Until this is solved, Iron Knuckle will be hardcoded to always display with the "authentic" texture fix

        // Patch using custom overflowed texture
        // if (!CVarGetInteger("gFixTexturesOOB", 0)) {
            // Unpatch the other texture fix
            for (size_t i = 0; i < 7; i++) {
                int instruction = start + (i == 0 ? 0 : i + 1);
                std::string unpatchName = "MetalTexFix" + std::to_string(instruction);
                ResourceMgr_UnpatchGfxByName(dlist, unpatchName.c_str());
            }

            std::string patchName = "MetalTexOverflow" + std::to_string(start);
            ResourceMgr_PatchGfxByName(dlist, patchName.c_str(), start, gIronKnuckleMetalOverflowTexFix);
        // } else { // Patch texture to use correct image size
        //     // Unpatch the other texture fix
        //     std::string unpatchName = "MetalTexOverflow" + std::to_string(start);
        //     ResourceMgr_UnpatchGfxByName(dlist, unpatchName.c_str());

        //     // Patch texture to use correct image size
        //     for (size_t i = 0; i < 7; i++) {
        //         int instruction = start + (i == 0 ? 0 : i + 1);
        //         std::string patchName = "MetalTexFix" + std::to_string(instruction);
        //         ResourceMgr_PatchGfxByName(dlist, patchName.c_str(), instruction, gIronKnuckleMetalTexFix[i]);
        //     }
        // }
    }
}

void ApplyAuthenticGfxPatches() {
    PatchDekuStickTextureOverflow();
    PatchFreezardTextureOverflow();
    PatchIronKnuckleTextureOverflow();
}

// Patches the Sold Out GI DL to render the texture in the mirror boundary
void PatchMirroredSoldOutGI() {
    static const char gSoldOutGIVtx[] = "__OTR__objects/object_gi_soldout/object_gi_soldoutVtx_000400";

    static Vtx* mirroredSoldOutVtx;

    // Using a dummy texture here, but will be ignoring the texture command itself
    // Only need to patch over the two SetTile commands to get the MIRROR effect
    Gfx mirroredSoldOutTex[] = {
        gsDPLoadTextureBlock("", G_IM_FMT_IA, G_IM_SIZ_8b, 32, 32, 0, G_TX_MIRROR | G_TX_WRAP,
                             G_TX_NOMIRROR | G_TX_CLAMP, 5, 5, G_TX_NOLOD, G_TX_NOLOD),
    };

    if (CVarGetInteger("gMirroredWorld", 0)) {
        if (mirroredSoldOutVtx == nullptr) {
            // Copy the original vertices that we want to modify (4 at the beginning of the resource)
            mirroredSoldOutVtx = (Vtx*)malloc(sizeof(Vtx) * 4);
            Vtx* origVtx = (Vtx*)ResourceGetDataByName(gSoldOutGIVtx);
            memcpy(mirroredSoldOutVtx, origVtx, sizeof(Vtx) * 4);

            // Offset the vertex U coordinate values by the width of the texture
            for (size_t i = 0; i < 4; i++) {
                mirroredSoldOutVtx[i].v.tc[0] += 32 << 5;
            }
        }

        ResourceMgr_PatchGfxByName(gGiSoldOutDL, "SoldOutGITexture_1", 9, mirroredSoldOutTex[1]);
        ResourceMgr_PatchGfxByName(gGiSoldOutDL, "SoldOutGITexture_2", 13, mirroredSoldOutTex[5]);
        ResourceMgr_PatchGfxByName(gGiSoldOutDL, "SoldOutGITextureCords_1", 17, gsSPVertex(mirroredSoldOutVtx, 4, 0));
        // noop as the original vertex command is 128 bit wide
        ResourceMgr_PatchGfxByName(gGiSoldOutDL, "SoldOutGITextureCords_2", 18, gsSPNoOp());
    } else {
        if (mirroredSoldOutVtx != nullptr) {
            free(mirroredSoldOutVtx);
            mirroredSoldOutVtx = nullptr;
        }

        ResourceMgr_UnpatchGfxByName(gGiSoldOutDL, "SoldOutGITexture_1");
        ResourceMgr_UnpatchGfxByName(gGiSoldOutDL, "SoldOutGITexture_2");
        ResourceMgr_UnpatchGfxByName(gGiSoldOutDL, "SoldOutGITextureCords_1");
        ResourceMgr_UnpatchGfxByName(gGiSoldOutDL, "SoldOutGITextureCords_2");
    }
}

// Patches the Sun Song Etching in the Royal Grave to be mirrored in mirror mode
// This is achieved by mirroring the texture at the boundary and overriding the vertex texture coordinates
void PatchMirroredSunSongEtching() {
    static const char gMqRoyalGraveBackRoomDL[] = "__OTR__scenes/mq/hakaana_ouke_scene/hakaana_ouke_room_2DL_005040";
    static const char gNonMqRoyalGraveBackRoomDL[] = "__OTR__scenes/nonmq/hakaana_ouke_scene/hakaana_ouke_room_2DL_005040";
    static const char gMqRoyalGraveBackRoomSongVtx[] = "__OTR__scenes/mq/hakaana_ouke_scene/hakaana_ouke_room_2Vtx_004F80";
    static const char gNonMqRoyalGraveBackRoomSongVtx[] = "__OTR__scenes/nonmq/hakaana_ouke_scene/hakaana_ouke_room_2Vtx_004F80";

    static Vtx* mirroredSunSongVtx;

    // Using a dummy texture here, but will be ignoring the texture command itself
    // Only need to patch over the two SetTile commands to get the MIRROR effect
    Gfx mirroredSunSongTex[] = {
        gsDPLoadTextureBlock("", G_IM_FMT_IA, G_IM_SIZ_8b, 128, 32, 0, G_TX_MIRROR | G_TX_WRAP,
                             G_TX_NOMIRROR | G_TX_CLAMP, 7, 5, G_TX_NOLOD, G_TX_NOLOD)
    };

    const char* royalGraveBackRoomDL;
    const char* royalGraveBackRoomSongVtx;

    // If we have the original game, then always prefer the nonmq paths as that is what will be used in game
    if (ResourceMgr_GameHasOriginal()) {
        royalGraveBackRoomDL = gNonMqRoyalGraveBackRoomDL;
        royalGraveBackRoomSongVtx = gNonMqRoyalGraveBackRoomSongVtx;
    } else {
        royalGraveBackRoomDL = gMqRoyalGraveBackRoomDL;
        royalGraveBackRoomSongVtx = gMqRoyalGraveBackRoomSongVtx;
    }

    if (CVarGetInteger("gMirroredWorld", 0)) {
        if (mirroredSunSongVtx == nullptr) {
            // Copy the original vertices that we want to modify (4 at the beginning of the resource)
            mirroredSunSongVtx = (Vtx*)malloc(sizeof(Vtx) * 4);
            Vtx* origVtx = (Vtx*)ResourceGetDataByName(royalGraveBackRoomSongVtx);
            memcpy(mirroredSunSongVtx, origVtx, sizeof(Vtx) * 4);

            // Offset the vertex U coordinate values by the width of the texture
            for (size_t i = 0; i < 4; i++) {
                mirroredSunSongVtx[i].v.tc[0] += 128 << 5;
            }
        }

        ResourceMgr_PatchGfxByName(royalGraveBackRoomDL, "RoyalGraveSunSongTexture_1", 13, mirroredSunSongTex[1]);
        ResourceMgr_PatchGfxByName(royalGraveBackRoomDL, "RoyalGraveSunSongTexture_2", 17, mirroredSunSongTex[5]);
        ResourceMgr_PatchGfxByName(royalGraveBackRoomDL, "RoyalGraveSunSongTextureCords_1", 24, gsSPVertex(mirroredSunSongVtx, 4, 0));
        // noop as the original vertex command is 128 bit wide
        ResourceMgr_PatchGfxByName(royalGraveBackRoomDL, "RoyalGraveSunSongTextureCords_2", 25, gsSPNoOp());
    } else {
        if (mirroredSunSongVtx != nullptr) {
            free(mirroredSunSongVtx);
            mirroredSunSongVtx = nullptr;
        }

        ResourceMgr_UnpatchGfxByName(royalGraveBackRoomDL, "RoyalGraveSunSongTexture_1");
        ResourceMgr_UnpatchGfxByName(royalGraveBackRoomDL, "RoyalGraveSunSongTexture_2");
        ResourceMgr_UnpatchGfxByName(royalGraveBackRoomDL, "RoyalGraveSunSongTextureCords_1");
        ResourceMgr_UnpatchGfxByName(royalGraveBackRoomDL, "RoyalGraveSunSongTextureCords_2");
    }
}

void ApplyMirrorWorldGfxPatches() {
    PatchMirroredSoldOutGI();
    PatchMirroredSunSongEtching();
}
