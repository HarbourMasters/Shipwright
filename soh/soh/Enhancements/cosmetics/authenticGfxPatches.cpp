#include <libultraship/bridge.h>
#include <string>
#include "soh/OTRGlobals.h"
#include "soh/cvar_prefixes.h"
#include "soh/ResourceManagerHelpers.h"

extern "C" {
#include <libultraship/libultra.h>
#include "objects/gameplay_keep/gameplay_keep.h"
#include "objects/object_fz/object_fz.h"
#include "objects/object_gi_soldout/object_gi_soldout.h"
#include "objects/object_ik/object_ik.h"
#include "objects/object_link_child/object_link_child.h"
}

typedef struct {
    const char* dlist;
    int startInstruction;
} DListPatchInfo;

static DListPatchInfo freezardBodyDListPatchInfos[] = {
    { gFreezardIntactDL, 5 },      { gFreezardTopRightHornChippedDL, 5 },
    { gFreezardHeadChippedDL, 5 }, { gFreezardIceTriangleDL, 5 },
    { gFreezardIceRockDL, 5 },
};

static DListPatchInfo ironKnuckleDListPatchInfos[] = {
    // VambraceLeft
    { object_ik_DL_01BE98, 39 },
    { object_ik_DL_01BE98, 59 },

    // ArmLeft
    { object_ik_DL_01C130, 38 },

    // VambraceRight
    { object_ik_DL_01C2B8, 39 },
    { object_ik_DL_01C2B8, 59 },

    // ArmRight
    { object_ik_DL_01C550, 38 },

    // Waist
    { object_ik_DL_01C7B8, 8 },
    { object_ik_DL_01C7B8, 28 },

    // PauldronLeft
    { object_ik_DL_01CB58, 8 },
    { object_ik_DL_01CB58, 31 },

    // BootTipLeft
    { object_ik_DL_01CCA0, 15 },
    { object_ik_DL_01CCA0, 37 },
    { object_ik_DL_01CCA0, 52 },
    { object_ik_DL_01CCA0, 68 },

    // WaistArmorLeft
    { object_ik_DL_01CEE0, 27 },
    { object_ik_DL_01CEE0, 46 },
    { object_ik_DL_01CEE0, 125 },

    // PauldronRight
    { object_ik_DL_01D2B0, 8 },
    { object_ik_DL_01D2B0, 32 },

    // BootTipRight
    { object_ik_DL_01D3F8, 15 },
    { object_ik_DL_01D3F8, 37 },
    { object_ik_DL_01D3F8, 52 },
    { object_ik_DL_01D3F8, 68 },

    // WaistArmorRight
    { object_ik_DL_01D638, 23 },
    { object_ik_DL_01D638, 42 },
    { object_ik_DL_01D638, 110 },
};

static DListPatchInfo arrowTipDListPatchInfos[] = {
    { gArrowNearDL, 46 },
    { gArrowFarDL, 5 },
};

void PatchArrowTipTexture() {
    // Custom texture for Arrow tips that accounts for overflow texture reading
    Gfx arrowTipTextureWithOverflowFixGfx =
        gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b_LOAD_BLOCK, 1, gHilite2Tex_Overflow);

    // Gfx instructions to fix authentic vanilla bug where the Arrow tips texture is read as the wrong size
    Gfx arrowTipTextureWithSizeFixGfx[] = {
        gsDPLoadTextureBlock(gHilite2Tex, G_IM_FMT_RGBA, G_IM_SIZ_16b, 16, 16, 0, G_TX_MIRROR | G_TX_WRAP,
                             G_TX_MIRROR | G_TX_WRAP, 5, 5, 1, 1),
    };

    bool fixTexturesOOB = CVarGetInteger(CVAR_ENHANCEMENT("FixTexturesOOB"), 0);

    for (const auto& patchInfo : arrowTipDListPatchInfos) {
        const char* dlist = patchInfo.dlist;
        int start = patchInfo.startInstruction;

        // Patch using custom overflowed texture
        if (!fixTexturesOOB) {
            // Unpatch the other texture fix
            for (size_t i = 4; i < 8; i++) {
                int instruction = start + i;
                std::string unpatchName = "arrowTipTextureWithSizeFix_" + std::to_string(instruction);
                ResourceMgr_UnpatchGfxByName(dlist, unpatchName.c_str());
            }

            std::string patchName = "arrowTipTextureWithOverflowFix_" + std::to_string(start);
            std::string patchName2 = "arrowTipTextureWithOverflowFix_" + std::to_string(start + 1);
            ResourceMgr_PatchGfxByName(dlist, patchName.c_str(), start, arrowTipTextureWithOverflowFixGfx);
            ResourceMgr_PatchGfxByName(dlist, patchName2.c_str(), start + 1, gsSPNoOp());
        } else { // Patch texture to use correct image size/fmt
            // Unpatch the other texture fix
            std::string unpatchName = "arrowTipTextureWithOverflowFix_" + std::to_string(start);
            std::string unpatchName2 = "arrowTipTextureWithOverflowFix_" + std::to_string(start + 1);
            ResourceMgr_UnpatchGfxByName(dlist, unpatchName.c_str());
            ResourceMgr_UnpatchGfxByName(dlist, unpatchName2.c_str());

            for (size_t i = 4; i < 8; i++) {
                int instruction = start + i;
                std::string patchName = "arrowTipTextureWithSizeFix_" + std::to_string(instruction);

                if (i == 0) {
                    ResourceMgr_PatchGfxByName(dlist, patchName.c_str(), instruction, gsSPNoOp());
                } else {
                    ResourceMgr_PatchGfxByName(dlist, patchName.c_str(), instruction,
                                               arrowTipTextureWithSizeFixGfx[i - 1]);
                }
            }
        }
    }
}

