#include <initializer_list>

#include "src/overlays/actors/ovl_En_Elf/z_en_elf.h"
#include "global.h"
#include "objects/object_link_boy/object_link_boy.h"
#include "objects/object_link_child/object_link_child.h"
#include "objects/object_custom_equip/object_custom_equip.h"
#include "soh/Enhancements/game-interactor/GameInteractor.h"
#include "soh/ShipInit.hpp"
#include "soh/ResourceManagerHelpers.h"
#include "soh_assets.h"
#include "z64player.h"
#include "kaleido.h"

extern SaveContext gSaveContext;
extern PlayState* gPlayState;
extern void Overlay_DisplayText(float duration, const char* text);

static void UpdatePatchCustomEquipmentDlists();
static void UpdatePatchHand();
static void RefreshCustomEquipment();

static void UpdateCustomEquipmentSetModel(u8 ModelGroup) {
    (void)ModelGroup;
    RefreshCustomEquipment();
}

static void UpdateCustomEquipment() {
    RefreshCustomEquipment();
}

static void PatchCustomEquipment() {
    COND_HOOK(OnPlayerSetModels, true, UpdateCustomEquipmentSetModel);
    COND_HOOK(OnSceneSpawnActors, true, UpdateCustomEquipment); // To be changed when kaleido hook is made
    // COND_HOOK(OnLinkSkeletonInit, true, UpdateCustomEquipment); //To be added once custom tunic fix is pulled
    COND_HOOK(OnAssetAltChange, true, UpdateCustomEquipment);
}

static RegisterShipInitFunc initFunc(PatchCustomEquipment);

static void RefreshCustomEquipment() {
    if (!GameInteractor::IsSaveLoaded() || gPlayState == NULL) {
        return;
    }

    UpdatePatchHand();
    UpdatePatchCustomEquipmentDlists();
}

