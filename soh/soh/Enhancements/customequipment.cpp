#include <initializer_list>
#include <libultraship/bridge/resourcebridge.h>
#include "objects/object_link_boy/object_link_boy.h"
#include "objects/object_link_child/object_link_child.h"
#include "objects/object_custom_equip/object_custom_equip.h"
#include "soh/Enhancements/game-interactor/GameInteractor.h"
#include "soh/ShipInit.hpp"
#include "soh/ResourceManagerHelpers.h"

extern "C" {
#include "macros.h"
#include "functions.h"
#include "variables.h"
extern SaveContext gSaveContext;
}

static const char* ResolveCustomChain(std::initializer_list<const char*> paths) {
    const char* fallback = nullptr;
    for (auto path : paths) {
        if (path == nullptr)
            continue;
        fallback = path;
        if (ResourceGetIsCustomByName(path) || ResourceMgr_FileAltExists(path))
            return path;
    }
    return fallback;
}

static const char* ResolveCustomFPSHand(const char* path) {
    const bool isAdult = path == gCustomAdultFPSHandDL;
    const bool isChild = path == gCustomChildFPSHandDL;

    if (!isAdult && !isChild) {
        return path;
    }

    switch (TUNIC_EQUIP_TO_PLAYER(CUR_EQUIP_VALUE(EQUIP_TYPE_TUNIC))) {
        case PLAYER_TUNIC_GORON:
            return ResolveCustomChain(
                { isAdult ? gCustomAdultGoronFPSHandDL : gCustomChildGoronFPSHandDL, path, nullptr });
        case PLAYER_TUNIC_ZORA:
            return ResolveCustomChain(
                { isAdult ? gCustomAdultZoraFPSHandDL : gCustomChildZoraFPSHandDL, path, nullptr });
        default:
            return path;
    }
}

static Gfx* LoadGfxByName(const char* path) {
    return path ? ResourceMgr_LoadGfxByName(path) : nullptr;
}

static Gfx* LoadCustomGfx(const char* path) {
    if (!path)
        return nullptr;
    path = ResolveCustomFPSHand(path);
    if (!ResourceGetIsCustomByName(path) && !ResourceMgr_FileAltExists(path))
        return nullptr;
    return ResourceMgr_LoadGfxByName(path);
}

static u8 sLastValidSwordEquip = EQUIP_VALUE_SWORD_NONE;

static u8 GetEquippedSwordValue(PlayState* play) {
    const u8 sword = CUR_EQUIP_VALUE(EQUIP_TYPE_SWORD);
    const bool inCutscene = (play->csCtx.state != CS_STATE_IDLE);
    if (!(inCutscene && gSaveContext.linkAge == LINK_AGE_CHILD && sword == EQUIP_VALUE_SWORD_MASTER)) {
        sLastValidSwordEquip = sword;
    }
    return sLastValidSwordEquip;
}

static const char* GetSwordInSheathDL(PlayState* play) {
    switch (GetEquippedSwordValue(play)) {
        case EQUIP_VALUE_SWORD_KOKIRI:
            return gCustomKokiriSwordInSheathDL;
        case EQUIP_VALUE_SWORD_MASTER:
            return gCustomMasterSwordInSheathDL;
        case EQUIP_VALUE_SWORD_BIGGORON:
            if (gSaveContext.bgsFlag)
                return gCustomLongswordInSheathDL;
            if (CHECK_OWNED_EQUIP_ALT(EQUIP_TYPE_SWORD, EQUIP_INV_SWORD_BROKENGIANTKNIFE))
                return gCustomBrokenLongswordInSheathDL;
            return ResolveCustomChain({ gCustomBreakableLongswordInSheathDL, gCustomLongswordInSheathDL, nullptr });
    }
    return nullptr;
}

static const char* GetSheathOnlyDL(PlayState* play) {
    switch (GetEquippedSwordValue(play)) {
        case EQUIP_VALUE_SWORD_KOKIRI:
            return gCustomKokiriSwordSheathDL;
        case EQUIP_VALUE_SWORD_MASTER:
            return gCustomMasterSwordSheathDL;
        case EQUIP_VALUE_SWORD_BIGGORON:
            if (gSaveContext.bgsFlag)
                return gCustomLongswordSheathDL;
            if (CHECK_OWNED_EQUIP_ALT(EQUIP_TYPE_SWORD, EQUIP_INV_SWORD_BROKENGIANTKNIFE))
                return gCustomBrokenLongswordSheathDL;
            return ResolveCustomChain({ gCustomBreakableLongswordSheathDL, gCustomLongswordSheathDL, nullptr });
    }
    return nullptr;
}

