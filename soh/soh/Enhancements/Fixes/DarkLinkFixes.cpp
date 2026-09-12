#include "soh/Enhancements/game-interactor/GameInteractor.h"
#include "soh/Enhancements/enhancementTypes.h"
#include "soh/ShipInit.hpp"
#include "soh/cvar_prefixes.h"

extern "C" {
#include "functions.h"
#include "src/overlays/actors/ovl_En_Rr/z_en_rr.h"
}

// Dark Link runs the player update function, so anything he does that has a global effect (voiding
// out, fall damage, the shared floor type statics) happens to Link instead.
static void RegisterDarkLinkFixes() {
    bool required =
        CVarGetInteger(CVAR_ENHANCEMENT("RandomizedEnemies"), ENEMY_RANDOMIZER_OFF) != ENEMY_RANDOMIZER_OFF ||
        CVarGetInteger(CVAR_REMOTE_CROWD_CONTROL("Enabled"), 0) || CVarGetInteger(CVAR_REMOTE_SAIL("Enabled"), 0);
    // prevent dark link from triggering a voidout
    COND_VB_SHOULD(VB_TRIGGER_VOIDOUT, required, {
        Actor* actor = va_arg(args, Actor*);

        if (*should == true && actor->category != ACTORCAT_PLAYER) {
            // Improvement opportunity: Play sfx
            *should = false;
            Actor_Kill(actor);
        }
    });

    // prevent dark link dealing fall damage to the player
    COND_VB_SHOULD(VB_RECIEVE_FALL_DAMAGE, required, {
        Actor* actor = va_arg(args, Actor*);

        if (actor->category != ACTORCAT_PLAYER) {
            *should = false;
        }
    });

    // prevent dark link from interfering with HESS/recoil/etc when at more than 100 away from him
    COND_VB_SHOULD(VB_TORCH2_HANDLE_CLANKING, required, {
        Actor* darkLink = va_arg(args, Actor*);

        if (darkLink->xzDistToPlayer > 100.0f) {
            *should = false;
        }
    });

    // prevent dark link from interfering with ice floors
    COND_VB_SHOULD(VB_SET_STATIC_PREV_FLOOR_TYPE, required, {
        Player* playerOrDarkLink = va_arg(args, Player*);

        if (playerOrDarkLink->actor.id != ACTOR_PLAYER) {
            *should = false;
        }
    });

    // prevent dark link from interfering with ice floors
    COND_VB_SHOULD(VB_SET_STATIC_FLOOR_TYPE, required, {
        Player* playerOrDarkLink = va_arg(args, Player*);

        if (playerOrDarkLink->actor.id != ACTOR_PLAYER) {
            *should = false;
        }
    });

    // prevent dark link from being grabbed by like likes and therefore grabbing the player
    COND_VB_SHOULD(VB_LIKE_LIKE_GRAB_PLAYER, required, {
        EnRr* likeLike = va_arg(args, EnRr*);

        if (!(likeLike->collider1.base.oc != NULL && likeLike->collider1.base.oc->category == ACTORCAT_PLAYER) &&
            !(likeLike->collider2.base.oc != NULL && likeLike->collider2.base.oc->category == ACTORCAT_PLAYER)) {
            *should = false;
        }
    });
}

static RegisterShipInitFunc initFunc(RegisterDarkLinkFixes,
                                     { CVAR_ENHANCEMENT("RandomizedEnemies"), CVAR_REMOTE_CROWD_CONTROL("Enabled"),
                                       CVAR_REMOTE_SAIL("Enabled") });
