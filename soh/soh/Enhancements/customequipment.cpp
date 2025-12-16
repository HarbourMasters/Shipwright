#include "src/overlays/actors/ovl_En_Elf/z_en_elf.h"
#include "objects/object_link_boy/object_link_boy.h"
#include "objects/object_link_child/object_link_child.h"
#include "objects/object_custom_equip/object_custom_equip.h"
#include "soh/Enhancements/game-interactor/GameInteractor.h"
#include "soh/Enhancements/customequipment.h"
#include "soh/ShipInit.hpp"
#include "soh/ResourceManagerHelpers.h"
#include "soh_assets.h"
#include "kaleido.h"

uint8_t Player_IsCustomLinkModel();

extern SaveContext gSaveContext;
extern PlayState* gPlayState;
extern void Overlay_DisplayText(float duration, const char* text);

void UpdatePatchHand() {
    if ((CVarGetInteger(CVAR_ENHANCEMENT("EquipmentAlwaysVisible"), 0)) && LINK_IS_CHILD) {
        ResourceMgr_PatchGfxByName(gLinkAdultLeftHandHoldingHammerNearDL, "childHammer1", 92, gsSPDisplayListOTRFilePath(gLinkChildLeftFistNearDL));
        ResourceMgr_PatchGfxByName(gLinkAdultLeftHandHoldingHammerNearDL, "childHammer2", 93, gsSPEndDisplayList());
        ResourceMgr_PatchGfxByName(gLinkAdultRightHandHoldingHookshotNearDL, "childHookshot1", 84, gsSPDisplayListOTRFilePath(gLinkChildRightHandClosedNearDL));
        ResourceMgr_PatchGfxByName(gLinkAdultRightHandHoldingHookshotNearDL, "childHookshot2", 85, gsSPEndDisplayList());
        ResourceMgr_PatchGfxByName(gLinkAdultRightHandHoldingBowNearDL, "childBow1", 51, gsSPDisplayListOTRFilePath(gLinkChildRightHandClosedNearDL));
        ResourceMgr_PatchGfxByName(gLinkAdultRightHandHoldingBowNearDL, "childBow2", 52, gsSPEndDisplayList());
        ResourceMgr_PatchGfxByName(gLinkAdultLeftHandHoldingMasterSwordNearDL, "childMasterSword1", 104, gsSPDisplayListOTRFilePath(gLinkChildLeftFistNearDL));
        ResourceMgr_PatchGfxByName(gLinkAdultLeftHandHoldingMasterSwordNearDL, "childMasterSword2", 105, gsSPEndDisplayList());
        ResourceMgr_PatchGfxByName(gLinkAdultLeftHandHoldingBgsNearDL, "childBiggoronSword1", 79, gsSPDisplayListOTRFilePath(gLinkChildLeftFistNearDL));
        ResourceMgr_PatchGfxByName(gLinkAdultLeftHandHoldingBgsNearDL, "childBiggoronSword2", 80, gsSPEndDisplayList());
        ResourceMgr_PatchGfxByName(gLinkAdultHandHoldingBrokenGiantsKnifeDL, "childBrokenGiantsKnife1", 76, gsSPDisplayListOTRFilePath(gLinkChildLeftFistNearDL));
        ResourceMgr_PatchGfxByName(gLinkAdultHandHoldingBrokenGiantsKnifeDL, "childBrokenGiantsKnife2", 77, gsSPEndDisplayList());
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
    if ((CVarGetInteger(CVAR_ENHANCEMENT("EquipmentAlwaysVisible"), 0)) && LINK_IS_ADULT) {
        ResourceMgr_PatchGfxByName(gLinkChildLeftFistAndKokiriSwordNearDL, "adultKokiriSword", 13, gsSPDisplayListOTRFilePath(gLinkAdultLeftHandClosedNearDL));
        ResourceMgr_PatchGfxByName(gLinkChildRightHandHoldingSlingshotNearDL, "adultSlingshot", 13, gsSPDisplayListOTRFilePath(gLinkAdultRightHandClosedNearDL));
        ResourceMgr_PatchGfxByName(gLinkChildLeftFistAndBoomerangNearDL, "adultBoomerang", 50, gsSPDisplayListOTRFilePath(gLinkAdultLeftHandClosedNearDL));
        ResourceMgr_PatchGfxByName(gLinkChildRightFistAndDekuShieldNearDL, "adultDekuShield", 49, gsSPDisplayListOTRFilePath(gLinkAdultRightHandClosedNearDL));
    } else {
        ResourceMgr_UnpatchGfxByName(gLinkChildLeftFistAndKokiriSwordNearDL, "adultKokiriSword");
        ResourceMgr_UnpatchGfxByName(gLinkChildRightHandHoldingSlingshotNearDL, "adultSlingshot");
        ResourceMgr_UnpatchGfxByName(gLinkChildLeftFistAndBoomerangNearDL, "adultBoomerang");
        ResourceMgr_UnpatchGfxByName(gLinkChildRightFistAndDekuShieldNearDL, "adultDekuShield");
    }
    if (CVarGetInteger("gEnhancements.FixHammerHand", 0) && LINK_IS_ADULT) {
        ResourceMgr_PatchGfxByName(gLinkAdultLeftHandHoldingHammerNearDL, "hammerHand1", 92, gsSPDisplayListOTRFilePath(gLinkAdultLeftHandClosedNearDL));
        ResourceMgr_PatchGfxByName(gLinkAdultLeftHandHoldingHammerNearDL, "hammerHand2", 93, gsSPEndDisplayList());
    } else {
        ResourceMgr_UnpatchGfxByName(gLinkAdultLeftHandHoldingHammerNearDL, "hammerHand1");
        ResourceMgr_UnpatchGfxByName(gLinkAdultLeftHandHoldingHammerNearDL, "hammerHand2");
    }
}

void RegisterPatchHandHandler() {
    GameInteractor::Instance->RegisterGameHook<GameInteractor::OnSceneSpawnActors>([]() { 
        UpdatePatchHand(); 
    });
}

void PatchOrUnpatch(const char* resource, const char* gfx, const char* dlist1, const char* dlist2, const char* dlist3, const char* alternateDL) {
    if (resource == NULL || gfx == NULL || dlist1 == NULL || dlist2 == NULL)
        return;

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

void UpdatePatchCustomEquipmentDlists() {
    if (gSaveContext.equips.buttonItems[0] == ITEM_SWORD_KOKIRI) {
        PatchOrUnpatch(gLinkChildSheathNearDL, gCustomKokiriSwordSheathDL, "customKokiriSheath1", "customKokiriSheath2", NULL, NULL);
        PatchOrUnpatch(gLinkChildSwordAndSheathNearDL, gCustomKokiriSwordInSheathDL, "customKokiriSwordSheath1", "customKokiriSwordSheath2", NULL, NULL);
        PatchOrUnpatch(gLinkChildDekuShieldSwordAndSheathNearDL, gCustomKokiriSwordInSheathDL, "customDekuShieldSword1", "customDekuShieldSword2", "customDekuShieldSword3", gCustomDekuShieldOnBackDL);
        PatchOrUnpatch(gLinkChildDekuShieldAndSheathNearDL, gCustomKokiriSwordSheathDL, "customDekuShieldSheath1", "customDekuShieldSheath2", "customDekuShieldSheath3", gCustomDekuShieldOnBackDL);
        PatchOrUnpatch(gLinkChildHylianShieldSwordAndSheathNearDL, gCustomKokiriSwordInSheathDL, "customChildHylianShieldSword1", "customChildHylianShieldSword2", "customChildHylianShieldSword3", gCustomHylianShieldOnChildBackDL);
        PatchOrUnpatch(gLinkChildHylianShieldAndSheathNearDL, gCustomKokiriSwordSheathDL, "customChildHylianShieldSheath1", "customChildHylianShieldSheath2", "customChildHylianShieldSheath3", gCustomHylianShieldOnChildBackDL);
        PatchOrUnpatch(gLinkAdultSheathNearDL, gCustomKokiriSwordSheathDL, "customSheath1", "customSheath2", NULL, NULL);
        PatchOrUnpatch(gLinkAdultHylianShieldSwordAndSheathNearDL, gCustomKokiriSwordInSheathDL, "customHylianShieldSword1", "customHylianShieldSword2", "customHylianShieldSword3", gCustomHylianShieldOnBackDL);
        PatchOrUnpatch(gLinkAdultMasterSwordAndSheathNearDL, gCustomKokiriSwordInSheathDL, "customMasterSwordSheath1", "customMasterSwordSheath2", NULL, NULL);
        PatchOrUnpatch(gLinkAdultHylianShieldAndSheathNearDL, gCustomKokiriSwordSheathDL, "customHylianShieldSheath1", "customHylianShieldSheath2", "customHylianShieldSheath3", gCustomHylianShieldOnBackDL);
        PatchOrUnpatch(gLinkAdultMirrorShieldSwordAndSheathNearDL, gCustomKokiriSwordInSheathDL, "customMirrorShieldSword1", "customMirrorShieldSword2", "customMirrorShieldSword3", gCustomMirrorShieldOnBackDL);
    }
    if (gSaveContext.equips.buttonItems[0] == ITEM_SWORD_MASTER) {
        PatchOrUnpatch(gLinkChildSheathNearDL, gCustomMasterSwordSheathDL, "customKokiriSheath1", "customKokiriSheath2", NULL, NULL);
        PatchOrUnpatch(gLinkChildSwordAndSheathNearDL, gCustomMasterSwordInSheathDL, "customKokiriSwordSheath1", "customKokiriSwordSheath2", NULL, NULL);
        PatchOrUnpatch(gLinkChildDekuShieldSwordAndSheathNearDL, gCustomMasterSwordInSheathDL, "customDekuShieldSword1", "customDekuShieldSword2", "customDekuShieldSword3", gCustomDekuShieldOnBackDL);
        if (!CVarGetInteger(CVAR_ENHANCEMENT("ScaleAdultEquimentAsChild"), 0)) {
            PatchOrUnpatch(gLinkChildDekuShieldWithMatrixDL, gCustomMasterSwordInSheathDL, "customDekuShieldBack1", "customDekuShieldBack2", "customDekuShieldBack2", gCustomDekuShieldOnBackDL);
        } else {
            PatchOrUnpatch(gLinkChildDekuShieldWithMatrixDL, gCustomMasterSwordInSheathDL, "customDekuShieldBack1", "customDekuShieldBack2", "customDekuShieldBack2", gCustomDekuShieldOnBackScaledDL);
        }
        PatchOrUnpatch(gLinkChildDekuShieldAndSheathNearDL, gCustomMasterSwordSheathDL, "customDekuShieldSheath1", "customDekuShieldSheath2", "customDekuShieldSheath3", gCustomDekuShieldOnBackDL);
        PatchOrUnpatch(gLinkChildHylianShieldSwordAndSheathNearDL, gCustomMasterSwordInSheathDL, "customChildHylianShieldSword1", "customChildHylianShieldSword2", "customChildHylianShieldSword3", gCustomHylianShieldOnChildBackDL);
        PatchOrUnpatch(gLinkChildHylianShieldAndSheathNearDL, gCustomMasterSwordSheathDL, "customChildHylianShieldSheath1", "customChildHylianShieldSheath2", "customChildHylianShieldSheath3", gCustomHylianShieldOnChildBackDL);
        PatchOrUnpatch(gLinkAdultSheathNearDL, gCustomMasterSwordSheathDL, "customSheath1", "customSheath2", NULL, NULL);
        PatchOrUnpatch(gLinkAdultHylianShieldSwordAndSheathNearDL, gCustomMasterSwordInSheathDL, "customHylianShieldSword1", "customHylianShieldSword2", "customHylianShieldSword3", gCustomHylianShieldOnBackDL);
        PatchOrUnpatch(gLinkAdultHylianShieldAndSheathNearDL, gCustomMasterSwordSheathDL, "customHylianShieldSheath1", "customHylianShieldSheath2", "customHylianShieldSheath3", gCustomHylianShieldOnBackDL);
        PatchOrUnpatch(gLinkAdultMirrorShieldSwordAndSheathNearDL, gCustomMasterSwordInSheathDL, "customMirrorShieldSword1", "customMirrorShieldSword2", "customMirrorShieldSword3", gCustomMirrorShieldOnBackDL);
    }
    if (gSaveContext.equips.buttonItems[0] == ITEM_SWORD_BGS && gSaveContext.bgsFlag == true) {
        PatchOrUnpatch(gLinkAdultLeftHandHoldingBgsNearDL, gCustomLongswordDL, "customBGS1", "customBGS2", "customBGS3", gLinkAdultLeftHandClosedNearDL);
        PatchOrUnpatch(gLinkChildSheathNearDL, gCustomLongswordSheathDL, "customKokiriSheath1", "customKokiriSheath2", NULL, NULL);
        PatchOrUnpatch(gLinkChildSwordAndSheathNearDL, gCustomLongswordInSheathDL, "customKokiriSwordSheath1", "customKokiriSwordSheath2", NULL, NULL);
        PatchOrUnpatch(gLinkChildDekuShieldSwordAndSheathNearDL, gCustomLongswordInSheathDL, "customDekuShieldSword1", "customDekuShieldSword2", "customDekuShieldSword3", gCustomDekuShieldOnBackDL);
        if (gPlayState != nullptr && GET_PLAYER(gPlayState)->sheathType == PLAYER_MODELTYPE_SHEATH_19) {
            if (!CVarGetInteger(CVAR_ENHANCEMENT("ScaleAdultEquimentAsChild"), 0)) {
                PatchOrUnpatch(gLinkChildDekuShieldWithMatrixDL, gCustomLongswordSheathDL, "customDekuShieldBack1", "customDekuShieldBack2", "customDekuShieldBack2", gCustomDekuShieldOnBackDL);
            } else {
                PatchOrUnpatch(gLinkChildDekuShieldWithMatrixDL, gCustomLongswordSheathDL, "customDekuShieldBack1", "customDekuShieldBack2", "customDekuShieldBack2", gCustomDekuShieldOnBackScaledDL);
            }
        } else {
            if (!CVarGetInteger(CVAR_ENHANCEMENT("ScaleAdultEquimentAsChild"), 0)) {
                PatchOrUnpatch(gLinkChildDekuShieldWithMatrixDL, gCustomLongswordInSheathDL, "customDekuShieldBack1", "customDekuShieldBack2", "customDekuShieldBack2", gCustomDekuShieldOnBackDL);
            } else {
                PatchOrUnpatch(gLinkChildDekuShieldWithMatrixDL, gCustomLongswordInSheathDL, "customDekuShieldBack1", "customDekuShieldBack2", "customDekuShieldBack2", gCustomDekuShieldOnBackScaledDL);
            }
        }
        PatchOrUnpatch(gLinkChildDekuShieldAndSheathNearDL, gCustomLongswordSheathDL, "customDekuShieldSheath1", "customDekuShieldSheath2", "customDekuShieldSheath3", gCustomDekuShieldOnBackDL);
        PatchOrUnpatch(gLinkChildHylianShieldSwordAndSheathNearDL, gCustomLongswordInSheathDL, "customChildHylianShieldSword1", "customChildHylianShieldSword2", "customChildHylianShieldSword3", gCustomHylianShieldOnChildBackDL);
        PatchOrUnpatch(gLinkChildHylianShieldAndSheathNearDL, gCustomLongswordSheathDL, "customChildHylianShieldSheath1", "customChildHylianShieldSheath2", "customChildHylianShieldSheath3", gCustomHylianShieldOnChildBackDL);
        PatchOrUnpatch(gLinkAdultSheathNearDL, gCustomLongswordSheathDL, "customSheath1", "customSheath2", NULL, NULL);
        PatchOrUnpatch(gLinkAdultHylianShieldSwordAndSheathNearDL, gCustomLongswordInSheathDL, "customHylianShieldSword1", "customHylianShieldSword2", "customHylianShieldSword3", gCustomHylianShieldOnBackDL);
        PatchOrUnpatch(gLinkAdultHylianShieldAndSheathNearDL, gCustomLongswordSheathDL, "customHylianShieldSheath1", "customHylianShieldSheath2", "customHylianShieldSheath3", gCustomHylianShieldOnBackDL);
        PatchOrUnpatch(gLinkAdultMirrorShieldSwordAndSheathNearDL, gCustomLongswordInSheathDL, "customMirrorShieldSword1", "customMirrorShieldSword2", "customMirrorShieldSword3", gCustomMirrorShieldOnBackDL);
        PatchOrUnpatch(gLinkAdultMirrorShieldAndSheathNearDL, gCustomLongswordSheathDL, "customMirrorShieldSheath1", "customMirrorShieldSheath2", "customMirrorShieldSheath3", gCustomMirrorShieldOnBackDL);
    }
    if (gSaveContext.equips.buttonItems[0] == ITEM_SWORD_BGS && (gSaveContext.bgsFlag == false)) {
        PatchOrUnpatch(gLinkAdultLeftHandHoldingBgsNearDL, gCustomBreakableLongswordDL, "customGK1", "customGK2", "customGK3", gLinkAdultLeftHandClosedNearDL);
        PatchOrUnpatch(gLinkChildSheathNearDL, gCustomBreakableLongswordSheathDL, "customKokiriSheath1", "customKokiriSheath2", NULL, NULL);
        PatchOrUnpatch(gLinkChildSwordAndSheathNearDL, gCustomBreakableLongswordInSheathDL, "customKokiriSwordSheath1", "customKokiriSwordSheath2", NULL, NULL);
        PatchOrUnpatch(gLinkChildDekuShieldSwordAndSheathNearDL, gCustomBreakableLongswordInSheathDL, "customDekuShieldSword1", "customDekuShieldSword2", "customDekuShieldSword3", gCustomDekuShieldOnBackDL);
        if (gPlayState != nullptr && GET_PLAYER(gPlayState)->sheathType == PLAYER_MODELTYPE_SHEATH_19) {
            if (!CVarGetInteger(CVAR_ENHANCEMENT("ScaleAdultEquimentAsChild"), 0)) {
                PatchOrUnpatch(gLinkChildDekuShieldWithMatrixDL, gCustomBreakableLongswordSheathDL, "customDekuShieldBack1", "customDekuShieldBack2", "customDekuShieldBack2", gCustomDekuShieldOnBackDL);
            } else {
                PatchOrUnpatch(gLinkChildDekuShieldWithMatrixDL, gCustomBreakableLongswordSheathDL, "customDekuShieldBack1", "customDekuShieldBack2", "customDekuShieldBack2", gCustomDekuShieldOnBackScaledDL);
            }
        } else {
            if (!CVarGetInteger(CVAR_ENHANCEMENT("ScaleAdultEquimentAsChild"), 0)) {
                PatchOrUnpatch(gLinkChildDekuShieldWithMatrixDL, gCustomBreakableLongswordInSheathDL, "customDekuShieldBack1", "customDekuShieldBack2", "customDekuShieldBack2", gCustomDekuShieldOnBackDL);
            } else {
                PatchOrUnpatch(gLinkChildDekuShieldWithMatrixDL, gCustomBreakableLongswordInSheathDL, "customDekuShieldBack1", "customDekuShieldBack2", "customDekuShieldBack2", gCustomDekuShieldOnBackScaledDL);
            }
        }
        PatchOrUnpatch(gLinkChildDekuShieldAndSheathNearDL, gCustomBreakableLongswordSheathDL, "customDekuShieldSheath1", "customDekuShieldSheath2", "customDekuShieldSheath3", gCustomDekuShieldOnBackDL);
        PatchOrUnpatch(gLinkChildHylianShieldSwordAndSheathNearDL, gCustomBreakableLongswordInSheathDL, "customChildHylianShieldSword1", "customChildHylianShieldSword2", "customChildHylianShieldSword3", gCustomHylianShieldOnChildBackDL);
        PatchOrUnpatch(gLinkChildHylianShieldAndSheathNearDL, gCustomBreakableLongswordSheathDL, "customChildHylianShieldSheath1", "customChildHylianShieldSheath2", "customChildHylianShieldSheath3", gCustomHylianShieldOnChildBackDL);
        PatchOrUnpatch(gLinkAdultSheathNearDL, gCustomBreakableLongswordSheathDL, "customSheath1", "customSheath2", NULL, NULL);
        PatchOrUnpatch(gLinkAdultHylianShieldSwordAndSheathNearDL, gCustomBreakableLongswordInSheathDL, "customHylianShieldSword1", "customHylianShieldSword2", "customHylianShieldSword3", gCustomHylianShieldOnBackDL);
        PatchOrUnpatch(gLinkAdultHylianShieldAndSheathNearDL, gCustomBreakableLongswordSheathDL, "customHylianShieldSheath1", "customHylianShieldSheath2", "customHylianShieldSheath3", gCustomHylianShieldOnBackDL);
        PatchOrUnpatch(gLinkAdultMirrorShieldSwordAndSheathNearDL, gCustomBreakableLongswordInSheathDL, "customMirrorShieldSword1", "customMirrorShieldSword2", "customMirrorShieldSword3", gCustomMirrorShieldOnBackDL);
        PatchOrUnpatch(gLinkAdultMirrorShieldAndSheathNearDL, gCustomBreakableLongswordSheathDL, "customMirrorShieldSheath1", "customMirrorShieldSheath2", "customMirrorShieldSheath3", gCustomMirrorShieldOnBackDL);
    }
    if (gSaveContext.equips.buttonItems[0] == ITEM_SWORD_KNIFE) {
        PatchOrUnpatch(gLinkChildSheathNearDL, gCustomBrokenLongswordSheathDL, "customKokiriSheath1", "customKokiriSheath2", NULL, NULL);
        PatchOrUnpatch(gLinkChildSwordAndSheathNearDL, gCustomBrokenLongswordInSheathDL, "customKokiriSwordSheath1", "customKokiriSwordSheath2", NULL, NULL);
        PatchOrUnpatch(gLinkChildDekuShieldSwordAndSheathNearDL, gCustomBrokenLongswordInSheathDL, "customDekuShieldSword1", "customDekuShieldSword2", "customDekuShieldSword3", gCustomDekuShieldOnBackDL);
        if (gPlayState != nullptr && GET_PLAYER(gPlayState)->sheathType == PLAYER_MODELTYPE_SHEATH_19) {
            if (!CVarGetInteger(CVAR_ENHANCEMENT("ScaleAdultEquimentAsChild"), 0)) {
                PatchOrUnpatch(gLinkChildDekuShieldWithMatrixDL, gCustomBrokenLongswordSheathDL, "customDekuShieldBack1", "customDekuShieldBack2", "customDekuShieldBack2", gCustomDekuShieldOnBackDL);
            } else {
                PatchOrUnpatch(gLinkChildDekuShieldWithMatrixDL, gCustomBrokenLongswordSheathDL, "customDekuShieldBack1", "customDekuShieldBack2", "customDekuShieldBack2", gCustomDekuShieldOnBackScaledDL);
            }
        } else {
            if (!CVarGetInteger(CVAR_ENHANCEMENT("ScaleAdultEquimentAsChild"), 0)) {
                PatchOrUnpatch(gLinkChildDekuShieldWithMatrixDL, gCustomBrokenLongswordInSheathDL, "customDekuShieldBack1", "customDekuShieldBack2", "customDekuShieldBack2", gCustomDekuShieldOnBackDL);
            } else {
                PatchOrUnpatch(gLinkChildDekuShieldWithMatrixDL, gCustomBrokenLongswordInSheathDL, "customDekuShieldBack1", "customDekuShieldBack2", "customDekuShieldBack2", gCustomDekuShieldOnBackScaledDL);
            }
        }
        PatchOrUnpatch(gLinkChildDekuShieldAndSheathNearDL, gCustomBrokenLongswordSheathDL, "customDekuShieldSheath1", "customDekuShieldSheath2", "customDekuShieldSheath3", gCustomDekuShieldOnBackDL);
        PatchOrUnpatch(gLinkChildHylianShieldSwordAndSheathNearDL, gCustomBrokenLongswordInSheathDL, "customChildHylianShieldSword1", "customChildHylianShieldSword2", "customChildHylianShieldSword3", gCustomHylianShieldOnChildBackDL);
        PatchOrUnpatch(gLinkChildHylianShieldAndSheathNearDL, gCustomBrokenLongswordSheathDL, "customChildHylianShieldSheath1", "customChildHylianShieldSheath2", "customChildHylianShieldSheath3", gCustomHylianShieldOnChildBackDL);
        PatchOrUnpatch(gLinkAdultSheathNearDL, gCustomBrokenLongswordSheathDL, "customSheath1", "customSheath2", NULL, NULL);
        PatchOrUnpatch(gLinkAdultHylianShieldSwordAndSheathNearDL, gCustomBrokenLongswordInSheathDL, "customHylianShieldSword1", "customHylianShieldSword2", "customHylianShieldSword3", gCustomHylianShieldOnBackDL);
        PatchOrUnpatch(gLinkAdultHylianShieldAndSheathNearDL, gCustomBrokenLongswordSheathDL, "customHylianShieldSheath1", "customHylianShieldSheath2", "customHylianShieldSheath3", gCustomHylianShieldOnBackDL);
        PatchOrUnpatch(gLinkAdultMirrorShieldSwordAndSheathNearDL, gCustomBrokenLongswordInSheathDL, "customMirrorShieldSword1", "customMirrorShieldSword2", "customMirrorShieldSword3", gCustomMirrorShieldOnBackDL);
        PatchOrUnpatch(gLinkAdultMirrorShieldAndSheathNearDL, gCustomBrokenLongswordSheathDL, "customMirrorShieldSheath1", "customMirrorShieldSheath2", "customMirrorShieldSheath3", gCustomMirrorShieldOnBackDL);
    }
    PatchOrUnpatch(gLinkAdultLeftHandHoldingMasterSwordNearDL, gCustomMasterSwordDL, "customMasterSword1", "customMasterSword2", "customMasterSword3", gLinkAdultLeftHandClosedNearDL);
    PatchOrUnpatch(gLinkAdultRightHandHoldingHylianShieldNearDL, gCustomHylianShieldDL, "customHylianShield1", "customHylianShield2", "customHylianShield3", gLinkAdultRightHandClosedNearDL);
    PatchOrUnpatch(gLinkAdultRightHandHoldingMirrorShieldNearDL, gCustomMirrorShieldDL, "customMirrorShield1", "customMirrorShield2", "customMirrorShield3", gLinkAdultRightHandClosedNearDL);
    PatchOrUnpatch(gLinkAdultHandHoldingBrokenGiantsKnifeDL, gCustomBrokenLongswordDL, "customBrokenBGS1", "customBrokenBGS2", "customBrokenBGS3", gLinkAdultLeftHandClosedNearDL);
    PatchOrUnpatch(gLinkChildLeftFistAndKokiriSwordNearDL, gCustomKokiriSwordDL, "customKokiriSword1", "customKokiriSword2", "customKokiriSword3", gLinkChildLeftFistNearDL);
    PatchOrUnpatch(gLinkChildRightFistAndDekuShieldNearDL, gCustomDekuShieldDL, "customDekuShield1", "customDekuShield2", "customDekuShield3", gLinkChildRightHandClosedNearDL);
    if (INV_CONTENT(ITEM_HOOKSHOT) == ITEM_HOOKSHOT) {
    PatchOrUnpatch(gLinkAdultRightHandHoldingHookshotNearDL, gCustomHookshotDL, "customHookshot1", "customHookshot2", "customHookshot3", gLinkAdultRightHandClosedNearDL);
    PatchOrUnpatch(gLinkAdultRightHandHoldingHookshotFarDL, gCustomHookshotDL, "customHookshotFPS1", "customHookshotFPS2", "customHookshotFPS3", gCustomAdultFPSHandDL);
    }
    if (INV_CONTENT(ITEM_LONGSHOT) == ITEM_LONGSHOT) {
    PatchOrUnpatch(gLinkAdultRightHandHoldingHookshotNearDL, gCustomLongshotDL, "customHookshot1", "customHookshot2", "customHookshot3", gLinkAdultRightHandClosedNearDL);
    PatchOrUnpatch(gLinkAdultRightHandHoldingHookshotFarDL, gCustomLongshotDL, "customHookshotFPS1", "customHookshotFPS2", "customHookshotFPS3", gCustomAdultFPSHandDL);
    }
    PatchOrUnpatch(gLinkAdultHookshotTipDL, gCustomHookshotTipDL, "customHookshotTip1", "customHookshotTip2", NULL, NULL);
    PatchOrUnpatch(gLinkAdultHookshotChainDL, gCustomHookshotChainDL, "customHookshotChain1", "customHookshotChain2", NULL, NULL);
    if (INV_CONTENT(ITEM_OCARINA_FAIRY) == ITEM_OCARINA_FAIRY) {
        PatchOrUnpatch(gLinkAdultRightHandHoldingOotNearDL, gCustomFairyOcarinaAdultDL, "customOcarina1", "customOcarina2", "customOcarina3", gLinkAdultRightHandNearDL);
    }
    if (INV_CONTENT(ITEM_OCARINA_TIME) == ITEM_OCARINA_TIME) {
        PatchOrUnpatch(gLinkAdultRightHandHoldingOotNearDL, gCustomOcarinaOfTimeAdultDL, "customOcarina1", "customOcarina2", "customOcarina3", gLinkAdultRightHandNearDL);
    }
    PatchOrUnpatch(gLinkChildRightHandHoldingFairyOcarinaNearDL, gCustomFairyOcarinaDL, "customFairyOcarina1", "customFairyOcarina2", "customFairyOcarina3", gLinkChildRightHandNearDL);
    PatchOrUnpatch(gLinkChildRightHandAndOotNearDL, gCustomOcarinaOfTimeDL, "customChildOcarina1", "customChildOcarina2", "customChildOcarina3", gLinkChildRightHandNearDL);
    PatchOrUnpatch(gLinkAdultRightHandHoldingBowNearDL, gCustomBowDL, "customBow1", "customBow2", "customBow3", gLinkAdultRightHandClosedNearDL);
    PatchOrUnpatch(gLinkAdultRightHandHoldingBowFirstPersonDL, gCustomBowDL, "customBowFPS1", "customBowFPS2", "customBowFPS3", gCustomAdultFPSHandDL);
    PatchOrUnpatch(gLinkAdultLeftHandHoldingHammerNearDL, gCustomHammerDL, "customHammer1", "customHammer2", "customHammer3", gLinkAdultLeftHandClosedNearDL);
    PatchOrUnpatch(gLinkChildLeftFistAndBoomerangNearDL, gCustomBoomerangDL, "customBoomerang1", "customBoomerang2", "customBoomerang3", gLinkChildLeftFistNearDL);
    PatchOrUnpatch(gLinkChildRightHandHoldingSlingshotNearDL, gCustomSlingshotDL, "customSlingshot1", "customSlingshot2", "customSlingshot3", gLinkChildRightHandClosedNearDL);
    PatchOrUnpatch(gLinkChildRightArmStretchedSlingshotDL, gCustomSlingshotDL, "customSlingshotFPS1", "customSlingshotFPS2", "customSlingshotFPS3", gCustomChildFPSHandDL);
    if (LINK_IS_CHILD && CVarGetInteger(CVAR_ENHANCEMENT("EquipmentAlwaysVisible"), 0)) { 
        PatchOrUnpatch(gCustomAdultFPSHandDL, gCustomChildFPSHandDL, "patchChildFPSHand1", "patchChildFPSHand2", NULL, NULL);
        PatchOrUnpatch(gLinkAdultRightHandClosedNearDL, gLinkChildRightHandClosedNearDL, "customChildRightHand1", "customChildRightHand2", NULL, NULL);
        PatchOrUnpatch(gLinkAdultLeftHandClosedNearDL, gLinkChildLeftFistNearDL, "customChildLeftHand1", "customChildLeftHand2", NULL, NULL);
    }
    if (LINK_IS_ADULT && CVarGetInteger(CVAR_ENHANCEMENT("EquipmentAlwaysVisible"), 0)) {
        PatchOrUnpatch(gCustomChildFPSHandDL, gCustomAdultFPSHandDL, "patchAdultFPSHand1", "patchAdultFPSHand2", NULL, NULL);
        PatchOrUnpatch(gLinkChildRightHandClosedNearDL, gLinkAdultRightHandClosedNearDL, "customAdultRightHand1", "customAdultRightHand2", NULL, NULL);
        PatchOrUnpatch(gLinkChildLeftFistNearDL, gLinkAdultLeftHandClosedNearDL, "customAdultLeftHand1", "customAdultLeftHand2", NULL, NULL);
    }
    //Below handles Adult Link not having a sword
    if (gSaveContext.equips.buttonItems[0] == ITEM_NONE) {
        PatchOrUnpatch(gLinkChildDekuShieldSwordAndSheathNearDL, gCustomDekuShieldOnBackDL, "customDekuShieldNoSword1", "customDekuShieldNoSword2", NULL, NULL);
        PatchOrUnpatch(gLinkChildDekuShieldAndSheathNearDL, gCustomDekuShieldOnBackDL, "customDekuShieldNoSheath1", "customDekuShieldNoSheath2", NULL, NULL);
        PatchOrUnpatch(gLinkAdultHylianShieldSwordAndSheathNearDL, gCustomHylianShieldOnBackDL, "customHylianShieldNoSword1", "customHylianShieldNoSword2", NULL, NULL);
        PatchOrUnpatch(gLinkAdultHylianShieldAndSheathNearDL, gCustomHylianShieldOnBackDL, "customHylianShieldNoSheath1", "customHylianShieldNoSheath2", NULL, NULL);
        PatchOrUnpatch(gLinkAdultMirrorShieldSwordAndSheathNearDL, gCustomMirrorShieldOnBackDL, "customMirrorShieldNoSword1", "customMirrorShieldNoSword2", NULL, NULL);
        PatchOrUnpatch(gLinkAdultMirrorShieldAndSheathNearDL, gCustomMirrorShieldOnBackDL, "customMirrorShieldNoSheath1", "customMirrorShieldNoSheath2", NULL, NULL);
        ResourceMgr_UnpatchGfxByName(gLinkAdultMasterSwordAndSheathNearDL, "customMasterSwordSheath1");
        ResourceMgr_UnpatchGfxByName(gLinkAdultMasterSwordAndSheathNearDL, "customMasterSwordSheath2");
    } else {
        if (gSaveContext.equips.buttonItems[0] == ITEM_SWORD_MASTER) {
            PatchOrUnpatch(gLinkAdultMasterSwordAndSheathNearDL, gCustomMasterSwordInSheathDL, "customMasterSwordSheath1", "customMasterSwordSheath2", NULL, NULL);
        }
        if (gSaveContext.equips.buttonItems[0] == ITEM_SWORD_BGS && gSaveContext.bgsFlag == true) {
            PatchOrUnpatch(gLinkAdultMasterSwordAndSheathNearDL, gCustomLongswordInSheathDL, "customMasterSwordSheath1", "customMasterSwordSheath2", NULL, NULL);
        }
        if (gSaveContext.equips.buttonItems[0] == ITEM_SWORD_BGS && gSaveContext.bgsFlag == false) {
            PatchOrUnpatch(gLinkAdultMasterSwordAndSheathNearDL, gCustomBreakableLongswordInSheathDL, "customMasterSwordSheath1", "customMasterSwordSheath2", NULL, NULL);
        }
        if (gSaveContext.equips.buttonItems[0] == ITEM_SWORD_KNIFE) {
            PatchOrUnpatch(gLinkAdultMasterSwordAndSheathNearDL, gCustomBrokenLongswordInSheathDL, "customMasterSwordSheath1", "customMasterSwordSheath2", NULL, NULL);
        }
    }
}

void RegisterPatchCustomEquipmentDlistsHandler() {
    GameInteractor::Instance->RegisterGameHook<GameInteractor::OnPlayerUpdate>([]() {
        static uint16_t lastItemOnB = gSaveContext.equips.buttonItems[0];
        static uint16_t lastOcarinaContent = INV_CONTENT(ITEM_OCARINA_TIME);
        static uint16_t lastHookshotContent = INV_CONTENT(ITEM_HOOKSHOT);
        static uint16_t lastSheathType = GET_PLAYER(gPlayState)->sheathType;
        static uint16_t lastBgsFlag = gSaveContext.bgsFlag;
        if (lastItemOnB != gSaveContext.equips.buttonItems[0] ||
            lastOcarinaContent != INV_CONTENT(ITEM_OCARINA_TIME) ||
            lastHookshotContent != INV_CONTENT(ITEM_HOOKSHOT) || 
            GET_PLAYER(gPlayState)->sheathType != lastSheathType ||
            lastBgsFlag != gSaveContext.bgsFlag) {
            UpdatePatchCustomEquipmentDlists();
            lastItemOnB = gSaveContext.equips.buttonItems[0];
            lastOcarinaContent = INV_CONTENT(ITEM_OCARINA_TIME);
            lastHookshotContent = INV_CONTENT(ITEM_HOOKSHOT);
            lastSheathType = GET_PLAYER(gPlayState)->sheathType;
            lastBgsFlag = gSaveContext.bgsFlag;
        }
    });
    GameInteractor::Instance->RegisterGameHook<GameInteractor::OnSceneSpawnActors>([]() { 
        UpdatePatchCustomEquipmentDlists(); 
    });

   GameInteractor::Instance->RegisterGameHook<GameInteractor::OnAssetAltChange>([]() {
        UpdatePatchCustomEquipmentDlists();
    });

	UpdatePatchCustomEquipmentDlists();
}
