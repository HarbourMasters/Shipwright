#include <libultraship/bridge.h>
#include "soh/Enhancements/game-interactor/GameInteractor.h"
#include "soh/ShipInit.hpp"

#define CVAR_ENEMY_RETARGET_NAME CVAR_ENHANCEMENT("EnemyRetarget")
#define CVAR_ENEMY_RETARGET_VALUE CVarGetInteger(CVAR_ENEMY_RETARGET_NAME, 0)

void RegisterEnemyRetarget() {
    COND_VB_SHOULD(VB_DROP_Z_TARGET, CVAR_ENEMY_RETARGET_VALUE, {
        Player* player = va_arg(args, Player*);
        PlayState* play = va_arg(args, PlayState*);

        // Determine if the lock break is due to the enemy dying/despawning
        bool targetDied = (player->focusActor->update == NULL) || !(player->focusActor->flags & ACTOR_FLAG_ATTENTION_ENABLED);

        if (targetDied) {
            bool usingHoldTargeting = (gSaveContext.zTargetSetting != 0);
            bool zButtonHeld = CHECK_BTN_ALL(play->state.input[0].cur.button, BTN_Z);

            if (!usingHoldTargeting || zButtonHeld) {
                // Grab the secondary target the game engine already calculated
                Actor* nextTarget = play->actorCtx.targetCtx.unk_94;

                if (nextTarget != NULL && !(nextTarget->flags & ACTOR_FLAG_LOCK_ON_DISABLED) &&
                    CHECK_FLAG_ALL(nextTarget->flags, ACTOR_FLAG_ATTENTION_ENABLED | ACTOR_FLAG_HOSTILE)) {
                    
                    // Seamlessly transition to the new target
                    player->focusActor = nextTarget;
                    player->zTargetActiveTimer = 15;
                    
                    // Tell the engine NOT to drop the lock-on!
                    *should = false; 
                }
            }
        }
    });
}

static RegisterShipInitFunc initFunc(RegisterEnemyRetarget, { CVAR_ENEMY_RETARGET_NAME });