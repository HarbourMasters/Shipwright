#include "ActorAccessibility.h"
#include <map>
#include <random>

#include <functions.h>

// Begin actor-specific policy callbacks.

void accessible_en_ishi(AccessibleActor* actor) {
    ActorAccessibility_PlaySpecialSound(actor, NA_SE_EN_OCTAROCK_ROCK);
}

void ActorAccessibility_Init() {
    // ActorAccessibility_AddSupportedActor(ACTOR_EN_ISHI, NA_SE_EN_OCTAROCK_ROCK, 20, 1000, 1.5);
    ActorAccessibility_AddSupportedActor(ACTOR_EN_ISHI, "Rock",  accessible_en_ishi, 20, 1000, 1.5);
}
