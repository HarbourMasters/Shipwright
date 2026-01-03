#include <initializer_list>
#include "src/overlays/actors/ovl_En_Elf/z_en_elf.h"
#include "objects/object_link_boy/object_link_boy.h"
#include "objects/object_link_child/object_link_child.h"
#include "objects/object_custom_equip/object_custom_equip.h"
#include "soh/Enhancements/game-interactor/GameInteractor.h"
#include "soh/ShipInit.hpp"
#include "soh/ResourceManagerHelpers.h"
#include "soh_assets.h"
#include "kaleido.h"
#include "soh/cvar_prefixes.h"

extern SaveContext gSaveContext;
extern PlayState* gPlayState;
extern void Overlay_DisplayText(float duration, const char* text);
void DummyPlayer_Update(Actor* actor, PlayState* play);

static void UpdatePatchCustomEquipmentDlists();
static void RefreshCustomEquipment();
static bool HasDummyPlayers();

static const char* ResolveCustomChain(std::initializer_list<const char*> paths) {
    const char* fallback = nullptr;
    for (auto path : paths) {
        if (path != nullptr) {
            fallback = path;
            if (ResourceMgr_FileExists(path)) {
                return path;
            }
        }
    }
    return fallback;
}

static const char* GetBreakableLongswordDL() {
    return ResolveCustomChain({ gCustomBreakableLongswordDL, gCustomLongswordDL });
}

static const char* GetBreakableLongswordSheathDL() {
    return ResolveCustomChain({ gCustomBreakableLongswordSheathDL, gCustomLongswordSheathDL });
}

static const char* GetBreakableLongswordInSheathDL() {
    return ResolveCustomChain({ gCustomBreakableLongswordInSheathDL, gCustomLongswordInSheathDL });
}

static const char* GetBrokenLongswordSheathDL() {
    return ResolveCustomChain(
        { gCustomBrokenLongswordSheathDL, gCustomBreakableLongswordSheathDL, gCustomLongswordSheathDL });
}

static const char* GetBrokenLongswordInSheathDL() {
    return ResolveCustomChain(
        { gCustomBrokenLongswordInSheathDL, gCustomBreakableLongswordInSheathDL, gCustomLongswordInSheathDL });
}

static void UpdateCustomEquipmentSetModel(Player* player, u8 ModelGroup) {
    (void)ModelGroup;

    if (player == nullptr || gPlayState == nullptr || player != GET_PLAYER(gPlayState)) {
        return;
    }

    RefreshCustomEquipment();
}

static void UpdateCustomEquipment() {
    if (!GameInteractor::IsSaveLoaded() || gPlayState == nullptr) {
        return;
    }

    Player* player = GET_PLAYER(gPlayState);
    if (player == nullptr || player->actor.update == DummyPlayer_Update) {
        return;
    }

    const bool altAssetsRuntime = ResourceMgr_IsAltAssetsEnabled();

    // If multiplayer dummy actors are present, skip patching shared resources to avoid corrupting them.
    if (HasDummyPlayers() && altAssetsRuntime) {
        return;
    }

    RefreshCustomEquipment();
}

static void PatchCustomEquipment() {
    COND_HOOK(OnPlayerSetModels, true, UpdateCustomEquipmentSetModel);
    COND_HOOK(OnLinkEquipmentChange, true, UpdateCustomEquipment);
    COND_HOOK(OnLinkSkeletonInit, true, UpdateCustomEquipment);
    COND_HOOK(OnAssetAltChange, true, UpdateCustomEquipment);
}

static RegisterShipInitFunc initFunc(PatchCustomEquipment);

static void RefreshCustomEquipment() {
    if (!GameInteractor::IsSaveLoaded() || gPlayState == NULL || GET_PLAYER(gPlayState) == nullptr) {
        return;
    }

    const bool hasDummyPlayers = HasDummyPlayers();
    const bool altAssetsRuntime = ResourceMgr_IsAltAssetsEnabled();

    // Keep custom patches off dummy players, but still allow unpatching when alt assets are disabled so we can
    // restore vanilla display lists.
    if (hasDummyPlayers && altAssetsRuntime) {
        return;
    }

    UpdatePatchCustomEquipmentDlists();
}