void UpdatePatchHand() {
    const bool equipmentAlwaysVisible = CVarGetInteger(CVAR_ENHANCEMENT("EquipmentAlwaysVisible"), 0) != 0;
    const bool fixHammerHand = CVarGetInteger("gEnhancements.FixHammerHand", 0) != 0;

    if (equipmentAlwaysVisible && LINK_IS_CHILD) {
        ResourceMgr_PatchGfxByName(gLinkAdultLeftHandHoldingHammerNearDL, "childHammer1", 92,
                                   gsSPDisplayListOTRFilePath(gLinkChildLeftFistNearDL));
        ResourceMgr_PatchGfxByName(gLinkAdultLeftHandHoldingHammerNearDL, "childHammer2", 93, gsSPEndDisplayList());
        ResourceMgr_PatchGfxByName(gLinkAdultRightHandHoldingHookshotNearDL, "childHookshot1", 84,
                                   gsSPDisplayListOTRFilePath(gLinkChildRightHandClosedNearDL));
        ResourceMgr_PatchGfxByName(gLinkAdultRightHandHoldingHookshotNearDL, "childHookshot2", 85,
                                   gsSPEndDisplayList());
        ResourceMgr_PatchGfxByName(gLinkAdultRightHandHoldingBowNearDL, "childBow1", 51,
                                   gsSPDisplayListOTRFilePath(gLinkChildRightHandClosedNearDL));
        ResourceMgr_PatchGfxByName(gLinkAdultRightHandHoldingBowNearDL, "childBow2", 52, gsSPEndDisplayList());
        ResourceMgr_PatchGfxByName(gLinkAdultLeftHandHoldingMasterSwordNearDL, "childMasterSword1", 104,
                                   gsSPDisplayListOTRFilePath(gLinkChildLeftFistNearDL));
        ResourceMgr_PatchGfxByName(gLinkAdultLeftHandHoldingMasterSwordNearDL, "childMasterSword2", 105,
                                   gsSPEndDisplayList());
        ResourceMgr_PatchGfxByName(gLinkAdultLeftHandHoldingBgsNearDL, "childBiggoronSword1", 79,
                                   gsSPDisplayListOTRFilePath(gLinkChildLeftFistNearDL));
        ResourceMgr_PatchGfxByName(gLinkAdultLeftHandHoldingBgsNearDL, "childBiggoronSword2", 80, gsSPEndDisplayList());
        ResourceMgr_PatchGfxByName(gLinkAdultHandHoldingBrokenGiantsKnifeDL, "childBrokenGiantsKnife1", 76,
                                   gsSPDisplayListOTRFilePath(gLinkChildLeftFistNearDL));
        ResourceMgr_PatchGfxByName(gLinkAdultHandHoldingBrokenGiantsKnifeDL, "childBrokenGiantsKnife2", 77,
                                   gsSPEndDisplayList());
    } else {
        ResourceMgr_UnpatchGfxByName(gLinkAdultLeftHandHoldingHammerNearDL, "childHammer1");
        ResourceMgr_UnpatchGfxByName(gLinkAdultLeftHandHoldingHammerNearDL, "childHammer2");
        ResourceMgr_UnpatchGfxByName(gLinkAdultRightHandHoldingHookshotNearDL, "childHookshot1");
        ResourceMgr_UnpatchGfxByName(gLinkAdultRightHandHoldingHookshotNearDL, "childHookshot2");
        ResourceMgr_UnpatchGfxByName(gLinkAdultRightHandHoldingBowNearDL, "childBow1");
        ResourceMgr_UnpatchGfxByName(gLinkAdultRightHandHoldingBowNearDL, "childBow2");
        ResourceMgr_UnpatchGfxByName(gLinkAdultLeftHandHoldingMasterSwordNearDL, "childMasterSword1");
        ResourceMgr_UnpatchGfxByName(gLinkAdultLeftHandHoldingMasterSwordNearDL, "childMasterSword2");
        ResourceMgr_UnpatchGfxByName(gLinkAdultLeftHandHoldingBgsNearDL, "childBiggoronSword1");
        ResourceMgr_UnpatchGfxByName(gLinkAdultLeftHandHoldingBgsNearDL, "childBiggoronSword2");
        ResourceMgr_UnpatchGfxByName(gLinkAdultHandHoldingBrokenGiantsKnifeDL, "childBrokenGiantsKnife1");
        ResourceMgr_UnpatchGfxByName(gLinkAdultHandHoldingBrokenGiantsKnifeDL, "childBrokenGiantsKnife2");
    }
    if (equipmentAlwaysVisible && LINK_IS_ADULT) {
        ResourceMgr_PatchGfxByName(gLinkChildLeftFistAndKokiriSwordNearDL, "adultKokiriSword", 13,
                                   gsSPDisplayListOTRFilePath(gLinkAdultLeftHandClosedNearDL));
        ResourceMgr_PatchGfxByName(gLinkChildRightHandHoldingSlingshotNearDL, "adultSlingshot", 13,
                                   gsSPDisplayListOTRFilePath(gLinkAdultRightHandClosedNearDL));
        ResourceMgr_PatchGfxByName(gLinkChildLeftFistAndBoomerangNearDL, "adultBoomerang", 50,
                                   gsSPDisplayListOTRFilePath(gLinkAdultLeftHandClosedNearDL));
        ResourceMgr_PatchGfxByName(gLinkChildRightFistAndDekuShieldNearDL, "adultDekuShield", 49,
                                   gsSPDisplayListOTRFilePath(gLinkAdultRightHandClosedNearDL));
    } else {
        ResourceMgr_UnpatchGfxByName(gLinkChildLeftFistAndKokiriSwordNearDL, "adultKokiriSword");
        ResourceMgr_UnpatchGfxByName(gLinkChildRightHandHoldingSlingshotNearDL, "adultSlingshot");
        ResourceMgr_UnpatchGfxByName(gLinkChildLeftFistAndBoomerangNearDL, "adultBoomerang");
        ResourceMgr_UnpatchGfxByName(gLinkChildRightFistAndDekuShieldNearDL, "adultDekuShield");
    }
    if (fixHammerHand && LINK_IS_ADULT) {
        ResourceMgr_PatchGfxByName(gLinkAdultLeftHandHoldingHammerNearDL, "hammerHand1", 92,
                                   gsSPDisplayListOTRFilePath(gLinkAdultLeftHandClosedNearDL));
        ResourceMgr_PatchGfxByName(gLinkAdultLeftHandHoldingHammerNearDL, "hammerHand2", 93, gsSPEndDisplayList());
    } else {
        ResourceMgr_UnpatchGfxByName(gLinkAdultLeftHandHoldingHammerNearDL, "hammerHand1");
        ResourceMgr_UnpatchGfxByName(gLinkAdultLeftHandHoldingHammerNearDL, "hammerHand2");
    }
}