void PatchDekuStickTextureOverflow() {
    // Custom texture for holding Deku Stick that accounts for overflow texture reading
    Gfx dekuSticTexkWithOverflowFixGfx = gsDPSetTextureImage(G_IM_FMT_I, G_IM_SIZ_8b, 1, gDekuStickOverflowTex);

    // Gfx instructions to fix authentic vanilla bug where the Deku Stick texture is read as the wrong size
    Gfx dekuStickTexWithSizeFixGfx[] = {
        gsDPLoadTextureBlock(gDekuStickTex, G_IM_FMT_I, G_IM_SIZ_8b, 8, 8, 0, G_TX_NOMIRROR | G_TX_WRAP,
                             G_TX_NOMIRROR | G_TX_WRAP, 4, 4, G_TX_NOLOD, G_TX_NOLOD),
    };

    const char* dlist = gLinkChildLinkDekuStickDL;
    int start = 5;

    // Patch using custom overflowed texture
    if (!CVarGetInteger(CVAR_ENHANCEMENT("FixTexturesOOB"), 0)) {
        // Unpatch the other texture fix
        for (size_t i = 0; i < 8; i++) {
            int instruction = start + i;
            std::string unpatchName = "dekuStickWithSizeFix_" + std::to_string(instruction);
            ResourceMgr_UnpatchGfxByName(dlist, unpatchName.c_str());
        }

        std::string patchName = "dekuStickWithOverflowFix_" + std::to_string(start);
        std::string patchName2 = "dekuStickWithOverflowFix_" + std::to_string(start + 1);
        ResourceMgr_PatchGfxByName(dlist, patchName.c_str(), start, dekuSticTexkWithOverflowFixGfx);
        ResourceMgr_PatchGfxByName(dlist, patchName2.c_str(), start + 1, gsSPNoOp());
    } else { // Patch texture to use correct image size/fmt
        // Unpatch the other texture fix
        std::string unpatchName = "dekuStickWithOverflowFix_" + std::to_string(start);
        std::string unpatchName2 = "dekuStickWithOverflowFix_" + std::to_string(start + 1);
        ResourceMgr_UnpatchGfxByName(dlist, unpatchName.c_str());
        ResourceMgr_UnpatchGfxByName(dlist, unpatchName2.c_str());

        for (size_t i = 0; i < 8; i++) {
            int instruction = start + i;
            std::string patchName = "dekuStickWithSizeFix_" + std::to_string(instruction);

            if (i == 0) {
                ResourceMgr_PatchGfxByName(dlist, patchName.c_str(), instruction, gsSPNoOp());
            } else {
                ResourceMgr_PatchGfxByName(dlist, patchName.c_str(), instruction, dekuStickTexWithSizeFixGfx[i - 1]);
            }
        }
    }
}