void PatchOrUnpatch(const char* resource, const char* gfx, const char* dlist1, const char* dlist2, const char* dlist3,
                    const char* alternateDL) {
    if (resource == NULL || gfx == NULL || dlist1 == NULL || dlist2 == NULL) {
        return;
    }

    const bool altAssetsRuntime = ResourceMgr_IsAltAssetsEnabled();

    if (!altAssetsRuntime) {
        // Alt assets are off; ensure any prior patches using these names are reverted.
        ResourceMgr_UnpatchGfxByName(resource, dlist1);
        ResourceMgr_UnpatchGfxByName(resource, dlist2);
        if (dlist3 != NULL) {
            ResourceMgr_UnpatchGfxByName(resource, dlist3);
        }
        // Drop any cached version of the resource so it reloads clean (unpatched) next use.
        ResourceMgr_UnloadResource(resource);
        return;
    }

    if (!ResourceGetIsCustomByName(gfx)) {
        return;
    }

    if (alternateDL == NULL || ResourceGetIsCustomByName(alternateDL) || ResourceMgr_FileExists(alternateDL)) {
        ResourceMgr_PatchCustomGfxByName(resource, dlist1, 0, gsSPDisplayListOTRFilePath(gfx));
        if (dlist3 == NULL) {
            ResourceMgr_PatchCustomGfxByName(resource, dlist2, 1, gsSPEndDisplayList());
        } else {
            ResourceMgr_PatchCustomGfxByName(resource, dlist2, 1, gsSPDisplayListOTRFilePath(alternateDL));
        }
        if (dlist3 != NULL) {
            ResourceMgr_PatchCustomGfxByName(resource, dlist3, 2, gsSPEndDisplayList());
        }
    }
}

struct PatchEntry {
    const char* resource;
    const char* gfx;
    const char* dlist1;
    const char* dlist2;
    const char* dlist3;
    const char* alternateDL;
};

static void ApplyPatchEntries(std::initializer_list<PatchEntry> entries) {
    for (const auto& entry : entries) {
        PatchOrUnpatch(entry.resource, entry.gfx, entry.dlist1, entry.dlist2, entry.dlist3, entry.alternateDL);
    }
}

static void UnpatchGroup(const char* resource, std::initializer_list<const char*> dlistNames) {
    for (const char* name : dlistNames) {
        ResourceMgr_UnpatchGfxByName(resource, name);
    }
}

static void ApplySwordlessChildPatches() {
    ApplyPatchEntries({
        { gLinkChildDekuShieldWithMatrixDL, gCustomDekuShieldOnBackDL, "customChildShieldOnly1",
          "customChildShieldOnly2", nullptr, nullptr },
        { gLinkChildHylianShieldSwordAndSheathNearDL, gCustomHylianShieldOnChildBackDL, "customChildHylianShieldOnly1",
          "customChildHylianShieldOnly2", nullptr, nullptr },
        { gLinkAdultMirrorShieldSwordAndSheathNearDL, gCustomMirrorShieldOnBackDL, "customAdultMirrorOnly1",
          "customAdultMirrorOnly2", nullptr, nullptr },
    });

    UnpatchGroup(gLinkChildSwordAndSheathNearDL, { "customKokiriSwordSheath1", "customKokiriSwordSheath2" });
    UnpatchGroup(gLinkChildSheathNearDL, { "customKokiriSheath1", "customKokiriSheath2" });
    UnpatchGroup(gLinkChildDekuShieldSwordAndSheathNearDL,
                 { "customDekuShieldSword1", "customDekuShieldSword2", "customDekuShieldSword3" });
    UnpatchGroup(gLinkChildHylianShieldSwordAndSheathNearDL,
                 { "customChildHylianShieldSword1", "customChildHylianShieldSword2", "customChildHylianShieldSword3" });
}

static void ApplySwordlessAdultPatches() {
    ApplyPatchEntries({
        { gLinkAdultHylianShieldSwordAndSheathNearDL, gCustomHylianShieldOnBackDL, "customAdultShieldOnly1",
          "customAdultShieldOnly2", nullptr, nullptr },
        { gLinkAdultMirrorShieldSwordAndSheathNearDL, gCustomMirrorShieldOnBackDL, "customAdultMirrorOnly1",
          "customAdultMirrorOnly2", nullptr, nullptr },
        { gLinkChildDekuShieldSwordAndSheathNearDL, gCustomDekuShieldOnBackDL, "customDekuShieldSword1",
          "customDekuShieldSword2", nullptr, nullptr },
    });

    UnpatchGroup(gLinkAdultMasterSwordAndSheathNearDL, { "customMasterSwordSheath1", "customMasterSwordSheath2" });
}

