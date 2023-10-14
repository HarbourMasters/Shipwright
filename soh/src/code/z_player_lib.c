#include "global.h"
#include "objects/gameplay_keep/gameplay_keep.h"
#include "objects/gameplay_field_keep/gameplay_field_keep.h"
#include "objects/object_link_boy/object_link_boy.h"
#include "objects/object_link_child/object_link_child.h"
#include "objects/object_triforce_spot/object_triforce_spot.h"
#include "overlays/actors/ovl_Demo_Effect/z_demo_effect.h"

#include "soh/Enhancements/game-interactor/GameInteractor.h"
#include "soh/Enhancements/randomizer/draw.h"

#include <stdlib.h>

typedef struct {
    /* 0x00 */ u8 flag;
    /* 0x02 */ u16 textId;
} TextTriggerEntry; // size = 0x04

typedef struct {
    /* 0x00 */ void* dList;
    /* 0x04 */ Vec3f pos;
} BowStringData; // size = 0x10

FlexSkeletonHeader* gPlayerSkelHeaders[] = { &gLinkAdultSkel, &gLinkChildSkel };

s16 sBootData[PLAYER_BOOTS_MAX][17] = {
    { 200, 1000, 300, 700, 550, 270, 600, 350, 800, 600, -100, 600, 590, 750, 125, 200, 130 },
    { 200, 1000, 300, 700, 550, 270, 1000, 0, 800, 300, -160, 600, 590, 750, 125, 200, 130 },
    { 200, 1000, 300, 700, 550, 270, 600, 600, 800, 550, -100, 600, 540, 270, 25, 0, 130 },
    { 200, 1000, 300, 700, 380, 400, 0, 300, 800, 500, -100, 600, 590, 750, 125, 200, 130 },
    { 80, 800, 150, 700, 480, 270, 600, 50, 800, 550, -40, 400, 540, 270, 25, 0, 80 },
    { 200, 1000, 300, 800, 500, 400, 800, 400, 800, 550, -100, 600, 540, 750, 125, 400, 200 },
};

// Used to map action params to model groups
u8 sActionModelGroups[] = {
    3,  15, 10, 2,  2,  5,  10, 11, 6,  6, 6, 6, 6, 6, 6, 6, 9, 9, 7, 7, 8, 3, 3, 6, 3, 3, 3, 3, 12, 13, 14, 14, 14, 14,
    14, 14, 14, 14, 14, 14, 14, 14, 14, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,  3,  3,  3,  3,   3,
    3,   3,  3,  3,  3,  3,  3,  3
};

TextTriggerEntry sTextTriggers[] = {
    { 1, 0x3040 },
    { 2, 0x401D },
    { 0, 0x0000 },
    { 2, 0x401D },
};

// Used to map model groups to model types for [animation, left hand, right hand, sheath, waist]
u8 gPlayerModelTypes[][5] = {
    { 2, 0, 10, 16, 20 }, { 1, 2, 9, 19, 20 },  { 1, 2, 10, 17, 20 }, { 0, 0, 8, 18, 20 },
    { 0, 0, 8, 18, 20 },  { 3, 4, 9, 19, 20 },  { 4, 1, 11, 18, 20 }, { 5, 0, 8, 18, 20 },
    { 0, 6, 8, 18, 20 },  { 4, 0, 15, 18, 20 }, { 3, 1, 9, 18, 20 },  { 3, 5, 9, 18, 20 },
    { 0, 0, 13, 18, 20 }, { 0, 0, 14, 18, 20 }, { 0, 7, 8, 18, 20 },  { 0, 2, 8, 19, 20 },
};

Gfx* sPlayerRightHandShieldDLs[] = {
    gLinkAdultRightHandClosedNearDL,
    gLinkChildRightHandClosedNearDL,
    gLinkAdultRightHandClosedFarDL,
    gLinkChildRightHandClosedFarDL,
    gLinkAdultRightHandClosedNearDL,
    gLinkChildRightFistAndDekuShieldNearDL,
    gLinkAdultRightHandClosedFarDL,
    gLinkChildRightFistAndDekuShieldFarDL,
    gLinkAdultRightHandHoldingHylianShieldNearDL,
    gLinkChildRightHandClosedNearDL,
    gLinkAdultRightHandHoldingHylianShieldFarDL,
    gLinkChildRightHandClosedFarDL,
    gLinkAdultRightHandHoldingMirrorShieldNearDL,
    gLinkChildRightHandClosedNearDL,
    gLinkAdultRightHandHoldingMirrorShieldFarDL,
    gLinkChildRightHandClosedFarDL,
};

Gfx* sSheathWithSwordDLs[] = {
    gLinkAdultMasterSwordAndSheathNearDL,       gLinkChildSwordAndSheathNearDL,
    gLinkAdultMasterSwordAndSheathFarDL,        gLinkChildSwordAndSheathFarDL,
    gLinkAdultMasterSwordAndSheathNearDL,       gLinkChildDekuShieldSwordAndSheathNearDL,
    gLinkAdultMasterSwordAndSheathFarDL,        gLinkChildDekuShieldSwordAndSheathFarDL,
    gLinkAdultHylianShieldSwordAndSheathNearDL, gLinkChildHylianShieldSwordAndSheathNearDL,
    gLinkAdultHylianShieldSwordAndSheathFarDL,  gLinkChildHylianShieldSwordAndSheathFarDL,
    gLinkAdultMirrorShieldSwordAndSheathNearDL, gLinkChildSwordAndSheathNearDL,
    gLinkAdultMirrorShieldSwordAndSheathFarDL,  gLinkChildSwordAndSheathFarDL,
    NULL,                                       NULL,
    NULL,                                       NULL,
    NULL,                                       gLinkChildDekuShieldWithMatrixDL,
    NULL,                                       gLinkChildDekuShieldWithMatrixDL,
};

Gfx* sSheathWithoutSwordDLs[] = {
    gLinkAdultSheathNearDL,
    gLinkChildSheathNearDL,
    gLinkAdultSheathFarDL,
    gLinkChildSheathFarDL,
    gLinkAdultSheathNearDL,
    gLinkChildDekuShieldAndSheathNearDL,
    gLinkAdultSheathFarDL,
    gLinkChildDekuShieldAndSheathFarDL,
    gLinkAdultHylianShieldAndSheathNearDL,
    gLinkChildHylianShieldAndSheathNearDL,
    gLinkAdultHylianShieldAndSheathFarDL,
    gLinkChildHylianShieldAndSheathFarDL,
    gLinkAdultMirrorShieldAndSheathNearDL,
    gLinkChildSheathNearDL,
    gLinkAdultMirrorShieldAndSheathFarDL,
    gLinkChildSheathFarDL,
    NULL,
    NULL,
    NULL,
    NULL,
    gLinkAdultSheathNearDL,
    gLinkChildDekuShieldWithMatrixDL,
    gLinkAdultSheathNearDL,
    gLinkChildDekuShieldWithMatrixDL,
};

Gfx* gPlayerLeftHandBgsDLs[] = {
    gLinkAdultLeftHandHoldingBgsNearDL,          gLinkChildLeftHandHoldingMasterSwordDL,
    gLinkAdultLeftHandHoldingBgsFarDL,           gLinkChildLeftHandHoldingMasterSwordDL,
    gLinkAdultHandHoldingBrokenGiantsKnifeDL,    gLinkChildLeftHandHoldingMasterSwordDL,
    gLinkAdultHandHoldingBrokenGiantsKnifeFarDL, gLinkChildLeftHandHoldingMasterSwordDL,
};

Gfx* gPlayerLeftHandOpenDLs[] = {
    gLinkAdultLeftHandNearDL,
    gLinkChildLeftHandNearDL,
    gLinkAdultLeftHandFarDL,
    gLinkChildLeftHandFarDL,
};

Gfx* gPlayerLeftHandClosedDLs[] = {
    gLinkAdultLeftHandClosedNearDL,
    gLinkChildLeftFistNearDL,
    gLinkAdultLeftHandClosedFarDL,
    gLinkChildLeftFistFarDL,
};

Gfx* sPlayerLeftHandSwordDLs2[] = {
    gLinkAdultLeftHandHoldingMasterSwordNearDL,
    gLinkChildLeftFistAndKokiriSwordNearDL,
    gLinkAdultLeftHandHoldingMasterSwordFarDL,
    gLinkChildLeftFistAndKokiriSwordFarDL,
};

Gfx* sPlayerLeftHandSwordDLs[] = {
    gLinkAdultLeftHandHoldingMasterSwordNearDL,
    gLinkChildLeftFistAndKokiriSwordNearDL,
    gLinkAdultLeftHandHoldingMasterSwordFarDL,
    gLinkChildLeftFistAndKokiriSwordFarDL,
};

Gfx* sPlayerRightHandOpenDLs[] = {
    gLinkAdultRightHandNearDL,
    gLinkChildRightHandNearDL,
    gLinkAdultRightHandFarDL,
    gLinkChildRightHandFarDL,
};

Gfx* sPlayerRightHandClosedDLs[] = {
    gLinkAdultRightHandClosedNearDL,
    gLinkChildRightHandClosedNearDL,
    gLinkAdultRightHandClosedFarDL,
    gLinkChildRightHandClosedFarDL,
};

Gfx* sPlayerRightHandBowSlingshotDLs[] = {
    gLinkAdultRightHandHoldingBowNearDL,
    gLinkChildRightHandHoldingSlingshotNearDL,
    gLinkAdultRightHandHoldingBowFarDL,
    gLinkChildRightHandHoldingSlingshotFarDL,
};

Gfx* sSwordAndSheathDLs[] = {
    gLinkAdultMasterSwordAndSheathNearDL,
    gLinkChildSwordAndSheathNearDL,
    gLinkAdultMasterSwordAndSheathFarDL,
    gLinkChildSwordAndSheathFarDL,
};

Gfx* sSheathDLs[] = {
    gLinkAdultSheathNearDL,
    gLinkChildSheathNearDL,
    gLinkAdultSheathFarDL,
    gLinkChildSheathFarDL,
};

Gfx* sPlayerWaistDLs[] = {
    gLinkAdultWaistNearDL,
    gLinkChildWaistNearDL,
    gLinkAdultWaistFarDL,
    gLinkChildWaistFarDL,
};

Gfx* sPlayerRightHandBowSlingshotDLs2[] = {
    gLinkAdultRightHandHoldingBowNearDL,
    gLinkChildRightHandHoldingSlingshotNearDL,
    gLinkAdultRightHandHoldingBowFarDL,
    gLinkChildRightHandHoldingSlingshotFarDL,
};

Gfx* sPlayerRightHandOcarinaDLs[] = {
    gLinkAdultRightHandHoldingOotNearDL,
    gLinkChildRightHandHoldingFairyOcarinaNearDL,
    gLinkAdultRightHandHoldingOotFarDL,
    gLinkChildRightHandHoldingFairyOcarinaFarDL,
};

Gfx* sPlayerRightHandOotDLs[] = {
    gLinkAdultRightHandHoldingOotNearDL,
    gLinkChildRightHandAndOotNearDL,
    gLinkAdultRightHandHoldingOotFarDL,
    gLinkChildRightHandHoldingOOTFarDL,
};

Gfx* sPlayerRightHandHookshotDLs[] = {
    gLinkAdultRightHandHoldingHookshotNearDL,
    gLinkChildRightHandNearDL,
    gLinkAdultRightHandHoldingHookshotNearDL, // The 'far' display list exists but is not used
    gLinkChildRightHandFarDL,
};

Gfx* sPlayerLeftHandHammerDLs[] = {
    gLinkAdultLeftHandHoldingHammerNearDL,
    gLinkChildLeftHandNearDL,
    gLinkAdultLeftHandHoldingHammerFarDL,
    gLinkChildLeftHandFarDL,
};

Gfx* gPlayerLeftHandBoomerangDLs[] = {
    gLinkAdultLeftHandNearDL,
    gLinkChildLeftFistAndBoomerangNearDL,
    gLinkAdultLeftHandFarDL,
    gLinkChildLeftFistAndBoomerangFarDL,
};

Gfx* sPlayerLeftHandBottleDLs[] = {
    gLinkAdultLeftHandOutNearDL,
    gLinkChildLeftHandUpNearDL,
    gLinkAdultLeftHandOutNearDL,
    gLinkChildLeftHandUpNearDL,
};

Gfx* sFirstPersonLeftForearmDLs[] = {
    gLinkAdultRightArmOutNearDL,
    NULL,
};

Gfx* sFirstPersonLeftHandDLs[] = {
    gLinkAdultRightHandOutNearDL,
    NULL,
};

Gfx* sFirstPersonRightShoulderDLs[] = {
    gLinkAdultRightShoulderNearDL,
    gLinkChildRightShoulderNearDL,
};

Gfx* sFirstPersonForearmDLs[] = {
    gLinkAdultLeftArmOutNearDL,
    NULL,
};

