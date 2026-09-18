#pragma once

#ifdef __cplusplus
extern "C" {
#include "z64actor.h"
#endif

int16_t GetActorListIndex(const Actor* actor);
void SetActorListIndex(const Actor* actor, int16_t index);

#ifdef __cplusplus
}
#endif