static void ApplyKokiriSwordPatches() {
    ApplyPatchEntries({
        { gLinkChildSheathNearDL, gCustomKokiriSwordSheathDL, "customKokiriSheath1", "customKokiriSheath2", nullptr,
          nullptr },
        { gLinkChildSwordAndSheathNearDL, gCustomKokiriSwordInSheathDL, "customKokiriSwordSheath1",
          "customKokiriSwordSheath2", nullptr, nullptr },
        { gLinkChildDekuShieldSwordAndSheathNearDL, gCustomKokiriSwordInSheathDL, "customDekuShieldSword1",
          "customDekuShieldSword2", "customDekuShieldSword3", gCustomDekuShieldOnBackDL },
        { gLinkChildDekuShieldAndSheathNearDL, gCustomKokiriSwordSheathDL, "customDekuShieldSheath1",
          "customDekuShieldSheath2", "customDekuShieldSheath3", gCustomDekuShieldOnBackDL },
        { gLinkChildHylianShieldSwordAndSheathNearDL, gCustomKokiriSwordInSheathDL, "customChildHylianShieldSword1",
          "customChildHylianShieldSword2", "customChildHylianShieldSword3", gCustomHylianShieldOnChildBackDL },
        { gLinkChildHylianShieldAndSheathNearDL, gCustomKokiriSwordSheathDL, "customChildHylianShieldSheath1",
          "customChildHylianShieldSheath2", "customChildHylianShieldSheath3", gCustomHylianShieldOnChildBackDL },
        { gLinkAdultSheathNearDL, gCustomKokiriSwordSheathDL, "customSheath1", "customSheath2", nullptr, nullptr },
        { gLinkAdultHylianShieldSwordAndSheathNearDL, gCustomKokiriSwordInSheathDL, "customHylianShieldSword1",
          "customHylianShieldSword2", "customHylianShieldSword3", gCustomHylianShieldOnBackDL },
        { gLinkAdultMasterSwordAndSheathNearDL, gCustomKokiriSwordInSheathDL, "customMasterSwordSheath1",
          "customMasterSwordSheath2", nullptr, nullptr },
        { gLinkAdultHylianShieldAndSheathNearDL, gCustomKokiriSwordSheathDL, "customHylianShieldSheath1",
          "customHylianShieldSheath2", "customHylianShieldSheath3", gCustomHylianShieldOnBackDL },
        { gLinkAdultMirrorShieldSwordAndSheathNearDL, gCustomKokiriSwordInSheathDL, "customMirrorShieldSword1",
          "customMirrorShieldSword2", "customMirrorShieldSword3", gCustomMirrorShieldOnBackDL },
    });
}

static void ApplyMasterSwordPatches() {
    ApplyPatchEntries({
        { gLinkChildDekuShieldWithMatrixDL, gCustomMasterSwordInSheathDL, "customDekuShieldBack1",
          "customDekuShieldBack2", "customDekuShieldBack2", gCustomDekuShieldOnBackDL },
        { gLinkChildHylianShieldAndSheathNearDL, gCustomMasterSwordSheathDL, "customChildHylianShieldSheath1",
          "customChildHylianShieldSheath2", "customChildHylianShieldSheath3", gCustomHylianShieldOnChildBackDL },
        { gLinkChildSheathNearDL, gCustomMasterSwordSheathDL, "customKokiriSheath1", "customKokiriSheath2", nullptr,
          nullptr },
        { gLinkChildSwordAndSheathNearDL, gCustomMasterSwordInSheathDL, "customKokiriSwordSheath1",
          "customKokiriSwordSheath2", nullptr, nullptr },
        { gLinkChildDekuShieldSwordAndSheathNearDL, gCustomMasterSwordInSheathDL, "customDekuShieldSword1",
          "customDekuShieldSword2", "customDekuShieldSword3", gCustomDekuShieldOnBackDL },
        { gLinkChildDekuShieldAndSheathNearDL, gCustomMasterSwordSheathDL, "customDekuShieldSheath1",
          "customDekuShieldSheath2", "customDekuShieldSheath3", gCustomDekuShieldOnBackDL },
        { gLinkChildHylianShieldSwordAndSheathNearDL, gCustomMasterSwordInSheathDL, "customChildHylianShieldSword1",
          "customChildHylianShieldSword2", "customChildHylianShieldSword3", gCustomHylianShieldOnChildBackDL },
        { gLinkAdultSheathNearDL, gCustomMasterSwordSheathDL, "customSheath1", "customSheath2", nullptr, nullptr },
        { gLinkAdultMasterSwordAndSheathNearDL, gCustomMasterSwordInSheathDL, "customMasterSwordSheath1",
          "customMasterSwordSheath2", nullptr, nullptr },
        { gLinkAdultHylianShieldSwordAndSheathNearDL, gCustomMasterSwordInSheathDL, "customHylianShieldSword1",
          "customHylianShieldSword2", "customHylianShieldSword3", gCustomHylianShieldOnBackDL },
        { gLinkAdultHylianShieldAndSheathNearDL, gCustomMasterSwordSheathDL, "customHylianShieldSheath1",
          "customHylianShieldSheath2", "customHylianShieldSheath3", gCustomHylianShieldOnBackDL },
        { gLinkAdultMirrorShieldSwordAndSheathNearDL, gCustomMasterSwordInSheathDL, "customMirrorShieldSword1",
          "customMirrorShieldSword2", "customMirrorShieldSword3", gCustomMirrorShieldOnBackDL },
    });
}

