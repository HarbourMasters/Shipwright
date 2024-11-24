#include "Holiday.hpp"

#define AUTHOR "aMannus"
#define CVAR(v) "gHoliday." AUTHOR "." v

extern "C" {
#include <z64.h>;
#include "functions.h";
#include "variables.h";
#include "macros.h";
#include "objects/gameplay_keep/gameplay_keep.h"
extern PlayState* gPlayState;
}

uint8_t rocsUseCount = 0;

static void ConfigurationChanged() {

    COND_HOOK(OnPlayerUpdate, CVarGetInteger(CVAR("RocsFeather"), 0), []() {
        Player* player = GET_PLAYER(gPlayState);
        // Reset Rocs count when touching the ground
        if (player->actor.bgCheckFlags & 1) {
            rocsUseCount = 0;
        }
    });

    COND_VB_SHOULD(VB_USE_ITEM, CVarGetInteger(CVAR("RocsFeather"), 0), {
        int32_t* usedItem = va_arg(args, int32_t*);
        Player* player = GET_PLAYER(gPlayState);

        // Roc's Feather behaviour
       if (*usedItem == ITEM_NAYRUS_LOVE) {
           *should = false;

           if (!rocsUseCount) {
               rocsUseCount++;
               player->linearVelocity = 5.0f;
               player->actor.velocity.y = 8.0f;
               player->actor.world.rot.y = player->yaw = player->actor.shape.rot.y;

               func_80838940(player, (LinkAnimationHeader*)&gPlayerAnim_link_fighter_backturn_jump,
                             !(2 & 1) ? 5.8f : 3.5f, gPlayState, 0);

               Vec3f effectsPos = player->actor.home.pos;
               effectsPos.y += 3;
               f32 effectsScale = 1;
               if (!gSaveContext.linkAge) {
                   effectsScale = 1.5f;
               }
               EffectSsGRipple_Spawn(gPlayState, &effectsPos, 200 * effectsScale, 300 * effectsScale, 1);
               EffectSsGSplash_Spawn(gPlayState, &effectsPos, NULL, NULL, 0, 150 * effectsScale);

               player->stateFlags2 &= ~(PLAYER_STATE2_HOPPING);

               Player_PlaySfx(&player->actor, NA_SE_PL_SKIP);
           }
        } 
    });
}

static void DrawMenu() {
    ImGui::SeparatorText(AUTHOR);

    if (UIWidgets::EnhancementCheckbox("Roc's Feather", CVAR("RocsFeather"))) {
        ConfigurationChanged();
    }
    UIWidgets::Tooltip("Using Nayru's Love will now act as Roc's Feather instead! No magic required.");
}

static void RegisterMod() {
    // #region Leave this alone unless you know what you are doing
    ConfigurationChanged();
    // #endregion
}

static Holiday holiday(DrawMenu, RegisterMod);
