#include <libultraship/bridge/resourcebridge.h>
#include <fast/resource/type/DisplayList.h>
#include <ship/Context.h>
#include <ship/resource/ResourceManager.h>
#include <spdlog/spdlog.h>
#include "soh/Enhancements/game-interactor/GameInteractor_Hooks.h"
#include "soh/ResourceManagerHelpers.h"
#include "soh/ShipInit.hpp"

#include <algorithm>
#include <string>

extern "C" {
#include "macros.h"
#include "variables.h"
#include "objects/object_link_boy/object_link_boy.h"
#include "objects/object_link_child/object_link_child.h"
#include "objects/object_custom_equip/object_custom_equip.h"
extern SaveContext gSaveContext;
}

/*
 * Makes player model mods complete: a mod that replaces Link's standalone hand DLs (and the
 * object_custom_equip FPS hands) gets its hands shown while holding vanilla items too, where
 * the vanilla DLs bake Link's hand in with the item.
 *
 * Each holding DL is patched in place:
 *  1. The baked hand's triangle commands are no-op'd. Only triangles: the item sections depend
 *     on state (textures/TLUTs/modes) loaded around the hand, so those commands keep running.
 *  2. The final ENDDL becomes a branch to the standalone hand DL, which resolves to the mod's
 *     version at draw time. Branching only at the very end keeps mod material state from
 *     corrupting the rest of the holding DL.
 *
 * PatchGfxByName skips custom-replaced DLs, so the hardcoded indices only ever touch authentic
 * assets. Custom *items* are customequipment.cpp's job — it bypasses the holding DL entirely.
 *
 * NOTE: hand DLs exported with legacy Fast64 are NOT compatible: their material reverts leave
 * the RDP in a non-vanilla state (1-cycle, texturing off) that corrupts vanilla DLs drawn later
 * in the frame. Confirmed not an issue with modern Fast64 exports.
 */

typedef struct {
    const char* dlist;             // authentic holding DL to patch
    const char* handDlist;         // standalone hand DL to draw instead
    const char* fallbackHandDlist; // used when handDlist has no custom/alt version (may be NULL)
    const char* name;              // unique patch name prefix
    int32_t branchIndex;           // the DL's final ENDDL, replaced with the branch
    int32_t noopStart;             // inclusive ranges containing the baked hand triangles
    int32_t noopEnd;
    int32_t noop2Start;
    int32_t noop2End;
} HandDlistPatchInfo;

static const HandDlistPatchInfo sHandDlistPatches[] = {
    // Adult
    { gLinkAdultLeftHandHoldingMasterSwordNearDL, gLinkAdultLeftHandClosedNearDL, nullptr, "cphMasterSword", 189, 104,
      188, 0, -1 },
    { gLinkAdultLeftHandHoldingBgsNearDL, gLinkAdultLeftHandClosedNearDL, nullptr, "cphBgs", 166, 77, 165, 0, -1 },
    { gLinkAdultHandHoldingBrokenGiantsKnifeDL, gLinkAdultLeftHandClosedNearDL, nullptr, "cphBrokenKnife", 163, 74, 162,
      0, -1 },
    { gLinkAdultLeftHandHoldingHammerNearDL, gLinkAdultLeftHandClosedNearDL, nullptr, "cphHammer", 181, 92, 180, 0,
      -1 },
    { gLinkAdultRightHandHoldingBowNearDL, gLinkAdultRightHandClosedNearDL, nullptr, "cphBow", 137, 50, 136, 0, -1 },
    { gLinkAdultRightHandHoldingHookshotNearDL, gLinkAdultRightHandClosedNearDL, nullptr, "cphHookshot", 170, 84, 169,
      0, -1 },
    { gLinkAdultRightHandHoldingOotNearDL, gLinkAdultRightHandNearDL, nullptr, "cphOot", 116, 3, 82, 0, -1 },
    // The Hylian shield DL draws its back face between the two hand sections
    { gLinkAdultRightHandHoldingHylianShieldNearDL, gLinkAdultRightHandClosedNearDL, nullptr, "cphHylianShield", 156,
      51, 100, 121, 155 },
    { gLinkAdultRightHandHoldingMirrorShieldNearDL, gLinkAdultRightHandClosedNearDL, nullptr, "cphMirrorShield", 179,
      66, 145, 0, -1 },
    // Adult first person (the hookshot "far" DL is what first person draws); prefer the FPS hand
    { gLinkAdultRightHandHoldingHookshotFarDL, gCustomAdultFPSHandDL, gLinkAdultRightHandClosedNearDL, "cphHookshotFar",
      219, 107, 218, 0, -1 },
    { gLinkAdultRightHandHoldingBowFirstPersonDL, gCustomAdultFPSHandDL, gLinkAdultRightHandClosedNearDL, "cphBowFP",
      174, 60, 173, 0, -1 },
    // Child
    { gLinkChildLeftFistAndBoomerangNearDL, gLinkChildLeftFistNearDL, nullptr, "cphBoomerang", 83, 47, 82, 0, -1 },
    { gLinkChildRightFistAndDekuShieldNearDL, gLinkChildRightHandClosedNearDL, nullptr, "cphDekuShield", 77, 47, 76, 0,
      -1 },
    { gLinkChildRightHandHoldingFairyOcarinaNearDL, gLinkChildRightHandNearDL, nullptr, "cphFairyOcarina", 81, 39, 80,
      0, -1 },
    { gLinkChildLeftFistAndKokiriSwordNearDL, gLinkChildLeftFistNearDL, nullptr, "cphKokiriSword", 91, 3, 38, 0, -1 },
    { gLinkChildRightHandHoldingSlingshotNearDL, gLinkChildRightHandClosedNearDL, nullptr, "cphSlingshot", 68, 3, 40, 0,
      -1 },
    { gLinkChildRightHandAndOotNearDL, gLinkChildRightHandNearDL, nullptr, "cphChildOot", 81, 3, 49, 0, -1 },
    { gLinkChildLeftHandHoldingMasterSwordDL, gLinkChildLeftFistNearDL, nullptr, "cphChildMasterSword", 147, 3, 47, 0,
      -1 },
    // Child first person slingshot: arm, hand and seed pouch are one fused mesh, so all three
    // are removed together and the FPS hand (which includes the arm) takes their place
    { gLinkChildRightArmStretchedSlingshotDL, gCustomChildFPSHandDL, nullptr, "cphChildSlingshotFP", 146, 42, 145, 0,
      -1 },
};