static void ApplyBiggoronSwordPatches() {
    if (gPlayState != nullptr && GET_PLAYER(gPlayState)->sheathType == PLAYER_MODELTYPE_SHEATH_19) {
        PatchOrUnpatch(gLinkChildDekuShieldWithMatrixDL, gCustomLongswordSheathDL, "customDekuShieldBack1",
                       "customDekuShieldBack2", "customDekuShieldBack2", gCustomDekuShieldOnBackDL);
    } else {
        PatchOrUnpatch(gLinkChildDekuShieldWithMatrixDL, gCustomLongswordInSheathDL, "customDekuShieldBack1",
                       "customDekuShieldBack2", "customDekuShieldBack2", gCustomDekuShieldOnBackDL);
    }

    ApplyPatchEntries({
        { gLinkChildHylianShieldAndSheathNearDL, gCustomLongswordSheathDL, "customChildHylianShieldSheath1",
          "customChildHylianShieldSheath2", "customChildHylianShieldSheath3", gCustomHylianShieldOnChildBackDL },
        { gLinkChildDekuShieldAndSheathNearDL, gCustomLongswordSheathDL, "customDekuShieldSheath1",
          "customDekuShieldSheath2", "customDekuShieldSheath3", gCustomDekuShieldOnBackDL },
        { gLinkAdultLeftHandHoldingBgsNearDL, gCustomLongswordDL, "customBGS1", "customBGS2", "customBGS3",
          gLinkAdultLeftHandClosedNearDL },
        { gLinkAdultMasterSwordAndSheathNearDL, gCustomLongswordInSheathDL, "customMasterSwordSheath1",
          "customMasterSwordSheath2", nullptr, nullptr },
        { gLinkChildSheathNearDL, gCustomLongswordSheathDL, "customKokiriSheath1", "customKokiriSheath2", nullptr,
          nullptr },
        { gLinkChildSwordAndSheathNearDL, gCustomLongswordInSheathDL, "customKokiriSwordSheath1",
          "customKokiriSwordSheath2", nullptr, nullptr },
        { gLinkChildDekuShieldSwordAndSheathNearDL, gCustomLongswordInSheathDL, "customDekuShieldSword1",
          "customDekuShieldSword2", "customDekuShieldSword3", gCustomDekuShieldOnBackDL },
        { gLinkChildHylianShieldSwordAndSheathNearDL, gCustomLongswordInSheathDL, "customChildHylianShieldSword1",
          "customChildHylianShieldSword2", "customChildHylianShieldSword3", gCustomHylianShieldOnChildBackDL },
        { gLinkAdultSheathNearDL, gCustomLongswordSheathDL, "customSheath1", "customSheath2", nullptr, nullptr },
        { gLinkAdultHylianShieldSwordAndSheathNearDL, gCustomLongswordInSheathDL, "customHylianShieldSword1",
          "customHylianShieldSword2", "customHylianShieldSword3", gCustomHylianShieldOnBackDL },
        { gLinkAdultHylianShieldAndSheathNearDL, gCustomLongswordSheathDL, "customHylianShieldSheath1",
          "customHylianShieldSheath2", "customHylianShieldSheath3", gCustomHylianShieldOnBackDL },
        { gLinkAdultMirrorShieldSwordAndSheathNearDL, gCustomLongswordInSheathDL, "customMirrorShieldSword1",
          "customMirrorShieldSword2", "customMirrorShieldSword3", gCustomMirrorShieldOnBackDL },
        { gLinkAdultMirrorShieldAndSheathNearDL, gCustomLongswordSheathDL, "customMirrorShieldSheath1",
          "customMirrorShieldSheath2", "customMirrorShieldSheath3", gCustomMirrorShieldOnBackDL },
    });
}

