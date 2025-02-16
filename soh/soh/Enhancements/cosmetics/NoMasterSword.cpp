#include "soh/Enhancements/game-interactor/GameInteractor_Hooks.h"
#include "soh/ShipInit.hpp"
#include "soh/OTRGlobals.h"
#include "soh/ResourceManagerHelpers.h"
#include "objects/object_link_boy/object_link_boy.h"

extern "C" {
#include "functions.h"
#include "macros.h"
#include "variables.h"

extern PlayState* gPlayState;
extern void Player_StartMode_Idle(PlayState*, Player*);
extern u8 Randomizer_GetSettingValue(RandomizerSettingKey);
}

void UpdateNoMSPatch() {
    // Condition for patching
    bool shouldPatch = (gSaveContext.equips.buttonItems[0] != ITEM_SWORD_MASTER &&
                        gSaveContext.equips.buttonItems[0] != ITEM_SWORD_BGS &&
                        gSaveContext.equips.buttonItems[0] != ITEM_SWORD_KNIFE &&
                        (gSaveContext.equips.buttonItems[0] != ITEM_FISHING_POLE || 
                        (!CHECK_OWNED_EQUIP(EQUIP_TYPE_SWORD, EQUIP_INV_SWORD_MASTER) &&
                            !CHECK_OWNED_EQUIP(EQUIP_TYPE_SWORD, EQUIP_INV_SWORD_BIGGORON) &&
                            !CHECK_OWNED_EQUIP(EQUIP_TYPE_SWORD, EQUIP_INV_SWORD_BROKENGIANTKNIFE))));

    if (OTRGlobals::Instance->gRandomizer->GetRandoSettingValue(RSK_SHUFFLE_MASTER_SWORD) && shouldPatch) {
        // Patching if conditions are met
        for (int i = 43; i <= 46; i++) {
            std::string patchName = "adultNoMS." + std::to_string(1) + "." + std::to_string(i);
            ResourceMgr_PatchGfxByName(gLinkAdultHylianShieldSwordAndSheathFarDL, patchName.c_str(), i, gsDPNoOp());
        }
        for (int i = 62; i <= 150; i++) {
            std::string patchName = "adultNoMS." + std::to_string(2) + "." + std::to_string(i);
            ResourceMgr_PatchGfxByName(gLinkAdultMirrorShieldSwordAndSheathNearDL, patchName.c_str(), i, gsDPNoOp());
        }
        for (int i = 61; i <= 118; i++) {
            std::string patchName = "adultNoMS." + std::to_string(3) + "." + std::to_string(i);
            ResourceMgr_PatchGfxByName(gLinkAdultMirrorShieldSwordAndSheathFarDL, patchName.c_str(), i, gsDPNoOp());
        }
        ResourceMgr_PatchGfxByName(gLinkAdultHylianShieldSwordAndSheathNearDL, "adultNoMSHylianShield1", 75, gsSPEndDisplayList());
        ResourceMgr_PatchGfxByName(gLinkAdultHylianShieldSwordAndSheathFarDL, "adultNoMSHylianShield2", 71, gsSPEndDisplayList());
        ResourceMgr_PatchGfxByName(gLinkAdultMasterSwordAndSheathNearDL, "adultNoMasterSword1", 2, gsSPEndDisplayList());
        ResourceMgr_PatchGfxByName(gLinkAdultMasterSwordAndSheathFarDL, "adultNoMasterSword2", 2, gsSPEndDisplayList());
    } else {
        // Unpatching if conditions are not met
        for (int i = 43; i <= 46; i++) {
            std::string patchName = "adultNoMS." + std::to_string(1) + "." + std::to_string(i);
            ResourceMgr_UnpatchGfxByName(gLinkAdultHylianShieldSwordAndSheathFarDL, patchName.c_str());
        }
        for (int i = 62; i <= 150; i++) {
            std::string patchName = "adultNoMS." + std::to_string(2) + "." + std::to_string(i);
            ResourceMgr_UnpatchGfxByName(gLinkAdultMirrorShieldSwordAndSheathNearDL, patchName.c_str());
        }
        for (int i = 61; i <= 118; i++) {
            std::string patchName = "adultNoMS." + std::to_string(3) + "." + std::to_string(i);
            ResourceMgr_UnpatchGfxByName(gLinkAdultMirrorShieldSwordAndSheathFarDL, patchName.c_str());
        }
        ResourceMgr_UnpatchGfxByName(gLinkAdultHylianShieldSwordAndSheathNearDL, "adultNoMSHylianShield1");
        ResourceMgr_UnpatchGfxByName(gLinkAdultHylianShieldSwordAndSheathFarDL, "adultNoMSHylianShield2");
        ResourceMgr_UnpatchGfxByName(gLinkAdultMasterSwordAndSheathNearDL, "adultNoMasterSword1");
        ResourceMgr_UnpatchGfxByName(gLinkAdultMasterSwordAndSheathFarDL, "adultNoMasterSword2");
    }
}

// copied from z_player (right above Player_StartMode_TimeTravel)
static Vec3f D_808546F4 = { -1.0f, 69.0f, 20.0f };

#define MASTER_SWORD_SHUFFLED (OTRGlobals::Instance->gRandomizer->GetRandoSettingValue(RSK_SHUFFLE_MASTER_SWORD) != 0)

void RegisterNoMasterSword() {
    // don't show link taking out master sword to put in pedestal when we don't have a master sword
    COND_VB_SHOULD(VB_SHOW_MASTER_SWORD_TO_PLACE_IN_PEDESTAL, IS_RANDO && MASTER_SWORD_SHUFFLED, {
        // if the vanilla condition is false respect it
        if (!*should) {
            return;
        }

        if (!CHECK_OWNED_EQUIP(EQUIP_TYPE_SWORD, EQUIP_INV_SWORD_MASTER)) {
            *should = false;
        }
    });
    
    // skip post pedestal animation when we don't have a master sword
    COND_VB_SHOULD(VB_EXECUTE_PLAYER_STARTMODE_FUNC, IS_RANDO && MASTER_SWORD_SHUFFLED, {
        int32_t startMode = va_arg(args, int32_t);
        Player* player = GET_PLAYER(gPlayState);

        if (startMode == PLAYER_START_MODE_TIME_TRAVEL && !CHECK_OWNED_EQUIP(EQUIP_TYPE_SWORD, EQUIP_INV_SWORD_MASTER)) {
            // don't run the vanilla startMode func
            *should = false;
            
            // position link correctly
            Math_Vec3f_Copy(&player->actor.world.pos, &D_808546F4);
            player->yaw = player->actor.shape.rot.y = -0x8000;
            
            // execute the idle startMode func
            Player_StartMode_Idle(gPlayState, player);
        }
    });

    COND_HOOK(OnPlayerUpdate, IS_RANDO, []{
        static uint16_t lastItemOnB = gSaveContext.equips.buttonItems[0];
        if (lastItemOnB != gSaveContext.equips.buttonItems[0]) {
            UpdateNoMSPatch();
            lastItemOnB = gSaveContext.equips.buttonItems[0];
        }
    });

    COND_HOOK(OnSceneSpawnActors, IS_RANDO, UpdateNoMSPatch);
}

static RegisterShipInitFunc initFunc(RegisterNoMasterSword, { "IS_RANDO" });