// No-op only the triangle commands in the range; everything else is state the item still needs
static void NoopHandTris(const HandDlistPatchInfo& info, int32_t start, int32_t end) {
    auto res = std::static_pointer_cast<Fast::DisplayList>(
        Ship::Context::GetRawInstance()->GetResourceManager()->LoadResource(info.dlist));
    if (res == nullptr || res->GetInitData()->IsCustom) {
        return;
    }
    int32_t last = std::min(end, (int32_t)res->Instructions.size() - 1);
    for (int32_t i = start; i <= last; i++) {
        Gfx* gfx = (Gfx*)&res->Instructions[i];
        uint8_t opcode = (uint8_t)(gfx->words.w0 >> 24);
        if (opcode == G_TRI1 || opcode == G_TRI2 || opcode == G_QUAD) {
            std::string patchName = std::string(info.name) + "Noop" + std::to_string(i);
            ResourceMgr_PatchGfxByName(info.dlist, patchName.c_str(), i, gsDPNoOp());
        }
    }
}

static void UnpatchRange(const HandDlistPatchInfo& info, int32_t start, int32_t end) {
    for (int32_t i = start; i <= end; i++) {
        std::string patchName = std::string(info.name) + "Noop" + std::to_string(i);
        ResourceMgr_UnpatchGfxByName(info.dlist, patchName.c_str());
    }
}

static void ApplyHandPatch(const HandDlistPatchInfo& info, const char* handDlist) {
    NoopHandTris(info, info.noopStart, info.noopEnd);
    if (info.noop2Start <= info.noop2End) {
        NoopHandTris(info, info.noop2Start, info.noop2End);
    }
    ResourceMgr_PatchGfxByName(info.dlist, info.name, info.branchIndex, gsSPBranchListOTRFilePath(handDlist));
}

static void RemoveHandPatch(const HandDlistPatchInfo& info) {
    ResourceMgr_UnpatchGfxByName(info.dlist, info.name);
    UnpatchRange(info, info.noopStart, info.noopEnd);
    if (info.noop2Start <= info.noop2End) {
        UnpatchRange(info, info.noop2Start, info.noop2End);
    }
}

static bool HandTargetAvailable(const char* path) {
    return path != nullptr && (ResourceGetIsCustomByName(path) || ResourceMgr_FileAltExists(path));
}