static void ApplyBreakableLongswordPatches() {
    if (gPlayState != nullptr && GET_PLAYER(gPlayState)->sheathType == PLAYER_MODELTYPE_SHEATH_19) {
        PatchOrUnpatch(gLinkChildDekuShieldWithMatrixDL, GetBreakableLongswordSheathDL(), "customDekuShieldBack1",
                       "customDekuShieldBack2", "customDekuShieldBack2", gCustomDekuShieldOnBackDL);
    } else {
        PatchOrUnpatch(gLinkChildDekuShieldWithMatrixDL, GetBreakableLongswordInSheathDL(), "customDekuShieldBack1",
                       "customDekuShieldBack2", "customDekuShieldBack2", gCustomDekuShieldOnBackDL);
    }

    ApplyPatchEntries({
        { gLinkChildHylianShieldAndSheathNearDL, GetBreakableLongswordSheathDL(), "customChildHylianShieldSheath1",
          "customChildHylianShieldSheath2", "customChildHylianShieldSheath3", gCustomHylianShieldOnChildBackDL },
        { gLinkChildDekuShieldAndSheathNearDL, GetBreakableLongswordSheathDL(), "customDekuShieldSheath1",
          "customDekuShieldSheath2", "customDekuShieldSheath3", gCustomDekuShieldOnBackDL },
        { gLinkAdultLeftHandHoldingBgsNearDL, GetBreakableLongswordDL(), "customGK1", "customGK2", "customGK3",
          gLinkAdultLeftHandClosedNearDL },
        { gLinkAdultMasterSwordAndSheathNearDL, GetBreakableLongswordInSheathDL(), "customMasterSwordSheath1",
          "customMasterSwordSheath2", nullptr, nullptr },
        { gLinkChildSheathNearDL, GetBreakableLongswordSheathDL(), "customKokiriSheath1", "customKokiriSheath2",
          nullptr, nullptr },
        { gLinkChildSwordAndSheathNearDL, GetBreakableLongswordInSheathDL(), "customKokiriSwordSheath1",
          "customKokiriSwordSheath2", nullptr, nullptr },
        { gLinkChildDekuShieldSwordAndSheathNearDL, GetBreakableLongswordInSheathDL(), "customDekuShieldSword1",
          "customDekuShieldSword2", "customDekuShieldSword3", gCustomDekuShieldOnBackDL },
        { gLinkChildHylianShieldSwordAndSheathNearDL, GetBreakableLongswordInSheathDL(),
          "customChildHylianShieldSword1", "customChildHylianShieldSword2", "customChildHylianShieldSword3",
          gCustomHylianShieldOnChildBackDL },
        { gLinkAdultSheathNearDL, GetBreakableLongswordSheathDL(), "customSheath1", "customSheath2", nullptr, nullptr },
        { gLinkAdultHylianShieldSwordAndSheathNearDL, GetBreakableLongswordInSheathDL(), "customHylianShieldSword1",
          "customHylianShieldSword2", "customHylianShieldSword3", gCustomHylianShieldOnBackDL },
        { gLinkAdultHylianShieldAndSheathNearDL, GetBreakableLongswordSheathDL(), "customHylianShieldSheath1",
          "customHylianShieldSheath2", "customHylianShieldSheath3", gCustomHylianShieldOnBackDL },
        { gLinkAdultMirrorShieldSwordAndSheathNearDL, GetBreakableLongswordInSheathDL(), "customMirrorShieldSword1",
          "customMirrorShieldSword2", "customMirrorShieldSword3", gCustomMirrorShieldOnBackDL },
        { gLinkAdultMirrorShieldAndSheathNearDL, GetBreakableLongswordSheathDL(), "customMirrorShieldSheath1",
          "customMirrorShieldSheath2", "customMirrorShieldSheath3", gCustomMirrorShieldOnBackDL },
    });
}

