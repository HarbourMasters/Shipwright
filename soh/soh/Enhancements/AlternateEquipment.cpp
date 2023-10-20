#include "AlternateEquipment.h"

#include <utility>
#include <unordered_map>

// Beginning of Alternate Equipment Loading "CanUse" function block.

// Alternate Equipment Loading function. (global function)
// Checks if player can use new loading method, for the left hand.
uint8_t AlternateEquipment_CanUseNewLoadingMethodLeftHand(Player* player) {
    if (!CVarGetInteger("gAltLinkEquip", 1)) {
        return false;
    }

    switch (player->leftHandType) {
        case PLAYER_MODELTYPE_LH_SWORD:   // Left hand is holding a one-handed sword.
        case PLAYER_MODELTYPE_LH_SWORD_2: // One of these is unused, but the same as the other.
            switch (CUR_EQUIP_VALUE(EQUIP_SWORD)) {
                case PLAYER_SWORD_KOKIRI:
                    if (ResourceGetIsCustomByName(gLinkKokiriSwordDL)) {
                        return true;
                    }
                default:
                    if (ResourceGetIsCustomByName(gLinkMasterSwordDL)) {
                        return true;
                    }
            }
            break;
        case PLAYER_MODELTYPE_LH_BGS:
            if (CUR_EQUIP_VALUE(EQUIP_SWORD) == PLAYER_SWORD_BIGGORON) {
                if (ResourceGetIsCustomByName(gLinkBrokenLongswordDL)) {
                    return true;
                } else if (ResourceGetIsCustomByName(gLinkLongswordDL)) {
                    return true;
                }
            }
            break;
        case PLAYER_MODELTYPE_LH_HAMMER:
            if (ResourceGetIsCustomByName(gLinkHammerDL)) {
                return true;
            }
            break;
        case PLAYER_MODELTYPE_LH_BOOMERANG:
            if (ResourceGetIsCustomByName(gLinkBoomerangDL)) {
                return true;
            }
            break;
        case PLAYER_MODELTYPE_LH_BOTTLE:
            if (ResourceGetIsCustomByName(gLinkBottleDL)) {
                return true;
            }
            break;
    }
    return false;
}

// Alternate Equipment Loading function.
// Checks if player can use new loading method, for the right hand.
uint8_t AlternateEquipment_CanUseNewLoadingMethodRightHand(Player* player) {
    if (!CVarGetInteger("gAltLinkEquip", 1)) {
        return false;
    }

    switch (player->rightHandType) {
        case PLAYER_MODELTYPE_RH_SHIELD:
            if (CUR_EQUIP_VALUE(EQUIP_SHIELD) == PLAYER_SHIELD_DEKU) {
                if (ResourceGetIsCustomByName(gLinkDekuShieldDL)) {
                    return true;
                }
            } else if ((CUR_EQUIP_VALUE(EQUIP_SHIELD) == PLAYER_SHIELD_HYLIAN) &&
                       !Player_IsChildWithHylianShield(player)) {
                if (ResourceGetIsCustomByName(gLinkHylianShieldDL)) {
                    return true;
                }
            } else if ((CUR_EQUIP_VALUE(EQUIP_SHIELD) == PLAYER_SHIELD_MIRROR)) {
                if (ResourceGetIsCustomByName(gLinkMirrorShieldDL)) {
                    return true;
                }
            }
            break;
        case PLAYER_MODELTYPE_RH_BOW_SLINGSHOT:   // Right hand is holding either Bow or Slingshot.
        case PLAYER_MODELTYPE_RH_BOW_SLINGSHOT_2: // Unused, but same as Bow_Slingshot.
            if (player->itemAction == PLAYER_IA_SLINGSHOT) {
                if (ResourceGetIsCustomByName(gLinkSlingshotDL)) {
                    return true;
                }
            } else {
                if (ResourceGetIsCustomByName(gLinkBowDL)) {
                    return true;
                }
            }
            break;
        case PLAYER_MODELTYPE_RH_HOOKSHOT:
            if (ResourceGetIsCustomByName(gLinkHookshotDL)) {
                return true;
            }
            break;
    }
    // Ocarinas check to see if the item is being used instead of rightHandType.
    // Otherwise, cutscenes for learning Ocarina songs don't work properly.
    if (player->itemAction == PLAYER_IA_OCARINA_FAIRY) {
        if (ResourceGetIsCustomByName(gLinkFairyOcarinaDL)) {
            return true;
        }
    }
    if (player->itemAction == PLAYER_IA_OCARINA_OF_TIME) {
        if (ResourceGetIsCustomByName(gLinkOcarinaOfTimeDL)) {
            return true;
        }
    }
    return false;
}