static const char* GetShieldOnBackDL(s32 shield) {
    const bool isAdult = gSaveContext.linkAge == LINK_AGE_ADULT;
    switch (shield) {
        case PLAYER_SHIELD_DEKU:
            return gCustomDekuShieldOnBackDL;
        case PLAYER_SHIELD_HYLIAN:
            return isAdult ? gCustomHylianShieldOnBackDL : gCustomHylianShieldOnChildBackDL;
        case PLAYER_SHIELD_MIRROR:
            return gCustomMirrorShieldOnBackDL;
    }
    return nullptr;
}

static const char* GetSwordInSheathDLForPlayer(Player* player, PlayState* play) {
    if (player == GET_PLAYER(play))
        return GetSwordInSheathDL(play);
    switch (player->heldItemId) {
        case ITEM_SWORD_KOKIRI:
            return gCustomKokiriSwordInSheathDL;
        case ITEM_SWORD_MASTER:
            return gCustomMasterSwordInSheathDL;
        case ITEM_SWORD_BGS:
            return gCustomLongswordInSheathDL;
        case ITEM_SWORD_KNIFE:
            return gCustomBrokenLongswordInSheathDL;
    }
    return nullptr;
}

static const char* GetSheathOnlyDLForPlayer(Player* player, PlayState* play) {
    if (player == GET_PLAYER(play))
        return GetSheathOnlyDL(play);
    switch (player->heldItemId) {
        case ITEM_SWORD_KOKIRI:
            return gCustomKokiriSwordSheathDL;
        case ITEM_SWORD_MASTER:
            return gCustomMasterSwordSheathDL;
        case ITEM_SWORD_BGS:
            return gCustomLongswordSheathDL;
        case ITEM_SWORD_KNIFE:
            return gCustomBrokenLongswordSheathDL;
    }
    return nullptr;
}

static u8 PauseGetLimbType(s32 limbIndex) {
    const u8 swordEquip = CUR_EQUIP_VALUE(EQUIP_TYPE_SWORD);
    const u8 shieldEquip = CUR_EQUIP_VALUE(EQUIP_TYPE_SHIELD);
    const bool isBGS = (swordEquip == EQUIP_VALUE_SWORD_BIGGORON);
    const bool isSword = (swordEquip != EQUIP_VALUE_SWORD_NONE);
    const bool childHylian = (!LINK_IS_ADULT && shieldEquip == PLAYER_SHIELD_HYLIAN);

    switch (limbIndex) {
        case PLAYER_LIMB_L_HAND:
            if (!isSword)
                return PLAYER_MODELTYPE_LH_OPEN;
            return isBGS ? PLAYER_MODELTYPE_LH_BGS : PLAYER_MODELTYPE_LH_SWORD;
        case PLAYER_LIMB_R_HAND:
            return (isBGS || childHylian) ? PLAYER_MODELTYPE_RH_CLOSED : PLAYER_MODELTYPE_RH_SHIELD;
        case PLAYER_LIMB_SHEATH:
            if (isBGS || childHylian)
                return PLAYER_MODELTYPE_SHEATH_19;
            return PLAYER_MODELTYPE_SHEATH_17;
    }
    return 0;
}

// Counter-scale hand mesh when EquipmentAlwaysVisible + ScaleAdultEquipmentAsChild is active on child Link.
static constexpr float HAND_COUNTER_SCALE_Y_OFFSET = 100.0f;

static void BuildHandItemDL(PlayState* play, Gfx** dList, Gfx* hand, Gfx* item, bool counterScaleHand) {
    if (counterScaleHand) {
        Mtx* scaleMtx = (Mtx*)Graph_Alloc(play->state.gfxCtx, sizeof(Mtx));
        MtxF mf = {};
        mf.xx = mf.yy = mf.zz = 1.25f;
        mf.ww = 1.0f;
        mf.yw = HAND_COUNTER_SCALE_Y_OFFSET;
        Matrix_MtxFToMtx(&mf, scaleMtx);
        Gfx* buf = (Gfx*)Graph_Alloc(play->state.gfxCtx, 5 * sizeof(Gfx));
        Gfx* p = buf;
        gSPMatrix(p++, scaleMtx, G_MTX_PUSH | G_MTX_MUL | G_MTX_MODELVIEW);
        gSPDisplayList(p++, hand);
        gSPPopMatrix(p++, G_MTX_MODELVIEW);
        gSPDisplayList(p++, item);
        gSPEndDisplayList(p);
        *dList = buf;
    } else {
        Gfx* buf = (Gfx*)Graph_Alloc(play->state.gfxCtx, 3 * sizeof(Gfx));
        Gfx* p = buf;
        gSPDisplayList(p++, hand);
        gSPDisplayList(p++, item);
        gSPEndDisplayList(p);
        *dList = buf;
    }
}

