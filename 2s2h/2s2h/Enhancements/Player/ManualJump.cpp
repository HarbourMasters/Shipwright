#include <libultraship/bridge/consolevariablebridge.h>
#include "2s2h/GameInteractor/GameInteractor.h"
#include "2s2h/ShipInit.hpp"

extern "C" {
#include "variables.h"
#include "assets/objects/gameplay_keep/gameplay_keep.h"
void func_80834DB8(Player* player, PlayerAnimationHeader* anim, f32 speed, PlayState* play);
void func_80834D50(PlayState* play, Player* player, PlayerAnimationHeader* anim, f32 speed, u16 sfxId);
void func_808395F0(PlayState* play, Player* player, PlayerMeleeWeaponAnimation meleeWeaponAnim, f32 linearVelocity,
                   f32 yVelocity);
}

#define CVAR_NAME "gEnhancements.Player.ManualJump"
#define CVAR CVarGetInteger(CVAR_NAME, 0)

void RegisterManualJump() {
    COND_VB_SHOULD(VB_START_JUMPSLASH, CVAR, {
        Player* player = GET_PLAYER(gPlayState);
        s32 temp_a2 = player->controlStickDirections[player->controlStickDataIndex];

        *should = false;

        if (player->transformation == PLAYER_FORM_ZORA) {
            func_808395F0(gPlayState, player, PLAYER_MWA_JUMPSLASH_START, 5.0f, 5.0f);
        } else if (temp_a2 == 0) {
            // Leap
            func_80834D50(gPlayState, player, (PlayerAnimationHeader*)&gPlayerAnim_link_fighter_front_jump, 5.8f,
                          NA_SE_VO_LI_SWORD_N);
        } else {
            // Jump
            func_80834DB8(player, (PlayerAnimationHeader*)&gPlayerAnim_link_normal_jump, REG(69) / 100.0f, gPlayState);
        }
    });
}

static RegisterShipInitFunc initFunc(RegisterManualJump, { CVAR_NAME });
