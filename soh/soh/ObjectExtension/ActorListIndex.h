#ifndef ACTOR_LIST_INDEX_H
#define ACTOR_LIST_INDEX_H

#ifdef __cplusplus
extern "C" {
#include "z64actor.h"
#endif

int16_t GetActorListIndex(const Actor* actor);
void SetActorListIndex(const Actor* actor, int16_t index);

#ifdef __cplusplus
}
#endif

#endif // ACTOR_LIST_INDEX_H