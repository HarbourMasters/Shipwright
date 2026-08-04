#include "soh/Enhancements/game-interactor/GameInteractor.h"
#include "soh/ShipInit.hpp"

extern "C" {
#include "z64save.h"
#include "macros.h"
#include "variables.h"
#include "functions.h"
extern PlayState* gPlayState;
extern SaveContext gSaveContext;
}

#define CVAR_BLOCKLIFT_NAME CVAR_ENHANCEMENT("FasterHeavyBlockLift")
#define CVAR_BLOCKLIFT_VALUE CVarGetInteger(CVAR_BLOCKLIFT_NAME, 0)

#define CVAR_SKIP_CUTSCENE_NAME CVAR_ENHANCEMENT("TimeSavers.SkipCutscene.OnePoint")
#define CVAR_SKIP_CUTSCENE_VALUE CVarGetInteger(CVAR_SKIP_CUTSCENE_NAME, IS_RANDO)

/**
 * This primarily handles speeding up the heavy block lifts (OGC and in the Fire Trial) but also handles skipping
 * the one point cutscene since the two options are so similar in what they do.
 */
void RegisterFasterHeavyBlockLift() {
    COND_VB_SHOULD(VB_PLAY_ONEPOINT_ACTOR_CS, CVAR_BLOCKLIFT_VALUE || CVAR_SKIP_CUTSCENE_VALUE, {
        Actor* actor = va_arg(args, Actor*);

        if (actor->id == ACTOR_BG_HEAVY_BLOCK) {
            *should = false;
        }
    });

    COND_VB_SHOULD(VB_FREEZE_LINK_FOR_BLOCK_THROW, CVAR_BLOCKLIFT_VALUE || CVAR_SKIP_CUTSCENE_VALUE,
                   { *should = false; });

    COND_VB_SHOULD(VB_PLAY_THROW_ANIMATION, CVAR_BLOCKLIFT_VALUE, {
        Player* player = GET_PLAYER(gPlayState);
        Actor* interactRangeActor = player->interactRangeActor;
        s32 interactActorId = interactRangeActor->id;
        LinkAnimationHeader* anim = va_arg(args, LinkAnimationHeader*);

        // Same actor is used for small and large silver rocks, use actor params to identify large ones
        bool isLargeSilverRock = (interactActorId == ACTOR_EN_ISHI) && ((interactRangeActor->params & 1) == 1);
        if (isLargeSilverRock || interactActorId == ACTOR_BG_HEAVY_BLOCK) {
            *should = false;
            LinkAnimation_PlayOnceSetSpeed(gPlayState, &player->skelAnime, anim, 5.0f);
        }
    });

    COND_VB_SHOULD(VB_MOVE_THROWN_ACTOR, CVAR_BLOCKLIFT_VALUE, {
        Actor* heldActor = va_arg(args, Actor*);

        heldActor->shape.rot.x -= 3510;
    });
}

static RegisterShipInitFunc initFunc(RegisterFasterHeavyBlockLift, { CVAR_BLOCKLIFT_NAME, CVAR_SKIP_CUTSCENE_NAME });