void PatchOrUnpatch(const char* resource, const char* gfx, const char* dlist1, const char* dlist2, const char* dlist3,
                    const char* alternateDL) {
    if (resource == NULL || gfx == NULL || dlist1 == NULL || dlist2 == NULL) {
        return;
    }

    if (CVarGetInteger(CVAR_ENHANCEMENT("AltAssets"), 0)) {
        if (ResourceGetIsCustomByName(gfx)) {
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
    } else {
        ResourceMgr_UnpatchGfxByName(resource, dlist1);
        ResourceMgr_UnpatchGfxByName(resource, dlist2);
        if (dlist3 != NULL) {
            ResourceMgr_UnpatchGfxByName(resource, dlist3);
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
    ApplyPatchEntries({
        { gLinkChildDekuShieldWithMatrixDL, gCustomLongswordSheathDL, "customDekuShieldBack1", "customDekuShieldBack2",
          "customDekuShieldBack2", gCustomDekuShieldOnBackDL },
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
        { gLinkChildDekuShieldWithMatrixDL, gCustomLongswordInSheathDL, "customDekuShieldBack1",
          "customDekuShieldBack2", "customDekuShieldBack2", gCustomDekuShieldOnBackDL },
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
    ApplyPatchEntries({
        { gLinkChildDekuShieldWithMatrixDL, gCustomBreakableLongswordSheathDL, "customDekuShieldBack1",
          "customDekuShieldBack2", "customDekuShieldBack2", gCustomDekuShieldOnBackDL },
        { gLinkChildHylianShieldAndSheathNearDL, gCustomBreakableLongswordSheathDL, "customChildHylianShieldSheath1",
          "customChildHylianShieldSheath2", "customChildHylianShieldSheath3", gCustomHylianShieldOnChildBackDL },
        { gLinkChildDekuShieldAndSheathNearDL, gCustomBreakableLongswordSheathDL, "customDekuShieldSheath1",
          "customDekuShieldSheath2", "customDekuShieldSheath3", gCustomDekuShieldOnBackDL },
        { gLinkAdultLeftHandHoldingBgsNearDL, gCustomBreakableLongswordDL, "customGK1", "customGK2", "customGK3",
          gLinkAdultLeftHandClosedNearDL },
        { gLinkAdultMasterSwordAndSheathNearDL, gCustomBreakableLongswordInSheathDL, "customMasterSwordSheath1",
          "customMasterSwordSheath2", nullptr, nullptr },
        { gLinkChildSheathNearDL, gCustomBreakableLongswordSheathDL, "customKokiriSheath1", "customKokiriSheath2",
          nullptr, nullptr },
        { gLinkChildSwordAndSheathNearDL, gCustomBreakableLongswordInSheathDL, "customKokiriSwordSheath1",
          "customKokiriSwordSheath2", nullptr, nullptr },
        { gLinkChildDekuShieldSwordAndSheathNearDL, gCustomBreakableLongswordInSheathDL, "customDekuShieldSword1",
          "customDekuShieldSword2", "customDekuShieldSword3", gCustomDekuShieldOnBackDL },
        { gLinkChildDekuShieldWithMatrixDL, gCustomBreakableLongswordInSheathDL, "customDekuShieldBack1",
          "customDekuShieldBack2", "customDekuShieldBack2", gCustomDekuShieldOnBackDL },
        { gLinkChildHylianShieldSwordAndSheathNearDL, gCustomBreakableLongswordInSheathDL,
          "customChildHylianShieldSword1", "customChildHylianShieldSword2", "customChildHylianShieldSword3",
          gCustomHylianShieldOnChildBackDL },
        { gLinkAdultSheathNearDL, gCustomBreakableLongswordSheathDL, "customSheath1", "customSheath2", nullptr,
          nullptr },
        { gLinkAdultHylianShieldSwordAndSheathNearDL, gCustomBreakableLongswordInSheathDL, "customHylianShieldSword1",
          "customHylianShieldSword2", "customHylianShieldSword3", gCustomHylianShieldOnBackDL },
        { gLinkAdultHylianShieldAndSheathNearDL, gCustomBreakableLongswordSheathDL, "customHylianShieldSheath1",
          "customHylianShieldSheath2", "customHylianShieldSheath3", gCustomHylianShieldOnBackDL },
        { gLinkAdultMirrorShieldSwordAndSheathNearDL, gCustomBreakableLongswordInSheathDL, "customMirrorShieldSword1",
          "customMirrorShieldSword2", "customMirrorShieldSword3", gCustomMirrorShieldOnBackDL },
        { gLinkAdultMirrorShieldAndSheathNearDL, gCustomBreakableLongswordSheathDL, "customMirrorShieldSheath1",
          "customMirrorShieldSheath2", "customMirrorShieldSheath3", gCustomMirrorShieldOnBackDL },
    });
}

static void ApplyBrokenKnifePatches() {
    ApplyPatchEntries({
        { gLinkChildDekuShieldWithMatrixDL, gCustomBrokenLongswordSheathDL, "customDekuShieldBack1",
          "customDekuShieldBack2", "customDekuShieldBack2", gCustomDekuShieldOnBackDL },
        { gLinkChildHylianShieldAndSheathNearDL, gCustomBrokenLongswordSheathDL, "customChildHylianShieldSheath1",
          "customChildHylianShieldSheath2", "customChildHylianShieldSheath3", gCustomHylianShieldOnChildBackDL },
        { gLinkChildDekuShieldAndSheathNearDL, gCustomBrokenLongswordSheathDL, "customDekuShieldSheath1",
          "customDekuShieldSheath2", "customDekuShieldSheath3", gCustomDekuShieldOnBackDL },
        { gLinkAdultMasterSwordAndSheathNearDL, gCustomBrokenLongswordInSheathDL, "customMasterSwordSheath1",
          "customMasterSwordSheath2", nullptr, nullptr },
        { gLinkChildSheathNearDL, gCustomBrokenLongswordSheathDL, "customKokiriSheath1", "customKokiriSheath2", nullptr,
          nullptr },
        { gLinkChildSwordAndSheathNearDL, gCustomBrokenLongswordInSheathDL, "customKokiriSwordSheath1",
          "customKokiriSwordSheath2", nullptr, nullptr },
        { gLinkChildDekuShieldSwordAndSheathNearDL, gCustomBrokenLongswordInSheathDL, "customDekuShieldSword1",
          "customDekuShieldSword2", "customDekuShieldSword3", gCustomDekuShieldOnBackDL },
        { gLinkChildDekuShieldWithMatrixDL, gCustomBrokenLongswordInSheathDL, "customDekuShieldBack1",
          "customDekuShieldBack2", "customDekuShieldBack2", gCustomDekuShieldOnBackDL },
        { gLinkChildHylianShieldSwordAndSheathNearDL, gCustomBrokenLongswordInSheathDL, "customChildHylianShieldSword1",
          "customChildHylianShieldSword2", "customChildHylianShieldSword3", gCustomHylianShieldOnChildBackDL },
        { gLinkAdultSheathNearDL, gCustomBrokenLongswordSheathDL, "customSheath1", "customSheath2", nullptr, nullptr },
        { gLinkAdultHylianShieldSwordAndSheathNearDL, gCustomBrokenLongswordInSheathDL, "customHylianShieldSword1",
          "customHylianShieldSword2", "customHylianShieldSword3", gCustomHylianShieldOnBackDL },
        { gLinkAdultHylianShieldAndSheathNearDL, gCustomBrokenLongswordSheathDL, "customHylianShieldSheath1",
          "customHylianShieldSheath2", "customHylianShieldSheath3", gCustomHylianShieldOnBackDL },
        { gLinkAdultMirrorShieldSwordAndSheathNearDL, gCustomBrokenLongswordInSheathDL, "customMirrorShieldSword1",
          "customMirrorShieldSword2", "customMirrorShieldSword3", gCustomMirrorShieldOnBackDL },
        { gLinkAdultMirrorShieldAndSheathNearDL, gCustomBrokenLongswordSheathDL, "customMirrorShieldSheath1",
          "customMirrorShieldSheath2", "customMirrorShieldSheath3", gCustomMirrorShieldOnBackDL },
    });
}

static void ApplyCommonEquipmentPatches() {
    ApplyPatchEntries({
        { gLinkAdultLeftHandHoldingMasterSwordNearDL, gCustomMasterSwordDL, "customMasterSword1", "customMasterSword2",
          "customMasterSword3", gLinkAdultLeftHandClosedNearDL },
        { gLinkAdultRightHandHoldingHylianShieldNearDL, gCustomHylianShieldDL, "customHylianShield1",
          "customHylianShield2", "customHylianShield3", gLinkAdultRightHandClosedNearDL },
        { gLinkAdultRightHandHoldingMirrorShieldNearDL, gCustomMirrorShieldDL, "customMirrorShield1",
          "customMirrorShield2", "customMirrorShield3", gLinkAdultRightHandClosedNearDL },
        { gLinkAdultHandHoldingBrokenGiantsKnifeDL, gCustomBrokenLongswordDL, "customBrokenBGS1", "customBrokenBGS2",
          "customBrokenBGS3", gLinkAdultLeftHandClosedNearDL },
        { gLinkChildLeftFistAndKokiriSwordNearDL, gCustomKokiriSwordDL, "customKokiriSword1", "customKokiriSword2",
          "customKokiriSword3", gLinkChildLeftFistNearDL },
        { gLinkChildRightFistAndDekuShieldNearDL, gCustomDekuShieldDL, "customDekuShield1", "customDekuShield2",
          "customDekuShield3", gLinkChildRightHandClosedNearDL },
    });

    if (INV_CONTENT(ITEM_HOOKSHOT) == ITEM_HOOKSHOT) {
        ApplyPatchEntries({
            { gLinkAdultRightHandHoldingHookshotNearDL, gCustomHookshotDL, "customHookshot1", "customHookshot2",
              "customHookshot3", gLinkAdultRightHandClosedNearDL },
            { gLinkAdultRightHandHoldingHookshotFarDL, gCustomHookshotDL, "customHookshotFPS1", "customHookshotFPS2",
              "customHookshotFPS3", gCustomAdultFPSHandDL },
        });
    }

    if (INV_CONTENT(ITEM_LONGSHOT) == ITEM_LONGSHOT) {
        ApplyPatchEntries({
            { gLinkAdultRightHandHoldingHookshotNearDL, gCustomLongshotDL, "customHookshot1", "customHookshot2",
              "customHookshot3", gLinkAdultRightHandClosedNearDL },
            { gLinkAdultRightHandHoldingHookshotFarDL, gCustomLongshotDL, "customHookshotFPS1", "customHookshotFPS2",
              "customHookshotFPS3", gCustomAdultFPSHandDL },
        });
    }

    ApplyPatchEntries({
        { gLinkAdultHookshotTipDL, gCustomHookshotTipDL, "customHookshotTip1", "customHookshotTip2", nullptr, nullptr },
        { gLinkAdultHookshotChainDL, gCustomHookshotChainDL, "customHookshotChain1", "customHookshotChain2", nullptr,
          nullptr },
    });

    if (INV_CONTENT(ITEM_OCARINA_FAIRY) == ITEM_OCARINA_FAIRY) {
        ApplyPatchEntries({
            { gLinkAdultRightHandHoldingOotNearDL, gCustomFairyOcarinaAdultDL, "customOcarina1", "customOcarina2",
              "customOcarina3", gLinkAdultRightHandNearDL },
        });
    }

    if (INV_CONTENT(ITEM_OCARINA_TIME) == ITEM_OCARINA_TIME) {
        ApplyPatchEntries({
            { gLinkAdultRightHandHoldingOotNearDL, gCustomOcarinaOfTimeAdultDL, "customOcarina1", "customOcarina2",
              "customOcarina3", gLinkAdultRightHandNearDL },
        });
    }

    ApplyPatchEntries({
        { gLinkChildRightHandHoldingFairyOcarinaNearDL, gCustomFairyOcarinaDL, "customFairyOcarina1",
          "customFairyOcarina2", "customFairyOcarina3", gLinkChildRightHandNearDL },
        { gLinkChildRightHandAndOotNearDL, gCustomOcarinaOfTimeDL, "customChildOcarina1", "customChildOcarina2",
          "customChildOcarina3", gLinkChildRightHandNearDL },
        { gLinkAdultRightHandHoldingBowNearDL, gCustomBowDL, "customBow1", "customBow2", "customBow3",
          gLinkAdultRightHandClosedNearDL },
        { gLinkAdultRightHandHoldingBowFirstPersonDL, gCustomBowDL, "customBowFPS1", "customBowFPS2", "customBowFPS3",
          gCustomAdultFPSHandDL },
        { gLinkAdultLeftHandHoldingHammerNearDL, gCustomHammerDL, "customHammer1", "customHammer2", "customHammer3",
          gLinkAdultLeftHandClosedNearDL },
        { gLinkChildLeftFistAndBoomerangNearDL, gCustomBoomerangDL, "customBoomerang1", "customBoomerang2",
          "customBoomerang3", gLinkChildLeftFistNearDL },
        { gLinkChildRightHandHoldingSlingshotNearDL, gCustomSlingshotDL, "customSlingshot1", "customSlingshot2",
          "customSlingshot3", gLinkChildRightHandClosedNearDL },
        { gLinkChildRightArmStretchedSlingshotDL, gCustomSlingshotDL, "customSlingshotFPS1", "customSlingshotFPS2",
          "customSlingshotFPS3", gCustomChildFPSHandDL },
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
