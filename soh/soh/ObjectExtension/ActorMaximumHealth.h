#ifndef ACTOR_MAXIMUM_HEALTH_H
#define ACTOR_MAXIMUM_HEALTH_H

#ifdef __cplusplus
extern "C" {
#include "z64actor.h"
#endif

// Max health value for use with health bars, set on actor init
u8 GetActorMaximumHealth(const Actor* actor);
void SetActorMaximumHealth(const Actor* actor, u8 maximumHealth);

#ifdef __cplusplus
}
#endif

#endif // ACTOR_MAXIMUM_HEALTH_H