// Alternate Equipment Loading function.
// Checks if pause screen player model can use new loading method, for the left hand.
uint8_t AlternateEquipment_CanUseNewLoadingMethodLeftHandPause(Player* player) {
    if (!CVarGetInteger("gAltLinkEquip", 1)) {
        return false;
    }

    switch (CUR_EQUIP_VALUE(EQUIP_SWORD)) {
        case PLAYER_SWORD_KOKIRI:
            if (ResourceGetIsCustomByName(gLinkKokiriSwordDL)) {
                return true;
            }
            break;
        case PLAYER_SWORD_MASTER:
            if (ResourceGetIsCustomByName(gLinkMasterSwordDL)) {
                return true;
            }
            break;
        case PLAYER_SWORD_BIGGORON:
            if (ResourceGetIsCustomByName(gLinkBrokenLongswordDL)) {
                return true;
            } else if (ResourceGetIsCustomByName(gLinkLongswordDL)) {
                return true;
            }
            break;
    }
    return false;
}

// Alternate Equipment Loading function.
// Checks if pause screen player model can use new loading method, for the right hand.
uint8_t AlternateEquipment_CanUseNewLoadingMethodRightHandPause(Player* player) {
    if (!CVarGetInteger("gAltLinkEquip", 1)) {
        return false;
    }

    if (CUR_EQUIP_VALUE(EQUIP_SHIELD) == PLAYER_SHIELD_DEKU) {
        if (ResourceGetIsCustomByName(gLinkDekuShieldDL)) {
            return true;
        }
    } else if ((CUR_EQUIP_VALUE(EQUIP_SHIELD) == PLAYER_SHIELD_HYLIAN) && !Player_IsChildWithHylianShield(player)) {
        if (ResourceGetIsCustomByName(gLinkHylianShieldDL)) {
            return true;
        }
    } else if ((CUR_EQUIP_VALUE(EQUIP_SHIELD) == PLAYER_SHIELD_MIRROR)) {
        if (ResourceGetIsCustomByName(gLinkMirrorShieldDL)) {
            return true;
        }
    }
    return false;
}

// Alternate Equipment Loading function.
// Checks if player can use new loading method, for the first person view.
uint8_t AlternateEquipment_CanUseNewLoadingMethodFirstPerson(Player* player) {
    if (!CVarGetInteger("gAltLinkEquip", 1)) {
        return false;
    }

    switch (player->rightHandType) {
        case PLAYER_MODELTYPE_RH_BOW_SLINGSHOT:   // Right hand is holding either Bow or Slingshot.
        case PLAYER_MODELTYPE_RH_BOW_SLINGSHOT_2: // Unused, but same as Bow_Slingshot.
            if (player->itemAction == PLAYER_IA_SLINGSHOT) {
                if (!ResourceGetIsCustomByName(gLinkSlingshotDL)) {
                    return false;
                }
            } else {
                if (!ResourceGetIsCustomByName(gLinkBowDL)) {
                    return false;
                }
            }
            break;
        case PLAYER_MODELTYPE_RH_HOOKSHOT:
            if (!(ResourceGetIsCustomByName(gLinkHookshotDL))) {
                return false;
            }
            break;
    }
    if (LINK_IS_ADULT) {
        if (!ResourceGetIsCustomByName(gLinkAdultFPSHandDL)) {
            return false;
        }
    }
    if (LINK_IS_CHILD) {
        if (!ResourceGetIsCustomByName(gLinkChildFPSHandDL)) {
            return false;
        }
    }
    return true;
}

// Alternate Equipment Loading function.
// Checks if player can use new loading method,
uint8_t AlternateEquipment_CanUseNewLoadingMethodSheathSword(Player* player) {
    if (!CVarGetInteger("gAltLinkEquip", 1)) {
        return false;
    }

    switch (CUR_EQUIP_VALUE(EQUIP_SWORD)) {
        case PLAYER_SWORD_KOKIRI:
            if (!(ResourceGetIsCustomByName(gLinkKokiriSwordSheathDL) &&
                  ResourceGetIsCustomByName(gLinkKokiriSwordInSheathDL))) {
                return false;
            }
            break;
        case PLAYER_SWORD_MASTER:
            if (!(ResourceGetIsCustomByName(gLinkMasterSwordSheathDL) &&
                  ResourceGetIsCustomByName(gLinkMasterSwordInSheathDL))) {
                return false;
            }
            break;
        case PLAYER_SWORD_BIGGORON:
            if (!(ResourceGetIsCustomByName(gLinkLongswordSheathDL) &&
                  ResourceGetIsCustomByName(gLinkLongswordInSheathDL))) {
                return false;
            }
    }
    return true;
}