static void ApplyBrokenKnifePatches() {
    if (gPlayState != nullptr && GET_PLAYER(gPlayState)->sheathType == PLAYER_MODELTYPE_SHEATH_19) {
        PatchOrUnpatch(gLinkChildDekuShieldWithMatrixDL, GetBrokenLongswordSheathDL(), "customDekuShieldBack1",
                       "customDekuShieldBack2", "customDekuShieldBack2", gCustomDekuShieldOnBackDL);
    } else {
        PatchOrUnpatch(gLinkChildDekuShieldWithMatrixDL, GetBrokenLongswordInSheathDL(), "customDekuShieldBack1",
                       "customDekuShieldBack2", "customDekuShieldBack2", gCustomDekuShieldOnBackDL);
    }

    ApplyPatchEntries({
        { gLinkChildHylianShieldAndSheathNearDL, GetBrokenLongswordSheathDL(), "customChildHylianShieldSheath1",
          "customChildHylianShieldSheath2", "customChildHylianShieldSheath3", gCustomHylianShieldOnChildBackDL },
        { gLinkChildDekuShieldAndSheathNearDL, GetBrokenLongswordSheathDL(), "customDekuShieldSheath1",
          "customDekuShieldSheath2", "customDekuShieldSheath3", gCustomDekuShieldOnBackDL },
        { gLinkAdultMasterSwordAndSheathNearDL, GetBrokenLongswordInSheathDL(), "customMasterSwordSheath1",
          "customMasterSwordSheath2", nullptr, nullptr },
        { gLinkChildSheathNearDL, GetBrokenLongswordSheathDL(), "customKokiriSheath1", "customKokiriSheath2", nullptr,
          nullptr },
        { gLinkChildSwordAndSheathNearDL, GetBrokenLongswordInSheathDL(), "customKokiriSwordSheath1",
          "customKokiriSwordSheath2", nullptr, nullptr },
        { gLinkChildDekuShieldSwordAndSheathNearDL, GetBrokenLongswordInSheathDL(), "customDekuShieldSword1",
          "customDekuShieldSword2", "customDekuShieldSword3", gCustomDekuShieldOnBackDL },
        { gLinkChildHylianShieldSwordAndSheathNearDL, GetBrokenLongswordInSheathDL(), "customChildHylianShieldSword1",
          "customChildHylianShieldSword2", "customChildHylianShieldSword3", gCustomHylianShieldOnChildBackDL },
        { gLinkAdultSheathNearDL, GetBrokenLongswordSheathDL(), "customSheath1", "customSheath2", nullptr, nullptr },
        { gLinkAdultHylianShieldSwordAndSheathNearDL, GetBrokenLongswordInSheathDL(), "customHylianShieldSword1",
          "customHylianShieldSword2", "customHylianShieldSword3", gCustomHylianShieldOnBackDL },
        { gLinkAdultHylianShieldAndSheathNearDL, GetBrokenLongswordSheathDL(), "customHylianShieldSheath1",
          "customHylianShieldSheath2", "customHylianShieldSheath3", gCustomHylianShieldOnBackDL },
        { gLinkAdultMirrorShieldSwordAndSheathNearDL, GetBrokenLongswordInSheathDL(), "customMirrorShieldSword1",
          "customMirrorShieldSword2", "customMirrorShieldSword3", gCustomMirrorShieldOnBackDL },
        { gLinkAdultMirrorShieldAndSheathNearDL, GetBrokenLongswordSheathDL(), "customMirrorShieldSheath1",
          "customMirrorShieldSheath2", "customMirrorShieldSheath3", gCustomMirrorShieldOnBackDL },
    });
}