static bool IsScalingAdultItemAsChild() {
    return CVarGetInteger(CVAR_ENHANCEMENT("EquipmentAlwaysVisible"), 0) &&
           CVarGetInteger(CVAR_ENHANCEMENT("ScaleAdultEquipmentAsChild"), 0) && !LINK_IS_ADULT;
}

const char* bottleContentDLs[] = {
    nullptr,                    // 0: PLAYER_IA_BOTTLE (empty - no custom content needed)
    gCustomBottleFishDL,        // 1: PLAYER_IA_BOTTLE_FISH
    gCustomBottleBlueFireDL,    // 2: PLAYER_IA_BOTTLE_FIRE
    gCustomBottleBugDL,         // 3: PLAYER_IA_BOTTLE_BUG
    gCustomBottlePoeDL,         // 4: PLAYER_IA_BOTTLE_POE
    gCustomBottleBigPoeDL,      // 5: PLAYER_IA_BOTTLE_BIG_POE
    gCustomBottleLetterDL,      // 6: PLAYER_IA_BOTTLE_RUTOS_LETTER
    gCustomBottleRedPotionDL,   // 7: PLAYER_IA_BOTTLE_POTION_RED
    gCustomBottleBluePotionDL,  // 8: PLAYER_IA_BOTTLE_POTION_BLUE
    gCustomBottleGreenPotionDL, // 9: PLAYER_IA_BOTTLE_POTION_GREEN
    gCustomBottleMilkDL,        // 10: PLAYER_IA_BOTTLE_MILK_FULL
    gCustomBottleMilkHalfDL,    // 11: PLAYER_IA_BOTTLE_MILK_HALF
    gCustomBottleFairyDL,       // 12: PLAYER_IA_BOTTLE_FAIRY
};