Gfx* sFirstPersonRightHandHoldingWeaponDLs[] = {
    gLinkAdultRightHandHoldingBowFirstPersonDL,
    gLinkChildRightArmStretchedSlingshotDL,
};

// Indexed by model types (left hand, right hand, sheath or waist)
Gfx** sPlayerDListGroups[] = {
    gPlayerLeftHandOpenDLs, gPlayerLeftHandClosedDLs, sPlayerLeftHandSwordDLs, sPlayerLeftHandSwordDLs2, gPlayerLeftHandBgsDLs, sPlayerLeftHandHammerDLs, gPlayerLeftHandBoomerangDLs,
    sPlayerLeftHandBottleDLs, sPlayerRightHandOpenDLs, sPlayerRightHandClosedDLs, sPlayerRightHandShieldDLs, sPlayerRightHandBowSlingshotDLs, sPlayerRightHandBowSlingshotDLs2, sPlayerRightHandOcarinaDLs,
    sPlayerRightHandOotDLs, sPlayerRightHandHookshotDLs, sSwordAndSheathDLs, sSheathDLs, sSheathWithSwordDLs, sSheathWithoutSwordDLs, sPlayerWaistDLs,
};

Gfx gCullBackDList[] = {
    gsSPSetGeometryMode(G_CULL_BACK),
    gsSPEndDisplayList(),
};

Gfx gCullFrontDList[] = {
    gsSPSetGeometryMode(G_CULL_FRONT),
    gsSPEndDisplayList(),
};

Vec3f* D_80160000;
s32 sDListsLodOffset;
Vec3f sGetItemRefPos;
s32 sLeftHandType;
s32 sRightHandType;

void Player_SetBootData(PlayState* play, Player* this) {
    s32 currentBoots;
    s16* bootRegs;

    REG(27) = 2000;
    REG(48) = 370;

    currentBoots = this->currentBoots;
    if (currentBoots == PLAYER_BOOTS_KOKIRI) {
        if (!LINK_IS_ADULT) {
            currentBoots = PLAYER_BOOTS_KOKIRI_CHILD;
        }
    } else if (currentBoots == PLAYER_BOOTS_IRON) {
        if (this->stateFlags1 & 0x8000000) {
            currentBoots = PLAYER_BOOTS_IRON_UNDERWATER;
        }
        REG(27) = 500;
        REG(48) = 100;
    }

    bootRegs = sBootData[currentBoots];
    REG(19) = bootRegs[0];
    REG(30) = bootRegs[1];
    REG(32) = bootRegs[2];
    REG(34) = bootRegs[3];
    REG(35) = bootRegs[4];
    REG(36) = bootRegs[5];
    REG(37) = bootRegs[6];
    REG(38) = bootRegs[7];
    REG(43) = bootRegs[8];
    REG(45) = bootRegs[9];
    REG(68) = bootRegs[10];
    REG(69) = bootRegs[11];
    IREG(66) = bootRegs[12];
    IREG(67) = bootRegs[13];
    IREG(68) = bootRegs[14];
    IREG(69) = bootRegs[15];
    MREG(95) = bootRegs[16];

    if (play->roomCtx.curRoom.behaviorType1 == ROOM_BEHAVIOR_TYPE1_2) {
        REG(45) = 500;
    }
}

// Custom method used to determine if we're using a custom model for link
uint8_t Player_IsCustomLinkModel() {
    return (LINK_IS_ADULT && ResourceGetIsCustomByName(gLinkAdultSkel)) ||
           (LINK_IS_CHILD && ResourceGetIsCustomByName(gLinkChildSkel));
}

s32 Player_InBlockingCsMode(PlayState* play, Player* this) {
    return (this->stateFlags1 & 0x20000080) || (this->csMode != 0) || (play->sceneLoadFlag == 0x14) ||
           (this->stateFlags1 & 1) || (this->stateFlags3 & 0x80) ||
           ((gSaveContext.magicState != 0) && (Player_ActionToMagicSpell(this, this->itemAction) >= 0));
}

s32 Player_InCsMode(PlayState* play) {
    Player* this = GET_PLAYER(play);

    return Player_InBlockingCsMode(play, this) || (this->unk_6AD == 4);
}

s32 func_8008E9C4(Player* this) {
    return (this->stateFlags1 & 0x10);
}

s32 Player_IsChildWithHylianShield(Player* this) {
    return gSaveContext.linkAge != 0 && (this->currentShield == PLAYER_SHIELD_HYLIAN);
}

s32 Player_ActionToModelGroup(Player* this, s32 actionParam) {
    s32 modelGroup = sActionModelGroups[actionParam];

    if ((modelGroup == 2) && Player_IsChildWithHylianShield(this)) {
        return 1;
    } else {
        return modelGroup;
    }
}

void Player_SetModelsForHoldingShield(Player* this) {
    if ((this->stateFlags1 & 0x400000) &&
        ((this->itemAction < 0) || (this->itemAction == this->heldItemAction))) {
        if ((CVarGetInteger("gShieldTwoHanded", 0) && (this->heldItemAction != PLAYER_IA_DEKU_STICK) ||
            !Player_HoldsTwoHandedWeapon(this)) && !Player_IsChildWithHylianShield(this)) {
            this->rightHandType = 10;
            this->rightHandDLists = &sPlayerDListGroups[10][gSaveContext.linkAge];
            if (this->sheathType == 18) {
                this->sheathType = 16;
            } else if (this->sheathType == 19) {
                this->sheathType = 17;
            }
            this->sheathDLists = &sPlayerDListGroups[this->sheathType][gSaveContext.linkAge];
            this->modelAnimType = 2;
            this->itemAction = -1;
        }
    }
}

void Player_SetModels(Player* this, s32 modelGroup) {
    // Left hand
    this->leftHandType = gPlayerModelTypes[modelGroup][1];
    this->leftHandDLists = &sPlayerDListGroups[this->leftHandType][gSaveContext.linkAge];
    
    // Right hand
    this->rightHandType = gPlayerModelTypes[modelGroup][2];
    this->rightHandDLists = &sPlayerDListGroups[this->rightHandType][gSaveContext.linkAge];

    if (CVarGetInteger("gBowSlingShotAmmoFix", 0) && this->rightHandType == 11) { // If holding Bow/Slingshot
        this->rightHandDLists = &sPlayerDListGroups[this->rightHandType][Player_HoldsSlingshot(this)];
    }

    // Sheath
    this->sheathType = gPlayerModelTypes[modelGroup][3];
    this->sheathDLists = &sPlayerDListGroups[this->sheathType][gSaveContext.linkAge];

    // Waist
    this->waistDLists = &sPlayerDListGroups[gPlayerModelTypes[modelGroup][4]][gSaveContext.linkAge];

    Player_SetModelsForHoldingShield(this);
}

void Player_SetModelGroup(Player* this, s32 modelGroup) {
    this->modelGroup = modelGroup;

    if (modelGroup == 1) {
        this->modelAnimType = 0;
    } else {
        this->modelAnimType = gPlayerModelTypes[modelGroup][0];
    }

    if ((this->modelAnimType < 3) && (this->currentShield == PLAYER_SHIELD_NONE)) {
        this->modelAnimType = 0;
    }

    Player_SetModels(this, modelGroup);
}

void func_8008EC70(Player* this) {
    this->itemAction = this->heldItemAction;
    Player_SetModelGroup(this, Player_ActionToModelGroup(this, this->heldItemAction));
    this->unk_6AD = 0;
}

void Player_SetEquipmentData(PlayState* play, Player* this) {
    if (this->csMode != 0x56) {
        this->currentShield = CUR_EQUIP_VALUE(EQUIP_SHIELD);
        this->currentTunic = CUR_EQUIP_VALUE(EQUIP_TUNIC) - 1;
        this->currentBoots = CUR_EQUIP_VALUE(EQUIP_BOOTS) - 1;
        this->currentSwordItemId = B_BTN_ITEM;
        Player_SetModelGroup(this, Player_ActionToModelGroup(this, this->heldItemAction));
        Player_SetBootData(play, this);
    }
}

void Player_UpdateBottleHeld(PlayState* play, Player* this, s32 item, s32 actionParam) {
    Inventory_UpdateBottleItem(play, item, this->heldItemButton);

    if (item != ITEM_BOTTLE) {
        this->heldItemId = item;
        this->heldItemAction = actionParam;
    }

    this->itemAction = actionParam;
}

void func_8008EDF0(Player* this) {
    this->unk_664 = NULL;
    this->stateFlags2 &= ~0x2000;
}

void func_8008EE08(Player* this) {
    if ((this->actor.bgCheckFlags & 1) || (this->stateFlags1 & 0x8A00000) ||
        (!(this->stateFlags1 & 0xC0000) && ((this->actor.world.pos.y - this->actor.floorHeight) < 100.0f))) {
        this->stateFlags1 &= ~0x400F8000;
    } else if (!(this->stateFlags1 & 0x2C0000)) {
        this->stateFlags1 |= 0x80000;
    }

    func_8008EDF0(this);
}

void func_8008EEAC(PlayState* play, Actor* actor) {
    Player* this = GET_PLAYER(play);

    func_8008EE08(this);
    this->unk_664 = actor;
    this->unk_684 = actor;
    this->stateFlags1 |= 0x10000;
    Camera_SetParam(Play_GetCamera(play, 0), 8, actor);
    Camera_ChangeMode(Play_GetCamera(play, 0), 2);
}

s32 func_8008EF30(PlayState* play) {
    Player* this = GET_PLAYER(play);

    return (this->stateFlags1 & 0x800000);
}

s32 func_8008EF44(PlayState* play, s32 ammo) {
    play->shootingGalleryStatus = ammo + 1;
    return 1;
}

s32 Player_IsBurningStickInRange(PlayState* play, Vec3f* pos, f32 xzRange, f32 yRange) {
    Player* this = GET_PLAYER(play);
    Vec3f diff;
    s32 pad;

    if ((this->heldItemAction == PLAYER_IA_DEKU_STICK) && (this->unk_860 != 0)) {
        Math_Vec3f_Diff(&this->meleeWeaponInfo[0].tip, pos, &diff);
        return ((SQ(diff.x) + SQ(diff.z)) <= SQ(xzRange)) && (0.0f <= diff.y) && (diff.y <= yRange);
    } else {
        return false;
    }
}

s32 Player_GetStrength(void) {
    s32 strengthUpgrade = CUR_UPG_VALUE(UPG_STRENGTH);

    if (CVarGetInteger("gTimelessEquipment", 0) || LINK_IS_ADULT) {
        return strengthUpgrade;
    } else if (strengthUpgrade != 0) {
        return PLAYER_STR_BRACELET;
    } else {
        return PLAYER_STR_NONE;
    }
}

u8 Player_GetMask(PlayState* play) {
    Player* this = GET_PLAYER(play);

    return this->currentMask;
}

Player* Player_UnsetMask(PlayState* play) {
    Player* this = GET_PLAYER(play);

    this->currentMask = PLAYER_MASK_NONE;

    return this;
}

s32 Player_HasMirrorShieldEquipped(PlayState* play) {
    Player* this = GET_PLAYER(play);

    return (this->currentShield == PLAYER_SHIELD_MIRROR);
}

s32 Player_HasMirrorShieldSetToDraw(PlayState* play) {
    Player* this = GET_PLAYER(play);

    return (this->rightHandType == 10) && (this->currentShield == PLAYER_SHIELD_MIRROR);
}

s32 Player_ActionToMagicSpell(Player* this, s32 actionParam) {
    s32 magicSpell = actionParam - PLAYER_IA_MAGIC_SPELL_15;

    if ((magicSpell >= 0) && (magicSpell < 6)) {
        return magicSpell;
    } else {
        return -1;
    }
}

s32 Player_HoldsHookshot(Player* this) {
    return (this->heldItemAction == PLAYER_IA_HOOKSHOT) || (this->heldItemAction == PLAYER_IA_LONGSHOT);
}

s32 Player_HoldsBow(Player* this) {
    switch(this->heldItemAction){
        case PLAYER_IA_BOW:
        case PLAYER_IA_BOW_FIRE:
        case PLAYER_IA_BOW_ICE:
        case PLAYER_IA_BOW_LIGHT:
            return true;
        default:
            return false;
    }
}

s32 Player_HoldsSlingshot(Player* this) {
    return this->heldItemAction == PLAYER_IA_SLINGSHOT;
}

s32 func_8008F128(Player* this) {
    return Player_HoldsHookshot(this) && (this->heldActor == NULL);
}

s32 Player_ActionToMeleeWeapon(s32 actionParam) {
    s32 sword = actionParam - PLAYER_IA_FISHING_POLE;

    if ((sword > 0) && (sword < 6)) {
        return sword;
    } else {
        return 0;
    }
}

s32 Player_GetMeleeWeaponHeld(Player* this) {
    return Player_ActionToMeleeWeapon(this->heldItemAction);
}

