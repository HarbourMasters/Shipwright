#ifndef ACTOR_LIST_INDEX_H
#define ACTOR_LIST_INDEX_H

#include <libultraship/libultraship.h>
#include <soh/ActorExtension/ActorExtension.h>

#ifdef __cplusplus
extern "C" {
#include "z64actor.h"
#endif

extern ActorExtensionId actorListIndexActorExt;
extern int16_t currentActorListIndex;

int16_t GetActorListIndex(Actor* actor);
void SetActorListIndex(Actor* actor, int16_t index);

#ifdef __cplusplus
}
#endif

#endif // ACTOR_LIST_INDEX_H