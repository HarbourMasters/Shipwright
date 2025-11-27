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

extern SaveContext gSaveContext;
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

static void UpdatePatchChildHylianShield() {
    SPDLOG_DEBUG("ChildHoldsHylianShield: UpdatePatchChildHylianShield called");

    ResourceMgr_UnpatchGfxByName(gLinkAdultHylianShieldSwordAndSheathNearDL, "childHylianShield1");
    ResourceMgr_UnpatchGfxByName(gLinkAdultHylianShieldSwordAndSheathNearDL, "childHylianShield2");
    ResourceMgr_UnpatchGfxByName(gLinkAdultHylianShieldSwordAndSheathNearDL, "childHylianShield3");

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
    COND_HOOK(OnLoadGame, true, [](int32_t fileNum) {
        if (gPlayState == nullptr) {
            return;
        }
        Player* player = GET_PLAYER(gPlayState);
        Player_SetModels(player, Player_ActionToModelGroup(player, player->heldItemAction));
    });

    COND_HOOK(OnPlayerUpdate, true, []() {
        static uint16_t lastItemOnB = gSaveContext.equips.buttonItems[0];
        if (lastItemOnB != gSaveContext.equips.buttonItems[0]) {
            UpdatePatchChildHylianShield();
            lastItemOnB = gSaveContext.equips.buttonItems[0];
        }
    });

    COND_HOOK(OnSceneInit, true, [](int16_t sceneNum) { UpdatePatchChildHylianShield(); });
}

static void RegisterChildHoldsHylianShieldReflect() {
    COND_VB_SHOULD(VB_REFLECT_NUTSBALL, CVAR_CHILDHOLDSHYLIANSHIELD_VALUE, {
        Player* player = GET_PLAYER(gPlayState);

        if (LINK_IS_CHILD && (player->currentShield == PLAYER_SHIELD_HYLIAN)) {
            SPDLOG_DEBUG("Reflecting Nutsball");
            *should = true;
        }
    });

    COND_VB_SHOULD(VB_REFLECT_OCTOROK_PROJECTILE, CVAR_CHILDHOLDSHYLIANSHIELD_VALUE, {
        Player* player = GET_PLAYER(gPlayState);

        if (LINK_IS_CHILD && (player->currentShield == PLAYER_SHIELD_HYLIAN)) {
            SPDLOG_DEBUG("Reflecting Octorok Projectile");
            *should = true;
        }
    });
}

static RegisterShipInitFunc initFunc_Graphics(RegisterChildHoldsHylianShieldGraphics);

static RegisterShipInitFunc initFunc_Reflect(RegisterChildHoldsHylianShieldReflect,
                                             { CVAR_CHILDHOLDSHYLIANSHIELD_NAME });