static void ApplyCommonEquipmentPatches() {
    const bool isChild = LINK_IS_CHILD;
    const char* rightHandClosed = isChild ? gLinkChildRightHandClosedNearDL : gLinkAdultRightHandClosedNearDL;
    const char* leftHandClosed = isChild ? gLinkChildLeftFistNearDL : gLinkAdultLeftHandClosedNearDL;
    const char* fpsHand = isChild ? gCustomChildFPSHandDL : gCustomAdultFPSHandDL;
    const char* rightHandNear = isChild ? gLinkChildRightHandNearDL : gLinkAdultRightHandNearDL;

    ApplyPatchEntries({
        { gLinkAdultLeftHandHoldingMasterSwordNearDL, gCustomMasterSwordDL, "customMasterSword1", "customMasterSword2",
          "customMasterSword3", leftHandClosed },
        { gLinkAdultRightHandHoldingHylianShieldNearDL, gCustomHylianShieldDL, "customHylianShield1",
          "customHylianShield2", "customHylianShield3", rightHandClosed },
        { gLinkAdultRightHandHoldingMirrorShieldNearDL, gCustomMirrorShieldDL, "customMirrorShield1",
          "customMirrorShield2", "customMirrorShield3", rightHandClosed },
        { gLinkAdultHandHoldingBrokenGiantsKnifeDL, gCustomBrokenLongswordDL, "customBrokenBGS1", "customBrokenBGS2",
          "customBrokenBGS3", leftHandClosed },
        { gLinkChildLeftFistAndKokiriSwordNearDL, gCustomKokiriSwordDL, "customKokiriSword1", "customKokiriSword2",
          "customKokiriSword3", leftHandClosed },
        { gLinkChildRightFistAndDekuShieldNearDL, gCustomDekuShieldDL, "customDekuShield1", "customDekuShield2",
          "customDekuShield3", rightHandClosed },
    });

    if (INV_CONTENT(ITEM_HOOKSHOT) == ITEM_HOOKSHOT) {
        ApplyPatchEntries({
            { gLinkAdultRightHandHoldingHookshotNearDL, gCustomHookshotDL, "customHookshot1", "customHookshot2",
              "customHookshot3", rightHandClosed },
            { gLinkAdultRightHandHoldingHookshotFarDL, gCustomHookshotDL, "customHookshotFPS1", "customHookshotFPS2",
              "customHookshotFPS3", fpsHand },
        });
    }

    if (INV_CONTENT(ITEM_LONGSHOT) == ITEM_LONGSHOT) {
        ApplyPatchEntries({
            { gLinkAdultRightHandHoldingHookshotNearDL, gCustomLongshotDL, "customHookshot1", "customHookshot2",
              "customHookshot3", rightHandClosed },
            { gLinkAdultRightHandHoldingHookshotFarDL, gCustomLongshotDL, "customHookshotFPS1", "customHookshotFPS2",
              "customHookshotFPS3", fpsHand },
        });
    }

    ApplyPatchEntries({
        { gLinkAdultHookshotTipDL, gCustomHookshotTipDL, "customHookshotTip1", "customHookshotTip2", nullptr, nullptr },
        { gLinkAdultHookshotChainDL, gCustomHookshotChainDL, "customHookshotChain1", "customHookshotChain2", nullptr,
          nullptr },
    });

    if (INV_CONTENT(ITEM_OCARINA_FAIRY) == ITEM_OCARINA_FAIRY) {
        ApplyPatchEntries({
            { gLinkAdultRightHandHoldingOotNearDL, isChild ? gCustomFairyOcarinaDL : gCustomFairyOcarinaAdultDL,
              "customOcarina1", "customOcarina2", "customOcarina3", rightHandNear },
        });
    }

    if (INV_CONTENT(ITEM_OCARINA_TIME) == ITEM_OCARINA_TIME) {
        ApplyPatchEntries({
            { gLinkAdultRightHandHoldingOotNearDL, isChild ? gCustomOcarinaOfTimeDL : gCustomOcarinaOfTimeAdultDL,
              "customOcarina1", "customOcarina2", "customOcarina3", rightHandNear },
        });
    }

    ApplyPatchEntries({
        { gLinkChildRightHandHoldingFairyOcarinaNearDL, gCustomFairyOcarinaDL, "customFairyOcarina1",
          "customFairyOcarina2", "customFairyOcarina3", rightHandNear },
        { gLinkChildRightHandAndOotNearDL, gCustomOcarinaOfTimeDL, "customChildOcarina1", "customChildOcarina2",
          "customChildOcarina3", rightHandNear },
        { gLinkAdultRightHandHoldingBowNearDL, gCustomBowDL, "customBow1", "customBow2", "customBow3",
          rightHandClosed },
        { gLinkAdultRightHandHoldingBowFirstPersonDL, gCustomBowDL, "customBowFPS1", "customBowFPS2", "customBowFPS3",
          fpsHand },
        { gLinkAdultLeftHandHoldingHammerNearDL, gCustomHammerDL, "customHammer1", "customHammer2", "customHammer3",
          leftHandClosed },
        { gLinkChildLeftFistAndBoomerangNearDL, gCustomBoomerangDL, "customBoomerang1", "customBoomerang2",
          "customBoomerang3", leftHandClosed },
        { gLinkChildRightHandHoldingSlingshotNearDL, gCustomSlingshotDL, "customSlingshot1", "customSlingshot2",
          "customSlingshot3", rightHandClosed },
        { gLinkChildRightArmStretchedSlingshotDL, gCustomSlingshotDL, "customSlingshotFPS1", "customSlingshotFPS2",
          "customSlingshotFPS3", fpsHand },
    });

    ApplyPatchEntries({
        { gLinkChildRightHandHoldingFairyOcarinaNearDL, gCustomFairyOcarinaDL, "customFairyOcarina1",
          "customFairyOcarina2", "customFairyOcarina3", rightHandNear },
        { gLinkChildRightHandAndOotNearDL, gCustomOcarinaOfTimeDL, "customChildOcarina1", "customChildOcarina2",
          "customChildOcarina3", rightHandNear },
        { gLinkChildLeftFistAndBoomerangNearDL, gCustomBoomerangDL, "customBoomerang1", "customBoomerang2",
          "customBoomerang3", leftHandClosed },
        { gLinkChildRightHandHoldingSlingshotNearDL, gCustomSlingshotDL, "customSlingshot1", "customSlingshot2",
          "customSlingshot3", rightHandClosed },
        { gLinkChildRightArmStretchedSlingshotDL, gCustomSlingshotDL, "customSlingshotFPS1", "customSlingshotFPS2",
          "customSlingshotFPS3", fpsHand },
    });

    const bool equipmentAlwaysVisible = CVarGetInteger(CVAR_ENHANCEMENT("EquipmentAlwaysVisible"), 0) != 0;

    if (LINK_IS_CHILD && equipmentAlwaysVisible) {
        ApplyPatchEntries({
            { gCustomAdultFPSHandDL, gCustomChildFPSHandDL, "patchChildFPSHand1", "patchChildFPSHand2", nullptr,
              nullptr },
            { gLinkAdultRightHandClosedNearDL, gLinkChildRightHandClosedNearDL, "customChildRightHand1",
              "customChildRightHand2", nullptr, nullptr },
            { gLinkAdultLeftHandClosedNearDL, gLinkChildLeftFistNearDL, "customChildLeftHand1", "customChildLeftHand2",
              nullptr, nullptr },
        });
    }

    if (LINK_IS_ADULT && equipmentAlwaysVisible) {
        ApplyPatchEntries({
            { gCustomChildFPSHandDL, gCustomAdultFPSHandDL, "patchAdultFPSHand1", "patchAdultFPSHand2", nullptr,
              nullptr },
            { gLinkChildRightHandClosedNearDL, gLinkAdultRightHandClosedNearDL, "customAdultRightHand1",
              "customAdultRightHand2", nullptr, nullptr },
            { gLinkChildLeftFistNearDL, gLinkAdultLeftHandClosedNearDL, "customAdultLeftHand1", "customAdultLeftHand2",
              nullptr, nullptr },
        });
    }
}