static void RegisterCustomEquipment() {
    // World (gameplay) character
    COND_VB_SHOULD(VB_PLAYER_OVERRIDE_LIMB_DRAW, CVarGetInteger(CVAR_SETTING("AltAssets"), 1), {
        s32 limbIndex = va_arg(args, s32);
        Gfx** dList = va_arg(args, Gfx**);
        Player* player = (Player*)va_arg(args, void*);
        PlayState* play = va_arg(args, PlayState*);

        const bool isAdult = gSaveContext.linkAge == LINK_AGE_ADULT;
        const char* customDL = nullptr;

        switch (limbIndex) {
            case PLAYER_LIMB_L_HAND: {
                const bool isOcarina =
                    player->heldItemAction == PLAYER_IA_OCARINA_FAIRY ||
                    player->heldItemAction == PLAYER_IA_OCARINA_OF_TIME ||
                    player->itemAction == PLAYER_IA_OCARINA_FAIRY || player->itemAction == PLAYER_IA_OCARINA_OF_TIME ||
                    player->modelGroup == PLAYER_MODELGROUP_OCARINA || player->modelGroup == PLAYER_MODELGROUP_OOT;
                if (isOcarina) {
                    Gfx* resolvedHand = LoadGfxByName(isAdult ? gLinkAdultLeftHandNearDL : gLinkChildLeftHandNearDL);
                    if (resolvedHand) {
                        Gfx* buf = (Gfx*)Graph_Alloc(play->state.gfxCtx, 2 * sizeof(Gfx));
                        Gfx* p = buf;
                        gSPDisplayList(p++, resolvedHand);
                        gSPEndDisplayList(p);
                        *dList = buf;
                    }
                    break;
                }
                switch ((u8)player->leftHandType) {
                    case PLAYER_MODELTYPE_LH_SWORD: {
                        if (player == GET_PLAYER(play)) {
                            if (gSaveContext.equips.buttonItems[0] == ITEM_SWORD_KOKIRI)
                                customDL = gCustomKokiriSwordDL;
                            else if (gSaveContext.equips.buttonItems[0] == ITEM_SWORD_MASTER)
                                customDL = gCustomMasterSwordDL;
                        } else {
                            if (player->heldItemAction == PLAYER_IA_SWORD_KOKIRI)
                                customDL = gCustomKokiriSwordDL;
                            else if (player->heldItemAction == PLAYER_IA_SWORD_MASTER)
                                customDL = gCustomMasterSwordDL;
                        }
                        break;
                    }
                    case PLAYER_MODELTYPE_LH_BGS: {
                        const bool isBrokenKnife =
                            (player == GET_PLAYER(play))
                                ? CHECK_OWNED_EQUIP_ALT(EQUIP_TYPE_SWORD, EQUIP_INV_SWORD_BROKENGIANTKNIFE)
                                : (player->heldItemId == ITEM_SWORD_KNIFE);
                        if (isBrokenKnife) {
                            customDL = gCustomBrokenLongswordDL;
                        } else if (player == GET_PLAYER(play)) {
                            if (gSaveContext.bgsFlag)
                                customDL = gCustomLongswordDL;
                            else
                                customDL =
                                    ResolveCustomChain({ gCustomBreakableLongswordDL, gCustomLongswordDL, nullptr });
                        } else {
                            customDL = gCustomLongswordDL;
                        }
                        break;
                    }
                    case PLAYER_MODELTYPE_LH_HAMMER:
                        customDL = gCustomHammerDL;
                        break;
                    case PLAYER_MODELTYPE_LH_BOOMERANG:
                        if (!(player->stateFlags1 & PLAYER_STATE1_BOOMERANG_THROWN))
                            customDL = gCustomBoomerangDL;
                        break;
                }
                Gfx* resolvedCustom = LoadCustomGfx(customDL);
                if (resolvedCustom) {
                    Gfx* resolvedHand =
                        LoadGfxByName(isAdult ? gLinkAdultLeftHandClosedNearDL : gLinkChildLeftFistNearDL);
                    if (resolvedHand) {
                        const u8 lht = (u8)player->leftHandType;
                        const bool scaleHand = IsScalingAdultItemAsChild() &&
                                               ((gSaveContext.equips.buttonItems[0] != ITEM_SWORD_KOKIRI &&
                                                 lht == PLAYER_MODELTYPE_LH_SWORD) ||
                                                lht == PLAYER_MODELTYPE_LH_BGS || lht == PLAYER_MODELTYPE_LH_HAMMER);
                        BuildHandItemDL(play, dList, resolvedHand, resolvedCustom, scaleHand);
                    }
                }
                break;
            }

            case PLAYER_LIMB_R_HAND: {
                if (player->unk_6AD == 2) {
                    const char* fpsHand = nullptr;
                    const char* fpsWeapon = nullptr;

                    switch (player->rightHandType) {
                        case PLAYER_MODELTYPE_RH_BOW_SLINGSHOT:
                        case PLAYER_MODELTYPE_RH_BOW_SLINGSHOT_2:
                            fpsHand = isAdult ? gCustomAdultFPSHandDL : gCustomChildFPSHandDL;
                            fpsWeapon =
                                Player_HoldsBow(player)
                                    ? ResolveCustomChain({ gCustomFPSBowDL, gCustomBowDL, nullptr })
                                    : ResolveCustomChain({ gCustomFPSSlingshotDL, gCustomSlingshotDL, nullptr });
                            break;
                        case PLAYER_MODELTYPE_RH_HOOKSHOT:
                            fpsHand = isAdult ? gCustomAdultFPSHandDL : gCustomChildFPSHandDL;
                            fpsWeapon = (player->heldItemAction == PLAYER_IA_HOOKSHOT)
                                            ? ResolveCustomChain({ gCustomFPSHookshotDL, gCustomHookshotDL, nullptr })
                                            : ResolveCustomChain({ gCustomFPSLongshotDL, gCustomLongshotDL, nullptr });
                            break;
                    }

                    Gfx* resolvedFpsWeapon = LoadCustomGfx(fpsWeapon);
                    Gfx* resolvedFpsHand = LoadCustomGfx(fpsHand);
                    if (resolvedFpsWeapon) {
                        Gfx* buf = (Gfx*)Graph_Alloc(play->state.gfxCtx, 3 * sizeof(Gfx));
                        Gfx* p = buf;
                        if (resolvedFpsWeapon)
                            gSPDisplayList(p++, resolvedFpsWeapon);
                        if (resolvedFpsHand)
                            gSPDisplayList(p++, resolvedFpsHand);
                        gSPEndDisplayList(p);
                        *dList = buf;
                    }
                } else {
                    bool useOpenHand = false;
                    const bool holdsTwoHanded = player->heldItemAction >= PLAYER_IA_SWORD_BIGGORON &&
                                                player->heldItemAction <= PLAYER_IA_HAMMER;
                    const bool isChildHylian = !isAdult && player->currentShield == PLAYER_SHIELD_HYLIAN;
                    const bool isShielding = (player->stateFlags1 & PLAYER_STATE1_SHIELDING) != 0 && !isChildHylian &&
                                             (!holdsTwoHanded || (CVarGetInteger(CVAR_CHEAT("ShieldTwoHanded"), 0) &&
                                                                  player->heldItemAction != PLAYER_IA_DEKU_STICK));
                    const bool isOcarina = player->heldItemAction == PLAYER_IA_OCARINA_FAIRY ||
                                           player->heldItemAction == PLAYER_IA_OCARINA_OF_TIME ||
                                           player->itemAction == PLAYER_IA_OCARINA_FAIRY ||
                                           player->itemAction == PLAYER_IA_OCARINA_OF_TIME ||
                                           player->modelGroup == PLAYER_MODELGROUP_OCARINA ||
                                           player->modelGroup == PLAYER_MODELGROUP_OOT;
                    if (isShielding) {
                        switch (player->currentShield) {
                            case PLAYER_SHIELD_DEKU:
                                customDL = gCustomDekuShieldDL;
                                break;
                            case PLAYER_SHIELD_HYLIAN:
                                customDL = gCustomHylianShieldDL;
                                break;
                            case PLAYER_SHIELD_MIRROR:
                                customDL = gCustomMirrorShieldDL;
                                break;
                        }
                    } else if (isOcarina) {
                        const bool isOoT = player->heldItemAction == PLAYER_IA_OCARINA_OF_TIME ||
                                           player->itemAction == PLAYER_IA_OCARINA_OF_TIME ||
                                           player->modelGroup == PLAYER_MODELGROUP_OOT;
                        customDL = isOoT ? (isAdult ? gCustomOcarinaOfTimeAdultDL : gCustomOcarinaOfTimeDL)
                                         : (isAdult ? gCustomFairyOcarinaAdultDL : gCustomFairyOcarinaDL);
                        useOpenHand = true;
                    } else {
                        switch ((u8)player->rightHandType) {
                            case PLAYER_MODELTYPE_RH_SHIELD:
                                switch (player->currentShield) {
                                    case PLAYER_SHIELD_DEKU:
                                        customDL = gCustomDekuShieldDL;
                                        break;
                                    case PLAYER_SHIELD_HYLIAN:
                                        customDL = gCustomHylianShieldDL;
                                        break;
                                    case PLAYER_SHIELD_MIRROR:
                                        customDL = gCustomMirrorShieldDL;
                                        break;
                                }
                                break;
                            case PLAYER_MODELTYPE_RH_BOW_SLINGSHOT:
                            case PLAYER_MODELTYPE_RH_BOW_SLINGSHOT_2:
                                customDL = Player_HoldsBow(player) ? gCustomBowDL : gCustomSlingshotDL;
                                break;
                            case PLAYER_MODELTYPE_RH_HOOKSHOT:
                                customDL = (player->heldItemAction == PLAYER_IA_HOOKSHOT) ? gCustomHookshotDL
                                                                                          : gCustomLongshotDL;
                                break;
                            case PLAYER_MODELTYPE_RH_OCARINA:
                                customDL = isAdult ? gCustomFairyOcarinaAdultDL : gCustomFairyOcarinaDL;
                                useOpenHand = true;
                                break;
                            case PLAYER_MODELTYPE_RH_OOT:
                                customDL = isAdult ? gCustomOcarinaOfTimeAdultDL : gCustomOcarinaOfTimeDL;
                                useOpenHand = true;
                                break;
                        }
                    }
                    Gfx* resolvedCustom = LoadCustomGfx(customDL);
                    if (resolvedCustom) {
                        const char* handPath =
                            useOpenHand ? (isAdult ? gLinkAdultRightHandNearDL : gLinkChildRightHandNearDL)
                                        : (isAdult ? gLinkAdultRightHandClosedNearDL : gLinkChildRightHandClosedNearDL);
                        Gfx* resolvedHand = LoadGfxByName(handPath);
                        if (resolvedHand) {
                            const u8 rht = (u8)player->rightHandType;
                            const bool scaleHand =
                                IsScalingAdultItemAsChild() && !isOcarina &&
                                ((player->currentShield == PLAYER_SHIELD_MIRROR &&
                                  (isShielding || rht == PLAYER_MODELTYPE_RH_SHIELD)) ||
                                 (!isShielding &&
                                  (rht == PLAYER_MODELTYPE_RH_HOOKSHOT ||
                                   (rht == PLAYER_MODELTYPE_RH_BOW_SLINGSHOT && Player_HoldsBow(player)))));
                            BuildHandItemDL(play, dList, resolvedHand, resolvedCustom, scaleHand);
                        }
                    }
                }
                break;
            }

            case PLAYER_LIMB_SHEATH: {
                u8 sheathType = (u8)player->sheathType;
                const bool isOcarinaSheath =
                    player->heldItemAction == PLAYER_IA_OCARINA_FAIRY ||
                    player->heldItemAction == PLAYER_IA_OCARINA_OF_TIME ||
                    player->itemAction == PLAYER_IA_OCARINA_FAIRY || player->itemAction == PLAYER_IA_OCARINA_OF_TIME ||
                    player->modelGroup == PLAYER_MODELGROUP_OCARINA || player->modelGroup == PLAYER_MODELGROUP_OOT;
                if (isOcarinaSheath) {
                    const bool hasShieldEquipped = player->currentShield != PLAYER_SHIELD_NONE;
                    sheathType = hasShieldEquipped ? PLAYER_MODELTYPE_SHEATH_18 : PLAYER_MODELTYPE_SHEATH_16;
                } else if (player->stateFlags1 & PLAYER_STATE1_SHIELDING) {
                    const bool sheathTwoHanded = player->heldItemAction >= PLAYER_IA_SWORD_BIGGORON &&
                                                 player->heldItemAction <= PLAYER_IA_HAMMER;
                    const bool sheathChildHylian = !isAdult && player->currentShield == PLAYER_SHIELD_HYLIAN;
                    const bool sheathCanShield =
                        !sheathChildHylian && (!sheathTwoHanded || (CVarGetInteger(CVAR_CHEAT("ShieldTwoHanded"), 0) &&
                                                                    player->heldItemAction != PLAYER_IA_DEKU_STICK));
                    if (sheathCanShield) {
                        if (sheathType == PLAYER_MODELTYPE_SHEATH_18)
                            sheathType = PLAYER_MODELTYPE_SHEATH_16;
                        else if (sheathType == PLAYER_MODELTYPE_SHEATH_19)
                            sheathType = PLAYER_MODELTYPE_SHEATH_17;
                    }
                }
                const bool hasSword =
                    (sheathType == PLAYER_MODELTYPE_SHEATH_16 || sheathType == PLAYER_MODELTYPE_SHEATH_18);
                const bool hasShield =
                    (sheathType == PLAYER_MODELTYPE_SHEATH_18 || sheathType == PLAYER_MODELTYPE_SHEATH_19);
                const bool emptySheath =
                    (sheathType == PLAYER_MODELTYPE_SHEATH_17 || sheathType == PLAYER_MODELTYPE_SHEATH_19);

                const char* swordPath = hasSword      ? GetSwordInSheathDLForPlayer(player, play)
                                        : emptySheath ? GetSheathOnlyDLForPlayer(player, play)
                                                      : nullptr;
                const char* shieldPath = hasShield ? GetShieldOnBackDL(player->currentShield) : nullptr;

                Gfx* resolvedSword = LoadCustomGfx(swordPath);
                Gfx* resolvedShield = LoadCustomGfx(shieldPath);
                if (resolvedSword || resolvedShield) {
                    Gfx* buf = (Gfx*)Graph_Alloc(play->state.gfxCtx, 3 * sizeof(Gfx));
                    Gfx* p = buf;
                    if (resolvedSword)
                        gSPDisplayList(p++, resolvedSword);
                    if (resolvedShield)
                        gSPDisplayList(p++, resolvedShield);
                    gSPEndDisplayList(p);
                    *dList = buf;
                }
                break;
            }

            default:
                break;
        }
    });

    // Pause/equipment screen character
    COND_VB_SHOULD(VB_PLAYER_OVERRIDE_LIMB_DRAW_PAUSE, CVarGetInteger(CVAR_SETTING("AltAssets"), 1), {
        s32 limbIndex = va_arg(args, s32);
        Gfx** dList = va_arg(args, Gfx**);
        Player* player = (Player*)va_arg(args, void*);
        PlayState* play = va_arg(args, PlayState*);

        const bool isAdult = gSaveContext.linkAge == LINK_AGE_ADULT;
        const char* customDL = nullptr;

        switch (limbIndex) {
            case PLAYER_LIMB_L_HAND: {
                switch (PauseGetLimbType(PLAYER_LIMB_L_HAND)) {
                    case PLAYER_MODELTYPE_LH_SWORD: {
                        const u8 swordEquip = CUR_EQUIP_VALUE(EQUIP_TYPE_SWORD);
                        if (swordEquip == EQUIP_VALUE_SWORD_KOKIRI)
                            customDL = gCustomKokiriSwordDL;
                        else
                            customDL = gCustomMasterSwordDL;
                        break;
                    }
                    case PLAYER_MODELTYPE_LH_BGS: {
                        if (gSaveContext.bgsFlag) {
                            customDL = gCustomLongswordDL;
                        } else if (CHECK_OWNED_EQUIP_ALT(EQUIP_TYPE_SWORD, EQUIP_INV_SWORD_BROKENGIANTKNIFE)) {
                            customDL = gCustomBrokenLongswordDL;
                        } else {
                            customDL = ResolveCustomChain({ gCustomBreakableLongswordDL, gCustomLongswordDL, nullptr });
                        }
                        break;
                    }
                }
                Gfx* resolvedCustom = LoadCustomGfx(customDL);
                if (resolvedCustom) {
                    Gfx* resolvedHand =
                        LoadGfxByName(isAdult ? gLinkAdultLeftHandClosedNearDL : gLinkChildLeftFistNearDL);
                    if (resolvedHand) {
                        Gfx* buf = (Gfx*)Graph_Alloc(play->state.gfxCtx, 3 * sizeof(Gfx));
                        Gfx* p = buf;
                        gSPDisplayList(p++, resolvedHand);
                        gSPDisplayList(p++, resolvedCustom);
                        gSPEndDisplayList(p);
                        *dList = buf;
                    }
                }
                break;
            }

            case PLAYER_LIMB_R_HAND: {
                if (PauseGetLimbType(PLAYER_LIMB_R_HAND) == PLAYER_MODELTYPE_RH_SHIELD) {
                    switch (CUR_EQUIP_VALUE(EQUIP_TYPE_SHIELD)) {
                        case PLAYER_SHIELD_DEKU:
                            customDL = gCustomDekuShieldDL;
                            break;
                        case PLAYER_SHIELD_HYLIAN:
                            customDL = gCustomHylianShieldDL;
                            break;
                        case PLAYER_SHIELD_MIRROR:
                            customDL = gCustomMirrorShieldDL;
                            break;
                    }
                }
                Gfx* resolvedCustom = LoadCustomGfx(customDL);
                if (resolvedCustom) {
                    Gfx* resolvedHand =
                        LoadGfxByName(isAdult ? gLinkAdultRightHandClosedNearDL : gLinkChildRightHandClosedNearDL);
                    if (resolvedHand) {
                        Gfx* buf = (Gfx*)Graph_Alloc(play->state.gfxCtx, 3 * sizeof(Gfx));
                        Gfx* p = buf;
                        gSPDisplayList(p++, resolvedHand);
                        gSPDisplayList(p++, resolvedCustom);
                        gSPEndDisplayList(p);
                        *dList = buf;
                    }
                }
                break;
            }

            case PLAYER_LIMB_SHEATH: {
                const u8 sheathType = PauseGetLimbType(PLAYER_LIMB_SHEATH);
                const bool hasSword =
                    (sheathType == PLAYER_MODELTYPE_SHEATH_16 || sheathType == PLAYER_MODELTYPE_SHEATH_18);
                const bool hasShield =
                    (sheathType == PLAYER_MODELTYPE_SHEATH_18 || sheathType == PLAYER_MODELTYPE_SHEATH_19);
                const bool emptySheath =
                    (sheathType == PLAYER_MODELTYPE_SHEATH_17 || sheathType == PLAYER_MODELTYPE_SHEATH_19);

                const char* swordPath = hasSword      ? GetSwordInSheathDL(play)
                                        : emptySheath ? GetSheathOnlyDL(play)
                                                      : nullptr;
                const char* shieldPath = hasShield ? GetShieldOnBackDL(CUR_EQUIP_VALUE(EQUIP_TYPE_SHIELD)) : nullptr;

                Gfx* resolvedSword = LoadCustomGfx(swordPath);
                Gfx* resolvedShield = LoadCustomGfx(shieldPath);
                if (resolvedSword || resolvedShield) {
                    Gfx* buf = (Gfx*)Graph_Alloc(play->state.gfxCtx, 3 * sizeof(Gfx));
                    Gfx* p = buf;
                    if (resolvedSword)
                        gSPDisplayList(p++, resolvedSword);
                    if (resolvedShield)
                        gSPDisplayList(p++, resolvedShield);
                    gSPEndDisplayList(p);
                    *dList = buf;
                }
                break;
            }

            default:
                break;
        }
    });

    COND_VB_SHOULD(VB_DRAW_HOOKSHOT_TIP, CVarGetInteger(CVAR_SETTING("AltAssets"), 1), {
        Player* player = va_arg(args, Player*);
        PlayState* play = va_arg(args, PlayState*);
        const char* tipPath = (player->heldItemAction == PLAYER_IA_LONGSHOT)
                                  ? ResolveCustomChain({ gCustomLongshotTipDL, gCustomHookshotTipDL, nullptr })
                                  : gCustomHookshotTipDL;
        Gfx* resolvedTip = LoadCustomGfx(tipPath);
        if (resolvedTip) {
            *should = false;
            gSPDisplayList(play->state.gfxCtx->polyOpa.p++, resolvedTip);
        }
    });

    COND_VB_SHOULD(VB_DRAW_HOOKSHOT_CHAIN, CVarGetInteger(CVAR_SETTING("AltAssets"), 1), {
        Player* player = va_arg(args, Player*);
        PlayState* play = va_arg(args, PlayState*);
        const char* chainPath = (player->heldItemAction == PLAYER_IA_LONGSHOT)
                                    ? ResolveCustomChain({ gCustomLongshotChainDL, gCustomHookshotChainDL, nullptr })
                                    : gCustomHookshotChainDL;
        Gfx* resolvedChain = LoadCustomGfx(chainPath);
        if (resolvedChain) {
            *should = false;
            gSPDisplayList(play->state.gfxCtx->polyOpa.p++, resolvedChain);
        }
    });

    COND_VB_SHOULD(VB_PLAYER_DRAW_BOTTLE, CVarGetInteger(CVAR_SETTING("AltAssets"), 1), {
        Player* player = va_arg(args, Player*);
        PlayState* play = va_arg(args, PlayState*);
        const char* contentDL = nullptr;
        Gfx* resolvedContent = nullptr;
        Gfx* resolvedBottle = LoadCustomGfx(gCustomBottleDL);
        if (resolvedBottle) {
            *should = false;
            gSPDisplayList(play->state.gfxCtx->polyXlu.p++, resolvedBottle);

            if (player->itemAction >= PLAYER_IA_BOTTLE &&
                player->itemAction < PLAYER_IA_BOTTLE + std::size(bottleContentDLs)) {
                contentDL = bottleContentDLs[player->itemAction - PLAYER_IA_BOTTLE];
            }

            if (contentDL) {
                resolvedContent = LoadCustomGfx(contentDL);
            }

            if (resolvedContent) {
                gSPDisplayList(play->state.gfxCtx->polyOpa.p++, resolvedContent);
            }
        }
    });

    COND_VB_SHOULD(VB_PLAYER_UPDATE_BOTTLE_HELD, CVarGetInteger(CVAR_SETTING("AltAssets"), 1), {
        Player* player = va_arg(args, Player*);
        const bool isFullMilk = player->itemAction == PLAYER_IA_BOTTLE_MILK_FULL;
        if (isFullMilk) {
            *should = false;
            player->itemAction = PLAYER_IA_BOTTLE_MILK_HALF;
        }
    });
}

static RegisterShipInitFunc initFunc(RegisterCustomEquipment, { CVAR_SETTING("AltAssets") });