// Alternate Equipment Loading function.
// Checks if player can use new loading method,
uint8_t AlternateEquipment_CanUseNewLoadingMethodSheathShield(Player* player) {
    if (!CVarGetInteger("gAltLinkEquip", 1)) {
        return false;
    }

    switch (CUR_EQUIP_VALUE(EQUIP_SHIELD)) {
        case PLAYER_SHIELD_DEKU:
            if (!ResourceGetIsCustomByName(gLinkDekuShieldOnBackDL)) {
                return false;
            }
            break;
        case PLAYER_SHIELD_HYLIAN:
            if (!ResourceGetIsCustomByName(gLinkHylianShieldOnBackDL) && !Player_IsChildWithHylianShield(player)) {
                return false;
            }
            if (!ResourceGetIsCustomByName(gLinkHylianShieldOnChildBackDL) && Player_IsChildWithHylianShield(player)) {
                return false;
            }
            break;
        case PLAYER_SHIELD_MIRROR:
            if (!ResourceGetIsCustomByName(gLinkMirrorShieldOnBackDL)) {
                return false;
            }
    }
    return true;
}

// End of Alternate Equipment Loading "CanUse" function block.

std::unordered_map<Gfx*,Gfx*> dListMap = {
    // PLAYER_MODELTYPE_LH_SWORD PLAYER_MODELTYPE_LH_SWORD_2
    {(Gfx*)gLinkAdultLeftHandHoldingMasterSwordNearDL,  (Gfx*)gLinkAdultLeftHandClosedNearDL},
    {(Gfx*)gLinkChildLeftFistAndKokiriSwordNearDL,      (Gfx*)gLinkChildLeftFistNearDL},
    {(Gfx*)gLinkAdultLeftHandHoldingMasterSwordFarDL,   (Gfx*)gLinkAdultLeftHandClosedFarDL},
    {(Gfx*)gLinkChildLeftFistAndKokiriSwordFarDL,       (Gfx*)gLinkChildLeftFistFarDL},
    // PLAYER_MODELTYPE_LH_BGS
    {(Gfx*)gLinkAdultLeftHandHoldingBgsNearDL,          (Gfx*)gLinkAdultLeftHandClosedNearDL},
    {(Gfx*)gLinkChildLeftHandHoldingMasterSwordDL,      (Gfx*)gLinkChildLeftFistNearDL},
    {(Gfx*)gLinkAdultLeftHandHoldingBgsFarDL,           (Gfx*)gLinkAdultLeftHandClosedFarDL},
    {(Gfx*)gLinkChildLeftHandHoldingMasterSwordDL,      (Gfx*)gLinkChildLeftFistFarDL},
    {(Gfx*)gLinkAdultHandHoldingBrokenGiantsKnifeDL,    (Gfx*)gLinkAdultLeftHandClosedNearDL},
    {(Gfx*)gLinkChildLeftHandHoldingMasterSwordDL,      (Gfx*)gLinkChildLeftFistNearDL},
    {(Gfx*)gLinkAdultHandHoldingBrokenGiantsKnifeFarDL, (Gfx*)gLinkAdultLeftHandClosedFarDL},
    {(Gfx*)gLinkChildLeftHandHoldingMasterSwordDL,      (Gfx*)gLinkChildLeftFistFarDL},
    // PLAYER_MODELTYPE_LH_HAMMER
    {(Gfx*)gLinkAdultLeftHandHoldingHammerNearDL,       (Gfx*)gLinkAdultLeftHandClosedNearDL},
    {(Gfx*)gLinkChildLeftHandNearDL,                    (Gfx*)gLinkChildLeftFistNearDL},
    {(Gfx*)gLinkAdultLeftHandHoldingHammerFarDL,        (Gfx*)gLinkAdultLeftHandClosedFarDL},
    {(Gfx*)gLinkChildLeftHandFarDL,                     (Gfx*)gLinkChildLeftFistFarDL},
    // PLAYER_MODELTYPE_LH_BOOMERANG
    {(Gfx*)gLinkAdultLeftHandNearDL,                    (Gfx*)gLinkAdultLeftHandClosedNearDL},
    {(Gfx*)gLinkChildLeftFistAndBoomerangNearDL,        (Gfx*)gLinkChildLeftFistNearDL},
    {(Gfx*)gLinkAdultLeftHandFarDL,                     (Gfx*)gLinkAdultLeftHandClosedFarDL},
    {(Gfx*)gLinkChildLeftFistAndBoomerangFarDL,         (Gfx*)gLinkChildLeftFistFarDL},
    // PLAYER_MODELTYPE_RH_SHIELD
    {(Gfx*)gLinkAdultRightHandClosedNearDL,             (Gfx*)gLinkAdultRightHandClosedNearDL},
    {(Gfx*)gLinkChildRightHandClosedNearDL,             (Gfx*)gLinkChildRightHandClosedNearDL},
    {(Gfx*)gLinkAdultRightHandClosedFarDL,              (Gfx*)gLinkAdultRightHandClosedFarDL},
    {(Gfx*)gLinkChildRightHandClosedFarDL,              (Gfx*)gLinkChildRightHandClosedFarDL},
    {(Gfx*)gLinkAdultRightHandClosedNearDL,             (Gfx*)gLinkAdultRightHandClosedNearDL},
    {(Gfx*)gLinkChildRightFistAndDekuShieldNearDL,      (Gfx*)gLinkChildRightHandClosedNearDL},
    {(Gfx*)gLinkAdultRightHandClosedFarDL,              (Gfx*)gLinkAdultRightHandClosedFarDL},
    {(Gfx*)gLinkChildRightFistAndDekuShieldFarDL,       (Gfx*)gLinkChildRightHandClosedFarDL},
    {(Gfx*)gLinkAdultRightHandHoldingHylianShieldNearDL,(Gfx*)gLinkAdultRightHandClosedNearDL},
    {(Gfx*)gLinkChildRightHandClosedNearDL,             (Gfx*)gLinkChildRightHandClosedNearDL},
    {(Gfx*)gLinkAdultRightHandHoldingHylianShieldFarDL, (Gfx*)gLinkAdultRightHandClosedFarDL},
    {(Gfx*)gLinkChildRightHandClosedFarDL,              (Gfx*)gLinkChildRightHandClosedFarDL},
    {(Gfx*)gLinkAdultRightHandHoldingMirrorShieldNearDL,(Gfx*)gLinkAdultRightHandClosedNearDL},
    {(Gfx*)gLinkChildRightHandClosedNearDL,             (Gfx*)gLinkChildRightHandClosedNearDL},
    {(Gfx*)gLinkAdultRightHandHoldingMirrorShieldFarDL, (Gfx*)gLinkAdultRightHandClosedFarDL},
    {(Gfx*)gLinkChildRightHandClosedFarDL,              (Gfx*)gLinkChildRightHandClosedFarDL},
    // PLAYER_MODELTYPE_RH_BOW_SLINGSHOT PLAYER_MODELTYPE_RH_BOW_SLINGSHOT_2 
    {(Gfx*)gLinkAdultRightHandHoldingBowNearDL,         (Gfx*)gLinkAdultRightHandClosedNearDL},
    {(Gfx*)gLinkChildRightHandHoldingSlingshotNearDL,   (Gfx*)gLinkChildRightHandClosedNearDL},
    {(Gfx*)gLinkAdultRightHandHoldingBowFarDL,          (Gfx*)gLinkAdultRightHandClosedFarDL},
    {(Gfx*)gLinkChildRightHandHoldingSlingshotFarDL,    (Gfx*)gLinkChildRightHandClosedFarDL},
    // PLAYER_MODELTYPE_RH_OCARINA
    {(Gfx*)gLinkAdultRightHandHoldingOotNearDL,         (Gfx*)gLinkAdultRightHandNearDL},
    {(Gfx*)gLinkChildRightHandHoldingFairyOcarinaNearDL,(Gfx*)gLinkChildRightHandNearDL},
    {(Gfx*)gLinkAdultRightHandHoldingOotFarDL,          (Gfx*)gLinkAdultRightHandFarDL},
    {(Gfx*)gLinkChildRightHandHoldingFairyOcarinaFarDL, (Gfx*)gLinkChildRightHandFarDL},
    // PLAYER_MODELTYPE_RH_OOT
    {(Gfx*)gLinkAdultRightHandHoldingOotNearDL,         (Gfx*)gLinkAdultRightHandNearDL},
    {(Gfx*)gLinkChildRightHandAndOotNearDL,             (Gfx*)gLinkChildRightHandNearDL},
    {(Gfx*)gLinkAdultRightHandHoldingOotFarDL,          (Gfx*)gLinkAdultRightHandFarDL},
    {(Gfx*)gLinkChildRightHandHoldingOOTFarDL,          (Gfx*)gLinkChildRightHandFarDL},
    // PLAYER_MODELTYPE_RH_HOOKSHOT
    {(Gfx*)gLinkAdultRightHandHoldingHookshotNearDL,    (Gfx*)gLinkAdultRightHandClosedNearDL},
    {(Gfx*)gLinkChildRightHandNearDL,                   (Gfx*)gLinkChildRightHandClosedNearDL},
    {(Gfx*)gLinkAdultRightHandHoldingHookshotNearDL,    (Gfx*)gLinkAdultRightHandClosedFarDL},
    {(Gfx*)gLinkChildRightHandFarDL,                    (Gfx*)gLinkChildRightHandClosedFarDL},
    // SHEATHES
    {(Gfx*)gLinkAdultMasterSwordAndSheathNearDL,        NULL},
    {(Gfx*)gLinkChildSwordAndSheathNearDL,              NULL},
    {(Gfx*)gLinkAdultMasterSwordAndSheathFarDL,         NULL},
    {(Gfx*)gLinkChildSwordAndSheathFarDL,               NULL},
    {(Gfx*)gLinkAdultSheathNearDL,                      NULL},
    {(Gfx*)gLinkChildSheathNearDL,                      NULL},
    {(Gfx*)gLinkAdultSheathFarDL,                       NULL},
    {(Gfx*)gLinkChildSheathFarDL,                       NULL},
    {(Gfx*)gLinkChildDekuShieldSwordAndSheathNearDL,    NULL},
    {(Gfx*)gLinkChildDekuShieldSwordAndSheathFarDL,     NULL},
    {(Gfx*)gLinkAdultHylianShieldSwordAndSheathNearDL,  NULL},
    {(Gfx*)gLinkChildHylianShieldSwordAndSheathNearDL,  NULL},
    {(Gfx*)gLinkAdultHylianShieldSwordAndSheathFarDL,   NULL},
    {(Gfx*)gLinkChildHylianShieldSwordAndSheathFarDL,   NULL},
    {(Gfx*)gLinkAdultMirrorShieldSwordAndSheathNearDL,  NULL},
    {(Gfx*)gLinkAdultMirrorShieldSwordAndSheathFarDL,   NULL},
    {(Gfx*)gLinkChildDekuShieldWithMatrixDL,            NULL},
    {(Gfx*)gLinkChildDekuShieldAndSheathNearDL,         NULL},
    {(Gfx*)gLinkChildDekuShieldAndSheathFarDL,          NULL},
    {(Gfx*)gLinkAdultHylianShieldAndSheathNearDL,       NULL},
    {(Gfx*)gLinkChildHylianShieldAndSheathNearDL,       NULL},
    {(Gfx*)gLinkAdultHylianShieldAndSheathFarDL,        NULL},
    {(Gfx*)gLinkChildHylianShieldAndSheathFarDL,        NULL},
    {(Gfx*)gLinkAdultMirrorShieldAndSheathNearDL,       NULL},
    {(Gfx*)gLinkAdultMirrorShieldAndSheathFarDL,        NULL},
};