s32 Player_HoldsTwoHandedWeapon(Player* this) {
    if ((this->heldItemAction >= PLAYER_IA_SWORD_BIGGORON) && (this->heldItemAction <= PLAYER_IA_HAMMER)) {
        return 1;
    } else {
        return 0;
    }
}

s32 Player_HoldsBrokenKnife(Player* this) {
    return (this->heldItemAction == PLAYER_IA_SWORD_BIGGORON) && (gSaveContext.swordHealth <= 0.0f);
}

s32 Player_ActionToBottle(Player* this, s32 actionParam) {
    s32 bottle = actionParam - PLAYER_IA_BOTTLE;

    if ((bottle >= 0) && (bottle < 13)) {
        return bottle;
    } else {
        return -1;
    }
}

s32 Player_GetBottleHeld(Player* this) {
    return Player_ActionToBottle(this, this->heldItemAction);
}

s32 Player_ActionToExplosive(Player* this, s32 actionParam) {
    s32 explosive = actionParam - PLAYER_IA_BOMB;

    if ((explosive >= 0) && (explosive < 2)) {
        return explosive;
    } else {
        return -1;
    }
}

s32 Player_GetExplosiveHeld(Player* this) {
    return Player_ActionToExplosive(this, this->heldItemAction);
}

s32 func_8008F2BC(Player* this, s32 actionParam) {
    s32 sword = 0;

    if (actionParam != PLAYER_IA_LAST_USED) {
        sword = actionParam - PLAYER_IA_SWORD_MASTER;
        if ((sword < 0) || (sword >= 3)) {
            goto return_neg;
        }
    }

    return sword;

return_neg:
    return -1;
}

s32 Player_GetEnvironmentalHazard(PlayState* play) {
    Player* this = GET_PLAYER(play);
    TextTriggerEntry* triggerEntry;
    s32 var;

    if (play->roomCtx.curRoom.behaviorType2 == ROOM_BEHAVIOR_TYPE2_3) { // Room is hot
        var = 0;
    } else if ((this->unk_840 > 80) &&
               ((this->currentBoots == PLAYER_BOOTS_IRON) || (this->unk_840 >= 300))) { // Deep underwater
        var = ((this->currentBoots == PLAYER_BOOTS_IRON) && (this->actor.bgCheckFlags & 1)) ? 1 : 3;
    } else if (this->stateFlags1 & 0x8000000) { // Swimming
        var = 2;
    } else {
        return 0;
    }

    // Trigger general textboxes under certain conditions, like "It's so hot in here!"
    if (!Player_InCsMode(play)) {
        triggerEntry = &sTextTriggers[var];

        if ((triggerEntry->flag != 0) && !(gSaveContext.textTriggerFlags & triggerEntry->flag) &&
            (((var == 0) && (this->currentTunic != PLAYER_TUNIC_GORON && CVarGetInteger("gSuperTunic", 0) == 0 && CVarGetInteger("gDisableTunicWarningText", 0) == 0)) ||
             (((var == 1) || (var == 3)) && (this->currentBoots == PLAYER_BOOTS_IRON) &&
              (this->currentTunic != PLAYER_TUNIC_ZORA && CVarGetInteger("gSuperTunic", 0) == 0 && CVarGetInteger("gDisableTunicWarningText", 0) == 0)))) {
            Message_StartTextbox(play, triggerEntry->textId, NULL);
            gSaveContext.textTriggerFlags |= triggerEntry->flag;
        }
    }

    return var + 1;
}

u8 sEyeMouthIndexes[][2] = {
    { 0, 0 }, { 1, 0 }, { 2, 0 }, { 0, 0 }, { 1, 0 }, { 2, 0 }, { 4, 0 }, { 5, 1 },
    { 7, 2 }, { 0, 2 }, { 3, 0 }, { 4, 0 }, { 2, 2 }, { 1, 1 }, { 0, 2 }, { 0, 0 },
};

/**
 * Link's eye and mouth textures are placed at the exact same place in adult and child Link's respective object files.
 * This allows the array to only contain the symbols for one file and have it apply to both. This is a problem for
 * shiftability, and changes will need to be made in the code to account for this in a modding scenario. The symbols
 * from adult Link's object are used here.
 */

#if defined(MODDING) || defined(_MSC_VER) || defined(__GNUC__)
//TODO: Formatting
void* sEyeTextures[2][8] = {
    { gLinkAdultEyesOpenTex, gLinkAdultEyesHalfTex, gLinkAdultEyesClosedfTex, gLinkAdultEyesRollLeftTex,
      gLinkAdultEyesRollRightTex, gLinkAdultEyesShockTex, gLinkAdultEyesUnk1Tex, gLinkAdultEyesUnk2Tex },
    { gLinkChildEyesOpenTex, gLinkChildEyesHalfTex, gLinkChildEyesClosedfTex, gLinkChildEyesRollLeftTex,
      gLinkChildEyesRollRightTex, gLinkChildEyesShockTex, gLinkChildEyesUnk1Tex, gLinkChildEyesUnk2Tex },
};

#else
void* sEyeTextures[] = {
    gLinkAdultEyesOpenTex,      gLinkAdultEyesHalfTex,  gLinkAdultEyesClosedfTex, gLinkAdultEyesRollLeftTex,
    gLinkAdultEyesRollRightTex, gLinkAdultEyesShockTex, gLinkAdultEyesUnk1Tex,    gLinkAdultEyesUnk2Tex,
};
#endif

#if defined(MODDING) || defined(_MSC_VER) || defined(__GNUC__)
void* sMouthTextures[2][4] = {
    {
        gLinkAdultMouth1Tex,
        gLinkAdultMouth2Tex,
        gLinkAdultMouth3Tex,
        gLinkAdultMouth4Tex,
    },
    {
        gLinkChildMouth1Tex,
        gLinkChildMouth2Tex,
        gLinkChildMouth3Tex,
        gLinkChildMouth4Tex,
    },
};
#else
void* sMouthTextures[] = {
    gLinkAdultMouth1Tex,
    gLinkAdultMouth2Tex,
    gLinkAdultMouth3Tex,
    gLinkAdultMouth4Tex,
};
#endif

Color_RGB8 sTunicColors[] = {
    { 30, 105, 27 },
    { 100, 20, 0 },
    { 0, 60, 100 },
};

Color_RGB8 sGauntletColors[] = {
    { 255, 255, 255 },
    { 254, 207, 15 },
};

Gfx* sBootDListGroups[][2] = {
    { gLinkAdultLeftIronBootDL, gLinkAdultRightIronBootDL },
    { gLinkAdultLeftHoverBootDL, gLinkAdultRightHoverBootDL },
};

void Player_DrawImpl(PlayState* play, void** skeleton, Vec3s* jointTable, s32 dListCount, s32 lod, s32 tunic,
                   s32 boots, s32 face, OverrideLimbDrawOpa overrideLimbDraw, PostLimbDrawOpa postLimbDraw,
                   void* data) {
    Color_RGB8* color;
    s32 eyeIndex = (jointTable[22].x & 0xF) - 1;
    s32 mouthIndex = (jointTable[22].x >> 4) - 1;

    OPEN_DISPS(play->state.gfxCtx);

    if (eyeIndex < 0) {
        eyeIndex = sEyeMouthIndexes[face][0];
    }

    if (eyeIndex > 7)
        eyeIndex = 7;

#if defined(MODDING) || defined(_MSC_VER) || defined(__GNUC__)
    gSPSegment(POLY_OPA_DISP++, 0x08, SEGMENTED_TO_VIRTUAL(sEyeTextures[gSaveContext.linkAge][eyeIndex]));
#else
    gSPSegment(POLY_OPA_DISP++, 0x08, SEGMENTED_TO_VIRTUAL(sEyeTextures[eyeIndex]));
#endif
    if (mouthIndex < 0) {
        mouthIndex = sEyeMouthIndexes[face][1];
    }

    if (mouthIndex > 3)
        mouthIndex = 3;

#if defined(MODDING) || defined(_MSC_VER) || defined(__GNUC__)
    gSPSegment(POLY_OPA_DISP++, 0x09, SEGMENTED_TO_VIRTUAL(sMouthTextures[gSaveContext.linkAge][mouthIndex]));
#else
    gSPSegment(POLY_OPA_DISP++, 0x09, SEGMENTED_TO_VIRTUAL(sMouthTextures[eyeIndex]));
#endif

    Color_RGB8 sTemp;
    color = &sTunicColors[tunic];
    if (tunic == PLAYER_TUNIC_KOKIRI && CVarGetInteger("gCosmetics.Link_KokiriTunic.Changed", 0)) {
        sTemp = CVarGetColor24("gCosmetics.Link_KokiriTunic.Value", sTunicColors[PLAYER_TUNIC_KOKIRI]);
        color = &sTemp;
    } else if (tunic == PLAYER_TUNIC_GORON && CVarGetInteger("gCosmetics.Link_GoronTunic.Changed", 0)) {
        sTemp = CVarGetColor24("gCosmetics.Link_GoronTunic.Value", sTunicColors[PLAYER_TUNIC_GORON]);
        color = &sTemp;
    } else if (tunic == PLAYER_TUNIC_ZORA && CVarGetInteger("gCosmetics.Link_ZoraTunic.Changed", 0)) {
        sTemp = CVarGetColor24("gCosmetics.Link_ZoraTunic.Value", sTunicColors[PLAYER_TUNIC_ZORA]);
        color = &sTemp;
    }

    gDPSetEnvColor(POLY_OPA_DISP++, color->r, color->g, color->b, 0);

    // If we have a custom link model, always use the most detailed LOD
    if (Player_IsCustomLinkModel()) {
        lod = 0;
    }

    sDListsLodOffset = lod * 2;

    SkelAnime_DrawFlexLod(play, skeleton, jointTable, dListCount, overrideLimbDraw, postLimbDraw, data, lod);

    if (((CVarGetInteger("gFPSGauntlets", 0) && LINK_IS_ADULT) || (overrideLimbDraw != Player_OverrideLimbDrawGameplayFirstPerson)) &&
        (overrideLimbDraw != Player_OverrideLimbDrawGameplayCrawling) &&
        (gSaveContext.gameMode != 3)) {
        if (LINK_IS_ADULT) {
            s32 strengthUpgrade = CUR_UPG_VALUE(UPG_STRENGTH);

            if (strengthUpgrade >= 2) { // silver or gold gauntlets
                gDPPipeSync(POLY_OPA_DISP++);

                color = &sGauntletColors[strengthUpgrade - 2];
                if (strengthUpgrade == PLAYER_STR_SILVER_G && CVarGetInteger("gCosmetics.Gloves_SilverGauntlets.Changed", 0)) {
                    sTemp = CVarGetColor24("gCosmetics.Gloves_SilverGauntlets.Value", sGauntletColors[PLAYER_STR_SILVER_G - 2]);
                    color = &sTemp;
                } else if (strengthUpgrade == PLAYER_STR_GOLD_G && CVarGetInteger("gCosmetics.Gloves_GoldenGauntlets.Changed", 0)) {
                    sTemp = CVarGetColor24("gCosmetics.Gloves_GoldenGauntlets.Value", sGauntletColors[PLAYER_STR_GOLD_G - 2]);
                    color = &sTemp;
                }
                gDPSetEnvColor(POLY_OPA_DISP++, color->r, color->g, color->b, 0);

                gSPDisplayList(POLY_OPA_DISP++, gLinkAdultLeftGauntletPlate1DL);
                gSPDisplayList(POLY_OPA_DISP++, gLinkAdultRightGauntletPlate1DL);
                gSPDisplayList(POLY_OPA_DISP++,
                               (sLeftHandType == 0) ? gLinkAdultLeftGauntletPlate2DL : gLinkAdultLeftGauntletPlate3DL);
                gSPDisplayList(POLY_OPA_DISP++,
                               (sRightHandType == 8) ? gLinkAdultRightGauntletPlate2DL : gLinkAdultRightGauntletPlate3DL);
            }

            if (boots != 0) {
                Gfx** bootDLists = sBootDListGroups[boots - 1];

                gSPDisplayList(POLY_OPA_DISP++, bootDLists[0]);
                gSPDisplayList(POLY_OPA_DISP++, bootDLists[1]);
            }
        } else {
            if (Player_GetStrength() > PLAYER_STR_NONE) {
                gSPDisplayList(POLY_OPA_DISP++, gLinkChildGoronBraceletDL);
            }
        }
    }

    CLOSE_DISPS(play->state.gfxCtx);
}

Vec3f sZeroVec = { 0.0f, 0.0f, 0.0f };

Vec3f D_80126038[] = {
    { 1304.0f, 0.0f, 0.0f },
    { 695.0f, 0.0f, 0.0f },
};

f32 D_80126050[] = { 1265.0f, 826.0f };
f32 D_80126058[] = { SQ(13.04f), SQ(6.95f) };
f32 D_80126060[] = { 10.019104f, -19.925102f };
f32 D_80126068[] = { 5.0f, 3.0f };

Vec3f D_80126070 = { 0.0f, -300.0f, 0.0f };

