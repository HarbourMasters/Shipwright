#include "soh/Enhancements/game-interactor/GameInteractor.h"
#include "soh/Enhancements/game-interactor/GameInteractor_Hooks.h"
#include "soh/OTRGlobals.h"
#include "spdlog/spdlog.h"

extern "C" {
    #include "z64save.h"
    #include "macros.h"
    #include "variables.h"
    #include "functions.h"
    extern PlayState* gPlayState;
    extern SaveContext gSaveContext;
}

/**
 * This primarily handles speeding up the heavy block lifts (OGC and in the Fire Trial) but also handles skipping
 * the one point cutscene since the two options are so similar in what they do.
 */
void FasterHeavyBlockLift_Register() {
    REGISTER_VB_SHOULD(VB_PLAY_ONEPOINT_ACTOR_CS, {
        Actor* actor = va_arg(args, Actor*);

        if (
            actor->id == ACTOR_BG_HEAVY_BLOCK && 
            (CVarGetInteger(CVAR_ENHANCEMENT("FasterHeavyBlockLift"), 0) || CVarGetInteger(CVAR_ENHANCEMENT("TimeSavers.SkipCutscene.OnePoint"), IS_RANDO))
        ) {
            *should = false;
        }
    });

    REGISTER_VB_SHOULD(VB_FREEZE_LINK_FOR_BLOCK_THROW, {
        if (CVarGetInteger(CVAR_ENHANCEMENT("FasterHeavyBlockLift"), 0) || CVarGetInteger(CVAR_ENHANCEMENT("TimeSavers.SkipCutscene.OnePoint"), IS_RANDO)) {
            *should = false;
        }
    });

    REGISTER_VB_SHOULD(VB_PLAY_THROW_ANIMATION, {
        Player *player = GET_PLAYER(gPlayState);
        Actor* interactRangeActor = player->interactRangeActor;
        s32 interactActorId = interactRangeActor->id;
        LinkAnimationHeader* anim = va_arg(args, LinkAnimationHeader*);

        // Same actor is used for small and large silver rocks, use actor params to identify large ones
        bool isLargeSilverRock = interactActorId == ACTOR_EN_ISHI && interactRangeActor->params & 1 == 1;
        if (CVarGetInteger(CVAR_ENHANCEMENT("FasterHeavyBlockLift"), 0) && (isLargeSilverRock || interactActorId == ACTOR_BG_HEAVY_BLOCK)) {
            *should = false;
            LinkAnimation_PlayOnceSetSpeed(gPlayState, &player->skelAnime, anim, 5.0f);
        }
    });

    REGISTER_VB_SHOULD(VB_MOVE_THROWN_ACTOR, {
        if (CVarGetInteger(CVAR_ENHANCEMENT("FasterHeavyBlockLift"), 0)) {
            Actor* heldActor = va_arg(args, Actor*);

            heldActor->shape.rot.x -= 3510;
        }
    });
}
