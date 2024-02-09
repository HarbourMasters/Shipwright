#include "SetTransitionActorList.h"

TransitionActorEntry* SetTransitionActorList::GetPointer() {
    return transitionActorList.data();
}

size_t SetTransitionActorList::GetPointerSize() {
	return transitionActorList.size() * sizeof(TransitionActorEntry);
}
