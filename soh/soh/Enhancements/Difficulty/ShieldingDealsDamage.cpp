#include <libultraship/bridge/consolevariablebridge.h>

#include "soh/Enhancements/game-interactor/GameInteractor.h"
#include "soh/ShipInit.hpp"

extern "C" {
#include "z64.h"
#include "functions.h"
extern PlayState* gPlayState;
}

static constexpr int32_t CVAR_SHIELDING_DEALS_DAMAGE_DEFAULT = 0;
#define CVAR_SHIELDING_DEALS_DAMAGE_NAME CVAR_ENHANCEMENT("ShieldingDealsDamage")
#define CVAR_SHIELDING_DEALS_DAMAGE_VALUE \
    CVarGetInteger(CVAR_SHIELDING_DEALS_DAMAGE_NAME, CVAR_SHIELDING_DEALS_DAMAGE_DEFAULT)

static void RegisterShieldingDealsDamage() {
    COND_VB_SHOULD(VB_PLAYER_BLOCK_ATTACK_WITH_SHIELD, CVAR_SHIELDING_DEALS_DAMAGE_VALUE, {
        Player* player = va_arg(args, Player*);
        Actor* attacker = player->shieldQuad.base.ac;

        // Twinrova's magic can only be reflected, so it is still blocked.
        // Deku Tree's Hint Deku Scrubs that guard the Slingshot (room 1) and the 2-3-1 puzzle before the
        // boss (room 9) can only be stunned by reflecting their nut (EnNutsball params 1)
        bool isScrubNut = attacker != NULL && attacker->id == ACTOR_EN_NUTSBALL && attacker->params == 1 &&
                          gPlayState->sceneNum == SCENE_DEKU_TREE &&
                          (gPlayState->roomCtx.curRoom.num == 1 || gPlayState->roomCtx.curRoom.num == 9);
        if (*should && attacker != NULL && attacker->id != ACTOR_BOSS_TW && !isScrubNut) {
            // If the body was also hit this frame, that hit already carries its own damage.
            if (!(player->cylinder.base.acFlags & AC_HIT)) {
                // Run the blocked hit through the body damage path, so it deals what it would have if it landed.
                player->cylinder.base.acFlags |= AC_HIT;
                player->cylinder.base.ac = attacker;
                player->cylinder.info.acHit = player->shieldQuad.info.acHit;
                player->cylinder.info.acHitInfo = player->shieldQuad.info.acHitInfo;
                player->cylinder.info.bumperFlags |= BUMP_HIT;
                CollisionCheck_ApplyDamage(gPlayState, &gPlayState->colChkCtx, &player->cylinder.base,
                                           &player->cylinder.info);
            }
            *should = false;
        }
    });
}

static RegisterShipInitFunc initFunc(RegisterShieldingDealsDamage, { CVAR_SHIELDING_DEALS_DAMAGE_NAME });