void UpdatePatchCustomEquipmentDlists() {
    const u8 equippedSword = gSaveContext.equips.buttonItems[0];

    if (equippedSword == ITEM_NONE) {
        if (LINK_IS_CHILD) {
            ApplySwordlessChildPatches();
        }
        if (LINK_IS_ADULT) {
            ApplySwordlessAdultPatches();
        }
    }

    switch (equippedSword) {
        case ITEM_SWORD_KOKIRI:
            ApplyKokiriSwordPatches();
            break;
        case ITEM_SWORD_MASTER:
            ApplyMasterSwordPatches();
            break;
        case ITEM_SWORD_BGS:
            if (gSaveContext.bgsFlag) {
                ApplyBiggoronSwordPatches();
            } else {
                ApplyBreakableLongswordPatches();
            }
            break;
        case ITEM_SWORD_KNIFE:
            ApplyBrokenKnifePatches();
            break;
        default:
            break;
    }

    ApplyCommonEquipmentPatches();
}

static bool HasDummyPlayers() {
    if (gPlayState == nullptr) {
        return false;
    }

    Actor* actor = gPlayState->actorCtx.actorLists[ACTORCAT_NPC].head;
    while (actor != nullptr) {
        if (actor->id == ACTOR_EN_OE2 && actor->update == DummyPlayer_Update) {
            return true;
        }
        actor = actor->next;
    }

    return false;
}
