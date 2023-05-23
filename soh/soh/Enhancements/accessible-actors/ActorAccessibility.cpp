#include "ActorAccessibility.h"
#include <map>
typedef void (*ActorAccessibilityCallback)(Actor*, AccessibleActorState*);

typedef struct {
    ActorAccessibilityCallback callback;//If set, it will be called once every n frames. If null, then sfx will be played once every n frames.
    int n;//How often to run the callback in frames.

}ActorAccessibilityPolicy;

typedef std::map<s16, ActorAccessibilityPolicy> SupportedActors_t;
typedef std::map<Actor*, AccessibleActorState> TrackedActors_t;

SupportedActors_t SupportedActors;
TrackedActors_t TrackedActors;

void ActorAccessibility_AddSupportedActor(s16 type, ActorAccessibilityCallback callback, int frames = 25) {
    ActorAccessibilityPolicy policy;
    policy.callback = callback;
    policy.n = frames;

    SupportedActors[type] = policy;

}
 ActorAccessibilityPolicy* ActorAccessibility_GetPolicyForActor(s16 type) {
    SupportedActors_t::iterator i = SupportedActors.find(type);
     if (i == SupportedActors.end())
         return NULL;
     return &(i->second);

 }
    void ActorAccessibility_TrackNewActor(Actor* actor) {
        // Don't track actors for which no accessibility policy has been configured.
        ActorAccessibilityPolicy* policy = ActorAccessibility_GetPolicyForActor(actor->id);
        if (policy == NULL)
            return;
        AccessibleActorState state;
        memset(&state, 0, sizeof(AccessibleActorState));
        state.frameCount = policy->n;

        TrackedActors[actor] = state;
    }
    AccessibleActorState* ActorAccessibility_GetTrackedActorState(Actor* actor) {
        TrackedActors_t::iterator i = TrackedActors.find(actor);
        if (i == TrackedActors.end())
            return NULL;
        return &(i->second);

    }
    void ActorAccessibility_RemoveTrackedActor(Actor* actor) {
        TrackedActors_t::iterator i = TrackedActors.find(actor);
        if (i == TrackedActors.end())
            return;
        TrackedActors.erase(i);

    }
    void ActorAccessibility_RunAccessibilityForActor(Actor* actor) {
        AccessibleActorState* state = ActorAccessibility_GetTrackedActorState(actor);
        if (state == NULL)
            return;
        ActorAccessibilityPolicy* policy = ActorAccessibility_GetPolicyForActor(actor->id);
        if (policy == NULL)
            return;
        state->frameCount--;
        if (state->frameCount > 0)
            return;
        state->frameCount = policy->n;
        if (policy->callback != NULL)
            policy->callback(actor, state);

    }
    void ActorAccessibility_RunAccessibilityForAllActors() {
        for (TrackedActors_t::iterator i = TrackedActors.begin(); i != TrackedActors.end(); i++)
            ActorAccessibility_RunAccessibilityForActor(i->first);

    }
    //Begin actor-specific policy callbacks.
    void accessible_en_ishi(Actor* actor, AccessibleActorState* state) {
        // SoundSource_PlaySfxAtFixedWorldPos(play, &this->actor.world.pos, 20, NA_SE_EN_OCTAROCK_ROCK);
        actor->sfx = NA_SE_EN_OCTAROCK_ROCK;
    }

    void ActorAccessibility_Init() {
        ActorAccessibility_AddSupportedActor(ACTOR_EN_ISHI, accessible_en_ishi);
    }