void func_8008F87C(PlayState* play, Player* this, SkelAnime* skelAnime, Vec3f* pos, Vec3s* rot,
                   s32 thighLimbIndex, s32 shinLimbIndex, s32 footLimbIndex) {
    Vec3f spA4;
    Vec3f sp98;
    Vec3f footprintPos;
    CollisionPoly* sp88;
    s32 sp84;
    f32 sp80;
    f32 sp7C;
    f32 sp78;
    f32 sp74;
    f32 sp70;
    f32 sp6C;
    f32 sp68;
    f32 sp64;
    f32 sp60;
    f32 sp5C;
    f32 sp58;
    f32 sp54;
    f32 sp50;
    s16 temp1;
    s16 temp2;
    s32 temp3;

    if ((this->actor.scale.y >= 0.0f) && !(this->stateFlags1 & 0x80) &&
        (Player_ActionToMagicSpell(this, this->itemAction) < 0)) {
        s32 pad;

        sp7C = D_80126058[gSaveContext.linkAge];
        sp78 = D_80126060[gSaveContext.linkAge];
        sp74 = D_80126068[gSaveContext.linkAge] - this->unk_6C4;

        Matrix_Push();
        Matrix_TranslateRotateZYX(pos, rot);
        Matrix_MultVec3f(&sZeroVec, &spA4);
        Matrix_TranslateRotateZYX(&D_80126038[gSaveContext.linkAge], &skelAnime->jointTable[shinLimbIndex]);
        Matrix_Translate(D_80126050[gSaveContext.linkAge], 0.0f, 0.0f, MTXMODE_APPLY);
        Matrix_MultVec3f(&sZeroVec, &sp98);
        Matrix_MultVec3f(&D_80126070, &footprintPos);
        Matrix_Pop();

        footprintPos.y += 15.0f;

        sp80 = BgCheck_EntityRaycastFloor4(&play->colCtx, &sp88, &sp84, &this->actor, &footprintPos) + sp74;

        if (sp98.y < sp80) {
            sp70 = sp98.x - spA4.x;
            sp6C = sp98.y - spA4.y;
            sp68 = sp98.z - spA4.z;

            sp64 = sqrtf(SQ(sp70) + SQ(sp6C) + SQ(sp68));
            sp60 = (SQ(sp64) + sp78) / (2.0f * sp64);

            sp58 = sp7C - SQ(sp60);
            sp58 = (sp7C < SQ(sp60)) ? 0.0f : sqrtf(sp58);

            sp54 = Math_FAtan2F(sp58, sp60);

            sp6C = sp80 - spA4.y;

            sp64 = sqrtf(SQ(sp70) + SQ(sp6C) + SQ(sp68));
            sp60 = (SQ(sp64) + sp78) / (2.0f * sp64);
            sp5C = sp64 - sp60;

            sp58 = sp7C - SQ(sp60);
            sp58 = (sp7C < SQ(sp60)) ? 0.0f : sqrtf(sp58);

            sp50 = Math_FAtan2F(sp58, sp60);

            temp1 = (M_PI - (Math_FAtan2F(sp5C, sp58) + ((M_PI / 2) - sp50))) * (0x8000 / M_PI);
            temp1 = temp1 - skelAnime->jointTable[shinLimbIndex].z;

            if ((s16)(ABS(skelAnime->jointTable[shinLimbIndex].x) + ABS(skelAnime->jointTable[shinLimbIndex].y)) < 0) {
                temp1 += 0x8000;
            }

            temp2 = (sp50 - sp54) * (0x8000 / M_PI);
            rot->z -= temp2;

            skelAnime->jointTable[thighLimbIndex].z = skelAnime->jointTable[thighLimbIndex].z - temp2;
            skelAnime->jointTable[shinLimbIndex].z = skelAnime->jointTable[shinLimbIndex].z + temp1;
            skelAnime->jointTable[footLimbIndex].z = skelAnime->jointTable[footLimbIndex].z + temp2 - temp1;

            temp3 = func_80041D4C(&play->colCtx, sp88, sp84);

            if ((temp3 >= 2) && (temp3 < 4) && !SurfaceType_IsWallDamage(&play->colCtx, sp88, sp84)) {
                footprintPos.y = sp80;
                EffectSsGFire_Spawn(play, &footprintPos);
            }
        }
    }
}

s32 Player_OverrideLimbDrawGameplayCommon(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, void* thisx) {
    Player* this = (Player*)thisx;

    if (limbIndex == PLAYER_LIMB_ROOT) {
        sLeftHandType = this->leftHandType;
        sRightHandType = this->rightHandType;
        D_80160000 = &this->meleeWeaponInfo[2].base;

        if (!LINK_IS_ADULT) {
            if (!(this->skelAnime.moveFlags & 4) || (this->skelAnime.moveFlags & 1)) {
                pos->x *= 0.64f;
                pos->z *= 0.64f;
            }

            if (!(this->skelAnime.moveFlags & 4) || (this->skelAnime.moveFlags & 2)) {
                pos->y *= 0.64f;
            }
        }

        pos->y -= this->unk_6C4;

        if (this->unk_6C2 != 0) {
            Matrix_Translate(pos->x, ((Math_CosS(this->unk_6C2) - 1.0f) * 200.0f) + pos->y, pos->z, MTXMODE_APPLY);
            Matrix_RotateX(this->unk_6C2 * (M_PI / 0x8000), MTXMODE_APPLY);
            Matrix_RotateZYX(rot->x, rot->y, rot->z, MTXMODE_APPLY);
            pos->x = pos->y = pos->z = 0.0f;
            rot->x = rot->y = rot->z = 0;
        }
    } else {
        if (*dList != NULL) {
            D_80160000++;
        }

        if (limbIndex == PLAYER_LIMB_HEAD) {
            if (CVarGetInteger("gCosmetics.Link_HeadScale.Changed", 0)) {
                f32 scale = CVarGetFloat("gCosmetics.Link_HeadScale.Value", 1.0f);
                Matrix_Scale(scale, scale, scale, MTXMODE_APPLY);
                if (scale > 1.2f) {
                    Matrix_Translate(-((LINK_IS_ADULT ? 320.0f : 200.0f) * scale), 0.0f, 0.0f, MTXMODE_APPLY);
                } else if (scale < 1.0f) {
                    Matrix_Translate((LINK_IS_ADULT ? 3600.0f : 2900.0f) * ABS(scale - 1.0f), 0.0f, 0.0f, MTXMODE_APPLY);
                }
            }
            rot->x += this->unk_6BA;
            rot->y -= this->unk_6B8;
            rot->z += this->unk_6B6;
        } else if (limbIndex == PLAYER_LIMB_L_HAND) {
            if (CVarGetInteger("gCosmetics.Link_SwordScale.Changed", 0)) {
                f32 scale = CVarGetFloat("gCosmetics.Link_SwordScale.Value", 1.0f);
                Matrix_Scale(scale, scale, scale, MTXMODE_APPLY);
                Matrix_Translate(-((LINK_IS_ADULT ? 320.0f : 200.0f) * scale), 0.0f, 0.0f, MTXMODE_APPLY);
            }
        } else if (limbIndex == PLAYER_LIMB_UPPER) {
            if (this->unk_6B0 != 0) {
                Matrix_RotateZ(0x44C * (M_PI / 0x8000), MTXMODE_APPLY);
                Matrix_RotateY(this->unk_6B0 * (M_PI / 0x8000), MTXMODE_APPLY);
            }
            if (this->unk_6BE != 0) {
                Matrix_RotateY(this->unk_6BE * (M_PI / 0x8000), MTXMODE_APPLY);
            }
            if (this->unk_6BC != 0) {
                Matrix_RotateX(this->unk_6BC * (M_PI / 0x8000), MTXMODE_APPLY);
            }
            if (this->unk_6C0 != 0) {
                Matrix_RotateZ(this->unk_6C0 * (M_PI / 0x8000), MTXMODE_APPLY);
            }
        } else if (limbIndex == PLAYER_LIMB_L_THIGH) {
            func_8008F87C(play, this, &this->skelAnime, pos, rot, PLAYER_LIMB_L_THIGH, PLAYER_LIMB_L_SHIN,
                          PLAYER_LIMB_L_FOOT);
        } else if (limbIndex == PLAYER_LIMB_R_THIGH) {
            func_8008F87C(play, this, &this->skelAnime, pos, rot, PLAYER_LIMB_R_THIGH, PLAYER_LIMB_R_SHIN,
                          PLAYER_LIMB_R_FOOT);
            return false;
        } else {
            return false;
        }
    }

    return false;
}

s32 Player_OverrideLimbDrawGameplayDefault(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, void* thisx) {
    Player* this = (Player*)thisx;

    if (!Player_OverrideLimbDrawGameplayCommon(play, limbIndex, dList, pos, rot, thisx)) 
    {
        if (limbIndex == PLAYER_LIMB_L_HAND) {
            Gfx** dLists = this->leftHandDLists;

            if ((sLeftHandType == 4) && (gSaveContext.swordHealth <= 0.0f)) {
                dLists += 4;
            } else if ((sLeftHandType == 6) && (this->stateFlags1 & 0x2000000)) {
                dLists = &gPlayerLeftHandOpenDLs[gSaveContext.linkAge];
                sLeftHandType = 0;
            } else if ((this->leftHandType == 0) && (this->actor.speedXZ > 2.0f) && !(this->stateFlags1 & 0x8000000)) {
                dLists = &gPlayerLeftHandClosedDLs[gSaveContext.linkAge];
                sLeftHandType = 1;
            }

            *dList = ResourceMgr_LoadGfxByName(dLists[sDListsLodOffset]);
        } else if (limbIndex == PLAYER_LIMB_R_HAND) {
            Gfx** dLists = this->rightHandDLists;

            if (sRightHandType == 10) {
                dLists += this->currentShield * 4;
            } else if ((this->rightHandType == 8) && (this->actor.speedXZ > 2.0f) && !(this->stateFlags1 & 0x8000000)) {
                dLists = &sPlayerRightHandClosedDLs[gSaveContext.linkAge];
                sRightHandType = 9;
            }

            *dList = ResourceMgr_LoadGfxByName(dLists[sDListsLodOffset]);
        } else if (limbIndex == PLAYER_LIMB_SHEATH) {
            Gfx** dLists = this->sheathDLists;

            if ((this->sheathType == 18) || (this->sheathType == 19)) {
                dLists += this->currentShield * 4;
                if (!LINK_IS_ADULT && (this->currentShield < PLAYER_SHIELD_HYLIAN) &&
                    (gSaveContext.equips.buttonItems[0] != ITEM_SWORD_KOKIRI)) {
                    dLists += 16;
                }
            } else if (!LINK_IS_ADULT && ((this->sheathType == 16) || (this->sheathType == 17)) &&
                       (gSaveContext.equips.buttonItems[0] != ITEM_SWORD_KOKIRI)) {
                dLists = &sSheathWithSwordDLs[16];
            }

            if (dLists[sDListsLodOffset] != NULL) {
                *dList = ResourceMgr_LoadGfxByName(dLists[sDListsLodOffset]);
            } else {
                *dList = NULL;
            }


        } else if (limbIndex == PLAYER_LIMB_WAIST) {
            
            if (!Player_IsCustomLinkModel()) {
                *dList = ResourceMgr_LoadGfxByName(
                    this->waistDLists[sDListsLodOffset]); // NOTE: This needs to be disabled when using custom
                                                          // characters - they're not going to have LODs anyways...
            }
        }
    }

    if (GameInteractor_InvisibleLinkActive()) {
        this->actor.shape.shadowDraw = NULL;
        *dList = NULL;
    }

    return false;
}

s32 Player_OverrideLimbDrawGameplayFirstPerson(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, void* thisx) {
    Player* this = (Player*)thisx;

    if (!Player_OverrideLimbDrawGameplayCommon(play, limbIndex, dList, pos, rot, thisx)) {
        if (this->unk_6AD != 2) {
            *dList = NULL;
        } else if (limbIndex == PLAYER_LIMB_L_FOREARM) {
            *dList = sFirstPersonLeftForearmDLs[gSaveContext.linkAge];
        } else if (limbIndex == PLAYER_LIMB_L_HAND) {
            s32 handOutDlIndex = gSaveContext.linkAge;
            if (CVarGetInteger("gBowSlingShotAmmoFix", 0) && LINK_IS_ADULT && Player_HoldsSlingshot(this)) {
                handOutDlIndex = 1;
            }
            *dList = sFirstPersonLeftHandDLs[handOutDlIndex];
        } else if (limbIndex == PLAYER_LIMB_R_SHOULDER) {
            *dList = sFirstPersonRightShoulderDLs[gSaveContext.linkAge];
        } else if (limbIndex == PLAYER_LIMB_R_FOREARM) {
            *dList = sFirstPersonForearmDLs[gSaveContext.linkAge];
        } else if (limbIndex == PLAYER_LIMB_R_HAND) {
            s32 firstPersonWeaponIndex = gSaveContext.linkAge;
            if (CVarGetInteger("gBowSlingShotAmmoFix", 0)) {
                if (Player_HoldsBow(this)) {
                    firstPersonWeaponIndex = 0;
                } else if (Player_HoldsSlingshot(this)) {
                    firstPersonWeaponIndex = 1;
                }
            }
            *dList = Player_HoldsHookshot(this) ? gLinkAdultRightHandHoldingHookshotFarDL
                                                : sFirstPersonRightHandHoldingWeaponDLs[firstPersonWeaponIndex];
        } else {
            *dList = NULL;
        }
    }

    return false;
}