// The gfx interpreter asserts on unresolvable references, so verify everything a hand DL
// references (sub-DLs, vertices, textures) actually loads before branching to it
static bool DlistReferencesLoad(const char* path, int32_t depth) {
    if (path == nullptr) {
        return false;
    }
    if (depth > 8) {
        return true;
    }
    auto res = std::static_pointer_cast<Fast::DisplayList>(
        Ship::Context::GetRawInstance()->GetResourceManager()->LoadResource(path));
    if (res == nullptr) {
        return false;
    }
    size_t count = res->Instructions.size();
    for (size_t i = 0; i < count; i++) {
        Gfx* gfx = (Gfx*)&res->Instructions[i];
        uint8_t opcode = (uint8_t)(gfx->words.w0 >> 24);
        switch (opcode) {
            case G_DL_OTR_FILEPATH:
                if (!DlistReferencesLoad((const char*)gfx->words.w1, depth + 1)) {
                    return false;
                }
                break;
            case G_VTX_OTR_FILEPATH:
                if (ResourceGetDataByName((const char*)gfx->words.w1) == nullptr) {
                    return false;
                }
                i++; // second instruction holds vertex counts and offsets
                break;
            case G_SETTIMG_OTR_FILEPATH:
            case G_MTX_OTR_FILEPATH:
                if (ResourceGetDataByName((const char*)gfx->words.w1) == nullptr) {
                    return false;
                }
                break;
            // 128-bit commands whose second instruction holds a resource hash
            case G_DL_OTR_HASH:
            case G_VTX_OTR_HASH:
            case G_SETTIMG_OTR_HASH:
            case G_BRANCH_Z_OTR: {
                if (i + 1 >= count) {
                    return false;
                }
                Gfx* hashGfx = (Gfx*)&res->Instructions[++i];
                uint64_t hash = ((uint64_t)(uint32_t)hashGfx->words.w0 << 32) | (uint32_t)hashGfx->words.w1;
                if (ResourceGetDataByCrc(hash) == nullptr) {
                    return false;
                }
                break;
            }
            // 128-bit commands with nothing to validate
            case G_MARKER:
            case G_MTX_OTR:
            case G_MOVEMEM_OTR:
                i++;
                break;
        }
    }
    return true;
}

static bool HandTargetUsable(const char* path) {
    if (!HandTargetAvailable(path)) {
        return false;
    }
    if (!DlistReferencesLoad(path, 0)) {
        SPDLOG_WARN("CustomPlayerHands: not using hand DL '{}': it references resources that fail to load", path);
        return false;
    }
    return true;
}

// FPS hands have optional Goron/Zora tunic variants (mirrors customequipment's ResolveCustomFPSHand)
static const char* ResolveTunicFPSHand(const char* path) {
    const bool isAdult = path == gCustomAdultFPSHandDL;
    const bool isChild = path == gCustomChildFPSHandDL;
    if (!isAdult && !isChild) {
        return path;
    }
    const char* variant = nullptr;
    switch (TUNIC_EQUIP_TO_PLAYER(CUR_EQUIP_VALUE(EQUIP_TYPE_TUNIC))) {
        case PLAYER_TUNIC_GORON:
            variant = isAdult ? gCustomAdultGoronFPSHandDL : gCustomChildGoronFPSHandDL;
            break;
        case PLAYER_TUNIC_ZORA:
            variant = isAdult ? gCustomAdultZoraFPSHandDL : gCustomChildZoraFPSHandDL;
            break;
    }
    return (variant != nullptr && HandTargetUsable(variant)) ? variant : path;
}

static void ApplyOrRemoveHandPatches() {
    bool altEnabled = ResourceMgr_IsAltAssetsEnabled();
    for (const auto& info : sHandDlistPatches) {
        // Patch per DL only when a mod provides the hand, so partial mods keep vanilla poses
        const char* handDlist = nullptr;
        if (altEnabled) {
            const char* primary = ResolveTunicFPSHand(info.handDlist);
            handDlist = HandTargetUsable(primary)                  ? primary
                        : HandTargetUsable(info.fallbackHandDlist) ? info.fallbackHandDlist
                                                                   : nullptr;
        }
        if (handDlist != nullptr) {
            ApplyHandPatch(info, handDlist);
        } else {
            RemoveHandPatch(info);
        }
    }
}

static void RegisterCustomPlayerHands() {
    ApplyOrRemoveHandPatches();

    COND_HOOK(OnSceneInit, true, [](int32_t) { ApplyOrRemoveHandPatches(); });
    COND_HOOK(OnAssetAltChange, true, ApplyOrRemoveHandPatches);
    COND_HOOK(OnLinkEquipmentChange, true, ApplyOrRemoveHandPatches);
}

static RegisterShipInitFunc initFunc(RegisterCustomPlayerHands, { CVAR_SETTING("AltAssets") });