void PatchFreezardTextureOverflow() {
    // Custom texture for Freezard effect that accounts for overflow texture reading
    Gfx freezardBodyTextureWithOverflowFixGfx =
        gsDPSetTextureImage(G_IM_FMT_IA, G_IM_SIZ_16b, 1, gEffUnknown12OverflowTex);

    // Gfx instructions to fix authentic vanilla bug where the Freezard effect texture is read as the wrong format
    Gfx freezardBodyTextureWithFormatFixGfx[] = {
        gsDPLoadTextureBlock(gEffUnknown12Tex, G_IM_FMT_I, G_IM_SIZ_8b, 32, 32, 0, G_TX_NOMIRROR | G_TX_WRAP,
                             G_TX_NOMIRROR | G_TX_WRAP, 5, 5, G_TX_NOLOD, G_TX_NOLOD),
    };

    bool fixTexturesOOB = CVarGetInteger(CVAR_ENHANCEMENT("FixTexturesOOB"), 0);

    for (const auto& patchInfo : freezardBodyDListPatchInfos) {
        const char* dlist = patchInfo.dlist;
        int start = patchInfo.startInstruction;

        // Patch using custom overflowed texture
        if (!fixTexturesOOB) {
            // Unpatch the other texture fix
            for (size_t i = 0; i < 8; i++) {
                int instruction = start + i;
                std::string unpatchName = "freezardBodyTextureWithFormatFix_" + std::to_string(instruction);
                ResourceMgr_UnpatchGfxByName(dlist, unpatchName.c_str());
            }

            std::string patchName = "freezardBodyTextureWithOverflowFix_" + std::to_string(start);
            std::string patchName2 = "freezardBodyTextureWithOverflowFix_" + std::to_string(start + 1);
            ResourceMgr_PatchGfxByName(dlist, patchName.c_str(), start, freezardBodyTextureWithOverflowFixGfx);
            ResourceMgr_PatchGfxByName(dlist, patchName2.c_str(), start + 1, gsSPNoOp());
        } else { // Patch texture to use correct image size/fmt
            // Unpatch the other texture fix
            std::string unpatchName = "freezardBodyTextureWithOverflowFix_" + std::to_string(start);
            std::string unpatchName2 = "freezardBodyTextureWithOverflowFix_" + std::to_string(start + 1);
            ResourceMgr_UnpatchGfxByName(dlist, unpatchName.c_str());
            ResourceMgr_UnpatchGfxByName(dlist, unpatchName2.c_str());

            for (size_t i = 0; i < 8; i++) {
                int instruction = start + i;
                std::string patchName = "freezardBodyTextureWithFormatFix_" + std::to_string(instruction);

                if (i == 0) {
                    ResourceMgr_PatchGfxByName(dlist, patchName.c_str(), instruction, gsSPNoOp());
                } else {
                    ResourceMgr_PatchGfxByName(dlist, patchName.c_str(), instruction,
                                               freezardBodyTextureWithFormatFixGfx[i - 1]);
                }
            }
        }
    }
}

void PatchIronKnuckleTextureOverflow() {
    // Custom texture for Iron Knuckle that accounts for overflow texture reading
    Gfx ironKnuckleFireTexWithOverflowFixGfx =
        gsDPSetTextureImage(G_IM_FMT_I, G_IM_SIZ_8b, 1, gIronKnuckleMetalOverflowTex);

    // Gfx instructions to fix authentic vanilla bug where the Iron Knuckle texture is read as the wrong format
    Gfx ironKnuckleFireTexWithFormatFixGfx[] = {
        gsDPLoadTextureBlock_4b(gIronKnuckleMetalTex, G_IM_FMT_I, 32, 64, 0, G_TX_MIRROR | G_TX_WRAP,
                                G_TX_MIRROR | G_TX_WRAP, 5, 6, G_TX_NOLOD, G_TX_NOLOD),
    };

    bool fixTexturesOOB = CVarGetInteger(CVAR_ENHANCEMENT("FixTexturesOOB"), 0);

    for (const auto& patchInfo : ironKnuckleDListPatchInfos) {
        const char* dlist = patchInfo.dlist;
        int start = patchInfo.startInstruction;

        // Patch using custom overflowed texture
        if (!fixTexturesOOB) {
            // Unpatch the other texture fix
            for (size_t i = 0; i < 8; i++) {
                int instruction = start + i;
                std::string unpatchName = "ironKnuckleFireTexWithSizeFix_" + std::to_string(instruction);
                ResourceMgr_UnpatchGfxByName(dlist, unpatchName.c_str());
            }

            std::string patchName = "ironKnuckleFireTexWithOverflowFix_" + std::to_string(start);
            std::string patchName2 = "ironKnuckleFireTexWithOverflowFix_" + std::to_string(start + 1);
            ResourceMgr_PatchGfxByName(dlist, patchName.c_str(), start, ironKnuckleFireTexWithOverflowFixGfx);
            ResourceMgr_PatchGfxByName(dlist, patchName2.c_str(), start + 1, ironKnuckleFireTexWithOverflowFixGfx);
        } else { // Patch texture to use correct image size/fmt
            // Unpatch the other texture fix
            std::string unpatchName = "ironKnuckleFireTexWithOverflowFix_" + std::to_string(start);
            std::string unpatchName2 = "ironKnuckleFireTexWithOverflowFix_" + std::to_string(start + 1);
            ResourceMgr_UnpatchGfxByName(dlist, unpatchName.c_str());
            ResourceMgr_UnpatchGfxByName(dlist, unpatchName2.c_str());

            for (size_t i = 0; i < 8; i++) {
                int instruction = start + i;
                std::string patchName = "ironKnuckleFireTexWithSizeFix_" + std::to_string(instruction);

                if (i == 0) {
                    ResourceMgr_PatchGfxByName(dlist, patchName.c_str(), instruction, gsSPNoOp());
                } else {
                    ResourceMgr_PatchGfxByName(dlist, patchName.c_str(), instruction,
                                               ironKnuckleFireTexWithFormatFixGfx[i - 1]);
                }
            }
        }
    }
}