bool AlternateEquipment_SwapDLists(Gfx** dLists, Gfx* dList, s32 dListsLodOffset, s32 limbIndex, Player* player) {
    bool patchDLists = false;
    switch (limbIndex) {
        case PLAYER_LIMB_L_HAND:
            // todo: replace the canuse conditions to depend on player->xxxxType
            if (AlternateEquipment_CanUseNewLoadingMethodLeftHand(player) && dListMap.find(dLists[dListsLodOffset]) != dListMap.end()) {
                dList = dListMap.at(dLists[dListsLodOffset]);
                patchDLists = true;
            }
            break;
        case PLAYER_LIMB_R_HAND:
            if (AlternateEquipment_CanUseNewLoadingMethodLeftHand(player) && dListMap.find(dLists[dListsLodOffset]) != dListMap.end()) {
                dList = dListMap.at(dLists[dListsLodOffset]);
                patchDLists = true;
            }
            break;
        case PLAYER_LIMB_SHEATH:
            if (AlternateEquipment_CanUseNewLoadingMethodSheathSword(player) && AlternateEquipment_CanUseNewLoadingMethodSheathShield(player) && dListMap.find(dLists[dListsLodOffset]) != dListMap.end()) {
                dList = dListMap.at(dLists[dListsLodOffset]);
                patchDLists = true;
            }
            break;

    }
    return patchDLists;
}

