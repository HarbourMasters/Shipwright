#include "soh/Enhancements/game-interactor/GameInteractor_Hooks.h"
#include "soh/ShipInit.hpp"
#include <spdlog/spdlog.h>

extern "C" {
#include "macros.h"
#include "variables.h"
#include "functions.h"
#include "soh/ResourceManagerHelpers.h"
#include "objects/object_link_boy/object_link_boy.h"
#include "objects/object_link_child/object_link_child.h"

extern PlayState* gPlayState;
}

static constexpr int32_t CVAR_SCALEADULTEQUIPMENTASCHILD_DEFAULT = 0;
#define CVAR_SCALEADULTEQUIPMENTASCHILD_NAME CVAR_ENHANCEMENT("ScaleAdultEquipmentAsChild")
#define CVAR_SCALEADULTEQUIPMENTASCHILD_VALUE \
    CVarGetInteger(CVAR_SCALEADULTEQUIPMENTASCHILD_NAME, CVAR_SCALEADULTEQUIPMENTASCHILD_DEFAULT)

static constexpr int32_t CVAR_CHILDHOLDSHYLIANSHIELD_DEFAULT = 0;
#define CVAR_CHILDHOLDSHYLIANSHIELD_NAME CVAR_CHEAT("ChildHoldsHylianShield")
#define CVAR_CHILDHOLDSHYLIANSHIELD_VALUE \
    CVarGetInteger(CVAR_CHILDHOLDSHYLIANSHIELD_NAME, CVAR_CHILDHOLDSHYLIANSHIELD_DEFAULT)

static void ResetPatchChildHylianShield() {
    ResourceMgr_UnpatchGfxByName(gLinkAdultHylianShieldSwordAndSheathNearDL, "childHylianShield1");
    ResourceMgr_UnpatchGfxByName(gLinkAdultHylianShieldSwordAndSheathNearDL, "childHylianShield2");
    ResourceMgr_UnpatchGfxByName(gLinkAdultHylianShieldSwordAndSheathNearDL, "childHylianShield3");
}

static void UpdatePatchChildHylianShield() {
    ResetPatchChildHylianShield();

    if (CVAR_SCALEADULTEQUIPMENTASCHILD_VALUE && LINK_IS_CHILD) {
        if (gSaveContext.equips.buttonItems[0] == ITEM_SWORD_KOKIRI ||
            gSaveContext.equips.buttonItems[0] == ITEM_FISHING_POLE) {
            ResourceMgr_PatchGfxByName(gLinkAdultHylianShieldSwordAndSheathNearDL, "childHylianShield1", 82,
                                       gsSPDisplayListOTRFilePath(gLinkChildSwordAndSheathNearDL));
            ResourceMgr_PatchGfxByName(gLinkAdultHylianShieldSwordAndSheathNearDL, "childHylianShield2", 83,
                                       gsSPEndDisplayList());
        }
        if (gSaveContext.equips.buttonItems[0] == ITEM_NONE || gSaveContext.equips.buttonItems[0] == ITEM_STICK) {
            ResourceMgr_PatchGfxByName(gLinkAdultHylianShieldSwordAndSheathNearDL, "childHylianShield3", 82,
                                       gsSPEndDisplayList());
        }
    }
}

static void RegisterChildHoldsHylianShieldGraphics() {
    ResetPatchChildHylianShield();

    COND_HOOK(OnLoadGame, CVAR_SCALEADULTEQUIPMENTASCHILD_VALUE, [](int32_t fileNum) {
        if (gPlayState == nullptr) {
            return;
        }
        Player* player = GET_PLAYER(gPlayState);
        Player_SetModels(player, Player_ActionToModelGroup(player, player->heldItemAction));
    });

    COND_HOOK(OnPlayerUpdate, CVAR_SCALEADULTEQUIPMENTASCHILD_VALUE, []() {
        static uint16_t lastItemOnB = gSaveContext.equips.buttonItems[0];
        if (lastItemOnB != gSaveContext.equips.buttonItems[0]) {
            UpdatePatchChildHylianShield();
            lastItemOnB = gSaveContext.equips.buttonItems[0];
        }
    });

    COND_HOOK(OnSceneInit, CVAR_SCALEADULTEQUIPMENTASCHILD_VALUE,
              [](int16_t sceneNum) { UpdatePatchChildHylianShield(); });
}

static void RegisterChildHoldsHylianShieldGameplay() {
    // Skip vanilla check for making child Link have the Hylian Shield on his back, allowing for it to be used in hand
    COND_VB_SHOULD(VB_BE_CHILD_WITH_HYLIAN_SHIELD, CVAR_CHILDHOLDSHYLIANSHIELD_VALUE, { *should = false; });

    // Deku Scrub Projectile Reflection
    COND_VB_SHOULD(VB_REFLECT_NUTSBALL, CVAR_CHILDHOLDSHYLIANSHIELD_VALUE, {
        Player* player = GET_PLAYER(gPlayState);

        if (LINK_IS_CHILD && (player->currentShield == PLAYER_SHIELD_HYLIAN)) {
            *should = true;
        }
    });

    // Octorok Projectile Reflection
    COND_VB_SHOULD(VB_REFLECT_OCTOROK_PROJECTILE, CVAR_CHILDHOLDSHYLIANSHIELD_VALUE, {
        Player* player = GET_PLAYER(gPlayState);

        if (LINK_IS_CHILD && (player->currentShield == PLAYER_SHIELD_HYLIAN)) {
            *should = true;
        }
    });
}

static RegisterShipInitFunc initFunc_Graphics(RegisterChildHoldsHylianShieldGraphics,
                                              { CVAR_SCALEADULTEQUIPMENTASCHILD_NAME });

static RegisterShipInitFunc initFunc_Gameplay(RegisterChildHoldsHylianShieldGameplay,
                                              { CVAR_CHILDHOLDSHYLIANSHIELD_NAME });