void ApplyAuthenticGfxPatches() {
    PatchArrowTipTexture();
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

    if (CVarGetInteger(CVAR_ENHANCEMENT("MirroredWorld"), 0)) {
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
    // Only using these strings for graphics patching lookup, we don't need aligned assets here
    static const char gRoyalGraveBackRoomDL[] = "__OTR__scenes/shared/hakaana_ouke_scene/hakaana_ouke_room_2DL_005040";
    static const char gRoyalGraveBackRoomSongVtx[] =
        "__OTR__scenes/shared/hakaana_ouke_scene/hakaana_ouke_room_2Vtx_004F80";

    static Vtx* mirroredSunSongVtx;

    // Using a dummy texture here, but will be ignoring the texture command itself
    // Only need to patch over the two SetTile commands to get the MIRROR effect
    Gfx mirroredSunSongTex[] = {
        gsDPLoadTextureBlock("", G_IM_FMT_IA, G_IM_SIZ_8b, 128, 32, 0, G_TX_MIRROR | G_TX_WRAP,
                             G_TX_NOMIRROR | G_TX_CLAMP, 7, 5, G_TX_NOLOD, G_TX_NOLOD),
    };

    if (CVarGetInteger(CVAR_ENHANCEMENT("MirroredWorld"), 0)) {
        if (mirroredSunSongVtx == nullptr) {
            // Copy the original vertices that we want to modify (4 at the beginning of the resource)
            mirroredSunSongVtx = (Vtx*)malloc(sizeof(Vtx) * 4);
            Vtx* origVtx = (Vtx*)ResourceGetDataByName(gRoyalGraveBackRoomSongVtx);
            memcpy(mirroredSunSongVtx, origVtx, sizeof(Vtx) * 4);

            // Offset the vertex U coordinate values by the width of the texture
            for (size_t i = 0; i < 4; i++) {
                mirroredSunSongVtx[i].v.tc[0] += 128 << 5;
            }
        }

        ResourceMgr_PatchGfxByName(gRoyalGraveBackRoomDL, "RoyalGraveSunSongTexture_1", 13, mirroredSunSongTex[1]);
        ResourceMgr_PatchGfxByName(gRoyalGraveBackRoomDL, "RoyalGraveSunSongTexture_2", 17, mirroredSunSongTex[5]);
        ResourceMgr_PatchGfxByName(gRoyalGraveBackRoomDL, "RoyalGraveSunSongTextureCords_1", 24,
                                   gsSPVertex(mirroredSunSongVtx, 4, 0));
        // noop as the original vertex command is 128 bit wide
        ResourceMgr_PatchGfxByName(gRoyalGraveBackRoomDL, "RoyalGraveSunSongTextureCords_2", 25, gsSPNoOp());
    } else {
        if (mirroredSunSongVtx != nullptr) {
            free(mirroredSunSongVtx);
            mirroredSunSongVtx = nullptr;
        }

        ResourceMgr_UnpatchGfxByName(gRoyalGraveBackRoomDL, "RoyalGraveSunSongTexture_1");
        ResourceMgr_UnpatchGfxByName(gRoyalGraveBackRoomDL, "RoyalGraveSunSongTexture_2");
        ResourceMgr_UnpatchGfxByName(gRoyalGraveBackRoomDL, "RoyalGraveSunSongTextureCords_1");
        ResourceMgr_UnpatchGfxByName(gRoyalGraveBackRoomDL, "RoyalGraveSunSongTextureCords_2");
    }
}

void ApplyMirrorWorldGfxPatches() {
    PatchMirroredSoldOutGI();
    PatchMirroredSunSongEtching();
}