#ifdef __cplusplus
extern "C" {
#endif

// Beginning of Alternate Equipment Loading "Draw Item" function block

// Alternate Equipment Loading function.
// Rescales Child Link's items for Adult Link.
void AlternateEquipment_DrawChildItem(PlayState* play, Gfx* dlist) {
    OPEN_DISPS(play->state.gfxCtx);

    // rescale child items for adult, otherwise clipping occurs
    if (LINK_IS_ADULT) {
        Matrix_Scale(1.35f, 1.35f, 1.35f, MTXMODE_APPLY);
    }

    gSPMatrix(POLY_OPA_DISP++, MATRIX_NEWMTX(play->state.gfxCtx), G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
    gSPDisplayList(POLY_OPA_DISP++, dlist);

    CLOSE_DISPS(play->state.gfxCtx);

    if (LINK_IS_ADULT) {
        // Scale the Matrix back to default, otherwise things get ruined
        Matrix_Scale(1.0f / 1.35f, 1.0f / 1.35f, 1.0f / 1.35f, MTXMODE_APPLY);
    }
}

// Alternate Equipment Loading function.
// Rescales Adult Link's items for Child Link.
void AlternateEquipment_DrawAdultItem(PlayState* play, Gfx* dlist) {
    OPEN_DISPS(play->state.gfxCtx);

    // rescale adult items for child, otherwise clipping occurs
    if (LINK_IS_CHILD) {
        Matrix_Scale(1.0f / 1.35f, 1.0f / 1.35f, 1.0f / 1.35f, MTXMODE_APPLY);
    }

    gSPMatrix(POLY_OPA_DISP++, MATRIX_NEWMTX(play->state.gfxCtx), G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
    gSPDisplayList(POLY_OPA_DISP++, dlist);

    CLOSE_DISPS(play->state.gfxCtx);

    if (LINK_IS_CHILD) {
        // Scale the Matrix back to default, otherwise things get ruined
        Matrix_Scale(1.35f, 1.35f, 1.35f, MTXMODE_APPLY);
    }
}

// Alternate Equipment Loading function.
// draws the bow and hookshot, which need an alternate method to make sure bowstrings and hookshot tips scale properly
void AlternateEquipment_DrawRightHandItem(PlayState* play, Gfx* dlist) {
    OPEN_DISPS(play->state.gfxCtx);

    // rescale adult items for child, otherwise clipping occurs
    if (LINK_IS_CHILD) {
        Matrix_Scale(1.0f / 1.35f, 1.0f / 1.35f, 1.0f / 1.35f, MTXMODE_APPLY);
    }

    gSPMatrix(POLY_OPA_DISP++, MATRIX_NEWMTX(play->state.gfxCtx), G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
    gSPDisplayList(POLY_OPA_DISP++, dlist);

    CLOSE_DISPS(play->state.gfxCtx);
}

// Alternate Equipment Loading function.
void AlternateEquipment_DrawOcarinaItem(PlayState* play, Gfx* dlist) {
    OPEN_DISPS(play->state.gfxCtx);

    if (LINK_IS_ADULT) {
        Matrix_Translate(115.0f, 200.0f, 115.0f, MTXMODE_APPLY);
        Matrix_RotateZYX(7.57f, 3.72f, 0.0f, MTXMODE_APPLY);
    }

    gSPMatrix(POLY_OPA_DISP++, MATRIX_NEWMTX(play->state.gfxCtx), G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);

    gSPDisplayList(POLY_OPA_DISP++, (Gfx*)dlist);

    CLOSE_DISPS(play->state.gfxCtx);
}

// End of Alternate Equipment Loading "Draw" function block

#ifdef __cplusplus
}
#endif