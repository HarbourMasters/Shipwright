#include <libultraship/bridge/consolevariablebridge.h>
#include "2s2h/GameInteractor/GameInteractor.h"
#include "2s2h/ShipInit.hpp"
#include "2s2h/Enhancements/Enhancements.h"

extern "C" {
#include "overlays/actors/ovl_Door_Ana/z_door_ana.h"
}

#define CVAR_NAME "gEnhancements.DifficultyOptions.HiddenGrottosVisibility"
#define CVAR CVarGetInteger(CVAR_NAME, 0)

void drawHiddenGrottoMarker(Actor* actor) {
    s32 grottoType = DOORANA_GET_TYPE(actor);
    if (grottoType == DOORANA_TYPE_HIDDEN_STORMS || grottoType == DOORANA_TYPE_HIDDEN_BOMB) {
        if (gGameState->frames % 4) {
            return;
        }

        static Vec3f sVelocity = { 0.0f, 3.0f, 0.0f };
        static Vec3f sAccel = { 0.0f, 0.0f, 0.0f };
        static Color_RGBA8 sPrimColor = { 255, 255, 255, 255 };
        static Color_RGBA8 sEnvColor = { 0, 255, 64, 255 };
        Vec3f newPos;

        newPos.x = Rand_CenteredFloat(10.0f) + actor->world.pos.x;
        newPos.y = (Rand_ZeroOne() * 10.0f) + actor->world.pos.y;
        newPos.z = Rand_CenteredFloat(10.0f) + actor->world.pos.z;

        EffectSsKirakira_SpawnDispersed(gPlayState, &newPos, &sVelocity, &sAccel, &sPrimColor, &sEnvColor, 5000, 16);
    }
}

void RegisterHiddenGrottosVisibility() {
    COND_ID_HOOK(OnActorDraw, ACTOR_DOOR_ANA, CVAR == HIDDEN_GROTTOS_VISIBLITY_HAVE_MASK_OF_TRUTH, [](Actor* actor) {
        if (INV_CONTENT(ITEM_MASK_TRUTH) == ITEM_MASK_TRUTH) {
            drawHiddenGrottoMarker(actor);
        }
    });

    COND_ID_HOOK(OnActorDraw, ACTOR_DOOR_ANA, CVAR == HIDDEN_GROTTOS_VISIBLITY_WEAR_MASK_OF_TRUTH, [](Actor* actor) {
        if (Player_GetMask(gPlayState) == PLAYER_MASK_TRUTH) {
            drawHiddenGrottoMarker(actor);
        }
    });

    COND_ID_HOOK(OnActorDraw, ACTOR_DOOR_ANA, CVAR == HIDDEN_GROTTOS_VISIBLITY_ALWAYS,
                 [](Actor* actor) { drawHiddenGrottoMarker(actor); });
}

static RegisterShipInitFunc initFunc(RegisterHiddenGrottosVisibility, { CVAR_NAME });