s32 Player_OverrideLimbDrawGameplayCrawling(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, void* thisx) {
    if (!Player_OverrideLimbDrawGameplayCommon(play, limbIndex, dList, pos, rot, thisx)) {
        *dList = NULL;
    }

    return false;
}

u8 func_80090480(PlayState* play, ColliderQuad* collider, WeaponInfo* weaponInfo, Vec3f* newTip,
                 Vec3f* newBase) {
    if (weaponInfo->active == 0) {
        if (collider != NULL) {
            Collider_ResetQuadAT(play, &collider->base);
        }
        Math_Vec3f_Copy(&weaponInfo->tip, newTip);
        Math_Vec3f_Copy(&weaponInfo->base, newBase);
        weaponInfo->active = 1;
        return 1;
    } else if ((weaponInfo->tip.x == newTip->x) && (weaponInfo->tip.y == newTip->y) &&
               (weaponInfo->tip.z == newTip->z) && (weaponInfo->base.x == newBase->x) &&
               (weaponInfo->base.y == newBase->y) && (weaponInfo->base.z == newBase->z)) {
        if (collider != NULL) {
            Collider_ResetQuadAT(play, &collider->base);
        }
        return 0;
    } else {
        if (collider != NULL) {
            Collider_SetQuadVertices(collider, newBase, newTip, &weaponInfo->base, &weaponInfo->tip);
            CollisionCheck_SetAT(play, &play->colChkCtx, &collider->base);
        }
        Math_Vec3f_Copy(&weaponInfo->base, newBase);
        Math_Vec3f_Copy(&weaponInfo->tip, newTip);
        weaponInfo->active = 1;
        return 1;
    }
}

void Player_UpdateShieldCollider(PlayState* play, Player* this, ColliderQuad* collider, Vec3f* quadSrc) {
    static u8 shieldColTypes[PLAYER_SHIELD_MAX] = {
        COLTYPE_METAL,
        COLTYPE_WOOD,
        COLTYPE_METAL,
        COLTYPE_METAL,
    };

    if (this->stateFlags1 & 0x400000) {
        Vec3f quadDest[4];

        this->shieldQuad.base.colType = shieldColTypes[this->currentShield];

        Matrix_MultVec3f(&quadSrc[0], &quadDest[0]);
        Matrix_MultVec3f(&quadSrc[1], &quadDest[1]);
        Matrix_MultVec3f(&quadSrc[2], &quadDest[2]);
        Matrix_MultVec3f(&quadSrc[3], &quadDest[3]);
        Collider_SetQuadVertices(collider, &quadDest[0], &quadDest[1], &quadDest[2], &quadDest[3]);

        CollisionCheck_SetAC(play, &play->colChkCtx, &collider->base);
        CollisionCheck_SetAT(play, &play->colChkCtx, &collider->base);
    }
}

Vec3f D_80126080 = { 5000.0f, 400.0f, 0.0f };
Vec3f D_8012608C = { 5000.0f, -400.0f, 1000.0f };
Vec3f D_80126098 = { 5000.0f, 1400.0f, -1000.0f };

Vec3f D_801260A4[3] = {
    { 0.0f, 400.0f, 0.0f },
    { 0.0f, 1400.0f, -1000.0f },
    { 0.0f, -400.0f, 1000.0f },
};

void func_800906D4(PlayState* play, Player* this, Vec3f* newTipPos) {
    Vec3f newBasePos[3];

    Matrix_MultVec3f(&D_801260A4[0], &newBasePos[0]);
    Matrix_MultVec3f(&D_801260A4[1], &newBasePos[1]);
    Matrix_MultVec3f(&D_801260A4[2], &newBasePos[2]);

    if (func_80090480(play, NULL, &this->meleeWeaponInfo[0], &newTipPos[0], &newBasePos[0]) &&
        !(this->stateFlags1 & 0x400000)) {
        EffectBlure_AddVertex(Effect_GetByIndex(this->meleeWeaponEffectIndex), &this->meleeWeaponInfo[0].tip,
                              &this->meleeWeaponInfo[0].base);
    }

    if ((this->meleeWeaponState > 0) && ((this->meleeWeaponAnimation < 0x18) || (this->stateFlags2 & 0x20000))) {
        func_80090480(play, &this->meleeWeaponQuads[0], &this->meleeWeaponInfo[1], &newTipPos[1], &newBasePos[1]);
        func_80090480(play, &this->meleeWeaponQuads[1], &this->meleeWeaponInfo[2], &newTipPos[2], &newBasePos[2]);
    }
}

void Player_DrawGetItemIceTrap(PlayState* play, Player* this, Vec3f* refPos, s32 drawIdPlusOne, f32 height) {
    OPEN_DISPS(play->state.gfxCtx);

    if (CVarGetInteger("gLetItSnow", 0)) {
        Gfx_SetupDL_25Opa(play->state.gfxCtx);

        Matrix_Scale(0.2f, 0.2f, 0.2f, MTXMODE_APPLY);
        gSPMatrix(POLY_OPA_DISP++, MATRIX_NEWMTX(play->state.gfxCtx), G_MTX_MODELVIEW | G_MTX_LOAD);

        gDPSetGrayscaleColor(POLY_OPA_DISP++, 75, 75, 75, 255);
        gSPGrayscale(POLY_OPA_DISP++, true);

        gSPDisplayList(POLY_OPA_DISP++, (Gfx*)gSilverRockDL);

        gSPGrayscale(POLY_OPA_DISP++, false);
    } else {
        if (iceTrapScale < 0.01) {
            iceTrapScale += 0.001f;
        } else if (iceTrapScale < 0.8f) {
            iceTrapScale += 0.2f;
        }
        gSPSegment(POLY_XLU_DISP++, 0x08,
                   Gfx_TwoTexScroll(play->state.gfxCtx, 0, 0, (0 - play->gameplayFrames) % 128, 32, 32, 1, 0,
                                    (play->gameplayFrames * -2) % 128, 32, 32));

        Matrix_Translate(0.0f, -40.0f, 0.0f, MTXMODE_APPLY);
        Matrix_Scale(iceTrapScale, iceTrapScale, iceTrapScale, MTXMODE_APPLY);
        gSPMatrix(POLY_XLU_DISP++, MATRIX_NEWMTX(play->state.gfxCtx), G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
        gDPSetEnvColor(POLY_XLU_DISP++, 0, 50, 100, 255);
        gSPDisplayList(POLY_XLU_DISP++, gEffIceFragment3DL);

        // Reset matrix for the fake item model because we're animating the size of the ice block around it before this.
        Matrix_Translate(refPos->x + (3.3f * Math_SinS(this->actor.shape.rot.y)), refPos->y + height,
                         refPos->z + ((3.3f + (IREG(90) / 10.0f)) * Math_CosS(this->actor.shape.rot.y)), MTXMODE_NEW);
        Matrix_RotateZYX(0, play->gameplayFrames * 1000, 0, MTXMODE_APPLY);
        Matrix_Scale(0.2f, 0.2f, 0.2f, MTXMODE_APPLY);
        // Draw fake item model.
        GetItem_Draw(play, drawIdPlusOne - 1);
    }

    CLOSE_DISPS(play->state.gfxCtx);
}

void Player_DrawGetItemImpl(PlayState* play, Player* this, Vec3f* refPos, s32 drawIdPlusOne) {
    f32 height = (this->exchangeItemId != EXCH_ITEM_NONE) ? 6.0f : 14.0f;

    OPEN_DISPS(play->state.gfxCtx);

    gSegments[6] = VIRTUAL_TO_PHYSICAL(this->giObjectSegment);

    gSPSegment(POLY_OPA_DISP++, 0x06, this->giObjectSegment);
    gSPSegment(POLY_XLU_DISP++, 0x06, this->giObjectSegment);

    Matrix_Translate(refPos->x + (3.3f * Math_SinS(this->actor.shape.rot.y)), refPos->y + height,
                     refPos->z + ((3.3f + (IREG(90) / 10.0f)) * Math_CosS(this->actor.shape.rot.y)), MTXMODE_NEW);
    Matrix_RotateZYX(0, play->gameplayFrames * 1000, 0, MTXMODE_APPLY);
    Matrix_Scale(0.2f, 0.2f, 0.2f, MTXMODE_APPLY);

    if (this->getItemEntry.modIndex == MOD_RANDOMIZER && this->getItemEntry.getItemId == RG_ICE_TRAP) {
        Player_DrawGetItemIceTrap(play, this, refPos, drawIdPlusOne, height);
    } else if (this->getItemEntry.modIndex == MOD_RANDOMIZER && this->getItemEntry.getItemId == RG_TRIFORCE_PIECE) {
        Randomizer_DrawTriforcePieceGI(play, this->getItemEntry);
    } else if (this->getItemEntry.drawFunc != NULL) {
        this->getItemEntry.drawFunc(play, &this->getItemEntry);
    } else {
        GetItem_Draw(play, drawIdPlusOne - 1);
    }

    CLOSE_DISPS(play->state.gfxCtx);
}

void Player_DrawGetItem(PlayState* play, Player* this) {
    //if (!this->giObjectLoading || !osRecvMesg(&this->giObjectLoadQueue, NULL, OS_MESG_NOBLOCK)) // OTRTODO: Do something about osRecvMesg here...
    {
        this->giObjectLoading = false;
        Player_DrawGetItemImpl(play, this, &sGetItemRefPos, ABS(this->unk_862));
    }
}

void func_80090A28(Player* this, Vec3f* vecs) {
    D_8012608C.x = D_80126080.x;

    if (this->unk_845 >= 3) {
        this->unk_845 += 1;
        D_8012608C.x *= 1.0f + ((9 - this->unk_845) * 0.1f);
    }

    D_8012608C.x += 1200.0f;
    D_80126098.x = D_8012608C.x;

    Matrix_MultVec3f(&D_80126080, &vecs[0]);
    Matrix_MultVec3f(&D_8012608C, &vecs[1]);
    Matrix_MultVec3f(&D_80126098, &vecs[2]);
}

void Player_DrawHookshotReticle(PlayState* play, Player* this, f32 hookshotRange) {
    static Vec3f D_801260C8 = { -500.0f, -100.0f, 0.0f };
    CollisionPoly* colPoly;
    s32 bgId;
    Vec3f hookshotStart;
    Vec3f hookshotEnd;
    Vec3f firstHit;
    Vec3f sp68;
    f32 sp64;

    D_801260C8.z = 0.0f;
    Matrix_MultVec3f(&D_801260C8, &hookshotStart);
    D_801260C8.z = hookshotRange;
    Matrix_MultVec3f(&D_801260C8, &hookshotEnd);

    if (BgCheck_AnyLineTest3(&play->colCtx, &hookshotStart, &hookshotEnd, &firstHit, &colPoly, 1, 1, 1, 1, &bgId)) {
        OPEN_DISPS(play->state.gfxCtx);

        WORLD_OVERLAY_DISP = Gfx_SetupDL(WORLD_OVERLAY_DISP, 0x07);

        SkinMatrix_Vec3fMtxFMultXYZW(&play->viewProjectionMtxF, &firstHit, &sp68, &sp64);

        const f32 sp60 = (sp64 < 200.0f) ? 0.08f : (sp64 / 200.0f) * 0.08f;

        Matrix_Translate(firstHit.x, firstHit.y, firstHit.z, MTXMODE_NEW);
        Matrix_Scale(sp60, sp60, sp60, MTXMODE_APPLY);

        gSPMatrix(WORLD_OVERLAY_DISP++, MATRIX_NEWMTX(play->state.gfxCtx), G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
        gSPMatrix(WORLD_OVERLAY_DISP++, SEG_ADDR(1, 0), G_MTX_NOPUSH | G_MTX_MUL | G_MTX_MODELVIEW);
        gSPTexture(WORLD_OVERLAY_DISP++, 0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON);
        gDPLoadTextureBlock(WORLD_OVERLAY_DISP++, gLinkAdultHookshotReticleTex, G_IM_FMT_I, G_IM_SIZ_8b, 64, 64, 0,
                            G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMIRROR | G_TX_CLAMP, 6, 6, G_TX_NOLOD, G_TX_NOLOD);
        if (SurfaceType_IsHookshotSurface(&play->colCtx, colPoly, bgId) && CVarGetInteger("gHookshotableReticle", false)) {
            const Color_RGBA8 defaultColor = { .r = 0, .g = 255, .b = 0, .a = 255 };
            const Color_RGBA8 color = CVarGetColor("gCosmetics.HookshotReticle_Target.Value", defaultColor);
            gDPSetPrimColor(WORLD_OVERLAY_DISP++, 0, 0, color.r, color.g, color.b, color.a);
        } else {
            const Color_RGBA8 defaultColor = { .r = 255, .g = 0, .b = 0, .a = 255 };
            const Color_RGBA8 color = CVarGetColor("gCosmetics.HookshotReticle_NonTarget.Value", defaultColor);
            gDPSetPrimColor(WORLD_OVERLAY_DISP++, 0, 0, color.r, color.g, color.b, color.a);
        }
        gSPVertex(WORLD_OVERLAY_DISP++, (uintptr_t)gLinkAdultHookshotReticleVtx, 3, 0);
        gSP1Triangle(WORLD_OVERLAY_DISP++, 0, 1, 2, 0);

        CLOSE_DISPS(play->state.gfxCtx);
    }
}

Vec3f D_801260D4 = { 1100.0f, -700.0f, 0.0f };

f32 sMeleeWeaponLengths[] = {
    0.0f, 4000.0f, 3000.0f, 5500.0f, 0.0f, 2500.0f,
};

f32 sSwordTypes[] = {
    0, 5, 4, 6, 0, 8,
};

Gfx* sBottleDLists[] = { gLinkAdultBottleDL, gLinkChildBottleDL };

Color_RGB8 sBottleColors[] = {
    { 255, 255, 255 }, { 80, 80, 255 },   { 255, 100, 255 }, { 0, 0, 255 }, { 255, 0, 255 },
    { 255, 0, 255 },   { 200, 200, 100 }, { 255, 0, 0 },     { 0, 0, 255 }, { 0, 255, 0 },
    { 255, 255, 255 }, { 255, 255, 255 }, { 80, 80, 255 },
};

Vec3f sLeftHandArrowVec3 = { 398.0f, 1419.0f, 244.0f };

BowStringData sBowStringData[] = {
    { gLinkAdultBowStringDL, { 0.0f, -360.4f, 0.0f } },       // bow
    { gLinkChildSlinghotStringDL, { 606.0f, 236.0f, 0.0f } }, // slingshot
};

Vec3f sRightHandLimbModelShieldQuadVertices[] = {
    { -4500.0f, -3000.0f, -600.0f },
    { 1500.0f, -3000.0f, -600.0f },
    { -4500.0f, 3000.0f, -600.0f },
    { 1500.0f, 3000.0f, -600.0f },
};

Vec3f D_80126184 = { 100.0f, 1500.0f, 0.0f };
Vec3f D_80126190 = { 100.0f, 1640.0f, 0.0f };

Vec3f sSheathLimbModelShieldQuadVertices[] = {
    { -3000.0f, -3000.0f, -900.0f },
    { 3000.0f, -3000.0f, -900.0f },
    { -3000.0f, 3000.0f, -900.0f },
    { 3000.0f, 3000.0f, -900.0f },
};

Vec3f sSheathLimbModelShieldOnBackPos = { 630.0f, 100.0f, -30.0f };
Vec3s sSheathLimbModelShieldOnBackZyxRot = { 0, 0, 0x7FFF };

Vec3f sLeftRightFootLimbModelFootPos[] = {
    { 200.0f, 300.0f, 0.0f },
    { 200.0f, 200.0f, 0.0f },
};

// OTRTODO: Figure out why this value works/what this value should be
// This was originally obtained by working down from FLT_MAX until the math
// started working out properly
#define RETICLE_MAX 3.402823466e+12f

void Player_PostLimbDrawGameplay(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot, void* thisx) {
    Player* this = (Player*)thisx;

    if (*dList != NULL) {
        Matrix_MultVec3f(&sZeroVec, D_80160000);
    }

    if (limbIndex == PLAYER_LIMB_L_HAND) {
        MtxF sp14C;
        Actor* hookedActor;

        Math_Vec3f_Copy(&this->leftHandPos, D_80160000);

        if (this->itemAction == PLAYER_IA_DEKU_STICK) {
            Vec3f sp124[3];

            OPEN_DISPS(play->state.gfxCtx);

            if (this->actor.scale.y >= 0.0f) {
                D_80126080.x = this->unk_85C * 5000.0f;
                func_80090A28(this, sp124);
                if (this->meleeWeaponState != 0) {
                    EffectBlure_ChangeType(Effect_GetByIndex(this->meleeWeaponEffectIndex), 7); // stick sword type
                    func_800906D4(play, this, sp124);
                } else {
                    Math_Vec3f_Copy(&this->meleeWeaponInfo[0].tip, &sp124[0]);
                }
            }

            Matrix_Translate(-428.26f, 267.2f, -33.82f, MTXMODE_APPLY);
            Matrix_RotateZYX(-0x8000, 0, 0x4000, MTXMODE_APPLY);
            Matrix_Scale(1.0f, this->unk_85C, 1.0f, MTXMODE_APPLY);

            gSPMatrix(POLY_OPA_DISP++, MATRIX_NEWMTX(play->state.gfxCtx),
                      G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
            gSPDisplayList(POLY_OPA_DISP++, gLinkChildLinkDekuStickDL);

            CLOSE_DISPS(play->state.gfxCtx);
        } else if ((this->actor.scale.y >= 0.0f) && (this->meleeWeaponState != 0)) {
            Vec3f spE4[3];

            if (Player_HoldsBrokenKnife(this)) {
                D_80126080.x = 1500.0f;
            } else {
                D_80126080.x = sMeleeWeaponLengths[Player_GetMeleeWeaponHeld(this)];
                EffectBlure_ChangeType(Effect_GetByIndex(this->meleeWeaponEffectIndex), sSwordTypes[Player_GetMeleeWeaponHeld(this)]);
            }

            func_80090A28(this, spE4);
            func_800906D4(play, this, spE4);
        } else if ((*dList != NULL) && (this->leftHandType == 7)) {
            Color_RGB8* bottleColor = &sBottleColors[Player_ActionToBottle(this, this->itemAction)];

            OPEN_DISPS(play->state.gfxCtx);

            gSPMatrix(POLY_XLU_DISP++, MATRIX_NEWMTX(play->state.gfxCtx),
                      G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
            gDPSetEnvColor(POLY_XLU_DISP++, bottleColor->r, bottleColor->g, bottleColor->b, 0);
            gSPDisplayList(POLY_XLU_DISP++, sBottleDLists[(gSaveContext.linkAge)]);

            CLOSE_DISPS(play->state.gfxCtx);
        }

        if (this->actor.scale.y >= 0.0f) {
            if (!Player_HoldsHookshot(this) && ((hookedActor = this->heldActor) != NULL)) {
                if (this->stateFlags1 & 0x200) {
                    Matrix_MultVec3f(&sLeftHandArrowVec3, &hookedActor->world.pos);
                    Matrix_RotateZYX(0x69E8, -0x5708, 0x458E, MTXMODE_APPLY);
                    Matrix_Get(&sp14C);
                    Matrix_MtxFToYXZRotS(&sp14C, &hookedActor->world.rot, 0);
                    hookedActor->shape.rot = hookedActor->world.rot;
                } else if (this->stateFlags1 & 0x800) {
                    Vec3s spB8;

                    Matrix_Get(&sp14C);
                    Matrix_MtxFToYXZRotS(&sp14C, &spB8, 0);

                    if (hookedActor->flags & ACTOR_FLAG_PILLAR_PICKUP) {
                        hookedActor->world.rot.x = hookedActor->shape.rot.x = spB8.x - this->unk_3BC.x;
                    } else {
                        hookedActor->world.rot.y = hookedActor->shape.rot.y = this->actor.shape.rot.y + this->unk_3BC.y;
                    }
                }
            } else {
                Matrix_Get(&this->mf_9E0);
                Matrix_MtxFToYXZRotS(&this->mf_9E0, &this->unk_3BC, 0);
            }
        }
    } else if (limbIndex == PLAYER_LIMB_R_HAND) {
        Actor* heldActor = this->heldActor;

        if (this->rightHandType == 0xFF) {
            Matrix_Get(&this->shieldMf);
        } else if ((this->rightHandType == 11) || (this->rightHandType == 12)) {
            s32 stringModelToUse = gSaveContext.linkAge;
            if(CVarGetInteger("gBowSlingShotAmmoFix", 0)){
                stringModelToUse = Player_HoldsSlingshot(this);
            }
            BowStringData* stringData = &sBowStringData[stringModelToUse];

            OPEN_DISPS(play->state.gfxCtx);

            Matrix_Push();
            Matrix_Translate(stringData->pos.x, stringData->pos.y, stringData->pos.z, MTXMODE_APPLY);

            if ((this->stateFlags1 & 0x200) && (this->unk_860 >= 0) && (this->unk_834 <= 10)) {
                Vec3f sp90;
                f32 distXYZ;

                Matrix_MultVec3f(&sZeroVec, &sp90);
                distXYZ = Math_Vec3f_DistXYZ(D_80160000, &sp90);

                this->unk_858 = distXYZ - 3.0f;
                if (distXYZ < 3.0f) {
                    this->unk_858 = 0.0f;
                } else {
                    this->unk_858 *= 1.6f;
                    if (this->unk_858 > 1.0f) {
                        this->unk_858 = 1.0f;
                    }
                }

                this->unk_85C = -0.5f;
            }

            Matrix_Scale(1.0f, this->unk_858, 1.0f, MTXMODE_APPLY);

            if (!LINK_IS_ADULT) {
                Matrix_RotateZ(this->unk_858 * -0.2f, MTXMODE_APPLY);
            }

            gSPMatrix(POLY_XLU_DISP++, MATRIX_NEWMTX(play->state.gfxCtx),
                      G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
            gSPDisplayList(POLY_XLU_DISP++, stringData->dList);

            Matrix_Pop();

            CLOSE_DISPS(play->state.gfxCtx);
        } else if ((this->actor.scale.y >= 0.0f) && (this->rightHandType == 10)) {
            Matrix_Get(&this->shieldMf);
            Player_UpdateShieldCollider(play, this, &this->shieldQuad, sRightHandLimbModelShieldQuadVertices);
        }

        if (this->actor.scale.y >= 0.0f) {
            if ((this->heldItemAction == PLAYER_IA_HOOKSHOT) ||
                (this->heldItemAction == PLAYER_IA_LONGSHOT)) {
                Matrix_MultVec3f(&D_80126184, &this->unk_3C8);

                if (heldActor != NULL) {
                    MtxF sp44;
                    s32 pad;

                    Matrix_MultVec3f(&D_80126190, &heldActor->world.pos);
                    Matrix_RotateZYX(0, -0x4000, -0x4000, MTXMODE_APPLY);
                    Matrix_Get(&sp44);
                    Matrix_MtxFToYXZRotS(&sp44, &heldActor->world.rot, 0);
                    heldActor->shape.rot = heldActor->world.rot;

                    if (func_8002DD78(this) != 0) {
                        Matrix_Translate(500.0f, 300.0f, 0.0f, MTXMODE_APPLY);
                        Player_DrawHookshotReticle(
                            play, this, ((this->heldItemAction == PLAYER_IA_HOOKSHOT) ? 38600.0f : 77600.0f) * CVarGetFloat("gCheatHookshotReachMultiplier", 1.0f));
                    }
                }
            } else if (CVarGetInteger("gBowReticle", 0) && (
                        (this->heldItemAction == PLAYER_IA_BOW_FIRE) ||
                        (this->heldItemAction == PLAYER_IA_BOW_ICE) ||
                        (this->heldItemAction == PLAYER_IA_BOW_LIGHT) ||
                        (this->heldItemAction == PLAYER_IA_BOW) ||
                        (this->heldItemAction == PLAYER_IA_SLINGSHOT))) {
                if (heldActor != NULL) {
                    MtxF sp44;
                    s32 pad;

                    Matrix_RotateZYX(0, -15216, -17496, MTXMODE_APPLY);
                    Matrix_Get(&sp44);

                    if (func_8002DD78(this) != 0) {
                        Matrix_Translate(500.0f, 300.0f, 0.0f, MTXMODE_APPLY);
                        Player_DrawHookshotReticle(play, this, RETICLE_MAX);
                    }
                }
            }

            if ((this->unk_862 != 0) || ((func_8002DD6C(this) == 0) && (heldActor != NULL))) {
                if (!(this->stateFlags1 & 0x400) && (this->unk_862 != 0) && (this->exchangeItemId != EXCH_ITEM_NONE)) {
                    Math_Vec3f_Copy(&sGetItemRefPos, &this->leftHandPos);
                } else {
                    sGetItemRefPos.x = (this->bodyPartsPos[15].x + this->leftHandPos.x) * 0.5f;
                    sGetItemRefPos.y = (this->bodyPartsPos[15].y + this->leftHandPos.y) * 0.5f;
                    sGetItemRefPos.z = (this->bodyPartsPos[15].z + this->leftHandPos.z) * 0.5f;
                }

                if (this->unk_862 == 0) {
                    Math_Vec3f_Copy(&heldActor->world.pos, &sGetItemRefPos);
                }
            }
        }
    } else if (this->actor.scale.y >= 0.0f) {
        if (limbIndex == PLAYER_LIMB_SHEATH) {
            if ((this->rightHandType != 10) && (this->rightHandType != 0xFF)) {
                if (Player_IsChildWithHylianShield(this)) {
                    Player_UpdateShieldCollider(play, this, &this->shieldQuad, sSheathLimbModelShieldQuadVertices);
                }

                Matrix_TranslateRotateZYX(&sSheathLimbModelShieldOnBackPos, &sSheathLimbModelShieldOnBackZyxRot);
                Matrix_Get(&this->shieldMf);
            }
        } else if (limbIndex == PLAYER_LIMB_HEAD) {
            Matrix_MultVec3f(&D_801260D4, &this->actor.focus.pos);
        } else {
            Vec3f* vec = &sLeftRightFootLimbModelFootPos[(gSaveContext.linkAge)];

            Actor_SetFeetPos(&this->actor, limbIndex, PLAYER_LIMB_L_FOOT, vec, PLAYER_LIMB_R_FOOT, vec);
        }
    }
}

u32 func_80091738(PlayState* play, u8* segment, SkelAnime* skelAnime) {
    s16 linkObjectId = gLinkObjectIds[gSaveContext.linkAge];
    size_t size;
    void* ptr;

    size = gObjectTable[OBJECT_GAMEPLAY_KEEP].vromEnd - gObjectTable[OBJECT_GAMEPLAY_KEEP].vromStart;
    ptr = segment + 0x3800;
    DmaMgr_SendRequest1(ptr, gObjectTable[OBJECT_GAMEPLAY_KEEP].vromStart, size, __FILE__, __LINE__);

    size = gObjectTable[linkObjectId].vromEnd - gObjectTable[linkObjectId].vromStart;
    ptr = segment + 0x8800;
    DmaMgr_SendRequest1(ptr, gObjectTable[linkObjectId].vromStart, size, __FILE__, __LINE__);

    ptr = (void*)ALIGN16((intptr_t)ptr + size);

    gSegments[4] = VIRTUAL_TO_PHYSICAL(segment + 0x3800);
    gSegments[6] = VIRTUAL_TO_PHYSICAL(segment + 0x8800);

    SkelAnime_InitLink(play, skelAnime, gPlayerSkelHeaders[gSaveContext.linkAge], &gPlayerAnim_link_normal_wait, 9,
                       ptr, ptr, PLAYER_LIMB_MAX);

    return size + 0x8800 + 0x90;
}

u8 sPauseModelGroupBySword[] = { 2, 2, 5 };

s32 Player_OverrideLimbDrawPause(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, void* arg) {
    u8* ptr = arg;
    u8 modelGroup = sPauseModelGroupBySword[ptr[0] > 0 ? ptr[0] - 1 : 0];
    s32 type;
    s32 dListOffset = 0;
    Gfx** dLists;
    size_t ptrSize = sizeof(uint32_t);

    if ((modelGroup == 2) && !LINK_IS_ADULT && (ptr[1] == 2)) {
        modelGroup = 1;
    }

    if (limbIndex == PLAYER_LIMB_L_HAND) {
        type = gPlayerModelTypes[modelGroup][1];
        sLeftHandType = type;

        if (ptr[0] == 0)
            type = 0;

        if ((type == 4) && (gSaveContext.swordHealth <= 0.0f)) {
            dListOffset = 4;
        }
    } else if (limbIndex == PLAYER_LIMB_R_HAND) {
        type = gPlayerModelTypes[modelGroup][2];
        sRightHandType = type;
        if (type == 10) {
            dListOffset = ptr[1] * ptrSize;
        }
    } else if (limbIndex == PLAYER_LIMB_SHEATH) {
        type = gPlayerModelTypes[modelGroup][3];
        if ((type == 18) || (type == 19)) {
            dListOffset = ptr[1] * ptrSize;
        } else if (type == 16 && CVarGetInteger("gPauseLiveLink", 0)) {
            //if (ptr[0] == 1)
                //dListOffset = 4;
        }
    } else if (limbIndex == PLAYER_LIMB_WAIST) {
        type = gPlayerModelTypes[modelGroup][4];
        
        if (Player_IsCustomLinkModel()) {
            return 0;
        }
    } else {
        return 0;
    }

    dLists = &sPlayerDListGroups[type][gSaveContext.linkAge];
    *dList = dLists[dListOffset];

    return 0;
}

#include <overlays/actors/ovl_Demo_Effect/z_demo_effect.h>
void DemoEffect_DrawTriforceSpot(Actor* thisx, PlayState* play);

void Pause_DrawTriforceSpot(PlayState* play, s32 showLightColumn) {
    static DemoEffect triforce;
    static s16 rotation = 0;

    triforce.triforceSpot.crystalLightOpacity = 244;
    triforce.triforceSpot.triforceSpotOpacity = 249;
    triforce.triforceSpot.lightColumnOpacity = showLightColumn ? 244 : 0;
    triforce.triforceSpot.rotation = rotation;

    DemoEffect_DrawTriforceSpot(&triforce, play);

    rotation += 0x03E8;
}

void Player_DrawPauseImpl(PlayState* play, void* seg04, void* seg06, SkelAnime* skelAnime, Vec3f* pos, Vec3s* rot,
                   f32 scale, s32 sword, s32 tunic, s32 shield, s32 boots, s32 width, s32 height, Vec3f* eye, Vec3f* at,
                   f32 fovy, void* img1, void* img2) {
    static Vp viewport = { 128, 224, 511, 0, 128, 224, 511, 0 };
    static Lights1 lights1 = gdSPDefLights1(80, 80, 80, 255, 255, 255, 84, 84, 172);
    static Vec3f lightDir = { 89.8f, 0.0f, 89.8f };
    u8 sp12C[2];
    Gfx* opaRef;
    Gfx* xluRef;
    Gfx* kalRef;
    u16 perspNorm;
    Mtx* perspMtx = Graph_Alloc(play->state.gfxCtx, sizeof(Mtx));
    Mtx* lookAtMtx = Graph_Alloc(play->state.gfxCtx, sizeof(Mtx));

    OPEN_DISPS(play->state.gfxCtx);

    gSPSegment(POLY_OPA_DISP++, 0x00, NULL);

    gDPPipeSync(POLY_OPA_DISP++);

    gSPLoadGeometryMode(POLY_OPA_DISP++, 0);
    gSPTexture(POLY_OPA_DISP++, 0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF);
    gDPSetCombineMode(POLY_OPA_DISP++, G_CC_SHADE, G_CC_SHADE);
    gDPSetOtherMode(POLY_OPA_DISP++,
                    G_AD_DISABLE | G_CD_MAGICSQ | G_CK_NONE | G_TC_FILT | G_TF_BILERP | G_TT_NONE | G_TL_TILE |
                        G_TD_CLAMP | G_TP_PERSP | G_CYC_FILL | G_PM_NPRIMITIVE,
                    G_AC_NONE | G_ZS_PIXEL | G_RM_NOOP | G_RM_NOOP2);
    gSPLoadGeometryMode(POLY_OPA_DISP++, G_ZBUFFER | G_SHADE | G_CULL_BACK | G_LIGHTING | G_SHADING_SMOOTH);
    gDPSetScissor(POLY_OPA_DISP++, G_SC_NON_INTERLACE, 0, 0, width, height);
    gSPClipRatio(POLY_OPA_DISP++, FRUSTRATIO_1);

    viewport.vp.vscale[0] = viewport.vp.vtrans[0] = width * 2;
    viewport.vp.vscale[1] = viewport.vp.vtrans[1] = height * 2;
    gSPViewport(POLY_OPA_DISP++, &viewport);

    guPerspective(perspMtx, &perspNorm, fovy, (f32)width / (f32)height, 10.0f, 4000.0f, 1.0f);

    gSPPerspNormalize(POLY_OPA_DISP++, perspNorm);
    gSPMatrix(POLY_OPA_DISP++, perspMtx, G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_PROJECTION);

    guLookAt(lookAtMtx, eye->x, eye->y, eye->z, at->x, at->y, at->z, 0.0f, 1.0f, 0.0f);

    gSPMatrix(POLY_OPA_DISP++, lookAtMtx, G_MTX_NOPUSH | G_MTX_MUL | G_MTX_PROJECTION);

    sp12C[0] = sword;
    sp12C[1] = shield;

    Matrix_SetTranslateRotateYXZ(pos->x - ((CVarGetInteger("gPauseLiveLink", 0) && LINK_AGE_IN_YEARS == YEARS_ADULT) ? 25 : 0),
                                 pos->y - (CVarGetInteger("gPauseTriforce", 0) ? 16 : 0), pos->z, rot);
    Matrix_Scale(scale * (CVarGetInteger("gMirroredWorld", 0) ? -1 : 1), scale, scale, MTXMODE_APPLY);

    gSPSegment(POLY_OPA_DISP++, 0x04, seg04);
    gSPSegment(POLY_OPA_DISP++, 0x06, seg06);

    gSPSetLights1(POLY_OPA_DISP++, lights1);

    func_80093C80(play);

    POLY_OPA_DISP = Gfx_SetFog2(POLY_OPA_DISP++, 0, 0, 0, 0, 997, 1000);

    func_8002EABC(pos, &play->view.eye, &lightDir, play->state.gfxCtx);

    gSPSegment(POLY_OPA_DISP++, 0x0C, gCullBackDList);

    Player_DrawImpl(play, skelAnime->skeleton, skelAnime->jointTable, skelAnime->dListCount, 0, tunic, boots, 0,
                  Player_OverrideLimbDrawPause, NULL, &sp12C);

     if (CVarGetInteger("gPauseTriforce", 0)) {

        Matrix_SetTranslateRotateYXZ(pos->x - (LINK_AGE_IN_YEARS == YEARS_ADULT ? 25 : 0),
                                      pos->y + 280 + (LINK_AGE_IN_YEARS == YEARS_ADULT ? 48 : 0), pos->z, rot);
        Matrix_Scale(scale * (CVarGetInteger("gMirroredWorld", 0) ? -1 : 1), scale * 1, scale * 1, MTXMODE_APPLY);

        Gfx* ohNo = POLY_XLU_DISP;
        POLY_XLU_DISP = POLY_OPA_DISP;

        Pause_DrawTriforceSpot(play, 1);

        POLY_OPA_DISP = POLY_XLU_DISP;
        POLY_XLU_DISP = ohNo;
    }

    POLY_OPA_DISP = Play_SetFog(play, POLY_OPA_DISP++);

    CLOSE_DISPS(play->state.gfxCtx);
}

uintptr_t SelectedAnim = 0; // Current Animaiton on the menu
s16 EquipedStance; // Link's current mode (Two handed, One handed...)
s16 FrameCountSinceLastAnim = 0; // Time since last animation
s16 MinFrameCount; // Frame to wait before checking if we need to change the animation

void Player_DrawPause(PlayState* play, u8* segment, SkelAnime* skelAnime, Vec3f* pos, Vec3s* rot, f32 scale,
                   s32 sword, s32 tunic, s32 shield, s32 boots) {
    Input* p1Input = &play->state.input[0];
    Vec3f eye = { 0.0f, 0.0f, -400.0f };
    Vec3f at = { 0.0f, 0.0f, 0.0f };
    Vec3s* destTable;
    Vec3s* srcTable;
    s32 i;
    bool canswitchrnd = false;
    s16 SelectedMode = CVarGetInteger("gPauseLiveLink", 0);
    MinFrameCount = CVarGetInteger("gMinFrameCount", 200);

    gSegments[4] = VIRTUAL_TO_PHYSICAL(segment + 0x3800);
    gSegments[6] = VIRTUAL_TO_PHYSICAL(segment + 0x8800);

    uintptr_t* PauseMenuAnimSet[15][4] = {
        { 0, 0, 0, 0 }, // 0 = none
        // IDLE               // Two Handed       // No shield        // Kid Hylian Shield
        { gPlayerAnim_link_normal_wait, gPlayerAnim_link_fighter_wait_long, gPlayerAnim_link_normal_wait_free, gPlayerAnim_link_normal_wait_free }, // Idle
        { gPlayerAnim_link_normal_waitF_typeA_20f, gPlayerAnim_link_normal_waitF_typeA_20f, gPlayerAnim_link_normal_waitF_typeA_20f, gPlayerAnim_link_normal_waitF_typeA_20f }, // Idle look around
        { gPlayerAnim_link_waitF_itemA_20f, gPlayerAnim_link_waitF_itemA_20f, gPlayerAnim_link_waitF_itemA_20f, gPlayerAnim_link_waitF_itemA_20f }, // Idle Belt
        { gPlayerAnim_link_wait_itemC_20f, gPlayerAnim_link_wait_itemC_20f, gPlayerAnim_link_wait_itemC_20f, gPlayerAnim_link_wait_itemC_20f }, // Idle shield adjust
        { gPlayerAnim_link_wait_itemD1_20f, gPlayerAnim_link_wait_itemD2_20f, gPlayerAnim_link_wait_itemD1_20f, gPlayerAnim_link_wait_itemD1_20f }, // Idle test sword
        { gPlayerAnim_link_waitF_typeD_20f, gPlayerAnim_link_waitF_typeD_20f, gPlayerAnim_link_waitF_typeD_20f, gPlayerAnim_link_waitF_typeD_20f }, // Idle yawn
        { gPlayerAnim_link_anchor_waitR, gPlayerAnim_link_fighter_waitR_long, gPlayerAnim_link_anchor_waitR, gPlayerAnim_link_anchor_waitR }, // Battle Stance
        { gPlayerAnim_link_normal_walk_free, gPlayerAnim_link_fighter_walk_long, gPlayerAnim_link_normal_walk_free, gPlayerAnim_link_normal_walk_free }, // Walking (No shield)
        { gPlayerAnim_link_normal_walk, gPlayerAnim_link_fighter_walk_long, gPlayerAnim_link_normal_walk, gPlayerAnim_link_normal_walk }, // Walking (Holding shield)
        { gPlayerAnim_link_normal_run, gPlayerAnim_link_fighter_run_long, gPlayerAnim_link_normal_run, gPlayerAnim_link_normal_run }, // Running (No shield)
        { gPlayerAnim_link_normal_run_free, gPlayerAnim_link_fighter_run_long, gPlayerAnim_link_normal_run_free, gPlayerAnim_link_normal_run_free }, // Running (Holding shield)
        { gPlayerAnim_link_normal_talk_free_wait, gPlayerAnim_link_normal_talk_free_wait, gPlayerAnim_link_normal_talk_free_wait, gPlayerAnim_link_normal_talk_free_wait }, // Hand on hip
        { gPlayerAnim_link_fighter_power_kiru_wait, gPlayerAnim_link_fighter_Lpower_kiru_wait, gPlayerAnim_link_fighter_power_kiru_wait, gPlayerAnim_link_fighter_power_kiru_wait }, // Spin Charge
        { gPlayerAnim_link_demo_look_hand_wait, gPlayerAnim_link_demo_look_hand_wait, gPlayerAnim_link_demo_look_hand_wait, gPlayerAnim_link_demo_look_hand_wait }, // Look at hand
    };
    s16 AnimArraySize = ARRAY_COUNT(PauseMenuAnimSet);

    if (CVarGetInteger("gPauseLiveLink", 0) || CVarGetInteger("gPauseTriforce", 0)) {
        uintptr_t anim = 0; // Initialise anim

        if (CUR_EQUIP_VALUE(EQUIP_SWORD) >= 3) {
            EquipedStance = 1;
        } else if (CUR_EQUIP_VALUE(EQUIP_SHIELD) == 0) {
            EquipedStance = 2;
        } else if (CUR_EQUIP_VALUE(EQUIP_SHIELD) == 2 && LINK_AGE_IN_YEARS == YEARS_CHILD) {
            EquipedStance = 3;
        } else {
            // Link is idle so revert to 0
            EquipedStance = 0;
        }
        if (SelectedMode == 16) {
            // Apply Random function
            s16 SwitchAtFrame = 0;
            s16 CurAnimDuration = 0;
            if (FrameCountSinceLastAnim == 0) {
                // When opening Kaleido this will be passed one time
                SelectedAnim = rand() % (AnimArraySize - 0);
                if (SelectedAnim == 0) {
                    // prevent loading 0 that would result to a crash.
                    SelectedAnim = 1;
                }
            } else if (FrameCountSinceLastAnim >= 1) {
                SwitchAtFrame = Animation_GetLastFrame(PauseMenuAnimSet[SelectedAnim][EquipedStance]);
                CurAnimDuration = Animation_GetLastFrame(PauseMenuAnimSet[SelectedAnim][EquipedStance]);
                if (SwitchAtFrame < MinFrameCount) {
                    // Animation frame count is lower than minimal wait time then we wait for another round.
                    // This will be looped to always add current animation time if that still lower than minimum time
                    while (SwitchAtFrame < MinFrameCount) {
                        SwitchAtFrame = SwitchAtFrame + CurAnimDuration;
                    }
                } else if (CurAnimDuration >= MinFrameCount) {
                    // Since we have more (or same) animation time than min duration we set the wait time to animation
                    // time.
                    SwitchAtFrame = CurAnimDuration;
                }
                if (FrameCountSinceLastAnim >= SwitchAtFrame) {
                    SelectedAnim = rand() % (AnimArraySize - 0);
                    if (SelectedAnim == 0) {
                        // prevent loading 0 that would result to a crash.
                        SelectedAnim = 1;
                    }
                    FrameCountSinceLastAnim = 1;
                }
                anim = PauseMenuAnimSet[SelectedAnim][EquipedStance];
            }
            FrameCountSinceLastAnim++;
        } else if (SelectedMode == 17) {
            // Apply Random function
            s16 SwitchAtFrame = 0;
            s16 CurAnimDuration = 0;
            s16 LastAnim;
            if (FrameCountSinceLastAnim == 0) {
                // When opening Kaleido this will be passed one time
                SelectedAnim = (rand() % (6 - 1 + 1)) + 1;
                if (SelectedAnim == 0) {
                    // prevent loading 0 that would result to a crash.
                    SelectedAnim = 1;
                }
            } else if (FrameCountSinceLastAnim >= 1) {
                SwitchAtFrame = Animation_GetLastFrame(PauseMenuAnimSet[SelectedAnim][EquipedStance]);
                CurAnimDuration = Animation_GetLastFrame(PauseMenuAnimSet[SelectedAnim][EquipedStance]);
                if (SwitchAtFrame < MinFrameCount) {
                    // Animation frame count is lower than minimal wait time then we wait for another round.
                    // This will be looped to always add current animation time if that still lower than minimum time
                    while (SwitchAtFrame < MinFrameCount) {
                        SwitchAtFrame = SwitchAtFrame + CurAnimDuration;
                    }
                } else if (CurAnimDuration >= MinFrameCount) {
                    // Since we have more (or same) animation time than min duration we set the wait time to animation
                    // time.
                    SwitchAtFrame = CurAnimDuration;
                }
                if (FrameCountSinceLastAnim >= SwitchAtFrame) {
                    LastAnim = SelectedAnim;
                    if (LastAnim==1) {
                        if ((CUR_EQUIP_VALUE(EQUIP_SWORD)!=PLAYER_SWORD_NONE) && (CUR_EQUIP_VALUE(EQUIP_SHIELD)!= PLAYER_SHIELD_NONE)) { // if the player has a sword and shield equipped
                            if ((LINK_AGE_IN_YEARS == YEARS_ADULT) || (CUR_EQUIP_VALUE(EQUIP_SHIELD) == PLAYER_SHIELD_DEKU)) { // if he's an adult or a kid with the deku shield
                                SelectedAnim = (rand() % (6 - 2 + 1)) + 2; // select any 5 animations that aren't the default standing anim
                            } else { //else if he's a child with a shield that isn't the deku shield
                                s16 randval = (rand() % (5 - 2 + 1)) + 2; // 4 animations
                                if (randval==4) { //if its the shield anim
                                    SelectedAnim==6; // set to yawn anim
                                } else {
                                    SelectedAnim=randval;
                                }
                            }
                        } else if ((CUR_EQUIP_VALUE(EQUIP_SWORD) != PLAYER_SWORD_NONE) && (CUR_EQUIP_VALUE(EQUIP_SHIELD)==PLAYER_SHIELD_NONE)) { // if the player has a sword equipped but no shield
                            s16 randval = (rand() % (5 - 2 + 1)) + 2; // 4 animations
                            if (randval==4) { //if its the shield anim
                                SelectedAnim==6; // set to yawn anim
                            } else {
                                SelectedAnim=randval;
                            }
                        } else if ((CUR_EQUIP_VALUE(EQUIP_SWORD) == PLAYER_SWORD_NONE) && (CUR_EQUIP_VALUE(EQUIP_SHIELD)!=PLAYER_SHIELD_NONE)) { //if the player has a shield equipped but no sword
                            if ((LINK_AGE_IN_YEARS == YEARS_ADULT) || (CUR_EQUIP_VALUE(EQUIP_SHIELD) == PLAYER_SHIELD_DEKU)) {// if he's an adult or a kid with the deku shield
                            s16 randval = (rand() % (5 - 2 + 1)) + 2; // 4 animations
                            if (randval==5) { //if its the sword anim
                                SelectedAnim==6; // set to yawn anim
                            } else {
                                SelectedAnim=randval;
                            }
                            } else {
                                s16 randval = (rand() % (4 - 2 + 1)) + 2; // 3 animations
                                if (randval==4) { //if its the shield anim
                                    SelectedAnim==6; // set to yawn anim
                                } else {
                                    SelectedAnim=randval;
                                }
                            } 
                        } else if ((CUR_EQUIP_VALUE(EQUIP_SWORD) == PLAYER_SWORD_NONE) && (CUR_EQUIP_VALUE(EQUIP_SHIELD)==PLAYER_SHIELD_NONE)) { // if the player has no sword or shield equipped
                            s16 randval = (rand() % (4 - 2 + 1)) + 2; // 3 animations
                            if (randval==4) { //if its the shield anim
                                SelectedAnim==6; // set to yawn anim
                            } else {
                                SelectedAnim=randval;
                            }
                        }
                    } else {
                        SelectedAnim = 1;
                    }
                    if (SelectedAnim == 0) {
                        // prevent loading 0 that would result to a crash. Also makes sure default idle is every other anim
                        SelectedAnim = 1;
                    }
                    FrameCountSinceLastAnim = 1;
                }
                anim = PauseMenuAnimSet[SelectedAnim][EquipedStance];
            }
            FrameCountSinceLastAnim++;
        } else if (SelectedMode == 15) {
            // When opening Kaleido this will be passed one time
            if (FrameCountSinceLastAnim < 1) {
                SelectedAnim = rand() % (AnimArraySize - 0);
                FrameCountSinceLastAnim++;
                if (SelectedAnim == 0) {
                    // prevent loading 0 that would result to a crash.
                    SelectedAnim = 1;
                }
                FrameCountSinceLastAnim = 1;
            }
            if (CHECK_BTN_ALL(p1Input->press.button, BTN_B) || CHECK_BTN_ALL(p1Input->press.button, BTN_START)) {
                FrameCountSinceLastAnim = 0;
            }
            anim = PauseMenuAnimSet[SelectedAnim][EquipedStance];
        } else if (SelectedMode < 16) {
            // Not random so we place our CVar as SelectedAnim
            SelectedAnim = SelectedMode;
            anim = PauseMenuAnimSet[SelectedAnim][EquipedStance];
        }

        anim = PauseMenuAnimSet[SelectedAnim][EquipedStance];

        //anim = gPlayerAnim_link_wait_itemD2_20f; // Use for biggoron sword?

        if (CVarGetInteger("gPauseTriforce", 0)) {
            anim = gPlayerAnim_link_magic_kaze2;
            sword = 0;
            shield = 0;
        }

        if (skelAnime->animation != anim) {
            LinkAnimation_Change(play, skelAnime, anim, 1.0f, 0.0f, Animation_GetLastFrame(anim), ANIMMODE_LOOP,
                                 -6.0f);
        }

        LinkAnimation_Update(play, skelAnime);

        if (!LINK_IS_ADULT) {
            // Link is placed too far up by default when animating
            at.y += 60;
        }
    } else {

        if (!LINK_IS_ADULT) {
            if (shield == PLAYER_SHIELD_DEKU) {
                srcTable = gLinkPauseChildDekuShieldJointTable;
            } else {
                srcTable = gLinkPauseChildJointTable;
            }
        } else {
            if (sword == 3) {
                srcTable = gLinkPauseAdultBgsJointTable;
            } else if (shield != PLAYER_SHIELD_NONE) {
                srcTable = gLinkPauseAdultShieldJointTable;
            } else {
                srcTable = gLinkPauseAdultJointTable;
            }
        }

        srcTable = ResourceMgr_LoadArrayByNameAsVec3s(srcTable);
        destTable = skelAnime->jointTable;
        for (i = 0; i < skelAnime->limbCount; i++) {
            *destTable++ = *srcTable++;
        }
    }


    Player_DrawPauseImpl(play, segment + 0x3800, segment + 0x8800, skelAnime, pos, rot, scale, sword, tunic, shield,
                  boots, 64, 112, &eye, &at, 60.0f, play->state.gfxCtx->curFrameBuffer,
                  play->state.gfxCtx->curFrameBuffer + 0x1C00);
}
